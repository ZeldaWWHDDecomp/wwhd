/* d_a_kytag03_t60.h (WWHD, true 60 build, 2026-11-20): d_a_kytag03.cpp on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's source keeps thin calls.
 *
 * the shared-world-state rule: the execute's frame logic reads the room's switch, the event system and the player's position and
 * writes the shared contrast state (dKy_contrast_flg / dKy_change_colpat): it runs on the tick30 step at the line place.
 * The door model's animation (mDoExt_McaMorf::play, a converted step primitive) and its base matrix stay on every step.
 * dt 1: the original. */
#pragma once
#include "d/t60conv.h"
