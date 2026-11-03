// Projection bounds and derived ratios reconstructed from the WWHD binary.
#include "sead/ProjectionState.h"
namespace sead {}
using namespace sead;
static u8 sProjectionVtable;
static u8 sOrthographicVtable;
static u8 sFrustumVtable;
static u8 sStoredProjectionVtable;
static ProjectionMatrix sDefaultStoredProjection;
static s32 sDefaultDevicePosture;
static f32 sDefaultDepthScale=1.0f;
static f32 sDefaultDepthOffset=0.0f;
/* 0274D6A0 */
void sead::destroyProjectionState(ProjectionState* state,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(state && (flags&1)) seadReleaseViewStorage(state);
}
/* 0274E038 */
void sead::destroyPerspectiveProjection(ProjectionState* state,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(state && (flags&1)) seadReleaseViewStorage(state);
}
/* 0274E474 */
void sead::destroyOrthographicProjection(ProjectionState* state,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(state && (flags&1)) seadReleaseViewStorage(state);
}
/* 0274E69C */
void sead::destroyFrustumProjection(ProjectionState* state,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(state && (flags&1)) seadReleaseViewStorage(state);
}
/* 0274EB7C */
void sead::destroyStoredProjection(ProjectionState* state,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(state && (flags&1)) seadReleaseViewStorage(state);
}
/* 0274E488 */
void sead::ProjectionState::setBounds(f32 top,f32 bottom,f32 left,f32 right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    mTop.value=top;
    mRight.value=right;
    mProjectionDirty=1;
    mLeft.value=left;
    mBottom.value=bottom;
}
/* 0274E4A4 */
void sead::ProjectionState::setBoundsFromVector(const ViewScalar* bounds) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 left=bounds[0].value;
    f32 bottom=bounds[1].value;
    f32 right=bounds[2].value;
    f32 top=bounds[3].value;
    setBounds(top,bottom,left,right);
}
/* 0274E7A0 */
f32 sead::ProjectionState::verticalAngle() const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 bottom=mBottom.value;
    f32 top=mTop.value;
    f32 height=gabi::fsubs_ppc(top,bottom);
    f32 halfHeight=gabi::fmuls_ppc(height,0.5f);
    f32 nearDistance=mNear.value;
    f32 halfAngle=seadProjectionAtan2(halfHeight,nearDistance);
    return gabi::fadds_ppc(halfAngle,halfAngle);
}
/* 0274E7E0 */
f32 sead::ProjectionState::aspectRatio() const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 left=mLeft.value;
    f32 right=mRight.value;
    f32 bottom=mBottom.value;
    f32 width=gabi::fsubs_ppc(right,left);
    f32 top=mTop.value;
    f32 height=gabi::fsubs_ppc(top,bottom);
    return gabi::fdivs_ppc(width,height);
}
/* 0274E800 */
f32 sead::ProjectionState::horizontalOffset() const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 left=mLeft.value;
    f32 right=mRight.value;
    f32 center=gabi::fadds_ppc(right,left);
    f32 width=gabi::fsubs_ppc(right,left);
    center=gabi::fmuls_ppc(center,0.5f);
    return gabi::fdivs_ppc(center,width);
}
/* 0274E824 */
f32 sead::ProjectionState::verticalOffset() const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 bottom=mBottom.value;
    f32 top=mTop.value;
    f32 center=gabi::fadds_ppc(top,bottom);
    f32 height=gabi::fsubs_ppc(top,bottom);
    center=gabi::fmuls_ppc(center,0.5f);
    return gabi::fdivs_ppc(center,height);
}
/* 0274E848 */
void sead::ProjectionState::offsets(ViewScalar* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 x=horizontalOffset();
    result[0].value=x;
    f32 y=verticalOffset();
    result[1].value=y;
}
/* 0274EC4C */
bool sead::projectionUsesFixedAspect() {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return false; }
/* 0274ECCC */
bool sead::projectionCanReverseDepth() {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return true; }
/* 0274D6B4 */
sead::ProjectionState* sead::createProjectionState(ProjectionState* state) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<ProjectionState*>(seadAllocateViewStorage(0x94));
    if(!state) return nullptr;
    state->mDeviceChanged=1;
    state->mVtable=&sProjectionVtable;
    state->mProjectionDirty=1;
    if(gabi::ea(&state->mProjection)==0) seadAllocateViewStorage(64);
    if(gabi::ea(&state->mDeviceProjection)==0) seadAllocateViewStorage(64);
    state->mDeviceMode=sDefaultDevicePosture;
    state->mDepthScale.value=sDefaultDepthScale;
    state->mDepthOffset.value=sDefaultDepthOffset;
    return state;
}
/* 0274D750 */
void sead::ProjectionState::update() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(mProjectionDirty) {
        auto* table=gabi::at<ProjectionDispatch>(gabi::ea(static_cast<void*>(mVtable)));
        gabi::call_ptr<void>(table->buildProjection,this,&mProjection);
        mProjectionDirty=0;
        table=gabi::at<ProjectionDispatch>(gabi::ea(static_cast<void*>(mVtable)));
        mDeviceChanged=1;
        u32 target=table->buildDeviceProjection;
        u32 mode=mDeviceMode;
        gabi::call_ptr<void>(target,this,&mDeviceProjection,&mProjection,mode);
        mDeviceChanged=0;
    } else if(mDeviceChanged) {
        auto* table=gabi::at<ProjectionDispatch>(gabi::ea(static_cast<void*>(mVtable)));
        u32 target=table->buildDeviceProjection;
        u32 mode=mDeviceMode;
        gabi::call_ptr<void>(target,this,&mDeviceProjection,&mProjection,mode);
        mDeviceChanged=0;
    }
}
/* 0274D80C */
sead::ProjectionMatrix* sead::ProjectionState::projection() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    update();
    return &mProjection;
}
/* 0274D83C */
sead::ProjectionMatrix* sead::ProjectionState::deviceProjection() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    update();
    return &mDeviceProjection;
}
/* 0274D9A0 */
void sead::swapProjectionRows(ProjectionMatrix* matrix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ProjectionRow> first(8);
    gabi::FrameLocal<ProjectionRow> second(0x18);
    seadReadProjectionRow(first.get(),matrix,0);
    seadReadProjectionRow(second.get(),matrix,1);
    seadWriteProjectionRow(matrix,second.get(),0);
    seadWriteProjectionRow(matrix,first.get(),1);
}

