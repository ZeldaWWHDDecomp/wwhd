// View transforms and blend-channel defaults reconstructed from the WWHD binary.
#include "sead/ViewTransform.h"
#include "sead/ProjectionState.h"
namespace sead {}
using namespace sead;
static u8 sViewCameraVtable;
static u8 sLookAtCameraVtable;
static u8 sProjectionCameraVtable;
static ViewTransform sDefaultCameraTransform;
/* 0274CA0C */
void sead::destroyBlendChannels(BlendState* state, u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (state && (flags & 1)) seadReleaseViewStorage(state);
}
/* 0274CA20 */
BlendState* sead::createBlendChannels(BlendState* state) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!state) state = static_cast<BlendState*>(seadAllocateViewStorage(sizeof(BlendState)));
    if (!state) return nullptr;
    state->mColorSource = 4;
    state->mColorEquation = 0;
    state->mColorDestination = 5;
    state->mAlphaSource = 4;
    state->mAlphaDestination = 5;
    state->mAlphaEquation = 0;
    return state;
}
/* 0274CA78 */
void sead::blendChannelsChanged() {
    gabi::AutoFrame originalFrame{gabi::current_function()};}
/* 0274CA7C */
void sead::ViewTransform::inverseTranslation(ViewVector* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 rightX = mRows[0][0].value;
    f32 translationX = mRows[0][3].value;
    f32 rightY = mRows[0][1].value;
    f32 productX = gabi::fmuls_ppc(rightX, translationX);
    f32 translationY = mRows[1][3].value;
    f32 negativeX = -productX;
    f32 upX = mRows[1][0].value;
    f32 productY = gabi::fmuls_ppc(rightY, translationX);
    f32 rightZ = mRows[0][2].value;
    f32 sumX = gabi::fnmsubs(upX, translationY, negativeX);
    f32 productZ = gabi::fmuls_ppc(rightZ, translationX);
    f32 forwardX = mRows[2][0].value;
    f32 negativeY = -productY;
    f32 translationZ = mRows[2][3].value;
    f32 negativeZ = -productZ;
    f32 upY = mRows[1][1].value;
    sumX = gabi::fnmsubs(forwardX, translationZ, sumX);
    f32 upZ = mRows[1][2].value;
    f32 sumY = gabi::fnmsubs(upY, translationY, negativeY);
    f32 forwardY = mRows[2][1].value;
    f32 sumZ = gabi::fnmsubs(upZ, translationY, negativeZ);
    f32 forwardZ = mRows[2][2].value;
    sumY = gabi::fnmsubs(forwardY, translationZ, sumY);
    result->x.value = sumX;
    sumZ = gabi::fnmsubs(forwardZ, translationZ, sumZ);
    result->y.value = sumY;
    result->z.value = sumZ;
}
/* 0274CAEC */
void sead::ViewTransform::copyForward(ViewVector* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 y = mRows[2][1].bits;
    u32 x = mRows[2][0].bits;
    u32 z = mRows[2][2].bits;
    result->x.bits = x; result->y.bits = y; result->z.bits = z;
}
/* 0274CB08 */
void sead::ViewTransform::copyRight(ViewVector* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 y = mRows[0][1].bits;
    u32 x = mRows[0][0].bits;
    u32 z = mRows[0][2].bits;
    result->x.bits = x; result->y.bits = y; result->z.bits = z;
}
/* 0274CB24 */
void sead::ViewTransform::copyUp(ViewVector* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 y = mRows[1][1].bits;
    u32 x = mRows[1][0].bits;
    u32 z = mRows[1][2].bits;
    result->x.bits = x; result->y.bits = y; result->z.bits = z;
}
/* 0274CB40 */
void sead::ViewTransform::transformPoint(ViewVector* result, const ViewVector* point) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    seadMultiplyViewPoint(this, point, result);
}
/* 0274CCB0 */
void sead::destroyViewTransform(ViewTransform* view, u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (view && (flags & 1)) seadReleaseViewStorage(view);
}
/* 0274CF04 */
void sead::destroyLookAtTransform(ViewTransform* view, u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (view && (flags & 1)) seadReleaseViewStorage(view);
}
/* 0274CF18 */
void sead::destroyOrthographicView(ViewTransform* view, u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (view && (flags & 1)) seadReleaseViewStorage(view);
}
/* 0274D114 */
void sead::copyStoredView(const ViewTransform* view, ViewTransform* result) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    const auto* stored = gabi::at<ViewTransform>(gabi::ea(view) + 0x34);
    seadCopyViewMatrix(stored, result);
}
/* 0274B3C4 */
void* sead::allocateGraphicsFrame(u32 size,s32 alignment) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    void* heap=seadBaseFrameHeap(0);
    return seadFrameHeapAllocate(heap,size,alignment);
}
/* 0274CBAC */
sead::ViewCamera* sead::createLookAtCamera(ViewCamera* camera,const ViewVector* eye,const ViewVector* target,const ViewVector* up) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!camera) camera=static_cast<ViewCamera*>(seadAllocateViewStorage(sizeof(ViewCamera)));
    if(!camera) return nullptr;
    camera->mVtable=&sViewCameraVtable;
    for(u32 row=0;row<3;++row) {
        for(u32 column=0;column<4;++column) {
            u32 bits=sDefaultCameraTransform.mRows[row][column].bits;
            camera->mTransform.mRows[row][column].bits=bits;
        }
    }
    camera->mVtable=&sLookAtCameraVtable;
    camera->mEye.x.bits=eye->x.bits;
    camera->mEye.y.bits=eye->y.bits;
    camera->mEye.z.bits=eye->z.bits;
    camera->mTarget.x.bits=target->x.bits;
    camera->mTarget.y.bits=target->y.bits;
    camera->mTarget.z.bits=target->z.bits;
    camera->mUp.x.bits=up->x.bits;
    camera->mUp.y.bits=up->y.bits;
    camera->mUp.z.bits=up->z.bits;
    seadNormalizeViewVector(&camera->mUp);
    return camera;
}
/* 0274CE34 */
sead::ViewCamera* sead::createProjectionCamera(ViewCamera* camera,const ProjectionState* projection) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!camera) camera=static_cast<ViewCamera*>(seadAllocateViewStorage(sizeof(ViewCamera)));
    if(!camera) return nullptr;
    gabi::FrameLocal<ViewVector> eye(8);
    gabi::FrameLocal<ViewVector> target(0x14);
    gabi::FrameLocal<ViewVector> up(0x20);
    f32 right=projection->mRight.value;
    f32 left=projection->mLeft.value;
    f32 centerX=gabi::fadds_ppc(left,right);
    f32 top=projection->mTop.value;
    f32 bottom=projection->mBottom.value;
    centerX=gabi::fmuls_ppc(centerX,0.5f);
    f32 nearDistance=projection->mNear.value;
    f32 centerY=gabi::fadds_ppc(top,bottom);
    eye->x.value=centerX;
    up->y.value=1.0f;
    centerY=gabi::fmuls_ppc(centerY,0.5f);
    target->x.value=centerX;
    eye->z.value=nearDistance;
    f32 targetZ=gabi::fsubs_ppc(nearDistance,1.0f);
    eye->y.value=centerY;
    target->y.value=centerY;
    up->z.value=0.0f;
    up->x.value=0.0f;
    target->z.value=targetZ;
    createLookAtCamera(camera,eye.get(),target.get(),up.get());
    camera->mVtable=&sProjectionCameraVtable;
    return camera;
}
/* 0274CCC4 */
void sead::ViewCamera::buildView(ViewTransform* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(mEye.x.value==mTarget.x.value && mEye.y.value==mTarget.y.value && mEye.z.value==mTarget.z.value) return;
    gabi::FrameLocal<ViewVector> direction(8);
    gabi::FrameLocal<ViewVector> side(0x14);
    gabi::FrameLocal<ViewVector> up(0x20);
    u32 eyeZBits=mEye.z.bits;
    u32 eyeXBits=mEye.x.bits;
    direction->z.bits=eyeZBits;
    f32 eyeZ=direction->z.value;
    direction->x.bits=eyeXBits;
    u32 eyeYBits=mEye.y.bits;
    f32 targetZ=mTarget.z.value;
    direction->y.bits=eyeYBits;
    f32 eyeX=direction->x.value;
    f32 eyeY=direction->y.value;
    f32 deltaZ=gabi::fsubs_ppc(eyeZ,targetZ);
    f32 targetX=mTarget.x.value;
    f32 targetY=mTarget.y.value;
    f32 deltaX=gabi::fsubs_ppc(eyeX,targetX);
    f32 deltaY=gabi::fsubs_ppc(eyeY,targetY);
    direction->z.value=deltaZ;
    direction->x.value=deltaX;
    direction->y.value=deltaY;
    seadNormalizeViewVector(direction.get());
    seadCrossViewVectors(&mUp,direction.get(),side.get());
    seadNormalizeViewVector(side.get());
    seadCrossViewVectors(direction.get(),side.get(),up.get());
    f32 upX=up->x.value;
    f32 upY=up->y.value;
    eyeX=mEye.x.value;
    eyeY=mEye.y.value;
    f32 directionX=direction->x.value;
    f32 directionY=direction->y.value;
    f32 upProductX=gabi::fmuls_ppc(upX,eyeX);
    f32 upProductY=gabi::fmuls_ppc(upY,eyeY);
    f32 directionProductX=gabi::fmuls_ppc(directionX,eyeX);
    f32 directionProductY=gabi::fmuls_ppc(directionY,eyeY);
    f32 sideX=side->x.value;
    f32 sideY=side->y.value;
    f32 sideProductX=gabi::fmuls_ppc(sideX,eyeX);
    f32 sideProductY=gabi::fmuls_ppc(sideY,eyeY);
    f32 directionDot=gabi::fadds_ppc(directionProductX,directionProductY);
    f32 upDot=gabi::fadds_ppc(upProductX,upProductY);
    eyeZ=mEye.z.value;
    f32 sideDot=gabi::fadds_ppc(sideProductX,sideProductY);
    result->mRows[0][0].value=sideX;
    f32 sideZ=side->z.value;
    f32 directionZ=direction->z.value;
    result->mRows[0][2].value=sideZ;
    upY=up->y.value;
    f32 sideTranslation=gabi::fmadds(sideZ,eyeZ,sideDot);
    directionY=direction->y.value;
    f32 directionTranslation=gabi::fmadds(directionZ,eyeZ,directionDot);
    result->mRows[0][1].value=sideY;
    result->mRows[2][2].value=directionZ;
    result->mRows[2][1].value=directionY;
    f32 negativeSide=-sideTranslation;
    f32 upZ=up->z.value;
    f32 negativeDirection=-directionTranslation;
    upX=up->x.value;
    f32 upTranslation=gabi::fmadds(upZ,eyeZ,upDot);
    result->mRows[1][1].value=upY;
    result->mRows[0][3].value=negativeSide;
    result->mRows[2][3].value=negativeDirection;
    f32 negativeUp=-upTranslation;
    directionX=direction->x.value;
    result->mRows[1][2].value=upZ;
    result->mRows[1][0].value=upX;
    result->mRows[2][0].value=directionX;
    result->mRows[1][3].value=negativeUp;
}

/* WWHD glue
data sViewCameraVtable 101450D0
data sLookAtCameraVtable 101450F8
data sProjectionCameraVtable 10145120
data sDefaultCameraTransform 104A041C
ext 028E8D4C void seadCrossViewVectors(const sead::ViewVector*,const sead::ViewVector*,sead::ViewVector*)
ext 025155D8 void seadNormalizeViewVector(sead::ViewVector*)
ext C0009790 void* seadBaseFrameHeap(s32)
ext C00096A0 void* seadFrameHeapAllocate(void*,u32,s32)

ext 0273AD10 void* seadAllocateViewStorage(u32)
ext 0273AF40 void seadReleaseViewStorage(void*)
ext 028E8F64 void seadMultiplyViewPoint(const sead::ViewTransform*, const sead::ViewVector*, sead::ViewVector*)
ext 028E90D4 void seadCopyViewMatrix(const sead::ViewTransform*, sead::ViewTransform*)
*/
