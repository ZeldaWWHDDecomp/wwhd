/* HD: the cannonball icon "Cannonball_00" (one "All" animation group, "G_All_00"), used by the cannon game
 * and battleship screens (hd_screen_cannonball.cpp). No GameCube counterpart. Layout and group names are
 * HD strings; class and method names are ours.
 *
 * The class derives from the "OneTime" parts class of FW-parts_msg_base (OneTimePart_l, 0x54). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_one_time.h"

struct hdScreenCannonball_c : OneTimePart_l {
    static void destroy(hdScreenCannonball_c* i_this, s32 flags);
    static hdScreenCannonball_c* construct(hdScreenCannonball_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
};
WWHD_SIZE(hdScreenCannonball_c, 0x54);
