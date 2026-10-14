/* HD WarpMap_00: sea-chart activation and area layout transitions.
 * */
#include "d/hd_screen_warp_map.h"
static u32 sScreenWarpMapRegistration;
static u32 sScreenWarpMapSecondRegistration;
static u32 sScreenWarpMapThirdRegistration;
static hdUiStringTable sMapStringVtable;
static hdPartsStateIdVtbl_c sMapStateVtable;
static u32 sMapHolderVtable;
static u32 sMapSecondaryVtable;
static hdWarpMapManagerPointer_l sInputManager;
static f32 sMapAnimationStart = 1.0f;
static hdPartsStateId_c sMapDisplay;
static hdPartsStateId_c sMapOut;
static hdPartsStateId_c sMapDecideWait;
static hdPartsStateId_c sMapConfirmationState;
static hdPartsStateId_c sMapAreaNameDisplay;
static hdWarpMapTextOwnerPointer_l sMapTextOwner;
static u32 sMapVtable;
static hdUiSafeString sMapAreaNamePane;
static u32 sMapAreaNameCapacity = 32;
static u16 sMapBlankText[2] = {0x20, 0};
static hdWarpMapDisplayControl_l sMapDisplayControl;
static hdWarpMapStatusPointer_l sMapStatus;
static hdUiSafeString sMapArchives[6] = {};
static hdUiSafeString sMapScreenName;
static hdUiSafeString sMapAnimationNames[4] = {};
static hdUiSafeString sMapAnimationPanes[2] = {};
static u32 sMapAnimationPaneIndices[4] = {0, 1, 1, 0};
static hdWarpMapUiSystemPointer_l sMapUiSystem;
enum { WARP_MAP_DISPLAY_MODE = 23, WARP_MAP_DISPLAY_SUBMODE = 7, WARP_CONFIRM_SOUND = 0x8CD };

/* 026EF114 */
static u32 hdWarpMap_stateKey(hdWarpAreaStateKey_l* state) { return state->mKey; }

/* 026EF214 */
void hdWarpMap_c::destroy(hdWarpMap_c* map, u32 flags) {
    if (!map) return;
    map->mStateHolder.mHolderVtable.v = gabi::ea(&sMapHolderVtable);
    map->mSecondaryVtable.v = gabi::ea(&sMapSecondaryVtable);
    hdWarpMap_destroyBase(map, 0);
    if (flags & 1) operator_delete(map);
}

/* 026EF280 */
void hdWarpMap_c::refreshSeaChart() {
    hdSeaChartMenu_c* chart = mpSeaChart;
    if (!chart) return;
    void* details = chart->mpMap.get();
    if (!details) return;
    hdWarpMap_prepareDetails(details);
    hdWarpMap_refreshDetails(details);
    hdWarpMap_setInputMode(sInputManager.mpManager.get(), 0);
}

/* 026EF2DC */
void hdWarpMap_c::initializeWarpAreas() {
    hdSeaChartMenu_c* chart = mpSeaChart;
    mSelectedArea = -1;
    if (chart) {
        if (chart->mpMap) {
            hdWarpMap_resetDetails(reinterpret_cast<hdSeaChartGrid_c*>(chart->mpMap.get()));
            chart = mpSeaChart;
        }
        chart->updateOpenMap();
        mpSeaChart->mMapInputActive = 1;
    }
    refreshSeaChart();
    for (u32 index = 1; index < 10; ++index) {
        hdParts_c* area = *mChildren.safeSlot(index);
        static_cast<hdWarpArea_c*>(area)->hideChange();
    }
}

/* 026EF754 */
void hdWarpMap_c::startOut() {
    hdWarpMap_startAnimation(mpLayout->animSet(), 1, 1, sMapAnimationStart);
}
/* 026EF770 */
void hdWarpMap_c::calculateOut() { gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this); }
/* 026EF780 */
void hdWarpMap_c::startAreaNameOut() {
    hdWarpMap_startAnimation(mpLayout->animSet(), 2, 1, sMapAnimationStart);
}
/* 026EF79C */
void hdWarpMap_c::calculateAreaNameOut() { gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this); }
/* 026F0024 */
void hdWarpMap_c::executeStateMachine() { hdWarpMap_executeMachine(&mStateHolder.mMachine); }

/* 026EF4BC */
static void hdWarpMap_updateText(void* owner, void* panel, void* text) {
    if (panel) hdWarpMap_setText(panel, text);
}

