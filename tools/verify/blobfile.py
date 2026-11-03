#!/usr/bin/env python3
"""Reference reader for the unit-file `blobfile` directive (harness: src/blobfile.h), used by blob_guard_test.py and
blobfile_test.py. A chain FILE[:STEP...] starts at game/FILE; a STEP is `<yaz0>` (Yaz0-decode the current bytes) or the name of
an entry of the current SARC or an embedded file of the current Wii U BFRES (Yaz0 bytes are decoded first).

usage: blobfile.py GAME_DIR CHAIN OUT   (writes the bytes of CHAIN to OUT; for inspection only, never into the repository)
"""
import os
import struct
import sys


def yaz0(d):
    if d[:4] != b"Yaz0":
        raise ValueError("not Yaz0 data")
    n = struct.unpack(">I", d[4:8])[0]
    out = bytearray(n)
    s, o = 16, 0
    while o < n:
        code = d[s]
        s += 1
        for bit in range(8):
            if o >= n:
                break
            if code & (0x80 >> bit):
                out[o] = d[s]
                s += 1
                o += 1
                continue
            b1, b2 = d[s], d[s + 1]
            s += 2
            dist, ln = ((b1 & 0xF) << 8 | b2) + 1, b1 >> 4
            if ln == 0:
                ln = d[s] + 0x12
                s += 1
            else:
                ln += 2
            if dist > o:
                raise ValueError("Yaz0 back reference before the start")
            for _ in range(ln):
                if o >= n:
                    break
                out[o] = out[o - dist]
                o += 1
    return bytes(out)


def sarc(s, name):
    if s[:4] != b"SARC":
        raise ValueError("not a SARC archive")
    e = ">" if s[6:8] == b"\xfe\xff" else "<"
    hdr = struct.unpack(e + "H", s[4:6])[0]
    data = struct.unpack(e + "I", s[0xC:0x10])[0]
    count, key = struct.unpack(e + "HI", s[hdr + 6:hdr + 12])
    nodes = hdr + 0xC
    sfnt = nodes + count * 16
    names = sfnt + struct.unpack(e + "H", s[sfnt + 4:sfnt + 6])[0]
    h = 0
    for ch in name.encode():
        h = (h * key + ch) & 0xFFFFFFFF
    for i in range(count):
        nh, attr, b, en = struct.unpack(e + "IIII", s[nodes + i * 16:nodes + i * 16 + 16])
        if attr >> 24:
            no = names + (attr & 0xFFFF) * 4
            hit = s[no:s.index(b"\0", no)] == name.encode()
        else:
            hit = nh == h
        if hit:
            return s[data + b:data + en]
    raise ValueError("no SARC entry " + name)


def fres(b, name):
    if b[:4] != b"FRES" or b[8:10] != b"\xfe\xff":
        raise ValueError("not a Wii U BFRES")
    go = struct.unpack(">i", b[0x20 + 44:0x20 + 48])[0]
    if not go:
        raise ValueError("BFRES without embedded files")
    g = 0x20 + 44 + go
    n = struct.unpack(">i", b[g + 4:g + 8])[0]
    for i in range(1, n + 1):
        ep = g + 8 + i * 16
        npo, dpo = struct.unpack(">ii", b[ep + 8:ep + 16])
        np_ = ep + 8 + npo
        if b[np_:b.index(b"\0", np_)] != name.encode():
            continue
        dp = ep + 12 + dpo
        fo, fs = struct.unpack(">iI", b[dp:dp + 8])
        return b[dp + fo:dp + fo + fs]
    raise ValueError("no BFRES embedded file " + name)


def step(cur, st):
    if st == "<yaz0>":
        return yaz0(cur)
    if cur[:4] == b"Yaz0":
        cur = yaz0(cur)
    if cur[:4] == b"SARC":
        return sarc(cur, st)
    if cur[:4] == b"FRES":
        return fres(cur, st)
    raise ValueError("step %s: the current bytes are not Yaz0, SARC or BFRES" % st)


def read_chain(game_dir, chain):
    parts = chain.split(":")
    if not parts[0] or parts[0].startswith("/") or ".." in parts[0]:
        raise ValueError("blobfile file must be a path inside game/")
    with open(os.path.join(game_dir, parts[0]), "rb") as f:
        cur = f.read()
    for st in parts[1:]:
        cur = step(cur, st)
    return cur


if __name__ == "__main__":
    if len(sys.argv) != 4:
        raise SystemExit(__doc__)
    open(sys.argv[3], "wb").write(read_chain(sys.argv[1], sys.argv[2]))
