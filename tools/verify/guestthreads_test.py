#!/usr/bin/env python3
"""Self-test of the `osmem ... threads` part (harness.cpp + src/gthr_sched.inc + src/gthr_model.h;
2026-11-01): the cooperative guest-thread model.

A. Scheduler and objects without guest code (harness option -gthrselftest NAME[:SEED], host bodies, no guest memory):
   exact event sequences for create/resume/join, blocking and non-blocking queues (capacity, high-priority send),
   sleep/yield order and the virtual clock, same-core priority preemption on resume and on wake (and none across cores),
   OSGetCoreId from every affinity mask (threads.cpp core_from_affinity, fallback = the creator's core, main = core 1),
   alarms (one-shot, periodic, cancelled; the callbacks on the priority -1 alarm service thread, the clock jumping to each
   firing), recursive mutex with contention, auto/manual events and a timeout, rendezvous (per-core arrivals as on
   Cafe OS and in the port since f5554821: a core-1 thread waiting with mask 1 waits for the core-0 thread), exit values, the drain, the slice
   preemption of a spin, and the aborts with receipts: deadlock, threadbudget (also a spin nobody can preempt), threadcap.
   The progress clock (threadrate, 2026-11-02): `clockpoll`, a same-core lower-priority worker that never blocks polls the
   time until an interval has passed while main sleeps in GX2WaitForVsync (J3D start-up T22): main wakes at its vsync tick
   and preempts the worker, the interval ends; `clockpoll0` (threadrate=0, the clock that only jumps) ends at threadbudget;
   `clockpollx` (worker on core 0) wakes main at the same tick; `clockpolleq` (equal priority, same core) only after the interval.
   `logcap`: a call log past threadlog entries aborts with a receipt naming the thread.
   Perturbation: the same threadsched seed gives the same schedule; different seeds give more than one schedule, all ending
   in the same result.
B. Original guest code (sead::Thread, wwhd_src/sead/Thread.cpp; unit tools/verify/units/_guestthreads_test/sead.txt):
   setups build a ThreadManager, the main-thread wrapper and a sead::Thread and start it (OSCreateThread/SetAffinity/
   ResumeThread; the drain after the setups runs it until it blocks in OSReceiveMessage); the tested Thread::quitAndJoin
   0275FF7C sends the quit message and joins (main blocks, the thread receives the message, leaves its loop, exits).
    1. rows ok at n 30 seeds 1 and 7, also with -nulltrap and -threadsched 9; the trace shows the creation, the switches,
       the thread's exit, and on the candidate side the thread running the unit's own Thread::run (candidate code).
    2. the schedule receipt (stderr `guestthreads: 0275FF7C: schedule digest ...`) is identical in two runs.
    3. candidate code in threads starts at the tested call. A mutant in Thread::run 0275FFCC (OSYieldThread after its loop), on the
       thread's stack since the setups' drain, survives row 0275FF7C, and the row names it (`threads: ...: unit function(s) on a thread
       stack at the tested call: ...`): the documented limit. A mutant in MessageQueue::pop 02760374, which the thread calls again
       after the message of the tested Thread::sendMessage 0275FEFC, is killed by the thread call log:
       `guest thread T1 (entry 02760ACC) ...`; it stays ok in row 0275FF7C, whose setups reach it too.
    4. join without a quit message (Thread::join 02760958): both sides deadlock: inputs aborted on both sides, the trace
       names the deadlock and the thread table.
    5. threadbudget=2: both sides abort with the budget receipt; threadlog=1: the call-log receipt names T1; threadrate=0
       and threadrate=1 keep the rows ok.
    6. setupsnapshot is refused with the guest-thread reason and the rows stay ok.
    7. -refcache-write stores no record for these inputs (the reader runs them live); rows equal.
    8. Off by default: without `threads`, OSCreateThread gets the generic answer (no thread is created).
    9. Spec errors exit 2: `threads` together with `ret! C0009BB0 1` (the model answers OSGetCoreId) or with
       mockthreadspecific; thread keys without `threads`; bad values.
Unit/spec files stay in tools/verify/units/_guestthreads_test/ (generated, not removed). Exit 0 when all pass."""
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UDIR = os.path.join(HERE, "units", "_guestthreads_test")
# Thread::sendMessage 0275FEFC with a message: sead::Thread's own message hook (vtable +0x7C) is pure virtual, so for this row only the
# object's vtable word (+0xC) points to a table whose hook is Thread::emptyHook 0275FFC8; the poke applies after the setups' drain
SEND_ARGS = ["arg! 0275FEFC r3 0x5D500000", "arg! 0275FEFC r4 5", "arg! 0275FEFC r5 0", "fill * 0x5D600100 0x80 0", "field * 5D60017C u32 0x0275FFC8",
             "stable 5D600100 0x80", "poke 0275FEFC 5D50000C u32 0x5D600100"]
FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


SELF = {
    "order": ["T0 receive", "T1 start", "T1 sent", "T2 start", "T0 got 1", "T2 after yield", "T1 woke clock 100", "T0 joined A value A"],
    "preempt": ["T0 resume C", "T1 start", "T0 send", "T1 got 7", "T0 after send"],
    "nopreempt": ["T0 resume C", "T0 send", "T0 after send"],
    "coreid": ["T0 mask 0 -> core 1", "T0 mask 1 -> core 0", "T0 mask 2 -> core 1", "T0 mask 4 -> core 2", "T0 mask 3 -> core 0",
               "T0 mask 5 -> core 0", "T0 mask 6 -> core 1", "T0 mask 7 -> core 1", "T9 core 2", "T9 mask 0 -> core 2",
               "T9 mask 3 -> core 0", "T9 mask 7 -> core 2", "T0 main core 1"],
    "mutex": ["T0 count 2", "T0 trylock by me 1", "T1 lock", "T0 after yield owner 0 count 2", "T0 unlocked", "T1 locked owner 1 count 1",
              "T0 joined"],
    "event": ["T1 wait 3000", "T2 wait 3000", "T3 timed wait", "T0 signal auto", "T1 woken 1", "T0 signaled 0", "T2 woken 1",
              "T0 manual signal", "T0 signaled 1", "T3 timed result 1 clock 10", "T0 joined all clock 10", "T4 timed wait",
              "T4 timed result 0 clock 60", "T0 timeout path joined clock 60"],
    "rendezvous": ["T0 arrive", "T1 arrive", "T1 passed", "T0 passed"],
    "rdvcore": ["T0 arrive mask 1", "T1 worker arrives", "T1 worker passed", "T0 passed"],
    "rdvsame": ["T1 arrives", "T2 arrives", "T0 arrive mask 1", "T0 passed", "T1 passed", "T2 passed", "T0 joined"],
    "queue": ["T1 try receive 0", "T1 try send 1", "T1 try send 0", "T1 high send blocks", "T0 receive 1", "T0 value 1", "T1 high sent",
              "T0 receive 1", "T0 value 3", "T0 joined"],
    "exit": ["T1 exit 0x55", "T0 value 55"],
    "drain": ["T0 drain", "T1 worker", "T0 drained clock 0"],
    "alarm": ["T1 wait", "T2 alarm 5000 fires clock 100", "T2 alarm 5100 fires clock 120", "T2 alarm 5000 fires clock 150",
              "T2 alarm 5000 fires clock 200", "T1 woken clock 200", "T0 joined clock 200"],
    "slice": ["T0 resume F later", "T1 spin", "T2 set flag", "T1 flag seen", "T0 joined E"],
    "stubs": ["T0 target 028FDE28 -> slot C0009BB0", "T0 target 028FDC00 -> slot 00000000", "T0 target 028FAE60 -> slot C0006AE8",
              "T0 target 028FAD60 -> slot 00000000"],
    "logstart": ["T1 before the tested call: 0 logged", "T1 after it: 2 logged", "T0 joined"],
    "slicecalls": ["T0 spin", "T1 lock", "T1 set flag", "T0 flag seen", "T0 joined"],
    # 64 ticks per time read, 1 per other OS call: the worker's call crossing vsync tick 1036973 wakes main, which preempts it
    # at once; then the worker's interval ends at its first read past 1500000
    "clockpoll": ["T0 vsync wait clock 0", "T1 poll from clock 0", "T0 woke clock 1036989",
                  "T1 interval passed clock 1500111 after 22727 polls", "T0 joined clock 1500111"],
    "clockpollx": ["T0 vsync wait clock 0", "T1 poll from clock 0", "T0 woke clock 1036989",
                   "T1 interval passed clock 1500111 after 22727 polls", "T0 joined clock 1500111"],
    "clockpolleq": ["T0 vsync wait clock 0", "T1 poll from clock 0", "T1 interval passed clock 1500110 after 22727 polls",
                    "T0 woke clock 1500111", "T0 joined clock 1500111"],
    "vsync": ["T0 frame 0 clock 0", "T1 piece 0 clock 0", "T1 piece 1 clock 0", "T1 piece 2 clock 0", "T0 frame 1 clock 1036974",
              "T0 frame 2 clock 2073947", "T0 after 3 vsyncs clock 3110921", "T0 joined"],  # vsync tick + 1 (a modelled call)
}
ABORTS = {"deadlock": "guest threads: deadlock (non-termination): every thread waits and none waits on the clock: T0 6FFE1000",
          "budget": "guest threads: scheduling budget exceeded (1000 scheduler events in one run; threadbudget)",
          "slicejoin": "guest threads: scheduling budget exceeded (5000 scheduler events in one run; threadbudget)",
          "cap": "guest threads: more than 4 threads in one run (threadcap)",
          "clockpoll0": "guest threads: scheduling budget exceeded (2000 scheduler events in one run; threadbudget): T0 6FFE1000 entry 028EA120 core 1 prio 16 OSSleepTicks until 1036973; T1 00000100 entry 00000000 core 1 prio 17 running",
          "logcap": "guest threads: the per-thread call logs exceed 100 entries in one run (threadlog): T1 (entry 0203DEEC) has 100 of them"}


