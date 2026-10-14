/* HD LoopBossIcon_00 timers and screen setup. */
#pragma once
#include "d/hd_parts_no_control.h"
#include "d/hd_parts_heart.h"
#include "SSystem/SComponent/c_math.h"
struct hdHudLoopBoss_c : NoControlPart_l {
    be<u32> mFirstTimer; // 50
    be<u32> mSecondTimer; // 54
    static void destroy(hdHudLoopBoss_c*,u32);
    static hdHudLoopBoss_c* construct(hdHudLoopBoss_c*);
    void resetTimers();
    BOOL init(void*,u32,hdPartsDesc_c*);
    BOOL loadLayout(void*,u32,u32,u32,void*);
};
WWHD_SIZE(hdHudLoopBoss_c,0x58);
