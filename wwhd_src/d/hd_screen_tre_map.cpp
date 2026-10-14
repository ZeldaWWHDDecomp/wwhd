/** HD treasure-chart display, chart-opening animations and treasure marker. Private decomp source. */
#include "d/hd_screen_tre_map.h"
static hdTreMapParameters_c sParameters;
static HdPadPoint sHelpfulSize;
static HdPadPoint sTreasureSize;
static HdPadPoint sTriforceSize;
static u32 sChartEnabled;
static hdPartsVtbl_c sTreMapVtable;
static u8 sHolderVtable;
static u8 sSecondaryVtable;
static f32 sOne;
static f32 sZero;
extern void* treMapAllocate(u32);
extern void treMapFree(void*);
extern void treMapAnimationShow(hdAnimSet*, u32, u32);
static hdPartsStateId_c sHelpfulOpeningState;
static hdPartsStateId_c sHelpfulClosedState;
static hdPartsStateId_c sHelpfulOpenState;
static hdPartsStateId_c sTriforceOpeningState;
static hdPartsStateId_c sTriforceClosedState;
static hdPartsStateId_c sTriforceUnopenedState;
static hdPartsStateId_c sTreasureOpeningState;
static hdPartsStateId_c sTreasureClosedState;
static hdPartsStateId_c sTreasureOpenState;

/* 026EBD58 */
static HdPadPoint* treMapHelpfulSize() { return &sHelpfulSize; }

/* 026EBD64 */
static f32 treMapHelpfulScaleX() { return sParameters.mHelpfulScaleX; }

/* 026EBD70 */
static f32 treMapHelpfulScaleY() { return sParameters.mHelpfulScaleY; }

/* 026EBD7C */
static HdPadPoint* treMapTriforceSize() { return &sTriforceSize; }

/* 026EBD88 */
static f32 treMapTriforceScaleX() { return sParameters.mTriforceScaleX; }

/* 026EBD94 */
static f32 treMapTriforceScaleY() { return sParameters.mTriforceScaleY; }

/* 026EBDA0 */
static HdPadPoint* treMapTreasureSize() { return &sTreasureSize; }

/* 026EBDAC */
static f32 treMapTreasureScaleX() { return sParameters.mTreasureScaleX; }

/* 026EBDB8 */
static f32 treMapTreasureScaleY() { return sParameters.mTreasureScaleY; }

/* 026EBDC4 */
static u32 treMapMode() { return sParameters.mMode; }

/* 026EBDD0 */
static f32 treMapDepth() { return sParameters.mDepth; }

/* 026EBDDC */
static u32 treMapFlags() { return sParameters.mFlags; }

/* 026EBDE8 */
static u32 treMapEnabled() { return sChartEnabled; }

/* 026EBDF4 */
static u32 treMapStateKey(hdPartsStateId_c* state) { return state->mNumber; }

/* 026EBEF4 */
void hdTreMapScreen_c::destroy(hdTreMapScreen_c* self, u32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sHolderVtable;
    self->mSecondaryVtable = &sSecondaryVtable;
    self->NoControlPart_l::destroy(0);
    if (flags & 1) treMapFree(self);
}

/* 026EBF60 */
void hdTreMapScreen_c::helpfulOpeningEnter() {
    hdAnimSet_play(animSet(), HELPFUL_OPEN, OPEN_SLOT, sOne);
}

/* 026EBF7C */
void hdTreMapScreen_c::helpfulOpeningExecute() {
    if (hdAnimSet_isEnd(animSet(), HELPFUL_OPEN) != 0) changeState(&sHelpfulOpenState);
    vCalcLayout();
}

/* 026EBFE0 */
void hdTreMapScreen_c::helpfulClosedEnter() {
    hdAnimSet_operationCC(animSet(), HELPFUL_OPEN, OPEN_SLOT);
}

/* 026EBFF4 */
void hdTreMapScreen_c::helpfulClosedExecute() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EC004 */
void hdTreMapScreen_c::helpfulOpenEnter() {
    treMapAnimationShow(animSet(), HELPFUL_OPEN, OPEN_SLOT);
}

