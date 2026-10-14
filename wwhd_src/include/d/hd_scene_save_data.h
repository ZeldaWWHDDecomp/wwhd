#pragma once
#include "bindings.h"
#include "hd/hd_sys_02006294.h"
#include "d/hd_save_info_holder.h"
#include "d/hd_swkbd_mgr.h"
#include "d/hd_input_ctrl.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_msg_text.h"
#include "d/hd_picture_accessor.h"
#include "d/hd_title_parts.h"
#include "d/d_file_select.h"
#include "d/hd_screen_system_window.h"
#include "d/hd_screen_command_extra.h"
#include "d/hd_screen_blur_bg.h"
#include "d/hd_cursor_mgr.h"
#include "hd/hd_olv_02038E24.h"
struct hdSaveDataDrawInfo_l { u8 mBase[0xC]; be<u32> mSlot; };
struct hdSaveDataUI_l { be<u32> mType; gptr<be<u32>> mpVtable; };

// Scene code reads only the low byte of the guest big-endian slot word.
inline u8 saveDataSelectedSlot(dFile_select_c* screen) {
    return reinterpret_cast<be<u8>*>(&screen->selectNum)[3];
}
struct hdSaveDataStateId_l { be<u32> mId; be<u32> mName; gptr<be<u32>> mpVtable; };

struct hdSaveDataListener_l {
    u8 mDisposer[0xC]; be<u32> mVtable;
    gptr<const char> mpName; be<u32> mStringVtable;
    be<u32> mEnabled; gptr<void> mpUser;
    be<u32> mDelegateVtable; gptr<void> mpObject;
    be<u16> mThisAdjustment; be<u16> mVirtualIndex; be<u32> mFunction;
    u8 mStorage[0x10];
};
WWHD_SIZE(hdSaveDataListener_l, 0x40);
struct hdSaveDataScene_c {
    be<u32> mFactoryVtable;
    be<u32> mCallbackStateVtable;
    gptr<void> mpStateOwner;
    gptr<hdSaveDataStateId_l> mpStateId;
    hdStateMachine mState;
    gptr<gfn> mpTaskVtable;
    gptr<void> mpSaveDataScreen;
    /* the save-data dialog: the system window of HUD-shortcut (hd_screen_system_window.h) */
    hdScreenSystemWindow_c* saveScreen() { return static_cast<hdScreenSystemWindow_c*>(mpSaveDataScreen.get()); }
    hdSaveDataListener_l mTvListener;
    hdSaveDataListener_l mTvForeListener;
    hdSaveDataListener_l mDrcListener;
    hdSaveDataListener_l mDrcForeListener;
    gptr<void> mpCursor;
    gptr<hdTitleControllerSelection_c> mpConfirmation;
    gptr<dFile_select_c> mpFileSelect;
    gptr<void> mpController;
    gptr<void> mpBackground;
    /* the blurred background screen (MENU-blur_bg, hd_screen_blur_bg.h) */
    hdBlurBackground_c* background() { return static_cast<hdBlurBackground_c*>(mpBackground.get()); }
    gptr<void> mpSaveHolder;
    be<u8> mNewGame;
    be<u8> mEmptySlot;
    be<u8> mLoading;
    u8 mFlagsPadding;
    be<u32> mPhase;
    be<u8> mFinishing;
    u8 mTailPadding[3];
    u32 receiveTouch();
    u32 receiveKey();
    u32 receiveController();
    void hideBackground();
    void clearBackRequest();
    void enterNameEntry();
    void beginControllerChoice();
    void resetEventMode();
    void updateNameEntry();
    void enterLoad();
    void leaveLoad();
    void beginGame();
    void beginNameEntry();
    void selectSaveState();
    void enterCancelKeyboard();
    void updateCancel();
    void updateSaveSelection();
    void beginSlotLoad();
    void updateSlotLoad();
    void updateLoadedFlags();
    void updateScreens();
    void updateControllerChoice();
    void createScreens();
    void initialize();
    void execute();
    void destroyScreens();
    void unloadLayouts();
    void finish();
    void draw(hdSaveDataDrawInfo_l*);
    void drawTv(hdSaveDataDrawInfo_l*);
    void drawTvFore(hdSaveDataDrawInfo_l*);
    void drawDrc(hdSaveDataDrawInfo_l*);
    void drawDrcFore(hdSaveDataDrawInfo_l*);
    u8 isSaveSelection();
    u8 isFileOperation();
    u8 isSaveScreenReady();
    void updateInputAvailability();
    void enterControllerSelect();
    void enterDifficultySelect();
    void enterFinish();
};
WWHD_OFFSET(hdSaveDataScene_c, mState, 0x10);
WWHD_OFFSET(hdSaveDataScene_c, mpTaskVtable, 0x24);
WWHD_OFFSET(hdSaveDataScene_c, mpSaveDataScreen, 0x28);
WWHD_OFFSET(hdSaveDataScene_c, mpCursor, 0x12C);
WWHD_OFFSET(hdSaveDataScene_c, mpFileSelect, 0x134);
WWHD_OFFSET(hdSaveDataScene_c, mpBackground, 0x13C);
WWHD_OFFSET(hdSaveDataScene_c, mPhase, 0x148);
WWHD_OFFSET(hdSaveDataScene_c, mFinishing, 0x14C);

