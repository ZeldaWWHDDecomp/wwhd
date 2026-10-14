/** HD GhostShip chart: phase panes, night visibility and completion event. Private source. */
#include "d/hd_screen_ghost_ship.h"
static u32 sScreenVtable;
static u32 sHolderVtable;
static u32 sSecondaryVtable;
static hdGhostShipEntryNames_l sPaneNames;
static hdGhostShipNames_l sGhostMoonPaneNames;
static hdGhostShipSaveRoot_l* sSaveRoot;
static f32 sNightStart;
static f32 sDayStart;
static f32 sTimeZero;
static f32 sAnimationRate;
static hdGhostShipArchives_l sGhostArchives;
static hdUiSafeString sGhostLayoutName;
static hdUiSafeString sGhostChildName;
static hdGhostShipAnimationNames_l sGhostAnimationGroups;
static hdGhostShipAnimationSourceNames_l sGhostAnimationNames;
static hdGhostShipAnimationRemap_l sGhostAnimationRemap;
static hdScreenUiSystem* sGhostUiSystem;
static u32 sGhostShipSafeStringVtable;
static hdMapChartStaticSettings_l sGhostShipSettings;
static hdMapChartSettingConstants_l sGhostShipSettingConstants;
static hdUiSafeString sGhostArchiveAlias;
static hdMapChartRegistration_l sGhostShipRegistration0;
static hdMapChartRegistration_l sGhostShipRegistration1;
static hdMapChartRegistration_l sGhostShipRegistration2;
enum { PANE_VISIBLE = 1, MOON_PANE_COUNT = 8, FIRST_PHASE_ANIMATION = 2, PHASE_ANIMATION_COUNT = 7, GHOST_SHIP_EVENT = 0x8803, GHOST_SHIP_COMPLETE = 3 };
/* 0266C448 */
u32 hdGhostShip_c::receiveTouch() { return 0; }
/* 0266C454 */
u32 hdGhostShip_c::receiveKey() { return 0; }
/* 0266C45C */
u32 hdGhostShip_c::receivePointer() { return 0; }
/* 0266C468 */
u32 hdGhostShip_c::receiveFocus() { return 0; }
/* 0266C470 */
static u32 ghostShipStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 0266C544 */
void hdGhostShip_c::destroy(hdGhostShip_c* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    seaChartLinkDestroy(self, 0);
    if (flags & 1) operator_delete(self);
}
/* 0266C5B0 */
hdGhostShip_c* hdGhostShip_c::construct(hdGhostShip_c* self) {
    if (!self) self = static_cast<hdGhostShip_c*>(chartAllocate(sizeof(hdGhostShip_c)));
    if (self) {
        seaChartLinkConstruct(self);
        self->mVtable.v = gabi::ea(&sScreenVtable);
        self->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
        self->mStateHolder.mHolderVtable.v = gabi::ea(&sHolderVtable);
        hdGhostShipPaneArray_l* panes = gabi::at<hdGhostShipPaneArray_l>(gabi::ea(self) + offsetof(hdGhostShip_c, mMoonPanes));
        // The GHS embedded-array constructor allocates on a wrapped NULL address, but keeps its original pointer.
        if (!panes) chartAllocate(sizeof(hdGhostShipPaneArray_l));
        self->mMoonVisible = 0;
        for (u32 i = 0; i < MOON_PANE_COUNT; ++i) panes->mPanes[i] = nullptr;
    }
    return self;
}
/* 0266C670 */
void hdGhostShip_c::configure() {
    mPaneNameCount = 2;
    mPaneNames = sPaneNames.mNames;
    configureAnimations(0, 9, 0, 0);
    mCompactLayout = 1;
    setupLayout();
}
/* 0266C6D0 */
void hdGhostShip_c::collectMoonPanes() {
    hdScreenResourceObject* root = gabi::at<hdScreenResourceObject>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    for (u32 i = 0; i < MOON_PANE_COUNT; ++i) {
        hdUiSafeString* name = &sGhostMoonPaneNames.mNames[i];
        hdScreenResourceTable* vtable = root->mVtable;
        name->assureTermination();
        u32 target = vtable->mFindPane.get();
        be<u8>* text = name->mText;
        hdGhostShipPane_l* pane = gabi::call_ptr<hdGhostShipPane_l*>(target, root, text, 1);
        mMoonPanes.mPanes[i] = pane;
    }
}
/* 0266C8BC */
void hdGhostShip_c::finalize() { destroyLayout(); detachArchives(); }
/* 0266C8F0 */
void hdGhostShip_c::updateMoon() {
    f32 nightStart = sNightStart;
    hdGhostShipSaveRoot_l* save = sSaveRoot;
    f32 time = save->mInfo.mSavedata.mPlayer.mPlayerStatusB.mTime;
    u32 visible = 0;
    if (time >= nightStart) visible = 1;
    else if (!(time < sTimeZero) && !(time > sDayStart)) visible = 1;
    u32 complete = 0;
    u32 event = save->mInfo.mSavedata.mEvent.getEventReg(GHOST_SHIP_EVENT);
    u8 oldVisible = mMoonVisible;
    if (event == GHOST_SHIP_COMPLETE) { visible = 0; complete = 1; }
    if (oldVisible == visible) return;
    mMoonVisible = visible;
    for (u32 i = FIRST_PHASE_ANIMATION; i < FIRST_PHASE_ANIMATION + PHASE_ANIMATION_COUNT; ++i)
        hdAnimSet_operationCC(animSet(), i, i);
    hdGhostShipPane_l* moon = mMoonPanes.mPanes[0];
    if (visible) {
        moon->mFlags = (moon->mFlags & ~PANE_VISIBLE) + PANE_VISIBLE;
        u32 phase = ghostShipMoonPhase();
        hdAnimSet_play(animSet(), phase + FIRST_PHASE_ANIMATION, phase + FIRST_PHASE_ANIMATION, sAnimationRate);
        for (u32 i = 1; i < MOON_PANE_COUNT; ++i) {
            hdGhostShipPane_l* pane = mMoonPanes.mPanes[i];
            pane->mFlags = (pane->mFlags & ~PANE_VISIBLE) + PANE_VISIBLE;
        }
    } else {
        moon->mFlags = moon->mFlags & ~PANE_VISIBLE;
        if (complete) for (u32 i = 1; i < MOON_PANE_COUNT; ++i) {
            hdGhostShipPane_l* pane = mMoonPanes.mPanes[i];
            pane->mFlags = pane->mFlags & ~PANE_VISIBLE;
        }
    }
}
/* 0266CABC */
void hdGhostShip_c::executeState() { updateMoon(); chartUpdateMachine(stateMachine()); }
/* 0266CD00 */
void hdGhostShip_c::resetMoon() {
    mMoonVisible = 0;
    for (u32 i = FIRST_PHASE_ANIMATION; i < FIRST_PHASE_ANIMATION + PHASE_ANIMATION_COUNT; ++i)
        hdAnimSet_operationCC(animSet(), i, i);
    hdGhostShipPane_l* moon = mMoonPanes.mPanes[0];
    moon->mFlags = moon->mFlags & ~PANE_VISIBLE;
}
/* 0266C7AC */
u32 hdGhostShip_c::load(void* context, void* resource, void* option) {
    attachArchives(sGhostArchives.mNames, 2);
    u32 load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sGhostLayoutName, accessor, context, resource, option)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    configure();
    collectMoonPanes();
    hdGhostShipPane_l* moon = mMoonPanes.mPanes[0];
    moon->mFlags = moon->mFlags & ~PANE_VISIBLE;
    hdAnimSet_play(animSet(), 1, 1, sAnimationRate);
    return 1;
}
/* 0266CAF0 */
u32 hdGhostShip_c::createNamedChild(hdMapChartDesc_l* desc) {
    enum { STRING_COMPARE_BUDGET = 0x40001 };
    hdUiSafeString* name = &desc->mName;
    name->assureTermination();
    name->assureTermination();
    be<u8>* capturedText = name->mText;
    sGhostChildName.assureTermination();
    bool match = capturedText == sGhostChildName.mText.get();
    if (!match) {
        be<u8>* text = name->mText;
        be<u8>* wanted = sGhostChildName.mText;
        for (u32 j = 0; j < STRING_COMPARE_BUDGET; ++j) {
            u8 a = *gabi::at<be<u8>>(gabi::ea(text) + j);
            u8 b = *gabi::at<be<u8>>(gabi::ea(wanted) + j);
            if (a != b) break;
            if (a == 0) { match = true; break; }
        }
    }
    if (match) return createChild(0, gabi::at<hdPartsDesc_c>(gabi::ea(desc)));
    return 1;
}
/* 0266CBD4 */
u32 hdGhostShip_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    enum { ANIMATION_COUNT = 10, GROUP_COUNT = 9 };
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sGhostUiSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, ANIMATION_COUNT, GROUP_COUNT, desc)) return 0;
    for (u32 i = 0; i < ANIMATION_COUNT; ++i) {
        hdAnimSet* animations = mpLayout->animSet();
        u32 animation = sGhostAnimationRemap.mIndices[i];
        hdUiSafeString* group = &sGhostAnimationGroups.mNames[i];
        hdUiSafeString* animationName = gabi::at<hdUiSafeString>(gabi::ea(&sGhostAnimationNames) + animation * sizeof(hdUiSafeString));
        chartBindAnimation(animations, i, group, animationName);
    }
    vCalcLayout();
    return 1;
}
/* 0266CD78 */
static void ghostShipInitialize() {
    sGhostShipSettings.mRegistrationWork[3] = 0;
    sGhostShipSettings.mRegistrationWork[2] = 0;
    sGhostShipSettings.mRegistrationWork[1] = 0;
    sGhostShipSettings.mRegistrationWork[0] = 0;
    chartRegisterGlobal(&sGhostShipRegistration0);
    u32 minimumAngleBits = sGhostShipSettingConstants.mMinimumAngle.mBits;
    u32 maximumAngleBits = sGhostShipSettingConstants.mMaximumAngle.mBits;
    sGhostShipSettings.mMinimumAngle.mBits = minimumAngleBits;
    sGhostShipSettings.mMaximumAngle.mBits = maximumAngleBits;
    chartInitializeSignedHeader(&sGhostShipSettings.mSignedHeader);
    chartRegisterGlobal(&sGhostShipRegistration1);
    chartInitializeUnsignedHeader(&sGhostShipSettings.mUnsignedHeader);
    chartRegisterGlobal(&sGhostShipRegistration2);
    sPaneNames.mNames[0].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    u32 farDistanceBits = sGhostShipSettingConstants.mFarDistance.mBits;
    sPaneNames.mNames[0].mText.v = gabi::guest_addr("N_All_01");
    sGhostShipSettings.mDistances[1].mBits = farDistanceBits;
    sPaneNames.mNames[1].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    u32 nearDistanceBits = sGhostShipSettingConstants.mNearDistance.mBits;
    sGhostAnimationNames.mNames[0].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[0].mText.v = gabi::guest_addr("G_All_00");
    sGhostShipSettings.mDistances[3].mBits = nearDistanceBits;
    sGhostAnimationNames.mNames[1].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostShipSettings.mDistances[0].mBits = farDistanceBits;
    sGhostAnimationNames.mNames[1].mText.v = gabi::guest_addr("G_Ship_00");
    sGhostAnimationNames.mNames[2].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostShipSettings.mDistances[2].mBits = nearDistanceBits;
    sGhostAnimationNames.mNames[2].mText.v = gabi::guest_addr("G_Moon_00");
    sGhostAnimationNames.mNames[3].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[3].mText.v = gabi::guest_addr("G_Moon_01");
    sGhostAnimationNames.mNames[4].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[4].mText.v = gabi::guest_addr("G_Moon_02");
    sGhostAnimationNames.mNames[5].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[5].mText.v = gabi::guest_addr("G_Moon_03");
    sGhostAnimationNames.mNames[6].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[6].mText.v = gabi::guest_addr("G_Moon_04");
    sGhostAnimationNames.mNames[7].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[7].mText.v = gabi::guest_addr("G_Moon_05");
    sGhostAnimationNames.mNames[8].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationNames.mNames[8].mText.v = gabi::guest_addr("G_Moon_06");
    sPaneNames.mNames[1].mText.v = gabi::guest_addr("L_BtnBack_00");
    sGhostAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[0].mText.v = gabi::guest_addr("InAll");
    sGhostAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[1].mText.v = gabi::guest_addr("ShipLoop");
    sGhostAnimationGroups.mNames[2].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[2].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[3].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[3].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[4].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[4].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[5].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[5].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[6].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[6].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[7].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[7].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[8].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[0].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostAnimationGroups.mNames[8].mText.v = gabi::guest_addr("MoonLoop");
    sGhostAnimationGroups.mNames[9].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostChildName.mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[0].mText.v = gabi::guest_addr("P_Ship_00");
    sGhostMoonPaneNames.mNames[1].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[1].mText.v = gabi::guest_addr("P_Moon_00");
    sGhostArchiveAlias.mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostChildName.mText.v = gabi::guest_addr("L_BtnBack_00");
    sGhostAnimationGroups.mNames[9].mText.v = gabi::guest_addr("OutAll");
    sGhostArchives.mNames[0].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostArchives.mNames[0].mText.v = gabi::guest_addr("GhostShip_00");
    sGhostMoonPaneNames.mNames[2].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[2].mText.v = gabi::guest_addr("P_Moon_01");
    sGhostArchives.mNames[1].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[3].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[3].mText.v = gabi::guest_addr("P_Moon_02");
    sGhostMoonPaneNames.mNames[4].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[4].mText.v = gabi::guest_addr("P_Moon_03");
    sGhostArchiveAlias.mText.v = gabi::guest_addr("BtnBack_00");
    sGhostMoonPaneNames.mNames[5].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostArchives.mNames[1].mText.v = gabi::guest_addr("BtnBack_00");
    sGhostMoonPaneNames.mNames[5].mText.v = gabi::guest_addr("P_Moon_04");
    sGhostMoonPaneNames.mNames[6].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[6].mText.v = gabi::guest_addr("P_Moon_05");
    sGhostLayoutName.mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[7].mVtable.v = gabi::ea(&sGhostShipSafeStringVtable);
    sGhostMoonPaneNames.mNames[7].mText.v = gabi::guest_addr("P_Moon_06");
    sGhostLayoutName.mText.v = gabi::guest_addr("GhostShip_00");
}
/* WWHD glue
 data sScreenVtable 100F5320 nocheck
 data sHolderVtable 100F5420 nocheck
 data sSecondaryVtable 100F5430 nocheck
 data sGhostShipSafeStringVtable 100F518C nocheck
data sGhostShipSettings 10494490
data sGhostShipSettingConstants 100F51E0
data sGhostArchiveAlias 10494488
data sGhostShipRegistration0 101F5F8C
data sGhostShipRegistration1 101F5F98
data sGhostShipRegistration2 101F5FA4
str 100F52C0 "N_All_01" in 0266CD78
str 100F52DC "G_All_00" in 0266CD78
str 100F5200 "G_Ship_00" in 0266CD78
str 100F520C "G_Moon_00" in 0266CD78
str 100F5218 "G_Moon_01" in 0266CD78
str 100F5224 "G_Moon_02" in 0266CD78
str 100F5230 "G_Moon_03" in 0266CD78
str 100F523C "G_Moon_04" in 0266CD78
str 100F5248 "G_Moon_05" in 0266CD78
str 100F5254 "G_Moon_06" in 0266CD78
str 100F52CC "L_BtnBack_00" in 0266CD78
str 100F51F0 "InAll" in 0266CD78
str 100F52E8 "ShipLoop" in 0266CD78
str 100F52F4 "MoonLoop" in 0266CD78
str 100F5260 "P_Ship_00" in 0266CD78
str 100F526C "P_Moon_00" in 0266CD78
str 100F51F8 "OutAll" in 0266CD78
str 100F530C "GhostShip_00" in 0266CD78
str 100F5278 "P_Moon_01" in 0266CD78
str 100F5284 "P_Moon_02" in 0266CD78
str 100F5290 "P_Moon_03" in 0266CD78
str 100F5300 "BtnBack_00" in 0266CD78
str 100F529C "P_Moon_04" in 0266CD78
str 100F52A8 "P_Moon_05" in 0266CD78
str 100F52B4 "P_Moon_06" in 0266CD78
ext 02560828 u32 ghostShipMoonPhase()
data sPaneNames 104944BC
data sGhostMoonPaneNames 104944DC
data sSaveRoot 101F84DC
data sNightStart 100F51D4
data sDayStart 100F51DC
data sTimeZero 100F51D8
data sAnimationRate 100F51D0
data sGhostArchives 104944CC
data sGhostLayoutName 104945B4
data sGhostChildName 10494480
data sGhostAnimationGroups 10494564
data sGhostAnimationNames 1049451C
data sGhostAnimationRemap 100F51A4
data sGhostUiSystem 1018C404
*/
