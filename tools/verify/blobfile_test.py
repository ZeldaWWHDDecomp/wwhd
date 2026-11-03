#!/usr/bin/env python3
"""Self-test of the unit-file `blobfile` directive (harness.cpp + src/blobfile.h; 2026-10-27, AGL#3 FilterAa request).

  blobfile FUNC|* DST|rN+OFF FILE[:STEP...] [OFFSET SIZE]

FILE is a path inside game/; each STEP is `<yaz0>` (Yaz0-decode the current bytes) or an entry name of the current SARC or an
embedded file of the current Wii U BFRES (Yaz0 data is decoded before a named step). Without OFFSET SIZE the whole result is
placed (at most 16 MB). Like `blob`, the bytes are read at run time and never committed (blob_guard_test.py samples them).
1. The bytes the harness extracts (dumped through WWHD_VERIFY_BLOBDUMP) equal the reference reader blobfile.py for a BFRES-
   embedded file inside a Yaz0 SARC (Misc.szs:Misc.bfres:wwHD_basic.baglfila, an AAMP), a Yaz0 decode and an OFFSET SIZE window.
2. Decoder identity: the game's own Yaz0 decoder 0275ED14 (sead SZS: magic check, 0275EB8C size, 0275EBC0 decode), run as an
   original setup on the stored bytes of a stage archive (`blob`), writes exactly the bytes `blobfile FILE:<yaz0>` places
   (observed with -watch over the whole destination: every byte written, all equal). The archive has literal runs, short and
   long (0x12 + n) back references and distances up to 4 KB.
3. Placement: the vehicle PSMTXCopy 028E90D4 (unit lib_mtx) loads the placed bytes (traced), and the unit passes 300/300.
4. Malformed lines exit 2: a path outside game/, a missing file, an empty step, a missing SARC entry / BFRES file, a named step
   on bytes that are no container, a window past the end, a wrong operand count.
Writes temporary unit files under tools/verify/units/_blobfile_test/ and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_blobfile_test")
sys.path.insert(0, HERE)
import blobfile  # noqa: E402

FUNC = "028E90D4"                       # PSMTXCopy(src r3, dst r4)
GAME = os.path.join(ROOT, "game")
FILA = "content/Common/Misc/Misc.szs:Misc.bfres:wwHD_basic.baglfila"
MISC = "content/Common/Misc/Misc.szs:<yaz0>"
STAGE = "content/Common/Stage/sea_T_Room0.szs"
SRCS = [l for l in open(os.path.join(UNITS, "lib_mtx.txt")).read().splitlines() if l.startswith("src ")]
HEAD = SRCS + ["arg! %s r3 0x5B000000" % FUNC, "arg! %s r4 0x5C000000" % FUNC, "stable 5B000000 0x30"]
DECODE = ["setup * 02759AD8",                                   # endian table index *104A0CD4 (sead endian init)
          "real 0275ED14 0275EB8C 0275EBC0 02759AD8 02759B38 02759A48 02759B3C 02759A60"]


def run(name, lines, *args, env=None):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(HEAD + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_blobfile_test/" + name, "-only", FUNC] + list(args),
                       cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout + r.stderr


def dumped(name, line):
    """the bytes the harness extracted for one blobfile line"""
    d = tempfile.mkdtemp(prefix="blobfile_test_", dir=os.environ.get("WWHD_VERIFY_SCRATCH") or None)
    try:
        env = dict(os.environ, WWHD_VERIFY_BLOBDUMP=d)
        rc, out = run(name, [line], "-n", "1", env=env)
        p = os.path.join(d, "blobfile_0.bin")
        return rc, (open(p, "rb").read() if os.path.exists(p) else None), out
    finally:
        shutil.rmtree(d, ignore_errors=True)


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if not os.path.exists(os.path.join(GAME, "content", "Common", "Misc", "Misc.szs")):
            raise SystemExit("blobfile_test: the game/ link must point at the user's game files")
        run("warm", [], "-n", "1")
        # 1. extraction equals the reference reader
        fila = blobfile.read_chain(GAME, FILA)
        if fila[:4] != b"AAMP" or len(fila) != 278:
            fails.append("reference: %s is not the 278-byte AAMP (%d bytes, %r)" % (FILA, len(fila), fila[:4]))
        misc = blobfile.read_chain(GAME, MISC)
        for name, line, want in (("fila", "blobfile * 5D000000 " + FILA, fila),
                                 ("misc_win", "blobfile * 5D000000 %s 0x100 0x200" % MISC, misc[0x100:0x300]),
                                 ("stage", "blobfile * 5D000000 %s:<yaz0>" % STAGE, blobfile.read_chain(GAME, STAGE + ":<yaz0>"))):
            rc, got, out = dumped(name, line)
            if rc or got != want:
                fails.append("%s: harness bytes differ from blobfile.py (rc %d, %s vs %d bytes)\n%s"
                             % (name, rc, None if got is None else len(got), len(want), out[-400:]))
        # 2. the original decoder 0275ED14 writes the same bytes
        raw = open(os.path.join(GAME, STAGE), "rb").read()
        want = blobfile.read_chain(GAME, STAGE + ":<yaz0>")
        rc, got, _ = dumped("stage2", "blobfile * 5D000000 %s:<yaz0>" % STAGE)
        lines = ["blob * 5B100000 %s 0 0x%X" % (STAGE, len(raw)), "fill * 5B200000 0x%X 0xEE" % len(want),
                 "setup * 0275ED14 r3=0x5B200000 r4=0x%X r5=0x5B100000" % len(want)] + DECODE
        rc2, out = run("decode", lines, "-n", "1", "-watch", "5B200000:0x%X" % len(want))
        wrote = {}
        for m in re.finditer(r"watch W ([0-9A-F]{8}) ([0-9A-F]{2}) side=orig", out):
            wrote[int(m.group(1), 16) - 0x5B200000] = int(m.group(2), 16)
        orig = bytes(wrote.get(k, 0xEE) for k in range(len(want)))
        if rc2 or len(wrote) != len(want) or orig != want or got != orig:
            fails.append("decode: original 0275ED14 wrote %d of %d bytes; equal to blobfile.py %s, to the harness %s (rc %d)\n%s"
                         % (len(wrote), len(want), orig == want, got == orig, rc2, out[-600:]))
        # 3. placement through the CopySpec path: traced loads and a passing unit
        rc, out = run("loads", ["blobfile %s 5B000000 %s 0 0x30" % (FUNC, FILA)], "-n", "1", "-seed", "1", "-trace", "1000003")
        words = {}
        for m in re.finditer(r"ld32 (5B0000[0-9A-F]{2}) -> ([0-9A-F]{8})", out):
            words.setdefault(int(m.group(1), 16) - 0x5B000000, bytes.fromhex(m.group(2)))
        if rc or b"".join(words.get(4 * i, b"????") for i in range(12)) != fila[:0x30]:
            fails.append("loads: the vehicle did not load the placed bytes (rc %d)" % rc)
        rc, out = run("pass", ["blobfile %s 5B000000 %s 0 0x30" % (FUNC, FILA)], "-n", "300")
        if rc or "300/300" not in out:
            fails.append("unit does not pass with a blobfile line:\n%s" % out[-600:])
        # 4. malformed lines
        for bad in ("blobfile * 5D000000 /etc/hosts", "blobfile * 5D000000 ../game/%s" % STAGE,
                    "blobfile * 5D000000 content/no_such_file", "blobfile * 5D000000 %s::<yaz0>" % STAGE,
                    "blobfile * 5D000000 content/Common/Misc/Misc.szs:Misc.bfres:no_such.bin",
                    "blobfile * 5D000000 content/Common/Misc/Misc.szs:no_such.bfres",
                    "blobfile * 5D000000 %s:Misc.bfres" % FILA, "blobfile * 5D000000 %s 0x100 0x1000" % FILA,
                    "blobfile * 5D000000 %s 0x10" % FILA, "blobfile * 5D000000"):
            rc, _ = run("bad", [bad], "-n", "1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_blobfile_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f[:2000])
    print("blobfile_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