struct hdSaveDataInput_l { u8 mBase[0x14]; be<u8> mEnabled; };
struct hdSaveDataCallback_l { be<u16> mThisAdjustment; be<u16> mVirtualIndex; be<u32> mFunction; };
struct hdSaveDataState_l {
    be<u32> mId; gptr<const char> mpName; gptr<be<u32>> mpVtable;
    hdSaveDataCallback_l mCallbacks[4];
};
WWHD_OFFSET(hdSaveDataInput_l, mEnabled, 0x14);
void saveDataBeginName(void*, u32, u32);
void saveDataChangeState(void*, hdSaveDataState_l*);
void saveDataGamePrepare();
void saveDataStartMusic(u32);


struct hdSaveDataPlay_l { u8 mBase[0x5C28]; be<u8> mMenuKind; be<u8> mMenuPage; };
WWHD_OFFSET(hdPictureSlotSummary_c, mRupees, 0x24);
WWHD_OFFSET(hdSaveDataPlay_l, mMenuKind, 0x5C28);
hdSaveDataStateId_l* saveDataCurrentStateId(hdStateMachine*);

hdSaveDataPlay_l* saveDataGetPlay();

void saveDataMachineUpdate(hdStateMachine*);

struct hdSaveDataCursorModes_l { be<u8> mModes[13]; };

struct hdSaveDataRenderSlot_l {
    u8 mBase[0x48]; gptr<void> mpMovie; gptr<void> mpResource; be<u32> mFlags;
    u8 mMoviePadding[0x30]; u8 mCustomMovie[0xE0]; gptr<void> mpCustomResource;
    u8 mRenderPadding[0x3E0]; u8 mRenderContext[4];
};
struct hdSaveDataRenderArray_l { u8 mBase[0x1020]; be<u32> mCount; gptr<gptr<hdSaveDataRenderSlot_l>> mpSlots; };
void saveDataSetShaderMode(u32, u32, u32, u32, u32, u32, u32);

struct hdSaveDataSafeString_l { gptr<const char> mpText; be<u32> mVtable; };
struct hdSaveDataGameInfo_l { u8 mBase[0x1684]; be<u32> mSceneMode; };
void saveDataLayoutRelease(void*, hdSaveDataSafeString_l*);
void saveDataDestroyArray(void*, u32, u32, void*, u32, u32);
void saveDataHolderDestroy(void*, u32);

