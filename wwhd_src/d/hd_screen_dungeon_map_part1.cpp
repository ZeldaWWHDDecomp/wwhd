/* HD dungeon-map selection and marker updates. Private decomp-derived source. */
#include "d/hd_screen_dungeon_map.h"
#include "d/hd_screen_dungeon_map_up.h"
#include "d/hd_screen_dungeon_map_treasure.h"
#include "d/hd_screen_dungeon_map_statue.h"
/* the dungeon map pane is the merged sea-chart map (MAP-sea_chart#1) */
static inline hdSeaChartMap_c* seaMap(HdPadMap* map) { return static_cast<hdSeaChartMap_c*>(map); }
using namespace gabi;
struct hdDungeonDoorList_l;
struct hdDungeonEventQueue_l;
struct hdDungeonFoundItem_l;
struct hdDungeonMapData_l;
struct hdDungeonMapIcon_l;
struct hdDungeonMarker_l;
struct hdDungeonPlay_l;
struct hdDungeonRoomControl_l;
struct hdDungeonRoomData_l;
struct hdDungeonStage_l;
struct hdDungeonTreasureList_l;
struct hdDungeonWindow_l;



hdDungeonFoundItem_l* dungeonDataItem(hdDungeonMapData_l*,u32,s32);
void dungeonDataDestroy(hdDungeonMapData_l*,s32);
void dungeonDelete(hdDungeonMap_c*);
void dungeonMarkerScales(hdDungeonMarker_l*,f32,f32);
u32 dungeonIsZoomEvent(HdPadEvent*);
void dungeonUpdateParts(hdDungeonMap_c*);
void dungeonUpdateMarkers(hdDungeonMap_c*);
hdDungeonDoorList_l* dungeonDoorList();
void dungeonDoorName(hdUiSafeString*);
void dungeonPaneMatrixPosition(HdPadPoint3*,HdPadPane*);
void dungeonSpecialInit(hdDungeonMapIcon_l*);
void dungeonSpecialUpdate(hdDungeonMapIcon_l*);
u32 dungeonUiRotated(HdPadUi*);
u32 dungeonIsCursorEvent(u32);
u32 dungeonIsDirectionEvent(u32);
void dungeonInputSetDefault(HdPadEventSource*,HdPadInputMode*);
HdPadInputMode* dungeonInputMode(HdPadEventSource*);
HdPadInputMode* dungeonDirectionReceiver(HdPadInputMode*,u32);
void dungeonCursorFeedback();
void dungeonUiSound(const char*);
void dungeonEventConstruct(HdPadEvent*,u32,u32,u32);
void dungeonQueueEvent(hdDungeonEventQueue_l*,HdPadEvent*);
void dungeonEventDestroy(HdPadEvent*);
void dungeonEventCopy(HdPadEvent*,HdPadEvent*);
void dungeonWindowChange(hdDungeonWindow_l*,hdPartsStateId_c*);
u32 dungeonPlayerSpecialVisible(void*);
u32 dungeonPlayerSecondaryVisible(void*);
void dungeonMarkerClear(hdDungeonMarker_l*);
hdDungeonPlay_l* dungeonPlay();
void dungeonDataUpdate(hdDungeonMapData_l*);
void dungeonDataOpen(hdDungeonMapData_l*);
void dungeonDataCalc(hdDungeonMapData_l*);
hdPartsStateId_c* dungeonCurrentState(HdPadStateController*);
void dungeonChangeMapState(HdPadStateController*,hdPartsStateId_c*);
hdDungeonTreasureList_l* dungeonTreasureList();
u32 dungeonTreasureFound(HdPadUi*,u32);
u32 dungeonRoomVisited(HdPadUi*,s32);
u32 dungeonEventBit(void*,u32);
u32 dungeonUiPlayerHidden(HdPadUi*);
void dungeonMarkerMap(hdDungeonMarker_l*,HdPadMap*);
u32 dungeonInputId(HdPadEventSource*);
void dungeonInputSetReceiver(HdPadEventSource*,HdPadInputMode*);
u32 dungeonStageFloor(hdDungeonStage_l*,f32);
hdDungeonRoomData_l* dungeonRoomData(hdDungeonRoomControl_l*,s32);
void* dungeonItemActor(u32);
u32 dungeonItemActorVisible(void*);
u32 dungeonSpecialActorVisible(void*);
void dungeonShowItem(hdParts_c*);
u32 dungeonUiBusy(HdPadUi*);
u32 dungeonUiPartnerChanged(HdPadUi*);
void dungeonReceiverPane(HdPadInputMode*,u32,HdPadPane*);
void dungeonStringDestroy(hdUiSafeString*,s32);
void dungeonDestroyStrings(void*,u32,u32,u32,u32,u32);
struct hdDungeonFloorChildren_l { be<u32> mIndex[5]; };
static hdDungeonFloorChildren_l sFloorChildren;
struct hdDungeonItemChildren_l { be<u32> mIndex[3]; };
static hdDungeonItemChildren_l sItemChildren;
static void* sPlayerActor;
static f32 sAngleScale;
struct hdDungeonItemRoomRecord_l { be<u32> mX; be<u32> mZ; be<s8> mRoom; u8 mReserved[3]; };
struct hdDungeonItemRooms_l { hdDungeonItemRoomRecord_l mRecords[6]; };
struct hdDungeonFoundItem_l { u8 mReserved[2]; be<u8> mFound; };
static hdDungeonItemRooms_l sItemRooms;
static f32 l_hdDungeonHalfTurn;
static hdPartsStateId_c sMapTrackingState;
static hdPartsStateId_c sMapBusyState;
static f32 sTwo;
static hdUiStringTable sCaptionNameVtable;
static hdUiStringTable sCaptionTextVtable;
struct hdDungeonCaptions_l { hdUiSafeString mLabels[8]; };
static hdDungeonCaptions_l sCaptions;
struct hdDungeonFixedTreasure_l { be<u32> mX; be<u32> mY; be<u8> mFloor; u8 mReserved[3]; be<u32> mKind; };
struct hdDungeonFixedTreasures_l { hdDungeonFixedTreasure_l mRecords[4]; };
static hdDungeonFixedTreasures_l sFixedTreasures;
struct hdDungeonDoorRecord_l {
    be<u8> mName[8]; be<u32> mFlags; HdPadPoint3 mPosition;
    be<u16> mRooms; be<s16> mHeading; be<u16> mKind; u8 mReserved[6];
};
struct hdDungeonDoorList_l { be<s32> mCount; hdDungeonDoorRecord_l mRecords[1]; };
struct hdDungeonFixedDoor_l { be<u32> mX; be<u32> mY; be<u32> mAngle; be<u8> mFloor; be<s8> mRoomA; be<s8> mRoomB; u8 mReserved; };
struct hdDungeonFixedDoors_l { hdDungeonFixedDoor_l mRecords[17]; };
static hdDungeonFixedDoors_l sFixedDoors;
static f32 sFullTurn;
static f32 sOne;
static void dungeonUpdatePart(hdParts_c* part) {
    gfn* table = reinterpret_cast<gfn*>(part->mVtable.get());
    call_ptr<void>(table[0x5C / 4].get(), part);
}
static f32 sMapAngleOffset;
struct hdDungeonVtable_l { u8 mSlots[8]; };
static hdDungeonVtable_l sHolderVtable;
static hdDungeonVtable_l sSecondaryVtable;
struct hdDungeonUiView_l : HdPadUi {
    be<u32> mInputMode;
    be<u8> mReservedFlag;
    be<u8> mHidden;
};
WWHD_OFFSET(hdDungeonUiView_l, mHidden, 0x59);
static hdDungeonUiView_l* dungeonUiView() {
    return static_cast<hdDungeonUiView_l*>(l_hdDungeonUi.mOwner->mUi.get());
}
struct hdDungeonSaveView_l { u8 mReserved[0x1DB]; be<u8> mPlayerMode; u8 mReservedEvents[0x644-0x1DC]; u8 mEvents[1]; };
struct hdDungeonSavePointer_l { gptr<hdDungeonSaveView_l> mpSave; };
static hdDungeonSavePointer_l sSave;
struct hdDungeonTreasureRecord_l { u8 mReserved[8]; be<u32> mFlags; HdPadPoint3 mPosition; u8 mReservedTail[8]; };
struct hdDungeonTreasureList_l { be<u32> mCount; hdDungeonTreasureRecord_l mRecords[1]; };
static s8 sCurrentRoom;
static u8 sPlayerSpecialVisible;
static u8 sPlayerSecondaryVisible;
static u16 sPlayerFlags;
struct hdDungeonWindow_l {};
struct hdDungeonWindowPointer_l { gptr<hdDungeonWindow_l> mpWindow; };
static hdDungeonWindowPointer_l sWindow;
static hdPartsStateId_c sWindowActiveState;
struct hdDungeonEventQueue_l { u8 mReserved[0x1C]; };
struct hdDungeonEventSource_l { u8 mReserved[0x14]; hdDungeonEventQueue_l mQueue; HdPadEvent mEvent; };
struct hdDungeonButtonMap_l { be<u32> mChild[8]; be<u32> mEventKind[8]; };
static hdDungeonButtonMap_l sButtonMap;
static HdPadEventSource* dungeonInput() { return l_hdDungeonInput.mInput; }
static void dungeonMarkerPosition(hdDungeonMarker_l* marker, HdPadPoint* in, HdPadPoint* out) {
    gfn* table = reinterpret_cast<gfn*>(marker->mVtable.get());
    call_ptr<void>(table[0x124 / 4].get(), marker, in, out);
}
static void dungeonBossPartUpdate(hdParts_c* part) {
    gfn* table = reinterpret_cast<gfn*>(part->mVtable.get());
    call_ptr<void>(table[0xCC / 4].get(), part);
}
struct hdDungeonSpecialActor_l { u8 mReserved[0x8F4]; be<u32> mFlags; };
WWHD_OFFSET(hdDungeonSpecialActor_l, mFlags, 0x8F4);

