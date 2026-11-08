#!/usr/bin/env python3
"""Self-test of decl_lint.py (exact declarations of guest calls, 2026-11-03):
1. an `ext` line of a class method passes `this` (no false "under-declared" for `R C::f()` against `R f(C*)`), and
   f32/f64 parameters count as floating-point registers;
2. compile mode (GABI_DECL_AUDIT) reports every guest call with its argument types: the ext stubs, the body's direct
   gabi::call, and each definition's own entry glue (its parameters as compiled);
3. classification: a call that passes exactly what the callee's definition takes is clean; one that leaves out a
   register the caller received as its own parameter is (a); one that leaves out a register the caller never
   declared is (b) (the caller passes its own entry value on); a register left over from an earlier call is
   `local`; an argument nothing reads is (e); a callee parameter the original never reads is (d);
4. the definition audit lists a function whose original reads an argument register its definition does not take;
5. ext stubs called from outside the source's own function bodies are audited, not skipped as "makes no call"
   (2026-11-04: m_Do_machine's cMl::init stub reached only through a header inline wrapper, d_s_play's and
   d_particle_callbacks' stubs called from another source of the same unit): a stub reached through a header inline
   wrapper or from a linked source is classified by the calling definition's original; an overloaded stub name counts
   only the calls with its own argument count; a stub passed as a host function pointer to a helper that calls it
   (`beamCall(mDoAud_seStartHD, ...)`) is audited; a stub whose guest address is only taken (HostFn) stays skipped.
6. an extra argument that only an indirect call may read (e?): when the callee's own original still holds it at one
   of its indirect calls (hdLayoutResMgr_c::resume -> the thread's sendMessage reads r4, 2026-11-04) the site is the
   error e?i (the definition must take and forward it); an import whose runtime HLE does not read it does not count;
   a callee left with e? sites is `guest` in the hand-off, not `shim`;
7. a definition whose original still holds an entry register at its own indirect call, where a possible target reads
   it and the decompiled call_ptr does not pass it (02002A40 buildLayoutScreen, 2026-11-04), is `guest` in the
   hand-off; passing it, a target set that does not read it, or an `indirect` exception clears it; an unresolved
   target counts as reading everything;
8. IndirectTargets narrows a vtable call whose object is the function's own `this` (entry r3, also when copied to a
   callee-saved register and kept across calls) to the vtables that hold a method of its class; an object that is
   not `this` on every path keeps every vtable at that offset;
9. step C: a pointer-to-member call through a constant table resolves to the table's functions (index -1), its virtual
   path to nothing when no entry is virtual; a subclass vtable (its constructor calls the base constructor that stores
   the narrowed vtable) stays a candidate even when it holds none of the class's decompiled methods;
10. result check (2026-11-04, dScnLogo_Delete): a call site that declares a result the callee's definition does not
   produce is the error `ret` when the caller's original reads the register after the call (also a register the callee
   leaves unchanged), `ret?` when it only returns it on to its own callers, `ret-unread` when nothing reads it or the
   decompiled callers discard it.
11. (2026-11-06) an indirect call inside a header's inline function counts for the decompiled functions whose bodies
   call that function (c_bg_w.h DispatchGrpThrough): a held register the wrapper passes is passed.
Exit 0 when all cases pass.
"""
import collections
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import decl_lint  # noqa: E402

CALLER = '''#include "bindings.h"
struct Obj { be<u32> v; u32 get(); };
void take2(Obj*);
void take2x(Obj*, u32);
void take1(Obj*, u32);
u32 produce();
void consume();
/* 02900000 */
void caller_a(Obj* o, u32 heap) { take2(o); }
/* 02900010 */
void caller_b(Obj* o) { take2(o); }
/* 02900020 */
void caller_exact(Obj* o, u32 heap) { take2x(o, heap); o->get(); gabi::call(0x02900400, o, 1.5f); }
/* 02900030 */
void caller_e(Obj* o) { take1(o, 7); }
/* 02900040 */
void caller_local(Obj* o) { produce(); consume(); }
/* WWHD glue
ext 02900100 void take2(Obj*)
ext 02900100 void take2x(Obj*, u32)
ext 02900200 void take1(Obj*, u32)
ext 02900300 u32 Obj::get()
ext 02900500 u32 produce()
ext 02900600 void consume()
*/
'''

