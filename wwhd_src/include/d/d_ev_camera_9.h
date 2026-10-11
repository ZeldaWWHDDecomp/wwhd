#pragma once
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
#include "SSystem/SComponent/c_m3d_g_lin.h"
namespace event_camera_rolling {
using GuestWord = be<u32>;
using GuestFloat = be<f32>;
struct RollingWork {
    be<u8> mTimerSet, mBankSet;
    u8 _02[2];
    cXyz mEye, mCenter, mEyeOffset, mCenterOffset;
    be<f32> mFovy, mBank;
    gptr<fopAc_ac_c> mpRelActor;
    be<char> mRelUseMask[4];
    GuestWord mTimer, mTransType;
    be<f32> mRoll, mRadiusAdd, mLatitude, mCtrCus;
};
WWHD_SIZE(RollingWork, 0x5C);
WWHD_OFFSET(RollingWork, mpRelActor, 0x3C);
struct WindWork {
    GuestWord mState, mCounter;
    cSGlobe_l mWind, mStart, mEnd;
    gptr<fopAc_ac_c> mpBird;
    cXyz mEye, mCenter, mTorishita;
    be<f32> mStopDist, mFollowCushion;
    GuestWord mFrameLimit, mSideFlag, mType;
    be<f32> mStartDistance, mBirdFlyDist, mNearFovy, mFarFovy, mFovyCushion;
    be<f32> mSwingLength, mSwingProgress;
};
WWHD_SIZE(WindWork, 0x78);
WWHD_OFFSET(WindWork, mpBird, 0x20);
WWHD_OFFSET(WindWork, mTorishita, 0x3C);
WWHD_OFFSET(WindWork, mSwingProgress, 0x74);
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
struct SideOffsets {
    cXyz mPoints[6];
};
WWHD_SIZE(SideOffsets, 0x48);
// HD: retain the original 0x20-byte temporary around the canonical 0x1C-byte line class.
struct LineFrame {
    cM3dGLin mLine;
    u8 _1C[4];
};
WWHD_SIZE(LineFrame, 0x20);
struct WindAnglesView {
    u8 _00[0xA24];
    be<s16> mTactWindAngleX, mTactWindAngleY;
};
struct EventString {
    char text[16];
};
inline cSAngle_l& zeroAngle() {
    return *gabi::at<cSAngle_l>(0x101FF354);
}
enum WindState : u32 {
    WIND_INITIAL = 0,
    WIND_WAIT = 1,
    WIND_FOLLOW_START = 2,
    WIND_FOLLOW = 3,
    WIND_SIDE_START = 0xA,
    WIND_SIDE_WAIT = 0xB,
    WIND_SIDE_SWING = 0xC,
    WIND_CLOSE_START = 0xD,
    WIND_CLOSE_WAIT = 0xE,
    WIND_CLOSE_FOLLOW = 0xF,
    WIND_HOLD = 0x63
};
} // namespace event_camera_rolling
using namespace event_camera_rolling;
u32 cameraRollingGetInt(dCamera_c*, GuestWord*, const char*, s32);
u32 cameraRollingGetFloat(dCamera_c*, GuestFloat*, const char*, f32);
u32 cameraRollingGetXyz(dCamera_c*, cXyz*, const char*, cXyz*);
u32 cameraRollingGetString(dCamera_c*, be<char>*, const char*, const char*);
fopAc_ac_c* cameraRollingGetActor(dCamera_c*, const char*);
void cameraRollingRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*);
void cameraRollingPositionOf(dCamera_c*, cXyz*, fopAc_ac_c*);
void cameraRollingAttentionPos(dCamera_c*, cXyz*, fopAc_ac_c*);
void cameraRollingDirectionOf(dCamera_c*, cSAngle_l*, fopAc_ac_c*);
void cameraRollingEyePos(dCamera_c*, cXyz*, fopAc_ac_c*);
void cameraRollingSubtract(cXyz*, cXyz*, cXyz*);
void cameraRollingAdd(cXyz*, cXyz*, cXyz*);
void cameraRollingScale(cXyz*, cXyz*, f32);
void cameraRollingPSVECAdd(cXyz*, cXyz*, cXyz*);
void cameraRollingGlobeXyz(cSGlobe_l*, cXyz*);
void cameraRollingGlobeCtor(cSGlobe_l*, cXyz*);
void cameraRollingGlobeCopyCtor(cSGlobe_l*, cSGlobe_l*);
cSGlobe_l* cameraRollingGlobeInvert(cSGlobe_l*);
void cameraRollingGlobeSet(cSGlobe_l*, f32, s32, s32);
void cameraRollingAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraRollingAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*);
void cameraRollingAngleScale(cSAngle_l*, cSAngle_l*, f32);
cSAngle_l* cameraRollingAngleFloatCtor(cSAngle_l*, f32);
cSAngle_l* cameraRollingAngleShortCtor(cSAngle_l*, s32);
f32 cameraRollingAngleDegree(cSAngle_l*);
f32 cameraRollingSquareMag(cXyz*);
f32 cameraRollingSqrt(f32);
bool cameraRollingLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32);
u32 cameraRollingTactWindDirection();
fopAc_ac_c* cameraRollingConvPId(dEvt_control_c*, u32);
void cameraRollingResetBlure(dCamera_c*, s32);
void cameraRollingBlureTimer(dCamera_c*, s32);
void cameraRollingBlureAlpha(dCamera_c*, f32);
void cameraRollingBlureScale(dCamera_c*, f32);
void cameraRollingLineCtor(cM3dGLin*, cXyz*, cXyz*);
bool cameraRollingNearestPoint(cM3dGLin*, cXyz*, cXyz*, GuestFloat*);
f32 cameraRollingHorizontalDistance(cXyz*, cXyz*);
