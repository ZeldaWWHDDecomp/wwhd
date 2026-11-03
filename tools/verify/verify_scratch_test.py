#!/usr/bin/env python3
"""Self-test of WWHD_VERIFY_SCRATCH (verify.unit_dir: unit build/output directories on a RAM disk, 2026-10-26).

Unit: tools/verify/units/_verify_scratch_test/u.txt = `include _verify_scratch_test/base.txt` (a copy of lib_mtx), so the
expanded spec file is written too. Scratch directories are made under build/verify/_verify_scratch_tmp/.
1. No variable: verify.py builds in build/verify/<unit> (reference rows).
2. WWHD_VERIFY_SCRATCH=S: the same result rows; unit.c, the objects, the verify executable and spec.expanded.txt are
   in S/<worktree>-<hash>/<unit>, nothing new in build/verify/<unit>, no stderr note.
3. verify_all.py --clean with S: same rows; the scratch unit directory is emptied, verify_all.log stays in
   build/verify/<unit>.
4. S missing: verify.py stops (exit 2) with an error naming the variable and S, no rows, nothing in
   build/verify/<unit>, S not created (2026-11-02: no silent SSD fallback).
5. S below the free-space floor (WWHD_VERIFY_SCRATCH_MIN_FREE larger than any disk): one note, build/verify/<unit>.
6. In process: unit_dir() with the variable unset is build/verify/<unit>; with S it is S/<key>/<unit> and stays the
   same for the process; unit_dirs_all() names both places; two roots get different keys.
Exit 0 when all cases pass; the temporary unit files and directories are removed.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UDIR = os.path.join(HERE, "units", "_verify_scratch_test")
UNIT = "_verify_scratch_test/u"
TMP = os.path.join(ROOT, "build", "verify", "_verify_scratch_tmp")
BUILD = os.path.join(ROOT, "build", "verify", UNIT)
ENVS = ("WWHD_VERIFY_SCRATCH", "WWHD_VERIFY_SCRATCH_MIN_FREE", "WWHD_MUT_SCRATCH")


def env(**kw):
    e = dict(os.environ)
    for k in ENVS:
        e.pop(k, None)
    e.update(kw)
    return e


def rows(out):
    return [l for l in out.splitlines() if re.match(r"^[0-9A-F]{8} ", l)]


def verify(**kw):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-n", "200", "-seed", "3", "-shards", "1"],
                       cwd=ROOT, capture_output=True, text=True, env=env(**kw))
    return r.returncode, rows(r.stdout), r.stderr


def files(d):
    return sorted(os.listdir(d)) if os.path.isdir(d) else []


def main():
    shutil.rmtree(UDIR, ignore_errors=True)
    shutil.rmtree(BUILD, ignore_errors=True)
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(UDIR)
    os.makedirs(TMP)
    S = os.path.join(TMP, "S")
    os.makedirs(S)
    fails = []
    try:
        open(os.path.join(UDIR, "base.txt"), "w").write(open(os.path.join(HERE, "units", "lib_mtx.txt")).read())
        open(os.path.join(UDIR, "u.txt"), "w").write("include _verify_scratch_test/base.txt\n")
        sys.path.insert(0, HERE)
        import verify as V
        real = os.path.realpath(ROOT)
        key = "%s-%s" % (os.path.basename(real), __import__("hashlib").sha1(real.encode()).hexdigest()[:8])
        SU = os.path.join(S, key, UNIT)

        rc, ref, err = verify()
        if rc or not ref:
            fails.append("reference run: rc %d, %d rows: %s" % (rc, len(ref), err[-300:]))
        if not {"unit.c", "verify", "spec.expanded.txt"} <= set(files(BUILD)):
            fails.append("reference run: build/verify/%s has %s" % (UNIT, files(BUILD)))
        shutil.rmtree(BUILD, ignore_errors=True)

        rc, got, err = verify(WWHD_VERIFY_SCRATCH=S)
        if rc or got != ref:
            fails.append("scratch run: rc %d, rows differ: %s" % (rc, got[:2]))
        if not {"unit.c", "unit.c.o", "harness.cpp.o", "verify", "spec.expanded.txt"} <= set(files(SU)):
            fails.append("scratch run: %s has %s" % (SU, files(SU)))
        if files(BUILD):
            fails.append("scratch run: build/verify/%s has %s" % (UNIT, files(BUILD)))
        if "WWHD_VERIFY_SCRATCH" in err:
            fails.append("scratch run printed a note: %s" % err[-300:])

        r = subprocess.run([sys.executable, os.path.join(HERE, "verify_all.py"), UNIT, "-n", "200", "-seed", "3", "-j", "1",
                            "--clean"], cwd=ROOT, capture_output=True, text=True, env=env(WWHD_VERIFY_SCRATCH=S))
        log = os.path.join(BUILD, "verify_all.log")
        if r.returncode or not os.path.exists(log) or rows(open(log).read()) != ref:
            fails.append("verify_all --clean: rc %d, log %s: %s" % (r.returncode, os.path.exists(log), (r.stdout + r.stderr)[-300:]))
        if files(SU):
            fails.append("verify_all --clean: scratch %s still has %s" % (SU, files(SU)))
        if files(BUILD) != ["verify_all.log"]:
            fails.append("verify_all --clean: build/verify/%s has %s" % (UNIT, files(BUILD)))
        shutil.rmtree(BUILD, ignore_errors=True)

        missing = os.path.join(TMP, "nosuchdir")
        rc, got, err = verify(WWHD_VERIFY_SCRATCH=missing)
        if rc != 2 or got or ("WWHD_VERIFY_SCRATCH=%s" % missing) not in err or files(BUILD):
            fails.append("missing dir: rc %d, rows %s, build %s, stderr %s" % (rc, got == ref, files(BUILD), err[-300:]))
        if os.path.exists(missing):
            fails.append("missing dir: %s was created" % missing)
        shutil.rmtree(BUILD, ignore_errors=True)

        shutil.rmtree(os.path.join(S, key), ignore_errors=True)
        rc, got, err = verify(WWHD_VERIFY_SCRATCH=S, WWHD_VERIFY_SCRATCH_MIN_FREE=str(1 << 60))
        if rc or got != ref or err.count("MB free") != 1 or "verify" not in files(BUILD) or os.path.exists(os.path.join(S, key)):
            fails.append("full dir: rc %d, rows %s, build %s, stderr %s" % (rc, got == ref, files(BUILD), err[-300:]))
        shutil.rmtree(BUILD, ignore_errors=True)

        # 6. in process
        for k in ENVS:
            os.environ.pop(k, None)
        V._unit_dirs.clear()
        if V.unit_dir("x") != os.path.join(ROOT, "build", "verify", "x"):
            fails.append("unit_dir unset: %s" % V.unit_dir("x"))
        os.environ["WWHD_VERIFY_SCRATCH"] = S
        V._unit_dirs.clear()
        d1 = V.unit_dir("y")
        os.environ["WWHD_VERIFY_SCRATCH_MIN_FREE"] = str(1 << 60)
        if d1 != os.path.join(S, key, "y") or V.unit_dir("y") != d1:
            fails.append("unit_dir with S: %s (then %s)" % (d1, V.unit_dir("y")))
        if V.unit_dirs_all("y") != [os.path.join(ROOT, "build", "verify", "y"), d1]:
            fails.append("unit_dirs_all: %s" % V.unit_dirs_all("y"))
        os.environ.pop("WWHD_VERIFY_SCRATCH_MIN_FREE")
        other = V.unit_dir("y", root=TMP)
        if os.path.dirname(os.path.dirname(other)) != S or os.path.dirname(other) == os.path.dirname(d1):
            fails.append("second root: %s vs %s" % (other, d1))
        os.environ.pop("WWHD_VERIFY_SCRATCH")
    finally:
        shutil.rmtree(UDIR, ignore_errors=True)
        shutil.rmtree(BUILD, ignore_errors=True)
        shutil.rmtree(os.path.dirname(BUILD), ignore_errors=True)
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("verify_scratch_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
