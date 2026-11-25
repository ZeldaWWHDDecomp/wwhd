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
/* (widened: pig route frame 134) set on every speed-ramp position-gap (3) tick30 place without an
 * earlier-step push; the check takes A's segment too when the frame started on the ground (landline begin()): there
 * the step's own check input (old.pos = the earlier step's end, its own y / speed.y share) met the sloped wall
 * differently from A's one check. NC WWHD_T60_WALLSEGALL_NEGCTRL=1 (the push frames only, as before). */
inline bool g_lkT60WallSegAll = false;
static inline bool lkT60_wallSegAllOff() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_WALLSEGALL_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* the xz moves of this frame's earlier steps' BG checks (mAcch.CrrPos: wall pushes), noted around the call in main_04
 * (d_a_player_landline_t60.h); reset on the frame's first step */
inline double g_lkT60BgK[2] = {0.0, 0.0};
/* (2026-11-25, long1 N 4 seg 1 frame 1082: Link +0.079 / +0.021, then the roll's wall
 * crash at 1094 a frame apart): posMove's collision push (lk_ccMoveStep, main_03) took a push first seen
 * on a later step of a frame that started without one (the collision pass after an earlier step: Link and the pusher at
 * their mid-frame places) and handed out its share on that step; A's frame has no push there: its collision pass runs at
 * the frame end and the push moves Link on the next frame. Form: only the frame's first step takes a push; a later step's
 * fresh one waits for the next frame's first step (the frame end's pass). NC WWHD_T60_CCFRESH_NEGCTRL=1 (the late take). */
static inline bool lkT60_ccLateFreshNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CCFRESH_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* (2026-11-25, route ld frame 405, Outset lookout ladder climb-over, N 2: Link
 * 5.53 xz off): g_lkT60BgK counts an earlier step's CrrPos xz push, which the tick30 speed-ramp position-gap (3) place takes out of K; but a
 * push the same execute overwrites afterwards (LADDER_UP_END: the ladder / root code puts the place back) never reaches
 * K (corr = p - W at the next posAdd), and the place subtracted a push K did not hold (405: exactly the push (2.297,
 * 5.031)). Form: landline::end keeps the step's pre-CrrPos xz and its push (g_lkT60BgLast); the frame's next posAdd,
 * when its p is bit-equal there in xz (the push undone), takes that step's note back out of g_lkT60BgK, before the speed-ramp position-gap (3)
 * place and the WallSeg flags read it. A push that survives (AIRSEG's walk frame 111, the pig route's wall) is kept.
 * NC WWHD_T60_BGKSURVIVE_NEGCTRL=1. Game build only; dt 1 never. */