/* 026EC018 */
void hdTreMapScreen_c::helpfulOpenExecute() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EC028 */
void hdTreMapScreen_c::triforceOpeningEnter() {
    hdAnimSet_play(animSet(), TRIFORCE_OPEN, OPEN_SLOT, sOne);
}

/* 026EC044 */
void hdTreMapScreen_c::triforceOpeningExecute() {
    if (hdAnimSet_isEnd(animSet(), TRIFORCE_OPEN) != 0) changeState(&sTriforceUnopenedState);
    vCalcLayout();
}

/* 026EC0A8 */
void hdTreMapScreen_c::triforceClosedEnter() {
    hdAnimSet_operationCC(animSet(), TRIFORCE_OPEN, OPEN_SLOT);
}

/* 026EC0BC */
void hdTreMapScreen_c::triforceClosedExecute() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EC0CC */
void hdTreMapScreen_c::triforceUnopenedEnter() {
    treMapAnimationShow(animSet(), TRIFORCE_OPEN, OPEN_SLOT);
}

/* 026EC0E0 */
void hdTreMapScreen_c::triforceUnopenedExecute() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EC0F0 */
void hdTreMapScreen_c::treasureOpeningEnter() {
    hdAnimSet_play(animSet(), TREASURE_OPEN, OPEN_SLOT, sOne);
}

/* 026EC10C */
void hdTreMapScreen_c::treasureOpeningExecute() {
    if (hdAnimSet_isEnd(animSet(), TREASURE_OPEN) != 0) changeState(&sTreasureOpenState);
    vCalcLayout();
}

/* 026EC170 */
void hdTreMapScreen_c::treasureClosedEnter() {
    hdAnimSet_operationCC(animSet(), TREASURE_OPEN, OPEN_SLOT);
}

/* 026EC184 */
void hdTreMapScreen_c::treasureClosedExecute() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EC194 */
void hdTreMapScreen_c::treasureOpenEnter() {
    treMapAnimationShow(animSet(), TREASURE_OPEN, OPEN_SLOT);
}

/* 026EC1A8 */
void hdTreMapScreen_c::treasureOpenExecute() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EC1B8 */
hdTreMapScreen_c* hdTreMapScreen_c::construct(hdTreMapScreen_c* self) {
    if (self == NULL) {
        self = static_cast<hdTreMapScreen_c*>(treMapAllocate(sizeof(hdTreMapScreen_c)));
        if (self == NULL) return NULL;
    }
    hdPartsNoControl_construct(self);
    self->mVtable = &sTreMapVtable;
    self->mSecondaryVtable = &sSecondaryVtable;
    self->mStateHolder.mHolderVtable = &sHolderVtable;
    // GHS retains allocation checks on embedded members whose guest addresses can wrap.
    if (gabi::ea(self->mGroups) == 0) treMapAllocate(sizeof(self->mGroups));
    if (gabi::ea(&self->mpLinkPane) == 0) treMapAllocate(sizeof(self->mpLinkPane));
    if (gabi::ea(self->mPatternPanes) == 0) treMapAllocate(sizeof(self->mPatternPanes));
    if (gabi::ea(&self->mpMapPane) == 0) treMapAllocate(sizeof(self->mpMapPane));
    if (gabi::ea(&self->mpPositionPane) == 0) treMapAllocate(sizeof(self->mpPositionPane));
    self->mChartKind = 0xFF;
    self->mScale.y = sZero;
    self->mHalfSize.x = sZero;
    self->mScale.x = sZero;
    self->mHalfSize.y = sZero;
    self->mChartIndex = NO_CHART;
    self->mTreasureShown = 0;
    self->mOrigin.x = sZero;
    self->mOrigin.y = sZero;
    for (u32 i = 0; i < GROUP_COUNT; ++i) self->mGroups[i] = NULL;
    return self;
}
/* 026ECA4C */
void hdTreMapScreen_c::beginTreasure() {
    hdAnimSet_play(animSet(), TREASURE_GET, TREASURE_SLOT, sOne);
    mTreasureShown = 1;
}
/* 026ECD28 */
void hdTreMapScreen_c::showTreasure() {
    treMapAnimationShow(animSet(), TREASURE_GET, TREASURE_SLOT);
    mTreasureShown = 1;
}
/* 026ECD6C */
void hdTreMapScreen_c::hideTreasure() {
    hdAnimSet_operationCC(animSet(), TREASURE_GET, TREASURE_SLOT);
    mTreasureShown = 0;
}

