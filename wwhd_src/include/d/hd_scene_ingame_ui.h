/** HD: in-game UI scene, coordinating menu and HUD state machines.
 * Private game-derived source. No GameCube counterpart. */
#pragma once
#include "d/hd_screen_menu_main.h"
#include "d/hd_screen_no_operate_drc.h"
#include "d/hd_screen_pause.h"
#include "d/hd_screen_game_over.h"
#include "d/hd_screen_pictograph_camera.h"
#include "d/hd_screen_pictograph_list.h"
#include "d/hd_sequence_option.h"
#include "d/hd_screen_blur_bg.h"
#include "d/hd_parts.h"
#include "d/hd_screen_menu_btns.h"
#include "d/hd_input_eventmgr.h"
#include "hd/hd_render_commands.h"
#include "d/hd_screen_map_display_coordinator.h"
#include "d/hd_sequence_window.h"
#include "d/hd_screen_menu_miiverse.h"
#include "d/hd_screen_baton_sign_list.h"

struct hdIngameStateId_c {
    be<u32> mNumber;
    gptr<const char> mName;
    gptr<hdPartsStateIdVtbl_c> mpVtable;
    hdPartsPtmf_c mEnter, mExecute, mExit;
    union { hdPartsPtmf_c mIsComplete; hdPartsPtmf_c mComplete; };
    u32 getKey() { return gabi::call_ptr<u32>(mpVtable->mGetKey.get(), this); }
};
WWHD_OFFSET(hdIngameStateId_c, mEnter, 0xC);
WWHD_OFFSET(hdIngameStateId_c, mIsComplete, 0x24);
WWHD_SIZE(hdIngameStateId_c, 0x2C);
void hdIngameChangeState(hdStateMachine*, hdIngameStateId_c*);
hdIngameStateId_c* hdIngameCurrentState(hdStateMachine*);

struct hdIngameDestroyVtable {
    u8 mPrefix[0xC];
    gfn mDestroy;
};
WWHD_OFFSET(hdIngameDestroyVtable, mDestroy, 0xC);
WWHD_SIZE(hdIngameDestroyVtable, 0x10);
struct hdIngameDisposer {
    u8 mPrefix[0xC];
    gptr<hdIngameDestroyVtable> mpVtable;
};
WWHD_SIZE(hdIngameDisposer, 0x10);
struct hdIngamePointerVector {
    gptr<hdIngameDestroyVtable> mpVtable;
    gptr<void> mpElements;
    be<u16> mCount, mCapacity;
    be<u32> mReserved;
};
WWHD_SIZE(hdIngamePointerVector, 0x10);
struct hdIngameScreenGroup {
    hdIngameDisposer mDisposer;
    hdUiSafeString mArchiveName;
    be<u32> mCount, mCapacity;
    hdIngamePointerVector mChildren;
    u8 mReserved[0x10];
};
WWHD_OFFSET(hdIngameScreenGroup, mArchiveName, 0x10);
WWHD_OFFSET(hdIngameScreenGroup, mChildren, 0x20);
WWHD_SIZE(hdIngameScreenGroup, 0x40);


