/* HD LayerMask draws upper and lower masking bars. Private decomp source. */
#include "hd/hd_layer_mask.h"
static hdLayerMaskHeaderZero_c sLayerMaskHeaderZero;
static hdLayerMaskHeaderDescriptor_c sLayerMaskHeaderDescriptor;
static hdLayerMaskHeaderDescriptor_c sLayerMaskRotationDescriptor;
static hdLayerMaskHeaderDescriptor_c sLayerMaskPositiveRotationDescriptor;
static f32 sLayerMaskMinRotation;
static f32 sLayerMaskMaxRotation;
static u8 sLayerMaskRotationGuard;
static u8 sLayerMaskPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sLayerMaskSafeStringVtable;
static u32 sLayerMaskLayerVtable;
static u32 sLayerMaskDrawVtable;
static constexpr u32 kLayerMaskLayer3DVtable = 0;
static u32 sLayerMaskStepVtable;
static u32 sLayerMaskDelegateVtable;
static hdMaskScreenMgr_l* sLayerMaskScreens;
static u8 sLayerMaskMaskRenderer;
static u8 sLayerMaskDrawUtility;
static void* allocateMaskLayer(u32 size);
static void drawLayerMask(hdLayerMask_c* layer, hdMaskRenderInfo_c* info);
static void constructMaskLayerBase(hdLayerMask_c* layer, SafeString* name);
static void constructMaskStep(hdMaskRenderStep_c* step);
static void addMaskStep(hdLayerMask_c* layer, hdMaskRenderStep_c* step);
/* 02733810 */
static s32 maskLayerNoDraw() { return 0; }
/* 027338D8 */
static s32 maskLayerCount() { return 17; }
/* 02733988 */
static BOOL maskSupportsTarget(hdLayerMask_c*, u32 target) { return target == 0; }
/* 02733994 */
static hdLayerMask_c* constructLayerMask(hdLayerMask_c* layer) {
    if (layer == NULL) {
        layer = (hdLayerMask_c*)allocateMaskLayer(sizeof(hdLayerMask_c));
        if (layer == NULL) return NULL;
    }
    SafeString name;
    name.__vtbl = gabi::ea(&sLayerMaskSafeStringVtable);
    name.mStringTop = gabi::guest_addr("3DMask");
    constructMaskLayerBase(layer, &name);
    layer->mVtable = gabi::ea(&sLayerMaskLayerVtable);
    layer->mDrawVtable = gabi::ea(&sLayerMaskDrawVtable);
    return layer;
}
/* 02733A10 */
void hdLayerMask_c::initialize() {
    hdMaskRenderStep_c* step = (hdMaskRenderStep_c*)allocateMaskLayer(sizeof(hdMaskRenderStep_c));
    if (step != NULL) {
        constructMaskStep(step);
        SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdMaskRenderStep_c, mName));
        if (name == NULL) name = (SafeString*)allocateMaskLayer(sizeof(SafeString));
        if (name != NULL) {
            name->__vtbl = gabi::ea(&sLayerMaskSafeStringVtable);
            name->mStringTop = gabi::guest_addr("");
        }
        step->mFlags = 0;
        step->mVtable = gabi::ea(&sLayerMaskStepVtable);
        if (u32(gabi::ea(step)+offsetof(hdMaskRenderStep_c, mDelegateVtable)) != 0) {
            step->mpLayer = this;
            step->mDelegateAdjustment = 0x0000FFFF;
            step->mDelegateVtable = gabi::ea(&sLayerMaskDelegateVtable);
            step->mDrawFunction = gabi::guest_fn((const void*)drawLayerMask);
        }
        step->mTarget = 1;
        step->mName.mStringTop = gabi::guest_addr("Layer3DMask");
    }
    addMaskStep(this, step);
}
/* 02733DEC */
static void __sinit_hd_layer_mask_cpp() {
    for (u32 i = 0; i < 4; ++i) sLayerMaskHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sLayerMaskHeaderDescriptor));
    sLayerMaskMinRotation = -3.1415927410125732f;
    sLayerMaskMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sLayerMaskRotationGuard);
    __register_global_object(gabi::ea(&sLayerMaskRotationDescriptor));
    initializePositiveRotationGuard(&sLayerMaskPositiveRotationGuard);
    __register_global_object(gabi::ea(&sLayerMaskPositiveRotationDescriptor));
}
static hdMaskPlay_l* getMaskPlay();
static void constructMaskState(hdMaskState_c* state);
static void applyMaskState(hdMaskState_c* state);
static void calculateMaskBounds(hdMaskBounds_c* bounds, hdMaskViewport_l* viewport);
static void applyMaskViewport(hdMaskBounds_c* bounds, hdMaskViewport_l* viewport);
static void constructMaskProjection(hdMaskProjection_c* projection, hdMaskBounds_c* bounds, f32 nearPlane, f32 farPlane);
static void constructMaskCamera(hdMaskCamera_c* camera, hdMaskProjection_c* projection);
static void makeMaskQuadMatrix(hdMaskMatrix_c* matrix, cXyz* scale, cXyz* translation);
static void concatenateMaskMatrix(hdMaskCamera_c* camera, hdMaskMatrix_c* matrix, hdMaskMatrix_c* result);
static hdMaskRenderInfo_c* setupMaskRenderer(void* renderer);
static void* getMaskProjectionParameters(hdMaskProjection_c* projection);
static u32 drawMaskQuad(void* utility, hdMaskMatrix_c* matrix, void* parameters, u32 target);
static void destroyMaskCamera(hdMaskCamera_c* camera, s32 flags);
static void destroyMaskProjection(hdMaskProjection_c* projection, s32 flags);
static void resetMaskProjection(hdMaskViewport_l* viewport);
/* 02733AF8 */
static void resetMaskViewport(void*, hdMaskLayerContext_c* context) {
    resetMaskProjection(context->mpViewport);
    hdMaskViewport_l* viewport = context->mpViewport;
    hdMaskBounds_c* bounds = context->mpBounds;
    applyMaskViewport(bounds, viewport);
}
/* 02733B34 */
static void drawLayerMask(hdLayerMask_c*, hdMaskRenderInfo_c* info) {
    hdMaskPlay_l* play = getMaskPlay();
    u32 target = info->mTarget;
    void* mask = play->mpMask;
    if (target != 0 || mask == NULL) return;
    play = getMaskPlay();
    u32 height = u16(gabi::ftoi(play->mMaskHeight));
    if (height == 0) return;
    hdMaskState_c state;
    constructMaskState(&state);
    state.mEnabled = 1;
    state.mBlend = 0;
    state.mDepthMode = 7;
    applyMaskState(&state);
    u32 screen = info->mScreen;
    hdMaskScreenMgr_l* screens = sLayerMaskScreens;
    hdMaskViewport_l* viewport = &screens->mScreens[screen].mViewport;
    hdMaskBounds_c bounds;
    calculateMaskBounds(&bounds, viewport);
    applyMaskViewport(&bounds, viewport);
    hdMaskProjection_c projection;
    constructMaskProjection(&projection, &bounds, 0.0f, 1.0f);
    hdMaskCamera_c camera;
    constructMaskCamera(&camera, &projection);
    f32 barHeight = f32(height);
    f64 halfHeight = f64(height) * 0.5;
    f32 left = bounds.mLeft;
    f32 right = bounds.mRight;
    f32 width = gabi::fsubs_ppc(right, left);
    f32 top = bounds.mTop;
    f32 bottom = bounds.mBottom;
    f32 screenHeight = gabi::fsubs_ppc(top, bottom);
    cXyz scale;
    cXyz translation;
    translation.z = 0.0f;
    scale.y = barHeight;
    translation.x = 0.0f;
    f32 halfScreen = gabi::fmuls_ppc(screenHeight, 0.5f);
    scale.z = 1.0f;
    scale.x = width;
    translation.y = f32(f64(halfScreen) - halfHeight);
    hdMaskMatrix_c matrix;
    makeMaskQuadMatrix(&matrix, &scale, &translation);
    concatenateMaskMatrix(&camera, &matrix, &matrix);
    hdMaskRenderInfo_c* renderer = setupMaskRenderer(&sLayerMaskMaskRenderer);
    u32 drawTarget = renderer->mTarget;
    void* parameters = getMaskProjectionParameters(&projection);
    u32 nextTarget = drawMaskQuad(&sLayerMaskDrawUtility, &matrix, parameters, drawTarget);
    bottom = bounds.mBottom;
    top = bounds.mTop;
    screenHeight = gabi::fsubs_ppc(top, bottom);
    right = bounds.mRight;
    left = bounds.mLeft;
    halfScreen = gabi::fmuls_ppc(screenHeight, 0.5f);
    scale.y = barHeight;
    translation.z = 0.0f;
    width = gabi::fsubs_ppc(right, left);
    scale.z = 1.0f;
    scale.x = width;
    translation.x = 0.0f;
    translation.y = f32(-f64(halfScreen) + halfHeight);
    makeMaskQuadMatrix(&matrix, &scale, &translation);
    concatenateMaskMatrix(&camera, &matrix, &matrix);
    parameters = getMaskProjectionParameters(&projection);
    drawMaskQuad(&sLayerMaskDrawUtility, &matrix, parameters, nextTarget);
    destroyMaskCamera(&camera, 2);
    destroyMaskProjection(&projection, 2);
}
/* WWHD glue
 data sLayerMaskSafeStringVtable 10141FCC nocheck
 data sLayerMaskLayerVtable 10142068 nocheck
 data sLayerMaskDrawVtable 10142110 nocheck
 data sLayerMaskStepVtable 10169E3C nocheck
 data sLayerMaskDelegateVtable 10142014 nocheck
 data sLayerMaskHeaderZero 1049FA08
 data sLayerMaskHeaderDescriptor 101F8860
 data sLayerMaskRotationDescriptor 101F886C
 data sLayerMaskPositiveRotationDescriptor 101F8878
 data sLayerMaskMinRotation 1049F9FC
 data sLayerMaskMaxRotation 1049FA00
 data sLayerMaskRotationGuard 1049FA04
 data sLayerMaskPositiveRotationGuard 1049FA05
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sLayerMaskScreens 101F95D0
 data sLayerMaskMaskRenderer 104B45C0
 data sLayerMaskDrawUtility 104A01CC
 str 10142030 "3DMask"
 str 10142038 "Layer3DMask"
 str 10141FC8 "" in 02733A10
 ext 0273AD10 void* allocateMaskLayer(u32 size)
 ext 027340D8 void constructMaskLayerBase(hdLayerMask_c* layer, SafeString* name)
 ext 02752A84 void constructMaskStep(hdMaskRenderStep_c* step)
 ext 027C2450 void addMaskStep(hdLayerMask_c* layer, hdMaskRenderStep_c* step)
 ext 025200D4 hdMaskPlay_l* getMaskPlay()
 ext 0274FDBC void constructMaskState(hdMaskState_c* state)
 ext 0274FEB0 void applyMaskState(hdMaskState_c* state)
 ext 0274F5F0 void calculateMaskBounds(hdMaskBounds_c* bounds, hdMaskViewport_l* viewport)
 ext 0274D690 void resetMaskProjection(hdMaskViewport_l* viewport)
 ext 0274F964 void applyMaskViewport(hdMaskBounds_c* bounds, hdMaskViewport_l* viewport)
 ext 0274E364 void constructMaskProjection(hdMaskProjection_c* projection, hdMaskBounds_c* bounds, f32 nearPlane, f32 farPlane)
 ext 0274CE34 void constructMaskCamera(hdMaskCamera_c* camera, hdMaskProjection_c* projection)
 ext 025C0AC4 void makeMaskQuadMatrix(hdMaskMatrix_c* matrix, cXyz* scale, cXyz* translation)
 ext 028E9108 void concatenateMaskMatrix(hdMaskCamera_c* camera, hdMaskMatrix_c* matrix, hdMaskMatrix_c* result)
 ext 027F29D4 hdMaskRenderInfo_c* setupMaskRenderer(void* renderer)
 ext 0274D80C void* getMaskProjectionParameters(hdMaskProjection_c* projection)
 ext 027AAABC u32 drawMaskQuad(void* utility, hdMaskMatrix_c* matrix, void* parameters, u32 target)
 ext 0274CF04 void destroyMaskCamera(hdMaskCamera_c* camera, s32 flags)
 ext 0274E474 void destroyMaskProjection(hdMaskProjection_c* projection, s32 flags)
*/
