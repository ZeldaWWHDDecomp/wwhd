/* d_kankyo: environment / lighting core, part 6 of 6 (02560E10..025638D8), WWHD.
 * The tevstr setting: setLight_actor, the colget/plight/eflight helpers, setLight_bg,
 * settingTevStruct, setLightTevColorType, then __sinit and the per-TU inline copies.
 * See d_kankyo.cpp for the TU. HD dKy_tevstr_c (0x1C8): mColorC0 s16 +0x90, mColorK0 u8 +0x98,
 * fog colour s16 +0xA0, fog start/end +0xA8/+0xAC, mColpatBlend +0xB0, mInitTimer +0xB4,
 * mEnvrIdxCurr/Prev +0xB5/+0xB6, mColpatCurr/Prev +0xB7/+0xB8, mRoomNo +0xB9,
 * mEnvrIdxOverride +0xBA, mLightMode +0xBB, mInitType +0xBC. */
#include "d_kankyo_local.h"

namespace d_kankyo_cpp {

s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add, f32 blend01);
f32 float_kankyo_color_ratio_set_noratio(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7);

struct Pal4_l {
    u32 a, b, c, d;
};

/* 02560E10: dScnKy_env_light_c::setLight_actor(dKy_tevstr_c*, GXColorS10* fog, f32* fogStartZ,
 * f32* fogEndZ): the tevstr's own palette blend (as setLight, HD u8 palettes of 0x6C bytes);
 * actor C0 (s16) / K0 (u8) with the actor addcols, the HD K0 fade towards C0 (+0x1030), the fog
 * colour with alpha (+0x5C) and the fog distances (end pushed by +0x1040) */
void dScnKy_env_light_c_setLight_actor(u32 t, u32 tev, u32 fog, u32 fogStart, u32 fogEnd) {
    WWHD_FUNC(0x02560E10, void, t, tev, fog, fogStart, fogEnd);
    st8(tev + 0xB8, ld8(envlight_l() + 0x108C));
    u8 cur = ld8(envlight_l() + 0x108D);
    u8 prev = ld8(tev + 0xB8);
    st8(tev + 0xB7, cur);
    if (prev != cur) stf(tev + 0xB0, ldf(envlight_l() + 0xFC8));
    gabi::Local<be<f32>> blendAB;
    gabi::Local<be<u8>> pal0A, pal0B, pal1A, pal1B;
    gabi::Local<be<s32>> psel0, psel1;
    gabi::call(0x025580FC, t, tev + 0xB6, tev + 0xB5, tev + 0xB8, tev + 0xB7, pal0A.get(), pal0B.get(), pal1A.get(),
               pal1B.get(), blendAB.get(), psel0.get(), psel1.get(), tev + 0xB0, tev + 0xB4);
    Pal4_l p;
    if (ld8(envlight_l() + 0x10A2) == 0xFF) {
        p.a = ld(envlight_l() + 0) + (u8)*pal0A * 0x6C;
        p.b = ld(envlight_l() + 0) + (u8)*pal0B * 0x6C;
        p.c = ld(envlight_l() + 0) + (u8)*pal1A * 0x6C;
        p.d = ld(envlight_l() + 0) + (u8)*pal1B * 0x6C;
    } else {
        p.a = ld(envlight_l() + 0);
        p.b = ld(envlight_l() + 0);
        p.c = ld(envlight_l() + 0);
        p.d = ld(envlight_l() + 0);
    }
    const u32 bAB = gabi::ea(blendAB.get());
    for (u32 i = 0; i < 3; i++) { /* C0: mActColRatio^2, + mActorAddColAmb */
        u32 e1 = envlight_l();
        u32 e2 = envlight_l();
        f32 ratio = ldf(e1 + 0xFE8) * ldf(e2 + 0xFE8);
        s16 r = kankyo_color_ratio_set(ld8(p.a + i), ld8(p.b + i), ldf(bAB), ld8(p.c + i), ld8(p.d + i), ldf(tev + 0xB0),
                                       lds16(t + 0xF58 + i * 2), ratio);
        st16(tev + 0x90 + i * 2, (u16)r);
    }
    for (u32 i = 0; i < 3; i++) { /* K0: mActColRatio, + mActorAddColDif */
        u32 e = envlight_l();
        s16 r = kankyo_color_ratio_set(ld8(p.a + 3 + i), ld8(p.b + 3 + i), ldf(bAB), ld8(p.c + 3 + i), ld8(p.d + 3 + i),
                                       ldf(tev + 0xB0), lds16(t + 0xF60 + i * 2), ldf(e + 0xFE8));
        st8(tev + 0x98 + i, (u8)r);
    }
    const f32 zero = ldf(0x1004F0B0);
    if (ldf(envlight_l() + 0x1030) > zero) {
        f32 prevk = (f32)ld8(tev + 0x98);
        for (u32 i = 0; i < 3; i++) {
            u32 e = envlight_l();
            f32 c = i2f(lds16(tev + 0x90 + i * 2));
            f32 v = gabi::fmadds(c - prevk, ldf(e + 0x1030), prevk);
            if (i < 2) prevk = (f32)ld8(tev + 0x99 + i);
            st8(tev + 0x98 + i, (u8)gabi::ftoi(v));
        }
    }
    for (u32 i = 0; i < 3; i++) { /* fog: mFogColRatio, + mAddColFog */
        u32 e = envlight_l();
        s16 r = kankyo_color_ratio_set(ld8(p.a + 0x1E + i), ld8(p.b + 0x1E + i), ldf(bAB), ld8(p.c + 0x1E + i),
                                       ld8(p.d + 0x1E + i), ldf(tev + 0xB0), lds16(t + 0xFA8 + i * 2), ldf(e + 0xFF0));
        st16(fog + i * 2, (u16)r);
    }
    {
        const f32 k255 = ldf(0x1004F104);
        u32 e = envlight_l();
        u8 a = (u8)gabi::ftoi(ldf(p.a + 0x5C) * k255), b = (u8)gabi::ftoi(ldf(p.b + 0x5C) * k255),
           c = (u8)gabi::ftoi(ldf(p.c + 0x5C) * k255), d = (u8)gabi::ftoi(ldf(p.d + 0x5C) * k255);
        s16 r = kankyo_color_ratio_set(a, b, ldf(bAB), c, d, ldf(tev + 0xB0), 0, ldf(e + 0xFF0));
        st16(fog + 6, (u16)r);
    }
    {
        f32 sz = ldf(envlight_l() + 0xFD8);
        u32 e = envlight_l();
        f32 v = float_kankyo_color_ratio_set_noratio(ldf(p.a + 0x24), ldf(p.b + 0x24), ldf(bAB), ldf(p.c + 0x24), ldf(p.d + 0x24),
                                                     ldf(tev + 0xB0), sz, ldf(e + 0xFE0));
        stf(fogStart, v);
        u32 e1 = envlight_l();
        u32 e2 = envlight_l();
        f32 ez = float_kankyo_color_ratio_set_noratio(ldf(p.a + 0x28), ldf(p.b + 0x28), ldf(bAB), ldf(p.c + 0x28), ldf(p.d + 0x28),
                                                      ldf(tev + 0xB0), ldf(e1 + 0xFDC), ldf(e2 + 0xFE0));
        stf(fogEnd, ez);
        u32 e3 = envlight_l();
        ez = gabi::fmadds(ldf(0x1004F1C4), ldf(e3 + 0x1040), ez);
        stf(fogEnd, ez);
        if (ldf(fogStart) > ez) stf(fogStart, ez);
    }
}
VERIFY(0x02560E10, dScnKy_env_light_c_setLight_actor);

