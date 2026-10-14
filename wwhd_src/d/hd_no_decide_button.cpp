// Message button state transitions and pane setup.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_no_decide_button.h"
using namespace gabi;
static hdPartsStateId_c sNoDecideNone;
static hdPartsStateId_c sButtonCursorOn;
static hdPartsStateId_c sButtonCursorOff;
static hdPartsStateId_c sButtonDefinitionOff;
static hdPartsStateId_c sButtonDefinitionOffAnimation;
static hdPartsStateId_c sButtonDefinitionOnCursor;
static hdPartsStateId_c sButtonDefinitionOnTouch;
static hdPartsStateId_c sButtonDefinitionTouchHeld;
static hdPartsStateId_c sButtonDefinitionReady;
static hdPartsStateId_c sButtonDefinitionDisabled;
static hdPartsStateId_c sButtonDefinitionHide;
static hdPartsStateId_c sButtonDefinitionShow;

using PartStatic0_l = be<u32>;
using PartStatic1_l = gptr<void>;
using PartStatic3_l = gptr<hdScreenUiSystem>;
using PartStatic4_l = gptr<hdButtonEventManager_l>;

extern void* noDecideAllocate(u32);
extern void noDecideDelete(void*);
extern void noDecideConstructBase(NoDecideButton_l*);
extern void noDecideDestroyBase(NoDecideButton_l*, u32);
extern void noDecideStartAnimation(void*, u32, u32, f32);
extern u32 noDecideAnimationDone(void*, u32);
extern void noDecideSetAnimationStart(void*, u32, u32);
extern void noDecideSetAnimationEnd(void*, u32, u32);
extern void noDecideSetEnabled(NoDecideButton_l*, u32);
extern void noDecideChange(hdStateMachine*, hdPartsStateId_c*);
extern void noDecideResetButtons(NoDecideButton_l*);
extern void noDecideUpdateBase(NoDecideButton_l*);
extern void noDecideUpdateState(hdStateMachine*);
extern void* noDecideAllocateHeap(u32, void*, u32);
extern hdPartsLayout_c* noDecideConstructLayout(hdPartsLayout_c*);
static PartStatic3_l sNoDecideHeap;
static u8 sNoDecideVTable;
static u8 sNoDecideReceiver;
static u8 sNoDecideStateVTable;

/* 02700C9C */
void NoDecideButton_l::destroy(u32 flags) {
    if (this) {
        reinterpret_cast<hdParts_c*>(this)->mStateHolder.mHolderVtable = at<void>(ea(&sNoDecideReceiver));
        reinterpret_cast<hdParts_c*>(this)->mSecondaryVtable = at<void>(ea(&sNoDecideStateVTable));
        noDecideDestroyBase(this, 0);
        if (flags & 1) noDecideDelete(this);
    }
}

/* 02701718 */
NoDecideButton_l* NoDecideButton_l::construct(NoDecideButton_l* part) {
    if (!part) part = static_cast<NoDecideButton_l*>(noDecideAllocate(0x78));
    if (part) {
        noDecideConstructBase(part);
        reinterpret_cast<hdParts_c*>(part)->mStateHolder.mHolderVtable = at<void>(ea(&sNoDecideReceiver));
        part->mVtable = at<hdPartsVtbl_c>(ea(&sNoDecideVTable));
        reinterpret_cast<hdParts_c*>(part)->mSecondaryVtable = at<void>(ea(&sNoDecideStateVTable));
        part->mAnimationState = 0;
    }
    return part;
}

/* 02701080 */
void NoDecideButton_l::enterOff() {
    noDecideStartAnimation(mpLayout.get()->animSet(), 0, 0, 1.0f);
    mAnimationState = 0;
}

/* 0270149C */
void NoDecideButton_l::enterOnTouch() {
    noDecideStartAnimation(mpLayout.get()->animSet(), 1, 0, 1.0f);
}

/* 02701640 */
void NoDecideButton_l::enterDisabled() {
    noDecideStartAnimation(mpLayout.get()->animSet(), 3, 0, 1.0f);
}

