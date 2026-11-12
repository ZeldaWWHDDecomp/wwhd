/* agl SSAO: the render entry (enabled check), the per-view core (preparation, primary occlusion pass, blur and
 * history), and the primary and secondary passes. */
// GX2 surface values: wut facts.
#include "agl/SsaoTargetSet.h"
#include "sead/ElementAt.h"
#include "agl/EffectShaderViews.h"
#include "agl/PrimitiveTexture.h"
#include "sead/RenderState.h"
#include "sead/DisplayTargets.h"
#include "sead/GraphicsRuntime.h"
#include "sead/ProjectionState.h"
#include "port_native_draw.h"
#include <atomic>
using namespace agl;
// The library's default model matrix (borrowed, read only).
static const sead::RegistryMatrix44 sSsaoDefaultModel;
u32 renderSsaoCore(SsaoEffect*,u32,u32,u32,const TextureData*,u32,const sead::RegistryMatrix44*,u32,f64,f64);
/* 02795D7C */
u32 agl::SsaoEffect::renderIfEnabled(u32 targetIndex,u32 width,u32 height,const TextureData* depth,
                                     u32 previousShaderMode,f64 primaryFactor,f64 secondaryFactor) {
    if ((u32)mEnabled.mValue == 0) return previousShaderMode;
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)renderSsaoCore),this,targetIndex,width,height,
                                  depth,(u32)0,&sSsaoDefaultModel,previousShaderMode,primaryFactor,secondaryFactor);
}
// Library singletons read here (pointer words owned by their merged sources).
static PrimitiveShapeInputsWord sSsaoShapeInputs;
static TextureCacheWord sTextureCache;
static TextByte sSsaoHistoryName;
static sead::SafeTextVtable sSsaoRenderTextMethods;
// d_snap-TU helper that attaches a cache texture to a colour surface.
void attachColorTexture(ColorSurface* color, const TextureData* texture);
u32 renderSsaoPreparation(SsaoEffect*,u32,const TextureData*,u32);
u32 renderSsaoPrimaryPass(SsaoEffect*,u32,u32,u32,u32,const sead::RegistryMatrix44*,u32,f64,f64);
u32 renderSsaoSecondaryPass(SsaoEffect*,u32,u32,u32,u32,u32);
// The target set of a view (the clamped lookup; a NULL array keeps NULL + index * stride).
static SsaoTargetSet* targetSetOf(SsaoEffect* effect, u32 targetIndex) {
    u32 count = (u32)(s32)effect->mTargetSetCount;
    auto* sets = effect->mTargetSets.get();
    /* guest EA arithmetic: sets + index * sizeof wraps at 32 bits as on the
     * console, for a NULL array as before and now also for a real one (a host index reached host addresses past 4 GB) */
    u32 setIndex = targetIndex < count ? targetIndex : 0;
    return &sead::elementAt<SsaoTargetSet>(gabi::ea(sets), setIndex);
}
/* 0279599C */
u32 renderSsaoCore(SsaoEffect* effect,u32 targetIndex,u32 width,u32 height,const TextureData* depth,u32 mode,
                   const sead::RegistryMatrix44* model,u32 previousShaderMode,f64 primaryFactor,f64 secondaryFactor) {
    gabi::AutoFrame frame{0x0279599C};
    gabi::FrameLocal<sead::CompactRenderState> state(0x18);
    // Write red/green only, no depth test or write, no blending.
    sead::CompactRenderState::create(state.get());
    state->mWriteBlue = 0;
    state->mWriteRed = 1;
    state->mWriteGreen = 1;
    state->mDepthTest = 0;
    state->mDepthWrite = 0;
    state->mBlendEnable = 0;
    state->mWriteAlpha = 0;
    state->apply();
    SsaoTargetSet* set = targetSetOf(effect, targetIndex);
    // Mode 0 renders the occlusion into a format-1 texture, the reprojection mode into format 0xA.
    u32 format = mode == 0 ? 1 : 0xA;
    if (set->mExternalFrame.get() == NULL) set->prepareSurfaces(format,(s32)width,(s32)height);
    else mode = 0;
    PrimitiveShapeInputs* inputs = sSsaoShapeInputs;
    inputs->mInputs[DrawFullscreenInput].bind();
    // The depth input: primary sampler 0, clamp modes 0.
    TextureSampler* sampler = &set->mPrimarySamplers[0];
    if (!sameTextureLayout(sampler->texture,*depth)) {
        sampler->copyTexture(depth);
        u32 dirty = sampler->dirty;
        sampler->settings.modes[1] = 0;
        sampler->settings.modes[0] = 0;
        sampler->settings.modes[2] = 0;
        sampler->dirty = dirty | 4;
    } else {
        u32 image = depth->surface.mImage.v;
        u32 mipImage = depth->surface.mMipImage.v;
        sampler->settings.modes[2] = 0;
        u32 dirty = sampler->dirty;
        sampler->settings.modes[1] = 0;
        sampler->texture.surface.mMipImage.v = mipImage;
        sampler->settings.modes[0] = 0;
        nativeSurface(sampler)->mMipImage.v = mipImage;
        sampler->texture.surface.mImage.v = image;
        nativeSurface(sampler)->mImage.v = image;
        sampler->dirty = dirty | 4;
    }
    previousShaderMode = renderSsaoPreparation(effect,targetIndex,depth,previousShaderMode);
    if (set->mExternalFrame.get() != NULL) {
        // The client's frame: its logical size and area, as sead's frame-buffer view reads them.
        auto* target = reinterpret_cast<sead::RenderSurface*>(set->mExternalFrame.get());
        gabi::FrameLocal<sead::Viewport> viewport(0x8C);
        sead::Viewport::createSurface(viewport.get(),(const sead::RenderVector2*)target);
        target = reinterpret_cast<sead::RenderSurface*>(set->mExternalFrame.get());
        viewport->apply(target);
        target = reinterpret_cast<sead::RenderSurface*>(set->mExternalFrame.get());
        sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(target));
    } else {
        TextureCacheEntry* work = set->mWorkTexture;
        attachColorTexture(&set->mPrimaryColor,work);
        sead::RenderSurface* frame = reinterpret_cast<sead::RenderSurface*>(&set->mPrimaryFrame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0xA4);
        sead::Viewport::createSurface(viewport.get(),(const sead::RenderVector2*)frame);
        viewport->apply(frame);
        sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&set->mPrimaryFrame));
    }
    previousShaderMode = renderSsaoPrimaryPass(effect,targetIndex,width,height,mode,model,previousShaderMode,primaryFactor,secondaryFactor);
    DynamicTextureCache* cache = sTextureCache;
    cache->release(set->mPrimaryPassTexture);
    void* externalFrame = set->mExternalFrame.get();
    set->mPrimaryPassTexture = NULL;
    if (externalFrame == 0) {
        // A history texture of another size or format (prepareSurfaces cleared mHistoryValid) is replaced.
        if ((u32)set->mHistoryValid == 0) {
            sTextureCache->releaseShared(set->mHistoryTexture);
            DynamicTextureCache* current = sTextureCache;
            gabi::FrameLocal<sead::SafeText> name(0x10);
            name->mText = &sSsaoHistoryName;
            name->mVtable = &sSsaoRenderTextMethods;
            set->mHistoryTexture = current->acquireShared2D(name.get(),format,width,height,1,0,1,1,0);
            set->mHistoryValid = 1;
        }
        previousShaderMode = renderSsaoSecondaryPass(effect,targetIndex,width,height,mode,previousShaderMode);
        TextureCacheEntry* secondary = set->mSecondaryPassTexture;
        if (secondary != NULL) {
            cache->release(secondary);
            set->mSecondaryPassTexture = NULL;
        }
        TextureCacheEntry* work = set->mWorkTexture;
        if (work != NULL) {
            cache->release(work);
            set->mWorkTexture = NULL;
        }
    }
    std::atomic_thread_fence(std::memory_order_acquire);
