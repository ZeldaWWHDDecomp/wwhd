#include "d/hd_input_ctrl.h"
#include "d/hd_sea_chart_components.h"
#include "hd/hd_ui_020015C8.h"
using namespace gabi;

static f32 chartGridDivide(f32 numerator, f32 denominator) {
    return f32(f64(numerator) / f64(denominator));
}

static f32 sChartGridHalf;
static f32 sChartGridDimension;
static f32 sChartGridZero;
static f32 sChartGridOffsetX;
static f32 sChartGridOffsetY;
struct hdSeaChartGridStatics_c {
    be<f32> mMinimum, mMaximum;
    HdPadPoint mOrigin, mOffset;
    be<u8> mFirstFlag, mSecondFlag;
    u8 mPadding[2];
    be<u32> mStorage[4];
};
WWHD_OFFSET(hdSeaChartGridStatics_c, mMinimum, 0);
WWHD_OFFSET(hdSeaChartGridStatics_c, mMaximum, 4);
WWHD_OFFSET(hdSeaChartGridStatics_c, mOrigin, 8);
WWHD_OFFSET(hdSeaChartGridStatics_c, mOffset, 0x10);
WWHD_OFFSET(hdSeaChartGridStatics_c, mFirstFlag, 0x18);
WWHD_OFFSET(hdSeaChartGridStatics_c, mSecondFlag, 0x19);
WWHD_OFFSET(hdSeaChartGridStatics_c, mStorage, 0x1C);
struct hdSeaChartGridRegistration_c {
    be<u32> mWords[3];
};
WWHD_SIZE(hdSeaChartGridRegistration_c, 0xC);
static hdSeaChartGridStatics_c sChartGridStatics;
static hdSeaChartGridRegistration_c sChartGridRegistrations[3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static f32 sChartGridMinimum;
static f32 sChartGridMaximum;
static f32 sChartGridDefaultOrigin;
static f32 sChartGridDefaultOffset;
void seaChartGridRegisterStatic(hdSeaChartGridRegistration_c* record);
void seaChartGridInitializeFirstFlag(be<u8>* flag);
void seaChartGridInitializeSecondFlag(be<u8>* flag);

/* 02683874 */
void seaChartInitializeGridStatics() {
    sChartGridStatics.mStorage[3] = 0;
    sChartGridStatics.mStorage[2] = 0;
    sChartGridStatics.mStorage[1] = 0;
    sChartGridStatics.mStorage[0] = 0;
    seaChartGridRegisterStatic(&sChartGridRegistrations[0]);
    const f32 minimum = sChartGridMinimum;
    const f32 maximum = sChartGridMaximum;
    sChartGridStatics.mMinimum = minimum;
    sChartGridStatics.mMaximum = maximum;
    seaChartGridInitializeFirstFlag(&sChartGridStatics.mFirstFlag);
    seaChartGridRegisterStatic(&sChartGridRegistrations[1]);
    seaChartGridInitializeSecondFlag(&sChartGridStatics.mSecondFlag);
    seaChartGridRegisterStatic(&sChartGridRegistrations[2]);
    const f32 origin = sChartGridDefaultOrigin;
    const f32 offset = sChartGridDefaultOffset;
    sChartGridStatics.mOrigin.x = origin;
    sChartGridStatics.mOffset.x = offset;
    sChartGridStatics.mOrigin.y = origin;
    sChartGridStatics.mOffset.y = offset;
}
static u32 sChartGridVtable;
void* seaChartGridAllocate(u32 size);
void* seaChartGridAllocateOnHeap(u32 size, hdScreenHeap* heap, u32 alignment);
hdSeaChartCell_c* seaChartGridConstructCell(hdSeaChartCell_c* cell);
struct hdSeaChartGridUiSingleton_c {
    gptr<hdScreenUiSystem> mValue;
};
static hdSeaChartGridUiSingleton_c sChartGridUi;
void seaChartGridCountDiscovered(HdPadGrid* grid);
u32 seaChartGridReadInput(hdSeaChartGrid_c* grid);
void seaChartGridPlaySound(const char* name);
struct hdSeaChartGridEventSingleton_c {
    gptr<void> mValue;
};
static hdSeaChartGridEventSingleton_c sChartGridEventManager;
void seaChartGridSetEventMode(void* manager, HdInputReceiver* receiver);
HdInputReceiver* seaChartGridGetEventReceiver(void* manager);
u32 seaChartGridIsDirectionalEvent(u32 kind);
u32 seaChartGridEventDirection(u32 kind);
HdInputReceiver* seaChartGridReceiverUserData(HdInputReceiver* receiver, u32 index);
struct hdSeaChartGridInputSingleton_c {
    gptr<hdInputManager> mValue;
};
static hdSeaChartGridInputSingleton_c sChartGridInput;
static HdPadPoint sChartGridFallbackPosition;
struct hdSeaChartInputKindPair_c {
    be<u32> mSource, mTarget;
};
struct hdSeaChartInputKindTable_c {
    hdSeaChartInputKindPair_c mItems[12];
};
static hdSeaChartInputKindTable_c sChartGridInputKinds;
u32 seaChartGridEventBusy(void* manager);
struct hdSeaChartGridOwnerSingleton_c {
    gptr<HdPadUiOwner> mValue;
};
struct hdSeaChartGridSaveSingleton_c {
    gptr<hdSaveInfoHolder_c> mValue;
};
static hdSeaChartGridOwnerSingleton_c sChartGridOwner;
static hdSeaChartGridSaveSingleton_c sChartGridSave;
s32 seaChartGridFindLocation(hdMapDisplayCoordinator_c* coordinator, s32 x, s32 y);
void seaChartGridGetLocation(hdMapDisplayCoordinator_c* coordinator, s32 location, be<s8>* x,
                             be<s8>* y, be<s16>* room, be<s16>* layer, be<u8>* kind);
hdChartEntry_c* seaChartGridFindSeaChart(hdMapDisplayCoordinator_c* coordinator, u32 chart);
u32 seaChartGridIsChartComplete(hdMapDisplayCoordinator_c* coordinator, u32 chart);
void seaChartGridBuild(HdPadGrid* grid);
hdPartsStateId_c* seaChartGridGetState(hdStateMachine* machine);
static hdPartsStateId_c sChartGridChangeToDungeonState;
static hdPartsStateId_c sChartGridBlankState;
static hdPartsStateId_c sChartGridFocusSuppressionState;
static hdPartsStateId_c sChartGridAlternateFocusSuppressionState;
static hdMapDisplayCoordinator_c* chartGridCoordinator();

static bool chartGridStateEquals(hdStateMachine* machine, hdPartsStateId_c* expected) {
    hdPartsStateId_c* state = seaChartGridGetState(machine);
    const u32 currentGetKey = state->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedVtable = expected->mVtable;
    const u32 currentKey = gabi::call_ptr<u32>(currentGetKey, state);
    const u32 expectedKey = gabi::call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
    return currentKey == expectedKey;
}

/* The owner contains the library-pane array before its shared chart-grid members. */
struct hdSeaChartMarkPanes_l {
    u8 mReserved[0x84];
    gptr<hdSeaChartPane_l> mPanes[49];
};
WWHD_OFFSET(hdSeaChartMarkPanes_l, mPanes, 0x84);
static f32 sChartGridCoordinateScale;

/* 026815A8 */
void hdSeaChartGrid_c::updateChartMarkLayout() {
    hdSeaChartPane_l* pane = mTouchPane;
    if (!pane || !mMapOwner)
        return;
    const f32 cellWidth = chartGridDivide(pane->mSize.x, sChartGridDimension);
    const f32 cellHeight = chartGridDivide(pane->mSize.y, sChartGridDimension);
    const f32 coordinateScale = sChartGridCoordinateScale;
    const f32 originY = pane->mPosition.y;
    const f32 zero = sChartGridZero;
    const f32 originX = pane->mPosition.x;
    for (u32 index = 0; index < 49; ++index) {
        hdSeaChartMarkPanes_l* owner = reinterpret_cast<hdSeaChartMarkPanes_l*>(mMapOwner.get());
        hdSeaChartCell_c* cell = mCells[index];
        const f32 coordinateX = fmuls_ppc(f32(s8(cell->mCoordinate.x)), coordinateScale);
        const f32 coordinateY = fmuls_ppc(f32(s8(cell->mCoordinate.y)), coordinateScale);
        const f32 resourceX = fadds_ppc(coordinateX, f32(s16(cell->mResourceIndices[0])));
        const f32 resourceY = fadds_ppc(coordinateY, f32(s16(cell->mResourceIndices[1])));
        const f32 positionX =
            fadds_ppc(originX, chartGridDivide(fmuls_ppc(resourceX, cellWidth), coordinateScale));
        const f32 positionY =
            fsubs_ppc(originY, chartGridDivide(fmuls_ppc(resourceY, cellHeight), coordinateScale));
        hdSeaChartPane_l* mark = owner->mPanes[index];
        if (mark) {
            const u8 flags = mark->mFlags;
            mark->mPosition.y = positionY;
            mark->mPosition.z = zero;
            mark->mPosition.x = positionX;
            mark->mFlags = flags | 0x10;
        }
        if (seaChartGridIsChartComplete(chartGridCoordinator(), index))
            mCells[index]->resetMarkAnimation();
        else
            mCells[index]->activateMarkAnimation();
    }
}

/* 026826EC */
void hdSeaChartGrid_c::findPendingFocus() {
    if (!mMapOwner)
        return;
    Local<HdPadCoordinate> coordinate;
    coordinate->x = chartGridCoordinator()->mSeaCoordinate.x;
    coordinate->y = chartGridCoordinator()->mSeaCoordinate.y;
    hdSeaChartCell_c* pending = reinterpret_cast<hdSeaChartCell_c*>(
        hdGridFind(reinterpret_cast<HdPadCellArray*>(this), coordinate));
    hdSeaChartCell_c* previous = mPreviousPendingFocus;
    mPendingFocus = pending;
    const u32 kind = mInputKind;
    if (pending != previous)
        mPendingFocusChanged = 1;
    if (kind || !mPendingFocusChanged || !mFocusedCell || !mPreviousPendingFocus || !mPendingFocus)
        return;
    hdSeaChartMenuMap_l* other = reinterpret_cast<hdSeaChartMenuMap_l*>(mMapOwner->mIcons.get());
    if (!other)
        return;
    hdStateMachine* machine = reinterpret_cast<hdStateMachine*>(&other->mState);
    if (chartGridStateEquals(machine, &sChartGridBlankState) ||
        chartGridStateEquals(machine, &sChartGridFocusSuppressionState) ||
        chartGridStateEquals(machine, &sChartGridAlternateFocusSuppressionState))
        return;
    const u32 mode = other->mSelectionMode;
    if (mode == 1 || mode == 4)
        mRefreshRequested = 1;
}

/* 026826D8 */
void hdSeaChartGrid_c::connectWidgets() {
    if (mSelectionMode == 1)
        connectRestrictedWidgets();
    else
        connectNeighborWidgets();
}

/* 02682AA8 */
void hdSeaChartGrid_c::refreshGrid() {
    seaChartGridBuild(reinterpret_cast<HdPadGrid*>(this));
    updateDiscoveredCount();
    refreshFocusAnimations();
    if (mSelectionMode == 0)
        updateChartMarkLayout();
    clearNeighborWidgets();
    connectWidgets();
    if (mSelectionMode == 0) {
        findPendingFocus();
        if (mRefreshRequested)
            transferPendingFocus();
        for (u32 index = 0; index < 49; ++index)
            mCells[index]->applyRequests();
        for (u32 index = 0; index < 49; ++index)
            mCells[index]->playFocusAnimationForPhase();
        updateFocusedCell();
        updateTrackedCell();
    }
}

/* 0268354C */
void hdSeaChartGrid_c::updateCells() {
    hdMapDisplayCoordinator_c* coordinator = chartGridCoordinator();
    if (coordinator) {
        hdPartsStateId_c* state = seaChartGridGetState(&coordinator->mState.mMachine);
        const u32 currentGetKey = state->mVtable->mGetKey.get();
        hdPartsStateIdVtbl_c* expectedVtable = sChartGridChangeToDungeonState.mVtable;
        const u32 current = gabi::call_ptr<u32>(currentGetKey, state);
        const u32 expectedGetKey = expectedVtable->mGetKey.get();
        const u32 expected = gabi::call_ptr<u32>(expectedGetKey, &sChartGridChangeToDungeonState);
        if (current == expected && mFocusTransferred) {
            mFocusTransferred = 0;
            mInputKind = 6;
        }
    }
    findPendingFocus();
    if (mRefreshRequested)
        transferPendingFocus();
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->mInputKind = mInputKind;
    handleInputPhase(mInputKind);
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->updateInput();
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->applyRequests();
    if (mSelectionMode == 0) {
        for (u32 index = 0; index < 49; ++index)
            mCells[index]->playFocusAnimationForPhase();
    }
    updateFocusedCell();
    updateTrackedCell();
    updateInputContext();
    seaChartGridCountDiscovered(reinterpret_cast<HdPadGrid*>(this));
    if (mDiscoveryChanged)
        applyCellFocusAnimations();
    if (mSelectionMode == 0)
        refreshChartMarks();
}

static hdMapDisplayCoordinator_c* chartGridCoordinator() {
    return reinterpret_cast<hdMapDisplayCoordinator_c*>(sChartGridOwner.mValue->mUi.get());
}

/* 02680E98 */
void hdSeaChartGrid_c::loadCellLocations() {
    for (s32 y = -3; y <= 3; ++y) {
        for (s32 x = -3; x <= 3; ++x) {
            const s32 location = seaChartGridFindLocation(chartGridCoordinator(), x, y);
            if (location == -1)
                continue;
            struct LocationOutput {
                be<s8> x, y;
                be<s16> room, layer;
            };
            Local<LocationOutput> output;
            output->y = -10;
            output->room = 0;
            output->layer = 0;
            output->x = -10;
            seaChartGridGetLocation(chartGridCoordinator(), location, &output->x, &output->y,
                                    &output->room, &output->layer, nullptr);
            const u32 slot = u32(location) < 49 ? u32(location) : 0;
            mCells[slot]->mId = location;
            mCells[slot]->mCoordinate.x = output->x;
            mCells[slot]->mCoordinate.y = output->y;
            const s16 room = output->room;
            const s16 layer = output->layer;
            mCells[slot]->mMapIndices[0] = u16(room);
            mCells[slot]->mMapIndices[1] = u16(layer);
        }
    }
}

/* 02680FEC */
void hdSeaChartGrid_c::loadSalvagePositions() {
    hdMapDisplayCoordinator_c* coordinator = chartGridCoordinator();
    for (u32 index = 0; index < 49; ++index) {
        hdChartEntry_c* entry = seaChartGridFindSeaChart(coordinator, index);
        if (!entry)
            continue;
        const u32 position =
            sChartGridSave.mValue->mSaveInfo.mSavedata.mPlayer.mInfo.mRandomSalvagePoint;
        if (position < 4) {
            const s16 x = entry->mPositions[position * 4 + 2];
            const s16 y = entry->mPositions[position * 4 + 3];
            mCells[index]->mResourceIndices[0] = u16(x);
            mCells[index]->mResourceIndices[1] = u16(y);
        }
    }
}

/* 026834A4 */
void hdSeaChartGrid_c::refreshChartMarks() {
    for (u32 index = 0; index < 49; ++index) {
        if (!mCells[index]->mMarkAnimationActive &&
            seaChartGridIsChartComplete(chartGridCoordinator(), index))
            mCells[index]->playMarkAnimation();
    }
}

/* 02682E0C */
u32 hdSeaChartGrid_c::translateInputKind(u32 kind) {
    for (u32 index = 0; index < 12; ++index) {
        if (sChartGridInputKinds.mItems[index].mSource == kind)
            return sChartGridInputKinds.mItems[index].mTarget;
    }
    return 0;
}

/* 02682E78 */
u32 hdSeaChartGrid_c::isButtonEvent(hdButtonEvent_l* event, u32 button) {
    if (seaChartGridEventBusy(sChartGridEventManager.mValue))
        return 0;
    return event->mType == HD_BUTTON_EVENT_TOUCH_e && event->mButton == button;
}

/* 02681AD0 */
void hdSeaChartGrid_c::bindCoordinateWidget(hdSeaChartCell_c* cell, HdPadCoordinate* coordinate,
                                            u32 direction) {
    HdInputReceiver* receiver = findCoordinateWidget(coordinate);
    cell->setWidgetUserData(direction, gabi::ea(receiver));
}

/* 02682BB8 */
HdInputReceiver* hdSeaChartGrid_c::resolveEventNeighbor(HdInputReceiver* receiver, u32 eventKind) {
    if (!receiver || !seaChartGridIsDirectionalEvent(eventKind))
        return nullptr;
    const u32 direction = seaChartGridEventDirection(eventKind);
    return seaChartGridReceiverUserData(receiver, direction);
}

/* 02682CA4 */
u32 hdSeaChartGrid_c::acceptPointInput(u32 kind) {
    if (mHitFound)
        return 0;
    if (mEnabled) {
        if (kind == 4)
            mEnabled = 0;
        return 0;
    }
    const u32 accepted = u32(kind - 2) <= 2;
    if (accepted && kind == 2) {
        hdInputManager* input = sChartGridInput.mValue;
        HdPadPoint* position = &sChartGridFallbackPosition;
        if (u32(input->mPointer.mPointerFlags) & 1)
            position = reinterpret_cast<HdPadPoint*>(&input->mPointer.mCalibratedX);
        for (u32 index = 0; index < 49; ++index) {
            if (mCells[index]->containsScaledPoint(position) && !mCells[index]->mInputEnabled) {
                mEnabled = 1;
                return 0;
            }
        }
    }
    return accepted;
}

/* 02683444 */
void hdSeaChartGrid_c::updateInputContext() {
    HdInputReceiver* receiver = seaChartGridGetEventReceiver(sChartGridEventManager.mValue);
    mInputContext = receiver ? u32(findWidgetCell(receiver) != nullptr) : 2;
}

/* 02683808 */
void hdSeaChartGrid_c::finishSelection() {
    HdPadGrid* other = reinterpret_cast<HdPadGrid*>(mMapOwner->mIcons.get());
    if (other && other->mOffsetMode != 3) {
        if (mTrackedCell)
            mTrackedCell->stopTrackingInput();
        if (mFocusedCell)
            mFocusedCell->beginSelection();
        mFocusTransferred = 0;
    }
}

/* 026832F4 */
void hdSeaChartGrid_c::handleInputPhase(u32 phase) {
    switch (phase) {
    case 1:
        mInputLocked = 1;
        mFocusTransferred = 1;
        if (mTrackedCell)
            mTrackedCell->stopTrackingInput();
        break;
    case 3:
        mInputLocked = 0;
        break;
    case 4:
        mInputLocked = 0;
        if (mFocusedCell)
            mFocusedCell->beginSelection();
        break;
    case 5: {
        HdPadGrid* other = reinterpret_cast<HdPadGrid*>(mMapOwner->mIcons.get());
        if (other && other->mOffsetMode != 3) {
            hdSeaChartCell_c* focused = mFocusedCell;
            if (focused) {
                seaChartGridSetEventMode(sChartGridEventManager.mValue, focused->mTouchWidget);
                focused->requestFocus();
            } else if (mPendingInputCell) {
                mPendingInputCell->requestFocus();
            }
        }
        break;
    }
    case 6: {
        HdPadGrid* other = reinterpret_cast<HdPadGrid*>(mMapOwner->mIcons.get());
        if (other && other->mOffsetMode != 3) {
            if (mTrackedCell)
                mTrackedCell->stopTrackingInput();
            if (mFocusedCell)
                mFocusedCell->beginSelection();
        }
        break;
    }
    default:
        break;
    }
    mPendingInputCell = nullptr;
}

/* 026819F0 */
void hdSeaChartGrid_c::clearNeighborWidgets() {
    for (s32 y = -3; y <= 3; ++y) {
        for (s32 x = -3; x <= 3; ++x) {
            Local<HdPadCoordinate> coordinate;
            coordinate->x = s8(x);
            coordinate->y = s8(y);
            hdSeaChartCell_c* cell = reinterpret_cast<hdSeaChartCell_c*>(
                hdGridFind(reinterpret_cast<HdPadCellArray*>(this), coordinate));
            if (cell) {
                for (u32 direction = 0; direction < 8; ++direction)
                    cell->setWidgetUserData(direction, 0);
            }
        }
    }
}

/* 02681A9C */
HdInputReceiver* hdSeaChartGrid_c::findCoordinateWidget(HdPadCoordinate* coordinate) {
    hdSeaChartCell_c* cell = reinterpret_cast<hdSeaChartCell_c*>(
        hdGridFind(reinterpret_cast<HdPadCellArray*>(this), coordinate));
    return cell ? cell->mTouchWidget.get() : nullptr;
}

u32 seaChartGridRestrictedCoordinate(u32 index);

static void chartGridRestrictedCoordinate(HdPadCoordinate* coordinate, u32 index) {
    const u32 packed = seaChartGridRestrictedCoordinate(index);
    coordinate->x = s8(packed >> 24);
    coordinate->y = s8(packed >> 16);
}

static hdSeaChartCell_c* chartGridFindRestricted(hdSeaChartGrid_c* grid, u32 index) {
    Local<HdPadCoordinate> coordinate;
    chartGridRestrictedCoordinate(coordinate, index);
    return reinterpret_cast<hdSeaChartCell_c*>(
        hdGridFind(reinterpret_cast<HdPadCellArray*>(grid), coordinate));
}

static void chartGridBindRestricted(hdSeaChartGrid_c* grid, hdSeaChartCell_c* cell, u32 target,
                                    u32 direction) {
    Local<HdPadCoordinate> coordinate;
    chartGridRestrictedCoordinate(coordinate, target);
    grid->bindCoordinateWidget(cell, coordinate, direction);
}

/* 02681B10 */
void hdSeaChartGrid_c::connectRestrictedWidgets() {
    hdSeaChartCell_c* fallback = chartGridFindRestricted(this, 0);
    if (fallback) {
        for (s32 y = -3; y <= 3; ++y) {
            for (s32 x = -3; x <= 3; ++x) {
                Local<HdPadCoordinate> coordinate;
                coordinate->x = s8(x);
                coordinate->y = s8(y);
                hdSeaChartCell_c* cell = reinterpret_cast<hdSeaChartCell_c*>(
                    hdGridFind(reinterpret_cast<HdPadCellArray*>(this), coordinate));
                if (cell && cell != fallback) {
                    cell->mInputEnabled = 0;
                    for (u32 direction = 0; direction < 8; ++direction)
                        cell->setWidgetUserData(direction, gabi::ea(fallback->mTouchWidget.get()));
                }
            }
        }
    }
    for (u32 index = 0; index < 9; ++index) {
        hdSeaChartCell_c* cell = chartGridFindRestricted(this, index);
        if (cell) {
            for (u32 direction = 0; direction < 8; ++direction)
                cell->setWidgetUserData(direction, 0);
        }
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 0)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 1, 7);
        chartGridBindRestricted(this, cell, 7, 3);
        chartGridBindRestricted(this, cell, 7, 5);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 1)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 0, 6);
        chartGridBindRestricted(this, cell, 2, 7);
        chartGridBindRestricted(this, cell, 7, 3);
        chartGridBindRestricted(this, cell, 7, 4);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 2)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 1, 6);
        chartGridBindRestricted(this, cell, 7, 4);
        chartGridBindRestricted(this, cell, 7, 3);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 7)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 0, 1);
        chartGridBindRestricted(this, cell, 0, 0);
        chartGridBindRestricted(this, cell, 1, 2);
        chartGridBindRestricted(this, cell, 3, 4);
        chartGridBindRestricted(this, cell, 3, 3);
        chartGridBindRestricted(this, cell, 4, 5);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 3)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 7, 0);
        chartGridBindRestricted(this, cell, 7, 2);
        chartGridBindRestricted(this, cell, 4, 7);
        chartGridBindRestricted(this, cell, 8, 3);
        chartGridBindRestricted(this, cell, 8, 5);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 4)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 7, 1);
        chartGridBindRestricted(this, cell, 7, 0);
        chartGridBindRestricted(this, cell, 3, 6);
        chartGridBindRestricted(this, cell, 8, 4);
        chartGridBindRestricted(this, cell, 8, 3);
        chartGridBindRestricted(this, cell, 5, 5);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 8)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 3, 1);
        chartGridBindRestricted(this, cell, 3, 0);
        chartGridBindRestricted(this, cell, 4, 2);
        chartGridBindRestricted(this, cell, 5, 7);
        chartGridBindRestricted(this, cell, 6, 4);
        chartGridBindRestricted(this, cell, 6, 3);
        chartGridBindRestricted(this, cell, 6, 6);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 5)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 4, 1);
        chartGridBindRestricted(this, cell, 4, 0);
        chartGridBindRestricted(this, cell, 8, 6);
    }
    if (hdSeaChartCell_c* cell = chartGridFindRestricted(this, 6)) {
        cell->mInputEnabled = 1;
        chartGridBindRestricted(this, cell, 8, 0);
        chartGridBindRestricted(this, cell, 8, 2);
        chartGridBindRestricted(this, cell, 8, 7);
    }
}

