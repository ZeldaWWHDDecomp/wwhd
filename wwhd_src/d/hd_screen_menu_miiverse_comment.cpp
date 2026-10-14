/** HD Miiverse comment button: selection flags and navigation. Private binary-derived source. */
#include "d/hd_screen_btn_miiverse_comment.h"
#include "d/hd_screen_menu_miiverse_comment.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_parts_basic_button.h"
#include "d/hd_save_info_holder.h"
#include "d/hd_screen_menu_miiverse.h"
namespace miiverseBindings {
static u32 sCommentSoundStringVtable;
static u32 sCommentPartsVtable;
static u32 sCommentHolderVtable;
static u32 sCommentSecondaryVtable;
} // namespace miiverseBindings
static f32 sMiiverseCommentZero;
static hdPartsStateId_c sMiiverseCommentOutState;
static hdButtonEventManager_l* sMiiverseCommentEventManager;
void miiverseCommentDelete(void*);
void miiverseCommentFocus(void*, hdButtonReceiver_l*);
void miiverseCommentConnectReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*);

hdParts_c* miiverseCommentBaseConstruct(hdParts_c*);
void miiverseCommentBaseDestroy(hdParts_c*, s32);
void miiverseCommentBaseDetachArchives(hdParts_c*);
/* 026A6BD8 */
void hdMenuMiiverseComment_c::clearSelection() {
    mSelectionPending = 0;
}
/* 026A73B8 */
void hdMenuMiiverseComment_c::clearCommentSelection() {
    mSelectionPending = 0;
}
/* 026A76A8 */
void hdMenuMiiverseComment_c::clearPictureSelection() {
    mSelectionPending = 0;
}
/* 026A9140 */
u32 hdMenuMiiverseComment_c::commentRequested() const {
    return mCommentRequested;
}
/* 026A9148 */
u32 hdMenuMiiverseComment_c::pictureRequested() const {
    return mPictureRequested;
}
/* 026A8D2C */
hdParts_c* hdMenuMiiverseComment_c::content() const {
    return *mChildren.mpData;
}
/* 026A879C */
void hdMenuMiiverseComment_c::connectUpperControl(hdButtonPart_c* control) {
    if (control) {
        auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseCommentConnectReceiver(control, 3, content->mpReceiver);
    }
}
/* 026A87C4 */
void hdMenuMiiverseComment_c::connectLowerControl(hdButtonPart_c* control) {
    if (control) {
        auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseCommentConnectReceiver(control, 3, content->mpReceiver);
    }
}
/* 026A87EC */
void hdMenuMiiverseComment_c::connectPictureControl(hdButtonPart_c* control) {
    if (control) {
        auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseCommentConnectReceiver(control, 3, content->mpReceiver);
    }
}
/* 026A6918 */
void hdMenuMiiverseComment_c::destroy(hdMenuMiiverseComment_c* self, u32 flags) {
    if (!self)
        return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&miiverseBindings::sCommentHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&miiverseBindings::sCommentSecondaryVtable);
    miiverseCommentBaseDestroy(self, 0);
    if (flags & 1)
        miiverseCommentDelete(self);
}
/* 026A7C84 */
void hdMenuMiiverseComment_c::finalizeParts() {
    gabi::call_ptr<void>(mVtable->mDeleteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    miiverseCommentBaseDetachArchives(this);
}
/* 026A6A98 */
f32 hdMenuMiiverseComment_c::layoutPositionY() const {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    return miiversePaneTranslationY(pane);
}
/* 026A73C4 */
void hdMenuMiiverseComment_c::setLayoutPositionY(f32 y) {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    const u8 flags = pane->mFlags;
    miiversePaneTranslationY(pane) = y;
    miiversePaneTranslationZ(pane) = sMiiverseCommentZero;
    pane->mFlags = flags | 0x10;
}
/* 026A74EC */
void hdMenuMiiverseComment_c::restoreLayoutPositionY() {
    setLayoutPositionY(mSavedY);
}
/* 026A8CCC */
void hdMenuMiiverseComment_c::focusContent() {
    auto* button = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    hdButtonReceiver_l* receiver = button->mpReceiver;
    miiverseCommentFocus(sMiiverseCommentEventManager, receiver);
}
/* 026A8D18 */
void hdMenuMiiverseComment_c::openContent() {
    reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())->getPictureHeight();
}
/* 026A8E1C */
f32 hdMenuMiiverseComment_c::pictureWidth() const {
    u32 index = u32(mChildren.mNum) > 2 ? 2 : 0;
    return reinterpret_cast<hdBtnMiiversePicture_c*>(mChildren.mpData[index].get())->getWidth();
}
/* 026A8E38 */
f32 hdMenuMiiverseComment_c::pictureHeight() const {
    u32 index = u32(mChildren.mNum) > 2 ? 2 : 0;
    return reinterpret_cast<hdBtnMiiversePicture_c*>(mChildren.mpData[index].get())->getHeight();
}
/* 026A6818 */
static u32 miiverseCommentStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}
/* 026A74A0 */
void hdMenuMiiverseComment_c::executeIn() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A75CC */
void hdMenuMiiverseComment_c::executeOut() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A76B4 */
void hdMenuMiiverseComment_c::executePhoto() {
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A8F20 */
void hdMenuMiiverseComment_c::beginOut() {
    if (mFilled)
        changeState(&sMiiverseCommentOutState);
}
void* miiverseCommentAllocate(u32);
void miiverseCommentInitializeTimer(void*);
void* miiverseCommentClearBuffer(void*, u32, u32);
/* 026A7700 */
hdMenuMiiverseComment_c* hdMenuMiiverseComment_c::construct(hdMenuMiiverseComment_c* self) {
    if (!self) {
        self = static_cast<hdMenuMiiverseComment_c*>(miiverseCommentAllocate(sizeof(*self)));
        if (!self)
            return nullptr;
    }
    miiverseCommentBaseConstruct(self);
    self->mCommentRequested = 0;
    self->mTargetY = sMiiverseCommentZero;
    self->mArchiveIndex = 0xFFFFFFFF;
    self->mPictureRequested = 0;
    self->mPicturePositionY = sMiiverseCommentZero;
    self->mScrollTarget = sMiiverseCommentZero;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&miiverseBindings::sCommentHolderVtable);
    self->mVtable.v = gabi::ea(&miiverseBindings::sCommentPartsVtable);
    self->mContentBaseY = sMiiverseCommentZero;
    self->mInteractionFlags[0] = 0;
    self->mSecondaryVtable.v = gabi::ea(&miiverseBindings::sCommentSecondaryVtable);
    self->mContentBaseZ = sMiiverseCommentZero;
    self->mPictureBaseZ = sMiiverseCommentZero;
    self->mPositionY = sMiiverseCommentZero;
    self->mPictureSlideStep = sMiiverseCommentZero;
    self->mPictureBaseX = sMiiverseCommentZero;
    self->mOpenPending = 0;
    self->mEnabled = 0;
    self->mPictureBaseY = sMiiverseCommentZero;
    self->mContentBaseX = sMiiverseCommentZero;
    self->mFilled = 0;
    self->mSelectionPending = 0;
    miiverseCommentInitializeTimer(&self->mTimerHigh);
    self->mPendingFlags[0] = 0;
    self->mPendingState = 0;
    if (u32(gabi::ea(self) + offsetof(hdMenuMiiverseComment_c, mPicturePanes)) == 0)
        miiverseCommentAllocate(sizeof(self->mPicturePanes));
    miiverseCommentClearBuffer(self->mContentStorage, 0, sizeof(self->mContentStorage));
    return self;
}
/* 026A8F3C */
void hdMenuMiiverseComment_c::translateLayoutY(f32 delta) {
    auto* layout = mpLayout.get();
    auto* source = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const f32 x = miiversePaneTranslationX(source);
    const f32 z = miiversePaneTranslationZ(source);
    const f32 y = gabi::fadds_ppc(miiversePaneTranslationY(source), delta);
    auto* destination = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    miiversePaneTranslationY(destination) = y;
    const u8 flags = destination->mFlags;
    miiversePaneTranslationX(destination) = x;
    destination->mFlags = flags | 0x10;
    miiversePaneTranslationZ(destination) = z;
}
/* 026A8FA0 */
void hdMenuMiiverseComment_c::setPicturePresentation(u32 alpha, f32 position) {
    if (!mFilled)
        return;
    for (u32 index = 0; index < 4; ++index)
        mPicturePanes[index]->mAlpha = u8(alpha);
    auto* layout = mpLayout.get();
    auto* source = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const f32 x = miiversePaneTranslationX(source);
    const f32 previousPosition = mScrollTarget;
    const f32 z = miiversePaneTranslationZ(source);
    const f32 previousY = miiversePaneTranslationY(source);
    const f32 delta = gabi::fsubs_ppc(position, previousPosition);
    auto* destination = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const f32 y = gabi::fadds_ppc(previousY, delta);
    const u8 flags = destination->mFlags;
    miiversePaneTranslationX(destination) = x;
    miiversePaneTranslationY(destination) = y;
    destination->mFlags = flags | 0x10;
    miiversePaneTranslationZ(destination) = z;
    mScrollTarget = position;
}
void miiverseCommentGlobalTranslation(cXyz*, hdPartsPaneSRT_c*);
/* 026A8DD0 */
void hdMenuMiiverseComment_c::pictureGlobalTranslation(cXyz* output) const {
    cXyz translation;
    miiverseCommentGlobalTranslation(&translation, mPicturePanes[2]);
    output->copy(translation);
}
// Distinct retained constant-false bodies; no additional role is inferred from identical code.
/* 026A6800 */
static u32 miiverseCommentDefaultFalseVariant1() {
    return 0;
}
/* 026A6808 */
static u32 miiverseCommentDefaultFalseVariant2() {
    return 0;
}
/* 026A6810 */
static u32 miiverseCommentDefaultFalseVariant3() {
    return 0;
}
/* 026A6BE4 */
void hdMenuMiiverseComment_c::showPicturePanes() {
    auto* layout = mpLayout.get();
    mSelectionPending = 0;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    for (u32 index = 0; index < 4; ++index)
        mPicturePanes[index]->mAlpha = 0xFF;
}
/* 026A8CEC */
f32 hdMenuMiiverseComment_c::contentGlobalY() const {
    cXyz translation;
    miiverseCommentGlobalTranslation(&translation, mPicturePanes[0]);
    return translation.y;
}
struct hdMiiverseCommentArchiveEntry_l {
    u8 mMetadata[0x24C];
    be<u8> mPictureRequested;
    be<u8> mContentDisabled;
    u8 mCountPadding[2];
    be<u32> mInteractionCount;
    u8 mCommentMetadata[0x259 - 0x254];
    be<u8> mCommentReady;
    u8 mPictureMetadata[0x274 - 0x25A];
    u8 mPhotoContent[0x3C4 - 0x274];
    u8 mRequestContent[0x3E5 - 0x3C4];
    be<u8> mPhotoReady;
    u8 mTail[6];
};
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mRequestContent, 0x3C4);
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mInteractionCount, 0x250);
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mPictureRequested, 0x24C);
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mPhotoContent, 0x274);
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mContentDisabled, 0x24D);
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mCommentReady, 0x259);
WWHD_OFFSET(hdMiiverseCommentArchiveEntry_l, mPhotoReady, 0x3E5);
WWHD_SIZE(hdMiiverseCommentArchiveEntry_l, 0x3EC);
struct hdMiiverseCommentArchive_l {
    u8 mHeader[0x14];
    hdMiiverseCommentArchiveEntry_l mEntries[10];
};
WWHD_OFFSET(hdMiiverseCommentArchive_l, mEntries, 0x14);
static hdMiiverseCommentArchive_l* sMiiverseCommentArchive;
static hdSaveInfoHolder_c* sMiiverseCommentSaveInfo;
hdSavePair_c* miiverseCommentScratchPair(hdSaveHolders_c*);
/* 026A6C50 */
u32 hdMenuMiiverseComment_c::canShowPhoto() const {
    if (!mFilled)
        return 0;
    const u32 index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
    if (!sMiiverseCommentArchive->mEntries[index].mPhotoReady)
        return 0;
    hdSavePair_c* pair = miiverseCommentScratchPair(&sMiiverseCommentSaveInfo->mHolders);
    return u8(pair->mSecondary) == 1;
}
static hdPartsStateId_c sMiiverseCommentWaitState;
u32 miiverseCommentAnimationFinished(hdAnimSet*, u32);
/* 026A6A20 */
void hdMenuMiiverseComment_c::executeInNewPost() {
    if (miiverseCommentAnimationFinished(animSet(), 0))
        changeState(&sMiiverseCommentWaitState);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A80B8 */
void hdMenuMiiverseComment_c::resetToWait() {
    mFilled = 0;
    mSavedY = sMiiverseCommentZero;
    mSelectionPending = 0;
    mPictureSlideStep = sMiiverseCommentZero;
    mArchiveIndex = 0;
    mPicturePositionY = sMiiverseCommentZero;
    mCommentRequested = 0;
    mScrollTarget = sMiiverseCommentZero;
    changeState(&sMiiverseCommentWaitState);
}
/* 026A8384 */
void hdMenuMiiverseComment_c::showContent() {
    auto* pane = mPicturePanes[1].get();
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    if (mCommentRequested) {
        pane = mPicturePanes[2].get();
        pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    }
    reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())->showComment();
}
/* 026A7EA0 */
void hdMenuMiiverseComment_c::drawFilled(hdScreenDrawInfo* drawInfo) {
    if (mFilled) {
        gabi::call_ptr<void>(mVtable->mDrawLayout.get(), this, drawInfo);
        gabi::call_ptr<void>(mVtable->mDrawChildren.get(), this, drawInfo);
    }
}
f32 miiverseCommentSlideDuration(hdMiiverseCommentArchive_l*);
/* 026A6AAC */
void hdMenuMiiverseComment_c::enterSlideDown() {
    mSelectionPending = 0;
    const f32 duration = miiverseCommentSlideDuration(sMiiverseCommentArchive);
    const f32 current = layoutPositionY();
    const f32 difference = gabi::fsubs_ppc(current, mPicturePositionY);
    const f32 magnitude = __builtin_fabsf(difference);
    // PowerPC division selects the first NaN operand and quiets its payload.
    f32 step;
    if (magnitude != magnitude)
        step = gabi::ppc_qnan(magnitude);
    else if (duration != duration)
        step = gabi::ppc_qnan(duration);
    else
        step = f32(f64(magnitude) / f64(duration));
    mPictureSlideStep = step;
}
u32 miiverseCommentChaseY(be<f32>*, f32, f32);
// Integer copies preserve signalling-NaN bits and use guest-memory accessors.
static void miiverseCommentCopyTranslationWord(be<f32>& destination, const be<f32>& source) {
    *reinterpret_cast<be<u32>*>(&destination) = *reinterpret_cast<const be<u32>*>(&source);
}
/* 026A6B08 */
void hdMenuMiiverseComment_c::executeSlideDown() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    cXyz translation;
    translation.copy(*reinterpret_cast<const cXyz*>(&miiversePaneTranslationX(pane)));
    const u32 arrived = miiverseCommentChaseY(&translation.y, mPicturePositionY, mPictureSlideStep);
    auto* layout = mpLayout->lyt();
    pane = reinterpret_cast<hdPartsPaneSRT_c*>(layout->mpRootPane.get());
    miiverseCommentCopyTranslationWord(miiversePaneTranslationX(pane), translation.x);
    const u8 flags = pane->mFlags;
    miiverseCommentCopyTranslationWord(miiversePaneTranslationY(pane), translation.y);
    miiverseCommentCopyTranslationWord(miiversePaneTranslationZ(pane), translation.z);
    pane->mFlags = flags | 0x10;
    // These integer operands remain live across the optional state change in GHS.
    gabi::cpu->r[5] = *reinterpret_cast<const be<u32>*>(&translation.z);
    gabi::cpu->r[6] = flags;
    gabi::cpu->r[7] = gabi::ea(layout);
    if (arrived)
        changeState(&sMiiverseCommentWaitState);
    auto* table = mVtable.get();
    gabi::cpu->r[8] = gabi::ea(table);
    gabi::call_ptr<void>(table->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
static hdPartsStateId_c sMiiverseCommentSlideDownState;
/* 026A73F0 */
void hdMenuMiiverseComment_c::enterIn() {
    mSelectionPending = 0;
    for (u32 i = 0; i < 4; ++i)
        mPicturePanes[i]->mAlpha = 0;
    const f32 position = mPicturePositionY;
    mSavedY = position;
    mScrollTarget = sMiiverseCommentZero;
    setLayoutPositionY(position);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A74F4 */
void hdMenuMiiverseComment_c::enterOut() {
    auto* current = reinterpret_cast<hdPartsStateId_c*>(stateMachine()->mpLastStateId.get());
    mSelectionPending = 0;
    const auto currentGetKey = current->mVtable->mGetKey.get();
    auto* slideDownVtable = sMiiverseCommentSlideDownState.mVtable.get();
    const u32 currentKey = gabi::call_ptr<u32>(currentGetKey, current);
    const u32 slideDownKey =
        gabi::call_ptr<u32>(slideDownVtable->mGetKey.get(), &sMiiverseCommentSlideDownState);
    if (currentKey != slideDownKey)
        mPicturePositionY = layoutPositionY();
    mScrollTarget = sMiiverseCommentZero;
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    mSavedY = layoutPositionY();
}
/* 026A7618 */
void hdMenuMiiverseComment_c::finishOut() {
    auto* current = reinterpret_cast<hdPartsStateId_c*>(stateMachine()->mpLastStateId.get());
    const auto currentGetKey = current->mVtable->mGetKey.get();
    auto* slideDownVtable = sMiiverseCommentSlideDownState.mVtable.get();
    const u32 currentKey = gabi::call_ptr<u32>(currentGetKey, current);
    const u32 slideDownKey =
        gabi::call_ptr<u32>(slideDownVtable->mGetKey.get(), &sMiiverseCommentSlideDownState);
    if (currentKey == slideDownKey)
        setLayoutPositionY(mPicturePositionY);
    else
        setLayoutPositionY(mSavedY);
}
// These distinct entry points have identical three-child direction-zero behavior.
/* 026A8814 */
void hdMenuMiiverseComment_c::connectAllAboveVariant1(hdButtonPart_c* control) {
    hdButtonReceiver_l* receiver = control ? control->mpReceiver.get() : nullptr;
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get()), 0,
                                   receiver);
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get()),
                                   0, receiver);
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get()),
                                   0, receiver);
}
/* 026A88B8 */
void hdMenuMiiverseComment_c::connectAllAboveVariant2(hdButtonPart_c* control) {
    hdButtonReceiver_l* receiver = control ? control->mpReceiver.get() : nullptr;
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get()), 0,
                                   receiver);
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get()),
                                   0, receiver);
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get()),
                                   0, receiver);
}
/* 026A895C */
void hdMenuMiiverseComment_c::connectAllAboveVariant3(hdButtonPart_c* control) {
    hdButtonReceiver_l* receiver = control ? control->mpReceiver.get() : nullptr;
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get()), 0,
                                   receiver);
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get()),
                                   0, receiver);
    miiverseCommentConnectReceiver(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get()),
                                   0, receiver);
}
void miiverseCommentSetNavigation(hdButtonPart_c*, hdButtonReceiver_l*, hdButtonReceiver_l*,
                                  hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*,
                                  hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*);
