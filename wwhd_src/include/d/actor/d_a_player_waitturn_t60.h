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
 * angles themselves; handled as they are met). dt 1: never.
 * (2026-11-23) also included by d_a_player_main_07.cpp (procGrabWait) for the same catch-up there, and
 * used by procFreeWait / procBoomerangCatch (d_a_player_main_04b.cpp / _07.cpp): each unit keeps its own record. */
#pragma once
#include "true60.h"

struct LkT60WaitTurn {
    uint32_t stamp; /* step counter of the record */
    s16 angle, shape;     /* before the turn */
    f32 speed;            /* mNormalSpeed before the turn */
    s16 angle1, shape1;   /* after the turn */
    f32 speed1;
    bool valid;
    /* before the turn: mDirection (setSpeedAndAngleAtn's branch, setBlendAtnMoveAnime's previous
     * direction) and mMaxNormalSpeed (the turn's speed chase); noted by lkT60_waitTurnNoteAtn */
    u8 dir;
    f32 maxSpeed;
    f32 m3598; /* setBlendAtnMoveAnime's strafe blend state (the init's call writes it) */
    bool atnNoted;
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
    s_lkT60WaitTurn.atnNoted = false;
}
/* the caller, first step, before the turn (after lkT60_waitTurnBegin): the values an ATN_MOVE
 * catch-up puts back before it redoes the turn and the init's blend */
