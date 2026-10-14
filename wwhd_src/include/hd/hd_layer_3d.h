/* HD main scene render layer. Private decomp source. */
#pragma once
#include "hd/hd_layer_base.h"
#include "hd/hd_multi_filter.h"
#include "hd/hd_render_commands.h"
struct hdScenePrimaryBinding_l { u8 _00[0x1D0]; gptr<struct hdScenePrimaryResource_l> mpResource; u8 _1D4[0x240-0x1D4]; be<u8> mEnabled; u8 _241[0x2E6C-0x610-0x241]; };
struct hdScenePacket_l { be<u32> mVtable,mFlags; u8 _08[0xC]; gptr<struct hdScenePacketResource_l> mpResource; u8 _18[0x3910-0x2E6C-0x18]; };
WWHD_OFFSET(hdScenePacket_l,mFlags,4);
struct hdSceneSecondaryBinding_l { u8 _00[0x4B8]; be<u8> mEnabled; u8 _4B9[0xF]; be<f32> mPhase; u8 _4CC[0x3C]; be<u32> mColor[4]; u8 _518[0x5C]; be<f32> mReference; u8 _578[0x770-0x578]; };
struct hdSceneBlur_l { u8 _00[0x2EC]; be<f32> mViewScale; u8 _2F0[0x334-0x2F0]; u8 mBinding[0xA48]; be<u8> mEnabled; u8 _D7D[0xEE0-0xD7D]; };
WWHD_OFFSET(hdSceneBlur_l,mBinding,0x334);
WWHD_OFFSET(hdSceneBlur_l,mEnabled,0xD7C);
WWHD_SIZE(hdSceneBlur_l,0xEE0);
struct hdSceneFilter_l { be<u32> mReady; u8 _04[0x2D0-4]; gptr<struct hdSceneMaterial_l> mpMaterial; u8 _2D4[0x2E4-0x2D4]; u8 mBinding[0x541C-0x5244]; be<u8> mEnabled; u8 _541D[0x5608-0x541D]; };
struct hdScenePostprocess_l { u8 _00[0x234]; be<u8> mEnabled; u8 _235[0x6AEC-0x5608-0x235]; };
WWHD_OFFSET(hdScenePostprocess_l,mEnabled,0x234);
struct hdSceneDrawTarget_l { u8 _00[0x3C]; gptr<void> mpTarget; };
struct hdLayer3D_c {
 hdLayerBase_c mBase;
 hdScenePrimaryBinding_l mPrimaryBinding;
 hdScenePacket_l mPacket;
 hdSceneSecondaryBinding_l mSecondaryBinding;
 hdSceneBlur_l mBlur;
 hdSceneFilter_l mFilter;
 hdScenePostprocess_l mPostprocess;
 gptr<void> mpSceneTexture,mpShadowTexture,mpAuxiliaryTexture;
 union {
  struct { gptr<void> mContexts[3]; gptr<hdSceneDrawTarget_l> mpDrawTarget; gptr<void> mExtraContexts[3]; };
  gptr<void> mOwnedContexts[7];
 };
 gptr<void> mPassResources[8];
 gptr<void> mAuxiliaryResources[2];
 u8 mDescriptor[0x90];
 gptr<void> mpAuxiliary0,mpShadowMatrix;
 be<u32> mState;
 be<u32> mDrawFlags;
 be<f32> mPhase;
 be<u32> mViewCount;
 be<f32> mAngle;
 be<f32> mViewScales[4];
 be<f32> mNearBias,mFarBias;
 void setFilterTarget(void* target);
 void setPrimaryTarget(void* target);
 void setSecondaryTarget(void* target);
 void bindPass(void* pass);
 void resetDrawFlags();
 f32 getViewScale(u32 index);
 void drawConfiguredPass(struct hdFilterRenderInfo_c* info);
};
WWHD_OFFSET(hdLayer3D_c,mPrimaryBinding,0x610);
WWHD_OFFSET(hdScenePrimaryBinding_l,mEnabled,0x240);
WWHD_OFFSET(hdLayer3D_c,mPacket,0x2E6C);
WWHD_OFFSET(hdLayer3D_c,mSecondaryBinding,0x3910);
WWHD_OFFSET(hdSceneSecondaryBinding_l,mEnabled,0x4B8);
WWHD_OFFSET(hdLayer3D_c,mFilter,0x4F60);
WWHD_OFFSET(hdSceneFilter_l,mBinding,0x2E4);
WWHD_OFFSET(hdSceneFilter_l,mEnabled,0x4BC);
WWHD_OFFSET(hdLayer3D_c,mpDrawTarget,0x6B04);
WWHD_OFFSET(hdLayer3D_c,mPassResources,0x6B14);
WWHD_OFFSET(hdLayer3D_c,mAuxiliaryResources,0x6B34);
WWHD_OFFSET(hdLayer3D_c,mDescriptor,0x6B3C);
WWHD_OFFSET(hdLayer3D_c,mState,0x6BD4);
WWHD_OFFSET(hdLayer3D_c,mDrawFlags,0x6BD8);
WWHD_OFFSET(hdLayer3D_c,mViewScales,0x6BE8);
WWHD_SIZE(hdLayer3D_c,0x6C00);
struct hdSceneDisplay_l { u8 _00[0x188]; be<f32> mWidth; u8 _18C[0x167C-0x18C]; be<f32> mRenderWidth; u8 _1680[4]; be<u32> mSpecialPass; be<u8> mUseAuxiliary,mSuspendSecondary,mDeferAuxiliary; u8 _168B; be<f32> mClearColor[4]; };
WWHD_OFFSET(hdSceneDisplay_l,mRenderWidth,0x167C);
WWHD_OFFSET(hdSceneDisplay_l,mSpecialPass,0x1684);
struct hdSceneDrawList_l { u8 _00[8]; be<u32> mMode; };
struct hdScenePlay_l {
 u8 _00[0x5AB0]; gptr<void> mpParticles; u8 _5AB4[0x15]; be<u8> mDrawWorld; u8 _5ACA[0x2A]; be<s8> mBackgroundIndex; u8 _5AF5[3];
 struct Background { be<u32> mEnabled; u8 _04[0x30]; } mBackgrounds[8];
 u8 _5C98[0xB0]; be<u8> mReverseOrder; u8 _5D49[3]; gptr<hdSceneDrawList_l> mpInitialList0,mpInitialList1;
 gptr<hdSceneDrawList_l> mpOpaqueList,mpTranslucentList,mpEarlyList,mpMiddleList;
 gptr<hdSceneDrawList_l> mpMainList,mpExtraList,mpLateList,mpModelList,mpPreParticleList;
 gptr<hdSceneDrawList_l> mpEffectList,mpParticleList0,mpParticleList1,mpParticleList2,mpParticleList3,mpParticleList4,mpParticleList5; gptr<hdSceneDrawList_l> mpLastList;
 u8 _5D98[4]; gptr<void> mpAuxiliaryDrawList; gptr<hdSceneDrawList_l> mpFinalList,mpInitialList2,mpFinalOverlayList;
};
WWHD_OFFSET(hdScenePlay_l,mBackgroundIndex,0x5AF4);
WWHD_OFFSET(hdScenePlay_l,mBackgrounds,0x5AF8);
WWHD_OFFSET(hdScenePlay_l,mReverseOrder,0x5D48);
WWHD_OFFSET(hdScenePlay_l,mpOpaqueList,0x5D54);
WWHD_OFFSET(hdScenePlay_l,mpFinalList,0x5DA0);
struct hdSceneColorState_c { be<u8> mEnabled,mBlend; u8 _02[2]; be<u32> mMode; u8 _08[4]; be<u8> mAlphaEnabled; u8 _0D[0x44-0xD]; be<u8> mRed,mGreen,mBlue,mAlpha; u8 _48[0x2C]; };
WWHD_OFFSET(hdSceneColorState_c,mMode,4);
WWHD_OFFSET(hdSceneColorState_c,mRed,0x44);
WWHD_OFFSET(hdSceneColorState_c,mGreen,0x45);
WWHD_OFFSET(hdSceneColorState_c,mBlue,0x46);
WWHD_OFFSET(hdSceneColorState_c,mAlpha,0x47);
WWHD_SIZE(hdSceneColorState_c,0x74);

