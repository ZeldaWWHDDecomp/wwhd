// The view's layer passes (RenderQueueManager::drawView 027C1AD8): the target is prepared, every pending layer runs its
// pass and the pass listener is told. A multisampled view then resolves: the colour surface is expanded (when it has
// an auxiliary buffer) and drawn through a stack texture sampler into the resolve surface with depth and blending off,
// and the render target gets its own colour and depth surfaces back.
#include "agl/DisplayFrameState.h"
#include "agl/ShaderContext.h"
#include "agl/TextureSampler.h"
#include "sead/RenderState.h"
using namespace agl::render;
static agl::ShaderContext* sShaderContext;
static sead::RenderVector2 sUnitScale;
static sead::RenderVector2 sZeroOffset;
void expandColorBuffer(agl::PlatformColorBuffer_l*);
// agl::TextureSampler::create/destroy (027BDF7C/027BE2B0), bound here under own names (the light collections bind
// the method names).
agl::TextureSampler* constructResolveSampler(agl::TextureSampler*);
void destroyResolveSampler(agl::TextureSampler*,u32);
u32 drawSamplerQuad(agl::TextureSampler*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32);
// The sampler's surface describes the colour surface already when every GX2 surface field the original compares agrees.
static bool samplerMatches(const sead::GraphicsSurface* sampler,const agl::PlatformSurface_l* color) {
    return (u32)sampler->mDimension == (u32)color->mDimension &&
           (u32)sampler->mWidth == (u32)color->mWidth &&
           (u32)sampler->mHeight == (u32)color->mHeight &&
           (u32)sampler->mDepth == (u32)color->mDepth &&
           (u32)sampler->mMipCount == (u32)color->mMipLevels &&
           (u32)sampler->mFormat == (u32)color->mFormat &&
           (u32)sampler->mSwizzle == (u32)color->mSwizzle &&
           (u32)sampler->mTileMode == (u32)color->mTileMode &&
           (u32)sampler->mSamples == (u32)color->mAntiAlias;
}
/* 027D6C3C */
void agl::render::DisplayFrameState::renderPendingPasses() {
    gabi::AutoFrame frame{gabi::current_function()};
    prepareAttachedSurface();
    GuestRange<gptr<agl::RenderLayerPrefix>> pending = pointerRange<agl::RenderLayerPrefix>(mPending);
    gptr<agl::RenderLayerPrefix>* slot = pending.first;
    gptr<agl::RenderLayerPrefix>* end = pending.end;
    for (; slot != end; ++slot) prepareRenderPass(*slot,0);
    invokePassHook();
    if (mExtraAttachment && mRenderTarget) {
        agl::ColorSurface* color = &mSurfaceA;
        if (gabi::ea(color->mAuxBuffer)) {
            if ((u8)color->mBase.mDirty) {
                color->initializeRegisters();
                color->mBase.mDirty = 0;
            }
            expandColorBuffer(&color->mColor);
            sShaderContext->applyContext();
        }
        gabi::FrameLocal<sead::CompactRenderState> state(8);
        sead::CompactRenderState::create(state.get());
        state->mDepthWrite = 0;
        state->mBlendEnable = 0;
        state->mDepthTest = 0;
        state->apply();
        agl::RenderBuffer* target = mRenderTarget;
        agl::ColorSurface* savedColor = target->mColors[0];
        agl::DepthSurface* savedDepth = target->mDepth;
        target->mColors[0] = &mSurfaceC;
        target = mRenderTarget;
        target->mDepth = NULL;
        bindRenderBuffer(mRenderTarget);
        mScaledViewport.apply(targetSurface());
        gabi::FrameLocal<agl::TextureSampler> sampler(0x7C);
        constructResolveSampler(sampler.get());
        if (!samplerMatches(&sampler->texture.surface,&color->mBase.mSurface)) {
            sampler->copyTexture(reinterpret_cast<agl::TextureData*>(color));
        } else {
            void* image = color->mBase.mSurface.mImage;
            void* mipImage = color->mBase.mSurface.mMipmaps;
            sampler->texture.surface.mImage = image;
            sampler->texture.surface.mMipImage = mipImage;
            auto* nativeSurface = reinterpret_cast<sead::GraphicsSurface*>(sampler->nativeTexture);
            nativeSurface->mImage = image;
            nativeSurface->mMipImage = mipImage;
        }
        drawSamplerQuad(sampler.get(),&mScaledViewport,&sUnitScale,&sZeroOffset,3);
        target = mRenderTarget;
        target->mColors[0] = savedColor;
        target = mRenderTarget;
        target->mDepth = savedDepth;
        destroyResolveSampler(sampler.get(),2);
    }
}
/* WWHD glue
data sShaderContext 101F8BD8
data sUnitScale 104A0920
data sZeroOffset 104A0908
ext 027B6DDC void agl::ColorSurface::initializeRegisters()
ext C0006198 void expandColorBuffer(agl::PlatformColorBuffer_l*)
ext 0276B114 void agl::ShaderContext::applyContext()
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 0274FEB0 void sead::CompactRenderState::apply()
ext 027BDEB4 void agl::TextureSampler::copyTexture(const agl::TextureData*)
ext 027BDF7C agl::TextureSampler* constructResolveSampler(agl::TextureSampler*)
ext 027BE2B0 void destroyResolveSampler(agl::TextureSampler*,u32)
ext 027A9304 u32 drawSamplerQuad(agl::TextureSampler*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32)
*/