/* 027016AC */
void NoDecideButton_l::enterHide() {
    noDecideStartAnimation(mpLayout.get()->animSet(), 4, 0, 1.0f);
}

/* 027014B8 */
void NoDecideButton_l::updateOnTouch() {
    if (noDecideAnimationDone(mpLayout.get()->animSet(), 1))
        noDecideChange(stateMachine(), &sButtonDefinitionOff);
}

/* 0270165C */
void NoDecideButton_l::updateDisabled() {
    if (noDecideAnimationDone(mpLayout.get()->animSet(), 3))
        noDecideChange(stateMachine(), &sButtonDefinitionOff);
}

/* 027016C8 */
void NoDecideButton_l::updateHide() {
    if (noDecideAnimationDone(mpLayout.get()->animSet(), 4))
        noDecideChange(stateMachine(), &sNoDecideNone);
}

/* 02701984 */
void NoDecideButton_l::reset() {
    noDecideResetButtons(this);
    noDecideUpdateBase(this);
}

/* 027019B8 */
void NoDecideButton_l::update() {
    call_ptr<void>(messagePartVTable(this)[0x1FC / 4].get(), this);
    noDecideUpdateState(stateMachine());
    call_ptr<void>(messagePartVTable(this)[0x104 / 4].get(), this);
    call_ptr<void>(messagePartVTable(this)[0xB4 / 4].get(), this);
}

/* 02701B38 */
void NoDecideButton_l::setOnTouch() {
    noDecideSetAnimationStart(mpLayout.get()->animSet(), 1, 0);
    noDecideSetEnabled(this, 1);
    noDecideChange(stateMachine(), &sButtonDefinitionOff);
}

/* 02701B90 */
void NoDecideButton_l::setOnCursor(u32 immediate) {
    void* animator = mpLayout.get()->animSet();
    if (immediate) {
        noDecideSetAnimationEnd(animator, 2, 0);
        noDecideSetEnabled(this, 1);
        noDecideChange(stateMachine(), &sButtonDefinitionOff);
    } else {
        noDecideSetAnimationStart(animator, 2, 0);
        noDecideSetEnabled(this, 1);
        noDecideChange(stateMachine(), &sButtonDefinitionDisabled);
    }
}

/* 02701C14 */
void NoDecideButton_l::hide() {
    noDecideChange(stateMachine(), &sButtonDefinitionHide);
}

/* 02701C24 */
u32 NoDecideButton_l::createLayout(void* archive, void* resource, u32 mode, u32 flags, u32 user) {
    void* heap = sNoDecideHeap.get()->mHeap.get();
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(noDecideAllocateHeap(0x110, heap, 4));
    if (layout) layout = noDecideConstructLayout(layout);
    mpLayout = layout;
    if (!layout) return 0;
    call_ptr<void>(layout->vtbl()->mLoad.get(), layout, archive, resource, mode, flags, 5u, 1u, user);
    return 1;
}


static PartStatic4_l sNoDecideEvents;
extern void buttonReadEvent(hdButtonEvent_l*, hdButtonEvent_l*);
extern u32 buttonIdentifier(NoDecideButton_l*);
extern u32 buttonCanControl(NoDecideButton_l*);
extern u32 buttonControlMode(hdButtonEventManager_l*);
extern u32 buttonEventIsTouch(u32);
extern void buttonOnTouch(NoDecideButton_l*);
extern void buttonLeaveCursor(NoDecideButton_l*);
extern void buttonEnterCursor(NoDecideButton_l*);
extern void buttonMakeEvent(hdButtonEvent_l*, u32, u32, u32);
extern void buttonQueueEvent(void*, hdButtonEvent_l*);

