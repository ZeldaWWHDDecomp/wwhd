/* d_a_player_posadd_t60.h (WWHD, true 60 build, 2026-11-20, ym83g2
 * irol pair). Decomp build only. Separate file.
 *
 * posMoveFromFootPos ends with Link's own move: A adds the frame's speed to current.pos once (one f32 rounding at the
 * world coordinate's ulp: 1/64 at |x| ~ 1.9e5); N steps added speed x dt N times, each rounded (ym83g2 A 3616: A x
 * -3.015625 = one add of -3.0110, B -1.5 -1.5 = -3.000; the follow camera's centre and yaw follow, Link's s16 stick
 * target crosses a quantum the next frame).
 * Form: a compensated add. A host residual r (f64 per axis) holds what the steps' roundings
 * left out of the frame's own move: each step adds (f64)pos + move x dt + r, rounds once to f32 and keeps the rest in
 * r. With the frame's speed constant over its steps (the whole chord) move x dt is an exact
 * fraction of A's f32 move (x 0.5 / 0.25 are exact), so the tick30 step's pos = round(frame-start pos + A's move), A's
 * add, bit for bit. The per-step collision / ground corrections between the adds stay per step (the per-step ground-check rule) and enter
 * the next add's base as they are.
 * Co push (pw85P Poe pair: Link's knockback pushed by the Poe's body split by f32
 * ulps and later hit angles amplified it): posMove adds the collision push m_cc_move (held from the tick30 Move, dt of
 * it per step: lk_ccMoveStep) right after this own add; A rounds twice a frame, X = round(P + V), then round(X + C).
 * The form keeps two host shadows instead of one residual: S, the exact own-move position (frame-start pos + the steps'
 * moves + the per-step corrections found between the writes), and Cacc, the push handed out so far (exact: C x 0.5 /
 * 0.25). A step's own add writes round(S + Cacc); its push add writes round(S + Cacc) after adding its part, and on the
 * tick30 step round((f32)round(S) + Cacc) = A's two roundings. Without a push (Cacc 0) S + Cacc = the old residual
 * form (S = pos + r): the same numbers as before.
 * Lifetime: host memory only, never saved (no state file, no guest memory); reset on every frame's first step and on
 * any call outside the per-step form (dt 1: the 30 Hz path, a held / whole tick30 step, events), dropped after the
 * tick30 step's add (nothing carries into the next frame, as in A). Link only (one player). */
#pragma once
#include "true60.h"
#include <cstdio>
#include <cstdlib>

struct LkT60PosAdd {
    double S[3];    /* the exact own-move position (corrections included) */
    double P0[3];   /* (speed-ramp position-gap (3)) the frame-start place */
    double K[3];    /* (speed-ramp position-gap (3)) the frame's per-step corrections so far */
    double C[3];    /* the frame's push handed out so far */
    f32 W[3];       /* the coordinates this form last wrote (a difference at the next call = a per-step correction) */
    uint32_t stamp; /* step counter of the frame's first step */
    bool valid;
};
static LkT60PosAdd s_lkT60PosAdd;
/* (speed-ramp position-gap (3) K=0 probe) set on a tick30 step whose place left out earlier-step xz corrections (a wall / ground push of this
 * frame): the step's CrrPos then checks A's segment from the frame start (d_a_player_landline_t60.h, main_04; one
 * instance across the player units) */
inline bool g_lkT60WallSeg = false;
/* the xz moves of this frame's earlier steps' BG checks (mAcch.CrrPos: wall pushes), noted around the call in main_04
 * (d_a_player_landline_t60.h); reset on the frame's first step */
inline double g_lkT60BgK[2] = {0.0, 0.0};

/* dt 1 (the original's add, held / whole steps included): the shadows are dropped */
static inline void lkT60_posAddReset() {
    s_lkT60PosAdd.valid = false;
}

static inline bool lkT60_posAddLive() {
    const LkT60PosAdd &s = s_lkT60PosAdd;
    return s.valid && t60_step_counter() - s.stamp < t60::steps();
}

