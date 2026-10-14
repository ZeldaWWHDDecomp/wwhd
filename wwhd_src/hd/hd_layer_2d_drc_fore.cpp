/** DRC foreground layer: orthographic projection and dedicated particle passes. */
#include "hd/hd_layer_2d.h"
static u8 sDRCForeVtable;
static u8 sDRCForeInterface;
static u8 sDRCForeNameVtable;
static u8 sDRCForeBaseInterface;
static u8 sDRCForeListenerVtable;
static u8 sDRCForeListenerInterface;
static u8 sDRCForeDelegateVtable;
static u8 sDRCForeActiveListener;
static u8 sDRCForeDisposerVtable;
static void* sDRCForeParticleManager;
static hdLayer2D_c* constructDRCForeLayer(hdLayer2D_c*, SafeString*);
static void constructDRCForeProjection(hdProjection_l*);
static void configureDRCForeLayer(hdLayer2D_c*);
static void setDRCForeProjectionBounds(hdProjection_l*, f32, f32, f32, f32);
static void constructDRCForeListener(hdFadeListener_l*);
static void attachDRCForeListener(hdLayer2D_c*, hdFadeListener_l*);
static void drawDRCForeCallback(hdLayer2DDRCFore_c*, void*);
static void destroyDRCForeContext(hdRenderContext_l*, u32);
static void destroyDRCForeCamera(hdCamera_l*, u32);
static void destroyDRCForeProjection(hdProjection_l*, u32);
static void destroyDRCForeLayer(hdLayer2D_c*, u32);
static hdLayerPlay_l* getDRCForePlay();
static void setDRCForeParticleMode(u32);
static void* getDRCForeProjectionMatrix(hdProjection_l*);
static void copyDRCForeProjectionMatrix(void*, be<u32>*);
static u32 getDRCForeWidth();
static u32 getDRCForeHeight();
static void prepareDRCForeParticles(void*, hdLayerParticleDrawParams_l*, u32);
static void drawDRCForeParticles(void*, hdLayerParticleDrawParams_l*, u32);
static void resetDRCForeGraphics(u32, u32, u32, u32, u32, u32, u32);

/* 0272EBE4 */
u32 hdLayer2DDRCFore_c::getFirstParameter() { return 0; }
/* 0272EBEC */
u32 hdLayer2DDRCFore_c::getSecondParameter() { return 0; }
/* 0272ED8C */
static u32 getDRCForegroundTypeCount() { return 4; }

/* 0272ECFC */
static void hdLayer2DDRCFore_destroyBase(hdLayer2D_c* i_this, u32 flags) {
    if (i_this != NULL) {
        i_this->mBase.mDrawVtable = gabi::ea(&sDRCForeBaseInterface);
        destroyDRCForeContext(&i_this->mRenderContext, 2);
        destroyDRCForeCamera(&i_this->mCamera, 2);
        destroyDRCForeProjection(&i_this->mSecondaryProjection, 2);
        destroyDRCForeProjection(&i_this->mProjection, 2);
        destroyDRCForeLayer(i_this, 0);
        if (flags & 1) operator_delete(i_this);
    }
}

/* 0272EE3C */
static hdLayer2DDRCFore_c* hdLayer2DDRCFore_construct(hdLayer2DDRCFore_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdLayer2DDRCFore_c*)operator_new(sizeof(hdLayer2DDRCFore_c));
        if (i_this == NULL) return NULL;
    }
    SafeString name;
    name.mStringTop = gabi::guest_addr("UI_DRCFore");
    name.__vtbl = gabi::ea(&sDRCForeNameVtable);
    constructDRCForeLayer(i_this, &name);
    i_this->mBase.mpVtable = (hdLayerVtable_c*)&sDRCForeVtable;
    i_this->mBase.mDrawVtable = gabi::ea(&sDRCForeInterface);
    constructDRCForeProjection(&i_this->mParticleProjection);
    return i_this;
}

/* 0272EEC0 */
void hdLayer2DDRCFore_c::initialize() {
    hdLayer2D_c::initialize();
    mParticleProjection.mNear = 0.0f;
    mParticleProjection.mDirty = 1;
    mParticleProjection.mFar = 300.0f;
    setDRCForeProjectionBounds(&mParticleProjection, -360.0f, 360.0f, -640.0f, 640.0f);
    configureDRCForeLayer(this);
    hdFadeListener_l* listener = (hdFadeListener_l*)operator_new(sizeof(hdFadeListener_l));
    if (listener != NULL) {
        constructDRCForeListener(listener);
        struct ListenerInterface_l {
            be<u32> mListener, mInterface;
        };
        ListenerInterface_l* interface = gabi::at<ListenerInterface_l>(
            gabi::ea(listener) + offsetof(hdFadeListener_l, mListenerVtable));
        if (interface == NULL)
            interface = (ListenerInterface_l*)operator_new(sizeof(ListenerInterface_l));
        if (interface != NULL) {
            interface->mInterface = gabi::ea(&sDRCForeListenerInterface);
            interface->mListener = gabi::ea(&sDRCForeListenerVtable);
        }
        listener->mUserData = 0;
        listener->mVtable = gabi::ea(&sDRCForeDisposerVtable);
        listener->mDelegate.mpOwner = this;
        listener->mDelegate.mAdjustment = 0;
        listener->mDelegate.mVirtualIndex = -1;
        listener->mDelegate.mVtable = gabi::ea(&sDRCForeDelegateVtable);
        listener->mDelegate.mCallback = gabi::guest_fn((const void*)drawDRCForeCallback);
        listener->mEnabled = 2;
        listener->mListenerVtable = gabi::ea(&sDRCForeActiveListener);
    }
    attachDRCForeListener(this, listener);
}