/* 02682524 */
hdSeaChartCell_c* hdSeaChartGrid_c::findNeighbor(HdPadCoordinate* coordinate, u32 direction) {
    Local<HdPadCoordinate> neighbor;
    neighbor->x = coordinate->x;
    neighbor->y = coordinate->y;
    switch (direction) {
    case 0:
        neighbor->y = s8(u8(neighbor->y) - 1);
        break;
    case 1:
        neighbor->y = s8(u8(neighbor->y) - 1);
        neighbor->x = s8(u8(neighbor->x) - 1);
        break;
    case 2:
        neighbor->x = s8(u8(neighbor->x) + 1);
        neighbor->y = s8(u8(neighbor->y) - 1);
        break;
    case 3:
        neighbor->y = s8(u8(neighbor->y) + 1);
        break;
    case 4:
        neighbor->x = s8(u8(neighbor->x) - 1);
        neighbor->y = s8(u8(neighbor->y) + 1);
        break;
    case 5:
        neighbor->y = s8(u8(neighbor->y) + 1);
        neighbor->x = s8(u8(neighbor->x) + 1);
        break;
    case 6:
        neighbor->x = s8(u8(neighbor->x) - 1);
        break;
    case 7:
        neighbor->x = s8(u8(neighbor->x) + 1);
        break;
    default:
        break;
    }
    return reinterpret_cast<hdSeaChartCell_c*>(
        hdGridFind(reinterpret_cast<HdPadCellArray*>(this), neighbor));
}

