/* d_a_hookshot_t60.h (WWHD, true 60 build, 2026-11-17): d_a_hookshot.cpp on a general
 * step length (dt = 1/N), converted by hand (the procs sit behind the member-pointer dispatch). Decomp build
 * only.
 *
 * Converted, the hookshot executes on every step with g_dt = 1/N, but only while Link does (rule 14, coupled parties:
 * f_pc_executor.cpp runs it as an unconverted process while Link is unconverted or held, since Link's procHookshotFly
 * adds the hookshot's per-step pull vector on each of his steps). At dt = 1 every helper returns the original per-frame
 * value (dt-1 check bit-exact).
 *
 * Sites:
 *     execute    snd   11    chain / wind-up sounds: once per frame on the tick30 step, from the frame-start mode
 *     execute    misc  30 Hz visual-motion   one proc state step per 30 Hz frame: a proc that changes the state (setProc, mRebound)
 *                            on a step holds the dispatch for the frame's remaining steps (the new proc runs from
 *                            the next frame, as in the original)
 *     execute    mov   6     speedF = the step's distance / dt (a per-frame speed)
 *     procWait   ord   14    the shot start (Mode_Shot, set by Link on one of his steps) on the tick30 step only, from
 *                            Link's frame-end root and body angle (frame 112: the start on the
 *                            first step took a half-frame-old root); the in-between steps only follow the root
 *     procShot   tmr   first-step AI-timer   the range stop (>= 1500 from the root) tested on the frame's first step only
 *     procShot   mov   6     the tip's move 15 x mMoveVec x dt; frame move exact (t60c::moveBegin / moveEnd: tick30
 *                            lands on the frame start + the whole frame's move, the original's single add)
 *     procShot   ord   14    the BG line check (stick -> Mode_Pull, rebound) on the tick30 step only, over the whole
 *                            frame segment from the latched frame start (hs60::start): Link, earlier in the order,
 *                            sees the new mode from the next frame as in the original (pair)
 *     procPlayerPull ord 14  the pull decided on the tick30 step only (9 links, the arrival / return decisions) from
 *                            Link's frame-end root; the in-between steps hold (pair: per step,
 *                            Link added the frame's first-step vector on his second step and led by half a frame)
 *     procPlayerPull mov 6   mMoveVec = the frame's pull x dt: Link adds it on each of his N steps of the next frame
 *                            (the original: Link adds the frame's vector in the next frame)
 *     procPlayerPull mov 30 Hz visual-motion dBgS::MoveBgTransPos (the moving background's frame transform) once per frame, tick30
 *     procReturn ord   14    the arrival test (chain <= 9 links) on the tick30 step from the frame-start position and
 *                            chain count (hs60::start) and Link's frame-end root: the return to Mode_Wait shows to Link
 *                            from the next frame (on the first step Link switched his animation one
 *                            frame early, frame 153)
 *     procReturn misc  14    9 chain links a frame, taken on the tick30 step; a carried actor follows the
 *                            tip on every step, the carry offset's ground correction is written on the tick30 step
 *                            only, and an arriving frame puts back the carried actor's position (hs60::Carry)
 *     procReturn ord   14    a carried actor that runs at 30 Hz in the frame (unconverted, held, or in the whole form:
 *                            hs60::carry30) is placed on the tick30 step only, from the frame-end tip, the original's
 *                            one placement a frame (routes pair frame 403:
 *                            Homen, earlier in the line, cancels the carry in its tick30 execute (falldown init), so
 *                            A's hookshot never moves it that frame; B's first step had carried it half a return step,
 *                            0.14 / 5.4 / 28.5 off for the rest of the route)
 *     procShot   ord   14    the AT hit and the range stop act on the tick30 step from the unmoved frame start (the
 *                            range answer taken on the first step and held); a frame that stops does not move
 *
 *     procReturn mov   6     the return move 63 units per frame: the in-between steps along the first step's direction
 *                            (hs60_latch), the tick30 step lands exactly on the original's frame move, computed from
 *                            the frame start and Link's frame-end root (moveBegin / moveEnd)
 */
#pragma once
#include <stdlib.h>
#include <cstring>
#include "d/t60conv.h"
#include "d/t60_whole_carrier.h"