def selftest(exe, name):
    r = subprocess.run([exe, "-gthrselftest", name], cwd=ROOT, capture_output=True, text=True, timeout=120)
    return r.returncode, r.stdout.splitlines()


def part_a(exe):
    for name, want in SELF.items():
        rc, out = selftest(exe, name)
        ev = [l for l in out if not l.startswith("digest ")]
        check(rc == 0 and ev == want and out and out[-1].startswith("digest "), "A %s: %s" % (name, ev if ev != want else "as expected"))
    rc, out = selftest(exe, "slice")
    check(any(" preempts 1 " in l for l in out), "A slice: one slice preemption")
    rc, out = selftest(exe, "slicecalls")
    check(any(" preempts 1 " in l for l in out), "A slicecalls: the worker gets the mutex at a slice preemption: %s" % out[-1:])
    rc, out = selftest(exe, "preempt")
    check(out[-1].endswith("preempts 0 clock 0"), "A preempt: priority preemption is not a slice preemption")
    for name, want in ABORTS.items():
        rc, out = selftest(exe, name)
        check(rc == 0 and out and out[-1].startswith("ABORT " + want), "A %s abort: %s" % (name, out[-1] if out else "no output"))
    seqs = {}
    for s in range(1, 17):
        a = selftest(exe, "order:%d" % s)[1]
        b = selftest(exe, "order:%d" % s)[1]
        check(a == b, "A perturbation: seed %d gives the same schedule twice" % s)
        ev = tuple(l for l in a if not l.startswith("digest "))
        seqs.setdefault(ev, []).append(s)
        check(ev and ev[-1] == "T0 joined A value A" and "T0 got 1" in ev, "A perturbation seed %d ends with the same result" % s)
    check(len(seqs) >= 2, "A perturbation: %d distinct schedules over seeds 1..16" % len(seqs))


def spec_lines(path):
    return [l for l in open(path).read().split("\n")]


