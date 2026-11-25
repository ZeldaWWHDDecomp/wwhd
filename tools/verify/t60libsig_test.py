#!/usr/bin/env python3
"""Self-test: a c_lib frame-split ease whose frame entry was opened by one call is continued, on a LATER step of the
same 30 Hz frame, by a call with OTHER parameters (c_lib_callsig_t60.h).

The integer eases on a general step length (c_lib.cpp t60IntEntry: addCalcAngleS, addCalcAngleS2,
addCalcAngleL and t60Chase = chaseUC / chaseS / chaseAngleS) keep the frame's start per value and on the tick30 step
write the original one step from that start (t60PinFull). In the game a procedure change on an earlier step hands the
value to another call site with other limits (Link's procMoveTurn -> procMove, mdC at N 4): without the call-signature
check the tick30 step wrote the SECOND call's full step from the FIRST call's frame start (the value jumped back).
The float eases (addCalc*, addCalcPos*, chaseF, chasePos*) keep no frame entry (each step scales its own parameters
from the current value): the pattern cannot occur there, they are not part of this test.

Test variants (a copy of c_lib.cpp), on every step after the frame's first (dt != 1 and !first_step), with the second
call's smaller limit (maxStep / step 1/16 of the first):
  site: the call also comes from another guest call site (the guest LR the key reads, + 0x40), as after the game's
        procedure change (a nested call of the marked function from its own body would run the original);
  same: the ease's own call takes that limit (the same call site with other limits: the rope swing's maxStep from
        cos(swingZ)); the site key keeps the frame's entry and its pin there by design (2026-11-24).
The one-step reference is the first call alone (one original step from the start, A); fixed inputs: value 0, target
far, limits fixed. Restarted (site): the second call starts a partial frame at the first step's value: start + A/N +
B*(N-1)/N, within |A - start| * 3/4 of the reference (N 2: A/2 - B/2 off, N 4: 3A/4 - 3B/4 off). Kept (the NC:
WWHD_T60_LIBSIG_NEGCTRL=1 in the game, the check off; and the same-site variant): the tick30 step writes start + B
(B = A/16) from the first call's start, A - B = 15A/16 off: fails.
For each of the six functions: 1. site passes T-02 at N 2 and N 4; 2. its NC fails at N 2 and N 4 on the value;
3. same (the entry kept) fails at N 2 on the value, as the NC does.
Temporary files go to tools/verify/units/_t60_test/ (removed at the end). Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import specfile  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_t60_test")
SRC = "wwhd_src/SSystem/SComponent/c_lib.cpp"
HDR = "wwhd_src/include/SSystem/SComponent/c_lib_callsig_t60.h"
INC = '#include "SSystem/SComponent/c_lib_callsig_t60.h"\n'
NC_KEY = "    if (t60cs_nc())\n        return;\n"
N = os.environ.get("T60_TEST_N", "200")
SEED = os.environ.get("T60_TEST_SEED", "1")
LATER = "    if (t60::dt() != 1.0f && !t60::first_step()) { %s } /* t60libsig_test: the second call's parameters */\n"
SITE = "gabi::cpu->lr = (u32)gabi::cpu->lr + 0x40u; /* another call site: its guest LR */"

# addr: (signature line, second call's parameters, steering, value type for the tolerance, tolerance = 3/4 of A)
CASES = {
    "0200F378": ("s16 cLib_addCalcAngleS(be<s16>* pValue, s32 target, s32 scale, s32 maxStep, s32 minStep) {\n",
                 "maxStep = 0x80;", ["field 0200F378 r3+0 s16 0 0", "arg! 0200F378 r4 16384 16384",
                                     "arg! 0200F378 r5 4 4", "arg! 0200F378 r6 2048 2048", "arg! 0200F378 r7 0 0"],
                 "a16", 1536),
    "0200F428": ("void cLib_addCalcAngleS2(be<s16>* pValue, s32 target, s32 scale, s32 maxStep) {\n",
                 "maxStep = 0x80;", ["field 0200F428 r3+0 s16 0 0", "arg! 0200F428 r4 16384 16384",
                                     "arg! 0200F428 r5 4 4", "arg! 0200F428 r6 2048 2048"],
                 "a16", 1536),
    "0200F474": ("s32 cLib_addCalcAngleL(be<s32>* pValue, s32 target, s32 scale, s32 maxStep, s32 minStep) {\n",
                 "maxStep = 0x800;", ["field 0200F474 r3+0 s32 0 0", "arg! 0200F474 r4 262144 262144",
                                      "arg! 0200F474 r5 4 4", "arg! 0200F474 r6 32768 32768", "arg! 0200F474 r7 0 0"],
                 "s32", 24576),
    "0200F4FC": ("s32 cLib_chaseUC(be<u8>* pValue, s32 target, s32 step) {\n",
                 "step = 4;", ["field 0200F4FC r3+0 u8 0 0", "arg! 0200F4FC r4 240 240", "arg! 0200F4FC r5 64 64"],
                 "u8", 48),
    "0200F564": ("s32 cLib_chaseS(be<s16>* pValue, s32 target, s32 step) {\n",
                 "step = 0x80;", ["field 0200F564 r3+0 s16 0 0", "arg! 0200F564 r4 16384 16384",
                                  "arg! 0200F564 r5 2048 2048"],
                 "s16", 1536),
    "0200F8D0": ("s32 cLib_chaseAngleS(be<s16>* pValue, s32 target, s32 step) {\n",
                 "step = 0x80;", ["field 0200F8D0 r3+0 s16 0 0", "arg! 0200F8D0 r4 16384 16384",
                                  "arg! 0200F8D0 r5 2048 2048"],
                 "a16", 1536),
}
BASE = [l.rstrip("\n") for l in specfile.expanded_lines(os.path.join(UNITS, "_t60", "c_lib.txt"))
        if l.split()[:1] in (["callee"], ["real"])]


def write(name, func, nc, mode="site"):
    sig, later, steer, ty, tol = CASES[func]
    text = open(os.path.join(ROOT, SRC)).read()
    assert sig in text and INC in text, "c_lib.cpp changed: update t60libsig_test.py (%s)" % func
    text = text.replace(sig, sig + LATER % (later if mode == "same" else later + " " + SITE), 1)
    if nc:
        hdr = open(os.path.join(ROOT, HDR)).read()
        assert NC_KEY in hdr, "c_lib_callsig_t60.h changed: update t60libsig_test.py"
        open(os.path.join(TMP, name + "_h.h"), "w").write(hdr.replace(NC_KEY, "    return; /* NC */\n"))
        text = text.replace(INC, '#include "%s"\n' % os.path.join(TMP, name + "_h.h"), 1)  # (verify compiles a glued copy)
    src = os.path.join(TMP, name + ".cpp")
    open(src, "w").write(text)
    spec = ["# GENERATED by t60libsig_test.py", "src " + os.path.relpath(src, ROOT)] + BASE + steer + [
        "t60conv " + func, "t60ret %s skip" % func, "t60tol %s r3+0 %s abs %d" % (func, ty, tol)]
    open(os.path.join(TMP, name + ".txt"), "w").write("\n".join(spec) + "\n")


def run(name, func, steps):
    cmd = [sys.executable, os.path.join(HERE, "verify.py"), "_t60_test/" + name, "-n", N, "-shards", "1", "--no-cache",
           "-only", func, "-seed", SEED, "-t60steps", steps]
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if re.match(r"^%s  " % func, l)]
    return r.returncode, (row[0] if row else ""), r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails, names = [], []
    only = sys.argv[1:] or list(CASES)
    try:
        for func in only:
            for nc, mode in ((False, "site"), (True, "site"), (False, "same")):
                name = "libsig_%s_%s" % (func, "nc" if nc else mode)
                names.append(name)
                write(name, func, nc, mode)
                nc = nc or mode == "same"  # both expected to keep the entry: the pin writes start + B
                for steps in (("2", "4") if name.endswith(("site", "nc")) else ("2",)):
                    rc, row, out = run(name, func, steps)
                    ok = rc == 0 and row.rstrip().endswith("ok")
                    short = re.sub(r"\s+", " ", row)[-200:]
                    st = [l for l in out.splitlines() if l.startswith("t60stat %s N=%s" % (func, steps))]
                    if not nc and not ok:
                        fails.append("%s N=%s another site: rc %d, row %r\n%s" % (func, steps, rc, row, out[-2500:]))
                    elif nc and ok:
                        fails.append("%s N=%s %s passed: the kept entry is not reproduced (%s)" % (func, steps, name, short))
                    elif nc and not re.search(r"t60 N=%s: r3\+0 " % steps, row):
                        fails.append("%s N=%s NC failed on something else: rc %d, %r\n%s" % (func, steps, rc, row,
                                                                                         out[-2500:]))
                    else:
                        print("ok   %s N=%s %s: %s" % (func, steps, ("NC fails" if name.endswith("nc") else "same site keeps the entry, fails")
                                                       if nc else "another site restarts, passes",
                                                       st[0] if (st and not nc) else short))
    finally:
        for f in os.listdir(TMP) if os.path.isdir(TMP) else []:
            if f.startswith("libsig_"):
                os.remove(os.path.join(TMP, f))
        import verify  # noqa: E402
        for nm in names:  # this test's own unit build dirs
            for d in verify.unit_dirs_all("_t60_test/" + nm):
                shutil.rmtree(d, ignore_errors=True)
        if os.path.isdir(TMP) and not os.listdir(TMP):
            os.rmdir(TMP)
    for f in fails:
        print("FAIL " + f)
    print("t60libsig_test: %s" % ("FAIL" if fails else "PASS"))
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
