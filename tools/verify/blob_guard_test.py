#!/usr/bin/env python3
"""Guard: game data that unit files read through `blob` (game/FILE OFFSET SIZE, read at run time) or `blobfile` (a file inside
Yaz0/SARC/BFRES containers, FILE[:STEP...] [OFFSET SIZE]; read with the reference reader blobfile.py) must never be committed.

check(repo) fails when
  - a tracked path lies under game/ (the link to the user's game files), or
  - a tracked file contains a sample of the bytes some tracked unit file's `blob` line reads: windows of 32 bytes taken every
    4 KB of each blob (at most 64 per blob; windows that are mostly one byte value, or periodic with a period of at most 8 bytes, are skipped: they would match anything).
The self-check builds a throw-away git repo with a blob line and proves that a committed copy of the blob's bytes is caught
and that the clean repo passes. Then the real repository is checked. Run before every merge of a unit with `blob` lines. Exit 0 = no committed blob data.

usage: blob_guard_test.py [REPO]
"""
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import blobfile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
BLOB_RE = re.compile(r"^\s*blob\s+\S+\s+\S+\s+(\S+)\s+(\S+)\s+(\S+)\s*(?:#.*)?$")
BLOBFILE_RE = re.compile(r"^\s*blobfile\s+\S+\s+\S+\s+(\S+)(?:\s+(\S+)\s+(\S+))?\s*(?:#.*)?$")
WIN, STEP, MAXWIN = 32, 4096, 64


def tracked(repo):
    out = subprocess.check_output(["git", "-C", repo, "ls-files", "-z"])
    return [p for p in out.decode(errors="replace").split("\0") if p]


def periodic(w, maxp=8):
    """the window repeats with a period of at most maxp bytes (e.g. float 1.0 repeated: 0000803F...); such a pattern
    occurs in any file that stores a run of the same small value, so it says nothing about where the bytes came from"""
    return any(all(w[i] == w[i % p] for i in range(len(w))) for p in range(1, maxp + 1))


def informative(w):
    """a window worth matching: not mostly one byte value and not periodic with a short period"""
    return max(w.count(bytes([b])) for b in set(w)) < WIN * 3 // 4 and not periodic(w)


def blob_windows(repo, files):
    """(unit file, game file, windows) for every blob line of a tracked unit file whose game file is readable"""
    res, missing = [], []
    for p in files:
        if not (p.startswith("tools/verify/units/") and p.endswith(".txt")):
            continue
        for line in open(os.path.join(repo, p), errors="replace"):
            m = BLOB_RE.match(line)
            mf = None if m else BLOBFILE_RE.match(line)
            if not m and not mf:
                continue
            try:
                if m:
                    path, off, size = m.group(1), int(m.group(2), 0), int(m.group(3), 0)
                    with open(os.path.join(repo, "game", path), "rb") as f:
                        f.seek(off)
                        data = f.read(size)
                else:
                    path = mf.group(1)
                    data = blobfile.read_chain(os.path.join(repo, "game"), path)
                    if mf.group(2):
                        off = int(mf.group(2), 0)
                        data = data[off:off + int(mf.group(3), 0)]
            except (OSError, ValueError):
                missing.append((p, path))
                continue
            wins = []
            for o in range(0, max(1, len(data) - WIN + 1), STEP):
                w = data[o:o + WIN]
                if len(w) == WIN and informative(w):
                    wins.append(w)
                if len(wins) >= MAXWIN:
                    break
            res.append((p, path, wins))
    return res, missing


def check(repo):
    files = tracked(repo)
    problems = ["tracked file under game/: %s" % p for p in files if p == "game" or p.startswith("game/")]
    blobs, missing = blob_windows(repo, files)
    if blobs:
        for p in files:
            fp = os.path.join(repo, p)
            if not os.path.isfile(fp) or os.path.islink(fp):
                continue
            data = open(fp, "rb").read()
            for unit, path, wins in blobs:
                if any(w in data for w in wins):
                    problems.append("%s contains bytes of game/%s (blob line in %s)" % (p, path, unit))
    return problems, blobs, missing