/* 02561384: dScnKy_env_light_c::settingTevStruct_colget_actor */
void dScnKy_env_light_c_settingTevStruct_colget_actor(u32 t, u32 pos, u32 tev, u32 c0, u32 k0, u32 fog, u32 fogStart,
                                                      u32 fogEnd) {
    WWHD_FUNC(0x02561384, void, t, pos, tev, c0, k0, fog, fogStart, fogEnd);
    u8 ov = ld8(tev + 0xBA);
    u8 prev = ld8(tev + 0xB6);
    u8 cur;
    if (ov != 0xFF) {
        cur = ov;
    } else {
        s32 r = (s8)ld8(tev + 0xB9);
        if (r < 0) r = 0;
        cur = (u8)r;
    }
    st8(tev + 0xB5, cur);
    if (prev != cur) {
        f32 b = ldf(tev + 0xB0);
        const f32 zero = ldf(0x1004F0B0);
        if (!(b < ldf(0x1004F0A4)) || !(b > zero)) stf(tev + 0xB0, zero);
    }
    dScnKy_env_light_c_setLight_actor(t, tev, fog, fogStart, fogEnd);
    st16(c0 + 0, ld16(tev + 0x90));
    st16(c0 + 2, ld16(tev + 0x92));
    st16(c0 + 4, ld16(tev + 0x94));
    st16(c0 + 6, ld16(tev + 0x96));
    st16(k0 + 0, ld8(tev + 0x98));
    st16(k0 + 2, ld8(tev + 0x99));
    st16(k0 + 4, ld8(tev + 0x9A));
}
VERIFY(0x02561384, dScnKy_env_light_c_settingTevStruct_colget_actor);

/* 0256146C: dScnKy_env_light_c::settingTevStruct_colget_player: HD: the room light override
 * (+0x10A3) also selects the player's environment */
void dScnKy_env_light_c_settingTevStruct_colget_player(u32 t, u32 tev) {
    WWHD_FUNC(0x0256146C, void, t, tev);
    u8 ov = ld8(tev + 0xBA);
    if (ov != 0xFF) {
        st8(tev + 0xB5, ov);
    } else {
        s32 r = (s8)ld8(tev + 0xB9);
        if (r >= 0) {
            st8(tev + 0xB5, (u8)r);
            if (ld8(envlight_l() + 0x10A3) != 0xFF) st8(tev + 0xB5, ld8(envlight_l() + 0x10A3));
        }
    }
    u32 env = envlight_l();
    u8 cur = ld8(tev + 0xB5);
    if (ld8(env + 0x1091) == cur) return;
    const f32 one = ldf(0x1004F0A4);
    if (cur == ld8(envlight_l() + 0x1090)) {
        u32 e1 = envlight_l();
        st8(e1 + 0x1090, ld8(envlight_l() + 0x1091));
        st8(envlight_l() + 0x1091, ld8(tev + 0xB5));
        e1 = envlight_l();
        f32 b = ldf(envlight_l() + 0xFC8);
        stf(e1 + 0xFC8, one - b);
        stf(tev + 0xB0, one - ldf(envlight_l() + 0xFC8));
    } else {
        const f32 zero = ldf(0x1004F0B0);
        bool reset = !(ldf(envlight_l() + 0xFC8) < one);
        if (!reset) reset = !(ldf(envlight_l() + 0xFC8) > zero);
        if (reset) {
            st8(envlight_l() + 0x1091, ld8(tev + 0xB5));
            u32 e = envlight_l();
            stf(e + 0xFC8, zero);
            stf(tev + 0xB0, zero);
        }
    }
}
VERIFY(0x0256146C, dScnKy_env_light_c_settingTevStruct_colget_player);

static inline f32 sqdist_l(u32 a, u32 b) { return gabi::call<f32>(0x028E8DE8, a, b); }
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
static inline f32 cLib_addCalc_l(u32 p, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, p, target, scale, maxStep, minStep);
}
BOOL toon_proc_check();
f32 dKy_eflight_influence_power(s32 idx);
f32 dKy_eflight_influence_distance(u32 pos, s32 idx);
static inline s16 scale16_l(s32 v, f32 k) { return (s16)gabi::ftoi(i2f(v) * k); }

/* tevstr light block: view-space position from +0x84, direction = mLightDir (+0xB14) */
static inline void tev_light_l(u32 tev, bool toon, u32 pos) {
    gabi::Local<cXyz> wp;
    u32 wa = gabi::ea(wp.get());
    if (!toon) {
        st(wa + 0, ld(tev + 0x84));
        st(wa + 4, ld(tev + 0x88));
        st(wa + 8, ld(tev + 0x8C));
    } else {
        /* toon: the light mirrored through the position */
        gabi::Local<cXyz> d;
        gabi::call(0x0201ADE0, tev + 0x84, d.get(), pos); /* cXyz::operator- */
        gabi::Local<cXyz> d2;
        u32 da = gabi::ea(d.get()), d2a = gabi::ea(d2.get());
        st(d2a + 0, ld(da + 0));
        st(d2a + 4, ld(da + 4));
        st(d2a + 8, ld(da + 8));
        gabi::call(0x0201ADE0, pos, d.get(), d2.get());
        st(wa + 0, ld(da + 0));
        st(wa + 4, ld(da + 4));
        st(wa + 8, ld(da + 8));
    }
    gabi::Local<cXyz> vp;
    u32 va = gabi::ea(vp.get());
    gabi::call(0x028E8F64, 0x104B45F8, wa, va); /* PSMTXMultVec(view) */
    st(tev + 0, ld(va + 0));
    st(tev + 4, ld(va + 4));
    st(tev + 8, ld(va + 8));
    u32 env = envlight_l();
    st(tev + 0xC, ld(env + 0xB14));
    st(tev + 0x10, ld(env + 0xB18));
    u32 dz = ld(env + 0xB1C);
    const f32 one = ldf(0x1004F0A4), zero = ldf(0x1004F0B0);
    st8(tev + 0x19, 0);
    st8(tev + 0x1A, 0);
    st8(tev + 0x1B, 0xFF);
    stf(tev + 0x2C, one);
    stf(tev + 0x30, zero);
    stf(tev + 0x34, zero);
    stf(tev + 0x38, one);
    stf(tev + 0x3C, zero);
    stf(tev + 0x40, zero);
    st(tev + 0x14, dz);
}