#if PORT_NATIVE_DRAW
    wwhd_native_pass_end();  // pass description (port_native_draw.h): the SSAO passes are done
#endif
    return previousShaderMode;
}
// More library singletons: the shader holder, the primitive shapes and the primitive textures.
static ShaderHolderWord sSsaoShaderHolder;
static PrimitiveShapesWord sSsaoShapes;
static PrimitiveTextureWord sSsaoPrimitiveTextures;
// Debug switch: occlude against the primitive depth texture instead of the reduced depth (no writer, always 0).
static u8 sSsaoUsePrimitiveDepth = 0;
// TextureSamplerLifecycle.cpp's file-local sampler bind.
void bindSsaoSampler(TextureSampler* sampler, const void* locations, u32 slot, u32 unused);
struct SsaoPassUniform { be<f32> mWords[4]; };
/* 02794970 */
u32 renderSsaoPrimaryPass(SsaoEffect* effect,u32 targetIndex,u32 width,u32 height,u32 mode,
                          const sead::RegistryMatrix44* model,u32 previousShaderMode,f64 primaryFactor,f64 secondaryFactor) {
    gabi::AutoFrame frame{0x02794970};
    SsaoTargetSet* set = targetSetOf(effect, targetIndex);
    detail::ShaderHolder* holder = sSsaoShaderHolder;
    ShaderProgram* aoBuffer = holder->program(detail::ShaderHolder::cShader_ssao_ao_buffer);
    // Variant: sample pairs, filter, distance attenuation and the reprojection mode.
    u32 pairBits = (u32)(s32)effect->mSamplePairCount.mValue;
    u32 filter = effect->mFilterEnabled;
    u32 attenuation = effect->mDistanceAttenuation.mValue;
    u32 variantIndex = (pairBits << 3) - 8 + (filter << 2) + (attenuation << 1) + mode;
    ShaderProgram* shader = effectProgramVariant(aoBuffer, (s32)variantIndex);
    previousShaderMode = activateEffectProgram(shader, previousShaderMode);
    // A sampler location (the bounds-checked lookup; a NULL table keeps NULL + index * stride).
    auto samplerLocation = [shader](u32 index) -> void* {
        u32 count = shader->samplerCount;
        u32 samplers = shader->samplers.v;
        u32 slot = count > index ? index : 0;
        return gabi::at<ProgramResource16_l>(samplers + slot * u32(sizeof(ProgramResource16_l)));
    };
    if ((u32)sSsaoUsePrimitiveDepth != 0) {
        PrimitiveTexture* primitives = sSsaoPrimitiveTextures;
        bindSsaoSampler(primitives->mSamplers[PrimitiveTexture_Depth1],samplerLocation(3),(u32)-1,0);
    } else bindSsaoSampler(&set->mSecondarySampler,samplerLocation(3),(u32)-1,0);
    bindSsaoSampler(&set->mPrimarySamplers[0],shader->samplers.get(),(u32)-1,0);
    // The rotation-noise texture of this frame (a NULL effect keeps the member's NULL-based address).
    u32 sampleIndex = set->mSampleIndex;
    auto* noiseSamplers = effect != NULL ? &effect->mSampleSamplers[0]
        : gabi::at<TextureSampler>(offsetof(SsaoEffect,mSampleSamplers));
    u32 noiseIndex = sampleIndex < 16 ? sampleIndex : 0;
    auto* noise = noiseSamplers;
    if (noiseIndex != 0) {
        if (noise != NULL) noise += noiseIndex;
        else noise = gabi::at<TextureSampler>(noiseIndex * sizeof(TextureSampler));
    }
    bindSsaoSampler(noise,samplerLocation(1),(u32)-1,0);
    if ((u32)effect->mFilterEnabled != 0)
        bindSsaoSampler(&set->mFinalSampler,samplerLocation(4),(u32)-1,0);
    if (mode != 0) {
        // Reprojection: the previous frame's occlusion (the history texture) on primary sampler 1.
        TextureCacheEntry* history = set->mHistoryTexture;
        TextureSampler* previous = &set->mPrimarySamplers[1];
        if (!sameTextureLayout(previous->texture,*history)) previous->copyTexture(history);
        else {
            u32 image = history->surface.mImage.v;
            u32 mipImage = history->surface.mMipImage.v;
            nativeSurface(previous)->mImage.v = image;
            nativeSurface(previous)->mMipImage.v = mipImage;
            previous->texture.surface.mImage.v = image;
            previous->texture.surface.mMipImage.v = mipImage;
        }
        bindSsaoSampler(previous,samplerLocation(2),(u32)-1,0);
    }
    if ((u32)effect->mRotateSamples.mValue != 0) {
        s32 nextSample = (s32)((u32)set->mSampleIndex + 1);
        set->mSampleIndex = (u32)(nextSample % 16);
    } else set->mSampleIndex = 0;
    f32 depthOffset = effect->mDepthOffset.mValue;
    f32 density = effect->mDensity.mValue;
    f32 remainingDepth = gabi::fsubs_ppc(1.0f,depthOffset);
    f32 doubledDensity = gabi::fadds_ppc(density,density);
    f32 mixRate = effect->mMixRate.mValue;
    gabi::FrameLocal<SsaoPassUniform> material(0x24);
    material->mWords[0] = mixRate;
    material->mWords[2] = remainingDepth;
    material->mWords[1] = doubledDensity;
    s32 pairs = effect->mSamplePairCount.mValue;
    material->mWords[3] = 0.0f;
    f32 factorSpan = gabi::fsubs_fpr(secondaryFactor,primaryFactor);
    if (pairs > 9) pairs = 9;
    if (pairs < 1) pairs = 1;
    // The original signed conversion leaves its two-word scratch slot observable.
    // Both f64 subtraction operands are exact for every signed 32-bit input.
    struct IntToFloatSlot { be<u32> mHigh, mLow; };
    gabi::FrameLocal<IntToFloatSlot> dimensionConversion(0x38);
    dimensionConversion->mLow = width ^ 0x80000000u;
    dimensionConversion->mHigh = 0x43300000u;
    auto readDimension = [&]() -> f64 {
        u64 bits = ((u64)(u32)dimensionConversion->mHigh << 32) | (u32)dimensionConversion->mLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return encoded;
    };
    f64 encodedWidth = readDimension();
    dimensionConversion->mLow = height ^ 0x80000000u;
    f64 encodedHeight = readDimension();
    f32 widthFloat = (f32)(encodedWidth - 4503601774854144.0);
    f32 heightFloat = (f32)(encodedHeight - 4503601774854144.0);
    f32 inverseSpan = gabi::fdivs_ppc(1.0f,factorSpan);
    for (s32 pair = 0; pair < pairs; ++pair) {
        f32 extent = effect->mKernelDepths[pair];
        f32 halfExtent = gabi::fmuls_ppc(extent,0.5f);
        effect->mKernel[pair].mDepthExtent = gabi::fdivs_ppc(1.0f,halfExtent);
    }
    f32 farDistance = effect->mFarDistance.mValue;
    f32 centerWeight = effect->mCenterWeight;
    f32 scaledFarDistance = gabi::fmuls_ppc(farDistance,inverseSpan);
    f32 attenuationDistance = effect->mAttenuationDistance.mValue;
    f32 halfCenterWeight = gabi::fmuls_ppc(centerWeight,0.5f);
    f32 scaledAttenuation = gabi::fmuls_ppc(attenuationDistance,inverseSpan);
    f32 inverseFarDistance = gabi::fdivs_ppc(1.0f,scaledFarDistance);
    f32 inverseAttenuation = gabi::fdivs_ppc(1.0f,scaledAttenuation);
    f32 radius = effect->mRadius.mValue;
    gabi::FrameLocal<SsaoPassUniform> uniform(8);
    uniform->mWords[1] = inverseFarDistance;
    uniform->mWords[3] = halfCenterWeight;
    uniform->mWords[2] = inverseAttenuation;
    uniform->mWords[0] = radius;
    auto upload = [](const UniformLocations_l* slot,u32 words,const void* values) {
        if ((s32)slot->mVertexRegister != -1) GX2SetVertexUniformReg((u32)(s32)slot->mVertexRegister,words,values);
        if ((s32)slot->mPixelRegister != -1) GX2SetPixelUniformReg((u32)(s32)slot->mPixelRegister,words,values);
    };
    auto uniformSlot = [shader](u32 index) {
        u32 count = shader->uniformCount;
        u32 uniforms = shader->uniforms.v;
        u32 slot = count > index ? index : 0;
        return gabi::at<UniformLocations_l>(uniforms + slot * u32(sizeof(UniformLocations_l)));
    };
    upload(programUniforms(shader),4,uniform.get());
    uniform->mWords[1] = gabi::f32_from_fpr(secondaryFactor);
    uniform->mWords[3] = gabi::f32_from_fpr(secondaryFactor);
    uniform->mWords[2] = factorSpan;
    uniform->mWords[0] = gabi::f32_from_fpr(primaryFactor);
    upload(uniformSlot(2),4,uniform.get());
    f32 inverseHeight = gabi::fdivs_ppc(1.0f,heightFloat);
    f32 inverseWidth = gabi::fdivs_ppc(1.0f,widthFloat);
    f32 aspect = gabi::fdivs_ppc(heightFloat,widthFloat);
    uniform->mWords[1] = inverseHeight;
    uniform->mWords[0] = inverseWidth;
    uniform->mWords[3] = 0.25f;
    uniform->mWords[2] = aspect;
    upload(uniformSlot(3),4,uniform.get());
    upload(uniformSlot(4),(u32)pairs * 4,effect->mKernel);
    upload(uniformSlot(6),16,model);
    const auto* modelValues = (const be<f32>*)model;
    f32 firstCoefficient = modelValues[2];
    f32 secondCoefficient = modelValues[6];
    gabi::FrameLocal<SsaoPassUniform> coefficients(0x18);
    coefficients->mWords[0] = firstCoefficient;
    f32 thirdCoefficient = modelValues[14];
    coefficients->mWords[1] = secondCoefficient;
    coefficients->mWords[2] = thirdCoefficient;
    upload(uniformSlot(7),4,coefficients.get());
    upload(uniformSlot(1),4,material.get());
    drawFullscreenShape(sSsaoShapes);
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousShaderMode;
}
static TextByte sSsaoBlurName;
static TextByte sSsaoBlurReleaseName;
// DrawProgram.cpp's default quad scale and position (borrowed, read only).
static const sead::RenderVector2 sSsaoQuadScale;
static const sead::RenderVector2 sSsaoQuadPosition;
// DrawProgram.cpp's file-local textured quad draw.
u32 drawDefaultQuad(TextureSampler* texture, const sead::ViewportBounds* bounds, const sead::RenderVector2* scale,
                    const sead::RenderVector2* position, u32 current);
