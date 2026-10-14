/* HD-only fade wipe animations and requests. */
#include "d/hd_screen_fade_wipe.h"
static hdPartsVtbl_c sFadeTable;
static u32 sFadeHolderTable;
static u32 sFadeSecondaryTable;
static hdPartsStateId_c sFadeIn;
static hdPartsStateId_c sFadeOut;
static hdPartsStateId_c sFadeBlank;
static hdPartsStateId_c sFadeDisplay;
static hdUiSafeString sFadeArchive;
static hdUiSafeString sFadeName;
static hdUiSafeString sFadeAnimationNames[2] = {};
static hdUiSafeString sFadeAnimationPane[1] = {};
static hdUiStringTable sFadeStringTable;
static hdPartsStateIdVtbl_c sFadeStateTable;
static hdStateId sFadeNullState;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sFadeStateGuard;
static hdHudWord_t sFadeStateCounter;
static hdHudWord_t sFadeInitWords[4] = {};
static hdHudWord_t sFadeRegistrations[9] = {};
static hdHudWord_t sFadePaneIndices[2] = {};
typedef be<u8> hdHudByte_t;
static hdHudByte_t sFadeReady;
static hdHudByte_t sFadeEnabled;
static f32 sFadePlayRate;
static f32 sFadeInitialFrame;
static f32 sFadeInitialRate;
static f32 sFadeFrame;
static f32 sFadeRate;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sFadeUi;
void fadeBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void fadeRegisterStatic(void*); void fadeConstructByte(void*); void fadeConstructBool(void*);
/* 02669250 */
void hdFadeWipe_c::destroy(hdFadeWipe_c* object, u32 flags) {
    if (!object) return;
    object->mStateHolder.mHolderVtable = &sFadeHolderTable;
    object->mSecondaryVtable = &sFadeSecondaryTable;
    hdParts_c::destroy(object, 0);
    if (flags&1) operator_delete(object);
}
/* 026692BC */
void hdFadeWipe_c::inEnter() { hdAnimSet_play(animSet(), 0, 0, sFadePlayRate); }
/* 026692D8 */
void hdFadeWipe_c::inExecute() { if(hdAnimSet_isEnd(animSet(), 0)) changeState(&sFadeDisplay); vCalcLayout(); }
/* 0266933C */
void hdFadeWipe_c::outEnter() { hdAnimSet_play(animSet(), 1, 0, sFadePlayRate); }
/* 02669358 */
void hdFadeWipe_c::outExecute() { if(hdAnimSet_isEnd(animSet(), 1)) changeState(&sFadeBlank); vCalcLayout(); }
/* 026693BC */
hdFadeWipe_c* hdFadeWipe_c::construct(hdFadeWipe_c* object) {
    if (!object) { object = (hdFadeWipe_c*)operator_new(sizeof(hdFadeWipe_c)); if(!object) return NULL; }
    hdParts_c::construct(object);
    object->mVtable = &sFadeTable; object->mStateHolder.mHolderVtable = &sFadeHolderTable;
    object->mSecondaryVtable = &sFadeSecondaryTable;
    return object;
}
/* 02669428 */
u32 hdFadeWipe_c::setup(u32 first, u32 second, void* desc) {
    attachArchives(&sFadeArchive, 1);
    if(!vLoad(&sFadeName, gabi::ea(mpResAccessor.get()), first, second, desc)) return 0;
    hdAnimSet_operationCC(animSet(), 0, 0);
    vCalcLayout();
    startState(&sFadeBlank);
    return 1;
}
/* 02669538 */
void hdFadeWipe_c::release() { vDestroyLayout(); detachArchives(); }
/* 02669578 */
void hdFadeWipe_c::execute() { hdParts_executeStateMachine(stateMachine()); }
/* 02669580 */
void hdFadeWipe_c::requestOut() { changeState(&sFadeOut); }
/* the target vtable is read before asking the current state for its key */
static u32 fadeStateMatches(hdStateMachine* machine, hdPartsStateId_c* target) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(machine);
    u32 getCurrent = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* targetTable = target->mVtable;
    u32 currentKey = gabi::call_ptr<u32>(getCurrent, current);
    return currentKey == gabi::call_ptr<u32>(targetTable->mGetKey.get(), target);
}
/* 02669590 */
u32 hdFadeWipe_c::isBlank() { return fadeStateMatches(stateMachine(), &sFadeBlank); }
/* 02669600 */
void hdFadeWipe_c::requestIn() { changeState(&sFadeIn); }
/* 02669610 */
u32 hdFadeWipe_c::isDisplay() { return fadeStateMatches(stateMachine(), &sFadeDisplay); }
/* 02669680 */
u32 hdFadeWipe_c::load(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sFadeUi->mHeap, 4);
    if(layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if(!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for(u32 i = 0; i<2; ++i) {
        u32 pane = sFadePaneIndices[i];
        fadeBindAnimation(animSet(), i, &sFadeAnimationNames[i], &sFadeAnimationPane[pane]);
    }
    vCalcLayout();
    return 1;
}
/* 026697AC */
static void __sinit_hd_screen_fade_wipe_cpp() {
    sFadeInitWords[2] = 0; sFadeInitWords[0] = 0; sFadeInitWords[3] = 0; sFadeInitWords[1] = 0;
    fadeRegisterStatic(&sFadeRegistrations[0]);
    sFadeFrame = sFadeInitialFrame; sFadeRate = sFadeInitialRate;
    fadeConstructByte(&sFadeReady); fadeRegisterStatic(&sFadeRegistrations[3]);
    fadeConstructBool(&sFadeEnabled); fadeRegisterStatic(&sFadeRegistrations[6]);
    sFadeAnimationPane[0].mVtable = &sFadeStringTable;
    sFadeAnimationPane[0].mText.v = gabi::guest_addr("G_All_00");
    u32 guard = sFadeStateGuard;
    sFadeArchive.mVtable = &sFadeStringTable;
    sFadeAnimationNames[0].mVtable = &sFadeStringTable;
    sFadeName.mVtable = &sFadeStringTable;
    sFadeAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sFadeAnimationNames[1].mVtable = &sFadeStringTable;
    sFadeName.mText.v = gabi::guest_addr("FadeWipe_00");
    sFadeAnimationNames[1].mText.v = gabi::guest_addr("OutAll");
    sFadeArchive.mText.v = gabi::guest_addr("FadeWipe_00");
    u32 number;
    if(guard) number = sFadeStateCounter; else { sFadeStateGuard = 1; number = 0; }
    sFadeIn.setup(++number, "::StateID_InAll", &sFadeStateTable, 0x19, &sFadeNullState);
    sFadeStateCounter = number+1;
    sFadeOut.setup(++number, "::StateID_OutAll", &sFadeStateTable, 0x1c, &sFadeNullState);
}
/* WWHD glue
data sFadeTable 100F4750 nocheck
data sFadeHolderTable 100F4848 nocheck
data sFadeSecondaryTable 100F4858 nocheck
data sFadeIn 10494100 nocheck
data sFadeOut 10494130 nocheck
data sFadeBlank 1049DD60 nocheck
data sFadeDisplay 1049DD90 nocheck
data sFadeArchive 104940CC nocheck
data sFadeName 10494160 nocheck
data sFadeAnimationNames 104940F0 nocheck
data sFadeAnimationPane 104940C4 nocheck
data sFadeStringTable 100F464C nocheck
data sFadeStateTable 100F46AC nocheck
data sFadeNullState 101FF32C nocheck
data sFadeStateGuard 101FD8E4 nocheck
data sFadeStateCounter 101FDD50 nocheck
data sFadeInitWords 104940E0 nocheck
data sFadeRegistrations 101F5ED8 nocheck
data sFadePaneIndices 100F4640 nocheck
data sFadeReady 104940DC nocheck
data sFadeEnabled 104940DD nocheck
data sFadePlayRate 100F46F8 nocheck
data sFadeInitialFrame 100F46FC nocheck
data sFadeInitialRate 100F4700 nocheck
data sFadeFrame 104940D4 nocheck
data sFadeRate 104940D8 nocheck
data sFadeUi 1018C404 nocheck
str 100F4704 "InAll" in 026697AC
str 100F470C "OutAll" in 026697AC
str 100F4714 "::StateID_InAll" in 026697AC
str 100F4724 "FadeWipe_00" in 026697AC
str 100F4730 "G_All_00" in 026697AC
str 100F473C "::StateID_OutAll" in 026697AC
ext 02004E04 void fadeBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void fadeRegisterStatic(void*)
ext 028ED6F8 void fadeConstructByte(void*)
ext 028EAB2C void fadeConstructBool(void*)
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
ext 026F91F8 void hdParts_c::detachArchives()
*/
