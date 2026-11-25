/* d_a_bk_kdturn_t60.h (WWHD, true 60 build, 2026-11-24, rule (a)): a knock-down
 * on an earlier step catches up the frame's turn. Decomp build only. Part of the
 * Bokoblin's form (d_a_bk_t60.h includes it); c_damagereaction.cpp includes it for the thin call at the knock-down.
 *
 * The hit is read on the frame's first step (the first step reads the tick30 Move: the policy of every damage-reaction
 * enemy). A's one call turns the Bokoblin in fight_attack (cLib_addCalcAngleS2 scale 4, max 0x800 toward
 * dr.mTurnTargetY) before damage_reaction knocks it down. On the first step B's fight_attack turned only its dt share,
 * then the knock-down stopped the turning (mode -100): pair bl, Bokoblin 47C5CE68 frame 164: 853 -> 917 for A's
 * 852 -> 965, kept through the knock-down (48). fight_attack notes the angle before its turn on an earlier step; the
 * knock-down on that step (dr_damage_set, mKnockedDown 0 -> 1) puts it back and turns A's one call (dt 1) toward the
 * frame's target, current and shape angle. Host latches (the port's, keyed by the angle's address: the note and the
 * knock-down sit in two units). dt 1: nothing. Negative control WWHD_T60_BKKDTURN_NEGCTRL=1: the form before. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool bk_t60_kdturn_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKKDTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
static inline u32 bk_t60_kdturn_tag() { return *gabi::at<be<u32>>(0x101FF558); /* the 30 Hz frame tag */ }
static inline u32 bk_t60_kdturn_key(const void* angleY, u32 i) { return (gabi::ea(angleY) ^ 0xC8000000u) + i; }
/* fight_attack's turn on an earlier step: the angle before it (the frame's first such turn only) */
static inline void bk_t60_kdturn_note(const void* angleY, s16 before) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_kdturn_nc())
        return;
    const u32 tag = bk_t60_kdturn_tag();
    if (wwhd_t60_latch_pending(bk_t60_kdturn_key(angleY, 16), tag) != 0)
        return;
    for (u32 i = 0; i < 16; i++)
        wwhd_t60_latch_set(bk_t60_kdturn_key(angleY, i), ((u32)(u16)before >> i) & 1u ? tag : ~tag);
    wwhd_t60_latch_set(bk_t60_kdturn_key(angleY, 16), tag);
}
/* the knock-down on an earlier step (a Bokoblin's damage reaction): A's one call's turn from the noted angle */
static inline void bk_t60_kdturn_catch_up(be<s16>* angleY, be<s16>* shapeY, s16 target) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_kdturn_nc())
        return;
    const u32 tag = bk_t60_kdturn_tag();
    if (wwhd_t60_latch_pending(bk_t60_kdturn_key(angleY, 16), tag) == 0)
        return;
    u32 before = 0;
    for (u32 i = 0; i < 16; i++)
        if (wwhd_t60_latch_pending(bk_t60_kdturn_key(angleY, i), tag) != 0)
            before |= 1u << i;
    wwhd_t60_latch_set(bk_t60_kdturn_key(angleY, 16), ~tag); /* used up */
    *angleY = (s16)(u16)before;
    const f32 previous = t60_set_dt(1.0f);
    cLib_addCalcAngleS2(angleY, target, 4, 0x800);
    t60_set_dt(previous);
    *shapeY = (s16)*angleY;
}

/* (2026-11-25; a long route at N 2, Bokoblin 47C5CE68
 * frame 290): the same catch-up for the flinch. A's one call turns the Bokoblin in fight_attack (10552 -> 10387) before
 * damage_reaction's flinch (react 5: dr action 4, mode 0) stops the attack; on the first step B turned its share
 * (-> 10463) and flinched there, the tick30 step held: the flinch pose's joint targets (dr_damage_anime from the angle)
 * and the head (eyePos) 0.13 - 0.42 off, Link's lock-on shape that reads it 17 - 45 off from 293, Link 2.08 off at 676.
 * The flinch entry on an earlier step now runs bk_t60_kdturn_catch_up (the turn noted by fight_attack this frame, A's
 * one call at dt 1, current and shape angle). Negative control WWHD_T60_BKFLINCHTURN_NEGCTRL=1 (game build). */
static inline bool bk_t60_flinchturn_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKFLINCHTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
static inline void bk_t60_flinchturn_catch_up(be<s16>* angleY, be<s16>* shapeY, s16 target) {
    if (bk_t60_flinchturn_nc())
        return;
    bk_t60_kdturn_catch_up(angleY, shapeY, target);
}

/* addendum (2026-11-25, bkL frame 165, holder 47C5CE68, its stick
 * 47C58678): A's call that knocks the Bokoblin down (dr_damage_set, mKnockedDown 0 -> 1) makes no knocked-down step in
 * that call (case 1: the fall pitch ease toward mFallPitchTarget, the bounce / down checks) - they start next frame. A
 * knock-down on an earlier step left the tick30 step's dr_damage_set in case 1: one ease step (mFallPitch 424, A 0) that
 * dr_matrix_set put into the body, the BUKI hand and the stick thrown from it (yaw 35 off). Form: the knock-down on an
 * earlier step is noted (frame tag, keyed by the dr's address); that frame's tick30 call skips case 1. dt 1: nothing.
 * Negative control WWHD_T60_BKKDHOLD_NEGCTRL=1. */
static inline bool bk_t60_kdhold_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKKDHOLD_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
static inline u32 bk_t60_kdhold_key(const void* dr) { return gabi::ea(dr) ^ 0xC9000000u; }
/* the knock-down set (case 0): noted on an earlier step */
static inline void bk_t60_kdhold_note(const void* dr) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_kdhold_nc())
        return;
    wwhd_t60_latch_set(bk_t60_kdhold_key(dr), bk_t60_kdturn_tag());
}
/* (2026-11-25, a Bokoblin route at N 4, holder 47C5CE68, frame 165): the skip only ran
 * on the tick30 step, so at N 4 the frame's middle steps after the knock-down made knocked-down steps: mFallPitch 0 ->
 * 0xD4 -> 0x1A8 (A 0) on steps 2 / 3, put into the body by dr_matrix_set; the model calc there froze the head on the
 * knock-down (mHeadAngleY A 16333 / B 18474, daBk_player_view_check's cone, from then on) with the joint points and the
 * eye. At N 2 the only later step is the tick30 step (strict). Form: every later step of that frame skips case 1.
 * NC WWHD_T60_BKKDHOLDN4_NEGCTRL=1: the tick30 step only, as before. */
static inline bool bk_t60_kdhold_n4_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKKDHOLDN4_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
/* case 1 on a later step of the frame whose earlier step knocked it down: skip (A's call made no knocked-down step) */
static inline bool bk_t60_kdhold_skip(const void* dr) {
    if (t60::dt() == 1.0f || bk_t60_kdhold_nc() || t60::first_step() || (bk_t60_kdhold_n4_nc() && !t60::tick30()))
        return false;
    return wwhd_t60_latch_pending(bk_t60_kdhold_key(dr), bk_t60_kdturn_tag()) != 0;
}
