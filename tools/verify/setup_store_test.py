#!/usr/bin/env python3
"""Self-test of the unit-file `setup`/`postsetup` operands `->ADDR` (store the returned r3 at ADDR) and `sp+OFF=VALUE`
(a word in the routine's caller frame, for stack parameters) (harness.cpp run_setups).

Uses the merged spec lib2_sead_15 (real 02759928: a 4-byte counter constructor; it clears *r3 and returns r3, or
allocates through 0273AD10 when r3 = 0), tested function 02759564.
1. `setup F 02759928 r3=0x5E000000 sp+8=0x11 sp+0xC=0x22 ->0x5E000010` then `setup F 02759928 r3=*5E000010`: the unit
   passes, and a traced input prints on each side the two stack words, the store of 5E000000 at 5E000010, and the
   second setup reading it back (-> r3 5E000000).
2. A producer that returns 0 (r3=0 with `nestedret 0273AD10 0`) with `->ADDR` exits 2.
3. Malformed operands exit 2 (unaligned or missing ADDR, a second store, sp+ offset < 8 or unaligned, missing value).
Writes temporary unit files under tools/verify/units/_setup_store_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_setup_store_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
FUNC = "02759564"


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_setup_store_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if "real 02759928" not in SPEC:
            fails.append("lib2_sead_15 no longer lists real 02759928")
        good = SPEC + ["setup %s 02759928 r3=0x5E000000 sp+8=0x11 sp+0xC=0x22 ->0x5E000010" % FUNC,
                       "setup %s 02759928 r3=*5E000010" % FUNC]
        rc, out = run("on", good, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with the new operands: %s" % (row or out[-300:]))
        rc, out = run("trace", good, extra=["-seed", "1", "-trace", "1000003"], n="20")
        got = re.findall(r"^\s+(setup 02759928 (?:sp\+0x[0-9A-F]+ <- [0-9A-F]{8}|->[0-9A-F]{8} <- [0-9A-F]{8}|-> r3 [0-9A-F]{8}))", out, re.M)
        want = ["setup 02759928 sp+0x8 <- 00000011", "setup 02759928 sp+0xC <- 00000022", "setup 02759928 -> r3 5E000000",
                "setup 02759928 ->5E000010 <- 5E000000", "setup 02759928 -> r3 5E000000"] * 2
        if got != want:
            fails.append("trace %s (want %s)" % (got, want))
        rc, out = run("zero", SPEC + ["nestedret 0273AD10 0", "setup %s 02759928 r3=0 ->0x5E000010" % FUNC], n="1")
        if rc != 2 or "stores r3 = 0" not in out:
            fails.append("a zero result with ->ADDR not rejected (rc %d): %s" % (rc, out[-300:]))
        for bad in ("->", "->5E000012", "r3=0x5E000000 ->5E000010 ->5E000020", "sp+4=1", "sp+9=1", "sp+8=", "sp+8",
                    "sp+0x104=1"):
            rc, _ = run("bad", SPEC + ["setup %s 02759928 %s" % (FUNC, bad)], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_setup_store_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("setup_store_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
