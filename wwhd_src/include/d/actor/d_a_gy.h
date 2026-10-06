/* daGy_c (Gyorg), WWHD layout. 
 *
 * GameCube -> WWHD: every member is +0x11C (fopEn_enemy_c grew by 0x11C; the members keep their
 * GameCube sizes). Size 0xFAC (constructor 02166FC4; GameCube 0xE90).
 * The GameCube decompilation has only "Nonmatching" stubs for this unit: the member names below
 * are ours, derived from the WWHD code. */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_gy_ctrl.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, as in d_a_cc.h) */
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

/* the HIO (one static instance at 0x1046446C, size 0x19C). Field names are ours. */
struct daGy_HIO_c {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<f32> m004;
    /* 0x008 */ be<f32> m008;
    /* 0x00C */ be<s16> m00C;
    /* 0x00E */ u8 _00E[2];
    /* 0x010 */ be<f32> m010, m014, m018, m01C, m020, m024, m028, m02C, m030, m034, m038, m03C, m040, m044;
    /* 0x048 */ be<f32> mSpeedSlow;      /* aim speed (ctrl target 0, outside the stroke window) */
    /* 0x04C */ be<f32> mSpeedSlowStroke; /* aim speed (ctrl target 0, inside the stroke window) */
    /* 0x050 */ be<f32> mSpeedFast;      /* aim speed (ctrl target != 0) */
    /* 0x054 */ be<f32> mSpeedFastStroke;
    /* 0x058 */ be<f32> m058, m05C, m060, m064, m068, m06C, m070, m074, m078, m07C, m080, m084, m088, m08C, m090;
    /* 0x094 */ be<u8> mDebugOffset;      /* setMtx: extra translation (mDebugOffsetPos) */
    /* 0x095 */ be<u8> mDebugMode;        /* modeProcCall: force mDebugModeNo */
    /* 0x096 */ be<u8> m096, m097, m098, m099;
    /* 0x09A */ be<s16> mDebugModeNo;
    /* 0x09C */ be<f32> m09C, m0A0, m0A4, m0A8, m0AC, m0B0, m0B4;
    /* 0x0B8 */ cXyz mDebugOffsetPos;
    /* 0x0C4 */ be<f32> m0C4, m0C8;
    /* 0x0CC */ be<f32> mStrokeStart;     /* setAimSpeedF: animation frame window */
    /* 0x0D0 */ be<f32> mStrokeEnd;
    /* 0x0D4 */ cXyz mHeadOffset;         /* _nodeControl: head joint offset */
    /* 0x0E0 */ be<f32> m0E0, m0E4, m0E8;
    /* 0x0EC */ cXyz mSplashOffsetLow;    /* setMtx */
    /* 0x0F8 */ cXyz mSplashOffsetHigh;
    /* 0x104 */ be<f32> m104, m108;
    /* 0x10C */ be<s16> m10C, m10E, m110, m112, m114, m116, m118;
    /* 0x11A */ u8 _11A[2];
    /* 0x11C */ be<u8> m11C[9];
    /* 0x125 */ u8 _125[3];
    /* 0x128 */ be<f32> m128, m12C;
    /* 0x130 */ be<s16> m130, m132;
    /* 0x134 */ be<f32> m134, m138, m13C;
    /* 0x140 */ be<s16> m140, m142;
    /* 0x144 */ be<f32> m144, m148, m14C, m150, m154, m158, m15C, m160;
    /* 0x164 */ be<s16> m164;
    /* 0x166 */ u8 _166[2];
    /* 0x168 */ be<f32> m168, m16C, m170, m174, m178, m17C;
    /* 0x180 */ be<s16> mHeadAngle;       /* _nodeControl: max angle to the player */
    /* 0x182 */ u8 _182[2];
    /* 0x184 */ be<f32> mHeadDist;        /* _nodeControl: max distance to the player */
    /* 0x188 */ be<f32> mHeadSlerp;
    /* 0x18C */ be<f32> m18C, m190, m194;
    /* 0x198 */ be<s16> m198;
    /* 0x19A */ u8 _19A[2];
};
WWHD_OFFSET(daGy_HIO_c, mDebugModeNo, 0x9A);
WWHD_OFFSET(daGy_HIO_c, m0C4, 0xC4);
WWHD_OFFSET(daGy_HIO_c, m10C, 0x10C);
WWHD_OFFSET(daGy_HIO_c, m128, 0x128);
WWHD_OFFSET(daGy_HIO_c, m140, 0x140);
WWHD_OFFSET(daGy_HIO_c, m164, 0x164);
WWHD_OFFSET(daGy_HIO_c, mHeadAngle, 0x180);
WWHD_OFFSET(daGy_HIO_c, m198, 0x198);
WWHD_SIZE(daGy_HIO_c, 0x19C);
#define GY_HIO_ADDR 0x1046446C
static inline daGy_HIO_c* gy_hio() { return gabi::at<daGy_HIO_c>(GY_HIO_ADDR); }