/* 025615B8: dScnKy_env_light_c::settingTevStruct_plightcol_plus(cXyz*, dKy_tevstr_c*, GXColorS10* c0,
 * GXColorS10* k0, u8 timer): HD: only the base light (no nearest point light): its brightness
 * flicker into the tevstr light colour (+0x18), the base colours (+0xF48 / +0xF50 work colours,
 * HD clamp 255) and the light position (eased for the player in state 3, toon mirrored) */
void dScnKy_env_light_c_settingTevStruct_plightcol_plus(u32 t, u32 pos, u32 tev, u32 c0, u32 k0, u32 timer) {
    WWHD_FUNC(0x025615B8, void, t, pos, tev, c0, k0, timer);
    const u32 env = envlight_l();
    gabi::Local<cXyz> bl;
    u32 ba = gabi::ea(bl.get());
    st(ba + 0, ld(env + 0x14));
    st(ba + 4, ld(env + 0x18));
    st(ba + 8, ld(env + 0x1C));
    f32 d = sqrtf_l(sqdist_l(env + 0x14, pos));
    f32 fl = ldf(env + 0x2C);
    const f32 zero = ldf(0x1004F0B0);
    f32 pw = ldf(env + 0x28);
    st16(t + 0xF50, 0);
    st16(t + 0xF52, 0);
    st16(t + 0xF54, 0);
    const f32 one = ldf(0x1004F0A4);
    f32 v = one;
    if (pw > zero && timer == 0) {
        v = d / pw;
        if (v > one) v = one;
    }
    f32 k = gabi::fnmsubs(v, v, one);
    s32 i;
    if (!(fl < ldf(0x1004F29C))) {
        i = gabi::ftoi(fl - ldf(0x1004F29C));
    } else {
        f32 k255 = ldf(0x1004F104);
        f32 w = gabi::fnmsubs(fl / ldf(0x1004F2D0), k, k255);
        f32 range = k255 - w;
        f32 r = gabi::call<f32>(0x020198D8, one); /* cM_rndF */
        i = gabi::ftoi(gabi::fmadds(range, r, w));
    }
    st8(tev + 0x18, (u8)i);
    {
        f32 kc = k * ldf(0x1004F218);
        s16 a = scale16_l(lds16(t + 0xF50), kc);
        s16 b = scale16_l(lds16(t + 0xF52), kc);
        s16 c = scale16_l(lds16(t + 0xF54), kc);
        s16 r0 = (s16)(lds16(c0 + 0) + a);
        st16(t + 0xF48, (u16)r0);
        s16 r1 = (s16)(lds16(c0 + 2) + b);
        st16(t + 0xF4A, (u16)r1);
        s16 r1r = lds16(t + 0xF4A);
        s16 r2 = (s16)(lds16(c0 + 4) + c);
        st16(t + 0xF4C, (u16)r2);
        if (r0 > 0xFF) st16(t + 0xF48, 0xFF);
        s16 r2r = lds16(t + 0xF4C);
        if (r1r > 0xFF) st16(t + 0xF4A, 0xFF);
        if (r2r > 0xFF) st16(t + 0xF4C, 0xFF);
    }
    {
        s16 a = scale16_l(lds16(t + 0xF50), k);
        s16 b = scale16_l(lds16(t + 0xF52), k);
        s16 c = scale16_l(lds16(t + 0xF54), k);
        s16 r0 = (s16)(lds16(k0 + 0) + a);
        st16(t + 0xF50, (u16)r0);
        s16 r1 = (s16)(lds16(k0 + 2) + b);
        st16(t + 0xF52, (u16)r1);
        s16 r2 = (s16)(lds16(k0 + 4) + c);
        st16(t + 0xF54, (u16)r2);
        if (r0 > 0xFF) st16(t + 0xF50, 0xFF);
        s16 r2r = lds16(t + 0xF54);
        if (r1 > 0xFF) st16(t + 0xF52, 0xFF);
        if (r2r > 0xFF) st16(t + 0xF54, 0xFF);
    }
    u32 player = ld(dComIfGp_get_l() + 0x5B2C);
    if (player != 0 && ld16(player + 0xF8) == 3 && timer == 0) {
        f32 d2 = sqrtf_l(sqdist_l(pos, tev + 0x84));
        f32 k1e4 = ldf(0x1004F338);
        f32 a = d2 / k1e4;
        if (a > one) a = one;
        a = a * a;
        f32 d3 = sqrtf_l(sqdist_l(pos, ba));
        f32 b = d3 / ldf(0x1004F180);
        if (b > one) b = one;
        b = one - b;
        f32 b2 = b * b;
        f32 base = gabi::fmadds(k1e4, a, ldf(0x1004F340));
        f32 b3 = b * b2;
        f32 step = gabi::fmadds(ldf(0x1004F240), b3, base) * ldf(0x1004F344);
        cLib_addCalc_l(tev + 0x84, ldf(ba + 0), one, step, one);
        cLib_addCalc_l(tev + 0x88, ldf(ba + 4), one, step, one);
        cLib_addCalc_l(tev + 0x8C, ldf(ba + 8), one, step, one);
    } else {
        st(tev + 0x84, ld(ba + 0));
        st(tev + 0x88, ld(ba + 4));
        st(tev + 0x8C, ld(ba + 8));
    }
    tev_light_l(tev, toon_proc_check(), pos);
}
VERIFY(0x025615B8, dScnKy_env_light_c_settingTevStruct_plightcol_plus);

/* 02561BA4: dScnKy_env_light_c::settingTevStruct_eflightcol_plus: mPlayerEflightIdx +0x1078;
 * K1 (+0x9C) and its alpha (+0x9F) */
