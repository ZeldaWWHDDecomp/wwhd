/* daSaku_c (wooden barricade), WWHD layout.
 * GameCube -> WWHD actor members +0x11C. HD collision cylinders retain stride0x130,
 * smoke callbacks stride0x20, and model/material resources use the checked private HD views below. */
#pragma once
#include "f_op/f_op_actor.h"

// HD: audited private views preserve HD model, material, callback and collision strides.
struct SakuCylinder {
    u8 mPadding0[0x3C];
    /* 0x3C */ be<u32> mObjectVtable;
    u8 mPadding40[0x4];
    /* 0x44 */ be<u32> mStatus;
    u8 mPadding48[0xC8];
    /* 0x110 */ be<u32> mCylinderVtable;
    /* 0x114 */ be<u32> mGeometryVtable;
    /* 0x118 */ u8 mGeometry[0x14];
    /* 0x12C */ be<u32> mShapeVtable;
};
WWHD_SIZE(SakuCylinder, 0x130);
WWHD_OFFSET(SakuCylinder, mObjectVtable, 0x3C);
WWHD_OFFSET(SakuCylinder, mStatus, 0x44);
WWHD_OFFSET(SakuCylinder, mCylinderVtable, 0x110);
WWHD_OFFSET(SakuCylinder, mGeometryVtable, 0x114);
WWHD_OFFSET(SakuCylinder, mGeometry, 0x118);
WWHD_OFFSET(SakuCylinder, mShapeVtable, 0x12C);

struct SakuSmokeCallback {
    /* 0x0 */ be<u32> mVtable;
    /* 0x4 */ be<u32> mpEmitter;
    u8 mPadding8[0x9];
    /* 0x11 */ be<u8> mRateOff;
    u8 mPadding12[0x4];
    /* 0x16 */ be<u8> mColor[4];
    u8 mPadding1A[0x6];
};
WWHD_SIZE(SakuSmokeCallback, 0x20);
WWHD_OFFSET(SakuSmokeCallback, mVtable, 0x0);
WWHD_OFFSET(SakuSmokeCallback, mpEmitter, 0x4);
WWHD_OFFSET(SakuSmokeCallback, mRateOff, 0x11);
WWHD_OFFSET(SakuSmokeCallback, mColor, 0x16);

struct SakuStatus {
    u8 mPadding0[0x18];
    /* 0x18 */ be<u32> mBaseVtable;
    /* 0x1C */ be<u32> mGameVtable;
    u8 mPadding20[0x1C];
};
WWHD_SIZE(SakuStatus, 0x3C);
WWHD_OFFSET(SakuStatus, mBaseVtable, 0x18);
WWHD_OFFSET(SakuStatus, mGameVtable, 0x1C);

struct SakuHeapView {
    u8 mPadding0[0xC];
    /* 0xC */ be<u32> mVtable;
    /* 0x10 */ be<u32> mName;
};
WWHD_SIZE(SakuHeapView, 0x14);
WWHD_OFFSET(SakuHeapView, mVtable, 0xC);
WWHD_OFFSET(SakuHeapView, mName, 0x10);

struct SakuHeapVtable {
    u8 mPadding0[0x54];
    /* 0x54 */ be<u32> mFreeAll;
};
WWHD_SIZE(SakuHeapVtable, 0x58);
WWHD_OFFSET(SakuHeapVtable, mFreeAll, 0x54);

struct SakuSmokeVtable {
    u8 mPadding0[0x44];
    /* 0x44 */ be<u32> mEnd;
};
WWHD_SIZE(SakuSmokeVtable, 0x48);
WWHD_OFFSET(SakuSmokeVtable, mEnd, 0x44);

struct SakuModelView {
    u8 mPadding0[0xAC];
    /* 0xAC */ be<u32> mpModelData;
    u8 mPaddingB0[0xC];
    /* 0xBC */ be<f32> mScaleX;
    /* 0xC0 */ be<f32> mScaleY;
    /* 0xC4 */ be<f32> mScaleZ;
    /* 0xC8 */ be<f32> mBaseMtx[12];
};
WWHD_SIZE(SakuModelView, 0xF8);
WWHD_OFFSET(SakuModelView, mpModelData, 0xAC);
WWHD_OFFSET(SakuModelView, mScaleX, 0xBC);
WWHD_OFFSET(SakuModelView, mScaleY, 0xC0);
WWHD_OFFSET(SakuModelView, mScaleZ, 0xC4);
WWHD_OFFSET(SakuModelView, mBaseMtx, 0xC8);

