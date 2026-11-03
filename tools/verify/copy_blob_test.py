#!/usr/bin/env python3
"""Self-test of the unit-file `copy` and `blob` directives (harness.cpp; 2026-10-21, stage 2 Q5).

  copy FUNC|* DST|rN+OFF SRC SIZE           SIZE bytes of the game image at SRC placed at DST in every generated input
  blob FUNC|* DST|rN+OFF FILE OFFSET SIZE   SIZE bytes of game/FILE at OFFSET (read at run time, never committed)

Uses the merged unit lib_mtx: PSMTXCopy 028E90D4 loads the 0x30-byte source matrix at r3 word by word, and `-trace` prints
those loads, so the bytes a directive placed are observed directly.
1. `copy` (FUNC and `*` form): the traced loads equal the image bytes at SRC; without the directive they do not.
2. `blob`: the traced loads equal the bytes of game/FILE at OFFSET.
3. Order: `fill`, then `copy`/`blob`, then `field` (a field line over copied bytes wins; copied bytes win over a fill).
4. The unit still passes with the directive (both sides see the same bytes).
5. Malformed lines exit 2: zero size, a copy source outside the image, a blob path outside game/ (absolute, ..), a missing file,
   a file shorter than OFFSET+SIZE.
Writes temporary unit files under tools/verify/units/_copy_blob_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_copy_blob_test")
FUNC = "028E90D4"                       # PSMTXCopy(src r3, dst r4)
SRC = 0x02100000                        # inside .text
BLOB = "content/Common/Layout/Boot_00.szs"
SRCS = [l for l in open(os.path.join(UNITS, "lib_mtx.txt")).read().splitlines() if l.startswith("src ")]
HEAD = SRCS + ["arg! %s r3 0x5B000000" % FUNC, "arg! %s r4 0x5C000000" % FUNC, "stable 5B000000 0x30"]
TRACE = ["-n", "1", "-seed", "1", "-trace", "1000003"]


def run(name, lines, *args):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(HEAD + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_copy_blob_test/" + name, "-only", FUNC] + list(args),
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout


def loads(name, lines):
    rc, out = run(name, lines, *TRACE)
    words = {}
    for m in re.finditer(r"ld32 (5B0000[0-9A-F]{2}) -> ([0-9A-F]{8})", out):
        words.setdefault(int(m.group(1), 16) - 0x5B000000, bytes.fromhex(m.group(2)))
    return rc, b"".join(words.get(4 * i, b"????") for i in range(12))


def image_bytes(addr, size):
    """bytes of build/verify/image.bin (mkimage.py format: [addr u32 LE, size u32 LE, data]*) at a guest address"""
    with open(os.path.join(ROOT, "build", "verify", "image.bin"), "rb") as f:
        blob = f.read()
    p = 0
    while p + 8 <= len(blob):
        a, n = struct.unpack("<II", blob[p:p + 8])
        if a <= addr and addr + size <= a + n:
            return blob[p + 8 + addr - a:p + 8 + addr - a + size]
        p += 8 + n
    raise SystemExit("image.bin: no region holds %08X+%X" % (addr, size))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        run("warm", [], "-n", "1")             # builds the unit and image.bin once
        img = image_bytes(SRC, 0x30)
        for name, line in (("copy", "copy %s 5B000000 %08X 0x30" % (FUNC, SRC)), ("copy_star", "copy * 5B000000 %08X 0x30" % SRC),
                           ("copy_reg", "copy %s r3+0 %08X 0x30" % (FUNC, SRC))):
            rc, got = loads(name, [line])
            if rc or got != img:
                fails.append("%s: loads %s, image %s (rc %d)" % (name, got.hex(), img.hex(), rc))
        rc, got = loads("none", [])
        if got == img:
            fails.append("without a directive the loads equal the image bytes (test lost its point)")
        rc, got = loads("field_wins", ["copy %s 5B000000 %08X 0x30" % (FUNC, SRC), "field %s 5B000004 u32 0x11223344" % FUNC])
        if got != img[:4] + bytes.fromhex("11223344") + img[8:]:
            fails.append("a field over copied bytes did not win: %s" % got.hex())
        rc, got = loads("over_fill", ["fill %s 5B000000 0x30 0xAB" % FUNC, "copy %s 5B000008 %08X 0x8" % (FUNC, SRC)])
        if got != b"\xAB" * 8 + img[:8] + b"\xAB" * 0x20:
            fails.append("copy did not apply over fill: %s" % got.hex())
        rc, out = run("pass", ["copy %s 5B000000 %08X 0x30" % (FUNC, SRC)], "-n", "300")
        if rc or "300/300" not in out:
            fails.append("unit does not pass with a copy line:\n%s" % out[-600:])
        game = os.path.join(ROOT, "game", BLOB)
        if not os.path.exists(game):
            fails.append("game/%s missing: the blob cases need the game/ link" % BLOB)
        else:
            data = open(game, "rb").read()[0x1000:0x1030]
            rc, got = loads("blob", ["blob %s 5B000000 %s 0x1000 0x30" % (FUNC, BLOB)])
            if rc or got != data:
                fails.append("blob: loads %s, file %s (rc %d)" % (got.hex(), data.hex(), rc))
        for bad in ("copy %s 5B000000 %08X 0" % (FUNC, SRC), "copy %s 5B000000 00001000 0x10" % FUNC,
                    "copy %s 5B000000 %08X" % (FUNC, SRC), "blob %s 5B000000 /etc/hosts 0 0x10" % FUNC,
                    "blob %s 5B000000 ../game/%s 0 0x10" % (FUNC, BLOB), "blob %s 5B000000 content/no_such_file 0 0x10" % FUNC,
                    "blob %s 5B000000 %s 0x7FFFFFFF 0x10" % (FUNC, BLOB)):
            rc, _ = run("bad", [bad], "-n", "1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_copy_blob_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f[:2000])
    print("copy_blob_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
