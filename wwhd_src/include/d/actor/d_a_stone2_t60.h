/* d_a_stone2_t60.h (WWHD, true 60 build; 2026-11-18): the liftable / breakable rock
 * (d_a_stone2.cpp, profile 0x1C7) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_stone2.cpp keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the barrel2 form through d/t60_whole.h, as the
 * carryable stone (d_a_stone_t60.h): mode_wait / mode_carry decide from Link's carry status (actor_status 0x2000) and
 * grab flags (0x8000 lift, 0x20 put-down: the lift smoke), damage_cc_proc from the AT / TG hits of the tick30 collision
 * Move; the thrown rock moves through the shared mover (daObj::posMoveF_stream) and the ground correction,
 * and a mode change runs the new mode in the same frame (carry -> drop, hit -> fine + break demo order). So the whole
 * Execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state: the mover's original move,
 * every decision with the other actors' current state, the thrown rock's attack cylinder registered once for the tick30
 * Move (the one-sweep-per-frame attack rule; the tool's per-step form registered it on every step), the effects, sounds and demo orders once.
 * The earlier steps change no logic state and show the rock moving on by the previous frame's change (position,
 * shape_angle) times the frame's dt so far, not while Link carries it. Every step still runs set_mtx and hands its
 * matrix (M_tmp_mtx, a static shared by all rocks) to the background wrapper (stoneMoveBGExecute), as the original
 * Execute does. dt 1: the original. Negative control: T60C_NEGCTRL runs the Execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace stone2_t60 {
static inline void get(daStone2::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->shape_angle.x, v[4] = (f32)(s16)a->shape_angle.y, v[5] = (f32)(s16)a->shape_angle.z;
}
static inline void set(daStone2::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->shape_angle.x = t60w::angle(v[3]), a->shape_angle.y = t60w::angle(v[4]), a->shape_angle.z = t60w::angle(v[5]);
}
/* the matrix step runs in the Execute hook on every step (the background wrapper reads it) */
static inline void mtx(daStone2::Act_c*) {}
/* held: Link carries the rock (fopAcM carry status, or the frame ended in mode_carry) */
static inline bool held(daStone2::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & 0x2000) != 0 || static_cast<s32>(a->mMode) == daStone2::Act_c::Mode_Carry;
}
static const t60w::Ops<daStone2::Act_c> kOps = {6, 3, get, set, mtx, held};
}  // namespace stone2_t60
