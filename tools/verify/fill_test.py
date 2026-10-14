#!/usr/bin/env python3
"""Self-test of the unit-file `fill FUNC|* ADDR|rN+OFF SIZE BYTE` directive (harness.cpp).

1. A `fill` of a region gives the same results (pass counts and covered blocks) as the equivalent per-word
   `field ... u64 -1` lines, for a FUNC and a `*` fill, and different ones from no fill (the region matters).
2. `field` lines over filled bytes win (fill is the background, applied before the field steering): the table's
   terminator fields inside a 4 bytes larger fill give the same rows.
3. Malformed fill lines are rejected (exit 2).

Uses the merged unit hd_inf_save_holders: calculateChecksum 027201E4 scans a 512 KB table at a pinned address up to a\ncap (the cap block is reached only with the whole table filled).
Writes temporary unit files under tools/verify/units/_fill_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_fill_test")
FUNC = "027201E4"
SRCS = [l for l in open(os.path.join(UNITS, "hd_inf_save_holders.txt")).read().splitlines() if l.startswith("src ")]
# the scan-cap fixture of hd_inf_save_holders/cap_boundary: a 512 KB all-0xFF name table at 5B000000, then a terminator
HEAD = SRCS + ["arg! %s r3 0x5A000000" % FUNC] + [
    "field %s r3+0x%x u32 0x%X" % (FUNC, off, v) for off, v in
    ((0xa0, 0x5B000000), (0xa4, 0x5C000000), (0xe0, 0x5B000000), (0xe4, 0x5C000000), (0x120, 0x5B000000), (0x124, 0x5C000000))] + [
    "field %s 5C000014 u32 0x028ED6F8" % FUNC, "stable 5A0000A0 0xC0", "stable 5C000000 0x20", "stable 5B000000 0x80004"]
TAIL = ["field %s 5B080000 u16 0,1" % FUNC, "field %s 5B080002 u16 0" % FUNC]


def run(name, lines, *args):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(HEAD + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_fill_test/" + name, "-only", FUNC] + list(args),
                       cwd=ROOT, capture_output=True, text=True)
    # the "unit ..." header names the unit file; everything else must match
    return r.returncode, "\n".join(l for l in r.stdout.splitlines() if not l.startswith("unit "))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        words = ["field %s %08X u64 -1" % (FUNC, 0x5B000000 + 8 * i) for i in range(0x10000)]
        n = ["-n", "1000"]
        a = run("words", words + TAIL, *n)
        b = run("fill", ["fill %s 5B000000 0x80000 0xFF" % FUNC] + TAIL, *n)
        c = run("star", ["fill * 5B000000 0x80000 0xFF"] + TAIL, *n)
        none = run("none", TAIL, *n)
        if a[0] or a != b or b != c:
            fails.append("fill and the equivalent field lines differ:\n%s\n%s\n%s" % (a[1], b[1], c[1]))
        if none == b:
            fails.append("the filled region does not change the results (test lost its point):\n%s" % none[1])
        # fill is the background: the terminator fields inside a larger fill still win
        d = run("over", ["fill %s 5B000000 0x80004 0xFF" % FUNC] + TAIL, *n)
        f = run("over_nofield", ["fill %s 5B000000 0x80004 0xFF" % FUNC], *n)
        if d != b:
            fails.append("field over a fill did not win:\n%s\n%s" % (d[1], b[1]))
        if f == b:
            print("note: the terminator fields do not change the rows; the field-wins case is not discriminating")
        for bad in ("fill %s 5B000000 0 0xFF" % FUNC, "fill %s 5B000000 0x10 0x100" % FUNC,
                    "fill %s 5B000000 0x10" % FUNC, "fill %s r40+4 0x10 0" % FUNC):
            rc, _ = run("bad", [bad], "-n", "1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_fill_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("fill_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
