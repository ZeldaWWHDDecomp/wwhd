// Shadow resource setup; external cache and sampler operations stay explicit.
#include "agl/ShadowPrePass.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
agl::DynamicTextureCache* shadowTextureCache();
static sead::SafeTextVtable sShadowSetupTextTable;
void updateCachedShadowSampler(agl::TextureSampler*,const agl::TextureData*);
void refreshCachedShadowSampler(agl::TextureSampler*,const agl::TextureData*,u32);
// The first refresh after acquiring the primary and the alternate texture: GHS inlined it and loaded the sampler's cached
// dimension before storing the new texture in its slot (0278F2E0 / 0278F2E8, 0278F650 / 0278F658).
static void adoptShadowTexture(gptr<agl::TextureCacheEntry>* slot, agl::TextureSampler* sampler, agl::TextureCacheEntry* texture) {
    u32 cachedDimension = sampler->texture.surface.mDimension;
    *slot = texture;
    refreshCachedShadowSampler(sampler, texture, cachedDimension);
}
void initializeShadowRenderSurface(agl::ColorSurface*,const agl::TextureData*);
void initializeShadowMipSurface(agl::ColorSurface*,const agl::TextureData*,u32,u32);
struct SetupDimensions { be<u32> mHigh, mLow; };
struct SetupRectangle { be<f32> mLeft,mBottom,mRight,mTop; };
struct TextureLookupArguments { be<u32> mSingle,mFlags; };
WWHD_SIZE(SetupDimensions,8);
WWHD_SIZE(SetupRectangle,16);
static f32 unsignedShadowDimension(SetupDimensions*,u32);
static void setShadowSurfaceRectangle(sead::VirtualFrameBuffer*,SetupRectangle*,f32,f32);
static void setShadowLookupText(sead::SafeText*,const char*);

