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
#include <cstdlib>

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

/* true60 (real logic error). The form above restarts every step from
 * the frame's m35C4, but the call's ease addCalc(m35C4, 0, 0.5, 25, 5) ran at the step's dt (ratio 1 - 0.5^dt, min step
 * 5 x dt), so the tick30 step took HALF a frame's ease from the frame start, not the original frame's one ease: an
 * offset below the min step (A: cleared to 0 in one frame) kept min step x (1 - dt) in B. Stair slope stZ1 rel 38: frame
 * start 3.049, A 0 - 0.7 x 0.0074 = -0.0052, B 3.049 - 2.5 - 0.0052 = 0.544; setWorldMatrix puts the model base 0.54 higher,
 * footBgCheck's leg IK (setLegAngle from h - base y) bends the legs differently, so the toe joints of the frame-end calc
 * (rel 39's first-step foot records, the delayed chord) differ: speedF 13.4772 / 13.4805 at rel 39, the stair step-up
 * missed at rel 42, the landing 2 frames late. (The base-foot switch on B's tick30 step at rel 38 is the per-step record
 * of the half-frame pose: mBaseFootIdx has no reader and the chord takes the first step's foot; a symptom, not the cause.)
 * Form (whole-frame form whole form, as footBgCheck's): under the frame-start form the ease runs at dt 1 on every step, so each step
 * eases once from the frame start and the tick30 step's offset is the original frame's (frame-end values = A's). A frame
 * whose first step did not come here keeps the step's own dt ease. dt 1 (N = 1): never. NC WWHD_T60_STAIRFOOT_NEGCTRL=1:
 * the step-dt ease. Returns the dt to put back (0: nothing changed). */
static inline bool lkT60_stepsOffsetEaseNegCtrl() {
    static const bool nc = [] {
        const char *e = getenv("WWHD_T60_STAIRFOOT_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

static inline float lkT60_stepsOffsetEaseBegin() {
    if (t60::dt() == 1.0f || t60::steps() <= 1 || lkT60_stepsOffsetEaseNegCtrl())
        return 0.0f;
    const LkT60StepsOffset &s = s_lkT60StepsOffset;
    if (!s.valid || t60_step_counter() - s.stamp >= t60::steps())
        return 0.0f;
    return t60_set_dt(1.0f);
}

static inline void lkT60_stepsOffsetEaseEnd(float prevDt) {
    if (prevDt != 0.0f)
        t60_set_dt(prevDt);
}
