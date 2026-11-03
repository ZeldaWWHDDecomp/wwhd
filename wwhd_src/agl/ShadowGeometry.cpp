#include "agl/ShadowGeometry.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
namespace agl { namespace shadow {
/* Borrowed configuration byte; nonzero enables the normal-based merge branch. */
static u8 sMergeParallelPlanes = 1;
static u8 sValidateAndAppendPlane = 1;
struct FaceCornerTable { FaceCornerIndices_l mFaces[6]; };
WWHD_SIZE(FaceCornerTable, 96);
static const FaceCornerTable sFaceCornerIndices;
static const sead::VtableSlot sWorkspaceMethodTable;
u32 workspaceMethodsAddress() { return gabi::ea(&sWorkspaceMethodTable); }
const FaceCornerIndices_l* faceCornerIndices(u32 faceIndex) {
    return &sFaceCornerIndices.mFaces[faceIndex];
}
/* 027C7504 */
void destroyWorkspace(WorkspaceOwnerPrefix_l* shadow, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!shadow) return;
    shadow->mWorkspaceMethods = workspaceMethodsAddress();
    __destroy_arr(shadow->mWorkspace.mSets, 2, sizeof(GeometryGroupSet_l),
                  aglShadowDestroyGroup, 0, 0);
    aglShadowDestroyGroup(&shadow->mWorkspace.mInput, 2);
    if (flags & 1) aglShadowStorageDelete(shadow);
}
/* 027C7590 */
void initializeWorkspace(WorkspaceOwnerPrefix_l* shadow, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    aglShadowInitializeWorkspace(&shadow->mWorkspace, heap);
}
/* 027C8698 */
u32 appendOrMergePlane(PlaneSetPrefix_l* shadow, const geometry::Plane* plane) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = shadow->mPlaneCount;
    if (count > 0) {
        u8 merge = sMergeParallelPlanes;
        constexpr f32 kParallelNormalTolerance = 1.1920928955078125e-6f;
        for (s32 index = 0; index < count; ++index) {
            if (!merge) continue;
            geometry::Plane* existing = &shadow->mPlanes[index < 24 ? index : 0];
            f32 productX = gabi::fmuls_ppc(plane->mNormal.x, existing->mNormal.x);
            f32 productY = gabi::fmuls_ppc(plane->mNormal.y, existing->mNormal.y);
            f32 sumXY = gabi::fadds_ppc(productX, productY);
            f32 dot = gabi::fmadds(plane->mNormal.z, existing->mNormal.z, sumXY);
            f32 difference = gabi::fsubs_ppc(dot, 1.0f);
            if (difference < -kParallelNormalTolerance || difference > kParallelNormalTolerance)
                continue;
            f32 oldDistance = existing->mDistance;
            f32 newDistance = plane->mDistance;
            f32 distanceDifference = gabi::fsubs_ppc(oldDistance, newDistance);
            existing->mDistance = distanceDifference >= 0.0f ? oldDistance : newDistance;
            return 0;
        }
    }
    u32 destinationIndex = static_cast<u32>(count) < 24 ? static_cast<u32>(count) : 0;
    geometry::Plane* destination = &shadow->mPlanes[destinationIndex];
    destination->mNormal.x = plane->mNormal.x;
    destination->mNormal.y = plane->mNormal.y;
    destination->mNormal.z = plane->mNormal.z;
    destination->mDistance = plane->mDistance;
    shadow->mPlaneCount = static_cast<s32>(static_cast<u32>(shadow->mPlaneCount) + 1);
    return 1;
}
static f32 planePointDot(f32 normalX, f32 normalY, f32 normalZ,
                         const sead::RenderVector3* point) {
    f32 productX = gabi::fmuls_ppc(normalX, point->x);
    f32 productY = gabi::fmuls_ppc(normalY, point->y);
    f32 sumXY = gabi::fadds_ppc(productX, productY);
    return gabi::fmadds(normalZ, point->z, sumXY);
}
/* 027C87DC */
void appendEdgePlane(PlaneSetPrefix_l* shadow, const sead::RenderVector3* edge,
                     const PlaneCornerSet* corners, u32 faceIndex,
                     const sead::RenderVector3* reference) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> perpendicular(0x14);
    aglShadowCrossProduct(edge, reference, perpendicular);
    aglShadowNormalizeVector(perpendicular);
    gabi::FrameLocal<sead::RenderVector3> normal(8);
    aglShadowCrossProduct(perpendicular, edge, normal);
    f32 length = aglShadowNormalizeVector(normal);
    f32 normalY = normal->y;
    f32 normalX = normal->x;
    f32 squareY = gabi::fmuls_ppc(normalY, normalY);
    f32 squareXY = gabi::fmadds(normalX, normalX, squareY);
    f32 normalZ = normal->z;
    f32 squareLength = gabi::fmadds(normalZ, normalZ, squareXY);
    f32 difference = gabi::fsubs_ppc(squareLength, 1.0f);
    constexpr f32 kUnitNormalTolerance = 1.1920928955078125e-6f;
    bool unitNormal = !(difference < -kUnitNormalTolerance)
        && !(difference > kUnitNormalTolerance);
    if (length != length || !(length > 0.0f) || !unitNormal) return;
    u32 count = shadow->mPlaneCount;
    geometry::Plane* plane = &shadow->mPlanes[count < 24 ? count : 0];
    u32 indices[4];
    for (u32 index = 0; index < 4; ++index)
        indices[index] = sFaceCornerIndices.mFaces[faceIndex].mIndices[index];
    const sead::RenderVector3* point = &corners->mPoints[indices[0]];
    f32 distance = planePointDot(normal->x, normal->y, normalZ, point);
    plane->mNormal.x = normalX;
    plane->mNormal.y = normal->y;
    normalX = plane->mNormal.x;
    normalY = plane->mNormal.y;
    plane->mDistance = distance;
    normalZ = normal->z;
    plane->mNormal.z = normalZ;
    for (u32 index = 1; index < 4; ++index) {
        point = &corners->mPoints[indices[index]];
        f32 candidate = planePointDot(normalX, normalY, normalZ, point);
        f32 delta = gabi::fsubs_ppc(candidate, distance);
        if (delta > 0.0f) {
            f32 x = normal->x;
            f32 y = normal->y;
            f32 z = normal->z;
            distance = planePointDot(x, y, z, point);
            plane->mNormal.x = x;
            plane->mNormal.y = normal->y;
            normalZ = normal->z;
            plane->mNormal.z = normalZ;
            normalX = plane->mNormal.x;
            normalY = plane->mNormal.y;
            plane->mDistance = distance;
        }
    }
    if (!sValidateAndAppendPlane) return;
    normalX = plane->mNormal.x;
    normalY = plane->mNormal.y;
    normalZ = plane->mNormal.z;
    for (u32 index = 0; index < 8; ++index) {
        f32 dot = planePointDot(normalX, normalY, normalZ, &corners->mPoints[index]);
        if (gabi::fsubs_ppc(dot, distance) > 0.0f) return;
    }
    appendOrMergePlane(shadow, plane);
}
/* 027C988C */
u32 intersectsBounds(const PlaneSetPrefix_l* shadow, const geometry::Bounds3* bounds) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = shadow->mPlaneCount;
    for (s32 index = 0; index < count; ++index) {
        const geometry::Plane* plane = &shadow->mPlanes[index < 24 ? index : 0];
        f32 normalX = plane->mNormal.x;
        f32 normalY = plane->mNormal.y;
        f32 normalZ = plane->mNormal.z;
        f32 x = normalX > 0.0f ? bounds->mMinimum.x : bounds->mMaximum.x;
        f32 y = normalY > 0.0f ? bounds->mMinimum.y : bounds->mMaximum.y;
        f32 z = normalZ > 0.0f ? bounds->mMinimum.z : bounds->mMaximum.z;
        f32 productX = gabi::fmuls_ppc(normalX, x);
        f32 productY = gabi::fmuls_ppc(normalY, y);
        f32 sum = gabi::fadds_ppc(productX, productY);
        f32 dot = gabi::fmadds(normalZ, z, sum);
        f32 distance = gabi::fsubs_ppc(dot, plane->mDistance);
        if (distance > 0.0f) return 0;
    }
    return 1;
}
/* 027C99BC */
u32 intersectsSphere(const PlaneSetPrefix_l* shadow, const geometry::Sphere* sphere) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 radius = sphere->mRadius;
    if (!(radius > 0.0f)) return 0;
    s32 count = shadow->mPlaneCount;
    if (count <= 0) return 1;
    f32 x = sphere->mCenter.x;
    f32 y = sphere->mCenter.y;
    f32 z = sphere->mCenter.z;
    for (s32 index = 0; index < count; ++index) {
        const geometry::Plane* plane = &shadow->mPlanes[index < 24 ? index : 0];
        f32 productX = gabi::fmuls_ppc(plane->mNormal.x, x);
        f32 productY = gabi::fmuls_ppc(plane->mNormal.y, y);
        f32 sum = gabi::fadds_ppc(productX, productY);
        f32 dot = gabi::fmadds(plane->mNormal.z, z, sum);
        f32 distance = gabi::fsubs_ppc(dot, plane->mDistance);
        if (radius < distance) return 0;
    }
    return 1;
}
} }
/* WWHD glue
data sWorkspaceMethodTable 1016A6BC
ext 027DA4C8 void aglShadowDestroyGroup(void*, s32) callback
ext 0273AF40 void aglShadowStorageDelete(void*)
ext 027D7D40 void aglShadowInitializeWorkspace(agl::shadow::GeometryWorkspace_l*, sead::RegistryHeap*)
data sMergeParallelPlanes 101F9603
data sValidateAndAppendPlane 101F9601
data sFaceCornerIndices 1016A604
ext 028E8D4C void aglShadowCrossProduct(const sead::RenderVector3*, const sead::RenderVector3*, sead::RenderVector3*)
ext 025155D8 f32 aglShadowNormalizeVector(sead::RenderVector3*)
*/
