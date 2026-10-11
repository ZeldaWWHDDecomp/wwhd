/** Ballad of Gales and two-actor event camera, private HD view.
 * GameCube dCamera_c fields retain their names; mWork shifts by four bytes in HD.
 * HD-only work overlays expose only the verified event fields. */
#pragma once
#include "gc_api/gc_api.h"
namespace event_camera_pair {
enum : u32 { CAMERA_EVENT_TRANSFORM = 0x400, CAMERA_LINE_DEFAULT = 0x7F, fpcNm_SHIP_e = 0xA5,
    JUDGE_FOR_PROCESS_NAME = 0x025E121C, JUDGE_BY_PROCESS_ID = 0x025E1234 };
enum : u32 { TORNADO_CHOOSE_EYE, TORNADO_TURN, TORNADO_EASE_OUT, TORNADO_DONE };
struct CameraProcessNameHD { be<u16> mPadding; be<u16> mName; be<u32> mTail; };
WWHD_SIZE(CameraProcessNameHD, 8);
WWHD_OFFSET(CameraProcessNameHD, mName, 2);
struct CameraEventGlobeHD {
    be<f32> mRadius;
    be<s16> mV, mU;
};
struct CameraEventViewHD {
    CameraEventGlobeHD mDirection;
    cXyz mCenter, mEye;
    be<s16> mBank;
    u8 mPadding[2];
    be<f32> mFovy;
};
WWHD_SIZE(CameraEventViewHD, 0x28);
struct TornadoWarpWorkHD {
    be<u32> mState;
    be<u32> mTimer;
    cXyz mEye;
    gptr<fopAc_ac_c> mpShipActor;
};
WWHD_SIZE(TornadoWarpWorkHD, 0x18);
struct TwoActorWorkHD {
    gptr<fopAc_ac_c> mpActor1, mpActor2;
    be<u32> mActorId1, mActorId2;
    be<f32> mCenterCushion, mEyeCushion;
    be<f32> mRadiusMin, mRadiusMax;
    be<f32> mLatitudeMin, mLatitudeMax;
    be<f32> mLongitudeMin, mLongitudeMax;
    be<f32> mFovy, mCenterRatio;
    cXyz mCenterGap;
    be<f32> mRadius, mLatitude, mLongitude;
};
WWHD_SIZE(TwoActorWorkHD, 0x50);
WWHD_OFFSET(TwoActorWorkHD, mCenterGap, 0x38);
WWHD_OFFSET(TwoActorWorkHD, mRadius, 0x44);
struct dCamera_c {
    bool tornadoWarpEvCamera();
    bool twoActor0EvCamera();
    /* 0x000 */ u8 mPadding0[0x3C];
    /* 0x03C */ CameraEventViewHD mViewCache;
    /* 0x064 */ u8 mPadding64[0x100 - 0x64];
    /* 0x100 */ be<u8> m100, m101, m102;
    /* 0x103 */ u8 mPadding103[0x11C - 0x103];
    /* 0x11C */ be<u32> m11C;
    /* 0x120 */ u8 mPadding120[8];
    /* 0x128 */ gptr<fopAc_ac_c> mpPlayerActor;
    /* 0x12C */ u8 mPadding12C[0x37C - 0x12C];
    /* 0x37C */ union { TornadoWarpWorkHD tornadoWarp; TwoActorWorkHD twoActor; } mWork;
    /* 0x3CC */ u8 mPadding3CC[0x510 - 0x3CC];
    /* 0x510 */ be<u32> mEventFlags;
    /* 0x514 */ u8 mPadding514[0x8DA - 0x514];
    /* 0x8DA */ be<u8> m786;
    /* 0x8DB */ u8 mPadding8DB[5];
};
WWHD_OFFSET(dCamera_c, mViewCache.mCenter, 0x44);
WWHD_OFFSET(dCamera_c, mViewCache.mEye, 0x50);
WWHD_OFFSET(dCamera_c, mViewCache.mBank, 0x5C);
WWHD_OFFSET(dCamera_c, mViewCache.mFovy, 0x60);
WWHD_OFFSET(dCamera_c, m100, 0x100);
WWHD_OFFSET(dCamera_c, m11C, 0x11C);
WWHD_OFFSET(dCamera_c, mpPlayerActor, 0x128);
WWHD_OFFSET(dCamera_c, mWork, 0x37C);
WWHD_OFFSET(dCamera_c, mEventFlags, 0x510);
WWHD_OFFSET(dCamera_c, m786, 0x8DA);
WWHD_SIZE(dCamera_c, 0x8E0);
struct TwoActorDefaultsHD {
    be<f32> mCenterCushion, mEyeCushion, mFovy;
    be<f32> mRadiusMin, mRadiusMax;
    be<f32> mLatitudeMin, mLatitudeMax;
    be<f32> mLongitudeMin, mLongitudeMax, mCenterRatio;
};
WWHD_SIZE(TwoActorDefaultsHD, 0x28);
struct CameraPointWordsHD { be<u32> x, y, z; };
struct CameraEventZeroHD {
    cXyz mZero;
    u8 mPaddingC[0x1C - 0xC];
    be<u32> mGuard;
};
WWHD_OFFSET(CameraEventZeroHD, mGuard, 0x1C);

} // namespace event_camera_pair
struct dCcS;
using namespace event_camera_pair;
// HD: explicit-result adapters retain the original c_angle/c_xyz and camera call ABI.
be<s16>* cameraPairAngleFromDegrees(be<s16>*, f32);
be<s16>* cameraPairAngleCopy(be<s16>*, const be<s16>*);
void cameraPairScalarMinusAngle(be<s16>*, s32, const be<s16>*);
void cameraPairAngleMultiply(const be<s16>*, be<s16>*, f32);
void cameraPairAngleAddAssign(be<s16>*, const be<s16>*);
void cameraPairAngleSubtractAssign(be<s16>*, const be<s16>*);
void cameraPairAngleSubtract(const be<s16>*, be<s16>*, const be<s16>*);
f32 cameraPairAngleDegree(const be<s16>*);
void cameraPairVectorSubtract(const cXyz*, cXyz*, const cXyz*);
void cameraPairVectorAdd(const cXyz*, cXyz*, const cXyz*);
void cameraPairVectorScale(const cXyz*, cXyz*, f32);
f32 cameraPairVectorMagnitudeSquared(const cXyz*);
f32 cameraPairSquareRoot(f32);
inline f32 cameraPairVectorLength(const cXyz* point) {
    return cameraPairSquareRoot(cameraPairVectorMagnitudeSquared(point));
}
void cameraPairGlobeVector(CameraEventGlobeHD*, const cXyz*);
void cameraPairGlobeFromVector(CameraEventGlobeHD*, const cXyz*);
void cameraPairGlobeSet(CameraEventGlobeHD*, f32, const be<s16>*, const be<s16>*);
void cameraPairGlobeXyz(const CameraEventGlobeHD*, cXyz*);
void cameraPairRelational(event_camera_pair::dCamera_c*, cXyz*, fopAc_ac_c*, const cXyz*);
void cameraPairRelationalWithAngle(event_camera_pair::dCamera_c*, cXyz*, fopAc_ac_c*, const cXyz*, const be<s16>*);
void cameraPairRelationalBetween(event_camera_pair::dCamera_c*, cXyz*, fopAc_ac_c*, fopAc_ac_c*, const cXyz*, f32);
void cameraPairAttention(event_camera_pair::dCamera_c*, cXyz*, fopAc_ac_c*);
bool cameraPairLineCheck(event_camera_pair::dCamera_c*, const cXyz*, const cXyz*, u32);
void cameraPairRotateY(cXyz*, const cXyz*, const be<s16>*);
bool cameraPairCheckCamera(dCcS*, cXyz*, cXyz*, f32, fopAc_ac_c*, fopAc_ac_c*);
fopAc_ac_c* cameraPairGetActor(event_camera_pair::dCamera_c*, const char*, const char*);
u32 cameraPairGetFloat(event_camera_pair::dCamera_c*, be<f32>*, const char*, f32);
u32 cameraPairGetXyz(event_camera_pair::dCamera_c*, cXyz*, const char*, const cXyz*);
