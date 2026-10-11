/* Sea tag actor, WWHD layout (zeldaret/tww d_a_seatag.h).
 * HD: fopAc_ac_c grows from 0x290 to 0x3AC; the unused phase follows it. */
#pragma once
#include "gc_api/gc_api.h"

class daSeatag_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10039F6C }; // HD: virtual destructor table.
    daSeatag_c();
    inline cPhs_State create();
    inline BOOL draw();
    inline BOOL execute();

    /* 0x3AC */ request_of_phase_process_class mPhase; // Unused, as on GameCube.
};
WWHD_OFFSET(daSeatag_c, mPhase, 0x3AC);
WWHD_SIZE(daSeatag_c, 0x3B4);
