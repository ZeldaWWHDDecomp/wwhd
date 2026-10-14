/** HD pause transitions. */
#include "d/hd_screen_pause.h"
namespace gc_tu {
static u32 sPauseStringVtable;
static hdPartsStateIdVtbl_c sPauseStateVtable;
static u32 sPauseVtable;
static u32 sPauseHolderVtable;
static u32 sPauseSecondaryVtable;
}
using namespace gc_tu;
static hdUiSafeString sPauseArchiveName;
static hdUiSafeString sPauseName;
static hdPartsStateId_c sPauseOutAll;
static u32 sPauseMode;
static f32 sPauseAnimationZero = 0.0f;
static hdPartsStateId_c sPauseDisplay;
static hdPartsStateId_c sPauseBlank;
static hdPartsStateId_c sPauseSetTexture;
static f32 sPauseAnimationStart = 1.0f;
struct hdPauseHeaderAngles_l { be<f32> mMin, mMax; };
struct hdPauseHeaderRectangle_l { be<f32> mLeft, mTop, mRight, mBottom; };
struct hdPauseHeaderFlags_l { be<u8> mFirst, mSecond; };
struct hdPauseHeaderZeros_l { be<u32> mWords[4]; };
struct hdPauseRegistration_l { be<u32> mWords[3]; };
struct hdPauseRegistrations_l { hdPauseRegistration_l mEntries[3]; };
static hdPauseHeaderAngles_l sPauseHeaderAngles;
static hdPauseHeaderRectangle_l sPauseHeaderRectangle;
static hdPauseHeaderFlags_l sPauseHeaderFlags;
static hdPauseHeaderZeros_l sPauseHeaderZeros;
static hdPauseRegistrations_l sPauseRegistrations;
static f32 sPauseNegativePi = -3.1415927410125732f;
static f32 sPausePositivePi = 3.1415927410125732f;
static f32 sPauseRectangleLimit = 50000.0f;
static f32 sPauseRectangleSize = 10000.0f;
static u32 sPauseStateCounterReady;
static u32 sPauseStateCounter;
static hdPartsStateId_c sPauseParentBlank;

static hdStateId sPauseNullState;
void pauseRegister(hdPauseRegistration_l* registration);
void pauseConstructHeaderFirst(be<u8>* flag);
void pauseConstructHeaderSecond(be<u8>* flag);

static hdPartsStateId_c sPauseInAll;
struct hdPausePaneNames_l { hdUiSafeString mNames[5]; };
static hdPausePaneNames_l sPausePaneNames;
struct hdPauseAnimationNames_l { hdUiSafeString mNames[6]; };
static hdPauseAnimationNames_l sPauseAnimationNames;
struct hdPausePaneIndices_l { be<u32> mIndices[6]; };
static hdPausePaneIndices_l sPausePaneIndices;
struct hdPauseHeapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
WWHD_OFFSET(hdPauseHeapManager_l, mpHeap, 0x10);
struct hdPauseHeapPointer_l { gptr<hdPauseHeapManager_l> mpManager; };
static hdPauseHeapPointer_l sPauseHeapManager;

