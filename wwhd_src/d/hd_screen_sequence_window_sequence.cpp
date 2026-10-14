/* HD: sequence-window prompt states and button flow. Companion to the merged
 * sequence-window source; existing verified state handlers remain there. */
#include "d/hd_sequence_window.h"
#include "d/hd_parts_valid.h"

extern void* sequenceGetSystem();
extern void sequenceSetSystemEnabled(void*, u32);
extern void sequenceRequestPartOut(hdParts_c*);
extern void sequenceDelete(void*);
extern void sequenceRegisterNames(hdSequenceRegistration_c*);
extern u32 sequenceIsCancelEvent(hdUiEvent*);
extern u32 sequenceIsConfirmEvent(hdUiEvent*);
extern u32 sequencePaneContains(hdSequencePane_c*, const cking::ui::input::Vec2*);
extern dOwPlayView_c* sequenceGetPlayButtons();
extern void sequencePlaySound(const char*);
extern void sequenceSetFrontendMode(u32);
extern void sequenceCopyEvent(hdUiEvent*, hdUiEvent*);
extern u32 sequenceIsEventAccepted(u32);
extern hdSequenceButton_c* sequenceConstructButton(hdSequenceButton_c*);
extern void sequenceInitializeButton(hdSequenceButton_c*, void*, void*, void*);
extern void sequenceDestroyString(hdUiSafeString*, s32);
extern void sequenceDestroyStringArray(void*, u32, u32, u32, u32, u32);
extern void sequenceConstructMessage(HdMsgText*);
extern void sequenceSetMessageLabel(HdMsgText*, hdUiSafeString*, hdUiSafeString*);
extern void sequenceRestartText(HdMsgText*, u32, u32);
static hdSequenceInputPointer_c sSequenceInput;
static hdSequenceInputManagerPointer_c sSequenceInputManager;
static cking::ui::input::Vec2 sSequenceDefaultTouch;
static dOwUiFlags_c sSequenceUiFlags;
static hdSequenceModeTable_c sSequenceModes;
static hdSequenceOptionsPointer_c sSequenceOptions;
static hdPartsStateId_c sSequenceBlankState;
static hdPartsStateId_c sSequenceDecideState;
static hdSequenceClosedPointer_c sSequenceClosed;
static hdPartsStateId_c sSequenceAdvanceState;
static hdPartsStateId_c sSequenceWaitState;
static hdPartsStateId_c sSequenceModeOneState;
static hdPartsStateId_c sSequenceDefaultState;
static hdPartsStateId_c sSequenceEndWaitState;
static hdPartsStateId_c sSequenceResetEndState;
static hdSequenceUiSystemPointer_c sSequenceUiSystem;
static hdSequencePaneNameTable_c sSequencePaneNames;
static hdSequenceGuard_c sSequencePaneNamesGuard;
static hdSequenceRegistration_c sSequencePaneNamesRegistration;
static hdSequenceTextPaneNameTable_c sSequenceTextPaneNames;
static hdSequenceModeWordTable_c sSequenceButtonAnimations;
static hdSequenceModeWordTable_c sSequenceTextKinds;
static hdSequenceModeWordTable_c sSequenceDefaultButtons;
static hdSequenceButtonVisibilityTable_c sSequenceButtonVisibility;
static hdSequenceLabelTable_c sSequenceLabels;
static hdSequenceButtonSoundTable_c sSequenceButtonSounds;
static hdSequenceButtonIndexTable_c sSequenceButtonIndices;
static hdUiStringTable sSequenceStringVtable;
static hdPartsVtbl_c sSequenceVtable;
static hdSequenceInterfaceTable_c sSequenceHolderVtable;
static hdSequenceInterfaceTable_c sSequenceSecondaryVtable;

/* 026DA05C */
void hdSequenceWindow_c::resetInputMode() {
    sSequenceInput.mpInput->setModeDefault(0);
}

/* 026DA06C */
void hdSequenceWindow_c::advance() {
    changeState(&sSequenceAdvanceState);
}

/* 026DA07C */
void hdSequenceWindow_c::requestAdvance() {
    mAdvanceRequested = 1;
}

/* 026DABF0 */
void hdSequenceWindow_c::saveExit() {
    void* system = sequenceGetSystem();
    sequenceSetSystemEnabled(system, 1);
}

/* 026DAC18 */
void hdSequenceWindow_c::closeSavePart() {
    sequenceRequestPartOut(child(3));
}

/* 026DAC34 */
void hdSequenceWindow_c::closePartExecute() {
    hdPartsInOutLoop_c* part = reinterpret_cast<hdPartsInOutLoop_c*>(child(3));
    if (part->mIsClosed) {
        changeState(&sSequenceWaitState);
    }
}

