/** HD BtnDungeonItemIcon_00 and IconChange animation. Private game-derived source. */
#include "d/hd_screen_btn_dungeon_item_icon.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdUiSafeString sDungeonIconLayoutName;
static hdUiSafeString sDungeonIconAnimationGroup;
static hdDungeonItemAnimationNames_l sDungeonIconAnimationNames;
static hdDungeonItemAnimationRemap_l sDungeonIconAnimationRemap;
static hdScreenUiSystem* sDungeonIconUiSystem;
static f32 sDungeonIconRate;
static hdPartsStateId_c sDungeonIconState;
static hdPartsStateId_c sDungeonBlankState;
static hdDungeonButtonStaticSettings_l sDungeonIconSettings;
static hdDungeonButtonAngleConstants_l sDungeonIconSettingConstants;
static u32 sDungeonIconStateRegistryReady;
static u32 sDungeonIconStateRegistryCounter;
static hdPartsStateIdVtbl_c sDungeonIconStateTable;
static u32 sDungeonIconSafeStringVtable;
static hdMapChartRegistration_l sDungeonIconRegistration0;
static hdMapChartRegistration_l sDungeonIconRegistration1;
static hdMapChartRegistration_l sDungeonIconRegistration2;
static hdPartsStateId_c sDungeonIconChangeState;
static hdStateId sDungeonIconNullState;
enum { ICON_CHANGE_ANIMATION = 5, ICON_ANIMATION_COUNT = 6, ICON_GROUP_COUNT = 2, BUTTON_SELECT_ANIMATION = 3 };
/* 0262E620 */
u32 hdDungeonItemIcon_c::receiveButton() { return 0; }
/* 0262E628 */
static u32 dungeonIconStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 0262E728 */
void hdDungeonItemIcon_c::destroy(hdDungeonItemIcon_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    self->noDecideButton()->destroy(0);
    if (flags & 1) operator_delete(self);
}
/* 0262E794 */
void hdDungeonItemIcon_c::playIconChange() { hdAnimSet_play(animSet(), ICON_CHANGE_ANIMATION, 1, sDungeonIconRate); }
/* 0262E7B0 */
void hdDungeonItemIcon_c::calcLayout() { vCalcLayout(); }
/* 0262E7C0 */
hdDungeonItemIcon_c* hdDungeonItemIcon_c::construct(hdDungeonItemIcon_c* self) {
    if (!self) self = static_cast<hdDungeonItemIcon_c*>(dungeonButtonAllocate(sizeof(hdDungeonItemIcon_c)));
    if (self) {
        NoDecideButton_l::construct(self->noDecideButton());
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    }
    return self;
}
/* 0262E82C */
u32 hdDungeonItemIcon_c::load(void* context, void* resource, void* option) {
    return noDecideButton()->create(&sDungeonIconLayoutName, context, gabi::ea(resource), gabi::ea(option));
}
/* 0262E844 */
u32 hdDungeonItemIcon_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sDungeonIconUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, ICON_ANIMATION_COUNT, ICON_GROUP_COUNT, desc)) return 0;
    u32 animation = sDungeonIconAnimationRemap.mIndex;
    hdAnimSet* set = mpLayout->animSet();
    hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sDungeonIconAnimationNames) + animation * sizeof(hdUiSafeString));
    dungeonButtonBindAnimation(set, ICON_CHANGE_ANIMATION, &sDungeonIconAnimationGroup, animationName);
    return 1;
}
/* 0262E930 */
void hdDungeonItemIcon_c::setIcon(s32 icon) { hdAnimSet_setFrame(animSet(), ICON_CHANGE_ANIMATION, 1, f32(icon)); }
/* 0262E984 */
void hdDungeonItemIcon_c::enterIcon() {
    dungeonIconAnimationOperationD4(animSet(), BUTTON_SELECT_ANIMATION, 0);
    changeState(&sDungeonIconState);
}
/* 0262E9D0 */
void hdDungeonItemIcon_c::leaveIcon() {
    hdAnimSet_operationCC(animSet(), BUTTON_SELECT_ANIMATION, 0);
    changeState(&sDungeonBlankState);
}
/* 0262EA1C */
static void dungeonIconInitialize() {
    sDungeonIconSettings.mRegistrationWork[3] = 0;
    sDungeonIconSettings.mRegistrationWork[2] = 0;
    sDungeonIconSettings.mRegistrationWork[1] = 0;
    sDungeonIconSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sDungeonIconRegistration0);
    u32 minimumAngleBits = sDungeonIconSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sDungeonIconSettingConstants.mMaximumAngle.mBits;
    sDungeonIconSettings.mMinimumAngle.mBits = minimumAngleBits;
    sDungeonIconSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sDungeonIconSettings.mSignedHeader);
    chartRegisterGlobal(&sDungeonIconRegistration1);
    chartInitializeUnsignedHeader(&sDungeonIconSettings.mUnsignedHeader);
    chartRegisterGlobal(&sDungeonIconRegistration2);
    sDungeonIconLayoutName.mVtable.v = gabi::ea(&sDungeonIconSafeStringVtable);
    sDungeonIconLayoutName.mText.v = gabi::guest_addr("BtnDungeonItemIcon_00");
    u32 registryReady = sDungeonIconStateRegistryReady;
    sDungeonIconAnimationGroup.mVtable.v = gabi::ea(&sDungeonIconSafeStringVtable);
    sDungeonIconAnimationGroup.mText.v = gabi::guest_addr("IconChange");
    sDungeonIconAnimationNames.mNames[0].mVtable.v = gabi::ea(&sDungeonIconSafeStringVtable);
    sDungeonIconAnimationNames.mNames[0].mText.v = gabi::guest_addr("G_Btn_00");
    sDungeonIconAnimationNames.mNames[1].mVtable.v = gabi::ea(&sDungeonIconSafeStringVtable);
    sDungeonIconAnimationNames.mNames[1].mText.v = gabi::guest_addr("G_IconChange_00");
    u32 number;
    if (registryReady) number = u32(sDungeonIconStateRegistryCounter) + 1;
    else { sDungeonIconStateRegistryReady = 1; number = 1; }
    sDungeonIconStateRegistryCounter = number;
    enum { ICON_ENTER_DELEGATE = 0x40 };
    sDungeonIconChangeState.setup(s32(number), "::StateID_IconChange", &sDungeonIconStateTable, ICON_ENTER_DELEGATE, &sDungeonIconNullState);
}
/* WWHD glue
 data sScreenVtable 100E7420 nocheck
 data sHolderVtable 100E7638 nocheck
 data sSecondaryVtable 100E7648 nocheck
 data sDungeonIconSafeStringVtable 100E7308 nocheck
data sDungeonIconSettings 1048F998
data sDungeonIconSettingConstants 100E73C0
data sDungeonIconStateRegistryReady 101FD8E4
data sDungeonIconStateRegistryCounter 101FDD50
data sDungeonIconStateTable 100E7368
data sDungeonIconRegistration0 101F54A0
data sDungeonIconRegistration1 101F54AC
data sDungeonIconRegistration2 101F54B8
data sDungeonIconChangeState 1048F9C4
data sDungeonIconNullState 101FF32C
str 100E73D8 "BtnDungeonItemIcon_00" in 0262EA1C
str 100E73FC "IconChange" in 0262EA1C
str 100E73F0 "G_Btn_00" in 0262EA1C
str 100E73C8 "G_IconChange_00" in 0262EA1C
str 100E7408 "::StateID_IconChange" in 0262EA1C
ext 02005708 void dungeonIconAnimationOperationD4(hdAnimSet* set, u32 entry, u32 slot)
ext 02700C9C void NoDecideButton_l::destroy(u32)
ext 02701718 NoDecideButton_l* NoDecideButton_l::construct(NoDecideButton_l*)
ext 0270178C u32 NoDecideButton_l::create(void*,void*,u32,u32)
ext 0273AD10 void* dungeonButtonAllocate(u32 size)
ext 02004E04 void dungeonButtonBindAnimation(hdAnimSet* set, u32 index, hdUiSafeString* group, hdUiSafeString* name)
data sDungeonIconLayoutName 1048F9F4
data sDungeonIconAnimationGroup 1048F990
data sDungeonIconAnimationNames 1048F9B4
data sDungeonIconAnimationRemap 100E7304
data sDungeonIconUiSystem 1018C404
data sDungeonIconRate 100E73B0
data sDungeonIconState 1049E53C
data sDungeonBlankState 1049DD60
*/
