/* HD dungeon-map companion: animation state handlers and map interaction. */
#include "d/hd_screen_dungeon_map_part2.h"

/* #1 types the 0x9C receiver as HdPadInputMode; the floor-button code links it as a button receiver */
static hdButtonReceiver_l* floorReceiver(hdDungeonMap_c* map) { return reinterpret_cast<hdButtonReceiver_l*>(map->mpInputMode.get()); }

static f32 sDungeonAnimationRate;
static hdDungeonInitializerWords_l sDungeonInitializerWords_l;
static hdDungeonGlobalRegistration_l sDungeonRegistration_l;
static hdDungeonGlobalRegistration_l sDungeonAngleRegistration_l;
static hdDungeonGlobalRegistration_l sDungeonLimitsRegistration_l;
static hdDungeonInitializerConstants_l sDungeonInitializerConstants_l;
static hdDungeonInitializerLimits_l sDungeonInitializerLimits_l;
static hdDungeonInitializedCoordinates_l sDungeonInitializedCoordinates_l;
static u8 sDungeonAngleGuard_l;
static u8 sDungeonLimitsGuard_l;
static hdDungeonCaptionNames_l sDungeonCaptionNames_l;
static hdDungeonResourceNames_l sDungeonResourceNames_l;
static hdDungeonStateInitializers_l sDungeonStates_l;
static hdPartsStateIdVtbl_c sDungeonStateDescriptorTable_l;
static u32 sDungeonStateCounterGuard_l;
static u32 sDungeonStateCounter_l;
static u8 sDungeonDisplayNullState_l;
static u8 sDungeonBlankNullState_l;
static u8 sDungeonOtherNullState_l;

static hdPartsVtbl_c sDungeonPartsTable_l;
static u8 sDungeonStateTable_l;
static u8 sDungeonSecondaryTable_l;
static hdMapHeapManager_l* sDungeonResourceContexts_l;
static u8 sDungeonDefaultPrimaryContext_l;
static u8 sDungeonDefaultSecondaryContext_l;
static hdDungeonChildNames_l sDungeonChildNames_l;
static hdScreenString sDungeonItemButtonClass_l;
static hdScreenString sDungeonFloorButtonClass_l;
static hdScreenString sDungeonNoDecideButtonClass_l;
static hdScreenStringTable sDungeonSafeStringTable_l;
static hdDungeonAnimationNames_l sDungeonAnimationNames_l;
static hdDungeonAnimationGroups_l sDungeonAnimationGroups_l;
static hdDungeonAnimationGroupOrder_l sDungeonAnimationGroupOrder_l;
static hdDungeonItemButtonOrder_l sDungeonItemButtonOrder_l;
static hdDungeonPositionSave_l* sDungeonPositionSave_l;
static hdDungeonMapArchiveNames_l sDungeonMapArchiveNames_l;
static hdScreenString sDungeonMapLayoutName_l;
static f32 sDungeonFloorLabelFrame_l;
static hdDungeonFloorLabelTexts_l sDungeonFloorLabelTexts_l;
static f32 sDungeonReverseAnimationRate;
static f32 sDungeonMapReferenceWidth;
static HdPadState sMapZoomInState_l;
static HdPadState sMapCenteringState_l;
static HdPadState sMapEntranceState_l;
static HdPadState sMapZoomOutState_l;
static hdCursorMgr_c* sDungeonCursorManager;
static HdPadState sDungeonZoomOutState_l;
static f32 sDungeonAngleScale_l;
static f32 sDungeonHalfTurn_l;
static hdDungeonFloorOrder_l sDungeonFloorOrder_l;
static HdPadState sDungeonDisplayState_l;
static HdPadState sDungeonBlankState_l;
static HdPadState sDungeonInAllState_l;
static HdPadState sDungeonOutAllState_l;
static HdPadState sDungeonChangeToDisplayState_l;
static HdPadState sDungeonNoDataState_l;
static hdDungeonFrameNames_l sDungeonFrameNames_l;
static hdDungeonFloorNames_l sDungeonFloorNames_l;
static hdDungeonFloorLabelNames_l sDungeonFloorLabelNames_l;
static hdScreenString sDungeonSelectedPaneName_l;
static hdScreenString sDungeonCaptionPaneName_l;
static hdScreenUiSystem* sDungeonUiSystem_l;
static u32 sDungeonCaptionCapacity_l;
static hdDungeonTextCapacities_l sDungeonFloorCapacities_l;
static u16 sDungeonEmptyCaption_l;
static u16 sDungeonEmptyFloorLabel_l;

/* 0265A728 */
void hdDungeonMap_c::enterOpening() {
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 1, 0, rate);
}

/* 0265A744 */
void hdDungeonMap_c::updateOpening() {
    gabi::tail_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265A754 */
void hdDungeonMap_c::enterWaiting() {
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 2, 1, rate);
}

/* 0265A770 */
void hdDungeonMap_c::updateWaiting() {
    gabi::tail_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265A780 */
void hdDungeonMap_c::enterClosing() {
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 3, 3, rate);
}

/* 0265A79C */
void hdDungeonMap_c::updateClosing() {
    gabi::tail_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265A7AC */
void hdDungeonMap_c::enterFinished() {
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 4, 4, rate);
}

/* 0265A7C8 */
void hdDungeonMap_c::updateFinished() {
    gabi::tail_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 02659A80 */
static void hdDungeonOpenAnimated(hdDungeonMap_c* self) {
    self->updateSelectedFloorPane();
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = self->animSet();
    hdAnimSet_play(player, 1, 0, rate);
}

/* 02659AC0 */
static void hdDungeonWaitAnimated(hdDungeonMap_c* self) {
    self->updatePlayerFloorPane();
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = self->animSet();
    hdAnimSet_play(player, 2, 1, rate);
}

/* 0265A0A0 */
void hdDungeonMap_c::beginMapZoomOut() {
    HdPadMap* map = mpMap;
    if (map)
        hdMapBeginZoomOut(map);
}

/* 0265A7D8 */
void hdDungeonMap_c::beginMapZoomIn() {
    HdPadMap* map = mpMap;
    if (map)
        hdMapBeginZoomIn(map);
}

/* 0265D1BC */
void hdDungeonMap_c::beginMapZoomToPlayer() {
    HdPadMap* map = mpMap;
    if (map)
        hdMapBeginZoomToPlayer(map);
}

/* 0265B218 */
void hdDungeonMap_c::beginMapZoomToCenter() {
    HdPadMap* map = mpMap;
    if (map)
        hdPadChangeState(&map->mState, &sMapCenteringState_l);
}

/* 0265C688 */
void hdDungeonMap_c::updateState() {
    hdPadStateExecute(&gamepadView()->mState);
}

/* 0265ACCC */
void hdDungeonMap_c::updatePaneAndIcons() {
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_DRAW].get(), this);
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
    hdPadDungeonUpdateIcons(gamepadView());
    hdPadDungeonUpdatePane(gamepadView());
}

/* 0265AD8C */
void hdDungeonMap_c::clearClosePending() {
    mClosePending = 0;
}

/* 0265C1D8 */
f32 hdDungeonMap_c::widthRatio() {
    f32 ratio = sDungeonAnimationRate;
    HdPadPane* pane = mpMapPane;
    if (pane) {
        f32 width = pane->mWidth;
        f32 reference = sDungeonMapReferenceWidth;
        ratio = f32(f64(width) / f64(reference));
    }
    return ratio;
}

/* 0265C200 */
static void hdDungeonMapBoundsForward(hdDungeonMap_c* self, HdPadRect* bounds) {
    hdDungeonBounds(self->gamepadView(), bounds);
}

/* 0265C690 */
void hdDungeonMap_c::drawSecondary(void* info) {
    hdParts_c* child = mpBossMarker;
    if (child)
        gabi::tail_ptr<void>(child->mVtable->mDraw.get(), child, info);
}

/* 0265C6AC */
void hdDungeonMap_c::drawTertiary(void* info) {
    hdParts_c* child = mpSpecialMarker;
    if (child)
        gabi::tail_ptr<void>(child->mVtable->mDraw.get(), child, info);
}

/* 0265C774 */
void hdDungeonMap_c::drawPrimary(void* info) {
    hdParts_c* child = mpLinkedMarker;
    if (child)
        gabi::tail_ptr<void>(child->mVtable->mDraw.get(), child, info);
}

/* 0265A710 */
void hdDungeonMap_c::clearOpeningCursor() {
    sDungeonCursorManager->mActive = CURSOR_MGR_FOCUS;
    mOpenPending = 0;
}

/* 0265D774 */
void hdDungeonMap_c::captureBusyForClosing() {
    mClosePending = 0;
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdPadUiBusy(ui))
        mClosePending = 1;
}

/* 0265D578 */
void hdDungeonMap_c::refreshChildLayouts() {
    clearParts();
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_DRAW].get(), this);
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265D978 */
void hdDungeonMap_c::finishZoomOutIfReady() {
    HdPadMap* map = mpMap;
    if (map && hdDungeonMapCanZoom(map) && mZoomMode == 1) {
        HdPadDungeon* dungeon = gamepadView();
        hdPadChangeState(&dungeon->mState, &sDungeonZoomOutState_l);
        mZoomMode = 0;
    }
}

/* 0265D9DC */
void hdDungeonMap_c::applyRequestedMode() {
    if (mpMap) {
        u8 requested = mSelectedFloor;
        HdPadMap* map = mpMap;
        mDisplayedFloor = requested;
        mZoomMode = 0;
        map->mZoomMode = requested;
    }
}

/* 0265DA04 */
void hdDungeonMap_c::updateRequestedMode() {
    u8 mode = mDisplayedFloor;
    u8 requested = mSelectedFloor;
    if (mode == requested)
        finishZoomOutIfReady();
    else
        applyRequestedMode();
}

// The generated immediate lfs/stfs round trip preserves the source NaN payload.
// Keep a float-store tag while avoiding the generic typed load's sNaN quieting.
static void dungeonCopyPositionFloat(be<f32>& destination, const be<f32>& source) {
    u32 bits = *hdPartsView<be<u32>>(gabi::ea(&source));
    f32 value;
    __builtin_memcpy(&value, &bits, sizeof(value));
    destination = value;
}

/* 0265DC64 */
static void hdDungeonActorMapPosition(hdDungeonMap_c* self, fopAc_ac_c* actor,
                                      HdPadPoint3* position, be<f32>* angle) {
    if (!position || !angle)
        return;
    if (actor) {
        // Keep each float load/store before reading the next coordinate.
        dungeonCopyPositionFloat(position->x, actor->current.pos.x);
        dungeonCopyPositionFloat(position->y, actor->current.pos.y);
        dungeonCopyPositionFloat(position->z, actor->current.pos.z);
        f32 heading = f32(s16(actor->shape_angle.y));
        f32 scale = sDungeonAngleScale_l;
        f32 halfTurn = sDungeonHalfTurn_l;
        *angle = gabi::fmadds(heading, scale, halfTurn);
    } else {
        f32 zero = l_hdDungeonZero;
        position->x = zero;
        position->y = zero;
        position->z = zero;
        *angle = zero;
    }
}

