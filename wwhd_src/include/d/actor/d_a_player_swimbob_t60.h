/* d_a_player_swimbob_t60.h (WWHD, true 60 build; 2026-11-23). Decomp build only,
 *. Separate file: d_a_player_main_07.cpp keeps a thin call.
 *
 * procSwimWait 0242F590 bobs Link on the water: once a 30 Hz frame the phase mProcVar2 advances by
 * ftoi(2330 x (0.85 + cM_rndF(0.3))) (one random draw a frame) and the model height offset m35C4 = 5 sin(phase) + 1
 * (setWorldMatrix draws him at pos.y + m35C4: a picture value; changeSwimOutProc reads m35C4 only in SWIM_MOVE, and
 * procSwimMove_init / procSwimUp_init set their own). Under the phase and its draw stay on the frame's
 * tick30 step (lk07_perFrame: the random draw once a frame, A's order) and m35C4 is rebuilt every step from the phase
 * held since the last frame: the frame's earlier steps draw the previous frame's bob and the tick30 step the whole
 * frame's change (up to ~1.2 units a frame, a 30 Hz stair on every bob).
 * Form (a picture phase; the frame-end logic unchanged): on the frame's step k of N, k < N, the drawn phase is the held
 * phase + k / N of the mean phase step (2330 x (0.85 + 0.15) = 2330: the frame's own draw is not known before its tick30
 * step, and drawing it earlier would move the random sequence); mProcVar2 itself is not touched. The tick30 step
 * (k = N) and dt 1 (N = 1) take the phase as it is, so the frame-end m35C4 is A's. Negative control
 * WWHD_T60_SWIMBOB_NEGCTRL=1 (game build): the held phase on the earlier steps (the 07#3 form). Host data only. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool lkT60_swimBobNegCtrl() {
    static const bool nc = [] {
        const char *e = getenv("WWHD_T60_SWIMBOB_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

/* the drawn bob phase of the step: phase = mProcVar2 after the step's (tick30-only) advance */
static inline s16 lkT60_swimBobPhase(s16 phase) {
    if (t60::dt() == 1.0f || t60::steps() <= 1 || t60::tick30() || lkT60_swimBobNegCtrl())
        return phase;
    const int32_t k = (int32_t)t60::step();
    const int32_t n = (int32_t)t60::steps();
    if (k < 1 || k >= n)
        return phase;
    return (s16)(uint16_t)((uint16_t)phase + (uint16_t)(2330 * k / n));
}
