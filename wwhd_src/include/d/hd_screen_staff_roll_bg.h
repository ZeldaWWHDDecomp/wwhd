/* HD: the staff roll background "StaffRollBG_00": one looping animation ("All", group "G_All_00")
 * behind the staff roll (hd_screen_staff_roll_bg.cpp). No GameCube counterpart. Layout, pane,
 * animation and state names are HD strings; class, member and method names are ours. Local
 * */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_screen_staff_roll.h"

/* 02004E04: bind animation `entry` to its pane group (hd_ui_02004A3C) */
void hdStaffRollBG_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group);
/* 028713B8: an animation's frame count (nw::lyt animator) */
u32 hdStaffRollBG_getAnimFrameCount(hdAnimSetController* controller);

enum hdStaffRollBGAnim_e {
    STAFF_ROLL_BG_ANIM_ALL = 0,
};

struct hdScreenStaffRollBG_c : hdParts_c {
    static void destroy(hdScreenStaffRollBG_c* i_this, s32 flags);
    void allEnter();
    void allExecute();
    static hdScreenStaffRollBG_c* construct(hdScreenStaffRollBG_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    s32 getAllFrameCount();
};
WWHD_SIZE(hdScreenStaffRollBG_c, 0x50);
