/* d_a_player_ropelet_t60.h (WWHD, true 60 build; 2026-11-25): the rope swing's
 * let-go on an earlier step of a 30 Hz frame. Decomp build only. Included by
 * d_a_player_main_07.cpp only (host state of that unit).
 *
 * procRopeSwing runs the frame's swing (the phases m35A8 / m35AC, the pump mProcVar2, the side swing mProcVar3, the
 * turn mRopeTurnSpeed / shape_angle.y, the tilt chases shape_angle.x / .z, Link placed on the rope end) and then lets go
 * when changeRopeEndProc sees A / B (mItemTrigger & 3): the release speed (mNormalSpeed, speed.y from mProcVar2's ratio),
 * its direction (the phase) and the fall's start position are the frame-end swing's. At N steps the press is seen on
 * the frame's first step, so B let go after that step's dt share of the swing: from the half-swung rope end (pair
 * long3G13n2, Abesso grapple: the swing exact to 2767, the release frame 2768 FALL pos 6.26 off, speedF 13.18 / A 13.16).
 * Form: on a step before the tick30 step with A / B triggered (the let-go's own test; not on "ADMumi", where
 * changeRopeEndProc never lets go), the frame-start swing state (noted on the frame's first step) is put back and the
 * whole procedure runs once at dt 1: A's frame (the whole swing, then the release from the frame-end state); the frame's
 * later steps run the fall. The item-button let-go (itemTrigger without a rope tag) keeps the per-step form.
 * NC WWHD_T60_ROPELET_NEGCTRL=1: the per-step let-go (as before). dt 1: never (the original). */
#pragma once
#include "true60.h"
#include <cstdlib>

struct LkT60RopeLet {
    uint32_t stamp; /* the frame's first-step counter of the record */
    bool valid;
    f32 a8, ac;     /* m35A8 / m35AC: the swing phases */
    s16 pv2, pv3;   /* mProcVar2 / mProcVar3 */
    s16 turn;       /* mRopeTurnSpeed */
    s16 sx, sy, sz; /* shape_angle */
    s16 ay;         /* current.angle.y */
    f32 px, py, pz; /* current.pos */
    u32 mode;       /* mModeFlg */
};
inline LkT60RopeLet g_lkT60RopeLet;

static inline bool lkT60_ropeLetNegctrl() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_ROPELET_NEGCTRL");
        s = (e && e[0] == '1') ? 1 : 0;
    }
    return s == 1;
}
/* procRopeSwing's entry, the frame's first step (dt < 1): the frame-start swing state */
template <class P, class T>
static inline void lkT60_ropeLetNote(P* lk, T* turnSpeed) {
    LkT60RopeLet& r = g_lkT60RopeLet;
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    r.stamp = t60_step_counter();
    r.valid = true;
    r.a8 = lk->m35A8, r.ac = lk->m35AC;
    r.pv2 = lk->mProcVar2, r.pv3 = lk->mProcVar3;
    r.turn = *turnSpeed;
    r.sx = lk->shape_angle.x, r.sy = lk->shape_angle.y, r.sz = lk->shape_angle.z;
    r.ay = lk->current.angle.y;
    r.px = lk->current.pos.x, r.py = lk->current.pos.y, r.pz = lk->current.pos.z;
    r.mode = lk->mModeFlg;
}
/* procRopeSwing's entry: true when this step lets go on a step before the tick30 step; the frame-start state is back
 * (a later step) and the caller runs the whole procedure once at dt 1. trig = mItemTrigger, noLetGo = the stage where
 * changeRopeEndProc never lets go. */
template <class P, class T>
static inline bool lkT60_ropeLetRedo(P* lk, T* turnSpeed, u32 trig, bool noLetGo) {
    if (t60::dt() == 1.0f || t60::tick30() || !(trig & 3) || noLetGo || lkT60_ropeLetNegctrl())
        return false;
    LkT60RopeLet& r = g_lkT60RopeLet;
    if (!t60::first_step()) {
        if (!r.valid || t60_step_counter() - r.stamp >= (uint32_t)t60::steps())
            return false; /* no record of this frame (the swing began on a later step): the per-step form */
        lk->m35A8 = r.a8, lk->m35AC = r.ac;
        lk->mProcVar2 = r.pv2, lk->mProcVar3 = r.pv3;
        *turnSpeed = r.turn;
        lk->shape_angle.x = r.sx, lk->shape_angle.y = r.sy, lk->shape_angle.z = r.sz;
        lk->current.angle.y = r.ay;
        lk->current.pos.x = r.px, lk->current.pos.y = r.py, lk->current.pos.z = r.pz;
        lk->mModeFlg = r.mode;
    }
    return true;
}