/* (carriables helper 2026-11-24; user approval, trial: the speed-ramp position-gap rule (3) applied to Link's own xz move): the
 * frame's tick30 step places Link at the frame-start place + the frame-end speed's whole move (A's one add of the speed
 * the frame ends with) + the frame's per-step corrections (the per-step ground-check rule), instead of the sum of the steps' own moves; the
 * earlier steps move as before (their own speed x dt, the picture). Exact on A's frame end while the corrections agree
 * (pig route frame 91: B's two half-step moves of a speed ramp summed 2 ulp off A at z 319256). The picture takes the
 * difference on the tick30 pass (the speed-ramp position-gap gap, ramps only). y keeps its exact-arc form. NC WWHD_T60_LKMOVE6C3_NEGCTRL=1;
 * WWHD_T60_LKMOVE6C3_LOG=1 prints each tick30 correction ([t60] lkmove6c3 step dx dz). Game build only; dt 1 never. */
static inline bool lkT60_move6c3Off() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_LKMOVE6C3_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* The tick30 place leaves the earlier steps' BG-check xz moves (wall pushes) out: frame start + the other per-step
 * corrections + A's one move; the tick30 step's own check corrects from A's input (pig route: the earlier step's wall
 * push carried into the frame end seeded a speed / turn divergence). Other corrections between the adds (root motion,
 * sets) stay. WWHD_T60_LKMOVE6C3_K=1 keeps the wall pushes too. */
static inline bool lkT60_move6c3K() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_LKMOVE6C3_K"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
static inline bool lkT60_move6c3Log() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_LKMOVE6C3_LOG"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* dt != 1: v = this step's own move (move x dt, plus y's arc term), vf = the step's speed as a whole frame's move (the
 * move A adds once with the frame-end speed); returns the new coordinates in out */
static inline void lkT60_posAdd(const f32 p[3], const double v[3], const double vf[3], f32 out[3]) {
    LkT60PosAdd &s = s_lkT60PosAdd;
    if (t60::first_step() || !lkT60_posAddLive()) {
        for (int i = 0; i < 3; i++) {
            s.S[i] = (double)p[i];
            s.C[i] = 0.0;
            s.P0[i] = (double)p[i];
            s.K[i] = 0.0;
        }
        s.stamp = t60_step_counter();
        s.valid = true;
        g_lkT60BgK[0] = g_lkT60BgK[1] = 0.0;
    } else {
        for (int i = 0; i < 3; i++) {
            const double corr = (double)p[i] - (double)s.W[i]; /* the corrections since the last write (the per-step ground-check rule) */
            s.S[i] += corr;
            s.K[i] += corr;
        }
    }
    const bool m6c3 = t60::tick30() && !lkT60_move6c3Off();
    double dl[3] = {0.0, 0.0, 0.0};
    for (int i = 0; i < 3; i++) {
        if (m6c3 && i != 1) { /* (speed-ramp position-gap (3)) xz: the frame start + A's one move + the corrections */
            const double s6 = s.P0[i] + s.K[i] - (lkT60_move6c3K() ? 0.0 : g_lkT60BgK[i == 0 ? 0 : 1]) + vf[i];
            dl[i] = s6 - (s.S[i] + v[i]);
            s.S[i] = s6;
        } else {
            s.S[i] += v[i];
        }
        out[i] = (f32)(s.S[i] + s.C[i]);
        s.W[i] = out[i];
    }
    if (m6c3 && !lkT60_move6c3K() && (g_lkT60BgK[0] != 0.0 || g_lkT60BgK[1] != 0.0))
        g_lkT60WallSeg = true;
    if (m6c3 && lkT60_move6c3Log() && (dl[0] != 0.0 || dl[2] != 0.0))
        fprintf(stderr, "[t60] lkmove6c3 %u %.6f %.6f\n", t60_step_counter(), dl[0], dl[2]);
}

/* posMove's push add, dt != 1: c = this step's part of the frame's push (lk_ccMoveStep); p = the coordinates now.
 * Returns false (the caller adds as before) when no own add of this frame came first. */
static inline bool lkT60_posAddCc(const f32 p[3], const f32 c[3], f32 out[3]) {
    LkT60PosAdd &s = s_lkT60PosAdd;
    if (!lkT60_posAddLive())
        return false;
    for (int i = 0; i < 3; i++) {
        s.S[i] += (double)p[i] - (double)s.W[i];
        s.C[i] += (double)c[i];
        out[i] = t60::tick30() ? (f32)((double)(f32)s.S[i] + s.C[i]) : (f32)(s.S[i] + s.C[i]);
        s.W[i] = out[i];
    }
    if (t60::tick30())
        lkT60_posAddReset();
    return true;
}
