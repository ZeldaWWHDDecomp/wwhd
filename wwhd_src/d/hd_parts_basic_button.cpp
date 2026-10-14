/** HD BasicButton animations and state transitions. */
#include "d/hd_parts_basic_button.h"
static u32 sBasicPartVtable;
static u32 sBasicInterfaceVtable;
static hdPartsStateId_c sBasicOff;
static hdPartsStateId_c sBasicCancel;
static hdPartsStateId_c sBasicOnCursor;
static hdPartsStateId_c sBasicInvalid;
static hdPartsStateId_c sBasicIn;
static hdPartsStateId_c sBasicOut;
static hdPartsStateId_c sBasicDisableWait;
void hdBasic_changeState(hdStateMachine*, hdPartsStateId_c*);
void hdBasic_play(void*, u32, u32, f32);
void hdBasic_updateState(hdStateMachine*);
BOOL hdBasic_finished(void*, u32);
void hdBasic_stop(void*, u32, u32);
void hdBasic_pause(void*, u32, u32);
BOOL hdBasic_inputEnabled();
void hdBasic_unloadBase(hdBasicButton_c*);
hdPartsStateId_c* hdBasic_currentState(hdStateMachine*);
static u32 stateId(hdPartsStateId_c* state) {
    return gabi::call_ptr<u32>(state->mVtable->mGetKey.get(), state);
}
static hdPartsStateId_c sBasicEnd;
static hdPartsStateId_c sBasicSelectCursor;
static hdPartsStateId_c sBasicUnSelect;
static hdPartsStateId_c sBasicSelectTouch;
static hdPartsStateId_c sBasicDecide;
static hdPartsStateId_c sBasicOnTouch;
static hdButtonEventManager_l* sBasicManager;
static void* sBasicScaleManager;
static hdScreenUiSystem* sBasicHeapManager;
static f32 sBasicDefaultScale;
static f32 sBasicTouchScale;
static hdUiSafeString sBasicSound;
static u32 sBasicStringVtable;
static u32 sBasicVtable;
u32 hdBasic_getMode(hdButtonEventManager_l*);
void hdBasic_setScale(void*, be<f32>*);
void hdBasic_soundName(be<u8>*);
void hdBasic_soundId(u32);
void* hdBasic_allocate(u32, void*, u32);
hdPartsLayout_c* hdBasic_constructLayout(hdPartsLayout_c*);
hdButtonPart_c* hdBasic_constructButton(hdButtonPart_c*);
struct hdBasicVtable_l {
    u8 mPrefix[0x7C];
    gabi::gfn mLoad;
    u8 mToCreate[0xA4 - 0x80];
    gabi::gfn mCreate;
    u8 mToUpdate[0xB4 - 0xA8];
    gabi::gfn mUpdateLayout;
    u8 mToDispatch[0xDC - 0xB8];
    gabi::gfn mDispatch;
    u8 mGapDecide[4];
    gabi::gfn mIsDecide;
    u8 mGapCancel[4];
    gabi::gfn mIsCancel;
    u8 mGapTouch[4];
    gabi::gfn mIsTouch;
    u8 mGapTouchEnd[4];
    gabi::gfn mIsTouchEnd;
    u8 mGapReceiver[4];
    gabi::gfn mUpdateReceiver;
    u8 mGapCursor[4];
    gabi::gfn mOnCursor;
    u8 mStorage[0x23C - 0x110];
    gabi::gfn mCancelCursor;
    u8 mToTouch[0x24C - 0x240];
    gabi::gfn mUpdateTouch;
    u8 mToSound[4];
    gabi::gfn mPlaySound;
};
static hdBasicVtable_l* basicVtable(hdBasicButton_c* button) {
    return reinterpret_cast<hdBasicVtable_l*>(button->mButton.mVtable.get());
}
hdButtonEvent_l* hdBasic_copyEvent(hdButtonEvent_l*, hdButtonEvent_l*);
hdButtonEvent_l* hdBasic_constructEvent(hdButtonEvent_l*, u32, u32, void*);
void hdBasic_sendEvent(void*, hdButtonEvent_l*);
BOOL hdBasic_canDispatch(u32);
void hdBasic_pendingRelease();
static bool ownsTouch(hdBasicButton_c* button) {
    if (!hdBasic_inputEnabled() || !button->mButton.mTouchEnabled)
        return false;
    return hdBasic_getMode(sBasicManager) == button->mButton.mpReceiver.v;
}
static void requestButtonState(hdBasicButton_c* button, hdPartsStateId_c* state) {
    hdBasic_changeState(&button->mButton.mStateHolder.mMachine, state);
}
static bool dispatchEvent(hdBasicButton_c* button, hdButtonEvent_l& event) {
    if (!hdBasic_canDispatch(event.mType))
        return false;
    return gabi::call_ptr<BOOL>(basicVtable(button)->mDispatch.get(), button, static_cast<u32>(event.mType));
}
/* 026F0BF8 */
void hdBasicButton_c::endMove() {
    if (!sBasicManager || !hdBasic_inputEnabled() || !mButton.mTouchEnabled)
        return;
    hdButtonEvent_l event;
    hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
    if (event.mReceiverId == mButton.getReceiverId()) {
        if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsDecide.get(), this, static_cast<u32>(event.mType)))
            mButton.clearFocus();
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        if (hdBasic_getMode(sBasicManager) != mButton.mpReceiver.v)
            return;
        if (hdBasic_canDispatch(event.mType)) {
            if (gabi::call_ptr<BOOL>(basicVtable(this)->mDispatch.get(), this, static_cast<u32>(event.mType)))
                hdBasic_pendingRelease();
        } else
            mButton.handleTouchRelease();
    } else
        mButton.handleTouchRelease();
}
/* 026F0D30 */
void hdBasicButton_c::offMove() {
    if (!sBasicManager)
        return;
    hdButtonEvent_l event;
    hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
    if (event.mReceiverId == mButton.getReceiverId()) {
        if (!gabi::call_ptr<BOOL>(basicVtable(this)->mIsDecide.get(), this, static_cast<u32>(event.mType)))
            return;
        if (hdBasic_inputEnabled() && mButton.mTouchEnabled)
            mButton.clearFocus();
        gabi::call_ptr<void>(basicVtable(this)->mOnCursor.get(), this);
        requestButtonState(this, &sBasicSelectTouch);
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        if (!ownsTouch(this))
            return;
        if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsTouch.get(), this, &event)) {
            gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
            requestButtonState(this, &sBasicDecide);
        }
    } else if (ownsTouch(this)) {
        mButton.handleTouchRelease();
        requestButtonState(this, &sBasicSelectCursor);
    }
}
/* 026F0EC0 */
void hdBasicButton_c::onTouchMove() {
    if (!sBasicManager)
        return;
    hdButtonEvent_l event;
    hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
    if (event.mReceiverId == mButton.getReceiverId()) {
        if (!gabi::call_ptr<BOOL>(basicVtable(this)->mIsCancel.get(), this, static_cast<u32>(event.mType)))
            return;
        gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
        requestButtonState(this, &sBasicDecide);
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        if (!ownsTouch(this))
            return;
        if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsTouch.get(), this, &event)) {
            gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
            requestButtonState(this, &sBasicDecide);
        } else if (dispatchEvent(this, event)) {
            hdBasic_pendingRelease();
            requestButtonState(this, &sBasicUnSelect);
        }
    } else if (event.mReceiverId == 0) {
        if (hdBasic_inputEnabled() && mButton.mTouchEnabled)
            requestButtonState(this, &sBasicOnCursor);
        else
            requestButtonState(this, &sBasicUnSelect);
    }
}
/* 026F1088 */
void hdBasicButton_c::onCursorMove() {
    if (!sBasicManager)
        return;
    hdButtonEvent_l event;
    hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
    if (event.mReceiverId == mButton.getReceiverId()) {
        if (!gabi::call_ptr<BOOL>(basicVtable(this)->mIsCancel.get(), this, static_cast<u32>(event.mType)))
            return;
        gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
        requestButtonState(this, &sBasicDecide);
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        if (!ownsTouch(this))
            return;
        if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsTouch.get(), this, &event)) {
            gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
            requestButtonState(this, &sBasicDecide);
        } else if (dispatchEvent(this, event)) {
            hdBasic_pendingRelease();
            requestButtonState(this, &sBasicUnSelect);
        }
    } else if (!ownsTouch(this))
        requestButtonState(this, &sBasicUnSelect);
}
/* 026F1240 */
void hdBasicButton_c::selectTouchEnter() {
    hdBasic_play(mButton.mpLayout->animSet(), 0, 0, 1.0f);
    mSelectPhase = 0;
}
/* 026F128C */
void hdBasicButton_c::selectTouchMove() {
    if (!sBasicManager)
        return;
    if (mSelectPhase == 0) {
        hdButtonEvent_l event;
        hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
        if (event.mReceiverId == mButton.getReceiverId()) {
            if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsCancel.get(), this,
                                     static_cast<u32>(event.mType))) {
                gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
                mSelectPhase = 2;
            }
        } else if (event.mReceiverId == 0xFFFFFFFF) {
            if (ownsTouch(this)) {
                if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsTouch.get(), this, &event)) {
                    gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
                    mSelectPhase = 2;
                } else if (dispatchEvent(this, event)) {
                    hdBasic_pendingRelease();
                    mSelectPhase = 3;
                }
            }
        } else if (event.mReceiverId == 0)
            mSelectPhase = hdBasic_inputEnabled() && mButton.mTouchEnabled ? 1 : 3;
    }
    if (mSelectPhase == 2)
        mButton.synchronizeFocus();
    if (hdBasic_finished(mButton.mpLayout->animSet(), 0)) {
        hdPartsStateId_c* state = &sBasicOnTouch;
        switch (static_cast<u32>(mSelectPhase)) {
        case 1:
            state = &sBasicOnCursor;
            break;
        case 2:
            state = &sBasicDecide;
            break;
        case 3:
            state = &sBasicUnSelect;
            break;
        }
        requestButtonState(this, state);
    }
}
/* 026F14F0 */
void hdBasicButton_c::selectCursorMove() {
    if (!sBasicManager)
        return;
    if (mSelectPhase == 1) {
        hdButtonEvent_l event;
        hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
        if (event.mReceiverId == mButton.getReceiverId()) {
            if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsCancel.get(), this,
                                     static_cast<u32>(event.mType))) {
                gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
                mSelectPhase = 2;
            }
        } else if (event.mReceiverId == 0xFFFFFFFF) {
            if (ownsTouch(this)) {
                if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsTouch.get(), this, &event)) {
                    gabi::call_ptr<void>(basicVtable(this)->mPlaySound.get(), this);
                    mSelectPhase = 2;
                } else if (dispatchEvent(this, event)) {
                    hdBasic_pendingRelease();
                    mSelectPhase = 3;
                }
            }
        } else if (!ownsTouch(this))
            mSelectPhase = 3;
    }
    if (mSelectPhase == 2)
        mButton.synchronizeFocus();
    if (hdBasic_finished(mButton.mpLayout->animSet(), 0)) {
        hdPartsStateId_c* state = &sBasicOnCursor;
        if (mSelectPhase == 2)
            state = &sBasicDecide;
        else if (mSelectPhase == 3)
            state = &sBasicUnSelect;
        requestButtonState(this, state);
    }
}
static void finishDecision(hdBasicButton_c* button) {
    switch (static_cast<u32>(button->mRequestedState)) {
    case 2:
        button->decideEnd();
        return;
    case 1:
        if (ownsTouch(button))
            button->returnToTouch();
        else
            gabi::call_ptr<void>(basicVtable(button)->mCancelCursor.get(), button);
        return;
    default:
        requestButtonState(button, &sBasicEnd);
        return;
    }
}
/* 026F183C */
void hdBasicButton_c::decideMove() {
    if (!sBasicManager)
        return;
    mButton.synchronizeFocus();
    if (!hdBasic_finished(mButton.mpLayout->animSet(), 2))
        return;
    u32 receiver = mButton.getReceiverId();
    hdButtonEvent_l event;
    hdBasic_constructEvent(&event, HD_BUTTON_EVENT_SPECIAL_e, receiver, NULL);
    hdBasic_sendEvent(sBasicManager->mQueue, &event);
    finishDecision(this);
}
/* 026F2148 */
void hdBasicButton_c::forceDecide() {
    hdBasic_stop(mButton.mpLayout->animSet(), 2, 0);
    mButton.setPaneEnabled(1);
    finishDecision(this);
}
/* 026F1A2C */
void hdBasicButton_c::invalidMove() {
    if (!sBasicManager || !hdBasic_inputEnabled() || !mButton.mTouchEnabled)
        return;
    hdButtonEvent_l event;
    hdBasic_copyEvent(&event, &sBasicManager->mCurrentEvent);
    if (event.mReceiverId == mButton.getReceiverId()) {
        if (gabi::call_ptr<BOOL>(basicVtable(this)->mIsDecide.get(), this, static_cast<u32>(event.mType)))
            mButton.clearFocus();
    } else if (event.mReceiverId == 0xFFFFFFFF) {
        if (hdBasic_getMode(sBasicManager) != mButton.mpReceiver.v)
            return;
        if (hdBasic_canDispatch(event.mType)) {
            if (gabi::call_ptr<BOOL>(basicVtable(this)->mDispatch.get(), this, static_cast<u32>(event.mType)))
                hdBasic_pendingRelease();
        } else
            mButton.handleTouchRelease();
    } else
        mButton.handleTouchRelease();
}
static u32 sBasicReceiverTypeGuard;
static hdScreenType sBasicReceiverType;
static u32 sBasicIntermediateTypeGuard;
static hdScreenType sBasicIntermediateType;
static u32 sBasicBaseTypeGuard;
static hdScreenType sBasicBaseType;
/* 026F1D04 */
BOOL hdBasicButton_c::load(void* name, void* archive, void* heap, void* parent) {
    if (!gabi::call_ptr<BOOL>(basicVtable(this)->mLoad.get(), this, name, 0, archive, heap, parent))
        return FALSE;
    if (!gabi::call_ptr<BOOL>(basicVtable(this)->mCreate.get(), this))
        return FALSE;
    hdButtonPane_l* receiverType = hdButtonResources(mButton.mpLayout)->mpResources[0];
    if (!sBasicReceiverTypeGuard) {
        sBasicReceiverTypeGuard = 1;
        if (!sBasicIntermediateTypeGuard) {
            sBasicIntermediateTypeGuard = 1;
            if (!sBasicBaseTypeGuard) {
                sBasicBaseType.mParent = NULL;
                sBasicBaseTypeGuard = 1;
            }
            sBasicIntermediateType.mParent = &sBasicBaseType;
        }
        sBasicReceiverType.mParent = &sBasicIntermediateType;
    }
    bool matches = false;
    if (receiverType) {
        hdScreenType* type = gabi::call_ptr<hdScreenType*>(
            receiverType->mPane.mVtable->mGetRuntimeTypeInfo.get(), receiverType);
        while (type) {
            if (type == &sBasicReceiverType) {
                matches = true;
                break;
            }
            type = type->mParent;
        }
    }
    if (!matches)
        receiverType = NULL;
    hdPartsLayout_c* layout = mButton.mpLayout;
    u32 index = hdButtonResources(layout)->mResourceCount > 1 ? 1 : 0;
    hdButtonPane_l* resource = hdButtonResources(layout)->mpResources[index];
    if (!mButton.createReceiver(gabi::ea(receiverType), gabi::ea(resource)))
        return FALSE;
    mRequestedState = 0;
    mButton.mTouchEnabled = 1;
    hdAnimSet* animations = mButton.mpLayout->animSet();
    u32 animationIndex = animations->mEntries.mCount > 3 ? 3 : 0;
    void* animation = animations->mEntries.mData[animationIndex].get();
    hdStateOwner* owner = mButton.mStateHolder.mMachine.mpOwner;
    mAnimation = animation ? 3 : 0;
    hdStateObject* action =
        gabi::call_ptr<hdStateObject*>(owner->mpVTable->mCreateState.get(), owner, &sBasicOff);
    mButton.mStateHolder.mMachine.mpCurrentState = action;
    gabi::call_ptr<void>(action->mpVTable->mEnter.get(), action);
    return TRUE;
}
static bool isState(hdBasicButton_c* button, hdPartsStateId_c* wanted) {
    hdPartsStateId_c* current = hdBasic_currentState(&button->mButton.mStateHolder.mMachine);
    hdPartsStateIdVtbl_c* wantedVtable = wanted->mVtable;
    u32 currentId = stateId(current);
    return currentId == gabi::call_ptr<u32>(wantedVtable->mGetKey.get(), wanted);
}
/* 026F14A4 */
void hdBasicButton_c::touchEnter() {
    hdBasic_play(mButton.mpLayout->animSet(), 0, 0, 1.0f);
    mSelectPhase = 1;
}
/* 026F1C3C */
static hdBasicButton_c* hdBasicButton_c__ct(hdBasicButton_c* button) {
    if (!button) {
        button = static_cast<hdBasicButton_c*>(operator_new(sizeof(hdBasicButton_c)));
        if (!button)
            return NULL;
    }
    hdBasic_constructButton(&button->mButton);
    button->mButton.mVtable.v = gabi::ea(&sBasicVtable);
    button->mButton.mStateHolder.mHolderVtable = &sBasicPartVtable;
    button->mButton.mSecondaryVtable = &sBasicInterfaceVtable;
    button->mSelectPhase = 0;
    button->mRequestedState = 0;
    button->mAnimation = 3;
    hdUiSafeString* sound = gabi::at<hdUiSafeString>(gabi::ea(button) + offsetof(hdBasicButton_c, mSound));
    if (!sound)
        sound = static_cast<hdUiSafeString*>(operator_new(sizeof(hdUiSafeString)));
    if (sound) {
        sound->mVtable.v = gabi::ea(&sBasicStringVtable);
        sound->mText.v = gabi::guest_addr("");
    }
    button->mSoundId = 0;
    button->mSound.mText.v = sBasicSound.mText.v;
    return button;
}
/* 026F1F70 */
void hdBasicButton_c::update() {
    hdBasicVtable_l* vt = reinterpret_cast<hdBasicVtable_l*>(mButton.mVtable.get());
    gabi::call_ptr<void>(vt->mUpdateTouch.get(), this);
    hdBasic_updateState(&mButton.mStateHolder.mMachine);
    vt = reinterpret_cast<hdBasicVtable_l*>(mButton.mVtable.get());
    gabi::call_ptr<void>(vt->mUpdateReceiver.get(), this);
    vt = reinterpret_cast<hdBasicVtable_l*>(mButton.mVtable.get());
    gabi::call_ptr<void>(vt->mUpdateLayout.get(), this);
}
/* 026F1FD8 */
void hdBasicButton_c::registerReceiver() {
    if (!isState(this, &sBasicDisableWait) && !isState(this, &sBasicIn) && !isState(this, &sBasicOut))
        mButton.registerReceiver();
}
/* 026F20F0 */
void hdBasicButton_c::cancelCursor() {
    hdBasic_stop(mButton.mpLayout->animSet(), 1, 0);
    mButton.setPaneEnabled(1);
    hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOff);
}
/* 026F2550 */
BOOL hdBasicButton_c::loadLayout(void* name, void* archive, void* heap, void* parent, u32 arg) {
    hdPartsLayout_c* layout =
        static_cast<hdPartsLayout_c*>(hdBasic_allocate(0x110, sBasicHeapManager->mHeap, 4));
    if (layout)
        layout = hdBasic_constructLayout(layout);
    mButton.mpLayout = layout;
    if (!layout)
        return FALSE;
    gabi::call_ptr<void>(layout->vtbl()->mLoad.get(), layout, name, archive, heap, parent, 7, 2, arg);
    return TRUE;
}
/* 026F2604 */
void hdBasicButton_c::updatePaneFlags() {
    if (isState(this, &sBasicSelectTouch) || isState(this, &sBasicSelectCursor) ||
        isState(this, &sBasicUnSelect) || isState(this, &sBasicDecide) || isState(this, &sBasicCancel) ||
        isState(this, &sBasicIn) || isState(this, &sBasicOut)) {
        mButton.setPaneEnabled(1);
        mButton.setTextEnabled(1);
    }
}
/* 026F284C */
void hdBasicButton_c::updateTouchScale() {
    if (!hdBasic_inputEnabled() || !mButton.mTouchEnabled)
        return;
    hdButtonReceiver_l* receiver = gabi::at<hdButtonReceiver_l>(hdBasic_getMode(sBasicManager));
    if (!receiver)
        return;
    u32 id = mButton.getReceiverId();
    if (receiver->mId != id)
        return;
    if (isState(this, &sBasicEnd) || isState(this, &sBasicUnSelect))
        hdBasic_setScale(sBasicScaleManager, &sBasicDefaultScale);
    else
        hdBasic_setScale(sBasicScaleManager, &sBasicTouchScale);
}
/* 026F2980 */
void hdBasicButton_c::playSound() {
    if (*mSound.mText) {
        gabi::call_ptr<void>(mSound.mVtable->mAssureTermination.get(), &mSound);
        hdBasic_soundName(mSound.mText);
    }
    if (mSoundId)
        hdBasic_soundId(mSoundId);
}
static hdHeartInitLists_l sBasicInitLists;
static hdHeartInitAngles_l sBasicInitAngles;
static hdHeartAngleLimits_l sBasicAngleLimits;
static hdHeartInitRecord_l sBasicFirstRecord;
static hdHeartInitRecord_l sBasicSecondRecord;
static hdHeartInitRecord_l sBasicThirdRecord;
static hdUiSafeString sBasicClassName;
static u32 sBasicStateGuard;
static u32 sBasicStateCounter;
static hdPartsStateIdVtbl_c sBasicStateVtable;
static hdStateId sBasicStateContext;
void hdBasic_registerStatic(hdHeartInitRecord_l*);
void hdBasic_initFlag(be<u8>*);
void hdBasic_initByte(be<u8>*);
static void initializeBasicState(hdPartsStateId_c& state, u32 id, const char* name, u32 firstSlot) {
    state.mNumber = id;
    state.mName.v = gabi::guest_addr(name);
    state.mVtable = &sBasicStateVtable;
    state.mEnter.mThisDelta = 0;
    state.mEnter.mVirtualIndex = firstSlot;
    state.mEnter.mFunction = 4;
    state.mExecute.mThisDelta = 0;
    state.mExecute.mVirtualIndex = firstSlot + 1;
    state.mExecute.mFunction = 4;
    state.mExit.mThisDelta = 0;
    state.mExit.mVirtualIndex = firstSlot + 2;
    state.mExit.mFunction = 4;
    state.mUnused.mThisDelta = 0;
    state.mUnused.mVirtualIndex = 0;
    state.mUnused.mFunction = 0;
    state.mpNullStateId = &sBasicStateContext;
}
/* 026F29E4 */
static void hdBasic_staticInit() {
    sBasicInitLists.mFourth = 0;
    sBasicInitLists.mThird = 0;
    sBasicInitLists.mSecond = 0;
    sBasicInitLists.mFirst = 0;
    hdBasic_registerStatic(&sBasicFirstRecord);
    f32 minimum = sBasicAngleLimits.mMinimum, maximum = sBasicAngleLimits.mMaximum;
    sBasicInitAngles.mMinimum = minimum;
    sBasicInitAngles.mMaximum = maximum;
    hdBasic_initFlag(&sBasicInitAngles.mFlag);
    hdBasic_registerStatic(&sBasicSecondRecord);
    hdBasic_initByte(&sBasicInitAngles.mByte);
    hdBasic_registerStatic(&sBasicThirdRecord);
    sBasicClassName.mVtable.v = gabi::ea(&sBasicStringVtable);
    sBasicClassName.mText.v = gabi::guest_addr("BasicButton");
    sBasicSound.mText.v = gabi::guest_addr("SE_MENU_DECIDE");
    sBasicSound.mVtable.v = gabi::ea(&sBasicStringVtable);
    u32 id;
    if (sBasicStateGuard)
        id = sBasicStateCounter;
    else {
        sBasicStateGuard = 1;
        id = 0;
    }
    initializeBasicState(sBasicEnd, ++id, "::StateID_End", 35);
    initializeBasicState(sBasicOff, ++id, "::StateID_Off", 38);
    initializeBasicState(sBasicOnTouch, ++id, "::StateID_OnTouch", 41);
    initializeBasicState(sBasicOnCursor, ++id, "::StateID_OnCursor", 44);
    initializeBasicState(sBasicSelectTouch, ++id, "::StateID_SelectTouch", 47);
    initializeBasicState(sBasicSelectCursor, ++id, "::StateID_SelectCursor", 50);
    initializeBasicState(sBasicUnSelect, ++id, "::StateID_UnSelect", 53);
    initializeBasicState(sBasicDecide, ++id, "::StateID_Decide", 56);
    initializeBasicState(sBasicCancel, ++id, "::StateID_Cancel", 59);
    initializeBasicState(sBasicInvalid, ++id, "::StateID_Invalid", 62);
    initializeBasicState(sBasicIn, ++id, "::StateID_In", 65);
    initializeBasicState(sBasicOut, ++id, "::StateID_Out", 68);
    sBasicStateCounter = id;
}
/* 026F0A8C */
static be<u32>* hdBasic_getObject(gptr<be<u32>>* object) {
    return *object;
}
/* 026F0B8C */
void hdBasicButton_c::destroy(hdBasicButton_c* part, u32 flags) {
    if (!part)
        return;
    part->mButton.mStateHolder.mHolderVtable = &sBasicPartVtable;
    part->mButton.mSecondaryVtable = &sBasicInterfaceVtable;
    hdButtonPart_c::destroy(&part->mButton, 0);
    if (flags & 1)
        operator_delete(part);
}
/* 026F1708 */
void hdBasicButton_c::cursorEnter() {
    hdBasic_play(mButton.mpLayout->animSet(), 1, 0, 1.0f);
}
/* 026F1724 */
void hdBasicButton_c::cursorMove() {
    if (hdBasic_finished(mButton.mpLayout->animSet(), 1))
        hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOff);
}
/* 026F1774 */
void hdBasicButton_c::decideEnter() {
    mButton.synchronizeFocus();
    hdBasic_play(mButton.mpLayout->animSet(), 2, 0, 1.0f);
}
/* 026F17BC */
void hdBasicButton_c::decideEnd() {
    hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicCancel);
}
/* 026F17CC */
void hdBasicButton_c::returnToTouch() {
    if (hdBasic_inputEnabled() && mButton.mTouchEnabled) {
        hdBasic_stop(mButton.mpLayout->animSet(), 0, 0);
        mButton.setPaneEnabled(1);
        hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOnCursor);
    }
}
/* 026F19B0 */
void hdBasicButton_c::animationEnter() {
    u32 animation = mAnimation;
    void* controller = mButton.mpLayout->animSet();
    hdBasic_play(controller, animation, 0, animation ? 1.0f : -1.0f);
}
/* 026F19DC */
void hdBasicButton_c::animationMove() {
    if (hdBasic_finished(mButton.mpLayout->animSet(), mAnimation))
        hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOff);
}
/* 026F1B64 */
void hdBasicButton_c::disableEnter() {
    hdBasic_play(mButton.mpLayout->animSet(), 5, 1, 1.0f);
}
/* 026F1B80 */
void hdBasicButton_c::disableMove() {
    if (hdBasic_finished(mButton.mpLayout->animSet(), 5))
        hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOff);
}
/* 026F1BD0 */
void hdBasicButton_c::enableEnter() {
    hdBasic_play(mButton.mpLayout->animSet(), 6, 1, 1.0f);
}
/* 026F1BEC */
void hdBasicButton_c::enableMove() {
    if (hdBasic_finished(mButton.mpLayout->animSet(), 6))
        hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicDisableWait);
}
/* 026F1F3C */
void hdBasicButton_c::unload() {
    mButton.destroyReceiver();
    hdBasic_unloadBase(this);
}
/* 026F226C */
void hdBasicButton_c::resetAnimation() {
    u32 animation = mAnimation;
    void* controller = mButton.mpLayout->animSet();
    if (animation)
        hdBasic_stop(controller, animation, 0);
    else
        hdBasic_pause(controller, 0, 0);
    mButton.setPaneEnabled(1);
    hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOff);
}
/* 026F22F8 */
void hdBasicButton_c::setEnabled(u32 enabled) {
    void* controller = mButton.mpLayout->animSet();
    if (enabled)
        hdBasic_pause(controller, 4, 0);
    else
        hdBasic_stop(controller, 4, 0);
    mButton.setPaneEnabled(1);
    hdBasic_changeState(&mButton.mStateHolder.mMachine, enabled ? &sBasicOff : &sBasicInvalid);
}
/* 026F237C */
void hdBasicButton_c::resetAndDisable() {
    u32 animation = mAnimation;
    void* controller = mButton.mpLayout->animSet();
    if (animation)
        hdBasic_stop(controller, animation, 0);
    else
        hdBasic_pause(controller, 0, 0);
    hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicIn);
}
/* 026F23F0 */
void hdBasicButton_c::hide() {
    hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicOut);
}
/* 026F2400 */
void hdBasicButton_c::show() {
    hdBasic_pause(mButton.mpLayout->animSet(), 5, 1);
    mButton.setPaneEnabled(1);
    hdBasic_changeState(&mButton.mStateHolder.mMachine, &sBasicDisableWait);
}
/* 026F2458 */
BOOL hdBasicButton_c::isEnabled() {
    hdPartsStateId_c* current = hdBasic_currentState(&mButton.mStateHolder.mMachine);
    hdPartsStateIdVtbl_c* otherVtable = sBasicInvalid.mVtable;
    u32 currentId = stateId(current);
    u32 otherId = gabi::call_ptr<u32>(otherVtable->mGetKey.get(), &sBasicInvalid);
    return currentId != otherId;
}
/* 026F24D0 */
BOOL hdBasicButton_c::isHidden() {
    hdPartsStateId_c* current = hdBasic_currentState(&mButton.mStateHolder.mMachine);
    hdPartsStateIdVtbl_c* otherVtable = sBasicIn.mVtable;
    u32 currentId = stateId(current);
    u32 otherId = gabi::call_ptr<u32>(otherVtable->mGetKey.get(), &sBasicIn);
    return currentId == otherId;
}
/* 026F2540 */
void hdBasicButton_c::setSound(hdUiSafeString* name, u32 id) {
    u32 text = name->mText.v;
    mSoundId = id;
    mSound.mText.v = text;
}
/* WWHD glue
 data sBasicReceiverTypeGuard 101FD938
 data sBasicReceiverType 101FD93C
 data sBasicIntermediateTypeGuard 101FD88C
 data sBasicIntermediateType 101FD940
 data sBasicBaseTypeGuard 101FD888
 data sBasicBaseType 101FD8FC

 ext 020013C0 hdButtonEvent_l* hdBasic_copyEvent(hdButtonEvent_l*,hdButtonEvent_l*)
 ext 02001338 hdButtonEvent_l* hdBasic_constructEvent(hdButtonEvent_l*,u32,u32,void*)
 ext 02001FF4 void hdBasic_sendEvent(void*,hdButtonEvent_l*)
 ext 026F621C BOOL hdBasic_canDispatch(u32)
 ext 026F69A4 void hdBasic_pendingRelease()

 data sBasicInitLists 1049D7F0
 data sBasicInitAngles 1049D7E4
 data sBasicAngleLimits 101095DC
 data sBasicFirstRecord 101F7108
 data sBasicSecondRecord 101F7114
 data sBasicThirdRecord 101F7120
 data sBasicClassName 1049DA40
 data sBasicStateGuard 101FD8E4
 data sBasicStateCounter 101FDD50
 data sBasicStateVtable 1010957C
 data sBasicStateContext 101FF32C
 ext 028F026C void hdBasic_registerStatic(hdHeartInitRecord_l*)
 ext 028ED6F8 void hdBasic_initFlag(be<u8>*)
 ext 028EAB2C void hdBasic_initByte(be<u8>*)
 str 101095E4 "BasicButton"
 str 10109660 "SE_MENU_DECIDE"
 str 101095F0 "::StateID_End"
 str 10109600 "::StateID_Off"
 str 10109610 "::StateID_OnTouch"
 str 10109670 "::StateID_OnCursor"
 str 10109624 "::StateID_SelectTouch"
 str 10109684 "::StateID_SelectCursor"
 str 1010969C "::StateID_UnSelect"
 str 101096B0 "::StateID_Decide"
 str 101096C4 "::StateID_Cancel"
 str 1010963C "::StateID_Invalid"
 str 101096D8 "::StateID_In"
 str 10109650 "::StateID_Out"

 data sBasicOnTouch 1049D860
 data sBasicDecide 1049D950
 data sBasicEnd 1049D800
 data sBasicSelectTouch 1049D8C0
 data sBasicSelectCursor 1049D8F0
 data sBasicUnSelect 1049D920
 data sBasicVtable 101096E8
 data sBasicStringVtable 1010951C
 data sBasicSound 1049D7DC
 data sBasicManager 101F4FF0
 data sBasicScaleManager 101F5A44
 data sBasicHeapManager 1018C404
 data sBasicDefaultScale 104920F0
 data sBasicTouchScale 1049DA90
 str 10000160 ""
 ext 02006364 void hdBasic_updateState(hdStateMachine*)
 ext 026F5A60 hdButtonPart_c* hdBasic_constructButton(hdButtonPart_c*)
 ext 02615140 u32 hdBasic_getMode(hdButtonEventManager_l*)
 ext 026538F4 void hdBasic_setScale(void*,be<f32>*)
 ext 02030B38 void hdBasic_soundName(be<u8>*)
 ext 025E1988 void hdBasic_soundId(u32)
 ext 0273B050 void* hdBasic_allocate(u32,void*,u32)
 ext 026FADB0 hdPartsLayout_c* hdBasic_constructLayout(hdPartsLayout_c*)

 data sBasicPartVtable 10109940
 data sBasicInterfaceVtable 10109950
 data sBasicOff 1049D830
 data sBasicCancel 1049D980
 data sBasicOnCursor 1049D890
 data sBasicInvalid 1049D9B0
 data sBasicIn 1049D9E0
 data sBasicOut 1049DA10
 data sBasicDisableWait 1049DD60
 ext 020063C0 void hdBasic_changeState(hdStateMachine*,hdPartsStateId_c*)
 ext 02006478 hdPartsStateId_c* hdBasic_currentState(hdStateMachine*)
 ext 020053E4 void hdBasic_play(void*,u32,u32,f32)
 ext 02005840 BOOL hdBasic_finished(void*,u32)
 ext 02005708 void hdBasic_stop(void*,u32,u32)
 ext 02005674 void hdBasic_pause(void*,u32,u32)
 ext 026F67A4 BOOL hdBasic_inputEnabled()
 ext 026F8B00 void hdBasic_unloadBase(hdBasicButton_c*)
*/
