/* HD DungeonMapStatue_00: the statue marker of the GamePad dungeon map (a menu-map pane part). */
#pragma once
#include "d/hd_menu_map_pane.h"

struct hdDungeonMapStatue_c : hdMenuMapPanePart {
    /* 0xA4 */ be<u8> mFirstStatueSet;
    /* 0xA5 */ be<u8> mSecondStatueSet;
    /* 0xA6 */ u8 _A6[2];
    static hdDungeonMapStatue_c* construct(hdDungeonMapStatue_c* part);
    static void destroy(hdDungeonMapStatue_c* part, s32 flags);
    void setStatueEnter();
    void calcLayoutVirtual();
    u32 create(u32 archiveArg, u32 layoutArg, void* description);
    u32 load(void* name, u32 archiveArg, u32 layoutArg, u32 resourceArg, void* description);
    void setFirstStatue();
    void setSecondStatue();
};
WWHD_OFFSET(hdDungeonMapStatue_c, mFirstStatueSet, 0xA4);
WWHD_OFFSET(hdDungeonMapStatue_c, mSecondStatueSet, 0xA5);
WWHD_SIZE(hdDungeonMapStatue_c, 0xA8);

/* merged menu-map pane functions, the allocator and helpers, bound by address (glue). 02685784 is the
 * sea-chart transition's bindRootAndPane (hdSeaChartStateHandlers_c); the binary calls it on this part,
 * whose first 0xA0 bytes the two merged classes view differently. */
void statuePaneDestroy(hdMenuMapPanePart* self, u32 flags);
hdMenuMapPanePart* statuePaneConstruct(hdMenuMapPanePart* self);
void statueBindRootAndPane(hdMenuMapPanePart* self, hdUiSafeString* name);
hdDungeonMapStatue_c* statueAllocate(u32 size);
void statueFree(hdDungeonMapStatue_c* part);
void bindDungeonStatueAnimation(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group);
void constructStatueHeaderObject(void* object);
void constructStatueHeaderOtherObject(void* object);
