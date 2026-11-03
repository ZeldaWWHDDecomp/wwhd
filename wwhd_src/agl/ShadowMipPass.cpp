// Shadow mip blur pass, reconstructed from this unit's rendering sequence.
#include "agl/ShadowPrePass.h"
#include "sead/RenderState.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
static sead::RenderVector2 sShadowMipOffset;
u32 drawShadowScaledSampler(agl::TextureSampler*, const sead::Viewport*, const sead::RenderVector2*, const sead::RenderVector2*, u32);
u32 drawShadowBlurredSampler(agl::TextureSampler*, const sead::Viewport*, u32, u32, u32, const sead::RenderVector2*, u32);

/* 02790EC4 */
u32 ShadowPrePass::applyMipBlur(u32 unusedFrameIndex, FrameResources* resources, u32 previousMode) {
    gabi::AutoFrame frame{0x02790EC4u};
    gabi::FrameLocal<sead::RenderVector2> initialScale(8), initialOffset(0x10);
    gabi::FrameLocal<sead::Viewport> horizontalView(0x18), verticalView(0x30), outputView(0x58);
    gabi::FrameLocal<sead::RenderVector2> finalScale(0x48), finalOffset(0x50);
    gabi::FrameLocal<sead::CompactRenderState> state(0x70);
    resources->mPrimaryStorage->invalidate();
    resources->mMipStorage[0]->invalidate();
    resources->mMipStorage[1]->invalidate();
    resources->mExtraTarget.mSurface.invalidate();
    resources->mMipSurfaces[0].invalidate();
    resources->mMipSurfaces[1].invalidate();
    agl::RenderBuffer* verticalTarget = &resources->mPassTargets[1];
    agl::RenderBuffer* horizontalTarget = &resources->mPassTargets[0];
    sead::activateFrameBuffer(frameDisplay(verticalTarget));
    sead::Viewport::createSurface(outputView.get(), frameSize(verticalTarget));
    outputView->apply(frameSurface(verticalTarget));
    sead::CompactRenderState::create(state.get());
    state->mDepthWrite = 0;
    state->mBlendEnable = 0;
    state->mDepthTest = 0;
    state->apply();
    f32 scale = mParameters.mMipBlurReduce.mValue != 0 ? 0.25f : 0.5f;
    initialScale->y = scale;
    resources->mPrimarySampler.dirty = static_cast<u8>(resources->mPrimarySampler.dirty | SamplerDirtyLod);
    initialOffset->x = 0.0f;
    resources->mPrimarySampler.overrideEnabled = 1;
    initialScale->x = scale;
    initialOffset->y = 0.0f;
    resources->mPrimarySampler.settings.scalarParameters[SamplerLodBias] = 0.0f;
    previousMode = drawShadowScaledSampler(&resources->mPrimarySampler, outputView.get(), initialScale.get(), initialOffset.get(), previousMode);
    resources->mPrimarySampler.dirty = static_cast<u8>(resources->mPrimarySampler.dirty | SamplerDirtyLod);
    resources->mPrimarySampler.settings.scalarParameters[SamplerLodBias] = 14.0f;
    resources->mMipStorage[0]->invalidate();
    resources->mMipStorage[1]->invalidate();
    resources->mMipSurfaces[0].invalidate();
    resources->mMipSurfaces[1].invalidate();
    u32 width = mParameters.mMipBlurWidth.mValue;
    if (width > 5) width = 5;
    u32 index = 0;
    while (static_cast<s32>(index) < static_cast<s32>(mParameters.mMipBlurRepeat.mValue)) {
        sead::activateFrameBuffer(frameDisplay(horizontalTarget));
        sead::Viewport::createSurface(horizontalView.get(), frameSize(horizontalTarget));
        horizontalView->apply(frameSurface(horizontalTarget));
        previousMode = drawShadowBlurredSampler(&resources->mMipSamplers[1], horizontalView.get(), width, 1, 1, &sShadowMipOffset, previousMode);
        resources->mMipStorage[0]->invalidate();
        resources->mMipStorage[1]->invalidate();
        resources->mMipSurfaces[0].invalidate();
        resources->mMipSurfaces[1].invalidate();
        if (index == static_cast<u32>(mParameters.mMipBlurRepeat.mValue) - 1u && mParameters.mMipBlurReduce.mValue == 0) {
            ColorTarget* target = &resources->mExtraTarget;
            sead::activateFrameBuffer(frameDisplay(&target->mFrame));
            sead::Viewport::createSurface(verticalView.get(), frameSize(&target->mFrame));
            verticalView->apply(frameSurface(&target->mFrame));
            previousMode = drawShadowBlurredSampler(&resources->mMipSamplers[0], verticalView.get(), width, 0, 1, &sShadowMipOffset, previousMode);
        } else {
            sead::activateFrameBuffer(frameDisplay(verticalTarget));
            sead::Viewport::createSurface(verticalView.get(), frameSize(verticalTarget));
            verticalView->apply(frameSurface(verticalTarget));
            previousMode = drawShadowBlurredSampler(&resources->mMipSamplers[0], verticalView.get(), width, 0, 1, &sShadowMipOffset, previousMode);
        }
        resources->mMipStorage[0]->invalidate();
        resources->mMipStorage[1]->invalidate();
        resources->mMipSurfaces[0].invalidate();
        resources->mMipSurfaces[1].invalidate();
        ++index;
    }
    if (mParameters.mMipBlurReduce.mValue != 0) {
        resources->mPrimaryStorage->invalidate();
        resources->mExtraTarget.mSurface.invalidate();
        ColorTarget* target = &resources->mExtraTarget;
        sead::activateFrameBuffer(frameDisplay(&target->mFrame));
        sead::Viewport::createSurface(outputView.get(), frameSize(&target->mFrame));
        outputView->apply(frameSurface(&target->mFrame));
        finalOffset->x = 0.0f;
        finalOffset->y = 0.0f;
        finalScale->y = 2.0f;
        finalScale->x = 2.0f;
        previousMode = drawShadowScaledSampler(&resources->mMipSamplers[1], outputView.get(), finalScale.get(), finalOffset.get(), previousMode);
    }
    resources->mPrimaryStorage->invalidate();
    resources->mExtraTarget.mSurface.invalidate();
    return previousMode;
}
/* WWHD glue
data sShadowMipOffset 104A0908
ext 027A90AC u32 drawShadowScaledSampler(agl::TextureSampler*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32)
ext 027A9584 u32 drawShadowBlurredSampler(agl::TextureSampler*,const sead::Viewport*,u32,u32,u32,const sead::RenderVector2*,u32)
*/
