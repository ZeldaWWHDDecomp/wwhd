/** HD sea/dungeon map display coordination and chart progress. Private decomp source. */
#include "d/hd_screen_map_display_coordinator.h"
static void* sMapInput;
static s8 sSpecialMode;
static hdSaveInfoHolder_c* sSave;
static hdMapDisplayInstances sInstances;
static HdPadInputActive* sInputActive;
static u8 sMapSafeStringVTable;
static hdMapInitializerWords_l sMapInitializerWords;
static u8 sMapRegistration;
static u8 sMapAngleRegistration;
static u8 sMapLimitRegistration;
static f32 sMapAngleMin;
static f32 sMapAngleMax;
static f32 sMapHalfWidth;
static f32 sMapHalfHeight;
static u8 sMapAngleGuard;
static u8 sMapLimitGuard;
static u32 sStateIdGuard;
static u32 sStateIdCounter;
static u8 sMapDescriptorVTable;

static hdMapLocationList_c* sMapLocations;
static hdMapChartArchive_l* sMapChartArchive;
static hdMapHeapManager_l* sMapHeapManager;
static u8 sDefaultPrimaryContext;
static u8 sDefaultSecondaryContext;
static hdMapChartHeap_l* sChartHeap;
static f32 sPlayerLimitX;
static f32 sPlayerLimitY;
static f32 sPlayerMargin;
static hdMapDisplayCoordinator_c* sUiController;
static u8 sCoordinatorBaseVTable;
static u8 sCoordinatorOwnerVTable;
static u8 sCoordinatorStateVTable;
static u8 sCoordinatorVTable;
static hdStateId sNoStateId_l;
static hdDungeonStageIds_l sDungeonStageIds;
static hdMapChartSwitches_l sSeaChartSwitches;
static hdMapChartSwitches_l sDungeonChartSwitches;
void seaMapOpen(HdPadMenuMap*);
void seaMapClose(HdPadMenuMap*);
void seaMapReset(HdPadMenuMap*);
void dungeonMapOpen(HdPadDungeon*);
void dungeonMapClose(HdPadDungeon*);
void dungeonMapReset(HdPadDungeon*);
void mapSetInputMode(void*, u32);
void seaMapUpdateSelection(HdPadMenuMap*);
void seaMapUpdateCursor(HdPadMenuMap*);
void seaMapClearSelection(HdPadMenuMap*);
u32 seaMapSelected(HdPadMenuMap*);
u32 seaMapCursorActive(HdPadMenuMap*);
u32 seaMapSelectionEmpty(HdPadMenuMap*);
void dungeonMapUpdateSelection(HdPadDungeon*);
void dungeonMapClearSelection(HdPadDungeon*);
u32 mapIsOpen(void*, s32);
void mapClearOpen(void*, s32);
u32 mapIsComplete(void*, s32);
void mapClearComplete(void*, s32);
u32 mapIsObtained(void*, s32);
void mapSetObtained(void*, s32);
u32 mapIsTriforce(void*, s32);
u32 mapGetCollectCount(void*);
void dungeonMapUpdateCoordinate(HdPadDungeon*);
void seaMapSetMarker(HdPadMenuMap*, u32);
void seaMapSetMarkersActive(HdPadMenuMap*, u32);
void seaCellsUpdate(HdPadCellArray*);
hdSavePlayLog_c* getMapSettings(hdSaveHolders_c*);
u32 mapSettingsHasCell(hdSavePlayLog_c*, s32);
void mapSettingsSetLocation(hdSavePlayLog_c*, s32);
hdMapStateDescriptor_l* mapGetCurrentState(HdPadStateController*);
void mapChangeState(hdStateMachine*, hdStateId*);
void seaMapShow(HdPadMenuMap*);
void dungeonMapShow(HdPadDungeon*);
void seaMapSetOrigin(HdPadMenuMap*, u32);
void seaMapSetMode(HdPadMenuMap*, u32);
void seaMapSetScale(HdPadMenuMap*, u32);
void seaMapSetTargetOrigin(HdPadMenuMap*, u32);
void seaMapSetTargetMode(HdPadMenuMap*, u32);
void seaMapSetTargetScale(HdPadMenuMap*, u32);
void dungeonMapSetOrigin(HdPadDungeon*, u32);
void dungeonMapSetMode(HdPadDungeon*, u32);
void dungeonMapSetScale(HdPadDungeon*, u32);
void dungeonMapSetTargetOrigin(HdPadDungeon*, u32);
void dungeonMapSetTargetMode(HdPadDungeon*, u32);
void dungeonMapSetTargetScale(HdPadDungeon*, u32);
void mapSettingsSetSeaPosition(hdSavePlayLog_c*, HdPadPoint*);
void mapSettingsSetDungeonPosition(hdSavePlayLog_c*, HdPadPoint*);
void mapSettingsSetSeaScale(hdSavePlayLog_c*, f32);
void mapSettingsSetDungeonScale(hdSavePlayLog_c*, f32);
HdPadPoint* mapSettingsGetSeaPosition(hdSavePlayLog_c*);
HdPadPoint* mapSettingsGetDungeonPosition(hdSavePlayLog_c*);
f32 mapSettingsGetSeaScale(hdSavePlayLog_c*);
f32 mapSettingsGetDungeonScale(hdSavePlayLog_c*);
void seaMapSaveSettings(HdPadMenuMap*);
void dungeonMapSaveSettings(HdPadDungeon*);
void seaMapUpdateMode(HdPadMenuMap*);
void dungeonMapUpdateMode(HdPadDungeon*);
void dungeonMapUpdateNormalMode(HdPadDungeon*);
hdMapPlay_l* mapGetPlay();
void hideLinkMarker(HdPadStateController*, hdMapStateDescriptor_l*);
void dungeonMapFinishClose(HdPadDungeon*);
HdPadInputMode* mapGetInputMode(void*);
u32 seaCellsAcceptMode(HdPadCellArray*, HdPadInputMode*);
u32 mapIconsGetMode(void*);
u32 mapIsVisitedRoom(void*, s32);
u32 mapIsStageTreasureOpened(u32, u32);
u32 mapIsTreasureOpened(void*, u32);
u32 mapHasDungeonItem(void*, u32);
u32 mapStageHasMap(u32);
u32 mapStageHasBossKey(u32);
u32 mapStageHasCompass(u32);
void* mapAllocate(u32);
void mapDelete(void*);
void mapConstructState(hdStateMachine*, hdStateOwner*, hdStateId*);
void detachSeaMap(hdMapDisplayCoordinator_c*, HdPadMenuMap*);
void detachDungeonMap(hdMapDisplayCoordinator_c*, HdPadDungeon*);
void freeChartArray(be<s32>*);
u32 mapUiCanUpdate();
u32 mapUiTransitionBusy();
void mapExecuteState(hdStateMachine*);
void mapPositionToCoordinate(be<s8>*, be<s8>*, f32, f32);
u32 mapStageGetX(void*);
u32 mapStageGetY(void*);
void mapSafeStringTouch(hdMapSafeString_l*);
u32 mapEventIsSet(void*, u32);
u32 mapSaveIsSwitch(void*, s32, s32);
u32 mapMemoryIsSwitch(void*, s32);
void mapWorldToSector(be<s8>*, be<s8>*, f32, f32);
hdMapCellCenter_l* mapFindCellCenter(void*, HdPadCoordinate*);
void allocateChartArray(be<s32>*, s32, void*, s32);
hdChartEntry_c* allocateChartEntry(u32, void*, s32);
void sortChartArray(be<s32>*, hdChartCompareCallback);
s32 compareChartEntries(hdChartEntry_c**, hdChartEntry_c**);
void attachSeaMap(hdMapDisplayCoordinator_c*, gptr<HdPadMenuMap>*, void*,
                  void*);
void attachDungeonMap(hdMapDisplayCoordinator_c*, gptr<HdPadDungeon>*, void*,
                      void*);
void mapBlankEnter(hdMapDisplayCoordinator_c*);
void mapBlankExecute(hdMapDisplayCoordinator_c*);
void mapBlankLeave(hdMapDisplayCoordinator_c*);
void mapBlankGetState(hdMapDisplayCoordinator_c*);
void mapDisplayMenuEnter(hdMapDisplayCoordinator_c*);
void mapDisplayMenuExecute(hdMapDisplayCoordinator_c*);
void mapDisplayMenuLeave(hdMapDisplayCoordinator_c*);
void mapDisplayMenuGetState(hdMapDisplayCoordinator_c*);
void mapDisplayDungeonEnter(hdMapDisplayCoordinator_c*);
void mapDisplayDungeonExecute(hdMapDisplayCoordinator_c*);
void mapDisplayDungeonLeave(hdMapDisplayCoordinator_c*);
void mapDisplayDungeonGetState(hdMapDisplayCoordinator_c*);
void mapInAllMenuEnter(hdMapDisplayCoordinator_c*);
void mapInAllMenuExecute(hdMapDisplayCoordinator_c*);
void mapInAllMenuLeave(hdMapDisplayCoordinator_c*);
void mapInAllMenuGetState(hdMapDisplayCoordinator_c*);
void mapInAllDungeonEnter(hdMapDisplayCoordinator_c*);
void mapInAllDungeonExecute(hdMapDisplayCoordinator_c*);
void mapInAllDungeonLeave(hdMapDisplayCoordinator_c*);
void mapInAllDungeonGetState(hdMapDisplayCoordinator_c*);
void mapOutAllMenuEnter(hdMapDisplayCoordinator_c*);
void mapOutAllMenuExecute(hdMapDisplayCoordinator_c*);
void mapOutAllMenuLeave(hdMapDisplayCoordinator_c*);
void mapOutAllMenuGetState(hdMapDisplayCoordinator_c*);
void mapOutAllDungeonEnter(hdMapDisplayCoordinator_c*);
void mapOutAllDungeonExecute(hdMapDisplayCoordinator_c*);
void mapOutAllDungeonLeave(hdMapDisplayCoordinator_c*);
void mapOutAllDungeonGetState(hdMapDisplayCoordinator_c*);
void mapChangeToMenuEnter(hdMapDisplayCoordinator_c*);
void mapChangeToMenuExecute(hdMapDisplayCoordinator_c*);
void mapChangeToMenuLeave(hdMapDisplayCoordinator_c*);
void mapChangeToMenuGetState(hdMapDisplayCoordinator_c*);
void mapChangeToDungeonEnter(hdMapDisplayCoordinator_c*);
void mapChangeToDungeonExecute(hdMapDisplayCoordinator_c*);
void mapChangeToDungeonLeave(hdMapDisplayCoordinator_c*);
void mapChangeToDungeonGetState(hdMapDisplayCoordinator_c*);
void mapRegisterGlobal(void*);
void mapInitializeAngle(void*);
void mapInitializeLimits(void*);
static hdMapStateDescriptor_l sBlankState;
static hdMapStateDescriptor_l sDisplayMenuMapState;
static hdMapStateDescriptor_l sDisplayDungeonMapState;
static hdMapStateDescriptor_l sInAllMenuMapState;
static hdMapStateDescriptor_l sInAllDungeonMapState;
static hdMapStateDescriptor_l sOutAllMenuMapState;
static hdMapStateDescriptor_l sOutAllDungeonMapState;
static hdMapStateDescriptor_l sMenuMapDisplayState;
static hdMapStateDescriptor_l sMenuMapBlankState;
static hdMapStateDescriptor_l sDungeonMapDisplayState;
static hdMapStateDescriptor_l sDungeonMapBlankState;
static hdMapStateDescriptor_l sDungeonMapWaitState;
static hdMapStateDescriptor_l sLinkPosDisplayState;
static hdMapStateDescriptor_l sLinkPosMoveState;
static hdMapStateDescriptor_l sLinkPosWaitState;
static hdMapStateDescriptor_l sChangeToMenuMapState;
static hdMapStateDescriptor_l sChangeToDungeonMapState;
static hdMapStateDescriptor_l sUiBlockedState;
static hdMapStageKinds_l sStageKinds;
static inline hdMapStageInfo_l* getMapStageInfo() {
    auto* play = mapGetPlay();
    auto* stage = &play->mStage;
    return gabi::call_ptr<hdMapStageInfo_l*>(stage->mpVTable[0x15C / 4].get(),
                                             stage);
}
static inline void copyMapPoint(HdPadPoint* dest, HdPadPoint* source) {
    auto* bitsDest = reinterpret_cast<hdMapPointBits_l*>(dest);
    auto* bitsSource = reinterpret_cast<hdMapPointBits_l*>(source);
    bitsDest->mX = bitsSource->mX;
    bitsDest->mY = bitsSource->mY;
}
static inline void dispatchSeaInput(hdMapChildView_l* child, u32 context) {
    gabi::call_ptr<void>(child->mpVTable[0x6C / 4].get(), child, context);
}
static inline void dispatchDungeonInput(hdMapChildView_l* child, u32 event,
                                        u32 context) {
    gabi::call_ptr<void>(child->mpVTable[0x74 / 4].get(), child, event,
                         context);
}
static inline void* getMapStagePosition() {
    auto* play = mapGetPlay();
    auto* stage = &play->mStage;
    return gabi::call_ptr<void*>(stage->mpVTable[0x74 / 4].get(), stage);
}
static inline bool mapCompareStage(hdMapSafeString_l* current,
                                   hdMapSafeString_l* expected,
                                   bool touchTwice) {
    if (touchTwice)
        gabi::call_ptr<void>(current->mpVTable[0x14 / 4].get(), current);
    gabi::call_ptr<void>(current->mpVTable[0x14 / 4].get(), current);
    u32 expectedFn = expected->mpVTable[0x14 / 4].get();
    const char* left = current->mpText;
    gabi::call_ptr<void>(expectedFn, expected);
    const char* right = expected->mpText;
    if (left == right)
        return true;
    auto* a = reinterpret_cast<const be<u8>*>(left);
    auto* b = reinterpret_cast<const be<u8>*>(right);
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = a[i];
        u8 y = b[i];
        if (x != y)
            return false;
        if (x == 0)
            return true;
    }
    return false;
}
static inline void executeMapChild(hdMapChildView_l* child) {
    gabi::call_ptr<void>(child->mpVTable[0x5C / 4].get(), child);
}
static inline bool mapStateEquals(HdPadStateController* machine,
                                  hdMapStateDescriptor_l* expected) {
    auto* current = mapGetCurrentState(machine);
    u32 currentGetId = current->getVTable()[0x14 / 4].get();
    gfn* expectedVTable = expected->getVTable();
    u32 currentId = gabi::call_ptr<u32>(currentGetId, current);
    u32 expectedId =
        gabi::call_ptr<u32>(expectedVTable[0x14 / 4].get(), expected);
    return currentId == expectedId;
}
/* 02685B84 */
void* hdMapDisplayCoordinator_c::getOwner() { return mpOwner; }

