// View basis construction with the game's two fallback axes and float operation order.
#include "agl/EnvironmentObject.h"
#include <atomic>
#include <cmath>
using namespace agl::env;
static f32 sViewMinimumLength;
static f32 sViewParallelLimit;
static sead::RenderVector3 sDefaultViewDirection;
static sead::RenderVector3 sFirstFallbackAxis;
static sead::RenderVector3 sSecondFallbackAxis;
f32 normalizeProjectorVector(sead::RenderVector3*);
void crossProjectorVectors(const sead::RenderVector3*,const sead::RenderVector3*,sead::RenderVector3*);

/* inlined into Projector::updateView */
static f32 projectorDot(const sead::RenderVector3* left,const sead::RenderVector3* right) {
    f32 x = gabi::fmuls_ppc(left->x,right->x);
    f32 y = gabi::fmuls_ppc(left->y,right->y);
    f32 xy = gabi::fadds_ppc(x,y);
    return gabi::fmadds(left->z,right->z,xy);
}
/* inlined into Projector::updateView */
static void assignProjectorVector(sead::RenderVector3* target,const sead::RenderVector3* source) {
    f32 x = source->x;
    f32 y = source->y;
    target->x = x;
    f32 z = source->z;
    target->y = y;
    target->z = z;
}

/* 0276D324 */
void Projector::updateView() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> up(8),backward(20),right(32);
    backward->x = gabi::fsubs_ppc(mViewPosition.mValue.x,mViewTarget.mValue.x);
    backward->y = gabi::fsubs_ppc(mViewPosition.mValue.y,mViewTarget.mValue.y);
    backward->z = gabi::fsubs_ppc(mViewPosition.mValue.z,mViewTarget.mValue.z);
    f32 directionLength = normalizeProjectorVector(backward.get());
    f32 minimumLength = sViewMinimumLength;
    // The original snapshots these three words before the direction fallback stores.
    u32 upX = reinterpret_cast<const be<u32>&>(mViewUp.mValue.x);
    u32 upZ = reinterpret_cast<const be<u32>&>(mViewUp.mValue.z);
    u32 upY = reinterpret_cast<const be<u32>&>(mViewUp.mValue.y);
    if (directionLength < minimumLength)
        assignProjectorVector(backward.get(),&sDefaultViewDirection);
    reinterpret_cast<be<u32>&>(up->y) = upY;
    reinterpret_cast<be<u32>&>(up->z) = upZ;
    reinterpret_cast<be<u32>&>(up->x) = upX;
    f32 upLength = normalizeProjectorVector(up.get());
    f32 parallelLimit = sViewParallelLimit;
    if (upLength < minimumLength || std::fabs(projectorDot(up.get(),backward.get())) > parallelLimit) {
        gabi::FrameLocal<sead::RenderVector3> firstFallback(80);
        crossProjectorVectors(backward.get(),&sFirstFallbackAxis,firstFallback.get());
        assignProjectorVector(up.get(),firstFallback.get());
    }
    // Repeat the test after the first cross product, retaining the original up length.
    if (upLength < minimumLength || std::fabs(projectorDot(up.get(),backward.get())) > parallelLimit) {
        gabi::FrameLocal<sead::RenderVector3> secondFallback(92);
        crossProjectorVectors(backward.get(),&sSecondFallbackAxis,secondFallback.get());
        assignProjectorVector(up.get(),secondFallback.get());
    }
    crossProjectorVectors(up.get(),backward.get(),right.get());
    normalizeProjectorVector(right.get());
    crossProjectorVectors(backward.get(),right.get(),up.get());

    f32 backwardX = backward->x,backwardY = backward->y,backwardZ = backward->z;
    f32 positionX = mViewPosition.mValue.x,positionY = mViewPosition.mValue.y;
    f32 upXValue = up->x,upYValue = up->y;
    f32 rightX = right->x,rightY = right->y;
    f32 backwardXY = gabi::fadds_ppc(gabi::fmuls_ppc(backwardX,positionX),gabi::fmuls_ppc(backwardY,positionY));
    f32 upXY = gabi::fadds_ppc(gabi::fmuls_ppc(upXValue,positionX),gabi::fmuls_ppc(upYValue,positionY));
    f32 positionZ = mViewPosition.mValue.z;
    f32 rightXY = gabi::fadds_ppc(gabi::fmuls_ppc(rightX,positionX),gabi::fmuls_ppc(rightY,positionY));
    f32 rightZ = right->z;
    mView.mRows[2][2] = backwardZ;
    f32 upZValue = up->z;
    mView.mRows[0][2] = rightZ;
    f32 rightTranslation = -gabi::fmadds(rightZ,positionZ,rightXY);
    mView.mRows[0][1] = rightY;
    f32 backwardTranslation = -gabi::fmadds(backwardZ,positionZ,backwardXY);
    mView.mRows[2][1] = backwardY;
    mView.mRows[0][3] = rightTranslation;
    f32 upTranslation = -gabi::fmadds(upZValue,positionZ,upXY);
    mView.mRows[1][0] = upXValue;
    mView.mRows[0][0] = rightX;
    mView.mRows[1][2] = upZValue;
    mView.mRows[2][3] = backwardTranslation;
    mView.mRows[2][0] = backwardX;
    mView.mRows[1][3] = upTranslation;
    mView.mRows[1][1] = upYValue;
    std::atomic_thread_fence(std::memory_order_acquire);
}

/* WWHD glue
 data sViewMinimumLength 10160010
 data sViewParallelLimit 10160014
 data sDefaultViewDirection 104A0940
 data sFirstFallbackAxis 104A0934
 data sSecondFallbackAxis 104A094C
 ext 025155D8 f32 normalizeProjectorVector(sead::RenderVector3*)
 ext 028E8D4C void crossProjectorVectors(const sead::RenderVector3*,const sead::RenderVector3*,sead::RenderVector3*)
*/
