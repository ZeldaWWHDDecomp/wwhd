/** HD Fairy chart screen: lifecycle and layout configuration. Private game-derived source. */
#include "d/hd_screen_fairy.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdMapChartPaneNames_l sFairyPaneNames;
static hdMapChartArchives_l sFairyArchives;
static hdUiSafeString sFairyLayoutName;
static hdMapChartAnimationNames_l sFairyAnimationGroups;
static hdMapChartAnimationName_l sFairyAnimationNames;
static hdMapChartAnimationRemap_l sFairyAnimationRemap;
static hdScreenUiSystem* sFairyUiSystem;
static hdMapChartChildNames_l sFairyChildNames;
static u32 sFairySafeStringVtable;
static hdMapChartStaticSettings_l sFairySettings;
static hdMapChartSettingConstants_l sFairySettingConstants;
static hdMapChartChildNames_l sFairyArchiveAliases;
static hdMapChartRegistration_l sFairyRegistration0;
static hdMapChartRegistration_l sFairyRegistration1;
static hdMapChartRegistration_l sFairyRegistration2;
enum { CHART_ENTRY_CAPACITY = 6 };
/* 0266A530 */
static u32 fairyStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 0266A630 */
void hdFairy_c::destroy(hdFairy_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    seaChartLinkDestroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 0266A69C */
hdFairy_c* hdFairy_c::construct(hdFairy_c* self) {
    if (!self) self = static_cast<hdFairy_c*>(chartAllocate(sizeof(hdFairy_c)));
    if (self) {
        seaChartLinkConstruct(self);
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    }
    return self;
}
/* 0266A708 */
void hdFairy_c::configure() {
    mPaneNameCount = CHART_ENTRY_CAPACITY;
    mPaneNames = sFairyPaneNames.mNames;
    configureAnimations(0, 1, 0, 1);
    setupLayout();
}
/* 0266A83C */
void hdFairy_c::finalize() {
    destroyLayout();
    detachArchives();
}
/* 0266A870 */
void hdFairy_c::executeState() { chartUpdateMachine(stateMachine()); }
/* 0266A760 */
u32 hdFairy_c::load(void* context, void* resource, void* option) {
    attachArchives(sFairyArchives.mNames, 4);
    u32 load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sFairyLayoutName, accessor, context, resource, option)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    configure();
    return 1;
}
/* 0266A980 */
u32 hdFairy_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sFairyUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdAnimSet* animations = mpLayout->animSet();
        u32 animation = sFairyAnimationRemap.mIndices[i];
        hdUiSafeString* group = &sFairyAnimationGroups.mNames[i];
        // The remap is an unchecked guest index, including 32-bit address wrap.
        hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sFairyAnimationNames) + animation * sizeof(hdUiSafeString));
        chartBindAnimation(animations, i, group, animationName);
    }
    vCalcLayout();
    return 1;
}
/* 0266A878 */
u32 hdFairy_c::createNamedChild(hdMapChartDesc_l* desc) {
    enum { CHILD_NAME_COUNT = 3, STRING_COMPARE_BUDGET = 0x40001 };
    hdUiSafeString* name = &desc->mName;
    for (u32 i = 0; i < CHILD_NAME_COUNT; ++i) {
        name->assureTermination();
        name->assureTermination();
        hdUiSafeString* expected = &sFairyChildNames.mNames[i];
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
/* 0266AAAC */
static void fairyInitialize() {
    sFairySettings.mRegistrationWork[3] = 0;
    sFairySettings.mRegistrationWork[2] = 0;
    sFairySettings.mRegistrationWork[1] = 0;
    sFairySettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sFairyRegistration0);
    u32 minimumAngleBits = sFairySettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sFairySettingConstants.mMaximumAngle.mBits;
    sFairySettings.mMinimumAngle.mBits = minimumAngleBits;
    sFairySettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sFairySettings.mSignedHeader);
    chartRegisterGlobal(&sFairyRegistration1);
    chartInitializeUnsignedHeader(&sFairySettings.mUnsignedHeader);
    chartRegisterGlobal(&sFairyRegistration2);
    u32 farDistanceBits = sFairySettingConstants.mFarDistance.mBits;
    u32 nearDistanceBits = sFairySettingConstants.mNearDistance.mBits;
    sFairySettings.mDistances[0].mBits = farDistanceBits;
    sFairySettings.mDistances[1].mBits = farDistanceBits;
    sFairyPaneNames.mNames[0].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_01");
    sFairyPaneNames.mNames[1].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairySettings.mDistances[3].mBits = nearDistanceBits;
    sFairyPaneNames.mNames[1].mText.v = gabi::guest_addr("N_Grid_00");
    sFairyPaneNames.mNames[2].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyPaneNames.mNames[2].mText.v = gabi::guest_addr("N_Grid_01");
    sFairyPaneNames.mNames[3].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyPaneNames.mNames[3].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sFairyPaneNames.mNames[4].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyPaneNames.mNames[4].mText.v = gabi::guest_addr("L_BtnBack_00");
    sFairyAnimationNames.mName.mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyAnimationNames.mName.mText.v = gabi::guest_addr("G_All_00");
    sFairySettings.mDistances[2].mBits = nearDistanceBits;
    sFairyArchiveAliases.mNames[1].mText.v = gabi::guest_addr("BtnBack_00");
    sFairyChildNames.mNames[0].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyChildNames.mNames[0].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sFairyAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyAnimationGroups.mNames[0].mText.v = gabi::guest_addr("InAll");
    sFairyChildNames.mNames[1].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyAnimationGroups.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sFairyArchiveAliases.mNames[0].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyChildNames.mNames[1].mText.v = gabi::guest_addr("L_BtnBack_00");
    sFairyPaneNames.mNames[5].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyPaneNames.mNames[5].mText.v = gabi::guest_addr("L_LinkPos_00");
    sFairyArchiveAliases.mNames[0].mText.v = gabi::guest_addr("MapAreaNow_00");
    sFairyLayoutName.mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyArchiveAliases.mNames[2].mText.v = gabi::guest_addr("LinkPos_00");
    sFairyChildNames.mNames[2].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyChildNames.mNames[2].mText.v = gabi::guest_addr("L_LinkPos_00");
    sFairyArchives.mNames[0].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyArchives.mNames[0].mText.v = gabi::guest_addr("Fairy_00");
    sFairyArchives.mNames[1].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyArchives.mNames[1].mText.v = gabi::guest_addr("MapAreaNow_00");
    sFairyArchives.mNames[2].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyArchives.mNames[2].mText.v = gabi::guest_addr("BtnBack_00");
    sFairyArchives.mNames[3].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyLayoutName.mText.v = gabi::guest_addr("Fairy_00");
    sFairyArchiveAliases.mNames[2].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyArchiveAliases.mNames[1].mVtable.v = gabi::ea(&sFairySafeStringVtable);
    sFairyArchives.mNames[3].mText.v = gabi::guest_addr("LinkPos_00");
}
/* WWHD glue
 data sScreenVtable 100F4BC0 nocheck
 data sHolderVtable 100F4CC0 nocheck
 data sSecondaryVtable 100F4CD0 nocheck
 data sFairySafeStringVtable 100F4AEC nocheck
data sFairySettings 10494244
data sFairySettingConstants 100F4B0C
data sFairyArchiveAliases 104942C8
data sFairyRegistration0 101F5F20
data sFairyRegistration1 101F5F2C
data sFairyRegistration2 101F5F38
str 100F4B64 "N_All_01" in 0266AAAC
str 100F4B3C "N_Grid_00" in 0266AAAC
str 100F4B48 "N_Grid_01" in 0266AAAC
str 100F4B2C "L_MapAreaNow_00" in 0266AAAC
str 100F4B70 "L_BtnBack_00" in 0266AAAC
str 100F4B90 "G_All_00" in 0266AAAC
str 100F4B9C "BtnBack_00" in 0266AAAC
str 100F4B1C "InAll" in 0266AAAC
str 100F4B24 "OutAll" in 0266AAAC
str 100F4B80 "L_LinkPos_00" in 0266AAAC
str 100F4B54 "MapAreaNow_00" in 0266AAAC
str 100F4BA8 "LinkPos_00" in 0266AAAC
str 100F4BB4 "Fairy_00" in 0266AAAC
data sFairyPaneNames 10494270
data sFairyArchives 104942E0
data sFairyLayoutName 10494300
data sFairyAnimationGroups 104942A0
data sFairyAnimationNames 1049423C
data sFairyAnimationRemap 100F4AD0
data sFairyUiSystem 1018C404
data sFairyChildNames 104942B0
*/
