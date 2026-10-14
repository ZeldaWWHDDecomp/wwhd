/* HD EX save flow: explanation, save selection and exit state predicates. */
#include "d/hd_scene_ex_save.h"
#include "gc_api/m_Do/m_Do_audio.h"
static hdSceneExSave_c* sExInstance;
static hdSceneDisposer_c* sExDisposer;
static hdExSceneInput_l* sExInput;
static hdExSceneState_c sExExplainState;
static hdExSceneState_c sExSelectState;
static hdExSceneState_c sExEndState;
static hdPartsStateId_c sExMainEndState;
static hdPartsStateId_c sExFileSelectStartState;
static hdPartsStateIdVtbl_c sExStateVtable;
static hdScreenDeleteTable sExSceneVtable;
static hdScreenDeleteTable sExDisposerVtable;
static u8 sExQuestComplete;
static u8 sExSaveMode;
static hdSceneHeaderWords_c sExHeaderWords;
static hdSceneHeaderRange_c sExHeaderRange;
static hdSceneRegistration_c sExHeaderRegistration;
static hdSceneRegistration_c sExSignedRegistration;
static hdSceneRegistration_c sExUnsignedRegistration;
static f32 sExLower;
static f32 sExUpper;
static u32 sExStateGuard;
static u32 sExStateCounter;
static u32 sExExplainEnter;
static u32 sExExplainExecute;
static u32 sExExplainExit;
static u32 sExExplainCheck;
static u32 sExSelectEnter;
static u32 sExSelectExecute;
static u32 sExSelectExit;
static u32 sExSelectCheck;
static u32 sExEndEnter;
static u32 sExEndExecute;
static u32 sExEndExit;
static u32 sExEndCheck;
extern u32 hdExCurrentBgm();
extern hdPartsStateId_c* hdExGetState(hdStateMachine*);
// Canonical lifecycle is private in the merged TU; preserve its address-bound ABI.

