/* d_a_gm_t60.h (WWHD, true 60 build; / gm#2, 2026-11-19): the Mothula and its larvae (gm,
 * d_a_gm.cpp, gm_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: the actions decide from
 * Link (attacks, the ground / air chase, the wind), the tick30 collision hits, enemy_ice / enemy_fire and the event
 * system, and the timers, RNG draws, wing emitters, posMove / BG check and the morf advance once per 30 Hz frame. So
 * daGM_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h,
 * t60objw form); the earlier steps change no logic state. The picture: frame interpolation. The execute ends in draw_SUB (matrix,
 * morf calc, enemy_fire, the light state), so the light state is made on the tick30 step only; daGM_Draw writes no
 * logic state. daGM_Create's draw_SUB pose is the original at dt 1 (createBegin / createEnd). Never held. dt 1: the original.
 * Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct gm_class;
namespace gm_t60 {
static inline const t60w::Ops<gm_class>& ops() { return t60objw::Logic<gm_class>::kOps; }
/* daGM_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace gm_t60
