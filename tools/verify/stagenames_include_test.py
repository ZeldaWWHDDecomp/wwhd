#!/usr/bin/env python3
"""Self-test (2026-11-01): `fieldstr ... @stages` in a spec reached through `include`.

A unit with `include` lines is expanded by specfile.py into the build / verify-scratch directory; the harness looked up
tools/verify/stage_names.txt relative to that expanded file, found nothing there and exited 2
("fieldstr: no .../stage_names.txt"). It now resolves it from the tools/verify directory it was compiled from (ctor_sizes.tsv keeps its old lookup).

Checks: a temporary companion unit tools/verify/units/_stagenames_test/comp.txt = `include d_a_arrow.txt` (d_a_arrow
uses @stages on 02053814) runs from a fresh verify scratch with exit 0, and its result rows are byte-identical to the
rows of d_a_arrow itself (the no-change check: the direct unit path is unchanged, and the include path now sees the same
stage names). -n 200, seed 1, -nulltrap. The temporary unit directory and scratch are removed at the end.
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITDIR = os.path.join(HERE, "units", "_stagenames_test")


def rows(out):
    return [l for l in out.splitlines() if len(l) > 10 and l[8:10] == "  " and all(c in "0123456789ABCDEF" for c in l[:8])]


def run(unit, scratch):
    env = dict(os.environ, WWHD_VERIFY_SCRATCH=scratch)
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-n", "200", "-seed", "1", "-nulltrap"],
                       cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout, r.stderr


def main():
    fails = []
    base = os.environ.get("WWHD_VERIFY_SCRATCH") or os.path.join(ROOT, "build", "verify")
    scratch = tempfile.mkdtemp(prefix="stagenames_test-", dir=base)
    os.makedirs(UNITDIR, exist_ok=True)
    try:
        open(os.path.join(UNITDIR, "comp.txt"), "w").write("include d_a_arrow.txt\n")
        rc, out, err = run("_stagenames_test/comp", scratch)
        if rc != 0 or "fieldstr: no" in err:
            fails.append("companion: exit %d: %s" % (rc, (err or out)[-400:]))
        rc2, out2, err2 = run("d_a_arrow", scratch)
        if rc2 != 0:
            fails.append("d_a_arrow: exit %d: %s" % (rc2, (err2 or out2)[-400:]))
        a, b = rows(out), rows(out2)
        if not fails and (not a or a != b):
            fails.append("rows differ (%d companion, %d direct): %s / %s" % (len(a), len(b), a[:2], b[:2]))
    finally:
        shutil.rmtree(UNITDIR, ignore_errors=True)
        if not fails:
            shutil.rmtree(scratch, ignore_errors=True)
    print("stagenames_include_test: %s" % ("FAIL: " + "; ".join(fails) if fails else "ok (include path exit 0, rows equal to d_a_arrow)"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