/* 026ED014 */
void hdTreMapScreen_c::requestHelpfulOpening() { changeState(&sHelpfulOpeningState); }

/* 026ED024 */
void hdTreMapScreen_c::requestHelpfulClosed() { changeState(&sHelpfulClosedState); }

/* 026ED034 */
void hdTreMapScreen_c::requestHelpfulOpen() { changeState(&sHelpfulOpenState); }

/* 026ED044 */
void hdTreMapScreen_c::requestTriforceOpening() { changeState(&sTriforceOpeningState); }

/* 026ED054 */
void hdTreMapScreen_c::requestTriforceClosed() { changeState(&sTriforceClosedState); }

/* 026ED064 */
void hdTreMapScreen_c::requestTriforceUnopened() { changeState(&sTriforceUnopenedState); }

/* 026ED074 */
void hdTreMapScreen_c::requestTreasureOpening() { changeState(&sTreasureOpeningState); }

/* 026ED084 */
void hdTreMapScreen_c::requestTreasureClosed() { changeState(&sTreasureClosedState); }

/* 026ED094 */
void hdTreMapScreen_c::requestTreasureOpen() { changeState(&sTreasureOpenState); }

static hdUiSafeString sLinkPaneName;
static hdTreMapPaneNames_l sPatternPaneNames;
static hdUiSafeString sMapPaneName;
static hdUiSafeString sPositionPaneName;
extern hdTreMapGroup_l* treMapFindGroup(hdTreMapGroupManager_l*, be<u8>*);
extern hdSeaChartPane_l* treMapFindLayoutPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*);

/* 026EC2C0 */
hdTreMapGroup_l* hdTreMapScreen_c::findGroup(hdUiSafeString* name) {
    if (name->mText[0] == 0) return NULL;
    auto* layout = reinterpret_cast<hdTreMapLayoutGroups_l*>(mpLayout->lyt());
    if (layout == NULL) return NULL;
    name->assureTermination();
    return treMapFindGroup(layout->mpGroups, name->mText);
}
/* 026EC33C */
u32 hdTreMapScreen_c::collectGroups(gptr<hdTreMapGroup_l>* output, hdUiSafeString* names, be<s32>* count) {
    u32 complete = 1;
    for (s32 i = 0; i < *count; ++i) {
        hdTreMapGroup_l* group = findGroup(&names[i]);
        output[i] = group;
        if (group == NULL) complete = 0;
    }
    return complete;
}
/* 026EC3B8 */
void hdTreMapScreen_c::setGroupVisible(hdTreMapGroup_l* group, u32 visible) {
    hdTreMapGroupNode_l* sentinel = &group->mPanes;
    hdTreMapGroupNode_l* node = sentinel->mpNext;
    while (node != sentinel) {
        hdSeaChartPane_l* pane = node->mpPane;
        pane->mFlags = (u32(pane->mFlags) & ~u32(PANE_VISIBLE)) | visible;
        node = node->mpNext;
    }
}
/* 026EC3EC */
void hdTreMapScreen_c::findLinkPane() {
    auto* root = reinterpret_cast<hdCursorPane_c*>(mpLayout->lyt()->mpRootPane.get());
    hdCursorPaneVtbl_c* table = root->mVtable;
    sLinkPaneName.assureTermination();
    mpLinkPane = gabi::call_ptr<hdSeaChartPane_l*>(table->mFindPaneByName.get(), root, sLinkPaneName.mText.get(), 1u);
}
/* 026EC474 */
void hdTreMapScreen_c::findPatternPanes() {
    auto* root = reinterpret_cast<hdCursorPane_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i < PATTERN_COUNT; ++i) {
        hdCursorPaneVtbl_c* table = root->mVtable;
        sPatternPaneNames.mNames[i].assureTermination();
        mPatternPanes[i] = gabi::call_ptr<hdSeaChartPane_l*>(table->mFindPaneByName.get(), root, sPatternPaneNames.mNames[i].mText.get(), 1u);
    }
}
/* 026EC550 */
void hdTreMapScreen_c::findMapPane() {
    auto* root = reinterpret_cast<hdCursorPane_c*>(mpLayout->lyt()->mpRootPane.get());
    hdCursorPaneVtbl_c* table = root->mVtable;
    sMapPaneName.assureTermination();
    mpMapPane = gabi::call_ptr<hdSeaChartPane_l*>(table->mFindPaneByName.get(), root, sMapPaneName.mText.get(), 1u);
}
/* 026EC5D8 */
void hdTreMapScreen_c::findPositionPane() {
    mpPositionPane = treMapFindLayoutPane(mpLayout, &sLinkPaneName, &sPositionPaneName);
}
/* 026ECB2C */
u32 hdTreMapScreen_c::createNamedChild(hdTreMapNamedDesc_l* desc) {
    desc->mName.assureTermination();
    desc->mName.assureTermination();
    be<u8>* name = desc->mName.mText;
    sLinkPaneName.assureTermination();
    bool equal = name == sLinkPaneName.mText.get();
    if (!equal) {
        name = desc->mName.mText;
        be<u8>* expected = sLinkPaneName.mText;
        for (u32 i = 0; i < NAME_SCAN_BUDGET; ++i) {
            u8 character = name[i];
            if (character != expected[i]) break;
            if (character == 0) { equal = true; break; }
        }
    }
    if (!equal) return 1;
    return createChild(0, reinterpret_cast<hdPartsDesc_c*>(desc));
}

