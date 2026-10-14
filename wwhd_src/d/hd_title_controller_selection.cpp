/* HD controller-selection screen parts; private binary-derived source. */
#include "d/hd_title_parts.h"
#include "hd/hd_ui_02001084.h"

static const u32 titleControllerStringDestructor = 0x026D578C;
extern void titleControllerDestroyStringArray(void*, u32, u32, u32, u32, u32);
struct ControllerButtonNamesArray_l { hdUiSafeString mNames[2]; };
static ControllerButtonNamesArray_l sControllerButtonNames;

static hdTitleControllerAvailability sControllerAvailability;
static void* sControllerInput;
static hdTitleControllerUi_l sControllerUi;
static hdButtonInputMode_l* sControllerInputStatus;
static void* sControllerPromptManager;
static hdPartsStateId_c sControllerInState;
static hdPartsStateId_c sControllerConfirmedState;
static void* sControllerDialog;
static hdPartsVtbl_c sControllerPartsVtable;
static u8 sControllerHolderVtable;
static u8 sControllerSecondaryVtable;
struct hdTitleControllerRootGuard_l { be<u32> mInitialized; };
static hdTitleControllerRootGuard_l sControllerRootGuard;
static hdTitleControllerRootGuard_l sControllerButtonNamesGuard;
static u8 sControllerButtonNamesRegistration;
static hdUiSafeString sControllerRootName;
static hdUiStringTable sControllerRootStringVtable;
static u8 sControllerRootRegistration;
extern u32 titleQueryPrimaryController(void*);
extern u32 titleQuerySecondaryController(void*);
extern hdTitleControllerPlay_l* titleControllerPlay();
extern void titleClearControllerPrompt(void*, u32);
extern void titleControllerStartAnimation(hdAnimSet*, u32, u32, f32);
extern u32 titleControllerAnimationEnded(hdAnimSet*, u32);
extern void titleOpenPrimaryConfirmation(void*);
extern void titleOpenSecondaryConfirmation(void*);
extern u32 titleConfirmationFinished(void*);
extern void titleCloseConfirmation(void*);
extern void titleControllerPlaySound(const char*);
extern void* titleControllerAllocate(u32);
extern void titleConstructControllerBase(hdParts_c*);
extern void titleDestroyControllerBase(hdParts_c*, u32);
extern void titleControllerDelete(void*);
extern void titleControllerRegisterRoot(void*);

/* Title reset uses a virtual slot beyond the canonical button vtable prefix. */
struct hdTitleControllerButtonVtbl_l {
    hdButtonPartVtable_l mButton;
    u8 _paddingReset[0x23C - sizeof(hdButtonPartVtable_l)];
    gfn mReset;
};
WWHD_OFFSET(hdTitleControllerButtonVtbl_l, mReset, 0x23C);
static hdTitleControllerButtonVtbl_l* titleControllerButtonVtable(hdButtonPart_c* button) {
    return reinterpret_cast<hdTitleControllerButtonVtbl_l*>(button->mVtable.get());
}

static void resetTitleControllerButton(hdParts_c* parts) {
    auto* button = reinterpret_cast<hdButtonPart_c*>(parts);
    gabi::call_ptr(titleControllerButtonVtable(button)->mReset.get(), button);
}

struct hdTitleControllerButtonIndices_l { be<u32> mValues[2]; };
static hdTitleControllerButtonIndices_l sControllerButtonIndices;
static hdTitleControllerButtonIndices_l sControllerEventKinds;
static hdPartsStateId_c sControllerPrimaryMissingState;
static hdPartsStateId_c sControllerSecondaryMissingState;
extern u32 titleControllerEventPending(hdUiEvent*);
extern void titleControllerCopyEvent(hdUiEvent*, hdUiEvent*);
extern u32 titleControllerIsSelectionEvent(s32);
extern u32 titleControllerPromptMode(void*);
extern void titleControllerConstructEvent(hdUiEvent*, u32, u32, void*);
extern void titleControllerEnqueueEvent(void*, hdUiEvent*);
extern void titleControllerDestroyEvent(hdUiEvent*);
extern u32 titleSelectionPrimaryAvailable(hdParts_c*);
extern u32 titleSelectionSecondaryAvailable(hdParts_c*);

