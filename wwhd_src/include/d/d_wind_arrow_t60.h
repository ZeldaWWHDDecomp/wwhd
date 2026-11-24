/* d_wind_arrow_t60.h (WWHD, true 60 build; follow-up, 2026-11-23 from the stair sweep t60-stsw): the
 * wind arrow over the boat (dWindArrow_c, profile 0x16, d_wind_arrow.cpp) on a general step length. Decomp build only;
 *. Separate file: d_wind_arrow.cpp keeps thin calls.
 *
 * The arrow is an unconverted kankyo process: its draw places its model from its owner actor (mParam: the ship) on every
 * pass, from the owner's live current.pos / shape_angle.y. The ship is converted (it moves on every step and is drawn at
 * W from its step history), the arrow's model is an unconverted one (frame interpolation records it on the record pass
 * and blends between the records): the arrow's record held a mid-frame place on the earlier-step passes, and it moved
 * ~31 / ~10 units on sub-step / tick30 passes while sailing (sweep: excess 8.3 vs 0.04 units / pass in the reference).
 * Form (draw side, like frame interpolation / the interpolated world): the draw places the arrow from the owner's frame-end place:
 * the draw after the frame's tick30 step notes the owner's place (its frame end, A's place for the frame's draw; the step
 * is told by the 30 Hz frame counter and the step counter, the draw itself runs at dt 1); the draws after the frame's
 * earlier steps use the noted place (the previous frame's end), so the arrow's model changes
 * once per 30 Hz frame, at the frame ends, and frame interpolation blends it between them as any unconverted model (its
 * record pass is the earlier-step pass: it recorded the mid-frame place, and the tick30 pass blended from there to the
 * frame end: 31 / 10 per pass).
 * No logic: the draw writes nothing but the model's matrices (as before). WWHD_T60_WINDARROW=0: the live place (as
 * before; the negative control). dt 1 / N 1: every step is tick30: the original. Host memory. */
#pragma once
#include "true60.h"
#include <cstdlib>

struct WindArrow60 {
    unsigned owner;
    float x, y, z;
    short ay;
    bool valid;
    unsigned tag;   /* the 30 Hz frame counter at the last draw */
    unsigned first; /* the step counter at the draw where it changed (the draw after the frame's first step) */
};
static WindArrow60 s_windArrow60;
static inline bool windArrow60_on() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_WINDARROW");
        return !(e && e[0] == '0');
    }();
    return on;
}
/* the place the arrow's draw uses: the owner's live place on the tick30 draw (noted), the noted one on the other draws */
static inline void windArrow60_place(unsigned owner, float x, float y, float z, short ay, float* ox, float* oy, float* oz,
                                     short* oay) {
    WindArrow60& w = s_windArrow60;
    /* (the draw runs outside the step, at dt 1: the step is told by g_Counter.mCounter0 101FF558, the 30 Hz frame counter,
     * which advances on the frame's first step (measured: it changed on the draw after step 1 of 2): the draw where it
     * changed is the frame's first step's, step k of the frame is the step counter k - 1 later, the frame's tick30 step
     * the one at k = N) */
    const unsigned tag = (unsigned)*gabi::at<be<u32>>(0x101FF558u), now = t60_step_counter();
    if (tag != w.tag) w.tag = tag, w.first = now;
    const bool frameEnd = now - w.first + 1u >= t60::steps();
    if (t60::steps() == 1 || frameEnd || !windArrow60_on()) {
        w.owner = owner, w.x = x, w.y = y, w.z = z, w.ay = ay, w.valid = true;
    } else if (w.valid && w.owner == owner) {
        x = w.x, y = w.y, z = w.z, ay = w.ay;
    }
    *ox = x, *oy = y, *oz = z, *oay = ay;
}
