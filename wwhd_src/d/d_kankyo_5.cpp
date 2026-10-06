/* d_kankyo: environment / lighting core, part 5 of 6 (0255DAC0..02560E10), WWHD.
 * envcolor_init, dKy_setLight(_again), the colour getters/setters, fog parameter upload (HD
 * shader blocks), sound/sword flush/arrow colour hooks, tevstr init, wave/time/moon/stars
 * helpers and dKyr_player_overhead_bg_chk. See d_kankyo.cpp for the TU. */
#include "d_kankyo_local.h"

namespace d_kankyo_cpp {

static inline f32 sqdist_l(u32 a, u32 b) { return gabi::call<f32>(0x028E8DE8, a, b); } /* PSVECSquareDistance */
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
static inline void dKyw_rain_set_l(s32 n) { gabi::call(0x0257E7C0, n); }
void dKankyo_DayProc();
s32 dKy_get_dayofweek();
BOOL dKy_checkEventNightStop();
void GXColor_to_f32(u32 dst, u32 src);

struct SafeStr_l {
    be<u32> str;
    be<u32> vtbl;
};
static constexpr u32 SafeString_vtbl = 0x1004EF7C;
static inline bool stage_name_eq_l(u32 lit) {
    gabi::Local<SafeStr_l> a;
    a->str = lit;
    a->vtbl = SafeString_vtbl;
    u32 play = dComIfGp_get_l();
    gabi::Local<SafeStr_l> b;
    b->vtbl = SafeString_vtbl;
    b->str = play + 0x5134;
    gabi::call_ptr(ld(a->vtbl + 0x14), a.get());
    gabi::call_ptr(ld(a->vtbl + 0x14), a.get());
    u32 bv = b->vtbl;
    u32 s1 = a->str;
    gabi::call_ptr(ld(bv + 0x14), b.get());
    u32 s2 = b->str;
    if (s1 == s2) return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = ld8(s1), c2 = ld8(s2);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
        s1++;
        s2++;
    }
    return false;
}

/* 0255F2F8: dKy_Get_DifCol: HD copies actor K0 (u8 +0xB68) into the GXColorS10 +0xBA8 and returns it */
u32 dKy_Get_DifCol() {
    WWHD_FUNC(0x0255F2F8, u32);
    for (u32 i = 0; i < 3; i++) {
        u32 e1 = envlight_l();
        u8 v = ld8(envlight_l() + 0xB68 + i);
        st16(e1 + 0xBA8 + i * 2, v);
    }
    return envlight_l() + 0xBA8;
}
VERIFY(0x0255F2F8, dKy_Get_DifCol);

/* 0255F360 */
f32 dKy_yuragi_ratio_set(f32 v) {
    WWHD_FUNC(0x0255F360, f32, v);
    f32 k1000 = ldf(0x1004F29C);
    return gabi::fmadds(v, ldf(0x1004F104), k1000);
}
VERIFY(0x0255F360, dKy_yuragi_ratio_set);

/* 0255F378: mColChgFlag +0x109B */
void dKy_Itemgetcol_chg_on() {
    WWHD_FUNC(0x0255F378, void);
    u32 env = envlight_l();
    u8 f = ld8(env + 0x109B);
    if (f == 0 || f == 6) st8(env + 0x109B, 1);
}
VERIFY(0x0255F378, dKy_Itemgetcol_chg_on);

/* 0255F3B4 */
void dKy_Itemgetcol_chg_off() {
    WWHD_FUNC(0x0255F3B4, void);
    u32 env = envlight_l();
    if (ld8(env + 0x109B) == 4) st8(env + 0x109B, 5);
}
VERIFY(0x0255F3B4, dKy_Itemgetcol_chg_off);

/* 0255F3E8: dKy_arrowcol_chg_on(cXyz*, int mode): HD starts the colour pulse of mColChgFlag's high
 * nibble (mode 1: 0x20, mode 2: 0x30, else 0x10) instead of the GameCube arrow colour state */
void dKy_arrowcol_chg_on(u32 pos, s32 mode) {
    WWHD_FUNC(0x0255F3E8, void, pos, mode);
    u32 env = envlight_l();
    if (ld8(env + 0x109B) != 0) return;
    if (mode == 1) st8(env + 0x109B, 0x20);
    else if (mode == 2) st8(env + 0x109B, 0x30);
    else st8(env + 0x109B, 0x10);
}
VERIFY(0x0255F3E8, dKy_arrowcol_chg_on);

/* 0255F458: dKy_Sound_set(cXyz pos, int, fpc_ProcID, int): mSound +0x9B4 */
void dKy_Sound_set(u32 pos, s32 p2, u32 p3, u32 p4) {
    WWHD_FUNC(0x0255F458, void, pos, p2, p3, p4);
    u32 cam = ld(dComIfGp_get_l() + 0x5AF8);
    f32 d1 = sqrtf_l(sqdist_l(pos, cam + 0xDC));
    u32 env = envlight_l();
    f32 d2 = sqrtf_l(sqdist_l(env + 0x9B4, cam + 0xDC));
    if (!(d1 < d2)) return;
    if (!(d2 < ldf(0x1004F2FC))) {
        if (!((s32)ld(envlight_l() + 0x9C0) < p2)) return;
    }
    env = envlight_l();
    st(env + 0x9B4, ld(pos + 0));
    st(env + 0x9B8, ld(pos + 4));
    st(env + 0x9BC, ld(pos + 8));
    st(envlight_l() + 0x9C0, (u32)p2);
    st(envlight_l() + 0x9C8, p3);
    st(envlight_l() + 0x9C4, p4);
}
VERIFY(0x0255F458, dKy_Sound_set);

/* 0255F530 */
u32 dKy_Sound_get() {
    WWHD_FUNC(0x0255F530, u32);
    return envlight_l() + 0x9B4;
}
VERIFY(0x0255F530, dKy_Sound_get);

/* 0255F554: dKy_SordFlush_set(cXyz, int): mEfLightProc +0x7E0 */
void dKy_SordFlush_set(u32 pos, u32 type) {
    WWHD_FUNC(0x0255F554, void, pos, type);
    if (ld8(envlight_l() + 0x7E0) == 0) {
        st8(envlight_l() + 0x7E0, 1);
        st(envlight_l() + 0x7E4, type);
        u32 env = envlight_l();
        st(env + 0x7E8, ld(pos + 0));
        st(env + 0x7EC, ld(pos + 4));
        st(env + 0x7F0, ld(pos + 8));
    } else if (ld8(envlight_l() + 0x7E0) == 2) {
        st8(envlight_l() + 0x7E0, 4);
    }
}
VERIFY(0x0255F554, dKy_SordFlush_set);

/* 0255F5FC: dKy_FirstlightVec_get(cXyz*) returning cXyz (r3 = result storage, allocated when NULL) */
void dKy_FirstlightVec_get(u32 ret, u32 dst) {
    WWHD_FUNC(0x0255F5FC, void, ret, dst);
    f32 zero = ldf(0x1004F0B0);
    stf(dst + 8, zero);
    stf(dst + 4, zero);
    stf(dst + 0, zero);
    if (ret == 0) {
        ret = operator_new_l(0xC);
        if (ret == 0) return;
    }
    stf(ret + 0, ldf(dst + 0));
    stf(ret + 4, ldf(dst + 4));
    stf(ret + 8, ldf(dst + 8));
}
VERIFY(0x0255F5FC, dKy_FirstlightVec_get);

/* HD: the light parameter object of a slot: the index (s16 at *idxPtr) selects an entry of the
 * resource table 104B4708, sead DynamicCast through the type info (guard / static object
 * initialised with 1004EFA4 on first use), NULL when missing or of another type */
