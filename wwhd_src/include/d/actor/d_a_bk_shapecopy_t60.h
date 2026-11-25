/* d_a_bk_shapecopy_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build only.
 * Separate file: d_a_bk_exec.cpp keeps thin calls.
 *
 * daBk_Execute copies shape_angle from current.angle right before damage_reaction; A's call copies once a frame, so its
 * frame-end shape holds the angle the frame had before damage_reaction's own turns (the landing turn toward mLandAngleY,
 * 0x500 a frame,'s bounce hold covers only the bounce frame). At N steps the tick30 step's copy took the
 * angle after the first step's damage_reaction turn: a long route (N 2), Bokoblin 47C5CB0C knocked down and
 * landing: shape_angle.y -17424 for A's -16784 on the frame after the bounce, the frame-end model (dr.mJntPos, eyePos,
 * the colliders) 0.5 - 6 units off, its later Co push on Link at 9043 (Link 20.4 off). Form: the first step notes the
 * turn its damage_reaction made (angle after - angle at the copy); the tick30 step's copy leaves that turn out (the
 * angle at A's copy point; turns made before the copy, Bk_move's, stay in). Host memory keyed by the Bokoblin.
 * dt 1: never. Negative control WWHD_T60_BKSHAPECOPY_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60bksc {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKSHAPECOPY_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
struct Entry {
    u32 stamp; /* step counter of the frame's first step */
    s16 turn;  /* the first step's damage_reaction turn of current.angle.y */
};
inline std::unordered_map<u32, Entry> s_entries;
/* after damage_reaction: on the first step, its turn (pre: current.angle.y at the copy) */
static inline void note(u32 key, s16 pre, s16 post) {
    if (t60::dt() == 1.0f || nc() || !t60::first_step())
        return;
    Entry& e = s_entries[key];
    e.stamp = t60_step_counter();
    e.turn = (s16)((u16)post - (u16)pre);
}
/* the copy's y: on the tick30 step the angle without the first step's damage_reaction turn */
static inline s16 copyY(u32 key, s16 angle) {
    if (t60::dt() == 1.0f || nc() || !t60::tick30() || t60::first_step())
        return angle;
    auto it = s_entries.find(key);
    if (it == s_entries.end() || t60_step_counter() - it->second.stamp >= (u32)t60::steps())
        return angle;
    return (s16)((u16)angle - (u16)it->second.turn);
}
} // namespace t60bksc