CALLEE = '''#include "bindings.h"
/* 02900100 */
void take2(void* env, u32 heap) { (void)env; (void)heap; }
/* 02900200 */
void take1(void* env) { (void)env; }
/* 02900300 */
u32 getter(u32 self) { return self; }
/* 02900400 */
void takef(void* p, f32 x) { (void)p; (void)x; }
/* 02900500 */
u32 produce() { return 1; }
/* 02900600 */
void consume(u32 value, u32 unused) { (void)value; (void)unused; }
/* WWHD glue
*/
'''


WRAP_H = '''#pragma once
void initFromReturn();
void setBoth(void* p);
void setBoth(void* p, void* q);
void sibling_stub();
void address_only(void* a, void* b);
namespace cMx {
inline void init() { initFromReturn(); }
}
'''

HDR_CALLER = '''#include "bindings.h"
#include "wrap.h"
u32 produce();
/* 02900700 */
void create_heaps() { produce(); cMx::init(); }
/* 02900710 */
void draw_one(void* p, void* e) { setBoth(p, e); }
void viaPtr(void* p);
template <class F, class... A> static void invoke(F f, A... a) { f(a...); }
/* 02900780 */
void play(void* p) { invoke(viaPtr, p); }
static gabi::HostFn addrBinding(reinterpret_cast<const void*>(&address_only), 0x02900740);
/* WWHD glue
ext 02900500 u32 produce()
ext 02900720 void initFromReturn()
ext 02900730 void setBoth(void* p)
ext 02900730 void setBoth(void* p, void* q)
ext 02900750 void sibling_stub()
ext 02900740 void address_only(void* a, void* b)
ext 02900770 void viaPtr(void* p)
*/
'''

SIBLING = '''#include "bindings.h"
#include "wrap.h"
/* 02900760 */
void phase(void* p) { setBoth(p); sibling_stub(); }
/* WWHD glue
*/
'''

HDR_CALLEE = '''#include "bindings.h"
/* 02900720 */
void init_heap(void* heap) { (void)heap; }
/* 02900730 */
void set_both(void* p, void* q) { (void)p; (void)q; }
/* 02900740 */
void typical(void* a, void* b, void* c) { (void)a; (void)b; (void)c; }
/* 02900750 */
void wide(u32 a, u32 b) { (void)a; (void)b; }
/* 02900770 */
void played(void* p, void* q) { (void)p; (void)q; }
/* WWHD glue
*/
'''


def stub_cases(td, fails):
    """case 5: stubs called through a header inline wrapper and from a linked source"""
    paths = {}
    for name, text in (("wrap.h", WRAP_H), ("hdr_caller.cpp", HDR_CALLER), ("sibling.cpp", SIBLING),
                       ("hdr_callee.cpp", HDR_CALLEE)):
        paths[name] = os.path.join(td, name)
        open(paths[name], "w").write(text)
    srcs = [paths["hdr_caller.cpp"], paths["sibling.cpp"], paths["hdr_callee.cpp"]]
    defs, exts = decl_lint.collect(srcs)
    compiled = {}
    for rel in srcs:
        text, sites, errs = decl_lint.compile_sites(rel, td)
        if errs:
            fails.append("%s does not compile: %s" % (os.path.basename(rel), errs[:400]))
        compiled[rel] = (sites, errs)
    li = {0x02900720: R(3), 0x02900730: R(3, 4), 0x02900740: R(3, 4, 5), 0x02900750: R(3, 4), 0x02900500: frozenset(),
          0x02900770: R(3, 4)}
    entry = R(3, 4, 5, 6)
    flow = FakeFlow({
        0x02900700: {0x02900500: [(entry, entry)], 0x02900720: [(frozenset(), frozenset())]},  # r3 = produce()
        0x02900710: {0x02900730: [(entry, entry)]},
        0x02900760: {0x02900730: [(entry, entry)], 0x02900750: [(entry, entry)]},  # r4 still the caller's entry r4
        0x02900780: {0x02900770: [(entry, entry)]},
    })
    flow.gen.loc.update({a: None for a in (0x02900700, 0x02900710, 0x02900720, 0x02900730, 0x02900740,
                                           0x02900750, 0x02900760, 0x02900770, 0x02900780)})
    sib = {paths["hdr_caller.cpp"]: {paths["sibling.cpp"]}, paths["sibling.cpp"]: {paths["hdr_caller.cpp"]}}
    au = FakeAudit(defs, exts, compiled, li, flow, siblings=sib)
    rel = paths["hdr_caller.cpp"]
    got = {}
    for st in au.sites:
        where, frel, line, kind, ret, types, a, caller, outer = st
        if where != "stub" or kind != "call" or frel != rel:
            continue
        regs = decl_lint.site_regs(types)
        if au.stub_skip(rel, a, regs):
            got[(a, regs)] = "skip"
            continue
        cls, note, dec = au.classify(st)
        got[(a, regs)] = cls
    want = {(0x02900720, (0, 0)): "local",   # cMx::init() in a header: r3 holds produce()'s result
            (0x02900730, (1, 0)): "b",       # setBoth(p) called from the linked source: r4 its undeclared entry r4
            (0x02900730, (2, 0)): None,      # setBoth(p, e): exact
            (0x02900750, (0, 0)): "b",       # sibling_stub() from the linked source
            (0x02900740, (2, 0)): "skip",    # only its guest address is taken (HostFn)
            (0x02900770, (1, 0)): "b"}       # passed as a host function pointer to a helper that calls it
    for k, v in want.items():
        if got.get(k, "missing") != v:
            fails.append("stub %08X %s: want %r, got %r" % (k[0], decl_lint.fmt_regs(k[1]), v, got.get(k, "missing")))
    users = {d.addr for d in au.stub_users(rel, 0x02900720, (0, 0))}
    if users != {0x02900700}:
        fails.append("header-wrapper stub users: want 02900700, got %s" % sorted("%08X" % u for u in users))


