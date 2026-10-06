/* ki_class (Keese), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; mDoExt_J3DModelPacketS grew from 0x14 to 0xB0
 * (constructor 02080404 allocates 0xB0), so the members after it are +0x1B8; the shadow id m2E0
 * is gone (HD shadows), so from mMaxAttackMoveDist300 on everything is +0x1B4. enemyfire grew by
 * 4 bytes (0x22C, constructor 0219D368). Size 0x10BC (constructor 0219D3F4; GameCube 0xF04). */
#pragma once
#include "bindings.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
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
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

struct enemyfire_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20];  /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> m228;     /* HD: 1.0f in the constructor */
};
WWHD_SIZE(enemyfire_l, 0x22C);

#endif

/* dPa_followEcallBack (HD: vtable at +0, 0x14 bytes) */
struct dPa_followEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x10];
};

struct ki_class {
    enum Action_e {
        ACT_WAIT_MOVE_e = 0,
        ACT_FLY_MOVE_e = 1,
        ACT_FIRE_SET_MOVE_e = 2,
        ACT_ATTACK_MOVE_e = 3,
        ACT_ATTACK_MOVE_INDEX_e = 10,
        ACT_DAMAGE_MOVE_e = 20,
        ACT_FAIL_MOVE_e = 30,
    };

    /* 0x000 */ fopEn_enemy_c actor;
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ u8 m2B8[0xB0];               /* mDoExt_J3DModelPacketS (HD 0xB0; set up only in GanonK) */
    /* 0x484 */ be<u8> mParameters;
    /* 0x485 */ be<u8> m2CD;
    /* 0x486 */ be<u8> mKiPathIndex;
    /* 0x487 */ be<u8> m2CF;
    /* 0x488 */ u8 _488;
    /* 0x489 */ be<u8> mAction;
    /* 0x48A */ u8 _48A;
    /* 0x48B */ be<s8> mBehaviorType;
    /* 0x48C */ be<u8> m2D4;
    /* 0x48D */ be<s8> mCurrKiPathIndex;
    /* 0x48E */ be<s8> m2D6;
    /* 0x48F */ be<s8> m2D7;
    /* 0x490 */ gptr<dPath> ppd;
    /* 0x494 */ be<u8> m2DC;
    /* 0x495 */ u8 _495[3];                   /* HD: no shadow id (GameCube m2E0) */
    /* 0x498 */ be<f32> mMaxAttackMoveDist300;
    /* 0x49C */ cXyz mPosMove;
    /* 0x4A8 */ be<f32> mPosMoveTarget;
    /* 0x4AC */ be<f32> mPosMoveMaxSpeed;
    /* 0x4B0 */ be<f32> mPosMoveDist;
    /* 0x4B4 */ u8 _4B4[4];
    /* 0x4B8 */ be<f32> m304;
    /* 0x4BC */ u8 _4BC[4];
    /* 0x4C0 */ be<s16> mTimers[4];
    /* 0x4C8 */ be<s16> m314;
    /* 0x4CA */ be<s16> m316;
    /* 0x4CC */ be<s16> mRand2000;
    /* 0x4CE */ u8 _4CE[2];
    /* 0x4D0 */ be<f32> m31C;
    /* 0x4D4 */ be<s16> m320;
    /* 0x4D6 */ be<s16> m322;
    /* 0x4D8 */ be<s16> m324;
    /* 0x4DA */ be<s16> m326;
    /* 0x4DC */ be<s16> m328;
    /* 0x4DE */ u8 _4DE[2];
    /* 0x4E0 */ gptr<J3DAnmTexPattern> m32C;
    /* 0x4E4 */ gptr<mDoExt_btpAnm> m330;     /* HD: an mDoExt_btpAnm (GameCube J3DTexNoAnm[]) */
    /* 0x4E8 */ be<u8> m334;
    /* 0x4E9 */ be<u8> m335;
    /* 0x4EA */ be<u8> m336;
    /* 0x4EB */ be<u8> m337;
    /* 0x4EC */ be<s8> m338;
    /* 0x4ED */ be<s8> m339;
    /* 0x4EE */ u8 _4EE[2];
    /* 0x4F0 */ dBgS_AcchCir mAcchCir;
    /* 0x530 */ dBgS_ObjAcch mAcch;
    /* 0x6F4 */ be<u8> m540;
    /* 0x6F5 */ u8 _6F5[3];
    /* 0x6F8 */ dCcD_Stts mStts;
    /* 0x734 */ dCcD_Sph m580;
    /* 0x860 */ dCcD_Sph m6AC;
    /* 0x98C */ dCcD_Sph mDamageSphere;
    /* 0xAB8 */ be<u8> m904;
    /* 0xAB9 */ be<u8> mDamageType;
    /* 0xABA */ u8 _ABA[2];
    /* 0xABC */ dPa_followEcallBack_l m908;
    /* 0xAD0 */ gptr<JPABaseEmitter> m91C;
    /* 0xAD4 */ gptr<mDoExt_btkAnm> m920;
    /* 0xAD8 */ enemyice_l mEnemyIce;
    /* 0xE90 */ enemyfire_l mEnemyFire;
};
WWHD_OFFSET(ki_class, mParameters, 0x484);
WWHD_OFFSET(ki_class, mMaxAttackMoveDist300, 0x498);
WWHD_OFFSET(ki_class, m32C, 0x4E0);
WWHD_OFFSET(ki_class, mAcchCir, 0x4F0);
WWHD_OFFSET(ki_class, mAcch, 0x530);
WWHD_OFFSET(ki_class, mStts, 0x6F8);
WWHD_OFFSET(ki_class, mDamageSphere, 0x98C);
WWHD_OFFSET(ki_class, m908, 0xABC);
WWHD_OFFSET(ki_class, mEnemyIce, 0xAD8);
WWHD_OFFSET(ki_class, mEnemyFire, 0xE90);
WWHD_SIZE(ki_class, 0x10BC);

