/* d_a_ss_t60.h (WWHD, true 60 build, 2026-11-19): the tentacle core (Ss, d_a_ss.cpp, ss_class)
 * on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: core_move and hand_move
 * decide from Link (his distance, the grab and the sword cut) and the tick30 collision hits, write the grab into Link,
 * and the counter, four timers, the damage and head-swing timers, the RNG draws and the morf play advance once per
 * 30 Hz frame. So daSs_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state
 * (d/t60_whole.h, t60objw form); the earlier steps change no logic state. The picture: frame interpolation. The morf is calculated
 * in the execute (nodeCallBack runs inside that calc and reads only the execute's head angles); daSs_Draw makes the
 * light state (settingTevStruct) on the tick30 step's draw only (rule 14), the line material update draws the
 * execute's segments. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step
 * at dt. */
#pragma once
#include "d/t60_objw.h"

struct ss_class;
namespace ss_t60 {
static inline const t60w::Ops<ss_class>& ops() { return t60objw::Logic<ss_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace ss_t60
