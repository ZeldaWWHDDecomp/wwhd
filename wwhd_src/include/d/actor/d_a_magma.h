/* daMagma_c (magma floor), WWHD layout.
 * GameCube -> WWHD: actor base grows from 0x290 to 0x3AC, moving mPhs by +0x11C.
 * HD adds a virtual destructor through the base's vtable. */
#pragma once
#include "gc_api/gc_api.h"

class daMagma_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1001437C }; /* HD: virtual destructor table */
    daMagma_c();
    inline cPhs_State create();
    s16 getPathNo() { return fopAcM_GetParam(this); }

public:
    /* 0x3AC */ request_of_phase_process_class mPhs;
};
WWHD_OFFSET(daMagma_c, mPhs, 0x3AC);
WWHD_SIZE(daMagma_c, 0x3B4);
