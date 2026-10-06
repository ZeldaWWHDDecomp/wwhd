/* d_kankyo: environment / lighting core (dKy_*, dScnKy_env_light_c), part 1 of 6, WWHD.
 *
 * Translation unit 02554288..025638D4 (one TU), from the image: d_jnt_hit ends with its __sinit
 * (025541F4); d_kankyo starts with dKy_get_dayofweek (02554288); its __sinit is 02563720
 * (header statics 10475A4C.., kankyo statics 104773B8..104773BB), followed by the per-TU inline
 * copies (LIGHT_INFLUENCE constructor 025637D4, constructors 02563814 / 02563840, destructors
 * 0256386C / 02563880, an empty virtual 025638D4); d_kankyo_data starts at 025638D8.
 * The HD function order differs from the GameCube object file.
 * Parts: d_kankyo.cpp 02554288..025560C0 (day change, addcol setters, the env light constructor
 * and accessor, colour ratio helpers), d_kankyo_2.cpp 025560C0..02557228, d_kankyo_3.cpp
 * 02557228..0255A2B8, d_kankyo_4.cpp 0255A2B8..0255DAC0, d_kankyo_5.cpp 0255DAC0..02560E10,
 * d_kankyo_6.cpp 02560E10..025638D8. */
#include "d_kankyo_local.h"

