/* HD-only item-screen Decide part; all class/member names are descriptive. */
#pragma once
#include "d/hd_screen_menu_item.h"
struct hdItemDecideContext_c {
    gptr<hdAnimSet> mpAnimation;
    be<u32> mReserved;
    gptr<hdItemPaneArray_c> mpPaneGroups[3];
    be<u32> mOption;
};
WWHD_OFFSET(hdItemDecideContext_c, mReserved, 4);
WWHD_OFFSET(hdItemDecideContext_c, mpPaneGroups, 8);
WWHD_OFFSET(hdItemDecideContext_c, mOption, 0x14);
WWHD_SIZE(hdItemDecideContext_c, 0x18);
struct hdItemDecide_c : hdMenuItem_c {
    static hdItemDecide_c* construct(hdItemDecide_c* self);
    static void destroy(hdItemDecide_c* self, s32 flags);
    void decideEnter();
    void decideExecute();
    void decideExit();
    u32 load(void* resource, u32 arg1, u32 arg2);
    void execute();
    void update();
    void beginAnimation();
    void endAnimation();
    void getContext(hdItemDecideContext_c* context);
    void setVisible(u32 visible);
    void cachePosition();
    void translatePoint(hdItemPoint_c* point);
};
WWHD_SIZE(hdItemDecide_c, 0x118);
