#!/usr/bin/env python3
"""Self-test of mutate.py --input-seed and --rerun (2026-10-25, AGL#7/AGL#18 requests).

Uses the merged spec lib2_sead_15, mutating only 02759928 (a small constructor).
1. A campaign with --seed 7 --receipts A: every run command carries `-seed 7` (the harness input seed follows --seed),
   and every receipt records input_seed 7.
2. --rerun A --rerun-class C (C = every class found in A) --input-seed 1 --receipts B reruns exactly the recorded IDs
   (same IDs, same mutated source text) with `-seed 1`, each receipt naming rerun_of A; the classes match campaign A
   for this deterministic unit.
3. --rerun with an empty class selection reruns nothing.
Writes temporary directories under build/verify/_mutate_rerun_test/ and removes them. Exit 0 when all cases pass.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_rerun_test")
UNIT, FUNC = "lib2_sead_15", "02759928"

def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel (2026-10-26: two tests building lib2_sead_15 in build/verify at once broke a baseline)"""
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)



def mutate(*args):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--func", FUNC, "--n", "100", "-j", "4"] + list(args),
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def receipts(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = (j, open(f[:-5] + ".cpp").read())
    return out


def main():
    private_build_dir()  # parallel self-tests must not share build/verify/<unit>
    os.makedirs(TMP, exist_ok=True)
    fails = []
    A, B, C = (os.path.join(TMP, x) for x in "ABC")
    try:
        rc, out = mutate("--seed", "7", "--max", "8", "--receipts", A)
        ra = receipts(A)
        if rc or not ra:
            fails.append("campaign A failed (rc %d): %s" % (rc, out[-400:]))
        for i, (j, _) in ra.items():
            if j.get("input_seed") != 7:
                fails.append("A mutant %d: input_seed %s" % (i, j.get("input_seed")))
            cmd = j.get("cmd")
            if cmd and cmd[cmd.index("-seed") + 1] != "7":
                fails.append("A mutant %d: run command %s" % (i, cmd))
        classes = sorted({j["class"] for j, _ in ra.values()})
        rc, out = mutate("--rerun", A, "--rerun-class", ",".join(classes), "--input-seed", "1", "--receipts", B)
        rb = receipts(B)
        if rc or sorted(rb) != sorted(ra):
            fails.append("rerun IDs %s != %s (rc %d): %s" % (sorted(rb), sorted(ra), rc, out[-400:]))
        for i in set(ra) & set(rb):
            ja, ta = ra[i]
            jb, tb = rb[i]
            if ta != tb or ja["mutation"] != jb["mutation"]:
                fails.append("mutant %d: rerun changed the variant" % i)
            if jb.get("rerun_of") != os.path.abspath(A) or jb.get("input_seed") != 1:
                fails.append("mutant %d: rerun receipt %s" % (i, {k: jb.get(k) for k in ("rerun_of", "input_seed")}))
            if jb.get("cmd") and jb["cmd"][jb["cmd"].index("-seed") + 1] != "1":
                fails.append("mutant %d: rerun command %s" % (i, jb["cmd"]))
            if ja["class"] != jb["class"]:
                fails.append("mutant %d: class %s at seed 7, %s at seed 1" % (i, ja["class"], jb["class"]))
        rc, out = mutate("--rerun", A, "--rerun-class", "nosuchclass", "--receipts", C)
        if rc or receipts(C) or "rerun of 0 recorded" not in out:
            fails.append("empty selection (rc %d): %s" % (rc, out[-300:]))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_rerun_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
