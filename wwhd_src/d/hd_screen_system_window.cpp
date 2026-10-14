/**
 * hd_screen_system_window.cpp
 * HD - the GamePad system window "SequenceWindow_00": the save / continue / warp / Miiverse /
 * second-screen / hard-mode dialogs, run as scripts of commands (companion of hd_screen_shortcut.cpp,
 * which holds the command handlers it shares this translation unit with)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_hud_shortcut). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, pane, text, sound and state names are
 * HD strings; all other names are ours.
 */

#include "d/hd_screen_system_window.h"
static u32 sSysWinHeaderStatics;
static u32 sSysWinHeaderDescriptors;
static u32 sSysWinHeaderZero;
static u32 sSysWinPiConstants;
/* hdParts_c::createChild (026F92C0) is bound by the glue of hd_screen_item_shortcut.cpp (same spec) */

static hdUiStringTable sSysWinSafeStringVtable;
static hdPartsStateIdVtbl_c sSysWinStateIdVtable;
static hdStateId sSysWinNoStateId;
static hdPartsStateId_c sSysWinPartsBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static hdScreenUiSystem* sSysWinUiSystem;
static hdSysWinMsgMgr_c* sSysWinMsgMgr;
static void* sSysWinMsgBuilder;
static hdSysWinWatchedHolder_c* sSysWinWatched;
static hdSysWinController_c* sSysWinController;
static hdSysWinOptions_c sSysWinOptions;
static hdSysWinScripts_c sSysWinScripts;
static hdPartsChar_t* sSysWinNullString; /* the empty string a SafeString starts with */

static hdUiSafeString sSysWinLoadingSound;
static hdUiSafeString sSysWinGroupNames[4] = {};
static hdUiSafeString sSysWinAnimNames[7] = {};
static hdUiSafeString sSysWinPartsPaneNames[5] = {};
static hdUiSafeString sSysWinPartsLayoutNames[5] = {};
static hdUiSafeString sSysWinArchiveNames[4] = {};
static hdUiSafeString sSysWinTextNames[51] = {};
static hdUiSafeString sSysWinCommandSounds[50] = {};
static hdUiSafeString sSysWinLayoutName;
static hdPartsStateId_c sSysWinInAll;
static hdPartsStateId_c sSysWinOutAll;
static hdPartsStateId_c sSysWinOutWait;
static hdPartsStateId_c sSysWinEndWait;
static hdPartsStateId_c sSysWinEnd;
static hdPartsStateId_c sSysWinButtonWait;
static hdPartsStateId_c sSysWinDecideWait;
static hdPartsStateId_c sSysWinOutButton;
static hdPartsStateId_c sSysWinDrcWait;
static hdPartsStateId_c sSysWinProWait;
static hdPartsStateId_c sSysWinConnectedWait;
static hdPartsStateId_c sSysWinSaveWait;
static hdPartsStateId_c sSysWinSaveEnd;
static hdPartsStateId_c sSysWinSaveInit;
static hdPartsStateId_c sSysWinReset;
static hdPartsStateId_c sSysWinResetEnd;
static u32 sSysWinAnimGroups[7] = {0, 1, 1, 1, 2, 3, 0};
static u32 sSysWinStateNumberReady;
static s32 sSysWinStateNumber;

/* inlined: is the state machine in `stateId` (the state's vtable is read before the current key) */
static bool hdSysWin_isState(hdParts_c* parts, hdPartsStateId_c* stateId) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(parts->stateMachine());
    u32 getCurrentKey = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* vtable = stateId->mVtable;
    u32 key = gabi::call_ptr<u32>(getCurrentKey, current);
    return key == gabi::call_ptr<u32>(vtable->mGetKey.get(), stateId);
}

/* 026DB41C */
// the parts panes the dialog buttons, the loading icon and the key icon live in
void hdSequenceWindow_c::findPartsPanes() {
    hdSequencePane_c* root = (hdSequencePane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < 5; i++) {
        hdUiSafeString* name = &sSysWinPartsPaneNames[i];
        u32 assure = name->mVtable->mAssureTermination.get();
        hdSequencePaneLookupTable_l* rootVtable = root->mVtable;
        gabi::call_ptr(assure, name);
        mpPartsPanes[i] = gabi::call_ptr<void*>(rootVtable->mFindPane.get(), root, (be<u8>*)name->mText, 1u);
    }
}

/* 026DB4F8 */
// the left and right buttons point at each other (probably the cursor links)
void hdSequenceWindow_c::linkButtons() {
    hdParts_c* left = *mChildren.slot(SYSWIN_CHILD_BUTTON_L);
    hdSysWinButton_c* right = (hdSysWinButton_c*)child(SYSWIN_CHILD_BUTTON_R);
    hdSysWin_linkButton(left, 0, 0, 0, 0, 0, 0, 0, gabi::ea(right->mpPane.get()));
    hdParts_c* button = child(SYSWIN_CHILD_BUTTON_R);
    hdSysWinButton_c* first = (hdSysWinButton_c*)mChildren.slot(SYSWIN_CHILD_BUTTON_L)->get();
    hdSysWin_linkButton(button, 0, 0, 0, 0, 0, 0, gabi::ea(first->mpPane.get()), 0);
    hdSysWin_linkButton(child(SYSWIN_CHILD_BUTTON_C), 0, 0, 0, 0, 0, 0, 0, 0);
}

