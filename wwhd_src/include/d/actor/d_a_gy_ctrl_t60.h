/* d_a_gy_ctrl_t60.h (WWHD, true 60 build, 2026-11-22): d_a_gy_ctrl.cpp (GyCtrl / GyCtrlB, 0xE5 / 0xE6) on a
 * general step length (dt = 1/N). Decomp build only.
 *
 * Whole form (the whole-frame form rule addendum, cross-actor reads; d/t60_objw.h): the controller's frame reads Link (position,
 * status: ship ride / swim) or the ship, and its Gyorgs (alive, search, create); setPathTargetPos centres the circle on
 * Link / the ship and steps the circling angle, the mode (modeCreate / modeWait) places the Gyorgs' spawn and check
 * positions from that angle and eases the radius. The tick30 step runs the original execute whole at dt 1 from the
 * frame-start state; the earlier steps change no state (no model: nothing to show).
 * Replaces the per-step form (t60conv + the circling share): its modeProc on the frame's first step placed
 * the spawn positions from the half-stepped angle and eased the radius at the step dt (pair on :
 * mSpawnPositions +0x3AC.. off A from frame 31, Link 52.6 off A at f267).
 *
 * Not in kT60Converted: the profiles are added after the paired runs. */
#pragma once
#include "d/t60_objw.h"
