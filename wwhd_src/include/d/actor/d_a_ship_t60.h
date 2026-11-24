/* d_a_ship_t60.h (WWHD, true 60 build): the King of Red Lions' execute on a general step length
 * (dt = 1/N). The ship (fpcNm_SHIP_e 0xA5) executes on every step with g_dt = 1/N once it is
 * converted; at dt = 1 (N = 1, the 30 Hz fallback, an unconverted ship) every helper here returns the original
 * per-frame value, so the original instructions run (dt-1 check bit-exact).
 *
 * Helpers (host memory only; the ship is one actor, each site keeps its own state, keyed by the actor):
 *   ship60_tick30  per-frame (30 Hz) work: on the tick30 step only (the 30 Hz timer rule: counters, per-frame draws, the per-step chance rule)
 *   ship60_first   the frame's first step (the first-step input rule: the frame's input; counters decremented before their setters)
 *   ship60_share   an integer per-frame add at a step of dt: dt of the amount per step, the fraction carried while
 *                    the site runs on consecutive steps (exact over a frame of constant amount; lk07_share's form)
 *   ship60_once    once per 30 Hz frame (the one-emitter-per-event rule: an emitter or sound start per logical event, never on two steps)
 *   ship60_hold    a per-frame change an unconverted driver makes on one step of the frame (a demo actor's
 *                    rotation, written on its tick30 step): held for a frame's steps from the step it was seen, then 0
 *   ship60_start   a value's frame start (latched on the first call in a 30 Hz frame): integer recurrences that end
 *                    each 30 Hz frame on the original step (the clamped-smoothing rule)
 * The 30 Hz frame id is g_Counter.mCounter0 (counts on tick30 only), passed in by the caller (the ship
 * reads it through its glued alias ship_counter). */
#pragma once
#include "true60.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

static inline bool ship60_tick30() { return t60::dt() == 1.0f || t60::tick30(); }
static inline bool ship60_first() { return t60::dt() == 1.0f || t60::first_step(); }

struct Ship60Carry {
    f32 acc;
    u32 stamp;
    const void* owner;
    bool valid;
};
static inline s32 ship60_share(Ship60Carry& c, const void* owner, s32 perFrame) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        return perFrame;
    }
    const u32 now = t60_step_counter();
    if (!c.valid || c.owner != owner || now - c.stamp != 1) {
        c.acc = 0.0f;
    }
    c.stamp = now;
    c.owner = owner;
    c.valid = true;
    c.acc += (f32)perFrame * dt;
    const s32 add = (s32)c.acc; /* toward zero; the rest carries */
    c.acc -= (f32)add;
    return add;
}

/* the rest of a frame's integer add when a step's decision ends the site's run early (the jump
 * during a turn): the site ran on this step (ship60_share gave dt of `perFrame`) and will not run on the frame's other
 * `restSteps` steps; returns what they would have added at the same amount, so the frame's total is the original
 * per-frame add (30 Hz: the frame turns by the whole amount before the decision). 0 at dt = 1 (no rest). */
static inline s32 ship60_shareRest(Ship60Carry& c, const void* owner, s32 perFrame, int restSteps) {
    const f32 dt = t60::dt();
    if (dt == 1.0f || restSteps <= 0 || !c.valid || c.owner != owner || c.stamp != t60_step_counter()) {
        return 0;
    }
    c.acc += (f32)perFrame * dt * (f32)restSteps;
    const s32 add = (s32)c.acc;
    c.acc -= (f32)add;
    return add;
}

/* the rest of a frame's integer add without taking it (the carry is not touched): what ship60_share will still add on
 * the frame's other `restSteps` steps at the same amount, when it ran on this step (0 otherwise, and at dt = 1). A
 * decision on the frame's first step reads the value the frame ends on (the animation-driven decision rule form). */
static inline s32 ship60_shareRestPeek(const Ship60Carry& c, const void* owner, s32 perFrame, int restSteps) {
    const f32 dt = t60::dt();
    if (dt == 1.0f || restSteps <= 0 || !c.valid || c.owner != owner || c.stamp != t60_step_counter()) {
        return 0;
    }
    return (s32)(c.acc + (f32)perFrame * dt * (f32)restSteps);
}

/* the same 30 Hz frame as a mark made at step counter `stamp` with frame id `frame`: the id matches and the steps are
 * fewer than a frame's apart (the frame id alone could repeat after a gap, e.g. across verification inputs) */
static inline bool ship60_sameFrame(u32 frame, u32 stamp, u32 frameId) {
    return frame == frameId && t60_step_counter() - stamp < t60::steps();
}
struct Ship60Once {
    u32 frame, stamp;
    const void* owner;
    bool valid;
};
static inline bool ship60_once(Ship60Once& o, const void* owner, u32 frameId) {
    if (t60::dt() == 1.0f) {
        return true;
    }
    if (o.valid && o.owner == owner && ship60_sameFrame(o.frame, o.stamp, frameId)) {
        return false;
    }
    o.frame = frameId;
    o.stamp = t60_step_counter();
    o.owner = owner;
    o.valid = true;
    return true;
}

/* a decision taken once per 30 Hz frame and held over its other steps (rule 2 2026-11-17, setSailAngle's wind
 * side; the animation-driven decision rule form): true on the frame's first step, or on the first call of a frame whose first step did not
 * reach the site (that body keeps the per-step form); false on the frame's later steps (the branch taken holds). */
struct Ship60Decide {
    u32 frame, stamp;
    const void* owner;
    bool valid;
};
static inline bool ship60_decide(Ship60Decide& d, const void* owner, u32 frameId) {
    if (t60::dt() == 1.0f) {
        return true;
    }
    if (!t60::first_step() && d.valid && d.owner == owner && ship60_sameFrame(d.frame, d.stamp, frameId)) {
        return false;
    }
    d.frame = frameId;
    d.stamp = t60_step_counter();
    d.owner = owner;
    d.valid = true;
    return true;
}

struct Ship60Start {
    u32 frame, stamp;
    const void* owner;
    s32 value;
    bool valid;
};
static inline s32 ship60_start(Ship60Start& s, const void* owner, u32 frameId, s32 now) {
    if (!s.valid || s.owner != owner || !ship60_sameFrame(s.frame, s.stamp, frameId)) {
        s.frame = frameId;
        s.stamp = t60_step_counter();
        s.owner = owner;
        s.value = now;
        s.valid = true;
    }
    return s.value;
}