/* 0274E1C8 */
sead::ProjectionState* sead::createOrthographicProjection(ProjectionState* state) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<ProjectionState*>(seadAllocateViewStorage(sizeof(ProjectionState)));
    if(!state) return nullptr;
    createProjectionState(state);
    state->mProjectionDirty=1;
    state->mLeft.value=-0.5f;
    state->mVtable=&sOrthographicVtable;
    state->mFar.value=1.0f;
    state->mNear.value=0.0f;
    state->mRight.value=0.5f;
    state->mBottom.value=-0.5f;
    state->mTop.value=0.5f;
    return state;
}
/* 0274E25C */
sead::ProjectionState* sead::createOrthographicBounds(ProjectionState* state,f32 nearDistance,f32 farDistance,f32 top,f32 bottom,f32 left,f32 right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<ProjectionState*>(seadAllocateViewStorage(sizeof(ProjectionState)));
    if(!state) return nullptr;
    createProjectionState(state);
    state->mRight.value=right;
    state->mNear.value=nearDistance;
    state->mTop.value=top;
    state->mVtable=&sOrthographicVtable;
    state->mBottom.value=bottom;
    state->mLeft.value=left;
    state->mProjectionDirty=1;
    state->mFar.value=farDistance;
    return state;
}
/* 0274E364 */
sead::ProjectionState* sead::createOrthographicViewport(ProjectionState* state,const ViewportBounds* bounds,f32 nearDistance,f32 farDistance) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<ProjectionState*>(seadAllocateViewStorage(sizeof(ProjectionState)));
    if(!state) return nullptr;
    createProjectionState(state);
    state->mFar.value=farDistance;
    state->mNear.value=nearDistance;
    state->mVtable=&sOrthographicVtable;
    f32 bottom=bounds->mBottom.value;
    f32 top=bounds->mTop.value;
    f32 height=gabi::fsubs_ppc(bottom,top);
    state->mTop.value=gabi::fmuls_ppc(height,0.5f);
    bottom=bounds->mBottom.value;
    top=bounds->mTop.value;
    height=gabi::fsubs_ppc(bottom,top);
    height=gabi::fmuls_ppc(height,0.5f);
    state->mBottom.value=-height;
    f32 right=bounds->mRight.value;
    f32 left=bounds->mLeft.value;
    f32 width=gabi::fsubs_ppc(right,left);
    width=gabi::fmuls_ppc(width,0.5f);
    state->mLeft.value=-width;
    right=bounds->mRight.value;
    left=bounds->mLeft.value;
    width=gabi::fsubs_ppc(right,left);
    state->mRight.value=gabi::fmuls_ppc(width,0.5f);
    u32 mode=bounds->mDeviceMode;
    state->mDeviceChanged=1;
    state->mDeviceMode=mode;
    state->mProjectionDirty=1;
    return state;
}
/* 0274E594 */
sead::ProjectionState* sead::createFrustumBounds(ProjectionState* state,f32 nearDistance,f32 farDistance,f32 top,f32 bottom,f32 left,f32 right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<ProjectionState*>(seadAllocateViewStorage(sizeof(ProjectionState)));
    if(!state) return nullptr;
    createProjectionState(state);
    state->mRight.value=right;
    state->mNear.value=nearDistance;
    state->mTop.value=top;
    state->mVtable=&sFrustumVtable;
    state->mBottom.value=bottom;
    state->mLeft.value=left;
    state->mProjectionDirty=1;
    state->mFar.value=farDistance;
    return state;
}
/* 0274E87C */
sead::StoredProjection* sead::createStoredProjection(StoredProjection* state) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<StoredProjection*>(seadAllocateViewStorage(sizeof(StoredProjection)));
    if(!state) return nullptr;
    createProjectionState(&state->mBase);
    state->mBase.mVtable=&sStoredProjectionVtable;
    for(u32 row=0;row<4;++row) for(u32 column=0;column<4;++column) {
        u32 bits=sDefaultStoredProjection.mRows[row][column].bits;
        gabi::at<StoredProjectionStorage>(gabi::ea(state))->mStored.mRows[row][column].bits=bits;
    }
    state->mBase.mProjectionDirty=1;
    return state;
}
/* 0274EB90 */
void sead::copyStoredProjection(const StoredProjection* state,ProjectionMatrix* result) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    seadCopyProjectionMatrix(&gabi::at<StoredProjectionStorage>(gabi::ea(state))->mStored,result);
}

