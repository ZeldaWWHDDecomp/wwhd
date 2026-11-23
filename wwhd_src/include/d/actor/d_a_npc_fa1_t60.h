/* d_a_npc_fa1_t60.h (WWHD, true 60 build; 2026-11-18): the recovery fairy
 * (d_a_npc_fa1.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_npc_fa1.cpp keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads; as coming2 / stone2): the fairy's frame decides
 * and moves from Link's state: it turns its neck and hovers towards Link (findPlayer, hover_move), orbits Link's
 * position once touched or released from a bottle (get_player_move, bottle_appear_move: position = Link + offset), is
 * touched through the tick30 collision Move's CO hit (Fa1_checkCollision), is caught through the bottle-catch event
 * (checkBinCatch: INCATCH, and the catch request every frame for dAttention_c::Run, which stays on the tick30 step with
 * its requesters) and follows the Big Fairy's / Baba bud's position (bottle_baba_*). Its own frame holds
 * frame timers (mTimer, mMoveTimer, mWanderTimer), random draws (timers, light flicker), the speed / gravity steps
 * and the orbit acceleration. So the whole _execute runs once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h): every decision, timer, draw, move, the catch / look requests, the item get and
 * the sounds once per frame, as the original. The earlier steps change no logic state and show the fairy moving on
 * by the previous frame's change of its position and facing times the frame's dt so far (setMtx, which also
 * re-poses the model and the attention / eye points: visual; the tick30 run sets them again). Never held. dt 1: the
 * original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace fa1_t60 {
static inline void get(daNpc_Fa1_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->shape_angle.y;
}
static inline void set(daNpc_Fa1_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->shape_angle.y = t60w::angle(v[3]);
}
static inline void mtx(daNpc_Fa1_c* a) { a->setMtx(); }
static inline bool held(daNpc_Fa1_c*) { return false; }
static const t60w::Ops<daNpc_Fa1_c> kOps = {4, 3, get, set, mtx, held};
}  // namespace fa1_t60
