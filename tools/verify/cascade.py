#!/usr/bin/env python3
"""Shared two-stage survivor cascade (2026-10-30; replaces the earlier per-unit copies, e.g. AGL#19
a local scratch file and AGL#7 build/agl7w/cascade3.py).

A main campaign leaves survivors; each companion spec (a fixture that reaches other paths of the same functions) may kill
some of them. The cascade reruns the still-alive mutants (their recorded mutantN.cpp, same IDs: mutate.py SPEC --rerun DIR)
on every companion whose scope holds them (the mutant's source is a `src` of the spec and, when the spec header has
`-only`, its function is listed), in the given order, and a mutant killed on one companion leaves the cascade.

Single stage (default until): every alive mutant on every in-scope companion at --n2 inputs (default 1000).
Two stages (--two-stage):
  stage 1: every alive mutant on every in-scope companion at --n1 inputs (default 100);
  stage 2: the stage-1 survivors on the same companions, in the same order, at --n2 inputs (default 1000).
The generated inputs of a smaller n are the first inputs of a larger one (same input seed), and runs stop at the first
difference, so a stage-1 kill is also a kill at n2: the final alive set is the one a single-stage cascade at n2 leaves
(only the companion that kills first can differ). Two-stage pays off only once the schemata/reach-skip mode ends the per-pass rebuilds (each
mutate.py pass compiles every mutant again: on AGL#19/AGL#7 companions two-stage took 270-333 s vs 210-223 s single-stage,
2026-10-30) or on specs with long n2 runs. `--single-stage` is accepted as a no-op alias of the default. Not reached on a
spec is not used yet: stage 2 still runs every in-scope companion.

Kill rule: a receipt of class `difference` kills when at least one
failing row is a clean difference or a CANDIDATE-only page-0 DSI / instruction-fetch fault; rows with original-only faults,
faults on both sides and aborts never kill (such a receipt counts as `difference-page0`, the mutant stays alive). Classes
`survived`, `abort`, `timeout`, `signal`, `invalid`, `equivalent-identical-code` never kill.

Files (OUT):
  alive.json             {id: {func, src, mutation, json, hist: [[stage, spec, kind], ...], killed_by: [stage, spec] | null}}
                         rewritten after every pass (atomic); `hist` is the per-mutant history
  progress.json          the finished (stage, spec) passes: a rerun of the same command resumes after the last one
  stage1/, stage2/       in-SPEC/ (the --rerun input: mutantN.json with class `rerun` + mutantN.cpp), rc-SPEC/ (mutate.py
                         receipts) and SPEC.log per pass
Load gate: a pass starts only while the 1-minute load is below --load-max (default 90); with --slot-dir DIR it also holds
--slots of the job-slot files in DIR (flock, as the helpers' slot.py) for the duration of the mutate.py run.
--fast: every pass runs mutate.py with --schemata --fork --reach-skip (one schema executable per source from the
recorded texts, fork servers, reach markers); a mutant no input of a companion reaches is recorded `not-reached` there.
Optional speed-ups (opt-in, they cost disk): --baseline-cache DIR (: each companion baseline once per n),
--refcache-from DIR (: a gate's original-side records), --prefetch (run the companions' baselines up front,
basecache.py prefetch; needs --baseline-cache).
--timeout N (2026-10-31): a fixed per-mutant limit for every pass (mutate.py --timeout N). Without it each pass derives its
limit once from its own baseline (10x, scaled up with the load) and never re-runs the baseline to measure again.
--sweep-stale-refcache (2026-11-10): forwarded to every mutate.py rerun pass (off by default); mutate.py then removes
dead-PID refcache-<spec>-* directories of the pass's spec in its own scratch directory below WWHD_MUT_SWEEP_ROOT, each
removal logged in the pass's SPEC.log (mutate.py's own limits; it refuses the flag without WWHD_MUT_SWEEP_ROOT).

usage: cascade.py --receipts MAIN[,EXCLUDE...] --out OUT --input-seed S [--root TREE] [--classes C,...] [--n1 100]
                  [--n2 1000] [--two-stage] [--fast] [-j 4] [--load-max 90] [--slot-dir DIR --slots K] [--from-alive PREV]
                  [--baseline-cache DIR] [--refcache-from DIR] [--prefetch] [--timeout N]
                  [--sweep-stale-refcache] SPEC...
MAIN: receipts of the main campaign; EXCLUDE: receipts whose `difference` kills (by the kill rule) are left out. --from-alive
PREV: start from what an earlier cascade (e.g. seed 1 for a seed-7 cascade) left alive. Self-test: cascade_test.py.
"""
import argparse
import fcntl
import glob
import json
import os
import re
import shutil
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROW = re.compile(r"^[0-9A-F]{8}\s")
ALIVE_CLASSES = "survived,abort,timeout,signal,invalid,difference-page0,difference-unproven"


