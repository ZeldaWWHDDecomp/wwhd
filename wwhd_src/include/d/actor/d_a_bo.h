#pragma once
#include "bindings.h"
namespace daBo {
struct Actor : fopAc_ac_c {
    u8 enemyState[0x3c8 - 0x3ac];
    u8 resourcePhase[8];
    u8 reserved3D0[4];
    gptr<mDoExt_McaMorf_c> upperMorph;
    gptr<mDoExt_McaMorf_c> lowerMorph;
    u8 reserved3DC[4];
    be<u8> action;
    be<u8> mode;
    u8 runtime[0x1104 - 0x3e2];
};
WWHD_OFFSET(Actor, upperMorph, 0x3d4);
WWHD_OFFSET(Actor, action, 0x3e0);
WWHD_SIZE(Actor, 0x1104);
} // namespace daBo
