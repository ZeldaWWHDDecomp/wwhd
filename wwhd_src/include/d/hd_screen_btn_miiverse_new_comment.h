/* HD-only Miiverse new-comment button. */
#pragma once
#include "d/hd_parts_basic_button.h"
struct hdBtnMiiverseNewComment_c : hdBasicButton_c {
    static hdBtnMiiverseNewComment_c* construct(hdBtnMiiverseNewComment_c*);
    static void destroy(hdBtnMiiverseNewComment_c*, s32);
    u32 init(void* resource, u32 mode, hdPartsDesc_c* desc);
    void stopLoop();
    void startLoop();
    void pauseLoop();
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* desc);
    static void initializeStatics();
};
WWHD_SIZE(hdBtnMiiverseNewComment_c, 0x8C);

void setupMiiverseAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
