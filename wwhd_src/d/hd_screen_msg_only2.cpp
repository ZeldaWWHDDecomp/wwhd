/**
 * hd_screen_msg_only2.cpp
 * HD - the second message-only screen "MsgOnly_00": a message without a window frame; states
 * ::StateID_InAll / _OutNow / _CloseWait / _AutoCloseWait / _HandCloseWait / _NpcAnswerWait / _OutMsg /
 * _InMsg / _Stop / _OutAll / _Continue / _Close (five state functions are in hd_screen_msg_window_main.cpp)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_msg_msg_window_main2). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation, label and state
 * names are HD strings; all other names are ours.
 */

#include "d/hd_screen_msg_only2.h"
static u32 sOnly2AllPaneNameRegistration;
static u32 sOnly2TextPaneNamesRegistration;
static u32 sOnly2StartLabelRegistration;
static u32 sOnly2ContinueLabelRegistration;
static u32 sOnly2HeaderStatics;
static u32 sOnly2HeaderDescriptors;
static u32 sOnly2HeaderZero;
static u32 sOnly2PiConstants;

static hdPartsVtbl_c sOnly2Vtable;
static u32 sOnly2HolderVtable;
static u32 sOnly2SecondaryVtable;
static hdUiStringTable sOnly2SafeStringVtable;
static hdPartsStateIdVtbl_c sOnly2StateIdVtable;
static hdStateId sOnly2NoStateId;
static hdPartsStateId_c sOnly2PartsBlank;
static hdScreenUiSystem* sOnly2UiSystem;
static hdMsgManager_l* sOnly2MessageManager;
static void* sOnly2MessageResources;
static hdInputManager* sOnly2Input;
static u8 sOnly2WaitIcon; /* the wait icon's mode, probably */
static s8 sOnly2RoomNo;
static u32 sOnly2LabelVtable;
static u32 sOnly2LabelVtableDone;
static const u32 sOnly2AnimSlots[4] = {0, 1, 1, 0};

static hdMsgOnly2Label_l sOnly2StartLabel; /* inAllEnter's function-local "dummy" */
static u32 sOnly2StartLabelReady;
static hdMsgOnly2Label_l sOnly2ContinueLabel; /* continueExecute's */
static u32 sOnly2ContinueLabelReady;
static hdUiSafeString sOnly2AllPaneName[1] = {};
static u32 sOnly2AllPaneNameReady;
static hdUiSafeString sOnly2TextPaneNames[4] = {};
static u32 sOnly2TextPaneNamesReady;

static hdUiSafeString sOnly2ArchiveNames[1] = {};
static hdUiSafeString sOnly2GroupNames[2] = {};
static hdUiSafeString sOnly2AnimNames[4] = {};
static hdPartsStateId_c sOnly2InAll;
static hdPartsStateId_c sOnly2OutNow;
static hdPartsStateId_c sOnly2CloseWait;
static hdPartsStateId_c sOnly2AutoCloseWait;
static hdPartsStateId_c sOnly2HandCloseWait;
static hdPartsStateId_c sOnly2NpcAnswerWait;
static hdPartsStateId_c sOnly2OutMsg;
static hdPartsStateId_c sOnly2InMsg;
static hdPartsStateId_c sOnly2Stop;
static hdPartsStateId_c sOnly2OutAll;
static hdPartsStateId_c sOnly2Continue;
static hdPartsStateId_c sOnly2Close;
static hdUiSafeString sOnly2LayoutName;
static u32 sOnly2StateNumberReady;
static s32 sOnly2StateNumber;

/* 026B777C */
// exit-time destruction of findAllPane's name
static void hdOnly2_destroyAllPaneName() {
    gabi::call(0x028F0164, gabi::ea(sOnly2AllPaneName), 1u, (u32)sizeof(hdUiSafeString), 0x026B980Cu, 0u, 0u);
}

/* 026B77A0 */
static void hdOnly2_destroyTextPaneNames() {
    gabi::call(0x028F0164, gabi::ea(sOnly2TextPaneNames), 4u, (u32)sizeof(hdUiSafeString), 0x026B980Cu, 0u, 0u);
}