static inline u32 hd_param_obj_l(u32 idxPtr, u32 guard, u32 rtti) {
    s32 idx = lds16(ld(idxPtr));
    u32 tbl = ld(0x104B4708);
    u32 n = ld(tbl + 8);
    u32 arr = ld(tbl + 0xC);
    bool has;
    if ((u32)idx < n) has = ld16(arr + (u32)idx * 4 + 2) != 0;
    else has = ld16(arr + 2) != 0;
    u32 obj, g;
    if (has) {
        u32 e = (u32)idx < n ? arr + (u32)idx * 4 : arr;
        u32 n2 = ld(tbl + 0x10);
        u32 k = ld16(e);
        u32 p = 0;
        if (k < n2) p = ld(tbl + 0x14) + k * 4;
        g = ld(guard);
        obj = ld(p);
    } else {
        g = ld(guard);
        obj = 0;
    }
    if (g == 0) {
        st(guard, 1);
        st(rtti, 0x1004EFA4);
    }
    if (obj != 0) {
        if (!gabi::call_ptr<BOOL>(ld(ld(obj + 0x58) + 0x44), obj, rtti)) obj = 0;
    }
    return obj;
}

/* HD: the fog code keeps the GameCube view near/far checks but no longer uses them */
static inline void view_check_l() {
    if (ld(dComIfGp_get_l() + 0x5FA4) == 0) return;
    f32 zero = ldf(0x1004F0B0);
    if (ldf(ld(dComIfGp_get_l() + 0x5FA4) + 0xCC) < zero) return;
    if (ldf(ld(dComIfGp_get_l() + 0x5FA4) + 0xD0) < zero) return;
    u32 v1 = ld(dComIfGp_get_l() + 0x5FA4);
    u32 v2 = ld(dComIfGp_get_l() + 0x5FA4);
    if (!(ldf(v1 + 0xCC) < ldf(v2 + 0xD0))) return;
    dComIfGp_get_l();
    dComIfGp_get_l();
}

/* 0255F668: GxFogSet_Sub(GXColor*): HD writes the fog colour (floats) and start/end z (+0xFD0/+0xFD4)
 * into the fog parameter object (+0x114 colour, +0xF4/+0x104 z) */
void GxFogSet_Sub(u32 col) {
    WWHD_FUNC(0x0255F668, void, col);
    gabi::Local<be<u32>> c;
    *c = ld(col);
    view_check_l();
    u32 obj = hd_param_obj_l(0x104A1460, 0x101FD7DC, 0x101FDCE0);
    gabi::Local<be<f32>[4]> f;
    GXColor_to_f32(gabi::ea(f.get()), gabi::ea(c.get()));
    u32 fa = gabi::ea(f.get());
    st(obj + 0x114, ld(fa + 0));
    st(obj + 0x118, ld(fa + 4));
    st(obj + 0x11C, ld(fa + 8));
    st(obj + 0x120, ld(fa + 0xC));
    stf(obj + 0xF4, ldf(envlight_l() + 0xFD0));
    stf(obj + 0x104, ldf(envlight_l() + 0xFD4));
}
VERIFY(0x0255F668, GxFogSet_Sub);

/* 0255F84C: GxFog_set: fog colour +0xB8C (HD RGBA) */
void GxFog_set() {
    WWHD_FUNC(0x0255F84C, void);
    gabi::Local<be<u8>[4]> c;
    for (u32 i = 0; i < 4; i++) (*c)[i] = ld8(envlight_l() + 0xB8C + i);
    GxFogSet_Sub(gabi::ea(c.get()));
}
VERIFY(0x0255F84C, GxFog_set);

/* 0255F8A0: GxFog_sea_set: vrbox uso umi colour +0xB94 */
void GxFog_sea_set() {
    WWHD_FUNC(0x0255F8A0, void);
    gabi::Local<be<u8>[4]> c;
    for (u32 i = 0; i < 4; i++) (*c)[i] = ld8(envlight_l() + 0xB94 + i);
    GxFogSet_Sub(gabi::ea(c.get()));
}
VERIFY(0x0255F8A0, GxFog_sea_set);

/* fog from a tevstr (HD: colour s16 +0xA0.., start/end z +0xA8/+0xAC) */
static inline void tevstr_fog_l(u32 tev) {
    gabi::Local<be<u8>[4]> c;
    u8 b = (u8)lds16(tev + 0xA4), g = (u8)lds16(tev + 0xA2);
    (*c)[2] = b;
    (*c)[1] = g;
    u8 r = (u8)lds16(tev + 0xA0), a = (u8)lds16(tev + 0xA6);
    (*c)[0] = r;
    (*c)[3] = a;
    view_check_l();
    u32 obj = hd_param_obj_l(0x104A1460, 0x101FD7DC, 0x101FDCE0);
    gabi::Local<be<f32>[4]> f;
    GXColor_to_f32(gabi::ea(f.get()), gabi::ea(c.get()));
    u32 fa = gabi::ea(f.get());
    st(obj + 0x114, ld(fa + 0));
    st(obj + 0x118, ld(fa + 4));
    st(obj + 0x11C, ld(fa + 8));
    st(obj + 0x120, ld(fa + 0xC));
    stf(obj + 0xF4, ldf(tev + 0xA8));
    stf(obj + 0x104, ldf(tev + 0xAC));
}

/* 0255F8F4 */
void dKy_GxFog_tevstr_set(u32 tev) {
    WWHD_FUNC(0x0255F8F4, void, tev);
    tevstr_fog_l(tev);
}
VERIFY(0x0255F8F4, dKy_GxFog_tevstr_set);

/* 0255FAF0: identical to dKy_GxFog_tevstr_set in HD */
void dKy_GfFog_tevstr_set(u32 tev) {
    WWHD_FUNC(0x0255FAF0, void, tev);
    tevstr_fog_l(tev);
}
VERIFY(0x0255FAF0, dKy_GfFog_tevstr_set);

/* 0255FCEC: mColpatCurrGather +0x108F, mColpatCurr +0x108D, mColPatBlendGather +0xFCC */
void dKy_change_colset(u32 p0, u32 p1, f32 p2) {
    WWHD_FUNC(0x0255FCEC, void, p0, p1, p2);
    st8(envlight_l() + 0x108F, (u8)p1);
    u32 e1 = envlight_l();
    u8 g = ld8(envlight_l() + 0x108F);
    if (ld8(e1 + 0x108D) != g) stf(envlight_l() + 0xFCC, ldf(0x1004F0B0));
}
VERIFY(0x0255FCEC, dKy_change_colset);

/* 0255FD48 */
void dKy_change_colpat(u32 p0) {
    WWHD_FUNC(0x0255FD48, void, p0);
    st8(envlight_l() + 0x108F, (u8)p0);
    u32 e1 = envlight_l();
    u8 g = ld8(envlight_l() + 0x108F);
    if (ld8(e1 + 0x108D) != g) stf(envlight_l() + 0xFCC, ldf(0x1004F0B0));
}
VERIFY(0x0255FD48, dKy_change_colpat);

/* 0255FDA4 */
void dKy_custom_colset(u32 p0, u32 p1, f32 blend) {
    WWHD_FUNC(0x0255FDA4, void, p0, p1, blend);
    if (blend < ldf(0x1004F0A4)) st8(envlight_l() + 0x108E, (u8)p0);
    else st8(envlight_l() + 0x108E, (u8)p1);
    st8(envlight_l() + 0x108F, (u8)p1);
    stf(envlight_l() + 0xFCC, blend);
    st8(envlight_l() + 0x1098, 1);
}
VERIFY(0x0255FDA4, dKy_custom_colset);

/* 0255FE50: mTimeAdv +0x1028 */
void dKy_custom_timeset(f32 speed) {
    WWHD_FUNC(0x0255FE50, void, speed);
    stf(envlight_l() + 0x1028, speed);
}
VERIFY(0x0255FE50, dKy_custom_timeset);

