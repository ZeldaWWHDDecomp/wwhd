/* d_s_play part 2: phase_3, phase_5, the HD ocean heap, phase_compleate, Create, the HIO
 * constructors, __sinit and the per-TU inline copies at the end of the unit. WWHD. See d_s_play.cpp for the unit's range and layouts.
 *
 * Phase table (101EAC64): phase_00, phase_01, phase_0 .. phase_5, phase_6 (025B375C, HD: no
 * REL preloading, returns NEXT), phase_compleate (025B3114, HD: creates the ocean heap on the
 * "sea" stage, returns NEXT), 025B3764 (returns COMPLEATE). PreLoadInfoT (HD 100530D8, 12-byte
 * entries): {stage name, resource names, count (u8 at +8)}; preLoadNo is 101EACB4. */
#include "bindings.h"
#include <cstring>

namespace d_s_play_2_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline s32 dComIfG_resLoad_l(u32 phase, u32 name) { return gabi::call<s32>(0x02520460, phase, name); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }
static inline void __register_global_object_l(u32 d) { gabi::call(0x028F026C, d); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 sOceanHeap = 0x1047C8B4;
struct sstr_l { be<u32> str, vt; };

/* 025B1F54: HD: waits for an HD system (101F7274) and starts it once */
static s32 phase_3(u32 i_this) {
    WWHD_FUNC(0x025B1F54, s32, i_this);
    if (gabi::call<s32>(0x026FBB7C, ld(0x101F7274)) == 0) return 0;
    u32 o = ld(0x101F4F28);
    if (ld8(o + 0x212C) == 0) {
        st8(o + 0x212C, 1);
        gabi::call(0x02612A80, ld(0x101F4F7C));
    }
    u32 cmd = ld(i_this + 0x1D0); /* sceneCommand */
    if (cmd != 0 && ld8(cmd + 0xC) == 0) return 0; /* !sync() */
    if (gabi::call<s32>(0x025E18B8) != 0) return 0;  /* check1stDynamicWave */
    return 2;
}
VERIFY(0x025B1F54, phase_3);

/* 025B2F90 */
static s32 phase_5(u32 i_this) {
    WWHD_FUNC(0x025B2F90, s32, i_this);
    s8 preLoadNo = (s8)ld8(0x101EACB4);
    if (preLoadNo < 0) return 2;
    u32 info = 0x100530D8 + preLoadNo * 0xC;
    u32 resName = ld(info + 4);
    s32 rt = 2;
    u32 num = ld8(info + 8);
    if (resName != 0 && ld(resName) != 0) {
        if (num > 0x23) JUT_ASSERT_l(0x100533FC, 0x127C, 0x1005340C);
        if ((s32)num > 0) {
            u32 p = resName;
            u32 off = 0;
            do {
                if (dComIfG_resLoad_l(0x1047B448 + off, ld(p)) != 4) rt = 0;
                p += 4;
                off += 8;
            } while (--num != 0);
        }
    }
    return rt;
}
VERIFY(0x025B2F90, phase_5);

/* 025B306C: HD-only: creates the ocean heap (sOceanHeap_, "LOD48" unit heap) */
static void dScnPly_createOceanHeap() {
    WWHD_FUNC(0x025B306C, void);
    u32 size = gabi::call<u32>(0x02754C90, 0x20) + 0x5000;
    gabi::Local<sstr_l> name;
    name->vt = 0x10052EEC;
    name->str = 0x10053444;
    u32 parent = gabi::call<u32>(0x025E2FBC);
    if (ld(sOceanHeap) != 0) JUT_ASSERT_l(0x10053458, 0x110, 0x10053468);
    st(sOceanHeap, gabi::call<u32>(0x027564DC, size, 0x30, gabi::ea(name.get()), 0x100, parent, 0));
}
VERIFY(0x025B306C, dScnPly_createOceanHeap);

/* 025B3114: HD: on the "sea" stage the ocean heap is created; returns NEXT (025B3764 completes) */
static s32 phase_compleate(u32 i_this) {
    WWHD_FUNC(0x025B3114, s32, i_this);
    u32 play = dComIfGp_ea();
    gabi::Local<sstr_l> stage, sea;
    sea->vt = 0x10052EEC;
    sea->str = 0x1005347C; /* "sea" */
    stage->str = play + 0x5134;
    stage->vt = 0x10052EEC;
    gabi::call(0x025B376C, gabi::ea(stage.get()));
    gabi::call_ptr(ld(stage->vt + 0x14), gabi::ea(stage.get()));
    u32 s1 = stage->str;
    gabi::call_ptr(ld(sea->vt + 0x14), gabi::ea(sea.get()));
    u32 s2 = sea->str;
    bool eq = s1 == s2;
    if (!eq) {
        for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
            u8 c = ld8(s1);
            if (c != ld8(s2)) break;
            if (c == 0) { eq = true; break; }
        }
    }
    if (eq) dScnPly_createOceanHeap();
    return 2;
}
VERIFY(0x025B3114, phase_compleate);

