/* d_grass_t60.h (WWHD, true 60 build; grass part): the grass (d_grass.cpp, dGrass_packet_c)
 * on a general step length (dt = 1/N), the tree way (d_tree_t60.h). Converted (d_s_play_t60.h
 * kT60W_Grass), the world block runs dGrass_packet_c::calc on every logic step with g_dt = 1/N; at dt = 1 (N = 1, the
 * 30 Hz fallback, an unconverted grass system) the original instructions run (dt-1 check bit-exact). Separate file:
 * d_grass.cpp keeps thin calls and the earlier steps' recovery.
 *
 * calc 0254C6C4:
 *   - the 8 wind animations sway with g_Counter.mTimer, a 30 Hz counter (the 60 Hz counter rule: counted on the tick30 step, after the
 *     world block). The frame's earlier steps read the frame's time: mTimer - 1 plus the frame's progress p (dt per
 *     step since the frame's first step); the tick30 step reads mTimer itself: every frame ends on the original sway.
 *   - the hit checks (hitCheck 0254AA3C: dCcMassS_Mng::Chk, WorkCo pushes, WorkAt cuts) run on the tick30 step only:
 *     the one-sweep-per-frame attack rule (attack and push checks once per 30 Hz frame against what the tick30 step registered).
 *   - a pushed blade's own slot (mAnimIdx >= 8, hitCheck's no-hit branch) recovers on the earlier steps at dt: its X
 *     angle as cLib_addCalcAngleS(wind slot's X, 16, 4000, 100) at dt (ratio 1 - (1 - 1/16)^dt, bounds x dt), then, once
 *     there, its Y angle as cLib_chaseAngleS(the wind slot's heading, 800) at dt; the values carried in host memory
 *     (real), the guest words written truncated toward the frame's start. No sound, no state change, no return to the
 *     wind slot (the frame's decisions: the tick30 step's).
 *   - the tick30 step puts every slot an earlier step moved back to the frame's start (if no one else wrote it since)
 *     and runs the original calc at dt 1: every frame ends exactly on the 30 Hz values (N-step spec byte exact); the earlier
 *     steps draw the 60 Hz motion (visual only: the recovery feeds no collision and no decision).
 *   - host memory: one entry per slot (104), keyed by the frame tag g_Counter.mCounter0; an entry whose bytes differ from
 *     what its last earlier step left (another writer) restarts from memory. Not in save states.
 * The flowers (d_flower.cpp, dFlower_packet_c calc 02548370) use the same form: their 72 slots
 * (dFlower_anm_c: mActive, mAngleY, mBend, as mState, mRotY, mRotX here), the wind slots from mTimer + 250 i, the hit
 * checks and the player's sword angles / cut flag on the tick30 step only, a hit flower's own slot recovering as
 * hitCheck 025457C4's no-hit branch (addCalcAngleS 16, 0xFA0, 0x64; chaseAngleS 0x320) at dt. Each unit keeps its own
 * slot table (a static of the including unit).
 * Draw interpolation:. */
#pragma once
#include "true60.h"
#include <cmath>

static const u32 kGrass60Slots = 104; /* dGrass_packet_c::mGrassAnm */

struct Grass60Slot {
    u32 tag;      /* g_Counter.mCounter0 of the frame */
    bool valid;   /* an earlier step of that frame moved the slot */
    u32 start[2]; /* mState / mRotY, mRotX at the frame's first step (guest words) */
    u32 last[2];  /* what the last earlier step left */
    f64 rx, ry;   /* the angles, real */
};
static Grass60Slot g_grass60Slots[kGrass60Slots];

static inline u32 grass60_tag() { return gabi::load<u32>(0x101FF558); }
/* the earlier steps of a frame (converted, N > 1, before the tick30 step) */
static inline bool grass60_between() { return t60::dt() != 1.0f && !t60::tick30(); }

/* the frame's progress after this step; 1 at dt 1 and on the tick30 step (calc runs once per step) */
static inline f32 grass60_progress() {
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
/* the wind animations' clock (f32)(mTimer + 250 i): the original at p 1, else the frame's time mTimer - 1 + p */
static inline f32 grass60_wind_time(u32 timer, u32 i, f32 p) {
    if (p >= 1.0f) {
        return (f32)(timer + i * 250);
    }
    return (f32)(u32)(timer - 1 + i * 250) + p;
}

static inline Grass60Slot& grass60_enter(u32 idx, u32 anm) {
    Grass60Slot& s = g_grass60Slots[idx % kGrass60Slots];
    const u32 tag = grass60_tag();
    const u32 w0 = gabi::load<u32>(anm), w1 = gabi::load<u32>(anm + 4);
    if (!(s.valid && s.tag == tag && s.last[0] == w0 && s.last[1] == w1)) {
        s.start[0] = w0;
        s.start[1] = w1;
        s.tag = tag;
        s.valid = true;
        s.ry = (f64)(s16)(u16)(w0 & 0xFFFFu);
        s.rx = (f64)(s16)(u16)(w1 >> 16);
    }
    return s;
}
static inline void grass60_leave(Grass60Slot& s, u32 anm) {
    s.last[0] = gabi::load<u32>(anm);
    s.last[1] = gabi::load<u32>(anm + 4);
}
/* a real angle carried from the frame's start, as the s16 the guest holds */
static inline s16 grass60_s16(f64 v, s32 start) { return (s16)(start + (s32)std::trunc(v - (f64)start)); }
/* the wrapped distance from the real angle v to target */
static inline f64 grass60_angleDiff(f64 v, s32 target) {
    const f64 fl = std::floor(v);
    return (f64)(s16)(u16)((u32)target - (u32)(s32)fl) - (v - fl);
}

/* the tick30 step (dt < 1): every slot moved by an earlier step of this frame back to the frame's start if the earlier
 * steps left it as it is; the entries are done */
static inline void grass60_restore_all(u32 anm0, u32 count) {
    const u32 tag = grass60_tag();
    for (u32 i = 0; i < count && i < kGrass60Slots; i++) {
        Grass60Slot& s = g_grass60Slots[i];
        const u32 anm = anm0 + i * 0x38;
        if (s.valid && s.tag == tag && gabi::load<u32>(anm) == s.last[0] && gabi::load<u32>(anm + 4) == s.last[1]) {
            gabi::store<u32>(anm, s.start[0]);
            gabi::store<u32>(anm + 4, s.start[1]);
        }
        s.valid = false;
    }
}
