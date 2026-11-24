/* c_damagereaction_bounce_t60.h (WWHD, true 60 build, 2026-11-24): a knocked-down
 * enemy's landing bounce (dr_damage_set, c_damagereaction.cpp) once per 30 Hz frame. Decomp build only.
 * Separate file: the source keeps thin calls.
 *
 * The ground branch of dr_damage_set reads mAcch's ground hit: knocked down and landing (mBounceCount 0, or falling faster
 * than 100) it bounces (speed.y 60, speed x / z x 0.75, mBounceCount + 1); grounded otherwise it stops the reaction
 * (speed 0, speed.y -1, mKnockbackSpeed 0). The original reads one ground state per frame, and after the bounce the next
 * frame's call finds the enemy rising. At N > 1 a landing on an earlier step bounced there, and the frame's next step read
 * the same ground hit again (the frame's fall term had not left the ground yet), now with mBounceCount 1 and speed.y 60:
 * the stop branch killed the bounce (a Bokoblin pair, Bokoblin 47C5CE68 knocked down from frame 165: B landed
 * on B 3695's first step, stopped dead at -343.4 -112.5 462.5 for good; A bounced to -97.5, -85.5 ... ; bk pos 208 off,
 * the lock-on camera 37 off at frame 204, the lock-on work words from 218).
 * Form: a bounce taken on an earlier step marks the frame (latch keyed by mBounceCount's address, c_damagereaction.h
 * t60dr_mark_set) and keeps the speed it set; the frame's later steps skip the ground branch (the frame's one ground
 * reaction is the bounce, as in A) and leave the bounce's speed at their end: their ground check (mAcch.CrrPos, the
 * frame's fall term still on the ground) zeroed speed.y before the branch, where A's frame sets 60 after it (a fix run with the
 * skip alone: the next frame's first step read the ground again and stopped; A rose to -97.5).
 * The ground branch's other reaction, the stop (grounded: speed 0, speed.y -1, mKnockbackSpeed 0), takes the same form:
 * A's frame makes its whole move with the frame's speed and stops after it; at N > 1 a stop on an earlier step zeroed the
 * speed the frame's later steps move with, and the tick30 step ended the frame on the zero speed's term, back at the
 * frame-start place (pair, Bokoblin lock-on, Bokoblin 47C5CE68 knocked back off a ledge, frame
 * 203: A -155.22 304.76, B -153.01 322.64, 18.02 off from then on; the lock-on camera followed).
 * dt 1: nothing is marked (the original). Negative control WWHD_T60_DRBOUNCE_NEGCTRL=1: the form before. */
#pragma once
#include "c/c_damagereaction.h"
#include <cstdlib>

namespace t60drb {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRBOUNCE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct Kept {
    u32 dr, tag;
    f32 x, y, z;     /* the bounce's speed (the frame end's, as A leaves it) */
    f32 x0, z0;      /* speed x / z before the bounce: the frame's later steps move on with it */
};
static inline Kept* kept(u32 dr, bool make) {
    static Kept s_kept[32];
    Kept* free = nullptr;
    for (Kept& k : s_kept) {
        if (k.dr == dr)
            return &k;
        if (!free && (k.dr == 0 || k.tag + 2 < t60dr_frame_tag()))
            free = &k;
    }
    if (!make)
        return nullptr;
    if (!free)
        free = &s_kept[(dr >> 4) % 32];
    free->dr = dr;
    return free;
}
/* the ground branch is skipped: this frame's bounce was taken on an earlier step; the enemy keeps the bounce's speed */
template <class E> static inline bool skip(const void* bounceCount, u32 dr, E* enemy) {
    if (nc() || !t60dr_pending(bounceCount))
        return false;
    if (Kept* k = kept(dr, false))
        if (k->tag == t60dr_frame_tag())
            enemy->speed.x = k->x, enemy->speed.y = k->y, enemy->speed.z = k->z;
    return true;
}
/* the bounce / stop branch's entry: speed x / z before it (an earlier step only) */
template <class E> static inline void entry(u32 dr, E* enemy) {
    if (nc() || t60::dt() == 1.0f || t60::tick30())
        return;
    Kept* k = kept(dr, true);
    k->x0 = enemy->speed.x, k->z0 = enemy->speed.z;
}
/* a bounce or a stop taken on this step (an earlier step: marks the frame and keeps its speed; the tick30 step and dt 1:
 * nothing). Called after the branch's speed sets. The frame's later steps move on with the speed x / z the frame had
 * (A's frame moves its whole step before the bounce's x 0.75): put back here, the kept bounce speed set at their end
 * (skip; a fix run: x / z 6.2 off from the landing frame on, A -345.9 456.9 / B -343.4 462.5). */
template <class E> static inline void note(const void* bounceCount, u32 dr, E* enemy) {
    if (nc() || t60::dt() == 1.0f || t60::tick30())
        return;
    t60dr_mark_set(bounceCount);
    Kept* k = kept(dr, true);
    k->tag = t60dr_frame_tag();
    k->x = enemy->speed.x, k->y = enemy->speed.y, k->z = enemy->speed.z;
    enemy->speed.x = k->x0, enemy->speed.z = k->z0;
}
/* the enemy's shape copy (shape_angle = current.angle before its damage_reaction call, daBk_Execute): after this frame's
 * bounce on an earlier step the copy waits for the next frame. The bounce turns current.angle.y to the landing angle in
 * the same dr_damage_set (mBounceCount != 0); A's frame copied before that turn, the next frame after it: at N > 1 the
 * tick30 step copied the turned angle a frame early (a fix run, Bokoblin 47C5CE68: shape_angle.y 4378 / A 3964 at frame 185).
 * `dr`: the reaction's address. */
static inline bool shapeHeld(u32 dr) { return !nc() && t60dr_pending(gabi::at<u8>(dr + 0x4F4)); }
/* the bounce (only: it sets mLandAngleY, +0x4F4) marks the shape copy's hold (a stop does not turn the angle; holding the
 * copy then lagged a turning Bokoblin's shape by the tick30 step's turn: a fix run, Bokoblin 47C59B00 frame 144, 512 off) */
static inline void bounced(u32 dr) {
    if (!nc())
        t60dr_mark_set(gabi::at<u8>(dr + 0x4F4));
}
}  // namespace t60drb