AB_NOTE = " (some inputs aborted on both sides)"  # harness.cpp row suffix: other inputs of the function aborted on both
# sides; it says nothing about the row's own (first) difference, so the kill rule must not read it


P0_MARK = "[original page-0 fault on this input: no evidence]"


def row_text(l):
    """a verify row without the both-sides-abort suffix"""
    l = l.rstrip()
    return l[:-len(AB_NOTE)] if l.endswith(AB_NOTE) else l


def failing_rows(j):
    return [row_text(l) for l in (j.get("stdout") or "").split("\n") if ROW.match(l) and not row_text(l).endswith(" ok")]


def row_kills(r):
    r = row_text(r)
    if P0_MARK in r:  # the shown difference is on an input where the original faulted on page 0 (harness, zero-page rule)
        return False
    if "by the candidate only" in r:
        return True
    if "page-0" in r or "abort" in r.lower() or "faulted" in r:
        return False
    return True


def ran_nulltrap(j):
    """the receipt's harness command ran with -nulltrap (a run without it cannot tell a difference on an input where the
    original itself read page 0 from a clean one: such rows carry no page-0 marker)"""
    cmd = j.get("cmd")
    if isinstance(cmd, str):
        cmd = cmd.split()
    return bool(cmd) and "-nulltrap" in cmd


def kind(j):
    """the receipt's class under the kill rule: `difference` only for a kill, `difference-page0` when every failing row is
    a page-0/abort/fault row, `difference-unproven` when the run had no -nulltrap (: then a page-0 input of the
    original looks like an ordinary difference, so the receipt proves no kill)"""
    c = j.get("class")
    if c == "difference" and not ran_nulltrap(j):
        return "difference-unproven"
    if c == "difference":
        rows = failing_rows(j)
        if rows and not any(row_kills(r) for r in rows):
            return "difference-page0"
    return c


def expand(units, path):
    out = []
    for l in open(path):
        f = l.split("#")[0].split()
        if f and f[0] == "include":
            out += expand(units, os.path.join(units, f[1]))
        else:
            out.append(l)
    return out


def scope_of(units, spec):
    """(set of src files, set of -only functions or None)"""
    path = os.path.join(units, spec + ".txt")
    only = None
    for l in open(path):
        m = re.match(r"#\s*verify:.*-only ([0-9A-Fa-f,]+)", l)
        if m:
            only = {x.upper() for x in m.group(1).split(",") if x}
            break
    srcs = set()
    for l in expand(units, path):
        f = l.split("#")[0].split()
        if f and f[0] == "src":
            srcs.update(f[1:])
    return srcs, only


def save(path, data):
    tmp = path + ".tmp"
    json.dump(data, open(tmp, "w"), indent=0, sort_keys=True)
    os.replace(tmp, path)


def wait_load(limit):
    while os.getloadavg()[0] >= limit:
        time.sleep(20)


