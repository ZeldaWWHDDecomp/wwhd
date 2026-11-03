#!/usr/bin/env python3
"""Self-test of mutate.py --scratch / WWHD_MUT_SCRATCH (2026-10-26, scratch files on a RAM disk).

Uses the merged spec lib2_sead_15, mutating only 02759928, --max 6 --seed 3, with receipts (each receipt's run command
names the mutant executable, so it shows where the mutant was built).
1. Baseline (no scratch): executables in build/verify/lib2_sead_15.
2. --scratch S: executables in a per-run subdirectory of S, a stderr note names it, S is empty afterwards, and the
   classes per mutant ID equal the baseline.
3. WWHD_MUT_SCRATCH=E: executables under E; with both, --scratch wins.
4. --scratch of a missing directory: exit 2 with an error naming it, no receipts, directory not created (2026-11-02).
5. A full scratch (WWHD_MUT_SCRATCH_MIN_FREE larger than any disk): one note, executables in the build directory.
7. WWHD_VERIFY_SCRATCH=S alone: the baseline build and the mutant files are in S/<worktree>-<hash>/<unit>, same
   classes; with --scratch E as well, the mutants go to E.
8. In process: a slashed (companion) unit name gets its run directory directly in the scratch (`/` -> `__`), and its
   verify unit directory and progress files work under WWHD_VERIFY_SCRATCH.
6. In process: Scratch.pick() with a fake free-space function switches per mutant (room -> build dir when below the
   margin -> back to the scratch when space returns) and notes the fallback once.
Temporary directories under build/verify/_mutate_scratch_test/ are removed. Exit 0 when all cases pass.
"""
import glob
import io
import json
import os
import shutil
import subprocess
import sys
from contextlib import redirect_stderr

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_scratch_test")
UNIT = "lib2_sead_15"
BUILD = os.path.join(ROOT, "build", "verify", UNIT)
BASE = [UNIT, "--func", "02759928", "--n", "50", "--max", "6", "--seed", "3", "-j", "2"]


def mutate(rec, *extra, env=None):
    e = dict(os.environ)
    e.pop("WWHD_MUT_SCRATCH", None)
    e.pop("WWHD_MUT_SCRATCH_MIN_FREE", None)
    e.pop("WWHD_VERIFY_SCRATCH", None)
    e.pop("WWHD_VERIFY_SCRATCH_MIN_FREE", None)
    e.update(env or {})
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + BASE + ["--receipts", rec] + list(extra),
                       cwd=ROOT, capture_output=True, text=True, env=e)
    return r.returncode, r.stdout, r.stderr


def classes(d):
    """{id: (class, directory of the mutant executable or None)}"""
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        cmd = j.get("cmd")
        out[j["id"]] = (j["class"], os.path.dirname(cmd[0]) if cmd else None)
    return out


