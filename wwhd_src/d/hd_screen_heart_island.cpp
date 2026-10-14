/** HD HeartIsland chart screen: lifecycle and layout configuration. Private game-derived source. */
#include "d/hd_screen_heart_island.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdMapChartPaneNames_l sHeartIslandPaneNames;
static hdMapChartArchives_l sHeartIslandArchives;
static hdUiSafeString sHeartIslandLayoutName;
static hdMapChartAnimationNames_l sHeartIslandAnimationGroups;
static hdMapChartAnimationName_l sHeartIslandAnimationNames;
static hdMapChartAnimationRemap_l sHeartIslandAnimationRemap;
static hdScreenUiSystem* sHeartIslandUiSystem;
static hdMapChartChildNames_l sHeartIslandChildNames;
static u32 sHeartIslandSafeStringVtable;
static hdMapChartStaticSettings_l sHeartIslandSettings;
static hdMapChartSettingConstants_l sHeartIslandSettingConstants;
static hdMapChartChildNames_l sHeartIslandArchiveAliases;
static hdMapChartRegistration_l sHeartIslandRegistration0;
static hdMapChartRegistration_l sHeartIslandRegistration1;
static hdMapChartRegistration_l sHeartIslandRegistration2;
enum { CHART_ENTRY_CAPACITY = 6 };
/* 0266E630 */
static u32 heartIslandStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 0266E730 */
void hdHeartIsland_c::destroy(hdHeartIsland_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    seaChartLinkDestroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 0266E79C */
hdHeartIsland_c* hdHeartIsland_c::construct(hdHeartIsland_c* self) {
    if (!self) self = static_cast<hdHeartIsland_c*>(chartAllocate(sizeof(hdHeartIsland_c)));
    if (self) {
        seaChartLinkConstruct(self);
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    }
    return self;
}
/* 0266E808 */
void hdHeartIsland_c::configure() {
    mPaneNameCount = CHART_ENTRY_CAPACITY;
    mPaneNames = sHeartIslandPaneNames.mNames;
    configureAnimations(0, 1, 0, 0);
    setupLayout();
}
/* 0266E93C */
void hdHeartIsland_c::finalize() {
    destroyLayout();
    detachArchives();
}
/* 0266E970 */
void hdHeartIsland_c::executeState() { chartUpdateMachine(stateMachine()); }
/* 0266E860 */
u32 hdHeartIsland_c::load(void* context, void* resource, void* option) {
    attachArchives(sHeartIslandArchives.mNames, 4);
    u32 load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sHeartIslandLayoutName, accessor, context, resource, option)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    configure();
    return 1;
}
/* 0266EA80 */
u32 hdHeartIsland_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sHeartIslandUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdAnimSet* animations = mpLayout->animSet();
        u32 animation = sHeartIslandAnimationRemap.mIndices[i];
        hdUiSafeString* group = &sHeartIslandAnimationGroups.mNames[i];
        // The remap is an unchecked guest index, including 32-bit address wrap.
        hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sHeartIslandAnimationNames) + animation * sizeof(hdUiSafeString));
        chartBindAnimation(animations, i, group, animationName);
    }
    vCalcLayout();
    return 1;
}
/* 0266E978 */
u32 hdHeartIsland_c::createNamedChild(hdMapChartDesc_l* desc) {
    enum { CHILD_NAME_COUNT = 3, STRING_COMPARE_BUDGET = 0x40001 };
    hdUiSafeString* name = &desc->mName;
    for (u32 i = 0; i < CHILD_NAME_COUNT; ++i) {
        name->assureTermination();
        name->assureTermination();
        hdUiSafeString* expected = &sHeartIslandChildNames.mNames[i];
        be<u8>* capturedText = name->mText;
        expected->assureTermination();
        bool match = capturedText == expected->mText.get();
        if (!match) {
            // The byte scan reloads both pointers after the callback.
            be<u8>* text = name->mText;
            be<u8>* wanted = expected->mText;
            for (u32 j = 0; j < STRING_COMPARE_BUDGET; ++j) {
                u8 a = *gabi::at<be<u8>>(gabi::ea(text) + j);
                u8 b = *gabi::at<be<u8>>(gabi::ea(wanted) + j);
                if (a != b) break;
                if (a == 0) { match = true; break; }
            }
        }
        if (match) return createChild(i, gabi::at<hdPartsDesc_c>(gabi::ea(desc)));
    }
    return 1;
}
/* 0266EBAC */
static void heartIslandInitialize() {
    sHeartIslandSettings.mRegistrationWork[3] = 0;
    sHeartIslandSettings.mRegistrationWork[2] = 0;
    sHeartIslandSettings.mRegistrationWork[1] = 0;
    sHeartIslandSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sHeartIslandRegistration0);
    u32 minimumAngleBits = sHeartIslandSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sHeartIslandSettingConstants.mMaximumAngle.mBits;
    sHeartIslandSettings.mMinimumAngle.mBits = minimumAngleBits;
    sHeartIslandSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sHeartIslandSettings.mSignedHeader);
    chartRegisterGlobal(&sHeartIslandRegistration1);
    chartInitializeUnsignedHeader(&sHeartIslandSettings.mUnsignedHeader);
    chartRegisterGlobal(&sHeartIslandRegistration2);
    u32 farDistanceBits = sHeartIslandSettingConstants.mFarDistance.mBits;
    u32 nearDistanceBits = sHeartIslandSettingConstants.mNearDistance.mBits;
    sHeartIslandSettings.mDistances[0].mBits = farDistanceBits;
    sHeartIslandSettings.mDistances[1].mBits = farDistanceBits;
    sHeartIslandPaneNames.mNames[0].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_01");
    sHeartIslandPaneNames.mNames[1].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandSettings.mDistances[3].mBits = nearDistanceBits;
    sHeartIslandPaneNames.mNames[1].mText.v = gabi::guest_addr("N_Grid_00");
    sHeartIslandPaneNames.mNames[2].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandPaneNames.mNames[2].mText.v = gabi::guest_addr("N_Grid_01");
    sHeartIslandPaneNames.mNames[3].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandPaneNames.mNames[3].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sHeartIslandPaneNames.mNames[4].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandPaneNames.mNames[4].mText.v = gabi::guest_addr("L_BtnBack_00");
    sHeartIslandAnimationNames.mName.mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandAnimationNames.mName.mText.v = gabi::guest_addr("G_All_00");
    sHeartIslandSettings.mDistances[2].mBits = nearDistanceBits;
    sHeartIslandArchiveAliases.mNames[1].mText.v = gabi::guest_addr("MapAreaNow_00");
    sHeartIslandChildNames.mNames[0].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandChildNames.mNames[0].mText.v = gabi::guest_addr("L_BtnBack_00");
    sHeartIslandAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandAnimationGroups.mNames[0].mText.v = gabi::guest_addr("InAll");
    sHeartIslandChildNames.mNames[1].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandAnimationGroups.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sHeartIslandArchiveAliases.mNames[0].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandChildNames.mNames[1].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sHeartIslandPaneNames.mNames[5].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandPaneNames.mNames[5].mText.v = gabi::guest_addr("L_LinkPos_00");
    sHeartIslandArchiveAliases.mNames[0].mText.v = gabi::guest_addr("BtnBack_00");
    sHeartIslandLayoutName.mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandArchiveAliases.mNames[2].mText.v = gabi::guest_addr("LinkPos_00");
    sHeartIslandChildNames.mNames[2].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandChildNames.mNames[2].mText.v = gabi::guest_addr("L_LinkPos_00");
    sHeartIslandArchives.mNames[0].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandArchives.mNames[0].mText.v = gabi::guest_addr("HeartIsland_00");
    sHeartIslandArchives.mNames[1].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandArchives.mNames[1].mText.v = gabi::guest_addr("BtnBack_00");
    sHeartIslandArchives.mNames[2].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandArchives.mNames[2].mText.v = gabi::guest_addr("MapAreaNow_00");
    sHeartIslandArchives.mNames[3].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandLayoutName.mText.v = gabi::guest_addr("HeartIsland_00");
    sHeartIslandArchiveAliases.mNames[2].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandArchiveAliases.mNames[1].mVtable.v = gabi::ea(&sHeartIslandSafeStringVtable);
    sHeartIslandArchives.mNames[3].mText.v = gabi::guest_addr("LinkPos_00");
}
/* WWHD glue
 data sScreenVtable 100F5A88 nocheck
 data sHolderVtable 100F5B88 nocheck
 data sSecondaryVtable 100F5B98 nocheck
 data sHeartIslandSafeStringVtable 100F59AC nocheck
data sHeartIslandSettings 10494894
data sHeartIslandSettingConstants 100F59CC
data sHeartIslandArchiveAliases 10494918
data sHeartIslandRegistration0 101F5FF8
data sHeartIslandRegistration1 101F6004
data sHeartIslandRegistration2 101F6010
str 100F5A24 "N_All_01" in 0266EBAC
str 100F59FC "N_Grid_00" in 0266EBAC
str 100F5A08 "N_Grid_01" in 0266EBAC
str 100F59EC "L_MapAreaNow_00" in 0266EBAC
str 100F5A30 "L_BtnBack_00" in 0266EBAC
str 100F5A50 "G_All_00" in 0266EBAC
str 100F5A14 "MapAreaNow_00" in 0266EBAC
str 100F59DC "InAll" in 0266EBAC
str 100F59E4 "OutAll" in 0266EBAC
str 100F5A40 "L_LinkPos_00" in 0266EBAC
str 100F5A5C "BtnBack_00" in 0266EBAC
str 100F5A68 "LinkPos_00" in 0266EBAC
str 100F5A74 "HeartIsland_00" in 0266EBAC
data sHeartIslandPaneNames 104948C0
data sHeartIslandArchives 10494930
data sHeartIslandLayoutName 10494950
data sHeartIslandAnimationGroups 104948F0
data sHeartIslandAnimationNames 1049488C
data sHeartIslandAnimationRemap 100F5990
data sHeartIslandUiSystem 1018C404
data sHeartIslandChildNames 10494900
*/
