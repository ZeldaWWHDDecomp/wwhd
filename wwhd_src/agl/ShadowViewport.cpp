/* agl shadow map: per-layer frame preparation (clear, viewport, scissor), the aligned extent, and the
 * configuration of the depth sampler. */
// GX2 depth-buffer values: wut facts.
#include "agl/ShadowMap.h"
#include "agl/RenderTarget.h"
#include "sead/DisplayTargets.h"
#include "sead/ColorChannels.h"
#include "sead/GraphicsRuntime.h"
#include <atomic>
using namespace agl;
// The library's default clear colour (borrowed, read only).
static const sead::ColorChannels sShadowClearColor;
// RenderTarget.cpp's file-local clear of a render buffer's colour/depth attachment.
void clearRenderTarget(RenderBuffer* target, u32 index, u32 flags, const void* color, u32 stencil, f32 depth);
/* 027967A0 */
void agl::ShadowMap::setExtent(const ShadowMapExtent* extent) {
    mWidth.mValue = ((u32)extent->mWidth + 7) & ~7u;
    mHeight.mValue = ((u32)extent->mHeight + 7) & ~7u;
}
/* 02796B3C */
void agl::ShadowMap::prepareLayer(u32 layer, const sead::RenderVector2* origin, const sead::RenderVector2* extent) {
    gabi::AutoFrame frame{0x02796B3C};
    // GHS reuses this slot for fctiwz transfer and signed conversion; its writes remain observable.
    struct IntToFloatSlot { be<u32> mHigh,mLow; };
    gabi::FrameLocal<IntToFloatSlot> conversion(8);
    auto readEncoded = [&]() -> f64 {
        u64 bits = ((u64)(u32)conversion->mHigh << 32) | (u32)conversion->mLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return encoded;
    };
    constexpr f64 signedBias = 4503601774854144.0;
    if ((u32)mNeedsRebuild != 0) {
        releaseTextures();
        allocateDepthTexture(mLayerCount);
        mNeedsRebuild = 0;
    }
    if ((u32)mDepth.mBase.mViewFirstSlice != layer) {
        mDepth.mBase.mViewFirstSlice = layer;
        mDepth.mBase.mDirty = 1;
    }
    mFrame.clearColors();
    conversion->mLow = (u32)mWidth.mValue ^ 0x80000000u;
    conversion->mHigh = 0x43300000u;
    f64 encodedWidth = readEncoded();
    conversion->mLow = (u32)mHeight.mValue ^ 0x80000000u;
    f64 encodedHeight = readEncoded();
    f32 width = (f32)(encodedWidth - signedBias);
    f32 height = (f32)(encodedHeight - signedBias);
    mFrame.mDepth = &mDepth;
    // The frame's logical size and area, as sead's frame-buffer view reads them.
    sead::RenderSurface& surface = *reinterpret_cast<sead::RenderSurface*>(&mFrame.mBase);
    struct Rect { be<f32> mMinimumX,mMinimumY,mMaximumX,mMaximumY; };
    gabi::FrameLocal<Rect> rectangle(0x10);
    rectangle->mMaximumX = width > 0.0f ? width : 0.0f;
    rectangle->mMinimumX = width > 0.0f ? 0.0f : width;
    if (!(height > 0.0f)) {
        f32 minimumX = rectangle->mMinimumX;
        rectangle->mMaximumY = 0.0f;
        surface.mLeft = minimumX;
        f32 maximumX = rectangle->mMaximumX;
        surface.mBottom = height;
        surface.mRight = maximumX;
        rectangle->mMinimumY = height;
        surface.mTop = 0.0f;
    } else {
        surface.mBottom = 0.0f;
        f32 minimumX = rectangle->mMinimumX;
        f32 maximumX = rectangle->mMaximumX;
        surface.mLeft = minimumX;
        surface.mRight = maximumX;
        rectangle->mMaximumY = height;
        surface.mTop = height;
        rectangle->mMinimumY = 0.0f;
    }
    surface.mWidth = width;
    surface.mHeight = height;
    sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&mFrame));
    gabi::FrameLocal<sead::Viewport> viewport(0x20);
    sead::Viewport::createSurface(viewport.get(), (const sead::RenderVector2*)&surface);
    viewport->apply(&surface);
    clearRenderTarget(&mFrame, 0, 2, &sShadowClearColor, 0, 1.0f);
    if ((u32)mScissor.mValue != 0) {
        f32 margin = mScissorMargin.mValue;
        f32 leftValue = gabi::fmsubs(width, origin->x, margin);
        conversion->mHigh = (u32)gabi::ftoi(leftValue);
        s32 leftInteger = (s32)(u32)conversion->mHigh;
        conversion->mHigh = 0x43300000u;
        conversion->mLow = (u32)leftInteger ^ 0x80000000u;
        f32 roundedLeft = (f32)(readEncoded() - signedBias);
        if (roundedLeft != leftValue && leftValue < 0.0f)
            leftInteger = (s32)((u32)leftInteger - 1);

        f32 bottomValue = gabi::fmsubs(height, origin->y, margin);
        conversion->mLow = (u32)leftInteger ^ 0x80000000u;
        conversion->mHigh = 0x43300000u;
        s32 bottomTruncated = gabi::ftoi(bottomValue);
        f64 encodedLeft = readEncoded();
        conversion->mHigh = (u32)bottomTruncated;
        s32 bottomInteger = (s32)(u32)conversion->mHigh;
        conversion->mHigh = 0x43300000u;
        conversion->mLow = (u32)bottomInteger ^ 0x80000000u;
        f32 roundedBottom = (f32)(readEncoded() - signedBias);
        f32 left = (f32)(encodedLeft - signedBias);
        if (left < 0.0f) left = 0.0f;
        if (roundedBottom != bottomValue && bottomValue < 0.0f)
            bottomInteger = (s32)((u32)bottomInteger - 1);

        f32 doubledMargin = gabi::fadds_ppc(margin, margin);
        f32 rightValue = gabi::fmadds(width, extent->x, doubledMargin);
        conversion->mLow = (u32)bottomInteger ^ 0x80000000u;
        conversion->mHigh = 0x43300000u;
        s32 rightTruncated = gabi::ftoi(rightValue);
        f64 encodedBottom = readEncoded();
        conversion->mHigh = (u32)rightTruncated;
        s32 rightInteger = (s32)(u32)conversion->mHigh;
        conversion->mHigh = 0x43300000u;
        conversion->mLow = (u32)rightInteger ^ 0x80000000u;
        f32 roundedRight = (f32)(readEncoded() - signedBias);
        f32 bottom = (f32)(encodedBottom - signedBias);
        if (bottom < 0.0f) bottom = 0.0f;
        if (roundedRight != rightValue && !(rightValue < 0.0f))
            rightInteger = (s32)((u32)rightInteger + 1);

        f32 topValue = gabi::fmadds(height, extent->y, doubledMargin);
        conversion->mLow = (u32)rightInteger ^ 0x80000000u;
        conversion->mHigh = 0x43300000u;
        s32 topTruncated = gabi::ftoi(topValue);
        f64 encodedRight = readEncoded();
        conversion->mHigh = (u32)topTruncated;
        s32 topInteger = (s32)(u32)conversion->mHigh;
        conversion->mHigh = 0x43300000u;
        conversion->mLow = (u32)topInteger ^ 0x80000000u;
        f64 encodedTop = readEncoded();
        f32 right = (f32)(encodedRight - signedBias);
        f32 roundedTop = (f32)(encodedTop - signedBias);
        if (gabi::fsubs_ppc(right, width) >= 0.0f) right = width;
        if (roundedTop != topValue && !(topValue < 0.0f))
            topInteger = (s32)((u32)topInteger + 1);
        conversion->mHigh = 0x43300000u;
        conversion->mLow = (u32)topInteger ^ 0x80000000u;
        f32 top = (f32)(readEncoded() - signedBias);
        if (gabi::fsubs_ppc(top, height) >= 0.0f) top = height;
        gabi::FrameLocal<sead::Viewport> scissor(0x38);
        sead::Viewport::createRect(scissor.get(), left, bottom, right, top);
        scissor->applyScissor(&surface);
    }
    std::atomic_thread_fence(std::memory_order_acquire);
}
static sead::SafeTextVtable sShadowViewportTextMethods;
// The resource record's empty attach hook, shared by identical-code folding (merged as TextureReference::notify).
void attachResourceRecord(ResourceRecord* record, u32 kind, const sead::SafeText* name, void* context);
/* 027966F0 */
void agl::ShadowMap::configure(u32 unused, void* context) {
    gabi::AutoFrame frame{0x027966F0};
    (void)unused;
    // Depth sampler: filter modes 6, border colour = the clear colour (word copies), depth compare enabled
    // with compare function 3.
    TextureSamplerSettings& settings = mDepthSampler.settings;
    be<u32>* border = reinterpret_cast<be<u32>*>(settings.scalarParameters);
    u32 dirty = (u32)mDepthSampler.dirty | 2;
    settings.modes[5] = 6;
    settings.modes[3] = 6;
    mLayerCount = 0;
    mDepthSampler.dirty = dirty;
    settings.modes[4] = 6;
    border[0] = sShadowClearColor.r.bits;
    border[1] = sShadowClearColor.g.bits;
    border[2] = sShadowClearColor.b.bits;
    u32 alpha = sShadowClearColor.a.bits;
    mDepthSampler.dirty = dirty | 0x30;
    settings.enabled = 1;
    border[3] = alpha;
    settings.tail = 3;
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mText.v = gabi::guest_str("Shadow Map");
    mCurrentTexture = &mDepthSampler;
    name->mVtable = &sShadowViewportTextMethods;
    attachResourceRecord(&mResource, 1, name.get(), context);
}
/* WWHD glue
data sShadowClearColor 104A01EC
data sShadowViewportTextMethods 10166A2C
str 10166D00
ext 027A063C void attachResourceRecord(agl::ResourceRecord*,u32,const sead::SafeText*,void*)
ext 027B997C void clearRenderTarget(agl::RenderBuffer*,u32,u32,const void*,u32,f32)
*/
