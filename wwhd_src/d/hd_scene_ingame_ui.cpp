/** HD: in-game UI scene state handlers. Private game-derived source. */
#include "d/hd_scene_ingame_ui.h"
#include "d/hd_input_ctrl.h"
#include "d/d_com_inf_game.h"
#include "d/d_mesg_core.h"
#include "hd/hd_ui_02001084.h"
#include "d/actor/d_a_player.h"
#include "d/hd_cursor_mgr.h"
#include "d/hd_screen_msg_cursor.h"
#include "d/hd_screen_boat_race_start.h"
#include "d/hd_screen_boat_race_failed.h"
#include "d/hd_screen_boat_race_result_parts.h"
#include "d/hd_sequence_window.h"
#include "d/hd_screen_auction_rupy.h"
#include "d/d_operate_wind.h"
#include "d/d_metronome.h"
#include "d/d_menu_window.h"
#include "d/hd_swkbd_mgr.h"
#include "d/hd_screen_menu_miiverse.h"

static hdParts_c* sSharedMenuScreen;
struct hdIngameSubWindowVtable_l {
    u8 mPrefix[0xFC];
    gfn mDrawOverlay;
};
WWHD_OFFSET(hdIngameSubWindowVtable_l, mDrawOverlay, 0xFC);
WWHD_SIZE(hdIngameSubWindowVtable_l, 0x100);
void hdIngameDrawMenuMessage(void*, void*);
void hdIngameDrawMessageChoiceCursor(void*, void*);
static hdIngameStateId_c l_ingamePictographListState;
static hdPartsStateId_c sMenuIdleState;
static hdPartsStateId_c sMenuEntryState;
static hdIngameStateId_c l_ingamePictographBoxState;
static hdIngameStateId_c l_ingameSelectPictographListState;
static cking::ui::input::Options* sInputOptions;
static u32 sUiMode;

struct hdIngameFramework_l;
hdIngameFramework_l* hdIngameGetFramework();
void hdIngameSetFrameworkRunning(hdIngameFramework_l*, u32);
void dMenu_setMenuStatus(u32);

static cking::ui::input::EventMgr* sInputEventManager;
void hdIngamePrepareMapCoordinator(hdIngameUi_c*);
void hdIngameFinishPictograph(void*);
void hdIngameFrameworkResume();

static hdPartsStateId_c sMenuClosingState;
static hdPartsStateId_c sOverlayClosingState;
static hdIngameStateId_c l_ingameToPauseDrcState;
void hdIngamePlayMenuExitSound(u32);
void hdIngamePauseScene();
void hdIngameCloseItemScreen(void*);
void hdIngameBeginModeSwitchScreen(void*);
void hdIngameSetTvScreens(hdIngameUi_c*, u32);
void hdIngameSetDrcScreens(hdIngameUi_c*, u32);
void hdIngameSetModeSwitchRendering(u32);
void hdIngameSetModeSwitchInput(u32);

static dComIfG_inf_c* gameInfo() {
    return gabi::at<dComIfG_inf_c>(gabi::ea(dComIfGp_get()));
}
static daPy_py_c* player(dComIfG_inf_c* game) {
    return reinterpret_cast<daPy_py_c*>(game->play.mPlayerInfo[0].mpPlayer.get());
}

void hdIngameSetItemScreenReceivers(void*, u32, u32, u32);
void hdIngameConnectItemScreenOpen(void*);
void hdIngameConnectItemScreenClosed(void*);

static hdIngameUi_c* sIngameUi;
static hdIngameDisposer* sIngameDisposer;
static hdIngameDestroyVtable sIngameDisposerVtable;
static hdIngameUi_c* constructIngameUi(hdIngameUi_c*);
void* hdIngameAllocate(u32, void*, u32);
void hdIngameConstructDisposer(hdIngameDisposer*, void*, u32);
void hdIngameDestroyDisposerBase(hdIngameDisposer*, u32);
void hdIngameDestroyScreenGroup(hdIngameScreenGroup*, u32);

static hdIngameStateId_c l_ingameToTaktSignListState;
static hdIngameStateId_c l_ingameTaktSignListState;
static hdIngameStateId_c l_ingameToPictographListState;
static hdIngameStateId_c l_ingameToSelectPictographListState;

// The expected vtable is captured before the current-key call, then its method is loaded afterward.
static bool sceneStateMatches(hdStateMachine* machine, hdIngameStateId_c* expected) {
    hdIngameStateId_c* current = hdIngameCurrentState(machine);
    u32 currentMethod = current->mpVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedVtable = expected->mpVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentMethod, current);
    u32 expectedKey = gabi::call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
    return currentKey == expectedKey;
}

static hdRenderSceneMgr_l* sRenderManager;
void hdIngameSetShaderModeEx(u32, u32, u32, u32, u32, u32, u32);
void hdIngameUpdateState(hdStateMachine*);
static hdParts_c* screenParts(void* screen) {
    return static_cast<hdParts_c*>(screen);
}

struct hdIngameHeaderStatics {
    be<f32> mMinAngle, mMaxAngle, mNearFirst, mNearSecond, mFarFirst, mFarSecond;
    be<u8> mFirstFlag, mSecondFlag;
    u8 mPadding[2];
    be<u32> mZeros[4];
};
WWHD_SIZE(hdIngameHeaderStatics, 0x2C);
struct hdIngameGlobalRegistration { be<u32> mWords[3]; };
WWHD_SIZE(hdIngameGlobalRegistration, 0xC);
static hdIngameHeaderStatics sHeaderStatics;
static hdIngameGlobalRegistration sHeaderRegistrationFirst;
static hdIngameGlobalRegistration sHeaderRegistrationSecond;
static hdIngameGlobalRegistration sHeaderRegistrationThird;
static hdPartsStateIdVtbl_c sSceneStateVtable;
static u32 sSceneStateGuard;
static u32 sSceneStateCounter;
static f32 sInitMinAngle;
static f32 sInitMaxAngle;
static f32 sInitNear;
static f32 sInitFar;
void hdIngameRegisterGlobal(hdIngameGlobalRegistration*);
void hdIngameConstructFirstFlag(be<u8>*);
void hdIngameConstructSecondFlag(be<u8>*);
using hdIngameStateCallback = void (*)(hdIngameUi_c*);
using hdIngameCompletionCallback = BOOL (*)(hdIngameUi_c*);
// Completion address adapters match the canonical hdMainScene_c predicates.
// Free function bindings let guest_fn store their original GHS callback addresses.

template <class Callback>
static void initializeStateDelegate(hdPartsPtmf_c* delegate, Callback callback) {
    delegate->mThisDelta = 0;
    delegate->mVirtualIndex = -1;
    delegate->mFunction = gabi::guest_fn((const void*)callback);
}
static void initializeSceneState(hdIngameStateId_c* state, u32 number, const char* name,
                                 hdIngameStateCallback enter, hdIngameStateCallback execute,
                                 hdIngameStateCallback exit, hdIngameCompletionCallback completion) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mpVtable = &sSceneStateVtable;
    initializeStateDelegate(&state->mEnter, enter);
    initializeStateDelegate(&state->mExecute, execute);
    initializeStateDelegate(&state->mExit, exit);
    initializeStateDelegate(&state->mIsComplete, completion);
}

static hdIngameStateId_c l_ingameToPlayState;
void ingameToPlayEnter(hdIngameUi_c*);
void ingameToPlayExecute(hdIngameUi_c*);
void ingameToPlayExit(hdIngameUi_c*);
BOOL ingameToPlayComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingamePlayState;
void ingamePlayEnter(hdIngameUi_c*);
void ingamePlayExecute(hdIngameUi_c*);
void ingamePlayExit(hdIngameUi_c*);
BOOL ingamePlayComplete(hdIngameUi_c*);
void ingameToPauseDrcEnter(hdIngameUi_c*);
void ingameToPauseDrcExecute(hdIngameUi_c*);
void ingameToPauseDrcExit(hdIngameUi_c*);
BOOL ingameToPauseDrcComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingamePauseDrcState;
void ingamePauseDrcEnter(hdIngameUi_c*);
void ingamePauseDrcExecute(hdIngameUi_c*);
void ingamePauseDrcExit(hdIngameUi_c*);
BOOL ingamePauseDrcComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToMenuState;
void ingameToMenuEnter(hdIngameUi_c*);
void ingameToMenuExecute(hdIngameUi_c*);
void ingameToMenuExit(hdIngameUi_c*);
BOOL ingameToMenuComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameMenuState;
void ingameMenuEnter(hdIngameUi_c*);
void ingameMenuExecute(hdIngameUi_c*);
void ingameMenuExit(hdIngameUi_c*);
BOOL ingameMenuComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameMenuSubWindowState;
void ingameMenuSubWindowEnter(hdIngameUi_c*);
void ingameMenuSubWindowExecute(hdIngameUi_c*);
void ingameMenuSubWindowExit(hdIngameUi_c*);
BOOL ingameMenuSubWindowComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToSaveState;
void ingameToSaveEnter(hdIngameUi_c*);
void ingameToSaveExecute(hdIngameUi_c*);
void ingameToSaveExit(hdIngameUi_c*);
BOOL ingameToSaveComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameSaveState;
void ingameSaveEnter(hdIngameUi_c*);
void ingameSaveExecute(hdIngameUi_c*);
void ingameSaveExit(hdIngameUi_c*);
BOOL ingameSaveComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToOptionState;
void ingameToOptionEnter(hdIngameUi_c*);
void ingameToOptionExecute(hdIngameUi_c*);
void ingameToOptionExit(hdIngameUi_c*);
BOOL ingameToOptionComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameOptionState;
void ingameOptionEnter(hdIngameUi_c*);
void ingameOptionExecute(hdIngameUi_c*);
void ingameOptionExit(hdIngameUi_c*);
BOOL ingameOptionComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToTaktState;
void ingameToTaktEnter(hdIngameUi_c*);
void ingameToTaktExecute(hdIngameUi_c*);
void ingameToTaktExit(hdIngameUi_c*);
BOOL ingameToTaktComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameTaktState;
void ingameTaktEnter(hdIngameUi_c*);
void ingameTaktExecute(hdIngameUi_c*);
void ingameTaktExit(hdIngameUi_c*);
BOOL ingameTaktComplete(hdIngameUi_c*);
void ingameToTaktSignListEnter(hdIngameUi_c*);
void ingameToTaktSignListExecute(hdIngameUi_c*);
void ingameToTaktSignListExit(hdIngameUi_c*);
BOOL ingameToTaktSignListComplete(hdIngameUi_c*);
void ingameTaktSignListEnter(hdIngameUi_c*);
void ingameTaktSignListExecute(hdIngameUi_c*);
void ingameTaktSignListExit(hdIngameUi_c*);
BOOL ingameTaktSignListComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameReturnTaktForProModeState;
void ingameReturnTaktForProModeEnter(hdIngameUi_c*);
void ingameReturnTaktForProModeExecute(hdIngameUi_c*);
void ingameReturnTaktForProModeExit(hdIngameUi_c*);
BOOL ingameReturnTaktForProModeComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameTaktPlayDemoState;
void ingameTaktPlayDemoEnter(hdIngameUi_c*);
void ingameTaktPlayDemoExecute(hdIngameUi_c*);
void ingameTaktPlayDemoExit(hdIngameUi_c*);
BOOL ingameTaktPlayDemoComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameControlWindState;
void ingameControlWindEnter(hdIngameUi_c*);
void ingameControlWindExecute(hdIngameUi_c*);
void ingameControlWindExit(hdIngameUi_c*);
BOOL ingameControlWindComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToWarpState;
void ingameToWarpEnter(hdIngameUi_c*);
void ingameToWarpExecute(hdIngameUi_c*);
void ingameToWarpExit(hdIngameUi_c*);
BOOL ingameToWarpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameWarpState;
void ingameWarpEnter(hdIngameUi_c*);
void ingameWarpExecute(hdIngameUi_c*);
void ingameWarpExit(hdIngameUi_c*);
BOOL ingameWarpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingamePhotoViewMiiverseState;
void ingamePhotoViewMiiverseEnter(hdIngameUi_c*);
void ingamePhotoViewMiiverseExecute(hdIngameUi_c*);
void ingamePhotoViewMiiverseExit(hdIngameUi_c*);
BOOL ingamePhotoViewMiiverseComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameMiiverseEmpathingState;
void ingameMiiverseEmpathingEnter(hdIngameUi_c*);
void ingameMiiverseEmpathingExecute(hdIngameUi_c*);
void ingameMiiverseEmpathingExit(hdIngameUi_c*);
BOOL ingameMiiverseEmpathingComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToMiiverseSettingState;
void ingameToMiiverseSettingEnter(hdIngameUi_c*);
void ingameToMiiverseSettingExecute(hdIngameUi_c*);
void ingameToMiiverseSettingExit(hdIngameUi_c*);
BOOL ingameToMiiverseSettingComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameMiiverseSettingState;
void ingameMiiverseSettingEnter(hdIngameUi_c*);
void ingameMiiverseSettingExecute(hdIngameUi_c*);
void ingameMiiverseSettingExit(hdIngameUi_c*);
BOOL ingameMiiverseSettingComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToConfirmMiiverseCommentJumpState;
void ingameToConfirmMiiverseCommentJumpEnter(hdIngameUi_c*);
void ingameToConfirmMiiverseCommentJumpExecute(hdIngameUi_c*);
void ingameToConfirmMiiverseCommentJumpExit(hdIngameUi_c*);
BOOL ingameToConfirmMiiverseCommentJumpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameConfirmMiiverseCommentJumpState;
void ingameConfirmMiiverseCommentJumpEnter(hdIngameUi_c*);
void ingameConfirmMiiverseCommentJumpExecute(hdIngameUi_c*);
void ingameConfirmMiiverseCommentJumpExit(hdIngameUi_c*);
BOOL ingameConfirmMiiverseCommentJumpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToConfirmMiiverseProfileJumpState;
void ingameToConfirmMiiverseProfileJumpEnter(hdIngameUi_c*);
void ingameToConfirmMiiverseProfileJumpExecute(hdIngameUi_c*);
void ingameToConfirmMiiverseProfileJumpExit(hdIngameUi_c*);
BOOL ingameToConfirmMiiverseProfileJumpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameConfirmMiiverseProfileJumpState;
void ingameConfirmMiiverseProfileJumpEnter(hdIngameUi_c*);
void ingameConfirmMiiverseProfileJumpExecute(hdIngameUi_c*);
void ingameConfirmMiiverseProfileJumpExit(hdIngameUi_c*);
BOOL ingameConfirmMiiverseProfileJumpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameMiiverseProfileJumpState;
void ingameMiiverseProfileJumpEnter(hdIngameUi_c*);
void ingameMiiverseProfileJumpExecute(hdIngameUi_c*);
void ingameMiiverseProfileJumpExit(hdIngameUi_c*);
BOOL ingameMiiverseProfileJumpComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToMiiverseExplainState;
void ingameToMiiverseExplainEnter(hdIngameUi_c*);
void ingameToMiiverseExplainExecute(hdIngameUi_c*);
void ingameToMiiverseExplainExit(hdIngameUi_c*);
BOOL ingameToMiiverseExplainComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameMiiverseExplainState;
void ingameMiiverseExplainEnter(hdIngameUi_c*);
void ingameMiiverseExplainExecute(hdIngameUi_c*);
void ingameMiiverseExplainExit(hdIngameUi_c*);
BOOL ingameMiiverseExplainComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToTelescopeState;
void ingameToTelescopeEnter(hdIngameUi_c*);
void ingameToTelescopeExecute(hdIngameUi_c*);
void ingameToTelescopeExit(hdIngameUi_c*);
BOOL ingameToTelescopeComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameTelescopeState;
void ingameTelescopeEnter(hdIngameUi_c*);
void ingameTelescopeExecute(hdIngameUi_c*);
void ingameTelescopeExit(hdIngameUi_c*);
BOOL ingameTelescopeComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToPictographBoxState;
void ingameToPictographBoxEnter(hdIngameUi_c*);
void ingameToPictographBoxExecute(hdIngameUi_c*);
void ingameToPictographBoxExit(hdIngameUi_c*);
BOOL ingameToPictographBoxComplete(hdIngameUi_c*);
void ingamePictographBoxEnter(hdIngameUi_c*);
void ingamePictographBoxExecute(hdIngameUi_c*);
void ingamePictographBoxExit(hdIngameUi_c*);
BOOL ingamePictographBoxComplete(hdIngameUi_c*);
void ingameToPictographListEnter(hdIngameUi_c*);
void ingameToPictographListExecute(hdIngameUi_c*);
void ingameToPictographListExit(hdIngameUi_c*);
BOOL ingameToPictographListComplete(hdIngameUi_c*);
void ingamePictographListEnter(hdIngameUi_c*);
void ingamePictographListExecute(hdIngameUi_c*);
void ingamePictographListExit(hdIngameUi_c*);
BOOL ingamePictographListComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameReturnPictographBoxState;
void ingameReturnPictographBoxEnter(hdIngameUi_c*);
void ingameReturnPictographBoxExecute(hdIngameUi_c*);
void ingameReturnPictographBoxExit(hdIngameUi_c*);
BOOL ingameReturnPictographBoxComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameExitPictographListState;
void ingameExitPictographListEnter(hdIngameUi_c*);
void ingameExitPictographListExecute(hdIngameUi_c*);
void ingameExitPictographListExit(hdIngameUi_c*);
BOOL ingameExitPictographListComplete(hdIngameUi_c*);
void ingameToSelectPictographListEnter(hdIngameUi_c*);
void ingameToSelectPictographListExecute(hdIngameUi_c*);
void ingameToSelectPictographListExit(hdIngameUi_c*);
BOOL ingameToSelectPictographListComplete(hdIngameUi_c*);
void ingameSelectPictographListEnter(hdIngameUi_c*);
void ingameSelectPictographListExecute(hdIngameUi_c*);
void ingameSelectPictographListExit(hdIngameUi_c*);
BOOL ingameSelectPictographListComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToFishManHintState;
void ingameToFishManHintEnter(hdIngameUi_c*);
void ingameToFishManHintExecute(hdIngameUi_c*);
void ingameToFishManHintExit(hdIngameUi_c*);
BOOL ingameToFishManHintComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameFishManHintState;
void ingameFishManHintEnter(hdIngameUi_c*);
void ingameFishManHintExecute(hdIngameUi_c*);
void ingameFishManHintExit(hdIngameUi_c*);
BOOL ingameFishManHintComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameInputPasswordState;
void ingameInputPasswordEnter(hdIngameUi_c*);
void ingameInputPasswordExecute(hdIngameUi_c*);
void ingameInputPasswordExit(hdIngameUi_c*);
BOOL ingameInputPasswordComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameDemoTauraArrivalState;
void ingameDemoTauraArrivalEnter(hdIngameUi_c*);
void ingameDemoTauraArrivalExecute(hdIngameUi_c*);
void ingameDemoTauraArrivalExit(hdIngameUi_c*);
BOOL ingameDemoTauraArrivalComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToDemoFishManState;
void ingameToDemoFishManEnter(hdIngameUi_c*);
void ingameToDemoFishManExecute(hdIngameUi_c*);
void ingameToDemoFishManExit(hdIngameUi_c*);
BOOL ingameToDemoFishManComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameDemoFishManState;
void ingameDemoFishManEnter(hdIngameUi_c*);
void ingameDemoFishManExecute(hdIngameUi_c*);
void ingameDemoFishManExit(hdIngameUi_c*);
BOOL ingameDemoFishManComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameToDemoTriforceState;
void ingameToDemoTriforceEnter(hdIngameUi_c*);
void ingameToDemoTriforceExecute(hdIngameUi_c*);
void ingameToDemoTriforceExit(hdIngameUi_c*);
BOOL ingameToDemoTriforceComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameDemoTriforceState;
void ingameDemoTriforceEnter(hdIngameUi_c*);
void ingameDemoTriforceExecute(hdIngameUi_c*);
void ingameDemoTriforceExit(hdIngameUi_c*);
BOOL ingameDemoTriforceComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameDemoPictBottleState;
void ingameDemoPictBottleEnter(hdIngameUi_c*);
void ingameDemoPictBottleExecute(hdIngameUi_c*);
void ingameDemoPictBottleExit(hdIngameUi_c*);
BOOL ingameDemoPictBottleComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameGameOverState;
void ingameGameOverEnter(hdIngameUi_c*);
void ingameGameOverExecute(hdIngameUi_c*);
void ingameGameOverExit(hdIngameUi_c*);
BOOL ingameGameOverComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameChangeToTvModeState;
void ingameChangeToTvModeEnter(hdIngameUi_c*);
void ingameChangeToTvModeExecute(hdIngameUi_c*);
void ingameChangeToTvModeExit(hdIngameUi_c*);
BOOL ingameChangeToTvModeComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameChangeToDrcModeState;
void ingameChangeToDrcModeEnter(hdIngameUi_c*);
void ingameChangeToDrcModeExecute(hdIngameUi_c*);
void ingameChangeToDrcModeExit(hdIngameUi_c*);
BOOL ingameChangeToDrcModeComplete(hdIngameUi_c*);
static hdIngameStateId_c l_ingameExchangeProModeState;
void ingameExchangeProModeEnter(hdIngameUi_c*);
void ingameExchangeProModeExecute(hdIngameUi_c*);
void ingameExchangeProModeExit(hdIngameUi_c*);
BOOL ingameExchangeProModeComplete(hdIngameUi_c*);

static hdStateOwnerVTable sSceneOwnerVtable;
static hdPartsStateIdVtbl_c sSceneInterfaceVtable;
static hdIngameDestroyVtable sSceneVtable;
static hdIngameDestroyVtable sBaseSceneVtable;
static hdIngameDestroyVtable sGroupDisposerVtable;
static hdIngameDestroyVtable sPointerVectorBaseVtable;
static hdIngameDestroyVtable sPointerVectorVtable;
static hdUiStringTable sGroupStringVtable;
static hdStateId sNullSceneState;
void hdIngameConstructState(hdStateMachine*, hdIngameUi_c*, hdStateId*);
void hdIngameConstructGroupDisposer(hdIngameDisposer*);

