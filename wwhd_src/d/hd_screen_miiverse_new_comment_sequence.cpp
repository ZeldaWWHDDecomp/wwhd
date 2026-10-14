/** HD new-comment sequence helpers. Private binary-derived source. */
#include "d/hd_screen_btn_miiverse_new_comment.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_screen_menu_miiverse_new_comment.h"
hdParts_c* miiverseNewCommentSequenceBaseConstruct(hdParts_c*);
void miiverseNewCommentSequenceBaseDestroy(hdParts_c*, s32);
/* 026AA104 */
void hdMenuMiiverseNewComment_c::clearPressed() {
    mPressed = 0;
}
/* 026A9CC4 */
static u32 miiverseNewCommentStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}
namespace miiverseBindings {
static u32 sPartsVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
} // namespace miiverseBindings
void miiverseNewCommentDelete(void*);
/* 026A9DBC */
void hdMenuMiiverseNewComment_c::destroy(hdMenuMiiverseNewComment_c* self, u32 flags) {
    if (!self)
        return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&miiverseBindings::sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&miiverseBindings::sSecondaryVtable);
    miiverseNewCommentSequenceBaseDestroy(self, 0);
    if (flags & 1)
        miiverseNewCommentDelete(self);
}
void* miiverseNewCommentAllocate(u32);
static f32 sNewCommentZero;
/* 026AA2B4 */
hdMenuMiiverseNewComment_c*
hdMenuMiiverseNewComment_c::construct(hdMenuMiiverseNewComment_c* self) {
    if (!self) {
        self = static_cast<hdMenuMiiverseNewComment_c*>(miiverseNewCommentAllocate(0x5C));
        if (!self)
            return nullptr;
    }
    miiverseNewCommentSequenceBaseConstruct(self);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&miiverseBindings::sHolderVtable);
    self->mEnabled = 0;
    self->mSecondaryVtable.v = gabi::ea(&miiverseBindings::sSecondaryVtable);
    self->mVisible = 0;
    self->mPressed = 0;
    self->mPositionY = sNewCommentZero;
    self->mVtable.v = gabi::ea(&miiverseBindings::sPartsVtable);
    // The original tests the 32-bit embedded pointer, including address wraparound.
    if (u32(gabi::ea(self) + offsetof(hdMenuMiiverseNewComment_c, mpSequencePane)) == 0)
        miiverseNewCommentAllocate(4);
    return self;
}
/* 026AA110 */
void hdMenuMiiverseNewComment_c::enterInOut() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026AA15C */
void hdMenuMiiverseNewComment_c::executeInOut() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
static hdUiSafeString smiiverseNewCommentSequenceName;
void miiverseNewCommentDestroySafeString(hdUiSafeString*, u32);
void miiverseNewCommentDestroyNameArray(void*, u32, u32, u32, u32, u32);
/* 026A9D98 */
static void miiverseNewCommentDestroySequenceNames() {
    miiverseNewCommentDestroyNameArray(
        &smiiverseNewCommentSequenceName, 1, sizeof(hdUiSafeString),
        gabi::guest_fn((const void*)miiverseNewCommentDestroySafeString), 0, 0);
}
// Distinct retained constant-false bodies; no additional role is inferred from identical code.
/* 026A9C7C */
static u32 miiverseNewCommentDefaultFalseVariant1() {
    return 0;
}
/* 026A9C88 */
static u32 miiverseNewCommentDefaultFalseVariant2() {
    return 0;
}
/* 026A9C98 */
static u32 miiverseNewCommentDefaultFalseVariant3() {
    return 0;
}
/* 026A9CA0 */
static u32 miiverseNewCommentDefaultFalseVariant4() {
    return 0;
}
/* 026A9CA8 */
static u32 miiverseNewCommentDefaultFalseVariant5() {
    return 0;
}
/* 026A9CB0 */
static u32 miiverseNewCommentDefaultFalseVariant6() {
    return 0;
}
/* 026A9CBC */
static u32 miiverseNewCommentDefaultFalseVariant7() {
    return 0;
}
static f32 sNewCommentAnimationSpeed;
static hdPartsStateId_c sNewCommentSequenceMenuCheckInputState;
void miiverseNewCommentPlayAnimation(hdAnimSet*, u32, u32, f32);
u32 miiverseNewCommentAnimationFinished(hdAnimSet*, u32);
/* 026AA1A8 */
void hdMenuMiiverseNewComment_c::enterAppear() {
    mpSequencePane->mAlpha = 0xFF;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    miiverseNewCommentPlayAnimation(animSet(), 0, 0, sNewCommentAnimationSpeed);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026AA23C */
void hdMenuMiiverseNewComment_c::executeAppear() {
    if (miiverseNewCommentAnimationFinished(animSet(), 0))
        changeState(&sNewCommentSequenceMenuCheckInputState);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A9E28 */
void hdMenuMiiverseNewComment_c::enterWait() {
    mpSequencePane->mAlpha = 0xFF;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    hdParts_c* content = *mChildren.mpData;
    mVisible = 1;
    reinterpret_cast<hdBtnMiiverseNewComment_c*>(content)->stopLoop();
}
namespace miiverseBindings {
static u32 sNewCommentSequenceStringVtable;
}
static hdUiSafeString sNewCommentSequencePaneName;
static u32 sNewCommentSequencePaneNameReady;
static u32 sNewCommentSequencePaneNameRegistration[3] = {0, 0, 0};
void miiverseNewCommentSequenceRegisterPaneName(void*);
/* 026AA34C */
void hdMenuMiiverseNewComment_c::findSequencePane() {
    if (!sNewCommentSequencePaneNameReady) {
        sNewCommentSequencePaneNameReady = 1;
        sNewCommentSequencePaneName.mVtable.v = gabi::ea(&miiverseBindings::sNewCommentSequenceStringVtable);
        sNewCommentSequencePaneName.mText.v = gabi::guest_addr("L_BtnSequence_00");
        miiverseNewCommentSequenceRegisterPaneName(sNewCommentSequencePaneNameRegistration);
    }
    auto* root = mpLayout->lyt()->mpRootPane.get();
    mpSequencePane = reinterpret_cast<hdPartsPaneSRT_c*>(
        miiverseFindNamedPane(root, &sNewCommentSequencePaneName));
}
static hdButtonEventManager_l* sNewCommentInputManager;
static cking::ui::input::Options* sNewCommentInputOptions;
static void* sNewCommentInputArchive;
static u32 sNewCommentInputChildIndex;
static u32 sNewCommentInputPostedEventType;
void miiverseNewCommentInputCopyEvent(hdButtonEvent_l*, const hdButtonEvent_l*);
u32 miiverseNewCommentInputIsSpecial(u32);
u32 miiverseNewCommentInputIsCancel(u32);
u32 miiverseNewCommentInputIsTouchBegin(hdButtonEvent_l*);
u32 miiverseNewCommentInputReceiverId(hdButtonPart_c*);
hdButtonReceiver_l* miiverseNewCommentInputFocusedReceiver(hdButtonEventManager_l*);
void miiverseNewCommentInputFocus(hdButtonEventManager_l*, hdButtonReceiver_l*);
u32 miiverseNewCommentInputArchiveBusy(void*);
void miiverseNewCommentInputConstructEvent(hdButtonEvent_l*, u32, u32, u32);
void miiverseNewCommentInputQueueEvent(void*, hdButtonEvent_l*);
void miiverseNewCommentInputDestroyEvent(hdButtonEvent_l*);
/* 026A9E6C */
u32 hdMenuMiiverseNewComment_c::processInput() {
    if (!mEnabled)
        return 0;
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    if (!(u8(root->mFlags) & 1))
        return 0;
    hdMiiverseSequenceInputEvents_l events;
    miiverseNewCommentInputCopyEvent(&events.mCurrent, &sNewCommentInputManager->mCurrentEvent);
    const u32 special = miiverseNewCommentInputIsSpecial(events.mCurrent.mType);
    const u32 cancel = miiverseNewCommentInputIsCancel(events.mCurrent.mType);
    u32 touch = 0;
    if (sNewCommentInputOptions->mDirectionalInputEnabled)
        touch = miiverseNewCommentInputIsTouchBegin(&events.mCurrent);
    auto* children = mChildren.mpData.get();
    const u32 index = sNewCommentInputChildIndex;
    if (special | cancel | touch) {
        auto* child = reinterpret_cast<hdButtonPart_c*>(
            children[index < u32(mChildren.mNum) ? index : 0].get());
        if (child) {
            const u32 receiverId = miiverseNewCommentInputReceiverId(child);
            bool target = u32(events.mCurrent.mReceiverId) == receiverId;
            if (!target) {
                auto* focused = miiverseNewCommentInputFocusedReceiver(sNewCommentInputManager);
                target = focused == child->mpReceiver.get();
            }
            if (target && !special) {
                miiverseNewCommentInputFocus(sNewCommentInputManager, child->mpReceiver);
                mPressed = 1;
                return 0;
            }
            if (mPressed) {
                const u32 pressedReceiver = miiverseNewCommentInputReceiverId(child);
                if (u32(events.mCurrent.mReceiverId) == pressedReceiver && special) {
                    mPressed = 0;
                    if (miiverseNewCommentInputArchiveBusy(sNewCommentInputArchive))
                        return 0;
                    const u32 postedType = sNewCommentInputPostedEventType;
                    const u32 postedReceiver = miiverseNewCommentInputReceiverId(child);
                    miiverseNewCommentInputConstructEvent(&events.mPosted, postedType,
                                                          postedReceiver, 0);
                    miiverseNewCommentInputQueueEvent(sNewCommentInputManager->mQueue,
                                                      &events.mPosted);
                    miiverseNewCommentInputDestroyEvent(&events.mPosted);
                    mPressed = 0;
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
/* 026AA088 */
void hdMenuMiiverseNewComment_c::executeWait() {
    processInput();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    reinterpret_cast<hdBtnMiiverseNewComment_c*>((*mChildren.mpData).get())->startLoop();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    reinterpret_cast<hdBtnMiiverseNewComment_c*>((*mChildren.mpData).get())->pauseLoop();
}
/* WWHD glue (tools/verify/autoglue.py)
 data sPartsVtable 100FC520 nocheck
 data sHolderVtable 100FC5E8 nocheck
 data sSecondaryVtable 100FC5F8 nocheck
 data sNewCommentSequenceStringVtable 100FC3A8 nocheck
data sNewCommentInputManager 101F4FF0
data sNewCommentInputOptions 101F8378
data sNewCommentInputArchive 1018F4AC
data sNewCommentInputChildIndex 100FC3A0 nocheck
data sNewCommentInputPostedEventType 100FC3A4 nocheck
ext 020013C0 void miiverseNewCommentInputCopyEvent(hdButtonEvent_l*, const hdButtonEvent_l*)
ext 026F620C u32 miiverseNewCommentInputIsSpecial(u32)
ext 026F61DC u32 miiverseNewCommentInputIsCancel(u32)
ext 026F6288 u32 miiverseNewCommentInputIsTouchBegin(hdButtonEvent_l*)
ext 026F5B74 u32 miiverseNewCommentInputReceiverId(hdButtonPart_c*)
ext 02615140 hdButtonReceiver_l* miiverseNewCommentInputFocusedReceiver(hdButtonEventManager_l*)
ext 02615154 void miiverseNewCommentInputFocus(hdButtonEventManager_l*, hdButtonReceiver_l*)
ext 020399DC u32 miiverseNewCommentInputArchiveBusy(void*)
ext 02001338 void miiverseNewCommentInputConstructEvent(hdButtonEvent_l*, u32, u32, u32)
ext 02001FF4 void miiverseNewCommentInputQueueEvent(void*, hdButtonEvent_l*)
ext 02001490 void miiverseNewCommentInputDestroyEvent(hdButtonEvent_l*)
data sNewCommentSequencePaneName 1049799C
data sNewCommentSequencePaneNameReady 104979F0
data sNewCommentSequencePaneNameRegistration 101F6468 nocheck
str 100FC458 "L_BtnSequence_00" in 026AA34C
ext 028F026C void miiverseNewCommentSequenceRegisterPaneName(void*)
data sNewCommentAnimationSpeed 100FC450 nocheck
data sNewCommentSequenceMenuCheckInputState 10497A28
ext 020053E4 void miiverseNewCommentPlayAnimation(hdAnimSet*, u32, u32, f32)
ext 02005840 u32 miiverseNewCommentAnimationFinished(hdAnimSet*, u32)
ext 026F89E8 hdParts_c* miiverseNewCommentSequenceBaseConstruct(hdParts_c*)
ext 026F88F0 void miiverseNewCommentSequenceBaseDestroy(hdParts_c*, s32)
data smiiverseNewCommentSequenceName 1049799C
ext 026AAF70 void miiverseNewCommentDestroySafeString(hdUiSafeString*, u32) callback
ext 028F0164 void miiverseNewCommentDestroyNameArray(void*, u32, u32, u32, u32, u32)
ext 0273AD10 void* miiverseNewCommentAllocate(u32)
data sNewCommentZero 100FC454 nocheck
ext 0273AF40 void miiverseNewCommentDelete(void*)
ext 02641288 void hdBtnMiiverseNewComment_c::startLoop()
ext 026412A4 void hdBtnMiiverseNewComment_c::pauseLoop()
ext 02641274 void hdBtnMiiverseNewComment_c::stopLoop()
*/
