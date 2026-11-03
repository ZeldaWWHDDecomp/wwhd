#!/usr/bin/env python3
"""Self-test of the `osmem ... portconst` word (harness.cpp osm::call, src/portans_model.h): the port's constant answers on
original code. The table against the port's own code is portans_equiv.py's job.
Carrier units/_portans_test/portconst.txt.
 1. rows ok at n 30, seeds 1 and 7, plain and -nulltrap, for each function.
 2. -trace: each import is answered by the model on both sides with the port's value (Initialize 0, Connect A0000000,
    IMDisableDim 0, OSEnableHomeButtonMenu 1, GX2DrawDone 1, OSGetSharedData 0).
 3. Without the word each of those calls is the generic mock, so the checks of 2 fail without it.
 4. Every table row, at its slot and its stub, is a spec error (exit 2) in another mock directive with the word, and
    not without it. An unknown word exits 2.
 5. `portmem` on original code (its carrier; lib2_sead_9 for SAVEOpenFile -> -6; lib2_sead_11_value_boundaries for the GX2Sample and
    GX2GPUTimeToCPUTime calls), `portmem` without `threads` exits 2, off without the word, conflicts exit 2. GX2SetClearDepthStencil,
    GX2SetSurfaceSwizzle and GX2InitDepthBufferHiZEnable rest on portans_equiv.py and the conflict check.
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
CALLS = {"0203CA70": [("C0007E48", "00000000"), ("C0007CA0", "A0000000")], "02032AD8": [("C0009010", "00000000")],
         "02035ED4": [("C0009B18", "00000001")], "027BF730": [("C0006168", "00000001")], "025F99BC": [("C0009C58", "00000000")]}


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def main():
    exe = verify.build("_portans_test/portconst", jobs=8)
    L = open(os.path.join(HERE, "units", "_portans_test", "portconst.txt")).read().rstrip("\n").split("\n")
    W = tempfile.mkdtemp(prefix="portans_test-", dir=verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify"))
    print("portans_test: specs in %s (not removed)" % W)

    def spec(name, lines):
        p = os.path.join(W, name + ".txt")
        open(p, "w").write("\n".join(lines) + "\n")
        return p

    def run(sp, args, only, n="30", seed="1"):
        r = subprocess.run([exe, "-spec", sp, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", only] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace", timeout=1800)
        return r.returncode, r.stdout, r.stderr
    sp = spec("base", L)
    off = spec("off", [l.replace("osmem portconst", "osmem heaps") for l in L])
    for f, calls in CALLS.items():
        for seed in ("1", "7"):
            for extra in ([], ["-nulltrap"]):
                rc, out, err = run(sp, extra, f, seed=seed)
                r = next((l for l in out.splitlines() if l.startswith(f)), "")
                check(rc == 0 and r.endswith(" ok") and "30/30" in r, "1 %s seed %s %s: %s" % (f, seed, " ".join(extra) or "plain", r or err[-300:]))
        rc, out, err = run(sp, ["-trace", "1000003"], f, n="1")
        for slot, val in calls:
            n = len(re.findall(r"osmem %s r3=\w+ .*-> %s" % (slot, val), out))
            check(n == 2, "2 %s: %s -> %s on both sides (%d)" % (f, slot, val, n))
        rc, out, err = run(off, ["-trace", "1000003"], f, n="1")
        for slot, val in calls:
            check("osmem %s" % slot not in out and ("call %s" % slot) in out, "3 %s: without the word %s is a generic call" % (f, slot))
    rows = re.findall(r"\{0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u, \w+, \"", open(os.path.join(HERE, "src", "portans_model.h")).read())
    check(len(rows) == 13, "4 13 table rows")
    for slot, delta in rows:
        stub = "%08X" % ((int(slot, 16) - int(delta, 16)) & 0xFFFFFFFF)
        for t in (slot, stub):
            rc, out, err = run(spec("c" + t, L + ["ret %s 0 1" % t]), [], "02032AD8", n="1")
            check(rc == 2, "4 %s in another mock directive exits 2 (%d)" % (t, rc))
            rc, out, err = run(spec("n" + t, [l.replace("osmem portconst", "osmem heaps") for l in L] + ["ret %s 0 1" % t]), [], "02032AD8", n="1")
            check(rc != 2, "4 %s without the word is no spec error (%d)" % (t, rc))
    rc, out, err = run(spec("word", [l.replace("osmem portconst", "osmem portconstx") for l in L]), [], "02032AD8", n="1")
    check(rc == 2, "4 an unknown word exits 2")
    # 5 `portmem`, on original code: its carrier, and existing units with the word instead of their mocks
    def variant(unit):
        lines = open(os.path.join(HERE, "units", unit + ".txt")).read().rstrip("\n").split("\n")
        slots = "C0007490|C00061B8|C0006760|C0006748|C00067C0|C00069B8|C0009060|C00063E8"
        return [l for l in lines if not l.startswith("osmem") and not re.match(r"(ret!?|mockfield|retfield|nestedret) (%s)\b" % slots, l)] + ["osmem threads portmem"]

    def runu(exe_, sp_, f, args, n="1", seed="1"):
        r = subprocess.run([exe_, "-spec", sp_, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", f] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace", timeout=1800)
        return r.returncode, r.stdout, r.stderr
    PM = [("_portans_test/portmem", "02032B48", "C0009060", r"-> 00000000", "base"), ("lib2_sead_9", "02744908", "C0007490", r"-> FFFFFFFA", "unit"),
          ("lib2_sead_11_value_boundaries", "0274C3B8", "C0006760", r"\(r3 kept\)", "unit"), ("lib2_sead_11_value_boundaries", "0274C038", "C00061B8", r"\(r3 kept\)", "unit")]
    for unit, f, slot, want, kind in PM:
        ex = verify.build(unit, jobs=8)
        sp_ = spec("pm_" + unit.replace("/", "_"), variant(unit) if kind == "unit" else open(os.path.join(HERE, "units", unit + ".txt")).read().rstrip("\n").split("\n"))
        rc, out, err = runu(ex, sp_, f, ["-trace", "1000003"])
        hit = re.findall(r"portmem %s r3=\w+ r4=\w+ %s" % (slot, want.replace("-> ", "-> ")), out) if "kept" not in want else re.findall(r"portmem %s .*\(r3 kept\)" % slot, out)
        check(rc != 2 and len(hit) >= 2, "5 %s %s: %s answered on both sides (%d): %s" % (unit, f, slot, len(hit), err.strip()[-160:] if rc == 2 else ""))
    ex = verify.build("_portans_test/portmem", jobs=8)
    Lm = open(os.path.join(HERE, "units", "_portans_test", "portmem.txt")).read().rstrip("\n").split("\n")
    rc, out, err = runu(ex, spec("pm_nothr", [l.replace("osmem threads portmem", "osmem portmem") for l in Lm]), "02032B48", [])
    check(rc == 2, "5 `portmem` without `threads` exits 2")
    rc, out, err = runu(ex, spec("pm_off", [l.replace("osmem threads portmem", "osmem threads") for l in Lm]), "02032B48", ["-trace", "1000003"])
    check("portmem" not in out and "call C0009060" in out, "5 without the word IMIsDimEnabled is a generic call")
    for t in ("C0009060", "C0007490", "C0006760", "C00061B8", "C00063E8", "028FAC30"):
        rc, out, err = runu(ex, spec("pm_c" + t, Lm + ["ret %s 0 1" % t]), "02032B48", [])
        check((rc == 2) == (t != "028FAC30"), "5 %s in another mock directive: exit %d" % (t, rc))
    print("portans_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
