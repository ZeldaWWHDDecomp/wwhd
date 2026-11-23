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

struct LkT60PosAdd {
    double S[3];    /* the exact own-move position (corrections included) */
    double C[3];    /* the frame's push handed out so far */
    f32 W[3];       /* the coordinates this form last wrote (a difference at the next call = a per-step correction) */
    uint32_t stamp; /* step counter of the frame's first step */
    bool valid;
};
static LkT60PosAdd s_lkT60PosAdd;

/* dt 1 (the original's add, held / whole steps included): the shadows are dropped */
static inline void lkT60_posAddReset() {
    s_lkT60PosAdd.valid = false;
}

static inline bool lkT60_posAddLive() {
    const LkT60PosAdd &s = s_lkT60PosAdd;
    return s.valid && t60_step_counter() - s.stamp < t60::steps();
}

/* dt != 1: v = this step's own move (move x dt, plus y's arc term); returns the new coordinates in out */
static inline void lkT60_posAdd(const f32 p[3], const double v[3], f32 out[3]) {
    LkT60PosAdd &s = s_lkT60PosAdd;
    if (t60::first_step() || !lkT60_posAddLive()) {
        for (int i = 0; i < 3; i++) {
            s.S[i] = (double)p[i];
            s.C[i] = 0.0;
        }
        s.stamp = t60_step_counter();
        s.valid = true;
    } else {
        for (int i = 0; i < 3; i++)
            s.S[i] += (double)p[i] - (double)s.W[i]; /* the corrections since the last write (the per-step ground-check rule) */
    }
    for (int i = 0; i < 3; i++) {
        s.S[i] += v[i];
        out[i] = (f32)(s.S[i] + s.C[i]);
        s.W[i] = out[i];
    }
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
