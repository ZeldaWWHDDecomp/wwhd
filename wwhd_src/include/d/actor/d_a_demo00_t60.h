/* d_a_demo00_t60.h (WWHD, true 60 build, 2026-11-22): d_a_demo00.cpp on a general step length
 * (dt = 1/N). Decomp build only. Separate file
 * the actor's source keeps thin calls.
 *
 * The cutscene actor (d_act0..31) takes its frame from the JStudio demo object (dDemo_actor_c: shape / animation /
 * parameter ids, event bits, items, fades, quakes; then its action's position, animation and model step). Whole form
 * (the whole-frame form rule + addendum, d/t60_objw.h): the execute runs once per 30 Hz frame on the tick30 step at dt 1, where the
 * demo object holds the frame-end values forward(1) left (the pass's last step evaluates at age itself), so
 * every discrete demo command (event bit, item, fade, quake) is taken once, as at 30 Hz; the earlier steps are picture
 * steps (frame interpolation: the skeleton drawn between its frame ends). dt 1: the original. */
#pragma once
#include "d/t60_objw.h"
