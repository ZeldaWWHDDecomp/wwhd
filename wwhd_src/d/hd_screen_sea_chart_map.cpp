/** HD sea-chart map animation and coordinate controls; gates pending. */
#include "d/hd_screen_sea_chart_map.h"
#include "d/hd_parts_text_pane.h"
#include "ppc.h"
using namespace gabi;
struct SeaMapDragView_l { u8 mReserved[0x4D8]; HdPadPoint mDragOffset; };
WWHD_OFFSET(SeaMapDragView_l, mDragOffset, 0x4D8);
/* 0267B644 */
void hdSeaChartMap_c::playOpenAnimation() {
    hdAnimSet_play(reinterpret_cast<hdParts_c*>(this)->animSet(), mOpenAnimation, mAnimationGroup, l_hdMapOne);
}
/* 0267B6C8 */
void hdSeaChartMap_c::playCloseAnimation() {
    hdAnimSet_play(reinterpret_cast<hdParts_c*>(this)->animSet(), mCloseAnimation, mAnimationGroup, l_hdMapOne);
}
/* 0267BA9C */
void* hdSeaChartMap_c::selectedCell() {
    u32 index = mSelectedCell;
    if (index >= u32(mCellCount)) return nullptr;
    return mpCells[index].get();
}
/* 0267BAC4 */
u32 hdSeaChartMap_c::hasMaskedFlag(u32 flags, u32 mask) {
    return u8(flags & mask) != 0;
}
/* 0267BE58 */
f32 hdSeaChartMap_c::interpolationRatio(f32 position) {
    f32 first = mOrigin.x, last = mOrigin.y;
    f32 ratio = l_hdMapOne;
    if (last == first || first > position || position > last) return ratio;
    return f32(f64(f32(position - first)) / f64(f32(last - first)));
}
/* 0267D5D8 */
void hdSeaChartMap_c::setDragOffset(HdPadPoint* offset) {
    auto* view = reinterpret_cast<SeaMapDragView_l*>(this);
    view->mDragOffset.x = offset->x;
    view->mDragOffset.y = offset->y;
}
/* 0267D5EC */
void hdSeaChartMap_c::setInterpolationBounds(f32 first, f32 last) {
    mOrigin.x = first;
    mOrigin.y = last;
}
/* 0267D5F8 */
void hdSeaChartMap_c::setTargetPosition(HdPadPoint* target) {
    mTarget.x = target->x;
    mTarget.y = target->y;
}
/* 0267D60C */
void hdSeaChartMap_c::setZoomY(f32 zoom) { mZoomY = zoom; }
/* 0267E454 */
void hdSeaChartMap_c::setResourceMappings(SeaMapResourceMapping_l* mappings, SeaMapMarkerMapping_l* markers) {
    mpResourceMappings = mappings;
    mpMarkerMappings = markers;
}
/* 0267E460 */
void hdSeaChartMap_c::configureAnimations(u32 cell, u32 first, u32 last, u32 open, u32 close,
    u32 group, u32 scroll, u32 scrollGroup, u32 zoom, u32 zoomGroup) {
    mSelectedCell = cell;
    mScrollGroup = scrollGroup;
    mCloseAnimation = close;
    mLastAnimation = last;
    mZoomGroup = zoomGroup;
    mZoomAnimation = zoom;
    mScrollAnimation = scroll;
    mOpenAnimation = open;
    mAnimationGroup = group;
    mFirstAnimation = first;
}

