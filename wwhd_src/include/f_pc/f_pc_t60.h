/* f_pc_t60.h (WWHD, true 60 build): frame-level work on the frame's first step (the first-step frame-level rule).
 *
 *
 * The original frame is: process deletion / priority / creation (fpcDt / fpcPi / fpcCt handlers), then the executes in
 * line order, the play scene first (its execute runs the pause transfer, the event system dEvent_manager_c::runProc =
 * exceptionProc + dEvt_control_c::check + mainProc, attention and detect), then the actors. With the frame-level work on
 * the tick30 step, a converted actor's steps before it ran BEFORE its own frame's handlers and scene: an order it queued
 * on its first step was taken by the same frame's check (one frame early), a cut / event start reached it on the tick30
 * step only (its new action moved for 1/N of the frame) and stayed visible on the next frame's earlier steps, a process
 * it created was created and run in its own frame (one frame early).
 *
 * Switch WWHD_T60_SCENE_FIRST=1 (the port's wwhd_t60_scene_first; default 0 until it is switched on): the handlers and
 * the play scene's execute run on the frame's FIRST step, with dt 1, and while they run t60::tick30 reads true (they
 * are the 30 Hz world's frame-level work: shared helpers take their original, once-per-frame path). Unconverted actors
 * and fapGm_After's managers stay on the tick30 step. Order per frame: handlers, scene f, converted steps 1..N,
 * unconverted actors on step N: the original order for every party. N = 1: the first step is the tick30 step, nothing
 * changes. */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) extern int wwhd_t60_scene_first;
}
static inline bool t60_scene_first() { return wwhd_t60_scene_first != 0; }
#else
static inline bool t60_scene_first() { return false; }
#endif

/* the step that runs the frame-level work (handlers, the play scene) */
static inline bool t60_frame_work_step() { return t60_scene_first() ? t60::first_step() : t60::tick30(); }

/* around the frame-level work on the first step: tick30 reads true while it runs (returns the value to put back) */
static inline u32 t60_frame_work_begin() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    u32 previous = wwhd_t60_tick30;
    if (t60_scene_first())
        wwhd_t60_tick30 = 1;
    return previous;
#else
    return 1;
#endif
}
static inline void t60_frame_work_end(u32 previous) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    wwhd_t60_tick30 = previous;
#else
    (void)previous;
#endif
}

/* the play scene (fpcNm_PLAY_SCENE_e 7) is frame-level work */
static inline bool t60_frame_work_proc(s16 profName) { return t60_scene_first() && profName == 7; }
