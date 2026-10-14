/* HD BatonSignForWindow_00: message-window sign animation and state requests. */
#include "d/hd_screen_baton_sign_for_window.h"
#include "d/hd_layout_mgr.h"
static hdPartsVtbl_c sBatonWindowScreenVtable;
static u8 sBatonWindowHolderVtable;
static u8 sBatonWindowSecondaryVtable;
static hdUiStringTable sBatonWindowStringVtable;
static hdPartsStateIdVtbl_c sBatonWindowStateVtable;
struct hdBatonWindowHeaderWords_c { be<u32> mWords[4]; };
struct hdBatonWindowRegistration_c { u8 mStorage[12]; };
struct hdBatonWindowGroups_c { hdUiSafeString mEntries[2]; };
struct hdBatonWindowAnimations_c { hdUiSafeString mEntries[3]; };
struct hdBatonWindowAnimationMap_c { be<u32> mGroups[3]; };
using hdBatonWindowUiSystemPointer = gptr<hdScreenUiSystem>;
static hdBatonWindowHeaderWords_c sBatonWindowHeaderWords;
static hdBatonWindowRegistration_c sBatonWindowHeaderRegistration;
static hdBatonWindowRegistration_c sBatonWindowRotationRegistration;
static hdBatonWindowRegistration_c sBatonWindowPositiveRotationRegistration;
static f32 sBatonWindowMinimumRotation;
static f32 sBatonWindowMaximumRotation;
static u8 sBatonWindowRotationGuard;
static u8 sBatonWindowPositiveRotationGuard;
static hdBatonWindowGroups_c sBatonWindowGroups;
static hdBatonWindowAnimations_c sBatonWindowAnimations;
static hdUiSafeString sBatonWindowScreenName;
static hdBatonWindowAnimationMap_c sBatonWindowAnimationMap;
static hdBatonWindowUiSystemPointer sBatonWindowUiSystem;
static hdPartsStateId_c sBatonWindowIn;
static hdPartsStateId_c sBatonWindowWait;
static hdPartsStateId_c sBatonWindowOut;
static hdPartsStateId_c sBatonWindowBaseBlank;
static u32 sBatonWindowStateCounterReady;
static u32 sBatonWindowStateCounter;
static hdStateId sBatonWindowNullState;
static hdBatonSignForWindow_c* allocateBatonWindow(u32 size);
static void deleteBatonWindow(void* object);
static void destroyBatonWindowBase(hdParts_c* self, s32 flags);
static void constructBatonWindowBase(hdParts_c* self);
static hdPartsLayout_c* allocateBatonWindowLayout(u32 size, hdScreenHeap* heap, u32 alignment);
static void bindBatonWindowAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group);
static void registerBatonWindowGlobal(hdBatonWindowRegistration_c* registration);
static void initializeBatonWindowRotation(be<u8>* guard);
static void initializePositiveBatonWindowRotation(be<u8>* guard);
static void playBatonWindowAnimation(hdBatonSignForWindow_c* self, u32 animation);
static u32 queryBatonWindowAnimation(hdBatonSignForWindow_c* self, u32 animation);
static void applyBatonWindowFrame(hdBatonSignForWindow_c* self, u32 animation, u32 frame);
static void runBatonWindowState(hdStateMachine* machine);