EQ_CALLER = '''#include "bindings.h"
u32 resumeStub(void* mgr, u32 message);
u32 freeStub(void* p, u32 size);
u32 deepStub(void* p, u32 x);
/* 02900800 */
u32 kick(void* mgr, u32 m) { return resumeStub(mgr, m); }
/* 02900810 */
u32 drop(void* p) { freeStub(p, 8); return deepStub(p, 3); }
/* WWHD glue
ext 02900820 u32 resumeStub(void*, u32)
ext 02900830 u32 freeStub(void*, u32)
ext 02900840 u32 deepStub(void*, u32)
*/
'''

EQ_CALLEE = '''#include "bindings.h"
/* 02900820 */
u32 resume(void* mgr) { (void)mgr; return 0; }
/* 02900830 */
u32 freeIt(void* p) { (void)p; return 0; }
/* 02900840 */
u32 deep(void* p) { (void)p; return 0; }
/* WWHD glue
*/
'''


def eq_cases(td, fails):
    """case 6: e? extras forwarded to the callee's own indirect call (e?i) or not (e?, guest in the hand-off)"""
    paths = {}
    for name, text in (("eq_caller.cpp", EQ_CALLER), ("eq_callee.cpp", EQ_CALLEE)):
        paths[name] = os.path.join(td, name)
        open(paths[name], "w").write(text)
    srcs = [paths["eq_caller.cpp"], paths["eq_callee.cpp"]]
    defs, exts = decl_lint.collect(srcs)
    compiled = {rel: decl_lint.compile_sites(rel, td)[1:] for rel in srcs}
    for rel, (sites, errs) in compiled.items():
        if errs:
            fails.append("%s does not compile: %s" % (os.path.basename(rel), errs[:400]))
    li = {0x02900820: R(3), 0x02900830: R(3), 0x02900840: R(3)}
    up = {0x02900820: R(3, 4), 0x02900830: R(3, 4), 0x02900840: R(3, 4)}
    flow = FakeFlow({})
    flow.gen.loc.update({a: None for a in (0x02900800, 0x02900810, 0x02900820, 0x02900830, 0x02900840)})
    flow.ind = {0x02900820: [(0x02900828, None, R(4, 5))],                     # bctr with its entry r4
                0x02900830: [(0x02900838, "imp_coreinit_MEMFreeToDefaultHeap", R(4))],  # import: reads r3 only
                0x02900840: []}                                               # an indirect call further down
    saved = dict(decl_lint.IMPORT_READS)
    decl_lint.IMPORT_READS["imp_coreinit_MEMFreeToDefaultHeap"] = R(3)

    class Au(FakeAudit):
        def upper(self, a):
            return up.get(a, frozenset())
    au = Au(defs, exts, compiled, li, flow, siblings={})
    got = {}
    for st in au.sites:
        if st[0] == "stub" and st[3] == "call":
            got[st[6]] = au.classify(st)
    decl_lint.IMPORT_READS.clear()
    decl_lint.IMPORT_READS.update(saved)
    want = {0x02900820: "e?i", 0x02900830: "e?", 0x02900840: "e?"}
    for a, c in want.items():
        if got.get(a, (None,))[0] != c:
            fails.append("e? case %08X: want %s, got %r" % (a, c, got.get(a)))
    if "e?i" not in decl_lint.ERROR_CLASSES:
        fails.append("e?i must be an error class")
    by_target = {a: collections.Counter({c[0]: 1}) for a, c in got.items()}
    res = dict(audit=au, by_target=by_target, under={}, exceptions={}, raw={})
    out = os.path.join(td, "handoff.tsv")
    decl_lint.handoff(res, out)
    st = {int(l.split("\t")[0], 16): l.split("\t")[1] for l in open(out) if not l.startswith("#")}
    if st.get(0x02900830) != "guest" or st.get(0x02900840) != "guest":
        fails.append("hand-off: want e? callees guest, got %r" % st)