WWHD_SIZE(hdSaveDataScene_c, 0x150);
WWHD_OFFSET(hdSaveDataScene_c, mTvListener, 0x2C);
WWHD_OFFSET(hdSaveDataScene_c, mDrcForeListener, 0xEC);
void saveDataMachineConstruct(hdStateMachine*, void*, void*);
void saveDataListenerConstruct(void*);
void saveDataListenerDestroy(void*, u32);

struct hdSaveDataDisposer_l { u8 mBase[0xC]; be<u32> mVtable; };
struct hdSaveDataSingleton_l { hdSaveDataScene_c mScene; hdSaveDataDisposer_l mDisposer; };
WWHD_SIZE(hdSaveDataSingleton_l, 0x160);
void* saveDataHeapAllocate(u32, void*, u32);
void saveDataDisposerConstruct(void*, void*, u32);
void saveDataDisposerDestroy(void*, u32);


void saveDataRegisterListener(hdSaveDataRenderSlot_l*, u32, void*);
void* saveDataHolderCreate(void*);
void saveDataPrepareInput();
void saveDataPrepareAudio();
void saveDataSetAudioMode(u32);
void saveDataClearSlot(void*, u32);

void saveDataDrawTvCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*);
void saveDataDrawTvForeCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*);
void saveDataDrawDrcCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*);
void saveDataDrawDrcForeCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*);


dFile_select_c* saveDataFileSelectConstruct(void*, void*);

u32 saveDataRequestCommentDownload(void*);
u32 saveDataSelectedSlotLoaded(dFile_select_c*);

struct hdSaveDataViewBounds_l { be<f32> mX; be<f32> mY; };

void saveDataControllerSelectEnterCallback();
void saveDataControllerSelectExecuteCallback();
void saveDataControllerSelectLeaveCallback();
void saveDataControllerSelectEventCallback();
void saveDataSaveDataSelectEnterCallback();
void saveDataSaveDataSelectExecuteCallback();
void saveDataSaveDataSelectLeaveCallback();
void saveDataSaveDataSelectEventCallback();
void saveDataNameInputEnterCallback();
void saveDataNameInputExecuteCallback();
void saveDataNameInputLeaveCallback();
void saveDataNameInputEventCallback();
void saveDataCancelEnterCallback();
void saveDataCancelExecuteCallback();
void saveDataCancelLeaveCallback();
void saveDataCancelEventCallback();
void saveDataCancelKeyboardEnterCallback();
void saveDataCancelKeyboardExecuteCallback();
void saveDataCancelKeyboardLeaveCallback();
void saveDataCancelKeyboardEventCallback();
void saveDataCheckDataEnterCallback();
void saveDataCheckDataExecuteCallback();
void saveDataCheckDataLeaveCallback();
void saveDataCheckDataEventCallback();
void saveDataWaitEndSelectEnterCallback();
void saveDataWaitEndSelectExecuteCallback();
void saveDataWaitEndSelectLeaveCallback();
void saveDataWaitEndSelectEventCallback();
void saveDataEndSelectEnterCallback();
void saveDataEndSelectExecuteCallback();
void saveDataEndSelectLeaveCallback();
void saveDataEndSelectEventCallback();

struct hdSaveDataNameString_l { gptr<be<u16>> mpText; gptr<be<u32>> mpVtable; };
struct hdSaveDataNameBuffer_l { hdSaveDataNameString_l mString; be<s32> mCapacity; be<u16> mCharacters[10]; };
void saveDataNameStringEnsure(hdSaveDataNameString_l*);
void saveDataNameBufferTerminate(hdSaveDataNameBuffer_l*);
void saveDataNameCopy(void*, void*, u32, u32);
void saveDataTextParserConstruct(HdMsgText*);
void saveDataTextParserSetKey(HdMsgText*, hdSaveDataSafeString_l*, hdSaveDataSafeString_l*);
void saveDataParseText(void*, HdMsgText*, void*, void*);
be<u16>* saveDataTextArrayElement(void*, u32);
void saveDataNameFinishSound(const char*);

