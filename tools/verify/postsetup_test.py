#!/usr/bin/env python3
"""Self-test of the unit-file `postsetup FUNC|* ADDR [rN=VALUE ...]` directive (harness.cpp): the same ABI as `setup`,
but it runs after every `poke` line and before the tested function (a `setup` runs before the pokes), for original
routines whose input is state a setup constructor would clear.

Uses the merged spec lib2_sead_15 (real 02759928: a 4-byte counter constructor, clears *r3), tested function 02759564.
1. `poke 02759564 5E000000 u32 0x12345678` + `postsetup 02759564 02759928 r3=0x5E000000`: the unit passes, and a traced
   input prints, on each side, the poke before the postsetup (which returns 5E000000).
2. The same lines with `setup` instead print the setup before the poke (unchanged ordering of setup).
3. A postsetup target that is not listed `real` exits 2; malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_postsetup_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_postsetup_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
FUNC = "02759564"
POKE = "poke %s 5E000000 u32 0x12345678" % FUNC


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_postsetup_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def order(out):
    return re.findall(r"^\s+(poke 5E000000|postsetup 02759928 -> r3 [0-9A-F]{8}|setup 02759928 -> r3 [0-9A-F]{8})", out, re.M)


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if "real 02759928" not in SPEC:
            fails.append("lib2_sead_15 no longer lists real 02759928")
        post = SPEC + [POKE, "postsetup %s 02759928 r3=0x5E000000" % FUNC]
        rc, out = run("on", post, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with postsetup: %s" % (row or out[-300:]))
        rc, out = run("trace", post, extra=["-seed", "1", "-trace", "1000003"], n="20")
        want = ["poke 5E000000", "postsetup 02759928 -> r3 5E000000"] * 2
        if order(out) != want:
            fails.append("postsetup trace order %s (want %s)" % (order(out), want))
        pre = SPEC + [POKE, "setup %s 02759928 r3=0x5E000000" % FUNC]
        rc, out = run("pre", pre, extra=["-seed", "1", "-trace", "1000003"], n="20")
        want = ["setup 02759928 -> r3 5E000000", "poke 5E000000"] * 2
        if order(out) != want:
            fails.append("setup trace order %s (want %s)" % (order(out), want))
        rc, out = run("notreal", SPEC + ["postsetup %s 0275992C" % FUNC], n="1")
        if rc != 2:
            fails.append("non-real postsetup target not rejected (rc %d)" % rc)
        for bad in ("postsetup %s" % FUNC, "postsetup %s 02759928 r2=1" % FUNC, "postsetup zz 02759928"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_postsetup_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("postsetup_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