void dScnKy_env_light_c_settingTevStruct_eflightcol_plus(u32 t, u32 pos, u32 tev) {
    WWHD_FUNC(0x02561BA4, void, t, pos, tev);
    if ((s32)ld(t + 0x1078) < 0) return;
    u32 env = envlight_l();
    s32 efi = (s32)ld(t + 0x1078);
    if (ld(env + 0x790 + efi * 4) == 0) return;
    f32 pw = dKy_eflight_influence_power(efi);
    if (!(pw > ldf(0x1004F0B0))) return;
    gabi::Local<cXyz> lp;
    u32 la = gabi::ea(lp.get());
    stf(la + 8, ldf(pos + 8));
    stf(la + 4, ldf(pos + 4));
    stf(la + 0, ldf(pos + 0));
    f32 b = dKy_eflight_influence_distance(la, efi) / pw;
    const f32 one = ldf(0x1004F0A4);
    if (!(b < one)) return;
    st8(tev + 0x9F, 1);
    if (efi < 0) efi = 0;
    u32 l = ld(envlight_l() + 0x790 + efi * 4);
    f32 k = one - b;
    s32 r = scale16_l(lds16(l + 0xC), k);
    s32 g = scale16_l(lds16(l + 0xE), k);
    s32 bb = scale16_l(lds16(l + 0x10), k);
    if (r > 0xFF) r = 0xFF;
    if (g > 0xFF) g = 0xFF;
    if (bb > 0xFF) bb = 0xFF;
    u8 rr = (u8)gabi::ftoi(i2f(r) * k), gg = (u8)gabi::ftoi(i2f(g) * k), b2 = (u8)gabi::ftoi(i2f(bb) * k);
    st8(tev + 0x9C, rr);
    st8(tev + 0x9D, gg);
    st8(tev + 0x9E, b2);
}
VERIFY(0x02561BA4, dScnKy_env_light_c_settingTevStruct_eflightcol_plus);

/* 02561E14: dScnKy_env_light_c::setLight_bg(dKy_tevstr_c*, GXColorS10* BG0_C0, BG0_K0, BG1_C0,
 * BG1_K0, BG2_C0, BG2_K0, [stack] BG3_C0, BG3_K0, fog, f32* fogStartZ, f32* fogEndZ): as
 * setLight_actor for the four BG colour sets (+ the BG addcols +0xF68..), alpha 255, the HD BG0
 * K0 fade towards C0 (+0x1030), fog colour with alpha, fog distances */
void dScnKy_env_light_c_setLight_bg(u32 t, u32 tev, u32 bg0c0, u32 bg0k0, u32 bg1c0, u32 bg1k0, u32 bg2c0, u32 bg2k0) {
    WWHD_FUNC(0x02561E14, void, t, tev, bg0c0, bg0k0, bg1c0, bg1k0, bg2c0, bg2k0);
    const u32 sp = gabi::cpu->r[1];
    const u32 bg3c0 = ld(sp + 8), bg3k0 = ld(sp + 0xC), fog = ld(sp + 0x10), fogStart = ld(sp + 0x14), fogEnd = ld(sp + 0x18);
    st8(tev + 0xB8, ld8(envlight_l() + 0x108C));
    u8 cur = ld8(envlight_l() + 0x108D);
    u8 prev = ld8(tev + 0xB8);
    st8(tev + 0xB7, cur);
    if (prev != cur) stf(tev + 0xB0, ldf(envlight_l() + 0xFC8));
    gabi::Local<be<f32>> blendAB;
    gabi::Local<be<u8>> pal0A, pal0B, pal1A, pal1B;
    gabi::Local<be<s32>> psel0, psel1;
    gabi::call(0x025580FC, t, tev + 0xB6, tev + 0xB5, tev + 0xB8, tev + 0xB7, pal0A.get(), pal0B.get(), pal1A.get(),
               pal1B.get(), blendAB.get(), psel0.get(), psel1.get(), tev + 0xB0, tev + 0xB4);
    Pal4_l p;
    if (ld8(envlight_l() + 0x10A2) == 0xFF) {
        p.a = ld(envlight_l() + 0) + (u8)*pal0A * 0x6C;
        p.b = ld(envlight_l() + 0) + (u8)*pal0B * 0x6C;
        p.c = ld(envlight_l() + 0) + (u8)*pal1A * 0x6C;
        p.d = ld(envlight_l() + 0) + (u8)*pal1B * 0x6C;
    } else {
        p.a = ld(envlight_l() + 0);
        p.b = ld(envlight_l() + 0);
        p.c = ld(envlight_l() + 0);
        p.d = ld(envlight_l() + 0);
    }
    const u32 bAB = gabi::ea(blendAB.get());
    const u32 outs[8] = {bg0c0, bg0k0, bg1c0, bg1k0, bg2c0, bg2k0, bg3c0, bg3k0};
    for (u32 g = 0; g < 8; g++) {
        for (u32 i = 0; i < 3; i++) {
            u32 off = 6 + g * 3 + i;
            u32 e = envlight_l();
            s16 r = kankyo_color_ratio_set(ld8(p.a + off), ld8(p.b + off), ldf(bAB), ld8(p.c + off), ld8(p.d + off), ldf(tev + 0xB0),
                                           lds16(t + 0xF68 + g * 8 + i * 2), ldf(e + 0xFEC));
            st16(outs[g] + i * 2, (u16)r);
        }
    }
    for (s32 g = 7; g >= 0; g--) st16(outs[g] + 6, 0xFF);
    const f32 zero = ldf(0x1004F0B0);
    if (ldf(envlight_l() + 0x1030) > zero) {
        f32 prevk = i2f(lds16(bg0k0 + 0));
        for (u32 i = 0; i < 3; i++) {
            u32 e = envlight_l();
            f32 c = i2f(lds16(bg0c0 + i * 2));
            f32 v = gabi::fmadds(c - prevk, ldf(e + 0x1030), prevk);
            if (i < 2) prevk = i2f(lds16(bg0k0 + (i + 1) * 2));
            st16(bg0k0 + i * 2, (u8)gabi::ftoi(v));
        }
    }
    for (u32 i = 0; i < 3; i++) {
        u32 e = envlight_l();
        s16 r = kankyo_color_ratio_set(ld8(p.a + 0x1E + i), ld8(p.b + 0x1E + i), ldf(bAB), ld8(p.c + 0x1E + i),
                                       ld8(p.d + 0x1E + i), ldf(tev + 0xB0), lds16(t + 0xFA8 + i * 2), ldf(e + 0xFF0));
        st16(fog + i * 2, (u16)r);
    }
    {
        const f32 k255 = ldf(0x1004F104);
        u32 e = envlight_l();
        u8 a = (u8)gabi::ftoi(ldf(p.a + 0x5C) * k255), b = (u8)gabi::ftoi(ldf(p.b + 0x5C) * k255),
           c = (u8)gabi::ftoi(ldf(p.c + 0x5C) * k255), d = (u8)gabi::ftoi(ldf(p.d + 0x5C) * k255);
        s16 r = kankyo_color_ratio_set(a, b, ldf(bAB), c, d, ldf(tev + 0xB0), 0, ldf(e + 0xFF0));
        st16(fog + 6, (u16)r);
    }
    {
        f32 sz = ldf(envlight_l() + 0xFD8);
        u32 e = envlight_l();
        f32 v = float_kankyo_color_ratio_set_noratio(ldf(p.a + 0x24), ldf(p.b + 0x24), ldf(bAB), ldf(p.c + 0x24), ldf(p.d + 0x24),
                                                     ldf(tev + 0xB0), sz, ldf(e + 0xFE0));
        stf(fogStart, v);
        u32 e1 = envlight_l();
        u32 e2 = envlight_l();
        f32 ez = float_kankyo_color_ratio_set_noratio(ldf(p.a + 0x28), ldf(p.b + 0x28), ldf(bAB), ldf(p.c + 0x28), ldf(p.d + 0x28),
                                                      ldf(tev + 0xB0), ldf(e1 + 0xFDC), ldf(e2 + 0xFE0));
        stf(fogEnd, ez);
        u32 e3 = envlight_l();
        ez = gabi::fmadds(ldf(0x1004F1C4), ldf(e3 + 0x1040), ez);
        stf(fogEnd, ez);
        if (ldf(fogStart) > ez) stf(fogStart, ez);
    }
}
VERIFY(0x02561E14, dScnKy_env_light_c_setLight_bg);

