#!/usr/bin/env python3
"""Self-test of the parallel schema check of mutate.py (2026-10-28, check_schemata at -j).

One campaign (lib2_sead_15, --n 100, --max 60, --seed 3) with --schemata --reach-skip --fork --nulltrap -j 4.
1. Probe: every schema executable made to fail its unmutated check (WWHD_MUT_SCHEMA_CHECK_FAIL=all), serial check: the
   campaign stops with 'none of the N schema build(s)' before any mutant; WWHD_MUT_SCHEMA_DUMP gives the N executables
   as groups of mutant IDs. N and the groups depend on the unit's sources (which mutants the sample draws and how
   mutschema layers them), so they are read here, not fixed: 2026-10-29 the test had 7 executables and
   mutants 0, 13, 14 in two of them hard-coded, and a later RuntimeIntegerFormat.cpp change (6 executables, those IDs
   in three) failed it although the schema machinery was unchanged.
2. Two runs, the schema check serial (WWHD_MUT_SCHEMA_CHECK_J=1) and parallel (=4), with the executables holding three
   chosen mutants (two in one executable, one in another) made to fail, so the per-schema fallback runs for exactly two
   executables. Required identical between the two: the executable groups (also equal to the probe's); every receipt's
   class, schema group, reach, scope and run-list use; the reach records (WWHD_MUT_REACH_DUMP); the schema-check
   messages.
3. Every executable failing with the parallel check also stops the campaign with 'none of the N schema build(s)'.
Temporary dirs are removed.
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
BASE = ["lib2_sead_15", "--n", "100", "--max", "60", "--seed", "3", "-j", "4", "--schemata", "--reach-skip", "--fork",
        "--nulltrap"]
FIELDS = ("src", "func", "mutation", "class", "reach", "scope", "scope_reason", "stage", "reason")


def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel"""
    import atexit
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3], dir=os.environ.get("WWHD_VERIFY_SCRATCH"))
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)
    return d


def mutate(tmp, tag, jobs, fail):
    env = dict(os.environ, WWHD_MUT_SCHEMA_CHECK_J=str(jobs), WWHD_MUT_SCHEMA_CHECK_FAIL=fail,
               WWHD_MUT_REACH_DUMP=os.path.join(tmp, tag + ".reach.json"),
               WWHD_MUT_SCHEMA_DUMP=os.path.join(tmp, tag + ".schema.json"))
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + BASE + ["--receipts", os.path.join(tmp, tag)],
                       cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout + r.stderr


def groups(tmp, tag):
    """the schema executables of a run as sorted groups of mutant IDs (WWHD_MUT_SCHEMA_DUMP), or None"""
    try:
        return json.load(open(os.path.join(tmp, tag + ".schema.json")))
    except (OSError, ValueError):
        return None


def receipts(d):
    """id -> the compared fields; the schema executable is compared as the group of mutants it holds (its file name's
    number comes from mutschema.build's parallel compile order, which -j already left unstable before 2026-10-28)"""
    out, groups = {}, {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        if j.get("schema"):
            groups.setdefault(j["schema"], set()).add(j["id"])
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        cmd = j.get("cmd") or []
        out[j["id"]] = tuple(json.dumps(j.get(k), sort_keys=True) for k in FIELDS) + (
            "-runlist" in cmd, tuple(sorted(groups.get(j.get("schema"), ()))))
    return out


def schema_lines(out):
    """the schema-check messages; executable names reduced to their source (numbering: see receipts)"""
    return [re.sub(r"(schema-\S+?)-\d+ ", r"\1-N ", l) for l in out.splitlines()
            if re.match(r"mutate\.py: (schema build|none of the|--reach-skip:|schema builds:)", l)]


def main():
    tmp = private_build_dir()
    fails = []
    # 1. probe: the executables (groups of mutant IDs); every one failing stops the campaign, serial check
    rc, out = mutate(tmp, "allfail1", 1, "all")
    g = groups(tmp, "allfail1")
    n = len(g or [])
    print("mutate_schemaj_test: %d schema executable(s), groups %s" % (n, g))
    if not g or n < 2 or not any(len(x) >= 2 for x in g):
        print("FAIL probe: need >= 2 schema executables, one with >= 2 mutants (rc %d): %s" % (rc, out[-400:]))
        print("mutate_schemaj_test: FAIL")
        return 1
    if rc == 0 or "none of the %d schema build(s) passes unmutated" % n not in out:
        fails.append("allfail1: rc %d, no fatal stop: %s" % (rc, out[-300:]))
    two = next(x for x in g if len(x) >= 2)
    one = next(x for x in g if x is not two)
    chosen = [two[0], two[1], one[0]]
    jpar = min(4, n)
    # 2. serial and parallel check with exactly two executables failing
    runs = {}
    for tag, jobs in (("serial", 1), ("parallel", 4)):
        rc, out = mutate(tmp, tag, jobs, ",".join(map(str, chosen)))
        runs[tag] = (rc, out, receipts(os.path.join(tmp, tag)))
        if rc or len(runs[tag][2]) < 40:
            fails.append("%s campaign (rc %d, %d receipts): %s" % (tag, rc, len(runs[tag][2]), out[-400:]))
        if groups(tmp, tag) != g:
            fails.append("%s: schema executables differ from the probe's: %s vs %s" % (tag, groups(tmp, tag), g))
    (_, oa, ra), (_, ob, rb) = runs["serial"], runs["parallel"]
    if "%d at a time" % jpar not in ob or "at a time" in oa:
        fails.append("serial/parallel check not as asked: %r / %r" % ([l for l in oa.splitlines() if "at a time" in l],
                                                                       [l for l in ob.splitlines() if "at a time" in l]))
    if sorted(schema_lines(oa)) != sorted(schema_lines(ob)):
        fails.append("schema-check messages differ:\n  %s\nvs\n  %s" % ("\n  ".join(schema_lines(oa)),
                                                                         "\n  ".join(schema_lines(ob))))
    for tag, o in (("serial", oa), ("parallel", ob)):
        if sum("does not pass unmutated" in l for l in schema_lines(o)) != 2:
            fails.append("%s: expected 2 failing schema builds (mutants %s): %s" % (tag, chosen, schema_lines(o)))
    if sorted(ra) != sorted(rb):
        fails.append("receipt ids differ: %s vs %s" % (sorted(ra), sorted(rb)))
    for i in sorted(set(ra) & set(rb)):
        if ra[i] != rb[i]:
            fails.append("mutant %d: serial %s, parallel %s" % (i, ra[i], rb[i]))
    da, db = (open(os.path.join(tmp, t + ".reach.json")).read() for t in ("serial", "parallel"))
    if da != db or len(json.loads(da)) < 20:
        fails.append("reach records differ or are few (%d vs %d mutants)" % (len(json.loads(da)), len(json.loads(db))))
    nschema = sum(1 for v in rb.values() if v[-1])
    print("mutate_schemaj_test: %d receipts, %d on a schema, %d with reach data, messages %d lines" % (
        len(rb), nschema, len(json.loads(db)), len(schema_lines(ob))))
    # 3. every schema build failing is fatal with the parallel check too
    rc, out = mutate(tmp, "allfail4", 4, "all")
    if rc == 0 or "none of the %d schema build(s) passes unmutated" % n not in out:
        fails.append("allfail4: rc %d, no fatal stop: %s" % (rc, out[-300:]))
    for f in fails:
        print("FAIL", f)
    print("mutate_schemaj_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
