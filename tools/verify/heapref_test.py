#!/usr/bin/env python3
"""heapref_test.py: a unit fact `callee ADDR rN=heapSIZE` sets the pointee size and the heapref bit (the harness then
compares SIZE bytes behind a heap pointer argument too); the other forms keep their meaning."""
import os, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mkunit

with tempfile.NamedTemporaryFile("w", suffix=".txt", delete=False) as f:
    f.write("callee 028363DC r4=heap48 r5=out12\ncallee 02830D78 r4=heap0x30\ncallee 02800000 r6=inout8 r7=16\n")
u = mkunit.read_unit(f.name)
os.remove(f.name)
want_over = {(0x028363DC, "r4"): 48, (0x028363DC, "r5"): 12, (0x02830D78, "r4"): 0x30, (0x02800000, "r6"): 8, (0x02800000, "r7"): 16}
fails = []
if u.get("over") != want_over:
    fails.append("over %r" % u.get("over"))
if u.get("heapref") != {(0x028363DC, 4), (0x02830D78, 4)}:
    fails.append("heapref %r" % u.get("heapref"))
if u.get("outonly") != {(0x028363DC, 5)} or u.get("fill") != {(0x02800000, 6)}:
    fails.append("outonly/fill %r %r" % (u.get("outonly"), u.get("fill")))
for x in fails:
    print("FAIL " + x)
print("heapref_test: %s" % ("FAIL" if fails else "PASS"))
sys.exit(1 if fails else 0)
