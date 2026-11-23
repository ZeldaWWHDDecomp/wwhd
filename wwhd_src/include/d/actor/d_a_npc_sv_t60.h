/* d_a_npc_sv_t60.h (WWHD, true 60 build, 2026-11-18): the salvage corps (Sv) (d_a_npc_sv*.cpp,
 * daNpcSv_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule NPC form: the NPC's frame reads Link (attention / talk start), the tick30
 * message status and the event manager's order window, and changes mode within the frame. So the whole _execute runs
 * once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h). The earlier steps change
 * no logic state and show the NPC moved on by the previous frame's change (position, facing current.angle.y) times the frame's dt so far:
 * setMtx (scale and base matrix only) (no animation play, no ground check, no attention / eye position: those are logic). Picture of the skeleton
 * between frame ends: frame interpolation. Always held (see held). dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on
 * every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace sv_t60 {
static inline void get(daNpcSv_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->current.angle.y;
}
static inline void set(daNpcSv_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->current.angle.y = t60w::angle(v[3]);
}
/* the picture: setMtx (scale and base matrix only) */
static inline void mtx(daNpcSv_c* a) {
    a->setMtx();
}
/* placed by its salvage ship: daObj_Ikada_c (type 4) writes the crew's current.pos from its own matrix in its frame
 * (d_a_obj_ikada.cpp, mSvId[]); a picture position put back on the tick30 step would overwrite the ship's placement
 * when the ship executes first. So nothing is shown or put back (the held case): the picture stays the frame end. */
static inline bool held(daNpcSv_c*) { return true; }
static const t60w::Ops<daNpcSv_c> kOps = {4, 3, get, set, mtx, held};
}  // namespace sv_t60
