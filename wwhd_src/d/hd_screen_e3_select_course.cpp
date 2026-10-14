/* HD E3SelectCourse_00 leftover demo screen. Private game-derived reconstruction. */
#include "d/hd_screen_e3_select_course.h"

static u32 sStringVtable;
static u32 sDirectStateVtable;
static hdPartsStateIdVtbl_c sPartsStateVtable;
static hdE3CourseRuntimeWords sCourseRuntime;
static hdE3CourseRegistration sCourseRuntimeRegistration;
static hdE3CourseRegistration sCourseFirstRegistration;
static hdE3CourseRegistration sCourseSecondRegistration;
static hdE3CourseContext sCourseFirstContext;
static hdE3CourseContext sCourseSecondContext;
static f32 sCourseMinimumAngle;
static f32 sCourseMaximumAngle;
static hdUiSafeString sCourseGroupName;
static hdE3CourseStringPair sCourseLayoutNames;
static hdUiSafeString sCourseArchiveName;
static hdE3CourseStringPair sCourseAnimationNames;
static hdE3CourseStringPair sCourseButtonGroups;
static hdE3CourseStringPair sCourseButtonArchives;
static hdE3CourseCallbackState_l sCourseWaitState;
static hdE3CourseCallbackState_l sCourseOutAllEndState;
static hdPartsStateId_c sCourseDisplayState;
static hdPartsStateId_c sCourseInAllState;
static hdPartsStateId_c sCourseOutAllState;
static hdPartsStateCounter_c sCourseCounterReady;
static hdPartsStateCounter_c sCourseNextState;
static hdStateId sCourseNullState;
static hdStateId sCourseDisplayNullState;
extern void hdCourseRegisterStatics(hdE3CourseRegistration* registration);
extern void hdCourseConstructFirstContext(hdE3CourseContext* context);
extern void hdCourseConstructSecondContext(hdE3CourseContext* context);
extern void hdCourseWaitInitialize(hdE3SelectCourse_c* screen);
extern void hdCourseWaitExecute(hdE3SelectCourse_c* screen);
extern void hdCourseWaitFinalize(hdE3SelectCourse_c* screen);
extern u32 hdCourseWaitIsDone(hdE3SelectCourse_c* screen);
extern void hdCourseOutAllEndInitialize(hdE3SelectCourse_c* screen);
extern void hdCourseOutAllEndExecute(hdE3SelectCourse_c* screen);
extern void hdCourseOutAllEndFinalize(hdE3SelectCourse_c* screen);
extern u32 hdCourseOutAllEndIsDone(hdE3SelectCourse_c* screen);

/* inlined into initializeE3CourseStatics */
static void setCourseDirectDelegate(hdPartsPtmf_c& delegate, const void* function) {
    delegate.mThisDelta = 0;
    delegate.mVirtualIndex = -1;
    delegate.mFunction = gabi::guest_fn(function);
}

/* 026673DC */
void hdE3SelectCourse_c::initializeAnimation() {
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
}

/* HD: compiler-generated static setup for this translation unit. */
/* 0266742C */
static void initializeE3CourseStatics() {
    sCourseRuntime.mWords[2] = 0;
    sCourseRuntime.mWords[0] = 0;
    sCourseRuntime.mWords[3] = 0;
    sCourseRuntime.mWords[1] = 0;
    hdCourseRegisterStatics(&sCourseRuntimeRegistration);
    sCourseMinimumAngle = -3.1415927410125732f;
    sCourseMaximumAngle = 3.1415927410125732f;
    hdCourseConstructFirstContext(&sCourseFirstContext);
    hdCourseRegisterStatics(&sCourseFirstRegistration);
    hdCourseConstructSecondContext(&sCourseSecondContext);
    hdCourseRegisterStatics(&sCourseSecondRegistration);
    sCourseGroupName.mVtable.v = gabi::ea(&sStringVtable);
    sCourseGroupName.mText.v = gabi::guest_addr("G_All_00");
    sCourseButtonGroups.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sCourseButtonGroups.mNames[0].mText.v = gabi::guest_addr("L_Btn_01");
    sCourseButtonGroups.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sCourseButtonGroups.mNames[1].mText.v = gabi::guest_addr("L_Btn_00");
    sCourseLayoutNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sCourseLayoutNames.mNames[0].mText.v = gabi::guest_addr("E3SelectCourse_00");
    sCourseAnimationNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sCourseAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    sCourseAnimationNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sCourseAnimationNames.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sCourseLayoutNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sCourseLayoutNames.mNames[1].mText.v = gabi::guest_addr("E3BtnCourse_00");
    sCourseButtonArchives.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sCourseButtonArchives.mNames[0].mText.v = gabi::guest_addr("E3BtnCourse_00");
    sCourseButtonArchives.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sCourseButtonArchives.mNames[1].mText.v = gabi::guest_addr("E3BtnCourse_00");
    sCourseArchiveName.mVtable.v = gabi::ea(&sStringVtable);
    sCourseArchiveName.mText.v = gabi::guest_addr("E3SelectCourse_00");
    u32 number;
    if (sCourseCounterReady.mValue) number = sCourseNextState.mValue;
    else { sCourseCounterReady.mValue = 1; number = 0; }
    sCourseWaitState.mNumber = ++number;
    sCourseWaitState.mName.v = gabi::guest_addr("::StateID_Wait");
    sCourseWaitState.mVtable.v = gabi::ea(&sDirectStateVtable);
    setCourseDirectDelegate(sCourseWaitState.mInitialize, reinterpret_cast<const void*>(hdCourseWaitInitialize));
    setCourseDirectDelegate(sCourseWaitState.mExecute, reinterpret_cast<const void*>(hdCourseWaitExecute));
    setCourseDirectDelegate(sCourseWaitState.mFinalize, reinterpret_cast<const void*>(hdCourseWaitFinalize));
    setCourseDirectDelegate(sCourseWaitState.mIsDone, reinterpret_cast<const void*>(hdCourseWaitIsDone));
    auto* stateVtable = &sPartsStateVtable;
    sCourseDisplayState.setup(++number, "::StateID_Display", stateVtable, 7, &sCourseDisplayNullState);
    sCourseInAllState.setup(++number, "::StateID_InAll", stateVtable, 0x19, &sCourseNullState);
    sCourseOutAllState.setup(++number, "::StateID_OutAll", stateVtable, 0x1C, &sCourseNullState);
    sCourseOutAllEndState.mNumber = ++number;
    sCourseOutAllEndState.mName.v = gabi::guest_addr("::StateID_OutAllEnd");
    sCourseOutAllEndState.mVtable.v = gabi::ea(&sDirectStateVtable);
    setCourseDirectDelegate(sCourseOutAllEndState.mInitialize, reinterpret_cast<const void*>(hdCourseOutAllEndInitialize));
    setCourseDirectDelegate(sCourseOutAllEndState.mExecute, reinterpret_cast<const void*>(hdCourseOutAllEndExecute));
    setCourseDirectDelegate(sCourseOutAllEndState.mFinalize, reinterpret_cast<const void*>(hdCourseOutAllEndFinalize));
    setCourseDirectDelegate(sCourseOutAllEndState.mIsDone, reinterpret_cast<const void*>(hdCourseOutAllEndIsDone));
    sCourseNextState.mValue = number;
}

