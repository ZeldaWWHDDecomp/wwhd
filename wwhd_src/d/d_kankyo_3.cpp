/* d_kankyo: environment / lighting core, part 3 of 6 (02557228..0255A2B8), WWHD.
 * Sun position, the HD room light direction and light position updates, the palette
 * selection (setLight_palno_get) and setLight. See d_kankyo.cpp for the TU. */
#include "d_kankyo_local.h"

namespace d_kankyo_cpp {

static inline f32 sin_l(f32 x) { return gabi::call<f32>(0x028F43F8, x); }
static inline f32 cos_l(f32 x) { return gabi::call<f32>(0x028F4BE0, x); }
static inline f32 cLib_addCalc_l(u32 p, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, p, target, scale, maxStep, minStep);
}

/* sead::SafeString temporaries {const char*, vtable} with this TU's SafeString vtable 1004EF7C
 * (+0x14: 025638D4, the empty assureTermination); operator== against the start stage name
 * (play+0x5134): two vcalls on the literal, one on the stage name, then a byte compare of at
 * most 0x40001 characters (equal pointers compare equal). */
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

/* 02557228: dScnKy_env_light_c::setSunpos(): mCurTime +0x1020, mSunPos +0xB38, mMoonPos +0xB44.
 * HD: the sun/moon positions are also kept while an event runs in "sea" room 44 with the camera
 * above 3000 (the sun then at 165 degrees), and the sun height factor +0x1038 (-cos * 0.9,
 * at least 0.25) is new. */
void dScnKy_env_light_c_setSunpos(u32 t) {
    WWHD_FUNC(0x02557228, void, t);
    u32 cam = ld(dComIfGp_get_l() + 0x5AF8);
    f32 ang, x, y, z;
    const f32 r80000 = ldf(0x1004F120);
    if (ldf(envlight_l() + 0x1020) > ldf(0x1004F0FC)) {
        f32 cur = ldf(envlight_l() + 0x1020);
        ang = (cur - ldf(0x1004F0FC)) * ldf(0x1004F11C);
    } else {
        f32 cur = ldf(envlight_l() + 0x1020);
        ang = (cur + ldf(0x1004F118)) * ldf(0x1004F11C);
    }
    x = sin_l(ang) * r80000;
    y = cos_l(ang) * r80000;
    f32 m48000 = ldf(0x1004F124);
    z = cos_l(ang) * m48000;
    bool set = ld8(dComIfGp_get_l() + 0x5292) == 0; /* event not running */
    if (!set && ld8(envlight_l() + 0x109C) != 0) set = true; /* mInitAnimTimer */
    if (set) {
        stf(t + 0xB38, ldf(cam + 0xDC) + x);
        stf(t + 0xB3C, ldf(cam + 0xE0) - y);
        stf(t + 0xB40, ldf(cam + 0xE4) + z);
        stf(t + 0xB44, ldf(cam + 0xDC) - x);
        stf(t + 0xB48, ldf(cam + 0xE0) + y);
        stf(t + 0xB4C, ldf(cam + 0xE4) - z);
    } else {
        bool special = false;
        if (stage_name_eq_l(0x1004F13C)) { /* "sea" */
            if (ld8(dComIfGp_get_l() + 0x5292) != 0 && (s8)ld8(0x1047E6C8) == 0x2C && ldf(cam + 0xE0) > ldf(0x1004F128))
                special = true;
        }
        if (special) {
            f32 a2 = ldf(0x1004F130);
            f32 deg = ldf(0x1004F12C);
            x = sin_l(a2) * r80000;
            y = cos_l(a2) * r80000;
            f32 c = cos_l(a2);
            stf(t + 0xB38, ldf(cam + 0xDC) + x);
            stf(t + 0xB3C, ldf(cam + 0xE0) - y);
            f32 cz = ldf(cam + 0xE4);
            ang = deg * ldf(0x1004F11C);
            stf(t + 0xB40, gabi::fmadds(c, m48000, cz));
        }
    }
    u32 env = envlight_l();
    f32 c = cos_l(ang);
    f32 k = ldf(0x1004F134);
    f32 lo = ldf(0x1004F138);
    stf(env + 0x1038, -c * k);
    if (ldf(envlight_l() + 0x1038) < lo) stf(envlight_l() + 0x1038, lo);
}
VERIFY(0x02557228, dScnKy_env_light_c_setSunpos);

