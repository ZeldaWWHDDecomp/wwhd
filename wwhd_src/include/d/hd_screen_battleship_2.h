/* HD: the ship icon of the battleship minigame (Battleship_00) "Battleship_00" (one "All" animation group, "G_All_00"), a part of the battleship minigame
 * screen (hd_screen_battleship_2.cpp). No GameCube counterpart. Layout and group names are
 * HD strings; class and method names are ours.
 *
 * The class derives from the "OneTime" parts class of FW-parts_msg_base (OneTimePart_l, 0x54). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_one_time.h"

struct hdScreenBattleshipIcon_c : OneTimePart_l {
    static void destroy(hdScreenBattleshipIcon_c* i_this, s32 flags);
    static hdScreenBattleshipIcon_c* construct(hdScreenBattleshipIcon_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
};
WWHD_SIZE(hdScreenBattleshipIcon_c, 0x54);
