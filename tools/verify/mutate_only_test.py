#!/usr/bin/env python3
"""Self-test of `mutate.py --only ADDR,...` (test scope; 2026-10-21, JS-JParticle#3):
1. malformed --only lists stop the run;
2. --func outside the --only scope stops the run;
3. a scoped run on lib_mtx (--only PSMTXCopy 028E90D4) mutates only that function: every mutant's source line lies inside
   PSMTXCopy's body in mtx.cpp, and the run completes.
Exit 0 when all cases pass."""
import os
import shutil
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import mutate  # noqa: E402

FUNC = "028E90D4"
SRC = "wwhd_src/sdk/mtx/mtx.cpp"

def private_build_dir():
    """this test's own unit build directories (WWHD_VERIFY_SCRATCH, removed at exit), so the mutate self-tests can run
    in parallel (2026-10-26: two tests building lib2_sead_15 in build/verify at once broke a baseline)"""
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)



def body_lines():
    lines = open(os.path.join(ROOT, SRC)).read().splitlines()
    start = next(i for i, l in enumerate(lines, 1) if l.strip() == "/* %s */" % FUNC)
    end = next((i for i, l in enumerate(lines, 1) if i > start and re.match(r"^/\* [0-9A-F]{8} \*/$", l.strip())), len(lines) + 1)
    return start, end


def main():
    private_build_dir()  # parallel self-tests must not share build/verify/<unit>
    fails = []
    for bad in ("xyz", "028E90D4,", "1234567890"):
        r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib_mtx", "--only", bad, "--max", "1"],
                           cwd=ROOT, capture_output=True, text=True)
        if r.returncode == 0:
            fails.append("bad --only accepted: %r" % bad)
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib_mtx", "--only", FUNC, "--func", "028E9108", "--max", "1"],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode == 0 or "outside the --only" not in r.stdout + r.stderr:
        fails.append("--func outside the scope accepted")
    # population: mutants are generated with the source line in their description (" @mtx.cpp:LINE")
    src = open(os.path.join(ROOT, SRC)).read()
    lo, hi = body_lines()
    pop = []
    for addr, s, e in mutate.bodies(src) if hasattr(mutate, "bodies") else []:
        if addr == int(FUNC, 16):
            pop = [src.count("\n", 0, m[1]) + 1 for m in mutate.mutants(src, s, e)]
    if not pop or not all(lo <= ln < hi for ln in pop):
        fails.append("population of %s not inside its body (%d mutants, lines %d..%d)" % (FUNC, len(pop), lo, hi))
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib_mtx", "--only", FUNC, "--max", "4", "--n", "30"],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode != 0 or "mutants compiled" not in r.stdout:
        fails.append("scoped run failed:\n%s" % (r.stdout[-800:] + r.stderr[-800:]))
    for f in fails:
        print("FAIL", f)
    print("mutate_only_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