/* 02685C84 */
void hdMapDisplayCoordinator_c::executeMenuMap() {
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get());
    if (map)
        executeMapChild(map);
}

/* 02685CA0 */
void hdMapDisplayCoordinator_c::executeDungeonMap() {
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get());
    if (map)
        executeMapChild(map);
}

/* 02685CBC */
void hdMapDisplayCoordinator_c::enterDisplayMenuMap() {
    auto* map = mpSeaMap.get();
    if (map) {
        seaMapOpen(map);
        mVisible = 1;
        mOpened = 1;
    }
}

/* 02685D00 */
void hdMapDisplayCoordinator_c::executeDisplayMenuMap() {
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get());
    if (!map)
        return;
    if (mapStateEquals(&map->mState, &sMenuMapDisplayState))
        mapChangeState(&mState.mMachine, &sDisplayMenuMapState);
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get()));
}

/* 02685DB0 */
void hdMapDisplayCoordinator_c::enterDisplayDungeonMap() {
    auto* map = mpDungeonMap.get();
    if (map) {
        dungeonMapOpen(map);
        mVisible = 1;
        mOpened = 1;
    }
}

/* 02685DF4 */
void hdMapDisplayCoordinator_c::executeDisplayDungeonMap() {
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get());
    if (!map)
        return;
    bool ready = mapStateEquals(&map->mState, &sDungeonMapDisplayState);
    if (!ready)
        ready = mapStateEquals(
            &reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get())->mState,
            &sDungeonMapWaitState);
    if (ready)
        mapChangeState(&mState.mMachine, &sDisplayDungeonMapState);
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get()));
}

/* 02685EF0 */
void hdMapDisplayCoordinator_c::enterHideMenuMap() {
    auto* map = mpSeaMap.get();
    if (map) {
        seaMapClose(map);
        mVisible = 0;
    }
}

/* 02685F30 */
void hdMapDisplayCoordinator_c::executeHideMenuMap() {
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get());
    if (!map)
        return;
    if (mapStateEquals(&map->mState, &sMenuMapBlankState))
        mapChangeState(&mState.mMachine, &sBlankState);
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get()));
}

/* 02685FE0 */
void hdMapDisplayCoordinator_c::enterHideDungeonMap() {
    auto* map = mpDungeonMap.get();
    if (map) {
        dungeonMapClose(map);
        mVisible = 0;
    }
}

/* 02686020 */
void hdMapDisplayCoordinator_c::executeHideDungeonMap() {
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get());
    if (!map)
        return;
    if (mapStateEquals(&map->mState, &sDungeonMapBlankState))
        mapChangeState(&mState.mMachine, &sBlankState);
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get()));
}

/* 026860D0 */
void hdMapDisplayCoordinator_c::restoreInputMode() {
    u32 mode = mInputMode;
    if (mode)
        mapSetInputMode(sMapInput, mode);
}

/* 026860E8 */
void hdMapDisplayCoordinator_c::enterChangeToMenuMap() {
    auto* dungeon = mpDungeonMap.get();
    if (dungeon) {
        dungeonMapClose(dungeon);
        mVisible = 0;
    }
    auto* sea = mpSeaMap.get();
    if (sea) {
        seaMapOpen(sea);
        mVisible = 1;
        mOpened = 1;
    }
    mShowLink = 0;
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get());
    if (map && map->mpLink) {
        bool active =
            mapStateEquals(&map->mpLink->mState, &sLinkPosDisplayState);
        if (!active)
            active = mapStateEquals(
                &reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())
                     ->mpLink->mState,
                &sLinkPosMoveState);
        if (!active)
            active = mapStateEquals(
                &reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())
                     ->mpLink->mState,
                &sLinkPosWaitState);
        if (active)
            mShowLink = 1;
    }
    restoreInputMode();
}

/* 0268626C */
void hdMapDisplayCoordinator_c::executeChangeToMenuMap() {
    if (!mpSeaMap || !mpDungeonMap)
        return;
    if (mapStateEquals(
            &reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())->mState,
            &sMenuMapDisplayState) &&
        mapStateEquals(
            &reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get())->mState,
            &sDungeonMapBlankState)) {
        auto* dungeon = mpDungeonMap.get();
        if (dungeon)
            dungeonMapFinishClose(dungeon);
        mapChangeState(&mState.mMachine, &sDisplayMenuMapState);
    }
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get()));
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get()));
    if (mShowLink) {
        auto* link =
            reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())->mpLink.get();
        if (link)
            executeMapChild(link);
    }
}

/* 026863C8 */
void hdMapDisplayCoordinator_c::leaveChangeMenuMap() { mShowLink = 0; }

/* 026863D4 */
void hdMapDisplayCoordinator_c::enterChangeToDungeonMap() {
    auto* sea = mpSeaMap.get();
    if (sea) {
        seaMapClose(sea);
        mVisible = 0;
    }
    auto* dungeon = mpDungeonMap.get();
    if (dungeon) {
        dungeonMapOpen(dungeon);
        mVisible = 1;
        mOpened = 1;
    }
    mShowLink = 0;
    auto* map = reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get());
    if (map && map->mpLink) {
        bool active =
            mapStateEquals(&map->mpLink->mState, &sLinkPosDisplayState);
        if (!active)
            active = mapStateEquals(
                &reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())
                     ->mpLink->mState,
                &sLinkPosMoveState);
        if (!active)
            active = mapStateEquals(
                &reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())
                     ->mpLink->mState,
                &sLinkPosWaitState);
        if (active) {
            hideLinkMarker(&reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())
                                ->mpLink->mState,
                           &sLinkPosWaitState);
            mShowLink = 1;
        }
    }
    restoreInputMode();
}

/* 02686574 */
void hdMapDisplayCoordinator_c::executeChangeToDungeonMap() {
    if (!mpSeaMap || !mpDungeonMap)
        return;
    if (mapStateEquals(
            &reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get())->mState,
            &sDungeonMapDisplayState) &&
        mapStateEquals(
            &reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())->mState,
            &sMenuMapBlankState)) {
        mapChangeState(&mState.mMachine, &sDisplayDungeonMapState);
    }
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get()));
    executeMapChild(reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get()));
    if (mShowLink) {
        auto* link =
            reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get())->mpLink.get();
        if (link)
            executeMapChild(link);
    }
}

/* 026866C0 */
void hdMapDisplayCoordinator_c::leaveChangeDungeonMap() { mShowLink = 0; }

/* 026866CC */
void hdMapDisplayCoordinator_c::clearInstances() {
    sInstances.mItems[0] = NULL;
    sInstances.mItems[2] = NULL;
    sInstances.mItems[1] = NULL;
}

/* 026866EC */
hdMapDisplayCoordinator_c*
hdMapDisplayCoordinator_c::construct(hdMapDisplayCoordinator_c* instance) {
    if (!instance) {
        instance = static_cast<hdMapDisplayCoordinator_c*>(
            mapAllocate(sizeof(hdMapDisplayCoordinator_c)));
        if (!instance)
            return NULL;
    }
    instance->mpParent = instance;
    instance->mBaseFlags = 0;
    instance->mpVTable = reinterpret_cast<gfn*>(&sCoordinatorBaseVTable);
    instance->mpOwner = &sCoordinatorOwnerVTable;
    instance->mState.mpVTable = &sCoordinatorStateVTable;
    mapConstructState(&instance->mState.mMachine,
                      reinterpret_cast<hdStateOwner*>(instance), &sNoStateId_l);
    instance->mpDungeonMap = NULL;
    instance->mpSeaMap = NULL;
    instance->mState.mpVTable = &sCoordinatorVTable;
    {
        auto* value = &instance->mSeaPosition;
        if (gabi::ea(value) == 0)
            value = static_cast<HdPadPoint*>(mapAllocate(8));
        if (value) {
            value->y = 0.0f;
            value->x = 0.0f;
        }
    }
    instance->mSeaScale = 0.0f;
    {
        auto* value = &instance->mSeaCoordinate;
        if (gabi::ea(value) == 0)
            value = static_cast<HdPadCoordinate*>(mapAllocate(2));
        if (value) {
            value->y = -10;
            value->x = -10;
        }
    }
    instance->mSelectedEntry = -1;
    {
        auto* value = &instance->mDungeonPosition;
        if (gabi::ea(value) == 0)
            value = static_cast<HdPadPoint*>(mapAllocate(8));
        if (value) {
            value->y = 0.0f;
            value->x = 0.0f;
        }
    }
    instance->mDungeonScale = 0.0f;
    {
        auto* value = &instance->mDungeonCoordinate;
        if (gabi::ea(value) == 0)
            value = static_cast<HdPadCoordinate*>(mapAllocate(2));
        if (value) {
            value->y = -10;
            value->x = -10;
        }
    }
    instance->mInputMode = 0;
    instance->mSpecialDungeon = 0;
    instance->mVisible = 0;
    instance->mStageKind = -1;
    instance->mMapKind = 9;
    instance->mpChartArchive = NULL;
    instance->mPreviousMode = 2;
    instance->mDetectedMode = 3;
    instance->mOpened = 0;
    instance->mBoss = 0;
    instance->mpLocations = NULL;
    instance->mShowLink = 0;
    instance->mInterior = 0;
    instance->mMapFlags = 0;
    instance->mMode = 2;
    instance->mRestoreSettings = 0;
    if (gabi::ea(&instance->mChartStorage[0]) == 0)
        mapAllocate(sizeof(instance->mChartStorage));
    auto* charts =
        reinterpret_cast<hdMapChartVector_l*>(&instance->mChartCount);
    if (gabi::ea(charts) == 0)
        charts = static_cast<hdMapChartVector_l*>(
            mapAllocate(sizeof(hdMapChartVector_l)));
    if (charts) {
        charts->mCapacity = 0;
        charts->mCount = 0;
        charts->mpEntries = NULL;
    }
    for (u32 i = 0; i < CHART_TOTAL; ++i)
        instance->mChartStorage[i] = 0;
    clearInstances();
    return instance;
}

