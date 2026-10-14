/** HD new-comment menu state and navigation. Private binary-derived source. */
#include "d/hd_screen_btn_miiverse_new_comment.h"
#include "d/hd_screen_menu_miiverse_new_comment.h"
#include "SSystem/SComponent/c_xyz.h"
static hdScreenUiSystem* sNewCommentUiSystem;
void miiverseNewCommentUpdateState(hdStateMachine*);
void miiverseNewCommentConnectReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*);
static hdPartsStateId_c sNewCommentInOutState;
static hdPartsStateId_c sNewCommentWaitState;
void miiverseNewCommentMenuBaseDetachArchives(hdParts_c*);
/* 026AA6A0 */
void hdMenuMiiverseNewComment_c::updateState() {
    miiverseNewCommentUpdateState(stateMachine());
}
/* 026AA7C4 */
f32 hdMenuMiiverseNewComment_c::sequenceHeight() const {
    return mpSequencePane->mSizeY;
}
/* 026AA6A8 */
void hdMenuMiiverseNewComment_c::connectUpperControl(hdButtonPart_c* control) {
    if (control) {
        auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseNewCommentConnectReceiver(control, 3, content->mpReceiver);
    }
}
/* 026AA6D0 */
void hdMenuMiiverseNewComment_c::connectLowerControl(hdButtonPart_c* control) {
    if (control) {
        auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
        miiverseNewCommentConnectReceiver(control, 0, content->mpReceiver);
    }
}
/* 026AA6F8 */
void hdMenuMiiverseNewComment_c::connectContentAbove(hdButtonPart_c* control) {
    hdButtonReceiver_l* receiver = control ? control->mpReceiver.get() : nullptr;
    auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    miiverseNewCommentConnectReceiver(content, 0, receiver);
}
/* 026AA720 */
void hdMenuMiiverseNewComment_c::connectContentBelow(hdButtonPart_c* control) {
    hdButtonReceiver_l* receiver = control ? control->mpReceiver.get() : nullptr;
    auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    miiverseNewCommentConnectReceiver(content, 3, receiver);
}
/* 026AA810 */
void hdMenuMiiverseNewComment_c::beginInOut() {
    mVisible = 0;
    changeState(&sNewCommentInOutState);
}
/* 026AA91C */
void hdMenuMiiverseNewComment_c::finishInOut() {
    auto* layout = mpLayout.get();
    mVisible = 0;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    pane->mFlags = u8(pane->mFlags) & 0xFE;
    changeState(&sNewCommentWaitState);
}
/* 026AA94C */
void hdMenuMiiverseNewComment_c::finishAppear() {
    auto* layout = mpLayout.get();
    mVisible = 0;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    pane->mFlags = u8(pane->mFlags) & 0xFE;
    changeState(&sNewCommentWaitState);
}
/* 026AA64C */
void hdMenuMiiverseNewComment_c::finalizeParts() {
    gabi::call_ptr<void>(mVtable->mDeleteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    miiverseNewCommentMenuBaseDetachArchives(this);
}
/* 026AA7D0 */
void hdMenuMiiverseNewComment_c::showInOut() {
    auto* sequencePane = mpSequencePane.get();
    mVisible = 1;
    sequencePane->mAlpha = 0;
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    pane->mFlags = (u8(pane->mFlags) & 0xFE) | 1;
    changeState(&sNewCommentInOutState);
}
void miiverseNewCommentSetNavigation(hdButtonPart_c*, hdButtonReceiver_l*, hdButtonReceiver_l*,
                                     hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*,
                                     hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*);
/* 026AA748 */
void hdMenuMiiverseNewComment_c::clearContentNavigation() {
    miiverseNewCommentSetNavigation(reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get()),
                                    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                    nullptr);
}
/* 026AA97C */
void hdMenuMiiverseNewComment_c::translateLayoutY(f32 delta) {
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
/* 026AA9D0 */
void hdMenuMiiverseNewComment_c::setSequencePresentation(u32 alpha, f32 position) {
    mpSequencePane->mAlpha = u8(alpha);
    auto* layout = mpLayout.get();
    auto* source = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get());
    const f32 x = miiversePaneTranslationX(source);
    const f32 oldPosition = mPositionY;
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
    mPositionY = position;
}
void miiverseNewCommentGlobalTranslation(cXyz*, hdPartsPaneSRT_c*);
/* 026AA798 */
f32 hdMenuMiiverseNewComment_c::sequenceGlobalY() const {
    cXyz translation;
    miiverseNewCommentGlobalTranslation(&translation, mpSequencePane);
    return translation.y;
}
namespace miiverseBindings {
static u32 sSequenceSafeStringVtable;
}
static hdUiSafeString sNewCommentPartName;
static hdUiSafeString sNewCommentButtonClassName;
u32 miiverseNewCommentCreateDefaultPart(hdParts_c*, u32, hdMiiversePartRequest_l*);
// String scans use console-width address arithmetic, including a boundary crossing.
static u8 miiverseNewCommentCharacterAt(const hdPartsChar_t* text, u32 index) {
    return *gabi::at<hdPartsChar_t>(u32(gabi::ea(text) + index));
}
static bool miiverseNewCommentNamesMatch(hdUiSafeString* name, hdUiSafeString* reference) {
    name->assureTermination();
    name->assureTermination();
    const u32 cachedText = gabi::ea(name->mText.get());
    reference->assureTermination();
    if (cachedText == gabi::ea(reference->mText.get()))
        return true;
    auto* left = name->mText.get();
    auto* right = reference->mText.get();
    for (u32 index = 0; index < 0x40001; ++index) {
        const u8 character = miiverseNewCommentCharacterAt(left, index);
        if (character != miiverseNewCommentCharacterAt(right, index))
            return false;
        if (!character)
            return true;
    }
    return false;
}
/* 026AAA38 */
u32 hdMenuMiiverseNewComment_c::createSequencePart(hdMiiversePartRequest_l* request) {
    hdScreenHeap* heap = sNewCommentUiSystem->mHeap;
    hdUiSafeString className;
    className.mText.v = request->mClassName.v;
    className.mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    if (!miiverseNewCommentNamesMatch(&request->mPartName, &sNewCommentPartName))
        return 1;
    if (!miiverseNewCommentNamesMatch(&className, &sNewCommentButtonClassName))
        return miiverseNewCommentCreateDefaultPart(this, 0, request);
    auto* destination = mChildren.mpData.get();
    auto* button = static_cast<hdButtonPart_c*>(hdParts_new(0x8C, heap, 4));
    if (button)
        button = reinterpret_cast<hdButtonPart_c*>(hdBtnMiiverseNewComment_c::construct(
            reinterpret_cast<hdBtnMiiverseNewComment_c*>(button)));
    *destination = button;
    auto* content = reinterpret_cast<hdButtonPart_c*>((*mChildren.mpData).get());
    if (!content)
        return 0;
    return reinterpret_cast<hdBtnMiiverseNewComment_c*>(content)->init(
        nullptr, 0, reinterpret_cast<hdPartsDesc_c*>(request));
}
static hdUiSafeString sNewCommentAnimationGroups[1] = {{0, 0}};
static hdUiSafeString sNewCommentAnimationNames[1] = {{0, 0}};
static u32 sNewCommentAnimationIndex;
void miiverseNewCommentBindAnimationGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
/* 026AA54C */
u32 hdMenuMiiverseNewComment_c::loadLayout(void* name, u32 resource, u32 options, u32 heapOptions,
                                           void* descriptor) {
    hdPartsLayout_c* layout = reinterpret_cast<hdPartsLayout_c*>(
        hdParts_new(sizeof(hdPartsLayout_c), sNewCommentUiSystem->mHeap, 4));
    if (layout)
        layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, resource, options, heapOptions, 1, 1, descriptor))
        return 0;
    miiverseNewCommentBindAnimationGroup(
        mpLayout->animSet(), 0, &sNewCommentAnimationGroups[0],
        &sNewCommentAnimationNames[u32(sNewCommentAnimationIndex)]);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}
