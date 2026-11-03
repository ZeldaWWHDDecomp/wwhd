#!/usr/bin/env python3
"""Re-verify every unit: build each one and run all its functions.

usage: verify_all.py [-n 10000] [-j JOBS] [-rec DIR] [UNIT...]

-rec DIR: recordings, DIR/ADDR/*.tap (one directory for all units; functions without
recordings just run their generated inputs). Without -rec, a unit file's own `rec DIR` line
(verify.py) supplies that unit's committed recordings. Prints one line per unit and a total; the exit
status is non-zero if any function fails or any unit does not build. Per-unit output is kept in
build/verify/<unit>/verify_all.log.
"""
import argparse
import shutil
import concurrent.futures
import os
import re
import subprocess
import sys
import threading
import time

import verify  # noqa: E402  (same directory)
import specfile  # noqa: E402  (same directory)
import progress  # noqa: E402  (progress lines, 2026-10-27; verify_all.log stays exactly as before)

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
LINE = verify.RESULT_LINE  # one definition; names may contain spaces


# units whose verify binary needs ~30-40 GB of RAM (HD GPU packet buffers): never run two at once
SEED = None
HEAVY = verify.HEAVY  # verify.py also takes the machine-wide HEAVY flock for these
HEAVY_LOCK = threading.Semaphore(verify.HEAVY_SLOTS)  # within this run; verify.py takes the machine-wide slot


CLEAN = False  # --clean: delete each unit's build directory as soon as it has run (keeps only its log)


def run(unit, n, rec):
    if unit in HEAVY:
        with HEAVY_LOCK:
            r = run1(unit, n, rec)
    else:
        r = run1(unit, n, rec)
    for d in verify.unit_dirs_all(unit) if CLEAN else []:  # build/verify/<unit> and its WWHD_VERIFY_SCRATCH copy
        for e in os.listdir(d) if os.path.isdir(d) else []:
            # keep the build directories of companion specs (units/<unit>/<spec>.txt build in build/verify/<unit>/<spec>)
            if e != "verify_all.log" and not os.path.exists(os.path.join(HERE, "units", unit, e + ".txt")):
                pth = os.path.join(d, e)
                shutil.rmtree(pth) if os.path.isdir(pth) else os.remove(pth)
    return r


P0_MARK = "[original page-0 fault on this input: no evidence]"  # harness.cpp, cascade.py, verify.py
P0_LINE = re.compile(r"nulltrap: ([0-9A-F]{8}): the original took a page-0 DSI on (\d+) input")


def original_p0_counts(out):
    """{function address text: inputs on which the original faulted on page 0}, summed over the harness's lines (shards)"""
    n = {}
    for m in P0_LINE.finditer(out):
        n[m.group(1)] = n.get(m.group(1), 0) + int(m.group(2))
    return n


NULLTRAP = True  # the user's zero-page rule (2026-10-26): every check runs with -nulltrap; --no-nulltrap for diagnosis only


def build_cmd(unit, n, rec):
    """verify.py's command line for one unit (2026-11-03: -nulltrap by default)"""
    cmd = [sys.executable, os.path.join(HERE, "verify.py"), unit, "-n", str(n)]
    if SEED is not None:
        cmd += ["-seed", str(SEED)]
    if rec:
        cmd += ["-rec", rec]
    if NULLTRAP:
        cmd += ["-nulltrap"]
    return cmd


def run1(unit, n, rec):
    t0 = time.time()
    cmd = build_cmd(unit, n, rec)
    r = progress.run_captured(cmd, "verify_all %s" % unit, cwd=ROOT)
    out = r.stdout + r.stderr
    logdir = os.path.join(ROOT, "build", "verify", unit)
    os.makedirs(logdir, exist_ok=True)
    open(os.path.join(logdir, "verify_all.log"), "w").write(out)
    funcs = []
    for line in out.splitlines():
        m = LINE.match(line)
        if m:
            funcs.append((m.group(1), m.group(2).strip(), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)),
                          m.group(9).strip()))
    return unit, r.returncode, funcs, time.time() - t0, out


