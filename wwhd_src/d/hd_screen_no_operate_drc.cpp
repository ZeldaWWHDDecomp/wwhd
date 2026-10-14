/** HD GamePad noOperate panel transitions. */
#include "d/hd_screen_no_operate_drc.h"
namespace gc_tu {
static u32 sNoOperateVtable;
static u32 sNoOperateHolderVtable;
static u32 sNoOperateSecondaryVtable;
static u32 sNoOperateStringVtable;
static hdPartsStateIdVtbl_c sNoOperateStateVtable;
}
using namespace gc_tu;
static hdPartsStateId_c sNoOperateInAll;
static hdPartsStateId_c sNoOperateDisplay;
static hdPartsStateId_c sNoOperateOutAll;
static hdPartsStateId_c sNoOperateBlank;
static hdUiSafeString sNoOperatePaneName;
static hdUiSafeString sNoOperateArchiveName;
static hdUiSafeString sNoOperateName;
struct hdNoOperateAnimationNames_l { hdUiSafeString mNames[2]; };
static hdNoOperateAnimationNames_l sNoOperateAnimationNames;
struct hdNoOperatePaneIndices_l { be<u32> mIndices[2]; };
static hdNoOperatePaneIndices_l sNoOperatePaneIndices;
static f32 sNoOperateAnimationStart = 1.0f;
static f32 sNoOperatePointerOffset = 48.0f;
struct hdNoOperateHeapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
WWHD_OFFSET(hdNoOperateHeapManager_l, mpHeap, 0x10);
struct hdNoOperateHeapPointer_l { gptr<hdNoOperateHeapManager_l> mpManager; };
static hdNoOperateHeapPointer_l sNoOperateHeapManager;
struct hdNoOperateControllerManagerPointer_l { gptr<void> mpManager; };
static hdNoOperateControllerManagerPointer_l sNoOperateControllerManager;
static f32 sNoOperateZero = 0.0f;
struct hdNoOperateHeaderAngles_l { be<f32> mMin, mMax; };
struct hdNoOperateHeaderRectangle_l { be<f32> mLeft, mTop, mRight, mBottom; };
struct hdNoOperateHeaderFlags_l { be<u8> mFirst, mSecond; };
struct hdNoOperateHeaderZeros_l { be<u32> mWords[4]; };
struct hdNoOperateRegistration_l { be<u32> mWords[3]; };
struct hdNoOperateRegistrations_l { hdNoOperateRegistration_l mEntries[3]; };
static hdNoOperateHeaderAngles_l sNoOperateHeaderAngles;
static hdNoOperateHeaderRectangle_l sNoOperateHeaderRectangle;
static hdNoOperateHeaderFlags_l sNoOperateHeaderFlags;
static hdNoOperateHeaderZeros_l sNoOperateHeaderZeros;
static hdNoOperateRegistrations_l sNoOperateRegistrations;
static f32 sNoOperateNegativePi = -3.1415927410125732f;
static f32 sNoOperatePositivePi = 3.1415927410125732f;
static f32 sNoOperateRectangleLimit = 50000.0f;
static f32 sNoOperateRectangleSize = 10000.0f;
static u32 sNoOperateStateCounterReady;
static u32 sNoOperateStateCounter;
static hdPartsStateId_c sNoOperateParentBlank;
static hdPartsStateId_c sNoOperateParentDisplay;
static hdStateId sNoOperateNullState;
void noOperateRegister(hdNoOperateRegistration_l* registration);
void noOperateConstructHeaderFirst(be<u8>* flag);
void noOperateConstructHeaderSecond(be<u8>* flag);

static u32 noOperateFloatBits(f32 value) {
    union { f32 value; u32 bits; } result;
    result.value = value;
    return result.bits;
}