using SeaMapZero_l = be<f32>;
static SeaMapZero_l sSeaMapZero;
struct SeaMapDragOrigin_l { u8 mReserved[0x4A4]; HdPadPoint mPosition; };
struct SeaMapDragState_l { u8 mReserved[0x4D0]; HdPadPoint mPosition; HdPadPoint mOffset; };
WWHD_OFFSET(SeaMapDragOrigin_l, mPosition, 0x4A4);
WWHD_OFFSET(SeaMapDragState_l, mPosition, 0x4D0);
WWHD_OFFSET(SeaMapDragState_l, mOffset, 0x4D8);
/* 0267B9B4 */
void hdSeaChartMap_c::beginDrag() {
    auto* origin = reinterpret_cast<SeaMapDragOrigin_l*>(this);
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    f32 y = origin->mPosition.y;
    f32 x = origin->mPosition.x;
    drag->mPosition.y = y;
    f32 zero = sSeaMapZero;
    drag->mPosition.x = x;
    drag->mOffset.x = zero;
    drag->mOffset.y = zero;
}
/* 0267B9D8 */
void hdSeaChartMap_c::resetDrag() { beginDrag(); }
/* 0267C91C */
void hdSeaChartMap_c::beginCenteredDrag() {
    auto* origin = reinterpret_cast<SeaMapDragOrigin_l*>(this);
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    f32 angle = mAngle;
    f32 y = origin->mPosition.y;
    f32 zero = sSeaMapZero;
    f32 x = origin->mPosition.x;
    drag->mPosition.y = y;
    drag->mPosition.x = x;
    drag->mOffset.x = zero;
    mZoomX = angle;
    drag->mOffset.y = zero;
}
/* 0267C948 */
void hdSeaChartMap_c::resetCenteredDrag() { beginCenteredDrag(); }
/* 0267C468 */
void hdSeaChartMap_c::updateZoom() { hdMapZoom(this); }
/* 0267C470 */
void hdSeaChartMap_c::recalculateLayout() { reinterpret_cast<hdParts_c*>(this)->vCalcLayout(); }
/* 0267C480 */
void hdSeaChartMap_c::recalculateLayoutAgain() { recalculateLayout(); }
/* 0267C6AC */
void hdSeaChartMap_c::recalculateCenteredLayout() { reinterpret_cast<hdParts_c*>(this)->vCalcLayout(); }
/* 0267C6BC */
void hdSeaChartMap_c::recalculateCenteredLayoutAgain() { recalculateCenteredLayout(); }
extern void seaMapReleaseCellArray(be<u32>*);
extern void seaMapUpdateCell(hdSeaChartMap_c*, u32);
extern void seaMapUpdateZoomY(hdSeaChartMap_c*, f32);
enum SeaMapVirtualSlot { SeaMapUpdateFirst = 0x214 / 4, SeaMapUpdateSecond = 0x21C / 4 };
/* 0267CEF0 */
void hdSeaChartMap_c::releaseCells() {
    if (mpCells) seaMapReleaseCellArray(&mCellCount);
}
/* 0267CF04 */
void hdSeaChartMap_c::releaseLayoutAndCells() {
    reinterpret_cast<hdParts_c*>(this)->vDestroyLayout();
    releaseCells();
}
/* 0267D730 */
void hdSeaChartMap_c::updateMapParts() {
    call_ptr(mVTable[SeaMapUpdateFirst].get(), this);
    call_ptr(mVTable[SeaMapUpdateSecond].get(), this);
}
/* 0267C8C4 */
void hdSeaChartMap_c::updateCenteredPosition() {
    hdMapSetPosition(this, &mTarget, u32(mZoomMode));
    seaMapUpdateZoomY(this, mZoomY);
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267C918 */
void hdSeaChartMap_c::updateCenteredPositionAgain() { updateCenteredPosition(); }
/* 0267BDD4 */
void hdSeaChartMap_c::updateDragPosition() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x20> frame;
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    auto* position = at<HdPadPoint>(frame + 0x10);
    f32 x = f32(drag->mPosition.x) + f32(drag->mOffset.x);
    f32 y = f32(drag->mPosition.y) + f32(drag->mOffset.y);
    position->x = x;
    position->y = y;
    chaseDragPosition(position);
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
    seaMapUpdateCell(this, u32(mZoomMode));
}
/* 0267BE54 */
void hdSeaChartMap_c::updateDragPositionAgain() { updateDragPosition(); }
static hdPartsStateId_c sSeaMapInAll;
static hdPartsStateId_c sSeaMapOutAll;
static hdPartsStateId_c sSeaMapZoomOut;
struct SeaMapOpeningPoints_l { u8 mReserved[0x4C0]; HdPadPoint mStart; HdPadPoint mTarget; };
WWHD_OFFSET(SeaMapOpeningPoints_l, mStart, 0x4C0);
WWHD_OFFSET(SeaMapOpeningPoints_l, mTarget, 0x4C8);
extern void seaMapCompleteAnimation(hdAnimSet*, u32, u32);
enum SeaMapStateUpdateSlot { SeaMapBeforeState = 0x1EC / 4, SeaMapAfterState = 0x1F4 / 4 };
/* 0267CF44 */
void hdSeaChartMap_c::updateState() {
    call_ptr(mVTable[SeaMapBeforeState].get(), this);
    call_ptr(mVTable[SeaMapAfterState].get(), this);
    hdPadStateExecute(&mState);
}
/* 0267CF98 */
void hdSeaChartMap_c::openAll() {
    hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapInAll));
}
/* 0267CFA8 */
void hdSeaChartMap_c::closeAll() {
    hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapOutAll));
}
/* 0267CFB8 */
void hdSeaChartMap_c::finishOpening() {
    hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapZoomOut));
    seaMapCompleteAnimation(reinterpret_cast<hdParts_c*>(this)->animSet(), mOpenAnimation, mAnimationGroup);
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267B948 */
void hdSeaChartMap_c::updateOpening() {
    auto* points = reinterpret_cast<SeaMapOpeningPoints_l*>(this);
    f32 ratio = chaseOpening(&points->mStart, &points->mTarget);
    if (!(f32(l_hdMapOne) > ratio))
        hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapZoomOut));
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267B9B0 */
void hdSeaChartMap_c::updateOpeningAgain() { updateOpening(); }
enum SeaMapZoomUpdateSlot { SeaMapApplyZoom = 0x22C / 4 };
/* 0267C5C8 */
void hdSeaChartMap_c::updateZoomOut() {
    f32 ratio = hdPadZoomRatio(this, u32(mZoomMode), mZoomStart, mScale);
    hdPadZoomPosition(this, &mCurrent, &mTarget, u32(mZoomMode));
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    interpolateDrag(&drag->mPosition, &drag->mOffset, ratio);
    hdPadZoomScale(this, u32(mZoomMode), mZoomX, mZoomY, ratio);
    call_ptr(mVTable[SeaMapApplyZoom].get(), this);
    if (!(f32(l_hdMapOne) > ratio))
        hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapZoomOut));
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267C6A4 */
void hdSeaChartMap_c::updateZoomOutAgain() { updateZoomOut(); }
/* 0267CB38 */
void hdSeaChartMap_c::updatePlayerCentering() {
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    f32 ratio = chaseCenter(&drag->mPosition, &drag->mOffset);
    hdMapSetPosition(this, &mTarget, u32(mZoomMode));
    hdPadZoomScale(this, u32(mZoomMode), mZoomX, mZoomY, ratio);
    if (!(f32(l_hdMapOne) > ratio)) hdPadChangeState(&mState, &l_mapActiveState);
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267CBE4 */
void hdSeaChartMap_c::updatePlayerCenteringAgain() { updatePlayerCentering(); }
/* 0267C52C */
void hdSeaChartMap_c::interpolateDrag(HdPadPoint* start, HdPadPoint* target, f32 ratio) {
    f32 targetX = target->x;
    f32 startX = start->x;
    f32 startY = start->y;
    f32 deltaX = targetX - startX;
    f32 targetY = target->y;
    f32 deltaY = targetY - startY;
    f32 scaledX = fmuls_ppc(deltaX, ratio);
    f32 scaledY = fmuls_ppc(deltaY, ratio);
    f32 x = startX + scaledX;
    f32 y = startY + scaledY;
    auto* origin = reinterpret_cast<SeaMapDragOrigin_l*>(this);
    origin->mPosition.x = x;
    origin->mPosition.y = y;
}
/* 0267B74C */
void hdSeaChartMap_c::updateAnimationState() {
    constexpr u32 updateAnimationSlot = 0x20C / 4;
    call_ptr(mVTable[updateAnimationSlot].get(), this);
}
static hdPartsStateId_c sSeaMapBlank;
using SeaMapUiPointer_l = gptr<hdScreenUiSystem>;
static SeaMapUiPointer_l sSeaMapUiSystem;
extern void seaMapAllocateCellArray(be<u32>*, u32, hdScreenHeap*, u32);
/* 0267B664 */
void hdSeaChartMap_c::pollOpening() {
    if (hdAnimSet_isEnd(reinterpret_cast<hdParts_c*>(this)->animSet(), mOpenAnimation))
        hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapZoomOut));
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267B6E8 */
void hdSeaChartMap_c::pollClosing() {
    if (hdAnimSet_isEnd(reinterpret_cast<hdParts_c*>(this)->animSet(), mCloseAnimation))
        hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapBlank));
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267D018 */
void hdSeaChartMap_c::cancelOpening() {
    hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapBlank));
    hdAnimSet_operationCC(reinterpret_cast<hdParts_c*>(this)->animSet(), mOpenAnimation, mAnimationGroup);
    reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
}
/* 0267DD24 */
void hdSeaChartMap_c::allocateCells(u32 count) {
    releaseCells();
    seaMapAllocateCellArray(&mCellCount, count, sSeaMapUiSystem->mHeap.get(), 4);
}
/* 0267BA44 */
u32 hdSeaChartMap_c::compareBounds(HdPadRect* first, HdPadRect* second) {
    enum Edge { Bottom = 1, Top = 2, Left = 4, Right = 8 };
    u32 flags = 0;
    if (f32(first->mBottom) < f32(second->mBottom)) flags |= Bottom;
    if (f32(second->mTop) < f32(first->mTop)) flags |= Top;
    if (f32(second->mLeft) < f32(first->mLeft)) flags |= Left;
    if (f32(first->mRight) < f32(second->mRight)) flags |= Right;
    return flags;
}
struct SeaMapPointBits_l { be<u32> mX, mY; };
static SeaMapPointBits_l* seaMapPointBits(HdPadPoint* point) { return reinterpret_cast<SeaMapPointBits_l*>(point); }
/* 0267B9DC */
void hdSeaChartMap_c::chaseDragPosition(HdPadPoint* target) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x18> frame;
    auto* origin = reinterpret_cast<SeaMapDragOrigin_l*>(this);
    auto* current = at<HdPadPoint>(frame + 8);
    u32 xBits = seaMapPointBits(&origin->mPosition)->mX;
    u32 yBits = seaMapPointBits(&origin->mPosition)->mY;
    seaMapPointBits(current)->mY = yBits;
    seaMapPointBits(current)->mX = xBits;
    hdMapDisplayChase(current, target, l_hdMapChaseRate, l_hdMapChaseMax, l_hdMapOne);
    f32 x = current->x, y = current->y;
    origin->mPosition.x = x;
    origin->mPosition.y = y;
}
struct SeaMapResourceCatalog_l {
    u8 mReserved[0xC]; be<s32> mCount; be<s32> mMarkerCount;
    u8 mReserved14[0x50F]; be<u8> mFirstIndex;
};
struct SeaMapCatalogOwner_l { u8 mReserved[0x4B8]; gptr<SeaMapResourceCatalog_l> mpCatalog; };
WWHD_OFFSET(SeaMapResourceCatalog_l, mCount, 0xC);
WWHD_OFFSET(SeaMapCatalogOwner_l, mpCatalog, 0x4B8);
/* 0267EB68 */
void* hdSeaChartMap_c::findResourceCell(u32 resource) {
    auto* catalog = reinterpret_cast<SeaMapCatalogOwner_l*>(this)->mpCatalog.get();
    if (!catalog) return nullptr;
    auto* mappings = mpResourceMappings.get();
    if (!mappings) return nullptr;
    s32 count = catalog->mCount;
    if (count <= 0) return nullptr;
    for (u32 i = 0; i < u32(count); ++i) {
        if (u32(mappings[i].mResource) != resource) continue;
        u32 index = mappings[i].mCell;
        if (index >= u32(mCellCount)) return nullptr;
        return mpCells[index].get();
    }
    return nullptr;
}
struct SeaMapTilesView_l { u8 mReserved[0x50]; SeaMapTile_l mTiles[10]; };
struct SeaMapCenterView_l { u8 mReserved[0x49C]; HdPadPoint mCenter; };
WWHD_OFFSET(SeaMapTilesView_l, mTiles, 0x50);
WWHD_OFFSET(SeaMapCenterView_l, mCenter, 0x49C);
enum SeaMapTileSlot { SeaMapGetTile = 0x1C4 / 4 };
/* 0267D614 */
SeaMapTile_l* hdSeaChartMap_c::findTile(u32 index) {
    auto* catalog = reinterpret_cast<SeaMapCatalogOwner_l*>(this)->mpCatalog.get();
    if (!catalog) return nullptr;
    s32 count = catalog->mCount;
    if (count <= 0) return nullptr;
    auto* tiles = reinterpret_cast<SeaMapTilesView_l*>(this);
    for (u32 i = 0; i < u32(count); ++i) {
        auto* tile = &tiles->mTiles[i < 10 ? i : 0];
        if (u32(tile->mIndex) == index) return tile;
    }
    return nullptr;
}
/* 0267D698 */
u32 hdSeaChartMap_c::findLargestTile() {
    constexpr u32 noTile = 10;
    u32 result = noTile;
    auto* catalog = reinterpret_cast<SeaMapCatalogOwner_l*>(this)->mpCatalog.get();
    if (!catalog) return result;
    s32 count = catalog->mCount;
    u32 first = catalog->mFirstIndex;
    for (u32 i = 0; s32(i) < count; ++i) {
        u32 index = u8(first + i);
        auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
        if (tile && f32(mOrigin.x) < f32(tile->mScale)) result = index;
    }
    return result;
}
/* 0267D77C */
u32 hdSeaChartMap_c::findMarker(u32 resource, u32 kind) {
    constexpr u32 noMarker = 0x8C;
    auto* catalog = reinterpret_cast<SeaMapCatalogOwner_l*>(this)->mpCatalog.get();
    if (!catalog) return noMarker;
    auto* mappings = mpMarkerMappings.get();
    if (!mappings) return noMarker;
    s32 count = catalog->mMarkerCount;
    for (u32 i = 0; s32(i) < count; ++i)
        if (u32(mappings[i].mResource) == resource && u32(mappings[i].mKind) == kind)
            return mappings[i].mMarker;
    return noMarker;
}
/* 0267D948 */
void hdSeaChartMap_c::prepareOpeningPoints(HdPadPoint* start, HdPadPoint* target, u32 index) {
    if (!start || !target || index == 10) return;
    auto* center = reinterpret_cast<SeaMapCenterView_l*>(this);
    seaMapPointBits(start)->mX = u32(seaMapPointBits(&center->mCenter)->mX);
    seaMapPointBits(start)->mY = u32(seaMapPointBits(&center->mCenter)->mY);
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
    if (!tile) return;
    seaMapPointBits(target)->mX = u32(seaMapPointBits(&tile->mPosition)->mX);
    target->y = -f32(tile->mPosition.y);
}
struct SeaMapConstructorPrefix_l {
    u8 mReserved[0x50];
    SeaMapTile_l mTiles[10];
    SeaMapTile_l mStandaloneTile;
    HdPadPoint mCenter;
    HdPadPoint mPosition;
    be<f32> mAngle;
    gptr<hdTextPane_c> mpPane;
    be<u32> mPaneMode;
    gptr<SeaMapResourceCatalog_l> mpCatalog;
    be<u8> mZoomMode, mPreviousZoomMode, mScrollMode, mDragMode;
};
WWHD_OFFSET(SeaMapConstructorPrefix_l, mTiles, 0x50);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mStandaloneTile, 0x438);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mCenter, 0x49C);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mPosition, 0x4A4);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mAngle, 0x4AC);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mpPane, 0x4B0);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mPaneMode, 0x4B4);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mpCatalog, 0x4B8);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mZoomMode, 0x4BC);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mPreviousZoomMode, 0x4BD);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mScrollMode, 0x4BE);
WWHD_OFFSET(SeaMapConstructorPrefix_l, mDragMode, 0x4BF);
static hdPartsVtbl_c sSeaMapVtable;
struct SeaMapSecondaryVtable_l { u8 mReserved[4]; };
static SeaMapSecondaryVtable_l sSeaMapSecondaryVtable;
static SeaMapSecondaryVtable_l sSeaMapHolderVtable;
extern void* seaMapAllocate(u32);
extern void seaMapConstructBase(hdParts_c*);
extern void seaMapDestroyBase(hdParts_c*, u32);
extern SeaMapTile_l* seaMapConstructTile(SeaMapTile_l*);
using SeaMapTileConstructor_l = SeaMapTile_l*(*)(SeaMapTile_l*);
extern void seaMapConstructTiles(SeaMapTile_l*, u32, u32, SeaMapTileConstructor_l);
struct SeaMapRatioView_l { u8 mReserved[0x500]; be<f32> mRatio; };
WWHD_OFFSET(SeaMapRatioView_l, mRatio, 0x500);
struct SeaMapCellStorage_l { be<u32> mCount, mCapacity; gptr<gptr<void>> mpData; };
WWHD_SIZE(SeaMapCellStorage_l, 12);
static void seaMapInitializePoint(HdPadPoint* point, f32 zero) {
    if (!point) point = static_cast<HdPadPoint*>(seaMapAllocate(sizeof(HdPadPoint)));
    if (point) { point->y = zero; point->x = zero; }
}
/* 0267CBE8 */
hdSeaChartMap_c* hdSeaChartMap_c::construct(hdSeaChartMap_c* map) {
    if (!map) {
        map = static_cast<hdSeaChartMap_c*>(seaMapAllocate(sizeof(hdSeaChartMap_c)));
        if (!map) return nullptr;
    }
    auto* parts = reinterpret_cast<hdParts_c*>(map);
    seaMapConstructBase(parts);
    parts->mVtable = &sSeaMapVtable;
    parts->mSecondaryVtable = &sSeaMapSecondaryVtable;
    parts->mStateHolder.mHolderVtable = &sSeaMapHolderVtable;
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(map);
    auto* tiles = at<SeaMapTile_l>(ea(prefix->mTiles));
    if (!tiles) tiles = static_cast<SeaMapTile_l*>(seaMapAllocate(sizeof(prefix->mTiles)));
    if (tiles) seaMapConstructTiles(tiles, 10, sizeof(SeaMapTile_l), seaMapConstructTile);
    seaMapConstructTile(&prefix->mStandaloneTile);
    f32 zero = sSeaMapZero;
    seaMapInitializePoint(at<HdPadPoint>(ea(&prefix->mCenter)), zero);
    seaMapInitializePoint(at<HdPadPoint>(ea(&prefix->mPosition)), zero);
    map->mAngle = zero;
    prefix->mpPane = nullptr;
    prefix->mDragMode = 10;
    prefix->mPreviousZoomMode = 10;
    prefix->mpCatalog = nullptr;
    prefix->mScrollMode = 10;
    prefix->mZoomMode = 10;
    prefix->mPaneMode = 0;
    auto* opening = reinterpret_cast<SeaMapOpeningPoints_l*>(map);
    seaMapInitializePoint(at<HdPadPoint>(ea(&opening->mStart)), zero);
    seaMapInitializePoint(at<HdPadPoint>(ea(&opening->mTarget)), zero);
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(map);
    seaMapInitializePoint(at<HdPadPoint>(ea(&drag->mPosition)), zero);
    seaMapInitializePoint(at<HdPadPoint>(ea(&drag->mOffset)), zero);
    map->mScale = zero;
    f32 one = l_hdMapOne;
    map->mZoomStart = zero;
    map->mOrigin.y = one;
    map->mOrigin.x = one;
    seaMapInitializePoint(at<HdPadPoint>(ea(&map->mCurrent)), zero);
    seaMapInitializePoint(at<HdPadPoint>(ea(&map->mTarget)), zero);
    map->mZoomX = zero;
    map->mZoomY = zero;
    reinterpret_cast<SeaMapRatioView_l*>(map)->mRatio = zero;
    seaMapInitializePoint(at<HdPadPoint>(ea(&map->mSavedPosition)), zero);
    seaMapInitializePoint(at<HdPadPoint>(ea(&map->mSavedTarget)), zero);
    map->mPaneNameCount = 0;
    // The embedded array constructor allocates on guest-address wraparound.
    auto* cells = at<SeaMapCellStorage_l>(ea(&map->mCellCount));
    if (!cells) cells = static_cast<SeaMapCellStorage_l*>(seaMapAllocate(sizeof(SeaMapCellStorage_l)));
    if (cells) { cells->mCapacity = 0; cells->mCount = 0; cells->mpData = nullptr; }
    map->mResourcePaneCount = 0;
    map->mMarkerPaneCount = 0;
    map->mpPaneNames = nullptr;
    if (!ea(map->mpResourcePanes)) seaMapAllocate(sizeof(map->mpResourcePanes));
    if (!ea(map->mpMarkerPanes)) seaMapAllocate(sizeof(map->mpMarkerPanes));
    map->mAnimationGroup = 0;
    map->mScrollGroup = 0;
    map->mFirstAnimation = 0;
    map->mpResourceMappings = nullptr;
    map->mSelectedCell = 0;
    map->mLastAnimation = 0;
    map->mpMarkerMappings = nullptr;
    map->mScrollAnimation = 0;
    map->mCloseAnimation = 0;
    map->mOpenAnimation = 0;
    map->mZoomGroup = 0;
    map->mZoomAnimation = 0;
    for (auto& slot : map->mpResourcePanes) slot = 0;
    for (auto& slot : map->mpMarkerPanes) slot = 0;
    return map;
}
using SeaMapMathConstant_l = be<f32>;
static SeaMapMathConstant_l sSeaMapHalf;
static SeaMapMathConstant_l sSeaMapThree;
static SeaMapMathConstant_l sSeaMapOpeningRate;
static SeaMapMathConstant_l sSeaMapOpeningMaximum;
static f32 seaMapVectorLength(f32 squared, f32 zero, f32 half, f32 three) {
    if (!(squared > zero)) return fmuls_ppc(zero, squared);
    f64 estimate = frsqrte(squared);
    f32 squareEstimate = f32(estimate * round25(estimate));
    f32 halfEstimate = f32(estimate * f64(half));
    f32 correction = fnmsubs(squareEstimate, squared, three);
    return fmuls_ppc(fmuls_ppc(correction, halfEstimate), squared);
}
static f32 seaMapChaseRatio(HdPadPoint* position, HdPadPoint* start, HdPadPoint* target) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x50> frame;
    f32 dy = fsubs_ppc(start->y, target->y);
    f32 dx = fsubs_ppc(start->x, target->x);
    f32 squared = fmadds(dx, dx, fmuls_ppc(dy, dy));
    f32 half = sSeaMapHalf;
    f32 zero = sSeaMapZero;
    f32 three = sSeaMapThree;
    f32 length = seaMapVectorLength(squared, zero, half, three);
    f32 one = l_hdMapOne;
    if (length == zero) return one;
    auto* current = at<HdPadPoint>(frame + 8);
    current->y = position->y;
    current->x = position->x;
    hdMapDisplayChase(current, target, sSeaMapOpeningRate, sSeaMapOpeningMaximum, one);
    f32 x = current->x, y = current->y;
    position->x = x;
    position->y = y;
    f32 startY = start->y, targetY = target->y, startX = start->x, targetX = target->x;
    f32 fullY = fsubs_ppc(startY, targetY), fullX = fsubs_ppc(startX, targetX);
    f32 fullSquared = fmadds(fullX, fullX, fmuls_ppc(fullY, fullY));
    f32 denominator = seaMapVectorLength(fullSquared, zero, half, three);
    f32 remainingY = fsubs_ppc(startY, current->y), remainingX = fsubs_ppc(startX, current->x);
    f32 remainingSquared = fmadds(remainingX, remainingX, fmuls_ppc(remainingY, remainingY));
    f32 numerator = seaMapVectorLength(remainingSquared, zero, half, three);
    return f32(f64(numerator) / f64(denominator));
}
/* 0267B75C */
f32 hdSeaChartMap_c::chaseOpening(HdPadPoint* start, HdPadPoint* target) {
    return seaMapChaseRatio(&reinterpret_cast<SeaMapCenterView_l*>(this)->mCenter, start, target);
}
/* 0267C94C */
f32 hdSeaChartMap_c::chaseCenter(HdPadPoint* start, HdPadPoint* target) {
    return seaMapChaseRatio(&reinterpret_cast<SeaMapDragOrigin_l*>(this)->mPosition, start, target);
}
static hdPartsStateId_c sSeaMapScroll;
static hdPartsStateId_c sSeaMapDrag;
static hdPartsStateId_c sSeaMapToZoomIn;
static hdPartsStateId_c sSeaMapToZoomOut;
static hdPartsStateId_c sSeaMapZoomInToCenteringPlayer;
struct SeaMapHeaderWords_l { be<u32> mWords[4]; };
struct SeaMapHeaderLimits_l { be<f32> mMinimumAngle, mMaximumAngle, mWidth, mHeight, mLimitX, mLimitY; };
struct SeaMapRegistration_l { u8 mReserved[12]; };
static SeaMapHeaderWords_l sSeaMapHeaderWords;
static SeaMapHeaderLimits_l sSeaMapHeaderLimits;
static SeaMapRegistration_l sSeaMapFirstRegistration;
static SeaMapRegistration_l sSeaMapSecondRegistration;
static SeaMapRegistration_l sSeaMapThirdRegistration;
using SeaMapByte_l = be<u8>;
static SeaMapByte_l sSeaMapFirstObject;
static SeaMapByte_l sSeaMapSecondObject;
static SeaMapMathConstant_l sSeaMapMinimumAngle;
static SeaMapMathConstant_l sSeaMapMaximumAngle;
static SeaMapMathConstant_l sSeaMapInitialExtent;
static SeaMapMathConstant_l sSeaMapInitialLimit;
static hdPartsStateIdVtbl_c sSeaMapStateVtable;
static hdStateId sSeaMapBlankParent;
static hdStateId sSeaMapNullState;
static u32 sSeaMapStateGuard;
static u32 sSeaMapNextState;
extern void seaMapRegisterGlobal(SeaMapRegistration_l*);
extern void seaMapConstructFirstObject(be<u8>*);
extern void seaMapConstructSecondObject(be<u8>*);
/* 0267EDC0 */
static void __sinit_hd_screen_sea_chart_map_cpp() {
    sSeaMapHeaderWords.mWords[3] = 0;
    sSeaMapHeaderWords.mWords[2] = 0;
    sSeaMapHeaderWords.mWords[1] = 0;
    sSeaMapHeaderWords.mWords[0] = 0;
    seaMapRegisterGlobal(&sSeaMapFirstRegistration);
    f32 minimum = sSeaMapMinimumAngle, maximum = sSeaMapMaximumAngle;
    sSeaMapHeaderLimits.mMinimumAngle = minimum;
    sSeaMapHeaderLimits.mMaximumAngle = maximum;
    seaMapConstructFirstObject(&sSeaMapFirstObject);
    seaMapRegisterGlobal(&sSeaMapSecondRegistration);
    seaMapConstructSecondObject(&sSeaMapSecondObject);
    seaMapRegisterGlobal(&sSeaMapThirdRegistration);
    f32 extent = sSeaMapInitialExtent;
    u32 guard = sSeaMapStateGuard;
    sSeaMapHeaderLimits.mWidth = extent;
    f32 limit = sSeaMapInitialLimit;
    sSeaMapHeaderLimits.mHeight = extent;
    sSeaMapHeaderLimits.mLimitX = limit;
    sSeaMapHeaderLimits.mLimitY = limit;
    u32 number;
    if (guard) number = sSeaMapNextState;
    else { number = 0; sSeaMapStateGuard = 1; }
    sSeaMapBlank.setup(number + 1, "::StateID_Blank", &sSeaMapStateVtable, 4, &sSeaMapBlankParent);
    sSeaMapInAll.setup(number + 2, "::StateID_InAll", &sSeaMapStateVtable, 0x19, &sSeaMapNullState);
    sSeaMapOutAll.setup(number + 3, "::StateID_OutAll", &sSeaMapStateVtable, 0x1C, &sSeaMapNullState);
    sSeaMapScroll.setup(number + 4, "::StateID_Scroll", &sSeaMapStateVtable, 0x1F, &sSeaMapNullState);
    sSeaMapDrag.setup(number + 5, "::StateID_Drag", &sSeaMapStateVtable, 0x22, &sSeaMapNullState);
    sSeaMapToZoomIn.setup(number + 6, "::StateID_ToZoomIn", &sSeaMapStateVtable, 0x25, &sSeaMapNullState);
    reinterpret_cast<hdPartsStateId_c*>(&l_mapWaitState)->setup(number + 7, "::StateID_ZoomIn", &sSeaMapStateVtable, 0x28, &sSeaMapNullState);
    sSeaMapToZoomOut.setup(number + 8, "::StateID_ToZoomOut", &sSeaMapStateVtable, 0x2B, &sSeaMapNullState);
    sSeaMapZoomOut.setup(number + 9, "::StateID_ZoomOut", &sSeaMapStateVtable, 0x2E, &sSeaMapNullState);
    reinterpret_cast<hdPartsStateId_c*>(&l_mapActiveState)->setup(number + 10, "::StateID_ZoomInCenteringPlayer", &sSeaMapStateVtable, 0x31, &sSeaMapNullState);
    sSeaMapZoomInToCenteringPlayer.setup(number + 11, "::StateID_ZoomInToCenteringPlayer", &sSeaMapStateVtable, 0x34, &sSeaMapNullState);
    sSeaMapNextState = number + 11;
}
enum SeaMapPreparationSlot { SeaMapResetForZoom = 0x204 / 4 };
/* 0267BE94 */
void hdSeaChartMap_c::prepareZoomOut() {
    call_ptr(mVTable[SeaMapResetForZoom].get(), this);
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, u32(mZoomMode));
    if (!tile) return;
    f32 scale = mOrigin.y;
    f32 tileScale = tile->mScale;
    f32 zero = sSeaMapZero;
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    mScale = scale;
    drag->mOffset.y = zero;
    drag->mOffset.x = zero;
    mCurrent.x = zero;
    drag->mPosition.y = zero;
    mZoomStart = tileScale;
    drag->mPosition.x = zero;
    mCurrent.y = zero;
    f32 ratio = interpolationRatio(tileScale);
    mZoomX = zero;
    reinterpret_cast<SeaMapRatioView_l*>(this)->mRatio = ratio;
}
/* 0267BF2C */
void hdSeaChartMap_c::prepareZoomOutAgain() { prepareZoomOut(); }
/* 0267C484 */
void hdSeaChartMap_c::prepareZoomIn() {
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, u32(mZoomMode));
    if (!tile) return;
    f32 tileScale = tile->mScale;
    f32 scale = mOrigin.x;
    mZoomStart = tileScale;
    mScale = scale;
    u32 cellX = seaMapPointBits(&tile->mCurrent)->mX;
    f32 zero = sSeaMapZero;
    seaMapPointBits(&mCurrent)->mX = cellX;
    u32 cellY = seaMapPointBits(&tile->mCurrent)->mY;
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    drag->mOffset.x = zero;
    seaMapPointBits(&mCurrent)->mY = cellY;
    mTarget.x = zero;
    auto* origin = reinterpret_cast<SeaMapDragOrigin_l*>(this);
    u32 x = seaMapPointBits(&origin->mPosition)->mX;
    mTarget.y = zero;
    u32 y = seaMapPointBits(&origin->mPosition)->mY;
    drag->mOffset.y = zero;
    seaMapPointBits(&drag->mPosition)->mY = y;
    seaMapPointBits(&drag->mPosition)->mX = x;
    f32 ratio = interpolationRatio(tileScale);
    f32 angle = mAngle;
    reinterpret_cast<SeaMapRatioView_l*>(this)->mRatio = ratio;
    mZoomX = angle;
    mZoomY = zero;
}
/* 0267C528 */
void hdSeaChartMap_c::prepareZoomInAgain() { prepareZoomIn(); }
/* 0267DCD0 */
void hdSeaChartMap_c::updateScrollMode() {
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    u32 current = mZoomMode;
    if (current != u32(prefix->mPreviousZoomMode)) {
        prefix->mScrollMode = current;
        hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapScroll));
        current = mZoomMode;
    }
    prefix->mPreviousZoomMode = current;
}
/* 0267D9C8 */
void hdSeaChartMap_c::initializeOpeningPosition() {
    call_ptr(mVTable[SeaMapResetForZoom].get(), this);
    auto* points = reinterpret_cast<SeaMapOpeningPoints_l*>(this);
    prepareOpeningPoints(&points->mStart, &points->mTarget, u32(mZoomMode));
    f32 x = points->mTarget.x, y = points->mTarget.y;
    u32 mode = mZoomMode;
    f32 zero = sSeaMapZero;
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    prefix->mPreviousZoomMode = mode;
    reinterpret_cast<SeaMapRatioView_l*>(this)->mRatio = zero;
    prefix->mCenter.x = x;
    prefix->mCenter.y = y;
    mAngle = zero;
    hdPadChangeState(&mState, reinterpret_cast<HdPadState*>(&sSeaMapZoomOut));
}
/* the layout's root pane and the selected cell are SRT panes; the code stores their translation and rotation as floats */
static inline be<f32>& paneTranslationX(hdPartsPaneSRT_c* pane) { return *reinterpret_cast<be<f32>*>(&pane->mTranslationXBits); }
static inline be<f32>& paneTranslationY(hdPartsPaneSRT_c* pane) { return *reinterpret_cast<be<f32>*>(&pane->mTranslationYBits); }
static inline be<f32>& paneTranslationZ(hdPartsPaneSRT_c* pane) { return *reinterpret_cast<be<f32>*>(&pane->mTranslationZBits); }
static inline be<f32>& paneRotationX(hdPartsPaneSRT_c* pane) { return *reinterpret_cast<be<f32>*>(&pane->mRotationXBits); }
static inline be<f32>& paneRotationY(hdPartsPaneSRT_c* pane) { return *reinterpret_cast<be<f32>*>(&pane->mRotationYBits); }
static inline be<f32>& paneRotationZ(hdPartsPaneSRT_c* pane) { return *reinterpret_cast<be<f32>*>(&pane->mRotationZBits); }
extern void seaMapTextPanePosition(HdPadPoint*, hdTextPane_c*);
extern void seaMapUpdateBaseLayout(hdParts_c*);
/* 0267D414 */
void hdSeaChartMap_c::updateRootPane() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x18> frame;
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->mAccessor->mObject.get());
    if (!root) return;
    auto* pane = reinterpret_cast<SeaMapConstructorPrefix_l*>(this)->mpPane.get();
    if (!pane) return;
    auto* position = at<HdPadPoint>(frame + 8);
    seaMapTextPanePosition(position, pane);
    f32 y = position->y;
    u32 flags = root->mFlags;
    f32 zero = sSeaMapZero;
    f32 x = position->x;
    paneTranslationZ(root) = zero;
    paneTranslationX(root) = x;
    paneTranslationY(root) = y;
    root->mFlags = flags | 0x10;
}
/* 0267D488 */
void hdSeaChartMap_c::updatePaneAndLayout() {
    updateRootPane();
    constexpr u32 updatePaneSlot = 0x1BC / 4;
    call_ptr(mVTable[updatePaneSlot].get(), this);
    seaMapUpdateBaseLayout(reinterpret_cast<hdParts_c*>(this));
}
extern void seaMapDestroyTile(SeaMapTile_l*, u32);
using SeaMapTileDestructor_l = void(*)(SeaMapTile_l*, u32);
extern void seaMapDestroyTiles(SeaMapTile_l*, u32, u32, SeaMapTileDestructor_l, u32, u32);
extern void seaMapDelete(void*);
/* 0267B5AC */
void hdSeaChartMap_c::destroy(hdSeaChartMap_c* map, u32 flags) {
    if (!map) return;
    auto* parts = reinterpret_cast<hdParts_c*>(map);
    parts->mStateHolder.mHolderVtable = &sSeaMapHolderVtable;
    parts->mSecondaryVtable = &sSeaMapSecondaryVtable;
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(map);
    seaMapDestroyTile(&prefix->mStandaloneTile, 2);
    seaMapDestroyTiles(prefix->mTiles, 10, sizeof(SeaMapTile_l), seaMapDestroyTile, 0, 0);
    seaMapDestroyBase(parts, 0);
    if (flags & 1) seaMapDelete(map);
}
extern hdPartsStateId_c* seaMapCurrentState(HdPadStateController*);
static bool seaMapMatchesState(HdPadStateController* machine, hdPartsStateId_c* expected) {
    auto* current = seaMapCurrentState(machine);
    auto* expectedVtable = expected->mVtable.get();
    u32 actualKey = current->getKey();
    u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
    return actualKey == expectedKey;
}
/* 0267D1F4 */
u32 hdSeaChartMap_c::isZoomedIn() {
    if (seaMapMatchesState(&mState, reinterpret_cast<hdPartsStateId_c*>(&l_mapWaitState))) return 1;
    return seaMapMatchesState(&mState, reinterpret_cast<hdPartsStateId_c*>(&l_mapActiveState));
}
/* 0267E3B0 */
void hdSeaChartMap_c::stopCellAnimations() {
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    for (u32 i = 0; i < u32(mFirstAnimation); ++i)
        hdAnimSet_operationCC(parts->animSet(), u32(mZoomAnimation) + i, u32(mZoomGroup) + i);
    for (u32 i = 0; i < u32(mFirstAnimation); ++i)
        hdAnimSet_operationCC(parts->animSet(), u32(mScrollAnimation) + i, u32(mScrollGroup) + i);
}
static SeaMapMathConstant_l sSeaMapScaleStep;
extern f32 seaMapChaseScale(be<f32>*, f32, f32, f32, f32);
/* 0267BF30 */
f32 hdSeaChartMap_c::chaseTileScale(u32 index, f32 start, f32 target) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x48> frame;
    f32 result = sSeaMapZero;
    if (f32(start - target) == result) return l_hdMapOne;
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
    if (!tile) return result;
    auto* scale = at<be<f32>>(frame + 8);
    f32 current = tile->mScale;
    f32 step = sSeaMapScaleStep;
    f32 rate = sSeaMapOpeningRate;
    f32 maximum = sSeaMapHalf;
    *scale = current;
    seaMapChaseScale(scale, target, maximum, rate, step);
    f32 storedScale = *scale;
    tile->mScale = storedScale;
    f32 updated = *scale;
    result = f32(f32(updated - start) / f32(target - start));
    f32 ratio = interpolationRatio(storedScale);
    reinterpret_cast<SeaMapRatioView_l*>(this)->mRatio = ratio;
    return result;
}
/* 0267C068 */
f32 hdSeaChartMap_c::interpolateTilePosition(HdPadPoint* start, HdPadPoint* target, u32 index, f32 ratio) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x58> frame;
    f32 result = sSeaMapZero;
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
    if (!tile) return result;
    f32 targetX = target->x, startX = start->x, startY = start->y;
    f32 differenceX = f32(targetX - startX);
    f32 targetY = target->y;
    auto* difference = at<HdPadPoint>(frame + 0x10);
    auto* point = at<HdPadPoint>(frame + 8);
    difference->x = differenceX;
    seaMapPointBits(point)->mX = u32(seaMapPointBits(difference)->mX);
    f32 differenceY = f32(targetY - startY);
    f32 dx = point->x;
    difference->y = differenceY;
    seaMapPointBits(point)->mY = u32(seaMapPointBits(difference)->mY);
    f32 scaledX = f32(dx * ratio);
    f32 scaledY = f32(f32(point->y) * ratio);
    auto* scaled = at<HdPadPoint>(frame + 0x18);
    scaled->x = scaledX;
    u32 scaledXBits = seaMapPointBits(scaled)->mX;
    scaled->y = scaledY;
    u32 scaledYBits = seaMapPointBits(scaled)->mY;
    auto* offset = at<HdPadPoint>(frame + 0x28);
    seaMapPointBits(offset)->mX = scaledXBits;
    f32 offsetX = offset->x;
    seaMapPointBits(offset)->mY = scaledYBits;
    f32 offsetY = offset->y;
    f32 x = f32(startX + offsetX), y = f32(startY + offsetY);
    auto* finalPoint = at<HdPadPoint>(frame + 0x20);
    finalPoint->x = x;
    u32 xBits = seaMapPointBits(finalPoint)->mX;
    finalPoint->y = y;
    u32 yBits = seaMapPointBits(finalPoint)->mY;
    seaMapPointBits(point)->mX = xBits;
    f32 finalX = point->x;
    seaMapPointBits(point)->mY = yBits;
    tile->mCurrent.x = finalX;
    tile->mCurrent.y = f32(point->y);
    return ratio;
}
struct SeaMapTileOffset_l { u8 mReserved[0x20]; HdPadPoint mOffset; };
WWHD_OFFSET(SeaMapTileOffset_l, mOffset, 0x20);
/* 0267E498 */
void hdSeaChartMap_c::resetTileOffsets() {
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    auto* catalog = prefix->mpCatalog.get();
    f32 zero = sSeaMapZero;
    prefix->mDragMode = 10;
    mCurrent.y = zero;
    mCurrent.x = zero;
    mScale = zero;
    mZoomStart = zero;
    auto* drag = reinterpret_cast<SeaMapDragState_l*>(this);
    drag->mOffset.y = zero;
    drag->mOffset.x = zero;
    drag->mPosition.y = zero;
    drag->mPosition.x = zero;
    auto* points = reinterpret_cast<SeaMapOpeningPoints_l*>(this);
    points->mTarget.y = zero;
    points->mTarget.x = zero;
    points->mStart.y = zero;
    points->mStart.x = zero;
    mZoomX = zero;
    s32 count = catalog->mCount;
    u32 index = catalog->mFirstIndex;
    if (count <= 0) return;
    do {
        auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index & 0xFF);
        auto* offset = reinterpret_cast<SeaMapTileOffset_l*>(tile);
        offset->mOffset.x = zero;
        --count;
        ++index;
        offset->mOffset.y = zero;
    } while (count != 0);
}
using SeaMapConversionBias_l = be<f64>;
static SeaMapConversionBias_l sSeaMapConversionBias;
static SeaMapMathConstant_l sSeaMapAngleUnits;
static SeaMapMathConstant_l sSeaMapHalfTurn;
static SeaMapMathConstant_l sSeaMapAngleStep;
static SeaMapMathConstant_l sSeaMapAngleRate;
extern void seaMapChaseAngle(be<s32>*, be<s32>*, be<s32>*, be<s32>*, f32);
/* 0267C7A4 */
void hdSeaChartMap_c::chaseRotation(f32 target) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x48> frame;
    f32 units = sSeaMapAngleUnits;
    f32 scaledTarget = f32(target * units);
    f32 halfTurn = sSeaMapHalfTurn;
    f32 step = sSeaMapAngleStep;
    f32 unitStep = f32(units / halfTurn);
    f32 angle = mAngle;
    f32 scaledStep = f32(step * units);
    f32 scaledAngle = f32(angle * units);
    f32 stepUnits = f32(scaledStep / halfTurn);
    auto* temporary = at<be<u32>>(frame + 8);
    *temporary = u32(ppc_fctiwz(stepUnits));
    u32 stepBits = *temporary;
    auto* maximum = at<be<s32>>(frame + 0x14);
    *maximum = s32(stepBits);
    *temporary = u32(ppc_fctiwz(unitStep));
    u32 minimumBits = *temporary;
    f32 angleUnits = f32(scaledAngle / halfTurn);
    auto* minimum = at<be<s32>>(frame + 0x18);
    *minimum = s32(minimumBits);
    *temporary = u32(ppc_fctiwz(angleUnits));
    f32 targetUnits = f32(scaledTarget / halfTurn);
    f32 rate = sSeaMapAngleRate;
    u32 angleBits = *temporary;
    *temporary = u32(ppc_fctiwz(targetUnits));
    u32 targetBits = *temporary;
    auto* current = at<be<s32>>(frame + 0x10);
    auto* goal = at<be<s32>>(frame + 0x1C);
    *current = s32(angleBits);
    *goal = s32(targetBits);
    seaMapChaseAngle(current, goal, maximum, minimum, rate);
    s32 updated = *current;
    *temporary = 0x43300000;
    *at<be<u32>>(frame + 0xC) = u32(updated) ^ 0x80000000;
    f64 encoded = *at<be<f64>>(frame + 8);
    f64 bias = sSeaMapConversionBias;
    f32 value = f32(encoded - bias);
    mAngle = f32(f32(value * halfTurn) / units);
}
/* 0267D2E0 */
u32 hdSeaChartMap_c::isZoomedOrZoomingOut() {
    if (seaMapMatchesState(&mState, reinterpret_cast<hdPartsStateId_c*>(&l_mapWaitState))) return 1;
    if (seaMapMatchesState(&mState, reinterpret_cast<hdPartsStateId_c*>(&l_mapActiveState))) return 1;
    return seaMapMatchesState(&mState, &sSeaMapZoomOut);
}
struct SeaMapMarkerEntry_l { be<s8> mKind; u8 mReserved; be<u8> mActive; u8 mPadding; };
struct SeaMapMarkerGroup_l {
    be<u8> mResource; u8 mReserved[3]; be<s32> mCount; SeaMapMarkerEntry_l mEntries[14];
};
struct SeaMapMarkerCatalog_l {
    be<u8> mShowInactive; u8 mReserved[0x13]; SeaMapMarkerGroup_l mGroups[10];
};
WWHD_SIZE(SeaMapMarkerEntry_l, 4);
WWHD_SIZE(SeaMapMarkerGroup_l, 0x40);
WWHD_OFFSET(SeaMapMarkerCatalog_l, mGroups, 0x14);
/* 0267D7E4 */
void hdSeaChartMap_c::updateMarkerAnimations() {
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    auto* catalog = prefix->mpCatalog.get();
    if (!catalog) return;
    s32 count = catalog->mCount;
    if (count <= 0) return;
    u32 groupIndex = 0;
    do {
        auto* markerCatalog = reinterpret_cast<SeaMapMarkerCatalog_l*>(catalog);
        auto* group = &markerCatalog->mGroups[groupIndex < 10 ? groupIndex : 0];
        s32 entries = group->mCount;
        if (entries > 0) {
            u32 entryIndex = 0;
            do {
                auto* entry = &group->mEntries[entryIndex < 14 ? entryIndex : 0];
                s32 kind = entry->mKind;
                u32 marker = findMarker(group->mResource, u32(kind));
                if (marker != 0x8C) {
                    u32 scrollGroup = u32(mScrollGroup) + marker;
                    u32 zoomAnimation = u32(mZoomAnimation) + marker;
                    u32 zoomGroup = u32(mZoomGroup) + marker;
                    u32 scrollAnimation = u32(mScrollAnimation) + marker;
                    auto* parts = reinterpret_cast<hdParts_c*>(this);
                    auto* animations = parts->animSet();
                    if (entry->mActive) {
                        seaMapCompleteAnimation(animations, scrollAnimation, scrollGroup);
                        seaMapCompleteAnimation(parts->animSet(), zoomAnimation, zoomGroup);
                    } else {
                        hdAnimSet_operationCC(animations, scrollAnimation, scrollGroup);
                        hdAnimSet_operationCC(parts->animSet(), zoomAnimation, zoomGroup);
                        auto* currentCatalog = reinterpret_cast<SeaMapMarkerCatalog_l*>(prefix->mpCatalog.get());
                        if (currentCatalog->mShowInactive)
                            seaMapCompleteAnimation(parts->animSet(), scrollAnimation, scrollGroup);
                    }
                }
                --entries;
                ++entryIndex;
            } while (entries != 0);
        }
        --count;
        ++groupIndex;
        if (count) catalog = prefix->mpCatalog.get();
    } while (count != 0);
}
/* 0267D078 */
u32 hdSeaChartMap_c::isScrollingOrZoomed() {
    if (seaMapMatchesState(&mState, &sSeaMapScroll)) return 1;
    if (seaMapMatchesState(&mState, reinterpret_cast<hdPartsStateId_c*>(&l_mapWaitState))) return 1;
    if (seaMapMatchesState(&mState, reinterpret_cast<hdPartsStateId_c*>(&l_mapActiveState))) return 1;
    return seaMapMatchesState(&mState, &sSeaMapZoomOut);
}
/* 0267DD78 */
void hdSeaChartMap_c::findCellPanes() {
    for (u32 i = 0; s32(i) < s32(mPaneNameCount); ++i) {
        auto* parts = reinterpret_cast<hdParts_c*>(this);
        auto* accessor = parts->mpLayout->mAccessor.get();
        auto* name = &mpPaneNames[i];
        name->assureTermination();
        auto* pane = accessor->mObject->findPane(reinterpret_cast<const char*>(name->mText.get()));
        if (pane && s32(mCellCount) < s32(mCellCapacity)) {
            mpCells[u32(mCellCount)] = pane;
            mCellCount = u32(mCellCount) + 1;
        }
    }
}
extern void seaMapInitializeTiles(hdSeaChartMap_c*);
extern void seaMapResetTile(SeaMapTile_l*);
/* 0267E2B0 */
void hdSeaChartMap_c::initializeCellPanes() {
    allocateCells(mPaneNameCount);
    findCellPanes();
    seaMapInitializeTiles(this);
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    for (u32 i = 0; i < 10; ++i) seaMapResetTile(&prefix->mTiles[i]);
    seaMapResetTile(&prefix->mStandaloneTile);
    constexpr u32 resetSlot = 0x1FC / 4;
    call_ptr(mVTable[resetSlot].get(), this);
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    hdAnimSet_operationCC(parts->animSet(), mOpenAnimation, mAnimationGroup);
    parts->vCalcLayout();
    parts->startState(&sSeaMapBlank);
}
struct SeaMapTileRotation_l { u8 mReserved[0x34]; be<f32> mRotation; };
WWHD_OFFSET(SeaMapTileRotation_l, mRotation, 0x34);
extern void seaMapUpdateTile(SeaMapTile_l*);
/* 0267D4D0 */
void hdSeaChartMap_c::updateTileTransforms() {
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    auto* catalog = prefix->mpCatalog.get();
    if (!catalog) return;
    s32 count = catalog->mCount;
    if (count > 0) {
        u32 index = 0;
        do {
            seaMapUpdateTile(&prefix->mTiles[index < 10 ? index : 0]);
            --count;
            ++index;
        } while (count != 0);
    }
    f32 angle = mAngle;
    f32 y = prefix->mPosition.y;
    reinterpret_cast<SeaMapTileRotation_l*>(&prefix->mStandaloneTile)->mRotation = angle;
    f32 x = prefix->mPosition.x;
    prefix->mStandaloneTile.mCurrent.y = y;
    prefix->mStandaloneTile.mCurrent.x = x;
    auto* pane = static_cast<hdPartsPaneSRT_c*>(selectedCell());
    if (!pane) return;
    f32 rotation = mAngle;
    f32 zero = sSeaMapZero;
    u8 flags = pane->mFlags;
    paneRotationY(pane) = zero;
    paneRotationX(pane) = zero;
    paneRotationZ(pane) = rotation;
    pane->mFlags = flags | 0x10;
    pane = static_cast<hdPartsPaneSRT_c*>(selectedCell());
    y = prefix->mPosition.y;
    x = prefix->mPosition.x;
    paneTranslationZ(pane) = zero;
    flags = pane->mFlags;
    paneTranslationY(pane) = y;
    paneTranslationX(pane) = x;
    pane->mFlags = flags | 0x10;
}
/* 0267E56C */
void hdSeaChartMap_c::prepareScrollTiles() {
    call_ptr(mVTable[SeaMapResetForZoom].get(), this);
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    auto* points = reinterpret_cast<SeaMapOpeningPoints_l*>(this);
    prepareOpeningPoints(&points->mStart, &points->mTarget, prefix->mScrollMode);
    constexpr u32 largestTileSlot = 0x1CC / 4;
    u32 selected = call_ptr<u32>(mVTable[largestTileSlot].get(), this);
    if (selected == 10) {
        prefix->mDragMode = 10;
        return;
    }
    prefix->mDragMode = selected;
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, selected);
    if (!tile) return;
    f32 zero = sSeaMapZero;
    f32 scale = mOrigin.x;
    mTarget.y = zero;
    reinterpret_cast<SeaMapRatioView_l*>(this)->mRatio = zero;
    mTarget.x = zero;
    mZoomY = zero;
    f32 ratio = interpolationRatio(scale);
    auto* catalog = prefix->mpCatalog.get();
    reinterpret_cast<SeaMapRatioView_l*>(this)->mRatio = ratio;
    mAngle = zero;
    s32 count = catalog->mCount;
    u32 index = catalog->mFirstIndex;
    if (count > 0) {
        do {
            tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index & 0xFF);
            if (tile) {
                f32 tileScale = mOrigin.x;
                tile->mCurrent.x = zero;
                tile->mScale = tileScale;
                tile->mCurrent.y = zero;
            }
            --count;
            ++index;
        } while (count != 0);
    }
    prefix->mPosition.y = zero;
    prefix->mPosition.x = zero;
}
struct SeaMapPaneDimensions_l { u8 mReserved[0x34]; be<f32> mScaleX; u8 mReserved38[4]; HdPadPoint mSize; };
WWHD_OFFSET(SeaMapPaneDimensions_l, mScaleX, 0x34);
WWHD_OFFSET(SeaMapPaneDimensions_l, mSize, 0x3C);
extern void seaMapInitializeTilePane(SeaMapTile_l*);
extern HdPadPoint* seaMapTileSize(SeaMapTile_l*);
/* 0267E6F8 */
void hdSeaChartMap_c::bindTilePanes() {
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    if (!prefix->mpCatalog) return;
    for (u32 i = 0; i < 10; ++i) seaMapResetTile(&prefix->mTiles[i]);
    auto* standalone = &prefix->mStandaloneTile;
    seaMapResetTile(standalone);
    auto* catalog = prefix->mpCatalog.get();
    u32 first = catalog->mFirstIndex;
    s32 count = catalog->mCount;
    auto* pane = static_cast<SeaMapPaneDimensions_l*>(selectedCell());
    if (pane) {
        standalone->mpPane = pane;
        standalone->mIndex = 10;
        f32 scale = pane->mScaleX;
        standalone->mpOwner = this;
        standalone->mBaseScale = scale;
        standalone->mSize.x = pane->mSize.x;
        standalone->mSize.y = pane->mSize.y;
    }
    if (count <= 0) return;
    u32 i = 0;
    do {
        constexpr u32 findPaneSlot = 0x224 / 4;
        u32 index = u8(first + i);
        auto* cellPane = call_ptr<void*>(mVTable[findPaneSlot].get(), this, index);
        if (cellPane) {
            auto* tile = &prefix->mTiles[i < 10 ? i : 0];
            tile->mpPane = cellPane;
            seaMapInitializeTilePane(tile);
            tile->mIndex = index;
            f32 scale = mOrigin.x;
            tile->mBaseScale = scale;
            tile->mpOwner = this;
            auto* text = prefix->mpPane.get();
            if (text) tile->mpTextPane = text;
            if (pane) {
                tile->mpParent = standalone;
                auto* size = seaMapTileSize(standalone);
                tile->mSize.x = size->x;
                tile->mSize.y = size->y;
            }
        }
        --count;
        ++i;
    } while (count != 0);
}
static SeaMapMathConstant_l sSeaMapFullTurn;
static SeaMapConversionBias_l sSeaMapAngleThreshold;
extern void seaMapChaseAngleStep(be<s32>*, be<s32>*, be<s32>*);
/* 0267C188 */
f32 hdSeaChartMap_c::interpolateTileRotation(u32 index, f32 start, f32 target, f32 ratio) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x60> frame;
    f32 zero = sSeaMapZero;
    f32 normalizedStart = start, normalizedTarget = target;
    f32 turn = sSeaMapFullTurn;
    if (start < zero) normalizedStart = f32(normalizedStart + turn);
    if (target < zero) normalizedTarget = f32(normalizedTarget + turn);
    f32 difference = f32(normalizedTarget - normalizedStart);
    f32 distance = __builtin_fabsf(difference);
    f64 threshold = sSeaMapAngleThreshold;
    if (threshold < f64(distance)) distance = f32(distance - turn);
    f32 step = __builtin_fabsf(f32(distance * ratio));
    f32 units = sSeaMapAngleUnits;
    f32 scaledStart = f32(start * units);
    f32 halfTurn = sSeaMapHalfTurn;
    f32 startUnits = f32(scaledStart / halfTurn);
    f32 stepUnits = f32(f32(step * units) / halfTurn);
    auto* temporary = at<be<u32>>(frame + 8);
    *temporary = u32(ppc_fctiwz(stepUnits));
    f32 scaledTarget = f32(target * units);
    u32 stepBits = *temporary;
    *temporary = u32(ppc_fctiwz(startUnits));
    auto* maximum = at<be<s32>>(frame + 0x14);
    *maximum = s32(stepBits);
    f32 targetUnits = f32(scaledTarget / halfTurn);
    u32 startBits = *temporary;
    auto* current = at<be<s32>>(frame + 0x10);
    *current = s32(startBits);
    *temporary = u32(ppc_fctiwz(targetUnits));
    u32 targetBits = *temporary;
    auto* goal = at<be<s32>>(frame + 0x18);
    *goal = s32(targetBits);
    seaMapChaseAngleStep(current, goal, maximum);
    s32 updated = *current;
    *temporary = 0x43300000;
    *at<be<u32>>(frame + 0xC) = u32(updated) ^ 0x80000000;
    f64 encoded = *at<be<f64>>(frame + 8);
    f64 bias = sSeaMapConversionBias;
    f32 value = f32(encoded - bias);
    mAngle = f32(f32(value * halfTurn) / units);
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
    if (tile) {
        auto* position = at<HdPadPoint>(frame + 0x1C);
        seaMapPointBits(position)->mX = u32(seaMapPointBits(&tile->mCurrent)->mX);
        seaMapPointBits(position)->mY = u32(seaMapPointBits(&tile->mCurrent)->mY);
        f32 scale = tile->mScale;
        f32 x = -f32(f32(position->x) / scale);
        f32 y = -f32(f32(position->y) / scale);
        tile->mOffset.x = x;
        tile->mOffset.y = y;
    }
    return ratio;
}
static SeaMapMathConstant_l sSeaMapTileSpacing;
/* 0267E934 */
void hdSeaChartMap_c::arrangeTilePositions() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x98> frame;
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    if (!prefix->mpCatalog) return;
    f32 zero = sSeaMapZero;
    auto* catalog = prefix->mpCatalog.get();
    prefix->mTiles[0].mPosition.y = zero;
    prefix->mTiles[0].mPosition.x = zero;
    s32 count = catalog->mCount;
    if (count > 1) {
        f32 spacing = sSeaMapTileSpacing;
        f32 half = sSeaMapHalf;
        u32 currentIndex = 1, previousIndex = 0;
        u32 remaining = u32(count) - 1;
        do {
            auto* previous = &prefix->mTiles[previousIndex < 10 ? previousIndex : 0];
            auto* current = &prefix->mTiles[currentIndex < 10 ? currentIndex : 0];
            auto* previousSize = seaMapTileSize(previous);
            f32 previousY = previousSize->y;
            auto* currentSize = seaMapTileSize(current);
            f32 currentY = currentSize->y;
            f32 distance = fmuls_ppc(fmadds(previousY, half, fmuls_ppc(currentY, half)), spacing);
            auto* point = at<HdPadPoint>(frame + 8);
            seaMapPointBits(point)->mX = u32(seaMapPointBits(&previous->mPosition)->mX);
            seaMapPointBits(point)->mY = u32(seaMapPointBits(&previous->mPosition)->mY);
            f32 y = point->y;
            f32 x = point->x;
            f32 nextY = f32(y + distance);
            current->mPosition.x = x;
            current->mPosition.y = nextY;
            ++previousIndex;
            ++currentIndex;
        } while (--remaining);
    }
    mAngle = zero;
    prefix->mCenter.x = zero;
    prefix->mPosition.x = zero;
    prefix->mPosition.y = zero;
    prefix->mCenter.y = zero;
}
/* 0267EBE0 */
void hdSeaChartMap_c::arrangeTileOffsets() {
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    auto* catalog = prefix->mpCatalog.get();
    u32 first = catalog->mFirstIndex;
    s32 count = catalog->mCount;
    constexpr u32 largestTileSlot = 0x1CC / 4;
    u32 selected = call_ptr<u32>(mVTable[largestTileSlot].get(), this);
    f32 zero = sSeaMapZero;
    if (selected == 10) {
        if (count <= 0) return;
        do {
            auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, first & 0xFF);
            if (tile) { tile->mCurrent.y = zero; tile->mCurrent.x = zero; }
            --count;
            ++first;
        } while (count != 0);
        return;
    }
    f32 halfGap = zero, center = zero;
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, selected);
    if (tile) {
        auto* size = seaMapTileSize(tile);
        f32 height = size->y;
        f32 baseHeight = tile->mSize.y;
        f32 difference = fsubs_ppc(height, baseHeight);
        f32 half = sSeaMapHalf;
        halfGap = fmuls_ppc(difference, half);
        center = tile->mCurrent.y;
    }
    if (count <= 0) return;
    f32 upper = fadds_ppc(halfGap, center), lower = fsubs_ppc(center, halfGap);
    u32 i = 0;
    do {
        u32 index = u8(first + i);
        tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
        if (tile) {
            if (index < selected) { tile->mCurrent.y = lower; tile->mCurrent.x = zero; }
            else if (index > selected) { tile->mCurrent.x = zero; tile->mCurrent.y = upper; }
        }
        --count;
        ++i;
    } while (count != 0);
}
struct SeaMapMarkerSelection_l { u8 mReserved[3]; be<u8> mEnabled; };
struct SeaMapMarkerRefresh_l {
    u8 mReserved[0x514]; gptr<SeaMapMarkerSelection_l> mpSelection;
    u8 mReserved518[5]; be<s8> mKind; u8 mReserved51E[2];
    be<u8> mChanged, mSelectionChanged;
};
struct SeaMapInactiveRefresh_l { u8 mReserved[6]; be<u8> mRefresh; };
WWHD_OFFSET(SeaMapMarkerRefresh_l, mpSelection, 0x514);
WWHD_OFFSET(SeaMapMarkerRefresh_l, mKind, 0x51D);
WWHD_OFFSET(SeaMapMarkerRefresh_l, mChanged, 0x520);
WWHD_OFFSET(SeaMapMarkerRefresh_l, mSelectionChanged, 0x521);
/* 0267DA4C */
void hdSeaChartMap_c::refreshMarkerAnimations() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x68> frame;
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    auto* catalog = prefix->mpCatalog.get();
    if (!catalog) return;
    auto* refresh = reinterpret_cast<SeaMapMarkerRefresh_l*>(catalog);
    if (!refresh->mpSelection) return;
    f32 one = l_hdMapOne;
    if ((refresh->mChanged || refresh->mSelectionChanged) && refresh->mpSelection->mEnabled) {
        s32 count = catalog->mCount;
        u32 first = catalog->mFirstIndex;
        if (count > 0) {
            do {
                refresh = reinterpret_cast<SeaMapMarkerRefresh_l*>(catalog);
                s32 kind = refresh->mKind;
                u32 marker = findMarker(first & 0xFF, u32(kind));
                if (marker != 0x8C) {
                    u32 scrollGroup = u32(mScrollGroup) + marker;
                    bool showInactive = reinterpret_cast<SeaMapMarkerCatalog_l*>(catalog)->mShowInactive;
                    u32 scrollAnimation = u32(mScrollAnimation) + marker;
                    u32 zoomGroup = u32(mZoomGroup) + marker;
                    u32 zoomAnimation = u32(mZoomAnimation) + marker;
                    auto* parts = reinterpret_cast<hdParts_c*>(this);
                    auto* animations = parts->animSet();
                    if (showInactive) {
                        seaMapCompleteAnimation(animations, scrollAnimation, scrollGroup);
                        hdAnimSet_play(parts->animSet(), zoomAnimation, zoomGroup, one);
                    } else {
                        seaMapCompleteAnimation(animations, zoomAnimation, zoomGroup);
                        hdAnimSet_play(parts->animSet(), scrollAnimation, scrollGroup, one);
                    }
                    catalog = prefix->mpCatalog.get();
                }
                --count;
                ++first;
            } while (count != 0);
        }
    }
    if (!reinterpret_cast<SeaMapInactiveRefresh_l*>(catalog)->mRefresh) return;
    s32 count = catalog->mCount;
    if (count <= 0) return;
    u32 i = 0;
    do {
        auto* groups = reinterpret_cast<SeaMapMarkerCatalog_l*>(catalog);
        auto* group = &groups->mGroups[i < 10 ? i : 0];
        s32 entries = group->mCount;
        if (entries > 0) {
            u32 j = 0;
            do {
                auto* entry = &group->mEntries[j < 14 ? j : 0];
                if (!entry->mActive) {
                    auto* word = at<be<u32>>(frame + 8);
                    *word = *reinterpret_cast<be<u32>*>(entry);
                    u32 packed = *word;
                    u32 resource = (packed >> 16) & 0xFF;
                    u32 kind = u32(s32(packed) >> 24);
                    u32 marker = findMarker(resource, kind);
                    if (marker != 0x8C) {
                        u32 zoomGroup = u32(mZoomGroup) + marker;
                        u32 scrollGroup = u32(mScrollGroup) + marker;
                        u32 zoomAnimation = u32(mZoomAnimation) + marker;
                        u32 scrollAnimation = u32(mScrollAnimation) + marker;
                        auto* parts = reinterpret_cast<hdParts_c*>(this);
                        hdAnimSet_operationCC(parts->animSet(), zoomAnimation, zoomGroup);
                        hdAnimSet_play(parts->animSet(), scrollAnimation, scrollGroup, one);
                    }
                }
                --entries;
                ++j;
            } while (entries != 0);
        }
        --count;
        ++i;
        if (count) catalog = prefix->mpCatalog.get();
    } while (count != 0);
}
struct SeaMapTileBounds_l { u8 mReserved[0x48]; HdPadRect mBounds; };
WWHD_OFFSET(SeaMapTileBounds_l, mBounds, 0x48);
extern void seaMapPaneBounds(HdPadRect*, void*);
extern void seaMapUpdateTileBounds(SeaMapTile_l*);
static void seaMapCopyRectWords(HdPadRect* target, HdPadRect* source) {
    auto* out = reinterpret_cast<be<u32>*>(target);
    auto* in = reinterpret_cast<be<u32>*>(source);
    for (u32 i = 0; i < 4; ++i) out[i] = u32(in[i]);
}
/* 0267BAD8 */
void hdSeaChartMap_c::clampToTile(u32 index) {
    AutoFrame automatic{Activation::current};
    NativeFrame<0x68> frame;
    auto* tile = call_ptr<SeaMapTile_l*>(mVTable[SeaMapGetTile].get(), this, index);
    if (!tile) return;
    f32 zero = sSeaMapZero;
    auto* viewport = at<HdPadRect>(frame + 0x18);
    viewport->mLeft = zero;
    viewport->mBottom = zero;
    viewport->mTop = zero;
    viewport->mRight = zero;
    auto* prefix = reinterpret_cast<SeaMapConstructorPrefix_l*>(this);
    seaMapPaneBounds(viewport, prefix->mpPane.get());
    seaMapUpdateTileBounds(tile);
    auto* snapshot = at<HdPadRect>(frame + 0x40);
    seaMapCopyRectWords(snapshot, &reinterpret_cast<SeaMapTileBounds_l*>(tile)->mBounds);
    auto* main = at<HdPadRect>(frame + 0x28);
    main->mTop = zero;
    main->mLeft = zero;
    main->mRight = zero;
    main->mBottom = zero;
    void* pane = selectedCell();
    seaMapPaneBounds(main, pane);
    auto* bounds = at<HdPadRect>(frame + 8);
    seaMapCopyRectWords(bounds, snapshot);
    f32 left = bounds->mLeft, mainLeft = main->mLeft;
    if (left > mainLeft) bounds->mLeft = mainLeft;
    f32 top = bounds->mTop, mainTop = main->mTop;
    if (top > mainTop) bounds->mTop = mainTop;
    f32 right = bounds->mRight, mainRight = main->mRight;
    f32 mainBottom = main->mBottom, bottom = bounds->mBottom;
    if (right < mainRight) bounds->mRight = mainRight;
    if (bottom < mainBottom) bounds->mBottom = mainBottom;
    f32 half = sSeaMapHalf;
    auto* center = at<HdPadPoint>(frame + 0x38);
    center->x = f32(f32(f32(main->mLeft) + f32(main->mRight)) * half);
    center->y = f32(f32(f32(main->mTop) + f32(main->mBottom)) * half);
    f32 boundsX = f32(f32(f32(bounds->mLeft) + f32(bounds->mRight)) * half);
    f32 boundsY = f32(f32(f32(bounds->mTop) + f32(bounds->mBottom)) * half);
    u32 flags = compareBounds(bounds, viewport);
    if (hasMaskedFlag(flags, 1)) {
        f32 viewportHeight = f32(f32(viewport->mBottom) - f32(viewport->mTop));
        f32 height = f32(f32(bounds->mBottom) - f32(bounds->mTop));
        f32 offset = f32(f32(center->y) - boundsY);
        f32 excess = fmsubs(height, half, f32(viewportHeight * half));
        prefix->mPosition.y = f32(offset - excess);
    } else if (hasMaskedFlag(flags, 2)) {
        f32 viewportHeight = f32(f32(viewport->mBottom) - f32(viewport->mTop));
        f32 height = f32(f32(bounds->mBottom) - f32(bounds->mTop));
        f32 offset = f32(f32(center->y) - boundsY);
        f32 excess = fmsubs(height, half, f32(viewportHeight * half));
        prefix->mPosition.y = f32(excess + offset);
    }
    if (hasMaskedFlag(flags, 4)) {
        f32 viewportWidth = f32(f32(viewport->mRight) - f32(viewport->mLeft));
        f32 width = f32(f32(bounds->mRight) - f32(bounds->mLeft));
        f32 offset = f32(f32(center->x) - boundsX);
        f32 excess = fmsubs(width, half, f32(viewportWidth * half));
        prefix->mPosition.x = f32(excess + offset);
    } else if (hasMaskedFlag(flags, 8)) {
        f32 viewportWidth = f32(f32(viewport->mRight) - f32(viewport->mLeft));
        f32 width = f32(f32(bounds->mRight) - f32(bounds->mLeft));
        f32 offset = f32(f32(center->x) - boundsX);
        f32 excess = fmsubs(width, half, f32(viewportWidth * half));
        prefix->mPosition.x = f32(offset - excess);
    }
    if (flags) {
        constexpr u32 updateTileSlot = 0x22C / 4;
        call_ptr(mVTable[updateTileSlot].get(), this);
        reinterpret_cast<hdParts_c*>(this)->vCalcLayout();
    }
}
struct SeaMapFixedName_l { gptr<be<u8>> mText; gptr<hdUiStringTable> mVtable; be<s32> mCapacity; be<u8> mBuffer[48]; };
static hdUiStringTable sSeaMapLocalStringVtable;
static hdUiStringTable sSeaMapFixedStringVtable;
extern void seaMapFormatName(SeaMapFixedName_l*, const char*, be<u8>*, u32);
extern void seaMapDestroyName(hdUiSafeString*, u32);
using SeaMapNameDestructor_l = void(*)(hdUiSafeString*, u32);
extern void seaMapDestroyNames(hdUiSafeString*, u32, u32, SeaMapNameDestructor_l, u32, u32);
static void seaMapInitializeName(hdUiSafeString* name, const char* text) {
    if (!name) name = static_cast<hdUiSafeString*>(seaMapAllocate(sizeof(hdUiSafeString)));
    if (name) { name->mVtable = &sSeaMapLocalStringVtable; name->mText.v = guest_addr(text); }
}
/* 0267DE38 */
void hdSeaChartMap_c::findMarkerPanes() {
    AutoFrame automatic{Activation::current};
    NativeFrame<0xD0> frame;
    auto* up = at<hdUiSafeString>(frame + 0x44);
    auto* down = at<hdUiSafeString>(frame + 0x7C);
    up[0].mText.v = guest_addr("N_B2Up_");
    up[0].mVtable = &sSeaMapLocalStringVtable;
    seaMapInitializeName(&up[1], "N_B1Up_");
    seaMapInitializeName(&up[2], "N_1FUp_");
    seaMapInitializeName(&up[3], "N_2FUp_");
    seaMapInitializeName(&up[4], "N_3FUp_");
    seaMapInitializeName(&up[5], "N_4FUp_");
    seaMapInitializeName(&up[6], "N_OutdoorUp_");
    down[0].mText.v = guest_addr("N_B2Down_");
    down[0].mVtable = &sSeaMapLocalStringVtable;
    seaMapInitializeName(&down[1], "N_B1Down_");
    seaMapInitializeName(&down[2], "N_1FDown_");
    seaMapInitializeName(&down[3], "N_2FDown_");
    seaMapInitializeName(&down[4], "N_3FDown_");
    seaMapInitializeName(&down[5], "N_4FDown_");
    seaMapInitializeName(&down[6], "N_OutdoorDown_");
    auto* name = at<SeaMapFixedName_l>(frame + 8);
    name->mText = name->mBuffer;
    mResourcePaneCount = 0;
    name->mBuffer[0] = 0;
    name->mVtable = &sSeaMapFixedStringVtable;
    name->mCapacity = 48;
    name->mBuffer[47] = 0;
    mMarkerPaneCount = 0;
    for (u32 group = 0; group < 7; ++group) {
        for (u32 index = 0; index < 8; ++index) {
            up[group].assureTermination();
            seaMapFormatName(name, "%s%02d", up[group].mText.get(), index);
            auto* parts = reinterpret_cast<hdParts_c*>(this);
            auto* accessor = parts->mpLayout->mAccessor.get();
            call_ptr(name->mVtable->mAssureTermination.get(), name);
            auto* pane = accessor->mObject->findPane(reinterpret_cast<const char*>(name->mText.get()));
            if (pane) {
                u32 count = mResourcePaneCount;
                mpResourcePanes[count < 16 ? count : 0] = pane;
                mResourcePaneCount = u32(mResourcePaneCount) + 1;
            }
        }
    }
    for (u32 group = 0; group < 7; ++group) {
        for (u32 index = 0; index < 8; ++index) {
            down[group].assureTermination();
            seaMapFormatName(name, "%s%02d", down[group].mText.get(), index);
            auto* parts = reinterpret_cast<hdParts_c*>(this);
            auto* accessor = parts->mpLayout->mAccessor.get();
            call_ptr(name->mVtable->mAssureTermination.get(), name);
            auto* pane = accessor->mObject->findPane(reinterpret_cast<const char*>(name->mText.get()));
            if (pane) {
                u32 count = mMarkerPaneCount;
                mpMarkerPanes[count < 16 ? count : 0] = pane;
                mMarkerPaneCount = u32(mMarkerPaneCount) + 1;
            }
        }
    }
    seaMapDestroyNames(down, 7, 8, seaMapDestroyName, 0, 0);
    seaMapDestroyNames(up, 7, 8, seaMapDestroyName, 0, 0);
}
/* WWHD glue (tools/verify/autoglue.py)
data sSeaMapZero 100F8D1C
ext 0273B5DC void seaMapReleaseCellArray(be<u32>*)
ext 0267BAD8 void seaMapUpdateCell(hdSeaChartMap_c*, u32)
ext 0267C7A4 void seaMapUpdateZoomY(hdSeaChartMap_c*, f32)
data sSeaMapInAll 10495F70
data sSeaMapOutAll 10495FA0
data sSeaMapZoomOut 104960C0
ext 02005708 void seaMapCompleteAnimation(hdAnimSet*, u32, u32)
data sSeaMapBlank 10495F40
data sSeaMapUiSystem 1018C404
ext 0273B584 void seaMapAllocateCellArray(be<u32>*, u32, hdScreenHeap*, u32)
data sSeaMapVtable 100F8F18
data sSeaMapSecondaryVtable 100F8F08
data sSeaMapHolderVtable 100F8EF8
ext 0273AD10 void* seaMapAllocate(u32)
ext 0267F49C SeaMapTile_l* seaMapConstructTile(SeaMapTile_l*) callback
ext 028EFFD0 void seaMapConstructTiles(SeaMapTile_l*, u32, u32, SeaMapTileConstructor_l)
data sSeaMapHalf 100F8D20
data sSeaMapThree 100F8D24
data sSeaMapOpeningRate 100F8D28
data sSeaMapOpeningMaximum 100F8D2C
data sSeaMapScroll 10495FD0
data sSeaMapDrag 10496000
data sSeaMapToZoomIn 10496030
data sSeaMapToZoomOut 10496090
data sSeaMapZoomInToCenteringPlayer 10496120
data sSeaMapHeaderWords 10495F30
data sSeaMapHeaderLimits 10495F14
data sSeaMapFirstRegistration 101F6200
data sSeaMapSecondRegistration 101F620C
data sSeaMapThirdRegistration 101F6218
data sSeaMapFirstObject 10495F2C
data sSeaMapSecondObject 10495F2D
data sSeaMapMinimumAngle 100F8DFC
data sSeaMapMaximumAngle 100F8E00
data sSeaMapInitialExtent 100F8E04
data sSeaMapInitialLimit 100F8E08
data sSeaMapStateVtable 100F8CCC
data sSeaMapBlankParent 1049DD60 nocheck
data sSeaMapNullState 101FF32C nocheck
data sSeaMapStateGuard 101FD8E4 nocheck
data sSeaMapNextState 101FDD50 nocheck
ext 028F026C void seaMapRegisterGlobal(SeaMapRegistration_l*)
ext 028ED6F8 void seaMapConstructFirstObject(be<u8>*)
ext 028EAB2C void seaMapConstructSecondObject(be<u8>*)
str 100F8E0C "::StateID_Blank" in 0267EDC0
str 100F8E1C "::StateID_InAll" in 0267EDC0
str 100F8E98 "::StateID_OutAll" in 0267EDC0
str 100F8EAC "::StateID_Scroll" in 0267EDC0
str 100F8EC0 "::StateID_Drag" in 0267EDC0
str 100F8ED0 "::StateID_ToZoomIn" in 0267EDC0
str 100F8EE4 "::StateID_ZoomIn" in 0267EDC0
str 100F8E4C "::StateID_ToZoomOut" in 0267EDC0
str 100F8E60 "::StateID_ZoomOut" in 0267EDC0
str 100F8E2C "::StateID_ZoomInCenteringPlayer" in 0267EDC0
str 100F8E74 "::StateID_ZoomInToCenteringPlayer" in 0267EDC0
ext 02704D2C void seaMapTextPanePosition(HdPadPoint*, hdTextPane_c*)
ext 026F8CD0 void seaMapUpdateBaseLayout(hdParts_c*)
ext 0267F684 void seaMapDestroyTile(SeaMapTile_l*, u32) callback
ext 028F0164 void seaMapDestroyTiles(SeaMapTile_l*, u32, u32, SeaMapTileDestructor_l, u32, u32)
ext 0273AF40 void seaMapDelete(void*)
ext 02006478 hdPartsStateId_c* seaMapCurrentState(HdPadStateController*)
ext 026F89E8 void seaMapConstructBase(hdParts_c*)
ext 026F88F0 void seaMapDestroyBase(hdParts_c*, u32)
ext 020063C0 void hdPadChangeState(HdPadStateController*, HdPadState*)
ext 02006364 void hdPadStateExecute(HdPadStateController*)
ext 02689BA0 f32 hdMapDisplayChase(HdPadPoint*, HdPadPoint*, f32, f32, f32)
ext 0267BF30 f32 hdPadZoomRatio(HdPadMap*, u32, f32, f32)
ext 0267C068 void hdPadZoomPosition(HdPadMap*, HdPadPoint*, HdPadPoint*, u32)
ext 0267C188 void hdPadZoomScale(HdPadMap*, u32, f32, f32, f32)
ext 02655CE8 u32 hdPadMapMode(void*)
data sSeaMapScaleStep 100F8D38
ext 02689934 f32 seaMapChaseScale(be<f32>*, f32, f32, f32, f32)
data sSeaMapConversionBias 100F8D48
data sSeaMapAngleUnits 10140730
data sSeaMapHalfTurn 1014072C
data sSeaMapAngleStep 100F8D50
data sSeaMapAngleRate 100F8D54
ext 0271F6F4 void seaMapChaseAngle(be<s32>*, be<s32>*, be<s32>*, be<s32>*, f32)
ext 0267DE38 void seaMapInitializeTiles(hdSeaChartMap_c*)
ext 0267F604 void seaMapResetTile(SeaMapTile_l*)
ext 0267F6B8 void seaMapUpdateTile(SeaMapTile_l*)
ext 0267F824 void seaMapInitializeTilePane(SeaMapTile_l*)
ext 0267F7F4 HdPadPoint* seaMapTileSize(SeaMapTile_l*)
data sSeaMapFullTurn 100F8D3C
data sSeaMapAngleThreshold 100F8D40
ext 0271F738 void seaMapChaseAngleStep(be<s32>*, be<s32>*, be<s32>*)
data sSeaMapTileSpacing 100F8DF8
ext 02689528 void seaMapPaneBounds(HdPadRect*, void*)
ext 0267F974 void seaMapUpdateTileBounds(SeaMapTile_l*)
data sSeaMapLocalStringVtable 100F8C24
data sSeaMapFixedStringVtable 100F8C6C
ext 02759C28 void seaMapFormatName(SeaMapFixedName_l*, const char*, be<u8>*, u32)
ext 0267F2FC void seaMapDestroyName(hdUiSafeString*, u32) callback
ext 028F0164 void seaMapDestroyNames(hdUiSafeString*, u32, u32, SeaMapNameDestructor_l, u32, u32)
str 100F8D88 "%s%02d" in 0267DE38
str 100F8D58 "N_B2Up_" in 0267DE38
str 100F8D60 "N_B1Up_" in 0267DE38
str 100F8D68 "N_1FUp_" in 0267DE38
str 100F8D70 "N_2FUp_" in 0267DE38
str 100F8D78 "N_3FUp_" in 0267DE38
str 100F8D80 "N_4FUp_" in 0267DE38
str 100F8DD8 "N_OutdoorUp_" in 0267DE38
str 100F8D90 "N_B2Down_" in 0267DE38
str 100F8D9C "N_B1Down_" in 0267DE38
str 100F8DA8 "N_1FDown_" in 0267DE38
str 100F8DB4 "N_2FDown_" in 0267DE38
str 100F8DC0 "N_3FDown_" in 0267DE38
str 100F8DCC "N_4FDown_" in 0267DE38
str 100F8DE8 "N_OutdoorDown_" in 0267DE38
*/