struct Ship60Hold {
    s32 value;
    u32 stamp;
    const void* owner;
    bool valid;
};
static inline s32 ship60_hold(Ship60Hold& h, const void* owner, s32 now) {
    if (t60::dt() == 1.0f) {
        return now;
    }
    const u32 step = t60_step_counter();
    if (now != 0) {
        h.value = now;
        h.stamp = step;
        h.owner = owner;
        h.valid = true;
        return now;
    }
    if (h.valid && h.owner == owner && step - h.stamp < (u32)t60::steps()) {
        return h.value;
    }
    return 0;
}

/* setYPos's float spring step-consistent (rule (c), 2026-11-16). The 30 Hz frame map on the
 * state (y, v) (v = mFloatSpeedY after its damping), with a = 1 - 0.1 r, g = 0.1 r (r = rate), w = mWaterY, c = the bob
 * (0.6 + 0.25 r sin(phase Y)), in cLib_addCalc's proportional range (|v'| >= 1) and inside the clamps:
 *     y' = 0.95 a y + v + (g + 0.05 (1 - g)) w + c,     v' = -0.0475 a y + 0.95 v + 0.0475 (1 - g) w,
 * i.e. x' = M x + b with M = [[0.95 a, 1], [-0.0475 a, 0.95]] (complex eigenvalues, |l| = sqrt(0.95 a): a damped
 * oscillator) and fixed point x* = (I - M)^-1 b (det(I - M) = 0.05). Its discrete solution at a share s of the frame is
 * x(s) = x* + M^s (x0 - x*), M^s = rho^s (cos(s th) I + sin(s th) / sin(th) (M / rho - cos(th) I)).
 *   - the frame's tick30 step (FrameEnd) runs the ORIGINAL frame (its f32 instructions, cLib_addCalc at dt 1, the
 *     clamps) from the frame-start state: the frame start y0 is the y the first step's call found plus every outside
 *     move of y between the frame's calls (posMove's dt shares of speed.y, CrrPos), v0 the frame-start float speed. So
 *     each 30 Hz frame ends on the original's state with the original's water, rate and sway draw, nonlinear parts
 *     (the +-20 clamp, the addCalc minimum step 0.05 below |v'| 1, the y clamps to the water -60 / +40) included.
 *   - the frame's other steps (Share) set x(k / N) from the frame start with the step's water, rate and bob (the
 *     linear map; then the y clamps on the step's water): the in-between state other code reads (Link's carrier, the
 *     crane's lift test), continuous into the frame end up to the water's change within the frame.
 *   - a spring that begins after the frame's first step, or a call that is not on the step after the previous one
 *     (Partial): the per-step form for the rest of that frame.
 * The semi-implicit Euler at dt 1 / N it replaces lagged the moving water by a different amount (the crane pairs: ship
 * y 3.3 apart at N 2, 5.0 at N 4; Session 14, tools/spring.py). */
enum class Ship60FloatPath { FrameEnd, Share, Partial };
struct Ship60Float {
    u32 frame, stamp, lastStep;
    const void* owner;
    f32 base, v0, lastExit;
    bool valid, partial;
};
/* exactY (the sailing's frame form): this step's y came from the frame's exact move (t60c::moveEnd: the
 * frame-start y + the move so far), so cy itself is the frame start plus every outside move: the base is cy */
static inline Ship60FloatPath ship60_floatEnter(Ship60Float& f, const void* owner, u32 frameId, f32 cy, f32 v,
                                                bool exactY = false) {
    const u32 now = t60_step_counter();
    if (!f.valid || f.owner != owner || !ship60_sameFrame(f.frame, f.stamp, frameId)) {
        f.frame = frameId;
        f.stamp = now;
        f.owner = owner;
        f.base = cy;
        f.v0 = v;
        f.valid = true;
        f.partial = !t60::first_step();
    } else {
        if (f.lastStep != now - 1) {
            f.partial = true;
        }
        if (exactY) {
            f.base = cy;
        } else {
            f.base += cy - f.lastExit; /* y moved by others since the previous call */
        }
    }
    f.lastStep = now;
    if (f.partial) {
        return Ship60FloatPath::Partial;
    }
    return t60::tick30() ? Ship60FloatPath::FrameEnd : Ship60FloatPath::Share;
}
static inline double ship60_floatShareOf(const Ship60Float& f) {
    const double k = (double)(t60_step_counter() - f.stamp + 1), n = (double)t60::steps();
    return k < n ? k / n : 1.0;
}
static inline void ship60_floatShare(f32 y0, f32 v0, f32 w, f32 r, f32 c, double s, f32* yOut, f32* vOut) {
    const double a = 1.0 - 0.1 * (double)r, g = 0.1 * (double)r;
    const double m00 = 0.95 * a, m01 = 1.0, m10 = -0.0475 * a, m11 = 0.95;
    const double b0 = (g + 0.05 * (1.0 - g)) * (double)w + (double)c, b1 = 0.0475 * (1.0 - g) * (double)w;
    const double ys = 20.0 * (0.05 * b0 + b1), vs = 20.0 * (-0.0475 * a * b0 + (1.0 - 0.95 * a) * b1);
    const double dy = (double)y0 - ys, dv = (double)v0 - vs;
    const double tr = m00 + m11, det = m00 * m11 - m01 * m10, disc = tr * tr / 4.0 - det;
    double p00, p01, p10, p11;
    if (disc < 0.0) {
        const double rho = std::sqrt(det), ct = tr / (2.0 * rho), th = std::acos(ct), st = std::sin(th);
        const double ps = std::pow(rho, s), cs = std::cos(s * th), ss = std::sin(s * th) / st;
        p00 = ps * (cs + ss * (m00 / rho - ct));
        p01 = ps * (ss * m01 / rho);
        p10 = ps * (ss * m10 / rho);
        p11 = ps * (cs + ss * (m11 / rho - ct));
    } else { /* real eigenvalues (not reached for rate in [0, 1]; kept general) */
        const double q = std::sqrt(disc), l1 = tr / 2.0 + q, l2 = tr / 2.0 - q;
        const double e1 = std::pow(l1, s), e2 = std::pow(l2 > 0.0 ? l2 : 0.0, s), dl = l1 - l2;
        if (dl == 0.0) {
            p00 = p11 = e1;
            p01 = p10 = 0.0;
        } else {
            p00 = (e1 * (m00 - l2) - e2 * (m00 - l1)) / dl;
            p01 = (e1 - e2) * m01 / dl;
            p10 = (e1 - e2) * m10 / dl;
            p11 = (e1 * (m11 - l2) - e2 * (m11 - l1)) / dl;
        }
    }
    *yOut = (f32)(ys + p00 * dy + p01 * dv);
    *vOut = (f32)(vs + p10 * dy + p11 * dv);
}

