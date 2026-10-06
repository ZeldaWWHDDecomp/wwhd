/* cc_class (ChuChu), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; HD has no mDoExt_J3DModelPacketS m2DC (0x14), so the
 * members from mBehaviorType (GameCube 0x2F0) on are +0x108. Size 0xF10 (constructor 02110BB4;
 * GameCube 0xE04). */
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

struct cc_class {
    /* 0x000 */ fopEn_enemy_c actor;
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> m2B4;
    /* 0x3D4 */ gptr<mDoExt_brkAnm> m2B8;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> m2BC;
    /* 0x3DC */ gptr<mDoExt_brkAnm> m2C0;
    /* 0x3E0 */ gptr<mDoExt_McaMorf> m2C4;
    /* 0x3E4 */ gptr<mDoExt_brkAnm> m2C8;
    /* 0x3E8 */ gptr<mDoExt_brkAnm> m2CC;
    /* 0x3EC */ gptr<mDoExt_btkAnm> m2D0;
    /* 0x3F0 */ gptr<mDoExt_btkAnm> m2D4;
    /* 0x3F4 */ gptr<mDoExt_McaMorf> m2D8;
    /* HD: no mDoExt_J3DModelPacketS m2DC */
    /* 0x3F8 */ be<u8> mBehaviorType;
    /* 0x3F9 */ be<u8> mColorType;
    /* 0x3FA */ be<u8> mNoticeRangeByte;
    /* 0x3FB */ be<u8> mDisableSpawnSwitchNo;
    /* 0x3FC */ be<u8> mCurrAction;
    /* 0x3FD */ be<u8> m2F5;
    /* 0x3FE */ be<u8> m2F6;
    /* 0x3FF */ be<u8> m2F7;
    /* 0x400 */ be<u8> m2F8;
    /* 0x401 */ be<u8> m2F9;
    /* 0x402 */ be<u8> m2FA;
    /* 0x403 */ be<u8> m2FB;
    /* 0x404 */ be<u8> m2FC;
    /* 0x405 */ be<u8> m2FD;
    /* 0x406 */ be<u8> m2FE;
    /* 0x407 */ be<u8> m2FF;
    /* 0x408 */ be<u8> m300;
    /* 0x409 */ be<u8> m301;
    /* 0x40A */ be<u8> m302;
    /* 0x40B */ u8 _40B;
    /* 0x40C */ be<u8> m304;
    /* 0x40D */ u8 _40D;
    /* 0x40E */ be<s16> m306;
    /* 0x410 */ be<s16> m308;
    /* 0x412 */ u8 _412[2];
    /* 0x414 */ be<u32> m30C;
    /* 0x418 */ csXyz m310;
    /* 0x41E */ csXyz m316;
    /* 0x424 */ be<s16> m31C;
    /* 0x426 */ u8 _426[2];
    /* 0x428 */ be<s32> m320;
    /* 0x42C */ be<f32> m324;
    /* 0x430 */ be<f32> m328;
    /* 0x434 */ be<f32> m32C;
    /* 0x438 */ be<f32> m330;
    /* 0x43C */ be<f32> m334;
    /* 0x440 */ be<f32> m338;
    /* 0x444 */ be<f32> mNoticeRange;
    /* 0x448 */ be<f32> m340;
    /* 0x44C */ be<f32> m344;
    /* 0x450 */ be<s16> m348;
    /* 0x452 */ be<s16> m34A;
    /* 0x454 */ be<s16> m34C;
    /* 0x456 */ be<s16> m34E[7];
    /* 0x464 */ be<s16> m35C[5];
    /* 0x46E */ u8 _46E[2];
    /* 0x470 */ dPa_followEcallBack m368;
    /* 0x484 */ dPa_followEcallBack m37C;
    /* 0x498 */ u8 m390[0x14];                 /* dPa_rippleEcallBack */
    /* 0x4AC */ cXyz m3A4;
    /* 0x4B8 */ u8 _4B8[0xA];
    /* 0x4C2 */ be<s16> m3BA;
    /* 0x4C4 */ cXyz m3BC;
    /* 0x4D0 */ u8 _4D0[0x18];
    /* 0x4E8 */ cXyz m3E0[6];
    /* 0x530 */ cXyz m428[6];
    /* 0x578 */ cXyz m470;
    /* 0x584 */ dBgS_AcchCir mAcchCir;
    /* 0x5C4 */ dBgS_ObjAcch mAcch;
    /* 0x788 */ dCcD_Stts mStts;
    /* 0x7C4 */ dCcD_Cyl mCyl;
    /* 0x8F4 */ Mtx34 m7EC;
    /* 0x924 */ enemyice_l mEnemyIce;
    /* 0xCDC */ enemyfire_l mEnemyFire;
    /* 0xF08 */ u8 mDFC[8];                    /* mDoExt_invisibleModel */
};
WWHD_OFFSET(cc_class, mBehaviorType, 0x3F8);
WWHD_OFFSET(cc_class, m30C, 0x414);
WWHD_OFFSET(cc_class, m320, 0x428);
WWHD_OFFSET(cc_class, m368, 0x470);
WWHD_OFFSET(cc_class, m3BA, 0x4C2);
WWHD_OFFSET(cc_class, m470, 0x578);
WWHD_OFFSET(cc_class, mAcchCir, 0x584);
WWHD_OFFSET(cc_class, mStts, 0x788);
WWHD_OFFSET(cc_class, mCyl, 0x7C4);
WWHD_OFFSET(cc_class, mEnemyIce, 0x924);
WWHD_OFFSET(cc_class, mEnemyFire, 0xCDC);
WWHD_SIZE(cc_class, 0xF10);