namespace d_kankyo_cpp {

static inline void dLetter_autoStock_l(u32 flag) { gabi::call(0x02586E94, flag); }
static inline void dLetter_delivery_l(u32 flag) { gabi::call(0x02586E58, flag); }
static inline BOOL isReserve_l(u32 p, u32 no) { return gabi::call<BOOL>(0x025B7840, p, no); }        /* dSv_player_get_bag_item_c::isReserve */
static inline BOOL checkReserveItem_l(u32 p, u32 no) { return gabi::call<BOOL>(0x025B7570, p, no); } /* dSv_player_bag_item_c::checkReserveItem */

/* 02554288: date (save+0x48) % 7 */
s32 dKy_get_dayofweek() {
    WWHD_FUNC(0x02554288, s32);
    s32 date = ld16(save_l() + 0x48);
    return date % 7;
}
VERIFY(0x02554288, dKy_get_dayofweek);

static inline u32 ev_l() { return save_l() + 0x644; }
static inline u32 tmp_l() { return save_l() + 0x1178; }
static inline void incEventRegMax_l(u32 reg, s32 max) {
    s32 v = (s32)getEventReg_l(ev_l(), reg) + 1;
    u32 ev = ev_l();
    if (v > max) v = max;
    setEventReg_l(ev, reg, (u8)v);
}

/* 02554288+: dKankyo_DayProc (d_kankyo_dayproc.inc), unchanged apart from the HD event flag calls */
void dKankyo_DayProc() {
    WWHD_FUNC(0x025542BC, void);
    if (isEventBit_l(ev_l(), 0x1820)) dLetter_autoStock_l(0x8B03);
    if (isEventBit_l(ev_l(), 0x1820) && isEventBit_l(ev_l(), 0x0B80)) dLetter_autoStock_l(0xB203);
    dLetter_delivery_l(0xAC03);
    dLetter_delivery_l(0xAE03);
    dLetter_delivery_l(0xB003);
    dLetter_delivery_l(0xAF03);
    dLetter_delivery_l(0x9D03);
    incEventRegMax_l(0xCF03, 3);
    incEventRegMax_l(0xAB03, 3);
    if (isReserve_l(save_l() + 0xB0, 0xF) && checkReserveItem_l(save_l() + 0x96, 0x9B) == 0) incEventRegMax_l(0xCCFF, 2);
    setEventReg_l(ev_l(), 0xBCFF, 0);
    setEventReg_l(ev_l(), 0xCB03, 0);
    setEventReg_l(ev_l(), 0xCA03, 0);
    setEventReg_l(ev_l(), 0xC903, 0);
    offEventBit_l(ev_l(), 0x1304);
    offEventBit_l(ev_l(), 0x1302);
    offEventBit_l(ev_l(), 0x1301);
    static const u16 tmp1[] = {0x120, 0x40, 0x20, 0x10, 8, 4, 2, 1, 0x180, 0x140, 0x580, 0x104};
    for (u16 f : tmp1) offEventBit_l(tmp_l(), f);
    if (getEventReg_l(ev_l(), 0xC103) == 1) setEventReg_l(ev_l(), 0xC103, 2);
    if (isEventBit_l(ev_l(), 0x1F10)) {
        u8 reg = getEventReg_l(ev_l(), 0xBB07);
        u32 ev = ev_l();
        if (reg == 7) {
            onEventBit_l(ev, 0x1F08);
        } else {
            s32 v = (s32)reg + 1;
            if (v > 7) v = 7;
            setEventReg_l(ev, 0xBB07, (u8)v);
        }
    }
    if (getEventReg_l(ev_l(), 0xC407) == 6) setEventReg_l(ev_l(), 0xC407, 7);
    u8 reg = getEventReg_l(ev_l(), 0xB907);
    if (reg & 1) setEventReg_l(ev_l(), 0xB907, (u8)(reg + 1));
    offEventBit_l(ev_l(), 0x2680);
    if (isEventBit_l(ev_l(), 0x2A20)) incEventRegMax_l(0xA60F, 3);
    if (dKy_get_dayofweek() == 5) {
        static const u16 ev5[] = {0x2080, 0x2004, 0x2002, 0x2804, 0x2802, 0x2801, 0x2980, 0x2940,
                                  0x3B01, 0x3C80, 0x3C40, 0x3C20, 0x3C10, 0x3C08, 0x3C04, 0x3C02};
        for (u16 f : ev5) offEventBit_l(ev_l(), f);
    }
    offEventBit_l(tmp_l(), 0x208);
    if (isEventBit_l(ev_l(), 0x2F01)) onEventBit_l(ev_l(), 0x3080);
    offEventBit_l(tmp_l(), 0x302);
    offEventBit_l(tmp_l(), 0x301);
}
VERIFY(0x025542BC, dKankyo_DayProc);

/* 02554870: WIND_INFLUENCE-like inline constructor (0x2C bytes, HD float +0x28 = 1.0); allocates when this == NULL */
u32 ctor_0x2C(u32 t) {
    WWHD_FUNC(0x02554870, u32, t);
    if (t == 0) {
        t = operator_new_l(0x2C);
        if (t == 0) return t;
    }
    stf(t + 0x28, ldf(0x1004F0A4));
    return t;
}
VERIFY(0x02554870, ctor_0x2C);

/* 025548B0: 0x3C-byte inline constructor (HD float +0x38 = 1.0) */
u32 ctor_0x3C(u32 t) {
    WWHD_FUNC(0x025548B0, u32, t);
    if (t == 0) {
        t = operator_new_l(0x3C);
        if (t == 0) return t;
    }
    stf(t + 0x38, ldf(0x1004F0A4));
    return t;
}
VERIFY(0x025548B0, ctor_0x3C);

/* GXColorS10 addcol setters: colour * factor, the env light fetched for each component.
 * The s16 arguments are converted from the whole register (GHS does not re-extend). */
static inline void addcol_set(u32 off, s32 r, s32 g, s32 b, f32 factor) {
    u32 env = envlight_l();
    gabi::store<s16>(env + off, (s16)gabi::ftoi(i2f(r) * factor));
    env = envlight_l();
    gabi::store<s16>(env + off + 2, (s16)gabi::ftoi(i2f(g) * factor));
    env = envlight_l();
    gabi::store<s16>(env + off + 4, (s16)gabi::ftoi(i2f(b) * factor));
}
/* 025548F0 */
void dKy_actor_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x025548F0, void, r, g, b, factor);
    addcol_set(0xF58, r, g, b, factor);
}
VERIFY(0x025548F0, dKy_actor_addcol_amb_set);
/* 02554A04 */
void dKy_actor_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02554A04, void, r, g, b, factor);
    addcol_set(0xF60, r, g, b, factor);
}
VERIFY(0x02554A04, dKy_actor_addcol_dif_set);
/* 02554B18 */
void dKy_bg_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02554B18, void, r, g, b, factor);
    addcol_set(0xF68, r, g, b, factor);
}
VERIFY(0x02554B18, dKy_bg_addcol_amb_set);
/* 02554C2C */
void dKy_bg_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02554C2C, void, r, g, b, factor);
    addcol_set(0xF70, r, g, b, factor);
}
VERIFY(0x02554C2C, dKy_bg_addcol_dif_set);
/* 02554D40 */
void dKy_bg1_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02554D40, void, r, g, b, factor);
    addcol_set(0xF78, r, g, b, factor);
}
VERIFY(0x02554D40, dKy_bg1_addcol_amb_set);
/* 02554E54 */
void dKy_bg1_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02554E54, void, r, g, b, factor);
    addcol_set(0xF80, r, g, b, factor);
}
VERIFY(0x02554E54, dKy_bg1_addcol_dif_set);
/* 02554F68 */
void dKy_bg2_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02554F68, void, r, g, b, factor);
    addcol_set(0xF88, r, g, b, factor);
}
VERIFY(0x02554F68, dKy_bg2_addcol_amb_set);
/* 0255507C */
void dKy_bg2_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x0255507C, void, r, g, b, factor);
    addcol_set(0xF90, r, g, b, factor);
}
VERIFY(0x0255507C, dKy_bg2_addcol_dif_set);
/* 02555190 */
void dKy_bg3_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x02555190, void, r, g, b, factor);
    addcol_set(0xF98, r, g, b, factor);
}
VERIFY(0x02555190, dKy_bg3_addcol_amb_set);
/* 025552A4 */
void dKy_bg3_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x025552A4, void, r, g, b, factor);
    addcol_set(0xFA0, r, g, b, factor);
}
VERIFY(0x025552A4, dKy_bg3_addcol_dif_set);

