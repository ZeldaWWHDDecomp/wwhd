/* d_a_player_heading_t60.h (WWHD, true 60 build; 2026-11-23): the move's heading on a turning
 * frame's earlier steps. Decomp build only. Included by
 * d_a_player_main_04b.cpp (the turns note the frame's whole turn) and d_a_player_main_03.cpp (posMoveFromFootPos reads
 * it); the record is an inline variable, one copy in the d_a_player_main unit (as d_a_player_swimrise_t60.h).
 *
 * setSpeedAndAngleNormal / setSpeedAndAngleAtn* turn current.angle.y toward the stick target by cLib_addCalcAngleS
 * (the earlier steps take a share, the tick30 step ends on the original's one step from the frame start), and
 * posMoveFromFootPos builds the move's velocity from current.angle.y. A moves the whole frame along the frame-end
 * heading; at N steps the earlier steps moved along the half-turned heading (a pig lift pair: 0.06 short at frame 77,
 * 6.1 by 219 through the follow camera's yaw; a Z strafe pair: 0.46 at 77, 3.4 at 83; the turn-path class).
 * The frame's whole turn is known on its first step: the target is the frame-start stick and the frame-start camera
 * yaw (lk_camControledAngleY, the frame-start camera read; the first-step input), and d_a_player_movecos_t60.h already computes the
 * original's one turn from the frame start there. Form (as procFanGlide's heading, d_a_player_glide_t60.h): the first
 * step notes the frame start and the whole-turn angle; on the frame's earlier steps a heading still on that turn's
 * segment (between the frame start and the whole turn) moves along the whole turn instead; current.angle.y itself (and
 * shape_angle.y, the picture) keep the per-step share; the tick30 step reads current.angle.y (the whole turn's end).
 * NC WWHD_T60_MOVEHEADING_NEGCTRL=1: the per-step heading, as before. dt 1: never. */
#pragma once
#include "true60.h"
#include <cstdlib>

struct LkT60MoveHeading {
    uint32_t stamp; /* step counter of the frame's first step that noted the turn */
    s16 start;      /* current.angle.y at the frame start (after an attention flip) */
    s16 whole;      /* the original's one turn from start */
    bool valid;
};
inline LkT60MoveHeading g_lkT60MoveHeading;

static inline bool lkT60_headingNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MOVEHEADING_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline bool lkT60_headingNoteStep() { return t60::dt() != 1.0f && t60::steps() > 1 && t60::first_step(); }
/* the turn's frame start, read only on the note step: at dt 1 the caller makes no read the original does not
 * (setAngleAtnHD 02416AB4 reads current.angle.y only through cLib_addCalcAngleS; the call-trace check 2026-11-24) */
template <class A> static inline s16 lkT60_headingStartOf(const A& angle) {
    return lkT60_headingNoteStep() ? (s16)angle : (s16)0;
}
/* the turn's caller, first step: the frame start and the frame's whole turn */
static inline void lkT60_headingNote(s16 start, s16 whole) {
    if (!lkT60_headingNoteStep())
        return;
    g_lkT60MoveHeading.stamp = t60_step_counter();
    g_lkT60MoveHeading.start = start;
    g_lkT60MoveHeading.whole = whole;
    g_lkT60MoveHeading.valid = true;
}
/* the frame's whole turn for `angle` on an earlier step of a frame whose first step noted a turn (no dt test: the speed
 * chase runs its first step at dt 1, rule (a)) */
static inline s16 lkT60_headingWhole(s16 angle) {
    const LkT60MoveHeading& h = g_lkT60MoveHeading;
    if (t60::steps() <= 1 || t60::tick30() || !h.valid || t60_step_counter() - h.stamp >= t60::steps())
        return angle;
    const s32 S = (s16)(h.whole - h.start), o = (s16)(angle - h.start);
    if (S == 0)
        return angle;
    if (S > 0 ? (o >= 0 && o <= S) : (o <= 0 && o >= S))
        return h.whole;
    return angle;
}
/* (2026-11-25; a long route at N 4, frame 5638: Link walking along a wall,
 * the sidle prompt (do status 0x10) enters WHIDE_READY from procMove). A's procMove turns current.angle.y the frame's
 * whole step toward the stick target (21146 -> 21097) before checkNextMode enters the new procedure, whose init takes
 * current.angle.y = shape_angle.y. At N 4 the entry came on the frame's first step, after that step's share of the turn
 * (21121): the init froze it, the held body turned no further, and the wall-hide turn (shape_angle.y +2048 per step) ran
 * 24 off from there (the sidle's place at 5813: anim 2.26, pos 0.83). Form: a procedure init on an earlier step of a
 * frame whose first step noted the move's whole turn (g_lkT60MoveHeading) puts current.angle.y and shape_angle.y (equal,
 * on the turn's segment) at the whole turn first, as A's body left them. Only for the procedures listed below (an entry
 * the waitturn / MIDENTRY catch-ups redo keeps its first-step angles: those redo the turn, speed and blend only while the
 * angles equal their first-step record; 2026-11-25). NC WWHD_T60_PROCINITTURN_NEGCTRL=1; dt 1 never. */
