// Message-window input, selection, opening and closing states.
// HD-only: reconstructed from the original executable; equivalence gates pending.
#include "d/hd_msg_window_states.h"
using namespace gabi;
static hdPartsStateId_c sMessageDefinitionCameraToOpen;
static hdPartsStateId_c sMessageDefinitionOpen;
static hdPartsStateId_c sMessageDefinitionStop;
static hdPartsStateId_c sMessageDefinitionSelectStartWait;
static hdPartsStateId_c sMessageDefinitionSelect2;
static hdPartsStateId_c sMessageDefinitionSelect3;
static hdPartsStateId_c sMessageDefinitionSelectOut;
static hdPartsStateId_c sMessageDefinitionSelectOutContinue;
static hdPartsStateId_c sMessageDefinitionKeyIconInAnimEndWaitToCloseWait;
static hdPartsStateId_c sMessageDefinitionCloseWait;
static hdPartsStateId_c sMessageDefinitionKeyIconInAnimEndWaitToContinue;
static hdPartsStateId_c sMessageDefinitionContinue;
static hdPartsStateId_c sMessageDefinitionDelete;
static hdPartsStateId_c sMessageDefinitionKeyIconDecideAnimEndWaitToNpcAnswerWait;
static hdPartsStateId_c sMessageDefinitionCloseAfterInput;

extern void statesChange(be<u32>*, WindowState*);
extern void statesUpdatePane(WindowPane*, WindowMessage*);
extern u32 statesGetStatus(WindowCore*);
extern void statesSetStatus(WindowCore*, u32);
extern void statesAcceptNumber();
extern u32 statesPlaySound(u32);
extern u32 statesHasInput(WindowBase*);
static u32 virtualState(hdMsgWindowStates_c* p, u32 slot) { return p->mpVTable[slot / 4].get(); }

/* 026FCE3C */
void hdMsgWindowStates_c::enterStop() {
    call_ptr<void>(virtualState(this, 0x4AC), this);
}

/* 026FD13C */
void hdMsgWindowStates_c::enterWaitEnd() {
    call_ptr<void>(virtualState(this, 0x4AC), this);
}

/* 026FD35C */
void hdMsgWindowStates_c::enterWaitInputEnd() {
    call_ptr<void>(virtualState(this, 0x4AC), this);
}

/* 026FE57C */
void hdMsgWindowStates_c::enterAutoWait() {
    call_ptr<void>(virtualState(this, 0x4AC), this);
}

/* 026FE6EC */
void hdMsgWindowStates_c::enterAutoWaitEnd() {
    call_ptr<void>(virtualState(this, 0x4AC), this);
}

/* 026FEA04 */
void hdMsgWindowStates_c::enterHandStop() {
    call_ptr<void>(virtualState(this, 0x4AC), this);
}

/* 026FEE68 */
void hdMsgWindowStates_c::enterClose() {
    call_ptr<void>(virtualState(this, 0x4B4), this);
}

/* 026FEEC8 */
void hdMsgWindowStates_c::enterCloseBlocked() {
    call_ptr<void>(virtualState(this, 0x4BC), this);
}

/* 026FD36C */
void hdMsgWindowStates_c::updateWaitInputEnd() {
    if (call_ptr<u32>(virtualState(this, 0x4C4), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionSelectStartWait));
}

/* 026FDD74 */
void hdMsgWindowStates_c::updateSelectionConfirm() {
    if (call_ptr<u32>(virtualState(this, 0x49C), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionDelete));
}

/* 026FECAC */
void hdMsgWindowStates_c::updateAccept() {
    if (call_ptr<u32>(virtualState(this, 0x474), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionDelete));
}

/* 026FEE78 */
void hdMsgWindowStates_c::updateClose() {
    if (call_ptr<u32>(virtualState(this, 0x4D4), this))
        statesChange(&mStateMachine, &l_windowState0_E40C);
}

/* 026FEED8 */
void hdMsgWindowStates_c::updateCloseBlocked() {
    if (call_ptr<u32>(virtualState(this, 0x4CC), this))
        statesChange(&mStateMachine, &l_windowState0_DE9C);
}

/* 026FF130 */
void hdMsgWindowStates_c::updateFinishClose() {
    if (call_ptr<u32>(virtualState(this, 0x4D4), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionCloseAfterInput));
}

/* 026FCE4C */
void hdMsgWindowStates_c::updateStopEnd() {
    if (call_ptr<u32>(virtualState(this, 0x4C4), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionStop));
    else
        statesUpdatePane(mpTextPane.get(), mpMessage.get());
}

/* 026FE58C */
void hdMsgWindowStates_c::updateAutoStopEnd() {
    if (call_ptr<u32>(virtualState(this, 0x4C4), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionCloseWait));
    else
        statesUpdatePane(mpTextPane.get(), mpMessage.get());
}

/* 026FCEBC */
void hdMsgWindowStates_c::blockOpen() {
    mOpenBlocked = 1;
}

/* 026FD968 */
void hdMsgWindowStates_c::leaveNumberSelect() {
    updateSelectionLayout();
}

/* 026FD96C */
void hdMsgWindowStates_c::enterSelectTwo() {
    mStopReady = 1;
    mSelection = 0;
}

/* 026FDAF4 */
void hdMsgWindowStates_c::enterSelectThree() {
    mStopReady = 1;
    mSelection = 0;
}

/* 026FDE88 */
void hdMsgWindowStates_c::resetNumberSelection() {
    gfn* table = mpVTable.get();
    mSelection = 0;
    call_ptr<void>(table[0x434 / 4].get(), this);
}

/* 026FE8C0 */
void hdMsgWindowStates_c::resetCloseRequest() {
    mpMessage.get()->mCloseRequested = 0;
}

