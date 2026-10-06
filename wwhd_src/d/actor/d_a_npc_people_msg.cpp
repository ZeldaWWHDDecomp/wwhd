/**
 * d_a_npc_people_msg.cpp (WWHD)
 * NPC - Windfall townspeople: message selection (getMsg, getMsg3) and its helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_people.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* 0257DAA8 dKyw_get_wind_vec() -> cXyz* */
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
/* 02560828 dKy_moon_type_chk() */
static inline s32 dKy_moon_type_chk() { return gabi::call<s32>(0x02560828); }
/* 02526100 daDai_c::getMaxDaiza(), 0252610C daDai_c::getDaizaSetItemNum() (statics) */
static inline u32 daDai_getMaxDaiza() { return gabi::call<u32>(0x02526100); }
static inline u32 daDai_getDaizaSetItemNum() { return gabi::call<u32>(0x0252610C); }
/* 025B8B7C dSv_event_c::offEventBit */
static inline void dComIfGs_offEventBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_event(), f); }
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz a, cXyz b (pointers to copies), f32* dist, s16* angY) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, f32* dist, s16* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* play object fields (HD offsets) */
static inline u8 dComIfGp_getPictureResult() { return gabi::load<u8>(dComIfGp_ea() + 0x5BE6); }
static inline u8 dComIfGp_getPictureResultDetail() { return gabi::load<u8>(dComIfGp_ea() + 0x5BE7); }
static inline fopAc_ac_c* dComIfGp_getShipActor() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); }
/* save info: rupees (u16) at save + 0x24 */
static inline u16 dComIfGs_getRupee() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24); }

enum {
    dItemNo_DELIVERY_BAG_e = 0x30, dItemNo_WIND_WAKER_e = 0x22, dItemNo_PICTO_BOX_e = 0x23,
    dItemNo_DELUXE_PICTO_BOX_e = 0x26, dItemNo_MAGIC_ARMOR_e = 0x2A, dItemNo_SKULL_NECKLACE_e = 0x45,
    dItemNo_PEARL_NAYRU_e = 0x69, dItemNo_SAIL_e = 0x78, dItemNo_SWIFT_SAIL_e = 0x77 /* HD */,
    dItemNo_COLLECT_MAP_16_e = 0xEF, dItemNo_COLLECT_MAP_15_e = 0xF0,
};
enum { ENDLESS_NIGHT = 0x0A02, UNK_B907 = 0xB907, UNK_C407 = 0xC407, UNK_FE03 = 0xFE03, UNK_FF03 = 0xFF03 };

