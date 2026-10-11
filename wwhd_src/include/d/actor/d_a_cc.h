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
    /* 0x228 */ be<f32> m228;     // HD: 1.0f in the constructor
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
    // HD: no mDoExt_J3DModelPacketS m2DC
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

// HD: private views used by the main unit; the shared actor layout above is unchanged.
template<class T> inline T* Cc_view(u32 address) { return gabi::at<T>(address); }
struct CcModel {
    u8 mPadding0[0x2C];
    /* 0x2C */ be<u32> mMatrixBlock;
    u8 mPadding30[0x7C];
    /* 0xAC */ be<u32> mModelData;
    u8 mPaddingB0[0x8];
    /* 0xB8 */ be<u32> mUserArea;
};
WWHD_OFFSET(CcModel, mMatrixBlock, 0x2C);
WWHD_OFFSET(CcModel, mModelData, 0xAC);
WWHD_OFFSET(CcModel, mUserArea, 0xB8);
WWHD_SIZE(CcModel, 0xBC);
struct CcMatrixBlock {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u16> mFlags;
    u8 mPadding6[0xA];
    /* 0x10 */ be<u32> mMatrices;
};
WWHD_OFFSET(CcMatrixBlock, mFlags, 0x4);
WWHD_OFFSET(CcMatrixBlock, mMatrices, 0x10);
WWHD_SIZE(CcMatrixBlock, 0x14);
struct CcModelData {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u32> mJointCount;
    /* 0x8 */ be<u32> mJoints;
    u8 mPaddingC[0x38];
    /* 0x44 */ be<u32> mTexMtxAnimation;
    /* 0x48 */ be<u32> mTevRegAnimation;
};
WWHD_OFFSET(CcModelData, mJointCount, 0x4);
WWHD_OFFSET(CcModelData, mJoints, 0x8);
WWHD_OFFSET(CcModelData, mTexMtxAnimation, 0x44);
WWHD_OFFSET(CcModelData, mTevRegAnimation, 0x48);
WWHD_SIZE(CcModelData, 0x4C);
struct CcJoint {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u16> mJointNo;
    u8 mPadding6[0x2];
    /* 0x8 */ be<u32> mCallback;
    u8 mPaddingC[0x10];
};
WWHD_OFFSET(CcJoint, mJointNo, 0x4);
WWHD_OFFSET(CcJoint, mCallback, 0x8);
WWHD_SIZE(CcJoint, 0x1C);
struct CcJointTree {
    u8 mPadding0[0x8];
    /* 0x8 */ be<u16> mJointCount;
};
WWHD_OFFSET(CcJointTree, mJointCount, 0x8);
WWHD_SIZE(CcJointTree, 0xA);
struct CcAnimation {
    u8 mPadding0[0x4];
    /* 0x4 */ be<f32> mFrame;
};
WWHD_OFFSET(CcAnimation, mFrame, 0x4);
WWHD_SIZE(CcAnimation, 0x8);
struct CcParticle {
    u8 mPadding0[0xF4];
    /* 0xF4 */ GXColor mToonColor;
};
WWHD_OFFSET(CcParticle, mToonColor, 0xF4);
WWHD_SIZE(CcParticle, 0xF8);
struct CcEmitter {
    u8 mPadding0[0x244];
    /* 0x244 */ GXColor mPrmColor;
    /* 0x248 */ GXColor mEnvColor;
};
WWHD_OFFSET(CcEmitter, mPrmColor, 0x244);
WWHD_OFFSET(CcEmitter, mEnvColor, 0x248);
WWHD_SIZE(CcEmitter, 0x24C);
struct CcRipple {
    u8 mPadding0[0x10];
    /* 0x10 */ be<f32> mRate;
};
WWHD_OFFSET(CcRipple, mRate, 0x10);
WWHD_SIZE(CcRipple, 0x14);
struct CcPlayer {
    u8 mPadding0[0x3C0];
    /* 0x3C0 */ be<u32> mNoResetFlg2;
    u8 mPadding3C4[0x20];
    /* 0x3E4 */ cXyz mSwordTopPos;
};
WWHD_OFFSET(CcPlayer, mNoResetFlg2, 0x3C0);
WWHD_OFFSET(CcPlayer, mSwordTopPos, 0x3E4);
WWHD_SIZE(CcPlayer, 0x3F0);
struct CcAcchVtables {
    u8 mPadding0[0x10];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ be<u32> mPassVtable;
    /* 0x18 */ be<u8> mObjectPass;
    u8 mPadding19[0x7];
    /* 0x20 */ be<u32> mObjectVtable;
};
WWHD_OFFSET(CcAcchVtables, mCheckVtable, 0x10);
WWHD_OFFSET(CcAcchVtables, mPassVtable, 0x14);
WWHD_OFFSET(CcAcchVtables, mObjectPass, 0x18);
WWHD_OFFSET(CcAcchVtables, mObjectVtable, 0x20);
WWHD_SIZE(CcAcchVtables, 0x24);
void Cc_enemyFire(enemyfire_l*);
void Cc_enemyFireRemove(enemyfire_l*);
void Cc_iceUpdateDL(mDoExt_McaMorf*, s32, void*);
BOOL Cc_bombCheckState(void*, s32);
BOOL Cc_bomb2CheckExplode(void*);
BOOL Cc_checkPolySafe(dBgS*, void*);
s32 Cc_getGroundCode(dBgS*, void*);
void Cc_rippleRemove(void*);
BOOL Cc_detectLight(u32, cXyz*);
void* Cc_brkCtor(void*);
void* Cc_btkCtor(void*);
BOOL Cc_brkInit(void*, J3DModelData*, void*, s32, s32, s16, s16, bool, s32, f32);
BOOL Cc_btkInit(void*, J3DModelData*, void*, s32, s32, s16, s16, bool, s32, f32);
void* Cc_jointTree(J3DModelData*);
bool Cc_invisibleCreate(void*, J3DModel*);
void Cc_sphereCtor(dCcD_Sph*);
void Cc_acchCtor(dBgS_ObjAcch*);
void Cc_rippleCtor(void*);
void Cc_acchCirCtor(dBgS_AcchCir*);
void Cc_invisibleCtor(void*);
s32 Cc_getNameIndex(u32, const char*, s32);
void Cc_acchDtor(dBgS_ObjAcch*, s32);
void Cc_invisibleDtor(void*, s32);
void Cc_sphereDtor(dCcD_Sph*, s32);
void Cc_circleDtor(void*, s32);
void Cc_actorDtor(void*, s32);
enum CcAnimationKind { CC_ANM_BRK, CC_ANM_BTK };

