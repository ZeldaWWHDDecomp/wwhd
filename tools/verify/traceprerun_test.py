#!/usr/bin/env python3
"""Self-test of `-trace SEED` on a dictionary pre-run seed (2026-11-01): the 8 pre-runs that build a function's constant
dictionary use seeds SEED0*7777+i; `-trace` with one of them now traces that pre-run like a generated input, headed
`trace prerun LABEL (dictionary pre-run i)`, so a pre-run DSI shows its accesses before the DSI line.

Uses lib_js_base with JKRDvdFile::close 027EACBC and this = NULL (`arg! 027EACBC r3 0`): every run of the original,
pre-runs included, takes a page-0 DSI under -nulltrap.
1. `-seed 1 -trace 7777` (pre-run 0): the output has `trace prerun`, at least one access/call line after it and then
   `DSI (original side)`;
2. `-seed 1 -trace 7778` (pre-run 1): headed `(dictionary pre-run 1)`;
3. the result row is the same with and without `-trace 7777`.
Writes a temporary unit directory and removes it. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_traceprerun_test")
CLOSE = "027EACBC"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()


def run(extra):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_traceprerun_test/t", "-only", CLOSE, "-n", "20",
                        "-seed", "1", "-nulltrap", "-buildj", "4"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(CLOSE)]
    return r.stdout, (row[0] if row else r.stdout[-400:])


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        with open(os.path.join(TMP, "t.txt"), "w") as f:
            f.write("\n".join(BASE + ["arg! %s r3 0" % CLOSE]) + "\n")
        _, plain = run([])
        out, row = run(["-trace", "7777"])
        lines = out.splitlines()
        head = [i for i, l in enumerate(lines) if l.strip().startswith("trace prerun") and "(dictionary pre-run 0)" in l]
        if not head:
            fails.append("1: no `trace prerun ... (dictionary pre-run 0)` line")
        else:
            rest = lines[head[0] + 1:]
            dsi = [i for i, l in enumerate(rest) if l.strip().startswith("DSI (original side)")]
            if not dsi:
                fails.append("1: no `DSI (original side)` after the prerun header")
            elif not any(l.strip().split(" ")[0] in ("ld32", "st32", "call", "fetch") or "ld" in l.split()[0] for l in rest[:dsi[0]] if l.strip()):
                fails.append("1: no access/call line before the DSI")
            else:
                print("1 ok:", "\n".join(lines[head[0]:head[0] + 1 + dsi[0] + 1]))
        out2, _ = run(["-trace", "7778"])
        if "(dictionary pre-run 1)" not in out2:
            fails.append("2: pre-run 1 not traced")
        else:
            print("2 ok")
        if row != plain:
            fails.append("3: row differs with -trace: %r vs %r" % (row, plain))
        else:
            print("3 ok:", row)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
