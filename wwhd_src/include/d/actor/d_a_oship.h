#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "d/d_cc_d.h"
#include "d/d_lib.h"
#include "d/d_particle.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"

struct OshipWaveEcallBack {
    be<u32> __vtbl;
    be<s16> mState, mFadeTimer;
    be<f32> mVelFade1, mVel, mVelSpeed, mVelFade2, mMaxParticleVelocity;
    cXyz mCollapsePos[2];
    be<u32> mpPos, mpRot;
    cXyz mRotMtx[3];
    be<u32> mpBaseEmitter;
};
struct OshipSplashEcallBack {
    be<u32> __vtbl;
    be<s16> mState;
    u8 padding6[2];
    be<f32> mScaleTimer, mMaxScaleTimer;
    be<u32> mpPos, mpRot, mpBaseEmitter;
};
struct OshipTrackEcallBack {
    be<u32> __vtbl;
    be<s16> mState;
    u8 padding6[2];
    be<f32> mBaseY, mMinY;
    cXyz mPos[3];
    be<u32> mpPos, mpRot;
    be<f32> mExTransY, mExScaleY, mVel, mMinVel;
    be<u32> mpBaseEmitter;
};
WWHD_SIZE(OshipWaveEcallBack, 0x64);
WWHD_SIZE(OshipSplashEcallBack, 0x1C);
WWHD_SIZE(OshipTrackEcallBack, 0x50);

