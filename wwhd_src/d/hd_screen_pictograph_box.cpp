/* HD Picto Box window transitions and pane ownership. */
#include "d/hd_screen_pictograph_box.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_parts_button.h"
using namespace gabi;
struct hdPictoRootNames { hdUiSafeString mNames[2]; };
struct hdPictoPictureNames { hdUiSafeString mNames[3]; };
static hdPictoRootNames sBoxRootNames;
static u32 sBoxRootNamesReady;
static hdPictoRegistration sBoxRootRegistration;
static u8 sBoxStringVtable;
static u8 sBoxHolderVtable;
static u8 sBoxSecondaryVtable;
static hdPartsVtbl_c sBoxVtable;
static f32 sBoxZero;
static f32 sBoxOne;
static f32 sBoxMotionFrames;
static hdPartsStateId_c sBoxWaitState;
static hdPartsStateId_c sBoxDisplayState;
static EventMgr* sBoxInput;
struct hdPictoButtonActions { be<u32> mIndices[2], mEvents[2]; };
static hdPictoButtonActions sBoxButtonActions;
extern hdUiSafeString& sBackgroundName;
extern hdUiSafeString& sMessageName;
extern hdPictoPictureNames& sPictureNames;
void pictographBoxDestroyBase(hdParts_c*, u32);
hdParts_c* pictographBoxConstructBase(hdParts_c*);
void* pictographAllocate(u32);
void pictographFree(void*);
void pictographDestroySafeString(hdUiSafeString*, u32);
void pictographDestroyStrings(void*, u32, u32, u32, u32, u32);
void pictographBoxPlayAnimation(hdAnimSet*, u32, u32, f32);
void pictographBoxRegisterRoots(hdPictoRegistration*);
f32 pictographBoxSqrt(f32);
u32 pictographBoxChasePosition(cXyz*, cXyz*, f32);
u32 pictographBoxChaseFloat(be<f32>*, f32, f32);
u32 pictographBoxAnimationFinished(hdAnimSet*, u32);
void pictographBoxCopyEvent(hdButtonEvent_l*, hdButtonEvent_l*);
u32 pictographBoxAcceptEvent(u32);
u32 pictographBoxButtonReady(hdParts_c*);
u32 pictographBoxReceiverId(hdParts_c*);
void pictographBoxMakeEvent(hdButtonEvent_l*, u32, u32, u32);
void pictographBoxQueueEvent(void*, hdButtonEvent_l*);
void pictographBoxDestroyEvent(hdButtonEvent_l*);
/* 026C3A0C */
u32 pictographBoxStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026C3B0C */
void destroyPictographRootNames() {
    pictographDestroyStrings(&sBoxRootNames, 2, 8, guest_fn(reinterpret_cast<const void*>(pictographDestroySafeString)), 0, 0);
}
/* 026C3B30 */
void destroyPictographBackgroundName() {
    pictographDestroyStrings(&sBackgroundName, 1, 8, guest_fn(reinterpret_cast<const void*>(pictographDestroySafeString)), 0, 0);
}
/* 026C3B54 */
void destroyPictographPictureNames() {
    pictographDestroyStrings(&sPictureNames, 3, 8, guest_fn(reinterpret_cast<const void*>(pictographDestroySafeString)), 0, 0);
}
/* 026C3B78 */
void destroyPictographMessageName() {
    pictographDestroyStrings(&sMessageName, 1, 8, guest_fn(reinterpret_cast<const void*>(pictographDestroySafeString)), 0, 0);
}
/* 026C3B9C */
void hdPictographBox::destroy(hdPictographBox* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable = &sBoxHolderVtable;
    self->mSecondaryVtable = &sBoxSecondaryVtable;
    pictographBoxDestroyBase(self, 0);
    if (flags&1) pictographFree(self);
}
/* 026C3C08 */
void hdPictographBox::inAllInitialize() {
    f32 rate = sBoxOne;
    pictographBoxPlayAnimation(animSet(), 0, 0, rate);
}
/* 026C3C24 */
void hdPictographBox::inAllMove() { vCalcLayout(); }
/* 026C3C34 */
void hdPictographBox::outAllInitialize() {
    f32 rate = sBoxOne;
    pictographBoxPlayAnimation(animSet(), 1, 0, rate);
}
/* 026C3C50 */
void hdPictographBox::outAllMove() { vCalcLayout(); }
/* 026C3C60 */
void hdPictographBox::initializeInMotion() {
    f32 one = sBoxOne;
    pictographBoxPlayAnimation(animSet(), 0, 0, one);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = (u8(root->mFlags)&~1u)+1;
    auto copyWord = [](be<f32>& destination, const be<f32>& source) {
        reinterpret_cast<be<u32>&>(destination) = reinterpret_cast<const be<u32>&>(source);
    };
    copyWord(mCurrentTranslation.y, mLocalPosition.y);
    copyWord(mCurrentPosition.y, mTargetPosition.y);
    copyWord(mCurrentPosition.z, mTargetPosition.z);
    mTransitionAlpha = one;
    copyWord(mCurrentTranslation.z, mLocalPosition.z);
    copyWord(mCurrentTranslation.x, mLocalPosition.x);
    copyWord(mCurrentPosition.x, mTargetPosition.x);
    auto* pane = mPicturePanes[0].get();
    pane->mTranslationXBits = u32(reinterpret_cast<const be<u32>&>(mCurrentPosition.x));
    u8 flags = pane->mFlags;
    pane->mTranslationYBits = u32(reinterpret_cast<const be<u32>&>(mCurrentPosition.y));
    u32 z = u32(reinterpret_cast<const be<u32>&>(mCurrentPosition.z));
    pane->mFlags = flags|0x10;
    pane->mTranslationZBits = z;
    pane = mPicturePanes[0].get();
    f32 scale = mPictureScale;
    flags = pane->mFlags;
    pane->mScaleX = scale;
    pane->mScaleY = scale;
    pane->mFlags = flags|0x10;
    f32 dx = fsubs_ppc(mAnchorPosition.x, mTargetPosition.x);
    f32 dy = fsubs_ppc(mAnchorPosition.y, mTargetPosition.y);
    f32 dz = fsubs_ppc(mAnchorPosition.z, mTargetPosition.z);
    f32 squared = fmadds(dx, dx, fmuls_ppc(dy, dy));
    mMotionStep.x = dx;
    mMotionStep.z = dz;
    mMotionStep.y = dy;
    f32 length = pictographBoxSqrt(fmadds(dz, dz, squared));
    dx = mMotionStep.x; dy = mMotionStep.y;
    squared = fmadds(dx, dx, fmuls_ppc(dy, dy));
    f32 stepLength = f32(length/f32(sBoxMotionFrames));
    dz = mMotionStep.z;
    f32 inverse = f32(one/pictographBoxSqrt(fmadds(dz, dz, squared)));
    dx = mMotionStep.x; dz = mMotionStep.z; dy = mMotionStep.y;
    dx = fmuls_ppc(dx, inverse);
    dz = fmuls_ppc(dz, inverse);
    dy = fmuls_ppc(dy, inverse);
    u8 message = mMessageShown;
    mMotionStep.x = dx;
    mMotionStep.y = dy;
    mMotionStep.z = fmuls_ppc(dz, stepLength);
    mMotionStep.x = fmuls_ppc(dx, stepLength);
    mMotionStep.y = fmuls_ppc(dy, stepLength);
    auto* messagePane = mpMessageRoot.get();
    flags = messagePane->mFlags;
    messagePane->mFlags = (flags&~1u)+(message?1:0);
    vExecuteChildren();
    vCalcLayout();
}
/* 026C3E8C */
u32 hdPictographBox::chasePosition() {
    f32 x = mMotionStep.x, y = mMotionStep.y;
    auto* pane = mPicturePanes[0].get();
    Local<cXyz> position, target;
    position->x = f32(reinterpret_cast<be<f32>&>(pane->mTranslationXBits));
    f32 z = f32(reinterpret_cast<be<f32>&>(pane->mTranslationZBits));
    target->z = mCurrentTranslation.z;
    position->y = f32(reinterpret_cast<be<f32>&>(pane->mTranslationYBits));
    f32 lengthSquared = fmadds(x, x, fmuls_ppc(y, y));
    f32 stepZ = mMotionStep.z;
    target->y = mCurrentTranslation.y;
    position->z = z;
    target->x = mCurrentTranslation.x;
    f32 distance = pictographBoxSqrt(fmadds(stepZ, stepZ, lengthSquared));
    u32 result = pictographBoxChasePosition(position, target, distance);
    pane = mPicturePanes[0].get();
    f32 newX = position->x, newZ = position->z, newY = position->y;
    u8 flags = pane->mFlags;
    reinterpret_cast<be<f32>&>(pane->mTranslationZBits) = newZ;
    reinterpret_cast<be<f32>&>(pane->mTranslationYBits) = newY;
    pane->mFlags = flags|0x10;
    reinterpret_cast<be<f32>&>(pane->mTranslationXBits) = newX;
    return result != 0;
}
/* 026C3F40 */
u32 hdPictographBox::chaseScale() {
    Local<be<f32>> scale;
    *scale=mPicturePanes[0]->mScaleX;
    u32 result = pictographBoxChaseFloat(scale, mTransitionAlpha, mMotionProgress);
    auto* pane = mPicturePanes[0].get();
    f32 value = *scale;
    u8 flags = pane->mFlags;
    pane->mScaleX = value;
    pane->mScaleY = value;
    pane->mFlags = flags|0x10;
    reinterpret_cast<hdPictographBackdrop*>(child(0))->setAlpha(*scale);
    return result != 0;
}
/* 026C3FD4 */
void hdPictographBox::updateInMotion() {
    u32 positionDone = chasePosition();
    u32 scaleDone = chaseScale();
    vExecuteChildren();
    vCalcLayout();
    if (positionDone && scaleDone && pictographBoxAnimationFinished(animSet(), 0))
        changeState(&sBoxDisplayState);
}
/* 026C4080 */
void hdPictographBox::initializeOutMotion() {
    pictographBoxPlayAnimation(animSet(), 1, 0, sBoxOne);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = (u8(root->mFlags)&~1u)+1;
    f32 scale = mPictureScale;
    mCurrentTranslation.copy(mCurrentPosition);
    mTransitionAlpha = scale;
    vExecuteChildren();
    vCalcLayout();
}
/* 026C4128 */
void hdPictographBox::updateOutMotion() {
    u32 positionDone = chasePosition();
    u32 scaleDone = chaseScale();
    vExecuteChildren();
    vCalcLayout();
    if (positionDone && scaleDone && pictographBoxAnimationFinished(animSet(), 0))
        changeState(&sBoxWaitState);
}
/* 026C41D4 */
void hdPictographBox::showRoot() {
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = (u8(root->mFlags)&~1u)+1;
}
/* 026C41F4 */
void hdPictographBox::displayMove() { vCalcLayout(); }
/* 026C4204 */
void hdPictographBox::hideRoot() {
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = u8(root->mFlags)&~1u;
}
/* 026C4220 */
void hdPictographBox::enterWait() {
    vCalcLayout();
    changeState(&sBoxWaitState);
}
/* 026C4268 */
void hdPictographBox::finishOut() {
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = u8(root->mFlags)&~1u;
}
/* 026C4284 */
void hdPictographBox::setRightInputMode() {
    hdParts_c* part = child(2);
    EventMgr* input = sBoxInput;
    input->setModeDefault(reinterpret_cast<hdButtonPart_c*>(part)->mpReceiver.v.get());
}
/* 026C42AC */
u32 hdPictographBox::dispatchInput() {
    Local<hdButtonEvent_l> event;
    EventMgr* input = sBoxInput;
    pictographBoxCopyEvent(event, reinterpret_cast<hdButtonEvent_l*>(&input->mEventQueue[0x1C]));
    u32 accepted = pictographBoxAcceptEvent(event->mType);
    auto* slots = mChildren.mpData.get();
    u32 count = u32(mChildren.mNum);
    if (accepted) {
        for (u32 index = 0; index<2; ++index) {
            u32 childIndex = sBoxButtonActions.mIndices[index];
            hdParts_c* part = slots[childIndex<count?childIndex:0].get();
            if (!part) continue;
            hdParts_c* checked = slots[childIndex<count?childIndex:0].get();
            if (pictographBoxButtonReady(checked) && event->mReceiverId == pictographBoxReceiverId(part)) {
                u32 type = sBoxButtonActions.mEvents[index];
                u32 receiver = pictographBoxReceiverId(part);
                Local<hdButtonEvent_l> notification;
                pictographBoxMakeEvent(notification, type, receiver, 0);
                input = sBoxInput;
                pictographBoxQueueEvent(input->mEventQueue, notification);
                pictographBoxDestroyEvent(notification);
                return type;
            }
            slots = mChildren.mpData.get();
            count = u32(mChildren.mNum);
        }
    }
    for (u32 index = 0; index<2; ++index) {
        u32 childIndex = sBoxButtonActions.mIndices[index];
        hdParts_c* part = slots[childIndex<count?childIndex:0].get();
        if (part) {
            auto* table = reinterpret_cast<hdButtonPartVtable_l*>(part->mVtable.get());
            call_ptr(table->mOnFocusChanged.get(), part);
        }
        if (index == 0) { slots = mChildren.mpData.get(); count = u32(mChildren.mNum); }
    }
    return 0;
}
/* 026C4450 */
void hdPictographBox::updateWait() {
    dispatchInput();
    vExecuteChildren();
    vCalcLayout();
}
/* 026C44A4 */
void hdPictographBox::clearInputMode() {
    sBoxInput->setModeDefault(0);
    mMessageShown = 0;
}
/* 026C44E4 */
hdPictographBox* hdPictographBox::construct(hdPictographBox* self) {
    if (!self) {
        self = static_cast<hdPictographBox*>(pictographAllocate(sizeof(hdPictographBox)));
        if (!self) return NULL;
    }
    pictographBoxConstructBase(self);
    self->mStateHolder.mHolderVtable = &sBoxHolderVtable;
    f32 zero = sBoxZero;
    self->mMotionProgress = zero;
    self->mVtable = &sBoxVtable;
    self->mCurrentTranslation.y = zero;
    self->mCurrentTranslation.x = zero;
    self->mTargetPosition.y = zero;
    self->mTargetPosition.x = zero;
    self->mLocalPosition.y = zero;
    self->mAnchorPosition.x = zero;
    self->mMotionStep.z = zero;
    self->mTargetPosition.z = zero;
    self->mTransitionAlpha = zero;
    self->mPictureWidth = zero;
    self->mAnchorPosition.z = zero;
    self->mMessageShown = 0;
    self->mAnchorPosition.y = zero;
    self->mMotionStep.y = zero;
    self->mPictureHeight = zero;
    self->mCurrentPosition.x = zero;
    self->mCurrentPosition.z = zero;
    self->mPictureScale = zero;
    self->mCurrentPosition.y = zero;
    self->mSecondaryVtable = &sBoxSecondaryVtable;
    self->mMotionStep.x = zero;
    u32 roots = ea(self)+offsetof(hdPictographBox, mpAllRoot);
    self->mClosing = 0;
    self->mInitialScale = zero;
    self->mCurrentTranslation.z = zero;
    self->mLocalPosition.z = zero;
    self->mLocalPosition.x = zero;
    self->mTargetScale = zero;
    if (!roots) pictographAllocate(8);
    if (u32(ea(self)+offsetof(hdPictographBox, mpBackground)) == 0) pictographAllocate(4);
    if (u32(ea(self)+offsetof(hdPictographBox, mPicturePanes)) == 0) pictographAllocate(12);
    if (u32(ea(self)+offsetof(hdPictographBox, mpMessageText)) == 0) pictographAllocate(4);
    return self;
}
/* 026C4608 */
void hdPictographBox::bindRootPanes() {
    if (!sBoxRootNamesReady) {
        sBoxRootNamesReady = 1;
        sBoxRootNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sBoxStringVtable);
        sBoxRootNames.mNames[0].mText.v = guest_addr("N_All_00");
        sBoxRootNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sBoxStringVtable);
        sBoxRootNames.mNames[1].mText.v = guest_addr("N_Msg_00");
        pictographBoxRegisterRoots(&sBoxRootRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    gptr<hdPartsPaneSRT_c>* slots = &mpAllRoot;
    for (u32 index = 0; index<2; ++index) {
        hdUiSafeString* name = &sBoxRootNames.mNames[index];
        auto* table = root->srtVtbl();
        name->assureTermination();
        slots[index] = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
    }
}
/* WWHD glue
ext 02615154 void cking::ui::input::EventMgr::setModeDefault(u32)
data sBoxRootNames 104999BC
data sBoxRootNamesReady 10499A54
data sBoxRootRegistration 101F68CC
data sBoxStringVtable 10100C1C
data sBoxHolderVtable 10100FC8
data sBoxSecondaryVtable 10100FD8
data sBoxVtable 10100ED0
data sBoxZero 10100D18
data sBoxOne 10100D10
data sBoxMotionFrames 10100D14
data sBoxWaitState 10499B7C
data sBoxDisplayState 10499B24
ext 028F4384 f32 pictographBoxSqrt(f32)
ext 0200F62C u32 pictographBoxChasePosition(cXyz*,cXyz*,f32)
ext 0200F5C8 u32 pictographBoxChaseFloat(be<f32>*,f32,f32)
ext 02005840 u32 pictographBoxAnimationFinished(hdAnimSet*,u32)
data sBoxInput 101F4FF0
data sBoxButtonActions 10100C08
ext 020013C0 void pictographBoxCopyEvent(hdButtonEvent_l*,hdButtonEvent_l*)
ext 026F620C u32 pictographBoxAcceptEvent(u32)
ext 026F2458 u32 pictographBoxButtonReady(hdParts_c*)
ext 026F5B74 u32 pictographBoxReceiverId(hdParts_c*)
ext 02001338 void pictographBoxMakeEvent(hdButtonEvent_l*,u32,u32,u32)
ext 02001FF4 void pictographBoxQueueEvent(void*,hdButtonEvent_l*)
ext 02001490 void pictographBoxDestroyEvent(hdButtonEvent_l*)
str 10100D1C "N_All_00" in 026C4608
str 10100D28 "N_Msg_00" in 026C4608
ext 026F88F0 void pictographBoxDestroyBase(hdParts_c*,u32)
ext 026F89E8 hdParts_c* pictographBoxConstructBase(hdParts_c*)
ext 026C578C void pictographDestroySafeString(hdUiSafeString*,u32) callback
ext 028F0164 void pictographDestroyStrings(void*,u32,u32,u32,u32,u32)
ext 020053E4 void pictographBoxPlayAnimation(hdAnimSet*,u32,u32,f32)
ext 028F026C void pictographBoxRegisterRoots(hdPictoRegistration*)
*/
