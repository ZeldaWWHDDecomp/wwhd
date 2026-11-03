/* AGL geometry helpers; canonical matrix layout, own assigned operation order. */
#pragma once
#include "sead/RenderMath.h"
namespace agl { namespace geometry {
struct Bounds3 {
    sead::RenderVector3 mMinimum;
    sead::RenderVector3 mMaximum;
};
struct Sphere {
    sead::RenderVector3 mCenter;
    be<f32> mRadius;
};
WWHD_SIZE(Sphere, 16);
WWHD_OFFSET(Sphere, mRadius, 12);
struct BoundsTransformState_l {
    u8 mUnobservedState[0x114];
    be<s32> mMinimumCorner[3];
    be<u32> mUnobservedMinimumTail;
    be<s32> mMaximumCorner[3];
    be<u32> mUnobservedMaximumTail;
    sead::RenderMatrix34 mTransform;
};
WWHD_OFFSET(BoundsTransformState_l, mMinimumCorner, 0x114);
WWHD_OFFSET(BoundsTransformState_l, mMaximumCorner, 0x124);
WWHD_OFFSET(BoundsTransformState_l, mTransform, 0x134);
WWHD_SIZE(BoundsTransformState_l, 0x164);
struct BoundsProjectionState_l {
    BoundsTransformState_l mBoundsTransform;
    sead::RenderMatrix44 mProjection;
};
WWHD_OFFSET(BoundsProjectionState_l, mProjection, 0x164);
WWHD_SIZE(BoundsProjectionState_l, 0x1A4);
void projectBounds(const BoundsProjectionState_l* context, Bounds3* destination, const Bounds3* bounds);
void projectSphereBounds(void* context, Bounds3* destination, const Sphere* sphere);
void initializeBoundsCornerSelectors(BoundsTransformState_l* context);
void transformBounds(const BoundsTransformState_l* context, Bounds3* destination, const Bounds3* bounds);
void sphereBounds(void* context, Bounds3* bounds, const Sphere* sphere);
void transformSphereBounds(void* context, Bounds3* destination, const Sphere* sphere);
struct Plane {
    sead::RenderVector3 mNormal;
    be<f32> mDistance;
};
WWHD_SIZE(Bounds3, 24);
WWHD_OFFSET(Bounds3, mMaximum, 12);
WWHD_SIZE(Plane, 16);
WWHD_OFFSET(Plane, mDistance, 12);
Bounds3* constructEmptyBounds(Bounds3* object);
Plane* constructDefaultPlane(Plane* object);
void* allocateOrReuseStorage(void* object);
void destroyStorage(void* object, u32 flags);
void emptyStorageHook(void* object);
f32 quantizeDown(f64 value, f64 subdivisions);
void copyTransposed(sead::RenderMatrix44* destination, const sead::RenderMatrix44* source);
void clearMatrix(sead::RenderMatrix44* matrix);
void initializeClipSpaceTransforms();
const sead::RenderMatrix44* clipTextureTransform();
const sead::RenderMatrix44* verticalDepthExchange();
} }
void* allocateGeometryStorage(u32);
void aglGeometryMultiplyVector(const sead::RenderMatrix34*, const sead::RenderVector3*, sead::RenderVector3*);
void aglGeometryCombineProjection(sead::RenderMatrix44*, const sead::RenderMatrix44*, const sead::RenderMatrix34*);
