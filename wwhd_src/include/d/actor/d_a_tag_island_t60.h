/* d_a_tag_island_t60.h (WWHD, true 60 build, 2026-11-22; whole-frame form): d_a_tag_island.cpp on a
 * general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's source keeps thin calls.
 *
 * The island arrival / Wind Waker lesson tag: its frame reads the event / switch / message state and, in its event,
 * moves the raft demo (daObj_Ikada_c::setMtx -> cLib_addCalcAngleS2, which smooths by dt). Whole form (the whole-frame form rule +
 * addendum, d/t60_objw.h): the execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state
 * (t60w::Guard sets dt 1 for the run); the earlier steps change no logic state and are picture steps (frame interpolation). Was
 * the shared-world-state whole-execute form: shared-world-state keeps the process's dt (1/N) on the tick30 step, and tools/true60/lint14s.py reports
 * the raft demo's dt-scaled smoothing under it (as wbird's fopAcM_posMove, pair). dt 1: the original. Negative
 * control: T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"
