/* d_a_player_stepdown_t60.h (WWHD, true 60 build; 2026-11-23, rule:
 * a different airborne / grounded state is not the per-step ground-check rule). Decomp build only.
 * Separate file: d_a_player_main_03.cpp keeps thin calls.
 *
 * autoGroundHit 023FE2F0's step-down snap: after the frame's BG check (CrrPos) found no ground hit, a ground at most 30.1
 * below Link puts him on it (speed.y 0, GROUND_HIT); further below, he is in the air. A checks once a frame, at the
 * frame-end position. At N steps an earlier step checks at its in-between position and can take a snap that A's frame
 * never takes: walking off the top ledge of the cliff stair slope (route, frame 3593: A's frame end is 45 above the
 * ground there, airborne, the auto jump next frame), B's step 0 found a lower ground 22.8 below its half-step position,
 * snapped down onto it, the tick30 step was pushed along the ledge's face (xz 30 units off), the auto jump started 24
 * lower and 37 to the side, the landing came 2 frames early with another procedure sequence (A 37 / 39 / 37, B 38 / 39 /
 * 37). Form (A's frame segment for the decision): on an earlier step (not tick30) the snap is taken only when the frame's
 * end would take it too: the ground under the frame-end position (this step's position + the rest of the frame's move at
 * the step's speeds: xz speed x the remaining time, y the fall with gravity) is probed (the Acch ground check's reach, from
 * 30.1 above); when that ground is more than 30.1 below the projected end (or none), A's frame ends in the air and the
 * earlier step stays in the air too (no snap; the hover test after it runs as A's would). The tick30 step and every
 * frame whose end keeps the ground snap as before (the per-step ground-check rule: the per-step snaps on the stair slope that rejoin A
 * stay). dt 1 (N = 1): never. Negative control WWHD_T60_STEPDOWN_NEGCTRL=1 (game build). Host memory, one player. */
#pragma once
#include "true60.h"
#include <cstdlib>

struct LkT60StepDown {
    uint32_t stamp; /* step counter of the frame's first step */
    bool valid;
};
static LkT60StepDown s_lkT60StepDown;

