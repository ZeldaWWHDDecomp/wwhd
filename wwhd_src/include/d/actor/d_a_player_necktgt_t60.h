/* d_a_player_necktgt_t60.h (WWHD, true 60 build; 2026-11-25). Decomp game build only,
 *. Header only (no new functions or function-local statics in the glued unit
 * d_a_player_main_04.cpp).
 *
 * daPy_lk_c::setNeckAngle turns the neck toward a look point: an actor's eyePos (the lock-on / talk / look target ...).
 * The frame's later steps rebuild the frame's neck move from the frame-start pose (g_lkT60NeckRef, lkT60NeckUseRef), but
 * read the target's eyePos live: a converted actor later in the process line has moved on its earlier steps (the thrown
 * boomerang ~57 units a frame), so the target was half a frame ahead of A's (A reads it before the actor's frame-F run).
 * The boomerang route at N 2: the lock-on frames equal, the neck yaw at the boomerang's first
 * locked frame B 2894 vs A 2987, the hat spring off after it. Form: the frame's first step keeps the actor look point it
 * read (address and value); the frame's later steps read that value while the target is the same address. Points made
 * from Link's own stack locals are not kept (built from the step's values by their own forms). N 1: the live read (the
 * original). NC: WWHD_T60_NECKTGT_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60NeckTgt {
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_NECKTGT_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
static uint32_t s_ea = 0, s_tag = 0xFFFFFFFFu;
static f32 s_p[3];
/* the frame's first step: the look point it read */
static inline void keep(uint32_t tag, uint32_t ea, const cXyz* p) {
    if (s_nc)
        return;
    s_ea = ea, s_tag = tag;
    s_p[0] = (f32)p->x, s_p[1] = (f32)p->y, s_p[2] = (f32)p->z;
}
/* a later step of the same frame looking at the same address: the first step's value (false: live) */
static inline bool use(uint32_t tag, uint32_t ea, cXyz* out) {
    if (s_nc || tag != s_tag || ea != s_ea)
        return false;
    out->x = s_p[0], out->y = s_p[1], out->z = s_p[2];
    return true;
}
} // namespace lkT60NeckTgt
