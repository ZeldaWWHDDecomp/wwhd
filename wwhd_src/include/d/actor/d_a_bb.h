/* bb_class (Kargaroc), WWHD layout, and the translation unit's statics and local bindings.
 *
 * GameCube -> WWHD (size 0x1260 -> 0x13D8, constructor 02061C70):
 * - +0x11C up to mBtpAnm (fopEn_enemy_c: mPhase at 0x3C8, mpMorf 0x3D0);
 * - mDoExt_btpAnm is 0x74 (GameCube 0x14): +0x17C for unk_2CC..unk_2D0;
 * - the shadow id unk_2D4 is gone (HD shadows): +0x176 for unk_2D8..unk_2E0;
 * - from unk_2E4 on (4-byte alignment) +0x174, up to the end; enemyice keeps its size (0x3B8),
 *   enemyfire is 0x22C (LIGHT_INFLUENCE 0x24).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

/* ---- shared classes, local layouts (SHARED-CANDIDATE, as in d_a_bk.h) ---- */
#ifndef WWHD_BB_SHARED_L
#define WWHD_BB_SHARED_L
struct dPa_followEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x14 - 0x04];
};
WWHD_SIZE(dPa_followEcallBack_l, 0x14);

struct mDoExt_btpAnm_l { u8 _[0x74]; }; /* HD size (constructor 025E7820) */

/* enemyice (unchanged size 0x3B8) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 m007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 m016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 m1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

/* enemyfire, HD 0x22C (constructor 02061BE4 in this TU) */
struct enemyfire_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 m007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 m00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 m01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 m09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 m09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x24];         /* LIGHT_INFLUENCE (HD 0x24; +0x20 = 1.0f) */
};
WWHD_SIZE(enemyfire_l, 0x22C);

/* dPath / dPnt (unchanged from GameCube) */
struct dPnt_l {
    /* 0x0 */ u8 _0[3];
    /* 0x3 */ be<u8> mArg3;
    /* 0x4 */ cXyz m_position;
};
struct dPath_l {
    /* 0x0 */ be<u16> m_num;
    /* 0x2 */ be<u16> m_nextID;
    /* 0x4 */ u8 _4;
    /* 0x5 */ be<u8> m_closed;
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ gptr<dPnt_l> m_points;
};
#endif

/* bbHIO_c (0x80, unchanged; vtable at +0) */
struct bbHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> unk_04;
    /* 0x05 */ be<u8> unk_05;
    /* 0x06 */ be<u8> unk_06;
    /* 0x07 */ u8 _07;
    /* 0x08 */ be<f32> unk_08;
    /* 0x0C */ be<f32> unk_0C;
    /* 0x10 */ be<s16> unk_10;
    /* 0x12 */ be<s16> unk_12;
    /* 0x14 */ be<s16> unk_14;
    /* 0x16 */ be<s16> unk_16;
    /* 0x18 */ be<s16> unk_18;
    /* 0x1A */ u8 _1A[2];
    /* 0x1C */ be<f32> unk_1C;
    /* 0x20 */ be<f32> unk_20;
    /* 0x24 */ be<f32> unk_24;
    /* 0x28 */ be<f32> unk_28;
    /* 0x2C */ be<f32> unk_2C;
    /* 0x30 */ be<s16> unk_30;
    /* 0x32 */ be<s16> unk_32;
    /* 0x34 */ be<f32> unk_34;
    /* 0x38 */ be<f32> unk_38;
    /* 0x3C */ be<f32> unk_3C;
    /* 0x40 */ be<s16> unk_40;
    /* 0x42 */ u8 _42[2];
    /* 0x44 */ be<f32> unk_44;
    /* 0x48 */ be<f32> unk_48;
    /* 0x4C */ be<f32> unk_4C;
    /* 0x50 */ be<s16> unk_50;
    /* 0x52 */ u8 _52[2];
    /* 0x54 */ be<f32> unk_54;
    /* 0x58 */ be<f32> unk_58;
    /* 0x5C */ be<f32> unk_5C;
    /* 0x60 */ be<s16> unk_60;
    /* 0x62 */ be<s16> unk_62;
    /* 0x64 */ be<s16> unk_64;
    /* 0x66 */ u8 _66[2];
    /* 0x68 */ be<f32> unk_68;
    /* 0x6C */ be<f32> unk_6C;
    /* 0x70 */ be<s16> unk_70;
    /* 0x72 */ be<s16> unk_72;
    /* 0x74 */ be<f32> unk_74;
    /* 0x78 */ be<f32> unk_78;
    /* 0x7C */ be<f32> unk_7C;
};
WWHD_SIZE(bbHIO_c, 0x80);