struct hdDungeonMapBounds_l { be<u32> mX; be<u32> mY; be<u32> mSize; be<u32> mRotation; };
struct hdDungeonMapBoundsPointer_l { u8 mReserved[4]; gptr<hdDungeonMapBounds_l> mpBounds; };
struct hdDungeonStageInfo_l { u8 mReserved[0xC]; be<u32> mFlags; };
struct hdDungeonStage_l {
    gptr<gfn> mpVtable;
    u8 mReserved[0x78];
    hdDungeonStageInfo_l* info() { return call_ptr<hdDungeonStageInfo_l*>(mpVtable[0x15C / 4].get(), this); }
};
static hdDungeonMapBoundsPointer_l* dungeonStageBounds(hdDungeonStage_l* stage) {
    return call_ptr<hdDungeonMapBoundsPointer_l*>(stage->mpVtable[0x23C / 4].get(), stage);
}
struct hdDungeonRoomActorView_l { u8 mReserved[0x236]; be<s16> mHeading; };
struct hdDungeonRoomActor_l { gptr<hdDungeonRoomActorView_l> mpActor; u8 mReserved[0x30]; };
struct hdDungeonRoomControl_l { u8 mReserved[0x5AF8-0x51CC]; hdDungeonRoomActor_l mActors[1]; };
struct hdDungeonRoomData_l { gptr<gfn> mpVtable; };
struct hdDungeonRoomBounds_l {
    be<u32> mLeft; be<u32> mReserved; be<f32> mRight; be<u32> mReservedRight;
    be<u32> mOriginX; be<u32> mOriginY;
};
struct hdDungeonPlay_l {
    u8 mReserved[0x5150];
    hdDungeonStage_l mStage;
    hdDungeonRoomControl_l mRooms;
    gptr<void> mpLinkedActor;
    be<s8> mPlayerRoom;
    u8 mReservedActor[3];
    gptr<void> mpPlayerActor;
    u8 mReservedPartner[4];
    gptr<void> mpPartnerActor;
    u8 mReservedFlags[0x5CD8-0x5B40]; be<u32> mFlags;
};
WWHD_OFFSET(hdDungeonPlay_l, mpLinkedActor, 0x5B2C);
WWHD_OFFSET(hdDungeonPlay_l, mpPlayerActor, 0x5B34);
struct hdDungeonActor_l {
    u8 mReserved[0xB4];
    gptr<gfn> mpVtable;
    u8 mReservedFlags[0x2E0-0xB8]; be<u32> mFlags;
    u8 mReservedPosition[0x314-0x2E4];
    be<u32> mPosition[3];
    u8 mReservedRoom[6];
    be<s8> mRoom;
    u8 mReservedHeading[3];
    be<s16> mHeading;
    u8 mReservedPlayerFlags[0x420C-0x32C]; be<u32> mPlayerFlags;
};
WWHD_OFFSET(hdDungeonActor_l, mpVtable, 0xB4);
WWHD_OFFSET(hdDungeonActor_l, mPosition, 0x314);
WWHD_OFFSET(hdDungeonActor_l, mHeading, 0x32A);
struct hdDungeonStateKey_l { be<u32> mKey; };
/* 0265463C */
static u32 dungeonStateKey(hdDungeonStateKey_l* state) { return state->mKey; }
/* 02654EA8 */
u32 hdDungeonMap_c::hasLinkedActor() {
    return dungeonPlay()->mpLinkedActor != nullptr;
}
/* 02654ED4 */
u32 hdDungeonMap_c::linkedActorVisible() {
    void* actor = dungeonPlay()->mpLinkedActor;
    if (!actor) return 1;
    hdDungeonActor_l* view = static_cast<hdDungeonActor_l*>(actor);
    return call_ptr<u32>(view->mpVtable[0x54 / 4].get(), actor) != 0;
}
/* 02655120 */
u32 hdDungeonMap_c::hasPlayer() { return sPlayerActor != nullptr; }
/* 026552E0 */
void hdDungeonMap_c::getPlayerPosition(HdPadPoint3* out) {
    auto* bits = reinterpret_cast<be<u32>*>(out);
    auto* source = reinterpret_cast<be<u32>*>(&mPlayerPosition);
    bits[0] = u32(source[0]);
    bits[1] = u32(source[1]);
    bits[2] = u32(source[2]);
}
/* 02655B84 */
void hdDungeonMap_c::saveFloor() {
    u8 floor = mSelectedFloor;
    s8 room = mRoom;
    mPreviousFloor = floor;
    mPreviousRoom = room;
    dungeonDataUpdate(&mMapData);
}
/* 02655B9C */
u32 hdDungeonMap_c::isLinkedPlayer() {
    void* linked = dungeonPlay()->mpLinkedActor;
    return linked == dungeonPlay()->mpPlayerActor;
}
/* 02655C88 */
hdDungeonFloorInfo_l* hdDungeonMap_c::selectedMarkerInfo() {
    if (isLinkedPlayer()) {
        hdDungeonMarker_l* marker = mpLinkedMarker;
        if (marker) return &marker->mInfo;
    }
    return findMarkerInfo();
}
/* 02655CE8 */
u32 hdDungeonMap_c::isMarkerOnFloor() {
    hdDungeonFloorInfo_l* info = selectedMarkerInfo();
    return info && info->mFloor == mDisplayedFloor;
}
/* 02656108 */
hdDungeonMarker_l* hdDungeonMap_c::selectedMarker() {
    if (isLinkedPlayer()) return mpLinkedMarker;
    return findMarker();
}
/* 02656378 */
u32 hdDungeonMap_c::isFloorIndex(u32 index) { return index < 5; }
/* 02656394 */
u32 hdDungeonMap_c::floorChildIndex(u32 floor) {
    enum { SPECIAL_FLOOR = 0x85 };
    if (floor == SPECIAL_FLOOR) return 1;
    u32 index = u8(floor - mFirstFloor);
    if (!isFloorIndex(index)) index = 0;
    return sFloorChildren.mIndex[index];
}
/* 026563FC */
hdPartsPaneSRT_c* hdDungeonMap_c::floorPane(u32 floor) {
    enum { SPECIAL_FLOOR = 0x85, NO_FLOOR = 10 };
    if (floor == SPECIAL_FLOOR) return mpFloorPanes[1];
    if (floor == NO_FLOOR) return nullptr;
    u32 index = u8(floorChildIndex(floor));
    return mpFloorPanes[index < 8 ? index : 0];
}
/* 02656478 */
void hdDungeonMap_c::updateSelectedFloorPane() {
    hdPartsPaneSRT_c* floor = floorPane(mSelectedFloor);
    hdPartsPaneSRT_c* pane = mpSelectedFloorPane;
    if (floor && pane) {
        f32 x = dungeonPaneTranslationY(floor);
        u8 flags = pane->mFlags;
        dungeonPaneSetTranslationZ(pane, l_hdDungeonZero);
        dungeonPaneSetTranslationY(pane, x);
        pane->mFlags = flags | 0x10;
    }
}
/* 02656598 */
void hdDungeonMap_c::updateItemParts() {
    for (u32 index = 0; index < 3; ++index) {
        if (mMapData.mItemVisible[index]) {
            hdParts_c* part = child(sItemChildren.mIndex[index]);
            dungeonShowItem(part);
        }
    }
}
/* 02656630 */
void hdDungeonMap_c::setFloorReceiver(HdPadInputMode* receiver) {
    if (dungeonUiBusy(l_hdDungeonUi.mOwner->mUi) || !receiver) return;
    mpFloorReceiver = receiver;
    u32 index = floorChildIndex(mSelectedFloor);
    hdParts_c* part = child(index);
    dungeonReceiverPane(receiver, 3, static_cast<hdDungeonChildPane_l*>(part)->mpPane.get());
}
/* 026566C0 */
void hdDungeonMap_c::setFloorTouchReceiver(HdPadInputMode* receiver) {
    if (dungeonUiBusy(l_hdDungeonUi.mOwner->mUi) || !receiver) return;
    mpFloorTouchReceiver = receiver;
    u32 index = floorChildIndex(mSelectedFloor);
    hdParts_c* part = child(index);
    dungeonReceiverPane(receiver, 3, static_cast<hdDungeonChildPane_l*>(part)->mpPane.get());
}
/* 02656750 */
void hdDungeonMap_c::updateFloorReceiver() {
    u32 index = floorChildIndex(mSelectedFloor);
    hdParts_c* part = child(index);
    dungeonReceiverPane(mpInputMode, 6, static_cast<hdDungeonChildPane_l*>(part)->mpPane.get());
}
/* 026567A8 */
void hdDungeonMap_c::updateFloorReceivers() {
    setFloorReceiver(mpFloorReceiver);
    setFloorTouchReceiver(mpFloorTouchReceiver);
    updateFloorReceiver();
}
/* 026567EC */
void hdDungeonMap_c::updateDisplayedFloor() {
    HdPadMap* map = mpMap;
    if (!map) return;
    u8 floor;
    if (!mFloorChanged && !mRoomChanged) {
        floor = mDisplayedFloor;
    } else {
        floor = mSelectedFloor;
        map = mpMap;
        mDisplayedFloor = floor;
    }
    map->mZoomMode = floor;
}
/* 026568CC */
void hdDungeonMap_c::updateMap() {
    HdPadMap* map = mpMap;
    if (map) call_ptr<void>(map->mVTable[0x5C / 4].get(), map);
}
/* 02657360 */
void hdDungeonMap_c::updatePlayerVisible() {
    hdDungeonMarker_l* marker = mpPlayerMarker;
    marker->mInfo.mVisible = marker->mInfo.mFloor == mDisplayedFloor && mMapData.mPlayerVisible != 0;
}
/* 026573E8 */
void hdDungeonMap_c::updateMapData() { dungeonDataCalc(&mMapData); }
/* 026573F0 */
void hdDungeonMap_c::updateCursor() {
    if (!dungeonUiBusy(l_hdDungeonUi.mOwner->mUi)) {
        updateMarkers();
        updateMapData();
    }
}
/* 026577FC */
u32 hdDungeonMap_c::isBossIndex(u32 index) { return index < 6; }
/* 02657C00 */
void hdDungeonMap_c::finishMap() {
    HdPadMap* map = mpMap;
    if (map) seaMap(map)->cancelOpening();
}
/* 02657FDC */
void hdDungeonMap_c::refreshPartner() { updateLinkedFloor(); }
/* 02657FE0 */
void hdDungeonMap_c::refreshLinked() { updatePlayerFloor(); }
/* 02658090 */
void hdDungeonMap_c::updatePartner() {
    if (dungeonUiPartnerChanged(l_hdDungeonUi.mOwner->mUi)) updatePartnerPosition();
}
/* 026580D8 */
void hdDungeonMap_c::refreshPlayer() { updatePlayerPosition(); }
/* 026580DC */
void hdDungeonMap_c::refreshBoss() { updateBossPosition(); }
/* 02659020 */
void hdDungeonMap_c::saveFloorForOpen() {
    u8 floor = mSelectedFloor;
    s8 room = mRoom;
    mPreviousFloor = floor;
    mPreviousRoom = room;
    dungeonDataOpen(&mMapData);
}