/* 0274D86C */
void sead::ProjectionState::projectPoint(ViewVector* result,const ViewVector* point) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    const ProjectionMatrix* matrix=projection();
    f32 wY=matrix->mRows[3][1].value;
    f32 y=point->y.value;
    f32 x=point->x.value;
    f32 wProduct=gabi::fmuls_ppc(wY,y);
    f32 wX=matrix->mRows[3][0].value;
    f32 xY=matrix->mRows[0][1].value;
    f32 wZ=matrix->mRows[3][2].value;
    f32 wSum=gabi::fmadds(wX,x,wProduct);
    f32 z=point->z.value;
    f32 xProduct=gabi::fmuls_ppc(xY,y);
    f32 wOffset=matrix->mRows[3][3].value;
    f32 xX=matrix->mRows[0][0].value;
    f32 wDot=gabi::fmadds(wZ,z,wSum);
    f32 xZ=matrix->mRows[0][2].value;
    f32 xSum=gabi::fmadds(xX,x,xProduct);
    f32 w=gabi::fadds_ppc(wDot,wOffset);
    f32 xDot=gabi::fmadds(xZ,z,xSum);
    f32 inverseW=gabi::fdivs_ppc(1.0f,w);
    f32 xOffset=matrix->mRows[0][3].value;
    f32 projectedX=gabi::fadds_ppc(xDot,xOffset);
    result->x.value=gabi::fmuls_ppc(projectedX,inverseW);
    f32 yY=matrix->mRows[1][1].value;
    f32 yX=matrix->mRows[1][0].value;
    f32 yProduct=gabi::fmuls_ppc(yY,y);
    f32 ySum=gabi::fmadds(yX,x,yProduct);
    f32 yZ=matrix->mRows[1][2].value;
    f32 yDot=gabi::fmadds(yZ,z,ySum);
    f32 yOffset=matrix->mRows[1][3].value;
    f32 projectedY=gabi::fadds_ppc(yDot,yOffset);
    result->y.value=gabi::fmuls_ppc(projectedY,inverseW);
    f32 zY=matrix->mRows[2][1].value;
    f32 zX=matrix->mRows[2][0].value;
    f32 zProduct=gabi::fmuls_ppc(zY,y);
    f32 zSum=gabi::fmadds(zX,x,zProduct);
    f32 zZ=matrix->mRows[2][2].value;
    f32 zDot=gabi::fmadds(zZ,z,zSum);
    f32 zOffset=matrix->mRows[2][3].value;
    f32 projectedZ=gabi::fadds_ppc(zDot,zOffset);
    result->z.value=gabi::fmuls_ppc(projectedZ,inverseW);
}
/* 0274D954 */
void sead::ProjectionState::projectToViewport(ViewVector* result,const ViewVector* point,const ViewportBounds* viewport) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ViewVector> projected(8);
    projectPoint(projected.get(),point);
    seadViewportProject(viewport,result,projected.get());
}
/* 0274CB50 */
void sead::ViewTransform::projectToViewport(ViewVector* result,const ViewVector* point,ProjectionState* projection,const ViewportBounds* viewport) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<ViewVector> viewed(8);
    transformPoint(viewed.get(),point);
    projection->projectToViewport(result,viewed.get(),viewport);
}

