/* d_a_st_t60.h (WWHD, true 60 build, 2026-11-19): the Stalfos (st, d_a_st.cpp, st_class) on a
 * general step length (dt = 1/N). Decomp build only. Separate
 * file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: St_move's actions decide
 * from Link (approach, guard, attacks, the jump), the tick30 collision hits (damage_check, the break / reassemble of
 * the parts), enemy_ice and the upper / lower body link, and the frame counter, the five timers, the no-hit and
 * reassemble waits, the RNG draws, the part / effect moves and the morf advance once per 30 Hz frame. So daSt_Execute
 * runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form);
 * the earlier steps change no logic state (one AI state step per 30 Hz frame, the check item, holds by
 * construction). The picture: frame interpolation. The execute ends in the light state (settingTevStruct), so it is made on the
 * tick30 step only; daSt_Draw writes no logic state. daSt_Create's execute run is the original at dt 1 (createBegin / createEnd).
 * Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct st_class;
namespace st_t60 {
static inline const t60w::Ops<st_class>& ops() { return t60objw::Logic<st_class>::kOps; }
/* daSt_Create's execute call: the original at dt 1 (create's run, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace st_t60
