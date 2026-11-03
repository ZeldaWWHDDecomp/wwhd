// Original projection helpers construct temporary objects before copying the device matrix.
#include "agl/EnvironmentObject.h"
#include "agl/ProjectionWorkspace.h"
using namespace agl::env;
static f32 sProjectorAngleScale;
static f32 sProjectorBoundsOrigin;
sead::PerspectiveState* constructProjectorPerspective(sead::PerspectiveState*,f32,f32,f32,f32);
sead::Viewport* constructProjectorBounds(sead::Viewport*,f32,f32,f32,f32);
sead::ProjectionState* constructProjectorOrthographic(sead::ProjectionState*,const sead::Viewport*,f32,f32);
const sead::ProjectionMatrix* projectorDeviceMatrix(sead::ProjectionState*);
void copyProjectorDeviceMatrix(const sead::ProjectionMatrix*,sead::RenderMatrix44*);
void destroyProjectorPerspective(sead::PerspectiveState*,u32);
void destroyProjectorOrthographic(sead::ProjectionState*,u32);

/* 0276D5E0 */
void Projector::updateProjection() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 projectionKind = mProjectionType.mValue;
    if (projectionKind == 0) {
        gabi::FrameLocal<sead::PerspectiveState> perspective(8);
        f32 fieldOfView = mFieldOfView.mValue;
        f32 aspect = mAspect.mValue;
        f32 angleScale = sProjectorAngleScale;
        f32 farPlane = mFarPlane.mValue;
        f32 angle = gabi::fmuls_ppc(fieldOfView,angleScale);
        f32 nearPlane = mNearPlane.mValue;
        constructProjectorPerspective(perspective.get(),nearPlane,farPlane,angle,aspect);
        const sead::ProjectionMatrix* matrix = projectorDeviceMatrix(reinterpret_cast<sead::ProjectionState*>(perspective.get()));
        copyProjectorDeviceMatrix(matrix,&mProjection);
        destroyProjectorPerspective(perspective.get(),2);
    } else if (projectionKind == 1) {
        gabi::FrameLocal<sead::Viewport> bounds(8);
        f32 height = mHeight.mValue;
        f32 aspect = mAspect.mValue;
        f32 origin = sProjectorBoundsOrigin;
        f32 width = gabi::fmuls_ppc(aspect,height);
        constructProjectorBounds(bounds.get(),origin,origin,width,height);
        f32 nearPlane = mNearPlane.mValue;
        gabi::FrameLocal<sead::ProjectionState> orthographic(32);
        f32 farPlane = mFarPlane.mValue;
        constructProjectorOrthographic(orthographic.get(),bounds.get(),nearPlane,farPlane);
        const sead::ProjectionMatrix* matrix = projectorDeviceMatrix(orthographic.get());
        copyProjectorDeviceMatrix(matrix,&mProjection);
        destroyProjectorOrthographic(orthographic.get(),2);
    }
}

void updateProjectorView(Projector*);
void updateProjectorProjection(Projector*);
void updateProjectorCombinedMatrix(Projector*);
/* 0276D6E0 */
void Projector::updateMatrices() {
    gabi::AutoFrame frame{gabi::current_function()};
    updateProjectorView(this);
    updateProjectorProjection(this);
    updateProjectorCombinedMatrix(this);
}

/* WWHD glue
 data sProjectorAngleScale 10160018
 data sProjectorBoundsOrigin 1015FDB4
 ext 0274DF4C sead::PerspectiveState* constructProjectorPerspective(sead::PerspectiveState*,f32,f32,f32,f32)
 ext 0274F424 sead::Viewport* constructProjectorBounds(sead::Viewport*,f32,f32,f32,f32)
 ext 0274E364 sead::ProjectionState* constructProjectorOrthographic(sead::ProjectionState*,const sead::Viewport*,f32,f32)
 ext 0274D83C const sead::ProjectionMatrix* projectorDeviceMatrix(sead::ProjectionState*)
 ext 028E8970 void copyProjectorDeviceMatrix(const sead::ProjectionMatrix*,sead::RenderMatrix44*)
 ext 0274E038 void destroyProjectorPerspective(sead::PerspectiveState*,u32)
 ext 0274E474 void destroyProjectorOrthographic(sead::ProjectionState*,u32)
 ext 0276D324 void updateProjectorView(Projector*)
 ext 0276D5E0 void updateProjectorProjection(Projector*)
 ext 0276D6D0 void updateProjectorCombinedMatrix(Projector*)
*/
