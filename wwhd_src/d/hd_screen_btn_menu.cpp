/**
 * hd_screen_btn_menu.cpp
 * HD - the pause menu's tab buttons "BtnMenu_00": BasicButton parts with their own decide and cancel
 * animations; the Miiverse button also shows a "new" mark with the number of unread comments; states
 * ::StateID_Decide / _Cancel (refining BasicButton's)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_btn_menu). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation, sound and state names
 * are HD strings; all other names are ours.
 */

#include "d/hd_screen_btn_menu.h"

static u32 sBtnMenuVtable;
static u32 sBtnMenuHolderVtable;
static u32 sBtnMenuSecondaryVtable;
static hdUiStringTable sBtnMenuSafeStringVtable;
static hdPartsStateIdVtbl_c sBtnMenuStateIdVtable;
static u32 sBtnMenuTextVtable;
static hdScreenUiSystem* sBtnMenuUiSystem;
static hdButtonEventManager_l* sBtnMenuEventManager;
static void* sBtnMenuComments;                     /* the Miiverse comment manager */
static hdBtnMenuController_l* sBtnMenuController;
/* BasicButton's states (hd_parts_basic_button.cpp) */
static hdPartsStateId_c sBtnMenuBasicEnd;
static hdPartsStateId_c sBtnMenuBasicOff;
static hdPartsStateId_c sBtnMenuBasicSelectTouch;
static hdPartsStateId_c sBtnMenuBasicDecide;
static hdPartsStateId_c sBtnMenuBasicCancel;
static hdPartsStateId_c sBtnMenuBasicIn;
static hdPartsStateId_c sBtnMenuBasicOut;
static hdPartsStateId_c sBtnMenuPartsBlank; /* hdParts_c's ::StateID_Blank */
/* the text box runtime type (lazily initialised; shared with the layout code) */
static u32 sBtnMenuTextBoxTypeReady;
static hdPartsTypeInfo_c sBtnMenuTextBoxType;
static u32 sBtnMenuTextBoxBaseTypeReady;
static hdPartsTypeInfo_c sBtnMenuTextBoxBaseType;
static u32 sBtnMenuPaneTypeReady;
static hdPartsTypeInfo_c sBtnMenuPaneType;
static u32 sBtnMenuRootTypeReady;
static hdPartsTypeInfo_c sBtnMenuRootType;

static f32 sBtnMenuConst50000[2] = {};
static f32 sBtnMenuConst10000[2] = {};
static hdUiSafeString sBtnMenuTextPaneNames[2] = {}; /* findTextPanes' function-local static */
static u32 sBtnMenuTextPaneNamesReady;
static hdUiSafeString sBtnMenuGroupNames[2] = {};
static hdUiSafeString sBtnMenuAnimNames[3] = {};
static hdPartsStateId_c sBtnMenuDecide;
static hdPartsStateId_c sBtnMenuCancel;
static hdUiSafeString sBtnMenuLayoutName;
static const u32 sBtnMenuAnimGroups[3] = {1, 1, 1};
static u32 sBtnMenuStateNumberReady;
static s32 sBtnMenuStateNumber;

void hdBtnMenu_formatCount(PaneString_l* text, const char* format, u32 value);

/* 0263BC30 */
// exit-time destruction of findTextPanes' pane names
static void hdScreenBtnMenu_destroyTextPaneNames() {
    gabi::call(0x028F0164, gabi::ea(sBtnMenuTextPaneNames), 2u, (u32)sizeof(hdUiSafeString), 0x0263CB1Cu, 0u, 0u);
}