def hold_slots(d, k):
    """k flocked job slots in d (8 slot files s0..s7, as the helpers' slot.py); returns the open files"""
    while True:
        got = []
        for i in range(8):
            f = open(os.path.join(d, "s%d" % i), "a")
            try:
                fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
                got.append(f)
                if len(got) == k:
                    return got
            except OSError:
                f.close()
        for f in got:
            f.close()
        time.sleep(5)


def load_alive(a):
    alive = {}
    mains = a.receipts.split(",")
    dead = set()
    for d in mains[1:]:
        for f in glob.glob(os.path.join(d, "mutant*.json")):
            j = json.load(open(f))
            if kind(j) == "difference":
                dead.add(str(j["id"]))
    classes = set(a.classes.split(","))
    for f in glob.glob(os.path.join(mains[0], "mutant*.json")):
        j = json.load(open(f))
        i = str(j["id"])
        if kind(j) in classes and i not in dead and os.path.exists(f[:-5] + ".cpp"):
            alive[i] = {"func": j["func"].upper(), "src": j["src"], "mutation": j.get("mutation"), "json": os.path.abspath(f),
                        "hist": [["main", "main", kind(j)]], "killed_by": None}
    if a.from_alive:
        prev = json.load(open(a.from_alive))
        for i, v in prev.items():
            if v.get("killed_by"):
                alive.pop(i, None)
    return alive