static inline bool lkT60_stepDownNegCtrl() {
    static const bool nc = [] {
        const char *e = getenv("WWHD_T60_STEPDOWN_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

/* top of autoGroundHit: the frame's first step */
static inline void lkT60_stepDownFrame() {
    if (t60::dt() != 1.0f && t60::first_step()) {
        s_lkT60StepDown.stamp = t60_step_counter();
        s_lkT60StepDown.valid = true;
    }
}

/* the time (in 30 Hz frames) of the frame left after this step; 0 on the tick30 step, at dt 1, without a record, or
 * under the negative control (the snap stays the step's own) */
static inline float lkT60_stepDownRest() {
    if (t60::dt() == 1.0f || t60::steps() <= 1 || t60::tick30() || lkT60_stepDownNegCtrl())
        return 0.0f;
    const LkT60StepDown &s = s_lkT60StepDown;
    const uint32_t k = t60_step_counter() - s.stamp + 1; /* this step's number in the frame, 1..N */
    if (!s.valid || k < 1 || k >= t60::steps())
        return 0.0f;
    return (float)(t60::steps() - k) * t60::dt();
}

/* true: the earlier step skips the snap (A's frame ends in the air). gndChk: the guest address of a dBgS_GndChk (bytes
 * saved and put back around the probe), size its size; probe(x, y, z) runs the ground check from there and returns the
 * cross height (-1e9 when none). */
template <class Probe>
static inline bool lkT60_stepDownHold(float rest, float x, float y, float z, float vx, float vy, float vz, float g,
                                      uint32_t gndChk, uint32_t size, Probe probe) {
    if (rest <= 0.0f)
        return false;
    const float xe = x + vx * rest;
    const float ze = z + vz * rest;
    const float ye = y + vy * rest + 0.5f * g * rest * rest;
    uint8_t keep[0x80];
    if (size > sizeof(keep))
        return false;
    for (uint32_t i = 0; i < size; i++)
        keep[i] = gabi::load<uint8_t>(gndChk + i);
    const double gc = probe(xe, ye + 30.1f, ze);
    for (uint32_t i = 0; i < size; i++)
        gabi::store<uint8_t>(gndChk + i, keep[i]);
    return gc == -1000000000.0 || (float)(gc - (double)ye) < -30.1f;
}

/* Second form (2026-11-23, pot-pairs at N 4, frame 253: walking off an edge into AUTO_JUMP): the
 * lift-off frame. A's frame starts on the ground, moves once (speed.y = the frame-start speed.y + gravity, the limit as
 * posMove's dt 1 path; pos.y + it) and its one check finds no ground: Link falls the whole frame (y0 - 2.5). At N steps
 * the steps still on the edge end on the ground (their fall cleared), the first step past it starts falling from 0 at
 * its sub-step: B reaches y0 - 1.64 (vy -0.625 / -1.25 / -1.875 per step), +0.234 kept from the auto jump on.
 * Form (A's frame segment, as's landing frame): when the frame started on the ground, an earlier step of
 * it ended on the ground and the tick30 step ends in the air (after autoGroundHit), the tick30 step gets A's frame-end
 * height and fall speed: y = y0 + A's speed.y, speed.y = A's; the step's own xz stays (the steps' xz moves add up to the
 * frame's). If the ground the step's check found under it (m_ground_h, same xz) would hold A there (at most 30.1 below
 * A's height, or above it), Link is put on it as A's check / snap would (speed.y 0, GROUND_HIT). Hover (the boots' flag)
 * and fly modes: never. dt 1 (N = 1): never. Negative control WWHD_T60_LIFTOFF_NEGCTRL=1 (game build). */
static inline bool lkT60_liftOffNegCtrl() {
    static const bool nc = [] {
        const char *e = getenv("WWHD_T60_LIFTOFF_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

struct LkT60LiftOff {
    uint32_t stamp;
    f32 y0, sy;
    bool grounded0;     /* the previous frame-end check had the ground */
    bool earlierGround; /* an earlier step of this frame ended on the ground */
    bool valid;
};
static LkT60LiftOff s_lkT60LiftOff;

/* the frame's first step, before posMove: y0 / speed.y / gravity / maxFallSpeed as the frame starts, ground = the
 * previous check's GROUND_HIT, skip = a fly mode or hover */
static inline void lkT60_liftOffStart(f32 y0, f32 vy, f32 g, f32 maxFall, bool heavy, bool ground, bool skip) {
    LkT60LiftOff &s = s_lkT60LiftOff;
    if (t60::dt() == 1.0f || t60::steps() <= 1 || !t60::first_step())
        return;
    s.valid = !skip && ground && !lkT60_liftOffNegCtrl();
    if (!s.valid)
        return;
    s.stamp = t60_step_counter();
    s.y0 = y0;
    f32 sy;
    if (heavy) {
        const f32 lim = maxFall * 1.5f;
        sy = gabi::fmadds(g, 2.25f, vy);
        if (sy < lim)
            sy = lim;
    } else {
        const f32 lim = maxFall;
        sy = vy + g;
        if (sy < lim)
            sy = lim;
    }
    s.sy = sy;
    s.grounded0 = ground;
    s.earlierGround = false;
}

/* after autoGroundHit, every step: ground = this step's GROUND_HIT now, skip = a fly mode or hover; on the tick30 step
 * returns true when A's frame end is put in place (y, vy, ground in / out) */
static inline bool lkT60_liftOffStep(bool ground, bool skip, f32 gh, f32 &y, f32 &vy, bool &groundOut) {
    LkT60LiftOff &s = s_lkT60LiftOff;
    if (t60::dt() == 1.0f || !s.valid || t60_step_counter() - s.stamp >= t60::steps())
        return false;
    if (!t60::tick30()) {
        if (ground)
            s.earlierGround = true;
        return false;
    }
    s.valid = false;
    if (ground || skip || !s.grounded0 || !s.earlierGround)
        return false;
    const f32 ya = s.y0 + s.sy;
    if (gh != -1000000000.0f && !(gh - ya < -30.1f)) {
        y = gh;
        vy = 0.0f;
        groundOut = true;
    } else {
        y = ya;
        vy = s.sy;
        groundOut = false;
    }
    return true;
}
