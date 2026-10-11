/** Balance lift layout.
 * HD: tww leaves this actor opaque. These private views name the verified HD fields;
 * collision objects retain the shared HD layouts and the chain packet retains its guest vtable.
 */
#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "d/d_cc_uty.h"
#include "gc_api/f_op/f_op_actor.h"
// HD: process IDs differ from the GameCube profile table.
enum BalanceProcess {
    BALANCE_LIFT = 0x6F,
    BALANCE_AM2 = 0xCB,
    BALANCE_NPC_OS = 0x13A,
    BALANCE_NPC_CB1 = 0x14E,
    BALANCE_STONE = 0x1CA,
};
enum Hten1Resource { HTEN1_BDL_PLATFORM = 4, HTEN1_DZB_PLATFORM = 7 };
enum BalanceAttack { BALANCE_AT_SWORD = 2 };
struct BalanceQuat {
    be<f32> x, y, z, w;
};
struct BalancePathPoint {
    u8 mArgs[4];
    cXyz mPos;
};
struct BalancePath {
    be<u16> mCount;
    u8 pad[6];
    gptr<BalancePathPoint> mpPoints;
};
struct BalanceBackground {
    u8 prefix[0xA8];
    be<u32> mCorrection;
    u8 gap[4];
    be<u32> mRideCallback;
};
struct BalanceLineVtable {
    u8 prefix[12];
    gfn mDestructor;
};
struct BalanceChainPacket {
    u8 prefix[12];
    gptr<BalanceLineVtable> mpVtable;
    u8 gap[0xAC - 16];
    gptr<cXyz> mpVertices;
    void destroy(s32 flags) { gabi::call_ptr(mpVtable->mDestructor.get(), this, flags); }
};
struct BalanceShapeAttr {
    u8 prefix[0x18];
    be<u32> mAabVtable, mShapeVtable;
};
struct BalanceAnimation {
    u8 mData[0x148];
};
struct BalancePlayerVtable {
    u8 prefix[0xBC];
    gfn mGetCarryActorID;
};
struct BalancePlayer : fopAc_ac_c {
    s32 getCarryActorID() {
        auto* vt = gabi::at<BalancePlayerVtable>(__vtbl);
        return gabi::call_ptr<s32>(vt->mGetCarryActorID.get(), this);
    }
};
struct daBalancelift_c : fopAc_ac_c {
    /* 0x3AC */ cXyz mAnchorTop;
    /* 0x3B8 */ cXyz mChainPosition;
    /* 0x3C4 */ cXyz mTargetChainPosition;
    /* 0x3D0 */ cXyz mChainVelocity;
    /* 0x3DC */ cXyz mChainTop;
    /* 0x3E8 */ BalanceQuat mSupportRotation;
    /* 0x3F8 */ BalanceQuat mPlatformRotation;
    /* 0x408 */ cXyz mPivot;
    /* 0x414 */ cXyz mPivotVelocity;
    /* 0x420 */ cXyz mPlatformCenter;
    /* 0x42C */ be<f32> mChainLength;
    /* 0x430 */ be<s16> mWeight;
    u8 pad432[2];
    /* 0x434 */ gptr<be<s16>> mSharedWeight;
    /* 0x438 */ be<s16> mIsSecond;
    /* 0x43A */ be<s16> mWeightFlags;
    /* 0x43C */ gptr<be<s16>> mSharedFlags;
    /* 0x440 */ request_of_phase_process_class mPhase;
    /* 0x448 */ gptr<J3DModel> mModel;
    /* 0x44C */ Mtx34 mBackgroundMatrix;
    /* 0x47C */ gptr<BalancePath> mPath;
    /* 0x480 */ gptr<BalanceBackground> mBackground;
    /* 0x484 */ be<f32> mVerticalSpeed;
    u8 pad488[0x30];
    /* 0x4B8 */ BalanceAnimation mAnimation;
    /* 0x600 */ gptr<BalanceChainPacket> mChainPacket;
    /* 0x604 */ dCcD_Stts mCollisionStatus;
    /* 0x640 */ dCcD_Cyl mCylinder;
    BOOL CreateHeap();
    void set_mtx();
    s32 CreateInit();
    void calc_weight();
    void calc_quat();
};

