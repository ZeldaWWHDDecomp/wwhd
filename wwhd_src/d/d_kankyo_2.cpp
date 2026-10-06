/* d_kankyo: environment / lighting core, part 2 of 6 (025560C0..02557228), WWHD.
 * Effect lights, point light registration, fog distances, the vrbox addcol setters, time of
 * day, the HD colour conversion helpers and the dice weather. See d_kankyo.cpp for the TU. */
#include "d_kankyo_local.h"

namespace d_kankyo_cpp {

f32 fl_data_ratio_set(f32 a, f32 b, f32 ratio);
static inline f32 sqdist_l(u32 a, u32 b) { return gabi::call<f32>(0x028E8DE8, a, b); } /* PSVECSquareDistance */
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }

/* 025560C0: HD: float_kankyo_color_ratio_set without the all-colour ratio and the clamp */
f32 float_kankyo_color_ratio_set_noratio(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7) {
    WWHD_FUNC(0x025560C0, f32, p0, p1, p2, p3, p4, p5, p6, p7);
    f32 a = fl_data_ratio_set(p0, p1, p2);
    f32 b = fl_data_ratio_set(p3, p4, p2);
    f32 c = fl_data_ratio_set(a, b, p5);
    return gabi::fmadds(p6 - c, p7, c);
}
VERIFY(0x025560C0, float_kankyo_color_ratio_set_noratio);

/* 02556108: dKy_eflight_influence_id(cXyz pos, int nth); effect lights mpEfLights[10] at +0x790 */
s32 dKy_eflight_influence_id(u32 pos, s32 nth) {
    WWHD_FUNC(0x02556108, s32, pos, nth);
    const f32 far = ldf(0x1004F0B8);
    s32 first = -1, second = -1;
    f32 best = far;
    for (s32 i = 0; i <= nth; i++) {
        const f32 minPower = ldf(0x1004F0BC);
        for (s32 j = 0; j < 10; j++) {
            if (ld(envlight_l() + 0x790 + j * 4) == 0) continue;
            if (i != 0 && (u32)j == (u32)first) continue;
            f32 d = sqrtf_l(sqdist_l(pos, ld(envlight_l() + 0x790 + j * 4)));
            if (!(best > d)) continue;
            if (!(ldf(ld(envlight_l() + 0x790 + j * 4) + 0x14) > minPower)) continue;
            best = sqrtf_l(sqdist_l(pos, ld(envlight_l() + 0x790 + j * 4)));
            if (i == 0) first = j;
            else second = j;
        }
        best = far;
    }
    if (nth == 0) second = first;
    return second;
}
VERIFY(0x02556108, dKy_eflight_influence_id);

/* 02556288: dKy_eflight_influence_pos(int) returning cXyz (r3 = result storage; HD allocates when NULL) */
void dKy_eflight_influence_pos(u32 ret, s32 idx) {
    WWHD_FUNC(0x02556288, void, ret, idx);
    if (idx < 0) idx = 0;
    u32 l = ld(envlight_l() + 0x790 + idx * 4);
    if (ret == 0) {
        ret = operator_new_l(0xC);
        if (ret == 0) return;
    }
    stf(ret + 0, ldf(l + 0));
    stf(ret + 4, ldf(l + 4));
    stf(ret + 8, ldf(l + 8));
}
VERIFY(0x02556288, dKy_eflight_influence_pos);

/* 02556304 */
f32 dKy_eflight_influence_power(s32 idx) {
    WWHD_FUNC(0x02556304, f32, idx);
    if (idx < 0) idx = 0;
    return ldf(ld(envlight_l() + 0x790 + idx * 4) + 0x14);
}
VERIFY(0x02556304, dKy_eflight_influence_power);

/* 0255634C */
f32 dKy_eflight_influence_yuragi(s32 idx) {
    WWHD_FUNC(0x0255634C, f32, idx);
    if (idx < 0) idx = 0;
    return ldf(ld(envlight_l() + 0x790 + idx * 4) + 0x18);
}
VERIFY(0x0255634C, dKy_eflight_influence_yuragi);

/* 02556394 */
f32 dKy_eflight_influence_distance(u32 pos, s32 idx) {
    WWHD_FUNC(0x02556394, f32, pos, idx);
    if (idx < 0) idx = 0;
    return sqrtf_l(sqdist_l(pos, ld(envlight_l() + 0x790 + idx * 4)));
}
VERIFY(0x02556394, dKy_eflight_influence_distance);

