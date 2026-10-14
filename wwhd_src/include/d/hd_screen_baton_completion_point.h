/* HD baton input-point marker. Private binary reconstruction. */
#pragma once
#include "d/hd_parts_one_time.h"

struct hdBatonCompletionPoint_c : OneTimePart_l {
    static void destroy(hdBatonCompletionPoint_c* self, s32 flags);
    static hdBatonCompletionPoint_c* construct(hdBatonCompletionPoint_c* self);
    u32 load(u32 first, u32 second, void* descriptor);
    void finish();
    u32 createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor);
};
WWHD_SIZE(hdBatonCompletionPoint_c, 0x54);
