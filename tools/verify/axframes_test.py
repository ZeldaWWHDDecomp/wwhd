#!/usr/bin/env python3
"""Self-test of the guest-called paths of `osmem ... threads ax` on original code under the scheduler (2026-11-02).

Carriers (committed specs): tools/verify/units/_axframes_test/carrier.txt (`threads ax`: AXInit, the JAudio device mixer's final-mix
callbacks, the app frame callback of the frame driver thread, the nw::snd periodic alarm) and carrier_alarm.txt (`threads` alone;
AXRmtGetSamplesLeft steered so that the alarm callback's capture loop runs). The tested Thread::sleep 02760C3C sleeps 1,000,000
ticks, so frames and alarms fire inside its row, on their service threads.

A. service_call (gthr_sched.inc), the scheduler's copy of the port's guest_call (runtime/src/core.cpp): measured on the guest stack
   with -watch and on the registers with -trace.
    1. Frame: r1 of the service thread minus 0x40, back chain = the old r1 (the harness store `P` at the new r1 holds the old r1).
    2. The callback's own stwu stores the entry r1 (r1 on entry = the frame of 1), its LR save stores 0 (lr = 0 on entry).
    3. Every firing has the same frame: r1 is restored after each call (alarm stack and AX stack; the AX stack with 3 callbacks
       per frame, app / TV / GamePad).
    4. Arguments: r3 is the alarm (r4 = 0), the app callback gets r3 = 0, the final-mix callbacks the device's FinalMix.
    5. The Cpu persists across firings, as on the port's service threads: r5 = 0 in the first two alarm firings, 0x28 in the third
       (the second firing's capture loop left it; speaker 0's state is only 4 once the row's poke applied).
   Not observable on a guest stack: the restore of lr and ctr (no guest code on a service thread runs between two calls).
B. Multi-frame AX under the scheduler, as guest code:
    1. rows ok at n 30, seeds 1 and 7, plain and -nulltrap, and under -threadsched 9.
    2. Exact sequence on both sides of every frame, callback, alarm firing and clock jump: frame k at floor(k * 3 * 62156250 / 1000)
       (3 ms at the 62.15625 MHz timebase), app -> TV -> GamePad in each frame, alarm period 0x652A7 from the guest's own
       OSSetPeriodicAlarm, alarm thread T3 on core 1, AX thread T1 on core 0. Frame 0 has no app callback: its registrar, the frame
       driver thread, runs when frame 0's callbacks are done.
    3. Same schedule digest on both sides and in two runs; the same non-empty net effect on both sides.
C. Mutants in code reached only through a callback, killed through that callback on the committed specs (`--no-stopfirst`: every row
   of the mutant is reported): the row of the function itself stays ok, row 02760C3C fails.
    1. AX: DeviceMixer::accumulate 0281A540 `destination[sample] = ...` dropped; the original's trace shows every downmix call of its
       loop on the AX thread T1, none in accumulate's own row.
    2. Alarm: RemoteSpeakerManager::capture 028A2194 remoteGetSamples dropped and literal 4->5 in `mState==4`; the original's
       trace shows C0006EF8 AXRmtGetSamples only on the alarm thread inside row 02760C3C, none in capture's own row.
D. mutate.py's scope on `threads` specs (a unit function's candidate code runs in any row there, so a per-mutant scope leaves false
   survivors):
    1. carrier_alarm #12 and #18 under the default command (no --whole-unit, stop-first): killed in row 02760C3C through the alarm
       thread; the receipts say the whole campaign scope ran and why. On the commit before the fix both survive in their own row.
    2. Non-threads spec lib2_agl_6: mutants keep their per-mutant scope, no threads note.
E. vcache on `threads` specs (keys used to cover only the closure of the row's own entry thunk, so an edit in code reached through a
   callback kept a stale ok row): a copy of carrier.txt whose JASDeviceMixer.cpp is a copy; filled, cached twice, then the copy's
   consumeTv `processFinalMix(0, mix)` -> `(1, mix)`: no row is cached, row 02760C3C fails (on the parent commit it stays cached
   and ok); restored: cached and ok again. The stderr names the reason.
F. The guest-thread call log's argument rule (gthr::compare uses the main comparison's call_mask: the callee's argument mask plus
   the candidate's declared argument count; the mask alone missed the arguments of declared imports): carrier_alarm with a copy of
   RemoteSpeaker.cpp, capture's `remoteGetSamples(..., 40)` -> 41 and `remoteAdvanceSamples(40)` -> 41, tested on row 02760C3C
   only: each fails there on the argument (`C0006EF8: r5`, `C0006EF0: r3`, 28 vs 29); the unedited copy passes. Both rows stayed ok
   on the parent commit.
G. Candidate code in threads starts at the tested call (gthr::unit_fn): during setups, postsetups and their drain both sides run the
   original. carrier_alarm with a copy of RemoteSpeaker.cpp, capture's `remoteSamplesLeft()<40` -> `<=40`, row 02760C3C only:
    1. sleep 1,000,000 ticks: the clock-0 firing in the drain is identical on both sides, the first difference is in the firing after
       the tested call started (`guest thread T1 ... call #1: original calls C0006EF8, candidate C0006F00`: the log starts at the tested call).
    2. sleep 0: the only firing is the one in the drain, so the row stays ok: no credit for it.
    3. the unedited copy, sleep 0, threadlog=1: the drain firing's two calls are not logged, so the cap does not fire.
R. The carriers pin threadrate=0 for the exact ticks of A and B2, so C, D1, E, F and G are repeated on copies at the default progress clock
   (generated units _axframes_test/_dr, _dr_alarm; removed at the end): the same mutants die through their callbacks in row 02760C3C.
Sections: `axframes_test.py [LETTERS]`, default all. Scratch files go to WWHD_VERIFY_SCRATCH. Exit 0 when all pass."""
import glob
import json
import math
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UDIR = os.path.join(HERE, "units", "_axframes_test")
SLEEP = "02760C3C"
ACCUM = "0281A540"
CAPTURE = "028A2194"
SLEEP_TICKS = 1000000
FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def run(exe, spec, args, n="1", seed="1", only=SLEEP):
    r = subprocess.run([exe, "-spec", spec, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", only] + args, cwd=ROOT,
                       capture_output=True, text=True, errors="replace", timeout=1800)
    return r.returncode, r.stdout, r.stderr


def row(out, func=SLEEP):
    return next((l for l in out.splitlines() if l.startswith(func)), "")


def watch_words(err, lo):
    """{(kind, side, word address): [words]} of the two stack words at lo, lo + 4, on the traced input (gen seed 1000003)"""
    bytes_ = {}
    for l in err.splitlines():
        p = l.split()
        if p[:1] != ["watch"] or p[-1] != "seed=1000003":
            continue
        a = int(p[2], 16)
        if lo <= a < lo + 8:
            bytes_.setdefault((p[1], p[4][5:], a & ~3), []).append(p[3])
    return {k: ["".join(v[i:i + 4]) for i in range(0, len(v), 4)] for k, v in bytes_.items()}


def frame_checks(exe, spec, tag, stack_lo, stack_len, old_r1, firings, callback_stwu=None):
    frame = old_r1 - 0x40
    rc, out, err = run(exe, spec, ["-watch", "%08X:0x%X" % (stack_lo, stack_len)])
    check(rc == 0 and row(out).endswith(" ok"), "A %s: row ok under -watch" % tag)
    w = watch_words(err, frame)
    for side in ("orig", "cand"):
        chain = w.get(("P", side, frame), [])
        check(chain == ["%08X" % old_r1] * firings, "A1/A3 %s %s: %d firings store back chain %08X at the new r1 %08X: %s" %
              (tag, side, firings, old_r1, frame, sorted(set(chain))))
        lr = w.get(("W", side, frame + 4), [])
        check(lr == ["00000000"] * firings, "A2 %s %s: LR save at r1+4 stores 0 in every firing: %s" % (tag, side, sorted(set(lr))))
    if callback_stwu:
        w = watch_words(err, callback_stwu)
        for side in ("orig", "cand"):
            s = w.get(("W", side, callback_stwu), [])
            check(s == ["%08X" % frame] * firings, "A2 %s %s: the callback's stwu stores its entry r1 %08X: %s" %
                  (tag, side, frame, sorted(set(s))))


def expected_events():
    period = 0x652A7
    frames = [k * 3 * 62156250 // 1000 for k in range(math.ceil(SLEEP_TICKS * 1000 / (3 * 62156250)))]
    alarms = list(range(0, SLEEP_TICKS, period))
    ev = []
    clock = 0
    for t in sorted(set(frames) | set(alarms)):
        if t != clock:
            ev.append("clock %d -> %d" % (clock, t))
            clock = t
        if t in frames:
            k = frames.index(t)
            ev.append("frame %d %d" % (k, t))
            ev += (["cb 02898F64 00000000 T1@c0"] if k else []) + ["cb 0281A738 6FF07FE0 T1@c0", "cb 0281A81C 6FF09220 T1@c0"]
        if t in alarms:
            ev.append("alarm 104BBEA0 T3@c1 028A2194 %d" % t)
    ev.append("clock %d -> %d" % (clock, SLEEP_TICKS))
    return ev


def trace_events(text):
    ev = []
    for l in text.splitlines():
        l = l.strip()
        m = re.match(r"thread ax frame (\d+) on T1@c0 \(clock (\d+)\)", l)
        if m:
            ev.append("frame %s %s" % m.groups())
        m = re.match(r"thread ax callback (\w+) \(r3=(\w+)\) on (\S+)", l)
        if m:
            ev.append("cb %s %s %s" % m.groups())
        m = re.match(r"thread alarm (\w+) fires on (\S+) \(callback (\w+), clock (\d+)\)", l)
        if m:
            ev.append("alarm %s %s %s %s" % m.groups())
        m = re.match(r"thread clock (\d+) -> (\d+)", l)
        if m:
            ev.append("clock %s -> %s" % m.groups())
    return ev


def sides(out):
    """(original side's trace, candidate side's trace)"""
    a, _, b = out.partition("net effect (original side)")
    return a, b


def mutate(unit, func, ids, rec, extra=("--no-stopfirst",), n="20", mx="40"):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), unit, "--func", func, "--n", n, "--max", mx] +
                       (["--ids", ids] if ids else []) + ["--receipts", rec] + list(extra),
                       cwd=ROOT, capture_output=True, text=True, errors="replace")
    out = {}
    for f in glob.glob(os.path.join(rec, "mutant*.json")):
        j = json.load(open(f))
        out[str(j["id"])] = j
    return r, out


