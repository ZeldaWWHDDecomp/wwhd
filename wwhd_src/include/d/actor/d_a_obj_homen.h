/* Wind Temple stone Hookshot target.
 * GC -> HD: unused enemy-base prefix makes actor members +0x118; collision status0x3C,
 * sphere0x12C and capsule0x138 strides are checked below. HD GHS temporaries retain linkage. */
#pragma once
#include "f_op/f_op_actor.h"
struct HomenStatus {
    u8 mPadding0[0x18];
    /* 0x18 */ be<u32> mBaseVtable;
    /* 0x1C */ be<u32> mGameVtable;
    u8 mPadding20[0x1C];
};
WWHD_SIZE(HomenStatus, 0x3C);
WWHD_OFFSET(HomenStatus, mBaseVtable, 0x18);
WWHD_OFFSET(HomenStatus, mGameVtable, 0x1C);

struct HomenSphere {
    u8 mPadding0[0x3C];
    /* 0x3C */ be<u32> mObjectVtable;
    u8 mPadding40[0x4];
    /* 0x44 */ be<u32> mStatus;
    u8 mPadding48[0x4C];
    /* 0x94 */ be<u32> mTargetFlags;
    u8 mPadding98[0x1C];
    /* 0xB4 */ be<u32> mTargetVector[3];
    u8 mPaddingC0[0x58];
    /* 0x118 */ u8 mGeometry[0x14];
};
WWHD_SIZE(HomenSphere, 0x12C);
WWHD_OFFSET(HomenSphere, mObjectVtable, 0x3C);
WWHD_OFFSET(HomenSphere, mStatus, 0x44);
WWHD_OFFSET(HomenSphere, mTargetFlags, 0x94);
WWHD_OFFSET(HomenSphere, mTargetVector, 0xB4);
WWHD_OFFSET(HomenSphere, mGeometry, 0x118);

struct HomenCapsule {
    u8 mPadding0[0x3C];
    /* 0x3C */ be<u32> mObjectVtable;
    u8 mPadding40[0x4];
    /* 0x44 */ be<u32> mStatus;
    u8 mPadding48[0xC8];
    /* 0x110 */ be<u32> mCapsuleVtable;
    /* 0x114 */ be<u32> mGeometryVtable;
    /* 0x118 */ u8 mGeometry[0x18];
    /* 0x130 */ be<u32> mShapeVtable;
    u8 mPadding134[0x4];
};
WWHD_SIZE(HomenCapsule, 0x138);
WWHD_OFFSET(HomenCapsule, mObjectVtable, 0x3C);
WWHD_OFFSET(HomenCapsule, mStatus, 0x44);
WWHD_OFFSET(HomenCapsule, mCapsuleVtable, 0x110);
WWHD_OFFSET(HomenCapsule, mGeometryVtable, 0x114);
WWHD_OFFSET(HomenCapsule, mGeometry, 0x118);
WWHD_OFFSET(HomenCapsule, mShapeVtable, 0x130);

struct HomenCapsuleShape {
    /* 0x0 */ be<u32> mStart[3];
    /* 0xC */ be<u32> mEnd[3];
    /* 0x18 */ be<f32> mRadius;
};
WWHD_SIZE(HomenCapsuleShape, 0x1C);
WWHD_OFFSET(HomenCapsuleShape, mStart, 0x0);
WWHD_OFFSET(HomenCapsuleShape, mEnd, 0xC);
WWHD_OFFSET(HomenCapsuleShape, mRadius, 0x18);

