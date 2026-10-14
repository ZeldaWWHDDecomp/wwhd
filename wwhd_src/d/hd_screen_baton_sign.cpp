/* HD BatonSign_00: In/Disp/Out states and the selected baton sign frame. */
#include "d/hd_screen_baton_sign.h"
#include "d/hd_layout_mgr.h"
static hdPartsVtbl_c sBatonSignScreenVtable;
static u8 sBatonSignHolderVtable;
static u8 sBatonSignSecondaryVtable;
static hdUiStringTable sBatonSignStringVtable;
static hdPartsStateIdVtbl_c sBatonSignStateVtable;
enum hdBatonSignAnimation_e { BATON_SIGN_IN, BATON_SIGN_OUT, BATON_SIGN_FINISH = 3, BATON_SIGN_FRAME };
struct hdBatonSignHeaderWords_c { be<u32> mWords[4]; };
struct hdBatonSignRegistration_c { u8 mStorage[12]; };
struct hdBatonSignGroupNames_c { hdUiSafeString mEntries[3]; };
struct hdBatonSignAnimationNames_c { hdUiSafeString mEntries[2]; };
struct hdBatonSignAnimationMap_c { be<u32> mGroups[2]; };
using hdBatonSignUiSystemPointer = gptr<hdScreenUiSystem>;
static hdBatonSignHeaderWords_c sBatonSignHeaderWords;
static hdBatonSignRegistration_c sBatonSignHeaderRegistration;
static hdBatonSignRegistration_c sBatonSignRotationRegistration;
static hdBatonSignRegistration_c sBatonSignPositiveRotationRegistration;
static f32 sBatonSignMinimumRotation;
static f32 sBatonSignMaximumRotation;
static u8 sBatonSignRotationGuard;
static u8 sBatonSignPositiveRotationGuard;
static hdBatonSignGroupNames_c sBatonSignGroups;
static hdBatonSignAnimationNames_c sBatonSignAnimations;
static hdUiSafeString sBatonSignScreenName;
static hdBatonSignAnimationMap_c sBatonSignAnimationMap;
static hdBatonSignUiSystemPointer sBatonSignUiSystem;
static hdPartsStateId_c sBatonSignWait;
static hdPartsStateId_c sBatonSignIn;
static hdPartsStateId_c sBatonSignDisplay;
static hdPartsStateId_c sBatonSignOut;
static u32 sBatonSignStateCounterReady;
static u32 sBatonSignStateCounter;
static hdStateId sBatonSignNullState;
static hdBatonSign_c* allocateBatonSign(u32 size);
static void deleteBatonSign(void* object);
static hdPartsLayout_c* allocateBatonSignLayout(u32 size, hdScreenHeap* heap, u32 alignment);
static void bindBatonSignAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group);
static void registerBatonSignGlobal(hdBatonSignRegistration_c* registration);
static void initializeBatonSignRotation(be<u8>* guard);
static void initializePositiveBatonSignRotation(be<u8>* guard);
static void destroyBatonSignLoopBase(hdPartsInOutLoop_c* self, s32 flags);
static void constructBatonSignLoopBase(hdPartsInOutLoop_c* self);
static u32 initializeBatonSignLoopBase(hdPartsInOutLoop_c* self, void* resource, u32 first, u32 second, hdPartsDesc_c* descriptor);