/* 026DB5F4 */
u32 hdSequenceWindow_c::init(u32 arg1, u32 arg2, void* desc) {
    sSysWinMsgMgr->mpSystemWindow = this;
    attachArchives(sSysWinArchiveNames, 4);
    if (!vLoad(&sSysWinLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) {
        return FALSE;
    }
    findPanes();
    findTextPanes();
    findPartsPanes();
    mpMessage = &sSysWinMsgMgr->mWork;
    linkButtons();
    for (u32 i = 0; i < 3; i++) {
        ((hdSysWinButton_c*)child(i))->mSelectable = TRUE;
    }
    hdAnimSet_setFrame(animSet(), SYSWIN_ANIM_IN_ALL, 0, 0.0f);
    hdAnimSet_setFrame(animSet(), SYSWIN_ANIM_ON_OFF_BG, 3, 1.0f);
    hdSysWin_deactivateLoop(child(SYSWIN_CHILD_LOADING));
    hdSysWin_setLoopSound(child(SYSWIN_CHILD_LOADING), &sSysWinLoadingSound);
    hdSysWin_hideKeyIcon(child(SYSWIN_CHILD_KEY_ICON));
    vCalcLayout();
    startState(&sSysWinPartsBlank);
    return TRUE;
}

/* 026DB7EC */
void hdSequenceWindow_c::finalize() {
    sSysWinMsgMgr->mpSystemWindow = NULL;
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 026DB850 */
void hdSequenceWindow_c::executeState() {
    if (hdSysWin_isState(this, &sSysWinPartsBlank)) {
        return;
    }
    mWatchedStatus = 0;
    hdSysWinWatchedHolder_c* watched = sSysWinWatched;
    mWatchedFirst = 0;
    if (watched != NULL && watched->mpObject != NULL) {
        hdSysWinWatched_c* object = watched->mpObject;
        mWatchedStatus = object->mStatus;
        mWatchedFirst = object->mFirst;
    }
    hdParts_executeStateMachine(stateMachine());
    if (!mCloseRequested) {
        vExecuteChildren();
    }
    vCalcLayout();
    if (mOpenComplete) {
        mOpenComplete = FALSE;
        selectInputButton();
    }
}

/* 026DB954 */
void hdSequenceWindow_c::draw(void* info) {
    if (!hdSysWin_isState(this, &sSysWinPartsBlank)) {
        vDrawLayout(info);
        vDrawChildren(info);
    }
}

/* 026DBA08 */
// start dialog `mode` (from Blank only); unless `keepPlay`, the play status is backed up and the menu locked
void hdSequenceWindow_c::open(s32 mode, u32 keepPlay) {
    if (!hdSysWin_isState(this, &sSysWinPartsBlank)) {
        return;
    }
    mMode = mode;
    hdSysWinCommand_c* commands = sSysWinScripts.mList[mode]->mpCommands;
    mpText = commands;
    mpText = hdSysWin_commandAt(commands, sSysWinScripts.mList[mode]->mStart);
    hdSequencePane_c* form = mpPanes[SEQUENCE_PANE_AREA_NAME];
    if (mode == SYSWIN_MODE_MESSAGE) {
        mMessageVisible = TRUE;
        form->mFlags = (form->mFlags & ~1) + 1;
    } else if (mode == SYSWIN_MODE_MIIVERSE_INFO) {
        mMessageVisible = TRUE;
        form->mFlags = form->mFlags & ~1;
    } else {
        mMessageVisible = FALSE;
        form->mFlags = form->mFlags & ~1;
    }
    s32 current = mMode;
    mActive = FALSE;
    if (current == SYSWIN_MODE_5 || current == SYSWIN_MODE_8) {
        mActive = TRUE;
    }
    mRestoreInputMode = FALSE;
    if (current == SYSWIN_MODE_HARD_MODE) {
        mRestoreInputMode = TRUE;
        mSavedInputMode = sSysWinOptions.mSaved;
        mSavedInputFlag = sSysWinOptions.mWindowOpen;
    }
    mKeepPlaySettings = keepPlay;
    mOutsideTouchArmed = 0;
    mMenuFlagSet = FALSE;
    mSecondScreen = FALSE;
    mWaiting = FALSE;
    mWaitComplete = 0;
    if (!keepPlay) {
        hdSysWinPlay_l* play = hdSysWin_getPlay();
        for (int i = 0; i < 3; i++) {
            play->mStatusBackup[i] = play->mStatus[i];
        }
        sSysWinOptions.mWindowOpen = TRUE;
        sSysWinOptions.mSaved = FALSE;
        if (!hdSysWin_getMenuFlag()) {
            current = mMode;
            if (current == 0 || current == 1) {
                hdSysWin_setMenuFlag(TRUE);
                sSysWinController->mCursorActive = TRUE;
                mMenuFlagSet = TRUE;
            }
        }
    }
    hdAnimSet_setFrame(animSet(), SYSWIN_ANIM_ON_OFF_BG, 3, 1.0f);
    changeState(&sSysWinInAll);
}

/* 026DBC44 */
void hdSequenceWindow_c::openSecondScreen(u32 index) {
    if (!hdSysWin_isState(this, &sSysWinPartsBlank)) {
        return;
    }
    mMessageIndex = index;
    open(SYSWIN_MODE_SECOND_SCREEN, TRUE);
    mSecondScreen = TRUE;
    hdAnimSet_setFrame(animSet(), SYSWIN_ANIM_ON_OFF_BG, 3, 0.0f);
}

/* 026DBD10 */
// the message of dialog line `index` (one warp line changes after event 0x2D80)
u32 hdSequenceWindow_c::getMessageId(u32 index) {
    u32 id = index + SYSWIN_MSG_FIRST;
    if (id == SYSWIN_MSG_WARP_HERE && dComIfGs_isEventBit(0x2D80)) {
        id = SYSWIN_MSG_WARP_HERE_LATER;
    }
    return id;
}

/* 026DBD64 */
void hdSequenceWindow_c::openMessage(u32 index) {
    if (!hdSysWin_isState(this, &sSysWinPartsBlank)) {
        return;
    }
    mMessageIndex = index;
    HdMsgText* msg = mpMessage;
    mMessageLayoutMode = 0;
    if (index >= SYSWIN_MSG_INDEX_COUNT) {
        index = 0;
        mMessageIndex = 0;
    }
    hdSysWin_setMessageId(msg, index + SYSWIN_MSG_INDEX_BASE, 0);
    msg = mpMessage;
    hdSysWin_buildMessage(sSysWinMsgBuilder, msg, &msg->mSetName, &msg->mLabel);
    open(SYSWIN_MODE_MESSAGE, FALSE);
    u32 id = getMessageId(mMessageIndex);
    hdSysWin_setTextBoxMessage(mpTextPanes[SEQUENCE_TEXT_AREA_NAME], id);
}

/* 026DBE64 */
void hdSequenceWindow_c::openMiiverseInfo() {
    if (!hdSysWin_isState(this, &sSysWinPartsBlank)) {
        return;
    }
    hdPartsLayout_c* layout = mpLayout;
    mMessageLayoutMode = 0;
    hdPartsLyt_c* lyt = layout->lyt();
    HdMsgText* msg = mpMessage;
    gabi::Local<hdUiSafeString> data;
    data->mVtable = &sSysWinSafeStringVtable;
    data->mText.v = lyt->mpMessageData.v;
    hdSysWin_setMessageLabel(msg, data, &sSysWinTextNames[SYSWIN_TEXT_MIIVERSE_INFO]);
    msg = mpMessage;
    hdSysWin_buildMessage(sSysWinMsgBuilder, msg, &msg->mSetName, &msg->mLabel);
    open(SYSWIN_MODE_MIIVERSE_INFO, FALSE);
}

/* 026DBF4C */
void hdSequenceWindow_c::close() {
    hdAnimSet_setFrame(animSet(), SYSWIN_ANIM_IN_ALL, 0, 0.0f);
    changeState(&sSysWinPartsBlank);
    vCalcLayout();
}

/* inlined into visit: SafeString equality (the descriptor's string is terminated twice) */
static bool hdSysWin_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
    name->assureTermination();
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

/* 026DBFB4 */
// the five parts panes get their parts in the matching child slot
u32 hdSequenceWindow_c::visit(hdSysWinDesc_l* desc) {
    for (u32 i = 0; i < 5; i++) {
        if (hdSysWin_isSameName(&desc->mName, &sSysWinPartsPaneNames[i])) {
            return createChild(i, (hdPartsDesc_c*)desc);
        }
    }
    return TRUE;
}

/* 026DC0BC */
u32 hdSequenceWindow_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sSysWinUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 7, 4, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 7; i++) {
        hdSysWin_bindGroup(animSet(), i, &sSysWinAnimNames[i], &sSysWinGroupNames[sSysWinAnimGroups[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* 026DC26C */
s32 hdSequenceWindow_c::cmdReset() {
    changeState(&sSysWinReset);
    return 0;
}

/* 026DC394 */
// wait for the message work area; once a message was set up, run the command's parameter
s32 hdSequenceWindow_c::cmdWaitMessage() {
    if (mpMessage->mWaitComplete) {
        return 1;
    }
    updateButtons();
    if (mpMessage->mWaitComplete) {
        setButtonLabel(2, mpText->mButtonLabels[1]);
    }
    return 0;
}

/* 026DC44C */
s32 hdSequenceWindow_c::cmdStartWaiting() {
    mWaiting = TRUE;
    return 1;
}

/* 026DC494 */
s32 hdSequenceWindow_c::cmdStopWaiting() {
    mWaiting = FALSE;
    return 1;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 026DC4DC */
static void __sinit_hd_screen_system_window_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSysWinHeaderStatics), gabi::ea(&sSysWinHeaderDescriptors), gabi::ea(&sSysWinHeaderZero),
                                gabi::ea(&sSysWinPiConstants));
    hdParts_setSafeString(&sSysWinLoadingSound, &sSysWinSafeStringVtable, "SE_NOW_LOADING");
    hdParts_setSafeString(&sSysWinGroupNames[0], &sSysWinSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sSysWinGroupNames[1], &sSysWinSafeStringVtable, "G_Form_00");
    hdParts_setSafeString(&sSysWinGroupNames[2], &sSysWinSafeStringVtable, "G_Form_01");
    hdParts_setSafeString(&sSysWinGroupNames[3], &sSysWinSafeStringVtable, "G_BG_00");
    hdParts_setSafeString(&sSysWinAnimNames[0], &sSysWinSafeStringVtable, "InAll");
    hdParts_setSafeString(&sSysWinAnimNames[1], &sSysWinSafeStringVtable, "FormDefault");
    hdParts_setSafeString(&sSysWinAnimNames[2], &sSysWinSafeStringVtable, "FormDRC");
    hdParts_setSafeString(&sSysWinAnimNames[3], &sSysWinSafeStringVtable, "FormProCon");
    hdParts_setSafeString(&sSysWinAnimNames[4], &sSysWinSafeStringVtable, "FormFrameCenter");
    hdParts_setSafeString(&sSysWinAnimNames[5], &sSysWinSafeStringVtable, "OnOffBG");
    hdParts_setSafeString(&sSysWinAnimNames[6], &sSysWinSafeStringVtable, "OutAll");
    hdParts_setSafeString(&sSysWinPartsPaneNames[0], &sSysWinSafeStringVtable, "L_BtnSeqenceL_00");
    hdParts_setSafeString(&sSysWinPartsPaneNames[1], &sSysWinSafeStringVtable, "L_BtnSeqenceR_00");
    hdParts_setSafeString(&sSysWinPartsPaneNames[2], &sSysWinSafeStringVtable, "L_BtnSeqenceC_00");
    hdParts_setSafeString(&sSysWinPartsPaneNames[3], &sSysWinSafeStringVtable, "L_LoadingIcon_00");
    hdParts_setSafeString(&sSysWinPartsPaneNames[4], &sSysWinSafeStringVtable, "L_KeyIcon_00");
    hdParts_setSafeString(&sSysWinPartsLayoutNames[0], &sSysWinSafeStringVtable, "BtnSequence_01");
    hdParts_setSafeString(&sSysWinPartsLayoutNames[1], &sSysWinSafeStringVtable, "BtnSequence_01");
    hdParts_setSafeString(&sSysWinPartsLayoutNames[2], &sSysWinSafeStringVtable, "BtnSequence_01");
    hdParts_setSafeString(&sSysWinPartsLayoutNames[3], &sSysWinSafeStringVtable, "LoadingIcon_00");
    hdParts_setSafeString(&sSysWinPartsLayoutNames[4], &sSysWinSafeStringVtable, "KeyIcon_00");
    hdParts_setSafeString(&sSysWinArchiveNames[0], &sSysWinSafeStringVtable, "SequenceWindow_00");
    hdParts_setSafeString(&sSysWinArchiveNames[1], &sSysWinSafeStringVtable, "BtnSequence_01");
    hdParts_setSafeString(&sSysWinArchiveNames[2], &sSysWinSafeStringVtable, "LoadingIcon_00");
    hdParts_setSafeString(&sSysWinArchiveNames[3], &sSysWinSafeStringVtable, "KeyIcon_00");
    hdParts_setSafeString(&sSysWinTextNames[0], &sSysWinSafeStringVtable, "T_Msg_00_SAVE");
    hdParts_setSafeString(&sSysWinTextNames[1], &sSysWinSafeStringVtable, "T_Msg_00_NowSave");
    hdParts_setSafeString(&sSysWinTextNames[2], &sSysWinSafeStringVtable, "T_Msg_00_Continue");
    hdParts_setSafeString(&sSysWinTextNames[3], &sSysWinSafeStringVtable, "T_Msg_00_Continue2");
    hdParts_setSafeString(&sSysWinTextNames[4], &sSysWinSafeStringVtable, "T_Msg_00_SAVE_Yes");
    hdParts_setSafeString(&sSysWinTextNames[5], &sSysWinSafeStringVtable, "T_Msg_00_SAVE_No");
    hdParts_setSafeString(&sSysWinTextNames[6], &sSysWinSafeStringVtable, "T_Msg_00_Conti_Yes");
    hdParts_setSafeString(&sSysWinTextNames[7], &sSysWinSafeStringVtable, "T_Msg_00_Conti_No");
    hdParts_setSafeString(&sSysWinTextNames[8], &sSysWinSafeStringVtable, "T_Msg_00_WARP");
    hdParts_setSafeString(&sSysWinTextNames[9], &sSysWinSafeStringVtable, "00069");
    hdParts_setSafeString(&sSysWinTextNames[10], &sSysWinSafeStringVtable, "00070");
    hdParts_setSafeString(&sSysWinTextNames[11], &sSysWinSafeStringVtable, "00071");
    hdParts_setSafeString(&sSysWinTextNames[12], &sSysWinSafeStringVtable, "00072");
    hdParts_setSafeString(&sSysWinTextNames[13], &sSysWinSafeStringVtable, "00073");
    hdParts_setSafeString(&sSysWinTextNames[14], &sSysWinSafeStringVtable, "00074");
    hdParts_setSafeString(&sSysWinTextNames[15], &sSysWinSafeStringVtable, "00075");
    hdParts_setSafeString(&sSysWinTextNames[16], &sSysWinSafeStringVtable, "00067");
    hdParts_setSafeString(&sSysWinTextNames[17], &sSysWinSafeStringVtable, "00066");
    hdParts_setSafeString(&sSysWinTextNames[18], &sSysWinSafeStringVtable, "T_Msg_00_WARP_Yes");
    hdParts_setSafeString(&sSysWinTextNames[19], &sSysWinSafeStringVtable, "T_Msg_00_WARP_No");
    hdParts_setSafeString(&sSysWinTextNames[20], &sSysWinSafeStringVtable, "T_Msg_00_ProCon01");
    hdParts_setSafeString(&sSysWinTextNames[21], &sSysWinSafeStringVtable, "T_Msg_00_DrcOn01");
    hdParts_setSafeString(&sSysWinTextNames[22], &sSysWinSafeStringVtable, "T_Msg_00_ProDrcBack");
    hdParts_setSafeString(&sSysWinTextNames[23], &sSysWinSafeStringVtable, "T_Msg_00_ContEnter01");
    hdParts_setSafeString(&sSysWinTextNames[24], &sSysWinSafeStringVtable, "T_Msg_00_ContEnter02");
    hdParts_setSafeString(&sSysWinTextNames[25], &sSysWinSafeStringVtable, "T_Msg_00_ContEnter03");
    hdParts_setSafeString(&sSysWinTextNames[26], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse01");
    hdParts_setSafeString(&sSysWinTextNames[27], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_yes01");
    hdParts_setSafeString(&sSysWinTextNames[28], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_no01");
    hdParts_setSafeString(&sSysWinTextNames[29], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse02");
    hdParts_setSafeString(&sSysWinTextNames[30], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_yes02");
    hdParts_setSafeString(&sSysWinTextNames[31], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_no02");
    hdParts_setSafeString(&sSysWinTextNames[32], &sSysWinSafeStringVtable, "13027");
    hdParts_setSafeString(&sSysWinTextNames[33], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_yes02");
    hdParts_setSafeString(&sSysWinTextNames[34], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_no02");
    hdParts_setSafeString(&sSysWinTextNames[35], &sSysWinSafeStringVtable, "T_Msg_00_Miiverse_info00");
    hdParts_setSafeString(&sSysWinTextNames[36], &sSysWinSafeStringVtable, "T_Msg_00_SaveInit00");
    hdParts_setSafeString(&sSysWinTextNames[37], &sSysWinSafeStringVtable, "T_Msg_00_SaveInit01");
    hdParts_setSafeString(&sSysWinTextNames[38], &sSysWinSafeStringVtable, "T_Msg_00_SaveInit02");
    hdParts_setSafeString(&sSysWinTextNames[39], &sSysWinSafeStringVtable, "T_Btn_SaveInit02");
    hdParts_setSafeString(&sSysWinTextNames[40], &sSysWinSafeStringVtable, "T_Msg_00_SecondInfo00");
    hdParts_setSafeString(&sSysWinTextNames[41], &sSysWinSafeStringVtable, "T_Msg_00_SecondInfo01");
    hdParts_setSafeString(&sSysWinTextNames[42], &sSysWinSafeStringVtable, "T_Msg_00_SecondInfo02");
    hdParts_setSafeString(&sSysWinTextNames[43], &sSysWinSafeStringVtable, "T_Msg_00_SecondInfo03");
    hdParts_setSafeString(&sSysWinTextNames[44], &sSysWinSafeStringVtable, "T_Msg_00_SecondIBtn00");
    hdParts_setSafeString(&sSysWinTextNames[45], &sSysWinSafeStringVtable, "T_Msg_00_SecondIBtn01");
    hdParts_setSafeString(&sSysWinTextNames[46], &sSysWinSafeStringVtable, "T_Msg_00_SecondIBtn02");
    hdParts_setSafeString(&sSysWinTextNames[47], &sSysWinSafeStringVtable, "T_Msg_00_SecondIBtn03");
    hdParts_setSafeString(&sSysWinTextNames[48], &sSysWinSafeStringVtable, "T_Msg_00_SecondIBtn04");
    hdParts_setSafeString(&sSysWinTextNames[49], &sSysWinSafeStringVtable, "T_Msg_00_hardmode00");
    hdParts_setSafeString(&sSysWinTextNames[50], &sSysWinSafeStringVtable, "T_Button_hardmode01");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[0], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[1], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[2], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[3], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[4], &sSysWinSafeStringVtable, "SE_SAVE_START");
    hdParts_setSafeString(&sSysWinCommandSounds[5], &sSysWinSafeStringVtable, "SE_SAVE_CANCEL");
    hdParts_setSafeString(&sSysWinCommandSounds[6], &sSysWinSafeStringVtable, "SE_MENU_OPTION_OUT");
    hdParts_setSafeString(&sSysWinCommandSounds[7], &sSysWinSafeStringVtable, "SE_EXIT_GAME_YES");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[8], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[9], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[10], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[11], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[12], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[13], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[14], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[15], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[16], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[17], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[18], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[19], &sSysWinSafeStringVtable, "SE_MENU_CANCEL");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[20], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[21], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[22], &sSysWinSafeStringVtable, "SE_MENU_CANCEL");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[23], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[24], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[25], &sSysWinSafeStringVtable, "SE_MENU_OPTION_OUT");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[26], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[27], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[28], &sSysWinSafeStringVtable, "SE_MENU_CANCEL");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[29], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[30], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[31], &sSysWinSafeStringVtable, "SE_MENU_CANCEL");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[32], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[33], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[34], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[35], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[36], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[37], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[38], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[39], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[40], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[41], &sSysWinSafeStringVtable, sSysWinNullString);
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[42], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[43], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[44], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[45], &sSysWinSafeStringVtable, "SE_MENU_CANCEL");
    hdParts_setSafeString(&sSysWinCommandSounds[46], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinCommandSounds[47], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdSysWin_setEmptySafeString(&sSysWinCommandSounds[48], &sSysWinSafeStringVtable, sSysWinNullString);
    hdParts_setSafeString(&sSysWinCommandSounds[49], &sSysWinSafeStringVtable, "SE_MENU_DECIDE");
    hdParts_setSafeString(&sSysWinLayoutName, &sSysWinSafeStringVtable, "SequenceWindow_00");
    s32 number;
    if (sSysWinStateNumberReady) {
        number = sSysWinStateNumber;
    } else {
        number = 0;
        sSysWinStateNumberReady = 1;
    }
    sSysWinInAll.setup(number + 1, "::StateID_InAll", &sSysWinStateIdVtable, 0x19, &sSysWinNoStateId);
    sSysWinOutAll.setup(number + 2, "::StateID_OutAll", &sSysWinStateIdVtable, 0x1C, &sSysWinNoStateId);
    sSysWinOutWait.setup(number + 3, "::StateID_OutWait", &sSysWinStateIdVtable, 0x1F, &sSysWinNoStateId);
    sSysWinEndWait.setup(number + 4, "::StateID_EndWait", &sSysWinStateIdVtable, 0x22, &sSysWinNoStateId);
    sSysWinEnd.setup(number + 5, "::StateID_End", &sSysWinStateIdVtable, 0x25, &sSysWinNoStateId);
    sSysWinButtonWait.setup(number + 6, "::StateID_ButtonWait", &sSysWinStateIdVtable, 0x28, &sSysWinNoStateId);
    sSysWinDecideWait.setup(number + 7, "::StateID_DecideWait", &sSysWinStateIdVtable, 0x2B, &sSysWinNoStateId);
    sSysWinOutButton.setup(number + 8, "::StateID_OutButton", &sSysWinStateIdVtable, 0x2E, &sSysWinNoStateId);
    sSysWinDrcWait.setup(number + 9, "::StateID_DrcWait", &sSysWinStateIdVtable, 0x31, &sSysWinNoStateId);
    sSysWinProWait.setup(number + 10, "::StateID_ProWait", &sSysWinStateIdVtable, 0x34, &sSysWinNoStateId);
    sSysWinConnectedWait.setup(number + 11, "::StateID_ConnectedWait", &sSysWinStateIdVtable, 0x37, &sSysWinNoStateId);
    sSysWinSaveWait.setup(number + 12, "::StateID_SaveWait", &sSysWinStateIdVtable, 0x3A, &sSysWinNoStateId);
    sSysWinSaveEnd.setup(number + 13, "::StateID_SaveEnd", &sSysWinStateIdVtable, 0x3D, &sSysWinNoStateId);
    sSysWinSaveInit.setup(number + 14, "::StateID_SaveInit", &sSysWinStateIdVtable, 0x40, &sSysWinNoStateId);
    sSysWinReset.setup(number + 15, "::StateID_Reset", &sSysWinStateIdVtable, 0x43, &sSysWinNoStateId);
    sSysWinResetEnd.setup(number + 16, "::StateID_ResetEnd", &sSysWinStateIdVtable, 0x46, &sSysWinNoStateId);
    sSysWinStateNumber = number + 16;
}

/* 026DD364 */
static s32 hdScreenSystemWindow_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sSysWinHeaderStatics 1049AFCC nocheck
data sSysWinHeaderDescriptors 101F6AC0 nocheck
data sSysWinHeaderZero 1049B018 nocheck
data sSysWinPiConstants 10104054 nocheck
ext 026F5C08 void hdSysWin_linkButton(hdParts_c* button, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h)
ext 025F6B18 void hdSysWin_setMessageId(HdMsgText* msg, u32 id, u32 option)
ext 025F6D70 void hdSysWin_setMessageLabel(HdMsgText* msg, hdUiSafeString* data, hdUiSafeString* label)
ext 025F50E8 void hdSysWin_buildMessage(void* builder, HdMsgText* msg, void* text, void* text2)
ext 027045F0 void hdSysWin_setTextBoxMessage(void* textBox, u32 id)
ext 026F7918 void hdSysWin_deactivateLoop(hdParts_c* part)
ext 026F7948 void hdSysWin_setLoopSound(hdParts_c* part, hdUiSafeString* label)
ext 026F826C void hdSysWin_hideKeyIcon(hdParts_c* part)
ext 02004E04 void hdSysWin_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group)
ext 025200D4 hdSysWinPlay_l* hdSysWin_getPlay()
ext 025986BC u8 hdSysWin_getMenuFlag()
ext 0259170C void hdSysWin_setMenuFlag(u8 flag)
data sSysWinSafeStringVtable 1010394C nocheck
data sSysWinStateIdVtable 10103C48 nocheck
data sSysWinNoStateId 101FF32C nocheck
data sSysWinPartsBlank 1049DD60 nocheck
data sSysWinUiSystem 1018C404 nocheck
data sSysWinMsgMgr 101F4B5C nocheck
data sSysWinMsgBuilder 101F4AE8 nocheck
data sSysWinWatched 101F5088 nocheck
data sSysWinController 101F8378 nocheck
data sSysWinOptions 1047B03C nocheck
data sSysWinScripts 101F6AE4 nocheck
data sSysWinNullString 104A0CD8 nocheck
data sSysWinLoadingSound 1049AFC4 nocheck
data sSysWinGroupNames 1049B028 nocheck
data sSysWinAnimNames 1049B048 nocheck
data sSysWinPartsPaneNames 1049B080 nocheck
data sSysWinPartsLayoutNames 1049B0A8 nocheck
data sSysWinArchiveNames 1049B0D0 nocheck
data sSysWinTextNames 1049B0F0 nocheck
data sSysWinCommandSounds 1049B288 nocheck
data sSysWinLayoutName 1049B720 nocheck
data sSysWinInAll 1049B420 nocheck
data sSysWinOutAll 1049B450 nocheck
data sSysWinOutWait 1049B480 nocheck
data sSysWinEndWait 1049B4B0 nocheck
data sSysWinEnd 1049B4E0 nocheck
data sSysWinButtonWait 1049B510 nocheck
data sSysWinDecideWait 1049B540 nocheck
data sSysWinOutButton 1049B570 nocheck
data sSysWinDrcWait 1049B5A0 nocheck
data sSysWinProWait 1049B5D0 nocheck
data sSysWinConnectedWait 1049B600 nocheck
data sSysWinSaveWait 1049B630 nocheck
data sSysWinSaveEnd 1049B660 nocheck
data sSysWinSaveInit 1049B690 nocheck
data sSysWinReset 1049B6C0 nocheck
data sSysWinResetEnd 1049B6F0 nocheck
data sSysWinAnimGroups 10103BD8
data sSysWinStateNumberReady 101FD8E4 nocheck
data sSysWinStateNumber 101FDD50 nocheck
str 101044BC "SE_NOW_LOADING" in 026DC4DC
str 10104418 "G_All_00" in 026DC4DC
str 1010420C "G_Form_00" in 026DC4DC
str 10104218 "G_Form_01" in 026DC4DC
str 1010405C "G_BG_00" in 026DC4DC
str 10104074 "InAll" in 026DC4DC
str 1010414C "FormDefault" in 026DC4DC
str 10104064 "FormDRC" in 026DC4DC
str 10104424 "FormProCon" in 026DC4DC
str 101040D4 "FormFrameCenter" in 026DC4DC
str 1010406C "OnOffBG" in 026DC4DC
str 101040CC "OutAll" in 026DC4DC
str 10104430 "L_BtnSeqenceL_00" in 026DC4DC
str 10104444 "L_BtnSeqenceR_00" in 026DC4DC
str 10104458 "L_BtnSeqenceC_00" in 026DC4DC
str 1010446C "L_LoadingIcon_00" in 026DC4DC
str 10104480 "L_KeyIcon_00" in 026DC4DC
str 10104490 "BtnSequence_01" in 026DC4DC
str 101044A0 "LoadingIcon_00" in 026DC4DC
str 101044B0 "KeyIcon_00" in 026DC4DC
str 10104224 "SequenceWindow_00" in 026DC4DC
str 10104238 "T_Msg_00_SAVE" in 026DC4DC
str 101044CC "T_Msg_00_NowSave" in 026DC4DC
str 10104248 "T_Msg_00_Continue" in 026DC4DC
str 101044E0 "T_Msg_00_Continue2" in 026DC4DC
str 1010425C "T_Msg_00_SAVE_Yes" in 026DC4DC
str 101044F4 "T_Msg_00_SAVE_No" in 026DC4DC
str 10104508 "T_Msg_00_Conti_Yes" in 026DC4DC
str 10104270 "T_Msg_00_Conti_No" in 026DC4DC
str 10104284 "T_Msg_00_WARP" in 026DC4DC
str 1010407C "00069" in 026DC4DC
str 10104084 "00070" in 026DC4DC
str 1010408C "00071" in 026DC4DC
str 10104094 "00072" in 026DC4DC
str 1010409C "00073" in 026DC4DC
str 101040A4 "00074" in 026DC4DC
str 101040AC "00075" in 026DC4DC
str 101040B4 "00067" in 026DC4DC
str 101040BC "00066" in 026DC4DC
str 10104294 "T_Msg_00_WARP_Yes" in 026DC4DC
str 1010451C "T_Msg_00_WARP_No" in 026DC4DC
str 101042A8 "T_Msg_00_ProCon01" in 026DC4DC
str 10104530 "T_Msg_00_DrcOn01" in 026DC4DC
str 10104158 "T_Msg_00_ProDrcBack" in 026DC4DC
str 10104544 "T_Msg_00_ContEnter01" in 026DC4DC
str 1010455C "T_Msg_00_ContEnter02" in 026DC4DC
str 10104574 "T_Msg_00_ContEnter03" in 026DC4DC
str 1010416C "T_Msg_00_Miiverse01" in 026DC4DC
str 101040E4 "T_Msg_00_Miiverse_yes01" in 026DC4DC
str 1010458C "T_Msg_00_Miiverse_no01" in 026DC4DC
str 10104180 "T_Msg_00_Miiverse02" in 026DC4DC
str 101040FC "T_Msg_00_Miiverse_yes02" in 026DC4DC
str 101045A4 "T_Msg_00_Miiverse_no02" in 026DC4DC
str 101040C4 "13027" in 026DC4DC
str 101045BC "T_Msg_00_Miiverse_info00" in 026DC4DC
str 10104194 "T_Msg_00_SaveInit00" in 026DC4DC
str 101041A8 "T_Msg_00_SaveInit01" in 026DC4DC
str 101041BC "T_Msg_00_SaveInit02" in 026DC4DC
str 101045D8 "T_Btn_SaveInit02" in 026DC4DC
str 101042BC "T_Msg_00_SecondInfo00" in 026DC4DC
str 101042D4 "T_Msg_00_SecondInfo01" in 026DC4DC
str 101042EC "T_Msg_00_SecondInfo02" in 026DC4DC
str 10104304 "T_Msg_00_SecondInfo03" in 026DC4DC
str 1010431C "T_Msg_00_SecondIBtn00" in 026DC4DC
str 10104334 "T_Msg_00_SecondIBtn01" in 026DC4DC
str 1010434C "T_Msg_00_SecondIBtn02" in 026DC4DC
str 10104364 "T_Msg_00_SecondIBtn03" in 026DC4DC
str 1010437C "T_Msg_00_SecondIBtn04" in 026DC4DC
str 101041D0 "T_Msg_00_hardmode00" in 026DC4DC
str 101041E4 "T_Button_hardmode01" in 026DC4DC
str 10104394 "SE_SAVE_START" in 026DC4DC
str 101045EC "SE_SAVE_CANCEL" in 026DC4DC
str 101045FC "SE_MENU_OPTION_OUT" in 026DC4DC
str 10104610 "SE_EXIT_GAME_YES" in 026DC4DC
str 10104624 "SE_MENU_DECIDE" in 026DC4DC
str 10104634 "SE_MENU_CANCEL" in 026DC4DC
str 10104114 "::StateID_InAll" in 026DC4DC
str 10104644 "::StateID_OutAll" in 026DC4DC
str 101043A4 "::StateID_OutWait" in 026DC4DC
str 101043B8 "::StateID_EndWait" in 026DC4DC
str 101043CC "::StateID_End" in 026DC4DC
str 10104658 "::StateID_ButtonWait" in 026DC4DC
str 10104670 "::StateID_DecideWait" in 026DC4DC
str 101041F8 "::StateID_OutButton" in 026DC4DC
str 101043DC "::StateID_DrcWait" in 026DC4DC
str 101043F0 "::StateID_ProWait" in 026DC4DC
str 10104124 "::StateID_ConnectedWait" in 026DC4DC
str 10104688 "::StateID_SaveWait" in 026DC4DC
str 10104404 "::StateID_SaveEnd" in 026DC4DC
str 1010469C "::StateID_SaveInit" in 026DC4DC
str 1010413C "::StateID_Reset" in 026DC4DC
str 101046B0 "::StateID_ResetEnd" in 026DC4DC
ext 026DB0A0 void hdSequenceWindow_c::findPanes()
ext 026DB284 void hdSequenceWindow_c::findTextPanes()
ext 026D9800 void hdSequenceWindow_c::selectInputButton()
ext 026D9C48 void hdSequenceWindow_c::updateButtons()
ext 026D989C void hdSequenceWindow_c::setButtonLabel(u32, u32)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString* names, u32 count)
ext 026F91F8 void hdParts_c::detachArchives()
*/