/* 02654F2C */
void hdDungeonMap_c::getLinkedPosition(HdPadPoint3* out) {
    hdDungeonActor_l* actor = static_cast<hdDungeonActor_l*>(dungeonPlay()->mpPlayerActor.get());
    be<u32>* bits = reinterpret_cast<be<u32>*>(out);
    if (actor) {
        bits[0] = u32(actor->mPosition[0]);
        bits[1] = u32(actor->mPosition[1]);
        bits[2] = u32(actor->mPosition[2]);
    } else {
        out->y = l_hdDungeonZero;
        out->x = l_hdDungeonZero;
        out->z = l_hdDungeonZero;
    }
}
/* 02655134 */
u32 hdDungeonMap_c::partnerMarkerActive() {
    if (l_hdDungeonUi.mOwner->mUi->mBossIndex != 3 || !mpPartnerMarker) return 0;
    return dungeonPlay()->mpPartnerActor != nullptr;
}
/* 02655198 */
u32 hdDungeonMap_c::bossMarkerActive() {
    if (l_hdDungeonUi.mOwner->mUi->mBossIndex != 4 || !mpBossMarker) return 0;
    if (sPlayerActor) return 1;
    return mBossFloor != 10 || mBossRoom != -1;
}
/* 0265524C */
u32 hdDungeonMap_c::specialMarkerActive() {
    if (l_hdDungeonUi.mOwner->mUi->mBossIndex != 5 || !mpSpecialMarker) return 0;
    return sPlayerActor != nullptr;
}
/* 026553A0 */
u32 hdDungeonMap_c::itemMarkerActive(u32 index) {
    if (l_hdDungeonUi.mOwner->mUi->mBossIndex != 3 || !mpItemMarkers[index < 3 ? index : 0]) return 0;
    return dungeonItemActor(index) != nullptr;
}
/* 02655418 */
u32 hdDungeonMap_c::itemMarkerVisible(u32 index) {
    void* actor = dungeonItemActor(index);
    return actor ? dungeonItemActorVisible(actor) : 0;
}
/* 026557E4 */
f32 hdDungeonMap_c::linkedAngle() {
    f32 angle = l_hdDungeonZero;
    hdDungeonActor_l* actor = static_cast<hdDungeonActor_l*>(dungeonPlay()->mpPlayerActor.get());
    if (actor) {
        // The actor heading is the signed halfword following its position.
        angle = fmadds(f32(s16(actor->mHeading)), sAngleScale, l_hdDungeonHalfTurn);
    }
    return angle;
}
/* 02655870 */
void hdDungeonMap_c::updatePlayerPosition() {
    u32 exists = hasLinkedActor();
    f32 zero = l_hdDungeonZero;
    if (exists) {
        HdPadPoint3 position;
        position.x = zero;
        position.y = zero;
        position.z = zero;
        getLinkedPosition(&position);
        f32 x = fsubs_ppc(position.x, mMapOrigin.x);
        f32 y = fsubs_ppc(position.z, mMapOrigin.y);
        mLinkedPosition.x = x;
        mLinkedPosition.y = y;
        mLinkedPosition.z = linkedAngle();
    } else {
        mLinkedPosition.z = zero;
        mLinkedPosition.y = zero;
        mLinkedPosition.x = zero;
    }
}
/* 02655BDC */
hdDungeonFloorInfo_l* hdDungeonMap_c::findMarkerInfo() {
    if (bossMarkerActive()) return &mpBossMarker->mInfo;
    if (specialMarkerActive()) return &mpSpecialMarker->mInfo;
    if (partnerMarkerActive()) return &mpPartnerMarker->mInfo;
    return nullptr;
}
/* 02655DF0 */
void hdDungeonMap_c::playerMapPosition(HdPadPoint* out) {
    if (!out) return;
    HdPadPoint3 position;
    position.x = l_hdDungeonZero;
    position.z = l_hdDungeonZero;
    position.y = l_hdDungeonZero;
    if (isLinkedPlayer()) getLinkedPosition(&position);
    else getPlayerPosition(&position);
    f32 originX = mMapOrigin.x;
    f32 x = position.x;
    f32 originY = mMapOrigin.y;
    f32 dx = fsubs_ppc(x, originX);
    f32 y = position.z;
    f32 dy = fsubs_ppc(y, originY);
    out->x = dx;
    out->y = dy;
}
/* 02656068 */
hdDungeonMarker_l* hdDungeonMap_c::findMarker() {
    if (bossMarkerActive()) return mpBossMarker;
    if (specialMarkerActive()) return mpSpecialMarker;
    if (partnerMarkerActive()) return mpPartnerMarker;
    return nullptr;
}
/* 02656AC4 */
u32 hdDungeonMap_c::specialMarkerVisible(u32 index) {
    void* actor = dungeonItemActor(index);
    return actor ? dungeonSpecialActorVisible(actor) : 0;
}
/* 02656AFC */
u32 hdDungeonMap_c::specialMarkerFlag(u32 index) {
    hdDungeonSpecialActor_l* actor = static_cast<hdDungeonSpecialActor_l*>(dungeonItemActor(index));
    return actor ? (u32(actor->mFlags) & 1) : 0;
}


/* 02656330 */
void hdDungeonMap_c::updateFloorParts() {
    for (u32 index = 0; index < 5; ++index) {
        hdFloorButton_c* part = reinterpret_cast<hdFloorButton_c*>(child(sFloorChildren.mIndex[index]));
        if (part) part->mSelectedFloor = mDisplayedFloor;
    }
}
/* 026564D4 */
void hdDungeonMap_c::updatePlayerFloorPane() {
    hdPartsPaneSRT_c* pane = mpPlayerFloorPane;
    if (!pane) return;
    hdPartsPaneSRT_c* floor = nullptr;
    if (!dungeonUiPlayerHidden(l_hdDungeonUi.mOwner->mUi)) floor = floorPane(mpPlayerMarker->mInfo.mFloor);
    u8 flags;
    if (floor) {
        flags = pane->mFlags;
        f32 x = dungeonPaneTranslationY(floor);
        dungeonPaneSetTranslationZ(pane, l_hdDungeonZero);
        flags = (flags & 0xEF) | 0x10;
        dungeonPaneSetTranslationY(pane, x);
        pane->mFlags = flags;
        if (mMapData.mPlayerVisible) {
            pane->mFlags = flags | 1;
            return;
        }
    } else flags = pane->mFlags;
    pane->mFlags = flags & 0xFE;
}
/* 026568E8 */
void hdDungeonMap_c::updateLinkedMarker() {
    hdDungeonMarker_l* marker = mpLinkedMarker;
    if (!marker) return;
    hdDungeonFloorInfo_l* info = &marker->mInfo;
    marker->mInfo.mFloor = mSelectedFloor;
    marker = mpLinkedMarker;
    if (marker) dungeonMarkerMap(marker, mpMap);
    auto* position = reinterpret_cast<be<u32>*>(&mLinkedPosition);
    auto* output = reinterpret_cast<be<u32>*>(&info->mPosition);
    output[0] = u32(position[0]);
    output[1] = u32(position[1]);
    u32 angle = position[2];
    info->mVisible = 1;
    *reinterpret_cast<be<u32>*>(&info->mAngle) = angle;
    info->mLinked = isLinkedPlayer();
}
/* 02656970 */
void* hdDungeonMap_c::playerActor() {
    if (hasPlayer()) return sPlayerActor;
    if (partnerMarkerActive()) return dungeonPlay()->mpPartnerActor;
    return nullptr;
}
/* 0265739C */
void hdDungeonMap_c::updateMarkers() {
    updateLinkedMarker();
    updatePlayerMarkers();
    updateTreasures();
    updateDoors();
    updatePlayerVisible();
}
/* 02657818 */
void hdDungeonMap_c::updateSingleBossPart() {
    hdParts_c* part = child(1);
    if (part) dungeonBossPartUpdate(part);
}
/* 02657848 */
void hdDungeonMap_c::updateBossParts() {
    u32 count = mMapData.mBossCount;
    if (!isBossIndex(count)) count = 0;
    for (u32 index = 0; index < count; ++index) {
        hdParts_c* part = child(sFloorChildren.mIndex[index]);
        if (part) dungeonBossPartUpdate(part);
    }
    if (dungeonUiPlayerHidden(l_hdDungeonUi.mOwner->mUi)) updateSingleBossPart();
}
/* 02657914 */
void hdDungeonMap_c::updateInputReceiver() {
    HdPadEventSource* input = dungeonInput();
    if (!input || !mpInputMode) return;
    if (dungeonInputId(input)) {
        HdPadInputMode* mode = mpInputMode;
        if (dungeonInputId(dungeonInput()) != mode->mId) return;
    }
    dungeonInputSetReceiver(dungeonInput(), mpInputMode);
}
/* 02657EAC */
void hdDungeonMap_c::selectMap() {
    HdPadMap* maps[6];
    maps[5] = mpMaps[5];
    maps[1] = mpMaps[1];
    maps[2] = mpMaps[2];
    maps[0] = mpMaps[0];
    maps[3] = mpMaps[3];
    maps[4] = mpMaps[4];
    u32 mode = l_hdDungeonUi.mOwner->mUi->mBossIndex;
    mpMap = mode < 6 ? maps[mode] : nullptr;
}


/* 02654A08 */
void hdDungeonMap_c::checkActiveInput() {
    if (!l_hdDungeonActive.mInput->mActive) return;
    HdPadInputMode* mode = mpInputMode;
    u32 id = mInputId;
    if (id == mode->mId) dungeonWindowChange(sWindow.mpWindow, &sWindowActiveState);
}
/* 026551F8 */
u32 hdDungeonMap_c::playerSpecialVisible() {
    u8 visible = sPlayerSpecialVisible;
    void* actor = sPlayerActor;
    return visible || dungeonPlayerSpecialVisible(actor) != 0;
}
/* 0265528C */
u32 hdDungeonMap_c::playerSecondaryVisible() {
    u8 visible = sPlayerSecondaryVisible;
    void* actor = sPlayerActor;
    return visible || dungeonPlayerSecondaryVisible(actor) != 0;
}


/* 0265614C */
void hdDungeonMap_c::updatePlayerMapPosition() {
    HdPadPoint position;
    position.x = l_hdDungeonZero;
    position.y = l_hdDungeonZero;
    playerMapPosition(&position);
    hdDungeonMarker_l* marker = selectedMarker();
    if (marker) dungeonMarkerPosition(marker, &position, &position);
    HdPadMap* map = mpMap;
    if (map) seaMap(map)->setTargetPosition(&position);
}
/* 02656830 */
void hdDungeonMap_c::updateLayoutData() {
    if (dungeonUiBusy(l_hdDungeonUi.mOwner->mUi)) return;
    updateLinkedFloor();
    updatePlayerFloor();
    updatePlayerPosition();
    updateBossPosition();
    saveFloor();
    updateMapTracking();
    updateFloorParts();
    updateSelectedFloorPane();
    updatePlayerFloorPane();
    updateItemParts();
    updateFloorReceivers();
    updateDisplayedFloor();
}


/* 02657FE4 */
void hdDungeonMap_c::updatePartnerPosition() {
    s32 room = mRoom;
    hdDungeonRoomControl_l* rooms = &dungeonPlay()->mRooms;
    hdDungeonRoomData_l* roomData = dungeonRoomData(rooms, room);
    if (!roomData) return;
    hdDungeonRoomBounds_l* bounds = call_ptr<hdDungeonRoomBounds_l*>(roomData->mpVtable[0x7C / 4].get(), roomData, 0x80);
    if (!bounds) return;
    auto* origin = reinterpret_cast<be<u32>*>(&mMapOrigin);
    origin[0] = u32(bounds->mOriginX);
    HdPadPane* pane = mpMapPane;
    origin[1] = u32(bounds->mOriginY);
    if (pane) {
        f32 right = bounds->mRight;
        HdPadFloatBits left{u32(bounds->mLeft)};
        f32 width = fsubs_ppc(right, left.mValue);
        mMapScale = f32(f64(f32(pane->mWidth)) / f64(width));
    }
    mMapRotation = l_hdDungeonZero;
}


/* 0265473C */
void hdDungeonMap_c::destroy(hdDungeonMap_c* screen, s32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &sHolderVtable;
    screen->mSecondaryVtable = &sSecondaryVtable;
    dungeonDataDestroy(&screen->mMapData, 2);
    hdParts_c::destroy(screen, 0);
    if (flags & 1) dungeonDelete(screen);
}


