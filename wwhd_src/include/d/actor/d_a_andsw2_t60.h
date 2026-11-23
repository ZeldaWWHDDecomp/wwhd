/* d_a_andsw2_t60.h (WWHD, true 60 build, 2026-11-20): d_a_andsw2.cpp on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's source keeps thin calls.
 *
 * the shared-world-state rule, whole-execute form: the switch tag's frame (the switch checks, its timer, the event order / end check,
 * its switch writes) runs only on the tick30 step at the line place at dt 1; no model (empty draw), no callback, no
 * picture step: behaviourally the unconverted 30 Hz actor (class pair). dt 1: the
 * original. */
#pragma once
#include "d/t60conv.h"
