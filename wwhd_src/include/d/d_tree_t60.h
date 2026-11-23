/* d_tree_t60.h (WWHD, true 60 build): the trees (d_tree.cpp, dTree_packet_c) on a general
 * step length (dt = 1/N). Converted (d_s_play_t60.h kT60W_Tree), the world block runs the tree calc
 * on every logic step with g_dt = 1/N; at dt = 1 (N = 1, the 30 Hz fallback, an unconverted tree system) every helper
 * answers the original, so the original instructions run (dt-1 check bit-exact). Separate file:
 * d_tree.cpp keeps thin calls only.
 *
 * calc 025C9948:
 *   - the 8 wind animations sway with g_Counter.mTimer, a 30 Hz counter (the 60 Hz counter rule: counted on the tick30 step, after the
 *     world block). The frame's earlier steps read the frame's time: mTimer - 1 plus the frame's progress p (dt per
 *     step since the frame's first step); the tick30 step reads mTimer itself (p = 1): every frame ends on the
 *     original sway.
 *   - the player's sword angles, the mass cylinder set-up and the hit checks (hitCheck: WorkCo pushes, WorkAt cuts,
 *     WorkAt_NoCutAnim hits; their checkMass reads the mass collision the scene prepares on the tick30 step) run on the
 *     tick30 step only: the one-sweep-per-frame attack rule (attack hit checks once per 30 Hz frame; Link registers
 *     his attack volumes for the tick30 step only) and the mass prepare stays on the tick30 step (d_s_play_8.cpp).
 * animation 025C6D40, a tree that was hit (its own slot, mAnimIdx >= 8):
 *   - the swing is a damped integer spring: v' = (v - x) d (d = 0.9), x' = x + (int)v' (x the trunk's X angle, s16).
 *     Without the truncation it is linear, (x, v)' = M (x, v), M = [[1 - d, d], [-d, d]] (complex eigenvalues of
 *     modulus sqrt(d)), so a step of length dt is M^dt (the exact fractional power: N steps of M^(1/N) are M); the
 *     earlier steps run it from the values they find, the angle real in host memory with its fractional carry
 *     (x0 + trunc(x - x0) drawn, x0 the frame's start), v stored as f32. A settled tree (x = v = 0 at the frame's
 *     start) chases its top back toward its wind animation (cLib_chaseAngleS x dt, c_lib's carry form). A cut tree's top
 *     falls with v += g dt, p += v dt + the exact-arc term g dt (1 - dt) / 2 (the per-step movement rule), the tilt speed and tilt through
 *     cLib_chaseF / cLib_chaseAngleS x dt.
 *   - the frame's decisions are the tick30 step's: the zero-crossing branch (settle below 40, the leaf particle 0x3DC
 *     and the sound 0x6932 above 400: the one-emitter-per-event rule, once per frame), the return to the wind animation (the slot freed), the
 *     cut top's smoke (0x2022, 0x3DF) and its landing (mState bit 8, the fallen matrix). On the tick30 step the slot's
 *     state at the frame's first step is put back (before the hit checks, so a push or a cut meets the state the
 *     original frame starts from: |v| against the push strength) and the original full step runs at dt 1 (the c_lib
 *     integer-variant form: in-between steps from the current values, the tick30 step the original
 *     step from the frame's start). Every frame ends exactly on the 30 Hz values (N-step spec byte exact); the earlier steps
 *     draw the 60 Hz motion (visual only: the tree swing feeds no collision and no decision).
 *   - host memory: one entry per slot (256: the slot index is a u8), keyed by the frame tag g_Counter.mCounter0; an
 *     entry whose bytes differ from what its last earlier step left (another writer) restarts from memory. Not in save
 *     states (a state saved between steps restarts the frame from memory, the 30 Hz behaviour).
 * Draw interpolation:. */
#pragma once
#include "true60.h"
#include <cmath>
#include <cstring>

static const u32 kTree60AnmWords = 9; /* dTree_anm_c's state: mState .. mTopOffsetZ, 0x24 bytes (the matrices are update's) */

struct Tree60Slot {
    u32 tag;                    /* g_Counter.mCounter0 of the frame */
    bool valid;                 /* an earlier step of that frame ran the slot */
    u32 start[kTree60AnmWords]; /* the slot at the frame's first step (guest words) */
    u32 last[kTree60AnmWords];  /* what the last earlier step left */
    s32 x0;                     /* the swing angle at the frame's start */
    f64 x, v;                   /* the swing angle (real, with its carry) and speed after the last earlier step */
};
static Tree60Slot g_tree60Slots[256];

/* the 30 Hz frame tag (g_Counter.mCounter0, counted after the tick30 step's world block) */
static inline u32 tree60_tag() { return gabi::load<u32>(0x101FF558); }

/* the earlier steps of a frame (converted, N > 1, before the tick30 step) */
static inline bool tree60_between() { return t60::dt() != 1.0f && !t60::tick30(); }
/* the hit checks and the frame's decisions run on this step (dt 1, or the tick30 step) */
static inline bool tree60_frame_step() { return !tree60_between(); }

/* the frame's progress after this step (dt per step since the frame's first step); 1 at dt 1 and on the tick30 step.
 * calc runs once per step (the world block), so one slot */