struct LkT60BgLast {
    f32 pre[2];     /* the step's xz before its CrrPos */
    double push[2]; /* the push its CrrPos noted into g_lkT60BgK */
    bool on;
};
inline LkT60BgLast g_lkT60BgLast = {};
static inline bool lkT60_bgSurviveOff() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BGKSURVIVE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* (2026-11-25; bk3 at N 2, Link frame 125 CUT_A, A 75.859 -> 72.557 / B 72.529, then the
 * Bokoblin 47C5CE68's sensing, its launch turn at 153 and the held stick 47C58678 0.15 off): posMove's root move (modes
 * 1 / 5 / 8 / 9: the animation root's step, turned into the world by shape_angle.y) adds A's frame root step once, turned
 * by the frame's angle. At N steps each step turned its own part by its own angle (a turning cut: step 0 -28659, A's
 * -28778), and the speed-ramp position-gap (3) tick30 place kept the earlier step's turned part as a correction (K): x 0.028 off. Form, with
 * speed-ramp position-gap (3): the earlier steps' root xz adds are noted (w); the tick30 place leaves them out (as the BG pushes), and the
 * tick30 step's root add takes the frame's root step (the root now - the frame-start root, A's subtraction) turned by
 * its angle: A's add on A's place. y (mode 5) keeps the per-step adds (no turn). Only when the tick30 place took the
 * note out (speed-ramp position-gap (3), the root note of this frame live). NC WWHD_T60_LKROOTMOVE_NEGCTRL=1 (game build); dt 1: never. */
struct LkT60RootMove {
    uint32_t stamp; /* step counter of the frame's first step */
    f32 s0[3];      /* the frame-start root (the first step's posMove start) */
    double w[2];    /* the earlier steps' root adds (world x, z) */
    bool sub;       /* the tick30 place left w out */
    bool valid;
};
inline LkT60RootMove g_lkT60Root = {};
static inline bool lkT60_rootNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_LKROOTMOVE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
static inline bool lkT60_rootLive() {
    const LkT60RootMove& r = g_lkT60Root;
    return r.valid && t60_step_counter() - r.stamp < t60::steps();
}
/* posMove, before posMoveFromFootPos: the step's root start (after the mode's own changes) */
static inline void lkT60_rootStart(f32 x, f32 y, f32 z) {
    LkT60RootMove& r = g_lkT60Root;
    if (t60::dt() == 1.0f || lkT60_rootNc()) {
        r.valid = false;
        return;
    }
    if (t60::first_step()) {
        r.stamp = t60_step_counter();
        r.s0[0] = x;
        r.s0[1] = y;
        r.s0[2] = z;
        r.w[0] = r.w[1] = 0.0;
        r.sub = false;
        r.valid = true;
    }
}
/* posMove's root add: on the tick30 step whose place left the earlier adds out, the frame's root step (m: the root mode;
 * nx / nz: the root now) */
static inline void lkT60_rootFrame(int m, f32 nx, f32 nz, f32* dx, f32* dz) {
    const LkT60RootMove& r = g_lkT60Root;
    if (t60::dt() == 1.0f || !t60::tick30() || !lkT60_rootLive() || !r.sub)
        return;
    if (m == 1 || m == 5) {
        *dx = nx - r.s0[0];
        *dz = nz - r.s0[2];
    } else {
        *dx = r.s0[0] - nx;
        *dz = r.s0[2] - nz;
    }
}
/* after the root add: an earlier step's add noted; the tick30 step drops the note */
static inline void lkT60_rootNote(f32 x0, f32 z0, f32 x1, f32 z1) {
    LkT60RootMove& r = g_lkT60Root;
    if (t60::dt() == 1.0f || !lkT60_rootLive())
        return;
    if (t60::tick30()) {
        r.valid = false;
        return;
    }
    r.w[0] += (double)x1 - (double)x0;
    r.w[1] += (double)z1 - (double)z0;
}

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
        /* the last noted push undone before this posAdd: its note out of BgK */
        if (g_lkT60BgLast.on && !lkT60_bgSurviveOff() && p[0] == g_lkT60BgLast.pre[0] && p[2] == g_lkT60BgLast.pre[1]) {
            g_lkT60BgK[0] -= g_lkT60BgLast.push[0];
            g_lkT60BgK[1] -= g_lkT60BgLast.push[1];
        }
    }
    g_lkT60BgLast.on = false;
    const bool m6c3 = t60::tick30() && !lkT60_move6c3Off();
    double dl[3] = {0.0, 0.0, 0.0};
    for (int i = 0; i < 3; i++) {
        if (m6c3 && i != 1) { /* (speed-ramp position-gap (3)) xz: the frame start + A's one move + the corrections */
            const double s6 = s.P0[i] + s.K[i] - (lkT60_move6c3K() ? 0.0 : g_lkT60BgK[i == 0 ? 0 : 1]) -
                              (lkT60_rootLive() ? g_lkT60Root.w[i == 0 ? 0 : 1] : 0.0) + vf[i]; /* (CUTA-MOVE) */
            if (lkT60_rootLive())
                g_lkT60Root.sub = true;
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
    else if (m6c3 && !lkT60_move6c3K() && !lkT60_wallSegAllOff())
        g_lkT60WallSegAll = true; /* (widened) */
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
