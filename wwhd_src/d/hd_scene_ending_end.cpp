/* HD ending end scene: resource lifetime, drawing and singleton ownership. */
#include "d/hd_scene_ending_end.h"
#include "gc_api/m_Do/m_Do_audio.h"
#include "d/hd_screen_ending_end.h"
static hdSceneEndingEnd_c* sEndingInstance;
static hdSceneDisposer_c* sEndingDisposer;
static hdScreenStringTable sEndingStringVtable;
static u32 sEndingDelegateBaseVtable;
static u32 sEndingDelegateVtable;
static u32 sEndingScreenVtable;
static u32 sEndingDisposerVtable;
static hdSceneHeaderWords_c sEndingHeaderWords;
static hdSceneHeaderRange_c sEndingHeaderRange;
static hdSceneRegistration_c sEndingHeaderRegistration;
static hdSceneRegistration_c sEndingSignedRegistration;
static hdSceneRegistration_c sEndingUnsignedRegistration;
static f32 sEndingLower;
static f32 sEndingUpper;
static hdLayoutResMgr_c* sEndingLayoutManager;
static mDoLibCameraSystem* sEndingCameraSystem;
static mDoLibMatrix44 sEndingDefaultProjection;
static Mtx34 sEndingDefaultView;
static void* sEndingDrawRenderer;

