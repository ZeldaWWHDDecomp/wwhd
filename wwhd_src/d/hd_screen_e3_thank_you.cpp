/* HD E3ThankYou_00 leftover demo state callbacks. Private game-derived reconstruction. */
#include "d/hd_screen_e3_thank_you.h"

static u32 sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
static hdE3ThankYouRuntimeWords sThankYouRuntime;
static hdE3ThankYouRegistration sThankYouRuntimeRegistration;
static hdE3ThankYouRegistration sThankYouFirstRegistration;
static hdE3ThankYouRegistration sThankYouSecondRegistration;
static hdE3ThankYouContext sThankYouFirstContext;
static hdE3ThankYouContext sThankYouSecondContext;
static f32 sThankYouMinimumAngle;
static f32 sThankYouMaximumAngle;
static hdUiSafeString sThankYouGroupName;
static hdUiSafeString sThankYouLayoutName;
static hdUiSafeString sThankYouArchiveName;
static hdE3ThankYouAnimationNames sThankYouAnimationNames;
static hdPartsStateId_c sThankYouInAllState;
static hdPartsStateId_c sThankYouOutAllState;
static hdPartsStateCounter_c sThankYouCounterReady;
static hdPartsStateCounter_c sThankYouNextState;
static hdStateId sThankYouNullState;
extern void hdThankYouRegisterStatics(hdE3ThankYouRegistration* registration);
extern void hdThankYouConstructFirstContext(hdE3ThankYouContext* context);
extern void hdThankYouConstructSecondContext(hdE3ThankYouContext* context);

/* 026677F8 */
u32 hdE3ThankYou_c::acceptsInput() { return 0; }

/* 0266780C */
u32 hdE3ThankYou_c::isComplete() { return 0; }

/* 02667814 */
u32 hdE3ThankYouStateKey(hdPartsStateId_c* state) { return state->mNumber; }

/* HD: compiler-generated static setup for this translation unit. */
/* 02667914 */
static void initializeE3ThankYouStatics() {
    sThankYouRuntime.mWords[0] = 0;
    sThankYouRuntime.mWords[2] = 0;
    sThankYouRuntime.mWords[3] = 0;
    sThankYouRuntime.mWords[1] = 0;
    hdThankYouRegisterStatics(&sThankYouRuntimeRegistration);
    sThankYouMinimumAngle = -3.1415927410125732f;
    sThankYouMaximumAngle = 3.1415927410125732f;
    hdThankYouConstructFirstContext(&sThankYouFirstContext);
    hdThankYouRegisterStatics(&sThankYouFirstRegistration);
    hdThankYouConstructSecondContext(&sThankYouSecondContext);
    hdThankYouRegisterStatics(&sThankYouSecondRegistration);
    sThankYouGroupName.mVtable.v = gabi::ea(&sStringVtable);
    sThankYouGroupName.mText.v = gabi::guest_addr("G_All_00");
    sThankYouAnimationNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sThankYouAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    sThankYouAnimationNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sThankYouAnimationNames.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sThankYouArchiveName.mVtable.v = gabi::ea(&sStringVtable);
    sThankYouArchiveName.mText.v = gabi::guest_addr("E3ThankYou_00");
    sThankYouLayoutName.mVtable.v = gabi::ea(&sStringVtable);
    sThankYouLayoutName.mText.v = gabi::guest_addr("E3ThankYou_00");
    u32 first;
    if (sThankYouCounterReady.mValue) first = u32(sThankYouNextState.mValue) + 1;
    else { sThankYouCounterReady.mValue = 1; first = 1; }
    auto* stateVtable = &sStateVtable;
    sThankYouInAllState.setup(first, "::StateID_InAll", stateVtable, 0x19, &sThankYouNullState);
    sThankYouNextState.mValue = first + 1;
    sThankYouOutAllState.setup(first + 1, "::StateID_OutAll", stateVtable, 0x1C, &sThankYouNullState);
}

/* WWHD glue
data sStringVtable 100F3F0C nocheck
data sStateVtable 100F3F6C nocheck
data sThankYouRuntime 10493E68
data sThankYouRuntimeRegistration 101F5E48
data sThankYouFirstRegistration 101F5E54
data sThankYouSecondRegistration 101F5E60
data sThankYouFirstContext 10493E64
data sThankYouSecondContext 10493E65
data sThankYouMinimumAngle 10493E5C
data sThankYouMaximumAngle 10493E60
data sThankYouGroupName 10493E4C
data sThankYouLayoutName 10493E54
data sThankYouArchiveName 10493EE8
data sThankYouAnimationNames 10493E78
data sThankYouInAllState 10493E88
data sThankYouOutAllState 10493EB8
data sThankYouCounterReady 101FD8E4
data sThankYouNextState 101FDD50
data sThankYouNullState 101FF32C
str 100F3FF0 "G_All_00" in 02667914
str 100F3FE0 "E3ThankYou_00" in 02667914
str 100F3FC0 "InAll" in 02667914
str 100F3FC8 "OutAll" in 02667914
str 100F3FD0 "::StateID_InAll" in 02667914
str 100F3FFC "::StateID_OutAll" in 02667914
ext 028F026C void hdThankYouRegisterStatics(hdE3ThankYouRegistration* registration)
ext 028ED6F8 void hdThankYouConstructFirstContext(hdE3ThankYouContext* context)
ext 028EAB2C void hdThankYouConstructSecondContext(hdE3ThankYouContext* context)
*/
