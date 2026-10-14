/* HD iris fade controlled by a fractional animation frame. */
#pragma once
#include "d/hd_parts.h"
struct hdFadeWipeIris_c : hdParts_c {
    be<f32> mFrameCount, mFraction;
    static hdFadeWipeIris_c* construct(hdFadeWipeIris_c* object);
    static void destroy(hdFadeWipeIris_c* object,u32 flags);
    void fadeEnter(); void fadeExecute(); void inEnter(); void inExecute(); void outEnter(); void outExecute();
    u32 setup(u32 first,u32 second,void* desc);
    void release(); void execute(); void draw(void* info); void stop(); void setAmount(f32 amount);
    u32 load(void* name,u32 first,u32 second,u32 third,void* desc);
};
WWHD_SIZE(hdFadeWipeIris_c,0x58);