void saveDataHeaderObjectConstruct(void*);
void saveDataHeaderAllocatorConstruct(void*);

struct hdSaveDataHeaderZeros_l { be<u32> mValues[4]; };
struct hdSaveDataAngles_l { be<f32> mValues[2]; };

WWHD_OFFSET(hdSaveDataScene_c, mFactoryVtable, 0x0);
WWHD_OFFSET(hdSaveDataScene_c, mCallbackStateVtable, 0x4);
WWHD_OFFSET(hdSaveDataScene_c, mpStateOwner, 0x8);
WWHD_OFFSET(hdSaveDataScene_c, mpStateId, 0xC);
WWHD_OFFSET(hdSaveDataScene_c, mpConfirmation, 0x130);
WWHD_OFFSET(hdSaveDataScene_c, mpController, 0x138);
WWHD_OFFSET(hdSaveDataScene_c, mpSaveHolder, 0x140);
WWHD_OFFSET(hdSaveDataScene_c, mNewGame, 0x144);
WWHD_OFFSET(hdSaveDataScene_c, mEmptySlot, 0x145);
WWHD_OFFSET(hdSaveDataScene_c, mLoading, 0x146);
WWHD_OFFSET(hdSaveDataScene_c, mTvForeListener, 0x6C);
WWHD_OFFSET(hdSaveDataScene_c, mDrcListener, 0xAC);
WWHD_OFFSET(dFile_select_c, mVtable, 0x4);
WWHD_OFFSET(dFile_select_c, mClose, 0x1CC);
WWHD_OFFSET(dFile_select_c, selectEnd, 0x1CD);
WWHD_OFFSET(dFile_select_c, mMessage, 0x27C);
WWHD_OFFSET(hdSaveDataStateId_l, mId, 0x0);
WWHD_OFFSET(hdSaveDataStateId_l, mName, 0x4);
WWHD_OFFSET(hdSaveDataStateId_l, mpVtable, 0x8);
WWHD_OFFSET(hdSaveDataListener_l, mVtable, 0xC);
WWHD_OFFSET(hdSaveDataListener_l, mpName, 0x10);
WWHD_OFFSET(hdSaveDataListener_l, mStringVtable, 0x14);
WWHD_OFFSET(hdSaveDataListener_l, mEnabled, 0x18);
WWHD_OFFSET(hdSaveDataListener_l, mpUser, 0x1C);
WWHD_OFFSET(hdSaveDataListener_l, mDelegateVtable, 0x20);
WWHD_OFFSET(hdSaveDataListener_l, mpObject, 0x24);
WWHD_OFFSET(hdSaveDataListener_l, mThisAdjustment, 0x28);
WWHD_OFFSET(hdSaveDataListener_l, mVirtualIndex, 0x2A);
WWHD_OFFSET(hdSaveDataListener_l, mFunction, 0x2C);
WWHD_OFFSET(hdSaveDataRenderSlot_l, mpMovie, 0x48);
WWHD_OFFSET(hdSaveDataRenderSlot_l, mpResource, 0x4C);
WWHD_OFFSET(hdSaveDataRenderSlot_l, mFlags, 0x50);
WWHD_OFFSET(hdSaveDataRenderSlot_l, mCustomMovie, 0x84);
WWHD_OFFSET(hdSaveDataRenderSlot_l, mpCustomResource, 0x164);
WWHD_OFFSET(hdSaveDataRenderSlot_l, mRenderContext, 0x548);
WWHD_OFFSET(hdSaveDataRenderArray_l, mCount, 0x1020);
WWHD_OFFSET(hdSaveDataRenderArray_l, mpSlots, 0x1024);
static_assert(offsetof(hdTitleControllerSelection_c, mStateHolder) + offsetof(hdPartsStateHolder_c, mMachine) == 0x18);
WWHD_OFFSET(hdTitleControllerSelection_c, mSelectionPending, 0x50);
WWHD_OFFSET(hdTitleControllerSelection_c, mInAnimationFinished, 0x51);
WWHD_OFFSET(hdSaveDataPlay_l, mMenuPage, 0x5C29);
WWHD_OFFSET(hdPictureSlotSummary_c, mPlayerState, 0x26);
WWHD_OFFSET(hdSaveDataState_l, mId, 0x0);
WWHD_OFFSET(hdSaveDataState_l, mpName, 0x4);
WWHD_OFFSET(hdSaveDataState_l, mpVtable, 0x8);
WWHD_OFFSET(hdSaveDataState_l, mCallbacks, 0xC);
WWHD_OFFSET(hdSaveDataNameString_l, mpText, 0x0);
WWHD_OFFSET(hdSaveDataNameString_l, mpVtable, 0x4);
WWHD_OFFSET(hdSaveDataNameBuffer_l, mString, 0x0);
WWHD_OFFSET(hdSaveDataNameBuffer_l, mCapacity, 0x8);
WWHD_OFFSET(hdSaveDataNameBuffer_l, mCharacters, 0xC);
WWHD_SIZE(hdSaveDataState_l, 0x2C);
WWHD_SIZE(hdSaveDataNameString_l, 8);
WWHD_SIZE(hdSaveDataNameBuffer_l, 0x20);

