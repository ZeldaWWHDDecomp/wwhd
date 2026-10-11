#pragma once
#include "d/d_cc_uty.h"
#include "gc_api/gc_api.h"

// HD: the ice effect retains the GameCube member layout.
struct Am2EnemyIce {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 mPadding;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ u8 mMotionState[0x30 - 0xC];
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ u8 mDeathState[4];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_OFFSET(Am2EnemyIce, mCyl, 0x6C);
WWHD_OFFSET(Am2EnemyIce, mParticleScale, 0x1AC);
WWHD_OFFSET(Am2EnemyIce, mBgAcch, 0x1F4);
WWHD_SIZE(Am2EnemyIce, 0x3B8);
struct Am2RippleCallback {
    u8 mCallbackState[0x10];
    be<f32> mRate;
    void end() { gabi::call(0x025A9270, this); }
};
WWHD_OFFSET(Am2RippleCallback, mRate, 0x10);
WWHD_SIZE(Am2RippleCallback, 0x14);
inline be<u32>& am2Word(be<f32>& f) { return *reinterpret_cast<be<u32>*>(&f); }

// Armos: the HD actor base adds 0x11C; actor-local subobjects keep their GC sizes.
struct am2_class : fopEn_enemy_c {
    request_of_phase_process_class mPhase;
    be<u32> mEyeJntHit;
    gptr<mDoExt_McaMorf> mpMorf;
    be<u32> mReservedModel;
    gptr<mDoExt_btkAnm> mpBtkAnm;
    gptr<mDoExt_brkAnm> mpBrkAnm;
    be<u8> mType, mPrmAreaRadius, mStartsInactive, mSwitch;
    be<u8> mAction, mMode, mHitDirection, mbIsWeakBeingHit;
    be<u8> mbNotInHomeRoom, mbMadeWaterSplash, mInAbyssTimer;
    u8 _3EF[0x3FC - 0x3EF];
    cXyz mEyeballPos, mNeedlePos, mWeakPos, m304, mSpawnPos, mLinChkCenter, mLinChkDest;
    be<s16> mCountDownTimers[5], mCountUpTimers[4], mTargetAngleY, mSpawnRotY;
    u8 _466[2];
    be<s32> mCurrBckIdx;
    be<f32> mCorrectionOffsetY, mAreaRadius, mPickedUpYPos, mAcchRadius;
    dBgS_AcchCir mAcchCir;
    dBgS_ObjAcch mAcch;
    dCcD_Stts mStts;
    dCcD_Cyl mBodyCyl, mNeedleCyl;
    dCcD_Sph mEyeSph, mWeakSph;
    dPa_smokeEcallBack mSmokeCb;
    Am2RippleCallback mRippleCb;
    Am2EnemyIce mEnemyIce;
};
WWHD_OFFSET(am2_class, mpMorf, 0x3D4);
WWHD_OFFSET(am2_class, mAction, 0x3E8);
WWHD_OFFSET(am2_class, mWeakPos, 0x414);
WWHD_OFFSET(am2_class, mCountDownTimers, 0x450);
WWHD_OFFSET(am2_class, mAcch, 0x4BC);
WWHD_OFFSET(am2_class, mBodyCyl, 0x6BC);
WWHD_OFFSET(am2_class, mRippleCb, 0xB94);
WWHD_OFFSET(am2_class, mEnemyIce, 0xBA8);
WWHD_SIZE(am2_class, 0xF60);

// HD: these constructors return the constructed pointer, which can differ from the storage.
inline mDoExt_btkAnm* am2ConstructBtk(mDoExt_btkAnm* p) {
    return gabi::call<mDoExt_btkAnm*>(0x025E7C6C, p);
}
inline mDoExt_brkAnm* am2ConstructBrk(mDoExt_brkAnm* p) {
    return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p);
}
inline u32 am2CreateJntHit(J3DModel* model, void* data, s32 count) {
    return gabi::call<u32>(0x02552B60, model, data, count);
}

// HD: the node accessor has already returned the joint data.
struct Am2JointView {
    be<u32> mpData;
    be<u16> mJntNo;
};
WWHD_OFFSET(Am2JointView, mJntNo, 4);

