/* agl light prepass: the per-view light buffers, the four light collections it drives (point, directed, spatial and
 * paired points) and the specular-curve texture it samples.
 *
 * The render buffer, colour and depth surfaces are AGL#14's, the texture data, sampler and uniform buffer AGL#15's
 * and the vertex input set AGL#16's canonical types; the view's camera prefix, the collection objects' derived parts
 * and the property facade are local views of units that have not merged yet (AGL#7, AGL#18/#19, AGL#10). Layouts come from WWHD's constructor (create), configure and
 * the accessors in this file. */
#pragma once
#include "agl/ParameterCollections.h"
#include "agl/RenderBuffer.h"
#include "agl/TextureSampler.h"
#include "agl/UniformBuffer.h"
#include "agl/VertexInput.h"
#include "agl/CurveParameter.h"
#include "sead/RenderState.h"
#include "sead/Viewport.h"
#include "sead/HeapRegistry.h"
namespace agl {

enum SamplerDirtyFlags { SamplerDirty_Texture = 2, SamplerDirty_Modes = 4 };

/* ---- one view of the light prepass ---- */
// The camera and projection prefix, constructed and set by AGL#7 (local view of the fields this TU reads).
struct LightViewCamera_l {
    sead::RegistryMatrix34 mView;
    be<f32> mNear;
    be<f32> mFar;
    u8 mCameraState[0xAC];
    be<f32> mProjectionWidth;
    be<f32> mProjectionAspect;
    be<u32> mProjectionReserved;
    be<f32> mHorizontalScale;
    be<f32> mVerticalScale;
    sead::RegistryMatrix34 mBaseProjection;
    sead::RegistryMatrix44 mProjection;
};
enum LightViewFlags { LightView_Empty = 1 };
enum LightViewPhase { LightPhase_Target = 1, LightPhase_Released = 2, LightPhase_Rendered = 4, LightPhase_Mask = 7 };
struct LightView {
    LightViewCamera_l mCamera;
    UniformBuffer mUniform;
    TextureSampler mPrimaryTexture;
    TextureSampler mLightingTexture;
    TextureSampler mSpecularTexture;
    TextureSampler mFollowingTexture;
    gptr<TextureData> mTemporaryTexture;
    RenderBuffer mTarget;
    ColorSurface mPrimaryColor;
    ColorSurface mFollowingColor;
    DepthSurface mDepth;
    sead::RenderState mResolveState;
    be<u32> mFlags;
    be<u32> mPhase;
    u8 mPostprocess[0xCC];                  // AGL#17's post-process state
    be<u32> mPostprocessStorage[6];
    be<u8> mTargetHasDepth;
    be<u8> mSeparateEnabled;
    be<u8> mFollowingEnabled;
    u8 mSelectionPadding;
    sead::RegistryMatrix34 mViewCopy;
    sead::RegistryMatrix44 mViewProjection;
    be<f32> mScaleX;
    be<f32> mScaleY;
    be<f32> mFade;
    be<f32> mDepthRange;
    be<f32> mInverseDepthRange;
    be<f32> mProjectionScale;
    be<f32> mClipSign;
};

/* ---- draw context handed to the collections and the draw callbacks ---- */
struct LightPrepass;
struct LightDrawContext {
    gptr<LightPrepass> mOwner;
    gptr<RenderBuffer> mTarget;
    gptr<LightView> mView;
    gptr<UniformBuffer> mUniform;
    be<u32> mViewIndex;
    gptr<TextureSampler> mFollowingTexture;
    gptr<TextureSampler> mLightingTexture;
    gptr<TextureSampler> mCurveTexture;
    gptr<TextureSampler> mSpecularTexture;
    be<u32> mUser;
    be<u8> mFollowing;
    u8 mPadding[3];
};
struct LightCallbackMethods_l { u8 mHeader[0xC]; be<u32> mInvoke; };
struct LightCallback_l { gptr<LightCallbackMethods_l> mMethods; };

/* ---- light parameters passed by the game ---- */
struct LightVector { be<f32> x, y, z; };
struct LightColorBits { be<u32> red, green, blue, alpha; };   // float colour words, copied bit for bit
struct LightScalarPair { be<f32> first, second; };
struct SpecularKeys { be<u32> mWords[6]; };

/* ---- the four light collections (AGL#18/#19 derive them; configure constructs them inline) ---- */
struct PointLightCollection_l {
    CompactCollection mBase;
    VertexInputSet mVertexInput;
    u8 mTail[4];
};
struct DirectedLightCollection_l {
    ExtendedCollection mBase;
    be<u32> mMode;
    VertexInputSet mVertexInput;
    u8 mTail[4];
};
struct SpatialLightCollection_l {
    SpatialCollection mBase;
    VertexInputSet mVertexInput;
    u8 mTail[4];
};
struct PairedLightCollection_l {
    CurveCollection mBase;
    VertexInputSet mVertexInput;
    u8 mTail[4];
};

/* ---- the light prepass ---- */
enum LightPrepassFlags {
    LightPrepass_Resolve = 0x20,
    LightPrepass_Separate = 0x100,
    LightPrepass_DepthSeparate = 0x200,
    LightPrepass_KindFilter = 0x800,
    LightPrepass_Combined = 0x2000,
    LightPrepass_PairPoints = 0x20000,
    LightPrepass_SkipEmpty = 0x40000,
};
enum LightPrepassDirty { LightDirty_Tracking = 1, LightDirty_Changed = 2 };
enum LightCollectionIndex {
    LightCollection_Point,
    LightCollection_Directed,
    LightCollection_Spatial,
    LightCollection_Paired,
    LightCollection_Count
};
struct LightPrepass {
    gptr<void> mMethods;
    be<u32> mFlags;
    be<u32> mDirty;
    be<u8> mEnabledKinds;
    u8 mEnabledPadding[3];
    be<u32> mViewCount;
    gptr<LightView> mViews;
    gptr<ParameterCollection> mCollections[LightCollection_Count];
    be<u32> mExtraCount;
    gptr<gptr<ParameterCollection>> mExtraCollections;
    be<f32> mWidthScale;
    be<f32> mHeightScale;
    be<u32> mMode;
    sead::CompactRenderState mMainState;
    sead::CompactRenderState mMainDepthState;
    sead::RenderState mSeparateState;
    sead::RenderState mSeparateDepthState;
    sead::CompactRenderState mFollowingState;
    sead::RenderState mFollowingSeparateState;
    CollectionFacade_l mProperties;
    u8 mPropertyState[0x1D0];
    be<u32> mCurveSampleCount;
    gptr<be<u32>> mCurveSamples;
    be<u32> mCurveSampleState;
    TextureData mCurveSurface;
    TextureSampler mCurveTexture;
    CurveParameter_l mSpecularCurve;
    be<f32> mCurveScaleX;
    be<f32> mCurveOffsetX;
    be<f32> mCurveScaleY;
    be<f32> mCurveOffsetY;
    be<f32> mPairThreshold;
    gptr<LightCallback_l> mBeforeDraw;
    gptr<LightCallback_l> mAfterDraw;
    u8 mDrawingOptions[0x40];
    be<u8> mCurveEnabled;
    u8 mCurveEnabledPadding[3];
    be<u32> mOutputIndex;
    be<f32> mThreshold;
    be<u32> mViewportWidth, mViewportHeight, mThumbnailWidth, mThumbnailHeight, mTileSize;

