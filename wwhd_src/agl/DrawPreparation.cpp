// Draw preparation leaves reconstructed from the assigned instructions.
#include "agl/DrawPreparation.h"

namespace agl {
// Borrowed read-only startup values; no initializer or storage ownership is introduced.
static sead::RenderVector3 sDrawDefaultPosition;
static sead::ColorChannels sDrawDefaultColor;
static sead::ColorChannels sDrawAlternateColor;

/* 027CC288 */
u32 agl::DrawPreparation_l::intersectsCullVolume(const DrawParameterState_l* value, u32 slot,
                                               const DrawCullVolume_l* volume) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u16(value->mFlags) & 0x20)
        return 0;
    f32 positionY = value->mPosition.y;
    f32 depthY = volume->mTransform.mRows[2][1];
    f32 positionX = value->mPosition.x;
    f32 depthProduct = gabi::fmuls_ppc(depthY, positionY);
    f32 depthX = volume->mTransform.mRows[2][0];
    f32 positionZ = value->mPosition.z;
    f32 depthPartial = gabi::fmadds(depthX, positionX, depthProduct);
    f32 depthZ = volume->mTransform.mRows[2][2];
    f32 nearPlane = volume->mNear;
    f32 depthTranslation = volume->mTransform.mRows[2][3];
    f32 depthSum = gabi::fmadds(depthZ, positionZ, depthPartial);
    f32 radius = value->mScale;
    f32 depth = gabi::fadds_ppc(depthSum, depthTranslation);
    f32 nearLimit = gabi::fsubs_ppc(nearPlane, radius);
    f32 negativeDepth = -depth;
    if (negativeDepth < nearLimit)
        return 0;
    f32 farLimit = gabi::fsubs_ppc(negativeDepth, radius);
    f32 farPlane = volume->mFar;
    if (farPlane < farLimit)
        return 0;
    f32 xY = volume->mTransform.mRows[0][1];
    f32 yY = volume->mTransform.mRows[1][1];
    f32 xProduct = gabi::fmuls_ppc(xY, positionY);
    f32 xX = volume->mTransform.mRows[0][0];
    f32 yProduct = gabi::fmuls_ppc(yY, positionY);
    f32 yX = volume->mTransform.mRows[1][0];
    f32 xPartial = gabi::fmadds(xX, positionX, xProduct);
    f32 xZ = volume->mTransform.mRows[0][2];
    f32 yPartial = gabi::fmadds(yX, positionX, yProduct);
    f32 yZ = volume->mTransform.mRows[1][2];
    f32 xSum = gabi::fmadds(xZ, positionZ, xPartial);
    f32 xTranslation = volume->mTransform.mRows[0][3];
    f32 ySum = gabi::fmadds(yZ, positionZ, yPartial);
    f32 yTranslation = volume->mTransform.mRows[1][3];
    f32 x = gabi::fadds_ppc(xSum, xTranslation);
    f32 y = gabi::fadds_ppc(ySum, yTranslation);
    gabi::FrameLocal<sead::RenderVector2> transformed(8);
    transformed->x = x;
    transformed->y = y;
    for (u32 index = 0; index < 4; ++index) {
        const sead::RenderVector3* plane = &volume->mSidePlanes[index];
        f32 xProduct = gabi::fmuls_ppc(x, plane->x);
        f32 yProduct = gabi::fmuls_ppc(y, plane->y);
        f32 sum = gabi::fadds_ppc(xProduct, yProduct);
        f32 distance = gabi::fmadds(depth, plane->z, sum);
        if (!(distance < radius))
            return 0;
    }
    return 1;
}

