/* d_camera_ride_t60.h (WWHD, true 60 build, 2026-11-19, rule note
 * speed-ramp position-gap (3)): rideCamera's eased work fields at their frame-end values on a general step length. Decomp build only.
 *
 * rideCamera (type 37 / style 36: Ocean room 0, y2fbrl1n2 / yb67a) eases its work fields toward their targets by
 * per-frame ratios - the radius and latitude (mRadius, mLatitude) and their speed scales by 0.08, the side offset
 * ratio by 0.05 / 0.1, the centre follow ratio by 0.2 - and uses the eased values in the same call: the globe g it
 * chases (radius, latitude), the centre offset and the centre target's chase ratio. A takes each frame's end values for
 * the whole frame. Per step the converted ratios (e2R) compose exactly while their targets hold still, but these
 * targets ease too: the frame's first step chased the half-eased radius (smaller than A's on the zoom-out), so the
 * radius grew slower every frame (yb67a: A 430.6 / B 422.7 @15, the eye 7.0 off @16, pitch 0.06 deg low, the centre
 * 1.6-2.0 off in the first frames). Form (as the follow camera's radius ease): every step of a frame
 * takes the fields' frame-end values - A's expression with A's ratio from the frame's start values - so N steps of
 * the chases compose to A's frame and the tick30 step stores A's exact values. The start values hold while the block
 * ran on every step of the frame and nothing else wrote the fields in between (the fields equal the block's last
 * exit); otherwise the step falls back to the per-step ratio. dt = 1: not used (the original). */
#pragma once
#include "true60.h"
#include "d/d_camera_key_t60.h"

namespace camera_ride_t60 {
enum { RADIUS, RADIUS_SPEED, LATITUDE, LATITUDE_SPEED, SIDE, CENTER_FOLLOW, N_FIELDS };
struct FrameEase {
    uint32_t last;       /* step counter of the block's last run */
    f32 s0[N_FIELDS];    /* the frame's start values */
    f32 x[N_FIELDS];     /* the fields at the block's last exit */
    bool frame;          /* s0 are this frame's */
    bool valid;
};
static camera_key_t60::PerCam<FrameEase> s_ease; /* host memory, this unit, one per camera body (d_camera_key_t60.h) */

/* once per run, before the eases: snapshot / check the fields; true: the frame's start values are valid */
static inline bool begin(const f32 (&f)[N_FIELDS]) {
    if (t60::dt() == 1.0f)
        return false;
    FrameEase& e = s_ease();
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        for (int i = 0; i < N_FIELDS; i++)
            e.s0[i] = f[i];
        e.frame = true;
    } else {
        bool same = e.frame && e.valid && e.last + 1 == now;
        /* bit identity ("nothing wrote the fields"): a float compare fails on a NaN field and dropped the whole
         * frame form to the per-step ratios (N-step spec inputs with a NaN latitude) */
        for (int i = 0; same && i < N_FIELDS; i++)
            same = __builtin_memcmp(&e.x[i], &f[i], sizeof(f32)) == 0;
        e.frame = same;
    }
    e.last = now;
    e.valid = true;
    return e.frame;
}
/* the eased value: frame valid -> A's frame-end value from the frame's start (ratio r, the original's per-frame
 * ratio); else the step's own ease with the step ratio rs (e2R(r)) from the current value */
static inline f32 ease(int i, f32 cur, f32 target, f32 r, f32 rs) {
    if (t60::dt() != 1.0f && s_ease().frame) {
        const f32 s = s_ease().s0[i];
        return gabi::fmadds(target - s, r, s);
    }
    return gabi::fmadds(target - cur, rs, cur);
}
/* after the block: the fields as it left them */
static inline void leave(const f32 (&f)[N_FIELDS]) {
    if (t60::dt() == 1.0f)
        return;
    for (int i = 0; i < N_FIELDS; i++)
        s_ease().x[i] = f[i];
}
}  // namespace camera_ride_t60
