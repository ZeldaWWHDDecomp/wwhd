/* t60_whole_carrier.h (WWHD, true 60 build, 2026-11-20, extension):
 * which actors ran in the whole form (the whole-frame form rule + addendum, d/t60_whole.h) in the current 30 Hz frame. Decomp build
 * only. Separate file: thin calls in the sources.
 *
 * A whole-form actor moves its logic state and its collision (its dBgW) once a 30 Hz frame, on the tick30 step. As a
 * carrier it is a 30 Hz carrier for its riders, as an unconverted one is: Link must be held at 30 Hz on it
 * (d_a_player_main_04.cpp). Otherwise his in-between step stands on the carrier's previous-frame BG while A's Link,
 * later in the line, stands on this frame's (pair: the kita's tilt amplitude from Link's ride differed from
 * frame 88; with the kita unconverted (control) Link is held and the frame is A's).
 *
 * The record: the whole-form guard (t60w::begin, every step at dt != 1) notes the actor for this frame in the port-owned
 * set latch (true60.h wwhd_t60_latch_set; key: the actor's guest address | 3, no field address of a latched counter);
 * the rider asks it for this frame. A carrier later in the line than the rider is noted after the rider's first step:
 * the hold request is made on every step and takes effect from the next frame, so a later step's answer
 * is enough. Host memory only (the port's map), no guest word written; the frame tag is g_Counter.mCounter0, as every
 * latch user. At dt 1 / N 1 nothing is noted or asked (the callers' gates), so dt-1 check is unchanged. */
#pragma once
#include "true60.h"

namespace t60wc {
static inline u32 key(u32 actor) { return actor | 3u; }
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }
/* the whole-form guard: this actor runs in the whole form in this frame */
static inline u32 keyWhole(u32 actor) { return actor | 1u; }
static inline u32 pcId(u32 actor) { return *gabi::at<be<u32>>(actor + 0x04); /* base_process_class::mBsPcId */ }
static inline void note(u32 actor) {
    wwhd_t60_latch_set(key(actor), frameTag());
    wwhd_t60_latch_set(keyWhole(actor), pcId(actor));
}
/* an actor that runs in the whole form whenever it is converted, noted at its create (N > 1 only): its riders hold
 * from its first frame, also while it is culled before its first execute (nestP2: Link stepped on the culled
 * nest for the 3 frames between the warp and the nest's first execute, and those stepped frames left him 0.024 off A) */
static inline void noteWhole(u32 actor) {
    /* not gated by steps > 1: a create during a scene load runs at N 1 (the load fallback; nestP3: the
     * gated note never happened, Link stepped on frames 31-33 as in nestP2). Host map only (the port's / the harness's
     * own): no guest word, dt-1 check unchanged; the riders only ask at N > 1. */
    wwhd_t60_latch_set(keyWhole(actor), pcId(actor));
}
/* the rider: the carrier ran in the whole form in this frame (so far) */
static inline bool ranThisFrame(u32 actor) { return actor != 0 && wwhd_t60_latch_pending(key(actor), frameTag()) != 0; }
/* the rider: the carrier runs in the whole form (it has since its creation), whether or not it executed in this frame */
static inline bool isWhole(u32 actor) { return actor != 0 && wwhd_t60_latch_pending(keyWhole(actor), pcId(actor)) != 0; }
}  // namespace t60wc