/* 02557614: HD: the room light of the current room (or of the room in +0x10A3 while the blend
 * +0x1048 is above 1): a direction from the per-room angle tables +0x1108 / +0x1188 (s16[64])
 * written to the point light record +0x10EC; NULL when the room has no angles or in "M2tower". */
u32 dKy_room_light_get_hd() {
    WWHD_FUNC(0x02557614, u32);
    u32 env0 = envlight_l();
    u32 room = ld8(0x1047E6C8);
    u32 ret = 0;
    if (ld8(envlight_l() + 0x10A3) != 0xFF && ldf(envlight_l() + 0x1048) > ldf(0x1004F0A4)) {
        if (ld8(envlight_l() + 0x10A3) == room) {
            st8(envlight_l() + 0x10A3, 0xFF);
        } else {
            room = ld8(envlight_l() + 0x10A3);
        }
    }
    s16 a = lds16(env0 + 0x1108 + room * 2), b = lds16(env0 + 0x1188 + room * 2);
    if ((a | b) != 0) {
        const u32 tbl = 0x104A44F8; /* {sin, cos} per 8 angle units */
        f32 v0 = ldf(tbl + ((u16)a >> 3) * 8 + 4) * ldf(tbl + ((u16)b >> 3) * 8);
        stf(env0 + 0x10EC, v0);
        f32 k = ldf(0x1004F140);
        f32 v1 = ldf(tbl + (ld16(env0 + 0x1108 + room * 2) >> 3) * 8);
        stf(env0 + 0x10F0, v1);
        u32 ia = ld16(env0 + 0x1108 + room * 2) >> 3, ib = ld16(env0 + 0x1188 + room * 2) >> 3;
        f32 v2 = ldf(tbl + ia * 8 + 4) * ldf(tbl + ib * 8 + 4);
        stf(env0 + 0x10EC, v0 * k);
        stf(env0 + 0x10F0, v1 * k);
        ret = env0 + 0x10EC;
        stf(env0 + 0x10F4, v2 * k);
    }
    if (stage_name_eq_l(0x1004F144)) ret = 0; /* "M2tower" */
    return ret;
}
VERIFY(0x02557614, dKy_room_light_get_hd);

/* 02557834: HD: light position (+0xB20) from the time of day with a day/night fade (+0x1030
 * towards +0x1034, side flag +0x10A6), and the room light blend (+0x1044 / +0x1048) by the
 * player's state (+0xF8 == 3) for stage types 1, 3, 4, 6 */
