#!/usr/bin/env python3
"""Self-test of the setup-snapshot cache (harness -snapcache-write / -snapcache / -snapcache-check, mutate.py's default;
2026-10-30). The writing run stores each function's accepted `setupsnapshot` snapshot and the post-setup state of
its validation inputs; a reading run loads them instead of running the setups. Rows must be byte-identical to a live run.

Runs the built harness of a unit directly with spec files written next to the cache (no temporary unit files):
 1. lib2_sead_15 + a constant setup (`setup 02759564 02759928 r3=0x5E000000` + `setupsnapshot`), -n 2000 (validation
    inputs 0, 500, 1000, 1500), seeds 1 and 7, plain and -nulltrap: live, writing and reading runs print the same rows and
    the same stderr apart from `snapcache:` lines; the reading run takes the snapshot and all four validation states
    from the cache. Seed 7 reads seed 1's cache: the snapshot is seed-free, the validation states are not (they run
    live). A -runlist subset (inputs 3,4,500): input 3 (first of the run, no record) runs its setups, 500 takes the record.
 2. -snapcache-check on the reading run passes (rc 0, same rows).
 3. Corruption (in copies of the cache): a flipped byte in the snapshot (world blob) is caught by its hash (reading run: setups
    live, same rows; -snapcache-check: exit 98). A byte changed in the decoded payload with the hash recomputed (what the
    hash cannot catch) is caught by -snapcache-check (exit 98 `CHECK FAILED`) for the snapshot; for a validation state by
    the hash of its reconstructed plain state (reading run: that input runs its setups live; -snapcache-check exits 98).
 4. Refusals (setups run live, same rows as live): a snapshot setupsnapshot refuses (per-input `field` under a setup
    operand) writes no file; another -snapcache-key or another start-up state (an extra `fill`) does not match;
    -trace, -watch, -nofastreset and -dsiout switch the cache off.
 5. (not with --quick) the agl world spec lib2_agl_12_ctor_027AB058_kind0, -n 200, seed 1, plain and -nulltrap: identical
    rows, -snapcache-check passes; prints the wall time of the live and the cached run.
Cache directories and spec files stay in a new directory under the verify scratch (path printed; never removed).
Exit 0 when all cases pass.
"""
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402
import specfile  # noqa: E402

M64 = (1 << 64) - 1


def hash64(data, h):  # harness.cpp snapc::hash64
    n = len(data)
    h = (h ^ (n * 0x9E3779B97F4A7C15)) & M64
    k8 = n - n % 8
    for (w,) in struct.iter_unpack("<Q", data[:k8]):
        h = ((h ^ w) * 0xFF51AFD7ED558CCD) & M64
        h ^= h >> 29
    for b in data[k8:]:
        h = ((h ^ b) * 0x100000001B3) & M64
    h ^= h >> 31
    h = (h * 0xC4CEB9FE1A85EC53) & M64
    h ^= h >> 33
    return h


def zdec_literals(enc):
    """(decoded bytes, [(encoded offset, decoded offset) of each literal byte])"""
    out, lits, i = bytearray(), [], 0
    while i < len(enc):
        t = enc[i]
        i += 1
        if t >= 0x80:
            out += bytes(t - 0x7F)
            continue
        for k in range(t + 1):
            lits.append((i + k, len(out) + k))
        out += enc[i:i + t + 1]
        i += t + 1
    return bytes(out), lits


def corrupt_blob(buf, pos, which=0.5):
    """change one literal byte of the blob at pos (u64 hash, u64 length, zero-run encoded payload) and recompute the hash"""
    hh, n = struct.unpack_from("<QQ", buf, pos)
    enc = bytes(buf[pos + 16:pos + 16 + n])
    dec, lits = zdec_literals(enc)
    if hash64(dec, 0x5A) != hh:
        raise RuntimeError("hash64 mismatch: the test's hash differs from the harness's")
    eo, do = lits[int(len(lits) * which)]
    nb = buf[pos + 16 + eo] ^ 0x01
    buf[pos + 16 + eo] = nb if nb else 0x02  # a literal stays nonzero
    dec2 = dec[:do] + bytes([buf[pos + 16 + eo]]) + dec[do + 1:]
    struct.pack_into("<Q", buf, pos, hash64(dec2, 0x5A))


