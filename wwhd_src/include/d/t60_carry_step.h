/* t60_carry_step.h (WWHD, true 60 build; 2026-11-23, rule "convert the
 * whole-form carriables to per-step logic": the exact-frame-move family, frame ends bit-exact,
 * decisions on the frame grid, the predict form for the moves). Decomp build only.
 * Separate file: the actor sources keep thin calls. Shared by the carryables
 * (d_a_obj_barrel, d_a_tsubo_mode, d_a_stone, d_a_stone2) and the bombs (d_a_bomb, d_a_bomb2).
 *
 * The step form (replaces the whole form of d/t60_whole.h, which stays as the negative control):
 *   - every step of the 30 Hz frame runs the actor's own execute. The frame's earlier steps (phase PIC) run its motion
 *     code on the current frame: the mode's moves, chases, rolls, the ground check and the matrix step, as the frame's
 *     dt so far. They make no decisions (mode changes, breaks, carry / throw, timers, RNG draws, sounds, effects, the
 *     collider registration: the one-sweep-per-frame attack rule). The actor's code gates those blocks with `if (!t60cs::pic())`;
 *   - the frame's moves predict (form (b)): the frame's first call of a mover runs the original move at dt 1 from
 *     the frame start on a copy of the words it writes (position, speed, speedF, gravity, angles, the actor's extra
 *     words), records the frame end and puts the words back. Each earlier step shows the position on the frame's move
 *     p0 + D s + dvy / 2 s (s - 1) (s the frame's dt so far; C1 along the speed change). The speed words are never
 *     committed on an earlier step: a frame whose tick30 run makes no move (a break, a pick-up) leaves the words as A;
 *   - an earlier step runs no ground check (CrrPos has world side effects: the ride callbacks, the moving-background
 *     carry); its place only does not go below the ground the frame-start check found (the picture floor), so the Acch
 *     words stay the frame-start state the tick30 run reads, as A; the picture's model matrix and the matrix stack go
 *     back on the tick30 step (a run that sets no matrix leaves A's);
 *   - guest memory is copied word by word through be<u32> views (the verify harness maps guest pages on access);
 *   - the frame's tick30 step (phase TICK) puts the frame-start words back (old / current place, shape angle, speed,
 *     speedF, gravity and the actor's motion words, latched on the frame's first step; a held actor keeps the place and
 *     angles its holder gave on this step) and runs the original execute at dt 1: every decision with the other
 *     actors' current state, the original move, CrrPos and bg checks, the timers, the collider registered once (one-sweep-per-frame attack),
 *     sounds and effects once. The frame end is the original's bit for bit (N-step spec / --admp-exact).
 * dt 1 (N 1): phase WHOLE, never consulted (the original). NC WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form (d/t60_whole.h)
 * runs instead (the actor calls t60w::begin / end under t60cs::nc()).
 * Host memory per translation unit (the split-actor state rule (1)), keyed by actor address and process id. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h"
#include "d/t60_carry_floor.h" /* the plane floor (t60cf::floorAt) */
#include <cstdlib>