/* 025553B8 */
void dKy_actor_addcol_set(s32 r, s32 g, s32 b, f32 factor) {
    WWHD_FUNC(0x025553B8, void, r, g, b, factor);
    dKy_actor_addcol_amb_set(r, g, b, factor);
    dKy_actor_addcol_dif_set(r, g, b, factor);
    dKy_bg_addcol_amb_set(r, g, b, factor);
    dKy_bg_addcol_dif_set(r, g, b, factor);
    dKy_bg1_addcol_amb_set(r, g, b, factor);
    dKy_bg1_addcol_dif_set(r, g, b, factor);
    dKy_bg2_addcol_amb_set(r, g, b, factor);
    dKy_bg2_addcol_dif_set(r, g, b, factor);
    dKy_bg3_addcol_amb_set(r, g, b, factor);
    dKy_bg3_addcol_dif_set(r, g, b, factor);
}
VERIFY(0x025553B8, dKy_actor_addcol_set);

/* 025554CC: dScnKy_env_light_c::dScnKy_env_light_c(), HD 0x120C bytes, vtable 1004F084 at +0x1208.
 * HD zero-fills its sub-objects (028F521C) and constructs the LIGHT_INFLUENCE arrays. */
u32 dScnKy_env_light_c_ct(u32 t) {
    WWHD_FUNC(0x025554CC, u32, t);
    if (t == 0) {
        t = operator_new_l(0x120C);
        if (t == 0) return t;
    }
    const f32 one = ldf(0x1004F0A4);
    st(t + 0, 0);
    st(t + 0x1208, 0x1004F084);
    st(t + 0xC, 0);
    st(t + 4, 0);
    st(t + 8, 0);
    st(t + 0x10, 0);
    memclr_l(t + 0x14, 0x24);
    stf(t + 0x34, one);
    gabi::call(0x028F0020, t + 0x38, 0x1E, 0x24, 0x025637D4, 0); /* LIGHT_INFLUENCE[30] */
    memclr_l(t + 0x470, 0x320);
    memclr_l(t + 0x790, 0x28);
    memclr_l(t + 0x7B8, 0x24);
    stf(t + 0x7D8, one);
    st(t + 0x7DC, 0);
    memclr_l(t + 0x7E0, 0x2C);
    ctor_0x2C(t + 0x7E0);
    st(t + 0x80C, 0);
    memclr_l(t + 0x810, 0x78);
    memclr_l(t + 0x888, 0x12C);
    memclr_l(t + 0x9B4, 0x18);
    memclr_l(t + 0x9CC, 0x30);
    memclr_l(t + 0x9FC, 0x34);
    memclr_l(t + 0xA30, 0xC);
    memclr_l(t + 0xA3C, 0xC);
    memclr_l(t + 0xA48, 0xC);
    memclr_l(t + 0xA54, 0x10);
    memclr_l(t + 0xA64, 0xC);
    memclr_l(t + 0xA70, 0xC);
    memclr_l(t + 0xA7C, 0xC);
    memclr_l(t + 0xA88, 0x10);
    memclr_l(t + 0xA98, 0xC);
    memclr_l(t + 0xAA4, 0xC);
    memclr_l(t + 0xAB0, 0x3C);
    ctor_0x3C(t + 0xAB0);
    memclr_l(t + 0xAEC, 0x28);
    memclr_l(t + 0xB14, 0xC);
    memclr_l(t + 0xB50, 0x14);
    const f32 zero = ldf(0x1004F0B0);
    gabi::store<u8>(t + 0xB64, 0);
    gabi::store<u8>(t + 0xB81, 0);
    gabi::store<u8>(t + 0xB73, 0xFF);
    gabi::store<u8>(t + 0xB74, 0);
    gabi::store<u8>(t + 0xB6F, 0xFF);
    gabi::store<u8>(t + 0xB6E, 0);
    gabi::store<u8>(t + 0xB6B, 0xFF);
    gabi::store<u8>(t + 0xB89, 0);
    gabi::store<u8>(t + 0xB67, 0xFF);
    gabi::store<u8>(t + 0xB80, 0);
    gabi::store<u8>(t + 0xB83, 0xFF);
    gabi::store<u8>(t + 0xB6C, 0);
    gabi::store<u8>(t + 0xB87, 0xFF);
    gabi::store<u8>(t + 0xB69, 0);
    gabi::store<u8>(t + 0xB7B, 0xFF);
    gabi::store<u8>(t + 0xB70, 0);
    gabi::store<u8>(t + 0xB7D, 0);
    gabi::store<u8>(t + 0xB8C, 0);
    gabi::store<u8>(t + 0xB79, 0);
    gabi::store<u8>(t + 0xB75, 0);
    gabi::store<u8>(t + 0xB7A, 0);
    gabi::store<u8>(t + 0xB7C, 0);
    gabi::store<u8>(t + 0xB82, 0);
    gabi::store<u8>(t + 0xB72, 0);
    gabi::store<u8>(t + 0xB8D, 0);
    gabi::store<u8>(t + 0xB77, 0xFF);
    gabi::store<u8>(t + 0xB71, 0);
    gabi::store<u8>(t + 0xB78, 0);
    gabi::store<u8>(t + 0xB8E, 0);
    gabi::store<u8>(t + 0xB7E, 0);
    gabi::store<u8>(t + 0xB65, 0);
    gabi::store<u8>(t + 0xB86, 0);
    gabi::store<u8>(t + 0xB8F, 0xFF);
    gabi::store<u8>(t + 0xB8B, 0xFF);
    gabi::store<u8>(t + 0xB85, 0);
    gabi::store<u8>(t + 0xB76, 0);
    gabi::store<u8>(t + 0xB90, 0);
    gabi::store<u8>(t + 0xB6D, 0);
    gabi::store<u8>(t + 0xB68, 0);
    gabi::store<u8>(t + 0xB66, 0);
    gabi::store<u8>(t + 0xB91, 0);
    gabi::store<u8>(t + 0xB6A, 0);
    gabi::store<u8>(t + 0xB84, 0);
    gabi::store<u8>(t + 0xB8A, 0);
    gabi::store<u8>(t + 0xB92, 0);
    gabi::store<u8>(t + 0xB7F, 0xFF);
    gabi::store<u8>(t + 0xB88, 0);
    gabi::store<u8>(t + 0xB93, 0xFF);
    gabi::store<u8>(t + 0xB94, 0);
    gabi::store<u8>(t + 0xB95, 0);
    gabi::store<u8>(t + 0xB96, 0);
    gabi::store<u8>(t + 0xB97, 0xFF);
    gabi::store<u8>(t + 0xB98, 0);
    gabi::store<u8>(t + 0xB99, 0);
    gabi::store<u8>(t + 0xB9A, 0);
    gabi::store<u8>(t + 0xB9B, 0xFF);
    gabi::store<u8>(t + 0xBA2, 0);
    gabi::store<u8>(t + 0xB9D, 0);
    gabi::store<u8>(t + 0xB9C, 0);
    gabi::store<u8>(t + 0xBA4, 0);
    gabi::store<u8>(t + 0xBA3, 0xFF);
    gabi::store<u8>(t + 0xBA7, 0xFF);
    gabi::store<u8>(t + 0xBA1, 0);
    gabi::store<u8>(t + 0xBA0, 0);
    gabi::store<u8>(t + 0xBA6, 0);
    gabi::store<u8>(t + 0xBA5, 0);
    gabi::store<u8>(t + 0xB9E, 0);
    gabi::store<u8>(t + 0xB9F, 0xFF);
    memclr_l(t + 0xBA8, 8);
    gabi::store<u8>(t + 0xBCA, 0);
    gabi::store<u8>(t + 0xBCE, 0);
    gabi::store<u8>(t + 0xBC7, 0xFF);
    gabi::store<u8>(t + 0xBB7, 0xFF);
    gabi::store<u8>(t + 0xBCB, 0xFF);
    gabi::store<u8>(t + 0xBBE, 0);
    gabi::store<u8>(t + 0xBB9, 0);
    gabi::store<u8>(t + 0xBB3, 0xFF);
    gabi::store<u8>(t + 0xBBC, 0);
    gabi::store<u8>(t + 0xBBA, 0);
    gabi::store<u8>(t + 0xBBF, 0xFF);
    gabi::store<u8>(t + 0xBCC, 0);
    gabi::store<u8>(t + 0xBB6, 0);
    gabi::store<u8>(t + 0xBB5, 0);
    gabi::store<u8>(t + 0xBD5, 0);
    gabi::store<u8>(t + 0xBD4, 0);
    gabi::store<u8>(t + 0xBB4, 0);
    gabi::store<u8>(t + 0xBB0, 0);
    gabi::store<u8>(t + 0xBD6, 0);
    gabi::store<u8>(t + 0xBD3, 0xFF);
    gabi::store<u8>(t + 0xBB1, 0);
    gabi::store<u8>(t + 0xBC0, 0);
    gabi::store<u8>(t + 0xBD7, 0xFF);
    gabi::store<u8>(t + 0xBB2, 0);
    gabi::store<u8>(t + 0xBC6, 0);
    gabi::store<u8>(t + 0xBD0, 0);
    gabi::store<u8>(t + 0xBD8, 0);
    gabi::store<u8>(t + 0xBC3, 0xFF);
    gabi::store<u8>(t + 0xBCD, 0);
    gabi::store<u8>(t + 0xBC5, 0);
    gabi::store<u8>(t + 0xBD9, 0);
    gabi::store<u8>(t + 0xBD2, 0);
    gabi::store<u8>(t + 0xBBD, 0);
    gabi::store<u8>(t + 0xBB8, 0);
    gabi::store<u8>(t + 0xBDA, 0);
    gabi::store<u8>(t + 0xBC2, 0);
    gabi::store<u8>(t + 0xBC9, 0);
    gabi::store<u8>(t + 0xBC1, 0);
    gabi::store<u8>(t + 0xBDB, 0xFF);
    gabi::store<u8>(t + 0xBBB, 0xFF);
    gabi::store<u8>(t + 0xBCF, 0xFF);
    gabi::store<u8>(t + 0xBD1, 0);
    gabi::store<u8>(t + 0xBDC, 0);
    gabi::store<u8>(t + 0xBC4, 0);
    gabi::store<u8>(t + 0xBC8, 0);
    gabi::store<u8>(t + 0xBDD, 0);
    gabi::store<u8>(t + 0xBDE, 0);
    gabi::store<u8>(t + 0xBDF, 0xFF);
    gabi::store<f32>(t + 0xBE0, zero);
    gabi::store<f32>(t + 0xC04, zero);
    gabi::store<u8>(t + 0xC23, 0xFF);
    gabi::store<u8>(t + 0xC22, 0);
    gabi::store<f32>(t + 0xC00, zero);
    gabi::store<f32>(t + 0xC08, zero);
    gabi::store<u8>(t + 0xC24, 0);
    gabi::store<u8>(t + 0xC21, 0);
    gabi::store<f32>(t + 0xBFC, zero);
    gabi::store<f32>(t + 0xC0C, zero);
    gabi::store<f32>(t + 0xC3C, zero);
    gabi::store<u8>(t + 0xC20, 0);
    gabi::store<f32>(t + 0xC38, zero);
    gabi::store<f32>(t + 0xBF8, zero);
    gabi::store<f32>(t + 0xC34, zero);
    gabi::store<f32>(t + 0xC10, zero);
    gabi::store<f32>(t + 0xC30, zero);
    gabi::store<u8>(t + 0xC26, 0);
    gabi::store<f32>(t + 0xC2C, zero);
    gabi::store<u8>(t + 0xC1F, 0xFF);
    gabi::store<u8>(t + 0xC2B, 0xFF);
    gabi::store<f32>(t + 0xBF4, zero);
    gabi::store<f32>(t + 0xBE4, zero);
    gabi::store<f32>(t + 0xC14, zero);
    gabi::store<u8>(t + 0xC1B, 0xFF);
    gabi::store<u8>(t + 0xC27, 0xFF);
    gabi::store<u8>(t + 0xC2A, 0);
    gabi::store<u8>(t + 0xC1E, 0);
    gabi::store<u8>(t + 0xC1A, 0);
    gabi::store<f32>(t + 0xBF0, zero);
    gabi::store<f32>(t + 0xBE8, zero);
    gabi::store<u8>(t + 0xC18, 0);
    gabi::store<u8>(t + 0xC1C, 0);
    gabi::store<u8>(t + 0xC28, 0);
    gabi::store<u8>(t + 0xC29, 0);
    gabi::store<u8>(t + 0xC1D, 0);
    gabi::store<u8>(t + 0xC19, 0);
    gabi::store<u8>(t + 0xC25, 0);
    gabi::store<f32>(t + 0xBEC, zero);
    gabi::call(0x027FB40C, t + 0xC40);
    st(t + 0xC4C, 0x1016F014);
    memclr_l(t + 0xCB4, 0x294);
    gabi::call(0x028EFFD0, t + 0xCB4, 4, 0x40, 0x02563814); /* __construct_array */
    /* inline sub-object constructors: allocate when their address is NULL */
    static const u16 subs[] = {0x100, 0x110, 0x160, 0x170, 0x180, 0x194, 0x1A4, 0x1B4, 0x1C4, 0x1D4, 0x1E4};
    for (u16 o : subs)
        if (t + 0xCB4 + o == 0) operator_new_l(o == 0x110 ? 0x40 : 0x10);
    gabi::call(0x028EFFD0, t + 0xEA8, 0xA, 0x10, 0x02563840);
    for (u32 o = 0xF48; o <= 0xFB8; o += 8) memclr_l(t + o, 8);
    gabi::store<f32>(t + 0x1020, zero);
    gabi::store<f32>(t + 0x104C, zero);
    gabi::store<f32>(t + 0x1014, zero);
    gabi::store<f32>(t + 0xFD8, zero);
    gabi::store<f32>(t + 0xFE8, zero);
    gabi::store<f32>(t + 0xFCC, zero);
    gabi::store<f32>(t + 0xFC4, zero);
    gabi::store<f32>(t + 0x1004, zero);
    gabi::store<f32>(t + 0xFEC, zero);
    gabi::store<f32>(t + 0xFF4, zero);
    gabi::store<f32>(t + 0x1010, zero);
    gabi::store<u32>(t + 0x1054, 0);
    gabi::store<f32>(t + 0xFDC, zero);
    gabi::store<f32>(t + 0x1000, zero);
    gabi::store<f32>(t + 0x103C, zero);
    gabi::store<u32>(t + 0x1050, 0);
    gabi::store<f32>(t + 0xFE4, zero);
    gabi::store<f32>(t + 0xFF8, zero);
    gabi::store<f32>(t + 0x1040, zero);
    gabi::store<f32>(t + 0x1018, zero);
    gabi::store<f32>(t + 0x101C, zero);
    gabi::store<f32>(t + 0x1028, zero);
    gabi::store<f32>(t + 0xFE0, zero);
    gabi::store<f32>(t + 0x1034, zero);
    gabi::store<f32>(t + 0xFC8, zero);
    gabi::store<f32>(t + 0x1024, zero);
    gabi::store<f32>(t + 0xFF0, zero);
    gabi::store<f32>(t + 0xFD0, zero);
    gabi::store<f32>(t + 0x102C, zero);
    gabi::store<f32>(t + 0x1038, zero);
    gabi::store<f32>(t + 0x1048, zero);
    gabi::store<f32>(t + 0xFC0, zero);
    gabi::store<f32>(t + 0x1030, zero);
    gabi::store<f32>(t + 0xFD4, zero);
    gabi::store<f32>(t + 0x1044, zero);
    gabi::store<f32>(t + 0x1008, zero);
    gabi::store<f32>(t + 0x100C, zero);
    gabi::store<f32>(t + 0xFFC, zero);
    memclr_l(t + 0x1058, 0x10);
    gabi::store<u8>(t + 0x1090, 0);
    gabi::store<u32>(t + 0x107C, 0);
    gabi::store<u8>(t + 0x1091, 0);
    gabi::store<u32>(t + 0x1078, 0);
    gabi::store<u8>(t + 0x1092, 0);
    gabi::store<u32>(t + 0x1080, 0);
    gabi::store<u32>(t + 0x1074, 0);
    gabi::store<u8>(t + 0x108F, 0);
    gabi::store<u8>(t + 0x1093, 0);
    gabi::store<u32>(t + 0x1084, 0);
    gabi::store<u32>(t + 0x1070, 0);
    gabi::store<u8>(t + 0x108C, 0);
    gabi::store<u8>(t + 0x10A5, 0);
    gabi::store<u8>(t + 0x10A2, 0);
    gabi::store<u8>(t + 0x109E, 0);
    gabi::store<f32>(t + 0x10B8, zero);
    gabi::store<f32>(t + 0x10A8, zero);
    gabi::store<u8>(t + 0x1095, 0);
    gabi::store<u8>(t + 0x1094, 0);
    gabi::store<u8>(t + 0x109B, 0);
    gabi::store<u8>(t + 0x1097, 0);
    gabi::store<u8>(t + 0x1098, 0);
    gabi::store<u8>(t + 0x109C, 0);
    gabi::store<u16>(t + 0x1088, 0);
    gabi::store<u16>(t + 0x108A, 0);
    gabi::store<f32>(t + 0x10AC, zero);
    gabi::store<f32>(t + 0x10BC, zero);
    gabi::store<u8>(t + 0x109F, 0);
    gabi::store<u8>(t + 0x10A3, 0);
    gabi::store<u8>(t + 0x10A1, 0);
    gabi::store<u8>(t + 0x109D, 0);
    gabi::store<f32>(t + 0x10B4, zero);
    gabi::store<u8>(t + 0x10A6, 0);
    gabi::store<u8>(t + 0x108D, 0);
    gabi::store<u8>(t + 0x108E, 0);
    gabi::store<u8>(t + 0x109A, 0);
    gabi::store<u8>(t + 0x1096, 0);
    gabi::store<u8>(t + 0x1099, 0);
    gabi::store<f32>(t + 0x10C0, zero);
    gabi::store<u32>(t + 0x106C, 0);
    gabi::store<u32>(t + 0x1068, 0);
    gabi::store<f32>(t + 0x10B0, zero);
    gabi::store<f32>(t + 0x10C4, zero);
    gabi::store<u8>(t + 0x10A0, 0);
    gabi::store<u8>(t + 0x10A4, 0);
    gabi::store<f32>(t + 0x10C8, zero);
    gabi::store<f32>(t + 0x10CC, zero);
    gabi::store<f32>(t + 0x10D0, zero);
    gabi::store<f32>(t + 0x10D4, zero);
    gabi::store<f32>(t + 0x10D8, zero);
    gabi::store<f32>(t + 0x10DC, zero);
    gabi::store<f32>(t + 0x10E0, zero);
    gabi::store<f32>(t + 0x10E4, zero);
    gabi::store<f32>(t + 0x10E8, zero);
    memclr_l(t + 0x10EC, 0x1C);
    memclr_l(t + 0x1108, 0x80);
    memclr_l(t + 0x1188, 0x80);
    dKy_actor_addcol_set(0, 0, 0, zero);
    stf(t + 0xB48, zero);
    stf(t + 0xB3C, zero);
    f32 m1 = ldf(0x1004F0B4);
    stf(t + 0xB4C, zero);
    stf(t + 0x1024, m1);
    stf(t + 0xB44, m1);
    stf(t + 0xB40, zero);
    stf(t + 0xB38, one);
    return t;
}
VERIFY(0x025554CC, dScnKy_env_light_c_ct);