def killed_only_through_callback(j, func, own, want_text, tag):
    rows = {l[:8]: l for l in j.get("stdout", "").splitlines() if l[:2] == "02"}
    check(want_text in j["mutation"], "%s mutant is the pinned one: %s" % (tag, j["mutation"]))
    check(j["class"] == "difference", "%s class difference (%s)" % (tag, j["class"]))
    check(rows.get(own, "").rstrip().endswith(" ok"), "%s own row %s stays ok: %s" % (tag, own, rows.get(own, "")[:70]))
    s = rows.get(func, "")
    check(s and not s.rstrip().endswith(" ok"), "%s row %s fails: %s" % (tag, func, s[50:170]))
    return s


def default_rate(text):
    """a carrier's text at the default progress clock (the carriers pin threadrate=0)"""
    assert " threadrate=0\n" in text
    return text.replace(" threadrate=0\n", "\n")


def section_c(ax_unit, al_unit, W, tag=""):
    r, ms = mutate(ax_unit, ACCUM, "12,34", os.path.join(W, "ax" + tag))
    check(set(ms) == {"12", "34"}, "C1%s receipts for the two AX mutants: %s (rc %d)" % (tag, sorted(ms), r.returncode))
    if "34" in ms:
        killed_only_through_callback(ms["34"], SLEEP, ACCUM, "destination[sample]", "C1%s AX #34" % tag)
    if "12" in ms:
        killed_only_through_callback(ms["12"], SLEEP, ACCUM, "literal 0->1", "C1%s AX #12" % tag)
    r, ms = mutate(al_unit, CAPTURE, "12,18", os.path.join(W, "alarm" + tag), ["--no-stopfirst", "--whole-unit"])
    check(set(ms) == {"12", "18"}, "C2%s receipts for the two alarm mutants: %s (rc %d)" % (tag, sorted(ms), r.returncode))
    if "12" in ms:
        s = killed_only_through_callback(ms["12"], SLEEP, CAPTURE, "drop statement `remoteGetSamples", "C2%s alarm #12" % tag)
        check("guest thread T1" in s, "C2%s alarm #12 dies in the alarm thread's call log" % tag)
    if "18" in ms:
        s = killed_only_through_callback(ms["18"], SLEEP, CAPTURE, "literal 4->5", "C2%s alarm #18" % tag)
        check("guest thread T1" in s, "C2%s alarm #18 dies in the alarm thread's call log" % tag)


