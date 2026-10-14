/* HD baton-sign list: archive loading, learned-song visibility and screen state requests. */
#include "d/hd_screen_baton_sign_list_states.h"
#include "d/hd_layout_mgr.h"
struct hdBatonInAllGroups_c { hdUiSafeString mEntries[3]; };
struct hdBatonInAllAnimations_c { hdUiSafeString mEntries[7]; };
struct hdBatonInAllArchives_c { hdUiSafeString mEntries[2]; };
struct hdBatonInAllAnimationMap_c { be<u32> mGroups[7]; };
struct hdBatonInAllInput_l { u8 _00[0x1D0]; be<u32> mMode; };
struct hdBatonInAllCollect_l { u8 mOpaque; };
struct hdBatonInAllSave_l { u8 _00[0xD4]; hdBatonInAllCollect_l mCollection; };
WWHD_OFFSET(hdBatonInAllInput_l, mMode, 0x1D0);
WWHD_OFFSET(hdBatonInAllSave_l, mCollection, 0xD4);
using hdBatonInAllInputPointer = gptr<hdBatonInAllInput_l>;
using hdBatonInAllSavePointer = gptr<hdBatonInAllSave_l>;
using hdBatonInAllUiPointer = gptr<hdScreenUiSystem>;
static hdBatonInAllInputPointer sBatonInAllInput;
static hdBatonInAllSavePointer sBatonInAllSave;
static hdBatonInAllUiPointer sBatonInAllUiSystem;
static hdBatonInAllGroups_c sBatonInAllGroups;
static hdBatonInAllAnimations_c sBatonInAllAnimations;
static hdBatonInAllArchives_c sBatonInAllArchives;
static hdBatonInAllAnimationMap_c sBatonInAllAnimationMap;
static hdUiSafeString sBatonInAllScreenName;
static hdPartsStateId_c sBatonInAllDisplay;
static hdPartsStateId_c sBatonInAllPause;
static hdPartsStateId_c sBatonInAllInAll;
static hdPartsStateId_c sBatonInAllOutAll;
static hdPartsStateId_c sBatonInAllBaseBlank;
static void attachBatonInAllArchives(hdParts_c* self, hdUiSafeString* names, u32 count);
static void detachBatonInAllArchives(hdParts_c* self);
static void bindBatonInAllPanes(hdBatonSignList_c* self);
static void refreshBatonInAllPanes(hdBatonSignList_c* self);
static void runBatonInAllState(hdStateMachine* machine);
static u32 queryBatonInAllSong(hdBatonInAllCollect_l* collection, u32 song);
static void setBatonInAllVisible(hdBatonSignList_c* self, u32 enabled);
static void setBatonInAllInput(hdBatonSignList_c* self, u32 enabled);
static hdPartsStateId_c* currentBatonInAllState(hdStateMachine* machine);
static hdPartsLayout_c* allocateBatonInAllLayout(u32 size, hdScreenHeap* heap, u32 alignment);
static void bindBatonInAllAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group);

static hdUiStringTable sBatonInAllStringVtable;
static hdPartsStateIdVtbl_c sBatonInAllStateVtable;
struct hdBatonInAllHeaderWords_c { be<u32> mWords[4]; };
struct hdBatonInAllRegistration_c { u8 mStorage[12]; };
struct hdBatonInAllChildStrings_c { hdUiSafeString mEntries[26]; };
static hdBatonInAllHeaderWords_c sBatonInAllHeaderWords;
static hdBatonInAllRegistration_c sBatonInAllHeaderRegistration;
static hdBatonInAllRegistration_c sBatonInAllRotationRegistration;
static hdBatonInAllRegistration_c sBatonInAllPositiveRotationRegistration;
static f32 sBatonInAllMinimumRotation;
static f32 sBatonInAllMaximumRotation;
static u8 sBatonInAllRotationGuard;
static u8 sBatonInAllPositiveRotationGuard;
// Probably UI limits; this TU initializes the coordinate pairs but does not consume them.
static hdBatonListOffset_c sBatonInAllWideLimits;
static hdBatonListOffset_c sBatonInAllNarrowLimits;
static hdBatonInAllChildStrings_c sBatonInAllChildPaneNames;
static hdBatonInAllChildStrings_c sBatonInAllChildLayoutNames;
static hdPartsStateId_c sBatonInAllDisplayTouch;
static u32 sBatonInAllCounterReady;
static u32 sBatonInAllCounter;
static hdStateId sBatonInAllNullState;
static hdStateId sBatonInAllParentDisplay;
static void registerBatonInAllGlobal(hdBatonInAllRegistration_c* registration);
static void initializeBatonInAllRotation(be<u8>* guard);
static void initializePositiveBatonInAllRotation(be<u8>* guard);
static void initializeBatonInAllString(hdUiSafeString* string, const char* text) {
    string->mVtable = &sBatonInAllStringVtable;
    string->mText.v = gabi::guest_addr(text);
}

