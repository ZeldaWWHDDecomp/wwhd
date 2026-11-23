/* d_a_bgn2_t60.h (WWHD, true 60 build, 2026-11-19): Puppet Ganon, phase 2 (the spider;
 * d_a_bgn2*.cpp, bgn2_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's / Helmaroc's / Puppet Ganon phase 1's form): the
 * frame reads the controller bgn (its form, fpcM_Search) and bgn3, Link (position, the cut scene placement), the tick30
 * collision hits (damage_check on the head, body, legs and core spheres), the keese count (ki_check) and the shared
 * five-timer set mTimers[] that gates every action (move / plesattack / jumpattack / mahi / damage / hensin); the head,
 * the core and the leg spheres follow the body's animation once per frame. So daBgn2_Execute runs whole once per 30 Hz
 * frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps
 * change no logic state. The picture: frame interpolation. bgn2's own draw is empty: daBgn_Draw draws its body, head and core
 * models, so the port's draw history of the bgn process blends them between the frame ends; their light
 * states are bgn's draw's (tick30 draw only, rule 14).
 * daBgn2_Create calls daBgn2_Execute once (the first frame's pose): that call is the original at dt 1 (create's run,
 * not a frame step). dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bgn2_class;
namespace bgn2_t60 {
static inline const t60w::Ops<bgn2_class>& ops() { return t60objw::Logic<bgn2_class>::kOps; }
/* daBgn2_Create's execute call: the original at dt 1 (t60w::on() is false at dt 1, so the Guard does nothing) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace bgn2_t60