CP_SRC = '''#include "bindings.h"
struct Vt { be<u32> pad[15]; be<u32> mInit; };
struct Scr { gptr<Vt> mVtable; };
/* 02900900 */
u32 build(Scr* s, u32 argument, u32 option) { (void)argument; (void)option; return gabi::call_ptr<u32>(s->mVtable->mInit.get(), s); }
/* 02900910 */
u32 buildFixed(Scr* s, u32 argument, u32 option) { return gabi::call_ptr<u32>(s->mVtable->mInit.get(), s, argument, option); }
/* 02900920 */
u32 buildNoRead(Scr* s, u32 argument) { (void)argument; return gabi::call_ptr<u32>(s->mVtable->mInit.get(), s); }
/* 02900930 */
u32 buildUnresolved(Scr* s, u32 argument) { (void)argument; return gabi::call_ptr<u32>(s->mVtable->mInit.get(), s); }
/* 02900940 */
u32 buildExcepted(Scr* s, u32 argument) { (void)argument; return gabi::call_ptr<u32>(s->mVtable->mInit.get(), s); }
/* WWHD glue
*/
'''


class FakeTargets:
    def __init__(self, table):
        self.table = table

    def candidates(self, site, known, fn=None, size=0, family=None):
        return self.table.get(site)

    def vtables_holding(self, fns):
        return frozenset()


def callptr_cases(td, fails):
    """case 7: entry registers left in place for the original's own indirect calls"""
    rel = os.path.join(td, "cp.cpp")
    open(rel, "w").write(CP_SRC)
    defs, exts = decl_lint.collect([rel])
    sites, errs = decl_lint.compile_sites(rel, td)[1:]
    if errs:
        fails.append("cp.cpp does not compile: %s" % errs[:400])
    compiled = {rel: (sites, errs)}
    held = R(3, 4, 5)
    flow = FakeFlow({})
    flow.gen.loc.update({a: None for a in (0x02900900, 0x02900910, 0x02900920, 0x02900930, 0x02900940, 0x02900A00,
                                           0x02900A10)})
    flow.ind = {a: [(a + 8, None, held)] for a in (0x02900900, 0x02900910, 0x02900920, 0x02900930, 0x02900940)}
    li = {0x02900A00: R(3, 4, 5), 0x02900A10: R(3)}
    targets = FakeTargets({0x02900908: frozenset([0x02900A00]), 0x02900918: frozenset([0x02900A00]),
                           0x02900928: frozenset([0x02900A10]), 0x02900948: frozenset([0x02900A00])})
    saved = dict(decl_lint.INDIRECT_EXCEPTIONS)
    decl_lint.INDIRECT_EXCEPTIONS[0x02900940] = R(4, 5)
    au = FakeAudit(defs, exts, compiled, li, flow, siblings={})
    got = decl_lint.callptr_audit(au, defs, li, targets)
    decl_lint.INDIRECT_EXCEPTIONS.clear()
    decl_lint.INDIRECT_EXCEPTIONS.update(saved)
    want = {0x02900900: R(4, 5), 0x02900930: R(4, 5)}
    have = {a: frozenset().union(*[r for _, r, _ in h]) for a, h in got.items()}
    if have != want:
        fails.append("callptr audit: want %r, got %r" % (sorted("%08X" % a for a in want), sorted("%08X" % a for a in have)))
    res = dict(audit=au, by_target={}, under={}, exceptions={}, raw={}, held=got)
    out = os.path.join(td, "handoff.tsv")
    decl_lint.handoff(res, out)
    st = {int(l.split("\t")[0], 16): l.split("\t")[1] for l in open(out) if not l.startswith("#")}
    if st.get(0x02900900) != "guest" or st.get(0x02900910) != "exact" or st.get(0x02900920) != "exact":
        fails.append("callptr hand-off: want 02900900 guest, 02900910/02900920 exact, got %r" % st)


