#!/usr/bin/env python3
"""Self-test of the unit-file `include PATH` line (specfile.py; verify.py, mutate.py, mkunit.py read expanded specs).
1. A unit = `include` of a copy of lib2_sead_15 (all its lines in tools/verify/units/_include_test/base.txt) gives the
   same per-function result rows as lib2_sead_15 itself (n=300, seeds 1/7).
2. An include placed mid-file keeps textual order: a `poke` in the including file after the include line is applied
   after the included `setup` (trace order setup → poke).
3. A missing include file and an include cycle both stop with an error; a header `# verify:` line inside the included
   file is ignored (the including file's header wins).
Temporary files under tools/verify/units/_include_test/ are removed.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_include_test")
BASE = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read()
F = "02759564"


def run(unit, extra=(), n="300"):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-only", F, "-n", n, "-shards", "1"] + list(extra),
                       cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    return r.returncode, [l for l in out.splitlines() if re.match(r"^[0-9A-F]{8} ", l)], out


def w(name, text):
    open(os.path.join(TMP, name), "w").write(text)


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        w("base.txt", "# verify: -only 0275992C\n" + BASE)
        w("inc.txt", "include _include_test/base.txt\n")
        for s in ("1", "7"):
            rc1, rows1, _ = run("lib2_sead_15", ["-seed", s])
            rc2, rows2, out2 = run("_include_test/inc", ["-seed", s])
            if rc1 or rc2 or rows1 != rows2 or not rows1:
                fails.append("seed %s: include result differs (rc %d/%d): %s vs %s" % (s, rc1, rc2, rows1, rows2 or out2[-300:]))
        w("order.txt", "include _include_test/base.txt\nsetup %s 02759928 r3=0x5E000000\npoke %s 5E000000 u32 0x12345678\n" % (F, F))
        rc, _, out = run("_include_test/order", ["-seed", "1", "-trace", "1000003"], n="5")
        seq = re.findall(r"(setup 02759928 -> r3 5E000000|poke 5E000000 <- 12345678)", out)
        if seq[:2] != ["setup 02759928 -> r3 5E000000", "poke 5E000000 <- 12345678"]:
            fails.append("order after include: %s" % seq[:4])
        w("missing.txt", "include _include_test/nope.txt\n")
        rc, _, out = run("_include_test/missing", n="1")
        if rc == 0 or "not found" not in out:
            fails.append("missing include not rejected (rc %d)" % rc)
        w("cyc_a.txt", "include _include_test/cyc_b.txt\n")
        w("cyc_b.txt", "include _include_test/cyc_a.txt\n")
        rc, _, out = run("_include_test/cyc_a", n="1")
        if rc == 0 or "cycle" not in out:
            fails.append("include cycle not rejected (rc %d)" % rc)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_include_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("include_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
