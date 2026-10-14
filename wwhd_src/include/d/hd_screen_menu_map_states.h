/* HD pause-menu sea chart controller. Reconstructed from the game binary; private source.
 * The canonical parts base owns the layout, child array and state machine. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_lyt.h"
#include "d/hd_parts_basic_button.h"
#include "d/hd_gamepad_map.h"
#include "d/hd_screen_map_display_coordinator.h"
#include "d/hd_screen_sea_chart_menu.h"
#include "d/hd_btn_map_icon_extra.h"
#include "d/hd_screen_tre_map.h"
/* the menu map's owner is the merged sea-chart menu (MAP-sea_chart#1): its map pane holds the cell grid and its active
 * button is the map icon button (MAP-btn_map_icon) */
inline HdPadGrid* menuMapCells(hdSeaChartMenu_c* owner) { return reinterpret_cast<HdPadGrid*>(owner->mpMap.get()); }
inline hdBtnMapIcon_c* menuMapIcons(hdSeaChartMenu_c* owner) { return reinterpret_cast<hdBtnMapIcon_c*>(owner->mpActiveButton.get()); }
/* the chart icons are the treasure-map screens (MAP-tre_map) and the map pattern buttons (MAP-btn_map_icon) */
inline hdTreMapScreen_c* menuTreMap(hdParts_c* part) { return reinterpret_cast<hdTreMapScreen_c*>(part); }
struct hdMenuChartView_l : hdParts_c {
    u8 mPadding[0x364];
    be<u8> mEnabled;
};
WWHD_OFFSET(hdMenuChartView_l, mEnabled, 0x3B4);
/* the wind arrow's rotation is written as a float into the canonical pane's raw rotation word */
inline void hdMenuMapSetPaneRotationZ(hdPartsPaneSRT_c* pane, f32 degrees) {
    reinterpret_cast<be<f32>&>(pane->mRotationZBits) = degrees;
}
struct hdMenuMap_c : hdParts_c {
    enum ChartChangePhase { CHART_IDLE=0, CHART_START=1, CHART_WAIT=2, CHART_FINISH=3, CHART_UNINITIALIZED=4 };
    void cacheWindPanes(); void cachePanes(); void cacheCaptions(); void initializeCaptions();
    void initializeLayout(); void initializeArrowSounds();
    u32 load(void*,u32,void*); void unload(); u32 isHelpful();
    void createChartViews(); void destroyChartViews();
    void postButtonEvent(u32);
    u32 loadLayout(void*,u32,u32,u32,void*);
    void openDemoEnter();
    u32 createChildFromDescription(hdPartsDesc_c*);
    static hdMenuMap_c* construct(hdMenuMap_c*);
    void startChartChange(); void updateChartChange(); void displayExecute();
    void enableButtons(); void configureArrows(); void markIslandDiscovered(); void openDemoExit();
    void connectChartButtons(); void connectIconsToButton(); void connectButtonToIcons();
    void connectCellsToButton(); void connectButtonToCells();
    void mapHelpfulInAllExecute(); void mapHelpfulReturnExecute(); void mapHelpfulOutAllExecute();
    void connectLeftMapReceiver(void*); void connectBottomMapReceiver(void*);
    void setRightMapReceivers(u32); void setBottomMapReceivers(u32);
    void updateIslandCaption(); void inAllExecute();
    void changeToNoDataEnter(); void changeToNoDataExecute();
    void connectChartReceiver(void* receiver); void setLeftReceiver(u32 receiver);
    void updateWindVisibility(); void waitChartChange(); void finishChartChange(); void restoreInputMode();
    void resetWind(); void refreshCaptions(); void inAllEnter();
    void displayImmediately(); void hideImmediately();
    void updateIslandAnimation(); void updateIslandChanges(); void showSingleChart();
    void updateChangedCaptions(); void activateBalloon(); void updateEnabledInput();
    void cacheChartIcons(); void cacheMap(); u32 isDisplay(); u32 isMapHelpfulDisplay();
    void showSelectedChart(); void changeToDisplayEnter(); void changeToDisplayExecute();
    void noDataEnter(); void noDataExecute();
    void updateChartCountCaption(); void updateTreasureCountCaption(); void updateBalloonVisibility();
    /* 0x050 */ gptr<hdMenuChartView_l> mChartViews[12];
    /* 0x080 */ gptr<hdMenuChartView_l> mpSelectedChartView;
    /* 0x084 */ gptr<hdParts_c> mChartIcons[3];
    /* 0x090 */ be<u32> mChartChangePhase;
    /* 0x094 */ be<u32> mOwnedCharts;
    /* 0x098 */ be<u32> mPreviousOwnedCharts;
    /* 0x09C */ be<u32> mOwnedTreasures;
    /* 0x0A0 */ be<u32> mPreviousOwnedTreasures;
    /* 0x0A4 */ be<u32> mDiscoveredIslands;
    /* 0x0A8 */ be<u32> mPreviousDiscoveredIslands;
    /* 0x0AC */ be<s32> mSelectedChart;
    /* 0x0B0 */ be<u32> mOutgoingChart;
    /* 0x0B4 */ be<s32> mPreviousSelectedChart;
    /* 0x0B8 */ be<u32> mChartDirection;
    /* 0x0BC */ be<u8> mInputFlags;
    /* 0x0BD */ be<u8> mInputChanged;
    /* 0x0BE */ be<u8> mOwnedChartsChanged;
    /* 0x0BF */ be<u8> mOwnedTreasuresChanged;
    /* 0x0C0 */ be<u8> mDiscoveredIslandsChanged;
    /* 0x0C1 */ be<u8> mSelectedChartChanged;
    u8 mPaddingCounter[2];
    /* 0x0C4 */ be<u32> mSoundFrame;
    /* 0x0C8 */ gptr<hdPartsPaneSRT_c> mpWindPane;
    /* 0x0CC */ gptr<hdPartsPaneSRT_c> mpWindNowPane;
    /* 0x0D0 */ gptr<hdPartsPaneSRT_c> mpLeftPane;
    /* 0x0D4 */ gptr<hdPartsPaneSRT_c> mpRightPane;
    gptr<hdPartsPaneSRT_c> mpTreMapPane;
    /* 0x0DC */ gptr<hdPartsPaneSRT_c> mpBalloonPane;
    gptr<hdPartsPaneSRT_c> mCaptionPanes[6];
    /* 0x0F8 */ gptr<void> mpChartCaption;
    /* 0x0FC */ gptr<void> mpIslandCaption;
    /* 0x100 */ gptr<void> mpChartCountCaption;
    /* 0x104 */ gptr<void> mpTreasureCountCaption;
    /* 0x108 */ be<s16> mWindAngle;
    /* 0x10A */ be<s16> mPreviousWindAngle;
    /* 0x10C */ be<s32> mWindRotation;
    /* 0x110 */ be<s32> mPreviousWindRotation;
    /* 0x114 */ be<s32> mIslandMessage;
    /* 0x118 */ be<s32> mPreviousIslandMessage;
    /* 0x11C */ gptr<hdSeaChartMenu_c> mpMap;
    /* 0x120 */ be<u8> mEnabled;
    /* 0x121 */ be<u8> mCloseRequested;
    /* 0x122 */ be<u8> mBusy;
    u8 mPaddingBusy;
    /* 0x124 */ be<u32> mIconFrame;
    /* 0x128 */ be<u8> mLeftVisible;
    /* 0x129 */ be<u8> mRightVisible;
    /* 0x12A */ be<u8> mIconsVisible;
    u8 mPaddingEnd;

