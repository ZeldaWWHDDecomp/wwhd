/** FaderMgr orchestrates color, wipe and task-based screen fades for the HD renderer. */
#include "hd/hd_fader_mgr.h"
#include "hd/hd_render_resources.h"
#include "hd/hd_layer_2d.h"
static hdFaderMgr_c* sManagerFaderMgr;
static hdFaderMgr_c* sManagerRegisteredFaderMgr;
static hdRenderLayerRegistry_l* sManagerRenderLayers;
static u8 sManagerManagerVtable;
static u8 sManagerListenerVtable;
static u8 sManagerListenerInterfaceVtable;
static u8 sManagerDisposerVtable;
static u8 sManagerEmptyDelegateVtable;
static u8 sManagerDelegateVtable;
static u8 sManagerBoundDelegateVtable;
static u8 sManagerActiveListenerVtable;
static u8 sManagerParticleDelegateVtable;
static u8 sManagerDrawDelegateVtable;
static u8 sManagerShadowDelegateVtable;
static u8 sManagerParticleListenerVtable;
static u8 sManagerDrawListenerVtable;
static u8 sManagerShadowListenerVtable;
static u8 sManagerFadeEnabled;
static f32 sManagerFadeDirection;
static u8 sManagerWipeEnabled;
static f32 sManagerWipeProgress;
static void* allocateFader(u32, void*, u32);
static void constructManagerDisposer(hdFaderMgr_c*, void*, u32);
static void constructListenerDisposer(hdFadeListener_l*);
static void destroyManagerDisposer(hdFaderMgr_c*, u32);
static hdColorFader_c* constructColorFader(hdColorFader_c*);
static hdWipeFader_c* constructWipeFader(hdWipeFader_c*);
static void destroyWipeFader(hdWipeFader_c*, u32);
static void destroyFadeListener(hdFadeListener_l*, u32);
static void registerFadeListener(hdLayerBase_c*, u32, hdFadeListener_l*);
static void unregisterFadeListener(hdLayerBase_c*, u32, hdFadeListener_l*);
static void* getFadeTaskHeap();
static u32 getFadeTaskHeapSize(void*);
static hdFadeTask_l* createFadeTask(u32, u32, u32, void*, u32, be<u32>*);
static void setFadeTaskMode(hdFadeTask_l*, u32, u32);
static void calcFadeTask(hdFadeTask_l*);
static void initializeWipeFader(hdWipeFader_c*);
static void resetWipeFader(hdWipeFader_c*);
static void calcWipeFader(hdWipeFader_c*);
static void particleFadeCallback(void*, void*);
static void drawFadeCallback(void*, void*);
static void shadowFadeCallback(void*, void*);
static void convertFadeColor(hdFadeColor_l*, hdFadeColorBytes_l*);
static void drawFadeColor(hdFadeColor_l*, void*);
static void managerDrawCallback(hdFaderMgr_c*, void*);
static void managerForegroundCallback(hdFaderMgr_c*, void*);

/* inlined into hdFaderMgr_construct */
static void constructManagerListener(hdFadeListener_l* listener) {
    constructListenerDisposer(listener);
    const u32 address = gabi::ea(listener);
    struct ListenerInterface_l {
        be<u32> mListener, mInterface;
    };
    ListenerInterface_l* interface =
        gabi::at<ListenerInterface_l>(address + offsetof(hdFadeListener_l, mListenerVtable));
    if (interface == NULL)
        interface = (ListenerInterface_l*)operator_new(sizeof(ListenerInterface_l));
    if (interface != NULL) {
        interface->mListener = gabi::ea(&sManagerListenerVtable);
        interface->mInterface = gabi::ea(&sManagerListenerInterfaceVtable);
    }
    listener->mEnabled = 0;
    listener->mVtable =
        gabi::ea(&sManagerDisposerVtable);
    listener->mUserData = 0;
    hdFadeDelegate_l* delegate =
        gabi::at<hdFadeDelegate_l>(address + offsetof(hdFadeListener_l, mDelegate));
    if (delegate != NULL) {
        delegate->mCallback = 0;
        delegate->mVtable = gabi::ea(&sManagerEmptyDelegateVtable);
        delegate->mAdjustment = 0;
        delegate->mVirtualIndex = 0;
        delegate->mpOwner = NULL;
        delegate->mVtable = gabi::ea(&sManagerDelegateVtable);
    }
}