/* 0272EFFC */
void hdLayer2DDRCFore_c::drawParticleBackground() {
    s32 view = getDRCForePlay()->mCurrentView;
    if (getDRCForePlay()->mViews[view].mpView != NULL) {
        setDRCForeParticleMode(1);
        Mtx34 transform;
        PSMTXTrans(&transform, 0.0f, 0.0f, 0.0f);
        hdLayerParticleDrawParams_l params;
        params.mScale = 0.0f;
        params.mFlags = 0;
        params.mAspect = 1.218000054359436f;
        params.mpTransform = &transform;
        f32 width = f32(getDRCForeWidth());
        f32 height = f32(getDRCForeHeight());
        params.mAspect = width / height;
        void* projection = getDRCForeProjectionMatrix(&mParticleProjection);
        copyDRCForeProjectionMatrix(projection, params.mProjection);
        if (getDRCForePlay()->mpParticles != NULL) {
            void* particles = getDRCForePlay()->mpParticles;
            prepareDRCForeParticles(sDRCForeParticleManager, &params, 12);
            drawDRCForeParticles(particles, &params, 12);
        }
    }
}

/* 0272F144 */
void hdLayer2DDRCFore_c::drawParticleForeground() {
    s32 view = getDRCForePlay()->mCurrentView;
    if (getDRCForePlay()->mViews[view].mpView != NULL) {
        setDRCForeParticleMode(1);
        Mtx34 transform;
        PSMTXTrans(&transform, 0.0f, 0.0f, 0.0f);
        hdLayerParticleDrawParams_l params;
        params.mScale = 0.0f;
        params.mpTransform = &transform;
        params.mFlags = 0;
        params.mAspect = 1.218000054359436f;
        f32 width = f32(getDRCForeWidth());
        f32 height = f32(getDRCForeHeight());
        params.mAspect = width / height;
        void* projection = getDRCForeProjectionMatrix(&mParticleProjection);
        copyDRCForeProjectionMatrix(projection, params.mProjection);
        if (getDRCForePlay()->mpParticles != NULL) {
            void* particles = getDRCForePlay()->mpParticles;
            prepareDRCForeParticles(sDRCForeParticleManager, &params, 11);
            drawDRCForeParticles(particles, &params, 11);
        }
        resetDRCForeGraphics(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    }
}

/* 0272F2AC */
void hdLayer2DDRCFore_c::drawParticles(void* renderInfo) {
    switch (u32(filterRenderInfo(static_cast<hdLayerRenderInfo_c*>(renderInfo))->mPass)) {
    case 0: drawParticleBackground(); break;
    case 3: drawParticleForeground(); break;
    }
}

/* 0272F2CC */
static void __sinit_hd_layer_2d_drc_fore_cpp() {
    sinit_header_statics_rodata(0x1049F9A8, 0x101F87A0, 0x1049F9B4, 0x10141B20);
}

/* WWHD glue
 data sDRCForeVtable 10141B28 nocheck
 data sDRCForeInterface 10141BD0 nocheck
 data sDRCForeNameVtable 10141A44 nocheck
 data sDRCForeBaseInterface 10141904 nocheck
 data sDRCForeListenerVtable 10141A38 nocheck
 data sDRCForeListenerInterface 10141A44 nocheck
 data sDRCForeDelegateVtable 10141AAC nocheck
 data sDRCForeActiveListener 10141B00 nocheck
 data sDRCForeDisposerVtable 10169E3C nocheck
 data sDRCForeParticleManager 1047B2D4
 str 10141ADC
 ext 0272D978 hdLayer2D_c* constructDRCForeLayer(hdLayer2D_c*, SafeString*)
 ext 0274E1C8 void constructDRCForeProjection(hdProjection_l*)
 ext 027343FC void configureDRCForeLayer(hdLayer2D_c*)
 ext 0274E488 void setDRCForeProjectionBounds(hdProjection_l*, f32, f32, f32, f32)
 ext 02752A84 void constructDRCForeListener(hdFadeListener_l*)
 ext 027C2450 void attachDRCForeListener(hdLayer2D_c*, hdFadeListener_l*)
 ext 0272F2AC void drawDRCForeCallback(hdLayer2DDRCFore_c*, void*) callback
 ext 0274CCB0 void destroyDRCForeContext(hdRenderContext_l*, u32)
 ext 0274E038 void destroyDRCForeCamera(hdCamera_l*, u32)
 ext 0274E474 void destroyDRCForeProjection(hdProjection_l*, u32)
 ext 02734248 void destroyDRCForeLayer(hdLayer2D_c*, u32)
 ext 025200D4 hdLayerPlay_l* getDRCForePlay()
 ext 027B9F68 void setDRCForeParticleMode(u32)
 ext 0274D80C void* getDRCForeProjectionMatrix(hdProjection_l*)
 ext 028E8970 void copyDRCForeProjectionMatrix(void*, be<u32>*)
 ext 02739594 u32 getDRCForeWidth()
 ext 027395E4 u32 getDRCForeHeight()
 ext 02821734 void prepareDRCForeParticles(void*, hdLayerParticleDrawParams_l*, u32)
 ext 025A83AC void drawDRCForeParticles(void*, hdLayerParticleDrawParams_l*, u32)
 ext C0006988 void resetDRCForeGraphics(u32, u32, u32, u32, u32, u32, u32)
*/
