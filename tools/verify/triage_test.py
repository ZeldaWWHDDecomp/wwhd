#!/usr/bin/env python3
"""Self-test of triage.py (2026-10-30): the four mechanical checks on real compiled candidates.

Textual mutants of merged AGL#2 sources (the same sites as AGL#2's archived ledger rows), each compiled like a
mutate.py per-mutant build and triaged; the expected candidate per case:
  1. swap of `color->mValue.b=0.0f;` / `color->mValue.a=1.0f;` (02770D10, AGL#2 #80)    -> swap      EQ?
  2. `skyName->mText=` moved after the initializeColorParameter call that reads the name (r4 of 0279E618)
                                                                                         -> swap      not EQ?
  3. FrameLocal skyHint 0x70 -> 0x71 (02770EC0, AGL#2 #19: overlaps only dead groundHint)  -> frame     EQ?
  4. FrameLocal binding 8 -> 9 (0277408C, AGL#2 #715: last byte on the r20 save slot)     -> frame     RI?
  5. destroyShaderArray(..., 1, 0) -> (..., 1, 1): r8 of 028F0164, live-in r3..r7 (#254) -> unreadarg EQ?
  6. `gabi::cpu->cr[6]=0` -> `cr[7]` before the variadic formatter 02779244 whose format has no float
     conversion (02775090, AGL#2 #560 kind)                                              -> cr6       EQ?
  7. `color->mValue.b=0.0f` -> `1.0f` (a stored value changes)                              -> no EQ?/RI?
  8. a mutant from another source version                                                  -> stale
Needs build/gen and the game link (for build/verify/image.bin, livein.tsv, frames.tsv; generated with the
verify.py steps when missing). Scratch: a temporary directory, removed at the end unless TRIAGE_TEST_KEEP=DIR is
set (then DIR is used and kept). Exit 0 when all cases pass.
"""
import os
import shutil
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import triage  # noqa: E402

LC = "wwhd_src/agl/LightConstructors.cpp"
CASES = [
    # name, src, func, mutation text, old, new, expected (check, candidate set)
    ("swap-independent", LC, "02770D10", "swap `color->mValue.b=0.0f;` / `color->mValue.a=1.0f;` @LightConstructors.cpp:48",
     "            color->mValue.b=0.0f;\n            color->mValue.a=1.0f;\n",
     "            color->mValue.a=1.0f;\n            color->mValue.b=0.0f;\n", ("swap", {"EQ?"})),
    ("swap-dependent", LC, "02770EC0", "swap `skyName->mText=` / `initializeColorParameter(` @LightConstructors.cpp:113",
     "        skyName->mVtable=&sParameterNameTable;skyName->mText=&sSkyColorName;\n"
     "        skyDescription->mVtable=&sParameterNameTable;skyDescription->mText=&sSkyColorDescription;\n"
     "        initializeColorParameter(lightEmbeddedField(&light->mSkyColor),skyName.get(),\n"
     "            skyDescription.get(),skyHint.get(),light,sky.get());\n",
     "        skyName->mVtable=&sParameterNameTable;\n"
     "        skyDescription->mVtable=&sParameterNameTable;skyDescription->mText=&sSkyColorDescription;\n"
     "        initializeColorParameter(lightEmbeddedField(&light->mSkyColor),skyName.get(),\n"
     "            skyDescription.get(),skyHint.get(),light,sky.get());skyName->mText=&sSkyColorName;\n",
     ("swap", {"MANUAL", "DEP"})),
    ("frame-eq", LC, "02770EC0", "literal 0x70->0x71 @LightConstructors.cpp:112",
     "skyDescription(0x30),skyHint(0x70);", "skyDescription(0x30),skyHint(0x71);", ("frame", {"EQ?"})),
    ("frame-ri", "wwhd_src/agl/ShaderCommandState.cpp", "0277408C", "literal 8->9 @ShaderCommandState.cpp:369",
     "gabi::FrameLocal<UniformBinding> binding(8);", "gabi::FrameLocal<UniformBinding> binding(9);", ("frame", {"RI?"})),
    ("unread-r8", "wwhd_src/agl/ShaderBindingEditor.cpp", "02772EE8", "literal 0->1 @ShaderBindingEditor.cpp:124",
     "destroyShaderArray(editors,-1,0x24,destroyEditorArrayElement,1,0);",
     "destroyShaderArray(editors,-1,0x24,destroyEditorArrayElement,1,1);", ("unreadarg", {"EQ?"})),
    ("cr6", "wwhd_src/agl/BloomParameters.cpp", "02775090", "literal 6->7 @BloomParameters.cpp:428",
     "setBloomName(description.get(),&sBloomText10161B5C);gabi::cpu->cr[6]=0;",
     "setBloomName(description.get(),&sBloomText10161B5C);gabi::cpu->cr[7]=0;", ("cr6", {"EQ?"})),
    ("value-change", LC, "02770D10", "literal 0.0f->1.0f @LightConstructors.cpp:48",
     "            color->mValue.b=0.0f;\n            color->mValue.a=1.0f;\n",
     "            color->mValue.b=1.0f;\n            color->mValue.a=1.0f;\n", (None, {"MANUAL", "DEP"})),
]


