/** Camera engines with undecompiled GameCube bodies: vomit, shield and hanging.
 * HD: mWork begins four bytes later; descriptive work fields document the HD-only algorithms.
 * The shared dCamera_c layout remains authoritative. */
#pragma once
#include "d/actor/d_a_player.h"
#include "d/d_camera.h"
#include "gc_api/d/d_com_inf_game.h"
namespace camera_engine2 {
enum : u32 {
    VOMIT_WORK = 0x564D4954,
    SHIELD_WORK = 0x53484C44,
    HUNG_WORK = 0x48554E47,
    PLAYER_IN_BUD = 0x80,
    PLAYER_HANG = 0x100,
    PLAYER_FALL = 0x80000000,
    PLAYER_FALL_OR_BUD = PLAYER_FALL | PLAYER_IN_BUD,
    CAMERA_FAST_TRANSITION = 0x8000,
    CAMERA_LINE_FLAGS = 0x7F
};
enum : s16 { fpcNm_PLAYER_e = 0xA8, fpcNm_NPC_MD_e = 0x16F };
struct VomitWorkHD {
    /* 0x00 */ be<u32> mTag;              /* 'VMIT' */
    /* 0x04 */ be<s32> mTransitionFrames; /* transition frames */
    /* 0x08 */ be<f32> mRemainingWeight;  /* remaining weight sum */
    /* 0x0C */ be<f32> mFrameWeight;      /* frame weight / fall time */
    /* 0x10 */ be<s32> mIdleFrames;
    /* 0x14 */ be<s32> mFallFrames; /* frames below the start height */
    /* 0x18 */ be<f32> mMovementBlend;
    /* 0x1C */ be<f32> mPlayerHeight; /* smoothed player height */
    /* 0x20 */ be<f32> mRadius;
    /* 0x24 */ be<f32> mTargetRadius;
    /* 0x28 */ be<f32> mLatitude;
    /* 0x2C */ be<f32> mCenterHeightBlend; /* centre height ratio */
    /* 0x30 */ u8 mPadding30[4];
    /* 0x34 */ be<f32> mLastPlayerHeight; /* last player height */
    /* 0x38 */ cXyz mCenter;              /* smoothed centre */
    /* 0x44 */ cXyz mPlayerPosition;      /* player position */
    /* 0x50 */ be<u8> mInitialized;
    /* 0x51 */ u8 mPadding51[3];
    /* 0x54 */ be<f32> mRadiusRate;
    /* 0x58 */ be<f32> mLatitudeRate;
    /* 0x5C */ u8 mPadding5C[0x10];
    /* 0x6C */ be<f32> mStickX;
    /* 0x70 */ be<f32> mStickY;
};

WWHD_SIZE(VomitWorkHD, 0x74);
WWHD_OFFSET(VomitWorkHD, mCenter, 0x38);
WWHD_OFFSET(VomitWorkHD, mPlayerHeight, 0x1C);
struct HungWorkHD {
    /* 0x00 */ be<u32> mTag;              /* 'HUNG' */
    /* 0x04 */ be<s32> mTransitionFrames; /* transition frames */
    /* 0x08 */ be<f32> mRemainingWeight;  /* remaining weight sum */
    /* 0x0C */ be<f32> mFrameWeight;      /* frame weight / fall time */
    /* 0x10 */ be<s32> mIdleFrames;
    /* 0x14 */ be<s32> mFallFrames; /* frames hanging */
    /* 0x18 */ be<f32> mMovementBlend;
    /* 0x1C */ be<f32> mPlayerHeight; /* smoothed player height */
    /* 0x20 */ be<f32> mRadius;
    /* 0x24 */ be<f32> mTargetRadius;
    /* 0x28 */ be<f32> mLatitude;
    /* 0x2C */ be<f32> mCenterHeightBlend; /* centre height ratio */
    /* 0x30 */ u8 mPadding30[4];
    /* 0x34 */ be<f32> mLastPlayerHeight; /* last player height */
    /* 0x38 */ cXyz mCenter;              /* smoothed centre */
    /* 0x44 */ be<u8> mInitialized;
    /* 0x45 */ u8 mPadding45[3];
    /* 0x48 */ be<f32> mRadiusRate;
    /* 0x4C */ be<f32> mLatitudeRate;
    /* 0x50 */ u8 mPadding50[0x10];
    /* 0x60 */ be<f32> mStickX; /* C stick x (smoothed) */
    /* 0x64 */ be<f32> mStickY; /* C stick y (smoothed) */
};

WWHD_SIZE(HungWorkHD, 0x68);
WWHD_OFFSET(HungWorkHD, mCenter, 0x38);
WWHD_OFFSET(HungWorkHD, mPlayerHeight, 0x1C);
struct ShieldWorkHD {
    be<u32> mTag;
    be<s32> mTransitionFrames;
    be<f32> mRemainingWeight, mFrameWeight, mRadius, mTargetRadius, mLatitude;
};
WWHD_SIZE(ShieldWorkHD, 0x1C);
WWHD_OFFSET(ShieldWorkHD, mLatitude, 0x18);
// HD: Medli's shield direction uses these canonical GameCube members.
struct MedliShieldViewHD {
    u8 mPadding0[0x4230];
    be<s16> m3114, m3116;
    u8 mPadding4234[0x43DC - 0x4234];
};
WWHD_SIZE(MedliShieldViewHD, 0x43DC);
WWHD_OFFSET(MedliShieldViewHD, m3114, 0x4230);
struct ShieldAnglesHD {
    be<s16> x, y;
};
WWHD_SIZE(ShieldAnglesHD, 4);
struct CameraLineCheckHD : cBgS_LinChk, dBgS_Chk {};
WWHD_SIZE(CameraLineCheckHD, 0x6C);
WWHD_OFFSET(CameraLineCheckHD, mLin, 0x24);
WWHD_OFFSET(CameraLineCheckHD, mbCamThrough, 0x5D);
struct PlaneNormalHD {
    cXyz mNormal;
};
WWHD_SIZE(PlaneNormalHD, 0xC);
struct CameraLineVtablesHD {
    u32 mCheck, mPolygon, mPass, mGroup;
    u32 mPolygonDestructor, mGroupDestructor, mPassDestructor;
};
WWHD_SIZE(CameraLineVtablesHD, 0x1C);
} // namespace camera_engine2
cSAngle_l* cameraEngineConstructAngle(cSAngle_l* angle, s16 value);
void cameraEngineConstructDifference(cSAngle_l* result, s16 angle, const cSAngle_l* other);
cSAngle_l* cameraEngineConstructDegrees(cSAngle_l* angle, f32 degrees);
void cameraEngineSetDegrees(cSAngle_l* angle, f32 degrees);
f32 cameraEngineAngleDegree(const void* angle);
f32 cameraEngineAngleNorm(const cSAngle_l* angle);
f32 cameraEngineAngleCos(const cSAngle_l* angle);
void cameraEngineAngleAdd(const void* angle, cSAngle_l* result, const void* other);
void cameraEngineAngleSubtract(const void* angle, cSAngle_l* result, const void* other);
void cameraEngineAngleScale(const void* angle, cSAngle_l* result, f32 scale);
void cameraEngineAngleAddValue(const cSAngle_l* angle, cSAngle_l* result, s16 value);
void cameraEngineAngleSubtractValue(const cSAngle_l* angle, cSAngle_l* result, s16 value);
void cameraEngineAngleAddAssign(cSAngle_l* angle, const cSAngle_l* other);
void cameraEngineAngleSubtractAssign(cSAngle_l* angle, const cSAngle_l* other);
void cameraEngineConstructGlobe(cSGlobe_l* globe, f32 radius, const cSAngle_l* latitude,
                                const cSAngle_l* longitude);
