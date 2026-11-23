/* JPA_field_t60.h (WWHD, true 60 build): the particle fields on a general step length
 * (dt = 1/N).
 *
 * Fields act on a particle through JPABaseField::calcVel 02822B54, which adds the field's per-frame vector to one of
 * three particle vectors (JPA_field_a.h JPAFieldVelocityType):
 *   - JPA_FIELD_ACCELERATION (+0x64 mFieldAccel, kept across frames) and JPA_BASE_VELOCITY (+0x40 mBaseVel): a per-frame
 *     change of a velocity: x dt (the per-step movement rule, v += a dt);
 *   - JPA_FIELD_VELOCITY (+0x58 mFieldVel, zeroed by every calcVelocity and rebuilt by the fields): a velocity the
 *     position takes x dt (calcPosition, JPA-07): as it is.
 * Discrete fields:
 *   - JPARandomField::calc 02823CDC (a random kick at age 0 and every `cycle` frames): fires on the step whose age crosses
 *     such a whole frame (the 30 Hz timer rule / 5), with the 30 Hz frame's kick (the emitter RNG drawn as often as at 30 Hz: draws at
 *     A's place): an acceleration / base-velocity kick added whole, a field-velocity kick (it lasts one 30 Hz frame) as
 *     its frame's displacement on that step (x 1/dt);
 *   - JPADragField::calc 02823E8C: the particle drag +0x88 is multiplied every frame (a compounding factor): factor^dt
 *     (the per-step smoothing rule, d -> d^dt).
 * Vortex, convection, spin, magnet, Newton, air (its speed limit is a cap) and gravity build per-frame vectors that go
 * through calcVel; the managers' init / preCalc / calc walks are unchanged. dt = 1: the original. */
#pragma once
#include "true60.h"

namespace jpa_t60f {
inline bool full() { return t60::dt() == 1.0f; }
/* calcVel's factor for a velocity type: dt for an acceleration / base-velocity change, 1 for a field velocity */
inline f32 vel_scale(u32 velType) { return velType == 2 ? 1.0f : t60::dt(); }
/* the step that took the age to `age` crossed (or reached) a whole frame */
inline bool crosses_frame(f32 age, f32 dt) { return __builtin_floorf(age) > __builtin_floorf(age - dt); }
/* a per-frame factor over a step: d^dt; a factor without a real power (d <= 0, NaN) applies whole on the frame's tick30
 * step and as 1 on the earlier steps (the frame-end product is the 30 Hz frame's) */
inline f32 damp(f32 d, f32 dt) {
    if (dt == 1.0f)
        return d;
    if (!(d > 0.0f))
        return t60::tick30() ? d : 1.0f;
    return (f32)__builtin_pow((double)d, (double)dt);
}
}  // namespace jpa_t60f