/* 026A8B14 */
void hdMenuMiiverseComment_c::clearContentNavigation() {
    miiverseCommentSetNavigation(reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get()),
                                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 nullptr);
    miiverseCommentSetNavigation(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get()),
                                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 nullptr);
    miiverseCommentSetNavigation(reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get()),
                                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 nullptr);
}
static hdPartsStateId_c sMiiverseCommentInState;
static hdPartsStateId_c sMiiverseCommentInNewPostState;
/* 026A8E54 */
void hdMenuMiiverseComment_c::beginIn() {
    if (!mFilled)
        return;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    for (u32 i = 0; i < 4; ++i)
        mPicturePanes[i]->mAlpha = 0;
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    changeState(&sMiiverseCommentInState);
}
/* 026A9048 */
void hdMenuMiiverseComment_c::beginInNewPost(f32 position) {
    if (!mFilled)
        return;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    for (u32 i = 0; i < 4; ++i)
        mPicturePanes[i]->mAlpha = 255;
    auto* vtable = mVtable.get();
    mPicturePositionY = position;
    gabi::call_ptr<void>(vtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    changeState(&sMiiverseCommentInNewPostState);
    setLayoutPositionY(position);
}
/* 026A8A00 */
void hdMenuMiiverseComment_c::connectAllVerticalControls(hdButtonPart_c* above,
                                                        hdButtonPart_c* below) {
    if (above) {
        auto* child = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseCommentConnectReceiver(child, 0, above->mpReceiver);
        child = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get());
        miiverseCommentConnectReceiver(child, 0, above->mpReceiver);
        child = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get());
        miiverseCommentConnectReceiver(child, 0, above->mpReceiver);
    }
    if (below) {
        auto* child = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseCommentConnectReceiver(child, 3, below->mpReceiver);
        child = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get());
        miiverseCommentConnectReceiver(child, 3, below->mpReceiver);
        child = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get());
        miiverseCommentConnectReceiver(child, 3, below->mpReceiver);
    }
}
hdButtonReceiver_l* miiverseCommentFocusedReceiver(hdButtonEventManager_l*);
/* 026A8290 */
void hdMenuMiiverseComment_c::hideContent() {
    auto* pane = mPicturePanes[1].get();
    pane->mFlags = u8(pane->mFlags) & 0xFE;
    if (mCommentRequested) {
        pane = mPicturePanes[2].get();
        pane->mFlags = u8(pane->mFlags) & 0xFE;
    }
    reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())->hideComment();
    hdButtonEventManager_l* manager = sMiiverseCommentEventManager;
    auto* child = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get());
    const auto* focused = miiverseCommentFocusedReceiver(manager);
    bool focusContent = focused == child->mpReceiver.get();
    if (!focusContent) {
        child = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get());
        focused = miiverseCommentFocusedReceiver(manager);
        focusContent = focused == child->mpReceiver.get();
    }
    if (focusContent) {
        child = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseCommentFocus(manager, child->mpReceiver);
    }
}
/* 026A9150 */
void hdMenuMiiverseComment_c::refreshContentVisibility() {
    if (!mFilled)
        return;
    if (canShowPhoto())
        hideContent();
    else
        showContent();
}
/* 026A8BE8 */
void hdMenuMiiverseComment_c::connectCommentToPicture() {
    if (!mCommentRequested || u32(mArchiveIndex) == 0xFFFFFFFF)
        return;
    const u32 index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
    if (!sMiiverseCommentArchive->mEntries[index].mCommentReady)
        return;
    auto* first = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    auto* picture = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get());
    miiverseCommentConnectReceiver(first, 7, picture->mpReceiver);
    picture = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(2)->get());
    first = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    miiverseCommentSetNavigation(picture, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 first->mpReceiver, nullptr);
}
static hdScreenUiSystem* sMiiverseCommentUiSystem;
static hdUiSafeString sCommentAnimationGroups[1] = {{0, 0}};
static hdUiSafeString sCommentAnimationNames[1] = {{0, 0}};
void miiverseCommentBindAnimationGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
/* 026A965C */
u32 hdMenuMiiverseComment_c::loadLayout(void* name, u32 resource, u32 options, u32 heapOptions,
                                       void* descriptor) {
    hdPartsLayout_c* layout = reinterpret_cast<hdPartsLayout_c*>(
        hdParts_new(sizeof(hdPartsLayout_c), sMiiverseCommentUiSystem->mHeap, 4));
    if (layout)
        layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, resource, options, heapOptions, 1, 1, descriptor))
        return 0;
    miiverseCommentBindAnimationGroup(mpLayout->animSet(), 0, &sCommentAnimationGroups[0],
                                      &sCommentAnimationNames[0]);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}
