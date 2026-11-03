#!/usr/bin/env python3
"""Self-test of the unit-file `mocklookup TARGET rIDX BASE COUNT` directive (harness.cpp).

The generated mock of a read-only word-table lookup returns the big-endian word at BASE + 4*rIDX, at top level and
inside `real` callees; an index >= COUNT aborts the input. Uses the merged spec lib2_sead_11 (-only 0274B40C) as a
carrier and a `setup` of the real texture initializer 027BE930 (format r5 = 0x1D), which calls 02786348, a
one-load lookup into the image table 101F8C24 (entry 0x1D = 0x1A, GX2 R8G8B8A8_UNORM).
1. With `mocklookup 02786348 r3 101F8C24 40` the traced input shows the nested lookup returning 0000001A.
2. With COUNT 10 (index 0x1D outside the table) the input aborts with the documented reason.
3. Malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_mocklookup_test/ and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_mocklookup_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_11.txt")).read().splitlines()
ONLY = "0274B40C"
TEX = ["real 027BE930", "real 027BE7F0", "real 027BE74C", "real 027BE744", "real 0278635C", "real 027BFF9C",
       "mocksurfacesize C00060D0", "stable 101F8C24 0x100", "stable 101F9208 0x1D0",
       "setup * 027BE930 r3=0x5F000000 r4=1 r5=0x1D r6=64 r7=64 r8=1 r9=1 r10=4"]


def run(name, lines, extra=(), n="20"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_mocklookup_test/" + name, "-n", n, "-only", ONLY] +
                       list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        base = [l for l in SPEC if not l.startswith("mockfield C00060D0 ")]
        rc, out = run("on", base + TEX + ["mocklookup 02786348 r3 101F8C24 40"], extra=["-seed", "1", "-trace", "1000005"])
        if "nested call 02786348 r3=0000001D" not in out or "mocklookup 02786348 index 1D -> 0000001A" not in out:
            fails.append("nested lookup not modelled: %s" % [l for l in out.splitlines() if "02786348" in l][:3])
        rc, out = run("range", base + TEX + ["mocklookup 02786348 r3 101F8C24 10"], extra=["-seed", "1", "-trace", "1000005"])
        if "mocklookup: index outside the declared table" not in out:
            fails.append("out-of-table index not aborted")
        for bad in ("mocklookup 02786348 r3 101F8C24", "mocklookup 02786348 r2 101F8C24 40", "mocklookup 02786348 r3 101F8C24 0",
                    "mocklookup 02786348 r3 101F8C24 40 extra"):
            rc, _ = run("bad", base + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_mocklookup_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mocklookup_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
