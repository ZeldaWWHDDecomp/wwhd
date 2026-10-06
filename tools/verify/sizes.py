#!/usr/bin/env python3
"""WWHD class sizes from constructors.

GHS constructors allocate their object when called with this == NULL:
    or. r31, r3, r3 ; bne 1f ; li r3, SIZE ; bl operator_new ; ...
so every out-of-line constructor states sizeof(class). The vtable pointers it stores
(lis/addi + stw OFF(this)) are listed too: their offsets show where the HD C++ vtable pointers sit.

usage: sizes.py [REGEX]      e.g. sizes.py 'dCcD|dBgS|McaMorf'   (matches the WWHD name)
Output: address, size, name, vtable stores (offset=table).
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, HERE)
from rpx import Rpx  # noqa: E402
from funcdb import GenIndex, load_names  # noqa: E402

OPERATOR_NEW = {0x0273AD10}


def main():
    pat = re.compile(sys.argv[1]) if len(sys.argv) > 1 else None
    gen = GenIndex(os.path.join(ROOT, "build", "gen"))
    names, gc = load_names(os.path.join(ROOT, "build"))
    rpx = Rpx(os.path.join(ROOT, "game", "code", "cking.rpx"))
    text = [s for s in rpx.sections if s.name == ".text"][0]

    def w(a):
        o = a - text.addr
        return struct.unpack(">I", text.data[o:o + 4])[0]

    for a in gen.addrs:
        if not (text.addr <= a < text.addr + len(text.data)):
            continue
        nm = names.get(a, ("",))[0]
        if pat and not pat.search(nm):
            continue
        n = min(gen.size(a) // 4, 400)
        ins = [w(a + 4 * i) for i in range(n)]
        size = None
        this = None
        for i in range(min(n, 64)):
            x = ins[i]
            # or. rA, r3, r3
            if (x >> 26) == 31 and ((x >> 1) & 0x3FF) == 444 and (x & 1) and ((x >> 21) & 31) == 3 and ((x >> 11) & 31) == 3:
                this = (x >> 16) & 31
            # li r3, IMM ; bl new
            if this is not None and (x >> 16) == (14 << 10 | 3 << 5 | 0) and i + 1 < n:
                for j in (i + 1, i + 2):
                    y = ins[j] if j < n else 0
                    if (y >> 26) == 18 and (y & 1):
                        off = y & 0x03FFFFFC
                        if off & 0x02000000:
                            off -= 0x04000000
                        if ((a + 4 * j + off) & 0xFFFFFFFF) in OPERATOR_NEW:
                            size = x & 0xFFFF
                if size is not None:
                    break
        if size is None:
            continue
        # vtable stores: lis rX,HI ; addi/addic rX,rX,LO ; stw rX, OFF(this or a copy)
        hi = {}
        vt = []
        for x in ins:
            op = x >> 26
            rd, ra, imm = (x >> 21) & 31, (x >> 16) & 31, x & 0xFFFF
            simm = imm - 0x10000 if imm & 0x8000 else imm
            if op == 15 and ra == 0:
                hi[rd] = imm << 16
            elif op in (14, 12, 13) and ra in hi and rd == ra:
                hi[rd] = (hi[ra] + simm) & 0xFFFFFFFF
                hi[("v", rd)] = True
            elif op == 36 and hi.get(("v", rd)) and ra == this:
                v = hi[rd]
                if 0x10000000 <= v < 0x10800000:
                    vt.append("%X=%08X" % (simm, v))
            elif op not in (36,) and rd in hi and op not in (15, 14, 12, 13):
                pass
        print("%08X %5X  %-50s %s" % (a, size, nm or "?", " ".join(vt)))


if __name__ == "__main__":
    main()