def section_abc(W):
    ax_spec = os.path.join(UDIR, "carrier.txt")
    al_spec = os.path.join(UDIR, "carrier_alarm.txt")
    exe = verify.build("_axframes_test/carrier", jobs=4)
    exe_al = verify.build("_axframes_test/carrier_alarm", jobs=4)

    # A: the alarm stack: 3 firings in the row; capture's stwu -0x68 from the frame stores at 6FFFEF38
    frame_checks(exe_al, al_spec, "alarm stack", 0x6FFFEF30, 0xC0, 0x6FFFEFE0, 3, callback_stwu=0x6FFFEF38)
    # the AX stack: frame 0 has 2 callbacks, frames 1..5 have 3
    frame_checks(exe, ax_spec, "AX stack", 0x6FE3FF00, 0x100, 0x6FE3FFE0, 17)

    rc, out, err = run(exe_al, al_spec, ["-trace", "1000003"])
    o, c = sides(out)
    for side, t in (("original", o), ("candidate", c)):
        fir = re.findall(r"thread alarm 104BBEA0 fires on T1@c1 \(callback 028A2194, clock (\d+)\)\n\s+nested call 028A2138 "
                         r"r3=(\w+) r4=(\w+) r5=(\w+)", t)
        check([f[0] for f in fir] == ["0", "414375", "828750"], "A4 %s: alarm firings at %s" % (side, [f[0] for f in fir]))
        check(all(f[1] == "104BBEA0" and f[2] == "00000000" for f in fir), "A4 %s: r3 = the alarm, r4 = 0 on every firing" % side)
        check([f[3] for f in fir] == ["00000000", "00000000", "00000028"], "A5 %s: r5 persists across firings: %s" % (side, [f[3] for f in fir]))
        # B/C evidence for the alarm carrier: the capture loop's import runs only on the alarm thread of this row
        check(re.search(r"call C0006EF8", t) is not None, "C2 %s: AXRmtGetSamples runs inside row %s" % (side, SLEEP))
    rc, own, err = run(exe_al, al_spec, ["-trace", "1000003"], only=CAPTURE)
    check("C0006EF8" not in own, "C2 capture's own row never reaches AXRmtGetSamples")

    # B
    for seed in ("1", "7"):
        for extra in ([], ["-nulltrap"]):
            rc, out, err = run(exe, ax_spec, extra, n="30", seed=seed)
            r = row(out)
            check(rc == 0 and r.endswith(" ok") and "30/30" in r, "B1 seed %s %s: %s" % (seed, " ".join(extra) or "plain", r or err[-300:]))
    # the carriers pin threadrate=0 (the exact ticks below); at the default progress clock their rows stay ok as well
    for name, sp_, ex_ in (("carrier", ax_spec, exe), ("carrier_alarm", al_spec, exe_al)):
        p = os.path.join(W, name + "_rate64.txt")
        open(p, "w").write(open(sp_).read().replace(" threadrate=0\n", "\n"))
        for seed in ("1", "7"):
            rc, out, err = run(ex_, p, [], n="30", seed=seed)
            r = row(out)
            check(rc == 0 and r.endswith(" ok") and "30/30" in r, "B1 %s default threadrate seed %s: %s" % (name, seed, r or err[-300:]))
    rc, out, err = run(exe, ax_spec, ["-threadsched", "9"], n="30")
    check(rc == 0 and row(out).endswith(" ok"), "B1 -threadsched 9: %s" % row(out))
    rc, out, err = run(exe, ax_spec, ["-trace", "1000003"])
    o, c = sides(out)
    want = expected_events()
    for side, t in (("original", o), ("candidate", c)):
        got = trace_events(t)
        check(got == want, "B2 %s: %d events exact (6 frames, 3 alarms)%s" % (side, len(want), "" if got == want else ": %s" % got[:12]))
    dig = re.findall(r"threads \((original|candidate) side\): schedule digest ([0-9A-F]+)", out)
    check(len(dig) == 2 and dig[0][1] == dig[1][1], "B3 both sides have the same schedule digest %s" % dig)
    rc2, out2, err2 = run(exe, ax_spec, ["-trace", "1000003"])
    dig2 = re.findall(r"schedule digest ([0-9A-F]+)", out2)
    check(dig2 == [d[1] for d in dig], "B3 the same digest in a second run")
    net = re.findall(r"net effect \((?:original|candidate) side\): (\d+) bytes", out)
    check(len(net) == 2 and net[0] == net[1] and int(net[0]) > 0, "B3 the same non-empty net effect on both sides: %s" % net)
    fix = [l for l in o.splitlines() if "0281A310" in l and "nested return" in l]
    check(fix and all("(unit function in guest thread T1)" in l for l in fix), "C1 every downmix of the row runs on the AX thread T1 (%d)" % len(fix))
    rc, own, err = run(exe, ax_spec, ["-trace", "1000003"], only=ACCUM)
    check("0281A310" not in own, "C1 accumulate's own row never reaches its sample loop")

    # C
    section_c("_axframes_test/carrier", "_axframes_test/carrier_alarm", W)