/* 0272854C */
static hdFaderMgr_c* hdFaderMgr_construct(hdFaderMgr_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdFaderMgr_c*)operator_new(sizeof(hdFaderMgr_c));
        if (i_this == NULL)
            return NULL;
    }
    constructManagerListener(
        &i_this->mSceneListener);
    constructManagerListener(
        &i_this->mForegroundListener);
    constructManagerListener(
        &i_this->mAuxiliaryListener);
    constructManagerListener(
        &i_this->mUnusedListener);
    constructColorFader(
        &i_this->mColorFader);
    constructWipeFader(
        &i_this->mWipeFader);
    i_this->mForegroundEnabled = 0;
    i_this->mpTask = NULL;
    i_this->mDelay = 0;
    return i_this;
}

/* 0272879C */
hdFaderMgr_c* hdFaderMgr_c::create(void* heap) {
    hdFaderMgr_c* manager = sManagerFaderMgr;
    if (manager == NULL) {
        manager = (hdFaderMgr_c*)allocateFader(sizeof(hdFaderMgr_c), heap, 4);
        if (manager != NULL) {
            constructManagerDisposer(manager, heap, 3);
            manager->mVtable = gabi::ea(&sManagerManagerVtable);
        }
        sManagerRegisteredFaderMgr = manager;
        if (manager != NULL) manager = hdFaderMgr_construct(manager);
        sManagerFaderMgr = manager;
    }
    return manager;
}

/* 02728830 */
void hdFaderMgr_c::destroy(u32 flags) {
    if (this != NULL) {
        destroyWipeFader(&mWipeFader, 2);
        mColorFader.destroy(2);
        destroyFadeListener(&mUnusedListener, 2);
        destroyFadeListener(&mAuxiliaryListener, 2);
        destroyFadeListener(&mForegroundListener, 2);
        destroyFadeListener(&mSceneListener, 2);
        if (flags & 1) operator_delete(this);
    }
}

/* inlined into hdFaderMgr_c::bindLayers */
static void bindManagerCallback(hdFadeListener_l* listener, hdFaderMgr_c* owner, u32 callback) {
    listener->mDelegate.mVtable = gabi::ea(&sManagerBoundDelegateVtable);
    listener->mDelegate.mpOwner = owner;
    listener->mDelegate.mVirtualIndex = -1;
    listener->mDelegate.mCallback = callback;
    listener->mDelegate.mAdjustment = 0;
    listener->mListenerVtable = gabi::ea(&sManagerActiveListenerVtable);
    listener->mEnabled = 1;
}

/* 027288C0 */
void hdFaderMgr_c::bindLayers() {
    bindManagerCallback(&mSceneListener, this, gabi::guest_fn((const void*)managerDrawCallback));
    registerFadeListener(sManagerRenderLayers->layer(6), 1, &mSceneListener);
    bindManagerCallback(&mForegroundListener, this, gabi::guest_fn((const void*)managerForegroundCallback));
    registerFadeListener(sManagerRenderLayers->layer(10), 1, &mForegroundListener);
}

/* 027289DC */
void hdFaderMgr_c::initialize() {
    void* heap = getFadeTaskHeap();
    u32 size = getFadeTaskHeapSize(heap);
    gabi::Local<be<u32>> options;
    *options.get() = 0;
    hdFadeTask_l* task = createFadeTask(0, 0, 0, heap, size, options.get());
    mpTask = task;
    setFadeTaskMode(task, 1, 0);
    bindLayers();
    mColorFader.initialize();
    initializeWipeFader(&mWipeFader);
    resetWipeFader(&mWipeFader);
}

/* 02728A74 */
void hdFaderMgr_c::calc() {
    hdColorFader_c::calc();
    u32 delay = mDelay;
    if (delay == 0) calcFadeTask(mpTask);
    else mDelay = delay - 1;
    calcWipeFader(&mWipeFader);
}