/* 026DADEC */
void hdSequenceWindow_c::loadExit() {
    void* system = sequenceGetSystem();
    sequenceSetSystemEnabled(system, 1);
    sSequenceUiFlags.mControlMode = 1;
}

/* 026DAE7C */
void hdSequenceWindow_c::requestClose() {
    *sSequenceClosed.mpFlag.get() = 1;
    mCloseRequested = 1;
}

/* 026D959C */
s32 sequenceStateNumber(hdStateId* state) {
    return state->mId;
}

/* 026DA088 */
void hdSequenceWindow_c::showNextIcon() {
    if (mMessageVisible) {
        reinterpret_cast<hdPartsKeyIcon_c*>(child(4))->requestIn();
    }
}

/* 026DA750 */
void hdSequenceWindow_c::hideNextIcon() {
    if (mMessageVisible) {
        reinterpret_cast<hdPartsKeyIcon_c*>(child(4))->requestOut();
    }
}

/* 026DA7D4 */
void hdSequenceWindow_c::releaseButtons() {
    for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) {
        if (mPaneVisible[i]) {
            reinterpret_cast<hdBasicButton_c*>(child(i))->hide();
        }
    }
}

/* 026DA864 */
void hdSequenceWindow_c::waitEnter() {
    sSequenceInput.mpInput->setModeDefault(0);
    releaseButtons();
    mTimer = mWaitFrames;
}

/* 026DA9EC */
void hdSequenceWindow_c::waitFlagEnter() {
    sSequenceInput.mpInput->setModeDefault(0);
    releaseButtons();
    mTimer = mWaitFrames;
}

/* 026DA8D0 */
void hdSequenceWindow_c::buttonWaitEnter() {
    mButtonsEnabled = 1;
    selectButton(1);
    updateButtons();
}

/* 026DAA74 */
void hdSequenceWindow_c::saveButtonWaitEnter() {
    mButtonsEnabled = 1;
    selectButton(1);
    updateButtons();
}

/* 026DAAB0 */
void hdSequenceWindow_c::saveEnter() {
    reinterpret_cast<hdPartsInOutLoop_c*>(child(3))->requestIn();
    mTimer = 30;
    mIoPhase = 0;
    void* system = sequenceGetSystem();
    sequenceSetSystemEnabled(system, 0);
}

/* 026DACAC */
void hdSequenceWindow_c::loadEnter() {
    reinterpret_cast<hdPartsInOutLoop_c*>(child(3))->requestIn();
    mTimer = 30;
    mIoPhase = 0;
    void* system = sequenceGetSystem();
    sequenceSetSystemEnabled(system, 0);
}

/* 026D9794 */
void hdSequenceWindow_c::destroy(hdSequenceWindow_c* window, s32 flags) {
    if (window) {
        window->mStateHolder.mHolderVtable = &sSequenceHolderVtable;
        window->mSecondaryVtable = &sSequenceSecondaryVtable;
        hdParts_c::destroy(window, 0);
        if (flags & 1) {
            sequenceDelete(window);
        }
    }
}

/* 026DAE94 */
hdSequenceWindow_c* hdSequenceWindow_c::construct(hdSequenceWindow_c* window) {
    if (!window) {
        window = static_cast<hdSequenceWindow_c*>(operator_new(sizeof(hdSequenceWindow_c)));
        if (!window) {
            return NULL;
        }
    }
    hdParts_c::construct(window);
    window->mVtable = &sSequenceVtable;
    window->mSecondaryVtable = &sSequenceSecondaryVtable;
    window->mStateHolder.mHolderVtable = &sSequenceHolderVtable;
    // GHS retains the allocation checks for its four embedded fixed arrays.
    if (gabi::ea(window) + u32(offsetof(hdSequenceWindow_c, mpPanes)) == 0)
        operator_new(sizeof(window->mpPanes));
    if (gabi::ea(window) + u32(offsetof(hdSequenceWindow_c, mpTextPanes)) == 0)
        operator_new(sizeof(window->mpTextPanes));
    if (gabi::ea(window) + u32(offsetof(hdSequenceWindow_c, mpPartsPanes)) == 0)
        operator_new(sizeof(window->mpPartsPanes));
    if (gabi::ea(window) + u32(offsetof(hdSequenceWindow_c, mPaneVisible)) == 0)
        operator_new(sizeof(window->mPaneVisible));
    window->mIoPhase = 0;
    window->mWaitFrames = 10;
    window->mSavedInputFlag = 0;
    window->mActive = 0;
    window->mMenuFlagSet = 0;
    window->mCloseRequested = 0;
    window->mWaiting = 0;
    window->mpMessage = NULL;
    window->mSavedInputMode = 0;
    window->mAdvanceRequested = 0;
    window->mTextKind = 0;
    window->mpText = NULL;
    window->mButtonSelectionEnabled = 0;
    window->mButtonMode = 0;
    window->mButtonsEnabled = 0;
    window->mButtonCount = 1;
    window->mSelectedButton = 2;
    window->mOutsideTouchArmed = 0;
    window->mSecondScreen = 0;
    window->mMode = 18;
    window->mSaveRequested = 0;
    window->mOpenComplete = 0;
    window->mRestoreInputMode = 0;
    window->mMessageIndex = 0;
    window->mKeepPlaySettings = 0;
    window->mpNextState = NULL;
    window->mWaitComplete = 0;
    window->mMessageVisible = 0;
    window->mLoadRequested = 0;
    window->mCancelRequested = 0;
    window->mWatchedStatus = 0;
    window->mMessageLayoutMode = 0;
    window->mTimer = 0;
    window->mWatchedFirst = 0;
    for (u32 i = 0; i < SEQUENCE_PANE_COUNT; ++i) window->mpPanes[i] = NULL;
    for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) window->mpTextPanes[i] = NULL;
    for (u32 i = 0; i < SEQUENCE_PANE_COUNT; ++i) {
        window->mpPartsPanes[i] = NULL;
        window->mPaneVisible[i] = 0;
    }
    return window;
}

