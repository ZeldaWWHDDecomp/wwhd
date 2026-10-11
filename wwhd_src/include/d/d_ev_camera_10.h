#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
namespace event_camera_uniform {
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
struct TransTail {
    GuestWord mTransType;
    GuestFloat mCushion;
    cSGlobe_l mStartGlobe;
    be<u8> mBankSet;
    u8 _65[3];
    GuestWord mBSpCurve;
};
struct BrakeTail {
    GuestFloat mTotal;
    GuestWord mFullFrames, mRampFrames, mBrakeType, mTransType;
    GuestFloat mSteps, mCushion;
    cSGlobe_l mStartGlobe;
    be<u8> mBankSet;
    u8 _79[3];
};
struct MoveWork {
    cXyz mStartEye, mStartCenter;
    GuestFloat mStartFovy, mStartBank;
    cXyz mEye, mCenter;
    GuestFloat mFovy, mBank;
    gptr<fopAc_ac_c> mpRelActor;
    GuestWord mRelActorId;
    be<char> mRelUseMask[8];
    GuestWord mTimer;
    union {
        TransTail mTrans;
        BrakeTail mBrake;
    };
};
WWHD_SIZE(MoveWork, 0x7C);
WWHD_OFFSET(MoveWork, mpRelActor, 0x40);
WWHD_OFFSET(MoveWork, mTrans.mStartGlobe, 0x5C);
WWHD_OFFSET(MoveWork, mBrake.mStartGlobe, 0x70);
enum MoveKind { UNIFORM, BRAKING };
struct VectorWords {
    GuestWord x, y, z;
};
struct GlobeWords {
    GuestWord mRadius, mAngles;
};
struct AnglePair {
    cSAngle_l m0, m1;
};
struct AngleFour {
    cSAngle_l m0, m1, m2, m3;
};
struct AngleSix {
    cSAngle_l m0, m1, m2, m3, m4, m5;
};
struct EventString {
    char text[32];
};
struct SplineCurve {
    GuestFloat mValues[4];
};
struct SinCosEntry {
    GuestFloat mSin, mCos;
};
struct SinCosTable {
    SinCosEntry mEntries[8192];
};
struct StageString {
    gptr<char> mpText;
    GuestWord mVtable;
};
// HD: the original string-holder virtual calls and bounded comparison remain observable.
struct StageStringVtable {
    u8 _00[0x14];
    GuestWord mEmpty;
};
struct StageEventView {
    u8 _00[0xA4];
    GuestWord mState;
};
struct TactActorView {
    u8 _00[0x6D8];
    GuestFloat mHeight;
};
inline cSAngle_l& zeroAngle() {
    return *gabi::at<cSAngle_l>(0x101FF354);
}
} // namespace event_camera_uniform

using namespace event_camera_uniform;
u32 cameraUniformGetRequiredInt(dCamera_c*, GuestWord*, const char*);
u32 cameraUniformGetInt(dCamera_c*, GuestWord*, const char*, s32);
u32 cameraUniformGetFloat(dCamera_c*, GuestFloat*, const char*, f32);
u32 cameraUniformGetXyz(dCamera_c*, cXyz*, const char*, cXyz*);
u32 cameraUniformGetString(dCamera_c*, be<char>*, const char*, const char*);
fopAc_ac_c* cameraUniformGetActor(dCamera_c*, const char*);
void cameraUniformRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*);
void cameraUniformSubtract(cXyz*, cXyz*, cXyz*);
void cameraUniformAdd(cXyz*, cXyz*, cXyz*);
void cameraUniformScale(cXyz*, cXyz*, f32);
void cameraUniformPSVECAdd(cXyz*, cXyz*, cXyz*);
void cameraUniformGlobeVal(cSGlobe_l*, cXyz*);
void cameraUniformGlobeXyz(cSGlobe_l*, cXyz*);
void cameraUniformGlobeCtor(cSGlobe_l*, cXyz*);
void cameraUniformAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraUniformAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraUniformAngleScale(cSAngle_l*, cSAngle_l*, f32);
cSAngle_l* cameraUniformAngleShortCtor(cSAngle_l*, s32);
void cameraUniformPositionOf(dCamera_c*, cXyz*, fopAc_ac_c*);
void cameraUniformDirectionOf(dCamera_c*, cSAngle_l*, fopAc_ac_c*);
void cameraUniformAttentionPos(dCamera_c*, cXyz*, fopAc_ac_c*);
f32 cameraUniformSqrt(f32);
f32 cameraUniformSquareMag(cXyz*);
bool cameraUniformLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
bool cameraUniformLineBGCheckCross(dCamera_c*, cXyz*, cXyz*, cXyz*, u32);
cSGlobe_l* cameraUniformGlobeInvert(cSGlobe_l*);
void cameraUniformRotateY(cXyz*, cXyz*, cSAngle_l*);
void cameraUniformGlobeAnglesCtor(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*);
void cameraUniformShortMinusAngle(cSAngle_l*, s32, cSAngle_l*);
cSAngle_l* cameraUniformAngleFloatCtor(cSAngle_l*, f32);
void cameraUniformAngleAddAssign(cSAngle_l*, cSAngle_l*);
void cameraUniformSplineInit(u8*, u32, u32);
f32 cameraUniformAngleDegree(cSAngle_l*);
s32 cameraUniformEventIndex(void*, const char*, s32);
StageEventView* cameraUniformEventData(void*, s32);
void cameraUniformAddCalcPos(cXyz*, cXyz*, f32, f32, f32);
void cameraUniformSplineStep(u8*);
f32 cameraUniformSplineCalc(u8*, GuestFloat*);
