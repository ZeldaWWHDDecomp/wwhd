/* HD dungeon-map marker parts, derived from the shared menu-map pane. */
#pragma once
#include "d/hd_menu_map_pane.h"
struct hdDungeonMapLink : hdMenuMapPanePart {
 static hdDungeonMapLink* construct(hdDungeonMapLink*);
 void destroy(u32);
 u32 loadMarker(u32, u32, void*);
};
struct hdDungeonMapMakore : hdMenuMapPanePart {
 static hdDungeonMapMakore* construct(hdDungeonMapMakore*);
 void destroy(u32);
 u32 loadMarker(u32, u32, void*);
};
struct hdDungeonMapMedley : hdMenuMapPanePart {
 static hdDungeonMapMedley* construct(hdDungeonMapMedley*);
 void destroy(u32);
 u32 loadMarker(u32, u32, void*);
};
struct hdDungeonMapShip : hdMenuMapPanePart {
 static hdDungeonMapShip* construct(hdDungeonMapShip*);
 void destroy(u32);
 u32 loadMarker(u32, u32, void*);
};
WWHD_SIZE(hdDungeonMapLink, 0xA4);
WWHD_SIZE(hdDungeonMapMakore, 0xA4);
WWHD_SIZE(hdDungeonMapMedley, 0xA4);
WWHD_SIZE(hdDungeonMapShip, 0xA4);
struct hdDungeonMarkerStatics {
 be<f32> mMinimum, mMaximum;
 HdPadPoint mOrigin, mOffset;
 be<u8> mFirstFlag, mSecondFlag;
 u8 mPadding[2];
 be<u32> mStorage[4];
};
struct hdDungeonMarkerRegistration { be<u32> mWords[3]; };
WWHD_OFFSET(hdDungeonMarkerStatics, mOrigin, 8);
WWHD_OFFSET(hdDungeonMarkerStatics, mOffset, 0x10);
WWHD_OFFSET(hdDungeonMarkerStatics, mFirstFlag, 0x18);
WWHD_OFFSET(hdDungeonMarkerStatics, mStorage, 0x1C);
WWHD_SIZE(hdDungeonMarkerStatics, 0x2C);
