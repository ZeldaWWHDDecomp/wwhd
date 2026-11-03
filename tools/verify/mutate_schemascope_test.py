#!/usr/bin/env python3
"""Self-test of the scoped schema check of mutate.py (2026-10-30, AGL#8 request): check_schemata runs each schema
executable's unmutated check only on the union of its members' per-mutant scopes (mutscope), not on every row.

One campaign (lib2_sead_15, --n 100, --max 60, --seed 3) with --schemata --reach-skip --fork --nulltrap -j 4, run
1. with every row (WWHD_MUT_SCHEMA_CHECK_WHOLE=1, the check as before) and
2. scoped (the default).
Required identical between the two: the schema executables (WWHD_MUT_SCHEMA_DUMP, as groups of mutant IDs); every
receipt's class, reach, scope (incl. scope_widened_by_reach), schema group and run-list use; the reach records
(WWHD_MUT_REACH_DUMP); the schema-check pass/drop messages. The scoped check must make fewer row runs (the harness
rows its checks printed, `schema check took ... N row run(s)`) and scope at least one executable; the whole check
must scope none.
3. Scoped with --whole-unit (no per-mutant scope): every executable falls back to the whole campaign scope.
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
FIELDS = ("src", "func", "mutation", "class", "reach", "scope", "scope_reason", "scope_widened_by_reach", "stage", "reason")


def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel"""
    import atexit
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3], dir=os.environ.get("WWHD_VERIFY_SCRATCH"))
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)
    return d


def mutate(tmp, tag, whole, extra=()):
    env = dict(os.environ, WWHD_MUT_SCHEMA_CHECK_WHOLE="1" if whole else "0",
               WWHD_MUT_REACH_DUMP=os.path.join(tmp, tag + ".reach.json"),
               WWHD_MUT_SCHEMA_DUMP=os.path.join(tmp, tag + ".schema.json"))
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + BASE + list(extra) +
                       ["--receipts", os.path.join(tmp, tag)], cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout + r.stderr


def receipts(d):
    """id -> the compared fields; the schema executable as the group of mutants it holds (file numbering is unstable)"""
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


def check_lines(out):
    """the pass/drop messages of the check; executable names reduced to their source"""
    return sorted(re.sub(r"(schema-\S+?)-\d+ ", r"\1-N ", l) for l in out.splitlines()
                  if re.match(r"mutate\.py: (schema build|none of the|--reach-skip:|schema builds:)", l))


def stats(out):
    """(executables scoped, executables, row runs planned, row runs whole, row runs made, check seconds) or None"""
    m = re.search(r"schema check scope: (\d+) of (\d+) executable\(s\) on their members' functions, (\d+) row run\(s\) "
                  r"instead of (\d+)", out)
    t = re.search(r"schema check took ([\d.]+) s, (\d+) row run\(s\)", out)
    if not m or not t:
        return None
    return tuple(int(x) for x in m.groups()) + (int(t.group(2)), float(t.group(1)))


def main():
    tmp = private_build_dir()
    fails = []
    runs = {}
    for tag, whole, extra in (("whole", True, ()), ("scoped", False, ()), ("wholeunit", False, ("--whole-unit",))):
        rc, out = mutate(tmp, tag, whole, extra)
        st = stats(out)
        runs[tag] = (rc, out, receipts(os.path.join(tmp, tag)), st)
        print("mutate_schemascope_test: %s: rc %d, %d receipts, check %s" % (tag, rc, len(runs[tag][2]), st))
        if rc or len(runs[tag][2]) < 40 or st is None:
            fails.append("%s campaign (rc %d, %d receipts, stats %s): %s" % (tag, rc, len(runs[tag][2]), st, out[-400:]))
    if fails:
        for f in fails:
            print("FAIL", f)
        print("mutate_schemascope_test: FAIL")
        return 1
    (_, ow, rw, sw), (_, os_, rs, ss), (_, ou, _ru, su) = runs["whole"], runs["scoped"], runs["wholeunit"]
    gw, gs = (json.load(open(os.path.join(tmp, t + ".schema.json"))) for t in ("whole", "scoped"))
    if gw != gs:
        fails.append("schema executables differ: %s vs %s" % (gw, gs))
    if check_lines(ow) != check_lines(os_):
        fails.append("schema-check messages differ:\n  %s\nvs\n  %s" % ("\n  ".join(check_lines(ow)),
                                                                         "\n  ".join(check_lines(os_))))
    if sorted(rw) != sorted(rs):
        fails.append("receipt ids differ: %s vs %s" % (sorted(rw), sorted(rs)))
    for i in sorted(set(rw) & set(rs)):
        if rw[i] != rs[i]:
            fails.append("mutant %d: whole %s, scoped %s" % (i, rw[i], rs[i]))
    da, db = (open(os.path.join(tmp, t + ".reach.json")).read() for t in ("whole", "scoped"))
    if da != db or len(json.loads(da)) < 20:
        fails.append("reach records differ or are few (%d vs %d mutants)" % (len(json.loads(da)), len(json.loads(db))))
    # whole: nothing scoped, every row; scoped: some executables scoped, fewer row runs made
    if sw[0] != 0 or sw[2] != sw[3] or sw[4] != sw[3]:
        fails.append("whole check not on every row: %s" % (sw,))
    if ss[0] < 1 or not ss[4] < sw[4] or ss[4] != ss[2]:
        fails.append("scoped check made no fewer row runs (or not the planned ones): scoped %s, whole %s" % (ss, sw))
    if su[0] != 0 or su[4] != su[3]:
        fails.append("--whole-unit: the check did not fall back to every row: %s" % (su,))
    nclass = {}
    for v in rs.values():
        nclass[v[3]] = nclass.get(v[3], 0) + 1
    print("mutate_schemascope_test: %d receipts identical (classes %s), %d mutants with reach data identical; "
          "row runs whole %d, scoped %d (%d of %d executables scoped); check %.1f s vs %.1f s" % (
              len(rs), nclass, len(json.loads(db)), sw[4], ss[4], ss[0], ss[1], sw[5], ss[5]))
    for f in fails:
        print("FAIL", f)
    print("mutate_schemascope_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
