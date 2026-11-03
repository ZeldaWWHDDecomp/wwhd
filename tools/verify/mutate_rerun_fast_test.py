#!/usr/bin/env python3
"""Self-test of mutate.py --rerun with --schemata, --fork and --reach-skip (2026-10-30).

Before, rerun mode ignored the three options (each recorded mutant was built on its own and ran every input). Now each
recorded mutantN.cpp becomes one edit of today's source (mutscope.changed_span), and the schema builds, fork servers and
reach markers work as in an enumerated campaign.

A main campaign on lib2_sead_15 (--max 40 --n 2, seed 1: weak, so many survive) is rerun (its survivors, --rerun-class
survived) on two companions (temporary copies of lib2_sead_15, one with `arg! 02759928 r3 0`) at n 200, plain and with
--schemata --fork --reach-skip:
- every rerun mutant gets a receipt in both modes with the same ID;
- the class is the same per mutant, except `survived` with reach `not reached` (fast) for `survived` (plain); a kill in
  one mode and not in the other fails;
- the fast run used schema executables (receipts name `schema`) and reach data (log line `--reach-skip:`);
- the schema check reads the baseline's original-side records; with WWHD_MUT_SCHEMA_CHECK_NOCACHE=1 (live original) the
  reach records (WWHD_MUT_REACH_DUMP) and the classes are identical.
Writes a temporary unit directory (tools/verify/units/_rerun_fast_test) and work directories below WWHD_VERIFY_SCRATCH
(or a temporary directory); nothing is deleted. Exit 0 when all pass.
"""
import glob
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_rerun_fast_test")


def mutate(*args):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + list(args) + ["-j", "4", "--no-progress"],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def receipts(d):
    return {json.load(open(f))["id"]: json.load(open(f)) for f in glob.glob(os.path.join(d, "mutant*.json"))}


def main():
    fails = []
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    work = tempfile.mkdtemp(prefix="rerun_fast_test-", dir=base if base and os.path.isdir(base) else None)
    os.makedirs(TMP, exist_ok=True)
    spec = open(os.path.join(UNITS, "lib2_sead_15.txt")).read()
    open(os.path.join(TMP, "comp_a.txt"), "w").write(spec)
    open(os.path.join(TMP, "comp_b.txt"), "w").write(spec + "\narg! 02759928 r3 0\n")
    main_dir = os.path.join(work, "main")
    rc, out = mutate("lib2_sead_15", "--max", "40", "--n", "2", "--seed", "1", "--nulltrap", "--receipts", main_dir)
    if rc:
        fails.append("main campaign: %s" % out[-400:])
    surv = [j for j in receipts(main_dir).values() if j["class"] == "survived"]
    rin = os.path.join(work, "in")
    os.makedirs(rin, exist_ok=True)
    for j in surv:
        shutil.copy(os.path.join(main_dir, "mutant%d.json" % j["id"]), rin)
        shutil.copy(os.path.join(main_dir, "mutant%d.cpp" % j["id"]), rin)
    print("mutate_rerun_fast_test: %d survivors to rerun" % len(surv))
    if len(surv) < 5:
        fails.append("too few survivors (%d)" % len(surv))
    for comp in ("comp_a", "comp_b"):
        res = {}
        for mode, extra in (("plain", []), ("fast", ["--schemata", "--fork", "--reach-skip"]),
                            ("fastlive", ["--schemata", "--fork", "--reach-skip"])):
            rd = os.path.join(work, "%s-%s" % (comp, mode))
            os.environ["WWHD_MUT_REACH_DUMP"] = rd + ".reach.json"
            os.environ["WWHD_MUT_SCHEMA_CHECK_NOCACHE"] = "1" if mode == "fastlive" else "0"
            rc, out = mutate("_rerun_fast_test/" + comp, "--rerun", rin, "--n", "200", "--seed", "1", "--nulltrap",
                             "--receipts", rd, *extra)
            if rc:
                fails.append("%s %s rc %d: %s" % (comp, mode, rc, out[-400:]))
            res[mode] = receipts(rd)
            if mode == "fast":
                if not any(j.get("schema") for j in res[mode].values()):
                    fails.append("%s fast: no mutant ran from a schema executable" % comp)
                if "--reach-skip:" not in out:
                    fails.append("%s fast: no reach data" % comp)
        da, db = (open(os.path.join(work, "%s-%s.reach.json" % (comp, m))).read() for m in ("fast", "fastlive"))
        if da != db:
            fails.append("%s: reach records differ between the schema check with records and with a live original" % comp)
        if {i: j["class"] for i, j in res["fast"].items()} != {i: j["class"] for i, j in res["fastlive"].items()}:
            fails.append("%s: classes differ between the schema check with records and with a live original" % comp)
        p, f = res["plain"], res["fast"]
        if sorted(p) != sorted(f) or sorted(p) != sorted(j["id"] for j in surv):
            fails.append("%s: receipt ids differ: %s / %s" % (comp, sorted(p), sorted(f)))
        same = nr = 0
        for i in sorted(set(p) & set(f)):
            cp, cf = p[i]["class"], f[i]["class"]
            if cp == cf:
                same += 1
                nr += f[i].get("reach") == "not reached"
            else:
                fails.append("%s mutant %d: plain %s, fast %s" % (comp, i, cp, cf))
        kills = sum(1 for j in p.values() if j["class"] == "difference")
        print("  %s: %d mutants, %d same class (%d of them not reached in fast mode), %d killed" % (comp, len(p), same, nr, kills))
    for x in fails:
        print("FAIL", x)
    print("mutate_rerun_fast_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
