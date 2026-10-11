/**
 * d_a_npc_people_msg.cpp (WWHD)
 * NPC - Windfall townspeople: message selection (getMsg, getMsg3) and its helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_people_msg.h"

enum {
    dItemNo_DELIVERY_BAG_e = 0x30, dItemNo_WIND_WAKER_e = 0x22, dItemNo_PICTO_BOX_e = 0x23,
    dItemNo_DELUXE_PICTO_BOX_e = 0x26, dItemNo_MAGIC_ARMOR_e = 0x2A, dItemNo_SKULL_NECKLACE_e = 0x45,
    dItemNo_PEARL_NAYRU_e = 0x69, dItemNo_SAIL_e = 0x78, dItemNo_SWIFT_SAIL_e = 0x77 /* HD */,
    dItemNo_COLLECT_MAP_16_e = 0xEF, dItemNo_COLLECT_MAP_15_e = 0xF0,
};
enum { ENDLESS_NIGHT = 0x0A02, UNK_B907 = 0xB907, UNK_C407 = 0xC407, UNK_FE03 = 0xFE03, UNK_FF03 = 0xFF03 };

// GameCube message tables, with the HD data checked by the glue.
// The four pointer tables are audited against the image separately because
// the glue checker cannot evaluate named array pointers.
static u32 l_msg_uo1_1st_haitatu[] = {0x2B69, 0x2B6A, 0x0};
static u32 l_msg_uo1_haitatu[] = {0x2B6A, 0x0};
static u32 l_msg_uo1_1st_talk[] = {0x2B5D, 0x2B5E, 0x2B5F, 0x0};
static u32 l_msg_uo1_1st_talk_fdai[] = {0x2B61, 0x2B62, 0x2B63, 0x1, 0x0};
static u32 l_msg_uo1_2nd_talk_fdai[] = {0x2B60, 0x0};
static u32 l_msg_uo1_fadi1[] = {0x2B65, 0x2B66, 0x0};
static u32 l_msg_uo1_fdai2[] = {0x2B67, 0x2B68, 0x0};
static u32 l_msg_uo2_surprise[] = {0x2BC1, 0x0};
static u32 l_msg_uo2_help[] = {0x2BC2, 0x0};
static u32 l_msg_uo2_1st_talk[] = {0x2BC4, 0x2BC5, 0x2BC6, 0x0};
static u32 l_msg_uo2_2nd_talk[] = {0x2BC3, 0x0};
static u32 l_msg_uo3_kyoro[] = {0x2C2E, 0x0};
static u32 l_msg_uo3_letter[] = {0x2C2F, 0x0};
static u32 l_msg_uo3_1st_talk[] = {0x2C25, 0x2C26, 0x2C27, 0x0};
static u32 l_msg_uo3_befor_letter[] = {0x2C28, 0x0};
static u32 l_msg_uo3_retry_letter[] = {0x2C2A, 0x0};
static u32 l_msg_uo3_after_letter[] = {0x2C29, 0x0};
static u32 l_msg_xy_uo3_photo[] = {0x2C2B, 0x2C2C, 0x0};
static u32 l_msg_xy_uo3_no_photo[] = {0x2C2D, 0x0};
static u32 l_msg_ub3_1st_talk[] = {0x2CEE, 0x2CEF, 0x2CF0, 0x0};
static u32 l_msg_ub3_ship_near5[] = {0x2CF3, 0x0};
static u32 l_msg_ub3_tact[] = {0x2CF4, 0x2CF5, 0x2CF6, 0x0};
static u32 l_msg_ub3_talk[] = {0x2CF1, 0x2CF2, 0x0};
static u32 l_msg_ub4_photo_house[] = {0x2D5B, 0x2D5C, 0x0};
static u32 l_msg_ub4_1st_talk[] = {0x2D57, 0x0};
static u32 l_msg_ub4_color_photo[] = {0x2D59, 0x0};
static u32 l_msg_ub4_no_photo_box[] = {0x2D58, 0x0};
static u32 l_msg_ub4_photo_box[] = {0x2D58, 0x2D5A, 0x0};
static u32 l_msg_xy_ub4_no_photo[] = {0x2D51, 0x0};
static u32 l_msg_xy_ub4_no_color[] = {0x2D52, 0x0};
static u32 l_msg_xy_ub4_get_item[] = {0x2D54, 0x2D55, 0x2, 0x0};
static u32 l_msg_xy_ub4_talk[] = {0x2D53, 0x0};
static u32 l_msg_uw1_1st_talk_day[] = {0x2DB5, 0x2DB6, 0x0};
static u32 l_msg_uw1_talk_day[] = {0x2DB7, 0xB};
static u32 l_msg_uw1_1st_talk_night[] = {0x2DC3, 0x2DC4, 0x0};
static u32 l_msg_uw1_magic_shield[] = {0x2DC7, 0x2DC8, 0x0};
static u32 l_msg_uw1_no_magic_shield[] = {0x2DC5, 0x2DC6, 0x0};
static u32 l_msg_uw1_talk_night[] = {0x2DC1, 0x2DC2, 0x0};
static u32 l_msg_uw2_1st_talk1[] = {0x2E19, 0x2E1A, 0x0};
static u32 l_msg_uw2_no_photo3[] = {0x2E1B, 0x2E1C, 0x0};
static u32 l_msg_uw2_no_1day_photo3[] = {0x2E1D, 0x2E1E, 0x0};
static u32 l_msg_uw2_request[] = {0x2E1F, 0x2E20, 0x2E21, 0xC};
static u32 l_msg_uw2_cafe_off[] = {0x2E24, 0x2E25, 0x0};
static u32 l_msg_uw2_cafe_on[] = {0x2E26, 0x0};
static u32 l_msg_uw2_1st_talk2[] = {0x2E27, 0x2E28, 0x3, 0x0};
static u32 l_msg_uw2_talk2[] = {0x2E2A, 0x0};
static u32 l_msg_uw2_talk3[] = {0x2E2B, 0x0};
static u32 l_msg_um1_frrs_stop[] = {0x2FAA, 0x2FAB, 0xD, 0x0};
static u32 l_msg_um1_1st_talk[] = {0x2FB7, 0x2FB8, 0x0};
static u32 l_msg_um1_light_off[] = {0x2FB9, 0x2FBA, 0x2FBB, 0x0};
static u32 l_msg_um1_get_item[] = {0x2FB4, 0x2FB5, 0x4, 0x0};
static u32 l_msg_um1_last[] = {0x2FBC, 0x0};
static u32 l_msg_um2_1st_talk1[] = {0x300D, 0x300E, 0x300F, 0x0};
static u32 l_msg_um2_no_1day_photo3[] = {0x3010, 0x3011, 0x3012, 0x0};
static u32 l_msg_um2_cafe_on[] = {0x3016, 0x3017, 0x0};
static u32 l_msg_um2_no_request[] = {0x3013, 0x0};
static u32 l_msg_um2_cafe_off[] = {0x3014, 0x3015, 0x0};
static u32 l_msg_um2_1st_talk2[] = {0x3018, 0x0};
static u32 l_msg_um2_talk2[] = {0x3019, 0x0};
static u32 l_msg_um2_talk3[] = {0x301A, 0x0};
static u32 l_msg_xy_um2_talk1[] = {0x301B, 0x301C, 0x301D, 0x0};
static u32 l_msg_xy_um2_talk2[] = {0x301B, 0x301E, 0x0};
static u32 l_msg_xy_um2_talk3[] = {0x301B, 0x301F, 0x3020, 0x0};
static u32 l_msg_xy_um2_talk4[] = {0x301B, 0x3021, 0x3022, 0x3023, 0x3024, 0x0};
static u32 l_msg_xy_um2_talk5[] = {0x301B, 0x3025, 0x0};
static u32 l_msg_um3_not_sail[] = {0x347A, 0x0};
static u32 l_msg_um3_1st_talk[] = {0x3461, 0x3462, 0x3463, 0x0};
static u32 l_msg_um3_no_nazo_talk[] = {0x3464, 0x3465, 0xE};
static u32 l_msg_um3_map15[] = {0x346A, 0x0};
static u32 l_msg_um3_no_map15[] = {0x3468, 0x3469, 0x0};
static u32 l_msg_um3_1st_night[] = {0x3459, 0x345A, 0x0};
static u32 l_msg_um3_no_look_moon[] = {0x345B, 0x345C, 0x0};
static u32 l_msg_um3_map15_n[] = {0x345E, 0x345F, 0x3460, 0x0};
static u32 l_msg_um3_no_map15_n[] = {0x345D, 0x0};
static u32 l_msg_xy_um3_ng[] = {0x346B, 0x0};
static u32 l_msg_xy_um3_sun[] = {0x346C, 0x0};
static u32 l_msg_xy_um3_no_full_moon[] = {0x346D, 0x0};
static u32 l_msg_xy_um3_no_color[] = {0x346E, 0x0};
static u32 l_msg_xy_moon[] = {0x346F, 0x3470, 0x3471, 0x5, 0x3472, 0x3473, 0x0};
static u32 l_msg_look_full_moon[] = {0x3474, 0x3475, 0x0};
static u32 l_msg_look_moon[] = {0x3476, 0x3477, 0x0};
static u32 l_msg_look_orion[] = {0x3478, 0x0};
static u32 l_msg_look_hokuto[] = {0x3479, 0x0};
static u32 l_msg_sa1_1st_talk[] = {0x2E7D, 0x2E7E, 0x2E7F, 0x2E80, 0x0};
static u32 l_msg_sa1_talk[] = {0x2E81, 0x0};
static u32 l_msg_sa2_1st_talk[] = {0x3071, 0x0};
static u32 l_msg_sa2_wind_west[] = {0x3072, 0x3073, 0x3074, 0x0};
static u32 l_msg_sa2_wind_east[] = {0x307C, 0x307D, 0x0};
static u32 l_msg_sa2_wind_not_west[] = {0x3075, 0x3076, 0x0};
static u32 l_msg_sa2_night[] = {0x3201, 0x0};
static u32 l_msg_sa3_not_sail[] = {0x30E2, 0x30E3, 0x0};
static u32 l_msg_sa3_1st_talk[] = {0x30D5, 0x30D6, 0x0};
static u32 l_msg_sa3_fdai_0[] = {0x30D7, 0x0};
static u32 l_msg_sa3_fdai_1[] = {0x30D7, 0x30D8, 0x6, 0x0, 0x30DF, 0x7, 0x30E0, 0x0};
static u32 l_msg_sa3_fdai_2[] = {0x30D7, 0x30D9, 0x6, 0x1, 0x30DF, 0x7, 0x30E0, 0x0};
static u32 l_msg_sa3_fdai_3[] = {0x30D7, 0x30DA, 0x0};
static u32 l_msg_sa3_fdai_4[] = {0x30D7, 0x30DB, 0x6, 0x2, 0x30DF, 0x7, 0x30E0, 0x0};
static u32 l_msg_sa3_fdai_5[] = {0x30D7, 0x30DC, 0x6, 0x3, 0x30DF, 0x7, 0x30E0, 0x0};
static u32 l_msg_sa3_fdai_6[] = {0x30D7, 0x30DD, 0x0};
static u32 l_msg_sa3_fdai_7[] = {0x30D7, 0x30DE, 0x0};
static u32 l_msg_sa3_night[] = {0x30E1, 0x0};
static u32 l_msg_sa4_not_sail[] = {0x321D, 0x0};
static u32 l_msg_sa4_1st_talk[] = {0x3202, 0x3203, 0x0};
static u32 l_msg_sa4_wind_0[] = {0x3215, 0x3216, 0x8, 0x0, 0x3213, 0x0, 0x3214, 0x0};
static u32 l_msg_sa4_wind_1[] = {0x321A, 0x321B, 0x321C, 0x0};
static u32 l_msg_sa4_wind_2[] = {0x3210, 0x3211, 0x8, 0x1, 0x3213, 0x0, 0x3212, 0x0};
static u32 l_msg_sa4_wind_3[] = {0x320A, 0x320B, 0x320C, 0x0};
static u32 l_msg_sa4_wind_4[] = {0x3204, 0x3205, 0x3206, 0x0};
static u32 l_msg_sa4_wind_5[] = {0x3207, 0x3208, 0x3209, 0x0};
static u32 l_msg_sa4_wind_6[] = {0x320D, 0x320E, 0x320F, 0x0};
static u32 l_msg_sa4_wind_7[] = {0x3217, 0x3218, 0x3219, 0x0};
static u32 l_msg_sa4_night[] = {0x3077, 0xF};
static u32 l_msg_sa5__next_day[] = {0x3286, 0x17};
static u32 l_msg_sa5_timer_zero[] = {0x3285, 0x17};
static u32 l_msg_sa5_mini_game_clr[] = {0x3287, 0x3288, 0x3289, 0x9, 0x328A, 0x17};
static u32 l_msg_sa5_false_1st1[] = {0x3293, 0x328C, 0x0};
static u32 l_msg_sa5_false_1st2[] = {0x3294, 0x328C, 0x0};
static u32 l_msg_sa5_false_1st3[] = {0x3295, 0x328C, 0x0};
static u32 l_msg_sa5_false_2nd[] = {0x328D, 0x328E, 0x17};
static u32 l_msg_xy_pig1[] = {0x3291, 0x0};
static u32 l_msg_xy_pig2[] = {0x3292, 0x0};
static u32 l_msg_xy_sa5_mini_game2_clr[] = {0x328F, 0x3289, 0xA, 0x3290, 0x17};
static u32 l_msg_sa5_1st_talk[] = {0x3266, 0x3267, 0x0};
static u32 l_msg_sa5_play0[] = {0x3268, 0x3296, 0x0};
static u32 l_msg_sa5_play1[] = {0x3268, 0x3297, 0x0};
static u32 l_msg_sa5_play2[] = {0x3268, 0x3298, 0x0};
static u32 l_msg_sa5_play3[] = {0x3268, 0x0};
static u32 l_msg_sa5_day[] = {0x3269, 0x0};
static u32 l_msg_sa5_50rupee[] = {0x326A, 0x0};
static u32 l_msg_sa5_1st[] = {0x326B, 0x326C, 0x10};
static u32 l_msg_sa5_2nd[] = {0x326B, 0x326D, 0x10};
static u32 l_msg_sa5_night[] = {0x3265, 0x0};
static u32 l_msg_xy_sa5_no_skull_necklace[] = {0x3279, 0x0};
static u32 l_msg_xy_sa5_1st[] = {0x327B, 0x327C, 0x327D, 0x11};
static u32 l_msg_xy_sa5_2nd[] = {0x327A, 0x327D, 0x11};
static u32 l_msg_ug1_out_area[] = {0x359C, 0x0};
static u32 l_msg_ug1_1st_talk[] = {0x3585, 0x3586, 0x0};
static u32 l_msg_ug1_hint_talk[] = {0x3587, 0x0};
static u32 l_msg_ug1_hint_talk_today[] = {0x3588, 0x0};
static u32 l_msg_ug2_out_area[] = {0x359D, 0x0};
static u32 l_msg_ug2_1st_talk[] = {0x358F, 0x3590, 0x0};
static u32 l_msg_ug2_hint_talk[] = {0x3591, 0x0};
static u32 l_msg_ug2_hint_talk_today[] = {0x3592, 0x3593, 0x0};
static PeopleMessageWord* l_msg_sa3_fdai[] = {l_msg_sa3_fdai_0, l_msg_sa3_fdai_1, l_msg_sa3_fdai_2, l_msg_sa3_fdai_3, l_msg_sa3_fdai_4, l_msg_sa3_fdai_5, l_msg_sa3_fdai_6, l_msg_sa3_fdai_7};
static PeopleMessageWord* l_msg_sa4_wind[] = {l_msg_sa4_wind_0, l_msg_sa4_wind_1, l_msg_sa4_wind_2, l_msg_sa4_wind_3, l_msg_sa4_wind_4, l_msg_sa4_wind_5, l_msg_sa4_wind_6, l_msg_sa4_wind_7};
static PeopleMessageWord* l_msg_sa5_false_1st[] = {l_msg_sa5_false_1st1, l_msg_sa5_false_1st2, l_msg_sa5_false_1st3};
static PeopleMessageWord* l_msg_sa5_play[] = {l_msg_sa5_play0, l_msg_sa5_play1, l_msg_sa5_play2, l_msg_sa5_play3};
static u16 l_daiza_no_tbl[] = {0xF8FF, 0xF7FF, 0xF6FF, 0xF5FF, 0xF4FF, 0xF3FF};