/* 02659C24 */
void hdDungeonMap_c::resetReceiverLinks() {
    enum { FLOOR_CHILDREN = 8, RECEIVER_BUTTONS = 8 };
    for (u32 childIndex = 0; childIndex < FLOOR_CHILDREN; ++childIndex) {
        for (u32 button = 0; button < RECEIVER_BUTTONS; ++button) {
            hdParts_c* part = child(childIndex);
            hdButtonPart_c* floor = static_cast<hdButtonPart_c*>(part);
            hdDungeonButtonSetReceiver(floor, button, NULL);
        }
    }
    for (u32 button = 0; button < RECEIVER_BUTTONS; ++button)
        hdDungeonReceiverSetButton(floorReceiver(this), button, NULL);
}

/* 02659E64 */
void hdDungeonMap_c::connectSecondaryNavigation() {
    hdParts_c* part = child(1);
    if (part) {
        hdButtonReceiver_l* receiver = floorReceiver(this);
        hdButtonPart_c* button = static_cast<hdButtonPart_c*>(part);
        hdDungeonButtonSetReceiver(button, 7, receiver);
    }
}

/* 0265DB1C */
void hdDungeonMap_c::connectSecondaryReceiver(hdButtonReceiver_l* receiver) {
    hdParts_c* part = child(1);
    if (part) {
        hdButtonPart_c* button = static_cast<hdButtonPart_c*>(part);
        hdDungeonButtonSetReceiver(button, 0, receiver);
    }
}

/* 0265DA1C */
void hdDungeonMap_c::connectReturnReceiver(hdButtonReceiver_l* receiver) {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (!hdPadUiBusy(ui) && receiver)
        hdDungeonReceiverSetButton(receiver, 3, floorReceiver(this));
}

/* 0265DC08 */
void hdDungeonMap_c::connectPrimaryReceiver(hdButtonReceiver_l* receiver) {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (!hdPadUiBusy(ui))
        hdDungeonReceiverSetButton(floorReceiver(this), 0, receiver);
}

/* 02659CC4 */
void hdDungeonMap_c::connectSelectedFloor() {
    u32 selected = mCursorFloor;
    u32 index = floorChildIndex(selected);
    u32 count = u32(mChildren.mNum);
    gptr<hdParts_c>* children = mChildren.mpData;
    hdButtonPart_c* floor = static_cast<hdButtonPart_c*>(children[index < count ? index : 0].get());
    hdButtonPart_c* secondary = static_cast<hdButtonPart_c*>(children[1 < count ? 1 : 0].get());
    if (floor && secondary) {
        hdDungeonButtonSetReceiver(floor, 0, secondary->mpReceiver);
        hdDungeonButtonSetReceiver(secondary, 3, floor->mpReceiver);
    }
}

/* 02659D64 */
void hdDungeonMap_c::connectFloorChain() {
    u32 floorCount = mMapData.mBossCount;
    s32 remaining = s32(floorCount - 1);
    if (!isBossIndex(floorCount))
        remaining = 0;
    for (u32 order = 0; remaining > 0; ++order, --remaining) {
        u32 firstIndex = sDungeonFloorOrder_l.mChildren[order];
        u32 count = u32(mChildren.mNum);
        gptr<hdParts_c>* children = mChildren.mpData;
        u32 secondIndex = sDungeonFloorOrder_l.mChildren[order + 1];
        hdButtonPart_c* first = static_cast<hdButtonPart_c*>(children[firstIndex < count ? firstIndex : 0].get());
        hdButtonPart_c* second = static_cast<hdButtonPart_c*>(children[secondIndex < count ? secondIndex : 0].get());
        if (first && second) {
            hdDungeonButtonSetReceiver(first, 0, second->mpReceiver);
            hdDungeonButtonSetReceiver(second, 3, first->mpReceiver);
        }
    }
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdDungeonSecondaryFloorVisible(ui))
        connectSelectedFloor();
}

/* 02659E94 */
void hdDungeonMap_c::connectFloorRoot() {
    u32 floorCount = mMapData.mBossCount;
    s32 remaining = isBossIndex(floorCount) ? s32(floorCount) : 0;
    for (u32 order = 0; remaining > 0; ++order, --remaining) {
        u32 index = sDungeonFloorOrder_l.mChildren[order];
        hdButtonPart_c* floor = static_cast<hdButtonPart_c*>(child(index));
        if (floor)
            hdDungeonButtonSetReceiver(floor, 7, floorReceiver(this));
    }
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdDungeonSecondaryFloorVisible(ui))
        connectSecondaryNavigation();
}

/* 02659F5C */
void hdDungeonMap_c::refreshNavigation() {
    resetReceiverLinks();
    connectFloorChain();
    connectFloorRoot();
    updateFloorReceiver();
}

// Inlined into the three marker transition walkers.
void hdDungeonMap_c::visit(void (*operation)(hdParts_c*)) {
    hdParts_c* marker = mpPlayerMarker;
    if (marker) operation(marker);
    marker = mpLinkedMarker;
    if (marker) operation(marker);
    marker = mpBossMarker;
    if (marker) operation(marker);
    marker = mpSpecialMarker;
    if (marker) operation(marker);
    marker = mpPartnerMarker;
    if (marker) operation(marker);
    for (u32 index = 0; index < 3; ++index) {
        if (mpItemMarkers[index])
            operation(mpItemMarkers[index]);
    }
    s32 doorCount = mDoorCount;
    for (u32 index = 0; s32(index) < doorCount; ++index) {
        u32 slot = index < 64 ? index : 0;
        if (mpDoors[slot]) {
            operation(mpDoors[slot]);
            doorCount = mDoorCount;
        }
    }
    s32 treasureCount = mTreasureCount;
    for (u32 index = 0; s32(index) < treasureCount; ++index) {
        u32 slot = index < 32 ? index : 0;
        if (mpTreasures[slot]) {
            operation(mpTreasures[slot]);
            treasureCount = mTreasureCount;
        }
    }
    s32 enemyCount = mEnemyCount;
    for (u32 index = 0; s32(index) < enemyCount; ++index) {
        u32 slot = index < 16 ? index : 0;
        if (mpEnemies[slot]) {
            operation(mpEnemies[slot]);
            enemyCount = mEnemyCount;
        }
    }
    s32 specialItemCount = mSpecialItemCount;
    for (u32 index = 0; s32(index) < specialItemCount; ++index) {
        u32 slot = index < 16 ? index : 0;
        if (mpSpecialItems[slot]) {
            operation(mpSpecialItems[slot]);
            specialItemCount = mSpecialItemCount;
        }
    }
}

/* 0265A0B0 */
void hdDungeonMap_c::beginIn() {
    visit(hdDungeonMarkerBeginIn);
}

/* 0265A7E8 */
void hdDungeonMap_c::beginOut() {
    visit(hdDungeonMarkerBeginOut);
}

/* 0265D1CC */
void hdDungeonMap_c::resetDisplay() {
    visit(hdDungeonMarkerResetDisplay);
}

/* 0265A664 */
void hdDungeonMap_c::updateOpeningTransition() {
    hdAnimSet* player = animSet();
    if (hdAnimSet_isEnd(player, 0)) {
        HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
        if (hdPadUiBusy(ui)) {
            if (!mOpenPending) {
                hdPadChangeState(&gamepadView()->mState, &sDungeonNoDataState_l);
                return;
            }
        }
        hdPadChangeState(&gamepadView()->mState, &sDungeonDisplayState_l);
    }
    hdDungeonBounds(gamepadView(), &mBounds);
    hdDungeonUpdateParts(gamepadView());
}

/* 0265AAD8 */
void hdDungeonMap_c::updateClosingTransition() {
    hdAnimSet* player = animSet();
    if (hdAnimSet_isEnd(player, 5))
        hdPadChangeState(&gamepadView()->mState, &sDungeonNoDataState_l);
    hdDungeonUpdateParts(gamepadView());
}

/* 0265AB88 */
void hdDungeonMap_c::updateReverseTransition() {
    hdAnimSet* player = animSet();
    if (hdAnimSet_isEnd(player, 5))
        hdPadChangeState(&gamepadView()->mState, &sDungeonDisplayState_l);
    hdDungeonUpdateParts(gamepadView());
}

/* 0265ABF0 */
void hdDungeonMap_c::waitForDisplay() {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (!hdPadUiBusy(ui))
        hdPadChangeState(&gamepadView()->mState, &sDungeonChangeToDisplayState_l);
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265AD28 */
void hdDungeonMap_c::updateBlankTransition() {
    hdAnimSet* player = animSet();
    if (hdAnimSet_isEnd(player, 6))
        hdPadChangeState(&gamepadView()->mState, &sDungeonBlankState_l);
    hdDungeonBounds(gamepadView(), &mBounds);
    updatePaneAndIcons();
}

/* 0265D19C */
void hdDungeonMap_c::enterInAll() {
    hdPadChangeState(&gamepadView()->mState, &sDungeonInAllState_l);
}

/* 0265D1AC */
void hdDungeonMap_c::enterOutAll() {
    hdPadChangeState(&gamepadView()->mState, &sDungeonOutAllState_l);
}

/* 0265D4F4 */
void hdDungeonMap_c::forceBlank() {
    hdPadChangeState(&gamepadView()->mState, &sDungeonBlankState_l);
    hdAnimSet* player = animSet();
    hdAnimSet_operationCC(player, 0, 2);
    finishMap();
    clearParts();
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_DRAW].get(), this);
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265A34C */
void hdDungeonMap_c::enterOpeningTransition() {
    rebuildDisplay();
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 0, 2, rate);
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (!hdPadUiBusy(ui)) {
        beginMapZoomOut();
        beginIn();
    }
}

/* 0265AA84 */
void hdDungeonMap_c::enterClosingTransition() {
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 5, 5, rate);
    beginMapZoomIn();
    beginOut();
}

/* 0265AB30 */
void hdDungeonMap_c::enterReverseTransition() {
    rebuildDisplay();
    f32 rate = sDungeonReverseAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 5, 5, rate);
    beginMapZoomOut();
    beginIn();
}

/* 0265AC54 */
void hdDungeonMap_c::enterBlankTransition() {
    f32 rate = sDungeonAnimationRate;
    hdAnimSet* player = animSet();
    hdAnimSet_play(player, 6, 2, rate);
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (!hdPadUiBusy(ui)) {
        if (!mClosePending) {
            beginMapZoomIn();
            beginOut();
        }
    }
}

/* 0265D468 */
void hdDungeonMap_c::forceDisplay() {
    hdPadChangeState(&gamepadView()->mState, &sDungeonDisplayState_l);
    rebuildDisplay();
    hdAnimSet* player = animSet();
    hdDungeonStopAnimation(player, 0, 2);
    beginMapZoomToPlayer();
    resetDisplay();
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_DRAW].get(), this);
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

