/** HD 2D layer projection and camera setup. Library rendering operations stay external. */
#include "hd/hd_layer_2d.h"
#include "hd/hd_graphics_task.h"
static u8 sLayer2DBaseVtable;
static u8 sLayer2DBaseInterface;
static u8 sLayer2DInitialContextVtable;
static hdRenderContextVtable_l sLayer2DContextVtable;
struct hdLayerDefaultMatrix_l { be<u32> mWords[12]; };
static hdLayerDefaultMatrix_l sLayer2DDefaultMatrix;
struct hdProjectionScale_l { be<f32> mWidthBase, mWidthSlope, mHeightBase, mHeightSlope; };
static hdProjectionScale_l sLayer2DProjectionScale;
struct hdProjectionRect_l { be<f32> mLeft, mBottom, mRight, mTop; };
WWHD_SIZE(hdProjectionRect_l, 16);
static hdLayer2D_c* constructLayerBase(hdLayer2D_c*, SafeString*);
static void constructOrthoProjection(hdProjection_l*);
static void constructPerspectiveProjection(hdCamera_l*);
static void setProjectionRect(hdProjection_l*, hdProjectionRect_l*);
static void setProjectionBounds(hdProjection_l*, f32, f32, f32, f32);
static void setPerspectiveBounds(hdCamera_l*, f32, f32, f32, f32);
static void normalizeLayerUp(cXyz*);

/* 0272D978 */
static hdLayer2D_c* hdLayer2D_construct(hdLayer2D_c* i_this, SafeString* name) {
    if (i_this == NULL) {
        i_this = (hdLayer2D_c*)operator_new(sizeof(hdLayer2D_c));
        if (i_this == NULL)
            return NULL;
    }
    constructLayerBase(i_this, name);
    i_this->mBase.mpVtable = (hdLayerVtable_c*)&sLayer2DBaseVtable;
    i_this->mBase.mDrawVtable = gabi::ea(&sLayer2DBaseInterface);
    constructOrthoProjection(
        gabi::at<hdProjection_l>(gabi::ea(i_this) + offsetof(hdLayer2D_c, mProjection)));
    constructOrthoProjection(
        gabi::at<hdProjection_l>(gabi::ea(i_this) + offsetof(hdLayer2D_c, mSecondaryProjection)));
    constructPerspectiveProjection(
        gabi::at<hdCamera_l>(gabi::ea(i_this) + offsetof(hdLayer2D_c, mCamera)));
    hdRenderContext_l* context =
        gabi::at<hdRenderContext_l>(gabi::ea(i_this) + offsetof(hdLayer2D_c, mRenderContext));
    context->mpVtable = (hdRenderContextVtable_l*)&sLayer2DInitialContextVtable;
    for (u32 i = 0; i < 12; i++)
        context->mMatrix[i] = sLayer2DDefaultMatrix.mWords[i];
    context->mpVtable = &sLayer2DContextVtable;
    const u32 contextAddress = gabi::ea(context);
    cXyz* eye = gabi::at<cXyz>(contextAddress + offsetof(hdRenderContext_l, mEye));
    if (eye == NULL)
        eye = (cXyz*)operator_new(sizeof(cXyz));
    if (eye != NULL) {
        eye->y = 0.0f;
        eye->x = 0.0f;
        eye->z = 10.0f;
    }
    cXyz* target = gabi::at<cXyz>(contextAddress + offsetof(hdRenderContext_l, mTarget));
    if (target == NULL)
        target = (cXyz*)operator_new(sizeof(cXyz));
    if (target != NULL) {
        target->y = 0.0f;
        target->x = 0.0f;
        target->z = 0.0f;
    }
    cXyz* up = gabi::at<cXyz>(contextAddress + offsetof(hdRenderContext_l, mUp));
    if (up == NULL)
        up = (cXyz*)operator_new(sizeof(cXyz));
    if (up != NULL) {
        up->y = 1.0f;
        up->x = 0.0f;
        up->z = 0.0f;
    }
    context->mHeight = 1.0f;
    context->mPerspective = 1;
    return i_this;
}

