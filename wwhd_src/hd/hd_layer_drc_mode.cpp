/* HD LayerDRCMode2D installs the DRC-mode render step. Private decomp source. */
#include "hd/hd_layer_drc_mode.h"
static hdLayerDrcModeHeaderZero_c sLayerDrcModeHeaderZero;
static hdLayerDrcModeHeaderDescriptor_c sLayerDrcModeHeaderDescriptor;
static hdLayerDrcModeHeaderDescriptor_c sLayerDrcModeRotationDescriptor;
static hdLayerDrcModeHeaderDescriptor_c sLayerDrcModePositiveRotationDescriptor;
static f32 sLayerDrcModeMinRotation;
static f32 sLayerDrcModeMaxRotation;
static u8 sLayerDrcModeRotationGuard;
static u8 sLayerDrcModePositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sLayerDrcModeSafeStringVtable;
static u32 sLayerDrcModeLayerVtable;
static u32 sLayerDrcModeDrawVtable;
static u32 sLayerDrcModeStepVtable;
static u32 sLayerDrcModeDelegateVtable;
static u32 sLayerDrcModeCallbackVtable;
static void* allocateDrcLayer(u32 size);
static void constructDrcLayerBase(hdDrcLayer_c* layer, SafeString* name);
static void initializeDrcLayerBase(hdDrcLayer_c* layer);
static void initializeLayerDraw(hdDrcLayer_c* layer);
static void constructDrcStep(hdDrcRenderStep_c* step);
static void addDrcStep(hdDrcLayer_c* layer, hdDrcRenderStep_c* step);

/* 0273460C */
static s32 drcLayerNoDraw() { return 0; }
/* 02734614 */
static s32 drcLayerNoCalc() { return 0; }
/* 027346A8 */
static hdDrcLayer_c* constructDrcModeLayer(hdDrcLayer_c* layer) {
    if (layer == NULL) {
        layer = (hdDrcLayer_c*)allocateDrcLayer(sizeof(hdDrcLayer_c));
        if (layer == NULL) return NULL;
    }
    SafeString name;
    name.__vtbl = gabi::ea(&sLayerDrcModeSafeStringVtable);
    name.mStringTop = gabi::guest_addr("UI_DRCMode");
    constructDrcLayerBase(layer, &name);
    layer->mVtable = gabi::ea(&sLayerDrcModeLayerVtable);
    layer->mDrawVtable = gabi::ea(&sLayerDrcModeDrawVtable);
    return layer;
}
/* 02734818 */
static void drawDrcMode() {}
/* 02734724 */
void hdDrcLayer_c::initialize() {
    initializeDrcLayerBase(this);
    initializeLayerDraw(this);
    hdDrcRenderStep_c* step = (hdDrcRenderStep_c*)allocateDrcLayer(sizeof(hdDrcRenderStep_c));
    if (step != NULL) {
        constructDrcStep(step);
        SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdDrcRenderStep_c, mName));
        if (name == NULL) name = (SafeString*)allocateDrcLayer(sizeof(SafeString));
        if (name != NULL) {
            name->__vtbl = gabi::ea(&sLayerDrcModeSafeStringVtable);
            name->mStringTop = gabi::guest_addr("");
        }
        step->mFlags = 0;
        step->mVtable = gabi::ea(&sLayerDrcModeStepVtable);
        if (u32(gabi::ea(step)+offsetof(hdDrcRenderStep_c, mDelegateVtable)) != 0) {
            step->mpLayer = this;
            step->mDelegateAdjustment = 0x0000FFFF;
            step->mDelegateVtable = gabi::ea(&sLayerDrcModeDelegateVtable);
            step->mDrawFunction = gabi::guest_fn((const void*)drawDrcMode);
        }
        step->mTarget = 2;
        step->mName.mStringTop = gabi::guest_addr("LayerDRCMode2D");
    }
    addDrcStep(this, step);
}
/* 0273481C */
static void __sinit_hd_layer_drc_mode_cpp() {
    for (u32 i = 0; i < 4; ++i) sLayerDrcModeHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sLayerDrcModeHeaderDescriptor));
    sLayerDrcModeMinRotation = -3.1415927410125732f;
    sLayerDrcModeMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sLayerDrcModeRotationGuard);
    __register_global_object(gabi::ea(&sLayerDrcModeRotationDescriptor));
    initializePositiveRotationGuard(&sLayerDrcModePositiveRotationGuard);
    __register_global_object(gabi::ea(&sLayerDrcModePositiveRotationDescriptor));
}
/* WWHD glue
 data sLayerDrcModeSafeStringVtable 10142274 nocheck
 data sLayerDrcModeLayerVtable 10142310 nocheck
 data sLayerDrcModeDrawVtable 101423B8 nocheck
 data sLayerDrcModeStepVtable 10169E3C nocheck
 data sLayerDrcModeDelegateVtable 101422DC nocheck
 data sLayerDrcModeCallbackVtable 101422F8 nocheck
 data sLayerDrcModeHeaderZero 1049FA40
 data sLayerDrcModeHeaderDescriptor 101F88AC
 data sLayerDrcModeRotationDescriptor 101F88B8
 data sLayerDrcModePositiveRotationDescriptor 101F88C4
 data sLayerDrcModeMinRotation 1049FA34
 data sLayerDrcModeMaxRotation 1049FA38
 data sLayerDrcModeRotationGuard 1049FA3C
 data sLayerDrcModePositiveRotationGuard 1049FA3D
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 str 101422EC
 str 10142268 "" in 02734724
 str 101422F8
 ext 0273AD10 void* allocateDrcLayer(u32 size)
 ext 0272D978 void constructDrcLayerBase(hdDrcLayer_c* layer, SafeString* name)
 ext 0272DB10 void initializeDrcLayerBase(hdDrcLayer_c* layer)
 ext 027343FC void initializeLayerDraw(hdDrcLayer_c* layer)
 ext 02752A84 void constructDrcStep(hdDrcRenderStep_c* step)
 ext 027C2450 void addDrcStep(hdDrcLayer_c* layer, hdDrcRenderStep_c* step)
*/
