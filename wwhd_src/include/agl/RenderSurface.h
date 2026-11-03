/* WWHD render-surface wrappers. SDK record offsets are public wut interface facts;
 * the library prefix and wrapper offsets come from this unit's instructions. */
#pragma once
#include "sead/Foundation.h"
namespace agl {
struct PlatformSurface_l {
    be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAntiAlias, mUsage;
    be<u32> mImageSize;
    gptr<void> mImage;
    be<u32> mMipmapSize;
    gptr<void> mMipmaps;
    be<u32> mTileMode, mSwizzle, mAlignment, mPitch;
    be<u32> mMipOffsets[13];
};
struct PlatformColorBuffer_l {
    PlatformSurface_l mSurface;
    be<u32> mViewMip, mViewFirstSlice, mViewNumSlices;
    gptr<void> mAuxBuffer;
    be<u32> mAuxSize;
    be<u32> mRegisters[5];
};
struct PlatformDepthBuffer_l {
    PlatformSurface_l mSurface;
    be<u32> mViewMip, mViewFirstSlice, mViewNumSlices;
    gptr<void> mHiZBuffer;
    be<u32> mHiZSize;
    be<f32> mDepthClear;
    be<u32> mStencilClear;
    be<u32> mRegisters[7];
};
struct SurfaceData_l {
    // Constructor-owned TextureData prefix: AGL#15 declaration, pending shared fold.
    be<u32> mFormat;
    PlatformSurface_l mSurface;
    be<u8> mMinimumWidth, mMinimumHeight, mMinimumDepth, mMaximumMipCount;
    be<u32> mComponentMapping[4];
    gptr<const void> mMethods;
};
struct SurfaceView_l : SurfaceData_l {
    be<u8> mDirty;
    u8 mPadding[3];
    be<u32> mViewMip;
    be<u32> mViewFirstSlice;
};
struct ColorSurface {
    SurfaceView_l mBase;
    be<u8> mHasAux;
    u8 mPadding[3];
    be<u32> mAuxSize;
    be<u32> mAuxAlignment;
    gptr<void> mAuxBuffer;
    PlatformColorBuffer_l mColor;
    ColorSurface* construct();
    void calculateAuxiliary();
    void invalidate();
    void initializeRegisters();
};
struct DepthSurface {
    SurfaceView_l mBase;
    be<u32> mHiZSize;
    be<u32> mHiZAlignment;
    gptr<void> mHiZBuffer;
    PlatformDepthBuffer_l mDepth;
    DepthSurface* construct();
    void calculateHiZ();
    void invalidate();
    void initializeRegisters();
};
WWHD_OFFSET(PlatformSurface_l,mUsage,0x1C);
WWHD_OFFSET(PlatformSurface_l,mImageSize,0x20);
WWHD_OFFSET(PlatformSurface_l,mImage,0x24);
WWHD_OFFSET(PlatformSurface_l,mMipmapSize,0x28);
WWHD_OFFSET(PlatformSurface_l,mMipmaps,0x2C);
WWHD_OFFSET(PlatformSurface_l,mTileMode,0x30);
WWHD_OFFSET(PlatformSurface_l,mMipOffsets,0x40);
WWHD_SIZE(PlatformSurface_l,0x74);
WWHD_OFFSET(PlatformColorBuffer_l,mViewMip,0x74);
WWHD_OFFSET(PlatformColorBuffer_l,mAuxBuffer,0x80);
WWHD_OFFSET(PlatformColorBuffer_l,mRegisters,0x88);
WWHD_SIZE(PlatformColorBuffer_l,0x9C);
WWHD_OFFSET(PlatformDepthBuffer_l,mViewMip,0x74);
WWHD_OFFSET(PlatformDepthBuffer_l,mHiZBuffer,0x80);
WWHD_OFFSET(PlatformDepthBuffer_l,mDepthClear,0x88);
WWHD_OFFSET(PlatformDepthBuffer_l,mRegisters,0x90);
WWHD_SIZE(PlatformDepthBuffer_l,0xAC);
WWHD_OFFSET(SurfaceData_l,mFormat,0);
WWHD_OFFSET(SurfaceData_l,mSurface,4);
WWHD_OFFSET(SurfaceData_l,mMinimumWidth,0x78);
WWHD_OFFSET(SurfaceData_l,mComponentMapping,0x7C);
WWHD_OFFSET(SurfaceData_l,mMethods,0x8C);
WWHD_SIZE(SurfaceData_l,0x90);
WWHD_OFFSET(SurfaceView_l,mDirty,0x90);
WWHD_OFFSET(SurfaceView_l,mViewMip,0x94);
WWHD_OFFSET(SurfaceView_l,mViewFirstSlice,0x98);
WWHD_SIZE(SurfaceView_l,0x9C);
WWHD_OFFSET(ColorSurface,mHasAux,0x9C);
WWHD_OFFSET(ColorSurface,mAuxSize,0xA0);
WWHD_OFFSET(ColorSurface,mAuxAlignment,0xA4);
WWHD_OFFSET(ColorSurface,mAuxBuffer,0xA8);
WWHD_OFFSET(ColorSurface,mColor,0xAC);
WWHD_SIZE(ColorSurface,0x148);
WWHD_OFFSET(DepthSurface,mHiZSize,0x9C);
WWHD_OFFSET(DepthSurface,mHiZAlignment,0xA0);
WWHD_OFFSET(DepthSurface,mHiZBuffer,0xA4);
WWHD_OFFSET(DepthSurface,mDepth,0xA8);
WWHD_SIZE(DepthSurface,0x154);
}
