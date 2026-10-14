/** Telescope screen loading and cover coordination. */
#pragma once
#include "d/hd_screen_telescope_hud.h"
struct hdTelescopeCover_c {
    static void initializeStatics();
    static BOOL create(hdTelescopeHud_c*,u32,u32,u32);
    static void destroyLayout(hdTelescopeHud_c*);
    static void calc(hdTelescopeHud_c*);
    static BOOL loadLayout(hdTelescopeHud_c*,void*,u32,u32,u32,void*);
};
