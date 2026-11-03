#!/usr/bin/env python3
"""Self-test of the harness option `-watch ADDR[:LEN]` (2026-10-26, AGL#18): every guest byte access to the range, at any
depth (setups and nested `real` callees), is printed to stderr with side, depth and the innermost `real` function.
lib2_sead_15 plus `setup 02759564 02759928 r3=0x5E000000` (real 02759928 clears the word at r3, a nested write):
`-watch 5E000000:4 -n 1` must show 4 `watch W 5E0000xx 00 … in=02759928` lines per side; a malformed -watch exits 2.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_watch_test")
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
F = "02759564"


def run(extra):
    with open(os.path.join(TMP, "w.txt"), "w") as f:
        f.write("\n".join(SPEC + ["setup %s 02759928 r3=0x5E000000" % F]) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_watch_test/w", "-only", F, "-n", "1"] + extra,
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc, out = run(["-watch", "5E000000:4"])
        for side in ("orig", "cand"):
            got = re.findall(r"watch W 5E00000[0-3] 00 side=%s depth=\d+ in=02759928" % side, out)
            if len(got) < 4:
                fails.append("side %s: %d nested writes seen (want >= 4): %s" % (side, len(got), out[-400:]))
        rc, out = run(["-watch", "zz"])
        if rc != 2:
            fails.append("malformed -watch not rejected (rc %d)" % rc)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_watch_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("watch_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