/* message tables (.data) */
enum : u32 {
    l_msg_uo1_1st_haitatu = 0x101C39F4, l_msg_uo1_haitatu = 0x101C2ECC, l_msg_uo1_1st_talk = 0x101C34E0,
    l_msg_uo1_1st_talk_fdai = 0x101C3A00, l_msg_uo1_2nd_talk_fdai = 0x101C2ED4, l_msg_uo1_fadi1 = 0x101C3A14,
    l_msg_uo1_fdai2 = 0x101C3A20,
    l_msg_uo2_surprise = 0x101C2EE4, l_msg_uo2_help = 0x101C2EEC, l_msg_uo2_1st_talk = 0x101C34F0,
    l_msg_uo2_2nd_talk = 0x101C2EDC,
    l_msg_uo3_kyoro = 0x101C2F0C, l_msg_uo3_letter = 0x101C2F14, l_msg_uo3_1st_talk = 0x101C3500,
    l_msg_uo3_befor_letter = 0x101C2EF4, l_msg_uo3_retry_letter = 0x101C2F04, l_msg_uo3_after_letter = 0x101C2EFC,
    l_msg_xy_uo3_photo = 0x101C3A2C, l_msg_xy_uo3_no_photo = 0x101C2F1C,
    l_msg_ub3_1st_talk = 0x101C3688, l_msg_ub3_ship_near5 = 0x101C2F4C, l_msg_ub3_tact = 0x101C3698,
    l_msg_ub3_talk = 0x101C3A6C,
    l_msg_ub4_photo_house = 0x101C3A84, l_msg_ub4_1st_talk = 0x101C2F54, l_msg_ub4_color_photo = 0x101C2F5C,
    l_msg_ub4_no_photo_box = 0x101C2F64, l_msg_ub4_photo_box = 0x101C3A78,
    l_msg_xy_ub4_no_photo = 0x101C2F6C, l_msg_xy_ub4_no_color = 0x101C2F74, l_msg_xy_ub4_get_item = 0x101C36A8,
    l_msg_xy_ub4_talk = 0x101C2F7C,
    l_msg_uw1_1st_talk_day = 0x101C3A90, l_msg_uw1_talk_day = 0x101C2F84, l_msg_uw1_1st_talk_night = 0x101C3AB4,
    l_msg_uw1_magic_shield = 0x101C3ACC, l_msg_uw1_no_magic_shield = 0x101C3AD8, l_msg_uw1_talk_night = 0x101C3AC0,
    l_msg_uw2_1st_talk1 = 0x101C3AE4, l_msg_uw2_no_photo3 = 0x101C3AF0, l_msg_uw2_no_1day_photo3 = 0x101C3AFC,
    l_msg_uw2_request = 0x101C36C8, l_msg_uw2_cafe_off = 0x101C3B08, l_msg_uw2_cafe_on = 0x101C2FAC,
    l_msg_uw2_1st_talk2 = 0x101C36D8, l_msg_uw2_talk2 = 0x101C2FB4, l_msg_uw2_talk3 = 0x101C2FBC,
    l_msg_um1_frrs_stop = 0x101C36E8, l_msg_um1_1st_talk = 0x101C3B14, l_msg_um1_light_off = 0x101C3718,
    l_msg_um1_get_item = 0x101C3728, l_msg_um1_last = 0x101C2FC4,
    l_msg_um2_1st_talk1 = 0x101C3738, l_msg_um2_no_1day_photo3 = 0x101C3748, l_msg_um2_cafe_on = 0x101C3B2C,
    l_msg_um2_no_request = 0x101C2FCC, l_msg_um2_cafe_off = 0x101C3B20, l_msg_um2_1st_talk2 = 0x101C2FD4,
    l_msg_um2_talk2 = 0x101C2FDC, l_msg_um2_talk3 = 0x101C2FE4,
    l_msg_xy_um2_talk1 = 0x101C3758, l_msg_xy_um2_talk2 = 0x101C3B38, l_msg_xy_um2_talk3 = 0x101C3768,
    l_msg_xy_um2_talk4 = 0x101C3778, l_msg_xy_um2_talk5 = 0x101C3B44,
    l_msg_um3_not_sail = 0x101C2FEC, l_msg_um3_1st_talk = 0x101C3790, l_msg_um3_no_nazo_talk = 0x101C3B50,
    l_msg_um3_map15 = 0x101C2FFC, l_msg_um3_no_map15 = 0x101C3B5C, l_msg_um3_1st_night = 0x101C3B68,
    l_msg_um3_no_look_moon = 0x101C3B74, l_msg_um3_map15_n = 0x101C37B0, l_msg_um3_no_map15_n = 0x101C3004,
    l_msg_xy_um3_ng = 0x101C300C, l_msg_xy_um3_sun = 0x101C3014, l_msg_xy_um3_no_full_moon = 0x101C301C,
    l_msg_xy_um3_no_color = 0x101C3024, l_msg_xy_moon = 0x101C3B80,
    l_msg_look_full_moon = 0x101C3B9C, l_msg_look_moon = 0x101C3BA8, l_msg_look_orion = 0x101C302C,
    l_msg_look_hokuto = 0x101C3034,
    l_msg_sa1_1st_talk = 0x101C3BB4, l_msg_sa1_talk = 0x101C303C,
    l_msg_sa2_1st_talk = 0x101C3044, l_msg_sa2_wind_west = 0x101C37C0, l_msg_sa2_wind_east = 0x101C3BC8,
    l_msg_sa2_wind_not_west = 0x101C3BD4, l_msg_sa2_night = 0x101C304C,
    l_msg_sa3_not_sail = 0x101C3BE0, l_msg_sa3_1st_talk = 0x101C3BEC, l_msg_sa3_fdai = 0x101C3850 /* [8] */,
    l_msg_sa3_night = 0x101C3064,
    l_msg_sa4_not_sail = 0x101C306C, l_msg_sa4_1st_talk = 0x101C3C1C, l_msg_sa4_wind = 0x101C3920 /* [8] */,
    l_msg_sa4_night = 0x101C3074,
    l_msg_sa5__next_day = 0x101C30BC, l_msg_sa5_timer_zero = 0x101C30B4, l_msg_sa5_mini_game_clr = 0x101C3960,
    l_msg_sa5_false_1st = 0x101C3CE8 /* [3] */, l_msg_sa5_false_2nd = 0x101C3CF4, l_msg_xy_pig1 = 0x101C30DC,
    l_msg_xy_pig2 = 0x101C30E4, l_msg_xy_sa5_mini_game2_clr = 0x101C3D3C, l_msg_sa5_1st_talk = 0x101C3C28,
    l_msg_sa5_play = 0x101C3950 /* [4] */, l_msg_sa5_day = 0x101C308C, l_msg_sa5_50rupee = 0x101C3094,
    l_msg_sa5_1st = 0x101C3C58, l_msg_sa5_2nd = 0x101C3C64, l_msg_sa5_night = 0x101C30C4,
    l_msg_xy_sa5_no_skull_necklace = 0x101C30CC, l_msg_xy_sa5_1st = 0x101C3978, l_msg_xy_sa5_2nd = 0x101C3D00,
    l_msg_ug1_out_area = 0x101C30FC, l_msg_ug1_1st_talk = 0x101C3D50, l_msg_ug1_hint_talk = 0x101C30EC,
    l_msg_ug1_hint_talk_today = 0x101C30F4,
    l_msg_ug2_out_area = 0x101C310C, l_msg_ug2_1st_talk = 0x101C3D5C, l_msg_ug2_hint_talk = 0x101C3104,
    l_msg_ug2_hint_talk_today = 0x101C3D68,
};
static inline be<u32>* MSG(u32 a) { return gabi::at<be<u32>>(a); }