/* 026868F8 */
void hdMapDisplayCoordinator_c::destroy(u32 flags) {
    if (gabi::ea(this) == 0)
        return;
    mState.mpVTable = &sCoordinatorVTable;
    clearInstances();
    auto* owner = mState.mMachine.mpOwner.get();
    gabi::call_ptr<void>(owner->mpVTable->mOnStateChange.get(), owner,
                         &mState.mMachine.mpPreviousState);
    owner = mState.mMachine.mpOwner;
    gabi::call_ptr<void>(owner->mpVTable->mOnStateChange.get(), owner,
                         &mState.mMachine.mpCurrentState);
    if (flags & 1)
        mapDelete(this);
}

/* 02686988 */
void hdMapDisplayCoordinator_c::updateStageKind() {
    if (gabi::ea(mapGetPlay()) + 0x5134u == 0)
        return;
    gabi::Local<hdMapSafeString_l> currentStorage;
    auto& current = *currentStorage.get();
    current.mpText = reinterpret_cast<const char*>(mapGetPlay()->mStageName);
    current.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    gabi::Local<hdMapSafeString_l> seaStorage;
    auto& sea = *seaStorage.get();
    sea.mpText.v = gabi::guest_addr("sea");
    sea.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    mapSafeStringTouch(&current);
    if (mapCompareStage(&current, &sea, false)) {
        mMapKind = MAP_SEA;
        return;
    }
    gabi::Local<hdMapSafeString_l> shopStorage;
    auto& shop = *shopStorage.get();
    shop.mpText.v = gabi::guest_addr("Obshop");
    shop.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &shop, true)) {
        mMapKind = MAP_SHOP;
        return;
    }
    gabi::Local<hdMapSafeString_l> abandonedStorage;
    auto& abandoned = *abandonedStorage.get();
    abandoned.mpText.v = gabi::guest_addr("Abship");
    abandoned.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &abandoned, true)) {
        mMapKind = MAP_ABANDONED_SHIP;
        return;
    }
    gabi::Local<hdMapSafeString_l> pirateStorage;
    auto& pirate = *pirateStorage.get();
    pirate.mpText.v = gabi::guest_addr("PShip");
    pirate.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &pirate, true)) {
        mMapKind = MAP_PIRATE_SHIP;
        return;
    }
    gabi::Local<hdMapSafeString_l> fortressStorage;
    auto& fortress = *fortressStorage.get();
    fortress.mpText.v = gabi::guest_addr("MajyuE");
    fortress.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &fortress, true)) {
        mMapKind = MAP_FORTRESS_EXTERIOR;
        return;
    }
    gabi::Local<hdMapSafeString_l> roomStorage;
    auto& room = *roomStorage.get();
    room.mpText.v = gabi::guest_addr("majroom");
    room.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &room, true)) {
        mMapKind = MAP_FORTRESS_INTERIOR;
        return;
    }
    gabi::Local<hdMapSafeString_l> tower2Storage;
    auto& tower2 = *tower2Storage.get();
    tower2.mpText.v = gabi::guest_addr("ma2room");
    tower2.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &tower2, true)) {
        mMapKind = MAP_SAVAGE_LABYRINTH;
        return;
    }
    gabi::Local<hdMapSafeString_l> tower3Storage;
    auto& tower3 = *tower3Storage.get();
    tower3.mpText.v = gabi::guest_addr("ma3room");
    tower3.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    if (mapCompareStage(&current, &tower3, true)) {
        mMapKind = MAP_SAVAGE_LABYRINTH_LOWER;
        return;
    }
    gabi::Local<hdMapSafeString_l> testStorage;
    auto& test = *testStorage.get();
    test.mpText.v = gabi::guest_addr("ADMumi");
    test.mpVTable = reinterpret_cast<gfn*>(&sMapSafeStringVTable);
    mMapKind = mapCompareStage(&current, &test, true) ? MAP_ADMUMI : MAP_OTHER;
}

/* 0268702C */
u32 hdMapDisplayCoordinator_c::isSeaMapForced() {
    s32 kind = mMapKind;
    s8 special = sSpecialMode;
    return kind == MAP_FORTRESS_EXTERIOR && special == 0;
}

/* 0268705C */
u32 hdMapDisplayCoordinator_c::isDungeonMapForced() {
    s32 kind = mMapKind;
    s8 special = sSpecialMode;
    return kind == MAP_SEA && special == 1;
}

/* 0268708C */
u32 hdMapDisplayCoordinator_c::isMapForced() {
    if (isSeaMapForced())
        return 1;
    return isDungeonMapForced() != 0;
}

/* 026870E0 */
u32 hdMapDisplayCoordinator_c::isDungeonStage() {
    s32 type = -1;
    if (getMapStageInfo())
        type = u32(getMapStageInfo()->mStageType) >> 1;
    return type == 2;
}

/* 0268714C */
u32 hdMapDisplayCoordinator_c::isDungeonMode() {
    if (isMapForced())
        return 1;
    return isDungeonStage() != 0;
}

/* 026871A0 */
s32 hdMapDisplayCoordinator_c::getStageType() {
    s32 type = -1;
    if (getMapStageInfo())
        type = u32(getMapStageInfo()->mStageType) >> 1;
    if (isDungeonMode())
        type = 2;
    return type;
}

/* 02687224 */
void hdMapDisplayCoordinator_c::updateInteriorFlags() {
    mBoss = 0;
    mSpecialDungeon = 0;
    mInterior = 0;
    if (!getMapStageInfo())
        return;
    if (gabi::ea(mapGetPlay()) + 0x5134u == 0)
        return;
    u32 kind = (u32(getMapStageInfo()->mFlags) >> 16) & 7;
    if (kind == 3) {
        mSpecialDungeon = 1;
        mInterior = 1;
    } else if (kind == 6)
        mInterior = 1;
    else if (kind == 2)
        mBoss = 1;
}

/* 026872F0 */
void hdMapDisplayCoordinator_c::updateStageFlags() {
    hdMapStageKinds_l kinds;
    for (u32 i = 0; i < 8; ++i) {
        kinds.mPairs[i].mInput = sStageKinds.mPairs[i].mInput;
        kinds.mPairs[i].mOutput = sStageKinds.mPairs[i].mOutput;
    }
    s32 type = getStageType();
    s32 stage = -1;
    for (u32 i = 0; i < 8; ++i)
        if (type == kinds.mPairs[i].mInput) {
            stage = kinds.mPairs[i].mOutput;
            break;
        }
    mStageKind = stage;
    updateInteriorFlags();
}

/* 02687398 */
u32 hdMapDisplayCoordinator_c::canUpdatePlayerPosition() {
    f32 x = 0.0f, y = 0.0f;
    auto* link = mapGetPlay()->mpLink.get();
    if (link) {
        y = reinterpret_cast<be<f32>*>(&link->mPositionZ)->get();
        x = reinterpret_cast<be<f32>*>(&link->mPositionX)->get();
    }
    gabi::Local<HdPadCoordinate> coordinate;
    coordinate.get()->x = -10;
    coordinate.get()->y = -10;
    mapWorldToSector(&coordinate.get()->x, &coordinate.get()->y, x, y);
    f32 relativeX = gabi::fnmsubs(f32(s8(coordinate.get()->x)), 100000.0f, x);
    f32 relativeY = gabi::fnmsubs(f32(s8(coordinate.get()->y)), 100000.0f, y);
    if (coordinate.get()->x != -3 || coordinate.get()->y != -3)
        return 0;
    auto* map = mpSeaMap->mMap.get();
    if (map && map->mCells) {
        auto* center = mapFindCellCenter(map->mCells, coordinate.get());
        if (center) {
            relativeX = relativeX - f32(s16(center->mX));
            relativeY = relativeY - f32(s16(center->mY));
        }
    }
    f32 upperX = sPlayerLimitX + sPlayerMargin,
        upperY = sPlayerLimitY + sPlayerMargin;
    f32 lowerX = -upperX, lowerY = -upperY;
    if (lowerX >= upperX) {
        f32 temp = lowerX;
        lowerX = upperX;
        upperX = temp;
    }
    if (lowerY >= upperY) {
        f32 temp = lowerY;
        lowerY = upperY;
        upperY = temp;
    }
    return !(lowerX > relativeX || relativeX > upperX || lowerY > relativeY ||
             relativeY > upperY);
}

/* 02687674 */
u32 hdMapDisplayCoordinator_c::detectForcedMapMode() {
    if (isDungeonStage())
        return 1;
    if (!isMapForced())
        return 3;
    if (mapGetPlay()->mPlayerStatus & 0x10000)
        return 0;
    return canUpdatePlayerPosition() ? 1 : 0;
}

/* 026876F4 */
void hdMapDisplayCoordinator_c::updateMapKind() {
    if (gabi::ea(mapGetPlay()) + 0x5134u == 0)
        return;
    if (!getMapStageInfo())
        return;
    u32 mode = u32(getMapStageInfo()->mMapMode) & 3;
    if (mode <= 2)
        mDetectedMode = mode;
    if (mStageKind == 0) {
        mDetectedMode = detectForcedMapMode();
        return;
    }
    if (mDetectedMode == 2) {
        u32 kind = (u32(getMapStageInfo()->mFlags) >> 16) & 7;
        bool dungeon = kind == 1 || kind == 3 || kind == 6;
        s32 stage = mStageKind;
        mDetectedMode = dungeon ? 1 : 0;
        if (stage == 7)
            mDetectedMode = 1;
    }
    if (mMapKind == MAP_ADMUMI)
        mDetectedMode = 1;
}

/* 02687830 */
u32 hdMapDisplayCoordinator_c::detectMode() {
    updateMapKind();
    u32 mode = mDetectedMode;
    return mode == 0 ? 0 : mode == 1 ? 1 : 2;
}

/* 0268787C */
u32 hdMapDisplayCoordinator_c::canRestoreSettings() {
    if (mDetectedMode == 1)
        return 0;
    if (mDetectedMode == 0 && mMapKind != 0)
        return 0;
    return 1;
}

/* 026878AC */
void hdMapDisplayCoordinator_c::enableRestoreSettings() {
    if (!canRestoreSettings()) {
        s32 kind = mMapKind;
        mRestoreSettings = 1;
        if (kind != 1)
            return;
    }
    mRestoreSettings = 1;
}

/* 026878F4 */
void hdMapDisplayCoordinator_c::loadMapSettings() {
    if (!sSave) {
        mSeaPosition.y = 0.0f;
        mSeaPosition.x = 0.0f;
        mDungeonPosition.y = 0.0f;
        mSeaScale = 0.0f;
        mDungeonPosition.x = 0.0f;
        mDungeonScale = 0.0f;
        return;
    }
    hdSavePlayLog_c* settings = getMapSettings(&sSave->mHolders);
    HdPadPoint* position = mapSettingsGetSeaPosition(settings);
    copyMapPoint(&mSeaPosition, position);
    settings = getMapSettings(&sSave->mHolders);
    mSeaScale = mapSettingsGetSeaScale(settings);
    settings = getMapSettings(&sSave->mHolders);
    position = mapSettingsGetDungeonPosition(settings);
    copyMapPoint(&mDungeonPosition, position);
    settings = getMapSettings(&sSave->mHolders);
    mDungeonScale = mapSettingsGetDungeonScale(settings);
}

