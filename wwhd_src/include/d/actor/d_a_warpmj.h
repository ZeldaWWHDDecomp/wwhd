#pragma once
#include "bindings.h"
// HD actor allocation is 0x5AC. GC provides no recovered layout.
struct daWarpmj_c : fopAc_ac_c {
    request_of_phase_process_class phase; // 3AC
    gptr<J3DModel> model;                // 3B4
    gptr<void> btk, brk, bck;            // 3B8..3C0
    gptr<void> particles[3];            // 3C4..3CC
    be<u32> destination;                // 3D0
    be<s32> order;                      // 3D4
    be<s16> eventIndex;                 // 3D8
    u8 pad3DA[2];
    be<s32> demoState;                  // 3DC
    be<s32> staff;                      // 3E0
    u8 lightingAndPadding[0x1C8];       // 3E4..5AB
};
WWHD_OFFSET(daWarpmj_c, model, 0x3B4);
WWHD_OFFSET(daWarpmj_c, order, 0x3D4);
WWHD_OFFSET(daWarpmj_c, staff, 0x3E0);
WWHD_SIZE(daWarpmj_c, 0x5AC);
