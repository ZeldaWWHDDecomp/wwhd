#pragma once
/* true60 (decomp build only, 2026-11-18): Link's side climb (procClimbMoveSide) moves by the
 * change of his hands' spacing (on pair). Separate file:
 * d_a_player_main_06 keeps two thin calls.
 *
 * The original moves Link per update by |d - m35A0| x 0.8: d = the XZ distance of his animated hands as the update
 * finds them (the pose of the previous frame's end), m35A0 = the previous update's d. Run per step, the abs was taken
 * of each step's change (the steps' abs values sum to more than the frame's change where the spacing turns within a
 * frame: B ran ahead), and the later steps read mid-frame poses A never samples (B's frame sum included half a
 * frame of the next frame's hand motion: cl66a's telescoping form, |d_k - d0| - |d_(k-1) - d0|, still split by a
 * half frame). Form: the frame's first call takes A's sample and A's move D = |d - m35A0| (d = the pose of the previous
 * frame's end on that step, as in A) and keeps d as the frame's m35A0; every step of the frame moves D x dt (the frame
 * sums to A's D) and leaves m35A0 = the frame's d, so the next frame's first step measures A's change again. A new
 * spacing written by a procedure init on a middle step (m35A0 not the frame's d) starts a new measurement there.
 * Host static state (frame tag g_Counter.mCounter0). dt 1 (the original, dt-1 check): never consulted.
 * Negative control: LKT60_CLIMB_NEGCTRL (the per-step form). */
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include <cmath>
#include <cstring>

struct LkT60ClimbSpread {
    uint32_t tag;  /* the frame (g_Counter.mCounter0) */
    float move;    /* A's |d - m35A0| of the frame */
    float spacing; /* the frame's d (A's m35A0 after the frame) */
    bool valid;
};
static LkT60ClimbSpread s_lkT60ClimbSpread;

static inline bool lkT60Climb_on() {
#ifdef LKT60_CLIMB_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f && t60::steps() > 1;
#endif
}

static inline bool lkT60Climb_same(float a, float b) { return std::memcmp(&a, &b, sizeof(float)) == 0; }

/* the step's travel before the 0.8 factor: d = this step's spacing, prev = m35A0 */
static inline float lkT60Climb_travel(float d, float prev) {
    if (!lkT60Climb_on()) {
        return std::fabs(d - prev);
    }
    LkT60ClimbSpread& s = s_lkT60ClimbSpread;
    const uint32_t tag = lkT60_frameTag();
    if (!s.valid || s.tag != tag || !lkT60Climb_same(prev, s.spacing)) {
        s.tag = tag;
        s.valid = true;
        s.move = std::fabs(d - prev);
        s.spacing = d;
    }
    return s.move * (float)t60::dt();
}

/* the m35A0 the step leaves: d at dt 1, the frame's d at N steps (call after lkT60Climb_travel) */
static inline float lkT60Climb_spacing(float d) {
    return lkT60Climb_on() ? s_lkT60ClimbSpread.spacing : d;
}
