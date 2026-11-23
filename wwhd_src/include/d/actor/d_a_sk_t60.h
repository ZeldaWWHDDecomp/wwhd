/* d_a_sk_t60.h (WWHD, true 60 build, 2026-11-19): the vine skull (sk, d_a_sk.cpp, sk_class) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum, the Magtail / Floormaster whole form: dousa_move decides from Link's sword hits (the tick30
 * collision) and the sway timer, hit timer and morf play advance once per 30 Hz frame.
 * So the execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h,
 * t60objw form); the earlier steps change no logic state. The picture: frame interpolation. The draw makes the light state on the
 * tick30 step's draw only (rule 14); the rest of the draw writes no logic state. The create's execute run is the
 * original at dt 1 (createBegin / createEnd). Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute
 * on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct sk_class;
namespace sk_t60 {
static inline const t60w::Ops<sk_class>& ops() { return t60objw::Logic<sk_class>::kOps; }
/* the create's execute call: the original at dt 1 */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace sk_t60