static hdUiSafeString* constructGroupArchiveName(hdUiSafeString* name) {
    if (name == NULL) {
        name = static_cast<hdUiSafeString*>(operator_new(sizeof(hdUiSafeString)));
        if (name == NULL) return NULL;
    }
    name->mText.v = gabi::guest_addr("");
    name->mVtable = &sGroupStringVtable;
    return name;
}
static hdIngamePointerVector* constructGroupVector(hdIngamePointerVector* vector) {
    if (vector == NULL) {
        vector = static_cast<hdIngamePointerVector*>(operator_new(sizeof(hdIngamePointerVector)));
        if (vector == NULL) return NULL;
    }
    vector->mReserved = 0;
    vector->mpVtable = &sPointerVectorBaseVtable;
    vector->mCount = 0;
    vector->mCapacity = 0;
    vector->mpElements = NULL;
    return vector;
}
static void constructScreenGroup(hdIngameScreenGroup* group) {
    hdIngameDisposer* disposer = reinterpret_cast<hdIngameDisposer*>(group);
    hdIngameConstructGroupDisposer(disposer);
    hdUiSafeString* archive = gabi::at<hdUiSafeString>(gabi::ea(group) + offsetof(hdIngameScreenGroup, mArchiveName));
    constructGroupArchiveName(archive);
    disposer->mpVtable = &sGroupDisposerVtable;
    group->mCount = 0;
    group->mCapacity = 0;
    hdIngamePointerVector* vector = gabi::at<hdIngamePointerVector>(gabi::ea(group) + offsetof(hdIngameScreenGroup, mChildren));
    if (vector != NULL) {
        constructGroupVector(vector);
        vector->mpVtable = &sPointerVectorVtable;
    }
}
static void constructSceneFlag(be<u8>* flag) {
    if (flag == NULL) {
        flag = static_cast<be<u8>*>(operator_new(1));
        if (flag == NULL) return;
    }
    *flag = 0;
}

static hdPartsStateId_c sOverlayDisplayState;
static hdPartsStateId_c sOverlayReturnState;
static hdPartsStateId_c sMenuListState;
static hdPartsStateId_c sHintIdleState;
static u8 sUiInputSuspended;
u32 hdIngameIsDrcMode(hdIngameUi_c*);
void hdIngameSuspendSceneUpdates();
void hdIngameSetHintSelection(hdSequenceWindow_c*, u32);
u32 hdIngameSceneSoundActive();
void hdIngameSetSceneSound(const char*);
void hdIngamePlaySceneSound();
void hdIngameCompleteHint(hdIngameUi_c*);

static bool partsStateMatches(hdStateMachine* machine, hdPartsStateId_c* expected) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(machine);
    u32 currentMethod = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedVtable = expected->mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentMethod, current);
    u32 expectedKey = gabi::call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
    return currentKey == expectedKey;
}

/* 027109CC */
void hdIngameUi_c::waitForMenuExit() {
    if (mpOverlay->isInAnimationFinished()) {
        hdIngameChangeState(&mState, &l_ingamePictographListState);
    }
}

/* 02710A14 */
void hdIngameUi_c::requestMenuIdle() {
    hdParts_changeState(mpMenu->stateMachine(), &sMenuIdleState);
}

/* 02710A28 */
void hdIngameUi_c::updatePictographList() {
    hdIngameFramework_l* framework = hdIngameGetFramework();
    hdIngameSetFrameworkRunning(framework, 0);
    hdPartsStateId_c* current = hdParts_getCurrentStateId(mpOverlay->stateMachine());
    u32 currentMethod = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* displayVtable = sOverlayDisplayState.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentMethod, current);
    u32 displayKey = gabi::call_ptr<u32>(displayVtable->mGetKey.get(), &sOverlayDisplayState);
    if (currentKey == displayKey && (hdInputGetManager()->mPointer.mHold & 2)) {
        gameInfo()->play.mScopeMesgStatus = 11;
        daPy_py_c* link = player(gameInfo());
        link->mNoResetFlg0 = link->mNoResetFlg0 | 0x80000;
        gameInfo()->play.mPictureStatus = 0;
        hdParts_changeState(mpMenu->stateMachine(), &sMenuClosingState);
        hdParts_changeState(mpOverlay->stateMachine(), &sOverlayClosingState);
        hdIngamePlayMenuExitSound(0x824);
        hdIngameChangeState(&mState, &l_ingameExitPictographListState);
    } else {
        current = hdParts_getCurrentStateId(mpOverlay->stateMachine());
        currentKey = current->getKey();
        displayKey = gabi::call_ptr<u32>(displayVtable->mGetKey.get(), &sOverlayDisplayState);
        if (currentKey == displayKey && (hdInputGetManager()->mPointer.mHold & 0x20)) {
            hdParts_changeState(mpOverlay->stateMachine(), &sOverlayReturnState);
            hdIngamePlayMenuExitSound(0x8D3);
            hdIngameChangeState(&mState, &l_ingameReturnPictographBoxState);
        }
    }
}

/* 02710BAC */
void hdIngameUi_c::resumeFramework() {
    hdIngameFramework_l* framework = hdIngameGetFramework();
    hdIngameSetFrameworkRunning(framework, 1);
}

/* 02710BD8 */
void hdIngameUi_c::waitForMenuEntry() {
    if (mpOverlay->isOutAnimationFinished()) {
        hdParts_changeState(mpMenu->stateMachine(), &sMenuEntryState);
        hdIngameChangeState(&mState, &l_ingamePictographBoxState);
    }
}

/* 02710C38 */
void hdIngameUi_c::waitForPictographReturn() {
    dMenu_setMenuStatus(0);
    gameInfo()->play.mScopeMesgStatus = 11;
    daPy_py_c* link = player(gameInfo());
    link->mNoResetFlg0 = link->mNoResetFlg0 | 0x80000;
    if (mpOverlay->isOutAnimationFinished() && !(gameInfo()->play.mPlayerStatus[0][1] & 8)) {
        if (hdIngameIsDrcMode(this)) hdIngameChangeState(&mState, &l_ingameToPauseDrcState);
        else hdIngameChangeState(&mState, &l_ingameToPlayState);
    }
}

/* 02710CDC */
void hdIngameUi_c::beginPictographList() {
    cking::ui::input::EventMgr* manager = sInputEventManager;
    sUiMode = 4;
    (manager)->reset();
    hdParts_changeState(mpOverlay->stateMachine(), &sMenuListState);
    hdIngameSuspendSceneUpdates();
}

/* 02710D34 */
void hdIngameUi_c::finishMenuExit() {
    if (mpOverlay->isInAnimationFinished()) {
        dMenu_setMenuStatus(1);
        hdIngameChangeState(&mState, &l_ingameSelectPictographListState);
    }
}

/* 02710EE0 */
void hdIngameUi_c::beginFishManHint() {
    u32 selection = (mpMapCoordinator)->getSelectedSeaCell();
    hdIngameSetHintSelection(mpHintScreen, selection);
    hdInputManager* input = hdInputGetManager();
    sUiInputSuspended = 1;
    if ((input)->isTvMode()) dMenu_setMenuStatus(1);
    if (!hdIngameSceneSoundActive()) {
        hdIngameSetSceneSound("SE_MENU_IN");
        hdIngamePlaySceneSound();
    }
    hdIngameChangeState(&mState, &l_ingameFishManHintState);
}

/* 02710F84 */
void hdIngameUi_c::updateFishManHint() {
    hdSequenceWindow_c* hint = mpHintScreen;
    if (partsStateMatches(hint->stateMachine(), &sHintIdleState) || hint->mAdvanceRequested != 0) {
        sUiInputSuspended = 0;
        hdIngameCompleteHint(this);
    }
}

/* 02710F70 */
void hdIngameUi_c::enableDirectionalInput() {
    sInputOptions->mDirectionalInputEnabled = 1;
}

/* 02710D88 */
void hdIngameUi_c::returnFromPictographList() {
    bool pressed = partsStateMatches(mpOverlay->stateMachine(), &sOverlayDisplayState) &&
                   (hdInputGetManager()->mPointer.mHold & 2);
    if (gameInfo()->play.mPictureStatus != 2 || pressed) {
        if (pressed) {
            gameInfo()->play.mPictureStatus = 0;
            hdIngamePlayMenuExitSound(0x824);
        }
        hdParts_changeState(mpOverlay->stateMachine(), &sOverlayClosingState);
        hdIngameChangeState(&mState, &l_ingameExitPictographListState);
    }
}

/* 02710E88 */
void hdIngameUi_c::saveInputReceiver() {
    u32 receiver = (sInputEventManager)->getMode();
    mpMapCoordinator->mMapFlags = receiver;
    (sInputEventManager)->setModeDefault(0);
    hdIngamePrepareMapCoordinator(this);
}

/* 02711030 */
void hdIngameUi_c::restoreInputReceiver() {
    cking::ui::input::EventMgr* manager = sInputEventManager;
    u32 receiver = mpMapCoordinator->mMapFlags;
    (manager)->setModeDefault(receiver);
    mpMapCoordinator->mMapFlags = 0;
}

static hdCursorMgr_c* sCursorManager;
void hdIngameBeginGameOver();
void hdIngameSetHintScreen(hdSequenceWindow_c*, u32, u32);
static void hdIngameActivateMenuButton(hdMenuButton_c* button) { button->forceDecide(); }

static hdPartsStateId_c sPictographIdleState;
static hdPartsStateId_c sFishManDisplayState;
static hdIngameStateId_c l_ingameMapCoordinatorClosedState;

/* 02711328 */
void ingameDemoTauraArrivalExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mPausePhase)) {
    case 0: {
        hdParts_c* pictograph = scene->mpPictographScreen;
        if (!partsStateMatches(pictograph->stateMachine(), &sPictographIdleState)) break;
        if (scene->mMenuSelection != 1) {
            hdMenuButtons* buttons = scene->mpMenuButtons;
            scene->mTransitionFrames = 1;
            hdIngameActivateMenuButton(buttons->button(1));
        }
        scene->mTransitionPhase = 20;
        scene->mPausePhase = 4;
        break;
    }
    case 4: {
        u32 frames = scene->mTransitionPhase;
        scene->mTransitionPhase = frames - 1;
        if (frames == 0) scene->mPausePhase = 5;
        break;
    }
    case 5:
        hdIngameChangeState(&scene->mState, &l_ingameToPauseDrcState);
        break;
    }
}

/* 02711834 */
void ingameDemoFishManExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mPausePhase)) {
    case 0:
        if ((scene->mpMapCoordinator)->isSeaMapSelectionEmpty() == 0) {
            sUiMode = 4;
            scene->mPausePhase = 4;
        }
        break;
    case 4: {
        hdParts_c* pictograph = scene->mpPictographScreen;
        if (partsStateMatches(pictograph->stateMachine(), &sFishManDisplayState)) {
            (scene->mpMapCoordinator)->changeSelectedMap();
            scene->mPausePhase = 5;
        }
        break;
    }
    case 5:
        if (sceneStateMatches(&scene->mpMapCoordinator->mState.mMachine, &l_ingameMapCoordinatorClosedState)) {
            (scene->mpMapCoordinator)->showSelectedMap();
            gameInfo()->play.mFmapOpen = 0;
            hdIngameChangeState(&scene->mState, &l_ingameToPauseDrcState);
        }
        break;
    }
}

u32 hdIngameCanCloseItemScreen(hdIngameUi_c*);
void hdIngameDismissItemScreen(void*);

/* 02711508 */
void ingameToDemoFishManExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mPausePhase)) {
    case 0: {
        hdParts_c* pictograph = screenParts(scene->mpPictographScreen);
        if (!partsStateMatches(pictograph->stateMachine(), &sPictographIdleState)) break;
        bool delay = false;
        u32 active = scene->mMenuSelection;
        if (active == 0) {
            if (hdIngameCanCloseItemScreen(scene)) {
                hdMenuMain_c* item = scene->mpItemScreen;
                if (item->mWindowMode != 3) {
                    hdIngameDismissItemScreen(item);
                    delay = (hdInputGetManager())->isAlternateGamePadMode() == 0;
                }
            }
        } else if (active == 1 && !(scene->mpMapCoordinator)->isSeaMapCursorActive()) {
            u32 transitioning = (hdInputGetManager())->isAlternateGamePadMode();
            hdMapDisplayCoordinator_c* buttons = scene->mpMapCoordinator;
            if (transitioning != 0) (buttons)->updateSeaMapCursor();
            else (buttons)->updateSeaMapSelection();
            delay = (hdInputGetManager())->isAlternateGamePadMode() == 0;
        }
        scene->mTransitionPhase = delay ? 20 : 0;
        scene->mPausePhase = 1;
        break;
    }
    case 1: {
        u32 frames = scene->mTransitionPhase;
        scene->mTransitionPhase = frames - 1;
        if (frames != 0) break;
        if (scene->mMenuSelection == 1) {
            scene->mTransitionPhase = 0;
            scene->mPausePhase = 4;
        } else {
            hdMenuButtons* buttons = scene->mpMenuButtons;
            scene->mTransitionFrames = 1;
            scene->mTransitionPhase = 20;
            hdIngameActivateMenuButton(buttons->button(1));
            scene->mPausePhase = 4;
        }
        break;
    }
    case 4: {
        u32 frames = scene->mTransitionPhase;
        scene->mTransitionPhase = frames - 1;
        if (frames == 0) {
            (sInputEventManager)->reset();
            scene->mPausePhase = 5;
        }
        break;
    }
    case 5:
        if (partsStateMatches(screenParts(scene->mpPictographScreen)->stateMachine(), &sPictographIdleState)) {
            hdIngameChangeState(&scene->mState, &l_ingameDemoFishManState);
        }
        break;
    }
}


/* 02711A2C */
void ingameToDemoTriforceExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mPausePhase)) {
    case 0: {
        if (!partsStateMatches(screenParts(scene->mpPictographScreen)->stateMachine(), &sPictographIdleState)) break;
        bool delay = false;
        u32 active = scene->mMenuSelection;
        if (active == 0) {
            if (hdIngameCanCloseItemScreen(scene)) {
                hdMenuMain_c* item = scene->mpItemScreen;
                if (item->mWindowMode != 3) {
                    hdIngameDismissItemScreen(item);
                    delay = (hdInputGetManager())->isAlternateGamePadMode() == 0;
                }
            }
        } else if (active == 1 && !(scene->mpMapCoordinator)->isSeaMapCursorActive()) {
            (scene->mpMapCoordinator)->updateSeaMapSelection();
            delay = (hdInputGetManager())->isAlternateGamePadMode() == 0;
        }
        scene->mTransitionPhase = delay ? 20 : 0;
        scene->mPausePhase = 1;
        break;
    }
    case 1: {
        u32 frames = scene->mTransitionPhase;
        scene->mTransitionPhase = frames - 1;
        if (frames == 0) {
            hdMenuButtons* buttons = scene->mpMenuButtons;
            scene->mTransitionFrames = 0;
            scene->mTransitionPhase = 20;
            hdIngameActivateMenuButton(buttons->button(0));
            scene->mPausePhase = 2;
        }
        break;
    }
    case 2: {
        u32 frames = scene->mTransitionPhase;
        scene->mTransitionPhase = frames - 1;
        if (frames != 0) break;
        hdMenuMain_c* item = scene->mpItemScreen;
        if (item->mSelectionMode == 1) scene->mPausePhase = 4;
        else {
            item->setSelectionMode(1);
            scene->mPausePhase = 3;
        }
        break;
    }
    case 3:
        if (scene->mpItemScreen->mOpenRequested != 0) {   /* the scene waits on this byte as "open finished" */
            scene->mPausePhase = 4;
        }
        break;
    case 4:
        (sInputEventManager)->reset();
        scene->mPausePhase = 5;
        break;
    case 5:
        if (partsStateMatches(screenParts(scene->mpPictographScreen)->stateMachine(), &sPictographIdleState)) {
            hdIngameChangeState(&scene->mState, &l_ingameDemoTriforceState);
        }
        break;
    }
}

/* 027119C0 */
void ingameToDemoTriforceEnter(hdIngameUi_c* scene) {
    sUiMode = 0;
    hdIngameFinishPictograph(scene->mpPictographScreen);
    if (scene->mTransitionFrames == 0 &&
        scene->mpItemScreen->mSelectionMode == 1) {
        scene->mPausePhase = 4;
    } else {
        scene->mPausePhase = 0;
    }
}

/* 0271201C */
void ingameGameOverEnter(hdIngameUi_c* scene) {
    scene->mMenuRequested = 1;
    scene->mReturnMenuRequested = 0;
    sUiMode = 4;
    (scene->mpMapCoordinator)->updateSeaMapCursor();
    sCursorManager->mActive = 0;
    hdIngameBeginGameOver();
}

/* 02712074 */
void ingameGameOverExecute(hdIngameUi_c* scene) {
    if (scene->mReturnMenuRequested != 0) {
        hdIngameSetHintScreen(scene->mpHintScreen, 1, 0);
        hdMenuButtons* buttons = scene->mpMenuButtons;
        scene->mTransitionFrames = 0;
        hdIngameActivateMenuButton(buttons->button(0));
        hdIngameChangeState(&scene->mState, &l_ingameToSaveState);
    }
}

static hdSwkbd_mgr_c* sKeyboardManager;

/* 02711078 */
void ingameInputPasswordEnter(hdIngameUi_c* scene) {
    sKeyboardManager->mMode = 1;
    (sKeyboardManager)->open();
    (sKeyboardManager)->layoutCall(1);
    scene->mPausePhase = 0;
    sUiInputSuspended = 1;
    scene->mpNameEntryScreen->mCaptureKind = 7;
    hdNoOperateDRC_c* touch = scene->mpTouchPanel;
    scene->mTouchPanelRequested = 1;
    touch->mDisabled = 1;   /* #2 read this as the touch panel switch */
}

struct hdIngameWideSafeString_l {
    gptr<be<u16>> mText;
    gptr<hdUiStringTable> mpVtable;
};
struct hdIngamePasswordBuffer_l {
    u8 mPrefix[0x4950];
    gptr<be<u16>> mText;
    u8 mReserved[4];
    be<s32> mCapacity;
};
WWHD_SIZE(hdIngameWideSafeString_l, 8);
WWHD_OFFSET(hdIngamePasswordBuffer_l, mText, 0x4950);
WWHD_OFFSET(hdIngamePasswordBuffer_l, mCapacity, 0x4958);
static hdUiStringTable sWideStringVtable;
u32 hdIngamePasswordAccepted();
u32 hdIngamePasswordCancelled();
void* hdIngameCopyPassword(void*, void*, u32, u32);

/* 027110FC */
void ingameInputPasswordExecute(hdIngameUi_c* scene) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch (u32(scene->mPausePhase)) {
    case 0:
        scene->mPausePhase = 4;
        break;
    case 4: {
        u32 accepted = hdIngamePasswordAccepted();
        hdSwkbd_mgr_c* manager = sKeyboardManager;
        if (accepted != 0) {
            be<u16>* entered = (manager)->getText();
            hdIngamePasswordBuffer_l* buffer = reinterpret_cast<hdIngamePasswordBuffer_l*>(&gameInfo()->play);
            gabi::FrameLocal<hdIngameWideSafeString_l> textLocal(8);
            hdIngameWideSafeString_l& text = *textLocal;
            text.mText = entered;
            text.mpVtable = &sWideStringVtable;
            be<u16>* destination = buffer->mText;
            gabi::call_ptr(sWideStringVtable.mAssureTermination.get(), &text);
            be<u16>* cursor = text.mText;
            u32 length = 0;
            while (*cursor != 0) {
                ++length;
                ++cursor;
                if (length > 0x40000) { length = 0; break; }
            }
            s32 capacity = buffer->mCapacity;
            u32 assure = text.mpVtable->mAssureTermination.get();
            if (s32(length) >= capacity) length = u32(capacity) - 1;
            gabi::call_ptr(assure, &text);
            hdIngameCopyPassword(destination, text.mText.get(), length << 1, 0);
            *gabi::at<be<u16>>(gabi::ea(destination) + (length << 1)) = 0;
            (sKeyboardManager)->close();
            gameInfo()->play.mNameOpen = 1;
            sUiInputSuspended = 0;
            scene->mPausePhase = 5;
        } else if (hdIngamePasswordCancelled()) {
            (sKeyboardManager)->close();
            gameInfo()->play.mNameOpen = 0;
            sUiInputSuspended = 0;
            scene->mPausePhase = 5;
        }
        break;
    }
    case 5:
        if ((sKeyboardManager)->isHidden()) {
            hdIngameChangeState(&scene->mState, &l_ingamePauseDrcState);
        }
        break;
    }
}

/* 027112D0 */
void hdIngameUi_c::clearTouchPanel() {
    mpTouchPanel->mDisabled = 0;
}

/* 027112E0 */
void hdIngameUi_c::closePictographList() {
    sUiMode = 0;
    hdIngameFinishPictograph(mpPictographScreen);
    mPausePhase = 0;
}

/* 0271148C */
void hdIngameUi_c::beginPictographTransition() {
    sUiMode = 0;
    hdIngameFinishPictograph(mpPictographScreen);
    if (mTransitionFrames == 1 && (mpMapCoordinator)->isSeaMapCursorActive()) {
        mPausePhase = 4;
        mTransitionPhase = 20;
    } else {
        mPausePhase = 0;
        mTransitionPhase = 20;
    }
}

/* 027117FC */
void hdIngameUi_c::endMapCoordinator() {
    (mpMapCoordinator)->clearSeaMapSelection();
    mPausePhase = 0;
}

/* 02711D3C */
void hdIngameUi_c::endItemScreen() {
    hdIngamePauseScene();
    hdIngameCloseItemScreen(mpItemScreen);
    mPausePhase = 0;
}

/* 02711F10 */
void hdIngameUi_c::resumeSceneFramework() {
    hdIngameFrameworkResume();
}

/* 02711F14 */
void hdIngameUi_c::beginMenuExit() {
    gameInfo()->play.mScopeMesgStatus = 11;
    daPy_py_c* link = player(gameInfo());
    link->mNoResetFlg0 = link->mNoResetFlg0 | 0x80000;
    gameInfo()->play.mPictureStatus = 0;
    hdParts_changeState(mpMenu->stateMachine(), &sMenuClosingState);
    hdParts_changeState(mpOverlay->stateMachine(), &sOverlayClosingState);
    hdIngamePlayMenuExitSound(0x824);
}

