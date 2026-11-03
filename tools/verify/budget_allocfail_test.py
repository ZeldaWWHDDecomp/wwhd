#!/usr/bin/env python3
"""Self-test of the unit-file directives `accessbudget N`, `timebudget S` and `allocfail TARGET K[,K...]` (harness.cpp,
2026-10-26).

Uses merged lib2_sead_15's constructWorkerCounter 02759928 (allocates 4 bytes through 0273AD10 when r3 = 0):
1. `alloc 0273AD10 r3 0x5E000000 4` + `allocfail 0273AD10 0,1` with `arg! r3 0`: the unit passes, and over traced inputs
   both the failing call (`-> 00000000 (allocfail: call 1)`) and the normal block (`-> 5E000000`) appear, identically on
   both sides of each input.
2. `allocfail` before its alloc line, a non-numeric K, or a missing list exits 2.
3. `accessbudget 1000000` and `timebudget 60` are accepted (unit passes); `accessbudget 5` and `timebudget 5` exit 2.
4. `accessbudget 1000000` with a function that needs more accesses is not exercised here (the default-limit abort path is
   unchanged code).
Writes temporary unit files under tools/verify/units/_budget_allocfail_test/ and removes them.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_budget_allocfail_test")
SPEC = [l for l in open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
        if l.strip() != "real 02759928" and not re.match(r"^(alloc|allocnested|allocfixed)\s+0273AD10\b", l)]
FUNC = "02759928"


def run(name, lines, extra=(), n="200"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_budget_allocfail_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    return r.returncode, (row[0].endswith(" ok") if row else None), out


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    base = SPEC + ["arg! %s r3 0" % FUNC, "alloc 0273AD10 r3 0x5E000000 4"]
    try:
        rc, ok, out = run("af", base + ["allocfail 0273AD10 0,1"], n="400")
        if not ok:
            fails.append("allocfail unit fails: %s" % out[-300:])
        seen = set()
        for i in range(12):
            rc, _, out = run("aft", base + ["allocfail 0273AD10 0,1"], extra=["-seed", "1", "-trace", str(1000003 + i)], n="20")
            got = re.findall(r"alloc 0273AD10 -> ([0-9A-F]{8})", out)
            if len(got) != 2 or got[0] != got[1]:
                fails.append("input %d: sides differ or no alloc: %s" % (i, got))
            elif got:
                seen.add(got[0])
            if "allocfail: call 1" in out:
                seen.add("fail")
        if not {"00000000", "5E000000"} <= seen:
            fails.append("both choices not seen over 12 inputs: %s" % sorted(seen))
        early = SPEC + ["arg! %s r3 0" % FUNC, "allocfail 0273AD10 0,1", "alloc 0273AD10 r3 0x5E000000 4"]
        for bad in (early, base + ["allocfail 0273AD10 x"], base + ["allocfail 0273AD10"],
                    base + ["accessbudget 5"], base + ["timebudget 5"]):
            rc, _, _ = run("bad", bad, n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad[-1]))
        rc, ok, out = run("budgets", base + ["accessbudget 1000000", "timebudget 60"])
        if not ok:
            fails.append("budget directives rejected or unit fails: %s" % out[-300:])
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_budget_allocfail_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("budget_allocfail_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
