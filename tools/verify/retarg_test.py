#!/usr/bin/env python3
"""Self-test of the unit-file `retarg TARGET rN` directive (harness.cpp): TARGET's generated mock returns its own
incoming rN in r3 (after its other mock effects; also for calls inside `real` callees), e.g. a read callback that
reports the full requested size.

Uses the merged spec hd_fw_parts_text_valid: createLayout (027076B8) calls the mocked 0273B050 twice per input.
1. With `retarg 0273B050 r3` the unit still passes and a traced input shows each call returning its incoming r3
   (the trace prints `retarg 0273B050 -> VALUE`; the value equals the request the function passed, the same on
   both sides).
2. Malformed lines exit 2.
Writes temporary unit files under tools/verify/units/_retarg_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_retarg_test")
SPEC = open(os.path.join(UNITS, "hd_fw_parts_text_valid.txt")).read().splitlines()
FUNC = "027076B8"


def run(name, lines, extra=(), n="300"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_retarg_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        on = SPEC + ["retarg 0273B050 r3"]
        rc, out = run("on", on, n="2000")
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("unit fails with retarg: %s" % (row or out[-300:]))
        rc, out = run("trace", on, extra=["-seed", "1", "-trace", "1000003"], n="20")
        got = re.findall(r"retarg 0273B050 -> ([0-9A-F]{8})", out)
        if len(got) < 2 or len(got) % 2:
            fails.append("traced input shows %d retarg returns (need an even number >= 2: both sides)" % len(got))
        else:
            half = len(got) // 2
            if got[:half] != got[half:]:
                fails.append("original and candidate retarg returns differ: %s" % got)
        for bad in ("retarg 0273B050", "retarg 0273B050 r11", "retarg 0273B050 r2", "retarg 0273B050 r3 x", "retarg zz r3"):
            rc, _ = run("bad", SPEC + [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_retarg_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("retarg_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