/* 0272DB10 */
void hdLayer2D_c::initialize() {
    hdProjectionRect_l rect;
    rect.mLeft = -640.0f;
    rect.mRight = 640.0f;
    rect.mBottom = -360.0f;
    rect.mTop = 360.0f;
    mProjection.mFar = 300.0f;
    mProjection.mDirty = 1;
    mProjection.mNear = 0.0f;
    setProjectionRect(&mProjection, &rect);
    mSecondaryProjection.mNear = 0.0f;
    mSecondaryProjection.mDirty = 1;
    mSecondaryProjection.mFar = 300.0f;
    setProjectionBounds(&mSecondaryProjection, -360.0f, 360.0f, -640.0f, 640.0f);
    setPerspectiveBounds(&mCamera, 1.0f, 10000.0f, 0.6981316804885864f, 1.7777777910232544f);
    f32 width = fmadds(sLayer2DProjectionScale.mWidthSlope, 0.22222232818603516f, sLayer2DProjectionScale.mWidthBase);
    f32 height = fmadds(sLayer2DProjectionScale.mHeightSlope, 0.22222232818603516f, sLayer2DProjectionScale.mHeightBase);
    f32 eyeZ = 360.0f / (width / height);
    mRenderContext.mHeight = eyeZ;
    f32 z = mRenderContext.mPerspective != 0 ? eyeZ : 1.0f;
    mRenderContext.mUp.z = 0.0f;
    cXyz eye(0.0f, 0.0f, z);
    cXyz up(0.0f, 1.0f, 0.0f);
    cXyz target(0.0f, 0.0f, 0.0f);
    mRenderContext.mUp.x = 0.0f;
    mRenderContext.mEye.z = z;
    mRenderContext.mUp.y = 1.0f;
    mRenderContext.mEye.x = 0.0f;
    mRenderContext.mEye.y = 0.0f;
    normalizeLayerUp(&mRenderContext.mUp);
    mRenderContext.mTarget.z = target.z;
    mRenderContext.mTarget.y = target.y;
    mRenderContext.mTarget.x = target.x;
    hdRenderContext_l* context = &mRenderContext;
    gabi::call_ptr(context->mpVtable->mUpdate.get(), context, context);
    if (mRenderContext.mPerspective != 0) {
        mBase.mpProjection = (hdLayerProjection_c*)&mCamera;
        mBase.mpViewMatrix = (hdLayerMatrix_c*)context;
    } else {
        mBase.mpViewMatrix = (hdLayerMatrix_c*)context;
        mBase.mpProjection = (hdLayerProjection_c*)&mProjection;
    }
}

/* 0272E418 */
static void __sinit_hd_layer_2d_cpp() {
    sinit_header_statics_rodata(0x1049F970, 0x101F8738, 0x1049F97C, 0x101417E4);
}

