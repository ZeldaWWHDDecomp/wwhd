/* HD boat-race start: countdown animation, tick sounds and start flag. */
#include "d/hd_screen_boat_race_start.h"
static u32 raceStartSScreenVtable;
static u32 raceStartSDelegateVtable;
static u32 raceStartSSecondaryVtable;
static hdPartsStateId_c raceStartSIdleState;
static hdPartsStateId_c raceStartSAllState;
static hdUiSafeString raceStartSGroupName;
static hdUiSafeString raceStartSAnimationName;
static hdUiSafeString raceStartSArchiveName;
static hdUiSafeString raceStartSLayoutName;
static hdUiStringTable raceStartSStringVtable;
static hdPartsStateIdVtbl_c raceStartSStateVtable;
static hdStateId raceStartSNullState;
static u32 raceStartSStateGuard;
static u32 raceStartSNextState;
static u32 raceStartSGroupIndex;
struct sUiPointer { gptr<hdScreenUiSystem> mInstance; };
static sUiPointer raceStartSUi;
static f32 raceStartSOne;
static f32 raceStartSSecondTick;
static f32 raceStartSThirdTick;
static f32 raceStartSStartFrame;
static f32 raceStartSLower;
static f32 raceStartSUpper;
static hdBoatRaceWords raceStartSWords;
static hdBoatRaceRange raceStartSRange;
static u8 raceStartSFirstObject;
static u8 raceStartSSecondObject;
static hdBoatRaceRegistration raceStartSFirstRegistration;
static hdBoatRaceRegistration raceStartSSecondRegistration;
static hdBoatRaceRegistration raceStartSThirdRegistration;
void raceStartBoatRaceBaseConstruct(hdParts_c*);
void raceStartBoatRaceBaseDestroy(hdParts_c*, u32);
void raceStartBoatRaceBaseRelease(hdParts_c*);
void raceStartBoatRaceSetArchives(hdParts_c*, hdUiSafeString*, u32);
void* raceStartBoatRaceAllocate(u32);
void raceStartBoatRaceDelete(void*);
void raceStartBoatRacePlaySound(u32);
void raceStartBoatRaceBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void raceStartBoatRaceRegister(hdBoatRaceRegistration*);
void raceStartBoatRaceConstructFirst(be<u8>*);
void raceStartBoatRaceConstructSecond(be<u8>*);
/* 026282D0 */
u32 boatRaceStartStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 026283D0 */
void hdScreenBoatRaceStart_c::destroy(hdScreenBoatRaceStart_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &raceStartSDelegateVtable;
    screen->mSecondaryVtable = &raceStartSSecondaryVtable;
    raceStartBoatRaceBaseDestroy(screen, 0);
    if (flags & 1) raceStartBoatRaceDelete(screen);
}
/* 0262843C */
void hdScreenBoatRaceStart_c::enterAll() {
    hdAnimSet_play(animSet(), 0, 0, raceStartSOne);
    mStarted = 0;
    mVisible = 1;
}
/* 02628490 */
void hdScreenBoatRaceStart_c::executeAll() {
    hdPartsLayout_c* layout = mpLayout;
    hdAnimSet* animation = layout->animSet();
    // HD: the first animation's controller owns the countdown frame.
    hdAnimSetEntry* entry = animation->mActiveSlots.mData[0];
    f32 frame = entry->mController->mSpeed;
    if (frame == raceStartSOne || frame == raceStartSSecondTick || frame == raceStartSThirdTick) {
        raceStartBoatRacePlaySound(0x843);
        layout = mpLayout;
    } else if (frame == raceStartSStartFrame) {
        raceStartBoatRacePlaySound(0x844);
        layout = mpLayout;
        mStarted = 1;
    }
    if (hdAnimSet_isEnd(layout->animSet(), 0)) changeState(&raceStartSIdleState);
    vCalcLayout();
}
/* 0262856C */
void hdScreenBoatRaceStart_c::leaveAll() { mVisible = 0; }
/* 02628578 */
hdScreenBoatRaceStart_c* hdScreenBoatRaceStart_c::construct(hdScreenBoatRaceStart_c* screen) {
    if (!screen) {
        screen = static_cast<hdScreenBoatRaceStart_c*>(raceStartBoatRaceAllocate(sizeof(hdScreenBoatRaceStart_c)));
        if (!screen) return NULL;
    }
    raceStartBoatRaceBaseConstruct(screen);
    screen->mStateHolder.mHolderVtable = &raceStartSDelegateVtable;
    screen->mVtable = reinterpret_cast<hdPartsVtbl_c*>(&raceStartSScreenVtable);
    screen->mSecondaryVtable = &raceStartSSecondaryVtable;
    screen->mStarted = 0;
    screen->mVisible = 0;
    return screen;
}
/* 026285F0 */
u32 hdScreenBoatRaceStart_c::setup(u32 first, u32 second, void* descriptor) {
    raceStartBoatRaceSetArchives(this, &raceStartSArchiveName, 1);
    if (!vLoad(&raceStartSLayoutName, gabi::ea(mpResAccessor.get()), first, second, descriptor)) return 0;
    startState(&raceStartSIdleState);
    return 1;
}
/* 026286D8 */
void hdScreenBoatRaceStart_c::release() {
    vDestroyLayout();
    raceStartBoatRaceBaseRelease(this);
}
/* 02628718 */
void hdScreenBoatRaceStart_c::execute() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    auto keyFunction = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* idleVtable = raceStartSIdleState.mVtable;
    u32 currentKey = gabi::call_ptr<u32>(keyFunction, current);
    u32 idleKey = gabi::call_ptr<u32>(idleVtable->mGetKey.get(), &raceStartSIdleState);
    if (currentKey != idleKey) hdParts_executeStateMachine(stateMachine());
}
/* 02628798 */
void hdScreenBoatRaceStart_c::draw(void* info) {
    if (mVisible) vDrawLayout(info);
}
/* 026287B4 */
u32 hdScreenBoatRaceStart_c::load(void* name, u32 archive, u32 first, u32 second, void* descriptor) {
    hdScreenHeap* heap = raceStartSUi.mInstance->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(0x110, heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, archive, first, second, 1, 1, descriptor)) return 0;
    u32 group = raceStartSGroupIndex;
    hdUiSafeString* groupName = hdPartsView<hdUiSafeString>(gabi::ea(&raceStartSGroupName) + group * 8);
    raceStartBoatRaceBindAnimation(animSet(), 0, &raceStartSAnimationName, groupName);
    vCalcLayout();
    return 1;
}
/* 026288B4 */
void boatRaceStartStaticInit() {
    raceStartSWords.mWords[2] = 0;
    raceStartSWords.mWords[0] = 0;
    raceStartSWords.mWords[3] = 0;
    raceStartSWords.mWords[1] = 0;
    raceStartBoatRaceRegister(&raceStartSFirstRegistration);
    f32 lower = raceStartSLower, upper = raceStartSUpper;
    raceStartSRange.mLower = lower;
    raceStartSRange.mUpper = upper;
    raceStartBoatRaceConstructFirst(&raceStartSFirstObject);
    raceStartBoatRaceRegister(&raceStartSSecondRegistration);
    raceStartBoatRaceConstructSecond(&raceStartSSecondObject);
    raceStartBoatRaceRegister(&raceStartSThirdRegistration);
    raceStartSGroupName.mVtable = &raceStartSStringVtable;
    raceStartSArchiveName.mVtable = &raceStartSStringVtable;
    raceStartSAnimationName.mVtable = &raceStartSStringVtable;
    raceStartSAnimationName.mText.v = gabi::guest_addr("All");
    raceStartSGroupName.mText.v = gabi::guest_addr("G_All_00");
    raceStartSLayoutName.mVtable = &raceStartSStringVtable;
    u32 guard = raceStartSStateGuard;
    raceStartSArchiveName.mText.v = gabi::guest_addr("BoatRaceStart_00");
    raceStartSLayoutName.mText.v = gabi::guest_addr("BoatRaceStart_00");
    u32 id;
    if (guard) id = raceStartSNextState + 1;
    else { raceStartSStateGuard = 1; id = 1; }
    raceStartSAllState.setup(id, "::StateID_All", &raceStartSStateVtable, 0x19, &raceStartSNullState);
    raceStartSNextState = id;
}
/* WWHD glue
data raceStartSScreenVtable 100E5C70
data raceStartSDelegateVtable 100E5D50
data raceStartSSecondaryVtable 100E5D60
data raceStartSIdleState 1049DD60
data raceStartSAllState 1048F27C
data raceStartSGroupName 1048F248
data raceStartSAnimationName 1048F250
data raceStartSArchiveName 1048F258
data raceStartSLayoutName 1048F2AC
data raceStartSStringVtable 100E5B78
data raceStartSStateVtable 100E5BD8
data raceStartSNullState 101FF32C
data raceStartSStateGuard 101FD8E4
data raceStartSNextState 101FDD50
data raceStartSGroupIndex 100E5B74
data raceStartSUi 1018C404
data raceStartSOne 100E5C20
data raceStartSSecondTick 100E5C24
data raceStartSThirdTick 100E5C28
data raceStartSStartFrame 100E5C2C
data raceStartSLower 100E5C30
data raceStartSUpper 100E5C34
data raceStartSWords 1048F26C
data raceStartSRange 1048F260
data raceStartSFirstObject 1048F268
data raceStartSSecondObject 1048F269
data raceStartSFirstRegistration 101F53A4
data raceStartSSecondRegistration 101F53B0
data raceStartSThirdRegistration 101F53BC
str 100E5C38 "All" in 026288B4
str 100E5C4C "G_All_00" in 026288B4
str 100E5C58 "BoatRaceStart_00" in 026288B4
str 100E5C3C "::StateID_All" in 026288B4
ext 026F89E8 void raceStartBoatRaceBaseConstruct(hdParts_c*)
ext 026F88F0 void raceStartBoatRaceBaseDestroy(hdParts_c*,u32)
ext 026F91F8 void raceStartBoatRaceBaseRelease(hdParts_c*)
ext 026F90D8 void raceStartBoatRaceSetArchives(hdParts_c*,hdUiSafeString*,u32)
ext 0273AD10 void* raceStartBoatRaceAllocate(u32)
ext 0273AF40 void raceStartBoatRaceDelete(void*)
ext 025E1988 void raceStartBoatRacePlaySound(u32)
ext 02004E04 void raceStartBoatRaceBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void raceStartBoatRaceRegister(hdBoatRaceRegistration*)
ext 028ED6F8 void raceStartBoatRaceConstructFirst(be<u8>*)
ext 028EAB2C void raceStartBoatRaceConstructSecond(be<u8>*)
*/