void dKy_tevstr_init(u32 tev, s32 roomNo, u32 p2);

struct S10_l {
    be<s16> c[4];
};

/* the work colours (+0xF48 C0, +0xF50 K0) into the tevstr colours */
static inline void tev_colors_l(u32 t, u32 tev) {
    st16(t + 0xF4E, 0xFF);
    st16(tev + 0x90, ld16(t + 0xF48));
    st16(tev + 0x92, ld16(t + 0xF4A));
    st16(tev + 0x94, ld16(t + 0xF4C));
    st16(tev + 0x96, 0xFF);
    st8(tev + 0x98, (u8)ld16(t + 0xF50));
    st8(tev + 0x99, (u8)ld16(t + 0xF52));
    st8(tev + 0x9A, (u8)ld16(t + 0xF54));
}

/* 025626A4: dScnKy_env_light_c::settingTevStruct(int type, cXyz* pos, dKy_tevstr_c*): HD: the
 * colours go through the work colours +0xF48/+0xF50, every type gets the HD light parameters
 * (+0x24/+0x28 from +0x10A8..+0x10CC by type, the K1-like +0x160 colour from +0xBA4 faded by
 * +0x1030, +0x16C from +0x10D0), BG types use the base light status and only the point light
 * types (91..93) the base light colour; BG1 (sea) takes the vrbox uso umi fog colour. */
