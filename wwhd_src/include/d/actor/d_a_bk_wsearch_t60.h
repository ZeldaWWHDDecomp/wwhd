/* d_a_bk_wsearch_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build only.
 * Separate file: d_a_bk_action.cpp keeps thin calls.
 *
 * wepon_search (BK_ACTION_WEPON_SEARCH) measures the distance and direction to the dropped weapon from the Bokoblin's
 * current place at the call; its run (mode 1, speedF 90) switches to the catch (mode 2) under 150. A's call measures from
 * the frame-start place (before the frame's move). At N steps the later step measured from the first step's moved place:
 * a long route (N 2), Bokoblin 47C5CB0C 7.8 nearer, caught a frame early (A 8645 / B 8644), stopped there and its
 * later path left A's (a different place by 9034, the Co push on Link at 9043, Link 20.4 off). Form: the frame's first
 * call notes the place; the later calls measure from it (distance and turn target, A's values). Host memory keyed by the
 * Bokoblin. dt 1: never. Negative control WWHD_T60_BKWSEARCHPOS_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60bkws {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKWSEARCHPOS_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
struct Entry {
    u32 stamp; /* step counter of the frame's first call */
    f32 x, z;  /* the place at that call */
};
inline std::unordered_map<u32, Entry> s_entries;
/* the place wepon_search measures from: x / z in, the frame-start place out on the later steps */
static inline void place(u32 key, f32* x, f32* z) {
    if (t60::dt() == 1.0f || nc())
        return;
    if (t60::first_step()) {
        Entry& e = s_entries[key];
        e.stamp = t60_step_counter();
        e.x = *x, e.z = *z;
        return;
    }
    auto it = s_entries.find(key);
    if (it == s_entries.end() || t60_step_counter() - it->second.stamp >= (u32)t60::steps())
        return;
    *x = it->second.x, *z = it->second.z;
}
} // namespace t60bkws