/* 0255FE90: dKy_setLight_mine(dKy_tevstr_c*): HD writes the tevstr light colour (+0x18, as floats)
 * and position (+0..+8) into the light parameter object (+0xF4 colour, +0x12C position) */
void dKy_setLight_mine(u32 tev) {
    WWHD_FUNC(0x0255FE90, void, tev);
    u32 obj = hd_param_obj_l(0x104A1F70, 0x101FD7D4, 0x101FDCCC);
    gabi::Local<be<f32>[4]> f;
    GXColor_to_f32(gabi::ea(f.get()), tev + 0x18);
    u32 fa = gabi::ea(f.get());
    st(obj + 0xF4, ld(fa + 0));
    st(obj + 0xF8, ld(fa + 4));
    st(obj + 0xFC, ld(fa + 8));
    st(obj + 0x100, ld(fa + 0xC));
    f32 y = ldf(tev + 4), x = ldf(tev + 0), z = ldf(tev + 8);
    stf(obj + 0x12C, x);
    stf(obj + 0x130, y);
    stf(obj + 0x134, z);
}
VERIFY(0x0255FE90, dKy_setLight_mine);

/* 0255FFF4: dKy_tevstr_init(dKy_tevstr_c*, s8 roomNo, u8): HD tevstr 0x1C8 bytes cleared word-wise
 * (028F5914, 0x72 words); mRoomNo +0xB9, mEnvrIdxCurr/Prev +0xB5/+0xB6, mEnvrIdxOverride +0xBA,
 * mInitTimer +0xB4, mInitType +0xBC = 123; the HD light block copies are taken from the cleared
 * object (as the original reads them back). */
void dKy_tevstr_init(u32 tev, s32 roomNo, u32 p2) {
    WWHD_FUNC(0x0255FFF4, void, tev, roomNo, p2);
    gabi::call(0x028F5914, tev, 0x72);
    s32 r31 = roomNo;
    if (roomNo == -1) r31 = (s8)ld8(0x1047E6C8);
    st8(tev + 0xB9, (u8)r31);
    u32 r30 = p2;
    s16 r5 = lds16(tev + 0x1e);
    f32 f13 = ldf(0x1004F0A4);
    f32 f0 = ldf(0x1004F0B0);
    u8 r12 = ld8(tev + 0x18);
    stf(tev + 0x38, f13);
    st8(tev + 0xd8, (u8)r12);
    stf(tev + 0x40, f0);
    f32 f12 = ldf(tev + 4);
    stf(tev + 0x3c, f0);
    stf(tev + 0xc4, f12);
    stf(tev + 0x34, f0);
    stf(tev + 0x24, f13);
    stf(tev + 0x2c, f13);
    s16 r0 = lds16(tev + 0x1c);
    st8(tev + 0xb5, (u8)r31);
    st16(tev + 0xdc, (u16)r0);
    st8(tev + 0xb6, (u8)r31);
    u32 r8 = 0xff;
    f32 f10 = ldf(tev + 0x14);
    st8(tev + 0x9b, (u8)r8);
    stf(tev + 0xd4, f10);
    stf(tev + 0x30, f0);
    stf(tev + 0x28, f13);
    f32 f8 = ldf(tev + 0xc);
    st16(tev + 0xa6, (u16)r8);
    stf(tev + 0xcc, f8);
    u32 r11 = 0;
    st16(tev + 0x96, (u16)r8);
    st8(tev + 0x1b, (u8)r8);
    st16(tev + 0xde, (u16)r5);
    u32 r6 = 1;
    f32 f7 = ldf(tev + 8);
    f32 f9 = ldf(tev + 0x10);
    f32 f11 = ldf(tev + 0);
    stf(tev + 0x154, f9);
    stf(tev + 0xc0, f11);
    st8(tev + 0xb4, (u8)r6);
    st8(tev + 0xdb, (u8)r8);
    st8(tev + 0x19, (u8)r11);
    st8(tev + 0xd9, (u8)r11);
    s16 r9 = lds16(tev + 0x22);
    st8(tev + 0xda, (u8)r11);
    st16(tev + 0xe2, (u16)r9);
    stf(tev + 0xe4, f13);
    stf(tev + 0xe8, f13);
    stf(tev + 0xec, f13);
    stf(tev + 0xf0, f0);
    stf(tev + 0xf4, f0);
    stf(tev + 0xf8, f13);
    stf(tev + 0x158, f10);
    u32 r7a = 0x7b;
    stf(tev + 0x100, f0);
    st8(tev + 0xbc, (u8)r7a);
    s16 r7 = lds16(tev + 0x20);
    st8(tev + 0x15f, (u8)r11);
    st16(tev + 0x164, (u16)r7);
    st16(tev + 0x162, (u16)r5);
    stf(tev + 0x144, f11);
    stf(tev + 0x168, f13);
    st8(tev + 0x15c, (u8)r11);
    st16(tev + 0x160, (u16)r0);
    stf(tev + 0x148, f12);
    stf(tev + 0x16c, f13);
    stf(tev + 0x184, f0);
    st8(tev + 0x15e, (u8)r11);
    stf(tev + 0x174, f0);
    stf(tev + 0xc8, f7);
    stf(tev + 0xd0, f9);
    st16(tev + 0x166, (u16)r9);
    stf(tev + 0xfc, f0);
    stf(tev + 0x180, f0);
    stf(tev + 0x150, f8);
    stf(tev + 0x17c, f13);
    st8(tev + 0x1a, (u8)r11);
    stf(tev + 0x170, f13);
    st8(tev + 0xba, (u8)r30);
    st16(tev + 0xe0, (u16)r7);
    stf(tev + 0x178, f0);
    stf(tev + 0x14c, f7);
    st8(tev + 0x15d, (u8)r11);
}
VERIFY(0x0255FFF4, dKy_tevstr_init);

/* 0256019C: mRainCount +0xA40 */
s32 dKy_rain_check() {
    WWHD_FUNC(0x0256019C, s32);
    return (s32)ld(envlight_l() + 0xA40);
}
VERIFY(0x0256019C, dKy_rain_check);

/* 025601C0: mWaveChan +0x9D8.. (count +0x9F8) */
void dKy_usonami_set(f32 v) {
    WWHD_FUNC(0x025601C0, void, v);
    if (lds16(envlight_l() + 0x9F8) < 200) {
        stf(envlight_l() + 0x9DC, ldf(0x1004F1C4));
        stf(envlight_l() + 0x9E0, ldf(0x1004F300));
        st8(envlight_l() + 0x9FA, 0);
        stf(envlight_l() + 0x9E4, ldf(0x1004F304));
        stf(envlight_l() + 0x9E8, ldf(0x1004F178));
        stf(envlight_l() + 0x9EC, ldf(0x1004F308));
        st8(envlight_l() + 0x9FB, 0);
        stf(envlight_l() + 0x9F0, ldf(0x1004F30C));
        st16(envlight_l() + 0x9F8, 300);
        stf(envlight_l() + 0x9D8, ldf(0x1004F2A4));
    }
    stf(envlight_l() + 0x9F4, v);
}
VERIFY(0x025601C0, dKy_usonami_set);

/* 025602A8: mSchbit +0x109D */
u8 dKy_get_schbit() {
    WWHD_FUNC(0x025602A8, u8);
    return ld8(envlight_l() + 0x109D);
}
VERIFY(0x025602A8, dKy_get_schbit);

/* 025602CC: mSchbitTimer +0x1084 */
s32 dKy_get_schbit_timer() {
    WWHD_FUNC(0x025602CC, s32);
    return (s32)ld(envlight_l() + 0x1084);
}
VERIFY(0x025602CC, dKy_get_schbit_timer);

static inline s32 clamp255_l(s32 v) {
    if (v < 0) v = 0;
    if (v > 0xFF) v = 0xFF;
    return v;
}