/* 025563F0: plight_init: mLightInfluence[0].mPower (+0x4C), mpPLights[200] (+0x470), mpEfLights[10]
 * (+0x790), mpWaveInfl[10] (+0xAEC), mPlayerPLightIdx (+0x1074), mPlayerEflightIdx (+0x1078) */
void plight_init() {
    WWHD_FUNC(0x025563F0, void);
    stf(envlight_l() + 0x4C, ldf(0x1004F0C0));
    for (u32 i = 0; i < 200; i++) st(envlight_l() + 0x470 + i * 4, 0);
    for (u32 i = 0; i < 10; i++) st(envlight_l() + 0x790 + i * 4, 0);
    for (u32 i = 0; i < 10; i++) st(envlight_l() + 0xAEC + i * 4, 0);
    st(envlight_l() + 0x1074, (u32)-1);
    st(envlight_l() + 0x1078, (u32)-1);
}
VERIFY(0x025563F0, plight_init);

/* 025564B4 */
void dKy_plight_set(u32 light) {
    WWHD_FUNC(0x025564B4, void, light);
    for (u32 i = 0; i < 200; i++)
        if (ld(envlight_l() + 0x470 + i * 4) == light) return;
    for (u32 i = 0; i < 200; i++) {
        if (ld(envlight_l() + 0x470 + i * 4) == 0) {
            st(envlight_l() + 0x470 + i * 4, light);
            st(ld(envlight_l() + 0x470 + i * 4) + 0x1C, i + 1);
            return;
        }
    }
}
VERIFY(0x025564B4, dKy_plight_set);

/* 02556588: plight_set: stage point lights (HD entries 0x1C: pos, radius +0xC, colour +0x18,
 * fluctuation u8 +0x1B) into mLightInfluence[30] (+0x38, 0x24 each) */
void plight_set() {
    WWHD_FUNC(0x02556588, void);
    s32 idx = 0;
    u32 sd = dComIfGp_get_l() + 0x5150;
    u32 info = gabi::call_ptr<u32>(ld(ld(sd) + 0xDC), sd);
    if (info == 0) return;
    st(envlight_l() + 0x80C, info);
    s32 i = 0;
    sd = dComIfGp_get_l() + 0x5150;
    if (!(i < gabi::call_ptr<s32>(ld(ld(sd) + 0x12C), sd))) return;
    const f32 scale = ldf(0x1004F0C4);
    for (;;) {
        if (idx < 30) {
            u32 o = idx * 0x24;
            stf(envlight_l() + o + 0x38, ldf(info + 0));
            stf(envlight_l() + o + 0x3C, ldf(info + 4));
            stf(envlight_l() + o + 0x40, ldf(info + 8));
            st16(envlight_l() + o + 0x44, ld8(info + 0x18));
            st16(envlight_l() + o + 0x46, ld8(info + 0x19));
            st16(envlight_l() + o + 0x48, ld8(info + 0x1A));
            stf(envlight_l() + o + 0x4C, ldf(info + 0xC) * scale);
            stf(envlight_l() + o + 0x50, (f32)ld8(info + 0x1B));
            dKy_plight_set(envlight_l() + 0x38 + o);
            idx++;
        }
        info += 0x1C;
        i++;
        sd = dComIfGp_get_l() + 0x5150;
        if (!(i < gabi::call_ptr<s32>(ld(ld(sd) + 0x12C), sd))) return;
    }
}
VERIFY(0x02556588, plight_set);

/* 0255671C: mFogStartZ +0xFD8, mFogEndZ +0xFDC, mFogRatio +0xFE0 */
void dKy_fog_startendz_set(f32 startZ, f32 endZ, f32 ratio) {
    WWHD_FUNC(0x0255671C, void, startZ, endZ, ratio);
    const f32 zero = ldf(0x1004F0B0);
    if (ratio < zero || ratio > ldf(0x1004F0A4)) {
        gabi::call(0x025F27E8, 0x1004F0D4); /* OSReport_Warning("\ndKy_fog_startendz_set ratio error!\n") */
        ratio = zero;
    } else if (ratio < ldf(0x1004F0D0)) {
        ratio = zero;
    }
    stf(envlight_l() + 0xFD8, startZ);
    stf(envlight_l() + 0xFDC, endZ);
    stf(envlight_l() + 0xFE0, ratio);
}
VERIFY(0x0255671C, dKy_fog_startendz_set);

