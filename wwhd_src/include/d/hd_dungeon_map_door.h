/* HD dungeon door marker. */
#pragma once
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_sea_chart_state_handlers.h"
struct hdDungeonMapDoor : hdSeaChartStateHandlers_c {
 be<s8> mDoorIndex, mPreviousDoorIndex;
 u8 mPadding[2];
 static hdDungeonMapDoor* construct(hdDungeonMapDoor*);
 void destroy(u32);
 u32 loadMarker(u32,u32,void*);
 void finalizeDoor();
 void executeDoor();
 void drawDoor(void*);
 void scalePosition(HdPadPoint*,HdPadPoint*);
};
WWHD_OFFSET(hdDungeonMapDoor,mDoorIndex,0xA0);
WWHD_SIZE(hdDungeonMapDoor,0xA4);
