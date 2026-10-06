/* d_kankyo: environment / lighting core, part 4 of 6 (0255A2B8..0255DAC0), WWHD.
 * Point light priority/cut, colour ratio setters, the item-get colour change, the HD per-frame
 * light update with its colour pulses, setDaytime, CalcTevColor, sound, effect lights, SetSchbit,
 * exeKankyo, dKy_event_proc and the process methods. See d_kankyo.cpp for the TU. */
#include "d_kankyo_local.h"

namespace d_kankyo_cpp {

static inline f32 cLib_addCalc_l(u32 p, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, p, target, scale, maxStep, minStep);
}
void dKy_actor_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_actor_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg1_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg1_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg2_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg2_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg3_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg3_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
BOOL toon_proc_check();
void dScnKy_env_light_c_setSunpos(u32 t);
void dScnKy_env_light_c_setLightPos_hd(u32 t);
void dScnKy_env_light_c_SetBaseLight(u32 t);
void dScnKy_env_light_c_setLight(u32 t);

struct SafeStr_l {
    be<u32> str;
    be<u32> vtbl;
};
static constexpr u32 SafeString_vtbl = 0x1004EF7C;
/* sead::SafeString operator== against the start stage name (see d_kankyo_3.cpp) */
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

/* 0255A2B8 */
void dKy_plight_priority_set(u32 light) {
    WWHD_FUNC(0x0255A2B8, void, light);
    for (u32 i = 0; i < 200; i++) {
        if (ld(envlight_l() + 0x470 + i * 4) == 0) {
            st(envlight_l() + 0x470 + i * 4, light);
            st(ld(envlight_l() + 0x470 + i * 4) + 0x1C, (u32)(-(s32)(i + 1)));
            return;
        }
    }
    gabi::call(0x025F27E8, 0x1004F1E0); /* OSReport_Warning("\nPOINTLIGHT COUNT OVER!!!\n") */
}
VERIFY(0x0255A2B8, dKy_plight_priority_set);

/* 0255A374 */
void dKy_plight_cut(u32 light) {
    WWHD_FUNC(0x0255A374, void, light);
    if (light == 0) return;
    s32 idx = (s32)ld(light + 0x1C);
    if (idx <= 0) {
        if (idx == 0) return;
        idx = (s32)(0u - (u32)idx);
        st(light + 0x1C, (u32)idx);
    }
    s32 i = (s32)((u32)idx - 1);
    if (i < 200) st(envlight_l() + 0x470 + i * 4, 0);
}
VERIFY(0x0255A374, dKy_plight_cut);

/* colour ratio setters: the "gather" ratios (HD +0x1000..+0x1010) */
/* 0255A3D8 */
void dKy_set_actcol_ratio(f32 ratio) {
    WWHD_FUNC(0x0255A3D8, void, ratio);
    stf(envlight_l() + 0x1000, ratio);
}
VERIFY(0x0255A3D8, dKy_set_actcol_ratio);

/* 0255A418 */
void dKy_set_bgcol_ratio(f32 ratio) {
    WWHD_FUNC(0x0255A418, void, ratio);
    stf(envlight_l() + 0x1004, ratio);
}
VERIFY(0x0255A418, dKy_set_bgcol_ratio);

/* 0255A458: HD calls the accessor twice */
void dKy_set_fogcol_ratio(f32 ratio) {
    WWHD_FUNC(0x0255A458, void, ratio);
    envlight_l();
    stf(envlight_l() + 0x1008, ratio);
}
VERIFY(0x0255A458, dKy_set_fogcol_ratio);

/* 0255A49C */
void dKy_set_vrboxsoracol_ratio(f32 ratio) {
    WWHD_FUNC(0x0255A49C, void, ratio);
    stf(envlight_l() + 0x100C, ratio);
}
VERIFY(0x0255A49C, dKy_set_vrboxsoracol_ratio);

/* 0255A4DC */
void dKy_set_vrboxkumocol_ratio(f32 ratio) {
    WWHD_FUNC(0x0255A4DC, void, ratio);
    stf(envlight_l() + 0x1010, ratio);
}
VERIFY(0x0255A4DC, dKy_set_vrboxkumocol_ratio);

/* 0255A51C */
void dKy_set_vrboxcol_ratio(f32 ratio) {
    WWHD_FUNC(0x0255A51C, void, ratio);
    envlight_l();
    dKy_set_vrboxsoracol_ratio(ratio);
    dKy_set_vrboxkumocol_ratio(ratio);
}
VERIFY(0x0255A51C, dKy_set_vrboxcol_ratio);

/* 0255A568: dKy_Itemgetcol_chg_move: mColChgFlag +0x109B (low nibble), mColChgLight +0x7B8,
 * strength +0x101C, flag +0x10A0. HD: the light rises by 8 per frame to 255 (GameCube: 1 to 25),
 * power 200, strength towards 1 at 0.1 and back at 0.25 / 1e-7, and the gather ratios are
 * lowered while it is on (actor 0.5, the rest 0.75 of the strength). */