// HD: the checked cylinder source descriptor is 0x44 bytes, as in GC.
struct CcCylinderSource {
    /* 0x0 */ be<u32> mFlags;
    /* 0x4 */ be<u32> mAtType;
    /* 0x8 */ be<u32> mAtPowerPacked;
    /* 0xC */ be<u32> mAtSPrm;
    /* 0x10 */ be<u32> mTgType;
    /* 0x14 */ be<u32> mTgSPrm;
    /* 0x18 */ be<u32> mCoSPrm;
    /* 0x1C */ be<u32> mGAtAttributes;
    /* 0x20 */ be<u32> mGAtSPrm;
    /* 0x24 */ be<u32> mGTgAttributes;
    /* 0x28 */ be<u32> mGTgSPrm;
    /* 0x2C */ be<u32> mGCoSPrm;
    /* 0x30 */ be<f32> mCenterX;
    /* 0x34 */ be<f32> mCenterY;
    /* 0x38 */ be<f32> mCenterZ;
    /* 0x3C */ be<f32> mRadius;
    /* 0x40 */ be<f32> mHeight;
};
WWHD_OFFSET(CcCylinderSource, mFlags, 0x0);
WWHD_OFFSET(CcCylinderSource, mAtType, 0x4);
WWHD_OFFSET(CcCylinderSource, mAtPowerPacked, 0x8);
WWHD_OFFSET(CcCylinderSource, mAtSPrm, 0xC);
WWHD_OFFSET(CcCylinderSource, mTgType, 0x10);
WWHD_OFFSET(CcCylinderSource, mTgSPrm, 0x14);
WWHD_OFFSET(CcCylinderSource, mCoSPrm, 0x18);
WWHD_OFFSET(CcCylinderSource, mGAtAttributes, 0x1C);
WWHD_OFFSET(CcCylinderSource, mGAtSPrm, 0x20);
WWHD_OFFSET(CcCylinderSource, mGTgAttributes, 0x24);
WWHD_OFFSET(CcCylinderSource, mGTgSPrm, 0x28);
WWHD_OFFSET(CcCylinderSource, mGCoSPrm, 0x2C);
WWHD_OFFSET(CcCylinderSource, mCenterX, 0x30);
WWHD_OFFSET(CcCylinderSource, mCenterY, 0x34);
WWHD_OFFSET(CcCylinderSource, mCenterZ, 0x38);
WWHD_OFFSET(CcCylinderSource, mRadius, 0x3C);
WWHD_OFFSET(CcCylinderSource, mHeight, 0x40);
WWHD_SIZE(CcCylinderSource, 0x44);
namespace CcResources {
enum CcResource_e {
    dRes_INDEX_CC_BMD_CC_e = 0x2A,
    dRes_INDEX_CC_BRK_CC_e = 0x2D,
    dRes_INDEX_CC_BDL_CC_IWA_e = 0x26,
    dRes_INDEX_CC_BRK_CC_IWA01_e = 0x2F,
    dRes_INDEX_CC_BRK_CC_IWA02_e = 0x30,
    dRes_INDEX_CC_BTK_CC_IWA01_e = 0x33,
    dRes_INDEX_CC_BTK_CC_IWA02_e = 0x34,
    dRes_INDEX_CC_BDL_CC_BETA_e = 0x25,
    dRes_INDEX_CC_BRK_CC_BETA_e = 0x2E,
    dRes_INDEX_CC_BDL_CC_PTCL_e = 0x27,
    dRes_INDEX_CC_BCK_CC_PTCL_e = 0x18,
    dRes_INDEX_CC_BCK_HUSE2TACHI_e = 0x1B,
    dRes_INDEX_CC_BCK_START_e = 0x1E,
};
}

