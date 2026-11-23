/* d_a_sss_t60.h (WWHD, true 60 build, 2026-11-19): the Floormaster-like vine hand (Sss,
 * d_a_sss.cpp, sss_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: hand_move decides from
 * Link (the reach and grab, the cut by his sword) and the tick30 collision hits, writes the grab into Link, and its frame
 * counter, timers, RNG draws and the vine segments advance once per 30 Hz frame. So daSss_Execute runs whole once per
 * 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form); the earlier steps
 * change no logic state. The picture: frame interpolation. daSss_Draw: the light state (settingTevStruct) on the tick30 step's draw
 * only (rule 14); the hand morf's updateDL and the vine / cut lines are drawn from the execute's state (no joint
 * callback: nothing written back). daSss_Create's execute run is the original at dt 1 (createBegin / createEnd). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct sss_class;
namespace sss_t60 {
static inline const t60w::Ops<sss_class>& ops() { return t60objw::Logic<sss_class>::kOps; }
/* daSss_Create's execute call: the original at dt 1 */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace sss_t60
