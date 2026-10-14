/** HD: in-game UI scene transitions, menu input and mode-dependent screens.
 * Binary-backed companion implementation; private game-derived source. */
#include "d/hd_scene_ingame_ui_part2.h"

static cking::ui::input::EventMgr* sEventMgr;
static cking::ui::input::Options* sInputOptions;
static hdIngameController_l* sController;
static u32 sMenuStatus;
static u8 sSaveActive;
static u8 sTaktRequested;
struct hdIngameFramework_l;
static hdIngameFramework_l* sFramework;
struct hdIngameMessage_l {
    u8 mPrefix[0x938];
    be<u32> mMessageId;
};
WWHD_OFFSET(hdIngameMessage_l, mMessageId, 0x938);
static hdIngameMessage_l* sMessage;
struct hdIngameSave_l;
static hdIngameSave_l* sSave;
static hdIngameStateId_c l_ingameToPlayState;
static hdIngameStateId_c l_ingamePlayState;
static hdIngameStateId_c l_ingamePauseDrcState;
static hdIngameStateId_c l_ingameToMenuState;
static hdIngameStateId_c l_ingameMenuState;
static hdIngameStateId_c l_ingameMenuSubWindowState;
static hdIngameStateId_c l_ingameToSaveState;
static hdIngameStateId_c l_ingameSaveState;
static hdIngameStateId_c l_ingameToOptionState;
static hdIngameStateId_c l_ingameOptionState;
static hdIngameStateId_c l_ingameToTaktState;
static hdIngameStateId_c l_ingameTaktState;
static hdIngameStateId_c l_ingameReturnTaktForProModeState;
static hdIngameStateId_c l_ingameTaktPlayDemoState;
static hdIngameStateId_c l_ingameControlWindState;
static hdIngameStateId_c l_ingameToWarpState;
static hdIngameStateId_c l_ingameWarpState;
static hdIngameStateId_c l_ingamePhotoViewMiiverseState;
static hdIngameStateId_c l_ingameMiiverseEmpathingState;
static hdIngameStateId_c l_ingameToMiiverseSettingState;
static hdIngameStateId_c l_ingameMiiverseSettingState;
static hdIngameStateId_c l_ingameToConfirmMiiverseCommentJumpState;
static hdIngameStateId_c l_ingameConfirmMiiverseCommentJumpState;
static hdIngameStateId_c l_ingameToConfirmMiiverseProfileJumpState;
static hdIngameStateId_c l_ingameConfirmMiiverseProfileJumpState;
static hdIngameStateId_c l_ingameMiiverseProfileJumpState;
static hdIngameStateId_c l_ingameToMiiverseExplainState;
static hdIngameStateId_c l_ingameMiiverseExplainState;
static hdIngameStateId_c l_ingameToTelescopeState;
static hdIngameStateId_c l_ingameTelescopeState;
static hdIngameStateId_c l_ingameToPictographBoxState;
static hdIngameStateId_c l_ingameToFishManHintState;
static hdIngameStateId_c l_ingameInputPasswordState;
static hdIngameStateId_c l_ingameDemoTauraArrivalState;
static hdIngameStateId_c l_ingameToDemoFishManState;
static hdIngameStateId_c l_ingameToDemoTriforceState;
static hdIngameStateId_c l_ingameGameOverState;
static hdIngameStateId_c l_ingameChangeToTvModeState;
static hdIngameStateId_c l_ingameChangeToDrcModeState;
static hdIngameStateId_c l_ingameExchangeProModeState;
static hdIngameStateId_c l_ingameToTaktSignListState;
static hdIngameStateId_c l_ingameTaktSignListState;
static hdIngameStateId_c l_ingameToPictographListState;
static hdIngameStateId_c l_ingameToSelectPictographListState;
static hdIngameStateId_c l_ingameToPauseDrcState;
static hdIngameStateId_c l_ingamePictographBoxState;

static hdIngameStateId_c sPictographIdleState;
static hdIngameStateId_c sMenuButtonsBlankState;
static hdIngameStateId_c sMenuButtonsDisplayState;
static hdIngameStateId_c sWindGuideBlankState;
static hdIngameStateId_c sScreenBlankState;
static hdUiStringTable sSafeStringVtable;
static hdIngameSocialData_l* sSocialData;
static hdIngameMiiverse_l* sMiiverse;
static u8 sProfileJumpDone;
static u8 sProfileJumpSucceeded;
static hdIngamePictureCapture_l* sPictureCapture;
static hdIngameStateId_c sWarpButtonsOpenState;
static hdIngameStateId_c sWarpButtonsDoneState;
static hdIngameStateId_c sSubWindowPhotoState;
static hdIngameStateId_c sPictographBoxOpenState;
static hdIngameStateId_c sPictographBoxDisplayState;
static hdIngameStateId_c sPictographBoxCloseState;
static hdIngameStateId_c sPictographBoxCaptureState;
static hdIngameStateId_c sPictographBoxListState;
static hdIngameStateId_c sPictographOverlayListState;
static u8 inventoryItem(hdIngameSave_l* save, u32 index) {
    if (index < 0x15)
        return save->mInventory[index + 0x3C];
    if (index >= 0x18 && index < 0x20)
        return save->mInventory[index + 0x5E];
    if (index >= 0x24 && index < 0x2C)
        return save->mInventory[index + 0x5A];
    if (index >= 0x30 && index < 0x38)
        return save->mInventory[index + 0x56];
    return 0xFF;
}

hdIngamePlay_l* ingamePlay();
u32 ingameGameOverStart(void*);
u32 ingameIsEventRunning();
u32 ingameOverlapRequested();
void ingamePause(u32);
void ingameClearScreen(void*);
void ingameCloseNameEntry(void*);
void ingameClosePictograph(void*);
void ingameOpenPictograph(void*, u32);
void ingameSetSubWindow(void*);
void ingameBeginMenuButtons(void*);
void ingameEndMenuButtons(void*);
void ingameShowWindGuide(void*);
void ingameHideWindGuide(void*);
u32 ingameUsesTv(hdIngameController_l*);
u32 ingameUsesPro(hdIngameController_l*);
u32 ingameUsesDrc(hdIngameController_l*);
u32 ingameFrameworkBusy(hdIngameFramework_l*);
void ingameSound(const char*);
u32 ingameSoundPlaying();
void ingameSoundBegin();
void ingameSoundEnd();
void ingameSoundResume();
void ingameSoundModeEnd();
void ingameChangeState(hdStateMachine*, hdIngameStateId_c*);
hdIngameStateId_c* ingameCurrentState(hdStateMachine*);
void ingameReadEvent(void*, void*);
void ingameSubWindowPhoto(void*);
void ingameResetItemSelection(void*);
u32 ingameItemScreenSelected(void*);
u32 ingameMapCloseBlocked(void*);
void ingameItemScreenBack(void*);
void ingameBeginSave(void*, u32, u32);
void ingameBeginOption(void*, u32);
u32 ingameTactNormalWait(void*);
void ingameMapPrepare(void*);
u32 ingameHasSong(hdIngameCollect_l*, u32);
u32 ingameWindControlReady();
void ingameSetWindGuide(void*, u32);
void ingameOpenTaktSignList(void*);
u32 ingameTaktSignListReady(void*);
void ingameCloseTaktSignList(void*);
void ingameMapRestore(void*);
u32 ingameMapWindReady(void*);
void ingameSelectButton(void*);
void ingameMapOpenWind(void*);
void ingameSoundId(u32);
void ingameSoundWarpBegin();
void ingameSoundWarpEnd();
void ingameSubWindowEmpath(void*);
hdIngameFramework_l* ingameGetFramework();
void ingameFrameworkRunning(hdIngameFramework_l*, u32);
void ingameSubWindowEmpathEnd(void*);
void ingameMiiversePrepare(hdIngameMiiverse_l*);
u32 ingameSubWindowReceiver(void*);
void ingameMiiverseCommentJump(hdIngameMiiverse_l*, hdIngameComment_l*);
void ingameSubWindowSetMode(void*, u32);
void ingameSubWindowCancelJump(void*);
void ingameMiiverseProfileJump(hdIngameMiiverse_l*);
void ingameBeginExplain(void*);
void ingameSoundPictographBegin();
u32 ingamePictographBoxReady(void*);
void ingameClearMessage(hdIngameMessage_l*);
hdIngamePictureSave_l* ingameGetPictureSave(hdIngamePictures_l*);
void ingamePictographBoxToggle(void*);
u32 ingameHoldBit1(cking::ui::input::EventMgr*);
// Expected state's vtable is held across the first getKey call, as in GHS.
static bool stateMatches(hdStateMachine* machine, hdIngameStateId_c* expected) {
    hdIngameStateId_c* current = ingameCurrentState(machine);
    u32 method = current->mpVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedVtable = expected->mpVtable;
    u32 key = gabi::call_ptr<u32>(method, current);
    return key == gabi::call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
}

