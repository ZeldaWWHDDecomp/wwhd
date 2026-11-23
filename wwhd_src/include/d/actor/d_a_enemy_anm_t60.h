/* d_a_enemy_anm_t60.h (WWHD, true 60 build): animation stop tests of converted enemies that play their animation AFTER
 * the dispatch (the Bokoblin, the ChuChu; dt = 1/N the first-step AI-timer rule / animation-driven decision)..
 *
 * (the Bokoblin's attack1 site, rate 1.2, fight_attack's isStop): the original
 * frame decides once, in its dispatch, on the animation state the previous frame's play left, then plays the frame.
 * At N steps the per-step test (the animation crossing-check rule) saw an animation end crossed mid-frame (a fractional rate) on a later step of
 * the same frame: B decided one frame EARLY. Form: the stop test is taken on the frame's FIRST step only, against the
 * live state there (= the end state of the previous frame, the state A's dispatch reads), and that answer is held on
 * the frame's later steps (the play on those steps does not change the frame's decision). A body that first runs on a
 * later step (no first-step answer this frame) and an animation set this frame (another J3DAnmTransform, or any
 * anm_init: en_t60_stop_reset) keep the per-step form. dt = 1 (N = 1, unconverted, the verify harness): the original
 * isStop. The hold uses the port's frame latch (true60.h wwhd_t60_latch_*), two keys inside the morf's frame control
 * (+2: held true, +3: held false), value = the 30 Hz frame tag mixed with the animation pointer. */
#pragma once
#include "true60.h"
#include "m_Do/m_Do_ext.h"

static inline u32 en_t60_anm_frame_tag() { return *gabi::at<be<u32>>(0x101FF558); } /* g_Counter.mCounter0 */
static inline u32 en_t60_stop_key(u32 morf) { return morf + 0x98u; }                 /* &mFrameCtrl */
static inline u32 en_t60_stop_tag(mDoExt_McaMorf* m) {
    return en_t60_anm_frame_tag() ^ ((u32)gabi::ea(m->getAnm()) * 0x9E3779B1u) ^ 0x3C3C0000u;
}

/* mDoExt_McaMorf::isStop as the frame's dispatch sees it (see above) */
static inline BOOL en_t60_isStop(mDoExt_McaMorf* m) {
    if (t60::dt() == 1.0f)
        return m->isStop();
    u32 key = en_t60_stop_key(gabi::ea(m)), tag = en_t60_stop_tag(m);
    if (t60::first_step()) {
        BOOL stop = m->isStop();
        wwhd_t60_latch_set(key + (stop ? 2u : 3u), tag);
        wwhd_t60_latch_set(key + (stop ? 3u : 2u), ~tag); /* never matches: ~tag (a fixed 0xFFFFFFFF matched a frame tag 0xFFFFFFFF) */
        return stop;
    }
    if (wwhd_t60_latch_pending(key + 2u, tag))
        return TRUE;
    if (wwhd_t60_latch_pending(key + 3u, tag))
        return FALSE;
    return m->isStop();
}

/* an animation (re)set on a step of this frame: the frame's held answer no longer applies to it (the same animation set
 * again restarts it). morf: the guest address of the mDoExt_McaMorf. dt = 1: nothing. */
static inline void en_t60_stop_reset(u32 morf) {
    if (t60::dt() == 1.0f || morf == 0)
        return;
    /* never matches: ~tag of the morf's current animation (a fixed 0xFFFFFFFF matched a frame tag 0xFFFFFFFF) */
    const u32 none = ~en_t60_stop_tag(gabi::at<mDoExt_McaMorf>(morf));
    wwhd_t60_latch_set(en_t60_stop_key(morf) + 2u, none);
    wwhd_t60_latch_set(en_t60_stop_key(morf) + 3u, none);
}
