#!/usr/bin/env python3
"""Self-test of the unit-file `cr FUNC|* BIT V[,V...]` directive (harness.cpp).

Generated inputs start with every CR bit clear. A GHS variadic function saves f1-f8 into its register
save area only when the caller set CR bit 6 (cr1.eq; PPC EABI "floats passed in FPRs"), so without the
directive that path is never taken.

Uses the merged unit m_Do_printf: OSReport_Error 025F2710 (`bne cr1` over eight stfd; the candidate reads
gabi::cpu->cr[6]).
1. `cr 025F2710 6 0,1` (and the `*` form) covers more blocks than no directive, and every input still passes.
2. A candidate that ignores the bit (`floats = false`) passes without the directive and fails with it.
3. Malformed lines are rejected (exit 2).
Writes temporary unit files under tools/verify/units/_cr_test/ and a temporary source next to
m_Do_printf.cpp, and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_cr_test")
FUNC = "025F2710"
SRC = "wwhd_src/m_Do/m_Do_printf.cpp"
MUT = "wwhd_src/m_Do/_cr_test_m_Do_printf.cpp"
SPEC = [l for l in open(os.path.join(UNITS, "m_Do_printf.txt")).read().splitlines()]


def run(name, lines, src=SRC, n="300"):
    body = [(("src " + src) if l.strip() == "src " + SRC else l) for l in SPEC]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_cr_test/" + name, "-only", FUNC, "-n", n],
                       cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    m = re.search(r"(\d+)/(\d+)\s+\d+/\d+\s+(\d+)/(\d+)", row[0]) if row else None
    return r.returncode, (int(m.group(1)), int(m.group(2)), int(m.group(3))) if m else None, row


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc0, none, row0 = run("none", [])
        rc1, on, row1 = run("on", ["cr %s 6 0,1" % FUNC])
        rc2, star, row2 = run("star", ["cr * 6 0,1"])
        if rc0 or rc1 or rc2 or not none or not on or none[0] != none[1] or on[0] != on[1] or on != star:
            fails.append("runs differ or fail:\n%s\n%s\n%s" % (row0, row1, row2))
        else:
            print("coverage of %s: %d blocks without cr, %d with cr 6 0,1" % (FUNC, none[2], on[2]))
        if not fails and on and none and on[2] <= none[2]:
            fails.append("cr 6 0,1 does not reach the float-save block: %s vs %s" % (row0, row1))
        text = open(os.path.join(ROOT, SRC)).read()
        mt = text.replace("bool floats = gabi::cpu->cr[6] != 0;", "bool floats = false;", 1)
        if mt == text:
            fails.append("mutation site not found in %s" % SRC)
        else:
            open(os.path.join(ROOT, MUT), "w").write(mt)
            rc3, _, row3 = run("mut_none", [], src=MUT)
            rc4, _, row4 = run("mut_on", ["cr %s 6 0,1" % FUNC], src=MUT)
            if rc3:
                fails.append("bit-ignoring candidate fails without the directive (test lost its point): %s" % row3)
            if not rc4:
                fails.append("bit-ignoring candidate passes with cr 6 0,1: %s" % row4)
        for bad in ("cr %s 32 1" % FUNC, "cr %s 6 2" % FUNC, "cr %s 6" % FUNC, "cr %s x 1" % FUNC,
                    "cr %s 6 0,1 extra" % FUNC):
            rc, _, _ = run("bad", [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_cr_test"), ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    for f in fails:
        print("FAIL", f)
    print("cr_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
