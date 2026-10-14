/** HD GhostShip chart and lunar-phase visibility. No GameCube screen counterpart. */
#pragma once
#include "d/hd_screen_enemy_tower.h"
#include "d/d_save.h"
struct hdGhostShipPane_l : hdPartsPane_c {
    u8 mTransformStorage[0x28]; be<u8> mFlags;
};
struct hdGhostShipPaneArray_l { gptr<hdGhostShipPane_l> mPanes[8]; };
struct hdGhostShipNames_l { hdUiSafeString mNames[8]; };
struct hdGhostShipAnimationNames_l { hdUiSafeString mNames[10]; };
struct hdGhostShipAnimationRemap_l { be<u32> mIndices[10]; };
struct hdGhostShipSaveRoot_l { u8 mRootPrefix[0x20]; dSv_info_c mInfo; };
struct hdGhostShip_c : hdMapChartBase_c {
    hdGhostShipPaneArray_l mMoonPanes;
    be<u8> mMoonVisible;
    u8 mTail[3];
    static hdGhostShip_c* construct(hdGhostShip_c*);
    static void destroy(hdGhostShip_c*, u32 flags);
    u32 receiveTouch(); u32 receiveKey(); u32 receivePointer(); u32 receiveFocus();
    void configure(); void collectMoonPanes(); void finalize(); void updateMoon(); void executeState(); void resetMoon();
    u32 load(void*, void*, void*);
    u32 loadLayout(void*, u32, u32, u32, void*);
    u32 createNamedChild(hdMapChartDesc_l*);
};
WWHD_OFFSET(hdGhostShipPane_l, mFlags, 0x44);
WWHD_SIZE(hdGhostShipPane_l, 0x45);
WWHD_SIZE(hdGhostShipPaneArray_l, 0x20);
WWHD_SIZE(hdGhostShipNames_l, 0x40);
WWHD_SIZE(hdGhostShipAnimationNames_l, 0x50);
WWHD_SIZE(hdGhostShipAnimationRemap_l, 0x28);
WWHD_OFFSET(hdGhostShipSaveRoot_l, mInfo, 0x20);
WWHD_OFFSET(hdGhostShip_c, mMoonPanes, 0x3BC);
WWHD_OFFSET(hdGhostShip_c, mMoonVisible, 0x3DC);
WWHD_SIZE(hdGhostShip_c, 0x3E0);
u32 ghostShipMoonPhase();
struct hdGhostShipArchives_l { hdUiSafeString mNames[2]; };
WWHD_SIZE(hdGhostShipArchives_l, 0x10);

struct hdGhostShipEntryNames_l { hdUiSafeString mNames[2]; };
struct hdGhostShipAnimationSourceNames_l { hdUiSafeString mNames[9]; };
WWHD_SIZE(hdGhostShipEntryNames_l, 0x10);
WWHD_SIZE(hdGhostShipAnimationSourceNames_l, 0x48);
