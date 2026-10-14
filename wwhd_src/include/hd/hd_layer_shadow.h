/* HD shadow rendering layer. Private decomp source. */
#pragma once
#include "hd/hd_layer_base.h"
#include "hd/hd_multi_filter.h"
#include "hd/hd_layer_3d.h"
struct hdShadowDelegate_c { be<u32> mVtable; gptr<void> mpOwner; be<u16> mAdjustment; be<s16> mVirtualOffset; be<u32> mFunction; };
WWHD_SIZE(hdShadowDelegate_c,0x10);
struct hdShadowView_l;
struct hdShadowSurface_l {
 u8 _00[0x244]; be<u32> mMode; u8 _248[0xC]; be<u32> mCascadeCount;
 u8 _258[0xC]; gptr<be<f32>> mpDistances; u8 _268[0x2BC-0x268]; be<s32> mCapacity; gptr<hdShadowView_l> mpViews; u8 mResolution[8];
 u8 _2CC[0x360-0x2CC]; be<u8> mFlag0; u8 _361[0xF];
 be<u8> mFlag1; u8 _371[0xF]; be<u8> mFlag2; u8 _381[0xF];
 be<u8> mFlag3; u8 _391[0xF]; be<u8> mFlag4; u8 _3A1[0x23];
 be<u32> mTextureCount; u8 _3C8[0xEA8-0x3C8];
 be<u8> mDirectionEnabled; u8 _EA9[3]; cXyz mDirection;
 u8 _EB8[0xF20-0xEB8]; gptr<hdShadowDelegate_c> mpDelegate;
 hdShadowDelegate_c mDrawDelegate; u8 _F34[0x14];
};
WWHD_OFFSET(hdShadowSurface_l,mMode,0x244);
WWHD_OFFSET(hdShadowSurface_l,mCascadeCount,0x254);
WWHD_OFFSET(hdShadowSurface_l,mCapacity,0x2BC);
WWHD_OFFSET(hdShadowSurface_l,mResolution,0x2C4);
WWHD_OFFSET(hdShadowSurface_l,mFlag0,0x360);
WWHD_OFFSET(hdShadowSurface_l,mFlag4,0x3A0);
WWHD_OFFSET(hdShadowSurface_l,mTextureCount,0x3C4);
WWHD_OFFSET(hdShadowSurface_l,mDirectionEnabled,0xEA8);
WWHD_OFFSET(hdShadowSurface_l,mDirection,0xEAC);
WWHD_OFFSET(hdShadowSurface_l,mpDelegate,0xF20);
WWHD_OFFSET(hdShadowSurface_l,mDrawDelegate,0xF24);
WWHD_SIZE(hdShadowSurface_l,0xF48);
struct hdShadowStep_c { u8 _00[0xC]; be<u32> mVtable; SafeString mName; be<u32> mTarget,mFlags; hdShadowDelegate_c mDelegate; u8 _30[0x10]; };
WWHD_OFFSET(hdShadowStep_c,mName,0x10);
WWHD_OFFSET(hdShadowStep_c,mDelegate,0x20);
WWHD_SIZE(hdShadowStep_c,0x40);
struct hdShadowEnvironment_l { u8 _00[0xC40]; u8 mShadow; };
WWHD_OFFSET(hdShadowEnvironment_l,mShadow,0xC40);
struct hdShadowBuffer_l { u8 mData[0xC]; };
struct hdShadowBinding_l { u8 _00[0x420]; be<u8> mEnabled; u8 _421[0x558-0x421]; };
WWHD_OFFSET(hdShadowBinding_l,mEnabled,0x420);
WWHD_SIZE(hdShadowBinding_l,0x558);
struct hdShadowRenderer_l {
 union {
  struct { u8 _00[4]; hdShadowBinding_l mBinding; u8 _55C[0x54]; cXyz mLightDirection; be<f32> mDirectionWeight; u8 _5C0[4]; };
  struct { u8 _view00[0x1D0]; gptr<hdSceneShadowTextureOwner_l> mpTextureOwner; u8 _view1D4[0x434-0x1D4]; be<u8> mUseFirstView; };
 };
};
WWHD_OFFSET(hdShadowRenderer_l,mpTextureOwner,0x1D0);
WWHD_OFFSET(hdShadowRenderer_l,mUseFirstView,0x434);
WWHD_OFFSET(hdShadowRenderer_l,mBinding,4);
WWHD_SIZE(hdShadowRenderer_l,0x5C4);
struct hdShadowParameters_c { be<f32> mValues[4]; };
struct hdShadowResolution_c { be<u32> mWidth,mHeight; };
struct hdLayerShadow_c {
 hdLayerBase_c mBase;
 hdShadowSurface_l mSurface;
 gptr<void> mpDrawContext;
 hdShadowStep_c mFinishStep;
 gptr<struct hdShadowEnvironmentParameters_l> mpEnvironmentShadow;
 hdShadowBuffer_l mBuffers[4];
 gptr<hdLayer3D_c> mpScene;
 hdShadowRenderer_l mRenderer;
 be<f32> mDistance,mFadeDistance;
 be<s32> mCascadeCount;
 hdShadowParameters_c mCascadeDistances,mCascadeScales,mBiasParameters;
 be<f32> mDepthBias,mBlend;
 hdShadowResolution_c mResolution;
 void setScene(hdLayer3D_c* scene);
 void setRenderTarget(void* target);
 void finish();
};
WWHD_OFFSET(hdLayerShadow_c,mSurface,0x610);
WWHD_OFFSET(hdLayerShadow_c,mpDrawContext,0x1558);
WWHD_OFFSET(hdLayerShadow_c,mFinishStep,0x155C);
WWHD_OFFSET(hdLayerShadow_c,mpEnvironmentShadow,0x159C);
WWHD_OFFSET(hdLayerShadow_c,mBuffers,0x15A0);
WWHD_OFFSET(hdLayerShadow_c,mpScene,0x15D0);
WWHD_OFFSET(hdLayerShadow_c,mRenderer,0x15D4);
WWHD_OFFSET(hdLayerShadow_c,mDistance,0x1B98);
WWHD_OFFSET(hdLayerShadow_c,mCascadeCount,0x1BA0);
WWHD_OFFSET(hdLayerShadow_c,mCascadeDistances,0x1BA4);
WWHD_OFFSET(hdLayerShadow_c,mCascadeScales,0x1BB4);
WWHD_OFFSET(hdLayerShadow_c,mBiasParameters,0x1BC4);
WWHD_OFFSET(hdLayerShadow_c,mDepthBias,0x1BD4);
WWHD_OFFSET(hdLayerShadow_c,mResolution,0x1BDC);
WWHD_SIZE(hdLayerShadow_c,0x1BE4);

