/* hd_sys_0203F8CC: the in-block part (0203F8CC..0203FFFF) of the HD SystemTask translation unit
 * (cking::system); its tail past 0x02040000 is wwhd_src/hd_sys/hd_sys_0203F8CC_tail.cpp. No GameCube
 * source: HD-only code.
 *
 * Contents: deleting destructors of the TU's sead classes, the termination helper of a fixed
 * SafeString buffer, two sead RTTI checks (function-local static type infos), five sead factories
 * ("new (heap) T(arg)"), the 'single instance' guard of the SystemTask (global 1018F5A8), the
 * SystemTask constructor (0xCC, sead task base 027454E0), its prepare step (font/message managers,
 * the "MsgMgr"-type expanded heaps, main01) and its enter step (creates the two child tasks through
 * the task manager, optional viewport setup on the debug draw path).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_0203F8CC {

/* operator new(size, sead::Heap*, align) */
static inline u32 new_heap(u32 size, u32 heap, u32 align) { return gabi::call<u32>(0x0273B050, size, heap, align); }

/* 0203F8CC: deleting destructor (vtable 1015E028 at +0x1C, base dtor 0275CFD0) */
static void Dt_0203F8CC(u32 p, u32 flags) {
    WWHD_FUNC(0x0203F8CC, void, p, flags);
    if (p == 0) return;
    st(p + 0x1C, 0x1015E028);
    gabi::call(0x0275CFD0, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203F8CC, Dt_0203F8CC);

/* 0203F92C: deleting destructor (vtable 100067A0 at +0x20, base dtor 02747D40) */
static void Dt_0203F92C(u32 p, u32 flags) {
    WWHD_FUNC(0x0203F92C, void, p, flags);
    if (p == 0) return;
    st(p + 0x20, 0x100067A0);
    gabi::call(0x02747D40, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203F92C, Dt_0203F92C);

static void Empty_0203F98C() {
    WWHD_FUNC(0x0203F98C, void);
}
VERIFY(0x0203F98C, Empty_0203F98C);

/* 0203F990: fixed SafeString buffer: terminate at the last byte (buffer +0, capacity +8) */
static void SafeBuf_assureTerminate(u32 self) {
    WWHD_FUNC(0x0203F990, void, self);
    stb(ld(self) + ld(self + 8) - 1, 0);
}
VERIFY(0x0203F990, SafeBuf_assureTerminate);

static void Dt_0203F9A8(u32 p, u32 flags) {
    WWHD_FUNC(0x0203F9A8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203F9A8, Dt_0203F9A8);

static void Dt_0203F9BC(u32 p, u32 flags) {
    WWHD_FUNC(0x0203F9BC, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203F9BC, Dt_0203F9BC);

/* sead checkDerivedRuntimeTypeInfo: the class's static type info, then its parent's */
static inline u32 rtti_check2(u32 type, u32 ga, u32 oa, u32 va, u32 gb, u32 ob, u32 vb) {
    if (ld(ga) == 0) { st(ga, 1); st(oa, va); }
    if (type == oa) return 1;
    if (ld(gb) == 0) { st(gb, 1); st(ob, vb); }
    return type == ob;
}
static u32 Rtti_0203F9D0(u32 self, u32 type) {
    WWHD_FUNC(0x0203F9D0, u32, self, type);
    return rtti_check2(type, 0x101FD6D0, 0x101FD9DC, 0x10006448, 0x101FD594, 0x101FD980, 0x10006428);
}
VERIFY(0x0203F9D0, Rtti_0203F9D0);

static u32 Rtti_0203FA48(u32 self, u32 type) {
    WWHD_FUNC(0x0203FA48, u32, self, type);
    return rtti_check2(type, 0x101FD720, 0x101FD9E0, 0x10006438, 0x101FD2E0, 0x101FD994, 0x10006428);
}
VERIFY(0x0203FA48, Rtti_0203FA48);

/* 0203FAC0: new (heap, align) T() with a 0x44-byte T (ctor 0275E11C) */
static u32 Create_0203FAC0(u32 unused, u32 heap, u32 align) {
    WWHD_FUNC(0x0203FAC0, u32, unused, heap, align);
    u32 p = new_heap(0x44, heap, align);
    if (p == 0) return 0;
    return gabi::call<u32>(0x0275E11C, p);
}
VERIFY(0x0203FAC0, Create_0203FAC0);

static void Empty_0203FAF0() {
    WWHD_FUNC(0x0203FAF0, void);
}
VERIFY(0x0203FAF0, Empty_0203FAF0);

/* sead task factories: new (heaps[current], 4) T(arg) */
static inline u32 task_heap(u32 arg) {
    u32 heaps = ld(arg);
    u32 idx = ld(heaps + 0x14);
    return ld(heaps + idx * 4);
}
static inline u32 make_task(u32 arg, u32 size, u32 ctor) {
    u32 p = new_heap(size, task_heap(arg), 4);
    if (p == 0) return 0;
    return gabi::call<u32>(ctor, p, arg);
}
static u32 Create_0203FAF4(u32 arg) {
    WWHD_FUNC(0x0203FAF4, u32, arg);
    return make_task(arg, 0xD0, 0x02761004);
}
VERIFY(0x0203FAF4, Create_0203FAF4);

static u32 Create_0203FB48(u32 arg) {
    WWHD_FUNC(0x0203FB48, u32, arg);
    return make_task(arg, 0x16AC, 0x027294A8);
}
VERIFY(0x0203FB48, Create_0203FB48);

static u32 Create_0203FB9C(u32 arg) {
    WWHD_FUNC(0x0203FB9C, u32, arg);
    return make_task(arg, 0xCC, 0x0203FC98);
}
VERIFY(0x0203FB9C, Create_0203FB9C);

static u32 Create_0203FBF0(u32 arg) {
    WWHD_FUNC(0x0203FBF0, u32, arg);
    return make_task(arg, 0xCC, 0x0203E380);
}
VERIFY(0x0203FBF0, Create_0203FBF0);

/* 0203FC44: destructor of the single-instance member (+0xC8 of the task): clears the instance
 * global 1018F5A8 when this member was the one that claimed it */
static void Single_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203FC44, void, p, flags);
    if (p == 0) return;
    if (lbz(p) != 0) st(0x1018F5A8, 0);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203FC44, Single_dt);

/* 0203FC70: non-virtual thunk of the SystemTask destructor (secondary base at +0x14) */
static void SystemTask_dt_thunk(u32 p, u32 flags) {
    WWHD_FUNC(0x0203FC70, void, p, flags);
    gabi::call(0x02040138, p - 0x14, flags);
}
VERIFY(0x0203FC70, SystemTask_dt_thunk);

/* 0203FC78: claim the single instance (first task wins) */
static void SystemTask_claimInstance(u32 self) {
    WWHD_FUNC(0x0203FC78, void, self);
    if (ld(0x1018F5A8) != 0) return;
    st(0x1018F5A8, self);
    stb(self + 0xC8, 1);
}
VERIFY(0x0203FC78, SystemTask_claimInstance);

/* 0203FC98: SystemTask constructor (0xCC; sead task base with the class's name info 100067F4) */
static u32 SystemTask_ct(u32 self, u32 arg) {
    WWHD_FUNC(0x0203FC98, u32, self, arg);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xCC);
        if (t == 0) return 0;
    }
    gabi::call(0x027454E0, t, arg, 0x100067F4);
    st(t + 0x70, 0x10006828);
    st(t + 0x20, 0x100068E0);
    stb(t + 0xC8, 0);
    return t;
}
VERIFY(0x0203FC98, SystemTask_ct);

