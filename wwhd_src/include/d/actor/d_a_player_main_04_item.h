// Link item models and particle effects. Private WWHD layout views.
#pragma once
#include "d/actor/d_a_player_main_04.h"
struct LinkItemAnimationResource;
struct LinkItemBck;
struct LinkItemBrk;
struct LinkItemAnimationValue;
struct LinkItemFollowCallback;
struct LinkItemMtxCallback;
struct LinkItemApi {
    static void updateAnimationValue(gptr<LinkItemAnimationValue>*);
    static void endFollow(LinkItemFollowCallback*);
    static void endMtxFollow(LinkItemMtxCallback*);
    static f64 groundCross(dBgS*, void*);
    static u32 createFootMark(u32, u32, cXyz*, cXyz*, u32);
    static BOOL checkLight(u32, cXyz*);
    static void lineVsPositionCross(const cXyz*, const cXyz*, const cXyz*, cXyz*);
    static void makeMtxEmitter(LinkItemMtxCallback*, u32, Mtx34*, cXyz*, u32);
    static s16 getGroundAngle(daPy_lk_c*, cBgS_PolyInfo*, s32);
    static void setEmitterMaterial(JPABaseEmitter*, void*);
    static void getSeaColor(GXColor*, GXColor*);
    static void resetFootEffect(daPy_lk_c*);
    static s32 checkAttributeEffect(dPa_control_c*, s32);
    static void XYZrotS(Mtx34*, s32, s32, s32);
    static void setShapeFanLeaf(daPy_lk_c*);
    static s16 rad2s(f32);
    static void playAnimation(void*);
    static void entryBrk(LinkItemBrk*, J3DModelData*, f32);
    static void entryBck(LinkItemBck*, J3DModelData*, f32);
    static void setSwordSoundType(s32, s32);
    static void setShieldSoundType(s32, s32);
    static void YrotS(Mtx34*, s32);
    static void concatShieldMtx(Mtx34*, const Mtx34*, Mtx34*);
    static void setBootsModel(daPy_lk_c*, gptr<J3DModel>*);
    static void setBootsSoundType(s32);

