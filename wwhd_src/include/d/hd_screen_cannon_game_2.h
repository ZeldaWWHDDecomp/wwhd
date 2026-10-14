/* HD: the cannon minigame screen "CannonGame_00": the cannon's angle (pane "N_BatteryAngle_00" turned, the
 * degrees in "T_NumDegree_00"), ten cannonball icons for the shots fired and five battleship icons for the
 * ships sunk (hd_screen_cannon_game_2.cpp; the sunk-ship reveal is hd_screen_cannon_game.cpp). No GameCube
 * counterpart. Layout, pane, animation and state names are HD strings; class, member and method names are
 * ours.
 *
 * The class, hdScreenCannonGame_c, is declared in d/hd_minigame_hud.h. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_minigame_hud.h"
#include "d/hd_screen_cursor.h"
#include "d/hd_screen_cannonball.h"
#include "d/hd_screen_battleship_2.h"
#include "d/hd_screen_battleship_game.h"

enum hdCannonGame_e {
    CANNON_GAME_SHOT_COUNT = 10,  /* children 0..9: cannonball icons */
    CANNON_GAME_SHIP_FIRST = 10,  /* children 10..14: battleship icons */
    CANNON_GAME_SHIP_COUNT = 5,
    CANNON_GAME_PANE_COUNT = 15,
};
enum hdCannonGameAnim_e {
    CANNON_GAME_ANIM_IN_ALL = 0,
    CANNON_GAME_ANIM_OUT_ALL = 1,
};

