#!/usr/bin/env python3
"""Build and run a verification unit.

usage: verify.py UNIT [harness options...]      e.g. verify.py d_a_mtoge -n 2000 -rec build/verify/rec

Steps: mkimage (once), mkunit (unit.c from build/gen), compile unit.c + the candidate sources +
the harness with exact FP flags, run. Harness options: -n N generated inputs per function,
-seed S, -rec DIR recorded inputs (DIR/ADDR/*.tap), -only ADDR, -v / -v -v.
Progress (2026-10-27, on by default): the harness prints a `[progress] ...` line to stderr every 10 s and a
`[progress] STUCK ...` line when one input runs longer than 60 s, and keeps a progress file in the unit's directory +
/progress (tools/verify/progress.py reads them); -noprogress (or WWHD_VERIFY_PROGRESS=0) turns all of it off,
-progress-interval S, -stuck S and -hanglimit S (the per-run wall-clock hang limit, default 120) set the rest. stdout
is the same either way.
Result cache (2026-10-27, vcache.py): rows whose key (compiled code, spec, tools, gen, options) is unchanged come from
$WWHD_VERIFY_SCRATCH/vcache; --no-cache (WWHD_VERIFY_CACHE=0) switches it off, --archive (WWHD_VERIFY_ARCHIVE=1) is the
mode for final/archive gates and never uses a cached row.
Unit split (2026-11-02): -unitsplit N / WWHD_UNIT_SPLIT=N / spec `unitsplit N` compile the generated unit.c as N
translation units (memory per clang process); automatic only above WWHD_UNIT_SPLIT_AUTO_MB when that is set (default 0 = never). See UNIT_SPLIT_AUTO_MB.
A unit file line `rec DIR` (path relative to the repository root, e.g. tools/verify/recordings/d_grass) adds
those recordings to every run of the unit; an explicit -rec on the command line takes precedence.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import specfile  # noqa: E402  (unit-spec `include` lines)
import progress  # noqa: E402  (progress lines / files, 2026-10-27)
import vcache  # noqa: E402  (result cache for re-checks, 2026-10-27)

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

# One result row of the table verify prints (harness.cpp and the sharded merge below print the same
# layout: "%08X  %-40.40s %10s %10s %8s  %s"). The function column is fixed width: 40 characters, padded
# or cut. Displayed names can contain spaces (an overload cast such as "(void (X::*)(int, int)) &X::f"),
# so the name is the whole 40-character field, not one token. Callers strip the padding.
# Groups: addr, name field, gen pass/n, rec pass/n, coverage hit/branches, first difference.
RESULT_LINE = re.compile(r"^([0-9A-F]{8})  (.{40})\s+(\d+)/(\d+)\s+(\d+)/(\d+)(?:\s*\(-?\d+\))?\s+(\d+)/(\d+)\s+(.*)$")
# -ffp-exception-behavior=maytrap (2026-10-24): clang otherwise assumes no signalling NaNs and may turn a compare and
# branch into fmaxnm/fminnm, which treat an sNaN unlike the PowerPC compare (d_a_race_item CreateInit, d_attention
# setLList). The harness feeds sNaN bit patterns, so reference and candidate both keep strict compare semantics.
# -DGABI_GUEST_GUARD=1 (2026-10-27, AGL#2): be<>/gptr<> accessed through a host object (a host-local `gptr<T> x;`) end the
# input with a diagnostic (gabi.h be<>::addr, harness.cpp gmem_host_object: a candidate-only abort, a FAIL row; exit
# status 86 outside a candidate run) instead of writing an
# arbitrary guest address. In this list so every verification compile (verify.py, mutate.py) has it and the flag stamp
# rebuilds old objects; objcmp.py leaves it out (a check, not behaviour: object identity against older trees).
# -fno-finite-loops: C++ (and C11, for loops whose condition is not a constant)
# lets the compiler assume every loop without side effects terminates (forward-progress rule), so clang deletes a
# side-effect-free loop that never ends: a mutant that hangs the guest (e.g. a dropped `last = link->mNext` in a list
# walk) returned normally and survived. With the flag such a loop stays a loop, so the run ends by the CPU time budget
# (a candidate-only abort) or the mutant's per-run timeout: a non-termination record instead of an invisible hang.
# Loops that terminate run exactly as before. clang >= 12; C and C++. Self-test finiteloops_test.py.
FP = ["-ffp-contract=off", "-fno-fast-math", "-ffp-exception-behavior=maytrap", "-fno-strict-aliasing", "-fwrapv", "-DGABI_GUEST_GUARD=1",
      "-fno-finite-loops"]
# -guardstrict: the host-index wrap diagnostic build (gabi.h GABI_GUEST_GUARD=2,
# harness gmem_host_wrap): every be<>/gptr<> access that reaches its guest address only through ea()'s 32-bit wrap is
# reported on stderr (`guardstrict: ...`); rows are unchanged. Its objects and executable live in <unit dir>/guardstrict,
# so the normal build is untouched.
GUARD_STRICT = False


def build_flags():
    return [("-DGABI_GUEST_GUARD=2" if GUARD_STRICT and f == "-DGABI_GUEST_GUARD=1" else f) for f in FP]


def sh(cmd):
    r = subprocess.run(cmd, cwd=ROOT)
    if r.returncode:
        sys.exit(r.returncode)


def prepare_shared():
    """Shared tables every unit build reads (image, indirect-call live-ins, original stack frames).
    verify_all.py calls this ONCE before its parallel jobs and sets WWHD_SHARED_READY=1, so parallel
    builds never regenerate (and race on) these files; a single verify.py run regenerates them when stale."""
    image = os.path.join(ROOT, "build", "verify", "image.bin")
    if not os.path.exists(image):
        sh([sys.executable, os.path.join(HERE, "mkimage.py"), "game/code/cking.rpx", image])
    import assets  # model data headers generated from the game (build/verify/include/assets, never committed)
    try:
        man = assets.manifest()
        for h in man:
            assets.generate(h, man)
    except assets.AssetError as e:
        sys.exit("assets: %s" % e)
    livein = os.path.join(ROOT, "build", "verify", "livein.tsv")  # indirect-call live-in table (livein_all.py)
    if not os.path.exists(livein) or os.path.getmtime(livein) < os.path.getmtime(os.path.join(ROOT, "build", "gen")):
        sh([sys.executable, os.path.join(HERE, "livein_all.py"), livein])
    frames = os.path.join(ROOT, "build", "verify", "frames.tsv")  # original stack frames (frames_all.py; gabi automatic native frames)
    if not os.path.exists(frames) or os.path.getmtime(frames) < os.path.getmtime(os.path.join(ROOT, "build", "gen")) \
            or os.path.getmtime(frames) < os.path.getmtime(os.path.join(HERE, "frames_all.py")):
        sh([sys.executable, os.path.join(HERE, "frames_all.py"), frames])
    statics = os.path.join(ROOT, "build", "verify", "statics.tsv")  # static-object boundaries (-observe-pointees; mkstatics.py)
    if not os.path.exists(statics) or os.path.getmtime(statics) < os.path.getmtime(os.path.join(HERE, "mkstatics.py")):
        sh([sys.executable, os.path.join(HERE, "mkstatics.py"), "game/code/cking.rpx", statics])


# Linked into every harness: the shipping port's GX2 surface formula behind the `mocksurfacesize` directive
# (src/surface_calc.cpp + the Cemu-derived sources it uses, MPL-2.0), compiled with the port's Cemu flags.
CEMU = "runtime/third_party/cemu"
SUPPORT_SRCS = ["tools/verify/src/surface_calc.cpp", CEMU + "/gx2_surface_calc.cpp",
                CEMU + "/Cafe/HW/Latte/LatteAddrLib/LatteAddrLib.cpp", CEMU + "/Cafe/HW/Latte/LatteAddrLib/LatteAddrLib_Coord.cpp"]


def support_objs():
    """The SUPPORT_SRCS objects, built once into build/verify/_support (shared by all units; atomic replace, so
    parallel builds are safe)."""
    d = os.path.join(ROOT, "build", "verify", "_support")
    os.makedirs(d, exist_ok=True)
    cc = ["clang++", "-std=c++20", "-O2", "-w", "-include", CEMU + "/cemu_shim.h", "-I" + CEMU, "-I" + CEMU + "/Cafe",
          "-Iruntime/third_party/fmt/include", "-DFMT_HEADER_ONLY"]
    deps = max(os.path.getmtime(os.path.join(ROOT, CEMU, f)) for f in ("cemu_shim.h", "gx2_surface_calc.h"))
    objs = []
    for src in SUPPORT_SRCS:
        obj = os.path.join(d, os.path.basename(src) + ".o")
        if not os.path.exists(obj) or os.path.getmtime(obj) < max(os.path.getmtime(os.path.join(ROOT, src)), deps):
            tmp = "%s.%d.tmp" % (obj, os.getpid())
            sh(cc + ["-c", src, "-o", tmp])
            os.replace(tmp, obj)
        objs.append(obj)
    return objs


# WWHD_VERIFY_SCRATCH=DIR (2026-10-26, campaigns off the SSD): each unit's build/output directory (unit.c, objects, the
# verify executable, glue/, spec.expanded.txt; mutate.py's mutant files unless --scratch says otherwise) lives in
# DIR/<tree key>/<unit> instead of build/verify/<unit>, e.g. on the RAM disk $RAMDISK. The tree key (worktree
# name + hash of its path) keeps worktrees apart. DIR set but missing or not writable: the tool stops with an error
# naming the variable and the path and writes nothing (2026-11-02: the silent SSD fallback wrote ~211 MB to the SSD;
# the folder is never created, so a typo cannot leave stray folders). Less than WWHD_VERIFY_SCRATCH_MIN_FREE
# bytes free (default 1 GB): build/verify/<unit> as before, with one stderr note per process. The shared tables
# (image.bin, livein.tsv, frames.tsv, _support) and the logs callers keep (verify_all.log) stay in build/verify.
VERIFY_SCRATCH_FLOOR = 1 << 30
_unit_dirs = {}  # (root, unit) -> directory: one decision per process, so a build and its run agree
_scratch_noted = set()


def scratch_note(key, msg):
    if key not in _scratch_noted:
        _scratch_noted.add(key)
        print(msg, file=sys.stderr)


SCRATCH_ENVS = ("WWHD_VERIFY_SCRATCH", "WWHD_MUT_SCRATCH")


def scratch_dir_error(var, path):
    """The error text for a scratch folder that is missing or not writable, or None when it is usable."""
    if not os.path.isdir(path):
        return "%s=%s: folder does not exist (create it, or unset %s to build in build/verify); nothing written" % (var, path, var)
    if not os.access(path, os.W_OK | os.X_OK):
        return "%s=%s: folder is not writable (or unset %s to build in build/verify); nothing written" % (var, path, var)
    return None


def require_scratch_dir(var, path, tool="verify"):
    """Stop (exit 2) when a requested scratch folder is missing or not writable; never create it, never fall back."""
    err = scratch_dir_error(var, path)
    if err:
        print("%s: error: %s" % (tool, err), file=sys.stderr)
        raise SystemExit(2)


def check_scratch_env(tool="verify"):
    """Every tool's first step: each scratch variable that is set must name an existing, writable folder."""
    for var in SCRATCH_ENVS:
        if os.environ.get(var):
            require_scratch_dir(var, os.environ[var], tool)


