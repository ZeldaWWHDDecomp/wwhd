/* d_a_player_lavarestart_t60.h (WWHD, true 60 build, 2026-11-23): Link's
 * lava bounce restart test (daPy_lk_c::dProcLavaDamage 02421E80, d_a_player_main_05.cpp) on a general step length. Decomp
 * build only. Separate header: the negative control's static lives here, not
 * as a function-local static in the glued unit (gen.py autoglue).
 *
 * the process-order read rule (a frame-grid threshold): the test speed.y < -gravity is A's one test of the frame, from the frame-start
 * speed.y (the procedure runs before the move). Per step it crossed within the frame: speed.y 80 - 2.5 per frame, A tests
 * 2.5 (no) then 0 (restart); at N 2 the frame's second step started at 1.25 < 2.5 and requested the restart a frame
 * early (pot pairs: the overlap request B f325 / A f326, fade-out, load and the new stage one frame
 * earlier in B). So at dt != 1 it tests on the frame's first step only. dt 1: the original test. Negative control
 * WWHD_T60_LAVARESTART_NEGCTRL=1 (every step, as before). */
#pragma once
#include "d/actor/d_a_player_t60.h"
#include <cstdlib>

static inline bool lkT60_lavaRestartNc() {
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_LAVARESTART_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}
/* true on the step that runs the test */
static inline bool lkT60_lavaRestartTest() { return lkT60_countStep() || lkT60_lavaRestartNc(); }