struct bb_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ mDoExt_btpAnm_l mBtpAnm;
    /* 0x448 */ be<u8> unk_2CC;
    /* 0x449 */ be<u8> unk_2CD;
    /* 0x44A */ be<u8> unk_2CE;
    /* 0x44B */ u8 _44B;
    /* 0x44C */ be<s16> unk_2D0;
    /* (GameCube unk_2D4 shadow id: gone) */
    /* 0x44E */ be<u8> unk_2D8;
    /* 0x44F */ be<u8> unk_2D9;
    /* 0x450 */ be<u8> unk_2DA;
    /* 0x451 */ be<u8> unk_2DB;
    /* 0x452 */ be<u8> unk_2DC;
    /* 0x453 */ be<u8> unk_2DD;
    /* 0x454 */ be<u8> unk_2DE;
    /* 0x455 */ be<u8> unk_2DF;
    /* 0x456 */ be<u8> unk_2E0;
    /* 0x457 */ u8 _457;
    /* 0x458 */ gptr<fopAc_ac_c> unk_2E4;
    /* 0x45C */ be<u32> unk_2E8;
    /* 0x460 */ be<s32> unk_2EC;
    /* 0x464 */ be<s8> unk_2F0;
    /* 0x465 */ be<s8> unk_2F1;
    /* 0x466 */ be<u8> unk_2F2;
    /* 0x467 */ u8 _467;
    /* 0x468 */ cXyz unk_2F4;
    /* 0x474 */ be<f32> unk_300;
    /* 0x478 */ be<f32> unk_304;
    /* 0x47C */ be<f32> unk_308;
    /* 0x480 */ be<f32> unk_30C;
    /* 0x484 */ be<f32> unk_310;
    /* 0x488 */ u8 unk314[4];
    /* 0x48C */ be<s16> unk_318[6];
    /* 0x498 */ be<s16> unk_324;
    /* 0x49A */ be<s16> unk_326;
    /* 0x49C */ be<f32> unk_328;
    /* 0x4A0 */ be<f32> unk_32C;
    /* 0x4A4 */ be<u32> unk_330;
    /* 0x4A8 */ u8 unk334[2];
    /* 0x4AA */ be<s16> unk_336;
    /* 0x4AC */ be<s16> unk_338;
    /* 0x4AE */ u8 _4AE[2];
    /* 0x4B0 */ be<f32> unk_33C;
    /* 0x4B4 */ be<s16> unk_340;
    /* 0x4B6 */ be<s16> unk_342;
    /* 0x4B8 */ be<s16> unk_344;
    /* 0x4BA */ u8 _4BA[2];
    /* 0x4BC */ be<f32> unk_348;
    /* 0x4C0 */ be<f32> unk_34C;
    /* 0x4C4 */ be<s16> unk_350;
    /* 0x4C6 */ be<s16> unk_352;
    /* 0x4C8 */ be<s16> unk_354;
    /* 0x4CA */ u8 _4CA[2];
    /* 0x4CC */ be<f32> unk_358;
    /* 0x4D0 */ u8 unk35C;
    /* 0x4D1 */ be<s8> unk_35D;
    /* 0x4D2 */ be<s8> unk_35E;
    /* 0x4D3 */ be<s8> unk_35F;
    /* 0x4D4 */ gptr<dPath_l> ppd;
    /* 0x4D8 */ be<u8> unk_364;
    /* 0x4D9 */ u8 _4D9;
    /* 0x4DA */ be<s16> unk_366;
    /* 0x4DC */ be<s16> unk_368;
    /* 0x4DE */ u8 unk36A[0xE];
    /* 0x4EC */ dBgS_AcchCir mAcchCir;
    /* 0x52C */ dBgS_ObjAcch mAcch;
    /* 0x6F0 */ be<u8> unk_57C;
    /* 0x6F1 */ u8 _6F1[3];
    /* 0x6F4 */ dCcD_Stts mStts;
    /* 0x730 */ dCcD_Sph mHeadAtSph;
    /* 0x85C */ dCcD_Sph mHeadTgSph;
    /* 0x988 */ dCcD_Sph mBodyTgSph;
    /* 0xAB4 */ dCcD_Sph mBodyCoSph;
    /* 0xBE0 */ cXyz unk_A6C[2];
    /* 0xBF8 */ gptr<J3DModel> unk_A84[9];
    /* 0xC1C */ cXyz unk_AA8[10];
    /* 0xC94 */ csXyz unk_B20[10];
    /* 0xCD0 */ cXyz unk_B5C[10];
    /* 0xD48 */ cXyz unk_BD4[2];
    /* 0xD60 */ be<s16> unk_BEC;
    /* 0xD62 */ be<s16> unk_BEE;
    /* 0xD64 */ u8 unkBF0[4];
    /* 0xD68 */ cXyz unk_BF4;
    /* 0xD74 */ csXyz unk_C00[11];
    /* 0xDB6 */ csXyz unk_C42[2];
    /* 0xDC2 */ be<s16> unk_C4E;
    /* 0xDC4 */ be<s16> unk_C50;
    /* 0xDC6 */ be<s16> unk_C52;
    /* 0xDC8 */ be<u32> unk_C54;
    /* 0xDCC */ be<f32> unk_C58;
    /* 0xDD0 */ be<s16> unk_C5C;
    /* 0xDD2 */ be<s16> unk_C5E;
    /* 0xDD4 */ be<u8> unk_C60;
    /* 0xDD5 */ u8 unkC61[7];
    /* 0xDDC */ dPa_followEcallBack_l mParticleCallBack;
    /* 0xDF0 */ be<s8> unk_C7C;
    /* 0xDF1 */ u8 unkC7D;
    /* 0xDF2 */ be<u8> unk_C7E;
    /* 0xDF3 */ u8 _DF3;
    /* 0xDF4 */ enemyice_l mEnemyIce;
    /* 0x11AC */ enemyfire_l mEnemyFire;
};
WWHD_OFFSET(bb_class, mpMorf, 0x3D0);
WWHD_OFFSET(bb_class, unk_2CC, 0x448);
WWHD_OFFSET(bb_class, unk_2D8, 0x44E);
WWHD_OFFSET(bb_class, unk_2E4, 0x458);
WWHD_OFFSET(bb_class, ppd, 0x4D4);
WWHD_OFFSET(bb_class, mAcchCir, 0x4EC);
WWHD_OFFSET(bb_class, mAcch, 0x52C);
WWHD_OFFSET(bb_class, mStts, 0x6F4);
WWHD_OFFSET(bb_class, mBodyCoSph, 0xAB4);
WWHD_OFFSET(bb_class, unk_A84, 0xBF8);
WWHD_OFFSET(bb_class, unk_BD4, 0xD48);
WWHD_OFFSET(bb_class, unk_C00, 0xD74);
WWHD_OFFSET(bb_class, unk_C4E, 0xDC2);
WWHD_OFFSET(bb_class, mParticleCallBack, 0xDDC);
WWHD_OFFSET(bb_class, mEnemyIce, 0xDF4);
WWHD_OFFSET(bb_class, mEnemyFire, 0x11AC);
WWHD_SIZE(bb_class, 0x13D8);

