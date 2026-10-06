/**
 * d_a_tn.h (WWHD)
 * Enemy - Darknut: tn_class in the WWHD layout. 
 */
#pragma once
#include "bindings.h"

// Embedded enemyfire constructor has this layout in the Darknut TU.
struct tn_enemyfire {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ u8 _004[0x08C - 0x004];
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ u8 _098[8];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20];
    /* 0x228 */ be<f32> mLightExpansion;
};
WWHD_OFFSET(tn_enemyfire, mStts, 0xA0);
WWHD_OFFSET(tn_enemyfire, mSph, 0xDC);
WWHD_OFFSET(tn_enemyfire, mLightExpansion, 0x228);
WWHD_SIZE(tn_enemyfire, 0x22C);

struct tn_p {
    /* 0x00 */ gptr<J3DModel> mpPartModel;
    /* 0x04 */ gptr<mDoExt_brkAnm> mpPartBrkAnm;
    /* 0x08 */ be<s8> mState;
    /* 0x09 */ be<s8> mTransformMode;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ cXyz mPosition;
    /* 0x18 */ cXyz mPreviousPosition;
    /* 0x24 */ cXyz mVelocity;
    /* 0x30 */ csXyz mAngles;
    /* 0x36 */ csXyz mAngularVelocity;
    /* 0x3C */ u8 _3C[6];
    /* 0x42 */ be<s16> mCounter;
    /* 0x44 */ be<s8> mCountDown;
    /* 0x45 */ u8 _45[7];
};
WWHD_OFFSET(tn_p, mPosition, 0x0C);
WWHD_OFFSET(tn_p, mPreviousPosition, 0x18);
WWHD_OFFSET(tn_p, mVelocity, 0x24);
WWHD_OFFSET(tn_p, mAngles, 0x30);
WWHD_OFFSET(tn_p, mAngularVelocity, 0x36);
WWHD_OFFSET(tn_p, mCounter, 0x42);
WWHD_OFFSET(tn_p, mCountDown, 0x44);
WWHD_SIZE(tn_p, 0x4C);

struct tn_enemyice {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ u8 _004[0x00E - 0x004];
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ u8 _010[0x3B8 - 0x010];
};
WWHD_SIZE(tn_enemyice, 0x3B8);

