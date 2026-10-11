/* HD-only map icon coordinates and pane placement. */
#pragma once
#include "bindings.h"
struct HdMapPoint { be<f32> x, y; };
struct HdMapCell { be<s8> x, y; };
struct HdMapInfo {
    u8 padding[4];
    HdMapCell mCell;
    be<s16> mOffsetX, mOffsetY;
};
struct HdMapIconOwner { u8 padding[0x118]; gptr<HdMapInfo> mpInfo; };
struct HdMapIconPane {
    u8 padding[0x1C];
    be<f32> mTranslateX, mTranslateY, mTranslateZ;
    u8 padding2[0xC];
    be<f32> mScaleX, mScaleY;
    u8 padding3[8];
    be<u8> mFlags;
};
struct HdMapLocations {
    u8 padding[0x30];
    HdMapPoint mPrimaryPosition;
    u8 padding2[4];
    HdMapCell mPrimaryCell;
    u8 padding3[2];
    HdMapPoint mSecondaryPosition;
    u8 padding4[4];
    HdMapCell mSecondaryCell;
};
struct HdMapManager { u8 padding[0x218]; gptr<HdMapLocations> mpLocations; };
struct HdMapManagerPointer { gptr<HdMapManager> mpManager; };
struct HdMapIcon {
    u8 padding[0x94];
    gptr<HdMapIconPane> mpParentPane;
    gptr<HdMapIconPane> mpSecondaryPane;
    u8 padding2[0x1C];
    gptr<HdMapIconPane> mpPrimaryPane;
    u8 padding3[0x18];
    HdMapPoint mOrigin;
    u8 padding4[8];
    HdMapPoint mPosition;
    u8 padding5[0x5C];
    HdMapPoint mTranslateScale;
    HdMapPoint mPointScale;
    u8 padding6[0xC];
    gptr<HdMapIconOwner> mpOwner;
    u32 matchesPrimaryCell();
    u32 matchesSecondaryCell();
    void getPrimaryMapPoint(HdMapPoint*);
    void getSecondaryMapPoint(HdMapPoint*);
    u32 insideBounds(HdMapPoint*);
    void getPrimaryRelativePoint(HdMapPoint*);
    void getSecondaryRelativePoint(HdMapPoint*);
    void updatePrimaryInside();
    void updateSecondaryInside();
    void updatePrimaryOutside();
    void updateSecondaryOutside();
};
WWHD_SIZE(HdMapPoint, 8);
WWHD_SIZE(HdMapCell, 2);
WWHD_SIZE(HdMapInfo, 10);
WWHD_SIZE(HdMapIconOwner, 0x11C);
WWHD_OFFSET(HdMapIconPane, mFlags, 0x44);
WWHD_SIZE(HdMapIconPane, 0x45);
WWHD_OFFSET(HdMapLocations, mPrimaryCell, 0x3C);
WWHD_OFFSET(HdMapLocations, mSecondaryCell, 0x4C);
WWHD_SIZE(HdMapLocations, 0x4E);
WWHD_SIZE(HdMapManager, 0x21C);
WWHD_SIZE(HdMapManagerPointer, 4);
WWHD_OFFSET(HdMapIcon, mOrigin, 0xD4);
WWHD_OFFSET(HdMapIcon, mPosition, 0xE4);
WWHD_OFFSET(HdMapIcon, mTranslateScale, 0x148);
WWHD_OFFSET(HdMapIcon, mPointScale, 0x150);
WWHD_OFFSET(HdMapIcon, mpOwner, 0x164);
WWHD_SIZE(HdMapIcon, 0x168);
f32 hdMapSqrt(f32);
u32 hdMapInsideOutsideBounds(HdMapIcon*, HdMapPoint*);
