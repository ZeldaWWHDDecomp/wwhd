/* HD treasure-chart display and its three chart-opening state families; no GameCube counterpart. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
#include "d/hd_sea_chart_components.h"
#include "d/hd_screen_cursor.h"

struct hdTreMapParameters_c {
    be<u32> mFlags;
    be<f32> mHelpfulScaleX, mHelpfulScaleY, mTreasureScaleX, mTreasureScaleY;
    be<f32> mTriforceScaleX, mTriforceScaleY;
    be<u32> mMode;
    be<f32> mDepth;
};
WWHD_SIZE(hdTreMapParameters_c, 0x24);
struct hdTreMapGroupNode_l {
    gptr<hdTreMapGroupNode_l> mpNext, mpPrevious;
    gptr<hdSeaChartPane_l> mpPane;
};
struct hdTreMapGroup_l { u8 mPrefix[0xC]; hdTreMapGroupNode_l mPanes; };
struct hdTreMapRegistrationWords_l { be<u32> mWords[4]; };
struct hdTreMapAngleRange_l { be<f32> mMinimum, mMaximum; HdPadPoint mCellSize; };
struct hdTreMapGroupNames_l { hdUiSafeString mNames[6]; };
struct hdTreMapAnimationNames_l { hdUiSafeString mNames[8]; };
struct hdTreMapAnimationGroups_l { be<u32> mIndices[8]; };
struct hdTreMapSaveKind_l { u8 mPrefix[0x1C1]; be<u8> mChartKind; };
WWHD_OFFSET(hdTreMapSaveKind_l, mChartKind, 0x1C1);
struct hdTreMapPaneNames_l { hdUiSafeString mNames[2]; };
struct hdTreMapNamedDesc_l { u8 mPrefix[0x14]; hdUiSafeString mName; u8 mTail[0xC]; };
WWHD_OFFSET(hdTreMapNamedDesc_l, mName, 0x14);
WWHD_SIZE(hdTreMapNamedDesc_l, sizeof(hdPartsDesc_c));
struct hdTreMapMode_l { u8 mPrefix[0xA0]; be<u8> mSeaMode; };
struct hdTreMapOwner_l {
    u8 mPrefix[0x1C4]; gptr<hdTreMapMode_l> mpMode;
    u8 mModePadding[0x50]; gptr<hdMapDisplayCoordinator_c> mpCoordinator;
};
WWHD_OFFSET(hdTreMapMode_l, mSeaMode, 0xA0);
WWHD_OFFSET(hdTreMapOwner_l, mpMode, 0x1C4);
WWHD_OFFSET(hdTreMapOwner_l, mpCoordinator, 0x218);
struct hdTreMapGroupManager_l;
struct hdTreMapLayoutGroups_l { u8 mPrefix[0x10]; gptr<hdTreMapGroupManager_l> mpGroups; };
WWHD_OFFSET(hdTreMapGroupNode_l, mpNext, 0);
WWHD_OFFSET(hdTreMapGroupNode_l, mpPane, 8);
WWHD_OFFSET(hdTreMapGroup_l, mPanes, 0xC);
WWHD_OFFSET(hdTreMapLayoutGroups_l, mpGroups, 0x10);
struct hdTreMapScreen_c : NoControlPart_l {
    enum { GROUP_COUNT = 4, PATTERN_COUNT = 2, ANIMATION_COUNT = 8, ANIMATION_GROUP_COUNT = 6, NAME_SCAN_BUDGET = 0x40001, NO_CHART = -1 };
    enum Animation { HELPFUL_OPEN = 0, TRIFORCE_OPEN = 1, TREASURE_OPEN = 2, TREASURE_GET = 7 };
    enum { OPEN_SLOT = 1, TREASURE_SLOT = 0, PANE_VISIBLE = 1, PANE_DIRTY = 0x10 };
    gptr<hdTreMapGroup_l> mGroups[GROUP_COUNT];
    gptr<hdSeaChartPane_l> mpLinkPane;
    gptr<hdSeaChartPane_l> mPatternPanes[PATTERN_COUNT];
    gptr<hdSeaChartPane_l> mpMapPane, mpPositionPane;
    HdPadPoint mOrigin, mHalfSize, mScale;
    be<s32> mChartIndex;
    be<u8> mChartKind, mTreasureShown;
    u8 mPadding[2];
    static void destroy(hdTreMapScreen_c*, u32);
    static hdTreMapScreen_c* construct(hdTreMapScreen_c*);
    void helpfulOpeningEnter(); void helpfulOpeningExecute();
    void helpfulClosedEnter(); void helpfulClosedExecute();
    void helpfulOpenEnter(); void helpfulOpenExecute();
    void triforceOpeningEnter(); void triforceOpeningExecute();
    void triforceClosedEnter(); void triforceClosedExecute();
    void triforceUnopenedEnter(); void triforceUnopenedExecute();
    void treasureOpeningEnter(); void treasureOpeningExecute();
    void treasureClosedEnter(); void treasureClosedExecute();
    void treasureOpenEnter(); void treasureOpenExecute();
    void requestHelpfulOpening(); void requestHelpfulClosed(); void requestHelpfulOpen();
    void requestTriforceOpening(); void requestTriforceClosed(); void requestTriforceUnopened();
    void requestTreasureOpening(); void requestTreasureClosed(); void requestTreasureOpen();
    hdTreMapGroup_l* findGroup(hdUiSafeString* name);
    u32 collectGroups(gptr<hdTreMapGroup_l>* output, hdUiSafeString* names, be<s32>* count);
    void setGroupVisible(hdTreMapGroup_l* group, u32 visible);
    void findLinkPane(); void findPatternPanes(); void findMapPane(); void findPositionPane();
    u32 createNamedChild(hdTreMapNamedDesc_l* desc);
    void initializePanes();
    u32 initialize(u32 first, u32 second, hdPartsDesc_c* desc);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* desc);
    void checkTreasure(); void executeState();
    void updatePosition(); void selectChart(u32 chart);
    void showTreasure(); void hideTreasure(); void beginTreasure();
};
WWHD_OFFSET(hdTreMapScreen_c, mGroups, 0x50);
WWHD_OFFSET(hdTreMapScreen_c, mpLinkPane, 0x60);
WWHD_OFFSET(hdTreMapScreen_c, mPatternPanes, 0x64);
WWHD_OFFSET(hdTreMapScreen_c, mpMapPane, 0x6C);
WWHD_OFFSET(hdTreMapScreen_c, mpPositionPane, 0x70);
WWHD_OFFSET(hdTreMapScreen_c, mOrigin, 0x74);
WWHD_OFFSET(hdTreMapScreen_c, mHalfSize, 0x7C);
WWHD_OFFSET(hdTreMapScreen_c, mScale, 0x84);
WWHD_OFFSET(hdTreMapScreen_c, mChartIndex, 0x8C);
WWHD_OFFSET(hdTreMapScreen_c, mChartKind, 0x90);
WWHD_OFFSET(hdTreMapScreen_c, mTreasureShown, 0x91);
WWHD_SIZE(hdTreMapScreen_c, 0x94);

WWHD_OFFSET(hdTreMapParameters_c, mFlags, 0);
WWHD_OFFSET(hdTreMapParameters_c, mHelpfulScaleX, 4);
WWHD_OFFSET(hdTreMapParameters_c, mHelpfulScaleY, 8);
WWHD_OFFSET(hdTreMapParameters_c, mTreasureScaleX, 12);
WWHD_OFFSET(hdTreMapParameters_c, mTreasureScaleY, 16);
WWHD_OFFSET(hdTreMapParameters_c, mTriforceScaleX, 20);
WWHD_OFFSET(hdTreMapParameters_c, mTriforceScaleY, 24);
WWHD_OFFSET(hdTreMapParameters_c, mMode, 28);
WWHD_OFFSET(hdTreMapParameters_c, mDepth, 32);
WWHD_OFFSET(hdTreMapAngleRange_l, mMinimum, 0);
WWHD_OFFSET(hdTreMapAngleRange_l, mMaximum, 4);
WWHD_OFFSET(hdTreMapAngleRange_l, mCellSize, 8);
WWHD_SIZE(hdTreMapAngleRange_l, 16);
WWHD_SIZE(hdTreMapPaneNames_l, 16);
WWHD_SIZE(hdTreMapGroupNames_l, 48);
WWHD_SIZE(hdTreMapAnimationNames_l, 64);
WWHD_SIZE(hdTreMapAnimationGroups_l, 32);