/* 02682614 */
void hdSeaChartGrid_c::connectNeighborWidgets() {
    for (s32 y = -3; y <= 3; ++y) {
        for (s32 x = -3; x <= 3; ++x) {
            Local<HdPadCoordinate> coordinate;
            coordinate->x = s8(x);
            coordinate->y = s8(y);
            hdSeaChartCell_c* cell = reinterpret_cast<hdSeaChartCell_c*>(
                hdGridFind(reinterpret_cast<HdPadCellArray*>(this), coordinate));
            if (cell)
                cell->mInputEnabled = 1;
            for (u32 direction = 0; direction < 8; ++direction) {
                Local<HdPadCoordinate> origin;
                origin->x = s8(x);
                origin->y = s8(y);
                hdSeaChartCell_c* neighbor = findNeighbor(origin, direction);
                if (cell && neighbor)
                    cell->setWidgetUserData(direction, gabi::ea(neighbor->mTouchWidget.get()));
            }
        }
    }
}

/* 026828B4 */
void hdSeaChartGrid_c::transferPendingFocus() {
    hdSeaChartCell_c* focused = mFocusedCell;
    if (!focused || !mPreviousPendingFocus || !mPendingFocus)
        return;
    if (focused->mTrackingInput) {
        focused->stopTrackingInput();
        focused = mFocusedCell;
    }
    if (focused->mFocused) {
        focused->cancelFocus();
        mPendingFocus->requestSelection();
        mPendingFocus->requestFocus();
        seaChartGridSetEventMode(sChartGridEventManager.mValue, mPendingFocus->mTouchWidget);
        mFocusTransferred = 1;
    }
}