/* procCraneUp's swing decision (q6bv; the first-step AI-timer rule / animation-driven decision). The original pulls the lean spring
 * toward the swing target (setControllAngle) and then, on the frame-end lean, decides once a frame: within 0x400 of the
 * target it draws a new target (cM_rnd) and may start the swing sound 0x381E; the new target pulls from the next frame.
 * Per step, the test could cross on an in-between step (a draw and a target change mid-frame, or two in one frame).
 * Here the frame's FIRST step decides (the draw and the sound on that step, the first-step AI-timer rule) against the original's frame-end
 * lean projected from the frame start (one dt-1 setControllAngle: speed + the pull, the lean + the speed; the damping
 * comes after the lean's add), and the new target is held until the frame's tick30 step (after its pull) or until the
 * procedure leaves on an earlier step, so the frame's steps pull toward the old target as the original frame does.
 * A procedure that first runs on a later step of a frame decides on that frame's tick30 step on its own lean. */
enum class Ship60SwayPath { Decide, Hold, Late };
/* the wave rocking, frame-anchored (2026-11-17, talk6: the converted ship's roll 50-120 s16 off A's
 * for the whole voyage, the eye point above the hull moved sideways, talktoCamera latched its yaw from it). At 30 Hz
 * the frame adds the sway to the wave angles (setYPos: x += 30 + 100 r sin(phase X), z += 35 + 115 r sin(phase Z), from
 * the frame's new random phases) and then springs them toward the water slope (setWaveAngle: v = ftoi(0.045 (slope -
 * x) + v), x += v, v damped by cLib_addCalcAngleS(v, 0, 20, 0x1000, 4)). The per-step form (each add dt of itself,
 * semi-implicit) is a different oscillator (B's roll a few frames behind A's) and sampled the sway at the mean-phase
 * steps. Now both are one frame map from the frame-start state (the wave angles and speeds the frame's first call
 * found, plus every outside change between the frame's calls, e.g. setCrashData's kick): the tick30 step runs the
 * original frame (the frame's random phases, the step's slope, the damping at dt 1); the in-between steps set the
 * frame start plus k / N of the frame's change predicted from the step's phases and slope. A frame whose first step did
 * not reach the site, or a step where setYPos's sway did not run before setWaveAngle: the per-step form. */
struct Ship60Wave {
    u32 frame, stamp, lastStep;
    const void* owner;
    s16 x0, z0, vx0, vz0;         /* the frame start (+ outside changes) */
    s16 lastX, lastZ, lastVX, lastVZ;
    s16 swayX, swayZ;             /* the frame's sway add (from the frame start, the step's phases) */
    bool valid, partial;
};
/* setYPos's sway: true when the frame-anchored form runs (false: the per-step form) */
static inline bool ship60_waveEnter(Ship60Wave& w, const void* owner, u32 frameId, s16 x, s16 z, s16 vx, s16 vz) {
    const u32 now = t60_step_counter();
    if (!w.valid || w.owner != owner || !ship60_sameFrame(w.frame, w.stamp, frameId)) {
        w.frame = frameId;
        w.stamp = now;
        w.owner = owner;
        w.x0 = x;
        w.z0 = z;
        w.vx0 = vx;
        w.vz0 = vz;
        w.valid = true;
        w.partial = !t60::first_step();
    } else {
        if (w.lastStep != now - 1) {
            w.partial = true;
        }
        /* changed by others since the previous call */
        w.x0 = (s16)(w.x0 + (s16)(x - w.lastX));
        w.z0 = (s16)(w.z0 + (s16)(z - w.lastZ));
        w.vx0 = (s16)(w.vx0 + (s16)(vx - w.lastVX));
        w.vz0 = (s16)(w.vz0 + (s16)(vz - w.lastVZ));
    }
    w.lastStep = now;
    return !w.partial;
}
static inline void ship60_waveMark(Ship60Wave& w, s16 x, s16 z, s16 vx, s16 vz) {
    w.lastX = x;
    w.lastZ = z;
    w.lastVX = vx;
    w.lastVZ = vz;
}
/* setWaveAngle: the anchored form runs on this step (setYPos's sway ran on it, in an anchored frame) */
static inline bool ship60_waveAnchored(const Ship60Wave& w, const void* owner) {
    return w.valid && w.owner == owner && !w.partial && w.lastStep == t60_step_counter();
}
/* the share of the frame done after this step (k / N; 1 on the tick30 step) */
static inline double ship60_waveShareOf(const Ship60Wave& w) {
    if (t60::tick30()) {
        return 1.0;
    }
    const double k = (double)(t60_step_counter() - w.stamp + 1), n = (double)t60::steps();
    return k < n ? k / n : 1.0;
}
static inline s16 ship60_waveLerp(s16 a, s16 b, double s) {
    if (s >= 1.0) {
        return b;
    }
    return (s16)(a + (s32)std::lround(s * (double)(s16)(b - a)));
}
/* cLib_addCalcAngleS(&v, 0, 0x14, 0x1000, 4) at dt 1 (the original's integer path, c_lib.cpp) on a value */
static inline s16 ship60_waveDamp(s16 v) {
    s32 x = v;
    s16 diff = (s16)(0 - x);
    if (x == 0) {
        return 0;
    }
    s32 step = (s16)(diff / 0x14);
    if (step > 4 || step < -4) {
        if (step > 0x1000) {
            step = 0x1000;
        }
        if (step < -0x1000) {
            step = -0x1000;
        }
        return (s16)(x + step);
    }
    if (diff >= 0) {
        x = (s16)(x + 4);
        if ((s16)(0 - x) > 0) {
            return (s16)x;
        }
    } else {
        x = (s16)(x - 4);
        if ((s16)(0 - x) < 0) {
            return (s16)x;
        }
    }
    return 0;
}

/* the lean spring (setControllAngle: speed += max(1, 0.05 (angle - lean)) signed, lean += speed, speed damped by
 * cLib_addCalcAngleS(speed, 0, 20, 0x1000, 4)), frame-anchored as the wave rocking (2026-11-17, talk7: the
 * per-step form left B's lean about a frame behind A's in turns, the roll with it): the frame's calls run the original
 * frame from the frame-start lean / speed (the first call's values plus outside changes) toward the step's angle, the
 * tick30 call to the end, the others k / N of it. A frame whose first step did not reach the site, a second call in a
 * step, or a gap: the per-step form for the rest of the frame. */
