/* HD-only iris wipe: automatic in/out animations and fractional-frame fading. */
#include "d/hd_screen_fade_wipe_iris.h"
static hdPartsVtbl_c sIrisTable;
static u32 sIrisHolderTable;
static u32 sIrisSecondaryTable;
static hdPartsStateId_c sIrisFade;
static hdPartsStateId_c sIrisIn;
static hdPartsStateId_c sIrisOut;
static hdPartsStateId_c sIrisBlank;
static hdUiSafeString sIrisArchive;
static hdUiSafeString sIrisName;
static hdUiSafeString sIrisAnimationNames[2] = {};
static hdUiSafeString sIrisAnimationPane[1] = {};
static hdUiStringTable sIrisStringTable;
static hdPartsStateIdVtbl_c sIrisStateTable;
static hdStateId sIrisNullState;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sIrisStateGuard;
static hdHudWord_t sIrisStateCounter;
static hdHudWord_t sIrisInitWords[4] = {};
static hdHudWord_t sIrisRegistrations[9] = {};
static hdHudWord_t sIrisPaneIndices[2] = {};
typedef be<u8> hdHudByte_t;
static hdHudByte_t sIrisReady;
static hdHudByte_t sIrisEnabled;
static f32 sIrisPlayRate;
static f32 sIrisMinimum;
static f32 sIrisInitialFrame;
static f32 sIrisInitialRate;
static f32 sIrisFrame;
static f32 sIrisRate;
static f64 sIrisFullAmount;
static f64 sIrisUnsignedBias;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sIrisUi;
void irisBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
u32 irisFrameCount(hdAnimSetController*);
void irisRegisterStatic(void*); void irisConstructByte(void*); void irisConstructBool(void*);
/* 02669A98 */
static s32 irisStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 02669B98 */
void hdFadeWipeIris_c::destroy(hdFadeWipeIris_c* object, u32 flags) {
    if(!object) return;
    object->mStateHolder.mHolderVtable = &sIrisHolderTable;
    object->mSecondaryVtable = &sIrisSecondaryTable;
    hdParts_c::destroy(object, 0);
    if(flags&1) operator_delete(object);
}
/* 02669C04 */
void hdFadeWipeIris_c::fadeEnter() { hdAnimSet_operationCC(animSet(), 0, 0); vCalcLayout(); }
/* 02669C54 */
void hdFadeWipeIris_c::fadeExecute() { hdAnimSet_setFrame(animSet(), 0, 0, (f32)(mFrameCount*mFraction)); vCalcLayout(); }
/* 02669CB0 */
void hdFadeWipeIris_c::inEnter() { hdAnimSet_play(animSet(), 0, 0, sIrisPlayRate); }
/* 02669CCC */
void hdFadeWipeIris_c::inExecute() { vCalcLayout(); }
/* 02669CDC */
void hdFadeWipeIris_c::outEnter() { hdAnimSet_play(animSet(), 1, 0, sIrisPlayRate); }
/* 02669CF8 */
void hdFadeWipeIris_c::outExecute() { vCalcLayout(); }
/* 02669D08 */
hdFadeWipeIris_c* hdFadeWipeIris_c::construct(hdFadeWipeIris_c* object) {
    if(!object) { object = (hdFadeWipeIris_c*)operator_new(sizeof(hdFadeWipeIris_c)); if(!object) return NULL; }
    hdParts_c::construct(object);
    f32 initial = sIrisMinimum;
    object->mFrameCount = initial;
    object->mVtable = &sIrisTable; object->mStateHolder.mHolderVtable = &sIrisHolderTable;
    object->mFraction = initial; object->mSecondaryVtable = &sIrisSecondaryTable;
    return object;
}
/* 02669D84 */
u32 hdFadeWipeIris_c::setup(u32 first, u32 second, void* desc) {
    attachArchives(&sIrisArchive, 1);
    if(!vLoad(&sIrisName, gabi::ea(mpResAccessor.get()), first, second, desc)) return 0;
    hdAnimSet_operationCC(animSet(), 0, 0);
    hdAnimSetEntry* entry = animSet()->mEntries.mData[0];
    u32 count = irisFrameCount(entry->mController);
    union { u64 bits; f64 value; } converted;
    converted.bits = 0x4330000000000000ULL|count;
    mFrameCount = (f32)(converted.value-sIrisUnsignedBias);
    startState(&sIrisBlank);
    return 1;
}
/* 02669EBC */
void hdFadeWipeIris_c::release() { vDestroyLayout(); detachArchives(); }
static bool irisIsBlank(hdStateMachine* machine) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(machine);
    u32 currentGetter = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* blankTable = sIrisBlank.mVtable;
    u32 key = gabi::call_ptr<u32>(currentGetter, current);
    return key == gabi::call_ptr<u32>(blankTable->mGetKey.get(), &sIrisBlank);
}
/* 02669EFC */
void hdFadeWipeIris_c::execute() { if(!irisIsBlank(stateMachine())) hdParts_executeStateMachine(stateMachine()); }
/* 02669F7C */
void hdFadeWipeIris_c::draw(void* info) { if(!irisIsBlank(stateMachine())) vDrawLayout(info); }
/* 0266A018 */
void hdFadeWipeIris_c::stop() { hdAnimSet_operationCC(animSet(), 0, 0); changeState(&sIrisBlank); }
/* 0266A064 */
void hdFadeWipeIris_c::setAmount(f32 amount) {
    if(irisIsBlank(stateMachine())) changeState(&sIrisFade);
    f32 fraction = (f32)(sIrisFullAmount-(f64)amount);
    f32 minimum = sIrisMinimum;
    if(fraction<minimum) fraction = minimum;
    else { f32 maximum = sIrisPlayRate; if(fraction>maximum) fraction = maximum; }
    mFraction = fraction;
}
/* 0266A178 */
u32 hdFadeWipeIris_c::load(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sIrisUi->mHeap, 4);
    if(layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if(!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for(u32 i = 0; i<2; ++i) {
        u32 pane = sIrisPaneIndices[i];
        irisBindAnimation(animSet(), i, &sIrisAnimationNames[i], &sIrisAnimationPane[pane]);
    }
    vCalcLayout();
    return 1;
}
/* 0266A2A4 */
static void __sinit_hd_screen_fade_wipe_iris_cpp() {
    sIrisInitWords[2] = 0; sIrisInitWords[0] = 0; sIrisInitWords[3] = 0; sIrisInitWords[1] = 0;
    irisRegisterStatic(&sIrisRegistrations[0]);
    sIrisFrame = sIrisInitialFrame; sIrisRate = sIrisInitialRate;
    irisConstructByte(&sIrisReady); irisRegisterStatic(&sIrisRegistrations[3]);
    irisConstructBool(&sIrisEnabled); irisRegisterStatic(&sIrisRegistrations[6]);
    sIrisArchive.mVtable = &sIrisStringTable;
    sIrisAnimationNames[0].mVtable = &sIrisStringTable;
    sIrisAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sIrisAnimationPane[0].mVtable = &sIrisStringTable;
    sIrisAnimationPane[0].mText.v = gabi::guest_addr("G_All_00");
    u32 guard = sIrisStateGuard;
    sIrisName.mVtable = &sIrisStringTable;
    sIrisAnimationNames[1].mVtable = &sIrisStringTable;
    sIrisAnimationNames[1].mText.v = gabi::guest_addr("OutAll");
    sIrisName.mText.v = gabi::guest_addr("FadeWipe_01");
    sIrisArchive.mText.v = gabi::guest_addr("FadeWipe_01");
    u32 number;
    if(guard) number = sIrisStateCounter; else { sIrisStateGuard = 1; number = 0; }
    sIrisFade.setup(++number, "::StateID_Fade", &sIrisStateTable, 0x19, &sIrisNullState);
    sIrisIn.setup(++number, "::StateID_InAll", &sIrisStateTable, 0x1c, &sIrisNullState);
    sIrisStateCounter = number+1;
    sIrisOut.setup(++number, "::StateID_OutAll", &sIrisStateTable, 0x1f, &sIrisNullState);
}
/* WWHD glue
data sIrisTable 100F49A0 nocheck
data sIrisHolderTable 100F4AB0 nocheck
data sIrisSecondaryTable 100F4AC0 nocheck
data sIrisFade 104941A4 nocheck
data sIrisIn 104941D4 nocheck
data sIrisOut 10494204 nocheck
data sIrisBlank 1049DD60 nocheck
data sIrisArchive 10494170 nocheck
data sIrisName 10494234 nocheck
data sIrisAnimationNames 10494194 nocheck
data sIrisAnimationPane 10494168 nocheck
data sIrisStringTable 100F4874 nocheck
data sIrisStateTable 100F48D4 nocheck
data sIrisNullState 101FF32C nocheck
data sIrisStateGuard 101FD8E4 nocheck
data sIrisStateCounter 101FDD50 nocheck
data sIrisInitWords 10494184 nocheck
data sIrisRegistrations 101F5EFC nocheck
data sIrisPaneIndices 100F4868 nocheck
data sIrisReady 10494180 nocheck
data sIrisEnabled 10494181 nocheck
data sIrisPlayRate 100F4928 nocheck
data sIrisMinimum 100F492C nocheck
data sIrisInitialFrame 100F4938 nocheck
data sIrisInitialRate 100F493C nocheck
data sIrisFrame 10494178 nocheck
data sIrisRate 1049417C nocheck
data sIrisFullAmount 100F4920 nocheck
data sIrisUnsignedBias 100F4930 nocheck
data sIrisUi 1018C404 nocheck
str 100F4940 "InAll" in 0266A2A4
str 100F4948 "OutAll" in 0266A2A4
str 100F4950 "::StateID_InAll" in 0266A2A4
str 100F4960 "FadeWipe_01" in 0266A2A4
str 100F496C "G_All_00" in 0266A2A4
str 100F4978 "::StateID_Fade" in 0266A2A4
str 100F4988 "::StateID_OutAll" in 0266A2A4
ext 02004E04 void irisBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028713B8 u32 irisFrameCount(hdAnimSetController*)
ext 028F026C void irisRegisterStatic(void*)
ext 028ED6F8 void irisConstructByte(void*)
ext 028EAB2C void irisConstructBool(void*)
*/