struct Am2PlayerView : fopAc_ac_c {
    be<u8> mCutType;
    u8 mCutState[3];
    be<s16> mDamageWaitTimer;
};
WWHD_OFFSET(Am2PlayerView, mCutType, 0x3AC);
WWHD_OFFSET(Am2PlayerView, mDamageWaitTimer, 0x3B0);
inline dComIfG_inf_c* am2GameInfo() { return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get()); }
// HD: the character-name table is embedded in the game-info object.
struct Am2CharTable {
    s32 GetNameIndex(const char* name, s32 index) {
        return gabi::call<s32>(0x0200E814, this, name, index);
    }
};
inline Am2CharTable* am2CharTable(dComIfG_inf_c* info) {
    return gabi::at<Am2CharTable>(gabi::ea(info) + 0x50AC);
}

struct Am2CameraBody {
    void ForceLockOff(s32 actorID) { gabi::call(0x025052BC, this, actorID); }
};
struct Am2CameraView {
    u8 mActor[0x248];
    Am2CameraBody mCamera;
};
WWHD_OFFSET(Am2CameraView, mCamera, 0x248);
inline Am2CameraView* am2Camera(dComIfG_inf_c* info) {
    return gabi::at<Am2CameraView>(info->play.mCameraInfo[0].mpCamera);
}
struct Am2DebugRegisters {
    be<f32> mWaterPillarScaleAdjust;
    be<f32> mEyeOffsetAdjust;
    be<f32> mNeedleRadiusAdjust;
    u8 mUnusedA[0x1047BAB8 - 0x1047BAA0];
    be<f32> mAcchCarryRadiusAdjust;
    be<f32> mAcchActiveRadiusAdjust;
    be<f32> mAcchHeightAdjust;
    u8 mUnusedB[0x1047BB1A - 0x1047BAC4];
    be<s16> mForceType;
    u8 mUnusedC[0x1047BCD0 - 0x1047BB1C];
    be<f32> mWaterDepthAdjust;
    u8 mUnusedD[0x1047BD1C - 0x1047BCD4];
    be<f32> mLineHeightAdjust;
};
WWHD_OFFSET(Am2DebugRegisters, mForceType, 0x86);
WWHD_OFFSET(Am2DebugRegisters, mLineHeightAdjust, 0x288);
WWHD_SIZE(Am2DebugRegisters, 0x28C);
inline Am2DebugRegisters& am2Debug() { return *gabi::at<Am2DebugRegisters>(0x1047BA94); }

inline void am2AttackCheck(fopAc_ac_c* actor, CcAtInfo* info) {
    gabi::call(0x025192A8, actor, info);
}

// HD: GHS initializes these overlapping shape/base vtables in different orders.
struct Am2ShapeAttrView {
    cXyz mMin, mMax;
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
};
WWHD_SIZE(Am2ShapeAttrView, 0x20);
inline Am2ShapeAttrView& am2Shape(dCcD_Cyl& cyl) {
    return *reinterpret_cast<Am2ShapeAttrView*>(cyl.mShapeAttr);
}
inline void am2ActorConstruct(am2_class* actor) { gabi::call(0x025D4ED0, actor); }
inline void am2AcchConstruct(dBgS_ObjAcch* acch) { gabi::call(0x024F0474, acch); }
inline void am2StatusBaseConstruct(dCcD_Stts* status) { gabi::call(0x0200BD2C, status); }
inline void am2GameStatusConstruct(be<u32>* status) { gabi::call(0x02515DA0, status); }
inline void am2CollisionConstruct(dCcD_Cyl* cyl) { gabi::call(0x02515FB8, cyl); }
inline void am2CylinderConstruct(cM3dGCyl* cyl) { gabi::call(0x02018590, cyl); }
inline void am2SphereConstruct(dCcD_Sph* sph) { gabi::call(0x025166F0, sph); }
inline void am2SmokeConstruct(dPa_smokeEcallBack* smoke) { gabi::call(0x025A5B18, smoke, 1); }
inline void am2RippleConstruct(Am2RippleCallback* ripple) { gabi::call(0x025A9084, ripple); }
inline void am2AcchDestruct(dBgS_ObjAcch* acch) { gabi::call(0x024EFD9C, acch, 0); }
inline void am2CircleDestruct(cM3dGCir_l* circle) { gabi::call(0x02018034, circle, 2); }
inline void am2SphereDestruct(dCcD_Sph* sph) { gabi::call(0x02515AE8, sph, 2); }

