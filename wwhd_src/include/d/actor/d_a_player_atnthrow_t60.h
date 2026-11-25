/* d_a_player_atnthrow_t60.h (WWHD, true 60 build; 2026-11-25). Decomp game build only,
 *. Header only (no new functions or function-local statics in the glued unit
 * d_a_player_main_01.cpp).
 *
 * daPy_lk_c::setAtnList (execute, before the proc) makes the thrown boomerang Link's lock-on actor while it flies
 * (mActorKeepThrow set and daPyStts0_BOOMERANG_WAIT_e): both are set by throwBoomerang inside the proc. In A the throw
 * frame's setAtnList ran before the throw, so the lock-on (and what reads it: setNeckAngle's look target, the hat spring
 * through the head) starts on the next frame. At N steps the throw is made on an earlier step and the frame's later
 * steps' setAtnList saw it: the lock-on a frame early. bmA (boomerang) at N 2 on intgc6: mpAttnActorLockOn set
 * at B's record 3635 vs A's 3636; mNeckAngle (681, 3066) vs A (0, 0) at 3635, the hat words (mHatSwingPhase ...) off
 * from 3636. Form: the frame's first step records the throw-lock condition as setAtnList reads it (before its proc: A's
 * frame-start state); the frame's later steps use that record. (A recorded throw whose actor is gone by a later step
 * falls back to the live test.) N 1: the live test (the original). NC: WWHD_T60_ATNTHROW_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60AtnThrow {
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_ATNTHROW_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
static bool s_valid = false;
static bool s_lock = false;
static uint32_t s_stamp = 0;
/* setAtnList's throw-lock test: live = the original's condition; returns the condition to use */
static inline bool lock(bool live, bool actorLive) {
    if (s_nc || t60::steps() <= 1)
        return live;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s_lock = live;
        s_stamp = now;
        s_valid = true;
        return live;
    }
    if (!s_valid || now - s_stamp >= (uint32_t)t60::steps())
        return live;
    return s_lock && actorLive;
}
} // namespace lkT60AtnThrow
