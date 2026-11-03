#include "agl/ShadowContext.h"
#include <limits>
void aglShadowGetViewDirection(sead::RenderVector3*, const sead::RenderMatrix34*);
void aglShadowGetProjectionWithAxis(const agl::shadow::ProjectionInputPrefix_l*, sead::RenderMatrix34*, sead::RenderMatrix44*, const sead::RenderVector3*);
void aglShadowGetProjectionWithView(const agl::shadow::ProjectionInputPrefix_l*, sead::RenderMatrix34*, sead::RenderMatrix44*, const sead::RenderMatrix34*);
void aglShadowPopulateWorkspace(agl::shadow::GeometryWorkspace_l*, const sead::RenderMatrix34*, const sead::RenderMatrix44*, f64, f64);
namespace agl { namespace shadow {
static const ProjectionDimensions_l sShadowDefaultDimensions;
static u8 sRefreshPlanesEnabled = 1;
static const sead::RenderVector3 sShadowProjectionAxis;
static u8 sBuildPlanesOnUpdateEnabled = 1;
/* 027C93A8 */
void updateShadowView(ShadowContext* context, const ProjectionInputPrefix_l* input,
        const sead::RenderMatrix34* view, const sead::RenderMatrix44* projection,
        f64 minimumDepth, f64 maximumDepth, f64 tolerance) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 matrixMode = context->mMatrixMode.mValue;
    /* These are direct stfs images, not arithmetic float conversions. */
    context->mMaximumDepth = gabi::f32_from_bits(ppc_stfs_bits(maximumDepth));
    context->mMinimumDepth = gabi::f32_from_bits(ppc_stfs_bits(minimumDepth));
    if (matrixMode == 2)
        aglShadowGetProjectionWithAxis(input, &context->mTransform, &context->mProjection, &sShadowProjectionAxis);
    else
        aglShadowGetProjectionWithView(input, &context->mTransform, &context->mProjection, view);
    context->mWorkspace.mTolerance = gabi::f32_from_bits(ppc_stfs_bits(tolerance));
    aglShadowPopulateWorkspace(&context->mWorkspace, view, projection, minimumDepth, maximumDepth);
    aglShadowCopyAffine(view, &context->mView);
    aglShadowCopyProjection(projection, &context->mViewProjection);
    u8 crop = input->mCropEnabled;
    context->mTransformBoundsMethod.d = 0;
    context->mTransformBoundsMethod.i = -1;
    context->mPlaneCount = 0;
    context->mCropEnabled = crop;
    if (crop) {
        context->mSphereBoundsMethod.mVirtualIndex = -1;
        context->mSphereBoundsMethod.mThisAdjustment = 0;
        context->mTransformBoundsMethod.f = gabi::guest_fn((const void*)&geometry::transformBounds);
        context->mSphereBoundsMethod.mFunction = gabi::guest_fn((const void*)&geometry::transformSphereBounds);
        configureProjection(context, &context->mView, 1, &sShadowDefaultDimensions, 0.0, 0.0);
        gabi::FrameLocal<sead::RenderMatrix34> firstCopy(0x20), secondCopy(0x50);
        for (u32 row = 0; row < 3; ++row)
            for (u32 column = 0; column < 4; ++column)
                firstCopy->mRows[row][column] = context->mTransform.mRows[row][column];
        for (u32 row = 0; row < 3; ++row)
            for (u32 column = 0; column < 4; ++column)
                secondCopy->mRows[row][column] = firstCopy->mRows[row][column];
        gabi::FrameLocal<sead::RenderVector3> direction(8), retainedDirection(0x14);
        aglShadowGetViewDirection(direction, secondCopy);
        retainedDirection->z = direction->z;
        retainedDirection->x = direction->x;
        retainedDirection->y = direction->y;
        if (sBuildPlanesOnUpdateEnabled) {
            f32 numeratorZ = context->mViewProjection.mRows[2][3];
            f32 numeratorScale = context->mViewProjection.mRows[2][2];
            f32 denominatorScale = context->mViewProjection.mRows[3][2];
            f32 nearDepth = context->mMinimumDepth;
            f32 denominatorZ = context->mViewProjection.mRows[3][3];
            f32 nearNumerator = gabi::fnmsubs(numeratorScale, nearDepth, numeratorZ);
            f32 farDepth = context->mMaximumDepth;
            f32 nearDenominator = gabi::fnmsubs(denominatorScale, nearDepth, denominatorZ);
            f32 farDenominator = gabi::fnmsubs(denominatorScale, farDepth, denominatorZ);
            f32 farNumerator = gabi::fnmsubs(numeratorScale, farDepth, numeratorZ);
            f32 first = gabi::fdivs_ppc(nearNumerator, nearDenominator);
            f32 second = gabi::fdivs_ppc(farNumerator, farDenominator);
            buildPlanes(context, &context->mView, &context->mViewProjection,
                                 retainedDirection, 1, first, second);
        }
        geometry::initializeBoundsCornerSelectors(
            reinterpret_cast<geometry::BoundsTransformState_l*>(context));
    } else {
        context->mSphereBoundsMethod.mThisAdjustment = 0;
        context->mSphereBoundsMethod.mFunction = gabi::guest_fn((const void*)&geometry::projectSphereBounds);
        context->mSphereBoundsMethod.mVirtualIndex = -1;
        context->mTransformBoundsMethod.f = gabi::guest_fn((const void*)&geometry::projectBounds);
        aglShadowCopyAffine(&context->mTransform, &context->mCroppedAffine);
        aglShadowCopyProjection(&context->mProjection, &context->mCroppedProjection);
        buildPlanes(context, &context->mCroppedAffine, &context->mCroppedProjection,
                             nullptr, 0, -1.0, 1.0);
    }
    const f32 limit = std::numeric_limits<f32>::max();
    for (u32 index = 0; index < 3; ++index) {
        geometry::Bounds3* first = &context->mFirstBounds[index];
        first->mMaximum.z = -limit;
        first->mMinimum.z = limit;
        first->mMaximum.x = -limit;
        first->mMinimum.x = limit;
        first->mMaximum.y = -limit;
        first->mMinimum.y = limit;
        geometry::Bounds3* second = &context->mSecondBounds[index];
        second->mMinimum.x = limit;
        second->mMaximum.x = -limit;
        second->mMinimum.y = limit;
        second->mMinimum.z = limit;
        second->mMaximum.y = -limit;
        second->mMaximum.z = -limit;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027C9770 */
void refreshShadowPlanes(ShadowContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!context->mCropEnabled) return;
    configureProjection(context, &context->mView, 1, &sShadowDefaultDimensions, 0.0, 0.0);
    gabi::FrameLocal<sead::RenderMatrix34> firstCopy(0x20), secondCopy(0x50);
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            firstCopy->mRows[row][column] = context->mTransform.mRows[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            secondCopy->mRows[row][column] = firstCopy->mRows[row][column];
    gabi::FrameLocal<sead::RenderVector3> direction(8), retainedDirection(0x14);
    aglShadowGetViewDirection(direction, secondCopy);
    retainedDirection->z = direction->z;
    retainedDirection->x = direction->x;
    retainedDirection->y = direction->y;
    if (sRefreshPlanesEnabled)
        buildPlanes(context, &context->mCroppedAffine, &context->mCroppedProjection,
                             retainedDirection, 0, -1.0, 1.0);
}
} }
/* WWHD glue
data sShadowProjectionAxis 104A094C
data sBuildPlanesOnUpdateEnabled 101F9600
ext 0278D668 void aglShadowGetProjectionWithAxis(const agl::shadow::ProjectionInputPrefix_l*, sead::RenderMatrix34*, sead::RenderMatrix44*, const sead::RenderVector3*)
ext 0278D434 void aglShadowGetProjectionWithView(const agl::shadow::ProjectionInputPrefix_l*, sead::RenderMatrix34*, sead::RenderMatrix44*, const sead::RenderMatrix34*)
ext 027D85E8 void aglShadowPopulateWorkspace(agl::shadow::GeometryWorkspace_l*, const sead::RenderMatrix34*, const sead::RenderMatrix44*, f64, f64)
data sShadowDefaultDimensions 104A07A8
data sRefreshPlanesEnabled 101F9602
ext 02791C68 void aglShadowGetViewDirection(sead::RenderVector3*, const sead::RenderMatrix34*)
*/