struct HomenGroundCheck {
    /* 0x0 */ be<u32> mPolyPass;
    /* 0x4 */ be<u32> mGroupPass;
    /* 0x8 */ be<u32> mActorID;
    u8 mPaddingC[0x4];
    /* 0x10 */ be<u32> mBaseVtable;
    u8 mPadding14[0xC];
    /* 0x20 */ be<u32> mVtable;
    /* 0x24 */ be<f32> mPosX;
    /* 0x28 */ be<f32> mPosY;
    /* 0x2C */ be<f32> mPosZ;
    u8 mPadding30[0x10];
    /* 0x40 */ be<u32> mPolyVtable;
    /* 0x44 */ be<u8> mObject;
    /* 0x45 */ be<u8> mCamera;
    /* 0x46 */ be<u8> mLink;
    /* 0x47 */ be<u8> mArrow;
    /* 0x48 */ be<u8> mBomb;
    /* 0x49 */ be<u8> mBoomerang;
    /* 0x4A */ be<u8> mRope;
    u8 mPadding4B[0x1];
    /* 0x4C */ be<u32> mGroupVtable;
    /* 0x50 */ be<u32> mGroupMask;
};
WWHD_SIZE(HomenGroundCheck, 0x54);
WWHD_OFFSET(HomenGroundCheck, mPolyPass, 0x0);
WWHD_OFFSET(HomenGroundCheck, mGroupPass, 0x4);
WWHD_OFFSET(HomenGroundCheck, mActorID, 0x8);
WWHD_OFFSET(HomenGroundCheck, mBaseVtable, 0x10);
WWHD_OFFSET(HomenGroundCheck, mVtable, 0x20);
WWHD_OFFSET(HomenGroundCheck, mPosX, 0x24);
WWHD_OFFSET(HomenGroundCheck, mPosY, 0x28);
WWHD_OFFSET(HomenGroundCheck, mPosZ, 0x2C);
WWHD_OFFSET(HomenGroundCheck, mPolyVtable, 0x40);
WWHD_OFFSET(HomenGroundCheck, mObject, 0x44);
WWHD_OFFSET(HomenGroundCheck, mCamera, 0x45);
WWHD_OFFSET(HomenGroundCheck, mLink, 0x46);
WWHD_OFFSET(HomenGroundCheck, mArrow, 0x47);
WWHD_OFFSET(HomenGroundCheck, mBomb, 0x48);
WWHD_OFFSET(HomenGroundCheck, mBoomerang, 0x49);
WWHD_OFFSET(HomenGroundCheck, mRope, 0x4A);
WWHD_OFFSET(HomenGroundCheck, mGroupVtable, 0x4C);
WWHD_OFFSET(HomenGroundCheck, mGroupMask, 0x50);

struct HomenModel {
    u8 mPadding0[0xBC];
    /* 0xBC */ be<u32> mScaleBits[3];
    /* 0xC8 */ be<f32> mBaseMtx[12];
};
WWHD_SIZE(HomenModel, 0xF8);
WWHD_OFFSET(HomenModel, mScaleBits, 0xBC);
WWHD_OFFSET(HomenModel, mBaseMtx, 0xC8);

struct HomenBgW {
    /* 0x0 */ be<u32> mUseFlags;
    u8 mPadding4[0xA4];
    /* 0xA8 */ be<u32> mCorrection;
};
WWHD_SIZE(HomenBgW, 0xAC);
WWHD_OFFSET(HomenBgW, mUseFlags, 0x0);
WWHD_OFFSET(HomenBgW, mCorrection, 0xA8);

struct HomenPlay {
    u8 mPadding0[0x12A0];
    /* 0x12A0 */ u8 mBgS[4];
    u8 mPadding12A4[0x1400];
    /* 0x26A4 */ u8 mCcS[4];
    u8 mPadding26A8[0x2C10];
    /* 0x52B8 */ be<u16> mEventFlags;
    u8 mPadding52BA[0xA];
    /* 0x52C4 */ u8 mEventManager[4];
    u8 mPadding52C8[0x6D4];
    /* 0x599C */ u8 mVibration[4];
    u8 mPadding59A0[0x18C];
    /* 0x5B2C */ be<u32> mpPlayer;
};
WWHD_SIZE(HomenPlay, 0x5B30);
WWHD_OFFSET(HomenPlay, mBgS, 0x12A0);
WWHD_OFFSET(HomenPlay, mCcS, 0x26A4);
WWHD_OFFSET(HomenPlay, mEventFlags, 0x52B8);
WWHD_OFFSET(HomenPlay, mEventManager, 0x52C4);
WWHD_OFFSET(HomenPlay, mVibration, 0x599C);
WWHD_OFFSET(HomenPlay, mpPlayer, 0x5B2C);

