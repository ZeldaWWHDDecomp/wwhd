#pragma once
#include "d/d_camera.h"

// HD: unit-private views describe render state without changing shared camera declarations.
namespace CameraProcessHD {
struct WordVector {
    be<u32> mWord[3];
};
struct WordMatrix {
    be<u32> mWord[12];
};
struct View {
    u8 mProcess[0xCC];
    be<f32> mNear, mFar, mFovy, mAspect;
    struct {
        cXyz mEye, mCenter, mUp;
    } mLookat;
    be<s16> mBank;
    u8 mBankPadding[2];
    be<f32> mProjMtx[4][4];
    Mtx34 mViewMtx, mInvViewMtx;
    be<f32> mProjViewMtx[4][4];
    Mtx34 mViewMtxNoTrans;
};
WWHD_SIZE(View, 0x214);
WWHD_OFFSET(View, mLookat, 0xDC);
WWHD_OFFSET(View, mViewMtxNoTrans, 0x1E4);
struct Process {
    View view;
    u8 mDrawTag[0x1C];
    be<s8> mPrm1, mPrm2, mPrm3;
    u8 mReserved;
    csXyz mAngle;
    u8 mPadding[2];
    be<s32> mField;
    u8 phase_request[8];
    dCamera_c mCamera;
};
WWHD_OFFSET(Process, mCamera, 0x248);
struct RenderLayer {
    u8 mPrefix[0x48];
    be<u32> mCamera, mProjection;
};
struct RenderManager {
    u8 mPrefix[0x1020];
    be<u32> mLayerCount, mLayers;
};
struct LookAtCamera {
    Mtx34 mMatrix;
    be<u32> mVtable;
    cXyz mPos, mAt, mUp;
};
WWHD_SIZE(LookAtCamera, 0x58);
struct Projection {
    be<u8> mDirty;
    u8 mPrefix[0xAF];
    be<f32> mOffsetX, mOffsetY;
    u8 mTail[0xC4 - 0xB8];
};
WWHD_SIZE(Projection, 0xC4);
struct Zoom {
    be<f32> mScale, mTarget;
};
struct Setup {
    u8 mPrefix[0x68];
    be<f32> mVistaTrimHeight, mCinemaScopeTrimHeight;
    u8 mBGPrefix[0x144 - 0x70];
    be<u32> mWallUpDistance;
    u8 mTail[0x168 - 0x148];
};
WWHD_SIZE(Setup, 0x168);
struct BG {
    dCamera_c::BG mChecks;
    // HD: this generated constructor also initializes the following work fields.
    u8 mPadding[0xDC - sizeof(dCamera_c::BG)];
    cSAngle_l mFirstAngle, mSecondAngle;
    u8 mAlignment[4];
    cSGlobe_l mFirstGlobe, mSecondGlobe;
    u8 mTail[0x11C - 0xF4];
};
WWHD_SIZE(BG, 0x11C);
struct SafeString {
    be<u32> top, vtable;
};
struct SafeStringVtable {
    u8 mPrefix[0x14];
    be<u32> terminate;
};
struct Stage {
    be<u32> mVtable;
};
struct StageVtable {
    u8 mPrefix[0x15C];
    be<u32> getStagInfo;
};
struct StageInfo {
    be<f32> mNearPlane, mFarPlane;
    be<u8> mCameraMapToolID;
    u8 mPadding[3];
    be<u32> mFlags;
    u8 mReserved[2];
    be<u16> mCullPoint;
};
struct TalkTable {
    be<u32> mPrimary, mSecondary, mEntries[10];
};
struct DrawView {
    be<u32> mWindow, mViewport, mView;
};
struct DemoCamera {
    be<u32> mVtable;
    be<u8> mEnable;
    u8 mPadding[0x10 - 5];
    be<f32> mFovy;
    u8 mReserved[4];
    cXyz mTrans, mUp, mTarget;
    be<f32> mRoll;
};
WWHD_OFFSET(DemoCamera, mTarget, 0x30);
struct CameraVtable {
    u8 mPrefix[0x24];
    be<u32> doUpdateMatrix;
};
struct DisplaySize {
    be<f32> mWidth, mHeight;
};
struct SmoothState {
    be<s32> mFrame;
    be<s16> mBankDelta;
};
} // namespace CameraProcessHD
