/* Spot-light box, WWHD layout: actor base +0x11C moves mMtx from GameCube 0x290. */
#pragma once
#include "gc_api/gc_api.h"

class daSpotbox_c : public fopAc_ac_c {
public:
    daSpotbox_c();
    inline cPhs_State create();
    inline BOOL execute();
    enum : u32 { __vt = 0x1003D0EC }; // HD: virtual destructor
    u32 getType() { return fopAcM_GetParam(this) & 1; }

    /* 0x3AC */ Mtx34 mMtx;
};
WWHD_OFFSET(daSpotbox_c, mMtx, 0x3AC);
WWHD_SIZE(daSpotbox_c, 0x3DC);