struct HomenPlayer {
    u8 mPadding0[0xB4];
    /* 0xB4 */ be<u32> mVtable;
    u8 mPaddingB8[0x272];
    /* 0x32A */ be<s16> mShapeYaw;
    u8 mPadding32C[0x8C];
    /* 0x3B8 */ be<u32> mEquipmentFlags;
};
WWHD_SIZE(HomenPlayer, 0x3BC);
WWHD_OFFSET(HomenPlayer, mVtable, 0xB4);
WWHD_OFFSET(HomenPlayer, mShapeYaw, 0x32A);
WWHD_OFFSET(HomenPlayer, mEquipmentFlags, 0x3B8);

struct HomenPlayerVtable {
    u8 mPadding0[0x10C];
    /* 0x10C */ be<u32> mSetHookshotCarryOffset;
};
WWHD_SIZE(HomenPlayerVtable, 0x110);
WWHD_OFFSET(HomenPlayerVtable, mSetHookshotCarryOffset, 0x10C);

struct HomenSave {
    u8 mPadding0[0x20];
    /* 0x20 */ u8 mZone[4];
    u8 mPadding24[0x620];
    /* 0x644 */ u8 mEvent[4];
};
WWHD_SIZE(HomenSave, 0x648);
WWHD_OFFSET(HomenSave, mZone, 0x20);
WWHD_OFFSET(HomenSave, mEvent, 0x644);

struct HomenAttack {
    u8 mPadding0[0x10];
    /* 0x10 */ be<u32> mType;
};
WWHD_SIZE(HomenAttack, 0x14);
WWHD_OFFSET(HomenAttack, mType, 0x10);

struct HomenEnemyData {
    /* 0x0 */ be<s16> mProcName;
    u8 mPadding2[0x2];
    /* 0x4 */ be<u32> mParameters;
};
WWHD_SIZE(HomenEnemyData, 0x8);
WWHD_OFFSET(HomenEnemyData, mProcName, 0x0);
WWHD_OFFSET(HomenEnemyData, mParameters, 0x4);

struct HomenSafeString {
    /* 0x0 */ be<u32> mText;
    /* 0x4 */ be<u32> mVtable;
};
WWHD_SIZE(HomenSafeString, 0x8);
WWHD_OFFSET(HomenSafeString, mText, 0x0);
WWHD_OFFSET(HomenSafeString, mVtable, 0x4);

struct HomenHeaderDefaults {
    /* 0x0 */ be<u32> mWords[4];
};
WWHD_SIZE(HomenHeaderDefaults, 0x10);
WWHD_OFFSET(HomenHeaderDefaults, mWords, 0x0);
namespace daObjHomen {
struct Act_c : fopAc_ac_c {
    void set_mtx();
    void init_mtx();
    BOOL create_heap();
    BOOL process_init(s16 state);
    BOOL _delete();
    void get_norse_offset(cXyz *out, s32 which);
    void make_item();
    void manage_item_timer();
    void make_enemy();
    void manage_enemy_timer();
    void get_norse_point(cXyz *out, s32 which);
    BOOL process_free_init();
    void process_main();
    BOOL _execute();
    BOOL _draw();
    BOOL _create();
    void adjust_hookshot();
    void exe_event();
    void set_at_collision();
    void set_co_collision();
    void process_free_main();
    BOOL process_wait_falldown_init();
    void process_wait_falldown_main();
    BOOL process_falldown_init();
    void process_falldown_main();
    BOOL process_wait_init();
    void process_wait_main();
    BOOL process_none_init();
    void process_none_main();
    /* 0x3AC */ u8 mEnemyBaseTail[0x1C];
    /* 0x3C8 */ HomenStatus mSphStts;
    /* 0x404 */ HomenSphere mSph;
    /* 0x530 */ HomenStatus mCpsStts[2];
    /* 0x5A8 */ HomenCapsule mCps[2];
    /* 0x818 */ HomenCapsuleShape mCpsS[2];
    /* 0x850 */ request_of_phase_process_class mPhase;
    /* 0x858 */ gptr<void> mpModel;
    /* 0x85C */ be<f32> mMtx[12];
    /* 0x88C */ gptr<void> mpBgW;
    /* 0x890 */ HomenGroundCheck mGndChk;
    /* 0x8E4 */ be<f32> mGroundY;
    /* 0x8E8 */ be<s32> mType;
    /* 0x8EC */ be<s16> spinX, spinZ, yaw, followCount, spinTarget, extra;
    /* 0x8F8 */ be<u32> mHookshotID, mEnemyID, mSmokeID;
    /* 0x904 */ be<s16> pivoted, mItemTimer, mEnemyTimer, mEventIdx, eventOrder, eventState, mState;
    /* 0x912 */ u8 mPadding912[2];
    /* 0x914 */ cXyz target;
};
WWHD_SIZE(Act_c, 0x920);
WWHD_OFFSET(Act_c, mSphStts, 0x3C8);
WWHD_OFFSET(Act_c, mSph, 0x404);
WWHD_OFFSET(Act_c, mCpsStts, 0x530);
WWHD_OFFSET(Act_c, mCps, 0x5A8);
WWHD_OFFSET(Act_c, mCpsS, 0x818);
WWHD_OFFSET(Act_c, mPhase, 0x850);
WWHD_OFFSET(Act_c, mpModel, 0x858);
WWHD_OFFSET(Act_c, mMtx, 0x85C);
WWHD_OFFSET(Act_c, mpBgW, 0x88C);
WWHD_OFFSET(Act_c, mGndChk, 0x890);
WWHD_OFFSET(Act_c, mType, 0x8E8);
WWHD_OFFSET(Act_c, mState, 0x910);
WWHD_OFFSET(Act_c, target, 0x914);
}
using daObjHomen_c = daObjHomen::Act_c;
using HomenArrayConstructor = void* (*)(void*);
using HomenHeapCallback = BOOL (*)(daObjHomen_c*);