/* 02711F98 */
void hdIngameUi_c::updateMenuExit() {
    gameInfo()->play.mScopeMesgStatus = 11;
    daPy_py_c* link = player(gameInfo());
    link->mNoResetFlg0 = link->mNoResetFlg0 | 0x80000;
    gameInfo()->play.mPictureStatus = 0;
    if (!(gameInfo()->play.mPlayerStatus[0][1] & 8) && mpOverlay->isOutAnimationFinished()) {
        hdIngameChangeState(&mState, &l_ingameToPauseDrcState);
    }
}

static hdPartsStateId_c sBatonTvState;
static hdPartsStateId_c sBatonDrcState;
static hdPartsStateId_c sBatonEffectsClosedState;
u32 hdIngameModeSwitchEntryComplete(hdParts_c*);
void hdIngameFinishModeSwitchScreen(hdParts_c*);
u32 hdIngameModeSwitchExitComplete(hdParts_c*);
void hdIngameResetPictograph(void*);

/* 027121B8 */
void ingameChangeToTvModeExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mModeSwitchPhase)) {
    case 0:
        if (hdIngameModeSwitchEntryComplete(scene->mpModeSwitchScreen)) {
            if (!hdIngameIsDrcMode(scene)) (scene->mpMenuButtons)->closeMenu();
            hdParts_changeState(scene->mpBatonSign->stateMachine(), &sBatonTvState);
            (hdInputGetManager())->setMode(1);
            sUiMode = 0;
            hdIngameResetPictograph(scene->mpPictographScreen);
            scene->mModeSwitchFrames = 2;
            scene->mModeSwitchPhase = 1;
        }
        break;
    case 1: {
        u32 frames = scene->mModeSwitchFrames;
        scene->mModeSwitchFrames = frames - 1;
        if (frames == 0) {
            hdIngameFinishModeSwitchScreen(scene->mpModeSwitchScreen);
            if (hdIngameIsDrcMode(scene)) sUiMode = 4;
            scene->mModeSwitchPhase = 6;
            scene->updateModeSwitchInput();
        }
        break;
    }
    case 6:
        if (hdIngameModeSwitchExitComplete(scene->mpModeSwitchScreen)) {
            hdParts_changeState(scene->mpBatonEffects->stateMachine(), &sBatonEffectsClosedState);
            hdIngameSetSceneSound("SE_GAME_MODE_SWITCH_END");
            if (hdIngameIsDrcMode(scene)) {
                hdIngameChangeState(&scene->mState, &l_ingamePauseDrcState);
            } else {
                (scene->mpMenuButtons)->openMenu();
                hdIngameChangeState(&scene->mState, &l_ingamePlayState);
            }
        }
        break;
    }
}

/* 027123E0 */
void ingameChangeToDrcModeExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mModeSwitchPhase)) {
    case 0:
        if (hdIngameModeSwitchEntryComplete(scene->mpModeSwitchScreen)) {
            if (!hdIngameIsDrcMode(scene)) (scene->mpMenuButtons)->closeMenu();
            hdParts_changeState(scene->mpBatonSign->stateMachine(), &sBatonDrcState);
            (hdInputGetManager())->setMode(2);
            sUiMode = 0;
            hdIngameResetPictograph(scene->mpPictographScreen);
            scene->mModeSwitchFrames = 2;
            scene->mModeSwitchPhase = 1;
        }
        break;
    case 1: {
        u32 frames = scene->mModeSwitchFrames;
        scene->mModeSwitchFrames = frames - 1;
        if (frames == 0) {
            hdIngameFinishModeSwitchScreen(scene->mpModeSwitchScreen);
            if (hdIngameIsDrcMode(scene)) sUiMode = 4;
            scene->mModeSwitchPhase = 6;
            scene->updateModeSwitchInput();
        }
        break;
    }
    case 6:
        if (hdIngameModeSwitchExitComplete(scene->mpModeSwitchScreen)) {
            hdIngameSetSceneSound("SE_GAME_MODE_SWITCH_END");
            if (hdIngameIsDrcMode(scene)) {
                hdIngameChangeState(&scene->mState, &l_ingamePauseDrcState);
            } else {
                hdIngameChangeState(&scene->mState, &l_ingamePlayState);
            }
        }
        break;
    }
}

/* 027120E8 */
void hdIngameUi_c::beginTvModeSwitch() {
    hdIngameBeginModeSwitchScreen(mpModeSwitchScreen);
    (sInputEventManager)->reset();
    hdIngameSetTvScreens(this, 0);
    hdIngameSetDrcScreens(this, 0);
    mModeSwitchPhase = 0;
    hdIngameSetModeSwitchRendering(1);
}

/* 0271214C */
void hdIngameUi_c::updateModeSwitchInput() {
    if ((hdInputGetManager())->isGamePadMode() ||
        (hdInputGetManager())->isTvMode() ||
        (hdInputGetManager())->isCombinedMode()) {
        hdIngameSetModeSwitchInput(0);
    } else {
        hdIngameSetModeSwitchInput(1);
    }
}

/* 0271237C */
void hdIngameUi_c::beginDrcModeSwitch() {
    hdIngameBeginModeSwitchScreen(mpModeSwitchScreen);
    (sInputEventManager)->reset();
    hdIngameSetTvScreens(this, 0);
    hdIngameSetDrcScreens(this, 0);
    mModeSwitchPhase = 0;
    hdIngameSetModeSwitchRendering(1);
}

/* 02712584 */
void hdIngameUi_c::beginProModeSwitch() {
    hdIngameBeginModeSwitchScreen(mpModeSwitchScreen);
    (sInputEventManager)->reset();
    mModeSwitchPhase = 0;
    hdIngameFramework_l* framework = hdIngameGetFramework();
    hdIngameSetFrameworkRunning(framework, 0);
    hdIngameSetModeSwitchRendering(1);
}

/* 02712844 */
void hdIngameUi_c::resumeAfterModeSwitch() {
    hdIngameFramework_l* framework = hdIngameGetFramework();
    hdIngameSetFrameworkRunning(framework, 1);
}

/* 02713D50 */
void hdIngameUi_c::captureModeSwitchButtons() {
    mModeSwitchButtonsHeld = 0;
    if (hdInputGetManager()->mPointer.mHold & 0x1800) {
        mModeSwitchButtonsHeld = 1;
    }
}

/* 027141E0 */
void hdIngameUi_c::connectItemScreenOpen() {
    mpMenuButtons->connectButtons();
    u32 first = mpMenuButtons->buttonKey(0);
    u32 second = mpMenuButtons->buttonKey(1);
    u32 third = mpMenuButtons->buttonKey(2);
    hdIngameSetItemScreenReceivers(mpItemScreen, first, second, third);
    hdIngameConnectItemScreenOpen(mpItemScreen);
}

/* 02714268 */
void hdIngameUi_c::connectItemScreenClosed() {
    mpMenuButtons->connectButtons();
    u32 first = mpMenuButtons->buttonKey(0);
    u32 second = mpMenuButtons->buttonKey(1);
    u32 third = mpMenuButtons->buttonKey(2);
    hdIngameSetItemScreenReceivers(mpItemScreen, first, second, third);
    hdIngameConnectItemScreenClosed(mpItemScreen);
}

/* 027142F0 */
void hdIngameUi_c::connectMapCoordinatorOpen() {
    mpMenuButtons->connectButtons();
    u32 first = mpMenuButtons->buttonKey(0);
    u32 second = mpMenuButtons->buttonKey(1);
    u32 third = mpMenuButtons->buttonKey(2);
    (mpMapCoordinator)->configureDungeonMap(first, second, third);
}

/* 02714370 */
void hdIngameUi_c::connectMapCoordinatorClosed() {
    mpMenuButtons->connectButtons();
    u32 first = mpMenuButtons->buttonKey(0);
    u32 second = mpMenuButtons->buttonKey(1);
    u32 third = mpMenuButtons->buttonKey(2);
    (mpMapCoordinator)->configureSeaMap(first, second, third);
}

struct hdIngameInputModeFlags_l {
    u8 mPrefix[0x26C];
    be<u8> mModeChangeRequested;
};
static hdPartsStateId_c sModeHintIdleState;
void hdIngameBeginModeHint(void*);
void hdIngameCompleteProMode(hdIngameUi_c*);

/* 027125E4 */
void ingameExchangeProModeExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mModeSwitchPhase)) {
    case 0:
        if (hdIngameModeSwitchEntryComplete(scene->mpModeSwitchScreen)) {
            hdParts_changeState(scene->mpBatonSign->stateMachine(), &sBatonTvState);
            sUiMode = 0;
            hdIngameResetPictograph(scene->mpPictographScreen);
            sUiInputSuspended = 0;
            hdIngameBeginModeHint(scene->mpNameEntryScreen);
            dMenu_setMenuStatus(0);
            scene->mModeSwitchPhase = 2;
        }
        break;
    case 2: {
        hdSequenceOption_c* hint = scene->mpHintEffects;
        hdInputManager* input = hdInputGetManager();
        (input)->setMode(hint->mPreviousCategory);
        hdIngamePrepareMapCoordinator(scene);
        scene->mModeSwitchPhase = 4;
        break;
    }
    case 4:
        dMenu_setMenuStatus(1);
        scene->mModeSwitchFrames = 2;
        scene->mModeSwitchPhase = 1;
        break;
    case 1: {
        u32 frames = scene->mModeSwitchFrames;
        scene->mModeSwitchFrames = frames - 1;
        if (frames == 0) {
            hdIngameFinishModeSwitchScreen(scene->mpModeSwitchScreen);
            sUiInputSuspended = 1;
            scene->mModeSwitchPhase = 6;
            scene->updateModeSwitchInput();
        }
        break;
    }
    case 6:
        if (hdIngameModeSwitchExitComplete(scene->mpModeSwitchScreen)) {
            if (!(hdInputGetManager())->isTvMode()) {
                reinterpret_cast<hdIngameInputModeFlags_l*>(hdInputGetManager())->mModeChangeRequested = 1;
            }
            hdIngameSetSceneSound("SE_GAME_MODE_SWITCH_END");
            scene->mpHintEffects->mCloseRequested = 0;
            scene->mModeSwitchPhase = 7;
        }
        break;
    case 7:
        if (partsStateMatches(scene->mpHintEffects->stateMachine(), &sModeHintIdleState)) {
            hdIngameSetDrcScreens(scene, 1);
            sUiInputSuspended = 0;
            hdIngameCompleteProMode(scene);
        }
        break;
    }
}

u32 hdIngameItemScreenReady(void*);
void hdIngameFinishTriforceItemScreen(void*);
void hdIngameSetItemScreenActive(void*, u32);

/* 02711D78 */
void ingameDemoTriforceExecute(hdIngameUi_c* scene) {
    switch (u32(scene->mPausePhase)) {
    case 0:
        if (hdIngameItemScreenReady(scene->mpItemScreen)) {
            sUiMode = 4;
            scene->mPausePhase = 4;
        }
        break;
    case 4:
        if (partsStateMatches(screenParts(scene->mpPictographScreen)->stateMachine(), &sFishManDisplayState)) {
            hdIngameFinishTriforceItemScreen(scene->mpItemScreen);
            scene->mPausePhase = 5;
        }
        break;
    case 5:
        if (partsStateMatches(screenParts(scene->mpItemScreen)->stateMachine(), &sModeHintIdleState)) {
            hdIngameSetItemScreenActive(scene->mpItemScreen, 1);
            gameInfo()->play.mMenuCollect = 0;
            (scene->mpMapCoordinator)->updateSeaCells();
            hdIngameChangeState(&scene->mState, &l_ingamePauseDrcState);
        }
        break;
    }
}


/* 027143F0 */
void hdIngameUi_c::connectSubWindow() {
    mpMenuButtons->connectButtons();
    (mpSubWindow)->resetChartButtons();
    (mpSubWindow)->updateChartNavigationLinks();
    hdMenuButtons* buttons = mpMenuButtons;
    hdMenuButton_c* first = buttons->button(0);
    hdMenuButton_c* second = buttons->button(1);
    hdMenuButton_c* third = buttons->button(2);
    (mpSubWindow)->updateFirstNavigationLinks(gabi::ea(first), gabi::ea(second), gabi::ea(third));
    buttons = mpMenuButtons;
    first = buttons->button(0);
    second = buttons->button(1);
    third = buttons->button(2);
    (mpSubWindow)->updateLastNavigationLinks(gabi::ea(first), gabi::ea(second), gabi::ea(third));
}

/* 027144C8 */
void hdIngameUi_c::refreshScreenReceivers() {
    if (mMenuSelection == 0) {
        if (mpItemScreen->mSelectionMode == 1) connectItemScreenOpen();
        else connectItemScreenClosed();
    }
    if (mMenuSelection == 1) {
        if (mpMapCoordinator->mMode == 1) connectMapCoordinatorOpen();
        else connectMapCoordinatorClosed();
    }
    if (mMenuSelection == 2) connectSubWindow();
}

u32 hdIngameMenuControlsBlocked(hdIngameUi_c*);
u8 hdIngameMenuPauseFlag();

/* 02713D78 */
void hdIngameUi_c::handleMenuActions() {
    u32 enabled = 0;
    if ((sceneStateMatches(&mState, &l_ingamePlayState) ||
         sceneStateMatches(&mState, &l_ingameMenuState) ||
         sceneStateMatches(&mState, &l_ingameMenuSubWindowState)) &&
        !hdIngameMenuControlsBlocked(this)) {
        enabled = 1;
        if ((hdInputGetManager())->isAlternateGamePadMode() && !hdIngameMenuPauseFlag()) enabled = 0;
    }
    mpMenuButtons->mEnabled = enabled;
    if ((mpMapCoordinator)->isSeaMapSelected()) mpMenuButtons->mEnabled = 0;
    mpItemScreen->mInputEnabled = enabled;
    (mpMapCoordinator)->setMarkersActive(enabled);
    (mpSubWindow)->setButtonsEnabled(enabled);
}

/* 02713F10 */
void hdIngameUi_c::updateMenuButtons() {
    if ((hdInputGetManager())->isTvMode() &&
        !sceneStateMatches(&mState, &l_ingameToPlayState) &&
        !sceneStateMatches(&mState, &l_ingamePlayState)) {
        reinterpret_cast<hdIngameInputModeFlags_l*>(hdInputGetManager())->mModeChangeRequested = 0;
    }
}

static void* sControllerManager;

/* 027151A0 */
void hdIngameUi_c::handleInvalidControls() {
    hdController* controller = hdInputControllerByOrder(sControllerManager, INPUT_CONTROLLER_PAD, 0);
    u32 tvInput = (hdInputGetManager())->hasPad();
    bool invalid = tvInput != 0 && controller != NULL &&
        (u32(controller->mHold) & 0x8000) != 0 &&
        !(hdInputGetManager())->isAlternateGamePadMode() && isMenuMode() &&
        !sceneStateMatches(&mState, &l_ingameInputPasswordState);
    if (invalid) {
        mModeSwitchButtonsHeld = 1;
        hdIngameSetSceneSound("SE_INVALID_CONTROL");
    } else if (mModeSwitchButtonsHeld != 0) {
        hdIngameSetSceneSound("SE_INVALID_CONTROL");
    }
}

static hdPartsStateId_c sSubWindowClosingState;
static hdPartsStateId_c sSubWindowOpeningState;
static void hdIngameDeactivateMenuButton(hdMenuButton_c* button) { button->decideEnd(); }
void hdIngameActivateItemScreen(void*);
void hdIngameRefreshMenuSelection(hdIngameUi_c*);

/* 02714000 */
void hdIngameUi_c::changeActiveScreen() {
    u32 selected = mpMenuButtons->mSelected;
    u32 active = mMenuSelection;
    if (selected <= 2) mTransitionFrames = selected;
    else selected = mTransitionFrames;
    if (active == selected) return;
    if (active == 0) {
        hdIngameFinishTriforceItemScreen(mpItemScreen);
        hdIngameDeactivateMenuButton(mpMenuButtons->button(0));
        selected = mTransitionFrames;
    } else if (active == 1) {
        (mpMapCoordinator)->changeSelectedMap();
        hdIngameDeactivateMenuButton(mpMenuButtons->button(1));
        selected = mTransitionFrames;
    } else if (active == 2) {
        hdParts_changeState(screenParts(mpSubWindow)->stateMachine(), &sSubWindowClosingState);
        hdIngameDeactivateMenuButton(mpMenuButtons->button(2));
        selected = mTransitionFrames;
    }
    if (selected == 0) hdIngameActivateItemScreen(mpItemScreen);
    else if (selected == 1) (mpMapCoordinator)->openSelectedMap();
    else if (selected == 2) hdParts_changeState(screenParts(mpSubWindow)->stateMachine(), &sSubWindowOpeningState);
    hdIngameRefreshMenuSelection(this);
    (sInputEventManager)->setParams(0, 8);
    mMenuSelection = mTransitionFrames;
}

static u8 sMapCoordinatorOnly;

/* 02715C10 */
void hdIngameUi_c::beginScreenUpdate() {
    if (sMapCoordinatorOnly != 0) {
        (mpMapCoordinator)->saveSelectedMapSettings();
        return;
    }
    (sInputEventManager)->setModeDefault(0);
    finalizeScreens();
    hdIngameInputModeFlags_l* input = reinterpret_cast<hdIngameInputModeFlags_l*>(hdInputGetManager());
    sUiInputSuspended = 0;
    sUiMode = 0;
    input->mModeChangeRequested = 1;
}

static void finalizeOwnedScreen(gptr<void>& slot) {
    screenParts(slot)->vFinalize();
    hdParts_c* reloaded = screenParts(slot);
    if (reloaded != NULL) reloaded->vDelete(3);
}
void hdIngameFinalizeSharedMenuResources();

/* 02715374 */
void hdIngameUi_c::finalizeScreens() {
    // The middle four screens have a deliberate destruction order.
    for (u32 i = 0; i < 12; ++i) finalizeOwnedScreen(mOwnedScreens[i]);
    finalizeOwnedScreen(mOwnedScreens[15]);
    finalizeOwnedScreen(mOwnedScreens[12]);
    finalizeOwnedScreen(mOwnedScreens[14]);
    finalizeOwnedScreen(mOwnedScreens[13]);
    for (u32 i = 16; i < 40; ++i) finalizeOwnedScreen(mOwnedScreens[i]);
    (mpMapCoordinator)->destroyMaps();
    hdMapDisplayCoordinator_c* buttons = mpMapCoordinator;
    if (buttons != NULL) gabi::call_ptr(reinterpret_cast<hdIngameDestroyVtable*>(buttons->mState.mpVTable.get())->mDestroy.get(), buttons, 3);
    (sCursorManager)->destroyCursors();
    hdCursorMgr_c::deleteInstance();
    sSharedMenuScreen->vFinalize();
    hdIngameFinalizeSharedMenuResources();
}

static dMesg_core_c* sMessageCore;
static hdPartsStateId_c sMenuResetState;
static hdPartsStateId_c sOverlayResetState;
static hdPartsStateId_c sMiiversePromptResetState;
void hdIngameResetMainHud(hdParts_c*);
void hdIngameResetStatusScreen(hdParts_c*);
void hdIngameResetModeSwitchEffects(hdParts_c*);
void hdIngameResetHintScreen(hdSequenceWindow_c*);
void hdIngameResetMenuAuxiliary(hdParts_c*);

/* 0271286C */
void hdIngameUi_c::resetMenuScreens() {
    hdIngameResetMainHud(mpMainHud);
    hdIngameResetStatusScreen(mpStatusScreen);
    hdIngameResetModeSwitchEffects(mpModeSwitchEffects);
    hdIngameResetHintScreen(mpHintScreen);
    hdParts_changeState(mpMenu->stateMachine(), &sMenuResetState);
    hdParts_changeState(mpOverlay->stateMachine(), &sOverlayResetState);
    hdParts_changeState(mpMiiversePrompt->stateMachine(), &sMiiversePromptResetState);
    sMessageCore->reset();
    hdIngameResetMenuAuxiliary(mpWindGuide);
}

hdUiEvent* hdIngameCopyInputEvent(hdUiEvent*, hdUiEvent*);
u32 hdIngameItemForReceiver(void*, u32);
void hdIngameBindItemSlot(void*, u32, u32);
u32 hdIngameSlotForReceiver(void*, u32);

/* 02714FDC */
void hdIngameUi_c::handleItemAssignment() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    cking::ui::input::EventMgr* manager = sInputEventManager;
    if (manager == NULL) return;
    gabi::FrameLocal<hdUiEvent> event(8);
    hdIngameCopyInputEvent(event, manager->currentEvent());
    u32 kind = event->mKind;
    cking::ui::input::Receiver* focused = gabi::at<cking::ui::input::Receiver>((manager)->getMode());
    cking::ui::input::Receiver* hovered = gabi::at<cking::ui::input::Receiver>((manager)->getHover());
    u32 item, slot;
    if (focused != NULL && kind == 0xD && (manager)->getId() == 0) {
        u8 button = *gabi::at<be<u8>>(gabi::ea(&event->mPayload[0]));
        if (button == 3) slot = 0;
        else if (button == 4) slot = 1;
        else if (button == 0xE) slot = 2;
        else return;
        item = u8(hdIngameItemForReceiver(mpItemScreen, focused->mId));
        if (item == 0xFF) return;
        hdIngameBindItemSlot(mpItemScreen, focused->mId, slot);
        if (item == 4) return;
    } else {
        if (hovered == NULL || kind != 0x45) return;
        item = u8(hdIngameItemForReceiver(mpItemScreen, event->mId));
        slot = hdIngameSlotForReceiver(mpItemScreen, hovered->mId);
        if (slot > 2 || item == 0xFF || item == 4) return;
    }
    if (item == 0xB || item == 0x12) return;
    assignItemSlot(u8(slot), item);
}

