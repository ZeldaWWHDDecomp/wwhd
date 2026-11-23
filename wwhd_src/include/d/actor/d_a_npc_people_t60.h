/* d_a_npc_people_t60.h (WWHD, true 60 build, 2026-11-18): the Windfall / sea
 * villagers (d_a_npc_people*.cpp, profile 0x0175, daNpcPeople_c) on a general step length (dt = 1/N).
 * Decomp build only. Separate file: the actor's sources
 * keep thin calls.
 *
 * the whole-frame form rule NPC form: the villager's frame reads Link (chkAttention / playerEyePos, the
 * talk start, chkSurprise's front-roll crash and distance), the tick30 message status and the event manager's order
 * window, and changes mode within the frame (executeSetMode). So the whole _execute runs once per 30 Hz frame on the
 * tick30 step at dt 1 from the frame-start state (d/t60_whole.h): every decision with the other actors' current state,
 * the morf / head morf / texture pattern plays, the mover (fopAcM_posMoveF + CrrPos), the collider set once for the
 * tick30 collision Move (the one-sweep-per-frame attack rule). The earlier steps change no logic state and show the villager moved on by the
 * previous frame's change (position, facing current.angle.y) times the frame's dt so far (setMtx). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace people_t60 {
static inline void get(daNpcPeople_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->current.angle.y;
}
static inline void set(daNpcPeople_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->current.angle.y = t60w::angle(v[3]);
}
static inline void mtx(daNpcPeople_c* a) { a->setMtx(); }
static inline bool held(daNpcPeople_c*) { return false; }
static const t60w::Ops<daNpcPeople_c> kOps = {4, 3, get, set, mtx, held};
}  // namespace people_t60
