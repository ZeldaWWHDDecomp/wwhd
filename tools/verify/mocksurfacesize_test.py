#!/usr/bin/env python3
"""Self-test of the unit-file `mocksurfacesize TARGET` directive (harness.cpp, src/surface_calc.cpp).

The generated mock of GX2CalcSurfaceSizeAndAlignment (C00060D0) rewrites the 0x74-byte GX2Surface at r3 with the
shipping port's formula (runtime/third_party/cemu/gx2_surface_calc.cpp), at top level and inside `real` callees.
Uses the merged spec lib2_sead_11, which models the call with `mockfield C00060D0 r3+0x20` / `r3+0x38` lines.
1. With those lines replaced by `mocksurfacesize C00060D0` the unit still passes (inputs whose random descriptor is
   outside the modelled domain abort on both sides) and the run counts surface computations.
2. Nested: `setup * 027BE74C r3=0x5F000000 r4=1` (real 027BE74C -> 027BE744 -> 0278635C -> C00060D0) on a
   1280x720 R8G8B8A8_UNORM 2D surface at 0x5F000004 gives imageSize 384000, alignment 800, pitch 500 (the port's
   values for that surface).
3. Zero width/height are modelled (the formula clamps each level's extent to
   max(x >> level, 1)): the nested 027BE74C on a 0x720 surface and on a 0x0 surface with 1 level completes and gives
   the same imageSize/alignment/pitch as the 1x720 / 1x1 surface (computed by the same run, not hard-coded).
4. Descriptors outside the modelled domain abort the input with the documented reason: width 16385, and numLevels 2
   on a 0x0 2D surface (the level count would become 0 and the outputs stay unwritten; the same request on 1x1 is
   modelled, the control that 027BE74C really passes numLevels 2).
5. Malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_mocksurfacesize_test/ and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_mocksurfacesize_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_11.txt")).read().splitlines()
ONLY = "0274B40C"  # sead::CafeGameFramework init path that calls C00060D0


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_mocksurfacesize_test/" + name, "-n", n] + list(extra),
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def surface(width, height=720, levels=1):
    # 027BE74C stores numLevels = r4 clamped to [1, u8 at obj+0x7B] (non-MSAA dims) at surface+0x10
    base = 0x5F000004
    words = {0x0: 1, 0x4: width, 0x8: height, 0xC: 1, 0x10: 1, 0x14: 0x1A, 0x18: 0, 0x1C: 1, 0x30: 0, 0x34: 0}
    return ["field * 0x%08X u32 %d %d" % (base + o, v, v) for o, v in sorted(words.items())] + \
        ["field * 0x5F00007B u8 %d %d" % (levels, levels)] + \
        ["real 027BE74C", "real 027BE744", "real 0278635C", "setup * 027BE74C r3=0x5F000000 r4=%d" % levels]


def sizes(out):
    """(imageSize, alignment, pitch) of the nested surface from the -trace line, or None"""
    for l in out.splitlines():
        if "mocksurfacesize C00060D0 surface 5F000004 -> " in l:
            w = l.split("-> ")[1].split()
            return tuple(int(w[i + 1], 16) for i in range(0, 6, 2) if w[i] in ("imageSize", "alignment", "pitch"))
    return None


ABORT = "aborted (original side): mocksurfacesize: surface descriptor outside the modelled domain [inside real 027BE74C"


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        mf = [l for l in SPEC if l.startswith("mockfield C00060D0 ")]
        if not mf:
            fails.append("lib2_sead_11 no longer models C00060D0 with mockfield lines")
        on = [l for l in SPEC if not l.startswith("mockfield C00060D0 ")] + ["mocksurfacesize C00060D0"]
        # --no-cache: a vcache hit prints no "surface calls=" line (same tools tree as an earlier run)
        rc, out = run("on", on, extra=["-only", ONLY, "--no-cache"])
        row = [l for l in out.splitlines() if l.startswith(ONLY)]
        # random top-level descriptors are often outside the modelled domain: those inputs abort on both sides
        if rc or not row or " ok" not in row[0] or "0/" in row[0].split()[2][:2]:
            fails.append("unit fails with mocksurfacesize: %s" % (row or out[-300:]))
        calls = [int(l.split("surface calls=")[1]) for l in out.splitlines() if "surface calls=" in l]
        if not calls or calls[0] == 0:
            fails.append("no top-level surface computation counted: %s" % calls)
        rc, out = run("nested", on + surface(1280), extra=["-only", ONLY, "-seed", "1", "-trace", "1000005"], n="20")
        want = "mocksurfacesize C00060D0 surface 5F000004 -> imageSize 384000 alignment 800 pitch 500"
        if want not in out or "nested call C00060D0 r3=5F000004" not in out:
            fails.append("nested surface not modelled: %s" % [l for l in out.splitlines() if "C00060D0" in l][:3])
        # zero extents: same sizes as the 1-pixel extent, computed by the formula in this run
        for name, zero, one in (("0x720", surface(0), surface(1)), ("0x0", surface(0, 0), surface(1, 1))):
            got = {}
            for tag, lines in (("zero", zero), ("one", one)):
                rc, out = run("z" + tag, on + lines, extra=["-only", ONLY, "-seed", "1", "-trace", "1000005"], n="20")
                got[tag] = sizes(out)
                if "outside the modelled domain" in out or "aborted (" in out:
                    fails.append("%s (%s) aborted: %s" % (name, tag, [l for l in out.splitlines() if "abort" in l][:2]))
            print("%s: zero %s, one %s" % (name, got["zero"], got["one"]))
            if got["zero"] is None or got["zero"] != got["one"] or len(got["zero"]) != 3:
                fails.append("%s: sizes %s differ from the 1-pixel case %s" % (name, got["zero"], got["one"]))
        # control for the numLevels case: the same 2-level request on a 1x1 surface is modelled
        rc, out = run("lv2", on + surface(1, 1, 2), extra=["-only", ONLY, "-seed", "1", "-trace", "1000005"], n="20")
        if sizes(out) is None or "aborted (" in out:
            fails.append("numLevels 2 at 1x1 not modelled: %s" % [l for l in out.splitlines() if "abort" in l or "C00060D0" in l][:3])
        else:
            print("numLevels 2 at 1x1: %s" % (sizes(out),))
        for name, lines in (("width 16385", surface(16385)), ("numLevels 2 at 0x0", surface(0, 0, 2))):
            rc, out = run("domain", on + lines, extra=["-only", ONLY, "-seed", "1", "-trace", "1000005"], n="20")
            if ABORT not in out:
                fails.append("%s not aborted: %s" % (name, [l for l in out.splitlines() if "abort" in l or "C00060D0" in l][:3]))
            else:
                print("%s: aborted (outside the modelled domain)" % name)
        for badl in ("mocksurfacesize", "mocksurfacesize C00060D0 extra", "mocksurfacesize zz"):
            rc, _ = run("bad", on + [badl], extra=["-only", ONLY], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, badl))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_mocksurfacesize_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mocksurfacesize_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
