/* hd_sys_0203F8CC tail (WWHD): the part of the HD SystemTask translation unit that lies past
 * 0x02040000 (0204007C..020402C7). Verified against
 * cking.rpx.
 *
 * The TU starts at 0203F8CC inside the block (hd_sys_0203F8CC, "in-block part only");
 * these eight functions are its tail: a forwarding thunk, the TU's header __sinit, two
 * destructors and an empty virtual referenced from the TU's vtables (100067B4.., 10006874), the
 * sead runtime-type check of the task class (two function-local static type-info objects), and
 * two sead task factories ("new (heap, 4) T(arg)"). HD-only code (no GameCube counterpart). */
#include "bindings.h"

namespace hd_sys_0203F8CC_tail_cpp {

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void register_global_object_l(u32 rec) { gabi::call(0x028F026C, rec); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }
/* operator new(size, sead::Heap*, align) */
static inline u32 operator_new_heap_l(u32 size, u32 heap, u32 align) { return gabi::call<u32>(0x0273B050, size, heap, align); }

/* 0204007C: forwards to 025F9B14 when the global at 101F4BD8 is set (a vtable slot, 100068DC) */
u32 hdSys_forward_0204007C() {
    WWHD_FUNC(0x0204007C, u32);
    u32 p = ld(0x101F4BD8);
    if (p == 0) return 0;
    return gabi::call<u32>(0x025F9B14, p);
}
VERIFY(0x0204007C, hdSys_forward_0204007C);

/* 02040090 __sinit: the header statics every TU of this family gets */
void __sinit_hd_sys_0203F8CC() {
    WWHD_FUNC(0x02040090, void);
    const u32 bss = 0x104612E4, rec = 0x1018F584, ro = 0x1000681C;
    st(bss + 0xC + 8, 0); st(bss + 0xC, 0); st(bss + 0xC + 0xC, 0); st(bss + 0xC + 4, 0);
    register_global_object_l(rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    register_global_object_l(rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    register_global_object_l(rec + 0x18);
}
VERIFY(0x02040090, __sinit_hd_sys_0203F8CC);

/* 02040124: deleting destructor of a trivially destructible class (vtable 100067B4) */
void hdSys_deletingDtor_02040124(u32 p, u32 flags) {
    WWHD_FUNC(0x02040124, void, p, flags);
    if (p == 0) return;
    if (flags & 1) operator_delete_l(p);
}
VERIFY(0x02040124, hdSys_deletingDtor_02040124);

/* 02040138: destructor of the task class: member vtable at +0x20, member at +0xC8 destroyed,
 * then the base destructor (027452C8) */
void hdSys_dtor_02040138(u32 p, u32 flags) {
    WWHD_FUNC(0x02040138, void, p, flags);
    if (p == 0) return;
    st(p + 0x20, 0x100068E0);
    gabi::call(0x0203FC44, p + 0xC8, 2u);
    gabi::call(0x027452C8, p, 0u);
    if (flags & 1) operator_delete_l(p);
}
VERIFY(0x02040138, hdSys_dtor_02040138);

/* 020401A4: empty virtual (vtable 100067B4) */
void hdSys_empty_020401A4() {
    WWHD_FUNC(0x020401A4, void);
}
VERIFY(0x020401A4, hdSys_empty_020401A4);

/* 020401A8: sead checkDerivedRuntimeTypeInfo: this class's and its base's static type-info
 * objects (function-local statics with guard words) */
u32 hdSys_checkDerivedRuntimeTypeInfo(u32 self, u32 type) {
    WWHD_FUNC(0x020401A8, u32, self, type);
    if (ld(0x101FD598) == 0) { st(0x101FD598, 1); st(0x101FD97C, 0x100067DC); }
    if (type == 0x101FD97C) return 1;
    if (ld(0x101FD594) == 0) { st(0x101FD594, 1); st(0x101FD980, 0x100067CC); }
    return type == 0x101FD980;
}
VERIFY(0x020401A8, hdSys_checkDerivedRuntimeTypeInfo);

/* 02040220 / 02040274: task factories: new (heaps[current], 4) T(arg) */
static inline u32 task_heap(u32 arg) {
    u32 heaps = ld(arg);
    u32 idx = ld(heaps + 0x14);
    return ld(heaps + idx * 4);
}
u32 hdSys_createTask_02040220(u32 arg) {
    WWHD_FUNC(0x02040220, u32, arg);
    u32 obj = operator_new_heap_l(0x1AC4, task_heap(arg), 4);
    if (obj == 0) return 0;
    return gabi::call<u32>(0x02032DF0, obj, arg);
}
VERIFY(0x02040220, hdSys_createTask_02040220);

u32 hdSys_createTask_02040274(u32 arg) {
    WWHD_FUNC(0x02040274, u32, arg);
    u32 obj = operator_new_heap_l(0xCC, task_heap(arg), 4);
    if (obj == 0) return 0;
    return gabi::call<u32>(0x020355F8, obj, arg);
}
VERIFY(0x02040274, hdSys_createTask_02040274);

} // namespace hd_sys_0203F8CC_tail_cpp