def section_d(W, al_unit="_axframes_test/carrier_alarm", part2=True):
    r, ms = mutate(al_unit, CAPTURE, "12,18", os.path.join(W, "default" + al_unit[-3:]), extra=())
    for i, text in (("12", "drop statement `remoteGetSamples"), ("18", "literal 4->5")):
        j = ms.get(i)
        check(j is not None and text in j["mutation"], "D1 default command: receipt of #%s" % i)
        if not j:
            continue
        s = next((l for l in j.get("stdout", "").splitlines() if l.startswith(SLEEP)), "")
        check(j["class"] == "difference" and "guest thread T1" in s, "D1 default command: #%s %s, killed in row %s: %s" % (i, j["class"], SLEEP, s[50:150]))
        check(j.get("scope") == "whole" and "threads" in (j.get("scope_reason") or ""),
              "D1 #%s receipt: scope %s, scope_reason %s" % (i, j.get("scope"), j.get("scope_reason")))
    if not part2:
        return
    r, ms = mutate("lib2_agl_6", "0278BBA8", "", os.path.join(W, "nonthreads"), extra=(), n="5", mx="4")
    check(bool(ms) and "osmem ... threads" not in r.stderr, "D2 non-threads spec: %d mutant(s), no threads note" % len(ms))
    narrowed = [j for j in ms.values() if j.get("scope") != "whole"]
    check(bool(narrowed) and not any("threads" in (j.get("scope_reason") or "") for j in ms.values()),
          "D2 non-threads spec keeps per-mutant scopes: %s" % sorted({str(j.get("scope")) for j in ms.values()}))


