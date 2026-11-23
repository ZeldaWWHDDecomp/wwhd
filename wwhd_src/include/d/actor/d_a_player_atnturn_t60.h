/* d_a_player_atnturn_t60.h (WWHD, true 60 build, 2026-11-18): the held frame's remaining
 * share for procAtnActorWait's turn (addendum to the 30 Hz visual-motion rule (2): "a step followed by held
 * steps runs with the frame's remaining dt"; pair). Decomp build only.
 * Included by d_a_player_main_04b.cpp only (host state of that unit).
 *
 * procAtnActorWait turns (setSpeedAndAngleAtnActor: current.angle.y to the stick target, shape_angle.y to the lock-on
 * target) and then asks checkNextMode. In A the frame turns the whole frame's step (max 0xBB8) and then enters the new
 * procedure (ATN_ACTOR_MOVE at 76: -3000). At N steps the first step turned its dt share (-1500), entered the new
 * procedure, and the frame's later steps are held (lkT60_procOpen / the dispatch hold): the frame turned half of A's.
 * Form: on the frame's first step setSpeedAndAngleAtnActor records the angles it started from and the stick target; when
 * that step's checkNextMode enters a procedure (lkT60_procEnteredThisFrame), both turns are redone from the recorded
 * start at dt 1 (the frame's whole share: what A turned). The speed (setNormalSpeedF) is a chase already run at dt 1 on
 * the first step (lkT60_chaseOpen); it keeps its value. dt 1: never. */
#pragma once
#include "true60.h"

struct LkT60AtnTurn {
    uint32_t stamp; /* step counter of the record */
    s16 angle;      /* current.angle.y before the turn (after the backward flip) */
    s16 target;     /* mMoveTargetAngleY */
    s16 shape;      /* shape_angle.y before setShapeAngleToAtnActor */
    bool turned;    /* the stick turn ran (mStickDistance > 0.05) */
    bool valid;
};
static LkT60AtnTurn s_lkT60AtnTurn;

static inline bool lkT60_atnTurnRecordStep() { return t60::dt() != 1.0f && t60::steps() > 1 && t60::first_step(); }
/* setSpeedAndAngleAtnActor, first step: the start of the turns */
static inline void lkT60_atnTurnRecord(bool turned, s16 angle, s16 target, s16 shape) {
    if (!lkT60_atnTurnRecordStep())
        return;
    s_lkT60AtnTurn.stamp = t60_step_counter();
    s_lkT60AtnTurn.angle = angle;
    s_lkT60AtnTurn.target = target;
    s_lkT60AtnTurn.shape = shape;
    s_lkT60AtnTurn.turned = turned;
    s_lkT60AtnTurn.valid = true;
}
/* procAtnActorWait after checkNextMode entered a procedure: this step's record, if any (the caller redoes the turns) */
static inline const LkT60AtnTurn* lkT60_atnTurnCatchUp(uint32_t curProcEa) {
    if (!lkT60_atnTurnRecordStep() || !s_lkT60AtnTurn.valid || s_lkT60AtnTurn.stamp != t60_step_counter() ||
        !lkT60_procEnteredThisFrame(curProcEa))
        return nullptr;
    s_lkT60AtnTurn.valid = false;
    return &s_lkT60AtnTurn;
}
