/* d_a_obj_Yboil_t60.h (WWHD, true 60 build; 2026-11-17): d_a_obj_Yboil.cpp on a general step length
 * (dt = 1/N). Decomp build only.
 *
 * Hand conversion (the tool's form, `_execute` whole on the first step, failed N-step spec: the 50 bubbles' animations step with
 * dt and must play on every step). Per bubble, _execute keeps the branch its timer chose at the frame's start
 * (t60c::heldInt): a waiting bubble counts its timer on the first step (first-step AI-timer) and plays nothing this frame; a playing
 * bubble plays its bck / btk / brk on every step (central dt) until it ends, is reset there (frames 0, rate 1, a new
 * random wait: one draw per event, the per-step chance rule) and does nothing on the frame's later steps. dt = 1: the original.
 *
 * Not in kT60Converted: the profile is added after the paired runs. */
#pragma once
#include "d/t60conv.h"