/* 026B77C4 */
// hdScreenMsgOnly2_c::~hdScreenMsgOnly2_c() (GHS deleting destructor)
void hdScreenMsgOnly2_c::destroy(hdScreenMsgOnly2_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sOnly2HolderVtable;
    i_this->mSecondaryVtable = &sOnly2SecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* inlined: a stop slot (sead Buffer: an index out of range uses slot 0) */
static be<s32>* hdOnly2_stop(hdScreenMsgOnly2_c* i_this, u32 index) {
    return &i_this->mStops[index < 10 ? index : 0];
}

/* 026B7830 */
// the message up to the current stop in "T_Msg_00", shown; the next stop's offset filled in from the message
void hdScreenMsgOnly2_c::updateText() {
    hdMsgOnly2Message_l* message = mpMessage;
    hdMsgText_restartPlay_l(message, *hdOnly2_stop(this, mStopIndex), MSG_MAIN2_TEXT_CAPACITY);
    ((hdTextPane_c*)(void*)mpTextPanes[0].get())->prepareMessage((PaneWork_l*)(void*)mpMessage.get(), 0);
    hdMsgControl_setStatus_l(sOnly2MessageManager, mpMessage->mStatus);
    hdMsgMain2TextBox_l* text = mpTextPanes[0];
    text->mFlags = (text->mFlags & ~MSG_MAIN2_PANE_VISIBLE) + MSG_MAIN2_PANE_VISIBLE;
    u32 next = mStopIndex + 1;
    if (*hdOnly2_stop(this, next) < 0) {
        s32 offset = mpMessage->mNextStop;
        *hdOnly2_stop(this, next) = offset;
    }
}

/* 026B7920 */
// restart animation `animation` from its first frame
void hdScreenMsgOnly2_c::playAnimation(u32 animation) {
    u32 slot = sOnly2AnimSlots[animation];
    hdAnimSet_setSpeed_l(animSet(), animation, slot, 0.0f);
    hdAnimSet_play(animSet(), animation, slot, 1.0f);
}

/* inlined: the 4 unaligned colour bytes, read as a block (lswi) and written as a block (stswi) */
static u32 hdOnly2_loadColor(u32 src) {
    u32 value = 0;
    for (int i = 0; i < 4; i++) {
        value = (value << 8) | gabi::load<u8>(src + i);
    }
    return value;
}
static void hdOnly2_storeColor(u32 dst, u32 value) {
    for (int i = 0; i < 4; i++) {
        gabi::store<u8>(dst + i, (u8)(value >> (24 - 8 * i)));
    }
}
static void hdOnly2_copyColor(u32 dst, u32 src) {
    hdOnly2_storeColor(dst, hdOnly2_loadColor(src));
}

/* inlined: a function-local FixedSafeString<6> initialised with "dummy" */
static void hdOnly2_initLabel(hdMsgOnly2Label_l* label, be<u32>* ready, const char* text, u32 registration) {
    label->mBuffer[5] = 0;
    gabi::Local<hdUiSafeString> source;
    source->mVtable = &sOnly2SafeStringVtable;
    source->mText.v = gabi::guest_addr(text);
    label->mText = label->mBuffer;
    label->mCapacity = 6;
    label->mVtable = &sOnly2LabelVtable;
    *ready = 1;
    hdMsgOnly2_noop_l(source);
    u32 length = 0;
    u32 chars = gabi::ea(source->mText.get());
    if (gabi::load<u8>(chars) != 0) {
        for (;;) {
            length++;
            chars++;
            if ((s32)length > 0x40000) {
                length = 0;
                break;
            }
            if (gabi::load<u8>(chars) == 0) {
                break;
            }
        }
    }
    if ((s32)length >= (s32)label->mCapacity) {
        length = label->mCapacity - 1;
    }
    source->assureTermination();
    hdMsgOnly2_copyChars_l(label->mBuffer, source->mText.get(), length, 0);
    label->mBuffer[length] = 0;
    label->mVtable = &sOnly2LabelVtableDone;
    __register_global_object(registration);
}

/* inlined into inAllEnter and continueExecute: the message from its start, its first page shown, its voice */
void hdScreenMsgOnly2_c::startMessage(bool first) {
    for (int i = 0; i < 10; i++) {
        mStops[i] = -1;
    }
    mStopIndex = 0;
    mStops[0] = 0;
    hdMsgMain2TextBox_l* text = mpTextPanes[0];
    u32 color = hdOnly2_loadColor(gabi::ea(mTextColor));
    hdOnly2_storeColor(gabi::ea(&text->mColor), color);
    hdOnly2_storeColor(gabi::ea(&text->mColor) + 4, color);
    hdOnly2_copyColor(gabi::ea(mpMessage->mColors), gabi::ea(mTextColor));
    hdOnly2_copyColor(gabi::ea(mpMessage->mColors) + 4, gabi::ea(mTextColor));
    hdMsgText_setNumberIdle_l(mpMessage, mMessageId);
    updateText();
    if (first) {
        playAnimation(MSG_ONLY2_ANIM_IN_ALL);
    }
    hdMsgOnly2_talkIn_l();
    if (first) {
        hdMsgOnly2_seStart_l(0x823);
    }
    hdMsgOnly2Message_l* message = mpMessage;
    if (message->mpInput == NULL) {
        hdMsgOnly2Label_l* label = first ? &sOnly2StartLabel : &sOnly2ContinueLabel;
        be<u32>* ready = first ? &sOnly2StartLabelReady : &sOnly2ContinueLabelReady;
        if (!*ready) {
            hdOnly2_initLabel(label, ready, "dummy", first ? gabi::ea(&sOnly2StartLabelRegistration) : gabi::ea(&sOnly2ContinueLabelRegistration));
        }
        hdMsgResMgr_getMessage_l(sOnly2MessageResources, mpMessage, (hdUiSafeString*)label, mpMessage->mOut);
        message = mpMessage;
    }
    hdMsgMain2Input_l* input = message->mpInput;
    if (input != NULL && input->mVoice != 0) {
        s32 reverb = hdMsgOnly2_getReverb_l(sOnly2RoomNo);
        hdMsgOnly2_messageSe_l(input->mVoice, 0, reverb);
    }
}

/* 026B79A0 */
// in: the frame shown, the message from its start, the "in" sound
void hdScreenMsgOnly2_c::inAllEnter() {
    hdMsgMain2Pane_l* all = mpAllPane;
    all->mFlags = (all->mFlags & ~MSG_MAIN2_PANE_VISIBLE) + MSG_MAIN2_PANE_VISIBLE;
    startMessage(true);
}

/* 026B7C30 */
u32 hdScreenMsgOnly2_c::isAnimationEnd(u32 animation) {
    return hdAnimSet_isEnd(animSet(), animation);
}

/* 026B7C3C */
void hdScreenMsgOnly2_c::inAllExecute() {
    if (isAnimationEnd(MSG_ONLY2_ANIM_IN_ALL)) {
        changeState(&sOnly2OutNow);
    }
}

/* 026B7C84 */
void hdScreenMsgOnly2_c::outNowEnter() {
    play()->mMessageState = 6;
}

/* 026B7CAC */
// the message's status picks the next state: stop, a close wait (automatic or by hand), or an NPC's answer
void hdScreenMsgOnly2_c::outNowExecute() {
    u8 state = (u8)mpMessage->mStatus;
    play()->mMessageState = state;
    hdMsgOnly2Message_l* message = mpMessage;
    u32 status = message->mStatus;
    if (status == MSG_ONLY2_STATUS_CONTINUE) {
        changeState(&sOnly2Stop);
    } else if (status == MSG_ONLY2_STATUS_WAIT) {
        if (message->mAutoClose) {
            changeState(&sOnly2AutoCloseWait);
        } else if (message->mHandClose) {
            changeState(&sOnly2HandCloseWait);
        }
    } else if (status == MSG_ONLY2_STATUS_ANSWER) {
        changeState(&sOnly2NpcAnswerWait);
    }
}

/* 026B8000 */
void hdScreenMsgOnly2_c::npcAnswerWaitExecute() {
    u32 state = play()->mMessageState;
    if (state == 15) {
        changeState(&sOnly2Continue);
    } else if (state == 16) {
        changeState(&sOnly2CloseWait);
    } else {
        ((hdTextPane_c*)(void*)mpTextPanes[0].get())->refresh((PaneWork_l*)(void*)mpMessage.get());
    }
}

/* 026B8084 */
void hdScreenMsgOnly2_c::outMsgEnter() {
    playAnimation(MSG_ONLY2_ANIM_OUT_MSG);
    hdMsgOnly2_seStart_l(0x80B);
}

/* 026B80B0 */
void hdScreenMsgOnly2_c::outMsgExecute() {
    if (isAnimationEnd(MSG_ONLY2_ANIM_OUT_MSG)) {
        changeState(&sOnly2InMsg);
    }
}

/* 026B8180 */
void hdScreenMsgOnly2_c::inMsgExecute() {
    if (isAnimationEnd(MSG_ONLY2_ANIM_IN_MSG)) {
        changeState(&sOnly2OutNow);
    }
}

/* 026B82D8 */
void hdScreenMsgOnly2_c::outAllEnter() {
    playAnimation(MSG_ONLY2_ANIM_OUT_ALL);
    play()->mMessageState = 18;
}

/* 026B8308 */
void hdScreenMsgOnly2_c::outAllExecute() {
    if (isAnimationEnd(MSG_ONLY2_ANIM_OUT_ALL)) {
        changeState(&sOnly2Close);
    }
}

/* 026B8354 */
u32 hdScreenMsgOnly2_c::hasDecide() {
    if (mTrigger & 1) {
        return TRUE;
    }
    return FALSE;
}

/* 026B836C */
// on decide or cancel: the next message from its start (the advance sound, the advance counted)
void hdScreenMsgOnly2_c::continueExecute() {
    if (!hasDecide() && !(mTrigger & 2)) {
        ((hdTextPane_c*)(void*)mpTextPanes[0].get())->refresh((PaneWork_l*)(void*)mpMessage.get());
        sOnly2WaitIcon = 1;
        return;
    }
    sOnly2WaitIcon = 2;
    hdMsgOnly2_seStart_l(play()->mKind == 39 ? 0x880 : 0x80B);
    u32 count = mAdvanceCount + 1;
    mAdvanceCount = count;
    play()->mAdvanceCount = count;
    play()->mHandInput = 0;
    startMessage(false);
    changeState(&sOnly2OutNow);
}

/* 026B8670 */
void hdScreenMsgOnly2_c::setAnimationFrame(u32 animation, f32 frame) {
    hdAnimSet_setFrame(animSet(), animation, sOnly2AnimSlots[animation], frame);
}

/* 026B868C */
// no stops, the first two animations at their start, hidden
void hdScreenMsgOnly2_c::reset() {
    for (int i = 0; i < 10; i++) {
        mStops[i] = -1;
    }
    mStopIndex = 0;
    setAnimationFrame(MSG_ONLY2_ANIM_IN_ALL, 0.0f);
    setAnimationFrame(MSG_ONLY2_ANIM_OUT_MSG, 0.0f);
    vCalcLayout();
    vExecuteChildren();
    hdMsgMain2Pane_l* all = mpAllPane;
    all->mFlags = all->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
}

/* 026B8748 */
void hdScreenMsgOnly2_c::closeEnter() {
    hdMsgOnly2_talkOut_l();
    hdMsgOnly2_seStart_l(0x80D);
    reset();
}

/* 026B8784 */
// closing: once the message state says so, the answer is passed on (13: yes / no) and the screen hides
void hdScreenMsgOnly2_c::closeExecute() {
    if (play()->mMessageState != 17) {
        return;
    }
    u32 answer = play()->mAnswer;
    hdMsgOnly2Play_l* game = play();
    if (answer == 13) {
        game->mMessageState = 13;
        play()->mAnswerResult = 0;
    } else {
        game->mMessageState = 11;
        play()->mAnswerResult = 7;
    }
    play()->mAnswer = 11;
    changeState(&sOnly2PartsBlank);
}

/* 026B8840 */
// hdScreenMsgOnly2_c::hdScreenMsgOnly2_c() (GHS: allocates when called with NULL)
hdScreenMsgOnly2_c* hdScreenMsgOnly2_c::construct(hdScreenMsgOnly2_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenMsgOnly2_c*)operator_new(sizeof(hdScreenMsgOnly2_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sOnly2Vtable;
    i_this->mStateHolder.mHolderVtable = &sOnly2HolderVtable;
    i_this->mSecondaryVtable = &sOnly2SecondaryVtable;
    i_this->mMessageId = 0;
    i_this->mPressed = 0;
    i_this->mTrigger = 0;
    i_this->mHold = 0;
    // HD: GHS tests the address of each (trivially constructed) member array
    if (gabi::ea(&i_this->mpAllPane) == 0) {
        operator_new(sizeof(i_this->mpAllPane));
    }
    if (gabi::ea(i_this->mpTextPanes) == 0) {
        operator_new(sizeof(i_this->mpTextPanes));
    }
    i_this->m74 = 0;
    i_this->mAdvanceCount = 0;
    if (gabi::ea(i_this->mStops) == 0) {
        operator_new(sizeof(i_this->mStops));
    }
    i_this->mStopIndex = 0;
    i_this->mpAllPane = NULL;
    i_this->mpMessage = NULL;
    i_this->mA8 = 0;
    gabi::store<u32>(gabi::ea(i_this->mTextColor), 0xFFFFFFFF);
    for (int i = 0; i < 4; i++) {
        i_this->mpTextPanes[i] = NULL;
    }
    for (int i = 0; i < 10; i++) {
        i_this->mStops[i] = -1;
    }
    return i_this;
}

/* inlined: find a pane of the layout by name */
static hdMsgMain2Pane_l* hdOnly2_findPane(hdMsgMain2Pane_l* root, hdUiSafeString* name) {
    u32 assure = name->mVtable->mAssureTermination.get();
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    gabi::call_ptr(assure, name);
    return gabi::call_ptr<hdMsgMain2Pane_l*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)name->mText, 1u);
}