struct hdFadeColorWords_l { be<u32> mRed, mGreen, mBlue, mAlpha; };
struct hdFadeBlendState_l {
    be<u8> mBlendEnabled;
    be<u8> mLogicEnabled;
    u8 mReserved[2];
    be<u32> mMode;
    u8 mState[0x6C];
};
WWHD_SIZE(hdFadeBlendState_l, 0x74);
struct hdFadeViewport_l { be<f32> mLeft, mTop, mRight, mBottom; u8 mReserved[8]; };
WWHD_SIZE(hdFadeViewport_l, 0x18);
struct hdFadeProjection_l { u8 mState[0xAC]; };
struct hdFadeView_l { u8 mState[0x58]; };
struct hdFadeDisplay_l { u8 mState[0x744]; hdFadeViewport_l mViewport; u8 mTail[0xA4]; };
WWHD_SIZE(hdFadeDisplay_l, 0x800);
struct hdFadeRenderer_l { u8 mState[0x18]; hdFadeDisplay_l mDisplays[2]; };
static hdFadeRenderer_l* sManagerFadeRenderer;
static u8 sManagerDrawModeOwner;
struct hdFadeDrawMode_l { be<u32> mMode; be<u32> mDisplay; };
static hdFadeDrawMode_l* acquireFadeDrawMode(void*);
static void prepareFadeColor(hdFadeColor_l*);
static void constructFadeBlendState(hdFadeBlendState_l*);
static void applyFadeBlendState(hdFadeBlendState_l*);
static void copyFadeViewport(hdFadeViewport_l*, hdFadeViewport_l*);
static void applyFadeViewport(hdFadeViewport_l*, hdFadeViewport_l*);
static void constructFadeProjection(hdFadeProjection_l*, hdFadeViewport_l*, f32, f32);
static void constructFadeView(hdFadeView_l*, hdFadeProjection_l*);
static void scaleFadeMatrix(Mtx34*, f32, f32, f32);
static void concatFadeMatrix(hdFadeView_l*, Mtx34*, Mtx34*);
static void* getFadeProjectionMatrix(hdFadeProjection_l*);
static void drawFadeQuad(hdFadeColor_l*, Mtx34*, void*, u32);
static void destroyFadeView(hdFadeView_l*, u32);
static void destroyFadeProjection(hdFadeProjection_l*, u32);

/* 02728AD4 */
static void drawFadeQuadColor(hdFadeColor_l* source, hdLayerRenderInfo_c* renderInfo) {
    hdFadeColor_l color;
    hdFadeColorWords_l* sourceWords = reinterpret_cast<hdFadeColorWords_l*>(source);
    hdFadeColorWords_l* colorWords = reinterpret_cast<hdFadeColorWords_l*>(&color);
    colorWords->mRed = sourceWords->mRed;
    colorWords->mBlue = sourceWords->mBlue;
    colorWords->mGreen = sourceWords->mGreen;
    colorWords->mAlpha = sourceWords->mAlpha;
    prepareFadeColor(&color);
    if (f32(color.mAlpha) != 0.0f) {
        hdFadeBlendState_l blendState;
        constructFadeBlendState(&blendState);
        blendState.mBlendEnabled = 1;
        blendState.mLogicEnabled = 0;
        blendState.mMode = 7;
        applyFadeBlendState(&blendState);
        hdFadeViewport_l* sourceViewport = &sManagerFadeRenderer->mDisplays[u32(renderDisplay(renderInfo))].mViewport;
        hdFadeViewport_l viewport;
        copyFadeViewport(&viewport, sourceViewport);
        applyFadeViewport(&viewport, sourceViewport);
        hdFadeProjection_l projection;
        constructFadeProjection(&projection, &viewport, 0.0f, 1.0f);
        hdFadeView_l view;
        constructFadeView(&view, &projection);
        f32 width = f32(viewport.mRight) - f32(viewport.mLeft);
        f32 height = f32(viewport.mBottom) - f32(viewport.mTop);
        Mtx34 matrix;
        scaleFadeMatrix(&matrix, width, height, 1.0f);
        concatFadeMatrix(&view, &matrix, &matrix);
        u32 mode = acquireFadeDrawMode(&sManagerDrawModeOwner)->mMode;
        void* projectionMatrix = getFadeProjectionMatrix(&projection);
        drawFadeQuad(&color, &matrix, projectionMatrix, mode);
        destroyFadeView(&view, 2);
        destroyFadeProjection(&projection, 2);
    }
}

/* 02728C54 */
void hdFaderMgr_c::draw(void* renderInfo) {
    hdFadeTask_l* task = mpTask;
    if (u32(task->mState) != 1) {
        hdFadeColorBytes_l bytes;
        bytes.mRed = task->mColor.mRed;
        bytes.mGreen = task->mColor.mGreen;
        bytes.mBlue = task->mColor.mBlue;
        bytes.mAlpha = task->mColor.mAlpha;
        hdFadeColor_l color;
        convertFadeColor(&color, &bytes);
        drawFadeColor(&color, renderInfo);
    }
}

