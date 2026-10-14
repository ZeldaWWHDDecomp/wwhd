/** HD EnemyTower chart screen: lifecycle and layout configuration. Private game-derived source. */
#include "d/hd_screen_enemy_tower.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdMapChartPaneNames_l sEnemyTowerPaneNames;
static hdMapChartArchives_l sEnemyTowerArchives;
static hdUiSafeString sEnemyTowerLayoutName;
static hdMapChartAnimationNames_l sEnemyTowerAnimationGroups;
static hdMapChartAnimationName_l sEnemyTowerAnimationNames;
static hdMapChartAnimationRemap_l sEnemyTowerAnimationRemap;
static hdScreenUiSystem* sEnemyTowerUiSystem;
static hdMapChartChildNames_l sEnemyTowerChildNames;
static u32 sEnemyTowerSafeStringVtable;
static hdMapChartStaticSettings_l sEnemyTowerSettings;
static hdMapChartSettingConstants_l sEnemyTowerSettingConstants;
static hdMapChartChildNames_l sEnemyTowerArchiveAliases;
static hdMapChartRegistration_l sEnemyTowerRegistration0;
static hdMapChartRegistration_l sEnemyTowerRegistration1;
static hdMapChartRegistration_l sEnemyTowerRegistration2;
enum { CHART_ENTRY_CAPACITY = 6 };
/* 02668AAC */
static u32 enemyTowerStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 02668BAC */
void hdEnemyTower_c::destroy(hdEnemyTower_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    seaChartLinkDestroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 02668C18 */
hdEnemyTower_c* hdEnemyTower_c::construct(hdEnemyTower_c* self) {
    if (!self) self = static_cast<hdEnemyTower_c*>(chartAllocate(sizeof(hdEnemyTower_c)));
    if (self) {
        seaChartLinkConstruct(self);
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    }
    return self;
}
/* 02668C84 */
void hdEnemyTower_c::configure() {
    mPaneNameCount = CHART_ENTRY_CAPACITY;
    mPaneNames = sEnemyTowerPaneNames.mNames;
    configureAnimations(0, 1, 0, 2);
    setupLayout();
}
/* 02668DB8 */
void hdEnemyTower_c::finalize() {
    destroyLayout();
    detachArchives();
}
/* 02668DEC */
void hdEnemyTower_c::executeState() { chartUpdateMachine(stateMachine()); }
/* 02668CDC */
u32 hdEnemyTower_c::load(void* context, void* resource, void* option) {
    attachArchives(sEnemyTowerArchives.mNames, 4);
    u32 load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sEnemyTowerLayoutName, accessor, context, resource, option)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    configure();
    return 1;
}
/* 02668EFC */
u32 hdEnemyTower_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sEnemyTowerUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 2, 1, desc)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdAnimSet* animations = mpLayout->animSet();
        u32 animation = sEnemyTowerAnimationRemap.mIndices[i];
        hdUiSafeString* group = &sEnemyTowerAnimationGroups.mNames[i];
        // The remap is an unchecked guest index, including 32-bit address wrap.
        hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sEnemyTowerAnimationNames) + animation * sizeof(hdUiSafeString));
        chartBindAnimation(animations, i, group, animationName);
    }
    vCalcLayout();
    return 1;
}
/* 02668DF4 */
u32 hdEnemyTower_c::createNamedChild(hdMapChartDesc_l* desc) {
    enum { CHILD_NAME_COUNT = 3, STRING_COMPARE_BUDGET = 0x40001 };
    hdUiSafeString* name = &desc->mName;
    for (u32 i = 0; i < CHILD_NAME_COUNT; ++i) {
        name->assureTermination();
        name->assureTermination();
        hdUiSafeString* expected = &sEnemyTowerChildNames.mNames[i];
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
/* 02669028 */
static void enemyTowerInitialize() {
    sEnemyTowerSettings.mRegistrationWork[3] = 0;
    sEnemyTowerSettings.mRegistrationWork[2] = 0;
    sEnemyTowerSettings.mRegistrationWork[1] = 0;
    sEnemyTowerSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sEnemyTowerRegistration0);
    u32 minimumAngleBits = sEnemyTowerSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sEnemyTowerSettingConstants.mMaximumAngle.mBits;
    sEnemyTowerSettings.mMinimumAngle.mBits = minimumAngleBits;
    sEnemyTowerSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sEnemyTowerSettings.mSignedHeader);
    chartRegisterGlobal(&sEnemyTowerRegistration1);
    chartInitializeUnsignedHeader(&sEnemyTowerSettings.mUnsignedHeader);
    chartRegisterGlobal(&sEnemyTowerRegistration2);
    u32 farDistanceBits = sEnemyTowerSettingConstants.mFarDistance.mBits;
    sEnemyTowerSettings.mDistances[0].mBits = farDistanceBits;
    u32 nearDistanceBits = sEnemyTowerSettingConstants.mNearDistance.mBits;
    sEnemyTowerSettings.mDistances[1].mBits = farDistanceBits;
    sEnemyTowerPaneNames.mNames[0].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_01");
    sEnemyTowerSettings.mDistances[2].mBits = nearDistanceBits;
    sEnemyTowerPaneNames.mNames[1].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerPaneNames.mNames[1].mText.v = gabi::guest_addr("N_Grid_00");
    sEnemyTowerPaneNames.mNames[2].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerPaneNames.mNames[2].mText.v = gabi::guest_addr("N_Grid_01");
    sEnemyTowerPaneNames.mNames[3].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerPaneNames.mNames[3].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sEnemyTowerPaneNames.mNames[4].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerSettings.mDistances[3].mBits = nearDistanceBits;
    sEnemyTowerPaneNames.mNames[4].mText.v = gabi::guest_addr("L_BtnBack_00");
    sEnemyTowerPaneNames.mNames[5].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerAnimationNames.mName.mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerAnimationNames.mName.mText.v = gabi::guest_addr("G_All_00");
    sEnemyTowerAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerAnimationGroups.mNames[0].mText.v = gabi::guest_addr("InAll");
    sEnemyTowerAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerAnimationGroups.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sEnemyTowerArchiveAliases.mNames[1].mText.v = gabi::guest_addr("LinkPos_00");
    sEnemyTowerChildNames.mNames[0].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerPaneNames.mNames[5].mText.v = gabi::guest_addr("L_LinkPos_00");
    sEnemyTowerChildNames.mNames[0].mText.v = gabi::guest_addr("L_MapAreaNow_00");
    sEnemyTowerArchiveAliases.mNames[0].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerChildNames.mNames[1].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchiveAliases.mNames[0].mText.v = gabi::guest_addr("MapAreaNow_00");
    sEnemyTowerChildNames.mNames[1].mText.v = gabi::guest_addr("L_LinkPos_00");
    sEnemyTowerArchiveAliases.mNames[2].mText.v = gabi::guest_addr("BtnBack_00");
    sEnemyTowerArchiveAliases.mNames[2].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerChildNames.mNames[2].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchives.mNames[0].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchives.mNames[0].mText.v = gabi::guest_addr("EnemyTower_00");
    sEnemyTowerArchives.mNames[1].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchives.mNames[1].mText.v = gabi::guest_addr("MapAreaNow_00");
    sEnemyTowerArchives.mNames[2].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchives.mNames[2].mText.v = gabi::guest_addr("LinkPos_00");
    sEnemyTowerArchives.mNames[3].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchives.mNames[3].mText.v = gabi::guest_addr("BtnBack_00");
    sEnemyTowerChildNames.mNames[2].mText.v = gabi::guest_addr("L_BtnBack_00");
    sEnemyTowerLayoutName.mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerArchiveAliases.mNames[1].mVtable.v = gabi::ea(&sEnemyTowerSafeStringVtable);
    sEnemyTowerLayoutName.mText.v = gabi::guest_addr("EnemyTower_00");
}
/* WWHD glue
 data sScreenVtable 100F4520 nocheck
 data sHolderVtable 100F4620 nocheck
 data sSecondaryVtable 100F4630 nocheck
 data sEnemyTowerSafeStringVtable 100F4444 nocheck
data sEnemyTowerSettings 10494000
data sEnemyTowerSettingConstants 100F4464
data sEnemyTowerArchiveAliases 10494084
data sEnemyTowerRegistration0 101F5EB4
data sEnemyTowerRegistration1 101F5EC0
data sEnemyTowerRegistration2 101F5ECC
str 100F44CC "N_All_01" in 02669028
str 100F4494 "N_Grid_00" in 02669028
str 100F44A0 "N_Grid_01" in 02669028
str 100F4484 "L_MapAreaNow_00" in 02669028
str 100F44D8 "L_BtnBack_00" in 02669028
str 100F44F8 "G_All_00" in 02669028
str 100F4474 "InAll" in 02669028
str 100F447C "OutAll" in 02669028
str 100F4504 "LinkPos_00" in 02669028
str 100F44E8 "L_LinkPos_00" in 02669028
str 100F44AC "MapAreaNow_00" in 02669028
str 100F4510 "BtnBack_00" in 02669028
str 100F44BC "EnemyTower_00" in 02669028
ext 028F026C void chartRegisterGlobal(hdMapChartRegistration_l* node)
ext 028ED6F8 void chartInitializeSignedHeader(be<u8>* value)
ext 028EAB2C void chartInitializeUnsignedHeader(be<u8>* value)
ext 026F91F8 void hdParts_c::detachArchives()
ext 02006364 void chartUpdateMachine(hdStateMachine*)
ext 0273AD10 void* chartAllocate(u32)
ext 02683938 void seaChartLinkDestroy(hdSeaChartLinkPos_c*, s32)
ext 026842D4 hdSeaChartLinkPos_c* seaChartLinkConstruct(hdSeaChartLinkPos_c*)
ext 02684AB4 void hdSeaChartLinkPos_c::configureAnimations(u32, u32, u32, u32)
ext 026847F4 void hdSeaChartLinkPos_c::setupLayout()
ext 026844C0 void hdSeaChartLinkPos_c::destroyLayout()
data sEnemyTowerPaneNames 1049402C
data sEnemyTowerArchives 1049409C
data sEnemyTowerLayoutName 104940BC
data sEnemyTowerAnimationGroups 1049405C
data sEnemyTowerAnimationNames 10493FF8
data sEnemyTowerAnimationRemap 100F4428
data sEnemyTowerUiSystem 1018C404
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString* names, u32 count)
ext 02004E04 void chartBindAnimation(hdAnimSet* set, u32 index, hdUiSafeString* group, hdUiSafeString* name)
data sEnemyTowerChildNames 1049406C
ext 026F92C0 u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc)
*/
