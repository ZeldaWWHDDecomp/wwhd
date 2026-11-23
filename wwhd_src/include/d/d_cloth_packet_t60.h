/* d_cloth_packet_t60.h (WWHD, true 60 build): the spring cloth (d_cloth_packet.cpp)
 * on a general step length (dt = 1/N). The cloth runs inside its owner's execute (the flags sie_flag /
 * tori_flag, the ghost ship, the shop items): a converted owner calls it on every step with g_dt = 1/N, an unconverted
 * one on tick30 with dt 1. At dt = 1 every helper returns the original per-frame value, so the original instructions
 * run (dt-1 check bit-exact). Separate file: d_cloth_packet.cpp keeps thin calls only.
 *
 * The cloth is a semi-implicit Euler integrator in per-frame units: per frame, speed += F (getFactor: the springs, the
 * gravity and the wind on the frame-start positions), speed *= drag, pos += speed. A step of dt (the per-step smoothing rule):
 * speed += F x g (F is a per-frame-squared push; g = cloth60_push, the drag-consistent share, dt at drag 1), speed *= drag^dt (dt steps of it compound to one frame's),
 * pos += speed x dt (speed stays the per-frame speed). The forces are re-evaluated on each step's positions (the finer
 * integration of the same springs; not exact against the frame form: N-step spec bound in units/_t60/d_cloth_packet.txt).
 * setNrm's two wave phases (mWave += mWaveSpeed, field_0xF0 += field_0xF2 a frame): shared x dt (cloth60_share).
 * Host memory: none (the share is stateless: the frame's tick30 step takes the rest). */
#pragma once
#include "true60.h"

/* the step's share of the frame's force impulse and move: 1 at dt = 1 */
static inline f32 cloth60_dt() { return t60::dt(); }

/* the per-frame speed factor (drag) for a step: drag^dt; drag outside (0, 1) as it is (a frozen or a growing speed
 * keeps the original's factor per step: no such parameter in the game's callers) */
static inline f32 cloth60_drag(f32 drag) {
    const f32 dt = t60::dt();
    if (dt == 1.0f || !(drag > 0.0f) || !(drag < 1.0f)) {
        return drag;
    }
    return (f32)__builtin_pow((double)drag, (double)dt);
}

/* the step's share of the frame's force push, drag-consistent (after pairs): the per-frame map is
 * speed' = (speed + F) x drag. N steps of speed += F x g, speed x= drag^dt compound to speed x drag + F x g x
 * drag^dt (1 - drag) / (1 - drag^dt); that equals the frame's (speed + F) x drag for g = drag^(1 - dt) (1 - drag^dt) /
 * (1 - drag). The plain share g = dt raised the cloth's speed gain by (dt drag^dt / (1 - drag^dt)) / (drag / (1 - drag))
 * (drag 0.9: +2.7 % at N 2, +4.1 % at N 4): B's flutter ran faster than A's (cloth-simulation gate: sf8 N 4 max speed +16 %, tori
 * tf2 N 2 +36 %). dt (the plain share) for drag outside (0, 1) */
static inline f32 cloth60_push(f32 drag) {
    const f32 dt = t60::dt();
    if (dt == 1.0f || !(drag > 0.0f) || !(drag < 1.0f)) {
        return dt;
    }
    const double d = drag, ddt = __builtin_pow(d, (double)dt);
    return (f32)(__builtin_pow(d, 1.0 - (double)dt) * (1.0 - ddt) / (1.0 - d));
}

/* an integer per-frame add (a wave phase) at a step of dt: dt of it (toward zero) on the frame's earlier steps, the
 * rest of the frame's amount on its tick30 step, so the frame's N steps add exactly the original amount (stateless: one
 * cloth packet per owner, several owners per step). The original amount at dt = 1. */
static inline s32 cloth60_share(s32 perFrame) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        return perFrame;
    }
    const s32 part = (s32)((f32)perFrame * dt);
    if (t60::tick30()) {
        return perFrame - part * (s32)(t60::steps() - 1);
    }
    return part;
}
