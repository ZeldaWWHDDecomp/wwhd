#!/usr/bin/env python3
"""Self-test of mutate.py's preamble helpers (2026-10-26, AGL#1: helpers above a source's first marked function were
never mutated).

preamble_helpers on a synthetic GameCube-style source: helperA (inside a namespace) is used only through helperB,
helperB by the first marked function, `unused` by none (left out); a struct's inline method and a global are not
helpers; regions start at the definition, not at `namespace`. Then a real campaign (lib2_nw_snd_5, --max 4): with
and without --no-preamble-helpers, IDs 0..3 are the same mutants (the old campaign keeps its IDs), and the helper
mutants follow as new IDs marked "(helper NAME)". Temporary dirs in a private build directory, removed.
"""
import glob
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402
import mutate  # noqa: E402

TMP = os.path.join(ROOT, "build", "verify", "_mutate_helpers_test")
SRC = """#include "x.h"
namespace ns {
static int helperA(int v) { return v + 1; }
}
struct S { int f() { return 2; } };
static int helperB(int v) {
    return ns::helperA(v) * 2;
}
static int unused(int v) { return v - 3; }
int g = 3;

/* 02000000 */
int f1(int a) { return helperB(a); }

/* 02000010 */
int f2(int a) { return a + 4; }

/* WWHD glue
*/
"""


def private_build_dir():
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-mutate_helpers_test-")
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)


def recs(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = j
    return out


def main():
    private_build_dir()
    fails = []
    got = {name: (SRC[s:e], ["%08X" % c for c in callers]) for s, e, name, callers in
           mutate.preamble_helpers(SRC, autoglue.bodies(SRC))}
    want = {"helperA": ["02000000"], "helperB": ["02000000"]}
    if {k: v[1] for k, v in got.items()} != want:
        fails.append("helpers %s, want %s" % ({k: v[1] for k, v in got.items()}, want))
    if "helperA" in got and not got["helperA"][0].startswith("static int helperA"):
        fails.append("helperA's region starts with %r" % got["helperA"][0][:30])
    os.makedirs(TMP, exist_ok=True)
    try:
        base = ["lib2_nw_snd_5", "--n", "20", "--max", "4", "--seed", "1", "-j", "2"]
        for tag, extra in (("A", ["--no-preamble-helpers"]), ("B", [])):
            r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + base + ["--receipts", os.path.join(TMP, tag)] + extra,
                               cwd=ROOT, capture_output=True, text=True)
            if r.returncode:
                fails.append("campaign %s: rc %d %s" % (tag, r.returncode, (r.stdout + r.stderr)[-300:]))
        a, b = recs(os.path.join(TMP, "A")), recs(os.path.join(TMP, "B"))
        if sorted(a) != [0, 1, 2, 3]:
            fails.append("old campaign IDs %s" % sorted(a))
        for i in a:
            if i not in b or b[i]["mutation"] != a[i]["mutation"]:
                fails.append("ID %d changed: %r -> %r" % (i, a[i]["mutation"], b.get(i, {}).get("mutation")))
        new = [b[i] for i in sorted(b) if i not in a]
        if not new or any("(helper " not in x["mutation"] for x in new):
            fails.append("new IDs %s" % [(x["id"], x["mutation"]) for x in new])
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_helpers_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