void dScnKy_env_light_c_settingTevStruct(u32 t, s32 type, u32 pos, u32 tev) {
    WWHD_FUNC(0x025626A4, void, t, type, pos, tev);
    dComIfGp_get_l();
    s32 bgIdx = -1;
    u32 timer = ld8(tev + 0xB4);
    bool full = false;
    if (ld8(tev + 0xBC) != 0x7B && ld8(tev + 0xBC) != 0x7C) dKy_tevstr_init(tev, (s8)ld8(0x1047E6C8), 0xFF);
    st8(tev + 0xBC, 0x7C);
    st8(tev + 0x9F, 0);
    st8(t + 0xB6B, 0xFF);
    st8(t + 0xB67, 0xFF);
    gabi::Local<S10_l> fog;
    gabi::Local<be<f32>> fogStart, fogEnd;
    const u32 fa = gabi::ea(fog.get());
    if (type == 0 || type == 9 || type == 99) {
        st8(tev + 0xBB, 1);
        gabi::Local<S10_l> c0, k0;
        const u32 ca = gabi::ea(c0.get()), ka = gabi::ea(k0.get());
        st16(ca + 0, ld8(t + 0xB64));
        st16(ca + 2, ld8(t + 0xB65));
        st16(ca + 4, ld8(t + 0xB66));
        st16(ka + 0, ld8(t + 0xB68));
        st16(ka + 2, ld8(t + 0xB69));
        st16(ka + 4, ld8(t + 0xB6A));
        st16(fa + 0, ld8(t + 0xB8C));
        st16(fa + 2, ld8(t + 0xB8D));
        st16(fa + 4, ld8(t + 0xB8E));
        st16(fa + 6, ld8(t + 0xB8F));
        stf(gabi::ea(fogStart.get()), ldf(t + 0xFD0));
        stf(gabi::ea(fogEnd.get()), ldf(t + 0xFD4));
        if (type == 9) {
            timer = ld8(envlight_l() + 0x109C);
            dScnKy_env_light_c_settingTevStruct_colget_player(t, tev);
        }
        dScnKy_env_light_c_settingTevStruct_colget_actor(t, pos, tev, ca, ka, fa, gabi::ea(fogStart.get()), gabi::ea(fogEnd.get()));
        st16(t + 0xF48, ld16(ca + 0));
        st16(t + 0xF4A, ld16(ca + 2));
        st16(t + 0xF4C, ld16(ca + 4));
        st16(t + 0xF4E, 0xFF);
        st16(t + 0xF50, ld16(ka + 0));
        st16(t + 0xF52, ld16(ka + 2));
        st16(t + 0xF54, ld16(ka + 4));
        st16(t + 0xF56, 0xFF);
        if (type != 99) {
            gabi::Local<S10_l> c0c, k0c;
            u32 cca = gabi::ea(c0c.get()), kca = gabi::ea(k0c.get());
            st(kca + 0, ld(ka + 0));
            st(cca + 4, ld(ca + 4));
            st(kca + 4, ld(ka + 4));
            st(cca + 0, ld(ca + 0));
            dScnKy_env_light_c_settingTevStruct_plightcol_plus(t, pos, tev, cca, kca, timer);
            dScnKy_env_light_c_settingTevStruct_eflightcol_plus(t, pos, tev);
            if (type == 9) {
                u32 env = envlight_l();
                st(env + 0xB2C, ld(tev + 0x84));
                st(env + 0xB30, ld(tev + 0x88));
                st(env + 0xB34, ld(tev + 0x8C));
            }
        }
    } else {
        st8(tev + 0xBB, 0);
        st8(tev + 0xB5, ld8(tev + 0xB9));
        gabi::Local<S10_l> bg[8]; /* C0/K0 of BG0..BG3 */
        u32 c0[4], k0[4];
        for (u32 i = 0; i < 4; i++) {
            c0[i] = gabi::ea(bg[i * 2].get());
            k0[i] = gabi::ea(bg[i * 2 + 1].get());
        }
        gabi::call(0x02561E14, t, tev, c0[0], k0[0], c0[1], k0[1], c0[2], k0[2], c0[3], k0[3], fa, gabi::ea(fogStart.get()),
                   gabi::ea(fogEnd.get())); /* setLight_bg */
        if ((u32)(type - 5) < 4) {
            type -= 4;
            full = true;
        }
        u32 pl = (u32)(type - 0x5B);
        bgIdx = type - (pl < 4 ? 0x5B : 1);
        u32 g = (u32)bgIdx < 3 ? (u32)bgIdx : 3;
        st16(t + 0xF48, ld16(c0[g] + 0));
        st16(t + 0xF4A, ld16(c0[g] + 2));
        st16(t + 0xF4C, ld16(c0[g] + 4));
        st16(t + 0xF4E, ld16(c0[g] + 6));
        s16 kr = lds16(k0[g] + 0), kg = lds16(k0[g] + 2), kb = lds16(k0[g] + 4);
        st16(t + 0xF50, (u16)kr);
        st16(t + 0xF52, (u16)kg);
        st16(t + 0xF54, (u16)kb);
        st16(t + 0xF56, ld16(k0[g] + 6));
        if (pl < 3) {
            gabi::Local<S10_l> c0c, k0c;
            u32 cca = gabi::ea(c0c.get()), kca = gabi::ea(k0c.get());
            st16(cca + 0, ld8(t + 0xF49));
            st16(cca + 2, ld8(t + 0xF4B));
            st16(cca + 4, ld8(t + 0xF4D));
            st16(kca + 0, (u8)kr);
            st16(kca + 2, (u8)kg);
            st16(kca + 4, (u8)kb);
            gabi::Local<S10_l> c0d, k0d;
            u32 cda = gabi::ea(c0d.get()), kda = gabi::ea(k0d.get());
            st(cda + 0, ld(cca + 0));
            st(cda + 4, ld(cca + 4));
            st8(tev + 0xBB, 2);
            st(kda + 0, ld(kca + 0));
            st(kda + 4, ld(kca + 4));
            dScnKy_env_light_c_settingTevStruct_plightcol_plus(t, pos, tev, cda, kda, timer);
        } else {
            gabi::Local<cXyz> vp;
            u32 va = gabi::ea(vp.get());
            gabi::call(0x028E8F64, 0x104B45F8, 0x10476C74, va); /* PSMTXMultVec(view, lightStatusData[0].mPos) */
            st(tev + 0, ld(va + 0));
            st(tev + 4, ld(va + 4));
            st(tev + 8, ld(va + 8));
            stf(tev + 0x84, ldf(0x10476C74));
            stf(tev + 0x88, ldf(0x10476C78));
            stf(tev + 0x8C, ldf(0x10476C7C));
            u32 env = envlight_l();
            st(tev + 0xC, ld(env + 0xB14));
            st(tev + 0x10, ld(env + 0xB18));
            u32 dz = ld(env + 0xB1C);
            st8(tev + 0x18, 0xFF);
            st(tev + 0x14, dz);
            const f32 one = ldf(0x1004F0A4), zero = ldf(0x1004F0B0);
            u8 c = full ? 0xFF : 0;
            st8(tev + 0x19, c);
            st8(tev + 0x1A, c);
            st8(tev + 0x1B, 0xFF);
            stf(tev + 0x2C, one);
            stf(tev + 0x30, zero);
            stf(tev + 0x34, zero);
            stf(tev + 0x38, one);
            stf(tev + 0x3C, zero);
            stf(tev + 0x40, zero);
        }
    }
    tev_colors_l(t, tev);
    if (bgIdx == 1) {
        st16(tev + 0xA0, ld8(envlight_l() + 0xB94));
        st16(tev + 0xA2, ld8(envlight_l() + 0xB95));
        st16(tev + 0xA4, ld8(envlight_l() + 0xB96));
        u8 a = ld8(envlight_l() + 0xB97);
        stf(tev + 0xA8, ldf(gabi::ea(fogStart.get())));
        st16(tev + 0xA6, a);
        stf(tev + 0xAC, ldf(gabi::ea(fogEnd.get())));
    } else {
        u16 f0 = ld16(fa + 0), f3 = ld16(fa + 6);
        st16(tev + 0xA0, f0);
        st16(tev + 0xA2, ld16(fa + 2));
        st16(tev + 0xA4, ld16(fa + 4));
        stf(tev + 0xA8, ldf(gabi::ea(fogStart.get())));
        stf(tev + 0xAC, ldf(gabi::ea(fogEnd.get())));
        st16(tev + 0xA6, f3);
    }
    u32 hp = 0;
    switch ((u32)type) {
    case 0: case 9: case 99: hp = 0x10A8; break;
    case 1: hp = 0x10B0; break;
    case 2: hp = 0x10B8; break;
    case 3: hp = 0x10C0; break;
    case 4: hp = 0x10C8; break;
    default: break;
    }
    if (hp != 0) {
        stf(tev + 0x24, ldf(t + hp + 4));
        stf(tev + 0x28, ldf(t + hp));
    }
    for (u32 i = 0; i < 3; i++) {
        f32 base = (f32)ld8(envlight_l() + 0xBA4 + i);
        s32 c = lds16(tev + 0x90 + i * 2);
        f32 d = i2f(c - (s32)ld8(envlight_l() + 0xBA4 + i));
        f32 v = gabi::fmadds(d, ldf(envlight_l() + 0x1030), base);
        st16(tev + 0x160 + i * 2, (u8)gabi::ftoi(v));
    }
    stf(tev + 0x16C, ldf(envlight_l() + 0x10D0));
}
VERIFY(0x025626A4, dScnKy_env_light_c_settingTevStruct);

void copy_f32x12(u32 dst, u32 src);
void GXColorS10_to_f32(u32 dst, u32 src);
void GXColor_to_f32(u32 dst, u32 src);

/* J3D HD material: +0x10 colour block (vtable at +0), +0x18 tev block (vtable at +4), +0x20 fog
 * block (vtable at +0), +0xA0 dirty flags (0x10 tev colour 0, 0x80 k colour 0), parameter slots
 * from 027F9F0C(material+0xA0, slot) */
static inline u32 vt0_l(u32 obj, u32 off) { return ld(ld(obj) + off); }
static inline u32 vt4_l(u32 obj, u32 off) { return ld(ld(obj + 4) + off); }
/* 02562F5C: dScnKy_env_light_c::setLightTevColorType(J3DModel*, dKy_tevstr_c*) with
 * setLightTevColorType_sub inlined per material (0x39C each): HD sets the light block (+0xC0,
 * second light +0x144) and also writes the tev/k colours as float parameters (with the HD
 * scale of +0x24) into the material's HD parameter block; the K1 path is gone. */
