/** HD wind-control button animation states. */
#include "d/hd_screen_btn_control_wind.h"
namespace gc_tu {
static u32 sWindButtonVtable;
static u32 sWindButtonHolderVtable;
static u32 sWindButtonSecondaryVtable;
static u32 sWindButtonStringVtable;
static hdPartsStateIdVtbl_c sWindButtonStateVtable;
static u32 sWindButtonHeaderAngles;
static u32 sWindButtonHeaderRegistrations;
static u32 sWindButtonHeaderZeros;
static u32 sWindButtonHeaderPi;
}
using namespace gc_tu;
static f32 sWindButtonAnimationStart = 1.0f;
static hdPartsStateId_c sWindButtonOnTouch;
static hdPartsStateId_c sWindButtonSelectTouch;
static hdPartsStateId_c sWindButtonSelectCursor;
static hdPartsStateId_c sWindButtonDecide;
static hdPartsStateId_c sWindButtonUnSelect;
static hdUiSafeString sWindButtonName;
static hdUiSafeString sWindButtonPaneName;
static hdUiSafeString sWindButtonAllPaneName;
static hdPartsStateId_c sWindButtonOff;
static hdPartsStateId_c sWindButtonParentOff;
static hdPartsStateId_c sWindButtonParentOnTouch;
static hdPartsStateId_c sWindButtonParentSelectTouch;
static hdPartsStateId_c sWindButtonParentSelectCursor;
static u32 sWindButtonStateCounterReady;
static u32 sWindButtonStateCounter;

struct hdWindButtonManagerPointer_l { gptr<hdButtonEventManager_l> mpManager; };
static hdWindButtonManagerPointer_l sWindButtonEventManager;
/* inlined into button event states */
static hdWindButtonVtbl_l* windButtonTable(hdBtnControlWind_c* button) {
    return static_cast<hdWindButtonVtbl_l*>(button->mButton.mVtable.get());
}

/* 0262C55C */
static u32 windButtonStateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 0262C65C */
static void destroyWindButton(hdBtnControlWind_c* button, u32 flags) {
    if (!button) return;
    button->mButton.mStateHolder.mHolderVtable.v = gabi::ea(&sWindButtonHolderVtable);
    button->mButton.mSecondaryVtable.v = gabi::ea(&sWindButtonSecondaryVtable);
    windButtonDestroyBase(button, 0);
    if (flags & 1) operator_delete(button);
}

/* 0262C6C8 */
void hdBtnControlWind_c::offExecute() {
    hdButtonEventManager_l* manager = sWindButtonEventManager.mpManager;
    if (!manager) return;
    hdButtonEvent_l event;
    windButtonCopyEvent(&event, &manager->mCurrentEvent);
    u32 receiver = windButtonReceiverId(this);
    if (event.mReceiverId == receiver) {
        if (!gabi::call_ptr<u32>(windButtonTable(this)->mAcceptTouch.get(), this, u32(event.mType))) return;
        if (windButtonInputEnabled(this) && mButton.mTouchEnabled) windButtonClearFocus(this);
        gabi::call_ptr<void>(windButtonTable(this)->mBeginTouch.get(), this);
        mButton.changeState(&sWindButtonSelectTouch);
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        u32 mode = windButtonGetMode(sWindButtonEventManager.mpManager);
        if (mode != gabi::ea(mButton.mpReceiver.get())) return;
        if (!gabi::call_ptr<u32>(windButtonTable(this)->mAcceptEvent.get(), this, &event)) return;
        gabi::call_ptr<void>(windButtonTable(this)->mPrepareDecide.get(), this);
        mButton.changeState(&sWindButtonSelectCursor);
    }
}

/* 0262C7F8 */
void hdBtnControlWind_c::onTouchExecute() {
    hdButtonEventManager_l* manager = sWindButtonEventManager.mpManager;
    if (!manager) return;
    hdButtonEvent_l event;
    windButtonCopyEvent(&event, &manager->mCurrentEvent);
    u32 receiver = windButtonReceiverId(this);
    if (event.mReceiverId == receiver) {
        if (!gabi::call_ptr<u32>(windButtonTable(this)->mAcceptTouchRelease.get(), this, u32(event.mType))) return;
        gabi::call_ptr<void>(windButtonTable(this)->mPrepareDecide.get(), this);
        mButton.changeState(&sWindButtonDecide);
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        u32 mode = windButtonGetMode(sWindButtonEventManager.mpManager);
        if (mode != gabi::ea(mButton.mpReceiver.get())) return;
        if (gabi::call_ptr<u32>(windButtonTable(this)->mAcceptEvent.get(), this, &event)) {
            gabi::call_ptr<void>(windButtonTable(this)->mPrepareDecide.get(), this);
            mButton.changeState(&sWindButtonDecide);
        } else if (windButtonIsDecideEvent(event.mType) &&
                   gabi::call_ptr<u32>(windButtonTable(this)->mAcceptCursor.get(), this, u32(event.mType))) {
            windButtonPendingRelease(this);
            mButton.changeState(&sWindButtonUnSelect);
        }
    } else if (!event.mReceiverId) mButton.changeState(&sWindButtonUnSelect);
}

