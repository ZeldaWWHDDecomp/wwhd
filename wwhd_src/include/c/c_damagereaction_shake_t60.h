/* c_damagereaction_shake_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build only,
 *. Separate file: c_damagereaction.cpp keeps thin calls.
 *
 * dr_damage_set sets the body shake (mShakeAmplitude 2000 .. 9000 on a hit, a bounce, a landing) and, later in the same
 * call, decays it (cLib_addCalc0, 300 a frame, x dt per step). A's one call sets it and decays the whole frame. At N
 * steps a set on a later step of the frame decayed only that step's share: a long route (N 2),
 * Bokoblin 47C5CE68 bounces on the tick30 step of frame 335: 6000 -> 5850 for A's 5700, 150 stronger from then on (the
 * shake term of the joint targets: joints 0 / 1 / 4 / 5 135 off, the eye 0.02 - 0.12 off, Link's lock-on shape 8 off at
 * 337). Form: the frame's first call notes the step counter; every call notes the amplitude it starts with; a call on a
 * later step whose amplitude rose (a set in this call) decays with the frame's share up to and including this step
 * (dt x (k + 1), k the step's index in the frame: the whole frame on the tick30 step), as A's decay after its set.
 * Host memory keyed by the reaction's guest address. dt 1: never. Negative control WWHD_T60_DRSHAKESET_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60drShake {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRSHAKESET_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
struct Entry {
    u32 frame;   /* 30 Hz frame tag of the first-step note */
    u32 stamp;   /* the step counter of the frame's first call */
    f32 amp0;    /* the amplitude this call started with */
};
inline std::unordered_map<u32, Entry> s_entries;

/* dr_damage_set's start */
static inline void begin(u32 key, u32 frame, f32 amp) {
    if (t60::dt() == 1.0f || nc())
        return;
    Entry& e = s_entries[key];
    if (t60::first_step()) {
        e.frame = frame;
        e.stamp = t60_step_counter();
    }
    e.amp0 = amp;
}
/* before the decay: the dt for it (0: the step's own, unchanged) */
static inline f32 decayDt(u32 key, u32 frame, f32 amp) {
    if (t60::dt() == 1.0f || nc() || t60::first_step())
        return 0.0f;
    auto it = s_entries.find(key);
    if (it == s_entries.end() || it->second.frame != frame || !(amp > it->second.amp0))
        return 0.0f;
    const u32 k = t60_step_counter() - it->second.stamp;
    if (k == 0 || k >= (u32)t60::steps())
        return 0.0f;
    return t60::dt() * (f32)(k + 1);
}
} // namespace t60drShake
