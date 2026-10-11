/* daGy_c (Gyorg), WWHD layout. 
 *
 * GameCube -> WWHD: every member is +0x11C (fopEn_enemy_c grew by 0x11C; the members keep their
 * GameCube sizes). Size 0xFAC (constructor 02166FC4; GameCube 0xE90).
 * The GameCube decompilation has only "Nonmatching" stubs for this unit: the member names below
 * are ours, derived from the WWHD code. */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_gy_ctrl.h"
#include "d/d_jnt_hit.h"
#include "gc_api/d/d_lib.h"

/* the HIO (one static instance at 0x1046446C, size 0x19C). Field names are ours. */
struct daGy_HIO_c {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<f32> mSwimAnimationScale;
    /* 0x008 */ be<f32> mMinimumAnimationSpeed;
    /* 0x00C */ be<s16> mFreezeDuration;
    /* 0x00E */ u8 _00E[2];
    /* 0x010 */ be<f32> mSplashSpeed, mMaxSplashSpeed, mWaveSpeed, mWaveDistanceSpeed, mWavePitchDifference,
        mWaveMaxSpeed, mCollapseNearX, mCollapseNearY, mCollapseNearZ, mCollapseFarX, mCollapseFarY,
        mCollapseFarZ, mEscortSpeedOffset, mAttackApproachDistance;
    /* 0x048 */ be<f32> mSpeedSlow;       /* aim speed (ctrl target 0, outside the stroke window) */
    /* 0x04C */ be<f32> mSpeedSlowStroke; /* aim speed (ctrl target 0, inside the stroke window) */
    /* 0x050 */ be<f32> mSpeedFast;       /* aim speed (ctrl target != 0) */
    /* 0x054 */ be<f32> mSpeedFastStroke;
    /* 0x058 */ be<f32> mPlayerApproachSpeed, mPlayerBiteSpeed, mPlayerLungeSpeed, mShipAttackSpeed,
        mEscortAttackSpeed, mCircleShipSpeed, mEscortShipSpeed, mUnusedTuning0, mUnusedTuning1,
        mUnusedTuning2, mUnusedTuning3, mUnusedTuning4, mHeadRadius, mBodyRadius, mActorScale;
    /* 0x094 */ be<u8> mDebugOffset; /* setMtx: extra translation (mDebugOffsetPos) */
    /* 0x095 */ be<u8> mDebugMode;   /* modeProcCall: force mDebugModeNo */
    /* 0x096 */ be<u8> mHideModel, mFloatOnWater, mUnusedFlag, mDeleteAfterEscortAttack;
    /* 0x09A */ be<s16> mDebugModeNo;
    /* 0x09C */ be<f32> mEscortJumpPlayerDistance, mDiveHeight, mBiteHeadHeight, mAttackHeadHeight,
        mCircleHeight, mAttackHeight, mBiteHeight;
    /* 0x0B8 */ cXyz mDebugOffsetPos;
    /* 0x0C4 */ be<f32> mHeightStep, mDiveHeightStep;
    /* 0x0CC */ be<f32> mStrokeStart; /* setAimSpeedF: animation frame window */
    /* 0x0D0 */ be<f32> mStrokeEnd;
    /* 0x0D4 */ cXyz mHeadOffset; /* _nodeControl: head joint offset */
    /* 0x0E0 */ be<f32> mUnusedTuning7, mUnusedTuning8, mUnusedTuning9;
    /* 0x0EC */ cXyz mSplashOffsetLow; /* setMtx */
    /* 0x0F8 */ cXyz mSplashOffsetHigh;
    /* 0x104 */ be<f32> mRecoilSpeed, mRecoilVerticalSpeed;
    /* 0x10C */ be<s16> mAttackTime, mEscortAttackTime, mEscortCircleTime, mShipCircleMinTime,
        mShipCircleMaxTime, mPlayerCircleMinTime, mPlayerCircleMaxTime;
    /* 0x11A */ u8 _11A[2];
    /* 0x11C */ be<u8> mUnusedFlags[9];
    /* 0x125 */ u8 _125[3];
    /* 0x128 */ be<f32> mUnusedTuning10, mUnusedTuning11;
    /* 0x130 */ be<s16> mMinEscortRandomTime, mEscortRandomTimeRange;
    /* 0x134 */ be<f32> mUnusedTuning12, mEscortJumpSpeedY, mUnusedTuning13;
    /* 0x140 */ be<s16> mEscortOuterAngle, mEscortInnerAngle;
    /* 0x144 */ be<f32> mHomeRadius, mPlayerAvoidanceRadius, mBombDamageSpeed, mArrowDamageSpeed,
        mHookshotDamageSpeed, mBoomerangDamageSpeed, mDeathVerticalSpeed, mUnusedTuning14;
    /* 0x164 */ be<s16> mDeathFloatTime;
    /* 0x166 */ u8 _166[2];
    /* 0x168 */ be<f32> mDeathSinkHeight, mDeathFloatHeight, mDeathFloatAmplitude, mHeightApproachRate,
        mDeathFloatStep, mDeathFloatApproachRate;
    /* 0x180 */ be<s16> mHeadAngle; /* _nodeControl: max angle to the player */
    /* 0x182 */ u8 _182[2];
    /* 0x184 */ be<f32> mHeadDist; /* _nodeControl: max distance to the player */
    /* 0x188 */ be<f32> mHeadSlerp;
    /* 0x18C */ be<f32> mUnusedTuning15, mUnusedTuning16, mUnusedTuning17;
    /* 0x198 */ be<s16> mBiteCount;
    /* 0x19A */ u8 _19A[2];
};
WWHD_OFFSET(daGy_HIO_c, mDebugModeNo, 0x9A);
WWHD_OFFSET(daGy_HIO_c, mHeightStep, 0xC4);
WWHD_OFFSET(daGy_HIO_c, mAttackTime, 0x10C);
WWHD_OFFSET(daGy_HIO_c, mUnusedTuning10, 0x128);
WWHD_OFFSET(daGy_HIO_c, mEscortOuterAngle, 0x140);
WWHD_OFFSET(daGy_HIO_c, mDeathFloatTime, 0x164);
WWHD_OFFSET(daGy_HIO_c, mHeadAngle, 0x180);
WWHD_OFFSET(daGy_HIO_c, mBiteCount, 0x198);
WWHD_SIZE(daGy_HIO_c, 0x19C);
extern daGy_HIO_c& l_HIO;
static inline daGy_HIO_c* gy_hio() { return &l_HIO; }