/* esa_class (bait): the field read here */
struct esa_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[8];
    /* 0x3B4 */ be<u8> field_0x298;
};

/* ---- resources / enums ---- */
enum {
    dRes_INDEX_BB_BAS_FLY02_e = 0x9,
    dRes_INDEX_BB_BCK_FLY02_e = 0x19,
    dRes_INDEX_BB_BDL_BB_e = 0x25,
    dRes_INDEX_BB_BDL_BB_TAIL_e = 0x26,
    dRes_INDEX_BB_BCK_DAMAGEP_e = 0x16, /* [g] confirm in damage_check */
};
enum {
    BB_JNT_FOOTL_e = 0x3,
    BB_JNT_FOOTR_e = 0x8,
    BB_JNT_ATAMA_e = 0x16,
    BB_JNT_KUCHIA_e = 0x17,
    BB_JNT_KUCHIB_e = 0x18,
    BB_JNT_OA_e = 0x1D,
    BB_JNT_OB_e = 0x1E,
    BB_JNT_NUM_e = 0x1F,
};
enum { fpcNm_ESA_e = 0xDD, fpcNm_MO2_e = 0xBC, fpcNm_BK_e = 0xBD };

/* ---- file statics (.data/.bss) ---- */
#define SAFESTRING_VTBL 0x10007BC4 /* this TU's sead::SafeString vtable */
#define BB_ARC STR(0x10007D20)     /* "Bb" (anm_init's copy; each use has its own literal) */
inline bbHIO_c& l_bbHIO() { return *gabi::at<bbHIO_c>(0x104615AC); }
inline be<s32>& esa_check_count() { return *gabi::at<be<s32>>(0x1046158C); }
inline gptr<esa_class>* esa_info() { return gabi::at<gptr<esa_class>>(0x1046162C); } /* [100] */
inline be<u8>* check_index() { return gabi::at<be<u8>>(0x104617BC); }               /* path_check: static u8[255] */
inline s8 callback_check_index(s32 i) { return gabi::load<s8>(0x101904AC + i); }
inline f32 tial_scale(s32 i) { return gabi::load<f32>(0x10190470 + 4 * i); }
inline u16 bb_tex_anm_idx(s32 i) { return gabi::load<u16>(0x10190494 + 2 * i); }
inline u16 bb_tex_max_frame(s32 i) { return gabi::load<u16>(0x101904A0 + 2 * i); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E789C mDoExt_btpAnm::init(modelData, btp, anmPlay, attr, speed, start, end, modify, entry) */
inline BOOL mDoExt_btpAnm_init(mDoExt_btpAnm_l* a, J3DModelData* d, J3DAnmTexPattern* btp, s32 anmPlay, s32 attr, f32 speed,
                               s16 start, s16 end, s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, btp, anmPlay, attr, speed, start, end, modify, entry);
}
/* 025E7B3C mDoExt_btpAnm::entry(modelData, s16 frame) */
inline void mDoExt_btpAnm_entry_l(mDoExt_btpAnm_l* a, J3DModelData* d, s16 frame) { gabi::call(0x025E7B3C, a, d, frame); }
/* mDoExt_btpAnm::remove(modelData) (HD inline): clears the model data's texture-pattern slot (+0x38) */
inline void mDoExt_btpAnm_remove_l(J3DModelData* d) { gabi::store<u32>(gabi::ea(d) + 0x38, 0); }
/* JPACallBackBase::remove: HD virtual (vtable slot 0x44) */
inline void dPa_EcallBack_remove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }
/* 02041C30 enemy_fire_remove(enemyfire*) */
inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
inline void dMat_ice_entryDL(mDoExt_McaMorf* m, s32 a, void* b) { gabi::call(0x0259138C, m, a, b); }
/* 025E2DA8 mDoExt_modelUpdate(J3DModel*) */
inline void mDoExt_modelUpdate(J3DModel* m) { gabi::call(0x025E2DA8, m); }
/* 025BEBB8 dSnap_RegistFig (USA/HD signature: type, actor, cXyz* pos, s16 angleY, x, y, z) */
inline void dSnap_RegistFig_l(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angY, x, y, z);
}
/* 0200F428 cLib_addCalcAngleS2(s16*, s16 target, s16 scale, s16 maxStep) */
inline void cLib_addCalcAngleS2_l(be<s16>* v, s16 target, s16 scale, s16 maxStep) { gabi::call(0x0200F428, v, target, scale, maxStep); }
/* 0200ED84 cLib_addCalc2(f32*, target, scale, maxStep) */
inline void cLib_addCalc2_l(be<f32>* v, f32 target, f32 scale, f32 maxStep) { gabi::call(0x0200ED84, v, target, scale, maxStep); }
inline bool dBgS_Acch_ChkGroundHit(dBgS_Acch* a) { return (gabi::load<u32>(gabi::ea(a) + 0x28) & 0x20) != 0; }
inline f32 dBgS_Acch_GetGroundH(dBgS_Acch* a) { return gabi::load<f32>(gabi::ea(a) + 0x94); }

