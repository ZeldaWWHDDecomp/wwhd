/* d_a_cc_easefe_t60.h (WWHD, true 60 build, 2026-11-25): the ChuChu's speedF ease
 * ends each 30 Hz frame on the original's one ease. Decomp build only.
 * Separate file: d_a_cc_exec.cpp keeps thin calls.
 *
 * cc_t60_ease0F / cc_t60_ease2F (d_a_cc_exec.cpp) run cLib_addCalc0 / cLib_addCalc2 on speedF on every
 * step in c_lib's dt form (ratio 1 - (1 - r)^dt, max step x dt). That form ends a frame on A's value only while the frame
 * stays in one regime (all max-step or all ratio). Exact-dump pair (N 2), ChuChu proc de70 frame 307: the
 * knock-back decay cLib_addCalc0(&speedF, 0.8, 4) from 4: A one step 4 x 0.8 = 3.2 < 4, ratio: 0.8. B step 1 max step
 * (4 x 0.553 = 2.21 > 2): 2, step 2 ratio: 2 x 0.447 = 0.894. speedF stayed 1.118 x A's to rest (x 0.2 a frame), and
 * the ChuChu came to rest 0.109 further (856.63 / A 856.52), held for good.
 * Form: the frame's first step latches the speed it found and the ease's arguments; when every step of the frame ran the
 * same ease on the speed the step before left (no set, no other ease, no skipped step), the tick30 step redoes the ease
 * from the latched start at dt 1 (A's arithmetic, bit-exact). Earlier steps keep the dt form (the in-between speed);
 * cc_t60_ramp_arc then makes the frame's distance the frame-end speed, as before. Host memory keyed by the actor's guest
 * address. dt = 1: nothing. Negative control WWHD_T60_CCEASEFE_NEGCTRL=1: the per-step form. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60ccfe {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CCEASEFE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct Latch {
    u32 frame, lastStep;
    f32 start, lastAfter;
    bool zero;
    f32 target, scale, maxStep;
};
inline std::unordered_map<u32, Latch>& latches() {
    static std::unordered_map<u32, Latch> m;
    return m;
}
/* before the step's ease: true on the tick30 step of a frame whose every step ran this ease (then *start is the frame's
 * start speed); the frame's first step latches */
inline bool redo(u32 key, u32 frame, f32 before, bool zero, f32 target, f32 scale, f32 maxStep, f32* start) {
    if (t60::dt() == 1.0f || nc())
        return false;
    auto& m = latches();
    const u32 step = t60_step_counter();
    if (t60::first_step()) {
        m[key] = Latch{frame, step, before, before, zero, target, scale, maxStep};
        return false;
    }
    auto it = m.find(key);
    if (it == m.end())
        return false;
    const Latch& l = it->second;
    const bool same = l.frame == frame && l.lastStep + 1 == step && l.lastAfter == before && l.zero == zero &&
                      l.target == target && l.scale == scale && l.maxStep == maxStep;
    if (!same || t60::tick30()) {
        const f32 s = l.start;
        m.erase(it);
        if (same)
            *start = s;
        return same;
    }
    return false;
}
/* after the step's ease (earlier steps): the speed it left */
inline void after(u32 key, f32 v) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    auto it = latches().find(key);
    if (it != latches().end()) {
        it->second.lastAfter = v;
        it->second.lastStep = t60_step_counter();
    }
}
} // namespace t60ccfe
