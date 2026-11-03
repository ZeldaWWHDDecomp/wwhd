#include "agl/ShadowContext.h"
#include <limits>
void aglShadowConcatProjection(const sead::RenderMatrix44*, const sead::RenderMatrix44*, sead::RenderMatrix44*);
void aglShadowProjectWorkspace(agl::shadow::GeometryWorkspace_l*, agl::geometry::Bounds3*, const sead::RenderMatrix44*, const sead::RenderMatrix34*);
void aglShadowCopyAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*);
void aglShadowCopyProjection(const sead::RenderMatrix44*, sead::RenderMatrix44*);
namespace agl { namespace shadow {
/* 027C8270 */
void configureProjection(ShadowContext* context, const sead::RenderMatrix34* suppliedTransform,
        u32 mode, const ProjectionDimensions_l* dimensions, f64 firstLimit, f64 secondLimit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderMatrix44> projection(0x80), correction(0x40);
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            projection->mRows[row][column] = context->mProjection.mRows[row][column];
    if (!mode && fitProjection(context, correction, projection, &context->mTransform,
                                       suppliedTransform, firstLimit, secondLimit))
        aglShadowConcatProjection(correction, projection, projection);
    gabi::FrameLocal<geometry::Bounds3> bounds(8);
    const f32 limit = std::numeric_limits<f32>::max();
    bounds->mMaximum.y = -limit;
    bounds->mMinimum.y = limit;
    bounds->mMaximum.z = -limit;
    bounds->mMinimum.z = limit;
    bounds->mMaximum.x = -limit;
    bounds->mMinimum.x = limit;
    aglShadowProjectWorkspace(&context->mWorkspace, bounds, projection, &context->mTransform);
    if (bounds->mMinimum.x > bounds->mMaximum.x
        || bounds->mMinimum.y > bounds->mMaximum.y
        || bounds->mMinimum.z > bounds->mMaximum.z) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return;
    }
    /* The frame rectangle keeps upper then lower corners, in the original order. */
    struct ProjectionInterval_l { sead::RenderVector3 mUpper, mLower; };
    WWHD_SIZE(ProjectionInterval_l, 24);
    WWHD_OFFSET(ProjectionInterval_l, mLower, 12);
    gabi::FrameLocal<ProjectionInterval_l> interval(0x20);
    interval->mLower.x = bounds->mMinimum.x;
    interval->mUpper.x = bounds->mMaximum.x;
    f32 halfWidth = gabi::fmuls_ppc(static_cast<f32>(static_cast<s32>(dimensions->mWidth)), 0.5f);
    interval->mUpper.y = bounds->mMaximum.y;
    f32 halfHeight = gabi::fmuls_ppc(static_cast<f32>(static_cast<s32>(dimensions->mHeight)), 0.5f);
    interval->mLower.y = bounds->mMinimum.y;
    interval->mUpper.z = bounds->mMaximum.z;
    interval->mLower.z = bounds->mMinimum.z;
    f32 lowerX = interval->mLower.x, upperX = interval->mUpper.x;
    f32 lowerY = interval->mLower.y, upperY = interval->mUpper.y;
    f32 width, height;
    if (mode == 2 && context->mCropEnabled) {
        f32 radius = gabi::fmuls_ppc(context->mWorkspace.mDiameter, 0.5f);
        f32 sumX = gabi::fadds_ppc(lowerX, upperX);
        f32 oldHeight = gabi::fsubs_ppc(upperY, lowerY);
        f32 sumY = gabi::fadds_ppc(lowerY, upperY);
        f32 centerX = gabi::fmuls_ppc(sumX, 0.5f);
        f32 centerY = gabi::fmuls_ppc(sumY, 0.5f);
        f32 newLowerX = gabi::fsubs_ppc(centerX, radius);
        f32 oldWidth = gabi::fsubs_ppc(upperX, lowerX);
        f32 newUpperX = gabi::fadds_ppc(centerX, radius);
        f32 newLowerY = gabi::fsubs_ppc(centerY, radius);
        interval->mLower.x = newLowerX;
        width = gabi::fsubs_ppc(newUpperX, newLowerX);
        f32 newUpperY = gabi::fadds_ppc(centerY, radius);
        f32 originX = gabi::fsubs_ppc(lowerX, newLowerX);
        height = gabi::fsubs_ppc(newUpperY, newLowerY);
        originX = gabi::fdivs_ppc(originX, width);
        interval->mLower.y = newLowerY;
        interval->mUpper.y = newUpperY;
        f32 originY = gabi::fsubs_ppc(lowerY, newLowerY);
        f32 spanX = gabi::fdivs_ppc(oldWidth, width);
        originY = gabi::fdivs_ppc(originY, height);
        context->mCropOriginX = originX;
        context->mCropSpanX = spanX;
        f32 spanY = gabi::fdivs_ppc(oldHeight, height);
        context->mCropOriginY = originY;
        interval->mUpper.x = newUpperX;
        context->mCropSpanY = spanY;
        lowerX = newLowerX; upperX = newUpperX;
        lowerY = newLowerY; upperY = newUpperY;
    } else {
        context->mCropOriginX = 0.0f;
        width = gabi::fsubs_ppc(upperX, lowerX);
        context->mCropOriginY = 0.0f;
        context->mCropSpanY = 1.0f;
        height = gabi::fsubs_ppc(upperY, lowerY);
        context->mCropSpanX = 1.0f;
    }
    geometry::clearMatrix(correction);
    f32 upperXRatio = gabi::fdivs_ppc(upperX, width);
    f32 upperZ = interval->mUpper.z, lowerZ = interval->mLower.z;
    f32 depth = gabi::fsubs_ppc(upperZ, lowerZ);
    f32 scaleX = gabi::fdivs_ppc(2.0f, width);
    f32 scaleZ = gabi::fdivs_ppc(2.0f, depth);
    correction->mRows[0][0] = scaleX;
    f32 upperZRatio = gabi::fdivs_ppc(upperZ, depth);
    f32 lowerZRatio = gabi::fdivs_ppc(lowerZ, depth);
    f32 upperYRatio = gabi::fdivs_ppc(upperY, height);
    f32 lowerYRatio = gabi::fdivs_ppc(lowerY, height);
    f32 scaleY = gabi::fdivs_ppc(2.0f, height);
    correction->mRows[1][1] = scaleY;
    f32 lowerXRatio = gabi::fdivs_ppc(lowerX, width);
    f32 translateX = gabi::fsubs_ppc(-upperXRatio, lowerXRatio);
    f32 translateZ = gabi::fsubs_ppc(-upperZRatio, lowerZRatio);
    f32 translateY = gabi::fsubs_ppc(-upperYRatio, lowerYRatio);
    correction->mRows[2][2] = scaleZ;
    correction->mRows[2][3] = translateZ;
    if (mode == 2 && context->mCropEnabled) {
        correction->mRows[0][3] = geometry::quantizeDown(translateX, halfWidth);
        correction->mRows[1][3] = geometry::quantizeDown(translateY, halfHeight);
    } else {
        correction->mRows[1][3] = translateY;
        correction->mRows[0][3] = translateX;
    }
    correction->mRows[3][3] = 1.0f;
    aglShadowConcatProjection(correction, projection, projection);
    aglShadowCopyAffine(&context->mTransform, &context->mCroppedAffine);
    aglShadowCopyProjection(projection, &context->mCroppedProjection);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
} }
/* WWHD glue
ext 028E89B4 void aglShadowConcatProjection(const sead::RenderMatrix44*, const sead::RenderMatrix44*, sead::RenderMatrix44*)
ext 027D9FD4 void aglShadowProjectWorkspace(agl::shadow::GeometryWorkspace_l*, agl::geometry::Bounds3*, const sead::RenderMatrix44*, const sead::RenderMatrix34*)
ext 028E90D4 void aglShadowCopyAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*)
ext 028E8970 void aglShadowCopyProjection(const sead::RenderMatrix44*, sead::RenderMatrix44*)
*/
