#!/usr/bin/env python3
"""Self-test of the unit-file `opbudget BYTES` directive (harness.cpp; 2026-11-01, the per-operation
byte cap of the mockset, mockzero and memmove models (default 20,000,000 bytes) raised for one spec.

Uses the merged spec c_lib with cLib_memSet 0200ECCC (r4 &= 0xFF, then a tail call to OSBlockSet C0009990) modelled by
`mockset C0009990 r3 r4 r5`, called with r3 = 0x5F000000 and r5 = 0x1600000 (23,068,672 bytes, over the default cap);
`accessbudget 100000000` covers the one access per byte.
1. Without `opbudget` the traced first run (dictionary pre-run 0, seed 7777) aborts with `memset exceeds guest operation budget`, and the row is not ok.
2. With `opbudget 0x4000000` the traced input shows the fill (`mockset C0009990 dst 5F000000 ... size 1600000`) and no
   abort, and the row is ok.
3. `-v` prints `opbudget: memset C0009990 dst 5F000000 size 1600000 lr ...` on stderr (with `opbudget 0x4000000`);
   without -v no `opbudget:` line is printed.
4. Invalid lines exit 2: 0x80000000 (over 1 GB), 0, 1073741825, -1, 12abc, 0x, a missing operand, an extra operand,
   two `opbudget` lines. 1073741824 and decimal 67108864 are accepted.
Writes temporary unit files under tools/verify/units/_opbudget_test/ and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_opbudget_test")
FUNC = "0200ECCC"
TRACE = "7777"  # -seed 1: dictionary pre-run 0 (traced), the first run of the function
BASE = open(os.path.join(UNITS, "c_lib.txt")).read().splitlines() + [
    "mockset C0009990 r3 r4 r5",
    "accessbudget 100000000",
    "arg! %s r3 0x5F000000" % FUNC,
    "arg! %s r5 0x1600000" % FUNC,
]


def run(name, lines, extra=(), n="3"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_opbudget_test/" + name, "-only", FUNC, "-n", n,
                        "-seed", "1", "-nulltrap", "-shards", "1", "-buildj", "4"] + list(extra),
                       cwd=ROOT, capture_output=True, text=True, errors="replace")
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    return r.returncode, r.stdout, r.stderr, (row[0] if row else "")


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        # 1. default cap: abort
        rc, out, err, _ = run("off", BASE, ["-trace", TRACE])
        ab = [l.strip() for l in out.splitlines() if "aborted (" in l]
        if not any("memset exceeds guest operation budget" in l for l in ab):
            fails.append("1: no `memset exceeds guest operation budget` abort without opbudget: %s" % (ab[:2] or out[-400:] + err[-400:]))
        else:
            print("1 ok:", ab[0])
        rc, out, err, row = run("off", BASE)
        if not row or row.endswith(" ok"):
            fails.append("1: row without opbudget should not be ok: %r" % row)
        else:
            print("1 ok: row", row)
        if "opbudget:" in err:
            fails.append("1: `opbudget:` line printed without -v")
        # 2. raised cap: the fill runs
        on = BASE + ["opbudget 0x4000000"]
        rc, out, err, _ = run("on", on, ["-trace", TRACE])
        fill = [l.strip() for l in out.splitlines() if "mockset C0009990 dst 5F000000" in l]
        if not fill or not fill[0].endswith("size 1600000") or "exceeds guest operation budget" in out:
            fails.append("2: no 0x1600000-byte fill with opbudget 0x4000000: %s" % (fill[:2] or out[-400:] + err[-400:]))
        else:
            print("2 ok:", fill[0])
        rc, out, err, row = run("on", on)
        if rc or not row.endswith(" ok"):
            fails.append("2: row with opbudget 0x4000000 not ok (rc %d): %r %s" % (rc, row, err[-300:]))
        else:
            print("2 ok: row", row)
        if "opbudget:" in err:
            fails.append("2: `opbudget:` line printed without -v")
        # 3. -v lists the large fill
        rc, out, err, _ = run("on", on, ["-v"], n="1")
        big = [l for l in err.splitlines() if l.startswith("opbudget: memset C0009990 dst 5F000000 size 1600000 lr ")]
        if not big:
            fails.append("3: no `opbudget: memset ...` line under -v: %s" % err[-400:])
        else:
            print("3 ok:", big[0])
        rc, out, err, _ = run("off", BASE, ["-v"], n="1")
        big = [l for l in err.splitlines() if l.startswith("opbudget: memset C0009990 dst 5F000000 size 1600000 ") and "(over the budget)" in l]
        if not big:
            fails.append("3: no `(over the budget)` line under -v without opbudget: %s" % err[-400:])
        else:
            print("3 ok:", big[0])
        # 4. parser
        for bad in (["opbudget 0x80000000"], ["opbudget 0"], ["opbudget 1073741825"], ["opbudget -1"], ["opbudget 12abc"],
                    ["opbudget 0x"], ["opbudget"], ["opbudget 0x4000000 7"], ["opbudget 0x4000000", "opbudget 0x4000000"]):
            rc, out, err, _ = run("bad", BASE + bad, n="1")
            if rc != 2 or "invalid opbudget directive" not in err:
                fails.append("4: not rejected (rc %d): %s" % (rc, bad))
        print("4 ok: invalid values exit 2" if not any(f.startswith("4:") for f in fails) else "4: see FAIL lines")
        for good in ("opbudget 1073741824", "opbudget 67108864"):
            rc, out, err, row = run("good", BASE + [good], n="1")
            if rc == 2:
                fails.append("4: valid line rejected: %s (%s)" % (good, err[-200:]))
        print("4 ok: 1073741824 and 67108864 accepted" if not any("valid line rejected" in f for f in fails) else "4: see FAIL lines")
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_opbudget_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("opbudget_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
