/* d_a_bomb2_t60.h (WWHD, true 60 build; 2026-11-18): the bomb flower's bomb (d_a_bomb2.cpp)
 * on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_bomb2.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables (as stone2 / tsubo / obj_barrel): the bomb's frame decides from
 * Link's carry status, the tick30 collision Move's hits and pushes, the ground / water hits and the wind; it moves
 * through the shared movers, swings on its pose tensor (tensor_proc_call, vib_proc) and counts its fuse to the
 * explosion within the frame (mode_proc_call: wait / carry / explode / sink). So the whole _execute runs once per 30 Hz
 * frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h): the fuse and explosion intervals, the
 * decisions, the movers, the springs, the cylinder registration (one-sweep-per-frame attack), effects and sounds once per frame, as the
 * original. The earlier steps change no logic state and show the bomb moving on by the previous frame's change of its
 * position times the frame's dt so far (set_mtx, which also moves the fuse emitters: visual), none while Link carries
 * it. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* the held picture */
#include <cstdlib>
#include <cstring>

namespace bomb2_t60 {
static inline void get(daBomb2::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
}
static inline void set(daBomb2::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
}
/* true60 (2026-11-22, bf.sh pair / control): set_mtx ends in
 * eff_fuse_move, which shifts the fuse emitters' old positions and, while the fuse burns, starts the ignition sound
 * (se_ignition, 0x6100). The whole form's picture (an earlier step) and its put-back on the tick30 step ran set_mtx too:
 * the sound up to three times a frame (241 starts, A = control 111) and the old positions shifted per call
 * (mFuseOldOldPos +0x8D0 off). Form: a picture set_mtx (s_picMtx) moves the model and the fuse emitter (mFusePos) only;
 * the shift and the sound stay with the tick30 run's own set_mtx, once a frame as A. One unit only (static). NC
 * WWHD_T60_BOMB2SND_NEGCTRL=1. dt 1: no picture step (the original). */
static bool s_picMtx = false;
static inline void mtx(daBomb2::Act_c* a) {
    s_picMtx = true;
    a->set_mtx();
    s_picMtx = false;
}
static inline bool picMtx() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BOMB2SND_NEGCTRL"); return e && e[0] == '1'; }();
    if (nc) /* negative control: the picture set_mtx shifts and sounds as before */
        return false;
#endif
    return s_picMtx;
}
/* held: Link carries the bomb (carry / hookshot carry status, or the frame ended in mode_carry) */
static inline bool held(daBomb2::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) != 0 ||
           static_cast<s32>(a->mState) == daBomb2::Mode_CARRY_e;
}
static const t60w::Ops<daBomb2::Act_c> kOps = {3, 3, get, set, mtx, held};
/* the execute's earlier-step tail; held, the model from this step's placement (the picture set_mtx
 * above: no fuse shift, no ignition sound). The fuse emitter's place it writes (mFusePos) is put back: held, no frame-
 * start put-back runs on the tick30 step, and its set_mtx shifts mFusePos into mFuseOldPos (A's frame-end value).
 * d/t60_held_pic.h */
static inline void heldMtx(daBomb2::Act_c* a) {
    u8 fuse[sizeof(a->mFusePos)];
    memcpy(fuse, &a->mFusePos, sizeof(fuse));
    mtx(a);
    memcpy(&a->mFusePos, fuse, sizeof(fuse));
}
static inline bool held_picture(daBomb2::Act_c* a) {
    t60hp::picture(a, kOps, heldMtx);
    return true;
}
}  // namespace bomb2_t60
