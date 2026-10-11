/* d_item (GameCube names). */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_itembase.h" /* isBomb */

enum dItemNo_e {
    dItemNo_HEART_e = 0x00,
    dItemNo_BOMB_10_e = 0x0C,
    dItemNo_BOMB_30_e = 0x0E,
    dItemNo_ARROW_10_e = 0x10,
    dItemNo_ARROW_30_e = 0x12,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_RED_POTION_e = 0x51,
    dItemNo_BLUE_POTION_e = 0x53,
    dItemNo_BIRD_BAIT_5_e = 0x82,
    dItemNo_HYOI_PEAR_e = 0x83,
    dItemNo_NONE_e = 0xFF,
};
enum dInvSlot_e { dInvSlot_BOW_e = 0x0C };

inline BOOL isArrow(u8 itemNo) { return gabi::call<BOOL>(0x02550FDC, itemNo); }
inline BOOL checkItemGet(u8 itemNo, int p) { return gabi::call<BOOL>(0x0254DA50, itemNo, p); }
inline void execItemGet(u8 itemNo) { gabi::call(0x0254DA38, itemNo); }