struct hdIngameSaveRoot_l {
    u8 mPrefix[0x20];
    dSv_info_c mInfo;
};
static hdIngameSaveRoot_l* sSaveRoot;
static dSv_player_c* savePlayer() { return &sSaveRoot->mInfo.mSavedata.mPlayer; }
static be<u8>* selectedItem(dSv_player_c* player, u32 slot) {
    return gabi::at<be<u8>>(gabi::ea(&player->mPlayerStatusA.mSelectItem[0]) + slot);
}
static u8 inventoryItem(dSv_player_c* player, u32 index) {
    if (index < 0x15) return player->mPlayerItem.mItems[index];
    if (index >= 0x18 && index < 0x20) return player->mBagItem.mBeast[index - 0x18];
    if (index >= 0x24 && index < 0x2C) return player->mBagItem.mBait[index - 0x24];
    if (index - 0x30 < 8) return player->mBagItem.mReserve[index - 0x30];
    return 0xFF;
}
static void refreshSelectedItem(u32 slot) {
    u8 captured = *selectedItem(savePlayer(), slot);
    dComIfG_play_c* play = &gameInfo()->play;
    be<u8>* displayed = gabi::at<be<u8>>(gabi::ea(&play->mSelectItem[0]) + slot);
    if (captured == 0xFF) {
        *displayed = 0xFF;
        return;
    }
    dSv_player_c* player = savePlayer();
    u8 item = inventoryItem(player, *selectedItem(player, slot));
    *displayed = item;
    player = savePlayer();
    if (inventoryItem(player, *selectedItem(player, slot)) == 0xFF) *selectedItem(player, slot) = 0xFF;
}

/* 02714B0C */
void hdIngameUi_c::assignItemSlot(u32 slot, u32 item) {
    dSv_player_c* player = savePlayer();
    u32 swapSlot = 3;
    if (slot == 0) {
        if (u32(*selectedItem(player, 1)) == item) swapSlot = 1;
        else if (u32(*selectedItem(player, 2)) == item) swapSlot = 2;
    } else if (slot == 1) {
        if (u32(*selectedItem(player, 0)) == item) swapSlot = 0;
        else if (u32(*selectedItem(player, 2)) == item) swapSlot = 2;
    } else if (slot == 2) {
        if (u32(*selectedItem(player, 0)) == item) swapSlot = 0;
        else if (u32(*selectedItem(player, 1)) == item) swapSlot = 1;
    }
    if (swapSlot < 3) {
        u8 previous = *selectedItem(player, slot);
        *selectedItem(player, swapSlot) = previous;
        refreshSelectedItem(swapSlot);
    }
    *selectedItem(savePlayer(), slot) = item;
    refreshSelectedItem(slot);
}

void hdIngameLoadSharedMenu(hdParts_c*, void*, void*, u32);
void hdIngameLoadHudCounterG(void*, void*, void*, u32);
void hdIngameLoadTouchPanel(void*, void*, void*, u32);
void hdIngameLoadBatonEffects(void*, void*, void*, u32);
void hdIngameLoadMenuEffects(void*, void*, void*, u32);
static void hdIngameLoadMiiverse(void* screen, void* first, void* second, u32 flags) {
    static_cast<hdMenuMiiverse_c*>(screen)->initializeMenu(gabi::ea(first), gabi::ea(second), flags);
}
void hdIngameLoadItemScreen(void*, void*, void*, u32);
void hdIngameLoadWarpButtons(void*, void*, void*, u32);
void hdIngameLoadHudCounterF(void*, void*, void*, u32);
static void hdIngameLoadBoatRaceStart(void* screen, void* first, void* second, u32 flags) {
    static_cast<hdScreenBoatRaceStart_c*>(screen)->setup(gabi::ea(first), gabi::ea(second), gabi::at<void>(flags));
}
static void hdIngameLoadBoatRaceResult(void* screen, void* first, void* second, u32 flags) {
    static_cast<hdBoatRaceResultView*>(screen)->setup(gabi::ea(first), gabi::ea(second), gabi::at<void>(flags));
}
static void hdIngameLoadBoatRaceFailed(void* screen, void* first, void* second, u32 flags) {
    static_cast<hdScreenBoatRaceFailed_c*>(screen)->setup(gabi::ea(first), gabi::ea(second));
}
void hdIngameLoadHudCounterB(void*, void*, void*, u32);
u32 rupy_setup(RupyScreen*, u32, u32, u32);
static void hdIngameLoadAuctionRupees(void* screen, void* first, void* second, u32 flags) {
    rupy_setup(static_cast<RupyScreen*>(screen), gabi::ea(first), gabi::ea(second), flags);
}
void hdIngameLoadNameEntry(void*, void*, void*, u32);
void hdIngameLoadPictographScreen(void*, void*, void*, u32);
void hdIngameLoadModeSwitchEffects(void*, void*, void*, u32);
void hdIngameLoadModeSwitchScreen(void*, void*, void*, u32);
void hdIngameLoadMenuAuxiliary(void*, void*, void*, u32);
static void hdIngameLoadMetronome(void* screen, void* first, void* second, u32 flags) {
    static_cast<Metronome::dMetronome_c*>(screen)->screenSet(gabi::ea(first), gabi::ea(second), flags);
}
void hdIngameLoadOverlay(void*, void*, void*, u32);
void hdIngameLoadMenu(void*, void*, void*, u32);
void hdIngameLoadMiiversePrompt(void*, void*, void*, u32);
void hdIngameLoadMenuConfirmation(void*, void*, void*, u32);
void hdIngameLoadItemControlHints(void*, void*, void*, u32);
static void hdIngameLoadWindGuide(void* screen, void* first, void* second, u32 flags) {
    static_cast<dOperate_wind_c*>(screen)->_create(gabi::ea(first), gabi::ea(second), flags);
}
void hdIngameLoadModeHint(void*, void*, void*, u32);
void hdIngameLoadHintScreen(void*, void*, void*, u32);
static void hdIngameLoadMessageChoiceCursor(void* screen, void* first, void* second, u32 flags) {
    static_cast<hdScreenMsgCursor_c*>(screen)->init(gabi::ea(first), gabi::ea(second), gabi::at<void>(flags));
}
void hdIngameLoadMessageComposite(void*, void*, void*, u32);
void hdIngameLoadMessageChoice(void*, void*, void*, u32);
void hdIngameLoadMenuMessage(void*, void*, void*, u32);
void hdIngameLoadBatonSign(void*, void*, void*, u32);
static void hdIngameLoadPlaceName(void* screen, void* first, void* second, u32 flags) {
    static_cast<MenuHD::dMenu_PlaceName_c*>(screen)->create(gabi::ea(first), gabi::ea(second), flags);
}
void hdIngameLoadAuxiliaryScreen(void*, void*, void*, u32);
void hdIngameLoadStatusScreen(void*, void*, void*, u32);
void hdIngameLoadMainHud(void*, void*, void*, u32);
using hdIngameScreenConstructor = void* (*)(void*);
static void* createOwnedScreen(u32 size, hdIngameScreenConstructor constructor) {
    void* screen = operator_new(size);
    if (screen != NULL) screen = constructor(screen);
    return screen;
}
struct hdIngameMessageLoadVtable_l {
    u8 mPrefix[0x414];
    gfn mLoadScreen;
};
static hdLayerProjection_c sDefaultUiProjection;
static hdLayerMatrix_c sDefaultUiMatrix;
void hdIngameCreateSharedMenuScreen(u32);
void* hdIngameConstructMainHud(void*);
void* hdIngameConstructItemScreen(void*);
static void* hdIngameConstructMiiverse(void* storage) {
    return hdMenuMiiverse_c::construct(static_cast<hdMenuMiiverse_c*>(storage));
}
static void* hdIngameConstructMenuButtons(void* storage) {
    return hdMenuButtons::construct(static_cast<hdMenuButtons*>(storage));
}
void* hdIngameConstructStatusScreen(void*);
void* hdIngameConstructMenuEffects(void*);
void* hdIngameConstructAuxiliaryScreen(void*);
u32 dMenu_PlaceName_c__ct(MenuHD::dMenu_PlaceName_c*);
static void* hdIngameConstructPlaceName(void* storage) { return gabi::at<void>(dMenu_PlaceName_c__ct(static_cast<MenuHD::dMenu_PlaceName_c*>(storage))); }
void* hdIngameConstructBatonSign(void*);
void* hdIngameConstructBatonEffects(void*);
void* hdIngameConstructMenuMessage(void*);
void* hdIngameConstructMessageCounter(void*);
void* hdIngameConstructMessageDetail(void*);
void* hdIngameConstructMessageList(void*);
void* hdIngameConstructMessageChoice(void*);
void* hdIngameConstructMessageComposite(void*);
static void* hdIngameConstructMessageChoiceCursor(void* storage) { return hdScreenMsgCursor_c::construct(static_cast<hdScreenMsgCursor_c*>(storage)); }
static void* hdIngameConstructHintScreen(void* storage) { return hdSequenceWindow_c::construct(static_cast<hdSequenceWindow_c*>(storage)); }
void* hdIngameConstructModeHint(void*);
u32 dOw_createScreen(u32);
static void* hdIngameConstructWindGuide(void* storage) { return gabi::at<void>(dOw_createScreen(gabi::ea(storage))); }
void* hdIngameConstructItemControlHints(void*);
void* hdIngameConstructMenuConfirmation(void*);
void* hdIngameConstructMiiversePrompt(void*);
void* hdIngameConstructMenu(void*);
void* hdIngameConstructOverlay(void*);
u32 metroCtor(u32);
static void* hdIngameConstructMetronome(void* storage) { return gabi::at<void>(metroCtor(gabi::ea(storage))); }
static void* hdIngameConstructMenuAuxiliary(void* storage) {
    return hdBatonSignList_c::construct(static_cast<hdBatonSignList_c*>(storage));
}
void* hdIngameConstructModeSwitchScreen(void*);
void* hdIngameConstructModeSwitchEffects(void*);
void* hdIngameConstructPictographScreen(void*);
void* hdIngameConstructNameEntry(void*);
u32 rupy_ctor(RupyScreen*);
static void* hdIngameConstructAuctionRupees(void* storage) { return gabi::at<void>(rupy_ctor(static_cast<RupyScreen*>(storage))); }
void* hdIngameConstructHudCounterB(void*);
static void* hdIngameConstructBoatRaceFailed(void* storage) { return hdScreenBoatRaceFailed_c::construct(static_cast<hdScreenBoatRaceFailed_c*>(storage)); }
static void* hdIngameConstructBoatRaceResult(void* storage) { return hdBoatRaceResultView::construct(static_cast<hdBoatRaceResultView*>(storage)); }
static void* hdIngameConstructBoatRaceStart(void* storage) { return hdScreenBoatRaceStart_c::construct(static_cast<hdScreenBoatRaceStart_c*>(storage)); }
void* hdIngameConstructHudCounterF(void*);
void* hdIngameConstructHudCounterG(void*);
void* hdIngameConstructWarpButtons(void*);
void* hdIngameConstructTouchPanel(void*);
static void* hdIngameConstructMapCoordinator(void* storage) {
    return hdMapDisplayCoordinator_c::construct(static_cast<hdMapDisplayCoordinator_c*>(storage));
}

/* 02712904 */
void hdIngameUi_c::createScreens() {
    mOwnedScreens[0] = createOwnedScreen(0xAC, hdIngameConstructMainHud);
    mOwnedScreens[1] = createOwnedScreen(0x9C, hdIngameConstructItemScreen);
    mOwnedScreens[2] = createOwnedScreen(0x204, hdIngameConstructMiiverse);
    mOwnedScreens[3] = createOwnedScreen(0x64, hdIngameConstructMenuButtons);
    mOwnedScreens[4] = createOwnedScreen(0xE0, hdIngameConstructStatusScreen);
    mOwnedScreens[5] = createOwnedScreen(0xE4, hdIngameConstructMenuEffects);
    mOwnedScreens[6] = createOwnedScreen(0x64, hdIngameConstructAuxiliaryScreen);
    mOwnedScreens[7] = createOwnedScreen(0x5C, hdIngameConstructPlaceName);
    mOwnedScreens[8] = createOwnedScreen(0x50, hdIngameConstructBatonSign);
    mOwnedScreens[9] = createOwnedScreen(0x50, hdIngameConstructBatonEffects);
    mOwnedScreens[10] = createOwnedScreen(0x98, hdIngameConstructMenuMessage);
    mOwnedScreens[11] = createOwnedScreen(0x15C, hdIngameConstructMessageCounter);
    mOwnedScreens[15] = createOwnedScreen(0x1F0, hdIngameConstructMessageDetail);
    mOwnedScreens[12] = createOwnedScreen(0x14C, hdIngameConstructMessageList);
    mOwnedScreens[14] = createOwnedScreen(0xB4, hdIngameConstructMessageChoice);
    mOwnedScreens[13] = createOwnedScreen(0x13C, hdIngameConstructMessageComposite);
    mOwnedScreens[16] = createOwnedScreen(0x84, hdIngameConstructMessageChoiceCursor);
    mOwnedScreens[17] = createOwnedScreen(0xDC, hdIngameConstructHintScreen);
    mOwnedScreens[18] = createOwnedScreen(0x148, hdIngameConstructModeHint);
    mOwnedScreens[19] = createOwnedScreen(0xA4, hdIngameConstructWindGuide);
    mOwnedScreens[20] = createOwnedScreen(0x58, hdIngameConstructItemControlHints);
    mOwnedScreens[21] = createOwnedScreen(0x54, hdIngameConstructMenuConfirmation);
    mOwnedScreens[22] = createOwnedScreen(0x68, hdIngameConstructMiiversePrompt);
    mOwnedScreens[23] = createOwnedScreen(0x110, hdIngameConstructMenu);
    mOwnedScreens[24] = createOwnedScreen(0x894, hdIngameConstructOverlay);
    mOwnedScreens[25] = createOwnedScreen(0x16C, hdIngameConstructMetronome);
    mOwnedScreens[26] = createOwnedScreen(0xA8, hdIngameConstructMenuAuxiliary);
    mOwnedScreens[27] = createOwnedScreen(0x50, hdIngameConstructModeSwitchScreen);
    mOwnedScreens[28] = createOwnedScreen(0x58, hdIngameConstructModeSwitchEffects);
    mOwnedScreens[29] = createOwnedScreen(0xFC, hdIngameConstructPictographScreen);
    mOwnedScreens[30] = createOwnedScreen(0xFC, hdIngameConstructNameEntry);
    mOwnedScreens[31] = createOwnedScreen(0x6C, hdIngameConstructAuctionRupees);
    mOwnedScreens[32] = createOwnedScreen(0x68, hdIngameConstructHudCounterB);
    mOwnedScreens[33] = createOwnedScreen(0x64, hdIngameConstructBoatRaceFailed);
    mOwnedScreens[34] = createOwnedScreen(0x68, hdIngameConstructBoatRaceResult);
    mOwnedScreens[35] = createOwnedScreen(0x54, hdIngameConstructBoatRaceStart);
    mOwnedScreens[36] = createOwnedScreen(0x68, hdIngameConstructHudCounterF);
    mOwnedScreens[37] = createOwnedScreen(0x54, hdIngameConstructHudCounterG);
    mOwnedScreens[38] = createOwnedScreen(0x60, hdIngameConstructWarpButtons);
    mOwnedScreens[39] = createOwnedScreen(0x58, hdIngameConstructTouchPanel);
    mOwnedScreens[40] = createOwnedScreen(0x180, hdIngameConstructMapCoordinator);
    hdCursorMgr_c::createInstance(0);
    hdIngameCreateSharedMenuScreen(0);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdLayerBase_c* tvLayer = &layers[count > 3 ? 3 : 0]->mBase;
    count = renderer->mLayerCount;
    layers = renderer->mpLayers;
    u32 tvFlags = tvLayer->mFlags;
    hdLayerBase_c* drcLayer = &layers[count > 7 ? 7 : 0]->mBase;
    u32 drcFlags = drcLayer->mFlags;
    void* tvProjection = &sDefaultUiProjection;
    if (tvFlags & 0x1000) tvProjection = tvLayer->mpInternalProjection;
    else if (tvLayer->mpProjection.get() != NULL) tvProjection = tvLayer->mpProjection;
    void* drcProjection = &sDefaultUiProjection;
    if (drcFlags & 0x1000) drcProjection = drcLayer->mpInternalProjection;
    else if (drcLayer->mpProjection.get() != NULL) drcProjection = drcLayer->mpProjection;
    void* tvMatrix = &sDefaultUiMatrix;
    if ((tvFlags & 0xC0) == 0xC0) tvMatrix = &tvLayer->mViewMatrix;
    else if (tvLayer->mpViewMatrix.get() != NULL) tvMatrix = tvLayer->mpViewMatrix;
    void* drcMatrix = &sDefaultUiMatrix;
    if ((drcFlags & 0xC0) == 0xC0) drcMatrix = &drcLayer->mViewMatrix;
    else if (drcLayer->mpViewMatrix.get() != NULL) drcMatrix = drcLayer->mpViewMatrix;
    hdIngameLoadMainHud(mOwnedScreens[0], tvProjection, tvMatrix, 0);
    hdIngameLoadStatusScreen(mOwnedScreens[4], tvProjection, tvMatrix, 0);
    hdIngameLoadAuxiliaryScreen(mOwnedScreens[6], tvProjection, tvMatrix, 0);
    hdIngameLoadPlaceName(mOwnedScreens[7], tvProjection, tvMatrix, 0);
    hdIngameLoadBatonSign(mOwnedScreens[8], tvProjection, tvMatrix, 0);
    hdIngameLoadMenuMessage(mOwnedScreens[10], tvProjection, tvMatrix, 0);
    {
        hdParts_c* message = screenParts(mOwnedScreens[11]);
        hdIngameMessageLoadVtable_l* table = reinterpret_cast<hdIngameMessageLoadVtable_l*>(message->mVtable.get());
        gabi::call_ptr(table->mLoadScreen.get(), message, tvProjection, tvMatrix, 0);
    }
    {
        hdParts_c* message = screenParts(mOwnedScreens[15]);
        hdIngameMessageLoadVtable_l* table = reinterpret_cast<hdIngameMessageLoadVtable_l*>(message->mVtable.get());
        gabi::call_ptr(table->mLoadScreen.get(), message, tvProjection, tvMatrix, 0);
    }
    {
        hdParts_c* message = screenParts(mOwnedScreens[12]);
        hdIngameMessageLoadVtable_l* table = reinterpret_cast<hdIngameMessageLoadVtable_l*>(message->mVtable.get());
        gabi::call_ptr(table->mLoadScreen.get(), message, tvProjection, tvMatrix, 0);
    }
    hdIngameLoadMessageChoice(mOwnedScreens[14], tvProjection, tvMatrix, 0);
    hdIngameLoadMessageComposite(mOwnedScreens[13], tvProjection, tvMatrix, 0);
    hdIngameLoadMessageChoiceCursor(mOwnedScreens[16], tvProjection, tvMatrix, 0);
    hdIngameLoadHintScreen(mOwnedScreens[17], tvProjection, tvMatrix, 0);
    hdIngameLoadModeHint(mOwnedScreens[18], tvProjection, tvMatrix, 0);
    hdIngameLoadWindGuide(mOwnedScreens[19], tvProjection, tvMatrix, 0);
    hdIngameLoadItemControlHints(mOwnedScreens[20], tvProjection, tvMatrix, 0);
    hdIngameLoadMenuConfirmation(mOwnedScreens[21], tvProjection, tvMatrix, 0);
    hdIngameLoadMiiversePrompt(mOwnedScreens[22], tvProjection, tvMatrix, 0);
    hdIngameLoadMenu(mOwnedScreens[23], tvProjection, tvMatrix, 0);
    hdIngameLoadOverlay(mOwnedScreens[24], tvProjection, tvMatrix, 0);
    hdIngameLoadMetronome(mOwnedScreens[25], tvProjection, tvMatrix, 0);
    hdIngameLoadMenuAuxiliary(mOwnedScreens[26], tvProjection, tvMatrix, 0);
    hdIngameLoadModeSwitchScreen(mOwnedScreens[27], tvProjection, tvMatrix, 0);
    hdIngameLoadModeSwitchEffects(mOwnedScreens[28], tvProjection, tvMatrix, 0);
    hdIngameLoadPictographScreen(mOwnedScreens[29], tvProjection, tvMatrix, 0);
    hdIngameLoadNameEntry(mOwnedScreens[30], tvProjection, tvMatrix, 0);
    hdIngameLoadAuctionRupees(mOwnedScreens[31], tvProjection, tvMatrix, 0);
    hdIngameLoadHudCounterB(mOwnedScreens[32], tvProjection, tvMatrix, 0);
    hdIngameLoadBoatRaceFailed(mOwnedScreens[33], tvProjection, tvMatrix, 0);
    hdIngameLoadBoatRaceResult(mOwnedScreens[34], tvProjection, tvMatrix, 0);
    hdIngameLoadBoatRaceStart(mOwnedScreens[35], tvProjection, tvMatrix, 0);
    hdIngameLoadHudCounterF(mOwnedScreens[36], tvProjection, tvMatrix, 0);
    hdIngameLoadWarpButtons(mOwnedScreens[38], tvProjection, tvMatrix, 0);
    hdIngameLoadItemScreen(mOwnedScreens[1], drcProjection, drcMatrix, 0);
    hdIngameLoadMiiverse(mOwnedScreens[2], drcProjection, drcMatrix, 0);
    static_cast<hdMenuButtons*>(mOwnedScreens[3].get())->create(drcProjection, drcMatrix, 0);
    hdIngameLoadMenuEffects(mOwnedScreens[5], drcProjection, drcMatrix, 0);
    hdIngameLoadBatonEffects(mOwnedScreens[9], drcProjection, drcMatrix, 0);
    hdIngameLoadTouchPanel(mOwnedScreens[39], drcProjection, drcMatrix, 0);
    (mpMapCoordinator)->setup();
    (sCursorManager)->create(gabi::ea(drcProjection), gabi::ea(drcMatrix));
    sCursorManager->mActive = 0;
    hdIngameLoadSharedMenu(sSharedMenuScreen, drcProjection, drcMatrix, 0);
    hdIngameLoadHudCounterG(mOwnedScreens[37], tvProjection, tvMatrix, 0);
    mTransitionFrames = 0;
    mMenuSelection = 0;
    hdIngameSetItemScreenActive(mpItemScreen, 0);
    hdIngameActivateMenuButton(mpMenuButtons->button(0));
    (mpMenuButtons)->show();
    (sInputEventManager)->setModeDefault(gabi::ea(mpMenuButtons->button(0)->mButton.mpReceiver.get()));
    if ((hdInputGetManager())->isAlternateGamePadMode()) hdParts_changeState(mpBatonSign->stateMachine(), &sBatonDrcState);
}

static void* sSceneActivityManager;
u32 hdIngameSceneUpdatesPaused(void*);

