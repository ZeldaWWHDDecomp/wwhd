/**
 * hd_screen_btn_menu_arrow.cpp
 * HD - the pause menu's page arrows "BtnMenuArrow_00": BasicButton parts decided by the menu button too,
 * pressed by their direction key, with their own input repeat while held; states ::StateID_SelectTouch /
 * _SelectCursor / _Decide (refining BasicButton's)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_btn_menu). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, group and state names are HD strings;
 * all other names are ours.
 */

#include "d/hd_screen_btn_menu.h"

static u32 sArrowVtable;
static u32 sArrowHolderVtable;
static u32 sArrowSecondaryVtable;
static hdUiStringTable sArrowSafeStringVtable;
static hdPartsStateIdVtbl_c sArrowStateIdVtable;
static hdButtonEventManager_l* sArrowEventManager;
static hdInputManager* sArrowInput;
/* BasicButton's states (hd_parts_basic_button.cpp) */
static hdPartsStateId_c sArrowBasicEnd;
static hdPartsStateId_c sArrowBasicOnTouch;
static hdPartsStateId_c sArrowBasicOnCursor;
static hdPartsStateId_c sArrowBasicUnSelect;
static hdPartsStateId_c sArrowBasicSelectTouch;
static hdPartsStateId_c sArrowBasicSelectCursor;
static hdPartsStateId_c sArrowBasicDecide;
static const u32 sArrowDirectionMasks[4] = {0x110000, 0x220000, 0x440000, 0x880000};
static const u8 sArrowDirectionButtons[4] = {5, 8, 11, 12};

static hdUiSafeString sArrowGroupName; /* "G_Btn_00" (not used by this code) */
static hdPartsStateId_c sArrowSelectTouch;
static hdPartsStateId_c sArrowSelectCursor;
static hdPartsStateId_c sArrowDecide;
static hdUiSafeString sArrowLayoutName;
static u32 sArrowStateNumberReady;
static s32 sArrowStateNumber;