/* 02622418 */
u32 hdBatonSignList_c::initialize(u32 first, u32 second, void* descriptor) {
    attachBatonInAllArchives(this, sBatonInAllArchives.mEntries, 2);
    if (!vLoad(&sBatonInAllScreenName, mpResAccessor.v.get(), first, second, descriptor)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    bindBatonInAllPanes(this);
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sBatonInAllBaseBlank);
    return 1;
}
/* 0262254C */
void hdBatonSignList_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
    detachBatonInAllArchives(this);
    mEffectCallback.end();
}
/* 026225A8 */
void hdBatonSignList_c::updatePaneVisibility() {
    // The resource lists songs A/B/C/F/D/E in this order. Reload save data each time.
    const u32 songs[] = {0, 1, 2, 5, 3, 4};
    for (u32 i = 0; i < 6; ++i) {
        hdBatonInAllSave_l* save = hdPartsView(sBatonInAllSave.get());
        hdPartsPaneSRT_c* pane = hdPartsView(mPanes[i + 2].get());
        const u32 learned = queryBatonInAllSong(&save->mCollection, songs[i]);
        pane->mFlags = (u8(pane->mFlags) & ~1u) | u32(learned != 0);
    }
}
/* 026226EC */
void hdBatonSignList_c::executeState() {
    refreshBatonInAllPanes(this);
    runBatonInAllState(stateMachine());
}
/* 02622720 */
void hdBatonSignList_c::requestInAll() { changeState(&sBatonInAllInAll); }
/* 02622730 */
void hdBatonSignList_c::requestOutAll() { changeState(&sBatonInAllOutAll); }
/* 02622740 */
void hdBatonSignList_c::requestDisplay() { changeState(&sBatonInAllDisplay); }
/* 02622750 */
void hdBatonSignList_c::requestPause() { changeState(&sBatonInAllPause); }
/* 02622760 */
void hdBatonSignList_c::reset() {
    hdAnimSet_operationCC(animSet(), 0, 0);
    setBatonInAllVisible(this, 0);
    setBatonInAllInput(this, 0);
    vCalcLayout();
    changeState(&sBatonInAllBaseBlank);
}
/* 026227D8 */
u32 hdBatonSignList_c::isPauseFinished() {
    hdPartsStateId_c* current = currentBatonInAllState(stateMachine());
    hdPartsStateIdVtbl_c* targetVtable = sBatonInAllPause.mVtable;
    const u32 currentKey = current->getKey();
    const u32 targetKey = gabi::call_ptr<u32>(targetVtable->mGetKey.get(), &sBatonInAllPause);
    if (currentKey != targetKey) return 0;
    const u32 mode = hdPartsView(sBatonInAllInput.get())->mMode;
    if (mode == 0) return 1;
    if (mode == 1) return hdAnimSet_isEnd(animSet(), 5) != 0;
    if (mode == 2) {
        if (!hdAnimSet_isEnd(animSet(), 5)) return 0;
        return hdAnimSet_isEnd(animSet(), 2) != 0;
    }
    return 0;
}
/* 026228E8 */
u32 hdBatonSignList_c::createChildren() { return 1; }
/* 026228F0 */
u32 hdBatonSignList_c::createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = allocateBatonInAllLayout(sizeof(hdPartsLayout_c), sBatonInAllUiSystem->mHeap.get(), 4);
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL) return 0;
    if (!layout->vLoad(resource, first, second, third, 7, 3, descriptor)) return 0;
    for (u32 i = 0; i < 7; ++i) {
        const u32 group = sBatonInAllAnimationMap.mGroups[i];
        bindBatonInAllAnimation(animSet(), i, &sBatonInAllAnimations.mEntries[i], &sBatonInAllGroups.mEntries[group]);
    }
    vCalcLayout();
    return 1;
}

