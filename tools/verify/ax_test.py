#!/usr/bin/env python3
"""Self-test of the `osmem ... ax` part (harness.cpp, src/gthr_sched.inc AX section, src/ax_model.h; ): the
port's snd_core with its AX frame thread. The model's arithmetic and order against the port's ax.cpp are ax_equiv.py's job;
this checks the harness integration on original code.

Carrier: sead::AudioSystem::initializeDevice 02762FB4 (AudioSystemRuntime.cpp: AXInit, AXSetDefaultMixerSelect,
AXSetDeviceLinearUpsampler, AXSetDeviceCompressor), unit tools/verify/units/_guestthreads_test/ax.txt.
 1. rows ok at n 30, seeds 1 and 7, also with -nulltrap.
 2. -trace: AXInit answered by the model, the AX frame thread (priority -1, core 0) created and its first frame run at once
    (the drain after the tested call), the same schedule digest on both sides.
 3. The GamePad aux buffers (the fixed host region from 6FE00000) stay zero until the game writes them: -watch over them
    shows only zero stores (the frame's memsets), none of another value.
 4. AXGetDeviceMode etc. are answered only with `ax`: without it (`osmem hle threads`) AXInit gets the generic answer and no
    AX frame thread exists.
 5. Spec errors exit 2: `ax` without `threads`; the fixed region 6FE00000..6FE40000 overlapping mainthread= or a fixture
    `fill` line; a bad `ostime=`.
Exit 0 when all pass."""
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

FAILS = []
F = "02762FB4"


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def run(exe, spec, args, n="30", seed="1"):
    r = subprocess.run([exe, "-spec", spec, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", F] + args, cwd=ROOT,
                       capture_output=True, text=True, errors="replace", timeout=1800)
    return r.returncode, r.stdout, r.stderr


def row(out):
    return next((l for l in out.splitlines() if l.startswith(F)), "")


def main():
    exe = verify.build("_guestthreads_test/ax", jobs=4)
    base = os.path.join(HERE, "units", "_guestthreads_test", "ax.txt")
    L = open(base).read().split("\n")
    W = tempfile.mkdtemp(prefix="ax_test-", dir=verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify"))
    print("ax_test: specs in %s (not removed)" % W)

    def spec(name, lines):
        p = os.path.join(W, name + ".txt")
        open(p, "w").write("\n".join(lines) + "\n")
        return p

    sp = spec("base", L)
    for seed in ("1", "7"):
        for extra in ([], ["-nulltrap"]):
            rc, out, err = run(exe, sp, extra, seed=seed)
            r = row(out)
            check(rc == 0 and r.endswith(" ok") and "30/30" in r, "1 seed %s %s: %s" % (seed, " ".join(extra) or "plain", r or err[-300:]))
    rc, out, err = run(exe, sp, ["-trace", "1000003"], n="1")
    for w in ["ax T0@c1 C0006E30", "AX frame thread (priority -1, core 0), first frame at clock", "thread ax frame 0 on T1@c0"]:
        check(w in out, "2 trace shows %r" % w)
    dig = re.findall(r"threads \((original|candidate) side\): schedule digest ([0-9A-F]+)", out)
    check(len(dig) == 2 and dig[0][1] == dig[1][1], "2 both sides have the same schedule digest %s" % dig)
    rc, out, err = run(exe, sp, ["-watch", "6FE00000:0x2400"], n="1")
    w = [l for l in err.splitlines() if l.startswith("watch ")]
    stores = [l for l in w if l.split()[1] in ("W", "P")]
    nonzero = [l for l in stores if int(l.split()[3], 16) != 0]
    check(len(stores) > 0 and not nonzero, "3 GamePad aux buffers: %d store(s), all zero%s" % (len(stores), (": " + nonzero[0]) if nonzero else ""))
    sp4 = spec("off", [l.replace("osmem hle threads ax", "osmem hle threads") for l in L])
    rc, out, err = run(exe, sp4, ["-trace", "1000003"], n="1")
    check("AX frame thread" not in out and "ax T0" not in out, "4 without `ax` there is no AX model")
    for name, lines in [("noth", [l.replace("osmem hle threads ax", "osmem hle ax") for l in L]),
                        ("mainov", [l.replace("osmem hle threads ax", "osmem hle threads ax mainthread=6FE10000") for l in L]),
                        ("fillov", L + ["fill * 0x6FE3F000 0x2000 0"]),
                        ("ostime", [l.replace("osmem hle threads ax", "osmem hle threads ax ostime=12") for l in L])]:
        rc, out, err = run(exe, spec("err_" + name, lines), [], n="1")
        check(rc == 2, "5 %s exits 2: %s" % (name, err.strip().splitlines()[-1] if err.strip() else rc))
    print("ax_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
