// Cannon minigame actor. HD expands the base/lighting fields; verified offsets below.
#pragma once
#include "d/d_2dnumber.h"
#include "d/d_cc_d.h"
#include "d/d_com_inf_game.h"
#include "f_op/f_op_actor.h"
struct daCanon_c : fopAc_ac_c {
    BOOL CreateHeap();
    void set_mtx();
    void wait_proc_init();
    void game_start_proc_init();
    void game_proc_init();
    void fire_proc_init();
    void pause_proc_init();
    void end_proc_init();
    void makeFireEffect(cXyz* position, csXyz* rotation, s32 kind);
    void createTargetObj();
    void PadMove();
    void GameInfo2DDraw();
    // HD: explicit result pointer keeps the verified hidden-result ABI.
    void getGridPos(cXyz* out, s32 column, s32 row);
    void getBulEndPos(cXyz* out, s16 yaw, s16 pitch);
    void bul_set_mtx(cXyz* p);
    Pair32 breakAllObj();
    BOOL createCheck(s32 column, s32 row, s32 count);
    void game_start_proc();
    s32 _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    void wait_proc();
    void game_proc();
    void fire_proc();
    void pause_proc();
    void end_proc();
    using Func = void (daCanon_c::*)();
    /* 0x3AC */ gabi::ptmf<Func> mProcess;
    /* 0x3B4 */ dCcD_Stts mStts1;
    /* 0x3F0 */ dCcD_Cyl mCyl1;
    /* 0x520 */ dCcD_Stts mStts2;
    /* 0x55C */ dCcD_Cyl mCyl2;
    /* 0x68C */ request_of_phase_process_class mPhase;
    /* 0x694 */ gptr<J3DModel> mpModel1, mpModel2;
    /* 0x69C */ gptr<dDlst_2DObject_c> mBombIcons[10];
    /* 0x6C4 */ gptr<dDlst_2DObject_c> mShipIcons[5];
    /* 0x6D8 */ gptr<dDlst_2DBattery_c> mpBattery;
    /* 0x6DC */ Mtx34 mBaseMtx;
    /* 0x70C */ be<u8> mTargetGrid[10];
    u8 pad[2];
    /* 0x718 */ be<u32> mTargetIds[5];
    /* 0x72C */ cXyz mLaunchPos, mEndPos, mBallPos;
    /* 0x750 */ be<f32> mRecoil;
    /* 0x754 */ cXyz mMuzzlePos;
    /* 0x760 */ csXyz mMuzzleAngle;
    /* 0x766 */ be<s16> mUnused1;
    /* 0x768 */ be<s16> mUnused2;
    /* 0x76A */ be<s16> mFlightFrames;
    /* 0x76C */ be<s16> mFlightDuration;
    /* 0x76E */ be<s16> mTimer;
    /* 0x770 */ be<s16> mUnused3;
    /* 0x772 */ be<s16> mHitCount;
    /* 0x774 */ be<s16> mRemainingShots;
    /* 0x776 */ be<s16> mRecoilPhase;
    /* 0x778 */ be<s16> mRecoilAmplitude;
    /* 0x77A */ be<u16> padding;
    /* 0x77C */ be<u8> mEffectActive;
    /* 0x77D */ u8 tail[3];
};
WWHD_OFFSET(daCanon_c, mProcess, 0x3AC);
WWHD_OFFSET(daCanon_c, mCyl1, 0x3F0);
WWHD_OFFSET(daCanon_c, mCyl2, 0x55C);
WWHD_OFFSET(daCanon_c, mpModel1, 0x694);
WWHD_OFFSET(daCanon_c, mBaseMtx, 0x6DC);
WWHD_OFFSET(daCanon_c, mLaunchPos, 0x72C);
WWHD_OFFSET(daCanon_c, mHitCount, 0x772);
WWHD_OFFSET(daCanon_c, mEffectActive, 0x77C);
static_assert(sizeof(daCanon_c) == 0x780);
namespace canon {
// A real nonleaf callee may save LR at callerSP+4. Keep payload above it.
template <class T> struct Local {
    struct Frame {
        be<u32> linkage[2];
        T payload;
    };
    static_assert(offsetof(Frame, payload) == 8);
    gabi::Local<Frame> frame;
    T* get() const { return gabi::at<T>(frame.a + 8); }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    operator T*() const { return get(); }
};
} // namespace canon