static hdPartsStateId_c sNewCommentAppearState;
static hdPartsStateId_c sNewCommentCheckInputState;
/* 026AA828 */
void hdMenuMiiverseNewComment_c::beginAppear() {
    mVisible = 1;
    auto* current = hdParts_getCurrentStateId(stateMachine());
    const auto getKey = current->mVtable->mGetKey.get();
    auto* appearVtable = sNewCommentAppearState.mVtable.get();
    const u32 currentKey = gabi::call_ptr<u32>(getKey, current);
    const u32 appearKey = gabi::call_ptr<u32>(appearVtable->mGetKey.get(), &sNewCommentAppearState);
    if (currentKey == appearKey)
        return;
    current = hdParts_getCurrentStateId(stateMachine());
    const auto secondGetKey = current->mVtable->mGetKey.get();
    auto* checkInputVtable = sNewCommentCheckInputState.mVtable.get();
    const u32 secondCurrentKey = gabi::call_ptr<u32>(secondGetKey, current);
    const u32 checkInputKey =
        gabi::call_ptr<u32>(checkInputVtable->mGetKey.get(), &sNewCommentCheckInputState);
    if (secondCurrentKey != checkInputKey)
        changeState(&sNewCommentAppearState);
}
static hdUiSafeString sNewCommentMenuArchiveNames[2] = {{0, 0}, {0, 0}};
static hdUiSafeString sNewCommentMenuLayoutName;
void miiverseNewCommentMenuAttachArchives(hdParts_c*, hdUiSafeString*, u32);
/* 026AA410 */
u32 hdMenuMiiverseNewComment_c::initializeParts(u32 options, u32 heapOptions, void* descriptor) {
    miiverseNewCommentMenuAttachArchives(this, sNewCommentMenuArchiveNames, 2);
    const auto load = mVtable->mLoad.get();
    auto* resource = mpResAccessor.get();
    if (!gabi::call_ptr<u32>(load, this, &sNewCommentMenuLayoutName, resource, options, heapOptions,
                             descriptor))
        return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this))
        return 0;
    findSequencePane();
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    const u8 rootFlags = root->mFlags;
    const u8 hiddenRootFlags = rootFlags & 0xFE;
    root->mFlags = hiddenRootFlags;
    auto* first = reinterpret_cast<hdBasicButton_c*>((*mChildren.mpData).get());
    first->mRequestedState = 1;
    // The state-owner virtual call also retains the preceding layout operands.
    gabi::cpu->r[6] = gabi::ea(root);
    gabi::cpu->r[7] = rootFlags;
    gabi::cpu->r[8] = hiddenRootFlags;
    startState(&sNewCommentWaitState);
    return 1;
}
namespace miiverseBindings {
static u32 sNewCommentStateVtable;
}
struct hdMiiverseNewCommentZeroWords_l {
    be<u32> mWords[4];
};
static hdMiiverseNewCommentZeroWords_l sNewCommentZeroWords;
// Registration records already contain object/destructor metadata in the original data image.
struct hdMiiverseNewCommentRegistration_l {
    be<u32> mNext;
    gptr<void> mpObject;
    gfn mDestroy;
};
WWHD_OFFSET(hdMiiverseNewCommentRegistration_l, mpObject, 4);
WWHD_OFFSET(hdMiiverseNewCommentRegistration_l, mDestroy, 8);
WWHD_SIZE(hdMiiverseNewCommentRegistration_l, 12);
static hdMiiverseNewCommentRegistration_l sNewCommentZeroRegistration;
static hdMiiverseNewCommentRegistration_l sNewCommentByteRegistrationA;
static hdMiiverseNewCommentRegistration_l sNewCommentByteRegistrationB;
static u8 sNewCommentRuntimeByteA;
static u8 sNewCommentRuntimeByteB;
static f32 sNewCommentAngleMinimum;
static f32 sNewCommentAngleMaximum;
static f32 sNewCommentOriginalAngleMinimum;
static f32 sNewCommentOriginalAngleMaximum;
static hdUiSafeString sNewCommentSecondaryArchiveName;
static u32 sNewCommentStateCounterReady;
static u32 sNewCommentStateCounter;
void miiverseNewCommentRegisterGlobal(void*);
void miiverseNewCommentInitializeRuntimeByteA(void*);
void miiverseNewCommentInitializeRuntimeByteB(void*);
void miiverseNewCommentWaitEnter(hdParts_c*);
void miiverseNewCommentWaitExecute(hdParts_c*);
void miiverseNewCommentWaitExit(hdParts_c*);
void miiverseNewCommentWaitUnused(hdParts_c*);
void miiverseNewCommentInputEnter(hdParts_c*);
void miiverseNewCommentInputExecute(hdParts_c*);
void miiverseNewCommentInputExit(hdParts_c*);
void miiverseNewCommentInputUnused(hdParts_c*);
void miiverseNewCommentInOutEnter(hdParts_c*);
void miiverseNewCommentInOutExecute(hdParts_c*);
void miiverseNewCommentInOutExit(hdParts_c*);
void miiverseNewCommentInOutUnused(hdParts_c*);
void miiverseNewCommentAppearEnter(hdParts_c*);
void miiverseNewCommentAppearExecute(hdParts_c*);
void miiverseNewCommentAppearExit(hdParts_c*);
void miiverseNewCommentAppearUnused(hdParts_c*);
static void miiverseNewCommentSetDelegate(hdPartsPtmf_c* delegate, const void* callback) {
    delegate->mThisDelta = 0;
    delegate->mVirtualIndex = -1;
    delegate->mFunction = gabi::guest_fn(callback);
}
static void miiverseNewCommentInitializeState(hdPartsStateId_c* state, u32 number, const char* name,
                                              const void* enter, const void* execute,
                                              const void* exit, const void* unused) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mVtable.v = gabi::ea(&miiverseBindings::sNewCommentStateVtable);
    miiverseNewCommentSetDelegate(&state->mEnter, enter);
    miiverseNewCommentSetDelegate(&state->mExecute, execute);
    miiverseNewCommentSetDelegate(&state->mExit, exit);
    miiverseNewCommentSetDelegate(&state->mUnused, unused);
}
/* 026AAC50 */
static void miiverseNewCommentInitializeStatics() {
    sNewCommentZeroWords.mWords[2] = 0;
    sNewCommentZeroWords.mWords[0] = 0;
    sNewCommentZeroWords.mWords[3] = 0;
    sNewCommentZeroWords.mWords[1] = 0;
    miiverseNewCommentRegisterGlobal(&sNewCommentZeroRegistration);
    sNewCommentAngleMinimum = sNewCommentOriginalAngleMinimum;
    sNewCommentAngleMaximum = sNewCommentOriginalAngleMaximum;
    miiverseNewCommentInitializeRuntimeByteA(&sNewCommentRuntimeByteA);
    miiverseNewCommentRegisterGlobal(&sNewCommentByteRegistrationA);
    miiverseNewCommentInitializeRuntimeByteB(&sNewCommentRuntimeByteB);
    miiverseNewCommentRegisterGlobal(&sNewCommentByteRegistrationB);
    sNewCommentPartName.mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    sNewCommentPartName.mText.v = gabi::guest_addr("L_BtnSequence_00");
    sNewCommentAnimationGroups[0].mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    sNewCommentAnimationNames[0].mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    sNewCommentAnimationNames[0].mText.v = gabi::guest_addr("G_All_00");
    sNewCommentAnimationGroups[0].mText.v = gabi::guest_addr("InAll");
    sNewCommentMenuArchiveNames[0].mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    sNewCommentMenuArchiveNames[0].mText.v = gabi::guest_addr("MenuMiiverseNewComment_00");
    sNewCommentMenuArchiveNames[1].mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    sNewCommentSecondaryArchiveName.mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    const bool counterReady = sNewCommentStateCounterReady != 0;
    sNewCommentSecondaryArchiveName.mText.v = gabi::guest_addr("BtnMiiverseNewComment_00");
    sNewCommentMenuLayoutName.mVtable.v = gabi::ea(&miiverseBindings::sSequenceSafeStringVtable);
    sNewCommentMenuLayoutName.mText.v = gabi::guest_addr("MenuMiiverseNewComment_00");
    sNewCommentMenuArchiveNames[1].mText.v = gabi::guest_addr("BtnMiiverseNewComment_00");
    u32 number = 0;
    if (counterReady)
        number = sNewCommentStateCounter;
    else
        sNewCommentStateCounterReady = 1;
    miiverseNewCommentInitializeState(
        &sNewCommentWaitState, ++number, "::StateID_Wait", (const void*)miiverseNewCommentWaitEnter,
        (const void*)miiverseNewCommentWaitExecute, (const void*)miiverseNewCommentWaitExit,
        (const void*)miiverseNewCommentWaitUnused);
    miiverseNewCommentInitializeState(
        &sNewCommentCheckInputState, ++number, "::StateID_CheckInput",
        (const void*)miiverseNewCommentInputEnter, (const void*)miiverseNewCommentInputExecute,
        (const void*)miiverseNewCommentInputExit, (const void*)miiverseNewCommentInputUnused);
    miiverseNewCommentInitializeState(
        &sNewCommentInOutState, ++number, "::StateID_InOut",
        (const void*)miiverseNewCommentInOutEnter, (const void*)miiverseNewCommentInOutExecute,
        (const void*)miiverseNewCommentInOutExit, (const void*)miiverseNewCommentInOutUnused);
    miiverseNewCommentInitializeState(
        &sNewCommentAppearState, ++number, "::StateID_Appear",
        (const void*)miiverseNewCommentAppearEnter, (const void*)miiverseNewCommentAppearExecute,
        (const void*)miiverseNewCommentAppearExit, (const void*)miiverseNewCommentAppearUnused);
    sNewCommentStateCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sSequenceSafeStringVtable 100FC3A8 nocheck
 data sNewCommentStateVtable 100FC408 nocheck
data sNewCommentZeroWords 104979D0
data sNewCommentZeroRegistration 101F6474
data sNewCommentByteRegistrationA 101F6480
data sNewCommentByteRegistrationB 101F648C
data sNewCommentRuntimeByteA 104979CC
data sNewCommentRuntimeByteB 104979CD
data sNewCommentAngleMinimum 104979C4
data sNewCommentAngleMaximum 104979C8
data sNewCommentOriginalAngleMinimum 100FC46C nocheck
data sNewCommentOriginalAngleMaximum 100FC470 nocheck
data sNewCommentSecondaryArchiveName 104979BC
data sNewCommentStateCounterReady 101FD8E4
data sNewCommentStateCounter 101FDD50
ext 028F026C void miiverseNewCommentRegisterGlobal(void*)
ext 028ED6F8 void miiverseNewCommentInitializeRuntimeByteA(void*)
ext 028EAB2C void miiverseNewCommentInitializeRuntimeByteB(void*)
ext 026AAF88 void miiverseNewCommentWaitEnter(hdParts_c*) callback
ext 026AAF8C void miiverseNewCommentWaitExecute(hdParts_c*) callback
ext 026AAF90 void miiverseNewCommentWaitExit(hdParts_c*) callback
ext 026AAF94 void miiverseNewCommentWaitUnused(hdParts_c*) callback
ext 026A9E28 void miiverseNewCommentInputEnter(hdParts_c*) callback
ext 026AA088 void miiverseNewCommentInputExecute(hdParts_c*) callback
ext 026AA104 void miiverseNewCommentInputExit(hdParts_c*) callback
ext 026AAF9C void miiverseNewCommentInputUnused(hdParts_c*) callback
ext 026AA110 void miiverseNewCommentInOutEnter(hdParts_c*) callback
ext 026AA15C void miiverseNewCommentInOutExecute(hdParts_c*) callback
ext 026AAFA4 void miiverseNewCommentInOutExit(hdParts_c*) callback
ext 026AAFA8 void miiverseNewCommentInOutUnused(hdParts_c*) callback
ext 026AA1A8 void miiverseNewCommentAppearEnter(hdParts_c*) callback
ext 026AA23C void miiverseNewCommentAppearExecute(hdParts_c*) callback
ext 026AAFB0 void miiverseNewCommentAppearExit(hdParts_c*) callback
ext 026AAFB4 void miiverseNewCommentAppearUnused(hdParts_c*) callback
str 100FC4B4 "L_BtnSequence_00" in 026AAC50
str 100FC4A8 "G_All_00" in 026AAC50
str 100FC474 "InAll" in 026AAC50
str 100FC48C "MenuMiiverseNewComment_00" in 026AAC50
str 100FC4C8 "BtnMiiverseNewComment_00" in 026AAC50
str 100FC4E4 "::StateID_Wait" in 026AAC50
str 100FC4F4 "::StateID_CheckInput" in 026AAC50
str 100FC47C "::StateID_InOut" in 026AAC50
str 100FC50C "::StateID_Appear" in 026AAC50

data sNewCommentMenuArchiveNames 104979E0 nocheck
data sNewCommentMenuLayoutName 104979F4
ext 026F90D8 void miiverseNewCommentMenuAttachArchives(hdParts_c*, hdUiSafeString*, u32)
data sNewCommentAppearState 10497A80
data sNewCommentCheckInputState 10497A28
data sNewCommentAnimationGroups 104979AC nocheck
data sNewCommentAnimationNames 104979A4 nocheck
ext 02004E04 void miiverseNewCommentBindAnimationGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
data sNewCommentAnimationIndex 100FC39C nocheck
ext 026F91F8 void miiverseNewCommentMenuBaseDetachArchives(hdParts_c*)
data sNewCommentUiSystem 1018C404
data sNewCommentPartName 104979B4
data sNewCommentButtonClassName 1049DA40
ext 026F92C0 u32 miiverseNewCommentCreateDefaultPart(hdParts_c*, u32, hdMiiversePartRequest_l*)
ext 02704D2C void miiverseNewCommentGlobalTranslation(cXyz*, hdPartsPaneSRT_c*)
ext 026F5C08 void miiverseNewCommentSetNavigation(hdButtonPart_c*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*, hdButtonReceiver_l*)
data sNewCommentInOutState 10497A54
data sNewCommentWaitState 104979FC
ext 02006364 void miiverseNewCommentUpdateState(hdStateMachine*)
ext 026F5C00 void miiverseNewCommentConnectReceiver(hdButtonPart_c*, u32, hdButtonReceiver_l*)
ext 026411F0 hdBtnMiiverseNewComment_c* hdBtnMiiverseNewComment_c::construct(hdBtnMiiverseNewComment_c*)
ext 0264125C u32 hdBtnMiiverseNewComment_c::init(void*, u32, hdPartsDesc_c*)
*/
