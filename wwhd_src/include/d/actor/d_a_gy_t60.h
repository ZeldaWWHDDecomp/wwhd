/* d_a_gy_t60.h (WWHD, true 60 build, 2026-11-19): Gyorg (d_a_gy.cpp, daGy_c) on a general step
 * length (dt = 1/N). Decomp build only. Separate file:
 *the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail / Floormaster whole form: the frame decides from its
 * controller daGyCtrl_c (the parent found by process id: its mode, the spawn positions, the attack turn), from Link and
 * the ship (circling, the ram attack), the tick30 collision hits and the line / water checks, and its timers, RNG draws
 * and both morfs advance once per 30 Hz frame. So daGy_c::_execute runs whole once per 30 Hz frame on the tick30 step at
 * dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every decision with the other actors' current state
 * (the controller is unconverted: it runs on the tick30 step, as for A), the colliders registered once for the tick30
 * collision Move (one-sweep-per-frame attack), the move through fopAcM_posMoveF once, a delete request at the frame's end (first-step AI-timer addendum 2).
 * The earlier steps change no logic state.
 * The picture: frame interpolation. Both morfs are calculated in the execute; _draw only enters them, writing no logic state; the
 * port blends the skeletons between the frame ends. _create's createInit (setMtx, the morf calcs, the first posMoveF)
 * is the original at dt 1 (createBegin / createEnd). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct daGy_c;
namespace gy_t60 {
static inline const t60w::Ops<daGy_c>& ops() { return t60objw::Logic<daGy_c>::kOps; }
/* _create's createInit: the original at dt 1 (create's first pose and move, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace gy_t60
