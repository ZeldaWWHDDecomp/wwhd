#!/usr/bin/env python3
"""Self-test of autoglue's handling of marked member functions and bindings (transform output).

a static member function (declared `static` in its class, defined out of class without the
keyword) gets no `this` in WWHD_GLUE_FUNC and is registered like a free function; an `ext` binding to
a static member passes no `this` either.
a reference parameter (`cXy& p`, `const cXy& p`) is passed on to the guest as its address in
WWHD_GLUE_FUNC's nested call and in ext bindings (the guest ABI passes references as pointers).
`ext ADDR sig callback` (a function of another unit the source passes by name, e.g. a heap
callback) also registers the binding as a HostFn, so gabi::guest_fn maps it to ADDR; without the flag
nothing is registered (objects of existing units unchanged).
an overloaded method (two declarations in the class, definitions possibly in other TUs) or a
name marked twice in one source is registered with a typed cast, VERIFY_NAMED(addr, static_cast<...>,
"&C::f(types)"); a name declared once keeps the plain VERIFY (objects unchanged).
table initialisers may name enum constants (plain, scoped, class-nested, from the source or a
header, with implicit and expression values); unknown or ambiguous names are errors.
Runs transform() on small sources in a temporary tree (no image needed); exit 0 when all pass.
"""
import os
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

HDR = """#pragma once
struct cXy { be<f32> x, y; };
struct cM2dGBox {
    void Set(cXy& p0, cXy& p1);
    f32 GetLen(const cXy& p, int n, const cXy*);
    void Get(const cXy& out, cXy&);
};
BOOL useHeapInit(fopAc_ac_c* a_this);
BOOL entry(fopAc_ac_c* a, BOOL (*cb)(fopAc_ac_c*), u32 size);
struct daRaceItem_c {
    void set_mtx();
    void set_mtx(cXyz*);
    BOOL Delete() const;
    BOOL Delete(int) const;
};
static int cnt(int);
static int cnt(int, int);
struct cMl {
public:
    static void init(void* heap);
    static void* memalignB(int alignment, u32 size);
    static inline int twice(int x) { return x + x; }
    void member(int x);
private:
    static const int kCount = 3;
    int mValue;
};
"""

SRC_STATIC = """#include "t/cml.h"
/* 02019424 */
void cMl::init(void* heap) {
}

/* 02019430 */
void* cMl::memalignB(int alignment, u32 size) {
    return NULL;
}

/* 02019500 */
void cMl::member(int x) {
}

/* 0201060C */
void cM2dGBox::Set(cXy& p0, cXy& p1) {
    entry(NULL, useHeapInit, 0x96000);
    onlyCalled(NULL);
}

/* 02010630 */
f32 cM2dGBox::GetLen(const cXy& p, int n, const cXy*) {
    return 0.0f;
}

/* 0245A028 */
void daRaceItem_c::set_mtx(cXyz* pos) {
}

/* 0245A100 */
BOOL daRaceItem_c::Delete(int n) const {
    return 0;
}

/* 0245A200 */
static int cnt(int a) {
    return a;
}

/* 0245A300 */
static int cnt(int a, int b) {
    return a + b;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0201945C void cMl::free(void*)
ext 02019600 int cMl::twice(int)
ext 02019700 void cM2dGBox::Get(const cXy& out, cXy&)
ext 0207FE14 BOOL useHeapInit(fopAc_ac_c* a_this) callback
ext 0207FE20 BOOL onlyCalled(fopAc_ac_c*)
*/
"""
# cMl::free is not declared static in the header: the binding stays a member call (this);
# cMl::twice is: no this


