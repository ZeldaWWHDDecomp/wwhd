/* t60_basehold.h (WWHD, true 60 build, 2026-11-24): the drawn placement of an
 * actor whose skeleton calc runs on a held base matrix. Decomp build only.
 * Separate file: actor sources keep thin calls.
 *
 * the process-order read rule base-frame form (first user: d_a_mo2_exec.cpp mo2_t60_base_frame): the calc runs before the
 * move and the base-matrix set, and at N steps every step's calc runs on the frame-start base matrix (frame f - 1's end),
 * as A's calc of frame f; the joint-callback words (hands, spear points, eyePos) keep that logic. The skeleton the calc
 * leaves is placed at frame f - 1's end on both steps, so the body was drawn at 30 Hz, one frame behind (stair sweep,
 * 3.0 units every other pass). The draw side (port t60_draw.cpp, wwhd_t60_draw_basehold) moves the models the
 * actor draws on the step rigidly from the held base to the base matrix the step set (B_step * inv(B_held)); logic and
 * guest words untouched. A follower placed from the holder's hand words (the Moblin's lantern, Kantera state 1) takes the
 * holder's correction of the same step (wwhd_t60_draw_basefollow, the holder's process id). dt 1: never called. Negative
 * control (port): WWHD_T60_BASEHOLD_DRAW_NEGCTRL=1. */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60_draw_basehold(uint32_t actor, uint32_t model, const float* held);
__attribute__((visibility("default"))) void wwhd_t60_draw_basefollow(uint32_t actor, uint32_t holderId);
}
/* the step's calc runs on `held` (3x4, host floats) for `model` of `actor` (call it with the matrix in place) */
static inline void t60bh_hold(uint32_t actor, uint32_t model, const float* held) { wwhd_t60_draw_basehold(actor, model, held); }
/* `actor` is placed from the hand words of the process with id `holderId` on this step */
static inline void t60bh_follow(uint32_t actor, uint32_t holderId) { wwhd_t60_draw_basefollow(actor, holderId); }
#else
static inline void t60bh_hold(uint32_t, uint32_t, const float*) {}
static inline void t60bh_follow(uint32_t, uint32_t) {}
#endif