/* 022C60D0 */
int daNpcPeople_c::getWindDir() {
    WWHD_FUNC(0x022C60D0, int, this);
    cXyz* wind = dKyw_get_wind_vec();
    u16 angle = cM_atan2s(wind->x, wind->z);
    angle += 0x1000;
    angle /= 0x2000;
    return angle & 7;
}
VERIFY(0x022C60D0, &daNpcPeople_c::getWindDir);

/* 022C6910 (HD: the picture's colour flag at play + 0x5BEA instead of the picture format) */
BOOL daNpcPeople_c::isColor() {
    WWHD_FUNC(0x022C6910, BOOL, this);
    return gabi::load<u8>(dComIfGp_ea() + 0x5BEA) != 0;
}
VERIFY(0x022C6910, &daNpcPeople_c::isColor);

/* 022C693C */
BOOL daNpcPeople_c::isUo1FdaiAll() {
    WWHD_FUNC(0x022C693C, BOOL, this);
    u32 max = daDai_getMaxDaiza();
    return max == daDai_getDaizaSetItemNum() ? TRUE : FALSE;
}
VERIFY(0x022C693C, &daNpcPeople_c::isUo1FdaiAll);

/* 022C6978 */
BOOL daNpcPeople_c::isUo1FdaiOne() {
    WWHD_FUNC(0x022C6978, BOOL, this);
    return daDai_getDaizaSetItemNum() != 0 ? TRUE : FALSE;
}
VERIFY(0x022C6978, &daNpcPeople_c::isUo1FdaiOne);

/* 022C69A0 */
s32 daNpcPeople_c::chkDaiza() {
    WWHD_FUNC(0x022C69A0, s32, this);
    /* l_daiza_no_tbl: u16[6] at .data 0x101C3E1C */
#define DAIZA(i) dComIfGs_getEventReg(gabi::load<u16>(0x101C3E1C + (i) * 2))
    int i;
    for (i = 0; i < 6; i++) {
        if (DAIZA(i) == 0x97) {
            return 1;
        }
    }
    for (i = 0; i < 6; i++) {
        if (DAIZA(i) == 0x96) {
            return 2;
        }
    }
    int temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = DAIZA(i);
        if (reg == 0x8C || reg == 0x8D || reg == 0x8E) {
            temp++;
        }
    }
    if (temp == 6) {
        return 3;
    }
    temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = DAIZA(i);
        if (reg == 0x8F || reg == 0x90 || reg == 0x91 || reg == 0x92 || reg == 0x93) {
            temp++;
        }
    }
    if (temp == 6) {
        return 4;
    }
    temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = DAIZA(i);
        if (reg == 0x94 || reg == 0x95) {
            temp++;
        }
    }
    if (temp == 6) {
        return 5;
    }
    temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = DAIZA(i);
        if (reg != 0) {
            temp++;
        }
    }
#undef DAIZA
    if (temp >= 2) {
        return 6;
    }
    if (temp >= 1) {
        return 7;
    }
    return 0;
}
VERIFY(0x022C69A0, &daNpcPeople_c::chkDaiza);