void dScnKy_env_light_c_setLightTevColorType(u32 t, u32 model, u32 tev) {
    WWHD_FUNC(0x02562F5C, void, t, model, tev);
    /* stack objects at the original's sp offsets (frame 0x158), as in the disassembly */
    if (ld8(tev + 0xBC) != 0x7C) {
        if (ld8(tev + 0xBC) != 0x7B) dKy_tevstr_init(tev, (s8)ld8(0x1047E6C8), 0xFF);
        dScnKy_env_light_c_settingTevStruct(t, 99, 0, tev);
    }
    u32 n = ld16(gabi::call<u32>(0x027F3F8C, ld(model + 0xAC)) + 0x24);
    s32 idx = (s32)n - 1;
    if (idx < 0) return;
    const f32 one = ldf(0x1004F0A4), k255 = ldf(0x1004F104);
    const u32 j3d = 0x104B45C0;
    for (u32 cnt = n; cnt != 0; cnt--, idx--) {
        u32 md = ld(model + 0xAC);
        u32 i16 = (u32)idx & 0xFFFF;
        u32 mat = ld(md + 0x10);
        if (i16 < ld(md + 0xC)) mat += i16 * 0x39C;
        envlight_l();
        if (ld8(tev + 0xBB) != 0) {
            u32 tb = ld(mat + 0x18);
            gabi::call_ptr(vt4_l(tb, 0x34), tb, 3);
        }
        gabi::FrameLocal<be<f32>[12]> view(0xAC);
        copy_f32x12(gabi::ea(view.get()), j3d + 0x38);
        gabi::FrameLocal<cXyz> lp(0x14);
        u32 la = gabi::ea(lp.get());
        stf(la + 0, ldf(tev + 0x84));
        stf(la + 4, ldf(tev + 0x88));
        stf(la + 8, ldf(tev + 0x8C));
        gabi::call(0x028E8F64, view.get(), la, la); /* PSMTXMultVec */
        gabi::FrameLocal<cXyz> vp(0x20);
        u32 va = gabi::ea(vp.get());
        gabi::call(0x028E8F64, j3d + 0x38, tev + 0x84, va);
        st(tev + 0, ld(va + 0));
        st(tev + 4, ld(va + 4));
        st(tev + 8, ld(va + 8));
        st(tev + 0xC0, ld(va + 0));
        st(tev + 0xC4, ld(va + 4));
        st(tev + 0xC8, ld(va + 8));
        st(tev + 0xD8, ld(tev + 0x18));
        {
            u32 cb = ld(mat + 0x10);
            gabi::call_ptr(vt0_l(cb, 0x3C), cb, 0, tev + 0xC0); /* setLight(0, light) */
        }
        bool hasFog;
        if (toon_proc_check() && ld8(tev + 0xBB) != 0) {
            u32 tb = ld(mat + 0x18);
            u32 kc = gabi::call_ptr<u32>(vt4_l(tb, 0x4C), tb, 0); /* getTevKColor(0) */
            if (kc != 0) {
                gabi::FrameLocal<be<u8>[4]> k(0x10);
                u32 ka = gabi::ea(k.get());
                u8 g = (u8)lds16(tev + 0x92), r = (u8)lds16(tev + 0x90), a = ld8(kc + 3);
                st8(ka + 0, r);
                u8 b = (u8)lds16(tev + 0x94);
                st8(ka + 1, g);
                st8(ka + 2, b);
                st8(ka + 3, a);
                tb = ld(mat + 0x18);
                gabi::call_ptr(vt4_l(tb, 0x3C), tb, 0, ka); /* setTevKColor(0) */
                gabi::FrameLocal<be<f32>[4]> f(0x7C);
                GXColor_to_f32(gabi::ea(f.get()), ka);
                gabi::FrameLocal<be<f32>[4]> sc(0x3C);
                u32 sa = gabi::ea(sc.get());
                gabi::call(0x0274D458, sa, f.get(), one);
                st(mat + 0xA0, ld(mat + 0xA0) | 0x80);
                u32 prm = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 7);
                f32 al = (f32)ld8(ka + 3);
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                stf(prm + 4, y);
                stf(prm + 8, z);
                stf(prm + 0, x);
                stf(prm + 0xC, al / k255);
            }
            tb = ld(mat + 0x18);
            u32 c = gabi::call_ptr<u32>(vt4_l(tb, 0x34), tb, 0); /* getTevColor(0) */
            if (c != 0) {
                gabi::FrameLocal<S10_l> cl(0x14);
                u32 cla = gabi::ea(cl.get());
                u8 g = ld8(tev + 0x99), r = ld8(tev + 0x98);
                s16 a = lds16(c + 6);
                st16(cla + 0, r);
                u8 b = ld8(tev + 0x9A);
                st16(cla + 2, g);
                st16(cla + 4, b);
                st16(cla + 6, (u16)a);
                tb = ld(mat + 0x18);
                gabi::call_ptr(vt4_l(tb, 0x24), tb, 0, cla); /* setTevColor(0) */
                gabi::FrameLocal<be<f32>[4]> f(0x6C);
                GXColorS10_to_f32(gabi::ea(f.get()), cla);
                gabi::FrameLocal<be<f32>[4]> sc(0x2C);
                u32 sa = gabi::ea(sc.get());
                gabi::call(0x0274D458, sa, f.get(), one);
                st(mat + 0xA0, ld(mat + 0xA0) | 0x10);
                u32 prm = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 4);
                f32 al = i2f(lds16(cla + 6));
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                stf(prm + 4, y);
                stf(prm + 8, z);
                stf(prm + 0, x);
                stf(prm + 0xC, al / k255);
            }
            u32 fb = ld(mat + 0x20);
            hasFog = gabi::call_ptr<u32>(vt0_l(fb, 0x14), fb) != 0;
        } else {
            u32 tb = ld(mat + 0x18);
            u32 c = gabi::call_ptr<u32>(vt4_l(tb, 0x34), tb, 0);
            if (c != 0) {
                st16(tev + 0x96, ld16(c + 6));
                tb = ld(mat + 0x18);
                gabi::call_ptr(vt4_l(tb, 0x24), tb, 0, tev + 0x90);
                gabi::FrameLocal<be<f32>[4]> f(0x9C);
                GXColorS10_to_f32(gabi::ea(f.get()), tev + 0x90);
                gabi::FrameLocal<be<f32>[4]> sc(0x5C);
                u32 sa = gabi::ea(sc.get());
                gabi::call(0x0274D458, sa, f.get(), one);
                st(mat + 0xA0, ld(mat + 0xA0) | 0x10);
                u32 prm = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 4);
                f32 al = i2f(lds16(tev + 0x96));
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                stf(prm + 4, y);
                stf(prm + 8, z);
                stf(prm + 0, x);
                stf(prm + 0xC, al / k255);
                u16 b = ld16(tev + 0x94), r = ld16(tev + 0x90), g = ld16(tev + 0x92);
                st16(tev + 0xDC, r);
                st16(tev + 0xDE, g);
                st16(tev + 0xE0, b);
                f32 w = ldf(tev + 0x28);
                u16 a = ld16(tev + 0x96);
                stf(tev + 0xE8, w);
                st16(tev + 0xE2, a);
                for (u32 i = 0; i < 3; i++) {
                    f32 base = (f32)ld8(envlight_l() + 0xBA4 + i);
                    s32 cc = lds16(tev + 0x90 + i * 2);
                    f32 d = i2f(cc - (s32)ld8(envlight_l() + 0xBA4 + i));
                    f32 v = gabi::fmadds(d, ldf(envlight_l() + 0x1030), base);
                    st16(tev + 0x160 + i * 2, (u8)gabi::ftoi(v));
                }
                stf(tev + 0x16C, ldf(envlight_l() + 0x10D0));
            }
            tb = ld(mat + 0x18);
            u32 kc = gabi::call_ptr<u32>(vt4_l(tb, 0x4C), tb, 0);
            if (kc != 0) {
                st8(tev + 0x9B, ld8(kc + 3));
                tb = ld(mat + 0x18);
                f32 scale = ldf(tev + 0x24);
                gabi::call_ptr(vt4_l(tb, 0x3C), tb, 0, tev + 0x98);
                gabi::FrameLocal<be<f32>[4]> f(0x8C);
                GXColor_to_f32(gabi::ea(f.get()), tev + 0x98);
                gabi::FrameLocal<be<f32>[4]> sc(0x4C);
                u32 sa = gabi::ea(sc.get());
                gabi::call(0x0274D458, sa, f.get(), scale);
                st(mat + 0xA0, ld(mat + 0xA0) | 0x80);
                u32 prm = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 7);
                f32 al = (f32)ld8(tev + 0x9B);
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                stf(prm + 4, y);
                stf(prm + 8, z);
                stf(prm + 0, x);
                stf(prm + 0xC, al / k255);
                st8(tev + 0x15C, 0);
                st8(tev + 0x15D, 0);
                st8(tev + 0x15E, 0);
                st8(tev + 0x15F, 0);
            }
            u32 cb = ld(mat + 0x10);
            gabi::call_ptr(vt0_l(cb, 0x3C), cb, 1, tev + 0x144); /* setLight(1, HD second light) */
            u32 fb = ld(mat + 0x20);
            hasFog = gabi::call_ptr<u32>(vt0_l(fb, 0x14), fb) != 0;
        }
        if (!hasFog) continue;
        u32 fb = ld(mat + 0x20);
        u32 fi = gabi::call_ptr<u32>(vt0_l(fb, 0x14), fb); /* getFogInfo */
        u8 ty = ld8(fi);
        if (!(ty == 0 || ty == 2)) continue;
        st8(fi, 2);
        /* lfs/stfs pairs keep the bits (a signalling NaN is not quietened by the recompiler here) */
        u32 szb = ld(tev + 0xA8);
        st(fi + 4, szb);
        u32 ezb = ld(tev + 0xAC);
        st(fi + 8, ezb);
        if (gabi::f32_from_bits(szb) > gabi::f32_from_bits(ezb)) st(fi + 4, ezb);
        stf(fi + 0xC, ldf(ld(dComIfGp_get_l() + 0x5FA4) + 0xCC));
        stf(fi + 0x10, ldf(ld(dComIfGp_get_l() + 0x5FA4) + 0xD0));
        st8(fi + 0x14, (u8)lds16(tev + 0xA0));
        st8(fi + 0x15, (u8)lds16(tev + 0xA2));
        st8(fi + 0x16, (u8)lds16(tev + 0xA4));
        st8(fi + 0x17, (u8)lds16(tev + 0xA6));
    }
}
VERIFY(0x02562F5C, dScnKy_env_light_c_setLightTevColorType);