struct Ship60Lean {
    u32 frame, stamp, lastStep;
    const void* owner;
    s16 c0, s0, lastC, lastS;
    bool valid, partial;
};
static inline bool ship60_leanEnter(Ship60Lean& l, const void* owner, u32 frameId, s16 c, s16 sp) {
    const u32 now = t60_step_counter();
    if (!l.valid || l.owner != owner || !ship60_sameFrame(l.frame, l.stamp, frameId)) {
        l.frame = frameId;
        l.stamp = now;
        l.owner = owner;
        l.c0 = c;
        l.s0 = sp;
        l.valid = true;
        l.partial = !t60::first_step();
    } else {
        if (l.lastStep != now - 1) {
            l.partial = true;
        }
        l.c0 = (s16)(l.c0 + (s16)(c - l.lastC));
        l.s0 = (s16)(l.s0 + (s16)(sp - l.lastS));
    }
    l.lastStep = now;
    return !l.partial;
}
static inline double ship60_leanShareOf(const Ship60Lean& l) {
    if (t60::tick30()) {
        return 1.0;
    }
    const double k = (double)(t60_step_counter() - l.stamp + 1), n = (double)t60::steps();
    return k < n ? k / n : 1.0;
}

struct Ship60Sway {
    u32 frame, stamp;
    const void* owner;
    s16 target;
    bool valid, late, pending;
};
static inline Ship60SwayPath ship60_swayEnter(Ship60Sway& s, const void* owner, u32 frameId) {
    /* a first step always opens the frame (a record left by an earlier frame with the same id never holds it) */
    if (t60::first_step() || !s.valid || s.owner != owner || !ship60_sameFrame(s.frame, s.stamp, frameId)) {
        s.frame = frameId;
        s.stamp = t60_step_counter();
        s.owner = owner;
        s.valid = true;
        s.pending = false;
        s.late = !t60::first_step();
        return s.late ? Ship60SwayPath::Late : Ship60SwayPath::Decide;
    }
    return s.late ? Ship60SwayPath::Late : Ship60SwayPath::Hold;
}
/* called at the procedure's top on every step: a first step drops any held target (it belongs to an earlier frame; the
 * frame's own decision comes later on this step), also when this frame takes the other branch and never decides */
static inline void ship60_swayStep(Ship60Sway& s) {
    if (t60::dt() != 1.0f && t60::first_step()) {
        s.pending = false;
    }
}
/* the held target, once: on the frame's tick30 step or when the procedure ends early; false when none is due */
static inline bool ship60_swayTake(Ship60Sway& s, const void* owner, u32 frameId, s16* target) {
    if (!s.valid || !s.pending || s.owner != owner || !ship60_sameFrame(s.frame, s.stamp, frameId)) {
        return false;
    }
    s.pending = false;
    *target = s.target;
    return true;
}

/* procTalk's talk-animation switch (the animation-driven decision rule, animation-driven decisions; 2026-11-17).
 * The original plays the talk animation (execute: J3DFrameCtrl::update before the dispatch) and then, when that play
 * reached the animation's end (checkState(2)), draws cM_rndF(2) for TALK_A / TALK_B and re-inits the frame control if
 * the file changes: once a frame, on the frame-end state; the new animation plays from the next frame.
 * At N steps the frame's FIRST step decides whether the frame's plays reach the end (its own play, or one of the
 * frame's remaining N - 1 plays run on a guest-stack copy through the real J3DFrameCtrl::update 027F2FC4 at this dt):
 * the draw on that step (the per-step chance rule: once a frame), a new file held (Ship60Sway, target = the file index) and applied on
 * the frame's tick30 step after its play, so the frame's steps play the old animation to the frame end as the
 * original frame does. Later steps hold (no second decision). A body that first runs on a later step of the frame
 * keeps the per-step form (the original runs no body of it in that frame). dt 1: the original test. */
static __attribute__((noinline)) bool ship60_anmEndWithin(J3DFrameCtrl* fc, int plays) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    for (int i = 0; i < plays; i++) {
        gabi::call(0x027F2FC4, c.get());
        if (c->checkState(2)) {
            return true;
        }
    }
    return false;
}

/* The ship's animation sets (the split-actor state rule (3) set-latch class, octo10 / octo17 camera
 * latitude: the cannon part-on animation half a frame ahead at N 2, the rider camera's latitude follows mCannonAngleX =
 * getAnglePartRate x 0x4000). daShip_c::execute plays mpBodyAnm and mpHeadAnm (mDoExt_McaMorf::play) BEFORE the
 * procedure dispatch, setHeadAnm runs after it: at 30 Hz an animation set (setAnm, a reversal setPlaySpeed(-1), the
 * cannon shot's head restart) first plays in the NEXT frame's update. At N steps a set made on a step other than the
 * frame's tick30 step played on the frame's remaining steps, (N - k) / N frame early (the cannon part-on: B's mast at
 * 621 at frame 5.5 against A's 5, mCannonAngleX 2730 against 0, the camera latitude 0.3 x that).
 * Form (Link's lkT60_markAnmSet): such a set is latched (the shared port set latch, keyed by
 * the morf, tagged with the 30 Hz frame counter); execute skips that morf's play for the rest of the frame; it first
 * plays on the next frame's first step. A set on the tick30 step is not latched (its next play is the next frame's
 * first step anyway). A set that CARRIES the running phase (setPartAnimeInit's remaining ratio of the old mast
 * animation, a reversal) takes the phase the original has after the frame's whole play: the old frame control
 * projected over the frame's remaining plays (ship60_anmRest, ship60_projectFrame; none when the old animation itself
 * waits for its first play). dt 1: never latched, nothing projected (the original). */
static inline void ship60_markAnm(const void* morf, u32 frameId) {
    if (t60::dt() == 1.0f || t60::tick30()) {
        return;
    }
    wwhd_t60_latch_set(gabi::ea(morf), frameId);
}
static inline bool ship60_anmPending(const void* morf, u32 frameId) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(morf), frameId) != 0;
}
/* the frame's plays still to come after this step's. execute (d_a_ship_exec.cpp, before its plays) notes them in the
 * shared latch (key: an odd address inside the ship for each count r, value: the step counter of the step); the
 * procedures (d_a_ship.cpp, another unit) read them back. -1: no note on this step (the ship did not execute it). */