/* 022C1F20 */
u32 daNpcPeople_c::getMsg3() {
    WWHD_FUNC(0x022C1F20, u32, this);
    u32 msgNo = 0;
    m734 = nullptr;
    switch (mNpcNo) {
    case NPC_UM3:
        switch (m7A7) {
        case 0:
        default:
            if (dKy_moon_type_chk() == 0) {
                m734 = MSG(l_msg_look_full_moon);
            } else {
                m734 = MSG(l_msg_look_moon);
            }
            break;
        case 1:
            m734 = MSG(l_msg_look_orion);
            break;
        case 2:
            m734 = MSG(l_msg_look_hokuto);
            break;
        }
    }
    if (m734.get() != nullptr) {
        msgNo = *m734;
    }
    return msgNo;
}
VERIFY(0x022C1F20, &daNpcPeople_c::getMsg3);

/* HD: the "endless night" check (Nayru's Pearl not yet obtained) */
static inline bool people_endlessNight() {
    return dComIfGs_isEventBit(ENDLESS_NIGHT) && !dComIfGs_checkGetItem(dItemNo_PEARL_NAYRU_e);
}
/* HD: the Swift Sail counts as the sail */
static inline bool people_haveSail() {
    return dComIfGs_checkGetItem(dItemNo_SAIL_e) || dComIfGs_checkGetItem(dItemNo_SWIFT_SAIL_e);
}

/* diff = current.pos - home.pos; diff.y = 0; diff.abs() (HD: sqrt without the zero test; x/z
 * copied as integers) */
static inline f32 people_homeDistXZ(daNpcPeople_c* i_this) {
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> diff;
    cXyz_mi(&i_this->current.pos, tmp, &i_this->home.pos);
    gabi::store<u32>(gabi::ea(diff.get()), gabi::load<u32>(gabi::ea(tmp.get())));
    gabi::store<u32>(gabi::ea(diff.get()) + 8, gabi::load<u32>(gabi::ea(tmp.get()) + 8));
    diff->y = 0.0f;
    return std_sqrtf(PSVECSquareMag(diff));
}

