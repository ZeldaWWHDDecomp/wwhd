/* d_a_bgn3_t60.h (WWHD, true 60 build, 2026-11-19): Puppet Ganon, phase 3 (the worm;
 * d_a_bgn3.cpp, bgn3_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's / Helmaroc's / Puppet Ganon phase 1 / 2's form): the
 * frame reads the controller bgn (its form, fpcM_Search), the bait (esa), Link, the tick30 collision hits
 * (damage_check on the head, tail and part spheres) and its timers, and steps the nine-part body chain and the hair
 * (part_control, ke_move) once per frame. So daBgn3_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1
 * from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic state. The picture:
 * frame interpolation. daBgn_Draw draws bgn3's body and its parts (the port's draw history of the bgn process blends
 * them, light states on the tick30 draw); daBgn3_Draw only updates the hair line materials from the frame end.
 * daBgn3_Create calls daBgn3_Execute once (the first frame's pose): that call is the original at dt 1 (create's run,
 * not a frame step). dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bgn3_class;
namespace bgn3_t60 {
static inline const t60w::Ops<bgn3_class>& ops() { return t60objw::Logic<bgn3_class>::kOps; }
/* daBgn3_Create's execute call: the original at dt 1 (t60w::on() is false at dt 1, so the Guard does nothing) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace bgn3_t60
