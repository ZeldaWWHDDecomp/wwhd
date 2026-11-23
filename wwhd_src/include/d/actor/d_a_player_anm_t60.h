#pragma once
/* true60 (decomp build only): Link's animation-driven decisions (the animation-driven decision rule;
 * from pair and pair).
 *
 * The 30 Hz update plays Link's animations (animeUpdate) BEFORE the procedure dispatch and then decides once on the
 * frame-end state:
 *   - a stop test (`getRate() < 0.01`: the frame control's update stopped the animation at its end) takes its branch
 *     in the frame whose play reaches the end, and the new motion runs for that whole frame;
 *   - a checkPass lookahead (`checkPass(f)`: does the NEXT update's play pass frame f) looks one frame past the end of
 *     the current frame.
 * At N steps the animation crossing-check rule (next = cur + rate x dt) decided on the step whose play reached the end (up to (N - 1) / N frame
 * late: the pair's spin run-out, z 9.5 behind) and looked only one STEP ahead (a lookahead pass a frame late: the pair's
 * sword draw -> cut, B 125 / A 124).
 *
 * The first-step form (first-step AI-timer for Link): on the frame's first step (after its play)
 *   lkT60_endRate(fc)   the frame control's rate at the END of the frame: the frame's remaining plays ((N - 1) updates
 *                       at rate x dt; none while a new animation set this frame waits for the next one, animation-phase lead exception)
 *                       run on a guest-stack copy through the real J3DFrameCtrl::update; `< 0.01` = "stops within this
 *                       frame's plays";
 *   lkT60_pass(fc, f)   checkPass from that frame-end copy over one whole frame (the copy's rate x N: the guest
 *                       checkPass looks rate x dt ahead), i.e. exactly A's window.
 * The chosen branch is held over the rest of the frame: a later step's stop test projects to the same frame end (the
 * remaining plays from the step's index in the frame), so it answers as the first step did unless that step's branch
 * changed the animation; a later step's pass test answers false (the frame's one lookahead was taken on the first
 * step: the original evaluates it once per frame). A body that first runs on a later step of the frame (no
 * first-step record in this translation unit for the frame) keeps the per-step form (the original runs no body of it in
 * that frame). dt 1 (the original): the plain getRate() / checkPass(), the same reads and calls.
 *
 * Host state: one per-translation-unit stamp of the frame's first step (the split-actor state rule (1): each Link part its own). */
#include "true60.h"
#include "d/actor/d_a_player_t60.h"

static uint32_t s_lkT60AnmFirst = 0xFFFFFFFFu; /* t60_step_counter() of the frame's first step (this TU saw it) */

/* the frame's plays still to come after this step's, or -1 when this TU has no record of the frame's first step */
static inline int lkT60_anmRemaining() {
    if (t60::first_step()) {
        s_lkT60AnmFirst = t60_step_counter();
        return (int)t60::steps() - 1;
    }
    uint32_t idx = t60_step_counter() - s_lkT60AnmFirst;
    if (idx >= t60::steps()) {
        return -1;
    }
    return (int)t60::steps() - 1 - (int)idx;
}

/* (2026-11-21, pot pair): a control playing from the frame's first step (J3DAnimation.cpp's
 * frame-start form) ends the frame on ONE original update from its frame-start frame and rate (A's add, bit-equal), so
 * the projection runs that update at dt 1 on the copy instead of the remaining per-step plays (whose sum was 8.999998
 * where A's add is 9.0). Game build only (the export lives in J3DAnimation.cpp); elsewhere the per-step plays. */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((weak)) int wwhd_t60_fc_frame_start(uint32_t ea, float* start, float* rate);
#endif
/* the copy put at the frame end through the frame-start form; false: not applicable (play the remaining steps) */
static inline bool lkT60_projectFrameEnd(J3DFrameCtrl* fc, J3DFrameCtrl* copy) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    float start, rate;
    if (wwhd_t60_fc_frame_start && wwhd_t60_fc_frame_start(gabi::ea(fc), &start, &rate)) {
        copy->mFrame = start;
        copy->mRate = rate;
        const float dt0 = t60_set_dt(1.0f);
        gabi::call(0x027F2FC4, copy);
        t60_set_dt(dt0);
        return true;
    }
