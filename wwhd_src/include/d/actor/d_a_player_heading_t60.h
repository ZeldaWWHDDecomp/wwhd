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
/* posMoveFromFootPos: the heading of this step's move (angle = current.angle.y) */
static inline s16 lkT60_moveHeading(s16 angle) {
    if (t60::dt() == 1.0f || lkT60_headingNegctrl())
        return angle;
    return lkT60_headingWhole(angle);
}

/* (2026-11-24; a pig pair's frame 136: Link pushing against a wall turns 1643 on the stick's
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
