/* Local helpers of the HD units of block 0x02000000..0x0203FFFF. Used only by wwhd_src/hd/hd_*.cpp of this block. */
#pragma once
#include "wwhd.h"

namespace hdblk {
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 lhz(u32 a) { return gabi::load<u16>(a); }
static inline u8 lbz(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void sth(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void stb(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
/* virtual function at OFF of the vtable whose pointer is at obj+VPTR */
static inline u32 vfn(u32 obj, u32 vptr, u32 off) { return ld(ld(obj + vptr) + off); }

static inline u32 op_new(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void op_delete(u32 p) { gabi::call(0x0273AF40, p); }

/* The 148-byte header initialiser every TU of the block ends with: a zeroed 16-byte object at OBJ
 * (registered with REG), the constants -pi/pi (FSRC, FSRC+4) stored at OBJ-0xC/OBJ-8, then two
 * one-byte objects at OBJ-4 / OBJ-3 constructed and registered (REG+0xC / REG+0x18). */
static inline void header_sinit_at(u32 obj, u32 reg, u32 fsrc, u32 fdst, u32 bdst) {
    st(obj + 8, 0); st(obj + 0, 0); st(obj + 0xC, 0); st(obj + 4, 0);
    gabi::call(0x028F026C, reg);
    f32 a = ldf(fsrc), b = ldf(fsrc + 4);
    stf(fdst, a);
    stf(fdst + 4, b);
    gabi::call(0x028ED6F8, bdst);
    gabi::call(0x028F026C, reg + 0xC);
    gabi::call(0x028EAB2C, bdst + 1);
    gabi::call(0x028F026C, reg + 0x18);
}
/* the usual layout: the -pi/pi pair at OBJ-0xC, the two one-byte objects at OBJ-4 / OBJ-3 */
static inline void header_sinit(u32 obj, u32 reg, u32 fsrc) { header_sinit_at(obj, reg, fsrc, obj - 0xC, obj - 4); }
/* coreinit imports (028FDC00 / 028FDC08 are their stubs) */
static constexpr u32 OSBlockMove = 0xC0009988;
static constexpr u32 OSBlockSet = 0xC0009990;
/* fdivs with PowerPC NaN propagation (first NaN operand, quieted) */
static inline f32 fdivs_ppc(f32 a, f32 b) {
    if (a != a) return gabi::ppc_qnan(a);
    if (b != b) return gabi::ppc_qnan(b);
    return (f32)((f64)a / (f64)b);
}
/* sead RTTI: getRuntimeTypeInfoStatic() of a class two levels below the root type info: function-local
 * statics with guards (GA/OA: the class, GB/OB: its parent, GR/OR: the root); returns OA */
static inline u32 rtti_static3(u32 ga, u32 oa, u32 gb, u32 ob, u32 gr, u32 orr) {
    if (ld(ga) != 0) return oa;
    u32 b = ld(gb);
    st(ga, 1);
    if (b != 0) { st(oa, ob); return oa; }
    u32 r = ld(gr);
    st(gb, 1);
    if (r == 0) { st(gr, 1); st(orr, 0); }
    st(oa, ob);
    st(ob, orr);
    return oa;
}
/* the same for a class directly below the root (inlined into callers) */
static inline void rtti_static2(u32 ga, u32 oa, u32 gr, u32 orr) {
    if (ld(ga) != 0) return;
    u32 r = ld(gr);
    st(ga, 1);
    if (r == 0) { st(gr, 1); st(orr, 0); }
    st(oa, orr);
}
/* sead DynamicCast: walk the type-info parent chain of OBJ (getRuntimeTypeInfo = slot SLOT of the vtable
 * at OBJ+VPTR) looking for TI; returns OBJ or 0 */
static inline u32 dyncast(u32 obj, u32 vptr, u32 slot, u32 ti) {
    if (obj == 0) return 0;
    u32 t = gabi::call_ptr<u32>(vfn(obj, vptr, slot), obj);
    if (t == 0) return 0;
    for (; t != 0; t = ld(t))
        if (t == ti) return obj;
    return 0;
}
/* sead SafeString length: characters before the NUL, 0 when there are more than 0x40000 */
static inline s32 sstrlen(u32 c) {
    s32 len = 0;
    if (lbz(c) == 0) return 0;
    for (;;) {
        ++len;
        ++c;
        if (len > 0x40000) return 0;
        if (lbz(c) == 0) return len;
    }
}
/* sead SafeString equality of two different buffers (bounded by 0x40001 characters; exhaustion = unequal) */
static inline bool sstr_eq(u32 p, u32 q) {
    for (u32 k = 0; k < 0x40001; ++k) {
        u8 c0 = lbz(p + k), c1 = lbz(q + k);
        if (c0 != c1) return false;
        if (c0 == 0) return true;
    }
    return false;
}
/* virtual assureTermination (vtable slot 0x14) of a SafeString {data, vtable} */
static inline void sstr_assure(u32 s) { gabi::call_ptr(ld(ld(s + 4) + 0x14), s); }
}  // namespace hdblk
