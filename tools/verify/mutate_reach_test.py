#!/usr/bin/env python3
"""Self-test of mutate.py --reach-skip (2026-10-26, mutfast).

The same small campaign (lib2_sead_15, --func 02759928, --max 12, --seed 3) runs the old way and with --reach-skip:
every mutant ID must end killed / survived / broken / timeout the same way (receipts A vs B), the marker build must
give reach data for some mutants, and reach-skipped runs must carry -runlist and -stopfirst. Temporary dirs under
build/verify/_mutate_reach_test/ are removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_reach_test")
BASE = ["lib2_sead_15", "--func", "02759928", "--n", "100", "--max", "12", "--seed", "3", "-j", "2"]
KILLED = {"difference", "abort", "signal", "invalid"}

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


def classes(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = j
    return out


def coarse(c):
    return "killed" if c in KILLED else c


def main():
    private_build_dir()  # parallel self-tests must not share build/verify/<unit>
    os.makedirs(TMP, exist_ok=True)
    fails = []
    A, B = os.path.join(TMP, "A"), os.path.join(TMP, "B")
    try:
        rc, out = mutate("--receipts", A)
        ra = classes(A)
        if rc or len(ra) < 8:
            fails.append("old campaign (rc %d, %d receipts): %s" % (rc, len(ra), out[-300:]))
        rc, out = mutate("--receipts", B, "--reach-skip")
        rb = classes(B)
        if rc or sorted(rb) != sorted(ra):
            fails.append("--reach-skip campaign (rc %d, ids %s vs %s): %s" % (rc, sorted(rb), sorted(ra), out[-300:]))
        if "with reach data" not in out or " 0 mutant(s) with reach data" in out:
            fails.append("no reach data: " + out[-400:])
        skipped = 0
        for i in sorted(set(ra) & set(rb)):
            ca, cb = ra[i]["class"], rb[i]["class"]
            if coarse(ca) != coarse(cb):
                fails.append("mutant %d: %s without, %s with --reach-skip (%s)" % (i, ca, cb, ra[i]["mutation"]))
            cmd = rb[i].get("cmd") or []
            if "-runlist" in cmd and "-stopfirst" in cmd or rb[i].get("reach") == "not reached":
                skipped += 1
        if not skipped:
            fails.append("no mutant ran with a run list")
        # a declaration swap that does not compile and that no input reaches (gate finding 2026-10-26, lib2_agl_6 #30):
        # it must still be built, and so stay broken, not become a 'not reached' survivor
        for tag, extra in (("C", []), ("D", ["--reach-skip"])):
            r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib2_agl_6", "--n", "50", "--max", "400",
                                "--seed", "1", "--ids", "30", "-j", "2", "--receipts", os.path.join(TMP, tag)] + extra,
                               cwd=ROOT, capture_output=True, text=True)
        rc_, rd = classes(os.path.join(TMP, "C")), classes(os.path.join(TMP, "D"))
        if 30 not in rc_ or 30 not in rd or coarse(rc_[30]["class"]) != coarse(rd[30]["class"]):
            fails.append("lib2_agl_6 #30: %s without, %s with --reach-skip" % (rc_.get(30, {}).get("class"), rd.get(30, {}).get("class")))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_reach_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