WWHD_OFFSET(hdScenePlay_l,mDrawWorld,0x5AC9);
WWHD_OFFSET(hdScenePlay_l,mpAuxiliaryDrawList,0x5D9C);
struct hdSceneDrawArgument_c { gptr<void> mpTarget; be<u32> mEnabled; be<u32> mUnknown0,mIndex; union { be<u32> mWords[9]; struct { be<u32> mReserved0,mReserved1; gptr<void> mpScene,mpPrimary,mpShadow,mpAuxiliary,mpPacket; be<u32> mReserved2; gptr<void> mpTexture; }; }; };
WWHD_SIZE(hdSceneDrawArgument_c,0x34);

struct hdLayer3DHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdLayer3DHeaderZero_c,16);
struct hdLayer3DHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdLayer3DHeaderDescriptor_c,12);

struct hdSceneProjection_l { u8 mData[0xAC]; };
struct hdScenePerspective_l { u8 mData[0xB8]; };
struct hdSceneCamera_l { u8 mData[0x60]; };
struct hdSceneLayerBase_c { hdLayerBase_c mBase; hdSceneProjection_l mProjection,mDrcProjection; hdScenePerspective_l mPerspective; hdSceneCamera_l mCamera; };
WWHD_OFFSET(hdSceneLayerBase_c,mProjection,0x610);
WWHD_OFFSET(hdSceneLayerBase_c,mDrcProjection,0x6BC);
WWHD_OFFSET(hdSceneLayerBase_c,mPerspective,0x768);
WWHD_OFFSET(hdSceneLayerBase_c,mCamera,0x820);
WWHD_SIZE(hdSceneLayerBase_c,0x880);

