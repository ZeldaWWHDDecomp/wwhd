/* d_a_player_landhist_t60.h (WWHD, true 60 build; 2026-11-23,
 * rule: not the per-step ground-check rule, A's frame segment for the decision). Decomp build only.
 * Separate file: d_a_player_main_04.cpp keeps a thin call. Next to
 * d_a_player_stepdown_t60.h (the step-down snap) and d_a_player_landline_t60.h (the landing frame's segment), whose
 * frame-start record (lkT60Land::s_rec: the frame-start position and A's frame fall) it reads.
 *
 * Direct ground hit vs step-down snap on the frame's check. Link clears GROUND_HIT before every CrrPos (execute), so
 * Acch's GroundCheckInit always keeps mPrevGroundHit 0 for him and GROUND_LANDING marks "GroundCheck itself found the
 * ground at or above the moved position" (GROUND_FIND | GROUND_HIT | GROUND_LANDING), against "no direct hit, the ground
 * within 30.1 below: autoGroundHit's snap" (GROUND_HIT only). A checks once a frame from the frame-start position moved
 * by the whole frame. At N steps the tick30 step checks from the earlier step's state: walking down the Windfall stairs
 * (fw, frame 383/384) B's step 0 stood on a tread under its half-step position (y 521.13, above both
 * frame-end heights), the tick30 step's moved position then lay at the next tread's height and the check hit it
 * directly (flags 0x28E0) where A's check (from y 533.17 + the frame's fall) found it below and snapped (0x2820); same
 * frame-end position, poly and height. The landing bits make the next frame's walk take the stair a frame early (speedF
 * 15.93 / anim rate 1.955 vs A 17 / 2.3 at 384, A one frame later); xz 1.4 kept, the stop off by 1.12 speed.
 * Form (flags only): on the tick30 step of a frame that started on the ground (the previous frame-end check hit) and
 * whose check was not already given A's segment (landline), the direct-hit decision is taken as A's: A's moved y = the
 * frame-start y + A's frame fall (lkT60Land's record); A's check hits directly when the found ground (m_ground_h +
 * m_ground_up_h) is above it. B direct / A not (A's ground within the snap's 30.1): GROUND_FIND | GROUND_LANDING off
 * (GROUND_HIT stays, the snap's result); B snap / A direct: both on. Position, speed and the picture are untouched (both
 * paths put Link on the same ground with speed.y 0). dt 1 (N = 1): never. Negative control WWHD_T60_LANDHIST_NEGCTRL=1
 * (game build). Host memory, one player. */
#pragma once
#include "true60.h"
#include "d/d_bg_s.h"
#include "d/actor/d_a_player_landline_t60.h"
#include <cstdlib>
#include <cstring>

namespace lkT60LandHist {
static inline bool negctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_LANDHIST_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
struct St {
    uint32_t tag;   /* the frame the first-step note is of */
    bool grounded;  /* the previous frame-end check hit the ground (the first step's acf) */
    bool segment;   /* the tick30 check had lkT60Land's A segment */
};
static St s_st;

/* before mAcch.CrrPos, every step (acf: the flags before ClrGroundHit; segment: lkT60Land::begin's result) */
static inline void note(uint32_t acf, bool segment, uint32_t tag) {
    if (t60::dt() == 1.0f || t60::steps() <= 1)
        return;
    if (t60::first_step()) {
        s_st.tag = tag;
        s_st.grounded = (acf & (uint32_t)dBgS_Acch::GROUND_HIT) != 0;
        s_st.segment = false;
    }
    if (t60::tick30() && s_st.tag == tag)
        s_st.segment = segment;
}

/* the tick30 step, after autoGroundHit and the lift-off block: the frame-end flags' direct-hit bits as A's check */
static inline uint32_t apply(uint32_t flags, f32 groundH, f32 upH, bool fly, uint32_t tag) {
    if (t60::dt() == 1.0f || t60::steps() <= 1 || !t60::tick30() || t60::first_step() || negctrl() || fly)
        return flags;
    const lkT60Land::Rec& r = lkT60Land::s_rec;
    if (s_st.tag != tag || !s_st.grounded || s_st.segment || !r.valid || r.tag != tag)
        return flags;
    if (!(flags & (uint32_t)dBgS_Acch::GROUND_HIT) || groundH == -1000000000.0f)
        return flags;
    f32 p0y;
    std::memcpy(&p0y, &r.p0[1], 4);
    const f32 aY = p0y + r.sy; /* A's moved y (A's add, as lkT60Land's segment) */
    const f32 h = gabi::fadds_ppc(groundH, upH);
    const bool aDirect = h > aY;
    const uint32_t edge = (uint32_t)(dBgS_Acch::GROUND_FIND | dBgS_Acch::GROUND_LANDING);
    const bool bDirect = (flags & (uint32_t)dBgS_Acch::GROUND_FIND) != 0;
    if (bDirect && !aDirect && !(groundH - aY < -30.1f)) /* A: no direct hit, the snap takes the ground */
        return flags & ~edge;
    if (!bDirect && aDirect)
        return flags | edge;
    return flags;
}
} // namespace lkT60LandHist

/* mGroundCode in execute's window 2 (the decision part), next to d_a_player_footga_t60.h's mGroundAngle view (-
 * FREEWAIT-STOP, rule: chase the stair speed drop with the frame-start form). setBlendMoveAnime reads both:
 * f30 = |cos(mGroundAngle) x mNormalSpeed| / max, with code 8 taking cos = 1. fw route  frame 384: the window
 * showed the frame-start mGroundAngle 5054 (A's) but the step's own mGroundCode 0, which B's step-0 check had read
 * on the next tread (poly 3060), where A's frame-start code is 8: B's tick30 body took cos(5054) (f30 0.885) and set the
 * walk/run blend m3598 0.2297 (A 0), so posMoveFromFootPos moved on the feet: speedF 15.93 / anim rate 1.955 vs A's
 * 17 / 2.3, the stair a frame early, xz 1.4 kept, the stop off by 1.12. Form: the frame's later steps see the frame's
 * first-step mGroundCode in the window (A's frame-start value, as the angle); after it the real value goes back unless
 * the part wrote its own. dt 1 (N = 1): never. Negative control WWHD_T60_GCVIEW_NEGCTRL=1 (game build). */
namespace lkT60GcView {
static inline bool negctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_GCVIEW_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
struct V {
    uint32_t stamp;
    bool valid, on;
    int32_t first, real, seen;
};
static V s_v;
template <class C> static inline void begin(C& code) {
    V& v = s_v;
    v.on = false;
    if (t60::dt() == 1.0f || t60::steps() <= 1 || negctrl())
        return;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        v.stamp = now;
        v.first = (int32_t)code;
        v.valid = true;
        return;
    }
    if (!v.valid || now - v.stamp >= (uint32_t)t60::steps())
        return;
    v.real = (int32_t)code;
    v.seen = v.first;
    code = v.seen;
    v.on = true;
}
template <class C> static inline void end(C& code) {
    V& v = s_v;
    if (!v.on)
        return;
    v.on = false;
    if ((int32_t)code == v.seen)
        code = v.real; /* not written by the part: the step's real value */
}
} // namespace lkT60GcView