def verify_scratch_base(root=ROOT):
    """The scratch base of this tree under WWHD_VERIFY_SCRATCH (DIR/<tree key>, created), or None: unset or below the
    free-space floor (one stderr note). DIR missing or not writable: exit 2 with an error (require_scratch_dir). The one place the verify-side scratch is resolved."""
    want = os.environ.get("WWHD_VERIFY_SCRATCH")
    if not want:
        return None
    require_scratch_dir("WWHD_VERIFY_SCRATCH", want)
    floor = int(os.environ.get("WWHD_VERIFY_SCRATCH_MIN_FREE") or VERIFY_SCRATCH_FLOOR)
    try:
        import shutil
        free = shutil.disk_usage(want).free
    except OSError:
        free = 0
    if free < floor:
        scratch_note("full", "verify: WWHD_VERIFY_SCRATCH %s has %d MB free (< %d MB); unit builds stay in build/verify"
                     % (want, free >> 20, floor >> 20))
        return None
    import hashlib
    real = os.path.realpath(root)
    base = os.path.join(want, "%s-%s" % (os.path.basename(real), hashlib.sha1(real.encode()).hexdigest()[:8]))
    try:
        os.makedirs(base, exist_ok=True)
    except OSError as e:
        print("verify: error: WWHD_VERIFY_SCRATCH=%s: cannot create %s (%s); nothing written" % (want, base, e), file=sys.stderr)
        raise SystemExit(2)
    return base


def unit_dir(unit, root=ROOT):
    """The build/output directory of a unit: build/verify/<unit>, or under WWHD_VERIFY_SCRATCH (verify_scratch_base)."""
    key = (os.path.realpath(root), unit)
    if key not in _unit_dirs:
        base = verify_scratch_base(root)
        _unit_dirs[key] = os.path.join(base, unit) if base else os.path.join(root, "build", "verify", unit)
    return _unit_dirs[key]


def unit_dirs_all(unit, root=ROOT):
    """Every directory a unit's build may be in (build/verify/<unit> and its WWHD_VERIFY_SCRATCH copy): for cleanups"""
    res = [os.path.join(root, "build", "verify", unit)]
    want = os.environ.get("WWHD_VERIFY_SCRATCH")
    if want:
        import hashlib
        real = os.path.realpath(root)
        res.append(os.path.join(want, "%s-%s" % (os.path.basename(real), hashlib.sha1(real.encode()).hexdigest()[:8]), unit))
    return res


# Unit split (2026-11-02, J3D start-up profile: a 54 MB unit.c took 5.6-6.2 GB in one clang -cc1): mkunit.py also writes
# unit.c as N translation units (unit_shared.h + unit.p0.c .. unit.pN-1.c, listed in <unit dir>/unit.parts) and the build
# compiles those instead of unit.c: the same compiler flags per TU, the same link, the same rows. N comes from, first one
# wins: the option -unitsplit N, WWHD_UNIT_SPLIT=N, the spec directive `unitsplit N` (the harness ignores it); N = 1 means
# no split. Otherwise automatic only when WWHD_UNIT_SPLIT_AUTO_MB is set above 0 (default 0 = never;
# strictly opt-in, so no existing unit's build changes): a unit.c larger than that is split into
# ceil(size / (threshold/2)) parts. The parts
# compile one after the other (memory); -buildj N compiles them in parallel. Self-test unitsplit_test.py.
UNIT_SPLIT_AUTO_MB = 0