    void updateOwnedCharts(); void updateOwnedTreasures(); void updateDiscoveredIslands();
    s32 findChart(s32 chart, s32 step);
    void setChartCaption(u32 chart); void resetChartSelection(); void resetCounters();
    void updateButtonVisibility(); void updatePaneVisibility(); void updateChartAvailability();
    void windowNowLoopEnter(); void windowNowLoopExecute();
    void treMapOutEnter(); void treMapOutExecute();
    void treMapInEnter(); void treMapInExecute();
    void treMapChangeLEnter(); void treMapChangeLExecute();
    void treMapChangeREnter(); void treMapChangeRExecute();
    void areaNameOutEnter(); void areaNameOutExecute();
    void areaNameInEnter(); void areaNameInExecute();
    void outAllEnter(); void outAllExecute();
    void requestClose(); void clearEvent(); void execute();
    void setEnabled(u32 enabled); u32 isBusy();
    void activate(); void deactivate(); void switchChart();
    void selectChartView(); void changeChartViewState(hdPartsStateId_c* state);
    void enterChartView(); void executeChartView(); void exitChartView();
    void refresh(); void refreshIslandCaption();
    u32 findButton(u32 receiver); u32 buttonIndex(u32 receiver);
};
WWHD_OFFSET(hdMenuMap_c, mChartViews, 0x50);
WWHD_OFFSET(hdMenuMap_c, mpSelectedChartView, 0x80);
WWHD_OFFSET(hdMenuMap_c, mOwnedCharts, 0x94);
WWHD_OFFSET(hdMenuMap_c, mSelectedChart, 0xAC);
WWHD_OFFSET(hdMenuMap_c, mpWindPane, 0xC8);
WWHD_OFFSET(hdMenuMap_c, mpChartCaption, 0xF8);
WWHD_OFFSET(hdMenuMap_c, mWindRotation, 0x10C);
WWHD_OFFSET(hdMenuMap_c, mpMap, 0x11C);
WWHD_OFFSET(hdMenuMap_c, mLeftVisible, 0x128);
WWHD_SIZE(hdMenuMap_c, 0x12C);

