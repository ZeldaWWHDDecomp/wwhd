/* d_a_gnd_t60.h (WWHD, true 60 build, 2026-11-22): d_a_gnd*.cpp on a general step length
 * (dt = 1/N; / gnd_exec / gnd_move). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * Ganondorf (gnd, 0xF6): the final fight runs inside the JStudio demo (daGnd_Execute reads the demo object, Link's
 * state, the hits of the tick30 collision Move; parry windows and attack timers decide on frame counts). Whole form
 * (the whole-frame form rule + addendum, d/t60_objw.h), the bosses' form (bdk, bgn, bmd, btd, bst): the execute runs once per 30 Hz
 * frame on the tick30 step at dt 1 from the frame-start state, every frame-count window exactly A's; the earlier steps
 * change no logic state and are picture steps (frame interpolation). The draw's flash fades stay on the frame pass (t60_draw.h).
 *dt 1: the original. Negative control: T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"