static hdTreMapOwner_l* sMapOwner;
extern void treMapUpdateMachine(hdStateMachine*);

/* 026ECA98 */
void hdTreMapScreen_c::checkTreasure() {
    if (mTreasureShown == 0) {
        hdMapDisplayCoordinator_c* coordinator = sMapOwner->mpCoordinator.get();
        if (coordinator->isChartComplete(mChartIndex) != 0) beginTreasure();
    }
}
/* 026ECAF0 */
void hdTreMapScreen_c::executeState() {
    updatePosition();
    checkTreasure();
    treMapUpdateMachine(stateMachine());
}

static hdTreMapSaveKind_l* sSaveKind;
static hdUiSafeString sLayoutName;
static hdTreMapGroupNames_l sGroupNames;
static hdTreMapGroupNames_l sVisibilityGroupNames;
static hdTreMapAnimationNames_l sAnimationNames;
static hdTreMapAnimationGroups_l sAnimationGroups;
static HdPadPoint sMapWorldSize;
static hdMapChartHeap_l* sHeapManager;
extern void* treMapAllocateHeap(u32, void*, u32);
extern void treMapBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 026EC61C */
void hdTreMapScreen_c::initializePanes() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<be<s32>> count(8);
    *count = GROUP_COUNT;
    collectGroups(mGroups, sVisibilityGroupNames.mNames, count);
    mChartKind = sSaveKind->mChartKind;
    for (u32 i = 0; i < GROUP_COUNT; ++i) {
        setGroupVisible(mGroups[i], i == u32(mChartKind));
    }
    findLinkPane();
    findPatternPanes();
    findMapPane();
    findPositionPane();
    hdSeaChartPane_l* pane = mpMapPane;
    f32 width = pane->mSize.x;
    f32 height = pane->mSize.y;
    mHalfSize.x = gabi::fmuls_ppc(width, 0.5f);
    mHalfSize.y = gabi::fmuls_ppc(height, 0.5f);
    mScale.x = f32(sMapWorldSize.x) / width;
    mScale.y = f32(sMapWorldSize.y) / height;
    startState(&sTreasureClosedState);
    vCalcLayout();
}
/* 026EC7C4 */
u32 hdTreMapScreen_c::initialize(u32 first, u32 second, hdPartsDesc_c* desc) {
    if (hdPartsNoControl_init(this, &sLayoutName, first, second, desc) == 0) return 0;
    initializePanes();
    return 1;
}
/* 026ECC10 */
u32 hdTreMapScreen_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* desc) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(treMapAllocateHeap(sizeof(hdPartsLayout_c), sHeapManager->mpHeap, 4));
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL || layout->vLoad(name, first, second, third, ANIMATION_COUNT, ANIMATION_GROUP_COUNT, desc) == 0) return 0;
    for (u32 i = 0; i < ANIMATION_COUNT; ++i) {
        hdAnimSet* animations = animSet();
        u32 group = sAnimationGroups.mIndices[i];
        treMapBindAnimation(animations, i, &sAnimationNames.mNames[i], &sGroupNames.mNames[group]);
    }
    return 1;
}