def unit_split(unit, args=None):
    """the requested split: ("n", N) from -unitsplit / WWHD_UNIT_SPLIT / the spec directive, else ("auto", BYTES) (0:
    never). Removes -unitsplit N from args."""
    if args is not None and "-unitsplit" in args:
        i = args.index("-unitsplit")
        if i + 1 >= len(args) or not args[i + 1].isdigit() or int(args[i + 1]) < 1:
            sys.exit("verify.py: -unitsplit takes a positive number (1: no split)")
        n = int(args[i + 1])
        del args[i:i + 2]
        return ("n", n)
    if os.environ.get("WWHD_UNIT_SPLIT"):
        if not os.environ["WWHD_UNIT_SPLIT"].isdigit() or int(os.environ["WWHD_UNIT_SPLIT"]) < 1:
            sys.exit("verify.py: WWHD_UNIT_SPLIT takes a positive number (1: no split)")
        return ("n", int(os.environ["WWHD_UNIT_SPLIT"]))
    for line in specfile.expanded_lines(os.path.join(HERE, "units", unit + ".txt")):
        f = line.split("#")[0].split()
        if f and f[0] == "unitsplit":
            if len(f) != 2 or not f[1].isdigit() or int(f[1]) < 1:
                sys.exit("verify.py: spec line `%s`: unitsplit takes one positive number (1: no split)" % line.strip())
            return ("n", int(f[1]))
    mb = os.environ.get("WWHD_UNIT_SPLIT_AUTO_MB", str(UNIT_SPLIT_AUTO_MB))
    if not mb.isdigit():
        sys.exit("verify.py: WWHD_UNIT_SPLIT_AUTO_MB takes a number of MB (0: never split automatically)")
    return ("auto", int(mb) << 20)


def unit_parts(out):
    """the generated sources of a unit's original code in <unit dir> out: the unit.parts manifest's files (a split
    build), else unit.c"""
    m = os.path.join(out, "unit.parts")
    if os.path.exists(m):
        return [os.path.join(out, x) for x in open(m).read().split()]
    return [os.path.join(out, "unit.c")]


def build(unit, srcopt="O2", only=None, jobs=1, split=None):
    """srcopt: optimisation of the unit's own `src` sources (default O2); the recompiled originals, glue and the
    harness always build at O2. A non-default level uses separate object files (name suffix), never the O2 cache.
    only: apply srcopt to that one `src` file and keep every other source at O2 (mutate.py's build: the mutated TU
    at O1 in an otherwise O2 link, so this is its matched control). split: unit_split()'s request (default: the
    environment / spec / automatic one)."""
    out = unit_dir(unit)
    if os.environ.get("WWHD_SHARED_READY") != "1":
        prepare_shared()
    kind, n = split or unit_split(unit)
    sh([sys.executable, os.path.join(HERE, "mkunit.py"), unit, "--root", ROOT, "--out", out] +
       (["--split", str(n)] if kind == "n" else ["--split-auto", str(n)]))
    uparts = unit_parts(out)
    srcs = []
    for line in specfile.expanded_lines(os.path.join(HERE, "units", unit + ".txt")):
        f = line.split("#")[0].split()
        if f and f[0] == "src":
            srcs += f[1:]
    inc = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include", "-Ibuild/gen", "-Ibuild/verify/include"]
    fp = build_flags()
    odir = os.path.join(out, "guardstrict") if GUARD_STRICT else out  # objects and executable (-guardstrict: own directory)
    os.makedirs(odir, exist_ok=True)
    # objects built with other compile flags are stale: the cache below only compares source times
    stamp, flags = os.path.join(odir, "fpflags.stamp"), " ".join(fp)
    if not os.path.exists(stamp) or open(stamp).read() != flags:
        for f in os.listdir(odir):
            if f.endswith(".o"):
                os.remove(os.path.join(odir, f))
        open(stamp, "w").write(flags)
    objs, cmds = [], []
    for src, lang in [(p, "c") for p in uparts] + [("runtime/src/espresso_fp.c", "c"), ("tools/verify/src/harness.cpp", "c++")] + \
            [(s, "c++src") for s in srcs]:
        opt = "-" + srcopt if lang == "c++src" and (only is None or src == only) else "-O2"
        obj = os.path.join(odir, os.path.basename(src) + ("" if opt == "-O2" else "." + opt[1:]) + ".o")
        objs.append(obj)
        if os.path.exists(obj) and os.path.getmtime(obj) > max(os.path.getmtime(os.path.join(ROOT, src)), newest_header()):
            continue
        src = glue(src, out)
        if lang == "c":
            cmd = ["clang", "-std=c11", "-O2", "-w"] + fp + inc + ["-c", src, "-o", obj]
        else:
            cmd = ["clang++", "-std=c++20", opt, "-Wall", "-Wno-invalid-offsetof", "-Wno-unused-function"] + fp + inc + ["-c", src, "-o", obj]
        cmds.append(cmd)
    if jobs > 1 and len(cmds) > 1:  # -buildj N: compile the unit's objects in parallel (same commands, same objects)
        import concurrent.futures
        with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
            rcs = list(ex.map(lambda c: subprocess.run(c, cwd=ROOT).returncode, cmds))
        bad = [rc for rc in rcs if rc]
        if bad:
            sys.exit(bad[0])
    else:
        for cmd in cmds:
            sh(cmd)
    if only is not None and only not in srcs:
        sys.exit("verify.py: -srcopt-only %s is not a `src` of unit %s" % (only, unit))
    exe = os.path.join(odir, "verify" if srcopt == "O2" else "verify." + srcopt + ("" if only is None else "-" + os.path.basename(only)))
    sup = support_objs()
    sh(["clang++", "-o", exe] + objs + sup + ["-Wl,-map," + exe + ".map"])  # the link map: vcache.py's symbol owners
    nsrc, nfix = len(srcs), len(uparts) + 2  # unit.c (or its parts), espresso_fp, harness
    BUILD_INFO.update(exe=exe, linkmap=exe + ".map", src_objs=objs[nfix:nfix + nsrc],
                      fixed_objs=objs[:nfix] + [os.path.join(ROOT, o) for o in sup], flags=fp + inc, srcopt=srcopt, only=only)
    return exe


BUILD_INFO = {}  # the last build's objects and flags (vcache.py keys)


def unit_rec(unit):
    """The `rec DIR` directive of a unit file (absolute path), or None."""
    for line in specfile.expanded_lines(os.path.join(HERE, "units", unit + ".txt")):
        f = line.split("#")[0].split()
        if len(f) == 2 and f[0] == "rec":
            d = os.path.join(ROOT, f[1])
            if not os.path.isdir(d):
                # recordings hold game memory and are not distributed: record them locally (README, "Recorded")
                print("verify: no recordings at %s (generated inputs only)" % f[1], file=sys.stderr)
                return None
            return d
    return None


def glue(src, out):
    """GameCube-style sources (a `/* WWHD glue` block) are compiled through autoglue.py"""
    text = open(os.path.join(ROOT, src)).read()
    if "/* WWHD glue" not in text:
        return src
    import autoglue
    try:
        res = autoglue.transform(text, src)
    except autoglue.GlueError as e:
        sys.exit("autoglue: %s" % e)
    path = os.path.join(out, "glue", os.path.basename(src))
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "w").write(res)
    return path


def newest_header():
    t = os.path.getmtime(os.path.join(HERE, "autoglue.py"))
    for d in ("tools/verify/include", "tools/verify/src", "wwhd_src/include", "runtime/include", "build/verify/include"):
        for dp, _, fs in os.walk(os.path.join(ROOT, d)):
            for f in fs:
                t = max(t, os.path.getmtime(os.path.join(dp, f)))
    return t