struct Am2EmitterView {
    u8 mEmitterState[0x34];
    be<f32> mRate;
    u8 mDynamicsState[0x220 - 0x38];
    cXyz mGlobalDynamicsScale;
    u8 mTransformState[0x238 - 0x22C];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(Am2EmitterView, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(Am2EmitterView, mGlobalParticleScale, 0x238);
inline Am2EmitterView* am2SmokeEmitter(am2_class* actor) {
    return reinterpret_cast<Am2EmitterView*>(actor->mSmokeCb.mpEmitter.get());
}
inline void am2SmokeScale(Am2EmitterView* emitter, f32 scale) {
    // HD: keep GHS's interleaved component store order.
    emitter->mGlobalDynamicsScale.y = scale;
    emitter->mGlobalParticleScale.z = scale;
    emitter->mGlobalParticleScale.x = scale;
    emitter->mGlobalParticleScale.y = scale;
    emitter->mGlobalDynamicsScale.x = scale;
    emitter->mGlobalDynamicsScale.z = scale;
}
struct Am2HeaderStatics {
    be<f32> mNegativePi, mPi;
    be<u8> mMathInit, mRandomInit;
    u8 mPadding[2];
    be<u32> mZeroState[4];
};
WWHD_SIZE(Am2HeaderStatics, 0x1C);
inline Am2HeaderStatics& am2HeaderStatics() { return *gabi::at<Am2HeaderStatics>(0x104613DC); }
struct Am2AngleLimits {
    be<f32> mNegativePi, mPi;
};
inline const Am2AngleLimits& am2AngleLimits() { return *gabi::at<Am2AngleLimits>(0x10007110); }
inline void am2MathInit(be<u8>* state) { gabi::call(0x028ED6F8, state); }
inline void am2RandomInit(be<u8>* state) { gabi::call(0x028EAB2C, state); }

inline void am2LineSet(dBgS_LinChk* check, cXyz* center, const cXyz* destination,
                       am2_class* actor) {
    gabi::call(0x024F1AFC, check, center, destination, actor);
}
inline void am2LineDestruct(dBgS_LinChk* check) { gabi::call(0x02008B4C, check, 0); }

// HD: this call returns a signed halfword, unlike the shared full-word angle-distance API.
inline s16 am2DistanceAngleS(s16 a, s16 b) { return gabi::call<s16>(0x0200FAAC, a, b); }
inline void am2MonsterSound(u32 id, cXyz* pos, s32 process, s32 parameter, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, process, parameter, reverb);
}
inline void am2SetGbaName(am2_class* actor, u8 item, u8 first, u8 second) {
    gabi::call(0x025DA088, actor, item, first, second);
}
inline BOOL am2EnemyIceProcess(Am2EnemyIce* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
inline void am2PositionMove(am2_class* actor, void* collision) {
    gabi::call(0x025D6800, actor, collision);
}

inline void* am2ResourceGet(dRes_control_c* control, SafeString* archive, s32 index) {
    return gabi::call<void*>(0x026066C4, control, archive, index);
}
inline BOOL am2CheckPolySafe(dBgS* background, cBgS_PolyInfo* polygon) {
    return gabi::call<BOOL>(0x02008254, background, polygon);
}
inline s32 am2GroundCode(dBgS* background, cBgS_PolyInfo* polygon) {
    return gabi::call<s32>(0x024EF0BC, background, polygon);
}
inline Am2CameraBody* am2CameraBody() { return gabi::call<Am2CameraBody*>(0x024F8044); }

// HD: flat collision-source layouts allow the verifier to check every stored field.
struct Am2CylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
    be<f32> mHeight;
};
WWHD_SIZE(Am2CylinderSource, 0x44);
struct Am2SphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
};
WWHD_SIZE(Am2SphereSource, 0x40);
struct Am2JointHitData {
    be<s16> mShapeType;
    be<s16> mJointIndex;
    be<f32> mRadius;
    be<u32> mpOffsets; // HD: the serialized guest pointer in the source table.
};
WWHD_SIZE(Am2JointHitData, 0xC);
struct Am2JointOffset {
    be<f32> x;
    be<f32> y;
    be<f32> z;
};
WWHD_SIZE(Am2JointOffset, 0xC);
