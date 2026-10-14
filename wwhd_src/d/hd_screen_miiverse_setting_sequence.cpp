/** HD settings sequence helpers. Private binary-derived source. */
#include "SSystem/SComponent/c_xyz.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_screen_menu_miiverse_setting.h"
hdParts_c* miiverseSettingSequenceBaseConstruct(hdParts_c*);
void miiverseSettingSequenceBaseDestroy(hdParts_c*, s32);
/* 026AB394 */
void hdMenuMiiverseSetting_c::clearPressed() {
    mPressed = 0;
}
static f32 sMiiverseSettingZero;
/* 026AB3A0 */
void hdMenuMiiverseSetting_c::setLayoutPositionY(f32 y) {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    const u8 flags = pane->mFlags;
    miiversePaneTranslationY(pane) = y;
    miiversePaneTranslationZ(pane) = sMiiverseSettingZero;
    pane->mFlags = flags | 0x10;
}
/* 026AB47C */
void hdMenuMiiverseSetting_c::restoreLayoutPositionY() {
    setLayoutPositionY(mSavedY);
}
/* 026AB484 */
f32 hdMenuMiiverseSetting_c::layoutPositionY() const {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    return miiversePaneTranslationY(pane);
}
/* 026AAFBC */
static u32 miiverseSettingStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}
namespace miiverseBindings {
static u32 sPartsVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
} // namespace miiverseBindings
void miiverseSettingDelete(void*);
/* 026AB0B4 */
void hdMenuMiiverseSetting_c::destroy(hdMenuMiiverseSetting_c* self, u32 flags) {
    if (!self)
        return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&miiverseBindings::sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&miiverseBindings::sSecondaryVtable);
    miiverseSettingSequenceBaseDestroy(self, 0);
    if (flags & 1)
        miiverseSettingDelete(self);
}
void* miiverseSettingAllocate(u32);
/* 026AB770 */
hdMenuMiiverseSetting_c* hdMenuMiiverseSetting_c::construct(hdMenuMiiverseSetting_c* self) {
    if (!self) {
        self = static_cast<hdMenuMiiverseSetting_c*>(miiverseSettingAllocate(0x68));
        if (!self)
            return nullptr;
    }
    miiverseSettingSequenceBaseConstruct(self);
    self->mPressed = 0;
    self->mPresentationOffset = sMiiverseSettingZero;
    self->mEnabled = 0;
    self->mSecondaryVtable.v = gabi::ea(&miiverseBindings::sSecondaryVtable);
    self->mVtable.v = gabi::ea(&miiverseBindings::sPartsVtable);
    self->mPositionY = sMiiverseSettingZero;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&miiverseBindings::sHolderVtable);
    // The original tests the 32-bit embedded pointer, including address wraparound.
    if (u32(gabi::ea(self) + offsetof(hdMenuMiiverseSetting_c, mpSequencePane)) == 0)
        miiverseSettingAllocate(4);
    return self;
}
/* 026AB430 */
void hdMenuMiiverseSetting_c::executeIn() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026AB55C */
void hdMenuMiiverseSetting_c::executeOut() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
static hdUiSafeString smiiverseSettingSequenceName;
void miiverseSettingDestroySafeString(hdUiSafeString*, u32);
void miiverseSettingDestroyNameArray(void*, u32, u32, u32, u32, u32);
/* 026AB090 */
static void miiverseSettingDestroySequenceNames() {
    miiverseSettingDestroyNameArray(&smiiverseSettingSequenceName, 1, sizeof(hdUiSafeString),
                                    gabi::guest_fn((const void*)miiverseSettingDestroySafeString),
                                    0, 0);
}
static void* sMiiverseSettingService;
static f32 sMiiverseSettingMinimumStep;
f32 miiverseSettingSlideDuration(void*);
/* 026AB638 */
void hdMenuMiiverseSetting_c::enterSlideDown() {
    const f32 duration = miiverseSettingSlideDuration(sMiiverseSettingService);
    const f32 current = layoutPositionY();
    const f32 difference = gabi::fsubs_ppc(current, mPositionY);
    const f32 magnitude = __builtin_fabsf(difference);
    f32 step = f32(f64(magnitude) / f64(duration));
    if (step < sMiiverseSettingMinimumStep)
        step = sMiiverseSettingMinimumStep;
    mSlideStep = step;
}
static hdPartsStateId_c sSettingSequenceMenuWaitState;
u32 miiverseSettingChaseY(be<f32>*, f32, f32);
// Integer copies preserve signalling-NaN bits and use guest-memory accessors.
static void miiverseSettingCopyTranslationWord(be<f32>& destination, const be<f32>& source) {
    *reinterpret_cast<be<u32>*>(&destination) = *reinterpret_cast<const be<u32>*>(&source);
}
/* 026AB6A0 */
void hdMenuMiiverseSetting_c::executeSlideDown() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    cXyz translation;
    // Preserve the original integer copies, including signalling-NaN payloads.
    translation.copy(*reinterpret_cast<const cXyz*>(&miiversePaneTranslationX(pane)));
    const u32 arrived = miiverseSettingChaseY(&translation.y, mPositionY, mSlideStep);
    auto* layout = mpLayout->lyt();
    pane = reinterpret_cast<hdPartsPaneSRT_c*>(layout->mpRootPane.get());
    miiverseSettingCopyTranslationWord(miiversePaneTranslationX(pane), translation.x);
    const u8 flags = pane->mFlags;
    miiverseSettingCopyTranslationWord(miiversePaneTranslationY(pane), translation.y);
    miiverseSettingCopyTranslationWord(miiversePaneTranslationZ(pane), translation.z);
    pane->mFlags = flags | 0x10;
    // These integer operands remain live across the optional state change in GHS.
    gabi::cpu->r[5] = *reinterpret_cast<const be<u32>*>(&translation.z);
    gabi::cpu->r[6] = flags;
    gabi::cpu->r[7] = gabi::ea(layout);
    if (arrived)
        changeState(&sSettingSequenceMenuWaitState);
    auto* table = mVtable.get();
    gabi::cpu->r[8] = gabi::ea(table);
    gabi::call_ptr<void>(table->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
// Distinct retained constant-false bodies; no additional role is inferred from identical code.
/* 026AAF94 */
static u32 miiverseSettingDefaultFalseVariant1() {
    return 0;
}
/* 026AAF9C */
static u32 miiverseSettingDefaultFalseVariant2() {
    return 0;
}
/* 026AAFA8 */
static u32 miiverseSettingDefaultFalseVariant3() {
    return 0;
}
/* 026AAFB4 */
static u32 miiverseSettingDefaultFalseVariant4() {
    return 0;
}
static hdPartsStateId_c sMiiverseSettingSlideDownState;
/* 026AB5A8 */
void hdMenuMiiverseSetting_c::finishOut() {
    auto* current = reinterpret_cast<hdPartsStateId_c*>(stateMachine()->mpLastStateId.get());
    const auto currentGetKey = current->mVtable->mGetKey.get();
    auto* slideDownVtable = sMiiverseSettingSlideDownState.mVtable.get();
    const u32 currentKey = gabi::call_ptr<u32>(currentGetKey, current);
    const u32 slideDownKey =
        gabi::call_ptr<u32>(slideDownVtable->mGetKey.get(), &sMiiverseSettingSlideDownState);
    if (currentKey == slideDownKey)
        setLayoutPositionY(mPositionY);
    else
        setLayoutPositionY(mSavedY);
}
/* 026AB3CC */
void hdMenuMiiverseSetting_c::enterIn() {
    const f32 position = mPositionY;
    mSavedY = position;
    mPresentationOffset = sMiiverseSettingZero;
    setLayoutPositionY(position);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026AB498 */
void hdMenuMiiverseSetting_c::enterOut() {
    const f32 zero = sMiiverseSettingZero;
    auto* current = reinterpret_cast<hdPartsStateId_c*>(stateMachine()->mpLastStateId.get());
    mPresentationOffset = zero;
    const auto currentGetKey = current->mVtable->mGetKey.get();
    auto* slideDownVtable = sMiiverseSettingSlideDownState.mVtable.get();
    const u32 currentKey = gabi::call_ptr<u32>(currentGetKey, current);
    const u32 slideDownKey =
        gabi::call_ptr<u32>(slideDownVtable->mGetKey.get(), &sMiiverseSettingSlideDownState);
    if (currentKey != slideDownKey)
        mPositionY = layoutPositionY();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
namespace miiverseBindings {
static u32 sSettingSequenceStringVtable;
}
static hdUiSafeString sSettingSequencePaneName;
static u32 sSettingSequencePaneNameReady;
static u32 sSettingSequencePaneNameRegistration[3] = {0, 0, 0};
void miiverseSettingSequenceRegisterPaneName(void*);
/* 026AB808 */
void hdMenuMiiverseSetting_c::findSequencePane() {
    if (!sSettingSequencePaneNameReady) {
        sSettingSequencePaneNameReady = 1;
        sSettingSequencePaneName.mVtable.v = gabi::ea(&miiverseBindings::sSettingSequenceStringVtable);
        sSettingSequencePaneName.mText.v = gabi::guest_addr("L_BtnSequence_00");
        miiverseSettingSequenceRegisterPaneName(sSettingSequencePaneNameRegistration);
    }
    auto* root = mpLayout->lyt()->mpRootPane.get();
    mpSequencePane = reinterpret_cast<hdPartsPaneSRT_c*>(
        miiverseFindNamedPane(root, &sSettingSequencePaneName));
}
static hdButtonEventManager_l* sSettingInputManager;
static cking::ui::input::Options* sSettingInputOptions;
static void* sSettingInputArchive;
static u32 sSettingInputChildIndex;
static u32 sSettingInputPostedEventType;
void miiverseSettingInputCopyEvent(hdButtonEvent_l*, const hdButtonEvent_l*);
u32 miiverseSettingInputIsSpecial(u32);
u32 miiverseSettingInputIsCancel(u32);
u32 miiverseSettingInputIsTouchBegin(hdButtonEvent_l*);
u32 miiverseSettingInputReceiverId(hdButtonPart_c*);
hdButtonReceiver_l* miiverseSettingInputFocusedReceiver(hdButtonEventManager_l*);
void miiverseSettingInputFocus(hdButtonEventManager_l*, hdButtonReceiver_l*);
u32 miiverseSettingInputArchiveBusy(void*);
void miiverseSettingInputConstructEvent(hdButtonEvent_l*, u32, u32, u32);
void miiverseSettingInputQueueEvent(void*, hdButtonEvent_l*);
void miiverseSettingInputDestroyEvent(hdButtonEvent_l*);
/* 026AB120 */
u32 hdMenuMiiverseSetting_c::processInput() {
    if (!mEnabled)
        return 0;
    hdMiiverseSequenceInputEvents_l events;
    miiverseSettingInputCopyEvent(&events.mCurrent, &sSettingInputManager->mCurrentEvent);
    const u32 special = miiverseSettingInputIsSpecial(events.mCurrent.mType);
    const u32 cancel = miiverseSettingInputIsCancel(events.mCurrent.mType);
    u32 touch = 0;
    if (sSettingInputOptions->mDirectionalInputEnabled)
        touch = miiverseSettingInputIsTouchBegin(&events.mCurrent);
    auto* children = mChildren.mpData.get();
    const u32 index = sSettingInputChildIndex;
    if (special | cancel | touch) {
        auto* child = reinterpret_cast<hdButtonPart_c*>(
            children[index < u32(mChildren.mNum) ? index : 0].get());
        if (child) {
            const u32 receiverId = miiverseSettingInputReceiverId(child);
            bool target = u32(events.mCurrent.mReceiverId) == receiverId;
            if (!target) {
                auto* focused = miiverseSettingInputFocusedReceiver(sSettingInputManager);
                target = focused == child->mpReceiver.get();
            }
            if (target && !special) {
                miiverseSettingInputFocus(sSettingInputManager, child->mpReceiver);
                mPressed = 1;
                return 0;
            }
            if (mPressed) {
                const u32 pressedReceiver = miiverseSettingInputReceiverId(child);
                if (u32(events.mCurrent.mReceiverId) == pressedReceiver && special) {
                    mPressed = 0;
                    if (miiverseSettingInputArchiveBusy(sSettingInputArchive))
                        return 0;
                    const u32 postedType = sSettingInputPostedEventType;
                    const u32 postedReceiver = miiverseSettingInputReceiverId(child);
                    miiverseSettingInputConstructEvent(&events.mPosted, postedType, postedReceiver,
                                                       0);
                    miiverseSettingInputQueueEvent(sSettingInputManager->mQueue, &events.mPosted);
                    miiverseSettingInputDestroyEvent(&events.mPosted);
                    auto* first = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
                    mPressed = 0;
                    miiverseSettingInputFocus(sSettingInputManager, first->mpReceiver);
                    return postedType;
                }
            }
            children = mChildren.mpData.get();
        }
    }
    auto* child =
        reinterpret_cast<hdButtonPart_c*>(children[index < u32(mChildren.mNum) ? index : 0].get());
    if (child) {
        auto* table = reinterpret_cast<hdButtonPartVtable_l*>(child->mVtable.get());
        gabi::call_ptr<void>(table->mOnFocusChanged.get(), child);
    }
    return 0;
}
/* 026AB340 */
void hdMenuMiiverseSetting_c::executeWait() {
    processInput();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* WWHD glue (tools/verify/autoglue.py)
 data sPartsVtable 100FC790 nocheck
 data sHolderVtable 100FC858 nocheck
 data sSecondaryVtable 100FC868 nocheck
 data sSettingSequenceStringVtable 100FC614 nocheck
data sSettingInputManager 101F4FF0
data sSettingInputOptions 101F8378
data sSettingInputArchive 1018F4AC
data sSettingInputChildIndex 100FC60C nocheck
data sSettingInputPostedEventType 100FC610 nocheck
ext 020013C0 void miiverseSettingInputCopyEvent(hdButtonEvent_l*, const hdButtonEvent_l*)
ext 026F620C u32 miiverseSettingInputIsSpecial(u32)
ext 026F61DC u32 miiverseSettingInputIsCancel(u32)
ext 026F6288 u32 miiverseSettingInputIsTouchBegin(hdButtonEvent_l*)
ext 026F5B74 u32 miiverseSettingInputReceiverId(hdButtonPart_c*)
ext 02615140 hdButtonReceiver_l* miiverseSettingInputFocusedReceiver(hdButtonEventManager_l*)
ext 02615154 void miiverseSettingInputFocus(hdButtonEventManager_l*, hdButtonReceiver_l*)
ext 020399DC u32 miiverseSettingInputArchiveBusy(void*)
ext 02001338 void miiverseSettingInputConstructEvent(hdButtonEvent_l*, u32, u32, u32)
ext 02001FF4 void miiverseSettingInputQueueEvent(void*, hdButtonEvent_l*)
ext 02001490 void miiverseSettingInputDestroyEvent(hdButtonEvent_l*)
data sSettingSequencePaneName 10497AAC
data sSettingSequencePaneNameReady 10497AF0
data sSettingSequencePaneNameRegistration 101F6498 nocheck
str 100FC6CC "L_BtnSequence_00" in 026AB808
ext 028F026C void miiverseSettingSequenceRegisterPaneName(void*)
data sMiiverseSettingSlideDownState 10497BAC
ext 026F89E8 hdParts_c* miiverseSettingSequenceBaseConstruct(hdParts_c*)
ext 026F88F0 void miiverseSettingSequenceBaseDestroy(hdParts_c*, s32)
data sSettingSequenceMenuWaitState 10497AFC
ext 0200F5C8 u32 miiverseSettingChaseY(be<f32>*, f32, f32)
data sMiiverseSettingService 1018F4AC
data sMiiverseSettingMinimumStep 100FC6C8 nocheck
ext 0203A910 f32 miiverseSettingSlideDuration(void*)
data smiiverseSettingSequenceName 10497AAC
ext 026AC09C void miiverseSettingDestroySafeString(hdUiSafeString*, u32) callback
ext 028F0164 void miiverseSettingDestroyNameArray(void*, u32, u32, u32, u32, u32)
ext 0273AD10 void* miiverseSettingAllocate(u32)
ext 0273AF40 void miiverseSettingDelete(void*)
data sMiiverseSettingZero 100FC6C4 nocheck
*/