/* 026879BC */
void hdMapDisplayCoordinator_c::initializeMapMode() {
    updateStageKind();
    updateStageFlags();
    u32 mode = detectMode();
    mMode = mode;
    mPreviousMode = mode;
    enableRestoreSettings();
    if (mRestoreSettings)
        loadMapSettings();
}

/* 02687A1C */
hdChartEntry_c* hdMapDisplayCoordinator_c::getChart(s32 index) {
    if (!mpCharts || mChartCount <= 0 || index < 0 || index >= mChartCount)
        return NULL;
    return mpCharts.get()[u32(index)];
}

/* 02687A64 */
hdChartEntry_c* hdMapDisplayCoordinator_c::findChart(u32 chart) {
    if (!mpCharts)
        return NULL;
    s32 count = mChartCount;
    if (count <= 0)
        return NULL;
    if (chart >= CHART_TOTAL)
        return NULL;
    u32 wanted = chart + 1;
    for (s32 i = 0; i < count; ++i) {
        hdChartEntry_c* entry = getChart(i);
        if (entry && u32(s32(entry->mChartNo)) == wanted)
            return entry;
    }
    return NULL;
}

/* 02687AEC */
void hdMapDisplayCoordinator_c::initializeCharts() {
    auto* archive = mpChartArchive.get();
    if (!archive)
        return;
    void* heap = sChartHeap->mpHeap;
    if (mpCharts) {
        s32 count = mChartCount;
        for (u32 i = 0; s32(i) < count; ++i) {
            if (i < u32(count)) {
                auto* entry = mpCharts.get()[i].get();
                if (entry) {
                    mapDelete(i < u32(count) ? entry : NULL);
                    count = mChartCount;
                }
            }
        }
        freeChartArray(&mChartCount);
        archive = mpChartArchive;
    }
    s32 count = archive->mCount;
    if (count > 0)
        allocateChartArray(&mChartCount, count, heap, 4);
    for (s32 i = 0; i < count; ++i) {
        auto* entry = allocateChartEntry(0x26, heap, 4);
        auto* source = &mpChartArchive->mEntries[i];
        entry->mSeaNo = source->mSeaNo;
        entry->mSaveNo = source->mSaveNo;
        entry->mChartNo = source->mChartNo;
        entry->mType = source->mType;
        entry->mUnknown4 = source->mUnknown4;
        entry->mUnknown5 = source->mUnknown5;
        for (u32 j = 0; j < 16; ++j)
            entry->mPositions[j] = source->mPositions[j];
        s32 used = mChartCount;
        if (used < mChartCapacity) {
            mpCharts.get()[used] = entry;
            mChartCount = s32(mChartCount) + 1;
        }
    }
    if (mpCharts && mChartCount > 0)
        sortChartArray(&mChartCount, &compareChartEntries);
    auto* entry = findChart(1);
    if (entry)
        entry->mType = 1;
    entry = findChart(3);
    if (entry)
        entry->mType = 1;
    entry = findChart(5);
    if (entry)
        entry->mType = 1;
    entry = findChart(6);
    if (entry)
        entry->mType = 1;
    entry = findChart(7);
    if (entry)
        entry->mType = 2;
    mChartStorage[0] = 0;
    mChartStorage[1] = 2;
    mChartStorage[2] = 4;
    for (u32 i = 0; i < 41; ++i)
        mChartStorage[i + 3] = i + 8;
    mChartStorage[44] = 1;
    mChartStorage[46] = 5;
    mChartStorage[45] = 3;
    mChartStorage[48] = 7;
    mChartStorage[47] = 6;
    for (u32 i = 49; i < CHART_TOTAL; ++i)
        mChartStorage[i] = i;
    if (!mpCharts)
        return;
    for (u32 i = 0; s32(i) < mChartCount; ++i) {
        u32 index = mChartStorage[i < CHART_TOTAL ? i : 0];
        auto* chart =
            index < u32(mChartCount) ? mpCharts.get()[index].get() : NULL;
        auto* chartNo = gabi::at<be<s8>>(gabi::ea(chart) +
                                         offsetof(hdChartEntry_c, mChartNo));
        *chartNo = s8(i + 1);
    }
}

/* 02687E88 */
u32 hdMapDisplayCoordinator_c::setup() {
    mpLocations = sMapLocations;
    mpChartArchive = sMapChartArchive;
    initializeMapMode();
    initializeCharts();
    auto* slots = sMapHeapManager->mpContexts.get();
    auto* heap = slots[u32(sMapHeapManager->mCount) > 7 ? 7 : 0].get();
    u32 flags = heap->mFlags;
    void* primary = flags & 0x1000
                        ? heap->mpSpecial.get()
                        : (heap->mpPrimary ? heap->mpPrimary.get()
                                           : &sDefaultPrimaryContext);
    void* secondary = (flags & 0x40) && (flags & 0x80)
                          ? &heap->mEmbedded
                          : (heap->mpSecondary ? heap->mpSecondary.get()
                                               : &sDefaultSecondaryContext);
    attachSeaMap(this, &mpSeaMap, primary, secondary);
    attachDungeonMap(this, &mpDungeonMap, primary, secondary);
    auto* owner = mState.mMachine.mpOwner.get();
    auto* current = gabi::call_ptr<hdStateObject*>(
        owner->mpVTable->mCreateState.get(), owner, &sBlankState);
    mState.mMachine.mpCurrentState = current;
    gabi::call_ptr<void>(current->mpVTable->mEnter.get(), current);
    return 1;
}

/* 0268801C */
void hdMapDisplayCoordinator_c::destroyMaps() {
    detachSeaMap(this, mpSeaMap);
    detachDungeonMap(this, mpDungeonMap);
    if (!mpCharts)
        return;
    s32 count = mChartCount;
    if (count > 0)
        for (u32 i = 0; s32(i) < count; ++i) {
            if (i < u32(count)) {
                auto* entry = mpCharts.get()[i].get();
                if (entry) {
                    mapDelete(i < u32(count) ? entry : NULL);
                    count = mChartCount;
                }
            }
        }
    freeChartArray(&mChartCount);
}

/* 026880D0 */
void hdMapDisplayCoordinator_c::updateLinkPosition() {
    auto* actor = mapGetPlay()->mpLink.get();
    if (actor) {
        u32 x = actor->mPositionX;
        u32 z = actor->mPositionZ;
        reinterpret_cast<hdMapPointBits_l*>(&mSeaPosition)->mX = x;
        reinterpret_cast<hdMapPointBits_l*>(&mSeaPosition)->mY = z;
        mSeaScale =
            gabi::fmadds(f32(s16(actor->mAngle)), 0.0054931640625f, 180.0f);
    }
}

/* 02688158 */
void hdMapDisplayCoordinator_c::updateShipPosition() {
    auto* actor = mapGetPlay()->mpShip.get();
    if (actor) {
        u32 x = actor->mPositionX;
        u32 z = actor->mPositionZ;
        reinterpret_cast<hdMapPointBits_l*>(&mDungeonPosition)->mX = x;
        reinterpret_cast<hdMapPointBits_l*>(&mDungeonPosition)->mY = z;
        mDungeonScale =
            gabi::fmadds(f32(s16(actor->mAngle)), 0.0054931640625f, 180.0f);
    }
}

/* 026881E0 */
void hdMapDisplayCoordinator_c::updatePlayerPositions() {
    updateLinkPosition();
    updateShipPosition();
}

/* 02688214 */
void hdMapDisplayCoordinator_c::updateLocationList() {
    bool restore = mRestoreSettings != 0;
    if (restore || mMapKind == 0) {
        f32 x = mSeaPosition.x, y = mSeaPosition.y;
        mapPositionToCoordinate(&mSeaCoordinate.x, &mSeaCoordinate.y, x, y);
        x = mDungeonPosition.x;
        y = mDungeonPosition.y;
        mapPositionToCoordinate(&mDungeonCoordinate.x, &mDungeonCoordinate.y, x,
                                y);
        if (restore) {
            mDungeonCoordinate.x = mSeaCoordinate.x;
            mDungeonCoordinate.y = mSeaCoordinate.y;
        }
        return;
    }
    u32 kind = u32(mMapKind) - 1;
    auto* play = mapGetPlay();
    if (kind <= 2) {
        u32 sector = play->mLastSeaSector;
        u32 row = sector / 7;
        mSeaCoordinate.y = row - 3;
        mSeaCoordinate.x = sector - row * 7 - 3;
    } else if (gabi::ea(play) + 0x5150u != 0) {
        void* stage = getMapStagePosition();
        s32 x = 0;
        if (stage) {
            x = s8(mapStageGetX(stage));
            if (x == 4)
                x = 0;
            mSeaCoordinate.x = x;
            s32 y = s8(mapStageGetY(stage));
            if (y != 4) {
                mDungeonCoordinate.x = mSeaCoordinate.x;
                mDungeonCoordinate.y = y;
                mSeaCoordinate.y = y;
                return;
            }
            mSeaCoordinate.y = 0;
        } else {
            mSeaCoordinate.x = 0;
            mSeaCoordinate.y = 0;
        }
    }
    mDungeonCoordinate.x = mSeaCoordinate.x;
    mDungeonCoordinate.y = mSeaCoordinate.y;
}

/* 0268840C */
hdMapLocation_c* hdMapDisplayCoordinator_c::getLocation(s32 index) {
    auto* locations = mpLocations.get();
    if (!locations || index < 0 || index >= locations->mCount)
        return NULL;
    return &locations->mEntries[index];
}

/* 02688440 */
s32 hdMapDisplayCoordinator_c::findLocation(u32 x, u32 y) {
    auto* locations = mpLocations.get();
    if (!locations)
        return -1;
    s32 count = locations->mCount;
    for (s32 i = 0; i < count; ++i) {
        auto* entry = getLocation(i);
        if (entry && x == u32(s32(entry->mX)) && y == u32(s32(entry->mY)))
            return i;
    }
    return -1;
}

/* 026884D4 */
void hdMapDisplayCoordinator_c::updateLocationSelection() {
    s32 x = mSeaCoordinate.x;
    s32 y = mSeaCoordinate.y;
    mSelectedEntry = findLocation(x, y);
}

/* 02688514 */
void hdMapDisplayCoordinator_c::updateLocations() {
    updateLocationList();
    updateLocationSelection();
}

/* 02688548 */
void hdMapDisplayCoordinator_c::updateDungeonCoordinate() {
    if (mMode != MODE_DUNGEON)
        return;
    auto* map = mpDungeonMap.get();
    if (map)
        dungeonMapUpdateCoordinate(map);
}

/* 02688564 */
u32 hdMapDisplayCoordinator_c::isDungeonInterior() {
    if (mStageKind == 0)
        return 0;
    return isDungeonMode() != 0;
}

/* 026885B0 */
u32 hdMapDisplayCoordinator_c::isSeaExterior() {
    if (mStageKind != 0)
        return 0;
    return isDungeonMode() == 0;
}

/* 026885FC */
void hdMapDisplayCoordinator_c::execute() {
    bool update = !mapUiCanUpdate();
    if (!update)
        update = mapUiTransitionBusy() != 0;
    if (!update)
        update = mapStateEquals(
            reinterpret_cast<HdPadStateController*>(&sUiController->mState),
            &sUiBlockedState);
    if (!update)
        return;
    if (canRestoreSettings())
        updatePlayerPositions();
    updateLocations();
    updateDungeonCoordinate();
    mOpened = 0;
    if (isDungeonInterior()) {
        auto* map = mpDungeonMap.get();
        mStageKind = 0;
        if (map)
            dungeonMapUpdateMode(map);
    } else if (isSeaExterior()) {
        mStageKind = -1;
        mMode = detectMode();
    }
    if (mVisible) {
        u32 mode = detectMode();
        u32 previous = mPreviousMode;
        mMode = mode;
        if (mode != previous) {
            if (mode == MODE_SEA) {
                mapChangeState(&mState.mMachine, &sChangeToMenuMapState);
                mode = mMode;
            } else if (mode == MODE_DUNGEON) {
                mapChangeState(&mState.mMachine, &sChangeToDungeonMapState);
                mode = mMode;
            }
        }
        mPreviousMode = mode;
    }
    mapExecuteState(&mState.mMachine);
}

