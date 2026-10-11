/* Dummy actor: GameCube layout moves by +0x11C for the WWHD actor base.
 * HD: virtual destructor table lives in the actor base; size 0x3E4. */
#pragma once
#include "gc_api/gc_api.h"

namespace daDummy {
class Act_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1000E228 };
    Act_c();
    bool create_heap() { return true; }
    cPhs_State _create();
    bool _delete() { return true; }
    void set_mtx();
    bool _execute() { return true; }
    bool _draw() { return true; }

    /* 0x3AC */ request_of_phase_process_class mPhase; // Unused
    /* 0x3B4 */ Mtx34 mMtx;
};
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mMtx, 0x3B4);
WWHD_SIZE(Act_c, 0x3E4);
} // namespace daDummy
