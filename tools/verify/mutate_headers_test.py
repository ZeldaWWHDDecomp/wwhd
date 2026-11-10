#!/usr/bin/env python3
"""Self-test of mutate.py --headers (2026-11-01, TODO item 3 header part: inline functions in headers were never mutated).

1. header_inlines on a synthetic header: in-class methods, a constructor with a member-init list, a static inline member,
   an inline free function inside a namespace and one inside extern "C" are definitions; a declaration, a data member
   initialiser and an aggregate table are not.
2. A real campaign (lib2_agl_6, --max 4, --n 20, seed 7: the sampled header mutant is the compilable `+ -> -`) without and with --headers wwhd_src/include/agl/TimingHistory_l.h:
   - every mutant ID of the campaign without --headers is the same mutant (description, function, source) with the same
     class in the campaign with --headers (existing IDs and results unchanged);
   - the header mutants follow as new IDs, described "(inline NAME)", their receipts carry header_mutant (includers,
     other units) and the whole-campaign scope, and at least one of them compiles and runs (not broken), so the
     scratch-include build path works.
Temporary dirs in a private scratch directory, removed.
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
import mutate  # noqa: E402

HDR = """#pragma once
#include "x.h"
namespace ns {
struct A {
    int get() const { return v; }
    A(int x) : v(x), w(2) { init(); }
    void decl();
    int v = 0;
    static inline int twice(int k) { return k * 2; }
};
inline int freeFn(int a) {
    if (a) return 1;
    return 0;
}
static const int table[2] = {1, 2};
}
extern "C" { inline void cfn() { } }
"""
UNIT = "lib2_agl_6"
HEADER = "wwhd_src/include/agl/TimingHistory_l.h"


def recs(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = j
    return out


def main():
    fails = []
    got = [name for s, e, name in mutate.header_inlines(HDR)]
    want = ["get", "A", "twice", "freeFn", "cfn"]
    if got != want:
        fails.append("header_inlines %s, want %s" % (got, want))
    for s, e, name in mutate.header_inlines(HDR):
        if not HDR[e - 1] == "}":
            fails.append("%s does not end at its closing brace" % name)
    tmp = tempfile.mkdtemp(prefix="wwhd-mutate_headers_test-", dir=os.environ.get("WWHD_TEST_TMP"))
    os.environ["WWHD_VERIFY_SCRATCH"] = tmp
    try:
        base = [UNIT, "--n", "20", "--max", "4", "--seed", "7", "-j", "2"]
        for tag, extra in (("A", []), ("B", ["--headers", HEADER])):
            r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + base + ["--receipts", os.path.join(tmp, tag)] + extra,
                               cwd=ROOT, capture_output=True, text=True)
            if r.returncode:
                fails.append("campaign %s: rc %d %s" % (tag, r.returncode, (r.stdout + r.stderr)[-600:]))
        ra, rb = recs(os.path.join(tmp, "A")), recs(os.path.join(tmp, "B"))
        if not ra:
            fails.append("campaign A wrote no receipts")
        for i, ja in ra.items():
            jb = rb.get(i)
            if jb is None or any(ja[k] != jb[k] for k in ("mutation", "func", "src", "class")):
                fails.append("ID %d differs: %s vs %s" % (i, {k: ja.get(k) for k in ("mutation", "class")},
                                                         jb and {k: jb.get(k) for k in ("mutation", "class")}))
        import hashlib
        spec_path = os.path.join(HERE, "units", UNIT + ".txt")
        want_sha = hashlib.sha256(open(spec_path, "rb").read()).hexdigest()
        bad_sha = sorted(i for i, j in list(ra.items()) + list(rb.items()) if j.get("spec_sha256") != want_sha or j.get("spec") != UNIT)
        if bad_sha:  # the receipt names the spec text it ran against (this spec has no includes: the file itself)
            fails.append("receipts without the spec's sha256: %s" % bad_sha[:5])
        new = {i: j for i, j in rb.items() if i not in ra}
        if not new or (ra and min(new) != max(ra) + 1):
            fails.append("header mutants are not the IDs after the old campaign: old %s, new %s" % (sorted(ra), sorted(new)))
        for i, j in new.items():
            if "(inline " not in j["mutation"] or j["src"] != HEADER or "header_mutant" not in j:
                fails.append("ID %d is not a recorded header mutant: %s" % (i, {k: j.get(k) for k in ("mutation", "src")}))
            elif j["class"] != "broken" and j.get("scope") != "whole":
                fails.append("ID %d ran on scope %s, want the whole campaign scope" % (i, j.get("scope")))
        if new and all(j["class"] == "broken" for j in new.values()):
            fails.append("every header mutant is broken: %s" % [(j.get("stage"), (j.get("stderr") or "")[-200:]) for j in new.values()])
        # --rerun replays a recorded header mutant from its receipt (same class)
        hn = {i: j for i, j in new.items() if j["class"] != "broken"}
        if hn:
            r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--n", "20", "-j", "2", "--rerun",
                                os.path.join(tmp, "B"), "--rerun-class", ",".join(sorted({j["class"] for j in hn.values()})),
                                "--receipts", os.path.join(tmp, "C")], cwd=ROOT, capture_output=True, text=True)
            rc = recs(os.path.join(tmp, "C"))
            if r.returncode or any(rc.get(i, {}).get("class") != j["class"] for i, j in hn.items()):
                fails.append("rerun of header mutants: rc %d, classes %s vs %s %s" % (
                    r.returncode, {i: rc.get(i, {}).get("class") for i in hn}, {i: j["class"] for i, j in hn.items()},
                    (r.stdout + r.stderr)[-300:]))
        print("mutate_headers_test: old IDs %s, header mutants %s" % (
            sorted(ra), {i: (j["mutation"], j["class"]) for i, j in sorted(new.items())}))
    finally:
        shutil.rmtree(tmp, True)
    if fails:
        print("FAIL\n  " + "\n  ".join(fails))
        sys.exit(1)
    print("PASS")


if __name__ == "__main__":
    main()
