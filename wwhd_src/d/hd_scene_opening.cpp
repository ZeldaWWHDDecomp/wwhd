/** HD opening-scene lifecycle and input callbacks. Private decomp-derived source. */
#include "d/hd_scene_opening.h"
static void openingDrawTv(hdOpeningScene_c*, hdOpeningDrawInfo_l*);
static void openingDrawDrc(hdOpeningScene_c*, hdOpeningDrawInfo_l*);
static void* sOpeningLayoutManager;
static hdOpeningSaveArray_l* sOpeningSaveArray;
static u8 sOpeningFallbackResource;
static u8 sOpeningFallbackMovie;
static u8 sOpeningSafeStringVtable;
enum { kOpeningDrawSlot = 0x6C / 4, kOpeningUpdateSlot = 0x5C / 4, kOpeningFinishSlot = 0x54 / 4, kOpeningDestroySlot = 0xC / 4, kOpeningStateEnterSlot = 0x24 / 4, kOpeningStateExecuteSlot = 0x2C / 4, kOpeningStateLeaveSlot = 0x34 / 4 };
static u8 sOpeningDelegateVtable;
static u8 sOpeningListenerVtable;
static u8 sOpeningEmptyDelegateVtable;
static u8 sOpeningBaseDelegateVtable;
static u8 sOpeningDisposerVtable;
static hdOpeningScene_c* sOpeningScene;
static hdOpeningDisposer_l* sOpeningDisposer;
static hdOpeningGameInfo_l* sOpeningGameInfo;
/* 0270C5F4 */
u32 hdOpeningScene_c::receiveTouch() { return 0; }
/* 0270C600 */
u32 hdOpeningScene_c::receiveKey() { return 0; }
/* 0270C60C */
u32 hdOpeningScene_c::receiveController() { return 0; }
/* 0270C61C */
u32 hdOpeningScene_c::receiveCursor() { return 0; }
/* 0270C628 */
u32 hdOpeningScene_c::receiveFocus() { return 0; }
/* 0270C634 */
u32 hdOpeningScene_c::receiveUnfocus() { return 0; }
/* 0270C640 */
u32 hdOpeningScene_c::receiveRelease() { return 0; }
/* 0270C650 */
u32 hdOpeningScene_c::receiveCancel() { return 0; }
/* 0270C6AC */
static u32 openingStateIdValue(hdOpeningStateId_l* id) { return id->mId; }
/* 0270C820 */
void hdOpeningScene_c::unloadLayout() {
    hdOpeningSafeString_l name;
    name.mpText.v = gabi::guest_str("Opening_00");
    name.mVtable = gabi::ea(&sOpeningSafeStringVtable);
    openingLayoutRelease(sOpeningLayoutManager, &name);
}
/* 0270C864 */
void hdOpeningScene_c::createScreen() {
    hdOpeningSaveArray_l* saves = sOpeningSaveArray;
    u32 count = saves->mSlotCount;
    gptr<hdOpeningSaveSlot_l>* slots = saves->mpSlots;
    hdOpeningSaveSlot_l* slot = slots[count > 3 ? 3 : 0];
    hdOpeningScreen_l* screen = (hdOpeningScreen_l*)operator_new(0x8C);
    if (screen != NULL) screen = openingScreenConstruct(screen);
    mpScreen = screen;
    u32 flags = slot->mFlags;
    void* resource;
    if ((flags & (1u << 12)) != 0) resource = slot->mpCustomResource;
    else {
        resource = slot->mpDefaultResource;
        if (resource == NULL) resource = &sOpeningFallbackResource;
    }
    void* movie;
    if ((flags & (1u << 6)) && (flags & (1u << 7))) movie = slot->mCustomMovie;
    else {
        movie = slot->mpDefaultMovie;
        if (movie == NULL) movie = &sOpeningFallbackMovie;
    }
    screen->setup(gabi::ea(resource), gabi::ea(movie), 0);
}
/* 0270C940 */
void hdOpeningScene_c::initialize() {
    void* framework = openingFrameworkGet();
    openingFrameworkSetEnabled(framework, 0);
    sOpeningGameInfo->mSceneMode = 0;
    mTvListener.mDelegate.mVtable = gabi::ea(&sOpeningDelegateVtable);
    mTvListener.mDelegate.mpObject = this;
    mTvListener.mDelegate.mVirtualIndex = 0xFFFF;
    mTvListener.mDelegate.mFunction = gabi::guest_fn((const void*)&openingDrawTv);
    mTvListener.mDelegate.mThisAdjustment = 0;
    mTvListener.mName.mpText.v = gabi::guest_str("Tv");
    mTvListener.mEnabled = 1;
    hdOpeningSaveArray_l* saves = sOpeningSaveArray;
    u32 count = saves->mSlotCount;
    gptr<hdOpeningSaveSlot_l>* slots = saves->mpSlots;
    openingRegisterListener(slots[count > 3 ? 3 : 0], 3, &mTvListener);
    mDrcListener.mDelegate.mVtable = gabi::ea(&sOpeningDelegateVtable);
    mDrcListener.mDelegate.mpObject = this;
    mDrcListener.mDelegate.mVirtualIndex = 0xFFFF;
    mDrcListener.mDelegate.mFunction = gabi::guest_fn((const void*)&openingDrawDrc);
    mDrcListener.mDelegate.mThisAdjustment = 0;
    mDrcListener.mEnabled = 1;
    mDrcListener.mName.mpText.v = gabi::guest_str("Drc");
    saves = sOpeningSaveArray;
    count = saves->mSlotCount;
    slots = saves->mpSlots;
    openingRegisterListener(slots[count > 7 ? 7 : 0], 1, &mDrcListener);
    createScreen();
}
/* 0270CB8C */
static void openingDrawTv(hdOpeningScene_c* scene, hdOpeningDrawInfo_l* info) {
    openingSetViewport(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdOpeningSaveArray_l* saves = sOpeningSaveArray;
    hdOpeningScreen_l* screen = scene->mpScreen;
    u32 count = saves->mSlotCount;
    u32 index = info->mSlot;
    be<u32>* vtable = reinterpret_cast<be<u32>*>(screen->mVtable.get());
    gptr<hdOpeningSaveSlot_l>* slots = saves->mpSlots;
    hdOpeningSaveSlot_l* slot = slots[index < count ? index : 0];
    gabi::call_ptr<void>(vtable[kOpeningDrawSlot].get(), screen, slot->mRenderContext);
}
/* 0270CC20 */
static void openingDrawDrc(hdOpeningScene_c* scene, hdOpeningDrawInfo_l* info) {
    openingSetViewport(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
}
/* inlined into openingSceneConstruct */
static void openingInitializeListener(hdOpeningListener_l* listener) {
    openingListenerConstruct(listener);
    hdOpeningSafeString_l* name = &listener->mName;
    if (gabi::ea(name) == 0) name = (hdOpeningSafeString_l*)operator_new(sizeof(hdOpeningSafeString_l));
    if (gabi::ea(name) != 0) {
        name->mpText.v = gabi::guest_str("");
        name->mVtable = gabi::ea(&sOpeningSafeStringVtable);
    }
    listener->mEnabled = 0;
    listener->mVtable = gabi::ea(&sOpeningListenerVtable);
    listener->mpUser = NULL;
    hdOpeningDelegate_l* delegate = &listener->mDelegate;
    if (gabi::ea(delegate) != 0) {
        delegate->mFunction = 0;
        delegate->mVtable = gabi::ea(&sOpeningBaseDelegateVtable);
        delegate->mThisAdjustment = 0;
        delegate->mVirtualIndex = 0;
        delegate->mpObject = NULL;
        delegate->mVtable = gabi::ea(&sOpeningEmptyDelegateVtable);
    }
}
/* 0270CC40 */
hdOpeningScene_c* openingSceneConstruct(hdOpeningScene_c* scene) {
    if (scene == NULL) {
        scene = (hdOpeningScene_c*)operator_new(sizeof(hdOpeningScene_c));
        if (scene == NULL) return NULL;
    }
    scene->mStartDelay = 0;
    openingInitializeListener(&scene->mTvListener);
    openingInitializeListener(&scene->mDrcListener);
    scene->mpScreen = NULL;
    return scene;
}
/* 0270CE34 */
static void openingSceneDestroy(hdOpeningScene_c* scene, u32 flags) {
    if (scene != NULL) {
        openingListenerDestroy(&scene->mDrcListener, 2);
        openingListenerDestroy(&scene->mTvListener, 2);
        if (flags & 1) operator_delete(scene);
    }
}
/* 0270CD94 */
static void openingSceneCreate(void* heap) {
    if (sOpeningScene == NULL) {
        hdOpeningScene_c* scene = (hdOpeningScene_c*)openingHeapAllocate(0x98, heap, 4);
        hdOpeningDisposer_l* disposer = (hdOpeningDisposer_l*)scene->mDisposer;
        if (disposer != NULL) {
            openingDisposerConstruct(disposer, heap, 3);
            disposer->mVtable = gabi::ea(&sOpeningDisposerVtable);
        }
        sOpeningDisposer = disposer;
        if (scene != NULL) scene = openingSceneConstruct(scene);
        sOpeningScene = scene;
    }
}
/* 0270CE94 */
static void openingSceneDelete() {
    hdOpeningDisposer_l* disposer = sOpeningDisposer;
    if (disposer != NULL) {
        sOpeningDisposer = NULL;
        gabi::call_ptr<void>(gabi::at<be<u32>>(disposer->mVtable)[3].get(), disposer, 2u);
        openingSceneDestroy(sOpeningScene, 3);
        sOpeningScene = NULL;
    }
}
/* 0270CF00 */
static void openingSceneDisposerDestroy(hdOpeningDisposer_l* disposer, u32 flags) {
    if (disposer != NULL) {
        disposer->mVtable = gabi::ea(&sOpeningDisposerVtable);
        if (disposer == sOpeningDisposer) {
            hdOpeningScene_c* scene = sOpeningScene;
            sOpeningDisposer = NULL;
            openingSceneDestroy(scene, 2);
            sOpeningScene = NULL;
        }
        openingDisposerDestroy(disposer, 0);
        if (flags & 1) operator_delete(disposer);
    }
}
/* 0270CA80 */
void hdOpeningScene_c::updateScreen() {
    hdOpeningScreen_l* screen = mpScreen;
    gabi::tail_ptr<void>(reinterpret_cast<be<u32>*>(screen->mVtable.get())[kOpeningUpdateSlot].get(), screen);
}
/* 0270CA94 */
void hdOpeningScene_c::update() {
    updateScreen();
    s32 delay = mStartDelay;
    if (delay > 0) {
        delay--;
        mStartDelay = delay;
        if (delay <= 0) mpScreen->hideSkip();
    }
}
/* 0270CAE0 */
void hdOpeningScene_c::destroyScreen() {
    hdOpeningScreen_l* screen = mpScreen;
    gabi::call_ptr<void>(reinterpret_cast<be<u32>*>(screen->mVtable.get())[kOpeningFinishSlot].get(), screen);
    screen = mpScreen;
    if (screen != NULL) gabi::call_ptr<void>(reinterpret_cast<be<u32>*>(screen->mVtable.get())[kOpeningDestroySlot].get(), screen, 3u);
}
/* 0270CB3C */
void hdOpeningScene_c::finish() {
    sOpeningGameInfo->mSceneMode = 3;
    destroyScreen();
    unloadLayout();
    void* framework = openingFrameworkGet();
    openingFrameworkSetEnabled(framework, 1);
}
/* 0270C658 */
static void openingSaveSceneDelete(void* scene, u32 flags) {
    if (scene != NULL) {
        openingSaveSceneDestroy(scene, 0);
        if (flags & 1) operator_delete(scene);
    }
}
/* 0270C780 */
hdOpeningCallbackState_l* hdOpeningStateFactory_l::createState(hdOpeningStateId_l* id) {
    mState.mpStateId = id;
    return &mState;
}
/* 0270C78C */
void hdOpeningStateFactory_l::clearPrevious(gptr<hdOpeningCallbackState_l>* previous) { *previous = NULL; }
/* 0270C798 */
hdOpeningStateId_l* hdOpeningCallbackState_l::getStateId() { return mpStateId; }
/* 0270C7A0 */
void hdOpeningCallbackState_l::enter() {
    hdOpeningStateId_l* id = mpStateId;
    gabi::tail_ptr<void>(id->mpVtable[kOpeningStateEnterSlot].get(), id, mpOwner.get());
}
/* 0270C7BC */
void hdOpeningCallbackState_l::execute() {
    hdOpeningStateId_l* id = mpStateId;
    gabi::tail_ptr<void>(id->mpVtable[kOpeningStateExecuteSlot].get(), id, mpOwner.get());
}
/* 0270C7D8 */
void hdOpeningCallbackState_l::leave() {
    hdOpeningStateId_l* id = mpStateId;
    gabi::call_ptr<void>(id->mpVtable[kOpeningStateLeaveSlot].get(), id, mpOwner.get());
    mpStateId = NULL;
}
/* 0270CF9C */
static void __sinit_hd_scene_opening_cpp() {
    sinit_header_statics_rodata(0x1049EB8C, 0x101F82F4, 0x1049EB98, 0x1013EE78);
}
/* WWHD glue
 ext 0270BA48 void openingSaveSceneDestroy(void*, u32)
 data sOpeningScene 101F8318
 data sOpeningDisposer 101F831C
 data sOpeningDisposerVtable 1013EE80
 ext 0273B0D4 void* openingHeapAllocate(u32, void*, u32)
 ext 02752B0C void openingDisposerConstruct(void*, void*, u32)
 ext 02752BEC void openingDisposerDestroy(void*, u32)
 data sOpeningListenerVtable 10169E3C
 data sOpeningEmptyDelegateVtable 1013EE38
 data sOpeningBaseDelegateVtable 1013EE08
 str 1013EE77 in 0270CC40
 ext 02752A84 void openingListenerConstruct(void*)
 ext 027C0804 void openingListenerDestroy(void*, u32)
 data sOpeningDelegateVtable 1013EE50
 str 1013EE74 in 0270C940
 str 1013EE70 in 0270C940
 ext C0006988 void openingSetViewport(u32, u32, u32, u32, u32, u32, u32)
 ext 027C23F8 void openingRegisterListener(hdOpeningSaveSlot_l*, u32, void*)
 data sOpeningSaveArray 101F95D0
 data sOpeningFallbackResource 104A20FC
 data sOpeningFallbackMovie 104A2098
 data sOpeningSafeStringVtable 1013EE20
 ext 026BED70 hdOpeningScreen_l* openingScreenConstruct(hdOpeningScreen_l*)
 ext 026BF28C s32 hdOpening_screen_c::setup(u32, u32, u32)
 data sOpeningLayoutManager 101F7274
 data sOpeningGameInfo 101F86E8
 str 1013EE64
 ext 026FBA38 void openingLayoutRelease(void*, hdOpeningSafeString_l*)
 ext 02032C6C void* openingFrameworkGet()
 ext 02032AD8 void openingFrameworkSetEnabled(void*, u32)
 ext 026BF440 void hdOpening_screen_c::hideSkip()
*/
