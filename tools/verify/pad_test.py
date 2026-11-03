#!/usr/bin/env python3
"""Self-test of the `osmem ... pad` part (harness.cpp osm::pad_call, src/pad_model.h;
2026-11-02): the port's GamePad / Pro Controller HLE with the buttons from a `padsched` press schedule. The model
against the port's own code is pad_equiv.py's job; this checks the harness integration on original code.

Carrier: sead::TabletDevice::read 0273E3FC (VPADRead(0, this+0x14, 16, this+0xAD8), count at +0xAD4) and
sead::RemoteDevice::read 0273D8EC (KPADReadEx(chan, this+0x1118+chan*0xF08, 16, ...+0xF04) for chan 0..3, counts at
...+0xF00), unit tools/verify/units/_pad_test/pad.txt: two setups run 0273E3FC first (reads #0 and #1), so the tested
call is read #2. Schedule: read 0 A; reads 2-3 A+PLUS, left stick (0.7, 0), right stick (0, -0.6), touch (0.25, 0.75).
 1. rows ok at n 30, seeds 1 and 7, plain and -nulltrap, for both functions.
 2. -trace: the setups' reads #0 (hold 00008000) and #1 (idle) nested, the tested read #2 with hold 20808008 (A, PLUS,
    left stick right, right stick down) on both sides.
 3. -watch over the VPADStatus and the count/error words: the final status of the tested read: hold / trig 20808008,
    release 0, sticks 0.7 0 0 -0.6, touch raw (0x0426, 0x042E) touched and valid in the three samples, slide volume and
    battery bytes, the rest pose (acc_magnitude about 1), error word 0, count 1.
 4. Idle: without padsched lines the read is "connected, nothing pressed" (r3 1, hold 0, touch invalid).
 5. The GamePad mode: every KPADReadEx channel answers 0 with error -2 (FFFFFFFE); `padpro`: channel 0 reads KPAD #0
    (r3 1, URCC devType 31, hold 0x10 = A, trig 0x10) and channels 1..3 answer 0 / -2; VPADRead in Pro Controller
    mode keeps the touch but no buttons.
 6. setupsnapshot (the setups captured once): rows ok and the tested call is still read #2 (the read ordinals are run
    state); -nofastreset gives the same rows.
 7. Without `pad` the pad imports get the generic answer (no `pad VPADRead` line).
 8. Spec errors exit 2: padsched without `pad`, `padpro` without `pad`, an unknown button name, a button bit the host
    input cannot produce (0x10000), a stick outside -1..1, touch outside 0..1 or without a comma, overlapping reads,
    LAST < FIRST, extra tokens, a pad import named in another mock directive.
Exit 0 when all pass."""
import os
import re
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