using CanonTargetCreateCallback = BOOL (*)(void*);
using CanonHeapCallback = BOOL (*)(daCanon_c*);
using CanonTargetSearch = void* (*)(void*, void*);
// Cannon actor interfaces; exact HD calling widths are retained.
void canon_setScreenPosition(void*, void*);
BOOL canon_getATrigger(s32);
f32 canon_getStickX(s32);
f32 canon_getStickY(s32);
void canon_registerCollision(void*, void*);
s16 canon_chaseAngle(be<s16>*, s16, s32, s32, s32);
void canon_chaseShort(be<s16>*, s32, s32, s32);
void canon_setCylinderCenter(void*, cXyz*);
void canon_setCylinderRadius(void*, f32);
void canon_addVector(cXyz*, cXyz*, cXyz*);
void canon_subtractVector(cXyz*, cXyz*, cXyz*);
void canon_scaleVector(cXyz*, cXyz*, f32);
void* canon_constructBattery(void*);
BOOL canon_initBattery(void*, void*, void*, void*, void*);
void* canon_constructIcon(void*);
BOOL canon_initIcon(void*, void*, void*);
u32 canon_makeBombParams(s32, s32, s32);
void canon_destroyStatus(void*, s32);
void canon_destroyCylinder(void*, s32);
void* canon_getHitObject(void*);
void canon_initStatus(void*, s32, s32, void*);
void canon_clearAttackHit(void*);
BOOL canon_checkAttackHit(void*);
void canon_setCylinder(void*, void*);
void* canon_getPlay();
s32 canon_loadResource(void*, const char*);
void canon_deleteResource(void*, const char*);
s32 canon_getReverb(s8);
void canon_addDisplayList(void*, void*, void*, void*);
s32 canon_getStaffId(void*, const char*, s32, s32);
s32 canon_getActIndex(void*, s32, void*, s32, s32, s32);
void canon_cutEnd(void*, s32);
void canon_clearDiceWeather(s32, s32, s32, f32);
void* canon_getEnvironment();
void canon_addPointLight(void*);
void canon_removePointLight(void*);
void canon_setLightTev(void*, s32, cXyz*, void*);
void canon_applyLightTev(void*, J3DModel*, void*);
void canon_removePointWind(void*);
void canon_addPointWind(void*);
void canon_emitParticle(void*, s32, s32, cXyz*, csXyz*, cXyz*, s32, s32, s32, void*, void*, void*);
void canon_emitSimpleParticle(void*, s32, cXyz*, csXyz*, cXyz*, s32);
void canon_startShock(void*, s32, s32, cXyz*);
void canon_constructActor(void*);
void canon_destroyActor(void*, s32);
Pair32 canon_searchTargets(CanonTargetSearch, void*);
void canon_createActor(s32, u32, cXyz*, s32, void*, void*, s32, u32);
u32 canon_createChild(s32, u32, u32, cXyz*, s32, void*, void*, s32, CanonTargetCreateCallback);
BOOL canon_createHeap(void*, CanonHeapCallback, u32);
void canon_createWaterPillar(cXyz*, f32, f32, s32);
void canon_startSound(u32, void*, s32, s32);
void canon_stopSound(cXyz*);
void canon_deleteSound(cXyz*);
void canon_translate(f32, f32, f32);
void* canon_getObjectResource(void*, void*, s32);
void canon_setShotsUsed(void*, s32);
void canon_setHits(void*, s16);
void canon_setHudAngle(void*, f32);
void* canon_getOptions(void*);
void canon_assert(const char*, s32, const char*);
s8 canon_findJointName(void*, const char*);
void* canon_getJointNames(void*);
void* canon_getJoint(void*);
f32 canon_vectorLengthSquared(cXyz*);
void canon_constructInitFlag(void*);
void canon_setInitFlag(void*);
void canon_registerDestructor(void*);
f32 canon_squareRoot(f32);

namespace canon {
struct Model {
    u8 pad0[0x2C];
    gptr<struct MatrixBuffer> mpMatrixBuffer;
    u8 pad30[0xB8 - 0x30];
    gptr<daCanon_c> mpActor;
    cXyz mScale;
    Mtx34 mBaseTransform;
};
struct MatrixBuffer {
    u8 pad0[4];
    be<u16> mFlags;
    u8 pad6[10];
    gptr<Mtx34> mpMatrices;
};
struct Joint {
    u8 pad0[4];
    be<u16> mJntNo;
};
struct Hud {
    u8 pad0[0x18];
    u8 mScreenPosition[0x40];
    be<f32> mAngle;
};
struct DrawList {
    u8 pad0[0x1DC];
    be<u32> mp2DOpaListCur, mp2DOpaListEnd;
};
struct Meter {
    u8 pad0[0x208];
    gptr<Hud> mpGameInfo;
};
struct Options {
    u8 pad0[2];
    be<u8> mCameraControl;
};
struct Camera {
    u8 pad0[0x234];
    csXyz mAngle;
};
struct PlayerVtable {
    u8 pad0[0x114];
    gfn mSetPosition;
};
struct Light {
    cXyz mPosition;
    be<u16> mRed, mGreen, mBlue;
    u8 pad12[2];
    be<f32> mPower, mFluctuation;
    u8 pad1c[4];
    be<f32> mInfluence;
};
struct Wind {
    cXyz mPosition, mDirection;
    be<f32> mRadius, mStrength, mSpeed;
};
struct JointEntry {
    u8 pad0[8];
    gfn mCallback;
    u8 padc[0x10];
};
struct ModelData {
    u8 pad0[4];
    be<u32> mJointCount;
    gptr<JointEntry> mpJoints;
};
struct JointNames {
    u8 pad0[0x10];
    be<u32> mNameTableOffset;
};
WWHD_OFFSET(Model, mBaseTransform, 0xC8);
WWHD_OFFSET(Hud, mAngle, 0x58);
WWHD_OFFSET(Light, mInfluence, 0x20);
} // namespace canon

