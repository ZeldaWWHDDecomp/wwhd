/* HD pause-menu sea chart states and control. Private, game-derived source. */
#include "d/hd_screen_menu_map_states.h"
#include "d/hd_screen_menu_miiverse.h"
static hdPartsVtbl_c sMenuMapVtable;
static HdPadActivePointer sMenuMapActiveInput;
static u32 sMapArrowIndices[2] = {0x0,0x1};
static hdPartsStateId_c sMenuMapChangeToNoDataState;
static hdPartsStateId_c sChartViewDisplayState;
static hdPartsStateId_c sChartViewBlankState;
static hdPartsStateId_c sMenuMapHelpfulOutAllState;
struct sMenuMapSaveSlot_l { gptr<hdMenuMapSave_l> mpObject; };
static sMenuMapSaveSlot_l sMenuMapSave;
static hdPartsStateId_c sMenuMapNoDataState;
static hdMenuChartKindTable_l sChartKindTable;
static u8 sChartKindFrames[5] = {0x0,0x2,0x3,0x1,0x2};
static u32 sMenuMapDisplayFlags;
struct sMenuMapInputManagerSlot_l { gptr<void> mpObject; };
static sMenuMapInputManagerSlot_l sMenuMapInputManager;
static u16 sChartCountFormat[7] = {0x25,0x63,0x25,0x30,0x32,0x64,0};
static u16 sTreasureCountFormat[7] = {0x25,0x63,0x25,0x30,0x32,0x64,0};
static u8 sMenuMapHolderVtable;
static u8 sMenuMapSecondaryVtable;
static hdPartsStateId_c sMenuMapBlankState;
static HdPadState sMenuMapActiveState;
static HdPadState sMenuMapInactiveState;
static HdPadState sMenuMapSwitchState;
static HdPadState sChartViewWaitState;
static HdPadState sChartViewHelpfulState;
static hdPartsStateId_c* hdMenuMapDisplayState() { return (hdPartsStateId_c*)&l_menuMapWaitState; }
static hdPartsStateId_c sMenuMapHelpfulDisplayState;
static hdPartsStateId_c sMenuMapChangeToDisplayState;
struct hdMenuEventOwner_l { u8 mPadding[0x18]; be<u32> mPending; };
struct sMenuEventOwnerSlot_l { gptr<hdMenuEventOwner_l> mpObject; };
static sMenuEventOwnerSlot_l sMenuEventOwner;
struct hdMenuMapButtonIndices { be<u32> mIndices[4]; };
static hdMenuMapButtonIndices sMapButtonIndices;
HdPadInputMode* hdMenuMapInputMode(void*);
HdPadMapCell* hdMenuMapFindReceiverCell(HdPadGrid*,HdPadInputMode*);
hdMenuMapPlay_l* hdMenuMapPlay();
void hdMenuMapChaseAngle(be<s32>*,be<s32>*,be<s32>*,be<s32>*,f32);
void hdMenuMapConsumeEvent(hdMenuMap_c*);
void hdMenuMapHandleEvent(hdMenuMap_c*);
void hdMenuMapMarkDiscovered(void*,u32);
u32 hdMenuMapCellReceiver(HdPadGrid*,HdPadCoordinate*);
void hdMenuMapSetCellReceiver(HdPadMapCell*,u32,u32);
HdPadMapCell* hdMenuMapFindCell(HdPadGrid*,HdPadCoordinate*);
u32 hdMenuMapEventBit(void*,u32);
void hdMenuMapResetUi(HdPadUi*);
void hdMenuMapRefreshUi(HdPadUi*);
void hdMenuMapRefreshReceivers(hdMenuMap_c*);
void hdMenuMapUpdateHint(hdMenuMap_c*);
void hdMenuMapUpdateMap(hdMenuMap_c*);
void hdMenuMapConnect(void*,u32,u32);
void hdMenuMapResetCellReceivers(HdPadGrid*);
void hdMenuMapConnectCellReceivers(HdPadGrid*);
hdMenuChartInfo_l* hdMenuMapChartInfo(HdPadUi*,u32);
u32 hdMenuMapChartCollected(HdPadUi*,u32);
u32 hdMenuMapChartDecoded(HdPadUi*,u32);
u32 hdMenuMapChartComplete(HdPadUi*,u32);
void hdMenuMapSetInputDefault(void*,u32);
u32 hdMenuMapGetWindAngle();
u32 hdMenuMapTotalCharts(HdPadUi*);
u32 hdMenuMapHasIsland(HdPadUi*);
u32 hdMenuMapChartKind(hdMenuMap_c*,u32);
void hdMenuMapFormatCount(hdMenuCountString_l*,be<u16>*,u32,u32);
void hdMenuMapUpdateChartDetails(hdMenuMap_c*,u32,u32);
u32 hdMenuMapButtonActive(hdParts_c*);
void hdMenuMapUpdateInput(hdMenuMap_c*);
hdPartsStateId_c* hdMenuMapCurrentState(hdStateMachine*);
void hdMenuMapPrepareDisplay(hdMenuMap_c*);
void hdMenuMapHideUi(HdPadUi*);
void hdMenuMapResetCells(HdPadGrid*);
u32 hdMenuMapOwnedCharts(HdPadUi*);
u32 hdMenuMapOwnedTreasures(HdPadUi*);
u32 hdMenuMapIslandDiscovered(HdPadUi*, u32);
u32 hdMenuMapChartOwned(HdPadUi*, u32);
u32 hdMenuMapUiBusy(HdPadUi*);
void hdMenuMapSetCaption(void*, u32);
void hdMenuMapEnableAnimation(hdAnimSet*,u32,u32);
void hdMenuMapUpdateChartIcon(hdMenuMap_c*,u32,u32);
void hdMenuMapUpdateChartFrame(hdMenuMap_c*,u32,u32);
void hdMenuMapSetButtonReceiver(hdParts_c*,u32,u32);
u32 hdMenuMapGetButtonReceiver(hdParts_c*);
void hdMenuMapUpdateIsland(hdMenuMap_c*);