/* 02700D08 */
void NoDecideButton_l::updateOffInput() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdButtonEventManager_l* events = sNoDecideEvents.get();
    if (!events) return;
    FrameLocal<hdButtonEvent_l> event(8);
    buttonReadEvent(event.get(), &events->mCurrentEvent);
    u32 id = buttonIdentifier(this);
    if (u32(event->mReceiverId) == id) {
        if (!call_ptr<u32>(messagePartVTable(this)[0xE4 / 4].get(), this, u32(event->mType))) return;
        if (buttonCanControl(this) && mEnabled) buttonOnTouch(this);
        call_ptr<void>(messagePartVTable(this)[0x10C / 4].get(), this);
        noDecideChange(stateMachine(), &sButtonDefinitionOnTouch);
    } else if (buttonCanControl(this) && mEnabled) {
        u32 mode = buttonControlMode(sNoDecideEvents.get());
        if (mode == mControlMode) {
            buttonLeaveCursor(this);
            noDecideChange(stateMachine(), &sButtonDefinitionTouchHeld);
        }
    }
}

/* 02700E18 */
void NoDecideButton_l::updateOnTouchInput() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdButtonEventManager_l* events = sNoDecideEvents.get();
    if (!events) return;
    FrameLocal<hdButtonEvent_l> event(8);
    buttonReadEvent(event.get(), &events->mCurrentEvent);
    u32 id = buttonIdentifier(this);
    bool clear = false;
    if (u32(event->mReceiverId) == id) {
        if (!call_ptr<u32>(messagePartVTable(this)[0xEC / 4].get(), this, u32(event->mType))) return;
        clear = true;
    } else if (u32(event->mReceiverId) == 0xFFFFFFFFu) {
        if (buttonCanControl(this) && mEnabled) {
            u32 mode = buttonControlMode(sNoDecideEvents.get());
            if (mode == mControlMode && buttonEventIsTouch(event->mType) &&
                call_ptr<u32>(messagePartVTable(this)[0xDC / 4].get(), this, u32(event->mType))) {
                buttonEnterCursor(this);
                noDecideChange(stateMachine(), &sButtonDefinitionReady);
            }
        }
        return;
    } else if (event->mReceiverId == 0) clear = true;
    if (clear) {
        if (buttonCanControl(this) && mEnabled)
            noDecideChange(stateMachine(), &sButtonDefinitionOnCursor);
        else noDecideChange(stateMachine(), &sButtonDefinitionReady);
    }
}

/* 02700F70 */
void NoDecideButton_l::updateOnCursorInput() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdButtonEventManager_l* events = sNoDecideEvents.get();
    if (!events) return;
    FrameLocal<hdButtonEvent_l> event(8);
    buttonReadEvent(event.get(), &events->mCurrentEvent);
    if (u32(event->mReceiverId) == 0xFFFFFFFFu) {
        if (buttonCanControl(this) && mEnabled) {
            u32 mode = buttonControlMode(sNoDecideEvents.get());
            if (mode == mControlMode && buttonEventIsTouch(event->mType) &&
                call_ptr<u32>(messagePartVTable(this)[0xDC / 4].get(), this, u32(event->mType))) {
                buttonEnterCursor(this);
                noDecideChange(stateMachine(), &sButtonDefinitionReady);
            }
        }
    } else {
        if (buttonCanControl(this) && mEnabled) {
            u32 mode = buttonControlMode(sNoDecideEvents.get());
            if (mode == mControlMode) return;
        }
        noDecideChange(stateMachine(), &sButtonDefinitionReady);
    }
}

/* 027012D0 */
void NoDecideButton_l::enterCursorAnimation() {
    noDecideStartAnimation(mpLayout.get()->animSet(), 0, 0, 1.0f);
    mAnimationState = 1;
}

/* 02701508 */
void NoDecideButton_l::updateReadyInput() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sNoDecideEvents.get() || !buttonCanControl(this) || !mEnabled) return;
    FrameLocal<hdButtonEvent_l> event(8);
    buttonReadEvent(event.get(), &sNoDecideEvents.get()->mCurrentEvent);
    u32 id = buttonIdentifier(this);
    if (u32(event->mReceiverId) == id) {
        if (call_ptr<u32>(messagePartVTable(this)[0xE4 / 4].get(), this, u32(event->mType))) buttonOnTouch(this);
    } else if (u32(event->mReceiverId) == 0xFFFFFFFFu) {
        u32 mode = buttonControlMode(sNoDecideEvents.get());
        if (mode != mControlMode) return;
        if (buttonEventIsTouch(event->mType)) {
            if (call_ptr<u32>(messagePartVTable(this)[0xDC / 4].get(), this, u32(event->mType))) buttonEnterCursor(this);
        } else buttonLeaveCursor(this);
    } else buttonLeaveCursor(this);
}