struct hdIngameRenderDelegate_l {
    gptr<void> mpVtable;
    gptr<hdIngameUi_c> mpOwner;
    be<u16> mThisDelta, mVirtualIndex;
    gfn mCallback;
};
WWHD_SIZE(hdIngameRenderDelegate_l, 0x10);
static u32 sRenderDelegateVtable;
struct hdIngameGroupName_l { char mText[1]; };
static hdIngameGroupName_l sHudGroupName;
static hdIngameGroupName_l sInputGroupName;
static hdIngameGroupName_l sPrimaryGroupName;
static hdIngameGroupName_l sOverlayGroupName;
static hdIngameGroupName_l sModeSwitchGroupName;
using hdIngameDrawCallback = void (*)(hdIngameUi_c*, hdLayerRenderInfo_c*);
void hdIngameHudDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*);
void hdIngameInputDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*);
void hdIngamePrimaryDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*);
void hdIngameOverlayDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*);
void hdIngameModeSwitchDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*);
void hdIngameAttachScreenGroup(hdRenderCommandLayer_l*, u32, hdIngameScreenGroup*);
void hdIngameResumeSceneUpdates();
void hdIngameResumeSceneRendering();
void hdIngameResumeSceneInput();
void hdIngameSetSceneTransition(u32);
static void registerScreenGroup(hdIngameUi_c* scene, hdIngameScreenGroup* group,
                                const char* name, hdIngameDrawCallback draw,
                                u32 layerIndex, u32 priority) {
    hdIngameRenderDelegate_l* delegate = gabi::at<hdIngameRenderDelegate_l>(
        gabi::ea(group) + offsetof(hdIngameScreenGroup, mChildren));
    if (delegate != NULL) {
        delegate->mpVtable = &sRenderDelegateVtable;
        delegate->mpOwner = scene;
        delegate->mVirtualIndex = 0xFFFF;
        delegate->mCallback.v = gabi::guest_fn((const void*)draw);
        delegate->mThisDelta = 0;
    }
    group->mCount = 1;
    group->mArchiveName.mText.v = gabi::ea(name);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdIngameAttachScreenGroup(layers[layerIndex < count ? layerIndex : 0], priority, group);
}

/* 02713368 */
void hdIngameUi_c::initializeScreens() {
    if (sMapCoordinatorOnly) {
        sMapCoordinatorOnly = 0;
        resetMenuScreens();
        mMenuRequested = 0;
        hdIngameSetFrameworkRunning(hdIngameGetFramework(), 1);
        bool reopenPictograph = false;
        if (sceneStateMatches(&mState, &l_ingameMenuSubWindowState) ||
            sceneStateMatches(&mState, &l_ingameToSaveState) ||
            sceneStateMatches(&mState, &l_ingameSaveState) ||
            sceneStateMatches(&mState, &l_ingameToOptionState) ||
            sceneStateMatches(&mState, &l_ingameOptionState)) {
            reopenPictograph = sceneStateMatches(&mState, &l_ingameToTaktState);
        } else {
            bool retainedMenu = sceneStateMatches(&mState, &l_ingameToTaktState) ||
                sceneStateMatches(&mState, &l_ingameTaktState) ||
                sceneStateMatches(&mState, &l_ingamePhotoViewMiiverseState) ||
                sceneStateMatches(&mState, &l_ingameMiiverseEmpathingState) ||
                sceneStateMatches(&mState, &l_ingameToMiiverseSettingState) ||
                sceneStateMatches(&mState, &l_ingameMiiverseSettingState) ||
                sceneStateMatches(&mState, &l_ingameToConfirmMiiverseCommentJumpState) ||
                sceneStateMatches(&mState, &l_ingameConfirmMiiverseCommentJumpState) ||
                sceneStateMatches(&mState, &l_ingameToConfirmMiiverseProfileJumpState) ||
                sceneStateMatches(&mState, &l_ingameConfirmMiiverseProfileJumpState) ||
                sceneStateMatches(&mState, &l_ingameToFishManHintState) ||
                sceneStateMatches(&mState, &l_ingameFishManHintState) ||
                sceneStateMatches(&mState, &l_ingameChangeToTvModeState) ||
                sceneStateMatches(&mState, &l_ingameChangeToDrcModeState);
            if (!retainedMenu) {
                hdIngameRefreshMenuSelection(this);
                hdIngameChangeState(&mState, &l_ingameToPauseDrcState);
                return;
            }
            reopenPictograph = sceneStateMatches(&mState, &l_ingameToTaktState);
        }
        if (reopenPictograph) {
            hdIngameResetPictograph(mpPictographScreen);
            sUiMode = 0;
            hdIngameChangeState(&mState, &l_ingameTaktState);
        }
        return;
    }
    hdInputManager* input = hdInputGetManager();
    if (input->mMode == 3) (input)->setMode(1);
    registerScreenGroup(this, &mScreenGroups[0], sHudGroupName.mText, hdIngameHudDrawCallback, 3, 3);
    registerScreenGroup(this, &mScreenGroups[1], sInputGroupName.mText, hdIngameInputDrawCallback, 5, 0);
    registerScreenGroup(this, &mScreenGroups[2], sPrimaryGroupName.mText, hdIngamePrimaryDrawCallback, 7, 1);
    registerScreenGroup(this, &mScreenGroups[3], sOverlayGroupName.mText, hdIngameOverlayDrawCallback, 9, 1);
    registerScreenGroup(this, &mScreenGroups[4], sModeSwitchGroupName.mText, hdIngameModeSwitchDrawCallback, 11, 0);
    createScreens();
    sUiInputSuspended = 0;
    sUiMode = 0;
    hdStateObject* state = mState.mpOwner->createState(reinterpret_cast<hdStateId*>(&l_ingameToPauseDrcState));
    mState.mpCurrentState = state;
    state->enter();
    hdIngameResumeSceneUpdates();
    hdIngameResumeSceneRendering();
    hdIngameResumeSceneInput();
    hdIngameSetSceneTransition((hdInputGetManager())->isAlternateGamePadMode() != 0);
}

/* 02714564 */
void hdIngameUi_c::handleScreenActions() {
    if (hdIngameSceneUpdatesPaused(sSceneActivityManager)) return;
    mpMainHud->vExecuteState();
    mpMenuButtons->vExecuteState();
    screenParts(mpItemScreen)->vExecuteState();
    (mpMapCoordinator)->execute();
    if (!sceneStateMatches(&mState, &l_ingameMiiverseSettingState) &&
        !sceneStateMatches(&mState, &l_ingameToConfirmMiiverseCommentJumpState) &&
        !sceneStateMatches(&mState, &l_ingameConfirmMiiverseCommentJumpState) &&
        !sceneStateMatches(&mState, &l_ingameToConfirmMiiverseProfileJumpState) &&
        !sceneStateMatches(&mState, &l_ingameConfirmMiiverseProfileJumpState)) {
        screenParts(mpSubWindow)->vExecuteState();
    }
    for (u32 i = 31; i < 37; ++i) screenParts(mOwnedScreens[i])->vExecuteState();
    screenParts(mpGameOver)->vExecuteState();
    mpHudIndicator->vExecuteState();
    screenParts(mpNameEntryScreen)->vExecuteState();
    mpHintScreen->vExecuteState();
    mpHintEffects->vExecuteState();
    mpWarpButtons->vExecuteState();
    mpWindGuide->vExecuteState();
    mpControlWindHint->vExecuteState();
    mpItemControlHints->vExecuteState();
    mpMenuConfirmation->vExecuteState();
    mpMiiversePrompt->vExecuteState();
    screenParts(mpMenu)->vExecuteState();
    screenParts(mpOverlay)->vExecuteState();
    mpMetronome->vExecuteState();
    sSharedMenuScreen->vExecuteState();
    screenParts(mOwnedScreens[37])->vExecuteState();
    changeActiveScreen();
    refreshScreenReceivers();
    screenParts(mpMenuMessageScreen)->vExecuteState();
    screenParts(mOwnedScreens[11])->vExecuteState();
    screenParts(mOwnedScreens[15])->vExecuteState();
    if (!sceneStateMatches(&mState, &l_ingameToDemoTriforceState) && !sceneStateMatches(&mState, &l_ingameDemoTriforceState)) {
        screenParts(mOwnedScreens[12])->vExecuteState();
    }
    screenParts(mOwnedScreens[14])->vExecuteState();
    screenParts(mOwnedScreens[13])->vExecuteState();
    mpStatusScreen->vExecuteState();
    mpMenuEffects->vExecuteState();
    mpBatonSign->vExecuteState();
    mpBatonEffects->vExecuteState();
    screenParts(mpMessageChoiceCursorScreen)->vExecuteState();
    (sCursorManager)->execute();
    mpModeSwitchEffects->vExecuteState();
    (sKeyboardManager)->calc();
}

/* 02715178 */
u8 hdIngameUi_c::isMenuMode() {
    u32 mode = sUiMode;
    return mode == 3 || mode == 4;
}

/* 027152B4 */
void hdIngameUi_c::calc() {
    captureModeSwitchButtons();
    hdIngameUpdateState(&mState);
    handleMenuActions();
    updateMenuButtons();
    handleScreenActions();
    handleItemAssignment();
    handleInvalidControls();
}

/* 02715310 */
void hdIngameUi_c::calcSecondaryScreens() {
    screenParts(mpTouchPanel)->vExecuteState();
    screenParts(mpPictographScreen)->vExecuteState();
    mpModeSwitchScreen->vExecuteState();
}


/* 027162CC */
void hdIngameUi_c::drawMenuScreens(void* info) {
    mpHintScreen->vDraw(info);
    mpHintEffects->vDraw(info);
    sSharedMenuScreen->vDraw(info);
    if (sInputOptions->mDirectionalInputEnabled != 0 &&
        !sceneStateMatches(&mState, &l_ingameControlWindState)) {
        bool showManager = true;
        if (sceneStateMatches(&mState, &l_ingameToWarpState) ||
            sceneStateMatches(&mState, &l_ingameWarpState)) {
            showManager = mpWarpButtons->mEnabled != 0;
        }
        if (showManager) (sCursorManager)->draw(info);
    }
    mpMenuEffects->vDraw(info);
}


struct hdIngameCompositeDrawVtable_l {
    u8 mPrefix[0x74];
    gfn mDrawComposite;
};
static void drawSubWindowOverlay(void* screen, void* viewport) {
    hdParts_c* subWindow = screenParts(screen);
    hdIngameSubWindowVtable_l* table = reinterpret_cast<hdIngameSubWindowVtable_l*>(subWindow->mVtable.get());
    gabi::call_ptr(table->mDrawOverlay.get(), subWindow, viewport);
}

