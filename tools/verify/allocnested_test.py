#!/usr/bin/env python3
"""Self-test of the unit-file `allocnested TARGET` directive (harness.cpp).

`alloc`/`allocfixed` steer an allocator's top-level calls only; inside a `real` callee its calls were answered
with a generated pointer (or a fixed `nestedret`). `allocnested TARGET` (after TARGET's alloc line) makes nested
calls take the same deterministic distinct blocks.

Uses the merged spec lib_js_jaudio_1/aaf_tables (real 02803C78 allocates through 0273B0D4, `alloc 0273B0D4 r3
0x60000000 64`; the spec pins nested results with `nestedret 0273B0D4 0x60000000`).
1. With the nestedret line replaced by `allocnested 0273B0D4` the unit passes, and a traced input shows nested
   allocations returning distinct 64-byte-aligned blocks from 0x60000000 in call order.
2. Malformed lines exit 2 (no alloc line for the target, extra token, bad address).
Writes temporary unit files under tools/verify/units/_allocnested_test/ and removes them. Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_allocnested_test")
SPEC = open(os.path.join(UNITS, "lib_js_jaudio_1", "aaf_tables.txt")).read().splitlines()
FUNC = "02804814"


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_allocnested_test/" + name, "-n", n] + list(extra),
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if "nestedret 0273B0D4 0x60000000" not in SPEC or not any(l.startswith("alloc 0273B0D4") for l in SPEC):
            fails.append("aaf_tables no longer has the expected alloc/nestedret lines")
        on = [l for l in SPEC if not l.startswith("nestedret 0273B0D4")] + ["allocnested 0273B0D4"]
        rc, out = run("on", on, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with allocnested: %s" % (row or out[-300:]))
        rc, out = run("trace", on, extra=["-seed", "1", "-trace", "1000005"], n="20")
        got = [int(m, 16) for m in re.findall(r"nested alloc 0273B0D4 -> ([0-9A-F]{8})", out)]
        if len(got) < 2:
            fails.append("traced input shows %d nested allocations (need >= 2)" % len(got))
        else:  # the trace shows the original's run, then the candidate's: each starts at the base
            cut = next((i for i in range(1, len(got)) if got[i] <= got[i - 1]), len(got))
            first, second = got[:cut], got[cut:]
        if len(got) >= 2 and (first[0] != 0x60000000 or any(b - a < 0x40 or b % 0x40 for a, b in zip(first, first[1:]))
                              or (second and second != first)):
            fails.append("nested blocks not distinct 64-aligned from 0x60000000 in order: %s" % ["%08X" % x for x in got[:6]])
        base = [l for l in SPEC if not l.startswith("nestedret 0273B0D4")]
        for bad in ("allocnested 0273B0D5", "allocnested 0273B0D4 extra", "allocnested zz"):
            rc, _ = run("bad", base + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
        rc, _ = run("before", [l for l in SPEC if not l.startswith("alloc 0273B0D4")] + ["allocnested 0273B0D4"], n="1")
        if rc != 2:
            fails.append("allocnested without an alloc line not rejected (rc %d)" % rc)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_allocnested_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("allocnested_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