void cameraEngineConstructVectorGlobe(cSGlobe_l* globe, const cXyz* vector);
void cameraEngineGlobeXyz(const cSGlobe_l* globe, cXyz* result);
void cameraEngineVectorProduct(const cXyz* vector, cXyz* result, const cXyz* scale);
void cameraEngineProjectYZ(cXyz* result, const cSAngle_l* angle, const cXyz* position,
                           const cXyz* anchor);
f32 cameraEngineHorizontalDistance(const cXyz* first, const cXyz* second);
f32 cameraEngineAtan2(f32 y, f32 x);
s16 cameraEngineTargetAngleY(const cXyz* first, const cXyz* second);
f32 cameraEngineBezierRatio(f32 ratio, f32 weight);
f32 cameraEngineLimit(f32 value, f32 minimum, f32 maximum);
void cameraEngineConstructLine(camera_engine2::CameraLineCheckHD* check);
void cameraEngineDestroyLine(camera_engine2::CameraLineCheckHD* check, s32 flags);
camera_engine2::PlaneNormalHD* cameraEngineGetTriPlane(dBgS* background, u32 backgroundIndex,
                                                       u32 polygonIndex);

// HD: out-of-line angle and globe operations used by tornado/manual cameras.
void cameraEngineCopyAngle(cSAngle_l* angle, const cSAngle_l* other);
f32 cameraEngineAngleSin(const cSAngle_l* angle);
void cameraEngineDefaultGlobe(cSGlobe_l* globe);
void cameraEngineSetGlobeAngles(cSGlobe_l* globe, f32 radius, s16 latitude, s16 longitude);
void cameraEngineSetGlobe(cSGlobe_l* globe, f32 radius, const cSAngle_l* latitude, const cSAngle_l* longitude);