struct SakuModelDataView {
    u8 mPadding0[0xC];
    /* 0xC */ be<u32> mMaterialCount;
    /* 0x10 */ be<u32> mpMaterials;
};
WWHD_SIZE(SakuModelDataView, 0x14);
WWHD_OFFSET(SakuModelDataView, mMaterialCount, 0xC);
WWHD_OFFSET(SakuModelDataView, mpMaterials, 0x10);

struct SakuNativeResourceView {
    u8 mPadding0[0x24];
    /* 0x24 */ be<u16> mMaterialCount;
};
WWHD_SIZE(SakuNativeResourceView, 0x26);
WWHD_OFFSET(SakuNativeResourceView, mMaterialCount, 0x24);

struct SakuMaterialView {
    /* 0x0 */ be<u32> mpNativeData;
    u8 mPadding4[0x14];
    /* 0x18 */ be<u32> mpTevBlock;
    u8 mPadding1C[0x84];
    /* 0xA0 */ be<u32> mShaderFlags;
    u8 mPaddingA4[0x2F8];
};
WWHD_SIZE(SakuMaterialView, 0x39C);
WWHD_OFFSET(SakuMaterialView, mpNativeData, 0x0);
WWHD_OFFSET(SakuMaterialView, mpTevBlock, 0x18);
WWHD_OFFSET(SakuMaterialView, mShaderFlags, 0xA0);

struct SakuNativeMaterialView {
    u8 mPadding0[0x20];
    /* 0x20 */ be<s32> mPixelEngineOffset;
};
WWHD_SIZE(SakuNativeMaterialView, 0x24);
WWHD_OFFSET(SakuNativeMaterialView, mPixelEngineOffset, 0x20);

struct SakuPixelEngineView {
    u8 mPadding0[0x8];
    /* 0x8 */ u8 mZMode[0x10];
    /* 0x18 */ u8 mBlend[4];
};
WWHD_SIZE(SakuPixelEngineView, 0x1C);
WWHD_OFFSET(SakuPixelEngineView, mZMode, 0x8);
WWHD_OFFSET(SakuPixelEngineView, mBlend, 0x18);

struct SakuTevBlockView {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u32> mVtable;
};
WWHD_SIZE(SakuTevBlockView, 0x8);
WWHD_OFFSET(SakuTevBlockView, mVtable, 0x4);

struct SakuTevVtable {
    u8 mPadding0[0x3C];
    /* 0x3C */ be<u32> mSetKColor;
    u8 mPadding40[0xC];
    /* 0x4C */ be<u32> mGetKColor;
};
WWHD_SIZE(SakuTevVtable, 0x50);
WWHD_OFFSET(SakuTevVtable, mSetKColor, 0x3C);
WWHD_OFFSET(SakuTevVtable, mGetKColor, 0x4C);

struct SakuColor {
    /* 0x0 */ be<u8> r;
    /* 0x1 */ be<u8> g;
    /* 0x2 */ be<u8> b;
    /* 0x3 */ be<u8> a;
};
WWHD_SIZE(SakuColor, 0x4);
WWHD_OFFSET(SakuColor, r, 0x0);
WWHD_OFFSET(SakuColor, g, 0x1);
WWHD_OFFSET(SakuColor, b, 0x2);
WWHD_OFFSET(SakuColor, a, 0x3);

struct SakuFloatColor {
    /* 0x0 */ be<f32> r;
    /* 0x4 */ be<f32> g;
    /* 0x8 */ be<f32> b;
    /* 0xC */ be<f32> a;
};
WWHD_SIZE(SakuFloatColor, 0x10);
WWHD_OFFSET(SakuFloatColor, r, 0x0);
WWHD_OFFSET(SakuFloatColor, g, 0x4);
WWHD_OFFSET(SakuFloatColor, b, 0x8);
WWHD_OFFSET(SakuFloatColor, a, 0xC);

struct SakuAttackView {
    u8 mPadding0[0x10];
    /* 0x10 */ be<u32> mAttackType;
};
WWHD_SIZE(SakuAttackView, 0x14);
WWHD_OFFSET(SakuAttackView, mAttackType, 0x10);

