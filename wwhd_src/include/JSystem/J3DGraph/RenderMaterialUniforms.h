// J3D HD renderer: material uniform layouts.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderUniformPair.h"
namespace j3d {
struct RenderTransformPair {
    RenderUniformRow mLinear;
    RenderUniformRow mOffset;
};
struct RenderMaterialUniforms : RenderUniformPair {
    RenderWideWords mMaterialValues[11];
    RenderUniformRow mTextureRows[3][2];
    Mtx34 mTextureMatrices[8];
    RenderTransformPair mTransforms[3];
    static void destroy(RenderMaterialUniforms* storage, u32 flags);
    void setTransform(u32 index, const RenderUniformRow* linear, const RenderUniformRow* offset);
    void configureMaterialFields(agl::RenderUniformBuffer_l* buffer, void* heap);
    void resetMaterialValues();
    void uploadMaterialValues(agl::RenderUniformBuffer_l* buffer);
};
struct RenderMutableMaterialUniforms : RenderMaterialUniforms {
    u8 mUpdateState[4];
    be<u64> mDirtyMask; // bit per uniform field (fields 0..27)
    static void destroy(RenderMutableMaterialUniforms* storage, u32 flags);
    void uploadDirtyMaterialValues(agl::RenderUniformBuffer_l* buffer);
    void setTextureMatrix(const Mtx34* matrix, u32 index);
    void setTransformDirty(u32 index, const RenderUniformRow* linear, const RenderUniformRow* offset);
    void setPairValue(const RenderWideWords* value, u32 index);
    void setTripletValue(const RenderWideWords* value, u32 index);
    void setQuartetValue(const RenderWideWords* value, u32 index);
};
}
WWHD_SIZE(j3d::RenderTransformPair, 32);
WWHD_OFFSET(j3d::RenderMaterialUniforms, mMaterialValues, 0x74);
WWHD_OFFSET(j3d::RenderMaterialUniforms, mTextureRows, 0x124);
WWHD_OFFSET(j3d::RenderMaterialUniforms, mTextureMatrices, 0x184);
WWHD_OFFSET(j3d::RenderMaterialUniforms, mTransforms, 0x304);
WWHD_SIZE(j3d::RenderMaterialUniforms, 0x364);
WWHD_OFFSET(j3d::RenderMutableMaterialUniforms, mDirtyMask, 0x368);
WWHD_SIZE(j3d::RenderMutableMaterialUniforms, 0x370);

void copyRenderMatrix34(const Mtx34* source, Mtx34* destination);
