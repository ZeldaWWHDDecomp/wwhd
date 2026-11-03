#!/usr/bin/env python3
"""Self-test of the per-mutant test scope (tools/verify/mutscope.py, mutate.py default since 2026-10-27):
1. changed_span finds the one replaced span (also for swaps and pure insertions/deletions);
2. a GameCube-style (glue) source: a change in a marked function's body -> that function only; in a helper -> every
   marked function that names it, also through other helpers; a helper named in a file-scope initialiser, a
   constructor body, code outside any function, a function listed `real` or used by a `setup` -> the whole unit;
   a host static the changed code touches -> every function naming it; a glue `data` view is guest memory (no widening);
3. a WWHD_FUNC-style source: code after WWHD_FUNC(...); -> that function; code in front of it -> the whole unit;
4. mutate.py on lib_mtx: receipts carry `scope` (a list of functions by default, "whole" with --whole-unit), and the
   harness command of a scoped mutant has -only with exactly its function and -stopfirst (default since 2026-10-27).
Exit 0 when all cases pass."""
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
import mutscope  # noqa: E402

GLUE = """#include "nothing.h"

static int sHits;
static int sTable;
static int helperCount(int x) { return x + 1; }
static int helperWrap(int x) { return helperCount(x) * 2; }
static int helperStart(int x) { return x - 1; }
static int sStartup = helperStart(3);

/* 02000100 */
int Foo::first(int x) {
    sHits = x;
    return helperCount(x) + sTable;
}

/* 02000200 */
int Foo::second(int x) {
    return helperWrap(x);
}

/* 02000300 */
int Foo::third(int x) {
    return sHits + 7;
}

/* 02000400 */
Foo::Foo() : mValue(5) {
    mOther = 6;
}

/* 02000500 */
int Foo::fifth(int x) {
    return x * 9;
}

/* WWHD glue (tools/verify/autoglue.py)
data sTable 10000000
*/
"""

PLAIN = """#include "nothing.h"

static int plainHelper(int x) { return x + 3; }

int plainOne(int x) {
    int pre = 4;
    WWHD_FUNC(0x02000600, int, x);
    return plainHelper(x) + pre;
}
VERIFY(0x02000600, plainOne);

int plainTwo(int x) {
    WWHD_FUNC(0x02000700, int, x);
    return x + 11;
}
VERIFY(0x02000700, plainTwo);
"""


def at(text, needle, k=0):
    i = text.index(needle)
    return i + k, i + len(needle)


