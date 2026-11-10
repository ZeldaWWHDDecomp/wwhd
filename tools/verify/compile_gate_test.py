#!/usr/bin/env python3
"""Self-test of mutate.py's compile load gate (WWHD_COMPILE_LOAD_MAX, 2026-11-01): unset it never waits; set, it waits
while the 1-minute load is at or above the limit and returns once it drops; every clang call in mutate.py goes
through run_clang."""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mutate  # noqa: E402


def main():
    fails = []
    os.environ.pop("WWHD_COMPILE_LOAD_MAX", None)
    if mutate.compile_gate(loadavg=lambda: (500.0,), sleep=lambda s: fails.append("slept while unset")) != 0:
        fails.append("unset gate waited")
    os.environ["WWHD_COMPILE_LOAD_MAX"] = "70"
    loads = iter([104.0, 80.0, 70.0, 69.9, 200.0])
    slept = []
    w = mutate.compile_gate(loadavg=lambda: (next(loads),), sleep=slept.append)
    if w != 3 or slept != [15, 15, 15]:
        fails.append("set gate: waited %d, sleeps %s (want 3 x 15 s: 104, 80, 70 are at/above 70)" % (w, slept))
    if mutate.compile_gate(loadavg=lambda: (10.0,), sleep=lambda s: fails.append("slept below the limit")) != 0:
        fails.append("set gate waited below the limit")
    os.environ.pop("WWHD_COMPILE_LOAD_MAX", None)
    os.environ["WWHD_COMPILE_MIN_FREE_MB"] = "2560"
    frees = iter([2000, 2559, 2560, 100])
    slept = []
    w = mutate.compile_gate(loadavg=lambda: (500.0,), sleep=slept.append, freemb=lambda: next(frees))
    if w != 2:
        fails.append("free-space gate: waited %d (want 2: 2000 and 2559 MB are below 2560; the load limit is unset)" % w)
    os.environ["WWHD_COMPILE_LOAD_MAX"] = "70"
    seq = iter([(80.0, 3000), (10.0, 1000), (10.0, 3000)])
    cur = [None]

    def la():
        cur[0] = next(seq)
        return (cur[0][0],)
    w = mutate.compile_gate(loadavg=la, sleep=lambda s: None, freemb=lambda: cur[0][1])
    if w != 2:
        fails.append("both gates: waited %d (want 2: high load, then low free space)" % w)
    os.environ.pop("WWHD_COMPILE_LOAD_MAX", None)
    os.environ.pop("WWHD_COMPILE_MIN_FREE_MB", None)
    if mutate.scratch_free_mb() <= 0:
        fails.append("scratch_free_mb: %d" % mutate.scratch_free_mb())
    src = open(os.path.join(HERE, "mutate.py")).read()
    direct = [l.strip() for l in src.split("\n") if re.search(r'subprocess\.run\((\["clang|mutant_flags\(|fl\[:4\])', l)]
    if direct:
        fails.append("clang calls bypassing run_clang: %s" % direct)
    print("compile_gate_test: %s" % ("FAIL: " + "; ".join(fails) if fails else "ok"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
