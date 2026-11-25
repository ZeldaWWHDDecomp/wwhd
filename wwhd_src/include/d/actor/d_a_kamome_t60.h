/* d_a_kamome_t60.h (WWHD, true 60 build, 2026-11-24): the seagull's read of
 * Link's move. Decomp build only. Separate file.
 *
 * kamome_heisou_move (the seagull flying beside Link's boat) takes Link's speed as diff_abs(player->old.pos,
 * player->current.pos): in A, executed after Link, Link's whole frame move; the branch 'boat fast' (> 30, the master gull
 * leads: height target 0) or not (height target home.pos.y, speed 20) follows from it. The seagull is whole form
 * (d/t60_whole.h: the frame on the tick30 step), where player->old.pos is Link's place before his tick30 step only: half
 * the frame's move at N 2 (stair sweep, sail route: A 50.06 / B 25 after the 1.2 scale -> B took the slow branch, its target
 * height eased the other way and the bird flew ~660 units higher, changing its sing decisions: the extra 0x4805 cries).
 * Here the frame's first step notes Link's place before his first step (player->old.pos then: the frame start), and the
 * tick30 run measures from it: A's old -> current. N 1: the same words as the original. NC WWHD_T60_KAMOME_LINKMOVE_NEGCTRL=1. */
#pragma once
#include "d/t60_whole.h"
#include <cstdlib>

namespace kamome_t60 {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_KAMOME_LINKMOVE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the frame start, host words (no guest object here: the .cpp's local cXyz takes it) */
struct LinkStart {
    u32 tag;
    f32 x, y, z;
};
static LinkStart s_linkStart = {0xFFFFFFFFu, 0.0f, 0.0f, 0.0f};
/* daKamome_Execute's head on every step (before the whole form's skip): the frame's first step notes the frame start */
static inline void note(fopAc_ac_c* player) {
    if (player == nullptr || !t60::first_step())
        return;
    s_linkStart.tag = t60w::frameTag();
    s_linkStart.x = player->old.pos.x;
    s_linkStart.y = player->old.pos.y;
    s_linkStart.z = player->old.pos.z;
}
/* the place Link's move is measured from, into *out: the frame start when noted this frame, else old.pos (the original) */
static inline void moveFrom(fopAc_ac_c* player, cXyz* out) {
    if (nc() || s_linkStart.tag != t60w::frameTag()) {
        out->x = player->old.pos.x;
        out->y = player->old.pos.y;
        out->z = player->old.pos.z;
        return;
    }
    out->x = s_linkStart.x;
    out->y = s_linkStart.y;
    out->z = s_linkStart.z;
}
}  // namespace kamome_t60
