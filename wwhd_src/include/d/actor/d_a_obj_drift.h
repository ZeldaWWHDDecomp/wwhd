/* Floating tree platform. HD: moving-background base adds 0x118 to the GameCube member offsets. */
#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "d/d_cc_uty.h"
#include "gc_api/f_op/f_op_actor.h"
namespace daObjDrift {
struct Act_c : dBgS_MoveBgActor {
    enum Mode { MODE_WAIT, MODE_ROT };
    enum Prm_e { PRM_TYPE_W = 3, PRM_TYPE_S = 0 };
    /* 0x3E0 */ request_of_phase_process_class mPhase;
    /* 0x3E8 */ gptr<J3DModel> mModel;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Cyl mCyl;
    /* 0x558 */ be<s32> mType;
    /* 0x55C */ Mtx34 mMtx;
    /* 0x58C */ be<u32> mFlowerPid;
    /* 0x590 */ be<s32> mMode;
    /* 0x594 */ be<f32> mRideYOff, mTiltTargetZ, mTiltTargetX, mTiltVelZ, mTiltVelX, mTiltZ, mTiltX;
    /* 0x5B0 */ be<s16> mWavePhaseX, mWavePhaseY, mWavePhaseZ;
    /* 0x5B6 */ be<u8> mRideFlag, field_0x49F;
    /* 0x5B8 */ be<s16> mTargetAngleY, mAngleChaseY;
    /* 0x5BC */ cXyz mHitDir, field_0x4B0, mPrevPos;
    /* 0x5E0 */ be<f32> field_0x4C8, field_0x4CC, field_0x4D0, field_0x4D4, mRotTimer;
    s32 Mthd_Create();
    BOOL Mthd_Delete();
    BOOL CreateHeap();
    BOOL Create();
    void set_mtx();
    void init_mtx();
    void calc_flower_param(cXyz*, csXyz*);
    void make_flower();
    void set_flower_current();
    void mode_wait_init();
    void mode_wait();
    void mode_rot_init();
    void mode_rot();
    void set_current();
    BOOL Execute(gptr<Mtx34>*);
    BOOL Draw();
    BOOL Delete();
};
WWHD_OFFSET(Act_c, mCyl, 0x428);
WWHD_OFFSET(Act_c, mType, 0x558);
WWHD_OFFSET(Act_c, mMtx, 0x55C);
WWHD_OFFSET(Act_c, mTiltZ, 0x5A8);
WWHD_OFFSET(Act_c, mRotTimer, 0x5F0);
WWHD_SIZE(Act_c, 0x5F4);
} // namespace daObjDrift