static inline u32 ship60_restKey(const void* ship, int r) { return gabi::ea(ship) + 1u + 2u * (u32)r; }
static inline void ship60_noteRest(const void* ship, int r) {
    if (t60::dt() == 1.0f || r < 0) {
        return;
    }
    wwhd_t60_latch_set(ship60_restKey(ship, r), t60_step_counter());
}
static inline int ship60_anmRest(const void* ship) {
    if (t60::dt() == 1.0f) {
        return 0;
    }
    for (int r = 0; r < (int)t60::steps(); r++) {
        if (wwhd_t60_latch_pending(ship60_restKey(ship, r), t60_step_counter())) {
            return r;
        }
    }
    return -1;
}
/* the frame of a guest-stack copy of fc after `plays` more updates (J3DFrameCtrl::update 027F2FC4 at this dt) */
static __attribute__((noinline)) f32 ship60_projectFrame(J3DFrameCtrl* fc, int plays) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    for (int i = 0; i < plays; i++) {
        gabi::call(0x027F2FC4, c.get());
    }
    return c->getFrame();
}
/* morf's frame as the original reads it after the frame's play: projected over the frame's remaining plays on a step
 * before the tick30 step (unless morf waits for its first play); the frame itself otherwise (dt 1: the plain read) */
template <class M>
static inline f32 ship60_frameEndFrame(M* morf, const void* ship, u32 frameId) {
    if (t60::dt() == 1.0f || t60::tick30() || ship60_anmPending(morf, frameId)) {
        return morf->getFrame();
    }
    int plays = ship60_anmRest(ship);
    if (plays <= 0) {
        return morf->getFrame();
    }
    return ship60_projectFrame(&morf->mFrameCtrl, plays);
}
/* a reversal (setPlaySpeed(-1)) made after the frame's play: on a step before the tick30 step the frame control first
 * runs the frame's remaining plays forward (the original reverses after the frame's whole play), then turns and is
 * latched (first played backwards on the next frame's first step). dt 1: setPlaySpeed(-1) alone. */
template <class M>
static inline void ship60_reverseAnm(M* morf, const void* ship, u32 frameId) {
    if (t60::dt() != 1.0f && !t60::tick30() && !ship60_anmPending(morf, frameId)) {
        int plays = ship60_anmRest(ship);
        for (int i = 0; i < plays; i++) {
            gabi::call(0x027F2FC4, &morf->mFrameCtrl);
        }
    }
    morf->setPlaySpeed(-1.0f);
    ship60_markAnm(morf, frameId);
}

/* The animation request form (form "B"; Moblin request form the
 * Hyoi seagull). The original's sets follow the frame's whole play (execute plays both morfs BEFORE the
 * dispatch), so for the rest of that frame the pose is the OLD animation's and the new one first plays next frame. A
 * set latched on a step before the tick30 step (ship60_markAnm above, unchanged: every logic read after it - the
 * same step's and the frame's later steps', d_a_ship.cpp's ~20 of them, getPlaySpeed / getFrame / ship60_endRate ... -
 * sees the REQUESTED words in the morf at once) is a request: the morf's old words are swapped in only around the
 * pose play (execute's plays) and the calc of the frame's later steps, and on the tick30 step, right after the old
 * animation's last play of the frame, the requested words go back for good (the redo: setAnm's words are mpAnm, the
 * frame control and the morf ratio words; its setMorf result depends only on the sign of mPrevMorf, which no play
 * changes, and setPartAnimeInit's setFrame / a reversal's rate are in the requested frame control). The blend then
 * starts from the old animation's pose of the last calc (mpTransformInfo), as in the Hyoi seagull's form.
 * Execute's unit only (true60.h: no host state across units): the old words are the morf's words after this step's
 * play, before the dispatch (ship60_snapAnm); the request is made from them the first time execute sees the latch
 * (the set's step's calc, or the next step's play). The words: +0x94 mpAnm, +0x98 mFrameCtrl (0x10), +0xB0 mCurMorf /
 * mPrevMorf / mMorfStep (copied as bytes: guest big-endian words). No latch this frame: nothing swapped, the plain
 * play / calc (dt 1: never). SHIP60_ANMREQ_NEGCTRL: no swap (the latch form's pose: the requested animation's). */
struct Ship60MorfWords {
    u8 a[0x14]; /* +0x94 mpAnm, +0x98 mFrameCtrl */
    u8 m[0x0C]; /* +0xB0 mCurMorf, +0xB4 mPrevMorf, +0xB8 mMorfStep */
};
struct Ship60AnmReq {
    Ship60MorfWords snap; /* the morf's words after this step's play (before the dispatch) */
    u32 snapFrame;
    bool snapValid;
    Ship60MorfWords other; /* a request's old words (requested while swapped in) */
    const void* morf;
    u32 frameId;
    bool active;
    u32 doneFrame; /* the frame whose request was put back on its tick30 step */
    bool doneValid;
};
template <class M> static inline u8* ship60_mw(M* m, u32 off) { return reinterpret_cast<u8*>(m) + off; }
template <class M> static inline void ship60_wordsGet(M* m, Ship60MorfWords& w) {
    std::memcpy(w.a, ship60_mw(m, 0x94), sizeof(w.a));
    std::memcpy(w.m, ship60_mw(m, 0xB0), sizeof(w.m));
}
template <class M> static inline void ship60_wordsPut(M* m, const Ship60MorfWords& w) {
    std::memcpy(ship60_mw(m, 0x94), w.a, sizeof(w.a));
    std::memcpy(ship60_mw(m, 0xB0), w.m, sizeof(w.m));
}
template <class M> static inline void ship60_wordsSwap(M* m, Ship60MorfWords& w) {
    Ship60MorfWords t;
    ship60_wordsGet(m, t);
    ship60_wordsPut(m, w);
    w = t;
}
/* morf's request of frame frameId (made from the snapshot the first time it is seen), or null */
template <class M>
static inline Ship60AnmReq* ship60_req(Ship60AnmReq& r, M* morf, u32 frameId) {
#ifdef SHIP60_ANMREQ_NEGCTRL
    (void)r; (void)morf; (void)frameId;
    return nullptr;
#else
    if (t60::dt() == 1.0f) {
        return nullptr;
    }
    if (r.active && r.morf == morf && r.frameId == frameId) {
        return &r;
    }
    if ((r.doneValid && r.doneFrame == frameId && r.morf == morf) || !ship60_anmPending(morf, frameId) ||
        !(r.snapValid && r.snapFrame == frameId)) {
        return nullptr; /* no request, already put back, or no old words of this frame (the latch form) */
    }
    r.other = r.snap;
    r.morf = morf;
    r.frameId = frameId;
    r.active = true;
    return &r;
#endif
}
/* execute, after its plays, before the dispatch: the old words a set of this step would replace */
template <class M>
static inline void ship60_snapAnm(Ship60AnmReq& r, M* morf, u32 frameId) {
    if (t60::dt() == 1.0f || (r.active && r.morf == morf && r.frameId == frameId)) {
        return;
    }
    ship60_wordsGet(morf, r.snap);
    r.snapFrame = frameId;
    r.snapValid = true;
}
/* execute's play: a request's old animation plays on (the pose); on the tick30 step after that play the requested
 * words go back (first played next frame). A latch without a request: no play (the latch form). */
