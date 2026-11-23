/* d_a_player_movecos_t60.h (WWHD, true 60 build, 2026-11-20): the angle that
 * setSpeedAndAngleNormal's acceleration reads on the frame's first step. Decomp build only.
 * Included by d_a_player_main_04b.cpp only (host state of that unit: none).
 *
 * setSpeedAndAngleNormal turns current.angle.y toward the stick target (cLib_addCalcAngleS) and then takes the
 * acceleration from the turned angle: 3.5 st^2 cos(target - angle). Under rule (a) (2026-11-17) the speed
 * change (setNormalSpeedF) runs once, on the frame's first step at dt 1, and the frame's later steps hold it. The turn
 * stays per step (dt share). So on the first step the acceleration read the angle after the first step's share of the
 * turn, while A reads it after the whole frame's turn: a nested ratio (pair / control
 * tmkX2, Windfall: MOVE frame 44 nspeed A 6.483 / B 6.434 = cos at -22572 vs -22991; the gap grows into the
 * frame-532 roll split). Form: on the frame's first step at N > 1 the cosine reads the angle the original's one turn
 * gives from the frame-start angle (the original call's arithmetic at dt 1, host values); the step's own turn is unchanged. The
 * frame's later steps are held by setNormalSpeedF (their cosine is not used); a speed chase first reached on a later
 * step keeps the per-step form. dt 1: never (the original, no extra call).
 * The attention moves (setSpeedAndAngleAtn / setSpeedAndAngleAtnActor: 5 st cos(the call's turn), scale 6, steps
 * 0xBB8 / 0x7D0) read the turn as a per-frame rate (lk_atnTurnCos: step turn / dt). 2026-11-20: model
 * (the c_lib per-step form, first step): that rate is not A's turn for |diff| < 17994 (the min
 * step clamp at dt and the ratio 1 - (5/6)^dt below the max clamp): max |d| 1000 angle units at N 2, 1500 at N 4
 * (cos 0.014 / 0.017). Same form there: on the first step the turn is the original's one turn from the frame start. */
#pragma once
#include "true60.h"

static inline bool lkT60_moveCosStep() { return t60::dt() != 1.0f && t60::steps() > 1 && t60::first_step(); }
/* the angle for the acceleration's cosine: on the first step the whole frame's turn from startAngle */
static inline s16 lkT60_moveCosAngle(s16 startAngle, s16 turnedAngle, s32 target, s32 maxStep, s32 minStep,
                                     s32 scale = 5) {
    if (!lkT60_moveCosStep())
        return turnedAngle;
    /* the original's one cLib_addCalcAngleS (0200F378, dt 1) as a host value function. The s87 form ran the
     * call on a host be<s16> local: be<> is a view of guest memory, so it read / wrote the guest word at
     * ea(<host stack address>) (t60_beprobe FAIL, q87f3). Same arithmetic as c_lib.cpp's dt = 1 path
     * (t60AngleSFull; divideSignedPPC: x / 0 and INT32_MIN / -1 give 0 or -1). */
    s32 x = startAngle;
    s16 diff = (s16)((u32)target - (u32)x);
    if (x == target)
        return (s16)x;
    s32 q = !scale || (diff == INT32_MIN && scale == -1) ? (diff < 0 ? -1 : 0) : (s32)diff / scale;
    s32 step = (s16)q;
    if (step > minStep || step < (s32)(0u - (u32)minStep)) {
        if (step > maxStep)
            step = maxStep;
        if (step < (s32)(0u - (u32)maxStep))
            step = (s16)(0u - (u32)maxStep);
        return (s16)((u32)x + (u32)step);
    }
    if (diff >= 0) {
        x = (s16)((u32)x + (u32)minStep);
        if ((s16)((u32)target - (u32)x) > 0)
            return (s16)x;
    } else {
        x = (s16)((u32)x - (u32)minStep);
        if ((s16)((u32)target - (u32)x) < 0)
            return (s16)x;
    }
    return (s16)target;
}
