/* d_a_boomerang_t60.h (WWHD, true 60 build, 2026-11-18): d_a_boomerang.cpp on a general
 * step length (dt = 1/N), converted by hand (the procs sit behind the guest-table dispatch). Decomp build
 * only.
 *
 * Form (option (a)): the boomerang's decisions (the homing re-aim, the catch, the hits, the
 * throw) and its exact frame move run on the frame's tick30 step from the frame start latched on the first step, as the
 * original's single execute at the frame end; the steps in between are visual only (spin, a straight glide along the
 * frame-start heading, no collision). The flight path equals the 30 Hz path (pair: the per-step homing missed
 * the pillar). At dt = 1 every gate takes the original path (dt-1 check bit-exact).
 *
 * Sites:
 *     sightPacket::play  vis  8    the lock-on sight frames once per frame (tick30)
 *     execute            misc 30 Hz visual-motion  one proc state step per 30 Hz frame: a proc change on a step holds the dispatch for
 *                                  the frame's remaining steps
 *     procWait           ord  14   the throw (mParameters 1, set by Link on one of his steps) on the tick30 step only,
 *                                  from Link's frame-end pose; the in-between steps only keep the matrix
 *     procWait           cc   one-sweep-per-frame attack  the sight line (lock-on: AT collider, hit callback) registered on the tick30 step only
 *     procWait           vis  8    the trail fade once per frame (tick30)
 *     procMove           ord  14   the in-between steps: visual only (spin share, glide along the frame-start heading
 *                                  by speedF x dt, model matrix); the tick30 step restores the frame start (current and
 *                                  old position, bm60::start) and runs the original frame: re-aim, catch, hits, turn
 *                                  limit, move speedF, BG line check, roll, collision set, water effect
 *     procMove           mov  6    the spin t60c::share(0x1F00) on every step (exact 0x1F00 per frame; the wrap sound
 *                                  per wrap)
 *     procMove           vis  8    the blur trail copy once per frame (tick30); (2026-11-23): an
 *                                  earlier step copies a picture head (below)
 *     procWait           vis  8: an earlier step shows the frame's fade so far (picture, below)
 *     blur draw          vis  -: the trail drawn at the world's time (d/t60_draw_words.h)
 *
 * (picture head): the trail keeps one copy per 30 Hz frame (tick30), so between frame ends its head sat on
 * the previous frame end while the boomerang glided on per step. On an earlier step (procMove) the trail words (segment
 * count, head position, the four point lanes) are kept and the step's copy runs on them as a picture: the head on the
 * step's place, five points from the frame start to it. In procWait an earlier step shows the frame's fade so far
 * (5 x k / N segments). The next execute puts the kept words back first, so the tick30 step's copy / fade starts from the
 * frame start's words and every frame end has the 30 Hz words. Host memory (one boomerang, Link's). dt 1: never.
 * Negative control WWHD_T60_BOOMTRAIL_NEGCTRL=1: no picture (the form before). */
#pragma once
#include "d/t60conv.h"
#include "d/t60_draw_words.h"
#include <cstdlib>
#include <cstring>

