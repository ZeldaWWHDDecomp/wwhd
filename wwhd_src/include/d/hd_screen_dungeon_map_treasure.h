/* HD DungeonMapTreasure_00: the treasure marker of the GamePad dungeon map (a sea-chart transition part). */
#pragma once
#include "d/hd_sea_chart_state_handlers.h"
#include "d/hd_screen_map_display_coordinator.h"

/* mTransition.mReservedFlag (+0x8A) holds the marker's visible bit (bit 0) here. */
struct hdDungeonMapTreasure_c : hdSeaChartStateHandlers_c {
    /* 0xA0 */ be<u32> mTreasureIndex;

    enum { TREASURE_VISIBLE = 1 };

    static hdDungeonMapTreasure_c* construct(hdDungeonMapTreasure_c* part);
    static void destroy(hdDungeonMapTreasure_c* part, s32 flags);
    u32 create(u32 archiveArg, u32 layoutArg, void* description);
    void release();
    void stateExecute();
    void drawLayoutVirtual(void* info);
    void update();
    void convertPosition(HdPadPoint* input, HdPadPoint* output);
    void setVisible(u32 visible);
    u32 isVisible();
};
WWHD_OFFSET(hdDungeonMapTreasure_c, mTreasureIndex, 0xA0);
WWHD_SIZE(hdDungeonMapTreasure_c, 0xA4);

/* the map display coordinator: the object at +0x218 of the UI owner (see hd_screen_dungeon_map_up.h) */
inline hdMapDisplayCoordinator_c* hdDungeonMapTreasure_coordinator() {
    return reinterpret_cast<hdMapDisplayCoordinator_c*>(l_hdDungeonUi.mOwner->mUi.get());
}

/* this TU's copy of the state-number getter (same body as 02006570 in hd_sys_02006294.cpp) */
u32 getDungeonTreasureStateNumber(hdPartsStateId_c* state);

/* merged sea-chart transition functions, the allocator and helpers, bound by address (glue) */
void treasureTransitionDestroy(hdSeaChartStateHandlers_c* self, s32 flags);
hdSeaChartStateHandlers_c* treasureTransitionConstruct(hdSeaChartStateHandlers_c* self);
void treasureTransitionBindPane(hdSeaChartStateHandlers_c* self, hdUiSafeString* name);
void treasureTransitionUpdateBlend(hdSeaChartStateHandlers_c* self);
void treasureTransitionApply(hdSeaChartStateHandlers_c* self);
hdDungeonMapTreasure_c* treasureAllocate(u32 size);
void treasureFree(hdDungeonMapTreasure_c* part);
void constructTreasureHeaderObject(void* object);
void constructTreasureHeaderOtherObject(void* object);