struct hdIngameUi_c {
    // Companion handlers share this exact scene layout.
    void setTvScreens(u32 focus);
    void setDrcScreens(u32 focus);
    void toPlayEnter();
    void toPlayExecute();
    void playEnter();
    u32 isGameOver();
    u32 isPasswordRequested();
    u32 isFishManDemoRequested();
    u32 menuControlsBlocked();
    u32 isDrcMode();
    u32 handleModeChange();
    void playExecute();
    void toPauseDrcEnter();
    void toPauseDrcExecute();
    void pauseDrcEnter();
    void clearGTowerItems();
    u32 isTauraArrivalRequested();
    u32 isTriforceDemoRequested();
    void pauseDrcExecute();
    void pauseDrcExit();
    void prepareMapCoordinator();
    void toMenuEnter();
    void toMenuExecute();
    u32 canCloseItemScreen();
    void menuExecute();
    void menuSubWindowEnter();
    void completeHint();
    void menuSubWindowExecute();
    void toSaveEnter();
    void toSaveExecute();
    void saveExecute();
    void saveExit();
    void toOptionEnter();
    void toOptionExecute();
    void optionExecute();
    void toTaktEnter();
    void toTaktExecute();
    void taktEnter();
    void taktExecute();
    void toTaktSignListEnter();
    void toTaktSignListExecute();
    void taktSignListExecute();
    void returnTaktForProModeExecute();
    void refreshMenuSelection();
    void taktPlayDemoEnter();
    void taktPlayDemoExecute();
    void controlWindEnter();
    void controlWindExecute();
    void controlWindExit();
    void toWarpEnter();
    void toWarpExecute();
    void warpEnter();
    void warpExecute();
    void warpExit();
    void photoViewMiiverseEnter();
    void photoViewMiiverseExecute();
    void miiverseEmpathingEnter();
    void miiverseEmpathingExecute();
    void miiverseEmpathingExit();
    void toMiiverseSettingEnter();
    void toMiiverseSettingExecute();
    void miiverseSettingExecute();
    void miiverseSettingExit();
    void toConfirmMiiverseCommentJumpEnter();
    void toConfirmMiiverseCommentJumpExecute();
    void confirmMiiverseCommentJumpEnter();
    void confirmMiiverseCommentJumpExecute();
    void toConfirmMiiverseProfileJumpEnter();
    void toConfirmMiiverseProfileJumpExecute();
    void confirmMiiverseProfileJumpEnter();
    void confirmMiiverseProfileJumpExecute();
    void miiverseProfileJumpEnter();
    void miiverseProfileJumpExecute();
    void toMiiverseExplainEnter();
    void toMiiverseExplainExecute();
    void miiverseExplainExecute();
    void miiverseExplainExit();
    void toTelescopeEnter();
    void toTelescopeExecute();
    void telescopeExecute();
    void toPictographBoxEnter();
    void toPictographBoxExecute();
    void pictographBoxExecute();


