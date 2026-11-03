// J3D HD renderer: deleting destructors of the uniform pair kinds.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderMaterialUniforms.h"

/* 027FF838 */
void j3d::RenderFrameUniforms::destroy(RenderFrameUniforms* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL) {
        RenderUniformPair::destroy(storage, 0);
        if (flags & 1) releaseRenderUniformObject(storage);
    }
}

/* 027FF96C */
void j3d::RenderMatrixUniforms::destroy(RenderMatrixUniforms* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL) {
        RenderUniformPair::destroy(storage, 0);
        if (flags & 1) releaseRenderUniformObject(storage);
    }
}

/* 027FFAA0 */
void j3d::RenderMaterialUniforms::destroy(RenderMaterialUniforms* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL) {
        RenderUniformPair::destroy(storage, 0);
        if (flags & 1) releaseRenderUniformObject(storage);
    }
}

/* 027FFAF4 */
void j3d::RenderMutableMaterialUniforms::destroy(RenderMutableMaterialUniforms* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL) {
        RenderUniformPair::destroy(storage, 0);
        if (flags & 1) releaseRenderUniformObject(storage);
    }
}

/* WWHD glue
*/
