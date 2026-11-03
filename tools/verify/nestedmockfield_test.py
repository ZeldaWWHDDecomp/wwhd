#!/usr/bin/env python3
"""Self-test of the unit-file `nestedmockfield TARGET` directive (harness.cpp).

`mockfield` lines describe what a generated mock writes through its arguments; they applied to top-level calls only.
`nestedmockfield TARGET` makes TARGET's mockfield lines also apply when TARGET is called inside a `real` callee.
Carrier: the merged spec lib2_sead_11 (-only 0274B40C) plus a `setup` of the real 027B6C60, which tail-calls
GX2CalcColorBufferAuxInfo (C00060A0) when its +4 word is 6, with r4/r5 = object+0xA0/+0xA4 as size/alignment out-pointers (the shipping port's HLE writes
0x1000 and 0x100 there).
1. With `nestedmockfield C00060A0` the traced input shows both nested writes.
2. Without it, no nested write is made (unchanged behaviour).
3. Malformed lines exit 2.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_nestedmockfield_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_11.txt")).read().splitlines()
ONLY = "0274B40C"
BASE = ["real 027B6C60", "field * 0x5F000004 u32 6 6", "setup * 027B6C60 r3=0x5F000000",
        "mockfield C00060A0 r4+0 u32 0x1000,0x1000", "mockfield C00060A0 r5+0 u32 0x100,0x100"]


def run(name, lines, extra=(), n="20"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_nestedmockfield_test/" + name, "-n", n, "-only", ONLY] +
                       list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc, out = run("on", SPEC + BASE + ["nestedmockfield C00060A0"], extra=["-seed", "1", "-trace", "1000005"])
        if "nested mockfield C00060A0 5F0000A0 <- 00001000" not in out or "nested mockfield C00060A0 5F0000A4 <- 00000100" not in out:
            fails.append("nested writes missing: %s" % [l for l in out.splitlines() if "C00060A0" in l][:4])
        rc, out = run("off", SPEC + BASE, extra=["-seed", "1", "-trace", "1000005"])
        if "nested mockfield" in out:
            fails.append("nested write made without nestedmockfield")
        if "nested call C00060A0" not in out:
            fails.append("carrier does not reach the nested call")
        for bad in ("nestedmockfield", "nestedmockfield C00060A0 extra", "nestedmockfield zz"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_nestedmockfield_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("nestedmockfield_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