static hdPartsPane_c* dungeonFindRootPane(hdPartsLyt_c* layout, hdScreenString* name) {
    name->assureTermination();
    hdPartsPane_c* root = layout->mpRootPane;
    hdPartsPaneSRTVtbl_c* table = hdPartsView<hdPartsPaneSRTVtbl_c>(gabi::ea(root->mVtable.get()));
    u32 target = table->mFindPane.get();
    const char* text = name->mText;
    return gabi::call_ptr<hdPartsPane_c*>(target, root, text, 1);
}

/* 0265BCB4 */
void hdDungeonMap_c::findFramePanes() {
    for (u32 index = 0; index < 4; ++index) {
        hdPartsLyt_c* layout = mpLayout->lyt();
        mpFramePanes[index] = dungeonFindRootPane(layout, &sDungeonFrameNames_l.mNames[index]);
    }
}

/* 0265BD68 */
void hdDungeonMap_c::findSelectedPane() {
    hdPartsLyt_c* layout = mpLayout->lyt();
    mpMapAreaPane = dungeonFindRootPane(layout, &sDungeonSelectedPaneName_l);
}

/* 0265BDE4 */
void hdDungeonMap_c::findCaptionPane() {
    hdPartsLyt_c* layout = mpLayout->lyt();
    mpCaption = reinterpret_cast<hdTextPane_c*>(dungeonFindRootPane(layout, &sDungeonCaptionPaneName_l));
}

/* 0265BE60 */
void hdDungeonMap_c::findFloorPanes() {
    for (u32 index = 0; index < 8; ++index) {
        hdPartsLyt_c* layout = mpLayout->lyt();
        mpFloorPanes[index] = static_cast<hdPartsPaneSRT_c*>(dungeonFindRootPane(layout, &sDungeonFloorNames_l.mNames[index]));
    }
}

/* 0265BF14 */
void hdDungeonMap_c::findFloorLabels() {
    for (u32 index = 0; index < 5; ++index) {
        u32 floorIndex = sDungeonFloorOrder_l.mChildren[index];
        hdPartsPane_c* floor = mpFloorPanes[floorIndex < 8 ? floorIndex : 0];
        if (!floor)
            continue;
        hdScreenString* name = &sDungeonFloorLabelNames_l.mNames[index];
        hdPartsPaneVtbl_c* vtable = floor->mVtable;
        name->assureTermination();
        hdPartsPaneSRTVtbl_c* table = hdPartsView<hdPartsPaneSRTVtbl_c>(gabi::ea(vtable));
        u32 target = table->mFindPane.get();
        const char* text = name->mText;
        mpFloorLabels[index] = gabi::call_ptr<hdPartsPane_c*>(target, floor, text, 1);
    }
}

/* 0265C3BC */
void hdDungeonMap_c::destroyReceiver() {
    if (mpInputMode) {
        hdButtonReceiver_l* receiver = floorReceiver(this);
        if (receiver)
            gabi::call_ptr<void>(receiver->mVtable->mDestroy.get(), receiver, 3);
        mpInputMode = NULL;
    }
}

/* 0265C418 */
void hdDungeonMap_c::releaseResources() {
    hdDungeonReleaseFirstMap(this, mpMaps[0]);
    hdDungeonReleaseSecondMap(this, mpMaps[1]);
    hdDungeonReleaseThirdMap(this, mpMaps[2]);
    hdDungeonReleaseFourthMap(this, mpMaps[3]);
    hdDungeonReleaseFifthMap(this, mpMaps[4]);
    hdDungeonReleaseSixthMap(this, mpMaps[5]);
    hdDungeonReleasePlayerMarker(this, mpPlayerMarker);
    hdDungeonReleaseLinkedMarker(this, mpLinkedMarker);
    hdDungeonReleaseBossMarker(this, mpBossMarker);
    hdDungeonReleaseSpecialMarker(this, mpSpecialMarker);
    hdDungeonReleasePartnerMarker(this, mpPartnerMarker);
    for (u32 index = 0; index < 3; ++index)
        hdDungeonReleaseItemMarker(this, mpItemMarkers[index]);
    for (u32 index = 0; index < 64; ++index)
        hdDungeonReleaseDoorMarker(this, mpDoors[index]);
    for (u32 index = 0; index < 32; ++index)
        hdDungeonReleaseTreasureMarker(this, mpTreasures[index]);
    for (u32 index = 0; index < 16; ++index)
        hdDungeonReleaseEnemyMarker(this, mpEnemies[index]);
    for (u32 index = 0; index < 16; ++index)
        hdDungeonReleaseSpecialItemMarker(this, mpSpecialItems[index]);
}

/* 0265C624 */
void hdDungeonMap_c::destroyResources() {
    destroyReceiver();
    gabi::call_ptr<void>(mVtable->mDeleteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    releaseResources();
    detachArchives();
}

/* 0265BFE4 */
void hdDungeonMap_c::initializeCaption() {
    if (!mpCaption)
        return;
    hdTextPane_c* caption = hdPartsView<hdTextPane_c>(gabi::ea(mpCaption.get()));
    caption->setCharacterCapacity(sDungeonCaptionCapacity_l);
    caption = hdPartsView<hdTextPane_c>(gabi::ea(mpCaption.get()));
    u32 target = caption->mpVTable[0xC4 / 4].get();
    gabi::call_ptr<void>(target, caption, &sDungeonEmptyCaption_l, 0);
}

/* 0265C048 */
void hdDungeonMap_c::initializeFloorLabels() {
    for (u32 index = 0; index < 5; ++index) {
        if (!mpFloorLabels[index])
            continue;
        hdTextPane_c* label = hdPartsView<hdTextPane_c>(gabi::ea(mpFloorLabels[index].get()));
        u32 capacity = sDungeonFloorCapacities_l.mCapacity[index];
        label->setCharacterCapacity(capacity);
        label = hdPartsView<hdTextPane_c>(gabi::ea(mpFloorLabels[index].get()));
        u32 target = label->mpVTable[0xC4 / 4].get();
        gabi::call_ptr<void>(target, label, &sDungeonEmptyFloorLabel_l, 0);
    }
}

/* 0265C148 */
void hdDungeonMap_c::createFloorReceiver() {
    hdPartsPane_c* selected = mpMapAreaPane;
    hdScreenHeap* heap = sDungeonUiSystem_l->mHeap;
    if (selected) {
        void* storage = hdDungeonAllocateReceiver(0x60, heap, 4);
        if (storage)
            mpInputMode = reinterpret_cast<HdPadInputMode*>(hdDungeonConstructReceiver(storage, mpMapAreaPane, 0));
        else
            mpInputMode = NULL;
    }
    hdDungeonReceiverFlags_l* flags = hdPartsView<hdDungeonReceiverFlags_l>(gabi::ea(floorReceiver(this)));
    flags->mEnabled = 1;
    flags = hdPartsView<hdDungeonReceiverFlags_l>(gabi::ea(floorReceiver(this)));
    flags->mSecondaryEnabled = 1;
}

/* 0265AD98 */
void hdDungeonMap_c::beginMapZoomToEntrance() {
    HdPadMap* map = mpMap;
    if (map)
        hdPadChangeState(&map->mState, &sMapEntranceState_l);
}

/* 0265ABE0 */
void hdDungeonSelectUiModeForward(HdPadDungeon*) {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    hdDungeonSelectUiMode(ui);
}

/* 0265D974 */
void hdDungeonMap_c::refreshDungeonSelection() {
    refreshLinked();
}

/* 0265DA80 */
void hdDungeonMap_c::connectSelectedReceiver(hdButtonReceiver_l* receiver) {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdPadUiBusy(ui))
        return;
    ui = l_hdDungeonUi.mOwner->mUi;
    if (hdDungeonSecondaryFloorVisible(ui))
        return;
    u32 index = floorChildIndex(mCursorFloor);
    u32 count = u32(mChildren.mNum);
    gptr<hdParts_c>* children = mChildren.mpData.get();
    hdParts_c* child = children[index < count ? index : 0];
    hdDungeonButtonSetReceiver(static_cast<hdButtonPart_c*>(child), 0, receiver);
}

/* 0265DB48 */
void hdDungeonMap_c::connectActiveFloorReceiver(hdButtonReceiver_l* receiver) {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdPadUiBusy(ui))
        return;
    ui = l_hdDungeonUi.mOwner->mUi;
    if (hdDungeonSecondaryFloorVisible(ui)) {
        connectSecondaryReceiver(receiver);
        return;
    }
    u32 index = floorChildIndex(mCursorFloor);
    u32 count = u32(mChildren.mNum);
    gptr<hdParts_c>* children = mChildren.mpData.get();
    hdParts_c* child = children[index < count ? index : 0];
    hdDungeonButtonSetReceiver(static_cast<hdButtonPart_c*>(child), 0, receiver);
}

/* 0265AFCC */
void hdDungeonMap_c::snapshotMapPosition() {
    f32 scale = playerAngle();
    mPreviousAngle = scale;
    playerMapPosition(&mPreviousPlayerPosition);
    f32 zero = l_hdDungeonZero;
    mAngleDelta = zero;      /* #2 reads 0x3E0/0x3E4 as the zoom start offset */
    mPositionDelta = zero;
}

/* 0265B018 */
void hdDungeonMap_c::enterMapZoomIn() {
    HdPadMap* map = mpMap;
    if (map)
        hdPadChangeState(&map->mState, &sMapZoomInState_l);
    snapshotMapPosition();
}

/* 0265B064 */
void hdDungeonMap_c::updateMapDrag() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!mpMap)
        return;
    if (isMarkerOnFloor()) {
        updatePlayerMapPosition();
        updateMapRotation();
        return;
    }
    HdPadMap* map = mpMap;
    hdDungeonMapPaneLink_l* link = hdPartsView<hdDungeonMapPaneLink_l>(gabi::ea(map));
    HdPadPane* pane = link->mpPane;
    if (pane) {
        gabi::FrameLocal<HdPadPoint3> origin(0x10);
        hdDungeonPaneDragOrigin(origin, pane);
        f32 x = mTouchPosition.x;
        f32 originX = origin->x;
        f32 originY = origin->y;
        f32 deltaX = gabi::fsubs_ppc(x, originX);
        f32 y = mTouchPosition.y;
        map = mpMap;
        f32 deltaY = gabi::fsubs_ppc(y, originY);
        gabi::FrameLocal<HdPadPoint> delta(0x8);
        delta->x = deltaX;
        delta->y = deltaY;
        hdDungeonMapSetDelta(map, delta);
        map = mpMap;
    }
    hdDungeonMapSetAngle(map, l_hdDungeonZero);
}

/* 0265B120 */
void hdDungeonMap_c::updateMapDragForward() {
    updateMapDrag();
}

/* 0265B124 */
void hdDungeonMap_c::enterMapZoomOut() {
    updateMapDragForward();
    HdPadMap* map = mpMap;
    if (map)
        hdPadChangeState(&map->mState, &sMapZoomOutState_l);
}

