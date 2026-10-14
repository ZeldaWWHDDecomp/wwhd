/** HD Miiverse settings menu state and navigation. Private binary-derived source. */
#include "d/hd_screen_menu_miiverse_setting.h"
void miiverseSettingUpdateState(hdStateMachine*);
void miiverseSettingConnectReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*);
static f32 sMiiverseSettingLowerOffset;
static hdPartsStateId_c sMiiverseSettingOutState;
static hdPartsStateId_c sMiiverseSettingInState;
void miiverseSettingMenuBaseDetachArchives(hdParts_c*);
/* 026ABA7C */
void hdMenuMiiverseSetting_c::updateState() {
    miiverseSettingUpdateState(stateMachine());
}
/* 026ABB24 */
f32 hdMenuMiiverseSetting_c::lowerBound() const {
    return gabi::fadds_ppc(mPositionY, sMiiverseSettingLowerOffset);
}
/* 026ABB38 */
f32 hdMenuMiiverseSetting_c::sequenceHeight() const {
    return mpSequencePane->mSizeY;
}
/* 026ABA84 */
void hdMenuMiiverseSetting_c::connectUpperControl(hdButtonPart_c* control) {
    if (control) {
        auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseSettingConnectReceiver(control, 3, content->mpReceiver);
    }
}
/* 026ABAAC */
void hdMenuMiiverseSetting_c::connectContentAbove(hdButtonPart_c* control) {
    hdButtonReceiver_l* receiver = control ? control->mpReceiver.get() : nullptr;
    auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    miiverseSettingConnectReceiver(content, 0, receiver);
}
/* 026ABB7C */
void hdMenuMiiverseSetting_c::beginOut() {
    changeState(&sMiiverseSettingOutState);
}
/* 026ABA28 */
void hdMenuMiiverseSetting_c::finalizeParts() {
    gabi::call_ptr<void>(mVtable->mDeleteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    miiverseSettingMenuBaseDetachArchives(this);
}
/* 026ABB44 */
void hdMenuMiiverseSetting_c::beginIn() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    mpSequencePane->mAlpha = 0;
    changeState(&sMiiverseSettingInState);
}
void miiverseSettingSetNavigation(hdButtonPart_c*, hdButtonReceiver_l*, hdButtonReceiver_l*,
                                  hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*,
                                  hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*);
