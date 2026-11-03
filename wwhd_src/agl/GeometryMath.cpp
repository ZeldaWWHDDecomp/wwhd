#include "agl/GeometryMath.h"
#include "agl/UtilityText.h"
#include <limits>
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
namespace agl { namespace geometry {
/* Borrowed SEAD#14 PoolHeap.cpp composite single defaults, elements11..13. */
struct PlaneNormalBits_l { be<u32> mX, mY, mZ; };
WWHD_SIZE(PlaneNormalBits_l, 12);
WWHD_OFFSET(PlaneNormalBits_l, mZ, 8);
static const PlaneNormalBits_l sDefaultPlaneNormal;
/* Borrowed from the same SEAD#14 defaults, elements20..22; startup writes (1,1,1). */
static const sead::RenderVector3 sSelectorHalfExtent;
static sead::RenderMatrix44 sVerticalDepthExchange;
static sead::RenderMatrix44 sClipTextureTransform;
/* 027C81E8 */
f32 quantizeDown(f64 value, f64 subdivisions) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 scaled = gabi::fmuls_fpr(value, subdivisions);
    u32 integerBits = static_cast<u32>(gabi::ftoi(gabi::fpr_from_f32(scaled)));
    f32 truncated = static_cast<f32>(static_cast<s32>(integerBits));
    if (truncated != scaled && scaled < 0.0f) --integerBits;
    f32 snapped = static_cast<f32>(static_cast<s32>(integerBits));
    return gabi::fdivs_fpr(gabi::fpr_from_f32(snapped), subdivisions);
}
/* 027C9A50 */
void sphereBounds(void*, Bounds3* bounds, const Sphere* sphere) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 radius = sphere->mRadius;
    f32 x = sphere->mCenter.x;
    f32 y = sphere->mCenter.y;
    f32 minimumX = gabi::fsubs_ppc(x, radius);
    f32 minimumY = gabi::fsubs_ppc(y, radius);
    f32 z = sphere->mCenter.z;
    f32 maximumX = gabi::fadds_ppc(x, radius);
    f32 maximumY = gabi::fadds_ppc(y, radius);
    f32 minimumZ = gabi::fsubs_ppc(z, radius);
    z = sphere->mCenter.z;
    f32 maximumZ = gabi::fadds_ppc(z, radius);
    bounds->mMinimum.y = minimumY;
    bounds->mMaximum.y = maximumY;
    bounds->mMaximum.x = maximumX;
    bounds->mMinimum.z = minimumZ;
    bounds->mMinimum.x = minimumX;
    bounds->mMaximum.z = maximumZ;
}
/* The indexed corners stay on the original frame, before destination writes. */
struct BoundsCorners { sead::RenderVector3 mPoints[8]; };
WWHD_SIZE(BoundsCorners, 96);
/* 027C90C0 */
void initializeBoundsCornerSelectors(BoundsTransformState_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    context->mMinimumCorner[1] = -1;
    context->mMaximumCorner[2] = -1;
    context->mMaximumCorner[1] = -1;
    context->mMinimumCorner[0] = -1;
    context->mMaximumCorner[0] = -1;
    context->mMinimumCorner[2] = -1;
    gabi::FrameLocal<BoundsCorners> corners(0x2C);
    __construct_array(&*corners, 8, sizeof(sead::RenderVector3), allocateOrReuseStorage);
    f32 positiveY = sSelectorHalfExtent.y;
    f32 positiveX = sSelectorHalfExtent.x;
    f32 negativeY = -positiveY;
    f32 positiveZ = sSelectorHalfExtent.z;
    f32 negativeX = -positiveX;
    f32 negativeZ = -positiveZ;
    gabi::FrameLocal<sead::RenderVector3> negative(0x20);
    negative->y = negativeY;
    negative->x = negativeX;
    negative->z = negativeZ;
    corners->mPoints[1].x = positiveX;
    corners->mPoints[6].z = positiveZ;
    corners->mPoints[5].z = negativeZ;
    corners->mPoints[0].x = negativeX;
    corners->mPoints[1].z = negativeZ;
    corners->mPoints[7].x = negativeX;
    corners->mPoints[4].z = negativeZ;
    corners->mPoints[4].x = negativeX;
    corners->mPoints[1].y = negativeY;
    corners->mPoints[3].x = negativeX;
    corners->mPoints[5].x = positiveX;
    corners->mPoints[5].y = positiveY;
    corners->mPoints[2].y = negativeY;
    corners->mPoints[2].x = positiveX;
    const f32 limit = std::numeric_limits<f32>::max();
    f32 minimumX = limit, minimumY = limit, minimumZ = limit;
    f32 maximumX = -limit, maximumY = -limit, maximumZ = -limit;
    gabi::FrameLocal<Bounds3> extrema(8);
    extrema->mMinimum.z = minimumX;
    extrema->mMaximum.y = maximumY;
    extrema->mMaximum.x = maximumX;
    corners->mPoints[4].y = positiveY;
    corners->mPoints[6].x = positiveX;
    corners->mPoints[6].y = positiveY;
    corners->mPoints[7].y = positiveY;
    extrema->mMaximum.z = maximumZ;
    extrema->mMinimum.y = minimumY;
    corners->mPoints[2].z = positiveZ;
    corners->mPoints[0].z = negativeZ;
    corners->mPoints[3].z = positiveZ;
    extrema->mMinimum.x = minimumX;
    corners->mPoints[7].z = positiveZ;
    corners->mPoints[0].y = negativeY;
    corners->mPoints[3].y = negativeY;
    for (s32 index = 0; index < 8; ++index) {
        sead::RenderVector3* point = &corners->mPoints[index];
        aglGeometryMultiplyVector(&context->mTransform, point, point);
        f32 x = point->x;
        f32 y = point->y;
        if (minimumX > x) {
            minimumX = x;
            maximumX = extrema->mMaximum.x;
            x = point->x;
            extrema->mMinimum.x = minimumX;
        }
        f32 z = point->z;
        if (minimumY > y) {
            minimumY = y;
            maximumY = extrema->mMaximum.y;
            y = point->y;
            extrema->mMinimum.y = minimumY;
        }
        if (minimumZ > z) {
            minimumZ = z;
            maximumZ = extrema->mMaximum.z;
            z = point->z;
            extrema->mMinimum.z = minimumZ;
        }
        if (maximumX < x) {
            maximumX = x;
            minimumX = extrema->mMinimum.x;
            x = point->x;
            extrema->mMaximum.x = maximumX;
        }
        if (maximumY < y) {
            maximumY = y;
            minimumY = extrema->mMinimum.y;
            y = point->y;
            extrema->mMaximum.y = maximumY;
        }
        if (maximumZ < z) {
            maximumZ = z;
            minimumZ = extrema->mMinimum.z;
            z = point->z;
            extrema->mMaximum.z = maximumZ;
        }
        if (minimumX == x) context->mMinimumCorner[0] = index;
        if (minimumY == y) context->mMinimumCorner[1] = index;
        if (minimumZ == z) context->mMinimumCorner[2] = index;
        if (maximumX == x) context->mMaximumCorner[0] = index;
        if (maximumY == y) context->mMaximumCorner[1] = index;
        if (maximumZ == z) context->mMaximumCorner[2] = index;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
static f32 transformCornerCoordinate(const sead::RenderVector3* corner,
                                     const be<f32>* coefficients) {
    f32 productY = gabi::fmuls_ppc(corner->y, coefficients[1]);
    f32 sumXY = gabi::fmadds(corner->x, coefficients[0], productY);
    f32 sumXYZ = gabi::fmadds(corner->z, coefficients[2], sumXY);
    return gabi::fadds_ppc(sumXYZ, coefficients[3]);
}
/* 027C9F68 */
void transformBounds(const BoundsTransformState_l* context, Bounds3* destination,
                     const Bounds3* bounds) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<Bounds3> input(8);
    f32 minimumX = bounds->mMinimum.x;
    f32 minimumY = bounds->mMinimum.y;
    input->mMinimum.x = minimumX;
    input->mMinimum.y = minimumY;
    f32 minimumZ = bounds->mMinimum.z;
    f32 maximumY = bounds->mMaximum.y;
    input->mMinimum.z = minimumZ;
    input->mMaximum.y = maximumY;
    f32 maximumZ = bounds->mMaximum.z;
    f32 maximumX = bounds->mMaximum.x;
    input->mMaximum.z = maximumZ;
    input->mMaximum.x = maximumX;
    gabi::FrameLocal<BoundsCorners> corners(0x20);
    corners->mPoints[0].x = minimumX;
    corners->mPoints[0].z = minimumZ;
    corners->mPoints[0].y = minimumY;
    corners->mPoints[1].x = maximumX;
    corners->mPoints[1].y = minimumY;
    corners->mPoints[1].z = minimumZ;
    corners->mPoints[2].x = maximumX;
    corners->mPoints[2].y = minimumY;
    corners->mPoints[2].z = maximumZ;
    corners->mPoints[3].x = minimumX;
    corners->mPoints[3].y = minimumY;
    corners->mPoints[3].z = maximumZ;
    corners->mPoints[4].x = minimumX;
    corners->mPoints[4].y = maximumY;
    corners->mPoints[4].z = minimumZ;
    corners->mPoints[5].x = maximumX;
    corners->mPoints[5].y = maximumY;
    corners->mPoints[5].z = minimumZ;
    corners->mPoints[6].x = maximumX;
    corners->mPoints[6].y = maximumY;
    corners->mPoints[6].z = maximumZ;
    corners->mPoints[7].y = maximumY;
    corners->mPoints[7].x = minimumX;
    corners->mPoints[7].z = maximumZ;
    f32 lowX = transformCornerCoordinate(&corners->mPoints[context->mMinimumCorner[0]], context->mTransform.mRows[0]);
    f32 lowY = transformCornerCoordinate(&corners->mPoints[context->mMinimumCorner[1]], context->mTransform.mRows[1]);
    f32 lowZ = transformCornerCoordinate(&corners->mPoints[context->mMinimumCorner[2]], context->mTransform.mRows[2]);
    f32 highX = transformCornerCoordinate(&corners->mPoints[context->mMaximumCorner[0]], context->mTransform.mRows[0]);
    f32 highY = transformCornerCoordinate(&corners->mPoints[context->mMaximumCorner[1]], context->mTransform.mRows[1]);
    f32 highZ = transformCornerCoordinate(&corners->mPoints[context->mMaximumCorner[2]], context->mTransform.mRows[2]);
    destination->mMinimum.x = lowX;
    destination->mMinimum.y = lowY;
    destination->mMaximum.x = highX;
    destination->mMaximum.y = highY;
    destination->mMinimum.z = lowZ;
    destination->mMaximum.z = highZ;
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027CA320 */
void transformSphereBounds(void* context, Bounds3* destination, const Sphere* sphere) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<Bounds3> bounds(8);
    const f32 limit = std::numeric_limits<f32>::max();
    bounds->mMaximum.x = -limit;
    bounds->mMinimum.x = limit;
    bounds->mMaximum.y = -limit;
    bounds->mMinimum.y = limit;
    bounds->mMaximum.z = -limit;
    bounds->mMinimum.z = limit;
    sphereBounds(context, bounds, sphere);
    transformBounds(static_cast<const BoundsTransformState_l*>(context), destination, bounds);
}
static f32 projectCornerCoordinate(const f32* row, f32 denominator,
                                   f32 x, f32 y, f32 z) {
    f32 scaledY = gabi::fdivs_ppc(row[1], denominator);
    f32 productY = gabi::fmuls_ppc(scaledY, y);
    f32 scaledX = gabi::fdivs_ppc(row[0], denominator);
    f32 sumXY = gabi::fmadds(scaledX, x, productY);
    f32 scaledZ = gabi::fdivs_ppc(row[2], denominator);
    f32 sumXYZ = gabi::fmadds(scaledZ, z, sumXY);
    f32 translation = gabi::fdivs_ppc(row[3], denominator);
    return gabi::fadds_ppc(sumXYZ, translation);
}
/* 027CA37C */
void projectBounds(const BoundsProjectionState_l* context, Bounds3* destination,
                   const Bounds3* bounds) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderMatrix44> combined(0x68);
    aglGeometryCombineProjection(combined, &context->mProjection,
                                &context->mBoundsTransform.mTransform);
    gabi::FrameLocal<BoundsCorners> corners(8);
    __construct_array(&*corners, 8, sizeof(sead::RenderVector3), allocateOrReuseStorage);
    for (u32 index = 0; index < 8; ++index) {
        bool maximumX = index == 1 || index == 2 || index == 5 || index == 6;
        bool maximumY = index >= 4;
        bool maximumZ = index == 2 || index == 3 || index == 6 || index == 7;
        corners->mPoints[index].x = maximumX ? bounds->mMaximum.x : bounds->mMinimum.x;
        corners->mPoints[index].y = maximumY ? bounds->mMaximum.y : bounds->mMinimum.y;
        corners->mPoints[index].z = maximumZ ? bounds->mMaximum.z : bounds->mMinimum.z;
    }
    f32 rows[4][4];
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            rows[row][column] = combined->mRows[row][column];
    const f32 limit = std::numeric_limits<f32>::max();
    destination->mMaximum.z = -limit;
    destination->mMaximum.y = -limit;
    destination->mMaximum.x = -limit;
    destination->mMinimum.y = limit;
    destination->mMinimum.z = limit;
    destination->mMinimum.x = limit;
    for (u32 index = 0; index < 8; ++index) {
        sead::RenderVector3* corner = &corners->mPoints[index];
        f32 x = corner->x;
        f32 y = corner->y;
        f32 productY = gabi::fmuls_ppc(rows[3][1], y);
        f32 sumXY = gabi::fmadds(rows[3][0], x, productY);
        f32 z = corner->z;
        f32 sumXYZ = gabi::fmadds(rows[3][2], z, sumXY);
        f32 denominator = gabi::fadds_ppc(sumXYZ, rows[3][3]);
        f32 projectedX = projectCornerCoordinate(rows[0], denominator, x, y, z);
        f32 projectedY = projectCornerCoordinate(rows[1], denominator, x, y, z);
        f32 projectedZ = projectCornerCoordinate(rows[2], denominator, x, y, z);
        corner->y = projectedY;
        corner->x = projectedX;
        corner->z = projectedZ;
        if (projectedX < -1.0f) projectedX = -1.0f;
        else if (projectedX > 1.0f) projectedX = 1.0f;
        projectedZ = corner->z;
        corner->x = projectedX;
        if (projectedY < -1.0f) projectedY = -1.0f;
        else if (projectedY > 1.0f) projectedY = 1.0f;
        f32 oldMinimumX = destination->mMinimum.x;
        corner->y = projectedY;
        if (projectedZ < -1.0f) projectedZ = -1.0f;
        else if (projectedZ > 1.0f) projectedZ = 1.0f;
        f32 oldMinimumY = destination->mMinimum.y;
        corner->z = projectedZ;
        if (oldMinimumX > projectedX) destination->mMinimum.x = projectedX;
        f32 oldMinimumZ = destination->mMinimum.z;
        if (oldMinimumY > projectedY) destination->mMinimum.y = projectedY;
        f32 oldMaximumX = destination->mMaximum.x;
        if (oldMinimumZ > projectedZ) destination->mMinimum.z = projectedZ;
        f32 oldMaximumY = destination->mMaximum.y;
        if (oldMaximumX < projectedX) destination->mMaximum.x = projectedX;
        f32 oldMaximumZ = destination->mMaximum.z;
        if (oldMaximumY < projectedY) destination->mMaximum.y = projectedY;
        if (oldMaximumZ < projectedZ) destination->mMaximum.z = projectedZ;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027CA7D0 */
void projectSphereBounds(void* context, Bounds3* destination, const Sphere* sphere) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<Bounds3> bounds(8);
    const f32 limit = std::numeric_limits<f32>::max();
    bounds->mMaximum.x = -limit;
    bounds->mMinimum.x = limit;
    bounds->mMaximum.y = -limit;
    bounds->mMinimum.y = limit;
    bounds->mMaximum.z = -limit;
    bounds->mMinimum.z = limit;
    sphereBounds(context, bounds, sphere);
    projectBounds(static_cast<const BoundsProjectionState_l*>(context), destination, bounds);
}
const sead::RenderMatrix44* clipTextureTransform() { return &sClipTextureTransform; }
const sead::RenderMatrix44* verticalDepthExchange() { return &sVerticalDepthExchange; }
/* 027CA82C */
void initializeClipSpaceTransforms() {
    gabi::AutoFrame frame{gabi::current_function()};
    sVerticalDepthExchange.mRows[0][0] = 1.0f;
    sVerticalDepthExchange.mRows[0][1] = 0.0f;
    sVerticalDepthExchange.mRows[0][2] = 0.0f;
    sVerticalDepthExchange.mRows[0][3] = 0.0f;
    sVerticalDepthExchange.mRows[1][0] = 0.0f;
    sVerticalDepthExchange.mRows[1][1] = 0.0f;
    sVerticalDepthExchange.mRows[1][2] = 1.0f;
    sVerticalDepthExchange.mRows[1][3] = 0.0f;
    sVerticalDepthExchange.mRows[2][0] = 0.0f;
    sVerticalDepthExchange.mRows[2][1] = 1.0f;
    sVerticalDepthExchange.mRows[2][2] = 0.0f;
    sVerticalDepthExchange.mRows[2][3] = 0.0f;
    sVerticalDepthExchange.mRows[3][0] = 0.0f;
    sVerticalDepthExchange.mRows[3][1] = 0.0f;
    sVerticalDepthExchange.mRows[3][2] = 0.0f;
    sVerticalDepthExchange.mRows[3][3] = 1.0f;
    sClipTextureTransform.mRows[0][0] = 0.5f;
    sClipTextureTransform.mRows[0][1] = 0.0f;
    sClipTextureTransform.mRows[0][2] = 0.0f;
    sClipTextureTransform.mRows[0][3] = 0.5f;
    sClipTextureTransform.mRows[1][0] = 0.0f;
    sClipTextureTransform.mRows[1][1] = -0.5f;
    sClipTextureTransform.mRows[1][2] = 0.0f;
    sClipTextureTransform.mRows[1][3] = 0.5f;
    sClipTextureTransform.mRows[2][0] = 0.0f;
    sClipTextureTransform.mRows[2][1] = 0.0f;
    sClipTextureTransform.mRows[2][2] = 0.5f;
    sClipTextureTransform.mRows[2][3] = 0.5f;
    sClipTextureTransform.mRows[3][0] = 0.0f;
    sClipTextureTransform.mRows[3][1] = 0.0f;
    sClipTextureTransform.mRows[3][2] = 0.0f;
    sClipTextureTransform.mRows[3][3] = 1.0f;
}
/* 027CA8D8 */
void destroyStorage(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object && (flags & 1)) aglUtilityTextStorageDelete(object);
}
/* 027CA8EC */
void* allocateOrReuseStorage(void* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    return object ? object : allocateGeometryStorage(12);
}
/* 027CA918 */
void emptyStorageHook(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027CA91C */
Bounds3* constructEmptyBounds(Bounds3* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) {
        object = static_cast<Bounds3*>(allocateGeometryStorage(sizeof(Bounds3)));
        if (!object) return nullptr;
    }
    const f32 limit = std::numeric_limits<f32>::max();
    object->mMinimum.y = limit;
    object->mMaximum.x = -limit;
    object->mMinimum.x = limit;
    object->mMaximum.y = -limit;
    object->mMinimum.z = limit;
    object->mMaximum.z = -limit;
    return object;
}
/* 027CA978 */
Plane* constructDefaultPlane(Plane* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) {
        object = static_cast<Plane*>(allocateGeometryStorage(sizeof(Plane)));
        if (!object) return nullptr;
    }
    u32 x = sDefaultPlaneNormal.mX;
    object->mNormal.x = gabi::f32_from_bits(x);
    u32 y = sDefaultPlaneNormal.mY;
    object->mNormal.y = gabi::f32_from_bits(y);
    u32 z = sDefaultPlaneNormal.mZ;
    object->mDistance = 0.0f;
    object->mNormal.z = gabi::f32_from_bits(z);
    return object;
}

