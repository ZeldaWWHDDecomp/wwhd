/* d_a_bl_t60.h (WWHD, true 60 build, 2026-11-19): Bubble (d_a_bl.cpp, bl_class) on a general
 * step length (dt = 1/N). Decomp build only. Separate file
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail / Floormaster whole form: the frame decides from Link (the
 * float toward him, the fire / curse touch), the tick30 collision hits (sword, boomerang, arrows, the fire / ice states)
 * and the BG checks, and its timers, the float wave (fuwafuwa_keisan), the fire ring spin (fire_kaiten_keisan), the RNG
 * draws and the morf / btk play advance once per 30 Hz frame. So daBL_Execute runs whole once per 30 Hz frame on the
 * tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every decision with the other actors'
 * current state, the colliders registered once for the tick30 collision Move (one-sweep-per-frame attack), the move once, a delete request at
 * the frame's end (first-step AI-timer addendum 2). The earlier steps change no logic state.
 * The picture: frame interpolation. The morf is calculated in the execute (draw_SUB); daBL_Draw only enters the morf and the btk
 * frame, writing no logic state; the port blends the skeleton between the frame ends. daBL_Create calls draw_SUB once
 * (the first pose): the original at dt 1 (createBegin / createEnd). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bl_class;
namespace bl_t60 {
static inline const t60w::Ops<bl_class>& ops() { return t60objw::Logic<bl_class>::kOps; }
/* daBL_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace bl_t60