/* 02708090 */
hdSceneEndingEnd_c* hdSceneEndingEnd_c::construct(hdSceneEndingEnd_c* self) {
    if (!self) {
        self = static_cast<hdSceneEndingEnd_c*>(hdSceneAllocate(sizeof(hdSceneEndingEnd_c)));
        if (!self) return NULL;
    }
    hdSceneScreenConstruct(&self->mScreen);
    hdScreenString* name = gabi::at<hdScreenString>(gabi::ea(self) + offsetof(hdSceneEndingEnd_c, mScreen) + offsetof(hdSceneScreen_c, mDisplayName));
    if (!name) name = static_cast<hdScreenString*>(hdSceneAllocate(sizeof(hdScreenString)));
    if (name) {
        name->mVtable = &sEndingStringVtable;
        name->mText.v = gabi::guest_addr("");
    }
    self->mScreen.mEnabled = 0;
    self->mScreen.mFlags = 0;
    hdSceneDrawDelegate_c* delegate = gabi::at<hdSceneDrawDelegate_c>(gabi::ea(self) + offsetof(hdSceneEndingEnd_c, mScreen) + offsetof(hdSceneScreen_c, mDrawDelegate));
    self->mScreen.mpVtable = &sEndingScreenVtable;
    if (delegate) {
        hdSceneDrawDelegate_c* storage = delegate;
        if (!storage) storage = static_cast<hdSceneDrawDelegate_c*>(hdSceneAllocate(sizeof(hdSceneDrawDelegate_c)));
        if (storage) {
            storage->mpMethod = NULL;
            storage->mThisAdjustment = 0;
            storage->mpVtable = &sEndingDelegateBaseVtable;
            storage->mVirtualIndex = 0;
            storage->mpOwner = NULL;
        }
        delegate->mpVtable = &sEndingDelegateVtable;
    }
    self->mpShortcut = NULL;
    self->mpMainParts = NULL;
    return self;
}
/* 02708188 */
void hdSceneEndingEnd_c::createInstance(void* heap) {
    if (sEndingInstance) return;
    hdSceneEndingEnd_c* instance = static_cast<hdSceneEndingEnd_c*>(hdSceneHeapAllocate(sizeof(hdSceneEndingEnd_c), heap, 4));
    if (instance) {
        hdSceneDisposerConstruct(&instance->mDisposer, heap, 3);
        instance->mDisposer.mpVtable = reinterpret_cast<hdScreenDeleteTable*>(&sEndingDisposerVtable);
    }
    sEndingDisposer = reinterpret_cast<hdSceneDisposer_c*>(instance);
    sEndingInstance = instance ? construct(instance) : NULL;
}
/* 0270821C */
void hdSceneEndingEnd_c::destroy(hdSceneEndingEnd_c* self, u32 flags) {
    if (!self) return;
    hdSceneScreenDestroy(&self->mScreen, 2);
    if (flags & 1) hdSceneDelete(self);
}
/* 02708270 */
void hdSceneEndingEnd_c::destroyInstance() {
    hdSceneDisposer_c* disposer = sEndingDisposer;
    if (!disposer) return;
    sEndingDisposer = NULL;
    gabi::call_ptr<void>(disposer->mpVtable->mDestroy.get(), disposer, 2u);
    destroy(sEndingInstance, 3);
    sEndingInstance = NULL;
}
/* 027082DC */
void hdSceneEndingEnd_c::destroyDisposer(hdSceneDisposer_c* disposer, u32 flags) {
    if (!disposer) return;
    disposer->mpVtable = reinterpret_cast<hdScreenDeleteTable*>(&sEndingDisposerVtable);
    if (disposer == sEndingDisposer) {
        hdSceneEndingEnd_c* instance = sEndingInstance;
        sEndingDisposer = NULL;
        destroy(instance, 2);
        sEndingInstance = NULL;
    }
    hdSceneDisposerDestroy(disposer, 0);
    if (flags & 1) hdSceneDelete(disposer);
}
/* 02708378 */
void hdSceneEndingEnd_c::staticInit() {
    sEndingHeaderWords.mWords[2] = 0;
    sEndingHeaderWords.mWords[0] = 0;
    sEndingHeaderWords.mWords[3] = 0;
    sEndingHeaderWords.mWords[1] = 0;
    hdSceneRegisterGlobal(&sEndingHeaderRegistration);
    f32 lower = sEndingLower;
    f32 upper = sEndingUpper;
    sEndingHeaderRange.mLower = lower;
    sEndingHeaderRange.mUpper = upper;
    hdSceneConstructSigned(&sEndingHeaderRange.mSigned);
    hdSceneRegisterGlobal(&sEndingSignedRegistration);
    hdSceneConstructUnsigned(&sEndingHeaderRange.mUnsigned);
    hdSceneRegisterGlobal(&sEndingUnsignedRegistration);
}
static u32 sEndingDrawDelegateVtable;
static u32 sEndingDrawMethod;
static hdPartsStateId_c sEndingMainDisplayState;
static u32 sEndingEventSound;
static u32 sEndingSceneRequestContext;
extern hdParts_c* hdSceneShortcutConstruct(hdParts_c*);
extern u32 hdSceneSelectProcess(u32);
extern void hdSceneRequestChange(u32 process, u32 mode, u32 argument, u32 frames, u32 flag);
extern void hdSceneResetAudio(void*);