/* 02715CA0 */
void hdIngameUi_c::drawHudScreens(hdLayerRenderInfo_c* info) {
    hdIngameSetShaderModeEx(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 index = info->mTarget;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdRenderCommandLayer_l* layer = layers[index < count ? index : 0];
    void* viewport = &layer->mBase.mViewport;
    if (!hdIngameMenuPauseFlag()) {
        mpMainHud->vDraw(viewport);
        for (u32 i = 31; i < 37; ++i) screenParts(mOwnedScreens[i])->vDraw(viewport);
        mpHudIndicator->vDraw(viewport);
        mpControlWindHint->vDraw(viewport);
        if ((hdInputGetManager())->isAlternateGamePadMode()) mpItemControlHints->vDraw(viewport);
        mpMenuConfirmation->vDraw(viewport);
        mpMiiversePrompt->vDraw(viewport);
        screenParts(mpMenu)->vDraw(viewport);
        mpMetronome->vDraw(viewport);
        screenParts(mOwnedScreens[37])->vDraw(viewport);
    }
    if (hdIngameMenuPauseFlag() && mMenuRequested == 0) {
        if (!(hdInputGetManager())->isGamePadMode()) {
            screenParts(mpItemScreen)->vDraw(viewport);
            (mpMapCoordinator)->processInput(gabi::ea(info), gabi::ea(viewport));
            screenParts(mpSubWindow)->vDraw(viewport);
            drawSubWindowOverlay(mpSubWindow, viewport);
            mpMenuButtons->vDraw(viewport);
            drawSubWindowOverlay(mpSubWindow, viewport);
            hdIngameDrawMenuMessage(mpMenuMessageScreen, viewport);
            hdIngameDrawMessageChoiceCursor(mpMessageChoiceCursorScreen, viewport);
        }
        (mpMapCoordinator)->setSeaMapMarkerForward(gabi::ea(info), gabi::ea(viewport));
    }
    if (sceneStateMatches(&mState, &l_ingameDemoFishManState)) (mpMapCoordinator)->processInput(gabi::ea(info), gabi::ea(viewport));
    if (sceneStateMatches(&mState, &l_ingameDemoTriforceState)) screenParts(mpItemScreen)->vDraw(viewport);
    screenParts(mpOverlay)->vDraw(viewport);
    screenParts(mpMenuMessageScreen)->vDraw(viewport);
    screenParts(mOwnedScreens[11])->vDraw(viewport);
    screenParts(mOwnedScreens[15])->vDraw(viewport);
    screenParts(mOwnedScreens[12])->vDraw(viewport);
    screenParts(mOwnedScreens[14])->vDraw(viewport);
    hdParts_c* composite = screenParts(mOwnedScreens[13]);
    hdIngameCompositeDrawVtable_l* table = reinterpret_cast<hdIngameCompositeDrawVtable_l*>(composite->mVtable.get());
    gabi::call_ptr(table->mDrawComposite.get(), composite, info, viewport);
    screenParts(mpMessageChoiceCursorScreen)->vDraw(viewport);
    screenParts(mpGameOver)->vDraw(viewport);
    bool drawStatus = hdIngameMenuPauseFlag() == 0;
    if (!drawStatus && hdIngameMenuPauseFlag()) {
        drawStatus = sceneStateMatches(&mState, &l_ingameToSelectPictographListState) ||
                     sceneStateMatches(&mState, &l_ingameSelectPictographListState);
    }
    if (drawStatus) mpStatusScreen->vDraw(viewport);
}

/* 02716DA4 */
void hdIngameUi_c::drawOverlayScreens(hdLayerRenderInfo_c* info) {
    hdIngameSetShaderModeEx(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 index = info->mTarget;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdRenderCommandLayer_l* layer = layers[index < count ? index : 0];
    void* viewport = &layer->mBase.mViewport;
    if (sUiMode == 4) screenParts(mpPictographScreen)->vDraw(viewport);
    mpBatonEffects->vDraw(viewport);
    if ((hdInputGetManager())->isGamePadMode() && mMenuRequested == 0 && mTouchPanelRequested == 0) {
        screenParts(mpNameEntryScreen)->vDraw(viewport);
    }
    if ((hdInputGetManager())->isGamePadMode() &&
        !sceneStateMatches(&mState, &l_ingameToWarpState) && !sceneStateMatches(&mState, &l_ingameWarpState)) {
        drawMenuScreens(viewport);
    }
    if (sceneStateMatches(&mState, &l_ingameInputPasswordState) && (hdInputGetManager())->isGamePadMode()) {
        (sKeyboardManager)->drawDRC(gabi::ea(viewport));
    }
    screenParts(mpTouchPanel)->vDraw(viewport);
    mpModeSwitchScreen->vDraw(viewport);
    mpModeSwitchEffects->vDraw(viewport);
}



/* 02716AD0 */
void hdIngameUi_c::drawPrimaryScreens(hdLayerRenderInfo_c* info) {
    if (!hdIngameMenuPauseFlag() && (hdInputGetManager())->isAlternateGamePadMode()) return;
    hdIngameSetShaderModeEx(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 index = info->mTarget;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdRenderCommandLayer_l* layer = layers[index < count ? index : 0];
    void* viewport = &layer->mBase.mViewport;
    screenParts(mpItemScreen)->vDraw(viewport);
    (mpMapCoordinator)->processInput(gabi::ea(info), gabi::ea(viewport));
    screenParts(mpSubWindow)->vDraw(viewport);
    mpMenuButtons->vDraw(viewport);
    if (!(hdInputGetManager())->isTvMode()) {
        hdParts_c* subWindow = screenParts(mpSubWindow);
        hdIngameSubWindowVtable_l* table = reinterpret_cast<hdIngameSubWindowVtable_l*>(subWindow->mVtable.get());
        gabi::call_ptr(table->mDrawOverlay.get(), subWindow, viewport);
    }
    (mpMapCoordinator)->setSeaMapMarkerForward(gabi::ea(info), gabi::ea(viewport));
    sSharedMenuScreen->vDraw(viewport);
    if (sInputOptions->mDirectionalInputEnabled != 0 &&
        !sceneStateMatches(&mState, &l_ingameControlWindState) &&
        !sceneStateMatches(&mState, &l_ingameToWarpState) &&
        !sceneStateMatches(&mState, &l_ingameWarpState) && mMenuRequested == 0) {
        (sCursorManager)->draw(viewport);
    }
    mpWindGuide->vDraw(viewport);
    if (!(hdInputGetManager())->isTvMode()) {
        mpControlWindHint->vDraw(viewport);
        mpItemControlHints->vDraw(viewport);
    }
    hdIngameDrawMenuMessage(mpMenuMessageScreen, viewport);
    hdIngameDrawMessageChoiceCursor(mpMessageChoiceCursorScreen, viewport);
}


/* 02716494 */
void hdIngameUi_c::drawInputScreens(hdLayerRenderInfo_c* info) {
    hdIngameSetShaderModeEx(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 index = info->mTarget;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdRenderCommandLayer_l* layer = layers[index < count ? index : 0];
    void* viewport = &layer->mBase.mViewport;
    if ((hdInputGetManager())->isAlternateGamePadMode() ||
        ((hdInputGetManager())->isTvMode() && isTaktSignListActive())) {
        mpWindGuide->vDraw(viewport);
    }
    if ((hdInputGetManager())->isTvMode() && sceneStateMatches(&mState, &l_ingamePhotoViewMiiverseState)) {
        drawSubWindowOverlay(mpSubWindow, viewport);
        hdIngameDrawMenuMessage(mpMenuMessageScreen, viewport);
        hdIngameDrawMessageChoiceCursor(mpMessageChoiceCursorScreen, viewport);
    }
    if (sUiMode == 2) screenParts(mpPictographScreen)->vDraw(viewport);
    bool showName = false;
    if (mMenuRequested == 0) {
        if ((hdInputGetManager())->isGamePadMode()) {
            showName = sceneStateMatches(&mState, &l_ingameInputPasswordState) || sceneStateMatches(&mState, &l_ingameWarpState);
        } else {
            showName = hdIngameMenuPauseFlag() != 0;
        }
    }
    if (showName) screenParts(mpNameEntryScreen)->vDraw(viewport);
    if (sceneStateMatches(&mState, &l_ingameToWarpState) || sceneStateMatches(&mState, &l_ingameWarpState)) mpWarpButtons->vDraw(viewport);
    bool showMenus = false;
    if (!(hdInputGetManager())->isGamePadMode() && hdIngameMenuPauseFlag()) {
        showMenus = true;
    } else if (sceneStateMatches(&mState, &l_ingameToWarpState) || sceneStateMatches(&mState, &l_ingameWarpState)) {
        showMenus = true;
    } else {
        if ((hdInputGetManager())->isTvMode()) {
            showMenus = sceneStateMatches(&mState, &l_ingamePhotoViewMiiverseState) ||
                sceneStateMatches(&mState, &l_ingameToConfirmMiiverseCommentJumpState) ||
                sceneStateMatches(&mState, &l_ingameConfirmMiiverseCommentJumpState) ||
                sceneStateMatches(&mState, &l_ingameToConfirmMiiverseProfileJumpState) ||
                sceneStateMatches(&mState, &l_ingameConfirmMiiverseProfileJumpState);
        }
        if (!showMenus) showMenus = mMenuRequested != 0;
    }
    if (showMenus) drawMenuScreens(viewport);
    if (sceneStateMatches(&mState, &l_ingameInputPasswordState)) {
        if ((hdInputGetManager())->isGamePadMode()) (sKeyboardManager)->drawLayouts(gabi::ea(viewport));
        else if ((hdInputGetManager())->isAlternateGamePadMode()) (sKeyboardManager)->drawDRC(gabi::ea(viewport));
        else if ((hdInputGetManager())->isTvMode()) (sKeyboardManager)->drawTV(gabi::ea(viewport));
    }
    mpModeSwitchScreen->vDraw(viewport);
}

/* 02716FFC */
void hdIngameUi_c::drawModeSwitchScreens(hdLayerRenderInfo_c* info) {
    hdIngameSetShaderModeEx(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    hdRenderSceneMgr_l* renderer = sRenderManager;
    u32 index = info->mTarget;
    u32 count = renderer->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = renderer->mpLayers;
    hdRenderCommandLayer_l* layer = layers[index < count ? index : 0];
    void* viewport = &layer->mBase.mViewport;
    mpBatonSign->vDraw(viewport);
    mpModeSwitchScreen->vDraw(viewport);
}

/* 027161BC */
u8 hdIngameUi_c::isTaktSignListActive() {
    hdIngameUi_c* scene = sIngameUi;
    if (scene == NULL) return 0;
    if (sceneStateMatches(&scene->mState, &l_ingameToTaktSignListState)) return 1;
    return sceneStateMatches(&sIngameUi->mState, &l_ingameTaktSignListState);
}

/* 02717120 */
u8 hdIngameUi_c::isPictographListActive() {
    hdIngameUi_c* scene = sIngameUi;
    if (scene == NULL) return 0;
    if (sceneStateMatches(&scene->mState, &l_ingameToPictographListState)) return 1;
    if (sceneStateMatches(&sIngameUi->mState, &l_ingamePictographListState)) return 1;
    if (sceneStateMatches(&sIngameUi->mState, &l_ingameToSelectPictographListState)) return 1;
    return sceneStateMatches(&sIngameUi->mState, &l_ingameSelectPictographListState);
}

/* 027170AC */
void hdIngameUi_c::requestButtonRefresh(u32 index) {
    // PPC variable shifts use six index bits and produce zero for shifts above 31.
    u32 shift = index & 63;
    u8 mask = shift < 32 ? u32(1) << shift : 0;
    if (mRefreshableButtons & mask) {
        mPendingRefreshButtons = mPendingRefreshButtons | mask;
    }
}

/* 027170D4 */
u32 hdIngameUi_c::requestGameOver() {
    hdGameOver_c* screen = mpGameOver;
    if (screen == NULL) return 0;
    screen->mRequested = 1;
    return 1;
}

/* 027170F8 */
u8 hdIngameUi_c::isGameOverFinished() {
    hdGameOver_c* screen = mpGameOver;
    return screen == NULL ? 0 : u8(screen->mFinished);
}

/* 02717114 */
u8 hdIngameUi_c::isCameraShutterVisible() {
    return mpMenu->mShutterVisible;
}

/* 027172C8 */
void hdIngameUi_c::clearPictographTexture(s32 index) {
    mpOverlay->clearTextureImage(index);
}

/* 027172D0 */
u32 hdIngameUi_c::isUiModeActive() {
    return sUiMode != 0;
}

static hdStateMachine* currentSceneMachine() {
    return gabi::at<hdStateMachine>(gabi::ea(sIngameUi) + offsetof(hdIngameUi_c, mState));
}

/* 027172E4 */
u8 hdIngameUi_c::isModalUiActive() {
    u32 mode = sUiMode;
    if (mode == 1 || mode == 2) return 1;
    if ((hdInputGetManager())->isTvMode() && hdIngameMenuPauseFlag()) return 1;
    if (sIngameUi != NULL) {
        if (sceneStateMatches(currentSceneMachine(), &l_ingameExchangeProModeState)) return 1;
        if (sIngameUi != NULL) {
            if (sceneStateMatches(currentSceneMachine(), &l_ingameToDemoFishManState) ||
                sceneStateMatches(currentSceneMachine(), &l_ingameDemoFishManState)) return 1;
            if (sIngameUi != NULL) {
                if (sceneStateMatches(currentSceneMachine(), &l_ingameToDemoTriforceState) ||
                    sceneStateMatches(currentSceneMachine(), &l_ingameDemoTriforceState)) return 1;
                if (sIngameUi != NULL && sceneStateMatches(currentSceneMachine(), &l_ingameWarpState)) return 1;
            }
        }
    }
    if ((hdInputGetManager())->isTvMode() &&
        sceneStateMatches(currentSceneMachine(), &l_ingameToMenuState)) return 1;
    if ((hdInputGetManager())->isTvMode()) {
        if (isTaktSignListActive() || sceneStateMatches(currentSceneMachine(), &l_ingameReturnTaktForProModeState)) return 1;
    }
    if (sceneStateMatches(currentSceneMachine(), &l_ingamePhotoViewMiiverseState)) return 1;
    if ((hdInputGetManager())->isTvMode() &&
        sceneStateMatches(currentSceneMachine(), &l_ingameMiiverseEmpathingState)) return 1;
    return 0;
}

/* 027176A4 */
static hdIngameUi_c* constructIngameUi(hdIngameUi_c* scene) {
    if (scene == NULL) {
        scene = static_cast<hdIngameUi_c*>(operator_new(sizeof(hdIngameUi_c)));
        if (scene == NULL) return NULL;
    }
    scene->mpSelf = scene;
    scene->mReservedBase = 0;
    scene->mpStateVtable = &sSceneInterfaceVtable;
    scene->mpOwnerVtable = &sSceneOwnerVtable;
    scene->mpVtable = &sBaseSceneVtable;
    hdIngameConstructState(&scene->mState, scene, &sNullSceneState);
    scene->mpVtable = &sSceneVtable;
    for (u32 i = 0; i < 5; ++i) {
        hdIngameScreenGroup* group = gabi::at<hdIngameScreenGroup>(gabi::ea(scene) + offsetof(hdIngameUi_c, mScreenGroups) + i * sizeof(hdIngameScreenGroup));
        constructScreenGroup(group);
    }
    for (u32 i = 0; i < 42; ++i) scene->mOwnedScreens[i] = NULL;
    scene->mPauseStartedDuringDemo = 0;
    scene->mMenuSelection = 0;
    scene->mMenuRequested = 0;
    scene->mTransitionFrames = 0;
    scene->mTransitionPhase = 10;
    scene->mTouchPanelRequested = 0;
    scene->mModeSwitchFrames = 0;
    scene->mPausePhase = 5;
    scene->mModeSwitchButtonsHeld = 0;
    scene->mModeSwitchPhase = 0;
    scene->mUiLocked = 0;
    constructSceneFlag(gabi::at<be<u8>>(gabi::ea(scene) + offsetof(hdIngameUi_c, mRefreshableButtons)));
    constructSceneFlag(gabi::at<be<u8>>(gabi::ea(scene) + offsetof(hdIngameUi_c, mPendingRefreshButtons)));
    scene->mTransitionComplete = 0;
    scene->mModeExchangePhase = 0;
    scene->mModeExchangeRequested = 0;
    scene->mReturnMenuRequested = 0;
    return scene;
}

/* 02717AD0 */
static hdIngameUi_c* createIngameUi(void* heap) {
    if (sIngameUi == NULL) {
        hdIngameUi_c* scene = static_cast<hdIngameUi_c*>(hdIngameAllocate(0x24C, heap, 4));
        hdIngameDisposer* disposer = gabi::at<hdIngameDisposer>(gabi::ea(scene) + offsetof(hdIngameUi_c, mDisposer));
        if (disposer != NULL) {
            hdIngameConstructDisposer(disposer, heap, 3);
            disposer->mpVtable = &sIngameDisposerVtable;
        }
        sIngameDisposer = disposer;
        if (scene != NULL) scene = constructIngameUi(scene);
        sIngameUi = scene;
    }
    return sIngameUi;
}

/* 02717B70 */
static void destroyIngameUiInstance() {
    hdIngameDisposer* disposer = sIngameDisposer;
    if (disposer != NULL) {
        sIngameDisposer = NULL;
        gabi::call_ptr(disposer->mpVtable->mDestroy.get(), disposer, 2);
        hdIngameUi_c* scene = sIngameUi;
        if (scene != NULL) {
            gabi::call_ptr(scene->mpVtable->mDestroy.get(), scene, 3);
        }
        sIngameUi = NULL;
    }
}

/* 02717BF0 */
static void destroyIngameUi(hdIngameUi_c* scene, u32 flags) {
    if (scene != NULL) {
        hdIngameDestroyScreenGroup(&scene->mScreenGroups[4], 2);
        hdIngameDestroyScreenGroup(&scene->mScreenGroups[3], 2);
        hdIngameDestroyScreenGroup(&scene->mScreenGroups[2], 2);
        hdIngameDestroyScreenGroup(&scene->mScreenGroups[1], 2);
        hdIngameDestroyScreenGroup(&scene->mScreenGroups[0], 2);
        scene->mState.mpOwner->onStateChange(&scene->mState.mpPreviousState);
        scene->mState.mpOwner->onStateChange(&scene->mState.mpCurrentState);
        if (flags & 1) operator_delete(scene);
    }
}

/* 02717CAC */
static void destroyIngameUiDisposer(hdIngameDisposer* disposer, u32 flags) {
    if (disposer != NULL) {
        disposer->mpVtable = &sIngameDisposerVtable;
        if (disposer == sIngameDisposer) {
            hdIngameUi_c* scene = sIngameUi;
            sIngameDisposer = NULL;
            gabi::call_ptr(scene->mpVtable->mDestroy.get(), scene, 2);
            sIngameUi = NULL;
        }
        hdIngameDestroyDisposerBase(disposer, 0);
        if (flags & 1) operator_delete(disposer);
    }
}

/* 02717D54 */
static void initializeIngameUiStatics() {
    sHeaderStatics.mZeros[3] = 0;
    sHeaderStatics.mZeros[2] = 0;
    sHeaderStatics.mZeros[1] = 0;
    sHeaderStatics.mZeros[0] = 0;
    hdIngameRegisterGlobal(&sHeaderRegistrationFirst);
    f32 minAngle = sInitMinAngle;
    f32 maxAngle = sInitMaxAngle;
    sHeaderStatics.mMinAngle = minAngle;
    sHeaderStatics.mMaxAngle = maxAngle;
    hdIngameConstructFirstFlag(&sHeaderStatics.mFirstFlag);
    hdIngameRegisterGlobal(&sHeaderRegistrationSecond);
    hdIngameConstructSecondFlag(&sHeaderStatics.mSecondFlag);
    hdIngameRegisterGlobal(&sHeaderRegistrationThird);
    f32 nearValue = sInitNear;
    u32 guard = sSceneStateGuard;
    f32 farValue = sInitFar;
    sHeaderStatics.mNearFirst = nearValue;
    sHeaderStatics.mFarSecond = farValue;
    sHeaderStatics.mNearSecond = nearValue;
    sHeaderStatics.mFarFirst = farValue;
    u32 number;
    if (guard != 0) {
        number = sSceneStateCounter;
    } else {
        number = 0;
        sSceneStateGuard = 1;
    }
    initializeSceneState(&l_ingameToPlayState, ++number, "::StateID_ToPlay", ingameToPlayEnter, ingameToPlayExecute, ingameToPlayExit, ingameToPlayComplete);
    initializeSceneState(&l_ingamePlayState, ++number, "::StateID_Play", ingamePlayEnter, ingamePlayExecute, ingamePlayExit, ingamePlayComplete);
    initializeSceneState(&l_ingameToPauseDrcState, ++number, "::StateID_ToPauseDrc", ingameToPauseDrcEnter, ingameToPauseDrcExecute, ingameToPauseDrcExit, ingameToPauseDrcComplete);
    initializeSceneState(&l_ingamePauseDrcState, ++number, "::StateID_PauseDrc", ingamePauseDrcEnter, ingamePauseDrcExecute, ingamePauseDrcExit, ingamePauseDrcComplete);
    initializeSceneState(&l_ingameToMenuState, ++number, "::StateID_ToMenu", ingameToMenuEnter, ingameToMenuExecute, ingameToMenuExit, ingameToMenuComplete);
    initializeSceneState(&l_ingameMenuState, ++number, "::StateID_Menu", ingameMenuEnter, ingameMenuExecute, ingameMenuExit, ingameMenuComplete);
    initializeSceneState(&l_ingameMenuSubWindowState, ++number, "::StateID_MenuSubWindow", ingameMenuSubWindowEnter, ingameMenuSubWindowExecute, ingameMenuSubWindowExit, ingameMenuSubWindowComplete);
    initializeSceneState(&l_ingameToSaveState, ++number, "::StateID_ToSave", ingameToSaveEnter, ingameToSaveExecute, ingameToSaveExit, ingameToSaveComplete);
    initializeSceneState(&l_ingameSaveState, ++number, "::StateID_Save", ingameSaveEnter, ingameSaveExecute, ingameSaveExit, ingameSaveComplete);
    initializeSceneState(&l_ingameToOptionState, ++number, "::StateID_ToOption", ingameToOptionEnter, ingameToOptionExecute, ingameToOptionExit, ingameToOptionComplete);
    initializeSceneState(&l_ingameOptionState, ++number, "::StateID_Option", ingameOptionEnter, ingameOptionExecute, ingameOptionExit, ingameOptionComplete);
    initializeSceneState(&l_ingameToTaktState, ++number, "::StateID_ToTakt", ingameToTaktEnter, ingameToTaktExecute, ingameToTaktExit, ingameToTaktComplete);
    initializeSceneState(&l_ingameTaktState, ++number, "::StateID_Takt", ingameTaktEnter, ingameTaktExecute, ingameTaktExit, ingameTaktComplete);
    initializeSceneState(&l_ingameToTaktSignListState, ++number, "::StateID_ToTaktSignList", ingameToTaktSignListEnter, ingameToTaktSignListExecute, ingameToTaktSignListExit, ingameToTaktSignListComplete);
    initializeSceneState(&l_ingameTaktSignListState, ++number, "::StateID_TaktSignList", ingameTaktSignListEnter, ingameTaktSignListExecute, ingameTaktSignListExit, ingameTaktSignListComplete);
    initializeSceneState(&l_ingameReturnTaktForProModeState, ++number, "::StateID_ReturnTaktForProMode", ingameReturnTaktForProModeEnter, ingameReturnTaktForProModeExecute, ingameReturnTaktForProModeExit, ingameReturnTaktForProModeComplete);
    initializeSceneState(&l_ingameTaktPlayDemoState, ++number, "::StateID_TaktPlayDemo", ingameTaktPlayDemoEnter, ingameTaktPlayDemoExecute, ingameTaktPlayDemoExit, ingameTaktPlayDemoComplete);
    initializeSceneState(&l_ingameControlWindState, ++number, "::StateID_ControlWind", ingameControlWindEnter, ingameControlWindExecute, ingameControlWindExit, ingameControlWindComplete);
    initializeSceneState(&l_ingameToWarpState, ++number, "::StateID_ToWarp", ingameToWarpEnter, ingameToWarpExecute, ingameToWarpExit, ingameToWarpComplete);
    initializeSceneState(&l_ingameWarpState, ++number, "::StateID_Warp", ingameWarpEnter, ingameWarpExecute, ingameWarpExit, ingameWarpComplete);
    initializeSceneState(&l_ingamePhotoViewMiiverseState, ++number, "::StateID_PhotoViewMiiverse", ingamePhotoViewMiiverseEnter, ingamePhotoViewMiiverseExecute, ingamePhotoViewMiiverseExit, ingamePhotoViewMiiverseComplete);
    initializeSceneState(&l_ingameMiiverseEmpathingState, ++number, "::StateID_MiiverseEmpathing", ingameMiiverseEmpathingEnter, ingameMiiverseEmpathingExecute, ingameMiiverseEmpathingExit, ingameMiiverseEmpathingComplete);
    initializeSceneState(&l_ingameToMiiverseSettingState, ++number, "::StateID_ToMiiverseSetting", ingameToMiiverseSettingEnter, ingameToMiiverseSettingExecute, ingameToMiiverseSettingExit, ingameToMiiverseSettingComplete);
    initializeSceneState(&l_ingameMiiverseSettingState, ++number, "::StateID_MiiverseSetting", ingameMiiverseSettingEnter, ingameMiiverseSettingExecute, ingameMiiverseSettingExit, ingameMiiverseSettingComplete);
    initializeSceneState(&l_ingameToConfirmMiiverseCommentJumpState, ++number, "::StateID_ToConfirmMiiverseCommentJump", ingameToConfirmMiiverseCommentJumpEnter, ingameToConfirmMiiverseCommentJumpExecute, ingameToConfirmMiiverseCommentJumpExit, ingameToConfirmMiiverseCommentJumpComplete);
    initializeSceneState(&l_ingameConfirmMiiverseCommentJumpState, ++number, "::StateID_ConfirmMiiverseCommentJump", ingameConfirmMiiverseCommentJumpEnter, ingameConfirmMiiverseCommentJumpExecute, ingameConfirmMiiverseCommentJumpExit, ingameConfirmMiiverseCommentJumpComplete);
    initializeSceneState(&l_ingameToConfirmMiiverseProfileJumpState, ++number, "::StateID_ToConfirmMiiverseProfileJump", ingameToConfirmMiiverseProfileJumpEnter, ingameToConfirmMiiverseProfileJumpExecute, ingameToConfirmMiiverseProfileJumpExit, ingameToConfirmMiiverseProfileJumpComplete);
    initializeSceneState(&l_ingameConfirmMiiverseProfileJumpState, ++number, "::StateID_ConfirmMiiverseProfileJump", ingameConfirmMiiverseProfileJumpEnter, ingameConfirmMiiverseProfileJumpExecute, ingameConfirmMiiverseProfileJumpExit, ingameConfirmMiiverseProfileJumpComplete);
    initializeSceneState(&l_ingameMiiverseProfileJumpState, ++number, "::StateID_MiiverseProfileJump", ingameMiiverseProfileJumpEnter, ingameMiiverseProfileJumpExecute, ingameMiiverseProfileJumpExit, ingameMiiverseProfileJumpComplete);
    initializeSceneState(&l_ingameToMiiverseExplainState, ++number, "::StateID_ToMiiverseExplain", ingameToMiiverseExplainEnter, ingameToMiiverseExplainExecute, ingameToMiiverseExplainExit, ingameToMiiverseExplainComplete);
    initializeSceneState(&l_ingameMiiverseExplainState, ++number, "::StateID_MiiverseExplain", ingameMiiverseExplainEnter, ingameMiiverseExplainExecute, ingameMiiverseExplainExit, ingameMiiverseExplainComplete);
    initializeSceneState(&l_ingameToTelescopeState, ++number, "::StateID_ToTelescope", ingameToTelescopeEnter, ingameToTelescopeExecute, ingameToTelescopeExit, ingameToTelescopeComplete);
    initializeSceneState(&l_ingameTelescopeState, ++number, "::StateID_Telescope", ingameTelescopeEnter, ingameTelescopeExecute, ingameTelescopeExit, ingameTelescopeComplete);
    initializeSceneState(&l_ingameToPictographBoxState, ++number, "::StateID_ToPictographBox", ingameToPictographBoxEnter, ingameToPictographBoxExecute, ingameToPictographBoxExit, ingameToPictographBoxComplete);
    initializeSceneState(&l_ingamePictographBoxState, ++number, "::StateID_PictographBox", ingamePictographBoxEnter, ingamePictographBoxExecute, ingamePictographBoxExit, ingamePictographBoxComplete);
    initializeSceneState(&l_ingameToPictographListState, ++number, "::StateID_ToPictographList", ingameToPictographListEnter, ingameToPictographListExecute, ingameToPictographListExit, ingameToPictographListComplete);
    initializeSceneState(&l_ingamePictographListState, ++number, "::StateID_PictographList", ingamePictographListEnter, ingamePictographListExecute, ingamePictographListExit, ingamePictographListComplete);
    initializeSceneState(&l_ingameReturnPictographBoxState, ++number, "::StateID_ReturnPictographBox", ingameReturnPictographBoxEnter, ingameReturnPictographBoxExecute, ingameReturnPictographBoxExit, ingameReturnPictographBoxComplete);
    initializeSceneState(&l_ingameExitPictographListState, ++number, "::StateID_ExitPictographList", ingameExitPictographListEnter, ingameExitPictographListExecute, ingameExitPictographListExit, ingameExitPictographListComplete);
    initializeSceneState(&l_ingameToSelectPictographListState, ++number, "::StateID_ToSelectPictographList", ingameToSelectPictographListEnter, ingameToSelectPictographListExecute, ingameToSelectPictographListExit, ingameToSelectPictographListComplete);
    initializeSceneState(&l_ingameSelectPictographListState, ++number, "::StateID_SelectPictographList", ingameSelectPictographListEnter, ingameSelectPictographListExecute, ingameSelectPictographListExit, ingameSelectPictographListComplete);
    initializeSceneState(&l_ingameToFishManHintState, ++number, "::StateID_ToFishManHint", ingameToFishManHintEnter, ingameToFishManHintExecute, ingameToFishManHintExit, ingameToFishManHintComplete);
    initializeSceneState(&l_ingameFishManHintState, ++number, "::StateID_FishManHint", ingameFishManHintEnter, ingameFishManHintExecute, ingameFishManHintExit, ingameFishManHintComplete);
    initializeSceneState(&l_ingameInputPasswordState, ++number, "::StateID_InputPassword", ingameInputPasswordEnter, ingameInputPasswordExecute, ingameInputPasswordExit, ingameInputPasswordComplete);
    initializeSceneState(&l_ingameDemoTauraArrivalState, ++number, "::StateID_DemoTauraArrival", ingameDemoTauraArrivalEnter, ingameDemoTauraArrivalExecute, ingameDemoTauraArrivalExit, ingameDemoTauraArrivalComplete);
    initializeSceneState(&l_ingameToDemoFishManState, ++number, "::StateID_ToDemoFishMan", ingameToDemoFishManEnter, ingameToDemoFishManExecute, ingameToDemoFishManExit, ingameToDemoFishManComplete);
    initializeSceneState(&l_ingameDemoFishManState, ++number, "::StateID_DemoFishMan", ingameDemoFishManEnter, ingameDemoFishManExecute, ingameDemoFishManExit, ingameDemoFishManComplete);
    initializeSceneState(&l_ingameToDemoTriforceState, ++number, "::StateID_ToDemoTriforce", ingameToDemoTriforceEnter, ingameToDemoTriforceExecute, ingameToDemoTriforceExit, ingameToDemoTriforceComplete);
    initializeSceneState(&l_ingameDemoTriforceState, ++number, "::StateID_DemoTriforce", ingameDemoTriforceEnter, ingameDemoTriforceExecute, ingameDemoTriforceExit, ingameDemoTriforceComplete);
    initializeSceneState(&l_ingameDemoPictBottleState, ++number, "::StateID_DemoPictBottle", ingameDemoPictBottleEnter, ingameDemoPictBottleExecute, ingameDemoPictBottleExit, ingameDemoPictBottleComplete);
    initializeSceneState(&l_ingameGameOverState, ++number, "::StateID_GameOver", ingameGameOverEnter, ingameGameOverExecute, ingameGameOverExit, ingameGameOverComplete);
    initializeSceneState(&l_ingameChangeToTvModeState, ++number, "::StateID_ChangeToTvMode", ingameChangeToTvModeEnter, ingameChangeToTvModeExecute, ingameChangeToTvModeExit, ingameChangeToTvModeComplete);
    initializeSceneState(&l_ingameChangeToDrcModeState, ++number, "::StateID_ChangeToDrcMode", ingameChangeToDrcModeEnter, ingameChangeToDrcModeExecute, ingameChangeToDrcModeExit, ingameChangeToDrcModeComplete);
    initializeSceneState(&l_ingameExchangeProModeState, ++number, "::StateID_ExchangeProMode", ingameExchangeProModeEnter, ingameExchangeProModeExecute, ingameExchangeProModeExit, ingameExchangeProModeComplete);
    sSceneStateCounter = number;
}

/* WWHD glue
 data sOverlayDisplayState 10499D74
 data sOverlayReturnState 10499F84
 data sMenuListState 104996F0
 data sHintIdleState 1049DD60
 data sUiInputSuspended 101F8351
 str 1013F0D4 in 02710EE0
 ext 0270D664 u32 hdIngameIsDrcMode(hdIngameUi_c*)
 ext 02030CD0 void hdIngameSuspendSceneUpdates()
 ext 026DBD64 void hdIngameSetHintSelection(hdSequenceWindow_c*, u32)
 ext 02030CAC u32 hdIngameSceneSoundActive()
 ext 02030B38 void hdIngameSetSceneSound(const char*)
 ext 02030C34 void hdIngamePlaySceneSound()
 ext 0270E844 void hdIngameCompleteHint(hdIngameUi_c*)
 data sSceneOwnerVtable 1013EF58
 data sSceneInterfaceVtable 1013EF18
 data sSceneVtable 1013F6E8
 data sBaseSceneVtable 1013EF08
 data sGroupDisposerVtable 10169E3C
 data sPointerVectorBaseVtable 1013EEA8
 data sPointerVectorVtable 1013EEF0
 data sGroupStringVtable 1013EEC0
 data sNullSceneState 101FF32C
 str 1013F160 in 027176A4
 ext 020062E8 void hdIngameConstructState(hdStateMachine*, hdIngameUi_c*, hdStateId*)
 ext 02752A84 void hdIngameConstructGroupDisposer(hdIngameDisposer*)
 data sHeaderStatics 1049EBA8
 data sHeaderRegistrationFirst 101F8320
 data sHeaderRegistrationSecond 101F832C
 data sHeaderRegistrationThird 101F8338
 data sSceneStateVtable 1013EFD0
 data sSceneStateGuard 101FD8E4
 data sSceneStateCounter 101FDD50
 data sInitMinAngle 1013F164
 data sInitMaxAngle 1013F168
 data sInitNear 1013F16C
 data sInitFar 1013F170
 ext 028F026C void hdIngameRegisterGlobal(hdIngameGlobalRegistration*)
 ext 028ED6F8 void hdIngameConstructFirstFlag(be<u8>*)
 ext 028EAB2C void hdIngameConstructSecondFlag(be<u8>*)
 data l_ingameToPlayState 1049EBD4
 ext 0270D208 void ingameToPlayEnter(hdIngameUi_c*) callback
 ext 0270D2A4 void ingameToPlayExecute(hdIngameUi_c*) callback
 ext 027195F4 void ingameToPlayExit(hdIngameUi_c*) callback
 ext 027195F8 BOOL ingameToPlayComplete(hdIngameUi_c*) callback
 str 1013F39C in 02717D54
 data l_ingamePlayState 1049EC00
 ext 0270D400 void ingamePlayEnter(hdIngameUi_c*) callback
 ext 0270D70C void ingamePlayExecute(hdIngameUi_c*) callback
 ext 02719600 void ingamePlayExit(hdIngameUi_c*) callback
 ext 02719604 BOOL ingamePlayComplete(hdIngameUi_c*) callback
 str 1013F3B0 in 02717D54
 ext 0270DB88 void ingameToPauseDrcEnter(hdIngameUi_c*) callback
 ext 0270DC00 void ingameToPauseDrcExecute(hdIngameUi_c*) callback
 ext 0271960C void ingameToPauseDrcExit(hdIngameUi_c*) callback
 ext 02719610 BOOL ingameToPauseDrcComplete(hdIngameUi_c*) callback
 str 1013F3C0 in 02717D54
 data l_ingamePauseDrcState 1049EC58
 ext 0270DC10 void ingamePauseDrcEnter(hdIngameUi_c*) callback
 ext 0270E008 void ingamePauseDrcExecute(hdIngameUi_c*) callback
 ext 0270E314 void ingamePauseDrcExit(hdIngameUi_c*) callback
 ext 02719618 BOOL ingamePauseDrcComplete(hdIngameUi_c*) callback
 str 1013F3D8 in 02717D54
 data l_ingameToMenuState 1049EC84
 ext 0270E384 void ingameToMenuEnter(hdIngameUi_c*) callback
 ext 0270E3E8 void ingameToMenuExecute(hdIngameUi_c*) callback
 ext 02719620 void ingameToMenuExit(hdIngameUi_c*) callback
 ext 02719624 BOOL ingameToMenuComplete(hdIngameUi_c*) callback
 str 1013F3EC in 02717D54
 data l_ingameMenuState 1049ECB0
 ext 0270E470 void ingameMenuEnter(hdIngameUi_c*) callback
 ext 0270E4E4 void ingameMenuExecute(hdIngameUi_c*) callback
 ext 0271962C void ingameMenuExit(hdIngameUi_c*) callback
 ext 02719630 BOOL ingameMenuComplete(hdIngameUi_c*) callback
 str 1013F400 in 02717D54
 data l_ingameMenuSubWindowState 1049ECDC
 ext 0270E840 void ingameMenuSubWindowEnter(hdIngameUi_c*) callback
 ext 0270E920 void ingameMenuSubWindowExecute(hdIngameUi_c*) callback
 ext 02719638 void ingameMenuSubWindowExit(hdIngameUi_c*) callback
 ext 0271963C BOOL ingameMenuSubWindowComplete(hdIngameUi_c*) callback
 str 1013F174 in 02717D54
 data l_ingameToSaveState 1049ED08
 ext 0270E9DC void ingameToSaveEnter(hdIngameUi_c*) callback
 ext 0270EA50 void ingameToSaveExecute(hdIngameUi_c*) callback
 ext 02719644 void ingameToSaveExit(hdIngameUi_c*) callback
 ext 02719648 BOOL ingameToSaveComplete(hdIngameUi_c*) callback
 str 1013F410 in 02717D54
 data l_ingameSaveState 1049ED34
 ext 0270EACC void ingameSaveEnter(hdIngameUi_c*) callback
 ext 0270EAD0 void ingameSaveExecute(hdIngameUi_c*) callback
 ext 0270EB88 void ingameSaveExit(hdIngameUi_c*) callback
 ext 02719650 BOOL ingameSaveComplete(hdIngameUi_c*) callback
 str 1013F424 in 02717D54
 data l_ingameToOptionState 1049ED60
 ext 0270EB9C void ingameToOptionEnter(hdIngameUi_c*) callback
 ext 0270EBA0 void ingameToOptionExecute(hdIngameUi_c*) callback
 ext 02719658 void ingameToOptionExit(hdIngameUi_c*) callback
 ext 0271965C BOOL ingameToOptionComplete(hdIngameUi_c*) callback
 str 1013F434 in 02717D54
 data l_ingameOptionState 1049ED8C
 ext 0270EC0C void ingameOptionEnter(hdIngameUi_c*) callback
 ext 0270EC10 void ingameOptionExecute(hdIngameUi_c*) callback
 ext 02719664 void ingameOptionExit(hdIngameUi_c*) callback
 ext 02719668 BOOL ingameOptionComplete(hdIngameUi_c*) callback
 str 1013F448 in 02717D54
 data l_ingameToTaktState 1049EDB8
 ext 0270ECF0 void ingameToTaktEnter(hdIngameUi_c*) callback
 ext 0270EF74 void ingameToTaktExecute(hdIngameUi_c*) callback
 ext 02719670 void ingameToTaktExit(hdIngameUi_c*) callback
 ext 02719674 BOOL ingameToTaktComplete(hdIngameUi_c*) callback
 str 1013F45C in 02717D54
 data l_ingameTaktState 1049EDE4
 ext 0270F014 void ingameTaktEnter(hdIngameUi_c*) callback
 ext 0270F01C void ingameTaktExecute(hdIngameUi_c*) callback
 ext 0271967C void ingameTaktExit(hdIngameUi_c*) callback
 ext 02719680 BOOL ingameTaktComplete(hdIngameUi_c*) callback
 str 1013F470 in 02717D54
 ext 0270F210 void ingameToTaktSignListEnter(hdIngameUi_c*) callback
 ext 0270F244 void ingameToTaktSignListExecute(hdIngameUi_c*) callback
 ext 02719688 void ingameToTaktSignListExit(hdIngameUi_c*) callback
 ext 0271968C BOOL ingameToTaktSignListComplete(hdIngameUi_c*) callback
 str 1013F480 in 02717D54
 ext 0270F28C void ingameTaktSignListEnter(hdIngameUi_c*) callback
 ext 0270F290 void ingameTaktSignListExecute(hdIngameUi_c*) callback
 ext 02719694 void ingameTaktSignListExit(hdIngameUi_c*) callback
 ext 02719698 BOOL ingameTaktSignListComplete(hdIngameUi_c*) callback
 str 1013F49C in 02717D54
 data l_ingameReturnTaktForProModeState 1049EE68
 ext 0270F378 void ingameReturnTaktForProModeEnter(hdIngameUi_c*) callback
 ext 0270F37C void ingameReturnTaktForProModeExecute(hdIngameUi_c*) callback
 ext 027196A0 void ingameReturnTaktForProModeExit(hdIngameUi_c*) callback
 ext 027196A4 BOOL ingameReturnTaktForProModeComplete(hdIngameUi_c*) callback
 str 1013F4B4 in 02717D54
 data l_ingameTaktPlayDemoState 1049EE94
 ext 0270F424 void ingameTaktPlayDemoEnter(hdIngameUi_c*) callback
 ext 0270F470 void ingameTaktPlayDemoExecute(hdIngameUi_c*) callback
 ext 027196AC void ingameTaktPlayDemoExit(hdIngameUi_c*) callback
 ext 027196B0 BOOL ingameTaktPlayDemoComplete(hdIngameUi_c*) callback
 str 1013F4D4 in 02717D54
 data l_ingameControlWindState 1049EEC0
 ext 0270F568 void ingameControlWindEnter(hdIngameUi_c*) callback
 ext 0270F634 void ingameControlWindExecute(hdIngameUi_c*) callback
 ext 0270F86C void ingameControlWindExit(hdIngameUi_c*) callback
 ext 027196B8 BOOL ingameControlWindComplete(hdIngameUi_c*) callback
 str 1013F270 in 02717D54
 data l_ingameToWarpState 1049EEEC
 ext 0270F8B8 void ingameToWarpEnter(hdIngameUi_c*) callback
 ext 0270F8FC void ingameToWarpExecute(hdIngameUi_c*) callback
 ext 027196C0 void ingameToWarpExit(hdIngameUi_c*) callback
 ext 027196C4 BOOL ingameToWarpComplete(hdIngameUi_c*) callback
 str 1013F4EC in 02717D54
 data l_ingameWarpState 1049EF18
 ext 0270F9F8 void ingameWarpEnter(hdIngameUi_c*) callback
 ext 0270FA84 void ingameWarpExecute(hdIngameUi_c*) callback
 ext 0270FB48 void ingameWarpExit(hdIngameUi_c*) callback
 ext 027196CC BOOL ingameWarpComplete(hdIngameUi_c*) callback
 str 1013F500 in 02717D54
 data l_ingamePhotoViewMiiverseState 1049EF44
 ext 0270FB6C void ingamePhotoViewMiiverseEnter(hdIngameUi_c*) callback
 ext 0270FBDC void ingamePhotoViewMiiverseExecute(hdIngameUi_c*) callback
 ext 027196D4 void ingamePhotoViewMiiverseExit(hdIngameUi_c*) callback
 ext 027196D8 BOOL ingamePhotoViewMiiverseComplete(hdIngameUi_c*) callback
 str 1013F1EC in 02717D54
 data l_ingameMiiverseEmpathingState 1049EF70
 ext 0270FCB4 void ingameMiiverseEmpathingEnter(hdIngameUi_c*) callback
 ext 0270FD48 void ingameMiiverseEmpathingExecute(hdIngameUi_c*) callback
 ext 0270FD94 void ingameMiiverseEmpathingExit(hdIngameUi_c*) callback
 ext 027196E0 BOOL ingameMiiverseEmpathingComplete(hdIngameUi_c*) callback
 str 1013F208 in 02717D54
 data l_ingameToMiiverseSettingState 1049EF9C
 ext 0270FDBC void ingameToMiiverseSettingEnter(hdIngameUi_c*) callback
 ext 0270FDE8 void ingameToMiiverseSettingExecute(hdIngameUi_c*) callback
 ext 027196E8 void ingameToMiiverseSettingExit(hdIngameUi_c*) callback
 ext 027196EC BOOL ingameToMiiverseSettingComplete(hdIngameUi_c*) callback
 str 1013F224 in 02717D54
 data l_ingameMiiverseSettingState 1049EFC8
 ext 0270FE54 void ingameMiiverseSettingEnter(hdIngameUi_c*) callback
 ext 0270FE58 void ingameMiiverseSettingExecute(hdIngameUi_c*) callback
 ext 0270FF08 void ingameMiiverseSettingExit(hdIngameUi_c*) callback
 ext 027196F4 BOOL ingameMiiverseSettingComplete(hdIngameUi_c*) callback
 str 1013F288 in 02717D54
 data l_ingameToConfirmMiiverseCommentJumpState 1049EFF4
 ext 0270FF14 void ingameToConfirmMiiverseCommentJumpEnter(hdIngameUi_c*) callback
 ext 0270FF6C void ingameToConfirmMiiverseCommentJumpExecute(hdIngameUi_c*) callback
 ext 027196FC void ingameToConfirmMiiverseCommentJumpExit(hdIngameUi_c*) callback
 ext 02719700 BOOL ingameToConfirmMiiverseCommentJumpComplete(hdIngameUi_c*) callback
 str 1013F510 in 02717D54
 data l_ingameConfirmMiiverseCommentJumpState 1049F020
 ext 0270FFF8 void ingameConfirmMiiverseCommentJumpEnter(hdIngameUi_c*) callback
 ext 0271000C void ingameConfirmMiiverseCommentJumpExecute(hdIngameUi_c*) callback
 ext 02719708 void ingameConfirmMiiverseCommentJumpExit(hdIngameUi_c*) callback
 ext 0271970C BOOL ingameConfirmMiiverseCommentJumpComplete(hdIngameUi_c*) callback
 str 1013F538 in 02717D54
 data l_ingameToConfirmMiiverseProfileJumpState 1049F04C
 ext 0271011C void ingameToConfirmMiiverseProfileJumpEnter(hdIngameUi_c*) callback
 ext 02710120 void ingameToConfirmMiiverseProfileJumpExecute(hdIngameUi_c*) callback
 ext 02719714 void ingameToConfirmMiiverseProfileJumpExit(hdIngameUi_c*) callback
 ext 02719718 BOOL ingameToConfirmMiiverseProfileJumpComplete(hdIngameUi_c*) callback
 str 1013F560 in 02717D54
 data l_ingameConfirmMiiverseProfileJumpState 1049F078
 ext 027101AC void ingameConfirmMiiverseProfileJumpEnter(hdIngameUi_c*) callback
 ext 027101C0 void ingameConfirmMiiverseProfileJumpExecute(hdIngameUi_c*) callback
 ext 02719720 void ingameConfirmMiiverseProfileJumpExit(hdIngameUi_c*) callback
 ext 02719724 BOOL ingameConfirmMiiverseProfileJumpComplete(hdIngameUi_c*) callback
 str 1013F588 in 02717D54
 data l_ingameMiiverseProfileJumpState 1049F0A4
 ext 027102B4 void ingameMiiverseProfileJumpEnter(hdIngameUi_c*) callback
 ext 027102C0 void ingameMiiverseProfileJumpExecute(hdIngameUi_c*) callback
 ext 0271972C void ingameMiiverseProfileJumpExit(hdIngameUi_c*) callback
 ext 02719730 BOOL ingameMiiverseProfileJumpComplete(hdIngameUi_c*) callback
 str 1013F2A4 in 02717D54
 data l_ingameToMiiverseExplainState 1049F0D0
 ext 02710328 void ingameToMiiverseExplainEnter(hdIngameUi_c*) callback
 ext 027103E0 void ingameToMiiverseExplainExecute(hdIngameUi_c*) callback
 ext 02719738 void ingameToMiiverseExplainExit(hdIngameUi_c*) callback
 ext 0271973C BOOL ingameToMiiverseExplainComplete(hdIngameUi_c*) callback
 str 1013F240 in 02717D54
 data l_ingameMiiverseExplainState 1049F0FC
 ext 02710548 void ingameMiiverseExplainEnter(hdIngameUi_c*) callback
 ext 0271054C void ingameMiiverseExplainExecute(hdIngameUi_c*) callback
 ext 027105FC void ingameMiiverseExplainExit(hdIngameUi_c*) callback
 ext 02719744 BOOL ingameMiiverseExplainComplete(hdIngameUi_c*) callback
 str 1013F2C4 in 02717D54
 data l_ingameToTelescopeState 1049F128
 ext 0271061C void ingameToTelescopeEnter(hdIngameUi_c*) callback
 ext 02710650 void ingameToTelescopeExecute(hdIngameUi_c*) callback
 ext 0271974C void ingameToTelescopeExit(hdIngameUi_c*) callback
 ext 02719750 BOOL ingameToTelescopeComplete(hdIngameUi_c*) callback
 str 1013F2E0 in 02717D54
 data l_ingameTelescopeState 1049F154
 ext 02710660 void ingameTelescopeEnter(hdIngameUi_c*) callback
 ext 02710664 void ingameTelescopeExecute(hdIngameUi_c*) callback
 ext 02719758 void ingameTelescopeExit(hdIngameUi_c*) callback
 ext 0271975C BOOL ingameTelescopeComplete(hdIngameUi_c*) callback
 str 1013F25C in 02717D54
 data l_ingameToPictographBoxState 1049F180
 ext 027106E0 void ingameToPictographBoxEnter(hdIngameUi_c*) callback
 ext 02710738 void ingameToPictographBoxExecute(hdIngameUi_c*) callback
 ext 02719764 void ingameToPictographBoxExit(hdIngameUi_c*) callback
 ext 02719768 BOOL ingameToPictographBoxComplete(hdIngameUi_c*) callback
 str 1013F2F8 in 02717D54
 ext 02710780 void ingamePictographBoxEnter(hdIngameUi_c*) callback
 ext 02710784 void ingamePictographBoxExecute(hdIngameUi_c*) callback
 ext 02719770 void ingamePictographBoxExit(hdIngameUi_c*) callback
 ext 02719774 BOOL ingamePictographBoxComplete(hdIngameUi_c*) callback
 str 1013F18C in 02717D54
 ext 027109C8 void ingameToPictographListEnter(hdIngameUi_c*) callback
 ext 027109CC void ingameToPictographListExecute(hdIngameUi_c*) callback
 ext 0271977C void ingameToPictographListExit(hdIngameUi_c*) callback
 ext 02719780 BOOL ingameToPictographListComplete(hdIngameUi_c*) callback
 str 1013F5B0 in 02717D54
 ext 02710A14 void ingamePictographListEnter(hdIngameUi_c*) callback
 ext 02710A28 void ingamePictographListExecute(hdIngameUi_c*) callback
 ext 02710BAC void ingamePictographListExit(hdIngameUi_c*) callback
 ext 02719788 BOOL ingamePictographListComplete(hdIngameUi_c*) callback
 str 1013F5CC in 02717D54
 data l_ingameReturnPictographBoxState 1049F230
 ext 02710BD4 void ingameReturnPictographBoxEnter(hdIngameUi_c*) callback
 ext 02710BD8 void ingameReturnPictographBoxExecute(hdIngameUi_c*) callback
 ext 02719790 void ingameReturnPictographBoxExit(hdIngameUi_c*) callback
 ext 02719794 BOOL ingameReturnPictographBoxComplete(hdIngameUi_c*) callback
 str 1013F314 in 02717D54
 data l_ingameExitPictographListState 1049F25C
 ext 02710C34 void ingameExitPictographListEnter(hdIngameUi_c*) callback
 ext 02710C38 void ingameExitPictographListExecute(hdIngameUi_c*) callback
 ext 0271979C void ingameExitPictographListExit(hdIngameUi_c*) callback
 ext 027197A0 BOOL ingameExitPictographListComplete(hdIngameUi_c*) callback
 str 1013F5E8 in 02717D54
 ext 02710CDC void ingameToSelectPictographListEnter(hdIngameUi_c*) callback
 ext 02710D34 void ingameToSelectPictographListExecute(hdIngameUi_c*) callback
 ext 027197A8 void ingameToSelectPictographListExit(hdIngameUi_c*) callback
 ext 027197AC BOOL ingameToSelectPictographListComplete(hdIngameUi_c*) callback
 str 1013F608 in 02717D54
 ext 02710D84 void ingameSelectPictographListEnter(hdIngameUi_c*) callback
 ext 02710D88 void ingameSelectPictographListExecute(hdIngameUi_c*) callback
 ext 027197B4 void ingameSelectPictographListExit(hdIngameUi_c*) callback
 ext 027197B8 BOOL ingameSelectPictographListComplete(hdIngameUi_c*) callback
 str 1013F62C in 02717D54
 data l_ingameToFishManHintState 1049F2E0
 ext 02710E88 void ingameToFishManHintEnter(hdIngameUi_c*) callback
 ext 02710EE0 void ingameToFishManHintExecute(hdIngameUi_c*) callback
 ext 027197C0 void ingameToFishManHintExit(hdIngameUi_c*) callback
 ext 027197C4 BOOL ingameToFishManHintComplete(hdIngameUi_c*) callback
 str 1013F1A4 in 02717D54
 data l_ingameFishManHintState 1049F30C
 ext 02710F70 void ingameFishManHintEnter(hdIngameUi_c*) callback
 ext 02710F84 void ingameFishManHintExecute(hdIngameUi_c*) callback
 ext 02711030 void ingameFishManHintExit(hdIngameUi_c*) callback
 ext 027197CC BOOL ingameFishManHintComplete(hdIngameUi_c*) callback
 str 1013F334 in 02717D54
 data l_ingameInputPasswordState 1049F338
 ext 027112D0 void ingameInputPasswordExit(hdIngameUi_c*) callback
 ext 027197D4 BOOL ingameInputPasswordComplete(hdIngameUi_c*) callback
 str 1013F1BC in 02717D54
 data l_ingameDemoTauraArrivalState 1049F364
 ext 027112E0 void ingameDemoTauraArrivalEnter(hdIngameUi_c*) callback
 ext 027197DC void ingameDemoTauraArrivalExit(hdIngameUi_c*) callback
 ext 027197E0 BOOL ingameDemoTauraArrivalComplete(hdIngameUi_c*) callback
 str 1013F64C in 02717D54
 data l_ingameToDemoFishManState 1049F390
 ext 0271148C void ingameToDemoFishManEnter(hdIngameUi_c*) callback
 ext 027197E8 void ingameToDemoFishManExit(hdIngameUi_c*) callback
 ext 027197EC BOOL ingameToDemoFishManComplete(hdIngameUi_c*) callback
 str 1013F1D4 in 02717D54
 data l_ingameDemoFishManState 1049F3BC
 ext 027117FC void ingameDemoFishManEnter(hdIngameUi_c*) callback
 ext 027197F4 void ingameDemoFishManExit(hdIngameUi_c*) callback
 ext 027197F8 BOOL ingameDemoFishManComplete(hdIngameUi_c*) callback
 str 1013F34C in 02717D54
 data l_ingameToDemoTriforceState 1049F3E8
 ext 02719800 void ingameToDemoTriforceExit(hdIngameUi_c*) callback
 ext 02719804 BOOL ingameToDemoTriforceComplete(hdIngameUi_c*) callback
 str 1013F668 in 02717D54
 data l_ingameDemoTriforceState 1049F414
 ext 02711D3C void ingameDemoTriforceEnter(hdIngameUi_c*) callback
 ext 02711F10 void ingameDemoTriforceExit(hdIngameUi_c*) callback
 ext 0271980C BOOL ingameDemoTriforceComplete(hdIngameUi_c*) callback
 str 1013F684 in 02717D54
 data l_ingameDemoPictBottleState 1049F440
 ext 02711F14 void ingameDemoPictBottleEnter(hdIngameUi_c*) callback
 ext 02711F98 void ingameDemoPictBottleExecute(hdIngameUi_c*) callback
 ext 02719814 void ingameDemoPictBottleExit(hdIngameUi_c*) callback
 ext 02719818 BOOL ingameDemoPictBottleComplete(hdIngameUi_c*) callback
 str 1013F69C in 02717D54
 data l_ingameGameOverState 1049F46C
 ext 02719820 void ingameGameOverExit(hdIngameUi_c*) callback
 ext 02719824 BOOL ingameGameOverComplete(hdIngameUi_c*) callback
 str 1013F6B8 in 02717D54
 data l_ingameChangeToTvModeState 1049F498
 ext 027120E8 void ingameChangeToTvModeEnter(hdIngameUi_c*) callback
 ext 0271982C void ingameChangeToTvModeExit(hdIngameUi_c*) callback
 ext 02719830 BOOL ingameChangeToTvModeComplete(hdIngameUi_c*) callback
 str 1013F6CC in 02717D54
 data l_ingameChangeToDrcModeState 1049F4C4
 ext 0271237C void ingameChangeToDrcModeEnter(hdIngameUi_c*) callback
 ext 02719838 void ingameChangeToDrcModeExit(hdIngameUi_c*) callback
 ext 0271983C BOOL ingameChangeToDrcModeComplete(hdIngameUi_c*) callback
 str 1013F364 in 02717D54
 data l_ingameExchangeProModeState 1049F4F0
 ext 02712584 void ingameExchangeProModeEnter(hdIngameUi_c*) callback
 ext 02712844 void ingameExchangeProModeExit(hdIngameUi_c*) callback
 ext 02719844 BOOL ingameExchangeProModeComplete(hdIngameUi_c*) callback
 str 1013F380 in 02717D54
 data sRenderManager 101F95D0
 data l_ingameToTaktSignListState 1049EE10
 data l_ingameTaktSignListState 1049EE3C
 data l_ingameToPictographListState 1049F1D8
 data l_ingameToSelectPictographListState 1049F288
 data sRenderDelegateVtable 1013EF78
 data sHudGroupName 1013F134
 data sInputGroupName 1013F138
 data sPrimaryGroupName 1013F130
 data sOverlayGroupName 1013F128
 data sModeSwitchGroupName 1013F140
 ext 02715CA0 void hdIngameHudDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*) callback
 ext 02716494 void hdIngameInputDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*) callback
 ext 02716AD0 void hdIngamePrimaryDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*) callback
 ext 02716DA4 void hdIngameOverlayDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*) callback
 ext 02716FFC void hdIngameModeSwitchDrawCallback(hdIngameUi_c*, hdLayerRenderInfo_c*) callback
 ext 027C23F8 void hdIngameAttachScreenGroup(hdRenderCommandLayer_l*, u32, hdIngameScreenGroup*)
 ext 02030C70 void hdIngameResumeSceneUpdates()
 ext 02030D6C void hdIngameResumeSceneRendering()
 ext 02030DC0 void hdIngameResumeSceneInput()
 ext 020309AC void hdIngameSetSceneTransition(u32)
 ext 0265370C void hdCursorMgr_c::destroyCursors()
 ext 02653588 void hdCursorMgr_c::deleteInstance()
 ext 026534F4 void hdCursorMgr_c::createInstance(void*)
 ext 0265361C u32 hdCursorMgr_c::create(u32, u32)
 ext 0265381C void hdCursorMgr_c::execute()
 ext 026538A0 void hdCursorMgr_c::draw(void*)
 ext 02619B68 void hdSwkbd_mgr_c::calc()
 ext 02619EDC be<u16>* hdSwkbd_mgr_c::getText()
 ext 02619E84 void hdSwkbd_mgr_c::close()
 ext 0261A008 u32 hdSwkbd_mgr_c::isHidden()
 ext 02619D7C void hdSwkbd_mgr_c::open()
 ext 02619EE4 void hdSwkbd_mgr_c::layoutCall(u32)
 ext 02619C90 void hdSwkbd_mgr_c::drawDRC(u32)
 ext 02619D04 void hdSwkbd_mgr_c::drawLayouts(u32)
 ext 02619C1C void hdSwkbd_mgr_c::drawTV(u32)
 ext 02617F14 u32 hdInputManager::hasPad()
 ext 026184AC u32 hdInputManager::isAlternateGamePadMode()
 ext 02618094 u32 hdInputManager::setMode(u32)
 ext 02618498 u32 hdInputManager::isGamePadMode()
 ext 02617AE4 u32 hdInputManager::isTvMode()
 ext 026184C0 u32 hdInputManager::isCombinedMode()
 ext 02687E88 u32 hdMapDisplayCoordinator_c::setup()
 ext 0268801C void hdMapDisplayCoordinator_c::destroyMaps()
 ext 026885FC void hdMapDisplayCoordinator_c::execute()
 ext 02688790 void hdMapDisplayCoordinator_c::processInput(u32, u32)
 ext 02688998 void hdMapDisplayCoordinator_c::setSeaMapMarkerForward(u32, u32)
 ext 02688B38 void hdMapDisplayCoordinator_c::saveSelectedMapSettings()
 ext 02688BD8 void hdMapDisplayCoordinator_c::openSelectedMap()
 ext 02688C50 void hdMapDisplayCoordinator_c::changeSelectedMap()
 ext 02688DF0 void hdMapDisplayCoordinator_c::showSelectedMap()
 ext 02688F98 void hdMapDisplayCoordinator_c::setMarkersActive(u32)
 ext 02688FF0 void hdMapDisplayCoordinator_c::updateSeaCells()
 ext 0268900C void hdMapDisplayCoordinator_c::updateSeaMapSelection()
 ext 02689028 void hdMapDisplayCoordinator_c::updateSeaMapCursor()
 ext 02689044 u32 hdMapDisplayCoordinator_c::isSeaMapSelected()
 ext 02689068 u32 hdMapDisplayCoordinator_c::isSeaMapCursorActive()
 ext 0268908C void hdMapDisplayCoordinator_c::clearSeaMapSelection()
 ext 026890A8 u32 hdMapDisplayCoordinator_c::isSeaMapSelectionEmpty()
 ext 02689128 s32 hdMapDisplayCoordinator_c::getSelectedSeaCell()
 ext 02688F04 void hdMapDisplayCoordinator_c::configureDungeonMap(u32, u32, u32)
 ext 02688E70 void hdMapDisplayCoordinator_c::configureSeaMap(u32, u32, u32)
 ext 02614FD8 void cking::ui::input::EventMgr::reset()
 ext 02615140 u32 cking::ui::input::EventMgr::getMode()
 ext 02615148 void cking::ui::input::EventMgr::setParams(u32, u32)
 ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
 ext 026151A0 u32 cking::ui::input::EventMgr::getHover()
 ext 02615398 u32 cking::ui::input::EventMgr::getId()
 ext 0268D8E4 hdMenuButtons* hdMenuButtons::construct(hdMenuButtons*)
 ext 026A3A4C hdMenuMiiverse_c* hdMenuMiiverse_c::construct(hdMenuMiiverse_c*)
 ext 026866EC hdMapDisplayCoordinator_c* hdMapDisplayCoordinator_c::construct(hdMapDisplayCoordinator_c*)
 ext 026F2148 void hdBasicButton_c::forceDecide()
 ext 0268DC34 u32 hdMenuButtons::create(void*, void*, void*)
 ext 0268DE6C void hdMenuButtons::show()
 ext 0268DEDC void hdMenuButtons::closeMenu()
 ext 0268DECC void hdMenuButtons::openMenu()
 ext 02626158 hdScreenBoatRaceFailed_c* hdScreenBoatRaceFailed_c::construct(hdScreenBoatRaceFailed_c*)
 ext 02627B08 hdBoatRaceResultView* hdBoatRaceResultView::construct(hdBoatRaceResultView*)
 ext 02628578 hdScreenBoatRaceStart_c* hdScreenBoatRaceStart_c::construct(hdScreenBoatRaceStart_c*)
 ext 026DAE94 hdSequenceWindow_c* hdSequenceWindow_c::construct(hdSequenceWindow_c*)
 ext 02651784 hdScreenMsgCursor_c* hdScreenMsgCursor_c::construct(hdScreenMsgCursor_c*)
 ext 0262633C u32 hdScreenBoatRaceFailed_c::setup(u32, u32)
 ext 02627B90 u32 hdBoatRaceResultView::setup(u32, u32, void*)
 ext 026285F0 u32 hdScreenBoatRaceStart_c::setup(u32, u32, void*)
 ext 026519C4 u32 hdScreenMsgCursor_c::init(u32, u32, void*)
 ext 0264E2C8 u32 dOperate_wind_c::_create(u32, u32, u32)
 ext 0261EDFC u32 Metronome::dMetronome_c::screenSet(u32, u32, u32)
 ext 026C9B10 u32 MenuHD::dMenu_PlaceName_c::create(u32, u32, u32)
 ext 026A4710 u32 hdMenuMiiverse_c::initializeMenu(u32, u32, u32)
 ext 026F17BC void hdBasicButton_c::decideEnd()
 ext 026A01AC void hdMenuMiiverse_c::setButtonsEnabled(u32)
 ext 026A6188 void hdMenuMiiverse_c::resetChartButtons()
 ext 026A5AAC void hdMenuMiiverse_c::updateChartNavigationLinks()
 ext 026A5754 void hdMenuMiiverse_c::updateFirstNavigationLinks(u32, u32, u32)
 ext 026A5E20 void hdMenuMiiverse_c::updateLastNavigationLinks(u32, u32, u32)
 ext 0261C48C u32 rupy_ctor(RupyScreen*)
 ext 0264E0E8 u32 dOw_createScreen(u32)
 ext 0261EAC8 u32 metroCtor(u32)
 ext 026C9A8C u32 dMenu_PlaceName_c__ct(MenuHD::dMenu_PlaceName_c*)
 ext 0261C54C u32 rupy_setup(RupyScreen*, u32, u32, u32)
 data sIngameUi 101F8344
 data sIngameDisposer 101F834C
 data sIngameDisposerVtable 1013F6F8
 data sMenuClosingState 104996C0
 data sOverlayClosingState 10499D44
 data l_ingameToPauseDrcState 1049EC2C
 data sInputEventManager 101F4FF0
 data l_ingamePictographListState 1049F204
 data sMenuIdleState 10499930
 data sMenuEntryState 10499720
 data l_ingamePictographBoxState 1049F1AC
 data l_ingameSelectPictographListState 1049F2B4
 data sInputOptions 101F8378
 data sPictographIdleState 10499524
 data sFishManDisplayState 1049DD90
 data l_ingameMapCoordinatorClosedState 104963D8
 ext 0268EC54 void hdMenuMain_c::setSelectionMode(u32)
 ext 0270E474 u32 hdIngameCanCloseItemScreen(hdIngameUi_c*)
 ext 0268F92C void hdIngameDismissItemScreen(void*)
 ext 0270D600 u32 hdIngameMenuControlsBlocked(hdIngameUi_c*)
 ext 025986BC u8 hdIngameMenuPauseFlag()
 data sControllerManager 101F8AE8
 ext 0273CB44 hdController* hdInputControllerByOrder(void*, u32, u32)
 str 1013F14C in 027151A0
 data sSubWindowClosingState 10497708
 data sSubWindowOpeningState 104976D8
 ext 026941AC void hdIngameActivateItemScreen(void*)
 ext 0270F38C void hdIngameRefreshMenuSelection(hdIngameUi_c*)
 ext 026AE0EC void hdIngameDrawMenuMessage(void*, void*)
 ext 02651C60 void hdIngameDrawMessageChoiceCursor(void*, void*)
 data sMapCoordinatorOnly 101F8350
 ext 02636778 void hdIngameFinalizeSharedMenuResources()
 data sMessageCore 101F4B5C
 data sMenuResetState 10499960
 data sOverlayResetState 10499FE4
 data sMiiversePromptResetState 1049CB88
 ext 0267672C void hdIngameResetMainHud(hdParts_c*)
 ext 02649624 void hdIngameResetStatusScreen(hdParts_c*)
 ext 0266A018 void hdIngameResetModeSwitchEffects(hdParts_c*)
 ext 026DBF4C void hdIngameResetHintScreen(hdSequenceWindow_c*)
 ext 02622760 void hdIngameResetMenuAuxiliary(hdParts_c*)
 ext 025F78C4 void dMesg_core_c::reset()
 ext 020013C0 hdUiEvent* hdIngameCopyInputEvent(hdUiEvent*, hdUiEvent*)
 ext 0268F420 u32 hdIngameItemForReceiver(void*, u32)
 ext 0269451C void hdIngameBindItemSlot(void*, u32, u32)
 ext 02694500 u32 hdIngameSlotForReceiver(void*, u32)
 data sSaveRoot 101F84DC
 ext 02675AF8 void* hdIngameConstructMainHud(void*)
 ext 02692B44 void* hdIngameConstructItemScreen(void*)
 ext 02648F3C void* hdIngameConstructStatusScreen(void*)
 ext 0264BC1C void* hdIngameConstructMenuEffects(void*)
 ext 0266ADBC void* hdIngameConstructAuxiliaryScreen(void*)
 ext 02671D8C void* hdIngameConstructBatonSign(void*)
 ext 026725B8 void* hdIngameConstructBatonEffects(void*)
 ext 026AD784 void* hdIngameConstructMenuMessage(void*)
 ext 026AFA1C void* hdIngameConstructMessageCounter(void*)
 ext 026B3410 void* hdIngameConstructMessageDetail(void*)
 ext 026B5D30 void* hdIngameConstructMessageList(void*)
 ext 026B8840 void* hdIngameConstructMessageChoice(void*)
 ext 026BB3FC void* hdIngameConstructMessageComposite(void*)
 ext 026D7828 void* hdIngameConstructModeHint(void*)
 ext 0264F188 void* hdIngameConstructItemControlHints(void*)
 ext 0264FD3C void* hdIngameConstructMenuConfirmation(void*)
 ext 026E8490 void* hdIngameConstructMiiversePrompt(void*)
 ext 026C29B0 void* hdIngameConstructMenu(void*)
 ext 026C7CA4 void* hdIngameConstructOverlay(void*)
 ext 02622080 hdBatonSignList_c* hdBatonSignList_c::construct(hdBatonSignList_c*)
 ext 026693BC void* hdIngameConstructModeSwitchScreen(void*)
 ext 02669D08 void* hdIngameConstructModeSwitchEffects(void*)
 ext 026C07D8 void* hdIngameConstructPictographScreen(void*)
 ext 026258A4 void* hdIngameConstructNameEntry(void*)
 ext 02623D14 void* hdIngameConstructHudCounterB(void*)
 ext 026463E0 void* hdIngameConstructHudCounterF(void*)
 ext 026DE8B8 void* hdIngameConstructHudCounterG(void*)
 ext 026EFCA0 void* hdIngameConstructWarpButtons(void*)
 ext 026BDE10 void* hdIngameConstructTouchPanel(void*)
 ext 02676174 void hdIngameLoadMainHud(void*, void*, void*, u32)
 ext 02649364 void hdIngameLoadStatusScreen(void*, void*, void*, u32)
 ext 0266AF9C void hdIngameLoadAuxiliaryScreen(void*, void*, void*, u32)
 ext 02671DF8 void hdIngameLoadBatonSign(void*, void*, void*, u32)
 ext 026ADBE4 void hdIngameLoadMenuMessage(void*, void*, void*, u32)
 ext 026B8C34 void hdIngameLoadMessageChoice(void*, void*, void*, u32)
 ext 026BBFAC void hdIngameLoadMessageComposite(void*, void*, void*, u32)
 ext 026DB5F4 void hdIngameLoadHintScreen(void*, void*, void*, u32)
 ext 026D7E4C void hdIngameLoadModeHint(void*, void*, void*, u32)
 ext 0264F214 void hdIngameLoadItemControlHints(void*, void*, void*, u32)
 ext 0264FDB0 void hdIngameLoadMenuConfirmation(void*, void*, void*, u32)
 ext 026E8820 void hdIngameLoadMiiversePrompt(void*, void*, void*, u32)
 ext 026C2C70 void hdIngameLoadMenu(void*, void*, void*, u32)
 ext 026C821C void hdIngameLoadOverlay(void*, void*, void*, u32)
 ext 02622418 void hdIngameLoadMenuAuxiliary(void*, void*, void*, u32)
 ext 02669428 void hdIngameLoadModeSwitchScreen(void*, void*, void*, u32)
 ext 02669D84 void hdIngameLoadModeSwitchEffects(void*, void*, void*, u32)
 ext 026C085C void hdIngameLoadPictographScreen(void*, void*, void*, u32)
 ext 02625928 void hdIngameLoadNameEntry(void*, void*, void*, u32)
 ext 02623EBC void hdIngameLoadHudCounterB(void*, void*, void*, u32)
 ext 02646470 void hdIngameLoadHudCounterF(void*, void*, void*, u32)
 ext 026EFEC4 void hdIngameLoadWarpButtons(void*, void*, void*, u32)
 ext 02693650 void hdIngameLoadItemScreen(void*, void*, void*, u32)
 ext 0264BCB8 void hdIngameLoadMenuEffects(void*, void*, void*, u32)
 ext 02672624 void hdIngameLoadBatonEffects(void*, void*, void*, u32)
 ext 026BDE90 void hdIngameLoadTouchPanel(void*, void*, void*, u32)
 ext 026DEA90 void hdIngameLoadHudCounterG(void*, void*, void*, u32)
 data sDefaultUiProjection 104A20FC
 data sDefaultUiMatrix 104A2098
 ext 026366D8 void hdIngameCreateSharedMenuScreen(u32)
 ext 026367F8 void hdIngameLoadSharedMenu(hdParts_c*, void*, void*, u32)
 data sSceneActivityManager 101F86B4
 ext 02728CE8 u32 hdIngameSceneUpdatesPaused(void*)
 data sWideStringVtable 1013EED8
 ext 02619ECC u32 hdIngamePasswordAccepted()
 ext 02619ED4 u32 hdIngamePasswordCancelled()
 ext C0009988 void* hdIngameCopyPassword(void*, void*, u32, u32)
 data sKeyboardManager 101F50D8
 ext 0269498C u32 hdIngameItemScreenReady(void*)
 ext 026942E0 void hdIngameFinishTriforceItemScreen(void*)
 ext 026943C8 void hdIngameSetItemScreenActive(void*, u32)
 data sModeHintIdleState 1049DD60
 ext 02625A80 void hdIngameBeginModeHint(void*)
 ext 0270E844 void hdIngameCompleteProMode(hdIngameUi_c*)
 str 1013F110 in 027125E4
 data sBatonTvState 10494DA0
 data sBatonDrcState 10494D40
 data sBatonEffectsClosedState 10494E14
 ext 02669610 u32 hdIngameModeSwitchEntryComplete(hdParts_c*)
 ext 02669580 void hdIngameFinishModeSwitchScreen(hdParts_c*)
 ext 02669590 u32 hdIngameModeSwitchExitComplete(hdParts_c*)
 ext 026C0A4C void hdIngameResetPictograph(void*)
 str 1013F0E0 in 027121B8
 str 1013F0F8 in 027123E0
 data sSharedMenuScreen 101F5578
 data sCursorManager 101F5A44
 ext 02030D94 void hdIngameBeginGameOver()
 ext 026DBA08 void hdIngameSetHintScreen(hdSequenceWindow_c*, u32, u32)
 data sUiMode 101F8348
 ext C0006988 void hdIngameSetShaderModeEx(u32, u32, u32, u32, u32, u32, u32)
 ext 02006364 void hdIngameUpdateState(hdStateMachine*)
 ext 0273B0D4 void* hdIngameAllocate(u32, void*, u32)
 ext 02752B0C void hdIngameConstructDisposer(hdIngameDisposer*, void*, u32)
 ext 02752BEC void hdIngameDestroyDisposerBase(hdIngameDisposer*, u32)
 ext 027C0804 void hdIngameDestroyScreenGroup(hdIngameScreenGroup*, u32)
 ext 020063C0 void hdIngameChangeState(hdStateMachine*, hdIngameStateId_c*)
 ext 02006478 hdIngameStateId_c* hdIngameCurrentState(hdStateMachine*)
 ext 0268DB18 void hdMenuButtons::connectButtons()
 ext 0268DAB0 u32 hdMenuButtons::buttonKey(u32)
 ext 02694994 void hdIngameSetItemScreenReceivers(void*, u32, u32, u32)
 ext 02695D18 void hdIngameConnectItemScreenOpen(void*)
 ext 026949A4 void hdIngameConnectItemScreenClosed(void*)
 ext 025E1988 void hdIngamePlayMenuExitSound(u32)
 ext 02030DE8 void hdIngamePauseScene()
 ext 0269474C void hdIngameCloseItemScreen(void*)
 ext 02669600 void hdIngameBeginModeSwitchScreen(void*)
 ext 0270D0F0 void hdIngameSetTvScreens(hdIngameUi_c*, u32)
 ext 0270D17C void hdIngameSetDrcScreens(hdIngameUi_c*, u32)
 ext 02030804 void hdIngameSetModeSwitchRendering(u32)
 ext 02030880 void hdIngameSetModeSwitchInput(u32)
 ext 0270E320 void hdIngamePrepareMapCoordinator(hdIngameUi_c*)
 ext 026C09B4 void hdIngameFinishPictograph(void*)
 ext 02030E10 void hdIngameFrameworkResume()
 ext 02035D78 hdIngameFramework_l* hdIngameGetFramework()
 ext 02035ED4 void hdIngameSetFrameworkRunning(hdIngameFramework_l*, u32)
 ext 0259170C void dMenu_setMenuStatus(u32)
 ext 026C86D4 u32 hdPictographList::isInAnimationFinished()
 ext 026C86E4 u32 hdPictographList::isOutAnimationFinished()
 ext 026C86F4 void hdPictographList::clearTextureImage(s32)
*/