extern hdMapPlay_l* treMapPlay();
static f32 sSeaCellCount;
static f32 sWorldCellSize;

/* 026EC830 */
void hdTreMapScreen_c::updatePosition() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x48> frame;
    gabi::FrameLocal<HdPadPoint> input(8);
    gabi::FrameLocal<HdPadPoint3> rotation(0x10);
    hdMapDisplayCoordinator_c* coordinator = sMapOwner->mpCoordinator;
    input->x = coordinator->mSeaPosition.x;
    f32 x = (f32(input->x) - f32(mOrigin.x)) / f32(mScale.x);
    input->y = coordinator->mSeaPosition.y;
    f32 y = -((f32(input->y) - f32(mOrigin.y)) / f32(mScale.y));
    hdSeaChartPane_l* pane = mpPositionPane;
    rotation->x = pane->mRotation.x;
    rotation->y = pane->mRotation.y;
    rotation->z = pane->mRotation.z;
    bool useSeaScale = true;
    if ((u32(treMapPlay()->mPlayerStatus) & 0x10000) != 0) {
        hdTreMapOwner_l* owner = sMapOwner;
        if (owner->mpMode->mSeaMode == 0) {
            if (owner->mpCoordinator->isLinkProcess() == 0) useSeaScale = false;
        }
    }
    coordinator = sMapOwner->mpCoordinator;
    f32 angle = useSeaScale ? f32(coordinator->mSeaScale) : f32(coordinator->mDungeonScale);
    u8 flags = pane->mFlags;
    pane->mRotation.x = rotation->x;
    pane->mRotation.z = angle;
    pane->mRotation.y = rotation->y;
    pane->mFlags = flags | PANE_DIRTY;
    f32 halfWidth = mHalfSize.x;
    bool inside = !(x < -halfWidth) && !(x > halfWidth);
    f32 halfHeight = mHalfSize.y;
    inside = inside && !(y < -halfHeight) && !(y > halfHeight); /* ble: an unordered (NaN) y counts as inside */
    hdSeaChartPane_l* link = mpLinkPane;
    link->mFlags = (u32(link->mFlags) & ~u32(PANE_VISIBLE)) | (inside ? PANE_VISIBLE : 0);
    if (!inside) return;
    link = mpLinkPane;
    flags = link->mFlags;
    link->mPosition.y = y;
    link->mPosition.z = sZero;
    link->mPosition.x = x;
    link->mFlags = flags | PANE_DIRTY;
}
/* 026ECDB0 */
void hdTreMapScreen_c::selectChart(u32 chart) {
    mChartIndex = NO_CHART;
    hdMapDisplayCoordinator_c* coordinator = sMapOwner->mpCoordinator;
    hdChartEntry_c* entry = coordinator->findChart(chart);
    if (entry == NULL) return;
    mChartIndex = chart;
    f32 sea = f32(s32(entry->mSeaNo) - 1);
    f32 zero = sZero;
    if (zero > sea || sea >= sSeaCellCount) return;
    u32 kind = mChartKind;
    hdAnimSet_setFrame(animSet(), kind + 3, kind + 2, sea);
    s32 tileX = s8(entry->mUnknown4);
    kind = mChartKind;
    // Chart positions contain four signed rectangle endpoints per chart kind.
    be<s16>* rectangle = &entry->mPositions[kind * 4];
    f32 left = f32(s16(rectangle[0]));
    mOrigin.x = gabi::fmadds(f32(tileX), sWorldCellSize, left);
    s32 tileY = s8(entry->mUnknown5);
    f32 top = f32(s16(rectangle[1]));
    mOrigin.y = gabi::fmadds(f32(tileY), sWorldCellSize, top);
    s32 width = s32(s16(rectangle[2])) - s32(s16(rectangle[0]));
    s32 height = -(s32(s16(rectangle[3])) - s32(s16(rectangle[1])));
    f32 x = f32(width) / f32(mScale.x);
    f32 y = f32(height) / f32(mScale.y);
    hdSeaChartPane_l* pane = mPatternPanes[0];
    pane->mFlags = u32(pane->mFlags) | PANE_DIRTY;
    pane->mPosition.x = x;
    pane->mPosition.y = y;
    pane->mPosition.z = zero;
    pane = mPatternPanes[1];
    u8 flags = pane->mFlags;
    pane->mPosition.y = y;
    pane->mPosition.z = zero;
    pane->mFlags = flags | PANE_DIRTY;
    pane->mPosition.x = x;
    if (coordinator->isChartComplete(mChartIndex) != 0) showTreasure();
    else hideTreasure();
}

