/* HD EndingEnd screen; no GameCube counterpart. */
#include "d/hd_screen_ending_end.h"
static u32 sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
static u32 sScreenVtable;
static u32 sStateHolderVtable;
static u32 sSecondaryVtable;
static hdEndingRuntimeWords sEndingRuntime;
static hdEndingRegistration sEndingRuntimeRegistration;
static hdEndingRegistration sEndingFirstRegistration;
static hdEndingRegistration sEndingSecondRegistration;
static hdEndingContext sEndingFirstContext;
static hdEndingContext sEndingSecondContext;
static f32 sEndingMinimumAngle;
static f32 sEndingMaximumAngle;
static hdPartsStateCounter_c sEndingCounterReady;
static hdPartsStateCounter_c sEndingNextState;
static hdPartsStateId_c sEndingAllState;
static hdStateId sEndingNullState;
extern void hdEndingRegisterStatics(hdEndingRegistration* registration);
extern void hdEndingConstructFirstContext(hdEndingContext* context);
extern void hdEndingConstructSecondContext(hdEndingContext* context);
static hdEndingUiPointer sEndingUiSystem;
static hdUiSafeString sEndingArchiveName;
static hdUiSafeString sEndingLayoutName;
static hdUiSafeString sEndingAnimationName;
static hdUiSafeString sEndingAnimationGroup;
static hdEndingGroupIndex sEndingGroupIndex;
extern void hdEndingBindAnimation(hdAnimSet* animation, u32 entry, hdUiSafeString* name, hdUiSafeString* group);
static f32 sEndingAnimationRate;
static hdPartsStateId_c sEndingDisplayState;
extern void hdEndingDestroyBase(hdParts_c* screen, u32 flags);
extern void hdEndingConstructBase(hdParts_c* screen);
extern hdEndingEnd_c* hdEndingAllocate(u32 size);
extern void hdEndingDelete(void* screen);
extern void hdEndingReleaseArchives(hdParts_c* screen);
extern void hdEndingAttachArchives(hdParts_c* screen, hdUiSafeString* names, u32 count);
/* 02667C04 */
u32 hdEndingEndStateKey(hdPartsStateId_c* state) {
    return state->mNumber;
}
/* 02667D04 */
void hdEndingEnd_c::destroy(hdEndingEnd_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    hdEndingDestroyBase(screen, 0);
    if (flags & 1) hdEndingDelete(screen);
}
/* 02667D70 */
void hdEndingEnd_c::allEnter() {
    hdAnimSet_play(animSet(), 0, 0, sEndingAnimationRate);
}
/* 02667D8C */
void hdEndingEnd_c::allExecute() {
    if (hdAnimSet_isEnd(animSet(), 0)) changeState(&sEndingDisplayState);
    vCalcLayout();
}
/* 02667DF0 */
hdEndingEnd_c* hdEndingEnd_c::construct(hdEndingEnd_c* screen) {
    if (!screen) {
        screen = hdEndingAllocate(sizeof(hdEndingEnd_c));
        if (!screen) return nullptr;
    }
    hdEndingConstructBase(screen);
    screen->mVtable.v = gabi::ea(&sScreenVtable);
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    return screen;
}
/* 02667E5C */
u32 hdEndingEnd_c::initializeEnding(u32 resource, u32 option, void* archive) {
    hdEndingAttachArchives(this, &sEndingArchiveName, 1);
    auto load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sEndingLayoutName, accessor, resource, option, archive)) return 0;
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sEndingDisplayState);
    return 1;
}
/* 02667FB4 */
u32 hdEndingEnd_c::loadEnding(hdUiSafeString* name, hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* archive) {
    hdScreenHeap* heap = sEndingUiSystem->mHeap;
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, gabi::ea(accessor), resource, option, 1, 1, archive)) return 0;
    hdAnimSet* animation = animSet();
    const u32 group = sEndingGroupIndex.mIndex;
    hdUiSafeString* groupName = gabi::at<hdUiSafeString>(gabi::ea(&sEndingAnimationGroup) + group * sizeof(hdUiSafeString));
    hdEndingBindAnimation(animation, 0, &sEndingAnimationName, groupName);
    vCalcLayout();
    return 1;
}
/* 02667F6C */
void hdEndingEnd_c::finalizeEnding() {
    vDestroyLayout();
    hdEndingReleaseArchives(this);
}
/* 02667FAC */
void hdEndingEnd_c::executeEndingState() {
    hdParts_executeStateMachine(stateMachine());
}
/* 026680B4 */
static void initializeEndingStatics() {
    sEndingRuntime.mWords[2] = 0;
    sEndingRuntime.mWords[0] = 0;
    sEndingRuntime.mWords[3] = 0;
    sEndingRuntime.mWords[1] = 0;
    hdEndingRegisterStatics(&sEndingRuntimeRegistration);
    sEndingMinimumAngle = -3.1415927410125732f;
    sEndingMaximumAngle = 3.1415927410125732f;
    hdEndingConstructFirstContext(&sEndingFirstContext);
    hdEndingRegisterStatics(&sEndingFirstRegistration);
    hdEndingConstructSecondContext(&sEndingSecondContext);
    hdEndingRegisterStatics(&sEndingSecondRegistration);
    sEndingAnimationGroup.mVtable.v = gabi::ea(&sStringVtable);
    sEndingArchiveName.mVtable.v = gabi::ea(&sStringVtable);
    sEndingAnimationName.mVtable.v = gabi::ea(&sStringVtable);
    sEndingAnimationName.mText.v = gabi::guest_addr("All");
    sEndingAnimationGroup.mText.v = gabi::guest_addr("G_All_00");
    sEndingLayoutName.mVtable.v = gabi::ea(&sStringVtable);
    sEndingArchiveName.mText.v = gabi::guest_addr("EndingEnd_00");
    sEndingLayoutName.mText.v = gabi::guest_addr("EndingEnd_00");
    u32 number;
    if (sEndingCounterReady.mValue) number = u32(sEndingNextState.mValue) + 1;
    else { sEndingCounterReady.mValue = 1; number = 1; }
    sEndingAllState.setup(number, "::StateID_All", &sStateVtable, 0x19, &sEndingNullState);
    sEndingNextState.mValue = number;
}
/* WWHD glue
data sStringVtable 100F4018 nocheck
data sStateVtable 100F4078 nocheck
data sScreenVtable 100F4100 nocheck
data sStateHolderVtable 100F41E0 nocheck
data sSecondaryVtable 100F41F0 nocheck
data sEndingRuntime 10493F14
data sEndingRuntimeRegistration 101F5E6C
data sEndingFirstRegistration 101F5E78
data sEndingSecondRegistration 101F5E84
data sEndingFirstContext 10493F10
data sEndingSecondContext 10493F11
data sEndingMinimumAngle 10493F08
data sEndingMaximumAngle 10493F0C
data sEndingCounterReady 101FD8E4
data sEndingNextState 101FDD50
data sEndingAllState 10493F24
data sEndingNullState 101FF32C
str 100F40CC "All" in 026680B4
str 100F40D0 "::StateID_All" in 026680B4
str 100F40E0 "G_All_00" in 026680B4
str 100F40EC "EndingEnd_00" in 026680B4
ext 028F026C void hdEndingRegisterStatics(hdEndingRegistration* registration)
ext 028ED6F8 void hdEndingConstructFirstContext(hdEndingContext* context)
ext 028EAB2C void hdEndingConstructSecondContext(hdEndingContext* context)
data sEndingUiSystem 1018C404
data sEndingArchiveName 10493F00
data sEndingLayoutName 10493F54
data sEndingAnimationName 10493EF8
data sEndingAnimationGroup 10493EF0
data sEndingGroupIndex 100F4014
ext 02004E04 void hdEndingBindAnimation(hdAnimSet* animation, u32 entry, hdUiSafeString* name, hdUiSafeString* group)
data sEndingAnimationRate 100F40C0
data sEndingDisplayState 1049DD60
ext 026F88F0 void hdEndingDestroyBase(hdParts_c* screen, u32 flags)
ext 026F89E8 void hdEndingConstructBase(hdParts_c* screen)
ext 0273AD10 hdEndingEnd_c* hdEndingAllocate(u32 size)
ext 0273AF40 void hdEndingDelete(void* screen)
ext 026F91F8 void hdEndingReleaseArchives(hdParts_c* screen)
ext 026F90D8 void hdEndingAttachArchives(hdParts_c* screen, hdUiSafeString* names, u32 count)
*/
