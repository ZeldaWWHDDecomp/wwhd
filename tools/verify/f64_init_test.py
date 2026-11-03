#!/usr/bin/env python3
"""f64_init_test.py: `double`/`f64` (and 64-bit integer) table initialisers are checked as 8-byte big-endian values
against the image: zero tables in .bss (SEAD#14's sMathDoubleDefaults[82] @104A04D0, sQuaternionDoubleDefaults[4]
@104A0770, sCompositeDoubleDefaults[47] @104A09C8) accept zeros and reject a non-zero entry; initialised doubles in
.rodata (0x4330000080000000 @10000208, 0.5 @1002B7B0, 1.0 @10007658) accept the right value and reject a wrong one."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import autoglue
bad = []
def ok(elem, addr, dims, init):
    try:
        autoglue.check_table("t", addr, elem, dims, init, "", {}, "test", "test.cpp", "")
        return True
    except autoglue.GlueError:
        return False
for addr, n in ((0x104A04D0, 82), (0x104A0770, 4), (0x104A09C8, 47)):
    for elem in ("f64", "double"):
        if not ok(elem, addr, [n], "{" + ", ".join(["0.0"] * n) + "}"): bad.append("%s zeros at %08X rejected" % (elem, addr))
        if ok(elem, addr, [n], "{" + ", ".join(["0.0"] * (n - 1) + ["1.0"]) + "}"): bad.append("%s non-zero at %08X accepted" % (elem, addr))
if not ok("f64", 0x10000208, [1], "{4503601774854144.0}"): bad.append("0x4330000080000000 rejected")
if ok("f64", 0x10000208, [1], "{4503599627370496.0}"): bad.append("wrong double accepted")
if not ok("double", 0x1002B7B0, [1], "{0.5}"): bad.append("0.5 rejected")
if not ok("f64", 0x10007658, [1], "{1.0}"): bad.append("1.0 rejected")
if ok("f64", 0x10007658, [1], "{1.0000001}"): bad.append("1.0000001 accepted for 1.0")
if not ok("u64", 0x10000208, [1], "{0x4330000080000000}"): bad.append("u64 rejected")
if not ok("f32", 0x10007658, [2], "{1.875f, 0.0f}"): bad.append("f32 view of the same bytes rejected (regression)")
for b in bad: print("FAIL", b)
print("f64_init_test: %s" % ("FAIL" if bad else "PASS"))
sys.exit(1 if bad else 0)
