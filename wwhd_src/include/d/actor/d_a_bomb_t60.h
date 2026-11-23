/* d_a_bomb_t60.h (WWHD, true 60 build; 2026-11-18): Link's bomb, the bomb flower nut and
 * the cannon bomb (d_a_bomb.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_bomb.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables (as stone2 / tsubo / obj_barrel / bomb2): the bomb's frame decides
 * from Link's carry status, the tick30 collision Move's hits (checkExplodeCc), the ground / wall / water / lava hits
 * (bgCrrPos, checkExplodeBg, bound) and the wind; it moves through the shared movers (fopAcM_posMoveF,
 * daObj::posMoveF_grade), counts its fuse (checkExplodeTimer: mRestTime, the fuse length bomb tricks depend on), its
 * no-gravity time, draw delay, explosion light / collider timers within the frame, and registers its sphere once per
 * frame (mMassCounter against the frame counter: one-sweep-per-frame attack). So the whole execute runs once per 30 Hz frame on the tick30
 * step at dt 1 from the frame-start state (d/t60_whole.h): fuse and explosion intervals, decisions, movers, the
 * explosion light fade (cLib_addCalc) and its env colour / point light / point wind sets, effects and sounds (fuse
 * sound 0x6100 every frame) once per frame, as the original. The earlier steps change no logic state and show the
 * bomb moving on by the previous frame's change of its position times the frame's dt so far (set_mtx), with the fuse
 * point (mFusePos: the sparks / smoke emitters follow it) moved by the same rule; the tick30 step puts both back first,
 * so the fuse trail history (mFusePos2 / 3) is the original's. Nothing is shown while Link carries the bomb (he places
 * it on every step). dt 1: the original. Negative control: T60C_NEGCTRL runs execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* the held picture */

namespace bomb_t60 {
static inline void get(daBomb_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = a->mFusePos.x, v[4] = a->mFusePos.y, v[5] = a->mFusePos.z;
}
static inline void set(daBomb_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mFusePos.x = v[3], a->mFusePos.y = v[4], a->mFusePos.z = v[5];
}
static inline void mtx(daBomb_c* a) { a->set_mtx(); }
/* held: Link carries the bomb (carry / hookshot carry status, or the frame ended in procCarry 020C76E0) */
static inline bool held(daBomb_c* a) {
    return (static_cast<u32>(a->actor_status) & 0x102000u) != 0 || /* fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e */
           static_cast<u32>(a->mFunc.f) == 0x020C76E0u;
}
static const t60w::Ops<daBomb_c> kOps = {6, 6, get, set, mtx, held};
/* the execute's earlier-step tail; held, the model from this step's placement (set_mtx writes the
 * model's base scale / matrix and the matrix stack scratch only). d/t60_held_pic.h */
static inline BOOL held_picture(daBomb_c* a) {
    t60hp::picture(a, kOps, mtx);
    return TRUE;
}
}  // namespace bomb_t60
