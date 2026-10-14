/* HD full-screen animated fade wipe. */
#pragma once
#include "d/hd_parts.h"
struct hdFadeWipe_c : hdParts_c {
    static hdFadeWipe_c* construct(hdFadeWipe_c* object);
    static void destroy(hdFadeWipe_c* object,u32 flags);
    void inEnter(); void inExecute(); void outEnter(); void outExecute();
    u32 setup(u32 first,u32 second,void* desc);
    void release(); void execute(); void requestOut(); u32 isBlank(); void requestIn(); u32 isDisplay();
    u32 load(void* name,u32 first,u32 second,u32 third,void* desc);
};
WWHD_SIZE(hdFadeWipe_c,0x50);