def header_end(buf):
    (hl,) = struct.unpack_from("<Q", buf, 0)
    return 8 + hl


class Unit:
    def __init__(self, unit):
        self.unit = unit
        self.exe = verify.build(unit, jobs=4)
        self.spec = specfile.spec_path(unit, ROOT, verify.unit_dir(unit))
        self.hdr = verify.header_args(unit)

    def run(self, spec, args):
        t = time.time()
        r = subprocess.run([self.exe, "-spec", spec] + self.hdr + ["-noprogress"] + args, cwd=ROOT, capture_output=True,
                           text=True, errors="replace")
        return r.returncode, r.stdout, r.stderr, time.time() - t


def plain(err):
    return "\n".join(l for l in err.splitlines() if not l.startswith("snapcache:"))


def main():
    quick = "--quick" in sys.argv[1:]
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="snapcache_test-", dir=base)
    print("snapcache_test: files in %s (not removed)" % W)
    fails = []

    def spec_file(name, lines):
        p = os.path.join(W, name + ".txt")
        with open(p, "w") as f:
            f.write("\n".join(lines) + "\n")
        return p

    def same(tag, live, other, want_rc=None):
        if live[0] != other[0] or live[1] != other[1] or plain(live[2]) != plain(other[2]):
            fails.append("%s: differs from the live run (rc %d/%d)\n%s\n---\n%s\n%s" % (
                tag, live[0], other[0], live[1][-400:], other[1][-400:], other[2][-400:]))
        if want_rc is not None and other[0] != want_rc:
            fails.append("%s: rc %d, want %d" % (tag, other[0], want_rc))

    sead = Unit("lib2_sead_15")
    func = "02759564"
    lines = open(sead.spec).read().splitlines()
    if "real 02759928" not in lines:
        fails.append("lib2_sead_15 no longer lists real 02759928")
    sp = spec_file("sead_snap", lines + ["setup %s 02759928 r3=0x5E000000" % func, "setupsnapshot"])
    cache = os.path.join(W, "cache")
    os.mkdir(cache)
    K = ["-snapcache-key", "testkey"]
    for nt in ([], ["-nulltrap"]):
        c = cache if not nt else cache + "-nt"
        if nt:
            os.mkdir(c)
        for seed in ("1", "7"):
            a = ["-only", func, "-n", "2000", "-seed", seed] + nt
            tag = "sead seed %s%s" % (seed, " -nulltrap" if nt else "")
            live = sead.run(sp, a)
            if live[0] != 0 or "setupsnapshot: refused" in live[2]:
                fails.append("%s: live run rc %d / refused: %s" % (tag, live[0], live[2][-300:]))
            if seed == "1":
                w = sead.run(sp, a + ["-snapcache-write", c] + K)
                same(tag + " write", live, w)
                if "snapcache: wrote 1 snapshot(s) and 4 validation state(s)" not in w[2]:
                    fails.append("%s write: %s" % (tag, w[2][-300:]))
            r = sead.run(sp, a + ["-snapcache", c] + K)
            same(tag + " read", live, r)
            want = "1 function(s) took the cached snapshot, 0 captured it live; %s" % (
                "4 validation input(s) took the recorded setups, 0 ran them live" if seed == "1" else
                "0 validation input(s) took the recorded setups, 4 ran them live")
            if want not in r[2]:
                fails.append("%s read: want `%s`: %s" % (tag, want, r[2][-300:]))
            ck = sead.run(sp, a + ["-snapcache", c] + K + ["-snapcache-check"])
            same(tag + " check", live, ck, 0)
        # -runlist subset: input 3 is the first (validation, no record), 500 a validation input with a record
        rl = os.path.join(W, "runlist.txt")
        with open(rl, "w") as f:
            f.write("%s 3,4,500\n" % func)
        a = ["-only", func, "-n", "2000", "-seed", "1", "-runlist", rl] + nt
        live = sead.run(sp, a)
        r = sead.run(sp, a + ["-snapcache", c] + K)
        same("sead runlist%s" % (" -nulltrap" if nt else ""), live, r)
        if "1 validation input(s) took the recorded setups, 1 ran them live" not in r[2]:
            fails.append("sead runlist: %s" % r[2][-300:])

    # 3. corruption
    a = ["-only", func, "-n", "2000", "-seed", "1"]
    live = sead.run(sp, a)
    import glob
    snapf = os.path.basename(glob.glob(os.path.join(cache, func + "-*.snap"))[0])  # FUNC-TAG.snap: header + world blob hash
    valf = snapf[:-5] + "-s1.val"
    blobf = os.path.basename(glob.glob(os.path.join(cache, "world-*.blob"))[0])  # the content-addressed snapshot

    def copy_cache(name):
        d = os.path.join(W, name)
        os.mkdir(d)
        for f in os.listdir(cache):
            shutil.copy(os.path.join(cache, f), d)
        return d

    d = copy_cache("corrupt-flip")
    p = os.path.join(d, blobf)
    buf = bytearray(open(p, "rb").read())
    buf[16 + (len(buf) - 16) // 2] ^= 0x40  # inside the encoded payload
    open(p, "wb").write(buf)
    r = sead.run(sp, a + ["-snapcache", d] + K)
    same("flipped snapshot byte, read", live, r)
    if "snapcache: the snapshot of %s is unreadable; its setups run live" % func not in r[2]:
        fails.append("flipped snapshot byte not detected: %s" % r[2][-300:])
    r = sead.run(sp, a + ["-snapcache", d] + K + ["-snapcache-check"])
    if r[0] != 98:
        fails.append("flipped snapshot byte: -snapcache-check rc %d, want 98: %s" % (r[0], r[2][-300:]))

    d = copy_cache("corrupt-snap")  # a changed blob under its new hash, and the pointer naming it
    buf = bytearray(open(os.path.join(d, blobf), "rb").read())
    corrupt_blob(buf, 0)
    (nh,) = struct.unpack_from("<Q", buf, 0)
    open(os.path.join(d, "world-%016x.blob" % nh), "wb").write(buf)
    ptr = bytearray(open(os.path.join(d, snapf), "rb").read())
    struct.pack_into("<Q", ptr, len(ptr) - 8, nh)
    open(os.path.join(d, snapf), "wb").write(ptr)
    r = sead.run(sp, a + ["-snapcache", d] + K + ["-snapcache-check"])
    if r[0] != 98 or "snapcache: CHECK FAILED %s: the cached snapshot differs from the live capture" % func not in r[2]:
        fails.append("hash-valid snapshot corruption: -snapcache-check rc %d: %s" % (r[0], r[2][-300:]))

    d = copy_cache("corrupt-val")
    p = os.path.join(d, valf)
    buf = bytearray(open(p, "rb").read())
    pos = header_end(buf) + 8 + 4  # record count, first record's input index
    corrupt_blob(buf, pos, 0.3)
    open(p, "wb").write(buf)
    # (the record also carries the hash of its reconstructed plain state: the reading run sees the change and runs that
    # input's setups live; -snapcache-check exits 98)
    r = sead.run(sp, a + ["-snapcache", d] + K)
    same("hash-valid validation-state corruption, read", live, r)
    if "snapcache: the validation state of %s input 0 is unreadable; its setups run live" % func not in r[2]:
        fails.append("hash-valid validation-state corruption not detected by the reading run: %s" % r[2][-300:])
    r = sead.run(sp, a + ["-snapcache", d] + K + ["-snapcache-check"])
    if r[0] != 98:
        fails.append("hash-valid validation-state corruption: -snapcache-check rc %d: %s" % (r[0], r[2][-300:]))

    # 4. refusals
    choices = ",".join("0x5E0000%02X" % (16 * k) for k in range(8))
    dep = spec_file("sead_dep", lines + ["field %s 5E100000 u32 %s" % (func, choices), "setup %s 02759928 r3=*5E100000" % func,
                                         "setupsnapshot"])
    dc = os.path.join(W, "cache-refused")
    os.mkdir(dc)
    a = ["-only", func, "-n", "1000", "-seed", "1"]
    live = sead.run(dep, a)
    w = sead.run(dep, a + ["-snapcache-write", dc] + K)
    same("refused snapshot, write", live, w)
    if os.listdir(dc) or "setupsnapshot: refused" not in w[2]:
        fails.append("refused snapshot: files %s / %s" % (os.listdir(dc), w[2][-300:]))
    r = sead.run(dep, a + ["-snapcache", dc] + K)
    same("refused snapshot, read", live, r)
    if "snapcache: no snapshot for %s; its setups run live" % func not in r[2]:
        fails.append("refused snapshot, read: %s" % r[2][-300:])

    a = ["-only", func, "-n", "2000", "-seed", "1"]
    live = sead.run(sp, a)
    r = sead.run(sp, a + ["-snapcache", cache, "-snapcache-key", "otherkey"])
    same("other key", live, r)
    if "snapcache: no snapshot for %s; its setups run live" % func not in r[2]:  # (file names carry the configuration)
        fails.append("other key not refused: %s" % r[2][-300:])
    sp2 = spec_file("sead_fill", lines + ["setup %s 02759928 r3=0x5E000000" % func, "setupsnapshot", "fill * 5F700000 0x40 0xA5"])
    live2 = sead.run(sp2, a)
    r = sead.run(sp2, a + ["-snapcache", cache] + K)
    same("other start-up state", live2, r)
    if "snapcache: no snapshot for %s; its setups run live" % func not in r[2]:
        fails.append("other start-up state not refused: %s" % r[2][-300:])
    seed = str(1 * 1000003 + 5)
    for opt, extra in (("-trace", ["-trace", seed]), ("-watch", ["-watch", "5E000000:4"]), ("-nofastreset", ["-nofastreset"]),
                       ("-dsiout", ["-nulltrap", "-dsiout", os.path.join(W, "dsi.txt")])):
        live = sead.run(sp, a + extra)
        r = sead.run(sp, a + extra + ["-snapcache", cache] + K)
        same("refusal " + opt, live, r)
        if "snapcache: off under %s (the setups run live)" % opt not in r[2]:
            fails.append("%s does not switch the cache off: %s" % (opt, r[2][-300:]))

    # 5. the agl world
    if not quick:
        world = Unit("lib2_agl_12_ctor_027AB058_kind0")
        for nt in ([], ["-nulltrap"]):
            c = os.path.join(W, "world" + ("-nt" if nt else ""))
            os.mkdir(c)
            a = ["-n", "200", "-seed", "1"] + nt
            live = world.run(world.spec, a)
            w = world.run(world.spec, a + ["-snapcache-write", c] + K)
            r = world.run(world.spec, a + ["-snapcache", c] + K)
            ck = world.run(world.spec, a + ["-snapcache", c] + K + ["-snapcache-check"])
            tag = "agl world" + (" -nulltrap" if nt else "")
            same(tag + " write", live, w, 0)
            same(tag + " read", live, r, 0)
            same(tag + " check", live, ck, 0)
            print("snapcache_test: %s n200: live %.1f s, cached %.1f s" % (tag, live[3], r[3]))

    if fails:
        print("snapcache_test: FAIL\n" + "\n".join(fails))
        return 1
    print("snapcache_test: ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
