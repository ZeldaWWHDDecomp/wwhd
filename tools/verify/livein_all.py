#!/usr/bin/env python3
"""Live-in argument registers of every function in the image (build/verify/livein.tsv).

The harness compares, at a call whose target the unit does not list (an indirect call through a
guest pointer: call_ptr / bctrl), the registers the resolved callee's code actually reads, not only
the ones the candidate passes, so an omitted argument register is caught (game test 2026-10-04:
d_s_play's isKindOf check without r4). Dataflow without guesses for the callee's own indirect calls.

usage: livein_all.py [OUT]     (verify.py regenerates it when build/gen is newer)
Output: one line per function: ADDR<TAB>int mask (bit N = rN, r3..r10)<TAB>float mask (bit N = fN, f1..f8), hex.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, HERE)
from funcdb import GenIndex, Dataflow  # noqa: E402


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "build", "verify", "livein.tsv")
    gen = GenIndex(os.path.join(ROOT, "build", "gen"))
    df = Dataflow(gen, unknown_reads_args=False)
    rows = []
    for a in sorted(gen.addrs):
        try:
            li = df.livein(a)
        except Exception:
            continue
        im = sum(1 << r for k, r in li if k == "r" and 3 <= r <= 10)
        fm = sum(1 << r for k, r in li if k == "f" and 1 <= r <= 8)
        if im or fm:
            rows.append("%08X\t%X\t%X\n" % (a, im, fm))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    tmp = out + ".tmp%d" % os.getpid()
    with open(tmp, "w") as f:
        f.writelines(rows)
    os.replace(tmp, out)  # atomic: parallel builds never see a partial table


if __name__ == "__main__":
    main()