namespace bm60 {

/* the frame-start position, latched on the frame's first step: one boomerang exists (Link's) */
struct Start {
    u32 ea, tag;
    f32 x, y, z;
    f32 ox, oy, oz; /* old.pos at the frame start (restored from its own latch, not from current.pos) */
};
static Start s_start;

static inline void latchStart(const void* k, f32 x, f32 y, f32 z) {
    s_start.ea = gabi::ea(k);
    s_start.tag = t60c::frameTag();
    s_start.x = x;
    s_start.y = y;
    s_start.z = z;
}
/* this frame's start position; false when none was latched on this frame's first step */
static inline void latchOld(const void* k, f32 x, f32 y, f32 z) {
    if (s_start.ea != gabi::ea(k) || s_start.tag != t60c::frameTag())
        return;
    s_start.ox = x;
    s_start.oy = y;
    s_start.oz = z;
}
static inline bool startOld(const void* k, f32* x, f32* y, f32* z) {
    if (s_start.ea != gabi::ea(k) || s_start.tag != t60c::frameTag())
        return false;
    *x = s_start.ox;
    *y = s_start.oy;
    *z = s_start.oz;
    return true;
}
/* the tick30 step used the latch: void it (a stale latch of the same address and frame tag, a procMove first
 * run on the tick30 step after procWait's throw, restored a position of another run) */
static inline void consume() { s_start.tag = ~s_start.tag; }
/* no latch of this frame: procWait ran the frame's first step, so a procMove first run on the tick30 step (the
 * throw) starts from the live position; drops a latch an earlier frame left unconsumed (the harness reuses one frame tag
 * across its inputs; in the game the tag check already rejects it) */
static inline void drop() { s_start.ea = 0; }
/* procWait's frame-start old.pos, latched on its first step: the throw falls through into procMove on the tick30
 * step, whose collision capsule / BG line start from old.pos; the game's per-step copy left the first step's position
 * there (the original's throw frame starts them from the previous frame's end, f115) */
struct WaitOld {
    u32 ea, tag;
    f32 x, y, z;
};
static WaitOld s_waitOld;
static inline void latchWaitOld(const void* k, f32 x, f32 y, f32 z) {
    s_waitOld.ea = gabi::ea(k);
    s_waitOld.tag = t60c::frameTag();
    s_waitOld.x = x;
    s_waitOld.y = y;
    s_waitOld.z = z;
}
static inline bool waitOld(const void* k, f32* x, f32* y, f32* z) {
    if (s_waitOld.ea != gabi::ea(k) || s_waitOld.tag != t60c::frameTag())
        return false;
    *x = s_waitOld.x;
    *y = s_waitOld.y;
    *z = s_waitOld.z;
    s_waitOld.ea = 0;
    return true;
}
static inline bool start(const void* k, f32* x, f32* y, f32* z) {
    if (s_start.ea != gabi::ea(k) || s_start.tag != t60c::frameTag())
        return false;
    *x = s_start.x;
    *y = s_start.y;
    *z = s_start.z;
    return true;
}


/* the trail words (daBoomerang_blur_c numTrailSegments, pos, mTrail[4][60]) at the actor's mBlur (0x2054) */
enum { kBlur = 0x2054, kTrailWords = 0x98, kTrailBytes = 0xBEC - 0x98 };
struct TrailPic {
    u32 ea;
    bool on;
    u8 words[kTrailBytes];
};
static TrailPic s_trailPic;
static inline bool trailNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BOOMTRAIL_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline u8* trailWords(const void* k) { return gabi::at<u8>(gabi::ea(k) + kBlur + kTrailWords); }
/* the execute's head: a picture left by the step before goes (the frame start's trail words back) */
static inline void trailRestore(const void* k) {
    if (!s_trailPic.on)
        return;
    s_trailPic.on = false;
    if (s_trailPic.ea == gabi::ea(k))
        memcpy(trailWords(k), s_trailPic.words, kTrailBytes);
}
/* an earlier step (not the tick30 step): keeps the trail words for the next execute and returns true (the caller then
 * changes them as a picture) */
static inline bool trailPicture(const void* k) {
    if (t60c::end() || trailNc())
        return false;
    s_trailPic.ea = gabi::ea(k);
    s_trailPic.on = true;
    memcpy(s_trailPic.words, trailWords(k), kTrailBytes);
    return true;
}
/* procWait's fade on an earlier step: the frame's 5 segments x k / N so far (rounded), never below 0 */
static inline s32 trailFadeSoFar(s32 segments) {
    const s32 n = (s32)t60::steps(), k = (s32)t60::step();
    const s32 cut = (10 * k + n) / (2 * n);
    return segments - cut > 0 ? segments - cut : 0;
}
}  // namespace bm60
