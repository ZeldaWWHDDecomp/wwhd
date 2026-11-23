/* f_op_actor_t60.h (WWHD, true 60 build; 2026-11-17): fopAc_Draw's culling answer on a general step
 * length (dt = 1/N, the first-step AI-timer rule). Decomp build only.
 *
 * fopAc_Draw leaves its culling answer in actor_condition (fopAcCnd_NODRAW_e). The next execute reads it: fopAc_Execute
 * skips an actor with fopAcStts_NOCULLEXEC_e while it is set, and a few actors read it themselves (d_a_obj_mkie). That
 * makes it a 30 Hz frame decision: in the original, the draw after frame f decides frame f + 1's execute.
 *
 * At N > 1 the game draws after every step. The draw after an in-between step saw the camera half a frame on, and its
 * answer decided the frame's later steps: an actor came into view (and started executing) half a frame early or late
 * (lpalm 0x49 in the Link pair: condition 8/14 at frame 65 and 151, the wind quaternions half a frame apart
 * from then on).
 *
 * Form: only the draw after the frame's tick30 step writes the answer, so every step of the next frame (and every
 * unconverted process on its tick30 step) sees the answer the original's one execute sees. The draw itself still
 * culls by its own test on every pass. At N = 1 every step is a tick30 step: the original. */
#pragma once
#include "true60.h"

/* true: this draw writes the frame's culling answer (fopAcCnd_NODRAW_e) */
static inline bool fopAc_t60_cull_answer_pass() { return t60::tick30(); }
