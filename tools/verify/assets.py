#!/usr/bin/env python3
"""assets: model data of the game, extracted from your own copy at build time (never committed).

The decompiled sources keep code-like tables (animation, parameter, mode, collision tables...) as
literals. Model data that the game embeds in its code image (vertex positions, texture coordinates,
vertex/index lists) is not written into the sources: like zeldaret/tww (`extract:` entries of
config/GZLE01/config.yml, generated into build/GZLE01/include/assets/), a source includes a generated
header

    #include "assets/l_pos__d_a_sail.h"

and this tool writes that header from the RPX image (build/verify/image.bin, made by mkimage.py from
game/code/cking.rpx) into build/verify/include/assets/. The manifest tools/verify/assets.txt lists
each header with the declaration it holds and the image address of the data:

    l_pos__d_a_sail.h   101CF71C   static f32 l_pos[252]

autoglue.py expands the include of a glued source in place (on one line, so line numbers stay) and
then treats it like any other static table: its `data` glue line turns it into a view of the guest
object, and its initialiser is compared with the image.

usage: assets.py [HEADER...]     (no argument: every header of the manifest)
"""
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
MANIFEST = os.path.join(HERE, "assets.txt")
OUT = os.path.join(ROOT, "build", "verify", "include", "assets")
IMAGE = os.path.join(ROOT, "build", "verify", "image.bin")
RPX = os.path.join(ROOT, "game", "code", "cking.rpx")
INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]+"assets/([\w.]+)"[ \t]*(?://.*)?$', re.M)

# element types: struct format (big-endian), C formatting of one leaf
SCALAR = {"f32": "f", "float": "f", "u8": "B", "s8": "b", "u16": "H", "s16": "h", "u32": "I", "s32": "i"}
STRUCT = {"cXyz": "fff", "Vec": "fff", "cXy": "ff", "GXColor": "BBBB"}
DECL = re.compile(r"^(static\s+)?(const\s+)?(\w+)\s+(\w+)((?:\[\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\])+)$")


class AssetError(Exception):
    pass


def manifest():
    """{header: (address, declaration)}"""
    out = {}
    for n, raw in enumerate(open(MANIFEST), 1):
        ln = raw.split("#")[0].strip()
        if not ln:
            continue
        w = ln.split(None, 2)
        if len(w) != 3 or not re.fullmatch(r"[0-9A-Fa-f]{8}", w[1]):
            raise AssetError("assets.txt:%d: expected `HEADER ADDRESS DECLARATION`" % n)
        if w[0] in out:
            raise AssetError("assets.txt:%d: %s listed twice" % (n, w[0]))
        out[w[0]] = (int(w[1], 16), " ".join(w[2].split()))
    return out


def ensure_image():
    if not os.path.exists(IMAGE):
        if not os.path.exists(RPX):
            raise AssetError("no %s and no game/code/cking.rpx: extract your own copy of the game first"
                             % os.path.relpath(IMAGE, ROOT))
        os.makedirs(os.path.dirname(IMAGE), exist_ok=True)
        subprocess.run([sys.executable, os.path.join(HERE, "mkimage.py"), RPX, IMAGE], check=True,
                       stdout=subprocess.DEVNULL)


_image = None


def img_read(a, n):
    global _image
    if _image is None:
        ensure_image()
        d, regs, i = open(IMAGE, "rb").read(), [], 0
        while i < len(d):
            b, size = struct.unpack_from("<II", d, i)
            i += 8
            regs.append((b, d[i:i + size]))
            i += size
        _image = regs
    for b, data in _image:
        if b <= a and a + n <= b + len(data):
            return data[a - b:a - b + n]
    raise AssetError("address %08X (+%d) is outside the image" % (a, n))


def f32_text(v):
    """the shortest decimal that reads back as the same float (always with a point, `f` suffix)"""
    bits = struct.pack(">f", v)
    for p in range(1, 10):
        s = "%.*g" % (p, v)
        if struct.pack(">f", float(s)) == bits:
            break
    return repr(float(s)) + "f"  # Python's shortest repr of that short value: 1000.0, 0.8333333


def leaf_text(fmt, v):
    return f32_text(v) if fmt == "f" else "%d" % v


def render(header, addr, decl):
    m = DECL.match(decl)
    if not m:
        raise AssetError("%s: declaration not understood: %s" % (header, decl))
    elem = m.group(3)
    dims = [int(d, 0) for d in re.findall(r"\[\s*([^\]]+?)\s*\]", m.group(5))]
    if elem in SCALAR:
        fmt, nested = SCALAR[elem], False
    elif elem in STRUCT:
        fmt, nested = STRUCT[elem], True
    else:
        raise AssetError("%s: element type %s not supported" % (header, elem))
    esize = struct.calcsize(">" + fmt)
    count = 1
    for d in dims:
        count *= d
    data = img_read(addr, esize * count)
    elems = []
    for i in range(count):
        vals = struct.unpack_from(">" + fmt, data, i * esize)
        txt = ", ".join(leaf_text(f, v) for f, v in zip(fmt, vals))
        elems.append("{%s}" % txt if nested else txt)

    def group(items, ds):
        """nest by the inner dimensions (the last one innermost)"""
        if len(ds) == 1:
            return items
        step = len(items) // ds[0]
        return ["{%s}" % ", ".join(group(items[i:i + step], ds[1:])) for i in range(0, len(items), step)]

    rows = group(elems, dims)
    per = max(1, 100 // (max(len(r) for r in rows) + 2))
    body = ",\n".join("    " + ", ".join(rows[i:i + per]) for i in range(0, len(rows), per))
    return ("/* Generated by tools/verify/assets.py from your copy of the game (image %08X, %d bytes).\n"
            "   Do not edit, do not commit: game data stays in build/. */\n"
            "%s = {\n%s,\n};\n") % (addr, esize * count, decl, body)


def stale(path):
    if not os.path.exists(path):
        return True
    t = os.path.getmtime(path)
    deps = [MANIFEST, os.path.abspath(__file__)] + ([IMAGE] if os.path.exists(IMAGE) else [])
    return any(os.path.getmtime(d) > t for d in deps)


def generate(header, man=None):
    """write build/verify/include/assets/HEADER when missing or stale; returns its path"""
    man = man if man is not None else manifest()
    if header not in man:
        raise AssetError("assets/%s is not in tools/verify/assets.txt" % header)
    path = os.path.join(OUT, header)
    if stale(path):
        os.makedirs(OUT, exist_ok=True)
        text = render(header, *man[header])
        tmp = "%s.%d.tmp" % (path, os.getpid())
        open(tmp, "w").write(text)
        os.replace(tmp, path)  # atomic: parallel builds may generate the same header
    return path


def expand_includes(text):
    """the source with each `#include "assets/X.h"` line replaced by the generated header, joined on
    one line (line numbers of the rest of the source unchanged)"""
    if "assets/" not in text:
        return text
    man = None

    def one(m):
        nonlocal man
        man = man if man is not None else manifest()
        body = open(generate(m.group(1), man)).read()
        body = re.sub(r"/\*.*?\*/", " ", body, flags=re.S)
        return " ".join(body.split())
    return INCLUDE.sub(one, text)


def main():
    try:
        man = manifest()
        for h in sys.argv[1:] or sorted(man):
            print(os.path.relpath(generate(h, man), ROOT))
    except AssetError as e:
        sys.exit("assets: %s" % e)


if __name__ == "__main__":
    main()