WRAP_HDR = '''#pragma once
struct WVt { be<u32> pad[15]; be<u32> mInit; };
struct Wrapped {
    gptr<WVt> mVtable;
    u32 dispatchInit(u32 a, u32 b) {
        return gabi::call_ptr<u32>(mVtable->mInit.get(), this, a, b);
    }
};
'''
WRAP_SRC = '''#include "bindings.h"
#include "wrap_case.h"
/* 02900B00 */
u32 viaWrapper(Wrapped* w, u32 a, u32 b) { return w->dispatchInit(a, b); }
/* 02900B10 */
u32 noWrapper(Wrapped* w, u32 a) { (void)a; return gabi::call_ptr<u32>(w->mVtable->mInit.get(), w); }
/* WWHD glue
*/
'''


def wrapper_cases(td, fails):
    """case 11: header inline wrappers of indirect calls"""
    open(os.path.join(td, "wrap_case.h"), "w").write(WRAP_HDR)
    rel = os.path.join(td, "wrap.cpp")
    open(rel, "w").write(WRAP_SRC)
    defs, exts = decl_lint.collect([rel])
    sites, errs = decl_lint.compile_sites(rel, td)[1:]
    if errs:
        fails.append("wrap.cpp does not compile: %s" % errs[:400])
    if not any(s[8] == "hdr" and s[3] == "call_ptr" for s in sites):
        fails.append("wrapper: the header's call_ptr site was not reported")
    compiled = {rel: (sites, errs)}
    flow = FakeFlow({})
    flow.gen.loc.update({a: None for a in (0x02900B00, 0x02900B10, 0x02900A00)})
    flow.ind = {0x02900B00: [(0x02900B08, None, R(3, 4, 5))], 0x02900B10: [(0x02900B18, None, R(3, 4))]}
    li = {0x02900A00: R(3, 4, 5)}
    targets = FakeTargets({0x02900B08: frozenset([0x02900A00]), 0x02900B18: frozenset([0x02900A00])})
    au = FakeAudit(defs, exts, compiled, li, flow, siblings={})
    got = decl_lint.callptr_audit(au, defs, li, targets)
    have = {a: frozenset().union(*[r for _, r, _ in h]) for a, h in got.items()}
    if have != {0x02900B10: R(4)}:
        fails.append("wrapper audit: want 02900B10 r4 only, got %r" % {"%08X" % a: sorted(r) for a, r in have.items()})


