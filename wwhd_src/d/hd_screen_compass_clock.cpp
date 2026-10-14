/* HD-only compass and clock meter. State and layout names are from the image. */
#include "d/hd_screen_compass_clock.h"
static hdPartsVtbl_c sCompassTable;
static u32 sCompassHolderTable;
static u32 sCompassSecondaryTable;
static hdPartsStateId_c sCompassWait;
static hdPartsStateId_c sCompassDisplay;
static hdPartsStateId_c sCompassIn;
static hdPartsStateId_c sCompassOut;
static hdPartsStateId_c sClockIn;
static hdPartsStateId_c sClockOut;
static hdUiSafeString sCompassPaneName;
static hdUiSafeString sClockPaneName;
static hdUiSafeString sCompassAnimationNames[7] = {};
static hdUiSafeString sCompassAnimationPanes[4] = {};
static hdUiSafeString sCompassName;
static hdUiStringTable sCompassStringTable;
static hdPartsStateIdVtbl_c sCompassStateTable;
static hdStateId sCompassNullState;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sCompassStateGuard;
static hdHudWord_t sCompassStateCounter;
static hdHudWord_t sCompassInitWords[4] = {};
static hdHudWord_t sCompassRegistrations[9] = {};
static hdHudWord_t sCompassPaneIndices[7] = {};
typedef be<u8> hdHudByte_t;
static hdHudByte_t sCompassActive;
static hdHudByte_t sCompassModeForced;
static hdHudByte_t sCompassReady;
static hdHudByte_t sCompassEnabled;
static f32 sCompassPlayRate;
static f32 sCompassZero;
static f32 sCompassAngleScale;
static f32 sCompassAngleUnits;
static f32 sCompassDayStart;
static f32 sCompassDayEnd;
static f32 sCompassInitialFrame;
static f32 sCompassInitialRate;
static f32 sCompassFrame;
static f32 sCompassRate;
static f64 sCompassSignedBias;
static f64 sCompassUnsignedBias;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sCompassUi;
typedef gptr<hdCompassGame_c> hdCompassGamePointer_t;
static hdCompassGamePointer_t sCompassGame;
typedef void* (*hdCompassActorJudge_t)(void*, void*);
void* compassActorPredicate(void*, void*);
void compassConstructBase(NoControlPart_l*); void compassDestroyBase(NoControlPart_l*, u32);
u32 compassSetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32);
hdCompassActor_c* compassFindActor(hdCompassActorJudge_t, be<u16>*);
u32 compassGetPhase(); void* compassGameInfo(); void compassPlaySound(u32);
void compassBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void compassRegisterStatic(void*); void compassConstructByte(void*); void compassConstructBool(void*);
static f32 compassSignedFloat(s32 value) {
    union { u64 bits; f64 value; } converted;
    converted.bits = 0x4330000000000000ULL|(u32(value)^0x80000000u);
    return (f32)(converted.value-sCompassSignedBias);
}
/* 0264C3BC */
static s32 compassStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0264C4BC */
void hdCompassClock_c::destroy(hdCompassClock_c* object, u32 flags) {
    if(!object) return;
    object->mStateHolder.mHolderVtable = &sCompassHolderTable;
    object->mSecondaryVtable = &sCompassSecondaryTable;
    compassDestroyBase(object, 0);
    if(flags&1) operator_delete(object);
}
/* 0264C528 */
void hdCompassClock_c::waitEnter() { hdAnimSet_setFrame(animSet(), 0, 1, sCompassZero); }
/* 0264C544 */
void hdCompassClock_c::waitExecute() { if(sCompassActive) changeState(&sCompassIn); vCalcLayout(); }
/* 0264C5A0 */
void hdCompassClock_c::updateCompass() {
    gabi::NativeFrame<0x28> frame;
    be<u16>* actorId = gabi::at<be<u16>>(frame + 8);
    *actorId=0xa5;
    hdCompassActor_c* actor = compassFindActor(compassActorPredicate, actorId);
    if(!actor) return;
    s32 heading = -(s32)(s16)(s32(actor->mHeading)+0x8000);
    hdCompassPane_c* pane = mpCompassPane;
    f32 angle = compassSignedFloat(heading);
    f32 x = pane->mRotationX, y = pane->mRotationY;
    angle = (f32)(angle*sCompassAngleScale);
    pane->mRotationX = x;
    pane->mRotationY = y;
    pane->mRotationZ = (f32)(angle*sCompassAngleUnits);
    pane->mFlags = u8(pane->mFlags)|0x10;
}
/* 0264C674 */
void hdCompassClock_c::updateClock() {
    hdCompassPane_c* pane = mpClockPane;
    f32 x = pane->mRotationX, y = pane->mRotationY;
    if(mCompassOnly) {
        u8 flags = pane->mFlags;
        pane->mRotationX = x; pane->mFlags = flags|0x10; pane->mRotationY = y;
        pane->mRotationZ = sCompassZero;
    } else {
        f32 angle = (f32)(sCompassAngleScale-sCompassGame->mClockAngle);
        u8 flags = pane->mFlags;
        pane->mRotationX = x; pane->mRotationZ = angle; pane->mRotationY = y; pane->mFlags = flags|0x10;
    }
}
/* 0264C70C */
void hdCompassClock_c::updatePhase() {
    u32 phase = compassGetPhase();
    u32 entry = mMode == COMPASS_CLOCK_NIGHT?5:4;
    mLastPhase = phase;
    u32 slot = sCompassPaneIndices[entry];
    hdAnimSet_setFrame(animSet(), entry, slot, compassSignedFloat((s32)phase));
}
/* 0264C790 */
u32 hdCompassClock_c::updateMode() {
    u32 previous = mMode;
    compassGameInfo();
    if(sCompassModeForced) { mMode = COMPASS_CLOCK_COMPASS; mCompassOnly = 0; return previous != COMPASS_CLOCK_COMPASS; }
    mCompassOnly = 1;
    f32 time = sCompassGame->mClockAngle;
    /* The original bge tests !LT, so an unordered time takes the night path. */
    u32 next = (!(time<sCompassDayStart) && time<sCompassDayEnd)?COMPASS_CLOCK_DAY:COMPASS_CLOCK_NIGHT;
    mMode = next;
    return previous != next;
}
/* 0264C850 */
void hdCompassClock_c::displayExecute() {
    updateCompass(); updateClock();
    u32 phase = compassGetPhase();
    if(mLastPhase != phase) updatePhase();
    if(!sCompassActive) changeState(&sCompassOut);
    else if(updateMode()) changeState(&sClockOut);
    vCalcLayout();
}
/* 0264C8F4 */
void hdCompassClock_c::setModeFrame() {
    union { u64 bits; f64 value; } converted;
    converted.bits = 0x4330000000000000ULL|u32(mMode);
    hdAnimSet_setFrame(animSet(), 3, 0, (f32)(converted.value-sCompassUnsignedBias));
}
/* 0264C948 */
void hdCompassClock_c::compassInEnter() {
    hdAnimSet_play(animSet(), 0, 1, sCompassPlayRate);
    mMode = COMPASS_CLOCK_UNSET; updateMode(); setModeFrame(); updatePhase(); compassPlaySound(0x81f);
}
/* 0264C9B4 */
void hdCompassClock_c::compassInExecute() {
    updateCompass(); updateClock();
    if(hdAnimSet_isEnd(animSet(), 0)) changeState(&sCompassDisplay);
    vCalcLayout();
}
/* 0264CA24 */
void hdCompassClock_c::clockInEnter() { hdAnimSet_play(animSet(), 6, 1, sCompassPlayRate); compassPlaySound(0x81f); }
/* 0264CA64 */
void hdCompassClock_c::clockInExecute() {
    updateCompass(); updateClock();
    if(hdAnimSet_isEnd(animSet(), 6)) changeState(&sCompassWait);
    vCalcLayout();
}
/* 0264CAD4 */
void hdCompassClock_c::compassOutEnter() {
    hdAnimSet_play(animSet(), 2, 1, sCompassPlayRate);
    setModeFrame(); updatePhase(); compassPlaySound(0x81f);
}
/* 0264CB30 */
void hdCompassClock_c::compassOutExecute() {
    updateCompass(); updateClock();
    if(hdAnimSet_isEnd(animSet(), 2)) changeState(&sCompassDisplay);
    vCalcLayout();
}
/* 0264CBA0 */
void hdCompassClock_c::clockOutEnter() { hdAnimSet_play(animSet(), 1, 1, sCompassPlayRate); compassPlaySound(0x81f); }
/* 0264CBE0 */
void hdCompassClock_c::clockOutExecute() {
    updateCompass(); updateClock();
    if(hdAnimSet_isEnd(animSet(), 1)) changeState(&sClockIn);
    vCalcLayout();
}
/* 0264CC50 */
hdCompassClock_c* hdCompassClock_c::construct(hdCompassClock_c* object) {
    if(!object) { object = (hdCompassClock_c*)operator_new(sizeof(hdCompassClock_c)); if(!object) return NULL; }
    compassConstructBase(object);
    object->mStateHolder.mHolderVtable = &sCompassHolderTable; object->mVtable = &sCompassTable;
    object->mSecondaryVtable = &sCompassSecondaryTable;
    object->mpClockPane = NULL; object->mpCompassPane = NULL; object->mCompassOnly = 0;
    object->mMode = 0; object->mLastPhase = 0;
    return object;
}
/* 0264CCD4 */
void hdCompassClock_c::findPanes() {
    hdCompassPane_c* root = (hdCompassPane_c*)mpLayout->lyt()->mpRootPane.get();
    hdCompassPaneTable_c* table = root->mVtable;
    sCompassPaneName.assureTermination();
    mpCompassPane = gabi::call_ptr<hdCompassPane_c*>(table->mFindPane.get(), root, sCompassPaneName.mText.get(), 1);
    table = root->mVtable;
    sClockPaneName.assureTermination();
    mpClockPane = gabi::call_ptr<hdCompassPane_c*>(table->mFindPane.get(), root, sClockPaneName.mText.get(), 1);
}
/* 0264CD98 */
u32 hdCompassClock_c::setup(u32 first, u32 second, u32 third) {
    if(!compassSetupBase(this, &sCompassName, first, second, third)) return 0;
    findPanes(); startState(&sCompassWait); return 1;
}
/* 0264CE34 */
void hdCompassClock_c::toWait() { changeState(&sCompassWait); }
/* 0264CE44 */
u32 hdCompassClock_c::load(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sCompassUi->mHeap, 4);
    if(layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if(!layout || !layout->vLoad(name, first, second, third, 7, 4, desc)) return 0;
    for(u32 i = 0; i<7; ++i) {
        u32 pane = sCompassPaneIndices[i];
        compassBindAnimation(animSet(), i, &sCompassAnimationNames[i], &sCompassAnimationPanes[pane]);
    }
    return 1;
}
/* 0264CF5C */
static void __sinit_hd_screen_compass_clock_cpp() {
    sCompassInitWords[3] = 0; sCompassInitWords[2] = 0; sCompassInitWords[1] = 0; sCompassInitWords[0] = 0;
    compassRegisterStatic(&sCompassRegistrations[0]);
    sCompassFrame = sCompassInitialFrame; sCompassRate = sCompassInitialRate;
    compassConstructByte(&sCompassReady); compassRegisterStatic(&sCompassRegistrations[3]);
    compassConstructBool(&sCompassEnabled); compassRegisterStatic(&sCompassRegistrations[6]);
    sCompassAnimationPanes[0].mVtable = &sCompassStringTable;
    sCompassAnimationPanes[0].mText.v = gabi::guest_addr("G_ClockChange_00");
    sCompassAnimationPanes[1].mVtable = &sCompassStringTable;
    sCompassAnimationPanes[1].mText.v = gabi::guest_addr("G_CompassClock_00");
    sCompassAnimationNames[0].mVtable = &sCompassStringTable;
    sCompassAnimationPanes[2].mVtable = &sCompassStringTable;
    sCompassAnimationPanes[2].mText.v = gabi::guest_addr("G_Day_00");
    sCompassAnimationNames[0].mText.v = gabi::guest_addr("CompassIn");
    sCompassAnimationNames[1].mVtable = &sCompassStringTable;
    sCompassAnimationNames[1].mText.v = gabi::guest_addr("ClockOut");
    sCompassAnimationNames[2].mVtable = &sCompassStringTable;
    sCompassAnimationNames[2].mText.v = gabi::guest_addr("ClockIn");
    sCompassAnimationNames[3].mVtable = &sCompassStringTable;
    sClockPaneName.mVtable = &sCompassStringTable;
    sClockPaneName.mText.v = gabi::guest_addr("P_Clock_00");
    sCompassAnimationNames[3].mText.v = gabi::guest_addr("ClockChange");
    sCompassAnimationNames[4].mVtable = &sCompassStringTable;
    sCompassAnimationPanes[3].mVtable = &sCompassStringTable;
    u32 guard = sCompassStateGuard;
    sCompassAnimationNames[4].mText.v = gabi::guest_addr("DayChange");
    sCompassName.mVtable = &sCompassStringTable;
    sCompassAnimationPanes[3].mText.v = gabi::guest_addr("G_Night_00");
    sCompassAnimationNames[5].mVtable = &sCompassStringTable;
    sCompassAnimationNames[5].mText.v = gabi::guest_addr("DayChange");
    sCompassAnimationNames[6].mVtable = &sCompassStringTable;
    sCompassPaneName.mVtable = &sCompassStringTable;
    sCompassPaneName.mText.v = gabi::guest_addr("P_Compass_00");
    sCompassName.mText.v = gabi::guest_addr("CompassClock_00");
    sCompassAnimationNames[6].mText.v = gabi::guest_addr("CompassOut");
    u32 number;
    if(guard) number = sCompassStateCounter; else { sCompassStateGuard = 1; number = 0; }
    sCompassWait.setup(++number, "::StateID_Wait", &sCompassStateTable, 0x19, &sCompassNullState);
    sCompassDisplay.setup(++number, "::StateID_Disp", &sCompassStateTable, 0x1c, &sCompassNullState);
    sCompassIn.setup(++number, "::StateID_CompassIn", &sCompassStateTable, 0x1f, &sCompassNullState);
    sCompassOut.setup(++number, "::StateID_CompassOut", &sCompassStateTable, 0x22, &sCompassNullState);
    sClockIn.setup(++number, "::StateID_ClockIn", &sCompassStateTable, 0x25, &sCompassNullState);
    sCompassStateCounter = number+1;
    sClockOut.setup(++number, "::StateID_ClockOut", &sCompassStateTable, 0x28, &sCompassNullState);
}
/* WWHD glue
data sCompassTable 100EE998 nocheck
data sCompassHolderTable 100EEAF0 nocheck
data sCompassSecondaryTable 100EEB00 nocheck
data sCompassWait 10491878 nocheck
data sCompassDisplay 104918A8 nocheck
data sCompassIn 104918D8 nocheck
data sCompassOut 10491908 nocheck
data sCompassPaneName 104917F4 nocheck
data sCompassAnimationNames 10491840 nocheck
data sCompassAnimationPanes 10491820 nocheck
data sCompassName 10491998 nocheck
data sCompassStringTable 100EE77C nocheck
data sCompassStateTable 100EE7F8 nocheck
data sCompassNullState 101FF32C nocheck
data sCompassStateGuard 101FD8E4 nocheck
data sCompassStateCounter 101FDD50 nocheck
data sCompassInitWords 10491810 nocheck
data sCompassRegistrations 101F58D0 nocheck
data sCompassPaneIndices 100EE794 nocheck
data sCompassActive 1047B093 nocheck
data sCompassModeForced 101EBFD8 nocheck
data sCompassReady 1049180C nocheck
data sCompassEnabled 1049180D nocheck
data sCompassPlayRate 100EE840 nocheck
data sCompassZero 100EE844 nocheck
data sCompassAngleScale 100EE850 nocheck
data sCompassAngleUnits 100EE854 nocheck
data sCompassDayStart 100EE858 nocheck
data sCompassDayEnd 100EE85C nocheck
data sCompassInitialFrame 100EE868 nocheck
data sCompassInitialRate 100EE86C nocheck
data sCompassFrame 10491804 nocheck
data sCompassRate 10491808 nocheck
data sCompassSignedBias 100EE848 nocheck
data sCompassUnsignedBias 100EE860 nocheck
data sCompassUi 1018C404 nocheck
data sCompassGame 101F84DC nocheck
data sClockIn 10491938 nocheck
data sClockOut 10491968 nocheck
data sClockPaneName 104917FC nocheck
ext 025E121C void* compassActorPredicate(void*,void*) callback
str 100EE870 "ClockIn" in 0264CF5C
str 100EE878 "CompassClock_00" in 0264CF5C
str 100EE888 "ClockChange" in 0264CF5C
str 100EE894 "::StateID_CompassIn" in 0264CF5C
str 100EE8A8 "G_CompassClock_00" in 0264CF5C
str 100EE8BC "CompassIn" in 0264CF5C
str 100EE8C8 "DayChange" in 0264CF5C
str 100EE8D4 "::StateID_ClockIn" in 0264CF5C
str 100EE8E8 "G_ClockChange_00" in 0264CF5C
str 100EE8FC "G_Day_00" in 0264CF5C
str 100EE908 "G_Night_00" in 0264CF5C
str 100EE914 "ClockOut" in 0264CF5C
str 100EE920 "CompassOut" in 0264CF5C
str 100EE92C "P_Compass_00" in 0264CF5C
str 100EE93C "P_Clock_00" in 0264CF5C
str 100EE948 "::StateID_Wait" in 0264CF5C
str 100EE958 "::StateID_Disp" in 0264CF5C
str 100EE968 "::StateID_CompassOut" in 0264CF5C
str 100EE980 "::StateID_ClockOut" in 0264CF5C
ext 02700ADC void compassConstructBase(NoControlPart_l*)
ext 02700A70 void compassDestroyBase(NoControlPart_l*, u32)
ext 02700B48 u32 compassSetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32)
ext 025D5218 hdCompassActor_c* compassFindActor(hdCompassActorJudge_t,be<u16>*)
ext 02560828 u32 compassGetPhase()
ext 025200D4 void* compassGameInfo()
ext 025E1988 void compassPlaySound(u32)
ext 02004E04 void compassBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void compassRegisterStatic(void*)
ext 028ED6F8 void compassConstructByte(void*)
ext 028EAB2C void compassConstructBool(void*)
*/
