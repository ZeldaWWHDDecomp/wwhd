#!/usr/bin/env python3
"""Self-test of mutate.py's adaptive per-mutant timeout (2026-10-26, mutfast).

TimeoutPolicy: max(60, 10x the unmutated run's wall time). With --timeout-remeasure (the behaviour until 2026-10-31) it is
measured again when the 1-minute load moved by more than half, in a background thread (2026-10-30: current() never
waits for it; meanwhile the last limit scaled up by the load ratio). The default since 2026-10-31 measures once (the
baseline gate's run, or a cached baseline's wall time and load) and only scales with the load: no thread, no second
measurement. A fixed --timeout never measures and never retries. run_with_retry with real processes: a fast run, a slow
one (sleep loop) that passes on the retry at twice the limit, one that still times out (class timeout), and the fixed
mode (no retry). Last, a small campaign (lib2_sead_15, --func 02759928, --max 4) without --timeout prints the derived
limit and records it in the receipts. Temporary dirs under build/verify/_mutate_timeout_test/ are removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import mutate  # noqa: E402

TMP = os.path.join(ROOT, "build", "verify", "_mutate_timeout_test")

def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel (2026-10-26: two tests building lib2_sead_15 in build/verify at once broke a baseline)"""
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)



def sleeper(sec):
    return [sys.executable, "-c", "import time\nt = time.time()\nwhile time.time() - t < %s: time.sleep(0.05)" % sec]


def main():
    private_build_dir()  # parallel self-tests must not share build/verify/<unit>
    fails = []
    # the policy
    walls, loads, calls = [2.0, 20.0, 3.0], [10.0, 12.0, 20.0, 21.0, 5.0], []  # init, then one per current()

    def measure():
        calls.append(1)
        return walls[len(calls) - 1]
    lo = iter(loads)
    p = mutate.TimeoutPolicy(None, measure, loadavg=lambda: next(lo), remeasure=True)
    # 2026-10-30: a load move of more than half re-measures in the background; meanwhile the last measurement
    # scaled up by the load ratio (never down). 12: 10*2*1.2=24 -> floor 60; 20 (+100%): 10*2*2=40 -> 60, then measured
    # 20 s at load 20; 21: 10*20*1.05=210; 5 (-75%): 10*20*1=200 while measuring, then 3 s -> 60
    got = []
    for _ in range(4):
        got.append(p.current())
        p.wait()
    after = max(mutate.TimeoutPolicy.FLOOR, p.FACTOR * p.base)
    if got != [60.0, 60.0, 210.0, 200.0] or after != 60.0 or len(calls) != 3 or not p.retry:
        fails.append("derived limits %s (then %s) after %d measurements" % (got, after, len(calls)))
    # the re-measurement does not block: current() returns while a slow measurement runs
    import time
    slow_loads = iter([10.0, 30.0, 30.0])
    nmeas = []

    def slow_measure():  # the first (constructor) measurement is quick, later ones take 2 s
        if nmeas:
            time.sleep(2)
        nmeas.append(1)
        return 5.0
    q = mutate.TimeoutPolicy(None, slow_measure, loadavg=lambda: next(slow_loads), remeasure=True)
    t0 = time.time()
    v1 = q.current()  # starts the 2 s measurement in the background
    v2 = q.current()  # still measuring: no second thread, no wait
    dt = time.time() - t0
    q.wait()
    if dt > 0.5 or v1 != 150.0 or v2 != 150.0 or q.base != 5.0:
        fails.append("background re-measurement blocked or wrong: %.2f s, limits %s %s, base %s" % (dt, v1, v2, q.base))
    # the default since 2026-10-31 (fasttools2 change 1): one measurement (the baseline gate's wall time), never again;
    # the limit follows the load up (never down) relative to the load of that measurement, and no thread ever starts
    fl, fcalls = iter([10.0, 12.0, 20.0, 40.0, 5.0, 100.0]), []

    def fmeasure():
        fcalls.append(1)
        return 4.0
    z = mutate.TimeoutPolicy(None, fmeasure, loadavg=lambda: next(fl))
    zg = [z.current() for _ in range(5)]  # loads 12, 20, 40, 5, 100 against the base load 10: 4 s x10 = 40 -> floor 60
    if zg != [60.0, 80.0, 160.0, 60.0, 400.0] or len(fcalls) != 1 or z.thread is not None or not z.retry:
        fails.append("frozen policy: limits %s, %d measurement(s), thread %s, retry %s" % (zg, len(fcalls), z.thread, z.retry))
    # a cached baseline (--baseline-cache): its recorded load is the reference, not the load now
    cl = iter([30.0, 30.0])
    c = mutate.TimeoutPolicy(None, lambda: 10.0, loadavg=lambda: next(cl), first_load=15.0)
    if c.current() != 200.0 or c.load != 15.0:
        fails.append("frozen policy with a cached baseline's load: %s (load %s)" % (c.current(), c.load))
    f = mutate.TimeoutPolicy(30, lambda: fails.append("a fixed --timeout measured the baseline"))
    if f.current() != 30.0 or f.retry:
        fails.append("fixed --timeout: %s, retry %s" % (f.current(), f.retry))
    # runs: fast, slow (passes on the retry), hung (timeout), fixed (no retry)
    r, tried = mutate.run_with_retry(sleeper(0), 2, True, capture_output=True)
    if r is None or r.returncode or tried != [2]:
        fails.append("fast run: %s %s" % (r, tried))
    r, tried = mutate.run_with_retry(sleeper(1.5), 1, True, capture_output=True)
    if r is None or r.returncode or tried != [1, 2]:
        fails.append("slow run not passed by the retry: %s %s" % (r, tried))
    r, tried = mutate.run_with_retry(sleeper(8), 1, True, capture_output=True)
    if r is not None or tried != [1, 2]:
        fails.append("hung run: %s %s" % (r, tried))
    r, tried = mutate.run_with_retry(sleeper(1.5), 1, False, capture_output=True)
    if r is not None or tried != [1]:
        fails.append("fixed limit retried: %s %s" % (r, tried))
    # a campaign without --timeout
    os.makedirs(TMP, exist_ok=True)
    try:
        rc = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib2_sead_15", "--func", "02759928", "--n", "50",
                             "--max", "4", "--seed", "3", "-j", "2", "--receipts", TMP], cwd=ROOT, capture_output=True, text=True)
        if rc.returncode or "per-mutant timeout" not in rc.stderr or "never measured again" not in rc.stderr or \
                rc.stderr.count("per-mutant timeout") != 1:
            fails.append("campaign: rc %d, no derived limit printed: %s" % (rc.returncode, (rc.stdout + rc.stderr)[-300:]))
        recs = [json.load(open(x)) for x in glob.glob(os.path.join(TMP, "mutant*.json"))]
        ran = [x for x in recs if x["class"] != "broken"]
        if not ran or any(not x.get("timeouts_tried") or x["timeouts_tried"][0] < 60 for x in ran):
            fails.append("receipts without a derived limit >= 60 s: %s" % [x.get("timeouts_tried") for x in recs])
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for x in fails:
        print("FAIL", x)
    print("mutate_timeout_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
