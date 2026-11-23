/* d_a_player_carry_t60.h (WWHD, true 60 build, 2026-11-19,:
 * visual only, no logic change). Decomp build only. Separate file
 * Link's sources keep thin calls.
 *
 * A converted carrier in the whole form (the whole-frame form rule + addendum, d/t60_whole.h; first case obj_movebox) moves its logic
 * state and collision once a 30 Hz frame, on the tick30 step, and is drawn between its frame ends (port frame interpolation). Link
 * (converted, per step) follows it once a frame: riding through dBgS::MoveBgCrrPos (execute), pushing / pulling through
 * setPushPullKeepData (the box's move since the last call). Logic is exact, but Link's picture makes a 30 Hz
 * staircase on the smooth carrier. Here Link only REPORTS each such carry (the position and Y angle change it gave him on
 * this step) to the port; the port draws his models with the carry spread over the frame like the interpolated world
 * (runtime/src/carry correction). Nothing guest-side is read differently or written: host call only,
 * game build only, N > 1 only. With an unconverted carrier Link is held at 30 Hz and drawn interpolated: the
 * port then sees whole-frame steps only and the correction is zero. */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60_link_carry(unsigned actor, float dx, float dy, float dz, int dangle);
}
#endif

namespace lkT60Carry {
/* before a carry: Link's position and shape angle Y, kept in host memory of this translation unit (one Link; a host
 * struct local would become a guest stack local under autoglue) */
static struct {
    float x, y, z;
    s16 ay;
} s_mark;
static inline void mark(const cXyz& pos, s16 shapeY) {
    s_mark.x = pos.x, s_mark.y = pos.y, s_mark.z = pos.z;
    s_mark.ay = shapeY;
}
/* after it: the change goes to the port (zero changes are not sent) */
static inline void report(u32 actor, const cXyz& pos, s16 shapeY) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() <= 1)
        return;
    const float dx = pos.x - s_mark.x, dy = pos.y - s_mark.y, dz = pos.z - s_mark.z;
    const int da = (s16)(shapeY - s_mark.ay);
    if (dx != 0.0f || dy != 0.0f || dz != 0.0f || da != 0)
        wwhd_t60_link_carry(actor, dx, dy, dz, da);
#else
    (void)actor, (void)pos, (void)shapeY;
#endif
}
}  // namespace lkT60Carry
