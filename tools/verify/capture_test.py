#!/usr/bin/env python3
"""Self-test of the capture route's harness part (`capture`, `capturemap`, `capturecall`; harness.cpp namespace capm;
2026-11-02; kills only). Everything runs on a SYNTHETIC capture written here (no game data), in a
temporary store. Tested function: 0275BF18 (wwhd_src/sead/RuntimeIntegerFormat.cpp) with r3 = 0x54200000, which reads a pointer and
two s32 fields at r3.
 1. A good capture: the loads of the object's fields return the captured words (trace), original vs original passes.
 2. A .zst capture gives the same result; the SHA-256 is of the decompressed bytes.
 3. Refused with exit 2: wrong SHA-256, not a WWCAPMEM file, truncated, overlapping windows, capture without capturemap, capturemap
    without capture, windows on .rodata / the port runtime / MEM1 / the foreground bucket, a `field`/`poke`/`setup`/`fill` on top,
    capture without capturecall, capturecall with a wrong hash, before capture or with an argument that differs from the manifest (drift).
 4. A byte outside every window is not served from the capture: it is generated, and WWHD_CAPTURE_GENLOG lists it; with the window
    covering it the same address is served and the log has no such address (the closed-read-set check).
 5. WWHD_CAPTURE_READLOG lists the bytes guest code read from the windows, and only with it a diagnostic spec may map a forbidden region
    and omit capturecall.
 6. A capture run with clobbering's input half (n = 40, odd input traced) keeps the captured value. The function makes few mocked calls,
    so the guard itself (clobberable: capm::present) is for the audit to read; this case only pins that nothing else rewrites it.
Writes temporary unit files under tools/verify/units/_capture_test/ and a temporary store, and removes both. Exit 0 when all pass."""
import hashlib, os, re, shutil, struct, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_capture_test")
FUNC = "0275BF18"
OBJ = 0x54200000
WORD = 0x54210040
SRC = [l.strip() for l in open(os.path.join(HERE, "units", "lib2_sead_15.txt")) if l.startswith("src ")]


def mem_file(path, word=WORD):
    page = bytearray(0x1000)
    page[0:12] = struct.pack(">Iii", word, 2, 8)
    with open(path, "wb") as f:
        f.write(b"WWCAPMEM" + struct.pack("<II", 1, 0x1000))
        f.write(struct.pack("<I", OBJ) + bytes(page))
    return hashlib.sha256(open(path, "rb").read()).hexdigest()


def manifest(path, r3=OBJ, r4=8):
    regs = {i: 0 for i in range(32)}
    regs[3], regs[4] = r3, r4
    t = "format wwhd-guest-capture 1\nfunc %s\n" % FUNC + "".join("r%d %08X\n" % (i, regs[i]) for i in range(32)) + "end\n"
    open(path, "w").write(t)
    return hashlib.sha256(t.encode()).hexdigest()


def spec(sha, meta, windows=("54200000 C",), name="a/mem.bin", extra=(), args=(("r3", OBJ), ("r4", 8))):
    l = ["# verify: -only %s -strictnan -calleesp -nulltrap" % FUNC] + SRC + ["real 028F5A1C"]
    if sha: l.append("capture %s %s" % (name, sha))
    if meta: l.append("capturecall %s" % meta)
    l += ["capturemap " + w for w in windows]
    l += ["arg! %s %s 0x%X" % (FUNC, r, v) for r, v in args]
    return l + list(extra)


def run(store, name, lines, n="4", env=(), extra=()):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    e = dict(os.environ, WWHD_CAPTURE_STORE=store)
    e.update(dict(env))
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_capture_test/" + name, "-n", n, "-seed", "1", "--no-cache"] + list(extra),
                       cwd=ROOT, capture_output=True, text=True, env=e)
    return r.returncode, r.stdout + r.stderr


