/* d_a_dr_t60.h (WWHD, true 60 build, 2026-11-19): Valoo on the Dragon Roost summit
 * (d_a_dr.cpp, dr_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail's / Big Octo's whole form: the frame
 * decides from state outside the actor (dComIfGp_getVibration_CheckQuake, mBikuRequest written by other actors, the
 * DRC boss-cleared bit), its own frame timers (mCountDownTimers[0..2], decremented once per frame), the morf's frame
 * checks (== 15, == 34, isStop) and cM_rndF draws (the wait time, ABARE1 / ABARE2). So daDr_Execute runs whole once
 * per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every decision,
 * timer, anim frame and random draw once per frame. The earlier steps change no logic state. The picture: frame interpolation (the
 * morf is played and calculated in daDr_Execute; it is a model drawn by this process, so the port blends its joint
 * matrices between the frame ends; a picture step's draw is kept and put back). The draw writes no logic state: the
 * light state (settingTevStruct into tevStr) is made on the tick30 step's draw only (rule 14). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs daDr_Execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct dr_class;
namespace dr_t60 {
static inline const t60w::Ops<dr_class>& ops() { return t60objw::Logic<dr_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace dr_t60