/* 025602F0: dKy_get_seacolor: BG1 C0/K0 (u8 +0xB74/+0xB78) + BG1 addcols (+0xF78/+0xF80) */
void dKy_get_seacolor(u32 amb, u32 dif) {
    WWHD_FUNC(0x025602F0, void, amb, dif);
    u32 e = envlight_l();
    s32 r;
    {
        u32 e2 = envlight_l();
        r = (s16)(ld8(e + 0xB74) + lds16(e2 + 0xF78));
    }
    s32 g;
    {
        u32 e2 = envlight_l();
        g = (s16)(ld8(e + 0xB75) + lds16(e2 + 0xF7A));
    }
    s32 b;
    {
        u32 e2 = envlight_l();
        b = (s16)(ld8(e + 0xB76) + lds16(e2 + 0xF7C));
    }
    r = clamp255_l(r);
    g = clamp255_l(g);
    b = clamp255_l(b);
    st8(amb + 1, (u8)g);
    st8(amb + 2, (u8)b);
    st8(amb + 0, (u8)r);
    r;
    {
        u32 e2 = envlight_l();
        r = (s16)(ld8(e + 0xB78) + lds16(e2 + 0xF80));
    }
    g;
    {
        u32 e2 = envlight_l();
        g = (s16)(ld8(e + 0xB79) + lds16(e2 + 0xF82));
    }
    b;
    {
        u32 e2 = envlight_l();
        b = (s16)(ld8(e + 0xB7A) + lds16(e2 + 0xF84));
    }
    r = clamp255_l(r);
    g = clamp255_l(g);
    b = clamp255_l(b);
    st8(dif + 1, (u8)g);
    st8(dif + 2, (u8)b);
    st8(dif + 0, (u8)r);
}
VERIFY(0x025602F0, dKy_get_seacolor);

/* 02560444: mAllColGatherRatio +0xFFC */
void dKy_set_allcol_ratio(f32 ratio) {
    WWHD_FUNC(0x02560444, void, ratio);
    stf(envlight_l() + 0xFFC, ratio);
}
VERIFY(0x02560444, dKy_set_allcol_ratio);

/* 02560484: dKy_itudemo_se: mMoyaSE +0x1058 */
void dKy_itudemo_se() {
    WWHD_FUNC(0x02560484, void);
    u32 se = ld(envlight_l() + 0x1058);
    s32 roomNo = (s8)ld8(0x1047E6C8);
    if (se != 0) gabi::call(0x025E1988, se); /* mDoAud_seStart */
    if (stage_name_eq_l(0x1004F310) && roomNo == 3) gabi::call(0x025E1D18); /* "M_NewD2": mDoAud_bgmMuteMtDragon */
    if (stage_name_eq_l(0x1004F318) && roomNo == 0x12) gabi::call(0x025E1D24); /* "Siren": mDoAud_stSkyCloisters */
}
VERIFY(0x02560484, dKy_itudemo_se);

/* 025606D4: mbContrastFlag +0x109F */
void dKy_contrast_flg_set(u32 f) {
    WWHD_FUNC(0x025606D4, void, f);
    st8(envlight_l() + 0x109F, (u8)f);
}
VERIFY(0x025606D4, dKy_contrast_flg_set);

/* 02560704 */
u8 dKy_contrast_flg_get() {
    WWHD_FUNC(0x02560704, u8);
    return ld8(envlight_l() + 0x109F);
}
VERIFY(0x02560704, dKy_contrast_flg_get);

/* 02560728: mNextTime +0x1024 */
void dKy_set_nexttime(f32 t) {
    WWHD_FUNC(0x02560728, void, t);
    stf(envlight_l() + 0x1024, t);
}
VERIFY(0x02560728, dKy_set_nexttime);

/* 02560768: dKy_DayProc (tail call) */
void dKy_DayProc() {
    WWHD_FUNC(0x02560768, void);
    dKankyo_DayProc();
}
VERIFY(0x02560768, dKy_DayProc);

/* 0256076C */
void dKy_instant_timechg(f32 t) {
    WWHD_FUNC(0x0256076C, void, t);
    u32 sv = save_l();
    if (t < ldf(sv + 0x44)) {
        st16(sv + 0x48, (u16)(ld16(sv + 0x48) + 1));
        dKankyo_DayProc();
        sv = save_l();
    }
    stf(sv + 0x44, t);
}
VERIFY(0x0256076C, dKy_instant_timechg);

/* 025607E0: mColpatWeather +0x1092, mColpatPrev/Curr +0x108C/+0x108D */
void dKy_instant_rainchg() {
    WWHD_FUNC(0x025607E0, void);
    dKyw_rain_set_l(250);
    st8(envlight_l() + 0x1092, 1);
    st8(envlight_l() + 0x108C, 1);
    st8(envlight_l() + 0x108D, 1);
}
VERIFY(0x025607E0, dKy_instant_rainchg);

/* 02560828 */
s32 dKy_moon_type_chk() {
    WWHD_FUNC(0x02560828, s32);
    s32 weekday = dKy_get_dayofweek();
    if (ldf(save_l() + 0x44) < ldf(0x1004F10C)) {
        if (weekday != 0) weekday--;
        else weekday = 6;
    }
    return weekday;
}
VERIFY(0x02560828, dKy_moon_type_chk);

/* 02560878: dKy_telescope_lookin_chk: HD projects to a centred screen (centre 0,0,0) */
BOOL dKy_telescope_lookin_chk(u32 pos, f32 maxDist, f32 minFov) {
    WWHD_FUNC(0x02560878, BOOL, pos, maxDist, minFov);
    BOOL ret = FALSE;
    if (ld8(dComIfGp_get_l() + 0x5BB3) == 0) return ret;
    if (!(ldf(ld(dComIfGp_get_l() + 0x5FA4) + 0xD4) > minFov)) return ret;
    gabi::Local<cXyz> proj;
    gabi::call(0x025F0EA4, pos, proj.get()); /* mDoLib_project */
    gabi::Local<cXyz> center;
    u32 ca = gabi::ea(center.get());
    f32 zero = ldf(0x1004F0B0);
    stf(ca + 4, zero);
    stf(ca + 8, zero);
    stf(ca + 0, zero);
    if (sqrtf_l(sqdist_l(ca, gabi::ea(proj.get()))) < maxDist) ret = TRUE;
    return ret;
}
VERIFY(0x02560878, dKy_telescope_lookin_chk);

/* 02560944: mMoonPos +0xB44 */
BOOL dKy_moon_look_chk() {
    WWHD_FUNC(0x02560944, BOOL);
    BOOL rt = FALSE;
    if (gabi::call<BOOL>(0x02565DAC)) { /* dKyr_moon_arrival_check */
        u32 env = envlight_l();
        rt = dKy_telescope_lookin_chk(env + 0xB44, ldf(0x1004F240), ldf(0x1004F2F8));
    }
    return rt;
}
VERIFY(0x02560944, dKy_moon_look_chk);

/* constellation position from the camera eye (r3 = result storage, allocated when NULL) */
static inline void star_pos_l(u32 ret, u32 kx, u32 ky, u32 kz, bool zsub) {
    u32 cam = ld(dComIfGp_get_l() + 0x5AF8);
    f32 x = ldf(cam + 0xDC) + ldf(kx);
    f32 y = ldf(cam + 0xE0) + ldf(ky);
    f32 z = zsub ? ldf(cam + 0xE4) - ldf(kz) : ldf(cam + 0xE4) + ldf(kz);
    if (ret == 0) {
        ret = operator_new_l(0xC);
        if (ret == 0) return;
    }
    stf(ret + 4, y);
    stf(ret + 0, x);
    stf(ret + 8, z);
}

/* 0256099C */
void dKy_get_orion_pos(u32 ret) {
    WWHD_FUNC(0x0256099C, void, ret);
    star_pos_l(ret, 0x1004F320, 0x1004F324, 0x1004F328, false);
}
VERIFY(0x0256099C, dKy_get_orion_pos);

