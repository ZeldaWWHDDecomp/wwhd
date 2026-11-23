/* d_a_player_stepsoffset_t60.h (WWHD, true 60 build, 2026-11-20,
 * irol pair: the follow camera's y split at Link's MOVE entry while Link's pos was exact). Decomp build only.
 * Separate file: d_a_player_main_03.cpp keeps thin calls.
 *
 * setStepsOffset 023FF47C keeps Link's visual step offset m35C4 (setWorldMatrix draws him at pos.y + m35C4; setAttentionPos
 * puts attention_info.position.y at that base matrix y + 92.5, and the follow camera's centre chases the attention
 * position: relationalPos). Per call: addCalc(m35C4, 0, 0.5, 25, 5) (min step 5: a small offset is cleared), then on a
 * step down (no ground under the probe) + 0.7 (dy - |xz| tan) of the move old.pos -> pos, on a step up - 0.7 diff and the
 * snap. At 30 Hz one call a frame: old.pos is the frame start, the offset is the frame's. At N steps every step cleared
 * the offset and the tick30 step kept 0.7 x ITS step's move only (about 1/N of the frame's), so the attention y and the
 * camera centre y followed a different offset while pos was exact.
 * Form (the whole-frame form rule for the offset; the step-up snap of pos stays per step, the per-step ground-check rule): each step computes the
 * offset from the frame's first-step values: m35C4 as the frame started and old.pos as the frame started (the move so
 * far), so the tick30 step's offset is the original frame's. A frame whose first step did not come here: the step's own
 * values. dt 1 (N = 1): never (the original). Host memory, one player. */
#pragma once
#include "true60.h"

struct LkT60StepsOffset {
    uint32_t stamp; /* step counter of the frame's first step */
    f32 m35C4;      /* the offset as the frame started */
    f32 ox, oy, oz; /* old.pos as the frame started */
    bool valid;
};
static LkT60StepsOffset s_lkT60StepsOffset;

/* top of setStepsOffset (dt != 1): the frame's start values put into m / o (o = a local copy of old.pos) */
static inline void lkT60_stepsOffsetBegin(f32 &m, f32 &ox, f32 &oy, f32 &oz) {
    if (t60::dt() == 1.0f || t60::steps() <= 1)
        return;
    LkT60StepsOffset &s = s_lkT60StepsOffset;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s.stamp = now;
        s.m35C4 = m;
        s.ox = ox;
        s.oy = oy;
        s.oz = oz;
        s.valid = true;
        return;
    }
    if (!s.valid || now - s.stamp >= t60::steps())
        return;
    m = s.m35C4;
    ox = s.ox;
    oy = s.oy;
    oz = s.oz;
}
