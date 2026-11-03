#!/usr/bin/env python3
"""Self-test of harness -observe-pointees (2026-10-28): pointee objects of undeclared heap/static pointer arguments at
mocked calls.

Carrier: the merged spec lib2_agl_5/depth_of_field, function 02781570 (DepthOfField::setVignettingUniforms). Its colour
send passes &settings->mVignettingColorParameter (0x610201E0, inside the steered settings object at 0x61020000) to
GX2SetVertexUniformReg (C0006A40) in r5; the other sends pass a stack uniform (r1+8). The test spec describes a sead
ExpHeap at 0x5F000000 (`observeheap`) whose one used block is [0x61020000, +0x400) (header at 0x6101FFF0), so the colour
pointer lies in a real heap block.
Candidate `flip`: flips one byte of the colour (+0x1E3 in the block) just before the colour send and restores it just
after, so the exit bytes and the call registers equal the original's; only the pointee at the call differs.
1. Undeclared r5 (the C0006A40/C0006908 pointee facts removed): `flip` passes without the switch and fails with it, the
   message naming the call, r5, the block base/size and offset +0x1E3; the unchanged candidate passes with it (baseline).
2. Declared pointee wins: with `callee C0006A40 r5=0` (value only) `flip` passes with the switch.
3. Stack pointers keep their rules: candidate `late` (writes the radius uniform's w after its stack send) passes with and
   without the switch.
4. Cap: -observe-pointees-cap 16 (window around the pointer contains +0x1E3) fails with "(truncated to the cap)";
   -observe-pointees-cap 4 (window [+0x1DC, +0x1E0)) passes.
5. Without an `observeheap` line the block is unknown (the default root 0x6A000000 holds no valid ExpHeap here): `flip`
   passes with the switch; `-observe-heap 5F000000` on the command line instead of the line catches it.
6. Static pointers: mkstatics boundaries; pointee_block of a .data address lies between two boundaries (checked
   through statics.tsv here: sorted, inside the sections).
Builds go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set), removed at exit.
"""
import atexit
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
NAME = "_observepointees_test"
TMP = os.path.join(UNITS, NAME)
SRC = "wwhd_src/agl/DepthOfFieldDraw.cpp"
F = "02781570"
COLOR_SEND = "    setUniform(programUniform(program, cUniform_Color), &arg->mParameters->mVignettingColorParameter.mValue.r);\n"
COLOR_FLIP = """    {
        gabi::be<u8>& flip = *(gabi::be<u8>*)((u8*)&arg->mParameters->mVignettingColorParameter.mValue.r + 3);
        bool fixture = gabi::ea(&flip) == 0x610201E3u; /* only the steered settings object (stable: no mock clobbers it) */
        u8 old = flip;
        if (fixture) flip = (u8)(old ^ 0x10);
        setUniform(programUniform(program, cUniform_Color), &arg->mParameters->mVignettingColorParameter.mValue.r);
        if (fixture) flip = old;
    }
"""
WRITE_IN_ORDER = """    values->w = gabi::fmadds(1.1f, invScale, shift);
    setUniform(programUniform(program, cUniform_Radius), values);
"""
WRITE_AFTER_SEND = """    setUniform(programUniform(program, cUniform_Radius), values);
    values->w = gabi::fmadds(1.1f, invScale, shift);
"""
H, HDR = 0x5F000000, 0x6101FFF0
HEAP = [  # a sead ExpHeap as the original lays it out (harness.cpp exp_block): one used block [0x61020000, +0x400)
    "field %s 0x%08X u32 0x101459E0" % (F, H + 0x0C),
    "field %s 0x%08X u32 0x%08X" % (F, H + 0x1C, HDR), "field %s 0x%08X u32 0x420" % (F, H + 0x20),
    "field %s 0x%08X u32 0x%08X" % (F, H + 0x28, H + 0x28), "field %s 0x%08X u32 0x%08X" % (F, H + 0x2C, H + 0x28),
    "field %s 0x%08X u32 0" % (F, H + 0x30), "field %s 0x%08X u32 0" % (F, H + 0x34),
    "field %s 0x%08X u32 0x%08X" % (F, H + 0xA8, HDR), "field %s 0x%08X u32 0x%08X" % (F, H + 0xAC, HDR),
    "field %s 0x%08X u32 1" % (F, H + 0xB0), "field %s 0x%08X u32 0" % (F, H + 0xB4),
    "field %s 0x%08X u32 0x%08X" % (F, HDR, H + 0xA8), "field %s 0x%08X u32 0x%08X" % (F, HDR + 4, H + 0xA8),
    "field %s 0x%08X u32 0x400" % (F, HDR + 8), "field %s 0x%08X u32 0" % (F, HDR + 12),
    "stable 0x%08X 0x100" % H, "stable 0x%08X 0x10" % HDR, "stable 0x61020000 0x400",
]