// GameCube actor fields move by +0x11C after the HD base; total WWHD size is 0xF1C.
struct daOship_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e {
        MODE_WAIT,
        MODE_ATTACK,
        MODE_DAMAGE,
        MODE_DELETE,
        MODE_RANGE_A,
        MODE_RANGE_B,
        MODE_RANGE_C,
        MODE_RANGE_D,
        MODE_NULL
    };
    // HD: compact GHS descriptors forward the dispatch kind and mode in r4/r5.
    using ModeProcFunc = void (daOship_c::*)(s32, s32);
    struct ModeEntry {
        gabi::ptmf<ModeProcFunc> mInitFunc, mUpdFunc;
        gptr<const char> mModeName;
    };

    void getArg();
    void setAttention();
    void modeDamageInit();
    void modeRangeAInit();
    void modeRangeBInit();
    void modeRangeCInit();
    void modeRangeDInit();
    void modeProc(s32 kind, s32 mode);
    void changeModeByRange();
    void modeWaitInit();
    void modeWait();
    BOOL _createHeap();
    s32 _draw();
    s32 lineCheck(cXyz* start, cXyz* end);
    void pathMove();
    void modeDeleteInit();
    BOOL plFireRepeat();
    void modeRangeA();
    void modeRangeB();
    void modeRangeC();
    void modeRangeD();
    void modeDamage();
    BOOL _pathMove(cXyz* position, cXyz* start, cXyz* end);
    void _nodeControl(void* node, void* model);
    void setCollision();
    void setWave();
    s32 _create();
    s32 _delete();
    void createWave();
    void setMtx();
    void createInit();
    void modeAttackInit();
    void attackCannon(s32 slot);
    void modeAttack();
    BOOL checkTgHit();
    void modeDelete();
    s32 _execute();

    u8 reserved3AC[0x1C];
    be<s32> mCurrentProc;
    be<u8> mSubMode, mSwitchA, mSwitchB, mPathId, mTriforce, mRandomSalvagePoint, mModelType;
    u8 reserved3D3;
    cXyz mOrigPos;
    u8 reserved3E0[4];
    cXyz mPathP0;
    be<s8> m2D4;
    u8 reserved3F1[3];
    gptr<dPath> mpPath;
    u8 reserved3F8[4];
    cXyz mPathP1;
    u8 reserved408[0xC];
    OshipWaveEcallBack mWaveCallback1, mWaveCallback2;
    OshipSplashEcallBack mSplashCallback;
    OshipTrackEcallBack mTrackCallback;
    dPa_followEcallBack mSmokeFollowCallback[3];
    cXyz mWavePos;
    csXyz mWaveRot;
    u8 reserved596[2];
    be<f32> mSplashScaleTimer;
    cXyz mTrackPos;
    csXyz mSmokeRot[3];
    be<s16> mSmokeRotY[3];
    be<s32> mSmokePtclCount;
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl[5];
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    dLib_wave_c mWave;
    cXyz mTargetPos, mBombSpawnPos, mSmokePos, mFlagOffset;
    Mtx34 mFlagMtx;
    be<u32> mFlagPcId, mBombPcId[5];
    be<f32> mVelocityFwdTarget;
    be<s32> mAimCounter, mHitTimer, mAttackTimer;
    u8 reservedE94[4];
    be<s32> mPlFireTimer;
    be<s16> mAttackSwayTimer, mAttackSwayAmount, mAimRotY, mAimRotYTarget, mAimRotX, mAimRotXTarget;
    be<u8> mBombAlloc[5];
    u8 reservedEAD[3];
    dBgS_LinChk mLinChk;
};
WWHD_OFFSET(daOship_c, mCurrentProc, 0x3C8);
WWHD_OFFSET(daOship_c, mSubMode, 0x3CC);
WWHD_OFFSET(daOship_c, mModelType, 0x3D2);
WWHD_OFFSET(daOship_c, mOrigPos, 0x3D4);
WWHD_OFFSET(daOship_c, mPathP0, 0x3E4);
WWHD_OFFSET(daOship_c, m2D4, 0x3F0);
WWHD_OFFSET(daOship_c, mpPath, 0x3F4);
WWHD_OFFSET(daOship_c, mPathP1, 0x3FC);
WWHD_OFFSET(daOship_c, mWaveCallback1, 0x414);
WWHD_OFFSET(daOship_c, mWaveCallback2, 0x478);
WWHD_OFFSET(daOship_c, mSplashCallback, 0x4DC);
WWHD_OFFSET(daOship_c, mTrackCallback, 0x4F8);
WWHD_OFFSET(daOship_c, mSmokeFollowCallback, 0x548);
WWHD_OFFSET(daOship_c, mWavePos, 0x584);
WWHD_OFFSET(daOship_c, mWaveRot, 0x590);
WWHD_OFFSET(daOship_c, mSplashScaleTimer, 0x598);
WWHD_OFFSET(daOship_c, mTrackPos, 0x59C);
WWHD_OFFSET(daOship_c, mSmokeRot, 0x5A8);
WWHD_OFFSET(daOship_c, mSmokeRotY, 0x5BA);
WWHD_OFFSET(daOship_c, mSmokePtclCount, 0x5C0);
WWHD_OFFSET(daOship_c, mPhs, 0x5C4);
WWHD_OFFSET(daOship_c, mpModel, 0x5CC);
WWHD_OFFSET(daOship_c, mStts, 0x5D0);
WWHD_OFFSET(daOship_c, mCyl, 0x60C);
WWHD_OFFSET(daOship_c, mAcch, 0xBFC);
WWHD_OFFSET(daOship_c, mAcchCir, 0xDC0);
WWHD_OFFSET(daOship_c, mWave, 0xE00);
WWHD_OFFSET(daOship_c, mTargetPos, 0xE0C);
WWHD_OFFSET(daOship_c, mBombSpawnPos, 0xE18);
WWHD_OFFSET(daOship_c, mSmokePos, 0xE24);
WWHD_OFFSET(daOship_c, mFlagOffset, 0xE30);
WWHD_OFFSET(daOship_c, mFlagMtx, 0xE3C);
WWHD_OFFSET(daOship_c, mFlagPcId, 0xE6C);
WWHD_OFFSET(daOship_c, mBombPcId, 0xE70);
WWHD_OFFSET(daOship_c, mVelocityFwdTarget, 0xE84);
WWHD_OFFSET(daOship_c, mAimCounter, 0xE88);
WWHD_OFFSET(daOship_c, mHitTimer, 0xE8C);
WWHD_OFFSET(daOship_c, mAttackTimer, 0xE90);
WWHD_OFFSET(daOship_c, mPlFireTimer, 0xE98);
WWHD_OFFSET(daOship_c, mAttackSwayTimer, 0xE9C);
WWHD_OFFSET(daOship_c, mAttackSwayAmount, 0xE9E);
WWHD_OFFSET(daOship_c, mAimRotY, 0xEA0);
WWHD_OFFSET(daOship_c, mAimRotYTarget, 0xEA2);
WWHD_OFFSET(daOship_c, mAimRotX, 0xEA4);
WWHD_OFFSET(daOship_c, mAimRotXTarget, 0xEA6);
WWHD_OFFSET(daOship_c, mBombAlloc, 0xEA8);
WWHD_OFFSET(daOship_c, mLinChk, 0xEB0);
WWHD_SIZE(daOship_c, 0xF1C);