/* 027CAA2C */
void copyTransposed(sead::RenderMatrix44* destination, const sead::RenderMatrix44* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    /* Individual load/store pairs retain the original behavior when both views alias. */
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->mRows[row][column] = source->mRows[column][row];
}
/* 027CAAB0 */
void clearMatrix(sead::RenderMatrix44* matrix) {
    gabi::AutoFrame frame{gabi::current_function()};
    matrix->mRows[1][0] = 0.0f;
    matrix->mRows[1][2] = 0.0f;
    matrix->mRows[0][0] = 0.0f;
    matrix->mRows[0][2] = 0.0f;
    matrix->mRows[3][0] = 0.0f;
    matrix->mRows[3][2] = 0.0f;
    matrix->mRows[2][0] = 0.0f;
    matrix->mRows[2][2] = 0.0f;
    matrix->mRows[2][3] = 0.0f;
    matrix->mRows[2][1] = 0.0f;
    matrix->mRows[3][3] = 0.0f;
    matrix->mRows[3][1] = 0.0f;
    matrix->mRows[0][3] = 0.0f;
    matrix->mRows[0][1] = 0.0f;
    matrix->mRows[1][3] = 0.0f;
    matrix->mRows[1][1] = 0.0f;
}
} }
/* WWHD glue
data sDefaultPlaneNormal 104A0934
data sSelectorHalfExtent 104A0958
data sVerticalDepthExchange 104A21A8
data sClipTextureTransform 104A21E8
ext 0273AD10 void* allocateGeometryStorage(u32)
ext 028E8F64 void aglGeometryMultiplyVector(const sead::RenderMatrix34*, const sead::RenderVector3*, sead::RenderVector3*)
ext 0272D680 void aglGeometryCombineProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*, const sead::RenderMatrix34*)
*/