/* WWHD glue
data sStringVtable 100F3D44 nocheck
data sDirectStateVtable 100F3DA4 nocheck
data sPartsStateVtable 100F3DEC nocheck
data sCourseRuntime 10493D0C
data sCourseRuntimeRegistration 101F5E24
data sCourseFirstRegistration 101F5E30
data sCourseSecondRegistration 101F5E3C
data sCourseFirstContext 10493D08
data sCourseSecondContext 10493D09
data sCourseMinimumAngle 10493D00
data sCourseMaximumAngle 10493D04
data sCourseGroupName 10493CF8
data sCourseLayoutNames 10493D4C
data sCourseArchiveName 10493DEC
data sCourseAnimationNames 10493D1C
data sCourseButtonGroups 10493D2C
data sCourseButtonArchives 10493D3C
data sCourseWaitState 10493DF4
data sCourseOutAllEndState 10493E20
data sCourseDisplayState 10493D5C
data sCourseInAllState 10493D8C
data sCourseOutAllState 10493DBC
data sCourseCounterReady 101FD8E4
data sCourseNextState 101FDD50
data sCourseNullState 101FF32C
data sCourseDisplayNullState 1049DD90
str 100F3EB0 "G_All_00" in 0266742C
str 100F3EBC "L_Btn_01" in 0266742C
str 100F3EC8 "L_Btn_00" in 0266742C
str 100F3E88 "E3SelectCourse_00" in 0266742C
str 100F3E54 "InAll" in 0266742C
str 100F3E5C "OutAll" in 0266742C
str 100F3ED4 "E3BtnCourse_00" in 0266742C
str 100F3EE4 "::StateID_Wait" in 0266742C
str 100F3E9C "::StateID_Display" in 0266742C
str 100F3E64 "::StateID_InAll" in 0266742C
str 100F3EF4 "::StateID_OutAll" in 0266742C
str 100F3E74 "::StateID_OutAllEnd" in 0266742C
ext 028F026C void hdCourseRegisterStatics(hdE3CourseRegistration* registration)
ext 028ED6F8 void hdCourseConstructFirstContext(hdE3CourseContext* context)
ext 028EAB2C void hdCourseConstructSecondContext(hdE3CourseContext* context)
ext 026673DC void hdCourseWaitInitialize(hdE3SelectCourse_c* screen) callback
ext 026677F0 void hdCourseWaitExecute(hdE3SelectCourse_c* screen) callback
ext 026677F4 void hdCourseWaitFinalize(hdE3SelectCourse_c* screen) callback
ext 026677F8 u32 hdCourseWaitIsDone(hdE3SelectCourse_c* screen) callback
ext 02667800 void hdCourseOutAllEndInitialize(hdE3SelectCourse_c* screen) callback
ext 02667804 void hdCourseOutAllEndExecute(hdE3SelectCourse_c* screen) callback
ext 02667808 void hdCourseOutAllEndFinalize(hdE3SelectCourse_c* screen) callback
ext 0266780C u32 hdCourseOutAllEndIsDone(hdE3SelectCourse_c* screen) callback
*/
