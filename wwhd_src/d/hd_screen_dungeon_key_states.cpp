/* HD dungeon-key count formatting and transitions; merged sparkle body is called by address. */
#include "d/hd_screen_dungeon_key_states.h"
#include "d/hd_screen_command_a.h"
static hdPartsVtbl_c sDungeonKeyTable;
static u32 sDungeonKeyHolderTable;
static u32 sDungeonKeySecondaryTable;
static u32 sDungeonKeyTextTable;
static hdPartsStateId_c sDungeonKeyWait;
static hdPartsStateId_c sDungeonKeyIn;
static hdPartsStateId_c sDungeonKeyDisplay;
static hdPartsStateId_c sDungeonKeyOut;
static hdUiSafeString sDungeonKeyPictureName;
static hdUiSafeString sDungeonKeyTextName;
static hdUiSafeString sDungeonKeyAnimationNames[4] = {};
static hdUiSafeString sDungeonKeyAnimationPanes[3] = {};
static hdUiSafeString sDungeonKeyName;
static hdUiStringTable sDungeonKeyStringTable;
static hdPartsStateIdVtbl_c sDungeonKeyStateTable;
static hdStateId sDungeonKeyNullState;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sDungeonKeyStateGuard;
static hdHudWord_t sDungeonKeyStateCounter;
static hdHudWord_t sDungeonKeyInitWords[4] = {};
static hdHudWord_t sDungeonKeyRegistrations[9] = {};
/* Canonical shared views; the fourth index is a separate adjacent image word. */
static hdHudWord_t sDungeonKeyFourthAnimationId;
typedef be<u8> hdHudByte_t;
static hdHudByte_t sDungeonKeyActive;
static hdHudByte_t sDungeonKeyReady;
static hdHudByte_t sDungeonKeyEnabled;
static f32 sDungeonKeyLastFrame;
static f32 sDungeonKeyInitialFrame;
static f32 sDungeonKeyInitialRate;
static f32 sDungeonKeyFrame;
static f32 sDungeonKeyRate;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sDungeonKeyUi;
typedef gptr<hdDungeonKeyGame_c> hdDungeonKeyGamePointer_t;
static hdDungeonKeyGamePointer_t sDungeonKeyGame;
void dungeonKeyConstructBase(NoControlPart_l*); void dungeonKeyDestroyBase(NoControlPart_l*, u32);
u32 dungeonKeySetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32);
void dungeonKeyBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void dungeonKeyUpdateSparkles(hdDungeonKeyStates_c*);
void dungeonKeyFormatCount(hdDungeonKeyText_c*, const char*, u32);
void dungeonKeyRegisterStatic(void*); void dungeonKeyConstructByte(void*); void dungeonKeyConstructBool(void*);
/* 02653A5C */
void hdDungeonKeyStates_c::destroy(hdDungeonKeyStates_c* object, u32 flags) {
    if(!object) return;
    object->mStateHolder.mHolderVtable = &sDungeonKeyHolderTable;
    object->mSecondaryVtable = &sDungeonKeySecondaryTable;
    dungeonKeyDestroyBase(object, 0);
    if(flags&1) operator_delete(object);
}
/* 02653AC8 */
void hdDungeonKeyStates_c::waitEnter() { hdAnimSet_setFrame(animSet(), 0, 2, sDungeonKeyLastFrame); }
/* 02653AE4 */
void hdDungeonKeyStates_c::waitExecute() { if(sDungeonKeyActive) changeState(&sDungeonKeyIn); }
/* 02653B04 */
void hdDungeonKeyStates_c::updateCountText() {
    gabi::NativeFrame<0x20> frame;
    hdDungeonKeyText_c* text = gabi::at<hdDungeonKeyText_c>(frame + 8);
    hdDungeonKeyTextBuffer_c* buffer = gabi::at<hdDungeonKeyTextBuffer_c>(frame + 0x14);
    buffer->mCharacters[0] = 0;
    text->mpBuffer = buffer->mCharacters;
    text->mCapacity = 3;
    text->mpVTable.v = gabi::ea(&sDungeonKeyTextTable);
    buffer->mCharacters[2] = 0;
    dungeonKeyFormatCount(text, "DungeonKey count format", mKeyCount);
    mpTextPane->setStringPrepared(text, nullptr);
}
/* 02653B7C */
void hdDungeonKeyStates_c::inEnter() { hdAnimSet_play(animSet(), 0, 2, sKeyAnimationRate); updateCountText(); }
/* 02653BC8 */
void hdDungeonKeyStates_c::inExecute() { if(hdAnimSet_isEnd(animSet(), 0)) changeState(&sDungeonKeyDisplay); }
/* 02653ED8 */
void hdDungeonKeyStates_c::displayExecute() {
    u32 count = sDungeonKeyGame->mKeyCount;
    if(mKeyCount != count) { mKeyCount = count; updateCountText(); }
    dungeonKeyUpdateSparkles(this);
    if(!sDungeonKeyActive) changeState(&sDungeonKeyOut);
}
/* 02653F4C */
void hdDungeonKeyStates_c::outEnter() { hdAnimSet_play(animSet(), 3, 2, sKeyAnimationRate); }
/* 02653F68 */
void hdDungeonKeyStates_c::outExecute() { if(hdAnimSet_isEnd(animSet(), 3)) changeState(&sDungeonKeyWait); }
/* 02653FB8 */
hdDungeonKeyStates_c* hdDungeonKeyStates_c::construct(hdDungeonKeyStates_c* object) {
    if(!object) { object = (hdDungeonKeyStates_c*)operator_new(sizeof(hdDungeonKeyStates_c)); if(!object) return NULL; }
    dungeonKeyConstructBase(object);
    object->mVtable = &sDungeonKeyTable;
    object->mSecondaryVtable = &sDungeonKeySecondaryTable;
    object->mStateHolder.mHolderVtable = &sDungeonKeyHolderTable;
    /* The embedded SafeArray constructor retains its wraparound allocation fallback. */
    if (gabi::ea(object) + u32(offsetof(hdDungeonKeyStates_c, mSparkleActive)) == 0) operator_new(8);
    object->mpTextPane = NULL;
    object->mRandomInterval = 30; object->mMinimumInterval = 15; object->mSparkleTimer = 0;
    object->mFlagB = 0; object->mFlagA = 0;
    object->mKeyCount = sDungeonKeyGame->mKeyCount;
    for(u32 i = 0; i<2; ++i) object->mSparkleActive[i] = 0;
    return object;
}
/* 0265407C */
void hdDungeonKeyStates_c::findTextPane() {
    hdDungeonKeyRootPane_c* root = (hdDungeonKeyRootPane_c*)mpLayout->lyt()->mpRootPane.get();
    hdDungeonKeyPaneTable_c* table = root->mVtable;
    sDungeonKeyTextName.assureTermination();
    mpTextPane = gabi::call_ptr<hdTextPane_c*>(table->mFindPane.get(), root, sDungeonKeyTextName.mText.get(), 1);
}
/* 02654104 */
u32 hdDungeonKeyStates_c::setup(u32 first, u32 second, u32 third) {
    if(!dungeonKeySetupBase(this, &sDungeonKeyName, first, second, third)) return 0;
    findTextPane(); updateCountText(); startState(&sDungeonKeyWait);
    return 1;
}
/* 026541A8 */
void hdDungeonKeyStates_c::toWait() { changeState(&sDungeonKeyWait); }
/* 026541B8 */
u32 hdDungeonKeyStates_c::load(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sDungeonKeyUi->mHeap, 4);
    if(layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if(!layout || !layout->vLoad(name, first, second, third, 4, 3, desc)) return 0;
    for(u32 i = 0; i<4; ++i) {
        u32 pane = i<3 ? (u32)sKeyAnimationIds[i] : (u32)sDungeonKeyFourthAnimationId;
        dungeonKeyBindAnimation(animSet(), i, &sDungeonKeyAnimationNames[i], &sDungeonKeyAnimationPanes[pane]);
    }
    return 1;
}
/* 026542D0 */
static void __sinit_hd_screen_dungeon_key_states_cpp() {
    sDungeonKeyInitWords[2] = 0; sDungeonKeyInitWords[0] = 0; sDungeonKeyInitWords[3] = 0; sDungeonKeyInitWords[1] = 0;
    dungeonKeyRegisterStatic(&sDungeonKeyRegistrations[0]);
    sDungeonKeyRate = sDungeonKeyInitialRate; sDungeonKeyFrame = sDungeonKeyInitialFrame;
    dungeonKeyConstructByte(&sDungeonKeyReady); dungeonKeyRegisterStatic(&sDungeonKeyRegistrations[3]);
    dungeonKeyConstructBool(&sDungeonKeyEnabled); dungeonKeyRegisterStatic(&sDungeonKeyRegistrations[6]);
    sDungeonKeyPictureName.mVtable = &sDungeonKeyStringTable;
    sDungeonKeyPictureName.mText.v = gabi::guest_addr("P_DungeonKey_00");
    sDungeonKeyTextName.mVtable = &sDungeonKeyStringTable;
    sDungeonKeyTextName.mText.v = gabi::guest_addr("T_NumKey_00");
    sDungeonKeyAnimationPanes[0].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyAnimationNames[0].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyAnimationPanes[0].mText.v = gabi::guest_addr("G_KeyEffect_00");
    sDungeonKeyAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sDungeonKeyAnimationNames[1].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyAnimationPanes[1].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyAnimationNames[1].mText.v = gabi::guest_addr("KeyEffect");
    sDungeonKeyAnimationNames[2].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyAnimationPanes[1].mText.v = gabi::guest_addr("G_KeyEffect_01");
    sDungeonKeyAnimationNames[2].mText.v = gabi::guest_addr("KeyEffect");
    sDungeonKeyAnimationPanes[2].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyName.mVtable = &sDungeonKeyStringTable;
    u32 guard = sDungeonKeyStateGuard;
    sDungeonKeyName.mText.v = gabi::guest_addr("DungeonKey_00");
    sDungeonKeyAnimationPanes[2].mText.v = gabi::guest_addr("G_All_00");
    sDungeonKeyAnimationNames[3].mVtable = &sDungeonKeyStringTable;
    sDungeonKeyAnimationNames[3].mText.v = gabi::guest_addr("OutAll");
    u32 number;
    if(guard) number = sDungeonKeyStateCounter; else { sDungeonKeyStateGuard = 1; number = 0; }
    sDungeonKeyWait.setup(++number, "::StateID_Wait", &sDungeonKeyStateTable, 0x19, &sDungeonKeyNullState);
    sDungeonKeyIn.setup(++number, "::StateID_In", &sDungeonKeyStateTable, 0x1c, &sDungeonKeyNullState);
    sDungeonKeyDisplay.setup(++number, "::StateID_Disp", &sDungeonKeyStateTable, 0x1f, &sDungeonKeyNullState);
    sDungeonKeyOut.setup(++number, "::StateID_Out", &sDungeonKeyStateTable, 0x22, &sDungeonKeyNullState);
    sDungeonKeyStateCounter = number;
}
/* WWHD glue
data sDungeonKeyTable 100EFF58 nocheck
data sDungeonKeyHolderTable 100F0080 nocheck
data sDungeonKeySecondaryTable 100F0090 nocheck
data sDungeonKeyTextTable 100EFDAC nocheck
data sDungeonKeyWait 1049215C nocheck
data sDungeonKeyIn 1049218C nocheck
data sDungeonKeyDisplay 104921BC nocheck
data sDungeonKeyOut 104921EC nocheck
data sDungeonKeyPictureName 104920F8 nocheck
data sDungeonKeyTextName 10492100 nocheck
data sDungeonKeyAnimationNames 1049213C nocheck
data sDungeonKeyAnimationPanes 10492124 nocheck
data sDungeonKeyName 1049221C nocheck
data sDungeonKeyStringTable 100EFD7C nocheck
data sDungeonKeyStateTable 100EFE1C nocheck
data sDungeonKeyNullState 101FF32C nocheck
data sDungeonKeyStateGuard 101FD8E4 nocheck
data sDungeonKeyStateCounter 101FDD50 nocheck
data sDungeonKeyInitWords 10492114 nocheck
data sDungeonKeyRegistrations 101F5A4C nocheck
data sDungeonKeyFourthAnimationId 100EFDD0 nocheck
data sDungeonKeyActive 1047B092 nocheck
data sDungeonKeyReady 10492110 nocheck
data sDungeonKeyEnabled 10492111 nocheck
data sDungeonKeyLastFrame 100EFE6C nocheck
data sDungeonKeyInitialFrame 100EFE98 nocheck
data sDungeonKeyInitialRate 100EFE9C nocheck
data sDungeonKeyFrame 10492108 nocheck
data sDungeonKeyRate 1049210C nocheck
data sDungeonKeyUi 1018C404 nocheck
data sDungeonKeyGame 101F84DC nocheck
str 100EFEA0 "InAll" in 026542D0
str 100EFEA8 "OutAll" in 026542D0
str 100EFEB0 "P_DungeonKey_00" in 026542D0
str 100EFEC0 "T_NumKey_00" in 026542D0
str 100EFECC "KeyEffect" in 026542D0
str 100EFED8 "DungeonKey_00" in 026542D0
str 100EFEE8 "::StateID_Out" in 026542D0
str 100EFEF8 "G_KeyEffect_00" in 026542D0
str 100EFF08 "G_KeyEffect_01" in 026542D0
str 100EFF18 "G_All_00" in 026542D0
str 100EFF24 "::StateID_Wait" in 026542D0
str 100EFF34 "::StateID_In" in 026542D0
str 100EFF44 "::StateID_Disp" in 026542D0
str 100EFE70 "DungeonKey count format" renamed
ext 02700ADC void dungeonKeyConstructBase(NoControlPart_l*)
ext 02700A70 void dungeonKeyDestroyBase(NoControlPart_l*, u32)
ext 02700B48 u32 dungeonKeySetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32)
ext 02004E04 void dungeonKeyBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 02653C18 void dungeonKeyUpdateSparkles(hdDungeonKeyStates_c*)
ext 02759D3C void dungeonKeyFormatCount(hdDungeonKeyText_c*,const char*,u32)
ext 028F026C void dungeonKeyRegisterStatic(void*)
ext 028ED6F8 void dungeonKeyConstructByte(void*)
ext 028EAB2C void dungeonKeyConstructBool(void*)
ext 027046D4 void hdTextPane_c::setStringPrepared(PaneString_l*,PaneWork_l*)
*/
