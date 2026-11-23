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
 *     procMove           vis  8    the blur trail copy once per frame (tick30)
 */
#pragma once
#include "d/t60conv.h"

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

}  // namespace bm60
