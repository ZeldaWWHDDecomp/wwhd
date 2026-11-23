/* d_a_wind_tag_t60.h (WWHD, true 60 build; 2026-11-17): d_a_wind_tag.cpp on a general step length
 * (dt = 1/N). Decomp build only.
 *
 * Hand conversion (the tool found nothing to change; its N-step spec failed):
 *   path_move     cLib_chasePos on every step (central dt); the step that reaches the point picks the next one
 *                 (set_next_pnt, per event: no N-step spec of its own) and the frame's later steps hold (t60c::mark)
 *   _execute      the switch / schedule edge test (mSwitchLatch) on the first step against the previous frame's values,
 *                 which the tick30 step stores (mPrevSchActive, mPrevSwitch); the wind strength / height smoothing and the
 *                 animations step on every step (central)
 *
 * Not in kT60Converted: the profile is added after the paired runs. */
#pragma once
#include "d/t60conv.h"
