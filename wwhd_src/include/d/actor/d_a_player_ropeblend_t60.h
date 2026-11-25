/* d_a_player_ropeblend_t60.h (WWHD, true 60 build; 2026-11-25). Decomp game build
 * only. Header only (no new functions or function-local statics in the glued
 * unit d_a_player_main_05.cpp).
 *
 * daPy_lk_c::setBlendRopeMoveAnime (ROPE_SWING, every update) reads the move blend's ratio out of the under blend calc
 * into a local, chases it (cLib_chaseF / cLib_addCalc toward the swing's target) and sets the move animation with it:
 * the blend ratio drives frame control 0 / 1's play rate (setMoveAnime: rate = f28 + ratio x (...)). A chases once a
 * frame; at N steps every step chased the local by its dt share, and the converted eases' frame entry (keyed by the
 * eased address) never saw the frame through, the local being new each call: the frame's ratio came out short
 * (the rope route at N 2, ROPE_SWING from 3688: frame control 0's rate A 2.6500 / B 2.6409, the swing pose off ~0.19
 * through the hold, Link's head / hands / hookshot root positions with it, about 80 frames). Form (the first-step input rule hold):
 * the frame's first step runs the chase at dt 1 (A's one chase on the frame-start ratio), the frame's later steps keep
 * the ratio (no chase; the set re-applies it). N 1 / dt 1: the original call. NC: WWHD_T60_ROPEBLEND_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60RopeBlend {
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_ROPEBLEND_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
/* the chase runs on this step (dt 1, the frame's first step, or the NC) */
static inline bool run() { return t60::dt() == 1.0f || s_nc || t60::first_step(); }
/* before / after the chase on a running step: the whole frame's chase at dt 1 */
static inline float open() { return (t60::dt() != 1.0f && !s_nc) ? t60_set_dt(1.0f) : 0.0f; }
static inline void close(float prev) {
    if (prev != 0.0f)
        t60_set_dt(prev);
}
/* part 2: daPy_lk_c::checkRopeSwingTurn (HD, ROPE_SWING's turn test) simulates the swing: it eases
 * shape_angle.x with cLib_addCalcAngleS, then puts the angle back. The simulated angle is not read. At N steps the eased
 * address' frame entry (c_lib.cpp T60IntEntry, keyed by the address) saw the restore as a foreign write: the real ease
 * after it restarted the frame as partial, so the tick30 step did not get the frame's whole step (the rope route at N 2
 * part 1, from 3688: shape_angle.x A 7424 / B 7491, then up to 509 apart; Link's swing pose with it). Form: at N > 1 the
 * simulated ease runs on a copy, so the real ease's entry sees no foreign write. NC: WWHD_T60_ROPESIM_NEGCTRL=1. */
static const bool s_simNc = [] {
    const char* e = getenv("WWHD_T60_ROPESIM_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
static inline bool simOnCopy() { return t60::dt() != 1.0f && !s_simNc; }
} // namespace lkT60RopeBlend