def main():
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(TMP)
    fails = []
    S, E = os.path.join(TMP, "scratchS"), os.path.join(TMP, "scratchE")
    os.makedirs(S)
    os.makedirs(E)

    def check(name, rc, err, got, base, where, note=None, notes=1):
        if rc:
            fails.append("%s: rc %d: %s" % (name, rc, err[-400:]))
        if {i: c for i, (c, _) in got.items()} != {i: c for i, (c, _) in base.items()}:
            fails.append("%s: classes %s != baseline %s" % (name, got, base))
        dirs = {d for _, d in got.values() if d}
        if not dirs or not all(where(d) for d in dirs):
            fails.append("%s: mutant executables in %s" % (name, dirs))
        if note is not None and err.count(note) != notes:
            fails.append("%s: %d stderr notes %r (want %d): %s" % (name, err.count(note), note, notes, err[-400:]))

    try:
        rc, out, err = mutate(os.path.join(TMP, "base"))
        base = classes(os.path.join(TMP, "base"))
        if rc or len(base) != 6:
            fails.append("baseline: rc %d, %d receipts: %s" % (rc, len(base), (out + err)[-400:]))
        if not all(d == BUILD for _, d in base.values() if d):
            fails.append("baseline: executables in %s" % {d for _, d in base.values()})
        if "mutant scratch" in err:
            fails.append("baseline printed a scratch note: %s" % err[-300:])

        rc, out, err = mutate(os.path.join(TMP, "opt"), "--scratch", S)
        check("--scratch", rc, err, classes(os.path.join(TMP, "opt")), base,
              lambda d: os.path.dirname(d) == S and os.path.basename(d).startswith(UNIT + "-"), "mutant scratch files in " + S)
        if os.listdir(S):
            fails.append("--scratch: left %s" % os.listdir(S))

        rc, out, err = mutate(os.path.join(TMP, "env"), env={"WWHD_MUT_SCRATCH": E})
        check("WWHD_MUT_SCRATCH", rc, err, classes(os.path.join(TMP, "env")), base, lambda d: os.path.dirname(d) == E,
              "(from WWHD_MUT_SCRATCH)")
        if os.listdir(E):
            fails.append("WWHD_MUT_SCRATCH: left %s" % os.listdir(E))

        rc, out, err = mutate(os.path.join(TMP, "both"), "--scratch", S, env={"WWHD_MUT_SCRATCH": E})
        check("option wins", rc, err, classes(os.path.join(TMP, "both")), base, lambda d: os.path.dirname(d) == S, "(from --scratch)")

        missing = os.path.join(TMP, "nosuchdir")
        rc, out, err = mutate(os.path.join(TMP, "missing"), "--scratch", missing)
        if rc != 2 or ("--scratch=%s" % missing) not in err or classes(os.path.join(TMP, "missing")):
            fails.append("missing dir: rc %d, stderr %s" % (rc, err[-300:]))
        if os.path.exists(missing):
            fails.append("missing dir: %s was created" % missing)

        rc, out, err = mutate(os.path.join(TMP, "full"), "--scratch", S, env={"WWHD_MUT_SCRATCH_MIN_FREE": str(1 << 60)})
        check("full dir", rc, err, classes(os.path.join(TMP, "full")), base, lambda d: d == BUILD, "MB margin")
        if os.listdir(S):
            fails.append("full dir: left %s" % os.listdir(S))

        # 7. WWHD_VERIFY_SCRATCH alone: the unit's build directory, mutants included, is S/<tree key>/<unit>
        import hashlib
        real = os.path.realpath(ROOT)
        VU = os.path.join(S, "%s-%s" % (os.path.basename(real), hashlib.sha1(real.encode()).hexdigest()[:8]), UNIT)
        rc, out, err = mutate(os.path.join(TMP, "vscratch"), env={"WWHD_VERIFY_SCRATCH": S})
        check("WWHD_VERIFY_SCRATCH", rc, err, classes(os.path.join(TMP, "vscratch")), base, lambda d: d == VU)
        if not os.path.exists(os.path.join(VU, "verify")) or any(f.startswith("mutant") and not f.endswith(".cpp") for f in os.listdir(VU)):
            # (a mutant that does not compile keeps its mutantN.cpp in the build directory, as before)
            fails.append("WWHD_VERIFY_SCRATCH: %s has %s" % (VU, os.listdir(VU) if os.path.isdir(VU) else None))
        # with both, the mutants go to the --scratch run directory and the baseline build stays in VU
        rc, out, err = mutate(os.path.join(TMP, "vboth"), "--scratch", E, env={"WWHD_VERIFY_SCRATCH": S})
        check("both scratches", rc, err, classes(os.path.join(TMP, "vboth")), base, lambda d: os.path.dirname(d) == E,
              "(from --scratch)")
        shutil.rmtree(os.path.join(S, os.path.basename(os.path.dirname(VU))), ignore_errors=True)

        # 6. per-mutant switching with a fake free-space function
        sys.path.insert(0, HERE)
        import mutate as mm
        frees = iter([10 << 30, 1 << 20, 1 << 20, 10 << 30])
        mm.free_bytes = lambda p: next(frees)
        buf = io.StringIO()
        with redirect_stderr(buf):
            sc = mm.Scratch(UNIT, BUILD, S, None)
            picks = [sc.pick() for _ in range(4)]
            sc.close()
        if picks[0] != sc.dir or picks[1] != BUILD or picks[2] != BUILD or picks[3] != sc.dir:
            fails.append("fake free space: picks %s (scratch %s)" % (picks, sc.dir))
        if buf.getvalue().count("MB margin") != 1 or sc.margin != mm.SCRATCH_FLOOR:
            fails.append("fake free space: margin %d, notes %r" % (sc.margin, buf.getvalue()))
        if os.path.exists(sc.dir):
            fails.append("fake free space: run dir %s left" % sc.dir)

        # 8. a companion spec (slashed unit name): the run directory is made directly in the scratch (`/` -> `__`), no
        # fallback to the build directory; verify.py's unit directory and a progress file of such a unit work there too
        buf = io.StringIO()
        with redirect_stderr(buf):
            sc = mm.Scratch("lib2_agl_5/shader_holder", BUILD, S, None)
        ok = sc.dir and os.path.dirname(sc.dir) == S and os.path.basename(sc.dir).startswith("lib2_agl_5__shader_holder-")
        sc.close()
        if not ok or "cannot create" in buf.getvalue():
            fails.append("slashed unit: run dir %s, stderr %r" % (sc.dir, buf.getvalue()))
        os.environ["WWHD_VERIFY_SCRATCH"] = S
        import verify as vv
        import progress as pp
        vu = vv.unit_dir("_scratch_slash_test/companion")
        w = pp.Writer("verify", os.path.join(vu, "progress"), 0)
        w.write({"unit": "_scratch_slash_test/companion", "phase": "build"}, force=True)
        found = pp.collect(["_scratch_slash_test/companion"])
        w.close()
        if not vu.startswith(S + os.sep) or not found:
            fails.append("slashed unit: unit dir %s, progress files %s" % (vu, found))
        shutil.rmtree(os.path.dirname(os.path.dirname(vu)), ignore_errors=True)
        os.environ.pop("WWHD_VERIFY_SCRATCH", None)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_scratch_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