void dKy_Itemgetcol_chg_move() {
    WWHD_FUNC(0x0255A568, void);
    u32 cam = ld(dComIfGp_get_l() + 0x5AF8);
    u32 env = envlight_l();
    u32 player = ld(dComIfGp_get_l() + 0x5B2C);
    const f32 one = ldf(0x1004F0A4), half = ldf(0x1004F1C8), zero = ldf(0x1004F0B0);
    u8 flag = ld8(env + 0x109B);
    bool fade = false;
    switch (flag) {
    case 0:
        break;
    case 1:
        st16(env + 0x7C4, 0);
        st8(env + 0x109B, 2);
        break;
    case 2:
    case 3:
    case 4:
        if (flag == 2) {
            f32 y = ldf(player + 0x318), z = ldf(player + 0x31C), x = ldf(player + 0x314);
            s32 offsAngle, offsY;
            if (toon_proc_check()) {
                offsAngle = 13000;
                offsY = -70;
            } else {
                offsY = 280;
                offsAngle = 23000;
            }
            gabi::Local<cXyz> v;
            gabi::call(0x02563F64, cam + 0xDC, cam + 0xE8, v.get()); /* dKyr_get_vectle_calc */
            u32 va = gabi::ea(v.get());
            f32 vz = ldf(va + 8), vx = ldf(va);
            gabi::call<f32>(0x028F4384, gabi::fmadds(vx, vx, vz * vz));
            s16 a = gabi::call<s16>(0x020195B0, ldf(va), ldf(va + 8)); /* cM_atan2s */
            u32 ang = (u16)(a - offsAngle);
            const u32 tbl = 0x104A44F8;
            f32 c0 = ldf(tbl + 4);
            f32 fx = c0 * ldf(tbl + (ang >> 3) * 8);
            f32 fz = c0 * ldf(tbl + (ang >> 3) * 8 + 4);
            stf(va, fx);
            stf(va + 8, fz);
            stf(va + 4, ldf(tbl));
            const f32 k80 = ldf(0x1004F1FC);
            y = y + i2f(offsY);
            st16(env + 0x7C4, 0);
            x = gabi::fmadds(k80, fx, x);
            st16(env + 0x7C6, 0);
            stf(env + 0x7BC, y);
            stf(env + 0x7B8, x);
            stf(env + 0x7CC, ldf(0x1004F0C4));
            stf(env + 0x7D0, zero);
            st16(env + 0x7C8, 0);
            stf(env + 0x7C0, gabi::fmadds(k80, fz, z));
            dKy_plight_priority_set(env + 0x7B8);
            st8(env + 0x109B, 3);
        }
        if (flag <= 3) {
            s16 r = (s16)(lds16(env + 0x7C4) + 8);
            s16 g = (s16)(lds16(env + 0x7C6) + 8);
            s16 b = (s16)(lds16(env + 0x7C8) + 8);
            st16(env + 0x7C4, (u16)r);
            st16(env + 0x7C6, (u16)g);
            st16(env + 0x7C8, (u16)b);
            if ((s32)r + 8 > 0xFF) {
                st8(env + 0x109B, 4);
                st16(env + 0x7C4, 0xFF);
                st16(env + 0x7C6, 0xFF);
                st16(env + 0x7C8, 0xFF);
            }
        }
        cLib_addCalc_l(env + 0x101C, one, half, ldf(0x1004F17C), ldf(0x1004F178));
        break;
    case 5:
    case 6:
        if (flag == 5) {
            dKy_plight_cut(env + 0x7B8);
            st8(env + 0x109B, 6);
        }
        cLib_addCalc_l(env + 0x101C, zero, half, ldf(0x1004F138), ldf(0x1004F0BC));
        if (ldf(env + 0x101C) < ldf(0x1004F200)) {
            st8(env + 0x109B, 0);
            stf(env + 0x101C, zero);
        }
        break;
    default:
        break;
    }
    flag = ld8(env + 0x109B);
    st8(env + 0x10A0, 0);
    fade = (flag & 7) != 0;
    if (!fade) return;
    f32 s = ldf(env + 0x101C);
    st8(env + 0x10A0, 1);
    dKy_set_actcol_ratio(gabi::fnmsubs(s, half, one));
    const f32 k = ldf(0x1004F204);
    dKy_set_bgcol_ratio(gabi::fnmsubs(ldf(env + 0x101C), k, one));
    dKy_set_fogcol_ratio(gabi::fnmsubs(ldf(env + 0x101C), k, one));
    dKy_set_vrboxcol_ratio(gabi::fnmsubs(ldf(env + 0x101C), k, one));
    dKy_set_vrboxsoracol_ratio(gabi::fnmsubs(ldf(env + 0x101C), k, one));
    dKy_set_vrboxkumocol_ratio(gabi::fnmsubs(ldf(env + 0x101C), k, one));
    s16 nr = (s16)(0 - (s32)ld8(env + 0xB64)), ng = (s16)(0 - (s32)ld8(env + 0xB65)), nb = (s16)(0 - (s32)ld8(env + 0xB66));
    dKy_actor_addcol_amb_set(nr, ng, nb, ldf(env + 0x101C));
    dKy_actor_addcol_dif_set(0, 0, 0, ldf(env + 0x101C));
}
VERIFY(0x0255A568, dKy_Itemgetcol_chg_move);

/* (s16)(c - colour byte) */
static inline s32 sub8_l(s32 c, u32 a) { return (s16)(c - (s32)ld8(a)); }

/* 0255A924: dScnKy_env_light_c::drawKankyo() (unnamed by the matcher): setSunpos, the HD light
 * position, SetBaseLight, setLight, dKy_Itemgetcol_chg_move, then dKy_arrowcol_chg_move inlined
 * as the HD colour pulses of mColChgFlag's high nibble (+0x109B): 0x10/0x40 orange,
 * 0x20/0x50 blue, 0x30/0x60 green ("GTower": its own colours); 0x10..0x30 ease the strength
 * +0x101C to 1 (then +0x30), 0x40..0x60 ease it to 0 (then clear); the addcols pull the actor,
 * BG0..BG3 C0 (amb) / K0 (dif) colours towards the pulse colour (BG1 at 0.6/0.2 of it; in
 * "GTower" BG2 instead and BG3 to fixed colours). */
