/* d_a_player_cohold_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build
 * only. Separate file: d_a_player_main_05.cpp keeps a
 * thin call.
 *
 * checkJumpRideShip (FALL and the other air procedures) enters SHIP_JUMP_RIDE when Link's cylinder has a Co hit with the
 * ship and old.pos is above the ship's ride point. A's call reads the Co hit the previous frame's collision Move left. At
 * N steps the cylinder's Co hit flag changed between the frame's steps (a collision pass between them): a long route (N 2),
 * Link dropped onto the ship by LINK_WARP: the tick30 step saw a hit the first step did not, from the mid-frame
 * place (old.pos -132.19), and entered the ride a frame before A (A: next frame, old.pos -147.50), Link 72 off. Form: the
 * frame's first call notes the Co hit and its actor; the frame's later calls read the note (the frame-start answer, as
 * A's one call). A call with no note this frame (first reached on a later step) reads live. Host memory, one player.
 * dt 1: never. Negative control WWHD_T60_SHIPRIDECO_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

struct LkT60CoHold {
    uint32_t stamp; /* step counter of the frame's first call */
    uint32_t ac;    /* the Co hit actor (guest address), 0: none */
    bool hit;
    bool valid;
};
inline LkT60CoHold g_lkT60ShipRideCo;
static inline bool lkT60_shipRideCoNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SHIPRIDECO_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* the Co hit as the frame's first call saw it: *hit / *ac from the live values on the first step (noted) or at dt 1 */
static inline void lkT60_shipRideCo(bool liveHit, uint32_t liveAc, bool* hit, uint32_t* ac) {
    *hit = liveHit;
    *ac = liveAc;
    if (t60::dt() == 1.0f || lkT60_shipRideCoNc())
        return;
    LkT60CoHold& h = g_lkT60ShipRideCo;
    if (t60::first_step()) {
        h.stamp = t60_step_counter();
        h.hit = liveHit;
        h.ac = liveAc;
        h.valid = true;
        return;
    }
    if (h.valid && t60_step_counter() - h.stamp < (uint32_t)t60::steps()) {
        *hit = h.hit;
        *ac = h.ac;
    }
}

/* (the same item, part 2): posMove consumes the out power another actor's execute hands Link
 * (mOutPower / mOutPowerAngle, cleared after the add). The ship sets it while Link's cylinder is in Co contact with it,
 * in its execute after Link's: A's Link consumes it on the frame after the contact. At N steps the contact was found by
 * the collision pass between the steps, the ship's first-step execute set the out power, and Link's tick30 posMove took
 * it the same frame: 37.6 in xz (a long route, the tick30 step before the ride), where A's frame has none. Form (as
 * for the Co push): only a frame that started with an out power takes one on its later steps; one
 * first seen on a later step is not taken (the actor hands it again at the frame end, for the next frame's first step).
 * NC WWHD_T60_OUTPOWERLATE_NEGCTRL=1 (the late take). dt 1: never. */
struct LkT60OutPowerNote {
    uint32_t stamp; /* step counter of the frame's first posMove */
    bool had;       /* the first step had an out power */
    bool valid;
};
inline LkT60OutPowerNote g_lkT60OutPower;
static inline bool lkT60_outPowerLateNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_OUTPOWERLATE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* posMove before the out power add: false when this step's out power is to be left out (first seen on a later step) */
static inline bool lkT60_outPowerTake(float k) {
    if (t60::dt() == 1.0f || lkT60_outPowerLateNc())
        return true;
    LkT60OutPowerNote& n = g_lkT60OutPower;
    if (t60::first_step()) {
        n.stamp = t60_step_counter();
        n.had = k != 0.0f;
        n.valid = true;
        return true;
    }
    if (!n.valid || t60_step_counter() - n.stamp >= (uint32_t)t60::steps())
        return true;
    return n.had || k == 0.0f;
}
