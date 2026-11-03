#!/usr/bin/env python3
"""Self-test of the unit-file `setup FUNC|* ADDR [rN=VALUE ...]` directive (harness.cpp): before the tested function
the `real` function ADDR runs once from a copy of the entry state (own stack, nested depth), on both sides, and its
memory effects become the starting state (e.g. an original constructor building a singleton the function reads).

Uses the merged spec lib2_sead_15 (real 02759928: a 4-byte counter constructor; clears *r3, allocates when r3 is 0),
tested function 02759564.
1. With `setup 02759564 02759928 r3=0x5E000000` the unit still passes, and a traced input shows the setup ran on
   both sides and returned its receiver (`setup 02759928 -> r3 5E000000` twice).
2. A setup target that is not listed `real` exits 2; malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_setup_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_setup_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
FUNC = "02759564"


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_setup_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if "real 02759928" not in SPEC:
            fails.append("lib2_sead_15 no longer lists real 02759928")
        on = SPEC + ["setup %s 02759928 r3=0x5E000000" % FUNC]
        rc, out = run("on", on, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with setup: %s" % (row or out[-300:]))
        rc, out = run("trace", on, extra=["-seed", "1", "-trace", "1000003"], n="20")
        got = re.findall(r"setup 02759928 -> r3 ([0-9A-F]{8})", out)
        if got != ["5E000000", "5E000000"]:
            fails.append("traced setup returns %s (want 5E000000 on both sides)" % got)
        rc, out = run("notreal", SPEC + ["setup %s 0275992C" % FUNC], n="1")
        if rc != 2:
            fails.append("non-real setup target not rejected (rc %d)" % rc)
        for bad in ("setup %s" % FUNC, "setup %s 02759928 r2=1" % FUNC, "setup %s 02759928 r3=zz" % FUNC, "setup zz 02759928"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_setup_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("setup_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