/* 02555D0C: dKy_getEnvlight() (matcher: setLightTevColorType, wrong): HD lazy function-local
 * static g_env_light (10475A68, guard 104773B4, destructor record 101E8E9C) */
u32 dKy_getEnvlight() {
    WWHD_FUNC(0x02555D0C, u32);
    if (ld(0x104773B4) == 0) {
        st(0x104773B4, 1);
        dScnKy_env_light_c_ct(g_env_light);
        register_global_object_l(0x101E8E9C);
    }
    return g_env_light;
}
VERIFY(0x02555D0C, dKy_getEnvlight);

/* 02555D64: toon_proc_check: HD calls the room's virtual (+0x1DC) and always returns 0 */
BOOL toon_proc_check() {
    WWHD_FUNC(0x02555D64, BOOL);
    s32 roomNo = (s8)ld8(0x1047E6C8);
    if (roomNo >= 0) {
        u32 rd = gabi::call<u32>(0x025C11DC, dComIfGp_get_l() + 0x51CC, roomNo);
        gabi::call_ptr(ld(ld(rd) + 0x1DC), rd);
    }
    return FALSE;
}
VERIFY(0x02555D64, toon_proc_check);

/* 02555DBC */
s16 u8_data_ratio_set(s32 a, s32 b, f32 ratio) {
    WWHD_FUNC(0x02555DBC, s16, a, b, ratio);
    s16 d = (s16)gabi::ftoi(i2f(b - a) * ratio);
    return (s16)(a + d);
}
VERIFY(0x02555DBC, u8_data_ratio_set);