def main():
    import verify as _scratch_verify  # noqa: E402  (same directory)
    _scratch_verify.check_scratch_env('verify_all.py')  # a set scratch variable naming a missing/unwritable folder stops here, nothing written
    ap = argparse.ArgumentParser()
    ap.add_argument("units", nargs="*")
    ap.add_argument("-n", type=int, default=10000)
    ap.add_argument("-seed", type=int, default=None, help="passed to verify.py (default: its own default seed)")
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    ap.add_argument("-rec")
    ap.add_argument("--clean", action="store_true", help="delete each unit's build output right after it ran")
    ap.add_argument("--no-nulltrap", action="store_true", help="diagnosis only: run without -nulltrap (the zero-page rule makes "
                    "-nulltrap the default for every check; a spec's own `nulltrap` line still applies)")
    a = ap.parse_args()
    global SEED
    SEED = a.seed
    global CLEAN
    CLEAN = a.clean
    global NULLTRAP
    NULLTRAP = not a.no_nulltrap
    ud = os.path.join(HERE, "units")
    units = a.units or specfile.gated_specs(ud)  # incl. companion specs in subfolders (d_a_bl/, d_a_demo00/ …)
    # shared tables (image, livein.tsv, frames.tsv) are generated once here, before the parallel jobs;
    # the jobs then skip regeneration (WWHD_SHARED_READY), so no two builds rewrite them at the same time
    sys.path.insert(0, HERE)
    import verify as _verify
    _verify.prepare_shared()
    os.environ["WWHD_SHARED_READY"] = "1"
    bad = 0
    tot_f = tot_ok = tot_rec = 0
    prog = progress.Settings()
    t_start, ndone = time.time(), [0]

    def counted(u):
        r = run(u, a.n, a.rec)
        ndone[0] += 1
        el = time.time() - t_start
        prog.say("verify_all: %d/%d units done (last %s), ETA %s" % (ndone[0], len(units), u,
                                                                     progress.fmt_dur(el / ndone[0] * (len(units) - ndone[0]))))
        return r
    with concurrent.futures.ThreadPoolExecutor(a.j) as ex:
        for unit, rc, funcs, dt, out in ex.map(counted, units):
            ok = [f for f in funcs if f[6].startswith("ok") and f[2] == f[3] and f[4] == f[5]]
            # zero-page rule: a row whose shown first difference carries the harness's page-0 marker has differences only on
            # inputs where the original itself faulted on page 0 (the harness shows a clean input's difference first when
            # one exists), so it is no evidence against the function (2026-11-03)
            p0only = [f for f in funcs if f not in ok and P0_MARK in f[6]]
            ok += p0only
            # floor of the zero-page rule: at least 10% clean inputs (1000 of 10000) per function and
            # seed, or the fixture must be steered; such a row is STEER, not ok
            p0n = original_p0_counts(out)
            steer = [f for f in funcs if p0n.get(f[0]) and (f[3] - p0n[f[0]]) * 10 < f[3]]
            ok = [f for f in ok if f not in steer]
            fail = [f for f in funcs if f not in ok]
            rec = sum(f[5] for f in funcs)
            tot_f += len(funcs)
            tot_ok += len(ok)
            tot_rec += rec
            state = "ok" if (rc == 0 or p0only) and funcs and not fail else ("BUILD FAILED" if not funcs else "FAIL")
            if state != "ok":
                bad += 1
            print("%-28s %3d/%3d functions ok  %6d recorded calls  %5.0fs  %s" % (unit, len(ok), len(funcs), rec, dt, state))
            for f in funcs:
                if p0n.get(f[0]):
                    print("    %s %s: %d of %d generated inputs clean (the original faults on page 0 on %d)%s" % (
                        f[0], f[1], f[3] - p0n[f[0]], f[3], p0n[f[0]], "  STEER: under 10% clean" if f in steer else ""))
            for f in p0only:
                print("    %s %s %d/%d: differences only on original page-0 fault inputs (no evidence)" % (f[0], f[1], f[2], f[3]))
            for f in fail:
                print("    %s %s %d/%d %d/%d %s" % (f[0], f[1], f[2], f[3], f[4], f[5], f[6]))
            if not funcs:
                print("\n".join("    " + l for l in out.splitlines()[-15:]))
    print("TOTAL %d/%d functions verified (%d generated inputs each), %d recorded calls, %d unit(s) failing"
          % (tot_ok, tot_f, a.n, tot_rec, bad))
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
