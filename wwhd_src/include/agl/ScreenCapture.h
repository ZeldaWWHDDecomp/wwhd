// Screen capture fields observed in the assigned WWHD routines.
#pragma once
#include "sead/FileDevice.h"
#include "sead/Viewport.h"
#include "sead/GraphicsSurface.h"
namespace agl {
struct CaptureSurfaceDescriptor {
    // Borrowed AGL15 texture prefix, with the merged SEAD11 SDK descriptor.
    // The0x7C prefix does not assert the owner's complete texture allocation.
    be<u32> mTable;
    sead::GraphicsSurface mSurface;
    be<u8> mWidthFloor, mHeightFloor;
    u8 mReserved7A[2];
};
struct CaptureSourcePrefix { u8 mBeforeSurface[0x1C]; gptr<CaptureSurfaceDescriptor> mSurface; };
struct CaptureCachedSurface {
    CaptureSurfaceDescriptor mDescriptor;
    u8 mBeforeImageMirror[0x58];
    be<u32> mImageMirror;
    be<u32> mReservedD8;
    be<u32> mMipImageMirror;
    u8 mRemaining[0xB8];
};
struct CaptureNotice {
    be<u32> mContext;
    sead::SafeText mPath;
    be<f32> mScale;
    be<u32> mWidth, mHeight, mOptions;
    be<u8> mMode;
    u8 mPadding[3];
};
struct CaptureNoticeTable { u8 mBeforeInvoke[12]; be<u32> mInvoke; };
struct CaptureNoticeCallback { gptr<CaptureNoticeTable> mTable; };
// Observed prefix only: allocation extent and private embedded rendering layouts
// beyond the members used here remain owned by their respective library units.
struct ScreenCapture {
    sead::Disposer mDisposer;
    be<u32> mReserved10;
    gptr<void> mOwnerHeap;
    be<u8> mFirstPending, mSecondPending, mBusy;
    u8 mPadding1B;
    be<u32> mFirstOptions, mSecondOptions;
    be<f32> mScale;
    be<u8> mOutputKind; u8 mPadding29[3];
    gptr<void> mFirstStorage, mSecondStorage;
    u8 mBeforeFirstPath[0x114];
    sead::PathText mFirstPath;
    sead::PathText mSecondPath;
    be<u8> mTransferMode; u8 mPadding361[3];
    CaptureCachedSurface mSurface;
    sead::RenderSurface mViewportSurface;
    u8 mBeforeTarget[0x28];
    u8 mTarget[0x148];
    gptr<CaptureSourcePrefix> mFirstSource, mSecondSource;
    gptr<CaptureNoticeCallback> mCallback;
    u8 mBeforeCallbackContext[0x24];
    be<u32> mCallbackContext;
    void clearBusy();
    void captureFirst();
    void captureSecond();
    u32 render(CaptureSourcePrefix* source, u32 options, sead::SafeText* path, void* storage);
};
namespace screenshot_text {
// Compiler-kept file handle destructor for this capture translation unit.
struct FileHandle : sead::FileHandle { void destroy(u32 flags); };
}
}
WWHD_SIZE(agl::screenshot_text::FileHandle, sizeof(sead::FileHandle));
WWHD_OFFSET(agl::CaptureSurfaceDescriptor,mSurface,4);
WWHD_OFFSET(agl::CaptureSurfaceDescriptor,mWidthFloor,0x78);
WWHD_SIZE(agl::CaptureSurfaceDescriptor,0x7C);
WWHD_OFFSET(agl::CaptureCachedSurface,mImageMirror,0xD4);
WWHD_OFFSET(agl::CaptureCachedSurface,mMipImageMirror,0xDC);
WWHD_SIZE(agl::CaptureCachedSurface,0x198);
WWHD_SIZE(agl::CaptureNotice,32);
WWHD_OFFSET(agl::ScreenCapture,mOwnerHeap,0x14);
WWHD_OFFSET(agl::ScreenCapture,mFirstPending,0x18);
WWHD_OFFSET(agl::ScreenCapture,mBusy,0x1A);
WWHD_OFFSET(agl::ScreenCapture,mFirstStorage,0x2C);
WWHD_OFFSET(agl::ScreenCapture,mFirstPath,0x148);
WWHD_OFFSET(agl::ScreenCapture,mSecondPath,0x254);
WWHD_OFFSET(agl::ScreenCapture,mSurface,0x364);
WWHD_OFFSET(agl::ScreenCapture,mViewportSurface,0x4FC);
WWHD_OFFSET(agl::ScreenCapture,mTarget,0x53C);
WWHD_OFFSET(agl::ScreenCapture,mFirstSource,0x684);
WWHD_OFFSET(agl::ScreenCapture,mCallback,0x68C);
WWHD_OFFSET(agl::ScreenCapture,mCallbackContext,0x6B4);
WWHD_SIZE(agl::ScreenCapture,0x6B8);
