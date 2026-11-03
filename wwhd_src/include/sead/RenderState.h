// WWHD rendering state layouts reconstructed from the local game instructions.
#pragma once
#include "wwhd.h"
namespace sead {
struct BlendState {
    be<u32> mColorSource, mAlphaSource, mColorDestination, mAlphaDestination;
    be<u32> mColorEquation, mAlphaEquation;
    static BlendState* create(BlendState* object);
};
struct RasterState {
    be<u32> mFrontMode, mBackMode;
};
struct RasterModeTable { RasterState mEntries[4]; };
struct CompactRenderState {
    be<u8> mDepthTest, mDepthWrite;
    u8 mDepthPadding[2];
    be<u32> mDepthComparison, mCullMode;
    be<u8> mBlendEnable;
    u8 mBlendPadding[3];
    BlendState mBlend;
    be<f32> mBlendRed, mBlendGreen, mBlendBlue, mBlendAlpha;
    be<u8> mAlphaTest;
    u8 mAlphaPadding[3];
    be<u32> mAlphaComparison;
    be<f32> mAlphaReference;
    be<u8> mWriteRed, mWriteGreen, mWriteBlue, mWriteAlpha;
    be<u8> mStencilEnable;
    u8 mStencilPadding[3];
    be<u32> mStencilComparison, mStencilReference, mStencilMask;
    be<u32> mStencilFail, mStencilDepthFail, mStencilPass;
    be<u32> mFrontPolygon, mBackPolygon;
    be<u8> mPolygonOffset, mLineOffset, mPointOffset;
    u8 mPolygonPadding;
    gptr<void> mTable;
    static CompactRenderState* create(CompactRenderState* object);
    void apply();
    void applyDepthStencil();
    void applyChannelMask();
    void applyBlend();
    void applyPolygon();
};
struct RenderState {
    be<u8> mDepthTest, mDepthWrite;
    u8 mDepthPadding[2];
    be<u32> mDepthComparison, mCullMode, mBlendMask;
    BlendState mBlend[8];
    be<f32> mBlendRed, mBlendGreen, mBlendBlue, mBlendAlpha;
    be<u8> mAlphaTest;
    u8 mAlphaPadding[3];
    be<u32> mAlphaComparison;
    be<f32> mAlphaReference;
    be<u32> mChannelMask;
    be<u8> mStencilEnable;
    u8 mStencilPadding[3];
    be<u32> mStencilComparison, mStencilReference, mStencilMask;
    be<u32> mStencilFail, mStencilDepthFail, mStencilPass;
    be<u32> mFrontPolygon, mBackPolygon;
    be<u8> mPolygonOffset, mLineOffset, mPointOffset;
    u8 mPolygonPadding;
    gptr<void> mTable;
    static RenderState* create(RenderState* object);
    void apply();
    void applyAlpha();
    void applyDepthStencil();
    void applyChannelMask();
    void applyBlend();
    void applyPolygon();
};
}
WWHD_OFFSET(sead::BlendState,mColorSource,0);
WWHD_OFFSET(sead::BlendState,mAlphaEquation,0x14);
WWHD_SIZE(sead::BlendState,0x18);
WWHD_OFFSET(sead::RasterState,mFrontMode,0);
WWHD_OFFSET(sead::RasterState,mBackMode,4);
WWHD_OFFSET(sead::RasterModeTable,mEntries,0);
WWHD_SIZE(sead::RasterModeTable,0x20);
WWHD_OFFSET(sead::CompactRenderState,mBlend,0x10);
WWHD_OFFSET(sead::CompactRenderState,mBlendRed,0x28);
WWHD_OFFSET(sead::CompactRenderState,mStencilEnable,0x48);
WWHD_OFFSET(sead::CompactRenderState,mTable,0x70);
WWHD_SIZE(sead::CompactRenderState,0x74);
WWHD_OFFSET(sead::RenderState,mBlend,0x10);
WWHD_OFFSET(sead::RenderState,mBlendRed,0xD0);
WWHD_OFFSET(sead::RenderState,mStencilEnable,0xF0);
WWHD_OFFSET(sead::RenderState,mTable,0x118);
WWHD_SIZE(sead::RenderState,0x11C);
