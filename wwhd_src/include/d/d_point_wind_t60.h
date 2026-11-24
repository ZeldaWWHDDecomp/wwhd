/* d_point_wind_t60.h (WWHD, true 60 build; 2026-11-24): the point wind's
 * chase on the frame grid. Decomp game build only. Header only.
 *
 * dPointWind_c::set_pwind_move chases the wind's position along its capsule (start -> end, cLib_addCalc ratio 0.1, max
 * step the radius, min step half of it) and restarts it at the start near the end (the restart once a frame on the tick30
 * step). Its converted caller (wind_tag 0x187: the Wind Temple updrafts) runs it on every step at the
 * step's dt; the ratio part of the chase (0.1 x the distance) is not linear in dt (two half steps 0.0975 of it, one frame
 * 0.1), so B's position ran up to 3.5 units ahead of A's: at A's 1060.7 (no restart) B's 1064.2 restarted, its cycle 8
 * frames against A's 9, the updraft bubbles drifted a frame a cycle (a glide pair: the bubble 537 units behind A's at the same
 * Link position, the glide's wind push a frame late). Frame-exact form: the frame's first step runs the chase for the
 * whole frame (dt 1), the later steps hold it; the restart test stays on the tick30 step. dt 1: the original.
 * NC: WWHD_T60_PWIND_CHASE_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60pw {
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_PWIND_CHASE_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
/* the chase runs whole once a frame (on the frame's first step) */
static inline bool whole() { return t60::dt() != 1.0f && !s_nc; }
} // namespace t60pw