# Units whose runs need ~30-40 GB of RAM. A machine-wide counting semaphore limits how many run at once,
# across all clones and worktrees: WWHD_HEAVY_SLOTS slots (default 3, for a
# 128 GB machine), each an flock on its own file. Slot 0 is WWHD_HEAVY_LOCK itself (default
# /tmp/wwhd-verify-heavy.lock, the old single lock, so older checkouts still count against slot 0);
# slot k > 0 is that path + ".k". WWHD_HEAVY_SLOTS=1 restores the old one-at-a-time behaviour.
HEAVY = {"d_a_sail", "d_a_bwdg"}
HEAVY_LOCK_PATH = os.environ.get("WWHD_HEAVY_LOCK", "/tmp/wwhd-verify-heavy.lock")
HEAVY_SLOTS = max(1, int(os.environ.get("WWHD_HEAVY_SLOTS", "3")))


def heavy_slot_paths():
    return [HEAVY_LOCK_PATH] + ["%s.%d" % (HEAVY_LOCK_PATH, k) for k in range(1, HEAVY_SLOTS)]


class heavy_lock:
    """Hold one machine-wide HEAVY slot while running a heavy unit (no-op for other units)."""

    def __init__(self, unit):
        self.unit, self.f = unit, None

    def _try(self):
        import fcntl
        for pth in heavy_slot_paths():
            f = open(pth, "a")
            try:
                fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
                return f
            except OSError:
                f.close()
        return None

    def __enter__(self):
        if self.unit in HEAVY:
            import time
            self.f = self._try()
            if not self.f:
                print("verify: waiting for one of %d machine-wide HEAVY slots (%s) for %s"
                      % (HEAVY_SLOTS, HEAVY_LOCK_PATH, self.unit), file=sys.stderr, flush=True)
                while not self.f:
                    time.sleep(10)
                    self.f = self._try()
        return self

    def __exit__(self, *exc):
        if self.f:
            self.f.close()  # releases the flock
        return False


def shard_count(unit, args):
    """-shards K on the command line, else WWHD_SHARDS, else WWHD_HEAVY_SLOTS for HEAVY units, else 1.
    Options that print per-input detail (-trace, -watch) or select inputs themselves (-range, -list) run unsharded; -v
    runs unsharded unless -shards K is given explicitly (2026-10-27: the merge then prints each function's per-input lines
    shard by shard and, with -v -v, its uncovered blocks, as an unsharded run does)."""
    k = None
    if "-shards" in args:
        i = args.index("-shards")
        k = int(args[i + 1])
        del args[i:i + 2]
    if any(a in args for a in ("-trace", "-watch", "-range", "-list", "-shardout")) or ("-v" in args and k is None):
        return 1
    if k is None and os.environ.get("WWHD_SHARDS"):
        k = int(os.environ["WWHD_SHARDS"])
    if k is None:
        k = HEAVY_SLOTS if unit in HEAVY else 1
    return max(1, k)


def shard_problems(outs, codes, ranges, expect=None):
    """why a sharded run's results are incomplete (2026-10-27, : under load 110-160 a shard of lib2_nw_snd_6
    died and the merge printed its last 28 functions with 7500 of 10000 inputs): every shard must exit 0/1, end with its
    `@SHARDEND N STOP` marker, and report every function the other shards report (unless it stopped early on purpose,
    -stopfirst). expect: the functions every shard must report (the result cache's misses), else those any shard reports.
    Returns a list of messages; empty = complete (then the result cache may store rows, and cache hits are printed)."""
    import signal as _signal
    probs, seen, stopped = [], [], False
    for i, out in enumerate(outs):
        lines = (out or "").splitlines()
        addrs = [ln.split()[1] for ln in lines if ln.startswith("@SHARD ")]
        seen.append(addrs)
        end = [ln for ln in lines if ln.startswith("@SHARDEND ")]
        lo, hi = ranges[i]
        where = "shard %d (inputs %d:%d)" % (i, lo, hi)
        c = codes[i]
        if c not in (0, 1):
            why = "killed by %s" % _signal.Signals(-c).name if c is not None and c < 0 and -c in _signal.valid_signals() \
                else "exit %s" % c
            probs.append("%s %s after %d function row(s)%s" % (where, why, len(addrs),
                                                                 " (last %s)" % addrs[-1] if addrs else ""))
        elif not end:
            probs.append("%s exit %s but its output has no end marker (cut short after %d function row(s))" % (where, c, len(addrs)))
        else:
            f = end[-1].split()
            if int(f[1]) != len(addrs):
                probs.append("%s: end marker says %s function row(s), %d read" % (where, f[1], len(addrs)))
            stopped |= f[2:3] == ["1"]
    if not probs and not stopped:
        allf = set(expect) if expect is not None else set().union(*map(set, seen)) if seen else set()
        for i, addrs in enumerate(seen):
            miss = sorted(allf - set(addrs))
            if miss:
                probs.append("shard %d (inputs %d:%d) has no row for %d function(s) the other shards report (first %s)" % (
                    i, ranges[i][0], ranges[i][1], len(miss), miss[0]))
    return probs


HEADER = "%-8s  %-40s %10s %10s %8s  %s" % ("addr", "function", "generated", "recorded", "coverage", "first difference")


class MarkedStderr:
    """reads a harness's stderr under WWHD_VERIFY_CACHE_MARKS=1: echoes every line except the `@VC< ADDR` / `@VC> ADDR`
    marks and collects each function's notes (not `[progress]` lines) for the result cache"""

    def __init__(self, stream):
        import threading
        self.stream, self.notes, self.done = stream, {}, set()
        self.t = threading.Thread(target=self.run, daemon=True)
        self.t.start()

    def run(self):
        cur = None
        for line in iter(self.stream.readline, b""):
            m = vcache.MARK.match(line.decode("latin-1").rstrip("\n"))
            if m:
                a = int(m.group(2), 16)
                if m.group(1) == "<":
                    cur = a
                    self.notes[a] = []
                else:
                    self.done.add(a)
                    cur = None
                continue
            sys.stderr.buffer.write(line)
            sys.stderr.flush()
            if cur is not None and not line.startswith(b"[progress]"):
                self.notes[cur].append(line.decode("latin-1"))

    def join(self):
        self.t.join()


def replay(entry, addr, key):
    """a cached function's notes on stderr (its stdout block is printed by the caller)"""
    for line in entry["stderr"]:
        sys.stderr.buffer.write(line.encode("latin-1"))
    if entry.get("resume"):
        print("resume: %08X from %s (key %s, %d part(s))" % (addr, entry["resume"], key[:16], entry["parts"]),
              file=sys.stderr, flush=True)
    else:
        print("vcache: %08X cached (key %s)" % (addr, key[:16]), file=sys.stderr, flush=True)


def gen_n(args):
    return int(args[args.index("-n") + 1]) if "-n" in args else 1000


def resume_store_part(sess, keys, addr, chunk, err, i, k, lo, hi, dsifile):
    """--resume: store one harness process's part of a finished function: its stdout lines between the marks, its stderr
    notes (once the stderr reader has seen the function's end mark) and its -dsiout records (once the harness wrote the
    `# ADDR done` record). Anything not yet complete is not stored: the function then simply runs again on a resume."""
    import time
    if addr not in keys:
        return
    t = time.time()
    while addr not in err.done and time.time() - t < 5:
        time.sleep(0.01)
    if addr not in err.done:
        return
    dsi = None
    if dsifile:
        try:
            per, done = split_dsi(open(dsifile).read())
        except OSError:
            return
        if addr not in done:
            return
        dsi = per.get(addr, [])
    sess.put_part(keys[addr], addr, i, k, lo, hi, chunk, list(err.notes.get(addr, [])), dsi)