template <class T> static T* chartGridEmbeddedMember(hdSeaChartGrid_c* self, u32 offset) {
    // Retain the 32-bit member-address test used by GHS embedded construction.
    const u32 address = u32(gabi::ea(self) + offset);
    return address ? gabi::at<T>(address) : static_cast<T*>(seaChartGridAllocate(sizeof(T)));
}

/* 02680BAC */
void hdSeaChartGrid_c::setupCellWidgets() {
    calculateLayoutBounds(&mLayoutBounds);
    const f32 width = fsubs_ppc(mLayoutBounds.mRight, mLayoutBounds.mLeft);
    const f32 dimension = sChartGridDimension;
    const f32 height = fsubs_ppc(mLayoutBounds.mBottom, mLayoutBounds.mTop);
    mCellSize.x = chartGridDivide(width, dimension);
    mCellSize.y = chartGridDivide(height, dimension);
    for (u32 index = 0; index < 49; ++index) {
        mCells[index]->mTouchPane = mTouchPane;
        mCells[index]->createTouchWidget();
        // Widget construction can replace the cell; retain the original reload.
        mCells[index]->mGrid = reinterpret_cast<HdPadGrid*>(this);
    }
}

/* 0268081C */
hdSeaChartGrid_c* seaChartGridConstruct(hdSeaChartGrid_c* self) {
    if (!self)
        self = static_cast<hdSeaChartGrid_c*>(seaChartGridAllocate(sizeof(*self)));
    if (!self)
        return nullptr;
    self->mVtable.v = gabi::ea(&sChartGridVtable);
    const f32 zero = sChartGridZero;
    self->mTouchPane = nullptr;
    self->mOwner = nullptr;
    HdPadRect* bounds =
        chartGridEmbeddedMember<HdPadRect>(self, offsetof(hdSeaChartGrid_c, mLayoutBounds));
    if (bounds) {
        bounds->mRight = zero;
        bounds->mTop = zero;
        bounds->mLeft = zero;
        bounds->mBottom = zero;
    }
    HdPadPoint* size =
        chartGridEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartGrid_c, mCellSize));
    if (size) {
        size->y = zero;
        size->x = zero;
    }
    self->mAnimations = nullptr;
    self->mInputChanged = 0;
    self->mSlotStarts[2] = -1;
    self->mInputContext = 2;
    self->mSlotStarts[1] = -1;
    self->mEntryStarts[0] = -1;
    self->mSlotStarts[0] = -1;
    self->mSelectionMode = 0;
    self->mEntryStarts[3] = -1;
    self->mFocusChanged = 0;
    self->mEntryStarts[2] = -1;
    self->mPreviousFocusedCell = nullptr;
    self->mTrackedCell = nullptr;
    self->mInputPositionChanged = 0;
    self->mPendingInputCell = nullptr;
    self->mMapOwner = nullptr;
    self->mPreviousPendingFocus = nullptr;
    self->mEntryStarts[1] = -1;
    self->mSlotStarts[3] = -1;
    self->mRefreshRequested = 0;
    self->mInputLocked = 0;
    self->mPendingFocus = nullptr;
    self->mFocusTransferred = 0;
    self->mFocusedCell = nullptr;
    self->mPreviousTrackedCell = nullptr;
    self->mPendingFocusChanged = 0;
    self->mDiscoveredCount = 0;
    self->mPreviousDiscoveredCount = 0;
    self->mDiscoveryChanged = 0;
    self->mInputKind = 0;
    self->mTrackingChanged = 0;
    HdPadPoint* offset =
        chartGridEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartGrid_c, mOffset));
    if (offset) {
        const f32 x = sChartGridOffsetX;
        const f32 y = sChartGridOffsetY;
        offset->x = x;
        offset->y = y;
    }
    self->mEnabled = 0;
    self->mHitFound = 0;
    for (u32 index = 0; index < 49; ++index)
        self->mCells[index] = nullptr;
    return self;
}

