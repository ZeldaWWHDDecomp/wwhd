/**
 * hd_screen_msg_window_main2.cpp
 * HD - the second copy of the main message window "MsgWindowMain_00": the message window of
 * FW-parts_msg_base with a two- and a three-choice window, a number selection ("P_SelNumber_00"), a key icon
 * part ("L_KeyIcon_00") and a tail that points at the speaker
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_msg_msg_window_main2). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation and group names
 * are HD strings; all other names are ours.
 *
 */

#include "d/hd_screen_msg_window_main2.h"
static u32 sMain2PaneNamesRegistration;
static u32 sMain2TextPaneNamesRegistration;
static u32 sMain2ChoiceWindowNamesRegistration;
static u32 sMain2SelNumberNameRegistration;
static u32 sMain2HeaderStatics;
static u32 sMain2HeaderDescriptors;
static u32 sMain2HeaderZero;
static u32 sMain2PiConstants;

static hdPartsVtbl_c sMain2Vtable;
static u32 sMain2HolderVtable;
static u32 sMain2SecondaryVtable;
static hdUiStringTable sMain2SafeStringVtable;
static hdScreenUiSystem* sMain2UiSystem;
static hdMsgManager_l* sMain2MessageManager;
static hdUiSafeString sMain2KeyIconClassName; /* the keyboard key icon's layout "KeyIcon_00" */
static hdPartsStateId_c sMain2PartsBlank; /* hdParts_c's ::StateID_Blank */
/* FW-parts_msg_base's message-window states (names from its static initialiser where known) */
static hdPartsStateId_c sMain2StateEnd;
static hdPartsStateId_c sMain2StateKeyIconInAnimEndWaitToContinue;
static hdPartsStateId_c sMain2StateContinue;
static hdPartsStateId_c sMain2StateE34C;
static hdPartsStateId_c sMain2StateDelete;
static hdPartsStateId_c sMain2StateOnlyContinue;
static hdPartsStateId_c sMain2StateStop;
static hdPartsStateId_c sMain2StateE10C;
static hdPartsStateId_c sMain2StateCameraToOpen;

/* the function-local statics of the pane finders, and their guards */
static hdUiSafeString sMain2PaneNames[5] = {};
static u32 sMain2PaneNamesReady;
static hdUiSafeString sMain2TextPaneNames[3] = {};
static u32 sMain2TextPaneNamesReady;
static hdUiSafeString sMain2ChoiceWindowNames[2] = {};
static u32 sMain2ChoiceWindowNamesReady;
static hdUiSafeString sMain2SelNumberName[1] = {};
static u32 sMain2SelNumberNameReady;

static hdUiSafeString sMain2KeyIconPaneName;
static hdUiSafeString sMain2KeyIconLayoutName;
static hdUiSafeString sMain2GroupNames[5] = {};
static hdUiSafeString sMain2AnimNames[7] = {};
static hdUiSafeString sMain2ArchiveNames[2] = {};
static hdUiSafeString sMain2LayoutName;
static const u32 sMain2AnimGroups[7] = {1, 4, 0, 0, 2, 1, 3};

/* inlined: find a pane of the layout by name */
static hdMsgMain2Pane_l* hdMain2_findPane(hdMsgMain2Pane_l* root, hdUiSafeString* name) {
    u32 assure = name->mVtable->mAssureTermination.get();
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    gabi::call_ptr(assure, name);
    return gabi::call_ptr<hdMsgMain2Pane_l*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)name->mText, 1u);
}

/* 026B5C34 */
// exit-time destruction of findPanes' names
static void hdMain2_destroyPaneNames() {
    gabi::call(0x028F0164, gabi::ea(sMain2PaneNames), 5u, (u32)sizeof(hdUiSafeString), 0x026B76D0u, 0u, 0u);
}

/* 026B5C58 */
static void hdMain2_destroyTextPaneNames() {
    gabi::call(0x028F0164, gabi::ea(sMain2TextPaneNames), 3u, (u32)sizeof(hdUiSafeString), 0x026B76D0u, 0u, 0u);
}

/* 026B5C7C */
static void hdMain2_destroyChoiceWindowNames() {
    gabi::call(0x028F0164, gabi::ea(sMain2ChoiceWindowNames), 2u, (u32)sizeof(hdUiSafeString), 0x026B76D0u, 0u, 0u);
}

/* 026B5CA0 */
static void hdMain2_destroySelNumberName() {
    gabi::call(0x028F0164, gabi::ea(sMain2SelNumberName), 1u, (u32)sizeof(hdUiSafeString), 0x026B76D0u, 0u, 0u);
}

