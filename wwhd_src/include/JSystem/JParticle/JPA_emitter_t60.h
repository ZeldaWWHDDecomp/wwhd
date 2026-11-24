/* JPA_emitter_t60.h (WWHD, true 60 build): the particle emitter's per-frame state on a general
 * step length (dt = 1/N).
 *
 * JPABaseEmitter (JPA_emitter_calc.cpp), once JPA-10 runs the particle calc on every step at g_dt = 1/N:
 *   - mTick +0x194 (calc 0281FE40: the emitter age; key frames, max frame) and mTime +0x198 (doStartFrameProcess
 *     028200C4: the start delay) advance by dt (the 30 Hz timer rule: continuous ages, read as floats: the key values interpolate).
 *   - the rate-step timer mRateStepTimer +0x250 (calcCreatePtcls 0281F878) advances by dt; the frame's earlier steps only
 *     advance it, the tick30 step runs the original emission (RateStepEmit set by the previous frame's crossing) and the
 *     crossing (revised with: the emission follows the frame's emitter callbacks, which run on the tick30
 *     step, so an emitter that follows an actor emits where the 30 Hz frame does). The emission itself is unchanged: count = floor(mEmitCount + rate x (1 + rndm x r)), the
 *     FirstEmit minimum and the FixedInterval count, with the emitter's own RNG (+0x1EC) drawn as in the 30 Hz frame:
 *     emission keeps the 30 Hz cadence (one emission per crossing, at the frame's place), so the emitter RNG sequence and
 *     the particle counts are A's (shared-RNG rule: draws at A's place); the particles then move at the step length
 *     (JPA-07). Not equivalent by design: a rate-step timer that is not a whole number at a frame start (only reachable
 *     from a step-length change, N 2 -> 4); the N-step spec domain is whole timers.
 *   - the HD countdowns mDeleteCountdown +0x3D4 (doTerminationProcess 0282014C: the two-frame delete wait) and
 *     mClearCountdown +0x3D5 (calc: the deferred particle clear) count 30 Hz frames: they decrement on the frame's tick30
 *     step only (the 30 Hz timer rule, half-step counting); a delete wait started on an earlier step starts at 3, so the frame's
 *     tick30 step brings it to the 30 Hz frame's 2 (the frame-end value is A's).
 *   - the game's emitter callbacks (calc's CallbackExecute / ExecuteAfter, also under StopCalc) run once per
 *     30 Hz frame, on the tick30 step: they are unconverted game code (d_particle_callbacks.cpp, converts
 *     them per callback); the particle callbacks of calcParticles likewise (JPA_particle.cpp).
 *   - an emitter's first calc (fix): the manager's first visit of an emitter (mTick and
 *     mTime still 0, FirstEmit still set: no start-delay step and no calc step has run) runs at the frame time already
 *     elapsed, k / N on step k of the frame (t60::step), not 1 / N: an emitter created on the tick30 step (by the
 *     frame's 30 Hz code, before the particle calc) gets one whole frame there like A's first calc (age 1, rate-step
 *     timer +1, A's crossing); one created on an earlier step k gets k / N, so with the frame's remaining steps it has
 *     k / N + (N - k) / N = 1 at the frame end. Without it a new emitter stays 1/N behind A for life (its rate-step
 *     crossings half a frame late at N 2: the 3BBBBB0C burst emits once instead of twice, pt7). Only in converted
 *     context (dt < 1); at N = 1 (step 1, dt 1) and in N-step spec's whole-frame runs (step 1 first) it is the earlier code.
 * One-update emitters (pair): emission happens on the frame's
 *     tick30 step only, so an emitter a converted actor creates on an earlier step and stops on a later step of the same
 *     frame (Link's foot smoke: setFootEffect on a footfall step, resetFootEffect 023DF9C0 -> the callback's end:
 *     StopEmit, mMaxFrame -1) would never emit; at 30 Hz the stop comes one frame later, after the frame's emission. Form:
 *     the manager notes an emitter on its first visit on an earlier step (not stopped yet: its mMaxFrame); on that frame's
 *     tick30 step a noted emitter that was stopped since gets the frame's calc as if not stopped (StopEmit / delete wait
 *     / mMaxFrame undone for the calc), then the stop is put back: the frame's emission is A's, the next frame's calc sees
 *     the stopped emitter as A's next frame does. A stop on the creating step itself is not deferred (A: no emission
 *     either). Entries of a group not seen on its tick30 visit (the emitter was deleted) are dropped. dt 1: never noted.
 * dt = 1 / tick30: the original. */