template <class M>
static inline void ship60_playAnm(Ship60AnmReq& r, M* morf, u32 frameId) {
    Ship60AnmReq* q = ship60_req(r, morf, frameId);
    if (q == nullptr) {
        if (!ship60_anmPending(morf, frameId)) {
            morf->play(nullptr, 0, 0);
        }
        return;
    }
    ship60_wordsSwap(morf, q->other); /* old in */
    morf->play(nullptr, 0, 0);
    if (t60::tick30()) {
        ship60_wordsPut(morf, q->other); /* the requested words, for good */
        q->active = false;
        q->doneFrame = frameId;
        q->doneValid = true;
        return;
    }
    ship60_wordsSwap(morf, q->other); /* requested back in; other = the old words as played */
}
/* execute's calc: a request's pose is the old animation's */
template <class M>
static inline void ship60_calcAnm(Ship60AnmReq& r, M* morf, u32 frameId) {
    Ship60AnmReq* q = ship60_req(r, morf, frameId);
    if (q == nullptr) {
        morf->calc();
        return;
    }
    ship60_wordsSwap(morf, q->other);
    morf->calc();
    ship60_wordsSwap(morf, q->other);
}
/* the body bck index the pose shows (bodyJointCallBack's cannon / crane scale, setRopePos's mast scale): the morf's
 * animation (the old one while a request's old words are swapped in), dt 1: mBodyAnmFileIdx (the original read) */
template <class M>
static inline u16 ship60_poseBodyIdx(M* morf, u16 fileIdx, const void* resOn2, u16 idxOn2, const void* resOff2,
                                     u16 idxOff2) {
    if (t60::dt() == 1.0f) {
        return fileIdx;
    }
    const void* a = morf->getAnm();
    return a == resOn2 ? idxOn2 : a == resOff2 ? idxOff2 : fileIdx;
}

/* The mast animation's decisions (the animation-driven decision rule, Link's lkT60_endRate / lkT60_pass form, d_a_player_anm_t60.h): with
 * the sets latched (above) the mast's frames line up with the frame ends, and the procedures' tests on them decide once
 * a frame on the frame-end state as the original does after its play (execute plays before the dispatch):
 *   ship60_frameEndFrame   `getFrame() < 9` (sail on, jump allowed, part put away): the frame-end frame on any step;
 *   ship60_endRate         `getPlaySpeed() < 0.01` (the mast stopped: next part animation, cannon / crane ready): the
 *                          frame control's rate at the frame end (the frame's remaining plays on a copy);
 *   ship60_passEnd         `checkFrame(9)` (checkPass lookahead: the sail's speed kick): on the frame's first step from
 *                          the frame-end copy over one whole frame (rate x N), false on the frame's later steps.
 * Without them the decisions fell on the tick30 step, (N - 1) / N frame late (the sail-up kick at 56 in: B's
 * speed on one step of two, xz 32 behind). A step with no execute note (the ship did not execute it) keeps the
 * per-step form. dt 1: the plain getPlaySpeed() / checkFrame(). */
static __attribute__((noinline)) f32 ship60_projectRate(J3DFrameCtrl* fc, int plays) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    for (int i = 0; i < plays; i++) {
        gabi::call(0x027F2FC4, c.get());
    }
    return c->getRate();
}
template <class M>
static inline f32 ship60_endRate(M* morf, const void* ship, u32 frameId) {
    if (t60::dt() == 1.0f || t60::tick30() || ship60_anmPending(morf, frameId)) {
        return morf->getPlaySpeed();
    }
    int plays = ship60_anmRest(ship);
    if (plays <= 0) {
        return morf->getPlaySpeed();
    }
    return ship60_projectRate(&morf->mFrameCtrl, plays);
}
static __attribute__((noinline)) BOOL ship60_projectPass(J3DFrameCtrl* fc, int plays, f32 frame) {
    gabi::Local<J3DFrameCtrl> c;
    *c.get() = *fc;
    for (int i = 0; i < plays; i++) {
        gabi::call(0x027F2FC4, c.get());
    }
    /* one whole frame from the frame end: the guest checkPass looks rate x dt ahead (N a power of two: exact) */
    c->setRate(c->getRate() * (f32)t60::steps());
    return gabi::call<BOOL>(0x027F2BF8, c.get(), frame);
}
template <class M>
static inline BOOL ship60_passEnd(M* morf, const void* ship, u32 frameId, f32 frame) {
    if (t60::dt() == 1.0f) {
        return morf->checkFrame(frame);
    }
    int plays = ship60_anmRest(ship);
    if (plays < 0) {
        return morf->checkFrame(frame); /* no execute note on this step: the per-step form */
    }
    if (!t60::first_step()) {
        return FALSE;
    }
    return ship60_projectPass(&morf->mFrameCtrl, ship60_anmPending(morf, frameId) ? 0 : plays, frame);
}

/* KoRL's eye (sv4 frame 776; the process-order read rule style frame-start read): execute
 * takes eyePos (and from it attention_info.position, the head-look vectors, the TALK camera's speaker position) from the
 * head joint J_FN_ME_L's matrix of the model's previous calc. At 30 Hz that is the previous frame's end. At N > 1 the
 * matrix is recalculated after every step, so a later step (the tick30 step, where the event camera reads it) would see
 * the head half a frame further turned. So at dt < 1 the translation is latched on the frame's first step (or the first
 * step of the frame that reaches the site) and every step of the frame uses it: the frame-start eye, A's view. dt 1: the
 * original copy (no host state consulted). The words are copied as words (signalling NaN bits kept). */
