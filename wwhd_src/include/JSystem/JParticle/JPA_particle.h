#pragma once
#include "wwhd.h"
// HD: tww particle fields retain their offsets; GPU state and deferred deletion follow them.
struct JPABaseParticle;
struct JPABaseEmitter;
struct JPAParticleLink {
    gptr<JPABaseParticle> mpObject;
    be<u32> mList;
    gptr<JPAParticleLink> mpPrev, mpNext;
};
struct JPAParticleList {
    gptr<JPAParticleLink> mpHead, mpTail;
    be<u32> mCount;
};
struct JPAParticleVector {
    be<u32> mSize, mCapacity;
    gptr<gptr<JPABaseParticle>> mpData;
};
struct JPAParticleCallback;
struct JPAGpuState {
    u8 mPrefix[0x368];
    be<u32> mPendingCommands[2];
};
struct JPAParticleRenderer {
    u8 mOpaque;
};
struct JPAParticleSlot {
    u8 mOpaque;
};
struct JPABaseParticle {
    void initParticle();
    void initChild(JPABaseParticle*);
    void incFrame();
    void calcVelocity();
    void calcPosition();
    bool checkCreateChild();
    /*0x000*/ JPAParticleLink mLink;
    /*0x010*/ be<f32> mOffsetPosition[3];
    /*0x01C*/ be<f32> mLocalPosition[3];
    /*0x028*/ be<f32> mGlobalPosition[3];
    /*0x034*/ be<f32> mVelocity[3];
    /*0x040*/ be<f32> mBaseVel[3];
    /*0x04C*/ be<f32> mAccel[3];
    /*0x058*/ be<f32> mFieldVel[3];
    /*0x064*/ be<f32> mFieldAccel[3];
    /*0x070*/ be<f32> mAirResist, mMoment, mCurFrame, mLifeTime, mCurNormTime, mFieldDrag, mDrag;
    /*0x08C*/ u8 mDrawParams[0x3C];
    /*0x0C8*/ gptr<JPAParticleCallback> mpCallBack2;
    /*0x0CC*/ be<u32> mStatus;
    /*0x0D0*/ be<u32> mChildState; // tww's unnamed field_0xd0.
    /*0x0D4*/ gptr<JPABaseEmitter> mpEmitter;
    u8 mBeforeSlot[0xC];
    /*0x0E4*/ gptr<JPAParticleSlot> mpSlot;
    u8 mBeforeGpu[4];
    /*0x0EC*/ gptr<JPAGpuState> mpGpuState;
    /*0x0F0*/ gptr<JPAParticleRenderer> mpRenderer;
    u8 mBeforeDelete[0x18];
    /*0x10C*/ be<u8> mDeletionDelay;
    u8 mBeforeHidden[0x13];
    /*0x120*/ be<u8> mHidden;
};
struct JPAShapeVtable {
    u8 mPrefix[0x14];
    gfn getType;
    u8 mBeforeLife[0x1C];
    gfn getLife;
    be<u32> mLifeDelta;
    gfn getRate;
    be<u32> mRateDelta;
    gfn getTiming;
    be<u32> mTimingDelta;
    gfn getStep;
    be<u32> mStepDelta;
    gfn getPosRndm;
    be<u32> mPosDelta;
    gfn getVelInfRate;
    be<u32> mVelInfDelta;
    gfn getBaseVel;
    be<u32> mBaseVelDelta;
    gfn getBaseVelRndm;
    be<u32> mBaseVelRndmDelta;
    gfn getGravity;
    be<u32> mGravityDelta;
    gfn isEnableField;
    be<u32> mFieldDelta;
    gfn isEnableDrawParent;
};
struct JPAShape {
    gptr<JPAShapeVtable> mpVtable;
};
struct JPADataBlockLinkInfo {
    u8 mPrefix[4];
    gptr<JPAShape> mpBaseShape;
    u8 mBeforeSweep[4];
    gptr<JPAShape> mpSweepShape;
};
struct JPAParticleCallbackVtable {
    u8 mPrefix[0x14];
    gfn init;
    be<u32> mInitDelta;
    gfn execute;
};
struct JPAParticleCallback {
    gptr<JPAParticleCallbackVtable> mpVtable;
};
struct JPADraw {
    u8 mState[0x100];
};
struct JPAFieldManager {
    u8 mState[0x10];
};
struct JPABaseEmitter {
    JPABaseParticle* getPtclFromVacList();
    JPABaseParticle* createParticle();
    void createChildren(JPABaseParticle*);
    void calcParticle();
    void calcChild();
    void deleteParticle(JPABaseParticle*);
    void releaseParticle(JPABaseParticle*, JPAParticleList*);
    void deleteAllParticle(s32);
    /*0x000*/ gabi::ptmf<void (JPABaseEmitter::*)()> mVolumeFunc;
    /*0x008*/ be<f32> mEmitterScale[3];
    u8 mBeforeAccel[0x28];
    /*0x03C*/ be<f32> mAccel, mAccelRndm, mAirResist, mAirResistRndm, mMoment, mMomentRndm, mLifeTimeRndm, mSpread;
    u8 mBeforeLife[4];
    /*0x060*/ be<s16> mLifeTime;
    u8 mBeforeVelocity[6];
    /*0x068*/ be<f32> mInitialVelOmni, mInitialVelAxis, mInitialVelDir, mInitialVelRndm, mInitialVelRatio;
    u8 mBeforeDataFlags[8];
    /*0x084*/ be<u32> mDataFlag;
    u8 mBeforeDraw[0x14];
    /*0x09C*/ JPADraw mDraw;
    /*0x19C*/ JPAFieldManager mFieldManager;
    /*0x1AC*/ JPAParticleList mActiveParticles;
    /*0x1B8*/ JPAParticleList mChildParticles;
    // HD: deferred particle batches precede the vacant list.
    /*0x1C4*/ JPAParticleVector mDeferredParticles[2];
    /*0x1DC*/ gptr<JPAParticleList> mpPtclVacList;
    /*0x1E0*/ gptr<JPADataBlockLinkInfo> mpDataLinkInfo;
    be<u32> mpEmitterCallBack;
    /*0x1E8*/ gptr<JPAParticleCallback> mpParticleCallBack;
    /*0x1EC*/ be<u32> mRandomSeed;
    u8 mBeforeSlots[0x110];
    /*0x300*/ gptr<JPAParticleSlot> mpSlots[2];
    u8 mBeforeDeleteState[0xCC];
    /*0x3D4*/ be<u8> mDeleteRequested, mDeletePending, mDeleteEnabled;
};
struct JPAEmitterInfo {
    be<u32> mRandom;
    gptr<JPABaseEmitter> mpCurEmitter;
    be<f32> mEmitterGlobalSR[12], mEmitterGlobalRot[12], mGlobalRot[12], mEmitterDirMtx[12];
    be<f32> mEmitterGlobalScale[3], mEmitterTranslation[3], mEmitterGlobalCenter[3], mPublicScale[3];
    u8 mBeforeVolume[0x3C];
    be<f32> mVolumePos[3], mVelOmni[3], mVelAxis[3];
};
struct JPAFieldWorkspace {
    u8 mState;
};
WWHD_OFFSET(JPABaseParticle, mCurFrame, 0x78);
WWHD_OFFSET(JPABaseParticle, mStatus, 0xCC);
WWHD_OFFSET(JPABaseParticle, mpGpuState, 0xEC);
WWHD_OFFSET(JPABaseParticle, mDeletionDelay, 0x10C);
WWHD_OFFSET(JPABaseParticle, mHidden, 0x120);
WWHD_OFFSET(JPABaseEmitter, mAccel, 0x3C);
WWHD_OFFSET(JPABaseEmitter, mInitialVelOmni, 0x68);
WWHD_OFFSET(JPABaseEmitter, mDataFlag, 0x84);
WWHD_OFFSET(JPABaseEmitter, mDraw, 0x9C);
WWHD_OFFSET(JPABaseEmitter, mActiveParticles, 0x1AC);
WWHD_OFFSET(JPABaseEmitter, mpPtclVacList, 0x1DC);
WWHD_OFFSET(JPABaseEmitter, mRandomSeed, 0x1EC);
WWHD_OFFSET(JPABaseEmitter, mpSlots, 0x300);
WWHD_OFFSET(JPABaseEmitter, mDeleteRequested, 0x3D4);
WWHD_OFFSET(JPAEmitterInfo, mEmitterGlobalCenter, 0xE0);
WWHD_OFFSET(JPAEmitterInfo, mVolumePos, 0x134);
WWHD_OFFSET(JPAEmitterInfo, mVelOmni, 0x140);
WWHD_OFFSET(JPAShapeVtable, getLife, 0x34);
WWHD_OFFSET(JPAShapeVtable, getTiming, 0x44);
WWHD_OFFSET(JPAShapeVtable, isEnableDrawParent, 0x84);
WWHD_SIZE(JPAParticleLink, 0x10);
WWHD_OFFSET(JPAParticleLink, mpNext, 0xC);
WWHD_SIZE(JPAParticleList, 0xC);
WWHD_OFFSET(JPAParticleList, mCount, 8);
WWHD_SIZE(JPAParticleVector, 0xC);
WWHD_OFFSET(JPAParticleVector, mpData, 8);
WWHD_OFFSET(JPAGpuState, mPendingCommands, 0x368);
WWHD_SIZE(JPAParticleRenderer, 1);
WWHD_SIZE(JPAParticleSlot, 1);
WWHD_OFFSET(JPABaseParticle, mOffsetPosition, 0x10);
WWHD_OFFSET(JPABaseParticle, mLocalPosition, 0x1C);
WWHD_OFFSET(JPABaseParticle, mGlobalPosition, 0x28);
WWHD_OFFSET(JPABaseParticle, mVelocity, 0x34);
WWHD_OFFSET(JPABaseParticle, mBaseVel, 0x40);
WWHD_OFFSET(JPABaseParticle, mAccel, 0x4C);
WWHD_OFFSET(JPABaseParticle, mFieldVel, 0x58);
WWHD_OFFSET(JPABaseParticle, mFieldAccel, 0x64);
WWHD_OFFSET(JPABaseParticle, mpCallBack2, 0xC8);
WWHD_OFFSET(JPABaseParticle, mpEmitter, 0xD4);
WWHD_OFFSET(JPABaseParticle, mpSlot, 0xE4);
WWHD_OFFSET(JPABaseParticle, mpRenderer, 0xF0);
WWHD_OFFSET(JPAShapeVtable, getType, 0x14);
WWHD_OFFSET(JPAShapeVtable, getRate, 0x3C);
WWHD_OFFSET(JPAShapeVtable, getBaseVel, 0x64);
WWHD_SIZE(JPAShape, 4);
WWHD_OFFSET(JPADataBlockLinkInfo, mpBaseShape, 4);
WWHD_OFFSET(JPADataBlockLinkInfo, mpSweepShape, 0xC);
WWHD_OFFSET(JPAParticleCallbackVtable, init, 0x14);
WWHD_OFFSET(JPAParticleCallbackVtable, execute, 0x1C);
WWHD_SIZE(JPAParticleCallback, 4);
WWHD_SIZE(JPADraw, 0x100);
WWHD_SIZE(JPAFieldManager, 0x10);
WWHD_OFFSET(JPABaseEmitter, mChildParticles, 0x1B8);
WWHD_OFFSET(JPABaseEmitter, mDeferredParticles, 0x1C4);
WWHD_OFFSET(JPABaseEmitter, mpDataLinkInfo, 0x1E0);
WWHD_OFFSET(JPABaseEmitter, mpParticleCallBack, 0x1E8);
WWHD_OFFSET(JPAEmitterInfo, mEmitterGlobalSR, 8);
WWHD_OFFSET(JPAEmitterInfo, mEmitterGlobalRot, 0x38);
WWHD_OFFSET(JPAEmitterInfo, mEmitterDirMtx, 0x98);
WWHD_OFFSET(JPAEmitterInfo, mPublicScale, 0xEC);
WWHD_OFFSET(JPAEmitterInfo, mVelAxis, 0x14C);
WWHD_SIZE(JPAFieldWorkspace, 1);

