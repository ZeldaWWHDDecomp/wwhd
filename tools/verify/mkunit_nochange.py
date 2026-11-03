#!/usr/bin/env python3
"""No-change proof for mkunit.py changes (2026-11-02: memory of very large units).

usage: mkunit_nochange.py BASE_TREE OUT_DIR UNIT[:SPLIT] ...

For each unit, runs BASE_TREE's mkunit.py and this tree's mkunit.py (each in its own process, the unit's spec from its
own tree; SPLIT, when given, is passed as --split as verify.py does for `unitsplit N`) into OUT_DIR/base/UNIT and
OUT_DIR/new/UNIT, and prints both processes' peak RSS (wait4 rusage) and whether every file they write (unit.c,
unit_shared.h, unit.pK.c, unit.parts) is byte-identical (SHA-256 of each). Both trees need their build/gen and name
tables. OUT_DIR is the caller's (e.g. a RAM-disk directory); the output directories are left for inspection.
Exit 0 when every unit's outputs are identical.
"""
import hashlib
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))


def gen(tree, unit, out, split):
    os.makedirs(out, exist_ok=True)
    cmd = [sys.executable, os.path.join(tree, "tools", "verify", "mkunit.py"), unit, "--root", tree, "--out", out]
    if split:
        cmd += ["--split", str(split)]
    t0 = time.time()
    p = subprocess.Popen(cmd, cwd=tree, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    so = p.stdout.read()
    _, status, ru = os.wait4(p.pid, 0)
    rc = os.waitstatus_to_exitcode(status)
    rss = ru.ru_maxrss / (1 << 20) if sys.platform == "darwin" else ru.ru_maxrss / 1024.0  # MB
    return rc, rss, time.time() - t0, so.decode(errors="replace")


def digests(d):
    out = {}
    for f in sorted(os.listdir(d)):
        if f == "unit.c" or f == "unit_shared.h" or f == "unit.parts" or (f.startswith("unit.p") and f.endswith(".c")):
            out[f] = hashlib.sha256(open(os.path.join(d, f), "rb").read()).hexdigest()
    return out


def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    base, outdir = os.path.abspath(sys.argv[1]), os.path.abspath(sys.argv[2])
    bad = 0
    for arg in sys.argv[3:]:
        unit, _, split = arg.partition(":")
        res = {}
        for tag, tree in (("base", base), ("new", ROOT)):
            d = os.path.join(outdir, tag, unit)
            rc, rss, wall, so = gen(tree, unit, d, int(split) if split else 0)
            if rc:
                print("%s %s: mkunit exit %d\n%s" % (unit, tag, rc, so[-2000:]))
                bad += 1
                break
            res[tag] = (rss, wall, digests(d))
        if len(res) < 2:
            continue
        same = res["base"][2] == res["new"][2] and "unit.c" in res["new"][2]
        bad += not same
        print("%-60s %s files %d  peak RSS base %.0f MB new %.0f MB  wall %.0f/%.0f s  unit.c %s" % (
            unit, "IDENTICAL" if same else "DIFFER", len(res["new"][2]), res["base"][0], res["new"][0], res["base"][1],
            res["new"][1], res["new"][2].get("unit.c", "-")[:16]), flush=True)
        if not same:
            for f in sorted(set(res["base"][2]) | set(res["new"][2])):
                if res["base"][2].get(f) != res["new"][2].get(f):
                    print("    differs: %s" % f)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