static hdTreMapRegistrationWords_l sRegistrationWords;
static u8 sRegistration;
static u8 sArchiveByte;
static u8 sHeaderByte;
static u8 sArchiveRegistration;
static u8 sHeaderRegistration;
static hdTreMapAngleRange_l sAngles;
static hdUiSafeString sLinkPartName;
static hdUiStringTable sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
static hdStateId sNullState;
static u32 sStateIdsInitialized;
static u32 sStateCounter;
static f32 sMinimumAngle;
static f32 sMaximumAngle;
static f32 sCellHalfSize;
static f32 sCellWorldSize;
extern void treMapRegisterStatic(void*);
extern void treMapConstructArchiveByte(void*);
extern void treMapConstructHeaderByte(void*);

/* 026ED0A4 */
static void __sinit_hd_screen_tre_map_cpp() {
    sRegistrationWords.mWords[3] = 0;
    sRegistrationWords.mWords[2] = 0;
    sRegistrationWords.mWords[1] = 0;
    sRegistrationWords.mWords[0] = 0;
    treMapRegisterStatic(&sRegistration);
    sAngles.mMinimum = sMinimumAngle;
    sAngles.mMaximum = sMaximumAngle;
    treMapConstructArchiveByte(&sArchiveByte);
    treMapRegisterStatic(&sArchiveRegistration);
    treMapConstructHeaderByte(&sHeaderByte);
    treMapRegisterStatic(&sHeaderRegistration);
    sAngles.mCellSize.x = sCellHalfSize;
    sAngles.mCellSize.y = sCellHalfSize;
    sMapWorldSize.x = sCellWorldSize;
    sMapWorldSize.y = sCellWorldSize;
    hdParts_setSafeString(&sVisibilityGroupNames.mNames[0], &sStringVtable, "G_MapPattern_00");
    hdParts_setSafeString(&sVisibilityGroupNames.mNames[1], &sStringVtable, "G_MapPattern_01");
    hdParts_setSafeString(&sVisibilityGroupNames.mNames[2], &sStringVtable, "G_MapPattern_02");
    hdParts_setSafeString(&sVisibilityGroupNames.mNames[3], &sStringVtable, "G_MapPattern_03");
    hdParts_setSafeString(&sGroupNames.mNames[0], &sStringVtable, "G_TreasureGet_00");
    hdParts_setSafeString(&sGroupNames.mNames[1], &sStringVtable, "G_TreMapOpen_00");
    hdParts_setSafeString(&sGroupNames.mNames[2], &sStringVtable, "G_MapPattern_00");
    hdParts_setSafeString(&sGroupNames.mNames[3], &sStringVtable, "G_MapPattern_01");
    hdParts_setSafeString(&sGroupNames.mNames[4], &sStringVtable, "G_MapPattern_02");
    hdParts_setSafeString(&sGroupNames.mNames[5], &sStringVtable, "G_MapPattern_03");
    hdParts_setSafeString(&sAnimationNames.mNames[0], &sStringVtable, "HelpMapUnOpen");
    hdParts_setSafeString(&sAnimationNames.mNames[1], &sStringVtable, "TriforceMapUnOpen");
    hdParts_setSafeString(&sAnimationNames.mNames[2], &sStringVtable, "TreMapOpen");
    hdParts_setSafeString(&sAnimationNames.mNames[3], &sStringVtable, "MapPattern");
    hdParts_setSafeString(&sAnimationNames.mNames[4], &sStringVtable, "MapPattern");
    hdParts_setSafeString(&sAnimationNames.mNames[5], &sStringVtable, "MapPattern");
    hdParts_setSafeString(&sAnimationNames.mNames[6], &sStringVtable, "MapPattern");
    hdParts_setSafeString(&sAnimationNames.mNames[7], &sStringVtable, "TreasureGet");
    hdParts_setSafeString(&sPositionPaneName, &sStringVtable, "N_Rotate_00");
    hdParts_setSafeString(&sLinkPartName, &sStringVtable, "LinkPos_00");
    hdParts_setSafeString(&sLinkPaneName, &sStringVtable, "L_LinkPos_01");
    hdParts_setSafeString(&sMapPaneName, &sStringVtable, "N_StandardPos_00");
    hdParts_setSafeString(&sLayoutName, &sStringVtable, "TreMap_00");
    hdParts_setSafeString(&sPatternPaneNames.mNames[0], &sStringVtable, "P_TreasureMark_00");
    hdParts_setSafeString(&sPatternPaneNames.mNames[1], &sStringVtable, "P_TreasureGetBig_00");
    u32 number = 0;
    if (sStateIdsInitialized != 0) number = sStateCounter;
    else sStateIdsInitialized = 1;
    sHelpfulOpeningState.setup(++number, "::StateID_HelpfulMapNoOpenToOpen", &sStateVtable, 25, &sNullState);
    sHelpfulClosedState.setup(++number, "::StateID_HelpfulMapNoOpen", &sStateVtable, 28, &sNullState);
    sHelpfulOpenState.setup(++number, "::StateID_HelpfulMapOpen", &sStateVtable, 31, &sNullState);
    sTriforceOpeningState.setup(++number, "::StateID_TriforceMapNoOpenToUnOpen", &sStateVtable, 34, &sNullState);
    sTriforceClosedState.setup(++number, "::StateID_TriforceMapNoOpen", &sStateVtable, 37, &sNullState);
    sTriforceUnopenedState.setup(++number, "::StateID_TriforceMapUnOpen", &sStateVtable, 40, &sNullState);
    sTreasureOpeningState.setup(++number, "::StateID_TreasureMapNoOpenToOpen", &sStateVtable, 43, &sNullState);
    sTreasureClosedState.setup(++number, "::StateID_TreasureMapNoOpen", &sStateVtable, 46, &sNullState);
    sTreasureOpenState.setup(++number, "::StateID_TreasureMapOpen", &sStateVtable, 49, &sNullState);
    sStateCounter = number;
}