#pragma once
#include "true60.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace jpa_t60 {
/* a per-frame count moves on this step: dt 1 or the frame's tick30 step */
inline bool frame_count() { return t60::dt() == 1.0f || t60::tick30(); }
/* a countdown of n 30 Hz frames started on this step: n, or n + 1 on an earlier step (its frame's tick30 decrements) */
inline unsigned countdown_start(unsigned n) { return frame_count() ? n : n + 1; }
/* the dt of the manager's first visit of an emitter: the frame time elapsed by the end of this step (k / N); 0 when
 * the step is not a later converted step (dt 1, step 1: the emitter is not read) or the emitter is not on its first
 * visit (the caller keeps the step's dt) */
template <class Emitter> inline float first_calc_dt(const Emitter* e, unsigned firstEmit) {
    float dt = t60::dt();
    unsigned k = t60::step();
    if (!(dt < 1.0f) || k <= 1u) return 0.0f;
    if ((float)e->mTick != 0.0f || (float)e->mTime != 0.0f || !((unsigned)e->mFlags & firstEmit)) return 0.0f;
    return k >= t60::steps() ? 1.0f : (float)k * dt;
}
/* one-update emitters (see above): the emitters first visited on an earlier step of this frame, per group */
struct Fresh { unsigned ea; int maxFrame; unsigned group; };
static Fresh s_fresh[64];
static unsigned s_freshN = 0;
/* first visit on an earlier step, not stopped yet: note it (its mMaxFrame, before a stop overwrites it) */
template <class Emitter> inline void fresh_note(const Emitter* e, unsigned group, unsigned firstEmit, unsigned stopEmit) {
    if (frame_count() || s_freshN >= 64u) return;
    if ((float)e->mTick != 0.0f || (float)e->mTime != 0.0f || !((unsigned)e->mFlags & firstEmit)) return;
    if (((unsigned)e->mFlags & stopEmit) || (int)e->mMaxFrame < 0) return;
    const unsigned ea = gabi::ea(e);
    for (unsigned i = 0; i < s_freshN; i++)
        if (s_fresh[i].ea == ea) return;  /* (noted already: a StopCalc emitter stays at age 0) */
    s_fresh[s_freshN++] = Fresh{ea, (int)e->mMaxFrame, group};
}
/* the tick30 visit: a noted emitter stopped since gets this calc unstopped; returns 1 when the stop is to be put back */
template <class Emitter> inline bool fresh_unstop(Emitter* e, unsigned group, unsigned stopEmit, unsigned enableDelete) {
    if (!s_freshN || !frame_count() || !(t60::dt() < 1.0f)) return false;
    const unsigned ea = gabi::ea(e);
    for (unsigned i = 0; i < s_freshN; i++) {
        if (s_fresh[i].ea != ea || s_fresh[i].group != group) continue;
        const int maxFrame = s_fresh[i].maxFrame;
        s_fresh[i] = s_fresh[--s_freshN];
        if (!((unsigned)e->mFlags & stopEmit) && (int)e->mMaxFrame >= 0) return false;
        e->mFlags = (unsigned)e->mFlags & ~(stopEmit | enableDelete);
        e->mMaxFrame = maxFrame;
        e->mDeleteCountdown = 0;  /* (only a stop on a later step can have started it: the emitter has no particles) */
        return true;
    }
    return false;
}
template <class Emitter> inline void fresh_restop(Emitter* e, unsigned stopEmit) {
    e->mFlags = (unsigned)e->mFlags | stopEmit;
    e->mMaxFrame = -1;
}
/* end of a group's tick30 visit: entries not seen belonged to deleted emitters */
inline void fresh_end(unsigned group) {
    if (!s_freshN || !frame_count()) return;
    for (unsigned i = 0; i < s_freshN;)
        if (s_fresh[i].group == group) s_fresh[i] = s_fresh[--s_freshN];
        else i++;
}
/* (follow emitters on the frame's earlier steps; 2026-11-23): the game's emitter
 * callbacks run on the tick30 step only (above), so an emitter that follows its owner (the King of Red Lions' bow waves,
 * splash and wake-track head, Link's flames / drips / follow effects) stayed at the previous frame's place on the earlier
 * steps while the owner moved on: its FollowEmtr particles (JPA_particle.cpp calcVelocity re-reads the emitter centre on
 * every calc) held there and jumped on the tick30 step, against the hull / joint they belong to (a staircase at 30 Hz).
 * Form: on an earlier step (dt < 1, not the frame's tick30 step) a known follow callback's position part alone moves the
 *   emitter translation (+0x22C) to the owner's place of this step before the step's calc (JPACalcEmitterInfo reads it):
 *     - pointer follows (the place is read through the callback's pointer, as its execute does; group >= 7 mirrors y):
 *       dPa_followEcallBack / smoke 025A590C (mFollowOff 0 and mFlag bit 2 clear), ripple 025A90E0, the ship splash
 *       025A99EC and wake track 025A9E6C (mState 0: not fading), the bow wave's executeAfter 025A92F4 (mState 0; it
 *       moves the emitter after the calc read the centre, so the 30 Hz frame's FollowEmtr wave particles follow the
 *       previous frame's place: on step k the centre is the place step k of the previous frame noted (WaveLag), and
 *       the executeAfter point gets this step's place for the draw's head vertex);
 *     - Link's pure follows (the execute only places the emitter): daPy_mtxFollowEcallBack_c 023D4528,
 *       daPy_followEcallBack_c 024140F8, daPy_mtxPosFollowEcallBack_c 024151B8, daPy_dmEcallBack_c 02415238: the original
 *       execute runs on a snapshot (the emitter's 0x3FC bytes and mDoMtx_stack_c::now are put back after it) and only
 *       its translation is kept;
 *   nothing else of the callback runs (alpha chases, fade timers, the delete checks, rate / colour / scale stay 30 Hz).
 *   The frame-start translation is noted (first move of the frame) and put back at the emitter's tick30 visit, before
 *   the frame's callbacks, when the emitter still holds the translation written here (an actor that set it since keeps
 *   its own value): the tick30 step's callbacks, the emission (count, emitter RNG +0x1EC, volume, the 30 Hz frame's
 *   place) and the frame end are A's bit for bit (the sound-gate rule / RNG-drift plausibility: no draw moves; the bow wave's executeAfter speed
 *   from the frame's move sees A's previous translation). Particles born at the frame's place keep their own offset;
 *   FollowEmtr particles follow the emitter on every step.
 * Debug: WWHD_T60_PTCL_FOLLOW=0 is the negative control (the earlier steps leave the emitter, as before);
 * WWHD_T60_EMTRLOG=path logs each follow-kind emitter after its calc on every step (step counter, tick30, frame tag,
 * group, emitter, kind, the data flags (8: FollowEmtr), translation, the owner's place, particle count, the newest particle's place, centroid,
 * the emitter's particle callback, the newest particle's callback, offset position and local position).
 * dt 1 / N 1: never run (frame_count() is true on every step). */