/* 02688790 */
void hdMapDisplayCoordinator_c::processInput(u32 event, u32 context) {
    if (mapStateEquals(reinterpret_cast<HdPadStateController*>(&mState),
                       &sChangeToMenuMapState)) {
        if (!mpSeaMap || !mpDungeonMap)
            return;
        dispatchDungeonInput(
            reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get()), event,
            context);
        dispatchSeaInput(reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get()),
                         context);
        return;
    }
    if (mapStateEquals(reinterpret_cast<HdPadStateController*>(&mState),
                       &sChangeToDungeonMapState)) {
        if (!mpSeaMap || !mpDungeonMap)
            return;
        dispatchSeaInput(reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get()),
                         context);
        dispatchDungeonInput(
            reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get()), event,
            context);
        return;
    }
    if (mMode == MODE_SEA) {
        auto* map = reinterpret_cast<hdMapChildView_l*>(mpSeaMap.get());
        if (map)
            dispatchSeaInput(map, context);
    } else if (mMode == MODE_DUNGEON) {
        auto* map = reinterpret_cast<hdMapChildView_l*>(mpDungeonMap.get());
        if (map)
            dispatchDungeonInput(map, event, context);
    }
}

/* 02688984 */
void hdMapDisplayCoordinator_c::setSeaMapMarker(u32 unused, u32 marker) {
    auto* map = mpSeaMap.get();
    if (map)
        seaMapSetMarker(map, marker);
}

/* 02688998 */
void hdMapDisplayCoordinator_c::setSeaMapMarkerForward(u32 unused, u32 marker) {
    setSeaMapMarker(unused, marker);
}

/* 0268899C */
void hdMapDisplayCoordinator_c::updateMapMode() {
    updateStageKind();
    updateStageFlags();
    mMode = detectMode();
    enableRestoreSettings();
    if (mRestoreSettings)
        loadMapSettings();
    if (isMapForced()) {
        if (canUpdatePlayerPosition())
            updatePlayerPositions();
        auto* sea = mpSeaMap.get();
        if (sea)
            seaMapUpdateMode(sea);
        auto* dungeon = mpDungeonMap.get();
        if (dungeon)
            dungeonMapUpdateMode(dungeon);
    } else if (mMode == MODE_SEA) {
        auto* sea = mpSeaMap.get();
        if (sea)
            seaMapUpdateMode(sea);
    } else if (mMode == MODE_DUNGEON) {
        auto* dungeon = mpDungeonMap.get();
        if (dungeon)
            dungeonMapUpdateNormalMode(dungeon);
    }
}

/* 02688AAC */
void hdMapDisplayCoordinator_c::saveMapSettings() {
    if (!sSave)
        return;
    hdSavePlayLog_c* settings = getMapSettings(&sSave->mHolders);
    mapSettingsSetSeaPosition(settings, &mSeaPosition);
    settings = getMapSettings(&sSave->mHolders);
    mapSettingsSetSeaScale(settings, mSeaScale);
    settings = getMapSettings(&sSave->mHolders);
    mapSettingsSetDungeonPosition(settings, &mDungeonPosition);
    settings = getMapSettings(&sSave->mHolders);
    mapSettingsSetDungeonScale(settings, mDungeonScale);
}

/* 02688B38 */
void hdMapDisplayCoordinator_c::saveSelectedMapSettings() {
    saveMapSettings();
    if (mMode == MODE_SEA) {
        auto* map = mpSeaMap.get();
        if (map)
            seaMapSaveSettings(map);
    } else if (mMode == MODE_DUNGEON) {
        auto* map = mpDungeonMap.get();
        if (map)
            dungeonMapSaveSettings(map);
    }
}

/* 02688BBC */
void hdMapDisplayCoordinator_c::updateDungeonMapSelection() {
    if (mMode != MODE_DUNGEON)
        return;
    auto* map = mpDungeonMap.get();
    if (map)
        dungeonMapUpdateSelection(map);
}

/* 02688BD8 */
void hdMapDisplayCoordinator_c::openSelectedMap() {
    u32 mode = detectMode();
    mPreviousMode = mode;
    mMode = mode;
    if (mode == MODE_SEA)
        mapChangeState(&mState.mMachine, &sInAllMenuMapState);
    else if (mode == MODE_DUNGEON)
        mapChangeState(&mState.mMachine, &sInAllDungeonMapState);
}

/* 02688C50 */
void hdMapDisplayCoordinator_c::changeSelectedMap() {
    u32 mode = mMode;
    if (mode == MODE_SEA)
        mapChangeState(&mState.mMachine, &sOutAllMenuMapState);
    else if (mode == MODE_DUNGEON)
        mapChangeState(&mState.mMachine, &sOutAllDungeonMapState);
}

/* 02688C84 */
void hdMapDisplayCoordinator_c::resetMenuMap() {
    auto* map = mpSeaMap.get();
    if (map) {
        seaMapReset(map);
        mVisible = 0;
    }
}

/* 02688CC4 */
void hdMapDisplayCoordinator_c::resetDungeonMap() {
    auto* map = mpDungeonMap.get();
    if (map) {
        dungeonMapReset(map);
        mVisible = 0;
    }
}

/* 02688D04 */
void hdMapDisplayCoordinator_c::resetMaps() {
    resetMenuMap();
    resetDungeonMap();
    mapChangeState(&mState.mMachine, &sBlankState);
}

/* 02688D48 */
void hdMapDisplayCoordinator_c::showMenuMap() {
    auto* map = mpSeaMap.get();
    if (map) {
        seaMapShow(map);
        mOpened = 1;
        mVisible = 1;
        mapChangeState(&mState.mMachine, &sDisplayMenuMapState);
    }
}

/* 02688D9C */
void hdMapDisplayCoordinator_c::showDungeonMap() {
    auto* map = mpDungeonMap.get();
    if (map) {
        dungeonMapShow(map);
        mOpened = 1;
        mVisible = 1;
        mapChangeState(&mState.mMachine, &sDisplayDungeonMapState);
    }
}

/* 02688DF0 */
void hdMapDisplayCoordinator_c::showSelectedMap() {
    resetMaps();
    u32 mode = detectMode();
    mMode = mode;
    mPreviousMode = mode;
    if (mode == MODE_SEA)
        showMenuMap();
    else if (mode == MODE_DUNGEON)
        showDungeonMap();
}

/* 02688E70 */
void hdMapDisplayCoordinator_c::configureSeaMap(u32 origin, u32 mode,
                                                u32 scale) {
    auto* map = mpSeaMap.get();
    mInputMode = mode;
    seaMapSetOrigin(map, origin);
    seaMapSetMode(mpSeaMap, mode);
    seaMapSetScale(mpSeaMap, scale);
    seaMapSetTargetOrigin(mpSeaMap, origin);
    seaMapSetTargetMode(mpSeaMap, mode);
    seaMapSetTargetScale(mpSeaMap, scale);
}

/* 02688F04 */
void hdMapDisplayCoordinator_c::configureDungeonMap(u32 origin, u32 mode,
                                                    u32 scale) {
    auto* map = mpDungeonMap.get();
    mInputMode = mode;
    dungeonMapSetOrigin(map, origin);
    dungeonMapSetMode(mpDungeonMap, mode);
    dungeonMapSetScale(mpDungeonMap, scale);
    dungeonMapSetTargetOrigin(mpDungeonMap, origin);
    dungeonMapSetTargetMode(mpDungeonMap, mode);
    dungeonMapSetTargetScale(mpDungeonMap, scale);
}

/* 02688F98 */
void hdMapDisplayCoordinator_c::setMarkersActive(u32 active) {
    auto* sea = mpSeaMap.get();
    if (sea)
        seaMapSetMarkersActive(sea, active);
    auto* dungeon = mpDungeonMap.get();
    if (dungeon)
        dungeon->mMarkersActive = active;
}

/* 02688FF0 */
void hdMapDisplayCoordinator_c::updateSeaCells() {
    if (mMode != MODE_SEA)
        return;
    HdPadMapOwner* owner = mpSeaMap->mMap;
    seaCellsUpdate(owner->mCells);
}

/* 0268900C */
void hdMapDisplayCoordinator_c::updateSeaMapSelection() {
    if (mMode != MODE_SEA)
        return;
    auto* map = mpSeaMap.get();
    if (map)
        seaMapUpdateSelection(map);
}

/* 02689028 */
void hdMapDisplayCoordinator_c::updateSeaMapCursor() {
    if (mMode != MODE_SEA)
        return;
    auto* map = mpSeaMap.get();
    if (map)
        seaMapUpdateCursor(map);
}

/* 02689044 */
u32 hdMapDisplayCoordinator_c::isSeaMapSelected() {
    if (mMode != MODE_SEA)
        return 0;
    auto* map = mpSeaMap.get();
    return map ? seaMapSelected(map) : 0;
}

/* 02689068 */
u32 hdMapDisplayCoordinator_c::isSeaMapCursorActive() {
    if (mMode != MODE_SEA)
        return 0;
    auto* map = mpSeaMap.get();
    return map ? seaMapCursorActive(map) : 0;
}

/* 0268908C */
void hdMapDisplayCoordinator_c::clearSeaMapSelection() {
    if (mMode != MODE_SEA)
        return;
    auto* map = mpSeaMap.get();
    if (map)
        seaMapClearSelection(map);
}

/* 026890A8 */
u32 hdMapDisplayCoordinator_c::isSeaMapSelectionEmpty() {
    if (mMode != MODE_SEA)
        return 0;
    auto* map = mpSeaMap.get();
    return map ? seaMapSelectionEmpty(map) : 0;
}

/* 026890CC */
void hdMapDisplayCoordinator_c::clearDungeonMapSelection() {
    if (mMode != MODE_DUNGEON)
        return;
    auto* map = mpDungeonMap.get();
    if (map)
        dungeonMapClearSelection(map);
}

/* 026890E8 */
u32 hdMapDisplayCoordinator_c::isLinkProcess() {
    auto* link = mapGetPlay()->mpLink.get();
    s32 process = link ? s32(link->mProcess) : 0x7FFF;
    return process == 0xC3;
}

/* 02689128 */
s32 hdMapDisplayCoordinator_c::getSelectedSeaCell() {
    if (mMode != MODE_SEA)
        return -1;
    auto* sea = mpSeaMap.get();
    if (!sea)
        return -1;
    auto* owner = sea->mMap.get();
    if (!owner)
        return -1;
    auto* cells = reinterpret_cast<hdSelectedSeaCells_l*>(owner->mCells.get());
    if (!cells)
        return -1;
    auto* selected = cells->mpSelected.get();
    return selected ? s32(selected->mId) : -1;
}

/* 02689174 */
u32 hdMapDisplayCoordinator_c::isSavedSeaCell(s32 cell) {
    if (cell < 0 || !sSave)
        return 0;
    hdSavePlayLog_c* settings = getMapSettings(&sSave->mHolders);
    return mapSettingsHasCell(settings, cell);
}

/* 026891C4 */
u32 hdMapDisplayCoordinator_c::isSelectedCellSaved() {
    s32 cell = -1;
    auto* owner = mpSeaMap->mMap.get();
    if (owner) {
        auto* cells =
            reinterpret_cast<hdSelectedSeaCells_l*>(owner->mCells.get());
        if (cells && cells->mpSelected)
            cell = cells->mpSelected->mId;
    }
    return isSavedSeaCell(cell);
}

/* 026891F8 */
u32 hdMapDisplayCoordinator_c::canSaveSelectedCell() {
    if (mMode != MODE_SEA || !mpSeaMap)
        return 0;
    if (sInputActive->mActive) {
        auto* mode = mapGetInputMode(sMapInput);
        if (!mode)
            return 0;
        auto* owner = mpSeaMap->mMap.get();
        if (!owner || !owner->mCells)
            return 0;
        if (!seaCellsAcceptMode(owner->mCells, mode)) {
            void* icons = owner->mIcons;
            if (!icons)
                return 0;
            u32 id = mapIconsGetMode(icons);
            if (mode->mId != id)
                return 0;
        }
    }
    return isSelectedCellSaved();
}

