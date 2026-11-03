#!/usr/bin/env python3
"""Self-test of the shared two-stage survivor cascade (tools/verify/cascade.py, 2026-10-30).
1. kind(): the kill rule (clean difference and candidate-only faults kill; original-only / both-sides faults and aborts
   do not: `difference-page0`);
2. a main campaign on lib2_sead_15 (--max 40 --n 2, seed 1: small, so many survive) and two companion specs (temporary copies of
   lib2_sead_15, one with `arg! 02759928 r3 0` so the allocation path runs): the two-stage cascade (n1 20, n2 200) and
   the single-stage one (n2 200; the default, and the --single-stage alias) leave the same alive set; every stage-2 pass runs only stage-1 survivors; alive.json has
   a history per mutant; running the two-stage command again resumes (no pass runs twice).
Writes a temporary unit directory (tools/verify/units/_cascade_test) and OUT directories below WWHD_VERIFY_SCRATCH (or a
temporary directory); nothing is deleted. Exit 0 when all pass.
"""
import json
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import cascade  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_cascade_test")


def main():
    fails = []
    NT = ["verify", "-spec", "x", "-n", "1000", "-nulltrap"]
    row = "02759928  constructWorkerCounter                      0/30        0/0      2/5  %s  {gen seed=1000003}"
    for text, want in (("memory 00000003: original unchanged, candidate 00", "difference"),
                       ("trap: page-0 DSI by the candidate only (store 00000000, depth 0)", "difference"),
                       ("trap: page-0 DSI by the original only (load 00000004, depth 0)", "difference-page0"),
                       ("only one side aborted: time budget exceeded", "difference-page0")):
        got = cascade.kind({"class": "difference", "stdout": row % text, "cmd": NT})
        if got != want:
            fails.append("kind(%r) = %s, want %s" % (text, got, want))
        #: the same receipt from a run without -nulltrap proves no kill
        got = cascade.kind({"class": "difference", "stdout": row % text, "cmd": ["verify", "-spec", "x", "-n", "1000"]})
        if got != "difference-unproven":
            fails.append("kind(%r) without -nulltrap = %s, want difference-unproven" % (text, got))
        # the harness's suffix about OTHER inputs aborting on both sides must not change the row's verdict (AGL#7 10:34)
        got = cascade.kind({"class": "difference", "stdout": row % text + cascade.AB_NOTE, "cmd": NT})
        if got != want:
            fails.append("kind(%r + both-sides-abort note) = %s, want %s" % (text, got, want))
    okrow = "02759928  constructWorkerCounter                     30/30        0/0      2/5  ok" + cascade.AB_NOTE
    if cascade.failing_rows({"stdout": okrow}):
        fails.append("an ok row with the both-sides-abort note counted as failing")
    for c in ("survived", "abort", "timeout"):
        if cascade.kind({"class": c}) != c:
            fails.append("kind(%s)" % c)
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    work = tempfile.mkdtemp(prefix="cascade_test-", dir=base if base and os.path.isdir(base) else None)
    os.makedirs(TMP, exist_ok=True)
    spec = open(os.path.join(UNITS, "lib2_sead_15.txt")).read()
    open(os.path.join(TMP, "comp_a.txt"), "w").write(spec)
    open(os.path.join(TMP, "comp_b.txt"), "w").write(spec + "\narg! 02759928 r3 0\n")
    main_dir = os.path.join(work, "main")
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib2_sead_15", "--max", "40",
                        "--n", "2", "--seed", "1", "--nulltrap", "-j", "4", "--no-progress", "--receipts", main_dir],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        fails.append("main campaign failed: %s" % (r.stdout + r.stderr)[-400:])
    specs = ["_cascade_test/comp_a", "_cascade_test/comp_b"]

    def casc(out, *extra):
        r = subprocess.run([sys.executable, os.path.join(HERE, "cascade.py"), "--receipts", main_dir, "--out", out,
                            "--input-seed", "1", "--n1", "20", "--n2", "200", "-j", "4"] + list(extra) + specs,
                           cwd=ROOT, capture_output=True, text=True)
        return r.returncode, r.stdout + r.stderr
    two, one = os.path.join(work, "two"), os.path.join(work, "one")
    rc2, o2 = casc(two, "--two-stage")
    rc1, o1 = casc(one)  # the default (single stage)
    rc1b, o1b = casc(os.path.join(work, "one_alias"), "--single-stage")  # the no-op alias
    if rc1b or o1b.count(" rerun, ") != o1.count(" rerun, ") or "the single stage" not in o1b or "stage 1 " in o1 + o1b:
        fails.append("default / --single-stage alias not the single stage: %s / %s" % (o1[-200:], o1b[-200:]))
    print(o2.strip())
    print(o1.strip())
    if rc1 or rc2:
        fails.append("cascade rc %d / %d" % (rc2, rc1))
    a2, a1 = json.load(open(os.path.join(two, "alive.json"))), json.load(open(os.path.join(one, "alive.json")))
    alive2 = sorted(i for i, v in a2.items() if not v["killed_by"])
    alive1 = sorted(i for i, v in a1.items() if not v["killed_by"])
    print("cascade_test: %d mutants; alive two-stage %s, single-stage %s" % (len(a2), alive2, alive1))
    if alive1 != alive2 or len(a2) < 5:
        fails.append("final alive sets differ (or too few mutants): %s vs %s" % (alive2, alive1))
    for i, v in a2.items():
        st2 = [h for h in v["hist"] if h[0] == 2]
        if st2 and v["killed_by"] and v["killed_by"][0] == 1:
            fails.append("mutant %s killed in stage 1 but rerun in stage 2: %s" % (i, v["hist"]))
        if not v["hist"] or v["hist"][0][0] != "main":
            fails.append("mutant %s: no history" % i)
    if not any(h[0] == 1 for v in a2.values() for h in v["hist"]):
        fails.append("no stage-1 pass recorded")
    rc, o = casc(two, "--two-stage")
    if rc or " rerun, " in o:
        fails.append("resume ran a pass again: %s" % o[-300:])
    for x in fails:
        print("FAIL", x)
    print("cascade_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
