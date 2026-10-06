/* hd_sys_0203E1D0: the HD ProfileTask translation unit (cking::system) with the weak companions in front
 * of it, 0203E1D0..0203E583. No GameCube source: HD-only code.
 *
 * 0203E1D0..0203E2D7 are companions emitted before the task code: the static-array destructor of the
 * Miiverse operation manager's 50-element array (10202CA0, built by the __sinit 0203E0F8 of the
 * OliveOperationMgr TU, so probably that TU's), deleting destructors, fixed SafeString / WSafeString
 * buffer terminators and the sead delegate (pointer-to-member) invoke. Then the ProfileTask: the
 * single-instance member destructor and claim (global 1018F538), the constructor (0xCC), the forwarded
 * calc, the TU's __sinit, the destructor and the sead RTTI check; an earlier map ended this TU at 0203E497,
 * its companions run to 0203E583. */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_0203E1D0 {

/* 0203E1D0: destroy the static array (50 x 0xC208, element dtor 0203E2D8) */
static void DestroyArray_10202CA0() {
    WWHD_FUNC(0x0203E1D0, void);
    gabi::call(0x028F0164, 0x10202CA0, 0x32u, 0xC208u, 0x0203E2D8, 0u, 0u);
}
VERIFY(0x0203E1D0, DestroyArray_10202CA0);

static void Dt_0203E1F8(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E1F8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E1F8, Dt_0203E1F8);

static void Dt_0203E20C(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E20C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E20C, Dt_0203E20C);

static void Empty_0203E220() {
    WWHD_FUNC(0x0203E220, void);
}
VERIFY(0x0203E220, Empty_0203E220);

/* 0203E224: fixed SafeString buffer: terminate at the last char (buffer +0, capacity +8) */
static void SafeBuf_assureTerminate(u32 self) {
    WWHD_FUNC(0x0203E224, void, self);
    stb(ld(self) + ld(self + 8) - 1, 0);
}
VERIFY(0x0203E224, SafeBuf_assureTerminate);

static void Dt_0203E23C(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E23C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E23C, Dt_0203E23C);

static void Empty_0203E250() {
    WWHD_FUNC(0x0203E250, void);
}
VERIFY(0x0203E250, Empty_0203E250);

/* 0203E254: the same for a 16-bit (char16) buffer */
static void WSafeBuf_assureTerminate(u32 self) {
    WWHD_FUNC(0x0203E254, void, self);
    sth(ld(self) + ld(self + 8) * 2 - 2, 0);
}
VERIFY(0x0203E254, WSafeBuf_assureTerminate);

static void Dt_0203E270(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E270, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E270, Dt_0203E270);

/* 0203E284: sead delegate invoke: object +4, GHS pointer-to-member {s16 this delta +8, s16 index +0xA
 * (negative: plain function +0xC; positive: virtual, vtable offset s16 +0xE, slot index*8+4)};
 * the arguments r4/r5 pass through (r6..r8 are scratch in the thunk) */
static u32 Delegate_invoke(u32 self, u32 a4, u32 a5) {
    WWHD_FUNC(0x0203E284, u32, self, a4, a5);
    u32 obj = ld(self + 4);
    if (obj == 0) return self;
    s32 idx = (s16)lhz(self + 0xA);
    if (idx == 0) return self;
    u32 t = obj + (u32)(s32)(s16)lhz(self + 8);
    u32 fn;
    if (idx < 0) {
        fn = ld(self + 0xC);
    } else {
        u32 vt = ld(t + (u32)(s32)(s16)lhz(self + 0xE));
        fn = ld(vt + (u32)idx * 8 + 4);
    }
    return gabi::call_ptr<u32>(fn, t, a4, a5);
}
VERIFY(0x0203E284, Delegate_invoke);

/* 0203E2D8: deleting destructor of the array element (an nn::olv::DownloadedDataBase, 0xC208) */
static void Elem_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E2D8, void, p, flags);
    if (p == 0) return;
    gabi::call(0xC0004FF0, p, 0u); /* nn::olv::DownloadedDataBase::~DownloadedDataBase (import) */
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E2D8, Elem_dt);

/* 0203E32C: destructor of the single-instance member (+0xC8 of the task) */
static void Single_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E32C, void, p, flags);
    if (p == 0) return;
    if (lbz(p) != 0) st(0x1018F538, 0);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E32C, Single_dt);

/* 0203E358: non-virtual thunk of the ProfileTask destructor (secondary base at +0x14) */
static void ProfileTask_dt_thunk(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E358, void, p, flags);
    gabi::call(0x0203E498, p - 0x14, flags);
}
VERIFY(0x0203E358, ProfileTask_dt_thunk);

/* 0203E360: claim the single instance */
static void ProfileTask_claimInstance(u32 self) {
    WWHD_FUNC(0x0203E360, void, self);
    if (ld(0x1018F538) != 0) return;
    st(0x1018F538, self);
    stb(self + 0xC8, 1);
}
VERIFY(0x0203E360, ProfileTask_claimInstance);

/* 0203E380: ProfileTask constructor (0xCC) */
static u32 ProfileTask_ct(u32 self, u32 arg) {
    WWHD_FUNC(0x0203E380, u32, self, arg);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xCC);
        if (t == 0) return 0;
    }
    gabi::call(0x027454E0, t, arg, 0x1000621C);
    st(t + 0x70, 0x10006230);
    st(t + 0x20, 0x100062E8);
    stb(t + 0xC8, 0);
    return t;
}
VERIFY(0x0203E380, ProfileTask_ct);

/* 0203E400: forwards to the task base (027488E8) */
static void ProfileTask_fwd(u32 self) {
    WWHD_FUNC(0x0203E400, void, self);
    gabi::call(0x027488E8, self);
}
VERIFY(0x0203E400, ProfileTask_fwd);

static void sinit_0203E404() {
    WWHD_FUNC(0x0203E404, void);
    header_sinit(0x1046123C, 0x1018F514, 0x10006228);
}
VERIFY(0x0203E404, sinit_0203E404);

/* 0203E498: ProfileTask destructor */
static void ProfileTask_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E498, void, p, flags);
    if (p == 0) return;
    st(p + 0x20, 0x100062E8);
    gabi::call(0x0203E32C, p + 0xC8, 2u);
    gabi::call(0x027452C8, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E498, ProfileTask_dt);

/* 0203E504: sead checkDerivedRuntimeTypeInfo */
static u32 ProfileTask_checkRtti(u32 self, u32 type) {
    WWHD_FUNC(0x0203E504, u32, self, type);
    if (ld(0x101FD598) == 0) { st(0x101FD598, 1); st(0x101FD97C, 0x10006204); }
    if (type == 0x101FD97C) return 1;
    if (ld(0x101FD594) == 0) { st(0x101FD594, 1); st(0x101FD980, 0x100061F4); }
    return type == 0x101FD980;
}
VERIFY(0x0203E504, ProfileTask_checkRtti);

static void Empty_0203E57C() {
    WWHD_FUNC(0x0203E57C, void);
}
VERIFY(0x0203E57C, Empty_0203E57C);

static void Empty_0203E580() {
    WWHD_FUNC(0x0203E580, void);
}
VERIFY(0x0203E580, Empty_0203E580);

}  // namespace hd_sys_0203E1D0