/* inlined: one element of a function-local static name array (constructed through its address) */
static void hdOnly2_setStaticName(hdUiSafeString* name, const char* text) {
    if (gabi::ea(name) == 0) {
        name = (hdUiSafeString*)operator_new(sizeof(hdUiSafeString));
        if (name == NULL) {
            return;
        }
    }
    hdParts_setSafeString(name, &sOnly2SafeStringVtable, text);
}

/* 026B8948 */
void hdScreenMsgOnly2_c::findAllPane() {
    if (!sOnly2AllPaneNameReady) {
        sOnly2AllPaneNameReady = 1;
        hdParts_setSafeString(&sOnly2AllPaneName[0], &sOnly2SafeStringVtable, "N_All_00");
        __register_global_object(gabi::ea(&sOnly2AllPaneNameRegistration));
    }
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    mpAllPane = hdOnly2_findPane(root, &sOnly2AllPaneName[0]);
}

/* 026B8A0C */
// the four text panes (room for 335 characters each), hidden; the message pane's colour kept
void hdScreenMsgOnly2_c::findTextPanes() {
    if (!sOnly2TextPaneNamesReady) {
        sOnly2TextPaneNamesReady = 1;
        hdParts_setSafeString(&sOnly2TextPaneNames[0], &sOnly2SafeStringVtable, "T_Msg_00");
        hdOnly2_setStaticName(&sOnly2TextPaneNames[1], "T_MsgLine1_00");
        hdOnly2_setStaticName(&sOnly2TextPaneNames[2], "T_ChoiseL_00");
        hdOnly2_setStaticName(&sOnly2TextPaneNames[3], "T_ChoiseR_00");
        __register_global_object(gabi::ea(&sOnly2TextPaneNamesRegistration));
    }
    hdMsgMain2Pane_l* root = (hdMsgMain2Pane_l*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < 4; i++) {
        mpTextPanes[i] = (hdMsgMain2TextBox_l*)hdOnly2_findPane(root, &sOnly2TextPaneNames[i]);
        if (mpTextPanes[i] != NULL) {
            ((hdTextPane_c*)(void*)mpTextPanes[i].get())->setCharacterCapacity(MSG_MAIN2_TEXT_CAPACITY);
            hdMsgMain2TextBox_l* text = mpTextPanes[i];
            text->mFlags = text->mFlags & ~MSG_MAIN2_PANE_VISIBLE;
        }
    }
    hdOnly2_copyColor(gabi::ea(mTextColor), gabi::ea(&mpTextPanes[0]->mColor));
}

