/* d_a_wz_t60.h (WWHD, true 60 build, 2026-11-19): the Wizzrobe, its balls and summon door
 * (wz, d_a_wz.cpp, wz_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: the actions (dousa,
 * itai, demo, tama_dousa, summon_dousa) decide from Link, the tick30 collision hits, enemy_ice / enemy_fire, the
 * partner / summon search and the event system, and the five timers, the RNG draws, the teleport / summon waves, the
 * posMove / BG check and the morf / btk advance once per 30 Hz frame. So daWZ_Execute runs whole once per 30 Hz frame
 * on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form); the earlier steps change no
 * logic state (one AI state step per 30 Hz frame, the check item, holds by construction). The picture:
 * frame interpolation. The execute ends in draw_SUB (matrix, morf / rod calc, enemy_fire, the light state), so the light state is
 * made on the tick30 step only; daWZ_Draw writes no logic state (the ball matrix is a picture). daWZ_Create's
 * draw_SUB pose is the original at dt 1 (createBegin / createEnd). Never held. dt 1: the original. Negative control: T60C_NEGCTRL
 * runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct wz_class;
namespace wz_t60 {
static inline const t60w::Ops<wz_class>& ops() { return t60objw::Logic<wz_class>::kOps; }
/* daWZ_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace wz_t60
