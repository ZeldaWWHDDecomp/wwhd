/* HD boat-race failure character: companion to the merged wait animation. */
#include "d/hd_screen_boat_race_failed.h"
#include "d/hd_parts_window.h"
WWHD_SIZE(hdWindowConfig_l, 0x6C);
WWHD_OFFSET(hdWindowConfig_l, mDuration, 0x4C);
WWHD_OFFSET(hdWindowConfig_l, mOrder, 0x54);
WWHD_OFFSET(hdWindowConfig_l, mMode, 0x58);
static u32 failedCharacterSScreenVtable;
static u32 failedCharacterSDelegateVtable;
static u32 failedCharacterSSecondaryVtable;
struct hdBoatRaceCharacterState {
    be<u32> mNumber;
    gptr<const char> mName;
    gptr<void> mVtable;
    hdPartsPtmf_c mEnter, mExecute, mExit, mCheck;
};
WWHD_SIZE(hdBoatRaceCharacterState, 0x2C);
static hdBoatRaceCharacterState failedCharacterSInState;
static hdBoatRaceCharacterState failedCharacterSWaitState;
static hdBoatRaceCharacterState failedCharacterSExitWaitState;
static hdBoatRaceCharacterState failedCharacterSFinishedState;
static u32 failedCharacterSStateVtable;
static hdUiSafeString failedCharacterSTextPaneName;
static hdUiStringTable failedCharacterSStringVtable;
static hdBoatRaceWords failedCharacterSWords;
static hdBoatRaceRange failedCharacterSRange;
static f32 failedCharacterSLower;
static f32 failedCharacterSUpper;
static f32 failedCharacterSSceneInitialFrame;
static u8 failedCharacterSFirstObject;
static u8 failedCharacterSSecondObject;
static u32 failedCharacterSStateGuard;
static u32 failedCharacterSNextState;
static hdBoatRaceRegistration failedCharacterSFirstRegistration;
static hdBoatRaceRegistration failedCharacterSSecondRegistration;
static hdBoatRaceRegistration failedCharacterSThirdRegistration;
static hdUiSafeString failedCharacterSLayoutName;
struct sUiPointer { gptr<hdScreenUiSystem> mInstance; };
static sUiPointer failedCharacterSUi;
void failedCharacterBoatRaceBaseConstruct(hdParts_c*);
void failedCharacterBoatRaceBaseDestroy(hdParts_c*, u32);
void* failedCharacterBoatRaceAllocate(u32);
void failedCharacterBoatRaceDelete(void*);
void failedCharacterBoatRacePlayNamed(hdWindowPart_c*, hdUiSafeString*);
u32 failedCharacterBoatRaceNamedIsPlaying(hdWindowPart_c*, hdUiSafeString*);
void failedCharacterBoatRaceAnimationSettingsConstruct(hdWindowConfig_l*);
void failedCharacterBoatRaceAnimationSettingsDestroy(hdWindowConfig_l*, u32);
void failedCharacterBoatRaceConfigureAnimation(hdWindowPart_c*, hdUiSafeString*, hdWindowConfig_l*);
hdWindowPart_c* failedCharacterBoatRaceAnimationSceneConstruct(hdWindowPart_c*, f32);
void failedCharacterBoatRaceAnimationSceneSetup(hdWindowPart_c*, hdPartsDesc_c*, void*, void*, void*);
void failedCharacterBoatRaceAnimationSceneAdd(hdWindowPart_c*, hdUiSafeString*, hdScreenHeap*);
void failedCharacterBoatRaceStopNamed(hdWindowPart_c*, hdUiSafeString*);
void failedCharacterBoatRaceAnimationSceneExecute(hdWindowPart_c*);
static hdWindowPart_c* scene(hdBoatRaceFailedCharView* character) {
    return reinterpret_cast<hdWindowPart_c*>(character->mScene.get());
}
/* 02626B58 */
u32 boatRaceFailedCharStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 02626C58 */
void hdBoatRaceFailedCharView::destroy(hdBoatRaceFailedCharView* character, u32 flags) {
    if (!character) return;
    character->parts()->mStateHolder.mHolderVtable = &failedCharacterSDelegateVtable;
    character->parts()->mSecondaryVtable = &failedCharacterSSecondaryVtable;
    failedCharacterBoatRaceBaseDestroy(character->parts(), 0);
    if (flags & 1) failedCharacterBoatRaceDelete(character);
}
/* 02626CC4 */
void hdBoatRaceFailedCharView::enterWait() {
    u32 timer = mDisplayWait;
    mFinished = 0;
    mWaitTimer = timer;
}
/* 02626D60 */
void hdBoatRaceFailedCharView::enterExitWait() { mWaitTimer = mExitWait; }
/* 02626D6C */
void hdBoatRaceFailedCharView::executeExitWait() {
    u32 timer = mWaitTimer;
    if (!timer) changeState(reinterpret_cast<hdPartsStateId_c*>(&failedCharacterSExitWaitState));
    else mWaitTimer = timer - 1;
}
/* 02626D94 */
void hdBoatRaceFailedCharView::enterOut() {
    hdUiSafeString names;
    names.mText.v = gabi::guest_addr("Out");
    names.mVtable.v = gabi::guest_addr("");
    failedCharacterBoatRacePlayNamed(scene(this), &names);
}
/* 02626DD4 */
void hdBoatRaceFailedCharView::executeOut() {
    hdUiSafeString names;
    names.mText.v = gabi::guest_addr("Out");
    names.mVtable.v = gabi::guest_addr("");
    if (failedCharacterBoatRaceNamedIsPlaying(scene(this), &names) != 1) changeState(reinterpret_cast<hdPartsStateId_c*>(&failedCharacterSFinishedState));
}
/* 02626E3C */
void hdBoatRaceFailedCharView::enterFinished() { mFinished = 1; }
/* 02626E48 */
void hdBoatRaceFailedCharView::enterIdle() { mAdvance = 0; }
/* 02626E54 */
void hdBoatRaceFailedCharView::executeIdle() { if (mAdvance) changeState(reinterpret_cast<hdPartsStateId_c*>(&failedCharacterSInState)); }
/* 02626E70 */
hdBoatRaceFailedCharView* hdBoatRaceFailedCharView::construct(hdBoatRaceFailedCharView* character) {
    if (!character) {
        character = static_cast<hdBoatRaceFailedCharView*>(failedCharacterBoatRaceAllocate(sizeof(hdBoatRaceFailedCharView)));
        if (!character) return NULL;
    }
    failedCharacterBoatRaceBaseConstruct(character->parts());
    character->setOwnLayout(NULL);
    character->mScene = NULL;
    character->mDisplayWait = 60;
    character->mUnusedFrames = 5;
    character->mWaitTimer = 0;
    character->parts()->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&failedCharacterSScreenVtable);
    character->parts()->mStateHolder.mHolderVtable = &failedCharacterSDelegateVtable;
    character->mExitWait = 100;
    character->mOutFrames = 15;
    character->mFinished = 0;
    character->mAdvance = 0;
    character->mInFrames = 29;
    character->parts()->mSecondaryVtable = &failedCharacterSSecondaryVtable;
    return character;
}
/* 02626F1C */
void hdBoatRaceFailedCharView::configureIn() {
    hdWindowConfig_l settings;
    failedCharacterBoatRaceAnimationSettingsConstruct(&settings);
    u32 frames = mInFrames;
    settings.mResetLayout = 0;
    settings.mMode = 2;
    hdUiSafeString names;
    names.mVtable.v = gabi::guest_addr("");
    settings.mDuration = frames;
    settings.mOrder = 0;
    names.mText.v = gabi::guest_addr("In");
    settings.mFinishEnabled = 0;
    failedCharacterBoatRaceConfigureAnimation(scene(this), &names, &settings);
    failedCharacterBoatRaceAnimationSettingsDestroy(&settings, 2);
}
/* 02626FA0 */
void hdBoatRaceFailedCharView::configureOut() {
    hdWindowConfig_l settings;
    failedCharacterBoatRaceAnimationSettingsConstruct(&settings);
    u32 frames = mOutFrames;
    settings.mOrder = 0;
    settings.mDuration = frames;
    settings.mMode = 0;
    hdUiSafeString names;
    names.mText.v = gabi::guest_addr("Out");
    settings.mResetLayout = 0;
    settings.mFinishEnabled = 0;
    names.mVtable.v = gabi::guest_addr("");
    failedCharacterBoatRaceConfigureAnimation(scene(this), &names, &settings);
    failedCharacterBoatRaceAnimationSettingsDestroy(&settings, 2);
}
/* 02627020 */
u32 hdBoatRaceFailedCharView::setup(void* archive, void* first, void* second, hdPartsDesc_c* descriptor) {
    parts()->mpResAccessor = static_cast<hdPartsResAccessor_c*>(archive);
    hdScreenHeap* heap = failedCharacterSUi.mInstance->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(0x110, heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    setOwnLayout(layout);
    layout->vLoad(&failedCharacterSLayoutName, gabi::ea(parts()->mpResAccessor.get()), gabi::ea(first), gabi::ea(second), 0, 0, descriptor);
    hdWindowPart_c* animation = static_cast<hdWindowPart_c*>(hdParts_new(0xDE8, heap, 4));
    if (animation) animation = failedCharacterBoatRaceAnimationSceneConstruct(animation, failedCharacterSSceneInitialFrame);
    layout = ownLayout();
    mScene = reinterpret_cast<MinigameAnimationScene*>(animation);
    failedCharacterBoatRaceAnimationSceneSetup(animation, descriptor, layout->mAccessor, first, second);
    hdUiSafeString names;
    names.mVtable.v = gabi::guest_addr("");
    names.mText.v = gabi::guest_addr("In");
    failedCharacterBoatRaceAnimationSceneAdd(scene(this), &names, heap);
    names.mVtable.v = gabi::guest_addr("");
    names.mText.v = gabi::guest_addr("Out");
    failedCharacterBoatRaceAnimationSceneAdd(scene(this), &names, heap);
    configureIn();
    configureOut();
    names.mText.v = gabi::guest_addr("In");
    names.mVtable.v = gabi::guest_addr("");
    failedCharacterBoatRaceStopNamed(scene(this), &names);
    startState(reinterpret_cast<hdPartsStateId_c*>(&failedCharacterSFinishedState));
    return 1;
}
/* 02627194 */
void hdBoatRaceFailedCharView::release() {
    hdWindowPart_c* animation = scene(this);
    if (animation) {
        gabi::call_ptr(reinterpret_cast<gfn*>(animation->mVtable.get())[5].get(), animation, 3);
        mScene = NULL;
    }
    hdPartsLayout_c* layout = ownLayout();
    if (layout) {
        layout->release();
        layout = ownLayout();
        if (layout) layout->destroy(3);
        setOwnLayout(NULL);
    }
}
/* 0262722C */
void hdBoatRaceFailedCharView::execute() {
    failedCharacterBoatRaceAnimationSceneExecute(scene(this));
    hdParts_executeStateMachine(stateMachine());
}
/* 02627264 */
void hdBoatRaceFailedCharView::reset() {
    hdUiSafeString names;
    names.mText.v = gabi::guest_addr("In");
    names.mVtable.v = gabi::guest_addr("");
    failedCharacterBoatRaceStopNamed(scene(this), &names);
    changeState(reinterpret_cast<hdPartsStateId_c*>(&failedCharacterSFinishedState));
}
void failedCharacterBoatRaceRegister(hdBoatRaceRegistration*);
void failedCharacterBoatRaceConstructFirst(be<u8>*);
void failedCharacterBoatRaceConstructSecond(be<u8>*);
static void setDirect(hdPartsPtmf_c* method, u32 function) {
    method->mThisDelta = 0;
    method->mVirtualIndex = -1;
    method->mFunction = function;
}
static void initializeState(hdBoatRaceCharacterState* state, u32 number, const char* name, u32 enter, u32 execute, u32 exit, u32 check) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mVtable = &failedCharacterSStateVtable;
    setDirect(&state->mEnter, enter);
    setDirect(&state->mExecute, execute);
    setDirect(&state->mExit, exit);
    setDirect(&state->mCheck, check);
}
/* 026272C0 */
void boatRaceFailedCharStaticInit() {
    failedCharacterSWords.mWords[2] = 0;
    failedCharacterSWords.mWords[0] = 0;
    failedCharacterSWords.mWords[3] = 0;
    failedCharacterSWords.mWords[1] = 0;
    failedCharacterBoatRaceRegister(&failedCharacterSFirstRegistration);
    f32 lower = failedCharacterSLower, upper = failedCharacterSUpper;
    failedCharacterSRange.mLower = lower;
    failedCharacterSRange.mUpper = upper;
    failedCharacterBoatRaceConstructFirst(&failedCharacterSFirstObject);
    failedCharacterBoatRaceRegister(&failedCharacterSSecondRegistration);
    failedCharacterBoatRaceConstructSecond(&failedCharacterSSecondObject);
    failedCharacterBoatRaceRegister(&failedCharacterSThirdRegistration);
    failedCharacterSLayoutName.mVtable = &failedCharacterSStringVtable;
    failedCharacterSLayoutName.mText.v = gabi::guest_addr("BoatRaceFailedChar_00");
    u32 guard = failedCharacterSStateGuard;
    failedCharacterSTextPaneName.mText.v = gabi::guest_addr("T_Text_00");
    failedCharacterSTextPaneName.mVtable = &failedCharacterSStringVtable;
    u32 id;
    if (guard) id = failedCharacterSNextState;
    else { failedCharacterSStateGuard = 1; id = 0; }
    initializeState(&failedCharacterSInState, id + 1, "::StateID_In", 0x02626CC4, 0x02626CD8, 0x0262759C, 0x026275A0);
    initializeState(&failedCharacterSWaitState, id + 2, "::StateID_Wait", 0x02626D60, 0x02626D6C, 0x026275A8, 0x026275AC);
    initializeState(&failedCharacterSExitWaitState, id + 3, "::StateID_Out", 0x02626D94, 0x02626DD4, 0x02626E3C, 0x026275B4);
    failedCharacterSNextState = id + 4;
    initializeState(&failedCharacterSFinishedState, id + 4, "::StateID_Sleep", 0x02626E48, 0x02626E54, 0x026275BC, 0x026275C0);
}
/* WWHD glue
data failedCharacterSWaitState 1048F0F0
data failedCharacterSStateVtable 100E56BC
data failedCharacterSTextPaneName 1048F098
data failedCharacterSStringVtable 100E565C
data failedCharacterSWords 1048F0AC
data failedCharacterSRange 1048F0A0
data failedCharacterSLower 100E5730
data failedCharacterSUpper 100E5734
data failedCharacterSSceneInitialFrame 100E5720
data failedCharacterSFirstObject 1048F0A8
data failedCharacterSSecondObject 1048F0A9
data failedCharacterSStateGuard 101FD8E4
data failedCharacterSNextState 101FDD50
data failedCharacterSFirstRegistration 101F535C
data failedCharacterSSecondRegistration 101F5368
data failedCharacterSThirdRegistration 101F5374
str 100E5748 "BoatRaceFailedChar_00" in 026272C0
str 100E5760 "T_Text_00" in 026272C0
str 100E577C "::StateID_In" in 026272C0
str 100E578C "::StateID_Wait" in 026272C0
str 100E576C "::StateID_Out" in 026272C0
str 100E5738 "::StateID_Sleep" in 026272C0
ext 028F026C void failedCharacterBoatRaceRegister(hdBoatRaceRegistration*)
ext 028ED6F8 void failedCharacterBoatRaceConstructFirst(be<u8>*)
ext 028EAB2C void failedCharacterBoatRaceConstructSecond(be<u8>*)
data failedCharacterSScreenVtable 100E57A0
data failedCharacterSDelegateVtable 100E5868
data failedCharacterSSecondaryVtable 100E5878
data failedCharacterSInState 1048F0C4
data failedCharacterSExitWaitState 1048F11C
data failedCharacterSFinishedState 1048F148
data failedCharacterSLayoutName 1048F0BC
data failedCharacterSUi 1018C404
str 100E565C ""
str 100E5710 "Out" in 02626D94
str 100E5714 "Out" in 02626DD4
str 100E5718 "In" in 02626F1C
str 100E571C "Out" in 02626FA0
str 100E5728 "In" in 02627020
str 100E5724 "Out" in 02627020
str 100E572B "In" in 02627264
ext 026F89E8 void failedCharacterBoatRaceBaseConstruct(hdParts_c*)
ext 026F88F0 void failedCharacterBoatRaceBaseDestroy(hdParts_c*,u32)
ext 0273AD10 void* failedCharacterBoatRaceAllocate(u32)
ext 0273AF40 void failedCharacterBoatRaceDelete(void*)
ext 026F5258 void failedCharacterBoatRacePlayNamed(hdWindowPart_c*,hdUiSafeString*)
ext 026F5534 u32 failedCharacterBoatRaceNamedIsPlaying(hdWindowPart_c*,hdUiSafeString*)
ext 026F43DC void failedCharacterBoatRaceAnimationSettingsConstruct(hdWindowConfig_l*)
ext 026F30D4 void failedCharacterBoatRaceAnimationSettingsDestroy(hdWindowConfig_l*,u32)
ext 026F55DC void failedCharacterBoatRaceConfigureAnimation(hdWindowPart_c*,hdUiSafeString*,hdWindowConfig_l*)
ext 026F31CC hdWindowPart_c* failedCharacterBoatRaceAnimationSceneConstruct(hdWindowPart_c*,f32)
ext 026F4174 void failedCharacterBoatRaceAnimationSceneSetup(hdWindowPart_c*,hdPartsDesc_c*,void*,void*,void*)
ext 026F459C void failedCharacterBoatRaceAnimationSceneAdd(hdWindowPart_c*,hdUiSafeString*,hdScreenHeap*)
ext 026F5344 void failedCharacterBoatRaceStopNamed(hdWindowPart_c*,hdUiSafeString*)
ext 026F4FD8 void failedCharacterBoatRaceAnimationSceneExecute(hdWindowPart_c*)
*/