struct hdShadowLight_l { u8 _00[0x14]; cXyz mDirection; u8 _20[0xB20-0x20]; cXyz mPosition; u8 _B2C[0x1030-0xB2C]; be<f32> mFade; u8 _1034[4]; be<f32> mIntensity,mBlend; u8 _1040[4]; be<f32> mOcclusion; };
WWHD_OFFSET(hdShadowLight_l,mDirection,0x14);
WWHD_OFFSET(hdShadowLight_l,mPosition,0xB20);
struct hdShadowCamera_l { u8 _00[0xDC]; cXyz mPosition; u8 _E8[0x258-0xE8]; cXyz mEye,mTarget; u8 _270[0x10]; be<f32> mHeight; u8 _284[0x100]; be<u32> mMode; u8 _388[0x42C]; cXyz mEyeOffset,mTargetOffset; be<f32> mHeightOffset; };
WWHD_OFFSET(hdShadowCamera_l,mPosition,0xDC);
struct hdShadowStageVtable_l { u8 _00[0x15C]; be<u32> mGetStageInfo; };
struct hdShadowPlay_l { u8 _00[0x12A0]; u8 mBackground[4]; u8 _12A4[0x5150-0x12A4]; gptr<hdShadowStageVtable_l> mpStageVtable; u8 _5154[0x5AF8-0x5154]; gptr<hdShadowCamera_l> mpCamera; u8 _5AFC[0x34]; be<s8> mBackgroundIndex; u8 _5B31[0x1AB]; be<u32> mEventFlags; };
WWHD_OFFSET(hdShadowPlay_l,mpCamera,0x5AF8);

