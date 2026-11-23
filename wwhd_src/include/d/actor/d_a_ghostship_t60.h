/* d_a_ghostship_t60.h (WWHD, true 60 build): the Ghost Ship (d_a_ghostship.cpp) on a
 * general step length (dt = 1/N). Converted, the ship executes on every step with g_dt = 1/N; at dt = 1
 * (N = 1, the 30 Hz fallback, an unconverted ghost ship) every helper returns the original per-frame value, so the
 * original instructions run (dt-1 check bit-exact). Separate file: d_a_ghostship.cpp keeps thin calls.
 *
 * Sites (_execute 0214A9A4):
 *   - mAlpha's cLib_chaseF 0.02, the circles' radius cLib_addCalc2, the path move (dLib_pathMove's callback _pathMove:
 *     cLib_addCalcAngleS + cLib_chasePosXZ, its point switch below; pathMove's cLib_addCalc2 / cLib_addCalcPosXZ / cLib_addCalcAngleS2), the
 *     btk play and the two cloths (d_cloth_packet_t60.h): c_lib's / J3D's / the cloth's own dt forms, no change here.
 *   - the hum 0x107A started every frame while visible: once per 30 Hz frame, on the tick30 step (the one-emitter-per-event rule / sound-gate: one
 *     start per logical event, at the frame-end state the 30 Hz frame sees).
 *   - the 12 circles: each frame cM_rndF(20) sets mAngleSpeed, then dLib_setCirclePath adds it to mAngle and places
 *     the circle; the particle 0x8306 is set at the circle once a frame. Converted: the draws only on the tick30 step,
 *     in the original order and RNG position (the per-step chance rule: the draw count per 30 Hz frame stays the original's); on the other
 *     steps mAngleSpeed keeps the last frame's value (no draw) and the circle advances by dt of it (ghost60_share's
 *     carry, consecutive steps); on the tick30 step mAngle is first reset to its frame start (ghost60_start: the value
 *     at the frame's first call) and the new speed added whole, so each 30 Hz frame ends on the original angle. The
 *     circle's centre height (600 cos / sin of 2 x (mAngle + mAngleSpeed)) takes the same start + speed form on the
 *     tick30 step and the step's own angle + dt of the held speed on the others. The particle on the tick30 step only
 *     (the one-emitter-per-event rule).
 *   - the circles' radius targets (1500 + 100 i below radius 900, 800 above 1400 + 100 i): a threshold decision on the
 *     frame-start radius, so it is taken on the frame's first step and held (mPathRadiusTarget) on its other steps;
 *     taken per step it switched up to a step early, the radius turned short of the original's peak / trough and its
 *     oscillation drifted ahead every half cycle (gk37n2 before the fix: 10, 20, ... 50 units apart).
 *   - the path follower's point switch (_pathMove's returns: degenerate segment, within step x (threshold + 1), exactly
 *     on the point): a frame-end decision, so on the tick30 step only (in-between steps return 0 and keep chasing the
 *     same point; cLib_chasePosXZ's dt form stops on the point). Taken per step it switched up to a step early, and the
 *     ship turned the corner from a different place (8 units off for the rest of the path).
 *   - the sway (ghostMatrices' dLib_waveRot): its phase adds are d_lib's dt form (d_lib_t60.h).
 *   - the enter-ship check (event register, next stage, sound 0x2889) is latched by mbEnteredShip: once, on whichever
 *     step first meets it (no change).
 * Host memory only, keyed by the owner (the circle). */
#pragma once
#include "d/actor/d_a_ship_t60.h"
#include "d/d_lib.h"

static inline bool ghost60_tick30() { return ship60_tick30(); }
static inline bool ghost60_first() { return ship60_first(); }

/* per circle: the angle carry and the frame-start latch (the ghost ship is a single actor; keyed by the circle) */
static Ship60Carry s_ghost60Angle[12];
static Ship60Start s_ghost60AngleStart[12];