void dScnKy_env_light_c_drawKankyo(u32 t) {
    WWHD_FUNC(0x0255A924, void, t);
    dScnKy_env_light_c_setSunpos(t);
    dScnKy_env_light_c_setLightPos_hd(t);
    dScnKy_env_light_c_SetBaseLight(t);
    dScnKy_env_light_c_setLight(t);
    dKy_Itemgetcol_chg_move();
    u32 env = envlight_l();
    u8 flag = ld8(env + 0x109B);
    gabi::Local<be<s16>[8]> col; /* C0 rgb at +0, K0 rgb at +8 (uninitialised for other flags) */
    auto setc = [&](s16 r0, s16 g0, s16 b0, s16 r1, s16 g1, s16 b1) {
        (*col)[0] = r0;
        (*col)[1] = g0;
        (*col)[2] = b0;
        (*col)[4] = r1;
        (*col)[5] = g1;
        (*col)[6] = b1;
    };
    if (flag == 0x10 || flag == 0x40) {
        setc(0xFF, 0x50, 0x14, 0xFF, 0x50, 0x14);
    } else if (flag == 0x20 || flag == 0x50) {
        setc(0x1E, 0xA0, 0xFF, 0x1E, 0xA0, 0xFF);
    } else if (flag == 0x30 || flag == 0x60) {
        if (stage_name_eq_l(0x1004EF74)) /* "GTower" */
            setc(0x45, 0x1B, 0, 0x74, 0x65, 0x10);
        else
            setc(0xE1, 0xFF, 0x3C, 0xE1, 0xFF, 0x3C);
        flag = ld8(env + 0x109B);
    }
    if (flag == 0x10 || flag == 0x20 || flag == 0x30) {
        const f32 one = ldf(0x1004F0A4);
        cLib_addCalc_l(env + 0x101C, one, ldf(0x1004F1C8), ldf(0x1004F208), ldf(0x1004F178));
        if (!(ldf(env + 0x101C) < one)) {
            flag = (u8)(ld8(env + 0x109B) + 0x30);
            st8(env + 0x109B, flag);
        } else {
            flag = ld8(env + 0x109B);
        }
    } else if (flag == 0x40 || flag == 0x50 || flag == 0x60) {
        const f32 zero = ldf(0x1004F0B0);
        cLib_addCalc_l(env + 0x101C, zero, ldf(0x1004F1C8), ldf(0x1004F20C), ldf(0x1004F178));
        if (ldf(env + 0x101C) < ldf(0x1004F210)) {
            st8(env + 0x109B, 0);
            stf(env + 0x101C, zero);
            return;
        }
        flag = ld8(env + 0x109B);
    }
    if ((flag & 0xF0) == 0) return;
    s32 cr = (*col)[0], cg = (*col)[1], cb = (*col)[2];
    dKy_actor_addcol_amb_set(sub8_l(cr, env + 0xB64), sub8_l(cg, env + 0xB65), sub8_l(cb, env + 0xB66), ldf(env + 0x101C));
    dKy_bg_addcol_amb_set(sub8_l(cr, env + 0xB6C), sub8_l(cg, env + 0xB6D), sub8_l(cb, env + 0xB6E), ldf(env + 0x101C));
    s32 r3, g3, b3;
    if (stage_name_eq_l(0x1004EF74)) {
        dKy_bg2_addcol_amb_set(sub8_l(cr, env + 0xB7C), sub8_l(cg, env + 0xB7D), sub8_l(cb, env + 0xB7E), ldf(env + 0x101C));
    } else {
        dKy_bg1_addcol_amb_set(sub8_l(cr, env + 0xB74), sub8_l(cg, env + 0xB75), sub8_l(cb, env + 0xB76),
                               ldf(env + 0x101C) * ldf(0x1004F214));
        dKy_bg2_addcol_amb_set(sub8_l(cr, env + 0xB7C), sub8_l(cg, env + 0xB7D), sub8_l(cb, env + 0xB7E), ldf(env + 0x101C));
    }
    r3 = sub8_l(cr, env + 0xB84);
    g3 = sub8_l(cg, env + 0xB85);
    b3 = sub8_l(cb, env + 0xB86);
    if (stage_name_eq_l(0x1004EF74)) {
        r3 = sub8_l(0xB7, env + 0xB84);
        g3 = sub8_l(0xB0, env + 0xB85);
        b3 = sub8_l(0x88, env + 0xB86);
    }
    dKy_bg3_addcol_amb_set(r3, g3, b3, ldf(env + 0x101C));
    s32 kr = (*col)[4], kg = (*col)[5], kb = (*col)[6];
    dKy_actor_addcol_dif_set(sub8_l(kr, env + 0xB68), sub8_l(kg, env + 0xB69), sub8_l(kb, env + 0xB6A), ldf(env + 0x101C));
    dKy_bg_addcol_dif_set(sub8_l(kr, env + 0xB70), sub8_l(kg, env + 0xB71), sub8_l(kb, env + 0xB72), ldf(env + 0x101C));
    if (stage_name_eq_l(0x1004EF74)) {
        dKy_bg2_addcol_dif_set(sub8_l(kr, env + 0xB80), sub8_l(kg, env + 0xB81), sub8_l(kb, env + 0xB82), ldf(env + 0x101C));
    } else {
        dKy_bg1_addcol_dif_set(sub8_l(kr, env + 0xB78), sub8_l(kg, env + 0xB79), sub8_l(kb, env + 0xB7A),
                               ldf(env + 0x101C) * ldf(0x1004F218));
        dKy_bg2_addcol_dif_set(sub8_l(kr, env + 0xB80), sub8_l(kg, env + 0xB81), sub8_l(kb, env + 0xB82), ldf(env + 0x101C));
    }
    r3 = sub8_l(kr, env + 0xB88);
    b3 = sub8_l(kb, env + 0xB8A);
    g3 = sub8_l(kg, env + 0xB89);
    if (stage_name_eq_l(0x1004EF74)) {
        r3 = sub8_l(0x8A, env + 0xB84);
        g3 = sub8_l(0x8A, env + 0xB85);
        b3 = sub8_l(0x44, env + 0xB86);
    }
    dKy_bg3_addcol_dif_set(r3, g3, b3, ldf(env + 0x101C));
}
VERIFY(0x0255A924, dScnKy_env_light_c_drawKankyo);

/* 0255B2D8: dKy_Draw (unnamed by the matcher): drawKankyo on the env light, returns TRUE */
BOOL dKy_Draw() {
    WWHD_FUNC(0x0255B2D8, BOOL);
    dScnKy_env_light_c_drawKankyo(envlight_l());
    return TRUE;
}
VERIFY(0x0255B2D8, dKy_Draw);

BOOL dKy_checkEventNightStop();
void dKankyo_DayProc();
s32 dKy_getdaytime_hour();
s32 dKy_getdaytime_minute();
s32 dKy_get_dayofweek();
s32 dKy_daynight_check();

/* 0255B300: dScnKy_env_light_c::setDaytime(): mCurTime +0x1020, mDayOfWeek +0x108A, mTimeAdv
 * +0x1028, m_time_pass 101EBFD8. HD: time also stops in "sea" room 14 layers 2/3 (as GameCube
 * USA); while stopped it still runs in "A_umikz" up to 270; in "ENDumi" it runs up to 180. */
