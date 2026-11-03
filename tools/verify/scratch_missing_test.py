#!/usr/bin/env python3
"""Self-test: a scratch variable naming a missing or unwritable folder stops the tool (2026-11-02).

WWHD_VERIFY_SCRATCH / WWHD_MUT_SCRATCH (and mutate.py --scratch) used to fall back silently to build/verify on the SSD
when the folder was missing; one night wrote ~211 MB to the SSD that way. Now:
1. In process: verify.verify_scratch_base() and mutate.Scratch with a missing folder exit 2 with an error naming the
   variable and the path; the folder is not created. An unwritable folder (mode 0500) does the same.
2. In process: a valid folder works (verify base DIR/<tree key>, mutate per-run directory below DIR, both removed);
   the variable unset keeps the default (no verify base, unit_dir in build/verify, no mutate run directory).
3. Command line: verify.py, verify_all.py, mutate.py (variable and --scratch), cascade.py, nulltrap_compare.py and
   readable_check.py each exit 2 with the error for each variable, write nothing (no build/ change in this tree, the
   folder and cascade's --out not created).
Temporary folders go below WWHD_TEST_TMP (default: the system temp
directory) and are removed at exit. Exit 0 when all cases pass. Takes a few seconds (no builds).
"""
import io
import os
import shutil
import stat
import subprocess
import sys
import tempfile
from contextlib import redirect_stderr

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
ENVS = ("WWHD_VERIFY_SCRATCH", "WWHD_MUT_SCRATCH", "WWHD_VERIFY_SCRATCH_MIN_FREE", "WWHD_MUT_SCRATCH_MIN_FREE")
UNIT = "_scratch_missing_test_nounit"


def tree(d):
    """every path below d (a snapshot to prove nothing was written)"""
    out = set()
    for r, ds, fs in os.walk(d):
        out.update(os.path.join(r, x) for x in ds + fs)
    return out


def exits(fn):
    """(exit code or None, stderr) of fn()"""
    err = io.StringIO()
    try:
        with redirect_stderr(err):
            fn()
    except SystemExit as e:
        return e.code, err.getvalue()
    return None, err.getvalue()