def resume_stream(stream, err, cache, i, k, lo, hi, dsifile):
    """a shard's stdout read line by line (--resume): each function's lines between its marks are stored as that shard's
    part as soon as the end mark arrives; returns the whole output as p.stdout.read() would"""
    sess, keys = cache[0], cache[1]
    lines, cur, part = [], None, []
    for raw in iter(stream.readline, b""):
        line = raw.decode()
        lines.append(line)
        m = vcache.MARK.match(line.rstrip("\n"))
        if m:
            a = int(m.group(2), 16)
            if m.group(1) == "<":
                cur, part = a, []
            elif cur == a:
                resume_store_part(sess, keys, a, "".join(part), err, i, k, lo, hi, dsifile)
                cur = None
            continue
        if cur is not None:
            part.append(line)
    return "".join(lines)


def without_only(args):
    out, i = [], 0
    while i < len(args):
        if args[i] == "-only":
            i += 2
            continue
        out.append(args[i])
        i += 1
    return out


def split_dsi(text):
    """a -dsiout file's lines per function ({addr: [lines]}) and the functions with a `# ADDR done` record"""
    per, done = {}, set()
    for line in text.splitlines(True):
        f = line.split()
        if not f:
            continue
        tok = f[1] if f[0] == "#" and len(f) > 1 else f[0]
        if re.match(r"^[0-9A-F]{8}$", tok):
            per.setdefault(int(tok, 16), []).append(line)
            if f[0] == "#" and len(f) > 2 and f[2] == "done":
                done.add(int(tok, 16))
    return per, done


P0_MARK = "[original page-0 fault on this input: no evidence]"  # harness.cpp, cascade.py


def merge_shard_outputs(outs, verbose=0):
    """the shards' -shardout outputs (in shard order) parsed and summed per function: (funcs {addr: merged counts},
    order (first appearance), extra lines). Counts are summed, coverage united, the first difference taken from the
    lowest input index; -v: each function's per-input lines in shard order"""
    funcs, order, extra = {}, [], []
    for out in outs:
        pending = []  # -v: a shard's per-input lines precede its @SHARD line for that function
        for line in (out or "").splitlines():
            if line.startswith("@SHARDEND ") or vcache.MARK.match(line):
                continue
            if verbose and not line.startswith("@SHARD "):
                pending.append(line)
                continue
            if line.startswith("@SHARD "):
                head, name, first = line[7:].split("|", 2)
                f = head.split()
                addr = int(f[0], 16)
                gp, gn, rp, rn, rbad, ab, fidx, nb = map(int, f[1:9])
                cov = set() if f[9] == "-" else set(map(int, f[9].split(",")))
                if addr not in funcs:
                    funcs[addr] = dict(name=name, gp=0, gn=0, rp=0, rn=0, rbad=0, ab=0, nb=nb, cov=set(), first="", fidx=None, p0=False, shards=0)
                    order.append(addr)
                m = funcs[addr]
                m.setdefault("lines", []).extend(pending)
                pending = []
                m["gp"] += gp; m["gn"] += gn; m["rp"] += rp; m["rn"] += rn; m["rbad"] += rbad; m["ab"] += ab
                m["cov"] |= cov
                m["shards"] += 1
                # the lowest input, but a clean input's difference before one where the original faulted on page 0 (which never
                # kills; each shard already prefers a clean one, harness first_p0), or the merged row would differ from the unsharded one
                p0 = P0_MARK in first
                if fidx >= 0 and (m["fidx"] is None or (m["p0"] and not p0) or (p0 == m["p0"] and fidx < m["fidx"])):
                    m["fidx"], m["first"], m["p0"] = fidx, first, p0
            elif line.strip() and line not in extra:
                extra.append(line)
        for line in pending:  # -v: lines after the last function (none expected)
            if line.strip() and line not in extra:
                extra.append(line)
    return funcs, order, extra


def merged_block(addr, m, verbose=0):
    """(the function's output lines as an unsharded harness prints them, ok) from merge_shard_outputs' counts"""
    g = "%d/%d" % (m["gp"], m["gn"])
    r = ("%d/%d(-%d)" % (m["rp"], m["rn"], m["rbad"])) if m["rbad"] else "%d/%d" % (m["rp"], m["rn"])
    cv = "%d/%d" % (len(m["cov"]), m["nb"])
    ok = m["gp"] == m["gn"] and m["rp"] == m["rn"] and m["gn"] + m["rn"] > 0
    block = [line + "\n" for line in m.get("lines", [])]
    block.append("%08X  %-40.40s %10s %10s %8s  %s%s\n" % (addr, m["name"][:40], g, r, cv, "ok" if ok else m["first"],
                                                         " (some inputs aborted on both sides)" if m["ab"] else ""))
    if verbose > 1:
        block.append("    uncovered blocks:" + "".join(" %d" % b for b in range(m["nb"]) if b not in m["cov"]) + "\n")
    return block, ok