/* 0263CC5C */
// hdScreenBtnMenuArrow_c::~hdScreenBtnMenuArrow_c() (GHS deleting destructor)
void hdScreenBtnMenuArrow_c::destroy(hdScreenBtnMenuArrow_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mButton.mStateHolder.mHolderVtable = &sArrowHolderVtable;
    i_this->mButton.mSecondaryVtable = &sArrowSecondaryVtable;
    hdBasicButton_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0263CCC8 */
void hdScreenBtnMenuArrow_c::selectTouchEnter() {
    hdAnimSet_play(part()->animSet(), 0, 0, 1.0f);
    mSelectPhase = 0;
}

/* 0263CD14 */
// selected by touch: this button's decide (or a tap on it) decides; a touch elsewhere unselects it; the
// cursor taking over selects it by cursor; once the animation ends, the next state
void hdScreenBtnMenuArrow_c::selectTouchMove() {
    if (sArrowEventManager == NULL) {
        return;
    }
    if (mSelectPhase == 0) {
        gabi::Local<hdButtonEvent_l> event;
        hdButtonEvent_copy_l(event, &sArrowEventManager->mCurrentEvent);
        if (event->mReceiverId == part()->getReceiverId()) {
            if (gabi::call_ptr<BOOL>(vtbl()->mIsCancel.get(), this, (u32)event->mType)) {
                gabi::call_ptr(vtbl()->mPlaySound.get(), this);
                part()->changeState(&sArrowDecide);
                return;
            }
        } else if (event->mReceiverId == 0xFFFFFFFF) {
            if (hdButton_inputEnabled_l() && mButton.mTouchEnabled &&
                hdButtonEventManager_getActiveReceiver_l(sArrowEventManager) == mButton.mpReceiver.v) {
                if (gabi::call_ptr<BOOL>(vtbl()->mIsTouch.get(), this, (hdButtonEvent_l*)event)) {
                    gabi::call_ptr(vtbl()->mPlaySound.get(), this);
                    part()->changeState(&sArrowDecide);
                    return;
                }
                if (hdButton_canDispatch_l(event->mType) && gabi::call_ptr<BOOL>(vtbl()->mDispatch.get(), this, (u32)event->mType)) {
                    hdButton_markTouchRelease_l();
                    mSelectPhase = 3;
                }
            }
        } else if (event->mReceiverId == 0) {
            if (hdButton_inputEnabled_l() && mButton.mTouchEnabled) {
                mSelectPhase = 1;
            } else {
                mSelectPhase = 3;
            }
        }
    }
    if (hdAnimSet_isEnd(part()->animSet(), 0)) {
        u32 phase = mSelectPhase;
        if (phase == 1) {
            part()->changeState(&sArrowBasicOnCursor);
        } else if (phase == 3) {
            part()->changeState(&sArrowBasicUnSelect);
        } else {
            part()->changeState(&sArrowBasicOnTouch);
        }
    }
}

/* 0263CF30 */
void hdScreenBtnMenuArrow_c::selectCursorEnter() {
    hdAnimSet_play(part()->animSet(), 0, 0, 1.0f);
    mSelectPhase = 1;
}

/* 0263CF7C */
// selected by cursor: like selected by touch; losing the input unselects it
void hdScreenBtnMenuArrow_c::selectCursorMove() {
    if (sArrowEventManager == NULL) {
        return;
    }
    if (mSelectPhase == 1) {
        gabi::Local<hdButtonEvent_l> event;
        hdButtonEvent_copy_l(event, &sArrowEventManager->mCurrentEvent);
        if (event->mReceiverId == part()->getReceiverId()) {
            if (gabi::call_ptr<BOOL>(vtbl()->mIsCancel.get(), this, (u32)event->mType)) {
                gabi::call_ptr(vtbl()->mPlaySound.get(), this);
                part()->changeState(&sArrowDecide);
                return;
            }
        } else if (event->mReceiverId == 0xFFFFFFFF) {
            if (hdButton_inputEnabled_l() && mButton.mTouchEnabled &&
                hdButtonEventManager_getActiveReceiver_l(sArrowEventManager) == mButton.mpReceiver.v) {
                if (gabi::call_ptr<BOOL>(vtbl()->mIsTouch.get(), this, (hdButtonEvent_l*)event)) {
                    gabi::call_ptr(vtbl()->mPlaySound.get(), this);
                    part()->changeState(&sArrowDecide);
                    return;
                }
                if (hdButton_canDispatch_l(event->mType) && gabi::call_ptr<BOOL>(vtbl()->mDispatch.get(), this, (u32)event->mType)) {
                    hdButton_markTouchRelease_l();
                    mSelectPhase = 3;
                }
            }
        } else if (!hdButton_inputEnabled_l() || !mButton.mTouchEnabled ||
                   hdButtonEventManager_getActiveReceiver_l(sArrowEventManager) != mButton.mpReceiver.v) {
            mSelectPhase = 3;
        }
    }
    if (hdAnimSet_isEnd(part()->animSet(), 0)) {
        hdPartsStateId_c* next = &sArrowBasicUnSelect;
        if (mSelectPhase != 3) {
            next = &sArrowBasicOnCursor;
        }
        part()->changeState(next);
    }
}

/* 0263D16C */
void hdScreenBtnMenuArrow_c::decideEnter() {
    part()->synchronizeFocus();
    hdAnimSet_play(part()->animSet(), BTN_MENU_ANIM_DECIDE, 0, 1.0f);
}

/* 0263D1B4 */
// decided: another decide on this button (or a tap) decides again; after the animation, back to the touch
// or cursor selection, or done
void hdScreenBtnMenuArrow_c::decideMove() {
    if (sArrowEventManager == NULL) {
        return;
    }
    part()->synchronizeFocus();
    gabi::Local<hdButtonEvent_l> event;
    hdButtonEvent_copy_l(event, &sArrowEventManager->mCurrentEvent);
    if (event->mReceiverId == part()->getReceiverId()) {
        if (gabi::call_ptr<BOOL>(vtbl()->mIsDecide.get(), this, (u32)event->mType)) {
            if (hdButton_inputEnabled_l() && mButton.mTouchEnabled) {
                part()->clearFocus();
            }
            gabi::call_ptr(vtbl()->mPlayTouchSound.get(), this);
            part()->changeState(&sArrowSelectTouch);
            return;
        }
    } else if (event->mReceiverId == 0xFFFFFFFF && hdButton_inputEnabled_l() && mButton.mTouchEnabled &&
               hdButtonEventManager_getActiveReceiver_l(sArrowEventManager) == mButton.mpReceiver.v &&
               gabi::call_ptr<BOOL>(vtbl()->mIsTouch.get(), this, (hdButtonEvent_l*)event)) {
        gabi::call_ptr(vtbl()->mPlaySound.get(), this);
        part()->changeState(&sArrowDecide);
        return;
    }
    if (!hdAnimSet_isEnd(part()->animSet(), BTN_MENU_ANIM_DECIDE)) {
        return;
    }
    u32 requested = mRequestedState;
    if (requested == BTN_MENU_SELECT_CURSOR) {
        if (hdButton_inputEnabled_l() && mButton.mTouchEnabled &&
            hdButtonEventManager_getActiveReceiver_l(sArrowEventManager) == mButton.mpReceiver.v) {
            returnToTouch();
            return;
        }
        gabi::call_ptr(vtbl()->mCancelCursor.get(), this);
    } else if (requested == BTN_MENU_SELECT_TOUCH) {
        decideEnd();
    } else {
        part()->changeState(&sArrowBasicEnd);
    }
}

/* 0263D438 */
// hdScreenBtnMenuArrow_c::hdScreenBtnMenuArrow_c() (GHS: allocates when called with NULL)
hdScreenBtnMenuArrow_c* hdScreenBtnMenuArrow_c::construct(hdScreenBtnMenuArrow_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenBtnMenuArrow_c*)operator_new(sizeof(hdScreenBtnMenuArrow_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdBasicButton_construct_l(i_this);
    i_this->mRepeating = FALSE;
    i_this->mButton.mVtable = (hdPartsVtbl_c*)(void*)&sArrowVtable;
    i_this->mButton.mStateHolder.mHolderVtable = &sArrowHolderVtable;
    i_this->mRepeatRate = 1;
    i_this->mRepeatDelay = 30;
    i_this->mDirectionKeys = FALSE;
    i_this->mButton.mSecondaryVtable = &sArrowSecondaryVtable;
    i_this->mDirection = 4;
    return i_this;
}

/* 0263D4C8 */
BOOL hdScreenBtnMenuArrow_c::load(void* archive, void* heap, void* par) {
    if (!hdBasicButton_c::load(&sArrowLayoutName, archive, heap, par)) {
        return FALSE;
    }
    mDirectionKeys = FALSE;
    mDirection = 4;
    mRepeatDelay = 30;
    mRepeatRate = 1;
    return TRUE;
}

/* 0263D54C */
// the input repeat back to its normal timing
void hdScreenBtnMenuArrow_c::restoreRepeat() {
    sArrowInput->pointerSlow(BTN_MENU_REPEAT_BUTTONS);
    if (mDirectionKeys) {
        sArrowInput->pointerDefault();
    }
}

/* 0263D5A4 */
void hdScreenBtnMenuArrow_c::unload() {
    if (mRepeating) {
        mRepeating = FALSE;
        restoreRepeat();
    }
    hdBasicButton_c::unload();
}

/* 0263D5F0 */
u32 hdScreenBtnMenuArrow_c::getDirectionMask(u32 direction) {
    if (direction > 3) {
        return 0;
    }
    return sArrowDirectionMasks[direction];
}

/* 0263D614 */
// held: this button has the input, or (with the input) the decide button or its direction key is held
BOOL hdScreenBtnMenuArrow_c::isHeld() {
    gabi::Local<hdButtonEvent_l> event;
    hdButtonEvent_copy_l(event, &sArrowEventManager->mCurrentEvent);
    if (event->mReceiverId == part()->getReceiverId()) {
        return TRUE;
    }
    if (hdButtonEventManager_getActiveReceiver_l(sArrowEventManager) != mButton.mpReceiver.v) {
        return FALSE;
    }
    u32 held = ((hdBtnMenuInput_l*)(hdInputManager*)sArrowInput)->mHeld;
    if (held & BTN_MENU_HELD_DECIDE) {
        return TRUE;
    }
    if (!mDirectionKeys) {
        return FALSE;
    }
    u32 mask = getDirectionMask(mDirection);
    if (mask == 0 || !(mask & held)) {
        return FALSE;
    }
    return TRUE;
}

/* 0263D6DC */
// the arrow's input repeat: its delay and rate for the decide buttons (and its direction keys)
void hdScreenBtnMenuArrow_c::setRepeat() {
    sArrowInput->pointerParamsMasked(BTN_MENU_REPEAT_BUTTONS, mRepeatDelay, mRepeatRate);
    if (mDirectionKeys) {
        sArrowInput->pointerParams(mRepeatDelay, mRepeatRate);
    }
}

/* 0263D744 */
void hdScreenBtnMenuArrow_c::updateRepeat() {
    BOOL held = isHeld();
    if (held) {
        if (!mRepeating) {
            mRepeating = TRUE;
            setRepeat();
        }
    } else if (mRepeating) {
        mRepeating = FALSE;
        restoreRepeat();
    }
}

/* 0263D7C0 */
void hdScreenBtnMenuArrow_c::update() {
    hdBasicButton_c::update();
    updateRepeat();
}

/* 0263D7F4 */
// the menu button also decides an arrow
BOOL hdScreenBtnMenuArrow_c::isDecide(u32 event) {
    bool decide = true;
    if (!hdButton_isMenu_l(event) && !part()->isDecideEvent(event)) {
        decide = false;
    }
    return decide;
}

/* 0263D860 */
BOOL hdScreenBtnMenuArrow_c::isCancel(u32 event) {
    bool cancel = true;
    if (!hdButton_isMenu_l(event) && !part()->isCancelEvent(event)) {
        cancel = false;
    }
    return cancel;
}

/* 0263D8CC */
u32 hdScreenBtnMenuArrow_c::getDirectionButton(u32 direction) {
    if (direction > 3) {
        return 0;
    }
    return sArrowDirectionButtons[direction];
}

/* 0263D8E8 */
// pressed: its direction key, a touch of an accepted button, or a touch release on it
BOOL hdScreenBtnMenuArrow_c::isTouch(hdButtonEvent_l* event) {
    bool direction = false;
    if (mDirectionKeys) {
        u32 button = getDirectionButton(mDirection);
        // HD: the type is read here, before hasAcceptedButton (the original's order)
        direction = u32(event->mType) == button;
    }
    bool accepted = part()->hasAcceptedButton(event->mButton);
    bool touch = event->mType == HD_BUTTON_EVENT_TOUCH_e;
    bool tap = accepted && touch;
    bool pressed = true;
    if (!(direction || tap) && !part()->isTouchEndEvent(event)) {
        pressed = false;
    }
    return pressed;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0263D9B0 */
static void __sinit_hd_screen_btn_menu_arrow_cpp() {
    sinit_header_statics_rodata(0x104906BC, 0x101F561C, 0x104906C8, 0x100EA7A8);
    hdParts_setSafeString(&sArrowGroupName, &sArrowSafeStringVtable, "G_Btn_00");
    s32 number;
    if (sArrowStateNumberReady) {
        number = sArrowStateNumber;
    } else {
        number = 0;
        sArrowStateNumberReady = 1;
    }
    sArrowSelectTouch.setup(number + 1, "::StateID_SelectTouch", &sArrowStateIdVtable, 0x2F, (hdStateId*)&sArrowBasicSelectTouch);
    sArrowSelectCursor.setup(number + 2, "::StateID_SelectCursor", &sArrowStateIdVtable, 0x32, (hdStateId*)&sArrowBasicSelectCursor);
    sArrowDecide.setup(number + 3, "::StateID_Decide", &sArrowStateIdVtable, 0x38, (hdStateId*)&sArrowBasicDecide);
    sArrowStateNumber = number + 3;
    hdParts_setSafeString(&sArrowLayoutName, &sArrowSafeStringVtable, "BtnMenuArrow_00");
}

/* 0263CB5C */
static s32 hdScreenBtnMenuArrow_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sArrowVtable 100EA810 nocheck
data sArrowHolderVtable 100EAA68 nocheck
data sArrowSecondaryVtable 100EAA78 nocheck
data sArrowSafeStringVtable 100EA6E4 nocheck
data sArrowStateIdVtable 100EA744 nocheck
data sArrowEventManager 101F4FF0 nocheck
data sArrowInput 101F5088 nocheck
data sArrowBasicEnd 1049D800 nocheck
data sArrowBasicOnTouch 1049D860 nocheck
data sArrowBasicOnCursor 1049D890 nocheck
data sArrowBasicUnSelect 1049D920 nocheck
data sArrowBasicSelectTouch 1049D8C0 nocheck
data sArrowBasicSelectCursor 1049D8F0 nocheck
data sArrowBasicDecide 1049D950 nocheck
data sArrowDirectionMasks 100EA794
data sArrowDirectionButtons 100EA7A4
data sArrowGroupName 104906B4 nocheck
data sArrowSelectTouch 104906D8 nocheck
data sArrowSelectCursor 10490708 nocheck
data sArrowDecide 10490738 nocheck
data sArrowLayoutName 10490768 nocheck
data sArrowStateNumberReady 101FD8E4 nocheck
data sArrowStateNumber 101FDD50 nocheck
str 100EA7D8 "G_Btn_00" in 0263D9B0
str 100EA7C0 "::StateID_SelectTouch" in 0263D9B0
str 100EA7E4 "::StateID_SelectCursor" in 0263D9B0
str 100EA7FC "::StateID_Decide" in 0263D9B0
str 100EA7B0 "BtnMenuArrow_00" in 0263D9B0
ext 026F1F3C void hdBasicButton_c::unload()
ext 026F67B4 BOOL hdButtonPart_c::isDecideEvent(u32)
ext 026F67BC BOOL hdButtonPart_c::isCancelEvent(u32)
ext 026F67C4 BOOL hdButtonPart_c::hasAcceptedButton(u32)
ext 026F6880 BOOL hdButtonPart_c::isTouchEndEvent(hdButtonEvent_l*)
ext 02618760 void hdInputManager::pointerParams(u32, u32)
ext 02618774 void hdInputManager::pointerDefault()
ext 02618780 void hdInputManager::pointerParamsMasked(u32, u32, u32)
ext 0261878C void hdInputManager::pointerSlow(u32)
*/
