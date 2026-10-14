/* HD dungeon boss marker and child-part factory callbacks. */
#pragma once
#include "d/hd_sea_chart_state_handlers.h"
struct hdDungeonChildVtable : hdPartsVtbl_c {
 u8 mReservedC8[0x34];
 gfn mLoadChild;
};
WWHD_OFFSET(hdDungeonChildVtable, mLoadChild, 0xFC);
struct hdDungeonMapBoss : hdSeaChartStateHandlers_c {
 static hdDungeonMapBoss* construct(hdDungeonMapBoss*);
 void destroy(u32);
 u32 loadBoss(u32,u32,void*);
 void finalizeBoss();
 void drawBoss(void*);

 void createMainFloor(gptr<hdParts_c>*, u32, u32);
 void deleteMainFloor(hdParts_c*);
 void createFloorFirst(gptr<hdParts_c>*, u32, u32);
 void deleteFloorFirst(hdParts_c*);
 void createFloorSecond(gptr<hdParts_c>*, u32, u32);
 void deleteFloorSecond(hdParts_c*);
 void createFloorThird(gptr<hdParts_c>*, u32, u32);
 void deleteFloorThird(hdParts_c*);
 void createFloorFourth(gptr<hdParts_c>*, u32, u32);
 void deleteFloorFourth(hdParts_c*);
 void createFloorFifth(gptr<hdParts_c>*, u32, u32);
 void deleteFloorFifth(hdParts_c*);
 void createBoss(gptr<hdParts_c>*, u32, u32);
 void deleteBoss(hdParts_c*);
 void createLink(gptr<hdParts_c>*, u32, u32);
 void deleteLink(hdParts_c*);
 void createMedley(gptr<hdParts_c>*, u32, u32);
 void deleteMedley(hdParts_c*);
 void createMakore(gptr<hdParts_c>*, u32, u32);
 void deleteMakore(hdParts_c*);
 void createShip(gptr<hdParts_c>*, u32, u32);
 void deleteShip(hdParts_c*);
 void createTreasure(gptr<hdParts_c>*, u32, u32);
 void deleteTreasure(hdParts_c*);
 void createDoor(gptr<hdParts_c>*, u32, u32);
 void deleteDoor(hdParts_c*);
 void createFloor(gptr<hdParts_c>*, u32, u32);
 void deleteFloor(hdParts_c*);
 void createUp(gptr<hdParts_c>*, u32, u32);
 void deleteUp(hdParts_c*);
 void createDown(gptr<hdParts_c>*, u32, u32);
 void deleteDown(hdParts_c*);
};
WWHD_SIZE(hdDungeonMapBoss, 0xA0);
