/* d_a_bdkobj_t60.h (WWHD, true 60 build, 2026-11-19): Helmaroc King fight objects
 * (d_a_bdkobj.cpp, bdkobj_class: tower blocks / poles type 0 / 1, the collapsing stairs type 2) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (t60objw, the Magtail's / bdk's form): the frame decides from the tick30 collision hits (the
 * blocks and poles break on an attack of type 0xB: the Helmaroc King), from Link riding the stairs (ride_call_back sets
 * health 10; the execute counts it down and the stairs fall at the param's step), moves its background (dBgW_Move) and
 * steps its fragments (gravity, ground / line checks, frame counter) once per frame. So daBdkobj_Execute runs whole once
 * per 30 Hz frame on the tick30 step at dt 1 from the frame-start state; the earlier steps change no logic state. The
 * picture: frame interpolation (the model and fragment models are drawn by this process, blended between the frame ends; the
 * picture-step draw kept and put back). ride_call_back is called by the rider's ground check on each of the rider's
 * steps: only the tick30 step's call counts (t60objw::cbSkip, the rule's BG callback form), as A has one call a frame.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bdkobj_class;
namespace bdkobj_t60 {
static inline const t60w::Ops<bdkobj_class>& ops() { return t60objw::Logic<bdkobj_class>::kOps; }
/* the ride callback: only the tick30 step's call counts */
static inline bool cbSkip() { return t60objw::cbSkip(); }
}  // namespace bdkobj_t60
