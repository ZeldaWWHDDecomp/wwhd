#!/usr/bin/env python3
"""Self-test of (post)setup register operands that read guest memory when the line runs (harness.cpp, 2026-10-25,
AGL#3 request): `rN=*ADDR` (the word at ADDR) and `rN=*(*ADDR+OFF)` (the word at that word plus OFF), for singletons an
earlier original setup allocated. A dereference that reads 0 exits 2 (the producer did not run).
Uses lib2_sead_15 (real 02759928: clears *r3), tested function 02759564, with `field` words as the pointer chain.
Writes temporary unit files under tools/verify/units/_setup_deref_test/ and removes them. Exit 0 when all pass."""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_setup_deref_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
FUNC = "02759564"
CHAIN = ["field * 0x5E000100 u32 0x5E000000 0x5E000000", "field * 0x5E000104 u32 0x5E000108 0x5E000108",
         "field * 0x5E000108 u32 0x5E000000 0x5E000000"]


def run(name, lines, extra=(), n="20"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_setup_deref_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        for name, op in (("one", "r3=*5E000100"), ("two", "r3=*(*5E000104+0)")):
            lines = SPEC + CHAIN + ["setup %s 02759928 %s" % (FUNC, op)]
            rc, out = run(name, lines, n="300")
            row = [l for l in out.splitlines() if l.startswith(FUNC)]
            if rc or not row or not row[0].endswith("ok"):
                fails.append("%s: unit fails: %s" % (op, row or out[-300:]))
            rc, out = run(name + "t", lines, extra=["-seed", "1", "-trace", "1000004"])
            got = re.findall(r"setup 02759928 -> r3 ([0-9A-F]{8})", out)
            if got != ["5E000000", "5E000000"]:
                fails.append("%s: traced r3 %s (want 5E000000 on both sides)" % (op, got))
        rc, out = run("zero", SPEC + ["setup %s 02759928 r3=*5E000200" % FUNC], n="1")
        if rc != 2:
            fails.append("zero dereference not rejected (rc %d)" % rc)
        for bad in ("r3=*zz", "r3=*(*5E000100+", "r3=*(5E000100+4)"):
            rc, _ = run("bad", SPEC + ["setup %s 02759928 %s" % (FUNC, bad)], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_setup_deref_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("setup_deref_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