    void calc();
    void calcSecondaryScreens();
    void finalizeScreens();
    void beginScreenUpdate();
    void resetMenuScreens();
    void createScreens();
    void initializeScreens();
    void drawModeSwitchScreens(hdLayerRenderInfo_c*);
    void drawMenuScreens(void*);
    void drawOverlayScreens(hdLayerRenderInfo_c*);
    void drawPrimaryScreens(hdLayerRenderInfo_c*);
    void drawHudScreens(hdLayerRenderInfo_c*);
    void drawInputScreens(hdLayerRenderInfo_c*);
    void connectSubWindow();
    void refreshScreenReceivers();
    void changeActiveScreen();
    void handleMenuActions();
    void updateMenuButtons();
    void handleScreenActions();
    void handleItemAssignment();
    void assignItemSlot(u32, u32);
    void handleInvalidControls();
    void updatePictographList();
    void returnFromPictographList();
    void waitForPictographReturn();
    void beginPictographList();
    void beginFishManHint();
    void updateFishManHint();
    void waitForMenuExit();
    void requestMenuIdle();
    void resumeFramework();
    void waitForMenuEntry();
    void finishMenuExit();
    void resumeAfterModeSwitch();
    void connectItemScreenOpen();
    void connectItemScreenClosed();
    void connectMapCoordinatorOpen();
    void connectMapCoordinatorClosed();
    void beginMenuExit();
    void updateMenuExit();
    void endItemScreen();
    void beginTvModeSwitch();
    void beginDrcModeSwitch();
    void beginProModeSwitch();
    void updateModeSwitchInput();
    void saveInputReceiver();
    void restoreInputReceiver();
    void closePictographList();
    void beginPictographTransition();
    void endMapCoordinator();
    void endPictographBox();
    void resumeSceneFramework();
    void clearTouchPanel();
    void captureModeSwitchButtons();
    void requestButtonRefresh(u32 index);
    u32 requestGameOver();
    u8 isGameOverFinished();
    u8 isCameraShutterVisible();
    void clearPictographTexture(s32 index);
    static u8 isTaktSignListActive();
    static u8 isPictographListActive();
    static void enableDirectionalInput();
    static u8 isMenuMode();
    static u32 isUiModeActive();
    static u8 isModalUiActive();
    gptr<hdStateOwnerVTable> mpOwnerVtable;
    gptr<hdPartsStateIdVtbl_c> mpStateVtable;
    gptr<hdIngameUi_c> mpSelf;
    be<u32> mReservedBase;
    hdStateMachine mState;
    gptr<hdIngameDestroyVtable> mpVtable;
    hdIngameDisposer mDisposer;
    hdIngameScreenGroup mScreenGroups[5];
    union {
        struct {
    gptr<hdParts_c> mpMainHud;
    gptr<hdMenuMain_c> mpItemScreen;
    gptr<hdMenuMiiverse_c> mpSubWindow;
    gptr<hdMenuButtons> mpMenuButtons;
    gptr<hdParts_c> mpStatusScreen;
    gptr<hdParts_c> mpMenuEffects;
    gptr<hdGameOver_c> mpGameOver;
    gptr<hdParts_c> mpHudIndicator;
    gptr<hdParts_c> mpBatonSign;
    gptr<hdParts_c> mpBatonEffects;
    gptr<void> mpMenuMessageScreen;
    u8 mReservedHint[0x1B8 - 0x1A4];
    gptr<void> mpMessageChoiceCursorScreen;
    union { gptr<hdSequenceWindow_c> mpHintScreen; gptr<hdSequenceWindow_c> mpSaveScreen; };
    union { gptr<hdSequenceOption_c> mpHintEffects; gptr<hdSequenceOption_c> mpOptionScreen; };
    gptr<hdParts_c> mpControlWindHint;
    gptr<hdParts_c> mpItemControlHints;
    gptr<hdParts_c> mpMenuConfirmation;
    gptr<hdParts_c> mpMiiversePrompt;
    gptr<hdPictographCamera> mpMenu;      /* the Picto Box camera screen */
    gptr<hdPictographList> mpOverlay;     /* the Picto Box picture list */
    gptr<hdParts_c> mpMetronome;
    gptr<hdBatonSignList_c> mpWindGuide;
    gptr<hdParts_c> mpModeSwitchScreen;
    gptr<hdParts_c> mpModeSwitchEffects;
    gptr<hdPause_c> mpPictographScreen;
    gptr<hdBlurBackground_c> mpNameEntryScreen;
    u8 mReservedTouch[0x210 - 0x1F4];
    gptr<hdMenuButtons> mpWarpButtons;
    gptr<hdNoOperateDRC_c> mpTouchPanel;
    gptr<hdMapDisplayCoordinator_c> mpMapCoordinator;
        };
        gptr<void> mOwnedScreens[42];
        struct { u8 mScreenPointerPrefix[41 * 4]; be<u32> mCountdown; };
    };
    be<u8> mPauseStartedDuringDemo;
    u8 mPaddingInitialized[3];
    be<u32> mMenuSelection;
    be<u32> mTransitionFrames, mTransitionPhase, mPausePhase;
    be<u32> mModeSwitchPhase, mModeSwitchFrames;
    be<u8> mModeSwitchButtonsHeld;
    be<u8> mMenuRequested, mTouchPanelRequested, mUiLocked;
    union { be<u8> mRefreshableButtons; be<u8> mAvailableSongs; };
    union { be<u8> mPendingRefreshButtons; be<u8> mSelectedSong; };
    union { be<u8> mModeExchangeRequested; be<u8> mTaktNormalWait; };
    union { be<u8> mTransitionComplete; be<u8> mMiiverseExplainRequested; };
    be<u32> mModeExchangePhase;
    be<u8> mReturnMenuRequested;
    u8 mTailPadding[3];
};
WWHD_OFFSET(hdIngameUi_c, mState, 0x10);
WWHD_OFFSET(hdIngameUi_c, mDisposer, 0x28);
WWHD_OFFSET(hdIngameUi_c, mScreenGroups, 0x38);
WWHD_OFFSET(hdIngameUi_c, mpGameOver, 0x190);
WWHD_OFFSET(hdIngameUi_c, mpMenu, 0x1D4);
WWHD_OFFSET(hdIngameUi_c, mpOverlay, 0x1D8);
WWHD_OFFSET(hdIngameUi_c, mpTouchPanel, 0x214);
WWHD_OFFSET(hdIngameUi_c, mModeSwitchButtonsHeld, 0x23C);
WWHD_OFFSET(hdIngameUi_c, mRefreshableButtons, 0x240);
WWHD_SIZE(hdIngameUi_c, 0x24C);
WWHD_OFFSET(hdIngameUi_c, mOwnedScreens, 0x178);

