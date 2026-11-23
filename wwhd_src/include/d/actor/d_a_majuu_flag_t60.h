/* d_a_majuu_flag_t60.h (WWHD, true 60 build): the Forsaken Fortress's flags
 * (d_a_majuu_flag.cpp, MjFlag) on a general step length (dt = 1/N). Converted, the flag executes on
 * every step with g_dt = 1/N; at dt = 1 (N = 1, the 30 Hz fallback, an unconverted flag) every helper returns the
 * original per-frame value, so the original instructions run (dt-1 check bit-exact, no extra loads). Separate file:
 * d_a_majuu_flag.cpp keeps thin calls only.
 *
 * majuu_flag_move 021B92DC:
 *   - the cloth (21 vertices, a triangle): the d_cloth_packet form (the per-step smoothing rule): per frame velocity +=
 *     F (the springs, gravity, the wind on the normals), velocity *= lDrag, pos = old + velocity. A step: velocity +=
 *     F x cloth60_push(lDrag) (the drag-consistent share), velocity *= lDrag^dt, pos = old + velocity x dt (the velocity
 *     stays a per-frame velocity). The forces are re-evaluated on each step's positions (the cloth-simulation rule: checked
 *     statistically in pairs).
 *   - mWave += mWaveSpeed (0x400) a frame (the wind strength's swing, visual): cloth60_share (exact per 30 Hz frame).
 *   - no random draws, no counters, no decisions (the first-step AI-timer rule: nothing to place).
 * daMajuu_Flag_Execute 021B9730: the pose from the parent matrix each step (no change); the yaw's cLib_addCalcAngleS2
 *   (8, 0x400) is c_lib's integer form.
 * No host memory. */
#pragma once
#include "d/d_cloth_packet_t60.h"

/* the wave phase's per-frame add for this step (exact per 30 Hz frame) */
static inline s32 majuu60_share(s32 perFrame) { return t60::dt() == 1.0f ? perFrame : cloth60_share(perFrame); }

/* the velocity's per-frame damping for this step: drag^dt; drag itself at dt 1 */
static inline f32 majuu60_drag(f32 drag) { return cloth60_drag(drag); }

/* the step's share of the frame's force push (drag-consistent): v x g at dt < 1 */
static inline void majuu60_push(cXyz* v, f32 drag) {
    if (t60::dt() == 1.0f) {
        return;
    }
    const f32 g = cloth60_push(drag);
    v->x = (f32)v->x * g;
    v->y = (f32)v->y * g;
    v->z = (f32)v->z * g;
}

/* pos += velocity (a frame) / velocity x dt (a step); the original PSVECAdd at dt 1 */
static inline void majuu60_move(majuu::Vector* pos, void* velocity) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        majuuApi_addVector(pos, velocity, pos);
        return;
    }
    const majuu::Vector* v = static_cast<const majuu::Vector*>(velocity);
    for (u32 i = 0; i < 3; ++i) {
        pos->mValues[i] = gabi::fmadds((f32)v->mValues[i], dt, (f32)pos->mValues[i]);
    }
}