def tiny_sarc(name, data):
    """a big-endian SARC with one named entry (format as blobfile.sarc reads it)"""
    import struct
    nm = name.encode() + b"\0"
    nm += b"\0" * (-len(nm) % 4)
    sfat = b"SFAT" + struct.pack(">HHI", 0xC, 1, 0x65) + struct.pack(">IIII", 0, 0x01000000, 0, len(data))
    sfnt = b"SFNT" + struct.pack(">HH", 8, 0) + nm
    head_len = 0x14 + len(sfat) + len(sfnt)
    dataoff = (head_len + 0x1F) & ~0x1F
    body = sfat + sfnt + b"\0" * (dataoff - head_len) + data
    return b"SARC" + struct.pack(">HHIII", 0x14, 0xFEFF, 0x14 + len(body), dataoff, 0x01000000) + body


def selfcheck():
    fails = []
    with tempfile.TemporaryDirectory() as d:
        def git(*a):
            subprocess.run(["git", "-C", d] + list(a), check=True, capture_output=True)
        git("init", "-q")
        git("config", "user.email", "t@t"), git("config", "user.name", "t")
        os.makedirs(os.path.join(d, "game", "content"))
        os.makedirs(os.path.join(d, "tools", "verify", "units"))
        secret = os.urandom(0x3000)
        open(os.path.join(d, "game", "content", "data.bin"), "wb").write(secret)
        open(os.path.join(d, ".gitignore"), "w").write("/game/\n")
        open(os.path.join(d, "tools", "verify", "units", "u.txt"), "w").write("blob 02000000 5B000000 content/data.bin 0x100 0x2000\n")
        open(os.path.join(d, "src.cpp"), "w").write("int x;\n")
        git("add", "-A"), git("commit", "-qm", "clean")
        p, b, _ = check(d)
        if p or not b or not b[0][2]:
            fails.append("clean repo flagged or blob not read: %s" % p)
        open(os.path.join(d, "table.bin"), "wb").write(b"hdr" + secret[0x100 + 0x1000:0x100 + 0x1400])
        git("add", "-A"), git("commit", "-qm", "leak")
        p, _, _ = check(d)
        if not any("table.bin" in x for x in p):
            fails.append("a committed copy of blob bytes was not caught: %s" % p)
        git("add", "-f", "game/content/data.bin"), git("commit", "-qm", "game file")
        git("rm", "-q", "--cached", "game/content/data.bin"), git("commit", "-qm", "untrack")
        inner = os.urandom(0x2000)
        open(os.path.join(d, "game", "content", "pack.sarc"), "wb").write(tiny_sarc("x/inner.bin", inner))
        open(os.path.join(d, "tools", "verify", "units", "v.txt"), "w").write("blobfile * 5C000000 content/pack.sarc:x/inner.bin\n")
        open(os.path.join(d, "table.bin"), "wb").write(b"clean")
        git("add", "-A"), git("commit", "-qm", "blobfile unit")
        p, b, _ = check(d)
        if p or len(b) != 2 or not b[1][2]:
            fails.append("clean blobfile repo flagged or entry not read: %s" % p)
        open(os.path.join(d, "copy.bin"), "wb").write(inner[0x1000:0x1100])
        git("add", "-A"), git("commit", "-qm", "leak entry")
        p, _, _ = check(d)
        if not any("copy.bin" in x for x in p):
            fails.append("a committed copy of blobfile entry bytes was not caught: %s" % p)
        git("add", "-f", "game/content/data.bin"), git("commit", "-qm", "game file")
        p, _, _ = check(d)
        if not any("under game/" in x for x in p):
            fails.append("a tracked file under game/ was not caught: %s" % p)
    # periodic windows (2026-11-03): float 1.0 repeated, offset by one byte, as in a generated glb, is skipped; a real
    # window of blob bytes is not
    ones = (b"\x00\x00\x80\x3f" * 9)[1:1 + WIN]
    if informative(ones):
        fails.append("a periodic window (float 1.0 repeated) was not skipped")
    if not informative(os.urandom(WIN)):
        fails.append("a random window was skipped")
    return fails


def main():
    repo = os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else ROOT
    fails = selfcheck()
    for f in fails:
        print("FAIL (self-check)", f)
    problems, blobs, missing = check(repo)
    for p in problems:
        print("FAIL", p)
    for unit, path in missing:
        print("note: %s reads game/%s, which is not readable here (cannot sample it)" % (unit, path))
    print("blob_guard_test: %s (%d blob line(s) sampled)" % ("FAIL" if fails or problems else "PASS", len(blobs)))
    return 1 if fails or problems else 0


if __name__ == "__main__":
    sys.exit(main())