struct Ship60Eye {
    Ship60Decide d;
    u32 w[3];
};
static inline void ship60_eyePos(Ship60Eye& e, const void* owner, u32 frameId, Mtx34* m, cXyz* out) {
    be<u32>* dst = reinterpret_cast<be<u32>*>(out);
    const be<u32>* r0 = reinterpret_cast<const be<u32>*>(&m->m[0][3]);
    const be<u32>* r1 = reinterpret_cast<const be<u32>*>(&m->m[1][3]);
    const be<u32>* r2 = reinterpret_cast<const be<u32>*>(&m->m[2][3]);
    if (t60::dt() == 1.0f) {
        dst[0] = *r0, dst[1] = *r1, dst[2] = *r2;
        return;
    }
#ifndef SHIP60_FRAME_NEGCTRL
    /* the sailing's frame form (the process-order read rule): the head joint after this step's calc,
     * as A reads it after its frame's calc (sqPm record 31: A's eye y 116.12 = the frame-end head; the first-step latch
     * gave 118.14, the head after the frame's first step). The tick30 step's is A's; daShip_Execute publishes it (the
     * earlier steps' eyePos is put back to the previous frame's after them) */
    (void)e, (void)owner, (void)frameId;
    dst[0] = *r0, dst[1] = *r1, dst[2] = *r2;
    return;
#endif
#ifndef SHIP60_EYE_NEGCTRL
    if (ship60_decide(e.d, owner, frameId)) {
        e.w[0] = r0->get(), e.w[1] = r1->get(), e.w[2] = r2->get();
    }
    dst[0] = e.w[0], dst[1] = e.w[1], dst[2] = e.w[2];
#else /* negative control: the per-step read of the latest matrix */
    dst[0] = *r0, dst[1] = *r1, dst[2] = *r2;
#endif
}

/* Link as the ship sees him (the process-order read rule): the ship (0xA5) executes before Link (0xA8) in the
 * process line, so at 30 Hz its frame reads Link's previous-frame-end state. At N steps its tick30 step would read Link
 * after his first step of the frame (half a frame on). ship60_fsActor: true and the 12 bytes of `a`'s frame-start
 * position (what 1) or eye (what 0) when the process-order read rule gives the frame-start state (a converted actor later in the line that
 * has not executed on this step); false otherwise (unconverted, earlier in the line, N = 1), and the caller keeps the
 * original read (dt-1 check bit-exact). The bytes are copied as bytes (signalling NaN bits kept). */
#include "f_pc/f_pc_t60_frame_start.h"
#include <cstring>
static inline bool ship60_fsActor(const void* a, int what, cXyz* out) {
    uint8_t fs[12];
    if (!t60_fs_read(gabi::ea(a), what, fs))
        return false;
    std::memcpy(out, fs, 12);
    return true;
}

/* Link's mode request (the process-order read rule, generic form; fst pair): Link (0xA8, after the ship
 * in the line) asks the ship for a mode by writing its mNextMode (setSteerMove / setPaddleMove / setReadyFirst /
 * setTactWarp, d_a_player_main_02/05/07); the ship's execute acts on it (checkNextMode) and ends with mNextMode =
 * mCurMode. At 30 Hz a request Link writes in frame f is seen by the ship in f + 1. At N steps Link writes it on his
 * first step (the A press, the first-step input rule) and the ship's tick30 step of the same frame took it: the sail-up steer mode, the
 * mast animation, SAIL_ON and the launch one frame early (mNextMode 1 on B 3820 tick30 = A 3823, A switches
 * at 3824; the ship's 'start' early ship-decision shift B 280 / A 281). Form: the value the ship left (mNextMode after its execute) is
 * remembered; on a step that is not the frame's first, a different value is a request written in this frame by an
 * actor later in the line: it is put aside and the ship runs the step with its own value; on the next frame's first
 * step a request put aside comes back (unless a newer request is there). dt 1: nothing (the original). Host memory. */
struct Ship60NextMode {
    const void* owner;
    u8 left;     /* mNextMode as the ship left it after its last execute */
    u8 pending;  /* a request put aside this frame */
    u8 value;
};
static inline void ship60_nextModeEnter(Ship60NextMode& m, const void* owner, be<u8>* next) {
    if (t60::dt() == 1.0f || m.owner != owner) {
        return;
    }
    const u8 v = *next;
    if (t60::first_step()) {
        if (m.pending && v == m.left) {
            *next = m.value;
        }
        m.pending = 0;
        return;
    }
    if (v != m.left) {
        m.pending = 1;
        m.value = v;
        *next = m.left;
    }
}
static inline void ship60_nextModeLeave(Ship60NextMode& m, const void* owner, be<u8>* next) {
    if (t60::dt() == 1.0f) {
        return;
    }
    if (m.owner != owner) {
        m.owner = owner;
        m.pending = 0;
    }
    m.left = *next;
}

/* ---- The sailing's frame form (approved in the kamome form). sqPm (pship
 * route) showed the per-step form's own drift from the first converted frame: the speed chase and the move interleaved per
 * step (the sail launch: A moves the frame at the frame-end speed 48.5 x cos of the frame-start pitch, z +48.451; B two half
 * steps at 49.0 / 48.5, z +48.705), x ~ -325000 rounding two half moves, old.pos / mOldShapeAngle / eyePos holding a
 * step's values where A's hold the frame's. The form, at dt != 1:
 *   - the speed block (procSteerMove's speed set / chase, setSelfMove's deceleration) runs whole on the frame's FIRST step
 *     at dt 1 (A's order: the procedure runs before the move) and is held on the later steps (ship60_speedPath); a body
 *     that first runs on a later step of a frame keeps the per-step form for that frame; setMoveAngle's turn rate reads
 *     the frame-start speedF (A's: the turn comes before the chase), so the turn's per-frame amount stays one value;
 *   - execute's speed from the pitch: the frame-start wave pitch (A's: setWaveAngle runs after the move) with the step's
 *     heading and the held speedF; x / y / z through t60c::moveBegin / moveEnd (the tornado / kamome form), so the tick30
 *     step lands on the frame-start place + the frame-end velocity (A's single add); only without a collision push;
 *   - mFwdVel: the frame's forward speed (A's |frame displacement|), from the frame's exact move on its first step (the
 *     same f32 operations as the tick30 landing) and again from the actual frame displacement on the tick30 step;
 *   - the water height (getWaterY) sampled once a frame on the tick30 step at the frame-end place (A's: after the move);
 *     the earlier steps keep the previous frame's; the float spring's frame start is the exact-move y (ship60_floatEnter);
 *   - old.pos, mOldShapeAngleX / Z (and the flight's reads of them) and eyePos (the head joint after the step's calc, the
 *     tick30 step's = A's frame end; published on the tick30 step by daShip_Execute) carry the frame's values (the process-order read rule).
 * dt 1: every helper is the original (dt-1 check). SHIP60_FRAME_NEGCTRL: the per-step form throughout (negative control). */