/* 026985C4 */
static u32 hdMenuMapStateKey(hdPartsStateId_c* state) { return state->mNumber; }
/* 02698850 */
void hdMenuMap_c::updateOwnedCharts() {
    u32 count = hdMenuMapOwnedCharts(l_hdMenuMapUi.mOwner->mUi);
    mOwnedCharts = count;
    mOwnedChartsChanged = 0;
    if (count != mPreviousOwnedCharts) { mOwnedChartsChanged = 1; count = mOwnedCharts; }
    mPreviousOwnedCharts = count;
}
/* 026988B0 */
void hdMenuMap_c::updateOwnedTreasures() {
    u32 count = hdMenuMapOwnedTreasures(l_hdMenuMapUi.mOwner->mUi);
    mOwnedTreasures = count;
    mOwnedTreasuresChanged = 0;
    if (count != mPreviousOwnedTreasures) { mOwnedTreasuresChanged = 1; count = mOwnedTreasures; }
    mPreviousOwnedTreasures = count;
}
/* 02698910 */
void hdMenuMap_c::updateDiscoveredIslands() {
    u32 count = 0;
    for (u32 i = 0; i < HD_PAD_GRID_CELLS; ++i)
        if (hdMenuMapIslandDiscovered(l_hdMenuMapUi.mOwner->mUi,i)) ++count;
    mDiscoveredIslands = count;
    mDiscoveredIslandsChanged = 0;
    if (count != mPreviousDiscoveredIslands) { mDiscoveredIslandsChanged = 1; count = mDiscoveredIslands; }
    mPreviousDiscoveredIslands = count;
}
/* 02698998 */
s32 hdMenuMap_c::findChart(s32 chart, s32 step) {
    enum { CHART_COUNT = 61 };
    if (u32(chart) >= CHART_COUNT) return chart;
    if (step == 0) return hdMenuMapChartOwned(l_hdMenuMapUi.mOwner->mUi,chart) ? chart : -1;
    if (step != 1 && step != -1) return chart;
    s32 start = chart;
    do {
        HdPadUi* ui = l_hdMenuMapUi.mOwner->mUi;
        chart += step;
        if (chart < 0) chart = CHART_COUNT - 1;
        else if (chart > CHART_COUNT - 1) chart = 0;
        if (hdMenuMapChartOwned(ui,chart)) return chart;
    } while (chart != start);
    return -1;
}
/* 02699030 */
static void hdMenuMapUpdateChart(hdMenuMap_c* map,u32 slot,u32 chart) {
    hdMenuMapUpdateChartIcon(map,slot,chart);
    hdMenuMapUpdateChartFrame(map,slot,chart);
}
/* 02699084 */
static void hdMenuMapSetText(hdMenuMap_c*,void* pane,u32 message) {
    if (pane) hdMenuMapSetCaption(pane,message);
}
/* 02699094 */
void hdMenuMap_c::setChartCaption(u32 chart) {
    if (chart >= 61) return;
    u32 message = -1;
    if (chart < 3) message = chart + 0x31D8;
    else if (chart - 3 < 33) message = chart + 0x31DD;
    else if (chart - 36 < 8) message = chart + 0x36F1;
    else if (chart - 44 < 5) message = chart + 0x36FD;
    else if (chart - 49 < 12) message = chart + 0x36EC;
    hdMenuMapSetText(this,mpChartCaption,message);
}
/* 02699118 */
void hdMenuMap_c::resetChartSelection() {
    s32 chart = findChart(mSelectedChart,0);
    if (chart < 0) chart = findChart(0,1);
    mSelectedChart = chart;
    mPreviousSelectedChart = chart;
    mSelectedChartChanged = 0;
    mChartDirection = 0;
    if (u32(chart) < 61) { hdMenuMapUpdateChart(this,0,chart); setChartCaption(mSelectedChart); }
}
/* 0269919C */
void hdMenuMap_c::resetCounters() {
    updateOwnedCharts(); mOwnedChartsChanged = 0;
    updateOwnedTreasures(); mOwnedTreasuresChanged = 0;
    updateDiscoveredIslands(); mDiscoveredIslandsChanged = 0;
    resetChartSelection(); mChartChangePhase = 0;
}
/* 02699398 */
static void hdMenuMapClearButtons(hdMenuMap_c* map) {
    for (u32 i = 0; i < 4; ++i) {
        u32 index = sMapButtonIndices.mIndices[i];
        for (u32 receiver = 0; receiver < 8; ++receiver)
            hdMenuMapSetButtonReceiver(map->child(index), receiver, 0);
    }
    hdSeaChartMenu_c* owner = map->mpMap;
    if (owner) {
        hdBtnMapIcon_c* icons = menuMapIcons(owner);
        if (icons) for (u32 receiver = 0; receiver < 8; ++receiver)
            hdMenuMapSetButtonReceiver((hdParts_c*)icons, receiver, 0);
    }
}
/* 02699458 */
void hdMenuMap_c::updateButtonVisibility() {
    s32 count = mOwnedCharts;
    if (count <= 0) { mLeftVisible=0; mIconsVisible=0; mRightVisible=0; }
    else if (count == 1) { mLeftVisible=0; mRightVisible=0; mIconsVisible=1; }
    else { mLeftVisible=1; mIconsVisible=1; mRightVisible=1; }
}
/* 02699A1C */
void hdMenuMap_c::updatePaneVisibility() {
    hdPartsPaneSRT_c* pane = mpLeftPane;
    if (pane) pane->mFlags = (u32(pane->mFlags) & 0xFE) | u32(mLeftVisible);
    pane = mpRightPane;
    if (pane) pane->mFlags = (u32(pane->mFlags) & 0xFE) | u32(mRightVisible);
}
/* 02699A60 */
void hdMenuMap_c::updateChartAvailability() {
    if (s32(mOwnedCharts)>0 && !hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi))
        hdMenuMapEnableAnimation(animSet(),3,3);
    else hdAnimSet_operationCC(animSet(),3,3);
}
/* 02699EC0 */
u32 hdMenuMap_c::findButton(u32 receiver) {
    for (u32 i = 0; i < 4; ++i) {
        hdParts_c* button = child(sMapButtonIndices.mIndices[i]);
        if (button && receiver == hdMenuMapGetButtonReceiver(button)) return i;
    }
    return 4;
}
/* 02699F4C */
u32 hdMenuMap_c::buttonIndex(u32 receiver) {
    u32 index = findButton(receiver);
    return index == 4 ? 10 : u32(sMapButtonIndices.mIndices[index]);
}
/* 0269A3C4 */
void hdMenuMap_c::windowNowLoopEnter() { hdAnimSet_play(animSet(),1,1,1.0f); }
/* 0269A3E0 */
void hdMenuMap_c::windowNowLoopExecute() { vCalcLayout(); }
/* 0269A3F0 */
void hdMenuMap_c::treMapOutEnter() { hdAnimSet_play(animSet(),2,3,1.0f); }
/* 0269A40C */
void hdMenuMap_c::treMapOutExecute() { vCalcLayout(); }
/* 0269A41C */
void hdMenuMap_c::treMapInEnter() { hdAnimSet_play(animSet(),3,3,1.0f); }
/* 0269A438 */
void hdMenuMap_c::treMapInExecute() { vCalcLayout(); }
/* 0269A448 */
void hdMenuMap_c::treMapChangeLEnter() { hdAnimSet_play(animSet(),4,4,1.0f); }
/* 0269A464 */
void hdMenuMap_c::treMapChangeLExecute() { vCalcLayout(); }
/* 0269A474 */
void hdMenuMap_c::treMapChangeREnter() { hdAnimSet_play(animSet(),5,4,1.0f); }
/* 0269A490 */
void hdMenuMap_c::treMapChangeRExecute() { vCalcLayout(); }
/* 0269A9DC */
void hdMenuMap_c::areaNameOutEnter() { hdAnimSet_play(animSet(),7,5,1.0f); }
/* 0269A9F8 */
void hdMenuMap_c::areaNameOutExecute() { vCalcLayout(); }
/* 0269AA08 */
void hdMenuMap_c::areaNameInEnter() { hdAnimSet_play(animSet(),8,5,1.0f); }
/* 0269AA24 */
void hdMenuMap_c::areaNameInExecute() { vCalcLayout(); }
/* 0269AD34 */
void hdMenuMap_c::outAllEnter() { hdAnimSet_play(animSet(),10,0,1.0f); }
/* 0269ADF8 */
void hdMenuMap_c::clearEvent() { sMenuEventOwner.mpObject->mPending = 0; }
/* 0269AE0C */
void hdMenuMap_c::requestClose() { mCloseRequested = 1; }
/* 0269C41C */
void hdMenuMap_c::changeChartViewState(hdPartsStateId_c* state) {
    hdMenuChartView_l* view = mpSelectedChartView;
    if (view) view->changeState(state);
}
/* 0269C430 */
void hdMenuMap_c::enterChartView() {
    selectChartView(); changeChartViewState((hdPartsStateId_c*)&sChartViewWaitState);
}
/* 0269C46C */
void hdMenuMap_c::executeChartView() {
    hdMenuChartView_l* view = mpSelectedChartView;
    if (view) view->vExecuteState();
}
/* 0269D2C4 */
void hdMenuMap_c::execute() { hdPadStateExecute((HdPadStateController*)stateMachine()); }
/* 0269D9CC */
void hdMenuMap_c::activate() { hdPadChangeState((HdPadStateController*)stateMachine(),&sMenuMapActiveState); }
/* 0269D9DC */
void hdMenuMap_c::deactivate() { hdPadChangeState((HdPadStateController*)stateMachine(),&sMenuMapInactiveState); }
/* 0269DADC */
void hdMenuMap_c::setEnabled(u32 enabled) {
    hdMenuChartView_l* view = mpSelectedChartView;
    mEnabled = enabled;
    if (view) view->mEnabled = enabled;
    hdSeaChartMenu_c* owner = mpMap;
    if (owner) owner->mMapInputActive = mEnabled;
}
/* 0269DB08 */
void hdMenuMap_c::refresh() {
    resetCounters(); hdSeaChartMenu_c* owner = mpMap;
    if (owner) owner->updateOpenMap();
    refreshIslandCaption();
}
/* 0269DD08 */
static void hdMenuMapForwardUpdate(hdMenuMap_c* map) { hdMenuMapUpdateIsland(map); }
/* 0269E2A8 */
void hdMenuMap_c::switchChart() { hdPadChangeState((HdPadStateController*)stateMachine(),&sMenuMapSwitchState); }
/* 0269E2B8 */
u32 hdMenuMap_c::isBusy() { return mBusy; }
/* 0269E2C0 */
void hdMenuMap_c::exitChartView() {
    hdMenuChartView_l* view = mpSelectedChartView;
    if (view) {
        // HD: this interface slot is separate from the parts state executor.
        gabi::call_ptr<void>(view->mVtable->mDraw.get(),view);
    }
}
/* 0269BB60 */
void hdMenuMap_c::showSelectedChart() {
    hdMenuMapUpdateChartIcon(this,0,mSelectedChart);
    hdMenuMapUpdateChartDetails(this,0,mSelectedChart);
}
/* 0269BEFC */
void hdMenuMap_c::updateIslandAnimation() {
    s32 current = mIslandMessage;
    s32 previous = mPreviousIslandMessage;
    hdAnimSet* animation = animSet();
    if (current >= 0) {
        if (previous >= 0) hdMenuMapEnableAnimation(animation,8,5);
        else hdAnimSet_play(animation,8,5,1.0f);
    } else if (previous >= 0) hdAnimSet_play(animation,7,5,1.0f);
    else hdAnimSet_operationCC(animation,8,5);
}
/* 0269BFCC */
void hdMenuMap_c::showSingleChart() {
    if (mOwnedCharts == 1) { hdAnimSet_play(animSet(),3,3,1.0f); resetChartSelection(); }
}
/* 0269C024 */
void hdMenuMap_c::updateChangedCaptions() {
    if (mOwnedChartsChanged) {
        updatePaneVisibility(); showSingleChart(); updateChartCountCaption();
    }
    if (mOwnedTreasuresChanged) updateTreasureCountCaption();
    updateBalloonVisibility();
}
/* 0269C174 */
void hdMenuMap_c::activateBalloon() {
    hdParts_c* balloon = child(3);
    if (balloon && hdMenuMapButtonActive(balloon))
        gabi::call_ptr<void>(((hdMenuMapButtonVtable_l*)balloon->mVtable.get())->mActivate.get(),balloon);
}
/* 0269C1DC */
void hdMenuMap_c::updateEnabledInput() {
    if (!hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi) && mEnabled) {
        hdMenuMapUpdateInput(this); activateBalloon();
    }
}
/* 0269CE44 */
void hdMenuMap_c::cacheChartIcons() {
    mChartIcons[0] = child(6); mChartIcons[1] = child(8); mChartIcons[2] = child(7);
}
/* 0269CEA8 */
void hdMenuMap_c::cacheMap() {
    hdSeaChartMenu_c* owner = (hdSeaChartMenu_c*)child(9);
    mpMap = owner;
    if (owner) { owner->setSelectionMode(0); mpMap->bindActiveButton(); }
}
/* 0269E178 */
u32 hdMenuMap_c::isDisplay() {
    hdPartsStateId_c* state = hdMenuMapCurrentState(stateMachine());
    hdPartsStateIdVtbl_c* targetVtable = hdMenuMapDisplayState()->mVtable;
    u32 current = state->getKey();
    u32 target = gabi::call_ptr<u32>(targetVtable->mGetKey.get(),hdMenuMapDisplayState());
    return current == target;
}
/* 0269E1E8 */
u32 hdMenuMap_c::isMapHelpfulDisplay() {
    hdPartsStateId_c* state = hdMenuMapCurrentState(stateMachine());
    hdPartsStateIdVtbl_c* targetVtable = sMenuMapHelpfulDisplayState.mVtable;
    u32 current = state->getKey();
    u32 target = gabi::call_ptr<u32>(targetVtable->mGetKey.get(),&sMenuMapHelpfulDisplayState);
    return current == target;
}
/* 0269E258 */
static void hdMenuMapStartHelpfulChart(hdMenuMap_c* map) {
    if (map->isMapHelpfulDisplay()) {
        hdMenuChartView_l* view = map->mpSelectedChartView;
        if (view) view->changeState((hdPartsStateId_c*)&sChartViewHelpfulState);
    }
}
/* 0269ABA8 */
void hdMenuMap_c::changeToDisplayEnter() {
    hdMenuMapPrepareDisplay(this); hdAnimSet_play(animSet(),9,6,-1.0f);
}
/* 0269ABF0 */
void hdMenuMap_c::changeToDisplayExecute() {
    if (hdAnimSet_isEnd(animSet(),9)) changeState(hdMenuMapDisplayState());
    vExecuteChildren(); vCalcLayout();
}
/* 0269AC68 */
void hdMenuMap_c::noDataEnter() {
    hdMenuMapHideUi(l_hdMenuMapUi.mOwner->mUi);
    sMenuEventOwner.mpObject->mPending=0;
    hdMenuMapResetCells(menuMapCells(mpMap));
}
/* 0269ACBC */
void hdMenuMap_c::noDataExecute() {
    if (!hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) changeState(&sMenuMapChangeToDisplayState);
    vExecuteChildren(); vCalcLayout();
}
/* 026986C4 */
static void hdMenuMapDestroy(hdMenuMap_c* map,u32 flags) {
    if (!map) return;
    map->mStateHolder.mHolderVtable = &sMenuMapHolderVtable;
    map->mSecondaryVtable = &sMenuMapSecondaryVtable;
    hdParts_c::destroy(map,0);
    if (flags & 1) operator_delete(map);
}
/* 02698730 */
void hdMenuMap_c::resetWind() {
    u32 angle = hdMenuMapGetWindAngle();
    f32 degrees = gabi::fmuls_ppc(f32(u16(angle)),0.0054931640625f);
    mWindAngle=angle; mPreviousWindAngle=angle;
    if (!(180.0f >= degrees)) degrees = gabi::fsubs_ppc(degrees,360.0f);
    s32 rotation=gabi::ftoi(f32(f64(gabi::fmuls_ppc(degrees,2147483648.0f))/180.0));
    hdPartsPaneSRT_c* pane=mpWindPane;
    mPreviousWindRotation=rotation; mWindRotation=rotation;
    if (pane) {
        f32 value=f32(f64(gabi::fmuls_ppc(f32(rotation),180.0f))/2147483648.0);
        u32 x=pane->mRotationXBits;
        u32 y=pane->mRotationYBits;
        pane->mRotationXBits=x; pane->mRotationYBits=y;
        pane->mFlags=u32(pane->mFlags)|0x10;
        hdMenuMapSetPaneRotationZ(pane,value);
    }
}
/* 02699AE8 */
static void hdMenuMapSetString(hdMenuMap_c*,hdMenuTextPane_l* pane,hdUiSafeString* text) {
    if (!pane) return;
    hdMenuTextPaneVtable_l* vtable=pane->mpVtable;
    text->assureTermination();
    gabi::call_ptr<void>(vtable->mSetString.get(),pane,text->mText.get(),0u);
}
/* 02699B58 */
void hdMenuMap_c::updateChartCountCaption() {
    u32 count=hdMenuMapTotalCharts(l_hdMenuMapUi.mOwner->mUi);
    hdMenuCountString_l text;
    hdMenuMapFormatCount(&text,sChartCountFormat,215u,count);
    hdMenuMapSetString(this,(hdMenuTextPane_l*)mpChartCountCaption.get(),(hdUiSafeString*)&text);
}
/* 02699BBC */
void hdMenuMap_c::updateTreasureCountCaption() {
    u32 count=mOwnedTreasures;
    hdMenuCountString_l text;
    hdMenuMapFormatCount(&text,sTreasureCountFormat,215u,count);
    hdMenuMapSetString(this,(hdMenuTextPane_l*)mpTreasureCountCaption.get(),(hdUiSafeString*)&text);
}
/* 02699C10 */
void hdMenuMap_c::updateBalloonVisibility() {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) {
        hdPartsPaneSRT_c* pane=mpBalloonPane;
        pane->mFlags=u32(pane->mFlags)&0xFE;
        return;
    }
    hdPartsPaneSRT_c* pane=mpBalloonPane;
    pane->mFlags=(u32(pane->mFlags)&0xFE)+(s32(mDiscoveredIslands)>0 ? 1 : 0);
    u32 visible=hdMenuMapHasIsland(l_hdMenuMapUi.mOwner->mUi);
    hdParts_c* balloon=child(3);
    if (!balloon) return;
    if (visible) {
        if (!hdMenuMapButtonActive(balloon))
            gabi::call_ptr<void>(((hdMenuMapButtonVtable_l*)balloon->mVtable.get())->mSetEnabled.get(),balloon,1u);
    } else if (hdMenuMapButtonActive(balloon))
        gabi::call_ptr<void>(((hdMenuMapButtonVtable_l*)balloon->mVtable.get())->mSetEnabled.get(),balloon,0u);
}
/* 02699D5C */
void hdMenuMap_c::refreshCaptions() {
    updatePaneVisibility(); updateChartAvailability(); updateChartCountCaption();
    updateTreasureCountCaption(); updateBalloonVisibility();
}
/* 02699E78 */
void hdMenuMap_c::inAllEnter() { hdMenuMapPrepareDisplay(this); hdAnimSet_play(animSet(),0,0,1.0f); }
/* 0269C360 */
void hdMenuMap_c::selectChartView() {
    u32 kind=hdMenuMapChartKind(this,mSelectedChart);
    if (kind-2>2) return;
    hdMenuChartViewList_l views;
    views.mViews[1]=mChartViews[0]; views.mViews[4]=mChartViews[1];
    views.mViews[7]=mChartViews[6]; views.mViews[9]=mChartViews[8];
    u32 chart=mSelectedChart;
    views.mViews[10]=mChartViews[5]; views.mViews[2]=mChartViews[4];
    views.mViews[8]=mChartViews[9]; views.mViews[11]=mChartViews[7];
    views.mViews[0]=mChartViews[11]; views.mViews[5]=mChartViews[2];
    views.mViews[6]=mChartViews[10]; views.mViews[3]=mChartViews[3];
    if (chart-49<12) mpSelectedChartView=views.mViews[chart-49];
}
/* 0269D9EC */
void hdMenuMap_c::displayImmediately() {
    changeState(hdMenuMapDisplayState()); hdMenuMapPrepareDisplay(this);
    hdMenuMapEnableAnimation(animSet(),0,0); vExecuteChildren(); vCalcLayout();
}
/* 0269DA68 */
void hdMenuMap_c::hideImmediately() {
    changeState(&sMenuMapBlankState); hdAnimSet_operationCC(animSet(),0,0);
    vExecuteChildren(); vCalcLayout();
}
/* 02698A94 */
u32 hdMenuMapChartKind(hdMenuMap_c*,u32 chart) {
    if (chart>=61) return 5;
    hdMenuChartKindTable_l table;
    for (u32 i=0;i<18;++i) {
        table.mEntries[i].mKind=sChartKindTable.mEntries[i].mKind;
        table.mEntries[i].mGroup=sChartKindTable.mEntries[i].mGroup;
    }
    hdMenuChartInfo_l* info=hdMenuMapChartInfo(l_hdMenuMapUi.mOwner->mUi,chart);
    if (info) for (u32 i=0;i<18;++i)
        if (u32(s32(info->mKind))==table.mEntries[i].mKind) return table.mEntries[i].mGroup;
    return 5;
}
/* 02698B44 */
void hdMenuMapUpdateChartIcon(hdMenuMap_c* map,u32 slot,u32 chart) {
    if (chart>=61) return;
    u32 kind=hdMenuMapChartKind(map,chart);
    u32 frame=10;
    if (kind==0) {
        if (hdMenuMapChartOwned(l_hdMenuMapUi.mOwner->mUi,chart)) {
            if (hdMenuMapChartCollected(l_hdMenuMapUi.mOwner->mUi,chart)) {
                if (hdMenuMapChartDecoded(l_hdMenuMapUi.mOwner->mUi,chart)) {
                    frame=6;
                    if (hdMenuMapChartComplete(l_hdMenuMapUi.mOwner->mUi,chart)) frame=8;
                } else frame=9;
            } else frame=4;
        }
    } else if (kind==1) {
        if (hdMenuMapChartOwned(l_hdMenuMapUi.mOwner->mUi,chart)) {
            if (hdMenuMapChartCollected(l_hdMenuMapUi.mOwner->mUi,chart)) {
                frame=5;
                if (hdMenuMapChartComplete(l_hdMenuMapUi.mOwner->mUi,chart)) frame=7;
            } else frame=0;
        }
    } else if (kind<=4) frame=sChartKindFrames[kind];
    hdParts_c* button;
    if (slot==0) button=map->child(2);
    else if (slot==1) button=map->child(4);
    else if (slot==2) button=map->child(5);
    else return;
    if (button && frame!=10) reinterpret_cast<hdBtnMapPattern_c*>(button)->setIcon(frame,chart);
}
/* 02698D74 */
void hdMenuMapUpdateChartFrame(hdMenuMap_c* map,u32 slot,u32 chart) {
    if (chart>=61) return;
    u32 kind=hdMenuMapChartKind(map,chart);
    if (kind<=4 && hdMenuMapChartOwned(l_hdMenuMapUi.mOwner->mUi,chart)) {
        u32 collected=hdMenuMapChartCollected(l_hdMenuMapUi.mOwner->mUi,chart);
        if (kind==0) {
            if (collected) {
                if (hdMenuMapChartDecoded(l_hdMenuMapUi.mOwner->mUi,chart))
                    menuTreMap(map->mChartIcons[slot<3?slot:0])->requestTreasureOpen();
                else menuTreMap(map->mChartIcons[slot<3?slot:0])->requestTriforceUnopened();
            } else menuTreMap(map->mChartIcons[slot<3?slot:0])->requestTriforceClosed();
        } else if (kind==1) {
            if (collected) menuTreMap(map->mChartIcons[slot<3?slot:0])->requestTreasureOpen();
            else menuTreMap(map->mChartIcons[slot<3?slot:0])->requestTreasureClosed();
        } else {
            if (collected) menuTreMap(map->mChartIcons[slot<3?slot:0])->requestHelpfulOpen();
            else menuTreMap(map->mChartIcons[slot<3?slot:0])->requestHelpfulClosed();
        }
    }
    menuTreMap(map->mChartIcons[slot<3?slot:0])->selectChart(chart);
}
/* 0269B3C8 */
void hdMenuMap_c::updateWindVisibility() {
    if (!mpWindNowPane) return;
    u32 visible=(u32(sMenuMapDisplayFlags)&0x400000)?0:1;
    hdPartsPaneSRT_c* pane=mpWindNowPane;
    pane->mFlags=(u32(pane->mFlags)&0xFE)+visible;
}
/* 0269BC90 */
void hdMenuMap_c::waitChartChange() {
    if (mInputChanged) { mChartChangePhase=CHART_FINISH; return; }
    u32 direction=mChartDirection;
    if (direction==1) { if (hdAnimSet_isEnd(animSet(),5)) mChartChangePhase=CHART_FINISH; }
    else if (direction==u32(-1)) { if (hdAnimSet_isEnd(animSet(),4)) mChartChangePhase=CHART_FINISH; }
}
/* 0269BD2C */
void hdMenuMap_c::finishChartChange() {
    if (!mInputChanged) {
        u32 direction=mChartDirection;
        if (direction==1) {
            hdAnimSet_operationCC(animSet(),5,4);
            hdMenuMapUpdateChart(this,0,mSelectedChart);
        } else if (direction==u32(-1)) {
            hdAnimSet_operationCC(animSet(),4,4);
            hdMenuMapUpdateChart(this,0,mSelectedChart);
        } else { mChartDirection=0; mChartChangePhase=CHART_IDLE; return; }
    }
    mChartChangePhase=CHART_IDLE; mChartDirection=0;
}
/* 0269C5C0 */
void hdMenuMap_c::restoreInputMode() {
    hdMenuMapButton_l* button=(hdMenuMapButton_l*)child(2);
    if (button) hdMenuMapSetInputDefault(sMenuMapInputManager.mpObject,button->mpReceiver.v);
    mInputFlags=0;
}
/* 026991FC */
void hdMenuMap_c::updateIslandCaption() {
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    if (!cells) return;
    mIslandMessage=-1;
    HdPadMapCell* cell=((hdSelectedSeaCells_l*)cells)->mpSelected;
    if (cell) {
        if (!cell->mDiscovered) return;
        mIslandMessage=u32(cell->mId)+0x31A6;
    } else {
        HdPadCoordinate coordinate;
        HdPadUi* ui=l_hdMenuMapUi.mOwner->mUi;
        coordinate.x=ui->mCoordinate.x; coordinate.y=ui->mCoordinate.y;
        cell=hdMenuMapFindCell(cells,&coordinate);
        if (cell && cell->mDiscovered) mIslandMessage=u32(cell->mId)+0x31A6;
    }
    s32 message=mIslandMessage;
    if (message<0) return;
    if (message==0x31C6 && hdMenuMapEventBit(&sMenuMapSave.mpObject->mEvents,0x2D80)) {
        mIslandMessage=0x31D7;
    }
    hdMenuMapSetText(this,mpIslandCaption,mIslandMessage);
}
/* 0269930C */
void hdMenuMap_c::refreshIslandCaption() {
    updateIslandCaption();
    if (s32(mIslandMessage)>=0 && !hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi))
        hdMenuMapEnableAnimation(animSet(),8,5);
    else hdAnimSet_operationCC(animSet(),8,5);
    mPreviousIslandMessage=mIslandMessage;
}
/* 02699DA4 */
void hdMenuMapPrepareDisplay(hdMenuMap_c* map) {
    hdMenuMapUi_l* ui=(hdMenuMapUi_l*)l_hdMenuMapUi.mOwner->mUi.get();
    if (!ui->mHelpful) {
        hdMenuMapResetUi((HdPadUi*)ui);
        hdMenuMapRefreshUi(l_hdMenuMapUi.mOwner->mUi);
    }
    map->resetWind(); map->resetCounters();
    hdSeaChartMenu_c* owner=map->mpMap;
    if (owner) owner->updateOpenMap();
    map->refreshIslandCaption(); hdMenuMapRefreshReceivers(map); map->refreshCaptions();
    u32 busy=hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi);
    hdAnimSet* animation=map->animSet();
    if (busy) hdMenuMapEnableAnimation(animation,9,6);
    else hdAnimSet_operationCC(animation,9,6);
}
/* 0269A2E4 */
void hdMenuMap_c::inAllExecute() {
    if (hdAnimSet_isEnd(animSet(),0)) {
        if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) { changeState(&sMenuMapNoDataState); return; }
        changeState(hdMenuMapDisplayState());
    }
    hdMenuMapUpdateHint(this);
    hdSeaChartMenu_c* owner=mpMap;
    if (owner) owner->openMap();
    hdMenuMapUpdateMap(this); vExecuteChildren(); vCalcLayout();
    owner=mpMap;
    if (owner) owner->closeMap();
}
/* 0269A4A0 */
static void hdMenuMapSetIslandCaption(hdMenuMap_c* map,HdPadMapCell* cell) {
    if (!cell) return;
    s32 message=u32(cell->mId)+0x31A6;
    if (message<0) return;
    if (message==0x31C6 && hdMenuMapEventBit(&sMenuMapSave.mpObject->mEvents,0x2D80)) message=0x31D7;
    hdMenuMapSetText(map,map->mpIslandCaption,message);
}
/* 0269AA34 */
void hdMenuMap_c::changeToNoDataEnter() {
    hdMenuMapHideUi(l_hdMenuMapUi.mOwner->mUi); sMenuEventOwner.mpObject->mPending=0;
    hdMenuMapResetCells(menuMapCells(mpMap));
    hdAnimSet_play(animSet(),9,6,1.0f); hdAnimSet_play(animSet(),2,3,1.0f);
    hdAnimSet_playAlternate(animSet(),7,5,1.0f);
    hdPartsPaneSRT_c* pane=mpBalloonPane; pane->mFlags=u32(pane->mFlags)&0xFE;
}
/* 0269AB00 */
void hdMenuMap_c::changeToNoDataExecute() {
    if (hdAnimSet_isEnd(animSet(),9) && hdAnimSet_isEnd(animSet(),2) && hdAnimSet_isEnd(animSet(),7))
        changeState(&sMenuMapNoDataState);
    vExecuteChildren(); vCalcLayout();
}
/* 0269AD50 */
void hdMenuMap_c::outAllExecute() {
    if (hdAnimSet_isEnd(animSet(),10)) changeState(&sMenuMapBlankState);
    hdMenuMapUpdateHint(this);
    hdSeaChartMenu_c* owner=mpMap;
    if (owner) owner->openMap();
    hdMenuMapUpdateMap(this); vExecuteChildren(); vCalcLayout();
    owner=mpMap;
    if (owner) owner->closeMap();
}
/* 0269DD0C */
void hdMenuMap_c::connectChartReceiver(void* receiver) {
    if (!hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi) && receiver && mIconsVisible)
        hdMenuMapConnect(receiver,3,((hdMenuMapButton_l*)child(2))->mpReceiver.v);
}
/* 0269DF10 */
void hdMenuMap_c::setLeftReceiver(u32 receiver) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) return;
    if (mLeftVisible) hdMenuMapSetButtonReceiver(child(0),0,receiver);
    if (mIconsVisible) hdMenuMapSetButtonReceiver(child(2),0,receiver);
}
/* 0269C488 */
void hdMenuMap_c::mapHelpfulInAllExecute() {
    hdPartsStateId_c* current=hdMenuMapCurrentState(mpSelectedChartView->stateMachine());
    hdPartsStateIdVtbl_c* vtable=sChartViewDisplayState.mVtable;
    u32 key=current->getKey();
    u32 target=gabi::call_ptr<u32>(vtable->mGetKey.get(),&sChartViewDisplayState);
    if (key==target) changeState(&sMenuMapHelpfulDisplayState);
    executeChartView();
}
/* 0269C524 */
void hdMenuMap_c::mapHelpfulReturnExecute() {
    hdPartsStateId_c* current=hdMenuMapCurrentState(mpSelectedChartView->stateMachine());
    hdPartsStateIdVtbl_c* vtable=sChartViewBlankState.mVtable;
    u32 key=current->getKey();
    u32 target=gabi::call_ptr<u32>(vtable->mGetKey.get(),&sChartViewBlankState);
    if (key==target) changeState(hdMenuMapDisplayState());
    executeChartView();
}
/* 0269C620 */
void hdMenuMap_c::mapHelpfulOutAllExecute() {
    hdPartsStateId_c* current=hdMenuMapCurrentState(mpSelectedChartView->stateMachine());
    hdPartsStateIdVtbl_c* vtable=((hdPartsStateId_c*)&sChartViewHelpfulState)->mVtable;
    u32 key=current->getKey();
    u32 target=gabi::call_ptr<u32>(vtable->mGetKey.get(),&sChartViewHelpfulState);
    if (key==target) changeState(&sMenuMapHelpfulOutAllState);
    executeChartView();
}
/* 0269DD94 */
void hdMenuMap_c::connectLeftMapReceiver(void* receiver) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi) || !receiver) return;
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!cells || !icons) return;
    if (icons->mMapMode==3) hdMenuMapConnect(receiver,3,icons->mButton.mpReceiver.v);
    else {
        HdPadCoordinate coordinate;
        coordinate.y=-3; coordinate.x=-3;
        u32 destination=hdMenuMapCellReceiver(cells,&coordinate);
        hdMenuMapConnect(receiver,3,destination);
    }
}
/* 0269DE50 */
void hdMenuMap_c::connectBottomMapReceiver(void* receiver) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi) || !receiver) return;
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!cells || !icons) return;
    if (icons->mMapMode==3) hdMenuMapConnect(receiver,3,icons->mButton.mpReceiver.v);
    else {
        HdPadCoordinate coordinate;
        coordinate.x=0; coordinate.y=-3;
        u32 destination=hdMenuMapCellReceiver(cells,&coordinate);
        hdMenuMapConnect(receiver,3,destination);
    }
}
/* 0269DFB4 */
void hdMenuMap_c::setRightMapReceivers(u32 receiver) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) return;
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!cells || !icons) return;
    if (mRightVisible) hdMenuMapSetButtonReceiver(child(1),0,receiver);
    if (icons->mMapMode==3) { hdMenuMapSetButtonReceiver((hdParts_c*)icons,0,receiver); return; }
    HdPadCoordinate coordinate;
    for (s32 x=-3;x<0;++x) {
        coordinate.x=x; coordinate.y=-3;
        HdPadMapCell* cell=hdMenuMapFindCell(cells,&coordinate);
        if (cell) hdMenuMapSetCellReceiver(cell,0,receiver);
    }
}
/* 0269E0C0 */
void hdMenuMap_c::setBottomMapReceivers(u32 receiver) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) return;
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!cells || !icons || icons->mMapMode==3) return;
    HdPadCoordinate coordinate;
    for (s32 x=0;x<4;++x) {
        coordinate.x=x; coordinate.y=-3;
        HdPadMapCell* cell=hdMenuMapFindCell(cells,&coordinate);
        if (cell) hdMenuMapSetCellReceiver(cell,0,receiver);
    }
}
/* 026994AC */
void hdMenuMap_c::connectChartButtons() {
    if (!mLeftVisible || !mRightVisible || !mIconsVisible) return;
    hdParts_c* button=child(0);
    u32 receiver=((hdMenuMapButton_l*)child(2))->mpReceiver.v;
    hdMenuMapSetButtonReceiver(button,7,receiver);
    button=child(1); receiver=((hdMenuMapButton_l*)child(2))->mpReceiver.v;
    hdMenuMapSetButtonReceiver(button,6,receiver);
    button=child(2); receiver=((hdMenuMapButton_l*)child(0))->mpReceiver.v;
    hdMenuMapSetButtonReceiver(button,6,receiver);
    button=child(2); receiver=((hdMenuMapButton_l*)child(1))->mpReceiver.v;
    hdMenuMapSetButtonReceiver(button,7,receiver);
}
/* 026995E0 */
void hdMenuMap_c::connectIconsToButton() {
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!icons || icons->mMapMode!=3) return;
    if (mIconsVisible && !mRightVisible)
        hdMenuMapSetButtonReceiver(child(2),7,icons->mButton.mpReceiver.v);
    else if (mRightVisible)
        hdMenuMapSetButtonReceiver(child(1),7,icons->mButton.mpReceiver.v);
}
/* 02699674 */
void hdMenuMap_c::connectButtonToIcons() {
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!icons || icons->mMapMode!=3) return;
    if (mIconsVisible && !mRightVisible)
        hdMenuMapSetButtonReceiver((hdParts_c*)icons,6,((hdMenuMapButton_l*)child(2))->mpReceiver.v);
    else if (mRightVisible)
        hdMenuMapSetButtonReceiver((hdParts_c*)icons,6,((hdMenuMapButton_l*)child(1))->mpReceiver.v);
}
/* 02699710 */
void hdMenuMap_c::connectCellsToButton() {
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    if (!cells) return;
    if (mIconsVisible && !mRightVisible) {
        HdPadCoordinate coordinate;
        coordinate.y=0; coordinate.x=-3;
        HdPadMapCell* cell=hdMenuMapFindCell(cells,&coordinate);
        u32 receiver=cell ? u32(((hdMenuCellReceiver_l*)cell)->mReceiver) : 0;
        hdMenuMapSetButtonReceiver(child(2),7,receiver);
    } else if (mRightVisible) {
        HdPadCoordinate coordinate;
        coordinate.y=0; coordinate.x=-3;
        HdPadMapCell* cell=hdMenuMapFindCell(cells,&coordinate);
        u32 receiver=cell ? u32(((hdMenuCellReceiver_l*)cell)->mReceiver) : 0;
        hdMenuMapSetButtonReceiver(child(1),7,receiver);
    }
}
/* 02699808 */
void hdMenuMap_c::connectButtonToCells() {
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    if (!cells) return;
    if (mIconsVisible && !mRightVisible) {
        HdPadCoordinate coordinate;
        for (s32 y=-3;y<4;++y) {
            coordinate.x=-3; coordinate.y=y;
            HdPadMapCell* cell=hdMenuMapFindCell(cells,&coordinate);
            if (cell) hdMenuMapSetCellReceiver(cell,6,((hdMenuMapButton_l*)child(2))->mpReceiver.v);
        }
    } else if (mRightVisible) {
        HdPadCoordinate coordinate;
        for (s32 y=-3;y<4;++y) {
            coordinate.x=-3; coordinate.y=y;
            HdPadMapCell* cell=hdMenuMapFindCell(cells,&coordinate);
            if (cell) hdMenuMapSetCellReceiver(cell,6,((hdMenuMapButton_l*)child(1))->mpReceiver.v);
        }
    }
}
/* 0269993C */
void hdMenuMapRefreshReceivers(hdMenuMap_c* map) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) return;
    hdSeaChartMenu_c* owner=map->mpMap;
    if (!owner) return;
    HdPadGrid* cells=menuMapCells(owner);
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    if (!cells || !icons) return;
    hdMenuMapResetCellReceivers(cells); hdMenuMapClearButtons(map);
    map->updateButtonVisibility(); hdMenuMapConnectCellReceivers(cells);
    map->connectChartButtons();
    if (icons->mMapMode==3) { map->connectIconsToButton(); map->connectButtonToIcons(); }
    else { map->connectCellsToButton(); map->connectButtonToCells(); }
}
/* 0269BBA4 */
void hdMenuMap_c::startChartChange() {
    if (!mInputChanged) {
        u32 direction=mChartDirection;
        if (direction==1) { hdAnimSet_play(animSet(),5,4,1.0f); hdMenuMapUpdateChart(this,2,mSelectedChart); }
        else if (direction==u32(-1)) { hdAnimSet_play(animSet(),4,4,1.0f); hdMenuMapUpdateChart(this,1,mSelectedChart); }
    }
    hdMenuMapUpdateChart(this,0,mOutgoingChart); setChartCaption(mSelectedChart); mChartChangePhase=CHART_WAIT;
}
/* 0269BDE0 */
void hdMenuMap_c::updateChartChange() {
    updateOwnedCharts(); updateOwnedTreasures(); showSelectedChart(); updateDiscoveredIslands();
    mSelectedChartChanged=0;
    if (mSelectedChart!=mPreviousSelectedChart) {
        mChartChangePhase=CHART_START; mSelectedChartChanged=1; startChartChange();
    } else {
        u32 timer=mChartChangePhase;
        if (timer==CHART_START) startChartChange();
        else if (timer==CHART_WAIT) waitChartChange();
        else if (timer==CHART_FINISH) finishChartChange();
    }
    mPreviousSelectedChart=mSelectedChart;
}
/* 0269BF50 */
void hdMenuMap_c::updateIslandChanges() {
    hdSeaChartMenu_c* owner=mpMap;
    if (!owner) return;
    hdMenuGridChanges_l* cells=(hdMenuGridChanges_l*)menuMapCells(owner);
    if (!cells) return;
    if (cells->mSelectionChanged || cells->mDiscoveryChanged || cells->mPositionChanged) {
        updateIslandCaption(); updateIslandAnimation(); mPreviousIslandMessage=mIslandMessage;
    }
}
/* 0269C088 */
void hdMenuMapUpdateInput(hdMenuMap_c* map) {
    if (map->mIconsVisible) {
        hdParts_c* button=map->child(2);
        if (button && map->mChartChangePhase==0)
            gabi::call_ptr<void>(((hdMenuMapButtonVtable_l*)button->mVtable.get())->mActivate.get(),button);
    }
    if (map->mLeftVisible && map->mRightVisible) {
        hdParts_c* button=map->child(0);
        if (button) gabi::call_ptr<void>(((hdMenuMapButtonVtable_l*)button->mVtable.get())->mActivate.get(),button);
        button=map->child(1);
        if (button) gabi::call_ptr<void>(((hdMenuMapButtonVtable_l*)button->mVtable.get())->mActivate.get(),button);
    }
}
/* 0269C238 */
void hdMenuMap_c::displayExecute() {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi)) { changeState(&sMenuMapChangeToNoDataState); return; }
    hdMenuMapUpdateHint(this); hdMenuMapConsumeEvent(this);
    hdSeaChartMenu_c* owner=mpMap;
    if (owner) owner->pollButtonEvent();
    hdMenuMapHandleEvent(this);
    owner=mpMap;
    if (owner) {
        owner->pollSelectionActivation();
        owner=mpMap;
        if (owner) { owner->processMapAction(); owner=mpMap; if (owner) owner->openMap(); }
    }
    hdMenuMapUpdateMap(this); updateWindVisibility(); updateChartChange(); updateIslandChanges();
    vExecuteChildren(); vCalcLayout(); owner=mpMap; if (owner) owner->closeMap();
    hdMenuMapRefreshReceivers(this); updateChangedCaptions(); updateEnabledInput();
}
/* 0269CDAC */
void hdMenuMap_c::enableButtons() {
    for (u32 i=0;i<4;++i) {
        hdBasicButton_c* button=(hdBasicButton_c*)child(sMapButtonIndices.mIndices[i]);
        if (button) button->mRequestedState=1;
    }
}
/* 0269CDF4 */
void hdMenuMap_c::configureArrows() {
    for (u32 i=0;i<2;++i) {
        hdMenuMapArrow_l* arrow=(hdMenuMapArrow_l*)child(sMapArrowIndices[i]);
        if (arrow) { arrow->mDirection=2; arrow->mAcceptedDirections=15; }
    }
}
/* 0269A91C */
void hdMenuMap_c::markIslandDiscovered() {
    HdPadUi* ui=l_hdMenuMapUi.mOwner->mUi;
    hdSeaChartMenu_c* owner=mpMap;
    HdPadMapCell* cell=hdMenuMapFindCell(menuMapCells(owner),&ui->mCoordinate);
    hdMenuMapMarkDiscovered(&sMenuMapSave.mpObject->mMapData,cell->mId);
}
/* 0269A968 */
void hdMenuMap_c::openDemoExit() {
    hdBtnMapIcon_c* icons=menuMapIcons(mpMap);
    icons->showPrimaryImmediately(); icons->setShipVisible(1); icons->setTreasureVisible(1);
    markIslandDiscovered(); mBusy=0; mSoundFrame=-1; mIconFrame=-1;
}
/* 02699F88 */
void hdMenuMapUpdateHint(hdMenuMap_c* map) {
    if (hdMenuMapUiBusy(l_hdMenuMapUi.mOwner->mUi) || !sMenuMapActiveInput.mInput->mActive) return;
    HdPadInputMode* mode=hdMenuMapInputMode(sMenuMapInputManager.mpObject);
    if (!mode) return;
    hdSeaChartMenu_c* owner=map->mpMap;
    if (!owner || !menuMapCells(owner)) return;
    HdPadMapCell* cell=hdMenuMapFindReceiverCell(menuMapCells(owner),mode);
    if (cell) {
        u32 discovered=cell->mDiscovered;
        hdMenuMapPlay_l* play=hdMenuMapPlay();
        play->mButtonHint=discovered ? 42 : 0;
        return;
    }
    if (map->buttonIndex(mode->mId)<=3) { hdMenuMapPlay()->mButtonHint=23; return; }
    owner=map->mpMap;
    if (!owner || !menuMapIcons(owner)) return;
    u32 receiver=hdMenuMapGetButtonReceiver((hdParts_c*)menuMapIcons(owner));
    u32 current=mode->mId;
    hdMenuMapPlay_l* play=hdMenuMapPlay();
    play->mButtonHint=receiver==current ? 0 : 23;
}
/* 0269A104 */
void hdMenuMapUpdateMap(hdMenuMap_c* map) {
    map->mPreviousWindAngle=map->mWindAngle;
    u32 angle=hdMenuMapGetWindAngle();
    u32 previous=u16(map->mPreviousWindAngle);
    map->mWindAngle=angle;
    if (angle!=previous) {
        f32 degrees=gabi::fmuls_ppc(f32(angle),0.0054931640625f);
        if (!(180.0f>=degrees)) degrees=gabi::fsubs_ppc(degrees,360.0f);
        s32 target=gabi::ftoi(f32(f64(gabi::fmuls_ppc(degrees,2147483648.0f))/180.0));
        s32 current=map->mWindRotation;
        map->mPreviousWindRotation=target;
        if (target==current) return;
    } else if (map->mPreviousWindRotation==map->mWindRotation) return;
    gabi::Local<be<s32>> maximumStorage;
    be<s32>& maximum=*maximumStorage;
    maximum=gabi::ftoi(f32(f64(gabi::fmuls_ppc(24.0f,2147483648.0f))/180.0));
    gabi::Local<be<s32>> minimumStorage;
    be<s32>& minimum=*minimumStorage;
    minimum=gabi::ftoi(f32(f64(gabi::fmuls_ppc(0.5f,2147483648.0f))/180.0));
    hdMenuMapChaseAngle(&map->mWindRotation,&map->mPreviousWindRotation,&maximum,&minimum,0.3f);
    hdPartsPaneSRT_c* pane=map->mpWindPane;
    if (pane) {
        s32 current=map->mWindRotation;
        u32 x=pane->mRotationXBits; u32 y=pane->mRotationYBits;
        f32 degrees=f32(f64(gabi::fmuls_ppc(f32(current),180.0f))/2147483648.0);
        pane->mRotationXBits=x; pane->mRotationYBits=y;
        pane->mFlags=u32(pane->mFlags)|0x10;
        hdMenuMapSetPaneRotationZ(pane,degrees);
    }
}
/* 0269C6BC */
hdMenuMap_c* hdMenuMap_c::construct(hdMenuMap_c* map) {
    if (!map) { map=(hdMenuMap_c*)operator_new(sizeof(hdMenuMap_c)); if (!map) return NULL; }
    hdParts_c::construct(map);
    map->mVtable=&sMenuMapVtable;
    map->mStateHolder.mHolderVtable=&sMenuMapHolderVtable;
    map->mSecondaryVtable=&sMenuMapSecondaryVtable;
    for (u32 i=0;i<12;++i) map->mChartViews[i]=NULL;
    map->mpSelectedChartView=NULL;
    // HD: GHS checks embedded array storage for wrapped NULL addresses before clearing it.
    if (u32(gabi::ea(map)+offsetof(hdMenuMap_c,mChartIcons))==0) operator_new(12);
    map->mInputFlags=0; map->mInputChanged=0; map->mChartChangePhase=CHART_UNINITIALIZED;
    map->mOwnedCharts=0; map->mPreviousOwnedCharts=0;
    map->mOwnedTreasures=0; map->mPreviousOwnedTreasures=0;
    map->mDiscoveredIslands=0; map->mPreviousDiscoveredIslands=0;
    map->mSelectedChart=0; map->mOutgoingChart=0; map->mPreviousSelectedChart=0; map->mChartDirection=0;
    map->mOwnedChartsChanged=0; map->mOwnedTreasuresChanged=0;
    map->mDiscoveredIslandsChanged=0; map->mSelectedChartChanged=0; map->mSoundFrame=-1;
    if (u32(gabi::ea(map)+offsetof(hdMenuMap_c,mpWindPane))==0) operator_new(8);
    if (u32(gabi::ea(map)+offsetof(hdMenuMap_c,mpLeftPane))==0) operator_new(40);
    if (u32(gabi::ea(map)+offsetof(hdMenuMap_c,mpChartCaption))==0) operator_new(16);
    map->mWindAngle=0; map->mPreviousWindAngle=0;
    map->mEnabled=0; map->mCloseRequested=0; map->mBusy=0;
    map->mWindRotation=0; map->mPreviousWindRotation=0;
    map->mIslandMessage=-1; map->mPreviousIslandMessage=-1; map->mpMap=NULL; map->mIconFrame=-1;
    if (u32(gabi::ea(map)+offsetof(hdMenuMap_c,mLeftVisible))==0) operator_new(3);
    for (u32 i=0;i<3;++i) map->mChartIcons[i]=NULL;
    map->mpWindPane=NULL; map->mpWindNowPane=NULL;
    map->mpLeftPane=NULL; map->mpRightPane=NULL; map->mpTreMapPane=NULL; map->mpBalloonPane=NULL;
    for (u32 i=0;i<6;++i) map->mCaptionPanes[i]=NULL;
    map->mpChartCaption=NULL; map->mpIslandCaption=NULL; map->mpChartCountCaption=NULL; map->mpTreasureCountCaption=NULL;
    map->mLeftVisible=0; map->mRightVisible=0; map->mIconsVisible=0;
    return map;
}
static hdScreenString sMenuMapWindNames[2] = {};
static hdScreenString sMenuMapPaneNames[10] = {};
static hdScreenString sMenuMapCaptionNames[4] = {};
static u32 sMenuMapCaptionMessages[4] = {0x20,0x20,0x8,0x8};
static u16 sMenuMapEmptyText;
static hdScreenStringTable sMenuMapStringVtable;
void hdMenuMapSetCaptionMessage(void*,u32);
void hdMenuMapSetArrowSound(hdParts_c*,hdScreenString*,u32);

