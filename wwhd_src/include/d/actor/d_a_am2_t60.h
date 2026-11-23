/* d_a_am2_t60.h (WWHD, true 60 build, 2026-11-19): Armos (the small Armos, d_a_am2.cpp,
 * am2_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail / Floormaster whole form: the frame decides from Link (the
 * wake-up and chase, the rebound off his shield), the tick30 collision hits (eye / weak spot / body / needle) and the
 * BG check, and its countdown timers, RNG draws, morf / btk / brk play advance once per 30 Hz frame. So daAM2_Execute
 * runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form):
 * every decision with the other actors' current state, the colliders registered once for the tick30 collision Move
 * (one-sweep-per-frame attack), the move once, a delete request at the frame's end (first-step AI-timer addendum 2). The earlier steps change no logic state.
 * The picture: frame interpolation. The body morf is calculated in the execute (draw_SUB: base matrix + mpMorf->calc, the joint
 * callback nodeCallBack writes the eye / needle / weak-spot positions there, on the tick30 step only); daAM2_Draw only
 * enters the morf and the btk / brk frames, so the draw writes no logic state; the port blends the skeleton between
 * the frame ends. daAM2_Create calls draw_SUB once (the first pose): the original at dt 1 (createBegin / createEnd). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct am2_class;
namespace am2_t60 {
static inline const t60w::Ops<am2_class>& ops() { return t60objw::Logic<am2_class>::kOps; }
/* daAM2_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace am2_t60
