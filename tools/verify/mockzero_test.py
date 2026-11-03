#!/usr/bin/env python3
"""Self-test of the unit-file `mockzero TARGET rDST rSIZE` directive (harness.cpp): the generated mock of DCZeroRange
(C00088C8) zeroes the cache lines it covers, from DST&~31 for ((DST&31)+SIZE+31)&~31 bytes (the port's
coreinit_mem.cpp formula), at top level and inside `real` callees.
Uses the merged spec lib_js_jaudio_4, function 02843E34 (JASCalc bzero: aligned 32-byte multiples go to DCZeroRange).
1. With `mockzero C00088C8 r3 r4` the function still passes, and traced inputs print `mockzero C00088C8 lines ...`
   with a 32-byte-aligned start and a multiple of 32 as size.
2. Malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_mockzero_test/ and removes them. Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_mockzero_test")
SPEC = open(os.path.join(UNITS, "lib_js_jaudio_4.txt")).read().splitlines()
FUNC = "02843E34"


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_mockzero_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        on = SPEC + ["mockzero C00088C8 r3 r4"]
        rc, out = run("on", on, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("function fails with mockzero: %s" % (row or out[-300:]))
        seen = []
        for sd in range(1000000, 1000040):
            rc, out = run("trace", on, extra=["-seed", "1", "-trace", str(sd)], n="40")
            seen += re.findall(r"mockzero C00088C8 lines ([0-9A-F]{8}) size ([0-9A-F]+)", out)
            if len(seen) >= 2:
                break
        if not seen:
            fails.append("no traced input reached the DCZeroRange mock")
        for start, size in seen:
            if int(start, 16) & 31 or int(size, 16) & 31:
                fails.append("mockzero line rounding wrong: start %s size %s" % (start, size))
        for bad in ("mockzero C00088C8 r3", "mockzero C00088C8 r2 r4", "mockzero C00088C8 r3 r4 r5", "mockzero zz r3 r4"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_mockzero_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mockzero_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