inline bool follow_on() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_PTCL_FOLLOW");
        return !(e && e[0] == '0');
    }();
    return on;
}
enum : unsigned { kFoCb = 0x1E4, kFoRot = 0x1F0, kFoTrans = 0x22C, kFoGroup = 0x262, kFoSize = 0x3FC,
                  kFoMtxNow = 0x1048D0CCu, kFoPtcls = 0x1AC };
inline unsigned fo_w(unsigned a) { return (unsigned)*gabi::at<be<u32>>(a); }
inline float fo_f(unsigned a) { return (float)*gabi::at<be<f32>>(a); }
inline int fo_h(unsigned a) { return (int)(s16)*gabi::at<be<s16>>(a); }
inline unsigned fo_b(unsigned a) { return (unsigned)(u8)*gabi::at<be<u8>>(a); }
/* the callback's execute (slot 0x1C) or, for the bow wave, its executeAfter (slot 0x24): the follow kind, 0 none */
inline unsigned follow_kind(unsigned e) {
    const unsigned cb = fo_w(e + kFoCb);
    if (!cb) return 0;
    const unsigned vt = fo_w(cb);
    if (!vt) return 0;
    const unsigned ex = fo_w(vt + 0x1C);
    switch (ex) {
    case 0x025A590Cu: case 0x025A90E0u: case 0x025A99ECu: case 0x025A9E6Cu:
    case 0x023D4528u: case 0x024140F8u: case 0x024151B8u: case 0x02415238u: return ex;
    default: break;
    }
    return fo_w(vt + 0x24) == 0x025A92F4u ? 0x025A92F4u : 0u;
}
/* the owner's place now (the callback's position part); false: the callback would not move the emitter */
template <class Emitter, class Callback> inline bool follow_place(Emitter* em, unsigned kind, float* out) {
    const unsigned e = gabi::ea(em), cb = fo_w(e + kFoCb);
    unsigned pos = 0;
    switch (kind) {
    case 0x025A590Cu: if (fo_b(cb + 0x12) || (fo_b(cb + 0x10) & 2u)) return false; pos = fo_w(cb + 8); break;
    case 0x025A90E0u: pos = fo_w(cb + 8); break;
    case 0x025A99ECu: if (fo_h(cb + 4)) return false; pos = fo_w(cb + 0x10); break;
    case 0x025A9E6Cu: if (fo_h(cb + 4)) return false; pos = fo_w(cb + 0x34); break;
    case 0x025A92F4u: if (fo_h(cb + 4)) return false; pos = fo_w(cb + 0x34); break;
    case 0x023D4528u: case 0x024140F8u: case 0x024151B8u: case 0x02415238u: {
        /* Link's pure follows: the original execute on a snapshot, its translation kept */
        u8 snap[kFoSize], now[0x30];
        std::memcpy(snap, gabi::at<u8>(e), kFoSize);
        std::memcpy(now, gabi::at<u8>(kFoMtxNow), 0x30);
        Callback* c = em->mpEmitterCallBack;
        gabi::call_ptr<void>(kind, c, em);
        for (unsigned i = 0; i < 3; i++) out[i] = fo_f(e + kFoTrans + 4 * i);
        std::memcpy(gabi::at<u8>(e), snap, kFoSize);
        std::memcpy(gabi::at<u8>(kFoMtxNow), now, 0x30);
        return true;
    }
    default: return false;
    }
    if (!pos) return false;
    out[0] = fo_f(pos);
    out[1] = fo_f(pos + 4);
    out[2] = fo_f(pos + 8);
    if (fo_b(e + kFoGroup) >= 7u) out[1] = -out[1];
    return true;
}
/* the frame-start translations of the emitters moved on this frame's earlier steps */
struct Follow { unsigned ea, group; float start[3], set[3]; };
static Follow s_follow[512];
static unsigned s_followN = 0;
inline Follow* follow_find(unsigned ea) {
    for (unsigned i = 0; i < s_followN; i++)
        if (s_follow[i].ea == ea) return &s_follow[i];
    return nullptr;
}
/* the bow wave's places of the previous frame's earlier steps (its executeAfter moves the emitter after the calc read
 * the centre, so the 30 Hz frame's particles follow the previous frame's place: at step k of a frame the centre is the
 * place step k of the frame before set, one frame behind as in A, and the tick30 step's centre is A's) */