/* 02655900 */
u32 hdDungeonMap_c::anyPlayerMarkerActive() {
    if (hasPlayer()) return 1;
    if (partnerMarkerActive()) return 1;
    hdDungeonUiView_l* ui = static_cast<hdDungeonUiView_l*>(l_hdDungeonUi.mOwner->mUi.get());
    if (ui->mHidden) return 1;
    u8 mode = sSave.mpSave->mPlayerMode;
    return mode == 1 || mode == 2;
}


/* 026570EC */
void hdDungeonMap_c::updateTreasures() {
    if (dungeonTreasureList()) {
        hdDungeonTreasureRecord_l* treasure = dungeonTreasureList()->mRecords;
        for (s32 index = 0; index < mTreasureCount; ++index, ++treasure) {
            HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
            u32 flags = treasure->mFlags;
            hdDungeonMarker_l* marker = mpTreasures[u32(index) < 32 ? index : 0];
            u32 found = dungeonTreasureFound(ui, (flags >> 7) & 31);
            reinterpret_cast<hdDungeonMapTreasure_c*>(marker)->setVisible(found != 0);
        }
    }
    for (s32 index = 0; index < mTreasureCount; ++index) {
        hdDungeonMarker_l* marker = mpTreasures[u32(index) < 32 ? index : 0];
        bool visible = marker->mInfo.mFloor == mDisplayedFloor;
        if (visible) visible = mMapData.mPlayerVisible != 0;
        if (visible) visible = reinterpret_cast<hdDungeonMapTreasure_c*>(marker)->isVisible() == 0;
        if (visible) {
            u32 kind = marker->mKind;
            visible = kind != 15 && kind != 16;
        }
        marker->mInfo.mVisible = visible;
    }
}
/* 02657270 */
void hdDungeonMap_c::updateDoors() {
    for (s32 index = 0; index < mDoorCount; ++index) {
        hdDungeonMarker_l* marker = mpDoors[u32(index) < 64 ? index : 0];
        bool visible = marker->mInfo.mFloor == mDisplayedFloor;
        if (visible && !mMapData.mMapOwned) {
            s32 firstRoom = s8(u32(marker->mKind) >> 24);
            visible = dungeonRoomVisited(l_hdDungeonUi.mOwner->mUi, firstRoom) != 0;
            if (!visible) {
                s32 secondRoom = s8(u32(marker->mKind) >> 16);
                visible = dungeonRoomVisited(l_hdDungeonUi.mOwner->mUi, secondRoom) != 0;
            }
        }
        marker->mInfo.mVisible = visible;
    }
}


/* 026569E0 */
void hdDungeonMap_c::updateBossMarker(hdDungeonMarker_l* marker) {
    if (!marker) return;
    hdDungeonFloorInfo_l* info = &marker->mInfo;
    if (!gabi::ea(info)) return;
    hdDungeonStage_l* stage = &dungeonPlay()->mStage;
    u32 floor = dungeonStageFloor(stage, mPlayerPosition.y);
    mBossFloor = floor;
    info->mFloor = floor;
    auto* position = reinterpret_cast<be<u32>*>(&mBossPosition);
    auto* output = reinterpret_cast<be<u32>*>(&info->mPosition);
    output[0] = u32(position[0]);
    output[1] = u32(position[1]);
    *reinterpret_cast<be<u32>*>(&info->mAngle) = u32(position[2]);
    if (mBossFloor == mDisplayedFloor) {
        dungeonMarkerMap(marker, mpMap);
        info->mLinked = 0;
        info->mVisible = 1;
    } else {
        hdPartsPaneSRT_c* pane = info->mpPane;
        info->mActive = 0;
        info->mEnabled = 0;
        if (pane) pane->mAlpha = 0;
    }
}


/* 02654F94 */
f32 hdDungeonMap_c::floorHeightOffset() {
    if (dungeonPlay()->mStage.info()) {
        hdDungeonStageInfo_l* info = dungeonPlay()->mStage.info();
        u32 mode = (u32(info->mFlags) >> 16) & 7;
        if (mode == 3) return mMapRotation;
        info = dungeonPlay()->mStage.info();
        if (((u32(info->mFlags) >> 16) & 7) == 6) return mMapRotation;
    }
    return l_hdDungeonZero;
}
/* 02655040 */
void hdDungeonMap_c::updateLinkedFloor() {
    if (hasLinkedActor()) {
        if (dungeonUiPartnerChanged(l_hdDungeonUi.mOwner->mUi)) {
            mSelectedFloor = 0x85;
            mRoom = sCurrentRoom;
        } else if (!linkedActorVisible()) {
            HdPadPoint3 position;
            position.z = l_hdDungeonZero;
            position.x = l_hdDungeonZero;
            position.y = l_hdDungeonZero;
            getLinkedPosition(&position);
            hdDungeonStage_l* stage = &dungeonPlay()->mStage;
            f32 offset = floorHeightOffset();
            f32 height = fadds_ppc(position.y, offset);
            mSelectedFloor = dungeonStageFloor(stage, height);
            mRoom = sCurrentRoom;
        }
    } else {
        mSelectedFloor = 10;
        mRoom = -1;
    }
}


/* 02655D48 */
f32 hdDungeonMap_c::playerAngle() {
    f32 angle = l_hdDungeonZero;
    s32 room = dungeonPlay()->mPlayerRoom;
    hdDungeonRoomActorView_l* actor = dungeonPlay()->mRooms.mActors[room].mpActor;
    if (actor) angle = fmadds(f32(s16(actor->mHeading)), sAngleScale, l_hdDungeonHalfTurn);
    return angle;
}
/* 02655EA4 */
void hdDungeonMap_c::updateMapRotation() {
    f32 angle = playerAngle();
    if (dungeonUiRotated(l_hdDungeonUi.mOwner->mUi)) angle = fsubs_ppc(angle, sMapAngleOffset);
    f32 halfTurn = l_hdDungeonHalfTurn;
    HdPadMap* map = mpMap;
    if (halfTurn < angle) angle = fsubs_ppc(angle, sFullTurn);
    angle = -angle;
    if (map) seaMap(map)->setZoomY(angle);
}


/* 026552FC */
void hdDungeonMap_c::actorFloorRoom(void* actorPointer, HdPadPoint3* position, be<u8>* floor, be<s8>* room) {
    if (!floor || !room) return;
    hdDungeonActor_l* actor = static_cast<hdDungeonActor_l*>(actorPointer);
    if (actor) {
        hdDungeonStage_l* stage = &dungeonPlay()->mStage;
        f32 offset = floorHeightOffset();
        f32 height = fadds_ppc(position->y, offset);
        *floor = dungeonStageFloor(stage, height);
        *room = actor->mRoom;
    } else {
        hdDungeonUiView_l* ui = static_cast<hdDungeonUiView_l*>(l_hdDungeonUi.mOwner->mUi.get());
        if (!ui->mHidden) {
            *floor = 10;
            *room = -1;
        }
    }
}


/* 026547B4 */
void hdDungeonMap_c::sendButtonEvent(u32 id) {
    for (u32 index = 0; index < 8; ++index) {
        hdButtonPart_c* button = static_cast<hdButtonPart_c*>(child(sButtonMap.mChild[index]));
        if (button && button->getReceiverId() == id) {
            u32 receiver = button->getReceiverId();
            u32 kind = sButtonMap.mEventKind[index];
            HdPadEvent event;
            dungeonEventConstruct(&event, kind, receiver, 0);
            hdDungeonEventSource_l* input = reinterpret_cast<hdDungeonEventSource_l*>(dungeonInput());
            dungeonQueueEvent(&input->mQueue, &event);
            dungeonEventDestroy(&event);
            return;
        }
    }
}
/* 02654A44 */
void hdDungeonMap_c::checkFloorInput() {
    HdPadMap* map = mpMap;
    if (!map || !seaMap(map)->isScrollingOrZoomed()) return;
    HdPadEventSource* input = dungeonInput();
    if (!input) return;
    u8 previous = mDisplayedFloor;
    HdPadEvent event;
    dungeonEventCopy(&event, &input->mEvent);
    u32 kind = event.mKind;
    if (kind >= 0x49 && kind <= 0x4D) {
        u32 special = dungeonUiPlayerHidden(l_hdDungeonUi.mOwner->mUi);
        u32 floor = mDisplayedFloor;
        u32 next;
        if (special && event.mKind == 0x4C) next = 0x85;
        else next = u32(mFirstFloor) + u8(u32(event.mKind) - 0x49);
        if (floor != next) {
            mDisplayedFloor = next;
            mZoomMode = 0;
        }
    }
    if (mDisplayedFloor != previous) dungeonUiSound("SE_MENU_MINI_WINDOW_OPEN");
}


/* 02654884 */
void hdDungeonMap_c::checkCursorInput() {
    HdPadEventSource* input = dungeonInput();
    if (!input) return;
    HdPadEvent event;
    dungeonEventCopy(&event, &input->mEvent);
    u32 kind = event.mKind;
    HdPadInputMode* inputMode = mpInputMode;
    u32 id = event.mId;
    u32 ownId = inputMode->mId;
    if (dungeonIsCursorEvent(kind)) {
        if (id == ownId) {
            if (l_hdDungeonActive.mInput->mActive) dungeonInputSetDefault(dungeonInput(), mpInputMode);
        } else sendButtonEvent(id);
        mInputId = id;
    } else if (dungeonIsDirectionEvent(kind)) {
        HdPadInputMode* mode = dungeonInputMode(dungeonInput());
        if (mode) {
            HdPadInputMode* target = dungeonDirectionReceiver(mode, kind - 5);
            if (target) {
                if (ownId == mode->mId) dungeonInputSetDefault(dungeonInput(), target);
                sendButtonEvent(target->mId);
                mInputId = target->mId;
                dungeonCursorFeedback();
                u32 current = mInputId;
                u32 previous = mPreviousInputId;
                if (current != previous) dungeonUiSound("SE_MENU_CURSOR");
            }
        }
    }
    u32 current = mInputId;
    u32 previous = mPreviousInputId;
    mInputChanged = 0;
    if (current != previous) {
        current = mInputId;
        mInputChanged = 1;
    }
    mPreviousInputId = current;
}


/* inlined into hdDungeonMap_c::updateMapTracking */
static bool dungeonStateMatches(hdPartsStateId_c* current, hdPartsStateId_c* expected) {
    hdPartsStateIdVtbl_c* table = expected->mVtable;
    u32 currentKey = current->getKey();
    u32 expectedKey = call_ptr<u32>(table->mGetKey.get(), expected);
    return currentKey == expectedKey;
}
/* 026561C8 */
void hdDungeonMap_c::updateMapTracking() {
    HdPadMap* map = mpMap;
    if (!map) return;
    hdPartsStateId_c* state = dungeonCurrentState(&map->mState);
    if (dungeonStateMatches(state, reinterpret_cast<hdPartsStateId_c*>(&l_mapWaitState))) {
        if (isMarkerOnFloor()) trackPlayer();
        return;
    }
    map = mpMap;
    state = dungeonCurrentState(&map->mState);
    bool tracking = dungeonStateMatches(state, reinterpret_cast<hdPartsStateId_c*>(&l_mapActiveState));
    if (!tracking) {
        map = mpMap;
        state = dungeonCurrentState(&map->mState);
        tracking = dungeonStateMatches(state, &sMapTrackingState);
    }
    if (tracking) {
        updatePlayerMapPosition();
        updateMapRotation();
    }
}


