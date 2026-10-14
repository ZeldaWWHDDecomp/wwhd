/** HD GamePad battery display. No GameCube counterpart. */
#include "d/hd_screen_battery.h"
namespace gc_tu {
static u32 sBatteryVtable;
static u32 sBatteryHolderVtable;
static u32 sBatterySecondaryVtable;
static u32 sBatteryStringVtable;
static hdPartsStateIdVtbl_c sBatteryStateVtable;
static u32 sBatteryHeaderAngles;
static u32 sBatteryHeaderRegistrations;
static u32 sBatteryHeaderZeros;
static u32 sBatteryHeaderPi;
}
using namespace gc_tu;
static hdPartsStateId_c sBatteryWait;
static hdPartsStateId_c sBatteryDisplay;
static hdUiSafeString sBatteryScreenName;
static hdUiSafeString sBatteryAnimationName;
static hdUiSafeString sBatteryAnimationPane;
static u32 sBatteryAnimationPaneIndex = 0;
static f32 sBatteryAnimationStart = 1.0f;
struct hdBatteryHeapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
WWHD_OFFSET(hdBatteryHeapManager_l, mpHeap, 0x10);
struct hdBatteryHeapPointer_l { gptr<hdBatteryHeapManager_l> mpManager; };
static hdBatteryHeapPointer_l sBatteryHeapManager;
static u32 sBatteryStateCounterReady;
static u32 sBatteryStateCounter;
static hdStateId sBatteryNullState;


/* 026230A4 */
static u32 batteryStateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 026231A4 */
static void destroyBattery(hdBattery_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sBatteryHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sBatterySecondaryVtable);
    batteryDestroyBase(screen, 0);
    if (flags & 1) operator_delete(screen);
}

/* 02623210 */
void hdBattery_c::hide() { mDisplayRequested = 0; }

/* 0262321C */
void hdBattery_c::waitExecute() {
    if (mDisplayRequested) changeState(&sBatteryDisplay);
}

/* 02623238 */
void hdBattery_c::displayEnter() {
    batteryStartAnimation(animSet(), 0, 0, sBatteryAnimationStart);
}

/* 02623254 */
void hdBattery_c::displayExecute() {
    if (!mDisplayRequested) changeState(&sBatteryWait);
    vCalcLayout();
}

/* 026232AC */
static hdBattery_c* constructBattery(hdBattery_c* screen) {
    if (!screen) {
        screen = (hdBattery_c*)operator_new(sizeof(hdBattery_c));
        if (!screen) return nullptr;
    }
    hdPartsNoControl_construct(screen);
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sBatteryHolderVtable);
    screen->mVtable.v = gabi::ea(&sBatteryVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sBatterySecondaryVtable);
    screen->mDisplayRequested = 0;
    return screen;
}

/* 02623320 */
u32 hdBattery_c::setup(u32 archive, u32 resource, u32 flags) {
    if (!batterySetupBase(this, &sBatteryScreenName, archive, resource, flags)) return 0;
    startState(&sBatteryWait);
    return 1;
}

/* 026233B4 */
void hdBattery_c::drawIfRequested() {
    if (mDisplayRequested) batteryDrawBase(this);
}

/* 026233C4 */
void hdBattery_c::toWait() { changeState(&sBatteryWait); }

/* 026233D4 */
u32 hdBattery_c::loadScreen(void* archive, u32 resource, u32 flags, u32 mode, void* heap) {
    hdPartsLayout_c* layout = batteryAllocateLayout(sizeof(hdPartsLayout_c), sBatteryHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!layout->vLoad(archive, resource, flags, mode, 1, 1, heap)) return 0;
    u32 paneIndex = sBatteryAnimationPaneIndex;
    hdUiSafeString* pane = gabi::at<hdUiSafeString>(gabi::ea(&sBatteryAnimationPane) + paneIndex * sizeof(hdUiSafeString));
    batteryBindAnimation(animSet(), 0, &sBatteryAnimationName, pane);
    return 1;
}

/* HD: translation-unit state and resource initialization. */
/* 026234C0 */
static void initializeBatteryStatics() {
    sinit_header_statics_rodata(gabi::ea(&sBatteryHeaderAngles), gabi::ea(&sBatteryHeaderRegistrations), gabi::ea(&sBatteryHeaderZeros), gabi::ea(&sBatteryHeaderPi));
    sBatteryAnimationPane.mVtable.v = gabi::ea(&sBatteryStringVtable);
    sBatteryAnimationPane.mText.v = gabi::guest_addr("G_Battery_00");
    sBatteryAnimationName.mVtable.v = gabi::ea(&sBatteryStringVtable);
    sBatteryAnimationName.mText.v = gabi::guest_addr("Battery");
    u32 ready = sBatteryStateCounterReady;
    sBatteryScreenName.mVtable.v = gabi::ea(&sBatteryStringVtable);
    sBatteryScreenName.mText.v = gabi::guest_addr("Battery_00");
    u32 number;
    if (ready) number = sBatteryStateCounter;
    else { number = 0; sBatteryStateCounterReady = 1; }
    sBatteryWait.setup(number + 1, "::StateID_Wait", &sBatteryStateVtable, 0x19, &sBatteryNullState);
    sBatteryDisplay.setup(number + 2, "::StateID_Disp", &sBatteryStateVtable, 0x1C, &sBatteryNullState);
    sBatteryStateCounter = number + 2;
}

/* WWHD glue
 data sBatteryVtable 100E4778 nocheck
 data sBatteryHolderVtable 100E4870 nocheck
 data sBatterySecondaryVtable 100E4880 nocheck
 data sBatteryStringVtable 100E4680 nocheck
 data sBatteryStateVtable 100E46E0 nocheck
 data sBatteryHeaderAngles 1048EB54 nocheck
 data sBatteryHeaderRegistrations 101F5278 nocheck
 data sBatteryHeaderZeros 1048EB60 nocheck
 data sBatteryHeaderPi 100E472C nocheck
data sBatteryStateCounterReady 101FD8E4
data sBatteryStateCounter 101FDD50
data sBatteryNullState 101FF32C
str 100E473C "G_Battery_00" in 026234C0
str 100E4734 "Battery" in 026234C0
str 100E474C "Battery_00" in 026234C0
str 100E4758 "::StateID_Wait" in 026234C0
str 100E4768 "::StateID_Disp" in 026234C0
data sBatteryWait 1048EB70
data sBatteryDisplay 1048EBA0
data sBatteryScreenName 1048EBD0
data sBatteryAnimationName 1048EB4C
data sBatteryAnimationPane 1048EB44
data sBatteryAnimationPaneIndex 100E467C
data sBatteryAnimationStart 100E4728
data sBatteryHeapManager 1018C404
ext 02700A70 void batteryDestroyBase(hdBattery_c* screen, u32 flags)
ext 02700B48 u32 batterySetupBase(hdBattery_c* screen, hdUiSafeString* name, u32 archive, u32 resource, u32 flags)
ext 026F8B54 void batteryDrawBase(hdBattery_c* screen)
ext 020053E4 void batteryStartAnimation(hdAnimSet* animation, u32 track, u32 flags, f32 frame)
ext 0273B050 hdPartsLayout_c* batteryAllocateLayout(u32 size, void* heap, u32 alignment)
ext 02004E04 void batteryBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
*/