def main():
    os.makedirs(TMP, exist_ok=True)
    store = tempfile.mkdtemp(prefix="capture_test")
    fails = []
    def expect_exit2(label, lines, want, env=()):
        rc, out = run(store, "bad", lines, env=env)
        if rc != 2 or want not in out:
            fails.append("%s: rc %d, wanted exit 2 with %r: %s" % (label, rc, want, out[-300:]))
    try:
        os.makedirs(store + "/a")
        sha = mem_file(store + "/a/mem.bin")
        meta = manifest(store + "/a/capture.txt")
        good = spec(sha, meta)
        # 1
        rc, out = run(store, "good", good, n="4", extra=["-trace", "1000003"])
        row = [l for l in out.splitlines() if l.startswith(FUNC)]
        if rc or not row or not row[0].endswith("ok"):
            fails.append("good capture: %s" % (row or out[-400:]))
        if "ld32 %08X -> 00000002" % (OBJ + 4) not in out or "ld32 %08X -> 00000008" % (OBJ + 8) not in out:
            fails.append("the loads do not return the captured words: %s" % out[-400:])
        # 2
        shutil.copy(store + "/a/mem.bin", store + "/a/mem2.bin")
        if subprocess.run(["zstd", "-q", store + "/a/mem2.bin", "-o", store + "/a/mem2.bin.zst"]).returncode == 0:
            rc, out = run(store, "zst", spec(sha, meta, name="a/mem2.bin.zst"), extra=["-trace", "1000003"])
            if rc or "ld32 %08X -> 00000002" % (OBJ + 4) not in out:
                fails.append(".zst capture: %s" % out[-300:])
        # 3
        expect_exit2("wrong hash", spec("0" * 64, meta), "SHA-256")
        open(store + "/a/bad.bin", "wb").write(b"NOTACAPT" + bytes(32))
        expect_exit2("not WWCAPMEM", spec(hashlib.sha256(open(store + "/a/bad.bin", "rb").read()).hexdigest(), meta, name="a/bad.bin"), "WWCAPMEM")
        b = open(store + "/a/mem.bin", "rb").read()[:-1]
        open(store + "/a/short.bin", "wb").write(b)
        expect_exit2("truncated", spec(hashlib.sha256(b).hexdigest(), meta, name="a/short.bin"), "WWCAPMEM")
        expect_exit2("overlap", spec(sha, meta, windows=("54200000 C", "54200008 8")), "overlap")
        expect_exit2("no capturemap", spec(sha, meta, windows=()), "capture without capturemap")
        expect_exit2("capturemap alone", spec(None, None), "capturemap without a capture")
        for w, what in (("10100000 10", ".rodata"), ("60000100 10", "runtime"), ("F4000000 10", "MEM1"), ("E0000000 10", "foreground")):
            expect_exit2("window " + what, spec(sha, meta, windows=("54200000 C", w)), what)
        for top, what in (("field * 0x54200004 s32 3", "field"), ("poke * 54200004 u32 3", "poke"), ("fill * 0x54300000 16 0", "fill"),
                          ("setup %s 028F5A1C r3=0" % FUNC, "setup")):
            expect_exit2("on top: " + what, good + [top], "sets nothing by hand")
        expect_exit2("no capturecall", spec(sha, None), "capture without capturecall")
        expect_exit2("capturecall hash", spec(sha, "1" * 64), "capture.txt SHA-256")
        expect_exit2("capturecall before capture", ["capturecall " + meta] + spec(sha, None), "after capture")
        expect_exit2("argument drift", spec(sha, meta, args=(("r3", OBJ + 4), ("r4", 8))), "drift")
        # 4
        gen = store + "/gen.txt"
        open(gen, "w").close()
        rc, out = run(store, "cov", spec(sha, meta), env={"WWHD_CAPTURE_GENLOG": gen})
        covered = open(gen).read()
        open(gen, "w").close()
        rc2, out2 = run(store, "unc", spec(sha, meta, windows=("54200000 4",)), env={"WWHD_CAPTURE_GENLOG": gen})
        uncovered = open(gen).read()
        def has(text, a):
            return any(int(l.split()[0], 16) <= a < int(l.split()[0], 16) + int(l.split()[1], 16) for l in text.splitlines())
        if has(covered, OBJ + 4):
            fails.append("a byte inside a window is logged as generated: %s" % covered[:200])
        if not has(uncovered, OBJ + 4):
            fails.append("a byte outside every window is not generated: %s | %s" % (uncovered[:200], out2[-200:]))
        # 5
        rl = store + "/read.txt"
        open(rl, "w").close()
        diag = spec(sha, None, windows=("10000000 50000000", "60000000 110000"))
        rc, out = run(store, "diag", diag, env={"WWHD_CAPTURE_READLOG": rl})
        if rc or not has(open(rl).read(), OBJ + 4):
            fails.append("read log: rc %d %s" % (rc, open(rl).read()[:200]))
        # 6
        rc, out = run(store, "stable", good, n="40", extra=["-trace", "1000004"])
        vals = set(re.findall(r"ld32 %08X -> ([0-9A-F]{8})" % (OBJ + 4), out))
        if rc or vals != {"00000002"}:
            fails.append("the captured word changed (clobber?): %s rc %d" % (vals, rc))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(store, ignore_errors=True)
    for f in fails:
        print("FAIL:", f)
    print("capture_test:", "PASS" if not fails else "FAIL (%d)" % len(fails))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
