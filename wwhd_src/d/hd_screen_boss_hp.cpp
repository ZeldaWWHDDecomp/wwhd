/* HD-only boss meter: in/out transitions, health animation and nested icon. */
#include "d/hd_screen_boss_hp.h"
static hdPartsVtbl_c sBossTable;
static u32 sBossHolderTable;
static u32 sBossSecondaryTable;
static hdPartsStateId_c sBossWait;
static hdPartsStateId_c sBossIn;
static hdPartsStateId_c sBossDisplay;
static hdPartsStateId_c sBossOut;
static hdPartsStateId_c sBossIconIn;
static hdPartsStateId_c sBossIconWait;
static hdUiSafeString sBossName;
static hdUiSafeString sBossClassName;
static hdUiSafeString sBossIconPane;
static hdUiSafeString sBossAnimationNames[3] = {};
static hdUiSafeString sBossAnimationPanes[2] = {};
typedef be<u32> hdHudWord_t;
static hdHudWord_t sBossPaneIndices[3] = {};
typedef be<u8> hdHudByte_t;
static hdHudByte_t sBossActive;
static f32 sBossRate;
static f32 sBossLastFrame;
static f32 sBossHealthFrame;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sBossUi;
static hdHudWord_t sBossInitWords[4] = {};
static hdHudWord_t sBossRegistrations[9] = {};
static hdHudWord_t sBossStateGuard;
static hdHudWord_t sBossStateCounter;
static hdStateId sBossNullState;
static hdPartsStateIdVtbl_c sBossStateTable;
static hdUiStringTable sBossStringTable;
static f32 sBossInitialFrame;
static f32 sBossInitialRate;
static f32 sBossFrame;
static f32 sBossInitialisedRate;
static hdHudByte_t sBossReady;
static hdHudByte_t sBossEnabled;
void bossRegisterStatic(void*);
void bossConstructByte(void*);
void bossConstructBool(void*);
void bossBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void bossConstructBase(NoControlPart_l*);
void bossDestroyBase(NoControlPart_l*, u32);
u32 bossSetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32);
hdParts_c* bossConstructIcon(hdParts_c*);
u32 bossSetupIcon(hdParts_c*, u32, u32, hdBossPartDesc_c*);

