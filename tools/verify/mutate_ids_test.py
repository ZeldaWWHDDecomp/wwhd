#!/usr/bin/env python3
"""Self-test of mutate.py --ids (2026-10-26, AGL#17 receipt-free survivor replay).

Campaign A (lib2_sead_15, --func 02759928, --max 8, --seed 3, receipts) enumerates IDs 0..7. The same command with
`--ids` of a subset runs exactly those IDs: receipts B hold the same IDs with the same mutation text and source as A.
An ID outside the campaign exits with an error. Temporary dirs under build/verify/_mutate_ids_test/ are removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_ids_test")
BASE = ["lib2_sead_15", "--func", "02759928", "--n", "50", "--max", "8", "--seed", "3", "-j", "4"]

def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel (2026-10-26: two tests building lib2_sead_15 in build/verify at once broke a baseline)"""
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)



def mutate(*extra):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + BASE + list(extra), cwd=ROOT,
                       capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def rec(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = (j["mutation"], open(f[:-5] + ".cpp").read())
    return out


def main():
    private_build_dir()  # parallel self-tests must not share build/verify/<unit>
    os.makedirs(TMP, exist_ok=True)
    fails = []
    A, B = os.path.join(TMP, "A"), os.path.join(TMP, "B")
    try:
        rc, out = mutate("--receipts", A)
        ra = rec(A)
        if rc or len(ra) < 4:
            fails.append("campaign A (rc %d, %d receipts): %s" % (rc, len(ra), out[-300:]))
        pick = sorted(ra)[1::2]
        rc, out = mutate("--receipts", B, "--ids", ",".join(map(str, pick)))
        rb = rec(B)
        if rc or sorted(rb) != pick:
            fails.append("--ids ran %s, wanted %s (rc %d)" % (sorted(rb), pick, rc))
        for i in set(rb) & set(ra):
            if rb[i] != ra[i]:
                fails.append("ID %d differs between the campaign and --ids" % i)
        rc, out = mutate("--ids", "99")
        if rc == 0 or "not in this campaign" not in out:
            fails.append("out-of-range ID not rejected (rc %d)" % rc)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_ids_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