/* 026D9800 */
void hdSequenceWindow_c::selectInputButton() {
    s32 selected = mSelectedButton;
    if (selected < 0) return;
    hdButtonPart_c* button = reinterpret_cast<hdButtonPart_c*>(child(sSequenceButtonIndices.index(selected)));
    u32 mode = gabi::ea(button->mpReceiver.get());
    sSequenceInput.mpInput->setModeDefault(mode);
}

/* 026D9C48 */
void hdSequenceWindow_c::updateButtons() {
    if (mMessageVisible) {
        sequenceRestartText(mpMessage, mMessageLayoutMode, 335);
        u32 textKind = mTextKind;
        hdTextPane_c* pane = reinterpret_cast<hdTextPane_c*>(mpTextPanes[textKind < 3 ? textKind : 0].get());
        HdMsgText* message = mpMessage;
        pane->prepareMessage(reinterpret_cast<PaneWork_l*>(message), 1);
        mMessageLayoutMode = mpMessage->mLayoutMode;
        return;
    }
    u32 mode = mMode;
    hdSequenceTextRecord_c* text = mpText;
    u32 textKind = mTextKind;
    u32 label = text->mLabel;
    if (mode == 2 && label == 8) label = mMessageIndex + 8;
    hdPartsLayout_c* layout = mpLayout;
    hdPartsLyt_c* lyt = layout->lyt();
    hdTextPane_c* pane = reinterpret_cast<hdTextPane_c*>(mpTextPanes[textKind < 3 ? textKind : 0].get());
    hdUiSafeString archive;
    archive.mVtable = &sSequenceStringVtable;
    archive.mText = reinterpret_cast<hdSequenceLayoutLabel_l*>(lyt)->mpArchiveLabel;
    pane->resetMessage(&archive, sSequenceLabels.label(label));
}

