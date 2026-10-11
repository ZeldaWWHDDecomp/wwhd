/* Particle emitter HD view: 8-byte volume method descriptor, larger draw storage,
 * and an extra 0x28 emitter-info block before volume vectors. GC names preserved.
 * HD teardown countdowns live in the expanded emitter tail. */
#pragma once
#include "wwhd.h"
namespace jpa_emitter_calc {
enum : u32 {
    JPAEmtrStts_StopEmit = 1, JPAEmtrStts_StopCalc = 2,
    JPAEmtrStts_EnableDeleteEmitter = 8, JPAEmtrStts_FirstEmit = 0x10,
    JPAEmtrStts_RateStepEmit = 0x20, JPAEmtrStts_Immortal = 0x40,
    JPADynFlag_FixedDensity = 1, JPADynFlag_FixedInterval = 2,
    CallbackExecute = 0x1C, CallbackExecuteAfter = 0x24,
};
struct JPABaseEmitter;
struct EmitterCallback;
struct JPADataBlockLinkInfo;
struct JPABaseEmitter {
    bool doStartFrameProcess();
    bool doTerminationProcess();
    void calc();
    void calcCreatePtcls();
    void calcKey();
    void calcEmitterGlobalPosition(void *dst);
    void calcEmitterInfo();
    void calcVolumePoint();
    void calcVolumeLine();
    void calcVolumeCube();
    void calcVolumeCircle();
    void calcVolumeSphere();
    void calcVolumeCylinder();
    void calcVolumeTorus();
    u8 mVolumeFunc[8];
    be<f32> mEmitterScale[3];
    be<f32> mEmitterTranslation[3];
    be<s16> mEmitterRot[3];
    be<u8> mVolumeType, mRateStep;
    be<f32> mEmitterDir[3];
    be<f32> mRate, mRateRndm, mAccel, mAccelRndm, mAirResist, mAirResistRndm;
    be<f32> mMoment, mMomentRndm, mLifeTimeRndm, mSpread;
    be<s32> mMaxFrame;
    be<s16> mLifeTime, mStartFrame;
    be<u16> mVolumeSize, mDivNumber;
    be<f32> mInitialVelOmni, mInitialVelAxis, mInitialVelDir, mInitialVelRndm, mInitialVelRatio;
    be<f32> mVolumeSweep, mVolumeMinRad;
    be<u32> mDataFlag, mUseKeyFlag;
    u8 _08C[0x9C - 0x8C];
    u8 mDraw[0x180 - 0x9C];
    be<f32> mKeyScale;
    u8 _184[0x194 - 0x184];
    be<f32> mTick, mTime;
    u8 mFieldManager[0x1B4 - 0x19C];
    be<u32> mActiveParticleCount;
    u8 _1B8[8];
    be<u32> mChildParticleCount;
    u8 _1C4[0x1E0 - 0x1C4];
    gptr<JPADataBlockLinkInfo> mpDataLinkInfo;
    gptr<EmitterCallback> mpEmitterCallBack;
    be<u32> mpParticleCallBack, mRandomSeed;
    be<f32> mGlobalRotation[12], mGlobalDynamicsScale[3], mGlobalTranslation[3];
    u8 pad238[0x24c - 0x238];
    be<f32> mEmitCount, mRateStepTimer;
    be<u32> mFlags;
    u8 pad258[0x3d4 - 0x258];
    be<u8> mDeleteCountdown, mClearCountdown;
    u8 pad3d6[0x3fc - 0x3d6];
};
WWHD_OFFSET(JPABaseEmitter, mEmitterScale, 8);
WWHD_OFFSET(JPABaseEmitter, mEmitterTranslation, 0x14);
WWHD_OFFSET(JPABaseEmitter, mEmitterRot, 0x20);
WWHD_OFFSET(JPABaseEmitter, mDataFlag, 0x84);
WWHD_OFFSET(JPABaseEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(JPABaseEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(JPABaseEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(JPABaseEmitter, mTick, 0x194);
WWHD_OFFSET(JPABaseEmitter, mRandomSeed, 0x1ec);
WWHD_OFFSET(JPABaseEmitter, mFlags, 0x254);
WWHD_OFFSET(JPABaseEmitter, mDeleteCountdown, 0x3d4);
static_assert(sizeof(JPABaseEmitter) == 0x3fc);

struct CallbackVtable { be<u32> slots[10]; };
struct EmitterCallback { gptr<CallbackVtable> __vtbl; };
struct KeyVtable {
    u8 _00[0x14]; be<u32> getID; u8 _18[4];
    be<u32> isLoopEnable; u8 _20[4];
    be<u32> getNumber; u8 _28[4]; be<u32> getKeyDataPtr;
};
struct JPAKeyBlock { gptr<KeyVtable> __vtbl; };
struct KeyFrame { be<f32> time; be<f32> values[3]; };
struct JPADataBlockLinkInfo {
    u8 _00[0x14]; gptr<gptr<JPAKeyBlock>> keyBlocks;
    u8 _18[0xA]; be<u8> keyNum;
};
struct EmitterLink { gptr<JPABaseEmitter> object; u8 _04[8]; gptr<EmitterLink> next; };
struct EmitterList { gptr<EmitterLink> first; u8 _04[8]; };
struct JPAEmitterManager {
    u8 _00[0x50]; EmitterList mEmtrGroup[16];
    void calc(u8 group);
};
struct SinCosEntry { be<f32> sin, cos; };
struct JPAEmitterInfo {
    be<u32> mRandom; gptr<JPABaseEmitter> mpCurEmitter;
    be<f32> mEmitterGlobalSR[12], mEmitterGlobalRot[12], mGlobalRot[12], mEmitterDirMtx[12];
    be<f32> mEmitterGlobalScale[3], mEmitterTranslation[3], mEmitterGlobalCenter[3], mPublicScale[3];
    u8 _0F8[0x134 - 0xF8];
    be<f32> mVolumePos[3], mVelOmni[3], mVelAxis[3], mVolumeSize;
    be<s32> mVolumeEmitCount;
    be<u32> mVolumeEmitIdx, mVolumeEmitAngleCount, mVolumeEmitXCount, mVolumeEmitAngleMax, mDivNumber;
};
WWHD_OFFSET(JPABaseEmitter, mKeyScale, 0x180);
WWHD_OFFSET(JPABaseEmitter, mActiveParticleCount, 0x1B4);
WWHD_OFFSET(JPABaseEmitter, mChildParticleCount, 0x1C0);
WWHD_OFFSET(JPABaseEmitter, mpDataLinkInfo, 0x1E0);
WWHD_OFFSET(JPABaseEmitter, mFieldManager, 0x19C);
WWHD_OFFSET(JPABaseEmitter, mDraw, 0x9C);
WWHD_OFFSET(JPADataBlockLinkInfo, keyBlocks, 0x14);
WWHD_OFFSET(JPADataBlockLinkInfo, keyNum, 0x22);
WWHD_SIZE(JPADataBlockLinkInfo, 0x23);
WWHD_OFFSET(EmitterLink, next, 0xC);
WWHD_SIZE(EmitterLink, 0x10);
WWHD_SIZE(EmitterList, 0xC);
WWHD_OFFSET(JPAEmitterManager, mEmtrGroup, 0x50);
WWHD_SIZE(JPAEmitterManager, 0x110);
WWHD_SIZE(CallbackVtable, 0x28);
WWHD_SIZE(EmitterCallback, 4);
WWHD_OFFSET(KeyVtable, getID, 0x14);
WWHD_OFFSET(KeyVtable, getKeyDataPtr, 0x2C);
WWHD_SIZE(KeyVtable, 0x30);
WWHD_SIZE(JPAKeyBlock, 4);
WWHD_SIZE(KeyFrame, 0x10);
WWHD_SIZE(SinCosEntry, 8);
WWHD_OFFSET(JPAEmitterInfo, mEmitterGlobalScale, 0xC8);
WWHD_OFFSET(JPAEmitterInfo, mVolumePos, 0x134);
WWHD_OFFSET(JPAEmitterInfo, mVolumeEmitCount, 0x15C);
WWHD_SIZE(JPAEmitterInfo, 0x174);
} // namespace jpa_emitter_calc
void JPADeleteAllParticleHD(jpa_emitter_calc::JPABaseEmitter*, u32);
void JPACalcKey(jpa_emitter_calc::JPABaseEmitter*);
void JPACalcEmitterInfo(jpa_emitter_calc::JPABaseEmitter*);
void JPADrawCalc(void*);
void JPAFieldPreCalc(void*);
void JPACalcCreatePtcls(jpa_emitter_calc::JPABaseEmitter*);
void JPACalcParticle(jpa_emitter_calc::JPABaseEmitter*);
void JPACalcChild(jpa_emitter_calc::JPABaseEmitter*);
void JPADeleteEmitter(jpa_emitter_calc::JPAEmitterManager*, jpa_emitter_calc::JPABaseEmitter*);
void* JPACreateParticle(jpa_emitter_calc::JPABaseEmitter*);
f32 JPAGetKeyFrameValueHD(u32, void*, f32);
void MTXScale(void*, f32, f32, f32);
void MTXConcat(void*, void*, void*);
void MTXCopy(void*, void*);
void MTXMultVec(void*, void*, void*);
void JPAGetXYZRotateMtx(s16, s16, s16, void*);
void JPAGetDirMtx(void*, void*);
void JPAAssert(void*, s32, void*);
