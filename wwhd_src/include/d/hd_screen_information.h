/* HD Information_00 HUD screen; no GameCube counterpart. */
#pragma once
#include "d/hd_parts_heart.h"
struct hdHudInformation_c : hdParts_c {
    static void destroy(hdHudInformation_c*,u32);
    static hdHudInformation_c* construct(hdHudInformation_c*);
    void inEnter(); void inMove(); void loopEnter(); void loopMove();
    void outEnter(); void outMove();
    BOOL init(void*,u32,u32);
    void finalize(); void execute();
    BOOL loadLayout(void*,u32,u32,u32,void*);
};
WWHD_SIZE(hdHudInformation_c,0x50);
struct hdInformationStateNumber_c { be<u32> mNumber; u32 getNumber(); };
