#!/usr/bin/env python3
"""Self-test: mutate.py removes its own refcache record directory (refcache-<spec>-<pid>-*) on every exit path it can
handle, and --sweep-stale-refcache (opt-in, off by default) removes the leftovers of dead campaigns (2026-11-09: a check
found 56 such directories, 3.3 GB, on the RAM disk, left by campaigns its guard had ended).

Carrier: lib2_sead_15 constructWorkerCounter 02759928, --max 2 --n 200, in a private scratch directory.
1. a campaign that ends normally leaves no refcache-* directory;
2. a campaign sent SIGTERM after it created its record directory exits (status 143) and leaves none;
3. a planted refcache-lib2_sead_15-<dead pid>-x directory stays without the flag, and goes with --sweep-stale-refcache
   and WWHD_MUT_SWEEP_ROOT above the scratch; without WWHD_MUT_SWEEP_ROOT the flag is refused (exit 2) and nothing goes;
   a planted one with a live pid (this test's own) stays in both cases, and so does another spec's directory;
4. the sweep's limits (sweep_stale_refcache called directly, its root pointed at a directory of this test): outside the
   root, or on a base that is not the run's scratch, nothing is removed and the skip is named; a symlink named like a dead
   campaign's directory is not followed or removed; every removal is named.
Temporary files are this test's own; they are removed at the end. Exit 0 when all pass."""
import glob
import os
import shutil
import signal
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
FAILS = []
UNIT, FUNC = "lib2_sead_15", "02759928"


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def leftovers(scr):
    return sorted(glob.glob(os.path.join(scr, "**", "refcache-*"), recursive=True))


def campaign(scr, extra=(), wait_for=None, env_extra=None):
    env = dict(os.environ, WWHD_VERIFY_SCRATCH=scr)
    # the campaign's record directory (and the sweep) must use this test's scratch: a WWHD_MUT_SCRATCH exported by the
    # caller would win over WWHD_VERIFY_SCRATCH and put both elsewhere (2026-11-10)
    env.pop("WWHD_MUT_SCRATCH", None)
    env.pop("WWHD_MUT_SWEEP_ROOT", None)
    env.update(env_extra or {})
    cmd = [sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--func", FUNC, "--max", "2", "--seed", "3", "--n", "200",
           "--nulltrap", "-j", "2", "--receipts", os.path.join(scr, "rc")] + list(extra)
    if wait_for is None:
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env)
        return r.returncode, r.stdout + r.stderr
    p = subprocess.Popen(cmd, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, env=env)
    out = []
    for line in p.stdout:
        out.append(line)
        if wait_for in line:
            p.send_signal(signal.SIGTERM)
            break
    rest, _ = p.communicate(timeout=600)
    return p.returncode, "".join(out) + (rest or "")


def dead_pid():
    p = subprocess.Popen(["true"])
    p.wait()
    return p.pid