struct WaveLag { unsigned ea; u32 stamp[4]; float p[4][3]; };
static WaveLag s_wlag[64];
inline WaveLag* wave_lag(unsigned ea, bool make) {
    WaveLag* old = nullptr;
    for (unsigned i = 0; i < 64u; i++) {
        if (s_wlag[i].ea == ea) return &s_wlag[i];
        u32 newest = 0;
        for (unsigned k = 0; k < 4u; k++) newest = s_wlag[i].stamp[k] > newest ? s_wlag[i].stamp[k] : newest;
        if (!old && (s_wlag[i].ea == 0 || t60_step_counter() - newest > 8u)) old = &s_wlag[i];
    }
    if (!make || !old) return nullptr;
    *old = WaveLag{};
    old->ea = ea;
    return old;
}
inline void follow_move(unsigned e, const float* to) {
    Follow* f = follow_find(e);
    if (!f) {
        if (s_followN >= 512u) return;
        f = &s_follow[s_followN++];
        f->ea = e;
        f->group = fo_b(e + kFoGroup);
        for (unsigned i = 0; i < 3; i++) f->start[i] = fo_f(e + kFoTrans + 4 * i);
    }
    for (unsigned i = 0; i < 3; i++) {
        f->set[i] = to[i];
        *gabi::at<be<f32>>(e + kFoTrans + 4 * i) = to[i];
    }
}
/* an earlier step's calc of an emitter, where the callback's own move would be: after = false at its execute (before
 * JPACalcEmitterInfo: the step's centre is the owner's place of this step), after = true at its executeAfter (the bow
 * wave: there the owner's place of this step (the draw's head vertex reads it), noted for the next frame's same step;
 * before the info its centre is the place noted one frame earlier, see WaveLag) */
