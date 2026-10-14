/** HD UI_Fore render layer. Private decomp source. */
#include "hd/hd_layer_2d.h"
static u8 shdLayer2DFore_cVtable;
static u8 shdLayer2DFore_cInterfaceVtable;
static u8 shdLayer2DFore_cNameVtable;
static u8 shdLayer2DFore_cListenerVtable;
static u8 shdLayer2DFore_cListenerInterface;
static u8 shdLayer2DFore_cDelegateVtable;
static u8 shdLayer2DFore_cActiveListener;
static u8 shdLayer2DFore_cDisposerVtable;
static hdLayer2D_c* constructLayer2D(hdLayer2D_c*, SafeString*);
static void configureLayer2D(hdLayer2D_c*);
static void constructLayerListener(hdFadeListener_l*);
static void attachLayerListener(hdLayer2D_c*, hdFadeListener_l*);
/* 0272F7B4 */
static void layerDrawCallback(hdLayer2DFore_c* i_this, hdLayerRenderInfo_c* renderInfo) {}
/* 0272F594 */
static u32 getForegroundTypeCount() { return 4; }


/* 0272F3D4 */
u32 hdLayer2DFore_c::getFirstParameter() { return 0; }
/* 0272F3DC */
u32 hdLayer2DFore_c::getSecondParameter() { return 0; }

/* 0272F644 */
static hdLayer2DFore_c* hdLayer2DFore_c__ct(hdLayer2DFore_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdLayer2DFore_c*)operator_new(sizeof(hdLayer2DFore_c));
        if (i_this == NULL) return NULL;
    }
    SafeString name;
    name.mStringTop = gabi::guest_addr("UI_Fore");
    name.__vtbl = gabi::ea(&shdLayer2DFore_cNameVtable);
    constructLayer2D(i_this, &name);
    i_this->mBase.mpVtable = (hdLayerVtable_c*)&shdLayer2DFore_cVtable;
    i_this->mBase.mDrawVtable = gabi::ea(&shdLayer2DFore_cInterfaceVtable);
    return i_this;
}

/* 0272F6C0 */
void hdLayer2DFore_c::initialize() {
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
            interface->mInterface = gabi::ea(&shdLayer2DFore_cListenerInterface);
            interface->mListener = gabi::ea(&shdLayer2DFore_cListenerVtable);
        }
        listener->mUserData = 0;
        listener->mVtable = gabi::ea(&shdLayer2DFore_cDisposerVtable);
        listener->mDelegate.mpOwner = this;
        listener->mDelegate.mAdjustment = 0;
        listener->mDelegate.mVirtualIndex = -1;
        listener->mDelegate.mVtable = gabi::ea(&shdLayer2DFore_cDelegateVtable);
        listener->mDelegate.mCallback = gabi::guest_fn((const void*)layerDrawCallback);
        listener->mEnabled = 2;
        listener->mListenerVtable = gabi::ea(&shdLayer2DFore_cActiveListener);
    }
    attachLayerListener(this, listener);
}

/* 0272F7B8 */
static void __sinit_hd_layer_2d_fore_cpp() {
    sinit_header_statics_rodata(0x1049F9C4, 0x101F87CC, 0x1049F9D0, 0x10141C88);
}

static u8 sForegroundProjectionInterface;
static u8 sForegroundBaseInterface;
static void destroyForegroundProjection(hdProjection_l*, u32);
static void destroyForegroundContext(hdRenderContext_l*, u32);
static void destroyForegroundCamera(hdCamera_l*, u32);
static void destroyForegroundLayer(hdLayer2D_c*, u32);

/* 0272F4EC */
void hdLayer2DDRCFore_c::destroy(u32 flags) {
    if (this != NULL) {
        mBase.mDrawVtable = gabi::ea(&sForegroundProjectionInterface);
        destroyForegroundProjection(&mParticleProjection, 2);
        mBase.mDrawVtable = gabi::ea(&sForegroundBaseInterface);
        destroyForegroundContext(&mRenderContext, 2);
        destroyForegroundCamera(&mCamera, 2);
        destroyForegroundProjection(&mSecondaryProjection, 2);
        destroyForegroundProjection(&mProjection, 2);
        destroyForegroundLayer(this, 0);
        if (flags & 1) operator_delete(this);
    }
}

/* WWHD glue
 data sForegroundProjectionInterface 10141BD0 nocheck
 data sForegroundBaseInterface 10141A7C nocheck
 ext 0274E474 void destroyForegroundProjection(hdProjection_l*, u32)
 ext 0274CCB0 void destroyForegroundContext(hdRenderContext_l*, u32)
 ext 0274E038 void destroyForegroundCamera(hdCamera_l*, u32)
 ext 02734248 void destroyForegroundLayer(hdLayer2D_c*, u32)
 str 10141C74
 data shdLayer2DFore_cVtable 10141C90 nocheck
 data shdLayer2DFore_cInterfaceVtable 10141D38 nocheck
 data shdLayer2DFore_cNameVtable 10141BFC nocheck
 data shdLayer2DFore_cListenerVtable 10141BF0 nocheck
 data shdLayer2DFore_cListenerInterface 10141BFC nocheck
 data shdLayer2DFore_cDelegateVtable 10141C64 nocheck
 data shdLayer2DFore_cActiveListener 10141C7C nocheck
 data shdLayer2DFore_cDisposerVtable 10169E3C nocheck
 ext 0272D978 hdLayer2D_c* constructLayer2D(hdLayer2D_c*, SafeString*)
 ext 027343FC void configureLayer2D(hdLayer2D_c*)
 ext 02752A84 void constructLayerListener(hdFadeListener_l*)
 ext 027C2450 void attachLayerListener(hdLayer2D_c*, hdFadeListener_l*)
*/