struct hdLayerShadowHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdLayerShadowHeaderZero_c,16);
struct hdLayerShadowHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdLayerShadowHeaderDescriptor_c,12);

struct hdShadowViewportVtable_l { u8 _00[0xC]; be<u32> mIsKindOf; };
struct hdShadowViewport_l { be<f32> mX,mY,mMinX,mMinY,mMaxX,mMaxY; gptr<hdShadowViewportVtable_l> mpVtable; u8 _1C[0x20]; gptr<void> mpTarget; };
WWHD_OFFSET(hdShadowViewport_l,mpTarget,0x3C);
struct hdShadowSafeStringVtable_l { u8 _00[0x14]; be<u32> mEnsureTerminated; };
WWHD_OFFSET(hdShadowSafeStringVtable_l,mEnsureTerminated,0x14);
struct hdShadowStageRange_c { union { struct { be<f32> mNear,mMiddle,mFar; }; be<f32> mValues[3]; }; };
WWHD_OFFSET(hdShadowStageRange_c,mValues,0);
WWHD_SIZE(hdShadowStageRange_c,12);
struct hdShadowDrawList_l { u8 _00[8]; be<u32> mDrawMode; };
struct hdShadowDrawPlay_l {
 u8 _00[0x5AC9]; be<u8> mDrawWorld; u8 _5ACA[0x5D54-0x5ACA];
 gptr<hdShadowDrawList_l> mpOpaqueList,mpTranslucentList;
 u8 _5D5C[8]; gptr<hdShadowDrawList_l> mpMainList,mpExtraList;
 u8 _5D6C[4]; gptr<hdShadowDrawList_l> mpModelList;
 u8 _5D74[4]; gptr<hdShadowDrawList_l> mpEffectList;
 u8 _5D7C[0x18]; gptr<hdShadowDrawList_l> mpLastList;
};
WWHD_OFFSET(hdShadowDrawPlay_l,mpOpaqueList,0x5D54);
WWHD_OFFSET(hdShadowDrawPlay_l,mpMainList,0x5D64);
WWHD_OFFSET(hdShadowDrawPlay_l,mpModelList,0x5D70);
WWHD_OFFSET(hdShadowDrawPlay_l,mpLastList,0x5D94);
struct hdShadowDrawState_c {
 be<u8> mEnabled,mBlend; u8 _02[2]; be<u32> mMode;
 u8 _08[4]; be<u8> mAlphaEnabled; u8 _0D[0x38-0xD];
 be<u8> mDepthEnabled; u8 _39[0x44-0x39];
 be<u8> mRed,mGreen,mBlue,mAlpha; u8 _48[0x6C-0x48];
 be<u8> mMaskRed,mMaskGreen,mMaskBlue; u8 _6F[5];
};
WWHD_SIZE(hdShadowDrawState_c,0x74);
struct hdShadowDrawContext_l { u8 _00[4]; be<u32> mFlags; be<s32> mIndex; u8 _0C[4]; gptr<hdShadowDrawState_c> mpState; gptr<hdShadowBuffer_l> mpBuffer; };
WWHD_OFFSET(hdShadowDrawContext_l,mpState,0x10);
WWHD_OFFSET(hdShadowDrawContext_l,mpBuffer,0x14);
struct hdShadowView_l { u8 _00[0x7C]; be<u32> mMode; u8 _80[0x254-0x80]; u8 mViewMatrix[0x30]; u8 mProjectionMatrix[0x30]; u8 _2B4[0x10]; be<f32> mShadowMatrix[16]; u8 _304[0x5B4-0x304]; };
WWHD_SIZE(hdShadowView_l,0x5B4);
struct hdShadowViewGroup_l { u8 _00[0x2BC]; be<u32> mCount; gptr<hdShadowView_l> mpViews; };
struct hdShadowCascadeInfo_c { be<u32> mIndex; gptr<hdShadowViewGroup_l> mpGroup; };
struct hdShadowDebugState_l { u8 _00[0x240]; u8 mMatrix; };
struct hdShadowDebugRenderer_l { u8 _00[0x148]; gptr<hdShadowDebugState_l> mpShadowState; u8 _14C[0x40]; u8 mCleanup; };
WWHD_OFFSET(hdShadowDebugRenderer_l,mpShadowState,0x148);
WWHD_OFFSET(hdShadowDebugRenderer_l,mCleanup,0x18C);

