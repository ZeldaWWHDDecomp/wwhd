#pragma once
#include "d/hd_sea_chart_link_pos.h"

// The sea-chart transition parts bind a pane transform to the owning map and its viewport.
struct hdSeaChartTransitionMapVtable_l {
    u8 mReserved[0x1C4];
    gfn mGetViewport;
};
WWHD_OFFSET(hdSeaChartTransitionMapVtable_l, mGetViewport, 0x1C4);
struct hdSeaChartTransitionMap_l {
    u8 mReserved[4];
    gptr<hdSeaChartTransitionMapVtable_l> mVtable;
    u8 mReserved8[0x4A4];
    be<f32> mRotation;
    u8 mReserved4B0[4];
    gptr<be<f32>> mScale;
    u8 mReserved4B8[0x48];
    be<f32> mBlend;
};
WWHD_OFFSET(hdSeaChartTransitionMap_l, mVtable, 4);
WWHD_OFFSET(hdSeaChartTransitionMap_l, mRotation, 0x4AC);
WWHD_OFFSET(hdSeaChartTransitionMap_l, mScale, 0x4B4);
WWHD_OFFSET(hdSeaChartTransitionMap_l, mBlend, 0x500);
struct hdSeaChartPaneTransition_c {
    gptr<hdSeaChartPane_l> mRoot, mTarget;
    HdPadPoint mSourcePosition, mReservedPosition, mPosition;
    u8 mReserved20[8];
    be<f32> mScale, mBlend, mReservedBlend, mRotation;
    be<u8> mAlpha, mPaneIndex, mReservedFlag, mHidden, mDirty;
    u8 mPadding3D[3];
};
WWHD_SIZE(hdSeaChartPaneTransition_c, 0x40);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mRoot, 0);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mTarget, 4);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mSourcePosition, 8);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mPosition, 0x18);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mScale, 0x28);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mBlend, 0x2C);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mRotation, 0x34);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mAlpha, 0x38);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mPaneIndex, 0x39);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mHidden, 0x3B);
WWHD_OFFSET(hdSeaChartPaneTransition_c, mDirty, 0x3C);
struct hdSeaChartTransitionVtable_l : hdPartsVtbl_c {
    u8 mReservedC8[0x4C];
    gfn mUpdatePosition;
    u8 mReserved118[4];
    gfn mUpdateRotation;
    u8 mReserved120[4];
    gfn mScaleMapPoint;
    u8 mReserved128[4];
    gfn mRotateViewportPoint;
    u8 mReserved130[4];
    gfn mScaleViewportPoint;
    u8 mReserved138[4];
    gfn mOffsetViewportPoint;
};
WWHD_OFFSET(hdSeaChartTransitionVtable_l, mUpdatePosition, 0x114);
WWHD_OFFSET(hdSeaChartTransitionVtable_l, mUpdateRotation, 0x11C);
WWHD_OFFSET(hdSeaChartTransitionVtable_l, mScaleMapPoint, 0x124);
WWHD_OFFSET(hdSeaChartTransitionVtable_l, mRotateViewportPoint, 0x12C);
WWHD_OFFSET(hdSeaChartTransitionVtable_l, mScaleViewportPoint, 0x134);
WWHD_OFFSET(hdSeaChartTransitionVtable_l, mOffsetViewportPoint, 0x13C);
struct hdSeaChartStateHandlers_c : hdParts_c {
    hdSeaChartPaneTransition_c mTransition;
    gptr<hdSeaChartTransitionMap_l> mMap;
    gptr<hdSeaChartTransform_c> mViewport;
    be<f32> mMinimumBlend, mMaximumBlend;
    hdSeaChartTransitionVtable_l* vtbl() {
        return reinterpret_cast<hdSeaChartTransitionVtable_l*>(mVtable.get());
    }
    void executeDisplay();
    void updatePosition();
    void executeIn();
    void executeOut();
    void bindMap(hdSeaChartTransitionMap_l* map);
    void rotateViewportPoint(HdPadPoint* input, HdPadPoint* output);
    void offsetViewportPoint(HdPadPoint* input, HdPadPoint* output);
    void bindPane(hdUiSafeString* name);
    void bindRootAndPane(hdUiSafeString* name);
    void clearAlpha();
    void fillAlpha();
    void calcLayout();
    void requestIn();
    void requestOut();
    void displayImmediately();
    void blankImmediately();
    void updateBlend();
    void updateRotation();
    void setBlendRange(f32 minimum, f32 maximum);
    void scaleMapPoint(HdPadPoint* input, HdPadPoint* output);
    void scaleViewportPoint(HdPadPoint* input, HdPadPoint* output);
};
WWHD_SIZE(hdSeaChartStateHandlers_c, 0xA0);
WWHD_OFFSET(hdSeaChartStateHandlers_c, mTransition, 0x50);
WWHD_OFFSET(hdSeaChartStateHandlers_c, mMap, 0x90);
WWHD_OFFSET(hdSeaChartStateHandlers_c, mViewport, 0x94);
WWHD_OFFSET(hdSeaChartStateHandlers_c, mMinimumBlend, 0x98);
WWHD_OFFSET(hdSeaChartStateHandlers_c, mMaximumBlend, 0x9C);

/* hd_sea_chart_state_handlers.cpp TU state (data glue there) */
extern hdPartsStateId_c& sTransitionBlankState;  /* 1049631C ::StateID_Blank */