/* 026D9974 */
void hdSequenceWindow_c::selectButton(u32 setInput) {
    u32 animation = mpText->mAnimation;
    hdAnimSet_play(animSet(), sSequenceButtonAnimations.word(animation), 1, 1.0f);
    hdSequenceTextRecord_c* text = mpText;
    mTextKind = sSequenceTextKinds.word(animation);
    u32 buttonMode = text->mButtonMode;
    mButtonMode = buttonMode;
    mSelectedButton = sSequenceDefaultButtons.word(buttonMode);
    if (buttonMode == SEQUENCE_BUTTON_PAIR) {
        hdSequenceTextRecord_c* current = mpText;
        s32 defaultButton = current->mDefaultButton;
        buttonMode = mButtonMode;
        mSelectedButton = defaultButton;
    }
    hdAnimSet_setFrame(animSet(), SEQUENCE_ANIM_BUTTON_SELECT, 2,
                       buttonMode == SEQUENCE_BUTTON_SINGLE ? 1.0f : 0.0f);
    if (setInput) selectInputButton();
    else sSequenceInput.mpInput->setModeDefault(0);
    mButtonSelectionEnabled = 1;
    for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) {
        u32 mode = mButtonMode;
        u8 visible = sSequenceButtonVisibility.visible(mode * SEQUENCE_BUTTON_COUNT + i);
        u8 enabled = mButtonsEnabled;
        if (visible) mButtonSelectionEnabled = 0;
        if (enabled) {
            hdParts_c* button = child(i);
            if (visible) reinterpret_cast<hdBasicButton_c*>(button)->resetAndDisable();
            else reinterpret_cast<hdBasicButton_c*>(button)->show();
        } else if (u8(mPaneVisible[i]) != visible) {
            hdParts_c* button = child(i);
            if (visible) reinterpret_cast<hdBasicButton_c*>(button)->resetAndDisable();
            else reinterpret_cast<hdBasicButton_c*>(button)->hide();
        }
        mPaneVisible[i] = visible;
    }
    mSaveRequested = 0;
    mCancelRequested = 1;
    u32 mode = mButtonMode;
    mLoadRequested = 1;
    if (mode == SEQUENCE_BUTTON_SINGLE) {
        mCancelRequested = 0;
    } else if (mode == SEQUENCE_BUTTON_CONFIRM) {
        hdSequenceTextRecord_c* text = mpText;
        mLoadRequested = 0;
        setButtonLabel(2, text->mButtonLabels[0]);
        if (mpText->mButtonLabels[0] < SEQUENCE_BUTTON_LABEL_COUNT) mCancelRequested = 0;
    } else if (mode == SEQUENCE_BUTTON_PAIR) {
        hdSequenceTextRecord_c* text = mpText;
        mLoadRequested = 0;
        setButtonLabel(0, text->mButtonLabels[0]);
        setButtonLabel(1, mpText->mButtonLabels[1]);
        hdSequenceTextRecord_c* current = mpText;
        if (current->mButtonLabels[0] < SEQUENCE_BUTTON_LABEL_COUNT ||
            current->mButtonLabels[1] < SEQUENCE_BUTTON_LABEL_COUNT) mCancelRequested = 0;
    }
    mButtonsEnabled = 0;
}

/* 026D9844 */
void hdSequenceWindow_c::setButtonSound(u32 buttonIndex, u32 sound) {
    if (sound > 31) return;
    hdUiSafeString* name = &sSequenceButtonSounds.mNames[sound];
    if (!*name->mText.get()) return;
    hdParts_c* button = child(buttonIndex);
    if (button) reinterpret_cast<hdBasicButton_c*>(button)->setSound(name, 0);
}

/* 026D989C */
void hdSequenceWindow_c::setButtonLabel(u32 buttonIndex, u32 label) {
    if (label >= SEQUENCE_BUTTON_LABEL_COUNT) return;
    HdMsgText message;
    sequenceConstructMessage(&message);
    hdPartsLyt_c* lyt = mpLayout->lyt();
    hdUiSafeString archive;
    archive.mVtable = &sSequenceStringVtable;
    archive.mText = reinterpret_cast<hdSequenceLayoutLabel_l*>(lyt)->mpArchiveLabel;
    sequenceSetMessageLabel(&message, &archive, sSequenceLabels.label(label));
    hdParts_c* button = child(buttonIndex);
    archive.mVtable = &sSequenceStringVtable;
    archive.mText.v = gabi::guest_addr("T_Btn_00");
    hdButtonPart_setTextStyle(reinterpret_cast<hdButtonPart_c*>(button), &archive, reinterpret_cast<PaneWork_l*>(&message));
    setButtonSound(buttonIndex, label);
}

/* 026D969C */
hdSequenceButton_c* sequenceCreateButton(void* archive, void* parent, void* options) {
    hdScreenHeap* heap = sSequenceUiSystem.mpSystem->mHeap;
    hdSequenceButton_c* button = static_cast<hdSequenceButton_c*>(hdParts_new(SEQUENCE_BUTTON_ALLOCATION_SIZE, heap, 4));
    if (!button) return NULL;
    button = sequenceConstructButton(button);
    if (!button) return NULL;
    sequenceInitializeButton(button, archive, parent, options);
    return button;
}

/* 026D974C */
void sequenceDestroyPaneNames() {
    sequenceDestroyStringArray(sSequencePaneNames.mNames, 5, sizeof(hdUiSafeString),
                               gabi::guest_fn(reinterpret_cast<const void*>(sequenceDestroyString)), 0, 0);
}

/* 026D9770 */
void sequenceDestroyTextPaneNames() {
    sequenceDestroyStringArray(sSequenceTextPaneNames.mNames, 3, sizeof(hdUiSafeString),
                               gabi::guest_fn(reinterpret_cast<const void*>(sequenceDestroyString)), 0, 0);
}

/* 026D9FE4 */
void hdSequenceWindow_c::selectModeState() {
    hdPartsStateId_c* state = mMode == 1 ? &sSequenceModeOneState : &sSequenceDefaultState;
    changeState(state);
}