// External indices use the original 32-bit indexed read, including wrapped addresses.
static u32 batonWindowAnimationGroup(u32 animation) {
    return *gabi::at<be<u32>>(gabi::ea(&sBatonWindowAnimationMap) + animation * sizeof(be<u32>));
}
/* 02620FA4 */
static u32 batonWindowStateKey(hdPartsStateId_c* state) { return u32(s32(state->mNumber)); }
/* 026210A4 */
void hdBatonSignForWindow_c::destroy(hdBatonSignForWindow_c* self, s32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sBatonWindowHolderVtable;
    self->mSecondaryVtable = &sBatonWindowSecondaryVtable;
    destroyBatonWindowBase(self, 0);
    if (flags & 1) deleteBatonWindow(self);
}
/* 02621110 */
void hdBatonSignForWindow_c::startAnimation(u32 animation) {
    hdAnimSet_play(animSet(), animation, batonWindowAnimationGroup(animation), 1.0f);
}
/* 02621134 */
void hdBatonSignForWindow_c::inEnter() { playBatonWindowAnimation(this, 0); }
/* 0262113C */
u32 hdBatonSignForWindow_c::animationDone(u32 animation) { return hdAnimSet_isEnd(animSet(), animation); }
/* 02621148 */
void hdBatonSignForWindow_c::inExecute() {
    if (queryBatonWindowAnimation(this, 0)) changeState(&sBatonWindowWait);
}
/* 02621190 */
void hdBatonSignForWindow_c::outEnter() { playBatonWindowAnimation(this, 1); }
/* 02621198 */
void hdBatonSignForWindow_c::outExecute() {
    if (queryBatonWindowAnimation(this, 1)) changeState(&sBatonWindowBaseBlank);
}
/* 026211E0 */
hdBatonSignForWindow_c* hdBatonSignForWindow_c::construct(hdBatonSignForWindow_c* self) {
    if (self == NULL) {
        self = allocateBatonWindow(sizeof(hdBatonSignForWindow_c));
        if (self == NULL) return self;
    }
    constructBatonWindowBase(self);
    self->mStateHolder.mHolderVtable = &sBatonWindowHolderVtable;
    self->mVtable = &sBatonWindowScreenVtable;
    self->mSecondaryVtable = &sBatonWindowSecondaryVtable;
    self->mSignFrame = 0;
    return self;
}
/* 02621254 */
void hdBatonSignForWindow_c::setAnimationFrame(u32 animation, u32 frame) {
    hdAnimSet_setFrame(animSet(), animation, batonWindowAnimationGroup(animation), f32(frame));
}
/* 026212AC */
u32 hdBatonSignForWindow_c::initialize(u32 first, u32 second, void* descriptor) {
    if (!vLoad(&sBatonWindowScreenName, 0, first, second, descriptor)) return 0;
    applyBatonWindowFrame(this, 0, 0);
    startState(&sBatonWindowBaseBlank);
    return 1;
}
/* 02621360 */
void hdBatonSignForWindow_c::finalize() { vDestroyLayout(); }
/* 02621370 */
void hdBatonSignForWindow_c::executeState() { runBatonWindowState(stateMachine()); }
/* 02621378 */
void hdBatonSignForWindow_c::setSignFrame(u32 frame) {
    mSignFrame = frame;
    applyBatonWindowFrame(this, 2, frame);
}
/* 02621388 */
void hdBatonSignForWindow_c::resetSignFrame() { applyBatonWindowFrame(this, 0, 0); }
/* 02621394 */
void hdBatonSignForWindow_c::requestIn() { changeState(&sBatonWindowIn); }
/* 026213A4 */
void hdBatonSignForWindow_c::requestOut() { changeState(&sBatonWindowOut); }
/* 026213B4 */
u32 hdBatonSignForWindow_c::createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = allocateBatonWindowLayout(sizeof(hdPartsLayout_c), sBatonWindowUiSystem->mHeap.get(), 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL) return 0;
    if (!layout->vLoad(resource, first, second, third, 3, 2, descriptor)) return 0;
    for (u32 i = 0; i < 3; ++i) {
        u32 group = sBatonWindowAnimationMap.mGroups[i];
        bindBatonWindowAnimation(animSet(), i, &sBatonWindowAnimations.mEntries[i], &sBatonWindowGroups.mEntries[group]);
    }
    return 1;
}
/* 026214CC */
static void initializeBatonWindowStatics() {
    sBatonWindowHeaderWords.mWords[3] = 0;
    sBatonWindowHeaderWords.mWords[2] = 0;
    sBatonWindowHeaderWords.mWords[1] = 0;
    sBatonWindowHeaderWords.mWords[0] = 0;
    registerBatonWindowGlobal(&sBatonWindowHeaderRegistration);
    sBatonWindowMinimumRotation = -3.1415927410125732f;
    sBatonWindowMaximumRotation = 3.1415927410125732f;
    initializeBatonWindowRotation(&sBatonWindowRotationGuard);
    registerBatonWindowGlobal(&sBatonWindowRotationRegistration);
    initializePositiveBatonWindowRotation(&sBatonWindowPositiveRotationGuard);
    registerBatonWindowGlobal(&sBatonWindowPositiveRotationRegistration);
    sBatonWindowGroups.mEntries[0].mVtable = &sBatonWindowStringVtable;
    sBatonWindowGroups.mEntries[0].mText.v = gabi::guest_addr("G_InOut_00");
    sBatonWindowAnimations.mEntries[0].mVtable = &sBatonWindowStringVtable;
    sBatonWindowAnimations.mEntries[0].mText.v = gabi::guest_addr("In");
    sBatonWindowAnimations.mEntries[1].mVtable = &sBatonWindowStringVtable;
    u32 ready = sBatonWindowStateCounterReady;
    sBatonWindowAnimations.mEntries[1].mText.v = gabi::guest_addr("Out");
    sBatonWindowScreenName.mVtable = &sBatonWindowStringVtable;
    sBatonWindowGroups.mEntries[1].mVtable = &sBatonWindowStringVtable;
    sBatonWindowGroups.mEntries[1].mText.v = gabi::guest_addr("G_Sign_00");
    sBatonWindowAnimations.mEntries[2].mVtable = &sBatonWindowStringVtable;
    sBatonWindowAnimations.mEntries[2].mText.v = gabi::guest_addr("Sign");
    sBatonWindowScreenName.mText.v = gabi::guest_addr("BatonSignForWindow_00");
    u32 counter;
    if (ready) counter = sBatonWindowStateCounter;
    else { sBatonWindowStateCounterReady = 1; counter = 0; }
    hdPartsStateIdVtbl_c* stateVtable = &sBatonWindowStateVtable;
    sBatonWindowIn.setup(s32(++counter), "::StateID_In", stateVtable, 0x19, &sBatonWindowNullState);
    sBatonWindowWait.setup(s32(++counter), "::StateID_Wait", stateVtable, 0x1C, &sBatonWindowNullState);
    sBatonWindowOut.setup(s32(++counter), "::StateID_Out", stateVtable, 0x1F, &sBatonWindowNullState);
    sBatonWindowStateCounter = counter;
}