/* the angle the circle starts the call from and the speed dLib_setCirclePath adds: at dt 1 (mAngle, the frame's new
 * speed) unchanged; at dt < 1 on the tick30 step (frame start, the new speed whole), on the others (mAngle, dt of the
 * held speed, carried). `speed` is the value mAngleSpeed holds for this call. */
static inline s32 ghost60_circleAdd(s32 i, const dLib_circle_path_c* c, u32 frameId, s32 speed, s16* angleOut) {
    const s16 angle = (s16)c->mAngle;
    if (t60::dt() == 1.0f) {
        *angleOut = angle;
        return speed;
    }
    const s32 start = ship60_start(s_ghost60AngleStart[i], c, frameId, angle);
    if (t60::tick30()) {
        *angleOut = (s16)start;
        return speed;
    }
    *angleOut = angle;
    return ship60_share(s_ghost60Angle[i], c, speed);
}

/* the circle's centre phase (mAngle + mAngleSpeed before the frame's draw): at dt 1 and on the tick30 step the frame
 * start + the held speed (the original's value); on the other steps the step's angle + dt of the held speed (no carry
 * touched: a preview) */
static inline s32 ghost60_phase(s32 i, const dLib_circle_path_c* c, u32 frameId) {
    const s32 angle = (s16)c->mAngle, speed = (s16)c->mAngleSpeed;
    if (t60::dt() == 1.0f) {
        return angle + speed;
    }
    const s32 start = ship60_start(s_ghost60AngleStart[i], c, frameId, (s16)angle);
    if (t60::tick30()) {
        return (s32)(s16)start + speed;
    }
    return angle + (s32)((f32)speed * t60::dt());
}

/* frame-end recomputation (after the pair): the radius chase (cLib_addCalc2 0.1 / max 10) and the
 * path move (speedF's cLib_addCalc2, dLib_pathMove's chase of the path point, cLib_addCalcPosXZ2 of the position,
 * cLib_addCalcAngleS2 of the heading) are run on in-between steps in c_lib's dt forms, which are exact per frame only
 * inside one regime (the radius crossing the max-step / ratio boundary near its targets left 0.12 apart, which flipped
 * the next threshold decision a frame early: 20, 40 ... units later) and round per step at the ship's coordinates
 * (~3e5, ULP 0.03: the path point drifted ~0.03 a frame ahead and switched a frame early). So on the frame's tick30 step
 * the state is reset to its frame start (latched on the frame's first step) and the ORIGINAL frame runs at dt 1: each
 * 30 Hz frame ends bit-exact on the original's state; the in-between steps keep the dt forms (the drawn in-between
 * state). A latch begun after the frame's first step (the actor created or the mode entered mid-frame) or a skipped
 * step leaves that frame on the dt forms. */
struct Ghost60FrameStart {
    u32 frame, stamp, lastStep;
    const void* owner;
    bool valid, partial;
};
/* true on the tick30 step of a frame whose start was latched on its first step (then `restore` puts the start back);
 * latches on the first call of a frame (`save`) */
template <class Save, class Restore>
static inline bool ghost60_frameEnd(Ghost60FrameStart& s, const void* owner, u32 frameId, Save save, Restore restore) {
    if (t60::dt() == 1.0f) {
        return false;
    }
    const u32 now = t60_step_counter();
    if (!s.valid || s.owner != owner || !ship60_sameFrame(s.frame, s.stamp, frameId)) {
        s.frame = frameId;
        s.stamp = now;
        s.owner = owner;
        s.valid = true;
        s.partial = !t60::first_step();
        save();
    } else if (s.lastStep != now - 1) {
        s.partial = true;
    }
    s.lastStep = now;
    if (!t60::tick30() || s.partial) {
        return false;
    }
    restore();
    return true;
}

static Ghost60FrameStart s_ghost60RadiusStart[12];
static f32 s_ghost60Radius[12];
/* host memory: plain host values only (a host cXyz would read / write guest memory at ea(host address)) */
struct Ghost60PathState {
    f32 speedF, px, pz;
    Vec3f pathPos;
    s8 pntNo;
    s16 angleY, shapeY;
};
static Ghost60FrameStart s_ghost60PathStart;
static Ghost60PathState s_ghost60Path;
