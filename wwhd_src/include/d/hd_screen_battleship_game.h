/* HD: the battleship (squid hunt) minigame screen "BattleshipGame_00": a grid of cannonball and squid
 * icons, the number of cannonballs fired and the best score (hd_screen_battleship_game.cpp; the hit
 * counter and the miss/hit marks are hd_screen_battleship.cpp). No GameCube counterpart. Layout, pane and
 * state names are HD strings; class, member and method names are ours.
 *
 * The class, hdScreenBattleship_c, is declared in d/hd_minigame_hud.h; its "OneTime" child parts are
 * OneTimePart_l (FW-parts_msg_base). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_minigame_hud.h"
#include "d/hd_screen_cursor.h"
#include "d/d_com_inf_game.h"
#include "d/hd_screen_cannonball.h"
#include "d/hd_screen_squid_icon.h"

/* a "OneTime" child part (FW-parts_msg_base) */
inline OneTimePart_l* hdOneTimeChild(hdParts_c* part) { return (OneTimePart_l*)part; }

enum hdBattleshipGame_e {
    BATTLESHIP_CANNONBALL_COUNT = 24, /* children 0..23: cannonball icons in the grid */
    BATTLESHIP_PANE_COUNT = 33,       /* L_Cannonball_* (30) and L_Squid_00..02 */
    BATTLESHIP_SQUID_FIRST = 30,      /* children 30..32: the three squids */
    BATTLESHIP_SQUID_COUNT = 3,
    BATTLESHIP_BEST_SCORE_REG = 0xBEFF, /* the event register holding the best score, probably */
};