/* 026FE9E4 */
void hdMsgWindowStates_c::clearCurrentNextMessage() {
    WindowMessage* message = mpMessage.get();
    if (message->mNextMessageNumber == message->mMessageNumber)
        message->mNextMessageNumber = 0;
}

/* 026FCA1C */
void hdMsgWindowStates_c::updateCameraToOpen() {
    if (call_ptr<u32>(virtualState(this, 0x464), this)) {
        call_ptr<void>(virtualState(this, 0x47C), this);
        statesChange(&mStateMachine, &l_windowState0_DEFC);
    }
}

/* 026FD3C0 */
void hdMsgWindowStates_c::updateStopInput() {
    if (statesHasInput(this) || (mInputFlags & WINDOW_INPUT_SKIP))
        statesChange(&mStateMachine, &l_windowState0_E07C);
}

/* 026FEC74 */
void hdMsgWindowStates_c::enterAccept() {
    call_ptr<void>(virtualState(this, 0x46C), this);
    statesAcceptNumber();
    statesPlaySound(0x80D);
}

/* 026FF180 */
void hdMsgWindowStates_c::enterFinishAccept() {
    call_ptr<void>(virtualState(this, 0x46C), this);
    statesAcceptNumber();
    statesPlaySound(0x80D);
}

/* 026FED74 */
void hdMsgWindowStates_c::enterAdvanceClose() {
    call_ptr<void>(virtualState(this, 0x4B4), this);
    statesPlaySound(0x80B);
}

/* 026FEFF4 */
void hdMsgWindowStates_c::enterFinishAdvance() {
    call_ptr<void>(virtualState(this, 0x4B4), this);
    statesPlaySound(0x80B);
}

/* 026FECFC */
void hdMsgWindowStates_c::enterNextMessage() {
    statesSetStatus(l_windowCore.get(), 18);
}

/* 026FED0C */
void hdMsgWindowStates_c::updateNextMessage() {
    WindowCore* core = l_windowCore.get();
    if (statesGetStatus(core) == 19) {
        statesSetStatus(core, 0);
        call_ptr<void>(virtualState(this, 0x41C), this);
    }
}

/* 026FF1B8 */
void hdMsgWindowStates_c::updateFinishAccept() {
    if (call_ptr<u32>(virtualState(this, 0x474), this)) {
        statesSetStatus(l_windowCore.get(), 14);
        call_ptr<void>(virtualState(this, 0x41C), this);
    }
}

/* 026FEF9C */
void hdMsgWindowStates_c::updateAdvanceStop() {
    if (call_ptr<u32>(virtualState(this, 0x4D4), this)) {
        mStopReady = 0;
        statesChange(&mStateMachine, &l_windowState0_E34C);
    }
}

/* 026FDC98 */
void hdMsgWindowStates_c::updateNumberMode() {
    u32 mode = statesGetStatus(l_windowCore.get());
    if (mode == 15)
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionSelectOutContinue));
    else if (mode == 16)
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionSelectOut));
}


/* inlined into message-window selection states */
static MsgWindowPanes_l* windowPanes(hdMsgWindowStates_c* window) { return reinterpret_cast<MsgWindowPanes_l*>(window); }

/* 026FDD14 */
void hdMsgWindowStates_c::enterSelectionConfirm() {
    call_ptr<void>(virtualState(this, 0x46C), this);
    call_ptr<void>(virtualState(this, 0x494), this);
    mStopReady = 0;
    statesAcceptNumber();
    statesPlaySound(WINDOW_SOUND_ACCEPT);
}

/* 026FDDC4 */
void hdMsgWindowStates_c::hideSelectionBackground() {
    WindowPane* pane = windowPanes(this)->mpSelectionBackground.get();
    pane->mFlags = pane->mFlags & 0xFE;
}

/* 026FDDD8 */
void hdMsgWindowStates_c::enterSelectionClose() {
    call_ptr<void>(virtualState(this, 0x494), this);
    WindowPane* pane = mpTextPane.get();
    pane->mFlags = pane->mFlags & 0xFE;
    mStopReady = 0;
}

/* 026FDE28 */
void hdMsgWindowStates_c::updateSelectionClose() {
    if (call_ptr<u32>(virtualState(this, 0x49C), this)) {
        WindowPane* pane = windowPanes(this)->mpSelectionBackground.get();
        pane->mFlags = pane->mFlags & 0xFE;
        statesChange(&mStateMachine, &l_windowState0_DE9C);
    }
}

/* 026FD6E8 */
void hdMsgWindowStates_c::updateSelectionLayout() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 mode = mpMessage.get()->mState;
    if (mode == 8) positionTwoChoices();
    else if (mode == 9) positionThreeChoices();
}