/* dBgS_LinChk on the stack (HD layout, as in d_a_kamome; this TU's vtables) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
inline void dBgS_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x10007C3C;
    c->__vtbl_64 = 0x10007C5C;
    c->__vtbl_58 = 0x10007C6C;
    c->__vtbl_20 = 0x10007C4C;
}
inline void dBgS_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x10007C6C;
    c->__vtbl_64 = 0x10007BFC;
    c->__vtbl_20 = 0x10007BEC;
    cBgS_LinChk_dt(c, 0);
}

/* HD J3D (as in d_a_kamome / d_a_mo2): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at
 * 0x104B4868; a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* getAnmMtx: HD marks the joint matrices dirty */
inline Mtx34* model_getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}

/* ---- Create / constructors ---- */
inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
inline u8* fopAcM_CreateAppend() { return gabi::call<u8*>(0x025D5600); }
inline u32 fpcLy_CurrentLayer() { return gabi::call<u32>(0x025DED64); }
inline u32 fpcSCtRq_Request(u32 layer, s16 name, u32 a, u32 b, void* params) { return gabi::call<u32>(0x025E14A8, layer, name, a, b, params); }
inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
inline BOOL dComIfGs_isEventBit(u16 flag) { return dSv_event_isEventBit(dComIfGs_event(), flag); }
inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 n) { return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + 0x50AC), name, n); }
inline u32 modelData_getJointTree(u32 md) { return gabi::call<u32>(0x027F3F94, md); }
inline void dCcD_Sph_ct(void* p) { gabi::call(0x025166F0, p); }
inline void dPa_followEcallBack_ct_l(void* p, u8 a, u8 b) { gabi::call(0x025A5894, p, a, b); }
/* dBgS_ObjAcch: dBgS_Acch::dBgS_Acch + this TU's inline vtables */
inline void dBgS_ObjAcch_ct_l(dBgS_ObjAcch* p) {
    gabi::call(0x024F0474, p);
    u32 a = gabi::ea(p);
    gabi::store<u32>(a + 0x10, 0x10007C0C);
    gabi::store<u32>(a + 0x14, 0x10007C2C);
    gabi::store<u8>(a + 0x18, 1);
    gabi::store<u32>(a + 0x20, 0x10007C1C);
}
#define BB_AAB_VTBL 0x10007BDC /* this TU's cM3dGAab vtable */
#define BB_VTBL 0x10007C8C     /* bb_class vtable */