void dScnKy_env_light_c_setDaytime(u32 t) {
    WWHD_FUNC(0x0255B300, void, t);
    stf(t + 0x1020, ldf(save_l() + 0x44));
    st16(t + 0x108A, ld16(save_l() + 0x48));
    bool stop = false;
    if (stage_name_eq_l(0x1004F22C)) { /* "sea" */
        if ((s8)ld8(0x1047E6C8) == 0xE) {
            if ((s8)ld8(dComIfGp_get_l() + 0x513F) == 2 || (s8)ld8(dComIfGp_get_l() + 0x513F) == 3) stop = true;
        }
    }
    bool nightStop = dKy_checkEventNightStop();
    const f32 k270 = ldf(0x1004F21C);
    bool endumi;
    if (nightStop || !gabi::call<BOOL>(0x025B5D40, save_l() + 0x71, 2, 0) || ld8(dComIfGp_get_l() + 0x5292) != 0 || stop) {
        if (stage_name_eq_l(0x1004F224)) { /* "A_umikz" */
            f32 v = ldf(t + 0x1020) + ldf(t + 0x1028);
            if (v < k270) stf(t + 0x1020, v);
        }
        endumi = stage_name_eq_l(0x1004F230);
    } else {
        dComIfGp_get_l();
        if (ld8(0x101EBFD8) != 0) { /* m_time_pass */
            f32 v = ldf(t + 0x1020) + ldf(t + 0x1028);
            const f32 k2p31 = ldf(0x1004F220);
            f32 u;
            if (v < k2p31) u = (f32)(f64)(u32)gabi::ftoi(v);
            else u = (f32)(f64)(u32)((u32)gabi::ftoi(v - k2p31) + 0x80000000u);
            if (!(u < ldf(0x1004F108))) {
                st16(t + 0x108A, (u16)(ld16(t + 0x108A) + 1));
                stf(t + 0x1020, ldf(0x1004F0B0));
                dKankyo_DayProc();
            } else {
                stf(t + 0x1020, v);
            }
        } else {
            s32 dn = dKy_daynight_check();
            f32 cur = ldf(t + 0x1020);
            bool run;
            if (dn == 0) run = cur < ldf(0x1004F12C);
            else run = !(cur < k270) && cur < ldf(0x1004F118);
            if (run) stf(t + 0x1020, cur + ldf(t + 0x1028));
        }
        endumi = stage_name_eq_l(0x1004F230);
    }
    if (endumi) { /* "ENDumi" */
        f32 cur = ldf(t + 0x1020);
        if (cur < ldf(0x1004F10C)) stf(t + 0x1020, cur + ldf(t + 0x1028));
    }
    gabi::call(0x025E1F80, dKy_getdaytime_hour());   /* mDoAud_setHour */
    gabi::call(0x025E1F90, dKy_getdaytime_minute()); /* mDoAud_setMinute */
    gabi::call(0x025E1FA0, dKy_get_dayofweek());     /* mDoAud_setWeekday */
    stf(save_l() + 0x44, ldf(t + 0x1020));
    u32 sv = save_l();
    st16(sv + 0x48, ld16(t + 0x108A));
}
VERIFY(0x0255B300, dScnKy_env_light_c_setDaytime);

s32 dKy_eflight_influence_id(u32 pos, s32 nth);
s16 u8_data_ratio_set(s32 a, s32 b, f32 ratio);

/* 0255B8D8: dScnKy_env_light_c::CalcTevColor(): HD keeps only the effect light of the player
 * (mPlayerEflightIdx +0x1078); the point light lookup is gone */
void dScnKy_env_light_c_CalcTevColor(u32 t) {
    WWHD_FUNC(0x0255B8D8, void, t);
    u32 player = ld(dComIfGp_get_l() + 0x5B2C);
    gabi::Local<cXyz> pos;
    u32 pa = gabi::ea(pos.get());
    stf(pa + 0, ldf(player + 0x314));
    stf(pa + 4, ldf(player + 0x318));
    stf(pa + 8, ldf(player + 0x31C));
    st(t + 0x1078, (u32)dKy_eflight_influence_id(pa, 0));
}
VERIFY(0x0255B8D8, dScnKy_env_light_c_CalcTevColor);

/* 0255B930: mSound (SND_INFLUENCE) +0x9B4 */
void dKy_Sound_init() {
    WWHD_FUNC(0x0255B930, void);
    const f32 far = ldf(0x1004F238);
    stf(envlight_l() + 0x9B4, far);
    stf(envlight_l() + 0x9B8, far);
    stf(envlight_l() + 0x9BC, far);
    st(envlight_l() + 0x9C0, 0);
    st(envlight_l() + 0x9C8, (u32)-1);
    st(envlight_l() + 0x9C4, 0);
}
VERIFY(0x0255B930, dKy_Sound_init);

/* 0255B9AC: dScnKy_env_light_c::Sndpos() */
void dScnKy_env_light_c_Sndpos(u32 t) {
    WWHD_FUNC(0x0255B9AC, void, t);
    u32 n = ld(t + 0x9C4);
    if (n == 0) return;
    n--;
    st(t + 0x9C4, n);
    if (n == 0) dKy_Sound_init();
}
VERIFY(0x0255B9AC, dScnKy_env_light_c_Sndpos);

/* 0255B9C8: mpEfLights[10] +0x790 */
void dKy_efplight_set(u32 light) {
    WWHD_FUNC(0x0255B9C8, void, light);
    for (u32 i = 0; i < 10; i++)
        if (ld(envlight_l() + 0x790 + i * 4) == light) return;
    for (u32 i = 0; i < 10; i++) {
        if (ld(envlight_l() + 0x790 + i * 4) == 0) {
            st(envlight_l() + 0x790 + i * 4, light);
            st(ld(envlight_l() + 0x790 + i * 4) + 0x1C, i + 1);
            return;
        }
    }
}
VERIFY(0x0255B9C8, dKy_efplight_set);

/* 0255BA9C */
void dKy_efplight_cut(u32 light) {
    WWHD_FUNC(0x0255BA9C, void, light);
    if (light == 0) return;
    u32 idx = ld(light + 0x1C);
    if (idx == 0) return;
    u32 i = idx - 1;
    if (i < 10) st(envlight_l() + 0x790 + i * 4, 0);
}
VERIFY(0x0255BA9C, dKy_efplight_cut);

/* 0255BAF4: dScnKy_env_light_c::Eflight_flush_proc(): mEfLightProc +0x7E0 (state), +0x7E1 (frame),
 * +0x7E4 (light type), mSwordLight +0x7E8; colour tables flush_col 101E8EA8 / flush_col2 101E8EB8.
 * HD: power 700 (GameCube 1000) and fluctuation 100 for both types. */