/* 0262939C */
static s32 bossStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 0262949C */
void hdBossHp_c::destroy(hdBossHp_c* object, u32 flags) {
    if (!object) return;
    object->mStateHolder.mHolderVtable = &sBossHolderTable;
    object->mSecondaryVtable = &sBossSecondaryTable;
    bossDestroyBase(object, 0);
    if (flags & 1) operator_delete(object);
}
/* 02629508 */
void hdBossHp_c::waitEnter() { hdAnimSet_setFrame(animSet(), 0, 1, sBossLastFrame); }
/* 02629524 */
void hdBossHp_c::waitExecute() {
    if (sBossActive) changeState(&sBossIn);
    vExecuteChildren();
    vCalcLayout();
}
/* 02629594 */
void hdBossHp_c::inEnter() {
    hdAnimSet_play(animSet(), 0, 1, sBossRate);
    hdParts_c* icon = *mChildren.mpData;
    icon->changeState(&sBossIconIn);
}
/* 026295F0 */
void hdBossHp_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), 0)) changeState(&sBossDisplay);
    vExecuteChildren();
    vCalcLayout();
}
/* 02629668 */
void hdBossHp_c::displayExecute() {
    if (!sBossActive) changeState(&sBossOut);
    hdAnimSet_setFrame(animSet(), 1, 0, sBossHealthFrame);
    vExecuteChildren();
    vCalcLayout();
}
/* 026296FC */
void hdBossHp_c::outEnter() { hdAnimSet_play(animSet(), 2, 1, sBossRate); }
/* 02629718 */
void hdBossHp_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), 2)) changeState(&sBossWait);
    vExecuteChildren();
    vCalcLayout();
}
/* 02629790 */
void hdBossHp_c::outExit() { hdParts_c* icon = *mChildren.mpData; icon->changeState(&sBossIconWait); }
/* 026297A8 */
hdBossHp_c* hdBossHp_c::construct(hdBossHp_c* object) {
    if (!object) { object = (hdBossHp_c*)operator_new(sizeof(hdBossHp_c)); if (!object) return NULL; }
    bossConstructBase(object);
    object->mVtable = &sBossTable;
    object->mStateHolder.mHolderVtable = &sBossHolderTable;
    object->mSecondaryVtable = &sBossSecondaryTable;
    return object;
}
/* 02629814 */
u32 hdBossHp_c::setup(u32 first, u32 second, u32 third) {
    if (!bossSetupBase(this, &sBossName, first, second, third)) return 0;
    startState(&sBossWait);
    return 1;
}
/* 026298A8 */
void hdBossHp_c::toWait() { changeState(&sBossWait); }
/* 026298B8 */
u32 hdBossHp_c::load(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sBossUi->mHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 3, 2, desc)) return 0;
    for (u32 i = 0; i<3; ++i) {
        u32 pane = sBossPaneIndices[i];
        bossBindAnimation(animSet(), i, &sBossAnimationNames[i], &sBossAnimationPanes[pane]);
    }
    return 1;
}
/* inlined into createBossIcon */
static bool bossStringsEqual(const be<u8>* left, const be<u8>* right) {
    for (u32 i = 0; i<0x40001; ++i) { u8 a = left[i], b = right[i]; if (a != b) return false; if (!a) return true; }
    return false;
}
/* 026299D0 */
u32 hdBossHp_c::createBossIcon(hdBossPartDesc_c* desc) {
    hdScreenHeap* heap = sBossUi->mHeap;
    desc->mClassName.assureTermination();
    desc->mClassName.assureTermination();
    be<u8>* text = desc->mClassName.mText;
    sBossClassName.assureTermination();
    if (text != sBossClassName.mText.get() && !bossStringsEqual(desc->mClassName.mText, sBossClassName.mText)) return 1;
    desc->mResource.assureTermination();
    desc->mResource.assureTermination();
    text = desc->mResource.mText;
    sBossIconPane.assureTermination();
    if (text != sBossIconPane.mText.get() && !bossStringsEqual(desc->mResource.mText, sBossIconPane.mText))
        return createChild(0, reinterpret_cast<hdPartsDesc_c*>(desc));
    gptr<hdParts_c>* slot = mChildren.mpData;
    hdParts_c* icon = (hdParts_c*)hdParts_new(0x58, heap, 4);
    if (icon) icon = bossConstructIcon(icon);
    *slot = icon;
    icon = *mChildren.mpData;
    if (!icon) return 0;
    return bossSetupIcon(*mChildren.mpData, 0, 0, desc);
}
/* 02629BD8 */
static void __sinit_hd_screen_boss_hp_cpp() {
    sBossInitWords[3] = 0; sBossInitWords[2] = 0; sBossInitWords[1] = 0; sBossInitWords[0] = 0;
    bossRegisterStatic(&sBossRegistrations[0]);
    sBossFrame = sBossInitialFrame; sBossInitialisedRate = sBossInitialRate;
    bossConstructByte(&sBossReady);
    bossRegisterStatic(&sBossRegistrations[3]);
    bossConstructBool(&sBossEnabled);
    bossRegisterStatic(&sBossRegistrations[6]);
    sBossAnimationPanes[0].mVtable = &sBossStringTable;
    sBossAnimationPanes[0].mText.v = gabi::guest_addr("G_BossHP_00");
    sBossAnimationPanes[1].mVtable = &sBossStringTable;
    u32 guard = sBossStateGuard;
    sBossAnimationNames[0].mVtable = &sBossStringTable;
    sBossAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sBossClassName.mVtable = &sBossStringTable;
    sBossClassName.mText.v = gabi::guest_addr("L_LoopBossIcon_00");
    sBossAnimationNames[1].mVtable = &sBossStringTable;
    sBossAnimationNames[1].mText.v = gabi::guest_addr("BossHP");
    sBossAnimationNames[2].mVtable = &sBossStringTable;
    sBossAnimationNames[2].mText.v = gabi::guest_addr("OutAll");
    sBossAnimationPanes[1].mText.v = gabi::guest_addr("G_All_00");
    sBossName.mVtable = &sBossStringTable;
    sBossName.mText.v = gabi::guest_addr("BossHP_00");
    u32 number;
    if (guard) number = sBossStateCounter;
    else { sBossStateGuard = 1; number = 0; }
    sBossWait.setup(++number, "::StateID_Wait", &sBossStateTable, 0x19, &sBossNullState);
    sBossIn.setup(++number, "::StateID_In", &sBossStateTable, 0x1c, &sBossNullState);
    sBossDisplay.setup(++number, "::StateID_Disp", &sBossStateTable, 0x1f, &sBossNullState);
    sBossStateCounter = number+1;
    sBossOut.setup(++number, "::StateID_Out", &sBossStateTable, 0x22, &sBossNullState);
}
/* WWHD glue
data sBossInitWords 1048F374 nocheck
data sBossRegistrations 101F53EC nocheck
data sBossStateGuard 101FD8E4 nocheck
data sBossStateCounter 101FDD50 nocheck
data sBossNullState 101FF32C nocheck
data sBossStateTable 100E6018 nocheck
data sBossStringTable 100E5FAC nocheck
data sBossInitialFrame 100E6068 nocheck
data sBossInitialRate 100E606C nocheck
data sBossFrame 1048F368 nocheck
data sBossInitialisedRate 1048F36C nocheck
data sBossReady 1048F370 nocheck
data sBossEnabled 1048F371 nocheck
ext 028F026C void bossRegisterStatic(void*)
ext 028ED6F8 void bossConstructByte(void*)
ext 028EAB2C void bossConstructBool(void*)
ext 02004E04 void bossBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
str 100E6088 "G_BossHP_00" in 02629BD8
str 100E6094 "L_LoopBossIcon_00" in 02629BD8
str 100E60A8 "BossHP_00" in 02629BD8
str 100E60C4 "G_All_00" in 02629BD8
str 100E6070 "InAll" in 02629BD8
str 100E6078 "BossHP" in 02629BD8
str 100E6080 "OutAll" in 02629BD8
str 100E60D0 "::StateID_Wait" in 02629BD8
str 100E60E0 "::StateID_In" in 02629BD8
str 100E60F0 "::StateID_Disp" in 02629BD8
str 100E60B4 "::StateID_Out" in 02629BD8
data sBossTable 100E6100 nocheck
data sBossHolderTable 100E6228 nocheck
data sBossSecondaryTable 100E6238 nocheck
data sBossWait 1048F3AC nocheck
data sBossIn 1048F3DC nocheck
data sBossDisplay 1048F40C nocheck
data sBossOut 1048F43C nocheck
data sBossIconIn 10495074 nocheck
data sBossIconWait 1049DD60 nocheck
data sBossName 1048F46C nocheck
data sBossClassName 1048F360 nocheck
data sBossIconPane 104950A4 nocheck
data sBossAnimationNames 1048F394 nocheck
data sBossAnimationPanes 1048F384 nocheck
data sBossPaneIndices 100E5FC4 nocheck
data sBossActive 1047B09B nocheck
data sBossRate 100E6060 nocheck
data sBossLastFrame 100E6064 nocheck
data sBossHealthFrame 1047B064 nocheck
data sBossUi 1018C404 nocheck
ext 02700ADC void bossConstructBase(NoControlPart_l*)
ext 02700A70 void bossDestroyBase(NoControlPart_l*, u32)
ext 02700B48 u32 bossSetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32)
ext 02674830 hdParts_c* bossConstructIcon(hdParts_c*)
ext 026748A8 u32 bossSetupIcon(hdParts_c*, u32, u32, hdBossPartDesc_c*)
ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
*/