/* 026809DC */
void hdSeaChartGrid_c::createCells() {
    hdScreenHeap* heap = sChartGridUi.mValue->mHeap;
    for (u32 index = 0; index < 49; ++index) {
        hdSeaChartCell_c* cell = static_cast<hdSeaChartCell_c*>(
            seaChartGridAllocateOnHeap(sizeof(hdSeaChartCell_c), heap, 4));
        if (cell)
            cell = seaChartGridConstructCell(cell);
        mCells[index] = cell;
    }
}

/* 02680A68 */
void hdSeaChartGrid_c::destroyCells() {
    for (u32 index = 0; index < 49; ++index) {
        hdSeaChartCell_c* cell = mCells[index];
        if (cell) {
            gabi::call_ptr<void>(cell->mVtable->mDestroy.get(), cell, 3);
            mCells[index] = nullptr;
        }
    }
}

/* 02680B24 */
void hdSeaChartGrid_c::calculateLayoutBounds(HdPadRect* bounds) {
    if (!mOwner || !mOwner->mPane)
        return;
    hdSeaChartPane_l* pane = reinterpret_cast<hdSeaChartPane_l*>(mOwner->mPane.get());
    const f32 half = sChartGridHalf;
    const f32 halfHeight = fmuls_ppc(pane->mSize.y, half);
    const f32 halfWidth = fmuls_ppc(pane->mSize.x, half);
    const f32 x = pane->mPosition.x;
    const f32 left = fsubs_ppc(x, halfWidth);
    const f32 y = pane->mPosition.y;
    const f32 right = fadds_ppc(x, halfWidth);
    const f32 top = fsubs_ppc(y, halfHeight);
    const f32 bottom = fadds_ppc(y, halfHeight);
    if (left < right) {
        bounds->mRight = right;
        bounds->mLeft = left;
    } else {
        bounds->mRight = left;
        bounds->mLeft = right;
    }
    if (top < bottom) {
        bounds->mBottom = bottom;
        bounds->mTop = top;
    } else {
        bounds->mTop = bottom;
        bounds->mBottom = top;
    }
}