/* 02560A60 */
BOOL dKy_orion_look_chk() {
    WWHD_FUNC(0x02560A60, BOOL);
    gabi::Local<cXyz> p;
    dKy_get_orion_pos(gabi::ea(p.get()));
    gabi::Local<cXyz> q;
    u32 pa = gabi::ea(p.get()), qa = gabi::ea(q.get());
    st(qa + 0, ld(pa + 0));
    st(qa + 8, ld(pa + 8));
    st(qa + 4, ld(pa + 4));
    return dKy_telescope_lookin_chk(qa, ldf(0x1004F240), ldf(0x1004F2F8));
}
VERIFY(0x02560A60, dKy_orion_look_chk);

/* 02560AB4 */
void dKy_get_hokuto_pos(u32 ret) {
    WWHD_FUNC(0x02560AB4, void, ret);
    star_pos_l(ret, 0x1004F32C, 0x1004F330, 0x1004F334, true);
}
VERIFY(0x02560AB4, dKy_get_hokuto_pos);

/* 02560B78 */
BOOL dKy_hokuto_look_chk() {
    WWHD_FUNC(0x02560B78, BOOL);
    gabi::Local<cXyz> p;
    dKy_get_hokuto_pos(gabi::ea(p.get()));
    gabi::Local<cXyz> q;
    u32 pa = gabi::ea(p.get()), qa = gabi::ea(q.get());
    st(qa + 0, ld(pa + 0));
    st(qa + 8, ld(pa + 8));
    st(qa + 4, ld(pa + 4));
    return dKy_telescope_lookin_chk(qa, ldf(0x1004F240), ldf(0x1004F2F8));
}
VERIFY(0x02560B78, dKy_hokuto_look_chk);

/* 02560BCC: dKy_get_moon_pos (r3 = result storage, allocated when NULL) */
void dKy_get_moon_pos(u32 ret) {
    WWHD_FUNC(0x02560BCC, void, ret);
    u32 src = envlight_l() + 0xB44;
    if (ret == 0) {
        ret = operator_new_l(0xC);
        if (ret == 0) return;
    }
    stf(ret + 0, ldf(src + 0));
    stf(ret + 4, ldf(src + 4));
    stf(ret + 8, ldf(src + 8));
}
VERIFY(0x02560BCC, dKy_get_moon_pos);

/* 02560C34: dKy_pship_existense_set (+0x1096) */
void dKy_pship_existense_set() {
    WWHD_FUNC(0x02560C34, void);
    st8(envlight_l() + 0x1096, 1);
}
VERIFY(0x02560C34, dKy_pship_existense_set);

/* 02560C5C */
void dKy_pship_existense_cut() {
    WWHD_FUNC(0x02560C5C, void);
    st8(envlight_l() + 0x1096, 0);
}
VERIFY(0x02560C5C, dKy_pship_existense_cut);

/* 02560C84: mbDayNightTactStop +0x10A1 */
BOOL dKy_daynighttact_stop_chk() {
    WWHD_FUNC(0x02560C84, BOOL);
    BOOL rt = FALSE;
    if (dKy_checkEventNightStop()) rt = TRUE;
    else if (ld8(envlight_l() + 0x10A1) != 0) rt = TRUE;
    return rt;
}
VERIFY(0x02560C84, dKy_daynighttact_stop_chk);

/* dBgS_ObjGndChk_All on the stack (0x54 bytes; inline constructor/destructor of this TU) */
struct GndChkAll_l {
    u8 b[0x54];
};

/* 02560CD0: dKyr_player_overhead_bg_chk */
BOOL dKyr_player_overhead_bg_chk() {
    WWHD_FUNC(0x02560CD0, BOOL);
    u32 player = ld(dComIfGp_get_l() + 0x5B2C);
    BOOL ret = FALSE;
    gabi::Local<GndChkAll_l> gc;
    u32 L = gabi::ea(gc.get());
    gabi::call(0x02008E0C, L); /* cBgS_GndChk::cBgS_GndChk */
    st8(L + 0x48, 0);
    st8(L + 0x49, 0);
    st8(L + 0x4A, 0);
    st(L + 0x00, L + 0x40);
    st(L + 0x10, 0x1004F044);
    st(L + 0x20, 0x1004F054);
    st8(L + 0x47, 0);
    st(L + 0x4C, 0x1004F064);
    st(L + 0x04, L + 0x4C);
    st8(L + 0x46, 0);
    st8(L + 0x44, 1);
    st(L + 0x50, 0xF);
    st(L + 0x40, 0x1004F074);
    st8(L + 0x45, 0);
    if (gabi::call<s32>(0x0256019C) > 200) { /* dKy_rain_check */
        f32 x = ldf(player + 0x314);
        f32 k = ldf(0x1004F338);
        f32 y = ldf(player + 0x318), z = ldf(player + 0x31C);
        stf(L + 0x24, x);
        stf(L + 0x2C, z);
        stf(L + 0x28, y + k);
        u32 bgs = dComIfGp_get_l() + 0x12A0;
        f32 h = gabi::call<f32>(0x02008974, bgs, L); /* cBgS::GroundCross */
        if (h < ldf(player + 0x318) + ldf(0x1004F33C)) ret = TRUE;
    }
    st(L + 0x20, 0x1004EFD4);
    st(L + 0x40, 0x1004EFF4);
    st(L + 0x4C, 0x1004EFB4);
    gabi::call(0x02008DAC, L, 0); /* cBgS_Chk::~cBgS_Chk */
    return ret;
}
VERIFY(0x02560CD0, dKyr_player_overhead_bg_chk);

/* 02560E08: dScnKy_env_light_c::getDaytime() */
f32 dScnKy_env_light_c_getDaytime(u32 t) {
    WWHD_FUNC(0x02560E08, f32, t);
    return ldf(t + 0x1020);
}
VERIFY(0x02560E08, dScnKy_env_light_c_getDaytime);

void dKy_actor_addcol_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_vrbox_addcol_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_fog_startendz_set(f32 startZ, f32 endZ, f32 ratio);
void plight_init();
void plight_set();
void dKy_setLight_init();
void dKy_Sound_init();
static inline u32 stage_vcall_l(u32 off) {
    u32 sd = dComIfGp_get_l() + 0x5150;
    return gabi::call_ptr<u32>(ld(ld(sd) + off), sd);
}

/* 0255DAC0: dKy_Create (matcher: envcolor_init): envcolor_init inlined, then the GameCube create
 * body. HD: the start time is also forced per stage ("A_umikz" at least 240, "sea_E" 240, "sea"
 * room 44 without the Wind Waker 165 after 180, "ENDumi" 105), the HD light block (+0x10A8..
 * +0x10E8, +0x1030..+0x104C) is reset by day/night, an HD constant (101E8EFC) 2.3 ("Obshop"
 * 1.25), and the per-room light angle tables +0x1108/+0x1188 are loaded from the "Stage"
 * archive's "baseLight..." resource. Returns cPhs_COMPLEATE_e (4). */