/* 0269CA90 */
void hdMenuMap_c::cacheWindPanes() {
    for (u32 i=0;i<2;++i) {
        hdScreenAccessor* accessor=mpLayout->mAccessor;
        sMenuMapWindNames[i].assureTermination();
        (&mpWindPane)[i]=(hdPartsPaneSRT_c*)accessor->mObject->findPane(sMenuMapWindNames[i].mText);
    }
}
/* 0269CB44 */
void hdMenuMap_c::cachePanes() {
    for (u32 i=0;i<10;++i) {
        hdScreenAccessor* accessor=mpLayout->mAccessor;
        sMenuMapPaneNames[i].assureTermination();
        (&mpLeftPane)[i]=(hdPartsPaneSRT_c*)accessor->mObject->findPane(sMenuMapPaneNames[i].mText);
    }
}
/* 0269CBF8 */
void hdMenuMap_c::cacheCaptions() {
    for (u32 i=0;i<4;++i) {
        hdScreenAccessor* accessor=mpLayout->mAccessor;
        sMenuMapCaptionNames[i].assureTermination();
        (&mpChartCaption)[i]=accessor->mObject->findPane(sMenuMapCaptionNames[i].mText);
    }
}
/* 0269CCAC */
void hdMenuMap_c::initializeCaptions() {
    for (u32 i=0;i<4;++i) {
        if (!(&mpChartCaption)[i]) continue;
        hdMenuMapSetCaptionMessage((&mpChartCaption)[i],sMenuMapCaptionMessages[i]);
        hdMenuTextPane_l* pane=(hdMenuTextPane_l*)(&mpChartCaption)[i].get();
        gabi::call_ptr(pane->mpVtable->mSetString.get(),pane,&sMenuMapEmptyText,0);
    }
}
/* 0269CF04 */
void hdMenuMap_c::initializeLayout() {
    cacheWindPanes(); cachePanes(); cacheCaptions(); initializeCaptions();
    enableButtons(); configureArrows(); cacheChartIcons(); cacheMap();
    hdButtonPart_c* balloon=(hdButtonPart_c*)child(3);
    if (balloon) balloon->mTouchEnabled=0;
    hdMenuMapEnableAnimation(animSet(),3,3);
    hdMenuMapEnableAnimation(animSet(),6,2);
    hdAnimSet_play(animSet(),1,1,1.0f);
    hdAnimSet_operationCC(animSet(),0,0); vCalcLayout();
}
/* 0269CFF0 */
void hdMenuMap_c::initializeArrowSounds() {
    hdParts_c* button=child(0);
    if (button) {
        hdScreenString sound;
        sound.mText.v=gabi::guest_addr("SE_MENU_CHART_SWITCH"); sound.mVtable=&sMenuMapStringVtable;
        hdMenuMapSetArrowSound(button,&sound,0);
    }
    button=child(1);
    if (button) {
        hdScreenString sound;
        sound.mText.v=gabi::guest_addr("SE_MENU_CHART_SWITCH"); sound.mVtable=&sMenuMapStringVtable;
        hdMenuMapSetArrowSound(button,&sound,0);
    }
}