/* 02728CA8 */
void hdFaderMgr_c::drawForeground(void* renderInfo) {
    if (mForegroundEnabled != 0) draw(renderInfo);
}

/* 02728CB8 */
void hdFaderMgr_c::start(u32 direction, u32 delay) {
    hdFadeTask_l* task = mpTask;
    mDelay = delay;
    gabi::tail_ptr(task->mpVtable->mStart.get(), task, direction, delay);
}

/* 02728CD4 */
u32 hdFaderMgr_c::isDone() {
    hdFadeTask_l* task = mpTask;
    return gabi::tail_ptr<u32>(task->mpVtable->mIsDone.get(), task);
}

/* 02728CE8 */
u32 hdFaderMgr_c::isFading() {
    if (sManagerFadeEnabled != 0 && sManagerFadeDirection < 0.0f) return 1;
    if (u32(mpTask->mState) == 2) return 1;
    if (sManagerWipeEnabled != 0 && sManagerWipeProgress > 0.0f && mWipeFader.mDrcEnabled != 0) return 1;
    return 0;
}

/* 02728D5C */
void hdFaderMgr_c::reset() {
    setFadeTaskMode(mpTask, 0, 0);
}

/* inlined into auxiliary callback setters */
static void bindAuxiliaryCallback(hdFadeListener_l* listener, void* owner, u32 callback, u32 delegateVtable, u32 listenerVtable) {
    listener->mDelegate.mpOwner = owner;
    listener->mDelegate.mAdjustment = 0;
    listener->mDelegate.mVtable = delegateVtable;
    listener->mDelegate.mCallback = callback;
    listener->mDelegate.mVirtualIndex = -1;
    listener->mEnabled = 2;
    listener->mListenerVtable = listenerVtable;
}

/* 02728D6C */
void hdFaderMgr_c::bindParticleCallback(void* owner) {
    bindAuxiliaryCallback(&mAuxiliaryListener, owner, gabi::guest_fn((const void*)particleFadeCallback), gabi::ea(&sManagerParticleDelegateVtable), gabi::ea(&sManagerParticleListenerVtable));
    registerFadeListener(sManagerRenderLayers->layer(6), 0, &mAuxiliaryListener);
}

/* 02728DE8 */
void hdFaderMgr_c::bindDrawCallback(void* owner) {
    bindAuxiliaryCallback(&mAuxiliaryListener, owner, gabi::guest_fn((const void*)drawFadeCallback), gabi::ea(&sManagerDrawDelegateVtable), gabi::ea(&sManagerDrawListenerVtable));
    registerFadeListener(sManagerRenderLayers->layer(6), 0, &mAuxiliaryListener);
}

/* 02728E64 */
void hdFaderMgr_c::bindShadowCallback(void* owner) {
    bindAuxiliaryCallback(&mAuxiliaryListener, owner, gabi::guest_fn((const void*)shadowFadeCallback), gabi::ea(&sManagerShadowDelegateVtable), gabi::ea(&sManagerShadowListenerVtable));
    registerFadeListener(sManagerRenderLayers->layer(6), 0, &mAuxiliaryListener);
}

/* 02728EE0 */
void hdFaderMgr_c::unbindAuxiliary() {
    unregisterFadeListener(sManagerRenderLayers->layer(6), 0, &mAuxiliaryListener);
}

/* 02728F10 */
void hdFaderMgr_c::dispose(u32 flags) {
    if (this != NULL) {
        mVtable = gabi::ea(&sManagerManagerVtable);
        if (this == sManagerRegisteredFaderMgr) {
            hdFaderMgr_c* manager = sManagerFaderMgr;
            sManagerRegisteredFaderMgr = NULL;
            manager->destroy(2);
            sManagerFaderMgr = NULL;
        }
        destroyManagerDisposer(this, 0);
        if (flags & 1) operator_delete(this);
    }
}

/* 02728FAC */
static void __sinit_hd_fader_mgr_cpp() {
    sinit_header_statics_rodata(0x1049F91C, 0x101F8690, 0x1049F928, 0x10141138);
}

