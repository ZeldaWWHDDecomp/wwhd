/* ph_class (Peahat / Seahat), WWHD layout. 
 *
 * GameCube -> WWHD: +0x11C up to the second enemyfire (the GameCube header's names are kept,
 * mGCOFFSET fields at GameCube offset + 0x11C); enemyfire is 0x22C in HD (GameCube 0x228), so
 * the invisible models and mpJntHit are +0x124. Size 0x12D8 (constructor 023CF44C).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

#ifndef D_A_DAMAGEREACTION_L
#define D_A_DAMAGEREACTION_L
/* c_damagereaction (SHARED-CANDIDATE: same as in d_a_pt.h) */
struct enemyice {
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
WWHD_OFFSET(enemyice, mCylHeight, 0x19C);
WWHD_SIZE(enemyice, 0x3B8);

/* enemyfire (c_damagereaction), HD 0x22C */
struct enemyfire {
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
    /* 0x208 */ u8 mLight[0x20]; /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> m228;    /* HD */
};
WWHD_OFFSET(enemyfire, mSph, 0xDC);
WWHD_SIZE(enemyfire, 0x22C);

#endif

struct ph_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ u8 _3D0[4];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpPropellerMorf;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3DC */ cXyz m02C0;
    /* 0x3E8 */ cXyz m02CC;
    /* 0x3F4 */ cXyz m02D8;
    /* 0x400 */ cXyz m02E4;
    /* 0x40C */ cXyz m02F0;
    /* 0x418 */ cXyz m02FC;
    /* 0x424 */ cXyz m0308;
    /* 0x430 */ cXyz m0314;
    /* 0x43C */ cXyz m0320;             /* target scale (damage) */
    /* 0x448 */ cXyz m032C;
    /* 0x454 */ u8 _454[2];
    /* 0x456 */ be<s16> m033A;          /* propeller angle */
    /* 0x458 */ u8 _458[2];
    /* 0x45A */ be<u8> mType;           /* 0 Peahat, 1 Seahat */
    /* 0x45B */ be<u8> m033F;
    /* 0x45C */ be<u8> m0340;
    /* 0x45D */ be<u8> m0341;
    /* 0x45E */ be<u8> m0342;
    /* 0x45F */ be<u8> m0343;           /* hit counter */
    /* 0x460 */ be<u8> m0344;
    /* 0x461 */ u8 _461;
    /* 0x462 */ be<s16> m0346;
    /* 0x464 */ be<s16> m0348;          /* propeller speed */
    /* 0x466 */ be<s16> m034A;          /* target propeller speed */
    /* 0x468 */ be<s16> m034C;
    /* 0x46A */ be<s16> m034E;          /* spin speed (damage) */
    /* 0x46C */ be<s16> m0350;
    /* 0x46E */ be<s16> m0352;          /* target angle */
    /* 0x470 */ be<s16> m0354;          /* propeller sound flags */
    /* 0x472 */ be<s16> m0356;
    /* 0x474 */ be<s16> m0358;
    /* 0x476 */ be<s16> m035A;
    /* 0x478 */ be<s16> m035C;          /* pitch wobble */
    /* 0x47A */ be<s16> m035E;          /* roll wobble */
    /* 0x47C */ be<s16> m0360;          /* spin speed */
    /* 0x47E */ be<s16> m0362;
    /* 0x480 */ be<s16> m0364;
    /* 0x482 */ be<s16> m0366;          /* timer */
    /* 0x484 */ be<s16> m0368;
    /* 0x486 */ u8 _486[2];
    /* 0x488 */ be<s16> m036C;          /* hit cooldown */
    /* 0x48A */ be<s16> m036E;
    /* 0x48C */ be<s16> m0370;
    /* 0x48E */ be<s16> m0372;          /* Seahat: attack delay */
    /* 0x490 */ be<s32> m0374;
    /* 0x494 */ be<f32> m0378;          /* target height above the player */
    /* 0x498 */ be<f32> m037C;
    /* 0x49C */ be<f32> m0380;
    /* 0x4A0 */ be<f32> m0384;          /* rising speed */
    /* 0x4A4 */ be<f32> m0388;          /* rising target height */
    /* 0x4A8 */ be<f32> m038C;
    /* 0x4AC */ be<f32> m0390;
    /* 0x4B0 */ be<f32> m0394;
    /* 0x4B4 */ be<f32> m0398;
    /* 0x4B8 */ be<f32> m039C;
    /* 0x4BC */ be<f32> m03A0;
    /* 0x4C0 */ dBgS_AcchCir mAcchCir;
    /* 0x500 */ dBgS_Acch mAcch;
    /* 0x6C4 */ u8 mParticleCallBack[0x14]; /* dPa_rippleEcallBack */
    /* 0x6D8 */ cXyz m05BC;
    /* 0x6E4 */ u8 _6E4[8];
    /* 0x6EC */ dCcD_Stts mStts;
    /* 0x728 */ dCcD_Sph mBodySph;
    /* 0x854 */ dCcD_Cyl mAtCyl;
    /* 0x984 */ dCcD_Cyl mTgCyl;
    /* 0xAB4 */ enemyice mEnemyIce;
    /* 0xE6C */ enemyfire mBodyEnemyFire;
    /* 0x1098 */ enemyfire mPropellerEnemyFire;
    /* 0x12C4 */ u8 mPropellerInvisibleModel[8]; /* mDoExt_invisibleModel */
    /* 0x12CC */ u8 mBodyInvisibleModel[8];
    /* 0x12D4 */ be<u32> mpJntHit;
};
WWHD_OFFSET(ph_class, mpPropellerMorf, 0x3D4);
WWHD_OFFSET(ph_class, mType, 0x45A);
WWHD_OFFSET(ph_class, m037C, 0x498);
WWHD_OFFSET(ph_class, mAcch, 0x500);
WWHD_OFFSET(ph_class, mStts, 0x6EC);
WWHD_OFFSET(ph_class, mAtCyl, 0x854);
WWHD_OFFSET(ph_class, mEnemyIce, 0xAB4);
WWHD_OFFSET(ph_class, mBodyEnemyFire, 0xE6C);
WWHD_OFFSET(ph_class, mPropellerInvisibleModel, 0x12C4);
WWHD_SIZE(ph_class, 0x12D8);
