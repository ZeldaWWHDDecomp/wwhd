/* d_a_sail_t60.h (WWHD, true 60 build): the pirate ship's sail (d_a_sail.cpp) on a general
 * step length (dt = 1/N). Converted, the sail executes on every step with g_dt = 1/N; at dt = 1 (N = 1,
 * the 30 Hz fallback, an unconverted sail) every helper returns the original per-frame value, so the original
 * instructions run (dt-1 check bit-exact). Separate file: d_a_sail.cpp keeps thin calls only.
 *
 * Sites (sail_move 02460678, setCorrectNrmAngle 0245FFC8):
 *   - the furl branch and mFurlSpeed's per-frame change (-0.001 toward the furled 0.6, +0.0075 toward 0): once per
 *     frame on its first step, held for its other steps (sail60_furlFrame); the furl's cLib_addCalc is c_lib's dt form
 *     (the per-step smoothing rule / the clamped-smoothing rule: ratio 1 - (1 - r)^dt, steps x dt) at the frame's speed on the earlier steps; the
 *     tick30 step runs the original ease from the frame-start mFurl (sail60_furlFull: exact at every frame end).
 *   - mWavePhase += tmp x (0.85 f25 + 0.15) a frame (ftoi of the float sum): latched at the frame's first step, dt of
 *     the increment a step from there, the tick30 step on the original form from the frame start (sail60_wavePhase).
 *   - setCorrectNrmAngle: cM_rndF(200) once per 30 Hz frame, on the tick30 step (the per-step chance rule: the draw's place in the
 *     frame's RNG sequence is the 30 Hz one, where the unconverted world draws), the value held for the frame's other
 *     steps (the first converted frame's earlier steps take its mean, 100: no draw); mNrmWobblePhase += rnd + 900 is
 *     shared x dt, the tick30 step taking the rest at the new draw (sail60_wobbleAdd, the first-step AI-timer rule: exact at every
 *     frame end); mNrmCorrectAngle's cLib_addCalcAngleS2 is c_lib's integer form (ends
 *     each 30 Hz frame on the original step, the clamped-smoothing rule).
 *   - the cloth vertices / normals are a pure function of these states (no change); daSail_Execute's pose is read from
 *     the pirate ship's model matrix each step (no change).
 * Host memory only, keyed by the owner (the sail actor / its packet). */
#pragma once
#include "d/actor/d_a_ship_t60.h"

/* a per-frame rate change for a step of dt */
static inline f32 sail60_rate(f32 perFrame) {
    const f32 dt = t60::dt();
    return dt == 1.0f ? perFrame : perFrame * dt;
}

struct Sail60Furl {
    s32 branch;
    f32 start;
    u32 stamp;
    const void* owner;
    bool valid, full;
};
/* sail_move's furl branch (furl toward 0.6 / unfurl toward 0 / neither, from l_HIO and mFurl) and mFurlSpeed's
 * per-frame update (0.015 or -0.001 a frame / +0.0075 a frame / 0): once per 30 Hz frame, on the frame's first step,
 * from the frame-start state; the frame's other steps keep the branch and the speed (a per-frame speed, the maxStep of
 * the furl's cLib_addCalc, which is c_lib's dt form: x dt a step). So every step of the frame eases mFurl with the
 * frame's speed and the N steps' clamped moves add up to the original frame's one (no per-step speed ramp, which put
 * the frame mean (N - 1) / (2N) x the change off, and no branch flip on a mid-frame mFurl). Returns whether this step
 * updates the speed; *branch: the branch to run. A frame the site joined after its first step (a new sail, a gap)
 * decides on that step. mFurl's frame-start value is latched with it (sail60_furlFull). dt = 1: the original (update
 * on every frame, its own branch). */
static inline bool sail60_furlFrame(Sail60Furl& c, const void* owner, s32* branch, f32 furl) {
    if (t60::dt() == 1.0f) {
        return true;
    }
    const u32 now = t60_step_counter();
    const bool first = t60::first_step();
    if (first || !c.valid || c.owner != owner || now - c.stamp != 1) {
        c.branch = *branch;
        c.start = furl;
        c.full = first;
        c.stamp = now;
        c.owner = owner;
        c.valid = true;
        return true;
    }
    c.stamp = now;
    *branch = c.branch;
    return false;
}
/* the furl's ease on the tick30 step of a frame the site ran from its first step: mFurl back to the frame-start value
 * and the original ease at dt 1 (the frame's one cLib_addCalc at the frame's speed), so the frame ends on the
 * original's value (no dt-ratio rounding carried into mFurl, mFurlFactor, the wave phase and the vertices; the earlier
 * steps keep c_lib's dt form for the picture between). Returns the dt to put back (sail60_furlFullEnd), 0: nothing. */