enum { CC_GREEN, CC_RED, CC_BLUE, CC_BLACK, CC_YELLOW };
enum { CC_ACTION_NORMAL, CC_ACTION_SWIM, CC_ACTION_ATTACK, CC_ACTION_DAMAGE, CC_ACTION_DEAD, CC_ACTION_CLIMB, CC_ACTION_STONE, CC_ACTION_WAIT_SWITCH, CC_ACTION_POT };
enum { CC_BEHAVIOR_GROUND, CC_BEHAVIOR_CEILING, CC_BEHAVIOR_DEKU_TREE, CC_BEHAVIOR_FIXED, CC_BEHAVIOR_POT };
enum { CC_MODE_WAIT = 0, CC_MODE_STAND_END = 11, CC_MODE_SWIM_START = 20, CC_MODE_DAMAGE_START = 40, CC_MODE_SPLIT_START = 45, CC_MODE_SPLIT_WAIT = 46, CC_MODE_DEAD_START = 50, CC_MODE_FLATTENED = 53, CC_MODE_CLIMB_START = 60, CC_MODE_STONE_START = 80, CC_MODE_WAIT_SWITCH = 100, CC_MODE_WAIT_SWITCH_DELAY = 101, CC_MODE_POT_SEARCH = 110, CC_MODE_POT_WAIT = 111, CC_MODE_POT_EMERGE = 112 };
enum { CC_PARTICLE_HEAD = 0x3F0, CC_PARTICLE_FOOT = 0x3EF };