// Scene state bindings shared with the companion unit.
extern hdIngameStateId_c& l_ingamePictographListState;
extern hdIngameStateId_c& l_ingamePictographBoxState;
extern hdIngameStateId_c& l_ingameSelectPictographListState;
extern hdIngameStateId_c& l_ingameToPauseDrcState;
extern hdIngameStateId_c& l_ingameToTaktSignListState;
extern hdIngameStateId_c& l_ingameTaktSignListState;
extern hdIngameStateId_c& l_ingameToPictographListState;
extern hdIngameStateId_c& l_ingameToSelectPictographListState;
extern hdIngameStateId_c& l_ingameToPlayState;
extern hdIngameStateId_c& l_ingamePlayState;
extern hdIngameStateId_c& l_ingamePauseDrcState;
extern hdIngameStateId_c& l_ingameToMenuState;
extern hdIngameStateId_c& l_ingameMenuState;
extern hdIngameStateId_c& l_ingameMenuSubWindowState;
extern hdIngameStateId_c& l_ingameToSaveState;
extern hdIngameStateId_c& l_ingameSaveState;
extern hdIngameStateId_c& l_ingameToOptionState;
extern hdIngameStateId_c& l_ingameOptionState;
extern hdIngameStateId_c& l_ingameToTaktState;
extern hdIngameStateId_c& l_ingameTaktState;
extern hdIngameStateId_c& l_ingameReturnTaktForProModeState;
extern hdIngameStateId_c& l_ingameTaktPlayDemoState;
extern hdIngameStateId_c& l_ingameControlWindState;
extern hdIngameStateId_c& l_ingameToWarpState;
extern hdIngameStateId_c& l_ingameWarpState;
extern hdIngameStateId_c& l_ingamePhotoViewMiiverseState;
extern hdIngameStateId_c& l_ingameMiiverseEmpathingState;
extern hdIngameStateId_c& l_ingameToMiiverseSettingState;
extern hdIngameStateId_c& l_ingameMiiverseSettingState;
extern hdIngameStateId_c& l_ingameToConfirmMiiverseCommentJumpState;
extern hdIngameStateId_c& l_ingameConfirmMiiverseCommentJumpState;
extern hdIngameStateId_c& l_ingameToConfirmMiiverseProfileJumpState;
extern hdIngameStateId_c& l_ingameConfirmMiiverseProfileJumpState;
extern hdIngameStateId_c& l_ingameMiiverseProfileJumpState;
extern hdIngameStateId_c& l_ingameToMiiverseExplainState;
extern hdIngameStateId_c& l_ingameMiiverseExplainState;
extern hdIngameStateId_c& l_ingameToTelescopeState;
extern hdIngameStateId_c& l_ingameTelescopeState;
extern hdIngameStateId_c& l_ingameToPictographBoxState;
extern hdIngameStateId_c& l_ingameReturnPictographBoxState;
extern hdIngameStateId_c& l_ingameExitPictographListState;
extern hdIngameStateId_c& l_ingameToFishManHintState;
extern hdIngameStateId_c& l_ingameFishManHintState;
extern hdIngameStateId_c& l_ingameInputPasswordState;
extern hdIngameStateId_c& l_ingameDemoTauraArrivalState;
extern hdIngameStateId_c& l_ingameToDemoFishManState;
extern hdIngameStateId_c& l_ingameDemoFishManState;
extern hdIngameStateId_c& l_ingameToDemoTriforceState;
extern hdIngameStateId_c& l_ingameDemoTriforceState;
extern hdIngameStateId_c& l_ingameDemoPictBottleState;
extern hdIngameStateId_c& l_ingameGameOverState;
extern hdIngameStateId_c& l_ingameChangeToTvModeState;
extern hdIngameStateId_c& l_ingameChangeToDrcModeState;
extern hdIngameStateId_c& l_ingameExchangeProModeState;
extern hdIngameStateId_c& l_ingameMapCoordinatorClosedState;

WWHD_OFFSET(hdIngameUi_c, mpSaveScreen, 0x1BC);
WWHD_OFFSET(hdIngameUi_c, mpOptionScreen, 0x1C0);
WWHD_OFFSET(hdIngameUi_c, mpControlWindHint, 0x1C4);
WWHD_OFFSET(hdIngameUi_c, mpWindGuide, 0x1E0);
WWHD_OFFSET(hdIngameUi_c, mCountdown, 0x21C);
WWHD_OFFSET(hdIngameUi_c, mPauseStartedDuringDemo, 0x220);
WWHD_OFFSET(hdIngameUi_c, mMenuSelection, 0x224);
WWHD_OFFSET(hdIngameUi_c, mTransitionFrames, 0x228);
WWHD_OFFSET(hdIngameUi_c, mTransitionPhase, 0x22C);
WWHD_OFFSET(hdIngameUi_c, mPausePhase, 0x230);
WWHD_OFFSET(hdIngameUi_c, mAvailableSongs, 0x240);
WWHD_OFFSET(hdIngameUi_c, mSelectedSong, 0x241);
WWHD_OFFSET(hdIngameUi_c, mTaktNormalWait, 0x242);
WWHD_OFFSET(hdIngameUi_c, mMiiverseExplainRequested, 0x243);