def ensure_inputs():
    """the inputs verify.py builds first (image.bin, livein.tsv, frames.tsv)"""
    bv = os.path.join(ROOT, "build", "verify")
    os.makedirs(bv, exist_ok=True)
    steps = [("image.bin", ["mkimage.py", "game/code/cking.rpx"]), ("livein.tsv", ["livein_all.py"]),
             ("frames.tsv", ["frames_all.py"])]
    for out, cmd in steps:
        p = os.path.join(bv, out)
        if not os.path.exists(p):
            r = subprocess.run([sys.executable, "-B", os.path.join(HERE, cmd[0])] + cmd[1:] + [p], cwd=ROOT,
                               capture_output=True, text=True)
            if r.returncode or not os.path.exists(p):
                return "cannot build %s: %s" % (out, (r.stdout + r.stderr)[-300:])
    return None


class Args:
    pass


def main():
    err = ensure_inputs()
    if err:
        print("FAIL", err)
        print("triage_test: FAIL")
        return 1
    keep = os.environ.get("TRIAGE_TEST_KEEP")
    tmp = keep or tempfile.mkdtemp(prefix="wwhd-triage-test-")
    os.makedirs(tmp, exist_ok=True)
    fails = []
    try:
        a = Args()
        bv = os.path.join(ROOT, "build", "verify")
        a.livein, a.frames, a.image = (os.path.join(bv, f) for f in ("livein.tsv", "frames.tsv", "image.bin"))
        a.wdis_root, a.scratch = ROOT, tmp
        ctx = triage.Ctx(a)
        ctx.unitc = None
        builder = triage.Builder(ROOT, ROOT, tmp)
        cases = list(CASES)
        for n, (name, src, func, mut, old, new, exp) in enumerate(cases):
            text = open(os.path.join(ROOT, src)).read()
            if text.count(old) != 1:
                fails.append("%s: site not found once in %s" % (name, src))
                continue
            cpp = os.path.join(tmp, "case%d.cpp" % n)
            with open(cpp, "w") as fh:
                fh.write(text.replace(old, new))
            row = triage.triage_one(ctx, builder, dict(id=900 + n, func=func, src=src, mutation=mut, cpp=cpp))
            ok = (exp[0] is None or row["check"] == exp[0]) and row["candidate"] in exp[1]
            print("%-16s %-10s %-7s %s" % (name, row["check"], row["candidate"], row["reason"][:150]))
            if not ok:
                fails.append("%s: expected %s %s, got %s %s (%s)" % (name, exp[0], sorted(exp[1]), row["check"],
                                                                     row["candidate"], row["evidence"]))
        # 8. a receipt from another version of the source
        text = open(os.path.join(ROOT, LC)).read().split("\n")
        other = "\n".join(text[:30] + ["// moved"] * 12 + text[30:]).replace("color->mValue.b=0.0f;", "color->mValue.b=2.0f;", 1)
        cpp = os.path.join(tmp, "stale.cpp")
        with open(cpp, "w") as fh:
            fh.write(other)
        row = triage.triage_one(ctx, builder, dict(id=999, func="02770D10", src=LC, mutation="literal x", cpp=cpp))
        print("%-16s %-10s %-7s %s" % ("stale-receipt", row["check"], row["candidate"], row["reason"][:150]))
        if row["check"] != "stale":
            fails.append("stale receipt not detected: %s" % row["check"])
    finally:
        if not keep:
            shutil.rmtree(tmp, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("triage_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