struct daOship_HIO_c {
    be<u32> __vtbl;
    be<u8> m04, m05, m06, m07, m08;
    u8 padding09[3];
    be<f32> mAttentionOffsY, mEyeOffsY, mWaveOffsZ, mTrackOffsZ;
    cXyz mBombOffset;
    be<f32> mDistRangeA, mDistRangeB, mDistRangeC;
    be<s16> mAttackDelayA, mAttackDelayB;
    be<f32> mPathSpeed, mSpecScale, mTrackOffsY, mTrackScaleY;
    be<f32> mSplashScaleTimerTarget, mSplashMaxScaleTimer;
    be<f32> mWaveVelFade1, mTrackVel, mWaveVelSpeed, mWaveVelFadeOffs, mWaveMaxVelocity;
    cXyz mWaveCollapsePos0, mWaveCollapsePos1;
    be<f32> mBombSpeed, mBombAcceleration;
    be<s16> mBombNoGravityTime;
    u8 padding86[2];
    be<f32> mBadAimAdjustDistanceStart;
    be<s16> mBadAimMax, field_0x8E, field_0x90, mNode2RotZ;
};
WWHD_SIZE(daOship_HIO_c, 0x94);

// HD: GHS copies vector words without passing through floating-point registers.
template <class T, class U> inline be<T>& OshipBits(be<U>& field) {
    static_assert(sizeof(T) == sizeof(U));
    return reinterpret_cast<be<T>&>(field);
}

s32 OshipTargetAngleY(const cXyz*, const cXyz*);
s32 OshipTargetAngleX(const cXyz*, const cXyz*);
s32 OshipAtan2s(f32, f32);
void OshipAddCalcAngleS2(be<s16>*, s32, s32, s32);
s32 OshipAddCalcAngleS(void*, s32, s32, s32, s32);
s32 OshipDistanceAngleS(s32, s32);
s32 OshipCalcTimer(be<s32>*);
s32 OshipGetReverb(s32);
f32 OshipGetWaterY(const cXyz*, void*);
u32 OshipJointTreeHeader(void*);
u32 OshipJointDescriptor(void*);
f64 OshipSqrt(f64);
f32 OshipSqrtLiveFpr();

WWHD_SIZE(daOship_c::ModeEntry, 20);

// HD: unchecked guest address arithmetic is modulo 2^32, including guest address zero.
template <class T> inline T* OshipGuest(u32 address) {
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}

