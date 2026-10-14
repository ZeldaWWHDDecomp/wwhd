/** HD HeartSea chart screen: lifecycle and layout configuration. Private game-derived source. */
#include "d/hd_screen_heart_sea.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdMapChartPaneNames_l sHeartSeaPaneNames;
static hdMapChartArchives_l sHeartSeaArchives;
static hdUiSafeString sHeartSeaLayoutName;
static hdMapChartAnimationNames_l sHeartSeaAnimationGroups;
static hdMapChartAnimationName_l sHeartSeaAnimationNames;
static hdMapChartAnimationRemap_l sHeartSeaAnimationRemap;
static hdScreenUiSystem* sHeartSeaUiSystem;
static hdMapChartChildNames_l sHeartSeaChildNames;
static u32 sHeartSeaSafeStringVtable;
static hdMapChartStaticSettings_l sHeartSeaSettings;
static hdMapChartSettingConstants_l sHeartSeaSettingConstants;
static hdMapChartChildNames_l sHeartSeaArchiveAliases;
static hdMapChartRegistration_l sHeartSeaRegistration0;
static hdMapChartRegistration_l sHeartSeaRegistration1;
static hdMapChartRegistration_l sHeartSeaRegistration2;
enum { CHART_ENTRY_CAPACITY = 6 };
/* 0266EDDC */
void hdHeartSea_c::destroy(hdHeartSea_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    seaChartLinkDestroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 0266EE48 */
hdHeartSea_c* hdHeartSea_c::construct(hdHeartSea_c* self) {
    if (!self) self = static_cast<hdHeartSea_c*>(chartAllocate(sizeof(hdHeartSea_c)));
    if (self) {
        seaChartLinkConstruct(self);
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    }
    return self;
}
/* 0266EEB4 */
void hdHeartSea_c::configure() {
    mPaneNameCount = CHART_ENTRY_CAPACITY;
    mPaneNames = sHeartSeaPaneNames.mNames;
    configureAnimations(0, 1, 0, 1);
    setupLayout();
}
/* 0266EFE8 */
void hdHeartSea_c::finalize() {
    destroyLayout();
    detachArchives();
}
/* 0266F01C */
void hdHeartSea_c::executeState() { chartUpdateMachine(stateMachine()); }
/* 0266EF0C */
u32 hdHeartSea_c::load(void* context, void* resource, void* option) {
    attachArchives(sHeartSeaArchives.mNames, 4);
    u32 load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sHeartSeaLayoutName, accessor, context, resource, option)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    configure();
    return 1;
}
/* 0266F12C */
u32 hdHeartSea_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sHeartSeaUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdAnimSet* animations = mpLayout->animSet();
        u32 animation = sHeartSeaAnimationRemap.mIndices[i];
        hdUiSafeString* group = &sHeartSeaAnimationGroups.mNames[i];
        // The remap is an unchecked guest index, including 32-bit address wrap.
        hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sHeartSeaAnimationNames) + animation * sizeof(hdUiSafeString));
        chartBindAnimation(animations, i, group, animationName);
    }
    vCalcLayout();
    return 1;
}
/* 0266F024 */
u32 hdHeartSea_c::createNamedChild(hdMapChartDesc_l* desc) {
    enum { CHILD_NAME_COUNT = 3, STRING_COMPARE_BUDGET = 0x40001 };
    hdUiSafeString* name = &desc->mName;
    for (u32 i = 0; i < CHILD_NAME_COUNT; ++i) {
        name->assureTermination();
        name->assureTermination();
        hdUiSafeString* expected = &sHeartSeaChildNames.mNames[i];
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
/* 0266F258 */
static void heartSeaInitialize() {
    sHeartSeaSettings.mRegistrationWork[3] = 0;
    sHeartSeaSettings.mRegistrationWork[2] = 0;
    sHeartSeaSettings.mRegistrationWork[1] = 0;
    sHeartSeaSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sHeartSeaRegistration0);
    u32 minimumAngleBits = sHeartSeaSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sHeartSeaSettingConstants.mMaximumAngle.mBits;
    sHeartSeaSettings.mMinimumAngle.mBits = minimumAngleBits;
    sHeartSeaSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sHeartSeaSettings.mSignedHeader);
    chartRegisterGlobal(&sHeartSeaRegistration1);
    chartInitializeUnsignedHeader(&sHeartSeaSettings.mUnsignedHeader);
    chartRegisterGlobal(&sHeartSeaRegistration2);
    u32 farDistanceBits = sHeartSeaSettingConstants.mFarDistance.mBits;
    u32 nearDistanceBits = sHeartSeaSettingConstants.mNearDistance.mBits;
    sHeartSeaSettings.mDistances[0].mBits = farDistanceBits;
    sHeartSeaSettings.mDistances[1].mBits = farDistanceBits;
    sHeartSeaPaneNames.mNames[0].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_01");
    sHeartSeaPaneNames.mNames[1].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaSettings.mDistances[3].mBits = nearDistanceBits;
    sHeartSeaPaneNames.mNames[1].mText.v = gabi::guest_addr("N_Grid_00");
    sHeartSeaPaneNames.mNames[2].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaPaneNames.mNames[2].mText.v = gabi::guest_addr("N_Grid_01");
    sHeartSeaPaneNames.mNames[3].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaPaneNames.mNames[3].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sHeartSeaPaneNames.mNames[4].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaPaneNames.mNames[4].mText.v = gabi::guest_addr("L_BtnBack_00");
    sHeartSeaAnimationNames.mName.mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaAnimationNames.mName.mText.v = gabi::guest_addr("G_All_00");
    sHeartSeaSettings.mDistances[2].mBits = nearDistanceBits;
    sHeartSeaArchiveAliases.mNames[1].mText.v = gabi::guest_addr("BtnBack_00");
    sHeartSeaChildNames.mNames[0].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaChildNames.mNames[0].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sHeartSeaAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaAnimationGroups.mNames[0].mText.v = gabi::guest_addr("InAll");
    sHeartSeaChildNames.mNames[1].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaAnimationGroups.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sHeartSeaArchiveAliases.mNames[0].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaChildNames.mNames[1].mText.v = gabi::guest_addr("L_BtnBack_00");
    sHeartSeaPaneNames.mNames[5].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaPaneNames.mNames[5].mText.v = gabi::guest_addr("L_LinkPos_00");
    sHeartSeaArchiveAliases.mNames[0].mText.v = gabi::guest_addr("MapAreaNow_00");
    sHeartSeaLayoutName.mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaArchiveAliases.mNames[2].mText.v = gabi::guest_addr("LinkPos_00");
    sHeartSeaChildNames.mNames[2].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaChildNames.mNames[2].mText.v = gabi::guest_addr("L_LinkPos_00");
    sHeartSeaArchives.mNames[0].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaArchives.mNames[0].mText.v = gabi::guest_addr("HeartSea_00");
    sHeartSeaArchives.mNames[1].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaArchives.mNames[1].mText.v = gabi::guest_addr("MapAreaNow_00");
    sHeartSeaArchives.mNames[2].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaArchives.mNames[2].mText.v = gabi::guest_addr("BtnBack_00");
    sHeartSeaArchives.mNames[3].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaLayoutName.mText.v = gabi::guest_addr("HeartSea_00");
    sHeartSeaArchiveAliases.mNames[2].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaArchiveAliases.mNames[1].mVtable.v = gabi::ea(&sHeartSeaSafeStringVtable);
    sHeartSeaArchives.mNames[3].mText.v = gabi::guest_addr("LinkPos_00");
}
/* WWHD glue
 data sScreenVtable 100F5C98 nocheck
 data sHolderVtable 100F5D98 nocheck
 data sSecondaryVtable 100F5DA8 nocheck
 data sHeartSeaSafeStringVtable 100F5BC4 nocheck
data sHeartSeaSettings 10494960
data sHeartSeaSettingConstants 100F5BE4
data sHeartSeaArchiveAliases 104949E4
data sHeartSeaRegistration0 101F601C
data sHeartSeaRegistration1 101F6028
data sHeartSeaRegistration2 101F6034
str 100F5C48 "N_All_01" in 0266F258
str 100F5C20 "N_Grid_00" in 0266F258
str 100F5C2C "N_Grid_01" in 0266F258
str 100F5C04 "L_MapAreaNow_00" in 0266F258
str 100F5C54 "L_BtnBack_00" in 0266F258
str 100F5C74 "G_All_00" in 0266F258
str 100F5C80 "BtnBack_00" in 0266F258
str 100F5BF4 "InAll" in 0266F258
str 100F5BFC "OutAll" in 0266F258
str 100F5C64 "L_LinkPos_00" in 0266F258
str 100F5C38 "MapAreaNow_00" in 0266F258
str 100F5C8C "LinkPos_00" in 0266F258
str 100F5C14 "HeartSea_00" in 0266F258
data sHeartSeaPaneNames 1049498C
data sHeartSeaArchives 104949FC
data sHeartSeaLayoutName 10494A1C
data sHeartSeaAnimationGroups 104949BC
data sHeartSeaAnimationNames 10494958
data sHeartSeaAnimationRemap 100F5BA8
data sHeartSeaUiSystem 1018C404
data sHeartSeaChildNames 104949CC
*/
