#!/usr/bin/env python3
"""Runtime test of stack-passed integer arguments at a candidate's entry (gabi.h int_arg).

EABI passes integer arguments 9.. in the caller's parameter area at entry SP+8, SP+12, ...; before
2026-10-11 ArgFrom read them from r11, r12, ... (FW-parts_buttons report, 026F5C08). The test writes a
temporary candidate for dCcS::ChkAtTgHitAfterCross 025185F8 (a member function whose 9th integer
argument, the target's gauge status, is read with `lwz r12, 8(r1)`) that declares all nine arguments
and lets the entry adapter fetch the 9th, and runs it against the original at 2,000 inputs, seeds 1
and 7. Writes wwhd_src/d/_stackarg_test.cpp and units/_stackarg_test.txt and removes them.
Exit 0 when every input passes.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
SRC = os.path.join(ROOT, "wwhd_src", "d", "_stackarg_test.cpp")
SPEC = os.path.join(HERE, "units", "_stackarg_test.txt")

CODE = r"""
#include "wwhd.h"
/* raw test candidate (not readable style): every argument through the entry adapter */
static u32 rd(u8* p, u32 off) { return *gabi::at<be<u32>>(gabi::ea(p) + off); }
static void wr(u8* p, u32 off, u32 v) { *gabi::at<be<u32>>(gabi::ea(p) + off) = v; }
struct StackArgTest_c {
    u32 chk(u32 setAt, u32 setTg, u8* atInf, u8* tgInf, u8* atStts, u8* tgStts, u8* atGStts, u8* tgGStts);
};
u32 StackArgTest_c::chk(u32 setAt, u32 setTg, u8* atInf, u8* tgInf, u8* atStts, u8* tgStts, u8* atGStts,
                        u8* tgGStts) {
    u32 tgApid = rd(tgStts, 0x10);
    u32 atApid = rd(atStts, 0x10);
    if (setAt) {
        wr(atGStts, 0xC, tgApid);
        if ((rd(atInf, 0x50) & 1) && rd(atGStts, 0x10) == rd(tgStts, 0x10)) return 1;
    }
    if (setTg) {
        wr(tgGStts, 0x14, atApid);
        if ((rd(tgInf, 0x94) & 2) && !(rd(atInf, 0x50) & 4) && rd(tgGStts, 0x18) == rd(atStts, 0x10)) return 1;
    }
    return 0;
}
VERIFY(0x025185F8, &StackArgTest_c::chk);
"""


def main():
    open(SRC, "w").write(CODE)
    open(SPEC, "w").write("src wwhd_src/d/_stackarg_test.cpp\n")
    fails = []
    try:
        for seed in ("1", "7"):
            r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_stackarg_test", "-n", "2000", "-seed", seed],
                               cwd=ROOT, capture_output=True, text=True)
            row = [l for l in r.stdout.splitlines() if l.startswith("025185F8")]
            if r.returncode or not row or not re.search(r"\b2000/2000\b.*\sok\b", row[0]):
                fails.append("seed %s: %s" % (seed, row[0] if row else (r.stdout + r.stderr)[-800:]))
    finally:
        for p in (SRC, SPEC):
            if os.path.exists(p):
                os.remove(p)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_stackarg_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("gabi_stackarg_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