def ptmf_cases(fails):
    """case 9: constant pointer-to-member tables and subclasses that override every decompiled method"""
    t = decl_lint.IndirectTargets.__new__(decl_lint.IndirectTargets)
    t.ok = True
    F = 0x02900D00
    body = [("lis", "r12, 0x1002"), ("addi", "r12, r12, 0x100"), ("mulli", "r0, r5, 0x10"), ("add", "r11, r12, r0"),
            ("lha", "r9, 2(r11)"), ("cmpwi", "r9, 0"), ("bge", "0x2900d28"),
            ("lwz", "r9, 4(r11)"), ("mtctr", "r9"), ("bctr", ""),
            ("lha", "r10, 2(r11)"), ("lha", "r7, 6(r11)"), ("slwi", "r8, r10, 3"), ("lwzx", "r7, r3, r7"),
            ("add", "r12, r7, r8"), ("lwz", "r9, 4(r12)"), ("mtctr", "r9"), ("bctr", "")]
    t.ins = {F + 4 * i: x for i, x in enumerate(body)}
    import struct
    table = b"".join(struct.pack(">hhI", 0, -1, fn) + b"\0" * 8 for fn in (0x02900E00, 0x02900E10)) + b"\xff" * 16
    t.data = [(0x10020100, table)]
    t.vt, t.words = {}, {}
    known = {0x02900E00, 0x02900E10, 0x02900E20}
    if t.ptmf_table(F + 36, known) != frozenset([0x02900E00, 0x02900E10]):
        fails.append("ptmf table: want 02900E00/02900E10, got %r" % (t.ptmf_table(F + 36, known),))
    if t.ptmf_table(F + 68, known) != frozenset():
        fails.append("ptmf table virtual path: want no target (no virtual entry), got %r" % (t.ptmf_table(F + 68, known),))
    # subclasses: Dctor (02900F40) calls Cctor (02900F00, stores vtable 10000000), then stores 10002000
    t.ins = {0x02900F00: ("stw", "r0, 0(r3)"), 0x02900F40: ("bl", "0x2900f00"), 0x02900F44: ("stw", "r0, 0(r3)")}
    t.vstores = [(0x02900F00, 0x10000000, 0), (0x02900F44, 0x10002000, 0)]
    kids = t.children([0x02900F00, 0x02900F40])
    if 0x10002000 not in kids.get(0x10000000, ()):
        fails.append("children: want 10002000 under 10000000, got %r" % dict(kids))
    got = t.with_subclasses(frozenset([0x10000000]), frozenset([0x10000000, 0x10002000, 0x10003000]))
    if got != frozenset([0x10000000, 0x10002000]):
        fails.append("with_subclasses: want 10000000+10002000, got %r" % sorted(got))


RET_SRC = '''#include "bindings.h"
u32 getStale();
u32 passOn();
u32 getDropped();
/* 02901000 */
u32 useIt() { u32 v = getStale(); return v + 1; }
/* 02901010 */
u32 forward() { return passOn(); }
/* 02901020 */
void drop() { getDropped(); }
/* 02901100 */
void voidCallee() { }
/* 02901110 */
void voidCallee2() { }
/* 02901120 */
void voidCallee3() { }
/* WWHD glue
ext 02901100 u32 getStale()
ext 02901110 u32 passOn()
ext 02901120 u32 getDropped()
*/
'''


def gen_fn(addr, lines):
    body = "".join("    %s /* %08X: 60000000 */\n" % (l, addr + 4 * i) for i, l in enumerate(lines))
    return "void f_%08X(Cpu* __restrict c) {\n%s}\n" % (addr, body)


class CfgDf:
    def __init__(self, texts, maydefs):
        from funcdb import Cfg
        self.cfgs = {a: Cfg(t) for a, t in texts.items()}
        self.md = maydefs

    def cfg(self, a):
        return self.cfgs[a]

    def maydef(self, a):
        return set(self.md.get(a, ()))


def result_cases(td, fails):
    """case 10: the result check"""
    rel = os.path.join(td, "ret.cpp")
    open(rel, "w").write(RET_SRC)
    defs, exts = decl_lint.collect([rel])
    sites, errs = decl_lint.compile_sites(rel, td)[1:]
    if errs:
        fails.append("ret.cpp does not compile: %s" % errs[:400])
    texts = {
        0x02901000: gen_fn(0x02901000, ["c->lr = 0x02901008u; f_02901100(c);", "c->r[3] = c->r[3] + 0x00000001u;", "return;"]),
        0x02901010: gen_fn(0x02901010, ["c->lr = 0x02901018u; f_02901110(c);", "return;"]),
        0x02901020: gen_fn(0x02901020, ["c->lr = 0x02901028u; f_02901120(c);", "return;"]),
        0x02901100: gen_fn(0x02901100, ["return;"]),
        0x02901110: gen_fn(0x02901110, ["return;"]),
        0x02901120: gen_fn(0x02901120, ["return;"]),
    }
    flow = FakeFlow({})
    flow.df = CfgDf(texts, {0x02901100: []})
    flow.gen.loc.update({a: None for a in texts})
    au = FakeAudit(defs, exts, {rel: (sites, errs)}, {}, flow, siblings={})
    rr = decl_lint.ResultReads(flow, defs, {0x02901000: frozenset([("r", 3)]), 0x02901010: frozenset([("r", 3)]),
                                            0x02901020: frozenset()})
    got = {}
    for cls, a, site, users, regs, note in decl_lint.result_audit(au, defs, rr):
        got[users] = cls
    want = {"02901000": "ret", "02901010": "ret?", "02901020": "ret-unread"}
    if got != want:
        fails.append("result check: want %r, got %r" % (want, got))