/* 0262C9C0 */
void hdBtnControlWind_c::selectTouchExecute() {
    hdButtonEventManager_l* manager = sWindButtonEventManager.mpManager;
    if (!manager) return;
    if (!mSelectPhase) {
        hdButtonEvent_l event;
        windButtonCopyEvent(&event, &manager->mCurrentEvent);
        u32 receiver = windButtonReceiverId(this);
        if (event.mReceiverId == receiver) {
            if (gabi::call_ptr<u32>(windButtonTable(this)->mAcceptTouchRelease.get(), this, u32(event.mType))) {
                gabi::call_ptr<void>(windButtonTable(this)->mPrepareDecide.get(), this);
                mSelectPhase = 2;
            }
        } else if (event.mReceiverId == 0xFFFFFFFF) {
            u32 mode = windButtonGetMode(sWindButtonEventManager.mpManager);
            if (mode == gabi::ea(mButton.mpReceiver.get())) {
                if (gabi::call_ptr<u32>(windButtonTable(this)->mAcceptEvent.get(), this, &event)) {
                    gabi::call_ptr<void>(windButtonTable(this)->mPrepareDecide.get(), this);
                    mSelectPhase = 2;
                } else if (windButtonIsDecideEvent(event.mType) &&
                           gabi::call_ptr<u32>(windButtonTable(this)->mAcceptCursor.get(), this, u32(event.mType))) {
                    windButtonPendingRelease(this);
                    mSelectPhase = 3;
                }
            }
        } else if (!event.mReceiverId) mSelectPhase = 3;
    }
    if (mSelectPhase == 2) windButtonSynchronizeFocus(this);
    if (!hdAnimSet_isEnd(mButton.animSet(), 0)) return;
    if (mSelectPhase == 2) mButton.changeState(&sWindButtonDecide);
    else if (mSelectPhase == 3) mButton.changeState(&sWindButtonUnSelect);
    else mButton.changeState(&sWindButtonOnTouch);
}

/* 0262C974 */
void hdBtnControlWind_c::selectTouchEnter() {
    hdAnimSet_play(mButton.animSet(), 0, 0, sWindButtonAnimationStart);
    mSelectPhase = 0;
}

/* 0262CB9C */
void hdBtnControlWind_c::selectCursorEnter() {
    windButtonSynchronizeFocus(this);
    hdAnimSet_play(mButton.animSet(), 0, 0, sWindButtonAnimationStart);
}

/* 0262CBE4 */
void hdBtnControlWind_c::selectCursorExecute() {
    windButtonSynchronizeFocus(this);
    if (hdAnimSet_isEnd(mButton.animSet(), 0)) mButton.changeState(&sWindButtonDecide);
}

/* 0262CC38 */
static hdBtnControlWind_c* constructWindButton(hdBtnControlWind_c* button) {
    if (!button) {
        button = (hdBtnControlWind_c*)operator_new(sizeof(hdBtnControlWind_c));
        if (!button) return nullptr;
    }
    windButtonConstructBase(button);
    button->mButton.mVtable.v = gabi::ea(&sWindButtonVtable);
    button->mButton.mStateHolder.mHolderVtable.v = gabi::ea(&sWindButtonHolderVtable);
    button->mButton.mSecondaryVtable.v = gabi::ea(&sWindButtonSecondaryVtable);
    return button;
}

/* 0262CCA4 */
u32 hdBtnControlWind_c::setup(void* archive, void* resource, void* flags) {
    return windButtonSetupBase(this, &sWindButtonName, archive, resource, flags);
}

