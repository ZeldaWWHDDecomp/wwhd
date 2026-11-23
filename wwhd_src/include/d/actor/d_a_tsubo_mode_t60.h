/* d_a_tsubo_mode_t60.h (WWHD, true 60 build; 2026-11-18): the small carryable objects (pots,
 * skulls, barrels, stools, ...: d_a_tsubo, profile 0x1C9 family; execute in d_a_tsubo_mode.cpp) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * d_a_tsubo_mode.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables: the pot's frame decides from Link's carry / hookshot-carry status
 * (actor_status 0x102000) and grab flags (se_pickup_carry), the tick30 collision Move's hits and pushes (damage_cc_proc,
 * m_cc_move), moves through the shared movers (daObj::posMoveF_stream / posMoveF_grade; fopAcM_posMoveF),
 * swings on springs (moment_proc_call, set_tensor_*: the pose tensor), decays the wind push and changes mode within the
 * frame (carry -> drop / put). So the whole _execute runs once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h): every decision with the other actors' current state, the movers' original move,
 * the springs once, the cylinder registered once for the tick30 Move (one-sweep-per-frame attack), effects and sounds once. The earlier steps
 * change no logic state and show the pot moving on by the previous frame's change of its position and model height
 * offset (mOffsetY) times the frame's dt so far (set_mtx; the pose matrix stays on the 30 Hz grid, visual only: 30 Hz visual-motion),
 * none while Link carries it. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace tsubo_t60 {
static inline void get(daTsuboMode::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z, v[3] = a->mOffsetY;
}
static inline void set(daTsuboMode::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2], a->mOffsetY = v[3];
}
static inline void mtx(daTsuboMode::Act_c* a) { tsuboMode_set_mtx(a); }
/* held: Link carries the pot (carry / hookshot carry status, or the frame ended in mode_carry) */
static inline bool held(daTsuboMode::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & 0x102000) != 0 || static_cast<s32>(a->mMode) == 4;
}
static const t60w::Ops<daTsuboMode::Act_c> kOps = {4, 4, get, set, mtx, held};
}  // namespace tsubo_t60
