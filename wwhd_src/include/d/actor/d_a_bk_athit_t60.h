/* d_a_bk_athit_t60.h (WWHD, true 60 build, 2026-11-25): the frame whose first step
 * reads the spear sphere's At hit still registers that frame's sweep on its tick30 step. Decomp build only.
 * Separate file: d_a_bk_fight.cpp keeps thin calls.
 *
 * yari_hit_check (the At grid): the frame's sweep (MoveCAt + Set) is registered on the tick30 step and
 * the hit it makes is read on the next frame's first step, where A's call reads it. A's call registers the frame's
 * sweep first and reads the hit after it (setting mAtHitWait 5), so the frame of the read still sweeps. Per step the
 * first step's read set mAtHitWait and the tick30 step returned at the mAtHitWait test: no sweep that frame. Pair
 * b3F4h10 (bk3, bkte10 = 4), Bokoblin 47C5CE68 frame 74: A's sphere moved to (40.45, 1090.9)
 * with At vec (-0.60, -27.43) and hit Link again in that frame's Move; B kept frame 73's sphere (41.05, 1118.4), vec
 * (75.8, 137.0), and no hit. Form: the first step notes its hit read (host latch, the frame tag); that frame's tick30
 * step, at the mAtHitWait test, registers the sweep as the grid's tick30 step does, then returns. dt = 1: nothing.
 * Negative control WWHD_T60_BKATHIT_NEGCTRL=1: no sweep (the form before). */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60bkah {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKATHIT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
inline u32 key(u32 hitWaitEa) { return hitWaitEa ^ 0xA7000000u; }
/* the first step read the At hit (it set mAtHitWait) */
inline void noteRead(u32 hitWaitEa, u32 frame) {
    if (t60::dt() == 1.0f || !t60::first_step() || t60::tick30())
        return;
    wwhd_t60_latch_set(key(hitWaitEa), frame);
}
/* the tick30 step of that frame: register the frame's sweep */
inline bool sweepDue(u32 hitWaitEa, u32 frame) {
    return t60::dt() != 1.0f && t60::tick30() && !nc() && wwhd_t60_latch_pending(key(hitWaitEa), frame) != 0;
}
} // namespace t60bkah
