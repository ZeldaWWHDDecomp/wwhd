/* JPA_particle_t60.h (WWHD, true 60 build): particle motion on a general step length
 * (dt = 1/N).
 *
 * JPABaseParticle (JPA_particle.cpp), once JPA-10 runs the particle calc on every step at g_dt = 1/N:
 *   - incFrame 028266C0: the age mCurFrame +0x78 advances by dt (the 30 Hz timer rule). The original clamps a negative age to 0 (a new
 *     particle starts at -1, its first calc makes it 0): on the frame's earlier steps the age may stay negative (-1 + dt),
 *     the clamp acts on the tick30 step, so the frame-end age is A's; the normalised age +0x80 reads 0 while negative.
 *   - calcVelocity 02826720: the base velocity +0x40 gets accel x dt (+0x4C), the air resistance +0x70 as air^dt; the
 *     field velocity (+0x58, rebuilt per call by the field manager, JPA-08) and field acceleration (+0x64) as before;
 *     mVelocity +0x34 stays a per-frame velocity.
 *   - calcPosition 0282685C: the local position +0x1C moves by velocity x dt plus the exact-arc term of the base
 *     acceleration, accel x air^dt x moment x drag x dt (1 - dt) / 2 (the per-step movement rule: two half steps of a semi-implicit step
 *     equal one full step for a constant acceleration without air resistance; with it within N-step spec's tolerance).
 *   - checkCreateChild 028268C8: the child spawn every (step + 1) frames is a crossing: the step whose age crosses a
 *     whole frame k with k % (step + 1) == 0 (the 30 Hz timer rule / 5); the timing ratio check stays.
 *   - calcParticles (0281FBD8 / 0281FD14): the deferred-deletion delay +0x10C counts 30 Hz frames (tick30 step); a
 *     child's first frame (no velocity update, age <= 0) is the frame, not the first step. deleteParticle 0281FBAC
 * the HD two-frame wait starts at 3 on an earlier step (the frame's tick30 step takes it to A's 2), and
 *     such a particle keeps aging over the frame's remaining steps (delay 3), so its frame-end age and its release
 *     frame are A's (with 2 it was released one 30 Hz frame early).
 * dt = 1: the original. */
#pragma once
#include "true60.h"

namespace jpa_t60p {
inline bool full() { return t60::dt() == 1.0f; }
/* a per-frame count moves on this step: dt 1 or the frame's tick30 step */
inline bool frame_count() { return t60::dt() == 1.0f || t60::tick30(); }
/* a countdown of n 30 Hz frames started on this step: n, or n + 1 on an earlier step (its frame's tick30 decrements) */
inline unsigned countdown_start(unsigned n) { return frame_count() ? n : n + 1; }
/* the step that took the age to `age` crossed (or reached) a whole frame: floor(age) > floor(age - dt) */
inline bool crosses_frame(f32 age, f32 dt) {
    return __builtin_floorf(age) > __builtin_floorf(age - dt);
}
/* a per-frame factor d over a step of dt frames: d^dt (d <= 0 as it is) */
inline f32 damp(f32 d, f32 dt) {
    if (dt == 1.0f || !(d > 0.0f))
        return d;
    return (f32)__builtin_pow((double)d, (double)dt);
}
}  // namespace jpa_t60p
