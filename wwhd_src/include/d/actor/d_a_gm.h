/** Mothula and its larva, measured WWHD actor layout.
 * HD: fopEn_enemy_c is 0x3C8. The GameCube source has incomplete, nonmatching
 * behavior stubs; retain its confirmed m2CD and m31E names and name the measured
 * wing, action, collision and camera fields by their use in the verified code.
 */
#pragma once
#include "SSystem/SComponent/c_cc_d.h"
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "m_Do/m_Do_ext_invisible.h"

// HD: private reaction views match the verified c_damagereaction layouts.
struct GmEnemyIce {
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
WWHD_OFFSET(GmEnemyIce, mCylHeight, 0x19C);
WWHD_SIZE(GmEnemyIce, 0x3B8);

/* GmEnemyFire (c_damagereaction), HD 0x22C */
struct GmEnemyFire {
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
    /* 0x208 */ LIGHT_INFLUENCE mLight;
};
WWHD_OFFSET(GmEnemyFire, mSph, 0xDC);
WWHD_SIZE(GmEnemyFire, 0x22C);

struct gm_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhase;
    /* 0x03D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x03D4 */ u8 reserved03D4[0x4];
    /* 0x03D8 */ gptr<mDoExt_brkAnm> mpBrkAnms[4];
    /* 0x03E8 */ be<u8> mWingFlags;
    /* 0x03E9 */ be<u8> m2CD;
    /* 0x03EA */ be<u8> mType;
    /* 0x03EB */ be<u8> mSwitchNo;
    /* 0x03EC */ be<u8> mAction;
    /* 0x03ED */ be<u8> mDamageType;
    /* 0x03EE */ be<u8> mHitLatched;
    /* 0x03EF */ be<u8> mWingEmitterState[2];
    /* 0x03F1 */ be<u8> mAttackEmitterState;
    /* 0x03F2 */ be<u8> mSaveSwitch;
    /* 0x03F3 */ be<u8> mStealCount;
    /* 0x03F4 */ u8 reserved03F4[0x2];
    /* 0x03F6 */ be<s16> mAttackFrame;
    /* 0x03F8 */ be<s16> mActionState;
    /* 0x03FA */ be<s16> mWingEmitterTimer[2];
    /* 0x03FE */ be<s16> mTimers[7];
    /* 0x040C */ csXyz mBodyPhase;
    /* 0x0412 */ be<s16> mAttackCounter;
    /* 0x0414 */ csXyz mWingPhases[4];
    /* 0x042C */ csXyz mWingPhaseStep;
    /* 0x0432 */ csXyz mBodyPhaseStep;
    /* 0x0438 */ be<s16> mBobPhase;
    /* 0x043A */ be<s16> m31E;
    /* 0x043C */ be<s16> mPitchOffset;
    /* 0x043E */ be<s16> mDiveCount;
    /* 0x0440 */ be<s16> mBrkIndex;
    /* 0x0442 */ be<s16> mGroundTurn;
    /* 0x0444 */ be<f32> mGroundOffset;
    /* 0x0448 */ be<f32> mWallHeight;
    /* 0x044C */ be<f32> mWallRadius;
    /* 0x0450 */ be<f32> mTurnSpeed;
    /* 0x0454 */ be<f32> mBodyCenterY;
    /* 0x0458 */ be<f32> mBodyRadius;
    /* 0x045C */ be<f32> mBodyHeight;
    /* 0x0460 */ be<s32> mAnimation;
    /* 0x0464 */ cXyz mTargetPos;
    /* 0x0470 */ cXyz mHomePos;
    /* 0x047C */ cXyz mProbeTargets[3];
    /* 0x04A0 */ cXyz mProbeHits[3];
    /* 0x04C4 */ cXyz mWeaponPos;
    /* 0x04D0 */ cXyz mPositionOffset;
    /* 0x04DC */ cXyz mBobOffset;
    /* 0x04E8 */ union {
        cXyz mLineStart;
        struct {
            be<f32> mLineStartX, z, mLineStartZ;
        };
    };
    /* 0x04F4 */ union {
        cXyz mLineEnd;
        struct {
            be<f32> mLineEndX, m3DC, m3E0;
        };
    };
    /* 0x0500 */ cXyz mJointPositions[3];
    /* 0x0524 */ cXyz mGroundCenterOffset;
    /* 0x0530 */ csXyz mWingAngles[4];
    /* 0x0548 */ csXyz mBodyAngle;
    /* 0x054E */ u8 reserved054E[0x2];
    /* 0x0550 */ be<s16> mTargetYaw;
    /* 0x0552 */ be<s16> mTargetRoll;
    /* 0x0554 */ csXyz mTwistAngle;
    /* 0x055A */ union {
        csXyz mGroundAngle;
        struct {
            be<s16> m43E, mGroundYaw, m442;
        };
    };
    /* 0x0560 */ be<s16> mDemoMode;
    /* 0x0562 */ u8 reserved0562[0x2];
    /* 0x0564 */ cXyz mCameraEye;
    /* 0x0570 */ cXyz mCameraCenter;
    /* 0x057C */ be<f32> mCameraStep;
    /* 0x0580 */ be<f32> mCameraFov;
    /* 0x0584 */ dBgS_AcchCir mAcchCir;
    /* 0x05C4 */ dBgS_ObjAcch mAcch;
    /* 0x0788 */ dCcD_Stts mStts;
    /* 0x07C4 */ dCcD_Cyl mBodyCyl;
    /* 0x08F4 */ dCcD_Cyl mDiveCyl;
    /* 0x0A24 */ dCcD_Cyl mWindCyl;
    /* 0x0B54 */ dCcD_Sph mWeaponSph;
    /* 0x0C80 */ gptr<JPABaseEmitter> mpEmitters[4];
    /* 0x0C90 */ u8 reserved0C90[0x14];
    /* 0x0CA4 */ GmEnemyIce mEnemyIce;
    /* 0x105C */ GmEnemyFire mEnemyFire;
    /* 0x1288 */ mDoExt_invisibleModel mInvisibleModel;
};
WWHD_SIZE(gm_class, 0x1290);
WWHD_OFFSET(gm_class, mPhase, 0x3C8);
WWHD_OFFSET(gm_class, mpMorf, 0x3D0);
WWHD_OFFSET(gm_class, mpBrkAnms, 0x3D8);
WWHD_OFFSET(gm_class, mWingFlags, 0x3E8);
WWHD_OFFSET(gm_class, m2CD, 0x3E9);
WWHD_OFFSET(gm_class, mType, 0x3EA);
WWHD_OFFSET(gm_class, mSwitchNo, 0x3EB);
WWHD_OFFSET(gm_class, mAction, 0x3EC);
WWHD_OFFSET(gm_class, mDamageType, 0x3ED);
WWHD_OFFSET(gm_class, mHitLatched, 0x3EE);
WWHD_OFFSET(gm_class, mWingEmitterState, 0x3EF);
WWHD_OFFSET(gm_class, mAttackEmitterState, 0x3F1);
WWHD_OFFSET(gm_class, mSaveSwitch, 0x3F2);
WWHD_OFFSET(gm_class, mStealCount, 0x3F3);
WWHD_OFFSET(gm_class, mAttackFrame, 0x3F6);
WWHD_OFFSET(gm_class, mActionState, 0x3F8);
WWHD_OFFSET(gm_class, mWingEmitterTimer, 0x3FA);
WWHD_OFFSET(gm_class, mTimers, 0x3FE);
WWHD_OFFSET(gm_class, mBodyPhase, 0x40C);
WWHD_OFFSET(gm_class, mAttackCounter, 0x412);
WWHD_OFFSET(gm_class, mWingPhases, 0x414);
WWHD_OFFSET(gm_class, mWingPhaseStep, 0x42C);
WWHD_OFFSET(gm_class, mBodyPhaseStep, 0x432);
WWHD_OFFSET(gm_class, mBobPhase, 0x438);
WWHD_OFFSET(gm_class, m31E, 0x43A);
WWHD_OFFSET(gm_class, mPitchOffset, 0x43C);
WWHD_OFFSET(gm_class, mDiveCount, 0x43E);
WWHD_OFFSET(gm_class, mBrkIndex, 0x440);
WWHD_OFFSET(gm_class, mGroundTurn, 0x442);
WWHD_OFFSET(gm_class, mGroundOffset, 0x444);
WWHD_OFFSET(gm_class, mWallHeight, 0x448);
WWHD_OFFSET(gm_class, mWallRadius, 0x44C);
WWHD_OFFSET(gm_class, mTurnSpeed, 0x450);
WWHD_OFFSET(gm_class, mBodyCenterY, 0x454);
WWHD_OFFSET(gm_class, mBodyRadius, 0x458);
WWHD_OFFSET(gm_class, mBodyHeight, 0x45C);
WWHD_OFFSET(gm_class, mAnimation, 0x460);
WWHD_OFFSET(gm_class, mTargetPos, 0x464);
WWHD_OFFSET(gm_class, mHomePos, 0x470);
WWHD_OFFSET(gm_class, mProbeTargets, 0x47C);
WWHD_OFFSET(gm_class, mProbeHits, 0x4A0);
WWHD_OFFSET(gm_class, mWeaponPos, 0x4C4);
WWHD_OFFSET(gm_class, mPositionOffset, 0x4D0);
WWHD_OFFSET(gm_class, mBobOffset, 0x4DC);
WWHD_OFFSET(gm_class, mLineStart, 0x4E8);
WWHD_OFFSET(gm_class, mLineEnd, 0x4F4);
WWHD_OFFSET(gm_class, mJointPositions, 0x500);
WWHD_OFFSET(gm_class, mGroundCenterOffset, 0x524);
WWHD_OFFSET(gm_class, mWingAngles, 0x530);
WWHD_OFFSET(gm_class, mBodyAngle, 0x548);
WWHD_OFFSET(gm_class, mTargetYaw, 0x550);
WWHD_OFFSET(gm_class, mTargetRoll, 0x552);
WWHD_OFFSET(gm_class, mTwistAngle, 0x554);
WWHD_OFFSET(gm_class, mGroundAngle, 0x55A);
WWHD_OFFSET(gm_class, mDemoMode, 0x560);
WWHD_OFFSET(gm_class, mCameraEye, 0x564);
WWHD_OFFSET(gm_class, mCameraCenter, 0x570);
WWHD_OFFSET(gm_class, mCameraStep, 0x57C);
WWHD_OFFSET(gm_class, mCameraFov, 0x580);
WWHD_OFFSET(gm_class, mAcchCir, 0x584);
WWHD_OFFSET(gm_class, mAcch, 0x5C4);
WWHD_OFFSET(gm_class, mStts, 0x788);
WWHD_OFFSET(gm_class, mBodyCyl, 0x7C4);
WWHD_OFFSET(gm_class, mDiveCyl, 0x8F4);
WWHD_OFFSET(gm_class, mWindCyl, 0xA24);
WWHD_OFFSET(gm_class, mWeaponSph, 0xB54);
WWHD_OFFSET(gm_class, mpEmitters, 0xC80);