def spec(src_line, r5):
    """depth_of_field with this src and the C0006A40/C0006908 r5 fact: None = removed, else the fact text"""
    out = []
    for line in open(os.path.join(UNITS, "lib2_agl_5", "depth_of_field.txt")).read().splitlines():
        if line.startswith("callee C0006A40 r5=") or line.startswith("callee C0006908 r5="):
            if r5 is None:
                continue
            line = line.split(" r5=")[0] + " r5=" + r5
        elif line.strip() == "src " + SRC:
            line = "src " + src_line
        out.append(line)
    return out


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-observepointees_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    atexit.register(shutil.rmtree, scratch, True)
    atexit.register(shutil.rmtree, TMP, True)
    os.makedirs(TMP, exist_ok=True)
    import verify
    src = open(os.path.join(ROOT, SRC)).read()
    if src.count(COLOR_SEND) != 1 or src.count(WRITE_IN_ORDER) != 1:
        print("FAIL carrier source changed: colour/radius send not found")
        return 1
    srcs = {"orig": SRC}
    for k, text in (("flip", src.replace(COLOR_SEND, COLOR_FLIP)), ("late", src.replace(WRITE_IN_ORDER, WRITE_AFTER_SEND))):
        d = os.path.join(TMP, k)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, "DepthOfFieldDraw.cpp"), "w") as fh:
            fh.write(text)
        srcs[k] = os.path.relpath(os.path.join(d, "DepthOfFieldDraw.cpp"), ROOT)
    exe = {}  # (candidate, r5 fact): callee facts are compiled into unit.c, so each declaration is its own build
    for k, r5, tag in (("orig", None, "orig"), ("flip", None, "flip"), ("late", None, "late"), ("flip", "0", "flip_r5zero"),
                       ("flip", "dyn4*r4:heap16", "flip_heap16")):
        with open(os.path.join(TMP, tag + ".txt"), "w") as fh:
            fh.write("\n".join(spec(srcs[k], r5)) + "\n")
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), NAME + "/" + tag, "-n", "1", "-only", F,
                            "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
        exe[(k, r5)] = os.path.join(verify.unit_dir(NAME + "/" + tag), "verify")
        if not os.path.exists(exe[(k, r5)]):
            print("FAIL build %s: rc %d %s" % (tag, r.returncode, (r.stdout + r.stderr)[-600:]))
            return 1
    fails = []
    nrun = [0]

    def run(k, r5, lines, opts):
        nrun[0] += 1
        path = os.path.join(TMP, "run%d.txt" % nrun[0])
        with open(path, "w") as fh:
            fh.write("\n".join(lines) + "\n")
        r = subprocess.run([exe[(k, r5)], "-spec", path, "-n", "30", "-seed", "1", "-only", F, "-noprogress"] + opts, cwd=ROOT,
                           capture_output=True, text=True)
        row = [l for l in r.stdout.splitlines() if l.startswith(F)]
        return r.returncode, (row[-1] if row else ""), (r.stderr.strip().splitlines() or [""])[-1]

    def expect(what, k, r5, heap, opts, ok, needles=()):
        lines = spec(srcs[k], r5) + (["observeheap %08X" % H] if heap else []) + HEAP
        rc, row, err = run(k, r5, lines, opts)
        good = (rc == 0 and row.endswith(" ok")) if ok else (rc == 1 and all(n in row for n in needles))
        print("  %-48s %-5s %s" % (what, k, row[42:][:150] or err[:120]))
        if not good:
            fails.append("%s: %s rc %d %s %s" % (what, k, rc, row, err))

    OP = ["-observe-pointees"]
    print("1. undeclared heap pointer")
    expect("flip passes without the switch", "flip", None, True, [], True)
    expect("flip fails with the switch", "flip", None, True, OP, False,
           ("C0006A40", "r5=610201E0 -> heap block 61020000 size 0x400 differs at +0x1E3 (610201E3)", "(-observe-pointees"))
    rc, row, err = run("orig", None, spec(srcs["orig"], None) + ["observeheap %08X" % H] + HEAP, OP)
    if "observe-pointees: " not in err or " 0 heap-block" in err:
        fails.append("no heap-block observation counted: %s" % err)
    expect("unchanged candidate passes with the switch", "orig", None, True, OP, True)
    print("2. declared pointee wins")
    expect("flip passes with r5=0 declared", "flip", "0", True, OP, True)
    expect("flip caught by the declared heap16 as before", "flip", "dyn4*r4:heap16", True, [], False, ("heap data differs",))
    print("3. stack pointers unchanged")
    expect("late passes without the switch", "late", None, True, [], True)
    expect("late passes with the switch", "late", None, True, OP, True)
    print("4. cap")
    expect("cap 16: caught, truncated", "flip", None, True, OP + ["-observe-pointees-cap", "16"], False,
           ("(truncated to the cap)", "+0x1E3"))
    expect("cap 4: outside the window", "flip", None, True, OP + ["-observe-pointees-cap", "4"], True)
    print("5. no observeheap: no block")
    expect("flip passes without a heap", "flip", None, False, OP, True)
    expect("-observe-heap instead of the line", "flip", None, False, OP + ["-observe-heap", "%08X" % H], False,
           ("heap block 61020000 size 0x400 differs at +0x1E3",))
    print("6. static boundaries")
    marks = [int(x, 16) for x in open(os.path.join(ROOT, "build", "verify", "statics.tsv")).read().split()]
    if marks != sorted(marks) or len(marks) < 1000 or marks[0] != 0x10000000:
        fails.append("statics.tsv: %d marks, first %s" % (len(marks), hex(marks[0]) if marks else None))
    rc, row, err = run("orig", None, spec(srcs["orig"], None), OP + ["-observe-pointees-cap", "3"])
    if rc != 2:
        fails.append("cap 3 not rejected (rc %d)" % rc)
    for x in fails:
        print("FAIL", x)
    print("observepointees_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