#else
    (void)fc;
    (void)copy;
#endif
    return false;
}
/* a guest-stack copy of the frame control after `plays` more updates (J3DFrameCtrl::update 027F2FC4 at this dt) */
static __attribute__((noinline)) f32 lkT60_projectRate(J3DFrameCtrl* fc, int plays) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    if (plays <= 0 || !lkT60_projectFrameEnd(fc, c.get())) /* the frame-start form's frame end */
        for (int i = 0; i < plays; i++) {
            gabi::call(0x027F2FC4, c.get());
        }
    return c->mRate;
}

static __attribute__((noinline)) BOOL lkT60_projectPass(J3DFrameCtrl* fc, int plays, f32 frame) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    if (plays <= 0 || !lkT60_projectFrameEnd(fc, c.get())) /* the frame-start form's frame end */
        for (int i = 0; i < plays; i++) {
            gabi::call(0x027F2FC4, c.get());
        }
    /* one whole frame from the frame end: the guest checkPass looks rate x dt ahead (N a power of two: exact) */
    c->mRate = (f32)c->mRate * (f32)t60::steps();
    return gabi::call<BOOL>(0x027F2BF8, c.get(), frame);
}

/* the frame of a guest-stack copy after `plays` more updates (setMoveAnime's carried phase at the frame end) */
static __attribute__((noinline)) f32 lkT60_projectFrame(J3DFrameCtrl* fc, int plays) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    if (plays <= 0 || !lkT60_projectFrameEnd(fc, c.get())) /* the frame-start form's frame end */
        for (int i = 0; i < plays; i++) {
            gabi::call(0x027F2FC4, c.get());
        }
    return c->mFrame;
}

/* the stop test's rate: `fc->getRate()` at dt 1, the frame-end rate at N steps */
static inline f32 lkT60_endRate(J3DFrameCtrl* fc) {
    if (t60::dt() == 1.0f) {
        return fc->mRate;
    }
    int plays = lkT60_anmRemaining();
    if (plays <= 0 || lkT60_anmSetPending(gabi::ea(fc))) {
        return fc->mRate;
    }
    return lkT60_projectRate(fc, plays);
}

/* (orca pair frame 1081): a frame threshold that opens the next-mode check (`getFrame() > m35A0`,
 * then checkNextMode: the B / stick reads) is A's once-a-frame test on the frame-end frame (A plays before the
 * dispatch). At N steps the threshold opened on the step whose play crossed it, here the tick30 step, while the frame's
 * button trigger was read on the first step: A's CUT_L at 1081 never started in B (LARGE_DAMAGE_UP ran out, WAIT 1090).
 * The frame end: `fc->getFrame()` at dt 1; on any step with a first-step record the frame-end frame of the projection
 * (the same answer on every step of the frame); without a record or while a new animation waits, the per-step frame. */
static inline f32 lkT60_endFrame(J3DFrameCtrl* fc) {
#if defined(LKT60_ENDFRAME_NEGCTRL)
    return fc->mFrame; /* negative control: the per-step frame */
#endif
    if (t60::dt() == 1.0f) {
        return fc->mFrame;
    }
    int plays = lkT60_anmRemaining();
    if (plays <= 0 || lkT60_anmSetPending(gabi::ea(fc))) {
        return fc->mFrame;
    }
    return lkT60_projectFrame(fc, plays);
}

/* the lookahead test: `fc->checkPass(f)` at dt 1, A's window on the frame's first step, false on its later steps */
static inline BOOL lkT60_pass(J3DFrameCtrl* fc, f32 frame) {
    if (t60::dt() == 1.0f) {
        return fc->checkPass(frame);
    }
    int plays = lkT60_anmRemaining();
    if (plays < 0) {
        return fc->checkPass(frame); /* no first-step record: the per-step form */
    }
    if (!t60::first_step()) {
        return FALSE;
    }
    return lkT60_projectPass(fc, lkT60_anmSetPending(gabi::ea(fc)) ? 0 : plays, frame);
}
