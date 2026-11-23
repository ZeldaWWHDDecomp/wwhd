/* d_a_bo_t60.h (WWHD, true 60 build, 2026-11-19): Boko Baba (d_a_bo.cpp, bo_class) on a
 * general step length (dt = 1/N). Decomp build only. Separate
 * file: the actor's source keeps thin calls.
 *
 * the whole-frame form rule, whole form: the frame reads Link (the grab / hold of Link, the bite, its
 * distance and state), the tick30 collision hits (damage_check) and the enemy ice / fire, so daBO_Execute runs whole once
 * per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form); the earlier steps
 * change no logic state. Link's state is read at the frame end, as A reads it (the process-order read rule). The picture: frame interpolation.
 * The light state (settingTevStruct in draw_SUB) is made inside the execute, so on the tick30 step only.
 * daBO_Create calls daBO_Execute once (the first frame's pose): that call is the original at dt 1 (bo_t60::createBegin / createEnd).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bo_class;
namespace bo_t60 {
static inline const t60w::Ops<bo_class>& ops() { return t60objw::Logic<bo_class>::kOps; }
/* daBO_Create's execute call: the original at dt 1 (t60w::on() is false at dt 1, so the Guard does nothing) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace bo_t60