/* 026D440C */
void hdTitleControllerSelection_c::updateSelection() {
    vExecuteChildren();
    vCalcLayout();
    void* managerPointer = sControllerPromptManager;
    auto* manager = static_cast<hdButtonEventManager_l*>(managerPointer);
    if (manager == NULL) return;
    if (titleControllerEventPending(reinterpret_cast<hdUiEvent*>(&manager->mCurrentEvent)) != 0) {
        if (mSelectionPending == 0)
            titleControllerPlaySound("SE_SELECT_CTRL_TO_TITLE");
        mSelectionPending = 1;
        return;
    }
    gabi::Local<hdUiEvent> event;
    managerPointer = sControllerPromptManager;
    manager = hdPartsView<hdButtonEventManager_l>(gabi::ea(managerPointer));
    titleControllerCopyEvent(event.get(), reinterpret_cast<hdUiEvent*>(&manager->mCurrentEvent));
    u32 isSelection = titleControllerIsSelectionEvent(event->mKind);
    auto* children = hdPartsView<gptr<hdParts_c>>(gabi::ea(mChildren.mpData.get()));
    u32 count = mChildren.mNum;
    if (isSelection != 0) {
        for (u32 index = 0; index < 2; ++index) {
            u32 buttonIndex = sControllerButtonIndices.mValues[index];
            hdParts_c* part = children[buttonIndex < count ? buttonIndex : 0];
            if (part == NULL) continue;
            u32 key = reinterpret_cast<hdButtonPart_c*>(part)->getReceiverId();
            bool chosen = u32(event->mId) == key;
            if (!chosen) {
                u32 mode = titleControllerPromptMode(sControllerPromptManager);
                auto* button = reinterpret_cast<hdButtonPart_c*>(part);
                chosen = mode == button->mpReceiver.v;
            }
            if (chosen) {
                key = reinterpret_cast<hdButtonPart_c*>(part)->getReceiverId();
                u32 kind = sControllerEventKinds.mValues[index];
                {
                    gabi::Local<hdUiEvent> outgoing;
                    titleControllerConstructEvent(outgoing.get(), kind, key, NULL);
                    void* currentPointer = sControllerPromptManager;
                    auto* current = hdPartsView<hdButtonEventManager_l>(gabi::ea(currentPointer));
                    titleControllerEnqueueEvent(current->mQueue, outgoing.get());
                    titleControllerDestroyEvent(outgoing.get());
                }
                mSelectedController = index;
                u32 available = index == 0 ? titleSelectionPrimaryAvailable(this)
                                           : titleSelectionSecondaryAvailable(this);
                if (available == 0) {
                    changeState(index == 0 ? &sControllerPrimaryMissingState : &sControllerSecondaryMissingState);
                    return;
                }
                changeState(&sControllerConfirmedState);
                children = hdPartsView<gptr<hdParts_c>>(gabi::ea(mChildren.mpData.get()));
                count = mChildren.mNum;
                break;
            }
            children = hdPartsView<gptr<hdParts_c>>(gabi::ea(mChildren.mpData.get()));
            count = mChildren.mNum;
        }
    }
    for (u32 index = 0; index < 2; ++index) {
        u32 buttonIndex = sControllerButtonIndices.mValues[index];
        hdParts_c* part = children[buttonIndex < count ? buttonIndex : 0];
        if (part != NULL) {
            auto* button = reinterpret_cast<hdButtonPart_c*>(part);
            gabi::call_ptr(hdButtonVtable(button)->mOnFocusChanged.get(), part);
        }
        if (index == 0) {
            children = hdPartsView<gptr<hdParts_c>>(gabi::ea(mChildren.mpData.get()));
            count = mChildren.mNum;
        }
    }
}

/* 026D41F4 */
void hdTitleControllerSelection_c::destroy(hdTitleControllerSelection_c* self, u32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sControllerHolderVtable;
    self->mSecondaryVtable = &sControllerSecondaryVtable;
    titleDestroyControllerBase(self, 0);
    if ((flags & 1) != 0) titleControllerDelete(self);
}

