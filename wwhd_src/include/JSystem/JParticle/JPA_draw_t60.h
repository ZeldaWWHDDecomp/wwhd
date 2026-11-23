/* JPA_draw_t60.h (WWHD, true 60 build): JPADraw's calc side on a general step length
 * (dt = 1/N).
 *
 * JPADraw::calcParticle 0282DCBC / calcChild 0282DD50 turn a particle's u16 angle (draw params +0x34) by its s16 speed
 * (0x36) per frame. On N steps the integer turn is split without stored state (the per-step smoothing rule, integer variant: the frame ends
 * exactly on the original step): every earlier step turns trunc(speed x dt), the tick30 step the rest,
 * speed - (N - 1) trunc(speed x dt). The frame-end angle is A's bit for bit.
 * The calc visitors (JPA_calcvis_a / _b: scale, colour, alpha, alpha flicker, texture-index animation, child fade) read
 * the particle / emitter age through ftoi (whole frames) and keep no state of their own: with the step ages (JPA-06 / 07)
 * they change at whole frames, i.e. at 30 Hz (accepted 30 Hz, the row's option), and give the 30 Hz frame's values at
 * every frame end. dt = 1: the original. */
#pragma once
#include "true60.h"

namespace jpa_t60d {
/* this step's share of a per-frame integer turn */
inline s32 turn_step(s32 speed) {
    if (t60::dt() == 1.0f)
        return speed;
    s32 part = (s32)((f32)speed * t60::dt());  // truncation toward zero
    return t60::tick30() ? speed - (s32)(t60::steps() - 1) * part : part;
}
}  // namespace jpa_t60d