static hdUiSafeString sMiiverseCommentPicturePaneNames[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
/* 026A7800 */
void hdMenuMiiverseComment_c::findPicturePanes() {
    auto* root = mpLayout->lyt()->mpRootPane.get();
    for (u32 index = 0; index < 4; ++index) {
        mPicturePanes[index] = reinterpret_cast<hdPartsPaneSRT_c*>(
            miiverseFindNamedPane(root, &sMiiverseCommentPicturePaneNames[index]));
    }
}
f32 miiverseCommentNewPostRate(hdMiiverseCommentArchive_l*);
void miiverseCommentPlayAppearSound(const char*);
/* 026A6984 */
void hdMenuMiiverseComment_c::enterInNewPost() {
    mSelectionPending = 0;
    mFilled = 1;
    hdMiiverseCommentArchive_l* archive = sMiiverseCommentArchive;
    auto* layout = mpLayout.get();
    const f32 rate = miiverseCommentNewPostRate(archive);
    hdAnimSet_play(layout->animSet(), 0, 0, rate);
    miiverseCommentPlayAppearSound("SE_MIIVERSE_COMMENT_APPEAR");
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A78DC */
void hdMenuMiiverseComment_c::connectContentPair() {
    auto* first = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    auto* second = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get());
    miiverseCommentSetNavigation(first, nullptr, nullptr, nullptr, nullptr, second->mpReceiver,
                                 nullptr, second->mpReceiver, nullptr);
    second = reinterpret_cast<hdButtonPart_c*>(mChildren.safeSlot(1)->get());
    first = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    miiverseCommentSetNavigation(second, nullptr, nullptr, first->mpReceiver, nullptr, nullptr,
                                 nullptr, nullptr, first->mpReceiver);
}
// PPC array addresses wrap at 32 bits even when a valid table crosses the address boundary.
static hdButtonPart_c* miiverseCommentInputChildAt(gptr<hdParts_c>* children, u32 index,
                                                   u32 count) {
    const u32 selected = index < count ? index : 0;
    const u32 address = u32(gabi::ea(children) + selected * sizeof(gptr<hdParts_c>));
    return reinterpret_cast<hdButtonPart_c*>(gabi::at<gptr<hdParts_c>>(address)->get());
}
/* 026A7F10 */
void hdMenuMiiverseComment_c::drawContent(hdScreenDrawInfo* info) {
    const u32 index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
    const bool photoReady = sMiiverseCommentArchive->mEntries[index].mPhotoReady != 0;
    s32 count = mChildren.mNum;
    if (photoReady) {
        auto* children = mChildren.mpData.get();
        for (u32 childIndex = 0; s32(childIndex) < count; ++childIndex) {
            if (childIndex == 0)
                continue;
            auto* child = miiverseCommentInputChildAt(children, childIndex, u32(count));
            if (child) {
                gabi::call_ptr<void>(child->mVtable->mDraw.get(), child, info);
                count = mChildren.mNum;
                children = mChildren.mpData.get();
            }
        }
        auto* first = miiverseCommentInputChildAt(children, 0, 1);
        if (first)
            gabi::call_ptr<void>(first->mVtable->mDraw.get(), first, info);
    } else {
        for (u32 childIndex = 0; s32(childIndex) < count; ++childIndex) {
            auto* children = mChildren.mpData.get();
            auto* child = miiverseCommentInputChildAt(children, childIndex, u32(count));
            if (child) {
                gabi::call_ptr<void>(child->mVtable->mDraw.get(), child, info);
                count = mChildren.mNum;
            }
        }
    }
}
/* 026A8D40 */
void hdMenuMiiverseComment_c::setPictureEnabled(u32 enabled) {
    auto* picture = mChildren.safeSlot(2)->get();
    auto* table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(picture->mVtable.get());
    gabi::call_ptr<void>(table->mSetEnabled.get(), picture, enabled);
    if (!enabled) {
        auto* content = mChildren.safeSlot(1)->get();
        table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(content->mVtable.get());
        gabi::call_ptr<void>(table->mCancelCursor.get(), content);
    }
}
u32 miiverseCommentCanInteract(hdMiiverseCommentArchive_l*);
/* 026A80F4 */
void hdMenuMiiverseComment_c::refreshContentEnabled() {
    if (!mFilled)
        return;
    const u32 index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
    auto* entry = &sMiiverseCommentArchive->mEntries[index];
    auto* content = mChildren.safeSlot(1)->get();
    auto* table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(content->mVtable.get());
    gabi::call_ptr<void>(table->mCancelCursor.get(), content);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    const u32 enabled = entry->mContentDisabled ? 0 : 1;
    content = mChildren.safeSlot(1)->get();
    table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(content->mVtable.get());
    gabi::call_ptr<void>(table->mSetEnabled.get(), content, enabled);
    if (!miiverseCommentCanInteract(sMiiverseCommentArchive)) {
        content = mChildren.safeSlot(1)->get();
        table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(content->mVtable.get());
        gabi::call_ptr<void>(table->mSetEnabled.get(), content, u32(0));
    }
    reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())->setCommentCount();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A83CC */
void hdMenuMiiverseComment_c::configureContent() {
    auto* picture = mChildren.safeSlot(2)->get();
    auto* table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(picture->mVtable.get());
    gabi::call_ptr<void>(table->mCancelCursor.get(), picture);
    auto* first = (*mChildren.mpData).get();
    cXyz previous;
    previous.copy(*reinterpret_cast<cXyz*>(
        reinterpret_cast<hdBtnMiiverseComment_c*>(first)->getYeahPaneState()));
    const u32 archiveIndex = mArchiveIndex;
    const u32 index = archiveIndex < 10 ? archiveIndex : 0;
    auto* entry = &sMiiverseCommentArchive->mEntries[index];
    reinterpret_cast<hdBtnMiiverseComment_c*>(first)->setPost(archiveIndex);
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    auto* current = reinterpret_cast<cXyz*>(
        reinterpret_cast<hdBtnMiiverseComment_c*>(first)->getYeahPaneState());
    const f32 differenceX = gabi::fsubs_ppc(current->x, previous.x);
    const f32 differenceY = gabi::fsubs_ppc(current->y, previous.y);
    (void)differenceX;
    auto* pane = mPicturePanes[1].get();
    const f32 y = gabi::fadds_ppc(miiversePaneTranslationY(pane), differenceY);
    const u8 flags = pane->mFlags;
    miiversePaneTranslationZ(pane) = sMiiverseCommentZero;
    miiversePaneTranslationY(pane) = y;
    pane->mFlags = flags | 0x10;
    auto* overlay = mPicturePanes[3].get();
    const f32 x = miiversePaneTranslationX(pane);
    const u8 overlayFlags = overlay->mFlags;
    miiversePaneTranslationX(overlay) = x;
    miiversePaneTranslationZ(overlay) = sMiiverseCommentZero;
    miiversePaneTranslationY(overlay) = y;
    overlay->mFlags = overlayFlags | 0x10;
    refreshContentEnabled();
    if (entry->mCommentReady) {
        auto* anchor = reinterpret_cast<cXyz*>(
            reinterpret_cast<hdBtnMiiverseComment_c*>(first)->getPicturePaneState());
        const f32 anchorX = gabi::fsubs_ppc(anchor->x, mContentBaseX);
        const f32 anchorY = gabi::fsubs_ppc(anchor->y, mContentBaseY);
        const f32 pictureX = gabi::fadds_ppc(mPictureBaseX, anchorX);
        const f32 pictureY = gabi::fadds_ppc(mPictureBaseY, anchorY);
        pane = mPicturePanes[2].get();
        const u8 photoFlags = pane->mFlags;
        miiversePaneTranslationX(pane) = pictureX;
        miiversePaneTranslationY(pane) = pictureY;
        pane->mFlags = photoFlags | 0x10;
        miiversePaneTranslationZ(pane) = sMiiverseCommentZero;
        pane = mPicturePanes[2].get();
        pane->mFlags = (u8(pane->mFlags) & ~1) | 1;
        picture = mChildren.safeSlot(2)->get();
        table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(picture->mVtable.get());
        gabi::call_ptr<void>(table->mSetEnabled.get(), picture, u32(1));
        picture = mChildren.safeSlot(2)->get();
        reinterpret_cast<hdBtnMiiversePicture_c*>(picture)->setPicture(entry->mPhotoContent);
        mCommentRequested = 1;
        mPictureRequested = entry->mPictureRequested;
    } else {
        pane = mPicturePanes[2].get();
        pane->mFlags = u8(pane->mFlags) & ~1;
        mCommentRequested = 0;
        mPictureRequested = entry->mPictureRequested;
    }
    if (canShowPhoto())
        hideContent();
    else
        showContent();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026A8F90 */
void hdMenuMiiverseComment_c::setArchiveIndex(u32 index) {
    mArchiveIndex = index;
    mFilled = 1;
    configureContent();
}
void miiverseCommentAnimationOperation(hdAnimSet*, u32, u32);
/* 026A868C */
void hdMenuMiiverseComment_c::copyPresentation(hdMenuMiiverseComment_c* source) {
    mArchiveIndex = source->mArchiveIndex;
    mFilled = source->mFilled;
    const f32 positionY = source->mPicturePositionY;
    auto* layout = mpLayout.get();
    mPicturePositionY = positionY;
    mCommentRequested = source->mCommentRequested;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & ~1) | 1;
    for (u32 index = 0; index < 4; ++index)
        mPicturePanes[index]->mAlpha = 0xFF;
    setLayoutPositionY(mPicturePositionY);
    // The leaf setter preserves the source argument; reload it after the first call.
    setLayoutPositionY(source->mPicturePositionY);
    miiverseCommentAnimationOperation(mpLayout->animSet(), 0, 0);
    changeState(&sMiiverseCommentWaitState);
    configureContent();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
static u8 sMiiverseCommentInteractionAllowed;
static void* sMiiverseCommentPendingRequest;
struct hdMiiverseCommentTickDivisor_l {
    be<u32> mHigh;
    be<u32> mLow;
};
WWHD_OFFSET(hdMiiverseCommentTickDivisor_l, mHigh, 0);
WWHD_OFFSET(hdMiiverseCommentTickDivisor_l, mLow, 4);
WWHD_SIZE(hdMiiverseCommentTickDivisor_l, 8);
static hdMiiverseCommentTickDivisor_l sMiiverseCommentTickDivisor;
void miiverseCommentFinishPending(hdParts_c*);
void miiverseCommentRestoreContent(hdParts_c*);
Pair32 miiverseCommentElapsedTicks(void*);
Pair32 miiverseCommentDivideTicks(u32, u32, u32, u32);
Pair32 miiverseCommentCurrentTicks();
void miiverseCommentPollRequest(void*);
/* 026A7CD8 */
void hdMenuMiiverseComment_c::updateComment() {
    const bool filled = mFilled != 0;
    mOpenPending = 0;
    if (!filled || u32(mArchiveIndex) == 0xFFFFFFFF)
        return;
    if (u32(mPendingState) == 1) {
        if (!mPendingFlags[0]) {
            mPendingState = 0;
            miiverseCommentFinishPending(mChildren.safeSlot(3)->get());
            if (sMiiverseCommentInteractionAllowed) {
                auto* content = mChildren.safeSlot(1)->get();
                auto* table =
                    reinterpret_cast<hdMiiverseCommentContentVtable_l*>(content->mVtable.get());
                gabi::call_ptr<void>(table->mSetEnabled.get(), content, u32(0));
                reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())->setOffCursorFrame();
                u32 index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
                auto* entry = &sMiiverseCommentArchive->mEntries[index];
                entry->mInteractionCount = u32(entry->mInteractionCount) + 1;
                index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
                sMiiverseCommentArchive->mEntries[index].mContentDisabled = 1;
                reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())->setCommentCount();
            } else {
                miiverseCommentRestoreContent(mChildren.safeSlot(1)->get());
            }
            mSelectionPending = 0;
            hdParts_executeStateMachine(stateMachine());
            return;
        }
        const Pair32 elapsed{miiverseCommentElapsedTicks(&mTimerHigh)};
        const Pair32 quotient{miiverseCommentDivideTicks(elapsed.r3, elapsed.r4,
                                                         sMiiverseCommentTickDivisor.mHigh,
                                                         sMiiverseCommentTickDivisor.mLow)};
        if (s32(quotient.r3) > 0 || (quotient.r3 == 0 && quotient.r4 >= 30)) {
            const Pair32 now{miiverseCommentCurrentTicks()};
            mTimerHigh = now.r3;
            mTimerLow = now.r4;
            miiverseCommentPollRequest(sMiiverseCommentPendingRequest);
        }
    }
    hdParts_executeStateMachine(stateMachine());
}
static hdUiSafeString sMiiverseCommentArchiveNames[5] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
static hdUiSafeString sMiiverseCommentLayoutName;
static hdUiSafeString sMiiverseCommentPendingName;
void miiverseCommentAttachArchives(hdParts_c*, hdUiSafeString*, u32);
void miiverseCommentBindSound(hdParts_c*, hdUiSafeString*, u32);
void miiverseCommentBindPendingName(hdParts_c*, hdUiSafeString*);
/* 026A79C8 */
u32 hdMenuMiiverseComment_c::initializeParts(u32 options, u32 heapOptions, void* descriptor) {
    miiverseCommentAttachArchives(this, sMiiverseCommentArchiveNames, 5);
    const auto load = mVtable->mLoad.get();
    auto* resource = mpResAccessor.get();
    if (!gabi::call_ptr<u32>(load, this, &sMiiverseCommentLayoutName, resource, options,
                             heapOptions, descriptor))
        return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this))
        return 0;
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = u8(root->mFlags) & ~1;
    findPicturePanes();
    reinterpret_cast<cXyz*>(&mPictureBaseX)
        ->copy(*reinterpret_cast<const cXyz*>(&miiversePaneTranslationX(mPicturePanes[2].get())));
    reinterpret_cast<cXyz*>(&mContentBaseX)
        ->copy(*reinterpret_cast<cXyz*>(
            reinterpret_cast<hdBtnMiiverseComment_c*>((*mChildren.mpData).get())
                ->getPicturePaneState()));
    auto* first = reinterpret_cast<hdBasicButton_c*>((*mChildren.mpData).get());
    first->mRequestedState = 1;
    auto* picture = reinterpret_cast<hdBasicButton_c*>(mChildren.safeSlot(2)->get());
    picture->mRequestedState = 1;
    if (!miiverseCommentCanInteract(sMiiverseCommentArchive)) {
        auto* contentSlot = mChildren.safeSlot(1);
        auto* content = contentSlot->get();
        auto* table = reinterpret_cast<hdMiiverseCommentContentVtable_l*>(content->mVtable.get());
        // GHS leaves the selected child slot in r6 at this dynamic callback boundary.
        gabi::cpu->r[6] = gabi::ea(contentSlot);
        gabi::call_ptr<void>(table->mSetEnabled.get(), content, u32(0));
    }
    root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = u8(root->mFlags) & ~1;
    for (u32 index = 0; index < 4; ++index)
        mPicturePanes[index]->mAlpha = 0;
    connectContentPair();
    startState(&sMiiverseCommentWaitState);
    auto* content = mChildren.safeSlot(1)->get();
    if (content) {
        hdUiSafeString sound;
        sound.mText.v = gabi::guest_addr("SE_MIIVERSE_CHECK_YEAR");
        sound.mVtable.v = gabi::ea(&miiverseBindings::sCommentSoundStringVtable);
        miiverseCommentBindSound(content, &sound, 0);
    }
    miiverseCommentBindPendingName(mChildren.safeSlot(3)->get(), &sMiiverseCommentPendingName);
    return 1;
}
struct hdMiiverseCommentInputEvents_l {
    hdButtonEvent_l mPosted;
    hdButtonEvent_l mCurrent;
};
WWHD_OFFSET(hdMiiverseCommentInputEvents_l, mCurrent, 0x28);
WWHD_SIZE(hdMiiverseCommentInputEvents_l, 0x50);
static cking::ui::input::Options* sMiiverseCommentInputOptions;
static u32 sMiiverseCommentInputChildren[3] = {1, 2, 0};
static u32 sMiiverseCommentPostedTypes[3] = {103, 104, 102};
void miiverseCommentCopyEvent(hdButtonEvent_l*, const hdButtonEvent_l*);
u32 miiverseCommentIsSpecialEvent(u32);
u32 miiverseCommentIsCancelEvent(u32);
u32 miiverseCommentIsTouchBegin(hdButtonEvent_l*);
u32 miiverseCommentAcceptInput(hdButtonPart_c*);
u32 miiverseCommentReceiverId(hdButtonPart_c*);
u32 miiverseCommentArchiveBusy(hdMiiverseCommentArchive_l*);
void miiverseCommentConstructEvent(hdButtonEvent_l*, u32, u32, void*);
void miiverseCommentQueueEvent(void*, hdButtonEvent_l*);
void miiverseCommentDestroyEvent(hdButtonEvent_l*);
/* 026A6CEC */
u32 hdMenuMiiverseComment_c::processInput() {
    if (!mEnabled)
        return 0;
    hdMiiverseCommentInputEvents_l events;
    if (!canShowPhoto()) {
        miiverseCommentCopyEvent(&events.mCurrent, &sMiiverseCommentEventManager->mCurrentEvent);
        const u32 special = miiverseCommentIsSpecialEvent(events.mCurrent.mType);
        const u32 cancel = miiverseCommentIsCancelEvent(events.mCurrent.mType);
        u32 touch = 0;
        if (sMiiverseCommentInputOptions->mDirectionalInputEnabled)
            touch = miiverseCommentIsTouchBegin(&events.mCurrent);
        if (special | cancel | touch) {
            auto* children = mChildren.mpData.get();
            u32 count = mChildren.mNum;
            for (u32 position = 0; position < 3; ++position) {
                const u32 index = sMiiverseCommentInputChildren[position];
                auto* child = miiverseCommentInputChildAt(children, index, count);
                if (!child)
                    continue;
                const bool accepted = miiverseCommentAcceptInput(child) != 0;
                if (accepted) {
                    const u32 receiver = miiverseCommentReceiverId(child);
                    bool target = cancel && u32(events.mCurrent.mReceiverId) == receiver;
                    if (!target) {
                        auto* focused =
                            miiverseCommentFocusedReceiver(sMiiverseCommentEventManager);
                        target = focused == child->mpReceiver.get() && touch;
                    }
                    if (target) {
                        mSelectionPending = 1;
                        if (index == 2)
                            mOpenPending = 1;
                        miiverseCommentFocus(sMiiverseCommentEventManager, child->mpReceiver);
                        return 0;
                    }
                }
                if (accepted && mSelectionPending) {
                    const u32 receiver = miiverseCommentReceiverId(child);
                    if (u32(events.mCurrent.mReceiverId) == receiver && special) {
                        mSelectionPending = 0;
                        if (miiverseCommentArchiveBusy(sMiiverseCommentArchive))
                            return 0;
                        miiverseCommentClearBuffer(mContentStorage, 0, 0x20);
                        mContentStorage[0] = u32(mArchiveIndex);
                        const u32 postedType = sMiiverseCommentPostedTypes[position];
                        const u32 postedReceiver = miiverseCommentReceiverId(child);
                        miiverseCommentConstructEvent(&events.mPosted, postedType, postedReceiver,
                                                      mContentStorage);
                        miiverseCommentQueueEvent(sMiiverseCommentEventManager->mQueue,
                                                  &events.mPosted);
                        miiverseCommentDestroyEvent(&events.mPosted);
                        return postedType;
                    }
                }
                children = mChildren.mpData.get();
                count = mChildren.mNum;
            }
        }
    }
    auto* children = mChildren.mpData.get();
    u32 count = mChildren.mNum;
    for (u32 position = 0; position < 3; ++position) {
        const u32 index = sMiiverseCommentInputChildren[position];
        auto* child = miiverseCommentInputChildAt(children, index, count);
        if (index != 2 || mCommentRequested) {
            const bool photo = canShowPhoto();
            if (!(photo && (index == 2 || index == 1)) && child) {
                auto* table = reinterpret_cast<hdButtonPartVtable_l*>(child->mVtable.get());
                gabi::call_ptr<void>(table->mOnFocusChanged.get(), child);
            }
        }
        if (position != 2) {
            children = mChildren.mpData.get();
            count = mChildren.mNum;
        }
    }
    return 0;
}
struct hdMiiverseCommentPlayer_l {
    u8 mPrefix[0x3BC];
    be<u32> mFlags;
};
WWHD_OFFSET(hdMiiverseCommentPlayer_l, mFlags, 0x3BC);
struct hdMiiverseCommentPlayInfo_l {
    u8 mPrefix[0x5292];
    be<u8> mEventRunning;
    u8 mBeforePlayer[0x5B2C - 0x5293];
    gptr<hdMiiverseCommentPlayer_l> mpPlayer;
    u8 mBeforeLife[0x5BAC - 0x5B30];
    be<u16> mLife;
    u8 mBeforeStatus[0x5CD8 - 0x5BAE];
    be<u32> mPlayerStatus;
};
WWHD_OFFSET(hdMiiverseCommentPlayInfo_l, mEventRunning, 0x5292);
WWHD_OFFSET(hdMiiverseCommentPlayInfo_l, mpPlayer, 0x5B2C);
WWHD_OFFSET(hdMiiverseCommentPlayInfo_l, mLife, 0x5BAC);
WWHD_OFFSET(hdMiiverseCommentPlayInfo_l, mPlayerStatus, 0x5CD8);
static hdPartsStateId_c sMiiverseCommentPhotoState;
hdMiiverseCommentPlayInfo_l* miiverseCommentGetPlayInfo();
void miiverseCommentBeginPending(hdParts_c*);
void miiverseCommentBeginRequest(void*, void*);
/* 026A716C */
void hdMenuMiiverseComment_c::executeCheckInput() {
    mInteractionFlags[0] = 0;
    processInput();
    if (!miiverseCommentArchiveBusy(sMiiverseCommentArchive)) {
        hdMiiverseCommentInputEvents_l events;
        miiverseCommentCopyEvent(&events.mPosted, &sMiiverseCommentEventManager->mCurrentEvent);
        const u32 type = events.mPosted.mType;
        if (type == 102) {
            mInteractionFlags[0] = 1;
        } else if (type == 103) {
            bool allowed = true;
            if (miiverseCommentGetPlayInfo()->mpPlayer &&
                (u32(miiverseCommentGetPlayInfo()->mpPlayer->mFlags) & 0x200))
                allowed = false;
            if (allowed && miiverseCommentGetPlayInfo()->mEventRunning)
                allowed = false;
            if (allowed && (u32(miiverseCommentGetPlayInfo()->mPlayerStatus) & 0x10))
                allowed = false;
            if (allowed && !miiverseCommentGetPlayInfo()->mLife)
                allowed = false;
            if (allowed && !miiverseCommentCanInteract(sMiiverseCommentArchive))
                allowed = false;
            if (allowed) {
                miiverseCommentCopyEvent(&events.mCurrent,
                                         &sMiiverseCommentEventManager->mCurrentEvent);
                u32 button = events.mCurrent.mButton;
                if (button >= 11)
                    button = 0;
                if (button == u32(mArchiveIndex)) {
                    mPendingState = 1;
                    miiverseCommentBeginPending(mChildren.safeSlot(3)->get());
                    const u32 index = u32(mArchiveIndex) < 10 ? u32(mArchiveIndex) : 0;
                    auto* entry = &sMiiverseCommentArchive->mEntries[index];
                    miiverseCommentBeginRequest(sMiiverseCommentPendingRequest,
                                                entry->mRequestContent);
                    const Pair32 now{miiverseCommentCurrentTicks()};
                    mTimerLow = now.r4;
                    mTimerHigh = now.r3;
                }
            }
        } else if (type == 104 && !miiverseCommentGetPlayInfo()->mEventRunning) {
            changeState(&sMiiverseCommentPhotoState);
        }
    }
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
struct hdMiiverseCommentPartRequest_l {
    u8 mResourcePrefix[8];
    hdUiSafeString mTypeName;
    u8 mResourceGap[4];
    hdUiSafeString mPartName;
    u8 mResourceSuffix[0xC];
};
WWHD_OFFSET(hdMiiverseCommentPartRequest_l, mTypeName, 8);
WWHD_OFFSET(hdMiiverseCommentPartRequest_l, mPartName, 0x14);
WWHD_SIZE(hdMiiverseCommentPartRequest_l, 0x28);
static hdUiSafeString sMiiverseCommentContentClass;
static hdUiSafeString sMiiverseCommentPictureClass;
static hdUiSafeString sMiiverseCommentSequenceClass;
hdParts_c* miiverseCommentConstructSequence(hdParts_c*);
u32 miiverseCommentInitializeSequence(hdParts_c*, u32, u32, hdMiiverseCommentPartRequest_l*);
u32 miiverseCommentCreateDefault(hdParts_c*, u32, hdMiiverseCommentPartRequest_l*);
// String scans use console-width address arithmetic, including a boundary crossing.
static u8 miiverseCommentCharacterAt(const hdPartsChar_t* text, u32 index) {
    return *gabi::at<hdPartsChar_t>(u32(gabi::ea(text) + index));
}
static bool miiverseCommentNamesMatch(hdUiSafeString* name, hdUiSafeString* reference) {
    name->assureTermination();
    name->assureTermination();
    const u32 cachedText = gabi::ea(name->mText.get());
    reference->assureTermination();
    if (cachedText == gabi::ea(reference->mText.get()))
        return true;
    auto* left = name->mText.get();
    auto* right = reference->mText.get();
    for (u32 index = 0; index < 0x40001; ++index) {
        const u8 character = miiverseCommentCharacterAt(left, index);
        if (character != miiverseCommentCharacterAt(right, index))
            return false;
        if (!character)
            return true;
    }
    return false;
}
/* 026A91B4 */
u32 hdMenuMiiverseComment_c::createPart(hdMiiverseCommentPartRequest_l* request) {
    hdScreenHeap* heap = sMiiverseCommentUiSystem->mHeap;
    for (u32 index = 0; index < 4; ++index) {
        if (!miiverseCommentNamesMatch(&request->mPartName,
                                       &sMiiverseCommentPicturePaneNames[index]))
            continue;
        if (miiverseCommentNamesMatch(&request->mTypeName, &sMiiverseCommentContentClass)) {
            auto* destination = mChildren.safeSlot(index);
            auto* content = static_cast<hdParts_c*>(hdParts_new(0x154, heap, 4));
            if (content)
                content = reinterpret_cast<hdParts_c*>(hdBtnMiiverseComment_c::construct(
                    reinterpret_cast<hdBtnMiiverseComment_c*>(content)));
            *destination = content;
            content = mChildren.safeSlot(index)->get();
            if (!content)
                return 0;
            return reinterpret_cast<hdBtnMiiverseComment_c*>(content)->init(
                nullptr, 0, reinterpret_cast<hdPartsDesc_c*>(request));
        }
        if (miiverseCommentNamesMatch(&request->mTypeName, &sMiiverseCommentPictureClass)) {
            auto* destination = mChildren.safeSlot(index);
            auto* picture = static_cast<hdParts_c*>(hdParts_new(0x9C, heap, 4));
            if (picture)
                picture = reinterpret_cast<hdParts_c*>(hdBtnMiiversePicture_c::construct(
                    reinterpret_cast<hdBtnMiiversePicture_c*>(picture)));
            *destination = picture;
            picture = mChildren.safeSlot(index)->get();
            if (!picture)
                return 0;
            return reinterpret_cast<hdBtnMiiversePicture_c*>(picture)->init(
                nullptr, 0, reinterpret_cast<hdPartsDesc_c*>(request));
        }
        if (!miiverseCommentNamesMatch(&request->mTypeName, &sMiiverseCommentSequenceClass))
            return miiverseCommentCreateDefault(this, index, request);
        auto* destination = mChildren.safeSlot(index);
        auto* sequence = static_cast<hdParts_c*>(hdParts_new(0x5C, heap, 4));
        if (sequence)
            sequence = miiverseCommentConstructSequence(sequence);
        *destination = sequence;
        sequence = mChildren.safeSlot(index)->get();
        if (!sequence)
            return 0;
        return miiverseCommentInitializeSequence(sequence, 0, 0, request);
    }
    return 1;
}
namespace miiverseBindings {
static u32 sCommentStateVtable;
}
struct hdMiiverseCommentZeroWords_l {
    be<u32> mWords[4];
};
struct hdMiiverseCommentRegistration_l {
    be<u32> mNext;
    gptr<void> mpObject;
    gfn mDestroy;
};
WWHD_OFFSET(hdMiiverseCommentRegistration_l, mpObject, 4);
WWHD_OFFSET(hdMiiverseCommentRegistration_l, mDestroy, 8);
WWHD_SIZE(hdMiiverseCommentRegistration_l, 12);
static hdMiiverseCommentZeroWords_l sCommentZeroWords;
static hdMiiverseCommentRegistration_l sCommentZeroRegistration;
static hdMiiverseCommentRegistration_l sCommentByteRegistrationA;
static hdMiiverseCommentRegistration_l sCommentByteRegistrationB;
static u8 sCommentRuntimeByteA;
static u8 sCommentRuntimeByteB;
static f32 sCommentAngleMinimum;
static f32 sCommentAngleMaximum;
static f32 sCommentOriginalAngleMinimum;
static f32 sCommentOriginalAngleMaximum;
static hdUiSafeString sCommentChildArchiveNames[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
static u32 sCommentStateCounterReady;
static u32 sCommentStateCounter;
static hdPartsStateId_c sCommentCheckInputState;
void miiverseCommentRegisterGlobal(void*);
void miiverseCommentInitializeRuntimeByteA(void*);
void miiverseCommentInitializeRuntimeByteB(void*);
void miiverseCommentStateInNewPostEnter(hdParts_c*);
void miiverseCommentStateInNewPostExecute(hdParts_c*);
void miiverseCommentStateInNewPostExit(hdParts_c*);
void miiverseCommentStateInNewPostUnused(hdParts_c*);
void miiverseCommentStateSlideDownEnter(hdParts_c*);
void miiverseCommentStateSlideDownExecute(hdParts_c*);
void miiverseCommentStateSlideDownExit(hdParts_c*);
void miiverseCommentStateSlideDownUnused(hdParts_c*);
void miiverseCommentStateWaitEnter(hdParts_c*);
void miiverseCommentStateWaitExecute(hdParts_c*);
void miiverseCommentStateWaitExit(hdParts_c*);
void miiverseCommentStateWaitUnused(hdParts_c*);
void miiverseCommentStateInputEnter(hdParts_c*);
void miiverseCommentStateInputExecute(hdParts_c*);
void miiverseCommentStateInputExit(hdParts_c*);
void miiverseCommentStateInputUnused(hdParts_c*);
void miiverseCommentStateInEnter(hdParts_c*);
void miiverseCommentStateInExecute(hdParts_c*);
void miiverseCommentStateInExit(hdParts_c*);
void miiverseCommentStateInUnused(hdParts_c*);
void miiverseCommentStateOutEnter(hdParts_c*);
void miiverseCommentStateOutExecute(hdParts_c*);
void miiverseCommentStateOutExit(hdParts_c*);
void miiverseCommentStateOutUnused(hdParts_c*);
void miiverseCommentStatePhotoEnter(hdParts_c*);
void miiverseCommentStatePhotoExecute(hdParts_c*);
void miiverseCommentStatePhotoExit(hdParts_c*);
void miiverseCommentStatePhotoUnused(hdParts_c*);
static void miiverseCommentSetDelegate(hdPartsPtmf_c* delegate, const void* callback) {
    delegate->mThisDelta = 0;
    delegate->mVirtualIndex = -1;
    delegate->mFunction = gabi::guest_fn(callback);
}
static void miiverseCommentInitializeState(hdPartsStateId_c* state, u32 number, const char* name,
                                           const void* enter, const void* execute, const void* exit,
                                           const void* unused) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mVtable.v = gabi::ea(&miiverseBindings::sCommentStateVtable);
    miiverseCommentSetDelegate(&state->mEnter, enter);
    miiverseCommentSetDelegate(&state->mExecute, execute);
    miiverseCommentSetDelegate(&state->mExit, exit);
    miiverseCommentSetDelegate(&state->mUnused, unused);
}
static void miiverseCommentInitializeName(hdUiSafeString* name, const char* text) {
    name->mVtable.v = gabi::ea(&miiverseBindings::sCommentSoundStringVtable);
    name->mText.v = gabi::guest_addr(text);
}
/* 026A974C */
static void miiverseCommentInitializeStatics() {
    sCommentZeroWords.mWords[2] = 0;
    sCommentZeroWords.mWords[0] = 0;
    sCommentZeroWords.mWords[3] = 0;
    sCommentZeroWords.mWords[1] = 0;
    miiverseCommentRegisterGlobal(&sCommentZeroRegistration);
    sCommentAngleMinimum = sCommentOriginalAngleMinimum;
    sCommentAngleMaximum = sCommentOriginalAngleMaximum;
    miiverseCommentInitializeRuntimeByteA(&sCommentRuntimeByteA);
    miiverseCommentRegisterGlobal(&sCommentByteRegistrationA);
    miiverseCommentInitializeRuntimeByteB(&sCommentRuntimeByteB);
    miiverseCommentRegisterGlobal(&sCommentByteRegistrationB);
    miiverseCommentInitializeName(&sCommentAnimationGroups[0], "InAll");
    miiverseCommentInitializeName(&sCommentAnimationNames[0], "G_All_00");
    miiverseCommentInitializeName(&sCommentChildArchiveNames[0], "BtnMiiverseComment_00");
    miiverseCommentInitializeName(&sCommentChildArchiveNames[1], "BtnMiiverseYeah_00");
    miiverseCommentInitializeName(&sCommentChildArchiveNames[2], "BtnMiiversePicture_00");
    miiverseCommentInitializeName(&sCommentChildArchiveNames[3], "LoadingIcon_00");
    miiverseCommentInitializeName(&sMiiverseCommentPicturePaneNames[0], "L_BtnComment_00");
    miiverseCommentInitializeName(&sMiiverseCommentPicturePaneNames[1], "L_BtnYeah_00");
    miiverseCommentInitializeName(&sMiiverseCommentPicturePaneNames[2], "L_BtnPicture_00");
    miiverseCommentInitializeName(&sMiiverseCommentPicturePaneNames[3], "L_LoadingIcon_00");
    miiverseCommentInitializeName(&sMiiverseCommentLayoutName, "MenuMiiverseContent_00");
    miiverseCommentInitializeName(&sMiiverseCommentPendingName, "SE_NOW_LOADING");
    miiverseCommentInitializeName(&sMiiverseCommentArchiveNames[0], "MenuMiiverseContent_00");
    miiverseCommentInitializeName(&sMiiverseCommentArchiveNames[1], "BtnMiiverseYeah_00");
    miiverseCommentInitializeName(&sMiiverseCommentArchiveNames[2], "BtnMiiversePicture_00");
    miiverseCommentInitializeName(&sMiiverseCommentArchiveNames[3], "BtnMiiverseComment_00");
    miiverseCommentInitializeName(&sMiiverseCommentArchiveNames[4], "LoadingIcon_00");
    u32 number = 0;
    if (sCommentStateCounterReady)
        number = sCommentStateCounter;
    else
        sCommentStateCounterReady = 1;
    miiverseCommentInitializeState(&sMiiverseCommentInNewPostState, ++number, "::StateID_InNewPost",
                                   (const void*)miiverseCommentStateInNewPostEnter,
                                   (const void*)miiverseCommentStateInNewPostExecute,
                                   (const void*)miiverseCommentStateInNewPostExit,
                                   (const void*)miiverseCommentStateInNewPostUnused);
    miiverseCommentInitializeState(&sMiiverseCommentSlideDownState, ++number, "::StateID_SlideDown",
                                   (const void*)miiverseCommentStateSlideDownEnter,
                                   (const void*)miiverseCommentStateSlideDownExecute,
                                   (const void*)miiverseCommentStateSlideDownExit,
                                   (const void*)miiverseCommentStateSlideDownUnused);
    miiverseCommentInitializeState(
        &sMiiverseCommentWaitState, ++number, "::StateID_Wait",
        (const void*)miiverseCommentStateWaitEnter, (const void*)miiverseCommentStateWaitExecute,
        (const void*)miiverseCommentStateWaitExit, (const void*)miiverseCommentStateWaitUnused);
    miiverseCommentInitializeState(
        &sCommentCheckInputState, ++number, "::StateID_CheckInput",
        (const void*)miiverseCommentStateInputEnter, (const void*)miiverseCommentStateInputExecute,
        (const void*)miiverseCommentStateInputExit, (const void*)miiverseCommentStateInputUnused);
    miiverseCommentInitializeState(
        &sMiiverseCommentInState, ++number, "::StateID_In",
        (const void*)miiverseCommentStateInEnter, (const void*)miiverseCommentStateInExecute,
        (const void*)miiverseCommentStateInExit, (const void*)miiverseCommentStateInUnused);
    miiverseCommentInitializeState(
        &sMiiverseCommentOutState, ++number, "::StateID_Out",
        (const void*)miiverseCommentStateOutEnter, (const void*)miiverseCommentStateOutExecute,
        (const void*)miiverseCommentStateOutExit, (const void*)miiverseCommentStateOutUnused);
    miiverseCommentInitializeState(
        &sMiiverseCommentPhotoState, ++number, "::StateID_Photo",
        (const void*)miiverseCommentStatePhotoEnter, (const void*)miiverseCommentStatePhotoExecute,
        (const void*)miiverseCommentStatePhotoExit, (const void*)miiverseCommentStatePhotoUnused);
    sCommentStateCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sCommentSoundStringVtable 100FC054 nocheck
 data sCommentPartsVtable 100FC2B0 nocheck
 data sCommentHolderVtable 100FC378 nocheck
 data sCommentSecondaryVtable 100FC388 nocheck
 data sCommentStateVtable 100FC0CC nocheck
data sCommentZeroWords 104977E8
data sCommentZeroRegistration 101F6444
data sCommentByteRegistrationA 101F6450
data sCommentByteRegistrationB 101F645C
data sCommentRuntimeByteA 104977E4
data sCommentRuntimeByteB 104977E5
data sCommentAngleMinimum 104977DC
data sCommentAngleMaximum 104977E0
data sCommentOriginalAngleMinimum 100FC154 nocheck
data sCommentOriginalAngleMaximum 100FC158 nocheck
data sCommentChildArchiveNames 10497818 nocheck
data sCommentStateCounterReady 101FD8E4
data sCommentStateCounter 101FDD50
data sCommentCheckInputState 104978EC
ext 028F026C void miiverseCommentRegisterGlobal(void*)
ext 028ED6F8 void miiverseCommentInitializeRuntimeByteA(void*)
ext 028EAB2C void miiverseCommentInitializeRuntimeByteB(void*)
ext 026A6984 void miiverseCommentStateInNewPostEnter(hdParts_c*) callback
ext 026A6A20 void miiverseCommentStateInNewPostExecute(hdParts_c*) callback
ext 026A9C78 void miiverseCommentStateInNewPostExit(hdParts_c*) callback
ext 026A9C7C void miiverseCommentStateInNewPostUnused(hdParts_c*) callback
ext 026A6AAC void miiverseCommentStateSlideDownEnter(hdParts_c*) callback
ext 026A6B08 void miiverseCommentStateSlideDownExecute(hdParts_c*) callback
ext 026A9C84 void miiverseCommentStateSlideDownExit(hdParts_c*) callback
ext 026A9C88 void miiverseCommentStateSlideDownUnused(hdParts_c*) callback
ext 026A6BD8 void miiverseCommentStateWaitEnter(hdParts_c*) callback
ext 026A9C90 void miiverseCommentStateWaitExecute(hdParts_c*) callback
ext 026A9C94 void miiverseCommentStateWaitExit(hdParts_c*) callback
ext 026A9C98 void miiverseCommentStateWaitUnused(hdParts_c*) callback
ext 026A6BE4 void miiverseCommentStateInputEnter(hdParts_c*) callback
ext 026A716C void miiverseCommentStateInputExecute(hdParts_c*) callback
ext 026A73B8 void miiverseCommentStateInputExit(hdParts_c*) callback
ext 026A9CA0 void miiverseCommentStateInputUnused(hdParts_c*) callback
ext 026A73F0 void miiverseCommentStateInEnter(hdParts_c*) callback
ext 026A74A0 void miiverseCommentStateInExecute(hdParts_c*) callback
ext 026A74EC void miiverseCommentStateInExit(hdParts_c*) callback
ext 026A9CA8 void miiverseCommentStateInUnused(hdParts_c*) callback
ext 026A74F4 void miiverseCommentStateOutEnter(hdParts_c*) callback
ext 026A75CC void miiverseCommentStateOutExecute(hdParts_c*) callback
ext 026A7618 void miiverseCommentStateOutExit(hdParts_c*) callback
ext 026A9CB0 void miiverseCommentStateOutUnused(hdParts_c*) callback
ext 026A76A8 void miiverseCommentStatePhotoEnter(hdParts_c*) callback
ext 026A76B4 void miiverseCommentStatePhotoExecute(hdParts_c*) callback
ext 026A9CB8 void miiverseCommentStatePhotoExit(hdParts_c*) callback
ext 026A9CBC void miiverseCommentStatePhotoUnused(hdParts_c*) callback
str 100FC15C "InAll" in 026A974C
str 100FC1FC "G_All_00" in 026A974C
str 100FC1BC "BtnMiiverseComment_00" in 026A974C
str 100FC22C "BtnMiiverseYeah_00" in 026A974C
str 100FC1D4 "BtnMiiversePicture_00" in 026A974C
str 100FC240 "LoadingIcon_00" in 026A974C
str 100FC250 "MenuMiiverseContent_00" in 026A974C
str 100FC164 "L_BtnComment_00" in 026A974C
str 100FC208 "L_BtnYeah_00" in 026A974C
str 100FC174 "L_BtnPicture_00" in 026A974C
str 100FC218 "L_LoadingIcon_00" in 026A974C
str 100FC268 "SE_NOW_LOADING" in 026A974C
str 100FC194 "::StateID_InNewPost" in 026A974C
str 100FC1A8 "::StateID_SlideDown" in 026A974C
str 100FC278 "::StateID_Wait" in 026A974C
str 100FC288 "::StateID_CheckInput" in 026A974C
str 100FC2A0 "::StateID_In" in 026A974C
str 100FC1EC "::StateID_Out" in 026A974C
str 100FC184 "::StateID_Photo" in 026A974C

data sMiiverseCommentContentClass 10490978
data sMiiverseCommentPictureClass 104909FC
data sMiiverseCommentSequenceClass 10495030
ext 02674320 hdParts_c* miiverseCommentConstructSequence(hdParts_c*)
ext 0267438C u32 miiverseCommentInitializeSequence(hdParts_c*, u32, u32, hdMiiverseCommentPartRequest_l*)
ext 026F92C0 u32 miiverseCommentCreateDefault(hdParts_c*, u32, hdMiiverseCommentPartRequest_l*)
data sMiiverseCommentPhotoState 10497970
ext 025200D4 hdMiiverseCommentPlayInfo_l* miiverseCommentGetPlayInfo()
ext 026F7908 void miiverseCommentBeginPending(hdParts_c*)
ext 0203C4A8 void miiverseCommentBeginRequest(void*, void*)
data sMiiverseCommentInputOptions 101F8378
data sMiiverseCommentInputChildren 100FC06C
data sMiiverseCommentPostedTypes 100FC078
ext 020013C0 void miiverseCommentCopyEvent(hdButtonEvent_l*, const hdButtonEvent_l*)
ext 026F620C u32 miiverseCommentIsSpecialEvent(u32)
ext 026F61DC u32 miiverseCommentIsCancelEvent(u32)
ext 026F6288 u32 miiverseCommentIsTouchBegin(hdButtonEvent_l*)
ext 026F2458 u32 miiverseCommentAcceptInput(hdButtonPart_c*)
ext 026F5B74 u32 miiverseCommentReceiverId(hdButtonPart_c*)
ext 020399DC u32 miiverseCommentArchiveBusy(hdMiiverseCommentArchive_l*)
ext 02001338 void miiverseCommentConstructEvent(hdButtonEvent_l*, u32, u32, void*)
ext 02001FF4 void miiverseCommentQueueEvent(void*, hdButtonEvent_l*)
ext 02001490 void miiverseCommentDestroyEvent(hdButtonEvent_l*)
str 100FC13C "SE_MIIVERSE_CHECK_YEAR" in 026A79C8
data sMiiverseCommentArchiveNames 10497838 nocheck
data sMiiverseCommentLayoutName 10497860
data sMiiverseCommentPendingName 104977D4
ext 026F90D8 void miiverseCommentAttachArchives(hdParts_c*, hdUiSafeString*, u32)
ext 026F2540 void miiverseCommentBindSound(hdParts_c*, hdUiSafeString*, u32)
ext 026F7948 void miiverseCommentBindPendingName(hdParts_c*, hdUiSafeString*)
data sMiiverseCommentInteractionAllowed 1018F510
data sMiiverseCommentPendingRequest 1018F504
data sMiiverseCommentTickDivisor 104A11C8
ext 026F7924 void miiverseCommentFinishPending(hdParts_c*)
ext 026F17BC void miiverseCommentRestoreContent(hdParts_c*)
ext 02760E58 Pair32 miiverseCommentElapsedTicks(void*)
ext 028F5B8C Pair32 miiverseCommentDivideTicks(u32, u32, u32, u32)
ext C0009C98 Pair32 miiverseCommentCurrentTicks()
ext 0203C780 void miiverseCommentPollRequest(void*)
ext 02005708 void miiverseCommentAnimationOperation(hdAnimSet*, u32, u32)
ext 0203A438 u32 miiverseCommentCanInteract(hdMiiverseCommentArchive_l*)
str 100FC11C "SE_MIIVERSE_COMMENT_APPEAR" in 026A6984
ext 0203A958 f32 miiverseCommentNewPostRate(hdMiiverseCommentArchive_l*)
ext 02030B38 void miiverseCommentPlayAppearSound(const char*)
data sMiiverseCommentPicturePaneNames 104977F8 nocheck
data sCommentAnimationGroups 104977CC nocheck
data sCommentAnimationNames 104977C4 nocheck
ext 02004E04 void miiverseCommentBindAnimationGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
data sMiiverseCommentUiSystem 1018C404
ext 02615140 hdButtonReceiver_l* miiverseCommentFocusedReceiver(hdButtonEventManager_l*)
data sMiiverseCommentInState 10497918
data sMiiverseCommentInNewPostState 10497868
ext 026F5C08 void miiverseCommentSetNavigation(hdButtonPart_c*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*)
data sMiiverseCommentSlideDownState 10497894
ext 0200F5C8 u32 miiverseCommentChaseY(be<f32>*, f32, f32)
ext 0203A910 f32 miiverseCommentSlideDuration(hdMiiverseCommentArchive_l*)
data sMiiverseCommentWaitState 104978C0
ext 02005840 u32 miiverseCommentAnimationFinished(hdAnimSet*, u32)
data sMiiverseCommentArchive 1018F4AC
data sMiiverseCommentSaveInfo 101F84DC
ext 027200F4 hdSavePair_c* miiverseCommentScratchPair(hdSaveHolders_c*)
ext 026F91F8 void miiverseCommentBaseDetachArchives(hdParts_c*)
ext 026F89E8 hdParts_c* miiverseCommentBaseConstruct(hdParts_c*)
ext 026F88F0 void miiverseCommentBaseDestroy(hdParts_c*, s32)
ext 02704D2C void miiverseCommentGlobalTranslation(cXyz*, hdPartsPaneSRT_c*)
ext 0273AD10 void* miiverseCommentAllocate(u32)
ext 02760DE8 void miiverseCommentInitializeTimer(void*)
ext C0009990 void* miiverseCommentClearBuffer(void*, u32, u32)
data sMiiverseCommentOutState 10497944
data sMiiverseCommentZero 100FC138 nocheck
data sMiiverseCommentEventManager 101F4FF0
ext 0273AF40 void miiverseCommentDelete(void*)
ext 02615154 void miiverseCommentFocus(void*, hdButtonReceiver_l*)
ext 026F5C00 void miiverseCommentConnectReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*)
ext 0263E430 hdBtnMiiverseComment_c* hdBtnMiiverseComment_c::construct(hdBtnMiiverseComment_c*)
ext 02641540 hdBtnMiiversePicture_c* hdBtnMiiversePicture_c::construct(hdBtnMiiversePicture_c*)
ext 0263EFB0 u32 hdBtnMiiverseComment_c::init(void*, u32, hdPartsDesc_c*)
ext 026415CC u32 hdBtnMiiversePicture_c::init(void*, u32, hdPartsDesc_c*)
ext 02640A04 void hdBtnMiiverseComment_c::setOffCursorFrame()
ext 026409EC gptr<void>* hdBtnMiiverseComment_c::getYeahPaneState()
ext 026409F8 gptr<void>* hdBtnMiiverseComment_c::getPicturePaneState()
ext 0263F4F0 void hdBtnMiiverseComment_c::setPost(u32)
ext 026418BC void hdBtnMiiversePicture_c::setPicture(void*)
ext 0263F124 void hdBtnMiiverseComment_c::setCommentCount()
ext 0263F35C void hdBtnMiiverseComment_c::hideComment()
ext 0263F3F4 void hdBtnMiiverseComment_c::showComment()
ext 0263F118 f32 hdBtnMiiverseComment_c::getPictureHeight()
ext 026418A4 f32 hdBtnMiiversePicture_c::getWidth()
ext 026418B0 f32 hdBtnMiiversePicture_c::getHeight()
*/
