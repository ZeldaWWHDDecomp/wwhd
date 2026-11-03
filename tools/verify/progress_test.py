#!/usr/bin/env python3
"""Self-test of the progress reporting (2026-10-27): harness progress/STUCK/HANG lines, the wall-clock hang limit,
verify.py pass-through (also -shards), mutate.py progress and tools/verify/progress.py.

1. byte-identical: verify.py on real merged specs (lib2_sead_1 and lib2_sead_15, -n 200, seeds 1 and 7, also -nulltrap and
   -shards 2) with progress at a 1 s interval and with -noprogress: stdout is identical, and stderr is identical once the
   `[progress] ` lines are removed (sharded runs: the same lines, as their order between parallel shards varies anyway); the progress runs printed progress lines; the exit codes agree.
2. hang demo: temporary specs (lib2_sead_15 with a mutant WorkerRuntimeInstances.cpp whose constructWorkerCounter
   02759928 never returns). Blocked (sleeps forever) with -stuck 1 -hanglimit 3: STUCK lines name the seed, a
   `[progress] HANG` line names function, seed and the candidate side, the row is a FAIL with "wall-clock limit exceeded"
   after ~3 s per input (the CPU time budget never sees a sleeping run). While it runs, progress.py PID and progress.py
   UNIT show the harness state. Computing (a two-instruction loop) with -hanglimit 3: the wall-clock limit leaves it
   alone and the 30 s CPU time budget ends it ("time budget exceeded"), with a HANG line.
3. mutate.py: a short campaign (lib2_sead_15 --func 02759928 --max 4 --n 50) with progress at 1 s and with --no-progress:
   same stdout, same receipts (apart from the measured timeout limits), mutant progress lines in the first.
Temporary files (unit, mutant source, receipts) are removed; builds go to a private WWHD_VERIFY_SCRATCH directory
(or below an existing WWHD_VERIFY_SCRATCH), removed at exit. Exit 0 when all cases pass.
"""
import atexit
import glob
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_progress_test")
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUT = "wwhd_src/sead/_progress_test_WorkerRuntimeInstances.cpp"  # blocked: sleeps forever
MUTSPIN = "wwhd_src/sead/_progress_test_spin_WorkerRuntimeInstances.cpp"  # computing: a two-instruction loop
FUNC = "02759928"
PREFIX = "[progress] "