static bool dungeonMapStateEquals(HdPadStateController* machine, HdPadState* expectedState) {
    hdPartsStateId_c* current = hdDungeonCurrentMapState(machine);
    u32 currentTarget = current->mVtable->mGetKey.get();
    hdPartsStateId_c* expected = hdPartsView<hdPartsStateId_c>(gabi::ea(expectedState));
    hdPartsStateIdVtbl_c* expectedTable = expected->mVtable;
    u32 currentKey = gabi::call_ptr<u32>(currentTarget, current);
    u32 expectedKey = gabi::call_ptr<u32>(expectedTable->mGetKey.get(), expected);
    return currentKey == expectedKey;
}

/* 0265B16C */
void hdDungeonMap_c::updateMapZoomOut() {
    HdPadMap* map = mpMap;
    if (map && !dungeonMapStateEquals(&map->mState, &sMapZoomOutState_l))
        hdPadChangeState(&gamepadView()->mState, &sDungeonDisplayState_l);
    updateMapDrag();
    hdDungeonUpdateParts(gamepadView());
}

/* 0265B234 */
void hdDungeonMap_c::updateMapCentering() {
    HdPadMap* map = mpMap;
    if (map && !dungeonMapStateEquals(&map->mState, &sMapCenteringState_l))
        hdPadChangeState(&gamepadView()->mState, &sDungeonDisplayState_l);
    hdDungeonUpdateParts(gamepadView());
}

/* 0265C204 */
void hdDungeonMap_c::initializeMapPanes() {
    findFramePanes();
    findSelectedPane();
    findCaptionPane();
    findFloorPanes();
    findFloorLabels();
    initializeCaption();
    initializeFloorLabels();
    createFloorReceiver();
    f32 ratio = widthRatio();
    mMapWidth = ratio;
    hdDungeonMapBoundsForward(this, &mBounds);
    hdAnimSet* player = animSet();
    hdAnimSet_operationCC(player, 0, 2);
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
}

/* 0265CAFC */
void hdDungeonMap_c::drawMap(void* drawInfo, void* renderer) {
    gabi::call_ptr<void>(gamepadView()->mVTable[0x9C / 4].get(), this, renderer);
    gabi::call_ptr<void>(gamepadView()->mVTable[0xC4 / 4].get(), this, renderer);
    if (!dungeonMapStateEquals(&gamepadView()->mState, &sDungeonNoDataState_l))
        drawMapLayers(drawInfo, renderer);
}

/* 0265C6C8 */
void hdDungeonMap_c::drawItems(void* info) {
    for (u32 index = 0; index < 3; ++index) {
        hdParts_c* child = mpItemMarkers[index];
        if (child) {
            child = mpItemMarkers[index];
            gabi::call_ptr<void>(child->mVtable->mDraw.get(), child, info);
        }
    }
}

/* 026593DC */
void hdDungeonMap_c::connectMapContext() {
    HdPadMap* map = mpMap;
    if (!map)
        return;
    hdDungeonMapLinks_l* links = hdPartsView<hdDungeonMapLinks_l>(gabi::ea(map));
    links->mpPane = mpMapPane;
    map = mpMap;
    links = hdPartsView<hdDungeonMapLinks_l>(gabi::ea(map));
    links->mpMapScale = &mMapScale;
    map = mpMap;
    links = hdPartsView<hdDungeonMapLinks_l>(gabi::ea(map));
    links->mpFloorContext = &mMapData;
    map = mpMap;
    map->mZoomMode = mDisplayedFloor;
    setPartScales();
    map = mpMap;
    mZoomMode = 0;
    gabi::call_ptr<void>(map->mVTable[0x1D4 / 4].get(), map);
    map = mpMap;
    gabi::call_ptr<void>(map->mVTable[0x1DC / 4].get(), map);
    map = mpMap;
    gabi::call_ptr<void>(map->mVTable[0x1E4 / 4].get(), map);
}

/* 02659754 */
void hdDungeonMap_c::updateFloorLabels() {
    f32 frame = sDungeonFloorLabelFrame_l;
    hdAnimSet* player = animSet();
    hdAnimSet_setFrame(player, 3, 3, frame);
    u32 first = u32(mFirstFloor) - 0x7E;
    u32 floorCount = mMapData.mBossCount;
    if (first < 3 && s32(floorCount) > 0) {
        for (u32 index = 0; index < floorCount; ++index) {
            hdPartsPane_c* label = mpFloorLabels[index < 5 ? index : 0];
            setPaneLabel(hdPartsView<hdTextPane_c>(gabi::ea(label)), hdPartsView<hdUiSafeString>(gabi::ea(&sDungeonFloorLabelTexts_l.mNames[first + index])));
        }
    }
    setPaneLabel(hdPartsView<hdTextPane_c>(gabi::ea(mpFloorLabels[3])), hdPartsView<hdUiSafeString>(gabi::ea(&sDungeonFloorLabelTexts_l.mNames[6])));
    u32 childCount = u32(mChildren.mNum);
    gptr<hdParts_c>* children = mChildren.mpData;
    for (u32 index = 0; index < 5; ++index) {
        u32 childIndex = sDungeonFloorOrder_l.mChildren[index];
        hdParts_c* child = children[childIndex < childCount ? childIndex : 0];
        if (child) {
            u8 floor = 10;
            if (s32(index) < s32(floorCount))
                floor = u8(u32(mFirstFloor) + index);
            hdPartsView<hdDungeonFloorButton_l>(gabi::ea(child))->mFloor = floor;
            childCount = u32(mChildren.mNum);
            children = mChildren.mpData;
        }
    }
    hdParts_c* secondary = children[childCount > 1 ? 1 : 0];
    if (secondary)
        hdPartsView<hdDungeonFloorButton_l>(gabi::ea(secondary))->mFloor = 0x85;
}

/* 0265C2A8 */
u32 hdDungeonMap_c::loadMap(u32 arg1, u32 arg2, u32 arg3) {
    hdParts_c* parts = this;
    parts->attachArchives(sDungeonMapArchiveNames_l.mNames, 3);
    u32 loaded = gabi::call_ptr<u32>(parts->mVtable->mLoad.get(), this,
        &sDungeonMapLayoutName_l, parts->mpResAccessor.get(), arg1, arg2, arg3);
    if (!loaded)
        return 0;
    if (!gabi::call_ptr<u32>(parts->mVtable->mCreateChildren.get(), this))
        return 0;
    initializeMapResources();
    initializeMapPanes();
    parts->startState(hdPartsView<hdPartsStateId_c>(gabi::ea(&sDungeonBlankState_l)));
    return 1;
}

/* 0265DCFC */
void hdDungeonMap_c::updateActorPositions() {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    hdDungeonPositionUi_l* positionUi = hdPartsView<hdDungeonPositionUi_l>(gabi::ea(ui));
    if (positionUi->mFreezePosition)
        return;
    if (u32(positionUi->mMode) == 3) {
        hdDungeonPositionPlay_l* play = hdDungeonPositionPlay();
        hdDungeonActorMapPosition(this, play->mpPartner, &mSpecialPosition, reinterpret_cast<be<f32>*>(&mSpecialAngle));
        for (u32 index = 0; index < 3; ++index) {
            fopAc_ac_c* actor = hdDungeonFindItemActor(index);
            hdDungeonActorMapPosition(this, actor, &mItemPositions[index], reinterpret_cast<be<f32>*>(&mItemAngles[index]));
        }
        return;
    }
    fopAc_ac_c* actor = static_cast<fopAc_ac_c*>(playerActor());
    f32 scale = sDungeonAngleScale_l;
    f32 halfTurn = sDungeonHalfTurn_l;
    if (actor) {
        dungeonCopyPositionFloat(mPlayerPosition.x, actor->current.pos.x);
        dungeonCopyPositionFloat(mPlayerPosition.y, actor->current.pos.y);
        dungeonCopyPositionFloat(mPlayerPosition.z, actor->current.pos.z);
        f32 heading = f32(s16(actor->shape_angle.y));
        mBossPosition.z = gabi::fmadds(heading, scale, halfTurn);
        return;
    }
    hdDungeonPositionSave_l* save = sDungeonPositionSave_l;
    u8 mode = save->mResumeMode;
    if (mode == 1 || mode == 2) {
        dungeonCopyPositionFloat(mPlayerPosition.x, save->mPosition.x);
        dungeonCopyPositionFloat(mPlayerPosition.y, save->mPosition.y);
        dungeonCopyPositionFloat(mPlayerPosition.z, save->mPosition.z);
        save = sDungeonPositionSave_l;
        f32 heading = f32(s16(save->mAngle));
        mBossPosition.z = gabi::fmadds(heading, scale, halfTurn);
        return;
    }
    f32 zero = l_hdDungeonZero;
    mPlayerPosition.y = zero;
    mPlayerPosition.z = zero;
    mBossPosition.z = zero;
    mPlayerPosition.x = zero;
}

/* 026598CC */
void hdDungeonMap_c::refreshFloorLabels() {
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdDungeonSecondaryFloorVisible(ui)) {
        updateFloorLabels();
        return;
    }
    u32 count = mMapData.mBossCount;
    if (count - 2 >= 4)
        return;
    hdAnimSet* player = animSet();
    hdAnimSet_setFrame(player, 3, 3, f32(5 - count));
    u32 first = u32(mFirstFloor) - 0x7E;
    if (first < 3) {
        for (u32 index = 0; index < count; ++index) {
            hdPartsPane_c* label = mpFloorLabels[index < 5 ? index : 0];
            setPaneLabel(hdPartsView<hdTextPane_c>(gabi::ea(label)), hdPartsView<hdUiSafeString>(gabi::ea(&sDungeonFloorLabelTexts_l.mNames[first + index])));
        }
    }
    for (u32 index = 0; index < 5; ++index) {
        u32 childIndex = sDungeonFloorOrder_l.mChildren[index];
        u32 childCount = u32(mChildren.mNum);
        gptr<hdParts_c>* children = mChildren.mpData;
        hdParts_c* child = children[childIndex < childCount ? childIndex : 0];
        if (child) {
            u8 floor = 10;
            if (index < count)
                floor = u8(u32(mFirstFloor) + index);
            hdPartsView<hdDungeonFloorButton_l>(gabi::ea(child))->mFloor = floor;
        }
    }
}

/* 02659B08 */
void hdDungeonMap_c::refreshItemButtons() {
    hdParts_c* parts = this;
    for (u32 index = 0; index < 3; ++index) {
        u32 childIndex = sDungeonItemButtonOrder_l.mChildren[index];
        u32 count = u32(parts->mChildren.mNum);
        gptr<hdParts_c>* children = parts->mChildren.mpData;
        hdDungeonPrepareItemButton(children[childIndex < count ? childIndex : 0], index);
    }
    hdAnimSet* player = animSet();
    hdDungeonStopAnimation(player, 4, 4);
    for (u32 index = 0; index < 3; ++index) {
        u8 present = (&mMapData.mMapOwned)[index];   /* #1 names these three bytes mMapOwned/mReserved/mPlayerVisible; the item buttons read them as one flag per item */
        u32 childIndex = sDungeonItemButtonOrder_l.mChildren[index];
        gptr<hdParts_c>* children = parts->mChildren.mpData;
        u32 count = u32(parts->mChildren.mNum);
        hdParts_c* child = children[childIndex < count ? childIndex : 0];
        if (present)
            hdDungeonActivateItemButton(child);
        else
            hdDungeonDeactivateItemButton(child);
    }
}

