// Perspective angle state and constructors reconstructed from the WWHD binary.
#include "sead/PerspectiveState.h"
namespace sead {}
using namespace sead;
static u8 sPerspectiveVtable;
struct ProjectionShift { be<u32> x,y; };
WWHD_SIZE(ProjectionShift,8);
static ProjectionShift sDefaultProjectionShift;
/* 0274DE2C */
void sead::PerspectiveState::setAngle(f32 angle) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 halfAngle=gabi::fmuls_ppc(angle,0.5f);
    mAngle.value=angle;
    mSinHalf.value=static_cast<f32>(seadProjectionSine(halfAngle));
    mCosHalf.value=static_cast<f32>(seadProjectionCosine(halfAngle));
    mTanHalf.value=static_cast<f32>(seadProjectionTangent(halfAngle));
    gabi::at<ProjectionState>(gabi::ea(this))->mProjectionDirty=1;
}
/* 0274DEA8 */
sead::PerspectiveState* sead::createPerspective(PerspectiveState* state) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<PerspectiveState*>(seadAllocateViewStorage(sizeof(PerspectiveState)));
    if(!state) return nullptr;
    auto* base=gabi::at<ProjectionState>(gabi::ea(state));
    createProjectionState(base);
    state->mNear.value=1.0f;
    base->mVtable=&sPerspectiveVtable;
    state->mFar.value=10000.0f;
    state->mAspect.value=1.3333333730697632f;
    state->mShiftX.bits=sDefaultProjectionShift.x;
    state->mShiftY.bits=sDefaultProjectionShift.y;
    state->setAngle(0.7853981852531433f);
    base->mProjectionDirty=1;
    return state;
}
/* 0274DF4C */
sead::PerspectiveState* sead::createPerspectiveParameters(PerspectiveState* state,f32 nearDistance,f32 farDistance,f32 angle,f32 aspect) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<PerspectiveState*>(seadAllocateViewStorage(sizeof(PerspectiveState)));
    if(!state) return nullptr;
    auto* base=gabi::at<ProjectionState>(gabi::ea(state));
    createProjectionState(base);
    state->mNear.value=nearDistance;
    state->mFar.value=farDistance;
    base->mVtable=&sPerspectiveVtable;
    state->mAspect.value=aspect;
    state->mShiftX.bits=sDefaultProjectionShift.x;
    state->mShiftY.bits=sDefaultProjectionShift.y;
    state->setAngle(angle);
    base->mProjectionDirty=1;
    return state;
}
/* 0274E04C */
void sead::PerspectiveState::setParameters(f32 nearDistance,f32 farDistance,f32 angle,f32 aspect) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    mNear.value=nearDistance;
    mFar.value=farDistance;
    gabi::at<ProjectionState>(gabi::ea(this))->mProjectionDirty=1;
    setAngle(angle);
    gabi::at<ProjectionState>(gabi::ea(this))->mProjectionDirty=1;
    mAspect.value=aspect;
}
/* 0274E0B8 */
void sead::PerspectiveState::buildMatrix(ProjectionMatrix* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 nearDistance=mNear.value;
    f32 tangent=mTanHalf.value;
    f32 doubleNear=gabi::fadds_ppc(nearDistance,nearDistance);
    f32 aspect=mAspect.value;
    f32 height=gabi::fmuls_ppc(doubleNear,tangent);
    f32 width=gabi::fmuls_ppc(height,aspect);
    f32 shiftX=mShiftX.value;
    f32 halfHeight=gabi::fmuls_ppc(height,0.5f);
    f32 centerX=gabi::fmuls_ppc(width,shiftX);
    f32 shiftY=mShiftY.value;
    f32 halfWidth=gabi::fmuls_ppc(width,0.5f);
    f32 centerY=gabi::fmuls_ppc(height,shiftY);
    f32 left=gabi::fsubs_ppc(centerX,halfWidth);
    f32 right=gabi::fadds_ppc(halfWidth,centerX);
    f32 bottom=gabi::fsubs_ppc(centerY,halfHeight);
    f32 top=gabi::fadds_ppc(halfHeight,centerY);
    f32 spanX=gabi::fsubs_ppc(right,left);
    f32 spanY=gabi::fsubs_ppc(top,bottom);
    f32 inverseX=gabi::fdivs_ppc(1.0f,spanX);
    f32 sumX=gabi::fadds_ppc(right,left);
    result->mRows[0][1].value=0.0f;
    f32 scaleX=gabi::fmuls_ppc(doubleNear,inverseX);
    result->mRows[1][0].value=0.0f;
    f32 offsetX=gabi::fmuls_ppc(sumX,inverseX);
    result->mRows[0][0].value=scaleX;
    f32 sumY=gabi::fadds_ppc(top,bottom);
    result->mRows[0][3].value=0.0f;
    f32 inverseY=gabi::fdivs_ppc(1.0f,spanY);
    result->mRows[0][2].value=offsetX;
    nearDistance=mNear.value;
    doubleNear=gabi::fadds_ppc(nearDistance,nearDistance);
    f32 offsetY=gabi::fmuls_ppc(sumY,inverseY);
    result->mRows[1][3].value=0.0f;
    f32 scaleY=gabi::fmuls_ppc(doubleNear,inverseY);
    result->mRows[1][2].value=offsetY;
    result->mRows[1][1].value=scaleY;
    f32 farDistance=mFar.value;
    nearDistance=mNear.value;
    result->mRows[2][0].value=0.0f;
    f32 spanZ=gabi::fsubs_ppc(farDistance,nearDistance);
    result->mRows[2][1].value=0.0f;
    f32 inverseZ=gabi::fdivs_ppc(1.0f,spanZ);
    nearDistance=mNear.value;
    farDistance=mFar.value;
    f32 sumZ=gabi::fadds_ppc(farDistance,nearDistance);
    f32 scaleZ=gabi::fmuls_ppc(sumZ,inverseZ);
    result->mRows[2][2].value=-scaleZ;
    farDistance=mFar.value;
    nearDistance=mNear.value;
    f32 doubleFar=gabi::fadds_ppc(farDistance,farDistance);
    result->mRows[3][1].value=0.0f;
    f32 depthProduct=gabi::fmuls_ppc(doubleFar,nearDistance);
    result->mRows[3][2].value=-1.0f;
    f32 offsetZ=gabi::fmuls_ppc(depthProduct,inverseZ);
    result->mRows[3][0].value=0.0f;
    result->mRows[3][3].value=0.0f;
    result->mRows[2][3].value=-offsetZ;
}

/* WWHD glue
data sPerspectiveVtable 10145260
data sDefaultProjectionShift 104A0908
ext 028F43F8 f64 seadProjectionSine(f64)
ext 028F4BE0 f64 seadProjectionCosine(f64)
ext 028F423C f64 seadProjectionTangent(f64)
*/