struct SakuEmitterView {
    u8 mPadding0[0x8];
    /* 0x8 */ be<f32> mScaleX;
    /* 0xC */ be<f32> mScaleY;
    /* 0x10 */ be<f32> mScaleZ;
    u8 mPadding14[0x20];
    /* 0x34 */ be<f32> mRate;
    u8 mPadding38[0x24];
    /* 0x5C */ be<u32> mMaxFrame;
    u8 mPadding60[0x1C0];
    /* 0x220 */ be<f32> mDynamicsX;
    /* 0x224 */ be<f32> mDynamicsY;
    /* 0x228 */ be<f32> mDynamicsZ;
    u8 mPadding22C[0xC];
    /* 0x238 */ be<f32> mParticleX;
    /* 0x23C */ be<f32> mParticleY;
    /* 0x240 */ be<f32> mParticleZ;
    u8 mPadding244[0x3];
    /* 0x247 */ be<u8> mGlobalAlpha;
    u8 mPadding248[0xC];
    /* 0x254 */ be<u32> mStatus;
};
WWHD_SIZE(SakuEmitterView, 0x258);
WWHD_OFFSET(SakuEmitterView, mScaleX, 0x8);
WWHD_OFFSET(SakuEmitterView, mScaleY, 0xC);
WWHD_OFFSET(SakuEmitterView, mScaleZ, 0x10);
WWHD_OFFSET(SakuEmitterView, mRate, 0x34);
WWHD_OFFSET(SakuEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(SakuEmitterView, mDynamicsX, 0x220);
WWHD_OFFSET(SakuEmitterView, mDynamicsY, 0x224);
WWHD_OFFSET(SakuEmitterView, mDynamicsZ, 0x228);
WWHD_OFFSET(SakuEmitterView, mParticleX, 0x238);
WWHD_OFFSET(SakuEmitterView, mParticleY, 0x23C);
WWHD_OFFSET(SakuEmitterView, mParticleZ, 0x240);
WWHD_OFFSET(SakuEmitterView, mGlobalAlpha, 0x247);
WWHD_OFFSET(SakuEmitterView, mStatus, 0x254);

struct SakuPlayView {
    u8 mPadding0[0x12A0];
    /* 0x12A0 */ u8 mBgS[4];
    u8 mPadding12A4[0x1400];
    /* 0x26A4 */ u8 mCcS[4];
    u8 mPadding26A8[0x32F4];
    /* 0x599C */ u8 mVibration[4];
    u8 mPadding59A0[0x110];
    /* 0x5AB0 */ be<u32> mpParticle;
    u8 mPadding5AB4[0x2BC];
    /* 0x5D70 */ be<u32> mpBGDraw;
    /* 0x5D74 */ be<u32> mpBGXluDraw;
    /* 0x5D78 */ be<u32> mpDraw;
    /* 0x5D7C */ be<u32> mpXluDraw;
};
WWHD_SIZE(SakuPlayView, 0x5D80);
WWHD_OFFSET(SakuPlayView, mBgS, 0x12A0);
WWHD_OFFSET(SakuPlayView, mCcS, 0x26A4);
WWHD_OFFSET(SakuPlayView, mVibration, 0x599C);
WWHD_OFFSET(SakuPlayView, mpParticle, 0x5AB0);
WWHD_OFFSET(SakuPlayView, mpBGDraw, 0x5D70);
WWHD_OFFSET(SakuPlayView, mpBGXluDraw, 0x5D74);
WWHD_OFFSET(SakuPlayView, mpDraw, 0x5D78);
WWHD_OFFSET(SakuPlayView, mpXluDraw, 0x5D7C);

struct SakuSaveView {
    u8 mPadding0[0x20];
    /* 0x20 */ u8 mZone[4];
};
WWHD_SIZE(SakuSaveView, 0x24);
WWHD_OFFSET(SakuSaveView, mZone, 0x20);

struct sakuHIO_c {
    /* 0x0 */ be<s8> mChild;
    u8 mPadding1[0x1];
    /* 0x2 */ be<s16> mDebugHeight;
    /* 0x4 */ be<s16> mDebugWidth;
    /* 0x6 */ be<s16> mDebugDepth;
    /* 0x8 */ be<s16> mDebugMode;
    /* 0xA */ be<u8> mEnabled;
    /* 0xB */ be<u8> mDebrisEnabled;
    /* 0xC */ be<s16> mAlphaSpeed;
    /* 0xE */ be<u8> mStartAlpha;
    /* 0xF */ be<u8> mDustR;
    /* 0x10 */ be<u8> mDustG;
    /* 0x11 */ be<u8> mDustB;
    /* 0x12 */ be<u8> mDustA;
    u8 mPadding13[0x1];
    /* 0x14 */ be<u32> mVtable;
};
WWHD_SIZE(sakuHIO_c, 0x18);
WWHD_OFFSET(sakuHIO_c, mChild, 0x0);
WWHD_OFFSET(sakuHIO_c, mDebugHeight, 0x2);
WWHD_OFFSET(sakuHIO_c, mDebugWidth, 0x4);
WWHD_OFFSET(sakuHIO_c, mDebugDepth, 0x6);
WWHD_OFFSET(sakuHIO_c, mDebugMode, 0x8);
WWHD_OFFSET(sakuHIO_c, mEnabled, 0xA);
WWHD_OFFSET(sakuHIO_c, mDebrisEnabled, 0xB);
WWHD_OFFSET(sakuHIO_c, mAlphaSpeed, 0xC);
WWHD_OFFSET(sakuHIO_c, mStartAlpha, 0xE);
WWHD_OFFSET(sakuHIO_c, mDustR, 0xF);
WWHD_OFFSET(sakuHIO_c, mDustG, 0x10);
WWHD_OFFSET(sakuHIO_c, mDustB, 0x11);
WWHD_OFFSET(sakuHIO_c, mDustA, 0x12);
WWHD_OFFSET(sakuHIO_c, mVtable, 0x14);

struct SakuHeaderDefaults {
    /* 0x0 */ be<u32> mWord0;
    /* 0x4 */ be<u32> mWord1;
    /* 0x8 */ be<u32> mWord2;
    /* 0xC */ be<u32> mWord3;
};
WWHD_SIZE(SakuHeaderDefaults, 0x10);
WWHD_OFFSET(SakuHeaderDefaults, mWord0, 0x0);
WWHD_OFFSET(SakuHeaderDefaults, mWord1, 0x4);
WWHD_OFFSET(SakuHeaderDefaults, mWord2, 0x8);
WWHD_OFFSET(SakuHeaderDefaults, mWord3, 0xC);

struct daSaku_c : fopAc_ac_c {
    s32 GetDzbId(s32 half);
    s32 CreateHeap(s32 heap, s32 half);
    s32 MoveBGResist(s32 heap, s32 half);
    s32 CreateDummyHeap(s32 half);
    s32 changeCollision(s32 half);
    s32 RecreateHeap(s32 heapId, s32 half);
    s32 mode_break_fire(s32 half);
    s32 mode_break_throw_obj(s32 half);
    void setMoveBGMtx();
    void setMtx();
    void CreateInit();
    s32 broken(s32 half);
    s32 burn();
    void checkCol();
    s32 loadModel(s32 index, s32 heap, s32 half);
    s32 loadMoveBG(s32 index, s32 heap, s32 half);
    void setCol();
    s32 saku_draw_sub(s32 half);
    s32 mode_break_none(s32 half);
    s32 setEffFire(s32 unused);
    s32 setEffBreak(s32 half);

    /* 0x3AC */ SakuSmokeCallback mSmokeCb[2];
    /* 0x3EC */ SakuStatus mStts;
    /* 0x428 */ SakuCylinder targetCylinders[2][3];
    /* 0xB48 */ cXyz mPos[2][3];
    /* 0xB90 */ SakuCylinder fireCylinders[3];
    /* 0xF20 */ request_of_phase_process_class modelPhase;
    /* 0xF28 */ request_of_phase_process_class collisionPhase;
    /* 0xF30 */ be<u32> m_heap[2][2];
    /* 0xF40 */ be<u32> mModels[2][2];
    /* 0xF50 */ be<u32> backgrounds[2][2];
    /* 0xF60 */ be<u32> activeBackgrounds[2];
    /* 0xF68 */ be<f32> mMtx[2][12];
    /* 0xFC8 */ be<u32> mpEmitter[2];
    /* 0xFD0 */ be<f32> smokeAlpha[2];
    /* 0xFD8 */ be<s32> mParticleTimers[2];
    /* 0xFE0 */ cXyz smokePositions[2];
    /* 0xFF8 */ be<u8> modelAlpha[2][2];
    /* 0xFFC */ be<s32> collisionTimers[3];
    /* 0x1008 */ be<s32> fireTimer;
    /* 0x100C */ be<u8> heapReleaseDelay[2];
    /* 0x100E */ be<u8> mType;
    /* 0x100F */ be<u8> mFlag;
    /* 0x1010 */ be<u8> burning;
    /* 0x1011 */ u8 padding[3];
    /* 0x1014 */ be<s32> state[2];
    /* 0x101C */ be<u32> mBottomHalfDestroyedSwitch;
    /* 0x1020 */ be<u32> mTopHalfDestroyedSwitch;
};
WWHD_SIZE(daSaku_c, 0x1024);
WWHD_OFFSET(daSaku_c, mSmokeCb, 0x3AC);
WWHD_OFFSET(daSaku_c, targetCylinders, 0x428);
WWHD_OFFSET(daSaku_c, mPos, 0xB48);
WWHD_OFFSET(daSaku_c, fireCylinders, 0xB90);
WWHD_OFFSET(daSaku_c, m_heap, 0xF30);
WWHD_OFFSET(daSaku_c, mModels, 0xF40);
WWHD_OFFSET(daSaku_c, mMtx, 0xF68);
WWHD_OFFSET(daSaku_c, state, 0x1014);
WWHD_OFFSET(daSaku_c, mTopHalfDestroyedSwitch, 0x1020);

void Saku_bgRelease(void*, void*);
s32 Saku_bgSet(void*, void*, s32, void*);
void Saku_statusBaseCtor(void*);
void Saku_collisionSet(void*, void*);
s32 Saku_chaseUC(be<u8>*, s32, s32);
s32 Saku_chaseF(be<f32>*, f32, f32);
void Saku_cylinderSetC(void*, const cXyz*);
void Saku_cylinderGeometryCtor(void*);
s32 Saku_bgRegist(void*, void*, daSaku_c*);
u32 Saku_bgNew(s32);
void Saku_bgMove(void*);
void Saku_statusDtor(void*, s32);
void Saku_statusCtor(void*);
void Saku_statusInit(void*, s32, s32, daSaku_c*);
void Saku_collisionObjectCtor(void*);
s32 Saku_checkTargetHit(void*);
void* Saku_getTargetHit(void*);
void Saku_setCylinder(void*, const void*);
u32 Saku_getPlay();
s32 Saku_resLoad(request_of_phase_process_class*, void*);
void Saku_resDelete(request_of_phase_process_class*, void*);
u32 Saku_getReverb(s32);
void* Saku_getEnvLight();
void Saku_settingTevStruct(void*, s32, const cXyz*, void*);
void Saku_setLightTevColorType(void*, void*, void*);
u32 Saku_smokeCallbackCtor(void*, s32);
void Saku_particleSet(void*, s32, s32, const cXyz*, const csXyz*, const cXyz*, s32, void*, s32, void*, void*, void*);
void Saku_onSwitch(void*, u32, s32);
s32 Saku_isSwitch(void*, u32, s32);
void Saku_startShock(void*, s32, s32, const cXyz*);
void Saku_actorCtor(daSaku_c*);
void Saku_actorDtor(daSaku_c*, s32);
void Saku_seStart(u32, const cXyz*, s32, u32);
void Saku_modelUpdateDL(void*, s32);
u32 Saku_setCurrentHeap(void*);
u32 Saku_createSolidHeap(s32, s32);
void Saku_adjustSolidHeap(void*);
void Saku_restoreCurrentHeap();
void Saku_destroySolidHeap(void*);
u32 Saku_modelCreate(void*, s32, u32);
s32 Saku_createHIOChild(void*, void*);
void Saku_deleteHIOChild(s32);
void Saku_ZXYrotM(void*, s16, s16, s16);
void Saku_YrotM(void*, s16);
void Saku_scaleM(f32, f32, f32);
void* Saku_getObjectRes(void*, void*, s32);
void Saku_assert(void*, s32, void*);
void* Saku_new(s32);
void Saku_delete(void*);
void Saku_convertColor(void*, void*, f32);
void Saku_setZUpdate(void*, bool);
void Saku_setBlendSrc(void*, s32);
void Saku_setBlendDst(void*, s32);
void Saku_setBlendSrcAlpha(void*, s32);
void Saku_setBlendDstAlpha(void*, s32);
void Saku_setBlendMode(void*, s32);
void* Saku_getNativeModelData(void*);
void Saku_setModelXlu(void*, s32);
void* Saku_getShaderColor(void*, s32);
void Saku_multVec(void*, cXyz*, cXyz*);
void Saku_mtxCopy(void*, void*);
void Saku_transS(void*, f32, f32, f32);
void Saku_headerByteCtor(void*);
void Saku_headerGuardCtor(void*);
void Saku_constructArray(void*, s32, s32, u32);
void Saku_destroyArray(void*, s32, s32, u32, s32, s32);
void Saku_registerDestructor(void*);

// HD: guest address translation for private views; offsets are expressed by their checked fields.
template<class T> inline T* Saku_guest(u32 address) { return gabi::at<T>(address); }

u32 sakuSmokeConstruct(void* callback);
void sakuColorToFloat(void* output, void* color);
s32 daSaku_Draw(daSaku_c* actor);
s32 daSaku_IsDelete(daSaku_c* actor);
void sakuSmokeDestruct(void* callback, s32 flags);
void sakuEmptyVirtual(void* actor);
void* sakuCylinderConstruct(void* object);
void sakuCylinderDestruct(void* object, s32 flags);
void* sakuConstruct(daSaku_c* actor);
void sakuDestruct(daSaku_c* actor, s32 flags);
s32 daSaku_Execute(daSaku_c* actor);
s32 daSaku_Delete(daSaku_c* actor);
s32 matAlphaAnim(void* model, u32 alpha, s32 depthWrite);
s32 daSaku_Create(daSaku_c* actor);
void __sinit_d_a_saku_cpp();
struct SakuActorColorView { u8 mPrefix[0x1A8]; SakuColor mColorK0; };
WWHD_OFFSET(SakuActorColorView, mColorK0, 0x1A8);




// HD: array destruction calls the shared dCcD_Cyl destructor, not this TU deleting wrapper.
enum : u32 { SakuCylinderDestructorAddress = 0x02515A70 };

enum SakuMode : s32 { SAKU_NONE = 0, SAKU_INTACT = 1, SAKU_BURNING = 2, SAKU_BROKEN = 3 };
enum : s32 { SAKU_PHASE_COMPLETE = 4, SAKU_PHASE_ERROR = 5, SAKU_MAX_PARTICLE_TIMER = 2000 };
enum : s32 {
    dRes_INDEX_KSAKU_00_BDL_KSAKU_00_e = 3,
    dRes_INDEX_KSAKU_00_BDL_KSAKU_01_e = 4,
    dRes_INDEX_KSAKU_00_BDL_KSAKU_02_e = 5,
    dRes_INDEX_KSAKU_00_BDL_KSAKU_03_e = 6,
    dRes_INDEX_KSAKU_00_BDL_KSAKU_04_e = 7,
    dRes_INDEX_KSAKU_00_BDL_KSAKU_05_e = 8,
    dRes_INDEX_KNSAK_00_BDL_KNSAK_00_e = 3,
    dRes_INDEX_KNSAK_00_BDL_KNSAK_01_e = 4,
    dRes_INDEX_KNSAK_00_BDL_KNSAK_02_e = 5,
    dRes_INDEX_KNSAK_00_BDL_KNSAK_03_e = 6,
    dRes_INDEX_KNSAK_00_BDL_KNSAK_04_e = 7,
    dRes_INDEX_KNSAK_00_BDL_KNSAK_05_e = 8,
    dRes_INDEX_KSAKUCO_DZB_KSAKU_00_e = 3,
    dRes_INDEX_KSAKUCO_DZB_KSAKU_01_e = 4,
    dRes_INDEX_KSAKUCO_DZB_KSAKU_02_e = 5,
    dRes_INDEX_KSAKUCO_DZB_KSAKU_03_e = 6,
};

struct SakuTUData {
    u32 mCylinderBaseVtable;
    u32 mCylinderVtable;
    u32 mCollisionObjectVtable;
    u32 mCylinderShapeVtable;
    u32 mCylinderGeometryVtable;
    u32 mActorVtable;
    u32 mStatusBaseVtable;
    u32 mStatusGameVtable;
    u32 mHeapAssertFile;
    u32 mHeapAssertText;
    u32 mSafeStringVtable;
    u32 mModelAssertFile;
    u32 mModelAssertText;
    u32 mCylinderSource;
    u32 mMaterialAssertFile;
    u32 mMaterialAssertText;
    u32 mMainHeapName;
    u32 mSpareHeapName;
    u32 mHIOName;
    u32 mHeaderDtorLink0;
    u32 mHeaderDtorLink1;
    u32 mHeaderDtorLink2;
    u32 mHIOVtable;
};
