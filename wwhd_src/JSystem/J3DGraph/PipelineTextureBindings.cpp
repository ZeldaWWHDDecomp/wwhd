// Indexed texture-matrix bindings from this unit's own load/store leaves.
#include "JSystem/J3DGraph/ModelDataPipeline.h"
using namespace j3d::pipeline;
namespace j3d::pipeline {
/* 027F7614 */
void MaterialIndexedProvider_l::setTextureMatrix(u32 index,MaterialTextureMatrix_l* matrix) {
    auto* slot=gabi::at<gptr<MaterialTextureMatrix_l>>(gabi::ea(this)+offsetof(MaterialIndexedProvider_l,mMatrices)+index*sizeof(gptr<MaterialTextureMatrix_l>));
    *slot=matrix;
}
/* 027F7624 */
MaterialTextureMatrix_l* MaterialIndexedProvider_l::textureMatrix(u32 index) const {
    const auto* slot=gabi::at<gptr<MaterialTextureMatrix_l>>(gabi::ea(this)+offsetof(MaterialIndexedProvider_l,mMatrices)+index*sizeof(gptr<MaterialTextureMatrix_l>));
    return slot->get();
}
}

/* WWHD glue
*/