def section_e(W, rate=lambda t: t):
    src = "wwhd_src/JSystem/JAudio/JASDeviceMixer.cpp"
    copy = "wwhd_src/JSystem/JAudio/_axframes_test_JASDeviceMixer.cpp"
    spec = os.path.join(UDIR, "_vc.txt")
    text = open(os.path.join(ROOT, src)).read()
    edit = ("    processFinalMix(0, mix);", "    processFinalMix(1, mix);")
    check(text.count(edit[0]) == 1, "E edit site found once")
    env = dict(os.environ, WWHD_VERIFY_SCRATCH=tempfile.mkdtemp(prefix="vc-", dir=W))
    env.pop("WWHD_VERIFY_CACHE", None)
    env.pop("WWHD_VERIFY_ARCHIVE", None)

    def v():
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_axframes_test/_vc", "-n", "5", "-noprogress", "-buildj", "4"],
                           cwd=ROOT, capture_output=True, text=True, errors="replace", env=env)
        return r.returncode, row(r.stdout), set(re.findall(r"vcache: ([0-9A-F]{8}) cached", r.stderr)), r.stderr
    try:
        open(spec, "w").write(rate(open(os.path.join(UDIR, "carrier.txt")).read()).replace(" " + src, " " + copy))
        open(os.path.join(ROOT, copy), "w").write(text)
        rc0, r0, c0, e0 = v()
        rc1, r1, c1, e1 = v()
        check(rc0 == 0 and r0.endswith(" ok") and not c0, "E fill: row ok, nothing cached: %s" % r0)
        check(rc1 == 0 and r1 == r0 and SLEEP in c1, "E second run: row %s cached" % SLEEP)
        check("keys cover all unit sources" in e1, "E stderr names the threads reason")
        open(os.path.join(ROOT, copy), "w").write(text.replace(*edit))
        rc2, r2, c2, e2 = v()
        check(not c2, "E edited consumeTv: no row cached (%s)" % sorted(c2))
        check(rc2 == 1 and r2 and not r2.endswith(" ok"), "E edited consumeTv: row %s fails: %s" % (SLEEP, r2[50:150]))
        open(os.path.join(ROOT, copy), "w").write(text)
        rc3, r3, c3, e3 = v()
        check(rc3 == 0 and r3 == r0 and SLEEP in c3, "E restored: cached and ok again")
    finally:
        for p in (spec, os.path.join(ROOT, copy)):
            if os.path.exists(p):
                os.remove(p)