/* 026ABAD4 */
void hdMenuMiiverseSetting_c::clearContentNavigation() {
    miiverseSettingSetNavigation(reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get()),
                                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 nullptr);
}
/* 026ABB8C */
void hdMenuMiiverseSetting_c::translateLayoutY(f32 delta) {
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
/* 026ABBE0 */
void hdMenuMiiverseSetting_c::setSequencePresentation(u32 alpha, f32 position) {
    mpSequencePane->mAlpha = u8(alpha);
    auto* layout = mpLayout.get();
    auto* source = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const f32 x = miiversePaneTranslationX(source);
    const f32 oldPosition = mPresentationOffset;
    const f32 z = miiversePaneTranslationZ(source);
    const f32 oldY = miiversePaneTranslationY(source);
    const f32 delta = gabi::fsubs_ppc(position, oldPosition);
    auto* destination = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const f32 y = gabi::fadds_ppc(oldY, delta);
    const u8 flags = destination->mFlags;
    miiversePaneTranslationX(destination) = x;
    miiversePaneTranslationY(destination) = y;
    destination->mFlags = flags | 0x10;
    miiversePaneTranslationZ(destination) = z;
    mPresentationOffset = position;
}
static hdUiSafeString sMiiverseSettingPartName;
u32 miiverseSettingCreateDefaultPart(hdParts_c*, u32, hdMiiversePartRequest_l*);
// Preserve 32-bit guest address wrapping during the bounded string comparison.
static u8 miiverseSettingCharacterAt(const hdPartsChar_t* text, u32 index) {
    return *gabi::at<hdPartsChar_t>(u32(gabi::ea(text) + index));
}
/* 026ABC48 */
u32 hdMenuMiiverseSetting_c::createSequencePart(hdMiiversePartRequest_l* request) {
    hdUiSafeString* name = &request->mPartName;
    name->assureTermination();
    name->assureTermination();
    const u32 cachedText = gabi::ea(name->mText.get());
    sMiiverseSettingPartName.assureTermination();
    bool sameName = cachedText == gabi::ea(sMiiverseSettingPartName.mText.get());
    if (!sameName) {
        auto* left = name->mText.get();
        auto* right = sMiiverseSettingPartName.mText.get();
        for (u32 index = 0; index < 0x40001; ++index) {
            const u8 character = miiverseSettingCharacterAt(left, index);
            if (character != miiverseSettingCharacterAt(right, index))
                break;
            if (!character) {
                sameName = true;
                break;
            }
        }
    }
    if (!sameName)
        return 1;
    return miiverseSettingCreateDefaultPart(this, 0, request);
}
static hdPartsStateId_c sMiiverseSettingWaitState;
static hdUiSafeString sSettingMenuArchiveNames[2] = {{0, 0}, {0, 0}};
static hdUiSafeString sSettingMenuLayoutName;
void miiverseSettingMenuAttachArchives(hdParts_c*, hdUiSafeString*, u32);
/* 026AB8CC */
u32 hdMenuMiiverseSetting_c::initializeParts(u32 options, u32 heapOptions, void* descriptor) {
    miiverseSettingMenuAttachArchives(this, sSettingMenuArchiveNames, 2);
    const auto load = mVtable->mLoad.get();
    auto* resource = mpResAccessor.get();
    if (!gabi::call_ptr<u32>(load, this, &sSettingMenuLayoutName, resource, options, heapOptions,
                             descriptor))
        return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this))
        return 0;
    findSequencePane();
    auto* layout = mpLayout.get();
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const u8 rootFlags = root->mFlags;
    const u8 hiddenRootFlags = rootFlags & 0xFE;
    root->mFlags = hiddenRootFlags;
    // Preserve GHS's retained layout operands for the dynamically selected callback.
    gabi::cpu->r[5] = gabi::ea(layout);
    gabi::cpu->r[7] = rootFlags;
    gabi::cpu->r[8] = hiddenRootFlags;
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    const f32 position = layoutPositionY();
    auto* children = mChildren.mpData.get();
    mPositionY = position;
    auto* first = reinterpret_cast<hdBasicButton_c*>(children[0].get());
    first->mRequestedState = 1;
    startState(&sMiiverseSettingWaitState);
    return 1;
}
namespace miiverseBindings {
static u32 sSettingStringVtable;
static u32 sSettingStateVtable;
} // namespace miiverseBindings
struct hdMiiverseSettingZeroWords_l {
    be<u32> mWords[4];
};
struct hdMiiverseSettingRegistration_l {
    be<u32> mNext;
    gptr<void> mpObject;
    gfn mDestroy;
};
WWHD_OFFSET(hdMiiverseSettingRegistration_l, mpObject, 4);
WWHD_OFFSET(hdMiiverseSettingRegistration_l, mDestroy, 8);
WWHD_SIZE(hdMiiverseSettingRegistration_l, 12);
static hdMiiverseSettingZeroWords_l sSettingZeroWords;
static hdMiiverseSettingRegistration_l sSettingZeroRegistration;
static hdMiiverseSettingRegistration_l sSettingByteRegistrationA;
static hdMiiverseSettingRegistration_l sSettingByteRegistrationB;
static u8 sSettingRuntimeByteA;
static u8 sSettingRuntimeByteB;
static f32 sSettingAngleMinimum;
static f32 sSettingAngleMaximum;
static f32 sSettingOriginalAngleMinimum;
static f32 sSettingOriginalAngleMaximum;
static hdUiSafeString sSettingSecondaryArchiveName;
static u32 sSettingStateCounterReady;
static u32 sSettingStateCounter;
static hdPartsStateId_c sSettingCheckInputState;
static hdPartsStateId_c sSettingSlideDownState;
void miiverseSettingRegisterGlobal(void*);
void miiverseSettingInitializeRuntimeByteA(void*);
void miiverseSettingInitializeRuntimeByteB(void*);
void miiverseSettingWaitEnter(hdParts_c*);
void miiverseSettingWaitExecute(hdParts_c*);
void miiverseSettingWaitExit(hdParts_c*);
void miiverseSettingWaitUnused(hdParts_c*);
void miiverseSettingInputEnter(hdParts_c*);
void miiverseSettingInputExecute(hdParts_c*);
void miiverseSettingInputExit(hdParts_c*);
void miiverseSettingInputUnused(hdParts_c*);
void miiverseSettingInEnter(hdParts_c*);
void miiverseSettingInExecute(hdParts_c*);
void miiverseSettingInExit(hdParts_c*);
void miiverseSettingInUnused(hdParts_c*);
void miiverseSettingOutEnter(hdParts_c*);
void miiverseSettingOutExecute(hdParts_c*);
void miiverseSettingOutExit(hdParts_c*);
void miiverseSettingOutUnused(hdParts_c*);
void miiverseSettingSlideDownEnter(hdParts_c*);
void miiverseSettingSlideDownExecute(hdParts_c*);
void miiverseSettingSlideDownExit(hdParts_c*);
void miiverseSettingSlideDownUnused(hdParts_c*);
static void miiverseSettingSetDelegate(hdPartsPtmf_c* delegate, const void* callback) {
    delegate->mThisDelta = 0;
    delegate->mVirtualIndex = -1;
    delegate->mFunction = gabi::guest_fn(callback);
}
static void miiverseSettingInitializeState(hdPartsStateId_c* state, u32 number, const char* name,
                                           const void* enter, const void* execute, const void* exit,
                                           const void* unused) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mVtable.v = gabi::ea(&miiverseBindings::sSettingStateVtable);
    miiverseSettingSetDelegate(&state->mEnter, enter);
    miiverseSettingSetDelegate(&state->mExecute, execute);
    miiverseSettingSetDelegate(&state->mExit, exit);
    miiverseSettingSetDelegate(&state->mUnused, unused);
}
/* 026ABD2C */
static void miiverseSettingInitializeStatics() {
    sSettingZeroWords.mWords[2] = 0;
    sSettingZeroWords.mWords[0] = 0;
    sSettingZeroWords.mWords[3] = 0;
    sSettingZeroWords.mWords[1] = 0;
    miiverseSettingRegisterGlobal(&sSettingZeroRegistration);
    sSettingAngleMinimum = sSettingOriginalAngleMinimum;
    sSettingAngleMaximum = sSettingOriginalAngleMaximum;
    miiverseSettingInitializeRuntimeByteA(&sSettingRuntimeByteA);
    miiverseSettingRegisterGlobal(&sSettingByteRegistrationA);
    miiverseSettingInitializeRuntimeByteB(&sSettingRuntimeByteB);
    miiverseSettingRegisterGlobal(&sSettingByteRegistrationB);
    sMiiverseSettingPartName.mVtable.v = gabi::ea(&miiverseBindings::sSettingStringVtable);
    const bool counterReady = sSettingStateCounterReady != 0;
    sSettingSecondaryArchiveName.mVtable.v = gabi::ea(&miiverseBindings::sSettingStringVtable);
    sMiiverseSettingPartName.mText.v = gabi::guest_addr("L_BtnSequence_00");
    sSettingMenuArchiveNames[0].mVtable.v = gabi::ea(&miiverseBindings::sSettingStringVtable);
    sSettingMenuLayoutName.mVtable.v = gabi::ea(&miiverseBindings::sSettingStringVtable);
    sSettingMenuArchiveNames[0].mText.v = gabi::guest_addr("MenuMiiverseSetting_00");
    sSettingMenuArchiveNames[1].mVtable.v = gabi::ea(&miiverseBindings::sSettingStringVtable);
    sSettingMenuArchiveNames[1].mText.v = gabi::guest_addr("BtnMiiverseSetComment_00");
    sSettingSecondaryArchiveName.mText.v = gabi::guest_addr("BtnMiiverseSetComment_00");
    sSettingMenuLayoutName.mText.v = gabi::guest_addr("MenuMiiverseSetting_00");
    u32 number = 0;
    if (counterReady)
        number = sSettingStateCounter;
    else
        sSettingStateCounterReady = 1;
    miiverseSettingInitializeState(
        &sMiiverseSettingWaitState, ++number, "::StateID_Wait",
        (const void*)miiverseSettingWaitEnter, (const void*)miiverseSettingWaitExecute,
        (const void*)miiverseSettingWaitExit, (const void*)miiverseSettingWaitUnused);
    miiverseSettingInitializeState(
        &sSettingCheckInputState, ++number, "::StateID_CheckInput",
        (const void*)miiverseSettingInputEnter, (const void*)miiverseSettingInputExecute,
        (const void*)miiverseSettingInputExit, (const void*)miiverseSettingInputUnused);
    miiverseSettingInitializeState(
        &sMiiverseSettingInState, ++number, "::StateID_In", (const void*)miiverseSettingInEnter,
        (const void*)miiverseSettingInExecute, (const void*)miiverseSettingInExit,
        (const void*)miiverseSettingInUnused);
    miiverseSettingInitializeState(
        &sMiiverseSettingOutState, ++number, "::StateID_Out", (const void*)miiverseSettingOutEnter,
        (const void*)miiverseSettingOutExecute, (const void*)miiverseSettingOutExit,
        (const void*)miiverseSettingOutUnused);
    miiverseSettingInitializeState(
        &sSettingSlideDownState, ++number, "::StateID_SlideDown",
        (const void*)miiverseSettingSlideDownEnter, (const void*)miiverseSettingSlideDownExecute,
        (const void*)miiverseSettingSlideDownExit, (const void*)miiverseSettingSlideDownUnused);
    sSettingStateCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sSettingStringVtable 100FC614 nocheck
 data sSettingStateVtable 100FC674 nocheck
data sSettingZeroWords 10497AD0
data sSettingZeroRegistration 101F64A4
data sSettingByteRegistrationA 101F64B0
data sSettingByteRegistrationB 101F64BC
data sSettingRuntimeByteA 10497ACC
data sSettingRuntimeByteB 10497ACD
data sSettingAngleMinimum 10497AC4
data sSettingAngleMaximum 10497AC8
data sSettingOriginalAngleMinimum 100FC6E4 nocheck
data sSettingOriginalAngleMaximum 100FC6E8 nocheck
data sSettingSecondaryArchiveName 10497ABC
data sSettingStateCounterReady 101FD8E4
data sSettingStateCounter 101FDD50
data sSettingCheckInputState 10497B28
data sSettingSlideDownState 10497BAC
ext 028F026C void miiverseSettingRegisterGlobal(void*)
ext 028ED6F8 void miiverseSettingInitializeRuntimeByteA(void*)
ext 028EAB2C void miiverseSettingInitializeRuntimeByteB(void*)
ext 026AC0B4 void miiverseSettingWaitEnter(hdParts_c*) callback
ext 026AC0B8 void miiverseSettingWaitExecute(hdParts_c*) callback
ext 026AC0BC void miiverseSettingWaitExit(hdParts_c*) callback
ext 026AC0C0 void miiverseSettingWaitUnused(hdParts_c*) callback
ext 026AC0C8 void miiverseSettingInputEnter(hdParts_c*) callback
ext 026AB340 void miiverseSettingInputExecute(hdParts_c*) callback
ext 026AB394 void miiverseSettingInputExit(hdParts_c*) callback
ext 026AC0CC void miiverseSettingInputUnused(hdParts_c*) callback
ext 026AB3CC void miiverseSettingInEnter(hdParts_c*) callback
ext 026AB430 void miiverseSettingInExecute(hdParts_c*) callback
ext 026AB47C void miiverseSettingInExit(hdParts_c*) callback
ext 026AC0D4 void miiverseSettingInUnused(hdParts_c*) callback
ext 026AB498 void miiverseSettingOutEnter(hdParts_c*) callback
ext 026AB55C void miiverseSettingOutExecute(hdParts_c*) callback
ext 026AB5A8 void miiverseSettingOutExit(hdParts_c*) callback
ext 026AC0DC void miiverseSettingOutUnused(hdParts_c*) callback
ext 026AB638 void miiverseSettingSlideDownEnter(hdParts_c*) callback
ext 026AB6A0 void miiverseSettingSlideDownExecute(hdParts_c*) callback
ext 026AC0E4 void miiverseSettingSlideDownExit(hdParts_c*) callback
ext 026AC0E8 void miiverseSettingSlideDownUnused(hdParts_c*) callback
str 100FC710 "L_BtnSequence_00" in 026ABD2C
str 100FC724 "BtnMiiverseSetComment_00" in 026ABD2C
str 100FC740 "MenuMiiverseSetting_00" in 026ABD2C
str 100FC758 "::StateID_Wait" in 026ABD2C
str 100FC768 "::StateID_CheckInput" in 026ABD2C
str 100FC780 "::StateID_In" in 026ABD2C
str 100FC700 "::StateID_Out" in 026ABD2C
str 100FC6EC "::StateID_SlideDown" in 026ABD2C

data sSettingMenuArchiveNames 10497AE0 nocheck
data sSettingMenuLayoutName 10497AF4
ext 026F90D8 void miiverseSettingMenuAttachArchives(hdParts_c*, hdUiSafeString*, u32)
data sMiiverseSettingWaitState 10497AFC
ext 026F91F8 void miiverseSettingMenuBaseDetachArchives(hdParts_c*)
data sMiiverseSettingPartName 10497AB4
ext 026F92C0 u32 miiverseSettingCreateDefaultPart(hdParts_c*, u32, hdMiiversePartRequest_l*)
ext 026F5C08 void miiverseSettingSetNavigation(hdButtonPart_c*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*)
data sMiiverseSettingOutState 10497B80
data sMiiverseSettingInState 10497B54
data sMiiverseSettingLowerOffset 100FC6E0 nocheck
ext 02006364 void miiverseSettingUpdateState(hdStateMachine*)
ext 026F5C00 void miiverseSettingConnectReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*)
*/