/* 02655450 */
void hdDungeonMap_c::updateItemFound(u32 item) {
    u32 slot = item < 3 ? item : 0;
    if (mItemFound[slot]) return;
    s32 room = mItemRooms[slot];
    if (room == -1) {
        hdDungeonActor_l* actor = static_cast<hdDungeonActor_l*>(dungeonItemActor(item));
        if (!actor) return;
        HdPadFloatBits x{u32(actor->mPosition[0])};
        u32 positionX = ftoi(x.mValue);
        for (u32 index = 0; index < 6; ++index) {
            hdDungeonItemRoomRecord_l* record = &sItemRooms.mRecords[index];
            if (record->mX == positionX) {
                HdPadFloatBits z{u32(actor->mPosition[2])};
                if (record->mZ == u32(ftoi(z.mValue))) {
                    room = record->mRoom;
                    mItemRooms[slot] = room;
                    break;
                }
            }
        }
    }
    hdDungeonFoundItem_l* found = dungeonDataItem(&mMapData, mItemFloors[slot], room);
    if (found && found->mFound) mItemFound[slot] = 1;
}
/* 026555AC */
void hdDungeonMap_c::updatePlayerFloor() {
    if (l_hdDungeonUi.mOwner->mUi->mBossIndex != 3) {
        if (hasPlayer() || partnerMarkerActive()) {
            if (bossMarkerActive()) {
                if (playerSpecialVisible()) return;
            } else if (specialMarkerActive() && playerSecondaryVisible()) return;
            HdPadPoint3 position;
            position.z = l_hdDungeonZero;
            position.x = l_hdDungeonZero;
            position.y = l_hdDungeonZero;
            getPlayerPosition(&position);
            if (dungeonUiPartnerChanged(l_hdDungeonUi.mOwner->mUi)) {
                mBossFloor = 0x85;
                mBossRoom = sCurrentRoom;
            } else {
                hdDungeonStage_l* stage = &dungeonPlay()->mStage;
                f32 offset = floorHeightOffset();
                f32 height = fadds_ppc(position.y, offset);
                mBossFloor = dungeonStageFloor(stage, height);
                mBossRoom = sCurrentRoom;
            }
        } else {
            hdDungeonUiView_l* ui = static_cast<hdDungeonUiView_l*>(l_hdDungeonUi.mOwner->mUi.get());
            if (!ui->mHidden) {
                mBossFloor = 10;
                mBossRoom = -1;
            }
        }
    } else {
        actorFloorRoom(dungeonPlay()->mpPartnerActor, &mSpecialPosition, &mSpecialFloor, &mSpecialRoom);
        mSpecialFloor = 0x80;
        for (u32 index = 0; index < 3; ++index) {
            if (itemMarkerActive(index) && itemMarkerVisible(index)) continue;
            void* actor = dungeonItemActor(index);
            actorFloorRoom(actor, &mItemPositions[index], &mItemFloors[index], &mItemRooms[index]);
            mItemFloors[index] = 0x81;
            updateItemFound(index);
        }
    }
}


/* 02655984 */
void hdDungeonMap_c::updateBossPosition() {
    s32 mode = l_hdDungeonUi.mOwner->mUi->mBossIndex;
    f32 zero = l_hdDungeonZero;
    if (mode != 3) {
        if (anyPlayerMarkerActive()) {
            HdPadPoint3 position;
            position.x = zero;
            position.y = zero;
            position.z = zero;
            getPlayerPosition(&position);
            f32 originX = mMapOrigin.x;
            f32 x = position.x;
            f32 z = position.z;
            f32 dx = fsubs_ppc(x, originX);
            f32 originY = mMapOrigin.y;
            f32 dy = fsubs_ppc(z, originY);
            mBossPosition.x = dx;
            mBossPosition.y = dy;
        } else {
            mBossPosition.y = zero;
            mBossPosition.x = zero;
            mBossPosition.z = zero;
        }
        return;
    }
    if (partnerMarkerActive()) {
        f32 x = mSpecialPosition.x;
        f32 originX = mMapOrigin.x;
        f32 z = mSpecialPosition.z;
        f32 dx = fsubs_ppc(x, originX);
        f32 originY = mMapOrigin.y;
        f32 dy = fsubs_ppc(z, originY);
        mSpecialMapPosition.x = dx;
        mSpecialMapPosition.y = dy;
    } else {
        mSpecialMapPosition.x = zero;
        mSpecialMapPosition.y = zero;
    }
    for (u32 index = 0; index < 3; ++index) {
        f32 originY = mMapOrigin.y;
        f32 originX = mMapOrigin.x;
        f32 x = mItemPositions[index].x;
        f32 dx = fsubs_ppc(x, originX);
        f32 z = mItemPositions[index].z;
        f32 dy = fsubs_ppc(z, originY);
        mItemMapPositions[index].x = dx;
        mItemMapPositions[index].y = dy;
    }
}
/* 02655F44 */
void hdDungeonMap_c::trackPlayer() {
    f32 angle = playerAngle();
    f32 oldAngle = mPreviousAngle;
    f32 zero = l_hdDungeonZero;
    f32 difference = fsubs_ppc(oldAngle, angle);
    HdPadPoint position;
    position.y = zero;
    position.x = zero;
    mAngleDelta = fabsf(difference);
    playerMapPosition(&position);
    f32 oldX = mPreviousPlayerPosition.x;
    f32 x = position.x;
    f32 y = position.y;
    f32 dx = fsubs_ppc(oldX, x);
    f32 oldY = mPreviousPlayerPosition.y;
    f32 dy = fsubs_ppc(oldY, y);
    f32 squared = fmadds(dx, dx, fmuls_ppc(dy, dy));
    f32 factor = zero;
    if (squared > zero) {
        f64 inverse = frsqrte(squared);
        f32 square = f32(inverse * round25(inverse));
        f32 halfInverse = f32(inverse * f64(l_hdDungeonHalf));
        f32 correction = fnmsubs(square, squared, l_hdDungeonThree);
        factor = fmuls_ppc(correction, halfInverse);
    }
    f32 angleDelta = mAngleDelta;
    f32 length = fmuls_ppc(factor, squared);
    f32 threshold = sOne;
    mPositionDelta = length;
    if (threshold < angleDelta || threshold < length) {
        updateMapRotation();
        HdPadMap* map = mpMap;
        if (map) dungeonChangeMapState(&map->mState, &sMapTrackingState);
    }
}
/* 02657440 */
void hdDungeonMap_c::updateAllParts() {
    if (mpPlayerMarker) dungeonUpdatePart(mpPlayerMarker);
    if (mpLinkedMarker) dungeonUpdatePart(mpLinkedMarker);
    if (mpBossMarker) dungeonUpdatePart(mpBossMarker);
    if (mpSpecialMarker) dungeonUpdatePart(mpSpecialMarker);
    if (mpPartnerMarker) dungeonUpdatePart(mpPartnerMarker);
    for (u32 index = 0; index < 3; ++index) {
        if (mpItemMarkers[index]) dungeonUpdatePart(mpItemMarkers[index]);
    }
    for (s32 index = 0; index < mDoorCount; ++index) {
        u32 slot = u32(index) < 64 ? index : 0;
        if (mpDoors[slot]) dungeonUpdatePart(mpDoors[slot]);
    }
    for (s32 index = 0; index < mTreasureCount; ++index) {
        u32 slot = u32(index) < 32 ? index : 0;
        if (mpTreasures[slot]) dungeonUpdatePart(mpTreasures[slot]);
    }
    for (s32 index = 0; index < mEnemyCount; ++index) {
        u32 slot = u32(index) < 16 ? index : 0;
        if (mpEnemies[slot]) dungeonUpdatePart(mpEnemies[slot]);
    }
    for (s32 index = 0; index < mSpecialItemCount; ++index) {
        u32 slot = u32(index) < 16 ? index : 0;
        if (mpSpecialItems[slot]) dungeonUpdatePart(mpSpecialItems[slot]);
    }
}


/* 026579F8 */
void hdDungeonMap_c::updateActive() {
    if (dungeonUiBusy(l_hdDungeonUi.mOwner->mUi)) {
        changeState(&sMapBusyState);
        return;
    }
    checkCursorInput();
    checkActiveInput();
    checkFloorInput();
    checkZoomInput();
    dungeonUpdateParts(this);
    dungeonUpdateMarkers(this);
}
/* 02658FCC */
void hdDungeonMap_c::updateWorldMarkers() {
    refreshPlayer();
    refreshBoss();
    initTreasuresFromStage();
    initDoorsFromStage();
    initPlayerDoor();
    initSpecialItems();
}
/* 02659038 */
void hdDungeonMap_c::setPartScales() {
    f32 one = sOne;
    f32 two = sTwo;
    HdPadMap* map = mpMap;
    if (map) seaMap(map)->setInterpolationBounds(one, two);
    if (mpPlayerMarker) dungeonMarkerScales(mpPlayerMarker, one, one);
    if (mpLinkedMarker) dungeonMarkerScales(mpLinkedMarker, one, one);
    if (mpBossMarker) dungeonMarkerScales(mpBossMarker, one, one);
    if (mpSpecialMarker) dungeonMarkerScales(mpSpecialMarker, one, one);
    if (mpPartnerMarker) dungeonMarkerScales(mpPartnerMarker, one, one);
    for (u32 index = 0; index < 3; ++index) {
        if (mpItemMarkers[index]) dungeonMarkerScales(mpItemMarkers[index], one, one);
    }
    for (s32 index = 0; index < mDoorCount; ++index) {
        u32 slot = u32(index) < 64 ? index : 0;
        if (mpDoors[slot]) dungeonMarkerScales(mpDoors[slot], one, two);
    }
    for (s32 index = 0; index < mTreasureCount; ++index) {
        u32 slot = u32(index) < 32 ? index : 0;
        if (mpTreasures[slot]) dungeonMarkerScales(mpTreasures[slot], one, one);
    }
    for (s32 index = 0; index < mEnemyCount; ++index) {
        u32 slot = u32(index) < 16 ? index : 0;
        if (mpEnemies[slot]) dungeonMarkerScales(mpEnemies[slot], one, one);
    }
    for (s32 index = 0; index < mSpecialItemCount; ++index) {
        u32 slot = u32(index) < 16 ? index : 0;
        if (mpSpecialItems[slot]) dungeonMarkerScales(mpSpecialItems[slot], one, one);
    }
}