void dScnKy_env_light_c_setLightPos_hd(u32 t) {
    WWHD_FUNC(0x02557834, void, t);
    u32 cam = ld(dComIfGp_get_l() + 0x5AF8);
    u32 dir = gabi::call<u32>(0x02557614, t);
    f32 ang;
    if (ldf(envlight_l() + 0x1020) > ldf(0x1004F0FC)) {
        f32 cur = ldf(envlight_l() + 0x1020);
        ang = (cur - ldf(0x1004F0FC)) * ldf(0x1004F11C);
    } else {
        f32 cur = ldf(envlight_l() + 0x1020);
        ang = (cur + ldf(0x1004F118)) * ldf(0x1004F11C);
    }
    f32 r80000 = ldf(0x1004F120);
    f32 x = sin_l(ang) * r80000;
    f32 y = cos_l(ang) * r80000;
    f32 c = cos_l(ang);
    const f32 zero = ldf(0x1004F0B0);
    f32 m48000 = ldf(0x1004F124);
    const f32 one = ldf(0x1004F0A4);
    f32 z = c * m48000;
    f32 f28 = ldf(0x1004F0BC);
    if (dir == 0) {
        f32 cur = ldf(t + 0x1020);
        if (cur > ldf(0x1004F14C) && cur < ldf(0x1004F150)) {
            bool alt = true;
            if (!(ldf(envlight_l() + 0x1020) < ldf(0x1004F154))) {
                if (!(ldf(envlight_l() + 0x1020) > ldf(0x1004F104))) alt = false;
            }
            if (alt) y = cos_l(ldf(0x1004F158)) * r80000;
            if (ld8(envlight_l() + 0x10A6) != 0) {
                st8(envlight_l() + 0x10A6, 0);
                stf(envlight_l() + 0x1034, one);
            }
            if (!(ldf(envlight_l() + 0x1034) > zero)) {
                stf(t + 0xB20, ldf(cam + 0xDC) + x);
                stf(t + 0xB24, ldf(cam + 0xE0) - y);
                stf(t + 0xB28, ldf(cam + 0xE4) + z);
            }
        } else {
            bool alt = true;
            if (!(ldf(envlight_l() + 0x1020) < ldf(0x1004F15C))) {
                if (!(ldf(envlight_l() + 0x1020) > ldf(0x1004F160))) alt = false;
            }
            if (alt) y = cos_l(ldf(0x1004F164)) * r80000;
            if (ld8(envlight_l() + 0x10A6) == 0) {
                st8(envlight_l() + 0x10A6, 1);
                stf(envlight_l() + 0x1034, one);
            }
            if (!(ldf(envlight_l() + 0x1034) > zero)) {
                stf(t + 0xB20, ldf(cam + 0xDC) - x);
                stf(t + 0xB24, ldf(cam + 0xE0) + y);
                stf(t + 0xB28, ldf(cam + 0xE4) - z);
            }
        }
        f32 target = ldf(envlight_l() + 0x1034);
        cLib_addCalc_l(envlight_l() + 0x1030, target, ldf(0x1004F168), f28, f28);
        if (!(ldf(envlight_l() + 0x1030) < one)) stf(envlight_l() + 0x1034, zero);
    }
    u32 player = ld(dComIfGp_get_l() + 0x5B2C);
    u32 sd = dComIfGp_get_l() + 0x5150;
    u32 info = gabi::call_ptr<u32>(ld(ld(sd) + 0x15C), sd);
    u32 type = (ld(info + 0xC) >> 16) & 7;
    if (!(type == 1 || type == 3 || type == 4 || type == 6)) return;
    if (player != 0) {
        if (ld16(player + 0xF8) == 3) {
            if (!(ldf(envlight_l() + 0x1048) > zero)) stf(envlight_l() + 0x1048, one);
            if (!(ldf(envlight_l() + 0x1044) < ldf(0x1004F16C))) {
                f32 two = ldf(0x1004F170);
                stf(envlight_l() + 0x1048, two);
            }
        } else {
            if (!(ldf(envlight_l() + 0x1048) < ldf(0x1004F170))) {
                if (!(ldf(envlight_l() + 0x1044) > f28)) stf(envlight_l() + 0x1048, zero);
            }
        }
    }
    if (ldf(envlight_l() + 0x1048) > one) {
        f32 a = ldf(0x1004F174), b = ldf(0x1004F178), s = ldf(0x1004F168);
        cLib_addCalc_l(envlight_l() + 0x1044, zero, s, a, b);
    } else {
        f32 target = ldf(envlight_l() + 0x1048);
        f32 a = ldf(0x1004F17C), s = ldf(0x1004F138);
        cLib_addCalc_l(envlight_l() + 0x1044, target, s, a, f28);
    }
}
VERIFY(0x02557834, dScnKy_env_light_c_setLightPos_hd);

/* 02557CF4: dScnKy_env_light_c::SetBaseLight() (unnamed by the matcher), HD-changed: the base
 * light (mBaseLightInfluence +0x14) from the room light, else from the
 * light position (+0xB20; fixed in "M2tower"); in "sea" the brightness +0x1040 eases to 1 in
 * rooms 0/13/26 or while +0x5CEA of the play object is 3, else to 0. */