static inline void addcol_set2(u32 off, s32 r, s32 g, s32 b, f32 factor) {
    u32 env = envlight_l();
    gabi::store<s16>(env + off, (s16)gabi::ftoi(i2f(r) * factor));
    env = envlight_l();
    gabi::store<s16>(env + off + 2, (s16)gabi::ftoi(i2f(g) * factor));
    env = envlight_l();
    gabi::store<s16>(env + off + 4, (s16)gabi::ftoi(i2f(b) * factor));
}

/* 025567FC */
void dKy_vrbox_addcol_sky0_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x025567FC, void, r, g, b, factor);
    addcol_set2(0xFB0, r, g, b, factor);
}
VERIFY(0x025567FC, dKy_vrbox_addcol_sky0_set);

/* 02556910 */
void dKy_vrbox_addcol_kasumi_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02556910, void, r, g, b, factor);
    addcol_set2(0xFB8, r, g, b, factor);
}
VERIFY(0x02556910, dKy_vrbox_addcol_kasumi_set);

/* 02556A24 */
void dKy_addcol_fog_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02556A24, void, r, g, b, factor);
    addcol_set2(0xFA8, r, g, b, factor);
}
VERIFY(0x02556A24, dKy_addcol_fog_set);

/* 02556B38 */
void dKy_vrbox_addcol_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02556B38, void, r, g, b, factor);
    dKy_vrbox_addcol_sky0_set(r, g, b, factor);
    dKy_vrbox_addcol_kasumi_set(r, g, b, factor);
    dKy_addcol_fog_set(r, g, b, factor);
}
VERIFY(0x02556B38, dKy_vrbox_addcol_set);

/* 02556BC0: ENDLESS_NIGHT (0x0A02) and not dSymbol_NAYRU */
BOOL dKy_checkEventNightStop() {
    WWHD_FUNC(0x02556BC0, BOOL);
    if (isEventBit_l(save_l() + 0x644, 0xA02) && !gabi::call<BOOL>(0x025B7D90, save_l() + 0xD4, 0)) return TRUE;
    return FALSE;
}
VERIFY(0x02556BC0, dKy_checkEventNightStop);

/* 02556C34 */
s32 dKy_getdaytime_hour() {
    WWHD_FUNC(0x02556C34, s32);
    f32 k = ldf(0x1004F0FC);
    return gabi::ftoi(ldf(save_l() + 0x44) / k);
}
VERIFY(0x02556C34, dKy_getdaytime_hour);

/* 02556C68 */
s32 dKy_getdaytime_minute() {
    WWHD_FUNC(0x02556C68, s32);
    f32 m = ldf(0x1004F0B8);
    s32 t = gabi::ftoi(ldf(save_l() + 0x44) * m);
    f32 tmp = i2f(t % 15000000) / m;
    return gabi::ftoi(tmp / ldf(0x1004F0FC) * ldf(0x1004F100));
}
VERIFY(0x02556C68, dKy_getdaytime_minute);

/* 02556D14 */
s32 dKy_daynight_check() {
    WWHD_FUNC(0x02556D14, s32);
    s32 hour = dKy_getdaytime_hour();
    return (u32)(hour - 6) >= 12 ? 1 : 0;
}
VERIFY(0x02556D14, dKy_daynight_check);

/* 02556D44: HD: copy of a 12-float structure through FPRs (lfs quiets signalling NaNs) */
void copy_f32x12(u32 dst, u32 src) {
    WWHD_FUNC(0x02556D44, void, dst, src);
    f32 v[12];
    for (u32 i = 0; i < 12; i++) v[i] = ldf(src + i * 4);
    for (u32 i = 0; i < 12; i++) stf(dst + i * 4, v[i]);
}
VERIFY(0x02556D44, copy_f32x12);