namespace camera_engine2 {
// HD: private work layouts for the undecompiled tornado and manual algorithms.
struct TornadoWorkHD {
    /* 0x00 */ be<u32> mTag;           /* 'TRND' */
    /* 0x04 */ gptr<fopAc_ac_c> mpTornado;  /* tornado actor (0xA5) */
    /* 0x08 */ gptr<fopAc_ac_c> mpShip;  /* ship actor (0x1BB or 0x11B) */
    /* 0x0C */ be<u32> mEntryViewIndex;           /* m07C at start */
    /* 0x10 */ be<s32> mEntryMode;           /* entry: 0 normal, 1 side eye, 2 event */
    /* 0x14 */ cXyz mSideEye;              /* side eye */
    /* 0x20 */ cXyz mStickCenterOffset;              /* C-stick centre offset */
    /* 0x2C */ cXyz mCenterTarget;              /* centre target */
};
WWHD_SIZE(TornadoWorkHD, 0x38);

struct ManualWorkHD {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ cXyz mLockonCenterOffset;       /* lock-on centre offset */
    /* 0x10 */ cXyz mCenterTarget;       /* centre target */
    /* 0x1C */ be<f32> mLockonBlend;    /* lock-on blend */
    /* 0x20 */ be<f32> mCenterHeight;    /* centre height */
    /* 0x24 */ u8 _24[4];
    /* 0x28 */ be<u8> mChargingAtStart;     /* charging at start */
    /* 0x29 */ u8 _29[3];
    /* 0x2C */ be<f32> mInitialRadius;
    /* 0x30 */ cSGlobe_l mDirection;  /* manual direction */
    /* 0x38 */ be<f32> mCenterFollowXZ;    /* centre follow ratio xz */
    /* 0x3C */ be<f32> mCenterFollowY;    /* centre follow ratio y */
    /* 0x40 */ be<s32> mStateCounter;
};
WWHD_SIZE(ManualWorkHD, 0x44);
WWHD_OFFSET(TornadoWorkHD, mCenterTarget, 0x2C);
WWHD_OFFSET(ManualWorkHD, mDirection, 0x30);
}
void cameraEngineChaseFloat(be<f32>* value, f32 target, f32 step);
bool cameraEngineRangeAdd(void* value, f32 add, f32 minimum, f32 maximum);

namespace camera_engine2 {
// HD: GetTriPla returns the plane coefficients without the GameCube vtable prefix.
struct PlaneEquationHD {
    cXyz mNormal;
    be<f32> mDistance;
};
WWHD_SIZE(PlaneEquationHD, 0x10);
WWHD_OFFSET(PlaneEquationHD, mDistance, 0x0C);
}
bool cameraEngineLineBlocked(dCamera_c* camera, cXyz* start, cXyz* end, u32 flags);
f32 cameraEngineFanBank(const void* setup);
void cameraEngineRelationalPosition(dCamera_c* camera, cXyz* result, fopAc_ac_c* player, fopAc_ac_c* target, cXyz* offset, f32 scale);

namespace camera_engine2 {
// HD: camera-visible fields of the tornado actor, measured independently of unused members.
struct TornadoActorViewHD {
    u8 mPadding000[0x64C];
    cSAngle_l mSideAngle;
    u8 mPadding64E[0x32];
    cSAngle_l mRideAngle;
};
WWHD_OFFSET(TornadoActorViewHD, mSideAngle, 0x64C);
WWHD_OFFSET(TornadoActorViewHD, mRideAngle, 0x680);
WWHD_SIZE(TornadoActorViewHD, 0x682);
struct ManualSetupViewHD {
    u8 mPadding00[0x80];
    be<f32> mFanFovySwing;
    be<f32> mLeafFovySwing;
};
WWHD_OFFSET(ManualSetupViewHD, mFanFovySwing, 0x80);
WWHD_OFFSET(ManualSetupViewHD, mLeafFovySwing, 0x84);
WWHD_SIZE(ManualSetupViewHD, 0x88);
struct CameraOptionViewHD {
    u8 mPadding00;
    be<u8> mCameraX;
};
WWHD_SIZE(CameraOptionViewHD, 2);
}
camera_engine2::CameraOptionViewHD* cameraEngineGetOptions(u32 saveOptions);

