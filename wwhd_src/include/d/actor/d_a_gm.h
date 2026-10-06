#pragma once
#include "f_op/f_op_actor.h"
// HD allocation size and actor prefix are fixed by 02150604.
struct gm_class : fopEn_enemy_c {
    u8 _3C8[0x1290 - 0x3C8];
};
WWHD_SIZE(gm_class, 0x1290);