/* 02659488 */
void hdDungeonMap_c::connectMapMarkers() {
    HdPadMap* map = mpMap;
    if (!map)
        return;
    hdParts_c* player = mpPlayerMarker;
    if (player)
        hdDungeonMarkerSetMap(player, map);
    auto bindSingle = [this](auto& slot) {
        hdParts_c* child = slot;
        if (child) {
            HdPadMap* currentMap = mpMap;
            hdDungeonMarkerSetMap(child, currentMap);
        }
    };
    bindSingle(mpLinkedMarker);
    bindSingle(mpBossMarker);
    bindSingle(mpSpecialMarker);
    bindSingle(mpPartnerMarker);
    for (u32 index = 0; index < 3; ++index) {
        if (mpItemMarkers[index]) {
            map = mpMap;
            hdDungeonMarkerSetMap(mpItemMarkers[index], map);
        }
    }
    auto bindGroup = [this](auto& slots, be<s32>& countField) {
        constexpr u32 capacity = sizeof(slots) / sizeof(slots[0]);
        u32 count = countField;
        for (u32 index = 0; s32(index) < s32(count); ++index) {
            u32 slot = index < capacity ? index : 0;
            if (slots[slot]) {
                HdPadMap* currentMap = mpMap;
                hdDungeonMarkerSetMap(slots[slot], currentMap);
                count = countField;
            }
        }
    };
    bindGroup(mpDoors, mDoorCount);
    bindGroup(mpTreasures, mTreasureCount);
    bindGroup(mpEnemies, mEnemyCount);
    bindGroup(mpSpecialItems, mSpecialItemCount);
}

/* 02659FA0 */
void hdDungeonMap_c::rebuildDisplay() {
    updateCaption();
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    bool busy = hdPadUiBusy(ui);
    hdAnimSet* player = animSet();
    if (busy) {
        hdDungeonStopAnimation(player, 5, 5);
        finishMap();
        clearParts();
        return;
    }
    hdAnimSet_operationCC(player, 5, 5);
    hdDungeonMap_c* selected = hdDungeonSelectMap(this);
    selected->updateMapBounds();
    refreshPartner();
    refreshLinked();
    mDisplayedFloor = mSelectedFloor;
    updatePartner();
    updateWorldMarkers();
    saveFloorForOpen();
    connectMapContext();
    connectMapMarkers();
    refreshFloorLabels();
    hdDungeonOpenAnimated(this);
    hdDungeonWaitAnimated(this);
    refreshItemButtons();
    refreshNavigation();
    hdDungeonMap_c* updated = hdDungeonApplyZoomMode(this);
    updated->updateMarkers();
}

/* 0265D070 */
u32 hdDungeonMap_c::loadMapLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sDungeonUiSystem_l->mHeap;
    void* storage = hdParts_new(0x110, heap, 4);
    hdPartsLayout_c* layout = NULL;
    if (storage)
        layout = hdPartsLayout_c::construct(static_cast<hdPartsLayout_c*>(storage));
    hdParts_c* parts = this;
    parts->mpLayout = layout;
    if (!layout || !layout->vLoad(name, arg1, arg2, arg3, 7, 6, desc))
        return 0;
    for (u32 index = 0; index < 7; ++index) {
        layout = parts->mpLayout;
        u32 group = sDungeonAnimationGroupOrder_l.mIndices[index];
        hdDungeonBindAnimation(layout->animSet(), index, &sDungeonAnimationNames_l.mNames[index],
            &sDungeonAnimationGroups_l.mNames[group]);
    }
    gabi::call_ptr<void>(gamepadView()->mVTable[HD_PAD_UPDATE].get(), this);
    return 1;
}

void hdDungeonMap_c::refreshDisplayContents(bool refreshItems) {
    updateCaption();
    mOpenPending = 0;
    HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
    if (hdPadUiBusy(ui)) {
        if (dungeonMapStateEquals(&gamepadView()->mState, &sDungeonInAllState_l) && !mClosePending)
            mOpenPending = 1;
        mClosePending = 0;
        return;
    }
    mClosePending = 0;
    ui = l_hdDungeonUi.mOwner->mUi;
    bool busy = hdPadUiBusy(ui);
    hdAnimSet* player = animSet();
    if (busy) {
        hdDungeonStopAnimation(player, 5, 5);
        finishMap();
        clearParts();
        return;
    }
    hdAnimSet_operationCC(player, 5, 5);
    hdDungeonMap_c* selected = hdDungeonSelectMap(this);
    selected->updateMapBounds();
    refreshPartner();
    refreshLinked();
    mDisplayedFloor = mSelectedFloor;
    updatePartner();
    updateWorldMarkers();
    if (refreshItems)
        saveFloorForOpen();
    connectMapMarkers();
    refreshFloorLabels();
    hdDungeonOpenAnimated(this);
    hdDungeonWaitAnimated(this);
    refreshItemButtons();
    refreshNavigation();
}

/* 0265D5CC */
void hdDungeonMap_c::refreshDisplay() {
    refreshDisplayContents(false);
}

/* 0265D7C4 */
void hdDungeonMap_c::refreshDisplayAndItems() {
    refreshDisplayContents(true);
}

/* 0265C790 */
void hdDungeonMap_c::drawMapLayers(void* drawInfo, void* renderer) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdDungeonDrawInfo_l* info = hdPartsView<hdDungeonDrawInfo_l>(gabi::ea(drawInfo));
    void* context = info->mpContext;
    gabi::FrameLocal<HdPadRect> previousClip(8);
    hdDungeonSaveClip(previousClip, context);
    hdDungeonApplyClip(&mBounds, context);
    HdPadMap* map = mpMap;
    if (map)
        gabi::call_ptr<void>(map->mVTable[0x6C / 4].get(), map, renderer);
    auto drawGroup = [renderer](auto& slots, be<s32>& countField) {
        constexpr u32 capacity = sizeof(slots) / sizeof(slots[0]);
        u32 count = u32(countField);
        for (u32 index = 0; s32(index) < s32(count); ++index) {
            u32 slot = index < capacity ? index : 0;
            if (slots[slot]) {
                hdParts_c* child = slots[slot];
                gabi::call_ptr<void>(child->mVtable->mDraw.get(), child, renderer);
                count = u32(countField);
            }
        }
    };
    drawGroup(mpDoors, mDoorCount);
    drawGroup(mpTreasures, mTreasureCount);
    hdParts_c* player = mpPlayerMarker;
    if (player)
        gabi::call_ptr<void>(player->mVtable->mDraw.get(), player, renderer);
    hdParts_c* partner = mpPartnerMarker;
    if (partner)
        gabi::call_ptr<void>(partner->mVtable->mDraw.get(), partner, renderer);
    drawGroup(mpEnemies, mEnemyCount);
    drawGroup(mpSpecialItems, mSpecialItemCount);
    if (isLinkedPlayer()) {
        drawSecondary(renderer);
        drawTertiary(renderer);
        drawItems(renderer);
        drawPrimary(renderer);
    } else {
        drawPrimary(renderer);
        drawSecondary(renderer);
        drawTertiary(renderer);
        drawItems(renderer);
    }
    hdDungeonApplyClip(previousClip, context);
}

static bool dungeonChildNameEquals(hdScreenString* left, hdScreenString* right) {
    left->assureTermination();
    left->assureTermination();
    const char* previousLeft = left->mText;
    right->assureTermination();
    if (previousLeft == right->mText.get())
        return true;
    u32 leftText = gabi::ea(left->mText.get());
    u32 rightText = gabi::ea(right->mText.get());
    for (u32 index = 0; index < 0x40001; ++index) {
        u8 character = *hdPartsView<be<u8>>(leftText + index);
        if (character != u8(*hdPartsView<be<u8>>(rightText + index)))
            return false;
        if (!character)
            return true;
    }
    return false;
}

/* 0265CBB0 */
u32 hdDungeonMap_c::createMapChild(hdPartsDesc_c* descriptor) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdDungeonChildDescriptor_l* desc = hdPartsView<hdDungeonChildDescriptor_l>(gabi::ea(descriptor));
    const char* classText = desc->mClassName;
    hdScreenHeap* heap = sDungeonUiSystem_l->mHeap;
    gabi::FrameLocal<hdScreenString> className(8);
    className->mVtable = &sDungeonSafeStringTable_l;
    className->mText = classText;
    hdParts_c* parts = this;
    for (u32 index = 0; index < 8; ++index) {
        if (!dungeonChildNameEquals(&desc->mPaneName, &sDungeonChildNames_l.mNames[index]))
            continue;
        if (dungeonChildNameEquals(&desc->mResourceName, &sDungeonItemButtonClass_l)) {
            gptr<hdParts_c>* slot = parts->childSlot(index);
            hdParts_c* child = static_cast<hdParts_c*>(hdParts_new(0x78, heap, 4));
            if (child)
                child = hdDungeonConstructItemButton(child);
            *slot = child;
            if (!parts->child(index))
                return 0;
            return hdDungeonInitializeItemButton(parts->child(index), 0, 0, descriptor);
        }
        if (dungeonChildNameEquals(&desc->mResourceName, &sDungeonFloorButtonClass_l)) {
            gptr<hdParts_c>* slot = parts->childSlot(index);
            hdParts_c* child = static_cast<hdParts_c*>(hdParts_new(0x7C, heap, 4));
            if (child)
                child = hdDungeonConstructFloorButton(child);
            *slot = child;
            if (!parts->child(index))
                return 0;
            return hdDungeonInitializeFloorButton(parts->child(index), 0, 0, descriptor);
        }
        if (!dungeonChildNameEquals(className, &sDungeonNoDecideButtonClass_l))
            return parts->createChild(index, descriptor);
        gptr<hdParts_c>* slot = parts->childSlot(index);
        hdParts_c* child = static_cast<hdParts_c*>(hdParts_new(0x78, heap, 4));
        if (child)
            child = hdPartsNoDecideButton_construct(child);
        *slot = child;
        if (!parts->child(index))
            return 0;
        return hdPartsNoDecideButton_init(parts->child(index), &desc->mResourceName, 0, 0, descriptor);
    }
    return 1;
}

