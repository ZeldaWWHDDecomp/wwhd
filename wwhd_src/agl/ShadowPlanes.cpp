#include "agl/ShadowContext.h"
void aglShadowPlanesInvertProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*);
void aglShadowPlanesInvertAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*);
void aglShadowPlanesMultiplyVector(const sead::RenderMatrix34*, const sead::RenderVector3*, sead::RenderVector3*);
namespace agl { namespace shadow {
static f32 projectCoordinate(const be<f32> (&row)[4], f32 x, f32 y, f32 z, f32 w) {
    f32 a = gabi::fdivs_ppc(row[0], w), b = gabi::fdivs_ppc(row[1], w);
    f32 c = gabi::fdivs_ppc(row[2], w), d = gabi::fdivs_ppc(row[3], w);
    f32 productY = gabi::fmuls_ppc(b, y);
    f32 sumXY = gabi::fmadds(a, x, productY);
    f32 sumXYZ = gabi::fmadds(c, z, sumXY);
    return gabi::fadds_ppc(sumXYZ, d);
}
static f32 planeDot(const sead::RenderVector3* first, const sead::RenderVector3* second) {
    f32 x = gabi::fmuls_ppc(first->x, second->x);
    f32 y = gabi::fmuls_ppc(first->y, second->y);
    return gabi::fmadds(first->z, second->z, gabi::fadds_ppc(x, y));
}
/* 027C8ABC */
void buildPlanes(ShadowContext* context, const sead::RenderMatrix34* affine,
        const sead::RenderMatrix44* projection, const sead::RenderVector3* reference,
        u32 addEdgePlanes, f64 firstDepth, f64 secondDepth) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderMatrix44> inverseProjection(0x44);
    gabi::FrameLocal<sead::RenderMatrix34> inverseAffine(0x12C);
    aglShadowPlanesInvertProjection(inverseProjection, projection);
    aglShadowPlanesInvertAffine(affine, inverseAffine);
    gabi::FrameLocal<PlaneCornerSet> corners(0xC0);
    /* The eight inline vector constructors have no allocation branch for a valid frame. */
    for (u32 index = 0; index < 8; ++index) {
        sead::RenderVector3* point = &corners->mPoints[index];
        point->y = index >= 4 ? 1.0f : -1.0f;
        point->x = (index == 1 || index == 2 || index == 5 || index == 6) ? 1.0f : -1.0f;
        point->z = gabi::f32_from_bits(ppc_stfs_bits(
            (index == 2 || index == 3 || index == 6 || index == 7) ? firstDepth : secondDepth));
    }
    for (u32 index = 0; index < 8; ++index) {
        sead::RenderVector3* point = &corners->mPoints[index];
        f32 x = point->x, y = point->y, z = point->z;
        const auto& row = inverseProjection->mRows[3];
        f32 productY = gabi::fmuls_ppc(row[1], y);
        f32 sumXY = gabi::fmadds(row[0], x, productY);
        f32 sumXYZ = gabi::fmadds(row[2], z, sumXY);
        f32 w = gabi::fadds_ppc(sumXYZ, row[3]);
        f32 projectedY = projectCoordinate(inverseProjection->mRows[1], x, y, z, w);
        f32 projectedX = projectCoordinate(inverseProjection->mRows[0], x, y, z, w);
        f32 projectedZ = projectCoordinate(inverseProjection->mRows[2], x, y, z, w);
        point->y = projectedY;
        point->x = projectedX;
        point->z = projectedZ;
        aglShadowPlanesMultiplyVector(inverseAffine, point, point);
    }
    gabi::FrameLocal<sead::RenderVector3> center(0x84);
    center->x = gabi::fmuls_ppc(gabi::fadds_ppc(corners->mPoints[0].x, corners->mPoints[6].x), 0.5f);
    center->y = gabi::fmuls_ppc(gabi::fadds_ppc(corners->mPoints[0].y, corners->mPoints[6].y), 0.5f);
    center->z = gabi::fmuls_ppc(gabi::fadds_ppc(corners->mPoints[0].z, corners->mPoints[6].z), 0.5f);
    constexpr f32 tolerance = 1.1920928955078125e-6f;
    constexpr f32 referenceThreshold = -0.001f;
    auto* shadow = reinterpret_cast<PlaneSetPrefix_l*>(context);
    for (u32 face = 0; face < 6; ++face) {
        const FaceCornerIndices_l* indices = faceCornerIndices(face);
        u32 third = indices->mIndices[2], first = indices->mIndices[0], second = indices->mIndices[1];
        const sead::RenderVector3* origin = &corners->mPoints[first];
        const sead::RenderVector3* point = &corners->mPoints[third];
        gabi::FrameLocal<sead::RenderVector3> edgeA(0x90), edgeB(0x9C), normal(8);
        edgeA->x = gabi::fsubs_ppc(point->x, origin->x);
        edgeA->y = gabi::fsubs_ppc(point->y, origin->y);
        edgeA->z = gabi::fsubs_ppc(point->z, origin->z);
        aglShadowNormalizeVector(edgeA);
        point = &corners->mPoints[second];
        edgeB->x = gabi::fsubs_ppc(point->x, origin->x);
        edgeB->y = gabi::fsubs_ppc(point->y, origin->y);
        edgeB->z = gabi::fsubs_ppc(point->z, origin->z);
        aglShadowNormalizeVector(edgeB);
        aglShadowCrossProduct(edgeA, edgeB, normal);
        f32 length = aglShadowNormalizeVector(normal);
        f32 x = normal->x, y = normal->y, z = normal->z;
        f32 squareY = gabi::fmuls_ppc(y, y);
        f32 squareXY = gabi::fmadds(x, x, squareY);
        f32 squareLength = gabi::fmadds(z, z, squareXY);
        f32 difference = gabi::fsubs_ppc(squareLength, 1.0f);
        if (length != length || !(length > 0.0f) || difference < -tolerance || difference > tolerance)
            continue;
        u32 count = static_cast<u32>(static_cast<s32>(context->mPlaneCount));
        geometry::Plane* plane = &context->mPlanes[count < 24 ? count : 0];
        f32 distance = planeDot(normal, origin);
        plane->mNormal.x = x;
        plane->mNormal.y = normal->y;
        plane->mNormal.z = normal->z;
        plane->mDistance = distance;
        f32 centerDistance = planeDot(&plane->mNormal, center);
        if (gabi::fsubs_ppc(centerDistance, distance) > 0.0f) {
            plane->mNormal.x = -static_cast<f32>(normal->x);
            plane->mNormal.y = -static_cast<f32>(normal->y);
            plane->mDistance = -distance;
            plane->mNormal.z = -static_cast<f32>(normal->z);
        }
        if (!reference || planeDot(reference, &plane->mNormal) > referenceThreshold)
            appendOrMergePlane(shadow, plane);
        if (addEdgePlanes && reference)
            appendEdgePlane(shadow, reference, corners, face, &plane->mNormal);
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
} }
/* WWHD glue
ext 0274EDD4 void aglShadowPlanesInvertProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*)
ext 028E91EC void aglShadowPlanesInvertAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*)
ext 028E8F64 void aglShadowPlanesMultiplyVector(const sead::RenderMatrix34*, const sead::RenderVector3*, sead::RenderVector3*)
*/