static inline f32 sail60_furlFull(const Sail60Furl& c, be<f32>* furl) {
    if (t60::dt() == 1.0f || !t60::tick30() || !c.full) {
        return 0.0f;
    }
    *furl = c.start;
    return t60_set_dt(1.0f);
}
static inline void sail60_furlFullEnd(f32 previous) {
    if (previous > 0.0f) {
        t60_set_dt(previous);
    }
}

struct Sail60FCarry {
    f32 acc;
    s32 start;
    u32 stamp;
    const void* owner;
    bool valid, full;
};
/* mWavePhase's update: at dt 1 the original ftoi(fmadds(tmp, k, phase)). At dt < 1 the phase is latched at the frame's
 * first step (`start`) and each step moves it by dt of the frame's increment tmp x k from there (the float sum `acc`,
 * the phase stays start + acc in the original's float form); the tick30 step of a frame the site ran from its first
 * step ends on the original form from the frame start, ftoi(fmadds(tmp, k, start)) (ship60_start's form: exact over a
 * frame of constant amount, no rounding carried into the next frame, the same float grid as the original at large
 * phases). A frame the site joined after its first step (a new sail, a gap) runs the in-between form to its end. */
static inline s32 sail60_wavePhase(Sail60FCarry& c, const void* owner, s32 tmp, f32 k, s32 phase) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        return gabi::ftoi(gabi::fmadds((f32)tmp, k, (f32)phase));
    }
    const u32 now = t60_step_counter();
    const bool first = t60::first_step();
    if (first || !c.valid || c.owner != owner || now - c.stamp != 1) {
        c.start = phase;
        c.acc = 0.0f;
        c.full = first;
    }
    c.stamp = now;
    c.owner = owner;
    c.valid = true;
    if (t60::tick30() && c.full) {
        return gabi::ftoi(gabi::fmadds((f32)tmp, k, (f32)c.start));
    }
    c.acc = gabi::fmadds((f32)tmp * k, dt, c.acc);
    return gabi::ftoi(c.acc + (f32)c.start);
}

struct Sail60Rnd {
    s32 value;
    const void* owner;
    bool valid;
};
/* the frame's cM_rndF(200) (as ftoi): drawn on the tick30 step only (always at dt 1), held for the other steps */
static inline s32 sail60_wobbleRnd(Sail60Rnd& h, const void* owner) {
    if (ship60_tick30()) {
        h.value = gabi::ftoi(cM_rndF(200.0f));
        h.owner = owner;
        h.valid = true;
        return h.value;
    }
    return h.valid && h.owner == owner ? h.value : 100;
}

/* mNrmWobblePhase += draw + 900 a frame (the first-step AI-timer rule, 2026-11-16: a frame's decision drives the whole frame's
 * motion). The draw stays on the tick30 step (the per-step chance rule: its place in the 30 Hz RNG sequence), so the frame's earlier
 * steps move by dt of the held draw + 900 (the previous frame's: no later value exists yet) and the tick30 step of a
 * frame the site ran from its first step takes the REST of the frame's amount at the new draw: the frame adds exactly
 * the original's draw + 900 and the phase ends every 30 Hz frame on the original value (was ship60_share's carry: the
 * earlier steps' share of the previous draw stayed in, (N - 1) / N x (draw_f - draw_f-1) at each frame end). A frame
 * the site joined after its first step (a new sail, a gap) adds dt shares only. dt = 1: the original amount. */
struct Sail60Add {
    s32 given;
    f32 acc;
    u32 stamp;
    const void* owner;
    bool valid, full;
};
static inline s32 sail60_wobbleAdd(Sail60Add& c, const void* owner, s32 perFrame) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        return perFrame;
    }
    const u32 now = t60_step_counter();
    const bool first = t60::first_step();
    if (first || !c.valid || c.owner != owner || now - c.stamp != 1) {
        c.given = 0;
        c.acc = 0.0f;
        c.full = first;
    }
    c.stamp = now;
    c.owner = owner;
    c.valid = true;
    if (t60::tick30() && c.full) {
        return perFrame - c.given;
    }
    c.acc += (f32)perFrame * dt;
    const s32 add = (s32)c.acc; /* toward zero; the rest carries to the frame's later steps */
    c.acc -= (f32)add;
    c.given += add;
    return add;
}
