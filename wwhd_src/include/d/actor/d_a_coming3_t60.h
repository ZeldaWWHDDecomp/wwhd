/* d_a_coming3_t60.h (WWHD, true 60 build; 2026-11-17, whole form 2026-11-20): d_a_coming3.cpp on a
 * general step length (dt = 1/N). Decomp build only. Separate
 * file: the actor's source keeps thin calls.
 *
 * the whole-frame form rule / shared-world-state, whole form (d/t60_objw.h): the challenge spawner reads Link's distance, the water height and the
 * safety search, creates its barrel (a process created on an earlier step ran in its own frame one frame early) and
 * follows the barrel's position: daComing3::Act_c::_execute runs whole once per 30 Hz frame on the tick30 step at dt 1;
 * the earlier steps change no logic state; its model is drawn between the frame ends by frame interpolation. The former per-step
 * form's first-step spawn count is back to the original. dt 1: the original. Negative control: T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"
