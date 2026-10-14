/** HD Moon chart screen: lifecycle and layout configuration. Private game-derived source. */
#include "d/hd_screen_moon.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdMapChartPaneNames_l sMoonPaneNames;
static hdMapChartArchives_l sMoonArchives;
static hdUiSafeString sMoonLayoutName;
static hdMapChartAnimationNames_l sMoonAnimationGroups;
static hdMapChartAnimationName_l sMoonAnimationNames;
static hdMapChartAnimationRemap_l sMoonAnimationRemap;
static hdScreenUiSystem* sMoonUiSystem;
static hdMapChartChildNames_l sMoonChildNames;
static u32 sMoonSafeStringVtable;
static hdMapChartStaticSettings_l sMoonSettings;
static hdMapChartSettingConstants_l sMoonSettingConstants;
static hdMapChartChildNames_l sMoonArchiveAliases;
static hdMapChartRegistration_l sMoonRegistration0;
static hdMapChartRegistration_l sMoonRegistration1;
static hdMapChartRegistration_l sMoonRegistration2;
enum { CHART_ENTRY_CAPACITY = 6 };
/* 026AC0C0 */
u32 hdMoon_c::receiveTouch() { return 0; }
/* 026AC0CC */
u32 hdMoon_c::receiveKey() { return 0; }
/* 026AC0D4 */
u32 hdMoon_c::receivePointer() { return 0; }
/* 026AC0DC */
u32 hdMoon_c::receiveFocus() { return 0; }
/* 026AC0E8 */
u32 hdMoon_c::receiveCancel() { return 0; }
/* 026AC0F0 */
static u32 moonStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 026AC1C4 */
void hdMoon_c::destroy(hdMoon_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    seaChartLinkDestroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 026AC230 */
hdMoon_c* hdMoon_c::construct(hdMoon_c* self) {
    if (!self) self = static_cast<hdMoon_c*>(chartAllocate(sizeof(hdMoon_c)));
    if (self) {
        seaChartLinkConstruct(self);
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    }
    return self;
}
/* 026AC29C */
void hdMoon_c::configure() {
    mPaneNameCount = CHART_ENTRY_CAPACITY;
    mPaneNames = sMoonPaneNames.mNames;
    configureAnimations(0, 1, 0, 0);
    setupLayout();
}
/* 026AC3D0 */
void hdMoon_c::finalize() {
    destroyLayout();
    detachArchives();
}
/* 026AC404 */
void hdMoon_c::executeState() { chartUpdateMachine(stateMachine()); }
/* 026AC2F4 */
u32 hdMoon_c::load(void* context, void* resource, void* option) {
    attachArchives(sMoonArchives.mNames, 4);
    u32 load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sMoonLayoutName, accessor, context, resource, option)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    configure();
    return 1;
}
/* 026AC514 */
u32 hdMoon_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sMoonUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdAnimSet* animations = mpLayout->animSet();
        u32 animation = sMoonAnimationRemap.mIndices[i];
        hdUiSafeString* group = &sMoonAnimationGroups.mNames[i];
        // The remap is an unchecked guest index, including 32-bit address wrap.
        hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sMoonAnimationNames) + animation * sizeof(hdUiSafeString));
        chartBindAnimation(animations, i, group, animationName);
    }
    vCalcLayout();
    return 1;
}
/* 026AC40C */
u32 hdMoon_c::createNamedChild(hdMapChartDesc_l* desc) {
    enum { CHILD_NAME_COUNT = 3, STRING_COMPARE_BUDGET = 0x40001 };
    hdUiSafeString* name = &desc->mName;
    for (u32 i = 0; i < CHILD_NAME_COUNT; ++i) {
        name->assureTermination();
        name->assureTermination();
        hdUiSafeString* expected = &sMoonChildNames.mNames[i];
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
/* 026AC640 */
static void moonInitialize() {
    sMoonSettings.mRegistrationWork[3] = 0;
    sMoonSettings.mRegistrationWork[2] = 0;
    sMoonSettings.mRegistrationWork[1] = 0;
    sMoonSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sMoonRegistration0);
    u32 minimumAngleBits = sMoonSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sMoonSettingConstants.mMaximumAngle.mBits;
    sMoonSettings.mMinimumAngle.mBits = minimumAngleBits;
    sMoonSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sMoonSettings.mSignedHeader);
    chartRegisterGlobal(&sMoonRegistration1);
    chartInitializeUnsignedHeader(&sMoonSettings.mUnsignedHeader);
    chartRegisterGlobal(&sMoonRegistration2);
    u32 farDistanceBits = sMoonSettingConstants.mFarDistance.mBits;
    u32 nearDistanceBits = sMoonSettingConstants.mNearDistance.mBits;
    sMoonSettings.mDistances[0].mBits = farDistanceBits;
    sMoonSettings.mDistances[1].mBits = farDistanceBits;
    sMoonPaneNames.mNames[0].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_01");
    sMoonPaneNames.mNames[1].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonSettings.mDistances[3].mBits = nearDistanceBits;
    sMoonPaneNames.mNames[1].mText.v = gabi::guest_addr("N_Grid_00");
    sMoonPaneNames.mNames[2].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonPaneNames.mNames[2].mText.v = gabi::guest_addr("N_Grid_01");
    sMoonPaneNames.mNames[3].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonPaneNames.mNames[3].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sMoonPaneNames.mNames[4].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonPaneNames.mNames[4].mText.v = gabi::guest_addr("L_BtnBack_00");
    sMoonAnimationNames.mName.mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonAnimationNames.mName.mText.v = gabi::guest_addr("G_All_00");
    sMoonSettings.mDistances[2].mBits = nearDistanceBits;
    sMoonArchiveAliases.mNames[1].mText.v = gabi::guest_addr("MapAreaNow_00");
    sMoonChildNames.mNames[0].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonChildNames.mNames[0].mText.v = gabi::guest_addr("L_BtnBack_00");
    sMoonAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonAnimationGroups.mNames[0].mText.v = gabi::guest_addr("InAll");
    sMoonChildNames.mNames[1].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonAnimationGroups.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sMoonArchiveAliases.mNames[0].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonChildNames.mNames[1].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sMoonPaneNames.mNames[5].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonPaneNames.mNames[5].mText.v = gabi::guest_addr("L_LinkPos_00");
    sMoonArchiveAliases.mNames[0].mText.v = gabi::guest_addr("BtnBack_00");
    sMoonLayoutName.mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonArchiveAliases.mNames[2].mText.v = gabi::guest_addr("LinkPos_00");
    sMoonChildNames.mNames[2].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonChildNames.mNames[2].mText.v = gabi::guest_addr("L_LinkPos_00");
    sMoonArchives.mNames[0].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonArchives.mNames[0].mText.v = gabi::guest_addr("Moon_00");
    sMoonArchives.mNames[1].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonArchives.mNames[1].mText.v = gabi::guest_addr("BtnBack_00");
    sMoonArchives.mNames[2].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonArchives.mNames[2].mText.v = gabi::guest_addr("MapAreaNow_00");
    sMoonArchives.mNames[3].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonLayoutName.mText.v = gabi::guest_addr("Moon_00");
    sMoonArchiveAliases.mNames[2].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonArchiveAliases.mNames[1].mVtable.v = gabi::ea(&sMoonSafeStringVtable);
    sMoonArchives.mNames[3].mText.v = gabi::guest_addr("LinkPos_00");
}
/* WWHD glue
 data sScreenVtable 100FC968 nocheck
 data sHolderVtable 100FCA68 nocheck
 data sSecondaryVtable 100FCA78 nocheck
 data sMoonSafeStringVtable 100FC894 nocheck
data sMoonSettings 10497BE0
data sMoonSettingConstants 100FC8B4
data sMoonArchiveAliases 10497C64
data sMoonRegistration0 101F64C8
data sMoonRegistration1 101F64D4
data sMoonRegistration2 101F64E0
str 100FC914 "N_All_01" in 026AC640
str 100FC8EC "N_Grid_00" in 026AC640
str 100FC8F8 "N_Grid_01" in 026AC640
str 100FC8DC "L_MapAreaNow_00" in 026AC640
str 100FC920 "L_BtnBack_00" in 026AC640
str 100FC940 "G_All_00" in 026AC640
str 100FC904 "MapAreaNow_00" in 026AC640
str 100FC8CC "InAll" in 026AC640
str 100FC8D4 "OutAll" in 026AC640
str 100FC930 "L_LinkPos_00" in 026AC640
str 100FC94C "BtnBack_00" in 026AC640
str 100FC958 "LinkPos_00" in 026AC640
str 100FC8C4 "Moon_00" in 026AC640
data sMoonPaneNames 10497C0C
data sMoonArchives 10497C7C
data sMoonLayoutName 10497C9C
data sMoonAnimationGroups 10497C3C
data sMoonAnimationNames 10497BD8
data sMoonAnimationRemap 100FC878
data sMoonUiSystem 1018C404
data sMoonChildNames 10497C4C
*/