/* 0270D0F0 */
void hdIngameUi_c::setTvScreens(u32 focus) {
    u32 mode = mpItemScreen->mSelectionMode;
    u32 index = mode == 1 ? 29 : 18;
    hdParts_c* screen = mpItemScreen->child(index);
    if (focus)
        sEventMgr->setModeDefault(gabi::ea(reinterpret_cast<hdButtonPart_c*>(screen)->mpReceiver.get()));
    ingameClearScreen(screen);
}
/* 0270D17C */
void hdIngameUi_c::setDrcScreens(u32 focus) {
    u32 mode = mpItemScreen->mSelectionMode;
    u32 index = mode == 1 ? 30 : 19;
    hdParts_c* screen = mpItemScreen->child(index);
    if (focus)
        sEventMgr->setModeDefault(gabi::ea(reinterpret_cast<hdButtonPart_c*>(screen)->mpReceiver.get()));
    ingameClearScreen(screen);
}
/* 0270D208 */
void hdIngameUi_c::toPlayEnter() {
    mModeSwitchButtonsHeld = 0;
    sEventMgr->reset();
    ingameClosePictograph(mpPictographScreen);
    ingameCloseNameEntry(mpNameEntryScreen);
    setTvScreens(0);
    setDrcScreens(0);
    ingameSetSubWindow(mpSubWindow);
    sInputOptions->mDirectionalInputEnabled = 0;
    sController->mEnabled = 1;
    ingameSoundEnd();
    ingameSoundResume();
    ingameSoundModeEnd();
}
/* 0270D2A4 */
void hdIngameUi_c::toPlayExecute() {
    if (!stateMatches(mpPictographScreen->stateMachine(), &sPictographIdleState))
        return;
    ingamePause(0);
    sMenuStatus = 0;
    mTouchPanelRequested = 0;
    if (!stateMatches(mpMenuButtons->stateMachine(), &sMenuButtonsBlankState) &&
        !stateMatches(mpMenuButtons->stateMachine(), &sMenuButtonsDisplayState))
        ingameBeginMenuButtons(mpMenuButtons);
    ingameChangeState(&mState, &l_ingamePlayState);
}
/* 0270D400 */
void hdIngameUi_c::playEnter() {
    if (!stateMatches(mpWindGuide->stateMachine(), &sWindGuideBlankState) &&
        !stateMatches(mpWindGuide->stateMachine(), &sScreenBlankState))
        ingameHideWindGuide(mpWindGuide);
}
/* 0270D4E8 */
u32 hdIngameUi_c::isGameOver() {
    void* player = ingamePlay()->mpPlayer;
    return player ? ingameGameOverStart(player) != 0 : 0;
}
/* 0270D528 */
u32 hdIngameUi_c::isPasswordRequested() {
    if (ingameIsEventRunning())
        return 0;
    if (ingamePlay()->mRoom != 0)
        return 0;
    if (ingameOverlapRequested())
        return 0;
    return ingamePlay()->mPasswordState == 2;
}
/* 0270D594 */
u32 hdIngameUi_c::isFishManDemoRequested() {
    if (ingameIsEventRunning())
        return 0;
    if (ingamePlay()->mRoom != 0)
        return 0;
    if (ingameOverlapRequested())
        return 0;
    return ingamePlay()->mFishManDemo == 2;
}
/* 0270D600 */
u32 hdIngameUi_c::menuControlsBlocked() {
    if (!ingamePlay()->mpPlayer)
        return 0;
    if (ingamePlay()->mEventRunning)
        return 1;
    return (ingamePlay()->mUiFlags & 0x10) != 0;
}
/* 0270D664 */
u32 hdIngameUi_c::isDrcMode() {
    return menuControlsBlocked();
}
/* 0270D668 */
u32 hdIngameUi_c::handleModeChange() {
    hdIngameController_l* controller = sController;
    if (!(controller->mHold & 0x1000))
        return 0;
    u32 mode = controller->mMode;
    if (mode == 1)
        ingameChangeState(&mState, &l_ingameChangeToDrcModeState);
    else if (mode == 2)
        ingameChangeState(&mState, &l_ingameChangeToTvModeState);
    return 1;
}
/* 0270D70C */
void hdIngameUi_c::playExecute() {
    hdIngameEvent_l event;
    ingameReadEvent(&event, sEventMgr->mEventQueue + 0x1C);
    if (isGameOver()) {
        ingameChangeState(&mState, &l_ingameGameOverState);
        return;
    }
    if (ingamePlay()->mDemoRequested && !ingamePlay()->mDemoState) {
        ingameChangeState(&mState, &l_ingameToPauseDrcState);
        return;
    }
    if (sTaktRequested) {
        ingameShowWindGuide(mpWindGuide);
        ingameChangeState(&mState, &l_ingameToTaktState);
        return;
    }
    if (ingamePlay()->mPictographListState == 2) {
        ingameChangeState(&mState, &l_ingameToSelectPictographListState);
        return;
    }
    if (ingamePlay()->mUiFlags & 0x200000) {
        ingameChangeState(&mState, &l_ingameToTelescopeState);
        return;
    }
    if (ingamePlay()->mUiRequests & 8) {
        ingameChangeState(&mState, &l_ingameToPictographBoxState);
        return;
    }
    if (isPasswordRequested()) {
        ingameChangeState(&mState, &l_ingameInputPasswordState);
        return;
    }
    if (isFishManDemoRequested()) {
        ingameChangeState(&mState, &l_ingameToDemoFishManState);
        return;
    }
    if (mMiiverseExplainRequested) {
        ingameChangeState(&mState, &l_ingameToMiiverseExplainState);
        return;
    }
    if (isDrcMode()) {
        if (event.mKind == 0x68)
            ingameSubWindowPhoto(mpSubWindow);
        ingameChangeState(&mState, &l_ingameToPauseDrcState);
        return;
    }
    if (handleModeChange()) {
        mModeSwitchButtonsHeld = 0;
        return;
    }
    if (ingameUsesTv(sController)) {
        if (event.mKind == 0x62)
            ingameChangeState(&mState, &l_ingameToMenuState);
        return;
    }
    if (!ingamePlay()->mMenuAllowed)
        return;
    switch (event.mKind) {
    case 0x2a:
        ingameChangeState(&mState, &l_ingameToSaveState);
        break;
    case 0x2b:
        ingameChangeState(&mState, &l_ingameToOptionState);
        break;
    case 0x62:
        ingameChangeState(&mState, &l_ingameToMenuState);
        break;
    case 0x66:
        if (!ingameFrameworkBusy(sFramework))
            ingameChangeState(&mState, &l_ingameToConfirmMiiverseCommentJumpState);
        break;
    case 0x67:
        if (!ingameFrameworkBusy(sFramework))
            ingameChangeState(&mState, &l_ingameMiiverseEmpathingState);
        break;
    case 0x68:
        if (!ingameFrameworkBusy(sFramework))
            ingameChangeState(&mState, &l_ingamePhotoViewMiiverseState);
        break;
    case 0x69:
        ingameChangeState(&mState, &l_ingameToConfirmMiiverseProfileJumpState);
        break;
    case 0x6a:
        ingameChangeState(&mState, &l_ingameToMiiverseSettingState);
        break;
    case 0x6d:
        ingameChangeState(&mState, &l_ingameToFishManHintState);
        break;
    }
}
/* 0270DB88 */
void hdIngameUi_c::toPauseDrcEnter() {
    sMenuStatus = 4;
    ingameOpenPictograph(mpPictographScreen, 0);
    mPauseStartedDuringDemo = 0;
    if (ingamePlay()->mpDemo)
        mPauseStartedDuringDemo = ingamePlay()->mpDemo->mFlags >> 31;
    sEventMgr->reset();
}
/* 0270DC00 */
void hdIngameUi_c::toPauseDrcExecute() {
    ingameChangeState(&mState, &l_ingamePauseDrcState);
}
/* 0270DC10 */
void hdIngameUi_c::pauseDrcEnter() {
    mPausePhase = 0;
    mCountdown = 10;
}
/* 0270DC24 */
void hdIngameUi_c::clearGTowerItems() {
    hdUiSafeString towerName;
    towerName.mText.v = gabi::guest_addr("GTower");
    towerName.mVtable = &sSafeStringVtable;
    hdUiSafeString stageName;
    stageName.mText = &ingamePlay()->mStageName[0];
    stageName.mVtable = &sSafeStringVtable;
    gabi::call_ptr(sSafeStringVtable.mAssureTermination.get(), &towerName);
    gabi::call_ptr(towerName.mVtable->mAssureTermination.get(), &towerName);
    be<u8>* tower = towerName.mText;
    gabi::call_ptr(stageName.mVtable->mAssureTermination.get(), &stageName);
    be<u8>* stage = stageName.mText;
    bool same = tower == stage;
    if (!same) {
        tower = towerName.mText;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 a = tower[i];
            u8 b = stage[i];
            if (a != b)
                break;
            if (a == 0) {
                same = true;
                break;
            }
        }
    }
    if (!same || sMessage->mMessageId != 0x3562)
        return;
    sSave->mInventory[0x48] = 0xff;
    for (u32 slot = 0; slot < 3; ++slot) {
        if (ingamePlay()->mAssignedItems[slot] != 0x27 &&
            ingamePlay()->mAssignedItems[slot] != 0x35 &&
            ingamePlay()->mAssignedItems[slot] != 0x36)
            continue;
        sSave->mInventory[9 + slot] = 0xff;
        u8 assigned = sSave->mInventory[9 + slot];
        hdIngamePlay_l* play = ingamePlay();
        if (assigned == 0xff) {
            play->mAssignedItems[slot] = 0xff;
            continue;
        }
        hdIngameSave_l* save = sSave;
        u8 index = save->mInventory[9 + slot];
        play->mAssignedItems[slot] = inventoryItem(save, index);
        save = sSave;
        index = save->mInventory[9 + slot];
        if (inventoryItem(save, index) == 0xff)
            save->mInventory[9 + slot] = 0xff;
    }
}
/* 0270DF60 */
u32 hdIngameUi_c::isTauraArrivalRequested() {
    if (!mUiLocked && sMessage->mMessageId == 0x61b) {
        mUiLocked = 1;
        return 1;
    }
    return 0;
}
/* 0270DF9C */
u32 hdIngameUi_c::isTriforceDemoRequested() {
    if (ingameIsEventRunning())
        return 0;
    if (ingamePlay()->mRoom != 0)
        return 0;
    if (ingameOverlapRequested())
        return 0;
    return ingamePlay()->mTriforceDemo != 0;
}
/* 0270E008 */
void hdIngameUi_c::pauseDrcExecute() {
    clearGTowerItems();
    switch (mPausePhase) {
    case 0: {
        u32 timer = mCountdown;
        mCountdown = timer - 1;
        if (timer == 0)
            mPausePhase = 4;
        return;
    }
    case 4:
        if (isGameOver()) {
            ingameChangeState(&mState, &l_ingameGameOverState);
            return;
        }
        if (sTaktRequested) {
            ingameShowWindGuide(mpWindGuide);
            ingameChangeState(&mState, &l_ingameToTaktState);
            return;
        }
        if (ingamePlay()->mPictographListState == 2) {
            ingameChangeState(&mState, &l_ingameToSelectPictographListState);
            return;
        }
        if (ingamePlay()->mUiRequests & 8) {
            ingameChangeState(&mState, &l_ingameToPictographBoxState);
            return;
        }
        if (isPasswordRequested()) {
            ingameChangeState(&mState, &l_ingameInputPasswordState);
            return;
        }
        if (isTauraArrivalRequested()) {
            ingameChangeState(&mState, &l_ingameDemoTauraArrivalState);
            return;
        }
        if (isFishManDemoRequested()) {
            ingameChangeState(&mState, &l_ingameToDemoFishManState);
            return;
        }
        if (isTriforceDemoRequested()) {
            ingameChangeState(&mState, &l_ingameToDemoTriforceState);
            return;
        }
        if (handleModeChange()) {
            mModeSwitchButtonsHeld = 0;
            return;
        }
        if (mMiiverseExplainRequested) {
            ingameChangeState(&mState, &l_ingameToMiiverseExplainState);
            return;
        }
        if (!isDrcMode()) {
            mCountdown = mPauseStartedDuringDemo ? 40 : 10;
            mPausePhase = 5;
        }
        return;
    case 5: {
        u32 timer = mCountdown;
        mCountdown = timer - 1;
        if (timer == 0) {
            ingameChangeState(&mState, &l_ingameToPlayState);
            return;
        }
        if (menuControlsBlocked())
            mPausePhase = 4;
        return;
    }
    }
}
/* 0270E314 */
void hdIngameUi_c::pauseDrcExit() {
    mPauseStartedDuringDemo = 0;
}
/* 0270E320 */
void hdIngameUi_c::prepareMapCoordinator() {
    u32 mode = sController->mMode;
    if (mode == 0) {
        sMenuStatus = 4;
        ingameOpenPictograph(mpPictographScreen, 1);
    } else if (mode == 1) {
        sMenuStatus = 2;
        ingameOpenPictograph(mpPictographScreen, 0);
    } else if (mode == 2) {
        sMenuStatus = 1;
        ingameOpenPictograph(mpPictographScreen, 0);
    }
}
/* 0270E384 */
void hdIngameUi_c::toMenuEnter() {
    ingameCloseNameEntry(mpNameEntryScreen);
    prepareMapCoordinator();
    mModeSwitchButtonsHeld = 0;
    setTvScreens(0);
    setDrcScreens(0);
    ingameSetSubWindow(mpSubWindow);
}
/* 0270E3E8 */
void hdIngameUi_c::toMenuExecute() {
    ingamePause(1);
    sInputOptions->mDirectionalInputEnabled = 1;
    if (mMenuSelection == 0)
        ingameResetItemSelection(mpItemScreen);
    ingameBeginMenuButtons(mpMenuButtons);
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    ingameChangeState(&mState, &l_ingameMenuState);
}
/* 0270E474 */
u32 hdIngameUi_c::canCloseItemScreen() {
    if (mpItemScreen->mWindowMode != 3)
        return 1;
    if (ingameItemScreenSelected(mpItemScreen))
        return 1;
    return ingameMapCloseBlocked(mpMapCoordinator) != 0;
}
/* 0270E4E4 */
void hdIngameUi_c::menuExecute() {
    if (canCloseItemScreen()) {
        ingameChangeState(&mState, &l_ingameMenuSubWindowState);
        return;
    }
    hdIngameEvent_l event;
    ingameReadEvent(&event, sEventMgr->mEventQueue + 0x1C);
    bool close = false;
    switch (event.mKind) {
    case 0x2a:
        sEventMgr->setModeDefault(0);
        ingameChangeState(&mState, &l_ingameToSaveState);
        return;
    case 0x2b:
        sEventMgr->setModeDefault(0);
        ingameChangeState(&mState, &l_ingameToOptionState);
        return;
    case 0x62:
        close = true;
        if (ingameItemScreenSelected(mpItemScreen))
            ingameItemScreenBack(mpItemScreen);
        break;
    case 0x66:
        ingameChangeState(&mState, &l_ingameToConfirmMiiverseCommentJumpState);
        return;
    case 0x67:
        ingameChangeState(&mState, &l_ingameMiiverseEmpathingState);
        return;
    case 0x68:
        ingameChangeState(&mState, &l_ingamePhotoViewMiiverseState);
        break;
    case 0x69:
        ingameChangeState(&mState, &l_ingameToConfirmMiiverseProfileJumpState);
        return;
    case 0x6a:
        ingameChangeState(&mState, &l_ingameToMiiverseSettingState);
        return;
    case 0x6d:
        ingameChangeState(&mState, &l_ingameToFishManHintState);
        return;
    }
    if (sController->mHold & 2) {
        if (!ingameHoldBit1(sEventMgr))
            close = true;
    }
    if (!close)
        return;
    sInputOptions->mDirectionalInputEnabled = 0;
    if (mMenuSelection == 0)
        ingameResetItemSelection(mpItemScreen);
    if (ingameUsesTv(sController))
        ingameEndMenuButtons(mpMenuButtons);
    ingameChangeState(&mState, &l_ingameToPlayState);
    ingameSound("SE_MENU_OUT");
    ingameSoundEnd();
}
/* 0270E840 */
void hdIngameUi_c::menuSubWindowEnter() {
    prepareMapCoordinator();
}
/* 0270E844 */
void hdIngameUi_c::completeHint() {
    u32 mode = sController->mMode;
    if (mode < 2) {
        if (ingameIsEventRunning() && !mMenuRequested)
            ingameChangeState(&mState, &l_ingameToMenuState);
        else {
            ingameChangeState(&mState, &l_ingameToPlayState);
            if (!mMenuRequested)
                ingameSound("SE_MENU_OUT");
            ingameSoundEnd();
        }
    } else if (mode == 2) {
        if (!mMenuRequested && !mMiiverseExplainRequested)
            ingameChangeState(&mState, &l_ingameToMenuState);
        else
            ingameChangeState(&mState, &l_ingameToPlayState);
    }
    ingameSoundModeEnd();
}
/* 0270E920 */
void hdIngameUi_c::menuSubWindowExecute() {
    if (!canCloseItemScreen()) {
        completeHint();
        return;
    }
    hdIngameEvent_l event;
    ingameReadEvent(&event, sEventMgr->mEventQueue + 0x1C);
    if (event.mKind != 0x62)
        return;
    if (ingameItemScreenSelected(mpItemScreen))
        ingameItemScreenBack(mpItemScreen);
    if (mMenuSelection == 0)
        ingameResetItemSelection(mpItemScreen);
    ingameChangeState(&mState, &l_ingameToPlayState);
    ingameSound("SE_MENU_OUT");
    ingameSoundEnd();
}
/* 0270E9DC */
void hdIngameUi_c::toSaveEnter() {
    if (!mMenuRequested) {
        prepareMapCoordinator();
        return;
    }
    hdIngameController_l* controller = sController;
    sMenuStatus = 4;
    if (!ingameUsesTv(controller))
        sController->mEnabled = 0;
}
/* 0270EA50 */
void hdIngameUi_c::toSaveExecute() {
    ingameBeginSave(mpSaveScreen, 0, 0);
    if (!mMenuRequested)
        sSaveActive = 1;
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    ingameChangeState(&mState, &l_ingameSaveState);
}
/* 0270EAD0 */
void hdIngameUi_c::saveExecute() {
    hdSequenceWindow_c* save = mpSaveScreen;
    if (stateMatches(save->stateMachine(), &sScreenBlankState) || save->mAdvanceRequested) {
        sSaveActive = 0;
        setTvScreens(1);
        completeHint();
    }
}
/* 0270EB88 */
void hdIngameUi_c::saveExit() {
    sController->mEnabled = 1;
}
/* 0270EB9C */
void hdIngameUi_c::toOptionEnter() {
    prepareMapCoordinator();
}
/* 0270EBA0 */
void hdIngameUi_c::toOptionExecute() {
    ingameBeginOption(mpOptionScreen, 0);
    sSaveActive = 1;
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    ingameChangeState(&mState, &l_ingameOptionState);
}
/* 0270EC10 */
void hdIngameUi_c::optionExecute() {
    if (stateMatches(mpOptionScreen->stateMachine(), &sScreenBlankState)) {
        sSaveActive = 0;
        setDrcScreens(1);
        completeHint();
        return;
    }
    if (mpOptionScreen->mCloseRequested)
        ingameChangeState(&mState, &l_ingameExchangeProModeState);
}
/* 0270ECF0 */
void hdIngameUi_c::toTaktEnter() {
    mTaktNormalWait = 1;
    if (ingamePlay()->mpPlayer)
        mTaktNormalWait = ingameTactNormalWait(ingamePlay()->mpPlayer) != 0;
    sEventMgr->reset();
    setTvScreens(0);
    setDrcScreens(0);
    ingameClosePictograph(mpPictographScreen);
    ingameMapPrepare(mpMapCoordinator);
    mAvailableSongs = 0;
    for (u32 song = 0; song < 6; ++song) {
        u32 obtained = ingameHasSong(&sSave->mCollect, song);
        u8 songs = mAvailableSongs;
        mAvailableSongs = obtained ? songs | (1u << song) : songs & ~(1u << song);
    }
}
/* 0270EF74 */
void hdIngameUi_c::toTaktExecute() {
    if (stateMatches(mpPictographScreen->stateMachine(), &sPictographIdleState)) {
        sMenuStatus = 0;
        ingameChangeState(&mState, &l_ingameTaktState);
    }
}
/* 0270F014 */
void hdIngameUi_c::taktEnter() {
    ingameMapPrepare(mpMapCoordinator);
}
/* 0270F01C */
void hdIngameUi_c::taktExecute() {
    hdIngameEvent_l event;
    ingameReadEvent(&event, sEventMgr->mEventQueue + 0x1C);
    if (event.mKind == 0x62 && mAvailableSongs) {
        ingameChangeState(&mState, &l_ingameToTaktSignListState);
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
        mModeSwitchButtonsHeld = 0;
        return;
    }
    hdIngameDemo_l* demo = ingamePlay()->mpDemo;
    u32 song = gabi::call_ptr<u32>(demo->mpVtable->mGetSong.get(), demo);
    if (song == 0 && mTaktNormalWait) {
        if (ingameWindControlReady() && !ingameUsesPro(sController))
            ingameHideWindGuide(mpWindGuide);
        else
            ingameSetWindGuide(mpWindGuide, 0);
        ingameChangeState(&mState, &l_ingameControlWindState);
        return;
    }
    if (song == 1 && mTaktNormalWait) {
        ingameChangeState(&mState, &l_ingameToWarpState);
        return;
    }
    if (song <= 5) {
        ingameChangeState(&mState, &l_ingameTaktPlayDemoState);
        return;
    }
    if (sTaktRequested)
        return;
    ingameHideWindGuide(mpWindGuide);
    if (ingamePlay()->mEventRunning)
        ingameChangeState(&mState, &l_ingameToPauseDrcState);
    else
        ingameChangeState(&mState, &l_ingameToPlayState);
}
/* 0270F210 */
void hdIngameUi_c::toTaktSignListEnter() {
    prepareMapCoordinator();
    ingameOpenTaktSignList(mpWindGuide);
}
/* 0270F244 */
void hdIngameUi_c::toTaktSignListExecute() {
    if (ingameTaktSignListReady(mpWindGuide))
        ingameChangeState(&mState, &l_ingameTaktSignListState);
}
/* 0270F290 */
void hdIngameUi_c::taktSignListExecute() {
    hdIngameEvent_l event;
    ingameReadEvent(&event, sEventMgr->mEventQueue + 0x1C);
    bool close = event.mKind == 0x62;
    hdIngameController_l* controller = sController;
    if (!(controller->mHold & 2) && !close)
        return;
    ingameCloseTaktSignList(mpWindGuide);
    if (ingameUsesPro(sController))
        ingameChangeState(&mState, &l_ingameReturnTaktForProModeState);
    else
        ingameChangeState(&mState, &l_ingameToTaktState);
    ingameSound("SE_MENU_OUT");
    ingameSoundEnd();
    mModeSwitchButtonsHeld = 0;
}
/* 0270F37C */
void hdIngameUi_c::returnTaktForProModeExecute() {
    ingameChangeState(&mState, &l_ingameToTaktState);
}
/* 0270F38C */
void hdIngameUi_c::refreshMenuSelection() {
    u32 selected = mTransitionFrames;
    if (selected > 2)
        return;
    u32 count = mpMenuButtons->mChildren.mNum;
    hdMenuButton_c* button = mpMenuButtons->button(count > selected ? selected : 0);
    if (selected == 0) {
        u32 mode = gabi::ea(button->mButton.mpReceiver.get());
        sEventMgr->setModeDefault(mode);
    } else {
        cking::ui::input::EventMgr* mgr = sEventMgr;
        mgr->setModeDefault(gabi::ea(button->mButton.mpReceiver.get()));
    }
}
/* 0270F424 */
void hdIngameUi_c::taktPlayDemoEnter() {
    ingameSetWindGuide(mpWindGuide, 0);
    ingameMapRestore(mpMapCoordinator);
    refreshMenuSelection();
    mPausePhase = 0;
}
/* 0270F470 */
void hdIngameUi_c::taktPlayDemoExecute() {
    if (mPausePhase == 0) {
        if (!isDrcMode()) {
            ingameHideWindGuide(mpWindGuide);
            mPausePhase = 5;
        }
    } else if (mPausePhase == 5 && stateMatches(mpWindGuide->stateMachine(), &sScreenBlankState))
        ingameChangeState(&mState, &l_ingameToPlayState);
}
/* 0270F568 */
void hdIngameUi_c::controlWindEnter() {
    mPausePhase = 0;
    if (!ingameUsesDrc(sController))
        return;
    if (mMenuSelection != 1) {
            if (!mpMapCoordinator->mMode || ingameMapWindReady(mpMapCoordinator)) {
                    mTransitionFrames = 1;
            ingameSelectButton(mpMenuButtons->button(1));
            return;
        }
        if (mMenuSelection != 1)
            return;
    }
    if (ingameMapWindReady(mpMapCoordinator))
        ingameMapOpenWind(mpMapCoordinator);
}
/* 0270F634 */
void hdIngameUi_c::controlWindExecute() {
    switch (mPausePhase) {
    case 0:
        if (ingamePlay()->mWindControlState == 2) {
            if (ingameUsesPro(sController)) {
                sMenuStatus = 4;
                ingameOpenPictograph(mpPictographScreen, 1);
            }
            mPausePhase = 4;
            return;
        }
        if (!ingameWindControlReady()) {
            mPausePhase = 5;
            return;
        }
        if (!(mAvailableSongs & 1) && !sTaktRequested) {
            ingameHideWindGuide(mpWindGuide);
            ingameChangeState(&mState, &l_ingameToPauseDrcState);
        }
        return;
    case 4: {
        sInputOptions->mDirectionalInputEnabled = 1;
        u8 wind = ingamePlay()->mWindControlState;
        if (wind == 0) {
            sInputOptions->mDirectionalInputEnabled = 0;
            ingameMapRestore(mpMapCoordinator);
            ingameChangeState(&mState, &l_ingameToPlayState);
        } else if (wind == 1) {
            sInputOptions->mDirectionalInputEnabled = 0;
            ingameMapRestore(mpMapCoordinator);
            mPausePhase = 5;
        }
        return;
    }
    case 5:
        if (!ingamePlay()->mEventRunning) {
            if (!ingameWindControlReady())
                ingameHideWindGuide(mpWindGuide);
            ingameChangeState(&mState, &l_ingameToPlayState);
        }
        return;
    }
}
/* 0270F86C */
void hdIngameUi_c::controlWindExit() {
    if (!ingameUsesTv(sController))
        ingameHideWindGuide(mpWindGuide);
    refreshMenuSelection();
}
/* 0270F8B8 */
void hdIngameUi_c::toWarpEnter() {
    mTouchPanelRequested = 1;
    ingameSetWindGuide(mpWindGuide, 0);
    mPausePhase = 0;
}
/* 0270F8FC */
void hdIngameUi_c::toWarpExecute() {
    if (!ingamePlay()->mEventRunning) {
        ingameHideWindGuide(mpWindGuide);
        ingameMapRestore(mpMapCoordinator);
        refreshMenuSelection();
        ingameChangeState(&mState, &l_ingameToPlayState);
        return;
    }
    if (ingamePlay()->mFishManDemo != 1)
        return;
    ingamePlay()->mFishManDemo = 0;
    ingameSoundId(0x8cb);
    sTaktRequested = 0;
    ingameChangeState(mpWarpButtons->stateMachine(), &sWarpButtonsOpenState);
    sSaveActive = 1;
    if (!ingameUsesTv(sController))
        mpNameEntryScreen->mCaptureKind = 7;
    if (!ingameSoundPlaying())
        ingameSoundBegin();
    ingameChangeState(&mState, &l_ingameWarpState);
}
/* 0270F9F8 */
void hdIngameUi_c::warpEnter() {
    sInputOptions->mDirectionalInputEnabled = 1;
    if (!ingameUsesTv(sController))
        sController->mEnabled = 0;
    hdIngameController_l* controller = sController;
    sMenuStatus = 4;
    if (ingameUsesPro(controller))
        ingameOpenPictograph(mpPictographScreen, 1);
    ingameSoundWarpBegin();
}
/* 0270FA84 */
void hdIngameUi_c::warpExecute() {
    if (!stateMatches(mpWarpButtons->stateMachine(), &sWarpButtonsDoneState))
        return;
    sSaveActive = 0;
    setTvScreens(1);
    ingameMapRestore(mpMapCoordinator);
    refreshMenuSelection();
    ingameHideWindGuide(mpWindGuide);
    ingameChangeState(&mState, &l_ingameToPlayState);
}
/* 0270FB48 */
void hdIngameUi_c::warpExit() {
    sController->mEnabled = 1;
    sInputOptions->mDirectionalInputEnabled = 0;
    ingameSoundWarpEnd();
}
/* 0270FB6C */
void hdIngameUi_c::photoViewMiiverseEnter() {
    ingameChangeState(mpSubWindow->stateMachine(), &sSubWindowPhotoState);
    sInputOptions->mDirectionalInputEnabled = 1;
    prepareMapCoordinator();
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
}
/* 0270FBDC */
void hdIngameUi_c::photoViewMiiverseExecute() {
    if (stateMatches(mpSubWindow->stateMachine(), &sSubWindowPhotoState))
        return;
    if (ingameIsEventRunning())
        ingameChangeState(&mState, &l_ingameToMenuState);
    else {
        ingameChangeState(&mState, &l_ingameToPlayState);
        ingameSound("SE_MENU_OUT");
        ingameSoundEnd();
    }
}
/* 0270FCB4 */
void hdIngameUi_c::miiverseEmpathingEnter() {
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    if (ingameUsesPro(sController))
        ingamePause(1);
    sInputOptions->mDirectionalInputEnabled = 0;
    prepareMapCoordinator();
    ingameSubWindowEmpath(mpSubWindow);
    ingameFrameworkRunning(ingameGetFramework(), 0);
}
/* 0270FD48 */
void hdIngameUi_c::miiverseEmpathingExecute() {
    if (sMiiverse->mState != 4) {
        ingameSubWindowEmpathEnd(mpSubWindow);
        completeHint();
    }
}
/* 0270FD94 */
void hdIngameUi_c::miiverseEmpathingExit() {
    ingameFrameworkRunning(ingameGetFramework(), 1);
}
/* 0270FDBC */
void hdIngameUi_c::toMiiverseSettingEnter() {
    prepareMapCoordinator();
    ingameMiiversePrepare(sMiiverse);
}
/* 0270FDE8 */
void hdIngameUi_c::toMiiverseSettingExecute() {
    ingameBeginOption(mpOptionScreen, 1);
    sSaveActive = 1;
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    ingameChangeState(&mState, &l_ingameMiiverseSettingState);
}
/* 0270FE58 */
void hdIngameUi_c::miiverseSettingExecute() {
    if (stateMatches(mpOptionScreen->stateMachine(), &sScreenBlankState)) {
        sSaveActive = 0;
        u32 mode = ingameSubWindowReceiver(mpSubWindow);
        sEventMgr->setModeDefault(mode);
        completeHint();
    }
}
/* 0270FF08 */
void hdIngameUi_c::miiverseSettingExit() {
    mMiiverseExplainRequested = 0;
}
/* 0270FF14 */
void hdIngameUi_c::toConfirmMiiverseCommentJumpEnter() {
    hdIngameEvent_l event;
    ingameReadEvent(&event, sEventMgr->mEventQueue + 0x1C);
    u32 index = event.mPayload;
    if (index >= 11)
        index = 0;
    mModeExchangePhase = index;
    prepareMapCoordinator();
}
/* 0270FF6C */
void hdIngameUi_c::toConfirmMiiverseCommentJumpExecute() {
    ingameBeginSave(mpSaveScreen, 9, 0);
    sSaveActive = 1;
    if (ingameUsesPro(sController))
        ingamePause(1);
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    ingameChangeState(&mState, &l_ingameConfirmMiiverseCommentJumpState);
}
/* 0270FFF8 */
void hdIngameUi_c::confirmMiiverseCommentJumpEnter() {
    sInputOptions->mDirectionalInputEnabled = 1;
}
/* 0271000C */
void hdIngameUi_c::confirmMiiverseCommentJumpExecute() {
    hdSequenceWindow_c* save = mpSaveScreen;
    if (!stateMatches(save->stateMachine(), &sScreenBlankState) && !save->mAdvanceRequested)
        return;
    sSaveActive = 0;
    if (mpSaveScreen->mSaveRequested) {
        u32 index = mModeExchangePhase;
        hdIngameSocialData_l* data = sSocialData;
        hdIngameComment_l* comment = &data->mComments[index < 10 ? index : 0].mComment;
        if (gabi::ea(comment) != 0)
            ingameMiiverseCommentJump(sMiiverse, comment);
        ingameSubWindowSetMode(mpSubWindow, 0);
        completeHint();
    } else {
        ingameSubWindowCancelJump(mpSubWindow);
        completeHint();
    }
}
/* 0271011C */
void hdIngameUi_c::toConfirmMiiverseProfileJumpEnter() {
    prepareMapCoordinator();
}
/* 02710120 */
void hdIngameUi_c::toConfirmMiiverseProfileJumpExecute() {
    ingameBeginSave(mpSaveScreen, 10, 0);
    sSaveActive = 1;
    if (ingameUsesPro(sController))
        ingamePause(1);
    if (!ingameSoundPlaying()) {
        ingameSound("SE_MENU_IN");
        ingameSoundBegin();
    }
    ingameChangeState(&mState, &l_ingameConfirmMiiverseProfileJumpState);
}
/* 027101AC */
void hdIngameUi_c::confirmMiiverseProfileJumpEnter() {
    sInputOptions->mDirectionalInputEnabled = 1;
}
/* 027101C0 */
void hdIngameUi_c::confirmMiiverseProfileJumpExecute() {
    hdSequenceWindow_c* save = mpSaveScreen;
    if (!stateMatches(save->stateMachine(), &sScreenBlankState) && !save->mAdvanceRequested)
        return;
    sSaveActive = 0;
    if (mpSaveScreen->mSaveRequested)
        ingameChangeState(&mState, &l_ingameMiiverseProfileJumpState);
    else {
        completeHint();
        ingameSubWindowCancelJump(mpSubWindow);
    }
}
/* 027102B4 */
void hdIngameUi_c::miiverseProfileJumpEnter() {
    ingameMiiverseProfileJump(sMiiverse);
}
/* 027102C0 */
void hdIngameUi_c::miiverseProfileJumpExecute() {
    if (!sProfileJumpDone)
        return;
    if (sProfileJumpSucceeded)
        sSocialData->mProfileJumpPending = 0;
    completeHint();
    ingameSubWindowCancelJump(mpSubWindow);
}
/* 02710328 */
void hdIngameUi_c::toMiiverseExplainEnter() {
    if (mMenuSelection != 2) {
        mTransitionFrames = 2;
            mTransitionPhase = 20;
        ingameSelectButton(mpMenuButtons->button(2));
    } else
        mTransitionPhase = 0;
    ingameClosePictograph(mpPictographScreen);
    mpPictographScreen->mInputMode = 1;
    sMenuStatus = 0;
    mPausePhase = 0;
}
/* 027103E0 */
void hdIngameUi_c::toMiiverseExplainExecute() {
    switch (mPausePhase) {
    case 0:
        if (stateMatches(mpPictographScreen->stateMachine(), &sPictographIdleState)) {
            prepareMapCoordinator();
            mPausePhase = 4;
        }
        return;
    case 4:
        mPausePhase = 5;
        return;
    case 5: {
        u32 timer = mTransitionPhase;
        mTransitionPhase = timer - 1;
        if (timer)
            return;
        ingameBeginExplain(mpSaveScreen);
        ingamePause(1);
        sSaveActive = 1;
        if (!ingameSoundPlaying()) {
            ingameSound("SE_MENU_IN");
            ingameSoundBegin();
        }
        ingameChangeState(&mState, &l_ingameMiiverseExplainState);
        return;
    }
    }
}
/* 0271054C */
void hdIngameUi_c::miiverseExplainExecute() {
    hdSequenceWindow_c* save = mpSaveScreen;
    if (stateMatches(save->stateMachine(), &sScreenBlankState) || save->mAdvanceRequested) {
        ingameMapRestore(mpMapCoordinator);
        ingameChangeState(&mState, &l_ingameToMiiverseSettingState);
    }
}
/* 027105FC */
void hdIngameUi_c::miiverseExplainExit() {
    mpPictographScreen->mInputMode = 0;
    sInputOptions->mDirectionalInputEnabled = 1;
}
/* 0271061C */
void hdIngameUi_c::toTelescopeEnter() {
    sEventMgr->reset();
    sMenuStatus = 4;
}
/* 02710650 */
void hdIngameUi_c::toTelescopeExecute() {
    ingameChangeState(&mState, &l_ingameTelescopeState);
}
/* 02710664 */
void hdIngameUi_c::telescopeExecute() {
    if (!(ingamePlay()->mUiFlags & 0x200000))
        ingameChangeState(&mState, &l_ingameToPlayState);
    else if (ingamePlay()->mCurrentItem == 13 && ingamePlay()->mEventRunning)
        ingameChangeState(&mState, &l_ingameToPauseDrcState);
}
/* 027106E0 */
void hdIngameUi_c::toPictographBoxEnter() {
    cking::ui::input::EventMgr* manager = sEventMgr;
    sMenuStatus = 4;
    manager->reset();
    ingameChangeState(mpMenu->stateMachine(), &sPictographBoxOpenState);
    ingameSoundPictographBegin();
}
/* 02710738 */
void hdIngameUi_c::toPictographBoxExecute() {
    if (ingamePictographBoxReady(mpMenu))
        ingameChangeState(&mState, &l_ingamePictographBoxState);
}
/* 02710784 */
void hdIngameUi_c::pictographBoxExecute() {
    hdIngameController_l* controller = sController;
    be<u32>* held = &controller->mHold;
    hdIngameStateId_c* current = ingameCurrentState(mpMenu->stateMachine());
    u32 method = current->mpVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedVtable = sPictographBoxDisplayState.mpVtable;
    u32 currentKey = gabi::call_ptr<u32>(method, current);
    u32 expectedKey =
        gabi::call_ptr<u32>(expectedVtable->mGetKey.get(), &sPictographBoxDisplayState);
    bool close = currentKey == expectedKey && (sController->mHold & 2);
    if (!close)
        close = !(ingamePlay()->mUiRequests & 8);
    if (close) {
        ingamePlay()->mCurrentItem = 11;
        hdIngameDemo_l* demo = ingamePlay()->mpDemo;
        demo->mPictureFlags = demo->mPictureFlags | 0x80000;
        ingamePlay()->mPictographListState = 0;
        ingameChangeState(mpMenu->stateMachine(), &sPictographBoxCloseState);
        ingameClearMessage(sMessage);
        ingameChangeState(&mState, &l_ingameToPlayState);
        return;
    }
    if (sPictureCapture->mState)
        return;
    current = ingameCurrentState(mpMenu->stateMachine());
    currentKey = gabi::call_ptr<u32>(current->mpVtable->mGetKey.get(), current);
    expectedKey = gabi::call_ptr<u32>(expectedVtable->mGetKey.get(), &sPictographBoxDisplayState);
    if (currentKey != expectedKey)
        return;
    if ((*held) & 0x6001) {
        hdIngamePictureSave_l* pictures = ingameGetPictureSave(&sSave->mPictures);
        if (pictures->mCount < 12) {
            ingameChangeState(mpMenu->stateMachine(), &sPictographBoxCaptureState);
            return;
        }
    }
    controller = sController;
    u32 buttons = controller->mHold;
    if (buttons & 0x20) {
        ingameSoundId(0x8d3);
        ingameChangeState(mpMenu->stateMachine(), &sPictographBoxListState);
        ingameChangeState(mpOverlay->stateMachine(), &sPictographOverlayListState);
        ingameChangeState(&mState, &l_ingameToPictographListState);
    } else if (controller->mHold & 8) {
        ingameSoundId(0x8d3);
        ingamePictographBoxToggle(mpMenu);
    }
}