struct hdSceneOwnedVtable_l { u8 _00[0x1C]; be<u32> mDestroy; };
struct hdSceneOwnedResource_l { u8 _00[0x18]; gptr<hdSceneOwnedVtable_l> mpVtable; };
WWHD_OFFSET(hdSceneOwnedResource_l,mpVtable,0x18);
WWHD_OFFSET(hdSceneOwnedVtable_l,mDestroy,0x1C);
struct hdSceneSize_l { be<f32> mWidth,mHeight; };

struct hdSceneMaterial_l { u8 _00[0xC78]; be<f32> mViewScale; };
WWHD_OFFSET(hdSceneMaterial_l,mViewScale,0xC78);
WWHD_OFFSET(hdSceneFilter_l,mpMaterial,0x2D0);
struct hdSceneScreens_l { u8 _00[0x1024]; gptr<gptr<void>> mpLayers; };
WWHD_OFFSET(hdSceneScreens_l,mpLayers,0x1024);

struct hdSceneTexture_l { u8 _00[8]; be<u32> mWidth,mHeight; u8 _10[0x68]; be<u8> mBlockWidth,mBlockHeight; u8 _7A[0x16]; be<u8> mNeedsResolve; u8 _91[0x13]; be<u32> mFormat; u8 mFormatState[0x80]; be<u32> mStoredFormat; };
WWHD_OFFSET(hdSceneTexture_l,mFormat,0xA4);
WWHD_OFFSET(hdSceneTexture_l,mFormatState,0xA8);
WWHD_OFFSET(hdSceneTexture_l,mStoredFormat,0x128);
struct hdSceneViewportVtable_l { u8 _00[0xC]; be<u32> mIsKindOf; u8 _10[0x1C]; be<u32> mClear; };
struct hdSceneViewport_l { union { struct { be<f32> mX,mY,mMinX,mMinY,mMaxX,mMaxY; }; be<u32> mWords[6]; }; gptr<hdSceneViewportVtable_l> mpVtable; gptr<void> mpColor,mpAuxiliaryColor; u8 _24[0x18]; gptr<hdSceneTexture_l> mpDepth; };
WWHD_OFFSET(hdSceneViewport_l,mpColor,0x1C);
WWHD_OFFSET(hdSceneViewport_l,mpAuxiliaryColor,0x20);
WWHD_OFFSET(hdSceneViewport_l,mpDepth,0x3C);
WWHD_SIZE(hdSceneViewport_l,0x40);