static inline void lkT60_waitTurnNoteAtn(u8 dir, f32 maxSpeed, f32 m3598) {
    if (!lkT60_waitTurnStep() || s_lkT60WaitTurn.stamp != t60_step_counter())
        return;
    s_lkT60WaitTurn.dir = dir;
    s_lkT60WaitTurn.maxSpeed = maxSpeed;
    s_lkT60WaitTurn.m3598 = m3598;
    s_lkT60WaitTurn.atnNoted = true;
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
/* (2026-11-23; pig lift route pigH, : A's frame 76 turns procGrabWait's whole step 173 -> 1029
 * and enters MOVE, B's first step its dt share (625), the MOVE body then held on the later step: B one frame behind in
 * angle, Link x off 7.34 @205) the same catch-up for the procedures that turn as procWait does and then ask
 * checkNextMode: procGrabWait, procFreeWait, procBoomerangCatch. NC WWHD_T60_GRABTURN_NEGCTRL=1: procWait only, as
 * before. */
static constexpr s32 kLkT60WaitTurnMove = 6; /* daPyProc_MOVE_e (procMove_init: commonProcInit(6)) */
static inline bool lkT60_grabTurnNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_GRABTURN_NEGCTRL"); return e && e[0] == '1'; }();
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

/* procGrabWait / procFreeWait / procBoomerangCatch after checkNextMode entered a procedure: this
 * step's record when the entered procedure is MOVE (only) and its init left the turn's values (the caller redoes the
 * turn) */
static inline const LkT60WaitTurn* lkT60_moveTurnCatchUp(uint32_t curProcEa, s32 curProc, s16 angle, s16 shape, f32 speed) {
    if (curProc != kLkT60WaitTurnMove) {
        s_lkT60WaitTurn.valid = false;
        return nullptr;
    }
    return lkT60_waitTurnCatchUp(curProcEa, curProc, true, angle, shape, speed);
}

/* (2026-11-23; Z-target strafe start, a Z-target pair: Z 2.0-5.0 s, no
 * target, strafe right from 2.5 s: A's frame 76 turns setSpeedAndAngleAtn's whole step 14986 -> 12258 and enters
 * ATN_MOVE, B's first step its dt share (13559), the ATN_MOVE body then held on the later step: B a frame of turn
 * behind, Link xz 1.4 -> 10.5 by frame 84) the same catch-up for an entered ATN_MOVE, from procWait, procMove and the
 * three procedures above. procAtnMove_init's setBlendAtnMoveAnime(2.4) picks mDirection from the turned angles and its
 * previous value (the band hysteresis; a direction change forces the 2.4 morf) and reads mMaxNormalSpeed, which
 * checkNextMode's attention branch set to 12 just before the init. The caller puts back the noted direction and max
 * speed, redoes the turn at dt 1, sets 12, puts back m3598 and calls setBlendAtnMoveAnime(2.4) again with t60::tick30()
 * reading true (lkT60_frameEndBegin: the band test takes the frame-end form, as A's init at the frame's one update; the
 * f_pc_t60.h frame-work precedent): the init's own call from A's state (the second morf in the same step, before any
 * animation calc, starts from the same old frame). NC
 * WWHD_T60_ATNMOVETURN_NEGCTRL=1: no ATN_MOVE catch-up, as before. */
static constexpr s32 kLkT60WaitTurnAtnMove = 7; /* daPyProc_ATN_MOVE_e (procAtnMove_init: commonProcInit(7)) */
static constexpr f32 kLkT60AtnMaxNormalSpeed = 12.0f; /* checkNextMode: the attention branch's mMaxNormalSpeed */
static inline bool lkT60_atnMoveTurnNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_ATNMOVETURN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* after checkNextMode entered a procedure: this step's record when it entered ATN_MOVE (with the direction noted) and
 * the init left the turn's values; the caller redoes the turn and the init's blend (lkT60_waitTurnAtnRedo's order) */
static inline const LkT60WaitTurn* lkT60_atnMoveTurnCatchUp(uint32_t curProcEa, s32 curProc, s16 angle, s16 shape,
                                                            f32 speed) {
    LkT60WaitTurn& t = s_lkT60WaitTurn;
    if (curProc != kLkT60WaitTurnAtnMove || lkT60_atnMoveTurnNegctrl() || !t.atnNoted)
        return nullptr;
    if (!lkT60_waitTurnStep() || !t.valid || t.stamp != t60_step_counter())
        return nullptr;
    t.valid = false;
    if (!lkT60_procEnteredThisFrame(curProcEa) || angle != t.angle1 || shape != t.shape1 || speed != t.speed1)
        return nullptr;
    return &t;
}

/* (2026-11-24; ghost ship pair, frame 571: Link running (procMove), B
 * pressed: A's frame turns setSpeedAndAngleNormal's whole step -31255 -> -30905 and then checkNextMode enters CUT_F;
 * B's first step turned its dt share (-31069) and entered CUT_F there, the cut body not turning on the later step: the
 * frame's move along a 164-unit different heading, Link 0.61 off from then on, amplified to 1.46 by a later direction
 * pick) the same catch-up from procMove for an entered CUT_F: procCutF_init keeps current.angle.y, shape_angle.y and
 * mNormalSpeed (it stores the cut target mMoveTargetAngleY, independent of the turn); the caller redoes the turn from the
 * frame start at dt 1 (setSpeedAndAngleNormal reads mCurProc only against MOVE_TURN, which neither MOVE nor CUT_F is).
 * NC WWHD_T60_CUTTURN_NEGCTRL=1: no CUT_F catch-up, as before. */
static constexpr s32 kLkT60WaitTurnCutF = 0x42; /* daPyProc_CUT_F_e (procCutF_init: commonProcInit(0x42)) */
static inline bool lkT60_cutTurnNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CUTTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
static inline const LkT60WaitTurn* lkT60_cutTurnCatchUp(uint32_t curProcEa, s32 curProc, s16 angle, s16 shape, f32 speed) {
    LkT60WaitTurn& t = s_lkT60WaitTurn;
    if (curProc != kLkT60WaitTurnCutF || lkT60_cutTurnNegctrl())
        return nullptr;
    if (!lkT60_waitTurnStep() || !t.valid || t.stamp != t60_step_counter())
        return nullptr;
    t.valid = false;
    if (!lkT60_procEnteredThisFrame(curProcEa) || angle != t.angle1 || shape != t.shape1 || speed != t.speed1)
        return nullptr;
    return &t;
}

/* (2026-11-24; the bomb route, frame 134: procMoveTurn's
 * frame turns setSpeedAndAngleNormal's whole step and the shape chase, then checkNextMode enters MOVE; B's first step
 * turned its dt share and entered MOVE there, its body held on the later step: angle 73 short, speed 9.3282 vs A
 * 9.3097, Link 0.45 off by frame 142) the same catch-up from procMoveTurn (d_a_player_main_05.cpp): the caller redoes
 * the turn and the shape chase from the frame start at dt 1 with mCurProc read as MOVE_TURN (setSpeedAndAngleNormal's
 * MOVE_TURN test) and the chase's proc vars as the turn had them. NC WWHD_T60_MOVETURNTURN_NEGCTRL=1. */
static constexpr s32 kLkT60WaitTurnMoveTurn = 0x18; /* daPyProc_MOVE_TURN_e */
static inline bool lkT60_moveTurnTurnNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MOVETURNTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}

/* (the held body extended to middle-step entries; bomb route
 * N 4 frame 134: MOVE_TURN's ease reached its end on step 2 of 4 and checkNextMode entered MOVE there, MOVE's body then
 * turning on steps 3 and 4, while A enters MOVE at its frame end) a procedure entered on a non-tick30 step after the
 * first redoes the frame's turn from the frame start at dt 1 at the entry (the first step's record) and its body is held
 * on the frame's remaining steps (the dispatch in d_a_player_main_04 reads g_lkT60MidHold). First implemented for
 * MOVE_TURN -> MOVE (procMoveTurn). NC WWHD_T60_MIDENTRY_NEGCTRL=1. dt 1: never. */
struct LkT60MidHold {
    uint32_t tag;  /* the frame (lkT60_frameTag) */
    int32_t proc;  /* the entered procedure */
    bool valid;
};
inline LkT60MidHold g_lkT60MidHold;
static inline bool lkT60_midEntryNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MIDENTRY_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* a middle step of a converted frame (not the first, not the tick30 step) */
static inline bool lkT60_midStep() {
    return t60::dt() != 1.0f && t60::steps() > 2 && !t60::first_step() && !t60::tick30();
}
/* the first step's record of this frame (procWait-style record taken by the same caller on the frame's first step) */
static inline const LkT60WaitTurn* lkT60_midEntryRecord() {
    const LkT60WaitTurn& t = s_lkT60WaitTurn;
    if (!lkT60_midStep() || lkT60_midEntryNegctrl() || !t.valid || t60_step_counter() - t.stamp >= t60::steps())
        return nullptr;
    return &t;
}
/* the dispatch (d_a_player_main_04): the body of the procedure entered on a middle step of this frame is held */
static inline bool lkT60_midHeld(int32_t curProc, uint32_t tag) {
    const LkT60MidHold& h = g_lkT60MidHold;
    return t60::dt() != 1.0f && !t60::first_step() && h.valid && h.tag == tag && h.proc == curProc;
}

/* around the redone init blend: t60::tick30() reads true (returns the value to put back); N = 1 / no t60: nothing */
static inline u32 lkT60_frameEndBegin() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    u32 previous = wwhd_t60_tick30;
    wwhd_t60_tick30 = 1;
    return previous;
#else
    return 1;
#endif
}
static inline void lkT60_frameEndEnd(u32 previous) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    wwhd_t60_tick30 = previous;
#else
    (void)previous;
#endif
}