// GameCube particle callback members; the HD sizes are unchanged.
struct GyWaveHD {
    be<u32> __vtbl;
    be<s16> mState, mFadeTimer;
    be<f32> mVelFade1, mVel, mVelSpeed, mVelFade2, mMaxParticleVelocity;
    cXyz mCollapsePos[2];
    gptr<const cXyz> mpPos;
    gptr<const csXyz> mpRot;
    cXyz mRotMtx[3];
    gptr<JPABaseEmitter> mpBaseEmitter;
};
WWHD_OFFSET(GyWaveHD, mCollapsePos, 0x1C);
WWHD_OFFSET(GyWaveHD, mRotMtx, 0x3C);
WWHD_OFFSET(GyWaveHD, mpBaseEmitter, 0x60);
WWHD_SIZE(GyWaveHD, 0x64);
struct GySplashHD {
    be<u32> __vtbl;
    be<s16> mState;
    u8 mPadding[2];
    be<f32> mScaleTimer, mMaxScaleTimer;
    gptr<const cXyz> mpPos;
    gptr<const csXyz> mpRot;
    gptr<JPABaseEmitter> mpBaseEmitter;
};
WWHD_OFFSET(GySplashHD, mScaleTimer, 8);
WWHD_OFFSET(GySplashHD, mpBaseEmitter, 0x18);
WWHD_SIZE(GySplashHD, 0x1C);

