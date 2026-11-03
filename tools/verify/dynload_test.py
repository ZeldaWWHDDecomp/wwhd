#!/usr/bin/env python3
"""Self-test of the `osmem ... act` and `osmem ... dynload` parts (harness.cpp osm::dyn_call, src/dynload_model.h;
2026-11-02 "(b) as needed"): the port's nn::act, OSDynLoad and erreula HLE on original
code. The model against the port's own code is dynload_equiv.py's job; this checks the harness integration.

Carrier: cking::system::ErrorViewerTask (wwhd_src/hd/hd_sys_02032D0C.cpp), unit tools/verify/units/_dynload_test/
dynload.txt: createViewer 020331A8 runs the original errorCreate 0286C5F8 (SetAllocator, Acquire("erreula.rpl"), 20
FindExports through the wrappers 0286CBCC.. into the slots 104B6748..104B6794, ErrEulaCreate through 104B675C);
stateClosing 02033AAC reads the viewer state through the trampoline 0286CA30 (GetStateErrorViewer); launch 02033AEC
(page 0x14) calls nn::act::GetSlotNo.
 1. rows ok at n 30, seeds 1 and 7, plain and -nulltrap, for 020331A8, 02033AAC, 02033AEC.
 2. -trace of 020331A8: Acquire -> 0, one handle for all 20 FindExports, the exports C2000018, C2000020, ... C20000B0
    in the game's lookup order, ErrEulaCreate (C2000028) -> 1, on both sides; -watch: the 20 slots hold those
    addresses and 104B679C the handle.
 3. The erreula timing through setups (createViewer, show = AppearError, K x errorSetInput 0286CA50 = one Calc each):
    K 6 -> state 1 (appearing), K 7 -> 2 (visible), K 37 -> visible and not decided, K 38 -> decided.
 4. nn::act: launch(0x14) gets GetSlotNo -> 1 on both sides.
 5. setupsnapshot (K 7): rows ok, not refused, the state still 2 (run state); -nofastreset gives the same rows.
 6. Without the parts (`osmem heaps`) there is no act / dynload answer (generic calls to C0009A98 / C0004660).
 8. The `swkbd` word: prepare 026195C4 runs the original loader 0286CD7C:
    Acquire("swkbd.rpl") -> 0, handle 702E7979 for all 35 FindExports, exports C2000018.. in the loader's order, the slots
    104B67A0..104B6828 hold them, SwkbdCreate (C2000048) -> 1 on both sides, form buffer taken once from the runtime region;
    a call to another export (isHidden 0261A008 -> SwkbdGetStateKeyboard through its slot) is a model rejection naming it, on
    both sides; without the word FindExport is the rejection, as before; `osmem swkbd` without `dynload` exits 2.
 9. The `procui` word (carrier units/_dynload_test/procui.txt): procUI 020350C4 gets ProcUIProcessMessages -> 0 on both sides and
    never runs its exit block; with ExitRequested as the answer the block runs (the check fails without the word); without the
    word the answer is generic; C0007880 in another mock directive exits 2.
 7. Spec errors exit 2: an unknown osmem word, a second osmem line, a dynload import or an export address in another
    mock directive (ret C0009AC0, ret C2000020), an act import in another mock directive (ret C0004660).
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
CREATE, CLOSING, LAUNCH = "020331A8", "02033AAC", "02033AEC"
SLOTS = [0x104B6748, 0x104B674C, 0x104B675C, 0x104B6760, 0x104B6764, 0x104B6768, 0x104B676C, 0x104B6774, 0x104B6784, 0x104B6788,
         0x104B678C, 0x104B6754, 0x104B6758, 0x104B6790, 0x104B6770, 0x104B6750, 0x104B6794, 0x104B6780, 0x104B6778, 0x104B677C]


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def main():
    exe = verify.build("_dynload_test/dynload", jobs=8)
    base = os.path.join(HERE, "units", "_dynload_test", "dynload.txt")
    L = open(base).read().rstrip("\n").split("\n")
    W = tempfile.mkdtemp(prefix="dynload_test-", dir=verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify"))
    print("dynload_test: specs in %s (not removed)" % W)

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

    sp = spec("base", L)
    # 1
    for f in (CREATE, CLOSING, LAUNCH):
        for seed in ("1", "7"):
            for extra in ([], ["-nulltrap"]):
                rc, out, err = run(sp, extra, f, seed=seed)
                r = row(out, f)
                check(rc == 0 and r.endswith(" ok") and "30/30" in r, "1 %s seed %s %s: %s" % (f, seed, " ".join(extra) or "plain", r or err[-300:]))
    # 2
    rc, out, err = run(sp, ["-trace", "1000003"], CREATE, n="1")
    acq = re.findall(r"\n        nested dynload OSDynLoad_Acquire r3=1017DDB0 r4=104B679C .* -> 00000000", out)
    finds = re.findall(r"\n        nested dynload OSDynLoad_FindExport r3=(\w+) r4=00000000 r5=\w+ r6=(\w+) -> 00000000 \(new export (\w+)\)", out)
    check(len(acq) == 2, "2 Acquire(\"erreula.rpl\") -> 0 on both sides (%d)" % len(acq))
    want = [("%08X" % s, "%08X" % (0xC2000018 + 8 * i)) for i, s in enumerate(SLOTS)]
    got = [(s, a) for _, s, a in finds]
    check(got == want + want, "2 20 FindExports per side, exports C2000018.. in lookup order")
    handles = {h for h, _, _ in finds}
    check(len(handles) == 1 and next(iter(handles)).startswith("7"), "2 one handle 7xxxxxxx for every FindExport: %s" % sorted(handles))
    cr = len(re.findall(r"dynload export C2000028 erreula\.ErrEulaCreate__3RplFPUcQ3_2nn7erreula10RegionTypeQ3_2nn7erreula8LangTypeP8FSClient "
                        r"r3=\w+ r4=\w+ -> 00000001 \(erreula state 0", out))
    check(cr == 2, "2 ErrEulaCreate through slot 104B675C -> 1 on both sides (%d)" % cr)
    rc, out, err = run(sp, ["-watch", "104B6748:0x58"], CREATE, n="1")
    mem = {}
    for l in err.splitlines():
        f = l.split()
        if len(f) >= 5 and f[0] == "watch" and f[1] in ("W", "P") and "side=orig" in l:
            mem[int(f[2], 16)] = int(f[3], 16)

    def w32(a):
        b = [mem.get(a + j) for j in range(4)]
        return None if None in b else int.from_bytes(bytes(b), "big")
    check(all(w32(s) == 0xC2000018 + 8 * i for i, s in enumerate(SLOTS)), "2 -watch: the slots hold the export addresses")
    check(handles and w32(0x104B679C) == int(next(iter(handles)), 16), "2 -watch: 104B679C holds the handle (%r)" % (w32(0x104B679C),))
    # 3
    def calc_spec(k, extra=()):
        return spec("calc%d%s" % (k, "_".join(extra)), L + ["setup %s 0286CA50 r3=0x5D000000" % CLOSING] * k + list(extra))

    def state_line(sp_k, args=()):
        """per side, the tested call's GetStateErrorViewer answer (the setups' errorSetInput reads the state too)"""
        rc, out, err = run(sp_k, ["-trace", "1000003"] + list(args), CLOSING, n="1")
        pat = re.compile(r"dynload export C2000050 erreula\.ErrEulaGetStateErrorViewer__3RplFv r3=\w+ r4=\w+ -> (\w+) \(erreula state (\d), decided (\d)\)")
        m, last = [], None
        for l in out.splitlines():
            if l.startswith("        setup "):
                last = None
            elif pat.search(l):
                last = pat.search(l).groups()
            elif "net effect (" in l:
                m.append(last)
                last = None
        return rc, m, err
    for k, want in ((6, ("00000001", "1", "0")), (7, ("00000002", "2", "0")), (37, ("00000002", "2", "0")), (38, ("00000002", "2", "1"))):
        rc, m, err = state_line(calc_spec(k))
        check(rc == 0 and m == [want, want], "3 %d Calc(s) after AppearError: state / decided %r (want %r on both sides)" % (k, m, want))
    # 4
    rc, out, err = run(sp, ["-trace", "1000003"], LAUNCH, n="1")
    check(len(re.findall(r"\n        act nn::act::GetSlotNo r3=\w+ .* -> 00000001", out)) == 2, "4 launch(0x14): GetSlotNo -> 1 on both sides")
    # 5
    snap = calc_spec(7, ["setupsnapshot"])
    rc, out, err = run(snap, [], CLOSING)
    r = row(out, CLOSING)
    check(rc == 0 and r.endswith(" ok") and "refused" not in err, "5 setupsnapshot: %s %s" % (r, [l for l in err.splitlines() if "setupsnapshot" in l][:1]))
    rc, m, err = state_line(snap)
    check(m == [("00000002", "2", "0")] * 2, "5 setupsnapshot: the state is still 2 (%r)" % m)
    k7 = calc_spec(7)
    rc1, out1, _ = run(k7, [], CLOSING, n="50")
    rc2, out2, _ = run(k7, ["-nofastreset"], CLOSING, n="50")
    check(rc1 == rc2 == 0 and row(out1, CLOSING) == row(out2, CLOSING) and row(out1, CLOSING).endswith(" ok"), "5 -nofastreset rows equal")
    # 6
    off = spec("off", [l.replace("osmem heaps act dynload", "osmem heaps") for l in L])
    rc, out, err = run(off, ["-trace", "1000003"], CREATE, n="1")
    check(rc == 0 and "dynload" not in out and "call C0009A98" in out, "6 without `dynload` Acquire is a generic call")
    rc, out, err = run(off, ["-trace", "1000003"], LAUNCH, n="1")
    check(rc == 0 and "act nn::act" not in out and "call C0004660" in out, "6 without `act` GetSlotNo is a generic call")
    # 7
    bad = {"word": [l.replace("osmem heaps act dynload", "osmem heaps act dynlod") for l in L], "twice": L + ["osmem act"],
           "dynimport": L + ["ret C0009AC0 0 1"], "export": L + ["ret C2000020 0 1"], "actimport": L + ["ret C0004660 0 1"]}
    for name, lines in bad.items():
        rc, out, err = run(spec("err_" + name, lines), [], CREATE, n="1")
        check(rc == 2, "7 %s exits 2: %s" % (name, err.strip().splitlines()[-1][:160] if err.strip() else rc))
    # 8
    exe2 = verify.build("_dynload_test/swkbd", jobs=8)
    L2 = open(os.path.join(HERE, "units", "_dynload_test", "swkbd.txt")).read().rstrip("\n").split("\n")
    PREP, HIDDEN = "026195C4", "0261A008"

    def run2(sp2, args, only, n="30", seed="1"):
        r = subprocess.run([exe2, "-spec", sp2, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", only] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace", timeout=1800)
        return r.returncode, r.stdout, r.stderr
    sw = spec("swkbd", L2)
    for seed in ("1", "7"):
        for extra in ([], ["-nulltrap"]):
            rc, out, err = run2(sw, extra, PREP, seed=seed)
            r = row(out, PREP)
            check(rc == 0 and r.endswith(" ok") and "30/30" in r, "8 %s seed %s %s: %s" % (PREP, seed, " ".join(extra) or "plain", r or err[-300:]))
    rc, out, err = run2(sw, ["-trace", "1000003"], PREP, n="1")
    finds = re.findall(r"nested dynload OSDynLoad_FindExport r3=(\w+) r4=00000000 r5=\w+ r6=(\w+) -> 00000000 \(new export (\w+)\)", out)
    check(len(re.findall(r"nested dynload OSDynLoad_Acquire r3=1017E2A0 r4=104B6830 .* -> 00000000", out)) == 2, "8 Acquire(\"swkbd.rpl\") -> 0 on both sides")
    check(len(finds) == 70 and {h for h, _, _ in finds} == {"702E7979"}, "8 35 FindExports per side, handle 702E7979 (%d)" % len(finds))
    slots = [0x104B67A0 + 4 * i for i in range(35)]
    want = [("%08X" % a, "%08X" % (0xC2000018 + 8 * i)) for i, a in enumerate(slots)]
    check([(s_, a) for _, s_, a in finds] == want + want, "8 exports C2000018.. in the loader's order, slots 104B67A0..104B6828")
    check(len(re.findall(r"dynload export C2000048 swkbd\.SwkbdCreate__3RplF\w+ r3=\w+ r4=\w+ -> 00000001", out)) == 2, "8 SwkbdCreate through 104B67B8 -> 1 on both sides")
    rc, out, err = run2(sw, ["-watch", "104B67A0:0x8C"], PREP, n="1")
    mem = {}
    for l in err.splitlines():
        f = l.split()
        if len(f) >= 5 and f[0] == "watch" and f[1] in ("W", "P") and "side=orig" in l:
            mem[int(f[2], 16)] = int(f[3], 16)
    got = [int.from_bytes(bytes(mem.get(a + j, 0) for j in range(4)), "big") for a in slots]
    check(got == [0xC2000018 + 8 * i for i in range(35)], "8 -watch: the 35 slots hold the export addresses")
    rc, out, err = run2(sw, ["-trace", "1000003"], HIDDEN, n="1")
    r = row(out, HIDDEN)
    check(rc != 0 and "aborted on both sides" in r and "0/0" in r, "8 call to an unmodelled export ends the input on both sides, no passed row: %s" % r)
    for side in ("original", "candidate"):
        check("aborted (%s side): osmem swkbd: call to SwkbdGetStateKeyboard__3RplFv (not modelled: by need)" % side in out, "8 the rejection names SwkbdGetStateKeyboard (%s)" % side)
    noword = spec("swkbd_noword", [l.replace("osmem heaps dynload swkbd", "osmem heaps dynload") for l in L2])
    rc, out, err = run2(noword, ["-trace", "1000003"], PREP, n="1")
    check("aborted (original side): osmem dynload: FindExport of a function in a module other than erreula" in out and "SwkbdCreate" not in out, "8 without the word FindExport is the rejection")
    bad2 = spec("swkbd_nodyn", [l.replace("osmem heaps dynload swkbd", "osmem heaps swkbd") for l in L2])
    rc, out, err = run2(bad2, [], PREP, n="1")
    check(rc == 2, "8 `swkbd` without `dynload` exits 2: %s" % (err.strip().splitlines()[-1][:120] if err.strip() else rc))
    # 9
    exe3 = verify.build("_dynload_test/procui", jobs=8)
    L3 = open(os.path.join(HERE, "units", "_dynload_test", "procui.txt")).read().rstrip("\n").split("\n")
    PROCUI = "020350C4"
    EXIT = "C0007880"

    def run3(sp3, args, n="200", seed="1"):
        r = subprocess.run([exe3, "-spec", sp3, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", PROCUI] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace", timeout=1800)
        return r.returncode, r.stdout, r.stderr
    pu = spec("procui", L3)
    for seed in ("1", "7"):
        for extra in ([], ["-nulltrap"]):
            rc, out, err = run3(pu, extra, n="30", seed=seed)
            r = row(out, PROCUI)
            check(rc == 0 and r.endswith(" ok") and "30/30" in r, "9 %s seed %s %s: %s" % (PROCUI, seed, " ".join(extra) or "plain", r or err[-300:]))
    rc, out, err = run3(pu, ["-trace", "1000003"], n="1")
    check(len(re.findall(r"\n        osmem C0007880 r3=00000001 .* -> 00000000", out)) == 2, "9 ProcUIProcessMessages(1) -> 0 on both sides")
    # the exit block's OSBlockThreadsOnExit-side calls: the block's first call, setExitRequested's manager getter
    gate = re.search(r"^ext ([0-9A-F]{8}) \S+ HDFramework_getExitManager", open(os.path.join(ROOT, "wwhd_src", "hd", "hd_sys_02034B14.cpp")).read(), re.M).group(1)
    runs = lambda sp_, seed_: subprocess.run([exe3, "-spec", sp_, "-noprogress", "-strictnan", "-n", "60", "-seed", seed_, "-only", PROCUI, "-trace", "1000003"],
                                             cwd=ROOT, capture_output=True, text=True, errors="replace", timeout=1800).stdout
    check(("call " + gate) not in runs(pu, "1"), "9 with `procui` the exit block does not run")
    off = spec("procui_off", [l.replace("osmem procui", "osmem heaps") for l in L3] + ["ret! C0007880 3 3"])
    check(("call " + gate) in runs(off, "1"), "9 the check fails without it: an ExitRequested answer runs the exit block (%s)" % gate)
    gen = spec("procui_gen", [l.replace("osmem procui", "osmem heaps") for l in L3])
    rc, out, err = run3(gen, ["-trace", "1000003"], n="1")
    check("call C0007880" in out and "osmem C0007880" not in out, "9 without the word the answer is generic")
    bad3 = spec("procui_conflict", L3 + ["ret C0007880 0 1"])
    rc, out, err = run3(bad3, [], n="1")
    check(rc == 2, "9 C0007880 in another mock directive exits 2: %s" % (err.strip().splitlines()[-1][:120] if err.strip() else rc))
    # the per-library stub deltas: a stub counts as the import, the coreinit delta's address does not
    for name, sp_lines, want in (("act stub 028F8AA0", L + ["ret 028F8AA0 0 1"], 2), ("act old alias 028F88D8", L + ["ret 028F88D8 0 1"], 0)):
        rc, out, err = run(spec("stub_" + name.split()[-1], sp_lines), [], CREATE, n="1")
        check(rc == want, "7 %s: exit %d (want %d)" % (name, rc, want))
    rc, out, err = run3(spec("stub_procui", L3 + ["ret 028FBB90 0 1"]), [], n="1")
    check(rc == 2, "9 procui stub 028FBB90 exits 2 in another mock directive")
    print("dynload_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
