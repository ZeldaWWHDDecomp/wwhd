/* agl SSAO ("aglssao"): the screen-space ambient occlusion effect and its per-view target sets. Layouts from
 * their constructors (SsaoEffect 0x285C bytes, SsaoTargetSet 0x9CC bytes); the members are the merged agl
 * parameter, texture, surface and render-buffer types (the constructor call is noted per member). */
#pragma once
#include "agl/ShadowMap.h"
#include "agl/ParameterRoot.h"
#include "agl/ParameterComparisons.h"
#include "sead/HeapRegistry.h"
namespace agl {
struct SsaoEffect;
// A 4x4 transform, row major.
struct SsaoTransform { be<f32> mRows[4][4]; };
// One sample of the occlusion kernel: offset, depth extent and weight.
struct SsaoKernelVector { be<f32> mX, mY, mDepthExtent, mWeight; };
// The two frame/colour targets a view draws into: the reduced depth (primary) and the occlusion (secondary).
struct SsaoTargetSet {
    /* 0x000 */ gptr<TextureCacheEntry> mHistoryTexture;      // shared pool, kept across frames (reprojection)
    /* 0x004 */ gptr<TextureCacheEntry> mWorkTexture;
    /* 0x008 */ RenderBuffer mPrimaryFrame;                    // 027B95B4
    /* 0x048 */ ColorSurface mPrimaryColor;                    // 027B6CB8
    /* 0x190 */ TextureSampler mPrimarySamplers[2];            // 027BDF7C
    /* 0x4C0 */ gptr<TextureCacheEntry> mPrimaryPassTexture;
    /* 0x4C4 */ RenderBuffer mSecondaryFrame;                  // 027B95B4
    /* 0x504 */ ColorSurface mSecondaryColor;                  // 027B6CB8
    /* 0x64C */ TextureSampler mSecondarySampler;              // 027BDF7C
    /* 0x7E4 */ gptr<TextureCacheEntry> mSecondaryPassTexture;
    /* 0x7E8 */ TextureSampler mFinalSampler;                  // 027BDF7C
    /* 0x980 */ gptr<RenderBuffer> mExternalFrame;             // client-supplied output, NULL = own textures
    /* 0x984 */ be<u32> mSampleIndex;                          // rotates through the 16 sample textures
    /* 0x988 */ SsaoTransform mPreviousViewProjection;
    /* 0x9C8 */ be<u8> mHistoryValid;
    u8 mPadding[3];
    void prepareSurfaces(u32 format, s32 width, s32 height);
    static SsaoTargetSet* construct(SsaoTargetSet* set);
    static void destroy(SsaoTargetSet* set, u32 flags);
};
WWHD_OFFSET(SsaoTargetSet, mWorkTexture, 4);
WWHD_OFFSET(SsaoTargetSet, mPrimaryFrame, 8);
WWHD_OFFSET(SsaoTargetSet, mPrimaryColor, 0x48);
WWHD_OFFSET(SsaoTargetSet, mPrimarySamplers, 0x190);
WWHD_OFFSET(SsaoTargetSet, mPrimaryPassTexture, 0x4C0);
WWHD_OFFSET(SsaoTargetSet, mSecondaryFrame, 0x4C4);
WWHD_OFFSET(SsaoTargetSet, mSecondaryColor, 0x504);
WWHD_OFFSET(SsaoTargetSet, mSecondarySampler, 0x64C);
WWHD_OFFSET(SsaoTargetSet, mSecondaryPassTexture, 0x7E4);
WWHD_OFFSET(SsaoTargetSet, mFinalSampler, 0x7E8);
WWHD_OFFSET(SsaoTargetSet, mExternalFrame, 0x980);
WWHD_OFFSET(SsaoTargetSet, mSampleIndex, 0x984);
WWHD_OFFSET(SsaoTargetSet, mPreviousViewProjection, 0x988);
WWHD_OFFSET(SsaoTargetSet, mHistoryValid, 0x9C8);
WWHD_SIZE(SsaoTargetSet, 0x9CC);
WWHD_SIZE(SsaoTransform, 64);
WWHD_SIZE(SsaoKernelVector, 16);
// The list a NULL-address constructor allocates in place of the embedded count/array pair.
struct SsaoTargetSetList { be<s32> mCount; gptr<SsaoTargetSet> mSets; };
WWHD_SIZE(SsaoTargetSetList, 8);

struct SsaoEffect {
    /* 0x000 */ ParameterRoot mRoot;                           // 0279A07C, name "aglssao"
    /* 0x1C8 */ gptr<const void> mRenderMethods;              // the effect's second vtable
    /* 0x1CC */ be<s32> mTargetSetCount;
    /* 0x1D0 */ gptr<SsaoTargetSet> mTargetSets;
    /* 0x1D4 */ ssao_parameters::OwnedParameterObject mParameters; // 027A84B8, name "ssao"
    /* 0x234 */ ssao_parameters::BooleanParameter mEnabled;
    /* 0x244 */ ssao_parameters::ScalarParameter mRadius;
    /* 0x254 */ ssao_parameters::ScalarParameter mFarDistance;
    /* 0x264 */ ssao_parameters::ScalarParameter mAttenuationDistance;
    /* 0x274 */ ssao_parameters::BooleanParameter mDistanceAttenuation;
    /* 0x284 */ ssao_parameters::BooleanParameter mReprojection;
    /* 0x294 */ ssao_parameters::ScalarParameter mMixRate;
    /* 0x2A4 */ ssao_parameters::ScalarParameter mDensity;
    /* 0x2B4 */ be<f32> mCenterWeight;
    /* 0x2B8 */ SsaoKernelVector mKernel[9];
    /* 0x348 */ be<f32> mKernelDepths[9];
    /* 0x36C */ ResourceRecord mResource;                      // 027A03A0
    /* 0x550 */ be<s32> mReductionLevel;                       // 1
    /* 0x554 */ be<s32> mFilterLevel;                          // 3
    /* 0x558 */ be<u8> mFilterEnabled;                         // 0, no other writer
    u8 mPadding[3];
    /* 0x55C */ TextureData mSampleTextures[16];               // 027BE6B8 (__construct_array)
    /* 0xE5C */ TextureSampler mSampleSamplers[16];            // 027BDF7C (__construct_array)
    /* 0x27DC */ gptr<void> mSampleImages[16];
    /* 0x281C */ ssao_parameters::BooleanParameter mRotateSamples;
    /* 0x282C */ ssao_parameters::IntegerParameter mBlurCount;
    /* 0x283C */ ssao_parameters::ScalarParameter mDepthOffset;
    /* 0x284C */ ssao_parameters::IntegerParameter mSamplePairCount;
    void prepareResources(s32 targetCount, sead::RegistryHeap* heap);
    u32 renderIfEnabled(u32 targetIndex, u32 width, u32 height, const TextureData* depth,
                        u32 previousShaderMode, f64 primaryFactor, f64 secondaryFactor);
    static SsaoEffect* construct(SsaoEffect* effect);
    static void destroy(SsaoEffect* effect, u32 flags);
};
WWHD_OFFSET(SsaoEffect, mRenderMethods, 0x1C8);
WWHD_OFFSET(SsaoEffect, mTargetSetCount, 0x1CC);
WWHD_OFFSET(SsaoEffect, mTargetSets, 0x1D0);
WWHD_OFFSET(SsaoEffect, mParameters, 0x1D4);
WWHD_OFFSET(SsaoEffect, mEnabled, 0x234);
WWHD_OFFSET(SsaoEffect, mRadius, 0x244);
WWHD_OFFSET(SsaoEffect, mFarDistance, 0x254);
WWHD_OFFSET(SsaoEffect, mAttenuationDistance, 0x264);
WWHD_OFFSET(SsaoEffect, mDistanceAttenuation, 0x274);
WWHD_OFFSET(SsaoEffect, mReprojection, 0x284);
WWHD_OFFSET(SsaoEffect, mMixRate, 0x294);
WWHD_OFFSET(SsaoEffect, mDensity, 0x2A4);
WWHD_OFFSET(SsaoEffect, mCenterWeight, 0x2B4);
WWHD_OFFSET(SsaoEffect, mKernel, 0x2B8);
WWHD_OFFSET(SsaoEffect, mKernelDepths, 0x348);
WWHD_OFFSET(SsaoEffect, mResource, 0x36C);
WWHD_OFFSET(SsaoEffect, mReductionLevel, 0x550);
WWHD_OFFSET(SsaoEffect, mFilterLevel, 0x554);
WWHD_OFFSET(SsaoEffect, mFilterEnabled, 0x558);
WWHD_OFFSET(SsaoEffect, mSampleTextures, 0x55C);
WWHD_OFFSET(SsaoEffect, mSampleSamplers, 0xE5C);
WWHD_OFFSET(SsaoEffect, mSampleImages, 0x27DC);
WWHD_OFFSET(SsaoEffect, mRotateSamples, 0x281C);
WWHD_OFFSET(SsaoEffect, mBlurCount, 0x282C);
WWHD_OFFSET(SsaoEffect, mDepthOffset, 0x283C);
WWHD_OFFSET(SsaoEffect, mSamplePairCount, 0x284C);
WWHD_SIZE(SsaoEffect, 0x285C);

void initializeSsaoTransforms();
void destroyResourceRecord(ResourceRecord* record, u32 flags);
void initializeSsaoSampling(SsaoEffect* effect);
}
