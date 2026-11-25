/* m_Do_ext_morf_curfe_t60.h (WWHD, true 60 build, t60-pgp 2026-11-25): mDoExt_McaMorf's
 * morf value ends each 30 Hz frame on the original's one chase. Decomp build only.
 * Separate file: m_Do_ext_morf.cpp keeps thin calls.
 *
 * mDoExt_McaMorf::play chases mCurMorf toward 1 by mMorfStep (cLib_chaseF; c_lib's dt form: mMorfStep x dt per step).
 * N steps of mMorfStep / N do not add up to A's one mMorfStep in single precision (at N 4 the quarter steps round), so
 * the tick30 calc's blend (m_Do_ext_morf_t60.h: the frame-start pose blended with (cur - prev0) / (1 - prev0)) used a
 * cur one ulp off A's while a morf runs: the joints of an animation set with a morf (a translation channel that differs
 * between the two animations: the root's y) one ulp off. long2 at N 4 (board the l2an4d):
 * the Bokoblins' first frames in Cave09 room 3 after the t222 load (A seq 10366 / B 10359): eyePos.y, the attention y,
 * the reaction joints' y, mSpitPos.y one ulp; 0.16 in eye x by 6983 after the look chains, and Link's lock-on turn
 * made long2 segment 15. Form: the frame's first play latches the morf value it found and the step; when every step of
 * the frame played on (the value the step before left, the same step), the tick30 play redoes the chase from the latch
 * at dt 1 (A's arithmetic). Earlier steps keep the dt form (the in-between pose). Host memory keyed by the morf's guest
 * address. dt = 1: nothing. Negative control WWHD_T60_MORFCUR_NEGCTRL=1: the per-step form. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60morfcur {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MORFCUR_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct Latch {
    u32 frame, lastStep;
    f32 start, lastAfter, step;
};
inline std::unordered_map<u32, Latch>& latches() {
    static std::unordered_map<u32, Latch> m;
    return m;
}
inline u32 frameTag() { return gabi::load<u32>(0x101FF558); /* g_Counter.mCounter0 */ }
/* before the play's chase: true on the tick30 step of a frame whose every step played on (*start: the frame's start) */
inline bool redo(u32 morf, f32 before, f32 step, f32* start) {
    if (t60::dt() == 1.0f || nc())
        return false;
    auto& m = latches();
    const u32 now = t60_step_counter(), frame = frameTag();
    if (t60::first_step()) {
        m[morf] = Latch{frame, now, before, before, step};
        return false;
    }
    auto it = m.find(morf);
    if (it == m.end())
        return false;
    const Latch& l = it->second;
    const bool same = l.frame == frame && l.lastStep + 1 == now && l.lastAfter == before && l.step == step;
    if (!same || t60::tick30()) {
        const f32 s = l.start;
        m.erase(it);
        if (same)
            *start = s;
        return same;
    }
    return false;
}
/* after the chase (earlier steps): the value it left */
inline void after(u32 morf, f32 v) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    auto it = latches().find(morf);
    if (it != latches().end()) {
        it->second.lastAfter = v;
        it->second.lastStep = t60_step_counter();
    }
}
} // namespace t60morfcur
