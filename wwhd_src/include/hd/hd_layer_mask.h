/* HD 3D letterbox mask layer and its render callback. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdLayerMask_c {
    u8 _00[0xC];
    be<u32> mVtable;
    be<u32> mDrawVtable;
    u8 _14[0x610-0x14];
    void initialize();

};
WWHD_OFFSET(hdLayerMask_c, mVtable, 0xC);
WWHD_OFFSET(hdLayerMask_c, mDrawVtable, 0x10);
WWHD_SIZE(hdLayerMask_c, 0x610);
struct hdMaskRenderStep_c {
    u8 _00[0xC];
    be<u32> mVtable;
    SafeString mName;
    be<u32> mTarget;
    be<u32> mFlags;
    be<u32> mDelegateVtable;
    gptr<hdLayerMask_c> mpLayer;
    be<u32> mDelegateAdjustment;
    be<u32> mDrawFunction;
    u8 _30[0x10];
};
WWHD_OFFSET(hdMaskRenderStep_c, mName, 0x10);
WWHD_OFFSET(hdMaskRenderStep_c, mTarget, 0x18);
WWHD_OFFSET(hdMaskRenderStep_c, mFlags, 0x1C);
WWHD_OFFSET(hdMaskRenderStep_c, mDelegateVtable, 0x20);
WWHD_OFFSET(hdMaskRenderStep_c, mpLayer, 0x24);
WWHD_OFFSET(hdMaskRenderStep_c, mDelegateAdjustment, 0x28);
WWHD_OFFSET(hdMaskRenderStep_c, mDrawFunction, 0x2C);
WWHD_SIZE(hdMaskRenderStep_c, 0x40);

struct hdMaskRenderInfo_c { be<u32> mTarget; be<u32> mScreen; };
WWHD_SIZE(hdMaskRenderInfo_c,8);
struct hdMaskPlay_l { u8 _00[0x5AE8]; be<f32> mMaskHeight; u8 _5AEC[12]; gptr<void> mpMask; };
WWHD_OFFSET(hdMaskPlay_l,mMaskHeight,0x5AE8);
WWHD_OFFSET(hdMaskPlay_l,mpMask,0x5AF8);
struct hdMaskViewport_l { be<f32> mExtent; u8 _04[0xB8]; };
WWHD_OFFSET(hdMaskViewport_l,mExtent,0);
WWHD_SIZE(hdMaskViewport_l,0xBC);
struct hdMaskScreen_l { u8 _00[0x744]; hdMaskViewport_l mViewport; };
WWHD_SIZE(hdMaskScreen_l,0x800);
struct hdMaskScreenMgr_l { u8 _00[0x18]; hdMaskScreen_l mScreens[2]; };
WWHD_OFFSET(hdMaskScreenMgr_l,mScreens,0x18);
struct hdMaskBounds_c { be<f32> mLeft,mBottom,mRight,mTop; u8 _10[8]; };
WWHD_SIZE(hdMaskBounds_c,0x18);
struct hdMaskState_c { be<u8> mEnabled,mBlend; u8 _02[2]; be<u32> mDepthMode; u8 _08[0x74-8]; };
WWHD_SIZE(hdMaskState_c,0x74);
struct hdMaskProjection_c { u8 mData[0xAC]; };
struct hdMaskCamera_c { u8 mData[0x58]; };
struct hdMaskMatrix_c { be<u32> mWords[12]; };
WWHD_SIZE(hdMaskProjection_c,0xAC);
WWHD_SIZE(hdMaskCamera_c,0x58);
WWHD_SIZE(hdMaskMatrix_c,0x30);

struct hdMaskLayerContext_c { u8 _00[8]; gptr<hdMaskViewport_l> mpViewport; u8 _0C[0x1C-0xC]; gptr<hdMaskBounds_c> mpBounds; };
WWHD_OFFSET(hdMaskLayerContext_c,mpViewport,8);
WWHD_OFFSET(hdMaskLayerContext_c,mpBounds,0x1C);

struct hdLayerMaskHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdLayerMaskHeaderZero_c,16);
struct hdLayerMaskHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdLayerMaskHeaderDescriptor_c,12);