WWHD_OFFSET(gm_class, mEnemyIce, 0xCA4);
WWHD_OFFSET(gm_class, mEnemyFire, 0x105C);
WWHD_OFFSET(gm_class, mInvisibleModel, 0x1288);

// HD: renderer and emitter views cover members absent from the shared GC interfaces.
struct GmModelHD {
    u8 padding00[0x14];
    gptr<void> mpResourceHeader;
    u8 padding18[0x14];
    gptr<J3DMtxBuffer_hd> mpMtxBuffer;
    u8 padding30[0x44];
    be<u32> mMaterialFlags;
    u8 padding78[0x34];
    gptr<J3DModelData> mModelData;
    u8 paddingB0[8];
    be<u32> mUserArea;
    cXyz mBaseScale;
    Mtx34 mBaseTransformMtx;
};
WWHD_OFFSET(GmModelHD, mpResourceHeader, 0x14);
WWHD_OFFSET(GmModelHD, mpMtxBuffer, 0x2C);
WWHD_OFFSET(GmModelHD, mMaterialFlags, 0x74);
WWHD_OFFSET(GmModelHD, mModelData, 0xAC);
WWHD_OFFSET(GmModelHD, mUserArea, 0xB8);
WWHD_OFFSET(GmModelHD, mBaseScale, 0xBC);
WWHD_OFFSET(GmModelHD, mBaseTransformMtx, 0xC8);
WWHD_SIZE(GmModelHD, 0xF8);
struct GmJointHD {
    be<u32> mpData;
    be<u16> mJntNo;
    u8 padding06[2];
    be<u32> mCallBack;
    u8 padding0C[4];
    be<u32> mpMeshes;
    u8 padding14[8];
};
WWHD_OFFSET(GmJointHD, mJntNo, 4);
WWHD_OFFSET(GmJointHD, mCallBack, 8);
WWHD_OFFSET(GmJointHD, mpMeshes, 0x10);
WWHD_SIZE(GmJointHD, 0x1C);
struct GmEmitterHD {
    u8 padding00[0x14];
    cXyz mGlobalTranslation;
    csXyz mGlobalRotation;
    u8 padding26[0xE];
    be<f32> mRate;
    u8 padding38[0x24];
    be<s32> mMaxFrame;
    u8 padding60[0x14];
    be<f32> mVolumeSize;
    u8 padding78[0x178];
    Mtx34 mGlobalRotationMtx;
    u8 padding220[0xC];
    cXyz mGlobalPosition;
    u8 padding238[0xC];
    GXColor mGlobalPrmColor;
    u8 padding248[0xC];
    be<u32> mStatus;
};
WWHD_OFFSET(GmEmitterHD, mGlobalTranslation, 0x14);
WWHD_OFFSET(GmEmitterHD, mGlobalRotation, 0x20);
WWHD_OFFSET(GmEmitterHD, mRate, 0x34);
WWHD_OFFSET(GmEmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(GmEmitterHD, mVolumeSize, 0x74);
WWHD_OFFSET(GmEmitterHD, mGlobalRotationMtx, 0x1F0);
WWHD_OFFSET(GmEmitterHD, mGlobalPosition, 0x22C);
WWHD_OFFSET(GmEmitterHD, mGlobalPrmColor, 0x244);
WWHD_OFFSET(GmEmitterHD, mStatus, 0x254);
WWHD_SIZE(GmEmitterHD, 0x258);
struct GmJointTreeHD {
    u8 padding[8];
    be<u16> mJointCount;
};
WWHD_OFFSET(GmJointTreeHD, mJointCount, 8);
WWHD_SIZE(GmJointTreeHD, 0xA);
struct GmMeshHD {
    be<u32> mpDescriptor, mpNext, mpShape;
};
struct GmMeshDescriptorHD {
    u8 padding[0xC];
    be<u16> mNameIndex;
};
WWHD_SIZE(GmMeshHD, 0xC);
WWHD_OFFSET(GmMeshDescriptorHD, mNameIndex, 0xC);
WWHD_SIZE(GmMeshDescriptorHD, 0xE);
struct GmSafeStringMethodsHD {
    u8 padding[0x14];
    be<u32> assureTermination;
};
struct GmPlayerMethodsHD {
    u8 padding[0x114];
    be<u32> setPlayerPosAndAngle;
};
WWHD_OFFSET(GmSafeStringMethodsHD, assureTermination, 0x14);
WWHD_SIZE(GmSafeStringMethodsHD, 0x18);
WWHD_OFFSET(GmPlayerMethodsHD, setPlayerPosAndAngle, 0x114);
WWHD_SIZE(GmPlayerMethodsHD, 0x118);
// HD: adding fields to address zero still accesses the guest image, as the original does.
template <class T> inline T* GmGuest(u32 address) {
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}

struct GmCcSHD {
    u8 padding[0x2854];
    u8 mMass_Mng;
};
WWHD_OFFSET(GmCcSHD, mMass_Mng, 0x2854);
WWHD_SIZE(GmCcSHD, 0x2855);

struct GmPlayerCutHD {
    u8 padding[0x3AC];
    be<u8> mCutType;
};
WWHD_OFFSET(GmPlayerCutHD, mCutType, 0x3AC);
WWHD_SIZE(GmPlayerCutHD, 0x3AD);

struct GmHIOHD {
    u8 padding000[0x498];
    be<f32> mBtHeight;
    be<f32> mBtRadius;
    be<f32> mBtDistance;
    be<f32> mAttackStartFrame;
    be<f32> mAttackEndFrame;
    u8 padding4AC[0x4];
    be<f32> mDiveSpeed;
    be<f32> mDiveGravity;
    be<f32> mDiveSpeedY;
    u8 padding4BC[0x4E];
    be<s16> mGroundAttackCount;
    be<s16> mGroundTurnAngle;
    be<s16> mFreezeSoundFrame;
    be<s16> mPauseDeathDemo;
    be<s16> mTypeOverride;
    u8 padding514[0x9C];
    be<f32> mDemoCenterHeight;
    u8 padding5B4[0x8];
    be<f32> mDemoEyeHeight;
    u8 padding5C0[0x8];
    be<f32> mDeathCenterHeight;
    be<f32> mDeathEyeHeight;
    be<f32> mDeathEyeDistance;
    u8 padding5D4[0x4C];
    be<s16> mDemoDelayAngle;
    be<s16> mDemoFlightDelay;
    u8 padding624[0xB8];
    be<f32> mPlayerStartX;
    be<f32> mPlayerStartZ;
    be<f32> mCameraOffsetX;
    be<f32> mCameraOffsetY;
    be<f32> mCameraOffsetZ;
    be<f32> mCameraEyeX;
    be<f32> mCameraEyeY;
    be<f32> mCameraEyeZ;
    u8 padding6FC[0x8];
    be<f32> mDemoStartHeight;
    be<f32> mDemoPitchStep;
    be<f32> mDemoCameraStep;
};
WWHD_SIZE(GmHIOHD, 0x710);
WWHD_OFFSET(GmHIOHD, mBtHeight, 0x498);
WWHD_OFFSET(GmHIOHD, mBtRadius, 0x49C);
WWHD_OFFSET(GmHIOHD, mBtDistance, 0x4A0);
WWHD_OFFSET(GmHIOHD, mAttackStartFrame, 0x4A4);
WWHD_OFFSET(GmHIOHD, mAttackEndFrame, 0x4A8);
WWHD_OFFSET(GmHIOHD, mDiveSpeed, 0x4B0);
WWHD_OFFSET(GmHIOHD, mDiveGravity, 0x4B4);
WWHD_OFFSET(GmHIOHD, mDiveSpeedY, 0x4B8);
WWHD_OFFSET(GmHIOHD, mGroundAttackCount, 0x50A);
WWHD_OFFSET(GmHIOHD, mGroundTurnAngle, 0x50C);
WWHD_OFFSET(GmHIOHD, mFreezeSoundFrame, 0x50E);
WWHD_OFFSET(GmHIOHD, mPauseDeathDemo, 0x510);
WWHD_OFFSET(GmHIOHD, mTypeOverride, 0x512);
WWHD_OFFSET(GmHIOHD, mDemoCenterHeight, 0x5B0);
WWHD_OFFSET(GmHIOHD, mDemoEyeHeight, 0x5BC);
WWHD_OFFSET(GmHIOHD, mDeathCenterHeight, 0x5C8);
WWHD_OFFSET(GmHIOHD, mDeathEyeHeight, 0x5CC);
WWHD_OFFSET(GmHIOHD, mDeathEyeDistance, 0x5D0);
WWHD_OFFSET(GmHIOHD, mDemoDelayAngle, 0x620);
WWHD_OFFSET(GmHIOHD, mDemoFlightDelay, 0x622);
WWHD_OFFSET(GmHIOHD, mPlayerStartX, 0x6DC);
WWHD_OFFSET(GmHIOHD, mPlayerStartZ, 0x6E0);
WWHD_OFFSET(GmHIOHD, mCameraOffsetX, 0x6E4);
WWHD_OFFSET(GmHIOHD, mCameraOffsetY, 0x6E8);
WWHD_OFFSET(GmHIOHD, mCameraOffsetZ, 0x6EC);
WWHD_OFFSET(GmHIOHD, mCameraEyeX, 0x6F0);
WWHD_OFFSET(GmHIOHD, mCameraEyeY, 0x6F4);
WWHD_OFFSET(GmHIOHD, mCameraEyeZ, 0x6F8);
WWHD_OFFSET(GmHIOHD, mDemoStartHeight, 0x704);
WWHD_OFFSET(GmHIOHD, mDemoPitchStep, 0x708);
WWHD_OFFSET(GmHIOHD, mDemoCameraStep, 0x70C);
struct GmLarvaOffsets {
    cXyz mPositions[4];
};
struct GmWingPose {
    csXyz mAngles[4];
};
struct GmProbeCoordinates {
    be<f32> mX[3], mY[3], mZ[3];
};
struct GmFlameScales {
    be<f32> mValues[10];
};
struct GmFlameJoints {
    be<u8> mValues[10];
};
struct GmWingNames {
    be<u32> mIndices[4];
};
struct GmCounters {
    be<u32> mValues[4];
};
struct GmRegistrations {
    u8 mEntries[3][12];
};
struct GmColliderSource {
    u8 mData[0x44];
};
struct GmSaveHeaderHD {
    u8 padding[0x20];
    dSv_info_c mSave;
};
WWHD_SIZE(GmLarvaOffsets, 0x30);
WWHD_SIZE(GmWingPose, 0x18);
WWHD_SIZE(GmProbeCoordinates, 0x24);
WWHD_SIZE(GmFlameScales, 0x28);
WWHD_SIZE(GmFlameJoints, 0xA);
WWHD_SIZE(GmWingNames, 0x10);
WWHD_SIZE(GmCounters, 0x10);
WWHD_SIZE(GmRegistrations, 0x24);
WWHD_SIZE(GmColliderSource, 0x44);
WWHD_OFFSET(GmSaveHeaderHD, mSave, 0x20);

using GmModelPointer = gptr<J3DModel>;
using GmMatrixPointer = gptr<Mtx34>;
using GmSavePointer = gptr<GmSaveHeaderHD>;
using GmResourcePointer = gptr<void>;

struct GmSinCosHD {
    be<f32> mSin, mCos;
};
struct GmSinCosTable {
    GmSinCosHD mSamples[8192];
};
WWHD_SIZE(GmSinCosHD, 8);
WWHD_SIZE(GmSinCosTable, 0x10000);
struct GmResourceHeaderHD {
    u8 padding00[4];
    be<u32> mModelNameOffset;
    u8 padding08[0x10];
    be<u32> mMaterialNameOffset;
};
WWHD_OFFSET(GmResourceHeaderHD, mModelNameOffset, 4);
WWHD_OFFSET(GmResourceHeaderHD, mMaterialNameOffset, 0x18);
WWHD_SIZE(GmResourceHeaderHD, 0x1C);
struct GmModelDataDrawHD {
    u8 padding[0x48];
    be<u32> mDrawState;
};
WWHD_OFFSET(GmModelDataDrawHD, mDrawState, 0x48);
WWHD_SIZE(GmModelDataDrawHD, 0x4C);
struct GmCameraHD {
    u8 padding[0x248];
    u8 mCameraBody;
};
WWHD_OFFSET(GmCameraHD, mCameraBody, 0x248);
WWHD_SIZE(GmCameraHD, 0x249);
struct GmNameTableHD {
    u8 padding[0x3E0C];
    u8 mNameTable;
};
WWHD_OFFSET(GmNameTableHD, mNameTable, 0x3E0C);
WWHD_SIZE(GmNameTableHD, 0x3E0D);

WWHD_OFFSET(gm_class, z, 0x4EC);
WWHD_OFFSET(gm_class, m3DC, 0x4F8);
WWHD_OFFSET(gm_class, m3E0, 0x4FC);
WWHD_OFFSET(gm_class, m43E, 0x55A);
WWHD_OFFSET(gm_class, m442, 0x55E);

// GameCube Gm archive indices and joint names are unchanged in the verified HD paths.
enum dRes_INDEX_GM {
    /* BAS  */
    dRes_INDEX_GM_BAS_ATACK02_e = 0x6,
    dRes_INDEX_GM_BAS_G_ATACK03_e = 0x7,
    dRes_INDEX_GM_BAS_G_HANE_e = 0x8,
    /* BCK  */
    dRes_INDEX_GM_BCK_ATACK01_e = 0xB,
    dRes_INDEX_GM_BCK_ATACK02_e = 0xC,
    dRes_INDEX_GM_BCK_FLY_e = 0xD,
    dRes_INDEX_GM_BCK_G_ATACK01_e = 0xE,
    dRes_INDEX_GM_BCK_G_ATACK02_e = 0xF,
    dRes_INDEX_GM_BCK_G_ATACK03_e = 0x10,
    dRes_INDEX_GM_BCK_G_CHAKUCHI_e = 0x11,
    dRes_INDEX_GM_BCK_G_DAMAGE_e = 0x12,
    dRes_INDEX_GM_BCK_G_DEAD01_e = 0x13,
    dRes_INDEX_GM_BCK_G_DEAD02_e = 0x14,
    dRes_INDEX_GM_BCK_G_HAMMER_e = 0x15,
    dRes_INDEX_GM_BCK_G_HANE_e = 0x16,
    dRes_INDEX_GM_BCK_G_MACHINGUN_e = 0x17,
    dRes_INDEX_GM_BCK_G_UCHIWA_e = 0x18,
    dRes_INDEX_GM_BCK_G_WAIT_e = 0x19,
    dRes_INDEX_GM_BCK_G_WALK_e = 0x1A,
    dRes_INDEX_GM_BCK_JETATACK_e = 0x1B,
    dRes_INDEX_GM_BCK_PAUSE_e = 0x1C,
    dRes_INDEX_GM_BCK_TOMARU_e = 0x1D,
    dRes_INDEX_GM_BCK_UMINIGE_e = 0x1E,
    /* BDLM */
    dRes_INDEX_GM_BDL_GM_e = 0x21,
    dRes_INDEX_GM_BDL_HANESL_e = 0x22,
    dRes_INDEX_GM_BDL_HANEUL_e = 0x23,
    /* BRK  */
    dRes_INDEX_GM_BRK_ATACK_e = 0x26,
    dRes_INDEX_GM_BRK_DAMAGE_e = 0x27,
    dRes_INDEX_GM_BRK_DEAD_e = 0x28,
    dRes_INDEX_GM_BRK_GM_e = 0x29,
};
enum GM_JNT {
    GM_JNT_GM_CENTER_e = 0x0,
    GM_JNT_HARAA_e = 0x1,
    GM_JNT_HARAB_e = 0x2,
    GM_JNT_HARAC_e = 0x3,
    GM_JNT_MUNE_e = 0x4,
    GM_JNT_ATAMA_e = 0x5,
    GM_JNT_KIBAL_e = 0x6,
    GM_JNT_KIBAR_e = 0x7,
    GM_JNT_TUNOLA_e = 0x8,
    GM_JNT_TUNOLB_e = 0x9,
    GM_JNT_TUNORA_e = 0xA,
    GM_JNT_TUNORB_e = 0xB,
    GM_JNT_HANESLA_e = 0xC,
    GM_JNT_HANESLB_e = 0xD,
    GM_JNT_HANESRA_e = 0xE,
    GM_JNT_HANESRB_e = 0xF,
    GM_JNT_HANEULA_e = 0x10,
    GM_JNT_HANEULB_e = 0x11,
    GM_JNT_HANEURA_e = 0x12,
    GM_JNT_HANEURB_e = 0x13,
    GM_JNT_UDELAA_e = 0x14,
    GM_JNT_UDELAB_e = 0x15,
    GM_JNT_TELA_e = 0x16,
    GM_JNT_UDELBA_e = 0x17,
    GM_JNT_UDELBB_e = 0x18,
    GM_JNT_TELB_e = 0x19,
    GM_JNT_UDERAA_e = 0x1A,
    GM_JNT_UDERAB_e = 0x1B,
    GM_JNT_TERA_e = 0x1C,
    GM_JNT_UDERBA_e = 0x1D,
    GM_JNT_UDERBB_e = 0x1E,
    GM_JNT_TERB_e = 0x1F,
};

enum GmAction {
    GM_ACTION_FLY = 0,
    GM_ACTION_WALL = 1,
    GM_ACTION_UCHIWA = 2,
    GM_ACTION_DIVE = 3,
    GM_ACTION_GROUND = 4,
    GM_ACTION_FLY_DAMAGE = 5,
    GM_ACTION_GROUND_ATTACK = 10,
    GM_ACTION_DEMO = 20
};

BOOL daGM_IsDelete(gm_class* actor);
void BG_check(gm_class* actor);
void wing_ret_set(gm_class* actor);
f32 gm_cosLookup(void* table, u16 angle);
void gm_staticDestructor(void* object, s32 flags);
void gm_emptyVirtual(gm_class* actor);
void anm_init(gm_class* actor, s32 animation, f32 morph, u8 mode, f32 speed, s32 auxiliary);
void* gm_fireConstructor(void* object);
gm_class* gm_constructor(gm_class* actor);
void gm_destructor(gm_class* actor, s32 flags);
void gm_staticInit();
BOOL nodeCallBack(void* node, s32 phase);
void draw_SUB(gm_class* actor);
BOOL ks_set_rtn(gm_class* actor);
void action_kabehari(gm_class* actor);
void action_fly_damage(gm_class* actor);
BOOL daGM_Delete(gm_class* actor);
BOOL daGM_Draw(gm_class* actor);
BOOL useHeapInit(gm_class* actor);
BOOL Line_check(gm_class* actor, cXyz* end);
s32 daGM_Create(gm_class* actor);
BOOL body_atari_check(gm_class* actor);
void action_ground_attack(gm_class* actor);
void action_demo(gm_class* actor);
BOOL daGM_Execute(gm_class* actor);

// HD: fixed ABI bridges preserve the argument widths of the verified GHS calls.
struct GmAttackInfo {
    gptr<void> mpObj;
    u8 _4[0x10];
    be<u32> pParticlePos;
    u8 _18[4];
};
WWHD_SIZE(GmAttackInfo, 0x1C);
int GmHD_cBgS_LineCross(void*, dBgS_LinChk*);
void GmHD_cBgS_LinChk_dtor(dBgS_LinChk*, int);
void GmHD_cCcD_Stts_ctor_Status(dCcD_Stts*);
void GmHD_cCcD_Stts_ctor_Storage(void*);
void GmHD_cCcS_Set_Cylinder(void*, dCcD_Cyl*);
void GmHD_cCcS_Set_Sphere(void*, dCcD_Sph*);
int GmHD_cDT_NamePTbl_GetIndex(void*, const char*, int);
void GmHD_cLib_addCalc2_VectorX(cXyz*, float, float, float);
void GmHD_cLib_addCalc2_Float(gabi::be<float>*, float, float, float);
void GmHD_cLib_addCalc0(gabi::be<float>*, float, float);
void GmHD_cLib_addCalcAngleS2_VectorX(csXyz*, int, int, int);
void GmHD_cLib_addCalcAngleS2_IntTarget(gabi::be<short>*, int, int, int);
void GmHD_cLib_addCalcAngleS2_ShortStep(gabi::be<short>*, int, int, short);
void GmHD_cLib_addCalcAngleS2_ShortTarget(gabi::be<short>*, short, int, int);
void GmHD_cLib_addCalcAngleS2_ShortTargetStep(gabi::be<short>*, short, int, short);
int GmHD_cLib_distanceAngleS_IntTarget(short, int);
int GmHD_cLib_distanceAngleS_ShortTarget(short, short);
void GmHD_MtxPosition(cXyz*, cXyz*);
void GmHD_cM3dGCir_dtor(void*, int);
void GmHD_cM3dGCyl_SetC(void*, cXyz*);
void GmHD_cM3dGCyl_SetH(void*, float);
void GmHD_cM3dGCyl_SetR(void*, float);
void GmHD_cM3dGCyl_ctor(void*);
void GmHD_cM3dGSph_SetR(void*, float);
void GmHD_cM3dGSph_SetC(void*, cXyz*);
int GmHD_cM_atan2s_WideResult(float, float);
short GmHD_cM_atan2s_AngleResult(float, float);
float GmHD_cM_rnd();
float GmHD_cM_rndF(float);
float GmHD_cM_rndFX(float);
void GmHD_csXyz_csXyz(csXyz*, int, int, int);
void GmHD_cXyz_add(cXyz*, cXyz*, cXyz*);
void GmHD_cXyz_subtract(cXyz*, cXyz*, cXyz*);
int GmHD_enemy_ice(void*);
void GmHD_enemy_fire(void*);
void GmHD_enemy_fire_remove(void*);
void GmHD_enemy_piyo_set(gm_class*);
void GmHD_draw_SUB(gm_class*);
void GmHD_anm_init(gm_class*, int, float, int, float, int);
void GmHD_body_atari_check(gm_class*);
int GmHD_Line_check(gm_class*, cXyz*);
void GmHD_ks_set_rtn(gm_class*);
void GmHD_BG_check(gm_class*);
void GmHD_wing_ret_set(gm_class*);
float GmHD_cosLookup(void*, short);
void GmHD_action_kabehari(gm_class*);
void GmHD_action_fly_damage(gm_class*);
void GmHD_action_ground_attack(gm_class*);
void GmHD_action_demo(gm_class*);
int GmHD_dBgS_GetMtrlSndId(void*, void*);
void GmHD_dBgS_Acch_dtor(dBgS_ObjAcch*, int);
void GmHD_dBgS_AcchCir_dBgS_AcchCir(dBgS_AcchCir*);
void GmHD_dBgS_AcchCir_SetWall(dBgS_AcchCir*, float, float);
void GmHD_dBgS_Acch_dBgS_Acch(dBgS_ObjAcch*);
void GmHD_dBgS_Acch_Set(dBgS_ObjAcch*, cXyz*, cXyz*, gm_class*, int, dBgS_AcchCir*, cXyz*, int,
                        int);
void GmHD_dBgS_Acch_CrrPos(dBgS_ObjAcch*, void*);
void GmHD_dBgS_LinChk_Set(dBgS_LinChk*, cXyz*, cXyz*, gm_class*);
int GmHD_dCam_getAngleY(void*);
void GmHD_dCamera_c_Stop(void*);
void GmHD_dCamera_c_Start(void*);
void GmHD_dCamera_c_Set(void*, cXyz*, cXyz*, int, float);
void GmHD_dCamera_c_Reset(void*, cXyz*, cXyz*);
void GmHD_dCamera_c_SetTrimSize(void*, int);
void GmHD_dCcD_Stts_dtor_Status(dCcD_Stts*, int);
void GmHD_dCcD_Stts_dtor_Storage(void*, int);
void GmHD_dCcD_Cyl_dtor(dCcD_Cyl*, int);
void GmHD_dCcD_Sph_dtor_Sphere(dCcD_Sph*, int);
void GmHD_dCcD_Sph_dtor_Storage(void*, int);
void GmHD_dCcD_GStts_dCcD_GStts(void*);
void GmHD_dCcD_GStts_Move(void*);
void GmHD_dCcD_Stts_Init(void*, int, int, gm_class*);
void GmHD_dCcD_GObjInf_dCcD_GObjInf(void*);
int GmHD_dCcD_GObjInf_ChkTgHit(dCcD_Cyl*);
void* GmHD_dCcD_GObjInf_GetTgHitObj(dCcD_Cyl*);
void GmHD_dCcD_Cyl_Set(dCcD_Cyl*, void*);
void GmHD_dCcD_Sph_ctor_Sphere(dCcD_Sph*);
void GmHD_dCcD_Sph_ctor_Storage(void*);
void GmHD_dCcD_Sph_Set(dCcD_Sph*, void*);
void GmHD_dCcMassS_Mng_Set(void*, dCcD_Cyl*, int);
void GmHD_cc_at_check(gm_class*, GmAttackInfo*);
void* GmHD_dComIfGp_get();
int GmHD_dComIfG_resLoad(request_of_phase_process_class*, const char*);
void GmHD_dComIfG_resDelete(request_of_phase_process_class*, const char*);
int GmHD_dComIfGp_getReverb(signed char);
void* GmHD_dKy_getEnvlight();
void GmHD_dScnKy_env_light_c_settingTevStruct(void*, int, cXyz*, dKy_tevstr_c*);
void GmHD_dScnKy_env_light_c_setLightTevColorType_MAJI(void*, void*, dKy_tevstr_c*);
void GmHD_dMat_ice_c_entryDL(mDoExt_McaMorf*, int, void*);
void GmHD_dPa_control_c_set_Effect(void*, int, unsigned int, cXyz*, csXyz*, cXyz*, int, int, int,
                                   int, int, int);
void* GmHD_dPa_control_c_set_Emitter(void*, int, unsigned int, cXyz*, csXyz*, cXyz*, int, int, int,
                                     int, int, int);
void GmHD_dSv_info_c_onSwitch(void*, unsigned char, signed char);
int GmHD_dSv_info_c_isSwitch(void*, unsigned char, signed char);
void GmHD_dSv_info_c_onActor(void*, unsigned short, signed char);
void GmHD_dSnap_RegistFig(int, gm_class*, cXyz*, short, float, float, float);
void GmHD_dVibration_c_StopQuake(void*, int);
void GmHD_fopAc_ac_c_fopAc_ac_c(gm_class*);
void GmHD_fopAc_ac_c_dtor_fopAc_ac_c(gm_class*, int);
void GmHD_fopAcM_delete(gm_class*);
void GmHD_fopAcM_create(int, unsigned int, cXyz*, signed char, csXyz*, cXyz*, int, int);
int GmHD_fopAcM_createChild(int, unsigned int, int, cXyz*, signed char, csXyz*, cXyz*, int, int);
int GmHD_fopAcM_entrySolidHeap(gm_class*, int, int);
void GmHD_fopAcM_setCullSizeBox(gm_class*, float, float, float, float, float, float);
void GmHD_fopAcM_posMove(gm_class*, void*);
int GmHD_fopAcM_searchActorAngleY_ActorWide(gm_class*, fopAc_ac_c*);
int GmHD_fopAcM_searchActorAngleY_StorageWide(gm_class*, void*);
short GmHD_fopAcM_searchActorAngleY_Angle(gm_class*, void*);
float GmHD_fopAcM_searchActorDistance(gm_class*, void*);
void GmHD_fopAcM_orderPotentialEvent(gm_class*, int, int, int);
void GmHD_fopAcM_createDisappear(gm_class*, cXyz*, int, int, unsigned char);
void GmHD_fopAcM_getGroundAngle(gm_class*, csXyz*);
void GmHD_fopAcM_setGbaName(gm_class*, int, int, int);
void GmHD_mDoAud_subBgmStart(unsigned int);
void GmHD_mDoAud_subBgmStop();
void GmHD_mDoAud_bgmAllMute(int);
void GmHD_mDoAud_seStart_SignedID(int, cXyz*, int, int);
void GmHD_mDoAud_seStart_UnsignedID(unsigned int, cXyz*, int, int);
void GmHD_mDoAud_seStartLevel(unsigned int, cXyz*, unsigned int, int, int);
void GmHD_mDoExt_McaMorf_setAnm(mDoExt_McaMorf*, void*, unsigned char, void*, float, float, float,
                                float);
void* GmHD_mDoExt_McaMorf_mDoExt_McaMorf(void*, void*, int, int, int, int, int, int, int, int, int,
                                         int, float);
void GmHD_mDoExt_McaMorf_play(mDoExt_McaMorf*, cXyz*, int, int);
void GmHD_mDoExt_McaMorf_updateDL(mDoExt_McaMorf*);
void GmHD_mDoExt_McaMorf_entryDL(mDoExt_McaMorf*);
void GmHD_mDoExt_McaMorf_calc(mDoExt_McaMorf*);
void GmHD_mDoExt_McaMorf_stopZelAnime(mDoExt_McaMorf*);
void GmHD_mDoExt_baseAnm_play(void*);
void* GmHD_mDoExt_brkAnm_init(void*);
int GmHD_mDoExt_brkAnm_init(void*, void*, void*, int, int, int, int, int, int, float);
void GmHD_mDoExt_brkAnm_entry(void*, void*, float);
void GmHD_mDoExt_invisibleModel_ctor(void*);
void GmHD_mDoExt_invisibleModel_dtor(void*, int);
int GmHD_mDoExt_invisibleModel_create(void*, void*);
void GmHD_mDoMtx_YrotS(void*, short);
void* GmHD_dRes_control_c_getRes(void*, SafeString*, int);
gm_class* GmHD_operator_new_Actor(int);
void* GmHD_operator_new_Storage(int);
void GmHD_operator_delete_Actor(gm_class*);
void GmHD_operator_delete_Storage(void*);
unsigned int GmHD_JUTNameTab_getIndex(void*, const char*);
int GmHD_J3DFrameCtrl_checkPass(void*, float);
void* GmHD_J3DModelData_getJointTree(void*);
void GmHD_J3DModel_setMaterialPacketFlags(void*, unsigned int);
void* GmHD_J3DNode_toJoint(void*);
void GmHD_JPASetRMtxTVecfromMtx(void*, void*, void*);
void GmHD_PSVECAdd(cXyz*, cXyz*, cXyz*);
void GmHD_PSVECSubtract(cXyz*, cXyz*, cXyz*);
void GmHD_PSMTXCopy(void*, void*);
void GmHD_PSMTXTrans(void*, float, float, float);
void GmHD_csXyz_defaultConstruct(void*);
void GmHD_cXyz_defaultConstruct(void*);
void GmHD___register_global_object(void*);
float GmHD_sqrtf(float);

WWHD_OFFSET(GmAttackInfo, mpObj, 0);
WWHD_OFFSET(GmAttackInfo, pParticlePos, 0x14);
WWHD_SIZE(GmSaveHeaderHD, 0x12C0);

namespace GmCollision {
enum dCcD_ObjAtType {
    /* 0x00000002 */ AT_TYPE_SWORD = (1 << 1),
    /* 0x00000008 */ AT_TYPE_UNK8 = (1 << 3),
    /* 0x00000020 */ AT_TYPE_BOMB = (1 << 5),
    /* 0x00000040 */ AT_TYPE_BOOMERANG = (1 << 6),
    /* 0x00000080 */ AT_TYPE_BOKO_STICK = (1 << 7),
    /* 0x00000100 */ AT_TYPE_WATER = (1 << 8),
    /* 0x00000200 */ AT_TYPE_FIRE = (1 << 9),
    /* 0x00000400 */ AT_TYPE_MACHETE = (1 << 10),
    /* 0x00000800 */ AT_TYPE_UNK800 = (1 << 11),
    /* 0x00001000 */ AT_TYPE_SPIKE = (1 << 12),
    /* 0x00002000 */ AT_TYPE_UNK2000 = (1 << 13),
    /* 0x00004000 */ AT_TYPE_NORMAL_ARROW = (1 << 14),
    /* 0x00008000 */ AT_TYPE_HOOKSHOT = (1 << 15),
    /* 0x00010000 */ AT_TYPE_SKULL_HAMMER = (1 << 16),
    /* 0x00020000 */ AT_TYPE_UNK20000 = (1 << 17),
    /* 0x00040000 */ AT_TYPE_FIRE_ARROW = (1 << 18),
    /* 0x00080000 */ AT_TYPE_ICE_ARROW = (1 << 19),
    /* 0x00100000 */ AT_TYPE_LIGHT_ARROW = (1 << 20),
    /* 0x00200000 */ AT_TYPE_WIND = (1 << 21),
    /* 0x00400000 */ AT_TYPE_UNK400000 = (1 << 22),
    /* 0x00800000 */ AT_TYPE_LIGHT = (1 << 23),
    /* 0x01000000 */ AT_TYPE_STALFOS_MACE = (1 << 24),
    /* 0x02000000 */ AT_TYPE_FAN_SWING = (1 << 25),
    /* 0x04000000 */ AT_TYPE_DARKNUT_SWORD = (1 << 26),
    /* 0x08000000 */ AT_TYPE_GRAPPLING_HOOK = (1 << 27),
    /* 0x10000000 */ AT_TYPE_MOBLIN_SPEAR = (1 << 28),
    /* 0x20000000 */ AT_TYPE_PGANON_SWORD = (1 << 29),
    /* 0xFFFFFFFF */ AT_TYPE_ALL = ~0,
};
}