/* 0263BC54 */
// hdScreenBtnMenu_c::~hdScreenBtnMenu_c() (GHS deleting destructor)
void hdScreenBtnMenu_c::destroy(hdScreenBtnMenu_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mButton.mStateHolder.mHolderVtable = &sBtnMenuHolderVtable;
    i_this->mButton.mSecondaryVtable = &sBtnMenuSecondaryVtable;
    hdBasicButton_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0263BCC0 */
void hdScreenBtnMenu_c::decideEnter() {
    part()->synchronizeFocus();
    hdAnimSet_play(part()->animSet(), BTN_MENU_ANIM_DECIDE, 0, 1.0f);
}

/* 0263BD08 */
// after the decide animation: back to the touch or cursor selection, or done
void hdScreenBtnMenu_c::decideMove() {
    if (sBtnMenuEventManager == NULL) {
        return;
    }
    part()->synchronizeFocus();
    if (!hdAnimSet_isEnd(part()->animSet(), BTN_MENU_ANIM_DECIDE)) {
        return;
    }
    u32 requested = mRequestedState;
    if (requested == BTN_MENU_SELECT_CURSOR) {
        if (hdButton_inputEnabled_l() && mButton.mTouchEnabled &&
            hdButtonEventManager_getActiveReceiver_l(sBtnMenuEventManager) == mButton.mpReceiver.v) {
            returnToTouch();
            return;
        }
        gabi::call_ptr(vtbl()->mCancelCursor.get(), this);
    } else if (requested == BTN_MENU_SELECT_TOUCH) {
        decideEnd();
    } else {
        part()->changeState(&sBtnMenuBasicEnd);
    }
}

/* 0263BE50 */
// the cancel animation plays backwards for the first animation
void hdScreenBtnMenu_c::cancelEnter() {
    u32 animation = mAnimation;
    f32 rate = 1.0f;
    if (animation == 0) {
        rate = -1.0f;
    }
    hdAnimSet_play(part()->animSet(), animation, 0, rate);
}

/* 0263BE7C */
// a decide on this button selects it by touch; otherwise off once the animation ends
void hdScreenBtnMenu_c::cancelMove() {
    hdButtonEventManager_l* manager = sBtnMenuEventManager;
    if (manager == NULL) {
        return;
    }
    gabi::Local<hdButtonEvent_l> event;
    hdButtonEvent_copy_l(event, &manager->mCurrentEvent);
    if (event->mReceiverId == part()->getReceiverId() && gabi::call_ptr<BOOL>(vtbl()->mIsDecide.get(), this, (u32)event->mType)) {
        if (hdButton_inputEnabled_l() && mButton.mTouchEnabled) {
            part()->clearFocus();
        }
        gabi::call_ptr(vtbl()->mPlayTouchSound.get(), this);
        part()->changeState(&sBtnMenuBasicSelectTouch);
        return;
    }
    if (hdAnimSet_isEnd(part()->animSet(), mAnimation)) {
        part()->changeState(&sBtnMenuBasicOff);
    }
}

/* 0263BF68 */
// hdScreenBtnMenu_c::hdScreenBtnMenu_c() (GHS: allocates when called with NULL)
hdScreenBtnMenu_c* hdScreenBtnMenu_c::construct(hdScreenBtnMenu_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenBtnMenu_c*)operator_new(sizeof(hdScreenBtnMenu_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdBasicButton_construct_l(i_this);
    i_this->mButton.mVtable = (hdPartsVtbl_c*)(void*)&sBtnMenuVtable;
    i_this->mButton.mStateHolder.mHolderVtable = &sBtnMenuHolderVtable;
    i_this->mNewMarkPhase = BTN_MENU_NEW_MARK_OFF;
    i_this->mButton.mSecondaryVtable = &sBtnMenuSecondaryVtable;
    i_this->mKind = 0;
    // HD: GHS tests the address of the (trivially constructed) pane pointer array
    if (gabi::ea(i_this->mpTextPanes) == 0) {
        operator_new(sizeof(i_this->mpTextPanes));
    }
    return i_this;
}

/* inlined into findTextPanes: the text box runtime type */
static hdPartsTypeInfo_c* hdBtnMenu_getTextBoxTypeInfo() {
    if (!sBtnMenuTextBoxTypeReady) {
        sBtnMenuTextBoxTypeReady = 1;
        if (!sBtnMenuTextBoxBaseTypeReady) {
            sBtnMenuTextBoxBaseTypeReady = 1;
            if (!sBtnMenuPaneTypeReady) {
                sBtnMenuPaneTypeReady = 1;
                if (!sBtnMenuRootTypeReady) {
                    sBtnMenuRootType.mParent = NULL;
                    sBtnMenuRootTypeReady = 1;
                }
                sBtnMenuPaneType.mParent = &sBtnMenuRootType;
            }
            sBtnMenuTextBoxBaseType.mParent = &sBtnMenuPaneType;
        }
        sBtnMenuTextBoxType.mParent = &sBtnMenuTextBoxBaseType;
    }
    return &sBtnMenuTextBoxType;
}

/* inlined: the type test of a dynamic cast */
static bool hdBtnMenu_isDerivedFrom(hdPartsTypeInfo_c* type, hdPartsTypeInfo_c* base) {
    for (; type != NULL; type = type->mParent) {
        if (type == base) {
            return true;
        }
    }
    return false;
}

/* 0263BFF0 */
// the two text panes, found by name and cast to text boxes
void hdScreenBtnMenu_c::findTextPanes() {
    if (!sBtnMenuTextPaneNamesReady) {
        sBtnMenuTextPaneNamesReady = 1;
        hdParts_setSafeString(&sBtnMenuTextPaneNames[0], &sBtnMenuSafeStringVtable, "T_Btn_00");
        // HD: GHS constructs the second element through its address, allocating if it is NULL
        hdUiSafeString* second = &sBtnMenuTextPaneNames[1];
        if (gabi::ea(second) == 0) {
            second = (hdUiSafeString*)operator_new(sizeof(hdUiSafeString));
        }
        if (second != NULL) {
            hdParts_setSafeString(second, &sBtnMenuSafeStringVtable, "T_NewNum_00");
        }
        __register_global_object(0x101F55EC);
    }
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mButton.mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < 2; i++) {
        hdUiSafeString* name = &sBtnMenuTextPaneNames[i];
        u32 assure = name->mVtable->mAssureTermination.get();
        hdCursorPaneVtbl_c* rootVtable = root->mVtable;
        gabi::call_ptr(assure, name);
        hdCursorPane_c* pane = gabi::call_ptr<hdCursorPane_c*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)name->mText, 1u);
        hdPartsTypeInfo_c* textBoxType = hdBtnMenu_getTextBoxTypeInfo();
        if (pane != NULL && hdBtnMenu_isDerivedFrom(pane->getRuntimeTypeInfo(), textBoxType)) {
            mpTextPanes[i] = pane;
        } else {
            mpTextPanes[i] = NULL;
        }
    }
}