extern hdPartsStateId_c* buttonCurrentState(hdStateMachine*);
extern void buttonIdle(NoDecideButton_l*);
extern void buttonSetInputEnabled(NoDecideButton_l*, u32);
static PartStatic1_l sButtonCursor;
extern void buttonSetCursor(void*, hdPartsStateId_c*);
/* inlined into NoDecideButton state predicates */
static bool buttonIsState(NoDecideButton_l* button, hdPartsStateId_c* expected) {
    hdPartsStateId_c* current = buttonCurrentState(button->stateMachine());
    u32 currentFn = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedTable = expected->mVtable.get();
    u32 currentId = call_ptr<u32>(currentFn, current);
    u32 expectedId = call_ptr<u32>(expectedTable->mGetKey.get(), expected);
    return currentId == expectedId;
}

/* 02701A20 */
void NoDecideButton_l::updateIdle() {
    if (!buttonIsState(this, &sNoDecideNone) && !buttonIsState(this, &sButtonDefinitionHide) &&
        !buttonIsState(this, &sButtonDefinitionShow)) buttonIdle(this);
}

/* 02701CD8 */
void NoDecideButton_l::enableWhileActive() {
    if (buttonIsState(this, &sButtonDefinitionOnTouch) || buttonIsState(this, &sButtonDefinitionTouchHeld) ||
        buttonIsState(this, &sButtonDefinitionReady) || buttonIsState(this, &sButtonDefinitionHide) ||
        buttonIsState(this, &sButtonDefinitionShow)) {
        noDecideSetEnabled(this, 1);
        buttonSetInputEnabled(this, 1);
    }
}

/* 02701E90 */
void NoDecideButton_l::updateCursorIndicator() {
    if (!buttonCanControl(this) || !mEnabled) return;
    u32 modeAddress = buttonControlMode(sNoDecideEvents.get());
    if (!modeAddress) return;
    u32 id = buttonIdentifier(this);
    struct ControlMode_l { u8 mReserved[0x10]; be<u32> mButtonId; };
    if (at<ControlMode_l>(modeAddress)->mButtonId != id) return;
    if (buttonIsState(this, &sButtonDefinitionOff) || buttonIsState(this, &sButtonDefinitionReady))
        buttonSetCursor(sButtonCursor.get(), &sButtonCursorOn);
    else buttonSetCursor(sButtonCursor.get(), &sButtonCursorOff);
}

/* 027010CC */
void NoDecideButton_l::updateOffAnimation() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdButtonEventManager_l* events = sNoDecideEvents.get();
    if (!events) return;
    FrameLocal<hdButtonEvent_l> event(8);
    if (!mAnimationState) {
        buttonReadEvent(event.get(), &events->mCurrentEvent);
        u32 id = buttonIdentifier(this);
        bool clear = false;
        if (u32(event->mReceiverId) == id) {
            clear = call_ptr<u32>(messagePartVTable(this)[0xEC / 4].get(), this, u32(event->mType)) != 0;
        } else if (u32(event->mReceiverId) == 0xFFFFFFFFu) {
            if (buttonCanControl(this) && mEnabled) {
                u32 mode = buttonControlMode(sNoDecideEvents.get());
                if (mode == mControlMode && buttonEventIsTouch(event->mType) &&
                    call_ptr<u32>(messagePartVTable(this)[0xDC / 4].get(), this, u32(event->mType))) {
                    buttonEnterCursor(this);
                    mAnimationState = 2;
                }
            }
        } else clear = event->mReceiverId == 0;
        if (clear) mAnimationState = buttonCanControl(this) && mEnabled ? 1 : 2;
    }
    if (!noDecideAnimationDone(mpLayout.get()->animSet(), 0)) return;
    u32 animation = mAnimationState;
    if (animation == 2) noDecideChange(stateMachine(), &sButtonDefinitionReady);
    else {
        u32 id = buttonIdentifier(this);
        buttonMakeEvent(event.get(), 17, id, 0);
        buttonQueueEvent(sNoDecideEvents.get()->mQueue, event.get());
        noDecideChange(stateMachine(), animation == 1 ? &sButtonDefinitionOnCursor : &sButtonDefinitionOffAnimation);
    }
}