struct hdSceneFilterArg_c { gptr<hdSceneViewport_l> mpViewport; gptr<hdFilterBounds_c> mpBounds; be<u32> mFlags,mReserved; be<u8> mEnabled; u8 _11[3]; gptr<void> mpAuxiliary; };
WWHD_SIZE(hdSceneFilterArg_c,0x18);
WWHD_OFFSET(hdSceneFilterArg_c,mpAuxiliary,0x14);

struct hdSceneType_l { gptr<const char> mpName; };
WWHD_OFFSET(hdSceneBlur_l,mViewScale,0x2EC);

struct hdSceneDelegate_c { be<u32> mVtable; gptr<hdLayer3D_c> mpLayer; be<u16> mAdjustment; be<s16> mVirtualOffset; be<u32> mFunction; };
WWHD_SIZE(hdSceneDelegate_c,0x10);
struct hdSceneRenderStep_c { u8 _00[0xC]; be<u32> mVtable; SafeString mName; be<u32> mTarget,mFlags; hdSceneDelegate_c mDelegate; u8 _30[0x10]; };
WWHD_SIZE(hdSceneRenderStep_c,0x40);
WWHD_OFFSET(hdSceneRenderStep_c,mName,0x10);
WWHD_OFFSET(hdSceneRenderStep_c,mDelegate,0x20);
struct hdSceneSecondaryArg_c { be<u32> mEnabled,mFlags; };
struct hdScenePacketArg_c { u8 mData[0x14]; };

WWHD_OFFSET(hdSceneSecondaryBinding_l,mPhase,0x4C8);
WWHD_OFFSET(hdSceneSecondaryBinding_l,mColor,0x508);
WWHD_OFFSET(hdSceneSecondaryBinding_l,mReference,0x574);
WWHD_OFFSET(hdSceneDisplay_l,mSuspendSecondary,0x1689);
struct hdSceneAngle_l { u8 _00[0x28]; be<f32> mAngle; };
WWHD_OFFSET(hdSceneAngle_l,mAngle,0x28);

struct hdSceneViewMatrix_c { be<f32> mValues[12]; };
struct hdSceneVector_c { be<f32> x,y,z; };
struct hdSceneColor_c { be<f32> r,g,b,a; };
WWHD_SIZE(hdSceneViewMatrix_c,0x30);

WWHD_OFFSET(hdSceneTexture_l,mWidth,8);
WWHD_OFFSET(hdSceneTexture_l,mBlockWidth,0x78);
WWHD_OFFSET(hdSceneTexture_l,mNeedsResolve,0x90);