/* 02707B80 */
void hdSceneEndingEnd_c::unloadArchive() {
    hdScreenString name;
    name.mText.v = gabi::guest_addr("EndingEnd_00");
    name.mVtable = &sEndingStringVtable;
    sEndingLayoutManager->unload(&name);
}
/* 02707BC4 */
void hdSceneEndingEnd_c::createParts() {
    mDoLibCameraSystem* system = sEndingCameraSystem;
    u32 count = system->cameraCount;
    gptr<mDoLibCamera>* cameras = system->cameras;
    mDoLibCamera* camera = *gabi::at<gptr<mDoLibCamera>>(gabi::ea(cameras) + (count > 3 ? 12 : 0));
    hdEndingEnd_c* parts = static_cast<hdEndingEnd_c*>(hdSceneAllocate(sizeof(hdEndingEnd_c)));
    if (parts) parts = hdEndingEnd_c::construct(parts);
    mpMainParts = parts;
    u32 flags = camera->flags;
    mDoLibMatrix44* projection;
    if (flags & mDoLibCamera_IndirectProjection) projection = camera->indirectProjection;
    else {
        projection = camera->projection;
        if (!projection) projection = &sEndingDefaultProjection;
    }
    Mtx34* view;
    if ((flags & mDoLibCamera_InlineViewA) && (flags & mDoLibCamera_InlineViewB)) view = &camera->inlineView;
    else {
        view = camera->view;
        if (!view) view = &sEndingDefaultView;
    }
    parts->initializeEnding(gabi::ea(projection), gabi::ea(view), NULL);
    hdParts_c* shortcut = static_cast<hdParts_c*>(hdSceneAllocate(0x15C));
    if (shortcut) shortcut = hdSceneShortcutConstruct(shortcut);
    mpShortcut = shortcut;
    flags = camera->flags;
    hdPartsVtbl_c* vtable = shortcut->mVtable;
    if (flags & mDoLibCamera_IndirectProjection) projection = camera->indirectProjection;
    else {
        projection = camera->projection;
        if (!projection) projection = &sEndingDefaultProjection;
    }
    if ((flags & mDoLibCamera_InlineViewA) && (flags & mDoLibCamera_InlineViewB)) view = &camera->inlineView;
    else {
        view = camera->view;
        if (!view) view = &sEndingDefaultView;
    }
    // HD: shortcut initialization uses its extended create slot, not the base parts slot.
    hdSceneShortcutVtable_c* shortcutVtable = reinterpret_cast<hdSceneShortcutVtable_c*>(vtable);
    gabi::call_ptr<void>(shortcutVtable->mCreate.get(), shortcut, projection, view, 0u);
}
/* 02707D70 */
void hdSceneEndingEnd_c::initialize() {
    hdSceneDrawDelegate_c* delegate = gabi::at<hdSceneDrawDelegate_c>(gabi::ea(this) + offsetof(hdSceneEndingEnd_c, mScreen) + offsetof(hdSceneScreen_c, mDrawDelegate));
    if (delegate) {
        delegate->mpOwner = this;
        delegate->mThisAdjustment = 0;
        delegate->mpVtable = &sEndingDrawDelegateVtable;
        delegate->mpMethod = &sEndingDrawMethod;
        delegate->mVirtualIndex = -1;
    }
    mScreen.mEnabled = 1;
    mScreen.mDisplayName.mText.v = gabi::guest_addr("Tv");
    mDoLibCameraSystem* system = sEndingCameraSystem;
    u32 count = system->cameraCount;
    gptr<mDoLibCamera>* cameras = system->cameras;
    mDoLibCamera* camera = *gabi::at<gptr<mDoLibCamera>>(gabi::ea(cameras) + (count > 3 ? 12 : 0));
    hdSceneAttachScreen(camera, 3, &mScreen);
    createParts();
    hdInputGetManager()->setMode(3);
}
/* 02707E28 */
void hdSceneEndingEnd_c::executeParts() {
    mpMainParts->vExecuteState();
    mpShortcut->vExecuteState();
}
/* 02707E78 */
void hdSceneEndingEnd_c::execute() {
    u32 event = sEndingEventSound;
    if (event == 0x8FC) {
        mpMainParts->changeState(&sEndingMainDisplayState);
        executeParts();
        return;
    }
    if (event == 0xA28) {
        u32 process = hdSceneSelectProcess(7);
        hdSceneRequestChange(process, 10, 0, 120, 1);
        mDoAudioApi::bgmStop(30);
        hdSceneResetAudio(&sEndingSceneRequestContext);
    }
    executeParts();
}
/* 02707F10 */
void hdSceneEndingEnd_c::releaseParts() {
    mpMainParts->vFinalize();
    hdParts_c* main = mpMainParts;
    if (main) main->vDelete(3);
    mpShortcut->vFinalize();
    hdParts_c* shortcut = mpShortcut;
    if (shortcut) shortcut->vDelete(3);
}
/* 02707FA0 */
void hdSceneEndingEnd_c::release() { releaseParts(); unloadArchive(); }
/* 02707FD4 */
void hdSceneEndingEnd_c::draw(hdSceneDrawInfo_c* info) {
    hdSceneDrawSetup(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdSceneDrawPrepare(sEndingDrawRenderer);
    mDoLibCameraSystem* system = sEndingCameraSystem;
    u32 index = info->mViewIndex;
    u32 count = system->cameraCount;
    hdParts_c* parts = mpMainParts;
    gptr<mDoLibCamera>* cameras = system->cameras;
    u32 selected = index < count ? index : 0;
    hdPartsVtbl_c* table = parts->mVtable;
    mDoLibCamera* camera = *gabi::at<gptr<mDoLibCamera>>(gabi::ea(cameras) + selected * 4);
    void* context = gabi::at<void>(gabi::ea(camera) + offsetof(hdSceneCameraDrawContext_l, mDrawContext));
    gabi::call_ptr<void>(table->mDraw.get(), parts, context);
    mpShortcut->vDraw(context);
}
/* WWHD glue
ext 02667DF0 hdEndingEnd_c* hdEndingEnd_c::construct(hdEndingEnd_c*)
ext 02667E5C u32 hdEndingEnd_c::initializeEnding(u32,u32,void*)
data sEndingInstance 101F8268
data sEndingDisposer 101F826C
data sEndingStringVtable 1013E810
data sEndingDelegateBaseVtable 1013E7F8
data sEndingDelegateVtable 1013E828
data sEndingDisposerVtable 1013E870
data sEndingHeaderWords 1049E924
data sEndingHeaderRange 1049E918
data sEndingHeaderRegistration 101F8244
data sEndingSignedRegistration 101F8250
data sEndingUnsignedRegistration 101F825C
data sEndingLower 1013E864
data sEndingUpper 1013E868
data sEndingScreenVtable 10169E3C
data sEndingLayoutManager 101F7274
data sEndingCameraSystem 101F95D0
data sEndingDefaultProjection 104A20FC
data sEndingDefaultView 104A2098
data sEndingDrawRenderer 101F86E8
ext 0273AD10 void* hdSceneAllocate(u32)
ext 0273B0D4 void* hdSceneHeapAllocate(u32, void*, u32)
ext 0273AF40 void hdSceneDelete(void*)
ext 02752A84 void hdSceneScreenConstruct(hdSceneScreen_c*)
ext 027C0804 void hdSceneScreenDestroy(hdSceneScreen_c*, u32)
ext 02752B0C void hdSceneDisposerConstruct(hdSceneDisposer_c*, void*, u32)
ext 02752BEC void hdSceneDisposerDestroy(hdSceneDisposer_c*, u32)
ext 027C23F8 void hdSceneAttachScreen(mDoLibCamera*, u32, hdSceneScreen_c*)
ext C0006988 void hdSceneDrawSetup(u32, u32, u32, u32, u32, u32, u32)
ext 0272AEE0 void hdSceneDrawPrepare(void*)
ext 026FBA38 u32 hdLayoutResMgr_c::unload(hdScreenString*)
ext 02618094 u32 hdInputManager::setMode(u32)
data sEndingDrawDelegateVtable 1013E840
data sEndingDrawMethod 02707FD4
data sEndingMainDisplayState 10493F24
data sEndingEventSound 101D600C
data sEndingSceneRequestContext 101D5E94
str 1013E850 "EndingEnd_00"
str 1013E860 "Tv" in 02707D70
str 1013E863 "" in 02708090
ext 026AFA1C hdParts_c* hdSceneShortcutConstruct(hdParts_c*)
ext 025DC83C u32 hdSceneSelectProcess(u32)
ext 025DC86C void hdSceneRequestChange(u32, u32, u32, u32, u32)
ext 025F05F8 void hdSceneResetAudio(void*)
*/
