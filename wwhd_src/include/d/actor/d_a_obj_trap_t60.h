/* d_a_obj_trap_t60.h (WWHD, true 60 build, 2026-11-19): the blade trap (Obj_Trap, d_a_obj_trap.cpp, daObjTrap_c) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum, the Magtail / Floormaster whole form: the blade decides from Link's position (its target line),
 * its wall / block checks and the tick30 collision, and its chase move and BG move run once per 30 Hz frame.
 * So the execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h,
 * t60objw form); the earlier steps change no logic state. The picture: frame interpolation. The draw makes the light state on the
 * tick30 step's draw only (rule 14); the rest of the draw writes no logic state. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute
 * on every step at dt. */
#pragma once
#include "d/t60_objw.h"

class daObjTrap_c;
namespace objtrap_t60 {
static inline const t60w::Ops<daObjTrap_c>& ops() { return t60objw::Logic<daObjTrap_c>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace objtrap_t60
