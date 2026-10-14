#pragma once
#include "d/hd_screen_enemy_tower.h"
struct hdHeartSea_c : hdMapChartBase_c {
    static hdHeartSea_c* construct(hdHeartSea_c*);
    static void destroy(hdHeartSea_c*, u32 flags);
    void configure();
    u32 createNamedChild(hdMapChartDesc_l* desc);
    u32 load(void* context, void* resource, void* option);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* desc);
    void finalize();
    void executeState();
};
WWHD_SIZE(hdHeartSea_c, 0x3BC);
