/* HD error-viewer layer and its render callback. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdErrorViewer_c {
    u8 _00[0xC];
    be<u32> mVtable;
    be<u32> mDrawVtable;
    u8 _14[0x610-0x14];
    void initialize();
};
WWHD_OFFSET(hdErrorViewer_c, mVtable, 0xC);
WWHD_OFFSET(hdErrorViewer_c, mDrawVtable, 0x10);
WWHD_SIZE(hdErrorViewer_c, 0x610);
struct hdErrorViewerProjection_l { u8 mData[0xAC]; };
struct hdErrorViewerPerspective_l { u8 mData[0xB8]; };
struct hdErrorViewerCamera_l { u8 mData[0x60]; };
struct hdErrorViewerLayer3D_c {
    hdErrorViewer_c mBase;
    hdErrorViewerProjection_l mProjection;
    hdErrorViewerProjection_l mDrcProjection;
    hdErrorViewerPerspective_l mPerspective;
    hdErrorViewerCamera_l mCamera;
};
WWHD_OFFSET(hdErrorViewerLayer3D_c, mProjection, 0x610);
WWHD_OFFSET(hdErrorViewerLayer3D_c, mDrcProjection, 0x6BC);
WWHD_OFFSET(hdErrorViewerLayer3D_c, mPerspective, 0x768);
WWHD_OFFSET(hdErrorViewerLayer3D_c, mCamera, 0x820);
struct hdErrorRenderStep_c {
    u8 _00[0xC];
    be<u32> mVtable;
    SafeString mName;
    be<u32> mTarget;
    be<u32> mFlags;
    be<u32> mDelegateVtable;
    gptr<hdErrorViewer_c> mpLayer;
    be<u32> mDelegateAdjustment;
    be<u32> mDrawFunction;
    u8 _30[0x10];
};
WWHD_OFFSET(hdErrorRenderStep_c, mName, 0x10);
WWHD_OFFSET(hdErrorRenderStep_c, mTarget, 0x18);
WWHD_OFFSET(hdErrorRenderStep_c, mFlags, 0x1C);
WWHD_OFFSET(hdErrorRenderStep_c, mDelegateVtable, 0x20);
WWHD_OFFSET(hdErrorRenderStep_c, mpLayer, 0x24);
WWHD_OFFSET(hdErrorRenderStep_c, mDelegateAdjustment, 0x28);
WWHD_OFFSET(hdErrorRenderStep_c, mDrawFunction, 0x2C);
WWHD_SIZE(hdErrorRenderStep_c, 0x40);

struct hdErrorViewerHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdErrorViewerHeaderZero_c,16);
struct hdErrorViewerHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdErrorViewerHeaderDescriptor_c,12);