def section_f(W, rate=lambda t: t, exact=True):
    src = "wwhd_src/nw/snd/RemoteSpeaker.cpp"
    copy = "wwhd_src/nw/snd/_axframes_test_RemoteSpeaker.cpp"
    spec = os.path.join(UDIR, "_cl.txt")
    text = open(os.path.join(ROOT, src)).read()
    cases = [("unedited", None, None, None),
             ("remoteGetSamples 41", "remoteGetSamples(channel,samples.get(),40);", "remoteGetSamples(channel,samples.get(),41);",
              "call #1 C0006EF8: r5 original 00000028 candidate 00000029"),
             ("remoteAdvanceSamples 41", "remoteAdvanceSamples(40);", "remoteAdvanceSamples(41);",
              "call #2 C0006EF0: r3 original 00000028 candidate 00000029")]  # the log starts at the tested call: #0 is the first firing's C0006F00, #1 its C0006EF8
    env = dict(os.environ, WWHD_VERIFY_CACHE="0")
    try:
        open(spec, "w").write(rate(open(os.path.join(UDIR, "carrier_alarm.txt")).read()).replace(" " + src, " " + copy))
        for name, a, b, want in cases:
            if a:
                check(text.count(a) == 1, "F %s: edit site found once" % name)
            open(os.path.join(ROOT, copy), "w").write(text.replace(a, b) if a else text)
            r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_axframes_test/_cl", "--no-header-args", "-strictnan",
                                "-only", SLEEP, "-n", "20", "-noprogress", "-buildj", "4"], cwd=ROOT, capture_output=True, text=True,
                               errors="replace", env=env)
            rw = row(r.stdout)
            if want and not exact:  # the default clock has other firings, so another call number
                want = want[want.index("C000"):]
            if want:
                check(r.returncode == 1 and "guest thread T1" in rw and want in rw, "F %s: row %s fails on the argument: %s" % (name, SLEEP, rw[50:200]))
            else:
                check(r.returncode == 0 and rw.endswith(" ok"), "F %s: row %s ok: %s" % (name, SLEEP, rw))
    finally:
        for p in (spec, os.path.join(ROOT, copy)):
            if os.path.exists(p):
                os.remove(p)