/* 0278F04C */
agl::TextureData* ShadowPrePass::prepareFrame(u32 frameIndex,u32 width,u32 height,u32 flags) {
    gabi::AutoFrame frameContext{0x0278F04Cu};
    gabi::FrameLocal<TextureLookupArguments> lookupArgs(8);
    gabi::FrameLocal<be<u8>> bindRequested(0x10);
    gabi::FrameLocal<SetupRectangle> mipRectangle(0x14), primaryRectangle(0x38), alternateRectangle(0x48), extraRectangle(0x58);
    gabi::FrameLocal<sead::SafeText> mipName(0x24), primaryName(0x68), alternateName(0x70);
    gabi::FrameLocal<SetupDimensions> conversion(0x30);
    *bindRequested.get() = static_cast<u8>(flags);
    FrameResources* resources = mFrames;
    if (frameIndex < mFrameCount)
        resources = guestElement(resources, frameIndex);
    f32 widthFloat = unsignedShadowDimension(conversion.get(),width);
    f32 heightFloat = unsignedShadowDimension(conversion.get(),height);
    resources->mConfigurationVector.x = gabi::fdivs_ppc(1.0f,widthFloat);
    resources->mConfigurationVector.y = gabi::fdivs_ppc(1.0f,heightFloat);
    releaseTransientStorage(frameIndex);
    resources->mReady = 1;
    u8 auxiliaryFlags = resources->mAuxiliarySampler.dirty;
    u8 finalFlags = resources->mFinalSampler.dirty;
    resources->mFinalSampler.componentMapping[0] = 1;
    resources->mFinalSampler.customMapping = 1;
    resources->mFinalSampler.componentMapping[1] = 2;
    resources->mFinalSampler.componentMapping[2] = 3;
    resources->mFinalSampler.componentMapping[3] = 0;
    resources->mAuxiliarySampler.componentMapping[0] = 1;
    resources->mAuxiliarySampler.componentMapping[1] = 2;
    resources->mAuxiliarySampler.componentMapping[2] = 3;
    resources->mAuxiliarySampler.customMapping = 1;
    resources->mAuxiliarySampler.componentMapping[3] = 0;
    resources->mAuxiliarySampler.dirty = static_cast<u8>(auxiliaryFlags | SamplerDirtyMapping);
    resources->mFinalSampler.dirty = static_cast<u8>(finalFlags | SamplerDirtyMapping);
    u32 format = 10;
    if (mParameters.mStaticDepth.mValue != 0 || mParameters.mDecalAo.mValue != 0 ||
        mParameters.mGenerateNormals.mValue != 0 || mParameters.mCombineSsao.mValue != 0) format = 29;
    u32 mipLevels = mParameters.mMipBlur.mValue != 0 ? 2u : 1u;
    setShadowSurfaceRectangle(&resources->mPrimaryTarget.mFrame.mBase, primaryRectangle.get(), widthFloat, heightFloat);
    setShadowLookupText(primaryName.get(), "shadow_prepass_light_buffer");
    lookupArgs->mSingle = 1;
    lookupArgs->mFlags = 0;
    agl::TextureCacheEntry* texture = shadowTextureCache()->acquire2D(primaryName.get(), format, width, height, mipLevels, 0, 0, 1, 0);
    adoptShadowTexture(&resources->mPrimaryStorage, &resources->mPrimarySampler, texture);
    texture = resources->mPrimaryStorage.get();
    updateCachedShadowSampler(&resources->mAuxiliarySampler, texture);
    texture = resources->mPrimaryStorage.get();
    updateCachedShadowSampler(&resources->mFinalSampler, texture);
    initializeShadowRenderSurface(&resources->mPrimaryTarget.mSurface, resources->mPrimaryStorage.get());
    resources->mPrimaryTarget.mFrame.mColors[0] = &resources->mPrimaryTarget.mSurface;
    widthFloat = unsignedShadowDimension(conversion.get(), width);
    heightFloat = unsignedShadowDimension(conversion.get(), height);
    setShadowSurfaceRectangle(&resources->mAlternateTarget.mFrame.mBase, alternateRectangle.get(), widthFloat, heightFloat);
    setShadowLookupText(alternateName.get(), "shadow_prepass_light_buffer_temp");
    lookupArgs->mSingle = 1;
    lookupArgs->mFlags = 0;
    texture = shadowTextureCache()->acquire2D(alternateName.get(), format, width, height, 1, 0, 0, 1, 0);
    adoptShadowTexture(&resources->mAlternateStorage, &resources->mAlternateSampler, texture);
    initializeShadowRenderSurface(&resources->mAlternateTarget.mSurface, resources->mAlternateStorage.get());
    resources->mAlternateTarget.mFrame.mColors[0] = &resources->mAlternateTarget.mSurface;
    if (mParameters.mMipBlur.mValue != 0) {
        u32 divisor = mParameters.mMipBlurReduce.mValue != 0 ? 4u : 2u;
        u32 mipHeight = height / divisor;
        u32 mipWidth = width / divisor;
        f32 mipHeightFloat = unsignedShadowDimension(conversion.get(), mipHeight);
        f32 mipWidthFloat = unsignedShadowDimension(conversion.get(), mipWidth);
        for (u32 index = 0; index != 2; ++index) {
            setShadowSurfaceRectangle(&resources->mPassTargets[index].mBase, mipRectangle.get(), mipWidthFloat, mipHeightFloat);
            setShadowLookupText(mipName.get(), "shadow_prepass_light_buffer_temp_half");
            lookupArgs->mSingle = 1;
            lookupArgs->mFlags = 0;
            texture = shadowTextureCache()->acquire2D(mipName.get(), format, mipWidth, mipHeight, 1, 0, 0, 1, 0);
            resources->mMipStorage[index] = texture;
            updateCachedShadowSampler(&resources->mMipSamplers[index], texture);
            resources->mMipSamplers[index].overrideEnabled = 1;
            initializeShadowRenderSurface(&resources->mMipSurfaces[index], resources->mMipStorage[index].get());
            resources->mPassTargets[index].mColors[0] = &resources->mMipSurfaces[index];
        }
        initializeShadowMipSurface(&resources->mExtraTarget.mSurface, resources->mPrimaryStorage.get(), 1, 0);
        widthFloat = unsignedShadowDimension(conversion.get(), width >> 1);
        heightFloat = unsignedShadowDimension(conversion.get(), height >> 1);
        setShadowSurfaceRectangle(&resources->mExtraTarget.mFrame.mBase, extraRectangle.get(), widthFloat, heightFloat);
        resources->mExtraTarget.mFrame.mColors[0] = &resources->mExtraTarget.mSurface;
    }
    if (*bindRequested.get() != 0) clearPrimaryTarget(frameIndex);
    return resources->mPrimaryStorage.get();
}
static f32 unsignedShadowDimension(SetupDimensions* scratch, u32 value) {
    scratch->mHigh = 0x43300000u;
    scratch->mLow = value;
    return static_cast<f32>(value);
}
static void setShadowSurfaceRectangle(sead::VirtualFrameBuffer* frame, SetupRectangle* rectangle, f32 width, f32 height) {
    frame->mSize.mWidth = width;
    frame->mSize.mHeight = height;
    if (0.0f >= width) {
        rectangle->mRight = 0.0f;
        rectangle->mLeft = width;
    } else {
        rectangle->mRight = width;
        rectangle->mLeft = 0.0f;
    }
    if (0.0f < height) {
        rectangle->mTop = height;
        rectangle->mBottom = 0.0f;
    } else {
        rectangle->mTop = 0.0f;
        rectangle->mBottom = height;
    }
    frame->mArea.mMinX = rectangle->mLeft;
    frame->mArea.mMinY = rectangle->mBottom;
    frame->mArea.mMaxX = rectangle->mRight;
    frame->mArea.mMaxY = rectangle->mTop;
}
static void setShadowLookupText(sead::SafeText* text, const char* name) {
    text->mVtable = &sShadowSetupTextTable;
    text->mText.v = gabi::guest_str(name);
}
/* WWHD glue
data sShadowSetupTextTable 101658B4
ext 025C0928 void initializeShadowRenderSurface(agl::ColorSurface*,const agl::TextureData*)
ext 025C0848 void initializeShadowMipSurface(agl::ColorSurface*,const agl::TextureData*,u32,u32)
str 10165F38 "shadow_prepass_light_buffer"
str 10165F7C "shadow_prepass_light_buffer_temp"
str 10165F54 "shadow_prepass_light_buffer_temp_half"
*/
