// Link collision support. Private typed views preserve the verified WWHD storage and ABI.
#pragma once
#include "d/actor/d_a_player_main_04_item.h"

struct LinkCollisionCutCallback {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> mAlpha;
    u8 _05;
    /* 0x06 */ be<s16> mCount;
    /* 0x08 */ gptr<cXyz> mpPos;
    u8 _0C[4];
};
WWHD_OFFSET(LinkCollisionCutCallback, mAlpha, 4);
WWHD_OFFSET(LinkCollisionCutCallback, mCount, 6);
WWHD_OFFSET(LinkCollisionCutCallback, mpPos, 8);
WWHD_SIZE(LinkCollisionCutCallback, 0x10);
struct LinkCollisionLayout {
    u8 _0000[0xBB4];
    dBgS_LinChk mArrowLinChk;
    dBgS_LinChk mMirLightLinChk;
    u8 _0C8C[0x4A68 - 0xC8C];
    LinkItemBrk mpCutfBrk;
    u8 _4AE0[0x53F0 - 0x4AE0];
    gptr<J3DModel> mpYuchw00Model;
    LinkItemBck mYuchw00Bck;
    LinkItemBtk mpYuchw00Btk;
    LinkItemBrk mpYuchw00Brk;
    gptr<J3DModel> mpYbafo00Model;
    LinkItemBtk mpYbafo00Btk;
    u8 _55E4[0x6730 - 0x55E4];
    LinkCollisionCutCallback m331C, m332C, m333C;
};
WWHD_OFFSET(LinkCollisionLayout, mArrowLinChk, 0xBB4);
WWHD_OFFSET(LinkCollisionLayout, mMirLightLinChk, 0xC20);
WWHD_OFFSET(LinkCollisionLayout, mpCutfBrk, 0x4A68);
WWHD_OFFSET(LinkCollisionLayout, mpYuchw00Model, 0x53F0);
WWHD_OFFSET(LinkCollisionLayout, mYuchw00Bck, 0x53F4);
WWHD_OFFSET(LinkCollisionLayout, mpYuchw00Btk, 0x5480);
WWHD_OFFSET(LinkCollisionLayout, mpYuchw00Brk, 0x54F4);
WWHD_OFFSET(LinkCollisionLayout, mpYbafo00Model, 0x556C);
WWHD_OFFSET(LinkCollisionLayout, mpYbafo00Btk, 0x5570);
WWHD_OFFSET(LinkCollisionLayout, m331C, 0x6730);
WWHD_OFFSET(LinkCollisionLayout, m332C, 0x6740);
WWHD_OFFSET(LinkCollisionLayout, m333C, 0x6750);
WWHD_SIZE(LinkCollisionLayout, 0x6760);
struct LinkCollisionEmitter {
    u8 _000[0x1F0];
    /* 0x1F0 */ Mtx34 mGlobalRot;
    u8 _220[0xC];
    /* 0x22C */ cXyz mGlobalTrans;
    /* 0x238 */ be<f32> mGlobalParticleWidthScale;
    u8 _23C[8];
    /* 0x244 */ GXColor mGlobalColorPrm;
};
WWHD_OFFSET(LinkCollisionEmitter, mGlobalRot, 0x1F0);
WWHD_OFFSET(LinkCollisionEmitter, mGlobalTrans, 0x22C);
WWHD_OFFSET(LinkCollisionEmitter, mGlobalParticleWidthScale, 0x238);
WWHD_OFFSET(LinkCollisionEmitter, mGlobalColorPrm, 0x244);
WWHD_SIZE(LinkCollisionEmitter, 0x248);
struct LinkCollisionSinCosTable {
    struct Entry {
        be<f32> sin, cos;
    } m[8192];
};
WWHD_SIZE(LinkCollisionSinCosTable, 0x10000);
struct LinkCollisionApi {
    static BOOL fanWindEffectDraw(daPy_lk_c*);
    static void ZXYrotM(Mtx34*, s32, s32, s32);
    static f64 getBlurTopRate(daPy_lk_c*);
    static void initSwBlur(daPy_swBlur_c*, Mtx34*, s32, s32, f32);
    static void StartCAt(dCcD_Cyl*, cXyz*);
    static void MoveCAt(dCcD_Cyl*, cXyz*);
    static void ResetAtHit(dCcD_GObjInf*);
    static void endCutTurn(LinkCollisionCutCallback*);
    static void setCollider(void*, dCcD_GObjInf*);
    static void setMass(void*, dCcD_GObjInf*, u32);
    static void pushMtx();
    static void popMtx();
    static void inverseMtx(Mtx34*, Mtx34*);
    static f32 squareMagnitude(const cXyz*);
    static f64 sqrtf(f32);
    static void setEmitterMatrix(Mtx34*, Mtx34*, cXyz*);
    static f64 colorGamma(u32);
    static void bgmNowKaitengiri();
};
inline LinkCollisionLayout& LinkCollision(daPy_lk_c* player) {
    return *reinterpret_cast<LinkCollisionLayout*>(player);
}
inline daPy_swBlur_c* LinkCollisionBlur(u32 pointer) { return gabi::at<daPy_swBlur_c>(pointer); }
inline LinkItemCapsuleGeometry& LinkCollisionCapsule(dCcD_Cps& capsule) {
    return *reinterpret_cast<LinkItemCapsuleGeometry*>(capsule.mCps);
}
inline LinkCollisionEmitter& LinkCollisionEmitterView(JPABaseEmitter* emitter) {
    return *reinterpret_cast<LinkCollisionEmitter*>(emitter);
}
// GHS copies these vector components as words, without quieting signaling NaNs.
inline u32 LinkCollisionWord(const be<f32>& field) { return gabi::load<u32>(gabi::ea(&field)); }
inline void LinkCollisionWord(be<f32>& field, u32 value) { gabi::store<u32>(gabi::ea(&field), value); }
namespace LinkCollisionConstants {
enum : u32 {
    daPyProc_JUMP_CUT_e = 0x5B,
    daPyProc_FRONT_ROLL_e = 0x1E,
    daPyProc_BACK_JUMP_e = 0x22,
    daPyProc_BT_JUMP_CUT_e = 0x5E,
    daPyProc_BT_JUMP_e = 0x5D,
    daPyProc_ROPE_SWING_e = 0x78,
    daPyProc_GUARD_SLIP_e = 0x6D,
    daPyProc_CROUCH_DEFENSE_SLIP_e = 0xD,
    daPyProc_VOMIT_WAIT_e = 0x97,
    daPyProc_CUT_TURN_e = 0x55,
    daPyProc_CUT_ROLL_e = 0x56,
    daPyProc_CUT_F_e = 0x42,
    daPyProc_BT_VERTICAL_JUMP_CUT_e = 0x63,
    daPyProc_FAN_SWING_e = 0x92,
    daPyProc_HAMMER_FRONT_SWING_e = 0x53,
    daPyProc_WEAPON_FRONT_SWING_e = 0x4E,
    daPyProc_JUMP_CUT_LAND_e = 0x5C,
    daPyProc_WEAPON_SIDE_SWING_e = 0x4C,
    daPyProc_BOTTLE_OPEN_e = 0xA4,
    daPyProc_FAN_GLIDE_e = 0x93,
    ModeFlg_IN_SHIP = 0x2000,
    ModeFlg_CRAWL = 0x01000000,
    ModeFlg_DAMAGE = 8,
    ModeFlg_PARRY = 0x80000000,
    daPyRFlg0_UNK1 = 1,
    daPyRFlg0_UNK2 = 2,
    daPyRFlg0_LIGHT_REFLECT = 0x200000,
    daPyFlg0_CUT_AT_FLG = 0x40,
    daPyFlg1_SOUP_POWER_UP = 0x8000,
    daPyItem_BOKO_e = 0x101,
    dItemNo_SWORD_e = 0x38,
    dItemNo_MASTER_SWORD_3_e = 0x3E,
    cCcD_CoSPrm_NoCrr_e = 0x100,
    COLLIDER_SET = 1,
    CL_JNT_LINK_ROOT_e = 0,
    CL_JNT_NECK_JNT_e = 14,
    CL_JNT_LTOE_JNT_e = 35,
    CL_JNT_RTOE_JNT_e = 40,
    ID_AK_JN_ROUNDATTACK00 = 0x25,
    ID_AK_JN_ROUNDATTACK01 = 0x26,
    ID_AK_JN_ROUNDATTACK02 = 0x28,
    ID_AK_JN_UCHIWAWIND01 = 0x49,
    JA_SE_LK_SUGOI_KAITEN = 0x203A,
    JA_SE_LK_FAN_WIND = 0x2054,
};
}

