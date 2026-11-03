/* agl SSAO: the depth preparation before the occlusion pass (reduced depth mip chain, optional filtered level). */
// GX2 surface values: wut facts.
#include "agl/SsaoTargetSet.h"
#include "agl/EffectShaderViews.h"
#include "sead/DisplayTargets.h"
#include "sead/GraphicsRuntime.h"
#include <atomic>
using namespace agl;
// Library singletons read here (pointer words owned by their merged sources).
static TextureCacheWord sTextureCache;
static ShaderHolderWord sPreparationShaderHolder;
static PrimitiveShapesWord sPreparationShapes;
static TextByte sSsaoReducedName;
static TextByte sSsaoFilteredName;
static sead::SafeTextVtable sSsaoPreparationTextMethods;
// d_snap-TU helpers that attach a cache texture (or one mip of it) to a colour surface.
void attachColorTexture(ColorSurface* color, const TextureData* texture);
void attachColorSlice(ColorSurface* color, const TextureData* texture, u32 mip, u32 slice);
// TextureSamplerLifecycle.cpp's file-local sampler bind.
void bindPreparationSampler(TextureSampler* sampler, const void* locations, u32 slot, u32 unused);
/* 02794270 */
u32 renderSsaoPreparation(SsaoEffect* effect,u32 targetIndex,const TextureData* input,u32 previousShaderMode) {
    gabi::AutoFrame frame{0x02794270};
    struct IntToFloatSlot { be<u32> mHigh,mLow; };
    gabi::FrameLocal<IntToFloatSlot> conversion(0x10);
    auto readEncoded = [&]() -> f64 {
        u64 bits = ((u64)(u32)conversion->mHigh << 32) | (u32)conversion->mLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return encoded;
    };
    // The input depth's size: the larger of the surface size and the texture's minimum size.
    u32 minimumWidth = input->minimumWidth;
    s32 width = input->surface.mWidth;
    s32 heightWord = input->surface.mHeight;
    s32 height = (u32)input->minimumHeight;
    if ((s32)minimumWidth > width) width = (s32)minimumWidth;
    u32 count = (u32)(s32)effect->mTargetSetCount;
    if (heightWord > height) height = heightWord;
    auto* sets = effect->mTargetSets.get();
    auto* set = sets;
    u32 filter = effect->mFilterEnabled;
    if (sets != NULL) {
        if (targetIndex < count) set = sets + targetIndex;
    } else {
        u32 setIndex = targetIndex < count ? targetIndex : 0;
        // A NULL target-set array keeps PPC's NULL + index * stride address.
        set = gabi::at<SsaoTargetSet>(setIndex * sizeof(SsaoTargetSet));
    }
    // Mip levels of the reduced depth: the reduction level, or the filter level when filtering is on.
    s32 levelCount = 0;
    s32 reductionLevel = effect->mReductionLevel;
    if (filter != 0) levelCount = effect->mFilterLevel;
    if (reductionLevel > levelCount) levelCount = reductionLevel;
    gabi::FrameLocal<sead::SafeText> reducedName(0x40);
    reducedName->mVtable = &sSsaoPreparationTextMethods;
    reducedName->mText = &sSsaoReducedName;
    TextureCacheEntry* reduced = sTextureCache->acquire2D(reducedName.get(),9,(u32)width,(u32)height,
                                                         (u32)levelCount + 1,0,1,1,0);
    set->mPrimaryPassTexture = reduced;
    detail::ShaderHolder* holder = sPreparationShaderHolder;
    u32 reduceProgram = 0;
    u32 programCount = holder->mPrograms.mCount;
    s32 allocatedLevels = reduced->surface.mMipCount;
    if (programCount > detail::ShaderHolder::cShader_ssao_reduce)
        reduceProgram = holder->programSlots()[detail::ShaderHolder::cShader_ssao_reduce].v;
    ShaderProgram* shader = effectProgramVariant(gabi::at<ShaderProgram>(reduceProgram), 0);
    u32 shaderMode = activateEffectProgram(shader, previousShaderMode);
    // Level 0 of the reduced texture takes the input depth's image.
    reduced = set->mPrimaryPassTexture;
    u32 inputImage = input->surface.mImage.v;
    reduced->surface.mImage.v = inputImage;
    reduced = set->mPrimaryPassTexture;
    TextureSampler* sampler = &set->mSecondarySampler;
    previousShaderMode = shaderMode;
    if (!sameTextureLayout(sampler->texture,*reduced)) {
        sampler->copyTexture(reduced);
        u32 dirty = sampler->dirty;
        sampler->settings.modes[0] = 1;
        sampler->settings.modes[1] = 1;
        sampler->dirty = dirty | 4;
        sampler->settings.modes[2] = 2;
    } else {
        u32 image = reduced->surface.mImage.v;
        u32 dirty = sampler->dirty;
        u32 mipImage = reduced->surface.mMipImage.v;
        sampler->settings.modes[0] = 1;
        sampler->settings.modes[1] = 1;
        sampler->texture.surface.mImage.v = image;
        sampler->settings.modes[2] = 2;
        nativeSurface(sampler)->mImage.v = image;
        nativeSurface(sampler)->mMipImage.v = mipImage;
        sampler->texture.surface.mMipImage.v = mipImage;
        sampler->dirty = dirty | 4;
    }
    for (s32 level = 0; level < allocatedLevels; ++level) {
        if (level != 0) {
            conversion->mHigh = 0x43300000u;
            conversion->mLow = (u32)level ^ 0x80000000u;
            f64 encodedLevel = readEncoded();
            conversion->mLow = (u32)width ^ 0x80000000u;
            f64 encodedWidth = readEncoded();
            conversion->mLow = (u32)height ^ 0x80000000u;
            f64 encodedHeight = readEncoded();
            f32 levelValue = (f32)(encodedLevel - 4503601774854144.0);
            f32 floatWidth = (f32)(encodedWidth - 4503601774854144.0);
            f32 floatHeight = (f32)(encodedHeight - 4503601774854144.0);
            // The sampler's lod range is pinned to this level (min = max = level, bias 0)...
            u32 dirty = sampler->dirty;
            sampler->settings.scalarParameters[4] = levelValue;
            sampler->settings.scalarParameters[5] = levelValue;
            sampler->settings.scalarParameters[6] = 0.0f;
            sampler->dirty = dirty | 8;
            // ...and render into this level of the reduced texture.
            ColorSurface* color = &set->mSecondaryColor;
            if ((u32)color->mBase.mViewMip != (u32)level) {
                color->mBase.mViewMip = (u32)level;
                color->mBase.mDirty = 1;
            }
            attachColorTexture(color,set->mPrimaryPassTexture);
            struct Rect { be<f32> mMinimumX,mMinimumY,mMaximumX,mMaximumY; };
            gabi::FrameLocal<Rect> rectangle(0x18);
            rectangle->mMaximumX = 0.0f < floatWidth ? floatWidth : 0.0f;
            rectangle->mMinimumX = 0.0f < floatWidth ? 0.0f : floatWidth;
            rectangle->mMaximumY = 0.0f < floatHeight ? floatHeight : 0.0f;
            rectangle->mMinimumY = 0.0f < floatHeight ? 0.0f : floatHeight;
            RenderBuffer* target = &set->mSecondaryFrame;
            target->mBase.mArea.mMinX = rectangle->mMinimumX;
            target->mBase.mArea.mMinY = rectangle->mMinimumY;
            target->mBase.mArea.mMaxX = rectangle->mMaximumX;
            target->mBase.mArea.mMaxY = rectangle->mMaximumY;
            target->mBase.mSize.mWidth = floatWidth;
            target->mBase.mSize.mHeight = floatHeight;
            target->mColors[0] = color;
            sead::RenderSurface* frame = reinterpret_cast<sead::RenderSurface*>(&target->mBase);
            gabi::FrameLocal<sead::Viewport> viewport(0x28);
            sead::Viewport::createSurface(viewport.get(),(const sead::RenderVector2*)frame);
            viewport->apply(frame);
            sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(target));
            bindPreparationSampler(sampler,shader->samplers.get(),(u32)-1,0);
            drawFullscreenShape(sPreparationShapes);
        }
        width >>= 1;
        height >>= 1;
    }
    // The reduced depth is then sampled over levels 0..reduction level.
    u32 reductionAfterCalls = effect->mReductionLevel;
    conversion->mHigh = 0x43300000u;
    conversion->mLow = (reductionAfterCalls + 1) ^ 0x80000000u;
    f64 encodedEnd = readEncoded();
    u32 dirty = sampler->dirty;
    sampler->settings.scalarParameters[4] = 0.0f;
    sampler->dirty = dirty | 8;
    sampler->settings.scalarParameters[5] = (f32)(encodedEnd - 4503601774854144.0);
    sampler->settings.scalarParameters[6] = 0.0f;
    if ((u32)effect->mFilterEnabled != 0) {
        // Filter (never enabled in the shipped game, see SsaoEffect::mFilterEnabled): the mask shader into a
        // filtered texture of the last level's size, sampled by the final sampler.
        holder = sPreparationShaderHolder;
        ShaderProgram* filterShader = holder->program(detail::ShaderHolder::cShader_ssao_mask);
        previousShaderMode = activateEffectProgram(filterShader, previousShaderMode);
        gabi::FrameLocal<sead::SafeText> filteredName(0x48);
        filteredName->mVtable = &sSsaoPreparationTextMethods;
        filteredName->mText = &sSsaoFilteredName;
        RenderBuffer* target = &set->mSecondaryFrame;
        f32 maximumX = target->mBase.mArea.mMaxX;
        f32 minimumX = target->mBase.mArea.mMinX;
        f32 maximumY = target->mBase.mArea.mMaxY;
        f32 extentX = gabi::fsubs_ppc(maximumX,minimumX);
        f32 minimumY = target->mBase.mArea.mMinY;
        f32 extentY = gabi::fsubs_ppc(maximumY,minimumY);
        s32 filteredWidth = gabi::ftoi(extentX);
        conversion->mHigh = (u32)filteredWidth;
        s32 filteredHeight = gabi::ftoi(extentY);
        conversion->mHigh = (u32)filteredHeight;
        TextureCacheEntry* filtered = sTextureCache->acquire2D(filteredName.get(),1,(u32)filteredWidth,
                                                              (u32)filteredHeight,1,0,1,1,0);
        set->mSecondaryPassTexture = filtered;
        attachColorSlice(&set->mSecondaryColor,filtered,0,0);
        bindPreparationSampler(sampler,filterShader->samplers.get(),(u32)-1,0);
        sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(target));
        drawFullscreenShape(sPreparationShapes);
        filtered = set->mSecondaryPassTexture;
        TextureSampler* finalSampler = &set->mFinalSampler;
        if (!sameTextureLayout(finalSampler->texture,*filtered)) {
            finalSampler->copyTexture(filtered);
        } else {
            u32 image = filtered->surface.mImage.v;
            u32 mipImage = filtered->surface.mMipImage.v;
            nativeSurface(finalSampler)->mImage.v = image;
            nativeSurface(finalSampler)->mMipImage.v = mipImage;
            finalSampler->texture.surface.mImage.v = image;
            finalSampler->texture.surface.mMipImage.v = mipImage;
        }
    }
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousShaderMode;
}
/* WWHD glue
data sTextureCache 101F8DD4
data sPreparationShaderHolder 101F8D6C
data sPreparationShapes 101F8DE4
data sSsaoReducedName 1016695C
data sSsaoFilteredName 10166950
data sSsaoPreparationTextMethods 1016650C
ext 027BE53C void bindPreparationSampler(agl::TextureSampler*,const void*,u32,u32)
ext C00060E0 void GX2CallDisplayList(const void*,u32)
ext C0006178 void GX2DrawIndexedEx(u32,u32,u32,const void*,u32,u32)
*/
