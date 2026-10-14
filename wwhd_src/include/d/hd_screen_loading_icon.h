/* HD LoadingIcon_00; canonical InOutLoop part layout. */
#pragma once
#include "d/hd_parts_in_out_loop.h"
#include "d/hd_parts_heart.h"
struct hdHudLoadingIcon_c : hdPartsInOutLoop_c {
 static void destroy(hdHudLoadingIcon_c*,u32);
 static hdHudLoadingIcon_c* construct(hdHudLoadingIcon_c*);
 BOOL loadScreen(void*,void*,void*);
};
WWHD_SIZE(hdHudLoadingIcon_c,0x5C);
