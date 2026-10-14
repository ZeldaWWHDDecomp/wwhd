#pragma once
#include "d/hd_screen_enemy_tower.h"
struct hdMoon_c : hdMapChartBase_c {
    static hdMoon_c* construct(hdMoon_c*);
    static void destroy(hdMoon_c*, u32 flags);
    u32 receiveTouch(); u32 receiveKey(); u32 receivePointer(); u32 receiveFocus(); u32 receiveCancel();
    void configure();
    u32 createNamedChild(hdMapChartDesc_l* desc);
    u32 load(void* context, void* resource, void* option);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* desc);
    void finalize();
    void executeState();
};
WWHD_SIZE(hdMoon_c, 0x3BC);