/* 02657A8C */
void hdDungeonMap_c::setPaneLabel(hdTextPane_c* pane, hdUiSafeString* label) {
    if (!pane) return;
    hdUiSafeString name;
    name.mVtable = &sCaptionNameVtable;
    name.mText = static_cast<be<u8>*>(mpLayout->lyt()->mpMessageData.get());
    pane->resetMessage(&name, label);
}
/* 02657AD8 */
void hdDungeonMap_c::setPaneText(hdTextPane_c* pane, hdUiSafeString* text) {
    if (!pane) return;
    gfn* table = pane->mpVTable;
    text->assureTermination();
    call_ptr<void>(table[0xC4 / 4].get(), pane, text->mText.get(), 0);
}
/* 02657B48 */
void hdDungeonMap_c::updateCaption() {
    hdDungeonUiView_l* ui = static_cast<hdDungeonUiView_l*>(l_hdDungeonUi.mOwner->mUi.get());
    u32 inputMode = ui->mInputMode;
    u32 mode = ui->mBossIndex;
    if (inputMode == 8) {
        setPaneLabel(mpCaption, &sCaptions.mLabels[3]);
    } else if (mode < 8) {
        if (mode == 6 || mode == 7) {
            hdUiSafeString text;
            text.mVtable = &sCaptionTextVtable;
            text.mText.v = guest_addr("");
            setPaneText(mpCaption, &text);
        } else setPaneLabel(mpCaption, &sCaptions.mLabels[mode]);
    }
}


/* 02657C10 */
void hdDungeonMap_c::clearParts() {
    if (mpPlayerMarker) dungeonMarkerClear(mpPlayerMarker);
    if (mpLinkedMarker) dungeonMarkerClear(mpLinkedMarker);
    if (mpBossMarker) dungeonMarkerClear(mpBossMarker);
    if (mpSpecialMarker) dungeonMarkerClear(mpSpecialMarker);
    if (mpPartnerMarker) dungeonMarkerClear(mpPartnerMarker);
    for (u32 index = 0; index < 3; ++index) {
        if (mpItemMarkers[index]) dungeonMarkerClear(mpItemMarkers[index]);
    }
    for (s32 index = 0; index < mDoorCount; ++index) {
        u32 slot = u32(index) < 64 ? index : 0;
        if (mpDoors[slot]) dungeonMarkerClear(mpDoors[slot]);
    }
    for (s32 index = 0; index < mTreasureCount; ++index) {
        u32 slot = u32(index) < 32 ? index : 0;
        if (mpTreasures[slot]) dungeonMarkerClear(mpTreasures[slot]);
    }
    for (s32 index = 0; index < mEnemyCount; ++index) {
        u32 slot = u32(index) < 16 ? index : 0;
        if (mpEnemies[slot]) dungeonMarkerClear(mpEnemies[slot]);
    }
    for (s32 index = 0; index < mSpecialItemCount; ++index) {
        u32 slot = u32(index) < 16 ? index : 0;
        if (mpSpecialItems[slot]) dungeonMarkerClear(mpSpecialItems[slot]);
    }
}
/* 02657F14 */
void hdDungeonMap_c::updateMapBounds() {
    if (dungeonUiPartnerChanged(l_hdDungeonUi.mOwner->mUi)) return;
    hdDungeonStage_l* stage = &dungeonPlay()->mStage;
    hdDungeonMapBoundsPointer_l* bounds = dungeonStageBounds(stage);
    if (bounds) {
        stage = &dungeonPlay()->mStage;
        hdDungeonMapBounds_l* map = dungeonStageBounds(stage)->mpBounds;
        *reinterpret_cast<be<u32>*>(&mMapOrigin.x) = u32(map->mX);
        u32 y = map->mY;
        f32 width = mMapWidth;
        *reinterpret_cast<be<u32>*>(&mMapOrigin.y) = y;
        u32 size = map->mSize;
        *reinterpret_cast<be<u32>*>(&mMapSize) = size;
        HdPadFloatBits sizeFloat{size};
        f32 scale = f32(f64(width) / f64(sizeFloat.mValue));
        u32 rotation = map->mRotation;
        *reinterpret_cast<be<u32>*>(&mMapRotation) = rotation;
        mMapScale = scale;
    } else {
        f32 zero = l_hdDungeonZero;
        f32 one = sOne;
        mMapOrigin.x = zero;
        mMapScale = one;
        mMapRotation = zero;
        mMapSize = one;
        mMapOrigin.y = zero;
    }
}


/* 026580E0 */
void hdDungeonMap_c::initFixedTreasures() {
    mTreasureCount = 4;
    for (s32 index = 0; index < mTreasureCount; ++index) {
        hdDungeonFixedTreasure_l* record = &sFixedTreasures.mRecords[index];
        u32 x = record->mX;
        u32 y = record->mY;
        HdPadUi* ui = l_hdDungeonUi.mOwner->mUi;
        u32 kind = record->mKind;
        u8 floor = record->mFloor;
        u32 found = dungeonTreasureFound(ui, kind);
        hdDungeonMarker_l* marker = mpTreasures[u32(index) < 32 ? index : 0];
        *reinterpret_cast<be<u32>*>(&marker->mInfo.mPosition.x) = x;
        *reinterpret_cast<be<u32>*>(&marker->mInfo.mPosition.y) = y;
        marker->mInfo.mFloor = floor;
        marker->mKind = kind;
        reinterpret_cast<hdDungeonMapTreasure_c*>(marker)->setVisible(found != 0);
    }
}


/* 026581C8 */
void hdDungeonMap_c::initTreasuresFromStage() {
    if (dungeonUiPlayerHidden(l_hdDungeonUi.mOwner->mUi)) {
        initFixedTreasures();
        return;
    }
    if (!dungeonTreasureList()) return;
    hdDungeonTreasureList_l* list = dungeonTreasureList();
    s32 count = list->mCount;
    hdDungeonTreasureRecord_l* record = list->mRecords;
    mTreasureCount = count;
    for (s32 index = 0; index < mTreasureCount; ++index, ++record) {
        f32 x = record->mPosition.x;
        f32 z = record->mPosition.z;
        f32 originX = mMapOrigin.x;
        f32 originY = mMapOrigin.y;
        f32 dx = fsubs_ppc(x, originX);
        u32 kind = (u32(record->mFlags) >> 7) & 31;
        f32 dy = fsubs_ppc(z, originY);
        hdDungeonStage_l* stage = &dungeonPlay()->mStage;
        u32 floor = dungeonStageFloor(stage, record->mPosition.y);
        u32 found = dungeonTreasureFound(l_hdDungeonUi.mOwner->mUi, kind);
        hdDungeonMarker_l* marker = mpTreasures[u32(index) < 32 ? index : 0];
        marker->mInfo.mPosition.x = dx;
        marker->mInfo.mPosition.y = dy;
        marker->mInfo.mFloor = floor;
        marker->mKind = kind;
        reinterpret_cast<hdDungeonMapTreasure_c*>(marker)->setVisible(found != 0);
    }
}
/* 0265833C */
void hdDungeonMap_c::initFixedDoors() {
    mDoorCount = 17;
    for (s32 index = 0; index < mDoorCount; ++index) {
        hdDungeonFixedDoor_l* record = &sFixedDoors.mRecords[index];
        u32 x = record->mX;
        s8 firstRoom = record->mRoomA;
        u32 angle = record->mAngle;
        s8 secondRoom = record->mRoomB;
        u32 y = record->mY;
        u8 floor = record->mFloor;
        hdDungeonMarker_l* marker = mpDoors[u32(index) < 64 ? index : 0];
        *reinterpret_cast<be<u32>*>(&marker->mInfo.mPosition.x) = x;
        *reinterpret_cast<be<u32>*>(&marker->mInfo.mPosition.y) = y;
        *reinterpret_cast<be<u32>*>(&marker->mInfo.mAngle) = angle;
        marker->mInfo.mFloor = floor;
        auto* rooms = reinterpret_cast<be<s8>*>(&marker->mKind);
        rooms[0] = firstRoom;
        rooms[1] = secondRoom;
    }
}
/* 026583E8 */
void hdDungeonMap_c::initDoorsFromStage() {
    if (dungeonUiPlayerHidden(l_hdDungeonUi.mOwner->mUi)) {
        initFixedDoors();
        return;
    }
    if (!dungeonDoorList()) return;
    hdDungeonDoorList_l* list = dungeonDoorList();
    s32 count = list->mCount;
    hdDungeonDoorRecord_l* record = list->mRecords;
    mDoorCount = count;
    if (count <= 0) return;
    f32 angleScale = sAngleScale;
    for (s32 index = 0; index < mDoorCount; ++index, ++record) {
        f32 originY = mMapOrigin.y;
        s32 heading = record->mHeading;
        f32 z = record->mPosition.z;
        f32 x = record->mPosition.x;
        f32 dy = fsubs_ppc(z, originY);
        f32 originX = mMapOrigin.x;
        f32 dx = fsubs_ppc(x, originX);
        f32 angle = fmuls_ppc(f32(heading), angleScale);
        hdDungeonStage_l* stage = &dungeonPlay()->mStage;
        u32 floor = dungeonStageFloor(stage, record->mPosition.y);
        u32 rooms = record->mRooms;
        s8 firstRoom = rooms & 63;
        s8 secondRoom = (rooms >> 6) & 63;
        hdDungeonMarker_l* marker = mpDoors[u32(index) < 64 ? index : 0];
        marker->mInfo.mPosition.x = dx;
        marker->mInfo.mPosition.y = dy;
        marker->mInfo.mFloor = floor;
        marker->mInfo.mAngle = angle;
        auto* output = reinterpret_cast<be<s8>*>(&marker->mKind);
        output[0] = firstRoom;
        output[1] = secondRoom;
    }
}


/* inlined into hdDungeonMap_c::isPlayerDoor */
static bool dungeonCaptionEquals(hdUiSafeString* name, hdUiSafeString* expected) {
    name->assureTermination();
    be<u8>* snapshot = name->mText;
    expected->assureTermination();
    be<u8>* text = expected->mText;
    if (snapshot == text) return true;
    be<u8>* actual = name->mText;
    for (u32 index = 0; index < 0x40001; ++index) {
        u8 a = actual[index];
        u8 b = text[index];
        if (a != b) return false;
        if (!a) return true;
    }
    return false;
}
/* 026585AC */
u32 hdDungeonMap_c::isPlayerDoor(hdDungeonDoorRecord_l* door) {
    hdUiSafeString name;
    name.mText = door->mName;
    name.mVtable = &sCaptionNameVtable;
    hdUiSafeString expected;
    expected.mVtable = &sCaptionNameVtable;
    expected.mText.v = guest_addr("door20");
    dungeonDoorName(&name);
    if (dungeonCaptionEquals(&name, &expected)) return 1;
    hdUiSafeString second;
    second.mVtable = &sCaptionNameVtable;
    second.mText.v = guest_addr("door12");
    name.assureTermination();
    if (!dungeonCaptionEquals(&name, &second)) return 0;
    u32 kind = (u16(door->mKind) >> 8) & 0xFF;
    return kind == 9 || kind == 12;
}
/* 02658790 */
void hdDungeonMap_c::initPlayerDoor() {
    if (dungeonUiPlayerHidden(l_hdDungeonUi.mOwner->mUi)) {
        mpPlayerMarker->mInfo.mFloor = 10;
        return;
    }
    if (!dungeonDoorList()) return;
    hdDungeonDoorList_l* list = dungeonDoorList();
    s32 count = list->mCount;
    hdDungeonDoorRecord_l* record = list->mRecords;
    for (s32 index = 0; index < count; ++index, ++record) {
        if (!isPlayerDoor(record)) continue;
        f32 x = record->mPosition.x;
        f32 originX = mMapOrigin.x;
        f32 originY = mMapOrigin.y;
        f32 dx = fsubs_ppc(x, originX);
        f32 z = record->mPosition.z;
        f32 dy = fsubs_ppc(z, originY);
        hdDungeonStage_l* stage = &dungeonPlay()->mStage;
        u32 floor = dungeonStageFloor(stage, record->mPosition.y);
        hdDungeonMarker_l* marker = mpPlayerMarker;
        marker->mInfo.mPosition.x = dx;
        marker->mInfo.mPosition.y = dy;
        marker->mInfo.mFloor = floor;
    }
}