/* 027CD0B4 */
u32 agl::DrawPreparation_l::intersectsExtendedCullVolume(const ExtendedDrawCullState_l* value, u32 slot,
                                               const DrawCullVolume_l* volume) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 depthY = volume->mTransform.mRows[2][1];
    f32 positionY = value->mPosition.y;
    f32 positionX = value->mPosition.x;
    f32 depthProduct = gabi::fmuls_ppc(depthY, positionY);
    f32 depthX = volume->mTransform.mRows[2][0];
    f32 positionZ = value->mPosition.z;
    f32 depthPartial = gabi::fmadds(depthX, positionX, depthProduct);
    f32 depthZ = volume->mTransform.mRows[2][2];
    f32 nearPlane = volume->mNear;
    f32 depthTranslation = volume->mTransform.mRows[2][3];
    f32 depthSum = gabi::fmadds(depthZ, positionZ, depthPartial);
    f32 radius = value->mRadius;
    f32 depth = gabi::fadds_ppc(depthSum, depthTranslation);
    f32 nearLimit = gabi::fsubs_ppc(nearPlane, radius);
    f32 negativeDepth = -depth;
    if (negativeDepth < nearLimit)
        return 0;
    f32 farLimit = gabi::fsubs_ppc(negativeDepth, radius);
    f32 farPlane = volume->mFar;
    if (farPlane < farLimit)
        return 0;
    f32 xY = volume->mTransform.mRows[0][1];
    f32 yY = volume->mTransform.mRows[1][1];
    f32 xProduct = gabi::fmuls_ppc(xY, positionY);
    f32 xX = volume->mTransform.mRows[0][0];
    f32 yProduct = gabi::fmuls_ppc(yY, positionY);
    f32 yX = volume->mTransform.mRows[1][0];
    f32 xPartial = gabi::fmadds(xX, positionX, xProduct);
    f32 xZ = volume->mTransform.mRows[0][2];
    f32 yPartial = gabi::fmadds(yX, positionX, yProduct);
    f32 yZ = volume->mTransform.mRows[1][2];
    f32 xSum = gabi::fmadds(xZ, positionZ, xPartial);
    f32 xTranslation = volume->mTransform.mRows[0][3];
    f32 ySum = gabi::fmadds(yZ, positionZ, yPartial);
    f32 yTranslation = volume->mTransform.mRows[1][3];
    f32 x = gabi::fadds_ppc(xSum, xTranslation);
    f32 y = gabi::fadds_ppc(ySum, yTranslation);
    gabi::FrameLocal<sead::RenderVector2> transformed(8);
    transformed->x = x;
    transformed->y = y;
    for (u32 index = 0; index < 4; ++index) {
        const sead::RenderVector3* plane = &volume->mSidePlanes[index];
        f32 xProduct = gabi::fmuls_ppc(x, plane->x);
        f32 yProduct = gabi::fmuls_ppc(y, plane->y);
        f32 sum = gabi::fadds_ppc(xProduct, yProduct);
        f32 distance = gabi::fmadds(depth, plane->z, sum);
        if (!(distance < radius))
            return 0;
    }
    return 1;
}

/* 027CF8DC */
u32 agl::DrawPreparation_l::intersectsSelectedCullVolume(const DrawPair_l* pair, u32 slot,
                                               const DrawCullVolume_l* volume) {
    gabi::AutoFrame frame{gabi::current_function()};
    const RankedDrawItem_l* value = pair->mSelected;
    f32 positionY = value->mPosition.y;
    f32 depthY = volume->mTransform.mRows[2][1];
    f32 positionX = value->mPosition.x;
    f32 depthProduct = gabi::fmuls_ppc(depthY, positionY);
    f32 depthX = volume->mTransform.mRows[2][0];
    f32 positionZ = value->mPosition.z;
    f32 depthPartial = gabi::fmadds(depthX, positionX, depthProduct);
    f32 depthZ = volume->mTransform.mRows[2][2];
    f32 nearPlane = volume->mNear;
    f32 depthTranslation = volume->mTransform.mRows[2][3];
    f32 depthSum = gabi::fmadds(depthZ, positionZ, depthPartial);
    f32 radius = value->mRank;
    f32 depth = gabi::fadds_ppc(depthSum, depthTranslation);
    f32 nearLimit = gabi::fsubs_ppc(nearPlane, radius);
    f32 negativeDepth = -depth;
    if (negativeDepth < nearLimit)
        return 0;
    f32 farLimit = gabi::fsubs_ppc(negativeDepth, radius);
    f32 farPlane = volume->mFar;
    if (farPlane < farLimit)
        return 0;
    f32 xY = volume->mTransform.mRows[0][1];
    f32 yY = volume->mTransform.mRows[1][1];
    f32 xProduct = gabi::fmuls_ppc(xY, positionY);
    f32 xX = volume->mTransform.mRows[0][0];
    f32 yProduct = gabi::fmuls_ppc(yY, positionY);
    f32 yX = volume->mTransform.mRows[1][0];
    f32 xPartial = gabi::fmadds(xX, positionX, xProduct);
    f32 xZ = volume->mTransform.mRows[0][2];
    f32 yPartial = gabi::fmadds(yX, positionX, yProduct);
    f32 yZ = volume->mTransform.mRows[1][2];
    f32 xSum = gabi::fmadds(xZ, positionZ, xPartial);
    f32 xTranslation = volume->mTransform.mRows[0][3];
    f32 ySum = gabi::fmadds(yZ, positionZ, yPartial);
    f32 yTranslation = volume->mTransform.mRows[1][3];
    f32 x = gabi::fadds_ppc(xSum, xTranslation);
    f32 y = gabi::fadds_ppc(ySum, yTranslation);
    gabi::FrameLocal<sead::RenderVector2> transformed(8);
    transformed->x = x;
    transformed->y = y;
    for (u32 index = 0; index < 4; ++index) {
        const sead::RenderVector3* plane = &volume->mSidePlanes[index];
        f32 xProduct = gabi::fmuls_ppc(x, plane->x);
        f32 yProduct = gabi::fmuls_ppc(y, plane->y);
        f32 sum = gabi::fadds_ppc(xProduct, yProduct);
        f32 distance = gabi::fmadds(depth, plane->z, sum);
        if (!(distance < radius))
            return 0;
    }
    return 1;
}

