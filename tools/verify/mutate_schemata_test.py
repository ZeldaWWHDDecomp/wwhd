#!/usr/bin/env python3
"""Self-test of mutate.py --schemata (2026-10-26, mutfast).

The same small campaign (lib2_sead_15, --func 02759928, --max 12, --seed 3) runs the old way and with --schemata:
every mutant ID must end killed / survived / broken / timeout the same way (receipts A vs B), and most mutants must
have run on a schema executable (receipt field `schema`) instead of their own build. Since 2026-10-31
every schema survivor is confirmed by its own plain build (receipt field `schema_survivor_rerun`). Temporary dirs under
build/verify/_mutate_schemata_test/ are removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_schemata_test")
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
        rc, out = mutate("--receipts", B, "--schemata")
        rb = classes(B)
        if rc or sorted(rb) != sorted(ra):
            fails.append("--schemata campaign (rc %d, ids %s vs %s): %s" % (rc, sorted(rb), sorted(ra), out[-300:]))
        if "schema builds:" not in out:
            fails.append("no schema build: " + out[-400:])
        inschema = confirmed = 0
        for i in sorted(set(ra) & set(rb)):
            ca, cb = ra[i]["class"], rb[i]["class"]
            if coarse(ca) != coarse(cb):
                fails.append("mutant %d: %s without, %s with --schemata (%s)" % (i, ca, cb, ra[i]["mutation"]))
            inschema += bool(rb[i].get("schema") or rb[i].get("schema_survivor_rerun"))
            if cb == "survived" and rb[i].get("schema"):  #: a schema survivor is confirmed by its own build
                fails.append("mutant %d: schema survivor without the plain confirmation run" % i)
            confirmed += bool(rb[i].get("schema_survivor_rerun"))
        ran = sum(1 for x in ra.values() if x["class"] != "broken")
        if any(x["class"] == "survived" for x in rb.values()) and not confirmed:
            fails.append("survivors but no plain confirmation run")
        if inschema * 2 < ran:
            fails.append("only %d of %d mutants ran on a schema executable" % (inschema, ran))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_schemata_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