/* 025B31E4 */
static s32 dScnPly_Create(u32 i_this) {
    WWHD_FUNC(0x025B31E4, s32, i_this);
    return gabi::call<s32>(0x02525FE4, i_this + 0x1C8, 0x101EAC64, i_this); /* dComLbG_PhaseHandler */
}
VERIFY(0x025B31E4, dScnPly_Create);

/* 025B31F8: dScnPly_reg_childHIO_c::dScnPly_reg_childHIO_c (HD 0x90, vtable +0) */
static u32 dScnPly_reg_childHIO_c_ct(u32 self) {
    WWHD_FUNC(0x025B31F8, u32, self);
    if (self == 0) {
        self = operator_new_l(0x90);
        if (self == 0) return self;
    }
    st(self + 0, 0x10052FBC);
    for (int i = 0; i < 30; i++) stf(self + 4 + 4 * i, 0.0f);
    for (int i = 0; i < 10; i++) st16(self + 0x7C + 2 * i, 0);
    return self;
}
VERIFY(0x025B31F8, dScnPly_reg_childHIO_c_ct);

/* 025B326C: dScnPly_dark_HIO_c::dScnPly_dark_HIO_c (HD 0x28, vtable +0x24) */
static u32 dScnPly_dark_HIO_c_ct(u32 self) {
    WWHD_FUNC(0x025B326C, u32, self);
    if (self == 0) {
        self = operator_new_l(0x28);
        if (self == 0) return self;
    }
    st8(self + 1, 0);
    st(self + 0x24, 0x10052FD4);
    st8(self + 2, 0);
    for (int i = 0; i < 8; i++) st(self + 4 + 4 * i, ld(0x101EE5C8 + 4 * i)); /* dStage_roomControl_c::getDarkStatus(0) */
    return self;
}
VERIFY(0x025B326C, dScnPly_dark_HIO_c_ct);

/* 025B32FC: dScnPly_env_otherHIO_c::dScnPly_env_otherHIO_c (HD 0x48, vtable +0) */
static u32 dScnPly_env_otherHIO_c_ct(u32 self) {
    WWHD_FUNC(0x025B32FC, u32, self);
    if (self == 0) {
        self = operator_new_l(0x48);
        if (self == 0) return self;
    }
    st(self + 0, 0x10052FFC);
    st8(self + 0x04, 1);
    st8(self + 0x05, 1);
    st8(self + 0x06, 0);
    st8(self + 0x07, 0);
    st8(self + 0x08, 0);
    st8(self + 0x09, 0);
    st8(self + 0x0A, 0);
    st8(self + 0x0B, 0);
    st8(self + 0x0C, 0x80);
    st8(self + 0x0D, 0x10);
    st8(self + 0x0E, 0);
    st8(self + 0x0F, 0);
    st8(self + 0x10, 0);
    st8(self + 0x11, 0);
    st16(self + 0x40, 0);
    stf(self + 0x44, 4000.0f);
    st8(self + 0x14, 0);
    for (int i = 0; i < 20; i++) st16(self + 0x18 + 2 * i, 0xFFFF);
    st8(self + 0x15, 0);
    return self;
}
VERIFY(0x025B32FC, dScnPly_env_otherHIO_c_ct);