/* WWHD glue
data sBatonWindowHeaderWords 1048E704
data sBatonWindowHeaderRegistration 101F5224
data sBatonWindowRotationRegistration 101F5230
data sBatonWindowPositiveRotationRegistration 101F523C
data sBatonWindowMinimumRotation 1048E6F8
data sBatonWindowMaximumRotation 1048E6FC
data sBatonWindowRotationGuard 1048E700
data sBatonWindowPositiveRotationGuard 1048E701
data sBatonWindowGroups 1048E714
data sBatonWindowAnimations 1048E724
data sBatonWindowScreenName 1048E7CC
data sBatonWindowAnimationMap 100E3EA4
data sBatonWindowUiSystem 1018C404
data sBatonWindowIn 1048E73C
data sBatonWindowWait 1048E76C
data sBatonWindowOut 1048E79C
data sBatonWindowBaseBlank 1049DD60
data sBatonWindowStateCounterReady 101FD8E4
data sBatonWindowStateCounter 101FDD50
data sBatonWindowNullState 101FF32C
str 100E3F9C "G_InOut_00" in 026214CC
str 100E3F68 "G_Sign_00" in 026214CC
str 100E3F5C "In" in 026214CC
str 100E3F58 "Out" in 026214CC
str 100E3F60 "Sign" in 026214CC
str 100E3F74 "BatonSignForWindow_00" in 026214CC
str 100E3FA8 "::StateID_In" in 026214CC
str 100E3FB8 "::StateID_Wait" in 026214CC
str 100E3F8C "::StateID_Out" in 026214CC
ext 0273AD10 hdBatonSignForWindow_c* allocateBatonWindow(u32 size)
ext 0273AF40 void deleteBatonWindow(void* object)
ext 026F88F0 void destroyBatonWindowBase(hdParts_c* self, s32 flags)
ext 026F89E8 void constructBatonWindowBase(hdParts_c* self)
ext 0273B050 hdPartsLayout_c* allocateBatonWindowLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 02004E04 void bindBatonWindowAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group)
ext 028F026C void registerBatonWindowGlobal(hdBatonWindowRegistration_c* registration)
ext 028ED6F8 void initializeBatonWindowRotation(be<u8>* guard)
ext 028EAB2C void initializePositiveBatonWindowRotation(be<u8>* guard)
ext 02621110 void playBatonWindowAnimation(hdBatonSignForWindow_c* self, u32 animation)
ext 0262113C u32 queryBatonWindowAnimation(hdBatonSignForWindow_c* self, u32 animation)
ext 02621254 void applyBatonWindowFrame(hdBatonSignForWindow_c* self, u32 animation, u32 frame)
ext 02006364 void runBatonWindowState(hdStateMachine* machine)
data sBatonWindowScreenVtable 100E3FC8
data sBatonWindowHolderVtable 100E40D8
data sBatonWindowSecondaryVtable 100E40E8
data sBatonWindowStringVtable 100E3E8C
data sBatonWindowStateVtable 100E3EF8
*/