def main():
    import verify as _scratch_verify  # noqa: E402  (same directory)
    _scratch_verify.check_scratch_env('cascade.py')  # a set scratch variable naming a missing/unwritable folder stops here, nothing written
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--receipts", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--input-seed", required=True)
    ap.add_argument("--root", default=os.path.abspath(os.path.join(HERE, "..", "..")))
    ap.add_argument("--classes", default=ALIVE_CLASSES)
    ap.add_argument("--n1", type=int, default=100)
    ap.add_argument("--n2", type=int, default=1000)
    ap.add_argument("--two-stage", action="store_true", help="stage 1 at --n1, then the survivors at --n2")
    ap.add_argument("--single-stage", action="store_true", help="the default (accepted as a no-op alias)")
    ap.add_argument("-j", default="4")
    ap.add_argument("--load-max", type=float, default=90.0)
    ap.add_argument("--slot-dir")
    ap.add_argument("--slots", type=int, default=4)
    ap.add_argument("--from-alive")
    ap.add_argument("--baseline-cache")
    ap.add_argument("--refcache-from")
    ap.add_argument("--prefetch", action="store_true")
    ap.add_argument("--timeout", type=int, help="seconds per mutant run, passed to every mutate.py pass (fixed: no retry, no "
                    "measurement). Default: mutate.py's own, max(60, 10x the pass's unmutated baseline wall time) scaled up with "
                    "the load and never measured again (2026-10-31). A timeout never kills")
    ap.add_argument("--fast", action="store_true", help="mutate.py --schemata --fork --reach-skip on every pass: one "
                    "schema executable per source instead of a build per mutant, fork servers, and a mutant whose site no "
                    "input of the companion reaches is `not reached` there (not run, stays alive)")
    ap.add_argument("--sweep-stale-refcache", action="store_true", help="forward --sweep-stale-refcache to every "
                    "mutate.py rerun pass (off by default; mutate.py's limits apply)")
    ap.add_argument("specs", nargs="+")
    a = ap.parse_args()
    units = os.path.join(a.root, "tools", "verify", "units")
    os.makedirs(a.out, exist_ok=True)
    ap_path, pg_path = os.path.join(a.out, "alive.json"), os.path.join(a.out, "progress.json")
    alive = load_alive(a)
    if os.path.exists(ap_path):  # resume: the recorded histories
        for i, v in json.load(open(ap_path)).items():
            if i in alive:
                alive[i] = v
    done = [tuple(x) for x in json.load(open(pg_path))] if os.path.exists(pg_path) else []
    stages = [(1, a.n1), (2, a.n2)] if a.two_stage else [(2, a.n2)]
    scopes = {s: scope_of(units, s) for s in a.specs}
    common = ["--input-seed", str(a.input_seed), "--nulltrap", "-j", str(a.j), "--no-progress"]
    if a.baseline_cache:
        common += ["--baseline-cache", os.path.abspath(a.baseline_cache)]
    if a.refcache_from:
        common += ["--refcache-from", os.path.abspath(a.refcache_from)]
    if a.fast:
        common += ["--schemata", "--fork", "--reach-skip"]
    if a.timeout:
        common += ["--timeout", str(a.timeout)]
    if a.prefetch and a.baseline_cache:
        for st, n in stages:
            subprocess.call([sys.executable, os.path.join(a.root, "tools", "verify", "basecache.py"), "prefetch", "--cache",
                             os.path.abspath(a.baseline_cache), "--load-max", str(a.load_max)] + list(a.specs) +
                            ["--", "--n", str(n)] + common, cwd=a.root)
    t_all = time.time()
    for st, n in stages:
        for spec in a.specs:
            if (st, spec) in done:
                continue
            srcs, only = scopes[spec]
            pick = sorted((i for i, v in alive.items() if not v["killed_by"] and v["src"] in srcs and
                           (only is None or v["func"] in only)), key=int)
            tag = spec.replace("/", "__")
            sd = os.path.join(a.out, "stage%d" % st)
            if not pick:
                print("stage %d %s: nothing in scope" % (st, spec), flush=True)
                done.append((st, spec))
                save(pg_path, done)
                continue
            d = os.path.join(sd, "in-" + tag)
            os.makedirs(d, exist_ok=True)
            for i in pick:
                jj = json.load(open(alive[i]["json"]))
                jj["class"] = "rerun"
                json.dump(jj, open(os.path.join(d, "mutant%s.json" % i), "w"))
                shutil.copy(alive[i]["json"][:-5] + ".cpp", os.path.join(d, "mutant%s.cpp" % i))
            r = os.path.join(sd, "rc-" + tag)
            cmd = [sys.executable, os.path.join(a.root, "tools", "verify", "mutate.py"), spec, "--rerun", d, "--rerun-class",
                   "rerun", "--n", str(n), "--receipts", r] + common
            if a.sweep_stale_refcache:  # rerun passes only: basecache.py prefetch does not take it
                cmd.append("--sweep-stale-refcache")
            wait_load(a.load_max)
            slots = hold_slots(a.slot_dir, a.slots) if a.slot_dir else []
            t0 = time.time()
            rc = subprocess.call(cmd, cwd=a.root, stdout=open(os.path.join(sd, tag + ".log"), "w"), stderr=subprocess.STDOUT)
            for f in slots:
                f.close()
            k = seen = 0
            for f in glob.glob(os.path.join(r, "mutant*.json")):
                j = json.load(open(f))
                i = str(j["id"])
                if i not in alive or i not in pick:
                    continue
                kd = kind(j)
                if kd == "survived" and j.get("reach") == "not reached":
                    kd = "not-reached"  # --fast: no input of this companion runs the site (not run here, stays alive)
                alive[i]["hist"].append([st, spec, kd])
                seen += 1
                if kd == "difference" and not alive[i]["killed_by"]:
                    alive[i]["killed_by"] = [st, spec]
                    k += 1
            save(ap_path, alive)
            if rc == 0 and seen == len(pick):  # a pass that did not finish is run again on resume
                done.append((st, spec))
                save(pg_path, done)
            print("stage %d %s rc %d: %d in scope, %d rerun, %d killed, %.0fs" % (st, spec, rc, len(pick), seen, k,
                                                                                  time.time() - t0), flush=True)
    save(ap_path, alive)  # also when no pass ran (every companion out of scope): a --from-alive cascade reads it
    left = sum(1 for v in alive.values() if not v["killed_by"])
    print("cascade: %d of %d alive after %s, %.0fs" % (left, len(alive), "stage 2" if a.two_stage else "the single stage",
                                                      time.time() - t_all), flush=True)


if __name__ == "__main__":
    main()