void* OshipFollowConstruct(void*, u32, u32);
void OshipCollisionObjectConstruct(void*);
void OshipCylinderShapeConstruct(void*);
f32 OshipActorDistanceXZ(fopAc_ac_c*, void*);
void OshipConstructArray(void*, u32, u32, u32);
void OshipStatusConstruct(void*);
void OshipGStatusConstruct(void*);
void OshipAcchConstruct(void*);
void OshipAcchCircleConstruct(void*);
void OshipLineCheckConstruct(void*);
void OshipLineCheckDestroy(void*, u32);
void OshipCircleShapeDestroy(void*, u32);
void OshipAcchDestroy(void*, u32);
void OshipDestroyArray(void*, u32, u32, u32, u32, u32);
void OshipStatusDestroy(void*, u32);
void OshipActorDestroy(void*, u32);
void* OshipGetObjectResource(void*, SafeString*, s32);
void OshipAssert(const char*, u32, const char*);
void* OshipCreateModel(void*, u32, u32);
void OshipSettingTevStruct(void*, u32, cXyz*, dKy_tevstr_c*);
void OshipSetLightTevColorType(void*, void*, dKy_tevstr_c*);
void OshipTexSpecmapST(cXyz*, dKy_tevstr_c*, void*, f32);
void OshipModelEntryDL(void*);
s32 OshipLineCross(void*, void*);
void OshipPathMoveHD(cXyz*, be<s8>*, void*, u32, daOship_c*, f32);
void OshipAddCalcPosXZ2(cXyz*, cXyz*, f32, f32);
void OshipCreateDisappear(fopAc_ac_c*, cXyz*, u32, u32, u32);
void OshipRegisterGlobalObject(void*);
void OshipInitializeMath(void*);
void OshipInitializeMatrix(void*);
void OshipVectorSubtract(cXyz*, cXyz*, cXyz*);
s32 OshipVectorNormalizeRS(cXyz*);
void OshipChasePosXZ(cXyz*, cXyz*, f32);
void OshipMatrixCopy(void*, void*);
void OshipRotationConstruct(csXyz*, s32, s32, s32);
void OshipMatrixZrotM(void*, s32);
void OshipMatrixZXYrotM(void*, s32, s32, s32);
void OshipCylinderSetC(void*, cXyz*);
void OshipCylinderSetR(void*, f32);
void OshipCylinderSetH(void*, f32);
void OshipCollisionSet(void*, void*);
s32 OshipResourceLoad(request_of_phase_process_class*, const char*);
s32 OshipIsSwitch(void*, s32, s32);
s32 OshipIsTriforceMap(void*, s32);
s32 OshipEntrySolidHeap(daOship_c*, u32, u32);
void OshipResourceDelete(request_of_phase_process_class*, const char*);
void OshipWaveRemove(void*);
void OshipSplashRemove(void*);
void OshipTrackRemove(void*);
void OshipParticleSet(void*, u32, u32, cXyz*, const void*, const void*, u32, void*, s32,
                      const void*, const void*, const void*);
void OshipWaveRotHD(cXyz*, dLib_wave_c*, f32);
u32 OshipActorAngleY(daOship_c*, void*);
void OshipMatrixTrans(void*, f32, f32, f32);
void OshipMatrixXYZrotM(void*, s32, s32, s32);
void OshipMatrixYrotM(void*, s32);
void OshipMatrixMultVec(void*, cXyz*, cXyz*);
u32 OshipCharacterNameIndex(void*, const char*, s32);
void OshipCylinderSet(void*, void*);
void OshipModelCalc(void*);
void OshipSetCullSizeBox(daOship_c*, f32, f32, f32, f32, f32, f32);
u32 OshipCreateFlag(s32, u32, cXyz*, s32, csXyz*, void*, s32, u32);
u32 OshipGetRoomPath(s32, s32);
void OshipAcchSet(void*, cXyz*, cXyz*, daOship_c*, s32, dBgS_AcchCir*, cXyz*, void*, void*);
u32 OshipBombParameters(u32, u32, u32);
u32 OshipFastCreateBomb(s32, u32, cXyz*, s32, csXyz*, void*, s32, u32, u32);
void OshipBombSetNoGravityTime(void*, s32);
void OshipStartBombSound(u32, cXyz*);
u32 OshipGetTargetHitObject(void*);
void OshipOrderOtherEvent(daOship_c*, const char*, u32, u32);
f32 OshipAddCalc(be<f32>*, f32, f32, f32, f32);
void OshipOnSwitch(void*, u32, s32);
u32 OshipActorJudge(u32, void*);
void OshipSalvageOnForOship(void*, daOship_c*);
void OshipStartSalvageSound(u32);
s32 OshipEventEndCheckOld(void*, const char*);
void OshipOnEventBit(void*, u32);
void OshipActorDelete(void*);
s32 OshipCheckCullingBox(void*, f32, f32, f32, f32, f32, f32);

