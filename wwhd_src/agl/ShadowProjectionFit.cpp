#include "agl/ShadowContext.h"
#include "sead/ProjectionState.h"
#include <cmath>
#include <limits>
void aglFitReadDirection(sead::RenderVector3*, const sead::RenderMatrix34*);
void aglFitReadAxis(sead::RenderVector3*, const sead::RenderMatrix34*);
void aglFitWorkspacePoint(sead::RenderVector3*, agl::shadow::GeometryWorkspace_l*, const sead::RenderMatrix34*);
void aglFitInvertAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*);
void aglFitMultiplyVector(const sead::RenderMatrix34*, const sead::RenderVector3*, sead::RenderVector3*);
void aglFitLookAt(sead::RenderMatrix34*, const sead::RenderVector3*, const sead::RenderVector3*, const sead::RenderVector3*);
void aglFitAffineProjection(sead::RenderMatrix44*, const sead::RenderMatrix34*, const sead::RenderMatrix44*);
void aglFitProjectWorkspace(agl::shadow::GeometryWorkspace_l*, agl::geometry::Bounds3*, const sead::RenderMatrix44*, const sead::RenderMatrix34*);
void aglFitInvertProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*);
void aglFitTranslation(sead::RenderMatrix44*, f64, f64, f64);
void aglFitScale(sead::RenderMatrix44*, f64, f64, f64);
sead::ProjectionState* aglFitConstructFrustum(sead::ProjectionState*, f64, f64, f64, f64, f64, f64);
sead::RenderMatrix44* aglFitFrustumMatrix(sead::ProjectionState*);
void aglFitDestroyFrustum(sead::ProjectionState*, u32);
namespace agl { namespace shadow {
/* Read-only slices of SEAD14's common defaults; no second global owner. */
static const sead::RenderVector3 sFitFrameOrigin;
static const sead::RenderVector3 sFitFrameUp;
template <u32 Rows> static void copyFitMatrix(sead::RenderMatrix34* destination,
        const sead::RenderMatrix34* source) {
    for (u32 row = 0; row < Rows; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->mRows[row][column] = source->mRows[row][column];
}
static void copyFitProjection(sead::RenderMatrix44* destination, const sead::RenderMatrix44* source) {
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->mRows[row][column] = source->mRows[row][column];
}
static f32 fitHomogeneousW(const sead::RenderMatrix44* matrix, f32 x, f32 y, f32 z) {
    const auto& row = matrix->mRows[3];
    f32 productY = gabi::fmuls_ppc(row[1], y);
    f32 sumXY = gabi::fmadds(row[0], x, productY);
    return gabi::fadds_ppc(gabi::fmadds(row[2], z, sumXY), row[3]);
}
static f32 fitProjectedCoordinate(const be<f32> (&row)[4], f32 x, f32 y, f32 z, f32 w) {
    f32 a = gabi::fdivs_ppc(row[0], w), b = gabi::fdivs_ppc(row[1], w);
    f32 c = gabi::fdivs_ppc(row[2], w), d = gabi::fdivs_ppc(row[3], w);
    f32 productY = gabi::fmuls_ppc(b, y);
    return gabi::fadds_ppc(gabi::fmadds(c, z, gabi::fmadds(a, x, productY)), d);
}
static void projectFitPoint(sead::RenderVector3* destination, const sead::RenderMatrix44* matrix,
        f32 x, f32 y, f32 z) {
    f32 w = fitHomogeneousW(matrix, x, y, z);
    f32 projectedY = fitProjectedCoordinate(matrix->mRows[1], x, y, z, w);
    f32 projectedX = fitProjectedCoordinate(matrix->mRows[0], x, y, z, w);
    f32 projectedZ = fitProjectedCoordinate(matrix->mRows[2], x, y, z, w);
    destination->y = projectedY;
    destination->x = projectedX;
    destination->z = projectedZ;
}
static f32 fitFusedFpr(f64 a, f64 c, f64 b) {
    return static_cast<f32>(gabi::ppc_nan3(a, b, c, a * round25(c) + b, gabi::ppc_dnan64()));
}
static f64 fitSelect(f64 test, f64 negative, f64 positive) {
    return test >= 0.0 ? positive : negative;
}
/* 027C7598 */
u32 fitProjection(ShadowContext* context, sead::RenderMatrix44* output,
        const sead::RenderMatrix44* projection, const sead::RenderMatrix34* transform,
        const sead::RenderMatrix34* suppliedTransform, f64 firstLimit, f64 secondLimit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderMatrix34> suppliedCopy(0x210), directionCopy(0x270);
    copyFitMatrix<3>(suppliedCopy, suppliedTransform);
    copyFitMatrix<3>(directionCopy, suppliedCopy);
    gabi::FrameLocal<sead::RenderVector3> directionResult(0x188), direction(0x38);
    aglFitReadDirection(directionResult, directionCopy);
    direction->z = directionResult->z;
    direction->y = directionResult->y;
    direction->x = directionResult->x;
    gabi::FrameLocal<sead::RenderMatrix34> axisCopy(0x240), axisInput(0x2A0);
    copyFitMatrix<3>(axisCopy, suppliedTransform);
    copyFitMatrix<3>(axisInput, axisCopy);
    gabi::FrameLocal<sead::RenderVector3> unusedAxis(0x1C4);
    aglFitReadAxis(unusedAxis, axisInput);
    const sead::RenderMatrix44* exchange = geometry::verticalDepthExchange();
    gabi::FrameLocal<sead::RenderMatrix44> combined(0xC8), originalCombined(0x300);
    aglShadowConcatProjection(exchange, projection, combined);
    aglGeometryCombineProjection(combined, combined, transform);
    copyFitProjection(originalCombined, combined);
    gabi::FrameLocal<sead::RenderMatrix34> workspaceTransform(0x2D0);
    copyFitMatrix<3>(workspaceTransform, suppliedTransform);
    gabi::FrameLocal<sead::RenderVector3> workspacePoint(0x44), center(0x14);
    aglFitWorkspacePoint(workspacePoint, &context->mWorkspace, workspaceTransform);
    center->x = workspacePoint->x;
    center->z = workspacePoint->z;
    center->y = workspacePoint->y;
    auto* inverseSupplied = reinterpret_cast<sead::RenderMatrix34*>(&*combined);
    aglFitInvertAffine(suppliedTransform, inverseSupplied);
    aglFitMultiplyVector(inverseSupplied, center, center);
    gabi::FrameLocal<sead::RenderVector3> transformedDirection(0x194), directionSnapshot(0xB0);
    aglFitMultiplyVector(transform, direction, transformedDirection);
    directionSnapshot->x = transformedDirection->x;
    directionSnapshot->y = transformedDirection->y;
    directionSnapshot->z = transformedDirection->z;
    gabi::FrameLocal<sead::RenderVector3> projectedDirection(0x68);
    projectFitPoint(projectedDirection, projection,
                    directionSnapshot->x, directionSnapshot->y, directionSnapshot->z);
    projectFitPoint(workspacePoint, exchange,
                    projectedDirection->x, projectedDirection->y, projectedDirection->z);
    gabi::FrameLocal<sead::RenderVector3> horizontalDirection(8);
    horizontalDirection->x = workspacePoint->x;
    horizontalDirection->y = 0.0f;
    horizontalDirection->z = workspacePoint->z;
    f32 length = aglShadowNormalizeVector(horizontalDirection);
    if (length != length || !(length > 0.0f)) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return 0;
    }
    aglFitLookAt(&context->mFirstAffine, &sFitFrameOrigin, horizontalDirection, &sFitFrameUp);
    gabi::FrameLocal<sead::RenderMatrix44> temporary(0x1D0), fittedCombined(0x380);
    aglFitAffineProjection(temporary, &context->mFirstAffine, originalCombined);
    copyFitProjection(fittedCombined, temporary);
    gabi::FrameLocal<geometry::Bounds3> bounds(0x50);
    const f32 limit = std::numeric_limits<f32>::max();
    bounds->mMaximum.z = -limit;
    bounds->mMinimum.z = limit;
    bounds->mMaximum.x = -limit;
    bounds->mMaximum.y = -limit;
    bounds->mMinimum.y = limit;
    bounds->mMinimum.x = limit;
    aglFitAffineProjection(temporary, &context->mFirstAffine, exchange);
    aglShadowConcatProjection(temporary, projection, temporary);
    gabi::FrameLocal<sead::RenderMatrix44> workspaceProjection(0x340);
    copyFitProjection(workspaceProjection, temporary);
    aglFitProjectWorkspace(&context->mWorkspace, bounds, workspaceProjection, transform);
    aglFitAffineProjection(temporary, &context->mFirstAffine, exchange);
    aglShadowConcatProjection(temporary, projection, temporary);
    gabi::FrameLocal<sead::RenderMatrix44> centerProjection(0x108);
    copyFitProjection(centerProjection, temporary);
    gabi::FrameLocal<sead::RenderVector3> transformedCenter(0x1A0), centerSnapshot(0xBC), projectedCenter(0xA4);
    aglFitMultiplyVector(transform, center, transformedCenter);
    centerSnapshot->x = transformedCenter->x;
    centerSnapshot->y = transformedCenter->y;
    centerSnapshot->z = transformedCenter->z;
    projectFitPoint(projectedCenter, centerProjection,
                    centerSnapshot->x, centerSnapshot->y, centerSnapshot->z);
    aglFitInvertProjection(combined, fittedCombined);
    gabi::FrameLocal<sead::RenderMatrix44> inverseTranspose(0x148);
    geometry::copyTransposed(inverseTranspose, combined);
    gabi::FrameLocal<sead::RenderVector3> transformedNormal(0x20);
    for (u32 row = 0; row < 3; ++row) {
        const auto& coefficients = inverseTranspose->mRows[row];
        f32 productY = gabi::fmuls_ppc(coefficients[1], direction->y);
        f32 sumXY = gabi::fmadds(coefficients[0], direction->x, productY);
        f32 sumXYZ = gabi::fmadds(coefficients[2], direction->z, sumXY);
        f32 value = gabi::fmadds(coefficients[3], 0.0f, sumXYZ);
        if (row == 0) transformedNormal->x = value;
        else if (row == 1) transformedNormal->y = value;
        else transformedNormal->z = value;
    }
    length = aglShadowNormalizeVector(transformedNormal);
    if (length != length || !(length > 0.0f)) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return 0;
    }
    f32 x = projectedCenter->x, y;
    f32 maximumZ = bounds->mMaximum.z;
    f32 normalY = transformedNormal->y;
    horizontalDirection->x = x;
    if (normalY != 0.0f) {
        f32 dx = gabi::fdivs_ppc(gabi::fmuls_ppc(direction->x, center->x), length);
        f32 dy = gabi::fdivs_ppc(gabi::fmuls_ppc(direction->y, center->y), length);
        f32 dz = gabi::fdivs_ppc(gabi::fmuls_ppc(direction->z, center->z), length);
        f32 dot = gabi::fadds_ppc(gabi::fadds_ppc(dx, dy), dz);
        f64 projectedZ = gabi::fmul_ppc(transformedNormal->z, gabi::fdivs_ppc(maximumZ, x));
        f64 zTerm = gabi::fdiv_ppc(projectedZ, normalY);
        f32 xTerm = gabi::fdivs_ppc(transformedNormal->x, normalY);
        f64 slope = gabi::fadd_ppc(zTerm, xTerm);
        f64 intercept = gabi::fdiv_ppc(dot, normalY);
        y = static_cast<f32>(gabi::fnmsub(slope, x, intercept));
    } else y = projectedCenter->y;
    gabi::FrameLocal<sead::RenderVector3> firstPosition(0x8C), secondPosition(0x98);
    f32 minimumZ = bounds->mMinimum.z;
    /* Each endpoint divides all coefficients by its own homogeneous W. */
    f32 firstW = fitHomogeneousW(combined, x, y, maximumZ);
    f32 secondW = fitHomogeneousW(combined, x, y, minimumZ);
    firstPosition->x = fitProjectedCoordinate(combined->mRows[0], x, y, maximumZ, firstW);
    firstPosition->y = fitProjectedCoordinate(combined->mRows[1], x, y, maximumZ, firstW);
    firstPosition->z = fitProjectedCoordinate(combined->mRows[2], x, y, maximumZ, firstW);
    secondPosition->x = fitProjectedCoordinate(combined->mRows[0], x, y, minimumZ, secondW);
    secondPosition->y = fitProjectedCoordinate(combined->mRows[1], x, y, minimumZ, secondW);
    secondPosition->z = fitProjectedCoordinate(combined->mRows[2], x, y, minimumZ, secondW);
    horizontalDirection->y = y;
    horizontalDirection->z = maximumZ;
    gabi::FrameLocal<sead::RenderVector3> firstDepthPoint(0x1AC), secondDepthPoint(0x1B8);
    aglFitMultiplyVector(suppliedTransform, firstPosition, firstDepthPoint);
    aglFitMultiplyVector(suppliedTransform, secondPosition, secondDepthPoint);
    f32 firstDepth = -static_cast<f32>(firstDepthPoint->z);
    f32 secondDepth = -static_cast<f32>(secondDepthPoint->z);
    f32 nearDepth = firstDepth, farDepth = secondDepth;
    f32 extent = gabi::fsubs_ppc(maximumZ, minimumZ);
    if (secondDepth < firstDepth) {
        horizontalDirection->x = x;
        horizontalDirection->y = y;
        horizontalDirection->z = minimumZ;
        nearDepth = secondDepth;
        farDepth = firstDepth;
    }
    if (!(gabi::fmuls_ppc(farDepth, nearDepth) > 0.0f) || farDepth == nearDepth) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return 0;
    }
    f32 lowerLimit = gabi::fadds_fpr(context->mMinimumDepth, std::fabs(firstLimit));
    f32 upperLimit = gabi::fsubs_fpr(context->mMaximumDepth, std::fabs(secondLimit));
    f64 selectedNear = fitSelect(gabi::fsubs_ppc(lowerLimit, nearDepth), nearDepth, lowerLimit);
    f64 selectedFar = fitSelect(gabi::fsubs_ppc(upperLimit, farDepth), upperLimit, farDepth);
    f64 nonnegativeNear = fitSelect(selectedNear, 0.0, selectedNear);
    f64 nonnegativeFar = fitSelect(selectedFar, 0.0, selectedFar);
    f32 product = gabi::fmuls_ppc(nonnegativeNear, nonnegativeFar);
    f64 reciprocalRoot = 0.0;
    if (product > 0.0f) {
        reciprocalRoot = gabi::frsqrte(product);
        f32 square = gabi::fmuls_fpr(reciprocalRoot, reciprocalRoot);
        f32 half = gabi::fmuls_fpr(reciprocalRoot, 0.5f);
        f32 correction = gabi::fnmsubs(square, product, 3.0f);
        reciprocalRoot = gabi::fmuls_ppc(correction, half);
    }
    f32 depthSpan = gabi::fsubs_ppc(farDepth, nearDepth);
    f32 offset = fitFusedFpr(reciprocalRoot, product, nearDepth);
    f32 scaledNear = gabi::fmuls_ppc(gabi::fdivs_ppc(extent, depthSpan), offset);
    gabi::FrameLocal<sead::RenderMatrix44> translation(0x3C0), scale(0x400);
    aglFitTranslation(translation, -static_cast<f32>(horizontalDirection->x),
        -static_cast<f32>(horizontalDirection->y),
        gabi::fsubs_ppc(-static_cast<f32>(horizontalDirection->z), scaledNear));
    aglFitScale(scale, 1.0, 1.0, -1.0);
    gabi::FrameLocal<sead::ProjectionState> frustum(0x440);
    aglFitConstructFrustum(frustum, scaledNear, gabi::fadds_ppc(scaledNear, extent), 1.0, -1.0, -1.0, 1.0);
    sead::RenderMatrix44* frustumMatrix = aglFitFrustumMatrix(frustum);
    aglShadowConcatProjection(scale, frustumMatrix, &context->mFirstProjection);
    aglShadowConcatProjection(&context->mFirstProjection, translation, &context->mFirstProjection);
    aglFitDestroyFrustum(frustum, 2);
    aglShadowConcatProjection(exchange, &context->mFirstProjection, output);
    aglGeometryCombineProjection(output, output, &context->mFirstAffine);
    aglShadowConcatProjection(output, exchange, output);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    return 1;
}
} }
/* WWHD glue
data sFitFrameOrigin 104A0928
data sFitFrameUp 104A0940
ext 02791C68 void aglFitReadDirection(sead::RenderVector3*, const sead::RenderMatrix34*)
ext 02791C90 void aglFitReadAxis(sead::RenderVector3*, const sead::RenderMatrix34*)
ext 027DA23C void aglFitWorkspacePoint(sead::RenderVector3*, agl::shadow::GeometryWorkspace_l*, const sead::RenderMatrix34*)
ext 028E91EC void aglFitInvertAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*)
ext 028E8F64 void aglFitMultiplyVector(const sead::RenderMatrix34*, const sead::RenderVector3*, sead::RenderVector3*)
ext 02791D00 void aglFitLookAt(sead::RenderMatrix34*, const sead::RenderVector3*, const sead::RenderVector3*, const sead::RenderVector3*)
ext 02525C94 void aglFitAffineProjection(sead::RenderMatrix44*, const sead::RenderMatrix34*, const sead::RenderMatrix44*)
ext 027D9FD4 void aglFitProjectWorkspace(agl::shadow::GeometryWorkspace_l*, agl::geometry::Bounds3*, const sead::RenderMatrix44*, const sead::RenderMatrix34*)
ext 0274EDD4 void aglFitInvertProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*)
ext 028E8AB8 void aglFitTranslation(sead::RenderMatrix44*, f64, f64, f64)
ext 028E8B04 void aglFitScale(sead::RenderMatrix44*, f64, f64, f64)
ext 0274E594 sead::ProjectionState* aglFitConstructFrustum(sead::ProjectionState*, f64, f64, f64, f64, f64, f64)
ext 0274D80C sead::RenderMatrix44* aglFitFrustumMatrix(sead::ProjectionState*)
ext 0274E69C void aglFitDestroyFrustum(sead::ProjectionState*, u32)
*/
