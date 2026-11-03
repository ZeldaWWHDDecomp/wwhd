#include "agl/ShadowContext.h"
#include <limits>
void aglShadowConstructArray(void*, u32, u32, u32);
namespace agl { namespace shadow {
/* Borrowed read-only defaults, initialized by the approved original SEAD14 startup. */
static const sead::RenderMatrix34 sShadowAffineDefaults;
static const sead::RenderMatrix44 sShadowProjectionDefaults;
static const ProjectionBottomRow_l sShadowBottomRow;
struct PairDefaults_l { be<f32> mX, mY; };
WWHD_SIZE(PairDefaults_l, 8);
WWHD_OFFSET(PairDefaults_l, mY, 4);
static const PairDefaults_l sShadowScaleDefaults;
static const PairDefaults_l sShadowOffsetDefaults;
static sead::SafeTextVtable sShadowParameterTextTable;
static sead::VtableSlot sShadowMatrixParameterTable;
static const sead::VtableSlot sShadowContextMethodTable;
template <u32 Rows> static void copyRows(
        be<f32> (&destination)[Rows][4], const be<f32> (&source)[Rows][4]) {
    for (u32 row = 0; row < Rows; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination[row][column] = source[row][column];
}
/* 027C70A8 */
ShadowContext* constructShadowContext(ShadowContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!context) {
        context = static_cast<ShadowContext*>(aglShadowAllocateContext(sizeof(ShadowContext)));
        if (!context) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            return nullptr;
        }
    }
    aglShadowConstructParameterOwner(context->mParameterOwner);
    context->mTransformBoundsMethod.f = gabi::guest_fn((const void*)&geometry::transformBounds);
    context->mTransformBoundsMethod.d = 0;
    context->mSphereBoundsMethod.mFunction = gabi::guest_fn((const void*)&geometry::transformSphereBounds);
    context->mShadowMethods = gabi::ea(&sShadowContextMethodTable);
    context->mWorkspaceMethods = workspaceMethodsAddress();
    context->mSphereBoundsMethod.mVirtualIndex = -1;
    context->mSphereBoundsMethod.mThisAdjustment = 0;
    context->mTransformBoundsMethod.i = -1;
    gabi::FrameLocal<sead::SafeText> name(8), label(0x10), description(0x18);
    name->mVtable = &sShadowParameterTextTable;
    label->mVtable = &sShadowParameterTextTable;
    label->mText.v = gabi::guest_str("Matrix Calc Type");
    name->mText.v = gabi::guest_str("matrix_calc_type");
    aglShadowConstructMatrixParameter(&context->mMatrixMode);
    context->mMatrixMode.mBinding.mMethods = &sShadowMatrixParameterTable;
    description->mVtable = &sShadowParameterTextTable;
    description->mText.v = gabi::guest_str("");
    aglShadowBindMatrixParameter(&context->mMatrixMode, name, label, description, context);
    context->mMatrixMode.mValue = 0;
    context->mParameterFlag = 0;
    aglShadowConstructWorkspace(&context->mWorkspace);
    context->mMinimumDepth = 0.0f;
    context->mMaximumDepth = 0.0f;
    copyRows(context->mTransform.mRows, sShadowAffineDefaults.mRows);
    copyRows(context->mProjection.mRows, sShadowProjectionDefaults.mRows);
    copyRows(context->mFirstAffine.mRows, sShadowAffineDefaults.mRows);
    copyRows(context->mFirstProjection.mRows, sShadowProjectionDefaults.mRows);
    copyRows(context->mCroppedAffine.mRows, sShadowAffineDefaults.mRows);
    aglShadowCompleteMatrix(&context->mCroppedProjection, &sShadowAffineDefaults, &sShadowBottomRow);
    copyRows(context->mSecondProjection.mRows, sShadowProjectionDefaults.mRows);
    copyRows(context->mView.mRows, sShadowAffineDefaults.mRows);
    copyRows(context->mViewProjection.mRows, sShadowProjectionDefaults.mRows);
    /* The original lifetime helper receives these owned constructors by address. */
    aglShadowConstructArray(context->mFirstBounds, 3u, sizeof(geometry::Bounds3),
               gabi::guest_fn((const void*)&geometry::constructEmptyBounds));
    aglShadowConstructArray(context->mSecondBounds, 3u, sizeof(geometry::Bounds3),
               gabi::guest_fn((const void*)&geometry::constructEmptyBounds));
    const f32 limit = std::numeric_limits<f32>::max();
    context->mCombinedBounds.mMinimum.y = limit;
    context->mCombinedBounds.mMaximum.x = -limit;
    context->mCombinedBounds.mMinimum.x = limit;
    context->mCombinedBounds.mMaximum.y = -limit;
    context->mCombinedBounds.mMinimum.z = limit;
    context->mCombinedBounds.mMaximum.z = -limit;
    context->mPlaneCount = 0;
    aglShadowConstructArray(context->mPlanes, 24u, sizeof(geometry::Plane),
               gabi::guest_fn((const void*)&geometry::constructDefaultPlane));
    context->mCropEnabled = 1;
    context->mCropOriginX = sShadowScaleDefaults.mX;
    context->mCropOriginY = sShadowScaleDefaults.mY;
    context->mCropSpanX = sShadowOffsetDefaults.mX;
    context->mCropSpanY = sShadowOffsetDefaults.mY;
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    return context;
}
} }
/* WWHD glue
data sShadowContextMethodTable 1016A6CC
ext 028EFFD0 void aglShadowConstructArray(void*, u32, u32, u32)
data sShadowAffineDefaults 104A041C
data sShadowProjectionDefaults 104A048C
data sShadowBottomRow 104A09A4
data sShadowScaleDefaults 104A0908
data sShadowOffsetDefaults 104A0920
data sShadowParameterTextTable 1016A554
data sShadowMatrixParameterTable 1016A58C
str 1016A66C "matrix_calc_type"
str 1016A680 "Matrix Calc Type"
str 1016A550 ""
ext 0273AD10 void* aglShadowAllocateContext(u32)
ext 027A84B8 void aglShadowConstructParameterOwner(void*)
ext 0279E674 void aglShadowConstructMatrixParameter(agl::utility_parameters::MatrixModeParameter*)
ext 0279E618 void aglShadowBindMatrixParameter(agl::utility_parameters::MatrixModeParameter*, const sead::SafeText*, const sead::SafeText*, const sead::SafeText*, void*)
ext 027D7E04 void aglShadowConstructWorkspace(agl::shadow::GeometryWorkspace_l*)
ext 02791BE4 void aglShadowCompleteMatrix(sead::RenderMatrix44*, const sead::RenderMatrix34*, const agl::shadow::ProjectionBottomRow_l*)
*/