/* 026B8C34 */
// registered with the message manager; the layout, its panes, the message, hidden
u32 hdScreenMsgOnly2_c::init(u32 arg1, u32 arg2, void* desc) {
    sOnly2MessageManager->mpMsgOnly2 = this;
    attachArchives(sOnly2ArchiveNames, 1);
    if (!vLoad(&sOnly2LayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) {
        return FALSE;
    }
    findAllPane();
    findTextPanes();
    mpMessage = (hdMsgOnly2Message_l*)(void*)sOnly2MessageManager->mMessage;
    reset();
    startState(&sOnly2PartsBlank);
    return TRUE;
}

/* 026B8D44 */
void hdScreenMsgOnly2_c::finalize() {
    sOnly2MessageManager->mpMsgOnly2 = NULL;
    play()->mAdvanceCount = 0;
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* inlined: whether the screen's current state is `state` */
static bool hdOnly2_isState(hdScreenMsgOnly2_c* i_this, hdPartsStateId_c* state) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(i_this->stateMachine());
    hdPartsStateIdVtbl_c* vtable = state->mVtable;
    u32 key = current->getKey();
    return key == gabi::call_ptr<u32>(vtable->mGetKey.get(), state);
}

/* 026B8DBC */
// while shown: the controller's buttons, then the state, the children and the layout
void hdScreenMsgOnly2_c::executeState() {
    if (hdOnly2_isState(this, &sOnly2PartsBlank)) {
        return;
    }
    play()->mAdvanceCount = 0;
    hdInputManager* input = sOnly2Input;
    if (input != NULL) {
        hdMsgOnly2Pad_l* pad = (hdMsgOnly2Pad_l*)gabi::at<u8>(gabi::load<u32>(gabi::ea(input)));
        if (pad != NULL) {
            mHold = pad->mHold;
            mTrigger = pad->mTrigger;
            mPressed = pad->mTrigger | pad->mRepeat;
        }
    }
    hdParts_executeStateMachine(stateMachine());
    vExecuteChildren();
    vCalcLayout();
}

/* 026B8EAC */
void hdScreenMsgOnly2_c::draw(void* info) {
    if (!hdOnly2_isState(this, &sOnly2PartsBlank)) {
        vDrawLayout(info);
        vDrawChildren(info);
    }
}

/* 026B8F60 */
// closed: hidden again, the play's message fields cleared
void hdScreenMsgOnly2_c::closeFinish() {
    reset();
    play()->mAnswer = 0;
    play()->mMessageState = 0;
    play()->mDemoMode = 0;
    play()->mZoomActive = 0;
    play()->mAnswerResult = 0;
    changeState(&sOnly2PartsBlank);
}

/* 026B8FD0 */
// a message: a hidden screen opens with it; while an NPC's answer is awaited a different message replaces it
u8 hdScreenMsgOnly2_c::open(u32 arg, u32 messageId) {
    if (hdOnly2_isState(this, &sOnly2PartsBlank)) {
        mMessageId = messageId;
        mAdvanceCount = 0;
        mpTextPanes[0]->mShrink = 0;
        f32 width = mpTextPanes[0]->mWidth;
        mpMessage->mWidth = width;
        changeState(&sOnly2InAll);
        return TRUE;
    }
    if (hdOnly2_isState(this, &sOnly2NpcAnswerWait) && messageId != mMessageId) {
        mAdvanceCount = 0;
        mMessageId = messageId;
        return TRUE;
    }
    return FALSE;
}

/* 026B90D4 */
u32 hdScreenMsgOnly2_c::createChild(hdPartsDesc_c* desc) {
    return TRUE;
}

/* 026B90DC */
u32 hdScreenMsgOnly2_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sOnly2UiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 4, 2, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 4; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sOnly2AnimNames[i], &sOnly2GroupNames[sOnly2AnimSlots[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026B9208 */
static void __sinit_hd_screen_msg_only2_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sOnly2HeaderStatics), gabi::ea(&sOnly2HeaderDescriptors), gabi::ea(&sOnly2HeaderZero),
                                gabi::ea(&sOnly2PiConstants));
    hdParts_setSafeString(&sOnly2ArchiveNames[0], &sOnly2SafeStringVtable, "MsgOnly_00");
    hdParts_setSafeString(&sOnly2GroupNames[0], &sOnly2SafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sOnly2GroupNames[1], &sOnly2SafeStringVtable, "G_Msg_00");
    hdParts_setSafeString(&sOnly2AnimNames[0], &sOnly2SafeStringVtable, "InAll");
    hdParts_setSafeString(&sOnly2AnimNames[1], &sOnly2SafeStringVtable, "OutMsg");
    hdParts_setSafeString(&sOnly2AnimNames[2], &sOnly2SafeStringVtable, "InMsg");
    hdParts_setSafeString(&sOnly2AnimNames[3], &sOnly2SafeStringVtable, "OutAll");
    s32 number;
    if (sOnly2StateNumberReady) {
        number = sOnly2StateNumber;
    } else {
        number = 0;
        sOnly2StateNumberReady = 1;
    }
    sOnly2InAll.setup(number + 1, "::StateID_InAll", &sOnly2StateIdVtable, 0x19, &sOnly2NoStateId);
    sOnly2OutNow.setup(number + 2, "::StateID_OutNow", &sOnly2StateIdVtable, 0x1C, &sOnly2NoStateId);
    sOnly2CloseWait.setup(number + 3, "::StateID_CloseWait", &sOnly2StateIdVtable, 0x1F, &sOnly2NoStateId);
    sOnly2AutoCloseWait.setup(number + 4, "::StateID_AutoCloseWait", &sOnly2StateIdVtable, 0x22, &sOnly2NoStateId);
    sOnly2HandCloseWait.setup(number + 5, "::StateID_HandCloseWait", &sOnly2StateIdVtable, 0x25, &sOnly2NoStateId);
    sOnly2NpcAnswerWait.setup(number + 6, "::StateID_NpcAnswerWait", &sOnly2StateIdVtable, 0x28, &sOnly2NoStateId);
    sOnly2OutMsg.setup(number + 7, "::StateID_OutMsg", &sOnly2StateIdVtable, 0x2B, &sOnly2NoStateId);
    sOnly2InMsg.setup(number + 8, "::StateID_InMsg", &sOnly2StateIdVtable, 0x2E, &sOnly2NoStateId);
    sOnly2Stop.setup(number + 9, "::StateID_Stop", &sOnly2StateIdVtable, 0x31, &sOnly2NoStateId);
    sOnly2OutAll.setup(number + 10, "::StateID_OutAll", &sOnly2StateIdVtable, 0x34, &sOnly2NoStateId);
    sOnly2Continue.setup(number + 11, "::StateID_Continue", &sOnly2StateIdVtable, 0x37, &sOnly2NoStateId);
    sOnly2Close.setup(number + 12, "::StateID_Close", &sOnly2StateIdVtable, 0x3A, &sOnly2NoStateId);
    sOnly2StateNumber = number + 12;
    hdParts_setSafeString(&sOnly2LayoutName, &sOnly2SafeStringVtable, "MsgOnly_00");
}

