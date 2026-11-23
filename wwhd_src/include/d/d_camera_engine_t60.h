/* d_camera_engine_t60.h (WWHD, true 60 build; 2026-11-17): helpers for the camera engines on a general
 * step length (dt = 1/N, the per-step smoothing rule / 4, the mode-start-once rule). Decomp build only.
 * Used by d_camera_talk.cpp (talktoCamera) and d_camera_engine.cpp (nonOwner / fixedFrame /
 * fixedPosition / crawl / hookshot cameras).
 *
 * The camera's style / mode timers (dCamera_c::Run) and the engines' own frame counters count 30 Hz frames. An
 * engine's start-up block (`timer == 0`) and its other per-frame-value blocks (`timer == 1`, a shot's first frame)
 * run once per value, on the first step that sees it (the mode-start-once rule), not on every step of that frame: a second run
 * would re-capture the start from what the first step already moved. CamStepEdge tracks one such counter: first()
 * is true on the first step of a run of consecutive steps that see the same value within one 30 Hz frame; a value
 * seen again after a frame end, after a gap (the engine did not run on the previous step) or after restart() is new.
 * Host memory, one camera. At dt = 1 first() is always true (the caller's original `timer == v` test decides). */
#pragma once
#include "true60.h"

struct CamStepEdge {
    uint32_t stamp;
    uint32_t val;
    bool inFrame;
    bool result;
    bool valid;
};

/* true on the first step that sees `v` (see above); repeated calls within one step return the step's answer */
static inline bool camEdgeFirst(CamStepEdge& e, uint32_t v) {
    if (t60::dt() == 1.0f)
        return true;
    uint32_t now = t60_step_counter();
    if (e.valid && e.stamp == now && e.val == v)
        return e.result;
    bool first = !(e.valid && e.inFrame && e.stamp + 1 == now && e.val == v);
    e.stamp = now;
    e.val = v;
    e.inFrame = !t60::tick30();
    e.result = first;
    e.valid = true;
    return first;
}
/* An engine whose start-up block returns (the view not moved on the start frame: nonOwner, hookshot): in the
 * original the start frame is the init alone and the update first runs on the next frame. At N steps the start frame's
 * later steps (the same timer value, consecutive, inside the frame: camEdgeFirst false) hold the view instead of
 * running the update half a frame early (the 30 Hz visual-motion rule addendum, upull pair
 * uy1: hookshotCamera's step 2 of the start frame moved the centre 23.7 and the eye ~15 ahead for good). Call after
 * camEdgeFirst returned false for the same `timer == v`; dt = 1: never (camEdgeFirst is always true there). */
static inline bool camEdgeStartHold() { return t60::dt() != 1.0f; }
/* the counter was reset to `v` by the engine itself on this step: a new start (the rest of the frame continues it) */
static inline bool camEdgeRestart(CamStepEdge& e, uint32_t v) {
    if (t60::dt() == 1.0f)
        return true;
    e.stamp = t60_step_counter();
    e.val = v;
    e.inFrame = !t60::tick30();
    e.result = true;
    e.valid = true;
    return true;
}
/* a per-frame smoothing ratio applied per step (the per-step smoothing rule): 1 - (1 - r)^dt; dt = 1: r */
static inline f32 camStepRatio(f32 r) { return t60::dt() == 1.0f ? r : t60_ratio(r, t60::dt()); }
/* the step that ends the 30 Hz frame (frame counters, per-frame bookkeeping); dt = 1: every step */
static inline bool camFrameEnd() { return t60::dt() == 1.0f || t60::tick30(); }
/* the speed-ramp position-gap rule (2) (upull pair uy: followCamera's start computes its transition length
 * m37C = ftoi(3.8 sqrt(...)) + 1 from the hookshot centre's lag behind Link's pull, 7 in A / 8 in B): a chase whose
 * result feeds a discrete decision takes the exact frame form for a moving target. The step chases with its own share
 * r_s = camStepRatio(r) toward the target moved ahead by a further lead x the step's own target move d:
 * x += r_s (target + lead d - x). Steady lag for a target moving w per 30 Hz frame: A (one step of r) w (1 - r) / r;
 * N steps of d = w dt: d ((1 - r_s) / r_s - lead); lead = (1 - r_s) / r_s - (1 - r) / (r dt) makes it A's (1/3 at
 * r 0.75, N 2). Ratios outside (0, 1) and dt = 1: 0 (the original). */
static inline f32 camChaseLead(f32 r) {
    if (t60::dt() == 1.0f || !(r > 0.0f) || !(r < 1.0f))
        return 0.0f;
    const double rs = (double)camStepRatio(r);
    return (f32)((1.0 - rs) / rs - (1.0 - (double)r) / ((double)r * (double)t60::dt()));
}
/* the chase target's move since the previous step of the same chase (consecutive steps only, else 0); host memory:
 * plain host floats (2026-11-19 beprobe: a host cXyz's be<> fields read / wrote the guest word at
 * ea(host address)) */
struct CamChaseTarget {
    uint32_t stamp;
    bool valid;
    float px, py, pz;
};
static inline void camChaseTargetMove(CamChaseTarget& t, const cXyz& p, cXyz* d) {
    const uint32_t now = t60_step_counter();
    const bool ok = t.valid && t.stamp + 1 == now;
    const float x = p.x, y = p.y, z = p.z;
    d->x = ok ? x - t.px : 0.0f;
    d->y = ok ? y - t.py : 0.0f;
    d->z = ok ? z - t.pz : 0.0f;
    t.stamp = now;
    t.valid = true;
    t.px = x;
    t.py = y;
    t.pz = z;
}

/* the same for an angle target (s16, wrapping): its move since the previous step of the same chase (consecutive steps
 * only, else 0); host memory */
struct CamChaseAngle {
    uint32_t stamp;
    bool valid;
    int16_t a;
};
static inline int16_t camChaseAngleMove(CamChaseAngle& t, int16_t a) {
    const uint32_t now = t60_step_counter();
    const bool ok = t.valid && t.stamp + 1 == now;
    const int16_t d = ok ? (int16_t)(uint16_t)((uint16_t)a - (uint16_t)t.a) : (int16_t)0;
    t.stamp = now;
    t.valid = true;
    t.a = a;
    return d;
}
