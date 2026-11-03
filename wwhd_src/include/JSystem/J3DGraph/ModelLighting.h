#pragma once
#include "JSystem/J3DGraph/ModelPipeline.h"
namespace j3d::pipeline {
struct PipelineLight_l {
    sead::ViewVector mPosition;
    u8 mState[12]; ByteColor mColor; SignedColor mSignedColor;
    be<f32> mColorScale,mSignedScale;
    u8 mUnobservedState[0x58];
};
struct PipelineLightMethods_l { u8 mPrefix[0x44]; gfn mLight; };
struct PipelineLightProvider_l {
    gptr<PipelineLightMethods_l> mMethods; u8 mState[0x10];
    gptr<PipelineLight_l> mLights[8]; u8 mTail[12];
    void setLight(u32,PipelineLight_l*);
    PipelineLight_l* light(u32) const;
};
struct PipelineFog_l { be<u8> mKind,mAdjustmentEnabled; be<u16> mCenter; Scalar mNear,mFar,mNearPlane,mFarPlane; ByteColor mColor; };
struct PipelineFogMethods_l { u8 mPrefix[0xC]; gfn mSet; u8 mSetPadding[4]; gfn mFog; u8 mFogPadding[0x14]; gfn mFlags; u8 mFlagsPadding[0x14]; gfn mOther; };
struct PipelineFogProvider_l {
    gptr<PipelineFogMethods_l> mMethods; gptr<PipelineFog_l> mFog;
    be<u8> mFlags[4]; be<u8> mOtherFlag; u8 mPadding[3];
    void setFog(PipelineFog_l*);
    PipelineFog_l* fog() const;
    be<u8>* flagStorage();
    be<u8>* otherFlagStorage();
};
struct LightingPage_l {
    u8 mPrefix[0xE4];
    sead::ViewVector mPositions[8];
    sead::ColorChannels mLightColors[8];
    sead::ColorChannels mSignedColors[2];
    sead::ColorChannels mFogColor;
    Scalar mFogNear; be<f32> mFogScale;
    u8 mTail[0x40];
};
struct LightingPageArray_l { be<u32> mCount; gptr<LightingPage_l> mPages; };
}
WWHD_OFFSET(j3d::pipeline::PipelineLight_l,mColor,0x18);
WWHD_OFFSET(j3d::pipeline::PipelineLight_l,mSignedColor,0x1C);
WWHD_OFFSET(j3d::pipeline::PipelineLight_l,mColorScale,0x24);
WWHD_OFFSET(j3d::pipeline::PipelineLight_l,mSignedScale,0x28);
// Complete HD light storage is84 bytes in canonical f_op_actor.h; only the proven prefix is refined.
WWHD_SIZE(j3d::pipeline::PipelineLight_l,0x84);
WWHD_OFFSET(j3d::pipeline::PipelineFog_l,mColor,0x14);
WWHD_SIZE(j3d::pipeline::PipelineFog_l,0x18);
WWHD_OFFSET(j3d::pipeline::LightingPage_l,mPositions,0xE4);
WWHD_OFFSET(j3d::pipeline::LightingPage_l,mLightColors,0x144);
WWHD_OFFSET(j3d::pipeline::LightingPage_l,mSignedColors,0x1C4);
WWHD_OFFSET(j3d::pipeline::LightingPage_l,mFogColor,0x1E4);
WWHD_OFFSET(j3d::pipeline::LightingPage_l,mFogNear,0x1F4);
WWHD_OFFSET(j3d::pipeline::LightingPage_l,mFogScale,0x1F8);
WWHD_SIZE(j3d::pipeline::LightingPage_l,0x23C);
WWHD_SIZE(j3d::pipeline::LightingPageArray_l,8);
WWHD_OFFSET(j3d::pipeline::PipelineLightProvider_l,mLights,0x14);
WWHD_SIZE(j3d::pipeline::PipelineLightProvider_l,0x40);
WWHD_OFFSET(j3d::pipeline::PipelineFogProvider_l,mFog,4);
WWHD_OFFSET(j3d::pipeline::PipelineFogProvider_l,mFlags,8);
WWHD_OFFSET(j3d::pipeline::PipelineFogProvider_l,mOtherFlag,0xC);
WWHD_SIZE(j3d::pipeline::PipelineFogProvider_l,0x10);