/* 026B5CC4 */
// hdScreenMsgWindowMain2_c::~hdScreenMsgWindowMain2_c() (GHS deleting destructor)
void hdScreenMsgWindowMain2_c::destroy(hdScreenMsgWindowMain2_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sMain2HolderVtable;
    i_this->mSecondaryVtable = &sMain2SecondaryVtable;
    destroyLayoutWaitScreen(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026B5D30 */
// hdScreenMsgWindowMain2_c::hdScreenMsgWindowMain2_c() (GHS: allocates when called with NULL)
hdScreenMsgWindowMain2_c* hdScreenMsgWindowMain2_c::construct(hdScreenMsgWindowMain2_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenMsgWindowMain2_c*)operator_new(sizeof(hdScreenMsgWindowMain2_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdMsgWindowStates_c::construct(i_this->windowStates());
    i_this->mVtable = &sMain2Vtable;
    i_this->mSecondaryVtable = &sMain2SecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sMain2HolderVtable;
    // HD: GHS tests the address of each (trivially constructed) pane pointer member
    if (gabi::ea(i_this->mpPanes) == 0) {
        operator_new(sizeof(i_this->mpPanes));
    }
    if (gabi::ea(i_this->mpTextPanes) == 0) {
        operator_new(sizeof(i_this->mpTextPanes));
    }
    if (gabi::ea(i_this->mpChoiceWindowPanes) == 0) {
        operator_new(sizeof(i_this->mpChoiceWindowPanes));
    }
    if (gabi::ea(&i_this->mpKeyIcon) == 0) {
        operator_new(sizeof(i_this->mpKeyIcon));
    }
    if (gabi::ea(&i_this->mpSelNumber) == 0) {
        operator_new(sizeof(i_this->mpSelNumber));
    }
    for (int i = 0; i < 5; i++) {
        i_this->mpPanes[i] = NULL;
    }
    for (int i = 0; i < 3; i++) {
        i_this->mpTextPanes[i] = NULL;
    }
    for (int i = 0; i < 2; i++) {
        i_this->mpChoiceWindowPanes[i] = NULL;
    }
    i_this->mpKeyIcon = NULL;
    i_this->mpSelNumber = NULL;
    return i_this;
}

/* inlined: one element of a function-local static name array (GHS constructs each element through its
 * address and allocates when that address is NULL) */
static void hdMain2_setStaticName(hdUiSafeString* name, const char* text) {
    if (gabi::ea(name) == 0) {
        name = (hdUiSafeString*)operator_new(sizeof(hdUiSafeString));
        if (name == NULL) {
            return;
        }
    }
    hdParts_setSafeString(name, &sMain2SafeStringVtable, text);
}

/* 026B5E40 */
// the five layout panes; "N_All_00" and "N_SubWindow_00" are kept separately, the sub window hidden
void hdScreenMsgWindowMain2_c::findPanes() {
    if (!sMain2PaneNamesReady) {
        sMain2PaneNamesReady = 1;
        hdParts_setSafeString(&sMain2PaneNames[0], &sMain2SafeStringVtable, "N_Position_00");
        hdMain2_setStaticName(&sMain2PaneNames[1], "N_All_00");
        hdMain2_setStaticName(&sMain2PaneNames[2], "N_Window_00");
        hdMain2_setStaticName(&sMain2PaneNames[3], "N_Msg_00");
        hdMain2_setStaticName(&sMain2PaneNames[4], "N_SubWindow_00");
        __register_global_object(gabi::ea(&sMain2PaneNamesRegistration));
    }
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (int i = 0; i < 5; i++) {
        mpPanes[i] = hdMain2_findPane(root, &sMain2PaneNames[i]);
    }
    mpAllPane = mpPanes[1];
    hdMsgMain2Pane_l* subWindow = mpPanes[4];
    mpSubWindowPane = subWindow;
    subWindow->mFlags = subWindow->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
}

/* 026B6040 */
// the message and the two choice texts (each with room for 335 characters); the message pane's position
// and colour are kept, the pane hidden
void hdScreenMsgWindowMain2_c::findTextPanes() {
    if (!sMain2TextPaneNamesReady) {
        sMain2TextPaneNamesReady = 1;
        hdParts_setSafeString(&sMain2TextPaneNames[0], &sMain2SafeStringVtable, "T_Msg_00");
        hdMain2_setStaticName(&sMain2TextPaneNames[1], "T_Choise2_00");
        hdMain2_setStaticName(&sMain2TextPaneNames[2], "T_Choise3_00");
        __register_global_object(gabi::ea(&sMain2TextPaneNamesRegistration));
    }
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < 3; i++) {
        mpTextPanes[i] = (hdMsgMain2TextBox_l*)hdMain2_findPane(root, &sMain2TextPaneNames[i]);
        if (mpTextPanes[i] != NULL) {
            ((hdTextPane_c*)(void*)mpTextPanes[i].get())->setCharacterCapacity(MSG_MAIN2_TEXT_CAPACITY);
        }
    }
    hdMsgMain2TextBox_l* text = mpTextPanes[0];
    f32 x = text->mMatrix[3];
    f32 y = text->mMatrix[7];
    mTextX = x;
    mTextY = y;
    text = mpTextPanes[0];
    text->mFlags = text->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
    mpTextPane = mpTextPanes[0];
    mpChoiceTexts[0] = mpTextPanes[1];
    mpChoiceTexts[1] = mpTextPanes[2];
    // HD: the colour is copied bytewise (GHS lswi/stswi: the field is unaligned here)
    u32 color = gabi::ea(&mpTextPanes[0]->mColor);
    for (int i = 0; i < 4; i++) {
        gabi::store<u8>(gabi::ea(mTextColor) + i, gabi::load<u8>(color + i));
    }
}

/* 026B628C */
void hdScreenMsgWindowMain2_c::findChoiceWindows() {
    if (!sMain2ChoiceWindowNamesReady) {
        sMain2ChoiceWindowNamesReady = 1;
        hdParts_setSafeString(&sMain2ChoiceWindowNames[0], &sMain2SafeStringVtable, "W_Choise2_00");
        hdMain2_setStaticName(&sMain2ChoiceWindowNames[1], "W_Choise3_00");
        __register_global_object(gabi::ea(&sMain2ChoiceWindowNamesRegistration));
    }
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (int i = 0; i < 2; i++) {
        mpChoiceWindowPanes[i] = hdMain2_findPane(root, &sMain2ChoiceWindowNames[i]);
    }
    mpChoiceWindows[0] = mpChoiceWindowPanes[0];
    mpChoiceWindows[1] = mpChoiceWindowPanes[1];
}

/* 026B63FC */
void hdScreenMsgWindowMain2_c::findKeyIcon() {
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdMsgMain2Pane_l* pane = hdMain2_findPane(root, &sMain2KeyIconPaneName);
    mpKeyIcon = pane;
    mpKeyIconPane = pane;
}

/* 026B6470 */
// the number selection's pane, hidden, with its loop animation running
void hdScreenMsgWindowMain2_c::findSelNumber() {
    if (!sMain2SelNumberNameReady) {
        sMain2SelNumberNameReady = 1;
        hdParts_setSafeString(&sMain2SelNumberName[0], &sMain2SafeStringVtable, "P_SelNumber_00");
        __register_global_object(gabi::ea(&sMain2SelNumberNameRegistration));
    }
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdMsgMain2Pane_l* pane = hdMain2_findPane(root, &sMain2SelNumberName[0]);
    mpSelNumber = pane;
    pane->mFlags = pane->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
    mpSelNumberPane = mpSelNumber;
    windowStates()->startAnimation(MSG_MAIN2_ANIM_LOOP_SEL_NUMBER, 4);
}

/* 026B6540 */
// registered with the message manager; the layout with its key icon part, the panes, the message
u32 hdScreenMsgWindowMain2_c::init(u32 arg1, u32 arg2, void* desc) {
    sMain2MessageManager->mpMainWindow2 = this;
    attachArchives(sMain2ArchiveNames, 2);
    if (!vLoad(&sMain2LayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!vCreateChildren()) {
        return FALSE;
    }
    findPanes();
    findTextPanes();
    findChoiceWindows();
    findKeyIcon();
    findSelNumber();
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    // HD: the layout's own spelling of this pane's name ("Kay")
    hdMsgMain2Pane_l* oldKeyIcon = gabi::call_ptr<hdMsgMain2Pane_l*>(root->mVtable->mFindPaneByName.get(), root, gabi::at<hdCursorChar_t>(gabi::guest_addr("L_KayIcon_00")), 1u);
    if (oldKeyIcon != NULL) {
        oldKeyIcon->mFlags = oldKeyIcon->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
    }
    mpMessage = (WindowMessage*)(void*)sMain2MessageManager->mMessage;
    ((hdMsgMain2Play_l*)dComIfGp_get())->mAdvanceCount = 0;
    ((hdMsgMain2Play_l*)dComIfGp_get())->mHandInput = 0;
    gabi::call_ptr(((gfn*)(void*)mVtable.get())[0x444 / 4].get(), this);
    startState(&sMain2PartsBlank);
    return TRUE;
}

/* 026B66C4 */
void hdScreenMsgWindowMain2_c::finalize() {
    sMain2MessageManager->mpMainWindow2 = NULL;
    windowStates()->update();
}

/* 026B66D8 */
// a placement request (a new position, or showing / hiding) is passed to the message manager; once it
// takes it, the window keeps the new state
void hdScreenMsgWindowMain2_c::updatePlacement() {
    u32 placed = mPlaced;
    if (placed) {
        if (!mPlaceRequest) {
            if (!hdMsgManager_setWindowPosition_l(sMain2MessageManager, 0, mRequestedPosition, mpPanes[4], 1)) {
                return;
            }
            mPlaced = mPlaceRequest;
        } else {
            if (mPosition == mRequestedPosition) {
                return;
            }
            if (!hdMsgManager_setWindowPosition_l(sMain2MessageManager, placed, mRequestedPosition, mpPanes[4], 1)) {
                return;
            }
        }
    } else {
        u32 request = mPlaceRequest;
        if (!request) {
            return;
        }
        if (!hdMsgManager_setWindowPosition_l(sMain2MessageManager, request, mRequestedPosition, mpPanes[4], 1)) {
            return;
        }
        mPlaced = mPlaceRequest;
    }
    mPosition = mRequestedPosition;
}

/* inlined: whether the window's current state is `state` */
static bool hdMain2_isState(hdScreenMsgWindowMain2_c* i_this, hdPartsStateId_c* state) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(i_this->stateMachine());
    hdPartsStateIdVtbl_c* vtable = state->mVtable;
    u32 key = current->getKey();
    return key == gabi::call_ptr<u32>(vtable->mGetKey.get(), state);
}

/* 026B67A4 */
// while a message runs and the window is not hidden: the advance count resets outside the end states, then
// the controller, the state, the children, the layout and the placement
void hdScreenMsgWindowMain2_c::executeState() {
    if (sMain2MessageManager->mMode != 1) {
        return;
    }
    if (hdMain2_isState(this, &sMain2PartsBlank)) {
        return;
    }
    if (!hdMain2_isState(this, &sMain2StateEnd) && !hdMain2_isState(this, &sMain2StateE34C) &&
        !hdMain2_isState(this, &sMain2StateDelete)) {
        ((hdMsgMain2Play_l*)dComIfGp_get())->mAdvanceCount = 0;
    }
    windowStates()->readController();
    hdParts_executeStateMachine(stateMachine());
    vExecuteChildren();
    vCalcLayout();
    updatePlacement();
}

/* 026B696C */
void hdScreenMsgWindowMain2_c::setMessage(u32 arg, u32 messageId, cXyz* speakerPos) {
    mMessageId = messageId;
    mAdvanceCount = 0;
    if (speakerPos != NULL) {
        mSpeakerPos.copy(*speakerPos);
    }
}

/* inlined into open: a text box's alignment field (two bits at `shift`), marking the text for a new layout
 * when it changes */
static void hdMain2_setTextAlign(hdMsgMain2TextBox_l* text, u32 shift, u32 value) {
    u32 align = text->mTextAlign;
    bool changed = ((align >> shift) & 3) != value;
    u32 flags = text->mTextFlags;
    text->mTextFlags = (flags & ~0x20) | ((((flags >> 5) & 1) | changed) << 5);
    if (changed) {
        text->mTextAlign = (text->mTextAlign & ~(3 << shift)) | (value << shift);
    }
}

/* 026B699C */
// a new message: while a close is requested it replaces the message unless it is the same one; a hidden
// window opens (towards the speaker, the text top-left aligned); in the end states a new message follows
u8 hdScreenMsgWindowMain2_c::open(u32 arg, u32 messageId, cXyz* speakerPos) {
    if (mpMessage->mCloseRequested) {
        if (messageId == mMessageId) {
            return FALSE;
        }
        if (!hdMain2_isState(this, &sMain2StateEnd) && !hdMain2_isState(this, &sMain2StateKeyIconInAnimEndWaitToContinue) &&
            !hdMain2_isState(this, &sMain2StateContinue) && !hdMain2_isState(this, &sMain2StateOnlyContinue) &&
            !hdMain2_isState(this, &sMain2StateStop)) {
            mpMessage->mCloseRequested = 0;
            mCloseRequested = 0;
            mOpenRequested = 1;
            setMessage(arg, messageId, speakerPos);
            return TRUE;
        }
    } else if (hdMain2_isState(this, &sMain2PartsBlank)) {
        if (speakerPos == NULL) {
            mSpeakerPos.copy(cXyz::Zero);
        }
        setMessage(arg, messageId, speakerPos);
        hdMain2_setTextAlign(mpTextPanes[0], 2, 0);
        hdMain2_setTextAlign(mpTextPanes[0], 0, 1);
        mpTextPanes[0]->mShrink = 0;
        f32 width = mpTextPanes[0]->mWidth;
        gabi::store<f32>(gabi::ea(mpMessage.get()) + 0x688, width);
        mCloseRequested = 0;
        mOpenRequested = 0;
        changeState(&sMain2StateCameraToOpen);
        return TRUE;
    } else {
        if (!hdMain2_isState(this, &sMain2StateEnd) && !hdMain2_isState(this, &sMain2StateE10C)) {
            return FALSE;
        }
        if (messageId == mMessageId) {
            return FALSE;
        }
    }
    mCloseRequested = 0;
    mOpenRequested = 1;
    setMessage(arg, messageId, speakerPos);
    return TRUE;
}

/* inlined: sead SafeString equality (both strings terminated first) */
static bool hdMain2_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
    name->assureTermination();
    u32 text = gabi::ea(name->mText.get());
    other->assureTermination();
    if (text == gabi::ea(other->mText.get())) {
        return true;
    }
    u32 left = gabi::ea(name->mText.get());
    u32 right = gabi::ea(other->mText.get());
    for (u32 n = 0; n < 0x40001; n++) {
        u8 c = *hdPartsView<be<u8>>(left + n);
        if (c != *hdPartsView<be<u8>>(right + n)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* the descriptor of a child part as the createChild hook reads it (our view): class and pane name */
struct hdMain2Desc_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ hdUiSafeString mClassName;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdUiSafeString mPaneName;
};

/* 026B6D94 */
// the child parts hook: pane "L_KeyIcon_00" gets the keyboard's key icon; other classes go to the base
u32 hdScreenMsgWindowMain2_c::createChild(hdPartsDesc_c* desc) {
    hdScreenHeap* heap = sMain2UiSystem->mHeap;
    hdMain2Desc_l* names = (hdMain2Desc_l*)desc;
    names->mPaneName.assureTermination();
    if (!hdMain2_isSameName(&names->mPaneName, &sMain2KeyIconPaneName)) {
        return TRUE;
    }
    names->mClassName.assureTermination();
    if (!hdMain2_isSameName(&names->mClassName, &sMain2KeyIconClassName)) {
        return hdParts_c::createChild(0, desc);
    }
    gptr<hdParts_c>* slot = childSlot(0);
    void* parts = hdParts_new(MSG_MAIN2_KEY_ICON_SIZE, heap, 4);
    if (parts != NULL) {
        parts = hdKeyboardKeyIcon_construct_l(parts);
    }
    *slot = (hdParts_c*)parts;
    if (child(0) == NULL || !hdKeyboardKeyIcon_create_l(child(0), 0, 0, desc)) {
        return FALSE;
    }
    return TRUE;
}

/* 026B6F98 */
u32 hdScreenMsgWindowMain2_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sMain2UiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 7, 5, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 7; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sMain2AnimNames[i], &sMain2GroupNames[sMain2AnimGroups[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* 0201AF98: cXyz::operator== */
static BOOL hdMain2_isSamePos(cXyz* a, cXyz* b) { return gabi::call<BOOL>(0x0201AF98, a, b); }

/* 026B70C4 */
// the tail: up (frame 2) when the speaker is above the window's centre, down (0) otherwise, centred (1) on
// request; a window without a speaker uses the message pane's height
void hdScreenMsgWindowMain2_c::updateTail() {
    hdMsgMain2Input_l* input = (hdMsgMain2Input_l*)(void*)mpMessage->mpInput.get();
    if (input == NULL) {
        return;
    }
    hdMsgMain2Play_l* game = (hdMsgMain2Play_l*)dComIfGp_get();
    if (input->mSpeaker == game->mSpeaker) {
        return;
    }
    u32 kind = input->mTailKind;
    f32 frame = 0.0f;
    if (kind == 0) {
        u8* player = ((hdMsgMain2Play_l*)dComIfGp_get())->mpPlayer;
        bool up;
        if (hdMain2_isSamePos(&mSpeakerPos, &cXyz::Zero)) {
            up = !(mpTextPanes[0]->mMatrix[7] < 0.0f);
        } else if (input->mSpeaker) {
            gabi::Local<cXyz> screen;
            mDoLib_projectScreen(&mSpeakerPos, screen);
            up = !(-screen->y < 0.0f);
        } else {
            gabi::Local<cXyz> head;
            mDoLib_projectScreen((cXyz*)(player + 0x37C), head);
            gabi::Local<cXyz> screen;
            mDoLib_projectScreen(&mSpeakerPos, screen);
            up = !(-(head->y + screen->y) < 0.0f);
        }
        if (up) {
            windowStates()->setAnimationFrame(MSG_MAIN2_ANIM_POSITION, 3, 2.0f);
            return;
        }
    } else if (kind == 1) {
        windowStates()->setAnimationFrame(MSG_MAIN2_ANIM_POSITION, 3, 2.0f);
        return;
    } else if (kind != 3) {
        frame = 1.0f;
    }
    windowStates()->setAnimationFrame(MSG_MAIN2_ANIM_POSITION, 3, frame);
}

/* 026B7268 */
// no text stops, the first animations at their start, the window hidden
void hdScreenMsgWindowMain2_c::reset() {
    for (int i = 0; i < 10; i++) {
        mStopOffsets[i] = -1;
    }
    for (int i = 0; i < 10; i++) {
        mStopCounts[i] = -1;
    }
    mStopIndex = 0;
    windowStates()->setAnimationFrame(MSG_MAIN2_ANIM_IN_ALL, 1, 0.0f);
    windowStates()->setAnimationFrame(MSG_MAIN2_ANIM_IN_SUB_WINDOW, 0, 0.0f);
    windowStates()->setAnimationFrame(MSG_MAIN2_ANIM_LOOP_ALL, 2, 0.0f);
    vCalcLayout();
    vExecuteChildren();
    mPlaceRequest = 0;
    hdMsgMain2Pane_l* all = mpAllPane;
    all->mFlags = all->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
}

/* 026B7368 */
void hdScreenMsgWindowMain2_c::hideTextPanes() {
    for (u32 i = 0; i < 3; i++) {
        hdMsgMain2TextBox_l* text = mpTextPanes[i];
        text->mFlags = text->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
    }
}

/* 026B73B8 */
void hdScreenMsgWindowMain2_c::hideChoiceWindows() {
    for (u32 i = 0; i < 2; i++) {
        hdMsgMain2Pane_l* window = mpChoiceWindowPanes[i];
        window->mFlags = window->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
    }
}

/* 026B7408 */
void hdScreenMsgWindowMain2_c::playInAll() {
    windowStates()->startAnimation(MSG_MAIN2_ANIM_IN_ALL, 1);
}

/* 026B7414 */
u32 hdScreenMsgWindowMain2_c::isInAllEnd() {
    return windowStates()->animationFinished(MSG_MAIN2_ANIM_IN_ALL);
}

/* 026B741C */
void hdScreenMsgWindowMain2_c::playOutAll() {
    windowStates()->startAnimation(MSG_MAIN2_ANIM_OUT_ALL, 1);
}

/* 026B7428 */
u32 hdScreenMsgWindowMain2_c::isOutAllEnd() {
    return windowStates()->animationFinished(MSG_MAIN2_ANIM_OUT_ALL);
}

/* 026B7430 */
void hdScreenMsgWindowMain2_c::playLoopAll() {
    windowStates()->startAnimation(MSG_MAIN2_ANIM_LOOP_ALL, 2);
}

/* 026B743C */
void hdScreenMsgWindowMain2_c::playInSubWindow() {
    windowStates()->startAnimation(MSG_MAIN2_ANIM_IN_SUB_WINDOW, 0);
}

/* 026B7448 */
u32 hdScreenMsgWindowMain2_c::isInSubWindowEnd() {
    return windowStates()->animationFinished(MSG_MAIN2_ANIM_IN_SUB_WINDOW);
}

/* 026B7450 */
void hdScreenMsgWindowMain2_c::playOutSubWindow() {
    windowStates()->startAnimation(MSG_MAIN2_ANIM_OUT_SUB_WINDOW, 0);
}

/* 026B745C */
u32 hdScreenMsgWindowMain2_c::isOutSubWindowEnd() {
    return windowStates()->animationFinished(MSG_MAIN2_ANIM_OUT_SUB_WINDOW);
}

/* 026B7464 */
void hdScreenMsgWindowMain2_c::hideKeyIcon() {
    hdMsgMain2Pane_l* pane = mpKeyIcon;
    pane->mFlags = pane->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
    keyIcon()->hide();
}

/* inlined: show the key icon's pane */
static void hdMain2_showKeyIconPane(hdScreenMsgWindowMain2_c* i_this) {
    hdMsgMain2Pane_l* pane = i_this->mpKeyIcon;
    pane->mFlags = (pane->mFlags & ~MSG_MAIN2_PANE_VISIBLE) + MSG_MAIN2_PANE_VISIBLE;
}

/* 026B7480 */
void hdScreenMsgWindowMain2_c::keyIconIn() {
    hdMain2_showKeyIconPane(this);
    keyIcon()->requestIn();
}

/* 026B74A0 */
void hdScreenMsgWindowMain2_c::keyIconDecide() {
    hdMain2_showKeyIconPane(this);
    keyIcon()->requestDecide();
}

/* 026B74C0 */
void hdScreenMsgWindowMain2_c::keyIconOut() {
    hdMain2_showKeyIconPane(this);
    keyIcon()->requestOut();
}

/* 026B74E0 */
u32 hdScreenMsgWindowMain2_c::isKeyIconWaiting() {
    return keyIcon()->isWaiting();
}

/* 026B74EC */
u32 hdScreenMsgWindowMain2_c::isKeyIconHidden() {
    return keyIcon()->isHidden();
}

/* 026B74F8 */
u8 hdScreenMsgWindowMain2_c::isKeyIconNotDeciding() {
    return keyIcon()->isNotDeciding();
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026B7504 */
static void __sinit_hd_screen_msg_window_main2_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sMain2HeaderStatics), gabi::ea(&sMain2HeaderDescriptors), gabi::ea(&sMain2HeaderZero),
                                gabi::ea(&sMain2PiConstants));
    hdParts_setSafeString(&sMain2KeyIconPaneName, &sMain2SafeStringVtable, "L_KeyIcon_00");
    hdParts_setSafeString(&sMain2KeyIconLayoutName, &sMain2SafeStringVtable, "KeyIcon_00");
    hdParts_setSafeString(&sMain2GroupNames[0], &sMain2SafeStringVtable, "G_SubWindow_00");
    hdParts_setSafeString(&sMain2GroupNames[1], &sMain2SafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sMain2GroupNames[2], &sMain2SafeStringVtable, "G_LoopAll_00");
    hdParts_setSafeString(&sMain2GroupNames[3], &sMain2SafeStringVtable, "G_Position_00");
    hdParts_setSafeString(&sMain2GroupNames[4], &sMain2SafeStringVtable, "G_LoopSelNumber_00");
    hdParts_setSafeString(&sMain2AnimNames[0], &sMain2SafeStringVtable, "InAll");
    hdParts_setSafeString(&sMain2AnimNames[1], &sMain2SafeStringVtable, "LoopSelNumber");
    hdParts_setSafeString(&sMain2AnimNames[2], &sMain2SafeStringVtable, "InSubWindow");
    hdParts_setSafeString(&sMain2AnimNames[3], &sMain2SafeStringVtable, "OutSubWindow");
    hdParts_setSafeString(&sMain2AnimNames[4], &sMain2SafeStringVtable, "LoopAll");
    hdParts_setSafeString(&sMain2AnimNames[5], &sMain2SafeStringVtable, "OutAll");
    hdParts_setSafeString(&sMain2AnimNames[6], &sMain2SafeStringVtable, "Position");
    hdParts_setSafeString(&sMain2ArchiveNames[0], &sMain2SafeStringVtable, "MsgWindowMain_00");
    hdParts_setSafeString(&sMain2ArchiveNames[1], &sMain2SafeStringVtable, "KeyIcon_00");
    hdParts_setSafeString(&sMain2LayoutName, &sMain2SafeStringVtable, "MsgWindowMain_00");
}

/* WWHD glue
 data sMain2PaneNamesRegistration 101F663C nocheck
 data sMain2TextPaneNamesRegistration 101F6648 nocheck
 data sMain2ChoiceWindowNamesRegistration 101F6654 nocheck
 data sMain2SelNumberNameRegistration 101F6660 nocheck
 data sMain2HeaderStatics 104986CC nocheck
 data sMain2HeaderDescriptors 101F666C nocheck
 data sMain2HeaderZero 10498728 nocheck
 data sMain2PiConstants 100FE744 nocheck
data sMain2Vtable 100FE820 nocheck
data sMain2HolderVtable 100FECF8 nocheck
data sMain2SecondaryVtable 100FED08 nocheck
data sMain2SafeStringVtable 100FE654 nocheck
data sMain2UiSystem 1018C404 nocheck
data sMain2MessageManager 101F4B5C nocheck
data sMain2KeyIconClassName 10494FE4 nocheck
data sMain2PartsBlank 1049DD60 nocheck
data sMain2StateEnd 1049E2BC nocheck
data sMain2StateKeyIconInAnimEndWaitToContinue 1049E2EC nocheck
data sMain2StateContinue 1049E31C nocheck
data sMain2StateE34C 1049E34C nocheck
data sMain2StateDelete 1049E37C nocheck
data sMain2StateOnlyContinue 1049DF2C nocheck
data sMain2StateStop 1049DF5C nocheck
data sMain2StateE10C 1049E10C nocheck
data sMain2StateCameraToOpen 1049DE6C nocheck
data sMain2PaneNames 104986D8 nocheck
data sMain2PaneNamesReady 104987A8 nocheck
data sMain2TextPaneNames 10498700 nocheck
data sMain2TextPaneNamesReady 104987AC nocheck
data sMain2ChoiceWindowNames 10498718 nocheck
data sMain2ChoiceWindowNamesReady 104987B0 nocheck
data sMain2SelNumberName 104986B4 nocheck
data sMain2SelNumberNameReady 104987B4 nocheck
data sMain2KeyIconPaneName 104986BC nocheck
data sMain2KeyIconLayoutName 104986C4 nocheck
data sMain2GroupNames 10498738 nocheck
data sMain2AnimNames 10498760 nocheck
data sMain2ArchiveNames 10498798 nocheck
data sMain2LayoutName 104987B8 nocheck
data sMain2AnimGroups 100FE66C
str 100FE698 "N_Position_00" in 026B5E40
str 100FE6A8 "N_All_00" in 026B5E40
str 100FE68C "N_Window_00" in 026B5E40
str 100FE6B4 "N_Msg_00" in 026B5E40
str 100FE6C0 "N_SubWindow_00" in 026B5E40
str 100FE6D0 "T_Msg_00" in 026B6040
str 100FE6DC "T_Choise2_00" in 026B6040
str 100FE6EC "T_Choise3_00" in 026B6040
str 100FE6FC "W_Choise2_00" in 026B628C
str 100FE70C "W_Choise3_00" in 026B628C
str 100FE71C "P_SelNumber_00" in 026B6470
str 100FE72C "L_KayIcon_00" in 026B6540
str 100FE7EC "L_KeyIcon_00" in 026B7504
str 100FE7FC "KeyIcon_00" in 026B7504
str 100FE790 "G_SubWindow_00" in 026B7504
str 100FE7A0 "G_All_00" in 026B7504
str 100FE7AC "G_LoopAll_00" in 026B7504
str 100FE770 "G_Position_00" in 026B7504
str 100FE7BC "G_LoopSelNumber_00" in 026B7504
str 100FE754 "InAll" in 026B7504
str 100FE780 "LoopSelNumber" in 026B7504
str 100FE764 "InSubWindow" in 026B7504
str 100FE7D0 "OutSubWindow" in 026B7504
str 100FE74C "LoopAll" in 026B7504
str 100FE75C "OutAll" in 026B7504
str 100FE7E0 "Position" in 026B7504
str 100FE808 "MsgWindowMain_00" in 026B7504
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c* i_this)
ext 026F88F0 void hdParts_c::destroy(hdParts_c* i_this, s32 flags)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString* names, u32 count)
ext 026F91F8 void hdParts_c::detachArchives()
ext 026F92C0 u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc)
ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32 count)
ext 026F826C void hdPartsKeyIcon_c::hide()
ext 026F8228 void hdPartsKeyIcon_c::requestIn()
ext 026F8238 void hdPartsKeyIcon_c::requestDecide()
ext 026F8260 void hdPartsKeyIcon_c::requestOut()
ext 026F82CC u32 hdPartsKeyIcon_c::isWaiting()
ext 026F83B4 u32 hdPartsKeyIcon_c::isHidden()
ext 026F833C u8 hdPartsKeyIcon_c::isNotDeciding()
ext 025F0EA4 void mDoLib_projectScreen(cXyz*, cXyz*)
ext 026FC4D8 void destroyLayoutWaitScreen(hdParts_c*, u32)
ext 026FF21C hdMsgWindowStates_c* hdMsgWindowStates_c::construct(hdMsgWindowStates_c*)
ext 026FF39C void hdMsgWindowStates_c::update()
ext 026FF5AC void hdMsgWindowStates_c::readController()
ext 026FF5EC void hdMsgWindowStates_c::startAnimation(u32,u32)
ext 026FF65C void hdMsgWindowStates_c::setAnimationFrame(u32,u32,f32)
ext 026FF668 u32 hdMsgWindowStates_c::animationFinished(u32)
*/