/* 027CEA18 */
u32 agl::DrawPreparation_l::intersectsBoxCullVolume(const LargeDrawCullState_l* value, u32 slot,
                                                  const DrawCullVolume_l* volume) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 maximumX = volume->mBounds.mMaximum.x;
    f32 minimumX = value->mBounds.mMinimum.x;
    if (maximumX < minimumX)
        return 0;
    f32 boxMaximumX = value->mBounds.mMaximum.x;
    f32 volumeMinimumX = volume->mBounds.mMinimum.x;
    if (boxMaximumX < volumeMinimumX)
        return 0;
    f32 minimumY = value->mBounds.mMinimum.y;
    f32 maximumY = volume->mBounds.mMaximum.y;
    if (maximumY < minimumY)
        return 0;
    f32 boxMaximumY = value->mBounds.mMaximum.y;
    f32 volumeMinimumY = volume->mBounds.mMinimum.y;
    if (boxMaximumY < volumeMinimumY)
        return 0;
    f32 minimumZ = value->mBounds.mMinimum.z;
    f32 maximumZ = volume->mBounds.mMaximum.z;
    if (maximumZ < minimumZ)
        return 0;
    f32 boxMaximumZ = value->mBounds.mMaximum.z;
    f32 volumeMinimumZ = volume->mBounds.mMinimum.z;
    if (boxMaximumZ < volumeMinimumZ)
        return 0;
    gabi::FrameLocal<sead::RenderVector3> support(8);
    for (u32 index = 0; index < 6; ++index) {
        const sead::RenderVector3* plane = &volume->mWorldPlanes[index];
        f32 normalX = plane->x;
        f32 x = -normalX >= 0.0f ? boxMaximumX : minimumX;
        support->x = x;
        f32 normalY = plane->y;
        f32 y = -normalY >= 0.0f ? boxMaximumY : minimumY;
        support->y = y;
        f32 normalZ = plane->z;
        f32 z = -normalZ >= 0.0f ? boxMaximumZ : minimumZ;
        support->z = z;
        f32 xProduct = gabi::fmuls_ppc(x, plane->x);
        f32 yProduct = gabi::fmuls_ppc(y, plane->y);
        f32 sum = gabi::fadds_ppc(xProduct, yProduct);
        f32 dot = gabi::fmadds(z, plane->z, sum);
        f32 offset = volume->mPlaneOffsets[index];
        f32 distance = gabi::fadds_ppc(offset, dot);
        if (distance > 0.0f)
            return 0;
        if (index != 5) {
            minimumX = value->mBounds.mMinimum.x;
            minimumZ = value->mBounds.mMinimum.z;
            boxMaximumY = value->mBounds.mMaximum.y;
            boxMaximumX = value->mBounds.mMaximum.x;
            boxMaximumZ = value->mBounds.mMaximum.z;
            minimumY = value->mBounds.mMinimum.y;
        }
    }
    return 1;
}

/* 027CC1BC */
void agl::DrawPreparation_l::initializeParameters(DrawParameterState_l* value) {
    value->mFlags = 3;
    value->mPosition.x = f32(sDrawDefaultPosition.x);
    f32 y = sDrawDefaultPosition.y;
    value->mPosition.y = y;
    f32 z = sDrawDefaultPosition.z;
    value->mScale = 1.0f;
    value->mPosition.z = z;
    value->mColor.r.bits = u32(sDrawDefaultColor.r.bits);
    value->mColor.g.bits = u32(sDrawDefaultColor.g.bits);
    value->mColor.b.bits = u32(sDrawDefaultColor.b.bits);
    value->mColor.a.bits = u32(sDrawDefaultColor.a.bits);
    value->mAlternateColor.r.bits = u32(sDrawAlternateColor.r.bits);
    value->mAlternateColor.g.bits = u32(sDrawAlternateColor.g.bits);
    u32 blue = sDrawAlternateColor.b.bits;
    value->mAlternateColor.b.bits = blue;
    u32 alpha = sDrawAlternateColor.a.bits;
    value->mPositiveValue = 2.0f;
    value->mAlternateColor.a.bits = alpha;
    value->mThreshold = 0.1f;
}

/* 027CC264 */
void agl::DrawPreparation_l::ensurePositiveValue(PositiveDrawValue_l* value) {
    if (!(f32(value->mPositiveValue) > 0.0f))
        value->mPositiveValue = 0.001f;
}

/* 027CC750 */
u32 isZeroDrawVector(const sead::RenderVector3* vector) {
    return f32(vector->x) == 0.0f && f32(vector->y) == 0.0f && f32(vector->z) == 0.0f;
}

/* 027CF888 */
void agl::DrawPreparation_l::resetPair(DrawPair_l* pair) {
    pair->mEnabled = 1;
    pair->mFirst = nullptr;
    pair->mSecond = nullptr;
}

/* 027CF8B4 */
void agl::DrawPreparation_l::selectHigherRank(DrawPair_l* pair) {
    RankedDrawItem_l* first = pair->mFirst;
    RankedDrawItem_l* second = pair->mSecond;
    f32 firstRank = first->mRank;
    f32 secondRank = second->mRank;
    if (firstRank > secondRank)
        pair->mSelected = first;
    else
        pair->mSelected = second;
}
}

/* WWHD glue
data sDrawDefaultPosition 104A0928
data sDrawDefaultColor 104A01EC
data sDrawAlternateColor 104A01CC
*/