    static f32 powf(f32, f32);
    static void SetStartEnd(void*, const cXyz*, const cXyz*);
};
struct LinkItemFollowCallback {
    be<u32> __vtbl;
    gptr<JPABaseEmitter> mpEmitter;
    cXyz mPos;
    csXyz mAngle;
    u8 _1A[2];
};
WWHD_OFFSET(LinkItemFollowCallback, mpEmitter, 4);
WWHD_OFFSET(LinkItemFollowCallback, mPos, 8);
WWHD_SIZE(LinkItemFollowCallback, 0x1C);
// HD: btk resources and bck resources live in the renderer tail rather than +0x10.
struct LinkItemAnimationValue {
    be<f32> mFrame, mStart, mEnd;
    u8 _0C[4];
    be<u32> mpInterpolate, mpContext;
};
WWHD_OFFSET(LinkItemAnimationValue, mpInterpolate, 0x10);
WWHD_OFFSET(LinkItemAnimationValue, mpContext, 0x14);
WWHD_SIZE(LinkItemAnimationValue, 0x18);
struct LinkItemBtk {
    J3DFrameCtrl mFrameCtrl;
    gptr<LinkItemAnimationValue> mpFrameValue;
    u8 _14[0x54];
    be<u32> mpAnm;
    u8 _6C[8];
};
WWHD_OFFSET(LinkItemBtk, mpFrameValue, 0x10);
WWHD_OFFSET(LinkItemBtk, mpAnm, 0x68);
WWHD_SIZE(LinkItemBtk, 0x74);
struct LinkItemBrk {
    J3DFrameCtrl mFrameCtrl;
    gptr<LinkItemAnimationResource> mpAnm;
    u8 _14[0xC];
    gptr<LinkItemAnimationValue> mpFrameValue;
    u8 _24[0x54];
};
WWHD_OFFSET(LinkItemBrk, mpAnm, 0x10);
WWHD_OFFSET(LinkItemBrk, mpFrameValue, 0x20);
WWHD_SIZE(LinkItemBrk, 0x78);
struct LinkItemBck {
    J3DFrameCtrl mFrameCtrl;
    u8 _10[0x78];
    be<u32> mpAnm;
};
WWHD_OFFSET(LinkItemBck, mpAnm, 0x88);
WWHD_SIZE(LinkItemBck, 0x8C);
struct LinkItemFootEffect {
    u8 mSmokeCb[0x20], mFollowCb[0x14];
    cXyz mPos;
    csXyz mAngle;
    u8 _46[2];
    be<s32> mId;
};
WWHD_OFFSET(LinkItemFootEffect, mPos, 0x34);
WWHD_OFFSET(LinkItemFootEffect, mAngle, 0x40);
WWHD_OFFSET(LinkItemFootEffect, mId, 0x48);
WWHD_SIZE(LinkItemFootEffect, 0x4C);
struct LinkItemMtxCallback {
    be<u32> __vtbl;
    gptr<JPABaseEmitter> mpEmitter;
    be<u32> mpMtx;
};
WWHD_SIZE(LinkItemMtxCallback, 0xC);
struct LinkItemLayout {
    u8 _0000[0xD04];
    LinkItemBck mSwgripmsabBckAnim;
    LinkItemBrk mpTswgripmsabBrk;
    LinkItemBtk mpTswgripmsBtk;
    gptr<J3DModel> mpPodmsModel;
    gptr<J3DModel> mpEquippedShieldModel;
    gptr<J3DModel> mpShaModel;
    gptr<J3DModel> mpShmsModel;
    LinkItemBck mAtngshaBck;
    LinkItemBtk mpTshmsBtk;
    u8 _0F8C[0x3428];
    gptr<J3DModel> mpYmsls00Model;
    LinkItemBtk mpYmsls00Btk;
    gptr<J3DModel> mpHbootsModels[2];
    gptr<J3DModel> mpPringModel;
    be<u32> mpItemHeaps[2];
    gptr<J3DModel> mpEquipItemModel;
    LinkItemBck mSwordAnim;
    gptr<mDoExt_McaMorf> mpParachuteFanMorf;
    u8 _44D4[0x33C];
    LinkItemBtk mpEquipItemBtk;
    LinkItemBtk mpSwordBtk;
    LinkItemBrk mpEquipItemBrk;
    gptr<J3DModel> mpBottleContentsModel;
    gptr<J3DModel> mpBottleCapModel;
    gptr<J3DModel> mpSwordModel1;
    u8 _497C[0xBF4];
    mDoExt_btkAnm mpYbafo00Btk;
    u8 _55E4[0x1018];
    LinkItemFootEffect mFootEffect[2];
    u8 _6694[0xEC];
    LinkItemFollowCallback m336C;
    u8 _679C[0x88];
    LinkItemFollowCallback m3410;
    LinkItemMtxCallback m342C;
    LinkItemFollowCallback m3438;
    u8 _6868[0x69EA - 0x6868];
    // HD: decaying hammer recoil.
    be<s16> mHammerSwingAngle, mHammerSwingTimer;
};
WWHD_OFFSET(LinkItemLayout, mpTswgripmsabBrk, 0xD90);
WWHD_OFFSET(LinkItemLayout, mpTswgripmsBtk, 0xE08);
WWHD_OFFSET(LinkItemLayout, mpPodmsModel, 0xE7C);
WWHD_OFFSET(LinkItemLayout, mpEquippedShieldModel, 0xE80);
WWHD_OFFSET(LinkItemLayout, mpShaModel, 0xE84);
WWHD_OFFSET(LinkItemLayout, mpShmsModel, 0xE88);
WWHD_OFFSET(LinkItemLayout, mAtngshaBck, 0xE8C);
WWHD_OFFSET(LinkItemLayout, mpTshmsBtk, 0xF18);
WWHD_OFFSET(LinkItemLayout, mpYmsls00Model, 0x43B4);
WWHD_OFFSET(LinkItemLayout, mpYmsls00Btk, 0x43B8);
WWHD_OFFSET(LinkItemLayout, mpHbootsModels, 0x442C);
WWHD_OFFSET(LinkItemLayout, mpPringModel, 0x4434);
WWHD_OFFSET(LinkItemLayout, mpItemHeaps, 0x4438);
WWHD_OFFSET(LinkItemLayout, mpEquipItemModel, 0x4440);
WWHD_OFFSET(LinkItemLayout, mSwordAnim, 0x4444);
WWHD_OFFSET(LinkItemLayout, mpParachuteFanMorf, 0x44D0);
WWHD_OFFSET(LinkItemLayout, mpEquipItemBtk, 0x4810);
WWHD_OFFSET(LinkItemLayout, mpSwordBtk, 0x4884);
WWHD_OFFSET(LinkItemLayout, mpEquipItemBrk, 0x48F8);
WWHD_OFFSET(LinkItemLayout, mpBottleContentsModel, 0x4970);
WWHD_OFFSET(LinkItemLayout, mpBottleCapModel, 0x4974);
WWHD_OFFSET(LinkItemLayout, mpSwordModel1, 0x4978);
WWHD_OFFSET(LinkItemLayout, mpYbafo00Btk, 0x5570);
WWHD_OFFSET(LinkItemLayout, mFootEffect, 0x65FC);
WWHD_OFFSET(LinkItemLayout, m336C, 0x6780);
WWHD_OFFSET(LinkItemLayout, m3410, 0x6824);
WWHD_OFFSET(LinkItemLayout, m342C, 0x6840);
WWHD_OFFSET(LinkItemLayout, m3438, 0x684C);
WWHD_OFFSET(LinkItemLayout, mHammerSwingAngle, 0x69EA);
WWHD_OFFSET(LinkItemLayout, mHammerSwingTimer, 0x69EC);
WWHD_SIZE(LinkItemLayout, 0x69EE);
inline LinkItemLayout& LinkItem(daPy_lk_c* player) {
    return gabi::object_at<LinkItemLayout>(gabi::ea(player));
}
inline void LinkItem_setStartEnd(dCcD_Cps* capsule, const cXyz* start, const cXyz* end) {
    LinkItemApi::SetStartEnd(capsule->mCps, start, end);
}
inline void LinkItem_setAtVec(dCcD_Cps* capsule, const cXyz* vector) { capsule->mGObjAt.mVec.copy(*vector); }