struct daBalancelift_HIO_c {
    /* 0x000 */ be<s8> mChild;
    u8 pad[3];
    /* 0x004 */ be<f32> mRotationMax;
    /* 0x008 */ be<f32> mPivotStiffness;
    /* 0x00C */ be<f32> mRideForce;
    /* 0x010 */ be<f32> mPivotDamping;
    /* 0x014 */ be<f32> mImpactForce;
    /* 0x018 */ be<f32> mChainDamping;
    /* 0x01C */ be<f32> mChainForce;
    /* 0x020 */ be<f32> mLengthRange;
    /* 0x024 */ be<f32> mChainLength;
    /* 0x028 */ be<f32> mScaleX;
    /* 0x02C */ be<f32> mScaleY;
    /* 0x030 */ be<f32> mScaleZ;
    /* 0x034 */ be<u32> __vtbl;
};
struct BalanceDebugTuning {
    be<f32> mBackgroundScaleY, mBackgroundOffsetY;
};
struct BalanceRotationBlend {
    be<f32> mSupport, mPlatform;
};
struct BalanceHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mHeaderByte, mHeaderFlag;
    u8 pad[2];
};
struct BalanceWordBlock {
    be<u32> mWords[4];
};
struct BalanceDestructorRecords {
    u8 mRecords[3][12];
};
struct BalanceCylinderSource {
    u8 mObjInf[0x30];
    cXyz mCenter;
    be<f32> mRadius, mHeight;
};
struct BalanceGameInfo {
    u8 prefix[0x12A0];
    u8 mBgS;
    u8 gap[0x26A4 - 0x12A1];
    u8 mCcS;
    u8 gap2[0x5AB0 - 0x26A5];
    gptr<void> mpParticles;
};
s32 balanceIsActor(fopAc_ac_c*);
fopAc_ac_c* balanceFindActor(u32, be<s32>*);
void balanceSubtract(const cXyz*, cXyz*, const cXyz*);
void balanceScaleVector(const cXyz*, cXyz*, f32);
void balanceAddVector(const cXyz*, const cXyz*, cXyz*);
void* balanceGetResource(void*, SafeString*, s32);
void balanceAssert(const char*, s32, const char*);
J3DModel* balanceCreateModel(void*, u32, u32);
BalanceChainPacket* balanceCreateChain(s32, dKy_tevstr_c*, f32);
BalanceBackground* balanceCreateBackground(u32);
void balanceSetBackground(BalanceBackground*, void*, u32, Mtx34*);
void balanceTranslate(Mtx34*, f32, f32, f32);
void balanceQuatMatrix(BalanceQuat*);
void balanceTranslateM(f32, f32, f32);
void balanceMultVec(Mtx34*, cXyz*, cXyz*);
void balanceScaleMatrix(f32, f32, f32);
void balanceCopyMatrix(Mtx34*, Mtx34*);
BalancePath* balanceGetPath(u32, s8);
u32 balanceCreateChild(s16, u32, s32, cXyz*, s8, u32, u32, s32, u32);
void balanceRegisterBackground(void*, BalanceBackground*, daBalancelift_c*);
void balanceInitializeStatus(dCcD_Stts*, s32, s32, daBalancelift_c*);
void balanceSetCylinder(dCcD_Cyl*, void*);
s32 balanceCreateHioChild(const char*, daBalancelift_HIO_c*);
void balanceConstructActor(daBalancelift_c*);
void balanceConstructAnimation(BalanceAnimation*);
void balanceConstructGObj(dCcD_Cyl*);
void balanceConstructCyl(cM3dGCyl*);
s32 balanceLoadResource(request_of_phase_process_class*, const char*);
s32 balanceEntryHeap(daBalancelift_c*, u32, u32);
void balanceSetCullBox(daBalancelift_c*, f32, f32, f32, f32, f32, f32);
void balanceReleaseBackground(void*, BalanceBackground*);
void balanceDeleteResource(request_of_phase_process_class*, const char*);
void balanceDeleteHioChild(s32);
f32 balanceSquaredMagnitude(cXyz*);
f32 balanceSqrt(f32);
s32 balanceGetReverb(s32);
void balanceStartSound(u32, cXyz*, u32, s32);
void balanceSubtractVector(const cXyz*, const cXyz*, cXyz*);
void balanceQuatRotBaseY2(BalanceQuat*, cXyz*);
void balanceQuatNormalize(BalanceQuat*, BalanceQuat*);
void balanceQuatSlerp(BalanceQuat*, BalanceQuat*, BalanceQuat*, f32);
void balanceMoveBackground(BalanceBackground*);
void balanceMoveStatus(be<u32>*);
s32 balanceCheckTgHit(dCcD_Cyl*);
fopAc_ac_c* balanceGetHitActor(dCcD_GObjTg*);
dCcD_GObjInf* balanceGetHitObject(dCcD_Cyl*);
s32 balanceNormalizeVector(cXyz*);
void balanceScaleTo(cXyz*, cXyz*, f32);
void balanceSetCylCenter(cM3dGCyl*, cXyz*);
BalanceGameInfo* balanceGetGameInfo();
void balanceRegisterCylinder(void*, dCcD_Cyl*);
void balanceSpawnParticle(void*, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32);
void* balanceGetEnvironment();
void balanceSetLight(void*, s32, cXyz*, dKy_tevstr_c*);
void balanceApplyLight(void*, J3DModel*, dKy_tevstr_c*);
void balanceUpdateModel(J3DModel*, u32);
void balanceEntryChain(void*, BalanceChainPacket*, u32);
void balanceDrawChain(BalanceChainPacket*);
daBalancelift_HIO_c* balanceAllocateHio(u32);
void balanceRegisterGlobal(void*);
void balanceInitializeHeaderByte(void*);
void balanceInitializeHeaderFlag(void*);
void balanceDeleteObject(void*);
void balanceDestroyCylinder(dCcD_Cyl*, s32);
void balanceDestroyStatus(dCcD_Stts*, s32);
void balanceDestroyAnimation(BalanceAnimation*, s32);
void balanceDestroyActor(daBalancelift_c*, s32);
daBalancelift_c* balanceFindLift(u32, be<u32>*);

