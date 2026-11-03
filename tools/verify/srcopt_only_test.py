#!/usr/bin/env python3
"""Self-test of verify.py -srcopt-only SRC (2026-10-25): only SRC is compiled at the -srcopt level (default O1),
every other unit source at O2, as mutate.py builds a mutant; it is the matched control for that TU's mutants.
Uses the merged unit lib2_sead_19 (several `src` files):
1. `-srcopt-only wwhd_src/sead/ChunkResources.cpp -only 027671F0 -n 300` passes; the build has
   ChunkResources.cpp.O1.o and an executable verify.O1-ChunkResources.cpp, and no other source got an .O1.o from
   this run.
2. A SRC that is not a `src` of the unit exits nonzero.
Exit 0 when both pass."""
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402  (unit_dir: the unit's build directory as verify.py resolves it, WWHD_VERIFY_SCRATCH-aware)
UNIT = "lib2_sead_19"
SRC = "wwhd_src/sead/ChunkResources.cpp"


def main():
    fails = []
    out = verify.unit_dir(UNIT)  # build/verify/<unit>, or its WWHD_VERIFY_SCRATCH directory
    t0 = time.time()
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-srcopt-only", SRC, "-only", "027671F0",
                        "-n", "300", "-shards", "1"], cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith("027671F0")]
    if r.returncode or not row or not row[0].endswith("ok"):
        fails.append("control run fails: %s" % (row or (r.stdout + r.stderr)[-400:]))
    if not os.path.exists(os.path.join(out, "ChunkResources.cpp.O1.o")):
        fails.append("no ChunkResources.cpp.O1.o")
    if not os.path.exists(os.path.join(out, "verify.O1-ChunkResources.cpp")):
        fails.append("no verify.O1-ChunkResources.cpp executable")
    for f in os.listdir(out):
        if f.endswith(".O1.o") and f != "ChunkResources.cpp.O1.o" and os.path.getmtime(os.path.join(out, f)) >= t0:
            fails.append("another source was rebuilt at O1: " + f)
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-srcopt-only", "wwhd_src/nonexistent.cpp",
                        "-n", "1"], cwd=ROOT, capture_output=True, text=True)
    if r.returncode == 0:
        fails.append("unknown source not rejected")
    for f in fails:
        print("FAIL", f)
    print("srcopt_only_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