/* 0274E4B8 */
void sead::ProjectionState::buildOrthographicMatrix(ProjectionMatrix* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 right=mRight.value;
    f32 left=mLeft.value;
    f32 top=mTop.value;
    f32 sumX=gabi::fadds_ppc(left,right);
    f32 bottom=mBottom.value;
    f32 sumY=gabi::fadds_ppc(top,bottom);
    f32 width=gabi::fsubs_ppc(right,left);
    f32 centerX=gabi::fmuls_ppc(sumX,0.5f);
    f32 halfWidth=gabi::fmuls_ppc(width,0.5f);
    f32 translatedX=gabi::fdivs_ppc(centerX,halfWidth);
    f32 centerY=gabi::fmuls_ppc(sumY,0.5f);
    result->mRows[0][1].value=0.0f;
    f32 scaleX=gabi::fdivs_ppc(1.0f,halfWidth);
    result->mRows[0][3].value=-translatedX;
    result->mRows[0][2].value=0.0f;
    result->mRows[0][0].value=scaleX;
    top=mTop.value;
    bottom=mBottom.value;
    result->mRows[1][2].value=0.0f;
    f32 height=gabi::fsubs_ppc(top,bottom);
    f32 halfHeight=gabi::fmuls_ppc(height,0.5f);
    f32 translatedY=gabi::fdivs_ppc(centerY,halfHeight);
    f32 scaleY=gabi::fdivs_ppc(1.0f,halfHeight);
    result->mRows[1][3].value=-translatedY;
    result->mRows[1][0].value=0.0f;
    result->mRows[1][1].value=scaleY;
    f32 farDistance=mFar.value;
    f32 nearDistance=mNear.value;
    result->mRows[2][1].value=0.0f;
    f32 distance=gabi::fsubs_ppc(farDistance,nearDistance);
    f32 inverseDepth=gabi::fdivs_ppc(1.0f,distance);
    f32 scaleZ=gabi::fmuls_ppc(-2.0f,inverseDepth);
    result->mRows[2][0].value=0.0f;
    result->mRows[2][2].value=scaleZ;
    farDistance=mFar.value;
    nearDistance=mNear.value;
    result->mRows[3][2].value=0.0f;
    f32 sumZ=gabi::fadds_ppc(nearDistance,farDistance);
    result->mRows[3][1].value=0.0f;
    f32 translatedZ=gabi::fmuls_ppc(sumZ,inverseDepth);
    result->mRows[3][3].value=1.0f;
    result->mRows[3][0].value=0.0f;
    result->mRows[2][3].value=-translatedZ;
}
/* 0274E6B0 */
void sead::ProjectionState::buildFrustumMatrix(ProjectionMatrix* result) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    f32 left=mLeft.value;
    f32 right=mRight.value;
    f32 nearDistance=mNear.value;
    f32 width=gabi::fsubs_ppc(right,left);
    f32 doubleNear=gabi::fadds_ppc(nearDistance,nearDistance);
    f32 inverseWidth=gabi::fdivs_ppc(1.0f,width);
    f32 scaleX=gabi::fmuls_ppc(doubleNear,inverseWidth);
    result->mRows[0][1].value=0.0f;
    result->mRows[0][0].value=scaleX;
    left=mLeft.value;
    right=mRight.value;
    f32 sumX=gabi::fadds_ppc(left,right);
    f32 offsetX=gabi::fmuls_ppc(sumX,inverseWidth);
    result->mRows[0][3].value=0.0f;
    result->mRows[0][2].value=offsetX;
    f32 top=mTop.value;
    f32 bottom=mBottom.value;
    result->mRows[1][0].value=0.0f;
    nearDistance=mNear.value;
    f32 height=gabi::fsubs_ppc(top,bottom);
    doubleNear=gabi::fadds_ppc(nearDistance,nearDistance);
    f32 inverseHeight=gabi::fdivs_ppc(1.0f,height);
    f32 scaleY=gabi::fmuls_ppc(doubleNear,inverseHeight);
    result->mRows[1][1].value=scaleY;
    top=mTop.value;
    bottom=mBottom.value;
    f32 sumY=gabi::fadds_ppc(top,bottom);
    f32 offsetY=gabi::fmuls_ppc(sumY,inverseHeight);
    result->mRows[1][3].value=0.0f;
    result->mRows[1][2].value=offsetY;
    nearDistance=mNear.value;
    f32 farDistance=mFar.value;
    result->mRows[2][0].value=0.0f;
    f32 depth=gabi::fsubs_ppc(farDistance,nearDistance);
    result->mRows[2][1].value=0.0f;
    f32 inverseDepth=gabi::fdivs_ppc(1.0f,depth);
    farDistance=mFar.value;
    nearDistance=mNear.value;
    f32 sumZ=gabi::fadds_ppc(farDistance,nearDistance);
    f32 scaleZ=gabi::fmuls_ppc(sumZ,inverseDepth);
    result->mRows[2][2].value=-scaleZ;
    farDistance=mFar.value;
    nearDistance=mNear.value;
    f32 doubleFar=gabi::fadds_ppc(farDistance,farDistance);
    result->mRows[3][2].value=-1.0f;
    f32 productZ=gabi::fmuls_ppc(doubleFar,nearDistance);
    result->mRows[3][1].value=0.0f;
    f32 offsetZ=gabi::fmuls_ppc(productZ,inverseDepth);
    result->mRows[3][0].value=0.0f;
    result->mRows[3][3].value=0.0f;
    result->mRows[2][3].value=-offsetZ;
}