/* 026DA008 */
void hdSequenceWindow_c::connectedWaitExecute() {
    hdUiEvent event;
    sequenceCopyEvent(&event, sequenceCurrentEvent(sSequenceInput.mpInput));
    if (sequenceIsEventAccepted(event.mKind)) selectModeState();
}

/* 026DA778 */
void hdSequenceWindow_c::endWaitExecute() {
    hdUiEvent event;
    sequenceCopyEvent(&event, sequenceCurrentEvent(sSequenceInput.mpInput));
    if (sequenceIsEventAccepted(event.mKind)) changeState(&sSequenceEndWaitState);
}

/* 026DAE20 */
void hdSequenceWindow_c::resetEndExecute() {
    hdUiEvent event;
    sequenceCopyEvent(&event, sequenceCurrentEvent(sSequenceInput.mpInput));
    if (sequenceIsEventAccepted(event.mKind)) changeState(&sSequenceResetEndState);
}

/* 026D9D44 */
void hdSequenceWindow_c::inEnter() {
    mAdvanceRequested = 0;
    mCloseRequested = 0;
    mButtonsEnabled = 1;
    selectButton(0);
    updateButtons();
    hdAnimSet_play(animSet(), SEQUENCE_ANIM_IN, 0, 1.0f);
    reinterpret_cast<hdPartsKeyIcon_c*>(child(4))->hide();
    hdSequenceModeRecord_c* mode = sSequenceModes.mode(mMode);
    u8 actionA = mode->mActionButtonA;
    sequenceGetPlayButtons()->mActionButtonA = actionA;
    u8 actionB = mode->mActionButtonB;
    sequenceGetPlayButtons()->mActionButtonB = actionB;
    sSequenceUiFlags.mControlMode = mode->mControlMode;
    sequencePlaySound("SE_MENU_MINI_WINDOW_OPEN");
}

/* 026D9E10 */
void hdSequenceWindow_c::inExecute() {
    u32 mode = mMode;
    hdPartsLayout_c* layout = mpLayout;
    hdSequenceModeRecord_c* record = sSequenceModes.mode(mode);
    if (hdAnimSet_isEnd(layout->animSet(), SEQUENCE_ANIM_IN)) {
        mOpenComplete = 1;
        changeState(record->mpNextState);
    }
}

/* 026D9E80 */
void hdSequenceWindow_c::outEnter() {
    sSequenceInput.mpInput->setModeDefault(0);
    if (!mSecondScreen) sSequenceUiFlags.mControlMode = 0;
    hdAnimSet_play(animSet(), SEQUENCE_ANIM_OUT, 0, 1.0f);
}

/* 026D9EEC */
void hdSequenceWindow_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), SEQUENCE_ANIM_OUT)) {
        if (mMenuFlagSet) {
            sequenceSetFrontendMode(0);
            hdPartsView(sSequenceOptions.mpOptions.get())->mDirectionalInputEnabled = 0;
        }
        changeState(&sSequenceBlankState);
    }
}

/* 026D9F60 */
void hdSequenceWindow_c::restorePlaySettings() {
    if (!mKeepPlaySettings) {
        dOwPlayView_c* play = sequenceGetPlayButtons();
        u8 actionR = play->mSavedActionButtonR;
        u8 actionA = play->mSavedActionButtonA;
        play->mActionButtonR = actionR;
        u8 actionB = play->mSavedActionButtonB;
        play->mActionButtonA = actionA;
        play->mActionButtonB = actionB;
        sSequenceUiFlags.mWindControl = 0;
    }
    if (mRestoreInputMode) {
        sSequenceUiFlags.mControlMode = mSavedInputMode;
        sSequenceUiFlags.mWindControl = mSavedInputFlag;
    }
}

