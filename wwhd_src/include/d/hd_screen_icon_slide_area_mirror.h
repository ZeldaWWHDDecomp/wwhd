/* HD item-slot slide-area mirror: shares the source screen's animation state. */
#pragma once
#include "d/hd_screen_icon_slide_area.h"
struct hdIconSlideAreaMirror_c : hdParts_c {
    /* 0x50 */ gptr<hdAnimSet> mpSourceAnimations;
    static hdIconSlideAreaMirror_c* construct(hdIconSlideAreaMirror_c* area);
    static void destroy(hdIconSlideAreaMirror_c* area, s32 flags);
    void enterFloat();
    void executeFloat();
    void enterFloatCancel();
    void executeFloatCancel();
    void enterSwitch();
    void executeSwitch();
    u32 load(u32 first, u32 second, hdPartsDesc_c* desc);
    void syncAnimations();
    void execute();
    void setSourceAnimations(gptr<hdAnimSet>* source);
    u32 createChild(hdPartsDesc_c* desc);
    u32 loadLayout(void* name, u32 a, u32 b, u32 c, void* desc);
};
WWHD_OFFSET(hdIconSlideAreaMirror_c, mpSourceAnimations, 0x50);
WWHD_SIZE(hdIconSlideAreaMirror_c, 0x54);