def section_g(W, rate=lambda t: t, tag=""):
    src = "wwhd_src/nw/snd/RemoteSpeaker.cpp"
    copy = "wwhd_src/nw/snd/_axframes_test_RemoteSpeaker.cpp"
    spec = os.path.join(UDIR, "_ug.txt")
    text = open(os.path.join(ROOT, src)).read()
    a, b = "if (remoteSamplesLeft()<40) return;", "if (remoteSamplesLeft()<=40) return;"
    check(text.count(a) == 1, "G edit site found once")
    base = rate(open(os.path.join(UDIR, "carrier_alarm.txt")).read()).replace(" " + src, " " + copy)
    ticks = "field * 5D820004 u32 1000000"
    check(ticks in base, "G the carrier's sleep line")
    env = dict(os.environ, WWHD_VERIFY_CACHE="0")

    def v():
        r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_axframes_test/_ug", "--no-header-args", "-strictnan", "-only", SLEEP,
                            "-n", "20", "-noprogress", "-buildj", "8"], cwd=ROOT, capture_output=True, text=True, errors="replace", env=env)
        return r.returncode, row(r.stdout)
    try:
        open(os.path.join(ROOT, copy), "w").write(text.replace(a, b))
        open(spec, "w").write(base)
        rc, r = v()
        check(rc == 1 and "guest thread T1" in r and (tag or "call #1: original calls C0006EF8, candidate C0006F00" in r),
              "G1%s killed in the first firing after the tested call started: %s" % (tag, r[50:190]))
        open(spec, "w").write(base.replace(ticks, "field * 5D820004 u32 0"))
        rc, r = v()
        check(rc == 0 and r.endswith(" ok"), "G2%s a firing only in the setups' drain gets no credit: %s" % (tag, r[50:190]))
        # the log starts at the tested call: the drain firing's calls do not count against threadlog=1
        open(os.path.join(ROOT, copy), "w").write(text)
        open(spec, "w").write(base.replace(ticks, "field * 5D820004 u32 0").replace("osmem hle threads", "osmem hle threads threadlog=1"))
        rc, r = v()
        check(rc == 0 and r.endswith(" ok"), "G3%s the setup-phase calls are not logged (threadlog=1 holds): %s" % (tag, r[50:190]))
    finally:
        for p in (spec, os.path.join(ROOT, copy)):
            if os.path.exists(p):
                os.remove(p)


def section_r(W):
    """C, D1, E, F at the default progress clock"""
    made = []
    try:
        for name, src in (("_dr", "carrier.txt"), ("_dr_alarm", "carrier_alarm.txt")):
            p = os.path.join(UDIR, name + ".txt")
            open(p, "w").write(default_rate(open(os.path.join(UDIR, src)).read()))
            made.append(p)
        section_c("_axframes_test/_dr", "_axframes_test/_dr_alarm", W, " default clock")
        section_d(W, "_axframes_test/_dr_alarm", part2=False)
    finally:
        for p in made:
            if os.path.exists(p):
                os.remove(p)
    section_e(W, default_rate)
    section_f(W, default_rate, exact=False)
    section_g(W, default_rate, " default clock")


def main():
    which = sys.argv[1] if len(sys.argv) > 1 else "ABCDEFGR"
    W = tempfile.mkdtemp(prefix="axframes_test-", dir=verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify"))
    print("axframes_test: receipts in %s (not removed)" % W)
    if set("ABC") & set(which):
        section_abc(W)
    if "D" in which:
        section_d(W)
    if "E" in which:
        section_e(W)
    if "F" in which:
        section_f(W)
    if "G" in which:
        section_g(W)
    if "R" in which:
        section_r(W)
    print("axframes_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