/* WWHD glue
 data sLinkPaneName 1049D028
 data sPatternPaneNames 1049D104
 data sMapPaneName 1049D038
 data sPositionPaneName 1049D040
 ext 026F92C0 u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc)
 ext 02874820 hdTreMapGroup_l* treMapFindGroup(hdTreMapGroupManager_l*, be<u8>*)
 ext 02704FC0 hdSeaChartPane_l* treMapFindLayoutPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*)
 data sMapOwner 101F8344
 ext 0268A648 u32 hdMapDisplayCoordinator_c::isChartComplete(u32)
 ext 02006364 void treMapUpdateMachine(hdStateMachine*)
 data sSaveKind 101F84DC
 data sLayoutName 1049D2C4
 data sGroupNames 1049D094
 data sVisibilityGroupNames 1049D074
 data sAnimationNames 1049D0C4
 data sAnimationGroups 1010847C
 data sMapWorldSize 1049D058
 data sHeapManager 1018C404
 ext 0273B050 void* treMapAllocateHeap(u32, void*, u32)
 ext 02004E04 void treMapBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
 ext 025200D4 hdMapPlay_l* treMapPlay()
 ext 026890E8 u32 hdMapDisplayCoordinator_c::isLinkProcess()
 ext 02687A64 hdChartEntry_c* hdMapDisplayCoordinator_c::findChart(u32)
 data sSeaCellCount 10108548
 data sWorldCellSize 1010854C
 data sRegistrationWords 1049D064
 data sRegistration 101F7054
 data sArchiveByte 1049D060
 data sHeaderByte 1049D061
 data sArchiveRegistration 101F7060
 data sHeaderRegistration 101F706C
 data sAngles 1049D048
 data sLinkPartName 1049D030
 data sStringVtable 10108464
 data sStateVtable 101084E4
 data sNullState 101FF32C
 data sStateIdsInitialized 101FD8E4
 data sStateCounter 101FDD50
 data sMinimumAngle 10108550
 data sMaximumAngle 10108554
 data sCellHalfSize 10108558
 data sCellWorldSize 1010855C
 ext 028F026C void treMapRegisterStatic(void*)
 ext 028ED6F8 void treMapConstructArchiveByte(void*)
 ext 028EAB2C void treMapConstructHeaderByte(void*)
 data sParameters 101F7030
 data sHelpfulSize 1049D00C
 data sTreasureSize 1049D014
 data sTriforceSize 1049D01C
 data sChartEnabled 1049D024
 data sTreMapVtable 10108790
 data sHolderVtable 10108930
 data sSecondaryVtable 10108940
 data sOne 10108530
 data sZero 10108534
 ext 0273AD10 void* treMapAllocate(u32)
 ext 0273AF40 void treMapFree(void*)
 ext 02700A70 void NoControlPart_l::destroy(u32 flags)
 ext 02005708 void treMapAnimationShow(hdAnimSet*, u32, u32)
 data sHelpfulOpeningState 1049D114
 data sHelpfulClosedState 1049D144
 data sHelpfulOpenState 1049D174
 data sTriforceOpeningState 1049D1A4
 data sTriforceClosedState 1049D1D4
 data sTriforceUnopenedState 1049D204
 data sTreasureOpeningState 1049D234
 data sTreasureClosedState 1049D264
 data sTreasureOpenState 1049D294
 str 10108560 "G_MapPattern_00"
 str 10108570 "G_MapPattern_01"
 str 10108580 "G_MapPattern_02"
 str 10108590 "G_MapPattern_03"
 str 101086D8 "G_TreasureGet_00"
 str 101085A0 "G_TreMapOpen_00"
 str 10108654 "HelpMapUnOpen"
 str 10108664 "TriforceMapUnOpen"
 str 101086EC "TreMapOpen"
 str 101086F8 "MapPattern"
 str 101085B0 "TreasureGet"
 str 101085D0 "N_Rotate_00"
 str 10108714 "LinkPos_00"
 str 10108704 "L_LinkPos_01"
 str 10108720 "N_StandardPos_00"
 str 1010868C "TreMap_00"
 str 10108678 "P_TreasureMark_00"
 str 101085BC "P_TreasureGetBig_00"
 str 10108734 "::StateID_HelpfulMapNoOpenToOpen"
 str 10108758 "::StateID_HelpfulMapNoOpen"
 str 10108774 "::StateID_HelpfulMapOpen"
 str 101085DC "::StateID_TriforceMapNoOpenToUnOpen"
 str 10108600 "::StateID_TriforceMapNoOpen"
 str 1010861C "::StateID_TriforceMapUnOpen"
 str 10108698 "::StateID_TreasureMapNoOpenToOpen"
 str 10108638 "::StateID_TreasureMapNoOpen"
 str 101086BC "::StateID_TreasureMapOpen"
*/