/* 02680CB4 */
void hdSeaChartGrid_c::resetCells() {
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->resetInteraction();
}

/* 02680D1C */
void hdSeaChartGrid_c::clearInput() {
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->clearInputState();
    mFocusTransferred = 0;
    mInputLocked = 0;
}

/* 02680D90 */
void hdSeaChartGrid_c::setSelectionMode(u32 mode) {
    mSelectionMode = mode;
    if (mode == 1) {
        for (u32 index = 0; index < 49; ++index)
            mCells[index]->mSelectionReleased = 1;
    }
}

/* 026814F8 */
void hdSeaChartGrid_c::updateDiscoveredCount() {
    mDiscoveredCount = 0;
    mPreviousDiscoveredCount = 0;
    seaChartGridCountDiscovered(reinterpret_cast<HdPadGrid*>(this));
    mDiscoveryChanged = 0;
}

/* 0268153C */
void hdSeaChartGrid_c::applyCellFocusAnimations() {
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->applyFocusAnimation();
}

/* 026815A4 */
void hdSeaChartGrid_c::refreshFocusAnimations() {
    applyCellFocusAnimations();
}

/* 02682950 */
hdSeaChartCell_c* hdSeaChartGrid_c::findFocusedCell() {
    for (u32 index = 0; index < 49; ++index) {
        if (mCells[index]->mFocused)
            return mCells[index];
    }
    return nullptr;
}

/* 026829B4 */
void hdSeaChartGrid_c::updateFocusedCell() {
    hdSeaChartCell_c* focused = findFocusedCell();
    hdSeaChartCell_c* previous = mPreviousFocusedCell;
    mFocusedCell = focused;
    if (focused != previous)
        mFocusChanged = 1;
}

/* 026829F0 */
hdSeaChartCell_c* hdSeaChartGrid_c::findTrackedCell() {
    for (u32 index = 0; index < 49; ++index) {
        if (mCells[index]->mTrackingInput)
            return mCells[index];
    }
    return nullptr;
}

/* 02682A54 */
void hdSeaChartGrid_c::updateTrackedCell() {
    hdSeaChartCell_c* tracked = findTrackedCell();
    hdSeaChartCell_c* previous = mPreviousTrackedCell;
    mTrackedCell = tracked;
    if (tracked != previous) {
        hdSeaChartCell_c* current = mTrackedCell;
        mTrackingChanged = 1;
        if (current)
            seaChartGridPlaySound("SE_MENU_CURSOR");
    }
}

