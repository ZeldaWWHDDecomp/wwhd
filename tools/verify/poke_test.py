#!/usr/bin/env python3
"""Self-test of the unit-file `poke FUNC|* ADDR|rN+OFF u8|u16|u32|f32 VALUE` directive (harness.cpp): after the
`setup` lines and before the tested function runs (both sides), the value is stored, e.g. to select a registered
mode in an object a setup constructor initialised (field steering is applied before the setups, so it cannot).

Uses the merged spec lib2_sead_15 (real 02759928 clears *r3), tested function 02759564:
1. `setup 02759564 02759928 r3=0x5E000000` then `poke 02759564 5E000000 u32 0x12345678` with `final 02759564 5E000000 4`:
   the unit passes, and a traced input shows the setup, then the poke, on both sides.
2. Choices: `poke 02759564 5E000000 u32 0x11,0x22,0x33` picks one value per input, the same on both sides, and
   traced inputs see more than one of the values; the unit passes.
3. Malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_poke_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_poke_test")
SPEC = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().splitlines()
FUNC = "02759564"


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_poke_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        on = SPEC + ["setup %s 02759928 r3=0x5E000000" % FUNC, "poke %s 5E000000 u32 0x12345678" % FUNC,
                     "final %s 5E000000 4" % FUNC]
        rc, out = run("on", on, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with setup+poke: %s" % (row or out[-300:]))
        rc, out = run("trace", on, extra=["-seed", "1", "-trace", "1000003"], n="20")
        seq = re.findall(r"(setup 02759928 -> r3 5E000000|poke 5E000000 <- 12345678)", out)
        want = ["setup 02759928 -> r3 5E000000", "poke 5E000000 <- 12345678"] * 2
        if seq != want:
            fails.append("traced order %s (want setup then poke on both sides)" % seq)
        for bad in ("poke %s 5E000000 u32" % FUNC, "poke %s 5E000000 u64 1" % FUNC, "poke %s r2+4 u32 1" % FUNC,
                    "poke %s 5E000000 u32 zz" % FUNC, "poke %s 5E000000 f32 1.5x" % FUNC, "poke zz 5E000000 u8 1"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_poke_test"), ignore_errors=True)
    try:
        os.makedirs(TMP, exist_ok=True)
        ch = SPEC + ["poke %s 5E000000 u32 0x11,0x22,0x33" % FUNC]
        rc, out = run("choice", ch, n="1000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with poke choices: %s" % (row or out[-300:]))
        seen = set()
        for sd in range(1000003, 1000011):
            rc, out = run("choicet", ch, extra=["-seed", "1", "-trace", str(sd)], n="10")
            got = re.findall(r"poke 5E000000 <- ([0-9A-F]{8})", out)
            if len(got) != 2 or got[0] != got[1]:
                fails.append("poke choice differs between the sides or missing: %s" % got)
            seen.update(got)
        if not seen <= {"00000011", "00000022", "00000033"} or len(seen) < 2:
            fails.append("poke choices not picked per input: %s" % sorted(seen))
        rc, _ = run("badc", SPEC + ["poke %s 5E000000 u32 0x11,zz" % FUNC], n="1")
        if rc != 2:
            fails.append("bad choice list not rejected (rc %d)" % rc)
        # a long choice list (> 63 characters, 2026-10-26: the parser truncated VALUE and rejected the line)
        lc = ",".join("0x%08X" % (0xABCDEF00 + i) for i in range(12))
        rc, out = run("long", SPEC + ["poke %s 5E000000 u32 %s" % (FUNC, lc)], extra=["-seed", "1", "-trace", "1000003"], n="10")
        got = re.findall(r"poke 5E000000 <- ([0-9A-F]{8})", out)
        if rc or len(got) != 2 or not got[0].startswith("ABCDEF"):
            fails.append("long choice list not accepted: rc %d %s" % (rc, got))
        # r1+OFF: a caller-frame stack word at the entry SP (OFF >= 8), the same address on both sides
        sp = SPEC + ["poke %s r1+0x8 u32 0x11,0x22" % FUNC]
        rc, out = run("sp", sp, n="500")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with an r1+8 poke: %s" % (row or out[-300:]))
        rc, out = run("spt", sp, extra=["-seed", "1", "-trace", "1000003"], n="10")
        got = re.findall(r"poke ([0-9A-F]{8}) <- (000000(?:11|22))", out)
        if len(got) != 2 or got[0] != got[1] or not got[0][0].startswith("7"):
            fails.append("r1+8 poke not on the entry stack identically on both sides: %s" % got)
        for bad in ("poke %s r1+0x4 u32 1" % FUNC, "poke %s r1 u32 1" % FUNC):
            rc, _ = run("badsp", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_poke_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("poke_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