s32 dKy_Create(u32 p) {
    WWHD_FUNC(0x0255DAC0, s32, p);
    const u32 env0 = envlight_l();
    u32 palette = stage_vcall_l(0x9C);
    u32 pselect = stage_vcall_l(0xAC);
    u32 envr = stage_vcall_l(0xBC);
    u32 vrbox = stage_vcall_l(0xCC);
    const f32 zero = ldf(0x1004F0B0);
    for (u32 i = 0; i < 20; i++) stf(0x1047B7C0 + i * 4, zero); /* REG3_F */
    for (u32 i = 0; i < 10; i++) st16(0x1047B838 + i * 2, 0);   /* REG3_S */
    gabi::call(0x025E1C20); /* mDoAud_initWindowPos */
    gabi::call(0x025E1B70); /* mDoAud_initSeaEnvPos */
    gabi::call(0x025E1BF8); /* mDoAud_initRiverPos */
    dKy_actor_addcol_set(0, 0, 0, zero);
    dKy_fog_startendz_set(zero, zero, zero);
    dKy_vrbox_addcol_set(0, 0, 0, zero);
    st(envlight_l() + 0x1050, 0);
    st8(envlight_l() + 0x109C, 1);
    st8(envlight_l() + 0x109D, 0);
    st(envlight_l() + 0x1084, 0);
    st8(envlight_l() + 0x109E, 0);
    st8(envlight_l() + 0x109F, 0);
    st8(envlight_l() + 0x1099, 1);
    st8(envlight_l() + 0x109A, 0);
    st16(envlight_l() + 0x1088, 0x140);
    gabi::call(0x025638D8, (u32)ld8(envlight_l() + 0x109A)); /* dKyd_xfog_table_set */
    const f32 one = ldf(0x1004F0A4);
    for (u32 o = 0xFE4; o <= 0x1010; o += 4) stf(envlight_l() + o, one);
    st8(envlight_l() + 0x109B, 0);
    stf(envlight_l() + 0x101C, zero);
    stf(envlight_l() + 0x1014, zero);
    stf(envlight_l() + 0x1018, zero);
    st8(envlight_l() + 0x10A0, 0);
    st8(envlight_l() + 0x10A1, 0);
    u32 info = stage_vcall_l(0x15C);
    u32 sched;
    if (((ld(info + 0xC) >> 16) & 7) == 3) sched = gabi::call<u32>(0x02563A10); /* dKyd_schejule_boss_getp */
    else sched = gabi::call<u32>(0x02563A04);                                    /* dKyd_schejule_getp */
    st(envlight_l() + 0x10, sched);
    st8(envlight_l() + 0x1090, 0);
    st8(envlight_l() + 0x1091, 0);
    stf(envlight_l() + 0xFC8, one);
    st(envlight_l() + 0x1080, 0);
    if (dKy_checkEventNightStop()) {
        st8(envlight_l() + 0x1092, 1);
        stf(envlight_l() + 0xA90, one);
    } else {
        st8(envlight_l() + 0x1092, 0);
        stf(envlight_l() + 0xA90, zero);
    }
    st(envlight_l() + 0x106C, 0);
    st(envlight_l() + 0x1070, 0);
    st8(envlight_l() + 0x1093, 0);
    {
        u32 env = envlight_l();
        stf(env + 0xFC0, ldf(save_l() + 0x44) + ldf(0x1004F2C0));
        env = envlight_l();
        if (!(ldf(env + 0xFC0) < ldf(0x1004F108))) {
            env = envlight_l();
            stf(env + 0xFC0, ldf(env + 0xFC0) - ldf(0x1004F108));
        }
    }
    st8(envlight_l() + 0x1094, 0);
    st8(envlight_l() + 0x1095, 0);
    st(envlight_l() + 0x1068, 0);
    stf(envlight_l() + 0xFC4, zero);
    st8(envlight_l() + 0x1096, 0);
    {
        u32 e1 = envlight_l();
        u8 w = ld8(envlight_l() + 0x1092);
        st8(e1 + 0x108C, w);
        e1 = envlight_l();
        w = ld8(envlight_l() + 0x1092);
        st8(e1 + 0x108D, w);
    }
    st8(envlight_l() + 0x108E, 0xFF);
    const f32 m1 = ldf(0x1004F0B4);
    st8(envlight_l() + 0x108F, 0xFF);
    stf(envlight_l() + 0xFCC, m1);
    st8(envlight_l() + 0x1097, 0);
    st8(envlight_l() + 0x1098, 0);
    if (envr == 0) envr = gabi::call<u32>(0x025639EC); /* dKyd_dmenvr_getp */
    st(envlight_l() + 8, envr);
    if (palette == 0) palette = gabi::call<u32>(0x025639D4); /* dKyd_dmpalet_getp */
    st(envlight_l() + 0, palette);
    if (pselect == 0) pselect = gabi::call<u32>(0x025639E0); /* dKyd_dmpselect_getp */
    st(envlight_l() + 4, pselect);
    if (vrbox == 0) vrbox = gabi::call<u32>(0x025639F8); /* dKyd_dmvrbox_getp */
    st(envlight_l() + 0xC, vrbox);
    plight_init();
    plight_set();
    if (ldf(envlight_l() + 0x1024) != m1) {
        f32 nt = ldf(envlight_l() + 0x1024);
        stf(save_l() + 0x44, nt);
        stf(envlight_l() + 0x1024, m1);
    }
    /* HD stage start times */
    bool umikz = stage_name_eq_l(0x1004EF40); /* "A_umikz" */
    if (!umikz) {
        u32 inf = stage_vcall_l(0x15C);
        s32 h = (s8)((ld(inf + 0xC) >> 8) & 0xFF); /* dStage_stagInfo_GetTimeH */
        if (h >= 0) {
            f32 t = ldf(0x1004F0FC) * i2f(h);
            stf(save_l() + 0x44, t);
        }
    }
    stf(envlight_l() + 0x1028, ldf(0x1004F2C4));
    const f32 k240 = ldf(0x1004F2C8);
    if (stage_name_eq_l(0x1004EF40)) {
        u32 sv = save_l();
        if (ldf(sv + 0x44) < k240) stf(sv + 0x44, k240);
    } else if (stage_name_eq_l(0x1004EF58)) { /* "sea_E" */
        stf(save_l() + 0x44, k240);
    }
    bool sea44 = false;
    if (stage_name_eq_l(0x1004EF50) && (s8)ld8(0x1047E6C8) == 0x2C) sea44 = true; /* "sea" */
    if (sea44 && !gabi::call<BOOL>(0x025B5D40, save_l() + 0x71, 2, 0)) {
        u32 sv = save_l();
        if (!(ldf(sv + 0x44) < ldf(0x1004F10C))) stf(sv + 0x44, ldf(0x1004F12C));
    }
    if (stage_name_eq_l(0x1004EF64)) stf(save_l() + 0x44, ldf(0x1004F2CC)); /* "ENDumi" */
    if (dKy_checkEventNightStop()) stf(save_l() + 0x44, zero);
    gabi::call(0xC000A858, envlight_l() + 0x1058, 0, 0x10); /* memset (import, via 028FEAD0) */
    stf(envlight_l() + 0x10A8, one);
    const f32 k3 = ldf(0x1004F2D0);
    stf(envlight_l() + 0x10AC, k3);
    stf(envlight_l() + 0x10B0, one);
    stf(envlight_l() + 0x10B4, k3);
    stf(envlight_l() + 0x10B8, k3);
    stf(envlight_l() + 0x10BC, k3);
    for (u32 o = 0x10C0; o <= 0x10CC; o += 4) stf(envlight_l() + o, one);
    for (u32 o = 0x10D4; o <= 0x10E4; o += 4) stf(envlight_l() + o, one);
    stf(envlight_l() + 0x1030, zero);
    stf(envlight_l() + 0x1034, zero);
    stf(envlight_l() + 0x1044, zero);
    stf(envlight_l() + 0x1048, zero);
    stf(envlight_l() + 0x1038, one);
    stf(envlight_l() + 0x103C, one);
    f32 t = ldf(save_l() + 0x44);
    u8 night = (t > ldf(0x1004F14C) && t < ldf(0x1004F150)) ? 0 : 1;
    st8(envlight_l() + 0x10A6, night);
    stf(envlight_l() + 0x10E8, zero);
    stf(envlight_l() + 0x1040, zero);
    st8(envlight_l() + 0x10A2, 0xFF);
    st8(envlight_l() + 0x10A3, 0xFF);
    st8(envlight_l() + 0x10A4, 0);
    stf(envlight_l() + 0x104C, one);
    stf(0x101E8EFC, ldf(0x1004F2D4));
    if (stage_name_eq_l(0x1004EF6C)) stf(0x101E8EFC, ldf(0x1004F2D8)); /* "Obshop" */
    st(envlight_l() + 0x1080, 0);
    stf(envlight_l() + 0x102C, one);
    stf(envlight_l() + 0xB2C, zero);
    stf(envlight_l() + 0xB30, zero);
    stf(envlight_l() + 0xB34, zero);
    dKy_setLight_init();
    gabi::call(0x0256EF7C); /* dKy_wave_chan_init */
    dKy_Sound_init();
    gabi::call(0x0257D398); /* dKyw_wind_set */
    /* HD room light record and angle tables */
    stf(env0 + 0x10EC, zero);
    stf(env0 + 0x10F0, zero);
    stf(env0 + 0x10F4, zero);
    stf(env0 + 0x10F8, zero);
    st8(env0 + 0x1104, 0);
    stf(env0 + 0x10FC, zero);
    st8(env0 + 0x1105, 0);
    st8(env0 + 0x1106, 0);
    stf(env0 + 0x1100, zero);
    st8(env0 + 0x1107, 0xFF);
    for (u32 i = 0; i < 0x40; i++) {
        st16(env0 + 0x1108 + i * 2, 0);
        st16(env0 + 0x1188 + i * 2, 0);
    }
    u32 res = gabi::call<u32>(0x0252447C, 0x1004F2DC, 0x1004F2E4); /* ("Stage", "baseLight...") */
    if (res != 0) {
        gabi::call(0xC000A848, env0 + 0x1108, res, 0x80); /* memcpy (import, via 028FEAC0) */
        gabi::call(0xC000A848, env0 + 0x1188, res + 0x80, 0x80);
    }
    return 4;
}
VERIFY(0x0255DAC0, dKy_Create);

