#!/usr/bin/env python3
"""Self-test: mutate.py --fork keeps its open file descriptors bounded (2026-10-27, NW-SND#6).

A long --reach-skip --schemata --fork campaign stopped after 1767 of 1979 mutants with "filedescriptor out of range
in select()". Cause: every worker thread kept one fork server, i.e. two pipe fds, for each schema executable for the
whole campaign, and ForkServer.close() never closed the stdout pipe. A stand-in fork server (a small Python script
that speaks the harness's -forkserver protocol: READY, then PID/DONE per RUN line, writing OUT.out/OUT.err) checks:
1. 300 start/run/close cycles of ForkServer: the process's open fds are the same before and after;
2. a server whose pipes are above fd 1024 still runs (poll, not select);
3. synthetic campaigns through ServerPool (6 threads, 40 schema executables, 600 mutants, 4 servers per thread), in random
   order and grouped by executable as mutate.py submits them: at most 6 x 4 servers are open at any time, the open fds
   stay below a fixed bound, once every mutant is done no server and no extra fd is left, and grouped, no more servers
   start than before the fix (threads x executables).
Exit 0 when all pass. Temporary files go to a temporary directory, removed at exit.
"""
import os
import random
import resource
import shutil
import sys
import tempfile
import threading

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mutate  # noqa: E402

FAKE = r'''
import os, sys
sys.stdout.write("READY\n"); sys.stdout.flush()
for line in sys.stdin:
    f = line.split()
    if len(f) != 4 or f[0] != "RUN":
        sys.stdout.write("ERR " + line); sys.stdout.flush(); continue
    open(f[2] + ".out", "w").write("run %s\n" % f[1])
    open(f[2] + ".err", "w").write("")
    sys.stdout.write("PID %d\nDONE 0\n" % os.getpid()); sys.stdout.flush()
'''


def nfds():
    return len(os.listdir("/dev/fd"))


def main():
    tmp = tempfile.mkdtemp(prefix="wwhd-mutate_forkfd_test-")
    fails = []
    try:
        script = os.path.join(tmp, "fake_forkserver.py")
        open(script, "w").write(FAKE)
        cmd = [sys.executable, "-I", script]

        # 1. start/run/close cycles leave no fd behind
        before = nfds()
        for k in range(300):
            s = mutate.ForkServer(cmd, tmp)
            r = s.run(k, None, 30)
            if not r or r[0] != 0 or r[1] != "run %d\n" % k:
                fails.append("cycle %d: run returned %r" % (k, r))
                break
            s.close()
        after = nfds()
        print("1. 300 fork server cycles: open fds %d before, %d after" % (before, after))
        if after != before:
            fails.append("fd leak: %d fds before, %d after 300 cycles" % (before, after))

        # 2. pipes above fd 1024
        soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
        want = 2048 if hard == resource.RLIM_INFINITY else min(hard, 2048)
        if soft < want:
            resource.setrlimit(resource.RLIMIT_NOFILE, (want, hard))
        pad = []
        try:
            while len(pad) < 1100:
                pad.append(os.open(os.devnull, os.O_RDONLY))
            s = mutate.ForkServer(cmd, tmp)
            fd = s.p.stdout.fileno()
            try:
                r = s.run(1, None, 30)
                ok = bool(r) and r[0] == 0
            except ValueError as e:
                ok, r = False, e
            s.close()
            print("2. fork server with its stdout pipe at fd %d: %s" % (fd, "runs" if ok else "FAILED (%r)" % (r,)))
            if fd <= 1024 or not ok:
                fails.append("high fd: pipe at %d, result %r" % (fd, r))
        finally:
            for f in pad:
                os.close(f)

        # 3. synthetic campaigns through ServerPool: mutants in random order (worst case) and grouped (mutate.py's order)
        for grouped in (False, True):
            campaign(cmd, tmp, grouped, fails)
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    for x in fails:
        print("FAIL", x)
    print("mutate_forkfd_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


def campaign(cmd, tmp, grouped, fails):
    if True:
        threads, exes, per_exe, per_thread = 6, 40, 15, 4
        live = [0]
        peak = [0, 0]
        lk = threading.Lock()

        class Counted(mutate.ForkServer):
            def __init__(self, c, d):
                super().__init__(c, d)
                with lk:
                    live[0] += 1
                    peak[0] = max(peak[0], live[0])

            def close(self):
                if not self.closed:
                    with lk:
                        live[0] -= 1
                super().close()

        jobs = [(i, "exe%d" % (i % exes)) for i in range(exes * per_exe)]
        random.Random(5).shuffle(jobs)
        if grouped:  # mutate.py --fork submits each executable's mutants together
            jobs = [jobs[k] for k in mutate.grouped_order([e for _, e in jobs])]
        pool = mutate.ServerPool(lambda exe: Counted(cmd + [exe], tmp), per_thread=per_thread)
        cnt = {}
        for _, e in jobs:
            cnt[e] = cnt.get(e, 0) + 1
        pool.set_remaining(cnt)
        base = nfds()
        it = iter(jobs)
        itl = threading.Lock()
        errs = []

        def worker():
            while True:
                with itl:
                    j = next(it, None)
                if j is None:
                    return
                i, e = j
                try:
                    r = pool.get(e).run(i, None, 30)
                    if not r or r[1] != "run %d\n" % i:
                        errs.append((i, r))
                except Exception as x:  # noqa: BLE001
                    errs.append((i, x))
                finally:
                    pool.done(e)
                with lk:
                    peak[1] = max(peak[1], nfds() - base)

        ts = [threading.Thread(target=worker) for _ in range(threads)]
        for t in ts:
            t.start()
        for t in ts:
            t.join()
        end = nfds()
        print("3. %s campaign: %d mutants, %d threads, %d executables: %d servers started, at most %d open at once "
              "(bound %d), at most %d extra fds, %d open / %d extra fds at the end, %d errors"
              % ("grouped" if grouped else "random-order", len(jobs), threads, exes, pool.opened, peak[0], threads * per_thread, peak[1], live[0], end - base, len(errs)))
        if errs:
            fails.append("campaign errors: %r" % errs[:3])
        if peak[0] > threads * per_thread:
            fails.append("campaign: %d servers open at once (bound %d)" % (peak[0], threads * per_thread))
        if peak[1] > 2 * threads * per_thread + 3 * threads + 16:
            fails.append("campaign: %d extra fds open at once" % peak[1])
        if live[0] or pool.open_count() or end != base:
            fails.append("campaign: %d servers / %d extra fds left after the last mutant" % (live[0], end - base))
        pool.close_all()
        if grouped and pool.opened > threads * exes:
            fails.append("grouped campaign: %d server starts (old behaviour: at most %d)" % (pool.opened, threads * exes))


if __name__ == "__main__":
    sys.exit(main())
