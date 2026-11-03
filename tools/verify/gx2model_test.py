#!/usr/bin/env python3
"""Self-test of `mockretfield TARGET rN OFF SHIFT MASK` and `mockdisplaylist BEGIN END GETCUR [BYTES]` (harness.cpp, 2026-10-26;
BYTES 2026-10-29).
Parser checks on lib2_sead_15 (the directives are accepted and inert for a unit that never calls the targets; malformed
lines exit 2). The functional check (exact HLE semantics on the AGL#3 ShaderMgr startup: GX2Get*ShaderGPRs/StackEntries
returning the shader word fields, and the GX2 display-list Begin/End/GetCurrent trio) ran in the AGL#3 recipe
(6832 Begin/End/GetCurrent triples, setupsnapshot accepted, seeds 1/7 pass) and is recorded in the README row.
Command condition (2026-10-29, ): END returns min(BYTES, open size) only when at least one GX2 import call
(gx2 slots other than BEGIN/END/GETCUR) was made since BEGIN. Checked on the original DisplayListRecording pair
027C073C begin / 027C077C end (a recording object at 0x5F000000 over storage 0x5F100000, capacity 0x400), with
`-trace` reading the END line and the stored mUsed (027C077C returns it):
  (a) empty recording (begin, end) -> 0 even with BYTES 0x100;
  (b) one GX2 call in between -> min(0x100, 0x400) = 0x100, and min(0x800, 0x400) = 0x400: nested (a setup of the
      real 027BF730 -> GX2DrawDone C0006168, a `nested call`) and top level (the tested function's own call of
      C0006218, with the directive's GETCUR slot moved to an import nobody calls);
  (c) BYTES absent -> 0 with or without the GX2 call (legacy model).
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_gx2model_test")
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
F = "02759564"


def run(lines, n="200"):
    with open(os.path.join(TMP, "g.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_gx2model_test/g", "-only", F, "-n", n, "-shards", "1"],
                       cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(F)]
    return r.returncode, (row[0].endswith(" ok") if row else None)


OBJ = ["src wwhd_src/agl/RenderRecording.cpp",
       "field * 0x5F000000 u32 0x5F100000", "field * 0x5F000004 u32 0x400", "field * 0x5F000008 u32 0x77",
       "stable 5F000000 0xC", "arg! 027C077C r3 0x5F000000", "arg! 027C077C r4 0",
       "real 027C073C", "real 027BF730", "setup * 027C073C r3=0x5F000000 r4=0"]


def run_dl(extra_lines, only="027C077C"):
    """trace of one input of the recording pair; returns (END trace lines, the 027C077C row, whole output)"""
    with open(os.path.join(TMP, "d.txt"), "w") as f:
        f.write("\n".join(OBJ + extra_lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_gx2model_test/d", "-only", only, "-n", "4",
                        "-shards", "1", "-seed", "1", "-trace", "1000003"], cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    ends = [l.strip() for l in out.splitlines() if "mockdisplaylist END" in l]
    row = [l for l in out.splitlines() if l.startswith(only)]
    return ends, row, out


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        dl = "mockdisplaylist C0006090 C0006190 C0006218"
        cases = [  # (name, lines, expected END value)
            ("a empty BYTES 0x100", [dl + " 0x100"], 0),
            ("b nested GX2 call BYTES 0x100", [dl + " 0x100", "setup * 027BF730"], 0x100),
            ("b nested GX2 call BYTES 0x800 (capped)", [dl + " 0x800", "setup * 027BF730"], 0x400),
            ("c legacy no BYTES, empty", [dl], 0),
            ("c legacy no BYTES, GX2 call", [dl, "setup * 027BF730"], 0),
        ]
        for name, lines, want in cases:
            ends, row, out = run_dl(lines)
            vals = [int(e.split("-> ")[1].split()[0], 16) for e in ends]
            if not vals or any(v != want for v in vals):
                fails.append("%s: END values %s, want %X (%s)" % (name, [hex(v) for v in vals], want, out[-400:] if not vals else ""))
            if not row or not row[0].endswith(" ok"):
                fails.append("%s: unit row fails: %s" % (name, row))
        # top level: the tested function's own GX2 import call. 027C077C calls GX2GetCurrentDisplayList (C0006218) before
        # END; with the directive's GETCUR slot moved to an import nobody calls (C0006AE8), that call is an ordinary mocked
        # GX2 import made by the tested function at depth 0 while the setup's recording is open
        ends, row, out = run_dl([dl.replace("C0006218", "C0006AE8") + " 0x100"])
        vals = [int(e.split("-> ")[1].split()[0], 16) for e in ends]
        if not vals or any(v != 0x100 for v in vals) or "        call C0006218" not in out:
            fails.append("top-level GX2 call: END values %s, want 0x100 (top-level call traced: %s)" %
                         ([hex(v) for v in vals], "        call C0006218" in out))
        rc, ok = run(SPEC + ["mockretfield C0006390 r3 0 0 0xFF", "mockretfield C0006398 r3 0 8 0xFF",
                             "mockdisplaylist C0006090 C0006190 C0006218", "mockdisplaylist C0006090 C0006190 C0006218 0x100"])
        if not ok:
            fails.append("valid directives rejected or unit fails (rc %d)" % rc)
        for bad in ("mockretfield C0006390 r2 0 0 0xFF", "mockretfield C0006390 r3 0 40 0xFF", "mockretfield C0006390 r3 0",
                    "mockdisplaylist C0006090 C0006190", "mockdisplaylist 0 C0006190 C0006218",
                    "mockdisplaylist C0006090 C0006190 C0006218 0x100 7"):
            rc, _ = run(SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_gx2model_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("gx2model_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
