#!/usr/bin/env python3
"""Self-test (2026-11-03): verify_all.py passes -nulltrap to verify.py by default and leaves it out with
--no-nulltrap; builds nothing. Exit 0 when all pass."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import verify_all  # noqa: E402

FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def main():
    verify_all.SEED, verify_all.NULLTRAP = None, True
    c = verify_all.build_cmd("lib_mtx", 1000, None)
    check("-nulltrap" in c, "default: -nulltrap passed: %s" % " ".join(c[2:]))
    verify_all.SEED = 7
    c = verify_all.build_cmd("lib_mtx", 1000, "recdir")
    check(c[-1] == "-nulltrap" and "-seed" in c and "-rec" in c, "with -seed and -rec: %s" % " ".join(c[2:]))
    verify_all.NULLTRAP = False
    c = verify_all.build_cmd("lib_mtx", 1000, None)
    check("-nulltrap" not in c, "--no-nulltrap: left out: %s" % " ".join(c[2:]))
    src = open(os.path.join(HERE, "verify_all.py")).read()
    check('"--no-nulltrap"' in src and "NULLTRAP = not a.no_nulltrap" in src, "the flag sets NULLTRAP")
    src = open(os.path.join(HERE, "verify_all.py")).read()
    check("p0only" in src and verify_all.P0_MARK == "[original page-0 fault on this input: no evidence]", "page-0-only rows count as no evidence")
    out = ("nulltrap: 028E948C: the original took a page-0 DSI on 9 input(s); first: x\n"
           "nulltrap: 0275CEE4: the original took a page-0 DSI on 950 input(s); first: x\n"
           "nulltrap: 0275CEE4: the original took a page-0 DSI on 6 input(s); first: x\n")
    n = verify_all.original_p0_counts(out)
    check(n == {"028E948C": 9, "0275CEE4": 956}, "page-0 counts summed per function over shard lines: %s" % n)
    check((1000 - n["0275CEE4"]) * 10 < 1000 and not (1000 - n["028E948C"]) * 10 < 1000,
          "STEER when under 10%% clean: 44 of 1000 is STEER, 991 of 1000 is not")
    print("verify_all_nulltrap_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