WWHD_OFFSET(BalanceQuat, x, 0x0);
WWHD_OFFSET(BalanceQuat, y, 0x4);
WWHD_OFFSET(BalanceQuat, z, 0x8);
WWHD_OFFSET(BalanceQuat, w, 0xC);
WWHD_SIZE(BalanceQuat, 0x10);

WWHD_OFFSET(BalancePathPoint, mArgs, 0x0);
WWHD_OFFSET(BalancePathPoint, mPos, 0x4);
WWHD_SIZE(BalancePathPoint, 0x10);

WWHD_OFFSET(BalancePath, mCount, 0x0);
WWHD_OFFSET(BalancePath, mpPoints, 0x8);
WWHD_SIZE(BalancePath, 0xC);

WWHD_OFFSET(BalanceBackground, mCorrection, 0xA8);
WWHD_OFFSET(BalanceBackground, mRideCallback, 0xB0);
WWHD_SIZE(BalanceBackground, 0xB4);

WWHD_OFFSET(BalanceLineVtable, mDestructor, 0xC);
WWHD_SIZE(BalanceLineVtable, 0x10);

WWHD_OFFSET(BalanceChainPacket, mpVtable, 0xC);
WWHD_OFFSET(BalanceChainPacket, mpVertices, 0xAC);
WWHD_SIZE(BalanceChainPacket, 0xB0);

WWHD_OFFSET(BalanceShapeAttr, mAabVtable, 0x18);
WWHD_OFFSET(BalanceShapeAttr, mShapeVtable, 0x1C);
WWHD_SIZE(BalanceShapeAttr, 0x20);

WWHD_OFFSET(BalanceAnimation, mData, 0x0);
WWHD_SIZE(BalanceAnimation, 0x148);

WWHD_OFFSET(BalancePlayerVtable, mGetCarryActorID, 0xBC);
WWHD_SIZE(BalancePlayerVtable, 0xC0);

WWHD_SIZE(BalancePlayer, 0x3AC);