void dScnKy_env_light_c_SetBaseLight(u32 t) {
    WWHD_FUNC(0x02557CF4, void, t);
    u32 env = envlight_l();
    u32 dir = gabi::call<u32>(0x02557614, t);
    f32 zero = ldf(0x1004F0B0);
    bool sea;
    if (dir != 0) {
        stf(t + 0x14, ldf(dir + 0));
        stf(t + 0x18, ldf(dir + 4));
        f32 dz = ldf(dir + 8);
        st16(t + 0x20, 0);
        stf(t + 0x1C, dz);
        st16(t + 0x22, 0);
        st16(t + 0x24, 0);
        st16(t + 0x26, 0);
        stf(t + 0x28, ldf(dir + 0xC) * ldf(0x1004F0C4));
        stf(t + 0x2C, (f32)ld8(dir + 0x1B));
        sea = stage_name_eq_l(0x1004F194);
    } else {
        if (stage_name_eq_l(0x1004F18C)) { /* "M2tower" */
            f32 a = ldf(0x1004F180), b = ldf(0x1004F184);
            stf(env + 0xB20, a);
            stf(env + 0xB24, b);
            stf(env + 0xB28, a);
            stf(t + 0x14, a);
            f32 y = ldf(env + 0xB24);
            stf(t + 0x28, zero);
            stf(t + 0x18, y);
            st16(t + 0x20, 0xFF);
            st16(t + 0x22, 0xFF);
            st16(t + 0x24, 0xFF);
            st16(t + 0x26, 0xFF);
            stf(t + 0x1C, a);
        } else {
            f32 x = ldf(env + 0xB20), z = ldf(env + 0xB28);
            stf(t + 0x14, x);
            f32 y = ldf(env + 0xB24);
            stf(t + 0x28, zero);
            stf(t + 0x18, y);
            st16(t + 0x20, 0xFF);
            stf(t + 0x1C, z);
            st16(t + 0x22, 0xFF);
            st16(t + 0x24, 0xFF);
            st16(t + 0x26, 0xFF);
        }
        stf(t + 0x2C, zero);
        sea = stage_name_eq_l(0x1004F194);
    }
    if (!sea) return;
    bool lit = ld8(dComIfGp_get_l() + 0x5CEA) == 3;
    if (!lit) {
        s32 room = (s8)ld8(0x1047E6C8);
        lit = room == 0 || room == 0xD || room == 0x1A;
    }
    f32 target = zero;
    if (lit) target = ldf(0x1004F0A4);
    u32 e = envlight_l();
    cLib_addCalc_l(e + 0x1040, target, ldf(0x1004F188), ldf(0x1004F0BC), ldf(0x1004F0D0));
}
VERIFY(0x02557CF4, dScnKy_env_light_c_SetBaseLight);

/* palette index by selector 0..5 (no store for other values) */
static inline void pal_pick_l(u32 dst, u32 psel, u32 sel) {
    if (sel <= 5) st8(dst, ld8(psel + sel));
}

/* 025580FC: dScnKy_env_light_c::setLight_palno_get(u8* envrSel0, u8* envrSel1, u8* pSelIdx0,
 * u8* pSelIdx1, u8* palIdx0A, u8* palIdx0B, u8* palIdx1A, u8* palIdx1B, f32* blendPalAB,
 * int* pSelPalIdx0, int* pSelPalIdx1, f32* blendPal01, u8* initTimer); the last five on the stack.
 * HD layout: mpPselectInfo +4, mpEnvrInfo +8, mpSchejule +0x10, mCurTime +0x1020, mColPatBlend
 * +0xFC8, mColPatMode +0x1097, mColPatModeGather +0x1098. HD: the slow 300-frame blend for a
 * pselect change in "sea" applies only when the pselect index differs; "ma2room" blends in 15
 * frames; a missing pselect entry skips the palette stores. */