static inline bool lkT60_procInitTurnNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_PROCINITTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* the procedures whose init takes the whole turn: WHIDE_READY 0x13 (procWHideReady_init: current.angle.y = shape_angle.y) */
static inline bool lkT60_procInitTurnProc(int proc) { return proc == 0x13; }
/* commonProcInit, first: proc = the procedure entered; cur / shape = current.angle.y / shape_angle.y */
template <class A>
static inline void lkT60_procInitTurn(int proc, A& cur, A& shape) {
    if (t60::dt() == 1.0f || t60::tick30() || !lkT60_procInitTurnProc(proc) || lkT60_procInitTurnNc())
        return;
    const s16 c = (s16)cur;
    if ((s16)shape != c)
        return;
    const s16 w = lkT60_headingWhole(c);
    if (w == c)
        return;
    cur = w;
    shape = w;
}
/* posMoveFromFootPos: the heading of this step's move (angle = current.angle.y) */
static inline s16 lkT60_moveHeading(s16 angle) {
    if (t60::dt() == 1.0f || lkT60_headingNegctrl())
        return angle;
    return lkT60_headingWhole(angle);
}

/* (2026-11-25; the oq route  + SWIMTURN at N 2, pair
 * the Octorok route, frame 244: Link swimming in a turn, x / z 0.16 / 0.11 off from then on, Link 72 off by 447) the same for the swim
 * stroke: posMoveFromFootPos's SWIM_MOVE branch moves along current.angle.y, which setSpeedAndAngleSwim turns per step
 * (the free swim: shape_angle.y to the stick target, current.angle.y after it; with attention: current.angle.y to the
 * target); the swim turn notes the frame's whole turn of current.angle.y on the first step (d_a_player_main_07.cpp, the
 * original's one turn from the frame start: the shape's, then the heading's after the whole shape) and the stroke's
 * earlier steps move along it. NC WWHD_T60_SWIMHEADING_NEGCTRL=1: the per-step heading. dt 1: never. */
static inline bool lkT60_swimHeadingNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SWIMHEADING_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* posMoveFromFootPos, SWIM_MOVE: the heading of this step's stroke (angle = current.angle.y) */
static inline s16 lkT60_swimMoveHeading(s16 angle) {
    if (t60::dt() == 1.0f || lkT60_swimHeadingNegctrl())
        return angle;
    return lkT60_headingWhole(angle);
}

/* (2026-11-24; the pig route, frame 136: Link pushing against a wall turns 1643 on the stick's
 * change to diagonal; A's speed 6.813 -> 6.881, B's 6.814 -> 6.755, the walk -> run switch (8.5) then a frame late)
 * setNormalSpeedF's wall term (maxSpeed x (1 - 0.6 cos(current.angle.y + 0x8000 - wall angle))) runs once a frame, on
 * the frame's first step at dt 1 (the speed chase hold, rule (a)), where current.angle.y held only the first
 * step's share of the frame's turn; A reads the frame's turned angle. On the chase's first step the term reads the
 * frame's whole turn (lkT60_headingWhole; the chase step runs at dt 1, so the test is the step's place in the frame,
 * not dt). NC WWHD_T60_WALLSPEED_NEGCTRL=1: current.angle.y, as before. */
static inline bool lkT60_wallSpeedNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_WALLSPEED_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline s16 lkT60_wallSpeedAngle(s16 angle) {
    if (!t60::first_step() || lkT60_wallSpeedNegctrl())
        return angle;
    return lkT60_headingWhole(angle);
}
