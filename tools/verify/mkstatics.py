#!/usr/bin/env python3
"""Static-object boundaries for the harness's -observe-pointees (2026-10-28): every address the RPX's relocations point
at inside .rodata, .data and .bss (symbol value + addend), plus each section's start and end, sorted, one hex address per
line. The RPX has no data symbols, so a static pointer's object is taken as [the greatest boundary <= the pointer, the
next boundary): the span from the referenced start to the next separately referenced address.

usage: mkstatics.py game/code/cking.rpx build/verify/statics.tsv
"""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
from rpx import Rpx  # noqa: E402

r = Rpx(sys.argv[1])
secs = [(s.addr, s.addr + s.size, s.name) for s in r.sections if s.name in (".rodata", ".data", ".bss") and s.size]
marks = set()
for lo, hi, _ in secs:
    marks.add(lo)
    marks.add(hi)
for _target, _off, _typ, sym, addend in r.relocs:
    a = (sym.value + addend) & 0xFFFFFFFF
    if any(lo <= a < hi for lo, hi, _ in secs):
        marks.add(a)
tmp = sys.argv[2] + ".tmp%d" % os.getpid()
os.makedirs(os.path.dirname(os.path.abspath(sys.argv[2])), exist_ok=True)
with open(tmp, "w") as f:
    for a in sorted(marks):
        f.write("%08X\n" % a)
os.replace(tmp, sys.argv[2])
print("%d static boundaries in %s" % (len(marks), ", ".join("%s %08X-%08X" % (n, lo, hi) for lo, hi, n in secs)))