    static LightPrepass* create(LightPrepass* object);
    static void destroy(LightPrepass* object, u32 flags);
    void dirtyTextures();
    void buildCurveTexture();
    void configure(const LightPrepassConfig* config, sead::RegistryHeap* heap);
    void update();
    void updateView(u32 index);
    void updateViewInputs(u32 index, void* projection, const void* camera, const void* viewport);
    void refreshCollections(void* context);
    u32 skipView(u32 index) const;
    void uploadView(u32 index, u32 invalidateGpu);
    void clearView(u32 index);
    TextureData* prepareTarget(u32 index, u32 width, u32 height, u32 clear);
    void selectState(const LightDrawContext* context, u32 separate, u32 force, u32 following);
    u32 drawView(u32 index, u32 reservedContext, u32 reservedFlags, u8 following, u32 user);
    void commitColors(u32 index);
    u32 render(u32 index, u32 drawContext, u32 drawFlags, const TextureData* following, const DepthSurface* depth,
               const TextureData* lighting, u32 user, const TextureData* specular);
    void releaseTemporary(u32 index);
    void setPoint(u32 index, const LightVector* position, const LightColorBits* color, u32 useFalloff,
                  const LightColorBits* falloff, u32 castsShadow, u32 visible, u8 dynamic, f64 radius, f64 intensity,
                  f64 attenuation);
    void setDirected(u32 index, const LightVector* position, const LightVector* direction, const LightColorBits* color,
                     u32 visible, const LightColorBits* falloff, f64 inner, f64 outer, f64 intensity, f64 attenuation);
};
}

