#pragma once
#include "JSystem/J3DGraph/PipelineConversions.h"
namespace j3d::pipeline {
struct PipelineTexturePivot_l { Scalar mS,mT; gptr<MatrixRows> mEffect; };
struct PipelineTextureTransform_l {
    be<s32> mMode;
    Scalar mScaleS,mScaleT,mRotation,mTranslateS,mTranslateT;
    gptr<PipelineTexturePivot_l> mPivot;
    static u32 build(MatrixRows* destination,const PipelineTextureTransform_l* parameters);
};
}
WWHD_SIZE(j3d::pipeline::PipelineTexturePivot_l,12);
WWHD_OFFSET(j3d::pipeline::PipelineTextureTransform_l,mRotation,0xC);
WWHD_OFFSET(j3d::pipeline::PipelineTextureTransform_l,mPivot,0x18);
WWHD_SIZE(j3d::pipeline::PipelineTextureTransform_l,0x1C);
