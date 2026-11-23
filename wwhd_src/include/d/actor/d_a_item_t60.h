/* d_a_item_t60.h (WWHD, true 60 build; 2026-11-18): the field item (d_a_item.cpp:
 * rupees, hearts, keys, arrows, magic ...) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_item.cpp keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads; as coming2 / stone2): the item's frame decides
 * from other actors: Link's touch and get (checkPlayerGet, the CO / TG hits of the tick30 collision Move, the get
 * demo order), the boomerang that carries it, the rat that brings it (execBringNezumi), the boss item hand-over; its
 * own frame holds the despawn timer and the blink (timeCount, checkItemDisappear: the pickup / despawn windows), the
 * bounce / water float / wall moves (fopAcM_posMoveF, Acch), the bound sounds, the spin and the item animations
 * (animPlay). So the whole _daItem_execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start
 * state (d/t60_whole.h): every decision, timer, move, the collider registration (one-sweep-per-frame attack) and the sounds once per frame,
 * as the original. The earlier steps change no logic state and show the item moving and spinning on by the previous
 * frame's change of its position and angles times the frame's dt so far (set_mtx: the item model and an arrow
 * bundle's two arrows; visual; the tick30 run sets them again); none while its spawn switch is off or while Link's
 * hookshot carries it (his hookshot places it on every step). dt 1: the original. Negative control: T60C_NEGCTRL runs
 * the execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace item_t60 {
static inline void get(daItem_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->current.angle.x, v[4] = (f32)(s16)a->current.angle.y, v[5] = (f32)(s16)a->current.angle.z;
    v[6] = (f32)(s16)a->shape_angle.y; /* the heart container's facing (set_mtx) */
}
static inline void set(daItem_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->current.angle.x = t60w::angle(v[3]), a->current.angle.y = t60w::angle(v[4]), a->current.angle.z = t60w::angle(v[5]);
    a->shape_angle.y = t60w::angle(v[6]);
}
static inline void mtx(daItem_c* a) { a->set_mtx(); }
/* held: Link's hookshot carries the item (fopAcStts_HOOK_CARRY_e) */
static inline bool held(daItem_c* a) { return (static_cast<u32>(a->actor_status) & 0x100000u) != 0; }
static const t60w::Ops<daItem_c> kOps = {7, 3, get, set, mtx, held};
}  // namespace item_t60