/* 026EF424 */
void hdWarpMap_c::executeIn() {
    if (hdWarpMap_animationDone(mpLayout->animSet(), 0)) hdWarpMap_changeState(&mStateHolder.mMachine, &sMapDisplay);
    if (mpSeaChart) mpSeaChart->openMap();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    if (mpSeaChart) mpSeaChart->closeMap();
}

/* 026EF7AC */
void hdWarpMap_c::startAreaNameIn() {
    hdWarpMap_startAnimation(mpLayout->animSet(), 3, 0, sMapAnimationStart);
    hdWarpMap_setInputMode(sInputManager.mpManager.get(), 0);
}

/* 026EF7F4 */
void hdWarpMap_c::executeAreaNameIn() {
    if (hdWarpMap_animationDone(mpLayout->animSet(), 3)) hdWarpMap_changeState(&mStateHolder.mMachine, &sMapAreaNameDisplay);
    if (mpSeaChart) mpSeaChart->openMap();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    if (mpSeaChart) mpSeaChart->closeMap();
}

/* 026EF4CC */
void hdWarpMap_c::updateAreaName(s32 area, u32 visible) {
    if (visible) {
        void* source = sMapTextOwner.mpOwner->mpTextSource;
        if (source) {
            void* text = hdWarpMap_getAreaText(source);
            hdWarpMap_updateText(this, mpAreaName.get(), text);
        }
        hdPartsPaneSRT_c* pane = mpAreaName;
        pane->mFlags = (u8(pane->mFlags) & 0xFE) + 1;
    } else {
        hdPartsPaneSRT_c* pane = mpAreaName;
        pane->mFlags = u8(pane->mFlags) & 0xFE;
    }
}

/* 026EF574 */
void hdWarpMap_c::selectArea(s32 area, u32 visible) {
    if (area < 0 || u32(mSelectedArea) == u32(area)) return;
    s32 previous = mSelectedArea;
    if (previous >= 0) {
        s32 index = hdSeaChartMenu_c::findMapResource(previous);
        if (index >= 0) static_cast<hdWarpArea_c*>((*mChildren.safeSlot(u32(index) + 1)).get())->hideChange();
    }
    mSelectedArea = area;
    s32 index = hdSeaChartMenu_c::findMapResource(area);
    if (index >= 0) static_cast<hdWarpArea_c*>((*mChildren.safeSlot(u32(index) + 1)).get())->showChange();
    updateAreaName(mSelectedArea, visible);
}

/* 026EFCA0 */
hdWarpMap_c* hdWarpMap_c::construct(hdWarpMap_c* map) {
    if (!map) map = static_cast<hdWarpMap_c*>(operator_new(sizeof(hdWarpMap_c)));
    if (map) {
        hdWarpMap_constructBase(map);
        map->mVtable.v = gabi::ea(&sMapVtable);
        map->mSecondaryVtable.v = gabi::ea(&sMapSecondaryVtable);
        map->mStateHolder.mHolderVtable.v = gabi::ea(&sMapHolderVtable);
        // GHS's embedded pointer constructor checks the wrapped guest address.
        if (gabi::ea(map) + u32(offsetof(hdWarpMap_c, mpAreaName)) == 0) operator_new(4);
        map->mpSeaChart = nullptr;
        map->mConfirmed = 0;
        map->mSelectedArea = 0;
    }
    return map;
}

/* 026EFE0C */
void hdWarpMap_c::bindSeaChart() {
    hdParts_c* child = *mChildren.slot(0);
    mpSeaChart.v = gabi::ea(child);
    if (child) {
        mpSeaChart->setSelectionMode(1);
        mpSeaChart->bindActiveButton();
    }
}

/* 026EFFD0 */
void hdWarpMap_c::release() {
    gabi::call_ptr<void>(mVtable->mDeleteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    hdWarpMap_releaseBase(this);
}

/* 026EFD2C */
void hdWarpMap_c::bindAreaName() {
    hdPartsLyt_c* layout = mpLayout->lyt();
    sMapAreaNamePane.assureTermination();
    hdPartsPaneSRT_c* root = static_cast<hdPartsPaneSRT_c*>(layout->mpRootPane.get());
    hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
    mpAreaName = gabi::call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, sMapAreaNamePane.mText.get(), 1);
}

