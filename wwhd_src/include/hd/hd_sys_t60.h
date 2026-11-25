/* hd_sys_t60.h (WWHD, true 60 build): (2026-11-25; found by the
 * carriables helper on long2 seg 17).
 *
 * The HD game task's per-frame function (hdGameTask_calc, 0203593C) runs, before the loop body (the play scene and the
 * actors), the parts that count 30 Hz frames: the HOME / system / button / controller hold counters and the input
 * event manager (cking::ui::input::EventMgr::calc: the frame's button events, among them Plus -> the pause request that
 * dScnPly_Execute takes). In the original frame they come before the play scene's execute. With the frame-level work on
 * the frame's first step (the first-step frame-level rule, WWHD_T60_SCENE_FIRST) and these parts on the tick30 step, at N > 1 the scene
 * of frame F ran before F's event update: it took F's Plus a frame late (the pause menu opened one frame after A's).
 * They run on the step that runs the frame-level work (t60_frame_work_step: the first step with SCENE_FIRST, else the
 * tick30 step), once per 30 Hz frame, after that step's GamePad read (the first-step input rule: the frame's first read) and the
 * input manager. N = 1: the first step is the tick30 step, nothing changes.
 * NC WWHD_T60_SCENEINPUT_NEGCTRL=1: the tick30 step as before. */
#pragma once
#include "true60.h"
#include "f_pc/f_pc_t60.h"
#include <cstdlib>

static inline bool hdT60_sceneInputNc() {
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_SCENEINPUT_NEGCTRL");
        return e != nullptr && atoi(e) != 0;
    }();
    return nc;
}
/* the step of the frame's hold counters and input events: the frame-level work's step */
static inline bool hdT60_frameInputStep() { return hdT60_sceneInputNc() ? t60::tick30() : t60_frame_work_step(); }
