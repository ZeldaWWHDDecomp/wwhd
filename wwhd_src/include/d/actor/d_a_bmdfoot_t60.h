/* d_a_bmdfoot_t60.h (WWHD, true 60 build, 2026-11-19): Kalle Demos, vine foot (d_a_bmdfoot.cpp, bmdfoot_class) on a
 * general step length (dt = 1/N). Decomp build only. Separate
 * file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form; the core d_a_bmd_t60.h has the same form):
 * the frame reads the core (the boss found by search: its ground y, the vine command) and Link (the attack aim,
 * ug_move), the tick30 collision hits, its timers (mTimers[4], the btk timer) and draws its random sway once per frame
 * (the sway / joint springs mJointRot[2..16] with cLib_addCalc2: a per-frame spring, the whole-frame form rule). So daBmdfoot_Execute
 * runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw
 * form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation (the body vine morf: base matrix and calc in the execute, nodeCallBack writes only its anim
 * matrices; the floor vine morf: blended between the frame ends, a picture step's draw kept and put back).
 * settingTevStruct (tevStr, mTevstr) in daBmdfoot_Draw on the tick30 step's draw only (rule 14).
 * The create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bmdfoot_class;
namespace bmdfoot_t60 {
static inline const t60w::Ops<bmdfoot_class>& ops() { return t60objw::Logic<bmdfoot_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bmdfoot_t60
