/* d_a_esa (bait): esa_class, the part other actors use (HD offsets: GameCube + 0x11C). */
#pragma once
#include "bindings.h"

struct esa_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3B4 - 0x3AC];
    /* 0x3B4 */ be<u8> field_0x298;      /* [v nz, kb] */
    /* 0x3B5 */ u8 _3B5[0x3BD - 0x3B5];
    /* 0x3BD */ be<s8> mState;           /* [v kb] */
    /* 0x3BE */ u8 _3BE[0x3D8 - 0x3BE];
    /* 0x3D8 */ gptr<J3DModel> mpModel;  /* [v nz] */
};
WWHD_OFFSET(esa_class, field_0x298, 0x3B4);
WWHD_OFFSET(esa_class, mpModel, 0x3D8);