/* 026892E0 */
u32 hdMapDisplayCoordinator_c::isTowerMap() {
    s32 kind = mMapKind;
    return kind == MAP_SAVAGE_LABYRINTH || kind == MAP_SAVAGE_LABYRINTH_LOWER;
}

/* 02689304 */
u32 hdMapDisplayCoordinator_c::isDungeonOrTowerMap() {
    if (isDungeonMapForced())
        return 1;
    return isTowerMap() != 0;
}

/* 02689358 */
u32 hdMapDisplayCoordinator_c::isForsakenFortressMap() {
    return mMapKind == MAP_FORTRESS_INTERIOR;
}

/* 0268936C */
u32 hdMapDisplayCoordinator_c::isDungeonMapRestricted() {
    if (mMode != MODE_DUNGEON)
        return 0;
    if (mSpecialDungeon)
        return 1;
    s32 stage = mStageKind;
    if (stage == 6 || stage == 7)
        return 1;
    if (stage == 0 && mBoss)
        return 1;
    return mMapKind == MAP_ADMUMI;
}

/* 02689D28 */
u32 hdMapDisplayCoordinator_c::isRoomVisited(s32 room) {
    if (room < 0 || u32(mStageKind) >= 6)
        return 0;
    s32 stage = sDungeonStageIds.mIds[u32(mStageKind)];
    if (stage == 2 && !isDungeonStage())
        return mapIsVisitedRoom(&sSave->mSaveInfo.mSavedata.mMemory[2].mMembit,
                                room) != 0;
    return mapIsVisitedRoom(&sSave->mSaveInfo.mMemory.mMembit, room) != 0;
}

/* 02689DDC */
u32 hdMapDisplayCoordinator_c::isTreasureOpened(u32 chest) {
    if (u32(mStageKind) >= 6)
        return 0;
    s32 stage = sDungeonStageIds.mIds[u32(mStageKind)];
    if (stage == 2 && !isDungeonStage())
        return mapIsStageTreasureOpened(2, chest) != 0;
    return mapIsTreasureOpened(&sSave->mSaveInfo.mMemory.mMembit, chest) != 0;
}

/* 02689E7C */
u32 hdMapDisplayCoordinator_c::hasDungeonMap() {
    if (u32(mStageKind) >= 6)
        return 0;
    s32 stage = sDungeonStageIds.mIds[u32(mStageKind)];
    if (stage == 2 && !isDungeonStage())
        return mapStageHasMap(2) != 0;
    return mapHasDungeonItem(&sSave->mSaveInfo.mMemory.mMembit, 0) != 0;
}

/* 02689F30 */
u32 hdMapDisplayCoordinator_c::hasDungeonBossKey() {
    if (u32(mStageKind) >= 6)
        return 0;
    s32 stage = sDungeonStageIds.mIds[u32(mStageKind)];
    if (stage == 2 && !isDungeonStage())
        return mapStageHasBossKey(2) != 0;
    return mapHasDungeonItem(&sSave->mSaveInfo.mMemory.mMembit, 2) != 0;
}

/* 02689FE4 */
u32 hdMapDisplayCoordinator_c::hasDungeonCompass() {
    if (u32(mStageKind) >= 6)
        return 0;
    s32 stage = sDungeonStageIds.mIds[u32(mStageKind)];
    if (stage == 2 && !isDungeonStage())
        return mapStageHasCompass(2) != 0;
    return mapHasDungeonItem(&sSave->mSaveInfo.mMemory.mMembit, 1) != 0;
}

/* 0268A098 */
u32 hdMapDisplayCoordinator_c::isSpecialChartUnlocked(u32 index) {
    if (mStageKind != 0 || index >= 3)
        return 0;
    if (mapEventIsSet(&sSave->mSaveInfo.mSavedata.mEvent, 0x1820))
        return 1;
    if (isSeaMapForced())
        return mapSaveIsSwitch(&sSave->mSaveInfo, sSeaChartSwitches.mIds[index],
                               sSpecialMode) != 0;
    if (isDungeonMapForced())
        return mapSaveIsSwitch(&sSave->mSaveInfo,
                               sDungeonChartSwitches.mIds[index],
                               sSpecialMode) != 0;
    if (isForsakenFortressMap()) {
        s32 stage = sDungeonStageIds.mIds[u32(mStageKind)];
        return mapMemoryIsSwitch(&sSave->mSaveInfo.mSavedata.mMemory[stage],
                                 sSeaChartSwitches.mIds[index]) != 0;
    }
    if (isTowerMap())
        return mapMemoryIsSwitch(sSave->mSaveInfo.mSavedata.mMemory,
                                 sDungeonChartSwitches.mIds[index]) != 0;
    return 0;
}

/* 0268A254 */
void hdMapDisplayCoordinator_c::clearChartProgress(s32 saveNo) {
    s32 index = saveNo - 1;
    if (mapIsOpen(&sSave->mSaveInfo.mSavedata.mPlayer.mMap, index))
        mapClearOpen(&sSave->mSaveInfo.mSavedata.mPlayer.mMap, index);
    if (mapIsComplete(&sSave->mSaveInfo.mSavedata.mPlayer.mMap, index))
        mapClearComplete(&sSave->mSaveInfo.mSavedata.mPlayer.mMap, index);
}

/* 0268A2D8 */
u32 hdMapDisplayCoordinator_c::countObtainedCharts(s32 first, s32 count) {
    u32 obtained = 0;
    if (count > 0)
        for (s32 i = 0; i < count; ++i) {
            auto* entry = findChart(u32(first) + u32(i));
            if (entry) {
                s32 saveNo = entry->mSaveNo;
                if (mapIsObtained(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                                  saveNo - 1))
                    ++obtained;
                else
                    clearChartProgress(saveNo);
            }
        }
    return obtained;
}

/* 0268A37C */
u32 hdMapDisplayCoordinator_c::countObtainedSpecialCharts() {
    return countObtainedCharts(CHART_SPECIAL_FIRST, CHART_SPECIAL_COUNT);
}

/* 0268A388 */
u32 hdMapDisplayCoordinator_c::countObtainedSeaCharts() {
    return countObtainedCharts(CHART_SEA_FIRST, CHART_SEA_COUNT);
}

/* 0268A394 */
u32 hdMapDisplayCoordinator_c::countObtainedOtherCharts() {
    return countObtainedCharts(CHART_OTHER_FIRST, CHART_OTHER_COUNT);
}

/* 0268A3A0 */
u32 hdMapDisplayCoordinator_c::countAllObtainedCharts() {
    u32 special = countObtainedSpecialCharts();
    u32 sea = countObtainedSeaCharts();
    u32 total = special + sea;
    return total + countObtainedOtherCharts();
}

/* 0268A3F0 */
u32 hdMapDisplayCoordinator_c::countCollectedCharts() {
    u32 special = countObtainedSpecialCharts();
    u32 sea = countObtainedSeaCharts();
    u32 total = special + sea;
    mapGetCollectCount(&sSave->mSaveInfo.mSavedata.mPlayer.mMap);
    return total;
}

/* 0268A448 */
u32 hdMapDisplayCoordinator_c::countCompletedCharts(s32 first, s32 count) {
    u32 complete = 0;
    if (count > 0)
        for (s32 i = 0; i < count; ++i) {
            auto* entry = findChart(u32(first) + u32(i));
            if (entry && mapIsComplete(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                                       s32(entry->mSaveNo) - 1))
                ++complete;
        }
    return complete;
}

/* 0268A4D0 */
u32 hdMapDisplayCoordinator_c::countCompletedSpecialCharts() {
    return countCompletedCharts(CHART_SPECIAL_FIRST, CHART_SPECIAL_COUNT);
}

/* 0268A4DC */
u32 hdMapDisplayCoordinator_c::countCompletedSeaCharts() {
    return countCompletedCharts(CHART_SEA_FIRST, CHART_SEA_COUNT);
}

/* 0268A4E8 */
u32 hdMapDisplayCoordinator_c::countAllCompletedCharts() {
    u32 special = countCompletedSpecialCharts();
    u32 sea = countCompletedSeaCharts();
    return special + sea;
}

/* 0268A52C */
void hdMapDisplayCoordinator_c::obtainChart(u32 chart) {
    auto* entry = findChart(chart);
    if (entry)
        mapSetObtained(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                       s32(entry->mSaveNo) - 1);
}

/* 0268A570 */
u32 hdMapDisplayCoordinator_c::isChartObtained(u32 chart) {
    if (chart >= CHART_TOTAL)
        return 0;
    auto* entry = findChart(chart);
    if (!entry)
        return 0;
    return mapIsObtained(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                         s32(entry->mSaveNo) - 1) != 0;
}

/* 0268A5DC */
u32 hdMapDisplayCoordinator_c::isChartOpened(u32 chart) {
    if (chart >= CHART_TOTAL)
        return 0;
    auto* entry = findChart(chart);
    if (!entry)
        return 0;
    return mapIsOpen(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                     s32(entry->mSaveNo) - 1) != 0;
}

/* 0268A648 */
u32 hdMapDisplayCoordinator_c::isChartComplete(u32 chart) {
    if (chart >= CHART_OTHER_FIRST)
        return 0;
    auto* entry = findChart(chart);
    if (!entry)
        return 0;
    return mapIsComplete(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                         s32(entry->mSaveNo) - 1) != 0;
}

/* 0268A6B4 */
u32 hdMapDisplayCoordinator_c::isTriforceChart(u32 chart) {
    if (chart >= CHART_SPECIAL_COUNT)
        return 0;
    auto* entry = findChart(chart);
    if (!entry)
        return 0;
    return mapIsTriforce(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                         s32(entry->mSaveNo) - 1) != 0;
}

/* 0268A720 */
hdChartEntry_c* hdMapDisplayCoordinator_c::findSeaChart(u32 chart) {
    if (!mpCharts)
        return NULL;
    s32 count = mChartCount;
    if (count <= 0)
        return NULL;
    u32 wanted = chart + 1;
    for (s32 i = 0; i < count; ++i) {
        hdChartEntry_c* entry = getChart(i);
        if (entry && u32(s32(entry->mSeaNo)) == wanted)
            return entry;
    }
    return NULL;
}

/* 0268A7A0 */
u32 hdMapDisplayCoordinator_c::isSeaChartComplete(u32 chart) {
    if (chart >= CHART_TOTAL)
        return 0;
    auto* entry = findSeaChart(chart);
    if (!entry)
        return 0;
    return mapIsComplete(&sSave->mSaveInfo.mSavedata.mPlayer.mMap,
                         s32(entry->mSaveNo) - 1) != 0;
}

/* 0268A80C */
void hdMapDisplayCoordinator_c::saveLocationSelection(s32 location) {
    if (location < 0 || !sSave)
        return;
    hdSavePlayLog_c* settings = getMapSettings(&sSave->mHolders);
    mapSettingsSetLocation(settings, location);
}

/* 0268A858 */
void hdMapDisplayCoordinator_c::saveCurrentLocationSelection() {
    saveLocationSelection(mSelectedEntry);
}

/* 0268A864 */
void hdMapDisplayCoordinator_c::getLocationInfo(s32 index, be<u8>* x, be<u8>* y,
                                                be<s16>* room, be<s16>* layer,
                                                be<u8>* kind) {
    auto* location = getLocation(index);
    if (!location)
        return;
    if (x)
        *x = location->mX;
    if (y)
        *y = location->mY;
    if (room)
        *room = location->mRoom;
    if (layer)
        *layer = location->mLayer;
    if (kind)
        *kind = location->mKind;
}

/* 0268A8DC */
void hdMapDisplayCoordinator_c::setInstance(hdMapDisplayCoordinator_c* instance,
                                            u32 index) {
    if (index < 3)
        sInstances.mItems[index] = instance;
}

