#!/usr/bin/env python3
"""Self-test: mutate.py's baseline gate applies the zero-page rule as verify_all does (2026-11-09: lib2_agl_17
s7, the original's page-0 fetch fault on gen 7000037 made `mutate --rerun` refuse the spec). A row whose shown first
difference carries the harness's page-0 marker ("[original page-0 fault on this input: no evidence]") differs only on
inputs where the original faulted on page 0, so it is no evidence and the baseline passes it.
1. exit 1 with every non-ok row marked: passes (None);
2. exit 1 with a marked row and an unmarked FAIL row: fails, naming only the unmarked row;
3. a marked row with any other exit status (2: spec error, a crash): fails;
4. a plain page-0 row without the marker (`trap: page-0 DSI by the candidate only`): fails, as before;
5. exit 0 with ok rows: passes; no rows at all: fails;
6. the marker is the same text verify_all.py and cascade.py use.
Pure Python, no build. Exit 0 when all pass."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import cascade  # noqa: E402
import mutate  # noqa: E402
import verify  # noqa: E402
import verify_all  # noqa: E402

OK = "027C4BE0  agl::lyr::Layer::okFunction                1000/1000        0/0    10/10  ok"
P0 = ("027C4C5C  agl::lyr::Layer::p0Function                 999/1000        0/0    12/14  call #3 C0006A40: r4 differs "
      "(original 00000007, candidate 00000008) [original page-0 fault on this input: no evidence]  {gen seed=7000037}")
BAD = "027C4D00  agl::lyr::Layer::badFunction                998/1000        0/0    12/14  memory 6A000010: original 01, candidate 02  {gen seed=7000002}"
DSI = "027C4D40  agl::lyr::Layer::dsiFunction                999/1000        0/0    12/14  trap: page-0 DSI by the candidate only (load 00000004, depth 0)  {gen seed=7000005}"
FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def out(*rows):
    return "addr      function                                  generated   recorded coverage  first difference\n" + "\n".join(rows) + "\n"


def main():
    check(mutate.check_baseline(1, out(OK, P0), "t") is None, "1: exit 1 with only marked rows passes")
    m = mutate.check_baseline(1, out(OK, P0, BAD), "t")
    check(m is not None and "027C4D00" in m and "027C4C5C" not in m and "1 of 3 function row(s) not ok" in m,
          "2: a marked row plus an unmarked FAIL: fails naming only the unmarked row: %r" % (m or "")[:300])
    check(mutate.check_baseline(2, out(OK, P0), "t") is not None, "3: a marked row with exit 2 fails")
    check(mutate.check_baseline(1, out(OK, DSI), "t") is not None, "4: an unmarked page-0 row fails")
    check(mutate.check_baseline(0, out(OK), "t") is None and mutate.check_baseline(0, "", "t") is not None,
          "5: exit 0 with ok rows passes; no rows fails")
    check(verify.P0_MARK == verify_all.P0_MARK == cascade.P0_MARK, "6: one marker text in verify, verify_all and cascade")
    print("mutate_baseline_p0_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