/* 02555E0C */
s16 s16_data_ratio_set(s32 a, s32 b, f32 ratio) {
    WWHD_FUNC(0x02555E0C, s16, a, b, ratio);
    s16 d = (s16)gabi::ftoi(i2f(b - a) * ratio);
    return (s16)(a + d);
}
VERIFY(0x02555E0C, s16_data_ratio_set);

/* 02555E5C: kankyo_color_ratio_set, scaled by the all-colour ratio (+0xFE4) and clamped to 0..255 */
s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add, f32 blend01) {
    WWHD_FUNC(0x02555E5C, s16, b0A, b0B, blendAB0, b1A, b1B, blendAB1, add, blend01);
    u32 env = envlight_l();
    s16 c0 = s16_data_ratio_set((s32)b0A, (s32)b0B, blendAB0);
    s16 c1 = s16_data_ratio_set((s32)b1A, (s32)b1B, blendAB0);
    s16 c = s16_data_ratio_set(c0, c1, blendAB1);
    s16 v = (s16)(c + add);
    f32 ratio = ldf(env + 0xFE4) * blend01;
    s16 r = (s16)gabi::ftoi(i2f(v) * ratio);
    if (r < 0) return 0;
    if (r > 0xFF) return 0xFF;
    return r;
}
VERIFY(0x02555E5C, kankyo_color_ratio_set);

/* 02555F80: fl_data_ratio_set */
f32 fl_data_ratio_set(f32 a, f32 b, f32 ratio) {
    WWHD_FUNC(0x02555F80, f32, a, b, ratio);
    return gabi::fmadds(b - a, ratio, a);
}
VERIFY(0x02555F80, fl_data_ratio_set);

/* 02555F8C: float_kankyo_color_ratio_set, scaled by the all-colour ratio (+0xFE4) */
f32 float_kankyo_color_ratio_set(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7) {
    WWHD_FUNC(0x02555F8C, f32, p0, p1, p2, p3, p4, p5, p6, p7);
    u32 env = envlight_l();
    f32 a = fl_data_ratio_set(p0, p1, p2);
    f32 b = fl_data_ratio_set(p3, p4, p2);
    f32 c = fl_data_ratio_set(a, b, p5);
    f32 rt = gabi::fmadds(p6 - c, p7, c) * ldf(env + 0xFE4);
    return rt >= 0.0f ? rt : ldf(0x1004F0B0);
}
VERIFY(0x02555F8C, float_kankyo_color_ratio_set);

}  // namespace d_kankyo_cpp
