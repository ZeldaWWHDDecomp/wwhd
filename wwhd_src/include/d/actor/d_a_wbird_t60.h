/* d_a_wbird_t60.h (WWHD, true 60 build, 2026-11-22; whole-frame form): d_a_wbird.cpp on a general
 * step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's source keeps thin calls.
 *
 * The wind-direction demo bird (WBird, after the Wind's Requiem): its frame reads the wind-select screen / event state,
 * counts its timer and moves through fopAcM_posMove. Whole form (the whole-frame form rule + addendum, d/t60_objw.h): the execute
 * runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (t60w::Guard sets dt 1 for the run);
 * the earlier steps change no logic state and are picture steps (frame interpolation). Was the shared-world-state whole-execute form until pair
 * shared-world-state keeps the process's dt (1/N) on the tick30 step, and fopAcM_posMove (true60
 * f_op_actor_mng_2.cpp: speed * dt) moved the bird by half its speed once per frame (P -15 / X -30 per frame on x);
 * tools/true60/lint14s.py now refuses a dt-scaled call under a shared-world-state gate. dt 1: the original. Negative control:
 * T60C_NEGCTRL. */
#pragma once
#include "d/t60_objw.h"