/* 02622A1C */
static void initializeBatonInAllStatics() {
    sBatonInAllHeaderWords.mWords[3] = 0;
    sBatonInAllHeaderWords.mWords[2] = 0;
    sBatonInAllHeaderWords.mWords[1] = 0;
    sBatonInAllHeaderWords.mWords[0] = 0;
    registerBatonInAllGlobal(&sBatonInAllHeaderRegistration);
    sBatonInAllMinimumRotation = -3.1415927410125732f;
    sBatonInAllMaximumRotation = 3.1415927410125732f;
    initializeBatonInAllRotation(&sBatonInAllRotationGuard);
    registerBatonInAllGlobal(&sBatonInAllRotationRegistration);
    initializePositiveBatonInAllRotation(&sBatonInAllPositiveRotationGuard);
    registerBatonInAllGlobal(&sBatonInAllPositiveRotationRegistration);
    sBatonInAllWideLimits.x = 50000.0f;
    sBatonInAllWideLimits.y = 50000.0f;
    sBatonInAllNarrowLimits.x = 10000.0f;
    sBatonInAllNarrowLimits.y = 10000.0f;
    initializeBatonInAllString(&sBatonInAllGroups.mEntries[0], "G_All_00");
    initializeBatonInAllString(&sBatonInAllGroups.mEntries[1], "G_SlideInput_00");
    initializeBatonInAllString(&sBatonInAllGroups.mEntries[2], "G_BatonSign_00");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[0], "InAll");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[1], "InSlideInput");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[2], "InBatonSign");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[3], "OnOffSlideInput");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[4], "OutBatonSign");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[5], "OutSlideInput");
    initializeBatonInAllString(&sBatonInAllAnimations.mEntries[6], "OutAll");
    initializeBatonInAllString(&sBatonInAllArchives.mEntries[0], "BatonSignList_00");
    initializeBatonInAllString(&sBatonInAllArchives.mEntries[1], "BatonSign_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[0], "L_BatonSignA_02");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[1], "L_BatonSignA_01");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[2], "L_BatonSignA_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[3], "L_BatonSignC_03");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[4], "L_BatonSignC_01");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[5], "L_BatonSignC_02");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[6], "L_BatonSignC_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[7], "L_BatonSignE_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[8], "L_BatonSignF_05");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[9], "L_BatonSignE_05");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[10], "L_BatonSignF_04");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[11], "L_BatonSignF_03");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[12], "L_BatonSignF_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[13], "L_BatonSignF_01");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[14], "L_BatonSignF_02");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[15], "L_BatonSignE_01");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[16], "L_BatonSignE_04");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[17], "L_BatonSignE_03");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[18], "L_BatonSignE_02");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[19], "L_BatonSignD_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[20], "L_BatonSignD_01");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[21], "L_BatonSignD_02");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[22], "L_BatonSignB_00");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[23], "L_BatonSignB_01");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[24], "L_BatonSignB_02");
    initializeBatonInAllString(&sBatonInAllChildPaneNames.mEntries[25], "L_BatonSignB_03");
    for (u32 i = 0; i < 26; ++i) initializeBatonInAllString(&sBatonInAllChildLayoutNames.mEntries[i], "BatonSign_00");
    initializeBatonInAllString(&sBatonInAllScreenName, "BatonSignList_00");
    u32 counter;
    if (sBatonInAllCounterReady) counter = sBatonInAllCounter;
    else { sBatonInAllCounterReady = 1; counter = 0; }
    hdPartsStateIdVtbl_c* stateVtable = &sBatonInAllStateVtable;
    sBatonInAllDisplay.setup(s32(++counter), "::StateID_Display", stateVtable, 7, &sBatonInAllParentDisplay);
    sBatonInAllDisplayTouch.setup(s32(++counter), "::StateID_DisplayTouch", stateVtable, 0x19, &sBatonInAllNullState);
    sBatonInAllPause.setup(s32(++counter), "::StateID_Pause", stateVtable, 0x1C, &sBatonInAllNullState);
    sBatonInAllInAll.setup(s32(++counter), "::StateID_InAll", stateVtable, 0x1F, &sBatonInAllNullState);
    sBatonInAllOutAll.setup(s32(++counter), "::StateID_OutAll", stateVtable, 0x22, &sBatonInAllNullState);
    sBatonInAllCounter = counter;
}

