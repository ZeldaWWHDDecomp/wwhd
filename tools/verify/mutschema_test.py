#!/usr/bin/env python3
"""Self-test of mutant schemata (mutschema.py, src/mutschema.h; 2026-10-26).

A synthetic source with every site kind mutate.py makes (literals, comparisons incl. NULL/0 right operands and a
non-copyable guest-field type, + -> -, && / ||, drop !, drop statement incl. one-line if, swap) is enumerated with
mutate.mutants. Every mutant is compiled on its own (the old per-mutant way) and its output is the reference; the
schema object (all schema-able mutants in one TU, mutschema.build) must print exactly the same with WWHD_MUT_ACTIVE=K,
and the original output with no mutant live. Also: map_site, statement_ok, the marker recording (WWHD_MUT_REC=1)
names the sites that ran, a site that never runs is never recorded, and a site that does not compile (a literal in
an array bound) falls back instead of breaking the schema. Temporary files under build/verify/_mutschema_test/.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import mutate  # noqa: E402
import mutschema  # noqa: E402
import verify  # noqa: E402

TMP = os.path.join(ROOT, "build", "verify", "_mutschema_test")
SRC = r'''#include <cstdio>
#include <cstddef>
struct Field { unsigned raw; unsigned get() const { return raw; } operator unsigned() const { return raw; }
               Field(unsigned v) : raw(v) {} Field(const Field&) = delete; };
template <int N> static int mask(int v) { return v & (N - 1); }
static int f(int a, int b, int* p) {
    int r = 0;
    int t[4] = {1, 2, 3, 4};
    Field g(a + 5);
    if (a < b) r += 1;
    if (a == 3 && b != 4) r += 10;
    if (p == NULL) r += 100;
    if (p != 0 || a > 2) r += 1000;
    if (!p) r += 10000;
    r = r + a;
    r += 2;
    if (g >= 7u) r -= 5;
    if (b <= 1) r = r * 2;
    r ^= t[a & 3];
    r = r - b;
    if (a == 100) r = r + 7;
    r += mask<4>(a) > 2 ? 1 : 0;
    int u;
    if (a > 0) {
        u = a;
    } else {
        u = -a;
    }
    r += u;
    if (b == 2)
        r = r + 3;
    if (b == 3)
        r += 4;
    r += 1;
    return r;
}
int main() {
    long long s = 0;
    for (int a = -3; a < 5; a++)
        for (int b = -3; b < 6; b++) {
            int x = 0;
            s = s * 31 + f(a, b, (a & 1) ? &x : NULL);
        }
    printf("%lld\n", s);
    return 0;
}
'''
STUB = r'''#include <cstdlib>
#include <cstdio>
extern "C" { int wwhd_mut_active = -1; int wwhd_mut_recording = 0; static bool seen[4096];
void wwhd_mut_hit(int k) { if (k >= 0 && k < 4096 && !seen[k]) { seen[k] = true; fprintf(stderr, "HIT %d\n", k); } } }
__attribute__((constructor(101))) static void init() {
    if (const char* e = getenv("WWHD_MUT_ACTIVE")) wwhd_mut_active = atoi(e);
    if (const char* e = getenv("WWHD_MUT_REC")) wwhd_mut_recording = *e == '1';
}
'''
CC = ["clang++", "-std=c++20", "-O1"] + verify.FP


def run(cmd, env=None):
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env)


def main():
    fails = []
    # pure helpers
    if mutschema.map_site("abc 12 def", "abc 13 def", "12", "13") != 4 or mutschema.map_site("a", "b", "x", "y") is not None:
        fails.append("map_site")
    for c, want in [("if (a == NULL) return;", True), ("x = 3;", True), ("u32 x = 3;", False), ("auto f = g();", False),
                    ("if (x) { y; }", False), ("case 1: x = 2;", False), ("a->b = c;", True), ("if (x) y; else z;", False)]:
        if mutschema.statement_ok(c) != want:
            fails.append("statement_ok(%r) != %s" % (c, want))
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(TMP)
    try:
        src = os.path.relpath(os.path.join(TMP, "probe.cpp"), ROOT)
        open(os.path.join(ROOT, src), "w").write(SRC)
        stub_o = os.path.join(TMP, "stub.o")
        open(os.path.join(TMP, "stub.cpp"), "w").write(STUB)
        r = run(CC + ["-c", os.path.join(TMP, "stub.cpp"), "-o", stub_o])
        if r.returncode:
            raise SystemExit("stub: " + r.stderr)
        start = SRC.index("static int f(")
        end = SRC.index("int main()")
        ms = mutate.mutants(SRC, start, end)
        ms.append(("literal 4->5 (array bound)", SRC.index("int t[4]") + 6, SRC.index("int t[4]") + 7, "5"))
        kinds = {m[0].split(" ")[0] + " " + m[0].split(" ")[1] for m in ms}
        entries = [(k, m[0], m[1], m[2], m[3]) for k, m in enumerate(ms)]
        tr = lambda t: "#line 1 \"%s\"\n" % src + t  # noqa: E731

        def cc(cpp, obj):
            r = run(CC + ["-Werror=vla", "-ferror-limit=0", "-include", "tools/verify/src/mutschema.h", "-c", cpp, "-o", obj])
            return r.returncode, r.stderr
        objects, fallback = mutschema.build(src, SRC, entries, tr, cc, TMP, "schema", jobs=4)
        live, marks = {}, {}
        for obj, ks in objects:
            exe = obj[:-2]
            r = run(["clang++", "-o", exe, obj, stub_o])
            if r.returncode:
                fails.append("schema link: " + r.stderr[-400:])
                continue
            for k in ks:
                if fallback.get(k, "").startswith(mutschema.MARK_PREFIX):
                    marks[k] = exe  # a marker only: never live, recorded when its statement runs
                else:
                    live[k] = exe
        # reference: the original and every mutant compiled on its own
        def own(text, name):
            p = os.path.join(TMP, name + ".cpp")
            open(p, "w").write(tr(text))
            r = run(CC + ["-w", p, "-o", p[:-4]])
            if r.returncode:
                return None
            return run([p[:-4]]).stdout
        orig = own(SRC, "orig")
        for exe in set(live.values()):
            got = run([exe], env=dict(os.environ, WWHD_MUT_ACTIVE="-1")).stdout
            if got != orig:
                fails.append("%s with no mutant live prints %r, the original %r" % (os.path.basename(exe), got, orig))
        for k, m in enumerate(ms):
            want = own(SRC[:m[1]] + m[3] + SRC[m[2]:], "m%d" % k)
            if k not in live:
                if want is not None and "array bound" not in m[0] and not m[0].startswith(("drop statement `int", "drop statement `Field", "swap `int", "swap `Field")):
                    print("  note: #%d %s not in a schema (%s)" % (k, m[0], fallback.get(k)))
                continue
            got = run([live[k]], env=dict(os.environ, WWHD_MUT_ACTIVE=str(k))).stdout
            if want is None:
                fails.append("#%d %s: in a schema but does not compile on its own" % (k, m[0]))
            elif got != want:
                fails.append("#%d %s: schema prints %r, the mutant on its own %r" % (k, m[0], got, want))
        for k, m in enumerate(ms):  # template brackets stay out of the schema (their old mutants do not compile)
            if m[0].startswith(("operator <", "operator >")) and SRC[m[1] - 4:m[1]] == "mask" and k in live:
                fails.append("#%d %s (a template bracket) is in a schema" % (k, m[0]))
        for k, m in enumerate(ms):  # assignments to an uninitialised local: the mutant reads garbage (codegen-dependent)
            if m[0].startswith(("drop statement `u = a;", "drop statement `u = -a;")) and k in live:
                fails.append("#%d %s (assigns an uninitialised local) is in a schema" % (k, m[0]))
        for k, m in enumerate(ms):  # bodies of braceless ifs: the old mutant means something else (or does not compile)
            if m[0].startswith(("drop statement `r = r + 3", "drop statement `r += 4", "swap `r += 4")) and k in live:
                fails.append("#%d %s (a braceless if body) is in a schema" % (k, m[0]))
        for k, exe in marks.items():  # a marker never changes what runs
            got = run([exe], env=dict(os.environ, WWHD_MUT_ACTIVE=str(k))).stdout
            if got != orig:
                fails.append("marker #%d %s changes the output with WWHD_MUT_ACTIVE=%d" % (k, ms[k][0], k))
        if not any(ms[k][0].startswith(("drop statement `int u", "drop statement `u = a;", "swap `int", "drop statement `Field"))
                   for k in marks):
            fails.append("no marker-only site for a declaration or an uninitialised local: %s" % sorted(ms[k][0] for k in marks))
        bound = len(ms) - 1
        if bound in live or bound not in fallback:
            fails.append("the array-bound literal was not left to the per-mutant build")
        if len(live) < len(ms) // 2:
            fails.append("only %d of %d mutants in a schema" % (len(live), len(ms)))
        # marker recording: every hit is a site of that schema; the sites inside `r = r + 7` (a == 100) never run
        dead_lo = SRC.index("r = r + 7")
        dead = {k for k, m in enumerate(ms) if dead_lo <= m[1] < dead_lo + len("r = r + 7;")}
        for exe in set(live.values()):
            r = run([exe], env=dict(os.environ, WWHD_MUT_REC="1"))
            hits = {int(l.split()[1]) for l in r.stderr.splitlines() if l.startswith("HIT ")}
            mine = {k for k, e in list(live.items()) + list(marks.items()) if e == exe}
            if not hits or not hits <= mine:
                fails.append("recording of %s: hits %s, sites %s" % (os.path.basename(exe), sorted(hits), sorted(mine)))
            if hits & dead:
                fails.append("sites that never run were recorded: %s" % sorted(hits & dead))
            if not (mine - dead) <= hits:
                fails.append("sites that run were not recorded: %s" % sorted((mine - dead) - hits))
        if not dead & (set(live) | set(marks)):
            fails.append("no site of the dead statement is in a schema")
        print("schema: %d of %d mutants (kinds %s); fallback: %s" % (len(live), len(ms), ", ".join(sorted(kinds)),
                                                                     ", ".join(sorted(set(fallback.values())))))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutschema_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
