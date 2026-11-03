#!/usr/bin/env python3
"""Self-test of the per-call-site forms of `observearg` (harness.cpp, 2026-10-28, AGL#12 request).

`observearg TARGET rN+OFF SIZE` compares SIZE bytes at rN+OFF at EVERY top-level call to TARGET. Where one import is
called at several sites and the original leaves its own temporaries next to the buffer at one of them, the span can
only be observed per site:
  `observearg TARGET@N rN+OFF SIZE`          only the N-th top-level call to TARGET in the run (0-based)
  `observearg TARGET rN+OFF SIZE if rK=V`    only calls where the ORIGINAL's rK (r3..r10) is V
  `observearg TARGET rN+OFF SIZE if lr=ADDR` only calls the ORIGINAL makes with return address ADDR (its call site)
The candidate's call at the same position is then compared whatever its own registers hold.

Carrier: the merged spec lib2_agl_5/depth_of_field, function 02781570 (DepthOfField::setVignettingUniforms), with the
GX2SetVertexUniformReg / GX2SetPixelUniformReg pointee facts (`r5=dyn4*r4:heap16`) reduced to plain register compares
(`r5=0`), as a spec that observes nothing behind r5. Its vertex sends: radius (r1+8, return 027817B8), param (r1+8,
02781968), colour (heap 0x610201E0, 027819B8), translation (r1+8, 02781A18). The original keeps two scale temporaries
at r1+0x18..0x1F, right behind the 16-byte stack uniform.
1. Write after send: a candidate that stores the radius uniform's w AFTER the radius send (the exit bytes are
   unchanged: the buffer is overwritten before the next send) passes without observation and is caught by
   `observearg C0006A40 r5+0 16 if lr=027817B8` and by `observearg C0006A40@0 r5+0 16`; the unchanged candidate passes
   both. (`final` compares exit bytes only, which this candidate gets right.)
2. Two call sites: a 24-byte span on every call fails the baseline (the stack sites carry the original's temporaries
   at +0x10); the same span restricted to the colour site (`if lr=027819B8`, or `if r5=0x610201E0`) passes next to a
   16-byte span at the radius site, while the other stack sites stay unobserved.
3. Malformed lines exit 2 (strict tokens), overlapping spans that can meet at one call exit 2, spans that cannot meet
   (same condition register, different values) are accepted.
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
NAME = "_observesite_test"
TMP = os.path.join(UNITS, NAME)
SRC = "wwhd_src/agl/DepthOfFieldDraw.cpp"
F = "02781570"
WRITE_IN_ORDER = """    values->w = gabi::fmadds(1.1f, invScale, shift);
    setUniform(programUniform(program, cUniform_Radius), values);
"""
WRITE_AFTER_SEND = """    setUniform(programUniform(program, cUniform_Radius), values);
    values->w = gabi::fmadds(1.1f, invScale, shift);