/* WWHD glue
data l_ingameToPlayState 1049EBD4 nocheck
data l_ingamePlayState 1049EC00 nocheck
data l_ingamePauseDrcState 1049EC58 nocheck
data l_ingameToMenuState 1049EC84 nocheck
data l_ingameMenuState 1049ECB0 nocheck
data l_ingameMenuSubWindowState 1049ECDC nocheck
data l_ingameToSaveState 1049ED08 nocheck
data l_ingameSaveState 1049ED34 nocheck
data l_ingameToOptionState 1049ED60 nocheck
data l_ingameOptionState 1049ED8C nocheck
data l_ingameToTaktState 1049EDB8 nocheck
data l_ingameTaktState 1049EDE4 nocheck
data l_ingameReturnTaktForProModeState 1049EE68 nocheck
data l_ingameTaktPlayDemoState 1049EE94 nocheck
data l_ingameControlWindState 1049EEC0 nocheck
data l_ingameToWarpState 1049EEEC nocheck
data l_ingameWarpState 1049EF18 nocheck
data l_ingamePhotoViewMiiverseState 1049EF44 nocheck
data l_ingameMiiverseEmpathingState 1049EF70 nocheck
data l_ingameToMiiverseSettingState 1049EF9C nocheck
data l_ingameMiiverseSettingState 1049EFC8 nocheck
data l_ingameToConfirmMiiverseCommentJumpState 1049EFF4 nocheck
data l_ingameConfirmMiiverseCommentJumpState 1049F020 nocheck
data l_ingameToConfirmMiiverseProfileJumpState 1049F04C nocheck
data l_ingameConfirmMiiverseProfileJumpState 1049F078 nocheck
data l_ingameMiiverseProfileJumpState 1049F0A4 nocheck
data l_ingameToMiiverseExplainState 1049F0D0 nocheck
data l_ingameMiiverseExplainState 1049F0FC nocheck
data l_ingameToTelescopeState 1049F128 nocheck
data l_ingameTelescopeState 1049F154 nocheck
data l_ingameToPictographBoxState 1049F180 nocheck
data l_ingameToFishManHintState 1049F2E0 nocheck
data l_ingameInputPasswordState 1049F338 nocheck
data l_ingameDemoTauraArrivalState 1049F364 nocheck
data l_ingameToDemoFishManState 1049F390 nocheck
data l_ingameToDemoTriforceState 1049F3E8 nocheck
data l_ingameGameOverState 1049F46C nocheck
data l_ingameChangeToTvModeState 1049F498 nocheck
data l_ingameChangeToDrcModeState 1049F4C4 nocheck
data l_ingameExchangeProModeState 1049F4F0 nocheck
data l_ingameToTaktSignListState 1049EE10 nocheck
data l_ingameTaktSignListState 1049EE3C nocheck
data l_ingameToPictographListState 1049F1D8 nocheck
data l_ingameToSelectPictographListState 1049F288 nocheck
data l_ingameToPauseDrcState 1049EC2C nocheck
data l_ingamePictographBoxState 1049F1AC nocheck
data sPictographIdleState 10499524 nocheck
data sPictographIdleState 10499524 nocheck
data sMenuButtonsBlankState 104966E4 nocheck
data sMenuButtonsDisplayState 10496714 nocheck
data sWindGuideBlankState 1048EB0C nocheck
data sScreenBlankState 1049DD60 nocheck
data sEventMgr 101F4FF0
data sInputOptions 101F8378
data sController 101F5088
data sMenuStatus 101F8348
data sSaveActive 101F8351
data sTaktRequested 1047B097
data sFramework 1018F4AC
data sSocialData 1018F4AC
data sMessage 101F4B5C
data sSave 101F84DC
data sMiiverse 1018F504
data sProfileJumpDone 1018F511
data sProfileJumpSucceeded 1018F510
data sPictureCapture 1018F3F4
data sSafeStringVtable 1013EEC0 nocheck
data sWarpButtonsOpenState 1049D684 nocheck
data sWarpButtonsDoneState 1049D7A4 nocheck
data sSubWindowPhotoState 10497798 nocheck
data sPictographBoxOpenState 104996F0 nocheck
data sPictographBoxDisplayState 10499720 nocheck
data sPictographBoxCloseState 10499900 nocheck
data sPictographBoxCaptureState 10499750 nocheck
data sPictographBoxListState 10499930 nocheck
data sPictographOverlayListState 10499CE4 nocheck
str 1013F020 "SE_MENU_IN" in 0270E3E8
str 1013F02C "SE_MENU_OUT" in 0270E4E4
str 1013F038 "SE_MENU_OUT" in 0270E844
str 1013F044 "SE_MENU_OUT" in 0270E920
str 1013F050 "SE_MENU_IN" in 0270EA50
str 1013F05C "SE_MENU_IN" in 0270EBA0
str 1013F068 "SE_MENU_IN" in 0270F01C
str 1013F074 "SE_MENU_OUT" in 0270F290
str 1013F080 "SE_MENU_IN" in 0270FB6C
str 1013F08C "SE_MENU_OUT" in 0270FBDC
str 1013F098 "SE_MENU_IN" in 0270FCB4
str 1013F0A4 "SE_MENU_IN" in 0270FDE8
str 1013F0B0 "SE_MENU_IN" in 0270FF6C
str 1013F0BC "SE_MENU_IN" in 02710120
str 1013F0C8 "SE_MENU_IN" in 027103E0
str 1013F018 "GTower"
ext 025200D4 hdIngamePlay_l* ingamePlay()
ext 02443664 u32 ingameGameOverStart(void*)
ext 025986BC u32 ingameIsEventRunning()
ext 025DBE38 u32 ingameOverlapRequested()
ext 0259170C void ingamePause(u32)
ext 026F226C void ingameClearScreen(void*)
ext 02625A80 void ingameCloseNameEntry(void*)
ext 026C09B4 void ingameClosePictograph(void*)
ext 026C04E0 void ingameOpenPictograph(void*,u32)
ext 026A207C void ingameSetSubWindow(void*)
ext 0268DECC void ingameBeginMenuButtons(void*)
ext 0268DEDC void ingameEndMenuButtons(void*)
ext 02622720 void ingameShowWindGuide(void*)
ext 02622730 void ingameHideWindGuide(void*)
ext 026184AC u32 ingameUsesTv(hdIngameController_l*)
ext 02617AE4 u32 ingameUsesPro(hdIngameController_l*)
ext 02618498 u32 ingameUsesDrc(hdIngameController_l*)
ext 020399DC u32 ingameFrameworkBusy(hdIngameFramework_l*)
ext 02030B38 void ingameSound(const char*)
ext 02030CAC u32 ingameSoundPlaying()
ext 02030C34 void ingameSoundBegin()
ext 02030C70 void ingameSoundEnd()
ext 02030CF8 void ingameSoundResume()
ext 02030DC0 void ingameSoundModeEnd()
ext 020063C0 void ingameChangeState(hdStateMachine*,hdIngameStateId_c*)
ext 02006478 hdIngameStateId_c* ingameCurrentState(hdStateMachine*)
ext 020013C0 void ingameReadEvent(void*,void*)
ext 026A35D4 void ingameSubWindowPhoto(void*)
ext 02690724 void ingameResetItemSelection(void*)
ext 026941BC u32 ingameItemScreenSelected(void*)
ext 02689044 u32 ingameMapCloseBlocked(void*)
ext 026917C8 void ingameItemScreenBack(void*)
ext 026DBA08 void ingameBeginSave(void*,u32,u32)
ext 026D83F0 void ingameBeginOption(void*,u32)
ext 024431AC u32 ingameTactNormalWait(void*)
ext 02689028 void ingameMapPrepare(void*)
ext 025B7B10 u32 ingameHasSong(hdIngameCollect_l*,u32)
ext 0257E6EC u32 ingameWindControlReady()
ext 02621C2C void ingameSetWindGuide(void*,u32)
ext 02622750 void ingameOpenTaktSignList(void*)
ext 026227D8 u32 ingameTaktSignListReady(void*)
ext 02622740 void ingameCloseTaktSignList(void*)
ext 02688FF0 void ingameMapRestore(void*)
ext 0268708C u32 ingameMapWindReady(void*)
ext 026F2148 void ingameSelectButton(void*)
ext 026890CC void ingameMapOpenWind(void*)
ext 025E1988 void ingameSoundId(u32)
ext 02030DE8 void ingameSoundWarpBegin()
ext 02030E10 void ingameSoundWarpEnd()
ext 026A620C void ingameSubWindowEmpath(void*)
ext 02035D78 hdIngameFramework_l* ingameGetFramework()
ext 02035ED4 void ingameFrameworkRunning(hdIngameFramework_l*,u32)
ext 026A625C void ingameSubWindowEmpathEnd(void*)
ext 0203BDD0 void ingameMiiversePrepare(hdIngameMiiverse_l*)
ext 0269FF8C u32 ingameSubWindowReceiver(void*)
ext 0203C570 void ingameMiiverseCommentJump(hdIngameMiiverse_l*,hdIngameComment_l*)
ext 026A1748 void ingameSubWindowSetMode(void*,u32)
ext 026A62A8 void ingameSubWindowCancelJump(void*)
ext 0203C61C void ingameMiiverseProfileJump(hdIngameMiiverse_l*)
ext 026DBE64 void ingameBeginExplain(void*)
ext 02030CD0 void ingameSoundPictographBegin()
ext 026C2F2C u32 ingamePictographBoxReady(void*)
ext 025F8E18 void ingameClearMessage(hdIngameMessage_l*)
ext 02720144 hdIngamePictureSave_l* ingameGetPictureSave(hdIngamePictures_l*)
ext 026C2F3C void ingamePictographBoxToggle(void*)
ext 02614FD8 void cking::ui::input::EventMgr::reset()
ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
ext 02615398 u32 ingameHoldBit1(cking::ui::input::EventMgr*)
*/
