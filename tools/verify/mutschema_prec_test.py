#!/usr/bin/env python3
"""Self-test: schema rewrites keep the textual mutant's parse (2026-10-28, SEAD#16 #1039).

`&&` binds tighter than `||`, so flipping one of them regroups its neighbours: the textual mutant of the first `&&` in
`a && b && c` is `a || b && c` = `a || (b && c)`, of the second `||` in `a || b || c` it is `a || b && c` as well.
The old logic rewrite parenthesised the old operator's operands and ran `(a || b) && c` / `(a || b) && c`: another
program than the recorded mutantN.cpp (SEAD#16 #1039 survived as a schema, its recorded source is killed).

A synthetic source with &&/|| chains of both kinds, mixed chains, chains next to `?:` / `=` / `return` / `,`,
bitwise and comparison operands inside them, equality/relational chains and +/- chains is enumerated with
mutate.mutants. Every mutant is compiled on its own (the textual mutant, the reference) and must print exactly the
same as its schema executable with WWHD_MUT_ACTIVE=K, over all inputs (every boolean combination, so the parses are
told apart). Every &&/|| and comparison/+ mutant must be in a schema (the test is not vacuous), and the original
prints the same with no mutant live. Operator swaps mutate.py makes: == != < <= > >= (within one precedence level),
&& <-> || (different levels: the case fixed here), + -> - (same level); no bitwise or shift swaps.
Temporary files under build/verify/_mutschema_prec_test/.
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
import mutschema_test  # noqa: E402  (STUB, CC)

TMP = os.path.join(ROOT, "build", "verify", "_mutschema_prec_test")
SRC = r'''#include <cstdio>
static int g(int v) { return v; }
static int f(int a, int b, int c, int d, int x, int y) {
    int r = 0;
    if (a && b && c) r += 1;
    if (a || b || c) r += 2;
    if (a && b || c && d) r += 4;
    if (a || b && c || d) r += 8;
    if (a && b && c || d && a && b) r += 16;
    r += (a && b && c) ? 32 : 0;
    r += a || b || c ? 64 : 0;
    int q = a || b && c;
    r += q * 128;
    r += g(a && b && c) * 256, r += g(c || d || a) * 512;
    if ((x & 1) == 0 && y < 3 || a) r += 1024;
    if (x | y && c || d == b) r += 2048;
    if (a ^ b && c || x > y && d) r += 4096;
    if (!a && b || c && !d) r += 8192;
    if ((a || b) && (c || d) && x) r += 16384;
    if (a == b != c) r += 32768;
    if (x < y == c) r += 65536;
    if (x <= y < c) r += 131072;
    r = r + x + y;
    r = r - x + y;
    r = r * x + y;
    return r;
}
static int h(int a, int b, int c) {
    return a && b || c && a || b;
}
int main() {
    long long s = 0;
    for (int m = 0; m < 16; m++)
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                s = s * 31 + f(m & 1, (m >> 1) & 1, (m >> 2) & 1, (m >> 3) & 1, x, y) + 7 * h(m & 1, (m >> 1) & 1, (m >> 2) & 1);
    printf("%lld\n", s);
    return 0;
}
'''


def run(cmd, env=None):
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env)


def main():
    fails = []
    CC = mutschema_test.CC
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(TMP)
    try:
        src = os.path.relpath(os.path.join(TMP, "probe.cpp"), ROOT)
        open(os.path.join(ROOT, src), "w").write(SRC)
        stub_o = os.path.join(TMP, "stub.o")
        open(os.path.join(TMP, "stub.cpp"), "w").write(mutschema_test.STUB)
        r = run(CC + ["-c", os.path.join(TMP, "stub.cpp"), "-o", stub_o])
        if r.returncode:
            raise SystemExit("stub: " + r.stderr)
        start = SRC.index("static int f(")
        end = SRC.index("int main()")
        ms = mutate.mutants(SRC, start, end)
        entries = [(k, m[0], m[1], m[2], m[3]) for k, m in enumerate(ms)]
        tr = lambda t: "#line 1 \"%s\"\n" % src + t  # noqa: E731

        def cc(cpp, obj):
            r = run(CC + ["-Werror=vla", "-ferror-limit=0", "-include", "tools/verify/src/mutschema.h", "-c", cpp, "-o", obj])
            return r.returncode, r.stderr
        objects, fallback = mutschema.build(src, SRC, entries, tr, cc, TMP, "schema", jobs=4)
        live = {}
        for obj, ks in objects:
            exe = obj[:-2]
            r = run(["clang++", "-o", exe, obj, stub_o])
            if r.returncode:
                fails.append("schema link: " + r.stderr[-400:])
                continue
            for k in ks:
                if not fallback.get(k, "").startswith(mutschema.MARK_PREFIX):
                    live[k] = exe

        def own(text, name):
            p = os.path.join(TMP, name + ".cpp")
            open(p, "w").write(tr(text))
            r = run(CC + ["-w", p, "-o", p[:-4]])
            return None if r.returncode else run([p[:-4]]).stdout
        orig = own(SRC, "orig")
        for exe in sorted(set(live.values())):
            got = run([exe], env=dict(os.environ, WWHD_MUT_ACTIVE="-1")).stdout
            if got != orig:
                fails.append("%s with no mutant live prints %r, the original %r" % (os.path.basename(exe), got, orig))
        nop = 0
        for k, m in enumerate(ms):
            isop = m[0].startswith("operator")
            nop += isop
            if k not in live:
                if isop:
                    fails.append("#%d %s not in a schema (%s)" % (k, m[0], fallback.get(k)))
                continue
            want = own(SRC[:m[1]] + m[3] + SRC[m[2]:], "m%d" % k)
            got = run([live[k]], env=dict(os.environ, WWHD_MUT_ACTIVE=str(k))).stdout
            if want is None:
                fails.append("#%d %s: in a schema but does not compile on its own" % (k, m[0]))
            elif got != want:
                ctx = SRC[SRC.rfind("\n", 0, m[1]) + 1:SRC.find("\n", m[2])].strip()
                fails.append("#%d %s in `%s`: schema prints %s, the textual mutant %s" % (
                    k, m[0], ctx, got.strip(), want.strip()))
        logic = sum(1 for m in ms if m[0].startswith(("operator &&", "operator ||")))
        if logic < 40:
            fails.append("only %d &&/|| mutants enumerated" % logic)
        print("schema: %d of %d mutants live (%d operator mutants, %d &&/||), %d schema objects" % (
            len(live), len(ms), nop, logic, len(objects)))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutschema_prec_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
