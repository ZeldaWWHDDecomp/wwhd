/* HD result screen: elapsed-time and rupee text, animation setup and visibility. */
#include "d/hd_screen_boat_race_result_parts.h"
static u32 raceResultSScreenVtable;
static u32 raceResultSDelegateVtable;
static u32 raceResultSSecondaryVtable;
static hdPartsStateId_c raceResultSIdleState;
static hdPartsStateId_c raceResultSWaitState;
static hdPartsStateId_c raceResultSRollState;
static hdUiSafeString raceResultSArchiveName;
static hdUiSafeString raceResultSLayoutName;
static hdUiSafeString raceResultSTimePaneName;
static hdUiSafeString raceResultSRupeePaneName;
struct sGroupNamesTable { hdUiSafeString mNames[3]; };
static sGroupNamesTable raceResultSGroupNames;
struct sAnimationNamesTable { hdUiSafeString mNames[3]; };
static sAnimationNamesTable raceResultSAnimationNames;
static hdUiSafeString raceResultSUnusedPaneName;
static hdUiStringTable raceResultSStringVtable;
static hdPartsStateIdVtbl_c raceResultSStateVtable;
static hdStateId raceResultSNullState;
static u32 raceResultSStateGuard;
static u32 raceResultSNextState;
static hdBoatRaceWords raceResultSWords;
static hdBoatRaceRange raceResultSRange;
static f32 raceResultSLower;
static f32 raceResultSUpper;
static f32 raceResultSOne;
static f32 raceResultSTwo;
static f32 raceResultSZero;
static u8 raceResultSFirstObject;
static u8 raceResultSSecondObject;
static hdBoatRaceRegistration raceResultSFirstRegistration;
static hdBoatRaceRegistration raceResultSSecondRegistration;
static hdBoatRaceRegistration raceResultSThirdRegistration;
struct sUiPointer { gptr<hdScreenUiSystem> mInstance; };
static sUiPointer raceResultSUi;
struct hdBoatRaceGroupMapping { be<u32> mGroups[3]; };
static hdBoatRaceGroupMapping raceResultSGroupMapping;
struct hdBoatRacePlayView {
    u8 mPadding[0x5CEC]; be<u16> mRupees;
    u8 mPaddingToStart[6]; be<u32> mStartFrame, mFinishFrame;
};
WWHD_OFFSET(hdBoatRacePlayView, mStartFrame, 0x5CF4);
WWHD_OFFSET(hdBoatRacePlayView, mFinishFrame, 0x5CF8);
struct hdBoatRaceTextBuffer9 { be<u16> mText[9]; };
struct hdBoatRaceTextBuffer5 { be<u16> mText[5]; };
// HD: the buffer immediately follows its 12-byte string descriptor.
struct hdBoatRaceTimeText { PaneString_l mString; hdBoatRaceTextBuffer9 mBuffer; };
struct hdBoatRaceRupeeText { PaneString_l mString; hdBoatRaceTextBuffer5 mBuffer; };
WWHD_OFFSET(hdBoatRaceTimeText, mBuffer, 12);
WWHD_OFFSET(hdBoatRaceRupeeText, mBuffer, 12);
static u32 raceResultSTimeStringVtable;
static u32 raceResultSRupeeStringVtable;
hdBoatRacePlayView* raceResultBoatRaceGetPlay();
void raceResultBoatRaceFormatTime(PaneString_l*, const char*, s32 minutes, s32 seconds, s32 centiseconds);
void raceResultBoatRaceFormatRupees(PaneString_l*, const char*, u32 value);
void raceResultBoatRaceSetText(hdTextPane_c*, PaneString_l*, u32);
void raceResultBoatRaceBaseConstruct(hdParts_c*);
void raceResultBoatRaceBaseDestroy(hdParts_c*, u32);
void raceResultBoatRaceBaseRelease(hdParts_c*);
void raceResultBoatRaceSetArchives(hdParts_c*, hdUiSafeString*, u32);
void* raceResultBoatRaceAllocate(u32);
void raceResultBoatRaceDelete(void*);
void raceResultBoatRaceBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void raceResultBoatRaceRegister(hdBoatRaceRegistration*);
void raceResultBoatRaceConstructFirst(be<u8>*);
void raceResultBoatRaceConstructSecond(be<u8>*);
/* 026275A0 */
u32 boatRaceInStateCheck() { return 0; }
/* 026275AC */
u32 boatRaceWaitStateCheck() { return 0; }
/* 026275B4 */
u32 boatRaceOutStateCheck() { return 0; }
/* 026275C0 */
u32 boatRaceSleepStateCheck() { return 0; }
/* 026275C8 */
u32 boatRaceResultStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 0262769C */
void hdBoatRaceResultView::destroy(hdBoatRaceResultView* screen, u32 flags) {
    if (!screen) return;
    screen->parts()->mStateHolder.mHolderVtable = &raceResultSDelegateVtable;
    screen->parts()->mSecondaryVtable = &raceResultSSecondaryVtable;
    raceResultBoatRaceBaseDestroy(screen->parts(), 0);
    if (flags & 1) raceResultBoatRaceDelete(screen);
}
/* 02627708 */
void hdBoatRaceResultView::enterWait() { mWaitTimer = 90; }
/* 02627734 */
void hdBoatRaceResultView::updateText() {
    if (timePane()) {
        u32 finish = raceResultBoatRaceGetPlay()->mFinishFrame;
        u32 start = raceResultBoatRaceGetPlay()->mStartFrame;
        // HD: the original multiply wraps before the signed division by 30.
        s32 milliseconds = s32((finish - start) * 1000u) / 30;
        if (milliseconds < 0) milliseconds = 0;
        s32 seconds = milliseconds / 1000;
        s32 minutes = seconds / 60;
        s32 centiseconds = (milliseconds % 1000) / 10;
        seconds -= minutes * 60;
        hdBoatRaceTimeText textStorage;
        textStorage.mBuffer.mText[0] = 0;
        textStorage.mBuffer.mText[8] = 0;
        textStorage.mString.mCapacity = 9;
        textStorage.mString.mpBuffer = &textStorage.mBuffer;
        textStorage.mString.mpVTable = reinterpret_cast<gfn*>(&raceResultSTimeStringVtable);
        raceResultBoatRaceFormatTime(&textStorage.mString, "%02d:%02d.%02d", minutes, seconds, centiseconds);
        raceResultBoatRaceSetText(timePane(), &textStorage.mString, 0);
    }
    if (rupeePane()) {
        u32 rupees = raceResultBoatRaceGetPlay()->mRupees;
        hdBoatRaceRupeeText textStorage;
        textStorage.mBuffer.mText[0] = 0;
        textStorage.mBuffer.mText[4] = 0;
        textStorage.mString.mpBuffer = &textStorage.mBuffer;
        textStorage.mString.mCapacity = 5;
        textStorage.mString.mpVTable = reinterpret_cast<gfn*>(&raceResultSRupeeStringVtable);
        f32 frame = raceResultSTwo;
        if (rupees < 10) frame = raceResultSZero;
        else if (rupees < 100) frame = raceResultSOne;
        else if (rupees >= 1000) rupees = 999;
        raceResultBoatRaceFormatRupees(&textStorage.mString, "%d", rupees);
        hdAnimSet_setFrame(animSet(), 2, 0, frame);
        raceResultBoatRaceSetText(rupeePane(), &textStorage.mString, 0);
    }
}
/* 0262799C */
void hdBoatRaceResultView::enterInAll() {
    f32 one = raceResultSOne;
    hdAnimSet_play(animSet(), 1, 2, one);
    hdAnimSet* animation = animSet();
    u32 result = mResult;
    if (result == 2) hdAnimSet_play(animation, 0, 1, one);
    else hdAnimSet_operationCC(animation, 0, 1);
    updateText();
    vCalcLayout();
    mVisible = 1;
    mRollFrame = 0;
}
/* 02627B08 */
hdBoatRaceResultView* hdBoatRaceResultView::construct(hdBoatRaceResultView* screen) {
    if (!screen) {
        screen = static_cast<hdBoatRaceResultView*>(raceResultBoatRaceAllocate(sizeof(hdBoatRaceResultView)));
        if (!screen) return NULL;
    }
    raceResultBoatRaceBaseConstruct(screen->parts());
    screen->parts()->mStateHolder.mHolderVtable = &raceResultSDelegateVtable;
    screen->parts()->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&raceResultSScreenVtable);
    screen->parts()->mSecondaryVtable = &raceResultSSecondaryVtable;
    screen->mVisible = 0;
    screen->setTimePane(NULL);
    screen->mRollFrame = 0;
    screen->setRupeePane(NULL);
    screen->mWaitTimer = 0;
    screen->mResult = 0;
    return screen;
}
struct hdBoatRacePaneLookup {
    u8 mPadding[8]; gptr<gfn> mVtable;
};
struct hdBoatRacePaneAccessor { u8 mPadding[0xC]; gptr<hdBoatRacePaneLookup> mpLookup; };
/* 02627B90 */
u32 hdBoatRaceResultView::setup(u32 first, u32 second, void* descriptor) {
    raceResultBoatRaceSetArchives(parts(), &raceResultSArchiveName, 1);
    if (!vLoad(&raceResultSLayoutName, gabi::ea(parts()->mpResAccessor.get()), first, second, descriptor)) return 0;
    hdBoatRacePaneAccessor* accessor = reinterpret_cast<hdBoatRacePaneAccessor*>(parts()->mpLayout->mAccessor.get());
    hdBoatRacePaneLookup* lookup = accessor->mpLookup;
    gfn* table = lookup->mVtable;
    raceResultSTimePaneName.assureTermination();
    setTimePane(gabi::call_ptr<hdTextPane_c*>(table[0x5C / 4].get(), lookup, raceResultSTimePaneName.mText.get(), 1));
    table = lookup->mVtable;
    raceResultSRupeePaneName.assureTermination();
    setRupeePane(gabi::call_ptr<hdTextPane_c*>(table[0x5C / 4].get(), lookup, raceResultSRupeePaneName.mText.get(), 1));
    hdAnimSet_operationCC(animSet(), 1, 2);
    hdAnimSet_operationCC(animSet(), 0, 1);
    startState(&raceResultSIdleState);
    vCalcLayout();
    return 1;
}
/* 02627D38 */
void hdBoatRaceResultView::release() { vDestroyLayout(); raceResultBoatRaceBaseRelease(parts()); }
/* 02627D78 */
void hdBoatRaceResultView::execute() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    auto keyFunction = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* idleVtable = raceResultSIdleState.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(keyFunction, current);
    u32 idleKey = gabi::call_ptr<u32>(idleVtable->mGetKey.get(), &raceResultSIdleState);
    if (currentKey != idleKey) hdParts_executeStateMachine(stateMachine());
}
/* 02627DF8 */
void hdBoatRaceResultView::updateLayout() {
    if (mVisible) gabi::call_ptr(parts()->mVtable->mUpdateLayout.get(), this);
}
/* 02627E14 */
void hdBoatRaceResultView::show(u32 result) { mResult = result; changeState(&raceResultSWaitState); }
/* 02627E28 */
u32 hdBoatRaceResultView::load(void* name, u32 archive, u32 first, u32 second, void* descriptor) {
    hdScreenHeap* heap = raceResultSUi.mInstance->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(0x110, heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    parts()->mpLayout = layout;
    if (!layout || !layout->vLoad(name, archive, first, second, 3, 3, descriptor)) return 0;
    for (u32 index = 0; index < 3; ++index) {
        hdAnimSet* animation = animSet();
        u32 group = raceResultSGroupMapping.mGroups[index];
        hdUiSafeString* groupName = hdPartsView<hdUiSafeString>(gabi::ea(&raceResultSGroupNames) + group * 8);
        raceResultBoatRaceBindAnimation(animation, index, &raceResultSAnimationNames.mNames[index], groupName);
    }
    vCalcLayout();
    return 1;
}
/* 02627F54 */
void boatRaceResultStaticInit() {
    raceResultSWords.mWords[3] = 0;
    raceResultSWords.mWords[2] = 0;
    raceResultSWords.mWords[1] = 0;
    raceResultSWords.mWords[0] = 0;
    raceResultBoatRaceRegister(&raceResultSFirstRegistration);
    f32 lower = raceResultSLower, upper = raceResultSUpper;
    raceResultSRange.mLower = lower;
    raceResultSRange.mUpper = upper;
    raceResultBoatRaceConstructFirst(&raceResultSFirstObject);
    raceResultBoatRaceRegister(&raceResultSSecondRegistration);
    raceResultBoatRaceConstructSecond(&raceResultSSecondObject);
    raceResultBoatRaceRegister(&raceResultSThirdRegistration);
    raceResultSGroupNames.mNames[0].mVtable = &raceResultSStringVtable;
    raceResultSGroupNames.mNames[0].mText.v = gabi::guest_addr("G_Rupy_00");
    raceResultSGroupNames.mNames[1].mVtable = &raceResultSStringVtable;
    raceResultSArchiveName.mVtable = &raceResultSStringVtable;
    raceResultSGroupNames.mNames[1].mText.v = gabi::guest_addr("G_Perfect_00");
    raceResultSAnimationNames.mNames[0].mVtable = &raceResultSStringVtable;
    raceResultSAnimationNames.mNames[0].mText.v = gabi::guest_addr("InAll");
    raceResultSRupeePaneName.mVtable = &raceResultSStringVtable;
    raceResultSRupeePaneName.mText.v = gabi::guest_addr("T_NumRupy_00");
    raceResultSAnimationNames.mNames[1].mVtable = &raceResultSStringVtable;
    raceResultSAnimationNames.mNames[1].mText.v = gabi::guest_addr("InAll");
    u32 guard = raceResultSStateGuard;
    raceResultSTimePaneName.mVtable = &raceResultSStringVtable;
    raceResultSGroupNames.mNames[2].mVtable = &raceResultSStringVtable;
    raceResultSGroupNames.mNames[2].mText.v = gabi::guest_addr("G_GetRupy_00");
    raceResultSTimePaneName.mText.v = gabi::guest_addr("T_NumTime_00");
    raceResultSArchiveName.mText.v = gabi::guest_addr("BoatRaceResult_00");
    raceResultSUnusedPaneName.mText.v = gabi::guest_addr("T_NumTime_00");
    raceResultSAnimationNames.mNames[2].mVtable = &raceResultSStringVtable;
    raceResultSAnimationNames.mNames[2].mText.v = gabi::guest_addr("Rupy");
    raceResultSUnusedPaneName.mVtable = &raceResultSStringVtable;
    raceResultSLayoutName.mVtable = &raceResultSStringVtable;
    raceResultSLayoutName.mText.v = gabi::guest_addr("BoatRaceResult_00");
    u32 id;
    if (guard) id = raceResultSNextState;
    else { raceResultSStateGuard = 1; id = 0; }
    raceResultSWaitState.setup(id + 1, "::StateID_Wait", &raceResultSStateVtable, 0x19, &raceResultSNullState);
    raceResultSRollState.setup(id + 2, "::StateID_InAll", &raceResultSStateVtable, 0x1C, &raceResultSNullState);
    raceResultSNextState = id + 2;
}
/* WWHD glue
str 100E59E4 "G_Rupy_00" in 02627F54
str 100E5A04 "G_Perfect_00" in 02627F54
str 100E59C4 "InAll" in 02627F54
str 100E59CC "Rupy" in 02627F54
str 100E5A34 "T_NumRupy_00" in 02627F54
str 100E5A14 "G_GetRupy_00" in 02627F54
str 100E5A24 "T_NumTime_00" in 02627F54
str 100E59F0 "BoatRaceResult_00" in 02627F54
str 100E5A44 "::StateID_Wait" in 02627F54
str 100E59D4 "::StateID_InAll" in 02627F54
data raceResultSScreenVtable 100E5A58
data raceResultSDelegateVtable 100E5B50
data raceResultSSecondaryVtable 100E5B60
data raceResultSIdleState 1049DD60
data raceResultSWaitState 1048F1E0
data raceResultSRollState 1048F210
data raceResultSArchiveName 1048F174
data raceResultSTimePaneName 1048F17C
data raceResultSUnusedPaneName 1048F184
data raceResultSRupeePaneName 1048F18C
data raceResultSGroupNames 1048F1B0
data raceResultSAnimationNames 1048F1C8
data raceResultSLayoutName 1048F240
data raceResultSStringVtable 100E588C
data raceResultSStateVtable 100E5940
data raceResultSNullState 101FF32C
data raceResultSStateGuard 101FD8E4
data raceResultSNextState 101FDD50
data raceResultSWords 1048F1A0
data raceResultSRange 1048F194
data raceResultSLower 100E59BC
data raceResultSUpper 100E59C0
data raceResultSOne 100E5988
data raceResultSTwo 100E598C
data raceResultSZero 100E5990
data raceResultSFirstObject 1048F19C
data raceResultSSecondObject 1048F19D
data raceResultSFirstRegistration 101F5380
data raceResultSSecondRegistration 101F538C
data raceResultSThirdRegistration 101F5398
data raceResultSUi 1018C404
data raceResultSGroupMapping 100E58EC
data raceResultSTimeStringVtable 100E58BC
data raceResultSRupeeStringVtable 100E58D4
str 100E599C "%02d:%02d.%02d" renamed
str 100E5994 "%d" renamed
ext 025200D4 hdBoatRacePlayView* raceResultBoatRaceGetPlay()
ext 02759D3C void raceResultBoatRaceFormatTime(PaneString_l*,const char*,s32,s32,s32)
ext 02759D3C void raceResultBoatRaceFormatRupees(PaneString_l*,const char*,u32)
ext 027046D4 void raceResultBoatRaceSetText(hdTextPane_c*,PaneString_l*,u32)
ext 026F89E8 void raceResultBoatRaceBaseConstruct(hdParts_c*)
ext 026F88F0 void raceResultBoatRaceBaseDestroy(hdParts_c*,u32)
ext 026F91F8 void raceResultBoatRaceBaseRelease(hdParts_c*)
ext 026F90D8 void raceResultBoatRaceSetArchives(hdParts_c*,hdUiSafeString*,u32)
ext 0273AD10 void* raceResultBoatRaceAllocate(u32)
ext 0273AF40 void raceResultBoatRaceDelete(void*)
ext 02004E04 void raceResultBoatRaceBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void raceResultBoatRaceRegister(hdBoatRaceRegistration*)
ext 028ED6F8 void raceResultBoatRaceConstructFirst(be<u8>*)
ext 028EAB2C void raceResultBoatRaceConstructSecond(be<u8>*)
*/