def main():
    fails = []

    def check(what, got, want):
        if got != want:
            fails.append("%s: got %r, want %r" % (what, got, want))

    check("span", mutscope.changed_span("abcXdef", "abcYYdef"), (3, 4, 5))
    check("span insert", mutscope.changed_span("aaa", "aaaa"), (3, 3, 4))
    check("span equal", mutscope.changed_span("same", "same"), None)
    a, b = "x = 1;\ny = 2;\n", "y = 2;\nx = 1;\n"
    check("span swap", mutscope.changed_span(a, b), (0, len(a) - 2, len(b) - 2))

    d = tempfile.mkdtemp(prefix="mutscope-test-")
    try:
        os.makedirs(os.path.join(d, "src"))
        open(os.path.join(d, "src", "glue.cpp"), "w").write(GLUE)
        open(os.path.join(d, "src", "plain.cpp"), "w").write(PLAIN)
        srcs = ["src/glue.cpp", "src/plain.cpp"]
        sc = mutscope.Scope(srcs, ["src src/glue.cpp src/plain.cpp"], d)

        def aff(src, needle, text, k=0):
            s, e = at(text, needle, k)
            r, why = sc.affected(src, s, e)
            return r if r is not None else "whole"

        G = "src/glue.cpp"
        check("own body", aff(G, "return x * 9;", GLUE), {0x02000500})
        check("helper", aff(G, "return x + 1;", GLUE), {0x02000100, 0x02000200})
        check("helper via helper", aff(G, "* 2;", GLUE), {0x02000200})
        check("helper run by a static initialiser", aff(G, "return x - 1;", GLUE), "whole")
        check("host static written", aff(G, "sHits = x;", GLUE), {0x02000100, 0x02000300})
        check("host static read", aff(G, "return sHits + 7;", GLUE), {0x02000100, 0x02000300})
        check("constructor body", aff(G, "mOther = 6;", GLUE), "whole")
        check("constructor init list", aff(G, "mValue(5)", GLUE), "whole")
        check("outside any function", aff(G, "static int sStartup", GLUE), "whole")
        check("host statics found", sorted(sc.host_statics), ["sHits", "sStartup"])
        P = "src/plain.cpp"
        check("after WWHD_FUNC", aff(P, "return x + 11;", PLAIN), {0x02000700})
        check("before WWHD_FUNC", aff(P, "int pre = 4;", PLAIN), "whole")
        check("plain helper", aff(P, "return x + 3;", PLAIN), {0x02000600})
        # Since 2026-10-30: real/setup targets run the original outside their own row, so a change in the
        # function's own checked body scopes to it even when it is listed real/setup
        sc2 = mutscope.Scope(srcs, ["real 02000500"], d)
        r, why = sc2.affected(G, *at(GLUE, "return x * 9;"))
        check("real-listed function, own body", r, {0x02000500})
        sc3 = mutscope.Scope(srcs, ["setup 02000100 02000200"], d)
        r, why = sc3.affected(G, *at(GLUE, "* 2;"))
        check("setup target, helper (kept conservative)", r, None)
        sc3b = mutscope.Scope(srcs, ["setup 02000100 02000500"], d)
        r, why = sc3b.affected(G, *at(GLUE, "return x * 9;"))
        check("setup target, own body", r, {0x02000500})
        sc3c = mutscope.Scope(srcs, ["real 02000300"], d)
        r, why = sc3c.affected(G, *at(GLUE, "sHits = x;"))
        check("own body whose host static reaches a real-listed function", r, None)
    finally:
        shutil.rmtree(d, True)

    # 4. mutate.py end to end (own build directory, removed at exit)
    vd = tempfile.mkdtemp(prefix="wwhd-mutscope_test-")
    rd = tempfile.mkdtemp(prefix="wwhd-mutscope_test-rc-")
    try:
        env = dict(os.environ, WWHD_VERIFY_SCRATCH=vd)
        for flag, want in (([], "list"), (["--whole-unit"], "whole")):
            rec = os.path.join(rd, want)
            r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "lib_mtx", "--max", "6", "--n", "20",
                                "--receipts", rec, "--no-progress"] + flag, cwd=ROOT, capture_output=True, text=True, env=env)
            if r.returncode != 0 or "mutants compiled" not in r.stdout:
                fails.append("mutate.py %s failed:\n%s" % (flag, r.stdout[-800:] + r.stderr[-800:]))
                continue
            ran = [json.load(open(f)) for f in glob.glob(os.path.join(rec, "*.json"))]
            ran = [j for j in ran if "cmd" in j]
            if not ran:
                fails.append("mutate.py %s: no mutant ran" % flag)
            for j in ran:
                sc_ = j.get("scope")
                if want == "whole":
                    if sc_ != "whole" or j.get("scope_reason") != "--whole-unit":
                        fails.append("--whole-unit receipt %d scope %r" % (j["id"], sc_))
                elif sc_ != "whole":
                    only = j["cmd"][j["cmd"].index("-only") + 1].split(",") if "-only" in j["cmd"] else None
                    if "-stopfirst" not in j["cmd"]:
                        fails.append("receipt %d: no -stopfirst in %r" % (j["id"], j["cmd"]))
                    if j["func"] not in sc_ or only != sc_:
                        fails.append("receipt %d: func %s scope %r -only %r" % (j["id"], j["func"], sc_, only))
            if want == "list" and not any(j.get("scope") != "whole" for j in ran):
                fails.append("no scoped mutant in the default run")
    finally:
        shutil.rmtree(vd, True)
        shutil.rmtree(rd, True)
    for f in fails:
        print("FAIL", f)
    print("mutscope_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