/* 026D4260 */
void hdTitleControllerSelection_c::resetControllerButtons() {
    titleClearControllerPrompt(sControllerPromptManager, 0);
    vCalcLayout();
    resetTitleControllerButton(child(0));
    resetTitleControllerButton(child(1));
}

/* 026D42F4 */
void hdTitleControllerSelection_c::calculateLayout() {
    vCalcLayout();
}

/* 026D4304 */
void hdTitleControllerSelection_c::updateControllerPrompt() {
    resetTitleControllerButton(child(0));
    resetTitleControllerButton(child(1));
    u32 selected = mSelectedController;
    u32 count = mChildren.mNum;
    mSelectionPending = 0;
    u32 index = selected < count ? selected : 0;
    hdParts_c* parts = *mChildren.slot(index);
    auto* button = reinterpret_cast<hdButtonPart_c*>(parts);
    void* manager = sControllerPromptManager;
    titleClearControllerPrompt(manager, button->mpReceiver.v);
}

/* 026D43AC */
static u32 primaryTitleControllerAvailable() {
    if (sControllerAvailability.mOverrideEnabled != 0) {
        return sControllerAvailability.mPrimaryAvailable != 0;
    }
    return titleQueryPrimaryController(sControllerInput);
}

/* 026D43DC */
static u32 secondaryTitleControllerAvailable() {
    if (sControllerAvailability.mOverrideEnabled != 0) {
        return sControllerAvailability.mSecondaryAvailable != 0;
    }
    return titleQuerySecondaryController(sControllerInput);
}

/* 026D46A0 */
void hdTitleControllerSelection_c::beginInitialAnimation() {
    resetTitleControllerButton(child(0));
    resetTitleControllerButton(child(1));
    titleControllerStartAnimation(animSet(), 0, 0, 1.0f);
    vExecuteChildren();
    vCalcLayout();
}