/* 0268A920 */
hdMapDisplayCoordinator_c* hdMapDisplayCoordinator_c::getInstance(u32 index) {
    return index < 3 ? sInstances.mItems[index].get() : NULL;
}

/* 0268A96C */
static void initializeMapDisplayStatics() {
    sMapInitializerWords.mValues[3] = 0;
    sMapInitializerWords.mValues[2] = 0;
    sMapInitializerWords.mValues[1] = 0;
    sMapInitializerWords.mValues[0] = 0;
    mapRegisterGlobal(&sMapRegistration);
    sMapAngleMin = -3.1415927410125732f;
    sMapAngleMax = 3.1415927410125732f;
    mapInitializeAngle(&sMapAngleGuard);
    mapRegisterGlobal(&sMapAngleRegistration);
    mapInitializeLimits(&sMapLimitGuard);
    mapRegisterGlobal(&sMapLimitRegistration);
    u32 guard = sStateIdGuard;
    sPlayerLimitX = 10000.0f;
    sMapHalfWidth = 50000.0f;
    sMapHalfHeight = 50000.0f;
    sPlayerLimitY = 10000.0f;
    u32 counter = 0;
    if (guard)
        counter = sStateIdCounter;
    else
        sStateIdGuard = 1;
    sBlankState.mId = ++counter;
    sBlankState.setName("::StateID_Blank");
    sBlankState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sBlankState.mEnter.mAdjust = 0;
    sBlankState.mEnter.mVirtualIndex = -1;
    sBlankState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapBlankEnter);
    sBlankState.mExecute.mAdjust = 0;
    sBlankState.mExecute.mVirtualIndex = -1;
    sBlankState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapBlankExecute);
    sBlankState.mLeave.mAdjust = 0;
    sBlankState.mLeave.mVirtualIndex = -1;
    sBlankState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapBlankLeave);
    sBlankState.mGetState.mAdjust = 0;
    sBlankState.mGetState.mVirtualIndex = -1;
    sBlankState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapBlankGetState);
    sDisplayMenuMapState.mId = ++counter;
    sDisplayMenuMapState.setName("::StateID_DisplayMenuMap");
    sDisplayMenuMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sDisplayMenuMapState.mEnter.mAdjust = 0;
    sDisplayMenuMapState.mEnter.mVirtualIndex = -1;
    sDisplayMenuMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayMenuEnter);
    sDisplayMenuMapState.mExecute.mAdjust = 0;
    sDisplayMenuMapState.mExecute.mVirtualIndex = -1;
    sDisplayMenuMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayMenuExecute);
    sDisplayMenuMapState.mLeave.mAdjust = 0;
    sDisplayMenuMapState.mLeave.mVirtualIndex = -1;
    sDisplayMenuMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayMenuLeave);
    sDisplayMenuMapState.mGetState.mAdjust = 0;
    sDisplayMenuMapState.mGetState.mVirtualIndex = -1;
    sDisplayMenuMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayMenuGetState);
    sDisplayDungeonMapState.mId = ++counter;
    sDisplayDungeonMapState.setName("::StateID_DisplayDungeonMap");
    sDisplayDungeonMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sDisplayDungeonMapState.mEnter.mAdjust = 0;
    sDisplayDungeonMapState.mEnter.mVirtualIndex = -1;
    sDisplayDungeonMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayDungeonEnter);
    sDisplayDungeonMapState.mExecute.mAdjust = 0;
    sDisplayDungeonMapState.mExecute.mVirtualIndex = -1;
    sDisplayDungeonMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayDungeonExecute);
    sDisplayDungeonMapState.mLeave.mAdjust = 0;
    sDisplayDungeonMapState.mLeave.mVirtualIndex = -1;
    sDisplayDungeonMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayDungeonLeave);
    sDisplayDungeonMapState.mGetState.mAdjust = 0;
    sDisplayDungeonMapState.mGetState.mVirtualIndex = -1;
    sDisplayDungeonMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapDisplayDungeonGetState);
    sInAllMenuMapState.mId = ++counter;
    sInAllMenuMapState.setName("::StateID_InAllMenuMap");
    sInAllMenuMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sInAllMenuMapState.mEnter.mAdjust = 0;
    sInAllMenuMapState.mEnter.mVirtualIndex = -1;
    sInAllMenuMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllMenuEnter);
    sInAllMenuMapState.mExecute.mAdjust = 0;
    sInAllMenuMapState.mExecute.mVirtualIndex = -1;
    sInAllMenuMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllMenuExecute);
    sInAllMenuMapState.mLeave.mAdjust = 0;
    sInAllMenuMapState.mLeave.mVirtualIndex = -1;
    sInAllMenuMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllMenuLeave);
    sInAllMenuMapState.mGetState.mAdjust = 0;
    sInAllMenuMapState.mGetState.mVirtualIndex = -1;
    sInAllMenuMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllMenuGetState);
    sInAllDungeonMapState.mId = ++counter;
    sInAllDungeonMapState.setName("::StateID_InAllDungeonMap");
    sInAllDungeonMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sInAllDungeonMapState.mEnter.mAdjust = 0;
    sInAllDungeonMapState.mEnter.mVirtualIndex = -1;
    sInAllDungeonMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllDungeonEnter);
    sInAllDungeonMapState.mExecute.mAdjust = 0;
    sInAllDungeonMapState.mExecute.mVirtualIndex = -1;
    sInAllDungeonMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllDungeonExecute);
    sInAllDungeonMapState.mLeave.mAdjust = 0;
    sInAllDungeonMapState.mLeave.mVirtualIndex = -1;
    sInAllDungeonMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllDungeonLeave);
    sInAllDungeonMapState.mGetState.mAdjust = 0;
    sInAllDungeonMapState.mGetState.mVirtualIndex = -1;
    sInAllDungeonMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapInAllDungeonGetState);
    sOutAllMenuMapState.mId = ++counter;
    sOutAllMenuMapState.setName("::StateID_OutAllMenuMap");
    sOutAllMenuMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sOutAllMenuMapState.mEnter.mAdjust = 0;
    sOutAllMenuMapState.mEnter.mVirtualIndex = -1;
    sOutAllMenuMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllMenuEnter);
    sOutAllMenuMapState.mExecute.mAdjust = 0;
    sOutAllMenuMapState.mExecute.mVirtualIndex = -1;
    sOutAllMenuMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllMenuExecute);
    sOutAllMenuMapState.mLeave.mAdjust = 0;
    sOutAllMenuMapState.mLeave.mVirtualIndex = -1;
    sOutAllMenuMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllMenuLeave);
    sOutAllMenuMapState.mGetState.mAdjust = 0;
    sOutAllMenuMapState.mGetState.mVirtualIndex = -1;
    sOutAllMenuMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllMenuGetState);
    sOutAllDungeonMapState.mId = ++counter;
    sOutAllDungeonMapState.setName("::StateID_OutAllDungeonMap");
    sOutAllDungeonMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sOutAllDungeonMapState.mEnter.mAdjust = 0;
    sOutAllDungeonMapState.mEnter.mVirtualIndex = -1;
    sOutAllDungeonMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllDungeonEnter);
    sOutAllDungeonMapState.mExecute.mAdjust = 0;
    sOutAllDungeonMapState.mExecute.mVirtualIndex = -1;
    sOutAllDungeonMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllDungeonExecute);
    sOutAllDungeonMapState.mLeave.mAdjust = 0;
    sOutAllDungeonMapState.mLeave.mVirtualIndex = -1;
    sOutAllDungeonMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllDungeonLeave);
    sOutAllDungeonMapState.mGetState.mAdjust = 0;
    sOutAllDungeonMapState.mGetState.mVirtualIndex = -1;
    sOutAllDungeonMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapOutAllDungeonGetState);
    sChangeToMenuMapState.mId = ++counter;
    sChangeToMenuMapState.setName("::StateID_ChangeToMenuMap");
    sChangeToMenuMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sChangeToMenuMapState.mEnter.mAdjust = 0;
    sChangeToMenuMapState.mEnter.mVirtualIndex = -1;
    sChangeToMenuMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToMenuEnter);
    sChangeToMenuMapState.mExecute.mAdjust = 0;
    sChangeToMenuMapState.mExecute.mVirtualIndex = -1;
    sChangeToMenuMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToMenuExecute);
    sChangeToMenuMapState.mLeave.mAdjust = 0;
    sChangeToMenuMapState.mLeave.mVirtualIndex = -1;
    sChangeToMenuMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToMenuLeave);
    sChangeToMenuMapState.mGetState.mAdjust = 0;
    sChangeToMenuMapState.mGetState.mVirtualIndex = -1;
    sChangeToMenuMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToMenuGetState);
    sStateIdCounter = counter + 1;
    sChangeToDungeonMapState.mId = ++counter;
    sChangeToDungeonMapState.setName("::StateID_ChangeToDungeonMap");
    sChangeToDungeonMapState.__vtbl = gabi::ea(&sMapDescriptorVTable);
    sChangeToDungeonMapState.mEnter.mAdjust = 0;
    sChangeToDungeonMapState.mEnter.mVirtualIndex = -1;
    sChangeToDungeonMapState.mEnter.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToDungeonEnter);
    sChangeToDungeonMapState.mExecute.mAdjust = 0;
    sChangeToDungeonMapState.mExecute.mVirtualIndex = -1;
    sChangeToDungeonMapState.mExecute.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToDungeonExecute);
    sChangeToDungeonMapState.mLeave.mAdjust = 0;
    sChangeToDungeonMapState.mLeave.mVirtualIndex = -1;
    sChangeToDungeonMapState.mLeave.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToDungeonLeave);
    sChangeToDungeonMapState.mGetState.mAdjust = 0;
    sChangeToDungeonMapState.mGetState.mVirtualIndex = -1;
    sChangeToDungeonMapState.mGetState.mFunction.v =
        gabi::guest_fn((const void*)&mapChangeToDungeonGetState);
}