/* 022C60D0 */
int daNpcPeople_c::getWindDir() {
    cXyz* wind = dKyw_get_wind_vec();
    u16 angle = cM_atan2s(wind->x, wind->z);
    angle += 0x1000;
    angle /= 0x2000;
    return angle & 7;
}

/* 022C6910 */
// HD: use the picture colour flag instead of the GX picture format.
BOOL daNpcPeople_c::isColor() {
    return peoplePicture()->mIsColor != 0;
}

/* 022C693C */
BOOL daNpcPeople_c::isUo1FdaiAll() {
    u32 max = daDai_getMaxDaiza();
    return max == daDai_getDaizaSetItemNum() ? TRUE : FALSE;
}

/* 022C6978 */
BOOL daNpcPeople_c::isUo1FdaiOne() {
    return daDai_getDaizaSetItemNum() != 0 ? TRUE : FALSE;
}

/* 022C69A0 */
s32 daNpcPeople_c::chkDaiza() {
    
    int i;
    for (i = 0; i < 6; i++) {
        if (dComIfGs_getEventReg(l_daiza_no_tbl[i]) == 0x97) {
            return 1;
        }
    }
    for (i = 0; i < 6; i++) {
        if (dComIfGs_getEventReg(l_daiza_no_tbl[i]) == 0x96) {
            return 2;
        }
    }
    int temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = dComIfGs_getEventReg(l_daiza_no_tbl[i]);
        if (reg == 0x8C || reg == 0x8D || reg == 0x8E) {
            temp++;
        }
    }
    if (temp == 6) {
        return 3;
    }
    temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = dComIfGs_getEventReg(l_daiza_no_tbl[i]);
        if (reg == 0x8F || reg == 0x90 || reg == 0x91 || reg == 0x92 || reg == 0x93) {
            temp++;
        }
    }
    if (temp == 6) {
        return 4;
    }
    temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = dComIfGs_getEventReg(l_daiza_no_tbl[i]);
        if (reg == 0x94 || reg == 0x95) {
            temp++;
        }
    }
    if (temp == 6) {
        return 5;
    }
    temp = 0;
    for (i = 0; i < 6; i++) {
        u8 reg = dComIfGs_getEventReg(l_daiza_no_tbl[i]);
        if (reg != 0) {
            temp++;
        }
    }
    if (temp >= 2) {
        return 6;
    }
    if (temp >= 1) {
        return 7;
    }
    return 0;
}

