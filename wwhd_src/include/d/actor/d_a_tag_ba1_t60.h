/* d_a_tag_ba1_t60.h (WWHD, true 60 build, 2026-11-22): d_a_tag_ba1.cpp on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 *the actor's source keeps thin calls.
 *
 * the shared-world-state rule, whole-execute form: the Ba1 event tag's frame (its event / switch / message checks, timers and
 * state changes) runs only on the tick30 step at the line place; no model (empty draw), no callback, no picture
 * step: behaviourally the unconverted 30 Hz actor (class of andsw2 / alldie / coming2).
 * dt 1: the original. */
#pragma once
#include "d/t60conv.h"