static u8 sUIVtable;
static u8 sUIInterface;
static u8 sUINameVtable;
static u8 sUIListenerVtable;
static u8 sUIListenerInterface;
static u8 sUIDelegateVtable;
static u8 sUIActiveListener;
static u8 sUIDisposerVtable;
static hdLayer2D_c* constructUILayer(hdLayer2D_c*, SafeString*);
static void configureUILayer(hdLayer2D_c*);
static void constructUIListener(hdFadeListener_l*);
static void attachUIListener(hdLayer2D_c*, hdFadeListener_l*);
static void drawUICallback(hdLayer2DUI_c*, hdLayerRenderInfo_c*);
static hdLayerPlay_l* getUIPlay();
static void* sUIParticleManager;
static void setUIParticleMode(u32);
static void* getUIProjectionMatrix(hdProjection_l*);
static void copyUIProjectionMatrix(void*, be<u32>*);
static u32 isUIParticleSuppressed();
static void prepareUIParticles(void*, hdLayerParticleDrawParams_l*, u32);
static void drawUIParticles(void*, hdLayerParticleDrawParams_l*, u32);
static void resetUIGraphics(u32, u32, u32, u32, u32, u32, u32);
static void setUIViewport(void*);
static void setUIRenderBuffer(void*, void*);
struct hdUIDrawMode_l { be<u32> mPass, mDisplay; };
static u8 sUIDrawModeOwner;
static hdUIDrawMode_l* acquireUIDrawMode(void*);
static hdGraphicsTask_c* sUIRenderSystem;
static void* sUIDrawDevice;
struct hdUIDrawParams_l {
    gptr<hdLayerRenderInfo_c> mpRenderInfo;
    be<u32> mEnabled;
    be<u32> mReserved;
    be<u32> mMode;
    be<u32> mParameters[9];
};
WWHD_SIZE(hdUIDrawParams_l, 0x34);
struct hdUIDrawModeOwner_l { u8 mReserved[0x18C]; u8 mRenderState[4]; };
static hdUIDrawModeOwner_l sUIDrawOwner;
static void prepareUIRenderState(void*);
static void drawUIRenderer(void*, hdUIDrawParams_l*);
static void drawUIScreen(void*, u32, hdLayerRenderInfo_c*);
static u32 queryUIRenderer(void*);
static void prepareUIDrawCommand(hdLayerUIDrawState_l*, void*, void*);
struct hdUIViewportVtable_l { u8 mReserved[0x2C]; be<u32> mClear; };
struct hdUIViewport_l { u8 mReserved[0x18]; gptr<hdUIViewportVtable_l> mpVtable; };

/* 0272DD2C */
static hdLayer2DUI_c* hdLayer2DUI_construct(hdLayer2DUI_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdLayer2DUI_c*)operator_new(sizeof(hdLayer2DUI_c));
        if (i_this == NULL) return NULL;
    }
    SafeString name;
    name.__vtbl = gabi::ea(&sUINameVtable);
    name.mStringTop = gabi::guest_addr("UI");
    constructUILayer(i_this, &name);
    i_this->mBase.mpVtable = (hdLayerVtable_c*)&sUIVtable;
    i_this->mBase.mDrawVtable = gabi::ea(&sUIInterface);
    return i_this;
}

/* 0272DDA8 */
void hdLayer2DUI_c::initialize() {
    hdLayer2D_c::initialize();
    configureUILayer(this);
    hdFadeListener_l* listener = (hdFadeListener_l*)operator_new(sizeof(hdFadeListener_l));
    if (listener != NULL) {
        constructUIListener(listener);
        struct ListenerInterface_l {
            be<u32> mListener, mInterface;
        };
        ListenerInterface_l* interface = gabi::at<ListenerInterface_l>(
            gabi::ea(listener) + offsetof(hdFadeListener_l, mListenerVtable));
        if (interface == NULL)
            interface = (ListenerInterface_l*)operator_new(sizeof(ListenerInterface_l));
        if (interface != NULL) {
            interface->mInterface = gabi::ea(&sUIListenerInterface);
            interface->mListener = gabi::ea(&sUIListenerVtable);
        }
        listener->mUserData = 0;
        listener->mVtable = gabi::ea(&sUIDisposerVtable);
        listener->mDelegate.mpOwner = this;
        listener->mDelegate.mAdjustment = 0;
        listener->mDelegate.mVirtualIndex = -1;
        listener->mDelegate.mVtable = gabi::ea(&sUIDelegateVtable);
        listener->mDelegate.mCallback = gabi::guest_fn((const void*)drawUICallback);
        listener->mEnabled = 1;
        listener->mListenerVtable = gabi::ea(&sUIActiveListener);
    }
    attachUIListener(this, listener);
}

/* 0272DE9C */
void hdLayer2DUI_c::drawUI(hdLayerRenderInfo_c* renderInfo) {
    hdUIDrawParams_l params;
    params.mpRenderInfo = renderInfo;
    params.mReserved = 0;
    params.mMode = 2;
    params.mEnabled = 1;
    for (u32 i = 0; i < 9; i++) params.mParameters[i] = 0;
    hdUIDrawMode_l* mode = acquireUIDrawMode(&sUIDrawModeOwner);
    mode->mDisplay = 0;
    mode->mPass = 3;
    prepareUIRenderState(sUIDrawOwner.mRenderState);
    drawUIRenderer(getUIPlay()->mUIDrawState.mpUIRenderer, &params);
}