/* 02656B38 */
void hdDungeonMap_c::updatePlayerMarkers() {
    mpBossMarker->mInfo.mVisible = 0;
    mpSpecialMarker->mInfo.mVisible = 0;
    mpPartnerMarker->mInfo.mVisible = 0;
    for (u32 index = 0; index < 3; ++index) mpItemMarkers[index]->mInfo.mVisible = 0;
    if (dungeonUiView()->mBossIndex != 3) {
        hdDungeonMarker_l* marker = findMarker();
        if (!marker) {
            hdDungeonSaveView_l* save = sSave.mpSave;
            if (save->mPlayerMode == 1) {
                if (dungeonUiView()->mBossIndex == 5 && dungeonEventBit(save->mEvents, 0x4004)) return;
                updateBossMarker(mpSpecialMarker);
            } else if (save->mPlayerMode == 2) {
                if (dungeonUiView()->mBossIndex == 4 && dungeonEventBit(save->mEvents, 0x3A02)) return;
                updateBossMarker(mpBossMarker);
            }
            return;
        }
        hdDungeonFloorInfo_l* info = findMarkerInfo();
        if (!info) return;
        u32 boss = bossMarkerActive();
        u32 special = specialMarkerActive();
        if (boss | special) {
            auto* actor = static_cast<hdDungeonActor_l*>(playerActor());
            if (actor) {
                u32 visible = 1;
                if (u32(actor->mFlags) & 0x2000) {
                    void* playerActor = sPlayerActor;
                    auto* player = static_cast<hdDungeonActor_l*>(playerActor);
                    visible = player && (boss ? (u32(player->mPlayerFlags) & 0x800) != 0 : (sPlayerFlags & 1) != 0);
                }
                info->mVisible = visible;
                info->mLinked = isLinkedPlayer() ^ 1;
            } else if (dungeonUiView()->mHidden) {
                info->mLinked = 0;
                info->mVisible = 1;
            }
            info->mFloor = mBossFloor;
        } else {
            if (!partnerMarkerActive()) return;
            info->mVisible = !(u32(dungeonPlay()->mFlags) & 0x10000);
            info->mFloor = mBossFloor;
        }
        dungeonMarkerMap(marker, mpMap);
        auto* source = reinterpret_cast<be<u32>*>(&mBossPosition);
        auto* destination = reinterpret_cast<be<u32>*>(&info->mPosition);
        destination[0] = u32(source[0]); destination[1] = u32(source[1]);
        reinterpret_cast<be<u32>*>(&info->mAngle)[0] = u32(source[2]);
        return;
    }
    hdDungeonFloorInfo_l* info = &mpPartnerMarker->mInfo;
    if (ea(info) != 0) {
        if (partnerMarkerActive()) {
            info->mVisible = !(u32(dungeonPlay()->mFlags) & 0x10000);
            info->mFloor = mSpecialFloor;
            auto* source = reinterpret_cast<be<u32>*>(&mSpecialMapPosition);
            auto* destination = reinterpret_cast<be<u32>*>(&info->mPosition);
            destination[0] = u32(source[0]); destination[1] = u32(source[1]);
            reinterpret_cast<be<u32>*>(&info->mAngle)[0] = u32(mSpecialAngle);
            dungeonMarkerMap(mpPartnerMarker, mpMap);
        } else info->mVisible = dungeonUiView()->mHidden;
    }
    if (mAllItemsFound) return;
    u32 found = 0;
    for (u32 index = 0; index < 3; ++index) {
        if (!mItemFound[index]) continue;
        hdDungeonMarker_l* marker = mpItemMarkers[index];
        info = &marker->mInfo;
        if (ea(info) == 0) continue;
        if (itemMarkerActive(index)) {
            if (!specialMarkerVisible(index)) info->mVisible = 1;
            info->mFloor = mItemFloors[index];
            auto* source = reinterpret_cast<be<u32>*>(&mItemMapPositions[index]);
            auto* destination = reinterpret_cast<be<u32>*>(&info->mPosition);
            destination[0] = u32(source[0]); destination[1] = u32(source[1]);
            reinterpret_cast<be<u32>*>(&info->mAngle)[0] = u32(mItemAngles[index]);
            dungeonMarkerMap(mpItemMarkers[index], mpMap);
            if (specialMarkerFlag(index)) { reinterpret_cast<hdDungeonMapStatue_c*>(mpItemMarkers[index].get())->setSecondStatue(); ++found; }
            else reinterpret_cast<hdDungeonMapStatue_c*>(mpItemMarkers[index].get())->setFirstStatue();
            void* linked = dungeonPlay()->mpLinkedActor;
            info->mLinked = linked == dungeonItemActor(index);
        } else info->mVisible = dungeonUiView()->mHidden;
    }
    if (found >= 3) mAllItemsFound = 1;
}

struct hdDungeonMapIconData_l {
    u8 mReserved[0xC]; be<u32> mKind;
    u8 mReservedName[0x80-0x10]; be<u8> mName[1];
};
struct hdDungeonMapIcons_l : HdPadMap {
    u8 mReservedCounts[0x530-sizeof(HdPadMap)];
    be<s32> mEnemyCount; be<s32> mSpecialCount;
    gptr<hdDungeonMapIconData_l> mEnemies[16];
    gptr<hdDungeonMapIconData_l> mSpecialItems[16];
};
WWHD_OFFSET(hdDungeonMapIcons_l, mEnemies, 0x538);
WWHD_OFFSET(hdDungeonMapIcons_l, mSpecialItems, 0x578);
struct hdDungeonMapIcon_l : hdDungeonMarker_l {
    gptr<hdDungeonMapIconData_l> mpData; be<u32> mDataKind;
};
/* inlined into hdDungeonMap_c::initSpecialItems */
static s32 dungeonStringLength(hdUiSafeString* text) {
    text->assureTermination();
    be<u8>* cursor = text->mText;
    s32 length = 0;
    while (cursor[length]) {
        if (++length > 0x40000) return 0;
    }
    return length;
}
/* inlined into hdDungeonMap_c::initSpecialItems */
static bool dungeonStringContains(hdUiSafeString* text, hdUiSafeString* pattern) {
    s32 length = dungeonStringLength(text);
    s32 patternLength = dungeonStringLength(pattern);
    s32 last = length - patternLength;
    for (s32 offset = 0; offset <= last; ++offset) {
        hdUiSafeString suffix;
        suffix.mVtable = &sCaptionNameVtable;
        suffix.mText = text->mText.get() + offset;
        dungeonDoorName(&suffix);
        suffix.assureTermination();
        be<u8>* snapshot = suffix.mText;
        pattern->assureTermination();
        be<u8>* expected = pattern->mText;
        if (snapshot == expected || patternLength <= 0) return true;
        be<u8>* actual = suffix.mText;
        bool equal = true;
        for (s32 index = 0; index < patternLength; ++index) {
            u8 a = actual[index]; u8 b = expected[index];
            if (a != b) { equal = false; break; }
            if (!a) break;
        }
        if (equal && offset != -1) return true;
    }
    return false;
}
struct hdDungeonFloorLabels_l { hdUiSafeString mNames[7]; };
WWHD_SIZE(hdDungeonFloorLabels_l, 0x38);
/* 026588C0 */
void hdDungeonMap_c::initSpecialItems() {
    hdDungeonFloorLabels_l labels;
    const char* names[7] = {"B2", "B1", "1F", "2F", "3F", "4F", "Outdoor"};
    u32 floors[7] = {0x7E,0x7F,0x80,0x81,0x82,0x83,0x85};
    for (u32 index = 0; index < 7; ++index) {
        labels.mNames[index].mText.v = guest_addr(names[index]);
        labels.mNames[index].mVtable = &sCaptionNameVtable;
    }
    if (mpMap) {
        mEnemyCount = static_cast<hdDungeonMapIcons_l*>(mpMap.get())->mEnemyCount;
        auto* data = static_cast<hdDungeonMapIcons_l*>(mpMap.get())->mEnemies;
        for (s32 index = 0; index < mEnemyCount; ++index) {
            u32 slot = u32(index) < 16 ? index : 0;
            auto* marker = static_cast<hdDungeonMapIcon_l*>(mpEnemies[slot].get());
            marker->mpData = data[slot];
            u32 kind = data[slot]->mKind;
            if (kind) marker->mDataKind = kind;
            hdUiSafeString name;
            name.mText = data[slot]->mName;
            name.mVtable = &sCaptionNameVtable;
            for (u32 floor = 0; floor < 7; ++floor) {
                if (!dungeonStringContains(&name, &labels.mNames[floor])) continue;
                marker->mInfo.mFloor = floors[floor];
                break;
            }
            reinterpret_cast<hdDungeonMapUp_c*>(marker)->refreshFloorText();
            reinterpret_cast<hdDungeonMapUp_c*>(marker)->findDungeonEntrance();
        }
        mSpecialItemCount = static_cast<hdDungeonMapIcons_l*>(mpMap.get())->mSpecialCount;
        data = static_cast<hdDungeonMapIcons_l*>(mpMap.get())->mSpecialItems;
        for (s32 index = 0; index < mSpecialItemCount; ++index) {
            u32 slot = u32(index) < 16 ? index : 0;
            auto* marker = static_cast<hdDungeonMapIcon_l*>(mpSpecialItems[slot].get());
            marker->mpData = data[slot];
            u32 kind = data[slot]->mKind;
            if (kind) marker->mDataKind = kind;
            hdUiSafeString name;
            name.mText = data[slot]->mName;
            name.mVtable = &sCaptionNameVtable;
            for (u32 floor = 0; floor < 7; ++floor) {
                if (!dungeonStringContains(&name, &labels.mNames[floor])) continue;
                marker->mInfo.mFloor = floors[floor];
                break;
            }
            dungeonSpecialInit(marker);
            dungeonSpecialUpdate(marker);
        }
    }
    dungeonDestroyStrings(labels.mNames, 7, sizeof(hdUiSafeString), guest_fn((const void*)dungeonStringDestroy), 0, 0);
}