/* WWHD glue
data sBatonInAllHeaderWords 1048E7F0
data sBatonInAllHeaderRegistration 101F5254
data sBatonInAllRotationRegistration 101F5260
data sBatonInAllPositiveRotationRegistration 101F526C
data sBatonInAllMinimumRotation 1048E7D4
data sBatonInAllMaximumRotation 1048E7D8
data sBatonInAllRotationGuard 1048E7EC
data sBatonInAllPositiveRotationGuard 1048E7ED
data sBatonInAllWideLimits 1048E7DC
data sBatonInAllNarrowLimits 1048E7E4
data sBatonInAllChildPaneNames 1048E8A8
data sBatonInAllChildLayoutNames 1048E978
data sBatonInAllDisplayTouch 1048EA7C
data sBatonInAllCounterReady 101FD8E4
data sBatonInAllCounter 101FDD50
data sBatonInAllNullState 101FF32C
data sBatonInAllParentDisplay 1049DD90
ext 028F026C void registerBatonInAllGlobal(hdBatonInAllRegistration_c* registration)
ext 028ED6F8 void initializeBatonInAllRotation(be<u8>* guard)
ext 028EAB2C void initializePositiveBatonInAllRotation(be<u8>* guard)
str 100E44A4 "G_All_00" in 02622A1C
str 100E4294 "G_SlideInput_00" in 02622A1C
str 100E44B0 "G_BatonSign_00" in 02622A1C
str 100E4284 "InAll" in 02622A1C
str 100E44C0 "InSlideInput" in 02622A1C
str 100E4474 "InBatonSign" in 02622A1C
str 100E42A4 "OnOffSlideInput" in 02622A1C
str 100E44D0 "OutBatonSign" in 02622A1C
str 100E4480 "OutSlideInput" in 02622A1C
str 100E428C "OutAll" in 02622A1C
str 100E44F0 "BatonSignList_00" in 02622A1C
str 100E44E0 "BatonSign_00" in 02622A1C
str 100E42B4 "L_BatonSignA_02" in 02622A1C
str 100E42C4 "L_BatonSignA_01" in 02622A1C
str 100E42D4 "L_BatonSignA_00" in 02622A1C
str 100E42E4 "L_BatonSignC_03" in 02622A1C
str 100E42F4 "L_BatonSignC_01" in 02622A1C
str 100E4304 "L_BatonSignC_02" in 02622A1C
str 100E4314 "L_BatonSignC_00" in 02622A1C
str 100E4324 "L_BatonSignE_00" in 02622A1C
str 100E4334 "L_BatonSignF_05" in 02622A1C
str 100E4344 "L_BatonSignE_05" in 02622A1C
str 100E4354 "L_BatonSignF_04" in 02622A1C
str 100E4364 "L_BatonSignF_03" in 02622A1C
str 100E4374 "L_BatonSignF_00" in 02622A1C
str 100E4384 "L_BatonSignF_01" in 02622A1C
str 100E4394 "L_BatonSignF_02" in 02622A1C
str 100E43A4 "L_BatonSignE_01" in 02622A1C
str 100E43B4 "L_BatonSignE_04" in 02622A1C
str 100E43C4 "L_BatonSignE_03" in 02622A1C
str 100E43D4 "L_BatonSignE_02" in 02622A1C
str 100E43E4 "L_BatonSignD_00" in 02622A1C
str 100E43F4 "L_BatonSignD_01" in 02622A1C
str 100E4404 "L_BatonSignD_02" in 02622A1C
str 100E4414 "L_BatonSignB_00" in 02622A1C
str 100E4424 "L_BatonSignB_01" in 02622A1C
str 100E4434 "L_BatonSignB_02" in 02622A1C
str 100E4444 "L_BatonSignB_03" in 02622A1C
str 100E4490 "::StateID_Display" in 02622A1C
str 100E4504 "::StateID_DisplayTouch" in 02622A1C
str 100E4454 "::StateID_Pause" in 02622A1C
str 100E4464 "::StateID_InAll" in 02622A1C
str 100E451C "::StateID_OutAll" in 02622A1C
data sBatonInAllInput 101F5088
data sBatonInAllSave 101F84DC
data sBatonInAllUiSystem 1018C404
data sBatonInAllGroups 1048E800
data sBatonInAllAnimations 1048E818
data sBatonInAllArchives 1048E850
data sBatonInAllAnimationMap 100E4124
data sBatonInAllScreenName 1048EB3C
data sBatonInAllDisplay 1048EA4C
data sBatonInAllPause 1048EAAC
data sBatonInAllInAll 1048EADC
data sBatonInAllOutAll 1048EB0C
data sBatonInAllBaseBlank 1049DD60
ext 026F90D8 void attachBatonInAllArchives(hdParts_c* self, hdUiSafeString* names, u32 count)
ext 026F91F8 void detachBatonInAllArchives(hdParts_c* self)
ext 02622184 void bindBatonInAllPanes(hdBatonSignList_c* self)
ext 026225A8 void refreshBatonInAllPanes(hdBatonSignList_c* self)
ext 02006364 void runBatonInAllState(hdStateMachine* machine)
ext 025B7B10 u32 queryBatonInAllSong(hdBatonInAllCollect_l* collection, u32 song)
ext 02621C58 void setBatonInAllVisible(hdBatonSignList_c* self, u32 enabled)
ext 02621C2C void setBatonInAllInput(hdBatonSignList_c* self, u32 enabled)
ext 02006478 hdPartsStateId_c* currentBatonInAllState(hdStateMachine* machine)
ext 0273B050 hdPartsLayout_c* allocateBatonInAllLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 02004E04 void bindBatonInAllAnimation(hdAnimSet* animations, u32 slot, hdUiSafeString* name, hdUiSafeString* group)
data sBatonInAllStringVtable 100E410C
data sBatonInAllStateVtable 100E4188
*/
