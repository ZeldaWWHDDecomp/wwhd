#!/usr/bin/env python3
"""Self-test of the `setup`/`postsetup` operands `fN=FLOAT` and `fN=*ADDR` (harness.cpp, 2026-10-26, AGL#18 culling volume).

Runs the original 0278CB8C (camera projection setter: stores f1 at r3+0x30 and f2 at r3+0x34) as a setup with
f1=1.0 and f2=*5E300000 (an f32 word 0x42C80000 = 100.0 placed by `field`). Two later setups of real 02759928 with
r3=*5E000030 / r3=*5E000034 return those words, so the trace must show `-> r3 3F800000` and `-> r3 42C80000`, and the
`fN = ` operand lines. Malformed operands (f0, f14, f1=, f1=abc) exit 2. Uses merged lib2_sead_15 (tested function
02759564 is unaffected). Temporary files under tools/verify/units/_setup_fpr_test/ are removed.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_setup_fpr_test")
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
F = "02759564"
LINES = ["real 0278CB8C 028F423C 028E8970", "field * 0x5E300000 u32 0x42C80000",
         "setup %s 0278CB8C r3=0x5E000000 r4=0x5E100000 r5=0x5E200000 f1=1.0 f2=*5E300000 f3=0.7853982 f4=1.7777778" % F,
         "setup %s 02759928 r3=*5E000030" % F, "setup %s 02759928 r3=*5E000034" % F]


def run(name, lines, extra=(), n="3"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_setup_fpr_test/" + name, "-only", F, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc, out = run("ok", SPEC + LINES, extra=["-seed", "1", "-trace", "1000003"])
        for want in ("setup 0278CB8C f1 = 3F800000", "setup 0278CB8C f2 = 42C80000", "setup 02759928 -> r3 3F800000",
                     "setup 02759928 -> r3 42C80000"):
            if out.count(want) < 2:
                fails.append("missing on both sides: %s" % want)
        for bad in ("f0=1", "f14=1", "f1=", "f1=abc"):
            rc, _ = run("bad", SPEC + [LINES[0], "setup %s 0278CB8C r3=0x5E000000 %s" % (F, bad)], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_setup_fpr_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("setup_fpr_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
