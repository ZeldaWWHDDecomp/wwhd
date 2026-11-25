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
#include <cstdlib>

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
/* The smoke / wind particles' drift (2026-11-24, seen on Link's landing dust): the smoke particles' callback
 * (dPa_smokeParticleExecute 025A60B0, emitter mUserData +0x258 0 = the wind applies; dPa_windParticleExecute 025A5098)
 * pushes the particle's offset position (+0x10) by the wind at it (dPa_setWindPower: dKyw_get_AllWind_vec x power x
 * 10051A08) once per 30 Hz frame: the game's particle callbacks run on the tick30 step, so the drift came in
 * one piece per frame (2-4x larger on the sub-step passes, the particles drawn one step back). Form (picture only): on a
 * frame's earlier step k the drawn place (+0x28, after calcPosition) gets k / N of the push the particle would get now
 * (the wind query is a pure read; no RNG, no state); the particle's next calc takes the add out of +0x28 before its
 * calcVelocity (wind_unpicture: a global-space field reads +0x28 there) and calcPosition rebuilds +0x28 from the offset;
 * the tick30 step's callback pushes the offset as in A (the frame end and every logic value are A's).
 * WWHD_T60_WIND_PIC=0: off (the negative control). dt 1 / tick30: nothing. */
inline bool wind_pic_on() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_WIND_PIC");
        return !(e && e[0] == '0');
    }();
    return on;
}
struct WindV { be<f32> v[3]; };
/* the picture's add per particle (put back before the particle's next calcVelocity: a global-space field reads +0x28
 * there, so the logic never sees the picture) */
struct WindPic { u32 ea, stamp; f32 add[3]; };
static WindPic s_windPic[4096];
inline WindPic& wind_slot(u32 q) { return s_windPic[(q >> 4) & 4095u]; }
/* the particle's calc on any step, before calcVelocity: the last step's picture out of +0x28 */
template <class Particle> inline void wind_unpicture(Particle* p) {
    const u32 q = gabi::ea(p);
    WindPic& w = wind_slot(q);
    if (w.ea != q) return;
    w.ea = 0;
    if (t60_step_counter() - w.stamp != 1u) return; /* (not the step before: a stale entry of a released particle) */
    for (u32 i = 0; i < 3; i++) {
        be<f32>* g = gabi::at<be<f32>>(q + 0x28 + 4 * i);
        *g = (f32)*g - w.add[i];
    }
}
template <class Emitter, class Particle> inline void wind_picture(Emitter* em, Particle* p) {
    if (frame_count() || !wind_pic_on()) return;
    auto* cb = p->mpCallBack2.get();
    if (!cb) return;
    const u32 ex = (u32)cb->mpVtable->execute.get();
    const u32 e = gabi::ea(em), q = gabi::ea(p);
    if (ex == 0x025A60B0u) {
        if ((u32)*gabi::at<be<u32>>(e + 0x258)) return;
    } else if (ex != 0x025A5098u)
        return;
    gabi::Local<WindV> pos, wind;
    gabi::Local<f32> power;
    for (u32 i = 0; i < 3; i++) *gabi::at<be<f32>>(gabi::ea(pos.get()) + 4 * i) = (f32)*gabi::at<be<f32>>(q + 0x10 + 4 * i);
    gabi::call<void>(0x0257E1B8u, pos.get(), wind.get(), power.get()); /* dKyw_get_AllWind_vec */
    const f32 amount = gabi::fmuls_ppc((f32)*gabi::at<be<f32>>(gabi::ea(power.get())), (f32)*gabi::at<be<f32>>(0x10051A08u));
    const f32 share = (f32)t60::step() / (f32)t60::steps();
    WindPic& w = wind_slot(q);
    if (w.ea && w.ea != q && t60_step_counter() - w.stamp <= 1u) return; /* (another particle's picture: none this step) */
    w.ea = q;
    w.stamp = t60_step_counter();
    for (u32 i = 0; i < 3; i++) {
        be<f32>* g = gabi::at<be<f32>>(q + 0x28 + 4 * i);
        const f32 before = *g;
        *g = before + (f32)*gabi::at<be<f32>>(gabi::ea(wind.get()) + 4 * i) * amount * share;
        w.add[i] = (f32)*g - before;
    }
}
/* follow-up (2026-11-24, swim strict pair: WATER_MARK A 38 / B 34): a particle's life (mLifeTime,
 * often fractional: the emitter's life random) ran out on an earlier step of a frame (its age there is the frame start's +
 * k / N), where the 30 Hz frame ends it on the frame's update (age + 1): it was deleted before the frame's tick30 step,
 * so its last particle callback (the game's callbacks run on the tick30 step) never ran, e.g. Link's water
 * drop's last ground check (daPy_waterDropPcallBack_c 02414048 creates a WATER_MARK from it). Form: on an earlier step
 * the expiry waits for the frame's tick30 step (the particle stays at the end of its life: norm time 1), where the 30 Hz
 * frame's expiry, callback and deletion happen as in A; an unordered (NaN) age expires at once. dt 1 / N 1: unchanged.
 * WWHD_T60_PTCL_EXPIRE=0: off (the particle ends on the earlier step, as before). */
inline bool expire_defer_on() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_PTCL_EXPIRE");
        return !(e && e[0] == '0');
    }();
    return on;
}
inline bool expire_now(f32 age) { return frame_count() || !expire_defer_on() || !(age == age); }
}  // namespace jpa_t60p