def main():
    for k in ENVS:
        os.environ.pop(k, None)
    base = os.environ.get("WWHD_TEST_TMP") or None
    tmp = tempfile.mkdtemp(prefix="scratch_missing_test-", dir=base)
    missing = os.path.join(tmp, "nosuchdir")
    locked = os.path.join(tmp, "locked")
    good = os.path.join(tmp, "good")
    os.makedirs(locked)
    os.makedirs(good)
    os.chmod(locked, stat.S_IRUSR | stat.S_IXUSR)
    fails = []
    build_before = tree(os.path.join(ROOT, "build"))
    try:
        import verify
        import mutate

        # 1. missing / unwritable, in process
        for bad, what in ((missing, "missing"), (locked, "not writable")):
            os.environ["WWHD_VERIFY_SCRATCH"] = bad
            verify._unit_dirs.clear()
            code, err = exits(lambda: verify.verify_scratch_base())
            if code != 2 or ("WWHD_VERIFY_SCRATCH=%s" % bad) not in err:
                fails.append("verify_scratch_base %s: exit %r, stderr %r" % (what, code, err))
            code, err = exits(lambda: verify.unit_dir(UNIT))
            if code != 2:
                fails.append("unit_dir %s: exit %r (a fallback to %s?)" % (what, code, verify._unit_dirs.get((os.path.realpath(ROOT), UNIT))))
            os.environ.pop("WWHD_VERIFY_SCRATCH")
            os.environ["WWHD_MUT_SCRATCH"] = bad
            code, err = exits(lambda: mutate.Scratch(UNIT, os.path.join(tmp, "out"), None, None))
            if code != 2 or ("WWHD_MUT_SCRATCH=%s" % bad) not in err:
                fails.append("mutate.Scratch %s: exit %r, stderr %r" % (what, code, err))
            os.environ.pop("WWHD_MUT_SCRATCH")
            code, err = exits(lambda: mutate.Scratch(UNIT, os.path.join(tmp, "out"), bad, None))
            if code != 2 or ("--scratch=%s" % bad) not in err:
                fails.append("mutate.Scratch --scratch %s: exit %r, stderr %r" % (what, code, err))
        if os.path.exists(missing):
            fails.append("in process: %s was created" % missing)
        if os.listdir(locked) or os.path.exists(os.path.join(tmp, "out")):
            fails.append("in process: files written: %s" % sorted(tree(tmp)))

        # 2. valid folder and unset
        os.environ["WWHD_VERIFY_SCRATCH"] = good
        os.environ["WWHD_MUT_SCRATCH"] = good
        verify._unit_dirs.clear()
        code, err = exits(lambda: verify.check_scratch_env("t"))
        b = verify.verify_scratch_base()
        if code is not None or not b or os.path.dirname(b) != good or verify.unit_dir(UNIT) != os.path.join(b, UNIT):
            fails.append("valid: exit %r, base %r, unit_dir %r: %s" % (code, b, verify._unit_dirs, err))
        sc = mutate.Scratch(UNIT, os.path.join(tmp, "out"), None, None)
        if not sc.dir or os.path.dirname(sc.dir) != good:
            fails.append("valid: mutate run directory %r" % sc.dir)
        sc.close()
        if b:
            shutil.rmtree(b, ignore_errors=True)
        if os.listdir(good):
            fails.append("valid: left %s" % os.listdir(good))
        os.environ.pop("WWHD_VERIFY_SCRATCH")
        os.environ.pop("WWHD_MUT_SCRATCH")
        verify._unit_dirs.clear()
        code, err = exits(lambda: verify.check_scratch_env("t"))
        if code is not None or verify.verify_scratch_base() is not None or \
                verify.unit_dir(UNIT) != os.path.join(ROOT, "build", "verify", UNIT):
            fails.append("unset: exit %r, unit_dir %r" % (code, verify._unit_dirs))
        sc = mutate.Scratch(UNIT, os.path.join(tmp, "out"), None, None)
        if sc.dir is not None:
            fails.append("unset: mutate run directory %r" % sc.dir)

        # 3. command line, every tool, every variable
        out = os.path.join(tmp, "cascade_out")
        cmds = [["verify.py", UNIT, "-n", "10"], ["verify_all.py", UNIT], ["mutate.py", UNIT, "--max", "1"],
                ["cascade.py", "--receipts", os.path.join(tmp, "r"), "--out", out, "--input-seed", "1"],
                ["nulltrap_compare.py", UNIT], ["readable_check.py", UNIT]]
        runs = [(c, var, bad) for c in cmds for var in ("WWHD_VERIFY_SCRATCH", "WWHD_MUT_SCRATCH") for bad in (missing, locked)]
        runs.append((["mutate.py", UNIT, "--max", "1", "--scratch", missing], "--scratch", missing))
        for c, var, bad in runs:
            e = {k: v for k, v in os.environ.items() if k not in ENVS}
            if var.startswith("WWHD_"):
                e[var] = bad
            r = subprocess.run([sys.executable, os.path.join(HERE, c[0])] + c[1:], cwd=ROOT, capture_output=True,
                               text=True, env=e, timeout=120)
            if r.returncode != 2 or ("%s=%s" % (var, bad)) not in r.stderr:
                fails.append("%s %s=%s: rc %d, stderr %s" % (c[0], var, bad, r.returncode, r.stderr[-300:]))
        if os.path.exists(missing) or os.path.exists(out) or os.listdir(locked):
            fails.append("command line: files written: %s" % sorted(tree(tmp)))
        if tree(os.path.join(ROOT, "build")) != build_before:
            fails.append("build/ changed: %s" % sorted(tree(os.path.join(ROOT, "build")) ^ build_before)[:10])
    finally:
        os.chmod(locked, stat.S_IRWXU)
        shutil.rmtree(tmp, ignore_errors=True)  # this run's own temporary folder
    for f in fails:
        print("FAIL", f)
    print("scratch_missing_test: %s" % ("OK" if not fails else "%d failures" % len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
