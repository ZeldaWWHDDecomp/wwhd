// WWHD display and scan-buffer layouts, derived from local game instructions.
#pragma once
#include "wwhd.h"
#include "sead/Allocation.h"
#include "sead/RenderMath.h"
namespace sead {
struct ColorBuffer_l { be<u32> mDimension, mWidth, mHeight; };
struct DisplayFrame {
    u8 mSurfaceFields[0x1C];
    gptr<ColorBuffer_l> mColor;
    gptr<void> mDepth;
    void clear(u32 flags, const RenderQuaternion* color, u32 stencil, f32 depth);
    void copyToScan(void* display);
    void bind();
};
struct ScanTarget {
    be<f32> mWidth, mHeight;
    gptr<void> mTable;
    be<u32> mTarget, mProgressive;
    gptr<void> mBuffer;
    static ScanTarget* create(ScanTarget* object, u32 target, u32 progressive);
    void allocate(AllocationHeap* heap);
    void configure(u32 format);
};
struct DeviceStateCache_l {
    u8 mBase[0x5C];
    be<u32> mRenderMode;
    u8 mDriverState[0x120 - 0x60];
    be<u32> mDepthComparison, mDepthTest, mDepthWrite, mStencilEnable, mAlphaComparison;
    be<f32> mAlphaReference;
    gptr<void> mContext;
    be<u32> mBlendMask, mChannelMask, mBlendEquation, mCullMode;
    be<u32> mFrontPolygon, mBackPolygon, mPolygonOffset, mLineOffset, mPointOffset;
    void initialize();
};
}
WWHD_OFFSET(sead::DisplayFrame,mColor,0x1C);
WWHD_OFFSET(sead::DisplayFrame,mDepth,0x20);
WWHD_SIZE(sead::DisplayFrame,0x24);
WWHD_OFFSET(sead::ScanTarget,mTarget,0xC);
WWHD_OFFSET(sead::ScanTarget,mProgressive,0x10);
WWHD_OFFSET(sead::ScanTarget,mBuffer,0x14);
WWHD_SIZE(sead::ScanTarget,0x18);
WWHD_OFFSET(sead::DeviceStateCache_l,mRenderMode,0x5C);
WWHD_OFFSET(sead::DeviceStateCache_l,mDepthComparison,0x120);
WWHD_OFFSET(sead::DeviceStateCache_l,mContext,0x138);
WWHD_OFFSET(sead::DeviceStateCache_l,mPointOffset,0x15C);
WWHD_SIZE(sead::DeviceStateCache_l,0x160);