static hdUiSafeString sMenuMapArchiveNames[9] = {};
static hdScreenString sMenuMapLayoutName;
static hdPartsStateId_c sMenuMapHelpfulInAllState;
void hdMenuMapExitHelpfulChart(hdMenuChartView_l*);

/* 0269D098 */
u32 hdMenuMap_c::load(void* resource,u32 option,void* descriptor) {
    attachArchives(sMenuMapArchiveNames,9);
    if (!vLoad(&sMenuMapLayoutName,mpResAccessor.v,u32(gabi::ea(resource)),option,descriptor)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    createChartViews(); initializeLayout(); startState(&sMenuMapBlankState);
    initializeArrowSounds(); return 1;
}
/* 0269D268 */
void hdMenuMap_c::unload() {
    vDeleteChildren(); vDestroyLayout(); destroyChartViews(); detachArchives();
}
/* 0269DB4C */
u32 hdMenuMap_c::isHelpful() {
    hdPartsStateId_c* state=hdMenuMapCurrentState(stateMachine());
    hdPartsStateIdVtbl_c* targetVtable=sMenuMapHelpfulInAllState.mVtable;
    u32 current=state->getKey();
    if (current==gabi::call_ptr<u32>(targetVtable->mGetKey.get(),&sMenuMapHelpfulInAllState)) return 1;
    state=hdMenuMapCurrentState(stateMachine()); targetVtable=sMenuMapHelpfulDisplayState.mVtable;
    current=state->getKey();
    if (current==gabi::call_ptr<u32>(targetVtable->mGetKey.get(),&sMenuMapHelpfulDisplayState)) return 1;
    state=hdMenuMapCurrentState(stateMachine()); targetVtable=sMenuMapHelpfulOutAllState.mVtable;
    current=state->getKey();
    if (current==gabi::call_ptr<u32>(targetVtable->mGetKey.get(),&sMenuMapHelpfulOutAllState)) return 1;
    return mInputFlags!=0;
}
/* 0269DC7C */
void hdMenuMapUpdateIsland(hdMenuMap_c* map) {
    if (!map->isHelpful() || !map->mpSelectedChartView) return;
    hdMenuMapExitHelpfulChart(map->mpSelectedChartView);
    map->changeState(hdMenuMapDisplayState());
    hdButtonPart_c* button=(hdButtonPart_c*)map->child(2);
    if (button) hdMenuMapSetInputDefault(sMenuMapInputManager.mpObject,button->mpReceiver.v);
    map->mInputFlags=0;
}

struct hdMenuMapScreenContext_l { u8 mPadding[0x48]; gptr<void> mpOptions; gptr<void> mpResource; be<u32> mFlags; u8 mPaddingInline[0x30]; u8 mInlineOptions[0xE0]; gptr<void> mpSpecialResource; };
struct hdMenuMapScreens_l { u8 mPadding[0x1020]; be<u32> mCount; gptr<gptr<hdMenuMapScreenContext_l>> mpScreens; };
struct sMenuMapScreensSlot_l { gptr<hdMenuMapScreens_l> mpObject; };
static sMenuMapScreensSlot_l sMenuMapScreens;
static u8 sMenuMapDefaultOptions;
static u8 sMenuMapDefaultResource;
/* 0269C8E4 */
void hdMenuMap_c::createChartViews() {
    hdMenuMapScreens_l* manager=sMenuMapScreens.mpObject;
    hdMenuMapScreenContext_l* context=manager->mpScreens.get()[manager->mCount>7 ? 7 : 0];
    u32 flags=context->mFlags;
    void* resource=(flags&0x1000) ? context->mpSpecialResource.get() : context->mpResource.get();
    if (!(flags&0x1000) && !resource) resource=&sMenuMapDefaultResource;
    void* options;
    if ((flags&0xC0)==0xC0) options=context->mInlineOptions;
    else { options=context->mpOptions; if (!options) options=&sMenuMapDefaultOptions; }
    hdMiiverseChartFactory_c* factory=(hdMiiverseChartFactory_c*)this;
    factory->createTincleTower((gptr<hdParts_c>*)&mChartViews[0],resource,gabi::ea(options));
    factory->createFairy((gptr<hdParts_c>*)&mChartViews[1],resource,gabi::ea(options));
    factory->createHeartIsland((gptr<hdParts_c>*)&mChartViews[2],resource,gabi::ea(options));
    factory->createSquid((gptr<hdParts_c>*)&mChartViews[3],resource,gabi::ea(options));
    factory->createTriforce((gptr<hdParts_c>*)&mChartViews[4],resource,gabi::ea(options));
    factory->createTerry((gptr<hdParts_c>*)&mChartViews[5],resource,gabi::ea(options));
    factory->createSecretEntrance((gptr<hdParts_c>*)&mChartViews[6],resource,gabi::ea(options));
    factory->createSubmarine((gptr<hdParts_c>*)&mChartViews[7],resource,gabi::ea(options));
    factory->createEnemyTower((gptr<hdParts_c>*)&mChartViews[8],resource,gabi::ea(options));
    factory->createMoon((gptr<hdParts_c>*)&mChartViews[9],resource,gabi::ea(options));
    factory->createHeartSea((gptr<hdParts_c>*)&mChartViews[10],resource,gabi::ea(options));
    factory->createGhostShip((gptr<hdParts_c>*)&mChartViews[11],resource,gabi::ea(options));
}
/* 0269D1B4 */
void hdMenuMap_c::destroyChartViews() {
    hdMiiverseChartFactory_c* factory=(hdMiiverseChartFactory_c*)this;
    factory->destroyTincleTower(mChartViews[0]);
    factory->destroyFairy(mChartViews[1]);
    factory->destroyHeartIsland(mChartViews[2]);
    factory->destroySquid(mChartViews[3]);
    factory->destroyTriforce(mChartViews[4]);
    factory->destroyTerry(mChartViews[5]);
    factory->destroySecretEntrance(mChartViews[6]);
    factory->destroySubmarine(mChartViews[7]);
    factory->destroyEnemyTower(mChartViews[8]);
    factory->destroyMoon(mChartViews[9]);
    factory->destroyHeartSea(mChartViews[10]);
    factory->destroyGhostShip(mChartViews[11]);
}
struct hdMenuMapEventButtonVtbl_l { u8 mPadding[0xE4]; gfn mHandleKind; u8 mPaddingEvent[0xC]; gfn mHandleEvent; };
struct hdMenuMapInputQueue_l { u8 mPadding[0x14]; u8 mQueue[0x1C]; HdPadEvent mEvent; };
static u32 sMenuMapButtonEventKinds[4] = {0x5d,0x5e,0x5f,0x6d};
static u8 sMenuMapTouchBlocked;
void hdMenuMapConstructEvent(HdPadEvent*,u32,u32,u32);
void hdMenuMapDestroyEvent(HdPadEvent*);
void hdMenuMapQueueEvent(void*,HdPadEvent*);
u32 hdMenuMapEventDirection(HdPadEvent*);
u32 hdMenuMapEventConfirmed(HdPadEvent*);
u32 hdMenuMapKindDirection(u32);
u32 hdMenuMapKindConfirmed(u32);
u32 hdMenuMapKindCancelled(u32);
void hdMenuMapPlayNamedSound(const char*);
u32 hdMenuMapChartCollectedMap(HdPadUi*,u32);
u32 hdMenuMapChartCollectedItem(HdPadUi*,u32);

/* 0269AE18 */
void hdMenuMap_c::postButtonEvent(u32 receiver) {
    for (u32 i=0;i<4;++i) {
        hdParts_c* button=child(sMapButtonIndices.mIndices[i]);
        if (!button || receiver!=hdMenuMapGetButtonReceiver(button)) continue;
        u32 id=hdMenuMapGetButtonReceiver(button);
        HdPadEvent event;
        hdMenuMapConstructEvent(&event,sMenuMapButtonEventKinds[i],id,0);
        hdMenuMapInputQueue_l* input=(hdMenuMapInputQueue_l*)sMenuMapInputManager.mpObject.get();
        hdMenuMapQueueEvent(input->mQueue,&event); hdMenuMapDestroyEvent(&event); return;
    }
}
/* 0269AEE8 */
void hdMenuMapConsumeEvent(hdMenuMap_c* map) {
    hdMenuMapInputQueue_l* input=(hdMenuMapInputQueue_l*)sMenuMapInputManager.mpObject.get();
    if (!input) return;
    HdPadEvent event;
    hdPadCopyEvent(&event,&input->mEvent);
    u32 receiver=event.mId,kind=event.mKind;
    HdPadInputMode* mode=NULL;
    if (sMenuMapActiveInput.mInput->mActive) mode=hdMenuMapInputMode(sMenuMapInputManager.mpObject);
    if (mode) {
        for (u32 i=0;i<2;++i) {
            hdParts_c* arrow=map->child(sMapArrowIndices[i]);
            if (!arrow) continue;
            u32 arrowReceiver=hdMenuMapGetButtonReceiver(arrow);
            if (mode->mId!=arrowReceiver) continue;
            hdMenuMapEventButtonVtbl_l* table=(hdMenuMapEventButtonVtbl_l*)arrow->mVtable.get();
            if (gabi::call_ptr<u32>(table->mHandleEvent.get(),arrow,&event)) {
                map->mInputChanged=hdMenuMapEventDirection(&event);
                map->postButtonEvent(hdMenuMapGetButtonReceiver(arrow));
            }
        }
        if (hdMenuMapEventConfirmed(&event)) {
            hdParts_c* button=map->child(2);
            u32 buttonReceiver=button ? hdMenuMapGetButtonReceiver(button) : 0;
            if (button && mode->mId==buttonReceiver) {
                u32 chartKind=hdMenuMapChartKind(map,map->mSelectedChart);
                if (chartKind!=0 && chartKind!=1) map->mInputFlags=1;
            }
        }
    }
    for (u32 i=0;i<2;++i) {
        hdParts_c* arrow=map->child(sMapArrowIndices[i]);
        if (!arrow || receiver!=hdMenuMapGetButtonReceiver(arrow)) continue;
        hdMenuMapEventButtonVtbl_l* table=(hdMenuMapEventButtonVtbl_l*)arrow->mVtable.get();
        if (gabi::call_ptr<u32>(table->mHandleKind.get(),arrow,kind)) {
            map->mInputChanged=hdMenuMapKindDirection(kind);
            map->postButtonEvent(hdMenuMapGetButtonReceiver(arrow));
        }
    }
    if (hdMenuMapKindConfirmed(kind)) {
        hdParts_c* button=map->child(2);
        if (!button || receiver!=hdMenuMapGetButtonReceiver(button)) return;
        u32 chartKind=hdMenuMapChartKind(map,map->mSelectedChart);
        if (chartKind!=0 && chartKind!=1) map->mInputFlags=1;
        return;
    }
    if (!hdMenuMapKindCancelled(kind)) return;
    hdParts_c* button=map->child(2);
    if (button && receiver==hdMenuMapGetButtonReceiver(button)) {
        if (!sMenuMapTouchBlocked) map->postButtonEvent(receiver);
        map->mInputFlags=0;
    }
    button=map->child(3);
    if (button && receiver==hdMenuMapGetButtonReceiver(button)) map->postButtonEvent(receiver);
}
/* 0269B238 */
void hdMenuMapHandleEvent(hdMenuMap_c* map) {
    HdPadEvent event;
    hdMenuMapInputQueue_l* input=(hdMenuMapInputQueue_l*)sMenuMapInputManager.mpObject.get();
    hdPadCopyEvent(&event,&input->mEvent);
    u32 kind=event.mKind;
    if (kind==0x5D || kind==0x5E) {
        if (s32(map->mOwnedCharts)<=0) return;
        s32 direction=kind==0x5D ? -1 : 1;
        map->mChartDirection=direction;
        s32 next=map->findChart(map->mSelectedChart,direction);
        if (next>=0) { u32 old=map->mSelectedChart; map->mSelectedChart=next; map->mOutgoingChart=old; }
    } else if (kind==0x5F) {
        u32 complete=hdMenuMapChartCollectedMap(l_hdMenuMapUi.mOwner->mUi,map->mSelectedChart);
        hdMenuMapChartCollectedItem(l_hdMenuMapUi.mOwner->mUi,map->mSelectedChart);
        u32 chartKind=hdMenuMapChartKind(map,map->mSelectedChart);
        if (chartKind==0 || chartKind==1) {
            if (!complete) hdMenuMapPlayNamedSound("SE_MENU_CHART_OPEN_CLOSE_S");
        } else map->changeState(&sMenuMapHelpfulInAllState);
    }
}
static hdPartsStateId_c sMenuTreasureMissingState;
static hdPartsStateId_c sMenuTreasureUndecodedState;
static hdPartsStateId_c sMenuTreasureDecodedState;
static hdPartsStateId_c sMenuTreasureCompleteState;
u32 hdMenuMapChartCollectedMap(HdPadUi*,u32);
u32 hdMenuMapChartCollectedItem(HdPadUi*,u32);
static bool hdMenuMapIconInState(hdMenuMap_c* map,u32 index,hdPartsStateId_c* target) {
    hdParts_c* icon=map->mChartIcons[index<3 ? index : 0];
    hdPartsStateId_c* current=hdMenuMapCurrentState(icon->stateMachine());
    hdPartsStateIdVtbl_c* targetVtable=target->mVtable;
    u32 key=current->getKey();
    return key==gabi::call_ptr<u32>(targetVtable->mGetKey.get(),target);
}
/* 0269B414 */
void hdMenuMapUpdateChartDetails(hdMenuMap_c* map,u32 index,u32 chart) {
    if (chart>=61) return;
    u32 kind=hdMenuMapChartKind(map,chart);
    if (kind>4 || !hdMenuMapChartOwned(l_hdMenuMapUi.mOwner->mUi,chart)) return;
    if (kind==0) {
        if (hdMenuMapChartCollectedMap(l_hdMenuMapUi.mOwner->mUi,chart)) {
            if (hdMenuMapChartDecoded(l_hdMenuMapUi.mOwner->mUi,chart)) {
                if (hdMenuMapIconInState(map,index,&sMenuTreasureUndecodedState))
                    menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestTreasureOpening();
                if (hdMenuMapIconInState(map,index,&sMenuTreasureDecodedState))
                    menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestTreasureOpen();
            } else if (hdMenuMapIconInState(map,index,&sMenuTreasureUndecodedState))
                menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestTriforceOpening();
        } else if (!hdMenuMapIconInState(map,index,&sMenuTreasureUndecodedState))
            menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestTriforceClosed();
    } else if (kind==1) {
        if (hdMenuMapChartCollectedMap(l_hdMenuMapUi.mOwner->mUi,chart)) {
            if (hdMenuMapIconInState(map,index,&sMenuTreasureCompleteState))
                menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestTreasureOpening();
        } else if (!hdMenuMapIconInState(map,index,&sMenuTreasureCompleteState))
            menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestTreasureClosed();
    } else {
        if (hdMenuMapChartCollectedMap(l_hdMenuMapUi.mOwner->mUi,chart)) {
            if (hdMenuMapIconInState(map,index,&sMenuTreasureMissingState))
                menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestHelpfulOpening();
        } else if (!hdMenuMapIconInState(map,index,&sMenuTreasureMissingState))
            menuTreMap(map->mChartIcons[index<3 ? index : 0])->requestHelpfulClosed();
    }
}
struct sMenuMapUiSystemSlot_l { gptr<hdScreenUiSystem> mpObject; };
static sMenuMapUiSystemSlot_l sMenuMapUiSystem;
static hdUiSafeString sMenuMapAnimationNames[11] = {};
static hdUiSafeString sMenuMapAnimationGroups[7] = {};
static u32 sMenuMapAnimationSlots[11] = {0x0,0x1,0x3,0x3,0x4,0x4,0x2,0x5,0x5,0x6,0x0};
void hdMenuMapBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
/* 0269D8A0 */
u32 hdMenuMap_c::loadLayout(void* name,u32 resource,u32 options,u32 heapOptions,void* descriptor) {
    hdPartsLayout_c* layout=(hdPartsLayout_c*)hdParts_new(0x110,sMenuMapUiSystem.mpObject->mHeap,4);
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,options,heapOptions,11,7,descriptor)) return 0;
    for (u32 i=0;i<11;++i)
        hdMenuMapBindAnimation(animSet(),i,&sMenuMapAnimationNames[i],&sMenuMapAnimationGroups[sMenuMapAnimationSlots[i]]);
    vCalcLayout(); return 1;
}
struct hdMenuMapOpeningCells_l { u8 mPadding[0x118]; gptr<HdPadMapCell> mpSelected; u8 mPaddingPrevious[4]; gptr<HdPadMapCell> mpPrevious; u8 mPaddingOpening[0x1B]; be<u8> mOpening; };
void hdMenuMapClearCellSelection(HdPadMapCell*);
void hdMenuMapClearPreviousCell(HdPadMapCell*);
void hdMenuMapSetCurrentCell(HdPadMapCell*);
void hdMenuMapSetOpeningCell(HdPadMapCell*);
void hdMenuMapUpdateCell(HdPadMapCell*);
void hdMenuMapStartOpeningCell(HdPadMapCell*);
/* 0269A51C */
void hdMenuMap_c::openDemoEnter() {
    hdSeaChartMenu_c* owner=mpMap;
    HdPadGrid* grid=menuMapCells(owner);
    hdBtnMapIcon_c* icons=menuMapIcons(owner);
    HdPadMapCell* cell=hdMenuMapFindCell(grid,&l_hdMenuMapUi.mOwner->mUi->mCoordinate);
    icons->setClipBounds(reinterpret_cast<hdMapIconBounds_c*>(&cell->mBounds)); icons->resetScroll();
    icons->showPrimaryImmediately(); icons->showIslandAtOrigin(reinterpret_cast<hdMapIconSelection_c*>(cell));
    hdMenuMapOpeningCells_l* cells=(hdMenuMapOpeningCells_l*)grid;
    HdPadMapCell* selected=cells->mpSelected;
    if (selected) { hdMenuMapClearCellSelection(selected); hdMenuMapUpdateCell(selected); }
    HdPadMapCell* previous=cells->mpPrevious;
    if (previous) { hdMenuMapClearPreviousCell(previous); hdMenuMapUpdateCell(previous); }
    hdMenuMapSetCurrentCell(cell); hdMenuMapSetOpeningCell(cell); hdMenuMapUpdateCell(cell);
    cells->mOpening=1;
    hdMenuMapSetInputDefault(sMenuMapInputManager.mpObject,((hdMenuCellReceiver_l*)cell)->mReceiver);
    hdMenuMapSetIslandCaption(this,cell); icons->setShipVisible(0); icons->setTreasureVisible(0);
    hdAnimSet_operationCC(animSet(),7,5);
    u32 id=cell->mId;
    if (id==0 || id==10 || id==43) hdMenuMapEnableAnimation(animSet(),6,2);
    else hdAnimSet_play(animSet(),6,2,1.0f);
    icons->revealIconEnter(); hdMenuMapStartOpeningCell(cell);
    hdMenuMapPlayNamedSound("SE_SO_DRAW_MAP");
    mIconFrame=0; mBusy=1; mSoundFrame=0;
}
struct hdMenuMapPartDescription_l { u8 mPadding[8]; hdScreenString mClass; u8 mPaddingName[4]; hdScreenString mName; };
static hdScreenString sMenuMapIconClass;
static hdScreenString sMenuMapTreasureClass;
static hdScreenString sMenuMapSeaClass;
static hdScreenString sMenuMapBalloonClass;
hdParts_c* hdMenuMapConstructBalloon(hdParts_c*);
u32 hdMenuMapInitializeBalloon(hdParts_c*,u32,u32,hdPartsDesc_c*);
static bool hdMenuMapSameString(hdScreenString* text,hdScreenString* reference) {
    text->assureTermination(); text->assureTermination();
    const be<u8>* first=(const be<u8>*)text->mText.get();
    reference->assureTermination();
    if (gabi::ea(first)==reference->mText.v) return true;
    first=(const be<u8>*)text->mText.get(); const be<u8>* second=(const be<u8>*)reference->mText.get();
    for (u32 i=0;i<0x40001;++i) {
        u8 a=first[i],b=second[i];
        if (a!=b) return false;
        if (!a) return true;
    }
    return false;
}
/* 0269D2CC */
u32 hdMenuMap_c::createChildFromDescription(hdPartsDesc_c* description) {
    hdScreenHeap* heap=sMenuMapUiSystem.mpObject->mHeap;
    hdMenuMapPartDescription_l* desc=(hdMenuMapPartDescription_l*)description;
    for (u32 i=0;i<10;++i) {
        if (!hdMenuMapSameString(&desc->mName,&sMenuMapPaneNames[i])) continue;
        if (hdMenuMapSameString(&desc->mClass,&sMenuMapIconClass)) {
            gptr<hdParts_c>* destination=childSlot(i);
            hdParts_c* part=(hdParts_c*)hdParts_new(0x8C,heap,4);
            if (part) part=reinterpret_cast<hdParts_c*>(hdBtnMapPattern_c::construct(reinterpret_cast<hdBtnMapPattern_c*>(part)));
            *destination=part; if (!child(i)) return 0;
            return reinterpret_cast<hdBtnMapPattern_c*>(child(i))->initialize(0,0,gabi::ea(description));
        }
        if (hdMenuMapSameString(&desc->mClass,&sMenuMapTreasureClass)) {
            gptr<hdParts_c>* destination=childSlot(i);
            hdParts_c* part=(hdParts_c*)hdParts_new(0x94,heap,4);
            if (part) part=reinterpret_cast<hdParts_c*>(hdTreMapScreen_c::construct(reinterpret_cast<hdTreMapScreen_c*>(part)));
            *destination=part; if (!child(i)) return 0;
            return menuTreMap(child(i))->initialize(0,0,description);
        }
        if (hdMenuMapSameString(&desc->mClass,&sMenuMapSeaClass)) {
            gptr<hdParts_c>* destination=childSlot(i);
            hdParts_c* part=(hdParts_c*)hdParts_new(0x164,heap,4);
            if (part) part=reinterpret_cast<hdParts_c*>(hdSeaChartMenu_c::construct(reinterpret_cast<hdSeaChartMenu_c*>(part)));
            *destination=part; if (!child(i)) return 0;
            return reinterpret_cast<hdSeaChartMenu_c*>(child(i))->create(0,0,gabi::ea(description));
        }
        if (hdMenuMapSameString(&desc->mClass,&sMenuMapBalloonClass)) {
            gptr<hdParts_c>* destination=childSlot(i);
            hdParts_c* part=(hdParts_c*)hdParts_new(0x94,heap,4);
            if (part) part=hdMenuMapConstructBalloon(part);
            *destination=part; if (!child(i)) return 0;
            return hdMenuMapInitializeBalloon(child(i),0,0,description);
        }
        return hdParts_c::createChild(i,description);
    }
    return 1;
}
struct hdMenuMapInitSettings_l {
    be<f32> mMinimumAngle,mMaximumAngle,mNearDistance,mFarDistance,mViewDistance,mClipDistance;
    be<u8> mMathInitializer,mRandomInitializer;
};
static hdMenuMapInitSettings_l sMenuMapInitSettings;
static u32 sMenuMapInitWords[4] = {};
static u32 sMenuMapInitRegistration[3] = {};
static u32 sMenuMapMathRegistration[3] = {};
static u32 sMenuMapRandomRegistration[3] = {};
static hdScreenString sMenuMapStaticNames[53] = {};
static hdPartsStateId_c sMenuMapStateIds[18] = {};
static hdPartsStateIdVtbl_c sMenuMapStateIdVtable;
static hdStateId sMenuMapNullStateId;
static hdStateId sMenuMapPartsBlankStateId;
static hdStateId sMenuMapPartsDisplayStateId;
static u32 sMenuMapStateCounterReady;
static hdPartsStateCounter_c sMenuMapStateCounter;
void hdMenuMapRegisterStatics(void*);
void hdMenuMapInitializeMath(void*);
void hdMenuMapInitializeRandom(void*);
/* 0269E2DC */
static void initializeMenuMapStatics() {
    for (s32 i=3;i>=0;--i) sMenuMapInitWords[i]=0;
    hdMenuMapRegisterStatics(sMenuMapInitRegistration);
    sMenuMapInitSettings.mMinimumAngle=-3.1415927410125732f;
    sMenuMapInitSettings.mMaximumAngle=3.1415927410125732f;
    hdMenuMapInitializeMath(&sMenuMapInitSettings.mMathInitializer);
    hdMenuMapRegisterStatics(sMenuMapMathRegistration);
    hdMenuMapInitializeRandom(&sMenuMapInitSettings.mRandomInitializer);
    hdMenuMapRegisterStatics(sMenuMapRandomRegistration);
    sMenuMapInitSettings.mFarDistance=50000.0f;
    sMenuMapInitSettings.mClipDistance=10000.0f;
    sMenuMapInitSettings.mNearDistance=50000.0f;
    sMenuMapInitSettings.mViewDistance=10000.0f;
    sMenuMapStaticNames[0].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[0].mText.v=gabi::guest_addr("N_WindNow_00");
    sMenuMapStaticNames[1].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[1].mText.v=gabi::guest_addr("N_Wind_00");
    sMenuMapStaticNames[2].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[2].mText.v=gabi::guest_addr("T_TitleTre_00");
    sMenuMapStaticNames[3].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[3].mText.v=gabi::guest_addr("T_AreaName_00");
    sMenuMapStaticNames[4].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[4].mText.v=gabi::guest_addr("T_NumTre_00");
    sMenuMapStaticNames[5].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[5].mText.v=gabi::guest_addr("T_NumTreChest_00");
    sMenuMapStaticNames[6].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[6].mText.v=gabi::guest_addr("G_All_00");
    sMenuMapStaticNames[7].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[7].mText.v=gabi::guest_addr("G_WindNow_00");
    sMenuMapStaticNames[8].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[8].mText.v=gabi::guest_addr("G_OpenDemo_00");
    sMenuMapStaticNames[9].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[9].mText.v=gabi::guest_addr("G_TreMap_00");
    sMenuMapStaticNames[10].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[10].mText.v=gabi::guest_addr("G_TreMapChange_00");
    sMenuMapStaticNames[11].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[11].mText.v=gabi::guest_addr("G_AreaName_00");
    sMenuMapStaticNames[12].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[12].mText.v=gabi::guest_addr("G_NoData_00");
    sMenuMapStaticNames[13].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[13].mText.v=gabi::guest_addr("InAll");
    sMenuMapStaticNames[14].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[14].mText.v=gabi::guest_addr("WindowNowLoop");
    sMenuMapStaticNames[15].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[15].mText.v=gabi::guest_addr("TreMapOut");
    sMenuMapStaticNames[16].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[16].mText.v=gabi::guest_addr("TreMapIn");
    sMenuMapStaticNames[17].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[17].mText.v=gabi::guest_addr("TreMapChangeL");
    sMenuMapStaticNames[18].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[18].mText.v=gabi::guest_addr("TreMapChangeR");
    sMenuMapStaticNames[19].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[19].mText.v=gabi::guest_addr("OpenDemo");
    sMenuMapStaticNames[20].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[20].mText.v=gabi::guest_addr("AreaNameOut");
    sMenuMapStaticNames[21].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[21].mText.v=gabi::guest_addr("AreaNameIn");
    sMenuMapStaticNames[22].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[22].mText.v=gabi::guest_addr("NoData");
    sMenuMapStaticNames[23].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[23].mText.v=gabi::guest_addr("OutAll");
    sMenuMapStaticNames[24].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[24].mText.v=gabi::guest_addr("L_TreMapArrowL_00");
    sMenuMapStaticNames[25].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[25].mText.v=gabi::guest_addr("L_TreMapArrowR_00");
    sMenuMapStaticNames[26].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[26].mText.v=gabi::guest_addr("L_BtnMapIcon_01");
    sMenuMapStaticNames[27].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[27].mText.v=gabi::guest_addr("L_BtnFishMan_00");
    sMenuMapStaticNames[28].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[28].mText.v=gabi::guest_addr("L_BtnMapIconL_01");
    sMenuMapStaticNames[29].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[29].mText.v=gabi::guest_addr("L_BtnMapIconR_01");
    sMenuMapStaticNames[30].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[30].mText.v=gabi::guest_addr("L_TreMap_00");
    sMenuMapStaticNames[31].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[31].mText.v=gabi::guest_addr("L_TreMapR_00");
    sMenuMapStaticNames[32].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[32].mText.v=gabi::guest_addr("L_TreMapL_00");
    sMenuMapStaticNames[33].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[33].mText.v=gabi::guest_addr("L_Map_00");
    sMenuMapStaticNames[34].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[34].mText.v=gabi::guest_addr("BtnMenuArrow_00");
    sMenuMapStaticNames[35].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[35].mText.v=gabi::guest_addr("BtnMenuArrow_00");
    sMenuMapStaticNames[36].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[36].mText.v=gabi::guest_addr("BtnMapIcon_01");
    sMenuMapStaticNames[37].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[37].mText.v=gabi::guest_addr("BtnFishMan_00");
    sMenuMapStaticNames[38].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[38].mText.v=gabi::guest_addr("BtnMapIcon_01");
    sMenuMapStaticNames[39].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[39].mText.v=gabi::guest_addr("BtnMapIcon_01");
    sMenuMapStaticNames[40].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[40].mText.v=gabi::guest_addr("TreMap_00");
    sMenuMapStaticNames[41].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[41].mText.v=gabi::guest_addr("TreMap_00");
    sMenuMapStaticNames[42].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[42].mText.v=gabi::guest_addr("TreMap_00");
    sMenuMapStaticNames[43].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[43].mText.v=gabi::guest_addr("Map_00");
    sMenuMapStaticNames[44].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[44].mText.v=gabi::guest_addr("MenuMap_00");
    sMenuMapStaticNames[45].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[45].mText.v=gabi::guest_addr("BtnMenuArrow_00");
    sMenuMapStaticNames[46].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[46].mText.v=gabi::guest_addr("BtnMapIcon_01");
    sMenuMapStaticNames[47].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[47].mText.v=gabi::guest_addr("BtnFishMan_00");
    sMenuMapStaticNames[48].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[48].mText.v=gabi::guest_addr("TreMap_00");
    sMenuMapStaticNames[49].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[49].mText.v=gabi::guest_addr("Map_00");
    sMenuMapStaticNames[50].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[50].mText.v=gabi::guest_addr("BtnMapBalloon_00");
    sMenuMapStaticNames[51].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[51].mText.v=gabi::guest_addr("MapAreaNow_00");
    sMenuMapStaticNames[52].mVtable=&sMenuMapStringVtable;
    sMenuMapStaticNames[52].mText.v=gabi::guest_addr("LinkPos_00");
    sMenuMapLayoutName.mVtable=&sMenuMapStringVtable;
    sMenuMapLayoutName.mText.v=gabi::guest_addr("MenuMap_00");
    u32 number=0;
    if (sMenuMapStateCounterReady) number=sMenuMapStateCounter.mValue;
    else sMenuMapStateCounterReady=1;
    sMenuMapStateIds[0].setup(s32(++number),"::StateID_InAll",&sMenuMapStateIdVtable,25,&sMenuMapNullStateId);
    sMenuMapStateIds[1].setup(s32(++number),"::StateID_WindowNowLoop",&sMenuMapStateIdVtable,28,&sMenuMapNullStateId);
    sMenuMapStateIds[2].setup(s32(++number),"::StateID_TreMapOut",&sMenuMapStateIdVtable,31,&sMenuMapNullStateId);
    sMenuMapStateIds[3].setup(s32(++number),"::StateID_TreMapIn",&sMenuMapStateIdVtable,34,&sMenuMapNullStateId);
    sMenuMapStateIds[4].setup(s32(++number),"::StateID_TreMapChangeL",&sMenuMapStateIdVtable,37,&sMenuMapNullStateId);
    sMenuMapStateIds[5].setup(s32(++number),"::StateID_TreMapChangeR",&sMenuMapStateIdVtable,40,&sMenuMapNullStateId);
    sMenuMapStateIds[6].setup(s32(++number),"::StateID_OpenDemo",&sMenuMapStateIdVtable,43,&sMenuMapNullStateId);
    sMenuMapStateIds[7].setup(s32(++number),"::StateID_AreaNameOut",&sMenuMapStateIdVtable,46,&sMenuMapNullStateId);
    sMenuMapStateIds[8].setup(s32(++number),"::StateID_AreaNameIn",&sMenuMapStateIdVtable,49,&sMenuMapNullStateId);
    sMenuMapStateIds[9].setup(s32(++number),"::StateID_ChangeToNoData",&sMenuMapStateIdVtable,52,&sMenuMapNullStateId);
    sMenuMapStateIds[10].setup(s32(++number),"::StateID_ChangeToDisplay",&sMenuMapStateIdVtable,55,&sMenuMapNullStateId);
    sMenuMapStateIds[11].setup(s32(++number),"::StateID_NoData",&sMenuMapStateIdVtable,58,&sMenuMapNullStateId);
    sMenuMapStateIds[12].setup(s32(++number),"::StateID_OutAll",&sMenuMapStateIdVtable,61,&sMenuMapNullStateId);
    sMenuMapStateIds[13].setup(s32(++number),"::StateID_Display",&sMenuMapStateIdVtable,7,&sMenuMapPartsDisplayStateId);
    sMenuMapStateIds[14].setup(s32(++number),"::StateID_Blank",&sMenuMapStateIdVtable,4,&sMenuMapPartsBlankStateId);
    sMenuMapStateIds[15].setup(s32(++number),"::StateID_MapHelpfulInAll",&sMenuMapStateIdVtable,64,&sMenuMapNullStateId);
    sMenuMapStateIds[16].setup(s32(++number),"::StateID_MapHelpfulOutAll",&sMenuMapStateIdVtable,67,&sMenuMapNullStateId);
    sMenuMapStateIds[17].setup(s32(++number),"::StateID_MapHelpfulDisplay",&sMenuMapStateIdVtable,70,&sMenuMapNullStateId);
    sMenuMapStateCounter.mValue=number;
}
WWHD_OFFSET(hdMenuEventOwner_l,mPending,0x18);
WWHD_OFFSET(hdMenuMapScreenContext_l,mpOptions,0x48);
WWHD_OFFSET(hdMenuMapScreenContext_l,mpResource,0x4C);
WWHD_OFFSET(hdMenuMapScreenContext_l,mFlags,0x50);
WWHD_OFFSET(hdMenuMapScreenContext_l,mInlineOptions,0x84);
WWHD_OFFSET(hdMenuMapScreenContext_l,mpSpecialResource,0x164);
WWHD_OFFSET(hdMenuMapScreens_l,mCount,0x1020);
WWHD_OFFSET(hdMenuMapScreens_l,mpScreens,0x1024);
WWHD_OFFSET(hdMenuMapEventButtonVtbl_l,mHandleKind,0xE4);
WWHD_OFFSET(hdMenuMapEventButtonVtbl_l,mHandleEvent,0xF4);
static_assert(sizeof(HdPadEvent)==0x28);
WWHD_OFFSET(HdPadEvent,mKind,0);
WWHD_OFFSET(HdPadEvent,mId,4);
WWHD_OFFSET(hdMenuMapInputQueue_l,mQueue,0x14);
WWHD_OFFSET(hdMenuMapInputQueue_l,mEvent,0x30);
WWHD_OFFSET(hdMenuMapOpeningCells_l,mpSelected,0x118);
WWHD_OFFSET(hdMenuMapOpeningCells_l,mpPrevious,0x120);
WWHD_OFFSET(hdMenuMapOpeningCells_l,mOpening,0x13F);
WWHD_OFFSET(hdMenuMapPartDescription_l,mClass,8);
WWHD_OFFSET(hdMenuMapPartDescription_l,mName,0x14);
WWHD_OFFSET(hdMenuMapInitSettings_l,mMathInitializer,0x18);
WWHD_OFFSET(hdMenuMapInitSettings_l,mRandomInitializer,0x19);

