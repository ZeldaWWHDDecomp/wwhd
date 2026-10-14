/* Baton input pane factories; companion to the merged metronome controller. */
#pragma once
#include "d/hd_parts_one_time.h"

struct hdBatonInputPoint_c : OneTimePart_l {
    static hdBatonInputPoint_c* construct(hdBatonInputPoint_c*);
    static void destroy(hdBatonInputPoint_c*, u32 flags);
    u32 initialize(u32 first, u32 second, void* descriptor);
};
WWHD_SIZE(hdBatonInputPoint_c, 0x54);
WWHD_OFFSET(hdBatonInputPoint_c, mTriggered, 0x50);