template <class Emitter, class Callback> inline void follow_step(Emitter* em, bool after) {
    if (frame_count() || !(t60::dt() < 1.0f) || !follow_on()) return;
    const unsigned e = gabi::ea(em), kind = follow_kind(e);
    if (!kind) return;
    const bool wave = kind == 0x025A92F4u;
    if (wave && !after) {
        const unsigned k = t60::step();
        WaveLag* w = k < 4u ? wave_lag(e, false) : nullptr;
        if (w && w->stamp[k] && t60_step_counter() - w->stamp[k] == t60::steps()) follow_move(e, w->p[k]);
        return;
    }
    if (wave != after) return;
    float to[3];
    if (!follow_place<Emitter, Callback>(em, kind, to)) return;
    if (wave) {
        const unsigned k = t60::step();
        if (WaveLag* w = k < 4u ? wave_lag(e, true) : nullptr) {
            w->stamp[k] = t60_step_counter();
            for (unsigned i = 0; i < 3; i++) w->p[k][i] = to[i];
        }
    }
    follow_move(e, to);
}
/* the emitter's tick30 visit (the manager, before anything of the frame's calc): the frame-start translation back */
inline void follow_restore(unsigned e) {
    if (!s_followN || !frame_count()) return;
    for (unsigned i = 0; i < s_followN; i++) {
        if (s_follow[i].ea != e) continue;
        const Follow f = s_follow[i];
        s_follow[i] = s_follow[--s_followN];
        for (unsigned k = 0; k < 3; k++)
            if (fo_f(e + kFoTrans + 4 * k) != f.set[k]) return;  /* set since (an actor): its value stays */
        for (unsigned k = 0; k < 3; k++) *gabi::at<be<f32>>(e + kFoTrans + 4 * k) = f.start[k];
        return;
    }
}
/* end of a group's tick30 visit: entries left belonged to emitters deleted since */
inline void follow_end(unsigned group) {
    if (!s_followN || !frame_count()) return;
    for (unsigned i = 0; i < s_followN;)
        if (s_follow[i].group == group) s_follow[i] = s_follow[--s_followN];
        else i++;
}
/* WWHD_T60_EMTRLOG (debug) */
inline FILE* follow_log_file() {
    static FILE* f = [] {
        const char* p = getenv("WWHD_T60_EMTRLOG");
        FILE* o = p ? fopen(p, "w") : nullptr;
        if (o) setvbuf(o, nullptr, _IOFBF, 1 << 16);
        return o;
    }();
    return f;
}
template <class Emitter, class Callback> inline void follow_log(Emitter* em, unsigned group) {
    FILE* f = follow_log_file();
    if (!f) return;
    const unsigned e = gabi::ea(em), kind = follow_kind(e);
    if (!kind) return;
    float own[3] = {0, 0, 0};
    const bool have = kind != 0x023D4528u && kind != 0x024140F8u && kind != 0x024151B8u && kind != 0x02415238u &&
                      follow_place<Emitter, Callback>(em, kind, own);
    unsigned n = 0, head = 0;
    double c[3] = {0, 0, 0};
    for (unsigned l = fo_w(e + kFoPtcls); l && n < 4096; l = fo_w(l + 12)) {
        const unsigned p = fo_w(l);
        if (!p) continue;
        if (!head) head = p;
        n++;
        for (unsigned k = 0; k < 3; k++) c[k] += fo_f(p + 0x28 + 4 * k);
    }
    fprintf(f, "%u %d %u %u %08X %08X %X %.3f %.3f %.3f %d %.3f %.3f %.3f %u %.3f %.3f %.3f %.3f %.3f %.3f %08X %08X %.3f %.3f %.3f %.3f %.3f %.3f\n", t60_step_counter(),
            t60::tick30() ? 1 : 0, fo_w(0x101FF558u), group, e, kind, fo_w(e + 0x84) /* mDataFlag (8 FollowEmtr) */, fo_f(e + kFoTrans), fo_f(e + kFoTrans + 4),
            fo_f(e + kFoTrans + 8), have ? 1 : 0, own[0], own[1], own[2], n, head ? fo_f(head + 0x28) : 0.0f,
            head ? fo_f(head + 0x2C) : 0.0f, head ? fo_f(head + 0x30) : 0.0f, n ? c[0] / n : 0.0, n ? c[1] / n : 0.0,
            n ? c[2] / n : 0.0, fo_w(e + 0x1E8), head ? fo_w(head + 0xC8) : 0u, head ? fo_f(head + 0x10) : 0.0f,
            head ? fo_f(head + 0x14) : 0.0f, head ? fo_f(head + 0x18) : 0.0f, head ? fo_f(head + 0x1C) : 0.0f,
            head ? fo_f(head + 0x20) : 0.0f, head ? fo_f(head + 0x24) : 0.0f);
}
}  // namespace jpa_t60