namespace camera_engine2 {
enum : u32 {
    TORNADO_WORK = 0x54524E44,
    CAMERA_UPDATE_VIEW = 0x400,
    CAMERA_MANUAL_LOCKON = 0x1000,
    CAMERA_MANUAL_ROTATING = 0x4000000
};
enum TornadoEntryModeHD { ENTRY_NORMAL, ENTRY_SIDE_EYE, ENTRY_EVENT };
}

namespace camera_engine2 {
struct TornadoStyleViewHD {
    u8 mPadding00[0x80];
    be<u16> mFlags;
    u8 mPadding82[2];
};
WWHD_OFFSET(TornadoStyleViewHD, mFlags, 0x80);
WWHD_SIZE(TornadoStyleViewHD, 0x84);
}

namespace camera_engine2 {
// HD: GameCube tower/ride bodies are undecompiled; work members describe the
// verified algorithms.
struct TowerWorkHD {
  /* 0x00 */ be<u32> mTag;              /* 'TOWR' */
  /* 0x04 */ be<s32> mTransitionFrames; /* transition frames */
  /* 0x08 */ be<f32> mRemainingWeight;  /* remaining weight sum */
  /* 0x0C */ be<f32> mFrameWeight;
  /* 0x10 */ be<s32> mIdleFrames; /* frames since free movement */
  /* 0x14 */ be<s32> mFallFrames;
  /* 0x18 */ be<f32> mMovementBlend;
  /* 0x1C */ be<f32> mMinimumRadius; /* min radius */
  /* 0x20 */ be<f32> mMaximumRadius; /* max radius */
  /* 0x24 */ be<f32> mLatitude;      /* latitude (deg) */
  /* 0x28 */ cSAngle_l mLongitude;   /* longitude */
  /* 0x2A */ u8 _2A[2];
  /* 0x2C */ be<f32> mInitialFovy;
  /* 0x30 */ be<f32> mCenterOffsetX; /* centre offset x */
  /* 0x34 */ be<s32> mStateCounter;
  /* 0x38 */ be<f32> mHeightBlend;
  /* 0x3C */ be<f32> mInitialLatitude;
  /* 0x40 */ cXyz mCenter;        /* smoothed centre */
  /* 0x4C */ cXyz mTowerPosition; /* tower (map arrow) position */
  /* 0x58 */ be<s32> mYawRangeSide;
  /* 0x5C */ be<u8> mInitialized;
  /* 0x5D */ be<u8> mLookLeft;
  /* 0x5E */ u8 _5E[2];
  /* 0x60 */ be<f32> mRadiusRate;
  /* 0x64 */ be<f32> mLatitudeRate;
  /* 0x68 */ be<f32> mLongitudeRate;
  /* 0x6C */ be<f32> mFovyRate;
  /* 0x70 */ be<f32> mCenterFollowXZ;
  /* 0x74 */ be<f32> mCenterFollowY;
};
WWHD_SIZE(TowerWorkHD, 0x78);

struct RideWorkHD {
  /* 0x00 */ be<u32> mTag;            /* 'RIDE' */
  /* 0x04 */ gptr<fopAc_ac_c> mpShip; /* ship (actor 0xA5) */
  /* 0x08 */ be<u32> mEntryViewIndex; /* m07C at start */
  /* 0x0C */ be<u8> mSteeringLeft;    /* steering left */
  /* 0x0D */ be<u8> mBoostBlurArmed;  /* boost blur armed */
  /* 0x0E */ u8 _0E[2];
  /* 0x10 */ be<s32> mEntryMode; /* entry: 0 normal, 1 side eye, 2 event */
  /* 0x14 */ cXyz mSideEye;      /* side eye */
  /* 0x20 */ cXyz mStickCenterOffset;
  /* 0x2C */ cXyz mCenterTarget;     /* centre target */
  /* 0x38 */ cSAngle_l mShipHeading; /* ship heading */
  /* 0x3A */ u8 _3A[2];
  /* 0x3C */ be<f32> mRadius;             /* radius */
  /* 0x40 */ be<f32> mRadiusSpeedScale;   /* radius speed scale */
  /* 0x44 */ be<f32> mLatitude;           /* latitude */
  /* 0x48 */ be<f32> mLatitudeSpeedScale; /* latitude speed scale */
  /* 0x4C */ be<f32> mSideOffsetRatio;    /* side offset ratio */
  /* 0x50 */ be<f32> mCenterFollowRatio;  /* centre follow ratio */
};
WWHD_SIZE(RideWorkHD, 0x54);
WWHD_OFFSET(TowerWorkHD, mCenter, 0x40);
WWHD_OFFSET(TowerWorkHD, mTowerPosition, 0x4C);
WWHD_OFFSET(RideWorkHD, mShipHeading, 0x38);
WWHD_OFFSET(RideWorkHD, mCenterFollowRatio, 0x50);
struct RideEyeTableHD {
  be<f32> mWords[12];
};
WWHD_SIZE(RideEyeTableHD, 0x30);
struct TowerSetupHD {
  u8 mPadding00[0xAC];
  be<f32> mChargeLatitude;
};
WWHD_OFFSET(TowerSetupHD, mChargeLatitude, 0xAC);
WWHD_SIZE(TowerSetupHD, 0xB0);
struct TowerArrowHD {
  cXyz mPosition;
  u8 mRest[8];
};
WWHD_SIZE(TowerArrowHD, 0x14);
struct RideStyleHD {
  be<u32> mName;
  u8 mPadding04[0x7C];
  be<u16> mFlags;
  u8 mPadding82[2];
};
WWHD_OFFSET(RideStyleHD, mFlags, 0x80);
WWHD_SIZE(RideStyleHD, 0x84);
struct RideShipHD : fopAc_ac_c {
  u8 mPadding3AC[0x644 - 0x3AC];
  be<u32> mStateFlags;
  u8 mPadding648[4];
  be<s16> mSideAngle;
  u8 mPadding64E[0x67C - 0x64E];
  be<s16> mHeadingOffset, mPitchOffset, mRideAngle;
};
WWHD_OFFSET(RideShipHD, mStateFlags, 0x644);
WWHD_OFFSET(RideShipHD, mHeadingOffset, 0x67C);
WWHD_OFFSET(RideShipHD, mRideAngle, 0x680);
WWHD_SIZE(RideShipHD, 0x682);
struct RidePlayHD {
  u8 mPadding0000[0x5D2C];
  be<u8> mBoostStatus;
};
WWHD_OFFSET(RidePlayHD, mBoostStatus, 0x5D2C);
WWHD_SIZE(RidePlayHD, 0x5D2D);
enum : u32 { TOWER_WORK = 0x544F5752, RIDE_WORK = 0x52494445 };
// HD: references to shared angle/vector constants whose mapped definitions are
// owned by camera talk/proc.
inline cSAngle_l *cameraRideRightAngle() {
  return gabi::at<cSAngle_l>(0x101FF358);
}
inline cXyz *cameraRideZeroVector() { return gabi::at<cXyz>(0x101FFBA8); }
inline RideStyleHD *cameraRideStyle(s32 style) {
  return gabi::at<RideStyleHD>(CAM_STYLES + style * sizeof(RideStyleHD));
}
inline RidePlayHD *cameraRidePlay() {
  return gabi::at<RidePlayHD>(dComIfGp_ea());
}
} // namespace camera_engine2
void cameraRideScaleAngleAssign(cSAngle_l *angle, f32 scale);
void cameraTowerMoveBackground(dBgS *background, cBgS_PolyInfo *check, u32 mode,
                               cXyz *position, u32 angle, u32 shape);
void cameraRideSetBlurScale(dCamera_c *camera, f32 scale);
namespace camera_engine2 {
enum : u32 {
  CAMERA_FORCE_FOLLOW = 0x100000,
  RIDE_STYLE_BE07 = 0x42453037,
  RIDE_STYLE_BE08 = 0x42453038
};
enum : s16 { RIDE_SHIP_PROCESS = 0xA5 };
} // namespace camera_engine2
namespace camera_engine2 {
// HD: callback identity passed to the actor iterator, not invoked by this translation unit.
enum : u32 { RIDE_JUDGE_FOR_PROCESS_NAME = 0x025E121C };
}