// HD: renderer-side emitter tail, kept separate from the shared GameCube view.
struct LinkItemEmitter {
    u8 _000[0x34];
    be<f32> mRate;
    u8 _038[0x20];
    be<f32> mSpread;
    be<u32> mMaxFrame;
    be<u16> mLifeTime;
    u8 _062[0xE];
    be<f32> mDirectionalSpeed;
    u8 _074[0x1AC];
    cXyz mGlobalDynamicsScale;
    u8 _22C[0xC];
    cXyz mGlobalParticleScale;
    GXColor mGlobalColor;
};
WWHD_OFFSET(LinkItemEmitter, mRate, 0x34);
WWHD_OFFSET(LinkItemEmitter, mSpread, 0x58);
WWHD_OFFSET(LinkItemEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(LinkItemEmitter, mLifeTime, 0x60);
WWHD_OFFSET(LinkItemEmitter, mDirectionalSpeed, 0x70);
WWHD_OFFSET(LinkItemEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(LinkItemEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(LinkItemEmitter, mGlobalColor, 0x244);
WWHD_SIZE(LinkItemEmitter, 0x248);
inline LinkItemEmitter& LinkEmitter(u32 emitter) { return *gabi::at<LinkItemEmitter>(emitter); }

namespace LinkItemConstants {
enum : u32 {
    CL_JNT_CHEST_JNT_e = 4,
    daPyProc_DEMO_LAST_COMBO_e = 0xD8,
    daPyItem_SWORD_e = 0x103,
    daPyItem_BOKO_e = 0x101,
    daPyProc_JUMP_CUT_e = 0x5B,
    daPyProc_JUMP_CUT_LAND_e = 0x5C,
    daPyFlg1_SOUP_POWER_UP = 0x8000,
    dBgS_Attr_SAND_e = 0xB,
    fpcNm_WATER_MARK_e = 0x1D4,
    daPyRFlg0_UNK1 = 1,
    fpcNm_BPW_e = 0xD3,
    daPyRFlg0_LIGHT_REFLECT = 0x200000,
    JA_SE_OBJ_MIRROR_REFLECT = 0x6961,
    JA_SE_OBJ_MIRROR_LIGHT = 0x7028,
    ID_AK_SN_MIRRORSHIELD00 = 0x8226,
    daPyFlg0_UNK80 = 0x80,
    daPyProc_CUT_ROLL_e = 0x56,
    ID_AK_JT_ELEMENTSMOKE00 = 0x2022,
    ID_AK_JN_ELEMENTKUSA00 = 0x24,
    CL_JNT_RFOOT_JNT_e = 39,
    CL_JNT_LFOOT_JNT_e = 34,
    daPyRFlg0_UNK10 = 0x10,
    dBgS_Attr_WATER_e = 0x13,
    ID_AK_JN_ELEMENTSHIBUKI00 = 0x23,
    daPyProc_FRONT_ROLL_e = 0x1E,
    CL_JNT_CENTER_e = 1,
    dBgS_Attr_GRASS_e = 4,
    CL_JNT_LINK_ROOT_e = 0,
    CL_JNT_CL_PODA_e = 13,
    CL_JNT_CL_LHANDA_e = 8,
    CL_JNT_CL_RHANDA_e = 12,
    CL_JNT_STOMACH_JNT_e = 3,
    dItemNo_TELESCOPE_e = 0x20,
    dItemNo_HYOI_PEAR_e = 0x83,
    CL_JNT_HEAD_JNT_e = 15,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_DEKU_LEAF_e = 0x34,
    dItemNo_FIREFLY_BOTTLE_e = 0x58,
    dItemNo_HOOKSHOT_e = 0x2F,
    dItemNo_WIND_WAKER_e = 0x22,
    daPyProc_DEMO_TOOL_e = 0xA9,
    daPyProc_GUARD_CRASH_e = 0x65,
    CL_JNT_LARMB_JNT_e = 7,
    CL_JNT_RARMB_JNT_e = 11,

    daPyFlg0_EQUIP_HEAVY_BOOTS = 0x2000000,
    daPyFlg0_UNK80000000 = 0x80000000,
    daPyItem_UNK10A_e = 0x10A,
    daPyItem_UNK104_e = 0x104,
    dItemNo_TINGLE_TUNER_e = 0x21,
    daPyProc_FAN_SWING_e = 0x92,
    daPyProc_FAN_GLIDE_e = 0x93,
    daPyProc_TACT_WAIT_e = 0x9A,
    daPyItem_UNK102_e = 0x102,
    daPyProc_DEMO_TALISMAN_WAIT_e = 0xB7,

};
}

struct LinkItemBokoView {
    u8 _000[0x3B4];
    gptr<J3DModel> mpModel;
};
WWHD_OFFSET(LinkItemBokoView, mpModel, 0x3B4);
// HD: this range reads the ship's tact model through this morph controller.
struct LinkItemShipView {
    u8 _000[0x3B8];
    gptr<mDoExt_McaMorf> mTactMorf;
};
WWHD_OFFSET(LinkItemShipView, mTactMorf, 0x3B8);
struct LinkItemParticleView {
    u8 _00[0x68];
    be<f32> mMaterialColor[4];
};
WWHD_OFFSET(LinkItemParticleView, mMaterialColor, 0x68);
// Camera process +0x248 is the camera body; mCurMode is +0x13C within it.
struct LinkItemCameraView {
    u8 _000[0x384];
    be<s32> mCurMode;
};
WWHD_OFFSET(LinkItemCameraView, mCurMode, 0x384);
struct LinkItemAnimationResource {
    be<f32> mFrame;
    be<u32> __vtbl;
};
WWHD_OFFSET(LinkItemAnimationResource, __vtbl, 4);

// HD: J3DPacket grows; the GameCube blur members move by +0x88.
struct daPy_swBlur_c {
    /* 0x000 */ u8 _000[0x9C];
    /* 0x09C */ be<s32> field_0x014;
    /* 0x0A0 */ be<s32> field_0x018;
    /* 0x0A4 */ be<s32> mBlurColorType;
    /* 0x0A8 */ be<f32> mBlurTopRate;
    /* 0x0AC */ gptr<cXyz> mpPosBuffer;
    /* 0x0B0 */ cXyz field_0x028;
    /* 0x0BC */ cXyz field_0x034[60];
    /* 0x38C */ cXyz field_0x304[60];
    void initSwBlur(Mtx34*, int, f32, int);
    void copySwBlur(Mtx34*, int);
};
WWHD_OFFSET(daPy_swBlur_c, field_0x304, 0x38C);

WWHD_OFFSET(daPy_swBlur_c, field_0x014, 0x9C);
WWHD_OFFSET(daPy_swBlur_c, mpPosBuffer, 0xAC);
WWHD_OFFSET(daPy_swBlur_c, field_0x034, 0xBC);
WWHD_SIZE(daPy_swBlur_c, 0x65C);

struct LinkItemCapsuleGeometry {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_OFFSET(LinkItemCapsuleGeometry, mEnd, 0xC);
WWHD_OFFSET(LinkItemCapsuleGeometry, mRadius, 0x1C);
WWHD_SIZE(LinkItemCapsuleGeometry, 0x20);

WWHD_OFFSET(LinkItemLayout, mSwgripmsabBckAnim, 0xD04);
