/* d_a_daiocta_t60.h (WWHD, true 60 build, 2026-11-19): the Big Octo (d_a_daiocta.cpp,
 * daDaiocta_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail's / Floormaster's whole form: the
 * frame decides from other actors' state (Link's distance and angle: the suck-in demo MODE_DEMO; its eyes' state through
 * isLivingEye / isDamageEye / isDamageBombEye; the bomb hits from the tick30 collision, coHit_CB -> MODE_DAMAGE_BOMB),
 * the event system (the DAIOCTA_SUIKOMI demo, the restart / next-stage order at its end) and its own frame timers
 * (mDemoEndTimer, the morf frame checks == 26 / 50 / 65). So _execute runs whole once per 30 Hz frame on the tick30 step
 * at dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every decision with the other actors' current state,
 * the timers and anim frames once per frame, the collider registered once for the tick30 collision Move (one-sweep-per-frame attack). The
 * earlier steps change no logic state. The picture: frame interpolation (the body morf is calculated in _execute, the bubble /
 * suck-in models in the draw through mDoExt_modelUpdateDL from base matrices set in _execute and anim frames advanced in
 * _execute; all of them are models drawn by this process, so the port blends their joint matrices between the frame
 * ends; a picture step's draw is kept and put back). The draws write no logic state: the light state
 * (oct_settingTevStruct into tevStr) is made on the tick30 step's draw only (rule 14). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct daDaiocta_c;
namespace daiocta_t60 {
static inline const t60w::Ops<daDaiocta_c>& ops() { return t60objw::Logic<daDaiocta_c>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace daiocta_t60