WWHD_OFFSET(agl::LightViewCamera_l, mNear, 0x30);
WWHD_OFFSET(agl::LightViewCamera_l, mFar, 0x34);
WWHD_OFFSET(agl::LightViewCamera_l, mProjectionWidth, 0xE4);
WWHD_OFFSET(agl::LightViewCamera_l, mProjectionAspect, 0xE8);
WWHD_OFFSET(agl::LightViewCamera_l, mHorizontalScale, 0xF0);
WWHD_OFFSET(agl::LightViewCamera_l, mVerticalScale, 0xF4);
WWHD_OFFSET(agl::LightViewCamera_l, mBaseProjection, 0xF8);
WWHD_OFFSET(agl::LightViewCamera_l, mProjection, 0x128);
WWHD_SIZE(agl::LightViewCamera_l, 0x168);
WWHD_OFFSET(agl::LightView, mUniform, 0x168);
WWHD_OFFSET(agl::LightView, mPrimaryTexture, 0x184);
WWHD_OFFSET(agl::LightView, mLightingTexture, 0x31C);
WWHD_OFFSET(agl::LightView, mSpecularTexture, 0x4B4);
WWHD_OFFSET(agl::LightView, mFollowingTexture, 0x64C);
WWHD_OFFSET(agl::LightView, mTemporaryTexture, 0x7E4);
WWHD_OFFSET(agl::LightView, mTarget, 0x7E8);
WWHD_OFFSET(agl::LightView, mPrimaryColor, 0x828);
WWHD_OFFSET(agl::LightView, mFollowingColor, 0x970);
WWHD_OFFSET(agl::LightView, mDepth, 0xAB8);
WWHD_OFFSET(agl::LightView, mResolveState, 0xC0C);
WWHD_OFFSET(agl::LightView, mFlags, 0xD28);
WWHD_OFFSET(agl::LightView, mPhase, 0xD2C);
WWHD_OFFSET(agl::LightView, mPostprocess, 0xD30);
WWHD_OFFSET(agl::LightView, mPostprocessStorage, 0xDFC);
WWHD_OFFSET(agl::LightView, mTargetHasDepth, 0xE14);
WWHD_OFFSET(agl::LightView, mSeparateEnabled, 0xE15);
WWHD_OFFSET(agl::LightView, mFollowingEnabled, 0xE16);
WWHD_OFFSET(agl::LightView, mViewCopy, 0xE18);
WWHD_OFFSET(agl::LightView, mViewProjection, 0xE48);
WWHD_OFFSET(agl::LightView, mScaleX, 0xE88);
WWHD_OFFSET(agl::LightView, mScaleY, 0xE8C);
WWHD_OFFSET(agl::LightView, mFade, 0xE90);
WWHD_OFFSET(agl::LightView, mDepthRange, 0xE94);
WWHD_OFFSET(agl::LightView, mInverseDepthRange, 0xE98);
WWHD_OFFSET(agl::LightView, mProjectionScale, 0xE9C);
WWHD_OFFSET(agl::LightView, mClipSign, 0xEA0);
WWHD_SIZE(agl::LightView, 0xEA4);
WWHD_OFFSET(agl::LightDrawContext, mTarget, 4);
WWHD_OFFSET(agl::LightDrawContext, mView, 8);
WWHD_OFFSET(agl::LightDrawContext, mUniform, 0xC);
WWHD_OFFSET(agl::LightDrawContext, mViewIndex, 0x10);
WWHD_OFFSET(agl::LightDrawContext, mFollowingTexture, 0x14);
WWHD_OFFSET(agl::LightDrawContext, mLightingTexture, 0x18);
WWHD_OFFSET(agl::LightDrawContext, mCurveTexture, 0x1C);
WWHD_OFFSET(agl::LightDrawContext, mSpecularTexture, 0x20);
WWHD_OFFSET(agl::LightDrawContext, mUser, 0x24);
WWHD_OFFSET(agl::LightDrawContext, mFollowing, 0x28);
WWHD_SIZE(agl::LightDrawContext, 0x2C);
WWHD_OFFSET(agl::LightCallbackMethods_l, mInvoke, 0xC);
WWHD_SIZE(agl::LightVector, 12);
WWHD_SIZE(agl::LightColorBits, 16);
WWHD_SIZE(agl::LightScalarPair, 8);
WWHD_SIZE(agl::SpecularKeys, 24);
WWHD_OFFSET(agl::PointLightCollection_l, mVertexInput, 0x1C);
WWHD_SIZE(agl::PointLightCollection_l, 0x114);
WWHD_OFFSET(agl::DirectedLightCollection_l, mMode, 0x1C);
WWHD_OFFSET(agl::DirectedLightCollection_l, mVertexInput, 0x20);
WWHD_SIZE(agl::DirectedLightCollection_l, 0x118);
WWHD_SIZE(agl::SpatialLightCollection_l, 0x114);
WWHD_SIZE(agl::PairedLightCollection_l, 0x114);
WWHD_OFFSET(agl::LightPrepass, mFlags, 4);
WWHD_OFFSET(agl::LightPrepass, mDirty, 8);
WWHD_OFFSET(agl::LightPrepass, mEnabledKinds, 0xC);
WWHD_OFFSET(agl::LightPrepass, mViewCount, 0x10);
WWHD_OFFSET(agl::LightPrepass, mViews, 0x14);
WWHD_OFFSET(agl::LightPrepass, mCollections, 0x18);
WWHD_OFFSET(agl::LightPrepass, mExtraCount, 0x28);
WWHD_OFFSET(agl::LightPrepass, mExtraCollections, 0x2C);
WWHD_OFFSET(agl::LightPrepass, mWidthScale, 0x30);
WWHD_OFFSET(agl::LightPrepass, mHeightScale, 0x34);
WWHD_OFFSET(agl::LightPrepass, mMode, 0x38);
WWHD_OFFSET(agl::LightPrepass, mMainState, 0x3C);
WWHD_OFFSET(agl::LightPrepass, mMainDepthState, 0xB0);
WWHD_OFFSET(agl::LightPrepass, mSeparateState, 0x124);
WWHD_OFFSET(agl::LightPrepass, mSeparateDepthState, 0x240);
WWHD_OFFSET(agl::LightPrepass, mFollowingState, 0x35C);
WWHD_OFFSET(agl::LightPrepass, mFollowingSeparateState, 0x3D0);
WWHD_OFFSET(agl::LightPrepass, mProperties, 0x4EC);
WWHD_OFFSET(agl::LightPrepass, mCurveSampleCount, 0x6D0);
WWHD_OFFSET(agl::LightPrepass, mCurveSamples, 0x6D4);
WWHD_OFFSET(agl::LightPrepass, mCurveSurface, 0x6DC);
WWHD_OFFSET(agl::LightPrepass, mCurveTexture, 0x76C);
WWHD_OFFSET(agl::LightPrepass, mSpecularCurve, 0x904);
WWHD_OFFSET(agl::LightPrepass, mCurveScaleX, 0xA28);
WWHD_OFFSET(agl::LightPrepass, mPairThreshold, 0xA38);
WWHD_OFFSET(agl::LightPrepass, mBeforeDraw, 0xA3C);
WWHD_OFFSET(agl::LightPrepass, mAfterDraw, 0xA40);
WWHD_OFFSET(agl::LightPrepass, mCurveEnabled, 0xA84);
WWHD_OFFSET(agl::LightPrepass, mOutputIndex, 0xA88);
WWHD_OFFSET(agl::LightPrepass, mThreshold, 0xA8C);
WWHD_OFFSET(agl::LightPrepass, mViewportWidth, 0xA90);
WWHD_OFFSET(agl::LightPrepass, mTileSize, 0xAA0);
WWHD_SIZE(agl::LightPrepass, 0xAA4);