def run_cases(tmp):
    fails = []
    inc = os.path.join(tmp, "wwhd_src", "include", "t")
    os.makedirs(inc)
    open(os.path.join(inc, "cml.h"), "w").write(HDR)
    out = autoglue.transform(SRC_STATIC, "wwhd_src/t/cml.cpp")

    def want(s, why):
        if s not in out:
            fails.append("case 02: %s: missing `%s`" % (why, s))

    def never(s, why):
        if s in out:
            fails.append("case 02: %s: unexpected `%s`" % (why, s))

    want("WWHD_GLUE_FUNC(0x02019424, void, heap);", "static init: no this")
    want("WWHD_GLUE_FUNC(0x02019430, void*, alignment, size);", "static memalignB: no this")
    want("WWHD_GLUE_FUNC(0x02019500, void, this, x);", "member keeps this")
    want("VERIFY(0x02019424, cMl::init);", "static registered as a function")
    want("static gabi::HostFn wwhd_fn_02019424((const void*)&cMl::init, 0x02019424);", "static HostFn")
    want("VERIFY(0x02019500, &cMl::member);", "member registered as member pointer")
    want("gabi::call<int>(0x02019600, wwhd_p0)", "static ext binding: no this")
    want("gabi::call<void>(0x0201945C, this, wwhd_p0)", "member ext binding keeps this")
    never("VERIFY(0x02019424, &cMl::init)", "static as member pointer")
    want("WWHD_GLUE_FUNC(0x0201060C, void, this, &p0, &p1);", "case 03: reference parameters as addresses")
    want("WWHD_GLUE_FUNC(0x02010630, f32, this, &p, n, wwhd_p2);", "case 03: const reference, value, pointer")
    want("gabi::call<void>(0x02019700, this, &out, &wwhd_p1)", "case 03: ext reference parameters as addresses")
    want("static gabi::HostFn wwhd_ext_fn_0207FE14((const void*)static_cast<BOOL (*)(fopAc_ac_c* a_this)>(&useHeapInit), 0x0207FE14);",
         "case 07: callback passed by name")
    never("wwhd_ext_fn_0207FE20", "case 07: ext without the flag")
    want("BOOL useHeapInit(fopAc_ac_c* a_this) { return gabi::call<BOOL>(0x0207FE14, a_this); }", "case 07: binding itself")
    want('VERIFY_NAMED(0x0245A028, static_cast<void (daRaceItem_c::*)(cXyz*)>(&daRaceItem_c::set_mtx), "&daRaceItem_c::set_mtx(cXyz*)");',
         "case 11: overload declared in the class")
    want('VERIFY_NAMED(0x0245A100, static_cast<BOOL (daRaceItem_c::*)(int) const>(&daRaceItem_c::Delete), "&daRaceItem_c::Delete(int) const");',
         "case 11: const overload")
    want('VERIFY_NAMED(0x0245A300, static_cast<int (*)(int, int)>(&cnt), "cnt(int, int)");', "case 11: free overload marked twice")
    want("static gabi::HostFn wwhd_fn_0245A200((const void*)static_cast<int (*)(int)>(&cnt), 0x0245A200);", "case 11: HostFn of an overload")
    want("VERIFY(0x02019500, &cMl::member);", "case 11: single declaration stays plain")
    statics = autoglue.static_members("cMl", SRC_STATIC, "wwhd_src/t/cml.cpp")
    if statics != {"init", "memalignB", "twice"}:
        fails.append("case 02: static_members: %r" % sorted(statics))
    return fails, 21


ENUM_HDR = """#pragma once
namespace leaves_particle {
enum {
    ID_AK_SN_FALLENLEAFRUN00 = 0x822E,
    ID_AK_SN_FALLENLEAFFAN00,
    ID_AK_SN_FALLENLEAFFIRE00 = 0x8268,
    ID_FLAGGED = ID_AK_SN_FALLENLEAFFIRE00 | 0x4000, /* expression */
};
}
struct daX_c {
    enum class Mode : u8 { WAIT = 3, RUN };
    enum Proc_e { PROC_A = 7 };
};
enum Twice_e { DUP = 1 };
namespace other { enum { DUP = 2 }; }
"""


def enum_cases(tmp):
    fails = []
    inc = os.path.join(tmp, "wwhd_src", "include", "t")
    os.makedirs(inc, exist_ok=True)
    open(os.path.join(inc, "enums.h"), "w").write(ENUM_HDR)
    src = '#include "t/enums.h"\n'
    r = autoglue.enum_resolver(src, "wwhd_src/t/x.cpp", "daX_c")
    cases = [("leaves_particle::ID_AK_SN_FALLENLEAFFAN00", 0x822F), ("ID_AK_SN_FALLENLEAFRUN00", 0x822E),
             ("leaves_particle::ID_FLAGGED", 0xC268), ("daX_c::Mode::RUN", 4), ("Mode::WAIT", 3),
             ("PROC_A", 7), ("daX_c::PROC_A", 7), ("other::DUP", 2), ("NOPE", None)]
    for name, want in cases:
        got = r(name)
        if got != want:
            fails.append("case 12: resolve %s: want %r got %r" % (name, want, got))
    try:
        r("DUP")
        fails.append("case 12: ambiguous DUP not reported")
    except autoglue.GlueError:
        pass
    for leaf, size, want in [("leaves_particle::ID_AK_SN_FALLENLEAFFAN00", 2, b"\x82\x2f"),
                             ("(u16)daX_c::PROC_A + 1", 2, b"\x00\x08"), ("0x10", 1, b"\x10")]:
        got = autoglue.encode_leaf(leaf, "int", size, None, {}, None, r)
        if got != want:
            fails.append("case 12: encode %s: want %r got %r" % (leaf, want, got))
    try:
        autoglue.encode_leaf("UNKNOWN_ID", "int", 2, None, {}, None, r)
        fails.append("case 12: unknown name not reported")
    except autoglue.GlueError:
        pass
    return fails, len(cases) + 5


def main():
    tmp = tempfile.mkdtemp(prefix="gluemem-")
    old_root = autoglue.ROOT
    try:
        autoglue.ROOT = tmp
        autoglue._layout_cache.clear()
        fails, n = run_cases(tmp)
        f2, n2 = enum_cases(tmp)
        fails += f2
        n += n2
    finally:
        autoglue.ROOT = old_root
        autoglue._layout_cache.clear()
        shutil.rmtree(tmp, ignore_errors=True)
    for f in fails:
        print("FAIL " + f)
    print("autoglue_members_test: %s %d/%d" % ("ok" if not fails else "FAIL", n - len(fails), n))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
