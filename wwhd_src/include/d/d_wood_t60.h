/* d_wood_t60.h (WWHD, true 60 build; d_wood part): the bushes (d_wood.cpp, dWood::Packet_c)
 * on a general step length (dt = 1/N), the tree way (d_tree_t60.h). Converted (d_s_play_t60.h
 * kT60W_Wood), the world block runs dWood::Packet_c::calc on every logic step with g_dt = 1/N; at dt = 1 (N = 1, the
 * 30 Hz fallback, an unconverted wood system) the original instructions run (dt-1 check bit-exact). Separate file:
 * d_wood.cpp keeps thin calls and the earlier steps' sway (it needs the unit's sway tables).
 *
 * calc 025D0994:
 *   - calc_cc (the mass hit checks: pushes, cuts, the cooldown count, the push sound, the cut particle and sound) and
 *     the units' proc (the mode changes: cut -> gone, to_norm -> back on a shared wind slot, a free slot -> a wind slot)
 *     run on the tick30 step only: the one-sweep-per-frame attack rule (hit checks once per 30 Hz frame against what the tick30 step
 *     registered) and the frame's decisions are the tick30 step's.
 *   - the frame's earlier steps move every playing animation slot (72: the 8 shared wind slots and the push / cut
 *     slots) at dt from what the earlier step before left (the frame's first step: from memory): phases advance by
 *     their per-frame velocity x dt, amplitudes chase at step x dt (cLib_chaseS / cLib_chaseAngleS forms),
 *     cLib_addCalcAngleS's ratio as 1 - (1 - 1/scale)^dt with its bounds x dt, the cut bush falls with v += g dt,
 *     p += v dt + g dt (1 - dt) / 2 (the per-step movement rule exact arc) and fades by 14 dt; the values carried in host memory (real),
 *     the guest words written truncated toward the frame's start; then the slot's matrix as the original builds it.
 *     No countdown, no mode change, no search for a slot (search_anm's round robin is a decision).
 *   - the tick30 step puts every slot an earlier step moved back to the frame's start (if no one else wrote it since)
 *     and runs the original calc at dt 1: every frame ends exactly on the 30 Hz values (N-step spec byte exact); the earlier
 *     steps draw the 60 Hz motion (visual only: the bushes' sway feeds no collision and no decision).
 *   - host memory: one entry per slot, keyed by the frame tag g_Counter.mCounter0; an entry whose bytes differ from what
 *     its last earlier step left (another writer) restarts from memory. Not in save states (a state saved between steps
 *     restarts the frame from memory, the 30 Hz behaviour).
 * Draw interpolation:. */
#pragma once
#include "true60.h"
#include <cmath>

static const u32 kWood60Slots = 72;     /* dWood::Packet_c::mAnm */
static const u32 kWood60Words = 0x8C / 4; /* dWood::Anm_c: the matrices and the state, 0x8C bytes */

struct Wood60Slot {
    u32 tag;                   /* g_Counter.mCounter0 of the frame */
    bool valid;                /* an earlier step of that frame moved the slot */
    u32 start[kWood60Words];   /* the slot at the frame's first step (guest words) */
    u32 last[kWood60Words];    /* what the last earlier step left */
    f64 phY[2], phX[2], ampY[2], ampX[2]; /* phases and amplitudes, real */
    f64 dir;                   /* mForceDir, real */
    f64 posY, posZ, velY;      /* the cut bush's fall */
    f64 alpha;                 /* mAlphaScale, real */
};
static Wood60Slot g_wood60Slots[kWood60Slots];

static inline u32 wood60_tag() { return gabi::load<u32>(0x101FF558); }

/* the frame's progress after this step (dt per step since the frame's first step); 1 at dt 1 and on the tick30 step.
 * calc runs once per step (the world block), so one counter */
