/* d_a_player_waitturn_t60.h (WWHD, true 60 build, 2026-11-20): the held frame's remaining
 * share for procWait's turn when the frame enters MOVE (the 30 Hz visual-motion rule (2) addendum 2, "a step followed by held steps
 * runs with the frame's remaining dt"; from the rider pair). Decomp build
 * only. Included by d_a_player_main_04b.cpp only (host state of that unit).
 *
 * procWait turns (setSpeedAndAngleNormal / setSpeedAndAngleAtn: current.angle.y to the stick target, shape_angle.y
 * after it, the speed chase from the turned angle) and then asks checkNextMode. In A the frame turns the whole frame's
 * step and then enters MOVE (sideways stick 0.7 from standstill: -1470). At N steps the first step turned its dt
 * share (-735), entered MOVE, and the frame's later steps are held (lkT60_procOpen / the dispatch hold): B trailed A by
 * 735 from then on. Form (as d_a_player_atnturn_t60.h): on the frame's first step procWait records the angles and the
 * speed it starts its turn from and the values the turn left; when that step's checkNextMode enters MOVE
 * (lkT60_procEnteredThisFrame) and procMove_init left the angles and the speed as the turn set them, the turn is redone
 * from the recorded start at dt 1 (the frame's whole share: what A turned; the speed chase runs again from the same start,
 * now from A's angle), and procMove_init's blend (setBlendMoveAnime) is set again without a new morf from the turned
 * state, as A's init set it. Other procedures entered from procWait keep the per-step form (their inits may set the
 * angles themselves; handled as they are met). dt 1: never. */
#pragma once
#include "true60.h"

struct LkT60WaitTurn {
    uint32_t stamp; /* step counter of the record */
    s16 angle, shape;     /* before the turn */
    f32 speed;            /* mNormalSpeed before the turn */
    s16 angle1, shape1;   /* after the turn */
    f32 speed1;
    bool valid;
};
static LkT60WaitTurn s_lkT60WaitTurn;

static inline bool lkT60_waitTurnStep() { return t60::dt() != 1.0f && t60::steps() > 1 && t60::first_step(); }
/* procWait, first step, before the turn */
static inline void lkT60_waitTurnBegin(s16 angle, s16 shape, f32 speed) {
    s_lkT60WaitTurn.valid = false;
    if (!lkT60_waitTurnStep())
        return;
    s_lkT60WaitTurn.stamp = t60_step_counter();
    s_lkT60WaitTurn.angle = angle;
    s_lkT60WaitTurn.shape = shape;
    s_lkT60WaitTurn.speed = speed;
}
/* procWait, first step, after the turn */
static inline void lkT60_waitTurnEnd(s16 angle, s16 shape, f32 speed) {
    if (!lkT60_waitTurnStep() || s_lkT60WaitTurn.stamp != t60_step_counter())
        return;
    s_lkT60WaitTurn.angle1 = angle;
    s_lkT60WaitTurn.shape1 = shape;
    s_lkT60WaitTurn.speed1 = speed;
    s_lkT60WaitTurn.valid = true;
}
/* (2026-11-23; crouch, stick held while R ends the NPC call: A's
 * frame 202 turns procWait's whole 0xBB8 (-13743 -> -16743) and enters CROUCH, B's first step its dt share (-15243),
 * the crouch body then held on the later step: link-motion angle 1500 at 204, camera 3.1 at 209) the same catch-up for
 * an entered CROUCH: procCrouch_init keeps the shape angle and the speed and copies the shape angle into
 * current.angle.y (the caller redoes the turn and that copy). NC WWHD_T60_CALLTURN_NEGCTRL=1: MOVE only, as before. */
static constexpr s32 kLkT60WaitTurnCrouch = 0xE; /* daPyProc_CROUCH_e (d_a_player_main_03.h) */
static inline bool lkT60_waitTurnCrouchNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CALLTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* procWait after checkNextMode entered a procedure: this step's record when the entered procedure is MOVE (or CROUCH)
 * and its init left the turn's values (the caller redoes the turn) */
static inline const LkT60WaitTurn* lkT60_waitTurnCatchUp(uint32_t curProcEa, s32 curProc, bool enteredMove, s16 angle,
                                                         s16 shape, f32 speed) {
    LkT60WaitTurn& t = s_lkT60WaitTurn;
    if (!lkT60_waitTurnStep() || !t.valid || t.stamp != t60_step_counter())
        return nullptr;
    t.valid = false;
    const bool crouch = curProc == kLkT60WaitTurnCrouch && !lkT60_waitTurnCrouchNegctrl();
    if (!(enteredMove || crouch) || !lkT60_procEnteredThisFrame(curProcEa) ||
        angle != (crouch ? t.shape1 : t.angle1) || shape != t.shape1 || speed != t.speed1)
        return nullptr;
    return &t;
}
