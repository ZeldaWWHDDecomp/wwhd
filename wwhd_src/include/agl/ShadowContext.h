/* Full storage of the assigned shadow context, from its own constructor.
 * The parameter owner prefix and foreign workspace keep qualified local views. */
#pragma once
#include "agl/ShadowGeometry.h"
#include "agl/UtilityParameterViews.h"
#include "sead/InlineInstances.h"
namespace agl { namespace shadow {
struct ProjectionBottomRow_l { be<f32> mValue[4]; };
WWHD_SIZE(ProjectionBottomRow_l, 16);
struct ShadowContext {
    u8 mParameterOwner[0x58];
    be<u32> mShadowMethods, mWorkspaceMethods;
    sead::instances::MemberFunction mSphereBoundsMethod;
    gabi::ptmf<void (ShadowContext::*)(geometry::Bounds3*, const geometry::Bounds3*)> mTransformBoundsMethod;
    utility_parameters::MatrixModeParameter mMatrixMode;
    be<u8> mParameterFlag;
    u8 mUnobserved81[3];
    GeometryWorkspace_l mWorkspace;
    be<s32> mMinimumCorner[3];
    be<f32> mMinimumDepth;
    be<s32> mMaximumCorner[3];
    be<f32> mMaximumDepth;
    sead::RenderMatrix34 mTransform;
    sead::RenderMatrix44 mProjection;
    u8 mUnobservedMatrix1A4[0x40];
    sead::RenderMatrix34 mFirstAffine;
    sead::RenderMatrix44 mFirstProjection;
    sead::RenderMatrix34 mCroppedAffine;
    sead::RenderMatrix44 mCroppedProjection;
    sead::RenderMatrix44 mSecondProjection;
    sead::RenderMatrix34 mView;
    sead::RenderMatrix44 mViewProjection;
    geometry::Bounds3 mFirstBounds[3], mSecondBounds[3], mCombinedBounds;
    be<s32> mPlaneCount;
    geometry::Plane mPlanes[24];
    be<u8> mCropEnabled;
    u8 mUnobserved5A1[3];
    be<f32> mCropOriginX, mCropOriginY, mCropSpanX, mCropSpanY;
};
WWHD_OFFSET(ShadowContext, mSphereBoundsMethod, 0x60);
WWHD_OFFSET(ShadowContext, mTransformBoundsMethod, 0x68);
WWHD_OFFSET(ShadowContext, mMatrixMode, 0x70);
WWHD_OFFSET(ShadowContext, mWorkspace, 0x84);
WWHD_OFFSET(ShadowContext, mTransform, 0x134);
WWHD_OFFSET(ShadowContext, mProjection, 0x164);
WWHD_OFFSET(ShadowContext, mFirstAffine, 0x1E4);
WWHD_OFFSET(ShadowContext, mFirstBounds, 0x374);
WWHD_OFFSET(ShadowContext, mPlaneCount, 0x41C);
WWHD_OFFSET(ShadowContext, mCropEnabled, 0x5A0);
WWHD_SIZE(ShadowContext, 0x5B4);
struct ProjectionInputPrefix_l { u8 mUnobserved[0x70]; be<u8> mCropEnabled; };
WWHD_OFFSET(ProjectionInputPrefix_l, mCropEnabled, 0x70);
WWHD_SIZE(ProjectionInputPrefix_l, 0x71);
struct ProjectionDimensions_l { be<s32> mWidth, mHeight; };
WWHD_SIZE(ProjectionDimensions_l, 8);
WWHD_OFFSET(ProjectionDimensions_l, mHeight, 4);
ShadowContext* constructShadowContext(ShadowContext* context);
void refreshShadowPlanes(ShadowContext* context);
void buildPlanes(ShadowContext*, const sead::RenderMatrix34*, const sead::RenderMatrix44*,
                  const sead::RenderVector3* reference, u32 addEdgePlanes, f64 firstDepth, f64 secondDepth);
void collectProjectedBounds(ShadowContext*, s32 width, s32 height, f64 firstLimit, f64 secondLimit);
void updateShadowView(ShadowContext*, const ProjectionInputPrefix_l*, const sead::RenderMatrix34*,
                      const sead::RenderMatrix44*, f64 minimumDepth, f64 maximumDepth, f64 tolerance);
u32 fitProjection(ShadowContext*, sead::RenderMatrix44*, const sead::RenderMatrix44*,
                  const sead::RenderMatrix34*, const sead::RenderMatrix34*, f64, f64);
void configureProjection(ShadowContext*, const sead::RenderMatrix34*, u32 mode,
                         const ProjectionDimensions_l*, f64 firstLimit, f64 secondLimit);
} }
void* aglShadowAllocateContext(u32);
void aglShadowConstructParameterOwner(void*);
void aglShadowConstructMatrixParameter(agl::utility_parameters::MatrixModeParameter*);
void aglShadowBindMatrixParameter(agl::utility_parameters::MatrixModeParameter*, const sead::SafeText*, const sead::SafeText*, const sead::SafeText*, void*);
void aglShadowConstructWorkspace(agl::shadow::GeometryWorkspace_l*);
void aglShadowCompleteMatrix(sead::RenderMatrix44*, const sead::RenderMatrix34*, const agl::shadow::ProjectionBottomRow_l*);

void aglShadowCopyAffine(const sead::RenderMatrix34*, sead::RenderMatrix34*);
void aglShadowCopyProjection(const sead::RenderMatrix44*, sead::RenderMatrix44*);

void aglShadowConcatProjection(const sead::RenderMatrix44*, const sead::RenderMatrix44*, sead::RenderMatrix44*);