def write_unit(name, lines):
    with open(os.path.join(UDIR, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")


def run(exe, spec, args, n="30", seed="1"):
    r = subprocess.run([exe, "-spec", spec, "-noprogress", "-strictnan", "-n", n, "-seed", seed] + args, cwd=ROOT,
                       capture_output=True, text=True, errors="replace", timeout=1800)
    return r.returncode, r.stdout, r.stderr


def row(out, addr):
    return next((l for l in out.splitlines() if l.startswith(addr)), "")


def ok_row(r):
    return r.endswith(" ok") and " 0/0  " not in r.split("0/0", 1)[0][:60]


def part_b():
    base = os.path.join(UDIR, "sead.txt")
    exe = verify.build("_guestthreads_test/sead", jobs=6)
    W = tempfile.mkdtemp(prefix="guestthreads_test-", dir=verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify"))
    print("guestthreads_test: specs in %s and %s (not removed)" % (W, UDIR))
    L = spec_lines(base)

    def spec(name, lines):
        p = os.path.join(W, name + ".txt")
        open(p, "w").write("\n".join(lines) + "\n")
        return p

    sp = spec("base", L)
    F = "0275FF7C"
    for seed in ("1", "7"):
        for extra in ([], ["-nulltrap"], ["-threadsched", "9"]):
            rc, out, err = run(exe, sp, ["-only", F] + extra, seed=seed)
            r = row(out, F)
            check(rc == 0 and r.endswith(" ok") and "30/30" in r, "B1 seed %s %s: %s" % (seed, " ".join(extra) or "plain", r or err[-300:]))
            if extra == ["-threadsched", "9"]:
                check("(threadsched 9," in err, "B1 seed %s: the receipt names threadsched 9" % seed)
    rc, out, err = run(exe, sp, ["-only", F, "-trace", "1000003"], n="1")
    for w in ["thread T1 created: OSThread", "entry 02760ACC", "thread drain T0@c1 -> T1@c1", "thread OSReceiveMessage T1@c1 -> T0@c1",
              "thread OSJoinThread T0@c1 -> T1@c1", "thread T1@c1 exited with 00000000", "(unit function in guest thread T1)",
              "threads (original side): schedule digest", "threads (candidate side): schedule digest"]:
        check(w in out, "B1 trace shows %r" % w)
    dig = re.findall(r"threads \((original|candidate) side\): schedule digest ([0-9A-F]+)", out)
    check(len(dig) == 2 and dig[0][1] == dig[1][1], "B1 trace: both sides have the same schedule digest %s" % dig)
    rc1, _, e1 = run(exe, sp, ["-only", F])
    rc2, _, e2 = run(exe, sp, ["-only", F])
    d1 = re.findall(r"guestthreads: 0275FF7C: schedule digest \S+.*", e1)
    d2 = re.findall(r"guestthreads: 0275FF7C: schedule digest \S+.*", e2)
    check(d1 and d1 == d2, "B2 schedule receipt identical in two runs: %s" % (d1[:1],))

    # 3. mutant in thread-reached code
    src = open(os.path.join(ROOT, "wwhd_src", "sead", "Thread.cpp")).read()
    old = """        message = mQueue.pop(mMessageMode);
    }
}"""
    assert src.count(old) == 1
    mut = src.replace(old, """        message = mQueue.pop(mMessageMode);
    }
    platformYieldThread(); /* guestthreads_test mutant */
}""")
    if "C000A248" not in mut:
        mut = mut.replace("ext C0009F38 s32 platformResumeThread", "ext C000A248 void platformYieldThread()\next C0009F38 s32 platformResumeThread")
    mpath = os.path.join(UDIR, "Thread_mut.cpp")
    open(mpath, "w").write(mut)
    rel = os.path.relpath(mpath, ROOT)
    write_unit("sead_mut", [l.replace("wwhd_src/sead/Thread.cpp", rel) for l in L])
    mexe = verify.build("_guestthreads_test/sead_mut", jobs=6)
    rc, out, err = run(mexe, sp, ["-only", F])
    r = row(out, F)
    check("guestthreads: threadsched=" not in err, "B1 default mode: no non-default header line")
    rc, out, err = run(exe, sp, ["-only", F, "-threadsched", "3"], n="1")
    check("guestthreads: threadsched=3: a perturbed schedule" in err, "B1 -threadsched prints the mode header line")
    # Thread::run entered during the setups' drain: its frame stays original to the end (candidate code starts at the tested call), so
    # this row cannot see its mutant, and the row says so
    check(r.endswith(" ok"), "B3 limit: a mutant in Thread::run, on the thread's stack at the tested call, survives in this row: %s" % r)
    note = next((l for l in err.splitlines() if l.startswith("threads: %s: unit function(s) on a thread stack at the tested call:" % F)), "")
    check(all(a in note for a in ("02760ACC", "0275FFCC", "02760374")), "B3 limit: the row names it: %s" % note)
    # a function the thread calls after the tested call starts runs as candidate: MessageQueue::pop, called again after the message
    # that Thread::sendMessage 0275FEFC sends; the setups' first pop is the original on both sides
    pop = "    u32 received = platformReceiveMessage(&mPlatformQueue, payload.get(), nonBlocking == 0);"
    check(src.count(pop) == 1, "B3 pop edit site found once")
    mut2 = src.replace(pop, "    platformYieldThread(); /* guestthreads_test mutant */\n" + pop)
    if "C000A248" not in mut2:
        mut2 = mut2.replace("ext C0009F38 s32 platformResumeThread", "ext C000A248 void platformYieldThread()\next C0009F38 s32 platformResumeThread")
    open(mpath, "w").write(mut2)
    write_unit("sead_mut2", [l.replace("wwhd_src/sead/Thread.cpp", rel) for l in L] + SEND_ARGS)
    m2exe = verify.build("_guestthreads_test/sead_mut2", jobs=6)
    write_unit("sead_send", L + SEND_ARGS)
    sexe = verify.build("_guestthreads_test/sead_send", jobs=6)
    SEND = "0275FEFC"
    rc, out, err = run(sexe, os.path.join(UDIR, "sead_send.txt"), ["-only", SEND])
    check(row(out, SEND).endswith(" ok"), "B3 the unmutated Thread::sendMessage row is ok: %s" % row(out, SEND))
    rc, out, err = run(m2exe, os.path.join(UDIR, "sead_mut2.txt"), ["-only", SEND])
    r2 = row(out, SEND)
    check("guest thread T1 (entry 02760ACC)" in r2 and not r2.endswith(" ok"), "B3 a mutant in what the thread calls after the tested call started is killed: %s" % r2)
    rc, out, err = run(m2exe, os.path.join(UDIR, "sead_mut2.txt"), ["-only", F])
    check(row(out, F).endswith(" ok"), "B3 the same mutant, whose only earlier call is in the setups' drain, makes no setup difference: %s" % row(out, F))

    # 4. join without quit: deadlock on both sides
    sp4 = spec("join", L + ["arg! 02760958 r3 0x5D500000"])
    rc, out, err = run(exe, sp4, ["-only", "02760958"], n="5")
    r = row(out, "02760958")
    check("aborted on both sides" in r, "B4 join without quit: %s" % r)
    rc, out, err = run(exe, sp4, ["-only", "02760958", "-trace", "1000003"], n="1")
    check("guest threads: deadlock (non-termination)" in out and "OSJoinThread" in out and "OSReceiveMessage" in out,
          "B4 trace names the deadlock and the waits")

    # 5. threadbudget
    sp5 = spec("budget", [l.replace("osmem hle threads mainthread=5D400000", "osmem hle threads mainthread=5D400000 threadbudget=2") for l in L])
    rc, out, err = run(exe, sp5, ["-only", F, "-trace", "1000003"], n="1")
    check("scheduling budget exceeded (2 scheduler events" in out, "B5 threadbudget=2 aborts with the receipt")
    # threadlog bounds what is compared, from the tested call on: Thread::quitAndJoin's thread makes no logged call after it (the setups'
    # calls do not count), the alarm thread of the carrier inside its sleep does
    sp5l = spec("logcap", [l.replace("osmem hle threads mainthread=5D400000", "osmem hle threads mainthread=5D400000 threadlog=1") for l in L])
    rc, out, err = run(exe, sp5l, ["-only", F])
    check(row(out, F).endswith(" ok"), "B5 threadlog=1: the setups' calls do not count: %s" % row(out, F))
    al = os.path.join(HERE, "units", "_axframes_test", "carrier_alarm.txt")
    alexe = verify.build("_axframes_test/carrier_alarm", jobs=6)
    sp5a = spec("logcap_alarm", [l.replace("osmem hle threads", "osmem hle threads threadlog=1") for l in open(al).read().split("\n")])
    rc, out, err = run(alexe, sp5a, ["-only", "02760C3C", "-trace", "1000003"], n="1")
    check("the per-thread call logs exceed 1 entries in one run (threadlog): T1 (entry 00000000)" in out, "B5 threadlog=1 aborts with the receipt naming T1")
    for rate in ("0", "1"):
        sp5r = spec("rate" + rate, [l.replace("osmem hle threads mainthread=5D400000", "osmem hle threads mainthread=5D400000 threadrate=" + rate) for l in L])
        rc, out, err = run(exe, sp5r, ["-only", F])
        check(rc == 0 and row(out, F).endswith(" ok") and "30/30" in row(out, F), "B5 threadrate=%s: %s" % (rate, row(out, F) or err[-300:]))
    # 6. setupsnapshot refused
    sp6 = spec("snap", L + ["setupsnapshot"])
    rc, out, err = run(exe, sp6, ["-only", F])
    check("setupsnapshot: refused (guest threads: the setups left guest-thread state" in err and row(out, F).endswith(" ok"),
          "B6 setupsnapshot refused, rows ok: %s" % row(out, F))
    # 7. refcache
    rd = tempfile.mkdtemp(prefix="refc-", dir=W)
    rc, out, err = run(exe, sp, ["-only", F, "-refcache-write", rd, "-refcache-key", "gt"])
    rc2, out2, err2 = run(exe, sp, ["-only", F, "-refcache", rd, "-refcache-key", "gt"])
    m = re.search(r"refcache: .*", err2)
    check(rc == 0 and rc2 == 0 and row(out, F) == row(out2, F) and m and ("live" in m.group(0)),
          "B7 refcache: rows equal, reader runs live: %s" % (m.group(0) if m else err2[-200:]))
    # 8. off by default
    sp8 = spec("off", [l.replace("osmem hle threads mainthread=5D400000", "osmem hle") for l in L])
    rc, out, err = run(exe, sp8, ["-only", F, "-trace", "1000003"], n="1")
    check("thread T1 created" not in out and "guestthreads:" not in err, "B8 without `threads` no thread is created")
    # 9. spec errors
    for name, lines in [("coreid", L + ["ret! C0009BB0 1"]), ("tls", L + ["mockthreadspecific C000A090 C0009CC8 5D400000"]),
                        ("keys", [l.replace("osmem hle threads mainthread=5D400000", "osmem hle threadsched=3") for l in L]),
                        ("badmain", [l.replace("mainthread=5D400000", "mainthread=5D400003") for l in L]),
                        ("badcap", [l.replace("mainthread=5D400000", "mainthread=5D400000 threadcap=1") for l in L]),
                        ("badrate", [l.replace("mainthread=5D400000", "mainthread=5D400000 threadrate=x") for l in L]),
                        ("badlog", [l.replace("mainthread=5D400000", "mainthread=5D400000 threadlog=0") for l in L]),
                        ("ratekey", [l.replace("osmem hle threads mainthread=5D400000", "osmem hle threadrate=8") for l in L]),
                        ("rdvmode", [l.replace("mainthread=5D400000", "mainthread=5D400000 threadrdv=port") for l in L])]:
        rc, out, err = run(exe, spec("err_" + name, lines), ["-only", F], n="1")
        check(rc == 2, "B9 %s exits 2: %s" % (name, err.strip().splitlines()[-1] if err.strip() else rc))


def main():
    exe = verify.build("_guestthreads_test/sead", jobs=6)
    part_a(exe)
    part_b()
    print("guestthreads_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 0 if not FAILS else 1


if __name__ == "__main__":
    sys.exit(main())