extern ChoiceFont_l* choiceGetFont(ChoiceTextPane_l*);
extern void choiceSetPositions(WindowCore*, ChoicePositions_l*, void*, ChoiceFont_l*, f32);
extern void choiceSetCursor(WindowCore*, WindowPane*);
/* inlined into selection position methods */
static void positionChoices(hdMsgWindowStates_c* window, bool three) {
    MsgWindowPanes_l* panes = windowPanes(window);
    auto* pane = reinterpret_cast<ChoiceTextPane_l*>(three ? panes->mpThreeChoices.get() : panes->mpTwoChoices.get());
    FrameLocal<ChoicePositions_l> positions(8);
    FrameLocal<be<u32>[12]> matrix(0x38);
    if (ea(pane->mMatrix) != ea(matrix.get()))
        for (u32 i = 0; i<12; ++i) (*matrix)[i] = pane->mMatrix[i];
    f32 halfHeight = fmuls_ppc(pane->mHeight, 0.5f);
    f32 center = three ? fadds_ppc(f32(*reinterpret_cast<be<f32>*>(&(*matrix)[7])), halfHeight) : fmadds(pane->mHeight, 0.5f, f32(*reinterpret_cast<be<f32>*>(&(*matrix)[7])));
    f32 fontHeight = pane->mFontHeight;
    f32 spacing = pane->mLineSpace;
    f32 fontWidth = pane->mFontWidth;
    f32 left = *reinterpret_cast<be<f32>*>(&(*matrix)[3]);
    positions->mPositions[1] = fnmsubs(fontHeight, 0.5f, center);
    positions->mPositions[4] = fsubs_ppc(fnmsubs(fontHeight, 1.5f, center), spacing);
    positions->mPositions[0] = left;
    positions->mPositions[2] = 0.0f;
    positions->mPositions[3] = left;
    positions->mPositions[5] = 0.0f;
    if (three) {
        positions->mPositions[6] = left;
        positions->mPositions[7] = fsubs_ppc(fnmsubs(fontHeight, 2.5f, center), fadds_ppc(spacing, spacing));
    } else {
        positions->mPositions[6] = 0.0f;
        positions->mPositions[7] = 0.0f;
    }
    positions->mPositions[8] = 0.0f;
    ChoiceFont_l* font = choiceGetFont(pane);
    s32 width = call_ptr<s32>(font->mpVTable[0x1C/4].get(), font);
    f32 scale = f32(fontWidth / f32(width));
    void* messagePositions = reinterpret_cast<MsgChoiceMessage_l*>(window->mpMessage.get())->mChoicePositions;
    choiceSetPositions(l_windowCore.get(), positions.get(), messagePositions, font, scale);
    WindowCore* core = l_windowCore.get();
    WindowPane* cursor = three ? windowPanes(window)->mpThreeChoiceCursor.get() : windowPanes(window)->mpTwoChoiceCursor.get();
    choiceSetCursor(core, cursor);
}
/* 026FD408 */
void hdMsgWindowStates_c::positionTwoChoices() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    positionChoices(this, false);
}
/* 026FD56C */
void hdMsgWindowStates_c::positionThreeChoices() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    positionChoices(this, true);
}

extern WindowPlay* statesGetPlay();
extern void statesUpdateBase(hdMsgWindowStates_c*);
extern void statesResetAnimator(void*, f32);
extern void statesStartAnimator(void*, u32, u32, f32);
extern void statesSetAnimatorFrame(void*, u32, u32, f32);
extern u32 statesAnimatorFinished(void*, u32);

/* 026FE600 */
void hdMsgWindowStates_c::updateAutoStopInput() {
    bool confirm = statesHasInput(this) != 0;
    if (!confirm) confirm = (mInputFlags & WINDOW_INPUT_SKIP) != 0;
    if (!confirm) confirm = mpMessage.get()->mConfirmInput != 0;
    if (confirm) {
        l_windowInputMode = 0;
        statesChange(&mStateMachine, &l_windowState0_E43C);
    } else {
        statesUpdatePane(mpTextPane.get(), mpMessage.get());
        l_windowInputMode = 4;
    }
}

/* 026FE8D0 */
void hdMsgWindowStates_c::updateHandMode() {
    u32 status = statesGetStatus(l_windowCore.get());
    if (status == 15) {
        if (mpMessage.get()->mCloseRequested && !statesGetPlay()->mInputBlocked &&
            !mpMessage.get()->mCloseBlocked) statesChange(&mStateMachine, &l_windowState0_DE9C);
        else statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionKeyIconInAnimEndWaitToContinue));
    } else if (status == 16) {
        WindowMessage* message = mpMessage.get();
        if (message->mTimedWait) {
            message->mTimedWait = 0;
            statesChange(&mStateMachine, &l_windowState0_E34C);
        } else statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionKeyIconInAnimEndWaitToCloseWait));
    } else statesUpdatePane(mpTextPane.get(), mpMessage.get());
}

/* 026FEA14 */
void hdMsgWindowStates_c::updateHandStopEnd() {
    if (mpMessage.get()->mCloseRequested && !statesGetPlay()->mInputBlocked &&
        !mpMessage.get()->mCloseBlocked) {
        call_ptr<void>(virtualState(this, 0x4A4), this);
        statesChange(&mStateMachine, &l_windowState0_DE9C);
    } else if (call_ptr<u32>(virtualState(this, 0x4C4), this))
        statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionContinue));
    else statesUpdatePane(mpTextPane.get(), mpMessage.get());
}

/* 026FF39C */
void hdMsgWindowStates_c::update() {
    if (statesGetPlay()->mInputBlocked) statesGetPlay()->mInputBlocked = 0;
    reinterpret_cast<MsgPlayExtension_l*>(statesGetPlay())->mMessageActive = 0;
    statesGetPlay()->mAdvanceCount = 0;
    call_ptr<void>(virtualState(this, 0xAC), this);
    call_ptr<void>(virtualState(this, 0x84), this);
    statesUpdateBase(this);
}

/* 026FF5EC */
void hdMsgWindowStates_c::startAnimation(u32 group, u32 animation) {
    void* animator = windowPanes(this)->mpLayout.get()->animSet();
    statesResetAnimator(animator, 0.0f);
    animator = windowPanes(this)->mpLayout.get()->animSet();
    statesStartAnimator(animator, group, animation, 1.0f);
}

/* 026FF65C */
void hdMsgWindowStates_c::setAnimationFrame(u32 group, u32 animation, f32 frame) {
    statesSetAnimatorFrame(windowPanes(this)->mpLayout.get()->animSet(), group, animation, frame);
}

/* 026FF668 */
u32 hdMsgWindowStates_c::animationFinished(u32 group) {
    return statesAnimatorFinished(windowPanes(this)->mpLayout.get()->animSet(), group);
}