/* WWHD glue
 data sOnly2AllPaneNameRegistration 101F66CC nocheck
 data sOnly2TextPaneNamesRegistration 101F66D8 nocheck
 data sOnly2StartLabelRegistration 101F66B4 nocheck
 data sOnly2ContinueLabelRegistration 101F66C0 nocheck
 data sOnly2HeaderStatics 104987EC nocheck
 data sOnly2HeaderDescriptors 101F66E4 nocheck
 data sOnly2HeaderZero 10498818 nocheck
 data sOnly2PiConstants 100FEE88 nocheck
data sOnly2Vtable 100FEFC0 nocheck
data sOnly2HolderVtable 100FF1B0 nocheck
data sOnly2SecondaryVtable 100FF1C0 nocheck
data sOnly2SafeStringVtable 100FED24 nocheck
data sOnly2StateIdVtable 100FEDDC nocheck
data sOnly2NoStateId 101FF32C nocheck
data sOnly2PartsBlank 1049DD60 nocheck
data sOnly2UiSystem 1018C404 nocheck
data sOnly2MessageManager 101F4B5C nocheck
data sOnly2MessageResources 101F4AE8 nocheck
data sOnly2Input 101F5088 nocheck
data sOnly2WaitIcon 1047B09E nocheck
data sOnly2RoomNo 1047E6C8 nocheck
data sOnly2LabelVtable 100FED54 nocheck
data sOnly2LabelVtableDone 100FED6C nocheck
data sOnly2AnimSlots 100FED84
data sOnly2StartLabel 10498858 nocheck
data sOnly2StartLabelReady 10498880 nocheck
data sOnly2ContinueLabel 1049886C nocheck
data sOnly2ContinueLabelReady 10498884 nocheck
data sOnly2AllPaneName 104987DC nocheck
data sOnly2AllPaneNameReady 10498888 nocheck
data sOnly2TextPaneNames 104987F8 nocheck
data sOnly2TextPaneNamesReady 1049888C nocheck
data sOnly2ArchiveNames 104987E4 nocheck
data sOnly2GroupNames 10498828 nocheck
data sOnly2AnimNames 10498838 nocheck
data sOnly2InAll 10498890 nocheck
data sOnly2OutNow 104988C0 nocheck
data sOnly2CloseWait 104988F0 nocheck
data sOnly2AutoCloseWait 10498920 nocheck
data sOnly2HandCloseWait 10498950 nocheck
data sOnly2NpcAnswerWait 10498980 nocheck
data sOnly2OutMsg 104989B0 nocheck
data sOnly2InMsg 104989E0 nocheck
data sOnly2Stop 10498A10 nocheck
data sOnly2OutAll 10498A40 nocheck
data sOnly2Continue 10498A70 nocheck
data sOnly2Close 10498AA0 nocheck
data sOnly2LayoutName 10498AD0 nocheck
data sOnly2StateNumberReady 101FD8E4 nocheck
data sOnly2StateNumber 101FDD50 nocheck
str 100FEE30 "dummy" in 026B79A0
str 100FEE38 "dummy" in 026B836C
str 100FEE40 "N_All_00" in 026B8948
str 100FEE5C "T_Msg_00" in 026B8A0C
str 100FEE4C "T_MsgLine1_00" in 026B8A0C
str 100FEE68 "T_ChoiseL_00" in 026B8A0C
str 100FEE78 "T_ChoiseR_00" in 026B8A0C
str 100FEF54 "MsgOnly_00" in 026B9208
str 100FEF3C "G_All_00" in 026B9208
str 100FEF48 "G_Msg_00" in 026B9208
str 100FEE90 "InAll" in 026B9208
str 100FEEA0 "OutMsg" in 026B9208
str 100FEE98 "InMsg" in 026B9208
str 100FEEA8 "OutAll" in 026B9208
str 100FEEB0 "::StateID_InAll" in 026B9208
str 100FEF60 "::StateID_OutNow" in 026B9208
str 100FEF28 "::StateID_CloseWait" in 026B9208
str 100FEEC0 "::StateID_AutoCloseWait" in 026B9208
str 100FEED8 "::StateID_HandCloseWait" in 026B9208
str 100FEEF0 "::StateID_NpcAnswerWait" in 026B9208
str 100FEF74 "::StateID_OutMsg" in 026B9208
str 100FEF08 "::StateID_InMsg" in 026B9208
str 100FEF88 "::StateID_Stop" in 026B9208
str 100FEF98 "::StateID_OutAll" in 026B9208
str 100FEFAC "::StateID_Continue" in 026B9208
str 100FEF18 "::StateID_Close" in 026B9208
ext 02704504 void hdTextPane_c::prepareMessage(PaneWork_l*, u32)
ext 0270477C void hdTextPane_c::refresh(PaneWork_l*)
*/
