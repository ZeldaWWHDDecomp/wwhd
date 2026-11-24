/* d_attention_t60.h (WWHD, true 60 build; 2026-11-23): the Z-target arrow drawn at its
 * target's world time. Decomp build only. Separate file:
 * d_attention_3.cpp keeps thin calls.
 *
 * dAttention_c's draw places the lock-on arrow (dAttDraw_c, a 4-joint model drawn by the play scene) at the target's
 * attention position (actor +0x390), which a converted target writes on every step. The target's models are drawn at
 * the world time W (one step back at N 2); the arrow sat on the newest step and the play scene's model was then
 * interpolated between its record pass and this pass: it bobbed against the target (a sweep of the drawn models on a Bokoblin
 * lock-on route: ~8-11 units on sub-step passes, ~2.8 on tick30 passes, the Bokoblin an even 2.7). Around each placement the target's
 * position goes to the port (runtime/src/t60/t60_draw.cpp follow_begin): its value at W in place, and the arrow drawn as
 * placed. The arrow's own animation frame advances once a 30 Hz frame on the frame's first step (the play scene runs
 * first): the port draws it between frames at W as well. An unconverted target is left to the interpolation (as before). Host call only; the position is put back
 * before the draw returns. Game build, N > 1 only. Port switch WWHD_T60_ATTN_FOLLOW=0: off. */
#pragma once
#include "true60.h"
#include "d/d_attention.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60_draw_follow_begin(unsigned owner, unsigned ea, unsigned n, unsigned anim);
__attribute__((visibility("default"))) void wwhd_t60_draw_follow_end();
}
#endif

namespace t60att {
enum { kAttnPos = 0x390, kMorfFrame = 0x9C }; /* fopAc_ac_c attention position; the arrow McaMorf's frame (mFrame) */
/* target: the locked actor; draw: the dAttDraw_c (its McaMorf at +0: the arrow's animation, whose frame advances once a
 * 30 Hz frame on the frame's first step; the port draws it between frames at W too) */
static inline void followBegin(u32 target, u32 draw) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() > 1) {
        const u32 morf = (u32)AttentionHD::view<AttentionHD::Draw>(draw)->anm;
        wwhd_t60_draw_follow_begin(target, target + kAttnPos, 3, morf ? morf + kMorfFrame : 0);
    }
#else
    (void)target, (void)draw;
#endif
}
static inline void followEnd() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() > 1)
        wwhd_t60_draw_follow_end();
#endif
}
}  // namespace t60att
