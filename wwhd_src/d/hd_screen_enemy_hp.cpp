/* HD-only enemy health screen. Game-derived names and layout views are descriptive. */
#include "d/hd_screen_enemy_hp.h"
static hdPartsVtbl_c sEnemyTable;
static u32 sEnemyHolderTable;
static u32 sEnemySecondaryTable;
static hdPartsStateId_c sEnemyWait;
static hdPartsStateId_c sEnemyDisplay;
static hdUiSafeString sEnemyName;
static hdUiSafeString sEnemyAnimationName;
static hdUiSafeString sEnemyAnimationPane[1] = {};
static hdUiSafeString sEnemyPaneName;
static hdUiStringTable sEnemyStringTable;
static hdPartsStateIdVtbl_c sEnemyStateTable;
static hdStateId sEnemyNullState;
typedef be<u32> hdHudWord_t;
static hdHudWord_t sEnemyStateGuard;
static hdHudWord_t sEnemyStateCounter;
static hdHudWord_t sEnemyInitWords[4] = {};
static hdHudWord_t sEnemyRegistrations[9] = {};
static hdHudWord_t sEnemyPaneIndex;
typedef be<u8> hdHudByte_t;
static hdHudByte_t sEnemyActive;
static hdHudByte_t sEnemyReady;
static hdHudByte_t sEnemyEnabled;
static f32 sEnemyX;
static f32 sEnemyY;
static f32 sEnemyHealth;
static f32 sEnemyInitialFrame;
static f32 sEnemyInitialRate;
static f32 sEnemyFrame;
static f32 sEnemyRate;
typedef gptr<hdScreenUiSystem> hdHudUiPointer_t;
static hdHudUiPointer_t sEnemyUi;
void enemyConstructBase(NoControlPart_l*);
void enemyDestroyBase(NoControlPart_l*, u32);
u32 enemySetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32);
void enemyBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void enemyRegisterStatic(void*); void enemyConstructByte(void*); void enemyConstructBool(void*);
/* 026682D4 */
static s32 enemyStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026683D4 */
void hdEnemyHp_c::destroy(hdEnemyHp_c* object, u32 flags) {
    if (!object) return;
    object->mStateHolder.mHolderVtable = &sEnemyHolderTable;
    object->mSecondaryVtable = &sEnemySecondaryTable;
    enemyDestroyBase(object, 0);
    if (flags&1) operator_delete(object);
}
/* 02668440 */
void hdEnemyHp_c::waitExecute() {
    if (sEnemyActive) changeState(&sEnemyDisplay);
    vCalcLayout();
}
/* 0266849C */
void hdEnemyHp_c::displayExecute() {
    if (!sEnemyActive) changeState(&sEnemyWait);
    hdAnimSet_setFrame(animSet(), 0, 0, sEnemyHealth);
    hdEnemyHpPane_c* pane = mpHealthPane;
    f32 z = pane->mZ;
    u8 flags = pane->mFlags;
    pane->mX = sEnemyX;
    pane->mY = sEnemyY;
    pane->mFlags = flags|0x10;
    pane->mZ = z;
    vCalcLayout();
}
/* 0266855C */
hdEnemyHp_c* hdEnemyHp_c::construct(hdEnemyHp_c* object) {
    if (!object) { object = (hdEnemyHp_c*)operator_new(sizeof(hdEnemyHp_c)); if (!object) return NULL; }
    enemyConstructBase(object);
    object->mStateHolder.mHolderVtable = &sEnemyHolderTable;
    object->mVtable = &sEnemyTable;
    object->mSecondaryVtable = &sEnemySecondaryTable;
    object->mpHealthPane = NULL;
    return object;
}
/* 026685D0 */
u32 hdEnemyHp_c::setup(u32 first, u32 second, u32 third) {
    if (!enemySetupBase(this, &sEnemyName, first, second, third)) return 0;
    hdEnemyHpPane_c* root = (hdEnemyHpPane_c*)mpLayout->lyt()->mpRootPane.get();
    hdEnemyHpPaneTable_c* table = root->mVtable;
    sEnemyPaneName.assureTermination();
    mpHealthPane = gabi::call_ptr<hdEnemyHpPane_c*>(table->mFindPane.get(), root, sEnemyPaneName.mText.get(), 1);
    startState(&sEnemyWait);
    return 1;
}
/* 026686D0 */
u32 hdEnemyHp_c::load(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sEnemyUi->mHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 1, 1, desc)) return 0;
    u32 pane = sEnemyPaneIndex;
    enemyBindAnimation(animSet(), 0, &sEnemyAnimationName, &sEnemyAnimationPane[pane]);
    return 1;
}
/* 026687BC */
static void __sinit_hd_screen_enemy_hp_cpp() {
    sEnemyInitWords[2] = 0; sEnemyInitWords[0] = 0; sEnemyInitWords[3] = 0; sEnemyInitWords[1] = 0;
    enemyRegisterStatic(&sEnemyRegistrations[0]);
    sEnemyFrame = sEnemyInitialFrame; sEnemyRate = sEnemyInitialRate;
    enemyConstructByte(&sEnemyReady);
    enemyRegisterStatic(&sEnemyRegistrations[3]);
    enemyConstructBool(&sEnemyEnabled);
    enemyRegisterStatic(&sEnemyRegistrations[6]);
    sEnemyName.mVtable = &sEnemyStringTable;
    sEnemyName.mText.v = gabi::guest_addr("EnemyHP_00");
    sEnemyAnimationName.mVtable = &sEnemyStringTable;
    sEnemyAnimationName.mText.v = gabi::guest_addr("BossHP");
    sEnemyAnimationPane[0].mVtable = &sEnemyStringTable;
    sEnemyPaneName.mVtable = &sEnemyStringTable;
    u32 guard = sEnemyStateGuard;
    sEnemyAnimationPane[0].mText.v = gabi::guest_addr("G_BossHP_00");
    sEnemyPaneName.mText.v = gabi::guest_addr("N_EnemyHP_00");
    u32 number;
    if (guard) number = sEnemyStateCounter;
    else { sEnemyStateGuard = 1; number = 0; }
    sEnemyWait.setup(++number, "::StateID_Wait", &sEnemyStateTable, 0x19, &sEnemyNullState);
    sEnemyStateCounter = number+1;
    sEnemyDisplay.setup(++number, "::StateID_Disp", &sEnemyStateTable, 0x1c, &sEnemyNullState);
}
/* WWHD glue
data sEnemyTable 100F4310 nocheck
data sEnemyHolderTable 100F4408 nocheck
data sEnemySecondaryTable 100F4418 nocheck
data sEnemyWait 10493F90 nocheck
data sEnemyDisplay 10493FC0 nocheck
data sEnemyName 10493FF0 nocheck
data sEnemyAnimationName 10493F64 nocheck
data sEnemyAnimationPane 10493F5C nocheck
data sEnemyPaneName 10493F6C nocheck
data sEnemyStringTable 100F4208 nocheck
data sEnemyStateTable 100F4268 nocheck
data sEnemyNullState 101FF32C nocheck
data sEnemyStateGuard 101FD8E4 nocheck
data sEnemyStateCounter 101FDD50 nocheck
data sEnemyInitWords 10493F80 nocheck
data sEnemyRegistrations 101F5E90 nocheck
data sEnemyPaneIndex 100F4204 nocheck
data sEnemyActive 1047B09C nocheck
data sEnemyReady 10493F7C nocheck
data sEnemyEnabled 10493F7D nocheck
data sEnemyX 1047B03C nocheck
data sEnemyY 1047B040 nocheck
data sEnemyHealth 1047B068 nocheck
data sEnemyInitialFrame 100F42B4 nocheck
data sEnemyInitialRate 100F42B8 nocheck
data sEnemyFrame 10493F74 nocheck
data sEnemyRate 10493F78 nocheck
data sEnemyUi 1018C404 nocheck
str 100F42BC "BossHP" in 026687BC
str 100F42C4 "G_BossHP_00" in 026687BC
str 100F42D0 "N_EnemyHP_00" in 026687BC
str 100F42E0 "EnemyHP_00" in 026687BC
str 100F42EC "::StateID_Wait" in 026687BC
str 100F42FC "::StateID_Disp" in 026687BC
ext 02700ADC void enemyConstructBase(NoControlPart_l*)
ext 02700A70 void enemyDestroyBase(NoControlPart_l*, u32)
ext 02700B48 u32 enemySetupBase(NoControlPart_l*, hdUiSafeString*, u32, u32, u32)
ext 02004E04 void enemyBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
ext 028F026C void enemyRegisterStatic(void*)
ext 028ED6F8 void enemyConstructByte(void*)
ext 028EAB2C void enemyConstructBool(void*)
*/
