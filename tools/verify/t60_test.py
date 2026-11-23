#!/usr/bin/env python3
"""Self-test of the true60 harness support (T-01 pinned step state, T-02 -t60steps N; README "true60").

Example: d_a_mtoge daMtoge_actionUp 021E01B8 (the spikes rise: speedF chases 30 by 4 per step, mHeightOffset chases 0 by
speedF), converted by hand to a general step length (dt = 1/N rules 2 and 6, 1.6): the dt = 1 path is the
original; otherwise the acceleration is 4*dt and the height step speedF*dt plus the exact-arc term dv*(1-dt)/2, so N
steps add up to one original step. cLib_chaseF 0200F5C8 runs for real (`real`), so the steps' state is the real one.

T-01 (normal check, step state pinned to dt 1 / tick30 1 / N 1):
 1. the unmodified unit passes (default and -t60fixed config);
 2. the converted example passes bit-exact (default and -t60fixed config);
 3. negative: the general formula without the dt = 1 path fails (inf/NaN speeds: inf - inf);
 4. negative: a candidate that writes the step state fails (`t60 step state written`).
T-02 (-t60steps N, inputs steered to speeds 0..26 and heights -300..0, the unit's `t60tol` lines):
 5. the converted example passes at N = 2 and N = 4, and prints a `t60stat` line per function;
 6. negative: acceleration not scaled by dt (4 per step) fails on speedF;
 7. negative: no exact-arc term fails on mHeightOffset (one step of a*(N-1)/(2N) too little);
 8. negative: the unmodified (unconverted) function fails at N = 2;
 9. `t60conv` limits T-02 to the listed functions (an unconverted function not listed keeps its T-01 row);
10. malformed t60tol / t60conv / t60ret lines and a bad -t60steps exit 2.
c_lib (unit _t60/c_lib: smoothing primitives with step clamps and the integer remainder carry):
11. the converted c_lib passes T-02 at N = 2 for cLib_addCalc (`clamp` tolerance, both regimes counted in t60stat),
    cLib_addCalcAngleS and cLib_chaseAngleS (byte exact); chaseAngleS is the 16th address of the `t60conv` line (the
    parser once kept only the first 6);
12. negative: max/min step not scaled by dt fails cLib_addCalc ("a step clamped");
13. negative: the ratio scaled linearly (r*dt) instead of 1-(1-r)^dt fails cLib_addCalc ("no step clamped");
14. negative: the integer variants without the frame's start (each sub-step a full step) fail byte exact;
15. malformed `clamp` lines exit 2.
f_op_actor_mng (exact arc: fopAcM_calcSpeed records the gravity step dv, the same actor's posMove in the same step
adds dv*(1-dt)/2). The harness runs a nested marked function as the original, so the pair is checked through a test copy
of f_op_actor_mng_2.cpp whose calcSpeed / posMove carry no address marker (plain helpers of posMoveF) and `t60nocmp
025D6870` (T-02 without the normal comparison, whose call sequence differs by construction):
16. posMoveF passes T-02 at N = 2 and N = 4 (positions and speeds to float rounding);
17. negative: without the arc term it fails on the position (|d| = |g|*(1-dt)/2*... per frame);
18. negative: gravity not scaled by dt fails on speed.y.
J3DFrameCtrl / McaMorf:
19. J3DFrameCtrl::update passes T-02 at N = 2 with the stop/loop latch as `orsteps`; negative: a latch cleared after
    every sub-step fails ("(orsteps)"); an `orsteps` line on a float field exits 2;
20. mDoExt_McaMorf::play passes T-02 at N = 2 with its converted callees as `t60cand`; negative: without `t60cand` the
    callees run as the originals (no dt) and it fails on the frame.
true60 host be<> probe (t60_beprobe.py):
21. every TU that includes a true60 header compiles under gabi's static probe (-DGABI_HOST_BE_PROBE=1 -fsyntax-only,
    its verify flags, glued as verify builds it) without a host be<> object beyond the PENDING list (owner-listed known
    findings); negative: a host be<u32> local is found. T60_TEST_BEPROBE=0 skips it; t60_beprobe.py runs it alone.
Scope objects (tools/true60/scopelint.py, gate since 2026-11-21: 0 hits after CreateRun fix and the hat spring):
22. no glued body in wwhd_src has a class-typed local whose type declares a constructor / destructor (the glue's guest
    stack slot never runs it: a RAII dt gate is a silent no-op); any hit fails, `// scopelint: ok REASON` accepts one;
    negative: scopelint_test.py (a DtOne gate in a marked body is flagged, the accepted forms are not).
    T60_TEST_SCOPELINT=0 skips it.
Temporary unit files and sources go to tools/verify/units/_t60_test/ and are removed at the end (with their build dirs).
Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402
import specfile  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_t60_test")
SRC = "wwhd_src/d/actor/d_a_mtoge.cpp"
FUNC = "021E01B8"
N_T01 = os.environ.get("T60_TEST_N", "3000")  # inputs per check (T60_TEST_N=10000 T60_TEST_SEED=7: the full proof runs)
SEED = os.environ.get("T60_TEST_SEED")

ORIG_BODY = """BOOL daMtoge_actionUp(daMtoge_c* i_this) {
    cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);

    if (cLib_chaseF(&i_this->mHeightOffset, 0.0f, i_this->speedF)) {
        i_this->setAction(ACT_ARRIVAL);
    }