struct hdShadowEnvironmentParameters_l {
 u8 _00[0x74]; be<f32> mMatrices[3][16]; u8 _134[0x40]; be<f32> mDistances[4]; be<f32> mSceneMatrix[16]; be<f32> mOcclusion,mIntensity,mFade; u8 _1D0[4]; be<f32> mFadeParameters[4]; be<f32> mCascadeScales[4],mBiasParameters[4]; be<f32> mDepthBias;
 be<f32> mRange,mNear,mInverseRange,mTanHalfFieldOfView; be<f32> mWidth,mHeight,mInverseWidth,mInverseHeight; be<f32> mColors[4][4];
};
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mCascadeScales,0x1E4);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mBiasParameters,0x1F4);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mDepthBias,0x204);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mColors,0x228);
struct hdShadowPassArg_c { be<u32> mPass; };

struct hdShadowTuning_c {
 be<f32> mMiddleLimit,mMiddleMinimum,mNormalHeightScale,mFarLimit,mFarExtension,mNearExtension,mDefaultNear,mMiddleMaximum,mNormalClampMinimum,mNormalDistanceScale;
 be<f32> mHighMiddleMinimum,mHighMiddleBase,mHighMiddleMaximum,mHeightOffset,mHighClampMaximum,mHighClampMinimum,mHighHeightScale,mHighNear;
 be<u8> mDrawExtra,mFirstMode,mOtherMode,mHighMode,mDisableSpecial; u8 _4D[3]; hdShadowStageRange_c mDefaultRange;
};
struct hdShadowProjectionVtable_l { u8 _00[0xC]; be<u32> mIsKindOf; };
struct hdShadowPerspective_l { u8 _00[0x90]; gptr<hdShadowProjectionVtable_l> mpVtable; be<f32> mNear,mFar; u8 _9C[0x10]; be<f32> mAspect; be<f32> mFrustum[4]; be<f32> mFieldOfView; };
struct hdShadowMatrix_c { be<f32> mValues[16]; };
struct hdShadowMatrices_c { hdShadowMatrix_c mMatrices[3]; };
/* The caller reserves 0x168 bytes between its volume and matrix array. The
 * volume initializer writes the perspective fields and its matrix at 0x128. */