/* 0270131C */
void NoDecideButton_l::updateCursorAnimation() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdButtonEventManager_l* events = sNoDecideEvents.get();
    if (!events) return;
    FrameLocal<hdButtonEvent_l> event(8);
    if (mAnimationState == 1) {
        buttonReadEvent(event.get(), &events->mCurrentEvent);
        if (u32(event->mReceiverId) == 0xFFFFFFFFu) {
            if (buttonCanControl(this) && mEnabled) {
                u32 mode = buttonControlMode(sNoDecideEvents.get());
                if (mode == mControlMode && buttonEventIsTouch(event->mType) &&
                    call_ptr<u32>(messagePartVTable(this)[0xDC / 4].get(), this, u32(event->mType))) {
                    buttonEnterCursor(this);
                    mAnimationState = 2;
                }
            }
        } else {
            bool active = buttonCanControl(this) != 0;
            if (active && mEnabled) {
                u32 mode = buttonControlMode(sNoDecideEvents.get());
                if (mode == mControlMode) active = false;
                else active = true;
            } else active = true;
            if (active) mAnimationState = 2;
        }
    }
    if (!noDecideAnimationDone(mpLayout.get()->animSet(), 0)) return;
    if (mAnimationState == 2) noDecideChange(stateMachine(), &sButtonDefinitionReady);
    else {
        u32 id = buttonIdentifier(this);
        buttonMakeEvent(event.get(), 17, id, 0);
        buttonQueueEvent(sNoDecideEvents.get()->mQueue, event.get());
        noDecideChange(stateMachine(), &sButtonDefinitionOnCursor);
    }
}

static PartStatic0_l sButtonPaneTypeGuard;
static hdUiAdaptorType sButtonPaneType;
static PartStatic0_l sButtonBasePaneTypeGuard;
static hdUiAdaptorType sButtonBasePaneType;
static PartStatic0_l sButtonRootTypeGuard;
static hdUiAdaptorType sButtonRootType;
extern u32 buttonBindPanes(NoDecideButton_l*, hdButtonPane_l*, hdButtonPane_l*);
/* 0270178C */
u32 NoDecideButton_l::create(void* archive, void* resource, u32 mode, u32 flags) {
 if (!call_ptr<u32>(messagePartVTable(this)[0x7C / 4].get(), this, archive, 0u, resource, mode, flags)) return 0;
 if (!call_ptr<u32>(messagePartVTable(this)[0xA4 / 4].get(), this)) return 0;
 auto* list = reinterpret_cast<hdButtonLayoutResources_l*>(mpLayout.get());
 hdButtonPane_l* first = list->mpResources[0].get();
 if (!sButtonPaneTypeGuard) {
  u32 base = sButtonBasePaneTypeGuard;
  sButtonPaneTypeGuard = 1;
  if (!base) {
   u32 root = sButtonRootTypeGuard;
   sButtonBasePaneTypeGuard = 1;
   if (!root) { sButtonRootTypeGuard = 1; sButtonRootType.mParent = nullptr; }
   sButtonBasePaneType.mParent = &sButtonRootType;
  }
  sButtonPaneType.mParent = &sButtonBasePaneType;
 }
 hdButtonPane_l* cursor = nullptr;
 if (first) {
  hdUiAdaptorType* type = call_ptr<hdUiAdaptorType*>(first->mPane.mVtable->mGetRuntimeTypeInfo.get(), first);
  while (type && type != &sButtonPaneType) type = type->mParent.get();
  if (type) cursor = first;
 }
 list = reinterpret_cast<hdButtonLayoutResources_l*>(mpLayout.get());
 u32 count = list->mResourceCount;
 auto panes = list->mpResources.get();
 hdButtonPane_l* second = panes[count > 1 ? 1 : 0].get();
 if (!buttonBindPanes(this, cursor, second)) return 0;
 struct MachineObject_l { gptr<gfn> mpVTable; };
 auto* machine = at<MachineObject_l>(gabi::ea(mStateHolder.mMachine.mpOwner.get()));
 mEnabled = 1;
 auto* current = call_ptr<MachineObject_l*>(machine->mpVTable[0x14 / 4].get(), machine, &sButtonDefinitionOff);
 mStateHolder.mMachine.mpCurrentState = reinterpret_cast<hdStateObject*>(current);
 call_ptr<void>(current->mpVTable[0x1C / 4].get(), current);
 return 1;
}