def targets_cases(fails):
    """case 8: narrowing to the class's own vtables only for `this`"""
    t = decl_lint.IndirectTargets.__new__(decl_lint.IndirectTargets)
    t.ok = True
    F, G = 0x02900B00, 0x02900C00
    body = [("mr", "r31, r3"), ("bl", "0x2900d00"), ("lwz", "r12, 0(r31)"), ("lwz", "r0, 0x14(r12)"), ("mtctr", "r0"),
            ("bctrl", ""), ("blr", "")]
    other = [("mr", "r31, r3"), ("cmpwi", "r4, 0"), ("beq", "0x2900c10"), ("mr", "r31, r4"), ("lwz", "r12, 0(r31)"),
             ("lwz", "r0, 0x14(r12)"), ("mtctr", "r0"), ("bctrl", ""), ("blr", "")]
    t.ins = {F + 4 * i: x for i, x in enumerate(body)}
    t.ins.update({G + 4 * i: x for i, x in enumerate(other)})
    t.vt = {0: frozenset([0x10000000, 0x10001000])}
    t.words = {0x10000000: 0x02900E00, 0x10000014: 0x02900E10, 0x10001000: 0x02900F00, 0x10001014: 0x02900F10}
    known = {0x02900E00, 0x02900E10, 0x02900F00, 0x02900F10}
    fam = t.vtables_holding({0x02900E00})
    if fam != frozenset([0x10000000]):
        fails.append("vtables_holding: want 10000000, got %r" % sorted(fam))
    c = t.candidates(F + 20, known, F, len(body) * 4, fam)
    if c != frozenset([0x02900E10]):
        fails.append("this-call: want the class's own slot 02900E10, got %r" % sorted(c or ()))
    c = t.candidates(G + 28, known, G, len(other) * 4, fam)
    if c != frozenset([0x02900E10, 0x02900F10]):
        fails.append("object not always this: want every vtable's slot, got %r" % sorted(c or ()))


class FakeGen:
    def __init__(self, addrs):
        self.loc = {a: None for a in addrs}


class FakeCfg:
    varargs = False


class FakeDf:
    def cfg(self, a):
        return FakeCfg()


class FakeFlow:
    """the original's facts: which registers still hold the entry value at each call"""

    def __init__(self, sites):
        self.memo = sites
        self.gen = FakeGen(set(sites) | {0x02900100, 0x02900200, 0x02900300, 0x02900400, 0x02900500, 0x02900600})
        self.df = FakeDf()

    def sites(self, f):
        return self.memo.get(f, {})

    def indirect_held(self, f):
        return self.ind.get(f, []) if hasattr(self, "ind") else []


class FakeAudit(decl_lint.Audit):
    def upper(self, a):
        return self.li.get(a, frozenset())


def R(*regs):
    return frozenset(("r" if r < 100 else "f", r % 100) for r in regs)