/* 02620780 */
void hdBatonSign_c::destroy(hdBatonSign_c* self, s32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sBatonSignHolderVtable;
    self->mSecondaryVtable = &sBatonSignSecondaryVtable;
    destroyBatonSignLoopBase(self, 0);
    if (flags & 1) deleteBatonSign(self);
}
/* 026207EC */
void hdBatonSign_c::waitExecute() {
    if (mSignRequested) changeState(&sBatonSignIn);
    vCalcLayout();
}
/* 02620844 */
void hdBatonSign_c::inEnter() {
    hdAnimSet_play(animSet(), BATON_SIGN_IN, 0, 1.0f);
    hdAnimSet_setFrame(animSet(), BATON_SIGN_FRAME, 1, f32(s32(mSignFrame)));
    mShownSignFrame = mSignFrame;
}
/* 026208CC */
void hdBatonSign_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), BATON_SIGN_IN)) changeState(&sBatonSignDisplay);
    vCalcLayout();
}
/* 02620930 */
void hdBatonSign_c::displayExecute() {
    if (mSignFrame != mShownSignFrame) {
        hdAnimSet_setFrame(animSet(), BATON_SIGN_FRAME, 1, f32(s32(mSignFrame)));
        mShownSignFrame = mSignFrame;
    }
    if (!mSignRequested) changeState(&sBatonSignOut);
    vCalcLayout();
}
/* 026209D8 */
void hdBatonSign_c::outEnter() { hdAnimSet_setFrame(animSet(), BATON_SIGN_IN, 0, 0.0f); }
/* 026209F4 */
void hdBatonSign_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), BATON_SIGN_OUT)) changeState(&sBatonSignWait);
    vCalcLayout();
}
/* 02620A58 */
hdBatonSign_c* hdBatonSign_c::construct(hdBatonSign_c* self) {
    if (self == NULL) {
        self = allocateBatonSign(sizeof(hdBatonSign_c));
        if (self == NULL) return self;
    }
    constructBatonSignLoopBase(self);
    self->mStateHolder.mHolderVtable = &sBatonSignHolderVtable;
    self->mVtable = &sBatonSignScreenVtable;
    self->mSecondaryVtable = &sBatonSignSecondaryVtable;
    self->mSignFrame = 0;
    self->mSignRequested = 0;
    self->mShownSignFrame = 0;
    return self;
}
/* 02620AD4 */
u32 hdBatonSign_c::initialize(u32 first, u32 second, hdPartsDesc_c* descriptor) {
    if (!initializeBatonSignLoopBase(this, &sBatonSignScreenName, first, second, descriptor)) return 0;
    startState(&sBatonSignWait);
    return 1;
}
/* 02620B68 */
void hdBatonSign_c::finish() { hdAnimSet_play(animSet(), BATON_SIGN_FINISH, 2, 1.0f); }
/* 02620B84 */
u32 hdBatonSign_c::createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = allocateBatonSignLayout(sizeof(hdPartsLayout_c), sBatonSignUiSystem->mHeap.get(), 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL) return 0;
    if (!layout->vLoad(resource, first, second, third, 5, 3, descriptor)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        u32 group = sBatonSignAnimationMap.mGroups[i];
        bindBatonSignAnimation(animSet(), BATON_SIGN_FINISH + i, &sBatonSignAnimations.mEntries[i], &sBatonSignGroups.mEntries[group]);
    }
    return 1;
}
/* 02620C9C */
static void initializeBatonSignStatics() {
    sBatonSignHeaderWords.mWords[3] = 0;
    sBatonSignHeaderWords.mWords[2] = 0;
    sBatonSignHeaderWords.mWords[1] = 0;
    sBatonSignHeaderWords.mWords[0] = 0;
    registerBatonSignGlobal(&sBatonSignHeaderRegistration);
    sBatonSignMinimumRotation = -3.1415927410125732f;
    sBatonSignMaximumRotation = 3.1415927410125732f;
    initializeBatonSignRotation(&sBatonSignRotationGuard);
    registerBatonSignGlobal(&sBatonSignRotationRegistration);
    initializePositiveBatonSignRotation(&sBatonSignPositiveRotationGuard);
    registerBatonSignGlobal(&sBatonSignPositiveRotationRegistration);
    sBatonSignGroups.mEntries[0].mVtable = &sBatonSignStringVtable;
    sBatonSignGroups.mEntries[0].mText.v = gabi::guest_addr("G_InOut_00");
    sBatonSignGroups.mEntries[1].mVtable = &sBatonSignStringVtable;
    sBatonSignGroups.mEntries[1].mText.v = gabi::guest_addr("G_Sign_00");
    sBatonSignGroups.mEntries[2].mVtable = &sBatonSignStringVtable;
    sBatonSignGroups.mEntries[2].mText.v = gabi::guest_addr("G_SiginFinish_00");
    u32 ready = sBatonSignStateCounterReady;
    sBatonSignAnimations.mEntries[0].mVtable = &sBatonSignStringVtable;
    sBatonSignAnimations.mEntries[0].mText.v = gabi::guest_addr("SignFinish");
    sBatonSignAnimations.mEntries[1].mVtable = &sBatonSignStringVtable;
    sBatonSignAnimations.mEntries[1].mText.v = gabi::guest_addr("Sign");
    sBatonSignScreenName.mVtable = &sBatonSignStringVtable;
    sBatonSignScreenName.mText.v = gabi::guest_addr("BatonSign_00");
    u32 counter;
    if (ready) counter = sBatonSignStateCounter;
    else { sBatonSignStateCounterReady = 1; counter = 0; }
    hdPartsStateIdVtbl_c* stateVtable = &sBatonSignStateVtable;
    sBatonSignWait.setup(s32(++counter), "::StateID_Wait", stateVtable, 0x19, &sBatonSignNullState);
    sBatonSignIn.setup(s32(++counter), "::StateID_In", stateVtable, 0x1C, &sBatonSignNullState);
    sBatonSignDisplay.setup(s32(++counter), "::StateID_Disp", stateVtable, 0x25, &sBatonSignNullState);
    sBatonSignOut.setup(s32(++counter), "::StateID_Out", stateVtable, 0x22, &sBatonSignNullState);
    sBatonSignStateCounter = counter;
}