BOOL solidHeapCB(daObjHomen_c *a);
BOOL Mthd_Create(daObjHomen_c *a);
BOOL Mthd_Delete(daObjHomen_c *a);
BOOL Mthd_Execute(daObjHomen_c *a);
BOOL Mthd_Draw(daObjHomen_c *a);
BOOL Mthd_IsDelete(daObjHomen_c *a);
u32 PrmAbstract(const daObjHomen_c *a, u32 width, u32 shift);
void __sinit_d_a_obj_homen_cpp();
void SafeString_dtor(void *self, s32 flags);
void* statusConstruct(void *self);
void* capsuleConstruct(void *self);
void SafeString_assureTerminationImpl(void *self);
void actorDestructor(daObjHomen_c *a, s32 flags);

void Homen_bgRelease(void*, void*);
f32 Homen_groundCross(void*, void*);
void Homen_groundDtor(void*, s32);
void Homen_groundCtor(void*);
s32 Homen_bgSet(void*, void*, s32, void*);
void Homen_statusBaseCtor(void*);
void Homen_collisionSet(void*, void*);
void Homen_addCalc2(be<f32>*, f32, f32, f32);
void Homen_addCalcAngleS2(be<s16>*, s16, s32, s32);
void Homen_capsuleGeometryCtor(void*);
void Homen_capsuleSet(void*, void*);
void Homen_sphereSetR(void*, f32);
void Homen_sphereSetC(void*, const cXyz*);
f32 Homen_rndFX(f32);
void Homen_anglesCtor(csXyz*, s32, s32, s32);
void Homen_vectorSubtract(cXyz*, cXyz*, cXyz*);
void Homen_bgRegist(void*, void*, daObjHomen_c*);
u32 Homen_bgNew(void*);
void Homen_bgMove(void*);
void Homen_statusDtor(void*, s32);
void Homen_sphereDtor(void*, s32);
u32 Homen_getTargetHitActor(void*);
void Homen_statusCtor(void*);
void Homen_statusInit(void*, s32, s32, daObjHomen_c*);
void Homen_collisionObjectCtor(void*);
void Homen_clearTargetHit(void*);
s32 Homen_checkTargetHit(void*);
u32 Homen_getTargetHit(void*);
void Homen_setCapsule(void*, void*);
void Homen_sphereCtor(void*);
void Homen_setSphere(void*, void*);
u32 Homen_getPlay();
s32 Homen_resLoad(void*, void*);
void Homen_resDelete(void*, void*);
s32 Homen_getEventIdx(void*, void*, s32);
s32 Homen_eventEndCheck(void*, s16);
u32 Homen_getEnvLight();
void Homen_settingTevStruct(void*, s32, const cXyz*, void*);
void Homen_setLightTevColorType(void*, void*, void*);
void Homen_onEventBit(void*, s32);
void Homen_onSwitch(void*, s32, s32);
s32 Homen_isSwitch(void*, s32, s32);
void Homen_startShock(void*, u32, s32, const cXyz*);
void Homen_actorCtor(daObjHomen_c*);
void Homen_actorDtor(daObjHomen_c*, s32);
u32 Homen_searchActor(void*, void*);
s32 Homen_searchByID(u32, void*);
void Homen_deleteActor(daObjHomen_c*);
u32 Homen_createActor(s16, u32, const cXyz*, s32, void*, void*, s32, void*);
u32 Homen_createChild(s32, u32, u32, const cXyz*, s32, const csXyz*, void*, s32, void*);
s32 Homen_entrySolidHeap(daObjHomen_c*, HomenHeapCallback, s32);
void Homen_setCullBox(daObjHomen_c*, f32, f32, f32, f32, f32, f32);
void Homen_calcSpeed(daObjHomen_c*);
void Homen_posMove(daObjHomen_c*, void*);
void Homen_orderEvent(daObjHomen_c*, s16, s32, s32, s32, s32);
void Homen_createItem(const cXyz*, s32, s32, s32, s32, const csXyz*, s32, const cXyz*);
void Homen_seStartGlobal(s32);
void Homen_seStartAt(s32, const cXyz*);
void Homen_modelUpdateDL(void*, s32);
u32 Homen_modelCreate(void*, s32, u32);
void Homen_YrotS(void*, s16);
void Homen_XrotM(void*, s16);
void Homen_YrotM(void*, s16);
void Homen_ZrotM(void*, s16);
void Homen_transM(f32, f32, f32);
u32 Homen_getObjectRes(void*, void*, u32);
void Homen_assert(void*, s32, void*);
u32 Homen_new(s32);
void Homen_delete(void*);
void Homen_modelCalc(void*);
void Homen_multVec(void*, void*, void*);
void Homen_mtxCopy(void*, void*);
void Homen_transS(void*, f32, f32, f32);
void Homen_headerByteCtor(void*);
void Homen_headerGuardCtor(void*);
void Homen_constructArray(void*, s32, s32, HomenArrayConstructor);
void Homen_destroyArray(void*, s32, s32, void*, s32, s32);
void Homen_registerDestructor(void*);
template<class T> inline T* Homen_guest(u32 address) { return gabi::at<T>(address); }
inline be<u32>& Homen_bits(be<f32>& value) { return *reinterpret_cast<be<u32>*>(&value); }
inline be<u32>& Homen_component(cXyz& v, u32 i) { return reinterpret_cast<be<u32>*>(&v)[i]; }
inline be<f32>& Homen_floatComponent(cXyz& v, u32 i) { return reinterpret_cast<be<f32>*>(&v)[i]; }