/* 02682C30 */
hdSeaChartCell_c* hdSeaChartGrid_c::findWidgetCell(HdInputReceiver* receiver) {
    if (!receiver)
        return nullptr;
    const u32 serial = reinterpret_cast<hdWidget_c*>(receiver)->mSerial;
    for (u32 index = 0; index < 49; ++index) {
        HdInputReceiver* widget = mCells[index]->mTouchWidget;
        if (u32(reinterpret_cast<hdWidget_c*>(widget)->mSerial) == serial)
            return mCells[index];
    }
    return nullptr;
}

/* Library event state and additional game menu fields absent from the shared geometry view. */
struct hdSeaChartInputActiveSingleton_c {
    gptr<HdPadInputActive> mValue;
};
static hdSeaChartInputActiveSingleton_c sChartGridInputActive;
struct hdSeaChartSecondaryMapView_l {
    u8 mReserved[0x184];
    gptr<void> mMap;
};
WWHD_OFFSET(hdSeaChartSecondaryMapView_l, mMap, 0x184);
void seaChartGridCopyEvent(HdPadEvent* output, HdPadEvent* input);
u32 seaChartGridIsPointerEvent(u32 kind);
u32 seaChartGridIsCancelButton(hdButtonEvent_l* event);
u32 seaChartGridCanChangeInput(hdMapDisplayCoordinator_c* coordinator);
u32 seaChartGridSecondaryAcceptsCancel(void* map);
void seaChartGridBeginNavigation();
void seaChartGridEndNavigation();

/* 02682F0C */
u32 seaChartGridReadInput(hdSeaChartGrid_c* grid) {
    void* manager = sChartGridEventManager.mValue;
    if (!manager)
        return 0;
    Local<HdPadEvent> storage;
    seaChartGridCopyEvent(storage, &reinterpret_cast<HdPadEventSource*>(manager)->mEvent);
    hdButtonEvent_l* event = reinterpret_cast<hdButtonEvent_l*>(storage.get());
    const u32 owner = event->mReceiverId;
    const u32 kind = event->mType;
    const u32 ownId =
        grid->mOwner ? reinterpret_cast<HdPadInputMode*>(grid->mOwner.get())->mId.get() : 0;
    if (owner == ownId)
        return grid->acceptPointInput(kind) ? grid->translateInputKind(kind) : 0;
    if (owner == 0xFFFFFFFF) {
        if (sChartGridInputActive.mValue->mActive) {
            if (seaChartGridIsDirectionalEvent(kind)) {
                HdInputReceiver* current =
                    seaChartGridGetEventReceiver(sChartGridEventManager.mValue);
                HdInputReceiver* target = grid->resolveEventNeighbor(current, kind);
                hdSeaChartMenuMap_l* menu =
                    reinterpret_cast<hdSeaChartMenuMap_l*>(grid->mMapOwner->mIcons.get());
                if (menu && menu->mSelectionMode == 3) {
                    hdButtonReceiver_l* receiver =
                        reinterpret_cast<hdButtonPart_c*>(menu)->mpReceiver;
                    if (receiver == reinterpret_cast<hdButtonReceiver_l*>(target)) {
                        if (!grid->mFocusTransferred) {
                            grid->mFocusTransferred = 1;
                            return 5;
                        }
                    } else if (target && grid->mFocusTransferred) {
                        grid->mFocusTransferred = 0;
                        return 6;
                    }
                    return 0;
                }
                hdSeaChartCell_c* targetCell = grid->findWidgetCell(target);
                hdSeaChartCell_c* currentCell = grid->findWidgetCell(current);
                if (targetCell) {
                    if (!currentCell) {
                        if (!grid->mFocusTransferred) {
                            grid->mPendingInputCell = targetCell;
                            grid->mFocusTransferred = 1;
                            return 5;
                        }
                    } else if (grid->mFocusTransferred) {
                        const u32 result = grid->translateInputKind(kind);
                        seaChartGridSetEventMode(sChartGridEventManager.mValue, target);
                        return result;
                    }
                } else if (currentCell && target && grid->mFocusTransferred) {
                    grid->mFocusTransferred = 0;
                    seaChartGridSetEventMode(sChartGridEventManager.mValue, target);
                    seaChartGridBeginNavigation();
                    return 6;
                }
                return 0;
            }
            if (grid->isButtonEvent(event, 0)) {
                hdSeaChartMenuMap_l* menu =
                    reinterpret_cast<hdSeaChartMenuMap_l*>(grid->mMapOwner->mIcons.get());
                if (menu && menu->mSelectionMode == 3) {
                    HdInputReceiver* current =
                        seaChartGridGetEventReceiver(sChartGridEventManager.mValue);
                    if (!current || reinterpret_cast<hdButtonPart_c*>(menu)->mpReceiver !=
                                        reinterpret_cast<hdButtonReceiver_l*>(current))
                        return 0;
                }
                if (grid->mTrackedCell) {
                    if (!grid->mFocusTransferred)
                        grid->mFocusTransferred = 1;
                    grid->mInputPositionChanged = 1;
                }
                return 0;
            }
            if (grid->isButtonEvent(event, 3)) {
                if (grid->mSelectionMode != 1 && seaChartGridCanChangeInput(chartGridCoordinator()))
                    grid->mInputChanged = 1;
                return 0;
            }
            if (!seaChartGridIsCancelButton(event))
                return 0;
            void* secondary =
                reinterpret_cast<hdSeaChartSecondaryMapView_l*>(sChartGridOwner.mValue.get())->mMap;
            if (!seaChartGridSecondaryAcceptsCancel(secondary) || grid->mSelectionMode == 1)
                return 0;
        } else {
            if (!seaChartGridIsCancelButton(event))
                return 0;
            void* secondary =
                reinterpret_cast<hdSeaChartSecondaryMapView_l*>(sChartGridOwner.mValue.get())->mMap;
            if (!seaChartGridSecondaryAcceptsCancel(secondary))
                return 0;
        }
        if (grid->mFocusTransferred) {
            grid->mFocusTransferred = 0;
            seaChartGridEndNavigation();
            return 6;
        }
        return 0;
    }
    if (!owner)
        return grid->mInputLocked ? 4 : 0;
    if (!seaChartGridIsPointerEvent(kind))
        return 0;
    hdButtonPart_c* menu =
        reinterpret_cast<hdButtonPart_c*>(chartGridCoordinator()->mpSeaMap.get());
    hdButtonPart_c* other = reinterpret_cast<hdButtonPart_c*>(grid->mMapOwner->mIcons.get());
    const u32 entryIndex = u32(menu->mChildren.mNum) > 3 ? 3 : 0;
    hdButtonPart_c* entry =
        reinterpret_cast<hdButtonPart_c*>(menu->mChildren.mpData[entryIndex].get());
    if (!other || !entry || u32(entry->mpReceiver->mId) == owner)
        return 0;
    if (u32(other->mpReceiver->mId) == owner) {
        if (!grid->mFocusTransferred) {
            grid->mFocusTransferred = 1;
            return 5;
        }
    } else if (grid->mFocusTransferred) {
        grid->mFocusTransferred = 0;
        seaChartGridEndNavigation();
        return 6;
    }
    return 0;
}

