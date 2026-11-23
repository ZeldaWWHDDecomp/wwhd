#pragma once
/* d_a_player_footga_t60.h (WWHD, true 60 build, 2026-11-23). Decomp build only.
 * Separate file: posMoveFromFootPos keeps two thin calls.
 *
 * posMoveFromFootPos 023FCB9C scales the foot speed by the slope Link walks on: when the last BG check hit the ground
 * (mAcch GROUND_HIT), the ground poly is safe and the ground code is not 8, speedF = v x cos(getGroundAngle(mAcch.m_gnd,
 * current.angle.y)) (x 0.85 uphill). At 30 Hz the call reads the previous frame's frame-end BG check. At N steps a
 * frame's later step read the check of the frame's earlier step (an in-between position): control arm (Moblin
 * unconverted) f43, a stair-like slope edge: step 1's check stood on the next poly half a frame early (ground angle
 * -2071, A's frame-start -1805), so step 2 moved with speedF 14.015 vs A 14.118, the step-up snap (setStepsOffset's probe
 * one speedF ahead) took y 25.590 vs 25.623, and x / z stayed off (-0.136 / +0.152 to f85, Link resting +0.217 / +0.026
 * from f98: the follow camera's read-back creep and the Moblin spear graze at f196).
 * Form (the process-order read rule frame-start reads, as for the hit bits): on the frame's later steps the slope test and
 * the ground angle read the BG check the frame's first step saw (A's previous frame end): its GROUND_HIT bit, its ground
 * poly (index / bg index / bg / actor) and its ground code; the angle itself is taken with the step's own
 * current.angle.y (the tick30 step's angle is the frame's turned one, as A's call). The real words go back right after
 * the test (nothing in it writes them). A frame whose first step did not come here: the step's own words. dt 1 (N = 1):
 * never (the original). Negative control WWHD_T60_FOOTGA_NEGCTRL=1 (game build): the step's own check. Host memory,
 * one player.
 * Second reader of the same in-between check: mGroundAngle (execute writes it after CrrPos from mAcch.m_gnd). The
 * decision part (execute's BG-hit window 2: setStickData and the procedure dispatch) reads it in setBlendMoveAnime
 * (cos(mGroundAngle) x mNormalSpeed: the walk / run blend m3598, posMoveFromFootPos's v = step x r + ns x (1 - r)),
 * setBlendAtnBackMoveAnime, checkNormalDamage and the WALKSLOPE pick: f43 step 2 took m3598 0.039 (A's f44) vs A 0.030,
 * speedF 14.082 vs 14.118 with the poly read above alone. In the window the frame's later steps see the frame's
 * first-step mGroundAngle (A's frame-start value); after it the real value goes back unless the part wrote its own. */
#include "true60.h"
#include "d/d_bg_s.h"
#include <cstdlib>

struct LkT60FootGa {
    uint32_t stamp; /* step counter of the frame's first step */
    bool valid;
    uint32_t hit;   /* GROUND_HIT bit as the frame's first step read it */
    uint16_t poly, bg;
    uint32_t bgw, actor;
    int32_t code;   /* mGroundCode */
    /* the real words of a later step, put back after the test */
    bool on;
    uint32_t rFlags;
    uint16_t rPoly, rBg;
    uint32_t rBgw, rActor;
    int32_t rCode;
};
static LkT60FootGa s_lkT60FootGa;

static inline bool lkT60_footGaNegCtrl() {
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_FOOTGA_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

/* before the slope test: flags = mAcch.m_flags, gnd = mAcch.m_gnd's poly info, code = mGroundCode */
template <class F, class C>
static inline void lkT60_footGaBegin(F& flags, cBgS_PolyInfo& gnd, C& code) {
    LkT60FootGa& s = s_lkT60FootGa;
    s.on = false;
    if (t60::dt() == 1.0f || t60::steps() <= 1 || lkT60_footGaNegCtrl())
        return;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s.stamp = now;
        s.hit = (uint32_t)flags & dBgS_Acch::GROUND_HIT;
        s.poly = gnd.mPolyIndex;
        s.bg = gnd.mBgIndex;
        s.bgw = gnd.mpBgW;
        s.actor = gnd.mActorId;
        s.code = (int32_t)code;
        s.valid = true;
        return;
    }
    if (!s.valid || now - s.stamp >= t60::steps())
        return;
    s.rFlags = flags;
    s.rPoly = gnd.mPolyIndex;
    s.rBg = gnd.mBgIndex;
    s.rBgw = gnd.mpBgW;
    s.rActor = gnd.mActorId;
    s.rCode = (int32_t)code;
    flags = (s.rFlags & ~(uint32_t)dBgS_Acch::GROUND_HIT) | s.hit;
    gnd.mPolyIndex = s.poly;
    gnd.mBgIndex = s.bg;
    gnd.mpBgW = s.bgw;
    gnd.mActorId = s.actor;
    code = s.code;
    s.on = true;
}

/* after the slope test: the step's real words back */
template <class F, class C>
static inline void lkT60_footGaEnd(F& flags, cBgS_PolyInfo& gnd, C& code) {
    LkT60FootGa& s = s_lkT60FootGa;
    if (!s.on)
        return;
    s.on = false;
    flags = s.rFlags;
    gnd.mPolyIndex = s.rPoly;
    gnd.mBgIndex = s.rBg;
    gnd.mpBgW = s.rBgw;
    gnd.mActorId = s.rActor;
    code = s.rCode;
}

/* mGroundAngle in execute's window 2 (decision part): the frame's first-step value on later steps */
struct LkT60GaView {
    uint32_t stamp;
    bool valid, on;
    int16_t first, real, seen;
};
static LkT60GaView s_lkT60GaView;

template <class A> static inline void lkT60_gaViewBegin(A& ga) {
    LkT60GaView& v = s_lkT60GaView;
    v.on = false;
    if (t60::dt() == 1.0f || t60::steps() <= 1 || lkT60_footGaNegCtrl())
        return;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        v.stamp = now;
        v.first = (int16_t)ga;
        v.valid = true;
        return;
    }
    if (!v.valid || now - v.stamp >= t60::steps())
        return;
    v.real = (int16_t)ga;
    v.seen = v.first;
    ga = v.seen;
    v.on = true;
}

template <class A> static inline void lkT60_gaViewEnd(A& ga) {
    LkT60GaView& v = s_lkT60GaView;
    if (!v.on)
        return;
    v.on = false;
    if ((int16_t)ga == v.seen)
        ga = v.real; /* not written by the part: the step's real value */
}