namespace hs60 {

/* the frame's return vector (procReturn), latched on the frame's first step: one hookshot exists (Link's) */
struct Latch {
    u32 ea, tag;
    f32 x, y, z;
};
static Latch s_latch;

static inline void latch(const void* k, f32 x, f32 y, f32 z) {
    s_latch.ea = gabi::ea(k);
    s_latch.tag = t60c::frameTag();
    s_latch.x = x;
    s_latch.y = y;
    s_latch.z = z;
}
/* the latched frame vector of this frame, false when none was latched on this frame's first step */
static inline bool latched(const void* k, f32* x, f32* y, f32* z) {
    if (s_latch.ea != gabi::ea(k) || s_latch.tag != t60c::frameTag())
        return false;
    *x = s_latch.x;
    *y = s_latch.y;
    *z = s_latch.z;
    return true;
}

/* the frame-start state, latched on the frame's first step: the decisions that change the hookshot's
 * mode (Link, earlier in the process order, reads it) run on the frame's tick30 step from it, as the original's single
 * execute at the frame end: the shot's line check over the whole frame segment, the return's arrival test */
struct Start {
    u32 ea, tag;
    f32 x, y, z;
    s32 chain;
};
static Start s_start;

static inline void latchStart(const void* k, const cXyz& p, s32 chain) {
    s_start.ea = gabi::ea(k);
    s_start.tag = t60c::frameTag();
    s_start.x = p.x;
    s_start.y = p.y;
    s_start.z = p.z;
    s_start.chain = chain;
}
/* this frame's start state; false when none was latched on this frame's first step */
static inline bool start(const void* k, cXyz* p, s32* chain) {
    if (s_start.ea != gabi::ea(k) || s_start.tag != t60c::frameTag())
        return false;
    p->x = s_start.x;
    p->y = s_start.y;
    p->z = s_start.z;
    *chain = s_start.chain;
    return true;
}

/* (route hook, pair hookM: the hit item's tg word +0x6DC, its mGObjTg +0x34 = the attacker's at
 * vector: A 105, B 52.5): procShot registers its sight capsule from current.pos to the step's end on every step; the
 * collision Move runs once a frame on the tick30 step and saw the tick30 step's capsule, from the half-frame place: half
 * the frame's segment (a hit on the frame's first half could be missed) and half its at vector. The tick30 step's
 * capsule now starts at the frame start (the latched start), so it spans the frame's segment as the original's one
 * capsule (its end is already the frame's end: the whole-segment line check). Negative control
 * WWHD_T60_HKCPS_NEGCTRL=1. true: `out` holds the frame start. */
static inline bool cpsFromStart(const void* k, cXyz* out) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_HKCPS_NEGCTRL"); return e && e[0] == '1'; }();
    if (nc)
        return false;
#endif
    s32 chain;
    return start(k, out, &chain);
}

/* procReturn with a carried actor (N-step spec with the procs compared): the in-between steps carry the actor
 * along with the tip (visual); a frame whose tick30 arrival test ends the return does not move in the original, so the
 * tick30 step puts back the words those steps wrote: the carried actor's current.pos and the ground probe's m_pos,
 * latched on the frame's first step */
struct Carry {
    u32 ea, tag;
    u32 actorPos, probePos;  /* guest addresses of the two cXyz */
    u32 words[6];
};
static Carry s_carry;

static inline void latchCarry(const void* k, u32 actorPos, u32 probePos) {
    s_carry.ea = gabi::ea(k);
    s_carry.tag = t60c::frameTag();
    s_carry.actorPos = actorPos;
    s_carry.probePos = probePos;
    for (int i = 0; i < 3; i++) {
        s_carry.words[i] = *gabi::at<be<u32>>(actorPos + 4 * i);
        s_carry.words[3 + i] = *gabi::at<be<u32>>(probePos + 4 * i);
    }
}
/* put back this frame's latched words (nothing when none were latched on this frame's first step) */
static inline void restoreCarry(const void* k, u32 actorPos) {
    if (s_carry.ea != gabi::ea(k) || s_carry.tag != t60c::frameTag() || s_carry.actorPos != actorPos)
        return;
    for (int i = 0; i < 3; i++) {
        *gabi::at<be<u32>>(s_carry.actorPos + 4 * i) = s_carry.words[i];
        *gabi::at<be<u32>>(s_carry.probePos + 4 * i) = s_carry.words[3 + i];
    }
}

/* the carried actor runs at 30 Hz in this frame: not converted, held, or in the whole form (the whole-frame form rule,
 * d/t60_whole_carrier.h). It reads and writes its position on the tick30 step only, so the hookshot places it there
 * only: an actor earlier in the line that cancels the carry in its tick30 execute (fopAcM_cancelHookCarryNow
 * / its own status bit) then sees and keeps the frame-start placement, as A's actor before A's hookshot; one later in
 * the line sees the frame-end placement, as A's. At dt 1 never asked (the caller's gate). Negative control
 * WWHD_T60_HKCARRY_NEGCTRL=1 (game build). */