/* 0265B850 */
void hdDungeonMap_c::initializeMapResources() {
    hdMapHeapManager_l* manager = sDungeonResourceContexts_l;
    u32 count = manager->mCount;
    gptr<hdMapHeapContext_l>* contexts = manager->mpContexts;
    hdMapHeapContext_l* context = contexts[count > 7 ? 7 : 0];
    u32 flags = context->mFlags;
    void* primary;
    if (flags & 0x1000)
        primary = context->mpSpecial;
    else {
        primary = context->mpPrimary;
        if (!primary)
            primary = &sDungeonDefaultPrimaryContext_l;
    }
    void* secondary;
    if ((flags & 0x40) && (flags & 0x80))
        secondary = &context->mEmbedded;
    else {
        secondary = context->mpSecondary;
        if (!secondary)
            secondary = &sDungeonDefaultSecondaryContext_l;
    }
    hdDungeonCreateFirstMap(gamepadView(), &mpMaps[0], primary, secondary);
    hdDungeonCreateSecondMap(gamepadView(), &mpMaps[1], primary, secondary);
    hdDungeonCreateThirdMap(gamepadView(), &mpMaps[2], primary, secondary);
    hdDungeonCreateFourthMap(gamepadView(), &mpMaps[3], primary, secondary);
    hdDungeonCreateFifthMap(gamepadView(), &mpMaps[4], primary, secondary);
    hdDungeonCreateSixthMap(gamepadView(), &mpMaps[5], primary, secondary);
    hdDungeonCreatePlayerMarker(gamepadView(), &mpPlayerMarker, primary, secondary);
    hdDungeonCreateLinkedMarker(gamepadView(), &mpLinkedMarker, primary, secondary);
    if (mpLinkedMarker)
        hdPartsView<hdDungeonMarkerRole_l>(gabi::ea(mpLinkedMarker.get()))->mRole = DUNGEON_MARKER_LINKED;
    hdDungeonCreateBossMarker(gamepadView(), &mpBossMarker, primary, secondary);
    if (mpBossMarker)
        hdPartsView<hdDungeonMarkerRole_l>(gabi::ea(mpBossMarker.get()))->mRole = DUNGEON_MARKER_BOSS;
    hdDungeonCreateSpecialMarker(gamepadView(), &mpSpecialMarker, primary, secondary);
    if (mpSpecialMarker)
        hdPartsView<hdDungeonMarkerRole_l>(gabi::ea(mpSpecialMarker.get()))->mRole = DUNGEON_MARKER_SPECIAL;
    hdDungeonCreatePartnerMarker(gamepadView(), &mpPartnerMarker, primary, secondary);
    if (mpPartnerMarker)
        hdPartsView<hdDungeonMarkerRole_l>(gabi::ea(mpPartnerMarker.get()))->mRole = DUNGEON_MARKER_PARTNER;
    for (u32 index = 0; index < 3; ++index) {
        hdDungeonCreateItemMarker(gamepadView(), &mpItemMarkers[index], primary, secondary);
        if (mpItemMarkers[index])
            hdPartsView<hdDungeonMarkerRole_l>(gabi::ea(mpItemMarkers[index].get()))->mRole = DUNGEON_MARKER_ITEM;
    }
    for (u32 index = 0; index < 64; ++index)
        hdDungeonCreateDoorMarker(gamepadView(), &mpDoors[index], primary, secondary);
    for (u32 index = 0; index < 32; ++index)
        hdDungeonCreateTreasureMarker(gamepadView(), &mpTreasures[index], primary, secondary);
    for (u32 index = 0; index < 16; ++index)
        hdDungeonCreateEnemyMarker(gamepadView(), &mpEnemies[index], primary, secondary);
    for (u32 index = 0; index < 16; ++index)
        hdDungeonCreateSpecialItemMarker(gamepadView(), &mpSpecialItems[index], primary, secondary);
}

static void dungeonConstructEmbeddedArray(void* array, u32 size) {
    if (!gabi::ea(array))
        hdDungeonNew(size);
}
static void dungeonConstructPoint(HdPadPoint* point, f32 zero) {
    if (!gabi::ea(point)) {
        point = static_cast<HdPadPoint*>(hdDungeonNew(sizeof(HdPadPoint)));
        if (!point)
            return;
    }
    point->y = zero;
    point->x = zero;
}
static void dungeonConstructPoint3(HdPadPoint3* point, f32 zero) {
    if (!gabi::ea(point)) {
        point = static_cast<HdPadPoint3*>(hdDungeonNew(sizeof(HdPadPoint3)));
        if (!point)
            return;
    }
    point->y = zero;
    point->x = zero;
    point->z = zero;
}

/* 0265B2D8 */
hdDungeonMap_c* hdDungeonMap_c::construct(hdDungeonMap_c* self) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!self) {
        self = static_cast<hdDungeonMap_c*>(hdDungeonNew(0x928));
        if (!self)
            return NULL;
    }
    hdParts_c::construct(self);
    self->mVtable = &sDungeonPartsTable_l;
    self->mSecondaryVtable = &sDungeonSecondaryTable_l;
    self->mStateHolder.mHolderVtable = &sDungeonStateTable_l;
    dungeonConstructEmbeddedArray(self->mpFramePanes, 0x10);
    dungeonConstructEmbeddedArray(&self->mpMapAreaPane, 4);
    dungeonConstructEmbeddedArray(&self->mpCaption, 4);
    dungeonConstructEmbeddedArray(self->mpFloorPanes, 0x20);
    dungeonConstructEmbeddedArray(self->mpFloorLabels, 0x14);
    dungeonConstructEmbeddedArray(&self->mpInputMode, 4);
    self->mpSpecialMarker = NULL;
    self->mpMaps[1] = NULL;
    self->mpMaps[2] = NULL;
    self->mpMaps[5] = NULL;
    self->mpMaps[3] = NULL;
    self->mpLinkedMarker = NULL;
    self->mpBossMarker = NULL;
    self->mpPartnerMarker = NULL;
    self->mpMaps[0] = NULL;
    self->mpMaps[4] = NULL;
    self->mpPlayerMarker = NULL;
    dungeonConstructEmbeddedArray(self->mpItemMarkers, 0xC);
    dungeonConstructEmbeddedArray(self->mpDoors, 0x100);
    dungeonConstructEmbeddedArray(self->mpTreasures, 0x80);
    dungeonConstructEmbeddedArray(self->mpEnemies, 0x40);
    dungeonConstructEmbeddedArray(self->mpSpecialItems, 0x40);
    self->mSpecialItemCount = 0;
    f32 zero = l_hdDungeonZero;
    self->mDoorCount = 0;
    self->mMapRotation = zero;
    self->mMapOrigin.x = zero;
    f32 one = sDungeonAnimationRate;
    self->mInputId = 0;
    self->mInputChanged = 0;
    self->mEnemyCount = 0;
    self->mPreviousInputId = 0;
    self->mMapSize = one;
    self->mTreasureCount = 0;
    self->mMapScale = one;
    self->mpFloorReceiver = NULL;
    self->mpFloorTouchReceiver = NULL;
    self->mMapOrigin.y = zero;
    dungeonConstructPoint(reinterpret_cast<HdPadPoint*>(&self->mLinkedPosition), zero);
    self->mLinkedPosition.z = zero;
    self->mSelectedFloor = 10;
    self->mRoom = 0xFF;
    dungeonConstructPoint3(&self->mPlayerPosition, zero);
    dungeonConstructPoint(reinterpret_cast<HdPadPoint*>(&self->mBossPosition), zero);  /* x/y: the player map position */
    self->mBossPosition.z = zero;     /* the player marker angle */
    self->mBossFloor = 10;
    self->mBossRoom = 0xFF;
    dungeonConstructPoint3(&self->mSpecialPosition, zero);
    dungeonConstructPoint(&self->mSpecialMapPosition, zero);
    *reinterpret_cast<be<f32>*>(&self->mSpecialAngle) = zero;
    self->mSpecialFloor = 10;
    self->mSpecialRoom = 0xFF;
    dungeonConstructEmbeddedArray(self->mItemPositions, 0x24);
    dungeonConstructEmbeddedArray(self->mItemMapPositions, 0x18);
    dungeonConstructEmbeddedArray(self->mItemAngles, 0xC);
    dungeonConstructEmbeddedArray(self->mItemFloors, 3);
    dungeonConstructEmbeddedArray(self->mItemRooms, 3);
    dungeonConstructEmbeddedArray(self->mItemFound, 3);
    self->mAllItemsFound = 0;
    self->mMapWidth = one;
    dungeonConstructPoint(&self->mMapPanePosition, zero);
    hdDungeonConstructRect(&self->mBounds);
    self->mPreviousAngle = zero;
    self->mDisplayedFloor = 10;
    dungeonConstructPoint(&self->mPreviousPlayerPosition, zero);
    self->mPositionDelta = zero;
    self->mZoomMode = 0;
    self->mAngleDelta = zero;
    dungeonConstructPoint(&self->mTouchPosition, zero);
    dungeonConstructPoint(&self->mDragPosition, zero);
    hdDungeonConstructFloorContext(&self->mMapData);
    self->mClosePending = 0;
    self->mMarkersActive = 0;
    self->mOpenPending = 0;
    self->mpMap = NULL;
    for (u32 index = 0; index < 4; ++index)
        self->mpFramePanes[index] = NULL;
    self->mpMapAreaPane = NULL;
    self->mpCaption = NULL;
    for (u32 index = 0; index < 8; ++index)
        self->mpFloorPanes[index] = NULL;
    for (u32 index = 0; index < 5; ++index)
        self->mpFloorLabels[index] = NULL;
    self->mpInputMode = NULL;
    for (u32 index = 0; index < 3; ++index)
        self->mpItemMarkers[index] = NULL;
    for (u32 index = 0; index < 64; ++index)
        self->mpDoors[index] = NULL;
    for (u32 index = 0; index < 32; ++index)
        self->mpTreasures[index] = NULL;
    for (u32 index = 0; index < 16; ++index)
        self->mpEnemies[index] = NULL;
    for (u32 index = 0; index < 16; ++index)
        self->mpSpecialItems[index] = NULL;
    gabi::FrameLocal<HdPadPoint3> pointTemplate(8);
    pointTemplate->x = zero;
    pointTemplate->y = zero;
    pointTemplate->z = zero;
    f32 x = zero, y = zero, z = zero;
    for (u32 index = 0; index < 3; ++index) {
        self->mItemPositions[index].x = x;
        self->mItemPositions[index].z = z;
        self->mItemPositions[index].y = y;
        if (index != 2) {
            y = pointTemplate->y;
            z = pointTemplate->z;
            x = pointTemplate->x;
        }
    }
    gabi::FrameLocal<HdPadPoint> paneTemplate(0x14);
    paneTemplate->x = zero;
    paneTemplate->y = zero;
    x = zero;
    y = zero;
    for (u32 index = 0; index < 3; ++index) {
        self->mItemMapPositions[index].x = x;
        self->mItemMapPositions[index].y = y;
        if (index != 2) {
            x = paneTemplate->x;
            y = paneTemplate->y;
        }
    }
    for (u32 index = 0; index < 3; ++index)
        *reinterpret_cast<be<f32>*>(&self->mItemAngles[index]) = zero;
    for (u32 index = 0; index < 3; ++index)
        self->mItemFloors[index] = 10;
    for (u32 index = 0; index < 3; ++index)
        self->mItemRooms[index] = 0xFF;
    for (u32 index = 0; index < 3; ++index)
        self->mItemFound[index] = 0;
    return self;
}

