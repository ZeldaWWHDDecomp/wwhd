/* Grass placement actor. GameCube fields retain their names; HD adds the actor vtable
 * and expands the base to 0x3AC. */
#pragma once
#include "gc_api/gc_api.h"

struct grass_class : fopAc_ac_c {
    /* 0x3AC */ be<u32> field_0x00;
    /* 0x3B0 */ be<u32> field_0x04;
    grass_class() { fopAc_ac_c_ct(this); __vtbl = 0x10010654; }
};
WWHD_OFFSET(grass_class, field_0x00, 0x3AC);
WWHD_OFFSET(grass_class, field_0x04, 0x3B0);
WWHD_SIZE(grass_class, 0x3B4);

struct OffsetData {
    /* 0x0 */ be<u8> num;
    u8 _1[3];
    /* 0x4 */ gptr<csXyz> pos;
};
WWHD_OFFSET(OffsetData, pos, 4);
WWHD_SIZE(OffsetData, 8);

// HD: typed storage for the function-local csXyz arrays. Their guard is set before construction.
template <int N> struct GrassOffsetSet { csXyz pos[N]; };
using GrassOffsetSet7 = GrassOffsetSet<7>;
using GrassOffsetSet21 = GrassOffsetSet<21>;
using GrassOffsetSet3 = GrassOffsetSet<3>;
using GrassOffsetSet17 = GrassOffsetSet<17>;
using GrassOffsetSet5 = GrassOffsetSet<5>;
