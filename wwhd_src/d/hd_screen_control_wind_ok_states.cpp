/** HD wind confirmation, local companion; */
#include "d/hd_screen_control_wind_ok_states.h"
#include "d/hd_screen_btn_control_wind.h"
namespace gc_tu {
static u32 sWindOKStringVtable;
static hdPartsStateIdVtbl_c sWindOKStateVtable;
static u32 sWindOKVtable;
static u32 sWindOKHolderVtable;
static u32 sWindOKSecondaryVtable;
}
using namespace gc_tu;
struct hdWindOKManagerPointer_l { gptr<hdButtonEventManager_l> mpManager; };
static hdWindOKManagerPointer_l sWindOKEventManager;
static hdPartsStateId_c sWindOKDisp;
static hdPartsStateId_c sWindOKOutWait;
static hdPartsStateId_c sWindOKOutAll;
static hdPartsStateId_c sWindOKDisabled;
struct hdWindOKArchiveNames_l { hdUiSafeString mNames[2]; };
static hdWindOKArchiveNames_l sWindOKArchiveNames;
static hdUiSafeString sWindOKName;

static hdPartsStateId_c sWindOKWait;
static hdStateId sWindOKNullState;
static hdUiSafeString sWindOKPaneName;
static hdUiSafeString sWindOKButtonClass;
struct hdWindOKHeapManager_l { u8 _00[0x10]; gptr<void> mpHeap; };
WWHD_OFFSET(hdWindOKHeapManager_l, mpHeap, 0x10);
struct hdWindOKHeapPointer_l { gptr<hdWindOKHeapManager_l> mpManager; };
static hdWindOKHeapPointer_l sWindOKHeapManager;
struct hdWindOKResourceDescriptor_l { u8 _00[8]; hdUiSafeString mClass; u8 _10[4]; hdUiSafeString mName; };
WWHD_OFFSET(hdWindOKResourceDescriptor_l, mClass, 8);
WWHD_OFFSET(hdWindOKResourceDescriptor_l, mName, 0x14);
static u32 sWindOKStateCounterReady;
static u32 sWindOKStateCounter;
struct hdWindOKHeaderAngles_l { be<f32> mMin, mMax; };
struct hdWindOKHeaderFlags_l { be<u8> mFirst, mSecond; };
struct hdWindOKHeaderZeros_l { be<u32> mWords[4]; };
struct hdWindOKRegistration_l { be<u32> mWords[3]; };
struct hdWindOKRegistrations_l { hdWindOKRegistration_l mEntries[3]; };
static hdWindOKHeaderAngles_l sWindOKHeaderAngles;
static hdWindOKHeaderFlags_l sWindOKHeaderFlags;
static hdWindOKHeaderZeros_l sWindOKHeaderZeros;
static hdWindOKRegistrations_l sWindOKRegistrations;
static f32 sWindOKNegativePi = -3.1415927410125732f;
static f32 sWindOKPositivePi = 3.1415927410125732f;
void windOKRegister(hdWindOKRegistration_l* registration);
void windOKConstructHeaderFirst(be<u8>* flag);
void windOKConstructHeaderSecond(be<u8>* flag);
static bool windOKNamesMatch(hdUiSafeString* input, hdUiSafeString* reference) {
    input->assureTermination();
    input->assureTermination();
    be<u8>* cached = input->mText;
    reference->assureTermination();
    be<u8>* other = reference->mText;
    if (cached == other) return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 left = input->mText.get()[i];
        u8 right = reference->mText.get()[i];
        if (left != right) return false;
        if (!left) return true;
    }
    return false;
}