static inline f32 tree60_progress() {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        return 1.0f;
    }
    static u32 stamp = 0;
    static f32 acc = 0.0f;
    const u32 now = t60_step_counter();
    const bool cont = now - stamp == 1 && !t60::first_step();
    acc = (cont ? acc : 0.0f) + dt;
    stamp = now;
    if (t60::tick30() || acc > 1.0f) {
        acc = 1.0f;
    }
    return acc;
}

/* the wind animations' clock for wind animation i: (f32)(mTimer + 250 i) on dt 1 and the tick30 step (the original),
 * else the frame's time mTimer - 1 + p */
static inline f32 tree60_wind_frame(u32 timer, u32 i, f32 p) {
    if (p >= 1.0f) {
        return (f32)(timer + i * 250);
    }
    return (f32)(u32)(timer - 1 + i * 250) + p;
}

/* M^dt applied to (x, v): M = [[1 - d, d], [-d, d]], M^t = r^t (cos(t th) I + sin(t th) / sin(th) (M / r - cos(th) I)),
 * r = sqrt(d), cos(th) = 1 / (2 r) */
static inline void tree60_spring(f64& x, f64& v, f64 dt, f32 damping) {
    const f64 d = (f64)damping;
    const f64 r = std::sqrt(d), c = 1.0 / (2.0 * r), s = std::sqrt(1.0 - c * c), th = std::atan2(s, c);
    const f64 rt = std::pow(r, dt), ct = std::cos(dt * th), st = std::sin(dt * th) / s;
    const f64 k00 = (1.0 - d) / r - c, k01 = d / r, k10 = -d / r, k11 = d / r - c;
    const f64 nx = rt * (ct * x + st * (k00 * x + k01 * v));
    const f64 nv = rt * (ct * v + st * (k10 * x + k11 * v));
    x = nx;
    v = nv;
}

/* the slot's state words (guest memory through gabi loads / stores) */
static inline void tree60_read(u32 anm, u32* w) {
    for (u32 i = 0; i < kTree60AnmWords; i++) {
        w[i] = gabi::load<u32>(anm + i * 4);
    }
}
static inline bool tree60_same(u32 anm, const u32* w) {
    for (u32 i = 0; i < kTree60AnmWords; i++) {
        if (gabi::load<u32>(anm + i * 4) != w[i]) {
            return false;
        }
    }
    return true;
}
static inline void tree60_write(u32 anm, const u32* w) {
    for (u32 i = 0; i < kTree60AnmWords; i++) {
        gabi::store<u32>(anm + i * 4, w[i]);
    }
}

/* an earlier step's entry for slot idx at guest address anm: continued, or (the frame's first step, another writer)
 * started from memory. Returns the entry. */
static inline Tree60Slot& tree60_enter(u32 idx, u32 anm) {
    Tree60Slot& s = g_tree60Slots[idx & 0xFF];
    const u32 tag = tree60_tag();
    if (!(s.valid && s.tag == tag && tree60_same(anm, s.last))) {
        tree60_read(anm, s.start);
        s.tag = tag;
        s.valid = true;
        s.x0 = (s16)gabi::load<u16>(anm + 8);           /* mTrunkAngleX */
        s.x = (f64)s.x0;
        const u32 v = gabi::load<u32>(anm + 0xC);       /* mAnimTimer */
        f32 vf;
        std::memcpy(&vf, &v, 4);
        s.v = (f64)vf;
    }
    return s;
}
/* after an earlier step: remember what it left */
static inline void tree60_leave(Tree60Slot& s, u32 anm) { tree60_read(anm, s.last); }

/* the frame's start values were (0, 0): the settled phase (the original's frame: settle branch, chase only) */
static inline bool tree60_settled(const Tree60Slot& s) {
    return s.x0 == 0 && (s.start[3] & 0x7FFFFFFFu) == 0; /* word 3: mAnimTimer */
}

/* the tick30 step (dt < 1): put the slot back to the frame's start if the earlier steps left it as it is (else someone
 * wrote it on this step - a hit check after this restore, or a slot that ran no earlier step: keep it) */
static inline void tree60_restore(u32 idx, u32 anm) {
    if (t60::dt() == 1.0f) {
        return;
    }
    Tree60Slot& s = g_tree60Slots[idx & 0xFF];
    if (s.valid && s.tag == tree60_tag() && tree60_same(anm, s.last)) {
        tree60_write(anm, s.start);
    }
}
/* the tick30 step ran the original frame: the entry is done */
static inline void tree60_done(u32 idx) { g_tree60Slots[idx & 0xFF].valid = false; }

/* the tick30 step's original frame (animation from the restored start): its c_lib chases take the full 30 Hz step (dt 1
 * around the call only; dt 1: the plain call) */
static inline u32 tree60_chaseAngle(void* value, u32 target, u32 step) {
    if (t60::dt() == 1.0f) {
        return chaseAngle(value, target, step);
    }
    const f32 previousDt = t60_set_dt(1.0f);
    const u32 reached = chaseAngle(value, target, step);
    t60_set_dt(previousDt);
    return reached;
}
static inline void tree60_chaseFloat(void* value, f32 target, f32 step) {
    if (t60::dt() == 1.0f) {
        chaseFloat(value, target, step);
        return;
    }
    const f32 previousDt = t60_set_dt(1.0f);
    chaseFloat(value, target, step);
    t60_set_dt(previousDt);
}