/* 02556DE4: HD: GXColor (u8 RGBA) -> 4 floats / 255 */
void GXColor_to_f32(u32 dst, u32 src) {
    WWHD_FUNC(0x02556DE4, void, dst, src);
    f32 k = ldf(0x1004F104);
    f32 r = (f32)ld8(src + 0) / k, g = (f32)ld8(src + 1) / k, b = (f32)ld8(src + 2) / k, a = (f32)ld8(src + 3) / k;
    stf(dst + 0, r);
    stf(dst + 4, g);
    stf(dst + 8, b);
    stf(dst + 0xC, a);
}
VERIFY(0x02556DE4, GXColor_to_f32);

/* 02556E98: HD: GXColorS10 (s16 RGBA) -> 4 floats / 255 */
void GXColorS10_to_f32(u32 dst, u32 src) {
    WWHD_FUNC(0x02556E98, void, dst, src);
    f32 k = ldf(0x1004F104);
    f32 r = i2f(lds16(src + 0)) / k, g = i2f(lds16(src + 2)) / k, b = i2f(lds16(src + 4)) / k, a = i2f(lds16(src + 6)) / k;
    stf(dst + 0, r);
    stf(dst + 4, g);
    stf(dst + 8, b);
    stf(dst + 0xC, a);
}
VERIFY(0x02556E98, GXColorS10_to_f32);

/* 02556F5C: mDiceWeatherMode +0x1093, mDiceWeatherTime +0xFC4 */
void dice_wether_init(u32 mode, f32 weatherTime, f32 curTime) {
    WWHD_FUNC(0x02556F5C, void, mode, weatherTime, curTime);
    u32 env = envlight_l();
    f32 t = curTime + weatherTime;
    f32 k = ldf(0x1004F108);
    st8(env + 0x1093, (u8)mode);
    if (!(t < k)) t = t - k;
    stf(env + 0xFC4, t);
}
VERIFY(0x02556F5C, dice_wether_init);

/* 02556FE0: mDiceWeatherCounter +0x1068, mDiceWeatherState +0x1094 */
void dice_wether_execute(u32 mode, f32 weatherTime, f32 curTime) {
    WWHD_FUNC(0x02556FE0, void, mode, weatherTime, curTime);
    u32 env = envlight_l();
    if (mode == 0xFF) {
        u8 s = ld8(env + 0x1094);
        st8(env + 0x1093, 0);
        st8(env + 0x1094, (u8)(s + 1));
        return;
    }
    f32 t = curTime + weatherTime;
    f32 k = ldf(0x1004F108);
    st8(env + 0x1093, (u8)mode);
    u32 cnt = ld(env + 0x1068);
    if (!(t < k)) t = t - k;
    stf(env + 0xFC4, t);
    st(env + 0x1068, cnt + 1);
}
VERIFY(0x02556FE0, dice_wether_execute);

/* 025570B4: mRainCount +0xA40 */
void dice_rain_minus() {
    WWHD_FUNC(0x025570B4, void);
    u32 env = envlight_l();
    s32 n = (s32)ld(env + 0xA40);
    if (n > 40) {
        n -= 3;
        st(env + 0xA40, (u32)n);
    } else if (n != 0) {
        n -= 1;
        st(env + 0xA40, (u32)n);
    }
    gabi::call(0x0257E7C0, n); /* dKyw_rain_set */
}
VERIFY(0x025570B4, dice_rain_minus);

s32 dKy_get_dayofweek();

/* 02557108: phantomship_wether (HD = GameCube USA: 285.0) */
u8 phantomship_wether() {
    WWHD_FUNC(0x02557108, u8);
    f32 t = ldf(save_l() + 0x44);
    s32 weekday = dKy_get_dayofweek();
    if (t < ldf(0x1004F10C)) {
        if (weekday != 0) weekday--;
        else weekday = 6;
    }
    u8 rt = 0;
    if (t > ldf(0x1004F110) || t < ldf(0x1004F114)) {
        s32 roomNo = (s8)ld8(0x1047E6C8);
        if ((roomNo == 5 && weekday == 0) || (roomNo == 0x24 && weekday == 1) || (roomNo == 0x22 && weekday == 2) ||
            (roomNo == 0xA && weekday == 3) || (roomNo == 0x31 && weekday == 4) || (roomNo == 0x15 && weekday == 5) ||
            (roomNo == 0x17 && weekday == 6))
            rt = 1;
    }
    return rt;
}
VERIFY(0x02557108, phantomship_wether);

}  // namespace d_kankyo_cpp