static PartGlobals_l sMessageGlobals;
static PartRegistration_l sMessageRegistrationA;
static PartRegistration_l sMessageRegistrationB;
static PartRegistration_l sMessageRegistrationC;
using MessageWord_l = be<u32>;
static MessageWord_l sMessageStateIdGuard;
static MessageWord_l sMessageStateId;
static u8 sMessageStateVTable;
static char sMessageStateParentName;
extern void messageRegister(PartRegistration_l*);
extern void messagePrimaryLock(be<u8>*);
extern void messageSecondaryLock(be<u8>*);
/* 026FFA48 */
void initializeMessageWindowStates() {
 sMessageGlobals.mHead = 0;
 sMessageGlobals.mOwner = 0;
 sMessageGlobals.mCount = 0;
 sMessageGlobals.mTail = 0;
 messageRegister(&sMessageRegistrationA);
 sMessageGlobals.mMinimumAngle = 0xC0490FDB;
 sMessageGlobals.mMaximumAngle = 0x40490FDB;
 messagePrimaryLock(&sMessageGlobals.mPrimaryLock);
 messageRegister(&sMessageRegistrationB);
 messageSecondaryLock(&sMessageGlobals.mSecondaryLock);
 messageRegister(&sMessageRegistrationC);
 u32 id = 0;
 if (sMessageStateIdGuard) id = sMessageStateId;
 else sMessageStateIdGuard = 1;
 sMessageDefinitionCameraToOpen.setup(id + 1, "::StateID_CameraToOpen", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 25, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_DE9C)->setup(id + 2, "::StateID_CameraToOutNow", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 28, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionOpen.setup(id + 3, "::StateID_Open", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 31, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_DEFC)->setup(id + 4, "::StateID_OutNow", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 34, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_DF2C)->setup(id + 5, "::StateID_KeyIconInAnimEndWaitToStop", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 37, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionStop.setup(id + 6, "::StateID_Stop", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 40, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_DF8C)->setup(id + 7, "::StateID_AutoStop", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 43, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_DFBC)->setup(id + 8, "::StateID_KeyIconInAnimEndWaitToHandStop", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 46, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_DFEC)->setup(id + 9, "::StateID_HandStop", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 49, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E01C)->setup(id + 10, "::StateID_KeyIconInAnimEndWaitToSelectStartWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 52, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionSelectStartWait.setup(id + 11, "::StateID_SelectStartWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 55, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E07C)->setup(id + 12, "::StateID_SelectIn", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 58, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionSelect2.setup(id + 13, "::StateID_Select2", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 61, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionSelect3.setup(id + 14, "::StateID_Select3", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 64, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E10C)->setup(id + 15, "::StateID_SelectNpcAnswerWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 67, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionSelectOut.setup(id + 16, "::StateID_SelectOut", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 70, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionSelectOutContinue.setup(id + 17, "::StateID_SelectOutContinue", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 73, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E19C)->setup(id + 18, "::StateID_Input", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 76, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionKeyIconInAnimEndWaitToCloseWait.setup(id + 19, "::StateID_KeyIconInAnimEndWaitToCloseWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 79, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionCloseWait.setup(id + 20, "::StateID_CloseWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 82, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E22C)->setup(id + 21, "::StateID_AutoCloseWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 85, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E25C)->setup(id + 22, "::StateID_KeyIconInAnimEndWaitToHandCloseWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 88, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E28C)->setup(id + 23, "::StateID_HandCloseWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 91, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E2BC)->setup(id + 24, "::StateID_NpcAnswerWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 94, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionKeyIconInAnimEndWaitToContinue.setup(id + 25, "::StateID_KeyIconInAnimEndWaitToContinue", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 97, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionContinue.setup(id + 26, "::StateID_Continue", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 100, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E34C)->setup(id + 27, "::StateID_Close", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 103, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionDelete.setup(id + 28, "::StateID_Delete", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 106, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E3AC)->setup(id + 29, "::StateID_KeyIconDecideAnimEndWaitToOutNow", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 109, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E3DC)->setup(id + 30, "::StateID_KeyIconDecideAnimEndWaitToOutNow2", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 112, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E40C)->setup(id + 31, "::StateID_KeyIconOutAnimEndWaitToOutNow", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 115, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E43C)->setup(id + 32, "::StateID_KeyIconDecideAnimEndWaitToClose", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 118, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionKeyIconDecideAnimEndWaitToNpcAnswerWait.setup(id + 33, "::StateID_KeyIconDecideAnimEndWaitToNpcAnswerWait", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 121, at<hdStateId>(ea(&sMessageStateParentName)));
 windowStateId(&l_windowState0_E49C)->setup(id + 34, "::StateID_KeyIconDecideAnimEndWaitToCloseAfterInput", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 124, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageDefinitionCloseAfterInput.setup(id + 35, "::StateID_CloseAfterInput", at<hdPartsStateIdVtbl_c>(ea(&sMessageStateVTable)), 127, at<hdStateId>(ea(&sMessageStateParentName)));
 sMessageStateId = id + 35;
}

extern void* messageAllocate(u32);
extern void messageConstructBase(hdMsgWindowStates_c*);
static u8 sMessageBaseVTable;
static u8 sMessageReceiverVTable;
static u8 sMessageStateMachineVTable;
struct MessagePosition_l { be<u32> mValues[3]; };
static MessagePosition_l sMessageDefaultPosition;
static MsgIdleState_l sMessageIdleState;
extern MsgIdleState_l* messageCurrentState(be<u32>*);
/* inlined into message-window state comparison */
static bool messageIsIdle(hdMsgWindowStates_c* window) {
 MsgIdleState_l* current = messageCurrentState(&window->mStateMachine);
 u32 fn = current->mpNameVTable[0x14/4].get();
 gfn* expected = sMessageIdleState.mpNameVTable.get();
 u32 currentId = call_ptr<u32>(fn, current);
 u32 idleId = call_ptr<u32>(expected[0x14/4].get(), &sMessageIdleState);
 return currentId == idleId;
}

/* 026FF21C */
hdMsgWindowStates_c* hdMsgWindowStates_c::construct(hdMsgWindowStates_c* window) {
    if (!window) window = static_cast<hdMsgWindowStates_c*>(messageAllocate(0x11C));
    if (window) {
        messageConstructBase(window);
        auto* fields = reinterpret_cast<MsgInitialFields_l*>(window);
        window->mInputFlags = 0;
        window->mMessageId = 0;
        window->mWaitTimer = 0;
        window->mTypingAccumulator = 0.0f;
        window->mpVTable = at<gfn>(ea(&sMessageBaseVTable));
        fields->mHeldInput = 0;
        fields->mpThreeCursor = nullptr;
        fields->mpTwo = nullptr;
        fields->mpThree = nullptr;
        window->mAdvanceCount = 0;
        window->mWaitValue = 0;
        fields->mWindowMode = 0;
        fields->mpNumber = nullptr;
        fields->mpTwoCursor = nullptr;
        fields->mReceiverVTable = ea(&sMessageReceiverVTable);
        window->mTypingFlags = 0;
        window->mCharacterCount = 0;
        fields->mpText = nullptr;
        fields->mpBackground = nullptr;
        fields->mStateVTable = ea(&sMessageStateMachineVTable);
        fields->mNumberValue = 0;
        if (!ea(window->mStopOffsets)) messageAllocate(0x28);
        if (!ea(window->mStopCounts)) messageAllocate(0x28);
        fields->mStopReady = 0;
        fields->mReady = 0;
        window->mStopIndex = 0;
        fields->mWaitKind = 0;
        window->mSelection = 0;
        for (u32 i = 0; i<3; ++i) fields->mPosition[i] = sMessageDefaultPosition.mValues[i];
        fields->mFrame = 0.0f;
        fields->mHeight = 0.0f;
        fields->mBlocked = 0;
        fields->mpMessage = nullptr;
        fields->mColor = -1;
        fields->mOpen = 0;
        fields->mWidth = 0.0f;
        for (u32 i = 0; i<10; ++i) window->mStopOffsets[i] = -1;
        for (u32 i = 0; i<10; ++i) window->mStopCounts[i] = -1;
    }
    return window;
}

/* 026FF428 */
void hdMsgWindowStates_c::updateWhileActive(u32 value) {
    if (!messageIsIdle(this)) {
        call_ptr<void>(virtualState(this, 0x9C), this, value);
        call_ptr<void>(virtualState(this, 0xC4), this, value);
    }
}

/* 026FF5AC */
void hdMsgWindowStates_c::readController() {
    auto* holder = hdInputGetManager();
    if (!holder) return;
    auto* buttons = holder->mInput.get();
    if (!buttons) return;
    mTypingFlags = buttons->mRepeat;
    mInputFlags = buttons->mHold;
    u32 triggered = buttons->mHold;
    u32 held = buttons->mRelease;
    windowPanes(this)->mHeldInput = triggered | held;
}

extern void selectionMessageReset(void*);
extern void selectionMessageSet(void*, u32, void*);
extern void selectionPaneUpdate(WindowPane*, void*, void*);
extern void messageShowText(WindowMessage*, s32, s32);
extern void messageHideText(WindowMessage*, s32, s32);
extern void messageApplyText(WindowPane*, WindowMessage*, u32);

/* 026FD70C */
void hdMsgWindowStates_c::enterChoiceLayout() {
    WindowPane* background = windowPanes(this)->mpSelectionBackground.get();
    background->mFlags = (background->mFlags & 0xFE) + 1;
    call_ptr<void>(virtualState(this, 0x484), this);
    u32 mode = mpMessage.get()->mState;
    if (mode == 8) {
        WindowPane* cursor = windowPanes(this)->mpTwoChoiceCursor.get();
        cursor->mFlags = (cursor->mFlags & 0xFE) + 1;
        WindowPane* text = windowPanes(this)->mpTwoChoices.get();
        text->mFlags = (text->mFlags & 0xFE) + 1;
        auto* pane = reinterpret_cast<MsgChoicePane_l*>(windowPanes(this)->mpTwoChoices.get());
        selectionMessageReset(pane->mMessage);
        pane->mMessageHeight = reinterpret_cast<MsgChoicePane_l*>(windowPanes(this)->mpTwoChoices.get())->mHeight;
        selectionMessageSet(pane->mMessage, 0, nullptr);
        pane->mMessageSpeed = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get())->mSpeed;
        void* positions = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get())->mChoicePositions;
        selectionPaneUpdate(windowPanes(this)->mpTwoChoices.get(), positions, pane->mMessage);
    } else {
        WindowPane* cursor = windowPanes(this)->mpThreeChoiceCursor.get();
        cursor->mFlags = (cursor->mFlags & 0xFE) + 1;
        WindowPane* text = windowPanes(this)->mpThreeChoices.get();
        text->mFlags = (text->mFlags & 0xFE) + 1;
        auto* pane = reinterpret_cast<MsgChoicePane_l*>(windowPanes(this)->mpThreeChoices.get());
        selectionMessageReset(pane->mMessage);
        pane->mMessageHeight = reinterpret_cast<MsgChoicePane_l*>(windowPanes(this)->mpThreeChoices.get())->mHeight;
        selectionMessageSet(pane->mMessage, 0, nullptr);
        pane->mMessageSpeed = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get())->mSpeed;
        void* positions = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get())->mChoicePositions;
        selectionPaneUpdate(windowPanes(this)->mpThreeChoices.get(), positions, pane->mMessage);
    }
    statesPlaySound(0x884);
    call_ptr<void>(virtualState(this, 0x4BC), this);
    updateSelectionLayout();
}

/* 026FD8AC */
void hdMsgWindowStates_c::updateChoiceLayout() {
    if (call_ptr<u32>(virtualState(this, 0x48C), this)) {
        u32 mode = mpMessage.get()->mState;
        if (mode == 8) statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionSelect2));
        else if (mode == 9) statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageDefinitionSelect3));
    } else updateSelectionLayout();
}

