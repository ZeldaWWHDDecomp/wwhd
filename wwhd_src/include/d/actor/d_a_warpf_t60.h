/* d_a_warpf_t60.h (WWHD, true 60 build, 2026-11-22): d_a_warpf.cpp on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 *the actor's source keeps thin calls.
 *
 * The boss blue warp (Warpf): its frame decides from switches / event state / Link and plays its animations and effects. Whole
 * form (the whole-frame form rule + addendum, d/t60_objw.h): the execute runs once per 30 Hz frame on the tick30 step at dt 1 from
 * the frame-start state; the earlier steps change no logic state and are picture steps (frame interpolation: the model drawn
 * between its frame ends). dt 1: the original. Negative control: T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"
#include "f_pc/f_pc_t60_frame_start.h"
#include <stdlib.h>

/* (the process-order read rule): the warp executes before Link in the process line, so at
 * 30 Hz its frame reads Link's previous-frame-end position. Whole form, its tick30 step read Link after his first step
 * of the frame (half a frame on): pair (SirenB, radius 200) check_warp_event found him in range at frame 172
 * (-193.6) where A's frame read -202.1 and ordered at 173: DEMO_BOSS_WARP one frame early. Link's frame-start position,
 * as A reads it (the live one when Link is not converted, has executed on this step already, or N = 1). Negative control
 * WWHD_T60_WARPF_FS_NEGCTRL=1 (game build). */
namespace warpf60 {
static inline void playerPos(fopAc_ac_c* player, cXyz* out) {
    out->copy(player->current.pos);
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_WARPF_FS_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl)
        return;
#endif
    t60_fs_read(gabi::ea(player), 1, out);
}
}  // namespace warpf60
