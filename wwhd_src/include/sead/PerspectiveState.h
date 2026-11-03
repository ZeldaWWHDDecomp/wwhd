#pragma once
#include "sead/ProjectionState.h"
namespace sead {
struct PerspectiveState {
    u8 mBase[0x94];
    ViewScalar mNear,mFar,mAngle,mSinHalf,mCosHalf,mTanHalf,mAspect,mShiftX,mShiftY;
    void setAngle(f32);
    void setParameters(f32,f32,f32,f32);
    void buildMatrix(ProjectionMatrix*) const;
};
WWHD_SIZE(PerspectiveState,0xB8);
WWHD_OFFSET(PerspectiveState,mNear,0x94);
WWHD_OFFSET(PerspectiveState,mAngle,0x9C);
WWHD_OFFSET(PerspectiveState,mAspect,0xAC);
WWHD_OFFSET(PerspectiveState,mShiftX,0xB0);
PerspectiveState* createPerspective(PerspectiveState*);
PerspectiveState* createPerspectiveParameters(PerspectiveState*,f32,f32,f32,f32);
}
f64 seadProjectionSine(f64);
f64 seadProjectionCosine(f64);
f64 seadProjectionTangent(f64);