s32 dKy_eflight_influence_id(u32 pos, s32 nth);
void dKy_eflight_influence_pos(u32 ret, s32 idx);
f32 dKy_eflight_influence_power(s32 idx);
f32 dKy_eflight_influence_yuragi(s32 idx);
f32 dKy_eflight_influence_distance(u32 pos, s32 idx);
static inline f32 cLib_addCalc_l(u32 p, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, p, target, scale, maxStep, minStep);
}
static inline f32 cLib_addCalc2_l(u32 p, f32 target, f32 scale, f32 maxStep) {
    return gabi::call<f32>(0x0200ED84, p, target, scale, maxStep);
}
void GXColorS10_to_f32(u32 dst, u32 src);

/* the light flicker target (static, base light 101E8EC8 / effect light 101E8ECC) */
static inline void flicker_l(u32 target, f32 v, f32 fluc, f32 k255, f32 k20, f32 one, f32 scale, bool div3) {
    if (v > one) v = one;
    v = (one - v) * k20;
    if (v > one) v = one;
    f32 f = div3 ? fluc / ldf(0x1004F2D0) : fluc;
    f32 wave = gabi::fnmsubs(f, v, k255);
    f32 range = k255 - wave;
    f32 r = gabi::call<f32>(0x020198D8, one); /* cM_rndF */
    cLib_addCalc2_l(target, gabi::fmadds(range, r, wave), scale, k20);
}

/* 0255E854: dKy_setLight: lightStatusData 10476C74 (0xE8 each; lightStatusPt 101E8CC8, lightMask
 * 101E8CDC, lightMaskData 101E8CE0). HD: the base light follows its target at once (or eased
 * while the player is in state 3), the point and effect lights are handed to the HD light
 * manager (0273862C), and the per-slot light parameter objects get the view-space positions,
 * directions and colours. */