static inline f32 wood60_progress() {
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

static inline void wood60_read(u32 anm, u32* w) {
    for (u32 i = 0; i < kWood60Words; i++) {
        w[i] = gabi::load<u32>(anm + i * 4);
    }
}
static inline bool wood60_same(u32 anm, const u32* w) {
    for (u32 i = 0; i < kWood60Words; i++) {
        if (gabi::load<u32>(anm + i * 4) != w[i]) {
            return false;
        }
    }
    return true;
}
static inline void wood60_write(u32 anm, const u32* w) {
    for (u32 i = 0; i < kWood60Words; i++) {
        gabi::store<u32>(anm + i * 4, w[i]);
    }
}
static inline f64 wood60_f32(u32 bits) {
    f32 f;
    __builtin_memcpy(&f, &bits, 4);
    return (f64)f;
}

/* an earlier step's entry for slot idx at guest address anm: continued, or (the frame's first step, another writer)
 * started from memory */
static inline Wood60Slot& wood60_enter(u32 idx, u32 anm) {
    Wood60Slot& s = g_wood60Slots[idx % kWood60Slots];
    const u32 tag = wood60_tag();
    if (!(s.valid && s.tag == tag && wood60_same(anm, s.last))) {
        wood60_read(anm, s.start);
        s.tag = tag;
        s.valid = true;
        s.dir = (f64)(s16)gabi::load<u16>(anm + 0x66);
        s.posY = wood60_f32(gabi::load<u32>(anm + 0x6C));
        s.posZ = wood60_f32(gabi::load<u32>(anm + 0x70));
        s.velY = wood60_f32(gabi::load<u32>(anm + 0x74));
        for (u32 i = 0; i < 2; i++) {
            s.phY[i] = (f64)(s16)gabi::load<u16>(anm + 0x78 + i * 2);
            s.phX[i] = (f64)(s16)gabi::load<u16>(anm + 0x7C + i * 2);
            s.ampY[i] = (f64)(s16)gabi::load<u16>(anm + 0x80 + i * 2);
            s.ampX[i] = (f64)(s16)gabi::load<u16>(anm + 0x84 + i * 2);
        }
        s.alpha = (f64)gabi::load<u8>(anm + 0x8A);
    }
    return s;
}
static inline void wood60_leave(Wood60Slot& s, u32 anm) { wood60_read(anm, s.last); }

/* a real value carried from the frame's start, as the s16 the guest holds: the start plus the truncated motion */
static inline s16 wood60_s16(f64 v, u32 startHalf) {
    const s32 s = (s16)(u16)startHalf;
    return (s16)(s + (s32)std::trunc(v - (f64)s));
}
/* the start's s16 at byte offset off of the slot */
static inline u32 wood60_start16(const Wood60Slot& s, u32 off) {
    const u32 w = s.start[off >> 2];
    return (off & 2) ? (w & 0xFFFFu) : (w >> 16);
}

/* cLib_chaseS / cLib_chaseAngleS at dt (step x dt toward the target, no overshoot) */
static inline f64 wood60_chase(f64 v, f64 target, f64 step) {
    if (v < target) {
        return v + step < target ? v + step : target;
    }
    return v - step > target ? v - step : target;
}
static inline f64 wood60_chaseAngle(f64 v, s32 target, f64 step) {
    const f64 d = (f64)(s16)(u16)((u32)target - (u32)(s32)std::floor(v)) - (v - std::floor(v));
    return v + (std::fabs(d) <= step ? d : (d > 0 ? step : -step));
}
/* cLib_addCalcAngleS(target, scale, maxStep, minStep) at dt: the move diff x (1 - (1 - 1/scale)^dt), bounded by
 * maxStep x dt, at least minStep x dt (not past the target) */
static inline f64 wood60_addCalcAngle(f64 v, s32 target, s32 scale, s32 maxStep, s32 minStep, f64 dt) {
    const f64 d = (f64)(s16)(u16)((u32)target - (u32)(s32)std::floor(v)) - (v - std::floor(v));
    if (d == 0.0) {
        return v;
    }
    f64 mv = scale >= 2 ? d * (1.0 - std::pow(1.0 - 1.0 / (f64)scale, dt)) : d;
    const f64 mx = (f64)maxStep * dt, mn = (f64)minStep * dt;
    if (mv > mx) mv = mx;
    if (mv < -mx) mv = -mx;
    if (mv > -mn && mv < mn) mv = d > 0 ? (mn < d ? mn : d) : (-mn > d ? -mn : d);
    return v + mv;
}

/* the tick30 step (dt < 1): every slot an earlier step of this frame moved goes back to the frame's start if the
 * earlier steps left it as it is (else someone wrote it since: keep it); the entries are done */
static inline void wood60_restore_all(u32 anm0) {
    const u32 tag = wood60_tag();
    for (u32 i = 0; i < kWood60Slots; i++) {
        Wood60Slot& s = g_wood60Slots[i];
        const u32 anm = anm0 + i * 0x8C;
        if (s.valid && s.tag == tag && wood60_same(anm, s.last)) {
            wood60_write(anm, s.start);
        }
        s.valid = false;
    }
}