/* 026FF834 */
void hdMsgWindowStates_c::advanceText() {
    u32 index = mStopIndex;
    WindowMessage* message = mpMessage.get();
    s32 count = mCharacterCount;
    s32 offset = mStopOffsets[index < 10 ? index : 0];
    messageShowText(message, offset, count);
    messageApplyText(mpTextPane.get(), mpMessage.get(), 0);
    auto* details = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get());
    gfn* table = mpVTable.get();
    mCharacterCount = details->mShown;
    call_ptr<void>(table[0x454 / 4].get(), this);
    WindowPane* pane = mpTextPane.get();
    pane->mFlags = (pane->mFlags & 0xFE) + 1;
    u32 next = u32(mStopIndex) + 1;
    u32 slot = next < 10 ? next : 0;
    if (mStopOffsets[slot] < 0) {
        mStopOffsets[slot] = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get())->mOffset;
        next = u32(mStopIndex) + 1;
    }
    slot = next < 10 ? next : 0;
    if (mStopCounts[slot] < 0) {
        auto* message = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get());
        mStopCounts[slot] = u32(message->mCount) + 1;
    }
}

/* 026FF9D4 */
void hdMsgWindowStates_c::finishText() {
    u32 index = mStopIndex;
    s32 count = mCharacterCount;
    WindowMessage* message = mpMessage.get();
    s32 offset = mStopOffsets[index < 10 ? index : 0];
    messageHideText(message, offset, count);
    messageApplyText(mpTextPane.get(), mpMessage.get(), 0);
    message = mpMessage.get();
    WindowCore* core = l_windowCore.get();
    statesSetStatus(core, message->mState);
}

