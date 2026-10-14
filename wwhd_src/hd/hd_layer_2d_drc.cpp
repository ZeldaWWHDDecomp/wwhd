/** HD UI_DRC render layer. Private decomp source. */
#include "hd/hd_layer_2d.h"
#include "hd/hd_graphics_task.h"
static u8 shdLayer2DDRC_cVtable;
static u8 shdLayer2DDRC_cInterfaceVtable;
static u8 shdLayer2DDRC_cNameVtable;
static u8 shdLayer2DDRC_cListenerVtable;
static u8 shdLayer2DDRC_cListenerInterface;
static u8 shdLayer2DDRC_cDelegateVtable;
static u8 shdLayer2DDRC_cActiveListener;
static u8 shdLayer2DDRC_cDisposerVtable;
static hdLayer2D_c* constructLayer2D(hdLayer2D_c*, SafeString*);
static void configureLayer2D(hdLayer2D_c*);
static void constructLayerListener(hdFadeListener_l*);
static void attachLayerListener(hdLayer2D_c*, hdFadeListener_l*);
struct hdLayerDrawMode_l { be<u32> mPass; be<u32> mDisplay; };
static hdGraphicsTask_c* sDRCRenderSystem;
static void* sDRCDrawDevice;
static u8 sDRCDrawModeOwner;
static hdLayerDrawMode_l* acquireDRCDrawMode(void*);
static void setDRCViewport(void*);
static void setDRCRenderBuffer(void*, void*);
static void drawDRC(void*, u32, hdLayerRenderInfo_c*, u32);
static u8 sDRCBaseInterface;
static void destroyDRCContext(hdRenderContext_l*, u32);
static void destroyDRCCamera(hdCamera_l*, u32);
static void destroyDRCProjection(hdProjection_l*, u32);
static void destroyDRCLayer(hdLayer2D_c*, u32);

/* 0272EA3C */
static void drawDRCLayer(hdLayer2DDRC_c* i_this, hdLayerRenderInfo_c* renderInfo) {
    u32 mode = sDRCRenderSystem->mMode;
    void* device = sDRCDrawDevice;
    if (mode <= 2) {
        hdLayerDrawMode_l* drawMode = acquireDRCDrawMode(&sDRCDrawModeOwner);
        drawMode->mPass = 3;
        drawMode->mDisplay = 0;
        setDRCViewport(filterRenderInfo(renderInfo)->mpViewport.get());
        setDRCRenderBuffer(filterRenderInfo(renderInfo)->mpViewportBounds, filterRenderInfo(renderInfo)->mpViewport.get());
        drawDRC(device, 14, renderInfo, 1);
    }
}

/* 0272EACC */
static void layerDrawCallback(hdLayer2DDRC_c* i_this, hdLayerRenderInfo_c* renderInfo) {
    if (u32(filterRenderInfo(renderInfo)->mPass) == 0) drawDRCLayer(i_this, renderInfo);
}

/* inlined into DRC layer destructors */
static void destroyDRCBase(hdLayer2DDRC_c* i_this, u32 flags) {
    if (i_this != NULL) {
        i_this->mBase.mDrawVtable = gabi::ea(&sDRCBaseInterface);
        destroyDRCContext(&i_this->mRenderContext, 2);
        destroyDRCCamera(&i_this->mCamera, 2);
        destroyDRCProjection(&i_this->mSecondaryProjection, 2);
        destroyDRCProjection(&i_this->mProjection, 2);
        destroyDRCLayer(i_this, 0);
        if (flags & 1) operator_delete(i_this);
    }
}

/* 0272E600 */
void hdLayer2DDRC_c::destroy(u32 flags) { destroyDRCBase(this, flags); }
/* 0272E78C */
void hdLayer2DDRC_c::destroyBase(u32 flags) { destroyDRCBase(this, flags); }
/* 0272E81C */
static u32 getDRCTypeCount() { return 8; }


/* 0272E520 */
u32 hdLayer2DDRC_c::getFirstParameter() { return 0; }
/* 0272E528 */
u32 hdLayer2DDRC_c::getSecondParameter() { return 0; }