/* kiHIO_c, HD: vtable pointer after the members (GameCube members -4) */
struct kiHIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<f32> m24;
    /* 0x24 */ be<f32> m28;
    /* 0x28 */ be<f32> m2C;
    /* 0x2C */ be<f32> m30;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<f32> m38;
    /* 0x38 */ be<f32> m3C;
    /* 0x3C */ be<f32> m40;
    /* 0x40 */ be<f32> m44;
    /* 0x44 */ be<f32> m48;
    /* 0x48 */ be<s16> m4C;
    /* 0x4A */ be<s16> m4E;
    /* 0x4C */ be<s16> m50;
    /* 0x4E */ be<s16> m52;
    /* 0x50 */ be<s16> m54;
    /* 0x52 */ u8 _52[2];
    /* 0x54 */ be<f32> m58;
    /* 0x58 */ be<u32> __vtbl;
};
WWHD_SIZE(kiHIO_c, 0x5C);

enum {
    dRes_INDEX_KI_BAS_ATTACK1_e = 0x7,
    dRes_INDEX_KI_BAS_BITA1_e = 0x8,
    dRes_INDEX_KI_BAS_DAMAGE1_e = 0x9,
    dRes_INDEX_KI_BAS_FLY1_e = 0xA,
    dRes_INDEX_KI_BAS_FLY2_e = 0xB,
    dRes_INDEX_KI_BAS_WAIT1_e = 0xC,
    dRes_INDEX_KI_BCK_ATTACK1_e = 0xF,
    dRes_INDEX_KI_BCK_BITA1_e = 0x10,
    dRes_INDEX_KI_BCK_DAMAGE1_e = 0x11,
    dRes_INDEX_KI_BCK_FLY1_e = 0x12,
    dRes_INDEX_KI_BCK_FLY2_e = 0x13,
    dRes_INDEX_KI_BCK_WAIT1_e = 0x14,
    dRes_INDEX_KI_BDL_FK_e = 0x17,
    dRes_INDEX_KI_BDL_KI_e = 0x18,
    dRes_INDEX_KI_BTK_FK_e = 0x1B,
};
enum { fpcNm_KI_e = 0xD7 };
enum { DSNAP_TYPE_KI = 0xAE };
enum { JA_SE_CV_KI_ATTACK = 0x481D };

/* ---- file statics ---- */
static kiHIO_c& l_kiHIO() { return *gabi::at<kiHIO_c>(0x10464B4C); }
static be<u8>& hio_set() { return *gabi::at<be<u8>>(0x101B8168); } /* HD: in .data */
static be<s32>& ki_all_count() { return *gabi::at<be<s32>>(0x10464B28); }
static be<s32>& ki_fight_count() { return *gabi::at<be<s32>>(0x10464B2C); }
/* static u16 ki_tex_anm_idx[4] (HD: plus a second table for the fire keese), ki_tex_max_frame[4],
 * ki_tex_loop[4] */
static u16 ki_tex_anm_idx(u32 i) { return gabi::load<u16>(0x101B816C + 2 * i); }
static u16 fk_tex_anm_idx(u32 i) { return gabi::load<u16>(0x101B8174 + 2 * i); }
static u16 ki_tex_max_frame(u32 i) { return gabi::load<u16>(0x101B8158 + 2 * i); }
static u8 ki_tex_loop(u32 i) { return gabi::load<u8>(0x101B8160 + i); }


/* HD: fopAcM_monsSeStart inline (the process id is read before the reverb call) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 se, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}

/* dBgS_LinChk (stack object), HD layout as in d_a_kamome (this TU's vtables) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk;
    /* 0x04 */ be<u32> mpGrpPassChk;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
static inline void ki_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x10012BD4;
    c->__vtbl_64 = 0x10012BF4;
    c->__vtbl_58 = 0x10012C04;
    c->__vtbl_20 = 0x10012BE4;
}
static inline void ki_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x10012C04;
    c->__vtbl_64 = 0x10012B94;
    c->__vtbl_20 = 0x10012B84;
    cBgS_LinChk_dt(c, 0);
}