/* 0263C25C */
BOOL hdScreenBtnMenu_c::load(void* archive, void* heap, void* par) {
    if (!hdBasicButton_c::load(&sBtnMenuLayoutName, archive, heap, par)) {
        return FALSE;
    }
    findTextPanes();
    hdBtnMenu_animOperationD4(part()->animSet(), BTN_MENU_ANIM_NEW_MARK, 3);
    return TRUE;
}

/* 0263C2DC */
// the number of unread Miiverse comments in "T_NewNum_00"
void hdScreenBtnMenu_c::updateNewCount() {
    // HD: keep the original frame offsets (text +8, buffer +0x14, ten characters)
    gabi::NativeFrame<0x30> frame;
    void* comments = sBtnMenuComments;
    PaneString_l* text = gabi::at<PaneString_l>(frame + 8);
    be<u16>* buffer = gabi::at<be<u16>>(frame + 0x14);
    buffer[9] = 0;
    text->mCapacity = 10;
    text->mpBuffer = buffer;
    buffer[0] = 0;
    text->mpVTable.v = gabi::ea(&sBtnMenuTextVtable);
    hdBtnMenu_formatCount(text, "btn menu new count format", hdBtnMenu_notifyCount_l(comments));
    ((hdTextPane_c*)(void*)mpTextPanes[1].get())->setStringPrepared(text, NULL);
}

