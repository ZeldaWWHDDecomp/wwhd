/* HD boot scene: resource lifetime, drawing and singleton ownership. */
#include "d/hd_scene_boot.h"
#include "gc_api/m_Do/m_Do_audio.h"
#include "d/hd_screen_boot.h"
static hdSceneBoot_c* sBootInstance;
static hdSceneDisposer_c* sBootDisposer;
static hdScreenStringTable sBootStringVtable;
static u32 sBootDelegateBaseVtable;
static u32 sBootDelegateVtable;
static u32 sBootScreenVtable;
static u32 sBootDisposerVtable;
static hdSceneHeaderWords_c sBootHeaderWords;
static hdSceneHeaderRange_c sBootHeaderRange;
static hdSceneRegistration_c sBootHeaderRegistration;
static hdSceneRegistration_c sBootSignedRegistration;
static hdSceneRegistration_c sBootUnsignedRegistration;
static f32 sBootLower;
static f32 sBootUpper;
static hdLayoutResMgr_c* sBootLayoutManager;
static mDoLibCameraSystem* sBootCameraSystem;
static mDoLibMatrix44 sBootDefaultProjection;
static Mtx34 sBootDefaultView;
static void* sBootDrawRenderer;

/* 0270881C */
hdSceneBoot_c* hdSceneBoot_c::construct(hdSceneBoot_c* self) {
    if (!self) {
        self = static_cast<hdSceneBoot_c*>(hdSceneAllocate(sizeof(hdSceneBoot_c)));
        if (!self) return NULL;
    }
    hdSceneScreenConstruct(&self->mScreen);
    hdScreenString* name = gabi::at<hdScreenString>(gabi::ea(self) + offsetof(hdSceneBoot_c, mScreen) + offsetof(hdSceneScreen_c, mDisplayName));
    if (!name) name = static_cast<hdScreenString*>(hdSceneAllocate(sizeof(hdScreenString)));
    if (name) {
        name->mVtable = &sBootStringVtable;
        name->mText.v = gabi::guest_addr("");
    }
    self->mScreen.mEnabled = 0;
    self->mScreen.mFlags = 0;
    hdSceneDrawDelegate_c* delegate = gabi::at<hdSceneDrawDelegate_c>(gabi::ea(self) + offsetof(hdSceneBoot_c, mScreen) + offsetof(hdSceneScreen_c, mDrawDelegate));
    self->mScreen.mpVtable = &sBootScreenVtable;
    if (delegate) {
        hdSceneDrawDelegate_c* storage = delegate;
        if (!storage) storage = static_cast<hdSceneDrawDelegate_c*>(hdSceneAllocate(sizeof(hdSceneDrawDelegate_c)));
        if (storage) {
            storage->mpMethod = NULL;
            storage->mThisAdjustment = 0;
            storage->mpVtable = &sBootDelegateBaseVtable;
            storage->mVirtualIndex = 0;
            storage->mpOwner = NULL;
        }
        delegate->mpVtable = &sBootDelegateVtable;
    }
    self->mpMainParts = NULL;
    return self;
}
/* 02708910 */
void hdSceneBoot_c::createInstance(void* heap) {
    if (sBootInstance) return;
    hdSceneBoot_c* instance = static_cast<hdSceneBoot_c*>(hdSceneHeapAllocate(sizeof(hdSceneBoot_c), heap, 4));
    if (instance) {
        hdSceneDisposerConstruct(&instance->mDisposer, heap, 3);
        instance->mDisposer.mpVtable = reinterpret_cast<hdScreenDeleteTable*>(&sBootDisposerVtable);
    }
    sBootDisposer = reinterpret_cast<hdSceneDisposer_c*>(instance);
    sBootInstance = instance ? construct(instance) : NULL;
}
/* 027089A4 */
void hdSceneBoot_c::destroy(hdSceneBoot_c* self, u32 flags) {
    if (!self) return;
    hdSceneScreenDestroy(&self->mScreen, 2);
    if (flags & 1) hdSceneDelete(self);
}
/* 027089F8 */
void hdSceneBoot_c::destroyInstance() {
    hdSceneDisposer_c* disposer = sBootDisposer;
    if (!disposer) return;
    sBootDisposer = NULL;
    gabi::call_ptr<void>(disposer->mpVtable->mDestroy.get(), disposer, 2u);
    destroy(sBootInstance, 3);
    sBootInstance = NULL;
}
/* 02708A64 */
void hdSceneBoot_c::destroyDisposer(hdSceneDisposer_c* disposer, u32 flags) {
    if (!disposer) return;
    disposer->mpVtable = reinterpret_cast<hdScreenDeleteTable*>(&sBootDisposerVtable);
    if (disposer == sBootDisposer) {
        hdSceneBoot_c* instance = sBootInstance;
        sBootDisposer = NULL;
        destroy(instance, 2);
        sBootInstance = NULL;
    }
    hdSceneDisposerDestroy(disposer, 0);
    if (flags & 1) hdSceneDelete(disposer);
}
/* 02708B00 */
void hdSceneBoot_c::staticInit() {
    sBootHeaderWords.mWords[2] = 0;
    sBootHeaderWords.mWords[0] = 0;
    sBootHeaderWords.mWords[3] = 0;
    sBootHeaderWords.mWords[1] = 0;
    hdSceneRegisterGlobal(&sBootHeaderRegistration);
    f32 lower = sBootLower;
    f32 upper = sBootUpper;
    sBootHeaderRange.mLower = lower;
    sBootHeaderRange.mUpper = upper;
    hdSceneConstructSigned(&sBootHeaderRange.mSigned);
    hdSceneRegisterGlobal(&sBootSignedRegistration);
    hdSceneConstructUnsigned(&sBootHeaderRange.mUnsigned);
    hdSceneRegisterGlobal(&sBootUnsignedRegistration);
}
static u32 sBootDrawDelegateVtable;
static u32 sBootDrawMethod;
extern void hdSceneSetSoundMode(u32);
extern void hdSceneStartSound(u32);
/* 027084CC */
void hdSceneBoot_c::cacheArchive() {
    hdScreenString name;
    name.mVtable = &sBootStringVtable;
    name.mText.v = gabi::guest_addr("Boot_00");
    sBootLayoutManager->cache(&name, NULL);
}
/* 02708514 */
void hdSceneBoot_c::unloadArchive() {
    hdScreenString name;
    name.mText.v = gabi::guest_addr("Boot_00");
    name.mVtable = &sBootStringVtable;
    sBootLayoutManager->unload(&name);
}
/* 02708558 */
void hdSceneBoot_c::createParts() {
    mDoLibCameraSystem* system = sBootCameraSystem;
    u32 count = system->cameraCount;
    gptr<mDoLibCamera>* cameras = system->cameras;
    mDoLibCamera* camera = *gabi::at<gptr<mDoLibCamera>>(gabi::ea(cameras) + (count > 3 ? 12 : 0));
    hdBoot_c* parts = static_cast<hdBoot_c*>(hdSceneAllocate(sizeof(hdBoot_c)));
    if (parts) parts = hdBoot_c::construct(parts);
    mpMainParts = parts;
    u32 flags = camera->flags;
    mDoLibMatrix44* projection;
    if (flags & mDoLibCamera_IndirectProjection) projection = camera->indirectProjection;
    else {
        projection = camera->projection;
        if (!projection) projection = &sBootDefaultProjection;
    }
    Mtx34* view;
    if ((flags & mDoLibCamera_InlineViewA) && (flags & mDoLibCamera_InlineViewB)) view = &camera->inlineView;
    else {
        view = camera->view;
        if (!view) view = &sBootDefaultView;
    }
    parts->initializeBoot(gabi::ea(projection), gabi::ea(view), NULL);
}
/* 02708634 */
void hdSceneBoot_c::initialize() {
    cacheArchive();
    hdSceneDrawDelegate_c* delegate = gabi::at<hdSceneDrawDelegate_c>(gabi::ea(this) + offsetof(hdSceneBoot_c, mScreen) + offsetof(hdSceneScreen_c, mDrawDelegate));
    if (delegate) {
        delegate->mpOwner = this;
        delegate->mThisAdjustment = 0;
        delegate->mpVtable = &sBootDrawDelegateVtable;
        delegate->mpMethod = &sBootDrawMethod;
        delegate->mVirtualIndex = -1;
    }
    mScreen.mEnabled = 1;
    mScreen.mDisplayName.mText.v = gabi::guest_addr("Tv");
    mDoLibCameraSystem* system = sBootCameraSystem;
    u32 count = system->cameraCount;
    gptr<mDoLibCamera>* cameras = system->cameras;
    mDoLibCamera* camera = *gabi::at<gptr<mDoLibCamera>>(gabi::ea(cameras) + (count > 3 ? 12 : 0));
    hdSceneAttachScreen(camera, 3, &mScreen);
    createParts();
    hdInputGetManager()->setMode(3);
    hdSceneSetSoundMode(2);
}
/* 027086F8 */
void hdSceneBoot_c::executeParts() { mpMainParts->vExecuteState(); }
/* 0270870C */
void hdSceneBoot_c::execute() { executeParts(); hdSceneStartSound(0x7070); }
/* 02708734 */
void hdSceneBoot_c::releaseParts() { mpMainParts->vFinalize(); }
/* 02708748 */
void hdSceneBoot_c::release() { releaseParts(); unloadArchive(); }
/* 0270877C */
void hdSceneBoot_c::draw(hdSceneDrawInfo_c* info) {
    hdSceneDrawSetup(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdSceneDrawPrepare(sBootDrawRenderer);
    u32 index = info->mViewIndex;
    mDoLibCameraSystem* system = sBootCameraSystem;
    hdParts_c* parts = mpMainParts;
    u32 count = system->cameraCount;
    gptr<mDoLibCamera>* cameras = system->cameras;
    hdPartsVtbl_c* table = parts->mVtable;
    u32 selected = index < count ? index : 0;
    mDoLibCamera* camera = *gabi::at<gptr<mDoLibCamera>>(gabi::ea(cameras) + selected * 4);
    void* context = gabi::at<void>(gabi::ea(camera) + offsetof(hdBootCameraDrawContext_l, mDrawContext));
    gabi::call_ptr<void>(table->mDraw.get(), parts, context);
}
/* WWHD glue
ext 02628C94 hdBoot_c* hdBoot_c::construct(hdBoot_c*)
ext 02628E10 u32 hdBoot_c::initializeBoot(u32,u32,void*)
data sBootInstance 101F8294
data sBootDisposer 101F8298
data sBootStringVtable 1013E8A0
data sBootDelegateBaseVtable 1013E888
data sBootDelegateVtable 1013E8B8
data sBootDisposerVtable 1013E900
data sBootHeaderWords 1049E940
data sBootHeaderRange 1049E934
data sBootHeaderRegistration 101F8270
data sBootSignedRegistration 101F827C
data sBootUnsignedRegistration 101F8288
data sBootLower 1013E8F4
data sBootUpper 1013E8F8
data sBootScreenVtable 10169E3C
data sBootLayoutManager 101F7274
data sBootCameraSystem 101F95D0
data sBootDefaultProjection 104A20FC
data sBootDefaultView 104A2098
data sBootDrawRenderer 101F86E8
data sBootDrawDelegateVtable 1013E8D0
data sBootDrawMethod 0270877C
str 1013E8E0 "Boot_00" in 027084CC
str 1013E8E8 "Boot_00" in 02708514
str 1013E8F0 "Tv" in 02708634
str 1013E8F3 "" in 0270881C
ext 026FB694 u32 hdLayoutResMgr_c::cache(hdScreenString*, hdLayoutHeap_c*)
ext 020309AC void hdSceneSetSoundMode(u32)
ext 025E1988 void hdSceneStartSound(u32)
*/