void dKy_setLight() {
    WWHD_FUNC(0x0255E854, void);
    const u32 env = envlight_l();
    u32 cam = ld(dComIfGp_get_l() + 0x5AF8);
    u32 player = ld(dComIfGp_get_l() + 0x5B2C);
    if (cam == 0) return;
    gabi::Local<cXyz> fwd;
    gabi::call(0x02563F64, cam + 0xDC, cam + 0xE8, fwd.get()); /* dKyr_get_vectle_calc */
    const u32 ptA = 0x101E8CC8;
    {
        u32 x = ld(cam + 0xDC);
        u32 s = ld(ptA);
        st(s + 0xC, x);
        st(s + 0x10, ld(cam + 0xE0));
        st(s + 0x14, ld(cam + 0xE4));
    }
    f32 bx = ldf(env + 0x14), bz = ldf(env + 0x1C), by = ldf(env + 0x18);
    u32 p2 = ld(dComIfGp_get_l() + 0x5B2C);
    const f32 one = ldf(0x1004F0A4);
    f32 power;
    if (p2 != 0 && ld16(p2 + 0xF8) == 3) {
        cLib_addCalc_l(ld(ptA), bx, one, ldf(0x1004F2F4), one);
        cLib_addCalc_l(ld(ptA) + 4, by, one, ldf(0x1004F2F4), one);
        cLib_addCalc_l(ld(ptA) + 8, bz, one, ldf(0x1004F2F4), one);
    } else {
        u32 s = ld(ptA);
        stf(s + 0, bx);
        stf(s + 4, by);
        stf(s + 8, bz);
    }
    const f32 zero = ldf(0x1004F0B0);
    power = ldf(env + 0x28);
    f32 v, fluc;
    if (player != 0) {
        if (power > zero) {
            f32 d = sqrtf_l(sqdist_l(env + 0x14, player + 0x314));
            f32 pw = ldf(env + 0x28);
            fluc = ldf(env + 0x2C);
            v = d / pw;
        } else {
            fluc = ldf(env + 0x2C);
            v = zero;
        }
    } else {
        fluc = ldf(env + 0x2C);
        v = one;
    }
    const f32 k1000 = ldf(0x1004F29C), k20 = ldf(0x1004F2F8), k255 = ldf(0x1004F104);
    const u32 tgt = 0x101E8EC8;
    if (fluc < k1000) {
        flicker_l(tgt, v, fluc, k255, k20, one, ldf(0x1004F2BC), true);
    } else {
        f32 f = fluc - k1000;
        stf(tgt, f);
    }
    {
        u32 s = ld(ptA);
        st8(s + 0x18, (u8)gabi::ftoi(ldf(tgt)));
        stf(env + 0xB2C, ldf(ld(ptA) + 0));
        stf(env + 0xB30, ldf(ld(ptA) + 4));
        stf(env + 0xB34, ldf(ld(ptA) + 8));
    }
    const u32 lightMask = 0x101E8CDC;
    s32 ef = -1;
    if (player != 0) {
        gabi::Local<cXyz> pp;
        u32 pa = gabi::ea(pp.get());
        stf(pa + 0, ldf(player + 0x314));
        stf(pa + 4, ldf(player + 0x318));
        stf(pa + 8, ldf(player + 0x31C));
        ef = dKy_eflight_influence_id(pa, 0);
    }
    if (ef < 0) {
        st16(lightMask, 1);
    } else {
        st16(lightMask, 3);
        gabi::Local<cXyz> ep;
        dKy_eflight_influence_pos(gabi::ea(ep.get()), ef);
        u32 s = ld(ptA), epa = gabi::ea(ep.get());
        st(s + 0xE8, ld(epa + 0));
        st(s + 0xEC, ld(epa + 4));
        st(s + 0xF0, ld(epa + 8));
        f32 pw = dKy_eflight_influence_power(ef);
        f32 ev;
        f32 fl;
        if (pw > zero) {
            gabi::Local<cXyz> eye;
            u32 ea_ = gabi::ea(eye.get());
            stf(ea_ + 0, ldf(cam + 0xDC));
            stf(ea_ + 4, ldf(cam + 0xE0));
            stf(ea_ + 8, ldf(cam + 0xE4));
            f32 d = dKy_eflight_influence_distance(ea_, ef);
            ev = d / pw;
            fl = dKy_eflight_influence_yuragi(ef);
        } else {
            ev = one;
            fl = dKy_eflight_influence_yuragi(ef);
        }
        const u32 tgt2 = 0x101E8ECC;
        if (!(fl < ldf(0x1004F29C))) {
            f32 f = fl - ldf(0x1004F29C);
            u32 s2 = ld(ptA);
            stf(tgt2, f);
            st8(s2 + 0x101, (u8)gabi::ftoi(f));
        } else {
            flicker_l(tgt2, ev, fl, k255, k20, one, ldf(0x1004F1C8), false);
            u32 s2 = ld(ptA);
            st8(s2 + 0x101, (u8)gabi::ftoi(ldf(tgt2)));
        }
        st8(ld(ptA) + 0x100, 0);
        st8(ld(ptA) + 0x102, 0);
    }
    /* HD: point lights and effect lights to the HD light manager */
    const f32 minPower = ldf(0x1004F0BC);
    for (u32 i = 0; i < 200; i++) {
        if (ld(envlight_l() + 0x470 + i * 4) == 0) continue;
        if (!(ldf(ld(envlight_l() + 0x470 + i * 4) + 0x14) > minPower)) continue;
        u32 l = ld(envlight_l() + 0x470 + i * 4);
        f32 x = ldf(l + 0), y = ldf(l + 4), z = ldf(l + 8);
        f32 pw = ldf(ld(envlight_l() + 0x470 + i * 4) + 0x14);
        gabi::Local<be<f32>[4]> col;
        GXColorS10_to_f32(gabi::ea(col.get()), ld(envlight_l() + 0x470 + i * 4) + 0xC);
        u32 e2 = envlight_l();
        f32 kp = ldf(0x101E8EFC);
        u32 l2 = ld(e2 + 0x470 + i * 4);
        f32 kr = ldf(0x101E8EF4);
        pw = pw * kp;
        f32 rad = kr * ldf(l2 + 0x20);
        gabi::call(0x0274D2AC, col.get(), ldf(0x101E8EF8));
        gabi::Local<cXyz> pos;
        u32 pa = gabi::ea(pos.get());
        stf(pa + 0, x);
        stf(pa + 4, y);
        stf(pa + 8, z);
        gabi::call(0x0273862C, ld(0x101F8A20), pa, col.get(), 1, 0x104A01CC, 0, 0, 0, pw, rad, zero);
    }
    for (u32 i = 0; i < 10; i++) {
        if (ld(envlight_l() + 0x790 + i * 4) == 0) continue;
        if (!(ldf(ld(envlight_l() + 0x790 + i * 4) + 0x14) > minPower)) continue;
        u32 l = ld(envlight_l() + 0x790 + i * 4);
        f32 x = ldf(l + 0), z = ldf(l + 8), y = ldf(l + 4);
        f32 pw = ldf(ld(envlight_l() + 0x790 + i * 4) + 0x14);
        gabi::Local<be<f32>[4]> col;
        GXColorS10_to_f32(gabi::ea(col.get()), ld(envlight_l() + 0x790 + i * 4) + 0xC);
        f32 kr = ldf(0x101E8EF4);
        gabi::Local<cXyz> pos;
        u32 pa = gabi::ea(pos.get());
        stf(pa + 0, x);
        stf(pa + 4, y);
        stf(pa + 8, z);
        gabi::call(0x0273862C, ld(0x101F8A20), pa, col.get(), 1, 0x104A01CC, 0, 0, 0, pw, kr, zero);
    }
    /* light parameter objects per lit slot */
    const u32 j3d = 0x104B45C0;
    gabi::Local<be<f32>[12]> inv;
    gabi::call(0x025F20B0, j3d + 0x38, inv.get()); /* mDoMtx_inverseTranspose */
    const u32 tbl = ld(j3d + 0x148);
    for (u32 i = 0; i < 8; i++) {
        if ((ld16(lightMask) & ld16(0x101E8CE0 + i * 2)) == 0) continue;
        s32 idx = lds16(ld(0x104A1F70));
        u32 n = ld(tbl + 8);
        u32 arr = ld(tbl + 0xC);
        u32 cnt = (u32)idx < n ? ld16(arr + (u32)idx * 4 + 2) : ld16(arr + 2);
        u32 obj, g;
        if ((s32)i < (s32)cnt) {
            u32 e = (u32)idx < n ? arr + (u32)idx * 4 : arr;
            u32 k = ld16(e) + i;
            u32 n2 = ld(tbl + 0x10);
            u32 pp = 0;
            if (k < n2) pp = ld(tbl + 0x14) + k * 4;
            g = ld(0x101FD7D4);
            obj = ld(pp);
        } else {
            g = ld(0x101FD7D4);
            obj = 0;
        }
        if (g == 0) {
            st(0x101FD7D4, 1);
            st(0x101FDCCC, 0x1004EFA4);
        }
        /* r5 still holds the guard word here (the real checkDerivedRuntimeTypeInfo reads r3/r4 only) */
        if (obj != 0 && !gabi::call_ptr<BOOL>(ld(ld(obj + 0x58) + 0x44), obj, 0x101FDCCC, g)) obj = 0;
        u32 st_ = 0x10476C74 + i * 0xE8;
        gabi::Local<cXyz> tmp;
        u32 ta = gabi::ea(tmp.get());
        gabi::call(0x028E8F64, j3d + 0x38, st_, ta); /* PSMTXMultVec */
        f32 tx = ldf(ta + 0), ty = ldf(ta + 4), tz = ldf(ta + 8);
        stf(obj + 0x130, ty);
        stf(obj + 0x12C, tx);
        stf(obj + 0x134, tz);
        gabi::call(0x028E8F64, inv.get(), st_ + 0x38, i == 0 ? env + 0xB14 : ta);
        gabi::Local<be<f32>[4]> f;
        GXColor_to_f32(gabi::ea(f.get()), st_ + 0x18);
        u32 fa = gabi::ea(f.get());
        st(obj + 0xF4, ld(fa + 0));
        st(obj + 0xF8, ld(fa + 4));
        st(obj + 0xFC, ld(fa + 8));
        st(obj + 0x100, ld(fa + 0xC));
    }
}
VERIFY(0x0255E854, dKy_setLight);

/* 0255F160: dKy_setLight_again: slot 0 only, into the light parameter object */
void dKy_setLight_again() {
    WWHD_FUNC(0x0255F160, void);
    const u32 j3d = 0x104B45C0;
    gabi::Local<be<f32>[12]> inv;
    gabi::call(0x025F20B0, j3d + 0x38, inv.get());
    gabi::Local<cXyz> tmp;
    u32 ta = gabi::ea(tmp.get());
    gabi::call(0x028E8F64, j3d + 0x38, 0x10476C74, ta);
    u32 obj = hd_param_obj_l(0x104A1F70, 0x101FD7D4, 0x101FDCCC);
    gabi::Local<be<f32>[4]> f;
    GXColor_to_f32(gabi::ea(f.get()), 0x10476C74 + 0x18);
    u32 fa = gabi::ea(f.get());
    st(obj + 0xF4, ld(fa + 0));
    st(obj + 0xF8, ld(fa + 4));
    st(obj + 0xFC, ld(fa + 8));
    st(obj + 0x100, ld(fa + 0xC));
    f32 tz = ldf(ta + 8), tx = ldf(ta + 0), ty = ldf(ta + 4);
    stf(obj + 0x12C, tx);
    stf(obj + 0x134, tz);
    stf(obj + 0x130, ty);
    gabi::call(0x028E8F64, inv.get(), 0x10476C74 + 0x38, ta);
}
VERIFY(0x0255F160, dKy_setLight_again);

}  // namespace d_kankyo_cpp
