/* t60_earlier_steps.h (WWHD, true 60 build, 2026-11-24): a 30 Hz frame's earlier
 * steps 1..N-1 at any N. Decomp build only.
 *
 * At N steps per 30 Hz frame the earlier steps are 1..N-1 and the tick30 step is N. A form that notes something on "the"
 * earlier step and catches up on the tick30 step is exact at N 2, where the earlier steps are only the first step, but at
 * N 4 it leaves out the steps between the noting step and the tick30 step (2..N-1, "held"). Pairs at N 4 on gbc1 (t60-trail
 * /): the knock-back stop on step 2 of 4 ended the frame at its start, the landing bounce moved two
 * steps at the bounce's speed, jyunkai's walk ramp ran on steps 1 and 4 only (2.5 for A's 5.0), the Moblin's turn at
 * fight_run's head ran on step 1 only (+32 for A's +113).
 * The helpers below name the step sets so a form covers every earlier step:
 *   earlier  a step 1..N-1 (dt < 1): a note taken here is read on the steps after it;
 *   later    a step 2..N (dt < 1): the steps that read a note of this frame;
 *   between  a step 2..N-1: a later step that is not the frame's end (empty at N 2);
 *   mark(key) / marked(key): a note keyed by a guest address on the port latch (shared by every unit), set on an earlier
 *   step and read on the steps after it in the same 30 Hz frame (dt 1 and the tick30 step: nothing is set).
 * dt 1 (and N 1): earlier / later / between / marked are false (the original). */
#pragma once
#include "bindings.h"
#include "true60.h"

namespace t60es {
/* the 30 Hz frame tag (g_Counter.mCounter0, data 101FF558: changes after the tick30 step's executes) */
inline u32 frame() { return *gabi::at<be<u32>>(0x101FF558); }
inline bool earlier() { return t60::dt() != 1.0f && !t60::tick30(); }
inline bool later() { return t60::dt() != 1.0f && !t60::first_step(); }
inline bool between() { return later() && !t60::tick30(); }
inline void mark(u32 key) {
    if (earlier())
        wwhd_t60_latch_set(key, frame());
}
inline bool marked(u32 key) { return later() && wwhd_t60_latch_pending(key, frame()) != 0; }
}  // namespace t60es
