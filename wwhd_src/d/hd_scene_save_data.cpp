/** HD file-selection scene, controller selection and name-entry transitions. Private source. */
#include "d/hd_scene_save_data.h"
static u8 sNameEntryActive;
static u8 sSaveTransition;
static hdInputManager* sInputManager;
static cking::ui::input::EventMgr* sEventManager;
static hdSaveDataInput_l* sInput;
static hdSwkbd_mgr_c* sKeyboard;
static hdSaveInfoHolder_c* sSaveInfo;
static void* sNameScene;
static hdCursorMgr_c* sCursorMgr; /* the cursor manager singleton (MENU-cursor, hd_cursor_mgr.cpp sCursorMgrInstance) */
static hdSaveDataRenderArray_l* sRenderArray;
static void* sLayoutManager;
static hdSaveDataGameInfo_l* sGameInfo;
static u8 sSafeStringVtable;
static u8 sStateFactoryVtable;
static u8 sCallbackStateVtable;
static u8 sBaseTaskVtable;
static u8 sTaskVtable;
static u8 sListenerVtable;
static u8 sDelegateBaseVtable;
static u8 sDelegateEmptyVtable;
static u8 sInitialStateFactory;
static u8 sSingletonTaskVtable;
static u8 sDisposerVtable;
static u8 sDelegateVtable;
static hdSaveDataScene_c* sSaveDataScene;
static hdSaveDataDisposer_l* sSaveDataDisposer;
static u8 sFallbackResource;
static u8 sFallbackMovie;
static u8 sControllerVisible;
static u8 sControllerSelection;
static OliveCommentMgr* sCommentManager;
static void* sOperationManager;
static u32 sStateIdsInitialized;
static u32 sStateIdCounter;
static u8 sStateVtable;
static hdSaveDataSafeString_l sDefaultNameKey;
static hdSaveDataSafeString_l sSaveDataLayoutKey;
static hdSaveDataViewBounds_l sFarBounds;
static hdSaveDataViewBounds_l sNearBounds;
static hdSaveDataHeaderZeros_l sHeaderZeros;
static hdSaveDataAngles_l sAngleConstants;
static hdSaveDataAngles_l sAngleRange;
static u8 sHeaderObject;
static u8 sHeaderAllocator;
static u8 sHeaderRegistration;
static u8 sObjectRegistration;
static u8 sAllocatorRegistration;
static hdSaveDataState_l sCancelState;
static hdSaveDataState_l sCancelKeyboardState;
static hdSaveDataState_l sConfirmationHideState;
static hdSaveDataState_l sFileSelectEnterState;
static hdSaveDataState_l sFileSelectWaitState;
static hdSaveDataState_l sFileSelectChoiceState;
static hdSaveDataState_l sEndSelectState;
static hdSaveDataState_l sControllerSelectState;
static hdSaveDataState_l sSaveSelectState;
static hdSaveDataState_l sNameInputState;
static hdSaveDataState_l sWaitEndSelectState;
static hdSaveDataState_l sConfirmationWaitState;
static hdSaveDataState_l sCheckDataState;
static hdSaveDataCursorModes_l sCursorModes;
static hdSaveDataState_l sConfirmationBlankState;
static hdSaveDataState_l sSaveScreenWaitState;
enum { kStateIdentitySlot = 0x14 / 4, kScreenUpdateSlot = 0x5C / 4, kScreenDrawSlot = 0x6C / 4, kTaskInitializeSlot = 0x14 / 4, kNameEnsureSlot = 0x14 / 4 };
/* inlined into the SaveData state handlers */
static bool saveDataStateEquals(hdStateMachine* machine, hdSaveDataState_l* descriptor) {
    hdSaveDataStateId_l* current = saveDataCurrentStateId(machine);
    u32 target = current->mpVtable[kStateIdentitySlot].get();
    be<u32>* descriptorVtable = descriptor->mpVtable;
    u32 currentId = gabi::call_ptr<u32>(target, current);
    u32 descriptorId = gabi::call_ptr<u32>(descriptorVtable[kStateIdentitySlot].get(), descriptor);
    return currentId == descriptorId;
}
/* inlined into the SaveData screen handlers */
static void saveDataUpdateUI(void* object) {
    hdSaveDataUI_l* screen = (hdSaveDataUI_l*)object;
    gabi::call_ptr<void>(screen->mpVtable[kScreenUpdateSlot].get(), screen);
}
/* inlined into the SaveData render handlers */
static void saveDataDrawUI(void* object, void* context) {
    hdSaveDataUI_l* screen = (hdSaveDataUI_l*)object;
    gabi::call_ptr<void>(screen->mpVtable[kScreenDrawSlot].get(), screen, context);
}
/* inlined into the SaveData render handlers */
static void* saveDataRenderContext(hdSaveDataDrawInfo_l* info) {
    hdSaveDataRenderArray_l* array = sRenderArray;
    u32 index = info->mSlot;
    u32 count = array->mCount;
    gptr<hdSaveDataRenderSlot_l>* slots = array->mpSlots;
    hdSaveDataRenderSlot_l* slot = slots[index < count ? index : 0];
    return slot->mRenderContext;
}
/* 027095D0 */
u32 hdSaveDataScene_c::receiveTouch() { return 0; }
/* 027095DC */
u32 hdSaveDataScene_c::receiveKey() { return 0; }
/* 027095F0 */
u32 hdSaveDataScene_c::receiveController() { return 0; }
/* 027095F8 */
static u32 saveDataStateIdValue(hdSaveDataStateId_l* id) { return id->mId; }
/* 0270A064 */
void hdSaveDataScene_c::hideBackground() {
    sNameEntryActive = 0;
    background()->requestOut();
}
/* 0270AD6C */
void hdSaveDataScene_c::clearBackRequest() { mpFileSelect->mCancelSoundPlayed = 0; }
/* 0270AE4C */
void hdSaveDataScene_c::enterNameEntry() { sNameEntryActive = 1; }
/* 0270B6E4 */
void hdSaveDataScene_c::enterControllerSelect() {
    enterNameEntry();
    saveScreen()->open(4, 0);
}
/* 0270B714 */
void hdSaveDataScene_c::enterDifficultySelect() {
    enterNameEntry();
    saveScreen()->open(7, 0);
}
/* 0270B744 */
void hdSaveDataScene_c::enterFinish() {
    enterNameEntry();
    void* screen = mpSaveDataScreen;
    mFinishing = 1;
    static_cast<hdScreenSystemWindow_c*>(screen)->open(0x11, 0);
}
/* 0270AAAC */
void hdSaveDataScene_c::beginControllerChoice() {
    sInputManager->setMode(3);
    mPhase = 0;
}
/* 0270AAEC */
void hdSaveDataScene_c::resetEventMode() {
    if (sInput->mEnabled != 0) sEventManager->setModeDefault(0);
}
/* 0270AE5C */
void hdSaveDataScene_c::enterLoad() {
    sSaveTransition = 0;
    enterNameEntry();
    sKeyboard->mMode = 0;
    sKeyboard->open();
    sKeyboard->layoutCall(0);
    mPhase = 0;
}
/* 0270B448 */
void hdSaveDataScene_c::leaveLoad() {
    mpFileSelect->refreshSlots();
    sKeyboard->close();
    hideBackground();
}
/* 0270B50C */
void hdSaveDataScene_c::beginGame() {
    sEventManager->reset();
    hdSaveInfoHolder_c* save = sSaveInfo;
    u8 slot = saveDataSelectedSlot(mpFileSelect);
    save->mSaveInfo.mDataNum = slot;
}
/* 0270B5B4 */
static void saveDataBeginPlay(hdSaveDataScene_c* scene) {
    scene->resetEventMode();
    sSaveTransition = 0;
    sNameEntryActive = 0;
    saveDataGamePrepare();
    saveDataStartMusic(30);
}
/* 0270B5F4 */
void hdSaveDataScene_c::beginNameEntry() {
    u8 slot = sSaveInfo->mSaveInfo.mDataNum;
    saveDataBeginName(sNameScene, slot, 1);
    saveDataChangeState(&mState, &sEndSelectState);
}
/* 0270B554 */
void hdSaveDataScene_c::beginSlotLoad() {
    u32 slot = saveDataSelectedSlot(mpFileSelect);
    hdPictureSlotSummary_c* record = reinterpret_cast<hdPictureSlotSummaries_c*>(mpSaveHolder.get())->slot(slot);
    if (record != NULL) {
        mEmptySlot = record->mRupees == 0;
        saveDataChangeState(&mState, &sWaitEndSelectState);
    }
}
/* 0270B48C */
void hdSaveDataScene_c::updateSlotLoad() {
    if (sKeyboard->isHidden() != 0) {
        mLoading = 0;
        saveDataGetPlay()->mMenuKind = 23;
        saveDataGetPlay()->mMenuPage = 7;
        sSaveTransition = 3;
        mpFileSelect->showSlots();
        saveDataChangeState(&mState, &sSaveSelectState);
    }
}
/* 0270AD7C */
void hdSaveDataScene_c::updateCancel() {
    // HD: the screen's selection indexes the controller-cursor mode table.
    u32 cursor = mpFileSelect->mMessage;
    if (cursor <= 12) static_cast<hdTitleStartBackground_c*>(mpCursor.get())->setCaptionIndex(sCursorModes.mModes[cursor].get());
}
/* 0270ADA0 */
void hdSaveDataScene_c::updateSaveSelection() {
    dFile_select_c* screen = mpFileSelect;
    if (screen->mCancelSoundPlayed != 0) {
        saveDataChangeState(&mState, &sControllerSelectState);
        resetEventMode();
        return;
    }
    updateCancel();
    screen = mpFileSelect;
    if (screen->selectEnd != 0) {
        saveDataChangeState(&mState, &sNameInputState);
        screen = mpFileSelect;
    }
    if (screen->mClose != 0) saveDataChangeState(&mState, &sCheckDataState);
}
/* 0270AA54 */
void hdSaveDataScene_c::selectSaveState() {
    hdStateOwner* owner = mState.mpOwner;
    hdStateObject* next = owner->createState((hdStateId*)&sControllerSelectState);
    mState.mpCurrentState = next;
    next->enter();
}
/* 0270B428 */
void hdSaveDataScene_c::enterCancelKeyboard() {
    sSaveTransition = 0;
    saveDataChangeState(mpConfirmation->stateMachine(), &sConfirmationWaitState);
}
/* 0270BCF4 */
u8 hdSaveDataScene_c::isSaveSelection() {
    return saveDataStateEquals(&mState, &sControllerSelectState) || saveDataStateEquals(&mState, &sSaveSelectState);
}
/* 02709FC4 */
u8 hdSaveDataScene_c::isSaveScreenReady() {
    hdSequenceWindow_c* screen = (hdSequenceWindow_c*)mpSaveDataScreen.get();
    if (saveDataStateEquals(&screen->mStateHolder.mMachine, &sSaveScreenWaitState)) return 1;
    return screen->mAdvanceRequested != 0;
}
/* 0270BDD0 */
void hdSaveDataScene_c::updateInputAvailability() {
    if (sInput->mEnabled == 0 && saveDataStateEquals(mpConfirmation->stateMachine(), &sConfirmationBlankState)) {
        sInput->mEnabled = 1;
    }
}
/* 0270A078 */
void hdSaveDataScene_c::updateScreens() {
    saveDataUpdateUI(mpController);
    saveDataUpdateUI(mpCursor);
    saveDataUpdateUI(mpConfirmation);
    if (mFinishing != 0) {
        if (isSaveScreenReady() != 0) {
            dFile_select_c* screen = mpFileSelect;
            mFinishing = 0;
            screen->deleteSound();
            sInput->mEnabled = 1;
            hideBackground();
            saveDataUpdateUI(mpSaveDataScreen);
            sKeyboard->calc();
            sCursorMgr->execute();
            saveDataUpdateUI(mpBackground);
            return;
        }
    } else if (!saveDataStateEquals(&mState, &sNameInputState)) {
        saveDataUpdateUI(mpFileSelect);
    }
    saveDataUpdateUI(mpSaveDataScreen);
    sKeyboard->calc();
    sCursorMgr->execute();
    saveDataUpdateUI(mpBackground);
}
/* 0270A218 */
void hdSaveDataScene_c::execute() {
    gabi::call_ptr<void>(mpTaskVtable[0x2C / 4].get(), this);
    saveDataMachineUpdate(&mState);
    updateScreens();
}
/* 0270A468 */
void hdSaveDataScene_c::draw(hdSaveDataDrawInfo_l* info) {
    void* context = saveDataRenderContext(info);
    saveDataDrawUI(mpCursor, context);
    if (saveDataStateEquals(&mState, &sControllerSelectState) || saveDataStateEquals(&mState, &sCancelState)) {
        saveDataDrawUI(mpFileSelect, context);
        saveDataDrawUI(mpConfirmation, context);
    } else if (gabi::call_ptr<u32>(mpTaskVtable[0x1C / 4].get(), this) != 0) {
        saveDataDrawUI(mpFileSelect, context);
    }
}
/* 0270A600 */
void hdSaveDataScene_c::drawTv(hdSaveDataDrawInfo_l* info) {
    saveDataSetShaderMode(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    draw(info);
}
/* 0270A660 */
void hdSaveDataScene_c::drawTvFore(hdSaveDataDrawInfo_l* info) {
    saveDataSetShaderMode(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    void* context = saveDataRenderContext(info);
    saveDataDrawUI(mpBackground, context);
    if (saveDataStateEquals(&mState, &sNameInputState) || saveDataStateEquals(&mState, &sCancelKeyboardState) || mLoading != 0) {
        sKeyboard->drawLayouts(gabi::ea(saveDataRenderContext(info)));
    }
    if (mNewGame == 0) saveDataDrawKeyboardTv();
    saveDataDrawUI(mpSaveDataScreen, context);
    if (gabi::call_ptr<u32>(mpTaskVtable[0x24 / 4].get(), this) != 0) sCursorMgr->draw(context);
    saveDataDrawUI(mpController, context);
}
/* 0270A830 */
void hdSaveDataScene_c::drawDrc(hdSaveDataDrawInfo_l* info) {
    saveDataSetShaderMode(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    draw(info);
}
/* 0270A890 */
void hdSaveDataScene_c::drawDrcFore(hdSaveDataDrawInfo_l* info) {
    saveDataSetShaderMode(0, 0x30, 0x40, 0, 0, 0xC8, 0xC0);
    void* context = saveDataRenderContext(info);
    saveDataDrawUI(mpBackground, context);
    if (saveDataStateEquals(&mState, &sNameInputState) || saveDataStateEquals(&mState, &sCancelKeyboardState) || mLoading != 0) {
        sKeyboard->drawLayouts(gabi::ea(saveDataRenderContext(info)));
    }
    saveDataDrawKeyboardDrc();
    saveDataDrawUI(mpSaveDataScreen, context);
    if (gabi::call_ptr<u32>(mpTaskVtable[0x24 / 4].get(), this) != 0) sCursorMgr->draw(context);
    saveDataDrawUI(mpController, context);
}
/* 0270BAF8 */
u8 hdSaveDataScene_c::isFileOperation() {
    return saveDataStateEquals(&mState, &sSaveSelectState) ||
           saveDataStateEquals(&mState, &sCheckDataState) ||
           saveDataStateEquals(&mState, &sNameInputState) ||
           saveDataStateEquals(&mState, &sWaitEndSelectState) ||
           saveDataStateEquals(&mState, &sEndSelectState) ||
           saveDataStateEquals(&mState, &sCancelKeyboardState);
}
/* 027096CC */
void hdSaveDataScene_c::unloadLayouts() {
    hdSaveDataSafeString_l name;
    name.mpText.v = gabi::guest_str("SelectController_00");
    name.mVtable = gabi::ea(&sSafeStringVtable);
    saveDataLayoutRelease(sLayoutManager, &name);
    name.mVtable = gabi::ea(&sSafeStringVtable);
    name.mpText.v = gabi::guest_str("BtnController_00");
    saveDataLayoutRelease(sLayoutManager, &name);
    name.mVtable = gabi::ea(&sSafeStringVtable);
    name.mpText.v = gabi::guest_str("StartSequeneBG_00");
    saveDataLayoutRelease(sLayoutManager, &name);
    name.mVtable = gabi::ea(&sSafeStringVtable);
    name.mpText.v = gabi::guest_str("SaveData_00");
    saveDataLayoutRelease(sLayoutManager, &name);
    name.mVtable = gabi::ea(&sSafeStringVtable);
    name.mpText.v = gabi::guest_str("BtnSaveData_00");
    saveDataLayoutRelease(sLayoutManager, &name);
    name.mVtable = gabi::ea(&sSafeStringVtable);
    name.mpText.v = gabi::guest_str("BtnDifficulty_00");
    saveDataLayoutRelease(sLayoutManager, &name);
}
/* inlined into destroyScreens */
static void saveDataReleaseUI(hdSaveDataUI_l* screen) {
    gabi::call_ptr<void>(screen->mpVtable[0x54 / 4].get(), screen);
}
/* inlined into destroyScreens */
static void saveDataDeleteUI(hdSaveDataUI_l* screen) {
    if (screen != NULL) gabi::call_ptr<void>(screen->mpVtable[0xC / 4].get(), screen, 3u);
}
/* 0270A260 */
void hdSaveDataScene_c::destroyScreens() {
    saveDataReleaseUI((hdSaveDataUI_l*)mpController.get());
    saveDataDeleteUI((hdSaveDataUI_l*)mpController.get());
    saveDataReleaseUI((hdSaveDataUI_l*)mpBackground.get());
    saveDataDeleteUI((hdSaveDataUI_l*)mpBackground.get());
    sCursorMgr->destroyCursors();
    hdCursorMgr_c::deleteInstance();
    saveDataReleaseUI((hdSaveDataUI_l*)mpConfirmation.get());
    saveDataDeleteUI((hdSaveDataUI_l*)mpConfirmation.get());
    saveDataReleaseUI((hdSaveDataUI_l*)mpCursor.get());
    saveDataDeleteUI((hdSaveDataUI_l*)mpCursor.get());
    saveDataReleaseUI((hdSaveDataUI_l*)mpSaveDataScreen.get());
    saveDataDeleteUI((hdSaveDataUI_l*)mpSaveDataScreen.get());
    saveDataReleaseUI((hdSaveDataUI_l*)mpFileSelect.get());
    saveDataDeleteUI((hdSaveDataUI_l*)mpFileSelect.get());
}
/* 0270A3D0 */
void hdSaveDataScene_c::finish() {
    sEventManager->reset();
    sGameInfo->mSceneMode = 3;
    destroyScreens();
    sInput->mEnabled = 0;
    unloadLayouts();
    void* holders = mpSaveHolder;
    if (holders != NULL) {
        saveDataDestroyArray(holders, 3, 0x48, gabi::at<void>(gabi::guest_fn((const void*)&saveDataHolderDestroy)), 0, 0);
        operator_delete(holders);
    }
}
/* inlined into saveDataSceneConstruct */
static void saveDataInitializeListener(hdSaveDataListener_l* listener) {
    saveDataListenerConstruct(listener);
    hdSaveDataSafeString_l* name = (hdSaveDataSafeString_l*)&listener->mpName;
    if (gabi::ea(name) == 0) name = (hdSaveDataSafeString_l*)operator_new(8);
    if (gabi::ea(name) != 0) {
        name->mpText.v = gabi::guest_str("");
        name->mVtable = gabi::ea(&sSafeStringVtable);
    }
    listener->mEnabled = 0;
    listener->mVtable = gabi::ea(&sListenerVtable);
    listener->mpUser = NULL;
    if (gabi::ea(&listener->mDelegateVtable) != 0) {
        listener->mFunction = 0;
        listener->mDelegateVtable = gabi::ea(&sDelegateBaseVtable);
        listener->mThisAdjustment = 0;
        listener->mVirtualIndex = 0;
        listener->mpObject = NULL;
        listener->mDelegateVtable = gabi::ea(&sDelegateEmptyVtable);
    }
}
/* 0270B780 */
hdSaveDataScene_c* saveDataSceneConstruct(hdSaveDataScene_c* scene) {
    if (scene == NULL) {
        scene = (hdSaveDataScene_c*)operator_new(sizeof(hdSaveDataScene_c));
        if (scene == NULL) return NULL;
    }
    scene->mpStateOwner = scene;
    scene->mpStateId = NULL;
    scene->mCallbackStateVtable = gabi::ea(&sCallbackStateVtable);
    scene->mFactoryVtable = gabi::ea(&sStateFactoryVtable);
    scene->mpTaskVtable = (gfn*)&sBaseTaskVtable;
    saveDataMachineConstruct(&scene->mState, scene, &sInitialStateFactory);
    scene->mpSaveDataScreen = NULL;
    scene->mpTaskVtable = (gfn*)&sTaskVtable;
    saveDataInitializeListener(&scene->mTvListener);
    saveDataInitializeListener(&scene->mTvForeListener);
    saveDataInitializeListener(&scene->mDrcListener);
    saveDataInitializeListener(&scene->mDrcForeListener);
    scene->mpCursor = NULL;
    scene->mPhase = 0;
    scene->mNewGame = 1;
    scene->mFinishing = 0;
    scene->mLoading = 0;
    scene->mpFileSelect = NULL;
    scene->mpBackground = NULL;
    scene->mpSaveHolder = NULL;
    scene->mEmptySlot = 0;
    scene->mpController = NULL;
    scene->mpConfirmation = NULL;
    return scene;
}
/* 0270BA48 */
void saveDataSceneDestroy(hdSaveDataScene_c* scene, u32 flags) {
    if (scene != NULL) {
        saveDataListenerDestroy(&scene->mDrcForeListener, 2);
        saveDataListenerDestroy(&scene->mDrcListener, 2);
        saveDataListenerDestroy(&scene->mTvForeListener, 2);
        saveDataListenerDestroy(&scene->mTvListener, 2);
        hdStateOwner* owner = scene->mState.mpOwner;
        owner->onStateChange(&scene->mState.mpPreviousState);
        owner = scene->mState.mpOwner;
        owner->onStateChange(&scene->mState.mpCurrentState);
        if (flags & 1) operator_delete(scene);
    }
}
/* 0270BE70 */
static hdSaveDataScene_c* saveDataSceneCreate(void* heap) {
    hdSaveDataScene_c* scene = sSaveDataScene;
    if (scene == NULL) {
        hdSaveDataSingleton_l* singleton = (hdSaveDataSingleton_l*)saveDataHeapAllocate(0x160, heap, 4);
        scene = &singleton->mScene;
        hdSaveDataDisposer_l* disposer = &singleton->mDisposer;
        if (disposer != NULL) {
            saveDataDisposerConstruct(disposer, heap, 3);
            disposer->mVtable = gabi::ea(&sDisposerVtable);
        }
        sSaveDataDisposer = disposer;
        if (scene != NULL) {
            saveDataSceneConstruct(scene);
            scene->mpTaskVtable = (gfn*)&sSingletonTaskVtable;
        }
        sSaveDataScene = scene;
    }
    return scene;
}
/* 0270BF24 */
static void saveDataSceneDelete() {
    hdSaveDataDisposer_l* disposer = sSaveDataDisposer;
    if (disposer != NULL) {
        sSaveDataDisposer = NULL;
        gabi::call_ptr<void>(gabi::at<be<u32>>(disposer->mVtable)[3].get(), disposer, 2u);
        hdSaveDataScene_c* scene = sSaveDataScene;
        if (scene != NULL) gabi::call_ptr<void>(scene->mpTaskVtable[3].get(), scene, 3u);
        sSaveDataScene = NULL;
    }
}
/* 0270BFA4 */
static void saveDataSceneDisposerDestroy(hdSaveDataDisposer_l* disposer, u32 flags) {
    if (disposer != NULL) {
        disposer->mVtable = gabi::ea(&sDisposerVtable);
        if (disposer == sSaveDataDisposer) {
            hdSaveDataScene_c* scene = sSaveDataScene;
            sSaveDataDisposer = NULL;
            gabi::call_ptr<void>(scene->mpTaskVtable[3].get(), scene, 2u);
            sSaveDataScene = NULL;
        }
        saveDataDisposerDestroy(disposer, 0);
        if (flags & 1) operator_delete(disposer);
    }
}
/* 0270AB10 */
void hdSaveDataScene_c::updateControllerChoice() {
    if (mPhase == 0) {
        // HD: wait until the file screen has left both of its transition states.
        hdStateMachine* machine = (hdStateMachine*)((u8*)mpFileSelect.get() + 0x18);
        if (!saveDataStateEquals(machine, &sFileSelectEnterState)) {
            machine = (hdStateMachine*)((u8*)mpFileSelect.get() + 0x18);
            if (!saveDataStateEquals(machine, &sFileSelectWaitState)) return;
        }
        mPhase = u32(mPhase) + 1;
        static_cast<hdTitleStartBackground_c*>(mpCursor.get())->setCaptionIndex(0);
        hdTitleControllerSelection_c* confirmation = mpConfirmation;
        confirmation->mInAnimationFinished = 0;
        saveDataChangeState(confirmation->stateMachine(), &sConfirmationHideState);
        saveDataGetPlay()->mMenuKind = 23;
        saveDataGetPlay()->mMenuPage = 7;
        sSaveTransition = 3;
        return;
    }
    hdTitleControllerSelection_c* confirmation = mpConfirmation;
    if (confirmation->mSelectionPending != 0) {
        saveDataChangeState(&mState, &sCancelState);
        resetEventMode();
        return;
    }
    if (confirmation->mInAnimationFinished != 0) {
        resetEventMode();
        if (reinterpret_cast<hdTitleControllerScreen_c*>(mpConfirmation.get())->isPrimarySelected() != 0) {
            mNewGame = 1;
            sInputManager->setMode(1);
        } else {
            mNewGame = 0;
            sInputManager->setMode(0);
        }
        sInput->mEnabled = 1;
        static_cast<hdTitleStartBackground_c*>(mpCursor.get())->setCaptionIndex(1);
        saveDataChangeState((u8*)mpFileSelect.get() + 0x18, &sFileSelectChoiceState);
        saveDataChangeState(&mState, &sSaveSelectState);
    }
}
/* inlined into hdSaveDataScene_c::initialize */
static void saveDataBindListener(hdSaveDataListener_l* listener, hdSaveDataScene_c* scene, u32 callback) {
    if (gabi::ea(&listener->mDelegateVtable) != 0) {
        listener->mDelegateVtable = gabi::ea(&sDelegateVtable);
        listener->mpObject = scene;
        listener->mVirtualIndex = 0xFFFF;
        listener->mFunction = callback;
        listener->mThisAdjustment = 0;
    }
}
/* inlined into hdSaveDataScene_c::initialize */
static hdSaveDataRenderSlot_l* saveDataListenerSlot(u32 index) {
    hdSaveDataRenderArray_l* array = sRenderArray;
    u32 count = array->mCount;
    gptr<hdSaveDataRenderSlot_l>* slots = array->mpSlots;
    return slots[count > index ? index : 0];
}
/* 02709D24 */
void hdSaveDataScene_c::initialize() {
    sInputManager->setMode(3);
    sGameInfo->mSceneMode = 1;
    saveDataBindListener(&mTvListener, this, gabi::guest_fn((const void*)&saveDataDrawTvCallback));
    mTvListener.mEnabled = 1;
    mTvListener.mpName.v = gabi::guest_str("Tv");
    saveDataRegisterListener(saveDataListenerSlot(3), 3, &mTvListener);
    saveDataBindListener(&mTvForeListener, this, gabi::guest_fn((const void*)&saveDataDrawTvForeCallback));
    mTvForeListener.mEnabled = 1;
    mTvForeListener.mpName.v = gabi::guest_str("TvFore");
    saveDataRegisterListener(saveDataListenerSlot(5), 0, &mTvForeListener);
    saveDataBindListener(&mDrcListener, this, gabi::guest_fn((const void*)&saveDataDrawDrcCallback));
    mDrcListener.mEnabled = 1;
    mDrcListener.mpName.v = gabi::guest_str("Drc");
    saveDataRegisterListener(saveDataListenerSlot(7), 1, &mDrcListener);
    saveDataBindListener(&mDrcForeListener, this, gabi::guest_fn((const void*)&saveDataDrawDrcForeCallback));
    mDrcForeListener.mEnabled = 1;
    mDrcForeListener.mpName.v = gabi::guest_str("DrcFore");
    saveDataRegisterListener(saveDataListenerSlot(9), 0, &mDrcForeListener);
    mpSaveHolder = saveDataHolderCreate(NULL);
    createScreens();
    saveDataPrepareInput();
    saveDataPrepareAudio();
    saveDataSetAudioMode(2);
    for (u32 slot = 0; slot < 3; ++slot) {
        hdPictureSlotSummary_c* record = reinterpret_cast<hdPictureSlotSummaries_c*>(mpSaveHolder.get())->slot(slot);
        if (record->mRupees == 0 && record->mPlayerState == 0) saveDataClearSlot(sNameScene, slot);
    }
    gabi::call_ptr<void>(mpTaskVtable[kTaskInitializeSlot].get(), this);
}
/* inlined into hdSaveDataScene_c::createScreens */
static void saveDataResourceArguments(hdSaveDataRenderSlot_l* slot, void*& resource, void*& movie) {
    u32 flags = slot->mFlags;
    if (flags & (1u << 12)) resource = slot->mpCustomResource;
    else {
        resource = slot->mpResource;
        if (resource == NULL) resource = &sFallbackResource;
    }
    if ((flags & (1u << 6)) && (flags & (1u << 7))) movie = slot->mCustomMovie;
    else {
        movie = slot->mpMovie;
        if (movie == NULL) movie = &sFallbackMovie;
    }
}
/* 027097AC */
void hdSaveDataScene_c::createScreens() {
    hdSaveDataRenderSlot_l* slot = saveDataListenerSlot(3);
    void* object = operator_new(0xE4);
    if (object != NULL) object = hdCommandButtonGuide_c::construct(static_cast<hdCommandButtonGuide_c*>(object));
    mpController = object;
    void* resource;
    void* movie;
    saveDataResourceArguments(slot, resource, movie);
    static_cast<hdCommandButtonGuide_c*>(object)->initializeButtonGuide(gabi::ea(resource), gabi::ea(movie), NULL);
    sControllerVisible = 1;
    sControllerSelection = 0xFF;
    saveDataGetPlay()->mMenuKind = 23;
    saveDataGetPlay()->mMenuPage = 7;
    sSaveTransition = 3;
    sControllerVisible = 1;
    static_cast<hdCommandButtonGuide_c*>(mpController.get())->hideMessageGroup();
    object = operator_new(0x60);
    if (object != NULL) object = hdTitleStartBackground_c::construct(static_cast<hdTitleStartBackground_c*>(object));
    mpCursor = object;
    saveDataResourceArguments(slot, resource, movie);
    static_cast<hdTitleStartBackground_c*>(object)->initialize(gabi::ea(resource), gabi::ea(movie), NULL);
    object = operator_new(0xFC);
    if (object != NULL) object = hdBlurBackground_c::construct(static_cast<hdBlurBackground_c*>(object));
    mpBackground = object;
    saveDataResourceArguments(slot, resource, movie);
    static_cast<hdBlurBackground_c*>(object)->load(gabi::ea(resource), gabi::ea(movie), 0);
    object = operator_new(0xDC);
    if (object != NULL) object = hdSequenceWindow_c::construct(static_cast<hdSequenceWindow_c*>(object));
    mpSaveDataScreen = object;
    saveDataResourceArguments(slot, resource, movie);
    static_cast<hdScreenSystemWindow_c*>(object)->init(gabi::ea(resource), gabi::ea(movie), NULL);
    hdCursorMgr_c::createInstance(NULL);
    saveDataResourceArguments(slot, resource, movie);
    sCursorMgr->create(gabi::ea(resource), gabi::ea(movie));
    sCursorMgr->mActive = 0;
    hdTitleControllerSelection_c* confirmation = (hdTitleControllerSelection_c*)operator_new(0x68);
    if (confirmation != NULL) confirmation = hdTitleControllerSelection_c::construct(confirmation);
    mpConfirmation = confirmation;
    saveDataResourceArguments(slot, resource, movie);
    reinterpret_cast<hdTitleControllerScreen_c*>(confirmation)->initialize(gabi::ea(resource), gabi::ea(movie), NULL);
    dFile_select_c* screen = saveDataFileSelectConstruct(NULL, mpSaveHolder);
    mpFileSelect = screen;
    saveDataResourceArguments(slot, resource, movie);
    screen->_create(gabi::ea(resource), gabi::ea(movie), 0);
    saveDataResourceArguments(slot, resource, movie);
    sKeyboard->setBoth(gabi::ea(resource), gabi::ea(movie));
}
/* 0270B648 */
void hdSaveDataScene_c::updateLoadedFlags() {
    sCommentManager->reset();
    saveDataRequestCommentDownload(sOperationManager);
    dFile_select_c* screen = mpFileSelect;
    u32 slot = saveDataSelectedSlot(screen);
    u32 loaded = saveDataSelectedSlotLoaded(screen);
    hdSaveSlotFlags_c* flags = sSaveInfo->mHolders.getSlotFlags(slot);
    flags->setLoaded(loaded);
    loaded = saveDataSelectedSlotLoaded(mpFileSelect);
    flags = sSaveInfo->mHolders.getScratchFlags();
    flags->setLoaded(loaded);
}
/* inlined into __sinit_hd_scene_save_data_cpp */
static void saveDataInitializeState(hdSaveDataState_l* state, u32 id, const char* name, u32 enter, u32 execute, u32 leave, u32 event) {
    state->mId = id;
    state->mpName.v = gabi::guest_str(name);
    state->mpVtable = (be<u32>*)&sStateVtable;
    u32 functions[4] = {enter, execute, leave, event};
    for (u32 index = 0; index < 4; ++index) {
        state->mCallbacks[index].mThisAdjustment = 0;
        state->mCallbacks[index].mVirtualIndex = 0xFFFF;
        state->mCallbacks[index].mFunction = functions[index];
    }
}
/* 0270C04C */
static void __sinit_hd_scene_save_data_cpp() {
    for (u32 i = 0; i != 4; ++i) sHeaderZeros.mValues[3 - i] = 0;
    __register_global_object(gabi::ea(&sHeaderRegistration));
    sAngleRange.mValues[0] = sAngleConstants.mValues[0];
    sAngleRange.mValues[1] = sAngleConstants.mValues[1];
    saveDataHeaderObjectConstruct(&sHeaderObject);
    __register_global_object(gabi::ea(&sObjectRegistration));
    saveDataHeaderAllocatorConstruct(&sHeaderAllocator);
    __register_global_object(gabi::ea(&sAllocatorRegistration));
    sFarBounds.mX = 50000.0f;
    sNearBounds.mX = 10000.0f;
    sFarBounds.mY = 50000.0f;
    sNearBounds.mY = 10000.0f;
    sDefaultNameKey.mVtable = gabi::ea(&sSafeStringVtable);
    sDefaultNameKey.mpText.v = gabi::guest_str("Defalt_Name00");
    sSaveDataLayoutKey.mVtable = gabi::ea(&sSafeStringVtable);
    sSaveDataLayoutKey.mpText.v = gabi::guest_str("SaveData_00");
    u32 id;
    if (sStateIdsInitialized != 0) id = sStateIdCounter;
    else { sStateIdsInitialized = 1; id = 0; }
    saveDataInitializeState(&sControllerSelectState, ++id, "::StateID_ControllerSelect", gabi::guest_fn((const void*)&saveDataControllerSelectEnterCallback), gabi::guest_fn((const void*)&saveDataControllerSelectExecuteCallback), gabi::guest_fn((const void*)&saveDataControllerSelectLeaveCallback), gabi::guest_fn((const void*)&saveDataControllerSelectEventCallback));
    saveDataInitializeState(&sSaveSelectState, ++id, "::StateID_SaveDataSelect", gabi::guest_fn((const void*)&saveDataSaveDataSelectEnterCallback), gabi::guest_fn((const void*)&saveDataSaveDataSelectExecuteCallback), gabi::guest_fn((const void*)&saveDataSaveDataSelectLeaveCallback), gabi::guest_fn((const void*)&saveDataSaveDataSelectEventCallback));
    saveDataInitializeState(&sNameInputState, ++id, "::StateID_NameInput", gabi::guest_fn((const void*)&saveDataNameInputEnterCallback), gabi::guest_fn((const void*)&saveDataNameInputExecuteCallback), gabi::guest_fn((const void*)&saveDataNameInputLeaveCallback), gabi::guest_fn((const void*)&saveDataNameInputEventCallback));
    saveDataInitializeState(&sCancelState, ++id, "::StateID_Cancel", gabi::guest_fn((const void*)&saveDataCancelEnterCallback), gabi::guest_fn((const void*)&saveDataCancelExecuteCallback), gabi::guest_fn((const void*)&saveDataCancelLeaveCallback), gabi::guest_fn((const void*)&saveDataCancelEventCallback));
    saveDataInitializeState(&sCancelKeyboardState, ++id, "::StateID_CancelKeyboard", gabi::guest_fn((const void*)&saveDataCancelKeyboardEnterCallback), gabi::guest_fn((const void*)&saveDataCancelKeyboardExecuteCallback), gabi::guest_fn((const void*)&saveDataCancelKeyboardLeaveCallback), gabi::guest_fn((const void*)&saveDataCancelKeyboardEventCallback));
    saveDataInitializeState(&sCheckDataState, ++id, "::StateID_CheckData", gabi::guest_fn((const void*)&saveDataCheckDataEnterCallback), gabi::guest_fn((const void*)&saveDataCheckDataExecuteCallback), gabi::guest_fn((const void*)&saveDataCheckDataLeaveCallback), gabi::guest_fn((const void*)&saveDataCheckDataEventCallback));
    saveDataInitializeState(&sWaitEndSelectState, ++id, "::StateID_WaitEndSelect", gabi::guest_fn((const void*)&saveDataWaitEndSelectEnterCallback), gabi::guest_fn((const void*)&saveDataWaitEndSelectExecuteCallback), gabi::guest_fn((const void*)&saveDataWaitEndSelectLeaveCallback), gabi::guest_fn((const void*)&saveDataWaitEndSelectEventCallback));
    saveDataInitializeState(&sEndSelectState, ++id, "::StateID_EndSelect", gabi::guest_fn((const void*)&saveDataEndSelectEnterCallback), gabi::guest_fn((const void*)&saveDataEndSelectExecuteCallback), gabi::guest_fn((const void*)&saveDataEndSelectLeaveCallback), gabi::guest_fn((const void*)&saveDataEndSelectEventCallback));
    sStateIdCounter = id;
}
static u8 sNameStringVtable;
static u8 sNameBufferBaseVtable;
static u8 sNameBufferVtable;
static void* sTextManager;
/* SafeString operations inlined by the name-entry handler. */
static s32 saveDataNameLength(hdSaveDataNameString_l* string) {
    be<u16>* text = string->mpText;
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > 0x40000) return 0;
    }
    return length;
}
static void saveDataNameEnsure(hdSaveDataNameString_l* string) {
    gabi::call_ptr<void>(string->mpVtable[kNameEnsureSlot].get(), string);
}
static bool saveDataNameCharacterIs(hdSaveDataNameString_l* string, s32 index, u16 character) {
    saveDataNameEnsure(string);
    s32 length = saveDataNameLength(string);
    return index >= 0 && index <= length && string->mpText[index] == character;
}
/* 0270AEC8 */
void hdSaveDataScene_c::updateNameEntry() {
    auto frame = gabi::AutoFrame(0x0270AEC8);
    if (mPhase > 1) return;
    if (mPhase == 1) {
        if (sKeyboard->isHidden() != 0) {
            mLoading = 0;
            saveDataGetPlay()->mMenuKind = 23;
            saveDataGetPlay()->mMenuPage = 7;
            sSaveTransition = 3;
            mpFileSelect->slotStatus();
            mpFileSelect->reset();
            saveDataChangeState(&mState, &sSaveSelectState);
        }
        return;
    }
    if (saveDataKeyboardCancelled() != 0) saveDataChangeState(&mState, &sCancelKeyboardState);
    if (saveDataKeyboardAccepted() == 0) return;
    be<u16>* name = sKeyboard->getText();
    if (name[0] == 0) return;
    gabi::FrameLocal<hdSaveDataNameBuffer_l> buffer(8);
    gabi::FrameLocal<hdSaveDataNameString_l> source(0x28);
    source->mpText = name;
    source->mpVtable = (be<u32>*)&sNameStringVtable;
    buffer->mString.mpText = buffer->mCharacters;
    buffer->mString.mpVtable = (be<u32>*)&sNameBufferBaseVtable;
    buffer->mCapacity = 10;
    buffer->mCharacters[9] = 0;
    saveDataNameStringEnsure(source);
    s32 length = saveDataNameLength(source);
    if (length >= buffer->mCapacity) length = buffer->mCapacity - 1;
    saveDataNameEnsure(source);
    saveDataNameCopy(buffer->mCharacters, source->mpText, u32(length) * 2, 0);
    buffer->mCharacters[length] = 0;
    buffer->mString.mpVtable = (be<u32>*)&sNameBufferVtable;
    saveDataNameBufferTerminate(buffer);
    length = saveDataNameLength(&buffer->mString);
    if (length >= buffer->mCapacity) {
        saveDataNameEnsure(&buffer->mString);
        saveDataNameLength(&buffer->mString);
    } else {
        if (length < 0) length = 0;
        buffer->mString.mpText[length] = 0;
    }
    saveDataNameEnsure(&buffer->mString);
    u32 count = saveDataNameLength(&buffer->mString) + 1;
    bool blank = true;
    for (u32 index = 0; index != count; ++index) {
        if (saveDataNameCharacterIs(&buffer->mString, index, 0x8140)) continue;
        if (saveDataNameCharacterIs(&buffer->mString, index, 0x20)) continue;
        saveDataNameEnsure(&buffer->mString);
        length = saveDataNameLength(&buffer->mString);
        if (s32(index) >= 0 && s32(index) <= length && buffer->mString.mpText[index] != 0) blank = false;
    }
    if (blank) {
        gabi::FrameLocal<HdMsgText> parser(0x40);
        saveDataTextParserConstruct(parser);
        saveDataTextParserSetKey(parser, &sSaveDataLayoutKey, &sDefaultNameKey);
        saveDataParseText(sTextManager, parser, &parser->mSetName, &parser->mLabel);
        HdMsgSet* array = parser->mpMessageSet;
        u32 index = parser->mLabelIndex;
        u8 slot = saveDataSelectedSlot(mpFileSelect);
        be<u16>* text = NULL;
        if (index < array->mEntryCount) text = saveDataTextArrayElement(array->mpData, index);
        gabi::FrameLocal<hdSaveDataNameString_l> result(0x30);
        result->mpText = text;
        result->mpVtable = (be<u32>*)&sNameStringVtable;
        hdSaveName_l* record = sSaveInfo->mHolders.getName(slot);
        reinterpret_cast<hdPlayerName_c*>(record)->copyWideName(reinterpret_cast<hdNameString_c*>(result.get()));
    } else {
        u8 slot = saveDataSelectedSlot(mpFileSelect);
        gabi::FrameLocal<hdSaveDataNameString_l> result(0x38);
        result->mpText = name;
        result->mpVtable = (be<u32>*)&sNameStringVtable;
        hdSaveName_l* record = sSaveInfo->mHolders.getName(slot);
        reinterpret_cast<hdPlayerName_c*>(record)->copyWideName(reinterpret_cast<hdNameString_c*>(result.get()));
    }
    sKeyboard->close();
    hideBackground();
    saveDataNameFinishSound("SE_NAME_ENTRY_FINISH");
    mPhase = mPhase + 1;
}
/* WWHD glue
 data sHeaderZeros 1049EA1C
 data sAngleConstants 1013ECD8
 data sAngleRange 1049EA00
 data sHeaderObject 1049EA18
 data sHeaderAllocator 1049EA19
 data sHeaderRegistration 101F82C8
 data sObjectRegistration 101F82D4
 data sAllocatorRegistration 101F82E0
 ext 028ED6F8 void saveDataHeaderObjectConstruct(void*)
 ext 028EAB2C void saveDataHeaderAllocatorConstruct(void*)
 data sStateIdsInitialized 101FD8E4
 data sStateIdCounter 101FDD50
 data sStateVtable 1013EBE0
 data sDefaultNameKey 1049E9F8
 data sSaveDataLayoutKey 1049E9F0
 data sFarBounds 1049EA08
 data sNearBounds 1049EA10
 str 1013ED48
 str 1013ED00 in 0270C04C
 ext 0270AAAC void saveDataControllerSelectEnterCallback() callback
 ext 0270AB10 void saveDataControllerSelectExecuteCallback() callback
 ext 0270C5F0 void saveDataControllerSelectLeaveCallback() callback
 ext 0270C5F4 void saveDataControllerSelectEventCallback() callback
 str 1013ED58
 ext 0270AD6C void saveDataSaveDataSelectEnterCallback() callback
 ext 0270ADA0 void saveDataSaveDataSelectExecuteCallback() callback
 ext 0270C5FC void saveDataSaveDataSelectLeaveCallback() callback
 ext 0270C600 void saveDataSaveDataSelectEventCallback() callback
 str 1013ED74
 ext 0270AE5C void saveDataNameInputEnterCallback() callback
 ext 0270AEC8 void saveDataNameInputExecuteCallback() callback
 ext 0270C608 void saveDataNameInputLeaveCallback() callback
 ext 0270C60C void saveDataNameInputEventCallback() callback
 str 1013ED0C
 ext 0270B428 void saveDataCancelEnterCallback() callback
 ext 0270C614 void saveDataCancelExecuteCallback() callback
 ext 0270C618 void saveDataCancelLeaveCallback() callback
 ext 0270C61C void saveDataCancelEventCallback() callback
 str 1013ED90
 ext 0270B448 void saveDataCancelKeyboardEnterCallback() callback
 ext 0270B48C void saveDataCancelKeyboardExecuteCallback() callback
 ext 0270C624 void saveDataCancelKeyboardLeaveCallback() callback
 ext 0270C628 void saveDataCancelKeyboardEventCallback() callback
 str 1013EDA4
 ext 0270B50C void saveDataCheckDataEnterCallback() callback
 ext 0270B554 void saveDataCheckDataExecuteCallback() callback
 ext 0270C630 void saveDataCheckDataLeaveCallback() callback
 ext 0270C634 void saveDataCheckDataEventCallback() callback
 str 1013ED20
 ext 0270B5B4 void saveDataWaitEndSelectEnterCallback() callback
 ext 0270B5F4 void saveDataWaitEndSelectExecuteCallback() callback
 ext 0270C63C void saveDataWaitEndSelectLeaveCallback() callback
 ext 0270C640 void saveDataWaitEndSelectEventCallback() callback
 str 1013ECE8
 ext 0270B648 void saveDataEndSelectEnterCallback() callback
 ext 0270C648 void saveDataEndSelectExecuteCallback() callback
 ext 0270C64C void saveDataEndSelectLeaveCallback() callback
 ext 0270C650 void saveDataEndSelectEventCallback() callback
 str 1013ED34

 data sCommentManager 1018F4AC
 data sOperationManager 1018F504
 ext 0203A24C void OliveCommentMgr::reset()
 ext 0203C42C u32 saveDataRequestCommentDownload(void*)
 ext 026CE368 u32 saveDataSelectedSlotLoaded(dFile_select_c*)
 ext 027200A0 hdSaveSlotFlags_c* hdSaveHolders_c::getSlotFlags(u32)
 ext 027200D0 hdSaveSlotFlags_c* hdSaveHolders_c::getScratchFlags()
 ext 0271FC70 void hdSaveSlotFlags_c::setLoaded(u32)
 data sFallbackResource 104A20FC
 data sFallbackMovie 104A2098
 data sControllerVisible 1047B096
 data sControllerSelection 1047B07D
 ext 0264BC1C hdCommandButtonGuide_c* hdCommandButtonGuide_c::construct(hdCommandButtonGuide_c*)
 ext 0264BCB8 u32 hdCommandButtonGuide_c::initializeButtonGuide(u32, u32, hdPartsDesc_c*)
 ext 026496A0 void hdCommandGuide_c::hideMessageGroup()
 ext 026E5214 hdTitleStartBackground_c* hdTitleStartBackground_c::construct(hdTitleStartBackground_c*)
 ext 026E52AC u32 hdTitleStartBackground_c::initialize(u32, u32, hdPartsDesc_c*)
 ext 026258A4 hdBlurBackground_c* hdBlurBackground_c::construct(hdBlurBackground_c*)
 ext 02625928 u32 hdBlurBackground_c::load(u32, u32, u32)
 ext 026DAE94 hdSequenceWindow_c* hdSequenceWindow_c::construct(hdSequenceWindow_c*)
 ext 026DB5F4 u32 hdScreenSystemWindow_c::init(u32 arg1, u32 arg2, void* desc)
 ext 026534F4 void hdCursorMgr_c::createInstance(void*)
 ext 0265361C u32 hdCursorMgr_c::create(u32, u32)
 ext 026D4B08 hdTitleControllerSelection_c* hdTitleControllerSelection_c::construct(hdTitleControllerSelection_c*)
 ext 026D4EE0 u32 hdTitleControllerScreen_c::initialize(u32, u32, hdPartsDesc_c*)
 ext 026D1634 dFile_select_c* saveDataFileSelectConstruct(void*, void*)
 ext 026D1D14 u32 dFile_select_c::_create(u32, u32, u32)
 ext 02619FB0 void hdSwkbd_mgr_c::setBoth(u32, u32)
 data sDelegateVtable 1013EB88
 str 1013ECA0 in 02709D24
 str 1013ECA4
 str 1013EC9C in 02709D24
 str 1013EC94
 ext 0270A600 void saveDataDrawTvCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*) callback
 ext 0270A660 void saveDataDrawTvForeCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*) callback
 ext 0270A830 void saveDataDrawDrcCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*) callback
 ext 0270A890 void saveDataDrawDrcForeCallback(hdSaveDataScene_c*, hdSaveDataDrawInfo_l*) callback
 ext 027C23F8 void saveDataRegisterListener(hdSaveDataRenderSlot_l*, u32, void*)
 ext 02727190 void* saveDataHolderCreate(void*)
 ext 020310F4 void saveDataPrepareInput()
 ext 02030D44 void saveDataPrepareAudio()
 ext 020309AC void saveDataSetAudioMode(u32)
 ext 02721A04 void saveDataClearSlot(void*, u32)
 data sConfirmationHideState 1049AA74
 data sFileSelectEnterState 1049A6EC
 data sFileSelectWaitState 1049A5B8
 data sFileSelectChoiceState 1049A550
 ext 026D5120 u32 hdTitleControllerScreen_c::isPrimarySelected()
 data sSaveDataScene 101F82EC
 data sSaveDataDisposer 101F82F0
 data sSingletonTaskVtable 1013EAB8
 data sDisposerVtable 1013EDF0
 ext 0273B0D4 void* saveDataHeapAllocate(u32, void*, u32)
 ext 02752B0C void saveDataDisposerConstruct(void*, void*, u32)
 ext 02752BEC void saveDataDisposerDestroy(void*, u32)
 data sStateFactoryVtable 1013EB68
 data sCallbackStateVtable 1013EB28
 data sBaseTaskVtable 1013EAA8
 data sTaskVtable 1013EDC0
 data sListenerVtable 10169E3C
 data sDelegateBaseVtable 1013EA48
 data sDelegateEmptyVtable 1013EA90
 data sInitialStateFactory 101FF32C
 str 1013ECD4 in 0270B780
 ext 020062E8 void saveDataMachineConstruct(hdStateMachine*, void*, void*)
 ext 02752A84 void saveDataListenerConstruct(void*)
 ext 027C0804 void saveDataListenerDestroy(void*, u32)
 data sLayoutManager 101F7274
 data sGameInfo 101F86E8
 data sSafeStringVtable 1013EA60
 str 1013EC28
 str 1013EC5C
 str 1013EC48
 str 1013EC3C in 027096CC
 str 1013EC70
 str 1013EC80
 ext 026FBA38 void saveDataLayoutRelease(void*, hdSaveDataSafeString_l*)
 ext 0265370C void hdCursorMgr_c::destroyCursors()
 ext 02653588 void hdCursorMgr_c::deleteInstance()
 ext 028F0164 void saveDataDestroyArray(void*, u32, u32, void*, u32, u32)
 ext 0270C5DC void saveDataHolderDestroy(void*, u32) callback
 data sRenderArray 101F95D0
 data sCancelState 1049EAB0
 data sCancelKeyboardState 1049EADC
 ext C0006988 void saveDataSetShaderMode(u32, u32, u32, u32, u32, u32, u32)
 ext 02619D04 void hdSwkbd_mgr_c::drawLayouts(u32)
 ext 02619D74 void saveDataDrawKeyboardTv()
 ext 02619D78 void saveDataDrawKeyboardDrc()
 ext 026538A0 void hdCursorMgr_c::draw(void*)
 data sCursorMgr 101F5A44
 ext 026D244C u32 dFile_select_c::deleteSound()
 ext 02619B68 void hdSwkbd_mgr_c::calc()
 ext 0265381C void hdCursorMgr_c::execute()
 ext 02006364 void saveDataMachineUpdate(hdStateMachine*)
 data sConfirmationBlankState 1049AA44
 data sSaveScreenWaitState 1049DD60
 ext 02006478 hdSaveDataStateId_l* saveDataCurrentStateId(hdStateMachine*)
 data sControllerSelectState 1049EA2C
 data sSaveSelectState 1049EA58
 data sNameInputState 1049EA84
 data sConfirmationWaitState 1049AADC
 data sCheckDataState 1049EB08
 data sWaitEndSelectState 1049EB34
 data sCursorModes 1013ECAC
 ext 027271F0 hdPictureSlotSummary_c* hdPictureSlotSummaries_c::slot(u32)
 ext 0261A008 u32 hdSwkbd_mgr_c::isHidden()
 ext 025200D4 hdSaveDataPlay_l* saveDataGetPlay()
 ext 026D230C void dFile_select_c::showSlots()
 ext 026E559C void hdTitleStartBackground_c::setCaptionIndex(u32)
 data sSaveTransition 1047B07C
 data sInputManager 101F5088
 data sEventManager 101F4FF0
 data sInput 101F8378
 data sKeyboard 101F50D8
 data sSaveInfo 101F84DC
 data sNameScene 101F852C
 data sEndSelectState 1049EB60
 ext 02618094 u32 hdInputManager::setMode(u32)
 ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
 ext 026CCF48 void dFile_select_c::refreshSlots()
 ext 02619E84 void hdSwkbd_mgr_c::close()
 ext 02619D7C void hdSwkbd_mgr_c::open()
 ext 02619EE4 void hdSwkbd_mgr_c::layoutCall(u32)
 ext 02614FD8 void cking::ui::input::EventMgr::reset()
 ext 02721908 void saveDataBeginName(void*, u32, u32)
 ext 020063C0 void saveDataChangeState(void*, hdSaveDataState_l*)
 ext 02030D6C void saveDataGamePrepare()
 ext 025E1904 void saveDataStartMusic(u32)
 data sNameEntryActive 101F8351
 ext 02625A80 void hdBlurBackground_c::requestOut()
 ext 026DBA08 void hdScreenSystemWindow_c::open(s32 mode, u32 keepPlay)
 data sNameStringVtable 1013EA78
 data sNameBufferBaseVtable 1013EAE8
 data sNameBufferVtable 1013EB00
 data sTextManager 101F4AE8
 ext 02619ED4 u32 saveDataKeyboardCancelled()
 ext 02619ECC u32 saveDataKeyboardAccepted()
 ext 02619EDC be<u16>* hdSwkbd_mgr_c::getText()
 ext 0270C514 void saveDataNameStringEnsure(hdSaveDataNameString_l*)
 ext 0270C518 void saveDataNameBufferTerminate(hdSaveDataNameBuffer_l*)
 ext C0009988 void saveDataNameCopy(void*, void*, u32, u32)
 ext 025F65F4 void saveDataTextParserConstruct(HdMsgText*)
 ext 025F6D70 void saveDataTextParserSetKey(HdMsgText*, hdSaveDataSafeString_l*, hdSaveDataSafeString_l*)
 ext 025F50E8 void saveDataParseText(void*, HdMsgText*, void*, void*)
 ext 0273A2E8 be<u16>* saveDataTextArrayElement(void*, u32)
 ext 02720154 hdSaveName_l* hdSaveHolders_c::getName(u32)
 ext 027266CC void hdPlayerName_c::copyWideName(hdNameString_c*)
 ext 02030B38 void saveDataNameFinishSound(const char*)
 ext 026D2424 u32 dFile_select_c::slotStatus()
 ext 026D23DC void dFile_select_c::reset()
 str 1013ECBC
*/