/* 026EFDA8 */
void hdWarpMap_c::initializeAreaName() {
    if (!mpAreaName) return;
    hdWarpMap_setTextCapacity(mpAreaName.get(), sMapAreaNameCapacity);
    hdPartsPaneSRT_c* pane = mpAreaName;
    hdWarpMapTextPaneVtbl_l* table = static_cast<hdWarpMapTextPaneVtbl_l*>(pane->mVtable.get());
    gabi::call_ptr<void>(table->mSetText.get(), pane, sMapBlankText, 0);
}

/* 026EFE60 */
void hdWarpMap_c::initializeLayout() {
    bindAreaName();
    initializeAreaName();
    bindSeaChart();
    hdWarpMap_stopAnimation(mpLayout->animSet(), 0, 0);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EF38C */
void hdWarpMap_c::startIn() {
    hdWarpMapPlay_l* play = hdWarpMap_getPlay();
    u8 mode = play->mDisplayMode;
    u8 submode = play->mDisplaySubmode;
    play->mSavedDisplayMode = mode;
    u8 option = play->mDisplayOption;
    play->mSavedDisplaySubmode = submode;
    play->mSavedDisplayOption = option;
    play = hdWarpMap_getPlay();
    play->mDisplayMode = WARP_MAP_DISPLAY_MODE;
    play = hdWarpMap_getPlay();
    play->mDisplaySubmode = WARP_MAP_DISPLAY_SUBMODE;
    sMapDisplayControl.mMode = 3;
    sMapDisplayControl.mActive = 1;
    initializeWarpAreas();
    hdWarpMap_startAnimation(mpLayout->animSet(), 0, 0, sMapAnimationStart);
}

/* 026EF88C */
void hdWarpMap_c::restoreDisplayMode() {
    hdWarpMapPlay_l* play = hdWarpMap_getPlay();
    u8 submode = play->mSavedDisplaySubmode;
    u8 option = play->mSavedDisplayOption;
    u8 mode = play->mSavedDisplayMode;
    play->mDisplayOption = option;
    play->mDisplayMode = mode;
    play->mDisplaySubmode = submode;
    hdWarpMapStatus_l* status = sMapStatus.mpStatus;
    sMapDisplayControl.mActive = 0;
    status->mActive = 0;
}

/* 026EF8DC */
void hdWarpMap_c::confirmWarp() {
    sMapStatus.mpStatus->mActive = 0;
    hdWarpMap_playSound(WARP_CONFIRM_SOUND);
    mConfirmed = 1;
}

/* 026EF924 */
void hdWarpMap_c::storeDestination() {
    hdWarpMapPlay_l* play = hdWarpMap_getPlay();
    hdSeaChartMenu_c* chart = mpSeaChart;
    hdWarpMapDestinationActor_l* actor = play->mpDestinationActor;
    if (chart && actor) actor->mWarpDestination = chart->mPreviousSelection;
}

/* 026EF644 */
void hdWarpMap_c::focusArea(u32 firstArea) {
    hdSeaChartMenu_c* chart = mpSeaChart;
    if (!chart) return;
    hdSeaChartGrid_c* details = reinterpret_cast<hdSeaChartGrid_c*>(chart->mpMap.get());
    if (!details) return;
    u32 receiver;
    if (firstArea) {
        u32 packedKey = hdSeaChartMenu_c::getMapCoordinate(0);
        gabi::Local<HdPadCoordinate> key;
        key->x = s8(packedKey >> 24);
        key->y = s8(packedKey >> 16);
        receiver = hdWarpMap_findReceiver(details, key.get());
    } else {
        hdSeaChartCell_c* item = details->mFocusedCell;
        if (!item) return;
        HdPadCoordinate* key = &item->mCoordinate;
        receiver = hdWarpMap_findReceiver(details, key);
    }
    if (!receiver) return;
    hdWarpMap_setInputMode(sInputManager.mpManager.get(), receiver);
    details->mFocusTransferred = 1;
    hdSeaChartCell_c* item = hdWarpMap_receiverItem(details, receiver);
    if (!item) return;
    hdWarpMap_prepareAreaItem(item);
    hdWarpMap_activateAreaItem(item);
    if (firstArea) {
        u8 visible = item->mDiscovered;
        s32 area = item->mId;
        selectArea(area, visible);
    }
}

/* 026EF74C */
void hdWarpMap_c::focusFirstArea() { focusArea(1); }

/* 026EFAC0 */
void hdWarpMap_c::startDisplay() {
    sMapStatus.mpStatus->mActive = 1;
    focusArea(0);
    mConfirmed = 0;
}

/* 026EFEC4 */
u32 hdWarpMap_c::setup(u32 first, u32 second, void* description) {
    hdWarpMap_attachArchives(this, sMapArchives, 6);
    if (!gabi::call_ptr<u32>(mVtable->mLoad.get(), this, &sMapScreenName, mpResAccessor.get(), first, second, description)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    initializeLayout();
    hdStateOwner* owner = mStateHolder.mMachine.mpOwner;
    hdStateObject* state = gabi::call_ptr<hdStateObject*>(owner->mpVTable->mCreateState.get(), owner, &sMapAreaNameDisplay);
    mStateHolder.mMachine.mpCurrentState = state;
    state->enter();
    return 1;
}

/* 026F03AC */
u32 hdWarpMap_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* description) {
    hdPartsLayout_c* layout = hdWarpMap_allocateLayout(sizeof(hdPartsLayout_c), sMapUiSystem.mpSystem->mHeap.get(), 4);
    if (layout) layout = hdWarpMap_constructLayout(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, first, second, third, 4, 2, description)) return 0;
    for (u32 track = 0; track < 4; ++track) {
        hdWarpMap_bindAnimation(mpLayout->animSet(), track, &sMapAnimationNames[track], &sMapAnimationPanes[sMapAnimationPaneIndices[track]]);
    }
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}