def sweep_limits(scr, mutate):
    import contextlib
    import io
    # root stands for SWEEP_ROOT (the RAM disk); base is a run's own scratch folder below it (the root itself is not)
    root, outside = os.path.join(scr, "root"), os.path.join(scr, "outside")
    base, target = os.path.join(root, "runscratch"), os.path.join(scr, "target")
    for d in (base, outside, target):
        os.makedirs(d)
    pid = dead_pid()
    d_out = os.path.join(outside, "refcache-%s-%d-out00001" % (UNIT, pid))
    d_in = os.path.join(base, "refcache-%s-%d-in000001" % (UNIT, pid))
    d_top = os.path.join(root, "refcache-%s-%d-top00001" % (UNIT, pid))
    link = os.path.join(base, "refcache-%s-%d-link0001" % (UNIT, pid))
    keep = os.path.join(base, "notrefcache-%s-%d-x" % (UNIT, pid))
    for d in (d_out, d_in, d_top, keep):
        os.makedirs(d)
    open(os.path.join(target, "f"), "w").close()
    os.symlink(target, link)
    saved = mutate.SWEEP_ROOT
    mutate.SWEEP_ROOT = root
    try:
        err = io.StringIO()
        with contextlib.redirect_stderr(err):
            r1 = mutate.sweep_stale_refcache(outside, UNIT)
            r2 = mutate.sweep_stale_refcache(base, UNIT, is_scratch=False)
            r0 = mutate.sweep_stale_refcache(root, UNIT)
        check(r1 == [] and r2 == [] and r0 == [] and os.path.isdir(d_out) and os.path.isdir(d_in) and os.path.isdir(d_top)
              and err.getvalue().count("skipped") == 3,
              "4: outside the root / not the run's scratch / the root itself: nothing removed, skip named")
        err = io.StringIO()
        with contextlib.redirect_stderr(err):
            r3 = mutate.sweep_stale_refcache(base, UNIT)
        check(r3 == [d_in] and not os.path.exists(d_in) and "removed " + d_in in err.getvalue(),
              "4: inside the root the dead campaign's directory is removed and named")
        check(os.path.islink(link) and os.path.isfile(os.path.join(target, "f")) and os.path.isdir(keep),
              "4: a symlink and a non-refcache directory are kept, the symlink target untouched")
    finally:
        mutate.SWEEP_ROOT = saved
    mutate.SWEEP_ROOT = None
    try:
        mutate.sweep_stale_refcache(base, UNIT)
        check(False, "4: without a root the sweep raises")
    except RuntimeError as e:
        check("WWHD_MUT_SWEEP_ROOT" in str(e), "4: without a root the sweep raises and names WWHD_MUT_SWEEP_ROOT")
    finally:
        mutate.SWEEP_ROOT = saved


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH") or tempfile.gettempdir()
    scr = tempfile.mkdtemp(prefix="mutate_refcache_cleanup_test-", dir=base)
    try:
        rc, out = campaign(scr)
        check(rc == 0 and "reference-side cache in" in out, "1: the campaign ran with a reference-side cache (rc %d)" % rc)
        check(not leftovers(scr), "1: a normal end leaves no refcache directory: %s" % leftovers(scr))
        rc, out = campaign(scr, wait_for="reference-side cache in")
        check(rc == 143, "2: SIGTERM ends the campaign with status 143 (got %s)" % rc)
        check(not leftovers(scr), "2: SIGTERM leaves no refcache directory: %s" % leftovers(scr))
        # 3: planted leftovers next to where the campaign puts its own (the tree's scratch directory)
        trees = [d for d in glob.glob(os.path.join(scr, "*")) if os.path.isdir(d) and os.path.basename(d) != "rc"]
        where = trees[0] if trees else scr
        dead = os.path.join(where, "refcache-%s-%d-dead0001" % (UNIT, dead_pid()))
        live = os.path.join(where, "refcache-%s-%d-live0001" % (UNIT, os.getpid()))
        other = os.path.join(where, "refcache-lib2_sead_150-%d-other001" % dead_pid())
        for d in (dead, live, other):
            os.makedirs(d)
        campaign(scr)
        check(os.path.isdir(dead) and os.path.isdir(live) and os.path.isdir(other), "3: without the flag nothing planted is removed")
        rc, out = campaign(scr, extra=["--sweep-stale-refcache"])
        check(rc == 2 and "needs WWHD_MUT_SWEEP_ROOT" in out and os.path.isdir(dead),
              "3: without WWHD_MUT_SWEEP_ROOT the flag is refused (exit 2) and nothing is removed (rc %d)" % rc)
        rc, out = campaign(scr, extra=["--sweep-stale-refcache"], env_extra={"WWHD_MUT_SWEEP_ROOT": scr})
        check(not os.path.exists(dead) and "--sweep-stale-refcache: removed " + dead in out,
              "3: with the flag and WWHD_MUT_SWEEP_ROOT above the scratch the dead campaign's directory is removed and named")
        sys.path.insert(0, HERE)
        import mutate
        check(os.path.isdir(live) and os.path.isdir(other), "3: a live pid's and another spec's directory are kept")
        sweep_limits(scr, mutate)
    finally:
        shutil.rmtree(scr, ignore_errors=True)  # this test's own directory
    print("mutate_refcache_cleanup_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