template <int = 0> struct Ship60Dt1 {
    f32 prev;
    bool set;
    explicit Ship60Dt1(bool on) : prev(1.0f), set(on && t60::dt() != 1.0f) {
        if (set)
            prev = t60_set_dt(1.0f);
    }
    ~Ship60Dt1() {
        if (set)
            t60_set_dt(prev);
    }
};
static inline bool ship60_frameForm() {
#ifdef SHIP60_FRAME_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}
enum class Ship60SpeedPath { Whole, Hold, Step };
struct Ship60Speed {
    u32 frame, stamp;
    const void* owner;
    bool valid;
};
/* Whole: run the block at dt 1 (the frame's first step); Hold: skip it (a later step of a frame whose first step ran it);
 * Step: the block as it is (dt 1, NEGCTRL, or a frame whose first step did not reach it) */
static inline Ship60SpeedPath ship60_speedPath(Ship60Speed& s, const void* owner, u32 frameId) {
    if (!ship60_frameForm()) {
        return Ship60SpeedPath::Step;
    }
    if (t60::first_step()) {
        s.frame = frameId;
        s.stamp = t60_step_counter();
        s.owner = owner;
        s.valid = true;
        return Ship60SpeedPath::Whole;
    }
    if (s.valid && s.owner == owner && ship60_sameFrame(s.frame, s.stamp, frameId)) {
        return Ship60SpeedPath::Hold;
    }
    return Ship60SpeedPath::Step;
}
/* a float's frame start (the first call in the frame), bits kept */
static inline f32 ship60_startF(Ship60Start& s, const void* owner, u32 frameId, f32 now) {
    s32 bits;
    std::memcpy(&bits, &now, 4);
    const s32 st = ship60_start(s, owner, frameId, bits);
    f32 v;
    std::memcpy(&v, &st, 4);
    return v;
}

/* the water shadow's texture scroll (setShadowBtkRate, end of execute, 2026-11-21): the rate's
 * chase (cLib_addCalc 0.02 toward 1 + speedF / 2 + tilt) and the play ran per step at dt, so B's frame end drifted from
 * A's (sqPg record 31: +0x5C0 rate 28.005 / A 28, +0x5D0 frame 27.245 / A 26.48) although speedF and the shape angles
 * are A's at the frame end. Form (the speed block's): the frame's first call keeps the frame control (rate, frame, state:
 * 0x10 bytes); the earlier steps only play it on at the held rate (the picture: the draw's btk entry reads the frame);
 * the tick30 step puts the kept control back and runs the original chase + play at dt 1 on the frame-end speedF / shape
 * angles: A's frame end. A frame whose earlier steps did not reach the site runs the tick30 step from its own state.
 * dt 1: the original. SHIP60_FRAME_NEGCTRL: per step. Host memory. */
struct Ship60Ctrl {
    u32 frame, stamp;
    const void* owner;
    bool valid;
    /* the kept frame control as its guest bytes (host storage): a J3DFrameCtrl here would be a guest-layout object in
     * host memory (be<> fields: `kept = *fc` stored 16 bytes at ea(&kept), a stray guest write, and read them back from
     * there; t60_beprobe, 2026-11-22). The bytes are copied, never read as values. */
    u8 kept[0x10];
};
static_assert(sizeof(J3DFrameCtrl) == sizeof(Ship60Ctrl::kept), "Ship60Ctrl: J3DFrameCtrl is 0x10 bytes");
static inline void ship60_ctrlKeep(Ship60Ctrl& c, const void* owner, u32 frameId, const J3DFrameCtrl* fc) {
    if (c.valid && c.owner == owner && ship60_sameFrame(c.frame, c.stamp, frameId)) {
        return;
    }
    c.frame = frameId;
    c.stamp = t60_step_counter();
    c.owner = owner;
    c.valid = true;
    std::memcpy(c.kept, fc, sizeof(c.kept)); /* guest bytes as they are (big-endian) */
}
static inline void ship60_ctrlBack(Ship60Ctrl& c, const void* owner, u32 frameId, J3DFrameCtrl* fc) {
    if (c.valid && c.owner == owner && ship60_sameFrame(c.frame, c.stamp, frameId)) {
        std::memcpy(fc, c.kept, sizeof(c.kept));
    }
    c.valid = false;
}
/* follow-up (2026-11-23): the rope's ripple place (setRopePos's end, d_a_ship.cpp,
 * mRopeRipplePos = the rope's first point + a fresh cM_rndFX(20) jitter in x and z, y the water) is set on the tick30
 * step only (the per-step chance rule: the two draws at A's place), so on the earlier steps it held the previous frame's place while the
 * rope and the hull moved on; the ripple emitter (dPa_rippleEcallBack, which follows it per step with JPA_emitter_t60.h
 * follow_step) then stairs. Form: the tick30 step notes the frame's jitter (the two draws' results, after the original
 * sets the place); an earlier step of the next frame sets the place from this step's rope point + the noted jitter and
 * this step's water y: no draw (the RNG and the draw count are A's), and the tick30 step's original overwrites all three
 * words (the frame end is A's). No note from the frame just before (a new ship, a skipped frame): the place holds as
 * before. WWHD_T60_SHIP_ROPEJIT=0: the place held (the negative control). dt 1: never run. Host memory. */
struct Ship60RopeJit {
    const void* owner;
    u32 stamp;
    f32 dx, dz;
    bool valid;
};
static Ship60RopeJit s_ship60RopeJit;
static inline bool ship60_ropeJitOn() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_SHIP_ROPEJIT");
        return !(e && e[0] == '0');
    }();
    return on;
}
/* the tick30 step, after the original set the place: the frame's jitter */
static inline void ship60_ropeJitNote(const void* owner, f32 dx, f32 dz) {
    if (t60::dt() == 1.0f) return;
    s_ship60RopeJit.owner = owner;
    s_ship60RopeJit.stamp = t60_step_counter();
    s_ship60RopeJit.dx = dx;
    s_ship60RopeJit.dz = dz;
    s_ship60RopeJit.valid = true;
}
/* an earlier step: the place from this step's rope point (x, z) and water y with the previous frame's jitter */
template <class V> static inline void ship60_ropeJitStep(const void* owner, f32 ropeX, f32 ropeZ, f32 waterY, V* pos) {
    const Ship60RopeJit& j = s_ship60RopeJit;
    if (t60::dt() == 1.0f || !ship60_ropeJitOn() || !j.valid || j.owner != owner) return;
    if (t60_step_counter() - j.stamp >= t60::steps()) return; /* not the frame just before */
    pos->x = ropeX + j.dx;
    pos->y = waterY;
    pos->z = ropeZ + j.dz;
}