enum JPAParticleStatus {
    JPAPtclStts_FirstFrame = 1,
    JPAPtclStts_Delete = 2,
    JPAPtlcStts_Child = 4,
    JPAPtclStts_UNK_20 = 0x20,
    JPAPtclStts_UNK_40 = 0x40,
    JPAPtclStts_UNK_80 = 0x80
};
enum JPADynamicsFlag { JPADynFlag_InheritScale = 4, JPADynFlag_FollowEmtr = 8, JPADynFlag_FollowEmtrChld = 16 };
void JPAFieldManager_calc(JPAFieldManager*, JPABaseParticle*);
void JPAFieldManager_init(JPAFieldManager*, JPABaseParticle*);
void JPA_rendererRelease(JPAParticleRenderer*, u32);
void JPA_particleDestroy(JPABaseParticle*);
void JSUPtrList_remove(JPAParticleList*, JPAParticleLink*);
void JSUPtrList_prepend(JPAParticleList*, JPAParticleLink*);
void JPADraw_initParticle(JPADraw*, JPABaseParticle*);
void JPADraw_initChild(JPADraw*, JPABaseParticle*, JPABaseParticle*);
void JPADraw_calcParticle(JPADraw*, JPABaseParticle*);
void JPADraw_calcChild(JPADraw*, JPABaseParticle*);
void JPA_deleteAllDeferred(JPABaseEmitter*);
void JPA_clearParticleVector(JPAParticleVector*);
void JPA_allocateParticleSlot(JPABaseEmitter*, s32);
void JPA_particleInitGpu(JPABaseParticle*, JPABaseEmitter*);
void JPA_particleInitField(JPABaseParticle*, JPAFieldWorkspace*);
void JPA_particleInitFieldDraw(JPABaseParticle*, JPAFieldWorkspace*);
void PSMTXMultVec(be<f32>*, be<f32>*, be<f32>*);
void PSMTXConcat(be<f32>*, be<f32>*, be<f32>*);
void JPAGetYZRotateMtx(s16, s16, be<f32>*);
void JPAGetUnitVec(s16, s16, be<f32>*);
f64 JPA_sqrt(f64);
