// Link attention, carried items and sword/hammer effects. Private WWHD views.
#pragma once
#include "d/actor/d_a_player_main_04_item.h"

// HD: the emitter's renderer tail differs from the shared GameCube layout.
struct LinkEffectEmitter {
    u8 _000[0x1F0];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    GXColor mGlobalPrmColor;
    GXColor mGlobalEnvColor;
};
WWHD_OFFSET(LinkEffectEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(LinkEffectEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(LinkEffectEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(LinkEffectEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(LinkEffectEmitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(LinkEffectEmitter, mGlobalEnvColor, 0x248);
WWHD_SIZE(LinkEffectEmitter, 0x24C);
struct LinkEffectLayout {
    u8 _0000[0x5848];
    be<u16> mUnderAnimationIndex;
    u8 _584A[0x6868 - 0x584A];
    LinkItemMtxCallback m3454;
};
WWHD_OFFSET(LinkEffectLayout, mUnderAnimationIndex, 0x5848);
WWHD_OFFSET(LinkEffectLayout, m3454, 0x6868);
WWHD_SIZE(LinkEffectLayout, 0x6874);
struct LinkEffectBarrel {
    u8 _000[0x738];
    be<s16> mSlantAngle;
};
WWHD_OFFSET(LinkEffectBarrel, mSlantAngle, 0x738);
WWHD_SIZE(LinkEffectBarrel, 0x73A);
struct LinkEffectShip {
    u8 _000[0x644];
    be<u32> mStateFlag;
};
WWHD_OFFSET(LinkEffectShip, mStateFlag, 0x644);
WWHD_SIZE(LinkEffectShip, 0x648);
inline LinkEffectLayout& LinkEffect(daPy_lk_c* player) {
    return gabi::object_at<LinkEffectLayout>(gabi::ea(player));
}
inline LinkEffectEmitter& LinkEffectParticle(JPABaseEmitter* emitter) {
    return gabi::object_at<LinkEffectEmitter>(gabi::ea(emitter));
}
struct LinkEffectApi {
    static void ZXYrotS(Mtx34*, s32, s32, s32);
    static void setWallRadius(dBgS_AcchCir*, f32);
    static s16 addCalcAngleS(be<s16>*, s32, s32, s32, s32);
    // HD: normalize writes both the receiver and the caller's result slot.
    static void normalize(cXyz*, cXyz*);
    static BOOL getWaterY(cXyz*, be<f32>*);
    // HD: this call uses the returned emitter; the item-model adapter discards r3.
    static JPABaseEmitter* makeMtxEmitter(LinkItemMtxCallback*, u32, Mtx34*, cXyz*, u32);
    static void setGlobalRTMatrix(Mtx34*, Mtx34*, cXyz*);
};
namespace LinkEffectConstants {
    enum : u32 {
        ModeFlg_MIDAIR = 2, ModeFlg_WHIDE = 0x10, ModeFlg_HANG = 0x20,
        ModeFlg_ROPE = 0x800, ModeFlg_IN_SHIP = 0x2000, ModeFlg_SWIM = 0x40000,
        ModeFlg_CROUCH = 0x800000, ModeFlg_CRAWL = 0x1000000,
        daPyStts0_SUBJECT_e = 0x2000, daPyFlg0_UNK400 = 0x400,
        daPyProc_WAIT_TURN_e = 0x17, daPyProc_HAMMER_FRONT_SWING_e = 0x53,
        daPyProc_GRAB_READY_e = 0x6E, daPyProc_GRAB_UP_e = 0x6F,
        daPyProc_GRAB_PUT_e = 0x72, daPyProc_GRAB_WAIT_e = 0x73,
        daPyProc_DEMO_STAND_ITEM_PUT_e = 0xC5,
        dRes_INDEX_LKANM_BCK_GRABUP_e = 0x94, fpcNm_MT_e = 0xD8,
        daPyItem_SWORD_e = 0x103, dItemNo_MASTER_SWORD_3_e = 0x3E,
        dActStts_PARRY_e = 0x1A, ID_IT_JN_HM_WP00 = 0x27C,
        ID_IT_JN_WP_HAMON01 = 0x3D, ID_AK_JN_ELEMENTSHIBUKI00 = 0x23,
        ID_AK_JN_LIGHTSAVER00 = 0x309, ID_AK_JN_LIGHTSAVER01 = 0x30A,
        YMSLI00_JNT_SWMSA_JNT_e = 2, SWMS_JNT_SWMSA_JNT_e = 2,
    };
}

// HD: the animation resource starts with the renderer's current frame.
inline be<f32>& LinkEffect_resourceFrame(u32 resource) {
    return gabi::object_at<be<f32>>(resource);
}
inline dPa_levelEcallBack* LinkEffect_singleRippleCallback() {
    return gabi::at<dPa_levelEcallBack>(0x1047B2E4);
}
inline void LinkEffect_storeScale(cXyz* destination, const cXyz* scale) {
    destination->x = scale->x;
    destination->y = scale->y;
    destination->z = scale->z;
}
