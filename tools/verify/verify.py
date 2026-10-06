#!/usr/bin/env python3
"""Build and run a verification unit.

usage: verify.py UNIT [harness options...]      e.g. verify.py d_a_mtoge -n 2000 -rec build/verify/rec

Steps: mkimage (once), mkunit (unit.c from build/gen), compile unit.c + the candidate sources +
the harness with exact FP flags, run. Harness options: -n N generated inputs per function,
-seed S, -rec DIR recorded inputs (DIR/ADDR/*.tap), -only ADDR, -v / -v -v.
A unit file line `rec DIR` (path relative to the repository root, e.g. tools/verify/recordings/d_grass) adds
those recordings to every run of the unit; an explicit -rec on the command line takes precedence.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
FP = ["-ffp-contract=off", "-fno-fast-math", "-fno-strict-aliasing"]


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
    livein = os.path.join(ROOT, "build", "verify", "livein.tsv")  # indirect-call live-in table (livein_all.py)
    if not os.path.exists(livein) or os.path.getmtime(livein) < os.path.getmtime(os.path.join(ROOT, "build", "gen")):
        sh([sys.executable, os.path.join(HERE, "livein_all.py"), livein])
    frames = os.path.join(ROOT, "build", "verify", "frames.tsv")  # original stack frames (frames_all.py; gabi automatic native frames)
    if not os.path.exists(frames) or os.path.getmtime(frames) < os.path.getmtime(os.path.join(ROOT, "build", "gen")) \
            or os.path.getmtime(frames) < os.path.getmtime(os.path.join(HERE, "frames_all.py")):
        sh([sys.executable, os.path.join(HERE, "frames_all.py"), frames])


def build(unit):
    out = os.path.join(ROOT, "build", "verify", unit)
    if os.environ.get("WWHD_SHARED_READY") != "1":
        prepare_shared()
    sh([sys.executable, os.path.join(HERE, "mkunit.py"), unit, "--root", ROOT])
    srcs = []
    for line in open(os.path.join(HERE, "units", unit + ".txt")):
        f = line.split("#")[0].split()
        if f and f[0] == "src":
            srcs += f[1:]
    inc = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include"]
    objs = []
    for src, lang in [(os.path.join(out, "unit.c"), "c"), ("runtime/src/espresso_fp.c", "c"), ("tools/verify/src/harness.cpp", "c++")] + \
            [(s, "c++") for s in srcs]:
        obj = os.path.join(out, os.path.basename(src) + ".o")
        objs.append(obj)
        if os.path.exists(obj) and os.path.getmtime(obj) > max(os.path.getmtime(os.path.join(ROOT, src)), newest_header()):
            continue
        if lang == "c":
            cmd = ["clang", "-std=c11", "-O2", "-w"] + FP + inc + ["-c", src, "-o", obj]
        else:
            cmd = ["clang++", "-std=c++20", "-O2", "-Wall", "-Wno-invalid-offsetof", "-Wno-unused-function"] + FP + inc + ["-c", src, "-o", obj]
        sh(cmd)
    exe = os.path.join(out, "verify")
    sh(["clang++", "-o", exe] + objs)
    return exe


def unit_rec(unit):
    """The `rec DIR` directive of a unit file (absolute path), or None."""
    for line in open(os.path.join(HERE, "units", unit + ".txt")):
        f = line.split("#")[0].split()
        if len(f) == 2 and f[0] == "rec":
            d = os.path.join(ROOT, f[1])
            if not os.path.isdir(d):
                # recordings hold game memory and are not distributed: record them locally (README, "Recorded")
                print("verify: no recordings at %s (generated inputs only)" % f[1], file=sys.stderr)
                return None
            return d
    return None


def newest_header():
    t = 0
    for d in ("tools/verify/include", "tools/verify/src", "wwhd_src/include", "runtime/include"):
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
    Options that print per-input detail (-v, -trace) or select inputs themselves (-range, -list) run unsharded."""
    k = None
    if "-shards" in args:
        i = args.index("-shards")
        k = int(args[i + 1])
        del args[i:i + 2]
    if any(a in args for a in ("-v", "-trace", "-range", "-list", "-shardout")):
        return 1
    if k is None and os.environ.get("WWHD_SHARDS"):
        k = int(os.environ["WWHD_SHARDS"])
    if k is None:
        k = HEAVY_SLOTS if unit in HEAVY else 1
    return max(1, k)


