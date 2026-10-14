/* HD difficulty button; private game-derived reconstruction. */
#include "d/hd_screen_btn_difficulty.h"
static u32 sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
static u32 sExplanationStateVtable;
static u32 sButtonVtable;
static u32 sStateHolderVtable;
static u32 sSecondaryVtable;
typedef gptr<void> hdDifficultySequencePointer;
static hdDifficultyRuntimeWords sDifficultyRuntime;
static hdDifficultyRegistration sDifficultyRuntimeRegistration;
static hdDifficultyRegistration sDifficultyFirstRegistration;
static hdDifficultyRegistration sDifficultySecondRegistration;
static hdDifficultyContext sDifficultyFirstContext;
static hdDifficultyContext sDifficultySecondContext;
static f32 sDifficultyMinimumAngle;
static f32 sDifficultyMaximumAngle;
static hdDifficultyAnimationNames sDifficultyButtonNames;
static hdDifficultyResourceNames sDifficultyResourceNames;
static hdDifficultyNamePair sDifficultyTextNames;
static hdUiSafeString sDifficultyHitboxName;
static hdPartsStateCounter_c sDifficultyCounterReady;
static hdPartsStateCounter_c sDifficultyNextState;
extern u32 hdDifficultyExplanationIsDone(hdBtnDifficulty_c* self);
extern void hdDifficultyRegisterStatics(hdDifficultyRegistration* registration);
extern void hdDifficultyConstructFirstContext(hdDifficultyContext* context);
extern void hdDifficultyConstructSecondContext(hdDifficultyContext* context);
extern void hdDifficultyExplanationEnter(hdBtnDifficulty_c* self);
extern void hdDifficultyExplanationExecute(hdBtnDifficulty_c* self);
extern void hdDifficultyExplanationExit(hdBtnDifficulty_c* self);
static hdPartsStateId_c sDifficultyOffState;
static hdStateId sDifficultyOffNullState;
static hdPartsStateId_c sDifficultyOnTouchState;
static hdStateId sDifficultyOnTouchNullState;
static hdStateId sDifficultyOnCursorNullState;
static hdPartsStateId_c sDifficultySelectTouchState;
static hdStateId sDifficultySelectTouchNullState;
static hdPartsStateId_c sDifficultySelectCursorState;
static hdStateId sDifficultySelectCursorNullState;
static hdStateId sDifficultyUnSelectNullState;
typedef gptr<hdButtonEventManager_l> hdDifficultyEventManagerPointer;
static hdDifficultyEventManagerPointer sDifficultyEventManager;
extern void hdDifficultyCopyEvent(hdButtonEvent_l* output, hdButtonEvent_l* input);
extern u32 hdDifficultyReceiverId(hdButtonPart_c* self);
extern u32 hdDifficultyHasFocus(hdButtonPart_c* self);
extern void hdDifficultyClearTouch(hdButtonPart_c* self);
extern hdButtonReceiver_l* hdDifficultyFocusedReceiver(hdButtonEventManager_l* manager);
extern void hdDifficultySynchronizeFocus(hdButtonPart_c* self);
extern u32 hdDifficultyIsActivation(u32 eventType);
extern void hdDifficultyReleaseFocus(hdButtonPart_c* self);
extern void hdDifficultyBuildEvent(hdButtonEvent_l* event, u32 type, u32 receiver, u32 button);
extern void hdDifficultyPushEvent(void* queue, hdButtonEvent_l* event);
/* Private game-specific virtual extension, retaining the canonical parts prefix. */
static hdDifficultyButtonVtable_l* difficultyVtable(hdBtnDifficulty_c* self) {
    return reinterpret_cast<hdDifficultyButtonVtable_l*>(self->mButton.mVtable.get());
}
static hdDifficultySequencePointer sDifficultySequence;
static hdDifficultyUiPointer sDifficultyUiSystem;
static hdDifficultyAnimationNames sDifficultyAnimationNames;
static hdDifficultyAnimationGroups sDifficultyAnimationGroups;
static hdDifficultyGroupIndices sDifficultyGroupIndices;
extern void hdDifficultyBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
static hdPartsStateId_c sDifficultyIdleState;
extern void hdDifficultyCompleteSequence(void* sequence);
static hdUiSafeString sDifficultyLayoutName;
static f32 sDifficultyAnimationRate;
extern void hdDifficultyPlaySound(const char* name);
extern void hdDifficultyInitializeBase(hdButtonPart_c* self, hdUiSafeString* name, u32 resource, u32 option, void* archive);
extern void hdDifficultyFinalizeBase(hdButtonPart_c* self);
static hdPartsStateId_c sDifficultySelectedState;
static hdPartsStateId_c sDifficultyUnselectedState;
extern void hdDifficultyDestroyBase(hdButtonPart_c*, u32);
extern hdBtnDifficulty_c* hdDifficultyAllocate(u32);
extern void hdDifficultyDelete(void*);
extern void hdDifficultyStopAnimation(hdAnimSet*, u32, u32);
/* 0262CFB4 */
u32 hdDifficultyStateKey(hdPartsStateId_c* state) {
    return state->mNumber;
}
/* 0262D0B4 */
void hdBtnDifficulty_c::destroy(hdBtnDifficulty_c* self, u32 flags) {
    if (!self) return;
    self->mButton.mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    self->mButton.mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    hdDifficultyDestroyBase(&self->mButton, 0);
    if (flags & 1) hdDifficultyDelete(self);
}
/* 0262D120 */
void hdBtnDifficulty_c::offMove() {
    hdButtonEventManager_l* manager = sDifficultyEventManager;
    if (!manager) return;
    gabi::LocalOf<hdButtonEvent_l> event;
    hdDifficultyCopyEvent(event, &manager->mCurrentEvent);
    const u32 receiver = hdDifficultyReceiverId(&mButton);
    if (u32(event->mReceiverId) == receiver) {
        if (!gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptTouch.get(), this, u32(event->mType))) return;
        if (hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled) hdDifficultyClearTouch(&mButton);
        gabi::call_ptr(difficultyVtable(this)->mOnTouch.get(), this);
        mButton.changeState(&sDifficultySelectTouchState);
    } else if (hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled) {
        hdButtonReceiver_l* focused = hdDifficultyFocusedReceiver(sDifficultyEventManager);
        if (focused == mButton.mpReceiver.get()) {
            hdDifficultySynchronizeFocus(&mButton);
            mButton.changeState(&sDifficultySelectCursorState);
        }
    }
}
/* 0262DB60 */
void hdBtnDifficulty_c::selectedMove() {
    hdButtonEventManager_l* manager = sDifficultyEventManager;
    gabi::LocalOf<hdButtonEvent_l> event;
    hdDifficultyCopyEvent(event, &manager->mCurrentEvent);
    const u32 receiver = hdDifficultyReceiverId(&mButton);
    if (u32(event->mReceiverId) == receiver) {
        if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptTouch.get(), this, u32(event->mType))) {
            if (hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled) hdDifficultyClearTouch(&mButton);
            gabi::call_ptr(difficultyVtable(this)->mOnTouch.get(), this);
            chooseNextState();
            return;
        }
    } else if (u32(event->mReceiverId) == 0xFFFFFFFF) {
        hdButtonReceiver_l* focused = hdDifficultyFocusedReceiver(sDifficultyEventManager);
        if (focused == mButton.mpReceiver.get() && gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptEvent.get(), this, event.get())) {
            toggleSelection();
            playSwitchSound();
            chooseNextState();
            return;
        }
    }
    if (hdAnimSet_isEnd(mButton.animSet(), mAnimation)) {
        mAnimation = 1;
        mButton.changeState(&sDifficultyOffState);
    }
}
/* 0262D2A8 */
void hdBtnDifficulty_c::onTouchMove() {
    hdButtonEventManager_l* manager = sDifficultyEventManager;
    if (!manager) return;
    gabi::LocalOf<hdButtonEvent_l> event;
    hdDifficultyCopyEvent(event, &manager->mCurrentEvent);
    const u32 receiver = hdDifficultyReceiverId(&mButton);
    if (u32(event->mReceiverId) == receiver) {
        if (!gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptSelection.get(), this, u32(event->mType))) return;
        toggleSelection();
        playSwitchSound();
        chooseNextState();
    } else if (u32(event->mReceiverId) == 0xFFFFFFFF) {
        if (hdDifficultyFocusedReceiver(sDifficultyEventManager) != mButton.mpReceiver.get()) return;
        if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptEvent.get(), this, event.get())) {
            toggleSelection();
            playSwitchSound();
            chooseNextState();
        } else if (hdDifficultyIsActivation(event->mType) && gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptActivation.get(), this, u32(event->mType))) {
            hdDifficultyReleaseFocus(&mButton);
            chooseNextState();
        }
    } else if (u32(event->mReceiverId) == 0) {
        if (hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled) mButton.changeState(&sDifficultyIdleState);
        else chooseNextState();
    }
}
/* 0262D444 */
void hdBtnDifficulty_c::onCursorMove() {
    hdButtonEventManager_l* manager = sDifficultyEventManager;
    if (!manager) return;
    gabi::LocalOf<hdButtonEvent_l> event;
    hdDifficultyCopyEvent(event, &manager->mCurrentEvent);
    const u32 receiver = hdDifficultyReceiverId(&mButton);
    if (u32(event->mReceiverId) == receiver) {
        if (!gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptSelection.get(), this, u32(event->mType))) return;
        toggleSelection();
        playSwitchSound();
        chooseNextState();
    } else if (u32(event->mReceiverId) == 0xFFFFFFFF) {
        if (hdDifficultyFocusedReceiver(sDifficultyEventManager) != mButton.mpReceiver.get()) return;
        if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptEvent.get(), this, event.get())) {
            toggleSelection();
            playSwitchSound();
            chooseNextState();
        } else if (hdDifficultyIsActivation(event->mType) && gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptActivation.get(), this, u32(event->mType))) {
            hdDifficultyReleaseFocus(&mButton);
            mButton.changeState(&sDifficultyUnselectedState);
        }
    } else {
        if (hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled && hdDifficultyFocusedReceiver(sDifficultyEventManager) == mButton.mpReceiver.get()) return;
        mButton.changeState(&sDifficultyUnselectedState);
    }
}
/* 0262D62C */
void hdBtnDifficulty_c::touchMove() {
    hdButtonEventManager_l* manager = sDifficultyEventManager;
    if (!manager) return;
    gabi::LocalOf<hdButtonEvent_l> event;
    if (mInputMode == 0) {
        hdDifficultyCopyEvent(event, &manager->mCurrentEvent);
        const u32 receiver = hdDifficultyReceiverId(&mButton);
        if (u32(event->mReceiverId) == receiver) {
            if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptSelection.get(), this, u32(event->mType))) {
                toggleSelection();
                playSwitchSound();
                chooseNextState();
                return;
            }
        } else if (u32(event->mReceiverId) == 0xFFFFFFFF) {
            if (hdDifficultyFocusedReceiver(sDifficultyEventManager) == mButton.mpReceiver.get()) {
                if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptEvent.get(), this, event.get())) {
                    toggleSelection();
                    playSwitchSound();
                    chooseNextState();
                    return;
                } else if (hdDifficultyIsActivation(event->mType) && gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptActivation.get(), this, u32(event->mType))) {
                    hdDifficultyReleaseFocus(&mButton);
                    mInputMode = 2;
                }
            }
        } else if (u32(event->mReceiverId) == 0) {
            if (hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled) mInputMode = 1;
            else mInputMode = 2;
        }
    }
    if (!hdAnimSet_isEnd(mButton.animSet(), 0)) return;
    const u32 mode = mInputMode;
    if (mode == 2) { chooseNextState(); return; }
    const u32 receiver = hdDifficultyReceiverId(&mButton);
    hdDifficultyBuildEvent(event, HD_BUTTON_EVENT_SPECIAL_e, receiver, 0);
    hdButtonEventManager_l* currentManager = sDifficultyEventManager;
    hdDifficultyPushEvent(&currentManager->mQueue, event);
    if (mode == 1) mButton.changeState(&sDifficultyIdleState);
    else mButton.changeState(&sDifficultyOnTouchState);
}
/* 0262D8CC */
void hdBtnDifficulty_c::cursorMove() {
    hdButtonEventManager_l* manager = sDifficultyEventManager;
    if (!manager) return;
    gabi::LocalOf<hdButtonEvent_l> event;
    if (mInputMode == 1) {
        hdDifficultyCopyEvent(event, &manager->mCurrentEvent);
        const u32 receiver = hdDifficultyReceiverId(&mButton);
        if (u32(event->mReceiverId) == receiver) {
            if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptSelection.get(), this, u32(event->mType))) {
                toggleSelection();
                playSwitchSound();
                chooseNextState();
                return;
            }
        } else if (u32(event->mReceiverId) == 0xFFFFFFFF) {
            if (hdDifficultyFocusedReceiver(sDifficultyEventManager) == mButton.mpReceiver.get()) {
                if (gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptEvent.get(), this, event.get())) {
                    toggleSelection();
                    playSwitchSound();
                    chooseNextState();
                    return;
                } else if (hdDifficultyIsActivation(event->mType) && gabi::call_ptr<u32>(difficultyVtable(this)->mAcceptActivation.get(), this, u32(event->mType))) {
                    hdDifficultyReleaseFocus(&mButton);
                    mInputMode = 2;
                }
            }
        } else {
            if (!(hdDifficultyHasFocus(&mButton) && mButton.mTouchEnabled && hdDifficultyFocusedReceiver(sDifficultyEventManager) == mButton.mpReceiver.get())) mInputMode = 2;
        }
    }
    if (!hdAnimSet_isEnd(mButton.animSet(), 0)) return;
    const u32 mode = mInputMode;
    if (mode == 2) { chooseNextState(); return; }
    const u32 receiver = hdDifficultyReceiverId(&mButton);
    hdDifficultyBuildEvent(event, HD_BUTTON_EVENT_SPECIAL_e, receiver, 0);
    hdButtonEventManager_l* currentManager = sDifficultyEventManager;
    hdDifficultyPushEvent(&currentManager->mQueue, event);
    mButton.changeState(&sDifficultyIdleState);
}
/* 0262D230 */
void hdBtnDifficulty_c::toggleSelection() {
    if (mSelected == 1) { mAnimation = 6; mSelected = 0; }
    else { mAnimation = 5; mSelected = 1; }
}
/* 0262D264 */
void hdBtnDifficulty_c::playSwitchSound() {
    hdDifficultyPlaySound("SE_DIFFICULTY_SWITCH");
}
/* 0262D270 */
void hdBtnDifficulty_c::chooseNextState() {
    if (!mSelectionLocked && mSelected == 1) mButton.changeState(&sDifficultySelectedState);
    else mButton.changeState(&sDifficultyUnselectedState);
}
/* 0262D5E0 */
void hdBtnDifficulty_c::touchEnter() {
    hdAnimSet_play(mButton.animSet(), 0, 0, sDifficultyAnimationRate);
    mInputMode = 0;
}
/* 0262D880 */
void hdBtnDifficulty_c::cursorEnter() {
    hdAnimSet_play(mButton.animSet(), 0, 0, sDifficultyAnimationRate);
    mInputMode = 1;
}
/* 0262DAC0 */
void hdBtnDifficulty_c::selectedEnter() {
    auto* current = reinterpret_cast<hdPartsStateId_c*>(mButton.stateMachine()->mpLastStateId.get());
    hdPartsStateIdVtbl_c* selectedVtable = sDifficultySelectedState.mVtable;
    const u32 currentKey = current->getKey();
    const u32 selectedKey = gabi::call_ptr<u32>(selectedVtable->mGetKey.get(), &sDifficultySelectedState);
    if (currentKey != selectedKey) hdAnimSet_play(mButton.animSet(), mAnimation, 0, sDifficultyAnimationRate);
}
/* 0262DCCC */
void hdBtnDifficulty_c::animationEnter() {
    hdPartsLayout_c* layout = mButton.mpLayout;
    const u32 animation = mAnimation;
    mAnimationPhase = 0;
    hdAnimSet_play(layout->animSet(), animation, 0, sDifficultyAnimationRate);
}
/* 0262DCF0 */
void hdBtnDifficulty_c::animationMove() {
    const u32 phase = mAnimationPhase;
    if (phase == 0) {
        if (hdAnimSet_isEnd(mButton.animSet(), mAnimation)) {
            mAnimationPhase = u32(mAnimationPhase) + 1;
            hdAnimSet_play(mButton.animSet(), 0, 0, sDifficultyAnimationRate);
        }
    } else if (phase == 1) {
        if (hdAnimSet_isEnd(mButton.animSet(), 0)) {
            mPendingAnimation = 3;
            mAnimationPhase = u32(mAnimationPhase) + 1;
        }
    } else if (phase == 2) {
        const u32 pending = mPendingAnimation;
        if (pending) mPendingAnimation = pending - 1;
        else {
            hdDifficultyCompleteSequence(sDifficultySequence);
            mAnimation = 1;
            mButton.changeState(&sDifficultyIdleState);
            mAnimationPhase = u32(mAnimationPhase) + 1;
        }
    }
}
/* 0262DE30 */
void hdBtnDifficulty_c::lockSelection() {
    mSelectionLocked = 1;
}
/* 0262DE3C */
hdBtnDifficulty_c* hdBtnDifficulty_c::construct(hdBtnDifficulty_c* self, u32 difficulty) {
    if (!self) {
        self = hdDifficultyAllocate(sizeof(hdBtnDifficulty_c));
        if (!self) return nullptr;
    }
    hdPartsNoDecideButton_construct(&self->mButton);
    self->mDifficulty = difficulty;
    self->mButton.mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    self->mButton.mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    self->mButton.mVtable.v = gabi::ea(&sButtonVtable);
    self->mSelectionLocked = 0;
    self->mAnimation = 1;
    self->mAnimationPhase = 0;
    self->mSelected = 0;
    self->mPendingAnimation = 0;
    self->mComplete = 0;
    return self;
}
/* 0262DED8 */
void hdBtnDifficulty_c::initializeDifficulty(u32 resource, u32 option, void* archive) {
    hdDifficultyInitializeBase(&mButton, &sDifficultyLayoutName, resource, option, archive);
}
/* 0262DEF0 */
void hdBtnDifficulty_c::finalizeDifficulty() {
    hdDifficultyFinalizeBase(&mButton);
}
/* 0262DEF4 */
void hdBtnDifficulty_c::selectAnimation() {
    mSelected = 1;
    hdDifficultyStopAnimation(mButton.animSet(), 5, 0);
    mButton.vCalcLayout();
}
/* 0262DF4C */
void hdBtnDifficulty_c::deselectAnimation() {
    mSelected = 0;
    hdDifficultyStopAnimation(mButton.animSet(), 6, 0);
    mButton.vCalcLayout();
}
/* 0262DFA0 */
void hdBtnDifficulty_c::setOverlay(u32 enabled) {
    hdAnimSet* animation = mButton.animSet();
    if (enabled) hdAnimSet_operationCC(animation, 2, 1);
    else hdDifficultyStopAnimation(animation, 2, 1);
}
/* 0262DFC0 */
u32 hdBtnDifficulty_c::loadDifficulty(hdUiSafeString* name, hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* archive) {
    hdScreenHeap* heap = sDifficultyUiSystem->mHeap;
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mButton.mpLayout = layout;
    if (!layout || !layout->vLoad(name, gabi::ea(accessor), resource, option, 8, 2, archive)) return 0;
    for (u32 i = 0; i < 3; ++i) {
        hdAnimSet* animation = mButton.animSet();
        const u32 group = sDifficultyGroupIndices.mIndices[i];
        hdUiSafeString* groupName = gabi::at<hdUiSafeString>(gabi::ea(&sDifficultyAnimationGroups) + group * sizeof(hdUiSafeString));
        hdDifficultyBindAnimation(animation, i + 5, &sDifficultyAnimationNames.mNames[i], groupName);
    }
    return 1;
}
/* 0262E0D8 */
static void initializeDifficultyStatics() {
    sDifficultyRuntime.mWords[3] = 0;
    sDifficultyRuntime.mWords[2] = 0;
    sDifficultyRuntime.mWords[1] = 0;
    sDifficultyRuntime.mWords[0] = 0;
    hdDifficultyRegisterStatics(&sDifficultyRuntimeRegistration);
    sDifficultyMinimumAngle = -3.1415927410125732f;
    sDifficultyMaximumAngle = 3.1415927410125732f;
    hdDifficultyConstructFirstContext(&sDifficultyFirstContext);
    hdDifficultyRegisterStatics(&sDifficultyFirstRegistration);
    hdDifficultyConstructSecondContext(&sDifficultySecondContext);
    hdDifficultyRegisterStatics(&sDifficultySecondRegistration);
    sDifficultyAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyAnimationGroups.mNames[0].mText.v = gabi::guest_addr("G_Btn_00");
    sDifficultyAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyAnimationGroups.mNames[1].mText.v = gabi::guest_addr("G_Valid_00");
    sDifficultyButtonNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyButtonNames.mNames[1].mText.v = gabi::guest_addr("N_BtnNormal_00");
    sDifficultyButtonNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyButtonNames.mNames[0].mText.v = gabi::guest_addr("N_Btn_00");
    sDifficultyHitboxName.mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyHitboxName.mText.v = gabi::guest_addr("B_Btn_00");
    sDifficultyAnimationNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyAnimationNames.mNames[0].mText.v = gabi::guest_addr("NormalHard");
    sDifficultyAnimationNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyAnimationNames.mNames[1].mText.v = gabi::guest_addr("HardNormal");
    sDifficultyAnimationNames.mNames[2].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyAnimationNames.mNames[2].mText.v = gabi::guest_addr("Valid");
    sDifficultyButtonNames.mNames[2].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyButtonNames.mNames[2].mText.v = gabi::guest_addr("N_BtnHard_00");
    sDifficultyResourceNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyResourceNames.mNames[0].mText.v = gabi::guest_addr("P_BtnShadow_00");
    sDifficultyResourceNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyResourceNames.mNames[1].mText.v = gabi::guest_addr("P_BtnSelect_00");
    sDifficultyResourceNames.mNames[2].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyResourceNames.mNames[2].mText.v = gabi::guest_addr("P_BtnHardPic_00");
    sDifficultyResourceNames.mNames[3].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyResourceNames.mNames[3].mText.v = gabi::guest_addr("P_BtnHard_00");
    sDifficultyResourceNames.mNames[4].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyResourceNames.mNames[4].mText.v = gabi::guest_addr("BtnDifficulty_00");
    sDifficultyResourceNames.mNames[5].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyResourceNames.mNames[5].mText.v = gabi::guest_addr("P_BtnNormalPic_00");
    sDifficultyLayoutName.mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyLayoutName.mText.v = gabi::guest_addr("BtnDifficulty_00");
    sDifficultyTextNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyTextNames.mNames[0].mText.v = gabi::guest_addr("T_BtnNormal_00");
    sDifficultyTextNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sDifficultyTextNames.mNames[1].mText.v = gabi::guest_addr("T_BtnHard_00");
    u32 number;
    if (sDifficultyCounterReady.mValue) number = sDifficultyNextState.mValue;
    else { sDifficultyCounterReady.mValue = 1; number = 0; }
    auto* stateVtable = &sStateVtable;
    sDifficultyOffState.setup(++number, "::StateID_Off", stateVtable, 0x23, &sDifficultyOffNullState);
    sDifficultyOnTouchState.setup(++number, "::StateID_OnTouch", stateVtable, 0x26, &sDifficultyOnTouchNullState);
    sDifficultyIdleState.setup(++number, "::StateID_OnCursor", stateVtable, 0x29, &sDifficultyOnCursorNullState);
    sDifficultySelectTouchState.setup(++number, "::StateID_SelectTouch", stateVtable, 0x2C, &sDifficultySelectTouchNullState);
    sDifficultySelectCursorState.setup(++number, "::StateID_SelectCursor", stateVtable, 0x2F, &sDifficultySelectCursorNullState);
    sDifficultyUnselectedState.setup(++number, "::StateID_UnSelect", stateVtable, 0x32, &sDifficultyUnSelectNullState);
    sDifficultySelectedState.mNumber = ++number;
    sDifficultySelectedState.mName.v = gabi::guest_addr("::StateID_HardModeExplain");
    sDifficultySelectedState.mVtable.v = gabi::ea(&sExplanationStateVtable);
    sDifficultySelectedState.mEnter.mThisDelta = 0;
    sDifficultySelectedState.mEnter.mVirtualIndex = -1;
    sDifficultySelectedState.mEnter.mFunction = gabi::guest_fn(reinterpret_cast<const void*>(hdDifficultyExplanationEnter));
    sDifficultySelectedState.mExecute.mThisDelta = 0;
    sDifficultySelectedState.mExecute.mVirtualIndex = -1;
    sDifficultySelectedState.mExecute.mFunction = gabi::guest_fn(reinterpret_cast<const void*>(hdDifficultyExplanationExecute));
    sDifficultySelectedState.mExit.mThisDelta = 0;
    sDifficultySelectedState.mExit.mVirtualIndex = -1;
    sDifficultySelectedState.mExit.mFunction = gabi::guest_fn(reinterpret_cast<const void*>(hdDifficultyExplanationExit));
    sDifficultySelectedState.mUnused.mThisDelta = 0;
    sDifficultySelectedState.mUnused.mVirtualIndex = -1;
    sDifficultySelectedState.mUnused.mFunction = gabi::guest_fn(reinterpret_cast<const void*>(hdDifficultyExplanationIsDone));
    sDifficultyNextState.mValue = number;
}
/* WWHD glue
data sStringVtable 100E6E2C nocheck
data sStateVtable 100E6EE0 nocheck
data sExplanationStateVtable 100E6E98 nocheck
data sButtonVtable 100E70E0 nocheck
data sStateHolderVtable 100E72E0 nocheck
data sSecondaryVtable 100E72F0 nocheck
ext 02001338 void hdDifficultyBuildEvent(hdButtonEvent_l* event, u32 type, u32 receiver, u32 button)
ext 02001FF4 void hdDifficultyPushEvent(void* queue, hdButtonEvent_l* event)
ext 026F621C u32 hdDifficultyIsActivation(u32 eventType)
ext 026F69A4 void hdDifficultyReleaseFocus(hdButtonPart_c* self)
data sDifficultyEventManager 101F4FF0
ext 020013C0 void hdDifficultyCopyEvent(hdButtonEvent_l* output, hdButtonEvent_l* input)
ext 026F5B74 u32 hdDifficultyReceiverId(hdButtonPart_c* self)
ext 026F67A4 u32 hdDifficultyHasFocus(hdButtonPart_c* self)
ext 026F66B0 void hdDifficultyClearTouch(hdButtonPart_c* self)
ext 02615140 hdButtonReceiver_l* hdDifficultyFocusedReceiver(hdButtonEventManager_l* manager)
ext 026F6918 void hdDifficultySynchronizeFocus(hdButtonPart_c* self)
data sDifficultyRuntime 1048F7AC
data sDifficultyRuntimeRegistration 101F547C
data sDifficultyFirstRegistration 101F5488
data sDifficultySecondRegistration 101F5494
data sDifficultyFirstContext 1048F7A8
data sDifficultySecondContext 1048F7A9
data sDifficultyMinimumAngle 1048F7A0
data sDifficultyMaximumAngle 1048F7A4
data sDifficultyButtonNames 1048F7E4
data sDifficultyResourceNames 1048F7FC
data sDifficultyTextNames 1048F82C
data sDifficultyHitboxName 1048F798
data sDifficultyCounterReady 101FD8E4
data sDifficultyNextState 101FDD50
ext 0262E620 u32 hdDifficultyExplanationIsDone(hdBtnDifficulty_c* self) callback
ext 028F026C void hdDifficultyRegisterStatics(hdDifficultyRegistration* registration)
ext 028ED6F8 void hdDifficultyConstructFirstContext(hdDifficultyContext* context)
ext 028EAB2C void hdDifficultyConstructSecondContext(hdDifficultyContext* context)
ext 0262DCCC void hdDifficultyExplanationEnter(hdBtnDifficulty_c* self) callback
ext 0262DCF0 void hdDifficultyExplanationExecute(hdBtnDifficulty_c* self) callback
ext 0262DE30 void hdDifficultyExplanationExit(hdBtnDifficulty_c* self) callback
data sDifficultyOffState 1048F83C
data sDifficultyOffNullState 1049E53C
str 100E6F78 "::StateID_Off" in 0262E0D8
data sDifficultyOnTouchState 1048F86C
data sDifficultyOnTouchNullState 1049E56C
str 100E6F88 "::StateID_OnTouch" in 0262E0D8
data sDifficultyOnCursorNullState 1049E59C
str 100E709C "::StateID_OnCursor" in 0262E0D8
data sDifficultySelectTouchState 1048F8CC
data sDifficultySelectTouchNullState 1049E5CC
str 100E6F9C "::StateID_SelectTouch" in 0262E0D8
data sDifficultySelectCursorState 1048F8FC
data sDifficultySelectCursorNullState 1049E5FC
str 100E70B0 "::StateID_SelectCursor" in 0262E0D8
data sDifficultyUnSelectNullState 1049E62C
str 100E70C8 "::StateID_UnSelect" in 0262E0D8
str 100E6F4C "Valid" in 0262E0D8
str 100E6F54 "P_BtnHardPic_00" in 0262E0D8
str 100E6F64 "P_BtnNormalPic_00" in 0262E0D8
str 100E6FD0 "G_Btn_00" in 0262E0D8
str 100E6FDC "G_Valid_00" in 0262E0D8
str 100E6FE8 "NormalHard" in 0262E0D8
str 100E6FF4 "HardNormal" in 0262E0D8
str 100E7000 "B_Btn_00" in 0262E0D8
str 100E700C "N_Btn_00" in 0262E0D8
str 100E7018 "N_BtnNormal_00" in 0262E0D8
str 100E7028 "N_BtnHard_00" in 0262E0D8
str 100E7038 "P_BtnShadow_00" in 0262E0D8
str 100E7048 "P_BtnSelect_00" in 0262E0D8
str 100E7058 "P_BtnHard_00" in 0262E0D8
str 100E7068 "BtnDifficulty_00" in 0262E0D8
str 100E707C "T_BtnNormal_00" in 0262E0D8
str 100E708C "T_BtnHard_00" in 0262E0D8
str 100E6FB4 "::StateID_HardModeExplain" in 0262E0D8

data sDifficultyUiSystem 1018C404
data sDifficultyAnimationNames 1048F7CC
data sDifficultyAnimationGroups 1048F7BC
data sDifficultyGroupIndices 100E6E44
ext 02004E04 void hdDifficultyBindAnimation(hdAnimSet* animation, u32 entry, hdUiSafeString* name, hdUiSafeString* group)
data sDifficultySequence 101F82EC
data sDifficultyIdleState 1048F89C
ext 0270B744 void hdDifficultyCompleteSequence(void* sequence)
data sDifficultyLayoutName 1048F95C
data sDifficultyAnimationRate 100E6F28
str 100E6F2C "SE_DIFFICULTY_SWITCH" in 0262D264
ext 02030B38 void hdDifficultyPlaySound(const char* name)
ext 0270178C void hdDifficultyInitializeBase(hdButtonPart_c* self, hdUiSafeString* name, u32 resource, u32 option, void* archive)
ext 027019B8 void hdDifficultyFinalizeBase(hdButtonPart_c* self)
data sDifficultySelectedState 1048F964
data sDifficultyUnselectedState 1048F92C
ext 02700C9C void hdDifficultyDestroyBase(hdButtonPart_c* screen, u32 flags)
ext 0273AD10 hdBtnDifficulty_c* hdDifficultyAllocate(u32 size)
ext 0273AF40 void hdDifficultyDelete(void* screen)
ext 02005708 void hdDifficultyStopAnimation(hdAnimSet* animation, u32 entry, u32 slot)
*/