/* 0203FD18: prepare: system managers, two expanded heaps (0x1000 and 0x300000 bytes, names
 * 10006800 / 10006808) under the root heap 0203E8E8 with their managers, main01, then the base */
static void SystemTask_prepare(u32 self) {
    WWHD_FUNC(0x0203FD18, void, self);
    gabi::call(0x0271AFA8, ld(ld(0x1018C404) + 0x10));
    gabi::call(0x0271B200, ld(0x101F8378));
    gabi::call(0x025F93B4, ld(ld(0x1018C404) + 0x10));
    gabi::call(0x025F9448, ld(0x101F4BAC));
    gabi::Local<be<u32>[2]> n1;
    st(n1.a + 4, 0x100067B4);
    st(n1.a + 0, 0x10006800);
    u32 parent = gabi::call<u32>(0x0203E8E8);
    u32 h1 = gabi::call<u32>(0x02753004, 0x1000u, n1.a, parent, 1u, 0u);
    gabi::call(0x025F7658, h1);
    gabi::call(0x025F90C4, ld(0x101F4B5C), h1);
    gabi::Local<be<u32>[2]> n2;
    st(n2.a + 4, 0x100067B4);
    st(n2.a + 0, 0x10006808);
    parent = gabi::call<u32>(0x0203E8E8);
    u32 h2 = gabi::call<u32>(0x02753004, 0x300000u, n2.a, parent, 1u, 0u);
    gabi::call(0x025F9818, h2);
    gabi::call(0x025F9A90, ld(0x101F4BD8), h2);
    gabi::call(0x025F1660);
    gabi::call(0x027488E8, self);
}
VERIFY(0x0203FD18, SystemTask_prepare);

