// J3D HD renderer: scene uniform layout.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderUniformPair.h"
namespace j3d {
struct RenderSceneUniforms : RenderUniformPair {
    Mtx44 mMatrices[4];
    RenderUniformRow mViewParameters;
    Mtx44 mProjectionMatrix;
    be<f32> mScalarParameters[4];
    RenderUniformRow mEnvironmentVectors[3];
    be<f32> mRange;
    RenderUniformRow mParameters[6];
    RenderUniformRow mExtraValues[10];
    static void destroy(RenderSceneUniforms* storage, u32 flags);
    void configureSceneFields(agl::RenderUniformBuffer_l* buffer, void* heap);
    void resetSceneValues();
    void uploadSceneValues(agl::RenderUniformBuffer_l* buffer);
};
}
WWHD_OFFSET(j3d::RenderSceneUniforms, mMatrices, 0x74);
WWHD_OFFSET(j3d::RenderSceneUniforms, mViewParameters, 0x174);
WWHD_OFFSET(j3d::RenderSceneUniforms, mProjectionMatrix, 0x184);
WWHD_OFFSET(j3d::RenderSceneUniforms, mScalarParameters, 0x1C4);
WWHD_OFFSET(j3d::RenderSceneUniforms, mEnvironmentVectors, 0x1D4);
WWHD_OFFSET(j3d::RenderSceneUniforms, mRange, 0x204);
WWHD_OFFSET(j3d::RenderSceneUniforms, mParameters, 0x208);
WWHD_OFFSET(j3d::RenderSceneUniforms, mExtraValues, 0x268);
WWHD_SIZE(j3d::RenderSceneUniforms, 0x308);
