#!/usr/bin/env python3
"""fixture_dyn_test.py: unit `callee` facts `fixture` (a mock-only target without generated code) and
`rN=dynK*rM[:CAP]` (pointee size K*rM per call, at most CAP, default 4096) parse as documented."""
import os, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mkunit
with tempfile.NamedTemporaryFile("w", suffix=".txt", delete=False) as f:
    f.write("callee 0F000100 fixture r3=0 r4=20\ncallee C0006A40 r5=dyn4*r4\ncallee 02800000 r6=dyn8*r7:0x100 r3=heap16\ncallee 02888864 r6=cstr:640\n")
u = mkunit.read_unit(f.name)
os.remove(f.name)
want = {"fixture": {0x0F000100},
        "dyn": {(0xC0006A40, 5): (4, 4), (0x02800000, 6): (8, 7), (0x02888864, 6): (0, 0xFF)},
        "over": {(0x0F000100, "r3"): 0, (0x0F000100, "r4"): 20, (0xC0006A40, "r5"): 4096, (0x02800000, "r6"): 0x100, (0x02800000, "r3"): 16, (0x02888864, "r6"): 640},
        "heapref": {(0x02800000, 3), (0x02888864, 6)}}
bad = [k for k in want if u.get(k) != want[k]]
for k in bad: print("FAIL %s: %r" % (k, u.get(k)))
print("fixture_dyn_test: %s" % ("FAIL" if bad else "PASS"))
sys.exit(1 if bad else 0)