void dScnKy_env_light_c_Eflight_flush_proc(u32 t) {
    WWHD_FUNC(0x0255BAF4, void, t);
    u32 tbl = ld(t + 0x7E4) != 0 ? 0x101E8EB8 : 0x101E8EA8;
    u8 state = ld8(t + 0x7E0);
    switch (state) {
    case 1: {
        st8(t + 0x7E1, 0);
        st16(t + 0x7F4, ld8(tbl + 1));
        st16(t + 0x7F6, ld8(tbl + 2));
        stf(t + 0x7FC, ldf(0x1004F23C));
        st16(t + 0x7F8, ld8(tbl + 3));
        stf(t + 0x800, ldf(0x1004F240));
        dKy_efplight_set(t + 0x7E8);
        st8(t + 0x7E0, (u8)(ld8(t + 0x7E0) + 1));
        return;
    }
    case 2: {
        const f32 one = ldf(0x1004F0A4);
        u32 frame = ld8(t + 0x7E1);
        u32 last = frame;
        bool found = false;
        for (u32 i = 0; i < 3; i++) {
            u32 cur = tbl + i * 4, next = cur + 4;
            u32 cf = ld8(cur);
            if (frame < cf) continue;
            u32 nf = ld8(next);
            if (frame > nf) continue;
            f32 r = one - i2f((s32)(nf - frame)) / i2f((s32)(nf - cf));
            st16(t + 0x7F4, (u16)u8_data_ratio_set(ld8(cur + 1), ld8(next + 1), r));
            st16(t + 0x7F6, (u16)u8_data_ratio_set(ld8(cur + 2), ld8(next + 2), r));
            st16(t + 0x7F8, (u16)u8_data_ratio_set(ld8(cur + 3), ld8(next + 3), r));
            found = true;
            break;
        }
        u32 endf = ld8(tbl + 0xC);
        if (found) last = ld8(t + 0x7E1);
        if (last > endf) {
            u8 st_ = ld8(t + 0x7E0);
            last = ld8(t + 0x7E1);
            st8(t + 0x7E0, (u8)(st_ + 1));
        }
        st8(t + 0x7E1, (u8)(last + 1));
        return;
    }
    case 3:
        dKy_efplight_cut(t + 0x7E8);
        st8(t + 0x7E0, 0);
        return;
    case 4:
        dKy_efplight_cut(t + 0x7E8);
        st8(t + 0x7E0, 1);
        return;
    default:
        return;
    }
}
VERIFY(0x0255BAF4, dScnKy_env_light_c_Eflight_flush_proc);

/* 0255BD4C: dScnKy_env_light_c::SetSchbit(): mSchbit +0x109D, mSchbitTimer +0x1084 */
void dScnKy_env_light_c_SetSchbit(u32 t) {
    WWHD_FUNC(0x0255BD4C, void, t);
    u32 sd = dComIfGp_get_l() + 0x5150;
    u32 info = gabi::call_ptr<u32>(ld(ld(sd) + 0x15C), sd);
    u32 start = (ld(info + 0x10) >> 16) & 0xFF;
    u32 sec = ld8(info + 0xF);
    if (start == 0) return;
    u8 bit = ld8(t + 0x109D);
    u32 timer = ld(t + 0x1084);
    if (bit == 0) st8(t + 0x109D, 0x80);
    s32 n = (s32)timer + 1;
    if (n < (s32)(sec * 30)) {
        st(t + 0x1084, (u32)n);
        return;
    }
    u8 b = ld8(t + 0x109D);
    st(t + 0x1084, 0);
    b = (u8)(b >> 1);
    st8(t + 0x109D, b != 0 ? b : 0x80);
}
VERIFY(0x0255BD4C, dScnKy_env_light_c_SetSchbit);

/* 0255BE0C: HD: the stage uses the time-of-day light presets: stage type 1..4 or 6 and not
 * one of ADMumi, Mjtower, M2tower, Ocrogh, ENDumi, M_Dra09, Hyrule room 0, M_NewD2 room 3,
 * Siren room 18, kinMB room 10, kindan rooms 2/4/13 (no stage info: FALSE) */
BOOL dKy_daylight_preset_chk_hd() {
    WWHD_FUNC(0x0255BE0C, BOOL);
    BOOL ret = FALSE;
    u32 sd = dComIfGp_get_l() + 0x5150;
    if (gabi::call_ptr<u32>(ld(ld(sd) + 0x15C), sd) == 0) return FALSE;
    sd = dComIfGp_get_l() + 0x5150;
    u32 info = gabi::call_ptr<u32>(ld(ld(sd) + 0x15C), sd);
    u32 type = (ld(info + 0xC) >> 16) & 7;
    bool special = true;
    if (stage_name_eq_l(0x1004F274) || stage_name_eq_l(0x1004F244) || stage_name_eq_l(0x1004F24C) ||
        stage_name_eq_l(0x1004F27C) || stage_name_eq_l(0x1004F274) || stage_name_eq_l(0x1004F284) ||
        stage_name_eq_l(0x1004F254)) {
    } else if (stage_name_eq_l(0x1004F28C) && (s8)ld8(0x1047E6C8) == 0) {        /* Hyrule */
    } else if (stage_name_eq_l(0x1004F25C) && (s8)ld8(0x1047E6C8) == 3) {        /* M_NewD2 */
    } else if (stage_name_eq_l(0x1004F264) && (s8)ld8(0x1047E6C8) == 0x12) {     /* Siren */
    } else if (stage_name_eq_l(0x1004F26C) && (s8)ld8(0x1047E6C8) == 0xA) {      /* kinMB */
    } else if (stage_name_eq_l(0x1004F294)) {                                      /* kindan */
        s32 room = (s8)ld8(0x1047E6C8);
        if (!(room == 2 || room == 4 || room == 0xD)) special = false;
    } else {
        special = false;
    }
    if (!special && ((type >= 1 && type <= 4) || type == 6)) ret = TRUE;
    return ret;
}
VERIFY(0x0255BE0C, dKy_daylight_preset_chk_hd);

void dScnKy_env_light_c_setDaytime(u32 t);
void dScnKy_env_light_c_CalcTevColor(u32 t);
void dScnKy_env_light_c_Sndpos(u32 t);
void dScnKy_env_light_c_Eflight_flush_proc(u32 t);
void dScnKy_env_light_c_SetSchbit(u32 t);

/* copy dst <- src through two accessor calls (GHS: env1 = acc; v = acc()[src]; env1[dst] = v) */
static inline void env_copy8_l(u32 dst, u32 src) {
    u32 e1 = envlight_l();
    u8 v = ld8(envlight_l() + src);
    st8(e1 + dst, v);
}
static inline void env_copyf_l(u32 dst, u32 src) {
    u32 e1 = envlight_l();
    f32 v = ldf(envlight_l() + src);
    stf(e1 + dst, v);
}

/* 0255C85C: dScnKy_env_light_c::exeKankyo(): mColPatMode +0x1097, mColPatModeGather +0x1098,
 * mColpatPrev/Curr +0x108C/+0x108D, gathers +0x108E/+0x108F, mColpatWeather +0x1092, mColPatBlend
 * +0xFC8, gather +0xFCC, ratios +0xFE4..+0xFF8 from the gathers +0xFFC..+0x1010. HD: after the
 * GameCube updates, chooses the HD light preset pair (101F8664 table, applied by 0272FEE0 to the
 * current scene light set 101F95D0): Hyroom blends by an HD parameter (101F4820/2, with "Demo49"
 * at full strength), else event bit 0x0408, an HD override (101F4828 / play +0x5B08), the
 * daylight stages blend day/night over hours 6-7 and 18-19. */
