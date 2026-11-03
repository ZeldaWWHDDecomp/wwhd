/* Read-prefix view for AGL#17's shadow clipping tests. The full shadow unit is
 * 0x5B4 bytes (own constructor027C70A8); these methods only read through0x59F.
 * Unobserved parameter/transform state is not claimed as a reconstructed layout. */
#pragma once
#include "agl/GeometryMath.h"
#include "sead/HeapRegistry.h"
namespace agl { namespace shadow {
/* Borrowed workspace prefix from the public GeometryProjection.h,
 * checked against original constructor 027D7E04; current shared owner AGL#19.
 * This local view does not define a second owner or initializer. */
struct GeometryPointList_l;
struct GeometryGroupSet_l {
    be<u32> mCapacity;
    gptr<gptr<GeometryPointList_l>> mEntries;
    be<s32> mCount;
};
struct GeometryWorkspace_l {
    GeometryGroupSet_l mInput;
    GeometryGroupSet_l mSets[2];
    be<u32> mActiveSet;
    sead::RenderVector3 mCorners[8];
    be<f32> mDiameter, mTolerance;
};
WWHD_SIZE(GeometryWorkspace_l, 0x90);
WWHD_SIZE(GeometryGroupSet_l, 12);
WWHD_OFFSET(GeometryGroupSet_l, mEntries, 4);
WWHD_OFFSET(GeometryGroupSet_l, mCount, 8);
WWHD_OFFSET(GeometryWorkspace_l, mSets, 0xC);
WWHD_OFFSET(GeometryWorkspace_l, mActiveSet, 0x24);
WWHD_OFFSET(GeometryWorkspace_l, mCorners, 0x28);
WWHD_OFFSET(GeometryWorkspace_l, mDiameter, 0x88);
WWHD_OFFSET(GeometryWorkspace_l, mTolerance, 0x8C);
struct WorkspaceOwnerPrefix_l {
    u8 mUnobservedState[0x5C];
    be<u32> mWorkspaceMethods;
    u8 mUnobservedState60[0x24];
    GeometryWorkspace_l mWorkspace;
};
WWHD_OFFSET(WorkspaceOwnerPrefix_l, mWorkspace, 0x84);
WWHD_OFFSET(WorkspaceOwnerPrefix_l, mWorkspaceMethods, 0x5C);
WWHD_SIZE(WorkspaceOwnerPrefix_l, 0x114);
u32 workspaceMethodsAddress();
void initializeWorkspace(WorkspaceOwnerPrefix_l* shadow, sead::RegistryHeap* heap);
void destroyWorkspace(WorkspaceOwnerPrefix_l* shadow, u32 flags);
struct PlaneSetPrefix_l {
    u8 mUnobservedState[0x41C];
    be<s32> mPlaneCount;
    geometry::Plane mPlanes[24];
};
WWHD_OFFSET(PlaneSetPrefix_l, mPlaneCount, 0x41C);
WWHD_OFFSET(PlaneSetPrefix_l, mPlanes, 0x420);
WWHD_SIZE(PlaneSetPrefix_l, 0x5A0);
struct FaceCornerIndices_l { be<u32> mIndices[4]; };
WWHD_SIZE(FaceCornerIndices_l, 16);
const FaceCornerIndices_l* faceCornerIndices(u32 faceIndex);
struct PlaneCornerSet { sead::RenderVector3 mPoints[8]; };
WWHD_SIZE(PlaneCornerSet, 96);
void appendEdgePlane(PlaneSetPrefix_l* shadow, const sead::RenderVector3* edge,
                     const PlaneCornerSet* corners, u32 faceIndex,
                     const sead::RenderVector3* reference);
u32 appendOrMergePlane(PlaneSetPrefix_l* shadow, const geometry::Plane* plane);
u32 intersectsBounds(const PlaneSetPrefix_l* shadow, const geometry::Bounds3* bounds);
u32 intersectsSphere(const PlaneSetPrefix_l* shadow, const geometry::Sphere* sphere);
} }
void aglShadowCrossProduct(const sead::RenderVector3*, const sead::RenderVector3*, sead::RenderVector3*);
f32 aglShadowNormalizeVector(sead::RenderVector3*);

void aglShadowInitializeWorkspace(agl::shadow::GeometryWorkspace_l*, sead::RegistryHeap*);

void aglShadowDestroyGroup(void*, s32);
void aglShadowStorageDelete(void*);
