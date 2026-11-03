#!/usr/bin/env python3
"""Self-test of the scoped --rerun baseline (2026-10-31, fasttools2 change 2).

mutate.py --rerun runs its baseline gate (and with it the reference-side records and the setup snapshots the mutant runs
read) only on the union of the rerun mutants' per-mutant scopes. That is sound only if a function's rows and records do
not depend on which other functions ran before it in the same process (the 2026-10-29 dictionary fix); this test checks
that directly and then the campaign-level verdicts.

1. Harness level (lib2_sead_15 plus a `setup` + `setupsnapshot` on 02759564, so snapshots and validation states are
   written too; -n 600, seeds 1 and 7, -nulltrap): a full run and runs with -only on subsets (single functions, a pair,
   the last function, every second function) print byte-identical rows for their functions, and every record file the
   scoped run writes (ADDR.ref, ADDR-TAG.snap, world blob, .val) is byte-identical to the full run's file.
2. Campaign level: a main campaign on lib2_sead_15 (--max 40 --n 2, seed 1); its bounded-scope survivors of four functions
   and up to three of their kills are rerun (n 300, seed 1) with the scoped baseline (default) and with WWHD_MUT_RERUN_BASELINE_WHOLE=1 (the whole spec),
   plain and with --schemata --fork --reach-skip: same mutant IDs, classes and harness stdout per mutant; the scoped run
   says `k of N function(s)` with k < N and its receipts name `baseline_scope`, the whole run's do not. Adding one mutant
   whose scope is the whole spec makes the baseline whole again.
Work files in a new directory under the verify scratch (removed at the end when everything passed). Exit 0 when all pass.
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
sys.path.insert(0, HERE)
import verify  # noqa: E402
import specfile  # noqa: E402


def rows(out):
    return {l[:8]: l for l in out.splitlines() if len(l) > 10 and l[8:10] == "  " and all(c in "0123456789ABCDEF" for c in l[:8])}


def files(d):
    return {f: open(os.path.join(d, f), "rb").read() for f in sorted(os.listdir(d))}


def mutate(*args, env=None):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + list(args) + ["-j", "4", "--no-progress"],
                       cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout + r.stderr


def receipts(d):
    return {json.load(open(f))["id"]: json.load(open(f)) for f in glob.glob(os.path.join(d, "mutant*.json"))}


def main():
    fails = []
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="rerun_scope_test-", dir=base)
    print("mutate_rerun_scope_test: files in %s" % W)
    unit = "lib2_sead_15"
    exe = verify.build(unit, jobs=4)
    spec = specfile.spec_path(unit, ROOT, verify.unit_dir(unit))
    hdr = verify.header_args(unit)
    sp = os.path.join(W, "sead_snap.txt")
    open(sp, "w").write(open(spec).read() + "\nsetup 02759564 02759928 r3=0x5E000000\nsetupsnapshot\n")

    def run(args):
        r = subprocess.run([exe, "-spec", sp] + hdr + ["-noprogress", "-nulltrap", "-n", "600"] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace")
        return r.returncode, r.stdout, r.stderr

    # 1. harness level
    lst = subprocess.run([exe, "-spec", sp] + hdr + ["-list"], cwd=ROOT, capture_output=True, text=True).stdout.split()
    if len(lst) < 6:
        fails.append("harness -list: %s" % lst)
    subsets = [[lst[1]], ["02759564"], [lst[2], lst[-2]], [lst[-1]], lst[::2]]
    for seed in ("1", "7"):
        fd = os.path.join(W, "full-s" + seed)
        os.mkdir(fd)
        keys = ["-refcache-key", "k" * 32, "-snapcache-key", "s" * 32]
        rc, out, err = run(["-seed", seed, "-refcache-write", fd, "-snapcache-write", fd] + keys)
        full = rows(out)
        ff = files(fd)
        if rc or sorted(full) != sorted(lst) or not any(f.endswith(".snap") for f in ff) or not any(f.endswith(".val") for f in ff):
            fails.append("seed %s: full run rc %d, rows %s, files %s" % (seed, rc, sorted(full), sorted(ff)))
            continue
        for k, sub in enumerate(subsets):
            d = os.path.join(W, "sub%d-s%s" % (k, seed))
            os.mkdir(d)
            rc2, out2, _ = run(["-seed", seed, "-only", ",".join(sub), "-refcache-write", d, "-snapcache-write", d] + keys)
            got = rows(out2)
            if rc2 != 0 or sorted(got) != sorted(sub) or any(got[a] != full[a] for a in sub):
                fails.append("seed %s -only %s: rows differ from the full run:\n%s\n---\n%s" % (
                    seed, ",".join(sub), "\n".join(got.values()), "\n".join(full[a] for a in sub)))
            sf = files(d)
            refs = [f for f in sf if f.endswith(".ref")]
            if sorted(refs) != sorted(a + ".ref" for a in sub):
                fails.append("seed %s -only %s: record files %s" % (seed, ",".join(sub), sorted(sf)))
            for f, data in sf.items():
                if ff.get(f) != data:
                    fails.append("seed %s -only %s: %s differs from the full run's (or the full run has none)" % (
                        seed, ",".join(sub), f))
        print("  harness seed %s: %d subsets, rows and %d record file(s) compared" % (seed, len(subsets), len(ff)))

    # 2. campaign level
    main_dir = os.path.join(W, "main")
    rc, out = mutate(unit, "--max", "40", "--n", "2", "--seed", "1", "--nulltrap", "--receipts", main_dir)
    if rc:
        fails.append("main campaign: %s" % out[-400:])
    allr = receipts(main_dir)
    surv = [j for j in allr.values() if j["class"] == "survived" and j.get("scope") != "whole"]
    funcs = sorted({j["func"] for j in surv})
    pick = [j for j in surv if j["func"] in funcs[:4]]
    # kills too: a few `difference` receipts of the same functions (the rerun must kill them again in both modes)
    pick += sorted((j for j in allr.values() if j["class"] == "difference" and j.get("scope") != "whole"
                    and j["func"] in funcs[:4]), key=lambda j: j["id"])[:3]

    def rerun_dir(name, sel):
        d = os.path.join(W, name)
        os.makedirs(d)
        for j in sel:
            jj = dict(j, **{"class": "rerun"})
            json.dump(jj, open(os.path.join(d, "mutant%d.json" % j["id"]), "w"))
            shutil.copy(os.path.join(main_dir, "mutant%d.cpp" % j["id"]), d)
        return d
    rin = rerun_dir("in", pick)
    print("  campaign: %d mutants in %s rerun" % (len(pick), funcs[:4]))
    if len(pick) < 3:
        fails.append("too few mutants to rerun (%d)" % len(pick))
    # a mutant whose scope is the whole spec makes the baseline whole again
    wide = [j for j in allr.values() if j.get("scope") == "whole" and j["class"] in ("survived", "difference")][:1]
    if wide:
        rc, out = mutate(unit, "--rerun", rerun_dir("in-wide", pick + wide), "--rerun-class", "rerun", "--n", "50", "--seed",
                         "1", "--nulltrap", "--receipts", os.path.join(W, "wide"))
        if rc or "--rerun baseline scope: the whole campaign scope (mutant %d:" % wide[0]["id"] not in out:
            fails.append("a whole-scope mutant did not make the baseline whole: rc %d %s" % (rc, out[-300:]))
    else:
        print("  (no whole-scope mutant in the main campaign: fallback case not exercised)")
    for mode, extra in (("plain", []), ("fast", ["--schemata", "--fork", "--reach-skip"])):
        res = {}
        for how in ("scoped", "whole"):
            env = dict(os.environ)
            env["WWHD_MUT_RERUN_BASELINE_WHOLE"] = "1" if how == "whole" else "0"
            rd = os.path.join(W, "%s-%s" % (mode, how))
            rc, out = mutate(unit, "--rerun", rin, "--rerun-class", "rerun", "--n", "300", "--seed", "1", "--nulltrap",
                             "--receipts", rd, *extra, env=env)
            if rc:
                fails.append("%s %s: rc %d: %s" % (mode, how, rc, out[-400:]))
            res[how] = receipts(rd)
            line = [l for l in out.splitlines() if "--rerun baseline scope:" in l]
            if how == "scoped":
                if not line or "the union of the rerun mutants' scopes" not in line[0]:
                    fails.append("%s scoped: no scoped baseline: %s" % (mode, line))
                elif int(line[0].split("scope: ")[1].split()[0]) >= int(line[0].split(" of ")[1].split()[0]):
                    fails.append("%s scoped: the scope is not smaller than the spec: %s" % (mode, line[0]))
                if any("baseline_scope" not in j for j in res[how].values()):
                    fails.append("%s scoped: receipts without baseline_scope" % mode)
                print("  %s: %s" % (mode, line[0].split("mutate.py: ")[-1] if line else "?"))
            elif any("baseline_scope" in j for j in res[how].values()) or not line or "WWHD_MUT_RERUN_BASELINE_WHOLE" not in line[0]:
                fails.append("%s whole: the baseline was scoped: %s" % (mode, line))
        a, b = res["scoped"], res["whole"]
        if sorted(a) != sorted(b) or sorted(a) != sorted(j["id"] for j in pick) or \
                not any(j["class"] == "difference" for j in a.values()):
            fails.append("%s: receipt ids differ: %s / %s" % (mode, sorted(a), sorted(b)))
        for i in sorted(set(a) & set(b)):
            if (a[i]["class"], a[i].get("stdout"), a[i].get("scope")) != (b[i]["class"], b[i].get("stdout"), b[i].get("scope")):
                fails.append("%s mutant %d: scoped %s, whole %s" % (mode, i, a[i]["class"], b[i]["class"]))
        print("  %s: %d mutants, classes %s" % (mode, len(a), sorted({j["class"] for j in a.values()})))
    for x in fails:
        print("FAIL", x)
    if not fails:
        shutil.rmtree(W, ignore_errors=True)
    print("mutate_rerun_scope_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