/* 0203FE20: enter: create the two child tasks (factories 02040220 / 02040274, create callbacks
 * 02032DD0 / 020355D8) through the task manager (+0x5C), each followed by a type check of the
 * created task; then, when the debug draw global 101F8B10 is set, viewport setup */
static void child_task(u32 self, u32 factory, u32 cb, bool heapsz) {
    gabi::Local<be<u32>[2]> id;
    gabi::Local<be<u32>[32]> src;
    gabi::Local<be<u32>[32]> arg;
    st(id.a + 0, 2);
    st(id.a + 4, factory);
    gabi::call(0x02748B98, src.a, id.a);
    if (heapsz) st(src.a + 0xC + ld(src.a + 0x58) * 0x14, 0x2800);
    for (u32 i = 0; i < 32; i++) st(arg.a + i * 4, ld(src.a + i * 4));
    st(arg.a + 0x7C, cb);
    u32 t = gabi::call<u32>(0x02749ECC, ld(self + 0x5C), arg.a);
    if (ld(0x101FD598) == 0) { st(0x101FD97C, 0x100067DC); st(0x101FD598, 1); }
    if (t != 0) gabi::call_ptr<u32>(vfn(t, 0x70, 0xC), t, 0x101FD97C);
}
static void SystemTask_enter(u32 self) {
    WWHD_FUNC(0x0203FE20, void, self);
    child_task(self, 0x02040220, 0x02032DD0, true);
    child_task(self, 0x02040274, 0x020355D8, false);
    if (ld(0x101F8B10) == 0) return;
    gabi::Local<be<u32>[6]> a;
    gabi::Local<be<u32>[6]> b;
    gabi::Local<be<u32>[6]> c;
    gabi::call(0x0274F358, a.a);
    gabi::call(0x0274F424, b.a, 0.0f, 0.0f, 1.0f, 1.0f);
    u32 o = gabi::call<u32>(0x02748974, self);
    gabi::call_ptr<u32>(vfn(o, 0x24, 0x3C), o, 2u);
    gabi::call(0x0274F424, b.a, 0.0f, 0.0f, 1.0f, 1.0f);
    gabi::call(0x0274F424, c.a, 0.0f, 0.0f, 1.0f, 1.0f);
}
VERIFY(0x0203FE20, SystemTask_enter);

}  // namespace hd_sys_0203F8CC