def run_sharded(exe, spec, unit, args, k):
    """K harness processes over the generated-input index ranges [i*n/K, (i+1)*n/K) (recorded inputs in the
    first one), each holding its own HEAVY slot for a heavy unit; their per-function results are merged into
    the normal output: one line per function, counts summed, coverage united, the first difference from the
    lowest input index (inputs are seeded by their index alone, so a sharded run equals an unsharded one)."""
    import threading
    n = 1000
    if "-n" in args:
        n = int(args[args.index("-n") + 1])
    k = min(k, max(1, n))
    outs, codes = [None] * k, [None] * k

    def worker(i):
        lo, hi = i * n // k, (i + 1) * n // k
        with heavy_lock(unit):
            r = subprocess.run([exe, "-spec", spec] + args + ["-range", "%d:%d" % (lo, hi), "-shardout"],
                               cwd=ROOT, stdout=subprocess.PIPE, text=True)
        outs[i], codes[i] = r.stdout, r.returncode

    ts = [threading.Thread(target=worker, args=(i,)) for i in range(k)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    funcs, order, extra = {}, [], []
    for i in range(k):
        for line in outs[i].splitlines():
            if line.startswith("@SHARD "):
                head, name, first = line[7:].split("|", 2)
                f = head.split()
                addr = int(f[0], 16)
                gp, gn, rp, rn, rbad, ab, fidx, nb = map(int, f[1:9])
                cov = set() if f[9] == "-" else set(map(int, f[9].split(",")))
                if addr not in funcs:
                    funcs[addr] = dict(name=name, gp=0, gn=0, rp=0, rn=0, rbad=0, ab=0, nb=nb, cov=set(), first="", fidx=None)
                    order.append(addr)
                m = funcs[addr]
                m["gp"] += gp; m["gn"] += gn; m["rp"] += rp; m["rn"] += rn; m["rbad"] += rbad; m["ab"] += ab
                m["cov"] |= cov
                if fidx >= 0 and (m["fidx"] is None or fidx < m["fidx"]):
                    m["fidx"], m["first"] = fidx, first
            elif line.strip() and line not in extra:
                extra.append(line)
    print("%-8s  %-40s %10s %10s %8s  %s" % ("addr", "function", "generated", "recorded", "coverage", "first difference"))
    for line in extra:
        print(line)
    fail = 0
    for addr in sorted(order):
        m = funcs[addr]
        g = "%d/%d" % (m["gp"], m["gn"])
        r = ("%d/%d(-%d)" % (m["rp"], m["rn"], m["rbad"])) if m["rbad"] else "%d/%d" % (m["rp"], m["rn"])
        cv = "%d/%d" % (len(m["cov"]), m["nb"])
        ok = m["gp"] == m["gn"] and m["rp"] == m["rn"] and m["gn"] + m["rn"] > 0
        fail += not ok
        print("%08X  %-40.40s %10s %10s %8s  %s%s" % (addr, m["name"][:40], g, r, cv, "ok" if ok else m["first"],
                                                    " (some inputs aborted on both sides)" if m["ab"] else ""), flush=True)
    bad = [c for c in codes if c not in (0, 1)]
    if bad:
        print("verify: %d of %d shards failed to run (exit %s)" % (len(bad), k, bad), file=sys.stderr)
        return 2
    return 1 if fail or any(c == 1 for c in codes) else 0


def main():
    unit = sys.argv[1]
    exe = build(unit)
    spec = os.path.join(HERE, "units", unit + ".txt")
    args = sys.argv[2:]
    rec = unit_rec(unit)
    if rec and "-rec" not in args:
        args += ["-rec", rec]
    k = shard_count(unit, args)
    if k > 1:
        sys.exit(run_sharded(exe, spec, unit, args, k))
    if "-list" in args:  # no inputs run: no HEAVY slot needed
        sys.exit(subprocess.run([exe, "-spec", spec] + args, cwd=ROOT).returncode)
    with heavy_lock(unit):
        r = subprocess.run([exe, "-spec", spec] + args, cwd=ROOT)
    sys.exit(r.returncode)


if __name__ == "__main__":
    main()