/* 025B33B8: dScnPly_env_debugHIO_c::dScnPly_env_debugHIO_c (HD 0x2C, vtable +0) */
static u32 dScnPly_env_debugHIO_c_ct(u32 self) {
    WWHD_FUNC(0x025B33B8, u32, self);
    if (self == 0) {
        self = operator_new_l(0x2C);
        if (self == 0) return self;
    }
    stf(self + 0x04, -100.0f);
    stf(self + 0x08, -100.0f);
    stf(self + 0x0C, -100.0f);
    stf(self + 0x10, 100.0f);
    stf(self + 0x14, 100.0f);
    stf(self + 0x18, 100.0f);
    stf(self + 0x1C, 0.0f);
    stf(self + 0x20, 0.0f);
    stf(self + 0x24, 0.0f);
    stf(self + 0x28, 100.0f);
    st(self + 0, 0x10053014);
    return self;
}
VERIFY(0x025B33B8, dScnPly_env_debugHIO_c_ct);

/* 025B3438: dScnPly_msg_HIO_c::dScnPly_msg_HIO_c (HD 0x14, vtable +0) */
static u32 dScnPly_msg_HIO_c_ct(u32 self) {
    WWHD_FUNC(0x025B3438, u32, self);
    if (self == 0) {
        self = operator_new_l(0x14);
        if (self == 0) return self;
    }
    st8(self + 5, 0);
    st8(self + 7, 0);
    st16(self + 0xC, 1);   /* mID */
    st(self + 0, 0x10053044);
    st(self + 0x10, (u32)-1);
    st8(self + 4, 0);      /* mIsUpdate */
    st8(self + 8, 0);
    st16(self + 0xA, 0);   /* mGroup */
    st8(self + 6, 0);
    return self;
}
VERIFY(0x025B3438, dScnPly_msg_HIO_c_ct);

/* 025B34A4 */
static void __sinit_d_s_play_cpp() {
    WWHD_FUNC(0x025B34A4, void, (u32)0);
    for (int i = 0; i < 4; i++) st(0x1047B438 + 4 * i, 0);
    __register_global_object_l(0x101EAC90);
    stf(0x1047B41C, -3.1415927f);
    stf(0x1047B420, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1047B434);
    __register_global_object_l(0x101EAC9C);
    gabi::call(0x028EAB2C, 0x1047B435);
    __register_global_object_l(0x101EACA8);
    stf(0x1047B424, 50000.0f);
    stf(0x1047B42C, 10000.0f);
    stf(0x1047B428, 50000.0f);
    stf(0x1047B430, 10000.0f);
    dScnPly_dark_HIO_c_ct(0x1047B560);                                    /* g_darkHIO */
    st(0x1047B608, 0x10052FE4);                                           /* g_regHIO */
    gabi::call(0x028EFFD0, 0x1047B60C, 0x21, 0x90, 0x025B31F8);            /* __construct_array of the children */
    st(0x1047B588, 0x1005302C);                                           /* g_envHIO */
    dScnPly_env_otherHIO_c_ct(0x1047B58C);
    dScnPly_env_debugHIO_c_ct(0x1047B5D4);
    dScnPly_msg_HIO_c_ct(0x1047C89C);                                     /* g_msgDHIO */
    st(0x1047B418, 0x1005305C);
}
VERIFY(0x025B34A4, __sinit_d_s_play_cpp);

/* per-TU inline deleting destructors without members (025B35E8, 025B3734, 025B3748, 025B3770,
 * 025B3784, 025B3798, 025B37AC) */
