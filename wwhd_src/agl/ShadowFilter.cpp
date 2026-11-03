/* agl shadow map: the separable filter of the depth layers (copy to a filter texture, horizontal and vertical
 * blur per layer). */
// GX2 surface values: wut facts.
#include "agl/ShadowMap.h"
#include "agl/EffectShaderViews.h"
#include "sead/RenderState.h"
#include "sead/DisplayTargets.h"
#include "sead/GraphicsRuntime.h"
#include <atomic>
using namespace agl;
// Library singletons read here (pointer words owned by their merged sources): the dynamic texture cache, the
// shader holder, the primitive shapes and their vertex inputs.
static TextureCacheWord sTextureCache;
static ShaderHolderWord sFilterShaderHolder;
static PrimitiveShapesWord sFilterShapes;
static PrimitiveShapeInputsWord sFilterShapeInputs;
// Names of the filter textures and this source's SafeString vtable.
static TextByte sShadowFilterName;
static TextByte sShadowTemporaryName;
static sead::SafeTextVtable sShadowFilterTextMethods;
struct ShadowFilterUniform { be<f32> mWords[4]; };
// d_snap-TU helpers that attach a cache texture (one slice) to a colour surface.
void attachColorSlice(ColorSurface* color, const TextureData* texture, u32 mip, u32 slice);
void attachColorTexture(ColorSurface* color, const TextureData* texture);
// TextureSamplerLifecycle.cpp's file-local sampler bind.
void bindFilterSampler(TextureSampler* sampler, const void* locations, u32 slot, u32 unused);
/* 02796F74 */
u32 agl::ShadowMap::filterDepthTexture(u32 previousShaderMode) {
    gabi::AutoFrame frame{0x02796F74};
    TextureCacheEntry* depth = mDepthTexture;
    u32 rangePool = (u32)mAllocFromMem1.mValue ^ 1;
    // The depth texture's dimension (2D or 2D array) selects the filter texture's shape and the shader variant.
    u32 dimension = depth->surface.mDimension;
    gabi::FrameLocal<gptr<TextureCacheWord>> cacheWord(0x14);
    *cacheWord = &sTextureCache;
    auto textureCache = [&cacheWord]() -> DynamicTextureCache* { return cacheWord->get()->get(); };
    gabi::FrameLocal<sead::SafeText> baseName(0x18);
    if (dimension == GX2SurfaceDim_2D || dimension == GX2SurfaceDim_2DArray) {
        baseName->mVtable = &sShadowFilterTextMethods;
        baseName->mText = &sShadowFilterName;
    } else {
        baseName->mText = &sShadowFilterName;
        baseName->mVtable = &sShadowFilterTextMethods;
    }
    u32 arrayVariant = dimension == GX2SurfaceDim_2D ? 0 : 1;
    TextureCacheEntry* filter;
    if (dimension == GX2SurfaceDim_2D || dimension == GX2SurfaceDim_2DArray) {
        u32 shared = mAllocWithoutContext.mValue;
        u32 slot = dimension == GX2SurfaceDim_2DArray ? (shared != 0 ? 0x74 : 0x7C) : (shared != 0 ? 0x84 : 0x8C);
        gabi::FrameLocal<sead::SafeText> name(slot);
        if (dimension == GX2SurfaceDim_2DArray) {
            name->mVtable = baseName->mVtable;
            name->mText = baseName->mText;
            u32 height = mHeight.mValue;
            u32 layers = mLayerCount;
            DynamicTextureCache* cache = textureCache();
            u32 width = mWidth.mValue;
            if (shared != 0) filter = cache->acquireShared2DArray(name.get(),ShadowFilterFormat,width,height,layers,1,0,rangePool,1,0);
            else filter = cache->acquire2DArray(name.get(),ShadowFilterFormat,width,height,layers,1,0,rangePool,1,0);
        } else {
            name->mText = baseName->mText;
            name->mVtable = baseName->mVtable;
            u32 height = mHeight.mValue;
            DynamicTextureCache* cache = textureCache();
            u32 width = mWidth.mValue;
            if (shared != 0) filter = cache->acquireShared2D(name.get(),ShadowFilterFormat,width,height,1,0,rangePool,1,0);
            else filter = cache->acquire2D(name.get(),ShadowFilterFormat,width,height,1,0,rangePool,1,0);
        }
        mFilterTexture = filter;
    } else filter = mFilterTexture;
    if (!sameTextureLayout(mFilterSampler.texture,*filter)) mFilterSampler.copyTexture(filter);
    else {
        u32 image = filter->surface.mImage.v;
        u32 mipImage = filter->surface.mMipImage.v;
        nativeSurface(&mFilterSampler)->mImage.v = image;
        nativeSurface(&mFilterSampler)->mMipImage.v = mipImage;
        mFilterSampler.texture.surface.mImage.v = image;
        mFilterSampler.texture.surface.mMipImage.v = mipImage;
    }
    u32 shared = mAllocWithoutContext.mValue;
    mCurrentTexture = &mFilterSampler;
    gabi::FrameLocal<sead::SafeText> temporaryName(shared != 0 ? 0x6C : 0x64);
    if (shared != 0) {
        temporaryName->mText = &sShadowTemporaryName;
        temporaryName->mVtable = baseName->mVtable;
    } else {
        temporaryName->mVtable = baseName->mVtable;
        temporaryName->mText = &sShadowTemporaryName;
    }
    u32 height = mHeight.mValue;
    DynamicTextureCache* cache = textureCache();
    u32 width = mWidth.mValue;
    TextureCacheEntry* temporary;
    if (shared != 0) temporary = cache->acquireShared2D(temporaryName.get(),ShadowFilterFormat,width,height,1,0,rangePool,1,0);
    else temporary = cache->acquire2D(temporaryName.get(),ShadowFilterFormat,width,height,1,0,rangePool,1,0);
    mUseSharedCache = mAllocWithoutContext.mValue;
    // Write red/green only, no depth test or write, no blending.
    gabi::FrameLocal<sead::CompactRenderState> state(0x94);
    sead::CompactRenderState::create(state.get());
    state->mWriteBlue = 0;
    state->mDepthTest = 0;
    state->mWriteRed = 1;
    state->mWriteAlpha = 0;
    state->mDepthWrite = 0;
    state->mBlendEnable = 0;
    state->mWriteGreen = 1;
    state->apply();
    // The frame's logical size and area, as sead's frame-buffer view reads them.
    sead::RenderSurface& surface = *reinterpret_cast<sead::RenderSurface*>(&mFrame.mBase);
    gabi::FrameLocal<sead::Viewport> viewport(0x108);
    sead::Viewport::createSurface(viewport.get(),(const sead::RenderVector2*)&surface);
    viewport->apply(&surface);
    PrimitiveShapeInputs* inputs = sFilterShapeInputs;
    inputs->mInputs[DrawFullscreenInput].bind();
    struct IntToFloatSlot { be<u32> mHigh, mLow; };
    gabi::FrameLocal<IntToFloatSlot> conversion(0x20);
    auto readConverted = [&]() -> f32 {
        u64 bits = ((u64)(u32)conversion->mHigh << 32) | (u32)conversion->mLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return (f32)(encoded - 4503601774854144.0);
    };
    conversion->mHigh = 0x43300000u;
    conversion->mLow = (u32)(s32)mWidth.mValue ^ 0x80000000u;
    f32 floatWidth = readConverted();
    conversion->mLow = (u32)(s32)mHeight.mValue ^ 0x80000000u;
    f32 floatHeight = readConverted();
    struct Rect { be<f32> mMinimumX,mMinimumY,mMaximumX,mMaximumY; };
    gabi::FrameLocal<Rect> rectangle(0x54);
    rectangle->mMaximumX = 0.0f < floatWidth ? floatWidth : 0.0f;
    rectangle->mMinimumX = 0.0f < floatWidth ? 0.0f : floatWidth;
    if (!(0.0f < floatHeight)) {
        f32 minimumX = rectangle->mMinimumX;
        rectangle->mMaximumY = 0.0f;
        surface.mLeft = minimumX;
        f32 maximumX = rectangle->mMaximumX;
        surface.mBottom = floatHeight;
        surface.mRight = maximumX;
        rectangle->mMinimumY = floatHeight;
        surface.mTop = 0.0f;
    } else {
        surface.mBottom = 0.0f;
        f32 minimumX = rectangle->mMinimumX;
        f32 maximumX = rectangle->mMaximumX;
        surface.mLeft = minimumX;
        surface.mRight = maximumX;
        rectangle->mMaximumY = floatHeight;
        surface.mTop = floatHeight;
        rectangle->mMinimumY = 0.0f;
    }
    surface.mWidth = floatWidth;
    surface.mHeight = floatHeight;
    auto selectShader = [](s32 variant) {
        detail::ShaderHolder* holder = sFilterShaderHolder;
        return effectProgramVariant(holder->program(detail::ShaderHolder::cShader_vsm), variant);
    };
    auto activate = [&previousShaderMode](ShaderProgram* shader) {
        previousShaderMode = activateEffectProgram(shader, previousShaderMode);
    };
    auto draw = [] { drawFullscreenShape(sFilterShapes); };
    // The filter-input sampler takes the pass's source texture; clamp modes (clamp, clamp, 0).
    auto bindInput = [this](const TextureData* input, u32 clamp) {
        TextureSampler* sampler = &mFilterInput;
        if (!sameTextureLayout(sampler->texture,*input)) {
            sampler->copyTexture(input);
            u32 dirty = sampler->dirty;
            sampler->settings.modes[0] = clamp;
            sampler->settings.modes[1] = clamp;
            sampler->settings.modes[2] = 0;
            sampler->dirty = dirty | 4;
        } else {
            u32 mipImage = input->surface.mMipImage.v;
            u32 image = input->surface.mImage.v;
            sampler->settings.modes[0] = clamp;
            nativeSurface(sampler)->mImage.v = image;
            nativeSurface(sampler)->mMipImage.v = mipImage;
            sampler->texture.surface.mImage.v = image;
            sampler->texture.surface.mMipImage.v = mipImage;
            u32 dirty = sampler->dirty;
            sampler->settings.modes[1] = clamp;
            sampler->settings.modes[2] = 0;
            sampler->dirty = dirty | 4;
        }
    };
    if ((s32)mLayerCount > 0) {
        f32 aspect = gabi::fdivs_ppc(floatHeight,floatWidth);
        f32 inverseWidth = gabi::fdivs_ppc(1.0f,floatWidth);
        f32 inverseHeight = gabi::fdivs_ppc(1.0f,floatHeight);
        gabi::FrameLocal<ShadowFilterUniform> horizontal(0x28);
        gabi::FrameLocal<ShadowFilterUniform> vertical(0x38);
        // These vectors intentionally overlap each other and the original rectangle.
        // Only the first word is assigned; the remaining original stack words are forwarded.
        gabi::FrameLocal<ShadowFilterUniform> initialLayer(0x48);
        gabi::FrameLocal<ShadowFilterUniform> horizontalLayer(0x4C);
        gabi::FrameLocal<ShadowFilterUniform> verticalLayer(0x50);
        for (s32 layer = 0; layer < (s32)mLayerCount; ++layer) {
            auto* shader = selectShader(arrayVariant);
            activate(shader);
            conversion->mHigh = 0x43300000u;
            conversion->mLow = (u32)layer ^ 0x80000000u;
            f32 layerValue = readConverted();
            initialLayer->mWords[0] = layerValue;
            setEffectUniform(shader,1,initialLayer.get());
            bindInput(mDepthTexture,0);
            bindFilterSampler(&mFilterInput,shader->samplers.get(),(u32)-1,0);
            attachColorSlice(&mFrameColor,mFilterTexture,0,(u32)layer);
            mFrame.mColors[0] = &mFrameColor;
            mFrame.mDepth = NULL;
            sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&mFrame));
            draw();
            shader = selectShader(arrayVariant != 0 ? 3 : 2);
            activate(shader);
            horizontal->mWords[1] = 0.0f;
            horizontal->mWords[3] = 1.0f;
            horizontal->mWords[2] = aspect;
            horizontal->mWords[0] = inverseWidth;
            setEffectUniform(shader,0,horizontal.get());
            horizontalLayer->mWords[0] = layerValue;
            setEffectUniform(shader,1,horizontalLayer.get());
            bindInput(mFilterTexture,1);
            bindFilterSampler(&mFilterInput,shader->samplers.get(),(u32)-1,0);
            attachColorTexture(&mFrameColor,temporary);
            mFrame.mColors[0] = &mFrameColor;
            mFrame.mDepth = NULL;
            sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&mFrame));
            draw();
            shader = selectShader(arrayVariant != 0 ? 5 : 4);
            activate(shader);
            vertical->mWords[1] = inverseHeight;
            vertical->mWords[3] = 1.0f;
            vertical->mWords[2] = aspect;
            vertical->mWords[0] = 0.0f;
            setEffectUniform(shader,0,vertical.get());
            verticalLayer->mWords[0] = layerValue;
            setEffectUniform(shader,1,verticalLayer.get());
            bindInput(temporary,1);
            bindFilterSampler(&mFilterInput,shader->samplers.get(),(u32)-1,0);
            attachColorSlice(&mFrameColor,mFilterTexture,0,(u32)layer);
            mFrame.mColors[0] = &mFrameColor;
            mFrame.mDepth = NULL;
            sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&mFrame));
            draw();
        }
    }
    mFrameColor.invalidate();
    mDepthTexture->invalidate();
    u32 sharedRelease = mUseSharedCache;
    gabi::FrameLocal<sead::SafeText> releaseName(0x64);
    releaseName->mVtable = baseName->mVtable;
    DynamicTextureCache* releaseCache = textureCache();
    releaseName->mText = baseName->mText;
    if (sharedRelease != 0) releaseCache->releaseShared(temporary);
    else releaseCache->release(temporary);
    releaseDepthTexture();
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousShaderMode;
}
/* WWHD glue
data sTextureCache 101F8DD4
data sFilterShaderHolder 101F8D6C
data sFilterShapes 101F8DE4
data sFilterShapeInputs 101F8DDC
data sShadowFilterName 10166D2C
data sShadowTemporaryName 10166D30
data sShadowFilterTextMethods 10166A2C
ext 027BE53C void bindFilterSampler(agl::TextureSampler*,const void*,u32,u32)
*/
