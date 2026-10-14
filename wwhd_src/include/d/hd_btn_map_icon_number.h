/* HD map-icon collected-map count. */
#pragma once
#include "d/hd_btn_map_icon_extra.h"
#include "d/hd_no_decide_button.h"
struct hdMapIconNamePointer_c { gptr<void> mpTarget; void* get(); };
struct hdBtnMapIconNumber_c : NoDecideButton_l {
    be<s32> mPaneCount;
    gptr<gptr<HdMapIconPane>> mpNumberPanes;
    be<u32> mLastCount;
    static void destroy(hdBtnMapIconNumber_c*,u32);
    static hdBtnMapIconNumber_c* construct(hdBtnMapIconNumber_c*);
    void updateCount();
    u32 initialize(u32,u32,u32); void finalize();
    static void initializeStatics();
};
WWHD_OFFSET(hdBtnMapIconNumber_c,mPaneCount,0x78);
WWHD_OFFSET(hdBtnMapIconNumber_c,mpNumberPanes,0x7C);
WWHD_OFFSET(hdBtnMapIconNumber_c,mLastCount,0x80);
WWHD_SIZE(hdBtnMapIconNumber_c,0x84);
