// Shadow pre-pass rendering and cached sampler updates.
#include "agl/ShadowPrePass.h"
#include "agl/ShadowShaderSetup.h"
#include "sead/RenderState.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
using agl::shadow_shader::ShaderSetup;
static sead::RenderVector2 sShadowRenderOffset;
u32 drawShadowBlurredSampler(agl::TextureSampler*, const sead::Viewport*, u32, u32, u32, const sead::RenderVector2*, u32);
static bool equalShadowSurface(const sead::GraphicsSurface*, const sead::GraphicsSurface*, u32 cachedDimension);
void updateCachedShadowSampler(agl::TextureSampler*, const agl::TextureData*);
void refreshCachedShadowSampler(agl::TextureSampler*, const agl::TextureData*, u32);
struct PrimaryShaderArguments { be<u32> mFirstSample; be<s32> mWidth, mHeight; };
WWHD_SIZE(PrimaryShaderArguments,12);

/* 02791230 */
u32 ShadowPrePass::render(u32 frameIndex, void* unused, const agl::TextureData* outputDimensions, void* unusedOther,
                            const agl::TextureData* depth, const agl::TextureData* normal,
                            const agl::TextureData* decal, u32 sampleCount, u32 previousMode) {
    gabi::AutoFrame frame{0x02791230u};
    if (mParameters.mEnabled.mValue == 0) return previousMode;
    FrameResources* resources = mFrames;
    if (frameIndex < mFrameCount)
        resources = guestElement(resources, frameIndex);
    u32 width = outputDimensions->surface.mWidth;
    u32 minimumWidth = outputDimensions->minimumWidth;
    if (static_cast<s32>(minimumWidth) > static_cast<s32>(width)) width = minimumWidth;
    u32 height = outputDimensions->surface.mHeight;
    u32 minimumHeight = outputDimensions->minimumHeight;
    if (static_cast<s32>(height) > static_cast<s32>(minimumHeight)) minimumHeight = height;
    ensureOutput(frameIndex, width, minimumHeight);
    bindOutputTarget(frameIndex, 0);
    if (depth != nullptr) updateCachedShadowSampler(&resources->mInitialSamplers[0], depth);
    u32 firstSample = 0;
    if (normal != nullptr) {
        if (normal->surface.mDimension == 1) updateCachedShadowSampler(&resources->mInitialSamplers[1], normal);
        else {
            updateCachedShadowSampler(&resources->mInitialSamplers[2], normal);
            firstSample = 1;
        }
    }
    if (decal != nullptr && mParameters.mStaticDepth.mValue != 0)
        updateCachedShadowSampler(&resources->mInitialSamplers[3], decal);
    gabi::FrameLocal<PrimaryShaderArguments> shaderArgs(8);
    shaderArgs->mFirstSample = firstSample;
    width = depth->minimumWidth;
    u32 actualWidth = depth->surface.mWidth;
    if (static_cast<s32>(actualWidth) > static_cast<s32>(width)) width = actualWidth;
    shaderArgs->mWidth = static_cast<s32>(width);
    height = depth->minimumHeight;
    u32 actualHeight = depth->surface.mHeight;
    if (static_cast<s32>(height) <= static_cast<s32>(actualHeight)) height = actualHeight;
    shaderArgs->mHeight = static_cast<s32>(height);
    previousMode = ShaderSetup::primary(this, resources, previousMode,
        &resources->mInitialSamplers[0], &resources->mInitialSamplers[1], &resources->mInitialSamplers[2],
        sampleCount, &resources->mInitialSamplers[3], static_cast<u8>(firstSample), static_cast<s32>(width), static_cast<s32>(height));
    if (mParameters.mBlurType.mValue != 0) {
        bindOutputTarget(frameIndex, 1);
        u32 blurWidth = mParameters.mBlurWidth.mValue;
        if (blurWidth >= 6) {
            u32 wide = static_cast<s32>(blurWidth) >= 7 ? 1u : 0u;
            agl::TextureSampler* sampler = outputSampler(frameIndex, 0);
            previousMode = ShaderSetup::paired(this, resources, 0, wide, &resources->mInitialSamplers[0], sampler, previousMode);
            bindOutputTarget(frameIndex, 2);
            sampler = outputSampler(frameIndex, 1);
            previousMode = ShaderSetup::paired(this, resources, 1, wide, &resources->mInitialSamplers[0], sampler, previousMode);
        } else {
            gabi::FrameLocal<sead::CompactRenderState> state(0x44);
            sead::CompactRenderState::create(state.get());
            u8 generate = mParameters.mGenerateNormals.mValue;
            u8 combine = mParameters.mCombineSsao.mValue;
            u8 green = 1;
            state->mDepthWrite = 0;
            state->mDepthTest = 0;
            state->mBlendEnable = 0;
            if (generate != 0 && mParameters.mDepthToNormalBlur.mValue == 0) green = 0;
            state->mWriteRed = combine != 0 ? 1 : 0;
            state->mWriteAlpha = 0;
            state->mWriteGreen = green;
            state->mWriteBlue = 1;
            state->apply();
            u32 index = 0;
            gabi::FrameLocal<sead::Viewport> horizontal(0x14), vertical(0x2C);
            while (static_cast<s32>(index) < static_cast<s32>(mParameters.mBlurRepeat.mValue)) {
                ColorTarget* target = bindOutputTarget(frameIndex, 1);
                sead::Viewport::createSurface(horizontal.get(), frameSize(&target->mFrame));
                horizontal->apply(frameSurface(&target->mFrame));
                u32 pass = index == 0 ? 0 : 2;
                agl::TextureSampler* sampler = outputSampler(frameIndex, pass);
                previousMode = drawShadowBlurredSampler(sampler, horizontal.get(), blurWidth, 1, 1, &sShadowRenderOffset, previousMode);
                target = bindOutputTarget(frameIndex, 2);
                sead::Viewport::createSurface(vertical.get(), frameSize(&target->mFrame));
                vertical->apply(frameSurface(&target->mFrame));
                sampler = outputSampler(frameIndex, 1);
                previousMode = drawShadowBlurredSampler(sampler, vertical.get(), blurWidth, 0, 1, &sShadowRenderOffset, previousMode);
                ++index;
            }
        }
    }
    u32 selection = passSelection();
    if (selection == 1 || selection == 3) {
        u32 pass = selection == 3 ? 2u : 0u;
        bindOutputTarget(frameIndex, pass + 1u);
        u8 combine = 0;
        if ((mRegistryQuery.mValue & 1u) != 0 && mParameters.mCombineSsao.mValue == 0) combine = 1;
        u8 edge = mParameters.mEdgeCutoff.mValue;
        agl::TextureSampler* sampler = outputSampler(frameIndex, pass);
        agl::TextureSampler* other = resources->mCombinedSampler.get();
        previousMode = ShaderSetup::combined(this, resources, previousMode, sampler, other, combine, edge);
    }
    resources->mPrimaryStorage->invalidate();
    resources->mPrimaryTarget.mSurface.invalidate();
    if (mParameters.mMipBlur.mValue != 0) {
        previousMode = applyMipBlur(frameIndex, resources, previousMode);
        resources->mPrimaryStorage->invalidate();
        resources->mPrimaryTarget.mSurface.invalidate();
    }
    return previousMode;
}
// The description words a cached sampler texture must share with the incoming one (the image addresses may differ).
static bool equalShadowSurface(const sead::GraphicsSurface* cached, const sead::GraphicsSurface* source, u32 cachedDimension) {
    return cachedDimension == source->mDimension && cached->mWidth == source->mWidth &&
        cached->mHeight == source->mHeight && cached->mDepth == source->mDepth &&
        cached->mMipCount == source->mMipCount && cached->mFormat == source->mFormat &&
        cached->mSwizzle == source->mSwizzle && cached->mTileMode == source->mTileMode &&
        cached->mSamples == source->mSamples;
}
// cachedDimension: the sampler's own surface dimension, read by the caller (prepareFrame reads it before it stores the
// new texture in its slot, as GHS scheduled the inlined refresh there).
void refreshCachedShadowSampler(agl::TextureSampler* sampler, const agl::TextureData* source, u32 cachedDimension) {
    if (!equalShadowSurface(&sampler->texture.surface, &source->surface, cachedDimension)) sampler->copyTexture(source);
    else {
        // TextureSampler::nativeTexture starts with the sampler's own GX2 surface.
        auto* native = reinterpret_cast<sead::GraphicsSurface*>(sampler->nativeTexture);
        void* image = source->surface.mImage;
        void* mipImage = source->surface.mMipImage;
        sampler->texture.surface.mImage = image;
        native->mMipImage = mipImage;
        native->mImage = image;
        sampler->texture.surface.mMipImage = mipImage;
    }
}
void updateCachedShadowSampler(agl::TextureSampler* sampler, const agl::TextureData* source) {
    refreshCachedShadowSampler(sampler, source, sampler->texture.surface.mDimension);
}
/* WWHD glue
data sShadowRenderOffset 104A0908
*/