/* dPa_waveEcallBack (0x64; vtable +0, three cXyz members at +0x3C/+0x48/+0x54, emitter +0x60) and
 * dPa_splashEcallBack (0x1C; emitter +0x18) */
struct dPa_waveEcallBack_gy {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x60 - 0x04];
    /* 0x60 */ gptr<JPABaseEmitter> mpEmitter;
};
WWHD_SIZE(dPa_waveEcallBack_gy, 0x64);
struct dPa_splashEcallBack_gy {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x18 - 0x04];
    /* 0x18 */ gptr<JPABaseEmitter> mpEmitter;
};
WWHD_SIZE(dPa_splashEcallBack_gy, 0x1C);

struct daGy_c : fopEn_enemy_c {
    /* 0x3C8 */ be<s32> mIndex;            /* the controller's spawn counter at creation */
    /* 0x3CC */ be<s32> mMode;
    /* 0x3D0 */ be<s32> mPrevMode;         /* the mode before the damage */
    /* 0x3D4 */ gptr<daGyCtrl_c> mpCtrl;
    /* 0x3D8 */ cXyz m3D8;
    /* 0x3E4 */ request_of_phase_process_class mPhase;
    /* 0x3EC */ u8 _3EC[4];
    /* 0x3F0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3F4 */ u8 mInvisibleModel[8];   /* mDoExt_invisibleModel */
    /* 0x3FC */ dBgS_ObjAcch mAcch;
    /* 0x5C0 */ dBgS_AcchCir mAcchCir;
    /* 0x600 */ be<f32> mBodyY;            /* setMtx: model height offset */
    /* 0x604 */ be<f32> mBodyYTarget;
    /* 0x608 */ be<f32> mBodyYStep;
    /* 0x60C */ be<f32> mAimSpeedF;
    /* 0x610 */ u8 _610[4];
    /* 0x614 */ be<s32> m614;
    /* 0x618 */ be<s32> mIceTimer;         /* ice arrow: frames until frozen (-1: none) */
    /* 0x61C */ be<s32> mLightTimer;       /* light arrow: frames until the light shrink (-1: none) */
    /* 0x620 */ be<u8> mHitPlayerShip;     /* modeAttackPlayer: the ship (0xA8) was hit once */
    /* 0x621 */ u8 _621[3];
    /* 0x624 */ be<f32> mApproachRate;     /* modeDive: approach to the spawn point */
    /* 0x628 */ dCcD_Sph mHeadSph;
    /* 0x754 */ dCcD_Sph mSph;
    /* 0x880 */ dCcD_Cps mCps;
    /* 0x9B8 */ cXyz m9B8;
    /* 0x9C4 */ dCcD_Stts mStts;
    /* 0xA00 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0xA04 */ u8 _A04[4];
    /* 0xA08 */ be<s32> mDisappearTimer;   /* modeDeleteBomb (-1: not started) */
    /* 0xA0C */ be<s32> mA0C;
    /* 0xA10 */ be<s16> mA10;
    /* 0xA12 */ u8 _A12[2];
    /* 0xA14 */ cXyz mTargetPos;
    /* 0xA20 */ cXyz mA20;
    /* 0xA2C */ u8 _A2C[4];
    /* 0xA30 */ be<s32> mTimer;
    /* 0xA34 */ be<f32> mBodyYScale;
    /* 0xA38 */ be<f32> mOldGravity;       /* gravity of the previous frame (0 on the water) */
    /* 0xA3C */ be<f32> mDamageSpeed;      /* checkTgHit: by weapon (HIO 0x14C..0x158) */
    /* 0xA40 */ be<s32> mBiteCount;
    /* 0xA44 */ be<s32> mSubMode;
    /* 0xA48 */ enemyice_l mEnemyIce;
    /* 0xE00 */ be<u32> mpJntHit;
    /* 0xE04 */ be<u8> mFreezeType;        /* 9: ice arrow, 11: light arrow */
    /* 0xE05 */ u8 _E05[3];
    /* 0xE08 */ cXyz mHeadTarget;
    /* 0xE14 */ u8 mHeadQuat[0x10];      /* Quaternion */
    /* 0xE24 */ cXyz mHeadPos;
    /* 0xE30 */ be<s8> mAnmIdx;
    /* 0xE31 */ be<s8> mAnmIdxNext;
    /* 0xE32 */ be<s8> mAnmIdxOld;        /* dLib_setAnm: the animation before (0: dive, 6: with circle) */
    /* 0xE33 */ u8 _E33;
    /* 0xE34 */ dPa_waveEcallBack_gy mWave[2];
    /* 0xEFC */ dPa_splashEcallBack_gy mSplash;
    /* 0xF18 */ cXyz mSplashPos;
    /* 0xF24 */ csXyz mSplashAngle;
    /* 0xF2A */ u8 _F2A[2];
    /* 0xF2C */ be<f32> mSplashRate;
    /* 0xF30 */ dBgS_LinChk mLinChk;
    /* 0xF9C */ be<u8> mLineHit;
    /* 0xF9D */ u8 _F9D[3];
    /* 0xFA0 */ cXyz mFA0;