"""


def base_spec(src_line):
    out = []
    for line in open(os.path.join(UNITS, "lib2_agl_5", "depth_of_field.txt")).read().splitlines():
        if line.startswith("callee C0006A40 r5=") or line.startswith("callee C0006908 r5="):
            line = line.split(" r5=")[0] + " r5=0"
        elif line.strip() == "src " + SRC:
            line = "src " + src_line
        out.append(line)
    return out


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-observesite_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    atexit.register(shutil.rmtree, scratch, True)
    atexit.register(shutil.rmtree, TMP, True)
    os.makedirs(TMP, exist_ok=True)
    import verify
    src = open(os.path.join(ROOT, SRC)).read()
    if src.count(WRITE_IN_ORDER) != 1:
        print("FAIL carrier source changed: radius send not found")
        return 1
    with open(os.path.join(TMP, "DepthOfFieldDraw.cpp"), "w") as fh:
        fh.write(src.replace(WRITE_IN_ORDER, WRITE_AFTER_SEND))
    specs = {"orig": base_spec(SRC), "late": base_spec(os.path.relpath(os.path.join(TMP, "DepthOfFieldDraw.cpp"), ROOT))}
    exe = {}
    for k, lines in specs.items():
        with open(os.path.join(TMP, k + ".txt"), "w") as fh:
            fh.write("\n".join(lines) + "\n")
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), NAME + "/" + k, "-n", "1", "-only", F,
                            "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
        exe[k] = os.path.join(verify.unit_dir(NAME + "/" + k), "verify")
        if r.returncode or not os.path.exists(exe[k]):
            print("FAIL build %s: rc %d %s" % (k, r.returncode, (r.stdout + r.stderr)[-600:]))
            return 1
    fails = []
    nrun = [0]

    def run(k, extra, n="20", seed="1"):
        nrun[0] += 1
        path = os.path.join(TMP, "run%d.txt" % nrun[0])
        with open(path, "w") as fh:
            fh.write("\n".join(specs[k] + list(extra)) + "\n")
        r = subprocess.run([exe[k], "-spec", path, "-n", n, "-seed", seed, "-only", F, "-noprogress"], cwd=ROOT,
                           capture_output=True, text=True)
        row = [l for l in r.stdout.splitlines() if l.startswith(F)]
        return r.returncode, (row[-1] if row else ""), r.stderr.strip().splitlines()[-1:] or [""]

    def expect(what, k, extra, ok, needle=None):
        rc, row, err = run(k, extra)
        good = (rc == 0 and row.endswith(" ok")) if ok else (" ok" not in row[-4:] and (needle is None or needle in row))
        print("  %-5s %-62s %s" % (k, "; ".join(extra)[:62], row[42:] or err[0][:100]))
        if not good:
            fails.append("%s: %s rc %d %s %s" % (what, k, rc, row, err[0]))

    print("1. write after send")
    expect("late passes unobserved", "late", [], True)
    expect("late caught by lr site", "late", ["observearg C0006A40 r5+0 16 if lr=027817B8"], False,
           "call #0 C0006A40 GX2SetVertexUniformReg: argument span r5+0x0 differs at r5+0xC (observearg if lr=027817B8)")
    expect("late caught by @0", "late", ["observearg C0006A40@0 r5+0 16"], False, "(observearg @0)")
    expect("late not caught at another site", "late", ["observearg C0006A40 r5+0 16 if lr=02781A18"], True)
    expect("orig passes lr site", "orig", ["observearg C0006A40 r5+0 16 if lr=027817B8"], True)
    expect("orig passes @0", "orig", ["observearg C0006A40@0 r5+0 16"], True)
    print("2. two call sites")
    expect("24 bytes everywhere fails the baseline", "orig", ["observearg C0006A40 r5+0 24"], False,
           "argument span r5+0x0 differs at r5+0x10 (observearg)")
    expect("24 bytes at the colour site only", "orig",
           ["observearg C0006A40 r5+0 24 if lr=027819B8", "observearg C0006A40 r5+0 16 if lr=027817B8"], True)
    expect("24 bytes at the colour site by r5", "orig", ["observearg C0006A40 r5+0 24 if r5=0x610201E0"], True)
    expect("stack sites unconditional 16 still pass", "orig", ["observearg C0006A40 r5+0 16"], True)
    print("3. parser")
    bad = ["observearg C0006A40@ r5+0 16", "observearg C0006A40@x r5+0 16", "observearg C0006A40@-1 r5+0 16",
           "observearg C0006A4G r5+0 16", "observearg C0006A40 r5+0x1G 16", "observearg C0006A40 r5+0 16x",
           "observearg C0006A40 r5+0 0", "observearg C0006A40 r5+0 257", "observearg C0006A40 r2+0 16",
           "observearg C0006A40 r5-4 16", "observearg C0006A40 r5+0 16 if", "observearg C0006A40 r5+0 16 when r4=4",
           "observearg C0006A40 r5+0 16 if r4", "observearg C0006A40 r5+0 16 if r4=", "observearg C0006A40 r5+0 16 if r4=4x",
           "observearg C0006A40 r5+0 16 if r11=4", "observearg C0006A40 r5+0 16 if f1=4", "observearg C0006A40 r5+0 16 if lr=0278ZZ",
           "observearg C0006A40 r5+0 16 if r4=0x100000000", "observearg C0006A40 r5+0 16 if r4=4 extra",
           "observearg C0006A40 r5+0 16 extra"]
    for line in bad:
        rc, row, err = run("orig", [line], n="1")
        print("  rc %d  %-58s %s" % (rc, line, err[0].replace(TMP, "")[-90:]))
        if rc != 2:
            fails.append("not rejected (rc %d): %s" % (rc, line))
    overlap = [["observearg C0006A40 r5+0 16", "observearg C0006A40@1 r5+8 8"],
               ["observearg C0006A40@1 r5+0 16", "observearg C0006A40 r5+0 8 if lr=027817B8"],
               ["observearg C0006A40 r5+0 16 if r4=4", "observearg C0006A40 r5+0 8 if r3=0"],
               ["observearg C0006A40@1 r5+0 16", "observearg C0006A40@1 r5+4 4"]]
    for lines in overlap:
        rc, row, err = run("orig", lines, n="1")
        print("  rc %d  overlap %s" % (rc, " / ".join(lines)))
        if rc != 2 or "overlap" not in err[0]:
            fails.append("overlap not rejected (rc %d): %s" % (rc, lines))
    disjoint = [["observearg C0006A40@0 r5+0 16", "observearg C0006A40@1 r5+0 16", "observearg C0006A40@2 r5+0 24"],
                ["observearg C0006A40 r5+0 16 if lr=027817B8", "observearg C0006A40 r5+0 24 if lr=027819B8"],
                ["observearg C0006A40 r5+0 16 if lr=027817B8", "observearg C0006A40 r5+0x10 8 if lr=027819B8"]]
    for lines in disjoint:
        rc, row, err = run("orig", lines, n="1")
        print("  rc %d  accepted %s" % (rc, " / ".join(lines)))
        if rc == 2:
            fails.append("disjoint spans rejected: %s %s" % (lines, err[0]))
    for x in fails:
        print("FAIL", x)
    print("observesite_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
