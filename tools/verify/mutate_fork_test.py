#!/usr/bin/env python3
"""Self-test of mutate.py --fork (2026-10-26, mutfast).

The same small campaign (lib2_sead_15, --func 02759928, --max 12, --seed 3) runs the old way and with --schemata --fork:
every mutant ID must end killed / survived / broken / timeout the same way (receipts A vs B), and most mutants must
have run as a fork of a schema executable's fork server (receipt field `fork`). ForkServer directly: a run past its
(tiny) timeout returns None and its child is killed, and the server still serves the next run; an unmutated run
passes. Temporary dirs under
build/verify/_mutate_fork_test/ are removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(ROOT, "build", "verify", "_mutate_fork_test")
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


def server_checks():
    """ForkServer on the unit's baseline harness (-forkserver works on any harness build)"""
    sys.path.insert(0, HERE)
    import mutate as M
    import specfile
    import verify
    out = []
    exe = os.path.join(verify.unit_dir("lib2_sead_15"), "verify")
    cmd = [exe, "-spec", specfile.spec_path("lib2_sead_15", ROOT, verify.unit_dir("lib2_sead_15")), "-n", "3000", "-seed", "1"] + \
        verify.header_args("lib2_sead_15")
    srv = M.ForkServer(cmd, TMP)
    try:
        if srv.run(-1, None, 0.001) is not None:
            out.append("a run past a 1 ms timeout was not reported as a timeout")
        r = srv.run(-1, None, 600)
        if r is None or r[0] != 0 or " ok" not in r[1]:
            out.append("unmutated run on the fork server after a timeout: %r" % (r and (r[0], r[1][-300:])))
    finally:
        srv.close()
    return out


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
        rc, out = mutate("--receipts", B, "--schemata", "--fork")
        rb = classes(B)
        if rc or sorted(rb) != sorted(ra):
            fails.append("--schemata --fork campaign (rc %d, ids %s vs %s): %s" % (rc, sorted(rb), sorted(ra), out[-300:]))
        if "schema builds:" not in out:
            fails.append("no schema build: " + out[-400:])
        inschema = 0
        for i in sorted(set(ra) & set(rb)):
            ca, cb = ra[i]["class"], rb[i]["class"]
            if coarse(ca) != coarse(cb):
                fails.append("mutant %d: %s without, %s with --schemata --fork (%s)" % (i, ca, cb, ra[i]["mutation"]))
            inschema += bool(rb[i].get("fork"))
        ran = sum(1 for x in ra.values() if x["class"] != "broken")
        if inschema * 2 < ran:
            fails.append("only %d of %d mutants ran on a fork server" % (inschema, ran))
        fails += server_checks()
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_fork_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