/* 0263C360 */
// the Miiverse button's new mark: in while comments are unread (not in touch-only mode), the count shown,
// out once they are read
void hdScreenBtnMenu_c::update() {
    if (mKind == 2) {
        void* comments = sBtnMenuComments;
        u32 phase = mNewMarkPhase;
        if (phase == BTN_MENU_NEW_MARK_OFF) {
            if (hdBtnMenu_hasNotify_l(comments) && sBtnMenuController->mMode != 2) {
                mNewMarkPhase = mNewMarkPhase + 1;
                hdAnimSet_play(part()->animSet(), BTN_MENU_ANIM_NEW_MARK_IN, 3, 1.0f);
                updateNewCount();
            }
        } else if (phase == BTN_MENU_NEW_MARK_IN) {
            if (hdAnimSet_isEnd(part()->animSet(), BTN_MENU_ANIM_NEW_MARK_IN) || !hdBtnMenu_hasNotify_l(comments)) {
                mNewMarkPhase = mNewMarkPhase + 1;
            }
            updateNewCount();
        } else if (phase == BTN_MENU_NEW_MARK_SHOWN) {
            if (!hdBtnMenu_hasNotify_l(comments) || sBtnMenuController->mMode == 2) {
                mNewMarkPhase = mNewMarkPhase + 1;
                hdAnimSet_play(part()->animSet(), BTN_MENU_ANIM_NEW_MARK_OUT, 3, 1.0f);
            } else {
                updateNewCount();
            }
        } else if (phase == BTN_MENU_NEW_MARK_OUT) {
            if (hdAnimSet_isEnd(part()->animSet(), BTN_MENU_ANIM_NEW_MARK_IN)) {
                mNewMarkPhase = BTN_MENU_NEW_MARK_OFF;
            }
        }
    }
    hdBasicButton_c::update();
}

/* inlined: whether the button's current state is `state` */
static bool hdBtnMenu_isState(hdScreenBtnMenu_c* i_this, hdPartsStateId_c* state) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(i_this->part()->stateMachine());
    hdPartsStateIdVtbl_c* vtable = state->mVtable;
    u32 key = current->getKey();
    return key == gabi::call_ptr<u32>(vtable->mGetKey.get(), state);
}

/* 0263C524 */
// the receiver joins the event manager unless the button is hidden or sliding in or out
void hdScreenBtnMenu_c::registerReceiver() {
    if (hdBtnMenu_isState(this, &sBtnMenuPartsBlank)) {
        return;
    }
    if (hdBtnMenu_isState(this, &sBtnMenuBasicIn)) {
        return;
    }
    if (hdBtnMenu_isState(this, &sBtnMenuBasicOut)) {
        return;
    }
    if (sBtnMenuEventManager != NULL && mButton.mpReceiver != NULL) {
        hdButtonEventManager_pushReceiver_l(sBtnMenuEventManager, mButton.mpReceiver);
    }
}

/* 0263C654 */
void hdScreenBtnMenu_c::decide() {
    gabi::call_ptr(vtbl()->mPlaySound.get(), this);
    part()->changeState(&sBtnMenuDecide);
}

/* 0263C69C */
// a tap: a touch event of the touch button
BOOL hdScreenBtnMenu_c::isTouchTap(hdButtonEvent_l* event) {
    bool touch = event->mType == HD_BUTTON_EVENT_TOUCH_e;
    return event->mButton == HD_BUTTON_EVENT_TOUCH_e && touch;
}

/* 0263C6D0 */
// the layout with BasicButton's seven animations and the three of the new mark
BOOL hdScreenBtnMenu_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sBtnMenuUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mButton.mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 10, 4, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 3; i++) {
        hdAnimSet_bindGroup(part()->animSet(), BTN_MENU_ANIM_NEW_MARK + i, &sBtnMenuAnimNames[i], &sBtnMenuGroupNames[sBtnMenuAnimGroups[i]]);
    }
    return TRUE;
}