"""

GENERAL = """    f32 dt = t60::dt();
    f32 v0 = i_this->speedF;
    cLib_chaseF(&i_this->speedF, 30.0f, %(accel)s);
    f32 dv = i_this->speedF - v0; /* this step's change of speed */
    /* exact arc: N steps of v*dt + dv*(1-dt)/2 add up to one original step's v */
    if (cLib_chaseF(&i_this->mHeightOffset, 0.0f, %(step)s)) {
        i_this->setAction(ACT_ARRIVAL);
    }
"""
OK_ARGS = {"accel": "4.0f * dt", "step": "i_this->speedF * dt + dv * (1.0f - dt) * 0.5f"}

VARIANTS = {
    # the hand-made conversion: dt = 1 is the original path (rule 1.5.2)
    "conv": "BOOL daMtoge_actionUp(daMtoge_c* i_this) {\n    if (t60::dt() == 1.0f) {\n" +
            ORIG_BODY.split("{\n", 1)[1].replace("\n    ", "\n        ") + "    } else {\n" +
            (GENERAL % OK_ARGS).replace("\n    ", "\n        ") + "    }\n",
    # negatives
    "generic": "BOOL daMtoge_actionUp(daMtoge_c* i_this) {\n" + GENERAL % OK_ARGS,
    "writes": "BOOL daMtoge_actionUp(daMtoge_c* i_this) {\n    wwhd_t60_tick30 = 0;\n" + ORIG_BODY.split("{\n", 1)[1],
    "noaccel": "BOOL daMtoge_actionUp(daMtoge_c* i_this) {\n    if (t60::dt() == 1.0f) {\n" +
               ORIG_BODY.split("{\n", 1)[1].replace("\n    ", "\n        ") + "    } else {\n" +
               (GENERAL % dict(OK_ARGS, accel="4.0f")).replace("\n    ", "\n        ") + "    }\n",
    "noarc": "BOOL daMtoge_actionUp(daMtoge_c* i_this) {\n    if (t60::dt() == 1.0f) {\n" +
             ORIG_BODY.split("{\n", 1)[1].replace("\n    ", "\n        ") + "    } else {\n" +
             (GENERAL % dict(OK_ARGS, step="i_this->speedF * dt")).replace("\n    ", "\n        ") + "    }\n",
}

BASE_SPEC = open(os.path.join(UNITS, "d_a_mtoge.txt")).read().splitlines()
REAL = ["real 0200F5C8  # cLib_chaseF: the steps' speeds and heights are the real ones"]
STEER = ["field 021E01B8 r3+0x370 f32 0 26      # speedF below the 30 cap (no clamp inside a full step)",
         "field 021E01B8 r3+0x3C0 f32 -300 0    # mHeightOffset: hidden .. up"]
TOL = ["t60tol 021E01B8 r3+0x370 f32 absrel 0.0001 0.000001   # speedF",
       "t60tol 021E01B8 r3+0x3C0 f32 absrel 0.0001 0.000001   # mHeightOffset"]


def write_unit(name, variant, extra=()):
    src = SRC
    if variant:
        text = open(os.path.join(ROOT, SRC)).read()
        assert ORIG_BODY in text, "d_a_mtoge.cpp changed: update t60_test.py"
        text = text.replace(ORIG_BODY, VARIANTS[variant]).replace('#include "d/actor/d_a_mtoge.h"\n',
                                                                    '#include "d/actor/d_a_mtoge.h"\n#include "gabi_t60.h"\n', 1)
        src = os.path.relpath(os.path.join(TMP, name + ".cpp"), ROOT)
        with open(os.path.join(ROOT, src), "w") as f:
            f.write(text)
    lines = [l if not l.startswith("src ") else "src " + src for l in BASE_SPEC] + REAL + list(extra)
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")


def run(name, *args, n=N_T01, only=FUNC):
    cmd = [sys.executable, os.path.join(HERE, "verify.py"), "_t60_test/" + name, "-n", n, "-shards", "1", "--no-cache"]
    if only:
        cmd += ["-only", only]
    if SEED:
        cmd += ["-seed", SEED]
    r = subprocess.run(cmd + list(args), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout, r.stderr


def rows(out):
    return {l[:8]: l for l in out.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)}


CLIB_SRC = "wwhd_src/SSystem/SComponent/c_lib.cpp"
CLIB_SPEC = [l.rstrip("\n") for l in specfile.expanded_lines(os.path.join(UNITS, "_t60", "c_lib.txt"))]
CLIB_VARIANTS = {  # (old, new) replacements in c_lib.cpp
    "conv": [],
    "nomax": [("        maxStep = fmuls_ppc(maxStep, t60::dt());                \\\n        minStep = fmuls_ppc(minStep, t60::dt());", "")],
    "linear": [("        scale = t60_ratio(scale, t60::dt());                    \\\n        maxStep = fmuls_ppc(maxStep, t60::dt());                \\\n        minStep",
                "        scale = fmuls_ppc(scale, t60::dt());                    \\\n        maxStep = fmuls_ppc(maxStep, t60::dt());                \\\n        minStep")],
    "nostart": [("s32 start = e ? e->start : cur;", "s32 start = cur;"), ("s32 start = e ? e->start : cur, ret;", "s32 start = cur, ret;")],
}


def write_clib(name, variant, extra=()):
    text = open(os.path.join(ROOT, CLIB_SRC)).read()
    for old, new in CLIB_VARIANTS[variant]:
        assert old in text, "c_lib.cpp changed: update t60_test.py (%s)" % variant
        text = text.replace(old, new)
    src = os.path.relpath(os.path.join(TMP, name + ".cpp"), ROOT)
    with open(os.path.join(ROOT, src), "w") as f:
        f.write(text)
    lines = [l if not l.startswith("src ") else "src " + src for l in CLIB_SPEC] + list(extra)
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")


def clib_checks(fails, expect_ok, expect_fail):
    n = str(max(300, int(N_T01) // 3))
    write_clib("clib_conv", "conv")
    for func in ("0200ECD4", "0200F378", "0200F8D0"):
        rc, out, err = run("clib_conv", "-t60steps", "2", n=n, only=func)
        expect_ok("T-02 c_lib %s N=2" % func, rc, out, func=func)
        st = [l for l in err.splitlines() if l.startswith("t60stat %s N=2:" % func)]
        if not st or (func == "0200ECD4" and not ("(clamp)" in st[0] and "(no clamp)" in st[0])):
            fails.append("T-02 c_lib %s: no t60stat line (with both clamp regimes for addCalc): %r" % (func, st or err[-300:]))
        else:
            print("     %s" % st[0])
    write_clib("clib_nomax", "nomax")
    rc, out, _ = run("clib_nomax", "-t60steps", "2", n=n, only="0200ECD4")
    expect_fail("T-02 c_lib max/min step not scaled", rc, out, r"t60 N=2: r3\+0 f32 .*a step clamped", func="0200ECD4")
    write_clib("clib_linear", "linear")
    rc, out, _ = run("clib_linear", "-t60steps", "2", n=n, only="0200ECD4")
    expect_fail("T-02 c_lib ratio scaled linearly", rc, out, r"t60 N=2: r3\+0 f32 .*no step clamped", func="0200ECD4")
    write_clib("clib_nostart", "nostart")
    for func in ("0200F378", "0200F8D0"):
        rc, out, _ = run("clib_nostart", "-t60steps", "2", n=n, only=func)
        expect_fail("T-02 c_lib %s without the frame's start" % func, rc, out, r"t60 N=2: .*\(exact\)", func=func)
    for bad in ("t60tol 0200ECD4 r3+0 f32 clamp f3 f4", "t60tol 0200ECD4 r3+0 f32 clamp x3 f4 f1",
                "t60tol 0200ECD4 r3+0 f32 clamp f3 r4+0 f1", "t60tol 0200ECD4 r3+0 f32 clamp f3 f4 f1 0.1",
                "t60tol 0200ECD4 r3+0 f32 clamp f14 f4 f1"):
        write_clib("clib_bad", "conv", [bad])
        rc, _, _ = run("clib_bad", n="1", only="0200ECD4")
        if rc != 2:
            fails.append("not rejected (rc %d): %s" % (rc, bad))
    print("ok   malformed clamp lines rejected" if not any("clamp" in f and "rejected" in f for f in fails) else "")


FOP_SRC = "wwhd_src/f_op/f_op_actor_mng_2.cpp"
FOP_SPEC = [l.rstrip("\n") for l in specfile.expanded_lines(os.path.join(UNITS, "_t60", "f_op_actor_mng.txt"))]
FOP_VARIANTS = {
    "conv": [],
    "noarc": [("            arc = gabi::fmuls_ppc(gabi::fmuls_ppc(t60Arc.dv, gabi::fsubs_ppc(1.0f, dt)), 0.5f);", "            arc = 0.0f;")],
    "nograv": [("f32 vy = gabi::fadds_ppc(vy0, gabi::fmuls_ppc((f32)actorView->gravity, dt));",
                "f32 vy = gabi::fadds_ppc(vy0, (f32)actorView->gravity);")],
}


def write_fop(name, variant):
    text = open(os.path.join(ROOT, FOP_SRC)).read()
    for old, new in [("/* 025D67A8 */\n", ""), ("/* 025D6800 */\n", "")] + FOP_VARIANTS[variant]:
        assert old in text, "f_op_actor_mng_2.cpp changed: update t60_test.py (%s)" % variant
        text = text.replace(old, new, 1)
    src = os.path.relpath(os.path.join(TMP, name + ".cpp"), ROOT)
    with open(os.path.join(ROOT, src), "w") as f:
        f.write(text)
    lines = ["src " + src if l == "src " + FOP_SRC else l for l in FOP_SPEC] + ["t60nocmp 025D6870"]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")


def fop_checks(fails, expect_ok, expect_fail):
    n = str(max(300, int(N_T01) // 3))
    func = "025D6870"
    write_fop("fop_conv", "conv")
    for steps in ("2", "4"):
        rc, out, err = run("fop_conv", "-t60steps", steps, n=n, only=func)
        expect_ok("T-02 posMoveF exact arc N=%s" % steps, rc, out, func=func)
        st = [l for l in err.splitlines() if l.startswith("t60stat %s N=%s:" % (func, steps))]
        if not st or "r3+0x314" not in st[0]:
            fails.append("T-02 posMoveF N=%s: no t60stat line with the position: %r" % (steps, st or err[-300:]))
        else:
            print("     %s" % st[0])
    write_fop("fop_noarc", "noarc")
    rc, out, _ = run("fop_noarc", "-t60steps", "2", n=n, only=func)
    expect_fail("T-02 posMoveF without the arc term", rc, out, r"t60 N=2: r3\+0x314 f32x3 at r3\+0x318", func=func)
    write_fop("fop_nograv", "nograv")
    rc, out, _ = run("fop_nograv", "-t60steps", "2", n=n, only=func)
    expect_fail("T-02 posMoveF gravity not scaled", rc, out, r"t60 N=2: r3\+0x(314|33C) f32x3", func=func)


def write_variant(name, base_unit, src_path, reps, extra=(), drop=()):
    """a test copy of unit base_unit with src_path replaced by an edited copy (reps: (old, new)), plus extra lines"""
    text = open(os.path.join(ROOT, src_path)).read()
    for old, new in reps:
        assert old in text, "%s changed: update t60_test.py" % src_path
        text = text.replace(old, new, 1)
    src = os.path.relpath(os.path.join(TMP, name + ".cpp"), ROOT)
    with open(os.path.join(ROOT, src), "w") as f:
        f.write(text)
    spec = [l.rstrip("\n") for l in specfile.expanded_lines(os.path.join(UNITS, base_unit + ".txt"))]
    lines = ["src " + src if l == "src " + src_path else l for l in spec if l not in drop] + list(extra)
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")


def anim_checks(fails, expect_ok, expect_fail):
    n = str(max(300, int(N_T01) // 3))
    j3d = "wwhd_src/J3DAnimation.cpp"
    write_variant("j3d_conv", "_t60/J3DAnimation", j3d, [])
    rc, out, err = run("j3d_conv", "-t60steps", "2", n=n, only="027F2FC4")
    expect_ok("T-02 J3DFrameCtrl::update N=2 (orsteps latch)", rc, out, func="027F2FC4")
    write_variant("j3d_nolatch", "_t60/J3DAnimation", j3d, [("        j3d_t60::note(gabi::ea(this), mFrame, mRate);\n}\n\n/* WWHD glue",
                                                              "        j3d_t60::note(gabi::ea(this), mFrame, mRate);\n    if (t60::dt() != 1.0f)\n        mState = 0;\n}\n\n/* WWHD glue")])
    rc, out, _ = run("j3d_nolatch", "-t60steps", "2", n=n, only="027F2FC4")
    expect_fail("T-02 J3DFrameCtrl::update with the latch cleared", rc, out, r"\(orsteps\)", func="027F2FC4")
    write_variant("j3d_bad", "_t60/J3DAnimation", j3d, [], ["t60tol 027F2FC4 r3+4 f32 orsteps"])
    rc, _, _ = run("j3d_bad", n="1", only="027F2FC4")
    if rc != 2:
        fails.append("not rejected (rc %d): orsteps on a float field" % rc)
    morf = "wwhd_src/m_Do/m_Do_ext_morf.cpp"
    write_variant("morf_cand", "_t60/m_Do_ext_04", morf, [])
    rc, out, _ = run("morf_cand", "-t60steps", "2", n=n, only="025E535C")
    expect_ok("T-02 McaMorf::play N=2 (t60cand callees)", rc, out, func="025E535C")
    write_variant("morf_nocand", "_t60/m_Do_ext_04", morf, [], drop=["t60cand 0200F5C8 027F2FC4"])
    rc, out, _ = run("morf_nocand", "-t60steps", "2", n=n, only="025E535C")
    expect_fail("T-02 McaMorf::play without t60cand", rc, out, r"t60 N=2:\s+(byte )?r3\+0x9C", func="025E535C")


def scope_checks(fails):
    """22: scopelint over wwhd_src finds nothing; its self-test (negative control) passes"""
    lint = os.path.join(ROOT, "tools", "true60")
    r = subprocess.run([sys.executable, os.path.join(lint, "scopelint_test.py")], cwd=ROOT, capture_output=True, text=True)
    if r.returncode or "scopelint_test: PASS" not in r.stdout:
        fails.append("scopelint_test (negative control) failed: %r" % (r.stdout + r.stderr)[-400:])
        return
    r = subprocess.run([sys.executable, os.path.join(lint, "scopelint.py"), os.path.join(ROOT, "wwhd_src")], cwd=ROOT,
                       capture_output=True, text=True)
    hits = [l for l in r.stdout.splitlines() if "declares a constructor" in l]
    if r.returncode or hits:
        fails.append("scopelint: %d scope object(s) in converted code (rc %d):\n  %s"
                     % (len(hits), r.returncode, "\n  ".join(hits) or (r.stdout + r.stderr)[-400:]))
    else:
        print("ok   scopelint: no scope object in a glued body (negative control flagged)")


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []

    def expect_ok(what, rc, out, func=FUNC):
        r = rows(out).get(func, "")
        if rc or not r.endswith(" ok"):
            fails.append("%s: expected ok, got rc %d row %r" % (what, rc, r or out[-400:]))
        else:
            print("ok   %s: %s" % (what, " ".join(r.split())))

    def expect_fail(what, rc, out, pattern, func=FUNC):
        r = rows(out).get(func, "")
        if not r or r.endswith(" ok") or not re.search(pattern, r):
            fails.append("%s: expected a FAIL row matching %r, got rc %d row %r" % (what, pattern, rc, r or out[-400:]))
        else:
            print("ok   %s (fails as it should): %s" % (what, " ".join(r.split())[:220]))

    try:
        # T-01
        write_unit("orig", None)
        rc, out, _ = run("orig")
        expect_ok("T-01 unmodified unit", rc, out)
        rc, out, _ = run("orig", "-t60fixed")
        expect_ok("T-01 unmodified unit, -t60fixed", rc, out)
        write_unit("conv", "conv")
        rc, out, _ = run("conv")
        expect_ok("T-01 converted example (dt pinned to 1)", rc, out)
        rc, out, _ = run("conv", "-t60fixed")
        expect_ok("T-01 converted example, -t60fixed", rc, out)
        write_unit("generic", "generic")
        rc, out, _ = run("generic")
        expect_fail("T-01 general formula without the dt = 1 path", rc, out, r"differ|byte|memory|call #")
        write_unit("writes", "writes")
        rc, out, _ = run("writes")
        expect_fail("T-01 candidate writes the step state", rc, out, r"t60 step state written")
        # T-02
        write_unit("conv_t2", "conv", STEER + TOL)
        for n in ("2", "4"):
            rc, out, err = run("conv_t2", "-t60steps", n)
            expect_ok("T-02 converted example N=%s" % n, rc, out)
            st = [l for l in err.splitlines() if l.startswith("t60stat %s N=%s:" % (FUNC, n))]
            m = re.search(r"(\d+) inputs compared", st[0]) if st else None
            if not m or int(m.group(1)) < int(N_T01) // 2:
                fails.append("T-02 N=%s: no t60stat line with enough compared inputs: %r" % (n, st or err[-300:]))
            else:
                print("     %s" % st[0])
        write_unit("noaccel", "noaccel", STEER + TOL)
        rc, out, _ = run("noaccel", "-t60steps", "2")
        expect_fail("T-02 acceleration not scaled by dt", rc, out, r"t60 N=2: r3\+0x370 f32")
        write_unit("noarc", "noarc", STEER + TOL)
        for n in ("2", "4"):
            rc, out, _ = run("noarc", "-t60steps", n)
            expect_fail("T-02 no exact-arc term N=%s" % n, rc, out, r"t60 N=%s: r3\+0x3C0 f32" % n)
        write_unit("orig_t2", None, STEER + TOL)
        rc, out, _ = run("orig_t2", "-t60steps", "2")
        expect_fail("T-02 unconverted function", rc, out, r"t60 N=2: ")
        # t60conv: only the listed functions get T-02 (021E02C8 actionDown is unconverted and not listed)
        write_unit("conv_only", "conv", STEER + TOL + ["t60conv 021E01B8"])
        rc, out, _ = run("conv_only", "-t60steps", "2", only=None, n="300")
        r = rows(out)
        if not r.get(FUNC, "").endswith(" ok") or not r.get("021E02C8", "").endswith(" ok"):
            fails.append("t60conv: expected 021E01B8 and 021E02C8 ok, got %r %r" % (r.get(FUNC), r.get("021E02C8")))
        else:
            print("ok   t60conv: unlisted unconverted 021E02C8 keeps its T-01 row")
        # malformed lines
        for bad in ("t60tol 021E01B8 r3+0x370 f33 abs 1", "t60tol 021E01B8 r3+0x370 f32 abs", "t60tol 021E01B8 r3+0x370 f32 near 1",
                    "t60tol 021E01B8 r2+0x370 f32 abs 1", "t60tol 021E01B8 r3+0x370 f32x0 abs 1", "t60tol 021E01B8 r3+0x370 f32 abs -1",
                    "t60conv", "t60ret 021E01B8 maybe"):
            write_unit("bad", None, [bad])
            rc, _, _ = run("bad", n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
        write_unit("bad", None)
        rc, _, _ = run("bad", "-t60steps", "3", n="1")
        if rc != 2:
            fails.append("-t60steps 3 not rejected (rc %d)" % rc)
        print("ok   malformed lines rejected" if not any("rejected" in f for f in fails) else "")
        clib_checks(fails, expect_ok, expect_fail)
        fop_checks(fails, expect_ok, expect_fail)
        anim_checks(fails, expect_ok, expect_fail)
        if os.environ.get("T60_TEST_SCOPELINT", "1") != "0":
            scope_checks(fails)
        if os.environ.get("T60_TEST_BEPROBE", "1") != "0":
            import t60_beprobe
            t60_beprobe.beprobe_checks(fails)
    finally:
        names = [f[:-4] for f in os.listdir(TMP) if f.endswith(".txt")] if os.path.isdir(TMP) else []
        shutil.rmtree(TMP, ignore_errors=True)
        for nm in names:  # this test's own unit build dirs
            for d in verify.unit_dirs_all("_t60_test/" + nm):
                shutil.rmtree(d, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("t60_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
