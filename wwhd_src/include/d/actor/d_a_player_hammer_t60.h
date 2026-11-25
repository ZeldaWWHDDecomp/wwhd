/* d_a_player_hammer_t60.h (WWHD, true 60 build, 2026-11-23): the Skull
 * Hammer's wobble after a hit (daPy_lk_c::setItemModel, d_a_player_main_04.cpp) on a frame's earlier steps. Decomp build
 * only. Separate file: the actor's source keeps thin calls.
 *
 * The wobble rolls the hammer about Link's hand by sin(t * 0x61A8) * t * 300 with t = mHammerSwingTimer - 1, the timer
 * counted down once per 30 Hz frame (stored on the frame's step only). Every step of a frame showed the
 * frame's end value (t), so the wobble (0x61A8 = 137 degrees a frame) moved in 30 Hz stairs at N > 1. The picture: the
 * frame's earlier step k of N shows the countdown at its own time, t_k = timer - k / N (timer: the stored value, the
 * previous frame's end), phase timer * 0x61A8 - (k / N) * 0x61A8 and amplitude t_k * 300; on the tick30 step (k = N) that
 * is the frame's end value, the original (frame-end identity). Picture only: the timer is not touched; the angle feeds
 * only the hammer model's base matrix. dt 1 (N 1): nothing. Negative control WWHD_T60_HAMMERPIC_NEGCTRL=1: the form
 * before (the frame's value on every step). */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkhammer {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_HAMMERPIC_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* `timer`: the stored mHammerSwingTimer (not yet counted down on an earlier step); `ang` / `amp`: the frame-end values
 * the caller computed, replaced on an earlier step by the step's own */
static inline void picture(s16 timer, u32& ang, f32& amp) {
    if (t60::dt() == 1.0f || t60::tick30() || nc() || timer <= 0)
        return;
    const f32 k = (f32)t60::step() / (f32)t60::steps(); /* k / N of the frame reached at this step */
    ang = (u16)((s32)timer * 0x61A8 - (s32)(k * (f32)0x61A8));
    amp = ((f32)timer - k) * 300.0f;
}
}  // namespace lkhammer