/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}

/* ---- the TU's functions: natural calls go through WWHD_FUNC; functions not decompiled yet
 * are defined as plain guest calls (weak) in d_a_bb_pending.cpp ---- */
BOOL nodeCallBack(J3DNode* node, int calcTiming);
void tail_control(bb_class* i_this);
void tex_anm_set(bb_class* i_this, u16 idx);
void anm_init(bb_class* i_this, int animFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
void* s_a_d_sub(void* ac1, void* ac2);
esa_class* search_esa(bb_class* i_this);
void kuti_open(bb_class* i_this, s16 arg1, u32 sfxId);
BOOL bb_player_bg_check(bb_class* i_this);
s32 bb_player_view_check(bb_class* i_this);
void path_check(bb_class* i_this);
BOOL daBb_Draw(bb_class* i_this);
void bb_pos_move(bb_class* i_this);
void bb_ground_pos_move(bb_class* i_this);
void* pl_name_check(void* ac, void*);
BOOL daBb_Execute(bb_class* i_this);
BOOL useHeapInit(fopAc_ac_c* ac);
void bb_auto_move(bb_class* i_this);
void bb_water_check(bb_class* i_this);
void bb_wait_move(bb_class* i_this);
void bb_su_wait_move(bb_class* i_this);
void damage_check(bb_class* i_this);