void dScnKy_env_light_c_exeKankyo(u32 t) {
    WWHD_FUNC(0x0255C85C, void, t);
    env_copy8_l(0x1097, 0x1098);
    f32 zero;
    bool mode;
    if (ld8(dComIfGp_get_l() + 0x5292) == 0 && ld8(envlight_l() + 0x1098) != 0) {
        if (ld8(envlight_l() + 0x1098) >= 3) {
            st8(envlight_l() + 0x1098, 0);
        } else {
            u32 e = envlight_l();
            st8(e + 0x1098, (u8)(ld8(e + 0x1098) + 1));
        }
    }
    zero = ldf(0x1004F0B0);
    mode = ld8(envlight_l() + 0x1097) != 0;
    bool resetBlend = false;
    if (mode) {
        if (ld8(envlight_l() + 0x108E) != 0xFF) {
            env_copy8_l(0x108C, 0x108E);
            if (ld8(envlight_l() + 0x1098) == 0) st8(envlight_l() + 0x108E, 0xFF);
        }
        if (ld8(envlight_l() + 0x108F) != 0xFF) {
            env_copy8_l(0x108D, 0x108F);
            if (ld8(envlight_l() + 0x1098) == 0) st8(envlight_l() + 0x108F, 0xFF);
        }
        if (!(ldf(envlight_l() + 0xFCC) < zero)) {
            env_copyf_l(0xFC8, 0xFCC);
            if (ld8(envlight_l() + 0x1098) == 0) resetBlend = true;
        }
    } else {
        u32 e1 = envlight_l();
        u8 cur = ld8(envlight_l() + 0x108D);
        if (ld8(e1 + 0x108C) == cur) {
            if (ld8(envlight_l() + 0x108E) != 0xFF) {
                env_copy8_l(0x108C, 0x108E);
                st8(envlight_l() + 0x108E, 0xFF);
            }
            if (ld8(envlight_l() + 0x108F) != 0xFF) {
                env_copy8_l(0x108D, 0x108F);
                st8(envlight_l() + 0x108F, 0xFF);
                env_copy8_l(0x1092, 0x108D);
            }
            if (!(ldf(envlight_l() + 0xFCC) < zero)) {
                env_copyf_l(0xFC8, 0xFCC);
                resetBlend = true;
            }
        }
    }
    if (resetBlend) stf(envlight_l() + 0xFCC, ldf(0x1004F0B4));
    env_copyf_l(0xFE4, 0xFFC);
    env_copyf_l(0xFE8, 0x1000);
    env_copyf_l(0xFEC, 0x1004);
    env_copyf_l(0xFF0, 0x1008);
    env_copyf_l(0xFF4, 0x100C);
    env_copyf_l(0xFF8, 0x1010);
    const f32 one = ldf(0x1004F0A4);
    for (u32 o = 0xFFC; o <= 0x1010; o += 4) stf(envlight_l() + o, one);
    dScnKy_env_light_c_setDaytime(t);
    gabi::call(0x0257D208); /* dKyw_wether_proc (squal_proc) */
    dScnKy_env_light_c_CalcTevColor(t);
    dScnKy_env_light_c_Sndpos(t);
    dScnKy_env_light_c_Eflight_flush_proc(t);
    dScnKy_env_light_c_SetSchbit(t);

    /* HD light presets */
    s32 hour = gabi::ftoi(ldf(t + 0x1020) * ldf(0x1004F1A0));
    u32 tbl = ld(0x101F8664);
    u32 g = ld(0x101F95D0);
    u32 pp = ld(g + 0x1024);
    if (ld(g + 0x1020) > 1) pp += 4;
    u32 night = ld(tbl + 0x34);
    u32 set = ld(pp);
    u32 day = ld(tbl + 0x30);
    u32 sd = dComIfGp_get_l() + 0x5150;
    gabi::call_ptr<u32>(ld(ld(sd) + 0x15C), sd);
    auto apply = [&](u32 a, u32 b, f32 f) { gabi::call(0x0272FEE0, set, a, b, f); };
    if (ld8(0x101F4829) != 0) {
        gabi::call(0x025F0840); /* mDoGph_gInf_c::calcMonotone */
        if (stage_name_eq_l(0x1004F2A8)) { /* "Hyroom" */
            u32 pa = ld(tbl + 0x3C);
            s32 v = lds16(0x101F4820);
            f32 f13 = i2f(v + 600) / ldf(0x1004F29C);
            s32 w = lds16(0x101F4822);
            u32 pb = ld(tbl + 0x44);
            f32 target = one - f13;
            if (w < 0 && v > -250) {
                stf(t + 0x10E8, zero);
            } else {
                gabi::call<BOOL>(0x0200F5C8, t + 0x10E8, target, ldf(0x1047BA08) + ldf(0x1004F2A0)); /* cLib_chaseF */
            }
            /* "Demo49" against the HD event name buffer 1047E6B8 (first assureTermination direct) */
            gabi::Local<SafeStr_l> a;
            a->str = 0x1004F2B0;
            a->vtbl = SafeString_vtbl;
            gabi::Local<SafeStr_l> b;
            b->vtbl = SafeString_vtbl;
            b->str = 0x1047E6B8;
            gabi::call(0x025638D4, a.get());
            gabi::call_ptr(ld(a->vtbl + 0x14), a.get());
            u32 bv = b->vtbl;
            u32 s1 = a->str;
            gabi::call_ptr(ld(bv + 0x14), b.get());
            u32 s2 = b->str;
            bool eq = s1 == s2;
            if (!eq) {
                for (u32 n = 0x40001; n != 0; n--) {
                    u8 c1 = ld8(s1), c2 = ld8(s2);
                    if (c1 != c2) break;
                    if (c1 == 0) {
                        eq = true;
                        break;
                    }
                    s1++;
                    s2++;
                }
            }
            if (eq && (s32)ld(0x101D6008) >= 0x262) {
                stf(t + 0x10E8, one);
                apply(pa, pb, one);
                return;
            }
            apply(pa, pb, ldf(t + 0x10E8));
            return;
        }
        apply(ld(tbl + 0x40), 0, one);
        return;
    }
    if (isEventBit_l(save_l() + 0x1178, 0x408)) {
        apply(ld(tbl + 0x48), 0, one);
        return;
    }
    if (ld8(0x101F4828) == 0 && ldf(dComIfGp_get_l() + 0x5B08) != one) {
        apply(ld(tbl + 0x38), 0, one);
        return;
    }
    if (dKy_daylight_preset_chk_hd()) {
        apply(day, 0, one);
        return;
    }
    const f32 k30 = ldf(0x1004F2A4);
    if ((u32)(hour - 6) < 2) {
        apply(night, day, (ldf(t + 0x1020) - ldf(0x1004F114)) / k30);
        return;
    }
    if ((u32)(hour - 18) < 2) {
        apply(day, night, (ldf(t + 0x1020) - ldf(0x1004F21C)) / k30);
        return;
    }
    if ((u32)(hour - 7) >= 13) {
        apply(night, 0, one);
        return;
    }
    if ((u32)(hour - 8) < 11) apply(day, 0, one);
}
VERIFY(0x0255C85C, dScnKy_env_light_c_exeKankyo);

