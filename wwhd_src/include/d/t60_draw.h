/* t60_draw.h (WWHD, true 60 build; 2026-11-17): draw-path code that writes 30 Hz state, on a general step
 * length (dt = 1/N, the first-step AI-timer rule). Decomp build only.
 *
 * At N > 1 the game draws after every step; in the original it draws once per 30 Hz frame, after the frame's logic. Draw
 * code that only reads state draws the same picture on every pass. Draw code that WRITES state is a 30 Hz frame step:
 *   - an answer the next execute reads (a culling answer computed from the camera, f_op_actor_t60.h, d_a_obj_search):
 *     the draw after an in-between step saw the camera half a frame on, and its answer decided the frame's later steps;
 *   - a per-draw counter or smoothing step (a timer counted in the draw, cLib_addCalc / chase of a draw-side value, a
 *     spin angle): it ran N times as fast (a flash ended early, a fade ran N times as fast).
 *
 * Form: the write runs on the draw after the frame's tick30 step only; the draw itself still draws on every pass (from
 * a local copy of an answer where it uses one). At N = 1 every step is a tick30 step: the original. Each site names this
 * header in its comment. */
#pragma once
#include "true60.h"

/* true: this draw pass is the 30 Hz frame's draw (the draw after the frame's tick30 step) and writes its state */
static inline bool t60_draw_frame_pass() { return t60::tick30(); }