extern u32 messageDecreaseSelection(WindowBase*);
extern u32 messageIncreaseSelection(WindowBase*, be<u32>*);
extern u32 messageIncreaseNumber(WindowBase*, be<s16>*, be<u32>*);
extern u32 messageDecreaseNumber(WindowBase*, be<s16>*);
extern void* messageSystemActive();
extern void messageSystemReset();
extern void messageLoad(WindowMessage*, u32, be<u32>*);
extern void messageConfigure(void*, WindowMessage*, void*, void*);
extern MsgTag_l* messageGetTag(void*, u32);
using MessageSystemPointer_l = gptr<void>;
static MessageSystemPointer_l sMessageSystem;
static MsgNumberLimits_l sNumberLimits;
static MsgNumberFactors_l sNumberFactors;

/* 026FE070 */
void hdMsgWindowStates_c::updateNumberInput(be<u32>* count) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    FrameLocal<be<s16>> number(8);
    *number = reinterpret_cast<MsgNumberPlay_l*>(statesGetPlay())->mNumber;
    u32 flags = mInputFlags;
    u32 changed = 0;
    if (flags & 0x800000) messageDecreaseSelection(this);
    else if (flags & 0x400000) messageIncreaseSelection(this, count);
    else {
        u32 held = windowPanes(this)->mHeldInput;
        if (held & 0x100000) changed = messageIncreaseNumber(this, number.get(), count);
        else if (held & 0x200000) changed = messageDecreaseNumber(this, number.get());
        else if (flags & 0x80000) messageDecreaseSelection(this);
        else if (flags & 0x40000) messageIncreaseSelection(this, count);
        else if (held & 0x10000) changed = messageIncreaseNumber(this, number.get(), count);
        else if (held & 0x20000) changed = messageDecreaseNumber(this, number.get());
    }
    bool clamped = false;
    if (*count == 2) {
        s32 value = *number;
        u32 maximum = sNumberLimits.mMaximum;
        if (value > s32(maximum)) { *number = u16(maximum); clamped = true; }
        else if (value <= 0) { *number = 1; clamped = true; }
    }
    if (!clamped && changed) statesPlaySound(0x8E2);
    s16 value = *number;
    reinterpret_cast<MsgNumberPlay_l*>(statesGetPlay())->mNumber = value;
    u8 low = u8(u16(reinterpret_cast<MsgNumberPlay_l*>(statesGetPlay())->mNumber));
    sNumberLimits.mSelected = low;
}

/* 026FE228 */
void hdMsgWindowStates_c::positionNumberPane() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x68> frame;
    auto* pane = reinterpret_cast<MsgMetricPane_l*>(mpTextPane.get());
    ChoiceFont_l* font = choiceGetFont(reinterpret_cast<ChoiceTextPane_l*>(pane));
    auto* message = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get());
    s32 line = u32(message->mLineCount) - 1;
    s32 digits = message->mNumberDigits;
    if (line < 0) line = 0;
    else if (line >= 5) line = 4;
    if (digits <= 0) digits = 1;
    else if (digits >= 5) digits = 4;
    s32 fontWidth = call_ptr<s32>(font->mpVTable[0x1C/4].get(), font);
    f32 scale = pane->mFontWidth / f32(fontWidth);
    s32 glyphWidth = call_ptr<s32>(font->mpVTable[0x7C/4].get(), font, 0xFF10u);
    f32 halfWidth = fmuls_ppc(fadds_ppc(f32(glyphWidth), pane->mCharacterSpace), 0.5f);
    s32 fontHeight = call_ptr<s32>(font->mpVTable[0x24/4].get(), font);
    f32 lineSpace = pane->mLineSpace;
    f32 height = pane->mHeight;
    f32 y = pane->mX;
    message = reinterpret_cast<MsgChoiceMessage_l*>(mpMessage.get());
    f32 base = fadds_ppc(fnmsubs(height, 0.5f, y), message->mNumberOffset);
    f32 factor = sNumberFactors.mValues[u32(line)*5+u32(digits)];
    f32 x = fmadds(halfWidth, scale, base);
    f32 offset = fmuls_ppc(fadds_ppc(f32(fontHeight), lineSpace), factor);
    f32 numberY = fnmsubs(offset, scale, pane->mY);
    auto* number = reinterpret_cast<MsgNumberPane_l*>(mpNumberPane.get());
    u8 flags = number->mFlags;
    number->mX = x;
    number->mY = numberY;
    f32 z = *at<be<f32>>(frame + 0x18);
    number->mFlags = flags | 0x10;
    number->mZ = z;
}