/* WWHD glue
data sMapInput 101F4FF0
data sSpecialMode 1047E6C8
data sSave 101F84DC
data sInstances 101F62FC
data sBlankState 104963D8
data sDisplayMenuMapState 10496404
data sDisplayDungeonMapState 10496430
data sInAllMenuMapState 1049645C
data sInAllDungeonMapState 10496488
data sOutAllMenuMapState 104964B4
data sOutAllDungeonMapState 104964E0
data sMenuMapDisplayState 1049751C
data sMenuMapBlankState 1049754C
data sDungeonMapDisplayState 10492410
data sDungeonMapBlankState 10492440
data sDungeonMapWaitState 104925C0
data sLinkPosDisplayState 10496200
data sLinkPosMoveState 10496260
data sLinkPosWaitState 10496290
data sInputActive 101F8378
data sDungeonStageIds 100F98C0
data sCoordinatorBaseVTable 100F9860
data sCoordinatorOwnerVTable 100F98A0
data sCoordinatorStateVTable 100F9850
data sCoordinatorVTable 100F9B4C
data sNoStateId_l 101FF32C
data sChangeToMenuMapState 1049650C
data sChangeToDungeonMapState 10496538
data sUiBlockedState 1049F3BC
data sUiController 101F8344
data sMapSafeStringVTable 100F9838
str 100F9998 "sea"
str 100F99A4 "Obshop"
str 100F99AC "Abship"
str 100F999C "PShip"
str 100F99B4 "MajyuE"
str 100F9980 "majroom"
str 100F9988 "ma2room"
str 100F9990 "ma3room"
str 100F99BC "ADMumi"
data sSeaChartSwitches 100F98D4
data sDungeonChartSwitches 100F98E0
data sPlayerLimitX 104963BC
data sPlayerLimitY 104963C0
data sPlayerMargin 1047C540
data sChartHeap 1018C404
data sMapLocations 1047A874
data sMapChartArchive 1047A878
data sMapHeapManager 101F95D0
data sDefaultPrimaryContext 104A20FC
data sDefaultSecondaryContext 104A2098
str 100F9A5C "::StateID_Blank"
str 100F9AD8 "::StateID_DisplayMenuMap"
str 100F9A84 "::StateID_DisplayDungeonMap"
str 100F9AF4 "::StateID_InAllMenuMap"
str 100F9AA0 "::StateID_InAllDungeonMap"
str 100F9A6C "::StateID_OutAllMenuMap"
str 100F9B0C "::StateID_OutAllDungeonMap"
str 100F9ABC "::StateID_ChangeToMenuMap"
str 100F9B28 "::StateID_ChangeToDungeonMap"
data sMapInitializerWords 104963C8
data sMapRegistration 101F62D8
data sMapAngleRegistration 101F62E4
data sMapLimitRegistration 101F62F0
data sMapAngleMin 104963AC
data sMapAngleMax 104963B0
data sMapHalfWidth 104963B4
data sMapHalfHeight 104963B8
data sMapAngleGuard 104963C4
data sMapLimitGuard 104963C5
data sStateIdGuard 101FD8E4
data sStateIdCounter 101FDD50
data sMapDescriptorVTable 100F9934
data sStageKinds 100F99C4
ext 0269D9CC void seaMapOpen(HdPadMenuMap*)
ext 0269D9DC void seaMapClose(HdPadMenuMap*)
ext 0269DA68 void seaMapReset(HdPadMenuMap*)
ext 0265D19C void dungeonMapOpen(HdPadDungeon*)
ext 0265D1AC void dungeonMapClose(HdPadDungeon*)
ext 0265D4F4 void dungeonMapReset(HdPadDungeon*)
ext 02615154 void mapSetInputMode(void*, u32)
ext 0269E258 void seaMapUpdateSelection(HdPadMenuMap*)
ext 0269DC7C void seaMapUpdateCursor(HdPadMenuMap*)
ext 0269E2A8 void seaMapClearSelection(HdPadMenuMap*)
ext 0269DB4C u32 seaMapSelected(HdPadMenuMap*)
ext 0269E178 u32 seaMapCursorActive(HdPadMenuMap*)
ext 0269E2B8 u32 seaMapSelectionEmpty(HdPadMenuMap*)
ext 0265D974 void dungeonMapUpdateSelection(HdPadDungeon*)
ext 0265DA04 void dungeonMapClearSelection(HdPadDungeon*)
ext 025B80C8 u32 mapIsOpen(void*, s32)
ext 025B8054 void mapClearOpen(void*, s32)
ext 025B8228 u32 mapIsComplete(void*, s32)
ext 025B81B4 void mapClearComplete(void*, s32)
ext 025B7F68 u32 mapIsObtained(void*, s32)
ext 025B7FE0 void mapSetObtained(void*, s32)
ext 025B8308 u32 mapIsTriforce(void*, s32)
ext 025B8378 u32 mapGetCollectCount(void*)
ext 0265DCFC void dungeonMapUpdateCoordinate(HdPadDungeon*)
ext 0269E2C0 void seaMapSetMarker(HdPadMenuMap*, u32)
ext 0269DADC void seaMapSetMarkersActive(HdPadMenuMap*, u32)
ext 02683808 void seaCellsUpdate(HdPadCellArray*)
ext 0272019C hdSavePlayLog_c* getMapSettings(hdSaveHolders_c*)
ext 027208EC u32 mapSettingsHasCell(hdSavePlayLog_c*, s32)
ext 027208C0 void mapSettingsSetLocation(hdSavePlayLog_c*, s32)
ext 02006478 hdMapStateDescriptor_l* mapGetCurrentState(HdPadStateController*)
ext 020063C0 void mapChangeState(hdStateMachine*, hdStateId*)
ext 0269D9EC void seaMapShow(HdPadMenuMap*)
ext 0265D468 void dungeonMapShow(HdPadDungeon*)
ext 0269DD0C void seaMapSetOrigin(HdPadMenuMap*, u32)
ext 0269DD94 void seaMapSetMode(HdPadMenuMap*, u32)
ext 0269DE50 void seaMapSetScale(HdPadMenuMap*, u32)
ext 0269DF10 void seaMapSetTargetOrigin(HdPadMenuMap*, u32)
ext 0269DFB4 void seaMapSetTargetMode(HdPadMenuMap*, u32)
ext 0269E0C0 void seaMapSetTargetScale(HdPadMenuMap*, u32)
ext 02656630 void dungeonMapSetOrigin(HdPadDungeon*, u32)
ext 026566C0 void dungeonMapSetMode(HdPadDungeon*, u32)
ext 0265DA1C void dungeonMapSetScale(HdPadDungeon*, u32)
ext 0265DA80 void dungeonMapSetTargetOrigin(HdPadDungeon*, u32)
ext 0265DB48 void dungeonMapSetTargetMode(HdPadDungeon*, u32)
ext 0265DC08 void dungeonMapSetTargetScale(HdPadDungeon*, u32)
ext 0272084C void mapSettingsSetSeaPosition(hdSavePlayLog_c*, HdPadPoint*)
ext 02720874 void mapSettingsSetDungeonPosition(hdSavePlayLog_c*, HdPadPoint*)
ext 02720864 void mapSettingsSetSeaScale(hdSavePlayLog_c*, f32)
ext 02720890 void mapSettingsSetDungeonScale(hdSavePlayLog_c*, f32)
ext 02720860 HdPadPoint* mapSettingsGetSeaPosition(hdSavePlayLog_c*)
ext 02720888 HdPadPoint* mapSettingsGetDungeonPosition(hdSavePlayLog_c*)
ext 0272086C f32 mapSettingsGetSeaScale(hdSavePlayLog_c*)
ext 02720898 f32 mapSettingsGetDungeonScale(hdSavePlayLog_c*)
ext 0269DD08 void seaMapSaveSettings(HdPadMenuMap*)
ext 0265D774 void dungeonMapSaveSettings(HdPadDungeon*)
ext 0269DB08 void seaMapUpdateMode(HdPadMenuMap*)
ext 0265D7C4 void dungeonMapUpdateMode(HdPadDungeon*)
ext 0265D5CC void dungeonMapUpdateNormalMode(HdPadDungeon*)
ext 025200D4 hdMapPlay_l* mapGetPlay()
ext 020063C0 void hideLinkMarker(HdPadStateController*, hdMapStateDescriptor_l*)
ext 0265D578 void dungeonMapFinishClose(HdPadDungeon*)
ext 02615140 HdPadInputMode* mapGetInputMode(void*)
ext 02682C30 u32 seaCellsAcceptMode(HdPadCellArray*, HdPadInputMode*)
ext 026F5B74 u32 mapIconsGetMode(void*)
ext 025B9020 u32 mapIsVisitedRoom(void*, s32)
ext 02520864 u32 mapIsStageTreasureOpened(u32, u32)
ext 025B8C74 u32 mapIsTreasureOpened(void*, u32)
ext 025B9100 u32 mapHasDungeonItem(void*, u32)
ext 025208F8 u32 mapStageHasMap(u32)
ext 02520A00 u32 mapStageHasBossKey(u32)
ext 0252097C u32 mapStageHasCompass(u32)
ext 0273AD10 void* mapAllocate(u32)
ext 0273AF40 void mapDelete(void*)
ext 020062E8 void mapConstructState(hdStateMachine*, hdStateOwner*, hdStateId*)
ext 0268B1BC void detachSeaMap(hdMapDisplayCoordinator_c*, HdPadMenuMap*)
ext 0268B214 void detachDungeonMap(hdMapDisplayCoordinator_c*, HdPadDungeon*)
ext 0273B5DC void freeChartArray(be<s32>*)
ext 02715178 u32 mapUiCanUpdate()
ext 025986BC u32 mapUiTransitionBusy()
ext 02006364 void mapExecuteState(hdStateMachine*)
ext 0259058C void mapPositionToCoordinate(be<s8>*, be<s8>*, f32, f32)
ext 025C1BA0 u32 mapStageGetX(void*)
ext 025C1BB8 u32 mapStageGetY(void*)
ext 0268AE6C void mapSafeStringTouch(hdMapSafeString_l*)
ext 025B8B94 u32 mapEventIsSet(void*, u32)
ext 025BA0C0 u32 mapSaveIsSwitch(void*, s32, s32)
ext 025B8DC8 u32 mapMemoryIsSwitch(void*, s32)
ext 0259058C void mapWorldToSector(be<s8>*, be<s8>*, f32, f32)
ext 02680DE0 hdMapCellCenter_l* mapFindCellCenter(void*, HdPadCoordinate*)
ext 0273B584 void allocateChartArray(be<s32>*, s32, void*, s32)
ext 0273B050 hdChartEntry_c* allocateChartEntry(u32, void*, s32)
ext 0273B6AC void sortChartArray(be<s32>*, hdChartCompareCallback)
ext 0268AEE4 s32 compareChartEntries(hdChartEntry_c**, hdChartEntry_c**) callback
ext 0268B0CC void attachSeaMap(hdMapDisplayCoordinator_c*, gptr<HdPadMenuMap>*, void*, void*)
ext 0268B144 void attachDungeonMap(hdMapDisplayCoordinator_c*, gptr<HdPadDungeon>*, void*, void*)
ext 0268AE70 void mapBlankEnter(hdMapDisplayCoordinator_c*) callback
ext 0268AE74 void mapBlankExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AE78 void mapBlankLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AE7C void mapBlankGetState(hdMapDisplayCoordinator_c*) callback
ext 0268AE84 void mapDisplayMenuEnter(hdMapDisplayCoordinator_c*) callback
ext 02685C84 void mapDisplayMenuExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AE88 void mapDisplayMenuLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AE8C void mapDisplayMenuGetState(hdMapDisplayCoordinator_c*) callback
ext 0268AE94 void mapDisplayDungeonEnter(hdMapDisplayCoordinator_c*) callback
ext 02685CA0 void mapDisplayDungeonExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AE98 void mapDisplayDungeonLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AE9C void mapDisplayDungeonGetState(hdMapDisplayCoordinator_c*) callback
ext 02685CBC void mapInAllMenuEnter(hdMapDisplayCoordinator_c*) callback
ext 02685D00 void mapInAllMenuExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AEA4 void mapInAllMenuLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AEA8 void mapInAllMenuGetState(hdMapDisplayCoordinator_c*) callback
ext 02685DB0 void mapInAllDungeonEnter(hdMapDisplayCoordinator_c*) callback
ext 02685DF4 void mapInAllDungeonExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AEB0 void mapInAllDungeonLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AEB4 void mapInAllDungeonGetState(hdMapDisplayCoordinator_c*) callback
ext 02685EF0 void mapOutAllMenuEnter(hdMapDisplayCoordinator_c*) callback
ext 02685F30 void mapOutAllMenuExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AEBC void mapOutAllMenuLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AEC0 void mapOutAllMenuGetState(hdMapDisplayCoordinator_c*) callback
ext 02685FE0 void mapOutAllDungeonEnter(hdMapDisplayCoordinator_c*) callback
ext 02686020 void mapOutAllDungeonExecute(hdMapDisplayCoordinator_c*) callback
ext 0268AEC8 void mapOutAllDungeonLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AECC void mapOutAllDungeonGetState(hdMapDisplayCoordinator_c*) callback
ext 026860E8 void mapChangeToMenuEnter(hdMapDisplayCoordinator_c*) callback
ext 0268626C void mapChangeToMenuExecute(hdMapDisplayCoordinator_c*) callback
ext 026863C8 void mapChangeToMenuLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AED4 void mapChangeToMenuGetState(hdMapDisplayCoordinator_c*) callback
ext 026863D4 void mapChangeToDungeonEnter(hdMapDisplayCoordinator_c*) callback
ext 02686574 void mapChangeToDungeonExecute(hdMapDisplayCoordinator_c*) callback
ext 026866C0 void mapChangeToDungeonLeave(hdMapDisplayCoordinator_c*) callback
ext 0268AEDC void mapChangeToDungeonGetState(hdMapDisplayCoordinator_c*) callback
ext 028F026C void mapRegisterGlobal(void*)
ext 028ED6F8 void mapInitializeAngle(void*)
ext 028EAB2C void mapInitializeLimits(void*)
*/
