/* d_a_mbdoor_t60.h (WWHD, true 60 build; 2026-11-18): the Forsaken Fortress barred door
 * (d_a_mbdoor.cpp) on a general step length (dt = 1/N). Separate file.
 *
 * Outside its events the door does not move: its actions decide (LockWait: the unlock timer mUnlockDelay, the switch /
 * enemy check and the MBDOOR_STOP_OPEN order; LockOff: the demo command or the order again; LockDemo: the event's end;
 * CloseWait: the door command or Link's area check with the CANDOOR condition; Wait: the matrix). The door's motion
 * (demoProc: the door and bar opening, Link's goal) runs inside its events, at the 30 Hz fallback (N = 1, dt 1).
 * At dt != 1 the action dispatch runs once per 30 Hz frame, on the tick30 step: the timer counts once per frame, the
 * switch / enemy / area checks see the frame-end state (an unconverted setter has run its frame: the original's frame
 * for either line order), an order is queued once per frame and taken by the next frame's event check, as in the
 * original. The frame's other steps keep the door as it is. A demo actor (cutscene pose) is applied on every step
 * (absolute angles). The tick30 dispatch runs at dt 1 (dtOneBegin / dtOneEnd). dt 1: never consulted. No host memory. */
#pragma once
#include "true60.h"

namespace mbdoorT60 {
/* a step on which the action dispatch waits for the frame's tick30 step */
static inline bool hold() {
#ifdef MBDOOR_T60_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f && !t60::tick30();
#endif
}
/* the tick30 step's dispatch runs at dt 1 (as t60w::Guard::begin does for the whole form): its cLib eases (ACT_ADJUSTMENT's
 * cLib_addCalcAngleS2 on Link's angle, c_lib.cpp scales by t60::dt()) take the 30 Hz frame's whole step. Without it the
 * dispatch ran at the step's dt 0.5 and the ease took half its step: rbdw1n2 (head x) vs its no-conversion control
 * kbdw1n2: Link's angle 16396 vs 16406 at the open frame (12 vs 22 of the ease), 22 frames to decay (
 * 2026-11-21). Plain calls, not a scope object: a struct local's constructor / destructor does not run under the source
 * glue (true60.h t60_set_dt, d_a_kita_t60.h cbBegin); head x2's DtOne struct was a no-op (nbdw1n2 B = rbdw1n2 B).
 * dtOneBegin returns the previous dt for dtOneEnd. dt 1: a no-op. */
static inline f32 dtOneBegin() { return t60_set_dt(1.0f); }
static inline void dtOneEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace mbdoorT60