struct hdShadowVolume_c {
 be<f32> mViewMatrix[12]; be<f32> mNear,mFar; u8 _38[0x30]; cXyz mMinimum,mMaximum;
 u8 _80[0x60]; be<f32> mFieldOfView,mAspect,mTangent,mInverseTangent;
 be<f32> mFrustum[2]; be<f32> mInverseViewMatrix[12]; be<f32> mProjectionMatrix[12]; u8 _158[0x10];
};
WWHD_OFFSET(hdShadowVolume_c,mViewMatrix,0);
WWHD_OFFSET(hdShadowVolume_c,mNear,0x30);
WWHD_OFFSET(hdShadowVolume_c,mMinimum,0x68);
WWHD_OFFSET(hdShadowVolume_c,mMaximum,0x74);
WWHD_OFFSET(hdShadowVolume_c,mFieldOfView,0xE0);
WWHD_OFFSET(hdShadowVolume_c,mFrustum,0xF0);
WWHD_OFFSET(hdShadowVolume_c,mInverseViewMatrix,0xF8);
WWHD_OFFSET(hdShadowVolume_c,mProjectionMatrix,0x128);
struct hdShadowDrawArgument_c { gptr<hdFilterRenderInfo_c> mpInfo; be<u32> mEnabled,mUnknown,mIndex; be<u32> mWords[9]; };
struct hdShadowDepthBias_c { be<u32> mWords[5]; };
struct hdShadowSave_l { u8 _00[0x1178]; u8 mEvents; };
struct hdShadowLineCheck_c {
 gptr<void> mpObjectVtable,mpGroupVtable; u8 _08[8]; be<u32> mLineVtable; u8 _14[0xC]; be<u32> mCollisionVtable; u8 _24[0xC]; cXyz mHit; u8 _3C[0x1C];
 be<u32> mObjectVtable; be<u8> mFlags[8]; be<u32> mGroupVtable,mGroupMask;
};
WWHD_SIZE(hdShadowLineCheck_c,0x6C);
WWHD_SIZE(hdShadowDrawArgument_c,0x34);
WWHD_SIZE(hdShadowVolume_c,0x168);
WWHD_OFFSET(hdShadowSurface_l,mpDistances,0x264);
WWHD_OFFSET(hdShadowSurface_l,mpViews,0x2C0);
WWHD_OFFSET(hdShadowRenderer_l,mLightDirection,0x5B0);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mMatrices,0x74);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mDistances,0x174);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mFadeParameters,0x1D4);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mRange,0x208);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mWidth,0x218);
WWHD_OFFSET(hdShadowCamera_l,mEye,0x258);
WWHD_OFFSET(hdShadowCamera_l,mMode,0x384);
WWHD_OFFSET(hdShadowCamera_l,mEyeOffset,0x7B4);
WWHD_OFFSET(hdShadowPlay_l,mBackgroundIndex,0x5B30);
WWHD_OFFSET(hdShadowPlay_l,mEventFlags,0x5CDC);
WWHD_OFFSET(hdShadowLineCheck_c,mHit,0x30);

struct hdShadowRangeState_c { be<f32> mNearRange,mFarRange,mScale; be<u32> mAdaptive,mDynamic,mSpecialCamera; };

struct hdShadowType_c { be<u32> mName; };

WWHD_OFFSET(hdShadowLight_l,mFade,0x1030);
WWHD_OFFSET(hdShadowLight_l,mIntensity,0x1038);
WWHD_OFFSET(hdShadowLight_l,mBlend,0x103C);
WWHD_OFFSET(hdShadowLight_l,mOcclusion,0x1044);
WWHD_OFFSET(hdShadowCamera_l,mTarget,0x264);
WWHD_OFFSET(hdShadowCamera_l,mHeight,0x280);
WWHD_OFFSET(hdShadowCamera_l,mTargetOffset,0x7C0);
WWHD_OFFSET(hdShadowCamera_l,mHeightOffset,0x7CC);
WWHD_OFFSET(hdShadowPlay_l,mBackground,0x12A0);
WWHD_OFFSET(hdShadowPlay_l,mpStageVtable,0x5150);
WWHD_OFFSET(hdShadowLineCheck_c,mFlags,0x5C);
WWHD_OFFSET(hdShadowLineCheck_c,mGroupVtable,0x64);
WWHD_OFFSET(hdShadowLineCheck_c,mGroupMask,0x68);
WWHD_OFFSET(hdShadowView_l,mMode,0x7C);
WWHD_OFFSET(hdShadowView_l,mShadowMatrix,0x2C4);
WWHD_OFFSET(hdShadowPerspective_l,mpVtable,0x90);
WWHD_OFFSET(hdShadowPerspective_l,mNear,0x94);
WWHD_OFFSET(hdShadowPerspective_l,mFar,0x98);
WWHD_OFFSET(hdShadowPerspective_l,mAspect,0xAC);
WWHD_OFFSET(hdShadowPerspective_l,mFrustum,0xB0);
WWHD_OFFSET(hdShadowPerspective_l,mFieldOfView,0xC0);
WWHD_OFFSET(hdShadowTuning_c,mHighMode,0x4B);
WWHD_OFFSET(hdShadowTuning_c,mDefaultRange,0x50);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mSceneMatrix,0x184);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mOcclusion,0x1C4);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mIntensity,0x1C8);
WWHD_OFFSET(hdShadowEnvironmentParameters_l,mFade,0x1CC);
WWHD_SIZE(hdShadowDepthBias_c,0x14);