def main():
    fails = []
    with tempfile.TemporaryDirectory() as td:
        rel_caller = os.path.join(td, "caller.cpp")
        rel_callee = os.path.join(td, "callee.cpp")
        open(rel_caller, "w").write(CALLER)
        open(rel_callee, "w").write(CALLEE)
        defs, exts = decl_lint.collect([rel_caller, rel_callee])
        ext = {(e.addr, e.qname): e for e in exts}
        if ext[(0x02900300, "Obj::get")].regs != (1, 0):
            fails.append("method ext without `this`: %r" % (ext[(0x02900300, "Obj::get")].regs,))
        if defs[0x02900400][0].regs != (1, 1):
            fails.append("f32 parameter not a float register: %r" % (defs[0x02900400][0].regs,))
        compiled = {}
        for rel in (rel_caller, rel_callee):
            text, sites, errs = decl_lint.compile_sites(rel, td)
            if errs:
                fails.append("%s does not compile: %s" % (os.path.basename(rel), errs[:400]))
            compiled[rel] = (sites, errs)
        kinds = {(s[8], s[7]) for s in compiled[rel_caller][0]}
        for want in (("stub", 0x02900100), ("stub", 0x02900300), ("body", 0x02900400), ("glue", 0x02900000)):
            if want not in kinds:
                fails.append("compile mode misses a %s site of %08X" % want)
        # the original: take2 reads r3, r4; take1 r3; consume r3 (left by produce's result); takef r3, f1
        li = {0x02900100: R(3, 4), 0x02900200: R(3), 0x02900300: R(3), 0x02900400: R(3, 101), 0x02900500: frozenset(),
              0x02900600: R(3), 0x02900000: R(3, 4), 0x02900010: R(3, 4)}
        entry = R(3, 4, 5, 6)
        flow = FakeFlow({
            0x02900000: {0x02900100: [(entry, entry)]},          # caller_a passes its own r4 on
            0x02900010: {0x02900100: [(entry, entry)]},          # caller_b too, without declaring it
            0x02900020: {0x02900100: [(frozenset(), frozenset())], 0x02900300: [(frozenset(), frozenset())]},
            0x02900030: {0x02900200: [(frozenset(), frozenset())]},
            0x02900040: {0x02900500: [(entry, entry)], 0x02900600: [(frozenset(), frozenset())]},
        })
        au = FakeAudit(defs, exts, compiled, li, flow)
        got = {}
        for st in au.sites:
            where, rel, line, kind, ret, types, a, caller, outer = st
            if kind != "call" or a is None:
                continue
            cls, note, dec = au.classify(st)
            got.setdefault((a, caller.addr if caller else None, where), set()).add(cls)
        stub_cls = {}
        for (a, c, where), cl in got.items():
            if where == "stub":
                stub_cls.setdefault(a, set()).update(cl)
        # take2 (02900100) has two ext lines: take2(Obj*) under-declared, take2x(Obj*, u32) exact; the under-declared
        # stub is called by caller_a (declares r4: a) and caller_b (does not: b), most severe wins: b
        if not ({"b"} <= stub_cls.get(0x02900100, set()) and None in stub_cls.get(0x02900100, set())):
            fails.append("take2 stubs: want b and exact, got %r" % (stub_cls.get(0x02900100),))
        if stub_cls.get(0x02900200) != {"e"}:
            fails.append("take1(Obj*, u32): want e (r4 read by nobody), got %r" % (stub_cls.get(0x02900200),))
        if stub_cls.get(0x02900300) != {None}:
            fails.append("Obj::get(): want exact (this), got %r" % (stub_cls.get(0x02900300),))
        if stub_cls.get(0x02900600) != {"local"}:
            fails.append("consume(): want local (r3 left by produce's result), got %r" % (stub_cls.get(0x02900600),))
        if got.get((0x02900400, 0x02900020, "body")) != {None}:
            fails.append("direct gabi::call(takef, o, 1.5f): want exact, got %r" % (got.get((0x02900400, 0x02900020, "body")),))
        # caller_a alone: r4 is its own parameter -> a
        st_a = [s for s in au.sites if s[0] == "stub" and s[6] == 0x02900100 and s[5] == ["Obj *"]]
        cls_a, note_a, _ = au.classify(st_a[0])
        if "a r4@02900000" not in note_a or "b r4@02900010" not in note_a:
            fails.append("take2 note: want a r4@02900000 and b r4@02900010, got %r" % note_a)
        under = au.def_under()
        if 0x02900010 not in under or 0x02900000 in under:
            fails.append("definition audit: want caller_b (02900010) under-declared only, got %r" % sorted(under))
        # consume declares (u32 value, u32 unused); the original reads only r3: a call passing r3 only is (d) there
        st_c = [s for s in au.sites if s[0] == "stub" and s[6] == 0x02900600]
        cls_c, note_c, _ = au.classify(st_c[0])
        if "never reads" not in note_c:
            fails.append("consume(): want the (d) note for r4, got %r" % note_c)
    with tempfile.TemporaryDirectory() as td:
        stub_cases(td, fails)
    with tempfile.TemporaryDirectory() as td:
        eq_cases(td, fails)
    with tempfile.TemporaryDirectory() as td:
        callptr_cases(td, fails)
    with tempfile.TemporaryDirectory() as td:
        wrapper_cases(td, fails)
    targets_cases(fails)
    ptmf_cases(fails)
    with tempfile.TemporaryDirectory() as td:
        result_cases(td, fails)
    if fails:
        print("decl_lint_test: FAIL")
        for f in fails:
            print("  " + f)
        return 1
    print("decl_lint_test: ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
