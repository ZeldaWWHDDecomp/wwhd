// J3D HD renderer: a group of frame, matrix and material uniform pairs prepared and bound together.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderFrameArray.h"

/* 027FE084 */
void j3d::RenderUniformGroup_l::prepare(s32 count, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderFrameArray::prepare(count, heap);
    mMatrixUniforms.RenderUniformPair::prepare(heap);
    mMaterialUniforms.RenderUniformPair::prepare(heap);
    mGroupReady = 1;
}

/* 027FE0DC */
void j3d::RenderUniformGroup_l::advance(u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderFrameArray::advance(index);
    mMatrixUniforms.RenderUniformPair::advance();
    mMaterialUniforms.RenderUniformPair::advance();
}

/* 027FE118 */
void j3d::RenderUniformGroup_l::bind(const RenderUniformLocations_l* locations, u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderFrameUniforms* selected = frames();
    if (index < (u32)mCount) selected += index;
    bindPreviousUniformBlock(selected, locations, RENDER_UNIFORM_SLOT_FRAME);
    bindPreviousUniformBlock(&mMatrixUniforms, locations, RENDER_UNIFORM_SLOT_MATRIX);
    bindPreviousUniformBlock(&mMaterialUniforms, locations, RENDER_UNIFORM_SLOT_MATERIAL);
}

/* WWHD glue
*/