/* 0264EBC8 */
static u32 windOKStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0264ECC8 */
static void destroyWindOK(hdControlWindOK_c* panel, u32 flags) {
    if (!panel) return;
    panel->mStateHolder.mHolderVtable.v = gabi::ea(&sWindOKHolderVtable);
    panel->mSecondaryVtable.v = gabi::ea(&sWindOKSecondaryVtable);
    windOKDestroyBase(panel, 0);
    if (flags & 1) operator_delete(panel);
}
/* 0264ED34 */
void hdControlWindOK_c::resetInput() {
    hdParts_c* button = *childSlot(0);
    windOKResetButton(button);
    windOKClearButton(button);
    windOKSelectReceiver(sWindOKEventManager.mpManager, nullptr);
    mHighlight = 0;
    mTouchTriggered = 0;
    mForced = 0;
    mReady = 0;
    mCancelTriggered = 0;
    mTouchEnabled = 0;
}
/* 0264EDB0 */
void hdControlWindOK_c::resetEnter() { windOKResetInput(this); }
/* 0264EDB4 */
u32 hdControlWindOK_c::testTouch() {
    hdButtonEvent_l event;
    windOKCopyEvent(&event, &sWindOKEventManager.mpManager->mCurrentEvent);
    u8 enabled = mTouchEnabled;
    mTouchTriggered = 0;
    if (enabled && event.mType == 4) mTouchTriggered = 1;
    u32 touched = windOKTouchEvent(&event);
    enabled = mTouchEnabled;
    if (touched) mTouchTriggered = 1;
    if (enabled && event.mType == 2) windOKPlaySound("SE_DRC_TOUCH_ON");
    return mTouchTriggered;
}
/* 0264EE58 */
u32 hdControlWindOK_c::testCancel() {
    hdButtonEvent_l event;
    windOKCopyEvent(&event, &sWindOKEventManager.mpManager->mCurrentEvent);
    mCancelTriggered = 0;
    if (windOKCancelEvent(&event)) { mCancelTriggered = 1; return 1; }
    return mCancelTriggered;
}
/* 0264EEBC */
void hdControlWindOK_c::waitExecute() {
    if (windOKTestTouch(this)) changeState(&sWindOKOutWait);
    else if (mTouchEnabled) changeState(&sWindOKDisp);
    else if (windOKTestCancel(this)) {
        windOKGameState()->mCancel = 0;
        changeState(&sWindOKOutAll);
    }
}
/* 0264EF50 */
void hdControlWindOK_c::displayEnter() {
    hdButtonPart_c* button = (hdButtonPart_c*)childSlot(0)->get();
    windOKHighlightButton(button);
    mHighlight = 1;
    windOKSelectReceiver(sWindOKEventManager.mpManager, button->mpReceiver);
}
/* 0264EFB0 */
void hdControlWindOK_c::updateButton() {
    if (!windOKInputBusy(hdInputGetManager())) {
        hdParts_c* button = *childSlot(0);
        if (button) {
            hdButtonPartVtable_l* table = (hdButtonPartVtable_l*)button->mVtable.get();
            gabi::call_ptr(table->mOnFocusChanged.get(), button);
        }
    }
}
/* 0264F014 */
void hdControlWindOK_c::displayExecute() {
    if (windOKTestTouch(this)) changeState(&sWindOKOutWait);
    else if (windOKTestCancel(this)) {
        windOKGameState()->mCancel = 0;
        changeState(&sWindOKOutAll);
    }
    windOKUpdateButton(this);
}
/* 0264F090 */
void hdControlWindOK_c::outWaitEnter() { mWaitTimer = 10; }
/* 0264F12C */
void hdControlWindOK_c::outAllEnter() {
    if (mHighlight) windOKFinishButton(*childSlot(0));
    windOKSelectReceiver(sWindOKEventManager.mpManager, nullptr);
}
/* 0264F178 */
void hdControlWindOK_c::hide() { changeState(&sWindOKDisabled); }
/* 0264F188 */
static hdControlWindOK_c* constructWindOK(hdControlWindOK_c* panel) {
    if (!panel) { panel = (hdControlWindOK_c*)operator_new(sizeof(hdControlWindOK_c)); if (!panel) return nullptr; }
    windOKConstructBase(panel);
    panel->mStateHolder.mHolderVtable.v = gabi::ea(&sWindOKHolderVtable);
    panel->mVtable.v = gabi::ea(&sWindOKVtable);
    panel->mSecondaryVtable.v = gabi::ea(&sWindOKSecondaryVtable);
    panel->mTouchTriggered = 0;
    panel->mTouchEnabled = 0;
    panel->mWaitTimer = 0;
    panel->mHighlight = 0;
    panel->mForced = 0;
    panel->mReady = 0;
    panel->mCancelTriggered = 0;
    return panel;
}
/* 0264F214 */
u32 hdControlWindOK_c::setup(u32 archive, u32 resource, u32 flags) {
    windOKAttachArchives(this, sWindOKArchiveNames.mNames, 2);
    if (!vLoad(&sWindOKName, mpResAccessor.v.get(), archive, resource, gabi::at<void>(flags))) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    windOKResetInput(this);
    startState(&sWindOKDisabled);
    vExecuteChildren();
    vCalcLayout();
    return 1;
}
/* 0264F348 */
void hdControlWindOK_c::release() { vDeleteChildren(); vDestroyLayout(); windOKDetachArchives(this); }
/* 0264F39C */
void hdControlWindOK_c::stateExecute() { windOKExecuteState(stateMachine()); vExecuteChildren(); vCalcLayout(); }
/* 0264F3F4 */
void hdControlWindOK_c::enableTouch() { mTouchEnabled = 1; }
/* 0264F400 */
u32 hdControlWindOK_c::createChild(hdPartsDesc_c* descriptor) {
    void* heap = sWindOKHeapManager.mpManager->mpHeap;
    hdWindOKResourceDescriptor_l* request = (hdWindOKResourceDescriptor_l*)descriptor;
    if (!windOKNamesMatch(&request->mName, &sWindOKPaneName)) return 1;
    if (!windOKNamesMatch(&request->mClass, &sWindOKButtonClass)) return windOKCreateBase(this, 0, descriptor);
    gptr<hdParts_c>* slot = childSlot(0);
    hdBtnControlWind_c* button = (hdBtnControlWind_c*)windOKAllocate(0x8C, heap, 4);
    if (button) button = windOKConstructButton(button);
    *slot = button ? &button->mButton : nullptr;
    if (!*childSlot(0)) return 0;
    return windOKSetupButton((hdBtnControlWind_c*)childSlot(0)->get(), nullptr, nullptr, descriptor);
}
/* 0264F608 */
static void initializeWindOKStatics() {
    for (u32 i = 0; i < 4; ++i) sWindOKHeaderZeros.mWords[i] = 0;
    windOKRegister(&sWindOKRegistrations.mEntries[0]);
    sWindOKHeaderAngles.mMin = sWindOKNegativePi;
    sWindOKHeaderAngles.mMax = sWindOKPositivePi;
    windOKConstructHeaderFirst(&sWindOKHeaderFlags.mFirst);
    windOKRegister(&sWindOKRegistrations.mEntries[1]);
    windOKConstructHeaderSecond(&sWindOKHeaderFlags.mSecond);
    windOKRegister(&sWindOKRegistrations.mEntries[2]);
    u32 ready = sWindOKStateCounterReady;
    sWindOKName.mVtable.v = gabi::ea(&sWindOKStringVtable);
    sWindOKPaneName.mVtable.v = gabi::ea(&sWindOKStringVtable);
    sWindOKPaneName.mText.v = gabi::guest_addr("L_Btn_00");
    sWindOKArchiveNames.mNames[0].mVtable.v = gabi::ea(&sWindOKStringVtable);
    sWindOKArchiveNames.mNames[0].mText.v = gabi::guest_addr("ControlWindOK_00");
    sWindOKArchiveNames.mNames[1].mVtable.v = gabi::ea(&sWindOKStringVtable);
    sWindOKName.mText.v = gabi::guest_addr("ControlWindOK_00");
    sWindOKArchiveNames.mNames[1].mText.v = gabi::guest_addr("BtnControlWind_00");
    u32 number;
    if (ready) number = sWindOKStateCounter;
    else { number = 0; sWindOKStateCounterReady = 1; }
    hdPartsStateIdVtbl_c* table = &sWindOKStateVtable;
    sWindOKWait.setup(number + 1, "::StateID_Wait", table, 0x19, &sWindOKNullState);
    sWindOKDisp.setup(number + 2, "::StateID_Disp", table, 0x1C, &sWindOKNullState);
    sWindOKOutWait.setup(number + 3, "::StateID_OutWait", table, 0x1F, &sWindOKNullState);
    sWindOKOutAll.setup(number + 4, "::StateID_OutAll", table, 0x22, &sWindOKNullState);
    sWindOKStateCounter = number + 4;
}
/* WWHD glue
 data sWindOKStringVtable 100EEED4 nocheck
 data sWindOKStateVtable 100EEF34 nocheck
 data sWindOKVtable 100EF018 nocheck
 data sWindOKHolderVtable 100EF140 nocheck
 data sWindOKSecondaryVtable 100EF150 nocheck
data sWindOKHeapManager 1018C404
data sWindOKPaneName 10491B3C
data sWindOKButtonClass 1048F790
data sWindOKWait 10491B70
data sWindOKNullState 101FF32C
data sWindOKHeaderAngles 10491B44
data sWindOKHeaderFlags 10491B4C
data sWindOKHeaderZeros 10491B50
data sWindOKRegistrations 101F5918
data sWindOKNegativePi 100EEF94
data sWindOKPositivePi 100EEF98
data sWindOKStateCounterReady 101FD8E4
data sWindOKStateCounter 101FDD50
ext 028F026C void windOKRegister(hdWindOKRegistration_l* registration)
ext 028ED6F8 void windOKConstructHeaderFirst(be<u8>* flag)
ext 028EAB2C void windOKConstructHeaderSecond(be<u8>* flag)
ext 0273B050 void* windOKAllocate(u32 size, void* heap, u32 alignment)
ext 0262CC38 hdBtnControlWind_c* windOKConstructButton(hdBtnControlWind_c* button)
ext 0262CCA4 u32 windOKSetupButton(hdBtnControlWind_c* button, void* archive, void* resource, void* descriptor)
ext 026F92C0 u32 windOKCreateBase(hdControlWindOK_c* panel, u32 index, hdPartsDesc_c* descriptor)
str 100EEFC4 "L_Btn_00" in 0264F608
str 100EEFD0 "ControlWindOK_00" in 0264F608
str 100EEF9C "BtnControlWind_00" in 0264F608
str 100EEFE4 "::StateID_Wait" in 0264F608
str 100EEFF4 "::StateID_Disp" in 0264F608
str 100EEFB0 "::StateID_OutWait" in 0264F608
str 100EF004 "::StateID_OutAll" in 0264F608
data sWindOKEventManager 101F4FF0
data sWindOKDisp 10491BA0
data sWindOKOutWait 10491BD0
data sWindOKOutAll 10491C00
data sWindOKDisabled 1049DD60
data sWindOKArchiveNames 10491B60
data sWindOKName 10491C30
str 100EEF84 "SE_DRC_TOUCH_ON" in 0264EDB4
ext 026F88F0 void windOKDestroyBase(hdControlWindOK_c* panel, u32 flags)
ext 026F89E8 hdControlWindOK_c* windOKConstructBase(hdControlWindOK_c* panel)
ext 026F226C void windOKResetButton(hdParts_c* button)
ext 026F2400 void windOKClearButton(hdParts_c* button)
ext 026F237C void windOKHighlightButton(hdParts_c* button)
ext 026F23F0 void windOKFinishButton(hdParts_c* button)
ext 02615154 void windOKSelectReceiver(hdButtonEventManager_l* manager, void* receiver)
ext 020013C0 void windOKCopyEvent(hdButtonEvent_l* destination, hdButtonEvent_l* source)
ext 026F6288 u32 windOKTouchEvent(hdButtonEvent_l* event)
ext 026F6340 u32 windOKCancelEvent(hdButtonEvent_l* event)
ext 02030B38 void windOKPlaySound(const char* name)
ext 02617AE4 u32 windOKInputBusy(hdInputManager* manager)
ext 0264EDB4 u32 windOKTestTouch(hdControlWindOK_c* panel)
ext 0264EE58 u32 windOKTestCancel(hdControlWindOK_c* panel)
ext 0264EFB0 void windOKUpdateButton(hdControlWindOK_c* panel)
ext 0264ED34 void windOKResetInput(hdControlWindOK_c* panel)
ext 026F90D8 void windOKAttachArchives(hdControlWindOK_c* panel, hdUiSafeString* names, u32 count)
ext 026F91F8 void windOKDetachArchives(hdControlWindOK_c* panel)
ext 02006364 void windOKExecuteState(hdStateMachine* machine)
ext 025200D4 hdWindOKGameState_l* windOKGameState()
*/