def private_scratch():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    d = tempfile.mkdtemp(prefix="wwhd-progress_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    os.environ["WWHD_VERIFY_CACHE"] = "0"  # the on/off pairs compare stderr: no cached second run (vcache.py)
    atexit.register(shutil.rmtree, d, True)


def strip(err):
    return "".join(l for l in err.splitlines(True) if not l.startswith(PREFIX))


def verify(args, env=None):
    return subprocess.run([sys.executable, os.path.join(HERE, "verify.py")] + args, cwd=ROOT, capture_output=True, text=True,
                          env=env)


def identical(fails):
    cases = [["lib2_sead_1", "-n", "200", "-seed", "1"], ["lib2_sead_1", "-n", "200", "-seed", "7"],
             ["lib2_sead_1", "-n", "200", "-seed", "1", "-nulltrap"], ["lib2_sead_1", "-n", "200", "-seed", "7", "-shards", "2"],
             ["lib2_sead_15", "-n", "200", "-seed", "1"], ["lib2_sead_15", "-n", "200", "-seed", "7", "-nulltrap", "-shards", "2"]]
    verify(cases[0][:1] + ["-n", "1", "-buildj", "4", "-noprogress"])  # build once, outside the comparisons
    verify(cases[4][:1] + ["-n", "1", "-buildj", "4", "-noprogress"])
    seen = 0
    for c in cases:
        on = verify(c + ["-progress-interval", "1"])
        off = verify(c + ["-noprogress"])
        n = sum(l.startswith(PREFIX) for l in on.stderr.splitlines())
        seen += n
        e_on, e_off = strip(on.stderr), off.stderr
        if "-shards" in c:  # parallel shards interleave their stderr lines in any order (also without progress): as sets
            e_on, e_off = sorted(e_on.splitlines()), sorted(e_off.splitlines())
        same = on.stdout == off.stdout and e_on == e_off and on.returncode == off.returncode
        rows = sum(1 for l in on.stdout.splitlines() if l[:8].isalnum() and len(l) > 10 and l[8:10] == "  ")
        print("  %-60s rc %d/%d, %d rows, %d progress line(s): %s" % (" ".join(c), on.returncode, off.returncode, rows, n,
                                                                    "identical" if same else "DIFFERENT"))
        if not same:
            fails.append("%s: output differs with progress on/off; stderr tails:\n%s\n---\n%s" % (
                " ".join(c), on.stderr[-1500:], off.stderr[-600:]))
        if any(l.startswith(PREFIX) for l in off.stderr.splitlines()):
            fails.append("%s: -noprogress still printed progress lines" % " ".join(c))
        if not rows:
            fails.append("%s: no result rows" % " ".join(c))
    if not seen:
        fails.append("no progress line in any run (interval 1 s)")


def hang(fails):
    text = open(os.path.join(ROOT, SRC)).read()
    old = "    if(object!=NULL) object->mValue=0;\n    return object;"
    if text.count(old) != 1:
        raise SystemExit("mutation site not found once in %s" % SRC)
    decl = "extern void releaseWorkerArray(void*);\n"
    sleepy = text.replace(old, "    if(object!=NULL) { for (;;) usleep(50000); }\n    return object;").replace(
        decl, decl + "extern \"C\" int usleep(unsigned int);\n")
    open(os.path.join(ROOT, MUT), "w").write(sleepy)
    open(os.path.join(ROOT, MUTSPIN), "w").write(text.replace(old, "    if(object!=NULL) { volatile unsigned spin = 0; for (;;) spin++; }\n"
                                                                   "    return object;"))
    os.makedirs(TMPU, exist_ok=True)
    for name, src in (("hang", MUT), ("spin", MUTSPIN)):
        spec = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + src)
        open(os.path.join(TMPU, name + ".txt"), "w").write(spec)
        verify(["_progress_test/" + name, "-n", "1", "-only", "0275A024", "-buildj", "4", "-noprogress"])  # build
    unit = "_progress_test/hang"
    t = time.time()
    p = subprocess.Popen([sys.executable, os.path.join(HERE, "verify.py"), unit, "-only", FUNC, "-n", "3", "-stuck", "1",
                          "-hanglimit", "3", "-progress-interval", "1"], cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         text=True)
    by_pid = by_unit = ""
    t_harness = None  # when the harness's first state appeared: the hang limit is judged on the harness run alone
    for _ in range(240):  # until the harness has written its first state (after verify.py's build steps, slow under load)
        time.sleep(0.5)
        by_pid = subprocess.run([sys.executable, os.path.join(HERE, "progress.py"), str(p.pid)], cwd=ROOT, capture_output=True,
                                text=True).stdout
        if "harness pid" in by_pid:
            t_harness = time.time()
            break
        if p.poll() is not None:
            break
    by_unit = subprocess.run([sys.executable, os.path.join(HERE, "progress.py"), unit], cwd=ROOT, capture_output=True, text=True).stdout
    out, err = p.communicate()
    t_end = time.time()
    dt = t_end - t
    dt_run = t_end - t_harness if t_harness else dt
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    stuck = [l for l in err.splitlines() if l.startswith(PREFIX + "STUCK")]
    hangs = [l for l in err.splitlines() if l.startswith(PREFIX + "HANG")]
    print("  hang run: %.1f s (harness %.1f s), rc %d\n    row: %s" % (dt, dt_run, p.returncode, row[0] if row else "-"))
    for l in (stuck[:1] + hangs[:1]):
        print("    " + l)
    print("    progress.py PID:\n      " + "\n      ".join(by_pid.splitlines()))
    print("    progress.py UNIT:\n      " + "\n      ".join(by_unit.splitlines()))
    if not row or "wall-clock limit exceeded" not in row[0] or p.returncode != 1:
        fails.append("hang: row is not a FAIL by the wall-clock limit: %s" % (row or out[-300:] + err[-300:]))
    if not stuck or "seed=" not in stuck[0]:
        fails.append("hang: no STUCK line with a seed")
    if not hangs or "candidate side" not in hangs[0] or FUNC not in hangs[0] or "seed=" not in hangs[0]:
        fails.append("hang: no HANG line naming function, seed and the candidate side: %s" % hangs[:1])
    # 3 inputs whose candidate sleeps forever: only the 3 s wall-clock limit ends them (the CPU budget never fires), so the
    # harness run is a few seconds per input; verify.py's build steps before it (mkunit, link) do not count
    if dt_run > 25:
        fails.append("hang: harness ran %.0f s: the wall-clock limit did not end the inputs" % dt_run)
    if "harness pid" not in by_pid or FUNC not in by_pid:
        fails.append("progress.py PID did not show the harness state: %r" % by_pid)
    if "harness pid" not in by_unit:
        fails.append("progress.py UNIT did not show the harness state: %r" % by_unit)
    # a computing loop is ended by the 30 s CPU time budget, not by the wall-clock limit (no verdict depends on the load),
    # and not early by the watchdog thread's spurious ITIMER_VIRTUAL expiries (harness.cpp run_timeout_handler)
    t = time.time()
    r = verify(["_progress_test/spin", "-only", FUNC, "-n", "1", "-hanglimit", "3", "-stuck", "5", "-progress-interval", "5"])
    dt = time.time() - t
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    hl = [l for l in r.stderr.splitlines() if l.startswith(PREFIX + "HANG")]
    print("  spinning candidate, CPU budget (-hanglimit 3): %.1f s, rc %d\n    row: %s\n    %s" % (dt, r.returncode, row[0] if row else "-",
                                                                                hl[0] if hl else "-"))
    if not row or "time budget exceeded (30 s CPU in one run)" not in row[0] or not hl or dt < 30:
        fails.append("CPU budget: not ended by the 30 s CPU budget: %s %s (%.0f s)" % (row, hl, dt))