static void dungeonInitializeString(hdScreenString* string, const char* text) {
    string->mVtable = &sDungeonSafeStringTable_l;
    string->mText.v = gabi::guest_addr(text);
}

static void dungeonInitializeString(hdUiSafeString* string, const char* text) {
    dungeonInitializeString(hdPartsView<hdScreenString>(gabi::ea(string)), text);
}

/* 0265DEF8 */
static void initializeDungeonMapStatics() {
    for (s32 index = 3; index >= 0; --index)
        sDungeonInitializerWords_l.mWords[index] = 0;
    hdDungeonRegisterGlobal(&sDungeonRegistration_l);
    f32 angleMin = sDungeonInitializerConstants_l.mAngleMin;
    f32 angleMax = sDungeonInitializerConstants_l.mAngleMax;
    sDungeonInitializerLimits_l.mAngleMin = angleMin;
    sDungeonInitializerLimits_l.mAngleMax = angleMax;
    hdDungeonInitializeAngle(&sDungeonAngleGuard_l);
    hdDungeonRegisterGlobal(&sDungeonAngleRegistration_l);
    hdDungeonInitializeLimits(&sDungeonLimitsGuard_l);
    hdDungeonRegisterGlobal(&sDungeonLimitsRegistration_l);
    f32 halfExtent = sDungeonInitializerConstants_l.mHalfExtent;
    f32 playerLimit = sDungeonInitializerConstants_l.mPlayerLimit;
    // Coordinate constants remain runtime image views, without copied game data.
    for (u32 index = 0; index < 17; ++index) {
        HdPadPoint* target = &sDungeonInitializedCoordinates_l.mEntries[index + 4].mPosition;
        target->x = f32(sDungeonInitializerConstants_l.mMapCoordinates[index].x);
        target->y = f32(sDungeonInitializerConstants_l.mMapCoordinates[index].y);
    }
    for (u32 index = 0; index < 4; ++index) {
        HdPadPoint* target = &sDungeonInitializedCoordinates_l.mEntries[index].mPosition;
        target->x = f32(sDungeonInitializerConstants_l.mExtraCoordinates[index].x);
        target->y = f32(sDungeonInitializerConstants_l.mExtraCoordinates[index].y);
    }
    sDungeonInitializerLimits_l.mHalfWidth = halfExtent;
    sDungeonInitializerLimits_l.mHalfHeight = halfExtent;
    sDungeonInitializerLimits_l.mPlayerLimitX = playerLimit;
    sDungeonInitializerLimits_l.mPlayerLimitY = playerLimit;
    dungeonInitializeString(&sDungeonSelectedPaneName_l, "B_MapArea_00");
    dungeonInitializeString(&sDungeonCaptionPaneName_l, "T_DungeonName_00");
    dungeonInitializeString(&sDungeonFrameNames_l.mNames[0], "N_Scissor_00");
    dungeonInitializeString(&sDungeonFrameNames_l.mNames[1], "N_DungeonMapFloor_00");
    dungeonInitializeString(&sDungeonFrameNames_l.mNames[2], "N_LinkIcon_00");
    dungeonInitializeString(&sDungeonFrameNames_l.mNames[3], "N_BossIcon_00");
    dungeonInitializeString(&sDungeonFloorLabelNames_l.mNames[0], "T_Floor_00");
    dungeonInitializeString(&sDungeonFloorLabelNames_l.mNames[1], "T_Floor_00");
    dungeonInitializeString(&sDungeonFloorLabelNames_l.mNames[2], "T_Floor_00");
    dungeonInitializeString(&sDungeonFloorLabelNames_l.mNames[3], "T_Floor_00");
    dungeonInitializeString(&sDungeonFloorLabelNames_l.mNames[4], "T_Floor_00");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[0], "L_BtnFloor_00-T_Floor_00_B2");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[1], "L_BtnFloor_00-T_Floor_00_B1");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[2], "L_BtnFloor_00-T_Floor_00_1F");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[3], "L_BtnFloor_00-T_Floor_00_2F");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[4], "L_BtnFloor_00-T_Floor_00_3F");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[5], "L_BtnFloor_00-T_Floor_00_4F");
    dungeonInitializeString(&sDungeonFloorLabelTexts_l.mNames[6], "L_BtnFloor_00-T_Floor_00_outdoor");
    dungeonInitializeString(&sDungeonCaptionNames_l.mNames[0], "T_DungeonName_00_01");
    dungeonInitializeString(&sDungeonCaptionNames_l.mNames[1], "T_DungeonName_00_02");
    dungeonInitializeString(&sDungeonCaptionNames_l.mNames[2], "T_DungeonName_00_03");
    dungeonInitializeString(&sDungeonCaptionNames_l.mNames[3], "T_DungeonName_00_04");
    dungeonInitializeString(&sDungeonCaptionNames_l.mNames[4], "T_DungeonName_00_05");
    dungeonInitializeString(&sDungeonCaptionNames_l.mNames[5], "T_DungeonName_00_06");
    dungeonInitializeString(&sDungeonAnimationGroups_l.mNames[0], "G_LinkIcon_00");
    dungeonInitializeString(&sDungeonAnimationGroups_l.mNames[1], "G_BossIcon_00");
    dungeonInitializeString(&sDungeonAnimationGroups_l.mNames[2], "G_All_00");
    dungeonInitializeString(&sDungeonAnimationGroups_l.mNames[3], "G_ChangeStory_00");
    dungeonInitializeString(&sDungeonAnimationGroups_l.mNames[4], "G_GetItem_00");
    dungeonInitializeString(&sDungeonAnimationGroups_l.mNames[5], "G_NoData_00");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[0], "InAll");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[1], "LoopLinkIcon");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[2], "LoopBossIcon");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[3], "ChangeStory");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[4], "GetItem");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[5], "NoData");
    dungeonInitializeString(&sDungeonAnimationNames_l.mNames[6], "OutAll");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[0], "L_BtnFloor_04");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[1], "L_BtnFloor_03");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[2], "L_BtnFloor_02");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[3], "L_BtnFloor_01");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[4], "L_BtnFloor_00");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[5], "L_BtnDItemIcon_00");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[6], "L_BtnDItemIcon_01");
    dungeonInitializeString(&sDungeonFloorNames_l.mNames[7], "L_BtnDItemIcon_02");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[0], "BtnFloor_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[1], "BtnFloor_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[2], "BtnFloor_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[3], "BtnFloor_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[4], "BtnFloor_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[5], "BtnDungeonItemIcon_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[6], "BtnDungeonItemIcon_00");
    dungeonInitializeString(&sDungeonResourceNames_l.mNames[7], "BtnDungeonItemIcon_00");
    dungeonInitializeString(&sDungeonMapArchiveNames_l.mNames[0], "DungeonMap_00");
    dungeonInitializeString(&sDungeonMapArchiveNames_l.mNames[1], "BtnFloor_00");
    dungeonInitializeString(&sDungeonMapArchiveNames_l.mNames[2], "BtnDungeonItemIcon_00");
    dungeonInitializeString(&sDungeonMapLayoutName_l, "DungeonMap_00");
    u32 counter = 0;
    if (sDungeonStateCounterGuard_l)
        counter = sDungeonStateCounter_l;
    else
        sDungeonStateCounterGuard_l = 1;
    sDungeonStates_l.mStates[0].setup(s32(++counter), "::StateID_Display",
        &sDungeonStateDescriptorTable_l, 7, hdPartsView<hdStateId>(gabi::ea(&sDungeonDisplayNullState_l)));
    sDungeonStates_l.mStates[1].setup(s32(++counter), "::StateID_Blank",
        &sDungeonStateDescriptorTable_l, 4, hdPartsView<hdStateId>(gabi::ea(&sDungeonBlankNullState_l)));
    sDungeonStates_l.mStates[2].setup(s32(++counter), "::StateID_InAll",
        &sDungeonStateDescriptorTable_l, 25, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[3].setup(s32(++counter), "::StateID_LoopLinkIcon",
        &sDungeonStateDescriptorTable_l, 28, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[4].setup(s32(++counter), "::StateID_LoopBossIcon",
        &sDungeonStateDescriptorTable_l, 31, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[5].setup(s32(++counter), "::StateID_ChangeStory",
        &sDungeonStateDescriptorTable_l, 34, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[6].setup(s32(++counter), "::StateID_GetItem",
        &sDungeonStateDescriptorTable_l, 37, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[7].setup(s32(++counter), "::StateID_ChangeToNoData",
        &sDungeonStateDescriptorTable_l, 40, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[8].setup(s32(++counter), "::StateID_ChangeToDisplay",
        &sDungeonStateDescriptorTable_l, 43, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[9].setup(s32(++counter), "::StateID_NoData",
        &sDungeonStateDescriptorTable_l, 46, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[10].setup(s32(++counter), "::StateID_OutAll",
        &sDungeonStateDescriptorTable_l, 49, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[11].setup(s32(++counter), "::StateID_Drag",
        &sDungeonStateDescriptorTable_l, 52, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[12].setup(s32(++counter), "::StateID_ToZoomIn",
        &sDungeonStateDescriptorTable_l, 55, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStates_l.mStates[13].setup(s32(++counter), "::StateID_ToZoomOut",
        &sDungeonStateDescriptorTable_l, 58, hdPartsView<hdStateId>(gabi::ea(&sDungeonOtherNullState_l)));
    sDungeonStateCounter_l = counter;
}

/* WWHD glue (tools/verify/autoglue.py)
data sDungeonInitializerWords_l 10492250
data sDungeonRegistration_l 101F5BC0
data sDungeonAngleRegistration_l 101F5BCC
data sDungeonLimitsRegistration_l 101F5BD8
data sDungeonInitializerConstants_l 100F0344
data sDungeonInitializerLimits_l 10492234
data sDungeonInitializedCoordinates_l 101F5A70
data sDungeonAngleGuard_l 1049224C
data sDungeonLimitsGuard_l 1049224D
data sDungeonCaptionNames_l 104922E0
data sDungeonResourceNames_l 104923B8
data sDungeonStates_l 10492410
data sDungeonStateDescriptorTable_l 100F01D4
data sDungeonStateCounterGuard_l 101FD8E4
data sDungeonStateCounter_l 101FDD50
data sDungeonDisplayNullState_l 1049DD90
data sDungeonBlankNullState_l 1049DD60
data sDungeonOtherNullState_l 101FF32C
str 100F03FC
str 100F0404
str 100F040C
str 100F0414
str 100F041C
str 100F042C
str 100F043C
str 100F0458
str 100F0474
str 100F0490
str 100F04AC
str 100F04C8
str 100F04E4
str 100F04F8
str 100F050C
str 100F0520
str 100F0534
str 100F0548
str 100F055C
str 100F0568
str 100F0574
str 100F0580
str 100F0594
str 100F05A4
str 100F05B4
str 100F05C4
str 100F05D4
str 100F05E4
str 100F05F4
str 100F0604
str 100F0614
str 100F0624
str 100F0638
str 100F064C
str 100F0660
str 100F0678
str 100F0688
str 100F069C
str 100F06B4
str 100F06C8
str 100F06E4
str 100F06F4
str 100F070C
str 100F071C
str 100F0730
str 100F073C
str 100F0760
str 100F076C
str 100F0780
str 100F0790
str 100F07A0
str 100F07B0
str 100F07C8
str 100F07E0
str 100F07FC
str 100F0810
str 100F0824
str 100F0834
ext 028F026C void hdDungeonRegisterGlobal(void*)
ext 028ED6F8 void hdDungeonInitializeAngle(void*)
ext 028EAB2C void hdDungeonInitializeLimits(void*)

data sDungeonPartsTable_l 100F0868
data sDungeonStateTable_l 100F0848
data sDungeonSecondaryTable_l 100F0858
ext 0273AD10 void* hdDungeonNew(u32)
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
ext 0274F358 void hdDungeonConstructRect(HdPadRect*)
ext 0268BF64 void hdDungeonConstructFloorContext(void*)
data sDungeonResourceContexts_l 101F95D0
data sDungeonDefaultPrimaryContext_l 104A20FC
data sDungeonDefaultSecondaryContext_l 104A2098
ext 0265ED4C void hdDungeonCreateFirstMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*)
ext 0265EDC4 void hdDungeonCreateSecondMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*)
ext 0265EE3C void hdDungeonCreateThirdMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*)
ext 0265EEB4 void hdDungeonCreateFourthMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*)
ext 0265EF2C void hdDungeonCreateFifthMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*)
ext 0265EFA4 void hdDungeonCreateSixthMap(HdPadDungeon*,gptr<HdPadMap>*,void*,void*)
ext 0265F01C void hdDungeonCreatePlayerMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F0A0 void hdDungeonCreateLinkedMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F124 void hdDungeonCreateBossMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F1A8 void hdDungeonCreateSpecialMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F22C void hdDungeonCreatePartnerMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F2B0 void hdDungeonCreateItemMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F334 void hdDungeonCreateDoorMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F3B8 void hdDungeonCreateTreasureMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F43C void hdDungeonCreateEnemyMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)
ext 0265F4C0 void hdDungeonCreateSpecialItemMarker(HdPadDungeon*,gptr<hdDungeonMarker_l>*,void*,void*)

data sDungeonChildNames_l 10492378
data sDungeonItemButtonClass_l 1048F9F4
data sDungeonFloorButtonClass_l 1048FAC0
data sDungeonNoDecideButtonClass_l 1049E6EC
data sDungeonSafeStringTable_l 100F00CC
ext 0262E7C0 hdParts_c* hdDungeonConstructItemButton(hdParts_c*)
ext 0262F0C4 hdParts_c* hdDungeonConstructFloorButton(hdParts_c*)
ext 0262E82C u32 hdDungeonInitializeItemButton(hdParts_c*,u32,u32,hdPartsDesc_c*)
ext 0262F140 u32 hdDungeonInitializeFloorButton(hdParts_c*,u32,u32,hdPartsDesc_c*)
ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
ext 0274F5F0 void hdDungeonSaveClip(HdPadRect*,void*)
ext 0274FA70 void hdDungeonApplyClip(HdPadRect*,void*)
ext 02655B9C u32 hdDungeonMap_c::isLinkedPlayer()
data sDungeonAnimationNames_l 10492340
data sDungeonAnimationGroups_l 10492310
data sDungeonAnimationGroupOrder_l 100F0170
ext 02004E04 void hdDungeonBindAnimation(hdAnimSet*,u32,hdScreenString*,hdScreenString*)
ext 02657B48 void hdDungeonMap_c::updateCaption()
ext 02657EAC hdDungeonMap_c* hdDungeonSelectMap(hdDungeonMap_c*)
ext 02657F14 void hdDungeonMap_c::updateMapBounds()
ext 02657FDC void hdDungeonMap_c::refreshPartner()
ext 02658090 void hdDungeonMap_c::updatePartner()
ext 02658FCC void hdDungeonMap_c::updateWorldMarkers()
ext 02659020 void hdDungeonMap_c::saveFloorForOpen()
ext 026567EC hdDungeonMap_c* hdDungeonApplyZoomMode(hdDungeonMap_c*)
ext 0265739C void hdDungeonMap_c::updateMarkers()
ext 026855D0 void hdDungeonMarkerSetMap(hdParts_c*,HdPadMap*)
data sDungeonItemButtonOrder_l 100F0164
ext 0262E930 void hdDungeonPrepareItemButton(hdParts_c*,u32)
ext 0262E984 void hdDungeonActivateItemButton(hdParts_c*)
ext 0262E9D0 void hdDungeonDeactivateItemButton(hdParts_c*)
data sDungeonPositionSave_l 101F84DC
ext 02656970 void* hdDungeonMap_c::playerActor()
ext 025200D4 hdDungeonPositionPlay_l* hdDungeonPositionPlay()
ext 0268A920 fopAc_ac_c* hdDungeonFindItemActor(u32)
data sDungeonMapArchiveNames_l 104923F8
data sDungeonMapLayoutName_l 104926B0
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
data sDungeonFloorLabelFrame_l 100F0324
data sDungeonFloorLabelTexts_l 104922A8
ext 02657A8C void hdDungeonMap_c::setPaneLabel(hdTextPane_c*, hdUiSafeString*)
ext 02659038 void hdDungeonMap_c::setPartScales()
ext 02006478 hdPartsStateId_c* hdDungeonCurrentMapState(HdPadStateController*)
data sMapZoomOutState_l 10496030
ext 02655CE8 u32 hdDungeonMap_c::isMarkerOnFloor()
ext 0265614C void hdDungeonMap_c::updatePlayerMapPosition()
ext 02655EA4 void hdDungeonMap_c::updateMapRotation()
ext 02704D2C void hdDungeonPaneDragOrigin(HdPadPoint3*,HdPadPane*)
ext 0267D5F8 void hdDungeonMapSetDelta(HdPadMap*,HdPadPoint*)
ext 0267D60C void hdDungeonMapSetAngle(HdPadMap*,f32)
ext 02655D48 f32 hdDungeonMap_c::playerAngle()
ext 02655DF0 void hdDungeonMap_c::playerMapPosition(HdPadPoint*)
ext 026860D0 void hdDungeonSelectUiMode(HdPadUi*)
ext 02657FE0 void hdDungeonMap_c::refreshLinked()
data sMapEntranceState_l 10496000
ext 026F91F8 void hdParts_c::detachArchives()
ext 0265F544 void hdDungeonReleaseFirstMap(hdDungeonMap_c*,HdPadMap*)
ext 0265F59C void hdDungeonReleaseSecondMap(hdDungeonMap_c*,HdPadMap*)
ext 0265F5F4 void hdDungeonReleaseThirdMap(hdDungeonMap_c*,HdPadMap*)
ext 0265F64C void hdDungeonReleaseFourthMap(hdDungeonMap_c*,HdPadMap*)
ext 0265F6A4 void hdDungeonReleaseFifthMap(hdDungeonMap_c*,HdPadMap*)
ext 0265F6FC void hdDungeonReleaseSixthMap(hdDungeonMap_c*,HdPadMap*)
ext 0265F754 void hdDungeonReleasePlayerMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F7AC void hdDungeonReleaseLinkedMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F804 void hdDungeonReleaseBossMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F85C void hdDungeonReleaseSpecialMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F8B4 void hdDungeonReleasePartnerMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F90C void hdDungeonReleaseItemMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F964 void hdDungeonReleaseDoorMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265F9BC void hdDungeonReleaseTreasureMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265FA14 void hdDungeonReleaseEnemyMarker(hdDungeonMap_c*,hdParts_c*)
ext 0265FA6C void hdDungeonReleaseSpecialItemMarker(hdDungeonMap_c*,hdParts_c*)
data sDungeonAnimationRate 100F0220
data sDungeonReverseAnimationRate 100F0330
data sDungeonMapReferenceWidth 100F0340
data sMapZoomInState_l 10496060
data sMapCenteringState_l 10496090
data sDungeonCursorManager 101F5A44
data sDungeonZoomOutState_l 10492680
data sDungeonAngleScale_l 100F02E0
data sDungeonHalfTurn_l 100F02E4
data sDungeonFloorOrder_l 100F0150
data sDungeonDisplayState_l 10492410
data sDungeonBlankState_l 10492440
data sDungeonInAllState_l 10492470
data sDungeonChangeToDisplayState_l 10492590
data sDungeonNoDataState_l 104925C0
data sDungeonOutAllState_l 104925F0
data sDungeonFrameNames_l 10492260
data sDungeonFloorNames_l 10492378
data sDungeonFloorLabelNames_l 10492280
data sDungeonSelectedPaneName_l 10492224
data sDungeonCaptionPaneName_l 1049222C
data sDungeonUiSystem_l 1018C404
data sDungeonCaptionCapacity_l 100F00C0
data sDungeonFloorCapacities_l 100F013C
data sDungeonEmptyCaption_l 100F0338
data sDungeonEmptyFloorLabel_l 100F033C
ext 02656478 void hdDungeonMap_c::updateSelectedFloorPane()
ext 026564D4 void hdDungeonMap_c::updatePlayerFloorPane()
ext 0267CF98 void hdMapBeginZoomOut(HdPadMap*)
ext 0267CFA8 void hdMapBeginZoomIn(HdPadMap*)
ext 0267CFB8 void hdMapBeginZoomToPlayer(HdPadMap*)
ext 02657C10 void hdDungeonMap_c::clearParts()
ext 0267D2E0 u32 hdDungeonMapCanZoom(HdPadMap*)
ext 026F5C00 void hdDungeonButtonSetReceiver(hdButtonPart_c*,u32,hdButtonReceiver_l*)
ext 02001BE4 void hdDungeonReceiverSetButton(hdButtonReceiver_l*,u32,hdButtonReceiver_l*)
ext 026577FC u32 hdDungeonMap_c::isBossIndex(u32)
ext 02656394 u32 hdDungeonMap_c::floorChildIndex(u32)
ext 0268714C u32 hdDungeonSecondaryFloorVisible(HdPadUi*)
ext 02656750 void hdDungeonMap_c::updateFloorReceiver()
ext 026851C4 void hdDungeonMarkerBeginIn(hdParts_c*)
ext 026851D4 void hdDungeonMarkerBeginOut(hdParts_c*)
ext 026851E4 void hdDungeonMarkerResetDisplay(hdParts_c*)
ext 02657C00 void hdDungeonMap_c::finishMap()
ext 02005708 void hdDungeonStopAnimation(hdAnimSet*,u32,u32)
ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
ext 0273B050 void* hdDungeonAllocateReceiver(u32,hdScreenHeap*,u32)
ext 02616ED4 hdButtonReceiver_l* hdDungeonConstructReceiver(void*,hdPartsPane_c*,u32)
*/
