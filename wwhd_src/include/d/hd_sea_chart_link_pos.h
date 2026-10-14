#pragma once
#include "d/hd_sea_chart_components.h"

WWHD_OFFSET(hdButtonPaneVtable_l, mFindPane, 0x5C);

WWHD_OFFSET(hdBasicButton_c, mButton, 0);
WWHD_OFFSET(hdBasicButton_c, mRequestedState, 0x78);
WWHD_OFFSET(hdButtonPartVtable_l, mOnFocusChanged, 0xCC);
struct hdSeaChartLinkVtable_l : hdPartsVtbl_c {
    u8 mReservedC8[0x34];
    gfn mEnterIn;
};
WWHD_OFFSET(hdSeaChartLinkVtable_l, mEnterIn, 0xFC);
struct hdSeaChartPaneArray_c {
    be<s32> mCount, mCapacity;
    gptr<gptr<hdSeaChartPane_l>> mData;
};
WWHD_OFFSET(hdSeaChartPaneArray_c, mCount, 0);
WWHD_OFFSET(hdSeaChartPaneArray_c, mCapacity, 4);
WWHD_OFFSET(hdSeaChartPaneArray_c, mData, 8);
WWHD_SIZE(hdSeaChartPaneArray_c, 0xC);
struct hdSeaChartLinkPos_c : hdParts_c {
    be<u32> mInAnimation, mOutAnimation, mAnimationSlot, mSelectedChild;
    be<s32> mPaneNameCount;
    hdSeaChartPaneArray_c mPanes;
    gptr<hdSeaChartPane_l> mHitPane, mGridPane, mLinkGridPane, mSelectedMark, mIconPane,
        mPlayerPositionPane, mPlayerRotationPane, mIconHitPane;
    gptr<hdUiSafeString> mPaneNames;
    HdPadPoint mCellSize, mPlayerCellSize;
    HdPadPoint mPositions[49];
    HdPadPoint mPlayerPositions[49];
    be<u8> mInputEnabled, mCancelRequested, mTouchHeld, mTouchReleased, mCompactLayout;
    u8 mPadding3B9[3];
    hdSeaChartLinkVtable_l* vtbl() {
        return reinterpret_cast<hdSeaChartLinkVtable_l*>(mVtable.get());
    }
    hdBasicButton_c* selectedChild() {
        return reinterpret_cast<hdBasicButton_c*>(mChildren.safeSlot(mSelectedChild)->get());
    }
    void selectInputReceiver();
    u32 isTouchHeldOutside(hdSeaChartPane_l* pane);
    u32 isTouchTriggeredOutside(hdSeaChartPane_l* pane);
    u32 isTouchRepeatedOutside(hdSeaChartPane_l* pane);
    void updateInput();
    void updatePlayerPosition();
    void updateSelectedMark();
    void updateSelectedInput();
    void executeDisplay();
    void enterIn();
    void executeIn();
    void enterOut();
    void executeOut();
    void releasePanes();
    void destroyLayout();
    void enterBlank();
    void allocatePanes(s32 capacity);
    void findPanes();
    void setupLayout();
    void configureAnimations(u32 inAnimation, u32 outAnimation, u32 slot, u32 selectedChild);
};
WWHD_SIZE(hdSeaChartLinkPos_c, 0x3BC);
WWHD_OFFSET(hdSeaChartLinkPos_c, mInAnimation, 0x50);
WWHD_OFFSET(hdSeaChartLinkPos_c, mOutAnimation, 0x54);
WWHD_OFFSET(hdSeaChartLinkPos_c, mAnimationSlot, 0x58);
WWHD_OFFSET(hdSeaChartLinkPos_c, mSelectedChild, 0x5C);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPaneNameCount, 0x60);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPanes, 0x64);
WWHD_OFFSET(hdSeaChartLinkPos_c, mHitPane, 0x70);
WWHD_OFFSET(hdSeaChartLinkPos_c, mGridPane, 0x74);
WWHD_OFFSET(hdSeaChartLinkPos_c, mLinkGridPane, 0x78);
WWHD_OFFSET(hdSeaChartLinkPos_c, mSelectedMark, 0x7C);
WWHD_OFFSET(hdSeaChartLinkPos_c, mIconPane, 0x80);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPlayerPositionPane, 0x84);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPlayerRotationPane, 0x88);
WWHD_OFFSET(hdSeaChartLinkPos_c, mIconHitPane, 0x8C);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPaneNames, 0x90);
WWHD_OFFSET(hdSeaChartLinkPos_c, mCellSize, 0x94);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPlayerCellSize, 0x9C);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPositions, 0xA4);
WWHD_OFFSET(hdSeaChartLinkPos_c, mPlayerPositions, 0x22C);
WWHD_OFFSET(hdSeaChartLinkPos_c, mInputEnabled, 0x3B4);
WWHD_OFFSET(hdSeaChartLinkPos_c, mCancelRequested, 0x3B5);
WWHD_OFFSET(hdSeaChartLinkPos_c, mTouchHeld, 0x3B6);
WWHD_OFFSET(hdSeaChartLinkPos_c, mTouchReleased, 0x3B7);
WWHD_OFFSET(hdSeaChartLinkPos_c, mCompactLayout, 0x3B8);
