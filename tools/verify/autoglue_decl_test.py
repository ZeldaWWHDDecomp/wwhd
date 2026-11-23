#!/usr/bin/env python3
"""Self-test of autoglue's local declarations (2026-11-19, d_a_msw.cpp:22): a guest-layout local declared with
several declarators (`cXyz a, b;`) must become one gabi::LocalOf per declarator, as `cXyz a;` does; before, the line
stayed two host objects at host addresses (read as guest memory), and `T e = a, g;` gave `LocalOf<T> e(a, g)`.
Pointers / references stay host declarations (const kept); an array keeps its host declaration behind a static_assert
that fails the build for a class element type; members of a local struct stay as they are; commas inside a call's parentheses (also after `->`) do not split.
Exit 0 when all pass."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

SRC = """/* 02000000 */
void f() {
    cXyz a;
    cXyz b, c(1.0f, 2.0f, 3.0f);
    const cXyz e = a, g;
    const cXyz *p, *q;
    cXyz d[2], h;
    INT32 w[8];
    T *slot = find(e->x, u32(c));
    int i, j;
    struct Positions {
        cXyz point[2];
        cXyz u, v;
        cXyz one;
    };
}

/* WWHD glue (tools/verify/autoglue.py)
*/
"""

EXPECT = [
    "gabi::LocalOf<cXyz> a__s; [[maybe_unused]] cXyz& a = a__s.ref();",
    "gabi::LocalOf<cXyz> b__s; [[maybe_unused]] cXyz& b = b__s.ref(); "
    "gabi::LocalOf<cXyz> c__s(1.0f, 2.0f, 3.0f); [[maybe_unused]] cXyz& c = c__s.ref();",
    "gabi::LocalOf<cXyz> e__s(a); [[maybe_unused]] cXyz& e = e__s.ref(); "
    "gabi::LocalOf<cXyz> g__s; [[maybe_unused]] cXyz& g = g__s.ref();",
    "const cXyz *p; const cXyz *q;",
    "static_assert(!std::is_class_v<cXyz>, \"autoglue: local array of a guest-layout class type (x.cpp:7) has no "
    "guest form\"); cXyz d[2]; gabi::LocalOf<cXyz> h__s; [[maybe_unused]] cXyz& h = h__s.ref();",
    "static_assert(!std::is_class_v<INT32>, \"autoglue: local array of a guest-layout class type (x.cpp:8) has no "
    "guest form\"); INT32 w[8];",
    "T *slot = find(e->x, u32(c));",
    "int i, j;",
    "struct Positions {",
    "cXyz point[2];",
    "cXyz u, v;",
    "cXyz one;",
]


def main():
    out = autoglue.transform(SRC, "x.cpp").split("/* WWHD glue")[0]
    body = [l.strip() for l in out.split("\n")]
    body = body[body.index("void f() { WWHD_GLUE_FUNC(0x02000000, void);") + 1:]
    fails = 0
    for i, want in enumerate(EXPECT):
        got = body[i] if i < len(body) else "<missing>"
        if got != want:
            fails += 1
            print("FAIL line %d\n  want %s\n  got  %s" % (i + 1, want, got))
    print("autoglue_decl_test: %s (%d checks)" % ("FAIL" if fails else "PASS", len(EXPECT)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