u8 phantomship_wether();
void dice_wether_init(u32 mode, f32 weatherTime, f32 curTime);
void dice_wether_execute(u32 mode, f32 weatherTime, f32 curTime);
void dice_rain_minus();
void dScnKy_env_light_c_exeKankyo(u32 t);
static inline void dKyw_rain_set_l(s32 n) { gabi::call(0x0257E7C0, n); }

/* 0255D128: dKy_Execute (matcher: dKy_event_proc): dKy_event_proc inlined, then exeKankyo and
 * dKyw_wind_set, plus the HD sky brightness +0x103C (eased to 1, or to 0.4 from the thunder
 * flash while the weather pattern +0x1092 is set). Dice weather tables: S_wether_table 101E8CC0,
 * modes 101E8CCC/CD0/CD4/CDE, times 101E8D24/D30/D3C (pattern 4: 10.0 at 101E8CD8),
 * S_time_table 101E8CF0. mRainCount +0xA40, mThunderEff +0xAB0 (mode +0xAB4), mDiceWeather*
 * +0xFC0 (change time) / +0xFC4 / +0x1068 / +0x1093..+0x1095. */
BOOL dKy_Execute() {
    WWHD_FUNC(0x0255D128, BOOL);
    u32 env0 = envlight_l();
    bool tail8B4 = false, tail8C4 = false;
    if (stage_name_eq_l(0x1004EF54)) { /* "sea" */
        u32 r31 = 0;
        auto modeChk = [&]() { /* acc; mColPatMode == 0 -> 8C4, else 910 */
            if (ld8(envlight_l() + 0x1097) == 0) tail8C4 = true;
        };
        if (dKy_checkEventNightStop()) {
            u8 w = ld8(env0 + 0x1092);
            s32 rain = (s32)ld(env0 + 0xA40);
            if (w != 1) {
                st8(env0 + 0x1092, 1);
                st8(env0 + 0x108F, 1);
            }
            if (rain < 250) {
                st(env0 + 0xA40, (u32)(rain + 1));
                dKyw_rain_set_l(rain + 1);
                modeChk();
            } else {
                tail8B4 = true;
            }
        } else if (ld8(envlight_l() + 0x109C) != 0) {
            tail8B4 = true;
        } else {
            dComIfGp_get_l();
            if (ld8(0x101EBFD8) != 0 && gabi::call<BOOL>(0x025B5D40, save_l() + 0x71, 2, 0)) {
                if (phantomship_wether()) {
                    st(envlight_l() + 0xAB4, 1);
                    dice_rain_minus();
                    if (ld8(env0 + 0x1092) == 1) {
                        tail8B4 = true;
                    } else {
                        st8(envlight_l() + 0x1092, 1);
                        st8(envlight_l() + 0x108F, 1);
                        modeChk();
                    }
                } else {
                    f32 cur = ldf(save_l() + 0x44);
                    f32 chg = ldf(env0 + 0xFC0);
                    const f32 k180 = ldf(0x1004F10C);
                    u8 st_;
                    if (cur > chg && cur - chg < k180) {
                        st_ = ld8(env0 + 0x1094);
                        if (st_ == 0) {
                            st8(env0 + 0x1094, 1);
                            st_ = 1;
                        }
                    } else {
                        st_ = ld8(env0 + 0x1094);
                    }
                    const f32 k799 = ldf(0x1004F2B8);
                    bool chk6B4 = false;
                    switch (st_) {
                    case 0:
                        chk6B4 = true;
                        break;
                    case 1: {
                        f32 r = gabi::call<f32>(0x020198D8, k799); /* cM_rndF */
                        u8 idx = (u8)gabi::ftoi(r);
                        u8 pat = ld8(0x101E8CC0 + idx);
                        st8(env0 + 0x1095, pat);
                        st(env0 + 0x1068, 0);
                        if (pat <= 3) {
                            if (pat == 0) dice_wether_init(ld8(0x101E8CCC), ldf(0x101E8D24), cur);
                            else if (pat == 1) dice_wether_init(ld8(0x101E8CD0), ldf(0x101E8D30), cur);
                            else if (pat == 2) dice_wether_init(ld8(0x101E8CD4), ldf(0x101E8D3C), cur);
                            else dice_wether_init(ld8(0x101E8CDE), ldf(0x101E8CD8), cur);
                            u32 cnt = ld(env0 + 0x1068);
                            u8 s2 = ld8(env0 + 0x1094);
                            st(env0 + 0x1068, cnt + 1);
                            st8(env0 + 0x1094, (u8)(s2 + 1));
                        } else {
                            u8 s2 = ld8(env0 + 0x1094);
                            st(env0 + 0x1068, 1);
                            st8(env0 + 0x1094, (u8)(s2 + 1));
                        }
                        if (ld8(envlight_l() + 0x1097) != 0) tail8B4 = true;
                        break;
                    }
                    case 2: {
                        f32 wt = ldf(env0 + 0xFC4);
                        if (cur > wt && cur - wt < k180) {
                            u8 pat = ld8(env0 + 0x1095);
                            if (pat <= 3) {
                                u32 cnt = ld(env0 + 0x1068);
                                if (pat == 0) dice_wether_execute(ld8(0x101E8CCC + cnt), ldf(0x101E8D24 + cnt * 4), cur);
                                else if (pat == 1) dice_wether_execute(ld8(0x101E8CD0 + cnt), ldf(0x101E8D30 + cnt * 4), cur);
                                else if (pat == 2) dice_wether_execute(ld8(0x101E8CD4 + cnt), ldf(0x101E8D3C + cnt * 4), cur);
                                else dice_wether_execute(ld8(0x101E8CDE + cnt), ldf(0x101E8CD8), cur);
                            }
                            if (ld8(envlight_l() + 0x1097) != 0) tail8B4 = true;
                        } else {
                            chk6B4 = true;
                        }
                        break;
                    }
                    case 3: {
                        f32 r = gabi::call<f32>(0x020198D8, k799);
                        u8 idx = (u8)gabi::ftoi(r);
                        f32 t = cur + ldf(0x101E8CF0 + idx * 4);
                        const f32 k360 = ldf(0x1004F108);
                        if (!(t < k360)) t = t - k360;
                        st8(env0 + 0x1094, (u8)r31);
                        stf(env0 + 0xFC0, t);
                        chk6B4 = true;
                        break;
                    }
                    default:
                        if (ld8(envlight_l() + 0x1097) != 0) tail8B4 = true;
                        break;
                    }
                    if (chk6B4 && ld8(envlight_l() + 0x1097) != 0) tail8B4 = true;
                    if (!tail8B4) {
                        if (ld8(envlight_l() + 0x1098) != 0) {
                            tail8B4 = true;
                        } else {
                            u8 mode = ld8(env0 + 0x1093);
                            u32 w = 0;
                            bool set = false;
                            switch (mode) {
                            case 0:
                                w = 0;
                                if (ld(envlight_l() + 0xAB4) == 1) st(envlight_l() + 0xAB4, r31);
                                dice_rain_minus();
                                set = ld8(env0 + 0x1092) != w;
                                break;
                            case 1:
                                w = 1;
                                dice_rain_minus();
                                set = ld8(env0 + 0x1092) != 1;
                                break;
                            case 2: {
                                s32 rain = (s32)ld(env0 + 0xA40);
                                w = 1;
                                if (rain < 40) {
                                    st(env0 + 0xA40, (u32)(rain + 1));
                                    dKyw_rain_set_l(rain + 1);
                                } else {
                                    st(env0 + 0xA40, (u32)(rain - 1));
                                    dKyw_rain_set_l(rain - 1);
                                }
                                set = ld8(env0 + 0x1092) != w;
                                break;
                            }
                            case 5:
                                st(envlight_l() + 0xAB4, 1);
                                /* fallthrough */
                            case 3: {
                                s32 rain = (s32)ld(env0 + 0xA40);
                                w = 1;
                                if (rain < 250) {
                                    st(env0 + 0xA40, (u32)(rain + 1));
                                    dKyw_rain_set_l(rain + 1);
                                }
                                set = ld8(env0 + 0x1092) != w;
                                break;
                            }
                            case 4:
                                w = 1;
                                st(envlight_l() + 0xAB4, w);
                                dice_rain_minus();
                                set = ld8(env0 + 0x1092) != w;
                                break;
                            default:
                                JUT_ASSERT_l(0x1004F094, 0x291A, 0x1004EF60);
                                w = ld8(env0 + 0x1092);
                                set = false;
                                break;
                            }
                            if (set) {
                                st8(envlight_l() + 0x1092, (u8)w);
                                st8(envlight_l() + 0x108F, (u8)w);
                                modeChk();
                            } else {
                                tail8B4 = true;
                            }
                        }
                    }
                }
            } else {
                if (phantomship_wether()) {
                    if (ld8(envlight_l() + 0x1092) != 1) {
                        r31 = 1;
                        st8(envlight_l() + 0x1092, 1);
                        st8(envlight_l() + 0x108F, 1);
                    }
                    st(envlight_l() + 0xAB4, 1);
                    modeChk();
                } else {
                    if (ld8(envlight_l() + 0x1092) != 0) {
                        st8(envlight_l() + 0x1092, (u8)r31);
                        st8(envlight_l() + 0x108F, (u8)r31);
                    }
                    if (ld(envlight_l() + 0xAB4) == 1) st(envlight_l() + 0xAB4, r31);
                    dice_rain_minus();
                    tail8B4 = true;
                }
            }
        }
        if (tail8B4) {
            if (ld8(envlight_l() + 0x1097) == 0) tail8C4 = true;
        }
        if (tail8C4) {
            if (ld8(envlight_l() + 0x1098) == 0 && ld8(envlight_l() + 0x108F) != 0xFF) {
                u32 e1 = envlight_l();
                u8 g = ld8(envlight_l() + 0x108F);
                if (ld8(e1 + 0x108D) != g) stf(envlight_l() + 0xFCC, ldf(0x1004F0B0));
            }
        }
    }
    dScnKy_env_light_c_exeKankyo(envlight_l());
    gabi::call(0x0257D398); /* dKyw_wind_set */
    u32 th = envlight_l() + 0xAB0;
    f32 target = ldf(0x1004F0A4);
    if (ld8(envlight_l() + 0x1092) != 0) {
        u8 state = ld8(th + 1);
        f32 k = ldf(0x1004F2BC);
        target = k;
        if (state == 0 || state >= 10) {
            f32 v = k * ldf(th + 8);
            stf(envlight_l() + 0x103C, v + k);
        } else {
            f32 v = ldf(0x1004F214) * ldf(th + 8) * ldf(th + 0xC);
            stf(envlight_l() + 0x103C, v + k);
        }
    }
    f32 step = ldf(0x1004F0BC);
    u32 e = envlight_l();
    cLib_addCalc_l(e + 0x103C, target, step, step, ldf(0x1004F178));
    return TRUE;
}
VERIFY(0x0255D128, dKy_Execute);

/* 0255DA4C: dKy_IsDelete */
BOOL dKy_IsDelete(u32 p) {
    WWHD_FUNC(0x0255DA4C, BOOL, p);
    return TRUE;
}
VERIFY(0x0255DA4C, dKy_IsDelete);

void plight_init();

/* 0255DA54: dKy_Delete */
BOOL dKy_Delete(u32 p) {
    WWHD_FUNC(0x0255DA54, BOOL, p);
    plight_init();
    return TRUE;
}
VERIFY(0x0255DA54, dKy_Delete);

/* 0255DA78: dKy_setLight_init: lightStatusData[8] (10476C74, 0xE8 each) = lightStatusBase (101E8D48) */
void dKy_setLight_init() {
    WWHD_FUNC(0x0255DA78, void);
    for (u32 i = 0; i < 8; i++) {
        u32 dst = 0x10476C74 + i * 0xE8;
        for (u32 j = 0; j < 0xE8; j += 4) st(dst + j, ld(0x101E8D48 + j));
    }
}
VERIFY(0x0255DA78, dKy_setLight_init);

}  // namespace d_kankyo_cpp
