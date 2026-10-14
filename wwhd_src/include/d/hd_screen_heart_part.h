/* HD Heart_00 screen part. The canonical Heart part supplies its 0x60 layout. */
#pragma once
#include "d/hd_parts_heart.h"
struct hdHudHeartPart_c : hdHeartPart_c {
    static void destroy(hdHudHeartPart_c*, u32);
    static hdHudHeartPart_c* construct(hdHudHeartPart_c*);
    BOOL loadScreen(void*, void*, void*);
};
WWHD_SIZE(hdHudHeartPart_c, 0x60);
