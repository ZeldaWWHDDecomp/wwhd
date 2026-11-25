/* d_a_player_cutend_t60.h (WWHD, true 60 build; 2026-11-24). Decomp build only.
 * Separate file (no getenv static in the glued unit).
 *
 * The sword cuts' run-out (d_a_player_main_08.cpp lk_procCutCommon: procCutA 16, procCutF 17 and the other cuts that share
 * it, and lk_procCutExA: procCutExA / Kesa) opens checkNextMode(1) on getFrame past the cut's end frame. At N steps the per-step frame opened it on the step
 * whose play crossed the end frame; the earlier steps of that frame ran the cut body once more (the kick move lk_cutKick,
 * the shape turn) that A's frame does not run: the sweep's route at N 2, frame 212, procCutF -> ATN_ACTOR_WAIT:
 * A's one update switches with Link at (-63.746, 988.875), B's first step still kicked him 2.9 on (speed 0) and turned the
 * shape 17, the second step switched; Link 2.9 -> 5.8 off, a Bokoblin and a stick on other paths from 257. Form (as
 * / d_a_player_anm_t60.h lkT60_endFrame): the frame-end frame on every step of the frame, so the change
 * is taken on the frame's first step and the entered body is held for the rest of the frame .
 * NC WWHD_T60_CUTEND_NEGCTRL=1: the per-step frame, as before. dt 1: lkT60_endFrame is the frame itself. */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_anm_t60.h"
#include <cstdlib>

static inline bool lkT60_cutEndNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CUTEND_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the cut's run-out test frame: the frame-end frame (NC: the per-step frame) */
static inline f32 lkT60_cutEndFrame(J3DFrameCtrl* fc) {
    return lkT60_cutEndNegctrl() ? (f32)fc->getFrame() : lkT60_endFrame(fc);
}
