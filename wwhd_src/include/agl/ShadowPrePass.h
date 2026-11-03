// Shadow pre-pass receiver and its per-frame resources, on the merged agl resource types.
#pragma once
#include "agl/ShadowRegistryQuery.h"
#include "agl/EffectMatrixSupport.h"
#include "sead/Viewport.h"
#include "sead/GraphicsRuntime.h"
#include "agl/ShadowParameterInstances.h"
#include "agl/ShadowView.h"
#include "agl/TextureSampler.h"
#include "agl/RenderBuffer.h"
#include "agl/RenderSurface.h"
#include "agl/UniformBuffer.h"
#include "agl/TextureResource.h"
#include "agl/ParameterObject.h"
#include "agl/ParameterRoot.h"
namespace agl::shadow_prepass {
// Element `index` of a guest array, addressed in guest (32-bit, wrapping) arithmetic like the console's base + index * size:
// a caller-supplied index (frame index, count, variant selector) then never forms a host address outside the guest window.
template <class T> inline T* guestElement(T* base, u32 index) {
    return gabi::at<T>(gabi::ea(base) + index * static_cast<u32>(sizeof(T)));
}
// A render buffer together with the colour surface it draws into (the pre-pass keeps its output targets so).
struct ColorTarget {
    RenderBuffer mFrame;
    ColorSurface mSurface;
};
struct FrameResources {
    UniformBuffer mBuffers[3];
    TextureSampler mInitialSamplers[4];
    TextureSampler mPrimarySampler;
    gptr<TextureCacheEntry> mPrimaryStorage;
    ColorTarget mPrimaryTarget;
    TextureSampler mAlternateSampler;
    gptr<TextureCacheEntry> mAlternateStorage;
    ColorTarget mAlternateTarget;
    TextureSampler mMipSamplers[2];
    gptr<TextureCacheEntry> mMipStorage[2];
    RenderBuffer mPassTargets[2];
    ColorSurface mMipSurfaces[2];
    ColorTarget mExtraTarget;
    DepthSurface mExtraDepth;
    TextureSampler mAuxiliarySampler;
    TextureSampler mFinalSampler;
    be<u8> mReady;
    be<u8> mStorageChanged;
    u8 mReadyPadding[2];
    be<u32> mConfigurationKey;
    sead::RenderVector2 mConfigurationVector;
    gptr<TextureSampler> mCombinedSampler;
    sead::RenderVector2 mDepthScale;
    be<f32> mResolutionScale;
};
// The pre-pass's parameter block at +0x1D8: the resource record 027A03A0 builds, then the parameter object 027A84B8 builds.
struct ParameterState {
    ResourceRecord mRecord;
    ParameterObject mObject;
};
// TextureSampler::settings slots as the pre-pass sets them (dirty bits: TextureSampler::dirty).
enum : u32 {
    SamplerFilterMode = 0,   // settings.modes[0..2]: filter modes (dirty bit 4)
    SamplerClampMode = 3,    // settings.modes[3..5]: clamp modes (dirty bit 2)
    SamplerBorderColor = 0,  // settings.scalarParameters[0..3]: border colour (dirty bit 0x20)
    SamplerLodBias = 5,      // settings.scalarParameters[5]: level-of-detail bias (dirty bit 8)
};
enum : u8 {
    SamplerDirtyMapping = 1, SamplerDirtyClamp = 2, SamplerDirtyFilter = 4, SamplerDirtyLod = 8,
    SamplerDirtyCompare = 0x10, SamplerDirtyBorder = 0x20,
};
enum : u32 { SamplerClampBorder = 6, SamplerCompareGreaterEqual = 3 };
struct PrePassParameters {
    shadow_parameter::BooleanParameter mEnabled;
    shadow_parameter::BooleanParameter mLinearOutput;
    shadow_parameter::IntegerParameter mResolution;
    shadow_parameter::IntegerParameter mBlurType;
    shadow_parameter::IntegerParameter mBlurWidth;
    shadow_parameter::IntegerParameter mBlurRepeat;
    shadow_parameter::FloatParameter mNoiseWidth;
    shadow_parameter::BooleanParameter mEdgeCutoff;
    shadow_parameter::FloatParameter mEdgeThreshold;
    shadow_parameter::FloatParameter mEdgeRange;
    shadow_parameter::BooleanParameter mStaticDepth;
    shadow_parameter::BooleanParameter mDecalAo;
    shadow_parameter::BooleanParameter mFarFade;
    shadow_parameter::FloatParameter mFarFadeStart;
    shadow_parameter::FloatParameter mFarFadeEnd;
    shadow_parameter::BooleanParameter mGenerateNormals;
    shadow_parameter::BooleanParameter mDepthToNormalBlur;
    shadow_parameter::FloatParameter mNormalShadowRatio;
    shadow_parameter::FloatParameter mNormalShadowMultiplier;
    shadow_parameter::BooleanParameter mMipBlur;
    shadow_parameter::BooleanParameter mMipBlurReduce;
    shadow_parameter::IntegerParameter mMipBlurWidth;
    shadow_parameter::IntegerParameter mMipBlurRepeat;
    shadow_parameter::BooleanParameter mCombineSsao;
    shadow_parameter::BooleanParameter mSsaoDirectDraw;
};
struct ShadowPrePass {
    gptr<void> mObjectTable;
    ParameterRoot mRoot;
    be<u32> mFrameCount;
    gptr<FrameResources> mFrames;
    shadow_registry::QuerySlot mRegistryQuery;
    ParameterState mParameterState;
    PrePassParameters mParameters;
    be<u32> mFrameSequence, mFrameToken;
    sead::RenderQuaternion mColor;
    be<u8> mSwapIndex;
    u8 mPadding[3];
    u32 passSelection() const;
    void swapBuffers();
    u32 configurationKey(u32 frameIndex) const;
    void releaseTransientStorage(u32 frameIndex);
    ColorTarget* outputTarget(u32 frameIndex, u32 pass);
    TextureSampler* outputSampler(u32 frameIndex, u32 pass);
    void clearPrimaryTarget(u32 frameIndex);
    ColorTarget* bindOutputTarget(u32 frameIndex, u32 pass);
    ColorTarget* ensureOutput(u32 frameIndex, u32 width, u32 height);
    void initializeFrames(s32 count, sead::RegistryHeap* heap);
    TextureData* prepareFrame(u32 frameIndex, u32 width, u32 height, u32 flags);
    u32 render(u32 frameIndex, void* unused, const TextureData* outputDimensions, void* unusedOther,
                 const TextureData* depth, const TextureData* normal,
                 const TextureData* decal, u32 sampleCount, u32 previousMode);
    void updateUniforms(u32 frameIndex, const agl::effect_matrix::Matrix34* view,
                        const agl::effect_matrix::Matrix4* projection,
                        const agl::effect_matrix::Matrix4* transforms, const void* vectors,
                        const shadow_view::CameraView* cameraView, f64 nearPlane, f64 farPlane,
                        f64 depthScale, f64 aspectScale);
    u32 applyMipBlur(u32 unusedFrameIndex, FrameResources* resources, u32 previousMode);
    static void destroy(ShadowPrePass* shadow, u32 flags);
    static ShadowPrePass* create(ShadowPrePass* shadow);
};
}
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mBuffers, 0);
WWHD_OFFSET(agl::shadow_prepass::ColorTarget, mFrame, 0);
WWHD_OFFSET(agl::shadow_prepass::ColorTarget, mSurface, 0x40);
WWHD_SIZE(agl::shadow_prepass::ColorTarget, 0x188);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mInitialSamplers, 0x54);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mExtraDepth, 0x14CC);
WWHD_OFFSET(agl::shadow_prepass::ParameterState, mObject, 0x1E4);
WWHD_SIZE(agl::shadow_prepass::ParameterState, 0x240);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mPrimarySampler, 0x6B4);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mPrimaryStorage, 0x84C);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mPrimaryTarget, 0x850);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mAlternateSampler, 0x9D8);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mAlternateStorage, 0xB70);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mAlternateTarget, 0xB74);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mMipSamplers, 0xCFC);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mMipStorage, 0x102C);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mPassTargets, 0x1034);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mExtraTarget, 0x1344);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mAuxiliarySampler, 0x1620);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mFinalSampler, 0x17B8);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mReady, 0x1950);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mStorageChanged, 0x1951);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mConfigurationKey, 0x1954);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mCombinedSampler, 0x1960);
WWHD_OFFSET(agl::shadow_prepass::FrameResources, mResolutionScale, 0x196C);
WWHD_SIZE(agl::shadow_prepass::FrameResources, 0x1970);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mObjectTable, 0);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mRoot, 4);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mFrameCount, 0x1CC);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mFrames, 0x1D0);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mRegistryQuery, 0x1D4);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mParameterState, 0x1D8);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mSwapIndex, 0x5C0);
WWHD_SIZE(agl::shadow_prepass::ShadowPrePass, 0x5C4);
// The sead views of a render buffer's logical frame buffer that sead's frame-buffer and viewport functions take: its size
// words as a RenderVector2, its size and area words as a RenderSurface, and the frame-buffer prefix as a DisplayFrame.
inline const sead::RenderVector2* frameSize(const agl::RenderBuffer* buffer) {
    return reinterpret_cast<const sead::RenderVector2*>(&buffer->mBase.mSize);
}
inline const sead::RenderSurface* frameSurface(const agl::RenderBuffer* buffer) {
    return reinterpret_cast<const sead::RenderSurface*>(&buffer->mBase);
}
inline sead::DisplayFrame* frameDisplay(agl::RenderBuffer* buffer) { return reinterpret_cast<sead::DisplayFrame*>(buffer); }
void clearShadowRenderTarget(agl::RenderBuffer*, u32, u32, const sead::ColorChannels*, u32, f64);
// agl::RenderBuffer::destroy as the plain function __destroy_arr calls back (a member function has no plain address).
void destroyShadowFrameTarget(agl::RenderBuffer*, u32);
void destroyShadowFrameArray(void*, u32, u32, u32, u32, u32);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mParameters, 0x418);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mEnabled, 0x0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mLinearOutput, 0x10);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mResolution, 0x20);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mBlurType, 0x30);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mBlurWidth, 0x40);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mBlurRepeat, 0x50);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mNoiseWidth, 0x60);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mEdgeCutoff, 0x70);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mEdgeThreshold, 0x80);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mEdgeRange, 0x90);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mStaticDepth, 0xA0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mDecalAo, 0xB0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mFarFade, 0xC0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mFarFadeStart, 0xD0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mFarFadeEnd, 0xE0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mGenerateNormals, 0xF0);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mDepthToNormalBlur, 0x100);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mNormalShadowRatio, 0x110);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mNormalShadowMultiplier, 0x120);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mMipBlur, 0x130);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mMipBlurReduce, 0x140);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mMipBlurWidth, 0x150);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mMipBlurRepeat, 0x160);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mCombineSsao, 0x170);
WWHD_OFFSET(agl::shadow_prepass::PrePassParameters, mSsaoDirectDraw, 0x180);
WWHD_SIZE(agl::shadow_prepass::PrePassParameters, 0x190);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mFrameSequence, 0x5A8);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mFrameToken, 0x5AC);
WWHD_OFFSET(agl::shadow_prepass::ShadowPrePass, mColor, 0x5B0);

WWHD_OFFSET(agl::shadow_prepass::FrameResources,mMipSurfaces,0x10B4);