FAILS = []
TAB, REM = "0273E3FC", "0273D8EC"
OBJ = 0x5A000000


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def main():
    exe = verify.build("_pad_test/pad", jobs=8)
    base = os.path.join(HERE, "units", "_pad_test", "pad.txt")
    L = open(base).read().rstrip("\n").split("\n")
    W = tempfile.mkdtemp(prefix="pad_test-", dir=verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify"))
    print("pad_test: specs in %s (not removed)" % W)

    def spec(name, lines):
        p = os.path.join(W, name + ".txt")
        open(p, "w").write("\n".join(lines) + "\n")
        return p

    def run(sp, args, only, n="30", seed="1"):
        r = subprocess.run([exe, "-spec", sp, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", only] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace", timeout=1800)
        return r.returncode, r.stdout, r.stderr

    def row(out, f):
        return next((l for l in out.splitlines() if l.startswith(f)), "")

    def final(sp, only, lo, size):
        """the original side's last store per byte in lo..lo+size (from -watch), as a bytearray (None: never stored)"""
        rc, out, err = run(sp, ["-watch", "%08X:0x%X" % (lo, size)], only, n="1")
        mem = [None] * size
        for l in err.splitlines():
            f = l.split()
            if len(f) >= 5 and f[0] == "watch" and f[1] in ("W", "P") and "side=orig" in l:
                mem[int(f[2], 16) - lo] = int(f[3], 16)
        return mem

    def w32(mem, off):
        b = mem[off:off + 4]
        return None if None in b else struct.unpack(">I", bytes(b))[0]

    def w16(mem, off):
        b = mem[off:off + 2]
        return None if None in b else struct.unpack(">H", bytes(b))[0]

    def f32(mem, off):
        v = w32(mem, off)
        return None if v is None else struct.unpack(">f", struct.pack(">I", v))[0]

    sp = spec("base", L)
    # 1
    for f in (TAB, REM):
        for seed in ("1", "7"):
            for extra in ([], ["-nulltrap"]):
                rc, out, err = run(sp, extra, f, seed=seed)
                r = row(out, f)
                check(rc == 0 and r.endswith(" ok") and "30/30" in r, "1 %s seed %s %s: %s" % (f, seed, " ".join(extra) or "plain", r or err[-300:]))
    # 2
    rc, out, err = run(sp, ["-trace", "1000003"], TAB, n="1")
    for w in ["nested pad VPADRead read #0 (chan 0, buffer 5A000014) hold 00008000 -> 00000001",
              "nested pad VPADRead read #1 (chan 0, buffer 5A000014) hold 00000000 -> 00000001",
              "\n        pad VPADRead read #2 (chan 0, buffer 5A000014) hold 20808008 -> 00000001"]:
        check(out.count(w) == 2, "2 trace shows %r on both sides (%d)" % (w.strip(), out.count(w)))
    # 3
    m = final(sp, TAB, OBJ + 0x14, 0xAD8 + 4 - 0x14)
    st = 0
    exp = {"hold": (w32(m, st), 0x20808008), "trig": (w32(m, st + 4), 0x20808008), "release": (w32(m, st + 8), 0),
           "lx": (f32(m, st + 0xC), struct.unpack(">f", struct.pack(">f", 0.7))[0]), "ly": (f32(m, st + 0x10), 0.0),
           "rx": (f32(m, st + 0x14), 0.0), "ry": (f32(m, st + 0x18), struct.unpack(">f", struct.pack(">f", -0.6))[0]),
           "acc_magnitude ~1": (abs(f32(m, st + 0x28) - 1.0) < 1e-6, True),
           "slide/battery": ((m[st + 0xA0], m[st + 0xA1], m[st + 0xA3]), (0xFF, 0xC0, 0xFF)),
           "count": (w32(m, 0xAD4 - 0x14), 1), "error": (w32(m, 0xAD8 - 0x14), 0)}
    for i in range(3):
        tp = st + 0x52 + 8 * i
        exp["touch %d" % i] = ((w16(m, tp), w16(m, tp + 2), w16(m, tp + 4), w16(m, tp + 6)), (0x0426, 0x042E, 1, 0))
    for k, (got, want) in exp.items():
        check(got == want, "3 status %s: %r (want %r)" % (k, got, want))
    # 4
    idle = spec("idle", [l for l in L if not l.startswith("padsched")])
    rc, out, err = run(idle, ["-trace", "1000003"], TAB, n="1")
    check(out.count("pad VPADRead read #2 (chan 0, buffer 5A000014) hold 00000000 -> 00000001") == 2, "4 idle: read #2 hold 0, r3 1")
    m = final(idle, TAB, OBJ + 0x14, 0xAC)
    check((w32(m, 0), w32(m, 4), w16(m, 0x52 + 6), m[0xA1]) == (0, 0, 3, 0xC0), "4 idle status: hold %r trig %r validity %r battery %r" % (
        w32(m, 0), w32(m, 4), w16(m, 0x58), m[0xA1]))
    # 5
    rc, out, err = run(sp, ["-trace", "1000003"], REM, n="1")
    ks = re.findall(r"\n        pad KPADReadEx r3=(\w+) .* -> (\w+)", out)
    check(len(ks) == 8 and all(r == "00000000" for _, r in ks) and "read #" not in "".join(l for l in out.splitlines() if "KPADReadEx" in l),
          "5 GamePad mode: KPADReadEx answers 0 on all channels (%d calls)" % len(ks))
    m = final(sp, REM, OBJ + 0x1118, 4 * 0xF08)
    check(all(w32(m, c * 0xF08 + 0xF04) == 0xFFFFFFFE and w32(m, c * 0xF08 + 0xF00) == 0 for c in range(4)), "5 GamePad mode: error -2 and count 0 per channel")
    pro = spec("pro", [l.replace("osmem pad", "osmem pad padpro") for l in L])
    rc, out, err = run(pro, ["-trace", "1000003"], REM, n="1")
    check(out.count("pad KPADReadEx read #0 (chan 0, buffer 5A001118) hold 00000010 -> 00000001") == 2, "5 padpro: KPADReadEx chan 0 is read #0 with hold 0x10 (A)")
    m = final(pro, REM, OBJ + 0x1118, 4 * 0xF08)
    check((m[0x5C], m[0x5F], w32(m, 0x60), w32(m, 0x64), w32(m, 0x7C), w32(m, 0x80), w32(m, 0xF00), w32(m, 0xF04)) == (31, 22, 0x10, 0x10, 1, 1, 1, 0),
          "5 padpro: KPADStatus devType 31, format 22, hold/trig 0x10, charge/cable 1, count 1, error 0")
    check(all(w32(m, c * 0xF08 + 0xF04) == 0xFFFFFFFE and w32(m, c * 0xF08 + 0xF00) == 0 for c in (1, 2, 3)), "5 padpro: channels 1..3 error -2, count 0")
    rc, out, err = run(pro, ["-trace", "1000003"], TAB, n="1")
    check(out.count("pad VPADRead read #2 (chan 0, buffer 5A000014) hold 00000000 -> 00000001") == 2, "5 padpro: VPADRead has no buttons")
    m = final(pro, TAB, OBJ + 0x14, 0xAC)
    check((w16(m, 0x52), w16(m, 0x54), w16(m, 0x56)) == (0x0426, 0x042E, 1), "5 padpro: VPADRead keeps the touch")
    # 6
    snap = spec("snap", L + ["setupsnapshot"])
    rc, out, err = run(snap, [], TAB)
    r = row(out, TAB)
    check(rc == 0 and r.endswith(" ok") and "refused" not in err, "6 setupsnapshot: %s %s" % (r, [l for l in err.splitlines() if "setupsnapshot" in l][:1]))
    rc, out, err = run(snap, ["-trace", "1000003"], TAB, n="1")
    check(out.count("\n        pad VPADRead read #2 (chan 0, buffer 5A000014) hold 20808008") == 2, "6 setupsnapshot: the tested call is still read #2")
    rc1, out1, _ = run(sp, [], TAB, n="50")
    rc2, out2, _ = run(sp, ["-nofastreset"], TAB, n="50")
    check(rc1 == rc2 == 0 and row(out1, TAB) == row(out2, TAB), "6 -nofastreset rows equal")
    # 7
    off = spec("off", [l.replace("osmem pad", "osmem heaps") for l in L if not l.startswith("padsched")])
    rc, out, err = run(off, ["-trace", "1000003"], TAB, n="1")
    check(rc == 0 and "pad VPADRead" not in out and "call C0007640" in out, "7 without `pad` there is no pad model")
    # 8
    bad = {"nopad": [l.replace("osmem pad", "osmem heaps") for l in L],
           "pronopad": [l.replace("osmem pad", "osmem heaps padpro") for l in L if not l.startswith("padsched")],
           "name": L + ["padsched 9 A+Q"], "bit": L + ["padsched 9 0x10000"], "plus": L + ["padsched 9 A+"],
           "stick": L + ["padsched 9 A 1.5 0 0 0"], "sticks3": L + ["padsched 9 A 0 0 0"], "touch": L + ["padsched 9 A touch=1.2,0"],
           "comma": L + ["padsched 9 A touch=0.5"], "overlap": L + ["padsched 3-5 B"], "order": L + ["padsched 9-8 B"],
           "extra": L + ["padsched 9 A 0 0 0 0 touch=0,0 x"], "neg": L + ["padsched -1 A"], "conflict": L + ["ret C0007640 0 1"]}
    for name, lines in bad.items():
        rc, out, err = run(spec("err_" + name, lines), [], TAB, n="1")
        check(rc == 2, "8 %s exits 2: %s" % (name, err.strip().splitlines()[-1][:160] if err.strip() else rc))
    print("pad_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
