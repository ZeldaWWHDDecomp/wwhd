/* d_a_obj_tide_t60.h (WWHD, true 60 build; 2026-11-17, whole form 2026-11-20): d_a_obj_tide.cpp
 * on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (d/t60_objw.h): the tide is a moving background (dBgS_MoveBgActor) whose water height
 * other actors read through the background (Link's mWaterY -> changeSwimProc's depth test) and whose rise chases an
 * aim (move_to_aim: speed + accel, cLib_addCalc, a snap under 0.1; se_set_gmtw tests the aim within 0.01). The
 * per-step form (2026-11-17) moved the water on every step: Link, earlier in the line, read half a frame of rise on
 * the tick30 step (Helmaroc : SWIM_UP one frame early, then the 0x183E swim-voice split), and the per-step
 * chase landed on the aim where A's frame step stays off it (0x702D WATER_PACHA in B only, enemies). So
 * daObjTide::Act_c::Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state;
 * the earlier steps change no logic state and hand the wrapper the actor's own mBgMtx (the background stays at the
 * previous frame's end); the water and bubble models are drawn between the frame ends by frame interpolation.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"