WWHD_OFFSET(hdSaveDataDrawInfo_l, mSlot, 0xC);
WWHD_OFFSET(hdSaveDataUI_l, mpVtable, 0x4);
WWHD_OFFSET(hdSaveDataSafeString_l, mpText, 0x0);
WWHD_OFFSET(hdSaveDataSafeString_l, mVtable, 0x4);
WWHD_OFFSET(hdSaveDataGameInfo_l, mSceneMode, 0x1684);
WWHD_OFFSET(hdSaveDataDisposer_l, mVtable, 0xC);
WWHD_OFFSET(hdSaveDataSingleton_l, mScene, 0x0);
WWHD_OFFSET(hdSaveDataSingleton_l, mDisposer, 0x150);
WWHD_OFFSET(hdSaveDataCallback_l, mThisAdjustment, 0x0);
WWHD_OFFSET(hdSaveDataCallback_l, mVirtualIndex, 0x2);
WWHD_OFFSET(hdSaveDataCallback_l, mFunction, 0x4);
WWHD_OFFSET(hdSaveDataCursorModes_l, mModes, 0x0);
WWHD_OFFSET(hdSaveDataViewBounds_l, mX, 0x0);
WWHD_OFFSET(hdSaveDataViewBounds_l, mY, 0x4);
WWHD_OFFSET(hdSaveDataHeaderZeros_l, mValues, 0x0);
WWHD_OFFSET(hdSaveDataAngles_l, mValues, 0x0);

void saveDataDrawKeyboardTv();
void saveDataDrawKeyboardDrc();
u32 saveDataKeyboardCancelled();
u32 saveDataKeyboardAccepted();

// copyWideName reads the source SafeString prefix only; local frame descriptors have no capacity member.
static_assert(offsetof(hdSaveDataNameString_l, mpText) == offsetof(hdNameString_c, mpText));
static_assert(offsetof(hdSaveDataNameString_l, mpVtable) == offsetof(hdNameString_c, mVtable));
static_assert(sizeof(hdSaveName_l) == sizeof(hdPlayerName_c));

/* hd_scene_save_data.cpp: the save-data scene's constructor (0270B780) and deleting destructor (0270BA48), also the
 * base of the extra-save scene (hd_scene_ex_save.cpp) */
hdSaveDataScene_c* saveDataSceneConstruct(hdSaveDataScene_c* scene);
void saveDataSceneDestroy(hdSaveDataScene_c* scene, u32 flags);