struct hdMenuTextPaneVtable_l { u8 mPadding[0xC4]; gfn mSetString; };
struct hdMenuTextPane_l { u8 mPadding[8]; gptr<hdMenuTextPaneVtable_l> mpVtable; };
struct hdMenuCountString_l { u8 mStorage[0x1C]; };
struct hdMenuChartViewList_l { gptr<hdMenuChartView_l> mViews[12]; };
WWHD_OFFSET(hdMenuTextPane_l, mpVtable, 8);
WWHD_OFFSET(hdMenuTextPaneVtable_l, mSetString, 0xC4);
WWHD_SIZE(hdMenuCountString_l, 0x1C);

struct hdMenuChartInfo_l { u8 mPadding[3]; be<s8> mKind; };
struct hdMenuChartKindEntry_l { be<u32> mKind, mGroup; };
struct hdMenuChartKindTable_l { hdMenuChartKindEntry_l mEntries[18]; };
using hdMenuMapButton_l = hdButtonPart_c;
WWHD_OFFSET(hdMenuChartInfo_l,mKind,3);


struct hdMenuMapUi_l { u8 mPadding[0x54]; be<u32> mHelpful; };
struct hdMenuMapSave_l { u8 mPadding[0xE4]; u8 mMapData; u8 mPaddingEvents[0x55F]; u8 mEvents; };
WWHD_OFFSET(hdMenuMapUi_l,mHelpful,0x54);
WWHD_OFFSET(hdMenuMapSave_l,mEvents,0x644);


struct hdMenuCellReceiver_l { u8 mPadding[0x9C]; be<u32> mReceiver; };
WWHD_OFFSET(hdMenuCellReceiver_l,mReceiver,0x9C);

struct hdMenuGridChanges_l { u8 mPadding[0x138]; be<u8> mSelectionChanged; u8 mPadSelection; be<u8> mPositionChanged; u8 mPadPosition; be<u8> mDiscoveryChanged; };
struct hdMenuMapButtonVtable_l { u8 mPadding[0xCC]; gfn mActivate; u8 mPaddingEnabled[0x174]; gfn mSetEnabled; };
struct hdMenuMapArrow_l { hdBasicButton_c mBasic; u8 mPadding[5]; be<u8> mAcceptedDirections; be<u8> mDirection; };
WWHD_OFFSET(hdMenuGridChanges_l,mSelectionChanged,0x138);
WWHD_OFFSET(hdMenuGridChanges_l,mDiscoveryChanged,0x13C);
WWHD_OFFSET(hdMenuMapButtonVtable_l,mSetEnabled,0x244);
WWHD_OFFSET(hdMenuMapArrow_l,mDirection,0x92);

struct hdMenuMapPlay_l { u8 mPadding[0x5C28]; be<u8> mButtonHint; };
WWHD_OFFSET(hdMenuMapPlay_l,mButtonHint,0x5C28);
