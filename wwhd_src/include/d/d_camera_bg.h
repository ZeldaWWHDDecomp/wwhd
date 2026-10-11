#pragma once
#include "d/d_camera.h"

// HD: private query views preserve the exact stack storage used by the camera.
namespace CameraBackgroundHD {
struct LineCheck {
    cBgS_LinChk mLine;
    dBgS_PolyPassChk mPolyPass;
    dBgS_GrpPassChk mGroupPass;
};
WWHD_SIZE(LineCheck, 0x6C);
struct RoofCheck {
    dBgS_RoofChk mRoof;
    u8 mTail[4];
};
WWHD_SIZE(RoofCheck, 0x50);
struct SphereCheck {
    cXyz mCenter;
    be<f32> mRadius;
    be<u32> mSphereVtable;
    cBgS_PolyInfo mPolyInfo;
    cBgS_Chk mCheck;
    dBgS_PolyPassChk mPolyPass;
    dBgS_GrpPassChk mGroupPass;
    be<u32> mCallback;
};
WWHD_SIZE(SphereCheck, 0x50);
WWHD_OFFSET(SphereCheck, mCheck, 0x24);
WWHD_OFFSET(SphereCheck, mPolyPass, 0x38);
struct ForwardProbe {
    be<f32> mDistance, mCheckAngle, mWeightHigh, mWeightLow;
};
struct Setup {
    u8 mPrefix[0xC0];
    be<f32> mMovingShipScale;
    u8 mReserved[4];
    be<u32> mBushHigh, mBushLow;
    u8 mBGPrefix[0x108 - 0xD0];
    be<f32> mFloorMargin;
    ForwardProbe mForward[2];
    be<f32> mForwardBackMargin;
    u8 mGap[8];
    be<f32> mGazeBackMargin, mCornerCushion, mWallCushion, mWallUpDistance;
    be<f32> mWallBackCushion, mCornerAngleMax;
    u8 mTail[0x168 - 0x150];
};
WWHD_SIZE(Setup, 0x168);
WWHD_OFFSET(Setup, mMovingShipScale, 0xC0);
WWHD_OFFSET(Setup, mBushHigh, 0xC8);
WWHD_OFFSET(Setup, mForward, 0x10C);
WWHD_OFFSET(Setup, mGazeBackMargin, 0x138);
struct TrimSizes {
    be<u8> mSize[4];
};
struct Registers {
    be<f32> mFloat[0x1178 / sizeof(f32)];
};
struct PlayerVtable {
    u8 mPrefix[0xBC];
    be<u32> getGrabActorID;
};
struct Parameter {
    be<u32> mStyleIndex;
    gptr<u8> mStyle;
    be<u32> mReserved;
};
struct Style {
    u8 mPrefix[4];
    be<u32> mAlgorithm;
};
struct GroundFrame {
    u8 mPrefix[0x48];
    dBgS_CamGndChk mGround;
    u8 mTail[0x1E0 - 0x48 - 0x54];
};
WWHD_SIZE(GroundFrame, 0x1E0);
} // namespace CameraBackgroundHD

// HD: shared camera constants are image-backed references owned by d_camera_proc.cpp.
extern cXyz& sZeroVector;
extern be<s16>& sZeroAngle;