using daObjDrift::Act_c;
struct DriftQuaternion {
    be<f32> x, y, z, w;
};
struct DriftFlowerProfiles {
    be<s16> mProfiles[2];
};
struct DriftDrawBuffers {
    be<u32> mOpaque, mTranslucent;
};
struct DriftSinCos {
    be<f32> mSin, mCos;
};
struct DriftSinCosTable {
    DriftSinCos mEntries[8192];
};
struct DriftModeProc {
    gabi::ptmf<void (Act_c::*)()> mModes[2];
};
struct DriftShapeAttr {
    u8 prefix[0x18];
    be<u32> mAabVtable, mShapeVtable;
};
struct DriftBackground {
    u8 prefix[0xB0];
    be<u32> mRideCallback;
};
struct DriftCylinderSource {
    u8 mObjInf[0x30];
    cXyz mCenter;
    be<f32> mRadius, mHeight;
};
struct DriftGameInfo {
    u8 prefix[0x26A4];
    u8 mCcS;
    u8 gap[0x5B2C - 0x26A5];
    gptr<fopAc_ac_c> mpPlayer;
    u8 gap2[0x5CD8 - 0x5B30];
    be<u32> mPlayerStatus;
    u8 gap3[0x5D70 - 0x5CDC];
    be<u32> mOpaqueCurrent, mTranslucentCurrent, mOpaqueNormal, mTranslucentNormal;
};
struct DriftHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mHeaderByte, mHeaderFlag;
    u8 pad[2];
    be<u32> mZero[4];
};
struct DriftDestructorRecords {
    u8 mRecords[3][12];
};
void rideCB(dBgW*, Act_c*, fopAc_ac_c*);
void driftTranslate(Mtx34*, f32, f32, f32);
void driftQuatRotBaseY2(DriftQuaternion*, cXyz*);
void driftQuatMatrix(DriftQuaternion*);
void driftRotateZXY(Mtx34*, s16, s16, s16);
void driftCopyMatrix(const Mtx34*, Mtx34*);
void driftMultVec(const Mtx34*, cXyz*, cXyz*);
void driftMtxToRot(Mtx34*, csXyz*);
void driftDeleteResource(request_of_phase_process_class*, const char*);
void* driftGetResource(void*, SafeString*, s32);
void driftAssert(const char*, s32, const char*);
J3DModel* driftCreateModel(void*, u32, u32);
u32 driftCreateActor(s16, u32, cXyz*, s8, csXyz*, u32, s32, u32);
void* driftGetEnvLight();
void driftSetLight(void*, s32, cXyz*, dKy_tevstr_c*);
void driftApplyLight(void*, J3DModel*, dKy_tevstr_c*);
DriftGameInfo* driftGetGameInfo();
void driftUpdateModel(J3DModel*, u32);
s32 driftGetReverb(s32);
void driftStartSound(u32, cXyz*, u32, s32);
s32 driftCheckTgHit(dCcD_Cyl*);
void driftClearTgHit(dCcD_Cyl*);
f32 driftPlayerDistanceSquared(Act_c*, fopAc_ac_c*);
void driftNormalizeZP(cXyz*, cXyz*);
void driftMoveCylinder(dCcD_Cyl*, cXyz*);
void driftRegisterCylinder(void*, dCcD_Cyl*);
void driftChaseAngle(be<s16>*, s16, s16);
void driftConstructMoveBg(Act_c*);
void driftConstructGObj(dCcD_Cyl*);
void driftConstructCyl(cM3dGCyl*);
s32 driftLoadResource(request_of_phase_process_class*, const char*);
s32 driftCreateMoveBg(Act_c*, const char*, s32, u32, u32);
void driftRegisterGlobal(void*);
void driftInitializeHeaderByte(void*);
void driftInitializeHeaderFlag(void*);
void driftDeleteObject(void*);
void driftDestroyCylinder(dCcD_Cyl*, s32);
void driftDestroyStatus(dCcD_Stts*, s32);
void driftDestroyActor(Act_c*, s32);
void driftSubtract(cXyz*, cXyz*, cXyz*);
s32 driftInverse(Mtx34*, Mtx34*);
void driftMultVecSR(Mtx34*, cXyz*, cXyz*);
f32 driftSquaredMagnitude(cXyz*);
void driftSetCullBox(Act_c*, f32, f32, f32, f32, f32, f32);
void driftInitializeStatus(dCcD_Stts*, s32, s32, Act_c*);
void driftSetCylinder(dCcD_Cyl*, void*);
s32 driftGetWaterY(cXyz*, be<f32>*);
f32 driftRandomSigned(f32);
void driftPosMoveStream(Act_c*, u32, cXyz*, f32, f32);