void dScnKy_env_light_c_setLight_palno_get(u32 t, u32 envrSel0, u32 envrSel1, u32 pSelIdx0, u32 pSelIdx1,
                                           u32 palIdx0A, u32 palIdx0B, u32 palIdx1A) {
    WWHD_FUNC(0x025580FC, void, t, envrSel0, envrSel1, pSelIdx0, pSelIdx1, palIdx0A, palIdx0B, palIdx1A);
    const u32 sp = gabi::cpu->r[1];
    const u32 palIdx1B = ld(sp + 8), blendPalAB = ld(sp + 0xC), pSelPalIdx0 = ld(sp + 0x10),
              pSelPalIdx1 = ld(sp + 0x14), blendPal01 = ld(sp + 0x18), initTimer = ld(sp + 0x1C);
    const f32 one = ldf(0x1004F0A4);
    u8 timer = ld8(initTimer);
    if (timer != 0) {
        u8 t1 = (u8)(timer + 1);
        st8(initTimer, t1 > 20 ? 0 : t1);
        if (ld8(envlight_l() + 0x1097) == 0 && ld8(envlight_l() + 0x1098) == 0) {
            f32 b = ldf(envlight_l() + 0xFC8);
            stf(blendPal01, b);
            if (!(b < one)) {
                st8(envrSel0, ld8(envrSel1));
                st8(pSelIdx0, ld8(pSelIdx1));
            }
        }
    }
    u32 sched = ld(t + 0x10);
    f32 cur = ldf(t + 0x1020);
    u32 e = sched;
    for (s32 i = 0; i < 11; i++, e += 0xC) {
        if (cur < ldf(e + 0)) continue;
        if (cur > ldf(e + 4)) continue;
        u32 off = i * 0xC;
        st(pSelPalIdx0, ld8(sched + off + 8));
        st(pSelPalIdx1, ld8(ld(t + 0x10) + off + 9));
        u32 sc = ld(t + 0x10);
        f32 tEnd = ldf(sc + off), tBegin = ldf(sc + off + 4);
        f32 d = tBegin - tEnd;
        const f32 zero = ldf(0x1004F0B0);
        f32 c = ldf(t + 0x1020);
        f32 pct = one;
        if (d != zero) {
            f32 r = one - (tBegin - c) / d;
            if (r < one) pct = r;
        }
        stf(blendPalAB, pct);
        u32 env = envlight_l();
        u32 sel = ld8(pSelIdx0);
        u32 envr = ld(env + 8) + ld8(envrSel0) * 8;
        u32 psel;
        if (sel > 7) {
            JUT_ASSERT_l(0x1004F1B4, 0xA99, 0x1004F1B0);
        } else {
            env = envlight_l();
            psel = ld(env + 4) + ld8(envr + sel) * 0xC;
            if (psel != 0) {
                pal_pick_l(palIdx0A, psel, ld(pSelPalIdx0));
                pal_pick_l(palIdx0B, psel, ld(pSelPalIdx1));
            }
        }
        env = envlight_l();
        u32 sel1 = ld8(pSelIdx1);
        envr = ld(env + 8) + ld8(envrSel1) * 8;
        if (sel1 > 7) {
            JUT_ASSERT_l(0x1004F1B4, 0xB3F, 0x1004F1B0);
            return;
        }
        env = envlight_l();
        psel = ld(env + 4) + ld8(envr + sel1) * 0xC;
        if (psel == 0) return;
        if (ld8(envrSel0) != ld8(envrSel1) || ld8(pSelIdx0) != ld8(pSelIdx1)) {
            const f32 rate = ldf(0x1004F198);
            if (ldf(psel + 8) < rate) stf(psel + 8, rate);
            if (ld8(envlight_l() + 0x1097) == 0) {
                bool done;
                bool slow = false;
                if (stage_name_eq_l(0x1004F1AC) && ld8(pSelIdx0) != ld8(pSelIdx1)) slow = true; /* "sea" */
                if (slow) {
                    f32 b = ldf(blendPal01) + ldf(0x1004F19C);
                    stf(blendPal01, b);
                    done = !(b < one);
                } else if (!(ldf(psel + 8) > zero)) {
                    done = !(ldf(blendPal01) < one);
                } else if (stage_name_eq_l(0x1004F1A4)) { /* "ma2room" */
                    f32 b = ldf(blendPal01) + ldf(0x1004F1A0);
                    stf(blendPal01, b);
                    done = !(b < one);
                } else {
                    f32 step = rate / ldf(psel + 8);
                    f32 b = ldf(blendPal01) + step;
                    stf(blendPal01, b);
                    done = !(b < one);
                }
                if (done) {
                    st8(envrSel0, ld8(envrSel1));
                    st8(pSelIdx0, ld8(pSelIdx1));
                    stf(blendPal01, one);
                }
            }
        }
        pal_pick_l(palIdx1A, psel, ld(pSelPalIdx0));
        pal_pick_l(palIdx1B, psel, ld(pSelPalIdx1));
        return;
    }
}
VERIFY(0x025580FC, dScnKy_env_light_c_setLight_palno_get);

s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add, f32 blend01);
f32 float_kankyo_color_ratio_set(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7);
f32 float_kankyo_color_ratio_set_noratio(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7);

/* setLight: the four blended palette (or vrbox) entries palIdx0A/0B/1A/1B as passed to
 * kankyo_color_ratio_set (a, b, blendAB, c, d) */
struct Pal4_l {
    u32 a, b, c, d;
};

/* K0 += (C0 - K0) * fade (+0x1030), per u8 component, for 3 components (HD: fade towards C0) */
static inline void fade_k0_l(u32 t, u32 c0, u32 k0, f32 zero) {
    if (!(ldf(envlight_l() + 0x1030) > zero)) return;
    f32 prev = (f32)ld8(t + k0);
    for (u32 i = 0; i < 3; i++) {
        u32 env = envlight_l();
        f32 c = (f32)ld8(t + c0 + i);
        f32 v = gabi::fmadds(c - prev, ldf(env + 0x1030), prev);
        if (i < 2) prev = (f32)ld8(t + k0 + i + 1);
        st8(t + k0 + i, (u8)gabi::ftoi(v));
    }
}

