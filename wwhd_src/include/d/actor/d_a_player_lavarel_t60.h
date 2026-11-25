/* d_a_player_lavarel_t60.h (WWHD, true 60 build, 2026-11-24,
 * from the potm route): Link falls into lava carrying a pot. Decomp build only.
 * Separate file: the sources keep thin calls.
 *
 * A: his frame's lava check (checkLavaFace on the frame-end place, the lava surface m35D4 above his y; main_04) starts
 * the lava damage and lets the pot go before setGrabItemPos places it, so A's pot stays at its frame-start place for that
 * frame (route potm frame 3750: -572.74; the tick30 release puts the frame-start placement back). At N 2 his earlier
 * step is still above the lava (y -765.27 against -800) and setGrabItemPos places the pot at his hands: the picture
 * showed it at -610.60, 38 units below the frame, then back up.
 *
 * Form (picture only): setGrabItemPos on a frame's earlier step (N > 1, not tick30) keeps the carried actor's place and
 * shape angle as they were before it when his place plus his fall over the frame's remaining steps (speed.y, gravity,
 * the fall-speed cap; 1/N each) ends below the lava surface his lava ground check found on this step, that surface is
 * lava (attribute 6) and he does not swim (his lava check's inputs). Link's own words (the wear ease, m35D8) are
 * untouched. The tick30 step runs as before (dt 1: never). NC WWHD_T60_LINK_LAVAREL_NEGCTRL=1; debug
 * WWHD_T60_LINK_LAVAREL_LOG=1 prints each firing ([t60] lavarel step y vy end lava) to stderr. */
#pragma once
#include "true60.h"
#include <cstdio>
#include <cstdlib>

namespace lkT60LavaRel {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_LINK_LAVAREL_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
static inline bool logOn() {
    static const bool v = [] { const char* e = getenv("WWHD_T60_LINK_LAVAREL_LOG"); return e && e[0] == '1'; }();
    return v;
}
/* Link (player ea) ends this frame in the lava: the predicate above (called after his move and lava check of this step) */
template <class Bgs> static inline bool endsInLava(u32 player, Bgs* bgs) {
    const u32 n = t60::steps();
    if (n <= 1 || t60::tick30() || t60::step() >= n)
        return false;
    const f32 lava = (f32)*gabi::at<be<f32>>(player + 0x6A2C); /* m35D4: the lava surface under him (-1e9: none) */
    if (!(lava > -1.0e8f))
        return false;
    const f32 y = (f32)*gabi::at<be<f32>>(player + 0x318);    /* current.pos.y */
    const f32 vy = (f32)*gabi::at<be<f32>>(player + 0x340);   /* speed.y */
    const f32 g = (f32)*gabi::at<be<f32>>(player + 0x374);    /* gravity */
    const f32 vmax = (f32)*gabi::at<be<f32>>(player + 0x378); /* maxFallSpeed */
    const f32 dt = 1.0f / (f32)n;
    f32 yEnd = y, v = vy;
    for (u32 i = t60::step(); i < n; i++) { /* the frame's remaining steps after this one */
        v = v + g * dt;
        if (v < vmax)
            v = vmax;
        yEnd = yEnd + v * dt;
    }
    if (!(yEnd < lava))
        return false;
    if (*gabi::at<be<u32>>(player + 0x6A70) & 0x40000) /* mModeFlg & ModeFlg_SWIM */
        return false;
    /* dBgS::GetAttributeCode on mLavaGndChk's poly: 6 dBgS_Attr_LAVA_e (water or other special ground: no damage) */
    if (gabi::call<s32>(0x024EF0F4, bgs, player + 0xC8C + 0x14) != 6)
        return false;
    if (logOn())
        fprintf(stderr, "[t60] lavarel %u y %.2f vy %.2f end %.2f lava %.2f\n", t60_step_counter(), y, vy, yEnd, lava);
    return true;
}
/* setGrabItemPos's scope (after its null check): the carried actor's place / shape angle back on scope exit when the
 * predicate fires */
template <class Lk, class Ac, class Bgs> struct Keep {
    Lk* lk;
    Ac* ac;
    Bgs* bgs;
    f32 p[3];
    s16 r[3];
    bool on;
    Keep(Lk* l, Ac* a, Bgs* b) : lk(l), ac(a), bgs(b), on(false) {
        if (nc() || t60::steps() <= 1 || t60::tick30())
            return;
        on = true;
        p[0] = ac->current.pos.x, p[1] = ac->current.pos.y, p[2] = ac->current.pos.z;
        r[0] = ac->shape_angle.x, r[1] = ac->shape_angle.y, r[2] = ac->shape_angle.z;
    }
    ~Keep() {
        if (!on || !endsInLava(gabi::ea(lk), bgs))
            return;
        ac->current.pos.x = p[0], ac->current.pos.y = p[1], ac->current.pos.z = p[2];
        ac->shape_angle.x = r[0], ac->shape_angle.y = r[1], ac->shape_angle.z = r[2];
    }
};
}  // namespace lkT60LavaRel