    void _nodeControl(J3DNode* node, J3DModel* model);
    BOOL _createHeap();
    void modeDiveInit();
    f32 getWaterY();
    void setMtx();
    void createWave();
    void createInit();
    cPhs_State _create();
    BOOL _delete();
    void lineCheck(cXyz* start, cXyz* end);
    void setAtCollision();
    void setCollision();
    void modeDeleteBombInit();
    void modeDeleteInit();
    void modeDamageInit();
    void checkTgHit();
    void setAnm();
    void setWave();
    void modeProcCall();
    BOOL _execute();
    void drawDebug();
    BOOL _draw();
    void modeCircleInit();
    void setAimSpeedF();
    void modeDive();
    void modeWithCircleInit();
    void modeAttackPlayerInit();
    void modeAttackInit();
    void modeCircle();
    void modeAttackBackInit();
    void modeAttack();
    void modeAttackPlayer();
    void modeAttackBack();
    void modeWithAttackInit();
    void modeWithAttack();
    void modeWithCircle();
    void modeDamage();
    void modeDelete();
    void modeDeleteBomb();
};
WWHD_OFFSET(daGy_c, mMode, 0x3CC);
WWHD_OFFSET(daGy_c, mPhase, 0x3E4);
WWHD_OFFSET(daGy_c, mAcch, 0x3FC);
WWHD_OFFSET(daGy_c, mHeadSph, 0x628);
WWHD_OFFSET(daGy_c, mCps, 0x880);
WWHD_OFFSET(daGy_c, mStts, 0x9C4);
WWHD_OFFSET(daGy_c, mSubMode, 0xA44);
WWHD_OFFSET(daGy_c, mEnemyIce, 0xA48);
WWHD_OFFSET(daGy_c, mHeadPos, 0xE24);
WWHD_OFFSET(daGy_c, mWave, 0xE34);
WWHD_OFFSET(daGy_c, mLinChk, 0xF30);
WWHD_SIZE(daGy_c, 0xFAC);

/* ---- this TU's statics / vtables ---- */
#define GY_ARC STR(0x10010C4C)        /* "Gy" */
#define GY_SAFESTRING_VTBL 0x100108B0
#define GY_VTBL 0x100109B8
#define GY_AAB_VTBL 0x100108C8
static const dBgS_ObjAcch_vt GY_OBJACCH_VT = {0x100108F8, 0x10010918, 0x10010908};
static const dBgS_LinChk_vt GY_LINCHK_VT = {0x10010968, 0x10010978, 0x10010998, 0x10010988};
