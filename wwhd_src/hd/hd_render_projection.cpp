/* HD render projection: update perspective from an independent X/Y scale. Private decomp source. */
#include "hd/hd_render_projection.h"
static hdRenderProjectionHeaderZero_c sRenderProjectionHeaderZero;
static hdRenderProjectionHeaderDescriptor_c sRenderProjectionHeaderDescriptor;
static hdRenderProjectionHeaderDescriptor_c sRenderProjectionRotationDescriptor;
static hdRenderProjectionHeaderDescriptor_c sRenderProjectionPositiveRotationDescriptor;
static f32 sRenderProjectionMinRotation;
static f32 sRenderProjectionMaxRotation;
static u8 sRenderProjectionRotationGuard;
static u8 sRenderProjectionPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sRenderProjectionProjectionVtable;
static hdProjectionScale_c sRenderProjectionDefaultScale;
static void constructPerspective(hdRenderProjection_c* projection);
static void destroyPerspective(hdRenderProjection_c* projection, s32 flags);
static hdProjectionParameters_l* getPerspectiveParameters(hdRenderProjection_c* projection);
static f32 projectionAtan(f32 value);
static void makePerspectiveMatrix(hdRenderProjection_c* projection, hdProjectionMatrix_c* matrix);
static void* allocateProjection(u32 size);
static void deleteProjection(hdRenderProjection_c* projection);

/* 027389F8 */
static hdRenderProjection_c* constructRenderProjection(hdRenderProjection_c* projection) {
    if (projection == NULL) {
        projection = (hdRenderProjection_c*)allocateProjection(sizeof(hdRenderProjection_c));
        if (projection == NULL) return NULL;
    }
    constructPerspective(projection);
    projection->mVtable = gabi::ea(&sRenderProjectionProjectionVtable);
    projection->mScale.mXBits = sRenderProjectionDefaultScale.mXBits;
    projection->mFieldOfView = 0.7853981852531433f;
    projection->mScale.mYBits = sRenderProjectionDefaultScale.mYBits;
    return projection;
}
/* 02738A6C */
static void destroyRenderProjection(hdRenderProjection_c* projection, u32 flags) {
    if (projection != NULL) {
        destroyPerspective(projection, 0);
        if (flags & 1) deleteProjection(projection);
    }
}
/* 02738AC0 */
void hdRenderProjection_c::setScale(const hdProjectionScale_c* scale) {
    mScale.mXBits = scale->mXBits;
    u32 y = scale->mYBits;
    mDirty = 1;
    mScale.mYBits = y;
    hdProjectionParameters_l* parameters = getPerspectiveParameters(this);
    f32 angle = projectionAtan(1.0f / parameters->mAspect);
    mFieldOfView = gabi::fadds_ppc(angle, angle);
}
/* 02738B20 */
void hdRenderProjection_c::makeMatrix(hdProjectionMatrix_c* matrix) {
    makePerspectiveMatrix(this, matrix);
    f32 xScale = mScale.mX;
    matrix->mElements[0] = gabi::fmuls_ppc(matrix->mElements[0], xScale);
    f32 yScale = mScale.mY;
    matrix->mElements[5] = gabi::fmuls_ppc(matrix->mElements[5], yScale);
}
/* 02738B78 */
static void __sinit_hd_render_projection_cpp() {
    for (u32 i = 0; i < 4; ++i) sRenderProjectionHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sRenderProjectionHeaderDescriptor));
    sRenderProjectionMinRotation = -3.1415927410125732f;
    sRenderProjectionMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sRenderProjectionRotationGuard);
    __register_global_object(gabi::ea(&sRenderProjectionRotationDescriptor));
    initializePositiveRotationGuard(&sRenderProjectionPositiveRotationGuard);
    __register_global_object(gabi::ea(&sRenderProjectionPositiveRotationDescriptor));
}
/* WWHD glue
 data sRenderProjectionProjectionVtable 10142D30 nocheck
 data sRenderProjectionHeaderZero 1049FB24
 data sRenderProjectionHeaderDescriptor 101F8A4C
 data sRenderProjectionRotationDescriptor 101F8A58
 data sRenderProjectionPositiveRotationDescriptor 101F8A64
 data sRenderProjectionMinRotation 1049FB18
 data sRenderProjectionMaxRotation 1049FB1C
 data sRenderProjectionRotationGuard 1049FB20
 data sRenderProjectionPositiveRotationGuard 1049FB21
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sRenderProjectionDefaultScale 104A0920
 ext 0274DEA8 void constructPerspective(hdRenderProjection_c* projection)
 ext 0274E038 void destroyPerspective(hdRenderProjection_c* projection, s32 flags)
 ext 0274D80C hdProjectionParameters_l* getPerspectiveParameters(hdRenderProjection_c* projection)
 ext 028F4DC4 f32 projectionAtan(f32 value)
 ext 0274E0B8 void makePerspectiveMatrix(hdRenderProjection_c* projection, hdProjectionMatrix_c* matrix)
 ext 0273AD10 void* allocateProjection(u32 size)
 ext 0273AF40 void deleteProjection(hdRenderProjection_c* projection)
*/