/* 0279515C */
u32 renderSsaoSecondaryPass(SsaoEffect* effect,u32 targetIndex,u32 width,u32 height,u32 mode,u32 previousShaderMode) {
    gabi::AutoFrame frame{0x0279515C};
    SsaoTargetSet* set = targetSetOf(effect, targetIndex);
    TextureSampler* sampler = &set->mPrimarySamplers[0];
    u32 dirty = sampler->dirty;
    struct IntToFloatSlot { be<u32> mHigh, mLow; };
    gabi::FrameLocal<IntToFloatSlot> dimensionConversion(0x18);
    dimensionConversion->mHigh = 0x43300000u;
    dimensionConversion->mLow = width ^ 0x80000000u;
    auto readDimension = [&]() -> f64 {
        u64 bits = ((u64)(u32)dimensionConversion->mHigh << 32) | (u32)dimensionConversion->mLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return encoded;
    };
    f64 encodedWidth = readDimension();
    dimensionConversion->mLow = height ^ 0x80000000u;
    f64 encodedHeight = readDimension();
    f32 widthFloat = (f32)(encodedWidth - 4503601774854144.0);
    f32 heightFloat = (f32)(encodedHeight - 4503601774854144.0);
    // Primary sampler 0 now reads the occlusion (the work texture): clamp modes 1/1/0.
    sampler->settings.modes[0] = 1;
    sampler->settings.modes[1] = 1;
    sampler->settings.modes[2] = 0;
    sampler->dirty = dirty | 4;
    detail::ShaderHolder* holder = sSsaoShaderHolder;
    u32 programCount = holder->mPrograms.mCount;
    TextureCacheEntry* work = set->mWorkTexture;
    u32 blurProgram = 0;
    if (programCount > detail::ShaderHolder::cShader_ssao_blur)
        blurProgram = holder->programSlots()[detail::ShaderHolder::cShader_ssao_blur].v;
    auto rebind = [sampler](const TextureData* texture) {
        if (!sameTextureLayout(sampler->texture,*texture)) sampler->copyTexture(texture);
        else {
            u32 image = texture->surface.mImage.v;
            u32 mipImage = texture->surface.mMipImage.v;
            nativeSurface(sampler)->mImage.v = image;
            nativeSurface(sampler)->mMipImage.v = mipImage;
            sampler->texture.surface.mImage.v = image;
            sampler->texture.surface.mMipImage.v = mipImage;
        }
    };
    if (!sameTextureLayout(sampler->texture,*work)) sampler->copyTexture(work);
    else {
        u32 image = work->surface.mImage.v;
        u32 mipImage = work->surface.mMipImage.v;
        nativeSurface(sampler)->mImage.v = image;
        sampler->texture.surface.mImage.v = image;
        sampler->texture.surface.mMipImage.v = mipImage;
        nativeSurface(sampler)->mMipImage.v = mipImage;
    }
    // Write red/green only, no depth test or write, no blending.
    gabi::FrameLocal<sead::CompactRenderState> state(0x40);
    sead::CompactRenderState::create(state.get());
    state->mBlendEnable = 0;
    state->mWriteBlue = 0;
    state->mWriteGreen = 1;
    state->mDepthWrite = 0;
    state->mWriteRed = 1;
    state->mWriteAlpha = 0;
    state->mDepthTest = 0;
    state->apply();
    if ((s32)effect->mBlurCount.mValue > 0) {
        // Blur iterations: horizontal into a temporary texture, vertical back into the history texture.
        DynamicTextureCache* cache = sTextureCache;
        gabi::FrameLocal<sead::SafeText> name(0x10);
        name->mVtable = &sSsaoRenderTextMethods;
        name->mText = &sSsaoBlurName;
        TextureCacheEntry* temporary = cache->acquire2D(name.get(),0xA,width,height,1,0,1,1,0);
        f32 aspect = gabi::fdivs_ppc(heightFloat,widthFloat);
        f32 depthOffset = effect->mDepthOffset.mValue;
        f32 density = effect->mDensity.mValue;
        f32 remainingDepth = gabi::fsubs_ppc(1.0f,depthOffset);
        f32 doubledDensity = gabi::fadds_ppc(density,density);
        f32 inverseHeight = gabi::fdivs_ppc(1.0f,heightFloat);
        gabi::FrameLocal<SsaoPassUniform> material(0x20);
        gabi::FrameLocal<SsaoPassUniform> dimensions(0x30);
        material->mWords[3] = 0.0f;
        dimensions->mWords[1] = inverseHeight;
        material->mWords[1] = doubledDensity;
        material->mWords[2] = remainingDepth;
        dimensions->mWords[2] = aspect;
        f32 mixRate = effect->mMixRate.mValue;
        dimensions->mWords[3] = 1.0f;
        material->mWords[0] = mixRate;
        dimensions->mWords[0] = gabi::fdivs_ppc(1.0f,widthFloat);
        auto select = [blurProgram](s32 index) {
            return effectProgramVariant(gabi::at<ShaderProgram>(blurProgram), index);
        };
        auto activate = [&previousShaderMode](ShaderProgram* shader) {
            previousShaderMode = activateEffectProgram(shader, previousShaderMode);
        };
        for (s32 iteration = 0; iteration < (s32)effect->mBlurCount.mValue; ++iteration) {
            auto* shader = select((s32)mode);
            activate(shader);
            setEffectUniform(shader,1,material.get());
            setEffectUniform(shader,3,dimensions.get());
            bindSsaoSampler(sampler,shader->samplers.get(),(u32)-1,0);
            attachColorTexture(&set->mPrimaryColor,temporary);
            sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&set->mPrimaryFrame));
            drawFullscreenShape(sSsaoShapes);
            shader = select((s32)mode + 2);
            activate(shader);
            setEffectUniform(shader,1,material.get());
            setEffectUniform(shader,3,dimensions.get());
            bindSsaoSampler(sampler,shader->samplers.get(),(u32)-1,0);
            attachColorTexture(&set->mPrimaryColor,set->mHistoryTexture);
            sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&set->mPrimaryFrame));
            rebind(temporary);
            bindSsaoSampler(sampler,shader->samplers.get(),(u32)-1,0);
            drawFullscreenShape(sSsaoShapes);
        }
        name->mVtable = &sSsaoRenderTextMethods;
        name->mText = &sSsaoBlurReleaseName;
        cache->release(temporary);
    } else {
        // No blur: copy the occlusion into the history texture with the default textured quad.
        attachColorTexture(&set->mPrimaryColor,set->mHistoryTexture);
        sead::RenderSurface* frame = reinterpret_cast<sead::RenderSurface*>(&set->mPrimaryFrame.mBase);
        gabi::FrameLocal<sead::Viewport> viewport(0xB4);
        sead::Viewport::createSurface(viewport.get(),(const sead::RenderVector2*)frame);
        viewport->apply(frame);
        sead::activateFrameBuffer(reinterpret_cast<sead::DisplayFrame*>(&set->mPrimaryFrame));
        previousShaderMode = drawDefaultQuad(sampler,(const sead::ViewportBounds*)viewport.get(),&sSsaoQuadScale,
                                             &sSsaoQuadPosition,previousShaderMode);
    }
    rebind(set->mHistoryTexture);
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousShaderMode;
}
/* WWHD glue
data sSsaoDefaultModel 104A048C
data sSsaoShapeInputs 101F8DDC
data sTextureCache 101F8DD4
data sSsaoHistoryName 10166990
data sSsaoRenderTextMethods 1016650C
data sSsaoShaderHolder 101F8D6C
data sSsaoShapes 101F8DE4
data sSsaoUsePrimitiveDepth 101F8D7C
data sSsaoPrimitiveTextures 101F8DEC
data sSsaoBlurName 10166980
data sSsaoBlurReleaseName 10166974
data sSsaoQuadScale 104A0920
data sSsaoQuadPosition 104A0908
ext 027BE53C void bindSsaoSampler(agl::TextureSampler*,const void*,u32,u32)
ext 027A90AC u32 drawDefaultQuad(agl::TextureSampler*,const sead::ViewportBounds*,const sead::RenderVector2*,const sead::RenderVector2*,u32)
ext C0006A40 void GX2SetVertexUniformReg(u32,u32,const void*)
ext C0006908 void GX2SetPixelUniformReg(u32,u32,const void*)
*/
