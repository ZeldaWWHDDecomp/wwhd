#pragma once
/* true60 (decomp build only; 2026-11-23): Link's sideways hang move
 * (procHangMove) moves A's frame move, in the side climb's frame-sample form (d_a_player_climb_t60.h,
 *  on). Separate file: d_a_player_main_06 keeps thin calls.
 *
 * The original sets mNormalSpeed = |d - m35A0| per update: d = the XZ distance of his animated hands as the update
 * finds them (the pose of the previous frame's end), m35A0 = the previous update's d. Run per step, each step took the
 * abs of its own half-frame change: where the spacing turns within a frame the steps' abs values sum to more than the
 * frame's change (a walking route's hangs: B 91.45 / 121.70 units against A's form 86.78 / 115.62, frame-end gap up to 8.6), and
 * the steps measure mid-frame poses A never samples. Form: the frame's first call takes A's sample and A's move
 * D = |d - m35A0| and keeps d as the frame's m35A0; every step of the frame keeps mNormalSpeed = D (A's frame value;
 * posMove moves D x dt) and the wall probe / snap take D x dt, and every step leaves m35A0 = the frame's d. A new
 * spacing written by procHangMove_init on a middle step (m35A0 not the frame's d) starts a new measurement there.
 * Host static state (frame tag g_Counter.mCounter0), one player. dt 1 (the original): never consulted.
 * Negative control WWHD_T60_HANGMOVE_NEGCTRL=1: the per-step form. */
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

struct LkT60HangSpread {
    uint32_t tag;  /* the frame (g_Counter.mCounter0) */
    float move;    /* A's |d - m35A0| of the frame */
    float spacing; /* the frame's d (A's m35A0 after the frame) */
    bool valid;
};
static LkT60HangSpread s_lkT60HangSpread;

static inline bool lkT60Hang_on() {
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_HANGMOVE_NEGCTRL");
        return e && e[0] == '1';
    }();
    return !nc && t60::dt() != 1.0f && t60::steps() > 1;
}

/* A's frame move |d - m35A0| (d = this step's spacing, prev = m35A0): per frame, the unit mNormalSpeed keeps */
static inline float lkT60Hang_frameMove(float d, float prev) {
    LkT60HangSpread& s = s_lkT60HangSpread;
    const uint32_t tag = lkT60_frameTag();
    if (!s.valid || s.tag != tag || std::memcmp(&prev, &s.spacing, sizeof(float)) != 0) {
        s.tag = tag;
        s.valid = true;
        s.move = std::fabs(d - prev);
        s.spacing = d;
    }
    return s.move;
}

/* the m35A0 the step leaves: the frame's d (call after lkT60Hang_frameMove) */
static inline float lkT60Hang_spacing() { return s_lkT60HangSpread.spacing; }
