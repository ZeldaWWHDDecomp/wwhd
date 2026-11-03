// Joint-owned material chains and the two bounded temporary packet groups.
#pragma once
#include "JSystem/J3DGraph/J3DModelAdapter.h"
#include "JSystem/J3DGraph/J3DModel_entry.h"
#include "JSystem/J3DGraph/J3DShapePacketResources.h"
#include "JSystem/J3DGraph/J3DMaterialResources.h"
namespace j3d {
struct ModelMaterialLink_l {
    gptr<MaterialParameterHeader> mResource;
    gptr<ModelMaterialLink_l> mNext;
    gptr<ShapeResource_l> mShape;
};
struct ModelMaterialPointers_l { gptr<ModelMaterialLink_l> mPointers[128]; };
}
WWHD_OFFSET(j3d::JointNode_l,mMaterials,0x10);
WWHD_OFFSET(j3d::ModelState_l,mMaterialState.mShapeCount,0x28);
WWHD_SIZE(j3d::ModelMaterialPointers_l,512);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::ModelMaterialLink_l,mNext,4);
WWHD_OFFSET(j3d::ModelMaterialLink_l,mShape,8);