struct LinkCollisionPlane {
    cXyz mNormal;
};
WWHD_SIZE(LinkCollisionPlane, 0xC);

struct LinkCollisionPlayView {
    u8 _0000[0x26A4];
    u8 mCollisionSpace;
    u8 _26A5[0x4EF8 - 0x26A5];
    u8 mMassManager;
    u8 _4EF9[0x5CEA - 0x4EF9];
    be<u8> mSwordEquipState;
};
WWHD_OFFSET(LinkCollisionPlayView, mCollisionSpace, 0x26A4);
WWHD_OFFSET(LinkCollisionPlayView, mMassManager, 0x4EF8);
WWHD_OFFSET(LinkCollisionPlayView, mSwordEquipState, 0x5CEA);
WWHD_SIZE(LinkCollisionPlayView, 0x5CEB);
inline LinkCollisionPlayView& LinkCollisionPlay() { return *gabi::at<LinkCollisionPlayView>(dComIfGp_ea()); }

inline LinkItemAnimationResource* LinkCollision_animationResource(LinkItemBrk& animation) {
    return animation.mpAnm.get();
}
inline LinkItemAnimationResource* LinkCollision_animationResource(LinkItemBtk& animation) {
    return gabi::at<LinkItemAnimationResource>(animation.mpAnm);
}