/* 0274E910 */
void sead::StoredProjection::setMatrix(const ProjectionMatrix* matrix,u32 orientation) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* stored=gabi::at<StoredProjectionStorage>(gabi::ea(this));
    seadCopyProjectionMatrix(matrix,&stored->mStored);
    if(orientation==1 || orientation==2) swapProjectionRows(&stored->mStored);
    if(orientation==1 || orientation==4) {
        f32 x=stored->mStored.mRows[0][0].value;
        f32 y=stored->mStored.mRows[0][1].value;
        f32 z=stored->mStored.mRows[0][2].value;
        stored->mStored.mRows[0][0].value=-x;
        stored->mStored.mRows[0][1].value=-y;
        stored->mStored.mRows[0][2].value=-z;
        f32 w=stored->mStored.mRows[0][3].value;
        stored->mStored.mRows[0][3].value=-w;
    } else if(orientation==2 || orientation==5) {
        f32 x=stored->mStored.mRows[1][0].value;
        f32 y=stored->mStored.mRows[1][1].value;
        f32 z=stored->mStored.mRows[1][2].value;
        stored->mStored.mRows[1][0].value=-x;
        stored->mStored.mRows[1][1].value=-y;
        stored->mStored.mRows[1][2].value=-z;
        f32 w=stored->mStored.mRows[1][3].value;
        stored->mStored.mRows[1][3].value=-w;
    } else if(orientation==3) {
        f32 row0W=stored->mStored.mRows[0][3].value;
        f32 row1Z=stored->mStored.mRows[1][2].value;
        f32 row1Y=stored->mStored.mRows[1][1].value;
        f32 row1X=stored->mStored.mRows[1][0].value;
        stored->mStored.mRows[0][3].value=-row0W;
        stored->mStored.mRows[1][2].value=-row1Z;
        f32 row0Z=stored->mStored.mRows[0][2].value;
        f32 row0X=stored->mStored.mRows[0][0].value;
        stored->mStored.mRows[1][0].value=-row1X;
        f32 row0Y=stored->mStored.mRows[0][1].value;
        stored->mStored.mRows[0][2].value=-row0Z;
        stored->mStored.mRows[1][1].value=-row1Y;
        stored->mStored.mRows[0][0].value=-row0X;
        stored->mStored.mRows[0][1].value=-row0Y;
        f32 row1W=stored->mStored.mRows[1][3].value;
        stored->mStored.mRows[1][3].value=-row1W;
    }
    mBase.mProjectionDirty=1;
}
/* 0274EAF0 */
sead::StoredProjection* sead::createStoredMatrix(StoredProjection* state,const ProjectionMatrix* matrix,u32 orientation) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if(!state) state=static_cast<StoredProjection*>(seadAllocateViewStorage(sizeof(StoredProjection)));
    if(!state) return nullptr;
    createProjectionState(&state->mBase);
    state->mBase.mVtable=&sStoredProjectionVtable;
    auto* stored=gabi::at<StoredProjectionStorage>(gabi::ea(state));
    if(gabi::ea(&stored->mStored)==0) seadAllocateViewStorage(sizeof(ProjectionMatrix));
    state->setMatrix(matrix,orientation);
    return state;
}

