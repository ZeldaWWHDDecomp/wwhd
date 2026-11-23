/* d_camera_follow_t60.h (WWHD, true 60 build, 2026-11-18, rule camera line):
 * the follow camera's turn-behind counter start on a general step length. Decomp build only.
 *
 * followCamera's turn-behind counter (Follow::m38C) starts when Link's status shows a hang / ladder / climb (status0
 * HANG / 0x2000000, status1 0x10000, or the bVar2 case): the call that sees m38C == 0 sets it to +-1 and leaves the
 * yaw ratio m3B8 as the stick part computed above; from the NEXT frame on the counter gives the ratio (c / 30, the swing
 * behind Link) and counts 30 Hz frames (frameEnd). At N steps the converted camera saw Link's status on the frame's
 * first step (Link runs before it), set m38C there, and its next step of the SAME frame already took c = 1: the swing
 * started one step after the status change instead of one frame (ld66a ladder start 65 step 2 vs A 66: eye 67.6 @72;
 * cl66b climb start 102 step 2 vs A 103: eye 39.5 @106). the input rule: a frame count started on a step starts counting on
 * the next frame's first step.
 * Form: a counter start on an in-between step (not the frame's tick30 step) holds the turn-behind block for the
 * frame's remaining steps (the start frame's rest: no counter change, m3B8 as computed). The hold ends at the frame end
 * or at a gap (the block did not run on the previous step). Host memory, one camera. dt = 1: never held. */
#pragma once
#include "d/d_camera_key_t60.h"
#include "true60.h"

namespace camera_follow_t60 {
struct TurnStart {
    uint32_t last;  /* step counter of the block's last run */
    bool inFrame;   /* that run was not the frame's tick30 step */
    bool started;   /* the counter started on that run or held since */
    bool valid;
};
static camera_key_t60::PerCam<TurnStart> s_turnStart;

/* once per run of the turn-behind block, before its counter tests: true on a later step of the frame whose earlier
 * step started the counter */
static inline bool turnStartHeld() {
    if (t60::dt() == 1.0f)
        return false;
    TurnStart& s = s_turnStart();
    const uint32_t now = t60_step_counter();
    const bool held = s.valid && s.started && s.inFrame && s.last + 1 == now;
    s.last = now;
    s.inFrame = !t60::tick30();
    s.started = held;
    s.valid = true;
    return held;
}
/* the block set the counter 0 -> +-1 on this run */
static inline void turnStartMark() {
    if (t60::dt() == 1.0f)
        return;
    s_turnStart().started = true;
}
} // namespace camera_follow_t60