/* 02558C40: dScnKy_env_light_c::setLight(), HD (GameCube 801912EC, much changed): palettes of
 * 0x6C bytes (u8 colours, floats from +0x24), env colours stored as u8 RGBA (actor C0 +0xB64 ..
 * vrbox +0xB90..+0xBA3), HD float light parameters +0x10A8..+0x10E4, fog z +0xFD0/+0xFD4.
 * HD: a fixed palette override (+0x10A2 != 0xFF copies 101E8E30 over palette 0 and uses it for
 * all four), a day/night fade of K0 towards C0 (+0x1030), the fog end pushed out by +0x1040,
 * and in "kenroom"/"ShipD" the vrbox kumo colour is the fog colour. */
void dScnKy_env_light_c_setLight(u32 t) {
    WWHD_FUNC(0x02558C40, void, t);
    u32 initTimer = envlight_l() + 0x109C;
    if (ld8(envlight_l() + 0x10A2) != 0xFF) {
        u32 dst = ld(envlight_l() + 0);
        for (u32 i = 0; i < 0x6C; i += 4) st(dst + i, ld(0x101E8E30 + i));
    }
    u32 envrPrev = envlight_l() + 0x1090;
    u32 envrCurr = envlight_l() + 0x1091;
    u32 colPrev = envlight_l() + 0x108C;
    u32 colCurr = envlight_l() + 0x108D;
    u32 blend01 = envlight_l() + 0xFC8;
    gabi::Local<be<f32>> blendAB;
    gabi::Local<be<u8>> pal0A, pal0B, pal1A, pal1B;
    gabi::Local<be<s32>> psel0, psel1;
    gabi::call(0x025580FC, t, envrPrev, envrCurr, colPrev, colCurr, pal0A.get(), pal0B.get(), pal1A.get(), pal1B.get(),
               blendAB.get(), psel0.get(), psel1.get(), blend01, initTimer);
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
    /* u8 colour component: blend (+0xFC8) after one accessor call, the ratio after the next */
    auto kc = [&](const Pal4_l& q, u32 off, u32 ratioOff, s32 addOff, u32 dst) {
        f32 blend = ldf(envlight_l() + 0xFC8);
        f32 ratio = ldf(envlight_l() + ratioOff);
        s32 add = addOff ? lds16(t + addOff) : 0;
        s16 r = kankyo_color_ratio_set(ld8(q.a + off), ld8(q.b + off), ldf(bAB), ld8(q.c + off), ld8(q.d + off), blend, add, ratio);
        st8(t + dst, (u8)r);
    };
    /* actor C0: ratio = mActColRatio^2 (two accessor calls) */
    for (u32 i = 0; i < 3; i++) {
        f32 blend = ldf(envlight_l() + 0xFC8);
        u32 e1 = envlight_l();
        u32 e2 = envlight_l();
        f32 ratio = ldf(e1 + 0xFE8) * ldf(e2 + 0xFE8);
        s16 r = kankyo_color_ratio_set(ld8(p.a + i), ld8(p.b + i), ldf(bAB), ld8(p.c + i), ld8(p.d + i), blend, 0, ratio);
        st8(t + 0xB64 + i, (u8)r);
    }
    for (u32 i = 0; i < 3; i++) kc(p, 3 + i, 0xFE8, 0, 0xB68 + i); /* actor K0 */
    const f32 zero = ldf(0x1004F0B0);
    fade_k0_l(t, 0xB64, 0xB68, zero);
    for (u32 i = 0; i < 3; i++) kc(p, 6 + i, 0xFEC, 0, 0xB6C + i); /* BG0 C0 */
    for (u32 i = 0; i < 3; i++) kc(p, 9 + i, 0xFEC, 0, 0xB70 + i); /* BG0 K0 */
    fade_k0_l(t, 0xB6C, 0xB70, zero);
    for (u32 j = 0; j < 6; j++) /* BG1..BG3 C0/K0 */
        for (u32 i = 0; i < 3; i++) kc(p, 0xC + j * 3 + i, 0xFEC, 0, 0xB74 + j * 4 + i);
    for (u32 i = 0; i < 3; i++) kc(p, 0x1E + i, 0xFF0, 0xFA8 + i * 2, 0xB8C + i); /* fog + mAddColFog */
    {
        /* HD: fog alpha from the palettes' float +0x5C (x 255) */
        const f32 k255 = ldf(0x1004F104);
        f32 blend = ldf(envlight_l() + 0xFC8);
        f32 ratio = ldf(envlight_l() + 0xFF0);
        u8 a = (u8)gabi::ftoi(ldf(p.a + 0x5C) * k255), b = (u8)gabi::ftoi(ldf(p.b + 0x5C) * k255),
           c = (u8)gabi::ftoi(ldf(p.c + 0x5C) * k255), d = (u8)gabi::ftoi(ldf(p.d + 0x5C) * k255);
        s16 r = kankyo_color_ratio_set(a, b, ldf(bAB), c, d, blend, 0, ratio);
        st8(t + 0xB8F, (u8)r);
    }
    {
        /* fog start / end z (+0xFD0 / +0xFD4), without the all-colour ratio */
        f32 blend = ldf(envlight_l() + 0xFC8);
        f32 sz = ldf(envlight_l() + 0xFD8);
        f32 ratio = ldf(envlight_l() + 0xFE0);
        f32 v = float_kankyo_color_ratio_set_noratio(ldf(p.a + 0x24), ldf(p.b + 0x24), ldf(bAB), ldf(p.c + 0x24), ldf(p.d + 0x24), blend, sz, ratio);
        stf(t + 0xFD0, v);
        u32 e1 = envlight_l();
        u32 e2 = envlight_l();
        u32 e3 = envlight_l();
        f32 ez = float_kankyo_color_ratio_set_noratio(ldf(p.a + 0x28), ldf(p.b + 0x28), ldf(bAB), ldf(p.c + 0x28), ldf(p.d + 0x28),
                                                      ldf(e1 + 0xFC8), ldf(e2 + 0xFDC), ldf(e3 + 0xFE0));
        stf(t + 0xFD4, ez);
        u32 e = envlight_l();
        ez = gabi::fmadds(ldf(0x1004F1C4), ldf(e + 0x1040), ez);
        f32 szv = ldf(t + 0xFD0);
        stf(t + 0xFD4, ez);
        if (szv > ez) stf(t + 0xFD0, ez);
    }
    for (u32 i = 0; i < 3; i++) kc(p, 0x2C + i, 0xFEC, 0xF68 + i * 2, 0xBA4 + i); /* HD colour + mBgAddColAmb */
    auto fk = [&](u32 off, u32 dst) {
        u32 env = envlight_l();
        f32 v = float_kankyo_color_ratio_set(ldf(p.a + off), ldf(p.b + off), ldf(bAB), ldf(p.c + off), ldf(p.d + off),
                                             ldf(env + 0xFC8), zero, zero);
        stf(t + dst, v);
    };
    fk(0x58, 0x10D0);
    for (u32 i = 0; i < 5; i++) fk(0x30 + i * 4, 0x10A8 + i * 4);
    {
        /* +0x44: scaled down by +0x9F4 (factor 0.5, or 0.2 while colpat prev/curr +0x108C/+0x108D is set) */
        f32 b = ldf(p.b + 0x44);
        const f32 one = ldf(0x1004F0A4);
        f32 a = ldf(p.a + 0x44), d = ldf(p.d + 0x44), c = ldf(p.c + 0x44);
        if (ldf(envlight_l() + 0x9F4) != zero) {
            f32 k = ldf(0x1004F1C8);
            if (ld8(envlight_l() + 0x108C) == 0) {
                f32 e1 = ldf(envlight_l() + 0x9F4);
                f32 aa = ldf(p.a + 0x44);
                f32 fa = gabi::fnmsubs(k, e1, one);
                f32 e2 = ldf(envlight_l() + 0x9F4);
                f32 fb = gabi::fnmsubs(k, e2, one);
                a = aa * fa;
                b = ldf(p.b + 0x44) * fb;
            } else {
                f32 e1 = ldf(envlight_l() + 0x9F4);
                f32 aa = ldf(p.a + 0x44);
                f32 fa = gabi::fnmsubs(ldf(0x1004F1CC), e1, one);
                f32 e2 = ldf(envlight_l() + 0x9F4);
                f32 fb = gabi::fnmsubs(ldf(0x1004F1CC), e2, one);
                a = aa * fa;
                b = ldf(p.b + 0x44) * fb;
            }
            if (ld8(envlight_l() + 0x108D) != 0) {
                f32 e1 = ldf(envlight_l() + 0x9F4);
                c = ldf(p.c + 0x44) * gabi::fnmsubs(ldf(0x1004F1CC), e1, one);
                f32 e2 = ldf(envlight_l() + 0x9F4);
                d = ldf(p.d + 0x44) * gabi::fnmsubs(ldf(0x1004F1CC), e2, one);
            } else {
                f32 e1 = ldf(envlight_l() + 0x9F4);
                c = ldf(p.c + 0x44) * gabi::fnmsubs(k, e1, one);
                f32 e2 = ldf(envlight_l() + 0x9F4);
                d = ldf(p.d + 0x44) * gabi::fnmsubs(k, e2, one);
            }
        }
        u32 env = envlight_l();
        stf(t + 0x10BC, float_kankyo_color_ratio_set(a, b, ldf(bAB), c, d, ldf(env + 0xFC8), zero, zero));
    }
    for (u32 i = 0; i < 4; i++) fk(0x48 + i * 4, 0x10C0 + i * 4);
    if (ldf(envlight_l() + 0x1030) > zero) {
        f32 prev = ldf(t + 0x10AC);
        u32 env = envlight_l();
        f32 v = gabi::fmadds(ldf(t + 0x10A8) - prev, ldf(env + 0x1030), prev);
        prev = ldf(t + 0x10B4);
        stf(t + 0x10AC, v);
        env = envlight_l();
        v = gabi::fmadds(ldf(t + 0x10B0) - prev, ldf(env + 0x1030), prev);
        stf(t + 0x10B4, v);
    }
    /* vrbox entries (0x38 bytes, mpVrboxInfo +0xC) by the palettes' +0x21 */
    {
        u32 ib = ld8(p.b + 0x21), ia = ld8(p.a + 0x21), ic = ld8(p.c + 0x21), id = ld8(p.d + 0x21);
        p.a = ld(envlight_l() + 0xC) + ia * 0x38;
        p.b = ld(envlight_l() + 0xC) + ib * 0x38;
        p.c = ld(envlight_l() + 0xC) + ic * 0x38;
        p.d = ld(envlight_l() + 0xC) + id * 0x38;
    }
    for (u32 i = 0; i < 3; i++) kc(p, 0x18 + i, 0xFF4, 0xFB0 + i * 2, 0xB90 + i); /* sky + mVrboxAddColSky0 */
    st8(t + 0xB93, 0xFF);
    for (u32 i = 0; i < 3; i++) kc(p, 0x1B + i, 0xFF4, 0xFB0 + i * 2, 0xB94 + i); /* uso umi */
    {
        const f32 k255 = ldf(0x1004F104);
        f32 blend = ldf(envlight_l() + 0xFC8);
        f32 ratio = ldf(envlight_l() + 0xFF4);
        u8 a = (u8)gabi::ftoi(ldf(p.a + 0x34) * k255), b = (u8)gabi::ftoi(ldf(p.b + 0x34) * k255),
           c = (u8)gabi::ftoi(ldf(p.c + 0x34) * k255), d = (u8)gabi::ftoi(ldf(p.d + 0x34) * k255);
        s16 r = kankyo_color_ratio_set(a, b, ldf(bAB), c, d, blend, lds16(t + 0xFB4), ratio);
        st8(t + 0xB97, (u8)r);
    }
    for (u32 i = 0; i < 3; i++) kc(p, 0x10 + i, 0xFF8, 0xFB0 + i * 2, 0xB98 + i); /* kumo */
    {
        u32 env = envlight_l();
        f32 one = ldf(0x1004F0A4);
        s16 r = kankyo_color_ratio_set(ld8(p.a + 0x13), ld8(p.b + 0x13), ldf(bAB), ld8(p.c + 0x13), ld8(p.d + 0x13), ldf(env + 0xFC8), 0, one);
        st8(t + 0xB9B, (u8)r);
    }
    for (u32 i = 0; i < 3; i++) kc(p, 0x14 + i, 0xFF8, 0xFB0 + i * 2, 0xB9C + i); /* kumo center */
    for (u32 i = 0; i < 3; i++) kc(p, 0x1E + i, 0xFF4, 0xFB8 + i * 2, 0xBA0 + i); /* kasumi mae + mVrboxAddColKasumi */
    st8(t + 0xBA3, 0xFF);
    fk(0x2C, 0x10D4);
    fk(0x24, 0x10DC);
    fk(0x28, 0x10E0);
    fk(0x30, 0x10E4);
    if (stage_name_eq_l(0x1004F1D0) || stage_name_eq_l(0x1004F1D8)) { /* "kenroom", "ShipD" */
        u8 r = ld8(t + 0xB8C), g = ld8(t + 0xB8D);
        st8(t + 0xB94, r);
        u8 b = ld8(t + 0xB8E);
        st8(t + 0xB95, g);
        st8(t + 0xB96, b);
    }
}
VERIFY(0x02558C40, dScnKy_env_light_c_setLight);

}  // namespace d_kankyo_cpp
