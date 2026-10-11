#pragma once
#include "d/d_camera.h"
// HD: per-engine work begins at 0x37C rather than GameCube 0x378.
namespace event_camera_fixed {
using GuestWord = be<u32>;
struct FixedPositionWork {
    be<u8> mTimerSet, mBankSet;
    u8 _02[2];
    cXyz mEye, mCtrGap, mCenter;
    be<f32> mFovy, mBank, mCtrCus, mStartRadius, mRadius;
    gptr<fopAc_ac_c> mpRelActor, mpTarget;
    GuestWord mTargetId;
    be<char> mRelUseMask[4];
    GuestWord mTimer;
};
WWHD_SIZE(FixedPositionWork, 0x50);
WWHD_OFFSET(FixedPositionWork, mpRelActor, 0x3C);
WWHD_OFFSET(FixedPositionWork, mTimer, 0x4C);
struct FixedFrameWork {
    be<u8> mTimerSet;
    u8 _01[3];
    cXyz mEye, mCenter;
    be<f32> mFovy, mBank;
    gptr<fopAc_ac_c> mpRelActor;
    be<char> mRelUseMask[4];
    GuestWord mTimer;
    be<u8> mBankSet;
    u8 _31[3];
    cXyz mBasePos;
};
WWHD_SIZE(FixedFrameWork, 0x40);
WWHD_OFFSET(FixedFrameWork, mpRelActor, 0x24);
WWHD_OFFSET(FixedFrameWork, mBankSet, 0x30);
struct StokerWork {
    be<u8> mTimerSet, mBankSet;
    u8 _02[2];
    cXyz mEyeGap, mCtrGap;
    be<f32> mFovy, mBank;
    gptr<fopAc_ac_c> mpStoker, mpTarget;
    GuestWord mStokerId, mTargetId;
    u8 _34[4];
    GuestWord mTimer;
};
WWHD_SIZE(StokerWork, 0x3C);
WWHD_OFFSET(StokerWork, mpStoker, 0x24);
WWHD_OFFSET(StokerWork, mTimer, 0x38);
struct VectorWords {
    GuestWord x, y, z;
};
WWHD_SIZE(VectorWords, 0xC);
struct AnglePair {
    cSAngle_l mDirection, mDifference;
};
WWHD_SIZE(AnglePair, 4);
struct EventString {
    char text[16];
};
// HD: references the shared cSAngle::_0 object without defining a TU-wide copy.
inline cSAngle_l& zeroAngle() {
    return *gabi::at<cSAngle_l>(0x101FF354);
}
enum : u32 { JUDGE_BY_ID = 0x025E1234, BANK_SET = 0x400, LINE_CHECK_FLAGS = 0x8F };
} // namespace event_camera_fixed
using namespace event_camera_fixed;
u32 cameraFixedGetInt(dCamera_c*, GuestWord*, const char*, s32);
u32 cameraFixedGetFloat(dCamera_c*, be<f32>*, const char*, f32);
u32 cameraFixedGetXyz(dCamera_c*, cXyz*, const char*, cXyz*);
u32 cameraFixedGetString(dCamera_c*, be<char>*, const char*, const char*);
fopAc_ac_c* cameraFixedGetActor(dCamera_c*, const char*);
fopAc_ac_c* cameraFixedGetActorDefault(dCamera_c*, const char*, const char*);
void cameraFixedRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*);
void cameraFixedPositionOf(dCamera_c*, cXyz*, fopAc_ac_c*);
void cameraFixedAttentionPos(dCamera_c*, cXyz*, fopAc_ac_c*);
void cameraFixedDirectionOf(dCamera_c*, cSAngle_l*, fopAc_ac_c*);
void cameraFixedSubtract(cXyz*, cXyz*, cXyz*);
void cameraFixedAdd(cXyz*, cXyz*, cXyz*);
void cameraFixedScale(cXyz*, cXyz*, f32);
void cameraFixedGlobeXyz(cSGlobe_l*, cXyz*);
void cameraFixedGlobeCtor(cSGlobe_l*, cXyz*);
void cameraFixedGlobeDefaultCtor(cSGlobe_l*);
void cameraFixedAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraFixedAngleAdd(cSAngle_l*, cSAngle_l*, s32);
cSAngle_l* cameraFixedAngleCtor(cSAngle_l*, s32);
f32 cameraFixedSquareMag(cXyz*);
f32 cameraFixedSqrt(f32);
bool cameraFixedLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
void cameraFixedPSVECAdd(cXyz*, cXyz*, cXyz*);
