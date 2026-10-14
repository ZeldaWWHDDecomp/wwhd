/** HD GamePad information panel transitions. */
#include "d/hd_screen_information_drc.h"
namespace gc_tu {
static u32 sInformationVtable;
static u32 sInformationHolderVtable;
static u32 sInformationSecondaryVtable;
static u32 sInformationStringVtable;
static hdPartsStateIdVtbl_c sInformationStateVtable;
static u32 sInformationHeaderAngles;
static u32 sInformationHeaderRegistrations;
static u32 sInformationHeaderZeros;
static u32 sInformationHeaderPi;
}
using namespace gc_tu;
static hdPartsStateId_c sInformationInAll;
static hdPartsStateId_c sInformationOutAll;
static hdPartsStateId_c sInformationDisableWait;
static hdUiSafeString sInformationPaneName;
static hdUiSafeString sInformationArchiveName;
static hdUiSafeString sInformationName;
struct hdInformationAnimationNames_l { hdUiSafeString mNames[2]; };
static hdInformationAnimationNames_l sInformationAnimationNames;
struct hdInformationPaneIndices_l { be<u32> mIndices[2]; };
static hdInformationPaneIndices_l sInformationPaneIndices;
static f32 sInformationAnimationStart = 1.0f;
static u32 sInformationStateCounterReady;
static u32 sInformationStateCounter;
static hdStateId sInformationNullState;
struct hdInformationHeapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
WWHD_OFFSET(hdInformationHeapManager_l, mpHeap, 0x10);
struct hdInformationHeapPointer_l { gptr<hdInformationHeapManager_l> mpManager; };
static hdInformationHeapPointer_l sInformationHeapManager;

