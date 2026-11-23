/* d_a_pirate_flag_t60.h (WWHD, true 60 build): the pirate ship's flag (d_a_pirate_flag.cpp,
 * the Kaizokusen's masthead flag) on a general step length (dt = 1/N). Converted, the flag executes on
 * every step with g_dt = 1/N; at dt = 1 (N = 1, the 30 Hz fallback, an unconverted flag) every helper returns the
 * original per-frame value, so the original instructions run (dt-1 check bit-exact, no extra loads). Separate file:
 * d_a_pirate_flag.cpp keeps thin calls only.
 *
 * pirate_flag_move 023D1EA4:
 *   - the cloth: the d_cloth_packet form (the per-step smoothing rule): per frame velocity += F (the springs, gravity,
 *     the wind on the normals), velocity *= 0.875, pos = old + velocity. A step: velocity += F x cloth60_push(0.875)
 *     (the drag-consistent share), velocity *= 0.875^dt, pos = old + velocity x dt (the velocity stays a per-frame
 *     velocity). The forces are re-evaluated on each step's positions (the cloth-simulation rule: checked statistically in pairs).
 *   - mWindPhase += mWindPhaseStep (0x40) a frame (the wind strength's slow swing, visual): cloth60_share (exact per
 *     30 Hz frame).
 *   - the demo warm-up (a demo frame 0 on the matching stage runs 60 cloth iterations in one frame): at dt < 1 (not
 *     expected: a demo runs the 30 Hz fallback) the frame's earlier steps hold after the wind phase's share, and its
 *     tick30 step runs the 60 iterations with dt 1 (the original frame, the phase taking the frame's rest).
 * setCorrectNrmAngle 023D1804 (inside the move): the sail's form (d_a_sail_t60.h): cM_rndF(200) once per 30 Hz frame on
 *   the tick30 step (the per-step chance rule; visual wobble only, no decision: the first-step AI-timer rule leaves it there), held for the frame's
 *   other steps; mNrmWobblePhase += rnd + 900 shared with the tick30 step taking the rest (sail60_wobbleAdd: exact at
 *   each frame end); mNrmCorrectAngle's cLib_addCalcAngleS2 is c_lib's integer form.
 * daPirate_Flag_Execute 023D2610: the flag's pose from the ship's model each step (no change); shape_angle.y's
 *   cLib_addCalcAngleS2 (8, 0x400) is c_lib's integer form.
 * Host memory: the sail helpers' held draw and wobble carry (d_a_pirate_flag.cpp statics, keyed by the packet). */
#pragma once
#include "d/actor/d_a_sail_t60.h"
#include "d/d_cloth_packet_t60.h"

/* the wind phase's per-frame add for this step (exact per 30 Hz frame) */
static inline s32 pirate60_share(s32 perFrame) { return t60::dt() == 1.0f ? perFrame : cloth60_share(perFrame); }

/* the demo warm-up frame (iterations > 1) at dt < 1: < 0 hold this step (an earlier step of the frame); > 0 the tick30
 * step runs the original frame at dt 1 (the previous dt, to put back with pirate60_warmEnd); 0 nothing to do */
static inline f32 pirate60_warm(s32 iterations) {
    if (t60::dt() == 1.0f || iterations == 1) {
        return 0.0f;
    }
    if (!t60::tick30()) {
        return -1.0f;
    }
    return t60_set_dt(1.0f);
}
static inline void pirate60_warmEnd(f32 previous) {
    if (previous > 0.0f) {
        t60_set_dt(previous);
    }
}

/* the step's share of the frame's force push (drag-consistent; the d_cloth_packet form): delta x g at dt < 1 */
static inline void pirate60_push(cXyz* delta, f32 drag) {
    if (t60::dt() == 1.0f) {
        return;
    }
    const f32 g = cloth60_push(drag);
    delta->x = (f32)delta->x * g;
    delta->y = (f32)delta->y * g;
    delta->z = (f32)delta->z * g;
}

/* pos += velocity (a frame) / velocity x dt (a step); the original PSVECAdd at dt 1 */
static inline void pirate60_move(void* pos, void* velocity) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        pirateVectorAdd(pos, velocity, pos);
        return;
    }
    cXyz* p = static_cast<cXyz*>(pos);
    const cXyz* v = static_cast<const cXyz*>(velocity);
    p->x = gabi::fmadds((f32)v->x, dt, (f32)p->x);
    p->y = gabi::fmadds((f32)v->y, dt, (f32)p->y);
    p->z = gabi::fmadds((f32)v->z, dt, (f32)p->z);
}