static PartGlobals_l sButtonGlobals;
static PartRegistration_l sButtonRegistrationA;
static PartRegistration_l sButtonRegistrationB;
static PartRegistration_l sButtonRegistrationC;
static hdUiSafeString sButtonName;
static u8 sButtonNameVTable;
static u8 sButtonStateVTable;
static char sButtonStateParentName;
static PartStatic0_l sButtonStateIdGuard;
static PartStatic0_l sButtonStateId;
extern void buttonRegister(PartRegistration_l*);
extern void buttonPrimaryLock(be<u8>*);
extern void buttonSecondaryLock(be<u8>*);

/* 02701FC4 */
void initializeNoDecideButton() {
 sButtonGlobals.mCount = 0;
 sButtonGlobals.mHead = 0;
 sButtonGlobals.mTail = 0;
 sButtonGlobals.mOwner = 0;
 buttonRegister(&sButtonRegistrationA);
 sButtonGlobals.mMinimumAngle = 0xC0490FDB;
 sButtonGlobals.mMaximumAngle = 0x40490FDB;
 buttonPrimaryLock(&sButtonGlobals.mPrimaryLock);
 buttonRegister(&sButtonRegistrationB);
 buttonSecondaryLock(&sButtonGlobals.mSecondaryLock);
 buttonRegister(&sButtonRegistrationC);
 sButtonName.mVtable = at<hdUiStringTable>(ea(&sButtonNameVTable));
 sButtonName.mText.v = gabi::guest_addr("NoDecideButton");
 u32 id = 0;
 if (sButtonStateIdGuard) id = sButtonStateId;
 else sButtonStateIdGuard = 1;
 sButtonDefinitionOff.setup(++id, "::StateID_Off", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 35, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionOffAnimation.setup(++id, "::StateID_OnTouch", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 38, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionOnCursor.setup(++id, "::StateID_OnCursor", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 41, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionOnTouch.setup(++id, "::StateID_SelectTouch", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 44, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionTouchHeld.setup(++id, "::StateID_SelectCursor", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 47, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionReady.setup(++id, "::StateID_UnSelect", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 50, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionDisabled.setup(++id, "::StateID_Invalid", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 53, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionHide.setup(++id, "::StateID_In", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 56, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonDefinitionShow.setup(++id, "::StateID_Out", at<hdPartsStateIdVtbl_c>(ea(&sButtonStateVTable)), 59, at<hdStateId>(ea(&sButtonStateParentName)));
 sButtonStateId = id;
}
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* noDecideAllocate(u32)
ext 0273AF40 void noDecideDelete(void*)
ext 026F5A60 void noDecideConstructBase(NoDecideButton_l*)
ext 026F59F4 void noDecideDestroyBase(NoDecideButton_l*,u32)
ext 020053E4 void noDecideStartAnimation(void*,u32,u32,f32)
ext 02005840 u32 noDecideAnimationDone(void*,u32)
ext 02005708 void noDecideSetAnimationStart(void*,u32,u32)
ext 02005674 void noDecideSetAnimationEnd(void*,u32,u32)
ext 026F5CCC void noDecideSetEnabled(NoDecideButton_l*,u32)
ext 020063C0 void noDecideChange(hdStateMachine*,hdPartsStateId_c*)
ext 026F65BC void noDecideResetButtons(NoDecideButton_l*)
ext 026F8B00 void noDecideUpdateBase(NoDecideButton_l*)
ext 02006364 void noDecideUpdateState(hdStateMachine*)
ext 0273B050 void* noDecideAllocateHeap(u32,void*,u32)
ext 026FADB0 hdPartsLayout_c* noDecideConstructLayout(hdPartsLayout_c*)
data sNoDecideHeap 1018C404
data sNoDecideVTable 1010B878
data sNoDecideReceiver 1010BA78
data sNoDecideStateVTable 1010BA88
data sNoDecideNone 1049DD60
data sNoDecideEvents 101F4FF0
ext 020013C0 void buttonReadEvent(hdButtonEvent_l*,hdButtonEvent_l*)
ext 026F5B74 u32 buttonIdentifier(NoDecideButton_l*)
ext 026F67A4 u32 buttonCanControl(NoDecideButton_l*)
ext 02615140 u32 buttonControlMode(hdButtonEventManager_l*)
ext 026F621C u32 buttonEventIsTouch(u32)
ext 026F66B0 void buttonOnTouch(NoDecideButton_l*)
ext 026F6918 void buttonLeaveCursor(NoDecideButton_l*)
ext 026F69A4 void buttonEnterCursor(NoDecideButton_l*)
ext 02001338 void buttonMakeEvent(hdButtonEvent_l*,u32,u32,u32)
ext 02001FF4 void buttonQueueEvent(void*,hdButtonEvent_l*)
ext 02006478 hdPartsStateId_c* buttonCurrentState(hdStateMachine*)
ext 026F5B80 void buttonIdle(NoDecideButton_l*)
ext 026F5CD4 void buttonSetInputEnabled(NoDecideButton_l*,u32)
data sButtonCursor 101F5A44
data sButtonCursorOn 104920F0
data sButtonCursorOff 1049DA90
ext 026538F4 void buttonSetCursor(void*,hdPartsStateId_c*)
data sButtonPaneTypeGuard 101FD938
data sButtonPaneType 101FD93C
data sButtonBasePaneTypeGuard 101FD88C
data sButtonBasePaneType 101FD940
data sButtonRootTypeGuard 101FD888
data sButtonRootType 101FD8FC
ext 026F6544 u32 buttonBindPanes(NoDecideButton_l*,hdButtonPane_l*,hdButtonPane_l*)
data sButtonGlobals 1049E520
data sButtonRegistrationA 101F72F4
data sButtonRegistrationB 101F7300
data sButtonRegistrationC 101F730C
data sButtonName 1049E6EC
data sButtonNameVTable 1010B6FC
data sButtonStateVTable 1010B75C
data sButtonStateParentName 101FF32C
data sButtonStateIdGuard 101FD8E4
data sButtonStateId 101FDD50
ext 028F026C void buttonRegister(PartRegistration_l*)
ext 028ED6F8 void buttonPrimaryLock(be<u8>*)
ext 028EAB2C void buttonSecondaryLock(be<u8>*)
data sButtonDefinitionOff 1049E53C
data sButtonDefinitionOffAnimation 1049E56C
data sButtonDefinitionOnCursor 1049E59C
data sButtonDefinitionOnTouch 1049E5CC
data sButtonDefinitionTouchHeld 1049E5FC
data sButtonDefinitionReady 1049E62C
data sButtonDefinitionDisabled 1049E65C
data sButtonDefinitionHide 1049E68C
data sButtonDefinitionShow 1049E6BC
str 1010B814 "NoDecideButton" in 02701FC4
str 1010B7B4 "::StateID_Off" in 02701FC4
str 1010B7C4 "::StateID_OnTouch" in 02701FC4
str 1010B824 "::StateID_OnCursor" in 02701FC4
str 1010B7D8 "::StateID_SelectTouch" in 02701FC4
str 1010B838 "::StateID_SelectCursor" in 02701FC4
str 1010B850 "::StateID_UnSelect" in 02701FC4
str 1010B7F0 "::StateID_Invalid" in 02701FC4
str 1010B864 "::StateID_In" in 02701FC4
str 1010B804 "::StateID_Out" in 02701FC4
*/