WWHD_OFFSET(daBalancelift_c, mAnchorTop, 0x3AC);
WWHD_OFFSET(daBalancelift_c, mChainPosition, 0x3B8);
WWHD_OFFSET(daBalancelift_c, mTargetChainPosition, 0x3C4);
WWHD_OFFSET(daBalancelift_c, mChainVelocity, 0x3D0);
WWHD_OFFSET(daBalancelift_c, mChainTop, 0x3DC);
WWHD_OFFSET(daBalancelift_c, mSupportRotation, 0x3E8);
WWHD_OFFSET(daBalancelift_c, mPlatformRotation, 0x3F8);
WWHD_OFFSET(daBalancelift_c, mPivot, 0x408);
WWHD_OFFSET(daBalancelift_c, mPivotVelocity, 0x414);
WWHD_OFFSET(daBalancelift_c, mPlatformCenter, 0x420);
WWHD_OFFSET(daBalancelift_c, mChainLength, 0x42C);
WWHD_OFFSET(daBalancelift_c, mWeight, 0x430);
WWHD_OFFSET(daBalancelift_c, mSharedWeight, 0x434);
WWHD_OFFSET(daBalancelift_c, mIsSecond, 0x438);
WWHD_OFFSET(daBalancelift_c, mWeightFlags, 0x43A);
WWHD_OFFSET(daBalancelift_c, mSharedFlags, 0x43C);
WWHD_OFFSET(daBalancelift_c, mPhase, 0x440);
WWHD_OFFSET(daBalancelift_c, mModel, 0x448);
WWHD_OFFSET(daBalancelift_c, mBackgroundMatrix, 0x44C);
WWHD_OFFSET(daBalancelift_c, mPath, 0x47C);
WWHD_OFFSET(daBalancelift_c, mBackground, 0x480);
WWHD_OFFSET(daBalancelift_c, mVerticalSpeed, 0x484);
WWHD_OFFSET(daBalancelift_c, mAnimation, 0x4B8);
WWHD_OFFSET(daBalancelift_c, mChainPacket, 0x600);
WWHD_OFFSET(daBalancelift_c, mCollisionStatus, 0x604);
WWHD_OFFSET(daBalancelift_c, mCylinder, 0x640);
WWHD_SIZE(daBalancelift_c, 0x770);

WWHD_OFFSET(daBalancelift_HIO_c, mChild, 0x0);
WWHD_OFFSET(daBalancelift_HIO_c, mRotationMax, 0x4);
WWHD_OFFSET(daBalancelift_HIO_c, mPivotStiffness, 0x8);
WWHD_OFFSET(daBalancelift_HIO_c, mRideForce, 0xC);
WWHD_OFFSET(daBalancelift_HIO_c, mPivotDamping, 0x10);
WWHD_OFFSET(daBalancelift_HIO_c, mImpactForce, 0x14);
WWHD_OFFSET(daBalancelift_HIO_c, mChainDamping, 0x18);
WWHD_OFFSET(daBalancelift_HIO_c, mChainForce, 0x1C);
WWHD_OFFSET(daBalancelift_HIO_c, mLengthRange, 0x20);
WWHD_OFFSET(daBalancelift_HIO_c, mChainLength, 0x24);
WWHD_OFFSET(daBalancelift_HIO_c, mScaleX, 0x28);
WWHD_OFFSET(daBalancelift_HIO_c, mScaleY, 0x2C);
WWHD_OFFSET(daBalancelift_HIO_c, mScaleZ, 0x30);
WWHD_OFFSET(daBalancelift_HIO_c, __vtbl, 0x34);
WWHD_SIZE(daBalancelift_HIO_c, 0x38);

WWHD_OFFSET(BalanceDebugTuning, mBackgroundScaleY, 0x0);
WWHD_OFFSET(BalanceDebugTuning, mBackgroundOffsetY, 0x4);
WWHD_SIZE(BalanceDebugTuning, 0x8);

WWHD_OFFSET(BalanceRotationBlend, mSupport, 0x0);
WWHD_OFFSET(BalanceRotationBlend, mPlatform, 0x4);
WWHD_SIZE(BalanceRotationBlend, 0x8);

WWHD_OFFSET(BalanceHeaderStatics, mMinAngle, 0x0);
WWHD_OFFSET(BalanceHeaderStatics, mMaxAngle, 0x4);
WWHD_OFFSET(BalanceHeaderStatics, mHeaderByte, 0x8);
WWHD_OFFSET(BalanceHeaderStatics, mHeaderFlag, 0x9);
WWHD_SIZE(BalanceHeaderStatics, 0xC);

WWHD_OFFSET(BalanceWordBlock, mWords, 0x0);
WWHD_SIZE(BalanceWordBlock, 0x10);

WWHD_OFFSET(BalanceDestructorRecords, mRecords, 0x0);
WWHD_SIZE(BalanceDestructorRecords, 0x24);

WWHD_OFFSET(BalanceCylinderSource, mObjInf, 0x0);
WWHD_OFFSET(BalanceCylinderSource, mCenter, 0x30);
WWHD_OFFSET(BalanceCylinderSource, mRadius, 0x3C);
WWHD_OFFSET(BalanceCylinderSource, mHeight, 0x40);
WWHD_SIZE(BalanceCylinderSource, 0x44);

WWHD_OFFSET(BalanceGameInfo, mBgS, 0x12A0);
WWHD_OFFSET(BalanceGameInfo, mCcS, 0x26A4);
WWHD_OFFSET(BalanceGameInfo, mpParticles, 0x5AB0);
WWHD_SIZE(BalanceGameInfo, 0x5AB4);