def shard_ranges(n, k):
    """the generated-input ranges [lo, hi) of k shards of n inputs (verify.py -shards, mutate.py's sharded phases)"""
    k = min(k, max(1, n))
    return [(i * n // k, (i + 1) * n // k) for i in range(k)]


def run_range_shards(cmd, n, k, env=None, timeout=None, wrap=None, part_args=None):
    """one harness command (executable and options, no -range/-shardout) run as k -range shards in parallel and merged
    into the output the unsharded command prints (2026-10-31, fasttools2 change 3: mutate.py's baseline gate and schema
    check). Inputs are seeded by their index alone and each function's dictionary is fixed (2026-10-29), so the merged
    rows equal the unsharded run's; recorded inputs run in the first shard. part_args(i, lo, hi): extra options for
    shard i (its own -mutrec file); wrap(fn): runs fn holding a job slot (mutate.py --pool); timeout: seconds per shard
    (a shard over it is killed). Returns dict(rc, stdout, stderr, walls, problems, timed_out): rc 2 when a shard died,
    timed out or was cut short (problems say which; stdout is then empty, never a partial table), else 1 when a row
    failed, else 0; stderr is every shard's stderr in shard order; walls the shards' wall times."""
    import threading
    import time
    ranges = shard_ranges(n, k)
    k = len(ranges)
    outs, errs, codes, walls, tos = [None] * k, [""] * k, [None] * k, [0.0] * k, [False] * k

    def one(i):
        lo, hi = ranges[i]
        c = list(cmd) + ["-range", "%d:%d" % (lo, hi), "-shardout"] + (part_args(i, lo, hi) if part_args else [])
        t = time.time()
        try:
            r = subprocess.run(c, cwd=ROOT, capture_output=True, text=True, errors="replace", env=env, timeout=timeout)
            outs[i], errs[i], codes[i] = r.stdout, r.stderr, r.returncode
        except subprocess.TimeoutExpired as e:
            outs[i] = e.stdout.decode(errors="replace") if isinstance(e.stdout, bytes) else (e.stdout or "")
            codes[i], tos[i] = -9, True
        except OSError as e:  # the executable did not start
            outs[i], errs[i], codes[i] = "", "%s\n" % e, 127
        walls[i] = time.time() - t
    job = wrap(one) if wrap else one
    ts = [threading.Thread(target=job, args=(i,)) for i in range(k)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    probs = shard_problems(outs, codes, ranges)
    res = dict(stderr="".join(errs), walls=walls, problems=probs, timed_out=any(tos))
    if probs:
        res.update(rc=2, stdout="")
        return res
    funcs, order, extra = merge_shard_outputs(outs)
    lines, fail = [HEADER + "\n"] + [line + "\n" for line in extra], 0
    for addr in sorted(order):  # the harness prints functions by ascending address
        block, ok = merged_block(addr, funcs[addr])
        lines += block
        fail += not ok
    res.update(rc=1 if fail or any(c == 1 for c in codes) else 0, stdout="".join(lines))
    return res


def run_sharded(exe, spec, unit, args, k, cache=None):
    """K harness processes over the generated-input index ranges [i*n/K, (i+1)*n/K) (recorded inputs in the
    first one), each holding its own HEAVY slot for a heavy unit; their per-function results are merged into
    the normal output: one line per function, counts summed, coverage united, the first difference from the
    lowest input index (inputs are seeded by their index alone, so a sharded run equals an unsharded one).
    cache (vcache): (session, keys, hits, misses): only the misses run (none: no shard starts); the hits' blocks
    (per-input -v lines, row, uncovered blocks) and -dsiout records are merged in; complete fresh blocks are stored."""
    import threading
    n = 1000
    if "-n" in args:
        n = int(args[args.index("-n") + 1])
    k = min(k, max(1, n))
    outs, codes, errs = [None] * k, [None] * k, [None] * k
    verbose = args.count("-v")
    dsiout = args[args.index("-dsiout") + 1] if "-dsiout" in args else None  # one file per shard, joined in shard order
    env = dict(os.environ)
    if PROG and PROG.settings.stderr == "1":  # the shards report only STUCK/HANG; the merged line comes from here
        env["WWHD_VERIFY_PROGRESS_STDERR"] = "alerts"
    run_args = args
    if cache:
        env["WWHD_VERIFY_CACHE_MARKS"] = "1"
        run_args = without_only(args) + ["-only", ",".join("%08X" % a for a in cache[3])]

    def worker(i):
        lo, hi = i * n // k, (i + 1) * n // k
        a = list(run_args)
        if dsiout:
            a[a.index("-dsiout") + 1] = "%s.shard%d" % (dsiout, i)
        with heavy_lock(unit):
            p = subprocess.Popen([exe, "-spec", spec] + a + ["-range", "%d:%d" % (lo, hi), "-shardout"],
                                 cwd=ROOT, stdout=subprocess.PIPE, env=env, stderr=subprocess.PIPE if cache else None)
            if PROG:
                PROG.child(p.pid)
            try:
                if cache:
                    errs[i] = MarkedStderr(p.stderr)
                if cache and isinstance(cache[0], vcache.ResumeStore):  # --resume: store each finished function's part
                    out = resume_stream(p.stdout, errs[i], cache, i, k, lo, hi,
                                        "%s.shard%d" % (dsiout, i) if dsiout else None)
                else:
                    out = p.stdout.read().decode()
                p.wait()
                if cache:
                    errs[i].join()
            except BaseException:
                p.kill()
                p.wait()
                raise
        outs[i], codes[i] = out, p.returncode
        if p.returncode not in (0, 1):  # report it as it happens (the stderr above it says why, if the harness could)
            print("verify: shard %d (inputs %d:%d) of %s ended with %s" % (i, lo, hi, unit, p.returncode), file=sys.stderr)

    if cache and not cache[3]:
        k, outs, codes = 0, [], []
    ts = [threading.Thread(target=worker, args=(i,)) for i in range(k)]
    if PROG and k:
        PROG.start(sharded=k)
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    if PROG and k:
        PROG.stop()
    dsi_parts = []  # (per-function lines, done) per shard, for the cache
    if dsiout:
        with open(dsiout, "a") as w:
            for i in range(k):
                part = "%s.shard%d" % (dsiout, i)
                if os.path.exists(part):
                    text = open(part).read()
                    w.write(text)
                    dsi_parts.append(split_dsi(text))
                    os.remove(part)
                else:
                    dsi_parts.append(({}, set()))
            for addr in sorted(cache[2]) if cache else []:  # cached functions' records (parse_dsiout unites per function)
                w.write("".join(cache[2][addr].get("dsiout") or []))
    probs = shard_problems(outs, codes, [(i * n // k, (i + 1) * n // k) for i in range(k)],
                           ["%08X" % a for a in cache[3]] if cache else None)
    if probs:  # never a partial table: callers that parse rows (nulltrap_compare, gates) then find none
        for line in probs:
            print("verify: INCOMPLETE sharded run: " + line)
            print("verify: INCOMPLETE sharded run: " + line, file=sys.stderr)
        print("verify: %s: no results printed (%d of %d shards incomplete); rerun" % (unit, len(probs), k), file=sys.stderr)
        return 2
    funcs, order, extra = merge_shard_outputs(outs, verbose)
    print(HEADER)
    for line in extra:
        print(line)
    hits = cache[2] if cache else {}
    fail = 0
    for addr in sorted(set(order) | set(hits)):
        if addr in hits and addr not in funcs:
            e = hits[addr]
            sys.stdout.write(e["stdout"])
            sys.stdout.flush()
            replay(e, addr, cache[1][addr])
            fail += not e["ok"]
            continue
        m = funcs[addr]
        block, ok = merged_block(addr, m, verbose)
        fail += not ok
        sys.stdout.write("".join(block))
        sys.stdout.flush()
        if cache and m["shards"] == k and all(c in (0, 1) for c in codes) and all(addr in e.done for e in errs) \
                and (not dsiout or all(addr in d[1] for d in dsi_parts)):
            cache[0].put(cache[1][addr], addr, "".join(block), [x for e in errs for x in e.notes.get(addr, [])], ok,
                         [x for d in dsi_parts for x in d[0].get(addr, [])] if dsiout else None)
    bad = [c for c in codes if c not in (0, 1)]
    if bad:
        print("verify: %d of %d shards failed to run (exit %s)" % (len(bad), k, bad), file=sys.stderr)
        return 2
    if "-refcache-write" in args:  # each shard wrote ADDR.ref.LO-HI: one ADDR.ref per function
        import refcache
        for addr, why in sorted(refcache.join_parts(args[args.index("-refcache-write") + 1], n).items()):
            print("verify: refcache: %s: no record file (%s); readers run its original live" % (addr, why), file=sys.stderr)
    if "-snapcache-write" in args:  # ... and ADDR-TAG-sSEED.val.LO-HI validation states (2026-10-31)
        import refcache
        for name, why in sorted(refcache.join_val_parts(args[args.index("-snapcache-write") + 1]).items()):
            print("verify: snapcache: %s: not joined (%s); readers run those setups live" % (name, why), file=sys.stderr)
    return 1 if fail or any(c == 1 for c in codes) else 0


def run_unsharded_cached(exe, spec, unit, args, cache):
    """one harness process for the cache misses (-only), its output passed through line by line with the hits' blocks
    inserted in address order (the harness prints functions by ascending address); complete fresh blocks are stored"""
    sess, keys, hits, misses = cache
    pending = sorted(hits)
    out = sys.stdout.buffer

    def flush_hits(upto):
        while pending and (upto is None or pending[0] < upto):
            a = pending.pop(0)
            out.write(hits[a]["stdout"].encode("latin-1"))
            out.flush()
            replay(hits[a], a, keys[a])

    dsiout = args[args.index("-dsiout") + 1] if "-dsiout" in args else None
    if dsiout:  # the cached functions' -dsiout records (parse_dsiout unites them per function; order is free)
        with open(dsiout, "a") as w:
            for a in sorted(hits):
                w.write("".join(hits[a].get("dsiout") or []))
    if not misses:
        out.write((HEADER + "\n").encode())
        flush_hits(None)
        return 1 if any(not e["ok"] for e in hits.values()) else 0
    env = dict(os.environ, WWHD_VERIFY_CACHE_MARKS="1")
    run_args = without_only(args) + ["-only", ",".join("%08X" % a for a in misses)]
    dsitmp = None
    if dsiout:  # this run's records in a file of their own: appended to -dsiout and split per function for the cache
        dsitmp = "%s.vcache%d" % (dsiout, os.getpid())
        run_args[run_args.index("-dsiout") + 1] = dsitmp
    with heavy_lock(unit):
        p = subprocess.Popen([exe, "-spec", spec] + run_args, cwd=ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if PROG:
            PROG.child(p.pid)
            PROG.start()
        err = MarkedStderr(p.stderr)
        blocks, done, cur = {}, set(), None
        try:
            for line in iter(p.stdout.readline, b""):
                m = vcache.MARK.match(line.decode("latin-1").rstrip("\n"))
                if m:
                    a = int(m.group(2), 16)
                    if m.group(1) == "<":
                        flush_hits(a)
                        cur, blocks[a] = a, []
                    else:
                        done.add(a)
                        cur = None
                        if isinstance(sess, vcache.ResumeStore):  # --resume: this function's part, stored now
                            resume_store_part(sess, keys, a, b"".join(blocks[a]).decode("latin-1"), err, 0, 1, 0, gen_n(args),
                                              dsitmp)
                    continue
                out.write(line)
                out.flush()
                if cur is not None:
                    blocks[cur].append(line)
            rc = p.wait()
            err.join()
        except BaseException:
            p.kill()
            p.wait()
            raise
        if PROG:
            PROG.stop()
    dsi, dsidone = {}, set()
    if dsitmp:
        text = open(dsitmp).read() if os.path.exists(dsitmp) else ""
        with open(dsiout, "a") as w:
            w.write(text)
        if os.path.exists(dsitmp):
            os.remove(dsitmp)
        dsi, dsidone = split_dsi(text)
    if rc not in (0, 1):
        return rc
    flush_hits(None)
    for a in misses:
        if a in done and a in err.done and (not dsiout or a in dsidone):
            block = b"".join(blocks[a]).decode("latin-1")
            ok = vcache.row_ok(block, a, RESULT_LINE)
            if ok is not None:
                sess.put(keys[a], a, block, err.notes.get(a, []), ok, dsi.get(a, []) if dsiout else None)
    return 1 if rc == 1 or any(not e["ok"] for e in hits.values()) else 0


def cache_plan(exe, spec, unit, args, k, resume=None):
    """the result-cache plan of this run: (session, keys, hits, misses), or None to run without the cache (switched off,
    archive mode, no RAM-disk scratch, an option that prints per-input detail, or the harness cannot list its functions).
    resume (--resume DIR): the plan reads and writes DIR's per-process parts instead (vcache.ResumeStore)"""
    cdir = resume or vcache.cache_dir()
    if not cdir or vcache.key_args(args, k) is None:
        if resume:
            print("verify: --resume %s off: the run's options print per-input detail or select inputs themselves" % resume,
                  file=sys.stderr, flush=True)
        return None
    r = subprocess.run([exe, "-spec", spec, "-listfn"], cwd=ROOT, capture_output=True, text=True,
                       env=dict(os.environ, WWHD_VERIFY_PROGRESS="0"))
    if r.returncode:
        return None
    funcs = []
    for line in r.stdout.splitlines():
        f = line.split()
        if len(f) == 2:
            funcs.append((int(f[0], 16), int(f[1], 16)))
    sel = vcache.only_set(args)
    funcs = [f for f in funcs if not sel or f[0] in sel]
    runlist = vcache.runlist_sel(args)  # -runlist: only its functions run, only its inputs; part of each key
    if runlist is not None:
        funcs = [f for f in funcs if f[0] in runlist]
    if not funcs:
        return None
    lines = specfile.expanded_lines(os.path.join(HERE, "units", unit + ".txt"))
    if resume:
        sess = vcache.ResumeStore(unit, lines, args, k, BUILD_INFO, cdir, exe)
    else:
        sess = vcache.Session(unit, lines, args, k, BUILD_INFO, cdir)
    keys, hits = {}, {}
    for addr, vm in funcs:
        keys[addr] = sess.func_key(addr, vm, runlist.get(addr) if runlist is not None else None)
        e = resume_entry(sess, keys[addr], addr, args, k) if resume else sess.get(keys[addr])
        if e and ("-dsiout" not in args or e.get("dsiout") is not None):
            hits[addr] = e
    misses = [a for a, _ in funcs if a not in hits]
    if resume:
        print("verify: resume %s: %d of %d function(s) from earlier parts of this campaign%s, %d to run" % (
            resume, len(hits), len(funcs), (": " + ",".join("%08X" % a for a in sorted(hits))) if hits else "", len(misses)),
            file=sys.stderr, flush=True)
    else:
        print("vcache: %d of %d function(s) cached, %d to run%s" % (len(hits), len(funcs), len(misses),
              " (%d key(s) cover all unit sources)" % sess.stats["fallback"] if sess.stats["fallback"] else ""), file=sys.stderr, flush=True)
    return sess, keys, hits, misses


def resume_entry(sess, key, addr, args, k):
    """--resume: a function's entry (as vcache's) from its k stored parts, or None when any part is missing or damaged.
    k > 1: the parts are merged exactly as run_sharded merges the shards' outputs"""
    n = gen_n(args)
    ranges = shard_ranges(n, k) if k > 1 else [(0, n)]
    kk = len(ranges)
    parts = []
    for i, (lo, hi) in enumerate(ranges):
        got = sess.get_part(key, addr, i, kk, lo, hi)
        if got is None:
            return None
        parts.append(got)
    notes = [x for p in parts for x in p[1]]
    dsi = None if any(p[2] is None for p in parts) else [x for p in parts for x in p[2]]
    if kk == 1:
        block = parts[0][0]
        ok = vcache.row_ok(block, addr, RESULT_LINE)
    else:
        funcs, _order, _extra = merge_shard_outputs([p[0] for p in parts], args.count("-v"))
        if addr not in funcs or funcs[addr]["shards"] != kk:
            return None
        bl, ok = merged_block(addr, funcs[addr], args.count("-v"))
        block = "".join(bl)
    if ok is None:
        return None
    return {"stdout": block, "stderr": notes, "ok": ok, "dsiout": dsi, "resume": sess.cdir, "parts": kk}


def header_args(unit):
    """arguments a spec declares for itself in a header comment `# verify: -only ADDR,... [-calleesp]`: a
    companion spec that steers a few functions and links the rest as support must run in that scope (unscoped
    runs can blow the memory guard or never finish)"""
    out = []
    scope = None
    for line in open(os.path.join(HERE, "units", unit + ".txt")):
        if not line.startswith("#"):
            continue
        m = re.match(r"#\s*verify:\s*(.+)$", line.strip())
        if m:
            out += m.group(1).split()
        # `# readable-scope: ADDR,...` (the scope readable_check and the merge gates use) is the spec's -only too
        m = re.match(r"#\s*readable-scope:\s*([0-9A-Fa-f]{8}(?:\s*,\s*[0-9A-Fa-f]{8})*)\s*$", line.strip())
        if m and scope is None:
            scope = re.sub(r"\s+", "", m.group(1))
    if scope and "-only" not in out:
        out += ["-only", scope]
    return out


class RunProgress:
    """verify.py's own progress: the file verify-PID.json in the unit's progress directory (unit, phase, harness children
    and their summed state) and, for a sharded run, the merged `[progress]` line (the shards print only STUCK/HANG)."""

    def __init__(self, unit, settings):
        self.unit, self.settings = unit, settings
        self.dir = os.path.join(unit_dir(unit), "progress")
        on_scratch = verify_scratch_base() is not None
        os.environ["WWHD_VERIFY_PROGRESS_DIR"] = self.dir
        os.environ["WWHD_VERIFY_PROGRESS_FILE_INTERVAL"] = str(progress.file_interval(settings, on_scratch))
        self.w = progress.Writer("verify", self.dir, progress.file_interval(settings, on_scratch))
        self.children, self.phase, self.sharded, self.ticker = [], "build", 0, None
        self.w.write({"unit": unit, "phase": "build"}, force=True)
        import atexit
        atexit.register(self.close)

    def child(self, pid):
        self.children.append(pid)

    def state(self):
        hs = [s for s in (progress.read_state(os.path.join(self.dir, "harness-%d.json" % c)) for c in self.children) if s]
        st = {"unit": self.unit, "phase": self.phase, "children": list(self.children)}
        if self.sharded:
            st["shards"] = self.sharded
        sm = progress.harness_summary(hs)
        if sm:
            st.update({"done": sm["all_done"], "total": sm["all_total"], "rate": sm["rate"], "eta_s": sm["eta_s"],
                       "hangs": sm["hangs"], "stuck": bool(sm["stuck"]),
                       "line": "function %d/%d (slowest shard)" % (sm["func_index"], sm["func_total"]) if self.sharded else
                       "function %d/%d" % (sm["func_index"], sm["func_total"])})
        return st, sm

    def tick(self):
        st, sm = self.state()
        self.w.write(st)
        if self.sharded and sm:
            self.settings.say("verify %s: %d shards, function %d/%d (slowest shard), %d/%d inputs, %.1f inputs/s, ETA %s%s" % (
                self.unit, self.sharded, sm["func_index"], sm["func_total"], sm["all_done"], sm["all_total"], sm["rate"],
                progress.fmt_dur(sm["eta_s"]), ", %d hang(s)" % sm["hangs"] if sm["hangs"] else ""))

    def start(self, sharded=0):
        self.phase, self.sharded = "run", sharded
        self.w.write(self.state()[0], force=True)
        self.ticker = progress.Ticker(self.settings.interval, self.tick)

    def stop(self):
        if self.ticker:
            self.ticker.stop()
            self.ticker = None

    def close(self):
        self.stop()
        self.w.close()


PROG = None


def main():
    global PROG
    check_scratch_env('verify')  # a set scratch variable naming a missing/unwritable folder stops here, nothing written
    unit = sys.argv[1]
    args = sys.argv[2:]
    os.environ.pop("WWHD_VERIFY_CACHE_MARKS", None)  # only verify.py's own cached runs ask the harness for marks
    use_cache = "--no-cache" not in args and os.environ.get("WWHD_VERIFY_CACHE", "1") != "0"
    archive = vcache.archive_mode(args)
    args = [a for a in args if a not in ("--no-cache", "--archive")]
    resume, live_all = None, False
    if "--resume" in args:  # --resume DIR: this campaign's per-function parts (a killed run resumes; also in archive mode)
        i = args.index("--resume")
        if i + 1 >= len(args):
            sys.exit("verify.py: --resume takes a directory")
        resume = os.path.abspath(args[i + 1])
        del args[i:i + 2]
    if archive and resume:
        print("verify: archive mode with --resume %s: functions this campaign finished earlier with the same key come from "
              "its parts there (listed below); every other function runs" % resume, file=sys.stderr)
    elif archive:  # final / archive gates: every row is computed by this run, never taken from the cache
        use_cache = False
        print("verify: archive mode: no cached rows (every function runs)", file=sys.stderr)
    settings = progress.Settings().take_flags(args)  # -noprogress / -progress-interval / -stuck / -hanglimit -> env
    if settings.on:
        PROG = RunProgress(unit, settings)
    args = ["-observe-pointees" if x == "--observe-pointees" else x for x in args]  # the mutate.py spelling works too
    if "--no-header-args" in args:
        args.remove("--no-header-args")
    else:
        hdr = header_args(unit)
        add = []
        i = 0
        while i < len(hdr):  # a flag the command line already gives wins
            if hdr[i] == "-only":
                if "-only" not in args:
                    add += hdr[i:i + 2]
                i += 2
            else:
                if hdr[i] not in args:
                    add.append(hdr[i])
                i += 1
        if add:
            print("verify: spec header adds " + " ".join(add) + " (--no-header-args to run without)", file=sys.stderr)
            args += add
    srcopt = "O2"
    if "-srcopt" in args:  # -srcopt O0|O1|O2|O3: the unit's own sources at that level (matched controls for mutate.py's O1 variants)
        i = args.index("-srcopt")
        if i + 1 >= len(args) or args[i + 1] not in ("O0", "O1", "O2", "O3"):
            sys.exit("verify.py: -srcopt takes O0, O1, O2 or O3")
        srcopt = args[i + 1]
        del args[i:i + 2]
    only = None
    if "-srcopt-only" in args:  # -srcopt-only SRC: only that unit source at -srcopt (default O1), the rest at O2
        i = args.index("-srcopt-only")
        if i + 1 >= len(args):
            sys.exit("verify.py: -srcopt-only takes a source path of the unit")
        only = args[i + 1]
        del args[i:i + 2]
        if srcopt == "O2":
            srcopt = "O1"
    global GUARD_STRICT
    if "-guardstrict" in args:  # the host-index wrap diagnostic build (GABI_GUEST_GUARD=2, see GUARD_STRICT)
        args.remove("-guardstrict")
        GUARD_STRICT = True
        use_cache = False  # a cached row would not repeat the run's guardstrict lines
        live_all = True
    if "-refcache-write" in args:  # reference-side records (nulltrap_compare --refcache-out): every function must run
        use_cache = False
        live_all = True
    if resume and live_all:  # -guardstrict / -refcache-write runs need every function's live run
        print("verify: --resume %s off for this run (-guardstrict or -refcache-write: every function runs)" % resume,
              file=sys.stderr)
        resume = None
    buildj = 1
    if "-buildj" in args:  # -buildj N: compile the unit's sources with N parallel compiler processes
        i = args.index("-buildj")
        if i + 1 >= len(args) or not args[i + 1].isdigit() or int(args[i + 1]) < 1:
            sys.exit("verify.py: -buildj takes a positive number")
        buildj = int(args[i + 1])
        del args[i:i + 2]
    split = unit_split(unit, args)  # -unitsplit N (removed from the harness options)
    exe = build(unit, srcopt, only, buildj, split)
    spec = specfile.spec_path(unit, ROOT, unit_dir(unit))
    rec = unit_rec(unit)
    if rec and "-rec" not in args:
        args += ["-rec", rec]
    k = shard_count(unit, args)
    plan = cache_plan(exe, spec, unit, args, k, resume) if (use_cache or resume) else None
    if plan:
        rc = run_sharded(exe, spec, unit, args, k, plan) if k > 1 else run_unsharded_cached(exe, spec, unit, args, plan)
        plan[0].evict()
        sys.exit(rc)
    if k > 1:
        sys.exit(run_sharded(exe, spec, unit, args, k))
    if "-list" in args:  # no inputs run: no HEAVY slot needed
        sys.exit(subprocess.run([exe, "-spec", spec] + args, cwd=ROOT).returncode)
    with heavy_lock(unit):
        p = subprocess.Popen([exe, "-spec", spec] + args, cwd=ROOT)
        if PROG:
            PROG.child(p.pid)
            PROG.start()
        try:
            rc = p.wait()
        except BaseException:
            p.kill()
            p.wait()
            raise
        if PROG:
            PROG.stop()
    sys.exit(rc)


if __name__ == "__main__":
    main()
