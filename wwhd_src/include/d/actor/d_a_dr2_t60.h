/* d_a_dr2_t60.h (WWHD, true 60 build, 2026-11-19): Valoo's tail in the Gohma fight, the lava
 * pit and the ceiling rock (d_a_dr2.cpp, dr2_class) on a general step length (dt = 1/N). Decomp build
 * only. Separate file: the actor's sources keep thin
 * calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form, as Gohma's btd and bita): the frame reads Gohma (s_a_d_sub: btd's state
 * for the rock drop and the floor break) and Link, counts mFrameCounter and the timers mTimers[5], shakes the tail joints
 * and the rock (cM_rndFX in iwa_move), drops the ceiling rock and breaks the floor, and moves its background (mpBgW2
 * Move). So daDr2_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state
 * (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation (the tail skeleton, rock, magma and floor models drawn by this process: blended between the frame
 * ends).
 * daDr2_Draw: settingTevStruct (body and dr_draw's Valoo) and dr_draw's mpMorf2 play (Valoo's animation frame) on the
 * tick30 step's draw only (rule 14): Valoo's animation advances once per 30 Hz frame as in the original.
 * daDr2_Create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct dr2_class;
namespace dr2_t60 {
static inline const t60w::Ops<dr2_class>& ops() { return t60objw::Logic<dr2_class>::kOps; }
/* the draw's logic-side stores (light state, Valoo's play): the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace dr2_t60
