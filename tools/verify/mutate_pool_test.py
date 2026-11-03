#!/usr/bin/env python3
"""Self-test of mutate.py --pool (2026-10-26, mutfast).

The same small campaign (lib2_sead_15, --func 02759928, --max 12, --seed 3) runs the old way and with --pool: every mutant ID must end the same way (receipts A vs B). JobPool directly: with 2
slots, 6 threads on two pool instances (two flock holders, as two processes would be) never hold more than 2 at once;
at a load above 1.25x the cores a pool that already holds a slot waits for a second one while another pool (another
process) still gets its first. Temporary dirs under
build/verify/_mutate_pool_test/ are removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_pool_test")
BASE = ["lib2_sead_15", "--func", "02759928", "--n", "100", "--max", "12", "--seed", "3", "-j", "2"]
KILLED = {"difference", "abort", "signal", "invalid"}

def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel (2026-10-26: two tests building lib2_sead_15 in build/verify at once broke a baseline)"""
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)



def mutate(*extra):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + BASE + list(extra), cwd=ROOT,
                       capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def classes(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = j
    return out


def coarse(c):
    return "killed" if c in KILLED else c


def pool_checks():
    import threading
    import time
    sys.path.insert(0, HERE)
    import mutate as M
    out = []
    d = os.path.join(TMP, "pool")
    A, B = M.JobPool(2, d, loadavg=lambda: 0.0), M.JobPool(2, d, loadavg=lambda: 0.0)
    cur, peak, lk = [0], [0], threading.Lock()

    def work(p):
        f = p.acquire()
        with lk:
            cur[0] += 1
            peak[0] = max(peak[0], cur[0])
        time.sleep(0.2)
        with lk:
            cur[0] -= 1
        p.release(f)
    ts = [threading.Thread(target=work, args=(A if k % 2 else B,)) for k in range(6)]
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    if peak[0] != 2:
        out.append("2 slots, peak %d jobs at once" % peak[0])
    hot = M.JobPool(8, os.path.join(TMP, "pool2"), loadavg=lambda: 1000.0)
    other = M.JobPool(8, os.path.join(TMP, "pool2"), loadavg=lambda: 1000.0)
    f1 = hot.acquire()
    got = []
    t = threading.Thread(target=lambda: got.append(hot.acquire()))
    t.start()
    t.join(0.6)
    if got:
        out.append("a second slot was granted at load 1000")
    f3 = other.acquire()  # another process's first slot
    hot.release(f1)
    t.join(5)
    if not got:
        out.append("the waiting job did not get a slot after the first was released")
    for f in got:
        hot.release(f)
    other.release(f3)
    return out


def main():
    private_build_dir()  # parallel self-tests must not share build/verify/<unit>
    os.makedirs(TMP, exist_ok=True)
    fails = []
    A, B = os.path.join(TMP, "A"), os.path.join(TMP, "B")
    try:
        rc, out = mutate("--receipts", A)
        ra = classes(A)
        if rc or len(ra) < 8:
            fails.append("old campaign (rc %d, %d receipts): %s" % (rc, len(ra), out[-300:]))
        rc, out = mutate("--receipts", B, "--pool")
        rb = classes(B)
        if rc or sorted(rb) != sorted(ra):
            fails.append("--pool campaign (rc %d, ids %s vs %s): %s" % (rc, sorted(rb), sorted(ra), out[-300:]))
        fails += pool_checks()
        for i in sorted(set(ra) & set(rb)):
            ca, cb = ra[i]["class"], rb[i]["class"]
            if coarse(ca) != coarse(cb):
                fails.append("mutant %d: %s without, %s with --pool (%s)" % (i, ca, cb, ra[i]["mutation"]))

    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_pool_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