/* 026C016C */
static u32 pauseStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026C026C */
static void destroyPause(hdPause_c* panel, u32 flags) {
    if (!panel) return;
    panel->mStateHolder.mHolderVtable.v = gabi::ea(&sPauseHolderVtable);
    panel->mSecondaryVtable.v = gabi::ea(&sPauseSecondaryVtable);
    pauseDestroyBase(panel, 0);
    if (flags & 1) operator_delete(panel);
}
/* 026C02D8 */
static void pauseWaitExecute(hdPause_c* panel) {
    if (pauseCanBegin(panel)) panel->changeState(&sPauseSetTexture);
}
struct hdPauseManagerPointer_l { gptr<void> mpManager; };
static hdPauseManagerPointer_l sPausePictureManager;
static hdPauseManagerPointer_l sPausePictureLock;
struct hdPauseFormat_l { be<u32> mValue; };
/* 026C031C */
void hdPause_c::setTexture() {
    u32 mode = sPauseMode;
    hdPartsPaneSRT_c* pane;
    hdPausePictureEntry_l* entry;
    if (mode == 2) {
        pane = ((hdPartsPaneSRT_c*)mpLayout->lyt()->mpRootPane.get())->find("PF_PauseTV_00");
        entry = pauseGetPicture(sPausePictureManager.mpManager, 7);
    } else if (mode == 4) {
        pane = ((hdPartsPaneSRT_c*)mpLayout->lyt()->mpRootPane.get())->find("PF_PauseDRC_00");
        entry = pauseGetPicture(sPausePictureManager.mpManager, 8);
    } else {
        changeState(&sPauseInAll);
        return;
    }
    if (entry) {
        pauseLockPictures(sPausePictureLock.mpManager);
        // The RPX import stub at 028FDC00 resolves to coreinit OSBlockMove.
        pauseCopyTexture(&mTexture, &entry->mTextureData, 0x74, 0);
        u32 extentY = mTexture.mExtentY;
        mTexture.mImageOffset = 0;
        mTexture.mMipOffset = 0;
        u32 extentX = mTexture.mExtentX;
        mTexture.mComponentOrder = 0x10203;
        mTexture.mImageExtentY = extentY;
        mTexture.mImageExtentX = extentX;
        pauseInitTextureRegisters(&mTexture);
        hdPauseFormat_l format;
        pauseGetTextureFormat(&format.mValue, 0x1A, 1);
        s32 width = entry->mMinimumWidth;
        s32 textureWidth = entry->mTextureData.mWidth.get();
        if (textureWidth > width) width = textureWidth;
        s32 height = entry->mMinimumHeight;
        s32 textureHeight = entry->mTextureData.mHeight.get();
        mPictureFormat = (u32)format.mValue;
        if (textureHeight > height) height = textureHeight;
        mPictureActive = 1;
        mPictureWidth = width;
        mPictureHeight = height;
        pauseUnlockPictures(sPausePictureLock.mpManager);
        pauseInstallPicture(pane, &mPictureActive, 0, 0);
    }
    changeState(&sPauseInAll);
}
/* 026C04E0 */
void hdPause_c::setAutomatic(u32 enabled) {
    hdAnimSet* animation = animSet();
    hdAnimSet_setFrame(animation, 4, 4, enabled ? sPauseAnimationStart : sPauseAnimationZero);
}
/* 026C0518 */
void hdPause_c::inAllEnter() {
    const f32 one = sPauseAnimationStart;
    const f32 zero = sPauseAnimationZero;
    if (mAutomatic) {
        mAutomatic = 0;
        pauseSetAutomatic(this, true);
    } else {
        hdAnimSet* animation = animSet();
        u8 mode = mInputMode;
        hdAnimSet_setFrame(animation, 3, 3, mode ? one : zero);
    }
    u32 mode = sPauseMode;
    if (mode == 2) {
        hdAnimSet_setFrame(animSet(), 2, 2, zero);
        hdAnimSet_setFrame(animSet(), 1, 1, one);
    } else if (mode == 4) {
        hdAnimSet_setFrame(animSet(), 2, 2, one);
        hdAnimSet_setFrame(animSet(), 1, 1, zero);
    }
    hdAnimSet_play(animSet(), 0, 0, one);
    pauseSetAnimationRate(animSet(), 0, 0, one);
    vCalcLayout();
}
/* 026C06F4 */
void hdPause_c::inAllExecute() {
    if (hdAnimSet_isEnd(animSet(), 0)) changeState(&sPauseDisplay);
    vCalcLayout();
}
/* 026C0758 */
void hdPause_c::outAllEnter() { hdAnimSet_play(animSet(), 5, 0, sPauseAnimationStart); }
/* 026C0774 */
void hdPause_c::outAllExecute() {
    if (pauseAnimationStopped(animSet(), 5)) changeState(&sPauseBlank);
    vCalcLayout();
}
/* 026C07D8 */
static hdPause_c* constructPause(hdPause_c* panel) {
    if (!panel) {
        panel = (hdPause_c*)operator_new(sizeof(hdPause_c));
        if (!panel) return nullptr;
    }
    pauseConstructBase(panel);
    panel->mStateHolder.mHolderVtable.v = gabi::ea(&sPauseHolderVtable);
    panel->mVtable.v = gabi::ea(&sPauseVtable);
    panel->mSecondaryVtable.v = gabi::ea(&sPauseSecondaryVtable);
    panel->mPictureWidth = 0;
    panel->mPictureActive = 0;
    panel->mAutomatic = 0;
    panel->mInputMode = 0;
    panel->mPictureHeight = 0;
    return panel;
}
/* 026C085C */
u32 hdPause_c::setup(u32 archive, u32 resource, u32 flags) {
    pauseAttachArchives(this, &sPauseArchiveName, 1);
    if (!vLoad(&sPauseName, mpResAccessor.v.get(), archive, resource, gabi::at<void>(flags))) return 0;
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sPauseBlank);
    return 1;
}
/* 026C096C */
void hdPause_c::release() { vDestroyLayout(); pauseDetachArchives(this); }
/* 026C09AC */
void hdPause_c::stateExecute() { pauseExecuteState(stateMachine()); }
/* 026C09B4 */
void hdPause_c::requestOut() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateIdVtbl_c* currentTable = current->mVtable;
    hdPartsStateIdVtbl_c* blankTable = sPauseBlank.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentTable->mGetKey.get(), current);
    u32 blankKey = gabi::call_ptr<u32>(blankTable->mGetKey.get(), &sPauseBlank);
    if (currentKey != blankKey) changeState(&sPauseOutAll);
}
/* 026C0A4C */
void hdPause_c::reset() {
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    changeState(&sPauseBlank);
}
/* 026C0AAC */
u32 hdPause_c::loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor) {
    hdPartsLayout_c* layout = pauseAllocateLayout(sizeof(hdPartsLayout_c), sPauseHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!layout->vLoad(name, archive, resource, flags, 6, 5, descriptor)) return 0;
    for (u32 i = 0; i < 6; ++i) {
        u32 paneIndex = sPausePaneIndices.mIndices[i];
        hdUiSafeString* pane = gabi::at<hdUiSafeString>(gabi::ea(&sPausePaneNames) + paneIndex * sizeof(hdUiSafeString));
        pauseBindAnimation(animSet(), i, &sPauseAnimationNames.mNames[i], pane);
    }
    vCalcLayout();
    return 1;
}
/* 026C0BD8 */
static void initializePauseStatics() {
    for (u32 i = 0; i < 4; ++i) sPauseHeaderZeros.mWords[i] = 0;
    pauseRegister(&sPauseRegistrations.mEntries[0]);
    sPauseHeaderAngles.mMin = sPauseNegativePi;
    sPauseHeaderAngles.mMax = sPausePositivePi;
    pauseConstructHeaderFirst(&sPauseHeaderFlags.mFirst);
    pauseRegister(&sPauseRegistrations.mEntries[1]);
    pauseConstructHeaderSecond(&sPauseHeaderFlags.mSecond);
    pauseRegister(&sPauseRegistrations.mEntries[2]);
    f32 limit = sPauseRectangleLimit;
    f32 size = sPauseRectangleSize;
    sPauseHeaderRectangle.mLeft = limit;
    sPauseHeaderRectangle.mBottom = size;
    sPauseHeaderRectangle.mRight = size;
    sPauseHeaderRectangle.mTop = limit;
    sPausePaneNames.mNames[0].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPausePaneNames.mNames[0].mText.v = gabi::guest_addr("G_All_00");
    sPausePaneNames.mNames[1].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPausePaneNames.mNames[1].mText.v = gabi::guest_addr("G_DRC_00");
    sPausePaneNames.mNames[2].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPausePaneNames.mNames[2].mText.v = gabi::guest_addr("G_TV_00");
    sPausePaneNames.mNames[3].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPausePaneNames.mNames[3].mText.v = gabi::guest_addr("G_AutomaticPause_00");
    sPauseAnimationNames.mNames[0].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    sPauseAnimationNames.mNames[1].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseAnimationNames.mNames[1].mText.v = gabi::guest_addr("Visible");
    sPauseAnimationNames.mNames[2].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseAnimationNames.mNames[2].mText.v = gabi::guest_addr("Visible");
    sPauseAnimationNames.mNames[3].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseAnimationNames.mNames[3].mText.v = gabi::guest_addr("AutomaticPause");
    sPauseAnimationNames.mNames[4].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseAnimationNames.mNames[4].mText.v = gabi::guest_addr("SpecialDRC");
    sPausePaneNames.mNames[4].mVtable.v = gabi::ea(&sPauseStringVtable);
    u32 ready = sPauseStateCounterReady;
    sPausePaneNames.mNames[4].mText.v = gabi::guest_addr("G_SpecialDRC_00");
    sPauseName.mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseArchiveName.mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseArchiveName.mText.v = gabi::guest_addr("Pause_00");
    sPauseAnimationNames.mNames[5].mVtable.v = gabi::ea(&sPauseStringVtable);
    sPauseName.mText.v = gabi::guest_addr("Pause_00");
    sPauseAnimationNames.mNames[5].mText.v = gabi::guest_addr("OutAll");
    u32 number;
    if (ready) number = sPauseStateCounter;
    else { number = 0; sPauseStateCounterReady = 1; }
    hdPartsStateIdVtbl_c* table = &sPauseStateVtable;
    sPauseBlank.setup(number + 1, "::StateID_Blank", table, 4, (hdStateId*)&sPauseParentBlank);
    sPauseSetTexture.setup(number + 2, "::StateID_SetTexture", table, 0x19, &sPauseNullState);
    sPauseInAll.setup(number + 3, "::StateID_InAll", table, 0x1C, &sPauseNullState);
    sPauseOutAll.setup(number + 4, "::StateID_OutAll", table, 0x1F, &sPauseNullState);
    sPauseStateCounter = number + 4;
}
/* WWHD glue
 data sPauseStringVtable 101001F4 nocheck
 data sPauseStateVtable 1010026C nocheck
 data sPauseVtable 101003C0 nocheck
 data sPauseHolderVtable 101004D0 nocheck
 data sPauseSecondaryVtable 101004E0 nocheck
data sPausePictureManager 101F9968
data sPausePictureLock 101F8B18
str 101002BC "PF_PauseTV_00" in 026C031C
str 101002CC "PF_PauseDRC_00" in 026C031C
ext 027F81A4 hdPausePictureEntry_l* pauseGetPicture(void* manager, u32 slot)
ext 0274FBF8 void pauseLockPictures(void* manager)
ext 0274FCCC void pauseUnlockPictures(void* manager)
ext C0009988 void pauseCopyTexture(hdPauseTexture_l* destination, void* source, u32 size, u32 flags)
ext C0006498 void pauseInitTextureRegisters(hdPauseTexture_l* texture)
ext 0287F1EC void pauseGetTextureFormat(be<u32>* format, u32 kind, u32 flags)
ext 02704F3C void pauseInstallPicture(hdPartsPaneSRT_c* pane, void* picture, u32 flags, u32 extra)
data sPauseInAll 10499584
data sPauseHeaderAngles 104994A0
data sPauseHeaderRectangle 104994A8
data sPauseHeaderFlags 104994B8
data sPauseHeaderZeros 104994BC
data sPauseRegistrations 101F6854
data sPauseNegativePi 101002E0
data sPausePositivePi 101002E4
data sPauseRectangleLimit 101002E8
data sPauseRectangleSize 101002EC
data sPauseStateCounterReady 101FD8E4
data sPauseStateCounter 101FDD50
data sPauseParentBlank 1049DD60
data sPauseNullState 101FF32C
data sPausePaneNames 104994CC
data sPauseAnimationNames 104994F4
data sPausePaneIndices 1010020C
data sPauseHeapManager 1018C404
ext 028F026C void pauseRegister(hdPauseRegistration_l* registration)
ext 028ED6F8 void pauseConstructHeaderFirst(be<u8>* flag)
ext 028EAB2C void pauseConstructHeaderSecond(be<u8>* flag)
ext 0273B050 hdPartsLayout_c* pauseAllocateLayout(u32 size, void* heap, u32 alignment)
ext 02004E04 void pauseBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
str 10100354 "G_All_00" in 026C0BD8
str 10100360 "G_DRC_00" in 026C0BD8
str 101002F0 "G_TV_00" in 026C0BD8
str 10100340 "G_AutomaticPause_00" in 026C0BD8
str 10100310 "G_SpecialDRC_00" in 026C0BD8
str 10100300 "InAll" in 026C0BD8
str 101002F8 "Visible" in 026C0BD8
str 1010036C "AutomaticPause" in 026C0BD8
str 1010037C "SpecialDRC" in 026C0BD8
str 10100308 "OutAll" in 026C0BD8
str 10100388 "Pause_00" in 026C0BD8
str 10100320 "::StateID_Blank" in 026C0BD8
str 10100394 "::StateID_SetTexture" in 026C0BD8
str 10100330 "::StateID_InAll" in 026C0BD8
str 101003AC "::StateID_OutAll" in 026C0BD8
data sPauseMode 101F8348
data sPauseAnimationZero 101002DC
data sPauseArchiveName 10499498
data sPauseName 104995E4
data sPauseOutAll 104995B4
data sPauseDisplay 1049DD90
data sPauseBlank 10499524
data sPauseSetTexture 10499554
data sPauseAnimationStart 101002B8
ext 026F88F0 void pauseDestroyBase(hdPause_c* panel, u32 flags)
ext 026F89E8 hdPause_c* pauseConstructBase(hdPause_c* panel)
ext 026F90D8 void pauseAttachArchives(hdPause_c* panel, hdUiSafeString* names, u32 count)
ext 026C04E0 void pauseSetAutomatic(hdPause_c* panel, u32 enabled)
ext 0200579C void pauseSetAnimationRate(hdAnimSet* animation, u32 entry, u32 slot, f32 rate)
ext 027172D0 bool pauseCanBegin(hdPause_c* panel)
ext 0200588C bool pauseAnimationStopped(hdAnimSet* animation, u32 track)
ext 026F91F8 void pauseDetachArchives(hdPause_c* panel)
ext 02006364 void pauseExecuteState(hdStateMachine* machine)
*/