/* 026DA0B0 */
u32 hdSequenceWindow_c::pollInput() {
    hdUiEvent event;
    sequenceCopyEvent(&event, sequenceCurrentEvent(sSequenceInput.mpInput));
    u8 controlMode = sSequenceUiFlags.mControlMode;
    hdInputManager* inputManager = sSequenceInputManager.mpManager;
    cking::ui::input::Event* pointer = &reinterpret_cast<cking::ui::input::InputManager*>(inputManager)->mPointer;
    u32 input = SEQUENCE_INPUT_NONE;
    bool checkTouch = false;
    if (controlMode & 1) {
        if (event.mKind == 4) {
            input = SEQUENCE_INPUT_EVENT;
            if (!mMessageVisible) return input;
            checkTouch = true;
        } else if (sequenceIsConfirmEvent(&event)) {
            for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) {
                hdParts_c* button = child(sSequenceButtonIndices.index(i));
                if (button && reinterpret_cast<hdBasicButton_c*>(button)->isHidden()) return SEQUENCE_INPUT_NONE;
            }
            input = SEQUENCE_INPUT_POINTER;
        }
    }
    if (!checkTouch && !mMessageVisible) return input;
    u32 hold = pointer->mHold;
    if (!(hold & 1)) {
        if (!(hold & INPUT_BUTTON_TOUCH)) return input;
        const cking::ui::input::Vec2* source = &sSequenceDefaultTouch;
        if (pointer->mPointerFlags & 1) source = &pointer->mTouch;
        hdSequencePoint_c position;
        u32 x = source->words[0];
        u32 y = source->words[1];
        position.mPosition.words[0] = x;
        position.mPosition.words[1] = y;
        if (!sequencePaneContains(mpPanes[3], &position.mPosition)) return input;
        sequencePlaySound("SE_DRC_TOUCH_ON");
    }
    sequencePlaySound("SE_TALK_NEXT_DRC");
    return SEQUENCE_INPUT_NEXT;
}

/* 026DA234 */
u32 hdSequenceWindow_c::isPointOutside(const cking::ui::input::Vec2* position) {
    hdSequencePoint_c copy;
    u32 x = position->words[0];
    u32 y = position->words[1];
    copy.mPosition.words[0] = x;
    copy.mPosition.words[1] = y;
    return sequencePaneContains(mpPanes[3], &copy.mPosition) ^ 1;
}

/* 026DA62C */
void hdSequenceWindow_c::executeButtons() {
    for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) {
        if (mPaneVisible[i]) {
            hdParts_c* button = child(sSequenceButtonIndices.index(i));
            if (button) {
                hdButtonPartVtable_l* table = hdButtonVtable(reinterpret_cast<hdButtonPart_c*>(button));
                gabi::call_ptr(table->mOnFocusChanged.get(), button);
            }
        }
    }
}

/* 026DA6D8 */
void hdSequenceWindow_c::buttonWaitExecute() {
    u32 input = pollInput();
    if (input && mMessageVisible) {
        reinterpret_cast<hdPartsKeyIcon_c*>(child(4))->requestDecideAndWait();
    }
    handleInput(input);
    executeButtons();
}

/* 026DA90C */
void hdSequenceWindow_c::drcWaitExecute() {
    u32 input = pollInput();
    if (sSequenceInputManager.mpManager->hasPad()) {
        input = SEQUENCE_INPUT_NEXT;
        sequencePlaySound("SE_SAVE_FINISH");
    }
    handleInput(input);
    executeButtons();
}

/* 026DA97C */
void hdSequenceWindow_c::proWaitExecute() {
    u32 input = pollInput();
    if (sSequenceInputManager.mpManager->hasDrc()) {
        input = SEQUENCE_INPUT_NEXT;
        sequencePlaySound("SE_SAVE_FINISH");
    }
    handleInput(input);
    executeButtons();
}

/* 026DA270 */
u32 hdSequenceWindow_c::pollCancel() {
    if (!(sSequenceUiFlags.mControlMode & 2) || mCancelRequested) return 0;
    hdUiEvent event;
    sequenceCopyEvent(&event, sequenceCurrentEvent(sSequenceInput.mpInput));
    if (sequenceIsCancelEvent(&event)) return 1;
    hdInputManager* manager = sSequenceInputManager.mpManager;
    cking::ui::input::Event* pointer = &reinterpret_cast<cking::ui::input::InputManager*>(manager)->mPointer;
    const cking::ui::input::Vec2* source = &sSequenceDefaultTouch;
    if (pointer->mHold & INPUT_BUTTON_TOUCH) {
        if (pointer->mPointerFlags & 1) source = &pointer->mTouch;
        hdSequencePoint_c position;
        u32 x = source->words[0];
        u32 y = source->words[1];
        position.mPosition.words[0] = x;
        position.mPosition.words[1] = y;
        if (isPointOutside(&position.mPosition)) mOutsideTouchArmed = 1;
        return 0;
    }
    if (pointer->mRepeat & INPUT_BUTTON_TOUCH) {
        if (pointer->mPointerFlags & 1) source = &pointer->mTouch;
        hdSequencePoint_c position;
        u32 x = source->words[0];
        u32 y = source->words[1];
        position.mPosition.words[0] = x;
        position.mPosition.words[1] = y;
        if (!isPointOutside(&position.mPosition)) mOutsideTouchArmed = 0;
        return 0;
    }
    if (pointer->mRelease & INPUT_BUTTON_TOUCH) {
        hdSequencePoint_c position;
        u32 x = pointer->mTouch.words[0];
        u32 y = pointer->mTouch.words[1];
        position.mPosition.words[0] = x;
        position.mPosition.words[1] = y;
        if (isPointOutside(&position.mPosition) && mOutsideTouchArmed) return 1;
    }
    return 0;
}