// Complete stack object used by the two Darknut visibility line checks.
struct tn_linecheck {
    /* 0x00 */ u8 _00[0x10];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ u8 _14[0x0C];
    /* 0x20 */ be<u32> mLineVtable;
    /* 0x24 */ u8 _24[0x0C];
    /* 0x30 */ cXyz mIntersection;
    /* 0x3C */ u8 _3C[0x1C];
    /* 0x58 */ be<u32> mPolyPassVtable;
    /* 0x5C */ be<u8> mPassFlags[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> mGroupPassVtable;
    /* 0x68 */ be<u32> mGroupMask;
};
WWHD_OFFSET(tn_linecheck, mPolyPassVtable, 0x58);
WWHD_OFFSET(tn_linecheck, mIntersection, 0x30);
WWHD_OFFSET(tn_linecheck, mGroupPassVtable, 0x64);
WWHD_SIZE(tn_linecheck, 0x6C);

struct tn_path {
    /* 0x00 */ be<u16> mPointCount;
    /* 0x02 */ u8 _02[6];
    /* 0x08 */ gptr<u8> mPoints;
};
WWHD_SIZE(tn_path, 0x0C);

struct tn_groundcheck {
    /* 0x00 */ u8 _00[0x10];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ u8 _14[0x0C];
    /* 0x20 */ be<u32> mGroundVtable;
    /* 0x24 */ cXyz mPosition;
    /* 0x30 */ u8 _30[0x10];
    /* 0x40 */ be<u32> mPolyPassVtable;
    /* 0x44 */ be<u8> mPassFlags[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> mGroupPassVtable;
    /* 0x50 */ be<u32> mGroupMask;
};
WWHD_OFFSET(tn_groundcheck, mPosition, 0x24);
WWHD_SIZE(tn_groundcheck, 0x54);

// Darknut: native allocation/profile size 0x1C5C. Unnamed intervals stay
// opaque until their exact HD layout has been established from actor bodies.
struct tn_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhaseTn;
    /* 0x03D0 */ request_of_phase_process_class mPhaseTkwn;
    /* 0x03D8 */ be<u8> mBehaviorType;
    /* 0x03D9 */ be<u8> mRangeOrFrozenAnim;
    /* 0x03DA */ be<u8> mPathIndex;
    /* 0x03DB */ be<u8> mEnableSpawnSwitch;
    /* 0x03DC */ be<u8> mDisableSpawnOnDeathSwitch;
    /* 0x03DD */ be<u8> m02C1;
    /* 0x03DE */ u8 _03DE[2];
    /* 0x03E0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x03E4 */ gptr<mDoExt_McaMorf> mpShieldMorf;
    /* 0x03E8 */ gptr<mDoExt_McaMorf> mpArmorMorf;
    /* 0x03EC */ be<s8> mRemainingEquipmentPieces;
    /* 0x03ED */ u8 _03ED[3];
    /* 0x03F0 */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x03F4 */ be<u8> m02DC;
    /* 0x03F5 */ u8 _03F5[3];
    /* 0x03F8 */ tn_p mParts[3];
    /* 0x04DC */ be<u8> mArmorColorIndex;
    /* 0x04DD */ be<u8> mEquipmentType;
    /* 0x04DE */ u8 _04DE[0x04F8 - 0x04DE];
    /* 0x04F8 */ be<s16> mCountDownTimers[5];
    /* 0x0502 */ be<s16> m03EA;
    /* 0x0504 */ be<s16> mFrozenAnimation;
    /* 0x0506 */ u8 _0506[0x0538 - 0x0506];
    /* 0x0538 */ cXyz mSmokePosition;
    /* 0x0544 */ csXyz mSmokeAngles;
    /* 0x054A */ u8 _054A[5];
    /* 0x054F */ be<u8> mSmokeCounter;
    /* 0x0550 */ u8 mSmokeCb1[0x20];
    /* 0x0570 */ u8 mSmokeCb2[0x20];
    /* 0x0590 */ gptr<fopEn_enemy_c> mDamageActor;
    /* 0x0594 */ be<s16> mActionMode;
    /* 0x0596 */ be<s16> mAction;
    /* 0x0598 */ u8 _0598[0x061E - 0x0598];
    /* 0x061E */ csXyz mJointAngles[21];
    /* 0x069C */ cXyz mJointPositions[21];
    /* 0x0798 */ u8 _0798[0x0D84 - 0x0798];
    /* 0x0D84 */ be<s32> mPathDriven;
    /* 0x0D88 */ be<u32> mPathTransition;
    /* 0x0D8C */ cXyz mShieldJointPosition;
    /* 0x0D98 */ be<s32> mAttackType;
    /* 0x0D9C */ u8 _0D9C[0x0DB6 - 0x0D9C];
    /* 0x0DB6 */ be<s8> mWeaponActivation;
    /* 0x0DB7 */ u8 _0DB7[0x0DC4 - 0x0DB7];
    /* 0x0DC4 */ dCcD_Cyl mCoCyl;
    /* 0x0EF4 */ dCcD_Cyl mTgCyl;
    /* 0x1024 */ dCcD_Sph mHeadSph;
    /* 0x1150 */ dCcD_Sph mDefenceSph;
    /* 0x127C */ dCcD_Sph mWeponSph;
    /* 0x13A8 */ dCcD_Sph mWepon2Sph;
    /* 0x14D4 */ cXyz mEffectPosition;
    /* 0x14E0 */ cXyz mJointAnchor20First;
    /* 0x14EC */ cXyz mJointAnchor20Second;
    /* 0x14F8 */ cXyz mJointAnchor17;
    /* 0x1504 */ cXyz mJointAnchor16;
    /* 0x1510 */ cXyz mWeaponPosition;
    /* 0x151C */ cXyz mWeapon2Position;
    /* 0x1528 */ cXyz mPreviousWeaponPosition;
    /* 0x1534 */ u8 _1534[0x154C - 0x1534];
    /* 0x154C */ be<u8> mWeaponCheckFlag;
    /* 0x154D */ be<u8> mWeaponColliderInitialized;
    /* 0x154E */ u8 _154E;
    /* 0x154F */ be<u8> mPathSearchEnabled;
    /* 0x1550 */ u8 _1550[0x10];
    /* 0x1560 */ be<u32> mWeaponActorId;
    /* 0x1564 */ u8 _1564[4];
    /* 0x1568 */ gptr<fopAc_ac_c> mNearbyBomb;
    /* 0x156C */ u8 _156C[8];
    /* 0x1574 */ be<u8> mActivePath;
    /* 0x1575 */ be<s8> mPathPoint;
    /* 0x1576 */ be<u8> mPathPointLookahead;
    /* 0x1577 */ u8 _1577;
    /* 0x1578 */ gptr<tn_path> mpPath;
    /* 0x157C */ u8 _157C[6];
    /* 0x1582 */ be<u8> mHioRegistered;
    /* 0x1583 */ u8 _1583[0x1598 - 0x1583];
    /* 0x1598 */ be<u32> mMantPcId;
    /* 0x159C */ u8 mCutTurnCallbacks[3][0x10];
    /* 0x15CC */ u8 _15CC[0x1668 - 0x15CC];
    /* 0x1668 */ be<s8> mDrawPaused;
    /* 0x1669 */ u8 _1669[0x1674 - 0x1669];
    /* 0x1674 */ tn_enemyice mEnemyIce;
    /* 0x1A2C */ tn_enemyfire mEnemyFire;
    /* 0x1C58 */ gptr<void> mpJntHit;
};
WWHD_OFFSET(tn_class, mPhaseTn, 0x3C8);
WWHD_OFFSET(tn_class, mBehaviorType, 0x3D8);
WWHD_OFFSET(tn_class, mpBodyMorf, 0x3E0);
WWHD_OFFSET(tn_class, mRemainingEquipmentPieces, 0x3EC);
WWHD_OFFSET(tn_class, mpBrkAnm, 0x3F0);
WWHD_OFFSET(tn_class, mParts, 0x3F8);
WWHD_OFFSET(tn_class, mEquipmentType, 0x4DD);
WWHD_OFFSET(tn_class, mArmorColorIndex, 0x4DC);
WWHD_OFFSET(tn_class, mFrozenAnimation, 0x504);
WWHD_OFFSET(tn_class, mCountDownTimers, 0x4F8);
WWHD_OFFSET(tn_class, mPathDriven, 0xD84);
WWHD_OFFSET(tn_class, mAttackType, 0xD98);
WWHD_OFFSET(tn_class, mWeaponActivation, 0xDB6);
WWHD_OFFSET(tn_class, mActionMode, 0x594);
WWHD_OFFSET(tn_class, mAction, 0x596);
WWHD_OFFSET(tn_class, mJointPositions, 0x69C);
WWHD_OFFSET(tn_class, mJointAngles, 0x61E);
WWHD_OFFSET(tn_class, mSmokeCb1, 0x550);
WWHD_OFFSET(tn_class, mSmokePosition, 0x538);
WWHD_OFFSET(tn_class, mSmokeCounter, 0x54F);
WWHD_OFFSET(tn_class, mDamageActor, 0x590);
WWHD_OFFSET(tn_class, mCoCyl, 0xDC4);
WWHD_OFFSET(tn_class, mTgCyl, 0xEF4);
WWHD_OFFSET(tn_class, mHeadSph, 0x1024);
WWHD_OFFSET(tn_class, mDefenceSph, 0x1150);
WWHD_OFFSET(tn_class, mWeponSph, 0x127C);
WWHD_OFFSET(tn_class, mWepon2Sph, 0x13A8);
WWHD_OFFSET(tn_class, mNearbyBomb, 0x1568);
WWHD_OFFSET(tn_class, mEffectPosition, 0x14D4);
WWHD_OFFSET(tn_class, mWeaponPosition, 0x1510);
WWHD_OFFSET(tn_class, mWeaponColliderInitialized, 0x154D);
WWHD_OFFSET(tn_class, mPathSearchEnabled, 0x154F);
WWHD_OFFSET(tn_class, mWeaponActorId, 0x1560);
WWHD_OFFSET(tn_class, mActivePath, 0x1574);
WWHD_OFFSET(tn_class, mpPath, 0x1578);
WWHD_OFFSET(tn_class, mEnemyFire, 0x1A2C);
WWHD_OFFSET(tn_class, mHioRegistered, 0x1582);
WWHD_OFFSET(tn_class, mMantPcId, 0x1598);
WWHD_OFFSET(tn_class, mCutTurnCallbacks, 0x159C);
WWHD_OFFSET(tn_class, mDrawPaused, 0x1668);
WWHD_OFFSET(tn_class, mEnemyIce, 0x1674);
WWHD_OFFSET(tn_class, mpJntHit, 0x1C58);
WWHD_SIZE(tn_class, 0x1C5C);

void Tn_move(tn_class* actor);
BOOL daTn_Execute(tn_class* actor);