struct OshipJointHeader {
    u8 padding0[8];
    be<u16> mJointNum;
};
struct OshipJointData {
    u8 padding0[4];
    be<u16> mJntNo;
};
struct OshipEmitter {
    u8 padding0[0x28];
    cXyz mLocalDirection;
    u8 padding34[0x220 - 0x34];
    cXyz mGlobalDynamicsScale;
    u8 padding22C[0xC];
    cXyz mGlobalParticleScale;
    u8 padding244[0x10];
    be<u32> mFlags;
};
WWHD_OFFSET(OshipEmitter, mLocalDirection, 0x28);
WWHD_OFFSET(OshipEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(OshipEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(OshipEmitter, mFlags, 0x254);
WWHD_SIZE(OshipEmitter, 0x258);

struct OshipDebugRegisters {
    be<f32> mFloat[30];
    be<s16> mShort[6];
};
struct OshipHeaderStatics {
    be<f32> mNegativePi, mPositivePi;
    be<u8> mMathInitialized, mMatrixInitialized;
    u8 paddingA[2];
    be<u32> mZeroWords[4];
};
struct OshipHeaderRecords {
    be<u32> mWords[9];
};
struct OshipResourceControlSlot {
    be<u32> mpControl;
};
struct OshipCylinderTable {
    be<f32> mValues[5];
};
WWHD_OFFSET(OshipDebugRegisters, mShort, 0x78);
WWHD_SIZE(OshipDebugRegisters, 0x84);
WWHD_SIZE(OshipHeaderStatics, 0x1C);
WWHD_SIZE(OshipHeaderRecords, 0x24);
WWHD_SIZE(OshipResourceControlSlot, 4);
WWHD_SIZE(OshipCylinderTable, 20);

struct OshipCylinderAttr {
    u8 padding0[0x18];
    be<u32> mAabbVtable, mShapeVtable;
};
struct OshipBoatState {
    u8 padding0[0x644];
    be<u32> mStateFlags;
};
struct OshipFlagParent {
    u8 padding0[0x1E34];
    be<u32> mpParentMtx, mpParentPos;
};
WWHD_SIZE(OshipCylinderAttr, 0x20);
WWHD_OFFSET(OshipBoatState, mStateFlags, 0x644);
WWHD_OFFSET(OshipFlagParent, mpParentMtx, 0x1E34);
WWHD_OFFSET(OshipFlagParent, mpParentPos, 0x1E38);
// HD: negative smoke indices and unchecked bomb slots wrap in guest address space.
template <class T, size_t N> inline T* OshipArrayElement(T (&array)[N], s32 index) {
    return OshipGuest<T>(gabi::ea(&array) + u32(index) * u32(sizeof(T)));
}

struct OshipCharacterNameTable {
    u8 opaque;
};
struct OshipModes {
    daOship_c::ModeEntry mEntries[8];
};

enum OshipResourceIndex {
    dRes_INDEX_OSHIP_BDL_VBTSP_e = 3,
    dRes_INDEX_OSHIP_BDL_VBTST_e = 4,
};
enum OshipJointIndex { VBTSP_JNT_HEAD_e = 1, VBTSP_JNT_CANON_e = 2 };
enum OshipProcessName { fpcNm_MAJUU_FLAG_e = 0xAE, fpcNm_SALVAGE_e = 0x191 };
daOship_c* daOship_c__ct(daOship_c*);

BOOL createHeap_CB(daOship_c* ship);
s32 nodeControl_CB(void* node, s32 timing);
BOOL pathMove_CB(cXyz* position, cXyz* start, cXyz* end, daOship_c* ship);
s32 daOshipCreate(daOship_c* ship);
s32 daOshipDelete(daOship_c* ship);
s32 daOshipExecute(daOship_c* ship);
s32 daOshipDraw(daOship_c* ship);
BOOL daOshipIsDelete(daOship_c* ship);
void OshipSafeStringDelete(void* object, u32 flags);
void* OshipVectorCtor(void* object);
void OshipFollowDelete(void* object, u32 flags);
void OshipSafeStringPrepare(void* object);
void* OshipFollowCtor(void* object);
void* OshipCylinderCtor(void* object);
daOship_c* daOship_c__ct(daOship_c* ship);
void daOship_c__dt(daOship_c* ship, u32 flags);
daOship_HIO_c* daOship_HIO_c__ct(daOship_HIO_c* data);
void __sinit_d_a_oship_cpp();
WWHD_OFFSET(daOship_c, mSwitchA, 0x3CD);
WWHD_OFFSET(daOship_c, mSwitchB, 0x3CE);
WWHD_OFFSET(daOship_c, mPathId, 0x3CF);
WWHD_OFFSET(daOship_c, mTriforce, 0x3D0);
WWHD_OFFSET(daOship_c, mRandomSalvagePoint, 0x3D1);
WWHD_SIZE(OshipBoatState, 0x648);
WWHD_SIZE(OshipFlagParent, 0x1E3C);