/* 022C1F20 */
u32 daNpcPeople_c::getMsg3() {
    u32 msgNo = 0;
    m734 = nullptr;
    switch (mNpcNo) {
    case NPC_UM3:
        switch (m7A7) {
        case 0:
        default:
            if (dKy_moon_type_chk() == 0) {
                m734 = l_msg_look_full_moon;
            } else {
                m734 = l_msg_look_moon;
            }
            break;
        case 1:
            m734 = l_msg_look_orion;
            break;
        case 2:
            m734 = l_msg_look_hokuto;
            break;
        }
    }
    if (m734.get() != nullptr) {
        msgNo = *m734;
    }
    return msgNo;
}

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
    peopleWords(diff.get())->x = static_cast<u32>(peopleWords(tmp.get())->x);
    peopleWords(diff.get())->z = static_cast<u32>(peopleWords(tmp.get())->z);
    diff->y = 0.0f;
    return std_sqrtf(PSVECSquareMag(diff));
}

/* 022C6C68 */
u32 daNpcPeople_c::getMsg() {
    u32 msgNo = 0;
    m734 = nullptr;

    /* HD: dEvt_control_c::chkPhoto is play + 0x52B2 (the play object is fetched twice) */
    u8 photo = peopleGame()->play.mEvtCtrl.mbInPhoto;
    dComIfG_inf_c* play = peopleGame();
    if (photo) {
        switch (mNpcNo) {
        case NPC_UO3:
            if (dComIfGp_getPictureResult() == 1) {
                m734 = l_msg_xy_uo3_photo;
            } else {
                m734 = l_msg_xy_uo3_no_photo;
            }
            break;
        case NPC_UB4:
            if (dComIfGp_getPictureResult() != 5 && dComIfGp_getPictureResult() != 6 &&
                dComIfGp_getPictureResult() != 0x6D) {
                m734 = l_msg_xy_ub4_no_photo;
            } else if (!isColor()) {
                m734 = l_msg_xy_ub4_no_color;
            } else if (!dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_16_e)) {
                m734 = l_msg_xy_ub4_get_item;
                dComIfGs_onEventBit(0x2504);
            } else {
                m734 = l_msg_xy_ub4_talk;
            }
            break;
        case NPC_UM2:
            if (dComIfGp_getPictureResult() != 0x67) { /* HD: 0x67 (GameCube 4) */
                m734 = l_msg_xy_um2_talk1;
            } else if (dComIfGp_getPictureResultDetail() != 0) {
                m734 = l_msg_xy_um2_talk2;
            } else if (!isColor()) {
                m734 = l_msg_xy_um2_talk3;
            } else if (!dComIfGs_isEventBit(0x2220)) {
                m734 = l_msg_xy_um2_talk4;
            } else {
                m734 = l_msg_xy_um2_talk5;
            }
            break;
        case NPC_UM3:
            if (dComIfGp_getPictureResult() != 7) {
                if (dComIfGp_getPictureResult() == 8) {
                    m734 = l_msg_xy_um3_no_full_moon;
                } else if (dComIfGp_getPictureResult() == 9) {
                    m734 = l_msg_xy_um3_sun;
                } else {
                    m734 = l_msg_xy_um3_ng;
                }
            } else if (!isColor()) {
                m734 = l_msg_xy_um3_no_color;
            } else {
                m734 = l_msg_xy_moon;
            }
            break;
        case NPC_SA5:
            m734 = l_msg_xy_sa5_no_skull_necklace;
            break;
        }
    } else if ((u32)(static_cast<u8>(play->play.mEvtCtrl.mTalkButton) - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
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
                m734 = l_msg_xy_sa5_no_skull_necklace;
            } else if (!dComIfGs_isEventBit(0x2620)) {
                m734 = l_msg_xy_sa5_1st;
            } else {
                m734 = l_msg_xy_sa5_2nd;
            }
            break;
        }
    } else {
        switch (mNpcNo) {
        case NPC_UO1:
            if (!dComIfGs_checkGetItem(dItemNo_DELIVERY_BAG_e)) {
                if (!dComIfGs_isEventBit(0x2501)) {
                    dComIfGs_onEventBit(0x2501);
                    m734 = l_msg_uo1_1st_haitatu;
                } else {
                    m734 = l_msg_uo1_haitatu;
                }
            } else if (!dComIfGs_isEventBit(0x1B40)) {
                dComIfGs_onEventBit(0x1B40);
                m734 = l_msg_uo1_1st_talk;
            } else if (isUo1FdaiAll()) {
                if (!dComIfGs_isEventBit(0x1B10)) {
                    m734 = l_msg_uo1_1st_talk_fdai;
                    dComIfGs_onEventBit(0x1B10);
                } else {
                    m734 = l_msg_uo1_2nd_talk_fdai;
                }
            } else if (isUo1FdaiOne()) {
                m734 = l_msg_uo1_fadi1;
            } else {
                m734 = l_msg_uo1_fdai2;
            }
            break;
        case NPC_UO2:
            if (m79A & 0x1) {
                m79A = m79A & ~0x1;
                m734 = l_msg_uo2_surprise;
            } else if (dComIfGs_isEventBit(0x2D01)) {
                m734 = l_msg_uo2_help;
            } else if (!dComIfGs_isEventBit(0x1B04)) {
                dComIfGs_onEventBit(0x1B04);
                m734 = l_msg_uo2_1st_talk;
            } else {
                m734 = l_msg_uo2_2nd_talk;
            }
            break;
        case NPC_UO3:
            if (m793 == 3 || (mEtcFlag & 0x8)) {
                m734 = l_msg_uo3_kyoro;
            } else if (m793 == 4) {
                m734 = l_msg_uo3_letter;
            } else if (!dComIfGs_isEventBit(0x1E08)) {
                dComIfGs_onEventBit(0x1E08);
                m734 = l_msg_uo3_1st_talk;
            } else if (!(mEtcFlag & 0x2)) {
                m734 = l_msg_uo3_befor_letter;
            } else if (mEtcFlag & 0x4) {
                m734 = l_msg_uo3_retry_letter;
            } else {
                m734 = l_msg_uo3_after_letter;
            }
            break;
        case NPC_UB3:
            if (!dComIfGs_isEventBit(0x1D20)) {
                m734 = l_msg_ub3_1st_talk;
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
                    peopleCalcDisXZAngY(a, b, (f32*)temp.get(), nullptr);
                    if (!(*temp > 500.0f)) {
                        m734 = l_msg_ub3_ship_near5;
                        break;
                    }
                }
                if (dComIfGs_checkGetItem(dItemNo_WIND_WAKER_e)) {
                    m734 = l_msg_ub3_tact;
                } else {
                    m734 = l_msg_ub3_talk;
                }
            }
            break;
        case NPC_UB4:
            if (mEtcFlag & 0x40) {
                m734 = l_msg_ub4_photo_house;
            } else if (!dComIfGs_isEventBit(0x1D10)) {
                m734 = l_msg_ub4_1st_talk;
                dComIfGs_onEventBit(0x1D10);
            } else if (dComIfGs_isEventBit(0x2504)) {
                m734 = l_msg_ub4_color_photo;
            } else if (!dComIfGs_checkGetItem(dItemNo_PICTO_BOX_e) && !dComIfGs_checkGetItem(dItemNo_DELUXE_PICTO_BOX_e)) {
                m734 = l_msg_ub4_no_photo_box;
            } else {
                m734 = l_msg_ub4_photo_box;
            }
            break;
        case NPC_UW1:
            if (people_endlessNight()) {
                msgNo = 0x2DC9;
            } else if (!mbIsNight) {
                if (!dComIfGs_isEventBit(0x1E20)) {
                    m734 = l_msg_uw1_1st_talk_day;
                    dComIfGs_onEventBit(0x1E20);
                } else {
                    m734 = l_msg_uw1_talk_day;
                }
            } else if (!dComIfGs_isEventBit(0x1E10)) {
                m734 = l_msg_uw1_1st_talk_night;
                dComIfGs_onEventBit(0x1E10);
            } else if (dComIfGs_checkGetItem(dItemNo_DELIVERY_BAG_e)) {
                if (dComIfGs_checkGetItem(dItemNo_MAGIC_ARMOR_e)) {
                    m734 = l_msg_uw1_magic_shield;
                } else {
                    m734 = l_msg_uw1_no_magic_shield;
                }
            } else {
                m734 = l_msg_uw1_talk_night;
            }
            break;
        case NPC_UW2:
            if (dComIfGs_getEventReg(UNK_B907) < 2) {
                if (!dComIfGs_isEventBit(0x2101)) {
                    m734 = l_msg_uw2_1st_talk1;
                    dComIfGs_onEventBit(0x2101);
                } else if (dComIfGs_getEventReg(UNK_C407) < 6) {
                    m734 = l_msg_uw2_no_photo3;
                } else if (dComIfGs_getEventReg(UNK_C407) < 7) {
                    m734 = l_msg_uw2_no_1day_photo3;
                } else if (!dComIfGs_isEventBit(0x2240)) {
                    m734 = l_msg_uw2_request;
                } else if (!dComIfGs_isEventBit(0x2220)) {
                    m734 = l_msg_uw2_cafe_off;
                } else {
                    m734 = l_msg_uw2_cafe_on;
                    dComIfGs_setEventReg(UNK_B907, 1);
                }
            } else if (dComIfGs_getEventReg(UNK_B907) < 4) {
                if (!dComIfGs_isEventBit(0x2280)) {
                    m734 = l_msg_uw2_1st_talk2;
                    dComIfGs_onEventBit(0x2280);
                    dComIfGs_setEventReg(UNK_B907, 3);
                } else {
                    m734 = l_msg_uw2_talk2;
                }
            } else {
                m734 = l_msg_uw2_talk3;
            }
            break;
        case NPC_UM1:
            if (people_endlessNight()) {
                msgNo = 0x2FBD;
            } else if (!dComIfGs_isEventBit(0x2104)) {
                m734 = l_msg_um1_frrs_stop;
            } else if (!dComIfGs_isEventBit(0x2210)) {
                m734 = l_msg_um1_1st_talk;
                dComIfGs_onEventBit(0x2210);
            } else if (!dComIfGs_isEventBit(0x1C02)) {
                m734 = l_msg_um1_light_off;
            } else if (!dComIfGs_isEventBit(0x1B20)) {
                m734 = l_msg_um1_get_item;
                dComIfGs_onEventBit(0x1B20);
            } else {
                m734 = l_msg_um1_last;
            }
            break;
        case NPC_UM2:
            if (dComIfGs_getEventReg(UNK_B907) < 2) {
                if (!dComIfGs_isEventBit(0x2208)) {
                    m734 = l_msg_um2_1st_talk1;
                    dComIfGs_onEventBit(0x2208);
                } else if (dComIfGs_getEventReg(UNK_C407) < 7) {
                    m734 = l_msg_um2_no_1day_photo3;
                } else if (dComIfGs_isEventBit(0x2220)) {
                    m734 = l_msg_um2_cafe_on;
                    dComIfGs_setEventReg(UNK_B907, 1);
                } else if (!dComIfGs_isEventBit(0x2240)) {
                    m734 = l_msg_um2_no_request;
                } else {
                    m734 = l_msg_um2_cafe_off;
                }
            } else if (dComIfGs_getEventReg(UNK_B907) < 4) {
                if (!dComIfGs_isEventBit(0x2204)) {
                    m734 = l_msg_um2_1st_talk2;
                    dComIfGs_onEventBit(0x2204);
                } else {
                    m734 = l_msg_um2_talk2;
                }
            } else {
                m734 = l_msg_um2_talk3;
            }
            break;
        case NPC_UM3:
            if (people_endlessNight()) {
                msgNo = 0x347B;
            } else if (!mbIsNight) {
                if (!people_haveSail()) {
                    m734 = l_msg_um3_not_sail;
                } else if (!dComIfGs_isEventBit(0x2340)) {
                    m734 = l_msg_um3_1st_talk;
                    dComIfGs_onEventBit(0x2340);
                } else if (!dComIfGs_isEventBit(0x2310)) {
                    m734 = l_msg_um3_no_nazo_talk;
                } else if (dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e)) {
                    m734 = l_msg_um3_map15;
                } else {
                    m734 = l_msg_um3_no_map15;
                }
            } else if (!dComIfGs_isEventBit(0x2320)) {
                m734 = l_msg_um3_1st_night;
                dComIfGs_onEventBit(0x2320);
            } else if (!dComIfGs_isEventBit(0x2308)) {
                m734 = l_msg_um3_no_look_moon;
            } else if (dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e) && dComIfGs_isEventBit(0x2280)) {
                m734 = l_msg_um3_map15_n;
            } else {
                m734 = l_msg_um3_no_map15_n;
            }
            break;
        case NPC_SA1:
            if (people_endlessNight()) {
                msgNo = 0x2E82;
            } else if (!dComIfGs_isEventBit(0x2304)) {
                m734 = l_msg_sa1_1st_talk;
                dComIfGs_onEventBit(0x2304);
            } else {
                m734 = l_msg_sa1_talk;
            }
            break;
        case NPC_SA2:
            if (!mbIsNight) {
                if (!dComIfGs_isEventBit(0x2302)) {
                    m734 = l_msg_sa2_1st_talk;
                    dComIfGs_onEventBit(0x2302);
                } else if (getWindDir() == 6) {
                    m734 = l_msg_sa2_wind_west;
                } else if (getWindDir() == 2) {
                    m734 = l_msg_sa2_wind_east;
                } else {
                    m734 = l_msg_sa2_wind_not_west;
                }
            } else {
                m734 = l_msg_sa2_night;
            }
            break;
        case NPC_SA3:
            if (people_endlessNight()) {
                msgNo = 0x30E4;
            } else if (!mbIsNight) {
                if (!people_haveSail()) {
                    m734 = l_msg_sa3_not_sail;
                } else if (!dComIfGs_isEventBit(0x2301)) {
                    m734 = l_msg_sa3_1st_talk;
                    dComIfGs_onEventBit(0x2301);
                } else {
                    m734 = l_msg_sa3_fdai[chkDaiza()];
                }
            } else {
                m734 = l_msg_sa3_night;
            }
            break;
        case NPC_SA4:
            if (people_endlessNight()) {
                msgNo = 0x321E;
            } else if (!mbIsNight) {
                if (!people_haveSail()) {
                    m734 = l_msg_sa4_not_sail;
                } else if (!dComIfGs_isEventBit(0x2480)) {
                    m734 = l_msg_sa4_1st_talk;
                    dComIfGs_onEventBit(0x2480);
                } else {
                    m734 = l_msg_sa4_wind[getWindDir()];
                }
            } else {
                m734 = l_msg_sa4_night;
            }
            break;
        case NPC_SA5:
            if (people_endlessNight()) {
                msgNo = 0x3299;
            } else if (!mbIsNight) {
                if (mEtcFlag & 0x80) {
                    mEtcFlag = mEtcFlag & ~0x80;
                    if (!dComIfGs_isEventBit(0x2680)) {
                        m734 = l_msg_sa5__next_day;
                    } else if (getPigTimer() == 0) {
                        m734 = l_msg_sa5_timer_zero;
                    } else if (dComIfGs_getTmpReg(UNK_FE03) == 0) {
                        if (isPigOk()) {
                            m734 = l_msg_sa5_mini_game_clr;
                            dComIfGs_onEventBit(0x2A04);
                        } else if (!dComIfGs_isTmpBit(0x0240)) {
                            u8 reg = dComIfGs_getTmpReg(UNK_FF03);
                            /* HD: the JUT_ASSERT's failure path skips the table read */
                            if (reg < 3) {
                                m734 = l_msg_sa5_false_1st[reg];
                            } else {
                                // HD: the debug expression calls reg a_tempReg.
                                JUT_ASSERT_fail("d_a_npc_people.cpp", 0x2113, "(reg >= 0) && (reg < (sizeof(l_msg_sa5_false_1st) / sizeof(l_msg_sa5_false_1st[0])))");
                            }
                            dComIfGs_onTmpBit(0x0240);
                        } else {
                            m734 = l_msg_sa5_false_2nd;
                        }
                    } else {
                        u8 reg = dComIfGs_getTmpReg(UNK_FE03);
                        switch (reg) {
                        case 3:
                            m734 = l_msg_xy_pig1;
                            break;
                        case 2:
                            m734 = l_msg_xy_pig2;
                            break;
                        case 1:
                            m734 = l_msg_xy_sa5_mini_game2_clr;
                            dComIfGs_offEventBit(0x2680);
                            break;
                        }
                        reg -= 1;
                        dComIfGs_setTmpReg(UNK_FE03, reg);
                    }
                } else if (!dComIfGs_isEventBit(0x2440)) {
                    m734 = l_msg_sa5_1st_talk;
                    dComIfGs_onEventBit(0x2440);
                } else if (dComIfGs_isTmpBit(0x0280)) {
                    if (!dComIfGs_isEventBit(0x2680)) {
                        m734 = l_msg_sa5__next_day;
                    } else if (getPigTimer() == 0) {
                        m734 = l_msg_sa5_timer_zero;
                    } else {
                        u8 reg = dComIfGs_getTmpReg(UNK_FF03);
                        if (reg < 4) {
                            m734 = l_msg_sa5_play[reg];
                        } else {
                            // HD: the debug expression calls reg a_tempReg.
                            JUT_ASSERT_fail("d_a_npc_people.cpp", 0x2151, "(reg >= 0) && (reg < (sizeof(l_msg_sa5_play) / sizeof(l_msg_sa5_play[0])))");
                        }
                    }
                } else if (dComIfGs_isEventBit(0x2680)) {
                    m734 = l_msg_sa5_day;
                } else if ((u32)dComIfGs_getRupee() > 80) {
                    m734 = l_msg_sa5_50rupee;
                } else if (!dComIfGs_isEventBit(0x2640)) {
                    m734 = l_msg_sa5_1st;
                } else {
                    m734 = l_msg_sa5_2nd;
                }
            } else {
                m734 = l_msg_sa5_night;
            }
            break;
        case NPC_UG1:
            if (!(people_homeDistXZ(this) < 200.0f)) {
                m734 = l_msg_ug1_out_area;
            } else if (!dComIfGs_isEventBit(0x2880)) {
                m734 = l_msg_ug1_1st_talk;
                dComIfGs_onEventBit(0x2880);
            } else if (dComIfGs_isEventBit(0x2B04)) {
                m734 = l_msg_ug1_hint_talk;
            } else {
                m734 = l_msg_ug1_hint_talk_today;
            }
            break;
        case NPC_UG2:
            if (!(people_homeDistXZ(this) < 200.0f)) {
                m734 = l_msg_ug2_out_area;
            } else if (!dComIfGs_isEventBit(0x2B08)) {
                m734 = l_msg_ug2_1st_talk;
                dComIfGs_onEventBit(0x2B08);
            } else if (dComIfGs_isEventBit(0x2B04)) {
                m734 = l_msg_ug2_hint_talk;
            } else {
                m734 = l_msg_ug2_hint_talk_today;
            }
            break;
        }
    }

    if (m734.get() != nullptr) {
        msgNo = *m734;
    }
    return msgNo;
}

