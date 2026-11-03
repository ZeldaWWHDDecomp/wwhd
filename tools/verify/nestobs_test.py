#!/usr/bin/env python3
"""Self-test of the pointee-offset span and of `observenested` (harness.cpp, 2026-10-30, AGL#3 Mip/D3C request).

1. Pointee-offset span `[rN+OFF]+OFF2`: `observearg TARGET [rN+OFF]+OFF2 SIZE` (also `observeptr TARGET [rN+OFF]+OFF2
   SIZE`, the same span) compares, at every top-level call to TARGET, the SIZE bytes at (word at rN+OFF) + OFF2.
2. `observenested TARGET [SPAN SIZE]`: calls to TARGET made inside `real` callees are recorded in order with their
   enclosing top-level call and compared original against candidate (count, order, span bytes).

Carrier: agl::RenderTarget::bindFirst 027B9F60 (RenderTargetSupport.cpp, `bindRenderTarget(this, 0)`), with
the original bindRenderTarget 027B9D00 and ColorSurface::initializeRegisters 027B6DDC `real`: for each colour target
whose dirty byte (+0x90) is set, 027B6DDC copies its view mip (+0x94) into the native GX2ColorBuffer (+0xAC, viewMip at
native +0x74) and calls GX2InitColorBufferRegs C00063D0 with r3 = native, so the SDK reads its viewMip at r3+0x74; all of
that is nested. Two colour targets (0x68502000, 0x68502200); fields from the lib2_agl_15 RenderTarget block.
Candidates (temporary copies of RenderTargetSupport.cpp; each restores every byte it changes, so the net effect and the
top-level call equal the original's):
  orig   unchanged;
  arg    flips colour 0's view mip around the call (the SDK sees viewMip ^ 1);
  count  forces colour 0 dirty around the call (an extra nested GX2InitColorBufferRegs when it was clean);
  swap   swaps the two view mips around the call (same values, other order).
Checks:
  (a) `observearg 027B9D00 [r3+0x1C]+0x94 4` (and the observeptr form) catches `arg` and passes `orig`; without it
      `arg` passes; the span reads the right bytes (with the view mip fixed to 0x11223344 and colour 0 dirty, -trace
      shows exactly those bytes for the span and for the nested record);
  (b) `observenested C00063D0 r3+0x74 4` catches `arg` (bytes), `count` (number) and `swap` (order), and passes `orig`;
      without it all three pass;
  (c) malformed lines exit 2; existing forms keep their messages (observesite_test.py / observepointees_test.py /
      cr_test.py cover the rest).
Builds go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set), removed at exit.
"""
import atexit
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
NAME = "_nestobs_test"
TMP = os.path.join(UNITS, NAME)
SRC = "wwhd_src/agl/RenderTargetSupport.cpp"
F = "027B9F60"
BODY = """void RenderTarget::bindFirst() {
    gabi::AutoFrame frame{gabi::current_function()};
    bindRenderTarget(this, 0);
}"""
VIEW = """struct NestObsColor_l {
    u8 head[0x90];
    be<u8> dirty;
    u8 reserved[3];
    be<u32> viewMip;
    u8 middle[0xAC - 0x98];
    be<u32> native[0x9C / 4]; /* native viewMip at +0x74 = word 0x1D */
};
"""
MUTANTS = {
    "orig": BODY,
    "arg": """void RenderTarget::bindFirst() {
    gabi::AutoFrame frame{gabi::current_function()};
    NestObsColor_l* c = reinterpret_cast<NestObsColor_l*>(colors[0].get());
    u32 vm = c->viewMip, nvm = c->native[0x1D];
    u8 dirty = c->dirty;
    c->viewMip = vm ^ 1;
    bindRenderTarget(this, 0);
    c->viewMip = vm;
    c->native[0x1D] = dirty ? vm : nvm;
}""",
    "count": """void RenderTarget::bindFirst() {
    gabi::AutoFrame frame{gabi::current_function()};
    NestObsColor_l* c = reinterpret_cast<NestObsColor_l*>(colors[0].get());
    u32 saved[0x9C / 4];
    for (u32 i = 0; i < 0x9C / 4; ++i) saved[i] = c->native[i];
    u8 dirty = c->dirty;
    c->dirty = 1;
    bindRenderTarget(this, 0);
    if (!dirty)
        for (u32 i = 0; i < 0x9C / 4; ++i) c->native[i] = saved[i];
}""",
    "swap": """void RenderTarget::bindFirst() {
    gabi::AutoFrame frame{gabi::current_function()};
    NestObsColor_l* c0 = reinterpret_cast<NestObsColor_l*>(colors[0].get());
    NestObsColor_l* c1 = reinterpret_cast<NestObsColor_l*>(colors[1].get());
    u32 v0 = c0->viewMip, v1 = c1->viewMip, n0 = c0->native[0x1D], n1 = c1->native[0x1D];
    u8 d0 = c0->dirty, d1 = c1->dirty;
    c0->viewMip = v1;
    c1->viewMip = v0;
    bindRenderTarget(this, 0);
    c0->viewMip = v0;
    c1->viewMip = v1;
    c0->native[0x1D] = d0 ? v0 : n0;
    c1->native[0x1D] = d1 ? v1 : n1;
}""",
}
SPEC = """# nestobs_test carrier (RenderTarget block of lib2_agl_15)
# verify: -strictnan -calleesp
src {src}
real 027B9D00 027B6DDC
arg! 027B9F60 r3 0x68500000
fill * 68500000 64 0
field * 6850001C u32 0x68502000
field * 68500020 u32 0x68502200
fill * 68502000 328 0
fill * 68502200 328 0
field * 68502090 u8 0,1
field * 68502290 u8 0,1
field * 68502094 u32 0,1,2
field * 68502294 u32 0,1,2
field * 68502098 u32 0,1,2
field * 68502298 u32 0,1,2
field * 104A2090 u32 0,1,2,3
field * 104A2094 u32 0x68504000
fill * 68504000 12 0
field * 104A0BC0 u32 0,1,2,3
field * 104A0BC4 u32 0,1,2,3
field * 104A0BC8 u32 0,1,2,3
ret! C0009BB0 0,1,2
""".splitlines()


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-nestobs_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    atexit.register(shutil.rmtree, scratch, True)
    atexit.register(shutil.rmtree, TMP, True)
    os.makedirs(TMP, exist_ok=True)
    import verify
    src = open(os.path.join(ROOT, SRC)).read()
    if src.count(BODY) != 1 or "/* 027B9F60 */" not in src:
        print("FAIL carrier source changed: bindFirst body not found")
        return 1
    specs, exe = {}, {}
    for k, body in MUTANTS.items():
        path = os.path.join(TMP, "RenderTargetSupport_%s.cpp" % k)
        with open(path, "w") as fh:
            fh.write(src.replace(BODY, body).replace("/* 027B9F60 */", VIEW + "/* 027B9F60 */"))
        specs[k] = [l.replace("{src}", os.path.relpath(path, ROOT)) for l in SPEC]
        with open(os.path.join(TMP, k + ".txt"), "w") as fh:
            fh.write("\n".join(specs[k]) + "\n")
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), NAME + "/" + k, "-n", "1", "-only", F,
                            "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
        exe[k] = os.path.join(verify.unit_dir(NAME + "/" + k), "verify")
        if r.returncode or not os.path.exists(exe[k]):
            print("FAIL build %s: rc %d %s" % (k, r.returncode, (r.stdout + r.stderr)[-800:]))
            return 1
    fails = []
    nrun = [0]

    def run(k, extra, n="60", seed="1", opts=(), sub=None):
        nrun[0] += 1
        path = os.path.join(TMP, "run%d.txt" % nrun[0])
        lines = [sub.get(l, l) for l in specs[k]] if sub else specs[k]
        with open(path, "w") as fh:
            fh.write("\n".join(lines + list(extra)) + "\n")
        r = subprocess.run([exe[k], "-spec", path, "-n", n, "-seed", seed, "-only", F, "-noprogress"] + list(opts),
                           cwd=ROOT, capture_output=True, text=True)
        row = [l for l in r.stdout.splitlines() if l.startswith(F)]
        return r.returncode, (row[-1] if row else ""), r.stderr.strip().splitlines()[-1:] or [""], r.stdout

    def expect(what, k, extra, ok, needle=None):
        rc, row, err, out = run(k, extra)
        good = (rc == 0 and row.endswith(" ok")) if ok else (" ok" not in row[-4:] and (needle is None or needle in out))
        shown = row[42:] or err[0][:100]
        if not ok and needle and needle in out:
            shown = next(l for l in out.splitlines() if needle in l).strip()[:200]
        print("  %-5s %-48s %s" % (k, "; ".join(extra)[:48], shown))
        if not good:
            fails.append("%s: %s rc %d %s %s" % (what, k, rc, row, err[0]))

    DEREF = "observearg 027B9D00 [r3+0x1C]+0x94 4"
    NEST = "observenested C00063D0 r3+0x74 4"
    print("0. carrier: every candidate passes without observation")
    for k in MUTANTS:
        expect("%s passes unobserved" % k, k, [], True)
    print("1. pointee-offset span")
    expect("orig passes deref", "orig", [DEREF], True)
    expect("arg caught by deref", "arg", [DEREF], False,
           "call #0 027B9D00: pointee span [r3+0x1C]+0x94 differs at [r3+0x1C]+0x97 (observearg)")
    expect("arg caught by observeptr form", "arg", ["observeptr 027B9D00 [r3+0x1C]+0x94 8"], False,
           "pointee span [r3+0x1C]+0x94 differs at [r3+0x1C]+0x97")
    expect("orig passes observeptr form", "orig", ["observeptr 027B9D00 [r3+0x1C]+0x94 8"], True)
    expect("arg not caught at colour 1", "arg", ["observearg 027B9D00 [r3+0x20]+0x94 4"], True)
    # the bytes read: colour 0's view mip fixed to 0x11223344 and dirty, colour 1 clean: the span and the one nested
    # record carry exactly those bytes, on both sides
    fixed = {"field * 68502094 u32 0,1,2": "field * 68502094 u32 0x11223344", "field * 68502090 u8 0,1": "field * 68502090 u8 1",
             "field * 68502290 u8 0,1": "field * 68502290 u8 0"}
    rc, row, err, out = run("orig", [DEREF, NEST], n="1", opts=["-trace", "1000003"], sub=fixed)
    span = re.findall(r"observearg 027B9D00 \[r3\+0x1C\]\+0x94 @0: ([0-9A-F ]+)", out)
    nest = re.findall(r"observenested C00063D0 in call #0: ([0-9A-F ]+)", out)
    print("  trace: span %s; nested %s; %s" % (span, nest, row[42:]))
    if span != ["11 22 33 44"] * 2 or nest != ["01 11 22 33 44"] * 2 or not row.endswith(" ok"):
        fails.append("traced bytes: span %s nested %s row %s" % (span, nest, row))
    print("2. observenested")
    expect("orig passes nested", "orig", [NEST], True)
    expect("orig passes nested + deref", "orig", [NEST, DEREF], True)
    expect("orig passes nested without span", "orig", ["observenested C00063D0"], True)
    expect("arg caught by nested bytes", "arg", [NEST], False,
           "nested observed call #0 C00063D0")
    expect("arg nested: span named", "arg", [NEST], False, "span r3+0x74 differs at r3+0x77")
    expect("count caught by nested number", "count", [NEST], False,
           "nested observed calls differ in number: original 0 candidate 1")
    expect("count caught without a span", "count", ["observenested C00063D0"], False,
           "nested observed calls differ in number: original 0 candidate 1")
    expect("swap caught by nested order", "swap", [NEST], False, "nested observed call #0 C00063D0")
    expect("swap not caught by count only", "swap", ["observenested C00063D0"], True)
    expect("unrelated target observes nothing", "arg", ["observenested C0006838 r3+0 4"], True)
    print("3. parser")
    bad = ["observearg 027B9D00 [r3+0x1C+0x94 4", "observearg 027B9D00 [r3+0x1C]0x94 4", "observearg 027B9D00 [r3+0x1C]+ 4",
           "observearg 027B9D00 [r2+0x1C]+0x94 4", "observearg 027B9D00 [r3+0x1C]+0x10000 4", "observearg 027B9D00 [x3]+4 4",
           "observearg 027B9D00 [r3+0x1C]+0x94 0", "observeptr 027B9D00 [r3+0x1C]+0x94 257", "observeptr 027B9D00 [r3+0x1C]+0x1G 4",
           "observeptr 027B9D00 r3+0x1C p+0x94 8", "observenested", "observenested C00063D0 r3+0x74",
           "observenested C00063D0 r3+0x74 4 extra", "observenested C00063DG", "observenested C00063D0 r11+0 4",
           "observenested C00063D0 [r3]+0x74 0", "observenested C00063D0 r3+0x74 4x"]
    for line in bad:
        rc, row, err, out = run("orig", [line], n="1")
        print("  rc %d  %-46s %s" % (rc, line, err[0].replace(TMP, "")[-80:]))
        if rc != 2:
            fails.append("not rejected (rc %d): %s" % (rc, line))
    overlap = [[DEREF, "observearg 027B9D00 [r3+0x1C]+0x96 4"], [DEREF, "observeptr 027B9D00 [r3+0x1C]+0x90 8"]]
    for lines in overlap:
        rc, row, err, out = run("orig", lines, n="1")
        print("  rc %d  overlap %s" % (rc, " / ".join(lines)))
        if rc != 2 or "overlap" not in err[0]:
            fails.append("overlap not rejected (rc %d): %s" % (rc, lines))
    disjoint = [[DEREF, "observearg 027B9D00 [r3+0x20]+0x94 4", "observearg 027B9D00 r3+0x1C 8"],
                ["observearg 027B9D00 [r3+0x1C]+0x94 4", "observearg 027B9D00 [r3+0x1C]+0x98 4"],
                [NEST, "observenested C00063D0 [r3+0x0]+0 4", "observenested C00067C8"],
                ["observearg 027B9D00@0 [r3+0x1C]+0x94 4", "observearg 027B9D00 [r3]+0 4 if r4=0"],
                ["observearg 027B9D00 [r3]+0x94 4", "observeptr 027B9D00 r3+0x1C 4"]]
    for lines in disjoint:
        rc, row, err, out = run("orig", lines, n="20")
        print("  rc %d  accepted %s" % (rc, " / ".join(lines)))
        if rc != 0:
            fails.append("valid lines rejected or failing (rc %d): %s %s" % (rc, lines, row or err[0]))
    for x in fails:
        print("FAIL", x)
    print("nestobs_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