static inline bool carry30(s16 profName, u32 procId, u32 actor) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    /* negative control WWHD_T60_HKCARRY_NEGCTRL=1: the per-step carry of every actor (the form before) */
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_HKCARRY_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl)
        return false;
#endif
    return !t60_converted(profName) || t60_held(procId) || t60wc::ranThisFrame(actor);
}

/* the picture place of a carried actor the hookshot does not place on this earlier step
 * (carry30: it places it on the tick30 step only, the original's one placement a frame). Its held picture (a whole-form
 * field item, d/t60_held_pic.h) drew it on the earlier steps at the previous frame end's placement while the tip moved
 * on: a 30 Hz staircase against the tip. Here an earlier step that does not place it (carry30; an actor the hookshot
 * places on this step draws from that placement) notes the place it would have given (this step's tip +
 * mCarryOffset; the offset's ground correction stays the tick30 step's) for the held picture, host memory only (no
 * guest word; the ground probe is not run). Game build, N > 1. Negative control WWHD_T60_HOOKPIC_NEGCTRL=1. */
#if defined(GABI_GAME_BUILD) && GABI_T60
struct Pic {
    u32 actor = 0, step = ~0u;
    f32 p[3] = {0, 0, 0};
    bool arrive = false; /* the place is the arrival's (notePicArrive): kept over notePic on the same step */
};
static Pic s_pic;
static inline void notePic(u32 actor, const cXyz& tip, const cXyz& off) {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_HOOKPIC_NEGCTRL"); return e && e[0] == '1'; }();
    if (nc)
        return;
    if (s_pic.arrive && s_pic.actor == actor && s_pic.step == t60_step_counter())
        return;
    s_pic.arrive = false;
    s_pic.actor = actor;
    s_pic.step = t60_step_counter();
    s_pic.p[0] = tip.x + off.x;
    s_pic.p[1] = tip.y + off.y;
    s_pic.p[2] = tip.z + off.z;
}
/* the arrival (route hook, hookI: the item drawn 31.5 past its arrival place on the arriving
 * frame's earlier pass, then put back by the tick30 step's restoreCarry): a frame whose arrival test ends the return
 * does not move the carried actor (the original; restoreCarry puts back its frame-start place), but the test is decided
 * on the tick30 step only, so the earlier steps carried it on along the tip. On an earlier step the same test is taken
 * from the frame start and Link's root of this step (the tick30 step uses his frame-end root); when it says the frame
 * arrives, the held picture shows the carried actor at its frame-start place (the latched words restoreCarry puts
 * back). Picture only (the placement words and the decision stay); host memory. A wrong guess near the threshold shows
 * one pass at the frame-start place. Negative control WWHD_T60_HOOKARRIVE_NEGCTRL=1. */
static inline void notePicArrive(const void* k, u32 actor, u32 actorPos, bool arrives) {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_HOOKARRIVE_NEGCTRL"); return e && e[0] == '1'; }();
    if (nc || !arrives || s_carry.ea != gabi::ea(k) || s_carry.tag != t60c::frameTag() || s_carry.actorPos != actorPos)
        return;
    s_pic.actor = actor;
    s_pic.step = t60_step_counter();
    s_pic.arrive = true;
    for (int i = 0; i < 3; i++) {
        const u32 w = s_carry.words[i];
        f32 f;
        memcpy(&f, &w, 4); /* (host words: the latched copy, not guest memory) */
        s_pic.p[i] = f;
    }
}
#else
static inline void notePic(u32, const cXyz&, const cXyz&) {}
static inline void notePicArrive(const void*, u32, u32, bool) {}
#endif
}  // namespace hs60
#if defined(GABI_GAME_BUILD) && GABI_T60
/* the held picture's place for `actor` noted on this step (d/t60_held_pic.h); false: none */
extern "C" __attribute__((visibility("default"))) bool wwhd_t60_hook_pic_pos(uint32_t actor, float* out) {
    if (actor == 0 || hs60::s_pic.actor != actor || hs60::s_pic.step != t60_step_counter() || t60::tick30())
        return false;
    for (int i = 0; i < 3; i++)
        out[i] = hs60::s_pic.p[i];
    return true;
}
#endif