/* 0274DA08 */
void sead::ProjectionState::applyDeviceTransform(ProjectionMatrix* result,const ProjectionMatrix* source,u32 orientation) const {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    seadCopyProjectionMatrix(source,result);
    switch(orientation) {
    case 1:
        for(u32 column=0;column<4;++column) result->mRows[0][column].value=-static_cast<f32>(result->mRows[0][column].value);
        swapProjectionRows(result);
        break;
    case 2:
        for(u32 column=0;column<4;++column) result->mRows[1][column].value=-static_cast<f32>(result->mRows[1][column].value);
        swapProjectionRows(result);
        break;
    case 3:
        for(u32 row=0;row<2;++row) for(u32 column=0;column<4;++column)
            result->mRows[row][column].value=-static_cast<f32>(result->mRows[row][column].value);
        break;
    case 4:
        for(u32 column=0;column<4;++column) result->mRows[0][column].value=-static_cast<f32>(result->mRows[0][column].value);
        break;
    case 5:
        for(u32 column=0;column<4;++column) result->mRows[1][column].value=-static_cast<f32>(result->mRows[1][column].value);
        break;
    }
    f32 x=result->mRows[2][0].value;
    f32 scale=mDepthScale.value;
    f32 scaledX=gabi::fmuls_ppc(x,scale);
    f32 y=result->mRows[2][1].value;
    result->mRows[2][0].value=scaledX;
    scale=mDepthScale.value;
    f32 z=result->mRows[2][2].value;
    f32 scaledY=gabi::fmuls_ppc(y,scale);
    f32 projectZ=result->mRows[3][2].value;
    result->mRows[2][1].value=scaledY;
    f32 offset=mDepthOffset.value;
    f32 translatedZ=gabi::fmadds(projectZ,offset,z);
    scale=mDepthScale.value;
    f32 scaledZ=gabi::fmuls_ppc(translatedZ,scale);
    f32 projectW=result->mRows[3][3].value;
    result->mRows[2][2].value=scaledZ;
    offset=mDepthOffset.value;
    scale=mDepthScale.value;
    f32 offsetW=gabi::fmuls_ppc(projectW,offset);
    f32 w=result->mRows[2][3].value;
    result->mRows[2][3].value=gabi::fmadds(w,scale,offsetW);
}
/* WWHD glue
ext 0274FAE0 void seadViewportProject(const sead::ViewportBounds*,sead::ViewVector*,const sead::ViewVector*)
data sOrthographicVtable 101452A0
data sFrustumVtable 101452E0
data sStoredProjectionVtable 10145320
data sDefaultStoredProjection 104A048C
ext 028E8970 void seadCopyProjectionMatrix(const sead::ProjectionMatrix*,sead::ProjectionMatrix*)
data sProjectionVtable 10145220
data sDefaultDevicePosture 101F8B1C
data sDefaultDepthScale 101F8B20
data sDefaultDepthOffset 101F8B24
ext 0274F300 void seadReadProjectionRow(sead::ProjectionRow*,const sead::ProjectionMatrix*,u32)
ext 0274F32C void seadWriteProjectionRow(sead::ProjectionMatrix*,const sead::ProjectionRow*,u32)
ext 028F4D28 f32 seadProjectionAtan2(f32,f32)
*/
