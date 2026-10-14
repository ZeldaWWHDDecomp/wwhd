/** ColorFader: registers HD fade processing with the scene and foreground render passes. */
#include "hd/hd_color_fader.h"
#include "hd/hd_render_resources.h"
#include "m_Do/m_Do_graphic.h"
static u8 sColorListenerVtable;
static u8 sColorListenerInterfaceVtable;
static u8 sColorDisposerVtable;
static u8 sColorEmptyDelegateVtable;
static u8 sColorDelegateVtable;
static u8 sColorBoundDelegateVtable;
static u8 sColorActiveListenerVtable;
static hdRenderLayerRegistry_l* sColorRenderLayers;
static void constructFadeDisposer(hdFadeListener_l*);
static void destroyFadeListener(hdFadeListener_l*, u32);
static void registerFadeListener(hdLayerBase_c*, u32, hdFadeListener_l*);
static hdFadeColorBytes_l sColorFadeColor;
static void convertFadeColor(hdFadeColor_l*, hdFadeColorBytes_l*);
static void drawColorFade(hdFadeColor_l*, void*);

/* inlined into hdColorFader_construct */
static void constructFadeListener(hdFadeListener_l* listener) {
    constructFadeDisposer(listener);
    const u32 address = gabi::ea(listener);
    struct ListenerInterface_l {
        be<u32> mListener, mInterface;
    };
    ListenerInterface_l* interface =
        gabi::at<ListenerInterface_l>(address + offsetof(hdFadeListener_l, mListenerVtable));
    if (interface == NULL)
        interface = (ListenerInterface_l*)operator_new(sizeof(ListenerInterface_l));
    if (interface != NULL) {
        interface->mListener = gabi::ea(&sColorListenerVtable);
        interface->mInterface = gabi::ea(&sColorListenerInterfaceVtable);
    }
    listener->mEnabled = 0;
    listener->mVtable =
        gabi::ea(&sColorDisposerVtable);
    listener->mUserData = 0;
    hdFadeDelegate_l* delegate =
        gabi::at<hdFadeDelegate_l>(address + offsetof(hdFadeListener_l, mDelegate));
    if (delegate != NULL) {
        delegate->mCallback = 0;
        delegate->mVtable = gabi::ea(&sColorEmptyDelegateVtable);
        delegate->mAdjustment = 0;
        delegate->mVirtualIndex = 0;
        delegate->mpOwner = NULL;
        delegate->mVtable = gabi::ea(&sColorDelegateVtable);
    }
}

/* 02728030 */
static hdColorFader_c* hdColorFader_construct(hdColorFader_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdColorFader_c*)operator_new(sizeof(hdColorFader_c));
        if (i_this == NULL)
            return NULL;
    }
    constructFadeListener(
        &i_this->mSceneListener);
    constructFadeListener(&i_this->mForegroundListener);
    i_this->mActive = 0;
    return i_this;
}

/* 02728180 */
void hdColorFader_c::destroy(u32 flags) {
    if (this != NULL) {
        destroyFadeListener(&mForegroundListener, 2);
        destroyFadeListener(&mSceneListener, 2);
        if (flags & 1) operator_delete(this);
    }
}

/* 027281E0 */
void hdColorFader_c::calc() {
    mDoGph_gInf_c::calcFade();
}

/* 027281E4 */
static void drawColorFadeCallback(hdColorFader_c* i_this, void* renderInfo) {
    hdFadeColor_l color;
    convertFadeColor(&color, &sColorFadeColor);
    drawColorFade(&color, renderInfo);
}

/* 02728494 */
static void finishColorFadeCallback(hdColorFader_c* i_this, void* renderInfo) {}

/* inlined into hdColorFader_c::initialize */
static void bindFadeCallback(hdFadeListener_l* listener, hdColorFader_c* owner, u32 callback) {
    listener->mDelegate.mVtable = gabi::ea(&sColorBoundDelegateVtable);
    listener->mDelegate.mpOwner = owner;
    listener->mDelegate.mVirtualIndex = -1;
    listener->mDelegate.mCallback = callback;
    listener->mDelegate.mAdjustment = 0;
    listener->mListenerVtable = gabi::ea(&sColorActiveListenerVtable);
    listener->mEnabled = 1;
}

/* 02728220 */
void hdColorFader_c::initialize() {
    bindFadeCallback(&mSceneListener, this, gabi::guest_fn((const void*)drawColorFadeCallback));
    registerFadeListener(sColorRenderLayers->layer(2), 1, &mSceneListener);
    bindFadeCallback(&mForegroundListener, this, gabi::guest_fn((const void*)finishColorFadeCallback));
    registerFadeListener(sColorRenderLayers->layer(10), 0, &mForegroundListener);
    mActive = 1;
}

/* 02728340 */
static void __sinit_hd_color_fader_cpp() {
    sinit_header_statics_rodata(0x1049F900, 0x101F866C, 0x1049F90C, 0x10141058);
}

/* WWHD glue
 data sColorListenerVtable 10141048 nocheck
 data sColorListenerInterfaceVtable 10141008 nocheck
 data sColorDisposerVtable 10169E3C nocheck
 data sColorEmptyDelegateVtable 10140FF0 nocheck
 data sColorDelegateVtable 10141020 nocheck
 data sColorBoundDelegateVtable 10141038 nocheck
 data sColorActiveListenerVtable 1014104C nocheck
 data sColorRenderLayers 101F95D0
 data sColorFadeColor 101F481C
 ext 02752A84 void constructFadeDisposer(hdFadeListener_l*)
 ext 027C0804 void destroyFadeListener(hdFadeListener_l*, u32)
 ext 027C23F8 void registerFadeListener(hdLayerBase_c*, u32, hdFadeListener_l*)
 ext 02727F7C void convertFadeColor(hdFadeColor_l*, hdFadeColorBytes_l*)
 ext 02728AD4 void drawColorFade(hdFadeColor_l*, void*)
 ext 025F06A8 void mDoGph_gInf_c::calcFade()
*/