/* WWHD glue (tools/verify/autoglue.py)
data sChartViewHelpfulState 10496290
data sMenuMapHelpfulDisplayState 104975DC
data sMenuMapChangeToDisplayState 1049748C

ext 026F2458 u32 hdMenuMapButtonActive(hdParts_c*)
ext 02679B88 void hdSeaChartMenu_c::setSelectionMode(u32)
ext 02679CCC void hdSeaChartMenu_c::bindActiveButton()
ext 02006478 hdPartsStateId_c* hdMenuMapCurrentState(hdStateMachine*)
ext 026860D0 void hdMenuMapHideUi(HdPadUi*)
ext 02680D1C void hdMenuMapResetCells(HdPadGrid*)
data sMenuMapHolderVtable 100FBB00
data sMenuMapSecondaryVtable 100FBB10
data sMenuMapBlankState 1049754C
ext 0257DACC u32 hdMenuMapGetWindAngle()
ext 0268A3F0 u32 hdMenuMapTotalCharts(HdPadUi*)
ext 026891F8 u32 hdMenuMapHasIsland(HdPadUi*)
ext 0269EE74 void hdMenuMapFormatCount(hdMenuCountString_l*,be<u16>*,u32,u32)
data sChartCountFormat 100FB3F4
data sTreasureCountFormat 100FB404
data sChartKindTable 100FB360
data sChartKindFrames 100FB3EE
data sMenuMapDisplayFlags 1047B074
data sMenuMapInputManager 101F4FF0
ext 02687A64 hdMenuChartInfo_l* hdMenuMapChartInfo(HdPadUi*,u32)
ext 0268A5DC u32 hdMenuMapChartCollected(HdPadUi*,u32)
ext 0268A6B4 u32 hdMenuMapChartDecoded(HdPadUi*,u32)
ext 0268A648 u32 hdMenuMapChartComplete(HdPadUi*,u32)
ext 0263B708 void hdBtnMapPattern_c::setIcon(u32,u32)
ext 026ED094 void hdTreMapScreen_c::requestTreasureOpen()
ext 026ED064 void hdTreMapScreen_c::requestTriforceUnopened()
ext 026ED054 void hdTreMapScreen_c::requestTriforceClosed()
ext 026ED084 void hdTreMapScreen_c::requestTreasureClosed()
ext 026ED034 void hdTreMapScreen_c::requestHelpfulOpen()
ext 026ED024 void hdTreMapScreen_c::requestHelpfulClosed()
ext 026ECDB0 void hdTreMapScreen_c::selectChart(u32)
ext 02615154 void hdMenuMapSetInputDefault(void*,u32)
data sMenuMapNoDataState 104974BC
data sMenuMapSave 101F84DC
ext 02680DE0 HdPadMapCell* hdMenuMapFindCell(HdPadGrid*,HdPadCoordinate*)
ext 025B8B94 u32 hdMenuMapEventBit(void*,u32)
ext 026881E0 void hdMenuMapResetUi(HdPadUi*)
ext 02688514 void hdMenuMapRefreshUi(HdPadUi*)
ext 02679D68 void hdSeaChartMenu_c::openMap()
ext 02679D78 void hdSeaChartMenu_c::closeMap()
ext 02001BE4 void hdMenuMapConnect(void*,u32,u32)
data sChartViewDisplayState 10496200
data sChartViewBlankState 10496230
data sMenuMapHelpfulOutAllState 104975AC
ext 02681A9C u32 hdMenuMapCellReceiver(HdPadGrid*,HdPadCoordinate*)
ext 02680748 void hdMenuMapSetCellReceiver(HdPadMapCell*,u32,u32)
ext 026819F0 void hdMenuMapResetCellReceivers(HdPadGrid*)
ext 026826D8 void hdMenuMapConnectCellReceivers(HdPadGrid*)
data sMenuMapChangeToNoDataState 1049745C
data sMapArrowIndices 100FB1E8


ext 02679E30 void hdSeaChartMenu_c::pollButtonEvent()
ext 02679FE4 void hdSeaChartMenu_c::pollSelectionActivation()
ext 0267A034 void hdSeaChartMenu_c::processMapAction()
ext 025B8484 void hdMenuMapMarkDiscovered(void*,u32)
ext 02638144 void hdBtnMapIcon_c::showPrimaryImmediately()
ext 02639FB4 void hdBtnMapIcon_c::setShipVisible(u32)
ext 02639FF0 void hdBtnMapIcon_c::setTreasureVisible(u32)
data sMenuMapVtable 100FB8B8
data sMenuMapActiveInput 101F8378
ext 02615140 HdPadInputMode* hdMenuMapInputMode(void*)
ext 02682C30 HdPadMapCell* hdMenuMapFindReceiverCell(HdPadGrid*,HdPadInputMode*)
ext 025200D4 hdMenuMapPlay_l* hdMenuMapPlay()
ext 0271F6F4 void hdMenuMapChaseAngle(be<s32>*,be<s32>*,be<s32>*,be<s32>*,f32)
data sMenuMapActiveState 104972AC
data sMenuMapInactiveState 104974EC
data sMenuMapSwitchState 104973CC
data sChartViewWaitState 10496260
data sMenuEventOwner 101F5A44
data sMapButtonIndices 100FB25C
ext 0268A3A0 u32 hdMenuMapOwnedCharts(HdPadUi*)
ext 0268A4E8 u32 hdMenuMapOwnedTreasures(HdPadUi*)
ext 02689174 u32 hdMenuMapIslandDiscovered(HdPadUi*,u32)
ext 0268A570 u32 hdMenuMapChartOwned(HdPadUi*,u32)
ext 0268936C u32 hdMenuMapUiBusy(HdPadUi*)
ext 02005708 void hdMenuMapEnableAnimation(hdAnimSet*,u32,u32)
ext 027045F0 void hdMenuMapSetCaption(void*,u32)
ext 026F5C00 void hdMenuMapSetButtonReceiver(hdParts_c*,u32,u32)
ext 026F5B74 u32 hdMenuMapGetButtonReceiver(hdParts_c*)
ext 02679C6C void hdSeaChartMenu_c::updateOpenMap()
data sMenuMapArchiveNames 10497264 nocheck
data sMenuMapLayoutName 1049760C
data sMenuMapHelpfulInAllState 1049757C
ext 02684514 void hdMenuMapExitHelpfulChart(hdMenuChartView_l*)
data sMenuMapWindNames 10497104 nocheck
data sMenuMapPaneNames 104971C4 nocheck
data sMenuMapCaptionNames 10497114 nocheck
data sMenuMapCaptionMessages 100FB24C
data sMenuMapEmptyText 100FB47C
data sMenuMapStringVtable 100FB204
ext 02704AFC void hdMenuMapSetCaptionMessage(void*,u32)
ext 026F2540 void hdMenuMapSetArrowSound(hdParts_c*,hdScreenString*,u32)
data sMenuMapScreens 101F95D0
data sMenuMapDefaultOptions 104A2098
data sMenuMapDefaultResource 104A20FC
ext 0269F0A4 u32 hdMiiverseChartFactory_c::createTincleTower(gptr<hdParts_c>*,void*,u32)
ext 0269F644 void hdMiiverseChartFactory_c::destroyTincleTower(hdParts_c*)
ext 0269F11C u32 hdMiiverseChartFactory_c::createFairy(gptr<hdParts_c>*,void*,u32)
ext 0269F69C void hdMiiverseChartFactory_c::destroyFairy(hdParts_c*)
ext 0269F194 u32 hdMiiverseChartFactory_c::createHeartIsland(gptr<hdParts_c>*,void*,u32)
ext 0269F6F4 void hdMiiverseChartFactory_c::destroyHeartIsland(hdParts_c*)
ext 0269F20C u32 hdMiiverseChartFactory_c::createSquid(gptr<hdParts_c>*,void*,u32)
ext 0269F74C void hdMiiverseChartFactory_c::destroySquid(hdParts_c*)
ext 0269F284 u32 hdMiiverseChartFactory_c::createTriforce(gptr<hdParts_c>*,void*,u32)
ext 0269F7A4 void hdMiiverseChartFactory_c::destroyTriforce(hdParts_c*)
ext 0269F2FC u32 hdMiiverseChartFactory_c::createTerry(gptr<hdParts_c>*,void*,u32)
ext 0269F7FC void hdMiiverseChartFactory_c::destroyTerry(hdParts_c*)
ext 0269F374 u32 hdMiiverseChartFactory_c::createSecretEntrance(gptr<hdParts_c>*,void*,u32)
ext 0269F854 void hdMiiverseChartFactory_c::destroySecretEntrance(hdParts_c*)
ext 0269F3EC u32 hdMiiverseChartFactory_c::createSubmarine(gptr<hdParts_c>*,void*,u32)
ext 0269F8AC void hdMiiverseChartFactory_c::destroySubmarine(hdParts_c*)
ext 0269F464 u32 hdMiiverseChartFactory_c::createEnemyTower(gptr<hdParts_c>*,void*,u32)
ext 0269F904 void hdMiiverseChartFactory_c::destroyEnemyTower(hdParts_c*)
ext 0269F4DC u32 hdMiiverseChartFactory_c::createMoon(gptr<hdParts_c>*,void*,u32)
ext 0269F95C void hdMiiverseChartFactory_c::destroyMoon(hdParts_c*)
ext 0269F554 u32 hdMiiverseChartFactory_c::createHeartSea(gptr<hdParts_c>*,void*,u32)
ext 0269F9B4 void hdMiiverseChartFactory_c::destroyHeartSea(hdParts_c*)
ext 0269F5CC u32 hdMiiverseChartFactory_c::createGhostShip(gptr<hdParts_c>*,void*,u32)
ext 0269FA0C void hdMiiverseChartFactory_c::destroyGhostShip(hdParts_c*)
data sMenuMapButtonEventKinds 100FB26C
data sMenuMapTouchBlocked 1047B097
ext 02001338 void hdMenuMapConstructEvent(HdPadEvent*,u32,u32,u32)
ext 020013C0 void hdPadCopyEvent(HdPadEvent*,HdPadEvent*)
ext 02001490 void hdMenuMapDestroyEvent(HdPadEvent*)
ext 02001FF4 void hdMenuMapQueueEvent(void*,HdPadEvent*)
ext 026F630C u32 hdMenuMapEventDirection(HdPadEvent*)
ext 026F6288 u32 hdMenuMapEventConfirmed(HdPadEvent*)
ext 026F61EC u32 hdMenuMapKindDirection(u32)
ext 026F61DC u32 hdMenuMapKindConfirmed(u32)
ext 026F620C u32 hdMenuMapKindCancelled(u32)
ext 02030B38 void hdMenuMapPlayNamedSound(const char*)
data sMenuTreasureMissingState 1049D144
data sMenuTreasureUndecodedState 1049D1D4
data sMenuTreasureDecodedState 1049D204
data sMenuTreasureCompleteState 1049D264
ext 0268A5DC u32 hdMenuMapChartCollectedMap(HdPadUi*,u32)
ext 0268A52C u32 hdMenuMapChartCollectedItem(HdPadUi*,u32)
ext 026ED074 void hdTreMapScreen_c::requestTreasureOpening()
ext 026ED044 void hdTreMapScreen_c::requestTriforceOpening()
ext 026ED014 void hdTreMapScreen_c::requestHelpfulOpening()
data sMenuMapUiSystem 1018C404
data sMenuMapAnimationNames 1049716C nocheck
data sMenuMapAnimationGroups 10497134 nocheck
data sMenuMapAnimationSlots 100FB27C
ext 02004E04 void hdMenuMapBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 0263A010 void hdBtnMapIcon_c::setClipBounds(hdMapIconBounds_c*)
ext 0263A348 void hdBtnMapIcon_c::resetScroll()
ext 02639F70 void hdBtnMapIcon_c::showIslandAtOrigin(hdMapIconSelection_c*)
ext 02680364 void hdMenuMapClearCellSelection(HdPadMapCell*)
ext 02680358 void hdMenuMapClearPreviousCell(HdPadMapCell*)
ext 0268034C void hdMenuMapSetCurrentCell(HdPadMapCell*)
ext 02680340 void hdMenuMapSetOpeningCell(HdPadMapCell*)
ext 0267FF00 void hdMenuMapUpdateCell(HdPadMapCell*)
ext 02639C28 void hdBtnMapIcon_c::revealIconEnter()
ext 02680578 void hdMenuMapStartOpeningCell(HdPadMapCell*)
data sMenuMapIconClass 104905DC
data sMenuMapTreasureClass 1049D2C4
data sMenuMapSeaClass 10495F0C
data sMenuMapBalloonClass 10490768
ext 0263B56C hdBtnMapPattern_c* hdBtnMapPattern_c::construct(hdBtnMapPattern_c*)
ext 026EC1B8 hdTreMapScreen_c* hdTreMapScreen_c::construct(hdTreMapScreen_c*)
ext 02677BFC hdSeaChartMenu_c* hdSeaChartMenu_c::construct(hdSeaChartMenu_c*)
ext 0263D438 hdParts_c* hdMenuMapConstructBalloon(hdParts_c*)
ext 0263B5D8 u32 hdBtnMapPattern_c::initialize(u32,u32,u32)
ext 026EC7C4 u32 hdTreMapScreen_c::initialize(u32,u32,hdPartsDesc_c*)
ext 02678308 u32 hdSeaChartMenu_c::create(u32,u32,u32)
ext 0263D4C8 u32 hdMenuMapInitializeBalloon(hdParts_c*,u32,u32,hdPartsDesc_c*)
ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
data sMenuMapInitSettings 104970D8
data sMenuMapInitWords 104970F4 nocheck
data sMenuMapInitRegistration 101F63C8 nocheck
data sMenuMapMathRegistration 101F63D4 nocheck
data sMenuMapRandomRegistration 101F63E0 nocheck
data sMenuMapStaticNames 10497104 nocheck
data sMenuMapStateIds 104972AC nocheck
data sMenuMapStateIdVtable 100FB2F0
data sMenuMapNullStateId 101FF32C
data sMenuMapPartsBlankStateId 1049DD60
data sMenuMapPartsDisplayStateId 1049DD90
data sMenuMapStateCounterReady 101FD8E4 nocheck
data sMenuMapStateCounter 101FDD50 nocheck
ext 028F026C void hdMenuMapRegisterStatics(void*)
ext 028ED6F8 void hdMenuMapInitializeMath(void*)
ext 028EAB2C void hdMenuMapInitializeRandom(void*)
str 100FB420 "SE_SO_DRAW_MAP"
str 100FB45C "SE_MENU_CHART_OPEN_CLOSE_S"
str 100FB480 "SE_MENU_CHART_SWITCH"
str 100FB4A8 "InAll"
str 100FB4B0 "NoData"
str 100FB4B8 "OutAll"
str 100FB4C0 "Map_00"
str 100FB4C8 "L_BtnMapIcon_01"
str 100FB4D8 "L_BtnFishMan_00"
str 100FB4E8 "BtnMenuArrow_00"
str 100FB4F8 "::StateID_InAll"
str 100FB508 "::StateID_WindowNowLoop"
str 100FB520 "::StateID_TreMapChangeL"
str 100FB538 "::StateID_TreMapChangeR"
str 100FB550 "::StateID_Blank"
str 100FB560 "T_NumTre_00"
str 100FB56C "G_TreMap_00"
str 100FB578 "G_NoData_00"
str 100FB584 "AreaNameOut"
str 100FB590 "L_TreMap_00"
str 100FB59C "::StateID_TreMapOut"
str 100FB5B0 "::StateID_MapHelpfulDisplay"
str 100FB5CC "N_Wind_00"
str 100FB5D8 "T_TitleTre_00"
str 100FB5E8 "T_AreaName_00"
str 100FB5F8 "G_OpenDemo_00"
str 100FB608 "G_TreMapChange_00"
str 100FB61C "G_AreaName_00"
str 100FB62C "WindowNowLoop"
str 100FB63C "TreMapOut"
str 100FB648 "TreMapChangeL"
str 100FB658 "TreMapChangeR"
str 100FB668 "L_TreMapArrowL_00"
str 100FB67C "L_TreMapArrowR_00"
str 100FB690 "BtnMapIcon_01"
str 100FB6A0 "BtnFishMan_00"
str 100FB6B0 "TreMap_00"
str 100FB6BC "MapAreaNow_00"
str 100FB6CC "::StateID_AreaNameOut"
str 100FB6E4 "::StateID_ChangeToDisplay"
str 100FB700 "::StateID_Display"
str 100FB714 "::StateID_MapHelpfulInAll"
str 100FB730 "N_WindNow_00"
str 100FB740 "T_NumTreChest_00"
str 100FB754 "G_All_00"
str 100FB760 "G_WindNow_00"
str 100FB770 "TreMapIn"
str 100FB77C "OpenDemo"
str 100FB788 "AreaNameIn"
str 100FB794 "L_BtnMapIconL_01"
str 100FB7A8 "L_BtnMapIconR_01"
str 100FB7BC "L_TreMapR_00"
str 100FB7CC "L_TreMapL_00"
str 100FB7DC "L_Map_00"
str 100FB7E8 "MenuMap_00"
str 100FB7F4 "BtnMapBalloon_00"
str 100FB808 "LinkPos_00"
str 100FB814 "::StateID_TreMapIn"
str 100FB828 "::StateID_OpenDemo"
str 100FB83C "::StateID_AreaNameIn"
str 100FB854 "::StateID_ChangeToNoData"
str 100FB870 "::StateID_NoData"
str 100FB884 "::StateID_OutAll"
str 100FB898 "::StateID_MapHelpfulOutAll"
# Canonical draw-map companion bindings (its original unit provided these in dungeon_map).
ext 020063C0 void hdPadChangeState(HdPadStateController*,HdPadState*)
ext 02639C54 void hdPadIconsOpening(void*)
ext 02639E4C void hdPadIconsWaitInit(void*)
ext 02639E50 void hdPadIconsWaiting(void*)
ext 02639EE0 void hdPadIconsClosing(void*)
ext 02639CF0 void hdPadIconsFinish(void*)
ext 02030B38 void hdPadUiSound(void*)
ext 025E1918 void hdPadPlaySound(u32)
ext 02005840 u32 hdPadAnimationEnded(void*,u32)
ext 02639C44 u32 hdPadIconsEnded(void*)
ext 02680610 u32 hdPadCellEnded(HdPadMapCell*)
ext 02679D68 void hdPadMapBeginDraw(HdPadMapOwner*)
ext 02679D78 void hdPadMapEndDraw(HdPadMapOwner*)
ext 02680DE0 HdPadMapCell* hdGridFind(HdPadCellArray*,HdPadCoordinate*)
ext 02006364 void hdPadStateExecute(HdPadStateController*)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
ext 026F91F8 void hdParts_c::detachArchives()
ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
*/
