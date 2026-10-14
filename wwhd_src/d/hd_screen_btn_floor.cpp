/** HD BtnFloor_00: floor selection and cursor-event routing. Private game-derived source. */
#include "d/hd_screen_btn_floor.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdUiSafeString sFloorLayoutName;
static hdPartsStateId_c sFloorOffState;
static hdPartsStateId_c sFloorOnCursorState;
static hdPartsStateId_c sFloorSelectCursorState;
static hdPartsStateId_c sFloorBlankState;
static cking::ui::input::EventMgr* sFloorEvents;
static f32 sFloorAnimationRate;
static hdMapChartStaticSettings_l sFloorSettings;
static hdMapChartSettingConstants_l sFloorSettingConstants;
static u32 sFloorStateRegistryReady;
static u32 sFloorStateRegistryCounter;
static hdPartsStateIdVtbl_c sFloorStateTable;
static u32 sFloorSafeStringVtable;
static hdMapChartRegistration_l sFloorRegistration0;
static hdMapChartRegistration_l sFloorRegistration1;
static hdMapChartRegistration_l sFloorRegistration2;
static hdUiSafeString sFloorAnimationGroup;
static hdStateId sFloorOffParent;
static hdStateId sFloorOnCursorParent;
static hdStateId sFloorSelectCursorParent;
enum { FLOOR_NONE = 10, FLOOR_INITIAL = 0x7B, SELECT_ANIMATION = 0, EVENT_NO_RECEIVER = 0xFFFFFFFF, SELECT_COMPLETE_EVENT = 17 };
/* inlined into floor state handlers */
static hdButtonEvent_l* floorCurrentEvent(cking::ui::input::EventMgr* events) {
    return gabi::at<hdButtonEvent_l>(gabi::ea(events) + offsetof(hdButtonEventManager_l, mCurrentEvent));
}
/* inlined into updateSelection */
static bool floorStateMatches(hdButtonPart_c* button, hdPartsStateId_c* expected) {
    hdPartsStateId_c* current = floorButtonCurrentState(button->stateMachine());
    u32 currentKey = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedTable = expected->mVtable;
    u32 key = gabi::call_ptr<u32>(currentKey, current);
    return key == gabi::call_ptr<u32>(expectedTable->mGetKey.get(), expected);
}
/* 0262EC3C */
static u32 floorButtonStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 0262ED3C */
void hdFloorButton_c::destroy(hdFloorButton_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    self->noDecideButton()->destroy(0);
    if (flags & 1) operator_delete(self);
}
/* 0262EDA8 */
void hdFloorButton_c::offExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    cking::ui::input::EventMgr* events = sFloorEvents;
    if (!events) return;
    gabi::FrameLocal<hdButtonEvent_l> event(8);
    floorButtonCopyEvent(event, floorCurrentEvent(events));
    u32 id = getReceiverId();
    if (event->mReceiverId == id) {
        u32 target = vtbl()->mAcceptEvent.get();
        if (!gabi::call_ptr<u32>(target, this, u32(event->mType))) return;
        if (floorButtonInputEnabled() && mTouchEnabled) clearFocus();
        gabi::call_ptr(vtbl()->mCursorMove.get(), this);
    } else if (floorButtonInputEnabled() && mTouchEnabled) sFloorEvents->getMode();
}
/* 0262EE98 */
void hdFloorButton_c::onCursorExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sFloorEvents || !floorButtonInputEnabled() || !mTouchEnabled) return;
    gabi::FrameLocal<hdButtonEvent_l> event(8);
    floorButtonCopyEvent(event, floorCurrentEvent(sFloorEvents));
    u32 id = getReceiverId();
    if (event->mReceiverId == id) {
        u32 target = vtbl()->mAcceptEvent.get();
        if (gabi::call_ptr<u32>(target, this, u32(event->mType))) clearFocus();
    } else if (event->mReceiverId == EVENT_NO_RECEIVER) {
        u32 mode = sFloorEvents->getMode();
        if (mode == gabi::ea(mpReceiver.get()) && floorButtonCanDispatch(event->mType))
            gabi::call_ptr(vtbl()->mDispatch.get(), this, u32(event->mType));
    }
}
/* 0262EFA0 */
void hdFloorButton_c::selectCursorEnter() { hdAnimSet_play(animSet(), SELECT_ANIMATION, 0, sFloorAnimationRate); }
/* 0262EFBC */
void hdFloorButton_c::selectCursorExecute() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sFloorEvents) return;
    gabi::FrameLocal<hdButtonEvent_l> event(8);
    if (floorButtonInputEnabled() && mTouchEnabled) {
        floorButtonCopyEvent(event, floorCurrentEvent(sFloorEvents));
        if (event->mReceiverId == EVENT_NO_RECEIVER) {
            u32 mode = sFloorEvents->getMode();
            if (mode == gabi::ea(mpReceiver.get()) && floorButtonCanDispatch(event->mType))
                gabi::call_ptr(vtbl()->mDispatch.get(), this, u32(event->mType));
        }
    }
    if (!hdAnimSet_isEnd(animSet(), SELECT_ANIMATION)) return;
    u32 id = getReceiverId();
    floorButtonBuildEvent(event, SELECT_COMPLETE_EVENT, id, nullptr);
    void* queue = gabi::at<void>(gabi::ea(sFloorEvents.get()) + offsetof(cking::ui::input::EventMgr, mEventQueue));
    floorButtonQueueEvent(queue, event);
    changeState(&sFloorOnCursorState);
}
/* 0262F0C4 */
hdFloorButton_c* hdFloorButton_c::construct(hdFloorButton_c* self) {
    if (!self) self = static_cast<hdFloorButton_c*>(dungeonButtonAllocate(sizeof(hdFloorButton_c)));
    if (self) {
        NoDecideButton_l::construct(self->noDecideButton());
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
        self->mSelectedFloor = FLOOR_NONE;
        self->mFloor = FLOOR_INITIAL;
    }
    return self;
}
/* 0262F140 */
u32 hdFloorButton_c::load(void* context, void* resource, void* option) { return noDecideButton()->create(&sFloorLayoutName, context, gabi::ea(resource), gabi::ea(option)); }
/* 0262F158 */
u32 hdFloorButton_c::isSelected() { u8 floor = mFloor; if (floor == FLOOR_NONE) return 0; u8 selected = mSelectedFloor; return selected != FLOOR_NONE && floor == selected; }
/* 0262F188 */
void hdFloorButton_c::updateSelection() {
    u32 selected = isSelected();
    if (selected) {
        if (floorStateMatches(this, &sFloorSelectCursorState)) return;
        if (floorStateMatches(this, &sFloorOnCursorState)) return;
        if (!floorButtonInputEnabled() || !mTouchEnabled) clearFocus();
        changeState(&sFloorSelectCursorState);
    } else {
        if (floorStateMatches(this, &sFloorBlankState)) return;
        if (floorStateMatches(this, &sFloorOffState)) return;
        changeState(&sFloorBlankState);
    }
}
/* 0262F324 */
void hdFloorButton_c::update() { updateSelection(); noDecideButton()->update(); }
/* 0262F358 */
u32 hdFloorButton_c::acquireFocus(u32 event) {
    hdButtonReceiver_l* receiver = mpReceiver;
    if (!receiver || !floorButtonCanDispatch(event)) return 0;
    u32 mask = floorButtonEventMask(event);
    return floorButtonReceiverAccepts(receiver, mask);
}
/* 0262F3D4 */
u32 hdFloorButton_c::dispatch(u32 event) {
    u32 focus = gabi::call_ptr<u32>(vtbl()->mAcquireFocus.get(), this, event);
    if (!focus) return 0;
    sFloorEvents->setNextMode(focus);
    sFloorEvents->setParams(0, 8);
    return 1;
}
/* 0262F448 */
static void floorButtonInitialize() {
    sFloorSettings.mRegistrationWork[3] = 0;
    sFloorSettings.mRegistrationWork[2] = 0;
    sFloorSettings.mRegistrationWork[1] = 0;
    sFloorSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sFloorRegistration0);
    u32 minimumAngleBits = sFloorSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sFloorSettingConstants.mMaximumAngle.mBits;
    sFloorSettings.mMinimumAngle.mBits = minimumAngleBits;
    sFloorSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sFloorSettings.mSignedHeader);
    chartRegisterGlobal(&sFloorRegistration1);
    chartInitializeUnsignedHeader(&sFloorSettings.mUnsignedHeader);
    chartRegisterGlobal(&sFloorRegistration2);
    u32 nearDistanceBits = sFloorSettingConstants.mNearDistance.mBits;
    u32 registryReady = sFloorStateRegistryReady;
    sFloorSettings.mDistances[2].mBits = nearDistanceBits;
    u32 farDistanceBits = sFloorSettingConstants.mFarDistance.mBits;
    sFloorAnimationGroup.mVtable.v = gabi::ea(&sFloorSafeStringVtable);
    sFloorAnimationGroup.mText.v = gabi::guest_addr("G_Btn_00");
    sFloorSettings.mDistances[0].mBits = farDistanceBits;
    sFloorSettings.mDistances[1].mBits = farDistanceBits;
    sFloorLayoutName.mVtable.v = gabi::ea(&sFloorSafeStringVtable);
    sFloorLayoutName.mText.v = gabi::guest_addr("BtnFloor_00");
    sFloorSettings.mDistances[3].mBits = nearDistanceBits;
    u32 number;
    if (registryReady) number = sFloorStateRegistryCounter;
    else { sFloorStateRegistryReady = 1; number = 0; }
    enum { OFF_ENTER_DELEGATE = 0x23, ON_CURSOR_ENTER_DELEGATE = 0x29, SELECT_CURSOR_ENTER_DELEGATE = 0x2F };
    ++number;
    sFloorOffState.setup(s32(number), "::StateID_Off", &sFloorStateTable, OFF_ENTER_DELEGATE, &sFloorOffParent);
    ++number;
    sFloorOnCursorState.setup(s32(number), "::StateID_OnCursor", &sFloorStateTable, ON_CURSOR_ENTER_DELEGATE, &sFloorOnCursorParent);
    ++number;
    sFloorSelectCursorState.setup(s32(number), "::StateID_SelectCursor", &sFloorStateTable, SELECT_CURSOR_ENTER_DELEGATE, &sFloorSelectCursorParent);
    sFloorStateRegistryCounter = number;
}
/* WWHD glue
 data sScreenVtable 100E7780 nocheck
 data sHolderVtable 100E7980 nocheck
 data sSecondaryVtable 100E7990 nocheck
 data sFloorSafeStringVtable 100E766C nocheck
data sFloorSettings 1048FA04
data sFloorSettingConstants 100E771C
data sFloorStateRegistryReady 101FD8E4
data sFloorStateRegistryCounter 101FDD50
data sFloorStateTable 100E76CC
data sFloorRegistration0 101F54C4
data sFloorRegistration1 101F54D0
data sFloorRegistration2 101F54DC
data sFloorAnimationGroup 1048F9FC
data sFloorOffParent 1049E53C
data sFloorOnCursorParent 1049E59C
data sFloorSelectCursorParent 1049E5FC
str 100E7748 "G_Btn_00" in 0262F448
str 100E772C "BtnFloor_00" in 0262F448
str 100E7738 "::StateID_Off" in 0262F448
str 100E7754 "::StateID_OnCursor" in 0262F448
str 100E7768 "::StateID_SelectCursor" in 0262F448
ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
ext 026F66B0 void hdButtonPart_c::clearFocus()
ext 02615140 u32 cking::ui::input::EventMgr::getMode()
ext 02615180 void cking::ui::input::EventMgr::setNextMode(u32 mode)
ext 02615148 void cking::ui::input::EventMgr::setParams(u32 a, u32 b)
ext 027019B8 void NoDecideButton_l::update()
ext 026F67A4 u32 floorButtonInputEnabled()
ext 026F621C u32 floorButtonCanDispatch(u32 event)
ext 026F63E0 u32 floorButtonEventMask(u32 event)
ext 026F6478 u32 floorButtonReceiverAccepts(hdButtonReceiver_l* receiver, u32 mask)
ext 02006478 hdPartsStateId_c* floorButtonCurrentState(hdStateMachine* machine)
ext 020013C0 void floorButtonCopyEvent(hdButtonEvent_l* dst, hdButtonEvent_l* src)
ext 02001338 void floorButtonBuildEvent(hdButtonEvent_l* event, u32 type, u32 receiver, void* payload)
ext 02001FF4 void floorButtonQueueEvent(void* queue, hdButtonEvent_l* event)
data sFloorLayoutName 1048FAC0
data sFloorOffState 1048FA30
data sFloorOnCursorState 1048FA60
data sFloorSelectCursorState 1048FA90
data sFloorBlankState 1049E62C
data sFloorEvents 101F4FF0
data sFloorAnimationRate 100E7718
*/