/* 02563720: __sinit_d_kankyo_cpp: the per-TU header statics (10475A4C..10475A64) and the kankyo
 * statics 104773B8..104773BB */
void d_kankyo_sinit_hd() {
    WWHD_FUNC(0x02563720, void);
    st(0x10475A60, 0);
    st(0x10475A58, 0);
    st(0x10475A64, 0);
    st(0x10475A5C, 0);
    register_global_object_l(0x101E8ED0);
    f32 a = ldf(0x1004F348), b = ldf(0x1004F34C);
    stf(0x10475A4C, a);
    stf(0x10475A50, b);
    gabi::call(0x028ED6F8, 0x10475A54);
    register_global_object_l(0x101E8EDC);
    gabi::call(0x028EAB2C, 0x10475A55);
    register_global_object_l(0x101E8EE8);
    st8(0x104773B8, 0);
    st8(0x104773BB, 0xFF);
    st8(0x104773BA, 0);
    st8(0x104773B9, 0);
}
VERIFY(0x02563720, d_kankyo_sinit_hd);

/* 025637D4: LIGHT_INFLUENCE::LIGHT_INFLUENCE() (this TU's copy; HD float +0x20 = 1.0) */
u32 LIGHT_INFLUENCE_ct(u32 p) {
    WWHD_FUNC(0x025637D4, u32, p);
    if (p == 0) {
        p = operator_new_l(0x24);
        if (p == 0) return p;
    }
    stf(p + 0x20, ldf(0x1004F0A4));
    return p;
}
VERIFY(0x025637D4, LIGHT_INFLUENCE_ct);

/* 02563814: inline constructor of a 0x40-byte element (nothing to initialise) */
u32 ctor_0x40(u32 p) {
    WWHD_FUNC(0x02563814, u32, p);
    if (p == 0) p = operator_new_l(0x40);
    return p;
}
VERIFY(0x02563814, ctor_0x40);

/* 02563840: inline constructor of a 0x10-byte element (nothing to initialise) */
u32 ctor_0x10(u32 p) {
    WWHD_FUNC(0x02563840, u32, p);
    if (p == 0) p = operator_new_l(0x10);
    return p;
}
VERIFY(0x02563840, ctor_0x10);

/* 0256386C: deleting destructor of a trivially destructible class (SafeString vtable +0xC) */
void dtor_empty(u32 p, u32 flags) {
    WWHD_FUNC(0x0256386C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p); /* __dl */
}
VERIFY(0x0256386C, dtor_empty);

/* 02563880: dScnKy_env_light_c::~dScnKy_env_light_c() (member at +0xC40) */
void dScnKy_env_light_c_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x02563880, void, p, flags);
    if (p == 0) return;
    gabi::call(0x027FB528, p + 0xC40, 0);
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x02563880, dScnKy_env_light_c_dt);

/* 025638D4: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
void SafeString_assureTermination(u32 p) {
    WWHD_FUNC(0x025638D4, void, p);
}
VERIFY(0x025638D4, SafeString_assureTermination);

}  // namespace d_kankyo_cpp