/* 0263C7E8 */
void hdScreenBtnMenu_c::playSound() {
    hdBtnMenu_playSoundName("SE_MENU_TAB_SWITCH");
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0263C7F4 */
static void __sinit_hd_screen_btn_menu_cpp() {
    // the statics of the shared headers (here their two flag bytes follow this TU's two float pairs)
    for (int i = 0; i < 4; i++) {
        gabi::store<u32>(0x10490610 + 4 * i, 0);
    }
    __register_global_object(0x101F55F8);
    gabi::store<f32>(0x104905E4, -3.1415927f);
    gabi::store<f32>(0x104905E8, 3.1415927f);
    gabi::call(0x028ED6F8, 0x104905FCu);
    __register_global_object(0x101F5604);
    gabi::call(0x028EAB2C, 0x104905FDu);
    __register_global_object(0x101F5610);
    sBtnMenuConst50000[0] = 50000.0f;
    sBtnMenuConst50000[1] = 50000.0f;
    sBtnMenuConst10000[0] = 10000.0f;
    sBtnMenuConst10000[1] = 10000.0f;
    hdParts_setSafeString(&sBtnMenuGroupNames[0], &sBtnMenuSafeStringVtable, "G_Btn_00");
    hdParts_setSafeString(&sBtnMenuGroupNames[1], &sBtnMenuSafeStringVtable, "G_NewMark_00");
    hdParts_setSafeString(&sBtnMenuAnimNames[0], &sBtnMenuSafeStringVtable, "NewMark");
    hdParts_setSafeString(&sBtnMenuAnimNames[1], &sBtnMenuSafeStringVtable, "NewMarkIn");
    hdParts_setSafeString(&sBtnMenuAnimNames[2], &sBtnMenuSafeStringVtable, "NewMarkOut");
    s32 number;
    if (sBtnMenuStateNumberReady) {
        number = sBtnMenuStateNumber;
    } else {
        number = 0;
        sBtnMenuStateNumberReady = 1;
    }
    // the decide and cancel states refine BasicButton's (field +0x2C)
    sBtnMenuDecide.setup(number + 1, "::StateID_Decide", &sBtnMenuStateIdVtable, 0x38, (hdStateId*)&sBtnMenuBasicDecide);
    sBtnMenuCancel.setup(number + 2, "::StateID_Cancel", &sBtnMenuStateIdVtable, 0x3B, (hdStateId*)&sBtnMenuBasicCancel);
    sBtnMenuStateNumber = number + 2;
    hdParts_setSafeString(&sBtnMenuLayoutName, &sBtnMenuSafeStringVtable, "BtnMenu_00");
}

/* 0263BB30 */
static s32 hdScreenBtnMenu_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sBtnMenuVtable 100EA468 nocheck
data sBtnMenuHolderVtable 100EA6C0 nocheck
data sBtnMenuSecondaryVtable 100EA6D0 nocheck
data sBtnMenuSafeStringVtable 100EA2AC nocheck
data sBtnMenuStateIdVtable 100EA360 nocheck
data sBtnMenuTextVtable 100EA2F4 nocheck
data sBtnMenuUiSystem 1018C404 nocheck
data sBtnMenuEventManager 101F4FF0 nocheck
data sBtnMenuComments 1018F4AC nocheck
data sBtnMenuController 101F8344 nocheck
data sBtnMenuBasicEnd 1049D800 nocheck
data sBtnMenuBasicOff 1049D830 nocheck
data sBtnMenuBasicSelectTouch 1049D8C0 nocheck
data sBtnMenuBasicDecide 1049D950 nocheck
data sBtnMenuBasicCancel 1049D980 nocheck
data sBtnMenuBasicIn 1049D9E0 nocheck
data sBtnMenuBasicOut 1049DA10 nocheck
data sBtnMenuPartsBlank 1049DD60 nocheck
data sBtnMenuTextBoxTypeReady 101FDD40 nocheck
data sBtnMenuTextBoxType 101FDD3C nocheck
data sBtnMenuTextBoxBaseTypeReady 101FD8F0 nocheck
data sBtnMenuTextBoxBaseType 101FD8F4 nocheck
data sBtnMenuPaneTypeReady 101FD89C nocheck
data sBtnMenuPaneType 101FD8F8 nocheck
data sBtnMenuRootTypeReady 101FD888 nocheck
data sBtnMenuRootType 101FD8FC nocheck
data sBtnMenuConst50000 104905EC nocheck
data sBtnMenuConst10000 104905F4 nocheck
data sBtnMenuTextPaneNames 10490600 nocheck
data sBtnMenuTextPaneNamesReady 10490648 nocheck
data sBtnMenuGroupNames 10490620 nocheck
data sBtnMenuAnimNames 10490630 nocheck
data sBtnMenuDecide 1049064C nocheck
data sBtnMenuCancel 1049067C nocheck
data sBtnMenuLayoutName 104906AC nocheck
data sBtnMenuAnimGroups 100EA30C
data sBtnMenuStateNumberReady 101FD8E4 nocheck
data sBtnMenuStateNumber 101FDD50 nocheck
str 100EA3BC "T_Btn_00" in 0263BFF0
str 100EA3B0 "T_NewNum_00" in 0263BFF0
str 100EA3C8 "btn menu new count format" renamed
str 100EA3D0 "SE_MENU_TAB_SWITCH" in 0263C7E8
str 100EA408 "G_Btn_00" in 0263C7F4
str 100EA414 "G_NewMark_00" in 0263C7F4
str 100EA3F4 "NewMark" in 0263C7F4
str 100EA3FC "NewMarkIn" in 0263C7F4
str 100EA424 "NewMarkOut" in 0263C7F4
str 100EA43C "::StateID_Decide" in 0263C7F4
str 100EA450 "::StateID_Cancel" in 0263C7F4
str 100EA430 "BtnMenu_00" in 0263C7F4
ext 026F0B8C void hdBasicButton_c::destroy(hdBasicButton_c*, u32)
ext 026F1D04 BOOL hdBasicButton_c::load(void*, void*, void*, void*)
ext 026F1F70 void hdBasicButton_c::update()
ext 026F17BC void hdBasicButton_c::decideEnd()
ext 026F17CC void hdBasicButton_c::returnToTouch()
ext 026F66F4 void hdButtonPart_c::synchronizeFocus()
ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
ext 026F66B0 void hdButtonPart_c::clearFocus()
ext 027046D4 void hdTextPane_c::setStringPrepared(PaneString_l*, PaneWork_l*)
ext 02759D3C void hdBtnMenu_formatCount(PaneString_l*, const char*, u32)
ext 026F1C3C hdBasicButton_c* hdBasicButton_construct_l(hdBasicButton_c*)
ext 026F67A4 BOOL hdButton_inputEnabled_l()
ext 026F621C BOOL hdButton_canDispatch_l(u32)
ext 026F69A4 void hdButton_markTouchRelease_l()
ext 020013C0 hdButtonEvent_l* hdButtonEvent_copy_l(hdButtonEvent_l*, hdButtonEvent_l*)
ext 02615140 u32 hdButtonEventManager_getActiveReceiver_l(hdButtonEventManager_l*)
ext 026150C0 void hdButtonEventManager_pushReceiver_l(hdButtonEventManager_l*, hdButtonReceiver_l*)
ext 020398B8 u32 hdBtnMenu_notifyCount_l(void*)
ext 02039970 u32 hdBtnMenu_hasNotify_l(void*)
ext 02005708 void hdBtnMenu_animOperationD4(hdAnimSet*, u32, u32)
ext 02030B38 void hdBtnMenu_playSound(u32)
ext 026F61EC BOOL hdButton_isMenu_l(u32)
*/
