#!/usr/bin/env python3
"""Self-test of the host memory budget: unit directive `hostmembudget MB` and environment WWHD_VERIFY_HOSTMEM_MB
(harness.cpp, 2026-11-01).

A run (setup, pokes, function; each side) during which the harness process's resident size exceeds the budget aborts
the input on the side that ran, like the access, call, page and time budgets, with the reason
`host memory budget exceeded (RSS N MB > B MB in one run)`. Carrier (as pagebudget_test.py): merged lib2_sead_15's
constructWorkerCounter 02759928 with a `real` setup of JASystem::Calc::bzero 02843E34 clearing 0x4000004 bytes at
0x40000000 (16385 guest pages, ~110 MB of host page state; `accessbudget 100000000` for its 67M byte accesses).
1. Default budget (2816 MB): the unit passes, row `ok` without aborts.
2. `hostmembudget 64`: every input aborts on both sides (0/0 compared, `aborted on both sides`, never `only one side
   aborted`); `-trace` shows the reason `host memory budget exceeded (RSS ... MB > 64 MB in one run)`.
3. `hostmembudget 64` with WWHD_VERIFY_HOSTMEM_MB=0 (the environment wins, 0 = off): passes like 1.
4. WWHD_VERIFY_HOSTMEM_MB=64 without a directive: aborts like 2.
5. Malformed or out-of-range lines (`hostmembudget`, `hostmembudget 5`, `hostmembudget 70000`, `hostmembudget 512 x`)
   and an invalid WWHD_VERIFY_HOSTMEM_MB (`abc`, `10`) exit 2.
Writes temporary unit files under tools/verify/units/_hostmembudget_test/ and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_hostmembudget_test")
FUNC = "02759928"
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
FILL = ["real 02843E34", "setup * 02843E34 r3=0x40000000 r4=0x4000004", "accessbudget 100000000"]


def run(name, lines, extra=(), n="3", env=None):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    e = dict(os.environ)
    e.pop("WWHD_VERIFY_HOSTMEM_MB", None)
    if env is not None:
        e["WWHD_VERIFY_HOSTMEM_MB"] = env
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_hostmembudget_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1", "-seed", "1", "--no-cache"] + list(extra), cwd=ROOT, capture_output=True, text=True, env=e)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    return r.returncode, (row[0] if row else ""), out


def aborts_both(rc, row):
    cols = row.split()
    return rc in (0, 1) and "only one side aborted" not in row and "aborted on both sides" in row and cols[2:4] == ["0/0", "0/0"]


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc, row, out = run("default", SPEC + FILL)
        if rc != 0 or not row.endswith(" ok") or "aborted" in row:
            fails.append("default budget: the setup should complete (rc %d): %s" % (rc, row or out[-300:]))
        rc, row, out = run("small", SPEC + FILL + ["hostmembudget 64"])
        if not aborts_both(rc, row):
            fails.append("hostmembudget 64: every input should abort on both sides (rc %d): %s" % (rc, row or out[-300:]))
        rc, _, out = run("small", SPEC + FILL + ["hostmembudget 64"], extra=["-trace", "1000003"], n="1")
        if "host memory budget exceeded (RSS " not in out or " MB > 64 MB in one run)" not in out:
            fails.append("hostmembudget 64: -trace does not show the abort reason: %s" % out[-400:])
        rc, row, out = run("small", SPEC + FILL + ["hostmembudget 64"], env="0")
        if rc != 0 or not row.endswith(" ok") or "aborted" in row:
            fails.append("WWHD_VERIFY_HOSTMEM_MB=0 should switch the budget off (rc %d): %s" % (rc, row or out[-300:]))
        rc, row, out = run("default", SPEC + FILL, env="64")
        if not aborts_both(rc, row):
            fails.append("WWHD_VERIFY_HOSTMEM_MB=64: every input should abort on both sides (rc %d): %s" % (rc, row or out[-300:]))
        for bad in ("hostmembudget", "hostmembudget 5", "hostmembudget 70000", "hostmembudget 512 x"):
            rc, _, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
        for bad in ("abc", "10"):
            rc, _, _ = run("default", SPEC, n="1", env=bad)
            if rc != 2:
                fails.append("not rejected (rc %d): WWHD_VERIFY_HOSTMEM_MB=%s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_hostmembudget_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("hostmembudget_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