/* 0272E8CC */
static hdLayer2DDRC_c* hdLayer2DDRC_c__ct(hdLayer2DDRC_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdLayer2DDRC_c*)operator_new(sizeof(hdLayer2DDRC_c));
        if (i_this == NULL) return NULL;
    }
    SafeString name;
    name.mStringTop = gabi::guest_addr("UI_DRC");
    name.__vtbl = gabi::ea(&shdLayer2DDRC_cNameVtable);
    constructLayer2D(i_this, &name);
    i_this->mBase.mpVtable = (hdLayerVtable_c*)&shdLayer2DDRC_cVtable;
    i_this->mBase.mDrawVtable = gabi::ea(&shdLayer2DDRC_cInterfaceVtable);
    return i_this;
}

/* 0272E948 */
void hdLayer2DDRC_c::initialize() {
    hdLayer2D_c::initialize();
    configureLayer2D(this);
    hdFadeListener_l* listener = (hdFadeListener_l*)operator_new(sizeof(hdFadeListener_l));
    if (listener != NULL) {
        constructLayerListener(listener);
        struct ListenerInterface_l {
            be<u32> mListener, mInterface;
        };
        ListenerInterface_l* interface = gabi::at<ListenerInterface_l>(
            gabi::ea(listener) + offsetof(hdFadeListener_l, mListenerVtable));
        if (interface == NULL)
            interface = (ListenerInterface_l*)operator_new(sizeof(ListenerInterface_l));
        if (interface != NULL) {
            interface->mInterface = gabi::ea(&shdLayer2DDRC_cListenerInterface);
            interface->mListener = gabi::ea(&shdLayer2DDRC_cListenerVtable);
        }
        listener->mUserData = 0;
        listener->mVtable = gabi::ea(&shdLayer2DDRC_cDisposerVtable);
        listener->mDelegate.mpOwner = this;
        listener->mDelegate.mAdjustment = 0;
        listener->mDelegate.mVirtualIndex = -1;
        listener->mDelegate.mVtable = gabi::ea(&shdLayer2DDRC_cDelegateVtable);
        listener->mDelegate.mCallback = gabi::guest_fn((const void*)layerDrawCallback);
        listener->mEnabled = 1;
        listener->mListenerVtable = gabi::ea(&shdLayer2DDRC_cActiveListener);
    }
    attachLayerListener(this, listener);
}

/* 0272EADC */
static void __sinit_hd_layer_2d_drc_cpp() {
    sinit_header_statics_rodata(0x1049F98C, 0x101F876C, 0x1049F998, 0x10141964);
}

/* WWHD glue
 data sDRCRenderSystem 101F86E8
 data sDRCDrawDevice 101F9968
 data sDRCDrawModeOwner 104B45C0 nocheck
 data sDRCBaseInterface 10141690 nocheck
 ext 027F29D4 hdLayerDrawMode_l* acquireDRCDrawMode(void*)
 ext 0274D690 void setDRCViewport(void*)
 ext 0274F964 void setDRCRenderBuffer(void*, void*)
 ext 027F8974 void drawDRC(void*, u32, hdLayerRenderInfo_c*, u32)
 ext 0274CCB0 void destroyDRCContext(hdRenderContext_l*, u32)
 ext 0274E038 void destroyDRCCamera(hdCamera_l*, u32)
 ext 0274E474 void destroyDRCProjection(hdProjection_l*, u32)
 ext 02734248 void destroyDRCLayer(hdLayer2D_c*, u32)
 str 10141950
 data shdLayer2DDRC_cVtable 10141970 nocheck
 data shdLayer2DDRC_cInterfaceVtable 10141A18 nocheck
 data shdLayer2DDRC_cNameVtable 101418CC nocheck
 data shdLayer2DDRC_cListenerVtable 101418B8 nocheck
 data shdLayer2DDRC_cListenerInterface 101418CC nocheck
 data shdLayer2DDRC_cDelegateVtable 10141934 nocheck
 data shdLayer2DDRC_cActiveListener 10141958 nocheck
 data shdLayer2DDRC_cDisposerVtable 10169E3C nocheck
 ext 0272D978 hdLayer2D_c* constructLayer2D(hdLayer2D_c*, SafeString*)
 ext 027343FC void configureLayer2D(hdLayer2D_c*)
 ext 02752A84 void constructLayerListener(hdFadeListener_l*)
 ext 027C2450 void attachLayerListener(hdLayer2D_c*, hdFadeListener_l*)
*/