/* 026BDA64 */
static void destroyNoOperate(hdNoOperateDRC_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sNoOperateHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sNoOperateSecondaryVtable);
    noOperateDestroyBase(screen, 0);
    if (flags & 1) operator_delete(screen);
}
/* 026BDAD0 */
void hdNoOperateDRC_c::waitExecute() {
    hdController* controller = noOperateControllerByOrder(sNoOperateControllerManager.mpManager, 8, 0);
    if (noOperateHasPad(hdInputGetManager()) && controller && !mDisabled && noOperateCanUpdate() &&
        (u32(controller->mRepeat) & 0x8000)) changeState(&sNoOperateInAll);
}
/* 026BDB68 */
void hdNoOperateDRC_c::displayExecute() {
    hdController* controller = noOperateControllerByOrder(sNoOperateControllerManager.mpManager, 8, 0);
    if (!noOperateHasPad(hdInputGetManager()) || !controller) return;
    if (!(u32(controller->mRepeat) & 0x8000) || !noOperateCanUpdate() || mDisabled) changeState(&sNoOperateOutAll);
}
/* 026BDC08 */
void hdNoOperateDRC_c::updatePosition() {
    hdController* controller = noOperateControllerByOrder(sNoOperateControllerManager.mpManager, 8, 0);
    if (!noOperateHasPad(hdInputGetManager()) || !controller || !(u32(controller->mRepeat) & 0x8000)) return;
    hdInputManager* manager = hdInputGetManager();
    f32 y = f32(controller->mPointerY) - f32(manager->mPointer.mOffsetY);
    f32 offset = mPointerOffsetY;
    hdPartsPaneSRT_c* root = (hdPartsPaneSRT_c*)mpLayout->lyt()->mpRootPane.get();
    f32 x = f32(controller->mPointerX) - f32(manager->mPointer.mOffsetX);
    f32 scaleY = manager->mPointer.mScaleY;
    f32 scaleX = manager->mPointer.mScaleX;
    y = gabi::fmadds(y, scaleY, offset);
    x = gabi::fmuls_ppc(x, scaleX);
    f32 z = sNoOperateZero;
    if (!root) return;
    u8 flags = root->mFlags;
    root->mTranslationYBits = noOperateFloatBits(y);
    root->mTranslationXBits = noOperateFloatBits(x);
    root->mFlags = flags | 0x10;
    root->mTranslationZBits = noOperateFloatBits(z);
}
/* 026BDCE4 */
void hdNoOperateDRC_c::inAllEnter() { noOperateUpdatePosition(this); hdAnimSet_play(animSet(), 0, 0, sNoOperateAnimationStart); }
/* 026BDD2C */
void hdNoOperateDRC_c::inAllExecute() {
    if (hdAnimSet_isEnd(animSet(), 0)) changeState(&sNoOperateDisplay);
    vCalcLayout();
}
/* 026BDD90 */
void hdNoOperateDRC_c::outAllEnter() { hdAnimSet_play(animSet(), 1, 0, sNoOperateAnimationStart); }
/* 026BDDAC */
void hdNoOperateDRC_c::outAllExecute() {
    if (hdAnimSet_isEnd(animSet(), 1)) changeState(&sNoOperateBlank);
    vCalcLayout();
}
/* 026BDE10 */
static hdNoOperateDRC_c* constructNoOperate(hdNoOperateDRC_c* screen) {
    if (!screen) {
        screen = (hdNoOperateDRC_c*)operator_new(sizeof(hdNoOperateDRC_c));
        if (!screen) return nullptr;
    }
    noOperateConstructBase(screen);
    screen->mVtable.v = gabi::ea(&sNoOperateVtable);
    screen->mPointerOffsetY = sNoOperatePointerOffset;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sNoOperateHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sNoOperateSecondaryVtable);
    screen->mDisabled = 0;
    return screen;
}
/* 026BDE90 */
u32 hdNoOperateDRC_c::setup(u32 archive, u32 resource, u32 flags) {
    noOperateAttachArchives(this, &sNoOperateArchiveName, 1);
    if (!vLoad(&sNoOperateName, mpResAccessor.v.get(), archive, resource, gabi::at<void>(flags))) return 0;
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sNoOperateBlank);
    return 1;
}
/* 026BDFA0 */
void hdNoOperateDRC_c::release() { vDestroyLayout(); noOperateDetachArchives(this); }
/* 026BDFE0 */
void hdNoOperateDRC_c::stateExecute() { noOperateExecuteState(stateMachine()); }
/* 026BDFE8 */
u32 hdNoOperateDRC_c::loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor) {
    hdPartsLayout_c* layout = noOperateAllocateLayout(sizeof(hdPartsLayout_c), sNoOperateHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!layout->vLoad(name, archive, resource, flags, 2, 1, descriptor)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        u32 paneIndex = sNoOperatePaneIndices.mIndices[i];
        hdUiSafeString* pane = gabi::at<hdUiSafeString>(gabi::ea(&sNoOperatePaneName) + paneIndex * sizeof(hdUiSafeString));
        noOperateBindAnimation(animSet(), i, &sNoOperateAnimationNames.mNames[i], pane);
    }
    vCalcLayout();
    return 1;
}
/* HD: translation-unit state and resource initialization. */
/* 026BE114 */
static void initializeNoOperateStatics() {
    for (u32 i = 0; i < 4; ++i) sNoOperateHeaderZeros.mWords[i] = 0;
    noOperateRegister(&sNoOperateRegistrations.mEntries[0]);
    sNoOperateHeaderAngles.mMin = sNoOperateNegativePi;
    sNoOperateHeaderAngles.mMax = sNoOperatePositivePi;
    noOperateConstructHeaderFirst(&sNoOperateHeaderFlags.mFirst);
    noOperateRegister(&sNoOperateRegistrations.mEntries[1]);
    noOperateConstructHeaderSecond(&sNoOperateHeaderFlags.mSecond);
    noOperateRegister(&sNoOperateRegistrations.mEntries[2]);
    f32 size = sNoOperateRectangleSize;
    sNoOperateHeaderRectangle.mBottom = size;
    f32 limit = sNoOperateRectangleLimit;
    sNoOperateName.mVtable.v = gabi::ea(&sNoOperateStringVtable);
    sNoOperateHeaderRectangle.mTop = limit;
    sNoOperateHeaderRectangle.mRight = size;
    sNoOperateHeaderRectangle.mLeft = limit;
    sNoOperateAnimationNames.mNames[0].mVtable.v = gabi::ea(&sNoOperateStringVtable);
    sNoOperateAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    sNoOperateName.mText.v = gabi::guest_addr("NoOperateDRC_00");
    u32 ready = sNoOperateStateCounterReady;
    sNoOperateAnimationNames.mNames[1].mVtable.v = gabi::ea(&sNoOperateStringVtable);
    sNoOperateAnimationNames.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sNoOperatePaneName.mVtable.v = gabi::ea(&sNoOperateStringVtable);
    sNoOperateArchiveName.mVtable.v = gabi::ea(&sNoOperateStringVtable);
    sNoOperatePaneName.mText.v = gabi::guest_addr("G_All_00");
    sNoOperateArchiveName.mText.v = gabi::guest_addr("NoOperateDRC_00");
    u32 number;
    if (ready) number = sNoOperateStateCounter;
    else { number = 0; sNoOperateStateCounterReady = 1; }
    hdPartsStateIdVtbl_c* table = &sNoOperateStateVtable;
    sNoOperateBlank.setup(number + 1, "::StateID_Blank", table, 4, (hdStateId*)&sNoOperateParentBlank);
    sNoOperateDisplay.setup(number + 2, "::StateID_Display", table, 7, (hdStateId*)&sNoOperateParentDisplay);
    sNoOperateInAll.setup(number + 3, "::StateID_InAll", table, 0x19, &sNoOperateNullState);
    sNoOperateOutAll.setup(number + 4, "::StateID_OutAll", table, 0x1C, &sNoOperateNullState);
    sNoOperateStateCounter = number + 4;
}
/* WWHD glue
 data sNoOperateVtable 100FF800 nocheck
 data sNoOperateHolderVtable 100FF8F8 nocheck
 data sNoOperateSecondaryVtable 100FF908 nocheck
 data sNoOperateStringVtable 100FF6C4 nocheck
 data sNoOperateStateVtable 100FF724 nocheck
data sNoOperateHeaderAngles 10498DCC
data sNoOperateHeaderRectangle 10498DD4
data sNoOperateHeaderFlags 10498DE4
data sNoOperateHeaderZeros 10498DE8
data sNoOperateRegistrations 101F678C
data sNoOperateNegativePi 100FF77C
data sNoOperatePositivePi 100FF780
data sNoOperateRectangleLimit 100FF784
data sNoOperateRectangleSize 100FF788
data sNoOperateStateCounterReady 101FD8E4
data sNoOperateStateCounter 101FDD50
data sNoOperateParentBlank 1049DD60
data sNoOperateParentDisplay 1049DD90
data sNoOperateNullState 101FF32C
str 100FF78C "InAll" in 026BE114
str 100FF794 "OutAll" in 026BE114
str 100FF79C "NoOperateDRC_00" in 026BE114
str 100FF7AC "::StateID_Blank" in 026BE114
str 100FF7BC "::StateID_InAll" in 026BE114
str 100FF7CC "::StateID_Display" in 026BE114
str 100FF7E0 "G_All_00" in 026BE114
str 100FF7EC "::StateID_OutAll" in 026BE114
ext 028F026C void noOperateRegister(hdNoOperateRegistration_l* registration)
ext 028ED6F8 void noOperateConstructHeaderFirst(be<u8>* flag)
ext 028EAB2C void noOperateConstructHeaderSecond(be<u8>* flag)
data sNoOperateControllerManager 101F8AE8
data sNoOperateZero 100FF774
data sNoOperatePointerOffset 100FF778
data sNoOperateDisplay 10498E38
data sNoOperateInAll 10498E68
data sNoOperateOutAll 10498E98
data sNoOperateBlank 10498E08
data sNoOperatePaneName 10498DBC
data sNoOperateArchiveName 10498DC4
data sNoOperateName 10498EC8
data sNoOperateAnimationNames 10498DF8
data sNoOperatePaneIndices 100FF6A8
data sNoOperateAnimationStart 100FF770
data sNoOperateHeapManager 1018C404
ext 026BDC08 void noOperateUpdatePosition(hdNoOperateDRC_c* screen)
ext 0273CB44 hdController* noOperateControllerByOrder(void* manager, u32 controller, u32 index)
ext 02617F14 u32 noOperateHasPad(hdInputManager* manager)
ext 02715178 u32 noOperateCanUpdate()
ext 026F88F0 void noOperateDestroyBase(hdNoOperateDRC_c* screen, u32 flags)
ext 026F89E8 hdNoOperateDRC_c* noOperateConstructBase(hdNoOperateDRC_c* screen)
ext 026F90D8 void noOperateAttachArchives(hdNoOperateDRC_c* screen, hdUiSafeString* names, u32 count)
ext 026F91F8 void noOperateDetachArchives(hdNoOperateDRC_c* screen)
ext 02006364 void noOperateExecuteState(hdStateMachine* machine)
ext 0273B050 hdPartsLayout_c* noOperateAllocateLayout(u32 size, void* heap, u32 alignment)
ext 02004E04 void noOperateBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
*/
