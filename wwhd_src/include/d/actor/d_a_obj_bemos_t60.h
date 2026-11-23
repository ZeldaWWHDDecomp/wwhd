/* d_a_obj_bemos_t60.h (WWHD, true 60 build, 2026-11-19): the Beamos (obj_bemos,
 * d_a_obj_bemos.cpp, daBemos_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum, the Magtail / Floormaster whole form: its eye procedures decide from Link (the range checks,
 * the beam lock-on, charge and search), the tick30 collision hits (eye break, the guard cylinder), the event system,
 * and their timers and the brk / btk frames advance once per 30 Hz frame. So daBemos_c::_execute runs whole once per
 * 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form); the earlier steps
 * change no logic state (one AI state step per 30 Hz frame, the check item, holds by construction). The
 * picture: frame interpolation. The draw makes the light state on the tick30 step's draw only (rule 14); the rest of the draw
 * writes no logic state. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every
 * step at dt. */
#pragma once
#include "d/t60_objw.h"

class daBemos_c;
namespace bemos_t60 {
static inline const t60w::Ops<daBemos_c>& ops() { return t60objw::Logic<daBemos_c>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bemos_t60