/* WWHD glue
data sBatonSignHeaderWords 1048E5F8
data sBatonSignHeaderRegistration 101F5200
data sBatonSignRotationRegistration 101F520C
data sBatonSignPositiveRotationRegistration 101F5218
data sBatonSignMinimumRotation 1048E5EC
data sBatonSignMaximumRotation 1048E5F0
data sBatonSignRotationGuard 1048E5F4
data sBatonSignPositiveRotationGuard 1048E5F5
data sBatonSignGroups 1048E608
data sBatonSignAnimations 1048E620
data sBatonSignScreenName 1048E6F0
data sBatonSignAnimationMap 100E3BC0
data sBatonSignUiSystem 1018C404
data sBatonSignWait 1048E630
data sBatonSignIn 1048E660
data sBatonSignDisplay 1048E690
data sBatonSignOut 1048E6C0
data sBatonSignStateCounterReady 101FD8E4
data sBatonSignStateCounter 101FDD50
data sBatonSignNullState 101FF32C
str 100E3CB8 "G_InOut_00" in 02620C9C
str 100E3C9C "G_Sign_00" in 02620C9C
str 100E3CC4 "G_SiginFinish_00" in 02620C9C
str 100E3CD8 "SignFinish" in 02620C9C
str 100E3C94 "Sign" in 02620C9C
str 100E3CE4 "BatonSign_00" in 02620C9C
str 100E3CF4 "::StateID_Wait" in 02620C9C
str 100E3D04 "::StateID_In" in 02620C9C
str 100E3D14 "::StateID_Disp" in 02620C9C
str 100E3CA8 "::StateID_Out" in 02620C9C
ext 0273AD10 hdBatonSign_c* allocateBatonSign(u32 size)
ext 0273AF40 void deleteBatonSign(void* object)
ext 0273B050 hdPartsLayout_c* allocateBatonSignLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 02004E04 void bindBatonSignAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group)
ext 028F026C void registerBatonSignGlobal(hdBatonSignRegistration_c* registration)
ext 028ED6F8 void initializeBatonSignRotation(be<u8>* guard)
ext 028EAB2C void initializePositiveBatonSignRotation(be<u8>* guard)
ext 026F7520 void destroyBatonSignLoopBase(hdPartsInOutLoop_c* self, s32 flags)
ext 026F7754 void constructBatonSignLoopBase(hdPartsInOutLoop_c* self)
ext 026F7844 u32 initializeBatonSignLoopBase(hdPartsInOutLoop_c* self, void* resource, u32 first, u32 second, hdPartsDesc_c* descriptor)
data sBatonSignScreenVtable 100E3D28
data sBatonSignHolderVtable 100E3E68
data sBatonSignSecondaryVtable 100E3E78
data sBatonSignStringVtable 100E3BCC
data sBatonSignStateVtable 100E3C2C
*/
