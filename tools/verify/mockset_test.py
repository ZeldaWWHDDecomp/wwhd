#!/usr/bin/env python3
"""Self-test of the unit-file `mockset TARGET rDST rVAL rSIZE` directive (harness.cpp).

The generated mock of a byte fill (OSBlockSet C0009990) really stores the low byte of VAL SIZE times at DST and
returns DST, at top level and inside `real` callees. Uses the merged spec lib2_sead_15_boundaries, which models
its OSBlockSet(destination, 0, 84) call with 21 `mockfield C0009990 r3+N u32 0` lines.
1. With those lines replaced by `mockset C0009990 r3 r4 r5` the unit still passes, and a traced input shows the
   fill (val 00, size 54).
2. Nested: `setup * 020012BC r3=0x5F000100` with `real 020012BC` (a thunk: OSBlockSet(r3+8, 0, 0x20)) prints a
   nested call to C0009990 followed by the fill of 5F000108, size 20.
3. Malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_mockset_test/ and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_mockset_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_15_boundaries.txt")).read().splitlines()


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_mockset_test/" + name, "-n", n] + list(extra),
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def rows(out):
    return [l for l in out.splitlines() if len(l) > 9 and l[:8].isalnum() and l[8] == " " and l.startswith("02")]


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        mf = [l for l in SPEC if l.startswith("mockfield C0009990 ")]
        if len(mf) != 21:
            fails.append("lib2_sead_15_boundaries no longer has the 21 OSBlockSet mockfield lines (%d)" % len(mf))
        on = [l for l in SPEC if not l.startswith("mockfield C0009990 ")] + ["mockset C0009990 r3 r4 r5"]
        rc, out = run("on", on)
        bad = [r for r in rows(out) if not r.endswith("ok")]
        if rc or not rows(out) or bad:
            fails.append("unit fails with mockset: %s" % (bad[:2] or out[-300:]))
        rc, out = run("trace", on, extra=["-seed", "1", "-trace", "1000005"], n="20")
        if "mockset C0009990 dst " not in out or " val 00 size 54" not in out:
            fails.append("traced input shows no 84-byte zero fill")
        nested = on + ["real 020012BC", "setup * 020012BC r3=0x5F000100"] + \
            ["field * 0x%08X u32 0xFFFFFFFF 0xFFFFFFFF" % (0x5F000108 + 4 * i) for i in range(8)]
        rc, out = run("nested", nested, extra=["-seed", "1", "-trace", "1000005"], n="20")
        lines = out.splitlines()
        i = next((k for k, l in enumerate(lines) if "nested call C0009990 r3=5F000108" in l), None)
        if i is None or i + 1 >= len(lines) or "mockset C0009990 dst 5F000108 val 00 size 20" not in lines[i + 1]:
            fails.append("nested OSBlockSet not modelled: %s" % [l for l in lines if "C0009990" in l][:3])
        for badl in ("mockset C0009990 r3 r4", "mockset C0009990 r2 r4 r5", "mockset zz r3 r4 r5", "mockset C0009990 r3 r4 r11"):
            rc, _ = run("bad", on + [badl], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, badl))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_mockset_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mockset_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