/* 026D4754 */
void hdTitleControllerSelection_c::waitInitialAnimation() {
    if (titleControllerAnimationEnded(animSet(), 0) != 0) {
        changeState(&sControllerInState);
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 026D47CC */
static void clearTitleControllerPrompt() {
    if (sControllerInputStatus->mEnabled != 0) {
        titleClearControllerPrompt(sControllerPromptManager, 0);
    }
}

/* 026D47F0 */
void hdTitleControllerSelection_c::beginInAnimation() {
    clearTitleControllerPrompt();
    titleControllerStartAnimation(animSet(), 1, 0, 1.0f);
}

/* 026D4838 */
void hdTitleControllerSelection_c::waitInAnimation() {
    u32 finished = titleControllerAnimationEnded(animSet(), 1);
    hdPartsVtbl_c* table = mVtable;
    if (finished != 0) mInAnimationFinished = 1;
    gabi::call_ptr(table->mExecuteChildren.get(), this);
    vCalcLayout();
}

/* inlined into the primary and secondary controller setting restore states */
static void restoreTitleControllerSettings() {
    hdTitleControllerPlay_l* play = titleControllerPlay();
    u8 third = play->mSavedSettings[2];
    u8 second = play->mSavedSettings[1];
    play->mCurrentSettings[2] = third;
    u8 first = play->mSavedSettings[0];
    play->mCurrentSettings[1] = second;
    play->mCurrentSettings[0] = first;
    sControllerUi.mSettingsRestored = 1;
    sControllerUi.mMode = 3;
}

static void saveTitleControllerSettings() {
    hdTitleControllerPlay_l* play = titleControllerPlay();
    u8 first = play->mCurrentSettings[0];
    u8 second = play->mCurrentSettings[1];
    play->mSavedSettings[0] = first;
    u8 third = play->mCurrentSettings[2];
    play->mSavedSettings[1] = second;
    play->mSavedSettings[2] = third;
}

/* 026D48A8 */
void hdTitleControllerSelection_c::savePrimarySettings() {
    saveTitleControllerSettings();
    titleOpenPrimaryConfirmation(sControllerDialog);
    titleControllerPlaySound("SE_DATA_ALERT");
}

/* 026D48F8 */
void hdTitleControllerSelection_c::waitPrimaryConfirmation() {
    if (titleConfirmationFinished(sControllerDialog) == 0) return;
    titleCloseConfirmation(sControllerDialog);
    titleClearControllerPrompt(sControllerPromptManager, 0);
    if (primaryTitleControllerAvailable() != 0) {
        changeState(&sControllerConfirmedState);
    } else {
        changeState(&sControllerInState);
    }
}

/* 026D4988 */
void hdTitleControllerSelection_c::restorePrimarySettings() {
    restoreTitleControllerSettings();
}

/* 026D49D8 */
void hdTitleControllerSelection_c::saveSecondarySettings() {
    saveTitleControllerSettings();
    titleOpenSecondaryConfirmation(sControllerDialog);
    titleControllerPlaySound("SE_DATA_ALERT");
}

/* 026D4A28 */
void hdTitleControllerSelection_c::waitSecondaryConfirmation() {
    if (titleConfirmationFinished(sControllerDialog) == 0) return;
    titleCloseConfirmation(sControllerDialog);
    titleClearControllerPrompt(sControllerPromptManager, 0);
    if (secondaryTitleControllerAvailable() != 0) {
        changeState(&sControllerConfirmedState);
    } else {
        changeState(&sControllerInState);
    }
}

/* 026D4AB8 */
void hdTitleControllerSelection_c::restoreSecondarySettings() {
    restoreTitleControllerSettings();
}

/* 026D4B08 */
hdTitleControllerSelection_c* hdTitleControllerSelection_c::construct(hdTitleControllerSelection_c* self) {
    if (self == NULL) {
        self = static_cast<hdTitleControllerSelection_c*>(titleControllerAllocate(sizeof(hdTitleControllerSelection_c)));
        if (self == NULL) return NULL;
    }
    titleConstructControllerBase(self);
    self->mVtable = &sControllerPartsVtable;
    self->mSecondaryVtable = &sControllerSecondaryVtable;
    self->mInAnimationFinished = 0;
    self->mSelectedController = 0;
    self->mStateHolder.mHolderVtable = &sControllerHolderVtable;
    self->mSelectionPending = 0;
    if (gabi::ea(&self->mpControllerRoot) == 0) titleControllerAllocate(sizeof(gptr<void>));
    if (gabi::ea(&self->mControllerPanes) == 0) titleControllerAllocate(sizeof(self->mControllerPanes));
    return self;
}

/* 026D4BA4 */
void hdTitleControllerSelection_c::findControllerRoot() {
    if (sControllerRootGuard.mInitialized == 0) {
        sControllerRootGuard.mInitialized = 1;
        sControllerRootName.mVtable = &sControllerRootStringVtable;
        sControllerRootName.mText.v = gabi::guest_addr("N_All_00");
        titleControllerRegisterRoot(&sControllerRootRegistration);
    }
    auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
    hdScreenResourceTable* table = root->mVtable;
    sControllerRootName.assureTermination();
    mpControllerRoot = gabi::call_ptr<void*>(table->mFindPane.get(), root, sControllerRootName.mText.get(), 1);
}


/* 026D41AC */
static void titleControllerDestroyControllerRootName() {
    titleControllerDestroyStringArray(&sControllerRootName, 1, sizeof(hdUiSafeString), titleControllerStringDestructor, 0, 0);
}

/* 026D41D0 */
static void titleControllerDestroyControllerButtonNames() {
    titleControllerDestroyStringArray(sControllerButtonNames.mNames, 2, sizeof(hdUiSafeString), titleControllerStringDestructor, 0, 0);
}

/* 026D4C68 */
void hdTitleControllerSelection_c::findControllerPane() {
    if (sControllerButtonNamesGuard.mInitialized == 0) {
        sControllerButtonNamesGuard.mInitialized = 1;
        sControllerButtonNames.mNames[0].mVtable = &sControllerRootStringVtable;
        sControllerButtonNames.mNames[0].mText.v = gabi::guest_addr("L_Btn_00");
        sControllerButtonNames.mNames[1].mVtable = &sControllerRootStringVtable;
        sControllerButtonNames.mNames[1].mText.v = gabi::guest_addr("L_Btn_01");
        titleControllerRegisterRoot(&sControllerButtonNamesRegistration);
    }
    auto* root = reinterpret_cast<hdScreenResourceObject*>(mpLayout->lyt()->mpRootPane.get());
    hdScreenResourceTable* table = root->mVtable;
    sControllerButtonNames.mNames[0].assureTermination();
    mControllerPanes[0] = gabi::call_ptr<void*>(table->mFindPane.get(), root,
        sControllerButtonNames.mNames[0].mText.get(), 1);
}

/* WWHD glue (tools/verify/autoglue.py): controller-selection parts
 data sControllerAvailability 101F6A70
 data sControllerInput 101F5088
 data sControllerUi 1047B03C
 data sControllerInputStatus 101F8378
 data sControllerPromptManager 101F4FF0
 data sControllerInState 1049AA44
 data sControllerConfirmedState 1049AAA4
 data sControllerDialog 101F82EC
 data sControllerPartsVtable 10102DE0
 data sControllerHolderVtable 10102ED8
 data sControllerSecondaryVtable 10102EE8
 data sControllerButtonNamesGuard 1049AA40
 data sControllerButtonNamesRegistration 101F6A40
 str 10102CD0
 str 10102CDC
 data sControllerRootGuard 1049AA3C
 data sControllerRootName 1049A9C0
 data sControllerRootStringVtable 10102B94
 data sControllerRootRegistration 101F6A34
 str 10102CC4 in 026D4BA4
 str 10102CA4 in 026D48A8
 str 10102CB4 in 026D49D8
 ext 02617F14 u32 titleQueryPrimaryController(void*)
 ext 02617FCC u32 titleQuerySecondaryController(void*)
 ext 025200D4 hdTitleControllerPlay_l* titleControllerPlay()
 ext 02615154 void titleClearControllerPrompt(void*,u32)
 ext 020053E4 void titleControllerStartAnimation(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 titleControllerAnimationEnded(hdAnimSet*,u32)
 ext 0270B6E4 void titleOpenPrimaryConfirmation(void*)
 ext 0270B714 void titleOpenSecondaryConfirmation(void*)
 ext 02709FC4 u32 titleConfirmationFinished(void*)
 ext 0270A064 void titleCloseConfirmation(void*)
 ext 02030B38 void titleControllerPlaySound(const char*)
 ext 0273AD10 void* titleControllerAllocate(u32)
 ext 026F89E8 void titleConstructControllerBase(hdParts_c*)
 ext 026F88F0 void titleDestroyControllerBase(hdParts_c*,u32)
 ext 0273AF40 void titleControllerDelete(void*)
 ext 028F026C void titleControllerRegisterRoot(void*)
 ext 028F0164 void titleControllerDestroyStringArray(void*,u32,u32,u32,u32,u32)
 data sControllerButtonNames 1049A9DC nocheck
 data sControllerButtonIndices 10102B80
 data sControllerEventKinds 10102B88
 data sControllerPrimaryMissingState 1049AB08
 data sControllerSecondaryMissingState 1049AB34
 str 10102C8C in 026D440C
 ext 026F6340 u32 titleControllerEventPending(hdUiEvent*)
 ext 020013C0 void titleControllerCopyEvent(hdUiEvent*,hdUiEvent*)
 ext 026F620C u32 titleControllerIsSelectionEvent(s32)
 ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
 ext 02615140 u32 titleControllerPromptMode(void*)
 ext 02001338 void titleControllerConstructEvent(hdUiEvent*,u32,u32,void*)
 ext 02001FF4 void titleControllerEnqueueEvent(void*,hdUiEvent*)
 ext 02001490 void titleControllerDestroyEvent(hdUiEvent*)
 ext 026D43AC u32 titleSelectionPrimaryAvailable(hdParts_c*)
 ext 026D43DC u32 titleSelectionSecondaryAvailable(hdParts_c*)
*/
