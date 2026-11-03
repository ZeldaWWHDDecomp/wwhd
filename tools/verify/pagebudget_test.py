#!/usr/bin/env python3
"""Self-test of the per-run page budget and the unit-file directive `pagebudget N` (harness.cpp, 2026-10-31).

A run (setup, pokes, function; each side) that touches more than the budget's guest pages aborts the input on the side
that ran, like the access, call and time budgets. Carrier: merged lib2_sead_15's constructWorkerCounter 02759928 (as in
budget_allocfail_test.py) with a `real` setup of the original JASystem::Calc::bzero 02843E34 clearing 0x1100004 bytes at
0x40000000: 4353 guest pages and ~4.5M word stores, well inside the default page and access budgets.
1. Default budget: the unit passes (the setup completes on every input), row `ok`.
2. `pagebudget 4096`: the setup touches its 4097th page and every input aborts on both sides (each side counts the
   pages it touches, also those the other side's run created): row `0/0 ... (some inputs aborted on both sides)`, never
   `only one side aborted`; `-trace` shows the reason `page budget exceeded (4096 guest pages touched in one run)`.
3. `pagebudget 8192` (above the setup's 4353): passes again.
4. Malformed or out-of-range lines (`pagebudget`, `pagebudget 5`, `pagebudget 2000000`, `pagebudget 8192 x`) exit 2.
Writes temporary unit files under tools/verify/units/_pagebudget_test/ and removes them. Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_pagebudget_test")
FUNC = "02759928"
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
FILL = ["real 02843E34", "setup * 02843E34 r3=0x40000000 r4=0x1100004"]


def run(name, lines, extra=(), n="8"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_pagebudget_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1", "-seed", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    return r.returncode, (row[0] if row else ""), out


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc, row, out = run("default", SPEC + FILL)
        if rc != 0 or not row.endswith(" ok") or "aborted" in row:
            fails.append("default budget: setup should complete (rc %d): %s" % (rc, row or out[-300:]))
        rc, row, out = run("small", SPEC + FILL + ["pagebudget 4096"])
        cols = row.split()
        if rc not in (0, 1) or "only one side aborted" in row or "aborted on both sides" not in row or cols[2:4] != ["0/0", "0/0"]:
            fails.append("pagebudget 4096: every input should abort on both sides (0/0 compared, no FAIL; rc %d): %s" % (rc, row or out[-300:]))
        rc, _, out = run("small", SPEC + FILL + ["pagebudget 4096"], extra=["-trace", "1000003"], n="1")
        if "page budget exceeded (4096 guest pages touched in one run)" not in out:
            fails.append("pagebudget 4096: -trace does not show the abort reason: %s" % out[-400:])
        rc, row, out = run("mid", SPEC + FILL + ["pagebudget 8192"])
        if rc != 0 or not row.endswith(" ok") or "aborted" in row:
            fails.append("pagebudget 8192: setup should complete (rc %d): %s" % (rc, row or out[-300:]))
        for bad in ("pagebudget", "pagebudget 5", "pagebudget 2000000", "pagebudget 8192 x"):
            rc, _, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_pagebudget_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("pagebudget_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
