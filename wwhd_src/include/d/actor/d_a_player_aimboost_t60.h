/* d_a_player_aimboost_t60.h (WWHD, true 60 build;-FRAME, 2026-11-24): HD's aim
 * speed boost (the rope / boomerang / bow / hookshot subject procs scale mNormalSpeed by 1.2 after the speed chase) across
 * a 30 Hz frame boundary. Decomp build only. Separate file:
 * the source keeps thin calls.
 *
 * (d_a_player_main_07.cpp lk07_boostUnapply) takes the last step's boost back at the start of every step,
 * so a per-step boost does not compound (prope2: 14.4 -> 1e32). The original's next frame starts its chase from the
 * BOOSTED speed (only at the 12 cap does the deceleration's minimum step snap it back): accelerating, the frame after a
 * boost chases from 1.2 x. Since the speed chase is the frame's once, on the first step (setNormalSpeedF,
 * lkT60_chaseHeld), a boost can compound only within a frame. Form: the take-back stays for
 * the frame's later steps; the frame's first step keeps the boosted speed, as A's frame starts. Hookshot aim route hkD on
 * intgc2 (frame 182, the strafe start): A 5.795 -> 12.811, B 5.795 -> 11.652 (the chase from 4.829 = 5.795 / 1.2), B
 * 1.6 units behind from there (hkAg2: link-motion 8.5 @228, camera 8.7). dt 1: nothing. Negative control
 * WWHD_T60_AIMBOOST_FRAME_NEGCTRL=1: the take-back on every step (the form before). */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60AimBoost {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_AIMBOOST_FRAME_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* true: this step keeps the last step's boosted speed (the frame's first step) */
inline bool keepBoost() { return !nc() && t60::first_step(); }
}  // namespace lkT60AimBoost