static void inline_dt_025B35E8(u32 self, u32 flags) {
    WWHD_FUNC(0x025B35E8, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B35E8, inline_dt_025B35E8);

static void inline_dt_025B3734(u32 self, u32 flags) {
    WWHD_FUNC(0x025B3734, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B3734, inline_dt_025B3734);

static void inline_dt_025B3748(u32 self, u32 flags) {
    WWHD_FUNC(0x025B3748, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B3748, inline_dt_025B3748);

static void inline_dt_025B3770(u32 self, u32 flags) {
    WWHD_FUNC(0x025B3770, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B3770, inline_dt_025B3770);

static void inline_dt_025B3784(u32 self, u32 flags) {
    WWHD_FUNC(0x025B3784, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B3784, inline_dt_025B3784);

static void inline_dt_025B3798(u32 self, u32 flags) {
    WWHD_FUNC(0x025B3798, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B3798, inline_dt_025B3798);

static void inline_dt_025B37AC(u32 self, u32 flags) {
    WWHD_FUNC(0x025B37AC, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025B37AC, inline_dt_025B37AC);

/* 025B35FC: sead::SafeString assureTerminationImpl_ (this unit's copy): buffer[size - 1] = 0 */
static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x025B35FC, void, self);
    u32 str = ld(self + 0);
    u32 size = ld(self + 8);
    st8(str + size - 1, 0);
}
VERIFY(0x025B35FC, SafeString_assureTermination);

/* va_list of a variadic function: the GHS/EABI layout as the original places it at sp+8 (register
 * save area at +0x10, the caller's stack arguments at +0x88 = entry sp + 8) */
struct va_area_l {
    be<u8> gpr, fpr;
    be<u16> _pad0;
    be<u32> overflow_arg_area;
    be<u32> reg_save_area;
    be<u32> _pad1;
    be<u32> gprs[8]; /* r3..r10 */
    be<u32> fprs[16]; /* f1..f8 as doubles */
    be<u32> _pad2[4];
};

/* 025B3614: sead::FixedSafeString<8>::FixedSafeString(fmt, ...) (this unit's copy; formats with
 * formatV 02759C10) */
static u32 FixedSafeString8_ct_format(u32 self, u32 fmt, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    WWHD_FUNC(0x025B3614, u32, self, fmt, a5, a6, a7, a8, a9, a10);
    bool floats = gabi::cpu->cr[6] != 0; /* cr1eq: the caller passed floating-point arguments */
    gabi::Local<va_area_l> va;
    const u32 regs[8] = {self, fmt, a5, a6, a7, a8, a9, a10};
    for (int i = 0; i < 8; i++) va->gprs[i] = regs[i];
    if (floats) {
        for (int i = 0; i < 8; i++) {
            double d = gabi::cpu->f[1 + i].ps0;
            u64 bits;
            memcpy(&bits, &d, 8);
            va->fprs[2 * i] = (u32)(bits >> 32);
            va->fprs[2 * i + 1] = (u32)bits;
        }
    }
    if (self == 0) {
        self = operator_new_l(0x14);
        if (self == 0) return self;
    }
    /* SafeStringBase(buffer, 8) */
    st(self + 0, self + 0xC);
    st(self + 4, 0x10052F04);
    st(self + 8, 8);
    st8(self + 0x13, 0);
    u32 buf = ld(self + 0);
    st(self + 4, 0x10052F7C);
    st8(buf, 0);
    st(self + 4, 0x1005306C);
    u32 v = gabi::ea(va.get());
    va->gpr = 2;
    va->overflow_arg_area = v + 0x88;
    va->reg_save_area = v + 0x10;
    va->fpr = 0;
    gabi::call(0x02759C10, self, fmt, v); /* formatV(fmt, va) */
    return self;
}
VERIFY(0x025B3614, FixedSafeString8_ct_format);

/* 025B375C: phase_6 (HD: no REL preloading) */
static s32 phase_6(u32 i_this) {
    WWHD_FUNC(0x025B375C, s32, i_this);
    return 2;
}
VERIFY(0x025B375C, phase_6);

/* 025B3764: the last phase (complete) */
static s32 phase_end(u32 i_this) {
    WWHD_FUNC(0x025B3764, s32, i_this);
    return 4;
}
VERIFY(0x025B3764, phase_end);

/* 025B376C: an empty per-TU inline (called on SafeString temporaries) */
static void inline_empty_025B376C(u32 self) {
    WWHD_FUNC(0x025B376C, void, self);
    (void)self;
}
VERIFY(0x025B376C, inline_empty_025B376C);

/* 025B37C0 */
static s32 inline_true_025B37C0(u32 self) {
    WWHD_FUNC(0x025B37C0, s32, self);
    return 1;
}
VERIFY(0x025B37C0, inline_true_025B37C0);

/* 025B37C8: deletes the process and returns 0 */
static s32 inline_delete_025B37C8(u32 p) {
    WWHD_FUNC(0x025B37C8, s32, p);
    gabi::call(0x025DF944, p); /* fpcM_Delete */
    return 0;
}
VERIFY(0x025B37C8, inline_delete_025B37C8);

} // namespace d_s_play_2_cpp
