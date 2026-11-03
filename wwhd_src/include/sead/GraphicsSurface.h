#pragma once
#include "wwhd.h"
namespace sead {
struct GraphicsSurface {
    be<u32> mDimension,mWidth,mHeight,mDepth,mMipCount,mFormat,mSamples,mUsage;
    be<u32> mImageBytes; gptr<void> mImage;
    be<u32> mMipBytes; gptr<void> mMipImage;
    be<u32> mTileMode,mSwizzle,mAlignment,mPitch,mMipOffsets[13];
};
struct GraphicsColorBuffer {
    GraphicsSurface mSurface;
    be<u32> mViewMip,mFirstSlice,mSliceCount;
    gptr<void> mAuxiliary; be<u32> mAuxiliaryBytes,mRegisters[5];
};
struct GraphicsDepthBuffer {
    GraphicsSurface mSurface;
    be<u32> mViewMip,mFirstSlice,mSliceCount;
    gptr<void> mHiZ; be<u32> mHiZBytes;
    be<f32> mDepthClear; be<u32> mStencilClear,mRegisters[7];
};
WWHD_SIZE(GraphicsSurface,0x74);
WWHD_OFFSET(GraphicsSurface,mImageBytes,0x20);
WWHD_OFFSET(GraphicsSurface,mImage,0x24);
WWHD_OFFSET(GraphicsSurface,mTileMode,0x30);
WWHD_OFFSET(GraphicsSurface,mAlignment,0x38);
WWHD_SIZE(GraphicsColorBuffer,0x9C);
WWHD_OFFSET(GraphicsColorBuffer,mViewMip,0x74);
WWHD_OFFSET(GraphicsColorBuffer,mAuxiliary,0x80);
WWHD_SIZE(GraphicsDepthBuffer,0xAC);
WWHD_OFFSET(GraphicsDepthBuffer,mHiZ,0x80);
WWHD_OFFSET(GraphicsDepthBuffer,mDepthClear,0x88);
}