/* 02708C54 */
void hdSceneExSave_c::enterExplainEX() {
    sExInput->mBlocked = 1;
    if (hdExCurrentBgm() != 0x8000001E) mDoAudioApi::bgmStart(0x8000001E);
}
/* 02708CA0 */
void hdSceneExSave_c::executeExplainEX() {
    hdSequenceWindow_c* parts = mainParts();
    hdPartsStateId_c* current = hdExGetState(parts->stateMachine());
    u32 currentTarget = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* targetTable = sExMainEndState.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentTarget, current);
    u32 targetKey = gabi::call_ptr<u32>(targetTable->mGetKey.get(), &sExMainEndState);
    if (currentKey == targetKey || parts->mAdvanceRequested) {
        resetEventMode();
        hdSequenceWindow_c* refreshedParts = mainParts();
        hdExSceneState_c* next = refreshedParts->mWaiting ? &sExSelectState : &sExEndState;
        hdParts_changeState(&mState, reinterpret_cast<hdPartsStateId_c*>(next));
    }
}
/* 02708D68 */
void hdSceneExSave_c::enterSelectSaveDataEX() {
    sExQuestComplete = 1;
    saveDataGetPlay()->mMenuKind = 0x17;
    saveDataGetPlay()->mMenuPage = 7;
    sExSaveMode = 3;
    shortcut()->showHeader();
    dFile_select_c* file = mpFileSelect;
    file->mUseType = 1;
    file->mCancelSoundPlayed = 0;
    hdStateMachine* state = gabi::at<hdStateMachine>(gabi::ea(file) + offsetof(dFile_select_c, mpStateProcessor));
    hdParts_changeState(state, &sExFileSelectStartState);
    updateCancel();
}
/* 02708E00 */
void hdSceneExSave_c::executeSelectSaveDataEX() {
    if (mpFileSelect->mCancelSoundPlayed) {
        shortcut()->requestHeaderState();
        mainParts()->open(0x10, 0);
        hdParts_changeState(&mState, reinterpret_cast<hdPartsStateId_c*>(&sExExplainState));
        return;
    }
    updateCancel();
    if (mpFileSelect->mClose) hdParts_changeState(&mState, reinterpret_cast<hdPartsStateId_c*>(&sExEndState));
}
/* 02708E8C */
void hdSceneExSave_c::restartExplainEX() {
    shortcut()->hideHeader();
    mainParts()->open(15, 0);
    hdStateOwner* owner = mState.mpOwner;
    hdStateObject* state = gabi::call_ptr<hdStateObject*>(owner->mpVTable->mCreateState.get(), owner, &sExExplainState);
    mState.mpCurrentState = state;
    state->enter();
}
/* 02708EFC */
void hdSceneExSave_c::blockInput() { sExInput->mBlocked = 1; }
/* 02708F10 */
hdSceneExSave_c* hdSceneExSave_c::construct(hdSceneExSave_c* self) {
    if (!self) {
        self = static_cast<hdSceneExSave_c*>(hdSceneAllocate(sizeof(hdSceneExSave_c)));
        if (!self) return NULL;
    }
    saveDataSceneConstruct(self);
    self->mpTaskVtable = reinterpret_cast<gfn*>(&sExSceneVtable);
    return self;
}
/* 02708F64 */
void hdSceneExSave_c::createInstance(void* heap) {
    if (sExInstance) return;
    hdSceneExSave_c* instance = static_cast<hdSceneExSave_c*>(hdSceneHeapAllocate(sizeof(hdSceneExSave_c), heap, 4));
    hdSceneDisposer_c* disposer = gabi::at<hdSceneDisposer_c>(gabi::ea(instance) + offsetof(hdSceneExSave_c, mDisposer));
    if (disposer) {
        hdSceneDisposerConstruct(disposer, heap, 3);
        disposer->mpVtable = &sExDisposerVtable;
    }
    sExDisposer = disposer;
    sExInstance = instance ? construct(instance) : NULL;
}
/* 02709004 */
void hdSceneExSave_c::destroyInstance() {
    hdSceneDisposer_c* disposer = sExDisposer;
    if (!disposer) return;
    sExDisposer = NULL;
    gabi::call_ptr<void>(disposer->mpVtable->mDestroy.get(), disposer, 2u);
    hdSceneExSave_c* instance = sExInstance;
    if (instance) gabi::call_ptr<void>(instance->sceneVtable()->mDestroy.get(), instance, 3u);
    sExInstance = NULL;
}
/* 02709084 */
void hdSceneExSave_c::destroy(hdSceneExSave_c* self, u32 flags) {
    if (!self) return;
    saveDataSceneDestroy(self, 0);
    if (flags & 1) hdSceneDelete(self);
}
/* inlined into isBusyEX and isSelectionBusyEX */
static bool hdExStateEquals(hdStateMachine* machine, hdExSceneState_c* target) {
    hdPartsStateId_c* current = hdExGetState(machine);
    u32 currentTarget = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* targetTable = target->mpVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentTarget, current);
    u32 targetKey = gabi::call_ptr<u32>(targetTable->mGetKey.get(), target);
    return currentKey == targetKey;
}
/* 027090D8 */
u8 hdSceneExSave_c::isBusyEX() {
    if (isFileOperation()) return 1;
    if (hdExStateEquals(&mState, &sExExplainState)) return 1;
    if (hdExStateEquals(&mState, &sExSelectState)) return 1;
    if (hdExStateEquals(&mState, &sExEndState)) return 1;
    return 0;
}
/* 02709208 */
u8 hdSceneExSave_c::isSelectionBusyEX() {
    if (isSaveSelection()) return 1;
    if (hdExStateEquals(&mState, &sExExplainState)) return 1;
    if (hdExStateEquals(&mState, &sExSelectState)) return 1;
    return 0;
}
/* 027092F0 */
void hdSceneExSave_c::destroyDisposer(hdSceneDisposer_c* disposer, u32 flags) {
    if (!disposer) return;
    disposer->mpVtable = &sExDisposerVtable;
    if (disposer == sExDisposer) {
        hdSceneExSave_c* instance = sExInstance;
        sExDisposer = NULL;
        gabi::call_ptr<void>(instance->sceneVtable()->mDestroy.get(), instance, 2u);
        sExInstance = NULL;
    }
    hdSceneDisposerDestroy(disposer, 0);
    if (flags & 1) hdSceneDelete(disposer);
}
/* inlined into staticInit */
static void hdExInitializeState(hdExSceneState_c* state, u32 number, const char* name,
                                void* enter, void* execute, void* exit, void* check) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mpVtable = &sExStateVtable;
    state->mEnter.mThisDelta = 0;
    state->mEnter.mVirtualIndex = -1;
    state->mEnter.mFunction = gabi::ea(enter);
    state->mExecute.mThisDelta = 0;
    state->mExecute.mVirtualIndex = -1;
    state->mExecute.mFunction = gabi::ea(execute);
    state->mExit.mThisDelta = 0;
    state->mExit.mVirtualIndex = -1;
    state->mExit.mFunction = gabi::ea(exit);
    state->mReserved = 0;
    state->mReservedIndex = -1;
    state->mpCheck = check;
}
/* 02709398 */
void hdSceneExSave_c::staticInit() {
    sExHeaderWords.mWords[2] = 0;
    sExHeaderWords.mWords[0] = 0;
    sExHeaderWords.mWords[3] = 0;
    sExHeaderWords.mWords[1] = 0;
    hdSceneRegisterGlobal(&sExHeaderRegistration);
    f32 lower = sExLower;
    f32 upper = sExUpper;
    sExHeaderRange.mLower = lower;
    sExHeaderRange.mUpper = upper;
    hdSceneConstructSigned(&sExHeaderRange.mSigned);
    hdSceneRegisterGlobal(&sExSignedRegistration);
    hdSceneConstructUnsigned(&sExHeaderRange.mUnsigned);
    hdSceneRegisterGlobal(&sExUnsignedRegistration);
    u32 number = 0;
    if (sExStateGuard) number = sExStateCounter;
    else sExStateGuard = 1;
    hdExInitializeState(&sExExplainState, number + 1, "::StateID_ExplainEX", &sExExplainEnter, &sExExplainExecute, &sExExplainExit, &sExExplainCheck);
    hdExInitializeState(&sExSelectState, number + 2, "::StateID_SelectSaveDataEX", &sExSelectEnter, &sExSelectExecute, &sExSelectExit, &sExSelectCheck);
    hdExInitializeState(&sExEndState, number + 3, "::StateID_EndEX", &sExEndEnter, &sExEndExecute, &sExEndExit, &sExEndCheck);
    sExStateCounter = number + 3;
}
/* WWHD glue
ext 0270AAEC void hdSaveDataScene_c::resetEventMode()
ext 0270AD7C void hdSaveDataScene_c::updateCancel()
ext 025200D4 hdSaveDataPlay_l* saveDataGetPlay()
ext 026E5544 void hdTitleStartBackground_c::showHeader()
ext 026E5570 void hdTitleStartBackground_c::requestHeaderState()
ext 026E5580 void hdTitleStartBackground_c::hideHeader()
ext 026DBA08 void hdSequenceWindow_c::open(s32,u32)
ext 0270BAF8 u8 hdSaveDataScene_c::isFileOperation()
ext 0270BCF4 u8 hdSaveDataScene_c::isSaveSelection()
data sExInstance 101F82C0
data sExDisposer 101F82C4
data sExInput 101F8378
data sExExplainState 1049E96C
data sExSelectState 1049E998
data sExEndState 1049E9C4
data sExMainEndState 1049DD60
data sExFileSelectStartState 1049A550
data sExStateVtable 1013E960
data sExSceneVtable 1013E9F0
data sExDisposerVtable 1013EA20
data sExQuestComplete 1047B096
data sExSaveMode 1047B07C
data sExHeaderWords 1049E95C
data sExHeaderRange 1049E950
data sExHeaderRegistration 101F829C
data sExSignedRegistration 101F82A8
data sExUnsignedRegistration 101F82B4
data sExLower 1013E9A8
data sExUpper 1013E9AC
data sExStateGuard 101FD8E4
data sExStateCounter 101FDD50
data sExExplainEnter 02708C54
data sExExplainExecute 02708CA0
data sExExplainExit 027095CC
data sExExplainCheck 027095D0
data sExSelectEnter 02708D68
data sExSelectExecute 02708E00
data sExSelectExit 027095D8
data sExSelectCheck 027095DC
data sExEndEnter 027095E4
data sExEndExecute 027095E8
data sExEndExit 027095EC
data sExEndCheck 027095F0
str 1013E9C0 "::StateID_ExplainEX"
str 1013E9D4 "::StateID_SelectSaveDataEX"
str 1013E9B0 "::StateID_EndEX"
ext 025E1E54 u32 hdExCurrentBgm()
ext 02006478 hdPartsStateId_c* hdExGetState(hdStateMachine*)
ext 0270B780 hdSaveDataScene_c* saveDataSceneConstruct(hdSaveDataScene_c*)
ext 0270BA48 void saveDataSceneDestroy(hdSaveDataScene_c*, u32)
*/