namespace daObjHomen {
using HomenInitFunc = BOOL (Act_c::*)();
using HomenMainFunc = void (Act_c::*)();
struct HomenInitEntry {
    gabi::ptmf<HomenInitFunc> proc;
};
struct HomenMainEntry {
    gabi::ptmf<HomenMainFunc> proc;
};
// HD: retain the audited direct-call harness convention while resolving GHS member descriptors.
template<class R> inline R Homen_dispatch(Act_c* actor, const gabi::ptmf<R (Act_c::*)()>& member) {
    s16 slot = member.i, adjust = member.d;
    u32 self = gabi::ea(actor) + (s32)adjust, target;
    if (slot < 0) target = member.f;
    else {
        s16 vptr = reinterpret_cast<const be<s16>*>(&member.f)[1];
        u32 table = *gabi::at<be<u32>>(self + (s32)vptr);
        target = *gabi::at<be<u32>>(table + (s32)slot * 8 + 4);
    }
    return gabi::call<R>(target, gabi::at<Act_c>(self));
}
}

struct HomenWordVector {
    be<u32> x, y, z;
};
WWHD_SIZE(HomenWordVector, 12);
WWHD_OFFSET(HomenWordVector, x, 0);
WWHD_OFFSET(HomenWordVector, y, 4);
WWHD_OFFSET(HomenWordVector, z, 8);
struct HomenCullBox {
    be<f32> mMinX, mMinY, mMinZ, mMaxX, mMaxY, mMaxZ;
};
WWHD_SIZE(HomenCullBox, 24);
WWHD_OFFSET(HomenCullBox, mMinX, 0);
WWHD_OFFSET(HomenCullBox, mMaxZ, 20);
WWHD_OFFSET(daObjHomen::Act_c, mGroundY, 0x8E4);
WWHD_OFFSET(daObjHomen::Act_c, spinX, 0x8EC);
WWHD_OFFSET(daObjHomen::Act_c, spinZ, 0x8EE);
WWHD_OFFSET(daObjHomen::Act_c, yaw, 0x8F0);
WWHD_OFFSET(daObjHomen::Act_c, followCount, 0x8F2);
WWHD_OFFSET(daObjHomen::Act_c, spinTarget, 0x8F4);
WWHD_OFFSET(daObjHomen::Act_c, extra, 0x8F6);
WWHD_OFFSET(daObjHomen::Act_c, mHookshotID, 0x8F8);
WWHD_OFFSET(daObjHomen::Act_c, mEnemyID, 0x8FC);
WWHD_OFFSET(daObjHomen::Act_c, mSmokeID, 0x900);
WWHD_OFFSET(daObjHomen::Act_c, pivoted, 0x904);
WWHD_OFFSET(daObjHomen::Act_c, mItemTimer, 0x906);
WWHD_OFFSET(daObjHomen::Act_c, mEnemyTimer, 0x908);
WWHD_OFFSET(daObjHomen::Act_c, mEventIdx, 0x90A);
WWHD_OFFSET(daObjHomen::Act_c, eventOrder, 0x90C);
WWHD_OFFSET(daObjHomen::Act_c, eventState, 0x90E);