// GHS member-function descriptors encode a signed this adjustment and virtual slot.
/* inlined into hdSequenceWindow_c::handleInput */
static u32 sequenceCanAdvance(hdSequenceWindow_c* window, hdPartsPtmf_c* callback) {
    s16 index = callback->mVirtualIndex;
    u32 function = callback->mFunction;
    s16 adjustment = callback->mThisDelta;
    if (!index) return 1;
    u32 object = gabi::ea(window) + s32(adjustment);
    if (index > 0) {
        gptr<gfn>* vtableSlot = hdPartsView<gptr<gfn>>(object + s32(s16(function)));
        gfn* vtable = *vtableSlot;
        function = hdPartsView<gfn>(gabi::ea(vtable) + u32(index) * 8 + 4)->get();
    }
    return gabi::call_ptr<u32>(function, hdPartsView<hdSequenceWindow_c>(object));
}

/* 026DA3D8 */
void hdSequenceWindow_c::handleInput(u32 input) {
    hdUiEvent event;
    sequenceCopyEvent(&event, sequenceCurrentEvent(sSequenceInput.mpInput));
    if (!input) {
        if (pollCancel()) {
            sequencePlaySound("SE_MENU_CANCEL");
            selectModeState();
        }
        return;
    }
    u32 id = 0;
    if (input == SEQUENCE_INPUT_EVENT) id = event.mId;
    else if (input == SEQUENCE_INPUT_POINTER) {
        u32 mode = sSequenceInput.mpInput->getMode();
        id = hdPartsView<cking::ui::input::Receiver>(mode)->mId;
    }
    u8 loadRequested = mLoadRequested;
    mSelectedButton = -1;
    hdPartsPtmf_c* callback;
    if (loadRequested) {
        callback = &mpText->mCanAdvance[0];
    } else {
        s32 selected = -1;
        for (u32 i = 0; i < SEQUENCE_BUTTON_COUNT; ++i) {
            hdButtonPart_c* button = reinterpret_cast<hdButtonPart_c*>(child(sSequenceButtonIndices.index(i)));
            if (button && (input == SEQUENCE_INPUT_NEXT ||
                hdPartsView(button->mpReceiver.get())->mId == id)) {
                selected = i;
                mSelectedButton = selected;
                if (selected == 1) mSaveRequested = 1;
                break;
            }
        }
        if (selected < 0) return;
        callback = &mpText->mCanAdvance[selected == 1 ? 1 : 0];
    }
    if (!sequenceCanAdvance(this, callback)) return;
    if (!mButtonSelectionEnabled && input != SEQUENCE_INPUT_NEXT) changeState(&sSequenceDecideState);
    else selectModeState();
}

