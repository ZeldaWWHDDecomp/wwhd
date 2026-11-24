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
#include "d/t60_carry_step.h" /* the step form */
#include "d/t60conv.h"

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
static inline bool carried(daStone2::Act_c* a) { return (static_cast<u32>(a->actor_status) & 0x2000) != 0; }
static const t60w::Ops<daStone2::Act_c> kOps = {6, 3, get, set, mtx, held};
/* The step form (d/t60_carry_step.h; "convert the whole-form carriables to per-step logic",
 * rule: exact-frame-move family, frame ends bit-exact, decisions on the frame grid). The rock's earlier steps run
 * the frame's motion as a picture: the thrown rock's predicted move (daObj posMoveF_stream from the frame start) and its
 * spin shape_angle.x (the step's share of 1000 a frame, t60c::frameAdd), the picture ground check (CrrPos on a copy),
 * set_mtx and its matrix for the background wrapper (carried: Link's place and bounce picture). No decision there: the
 * carry entry, the lift smoke, the throw, the bg lock timer, the hit / landing-break checks, the fine timer and the
 * break demo (the event-step rule), the room, attention, the collider and its mass (one-sweep-per-frame attack), eye. The tick30 step puts the frame-start
 * words back and runs the original Execute at dt 1. 0x1C7 is not in kT60Converted: it runs per step only with the
 * test conversion (WWHD_T60_TEST_CONVERT). NC WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form above. */
static const t60cs::Spec<daStone2::Act_c> kSpec = {{}, held, 0x7C4, 0x3E8, carried};
/* the model step of every step (carried, on a PIC step: at Link's picture place, d/t60_held_pic.h) */
static inline void picMtx(daStone2::Act_c* a) { t60cs::mtx(a, held(a), &daStone2::Act_c::set_mtx); }
}  // namespace stone2_t60