namespace canon {
struct ArchiveName {
    char mName[7];
};
struct ColumnAngles {
    be<s16> mAngles[8];
};
struct CylinderSource {
    u8 mData[0x44];
};
struct CutTable {
    gptr<const char> mCutName;
};
struct DestructorRecord {
    be<u32> mLink, mDestructor, mObject;
};
struct ScreenPosition {
    u8 mData[0x30];
};
struct SaveRoot {
    u8 pad0[0x12C0];
    u8 mOptions[4];
};
struct TargetCell {
    be<u8> mColumn, mRow;
};
} // namespace canon

// HD resource archive indices, preserving the Bomber archive's ordering.
enum BomberResource {
    BOMBER_BDL_CANON = 5,
    BOMBER_BDL_CANNONBALL = 8,
    BOMBER_BTI_BATTERY2 = 11,
    BOMBER_BTI_BATTERY3 = 12,
    BOMBER_BTI_BOMB1 = 13,
    BOMBER_BTI_BOMB2 = 14,
    BOMBER_BTI_SHIP2 = 15,
    BOMBER_BTI_BATTERY1 = 16,
    BOMBER_BTI_SHIP1 = 17,
    BOMBER_BTI_BATTERY4 = 18
};

WWHD_OFFSET(canon::Model, mpMatrixBuffer, 0x2C);
WWHD_OFFSET(canon::Model, mpActor, 0xB8);
WWHD_OFFSET(canon::Model, mScale, 0xBC);
WWHD_SIZE(canon::Model, 0xF8);
WWHD_OFFSET(canon::MatrixBuffer, mFlags, 4);
WWHD_OFFSET(canon::MatrixBuffer, mpMatrices, 0x10);
WWHD_SIZE(canon::MatrixBuffer, 0x14);
WWHD_OFFSET(canon::Joint, mJntNo, 4);
WWHD_SIZE(canon::Joint, 6);
WWHD_SIZE(canon::Hud, 0x5C);
WWHD_OFFSET(canon::Meter, mpGameInfo, 0x208);
WWHD_SIZE(canon::Meter, 0x20C);
WWHD_OFFSET(canon::Options, mCameraControl, 2);
WWHD_SIZE(canon::Options, 3);
WWHD_OFFSET(canon::Camera, mAngle, 0x234);
WWHD_SIZE(canon::Camera, 0x23A);
WWHD_OFFSET(canon::PlayerVtable, mSetPosition, 0x114);
WWHD_SIZE(canon::PlayerVtable, 0x118);
WWHD_SIZE(canon::Light, 0x24);
WWHD_SIZE(canon::Wind, 0x24);
WWHD_OFFSET(canon::Light, mRed, 0xC);
WWHD_OFFSET(canon::Light, mPower, 0x14);
WWHD_OFFSET(canon::Wind, mDirection, 0xC);
WWHD_OFFSET(canon::Wind, mRadius, 0x18);
WWHD_OFFSET(canon::JointEntry, mCallback, 8);
WWHD_SIZE(canon::JointEntry, 0x1C);
WWHD_OFFSET(canon::ModelData, mJointCount, 4);
WWHD_OFFSET(canon::ModelData, mpJoints, 8);
WWHD_SIZE(canon::ModelData, 0xC);
WWHD_OFFSET(canon::JointNames, mNameTableOffset, 0x10);
WWHD_SIZE(canon::JointNames, 0x14);
WWHD_SIZE(canon::ColumnAngles, 0x10);
WWHD_SIZE(canon::CylinderSource, 0x44);
WWHD_SIZE(canon::CutTable, 4);
WWHD_SIZE(canon::DestructorRecord, 0xC);
WWHD_SIZE(canon::ScreenPosition, 0x30);
WWHD_OFFSET(canon::SaveRoot, mOptions, 0x12C0);
WWHD_SIZE(canon::SaveRoot, 0x12C4);
WWHD_SIZE(canon::TargetCell, 2);
WWHD_SIZE(canon::ArchiveName, 7);
WWHD_OFFSET(canon::DrawList, mp2DOpaListCur, 0x1DC);
WWHD_SIZE(canon::DrawList, 0x1E4);
