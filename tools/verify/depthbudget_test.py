#!/usr/bin/env python3
"""Self-test of the nested `real` call depth budget and the unit-file directive `depthbudget N` (harness.cpp, 2026-10-31).

The abort path itself was proven on the AGL#8 repro (027931EC, gen seed 1001489: the original's real heap routine
027540AC recursing with r3 = 0; both sides now abort with `depth budget exceeded (1024 nested real calls)` instead of a
host stack overflow). This test covers what a merged carrier can show:
1. lib2_sead_15 rows are byte-identical with the default budget and with `depthbudget 64` (its real chains are shallow),
   so the check costs no rows.
2. Malformed or out-of-range lines (`depthbudget`, `depthbudget 5`, `depthbudget 4096`, `depthbudget 64 x`) exit 2.
Writes temporary unit files under tools/verify/units/_depthbudget_test/ and removes them. Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_depthbudget_test")
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()


def run(name, lines, n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_depthbudget_test/" + name, "-n", n, "-seed", "1"],
                       cwd=ROOT, capture_output=True, text=True)
    rows = [l for l in r.stdout.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)]
    return r.returncode, rows


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc1, a = run("default", SPEC)
        rc2, b = run("d64", SPEC + ["depthbudget 64"])
        if not a or a != b or rc1 != rc2:
            fails.append("rows differ with depthbudget 64 (rc %d/%d, %d/%d rows)" % (rc1, rc2, len(a), len(b)))
        for bad in ("depthbudget", "depthbudget 5", "depthbudget 4096", "depthbudget 64 x"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_depthbudget_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("depthbudget_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