/* HD: translation-unit state and resource initialization. */
/* 0262CCBC */
static void initializeWindButtonStatics() {
    sinit_header_statics_rodata(gabi::ea(&sWindButtonHeaderAngles), gabi::ea(&sWindButtonHeaderRegistrations), gabi::ea(&sWindButtonHeaderZeros), gabi::ea(&sWindButtonHeaderPi));
    sWindButtonAllPaneName.mVtable.v = gabi::ea(&sWindButtonStringVtable);
    sWindButtonPaneName.mVtable.v = gabi::ea(&sWindButtonStringVtable);
    sWindButtonPaneName.mText.v = gabi::guest_addr("G_Btn_00");
    u32 ready = sWindButtonStateCounterReady;
    sWindButtonAllPaneName.mText.v = gabi::guest_addr("G_All_00");
    sWindButtonName.mVtable.v = gabi::ea(&sWindButtonStringVtable);
    sWindButtonName.mText.v = gabi::guest_addr("BtnControlWind_00");
    u32 number;
    if (ready) number = sWindButtonStateCounter;
    else { number = 0; sWindButtonStateCounterReady = 1; }
    hdPartsStateIdVtbl_c* table = &sWindButtonStateVtable;
    sWindButtonOff.setup(number + 1, "::StateID_Off", table, 0x26, (hdStateId*)&sWindButtonParentOff);
    sWindButtonOnTouch.setup(number + 2, "::StateID_OnTouch", table, 0x29, (hdStateId*)&sWindButtonParentOnTouch);
    sWindButtonSelectTouch.setup(number + 3, "::StateID_SelectTouch", table, 0x2F, (hdStateId*)&sWindButtonParentSelectTouch);
    sWindButtonSelectCursor.setup(number + 4, "::StateID_SelectCursor", table, 0x32, (hdStateId*)&sWindButtonParentSelectCursor);
    sWindButtonStateCounter = number + 4;
}

/* WWHD glue
 data sWindButtonVtable 100E6BB0 nocheck
 data sWindButtonHolderVtable 100E6E08 nocheck
 data sWindButtonSecondaryVtable 100E6E18 nocheck
 data sWindButtonStringVtable 100E6A74 nocheck
 data sWindButtonStateVtable 100E6AD4 nocheck
 data sWindButtonHeaderAngles 1048F6A4 nocheck
 data sWindButtonHeaderRegistrations 101F5458 nocheck
 data sWindButtonHeaderZeros 1048F6B0 nocheck
 data sWindButtonHeaderPi 100E6B24 nocheck
data sWindButtonOff 1048F6D0
data sWindButtonParentOff 1049D830
data sWindButtonParentOnTouch 1049D860
data sWindButtonParentSelectTouch 1049D8C0
data sWindButtonParentSelectCursor 1049D8F0
data sWindButtonPaneName 1048F6C0
data sWindButtonAllPaneName 1048F6C8
data sWindButtonStateCounterReady 101FD8E4
data sWindButtonStateCounter 101FDD50
str 100E6B2C "BtnControlWind_00" in 0262CCBC
str 100E6B40 "::StateID_Off" in 0262CCBC
str 100E6B50 "::StateID_OnTouch" in 0262CCBC
str 100E6B64 "::StateID_SelectTouch" in 0262CCBC
str 100E6B7C "G_Btn_00" in 0262CCBC
str 100E6B88 "G_All_00" in 0262CCBC
str 100E6B94 "::StateID_SelectCursor" in 0262CCBC
data sWindButtonEventManager 101F4FF0
ext 026F5B74 u32 windButtonReceiverId(hdBtnControlWind_c* button)
ext 026F66B0 void windButtonClearFocus(hdBtnControlWind_c* button)
ext 026F621C u32 windButtonIsDecideEvent(u32 type)
ext 020013C0 void windButtonCopyEvent(hdButtonEvent_l* out, hdButtonEvent_l* event)
ext 02615140 u32 windButtonGetMode(hdButtonEventManager_l* manager)
ext 026F67A4 u32 windButtonInputEnabled(hdBtnControlWind_c* button)
ext 026F69A4 void windButtonPendingRelease(hdBtnControlWind_c* button)
data sWindButtonAnimationStart 100E6B20
data sWindButtonOnTouch 1048F700
data sWindButtonSelectTouch 1048F730
data sWindButtonSelectCursor 1048F760
data sWindButtonDecide 1049D950
data sWindButtonUnSelect 1049D920
data sWindButtonName 1048F790
ext 026F0B8C void windButtonDestroyBase(hdBtnControlWind_c* button, u32 flags)
ext 026F1C3C hdBtnControlWind_c* windButtonConstructBase(hdBtnControlWind_c* button)
ext 026F1D04 u32 windButtonSetupBase(hdBtnControlWind_c* button, hdUiSafeString* name, void* archive, void* resource, void* flags)
ext 026F66F4 void windButtonSynchronizeFocus(hdBtnControlWind_c* button)
*/