struct hdScenePrimaryResource_l { u8 _00[0x190]; u8 mDescriptor[4]; u8 _194[0x980-0x194]; gptr<hdSceneViewport_l> mpShadowViewport; };
struct hdScenePacketResource_l { u8 _00[0x184]; u8 mDescriptor[4]; };
struct hdSceneAuxiliaryResource_l { u8 _00[0x1CC]; u8 mDescriptor[4]; };
struct hdSceneProjectionMatrix_c { be<f32> mValues[16]; };
struct hdSceneParticleArg_c { gptr<hdLayerMatrix_c> mpView; hdSceneProjectionMatrix_c mProjection; be<f32> mFieldOfView,mAspect; gptr<hdSceneDrawArgument_c> mpContext; };
WWHD_OFFSET(hdSceneParticleArg_c,mpView,0);
WWHD_OFFSET(hdSceneParticleArg_c,mProjection,4);
WWHD_OFFSET(hdSceneParticleArg_c,mFieldOfView,0x44);
WWHD_OFFSET(hdSceneParticleArg_c,mAspect,0x48);
WWHD_OFFSET(hdSceneParticleArg_c,mpContext,0x4C);
WWHD_SIZE(hdSceneParticleArg_c,0x50);
/* The opaque pass reuses one stack slot for color state and particle arguments. */
struct hdSceneOpaqueScratch_c { union { hdSceneColorState_c mColorState; hdSceneParticleArg_c mParticles; }; };
WWHD_OFFSET(hdSceneOpaqueScratch_c,mColorState,0);
WWHD_OFFSET(hdSceneOpaqueScratch_c,mParticles,0);
WWHD_SIZE(hdSceneOpaqueScratch_c,0x74);
/* Escaping pointers retain the original relative stack positions of arguments, particles and textures. */
struct hdSceneOpaqueDrawStorage_c { hdSceneDrawArgument_c mArgument; hdSceneOpaqueScratch_c mScratch; u8 _A8[0xC0-0xA8]; hdFilterTexture_c mTexture; };
struct hdSceneTranslucentDrawStorage_c { hdSceneDrawArgument_c mArgument; hdSceneParticleArg_c mParticles,mWorldParticles; hdFilterTexture_c mTexture; };
struct hdSceneBlurDrawStorage_c { hdSceneDrawArgument_c mArgument; hdSceneParticleArg_c mParticles; };
WWHD_OFFSET(hdSceneOpaqueDrawStorage_c,mArgument,0);
WWHD_OFFSET(hdSceneOpaqueDrawStorage_c,mScratch,0x34);
WWHD_OFFSET(hdSceneOpaqueDrawStorage_c,mTexture,0xC0);
WWHD_OFFSET(hdSceneTranslucentDrawStorage_c,mArgument,0);
WWHD_OFFSET(hdSceneTranslucentDrawStorage_c,mParticles,0x34);
WWHD_OFFSET(hdSceneTranslucentDrawStorage_c,mWorldParticles,0x84);
WWHD_OFFSET(hdSceneTranslucentDrawStorage_c,mTexture,0xD4);
WWHD_OFFSET(hdSceneBlurDrawStorage_c,mArgument,0);
WWHD_OFFSET(hdSceneBlurDrawStorage_c,mParticles,0x34);
WWHD_SIZE(hdSceneOpaqueDrawStorage_c,0x258);
WWHD_SIZE(hdSceneTranslucentDrawStorage_c,0x26C);
WWHD_SIZE(hdSceneBlurDrawStorage_c,0x84);

WWHD_OFFSET(hdScenePacket_l,mpResource,0x14);
WWHD_OFFSET(hdScenePrimaryBinding_l,mpResource,0x1D0);
WWHD_OFFSET(hdScenePlay_l,mpParticles,0x5AB0);