/* 026EF970 */
void hdWarpMap_c::executeConfirmation() {
    hdWarpMapTextSource_l* source = static_cast<hdWarpMapTextSource_l*>(sMapTextOwner.mpOwner->mpTextSource.get());
    if (source) {
        hdPartsStateId_c* current = hdWarpMap_currentState(&source->mMachine);
        hdPartsStateIdVtbl_c* expectedTable = sMapConfirmationState.mVtable;
        u32 currentKey = current->getKey();
        u32 expectedKey = gabi::call_ptr<u32>(expectedTable->mGetKey.get(), &sMapConfirmationState);
        if (currentKey == expectedKey || source->mReady) {
            hdSeaChartGrid_c* details = reinterpret_cast<hdSeaChartGrid_c*>(mpSeaChart->mpMap.get());
            if (details) details->mHitFound = 0;
            if (source->mAccepted) {
                storeDestination();
                hdWarpMap_changeState(&mStateHolder.mMachine, &sMapOut);
                hdWarpMap_playSound(0x8CE);
            } else {
                hdWarpMap_changeState(&mStateHolder.mMachine, &sMapDisplay);
            }
        }
    }
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026EFB08 */
void hdWarpMap_c::executeDisplay() {
    hdButtonEventManager_l* manager = static_cast<hdButtonEventManager_l*>(sInputManager.mpManager.get());
    if (!manager) return;
    gabi::Local<hdButtonEvent_l> event;
    hdWarpMap_copyEvent(event.get(), &manager->mCurrentEvent);
    u32 cancel = hdWarpMap_isCancel(event.get());
    hdSeaChartMenu_c* chart = mpSeaChart;
    if (cancel && (!chart || !chart->mOpening)) {
        hdWarpMapPlay_l* play = hdWarpMap_getPlay();
        hdWarpMapDestinationActor_l* actor = play->mpDestinationActor;
        if (actor) actor->mWarpDestination = 0xFFFFFFFF;
        hdWarpMap_playSound(0x8CC);
        hdWarpMap_changeState(&mStateHolder.mMachine, &sMapOut);
        chart = mpSeaChart;
        if (chart) chart->mClosing = 1;
        gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
        gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
        return;
    }
    if (chart) {
        if (chart->mOpening) {
            hdWarpMap_changeState(&mStateHolder.mMachine, &sMapDecideWait);
            chart = mpSeaChart;
        } else {
            chart->pollButtonEvent();
            mpSeaChart->pollSelectionActivation();
            mpSeaChart->processMapAction();
            chart = mpSeaChart;
            hdSeaChartGrid_c* details = reinterpret_cast<hdSeaChartGrid_c*>(chart->mpMap.get());
            if (details) {
                hdSeaChartCell_c* item = details->mFocusedCell;
                if (item) {
                    u8 visible = item->mDiscovered;
                    s32 area = item->mId;
                    selectArea(area, visible);
                    chart = mpSeaChart;
                }
            }
        }
        chart->openMap();
    }
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    chart = mpSeaChart;
    if (chart) chart->closeMap();
}

static hdUiSafeString sMapChildPaneNames[10] = {};
static hdUiSafeString sMapSeaChartResource;
static hdUiSafeString sMapWarpAreaResource;

/* The reference bounds SafeString comparison to 0x40001 bytes. Termination
 * callbacks may replace either string, so reload both pointers afterwards. */
static bool hdWarpMap_sameName(hdUiSafeString* name, hdUiSafeString* expected) {
    name->assureTermination();
    u32 text = gabi::ea(name->mText.get());
    expected->assureTermination();
    if (text == gabi::ea(expected->mText.get())) return true;
    be<u8>* left = name->mText.get();
    be<u8>* right = expected->mText.get();
    for (u32 index = 0; index < 0x40001; ++index) {
        u8 value = left[index];
        if (value != right[index]) return false;
        if (!value) return true;
    }
    return false;
}

/* 026F002C */
u32 hdWarpMap_c::createChild(hdWarpMapChildDesc_l* description) {
    hdScreenHeap* heap = sMapUiSystem.mpSystem->mHeap;
    for (u32 index = 0; index < 10; ++index) {
        description->mPaneName.assureTermination();
        if (!hdWarpMap_sameName(&description->mPaneName, &sMapChildPaneNames[index])) continue;
        description->mResourceName.assureTermination();
        if (hdWarpMap_sameName(&description->mResourceName, &sMapSeaChartResource)) {
            gptr<hdParts_c>* slot = mChildren.safeSlot(index);
            hdParts_c* child = hdWarpMap_allocateChild(0x164, heap, 4);
            if (child) child = hdSeaChartMenu_c::construct(static_cast<hdSeaChartMenu_c*>(child));
            *slot = child;
            if (!*mChildren.safeSlot(index)) return 0;
            return static_cast<hdSeaChartMenu_c*>((*mChildren.safeSlot(index)).get())->create(0, 0, gabi::ea(description));
        }
        description->mResourceName.assureTermination();
        if (hdWarpMap_sameName(&description->mResourceName, &sMapWarpAreaResource)) {
            gptr<hdParts_c>* slot = mChildren.safeSlot(index);
            hdParts_c* child = hdWarpMap_allocateChild(0x50, heap, 4);
            if (child) child = hdWarpArea_c::construct(static_cast<hdWarpArea_c*>(child));
            *slot = child;
            if (!*mChildren.safeSlot(index)) return 0;
            return static_cast<hdWarpArea_c*>((*mChildren.safeSlot(index)).get())->setup(0, 0, gabi::ea(description));
        }
        return hdWarpMap_createBaseChild(this, index, description);
    }
    return 1;
}

struct hdWarpMapHeaderStatics_l {
    be<f32> mMinimumAngle, mMaximumAngle;
    u8 mReserved08[0x10]; be<u8> mFirstMarker, mSecondMarker;
    u8 mReserved1A[2]; be<u32> mZeros[4];
};
WWHD_OFFSET(hdWarpMapHeaderStatics_l, mFirstMarker, 0x18);
WWHD_OFFSET(hdWarpMapHeaderStatics_l, mZeros, 0x1C);
static hdWarpMapHeaderStatics_l sMapHeaderStatics;
static f32 sMapMinimumAngle = -3.1415927410125732f;
static f32 sMapMaximumAngle = 3.1415927410125732f;
extern void hdWarpMap_constructFirstMarker(void* marker);
extern void hdWarpMap_constructSecondMarker(void* marker);
static hdUiSafeString sMapChartResourceName;
struct hdWarpMapBounds_l { be<f32> mFarX, mFarY, mNearX, mNearY; };
static hdWarpMapBounds_l sMapBounds;
static hdPartsStateId_c sMapIn;
static hdPartsStateId_c sMapAreaNameOut;
static hdPartsStateId_c sMapAreaNameIn;
static u32 sMapStateReady;
static u32 sMapStateCounter;
static hdStateId sMapNullContext;
static hdStateId sMapDisplayContext;

static void hdWarpMap_initString(hdUiSafeString& name, const char* text) {
    name.mVtable = &sMapStringVtable;
    name.mText.v = gabi::guest_addr(text);
}
static void hdWarpMap_initState(hdPartsStateId_c& state, u32 number, const char* name,
                                s16 enter, hdStateId* context) {
    state.mNumber = number;
    state.mName.v = gabi::guest_addr(name);
    state.mVtable = &sMapStateVtable;
    state.mEnter.setVirtual(enter);
    state.mExecute.setVirtual(enter + 1);
    state.mExit.setVirtual(enter + 2);
    state.mUnused.clear();
    state.mpNullStateId = context;
}

/* 026F04D8 */
static void __sinit_hd_screen_warp_map_cpp() {
    for (u32 i = 0; i < 4; ++i) sMapHeaderStatics.mZeros[i] = 0;
    __register_global_object(gabi::ea(&sScreenWarpMapRegistration));
    sMapHeaderStatics.mMaximumAngle = sMapMaximumAngle;
    sMapHeaderStatics.mMinimumAngle = sMapMinimumAngle;
    hdWarpMap_constructFirstMarker(&sMapHeaderStatics.mFirstMarker);
    __register_global_object(gabi::ea(&sScreenWarpMapSecondRegistration));
    hdWarpMap_constructSecondMarker(&sMapHeaderStatics.mSecondMarker);
    __register_global_object(gabi::ea(&sScreenWarpMapThirdRegistration));
    sMapBounds.mFarX = 50000.0f;
    sMapBounds.mFarY = 50000.0f;
    sMapBounds.mNearX = 10000.0f;
    sMapBounds.mNearY = 10000.0f;
    hdWarpMap_initString(sMapAnimationNames[0], "InAll");
    hdWarpMap_initString(sMapAnimationNames[1], "AreaNameOut");
    hdWarpMap_initString(sMapAnimationNames[2], "AreaNameIn");
    hdWarpMap_initString(sMapAnimationNames[3], "OutAll");
    hdWarpMap_initString(sMapAnimationPanes[0], "G_All_00");
    hdWarpMap_initString(sMapAnimationPanes[1], "G_AreaName_00");
    hdWarpMap_initString(sMapChildPaneNames[0], "L_Map_00");
    hdWarpMap_initString(sMapChildPaneNames[1], "L_WarpArea_00");
    hdWarpMap_initString(sMapChildPaneNames[2], "L_WarpArea_01");
    hdWarpMap_initString(sMapChildPaneNames[3], "L_WarpArea_02");
    hdWarpMap_initString(sMapChildPaneNames[4], "L_WarpArea_04");
    hdWarpMap_initString(sMapChildPaneNames[5], "L_WarpArea_05");
    hdWarpMap_initString(sMapChildPaneNames[6], "L_WarpArea_07");
    hdWarpMap_initString(sMapChildPaneNames[7], "L_WarpArea_08");
    hdWarpMap_initString(sMapChildPaneNames[8], "L_WarpArea_03");
    hdWarpMap_initString(sMapChildPaneNames[9], "L_WarpArea_06");
    hdWarpMap_initString(sMapArchives[0], "WarpMap_00");
    hdWarpMap_initString(sMapArchives[1], "Map_00");
    hdWarpMap_initString(sMapArchives[2], "WarpArea_00");
    hdWarpMap_initString(sMapArchives[3], "BtnMapBalloon_00");
    hdWarpMap_initString(sMapArchives[4], "MapAreaNow_00");
    hdWarpMap_initString(sMapArchives[5], "LinkPos_00");
    hdWarpMap_initString(sMapAreaNamePane, "T_AreaName_00");
    hdWarpMap_initString(sMapChartResourceName, "Map_00");
    u32 ready = sMapStateReady;
    hdWarpMap_initString(sMapScreenName, "WarpMap_00");
    u32 number;
    if (ready) number = sMapStateCounter;
    else { sMapStateReady = 1; number = 0; }
    hdWarpMap_initState(sMapIn, ++number, "::StateID_InAll", 0x19, &sMapNullContext);
    hdWarpMap_initState(sMapAreaNameOut, ++number, "::StateID_AreaNameOut", 0x1C, &sMapNullContext);
    hdWarpMap_initState(sMapAreaNameIn, ++number, "::StateID_AreaNameIn", 0x1F, &sMapNullContext);
    hdWarpMap_initState(sMapOut, ++number, "::StateID_OutAll", 0x22, &sMapNullContext);
    hdWarpMap_initState(sMapDecideWait, ++number, "::StateID_DecideWait", 0x25, &sMapNullContext);
    hdWarpMap_initState(sMapDisplay, ++number, "::StateID_Display", 7, &sMapDisplayContext);
    hdWarpMap_initState(sMapAreaNameDisplay, ++number, "::StateID_Blank", 4, reinterpret_cast<hdStateId*>(&sMapConfirmationState));
    sMapStateCounter = number;
}

/* WWHD glue
 data sScreenWarpMapRegistration 101F70E4 nocheck
 data sScreenWarpMapSecondRegistration 101F70F0 nocheck
 data sScreenWarpMapThirdRegistration 101F70FC nocheck
 data sMapHolderVtable 101094F8 nocheck
 data sMapSecondaryVtable 10109508 nocheck
 data sMapVtable 101093B8 nocheck
 data sMapStringVtable 10109118 nocheck
 data sMapStateVtable 10109188 nocheck
data sMapHeaderStatics 1049D5A8 nocheck
data sMapMinimumAngle 101091D8
data sMapMaximumAngle 101091DC
ext 028ED6F8 void hdWarpMap_constructFirstMarker(void* marker)
ext 028EAB2C void hdWarpMap_constructSecondMarker(void* marker)
str 10109330 "AreaNameIn" in 026F04D8
data sMapChartResourceName 1049D5A0 nocheck
data sMapBounds 1049D5B0 nocheck
data sMapIn 1049D684 nocheck
data sMapAreaNameOut 1049D6B4 nocheck
data sMapDecideWait 1049D744 nocheck
data sMapStateReady 101FD8E4 nocheck
data sMapStateCounter 101FDD50 nocheck
data sMapNullContext 101FF32C nocheck
data sMapDisplayContext 1049DD90 nocheck
str 101091E8 "InAll" in 026F04D8
str 10109220 "AreaNameOut" in 026F04D8
str 10109248 "G_AreaName_00" in 026F04D8
str 1010933C "L_Map_00" in 026F04D8
str 10109258 "L_WarpArea_00" in 026F04D8
str 10109268 "L_WarpArea_01" in 026F04D8
str 10109278 "L_WarpArea_02" in 026F04D8
str 10109288 "L_WarpArea_04" in 026F04D8
str 10109298 "L_WarpArea_05" in 026F04D8
str 101092A8 "L_WarpArea_07" in 026F04D8
str 101092B8 "L_WarpArea_08" in 026F04D8
str 10109324 "G_All_00" in 026F04D8
str 10109348 "WarpMap_00" in 026F04D8
str 10109238 "T_AreaName_00" in 026F04D8
str 101091F8 "Map_00" in 026F04D8
str 101092C8 "L_WarpArea_03" in 026F04D8
str 101092D8 "L_WarpArea_06" in 026F04D8
str 1010922C "WarpArea_00" in 026F04D8
str 10109354 "BtnMapBalloon_00" in 026F04D8
str 101092E8 "MapAreaNow_00" in 026F04D8
str 101091F0 "OutAll" in 026F04D8
str 10109368 "LinkPos_00" in 026F04D8
str 101092F8 "::StateID_AreaNameOut" in 026F04D8
str 10109374 "::StateID_AreaNameIn" in 026F04D8
str 10109200 "::StateID_InAll" in 026F04D8
str 1010938C "::StateID_OutAll" in 026F04D8
str 101093A0 "::StateID_DecideWait" in 026F04D8
str 10109310 "::StateID_Display" in 026F04D8
str 10109210 "::StateID_Blank" in 026F04D8
data sMapChildPaneNames 1049D634 nocheck
data sMapSeaChartResource 10495F0C nocheck
data sMapWarpAreaResource 1049D590 nocheck
ext 0273B050 hdParts_c* hdWarpMap_allocateChild(u32 size, hdScreenHeap* heap, u32 alignment)
ext 02677BFC hdSeaChartMenu_c* hdSeaChartMenu_c::construct(hdSeaChartMenu_c*)
ext 02678308 u32 hdSeaChartMenu_c::create(u32, u32, u32)
ext 026F92C0 u32 hdWarpMap_createBaseChild(hdWarpMap_c* map, u32 index, hdWarpMapChildDesc_l* description)
data sInputManager 101F4FF0 nocheck
data sMapAnimationStart 101091D0
data sMapTextOwner 101F8344 nocheck
data sMapAreaNamePane 1049D598 nocheck
data sMapAreaNameCapacity 10109114
data sMapDisplayControl 1047B03C nocheck
data sMapStatus 101F5A44 nocheck
data sMapArchives 1049D604 nocheck
data sMapScreenName 1049D7D4 nocheck
data sMapAnimationNames 1049D5E4 nocheck
data sMapAnimationPanes 1049D5D4 nocheck
data sMapAnimationPaneIndices 10109130
data sMapUiSystem 1018C404 nocheck
ext 026F90D8 void hdWarpMap_attachArchives(hdWarpMap_c* map, hdUiSafeString* names, u32 count)
ext 0273B050 hdPartsLayout_c* hdWarpMap_allocateLayout(u32 size, hdScreenHeap* heap, u32 alignment)
ext 026FADB0 hdPartsLayout_c* hdWarpMap_constructLayout(hdPartsLayout_c* layout)
ext 02004E04 void hdWarpMap_bindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane)
ext 025200D4 hdWarpMapPlay_l* hdWarpMap_getPlay()
ext 025E1988 void hdWarpMap_playSound(u32 sound)
ext 0267A204 u32 hdSeaChartMenu_c::getMapCoordinate(u32)
ext 02681A9C u32 hdWarpMap_findReceiver(hdSeaChartGrid_c* details, HdPadCoordinate* key)
ext 02682C30 hdSeaChartCell_c* hdWarpMap_receiverItem(hdSeaChartGrid_c* details, u32 receiver)
ext 02680340 void hdWarpMap_prepareAreaItem(hdSeaChartCell_c* item)
ext 0268034C void hdWarpMap_activateAreaItem(hdSeaChartCell_c* item)
data sMapBlankText 101091D4
ext 02704AFC void hdWarpMap_setTextCapacity(hdPartsPaneSRT_c* pane, u32 capacity)
ext 02005674 void hdWarpMap_stopAnimation(hdAnimSet* animation, u32 track, u32 flags)
ext 026DBD10 void* hdWarpMap_getAreaText(void* source)
ext 0267A250 s32 hdSeaChartMenu_c::findMapResource(u32)
ext 026F89E8 void hdWarpMap_constructBase(hdWarpMap_c* map)
ext 02679B88 void hdSeaChartMenu_c::setSelectionMode(u32)
ext 02679CCC void hdSeaChartMenu_c::bindActiveButton()
ext 026F91F8 void hdWarpMap_releaseBase(hdWarpMap_c* map)
data sMapDisplay 1049D774 nocheck
data sMapOut 1049D714 nocheck
data sMapAreaNameIn 1049D6E4 nocheck
ext 020013C0 void hdWarpMap_copyEvent(hdButtonEvent_l* output, hdButtonEvent_l* input)
ext 026F6340 u32 hdWarpMap_isCancel(hdButtonEvent_l* event)
ext 02679E30 void hdSeaChartMenu_c::pollButtonEvent()
ext 02679FE4 void hdSeaChartMenu_c::pollSelectionActivation()
ext 0267A034 void hdSeaChartMenu_c::processMapAction()
data sMapConfirmationState 1049DD60 nocheck
ext 02006478 hdPartsStateId_c* hdWarpMap_currentState(hdStateMachine* machine)
data sMapAreaNameDisplay 1049D7A4 nocheck
ext 027045F0 void hdWarpMap_setText(void* panel, void* text)
ext 02005840 u32 hdWarpMap_animationDone(hdAnimSet* animation, u32 track)
ext 020063C0 void hdWarpMap_changeState(hdStateMachine* machine, hdPartsStateId_c* state)
ext 02679D68 void hdSeaChartMenu_c::openMap()
ext 02679D78 void hdSeaChartMenu_c::closeMap()
ext 026F88F0 void hdWarpMap_destroyBase(hdWarpMap_c* map, u32 flags)
ext 026819F0 void hdWarpMap_prepareDetails(void* details)
ext 026826D8 void hdWarpMap_refreshDetails(void* details)
ext 02680CB4 void hdWarpMap_resetDetails(void* details)
ext 02679C6C void hdSeaChartMenu_c::updateOpenMap()
ext 02615154 void hdWarpMap_setInputMode(void* manager, u32 mode)
ext 020053E4 void hdWarpMap_startAnimation(hdAnimSet* animation, u32 track, u32 flags, f32 frame)
ext 02006364 void hdWarpMap_executeMachine(hdStateMachine* machine)
*/
