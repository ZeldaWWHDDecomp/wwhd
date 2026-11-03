#!/usr/bin/env python3
"""Self-test of mock-model rejections as compared calls (harness model_reject / model_compare, 2026-10-29).

A mock model's domain check (mocksurfacesize `surface descriptor outside the modelled domain`, mocklookup index, alloc
alignment/size, fetch-shader stream count) used to abort that side before anything was compared, and aborts never count:
a candidate-only out-of-model argument was `only one side aborted` (mutate class abort), and a call difference before a
rejection on both sides was hidden. Now the rejected call is recorded first and the calls up to it are compared.

Case b is the AGL#3 mip zero-dimension shape (a candidate-only out-of-domain width at C00060D0; since 2026-10-30 width 0
is modelled, so the candidate stores width 16385, still outside the domain).
Uses lib2_agl_5's TextureDataUtil::calculateSizeAndAlignment 0278635C (one call: C00060D0 with r3 = the surface), with a
temporary spec: `mocksurfacesize C00060D0`, r3 = 0x50000000 and an in-domain surface there (2D, 1..64 x 1..64, format 0x1A):
a. unmodified: ok, no abort (same row as before the change);
b. the candidate stores width 16385 before the call: `call #0 ...: argument outside the modelled domain on the candidate side
   only (mocksurfacesize: surface descriptor outside the modelled domain)`, and mutate.classify says `difference`
   (before: `only one side aborted: ...`, class abort);
c. the candidate passes surface + 0x74 (random bytes, rejected on most inputs): an ordinary argument difference
   `call #0 ...: r3 original 50000000 candidate 50000074`;
d. a fixture outside the domain on both sides (width 16385) with the case-c candidate: `call #0 ...: r3 ... (before mock-model
   rejections on both sides)` (before: every input aborted on both sides, row ok);
e. the same out-of-domain fixture with the unmodified candidate: every input aborted on both sides (0/0), as before.
Writes a temporary unit directory (tools/verify/units/_modelreject_test) and variant sources next to the original
(overwritten by the next run, never deleted). Builds go below WWHD_VERIFY_SCRATCH when set. Exit 0 when all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_modelreject_test")
F = "0278635C"
SRC = "wwhd_src/agl/TextureDataUtil.cpp"
SITE = "    calculateSurfaceSizeAndAlignment(surface);\n"
VARIANTS = {
    "base": None,
    "bigwidth": "    gabi::store<u32>(gabi::ea(surface) + 4, 16385);\n    calculateSurfaceSizeAndAlignment(surface);\n",
    "shifted": "    calculateSurfaceSizeAndAlignment(gabi::at<sead::GraphicsSurface>(gabi::ea(surface) + 0x74));\n",
}
SURF = 0x50000000


def fixture(width):
    return ["mocksurfacesize C00060D0", "arg! %s r3 0x%X" % (F, SURF), "stable 0x%X 0x100" % SURF,
            "fill %s 0x%X 0x74 0" % (F, SURF),
            "field %s 0x%X u32 1" % (F, SURF), "field %s 0x%X u32 %s" % (F, SURF + 4, width),
            "field %s 0x%X u32 1 64" % (F, SURF + 8), "field %s 0x%X u32 1" % (F, SURF + 0xC),
            "field %s 0x%X u32 1" % (F, SURF + 0x10), "field %s 0x%X u32 0x1A" % (F, SURF + 0x14),
            "field %s 0x%X u32 0 4" % (F, SURF + 0x30)]


def spec(name, variant, width):
    os.makedirs(TMP, exist_ok=True)
    src = SRC
    if VARIANTS[variant] is not None:
        text = open(os.path.join(ROOT, SRC)).read()
        if text.count(SITE) != 1:
            raise SystemExit("site not found once in %s" % SRC)
        src = "wwhd_src/agl/_modelreject_test_%s_TextureDataUtil.cpp" % variant
        open(os.path.join(ROOT, src), "w").write(text.replace(SITE, VARIANTS[variant]))
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("# verify: -strictnan\nsrc %s\n%s\n" % (src, "\n".join(fixture(width))))
    return "_modelreject_test/" + name


def run(unit):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-only", F, "-n", "100", "-seed", "1",
                        "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
    rows = [l for l in r.stdout.splitlines() if l.startswith(F)]
    return r.returncode, (rows[0] if rows else r.stdout[-300:] + r.stderr[-300:]), r.stdout + r.stderr


def main():
    import mutate
    fails = []
    cases = [("a", "base", "1 64"), ("b", "bigwidth", "1 64"), ("c", "shifted", "1 64"), ("d", "shifted", "16385"), ("e", "base", "16385")]
    for tag, variant, width in cases:
        rc, row, out = run(spec(tag + "_" + variant, variant, width))
        cls = mutate.classify(rc, out) if rc else "ok"
        print("%s. %s width %s: rc %d class %s\n   %s" % (tag, variant, width, rc, cls, row))
        if tag == "a" and (rc or " ok" not in row or "aborted" in row):
            fails.append("a: want ok without aborts, got %s" % row)
        if tag == "b" and ("argument outside the modelled domain on the candidate side only (mocksurfacesize: surface descriptor "
                           "outside the modelled domain)" not in row or "call #0" not in row or cls != "difference"):
            fails.append("b: want the candidate-only model difference at call #0 (class difference), got %s %s" % (cls, row))
        if tag == "c" and ("call #0" not in row or "r3 original 50000000 candidate 50000074" not in row or cls != "difference"):
            fails.append("c: want an ordinary r3 argument difference at call #0, got %s %s" % (cls, row))
        if tag == "d" and ("r3 original 50000000 candidate 50000074 (before mock-model rejections on both sides)" not in row
                           or cls != "difference"):
            fails.append("d: want the r3 difference before rejections on both sides, got %s %s" % (cls, row))
        if tag == "e" and (" 0/0 " not in row or "(some inputs aborted on both sides)" not in row or "call #" in row):
            fails.append("e: want every input aborted on both sides (0/0, no difference), got %s" % row)
    for x in fails:
        print("FAIL", x)
    print("modelreject_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