/* inlined into UI particle passes */
static void drawUILayerParticles(hdLayer2DUI_c* layer, hdLayerRenderInfo_c* renderInfo, bool foreground) {
    s32 view = getUIPlay()->mCurrentView;
    if (getUIPlay()->mViews[view].mpView == NULL) return;
    if (foreground) {
        hdUIDrawMode_l* mode = acquireUIDrawMode(&sUIDrawModeOwner);
        mode->mPass = 3;
        mode->mDisplay = 0;
    }
    setUIViewport(filterRenderInfo(renderInfo)->mpViewport.get());
    setUIRenderBuffer(filterRenderInfo(renderInfo)->mpViewportBounds, filterRenderInfo(renderInfo)->mpViewport.get());
    setUIParticleMode(1);
    Mtx34 transform;
    PSMTXTrans(&transform, 0.0f, 0.0f, 0.0f);
    hdLayerParticleDrawParams_l params;
    params.mScale = 0.0f;
    params.mpTransform = &transform;
    params.mFlags = 0;
    params.mAspect = 1.7777777910232544f;
    void* projection = getUIProjectionMatrix(&layer->mSecondaryProjection);
    copyUIProjectionMatrix(projection, params.mProjection);
    if (isUIParticleSuppressed() == 0 && getUIPlay()->mpParticles != NULL) {
        void* particles = getUIPlay()->mpParticles;
        u32 pass = foreground ? 7 : 8;
        prepareUIParticles(sUIParticleManager, &params, pass);
        drawUIParticles(particles, &params, pass);
    }
    if (getUIPlay()->mpParticles != NULL) {
        void* particles = getUIPlay()->mpParticles;
        u32 pass = foreground ? 9 : 10;
        prepareUIParticles(sUIParticleManager, &params, pass);
        drawUIParticles(particles, &params, pass);
    }
    if (foreground) resetUIGraphics(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
}

/* 0272DF3C */
void hdLayer2DUI_c::drawParticleBackground(hdLayerRenderInfo_c* renderInfo) {
    drawUILayerParticles(this, renderInfo, false);
}

/* 0272E09C */
void hdLayer2DUI_c::drawParticleForeground(hdLayerRenderInfo_c* renderInfo) {
    drawUILayerParticles(this, renderInfo, true);
}

/* 0272E234 */
void hdLayer2DUI_c::drawScreen(hdLayerRenderInfo_c* renderInfo) {
    u32 mode = sUIRenderSystem->mMode;
    void* device = sUIDrawDevice;
    if (mode == 0) {
        hdUIDrawMode_l* drawMode = acquireUIDrawMode(&sUIDrawModeOwner);
        drawMode->mPass = 3;
        drawMode->mDisplay = 0;
        drawUIScreen(device, 14, renderInfo);
    }
}

/* 0272E2AC */
void hdLayer2DUI_c::draw(hdLayerRenderInfo_c* renderInfo) {
    acquireUIDrawMode(&sUIDrawModeOwner);
    switch (u32(filterRenderInfo(renderInfo)->mPass)) {
    case 1: {
        void* renderer = getUIPlay()->mUIDrawState.mpUIRenderer;
        if (renderer == NULL || queryUIRenderer(renderer) != 0) return;
        hdFadeColor_l color;
        hdUIViewport_l* viewport = reinterpret_cast<hdUIViewport_l*>(filterRenderInfo(renderInfo)->mpViewport.get());
        color.mGreen = 0.0f;
        color.mAlpha = 1.0f;
        color.mRed = 0.0f;
        color.mBlue = 0.0f;
        gabi::call_ptr(viewport->mpVtable->mClear.get(), viewport, 2u, &color, 0u, 1.0f);
        drawUI(renderInfo);
        break;
    }
    case 2:
        drawParticleBackground(renderInfo);
        break;
    case 6: {
        hdLayerPlay_l* play = getUIPlay();
        void* data = play->mpDrawData;
        prepareUIDrawCommand(&play->mUIDrawState, play->mDrawCommand, data);
        drawParticleForeground(renderInfo);
        break;
    }
    case 7:
        drawScreen(renderInfo);
        break;
    }
}

/* WWHD glue
 data sUIVtable 101417F0 nocheck
 data sUIInterface 10141898 nocheck
 data sUINameVtable 10141658 nocheck
 data sUIListenerVtable 10141644 nocheck
 data sUIListenerInterface 10141658 nocheck
 data sUIDelegateVtable 101416C0 nocheck
 data sUIActiveListener 101417DC nocheck
 data sUIDisposerVtable 10169E3C nocheck
 data sUIParticleManager 1047B2D4
 data sUIDrawModeOwner 104B45C0 nocheck
 data sUIDrawOwner 104B45C0
 data sUIRenderSystem 101F86E8
 data sUIDrawDevice 101F9968
 str 101417D8
 ext 0272D978 hdLayer2D_c* constructUILayer(hdLayer2D_c*, SafeString*)
 ext 027343FC void configureUILayer(hdLayer2D_c*)
 ext 02752A84 void constructUIListener(hdFadeListener_l*)
 ext 027C2450 void attachUIListener(hdLayer2D_c*, hdFadeListener_l*)
 ext 0272E2AC void drawUICallback(hdLayer2DUI_c*, hdLayerRenderInfo_c*) callback
 ext 025200D4 hdLayerPlay_l* getUIPlay()
 ext 027B9F68 void setUIParticleMode(u32)
 ext 0274D80C void* getUIProjectionMatrix(hdProjection_l*)
 ext 028E8970 void copyUIProjectionMatrix(void*, be<u32>*)
 ext 025986BC u32 isUIParticleSuppressed()
 ext 02821734 void prepareUIParticles(void*, hdLayerParticleDrawParams_l*, u32)
 ext 025A83AC void drawUIParticles(void*, hdLayerParticleDrawParams_l*, u32)
 ext C0006988 void resetUIGraphics(u32, u32, u32, u32, u32, u32, u32)
 ext 0274D690 void setUIViewport(void*)
 ext 0274F964 void setUIRenderBuffer(void*, void*)
 ext 027F29D4 hdUIDrawMode_l* acquireUIDrawMode(void*)
 ext 02750370 void prepareUIRenderState(void*)
 ext 027F1088 void drawUIRenderer(void*, hdUIDrawParams_l*)
 ext 027F869C void drawUIScreen(void*, u32, hdLayerRenderInfo_c*)
 ext 027F1210 u32 queryUIRenderer(void*)
 ext 0252F480 void prepareUIDrawCommand(hdLayerUIDrawState_l*, void*, void*)
 data sLayer2DBaseVtable 10141708 nocheck
 data sLayer2DBaseInterface 10141690 nocheck
 data sLayer2DInitialContextVtable 101450D0 nocheck
 data sLayer2DContextVtable 101450F8
 data sLayer2DDefaultMatrix 104A041C
 data sLayer2DProjectionScale 10146080
 ext 027340D8 hdLayer2D_c* constructLayerBase(hdLayer2D_c*, SafeString*)
 ext 0274E1C8 void constructOrthoProjection(hdProjection_l*)
 ext 0274DEA8 void constructPerspectiveProjection(hdCamera_l*)
 ext 0274E4A4 void setProjectionRect(hdProjection_l*, hdProjectionRect_l*)
 ext 0274E488 void setProjectionBounds(hdProjection_l*, f32, f32, f32, f32)
 ext 0274E04C void setPerspectiveBounds(hdCamera_l*, f32, f32, f32, f32)
 ext 025155D8 void normalizeLayerUp(cXyz*)
*/