struct hdSceneShadowTextureOwner_l { u8 _00[0x1620]; hdFilterTexture_c mTextures[2]; u8 _1950[0x10]; gptr<void> mpPrimaryDescriptor; u8 _1964[8]; be<f32> mViewScale; };
struct hdSceneShadowRenderer_l { u8 _00[0x1D0]; gptr<hdSceneShadowTextureOwner_l> mpOwner; be<u32> mFlags; u8 _1D8[0x5A4-0x1D8]; be<u8> mEnabled; };
struct hdSceneShadowLayer_l { u8 _00[0x15D4]; hdSceneShadowRenderer_l mRenderer; };
WWHD_OFFSET(hdScenePrimaryResource_l,mpShadowViewport,0x980);
WWHD_OFFSET(hdSceneShadowRenderer_l,mpOwner,0x1D0);
WWHD_OFFSET(hdSceneShadowRenderer_l,mFlags,0x1D4);
WWHD_OFFSET(hdSceneShadowRenderer_l,mEnabled,0x5A4);
WWHD_OFFSET(hdSceneShadowTextureOwner_l,mTextures,0x1620);
WWHD_OFFSET(hdSceneShadowTextureOwner_l,mpPrimaryDescriptor,0x1960);
WWHD_OFFSET(hdSceneShadowTextureOwner_l,mViewScale,0x196C);

struct hdSceneDepthBias_c { be<f32> mBias,mSlope; be<u32> mWords[3]; };
WWHD_SIZE(hdSceneDepthBias_c,0x14);

WWHD_OFFSET(hdSceneDisplay_l,mUseAuxiliary,0x1688);
WWHD_OFFSET(hdSceneDisplay_l,mDeferAuxiliary,0x168A);
WWHD_OFFSET(hdSceneDisplay_l,mClearColor,0x168C);
WWHD_OFFSET(hdSceneViewportVtable_l,mClear,0x2C);

WWHD_OFFSET(hdLayer3D_c,mPhase,0x6BDC);
WWHD_OFFSET(hdLayer3D_c,mViewCount,0x6BE0);
WWHD_OFFSET(hdLayer3D_c,mAngle,0x6BE4);
WWHD_OFFSET(hdLayer3D_c,mNearBias,0x6BF8);
WWHD_OFFSET(hdLayer3D_c,mFarBias,0x6BFC);
WWHD_OFFSET(hdLayer3D_c,mOwnedContexts,0x6AF8);
WWHD_OFFSET(hdLayer3D_c,mExtraContexts,0x6B08);
WWHD_OFFSET(hdLayer3D_c,mpAuxiliary0,0x6BCC);
WWHD_OFFSET(hdSceneViewport_l,mpVtable,0x18);
WWHD_OFFSET(hdSceneViewport_l,mMaxX,0x10);
WWHD_OFFSET(hdSceneViewport_l,mMaxY,0x14);
WWHD_OFFSET(hdScenePlay_l,mpInitialList0,0x5D4C);
WWHD_OFFSET(hdScenePlay_l,mpMiddleList,0x5D60);
WWHD_OFFSET(hdScenePlay_l,mpPreParticleList,0x5D74);
WWHD_OFFSET(hdScenePlay_l,mpParticleList0,0x5D7C);
WWHD_OFFSET(hdScenePlay_l,mpFinalOverlayList,0x5DA8);
WWHD_OFFSET(hdSceneDrawArgument_c,mpScene,0x18);
WWHD_OFFSET(hdSceneDrawArgument_c,mpPrimary,0x1C);
WWHD_OFFSET(hdSceneDrawArgument_c,mpShadow,0x20);
WWHD_OFFSET(hdSceneDrawArgument_c,mpAuxiliary,0x24);
WWHD_OFFSET(hdSceneDrawArgument_c,mpPacket,0x28);
WWHD_OFFSET(hdSceneDrawArgument_c,mReserved2,0x2C);
WWHD_OFFSET(hdSceneDrawArgument_c,mpTexture,0x30);

WWHD_OFFSET(hdLayer3D_c,mpSceneTexture,0x6AEC);
WWHD_OFFSET(hdLayer3D_c,mpShadowTexture,0x6AF0);
WWHD_OFFSET(hdLayer3D_c,mpAuxiliaryTexture,0x6AF4);
WWHD_OFFSET(hdLayer3D_c,mpShadowMatrix,0x6BD0);

WWHD_OFFSET(hdSceneViewport_l,mWords,0);