/* 026FF4DC */
void hdMsgWindowStates_c::reset() {
    call_ptr<void>(virtualState(this, 0x444), this);
    call_ptr<void>(virtualState(this, 0x4A4), this);
    reinterpret_cast<MsgNumberPlay_l*>(statesGetPlay())->mNumberIndex = 0xFF;
    statesGetPlay()->mWaitValue = -1;
    reinterpret_cast<MsgNumberPlay_l*>(statesGetPlay())->mNumberMode = 0;
    reinterpret_cast<MsgNumberPlay_l*>(statesGetPlay())->mNumberControl = 0;
    if (statesGetPlay()->mInputBlocked) statesGetPlay()->mInputBlocked = 0;
    if (messageSystemActive()) messageSystemReset();
    statesGetPlay()->mAdvanceCount = 0;
    statesGetPlay()->mHandInput = 0;
    mpMessage.get()->mNextMessageNumber = 0;
    statesChange(&mStateMachine, reinterpret_cast<WindowState*>(&sMessageIdleState));
}

/* 026FF674 */
void hdMsgWindowStates_c::prepareMessage() {
    for (u32 i = 0; i<10; ++i) mStopOffsets[i] = -1;
    for (u32 i = 0; i<10; ++i) mStopCounts[i] = -1;
    mStopIndex = 0;
    mStopOffsets[0] = 0;
    mCharacterCount = 0;
    auto* initial = reinterpret_cast<MsgInitialFields_l*>(this);
    WindowPane* pane = mpTextPane.get();
    u32 color = initial->mColor;
    pane->mTopColor = color;
    pane->mBottomColor = color;
    reinterpret_cast<MsgPreparation_l*>(mpMessage.get())->mInitialColor = initial->mColor;
    reinterpret_cast<MsgPreparation_l*>(mpMessage.get())->mTextColor = initial->mColor;
    mStopReady = 0;
    messageLoad(mpMessage.get(), mMessageId, initial->mPosition);
    reinterpret_cast<MsgPreparation_l*>(mpMessage.get())->mInitialized = 1;
    WindowMessage* message = mpMessage.get();
    messageConfigure(sMessageSystem.get(), message, reinterpret_cast<MsgConfiguration_l*>(message)->mTextBuffer, reinterpret_cast<MsgConfiguration_l*>(message)->mPositionData);
    auto* preparation = reinterpret_cast<MsgPreparation_l*>(mpMessage.get());
    MsgBuffer_l* buffer = preparation->mpBuffer.get();
    if (buffer) {
        u32 index = preparation->mBufferIndex;
        if (index < buffer->mCount) {
            MsgTag_l* tag = messageGetTag(buffer->mpBuffer.get(), index);
            if (tag && (tag->mType == 14 || tag->mType == 15) && tag->mKind == 5) {
                u16 value = tag->mValue;
                statesGetPlay()->mWaitValue = value;
                mWaitValue = tag->mValue;
            }
        }
    }
}
/* WWHD glue (tools/verify/autoglue.py)
ext 020063C0 void statesChange(be<u32>*,WindowState*)
ext 0270477C void statesUpdatePane(WindowPane*,WindowMessage*)
ext 025F795C u32 statesGetStatus(WindowCore*)
ext 025F74D0 void statesSetStatus(WindowCore*,u32)
ext 025E1C64 void statesAcceptNumber()
ext 025E1988 u32 statesPlaySound(u32)
ext 026FCEC8 u32 statesHasInput(WindowBase*)
ext 02878098 ChoiceFont_l* choiceGetFont(ChoiceTextPane_l*)
ext 025F85B0 void choiceSetPositions(WindowCore*,ChoicePositions_l*,void*,ChoiceFont_l*,f32)
ext 025F85C8 void choiceSetCursor(WindowCore*,WindowPane*)
ext 025200D4 WindowPlay* statesGetPlay()
ext 026F91F8 void statesUpdateBase(hdMsgWindowStates_c*)
ext 0200579C void statesResetAnimator(void*,f32)
ext 020053E4 void statesStartAnimator(void*,u32,u32,f32)
ext 0200552C void statesSetAnimatorFrame(void*,u32,u32,f32)
ext 02005840 u32 statesAnimatorFinished(void*,u32)
data sMessageDefinitionCameraToOpen 1049DE6C
data sMessageDefinitionOpen 1049DECC
data sMessageDefinitionStop 1049DF5C
data sMessageDefinitionSelectStartWait 1049E04C
data sMessageDefinitionSelect2 1049E0AC
data sMessageDefinitionSelect3 1049E0DC
data sMessageDefinitionSelectOut 1049E13C
data sMessageDefinitionSelectOutContinue 1049E16C
data sMessageDefinitionKeyIconInAnimEndWaitToCloseWait 1049E1CC
data sMessageDefinitionCloseWait 1049E1FC
data sMessageDefinitionKeyIconInAnimEndWaitToContinue 1049E2EC
data sMessageDefinitionContinue 1049E31C
data sMessageDefinitionDelete 1049E37C
data sMessageDefinitionKeyIconDecideAnimEndWaitToNpcAnswerWait 1049E46C
data sMessageDefinitionCloseAfterInput 1049E4CC
data sMessageGlobals 1049DE38
data sMessageRegistrationA 101F72AC
data sMessageRegistrationB 101F72B8
data sMessageRegistrationC 101F72C4
data sMessageStateIdGuard 101FD8E4
data sMessageStateId 101FDD50
data sMessageStateVTable 1010ABDC
data sMessageStateParentName 101FF32C
ext 028F026C void messageRegister(PartRegistration_l*)
ext 028ED6F8 void messagePrimaryLock(be<u8>*)
ext 028EAB2C void messageSecondaryLock(be<u8>*)
ext 0273AD10 void* messageAllocate(u32)
ext 026F89E8 void messageConstructBase(hdMsgWindowStates_c*)
data sMessageBaseVTable 1010B0E0
data sMessageReceiverVTable 1010B5B8
data sMessageStateMachineVTable 1010B5C8
data sMessageDefaultPosition 101FFBA8
data sMessageIdleState 1049DD60
ext 02006478 MsgIdleState_l* messageCurrentState(be<u32>*)
ext 025F6EC8 void selectionMessageReset(void*)
ext 025F6B18 void selectionMessageSet(void*,u32,void*)
ext 027046D4 void selectionPaneUpdate(WindowPane*,void*,void*)
ext 025F6FCC void messageShowText(WindowMessage*,s32,s32)
ext 025F7040 void messageHideText(WindowMessage*,s32,s32)
ext 02704504 void messageApplyText(WindowPane*,WindowMessage*,u32)
ext 026FDEA0 u32 messageDecreaseSelection(WindowBase*)
ext 026FDEC0 u32 messageIncreaseSelection(WindowBase*,be<u32>*)
ext 026FDEE8 u32 messageIncreaseNumber(WindowBase*,be<s16>*,be<u32>*)
ext 026FDFE0 u32 messageDecreaseNumber(WindowBase*,be<s16>*)
ext 025DBB40 void* messageSystemActive()
ext 025DB59C void messageSystemReset()
ext 025F6B18 void messageLoad(WindowMessage*,u32,be<u32>*)
ext 025F50E8 void messageConfigure(void*,WindowMessage*,void*,void*)
ext 0273A2E8 MsgTag_l* messageGetTag(void*,u32)
data sMessageSystem 101F4AE8
data sNumberLimits 101D5F47
data sNumberFactors 1010AC5C
str 1010AF68 "::StateID_CameraToOpen" in 026FFA48
str 1010AF80 "::StateID_CameraToOutNow" in 026FFA48
str 1010AF9C "::StateID_Open" in 026FFA48
str 1010AFAC "::StateID_OutNow" in 026FFA48
str 1010AFC0 "::StateID_KeyIconInAnimEndWaitToStop" in 026FFA48
str 1010AFE8 "::StateID_Stop" in 026FFA48
str 1010AFF8 "::StateID_AutoStop" in 026FFA48
str 1010B00C "::StateID_KeyIconInAnimEndWaitToHandStop" in 026FFA48
str 1010B038 "::StateID_HandStop" in 026FFA48
str 1010ACC8 "::StateID_KeyIconInAnimEndWaitToSelectStartWait" in 026FFA48
str 1010AE2C "::StateID_SelectStartWait" in 026FFA48
str 1010B04C "::StateID_SelectIn" in 026FFA48
str 1010AE48 "::StateID_Select2" in 026FFA48
str 1010AE5C "::StateID_Select3" in 026FFA48
str 1010AE70 "::StateID_SelectNpcAnswerWait" in 026FFA48
str 1010AD88 "::StateID_SelectOut" in 026FFA48
str 1010AD9C "::StateID_SelectOutContinue" in 026FFA48
str 1010ACF8 "::StateID_Input" in 026FFA48
str 1010AE90 "::StateID_KeyIconInAnimEndWaitToCloseWait" in 026FFA48
str 1010ADB8 "::StateID_CloseWait" in 026FFA48
str 1010AD08 "::StateID_AutoCloseWait" in 026FFA48
str 1010AEBC "::StateID_KeyIconInAnimEndWaitToHandCloseWait" in 026FFA48
str 1010AD20 "::StateID_HandCloseWait" in 026FFA48
str 1010AD38 "::StateID_NpcAnswerWait" in 026FFA48
str 1010B060 "::StateID_KeyIconInAnimEndWaitToContinue" in 026FFA48
str 1010B08C "::StateID_Continue" in 026FFA48
str 1010AD50 "::StateID_Close" in 026FFA48
str 1010B0A0 "::StateID_Delete" in 026FFA48
str 1010B0B4 "::StateID_KeyIconDecideAnimEndWaitToOutNow" in 026FFA48
str 1010ADCC "::StateID_KeyIconDecideAnimEndWaitToOutNow2" in 026FFA48
str 1010AD60 "::StateID_KeyIconOutAnimEndWaitToOutNow" in 026FFA48
str 1010AEEC "::StateID_KeyIconDecideAnimEndWaitToClose" in 026FFA48
str 1010AF18 "::StateID_KeyIconDecideAnimEndWaitToNpcAnswerWait" in 026FFA48
str 1010ADF8 "::StateID_KeyIconDecideAnimEndWaitToCloseAfterInput" in 026FFA48
str 1010AF4C "::StateID_CloseAfterInput" in 026FFA48
*/