struct hdDungeonInputManager_l {
    u8 mReserved[0x24]; be<u32> mFlags;
    u8 mReservedStick[0x138-0x28]; HdPadPoint mStick;
    u8 mReservedTouch[0x1B4-0x140]; HdPadPoint mTouch;
};
struct hdDungeonInputManagerPointer_l { gptr<hdDungeonInputManager_l> mpManager; };
static hdDungeonInputManagerPointer_l sInputManager;
static HdPadPoint sNoTouch;
static hdPartsStateId_c sMapDragState;
static hdPartsStateId_c sMapZoomInState;
static hdPartsStateId_c sMapZoomOutState;
struct hdDungeonMapInput_l {
    u8 mReserved[0x4B0]; gptr<HdPadPane> mpPane;
};
/* 02654B6C */
void hdDungeonMap_c::checkZoomInput() {
    if (!mpMap) return;
    if (!seaMap(mpMap)->isScrollingOrZoomed() && !seaMap(mpMap)->isZoomedIn() && !seaMap(mpMap)->isZoomedOrZoomingOut()) return;
    HdPadEventSource* input = dungeonInput();
    if (!input) return;
    HdPadEvent event;
    dungeonEventCopy(&event, &input->mEvent);
    hdDungeonInputManager_l* manager = sInputManager.mpManager;
    u32 kind = event.mKind;
    u32 id = mpInputMode->mId;
    HdPadPoint* touch = &sNoTouch;
    if (u32(manager->mFlags) & 1) touch = &manager->mTouch;
    if (kind == 2 || kind == 3) {
        if (event.mId != id) return;
        if (kind == 2) dungeonUiSound("SE_DRC_TOUCH_ON");
        auto* source = reinterpret_cast<be<u32>*>(touch);
        auto* destination = reinterpret_cast<be<u32>*>(&mTouchPosition);
        destination[0] = u32(source[0]); destination[1] = u32(source[1]);
        return;
    }
    if (kind == 4) {
        if (event.mId != id) return;
    } else {
        if (kind == 0x65) return;
        if (kind == 0x43) {
            if (event.mId != id) return;
            if (seaMap(mpMap)->isZoomedIn()) changeState(&sMapDragState);
            return;
        }
        if (dungeonIsZoomEvent(&event)) {
            if (!l_hdDungeonActive.mInput->mActive) return;
            HdPadInputMode* mode = dungeonInputMode(dungeonInput());
            if (!mode || mode->mId != id) return;
            HdPadPane* pane = reinterpret_cast<hdDungeonMapInput_l*>(mpMap.get())->mpPane;
            if (pane) {
                HdPadPoint3 position;
                dungeonPaneMatrixPosition(&position, pane);
                auto* source = reinterpret_cast<be<u32>*>(&position);
                auto* destination = reinterpret_cast<be<u32>*>(&mTouchPosition);
                destination[0] = u32(source[0]); destination[1] = u32(source[1]);
            }
        } else {
            f32 y = manager->mStick.y;
            f32 x = manager->mStick.x;
            f32 squared = fmadds(x, x, fmuls_ppc(y, y));
            f32 zero = l_hdDungeonZero;
            f32 length;
            if (squared > zero) {
                f64 inverse = frsqrte(squared);
                f32 square = f32(inverse * round25(inverse));
                f32 halfInverse = f32(inverse * f64(l_hdDungeonHalf));
                f32 correction = fnmsubs(square, squared, l_hdDungeonThree);
                length = fmuls_ppc(fmuls_ppc(correction, halfInverse), squared);
            } else length = fmuls_ppc(zero, squared);
            if (!(length > zero)) return;
            if (!l_hdDungeonActive.mInput->mActive) return;
            HdPadInputMode* mode = dungeonInputMode(dungeonInput());
            if (!mode || mode->mId != id) return;
            mDragPosition.x = zero; mDragPosition.y = zero;
            if (seaMap(mpMap)->isZoomedIn()) changeState(&sMapDragState);
            return;
        }
    }
    if (!seaMap(mpMap)->isZoomedOrZoomingOut()) return;
    if (mZoomMode == 0) {
        dungeonUiSound("SE_MENU_ZOOM_IN");
        changeState(&sMapZoomInState);
        mZoomMode = 1;
    } else if (mZoomMode == 1) {
        dungeonUiSound("SE_MENU_ZOOM_OUT");
        changeState(&sMapZoomOutState);
        mZoomMode = 0;
    }
}

/* WWHD glue
ext 02704D2C void dungeonPaneMatrixPosition(HdPadPoint3*,HdPadPane*)
ext 02689BA0 f32 hdMapDisplayChase(HdPadPoint*,HdPadPoint*,f32,f32,f32)
ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString*,hdUiSafeString*)
ext 026F5B74 u32 hdButtonPart_c::getReceiverId()
ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
ext 0265EBF8 void dungeonStringDestroy(hdUiSafeString*,s32) callback
ext 028F0164 void dungeonDestroyStrings(void*,u32,u32,u32,u32,u32)
data sFloorChildren 100F0150
data sItemRooms 100F0290
ext 0268C3CC hdDungeonFoundItem_l* dungeonDataItem(hdDungeonMapData_l*,u32,s32)
data sHolderVtable 100F0848
data sSecondaryVtable 100F0858
ext 0268C2D4 void dungeonDataDestroy(hdDungeonMapData_l*,s32)
ext 0273AF40 void dungeonDelete(hdDungeonMap_c*)
data sItemChildren 100F0164
data sPlayerActor 101CEF74
data sAngleScale 100F02E0
data l_hdDungeonHalfTurn 100F02E4
data sMapTrackingState 10496120
data sMapBusyState 10492560
data sTwo 100F0320
ext 0267D5EC void hdSeaChartMap_c::setInterpolationBounds(f32, f32)
ext 02685620 void dungeonMarkerScales(hdDungeonMarker_l*,f32,f32)
ext 0267D1F4 u32 hdSeaChartMap_c::isZoomedIn()
ext 0267D2E0 u32 hdSeaChartMap_c::isZoomedOrZoomingOut()
ext 026F6288 u32 dungeonIsZoomEvent(HdPadEvent*)
data sInputManager 101F5088
data sNoTouch 1049FFA0
data sMapDragState 10492620
data sMapZoomInState 10492650
data sMapZoomOutState 10492680
str 100F025C "SE_DRC_TOUCH_ON"
str 100F026C "SE_MENU_ZOOM_IN"
str 100F027C "SE_MENU_ZOOM_OUT"
ext 02657790 void dungeonUpdateParts(hdDungeonMap_c*)
ext 0265799C void dungeonUpdateMarkers(hdDungeonMap_c*)

ext 025C4740 hdDungeonDoorList_l* dungeonDoorList()
ext 0265EC0C void dungeonDoorName(hdUiSafeString*)
str 100F02F4 "door20"
str 100F02FC "door12"
ext 02666DA0 void hdDungeonMapUp_c::refreshFloorText()
ext 02666EA4 void hdDungeonMapUp_c::findDungeonEntrance()
ext 02660880 void dungeonSpecialInit(hdDungeonMapIcon_l*)
ext 02660984 void dungeonSpecialUpdate(hdDungeonMapIcon_l*)
str 100F030C "B2"
str 100F030F "B1"
str 100F0312 "1F"
str 100F0315 "2F"
str 100F0318 "3F"
str 100F031B "4F"
str 100F0304 "Outdoor"
data sCaptionNameVtable 100F00CC
data sCaptionTextVtable 100F00E4
data sCaptions 104922E0
str 100F02F0 ""
data sFixedTreasures 101F5A70
data sFixedDoors 101F5AB0
data sFullTurn 100F02EC
data sOne 100F0220
data sMapAngleOffset 100F02E8
ext 02689304 u32 dungeonUiRotated(HdPadUi*)
ext 0267D60C void hdSeaChartMap_c::setZoomY(f32)
data sButtonMap 100F00FC
str 100F0224 "SE_MENU_CURSOR"
ext 026F61CC u32 dungeonIsCursorEvent(u32)
ext 026F621C u32 dungeonIsDirectionEvent(u32)
ext 02615154 void dungeonInputSetDefault(HdPadEventSource*,HdPadInputMode*)
ext 02615140 HdPadInputMode* dungeonInputMode(HdPadEventSource*)
ext 02001C00 HdPadInputMode* dungeonDirectionReceiver(HdPadInputMode*,u32)
ext 026F6514 void dungeonCursorFeedback()
str 100F0234 "SE_MENU_MINI_WINDOW_OPEN"
ext 02030B38 void dungeonUiSound(const char*)
ext 02001338 void dungeonEventConstruct(HdPadEvent*,u32,u32,u32)
ext 02001FF4 void dungeonQueueEvent(hdDungeonEventQueue_l*,HdPadEvent*)
ext 02001490 void dungeonEventDestroy(HdPadEvent*)
ext 020013C0 void dungeonEventCopy(HdPadEvent*,HdPadEvent*)
ext 0267D078 u32 hdSeaChartMap_c::isScrollingOrZoomed()
data sSave 101F84DC
data sCurrentRoom 1047E6C8
data sPlayerSpecialVisible 101D5F3E
data sPlayerSecondaryVisible 101BDC2D
data sPlayerFlags 101BDC2A
data sWindow 101F5A44
data sWindowActiveState 104920F0
ext 026538F4 void dungeonWindowChange(hdDungeonWindow_l*,hdPartsStateId_c*)
ext 02287684 u32 dungeonPlayerSpecialVisible(void*)
ext 02221B68 u32 dungeonPlayerSecondaryVisible(void*)
ext 02685238 void dungeonMarkerClear(hdDungeonMarker_l*)
ext 025200D4 hdDungeonPlay_l* dungeonPlay()
ext 0268CB98 void dungeonDataUpdate(hdDungeonMapData_l*)
ext 0268C960 void dungeonDataOpen(hdDungeonMapData_l*)
ext 0268C4C4 void dungeonDataCalc(hdDungeonMapData_l*)
ext 02006478 hdPartsStateId_c* dungeonCurrentState(HdPadStateController*)
ext 020063C0 void dungeonChangeMapState(HdPadStateController*,hdPartsStateId_c*)
ext 0267D5F8 void hdSeaChartMap_c::setTargetPosition(HdPadPoint*)
ext 025C4734 hdDungeonTreasureList_l* dungeonTreasureList()
ext 02689DDC u32 dungeonTreasureFound(HdPadUi*,u32)
ext 026666D0 void hdDungeonMapTreasure_c::setVisible(u32)
ext 026666F4 u32 hdDungeonMapTreasure_c::isVisible()
ext 02689D28 u32 dungeonRoomVisited(HdPadUi*,s32)
ext 02665FD4 void hdDungeonMapStatue_c::setFirstStatue()
ext 02666020 void hdDungeonMapStatue_c::setSecondStatue()
ext 025B8B94 u32 dungeonEventBit(void*,u32)
ext 0268714C u32 dungeonUiPlayerHidden(HdPadUi*)
ext 026855D0 void dungeonMarkerMap(hdDungeonMarker_l*,HdPadMap*)
ext 02615398 u32 dungeonInputId(HdPadEventSource*)
ext 026150C0 void dungeonInputSetReceiver(HdPadEventSource*,HdPadInputMode*)
ext 0258DC48 u32 dungeonStageFloor(hdDungeonStage_l*,f32)
ext 025C11DC hdDungeonRoomData_l* dungeonRoomData(hdDungeonRoomControl_l*,s32)
ext 0268A920 void* dungeonItemActor(u32)
ext 022ADF24 u32 dungeonItemActorVisible(void*)
ext 022ADFCC u32 dungeonSpecialActorVisible(void*)

ext 02701C14 void dungeonShowItem(hdParts_c*)
ext 0268936C u32 dungeonUiBusy(HdPadUi*)
ext 0268708C u32 dungeonUiPartnerChanged(HdPadUi*)
ext 02001BE4 void dungeonReceiverPane(HdPadInputMode*,u32,HdPadPane*)
ext 0267D018 void hdSeaChartMap_c::cancelOpening()




*/