/* 0267232C */
static u32 informationStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0267242C */
static void destroyInformation(hdInformationDRC_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sInformationHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sInformationSecondaryVtable);
    informationDestroyBase(screen, 0);
    if (flags & 1) operator_delete(screen);
}
/* 02672498 */
void hdInformationDRC_c::inAllEnter() { hdAnimSet_play(animSet(), 0, 0, sInformationAnimationStart); }
/* 026724B4 */
void hdInformationDRC_c::inAllExecute() {
    hdInputManager* manager = hdInputGetManager();
    hdPointerController* pointer = manager->pointer();
    if (hdAnimSet_isEnd(animSet(), 0) || pointer->mHold) changeState(&sInformationOutAll);
    vCalcLayout();
}
/* 02672538 */
void hdInformationDRC_c::outAllEnter() { hdAnimSet_play(animSet(), 1, 0, sInformationAnimationStart); }
/* 02672554 */
void hdInformationDRC_c::outAllExecute() {
    if (hdAnimSet_isEnd(animSet(), 1)) changeState(&sInformationDisableWait);
    vCalcLayout();
}
/* 026725B8 */
static hdInformationDRC_c* constructInformation(hdInformationDRC_c* screen) {
    if (!screen) {
        screen = (hdInformationDRC_c*)operator_new(sizeof(hdInformationDRC_c));
        if (!screen) return nullptr;
    }
    informationConstructBase(screen);
    screen->mVtable.v = gabi::ea(&sInformationVtable);
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sInformationHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sInformationSecondaryVtable);
    return screen;
}
/* 02672624 */
u32 hdInformationDRC_c::setup(u32 archive, u32 resource, u32 flags) {
    informationAttachArchives(this, &sInformationArchiveName, 1);
    if (!vLoad(&sInformationName, mpResAccessor.v.get(), archive, resource, gabi::at<void>(flags))) return 0;
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sInformationDisableWait);
    return 1;
}
/* 02672734 */
void hdInformationDRC_c::release() { vDestroyLayout(); informationDetachArchives(this); }
/* 02672774 */
void hdInformationDRC_c::stateExecute() { informationExecuteState(stateMachine()); }
/* 0267277C */
u32 hdInformationDRC_c::loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor) {
    hdPartsLayout_c* layout = informationAllocateLayout(sizeof(hdPartsLayout_c), sInformationHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!layout->vLoad(name, archive, resource, flags, 2, 1, descriptor)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        u32 paneIndex = sInformationPaneIndices.mIndices[i];
        hdUiSafeString* pane = gabi::at<hdUiSafeString>(gabi::ea(&sInformationPaneName) + paneIndex * sizeof(hdUiSafeString));
        informationBindAnimation(animSet(), i, &sInformationAnimationNames.mNames[i], pane);
    }
    vCalcLayout();
    return 1;
}
/* 026728A8 */
static void initializeInformationStatics() {
    sinit_header_statics_rodata(gabi::ea(&sInformationHeaderAngles), gabi::ea(&sInformationHeaderRegistrations), gabi::ea(&sInformationHeaderZeros), gabi::ea(&sInformationHeaderPi));
    sInformationPaneName.mVtable.v = gabi::ea(&sInformationStringVtable);
    sInformationPaneName.mText.v = gabi::guest_addr("G_All_00");
    u32 ready = sInformationStateCounterReady;
    sInformationAnimationNames.mNames[0].mVtable.v = gabi::ea(&sInformationStringVtable);
    sInformationAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    sInformationAnimationNames.mNames[1].mVtable.v = gabi::ea(&sInformationStringVtable);
    sInformationAnimationNames.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sInformationName.mVtable.v = gabi::ea(&sInformationStringVtable);
    sInformationName.mText.v = gabi::guest_addr("InformationDRC_00");
    sInformationArchiveName.mVtable.v = gabi::ea(&sInformationStringVtable);
    sInformationArchiveName.mText.v = gabi::guest_addr("InformationDRC_00");
    u32 number;
    if (ready) number = sInformationStateCounter;
    else { number = 0; sInformationStateCounterReady = 1; }
    hdPartsStateIdVtbl_c* table = &sInformationStateVtable;
    sInformationInAll.setup(number + 1, "::StateID_InAll", table, 0x19, &sInformationNullState);
    sInformationOutAll.setup(number + 2, "::StateID_OutAll", table, 0x1C, &sInformationNullState);
    sInformationStateCounter = number + 2;
}
/* WWHD glue
 data sInformationVtable 100F67C8 nocheck
 data sInformationHolderVtable 100F68C0 nocheck
 data sInformationSecondaryVtable 100F68D0 nocheck
 data sInformationStringVtable 100F66BC nocheck
 data sInformationStateVtable 100F671C nocheck
 data sInformationHeaderAngles 10494DE8 nocheck
 data sInformationHeaderRegistrations 101F60AC nocheck
 data sInformationHeaderZeros 10494DF4 nocheck
 data sInformationHeaderPi 100F676C nocheck
data sInformationInAll 10494E14
data sInformationOutAll 10494E44
data sInformationDisableWait 1049DD60
data sInformationPaneName 10494DD8
data sInformationArchiveName 10494DE0
data sInformationName 10494E74
data sInformationAnimationNames 10494E04
data sInformationPaneIndices 100F66B0
data sInformationAnimationStart 100F6768
data sInformationStateCounterReady 101FD8E4
data sInformationStateCounter 101FDD50
data sInformationNullState 101FF32C
data sInformationHeapManager 1018C404
str 100F6774 "InAll" in 026728A8
str 100F677C "OutAll" in 026728A8
str 100F6784 "::StateID_InAll" in 026728A8
str 100F6794 "InformationDRC_00" in 026728A8
str 100F67A8 "G_All_00" in 026728A8
str 100F67B4 "::StateID_OutAll" in 026728A8
ext 026F88F0 void informationDestroyBase(hdInformationDRC_c* screen, u32 flags)
ext 026F89E8 hdInformationDRC_c* informationConstructBase(hdInformationDRC_c* screen)
ext 026F90D8 void informationAttachArchives(hdInformationDRC_c* screen, hdUiSafeString* names, u32 count)
ext 026F91F8 void informationDetachArchives(hdInformationDRC_c* screen)
ext 02006364 void informationExecuteState(hdStateMachine* machine)
ext 0273B050 hdPartsLayout_c* informationAllocateLayout(u32 size, void* heap, u32 alignment)
ext 02004E04 void informationBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
*/