namespace t60cs {

static inline bool nc() {
#ifdef T60C_NEGCTRL
    return true; /* the tool negative control (tools/t60conv): the whole form, whose T60C_NEGCTRL runs the execute per step */
#elif defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CARRYSTEP_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline bool on() { return t60::dt() != 1.0f && !nc(); }

enum Phase { WHOLE = 0, PIC = 1, TICK = 2 };
/* an actor's extra words (offsets from the actor base; lengths in whole words) */
struct Range {
    u16 off, len;
};
enum { kRanges = 8, kExtra = 192, kSlots = 64, kGlide = 16, kActorMax = 0x1000 };

/* guest memory through be<u32> views only, word by word (the verify harness maps guest pages on access: no raw host copy
 * of guest memory); the host buffers hold the words as values */
static inline void gget(u32* dst, u32 ea, u32 len) {
    for (u32 i = 0; i < len / 4; i++)
        dst[i] = *gabi::at<be<u32>>(ea + 4 * i);
}
static inline void gput(u32 ea, const u32* src, u32 len) {
    for (u32 i = 0; i < len / 4; i++)
        *gabi::at<be<u32>>(ea + 4 * i) = src[i];
}

/* the actor's frame-start words: old / current place + shape angle (0x300..0x330), speed (0x33C..0x348), speedF / gravity /
 * maxFallSpeed (0x370..0x37C) */
enum { kBaseA = 0x300, kBaseALen = 0x30, kBaseB = 0x33C, kBaseBLen = 0xC, kBaseC = 0x370, kBaseCLen = 0xC };
/* the picture's model words: J3DModel base scale (+0xBC) and base matrix (+0xC8), 15 words; the matrix stack's now
 * (mDoMtx_stack_c, 0x1048D0CC), 12 words */
enum { kMdlOff = 0xBC, kMdlLen = 15 * 4, kStackEa = 0x1048D0CC, kStackLen = 12 * 4 };

enum { kOldLen = 0x14, kWords = (kBaseALen - kOldLen + kBaseBLen + kBaseCLen + kExtra) / 4 };
struct Frame {
    u32 actor, pcid, tag;
    u32 a[kBaseALen / 4];  /* the place block at the frame's first step (old place, current place, shape angle) */
    u32 base[kWords];      /* the tracked words as A has them: the frame start plus every other writer's change */
    u32 last[kWords];      /* the tracked words as the actor's last PIC step left them */
    u32 n;                 /* tracked words in use */
    f32 span;     /* the frame's dt so far (this step included) */
    f32 p0[3];    /* frame-start position */
    bool moved;   /* the frame's move is predicted */
    f32 m0[3];    /* the move's start: the place at the frame's first mover call */
    f32 D[3], dvy; /* the frame's move and its speed.y change */
    f32 s0;       /* the frame's dt before the step that predicted the move (a move first called on a later step) */
    bool glided;  /* the frame's glide is predicted */
    u32 glOff, glN;
    f32 gl0[kGlide], gl1[kGlide]; /* the glided values at the frame start and the frame end */
    bool picSaved; /* the model / stack words before the frame's first picture matrix */
    u32 mdlEa, mdl[kMdlLen / 4], stk[kStackLen / 4];
    u32 prevEnd[kOldLen / 4], prevTag; /* the place block as the previous frame's tick30 run left it */
};
static Frame s_frame[kSlots];
static Phase s_phase = WHOLE;
static Frame* s_cur = 0;
static f32 s_prevDt = 1.0f;
static u32 s_size = 0;
static u32 s_modelOff = 0;
static bool s_held = false;
static u32 s_w[kActorMax / 4];
static u32 s_stamp = ~0u; /* the step counter begin() ran on: a phase left by a run that never reached end() (an aborted
                           * call, the verify harness's next input) is no phase */ /* a whole-actor copy (prediction, picture check) */

/* the spec an actor header gives */
template <class A> struct Spec {
    Range extra[kRanges]; /* the actor's motion words an earlier step may change (0-length ends the list) */
    bool (*held)(A*);     /* its holder places it on every step (carried): place and angles stay live on tick30 */
    u32 size;             /* the actor's size: a PIC step's predicted move runs on a copy of all of it */
    u32 modelOff;         /* the actor's J3DModel pointer word (the picture's model matrix put back on tick30) */
    bool (*carried)(A*);  /* the holder's carry status bits alone (held without them: released this frame) */
};

static inline u32 extraLen(const Range* r) {
    u32 n = 0;
    for (int i = 0; i < kRanges && r[i].len; i++)
        n += r[i].len;
    return n;
}
static inline void copyOut(u32 base, const Range* r, u32* dst) {
    for (int i = 0; i < kRanges && r[i].len; i++) {
        gget(dst, base + r[i].off, r[i].len);
        dst += r[i].len / 4;
    }
}
static inline void copyIn(u32 base, const Range* r, const u32* src) {
    for (int i = 0; i < kRanges && r[i].len; i++) {
        gput(base + r[i].off, src, r[i].len);
        src += r[i].len / 4;
    }
}

/* the tracked words: current place + shape angle (0x314..0x330), speed, speedF / gravity / maxFallSpeed, the extras */
static inline u32 gather(u32 actor, const Range* extra, u32* w) {
    gget(w, actor + kBaseA + kOldLen, kBaseALen - kOldLen);
    u32 n = (kBaseALen - kOldLen) / 4;
    gget(w + n, actor + kBaseB, kBaseBLen), n += kBaseBLen / 4;
    gget(w + n, actor + kBaseC, kBaseCLen), n += kBaseCLen / 4;
    if (extraLen(extra) <= kExtra) {
        copyOut(actor, extra, w + n);
        n += extraLen(extra) / 4;
    }
    return n;
}
static inline void scatter(u32 actor, const Range* extra, const u32* w) {
    gput(actor + kBaseA + kOldLen, w, kBaseALen - kOldLen);
    u32 n = (kBaseALen - kOldLen) / 4;
    gput(actor + kBaseB, w + n, kBaseBLen), n += kBaseBLen / 4;
    gput(actor + kBaseC, w + n, kBaseCLen), n += kBaseCLen / 4;
    if (extraLen(extra) <= kExtra)
        copyIn(actor, extra, w + n);
}
static const Range* s_extra = 0;

static inline Frame* slot(u32 actor, u32 pcid) {
    Frame* freeS = 0;
    const u32 tag = t60w::frameTag();
    for (u32 i = 0; i < kSlots; i++) {
        Frame& f = s_frame[i];
        if (f.actor == actor && f.pcid == pcid)
            return &f;
        if (!freeS && (f.actor == 0 || f.tag + 2 < tag))
            freeS = &f;
    }
    Frame* f = freeS ? freeS : &s_frame[(actor >> 4) % kSlots];
    f->actor = actor;
    f->pcid = pcid;
    f->tag = ~0u;
    f->prevTag = ~0u;
    return f;
}

/* the execute's head: the phase of this step. PIC: an earlier step of the frame (motion as a picture, no decisions);
 * TICK: the tick30 step, the frame-start words put back and dt 1 until end(); WHOLE: dt 1 / NC (the original) */
template <class A> static inline Phase begin(A* a, const Spec<A>& s) {
    s_phase = WHOLE;
    s_cur = 0;
    s_stamp = t60_step_counter();
    s_size = s.size <= kActorMax ? s.size & ~3u : 0;
    s_modelOff = s.modelOff;
    if (!on())
        return WHOLE;
    const u32 actor = gabi::ea(a), tag = t60w::frameTag();
    Frame* f = slot(actor, *gabi::at<be<u32>>(actor + 0x04));
    s_cur = f;
    if (t60::first_step() || f->tag != tag) {
        f->tag = tag;
        gget(f->a, actor + kBaseA, kBaseALen);
        f->n = gather(actor, s.extra, f->base);
        for (u32 i = 0; i < f->n; i++)
            f->last[i] = f->base[i];
        f->p0[0] = a->current.pos.x, f->p0[1] = a->current.pos.y, f->p0[2] = a->current.pos.z;
        f->span = 0.0f;
        f->moved = false;
        f->glided = false;
        f->picSaved = false;
    }
    f->span = gabi::fadds_ppc(f->span, t60::dt());
    t60wc::note(actor); /* extension: the frame's 30 Hz carrier for its riders (as the whole form) */
    s_held = s.held(a);
    s_extra = s.extra;
    /* a word another party wrote since the actor's last PIC step (Link's throw speed and angle,
     * his placement of a carried actor, the hookshot) is A's: it becomes the word's base; only the actor's own PIC
     * changes are put back on the tick30 step */
    {
        u32 cur[kWords];
        gather(actor, s.extra, cur);
        for (u32 i = 0; i < f->n; i++)
            if (cur[i] != f->last[i])
                f->base[i] = cur[i];
    }
    if (!t60::tick30()) {
        s_phase = PIC;
        return PIC;
    }
    /* tick30: the tracked words as A has them (the actor's own PIC changes undone, every other writer's kept); the old
     * place fopAc_Execute copied from the last picture back to the frame start's (held: its holder's tick30 placement, as A
     * has it after the holder's frame) */
    scatter(actor, s.extra, f->base);
    /* (a holder that released the actor on an earlier step placed it no more; the old
     * place fopAc_Execute copied is then the earlier step's picture: the frame start's, as A) */
    if (!s_held) {
        gput(actor + kBaseA, f->a, kOldLen);
    } else if (s.carried && !s.carried(a)) {
        /* a holder that released the actor (rule): A's old place is the place the previous frame left
         * (nobody placed it after the release); the first step's copy may already hold the holder's first-step place.
         * (Not taken from "not re-placed since the last PIC step": a holder's tick30 place equal to its first-step place
         * is a placement, stt7 frame 228; the void case stt6 frame 295, Link placing on his first step and voiding on his
         * tick30 step, is Link's own step order and stays) */
        /* the position only: the old angles / room fopAc copied keep the holder's own writes of this frame (Link's
         * procGrabThrow sets current.angle.y before freeGrabItem; bomb pair btH: A's old.angle.y 0x3B17) */
        gput(actor + kBaseA, f->prevTag + 1 == tag ? f->prevEnd : f->a, 12);
    }
    /* the pictures' model matrix and matrix stack back (a run that sets no matrix, e.g. a delete, leaves A's) */
    if (f->picSaved) {
        if (f->mdlEa)
            gput(f->mdlEa + kMdlOff, f->mdl, kMdlLen);
        gput(kStackEa, f->stk, kStackLen);
        f->picSaved = false;
    }
    s_prevDt = t60_set_dt(1.0f);
    t60py_swap_begin(); /* (the process-order read rule addendum 2) Link's reset flags as his whole frame, as the whole form */
    s_phase = TICK;
    return TICK;
}
/* the execute's end (every return path) */
static inline void end() {
    if (s_phase == PIC && s_cur) /* the words as this PIC step leaves them */
        gather(s_cur->actor, s_extra, s_cur->last);
    if (s_phase == TICK) {
        if (s_cur) { /* the place this frame leaves, for the next frame's old place */
            gget(s_cur->prevEnd, s_cur->actor + kBaseA + kOldLen, kOldLen);
            s_cur->prevTag = s_cur->tag;
        }
        t60_set_dt(s_prevDt);
        t60py_swap_end();
    }
    s_phase = WHOLE;
    s_cur = 0;
}
/* the phase of this step (WHOLE outside a begin() of this step) */
static inline Phase phase() { return (s_cur && s_stamp == t60_step_counter() && t60::dt() != 1.0f) ? s_phase : WHOLE; }
/* this step is an earlier step of a converted frame: motion only, no decisions */
static inline bool pic() { return phase() == PIC; }

/* a mover call (fopAcM_posMoveF, daObj::posMoveF_stream / _grade, ...): `fn(a, args...)`. PIC: the predicted frame move;
 * else the call itself (dt 1) */
template <class A, class Fn, class... Args> static inline void move(A* a, Fn fn, Args... args) {
    if (phase() != PIC || !s_cur || !s_size) {
        if (phase() != PIC)
            fn(a, args...);
        return;
    }
    Frame* f = s_cur;
    const u32 actor = gabi::ea(a);
    if (!f->moved) {
        /* the frame's move at dt 1 on a copy of the whole actor; the move starts where this call finds the place (the frame
         * start plus what the call did before it, e.g. the pot's water tension), as A's single call */
        gget(s_w, actor, s_size);
        f->m0[0] = a->current.pos.x, f->m0[1] = a->current.pos.y, f->m0[2] = a->current.pos.z;
        const f32 vy0 = a->speed.y;
        const f32 dt = t60_set_dt(1.0f);
        fn(a, args...);
        t60_set_dt(dt);
        f->D[0] = (f32)a->current.pos.x - f->m0[0];
        f->D[1] = (f32)a->current.pos.y - f->m0[1];
        f->D[2] = (f32)a->current.pos.z - f->m0[2];
        f->dvy = (f32)a->speed.y - vy0;
        gput(actor, s_w, s_size);
        f->moved = true;
        f->s0 = f->span - t60::dt(); /* (2026-11-23: a move first called on a later step, e.g. after a release
                                      * on step 2 / 3 at N 4, runs its frame move over the frame's rest) */
    }
    const f32 s = f->s0 < 1.0f ? (f->span - f->s0) / (1.0f - f->s0) : 1.0f;
    a->current.pos.x = f->m0[0] + f->D[0] * s;
    a->current.pos.y = f->m0[1] + f->D[1] * s + 0.5f * f->dvy * s * (s - 1.0f);
    a->current.pos.z = f->m0[2] + f->D[2] * s;
}

/* a visual-only per-frame update (the pot's pose tensor, the stone's tilt: spins and their damping; the 30 Hz visual-motion rule /
 * whole-frame form: the original update once a frame, the earlier steps on the straight line to its frame end). PIC: the first call
 * of the frame runs `fn(args...)` at dt 1 on a copy of the whole actor placed at the frame's predicted end (current =
 * the move's end, old = the frame start) and records the n values (T: f32, or s16 angles: the wrapped difference) at
 * `off` it gives; each PIC call writes start + (end - start) x s. `fn` puts back any word outside the actor it writes.
 * Not PIC: the call itself */
template <class T> static inline f32 glGet(u32 ea) { return (f32)(T)*gabi::at<be<T>>(ea); }
template <class T> static inline void glPut(u32 ea, f32 v);
template <> inline void glPut<f32>(u32 ea, f32 v) { *gabi::at<be<f32>>(ea) = v; }
template <> inline void glPut<s16>(u32 ea, f32 v) { *gabi::at<be<s16>>(ea) = (s16)(s32)v; }
template <class T, class A, class Fn, class... Args> static inline void glideT(A* a, u32 off, u32 n, Fn fn, Args... args) {
    if (phase() != PIC || !s_cur || !s_size || n > kGlide) {
        if (phase() != PIC)
            fn(args...);
        return;
    }
    Frame* f = s_cur;
    const u32 actor = gabi::ea(a);
    if (!f->glided || f->glOff != off || f->glN != n) {
        gget(s_w, actor, s_size);
        for (u32 i = 0; i < n; i++)
            f->gl0[i] = glGet<T>(actor + off + sizeof(T) * i);
        gput(actor + kBaseA, f->a, 0x14); /* old place = the frame start */
        if (f->moved) {
            a->current.pos.x = f->m0[0] + f->D[0];
            a->current.pos.y = f->m0[1] + f->D[1];
            a->current.pos.z = f->m0[2] + f->D[2];
        }
        const f32 dt = t60_set_dt(1.0f);
        fn(args...);
        t60_set_dt(dt);
        for (u32 i = 0; i < n; i++) {
            const f32 e = glGet<T>(actor + off + sizeof(T) * i);
            /* an angle takes the short way (the wrapped difference) */
            f->gl1[i] = sizeof(T) == 2 ? f->gl0[i] + (f32)(s16)(s32)(e - f->gl0[i]) : e;
        }
        gput(actor, s_w, s_size);
        f->glided = true, f->glOff = off, f->glN = n;
    }
    const f32 s = f->span;
    for (u32 i = 0; i < n; i++)
        glPut<T>(actor + off + sizeof(T) * i, f->gl0[i] + (f->gl1[i] - f->gl0[i]) * s);
}
template <class A, class Fn, class... Args> static inline void glide(A* a, u32 off, u32 n, Fn fn, Args... args) {
    glideT<f32>(a, off, n, fn, args...);
}

/* the frame's horizontal / full move as the step sees it: PIC the predicted frame move (or (pos - p0) / s when the frame
 * has no predicted move); false when not PIC */
template <class A> static inline bool frameDelta(A* a, f32* d) {
    if (phase() != PIC || !s_cur)
        return false;
    Frame* f = s_cur;
    if (f->moved) {
        d[0] = f->D[0], d[1] = f->D[1], d[2] = f->D[2];
    } else {
        const f32 s = f->span > 0.0f ? f->span : 1.0f;
        d[0] = ((f32)a->current.pos.x - f->p0[0]) / s;
        d[1] = ((f32)a->current.pos.y - f->p0[1]) / s;
        d[2] = ((f32)a->current.pos.z - f->p0[2]) / s;
    }
    return true;
}
/* the frame-start value of an s16 word of the place / shape-angle block (offset 0x300..0x32E) on a PIC step (a target the
 * original picks from the frame-start value); `live` otherwise */
static inline s16 startS16(u32 off, s16 live) {
    if (phase() != PIC || !s_cur || off < kBaseA || off + 2 > kBaseA + kBaseALen)
        return live;
    const u32 w = s_cur->a[(off - kBaseA) / 4];
    return (s16)((off & 2) ? (w & 0xFFFF) : (w >> 16)); /* the guest word's value: big-endian halves */
}
/* the frame's dt so far (PIC), 1 otherwise */
static inline f32 span() { return (phase() == PIC && s_cur) ? s_cur->span : 1.0f; }

/* the picture floor: the ground height the actor's last ground check found (dBgS_Acch m_ground_h +0x94 plus
 * m_ground_up_h +0x8C); false when it found none */
static inline bool groundFloor(const void* acch, f32* y) {
    if (!acch)
        return false;
    const u32 ac = gabi::ea(acch);
    const f32 gh = *gabi::at<be<f32>>(ac + 0x94);
    if (!(gh > -1.0e9f))
        return false;
    *y = gabi::fadds_ppc(gh, *gabi::at<be<f32>>(ac + 0x8C));
    return true;
}
/* the ground check (`fn(args...)`: Acch CrrPos, the actor's own crr_pos; `acch` its dBgS_Acch). PIC: no check (it has
 * world side effects: dBgS::RideCallBack on ride-callback backgrounds, the moving-background carry), only a picture
 * floor: a step's place does not go below the ground the frame-start check found under the actor (dBgS_Acch m_ground_h
 * +0x94 plus m_ground_up_h +0x8C; none found: as it is; held: as its holder placed it). Not PIC: the call itself */
template <class A, class Fn, class... Args> static inline void check(A* a, const void* acch, Fn fn, Args... args) {
    if (phase() != PIC) {
        fn(args...);
        return;
    }
    f32 floor;
    /* the frame-start ground plane at the step's xz (a downhill roll without a 30 Hz staircase), the flat height when
     * the plane does not fit (d/t60_carry_floor.h) */
    if (s_held || !s_cur || !t60cf::floorAt(acch, s_cur->p0[0], s_cur->p0[2], a->current.pos.x, a->current.pos.z, &floor))
        return;
    if ((f32)a->current.pos.y < floor)
        a->current.pos.y = floor;
}
/* the same for a member CrrPos */
template <class A, class Acch, class Bgs> static inline void crrPos(A* a, Acch* acch, Bgs* bgs) {
    check(a, acch, [acch, bgs] { acch->CrrPos(bgs); });
}

/* the matrix step of a PIC step: held, at its holder's picture place (Link's bounce picture / the hookshot's place,
 * d/t60_held_pic.h); else as it is. The frame's first picture matrix keeps the model / stack words it overwrites for
 * the tick30 step */
template <class A, class F> static inline void mtxF(A* a, bool held, F fn) {
    if (phase() == PIC && s_cur && !s_cur->picSaved) {
        Frame* f = s_cur;
        f->mdlEa = s_modelOff ? (u32)*gabi::at<be<u32>>(gabi::ea(a) + s_modelOff) : 0;
        if (f->mdlEa)
            gget(f->mdl, f->mdlEa + kMdlOff, kMdlLen);
        gget(f->stk, kStackEa, kStackLen);
        f->picSaved = true;
    }
    if (phase() == PIC && held && !t60hp::nc())
        t60hp::with_grab_dy(gabi::ea(a), fn);
    else
        fn();
}
/* the same for a member matrix step (no lambda in the glued actor source) */
template <class A, class B> static inline void mtx(A* a, bool held, void (B::*fn)()) {
    mtxF(a, held, [a, fn] { (a->*fn)(); });
}

}  // namespace t60cs