def receipt_stable(d):
    """a mutate.py receipt without its run-dependent parts, named one by one (everything else must match between runs):
    the measured times (`wall`: seconds of the mutant's harness runs; `timeouts_tried` / `timeout`: the adaptive limits
    derived from measured baseline times) and the per-run scratch directory the mutant executable lives in
    (`<unit>-<pid>-<random>`, cmd[0]), which also appears wherever the receipt quotes that path, and the per-campaign
    reference-side cache directory (`-refcache DIR`, refcache-<unit>-<pid>-<random>)"""
    for k in ("wall", "timeouts_tried", "timeout"):
        d.pop(k, None)
    cmd = d.get("cmd") or []
    run_dir = os.path.dirname(cmd[0]) if cmd and os.path.basename(cmd[0]) == "mutant%d" % d.get("id", -1) else None
    if run_dir:
        d = json.loads(json.dumps(d).replace(json.dumps(run_dir)[1:-1], "<run dir>"))
    if "-refcache" in cmd:  # the per-campaign reference-side cache directory (mutate.py refcache_setup, 2026-10-28)
        rc_dir = cmd[cmd.index("-refcache") + 1]
        d = json.loads(json.dumps(d).replace(json.dumps(rc_dir)[1:-1], "<refcache dir>"))
    return d


def mutate(fails):
    res = []
    for flag in (["--no-progress"], []):
        rd = tempfile.mkdtemp(prefix="wwhd-progress_test-receipts-")
        try:
            env = dict(os.environ, WWHD_VERIFY_PROGRESS_INTERVAL="1")
            r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib2_sead_15", "--func", FUNC, "--n", "50",
                                "--max", "4", "--seed", "3", "-j", "2", "--receipts", rd] + flag, cwd=ROOT, capture_output=True,
                               text=True, env=env)
            recs = {}
            for f in sorted(glob.glob(os.path.join(rd, "mutant*"))):
                if f.endswith(".json"):
                    recs[os.path.basename(f)] = receipt_stable(json.load(open(f)))
                else:
                    recs[os.path.basename(f)] = open(f).read()
            res.append((r, recs))
        finally:
            shutil.rmtree(rd, ignore_errors=True)
    (off, roff), (on, ron) = res
    lines = [l for l in on.stderr.splitlines() if l.startswith(PREFIX + "mutate")]
    print("  mutate: rc %d/%d, %d receipt files, %d progress line(s)%s" % (on.returncode, off.returncode, len(ron), len(lines),
                                                                         (": " + lines[-1]) if lines else ""))
    if on.stdout != off.stdout or on.returncode != off.returncode:
        fails.append("mutate: stdout differs with progress on/off")
    if ron != roff or not ron:
        fails.append("mutate: receipts differ with progress on/off (or none)")
    if not lines or "kills" not in lines[-1]:
        fails.append("mutate: no progress line with kills")
    if any(l.startswith(PREFIX) for l in off.stderr.splitlines()):
        fails.append("mutate --no-progress printed progress lines")


def main():
    private_scratch()
    fails = []
    try:
        print("1. byte-identical output, progress on vs off")
        identical(fails)
        print("2. stuck input / wall-clock hang limit")
        hang(fails)
        print("3. mutate.py")
        mutate(fails)
    finally:
        shutil.rmtree(TMPU, ignore_errors=True)
        for m in (MUT, MUTSPIN):
            if os.path.exists(os.path.join(ROOT, m)):
                os.remove(os.path.join(ROOT, m))
    for x in fails:
        print("FAIL", x)
    print("progress_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