/* 026832C4 */
void hdSeaChartGrid_c::updateInputKind() {
    mInputKind = seaChartGridReadInput(this);
}

/* 02683760 */
void hdSeaChartGrid_c::finishFrame() {
    const u32 discovered = mDiscoveredCount;
    mRefreshRequested = 0;
    mPreviousDiscoveredCount = discovered;
    hdSeaChartCell_c* focused = mFocusedCell;
    mInputChanged = 0;
    mPreviousFocusedCell = focused;
    hdSeaChartCell_c* pending = mPendingFocus;
    mFocusChanged = 0;
    mPreviousPendingFocus = pending;
    hdSeaChartCell_c* tracked = mTrackedCell;
    mPendingFocusChanged = 0;
    mPreviousTrackedCell = tracked;
    mDiscoveryChanged = 0;
    mInputKind = 0;
    mTrackingChanged = 0;
    mInputPositionChanged = 0;
    for (u32 index = 0; index < 49; ++index)
        mCells[index]->finishInput();
}

/* WWHD glue (tools/verify/autoglue.py)
 data sChartGridVtable 100F92B0 nocheck
ext 02680DE0 HdPadMapCell* hdGridFind(HdPadCellArray*, HdPadCoordinate*)
data sChartGridCoordinateScale 100F9228
data sChartGridHalf 100F9218
data sChartGridDimension 100F921C
data sChartGridEventManager 101F4FF0
ext 02615154 void seaChartGridSetEventMode(void*, HdInputReceiver*)
ext 02615140 HdInputReceiver* seaChartGridGetEventReceiver(void*)
ext 026F621C u32 seaChartGridIsDirectionalEvent(u32)
ext 026F63E0 u32 seaChartGridEventDirection(u32)
ext 02001C00 HdInputReceiver* seaChartGridReceiverUserData(HdInputReceiver*, u32)
data sChartGridInput 101F5088
data sChartGridFallbackPosition 1049FFA0
data sChartGridInputKinds 100F923C nocheck
ext 02615398 u32 seaChartGridEventBusy(void*)
data sChartGridOwner 101F8344
data sChartGridSave 101F84DC
ext 02688440 s32 seaChartGridFindLocation(hdMapDisplayCoordinator_c*, s32, s32)
ext 0268A864 void seaChartGridGetLocation(hdMapDisplayCoordinator_c*, s32, be<s8>*, be<s8>*, be<s16>*, be<s16>*, be<u8>*)
ext 0268A720 hdChartEntry_c* seaChartGridFindSeaChart(hdMapDisplayCoordinator_c*, u32)
ext 0268A7A0 u32 seaChartGridIsChartComplete(hdMapDisplayCoordinator_c*, u32)
ext 026810A8 void seaChartGridBuild(HdPadGrid*)
ext 02006478 hdPartsStateId_c* seaChartGridGetState(hdStateMachine*)
data sChartGridChangeToDungeonState 10496538
data sChartGridBlankState 1049DD60
data sChartGridFocusSuppressionState 10490464
data sChartGridAlternateFocusSuppressionState 10490494
data sChartGridStatics 104961A8
data sChartGridRegistrations 101F626C nocheck
data sChartGridMinimum 100F929C
data sChartGridMaximum 100F92A0
data sChartGridDefaultOrigin 100F92A4
data sChartGridDefaultOffset 100F92A8
ext 028F026C void seaChartGridRegisterStatic(hdSeaChartGridRegistration_c*)
ext 028ED6F8 void seaChartGridInitializeFirstFlag(be<u8>*)
ext 028EAB2C void seaChartGridInitializeSecondFlag(be<u8>*)
data sChartGridZero 100F920C
data sChartGridOffsetX 100F9210
data sChartGridOffsetY 100F9214
data sChartGridUi 1018C404
ext 0273AD10 void* seaChartGridAllocate(u32)
ext 0273B050 void* seaChartGridAllocateOnHeap(u32, hdScreenHeap*, u32)
ext 0267FA74 hdSeaChartCell_c* seaChartGridConstructCell(hdSeaChartCell_c*)
str 100F922C "SE_MENU_CURSOR"
ext 02030B38 void seaChartGridPlaySound(const char*)
ext 02681400 void seaChartGridCountDiscovered(HdPadGrid*)
data sChartGridInputActive 101F8378
ext 020013C0 void seaChartGridCopyEvent(HdPadEvent*, HdPadEvent*)
ext 026F61CC u32 seaChartGridIsPointerEvent(u32)
ext 0263C69C u32 seaChartGridIsCancelButton(hdButtonEvent_l*)
ext 026891F8 u32 seaChartGridCanChangeInput(hdMapDisplayCoordinator_c*)
ext 0268D02C u32 seaChartGridSecondaryAcceptsCancel(void*)
ext 026F6504 void seaChartGridBeginNavigation()
ext 026F6514 void seaChartGridEndNavigation()
ext 0267A204 u32 seaChartGridRestrictedCoordinate(u32)
*/