/* 026DB0A0 */
void hdSequenceWindow_c::findPanes() {
    if (!sSequencePaneNamesGuard.mInitialized) {
        sSequencePaneNamesGuard.mInitialized = 1;
        hdParts_setSafeString(&sSequencePaneNames.mNames[0], &sSequenceStringVtable, "N_All_00");
        hdParts_setSafeString(&sSequencePaneNames.mNames[1], &sSequenceStringVtable, "N_All_01");
        hdParts_setSafeString(&sSequencePaneNames.mNames[2], &sSequenceStringVtable, "N_MsgProCon_00");
        hdParts_setSafeString(&sSequencePaneNames.mNames[3], &sSequenceStringVtable, "N_BaseArea_00");
        hdParts_setSafeString(&sSequencePaneNames.mNames[4], &sSequenceStringVtable, "N_AreaName_00");
        sequenceRegisterNames(&sSequencePaneNamesRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < SEQUENCE_PANE_COUNT; ++i) {
        hdUiSafeString* name = &sSequencePaneNames.mNames[i];
        hdSequencePaneLookupTable_l* table = reinterpret_cast<hdSequencePaneLookupTable_l*>(root->mVtable.get());
        name->assureTermination();
        u32 function = table->mFindPane.get();
        mpPanes[i] = gabi::call_ptr<hdSequencePane_c*>(function, root, name->mText.get(), 1);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data sSequenceInput 101F4FF0
data sSequenceInputManager 101F5088
data sSequenceDefaultTouch 1049FFA0
str 10103F6C "SE_DRC_TOUCH_ON"
str 10103F7C "SE_TALK_NEXT_DRC"
str 10103FA0 "SE_SAVE_FINISH" in 026DA90C
str 10103FB0 "SE_SAVE_FINISH" in 026DA97C
data sSequenceUiFlags 1047B03C
data sSequenceModes 101F6AE4
data sSequenceOptions 101F8378
data sSequenceBlankState 1049DD60
data sSequenceDecideState 1049B480
str 10103F90 "SE_MENU_CANCEL"
str 10103F50 "SE_MENU_MINI_WINDOW_OPEN"
data sSequenceClosed 101F4974
data sSequenceAdvanceState 1049B4E0
data sSequenceWaitState 1049B510
data sSequenceModeOneState 1049B4B0
data sSequenceDefaultState 1049B450
data sSequenceEndWaitState 1049B570
data sSequenceResetEndState 1049B6F0
data sSequenceUiSystem 1018C404
data sSequencePaneNames 1049AFD8
data sSequencePaneNamesGuard 1049B418
data sSequencePaneNamesRegistration 101F6AA8
str 10104000 "N_All_00"
str 1010400C "N_All_01"
str 10104018 "N_MsgProCon_00"
str 10103FE0 "N_BaseArea_00"
str 10103FF0 "N_AreaName_00"
data sSequenceTextPaneNames 1049B000
data sSequenceButtonAnimations 10103BF4
data sSequenceTextKinds 10103F38
data sSequenceDefaultButtons 10103F28
data sSequenceButtonVisibility 10103F44
data sSequenceLabels 1049B0F0
data sSequenceButtonSounds 1049B288
str 10103F14 "T_Btn_00"
data sSequenceButtonIndices 10103BCC
data sSequenceStringVtable 1010394C
data sSequenceVtable 101046C8
data sSequenceHolderVtable 10104910
data sSequenceSecondaryVtable 10104920
ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
ext 020013C0 void sequenceCopyEvent(hdUiEvent*,hdUiEvent*)
ext 026F620C u32 sequenceIsEventAccepted(u32)
ext 026F6288 u32 sequenceIsConfirmEvent(hdUiEvent*)
ext 026F24D0 BOOL hdBasicButton_c::isHidden()
ext 0287F05C u32 sequencePaneContains(hdSequencePane_c*,const cking::ui::input::Vec2*)
ext 026F824C void hdPartsKeyIcon_c::requestDecideAndWait()
ext 026F6340 u32 sequenceIsCancelEvent(hdUiEvent*)
ext 02615140 u32 cking::ui::input::EventMgr::getMode()
ext 02617F14 u32 hdInputManager::hasPad()
ext 02617FCC u32 hdInputManager::hasDrc()
ext 02035D78 void* sequenceGetSystem()
ext 02035ED4 void sequenceSetSystemEnabled(void*,u32)
ext 026F7924 void sequenceRequestPartOut(hdParts_c*)
ext 026F8228 void hdPartsKeyIcon_c::requestIn()
ext 026F8260 void hdPartsKeyIcon_c::requestOut()
ext 026F7908 void hdPartsInOutLoop_c::requestIn()
ext 026F23F0 void hdBasicButton_c::hide()
ext 025F65F4 void sequenceConstructMessage(HdMsgText*)
ext 025F6D70 void sequenceSetMessageLabel(HdMsgText*,hdUiSafeString*,hdUiSafeString*)
ext 026F6048 void hdButtonPart_c::setTextStyle(hdUiSafeString*,u32)
ext 026F2540 void hdBasicButton_c::setSound(hdUiSafeString*,u32)
ext 026F237C void hdBasicButton_c::resetAndDisable()
ext 026F2400 void hdBasicButton_c::show()
ext 025F6FCC void sequenceRestartText(HdMsgText*,u32,u32)
ext 02704504 void hdTextPane_c::prepareMessage(PaneWork_l*,u32)
ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString*,hdUiSafeString*)
ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
ext 0273AF40 void sequenceDelete(void*)
ext 028F026C void sequenceRegisterNames(hdSequenceRegistration_c*)
ext 025200D4 dOwPlayView_c* sequenceGetPlayButtons()
ext 02030B38 void sequencePlaySound(const char*)
ext 0259170C void sequenceSetFrontendMode(u32)
ext 026F826C void hdPartsKeyIcon_c::hide()
ext 0263D438 hdSequenceButton_c* sequenceConstructButton(hdSequenceButton_c*)
ext 0263D4C8 void sequenceInitializeButton(hdSequenceButton_c*,void*,void*,void*)
ext 026DD30C void sequenceDestroyString(hdUiSafeString*,s32) callback
ext 028F0164 void sequenceDestroyStringArray(void*,u32,u32,u32,u32,u32)
*/