/* WWHD glue
data l_msg_uo1_1st_haitatu 101C39F4
data l_msg_uo1_haitatu 101C2ECC
data l_msg_uo1_1st_talk 101C34E0
data l_msg_uo1_1st_talk_fdai 101C3A00
data l_msg_uo1_2nd_talk_fdai 101C2ED4
data l_msg_uo1_fadi1 101C3A14
data l_msg_uo1_fdai2 101C3A20
data l_msg_uo2_surprise 101C2EE4
data l_msg_uo2_help 101C2EEC
data l_msg_uo2_1st_talk 101C34F0
data l_msg_uo2_2nd_talk 101C2EDC
data l_msg_uo3_kyoro 101C2F0C
data l_msg_uo3_letter 101C2F14
data l_msg_uo3_1st_talk 101C3500
data l_msg_uo3_befor_letter 101C2EF4
data l_msg_uo3_retry_letter 101C2F04
data l_msg_uo3_after_letter 101C2EFC
data l_msg_xy_uo3_photo 101C3A2C
data l_msg_xy_uo3_no_photo 101C2F1C
data l_msg_ub3_1st_talk 101C3688
data l_msg_ub3_ship_near5 101C2F4C
data l_msg_ub3_tact 101C3698
data l_msg_ub3_talk 101C3A6C
data l_msg_ub4_photo_house 101C3A84
data l_msg_ub4_1st_talk 101C2F54
data l_msg_ub4_color_photo 101C2F5C
data l_msg_ub4_no_photo_box 101C2F64
data l_msg_ub4_photo_box 101C3A78
data l_msg_xy_ub4_no_photo 101C2F6C
data l_msg_xy_ub4_no_color 101C2F74
data l_msg_xy_ub4_get_item 101C36A8
data l_msg_xy_ub4_talk 101C2F7C
data l_msg_uw1_1st_talk_day 101C3A90
data l_msg_uw1_talk_day 101C2F84
data l_msg_uw1_1st_talk_night 101C3AB4
data l_msg_uw1_magic_shield 101C3ACC
data l_msg_uw1_no_magic_shield 101C3AD8
data l_msg_uw1_talk_night 101C3AC0
data l_msg_uw2_1st_talk1 101C3AE4
data l_msg_uw2_no_photo3 101C3AF0
data l_msg_uw2_no_1day_photo3 101C3AFC
data l_msg_uw2_request 101C36C8
data l_msg_uw2_cafe_off 101C3B08
data l_msg_uw2_cafe_on 101C2FAC
data l_msg_uw2_1st_talk2 101C36D8
data l_msg_uw2_talk2 101C2FB4
data l_msg_uw2_talk3 101C2FBC
data l_msg_um1_frrs_stop 101C36E8
data l_msg_um1_1st_talk 101C3B14
data l_msg_um1_light_off 101C3718
data l_msg_um1_get_item 101C3728
data l_msg_um1_last 101C2FC4
data l_msg_um2_1st_talk1 101C3738
data l_msg_um2_no_1day_photo3 101C3748
data l_msg_um2_cafe_on 101C3B2C
data l_msg_um2_no_request 101C2FCC
data l_msg_um2_cafe_off 101C3B20
data l_msg_um2_1st_talk2 101C2FD4
data l_msg_um2_talk2 101C2FDC
data l_msg_um2_talk3 101C2FE4
data l_msg_xy_um2_talk1 101C3758
data l_msg_xy_um2_talk2 101C3B38
data l_msg_xy_um2_talk3 101C3768
data l_msg_xy_um2_talk4 101C3778
data l_msg_xy_um2_talk5 101C3B44
data l_msg_um3_not_sail 101C2FEC
data l_msg_um3_1st_talk 101C3790
data l_msg_um3_no_nazo_talk 101C3B50
data l_msg_um3_map15 101C2FFC
data l_msg_um3_no_map15 101C3B5C
data l_msg_um3_1st_night 101C3B68
data l_msg_um3_no_look_moon 101C3B74
data l_msg_um3_map15_n 101C37B0
data l_msg_um3_no_map15_n 101C3004
data l_msg_xy_um3_ng 101C300C
data l_msg_xy_um3_sun 101C3014
data l_msg_xy_um3_no_full_moon 101C301C
data l_msg_xy_um3_no_color 101C3024
data l_msg_xy_moon 101C3B80
data l_msg_look_full_moon 101C3B9C
data l_msg_look_moon 101C3BA8
data l_msg_look_orion 101C302C
data l_msg_look_hokuto 101C3034
data l_msg_sa1_1st_talk 101C3BB4
data l_msg_sa1_talk 101C303C
data l_msg_sa2_1st_talk 101C3044
data l_msg_sa2_wind_west 101C37C0
data l_msg_sa2_wind_east 101C3BC8
data l_msg_sa2_wind_not_west 101C3BD4
data l_msg_sa2_night 101C304C
data l_msg_sa3_not_sail 101C3BE0
data l_msg_sa3_1st_talk 101C3BEC
data l_msg_sa3_fdai_0 101C3054
data l_msg_sa3_fdai_1 101C37D0
data l_msg_sa3_fdai_2 101C37F0
data l_msg_sa3_fdai_3 101C3BF8
data l_msg_sa3_fdai_4 101C3810
data l_msg_sa3_fdai_5 101C3830
data l_msg_sa3_fdai_6 101C3C04
data l_msg_sa3_fdai_7 101C3C10
data l_msg_sa3_night 101C3064
data l_msg_sa4_not_sail 101C306C
data l_msg_sa4_1st_talk 101C3C1C
data l_msg_sa4_wind_0 101C3880
data l_msg_sa4_wind_1 101C38A0
data l_msg_sa4_wind_2 101C38B0
data l_msg_sa4_wind_3 101C38D0
data l_msg_sa4_wind_4 101C38E0
data l_msg_sa4_wind_5 101C38F0
data l_msg_sa4_wind_6 101C3900
data l_msg_sa4_wind_7 101C3910
data l_msg_sa4_night 101C3074
data l_msg_sa5__next_day 101C30BC
data l_msg_sa5_timer_zero 101C30B4
data l_msg_sa5_mini_game_clr 101C3960
data l_msg_sa5_false_1st1 101C3CC4
data l_msg_sa5_false_1st2 101C3CD0
data l_msg_sa5_false_1st3 101C3CDC
data l_msg_sa5_false_2nd 101C3CF4
data l_msg_xy_pig1 101C30DC
data l_msg_xy_pig2 101C30E4
data l_msg_xy_sa5_mini_game2_clr 101C3D3C
data l_msg_sa5_1st_talk 101C3C28
data l_msg_sa5_play0 101C3C34
data l_msg_sa5_play1 101C3C40
data l_msg_sa5_play2 101C3C4C
data l_msg_sa5_play3 101C3084
data l_msg_sa5_day 101C308C
data l_msg_sa5_50rupee 101C3094
data l_msg_sa5_1st 101C3C58
data l_msg_sa5_2nd 101C3C64
data l_msg_sa5_night 101C30C4
data l_msg_xy_sa5_no_skull_necklace 101C30CC
data l_msg_xy_sa5_1st 101C3978
data l_msg_xy_sa5_2nd 101C3D00
data l_msg_ug1_out_area 101C30FC
data l_msg_ug1_1st_talk 101C3D50
data l_msg_ug1_hint_talk 101C30EC
data l_msg_ug1_hint_talk_today 101C30F4
data l_msg_ug2_out_area 101C310C
data l_msg_ug2_1st_talk 101C3D5C
data l_msg_ug2_hint_talk 101C3104
data l_msg_ug2_hint_talk_today 101C3D68
data l_msg_sa3_fdai 101C3850 nocheck
data l_msg_sa4_wind 101C3920 nocheck
data l_msg_sa5_false_1st 101C3CE8 nocheck
data l_msg_sa5_play 101C3950 nocheck
data l_daiza_no_tbl 101C3E1C
ext 02560828 s32 dKy_moon_type_chk()
ext 02526100 u32 daDai_getMaxDaiza()
ext 0252610C u32 daDai_getDaizaSetItemNum()
ext 0259D624 void peopleCalcDisXZAngY(cXyz*, cXyz*, f32*, s16*)
str 10020874 "d_a_npc_people.cpp"
str 10020888 "(reg >= 0) && (reg < (sizeof(l_msg_sa5_false_1st) / sizeof(l_msg_sa5_false_1st[0])))" renamed
str 100208EC "(reg >= 0) && (reg < (sizeof(l_msg_sa5_play) / sizeof(l_msg_sa5_play[0])))" renamed
*/
