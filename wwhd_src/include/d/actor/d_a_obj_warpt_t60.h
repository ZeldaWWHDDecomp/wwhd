/* d_a_obj_warpt_t60.h (WWHD, true 60 build, 2026-11-22): d_a_obj_warpt.cpp on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 *the actor's source keeps thin calls.
 *
 * The warp pot (Warpt / Warpts1-3): its frame decides from switches / event state / Link and plays its animations and effects. Whole
 * form (the whole-frame form rule + addendum, d/t60_objw.h): the execute runs once per 30 Hz frame on the tick30 step at dt 1 from
 * the frame-start state; the earlier steps change no logic state and are picture steps (frame interpolation: the model drawn
 * between its frame ends). dt 1: the original. Negative control: T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"
