/* Projection parameter extraction from the utility TU. Layouts are the merged
 * SEAD declarations; identity nodes below are borrowed views, not new owners. */
#include "sead/PerspectiveState.h"
#include <cmath>

struct UtilityProjectionTypeDispatch_l {
    u8 mPrefix[12];
    be<u32> mMatchesType;
};
WWHD_OFFSET(UtilityProjectionTypeDispatch_l, mMatchesType, 12);
WWHD_SIZE(UtilityProjectionTypeDispatch_l, 16);
struct UtilityProjectionOffset_l { sead::ViewScalar x, y; };
WWHD_SIZE(UtilityProjectionOffset_l, 8);

static u32 sBorrowedPerspectiveGuard;
static void* sBorrowedPerspectiveType;
static u32 sBorrowedFrustumGuard;
static void* sBorrowedFrustumType;
static u32 sBorrowedOrthographicGuard;
static void* sBorrowedOrthographicType;
static u32 sBorrowedProjectionTypeMethods;
static f32 sProjectionZero;
static f32 sProjectionDefaultAspect;
static UtilityProjectionOffset_l sProjectionDefaultOffset;

namespace agl { namespace utility_projection {
static u32 matchesType(sead::ProjectionState* projection, const void* type) {
    auto* dispatch = static_cast<UtilityProjectionTypeDispatch_l*>(
        static_cast<void*>(projection->mVtable));
    return gabi::call<u32>(dispatch->mMatchesType, projection, type);
}

/* 027C4C5C */
bool extractParameters(sead::ProjectionState* projection, sead::ViewScalar* nearPlane,
                       sead::ViewScalar* farPlane, sead::ViewScalar* aspect,
                       sead::ViewScalar* angle, sead::ViewScalar* offset) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sBorrowedPerspectiveGuard == 0) {
        sBorrowedPerspectiveGuard = 1;
        sBorrowedPerspectiveType = &sBorrowedProjectionTypeMethods;
    }
    if (projection && matchesType(projection, &sBorrowedPerspectiveType)) {
        if (sBorrowedPerspectiveGuard == 0) {
            sBorrowedPerspectiveType = &sBorrowedProjectionTypeMethods;
            sBorrowedPerspectiveGuard = 1;
        }
        if (!matchesType(projection, &sBorrowedPerspectiveType)) projection = nullptr;
        auto* perspective = reinterpret_cast<sead::PerspectiveState*>(projection);
        if (nearPlane) nearPlane->bits = perspective->mNear.bits;
        if (farPlane) farPlane->bits = perspective->mFar.bits;
        if (aspect) aspect->bits = perspective->mAspect.bits;
        if (angle) angle->bits = perspective->mAngle.bits;
        if (offset) {
            offset[0].bits = perspective->mShiftX.bits;
            offset[1].bits = perspective->mShiftY.bits;
        }
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return true;
    }

    if (sBorrowedFrustumGuard == 0) {
        sBorrowedFrustumGuard = 1;
        sBorrowedFrustumType = &sBorrowedProjectionTypeMethods;
    }
    if (projection && matchesType(projection, &sBorrowedFrustumType)) {
        if (sBorrowedFrustumGuard == 0) {
            sBorrowedFrustumType = &sBorrowedProjectionTypeMethods;
            sBorrowedFrustumGuard = 1;
        }
        if (!matchesType(projection, &sBorrowedFrustumType)) projection = nullptr;
        if (nearPlane) nearPlane->bits = projection->mNear.bits;
        if (farPlane) farPlane->bits = projection->mFar.bits;
        if (aspect) aspect->value = projection->aspectRatio();
        if (angle) angle->value = projection->verticalAngle();
        if (offset) projection->offsets(offset);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return true;
    }

    if (sBorrowedOrthographicGuard == 0) {
        sBorrowedOrthographicGuard = 1;
        sBorrowedOrthographicType = &sBorrowedProjectionTypeMethods;
    }
    f32 zero = sProjectionZero;
    f32 defaultAspect = sProjectionDefaultAspect;
    if (projection && matchesType(projection, &sBorrowedOrthographicType)) {
        if (sBorrowedOrthographicGuard == 0) {
            sBorrowedOrthographicType = &sBorrowedProjectionTypeMethods;
            sBorrowedOrthographicGuard = 1;
        }
        if (!matchesType(projection, &sBorrowedOrthographicType)) projection = nullptr;
        if (nearPlane) nearPlane->bits = projection->mNear.bits;
        if (farPlane) farPlane->bits = projection->mFar.bits;
        if (aspect) {
            f32 bottom = projection->mBottom.value;
            f32 top = projection->mTop.value;
            f32 height = gabi::fsubs_ppc(top, bottom);
            f32 left = projection->mLeft.value;
            f32 right = projection->mRight.value;
            f32 absHeight = std::fabs(height);
            f32 width = gabi::fsubs_ppc(left, right);
            f32 absWidth = std::fabs(width);
            if (zero < absHeight)
                aspect->value = std::fabs(gabi::fdivs_ppc(absWidth, absHeight));
            else
                aspect->value = defaultAspect;
        }
        if (angle) angle->value = zero;
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        return false;
    }
    if (nearPlane) nearPlane->value = zero;
    if (farPlane) farPlane->value = zero;
    if (aspect) aspect->value = defaultAspect;
    if (angle) angle->value = zero;
    if (offset) {
        offset[0].bits = sProjectionDefaultOffset.x.bits;
        offset[1].bits = sProjectionDefaultOffset.y.bits;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    return false;
}
} }

/* WWHD glue
data sBorrowedPerspectiveGuard 101FD6A0
data sBorrowedPerspectiveType 101FDCDC
data sBorrowedFrustumGuard 101FD6A8
data sBorrowedFrustumType 101FDF84
data sBorrowedOrthographicGuard 101FD6A4
data sBorrowedOrthographicType 101FDF80
data sBorrowedProjectionTypeMethods 1016A350
data sProjectionZero 1016A360
data sProjectionDefaultAspect 1016A364
data sProjectionDefaultOffset 104A0908
ext 0274E7E0 f32 sead::ProjectionState::aspectRatio() const
ext 0274E7A0 f32 sead::ProjectionState::verticalAngle() const
ext 0274E848 void sead::ProjectionState::offsets(sead::ViewScalar*) const
*/