struct HomenTUData {
    u32 mSafeStringVtable;
    u32 mArchiveName;
    u32 mHeapAssertFile;
    u32 mModelAssertText;
    u32 mCollisionAssertText;
    u32 mEnemyAssertFile;
    u32 mEnemyAssertText;
    u32 mActorVtable;
    u32 mStatusBaseVtable;
    u32 mStatusGameVtable;
    u32 mGroundBaseVtable;
    u32 mGroundPolyVtable;
    u32 mGroundVtable;
    u32 mGroundGroupVtable;
    u32 mEventName;
    u32 mGroundDeletingVtable;
    u32 mGroundDeletingPolyVtable;
    u32 mGroundDeletingGroupVtable;
    u32 mGeometryBaseVtable;
    u32 mCapsuleVtable;
    u32 mCollisionObjectVtable;
    u32 mCapsuleShapeVtable;
    u32 mCapsuleGeometryVtable;
    u32 mHeaderDtorLink0;
    u32 mHeaderDtorLink1;
    u32 mHeaderDtorLink2;
    u32 mFindByIDCallback;
    u32 mCapsuleDestructor;
    u32 mStatusDestructor;
};

enum HomenParam : u32 {
    PRM_ARG0_W = 4, PRM_ARG0_S = 8,
    PRM_ITEM_W = 6, PRM_ITEM_S = 12,
    PRM_ITEMBIT_W = 7, PRM_ITEMBIT_S = 0,
    PRM_ENEMY_W = 4, PRM_ENEMY_S = 18,
    PRM_SWITCH_W = 8, PRM_SWITCH_S = 24,
};
enum HomenState : s16 { HOMEN_FREE, HOMEN_WAIT_FALLDOWN, HOMEN_FALLDOWN, HOMEN_WAIT, HOMEN_NONE };
enum : s32 { HOMEN_PHASE_COMPLETE = 4, HOMEN_PHASE_ERROR = 5, HOMEN_SMOKE_PROCESS = 0x82 };
