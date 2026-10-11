// Windfall children modes; retain the shared people actor's GameCube member names.
// HD: integer vector view preserves GHS word copies without converting NaNs.
#pragma once
#include "d/actor/d_a_npc_people.h"
namespace peopleUgHD {
enum Mode {
    MODE_WAIT = 0,
    MODE_TURN = 3,
    MODE_KYORO = 6,
    MODE_LETTER = 7,
    MODE_LOOK = 8,
    MODE_LOOK2 = 9,
    MODE_UG_WALK = 10,
    MODE_UG_TURN = 11,
    MODE_UG_LOOK = 12,
    MODE_UG_LOOK2 = 13,
    MODE_UG_SIT = 14,
};
struct VectorWords {
    /* 0x00 */ be<u32> x;
    /* 0x04 */ be<u32> y;
    /* 0x08 */ be<u32> z;
};
WWHD_OFFSET(VectorWords, x, 0);
WWHD_OFFSET(VectorWords, y, 4);
WWHD_OFFSET(VectorWords, z, 8);
WWHD_SIZE(VectorWords, 12);
}