struct GyIceHD {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> mReactionFlag;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> mUnusedFloat0;
    /* 0x02C */ be<f32> mUnusedFloat1;
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> mUnusedByte;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(GyIceHD, 0x3B8);

struct daGy_c : fopEn_enemy_c {
    enum Mode {
        MODE_DIVE,
        MODE_CIRCLE,
        MODE_ATTACK,
        MODE_ATTACK_PLAYER,
        MODE_ATTACK_BACK,
        MODE_WITH_ATTACK,
        MODE_WITH_CIRCLE,
        MODE_DAMAGE,
        MODE_DELETE,
        MODE_DELETE_BOMB
    };
    /* 0x3C8 */ be<s32> mIndex; /* the controller's spawn counter at creation */
    /* 0x3CC */ be<s32> mMode;
    /* 0x3D0 */ be<s32> mPrevMode; /* the mode before the damage */
    /* 0x3D4 */ gptr<daGyCtrl_c> mpCtrl;
    /* 0x3D8 */ cXyz mSpawnPos;
    /* 0x3E4 */ request_of_phase_process_class mPhase;
    /* 0x3EC */ u8 _3EC[4];
    /* 0x3F0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3F4 */ u8 mInvisibleModel[8]; /* mDoExt_invisibleModel */
    /* 0x3FC */ dBgS_ObjAcch mAcch;
    /* 0x5C0 */ dBgS_AcchCir mAcchCir;
    /* 0x600 */ be<f32> mBodyY; /* setMtx: model height offset */
    /* 0x604 */ be<f32> mBodyYTarget;
    /* 0x608 */ be<f32> mBodyYStep;
    /* 0x60C */ be<f32> mAimSpeedF;
    /* 0x610 */ u8 _610[4];
    /* 0x614 */ be<s32> mEscortRandomTime;
    /* 0x618 */ be<s32> mIceTimer;   /* ice arrow: frames until frozen (-1: none) */
    /* 0x61C */ be<s32> mLightTimer; /* light arrow: frames until the light shrink (-1: none) */
    /* 0x620 */ be<u8> mHitPlayer;   /* modeAttackPlayer: the ship (0xA8) was hit once */
    /* 0x621 */ u8 _621[3];
    /* 0x624 */ be<f32> mApproachRate; /* modeDive: approach to the spawn point */
    /* 0x628 */ dCcD_Sph mHeadSph;
    /* 0x754 */ dCcD_Sph mSph;
    /* 0x880 */ dCcD_Cps mCps;
    /* 0x9B8 */ cXyz mBodyCollisionPos;
    /* 0x9C4 */ dCcD_Stts mStts;
    /* 0xA00 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0xA04 */ u8 _A04[4];
    /* 0xA08 */ be<s32> mDisappearTimer; /* modeDeleteBomb (-1: not started) */
    /* 0xA0C */ be<s32> mParentWaitTimer;
    /* 0xA10 */ be<s16> mRecoilAngle;
    /* 0xA12 */ u8 _A12[2];
    /* 0xA14 */ cXyz mTargetPos;
    /* 0xA20 */ cXyz mAttackStartPos;
    /* 0xA2C */ u8 _A2C[4];
    /* 0xA30 */ be<s32> mTimer;
    /* 0xA34 */ be<f32> mBodyYScale;
    /* 0xA38 */ be<f32> mOldGravity;  /* gravity of the previous frame (0 on the water) */
    /* 0xA3C */ be<f32> mDamageSpeed; /* checkTgHit: by weapon (HIO 0x14C..0x158) */
    /* 0xA40 */ be<s32> mBiteCount;
    /* 0xA44 */ be<s32> mSubMode;
    /* 0xA48 */ GyIceHD mEnemyIce;
    /* 0xE00 */ be<u32> mpJntHit;
    /* 0xE04 */ be<u8> mFreezeType; /* 9: ice arrow, 11: light arrow */
    /* 0xE05 */ u8 _E05[3];
    /* 0xE08 */ cXyz mHeadTarget;
    /* 0xE14 */ Quaternion mHeadQuat; /* Quaternion */
    /* 0xE24 */ cXyz mHeadPos;
    /* 0xE30 */ be<s8> mAnmIdx;
    /* 0xE31 */ be<s8> mAnmIdxNext;
    /* 0xE32 */ be<s8> mAnmIdxOld; /* dLib_setAnm: the animation before (0: dive, 6: with circle) */
    /* 0xE33 */ u8 _E33;
    /* 0xE34 */ GyWaveHD mWave[2];
    /* 0xEFC */ GySplashHD mSplash;
    /* 0xF18 */ cXyz mSplashPos;
    /* 0xF24 */ csXyz mSplashAngle;
    /* 0xF2A */ u8 _F2A[2];
    /* 0xF2C */ be<f32> mSplashRate;
    /* 0xF30 */ dBgS_LinChk mLinChk;
    /* 0xF9C */ be<u8> mLineHit;
    /* 0xF9D */ u8 _F9D[3];
    /* 0xFA0 */ cXyz mLineEnd;

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
#define GY_VTBL 0x100109B8
#define GY_AAB_VTBL 0x100108C8
static const dBgS_LinChk_vt GY_LINCHK_VT = {0x10010968, 0x10010978, 0x10010998, 0x10010988};

// HD views used only by this translation unit.
struct GyColourGuardsHD {
    be<s32> mHead, mBody, mPadding, mShip;
};
struct GyColoursHD {
    GXColor mHead, mBody, mPadding, mShip;
};
struct GyColourSourcesHD {
    GXColor mHead, mShip, mBody;
};
struct GyAnimationIndicesHD {
    dLib_anm_idx_c mItems[9];
};
struct GyAnimationParametersHD {
    dLib_anm_prm_c mItems[12];
};
struct GyCharacterTableHD {
    u8 mPrefix[0x50AC];
    u8 mTable[1];
};
struct GyJointHD {
    be<u32> mpData;
    be<u16> mJointNo;
};
struct GyDebugHD {
    be<f32> mSplashScale, mSplashHeight;
    u8 mGap[0x148 - 8];
    be<f32> mBiteFrame;
    u8 mBiteGap[8];
    be<f32> mWaveScale;
    u8 mWaveGap[0x19A - 0x158];
    be<s16> mDiveAngle, mRiseAngle;
};
struct GyWaveOffsetsHD {
    be<s32> mLeftGuard, mRightGuard;
    cXyz mLeft, mRight;
};
struct GyEmitterHD {
    u8 mPrefix[0x28];
    cXyz mGlobalScale;
    u8 mGap[0x254 - 0x34];
    be<u32> mStatus;
};
struct GyQuaternionWordsHD {
    be<u32> mWords[4];
};
struct GyModeTableHD {
    be<u32> mWords[20];
};
struct GyModelFlagsHD {
    u8 mPrefix[0x74];
    be<u32> mFlags;
};
struct GyCapsuleHD {
    cM3dGLin mLine;
    be<f32> mRadius;
};
struct GyShapeVtablesHD {
    u8 mPrefix[0x18];
    be<u32> mAabVtable, mShapeVtable;
};
struct GyCapsuleSourceHD {
    u8 mBytes[0x4C];
};
WWHD_SIZE(GyColourGuardsHD, 0x10);
WWHD_SIZE(GyColoursHD, 0x10);
WWHD_SIZE(GyColourSourcesHD, 0xC);
WWHD_SIZE(GyAnimationIndicesHD, 0x48);
WWHD_SIZE(GyAnimationParametersHD, 0xC0);
WWHD_OFFSET(GyCharacterTableHD, mTable, 0x50AC);
WWHD_SIZE(GyCharacterTableHD, 0x50AD);
WWHD_OFFSET(GyJointHD, mJointNo, 4);
WWHD_SIZE(GyJointHD, 6);
WWHD_OFFSET(GyDebugHD, mBiteFrame, 0x148);
WWHD_OFFSET(GyDebugHD, mWaveScale, 0x154);
WWHD_OFFSET(GyDebugHD, mDiveAngle, 0x19A);
WWHD_SIZE(GyDebugHD, 0x19E);
WWHD_OFFSET(GyWaveOffsetsHD, mLeft, 8);
WWHD_SIZE(GyWaveOffsetsHD, 0x20);
WWHD_OFFSET(GyEmitterHD, mStatus, 0x254);
WWHD_SIZE(GyEmitterHD, 0x258);
WWHD_SIZE(GyQuaternionWordsHD, 0x10);
WWHD_SIZE(GyModeTableHD, 0x50);
WWHD_OFFSET(GyModelFlagsHD, mFlags, 0x74);
WWHD_SIZE(GyModelFlagsHD, 0x78);
WWHD_OFFSET(GyCapsuleHD, mRadius, 0x1C);
WWHD_SIZE(GyCapsuleHD, 0x20);
WWHD_SIZE(GyShapeVtablesHD, 0x20);
WWHD_SIZE(GyCapsuleSourceHD, 0x4C);

// HD: process ordinals differ from the GameCube profile table.
enum { fpcNm_SHIP_e = 0xA5, fpcNm_PLAYER_e = 0xA8, fpcNm_GY_CTRL_e = 0xE5, fpcNm_GY_CTRLB_e = 0xE6 };
enum { GY_BCK_INITIAL_e = 0xB, GY_BDL_BODY_e = 0xF };
enum {
    GY_AT_BOMB = 0x20,
    GY_AT_BOOMERANG = 0x40,
    GY_AT_NORMAL_ARROW = 0x4000,
    GY_AT_HOOKSHOT = 0x8000,
    GY_AT_FIRE_ARROW = 0x40000,
    GY_AT_ICE_ARROW = 0x80000,
    GY_AT_LIGHT_ARROW = 0x100000
};
struct GyJointHitDataHD {
    __jnt_hit_data_c mItems[8];
};
WWHD_SIZE(GyJointHitDataHD, 0x60);
