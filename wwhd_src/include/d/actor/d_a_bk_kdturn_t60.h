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