/* 022C6C68 */
u32 daNpcPeople_c::getMsg() {
    WWHD_FUNC(0x022C6C68, u32, this);
    u32 msgNo = 0;
    m734 = nullptr;

    /* HD: dEvt_control_c::chkPhoto is play + 0x52B2 (the play object is fetched twice) */
    u8 photo = gabi::load<u8>(dComIfGp_ea() + 0x52B2);
    u32 play = dComIfGp_ea();
    if (photo) {
        switch (mNpcNo) {
        case NPC_UO3:
            if (dComIfGp_getPictureResult() == 1) {
                m734 = MSG(l_msg_xy_uo3_photo);
            } else {
                m734 = MSG(l_msg_xy_uo3_no_photo);
            }
            break;
        case NPC_UB4:
            if (dComIfGp_getPictureResult() != 5 && dComIfGp_getPictureResult() != 6 &&
                dComIfGp_getPictureResult() != 0x6D) {
                m734 = MSG(l_msg_xy_ub4_no_photo);
            } else if (!isColor()) {
                m734 = MSG(l_msg_xy_ub4_no_color);
            } else if (!dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_16_e)) {
                m734 = MSG(l_msg_xy_ub4_get_item);
                dComIfGs_onEventBit(0x2504);
            } else {
                m734 = MSG(l_msg_xy_ub4_talk);
            }
            break;
        case NPC_UM2:
            if (dComIfGp_getPictureResult() != 0x67) { /* HD: 0x67 (GameCube 4) */
                m734 = MSG(l_msg_xy_um2_talk1);
            } else if (dComIfGp_getPictureResultDetail() != 0) {
                m734 = MSG(l_msg_xy_um2_talk2);
            } else if (!isColor()) {
                m734 = MSG(l_msg_xy_um2_talk3);
            } else if (!dComIfGs_isEventBit(0x2220)) {
                m734 = MSG(l_msg_xy_um2_talk4);
            } else {
                m734 = MSG(l_msg_xy_um2_talk5);
            }
            break;
        case NPC_UM3:
            if (dComIfGp_getPictureResult() != 7) {
                if (dComIfGp_getPictureResult() == 8) {
                    m734 = MSG(l_msg_xy_um3_no_full_moon);
                } else if (dComIfGp_getPictureResult() == 9) {
                    m734 = MSG(l_msg_xy_um3_sun);
                } else {
                    m734 = MSG(l_msg_xy_um3_ng);
                }
            } else if (!isColor()) {
                m734 = MSG(l_msg_xy_um3_no_color);
            } else {
                m734 = MSG(l_msg_xy_moon);
            }
            break;
        case NPC_SA5:
            m734 = MSG(l_msg_xy_sa5_no_skull_necklace);
            break;
        }
    } else if ((u32)(gabi::load<u8>(play + 0x52B0) - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
        u32 itemNo = dComIfGp_event_getPreItemNo();
        switch (mNpcNo) {
        case NPC_UB1:
        case NPC_UB2:
            msgNo = 0x2C9D;
            break;
        case NPC_UB3:
            msgNo = 0x2CF7;
            break;
        case NPC_UB4:
            msgNo = 0x2D63;
            break;
        case NPC_SA5:
            if (itemNo != dItemNo_SKULL_NECKLACE_e) {
                m734 = MSG(l_msg_xy_sa5_no_skull_necklace);
            } else if (!dComIfGs_isEventBit(0x2620)) {
                m734 = MSG(l_msg_xy_sa5_1st);
            } else {
                m734 = MSG(l_msg_xy_sa5_2nd);
            }
            break;
        }
    } else {
        switch (mNpcNo) {
        case NPC_UO1:
            if (!dComIfGs_checkGetItem(dItemNo_DELIVERY_BAG_e)) {
                if (!dComIfGs_isEventBit(0x2501)) {
                    dComIfGs_onEventBit(0x2501);
                    m734 = MSG(l_msg_uo1_1st_haitatu);
                } else {
                    m734 = MSG(l_msg_uo1_haitatu);
                }
            } else if (!dComIfGs_isEventBit(0x1B40)) {
                dComIfGs_onEventBit(0x1B40);
                m734 = MSG(l_msg_uo1_1st_talk);
            } else if (isUo1FdaiAll()) {
                if (!dComIfGs_isEventBit(0x1B10)) {
                    m734 = MSG(l_msg_uo1_1st_talk_fdai);
                    dComIfGs_onEventBit(0x1B10);
                } else {
                    m734 = MSG(l_msg_uo1_2nd_talk_fdai);
                }
            } else if (isUo1FdaiOne()) {
                m734 = MSG(l_msg_uo1_fadi1);
            } else {
                m734 = MSG(l_msg_uo1_fdai2);
            }
            break;
        case NPC_UO2:
            if (m79A & 0x1) {
                m79A = m79A & ~0x1;
                m734 = MSG(l_msg_uo2_surprise);
            } else if (dComIfGs_isEventBit(0x2D01)) {
                m734 = MSG(l_msg_uo2_help);
            } else if (!dComIfGs_isEventBit(0x1B04)) {
                dComIfGs_onEventBit(0x1B04);
                m734 = MSG(l_msg_uo2_1st_talk);
            } else {
                m734 = MSG(l_msg_uo2_2nd_talk);
            }
            break;
        case NPC_UO3:
            if (m793 == 3 || (mEtcFlag & 0x8)) {
                m734 = MSG(l_msg_uo3_kyoro);
            } else if (m793 == 4) {
                m734 = MSG(l_msg_uo3_letter);
            } else if (!dComIfGs_isEventBit(0x1E08)) {
                dComIfGs_onEventBit(0x1E08);
                m734 = MSG(l_msg_uo3_1st_talk);
            } else if (!(mEtcFlag & 0x2)) {
                m734 = MSG(l_msg_uo3_befor_letter);
            } else if (mEtcFlag & 0x4) {
                m734 = MSG(l_msg_uo3_retry_letter);
            } else {
                m734 = MSG(l_msg_uo3_after_letter);
            }
            break;
        case NPC_UB3:
            if (!dComIfGs_isEventBit(0x1D20)) {
                m734 = MSG(l_msg_ub3_1st_talk);
                dComIfGs_onEventBit(0x1D20);
            } else {
                fopAc_ac_c* pShip = dComIfGp_getShipActor();
                gabi::Local<be<f32>> temp;
                *temp = 9999.9f;
                if (pShip != nullptr) {
                    gabi::Local<cXyz> a;
                    gabi::Local<cXyz> b;
                    f32 z = current.pos.z, x = current.pos.x, y = current.pos.y;
                    a->z = z;
                    a->x = x;
                    a->y = y;
                    b->x = (f32)pShip->current.pos.x;
                    b->y = (f32)pShip->current.pos.y;
                    b->z = (f32)pShip->current.pos.z;
                    dNpc_calc_DisXZ_AngY(a, b, (f32*)temp.get(), nullptr);
                    if (!(*temp > 500.0f)) {
                        m734 = MSG(l_msg_ub3_ship_near5);
                        break;
                    }
                }
                if (dComIfGs_checkGetItem(dItemNo_WIND_WAKER_e)) {
                    m734 = MSG(l_msg_ub3_tact);
                } else {
                    m734 = MSG(l_msg_ub3_talk);
                }
            }
            break;
        case NPC_UB4:
            if (mEtcFlag & 0x40) {
                m734 = MSG(l_msg_ub4_photo_house);
            } else if (!dComIfGs_isEventBit(0x1D10)) {
                m734 = MSG(l_msg_ub4_1st_talk);
                dComIfGs_onEventBit(0x1D10);
            } else if (dComIfGs_isEventBit(0x2504)) {
                m734 = MSG(l_msg_ub4_color_photo);
            } else if (!dComIfGs_checkGetItem(dItemNo_PICTO_BOX_e) && !dComIfGs_checkGetItem(dItemNo_DELUXE_PICTO_BOX_e)) {
                m734 = MSG(l_msg_ub4_no_photo_box);
            } else {
                m734 = MSG(l_msg_ub4_photo_box);
            }
            break;
        case NPC_UW1:
            if (people_endlessNight()) {
                msgNo = 0x2DC9;
            } else if (!mbIsNight) {
                if (!dComIfGs_isEventBit(0x1E20)) {
                    m734 = MSG(l_msg_uw1_1st_talk_day);
                    dComIfGs_onEventBit(0x1E20);
                } else {
                    m734 = MSG(l_msg_uw1_talk_day);
                }
            } else if (!dComIfGs_isEventBit(0x1E10)) {
                m734 = MSG(l_msg_uw1_1st_talk_night);
                dComIfGs_onEventBit(0x1E10);
            } else if (dComIfGs_checkGetItem(dItemNo_DELIVERY_BAG_e)) {
                if (dComIfGs_checkGetItem(dItemNo_MAGIC_ARMOR_e)) {
                    m734 = MSG(l_msg_uw1_magic_shield);
                } else {
                    m734 = MSG(l_msg_uw1_no_magic_shield);
                }
            } else {
                m734 = MSG(l_msg_uw1_talk_night);
            }
            break;
        case NPC_UW2:
            if (dComIfGs_getEventReg(UNK_B907) < 2) {
                if (!dComIfGs_isEventBit(0x2101)) {
                    m734 = MSG(l_msg_uw2_1st_talk1);
                    dComIfGs_onEventBit(0x2101);
                } else if (dComIfGs_getEventReg(UNK_C407) < 6) {
                    m734 = MSG(l_msg_uw2_no_photo3);
                } else if (dComIfGs_getEventReg(UNK_C407) < 7) {
                    m734 = MSG(l_msg_uw2_no_1day_photo3);
                } else if (!dComIfGs_isEventBit(0x2240)) {
                    m734 = MSG(l_msg_uw2_request);
                } else if (!dComIfGs_isEventBit(0x2220)) {
                    m734 = MSG(l_msg_uw2_cafe_off);
                } else {
                    m734 = MSG(l_msg_uw2_cafe_on);
                    dComIfGs_setEventReg(UNK_B907, 1);
                }
            } else if (dComIfGs_getEventReg(UNK_B907) < 4) {
                if (!dComIfGs_isEventBit(0x2280)) {
                    m734 = MSG(l_msg_uw2_1st_talk2);
                    dComIfGs_onEventBit(0x2280);
                    dComIfGs_setEventReg(UNK_B907, 3);
                } else {
                    m734 = MSG(l_msg_uw2_talk2);
                }
            } else {
                m734 = MSG(l_msg_uw2_talk3);
            }
            break;
        case NPC_UM1:
            if (people_endlessNight()) {
                msgNo = 0x2FBD;
            } else if (!dComIfGs_isEventBit(0x2104)) {
                m734 = MSG(l_msg_um1_frrs_stop);
            } else if (!dComIfGs_isEventBit(0x2210)) {
                m734 = MSG(l_msg_um1_1st_talk);
                dComIfGs_onEventBit(0x2210);
            } else if (!dComIfGs_isEventBit(0x1C02)) {
                m734 = MSG(l_msg_um1_light_off);
            } else if (!dComIfGs_isEventBit(0x1B20)) {
                m734 = MSG(l_msg_um1_get_item);
                dComIfGs_onEventBit(0x1B20);
            } else {
                m734 = MSG(l_msg_um1_last);
            }
            break;
        case NPC_UM2:
            if (dComIfGs_getEventReg(UNK_B907) < 2) {
                if (!dComIfGs_isEventBit(0x2208)) {
                    m734 = MSG(l_msg_um2_1st_talk1);
                    dComIfGs_onEventBit(0x2208);
                } else if (dComIfGs_getEventReg(UNK_C407) < 7) {
                    m734 = MSG(l_msg_um2_no_1day_photo3);
                } else if (dComIfGs_isEventBit(0x2220)) {
                    m734 = MSG(l_msg_um2_cafe_on);
                    dComIfGs_setEventReg(UNK_B907, 1);
                } else if (!dComIfGs_isEventBit(0x2240)) {
                    m734 = MSG(l_msg_um2_no_request);
                } else {
                    m734 = MSG(l_msg_um2_cafe_off);
                }
            } else if (dComIfGs_getEventReg(UNK_B907) < 4) {
                if (!dComIfGs_isEventBit(0x2204)) {
                    m734 = MSG(l_msg_um2_1st_talk2);
                    dComIfGs_onEventBit(0x2204);
                } else {
                    m734 = MSG(l_msg_um2_talk2);
                }
            } else {
                m734 = MSG(l_msg_um2_talk3);
            }
            break;
        case NPC_UM3:
            if (people_endlessNight()) {
                msgNo = 0x347B;
            } else if (!mbIsNight) {
                if (!people_haveSail()) {
                    m734 = MSG(l_msg_um3_not_sail);
                } else if (!dComIfGs_isEventBit(0x2340)) {
                    m734 = MSG(l_msg_um3_1st_talk);
                    dComIfGs_onEventBit(0x2340);
                } else if (!dComIfGs_isEventBit(0x2310)) {
                    m734 = MSG(l_msg_um3_no_nazo_talk);
                } else if (dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e)) {
                    m734 = MSG(l_msg_um3_map15);
                } else {
                    m734 = MSG(l_msg_um3_no_map15);
                }
            } else if (!dComIfGs_isEventBit(0x2320)) {
                m734 = MSG(l_msg_um3_1st_night);
                dComIfGs_onEventBit(0x2320);
            } else if (!dComIfGs_isEventBit(0x2308)) {
                m734 = MSG(l_msg_um3_no_look_moon);
            } else if (dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e) && dComIfGs_isEventBit(0x2280)) {
                m734 = MSG(l_msg_um3_map15_n);
            } else {
                m734 = MSG(l_msg_um3_no_map15_n);
            }
            break;
        case NPC_SA1:
            if (people_endlessNight()) {
                msgNo = 0x2E82;
            } else if (!dComIfGs_isEventBit(0x2304)) {
                m734 = MSG(l_msg_sa1_1st_talk);
                dComIfGs_onEventBit(0x2304);
            } else {
                m734 = MSG(l_msg_sa1_talk);
            }
            break;
        case NPC_SA2:
            if (!mbIsNight) {
                if (!dComIfGs_isEventBit(0x2302)) {
                    m734 = MSG(l_msg_sa2_1st_talk);
                    dComIfGs_onEventBit(0x2302);
                } else if (getWindDir() == 6) {
                    m734 = MSG(l_msg_sa2_wind_west);
                } else if (getWindDir() == 2) {
                    m734 = MSG(l_msg_sa2_wind_east);
                } else {
                    m734 = MSG(l_msg_sa2_wind_not_west);
                }
            } else {
                m734 = MSG(l_msg_sa2_night);
            }
            break;
        case NPC_SA3:
            if (people_endlessNight()) {
                msgNo = 0x30E4;
            } else if (!mbIsNight) {
                if (!people_haveSail()) {
                    m734 = MSG(l_msg_sa3_not_sail);
                } else if (!dComIfGs_isEventBit(0x2301)) {
                    m734 = MSG(l_msg_sa3_1st_talk);
                    dComIfGs_onEventBit(0x2301);
                } else {
                    m734 = MSG(gabi::load<u32>(l_msg_sa3_fdai + chkDaiza() * 4));
                }
            } else {
                m734 = MSG(l_msg_sa3_night);
            }
            break;
        case NPC_SA4:
            if (people_endlessNight()) {
                msgNo = 0x321E;
            } else if (!mbIsNight) {
                if (!people_haveSail()) {
                    m734 = MSG(l_msg_sa4_not_sail);
                } else if (!dComIfGs_isEventBit(0x2480)) {
                    m734 = MSG(l_msg_sa4_1st_talk);
                    dComIfGs_onEventBit(0x2480);
                } else {
                    m734 = MSG(gabi::load<u32>(l_msg_sa4_wind + getWindDir() * 4));
                }
            } else {
                m734 = MSG(l_msg_sa4_night);
            }
            break;
        case NPC_SA5:
            if (people_endlessNight()) {
                msgNo = 0x3299;
            } else if (!mbIsNight) {
                if (mEtcFlag & 0x80) {
                    mEtcFlag = mEtcFlag & ~0x80;
                    if (!dComIfGs_isEventBit(0x2680)) {
                        m734 = MSG(l_msg_sa5__next_day);
                    } else if (getPigTimer() == 0) {
                        m734 = MSG(l_msg_sa5_timer_zero);
                    } else if (dComIfGs_getTmpReg(UNK_FE03) == 0) {
                        if (isPigOk()) {
                            m734 = MSG(l_msg_sa5_mini_game_clr);
                            dComIfGs_onEventBit(0x2A04);
                        } else if (!dComIfGs_isTmpBit(0x0240)) {
                            u8 reg = dComIfGs_getTmpReg(UNK_FF03);
                            /* HD: the JUT_ASSERT's failure path skips the table read */
                            if (reg < 3) {
                                m734 = MSG(gabi::load<u32>(l_msg_sa5_false_1st + reg * 4));
                            } else {
                                JUT_ASSERT_fail(STR(0x10020874), 0x2113, STR(0x10020888));
                            }
                            dComIfGs_onTmpBit(0x0240);
                        } else {
                            m734 = MSG(l_msg_sa5_false_2nd);
                        }
                    } else {
                        u8 reg = dComIfGs_getTmpReg(UNK_FE03);
                        switch (reg) {
                        case 3:
                            m734 = MSG(l_msg_xy_pig1);
                            break;
                        case 2:
                            m734 = MSG(l_msg_xy_pig2);
                            break;
                        case 1:
                            m734 = MSG(l_msg_xy_sa5_mini_game2_clr);
                            dComIfGs_offEventBit(0x2680);
                            break;
                        }
                        reg -= 1;
                        dComIfGs_setTmpReg(UNK_FE03, reg);
                    }
                } else if (!dComIfGs_isEventBit(0x2440)) {
                    m734 = MSG(l_msg_sa5_1st_talk);
                    dComIfGs_onEventBit(0x2440);
                } else if (dComIfGs_isTmpBit(0x0280)) {
                    if (!dComIfGs_isEventBit(0x2680)) {
                        m734 = MSG(l_msg_sa5__next_day);
                    } else if (getPigTimer() == 0) {
                        m734 = MSG(l_msg_sa5_timer_zero);
                    } else {
                        u8 reg = dComIfGs_getTmpReg(UNK_FF03);
                        if (reg < 4) {
                            m734 = MSG(gabi::load<u32>(l_msg_sa5_play + reg * 4));
                        } else {
                            JUT_ASSERT_fail(STR(0x10020874), 0x2151, STR(0x100208EC));
                        }
                    }
                } else if (dComIfGs_isEventBit(0x2680)) {
                    m734 = MSG(l_msg_sa5_day);
                } else if ((u32)dComIfGs_getRupee() > 80) {
                    m734 = MSG(l_msg_sa5_50rupee);
                } else if (!dComIfGs_isEventBit(0x2640)) {
                    m734 = MSG(l_msg_sa5_1st);
                } else {
                    m734 = MSG(l_msg_sa5_2nd);
                }
            } else {
                m734 = MSG(l_msg_sa5_night);
            }
            break;
        case NPC_UG1:
            if (!(people_homeDistXZ(this) < 200.0f)) {
                m734 = MSG(l_msg_ug1_out_area);
            } else if (!dComIfGs_isEventBit(0x2880)) {
                m734 = MSG(l_msg_ug1_1st_talk);
                dComIfGs_onEventBit(0x2880);
            } else if (dComIfGs_isEventBit(0x2B04)) {
                m734 = MSG(l_msg_ug1_hint_talk);
            } else {
                m734 = MSG(l_msg_ug1_hint_talk_today);
            }
            break;
        case NPC_UG2:
            if (!(people_homeDistXZ(this) < 200.0f)) {
                m734 = MSG(l_msg_ug2_out_area);
            } else if (!dComIfGs_isEventBit(0x2B08)) {
                m734 = MSG(l_msg_ug2_1st_talk);
                dComIfGs_onEventBit(0x2B08);
            } else if (dComIfGs_isEventBit(0x2B04)) {
                m734 = MSG(l_msg_ug2_hint_talk);
            } else {
                m734 = MSG(l_msg_ug2_hint_talk_today);
            }
            break;
        }
    }

    if (m734.get() != nullptr) {
        msgNo = *m734;
    }
    return msgNo;
}
VERIFY(0x022C6C68, &daNpcPeople_c::getMsg);