/* WWHD glue
 data sManagerFadeRenderer 101F95D0
 data sManagerDrawModeOwner 104B45C0 nocheck
 ext 027F29D4 hdFadeDrawMode_l* acquireFadeDrawMode(void*)
 ext 0274D11C void prepareFadeColor(hdFadeColor_l*)
 ext 0274FDBC void constructFadeBlendState(hdFadeBlendState_l*)
 ext 0274FEB0 void applyFadeBlendState(hdFadeBlendState_l*)
 ext 0274F5F0 void copyFadeViewport(hdFadeViewport_l*, hdFadeViewport_l*)
 ext 0274F964 void applyFadeViewport(hdFadeViewport_l*, hdFadeViewport_l*)
 ext 0274E364 void constructFadeProjection(hdFadeProjection_l*, hdFadeViewport_l*, f32, f32)
 ext 0274CE34 void constructFadeView(hdFadeView_l*, hdFadeProjection_l*)
 ext 028E945C void scaleFadeMatrix(Mtx34*, f32, f32, f32)
 ext 028E9108 void concatFadeMatrix(hdFadeView_l*, Mtx34*, Mtx34*)
 ext 0274D80C void* getFadeProjectionMatrix(hdFadeProjection_l*)
 ext 027AAABC void drawFadeQuad(hdFadeColor_l*, Mtx34*, void*, u32)
 ext 0274CF04 void destroyFadeView(hdFadeView_l*, u32)
 ext 0274E474 void destroyFadeProjection(hdFadeProjection_l*, u32)
 ext 02728C54 void managerDrawCallback(hdFaderMgr_c*, void*) callback
 ext 02728CA8 void managerForegroundCallback(hdFaderMgr_c*, void*) callback
 data sManagerFaderMgr 101F86B4
 data sManagerRegisteredFaderMgr 101F86B8
 data sManagerRenderLayers 101F95D0
 data sManagerManagerVtable 10141140 nocheck
 data sManagerListenerVtable 10141100 nocheck
 data sManagerListenerInterfaceVtable 10141090 nocheck
 data sManagerDisposerVtable 10169E3C nocheck
 data sManagerEmptyDelegateVtable 10141078 nocheck
 data sManagerDelegateVtable 101410A8 nocheck
 data sManagerBoundDelegateVtable 101410C0 nocheck
 data sManagerActiveListenerVtable 10141104 nocheck
 data sManagerParticleDelegateVtable 101410D0 nocheck
 data sManagerDrawDelegateVtable 101410F0 nocheck
 data sManagerShadowDelegateVtable 101410E0 nocheck
 data sManagerParticleListenerVtable 10141114 nocheck
 data sManagerDrawListenerVtable 10141120 nocheck
 data sManagerShadowListenerVtable 1014112C nocheck
 data sManagerFadeEnabled 101F4827
 data sManagerFadeDirection 101F4814
 data sManagerWipeEnabled 101D616C
 data sManagerWipeProgress 101D6160
 ext 0273B0D4 void* allocateFader(u32, void*, u32)
 ext 02752B0C void constructManagerDisposer(hdFaderMgr_c*, void*, u32)
 ext 02752A84 void constructListenerDisposer(hdFadeListener_l*)
 ext 02752BEC void destroyManagerDisposer(hdFaderMgr_c*, u32)
 ext 02728030 hdColorFader_c* constructColorFader(hdColorFader_c*)
 ext 02739734 hdWipeFader_c* constructWipeFader(hdWipeFader_c*)
 ext 02739898 void destroyWipeFader(hdWipeFader_c*, u32)
 ext 027C0804 void destroyFadeListener(hdFadeListener_l*, u32)
 ext 027C23F8 void registerFadeListener(hdLayerBase_c*, u32, hdFadeListener_l*)
 ext 027C2560 void unregisterFadeListener(hdLayerBase_c*, u32, hdFadeListener_l*)
 ext 02739398 void* getFadeTaskHeap()
 ext 027393E8 u32 getFadeTaskHeapSize(void*)
 ext 027ECD78 hdFadeTask_l* createFadeTask(u32, u32, u32, void*, u32, be<u32>*)
 ext 027ED004 void setFadeTaskMode(hdFadeTask_l*, u32, u32)
 ext 027ECED4 void calcFadeTask(hdFadeTask_l*)
 ext 02739904 void initializeWipeFader(hdWipeFader_c*)
 ext 02739E3C void resetWipeFader(hdWipeFader_c*)
 ext 02739A90 void calcWipeFader(hdWipeFader_c*)
 ext 025A332C void particleFadeCallback(void*, void*) callback
 ext 025A28B0 void drawFadeCallback(void*, void*) callback
 ext 025A2374 void shadowFadeCallback(void*, void*) callback
 ext 02728498 void convertFadeColor(hdFadeColor_l*, hdFadeColorBytes_l*)
 ext 02728AD4 void drawFadeColor(hdFadeColor_l*, void*)
*/
