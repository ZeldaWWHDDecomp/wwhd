/* HD ErrorViewer layer installs a global error renderer. Private decomp source. */
#include "hd/hd_error_viewer.h"
static hdErrorViewerHeaderZero_c sErrorViewerHeaderZero;
static hdErrorViewerHeaderDescriptor_c sErrorViewerHeaderDescriptor;
static hdErrorViewerHeaderDescriptor_c sErrorViewerRotationDescriptor;
static hdErrorViewerHeaderDescriptor_c sErrorViewerPositiveRotationDescriptor;
static f32 sErrorViewerMinRotation;
static f32 sErrorViewerMaxRotation;
static u8 sErrorViewerRotationGuard;
static u8 sErrorViewerPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sErrorViewerSafeStringVtable;
static u32 sErrorViewerLayerVtable;
static u32 sErrorViewerDrawVtable;
static u32 sErrorViewerLayer3DVtable;
static u32 sErrorViewerStepVtable;
static u32 sErrorViewerDelegateVtable;
static u8 sErrorViewerErrorViewer;
static void* allocateErrorLayer(u32 size);
static void deleteErrorLayer(void* layer);
static void destroyErrorCamera(hdErrorViewerCamera_l* camera, s32 flags);
static void destroyErrorPerspective(hdErrorViewerPerspective_l* perspective, s32 flags);
static void destroyErrorProjection(hdErrorViewerProjection_l* projection, s32 flags);
static void destroyErrorLayerBase(hdErrorViewer_c* layer, s32 flags);
static void constructErrorLayerBase(hdErrorViewer_c* layer, SafeString* name);
static void constructErrorStep(hdErrorRenderStep_c* step);
static void addErrorStep(hdErrorViewer_c* layer, hdErrorRenderStep_c* step);
static void renderGlobalErrorViewer(void* viewer);
/* 02734924 */
static s32 errorLayerNoDraw() { return 0; }
/* 0273492C */
static s32 errorLayerNoCalc() { return 0; }
/* 02734A3C */
static void destroyErrorViewerLayer3D(hdErrorViewerLayer3D_c* layer, u32 flags) {
    if (layer != NULL) {
        layer->mBase.mDrawVtable = gabi::ea(&sErrorViewerLayer3DVtable);
        destroyErrorCamera(&layer->mCamera, 2);
        destroyErrorPerspective(&layer->mPerspective, 2);
        destroyErrorProjection(&layer->mDrcProjection, 2);
        destroyErrorProjection(&layer->mProjection, 2);
        destroyErrorLayerBase(&layer->mBase, 0);
        if (flags & 1) deleteErrorLayer(layer);
    }
}
/* 02734ACC */
static s32 errorLayerDrawOrder() { return 1; }
/* 02734B68 */
static hdErrorViewer_c* constructErrorViewer(hdErrorViewer_c* layer) {
    if (layer == NULL) {
        layer = (hdErrorViewer_c*)allocateErrorLayer(sizeof(hdErrorViewer_c));
        if (layer == NULL) return NULL;
    }
    SafeString name;
    name.__vtbl = gabi::ea(&sErrorViewerSafeStringVtable);
    name.mStringTop = gabi::guest_addr("ErrorViewer");
    constructErrorLayerBase(layer, &name);
    layer->mVtable = gabi::ea(&sErrorViewerLayerVtable);
    layer->mDrawVtable = gabi::ea(&sErrorViewerDrawVtable);
    return layer;
}
/* 02734CCC */
static void drawErrorViewer() { renderGlobalErrorViewer(&sErrorViewerErrorViewer); }
/* 02734BE4 */
void hdErrorViewer_c::initialize() {
    hdErrorRenderStep_c* step = (hdErrorRenderStep_c*)allocateErrorLayer(sizeof(hdErrorRenderStep_c));
    if (step != NULL) {
        constructErrorStep(step);
        SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdErrorRenderStep_c, mName));
        if (name == NULL) name = (SafeString*)allocateErrorLayer(sizeof(SafeString));
        if (name != NULL) {
            name->__vtbl = gabi::ea(&sErrorViewerSafeStringVtable);
            name->mStringTop = gabi::guest_addr("");
        }
        step->mFlags = 0;
        step->mVtable = gabi::ea(&sErrorViewerStepVtable);
        if (u32(gabi::ea(step)+offsetof(hdErrorRenderStep_c, mDelegateVtable)) != 0) {
            step->mpLayer = this;
            step->mDelegateAdjustment = 0x0000FFFF;
            step->mDelegateVtable = gabi::ea(&sErrorViewerDelegateVtable);
            step->mDrawFunction = gabi::guest_fn((const void*)drawErrorViewer);
        }
        step->mTarget = 2;
        step->mName.mStringTop = gabi::guest_addr("ErrorViewer");
    }
    addErrorStep(this, step);
}
/* 02734CD8 */
static void __sinit_hd_error_viewer_cpp() {
    for (u32 i = 0; i < 4; ++i) sErrorViewerHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sErrorViewerHeaderDescriptor));
    sErrorViewerMinRotation = -3.1415927410125732f;
    sErrorViewerMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sErrorViewerRotationGuard);
    __register_global_object(gabi::ea(&sErrorViewerRotationDescriptor));
    initializePositiveRotationGuard(&sErrorViewerPositiveRotationGuard);
    __register_global_object(gabi::ea(&sErrorViewerPositiveRotationDescriptor));
}
/* WWHD glue
 data sErrorViewerSafeStringVtable 101423DC nocheck
 data sErrorViewerLayerVtable 10142460 nocheck
 data sErrorViewerDrawVtable 10142508 nocheck
 data sErrorViewerLayer3DVtable 101422AC nocheck
 data sErrorViewerStepVtable 10169E3C nocheck
 data sErrorViewerDelegateVtable 10142424 nocheck
 data sErrorViewerHeaderZero 1049FA5C
 data sErrorViewerHeaderDescriptor 101F88D8
 data sErrorViewerRotationDescriptor 101F88E4
 data sErrorViewerPositiveRotationDescriptor 101F88F0
 data sErrorViewerMinRotation 1049FA50
 data sErrorViewerMaxRotation 1049FA54
 data sErrorViewerRotationGuard 1049FA58
 data sErrorViewerPositiveRotationGuard 1049FA59
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sErrorViewerErrorViewer 104B45C0
 str 10142440 in 02734B68
 str 1014244C in 02734BE4
 str 101423D8 "" in 02734BE4
 ext 0273AD10 void* allocateErrorLayer(u32 size)
 ext 0273AF40 void deleteErrorLayer(void* layer)
 ext 0274CCB0 void destroyErrorCamera(hdErrorViewerCamera_l* camera, s32 flags)
 ext 0274E038 void destroyErrorPerspective(hdErrorViewerPerspective_l* perspective, s32 flags)
 ext 0274E474 void destroyErrorProjection(hdErrorViewerProjection_l* projection, s32 flags)
 ext 02734248 void destroyErrorLayerBase(hdErrorViewer_c* layer, s32 flags)
 ext 027340D8 void constructErrorLayerBase(hdErrorViewer_c* layer, SafeString* name)
 ext 02752A84 void constructErrorStep(hdErrorRenderStep_c* step)
 ext 027C2450 void addErrorStep(hdErrorViewer_c* layer, hdErrorRenderStep_c* step)
 ext 027F29D4 void renderGlobalErrorViewer(void* viewer)
*/
