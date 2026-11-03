#include "agl/ShadowContext.h"
#include <limits>
void aglShadowInvertAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*);
void aglShadowInvertProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*);
void aglShadowClipAffineBounds(agl::shadow::GeometryWorkspace_l*, const agl::geometry::Bounds3*, const sead::RenderMatrix34*, be<u8>*);
void aglShadowClipProjectedBounds(agl::shadow::GeometryWorkspace_l*, const agl::geometry::Bounds3*, const sead::RenderMatrix44*, be<u8>*);
void aglShadowPrepareWorkspace(agl::shadow::GeometryWorkspace_l*);
void aglShadowTransformWorkspace(agl::shadow::GeometryWorkspace_l*, const sead::RenderMatrix44*);
namespace agl { namespace shadow {
static void accumulateBounds(geometry::Bounds3* destination, const geometry::Bounds3* source) {
    if (destination->mMinimum.x > source->mMinimum.x) destination->mMinimum.x = source->mMinimum.x;
    if (destination->mMinimum.y > source->mMinimum.y) destination->mMinimum.y = source->mMinimum.y;
    if (destination->mMinimum.z > source->mMinimum.z) destination->mMinimum.z = source->mMinimum.z;
    if (destination->mMaximum.x < source->mMaximum.x) destination->mMaximum.x = source->mMaximum.x;
    if (destination->mMaximum.y < source->mMaximum.y) destination->mMaximum.y = source->mMaximum.y;
    if (destination->mMaximum.z < source->mMaximum.z) destination->mMaximum.z = source->mMaximum.z;
}
static void applyBoundsMethod(ShadowContext* context, geometry::Bounds3* destination,
                               const geometry::Bounds3* source) {
    const auto& method = context->mTransformBoundsMethod;
    (context->*method)(destination, source);
}
/* 027C9AFC */
void collectProjectedBounds(ShadowContext* context, s32 width, s32 height,
                            f64 firstLimit, f64 secondLimit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderMatrix44> combinedProjection(0x6C);
    aglGeometryCombineProjection(combinedProjection, &context->mProjection, &context->mTransform);
    const f32 limit = std::numeric_limits<f32>::max();
    context->mCombinedBounds.mMinimum.z = limit;
    context->mCombinedBounds.mMaximum.x = -limit;
    context->mCombinedBounds.mMinimum.y = limit;
    context->mCombinedBounds.mMaximum.y = -limit;
    context->mCombinedBounds.mMinimum.x = limit;
    context->mCombinedBounds.mMaximum.z = -limit;
    for (u32 index = 0; index < 3; ++index) {
        const geometry::Bounds3* input = &context->mFirstBounds[index];
        if (!(input->mMinimum.x > input->mMaximum.x
              || input->mMinimum.y > input->mMaximum.y
              || input->mMinimum.z > input->mMaximum.z)) {
            gabi::FrameLocal<geometry::Bounds3> transformed(8);
            transformed->mMaximum.z = -limit;
            transformed->mMaximum.x = -limit;
            transformed->mMaximum.y = -limit;
            transformed->mMinimum.y = limit;
            transformed->mMinimum.x = limit;
            transformed->mMinimum.z = limit;
            applyBoundsMethod(context, transformed, input);
            accumulateBounds(&context->mSecondBounds[index], transformed);
        }
        accumulateBounds(&context->mCombinedBounds, &context->mSecondBounds[index]);
    }
    const geometry::Bounds3* combined = &context->mCombinedBounds;
    if (!(combined->mMinimum.x > combined->mMaximum.x
          || combined->mMinimum.y > combined->mMaximum.y
          || combined->mMinimum.z > combined->mMaximum.z)) {
        /* One frame slot holds either the inverse affine (crop) or the inverse projection. */
        union InverseMatrix_l { sead::RenderMatrix44 mProjection; sead::RenderMatrix34 mAffine; };
        gabi::FrameLocal<InverseMatrix_l> inverse(0x2C);
        struct ClipMasks_l { be<u8> mAffineUnused, mProjectedUnused, mAffine, mProjected; };
        WWHD_SIZE(ClipMasks_l, 4);
        WWHD_OFFSET(ClipMasks_l, mAffine, 2);
        WWHD_OFFSET(ClipMasks_l, mProjected, 3);
        gabi::FrameLocal<ClipMasks_l> masks(0x20);
        if (context->mCropEnabled) {
            sead::RenderMatrix34* inverseAffine = &inverse->mAffine;
            aglShadowInvertAffine(&context->mTransform, inverseAffine);
            masks->mAffineUnused = 0xFE;
            masks->mAffine = 0xFE;
            aglShadowClipAffineBounds(&context->mWorkspace, combined, inverseAffine, &masks->mAffine);
        } else {
            aglGeometryCombineProjection(&inverse->mProjection, &context->mProjection, &context->mTransform);
            aglShadowInvertProjection(&inverse->mProjection, &inverse->mProjection);
            masks->mProjectedUnused = 0xFF;
            masks->mProjected = 0xFF;
            aglShadowClipProjectedBounds(&context->mWorkspace, combined, &inverse->mProjection, &masks->mProjected);
        }
    }
    u32 activeSet = context->mWorkspace.mActiveSet;
    GeometryGroupSet_l* set = &context->mWorkspace.mSets[activeSet < 2 ? activeSet : 0];
    if (static_cast<s32>(set->mCapacity) <= 0) aglShadowPrepareWorkspace(&context->mWorkspace);
    if (!context->mCropEnabled)
        aglShadowTransformWorkspace(&context->mWorkspace, combinedProjection);
    gabi::FrameLocal<ProjectionDimensions_l> dimensions(0x24);
    dimensions->mWidth = width;
    dimensions->mHeight = height;
    u32 mode = static_cast<u32>(static_cast<s32>(context->mMatrixMode.mValue));
    configureProjection(context, &context->mView, mode, dimensions, firstLimit, secondLimit);
    aglGeometryCombineProjection(&context->mSecondProjection, &context->mCroppedProjection, &context->mTransform);
    aglShadowConcatProjection(geometry::clipTextureTransform(), &context->mSecondProjection, &context->mSecondProjection);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
} }
/* WWHD glue
ext 028E91EC void aglShadowInvertAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*)
ext 0274EDD4 void aglShadowInvertProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*)
ext 027D9AE8 void aglShadowClipAffineBounds(agl::shadow::GeometryWorkspace_l*, const agl::geometry::Bounds3*, const sead::RenderMatrix34*, be<u8>*)
ext 027D9C2C void aglShadowClipProjectedBounds(agl::shadow::GeometryWorkspace_l*, const agl::geometry::Bounds3*, const sead::RenderMatrix44*, be<u8>*)
ext 027D7F44 void aglShadowPrepareWorkspace(agl::shadow::GeometryWorkspace_l*)
ext 027D9F1C void aglShadowTransformWorkspace(agl::shadow::GeometryWorkspace_l*, const sead::RenderMatrix44*)
*/