enum dRes_INDEX_KRYU { dRes_INDEX_KRYU_BDL_RYU_00_e = 4, dRes_INDEX_KRYU_DZB_RYU_00_e = 7 };
static_assert(offsetof(Act_c, mPhase) == 0x3E0);
static_assert(offsetof(Act_c, mModel) == 0x3E8);
static_assert(offsetof(Act_c, mStts) == 0x3EC);
static_assert(offsetof(Act_c, mCyl) == 0x428);
static_assert(offsetof(Act_c, mType) == 0x558);
static_assert(offsetof(Act_c, mMtx) == 0x55C);
static_assert(offsetof(Act_c, mFlowerPid) == 0x58C);
static_assert(offsetof(Act_c, mMode) == 0x590);
static_assert(offsetof(Act_c, mRideYOff) == 0x594);
static_assert(offsetof(Act_c, mTiltTargetZ) == 0x598);
static_assert(offsetof(Act_c, mTiltTargetX) == 0x59C);
static_assert(offsetof(Act_c, mTiltVelZ) == 0x5A0);
static_assert(offsetof(Act_c, mTiltVelX) == 0x5A4);
static_assert(offsetof(Act_c, mTiltZ) == 0x5A8);
static_assert(offsetof(Act_c, mTiltX) == 0x5AC);
static_assert(offsetof(Act_c, mWavePhaseX) == 0x5B0);
static_assert(offsetof(Act_c, mWavePhaseY) == 0x5B2);
static_assert(offsetof(Act_c, mWavePhaseZ) == 0x5B4);
static_assert(offsetof(Act_c, mRideFlag) == 0x5B6);
static_assert(offsetof(Act_c, field_0x49F) == 0x5B7);
static_assert(offsetof(Act_c, mTargetAngleY) == 0x5B8);
static_assert(offsetof(Act_c, mAngleChaseY) == 0x5BA);
static_assert(offsetof(Act_c, mHitDir) == 0x5BC);
static_assert(offsetof(Act_c, field_0x4B0) == 0x5C8);
static_assert(offsetof(Act_c, mPrevPos) == 0x5D4);
static_assert(offsetof(Act_c, field_0x4C8) == 0x5E0);
static_assert(offsetof(Act_c, field_0x4CC) == 0x5E4);
static_assert(offsetof(Act_c, field_0x4D0) == 0x5E8);
static_assert(offsetof(Act_c, field_0x4D4) == 0x5EC);
static_assert(offsetof(Act_c, mRotTimer) == 0x5F0);
static_assert(sizeof(DriftQuaternion) == 16);
static_assert(offsetof(DriftQuaternion, x) == 0);
static_assert(offsetof(DriftQuaternion, y) == 4);
static_assert(offsetof(DriftQuaternion, z) == 8);
static_assert(offsetof(DriftQuaternion, w) == 12);
static_assert(sizeof(DriftFlowerProfiles) == 4);
static_assert(offsetof(DriftFlowerProfiles, mProfiles) == 0);
static_assert(sizeof(DriftDrawBuffers) == 8);
static_assert(offsetof(DriftDrawBuffers, mOpaque) == 0);
static_assert(offsetof(DriftDrawBuffers, mTranslucent) == 4);
static_assert(sizeof(DriftSinCos) == 8);
static_assert(offsetof(DriftSinCos, mSin) == 0);
static_assert(offsetof(DriftSinCos, mCos) == 4);
static_assert(sizeof(DriftSinCosTable) == 65536);
static_assert(offsetof(DriftSinCosTable, mEntries) == 0);
static_assert(sizeof(DriftModeProc) == 16);
static_assert(offsetof(DriftModeProc, mModes) == 0);
static_assert(sizeof(DriftShapeAttr) == 32);
static_assert(offsetof(DriftShapeAttr, mAabVtable) == 24);
static_assert(offsetof(DriftShapeAttr, mShapeVtable) == 28);
static_assert(sizeof(DriftBackground) == 180);
static_assert(offsetof(DriftBackground, mRideCallback) == 176);
static_assert(sizeof(DriftCylinderSource) == 68);
static_assert(offsetof(DriftCylinderSource, mObjInf) == 0);
static_assert(offsetof(DriftCylinderSource, mCenter) == 48);
static_assert(offsetof(DriftCylinderSource, mRadius) == 60);
static_assert(offsetof(DriftCylinderSource, mHeight) == 64);
static_assert(sizeof(DriftGameInfo) == 23936);
static_assert(offsetof(DriftGameInfo, mCcS) == 9892);
static_assert(offsetof(DriftGameInfo, mpPlayer) == 23340);
static_assert(offsetof(DriftGameInfo, mPlayerStatus) == 23768);
static_assert(offsetof(DriftGameInfo, mOpaqueCurrent) == 23920);
static_assert(offsetof(DriftGameInfo, mTranslucentCurrent) == 23924);
static_assert(offsetof(DriftGameInfo, mOpaqueNormal) == 23928);
static_assert(offsetof(DriftGameInfo, mTranslucentNormal) == 23932);
static_assert(sizeof(DriftHeaderStatics) == 28);
static_assert(offsetof(DriftHeaderStatics, mMinAngle) == 0);
static_assert(offsetof(DriftHeaderStatics, mMaxAngle) == 4);
static_assert(offsetof(DriftHeaderStatics, mHeaderByte) == 8);
static_assert(offsetof(DriftHeaderStatics, mHeaderFlag) == 9);
static_assert(offsetof(DriftHeaderStatics, mZero) == 12);
static_assert(sizeof(DriftDestructorRecords) == 36);
static_assert(offsetof(DriftDestructorRecords, mRecords) == 0);
