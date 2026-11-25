/* d_a_player_freezeanm_t60.h (WWHD, true 60 build; 2026-11-25: a long route at
 * N 2, frame 2712). Decomp build only. Separate file:
 *d_a_player_main_04.cpp keeps a thin call.
 *
 * daPy_lk_c::execute plays Link's animations (animeUpdate) unless his freeze flags are set (mNoResetFlg1: FREEZE_STATE
 * 0x800 or 0x40000000). A's one call reads them as the previous frame left them: a flag set later in the frame (the
 * drowning's freeze, SWIM_WAIT at 2712) stops the plays from the NEXT frame on, after this frame's whole play
 * (swim wait 24.870 + 2.997 -> wrap 1.867). At N steps a flag set on the first step stopped the tick30 step's play: B
 * froze after its first step's share (wrap 0.369), animation 1.498 off from then on. Form: the gate reads the frame-start
 * bits of those two flags on the frame's later steps (the frame's first step notes them); the other bits and every write
 * stay the step's own. dt 1: the original. NC WWHD_T60_ANMFREEZE_NEGCTRL=1. Host memory, one player. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60FreezeAnm {
static const uint32_t kMask = 0x800u | 0x40000000u; /* daPyFlg1_FREEZE_STATE | daPyFlg1_UNK40000000 */
inline uint32_t s_tag = 0xFFFFFFFFu, s_bits = 0;
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_ANMFREEZE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* the flags the animation gate reads (f1: mNoResetFlg1 now; tag: the 30 Hz frame tag) */
static inline uint32_t view(uint32_t f1, uint32_t tag) {
    if (t60::dt() == 1.0f || nc())
        return f1;
    if (t60::first_step()) {
        s_tag = tag;
        s_bits = f1 & kMask;
        return f1;
    }
    if (s_tag != tag)
        return f1;
    return (f1 & ~kMask) | s_bits;
}
}  // namespace lkT60FreezeAnm
