/* d_a_deku_item_t60.h (WWHD, true 60 build; 2026-11-18): the Deku Leaf on the ground
 * (d_a_deku_item.cpp, profile 0x104) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_deku_item.cpp keeps thin calls.
 *
 * The execute runs on every step (fopAcM_posMoveF and the ground correction are central). By hand (the tool's pass
 * saw no site: the modes are reached through the guest member table, the tool finding of 2026-11-18):
 *   - mode_proc_call dispatches the mode the frame began with (t60c::heldInt): a CO pick-up hit read on the first step
 *     changes the mode for the next frame, as in the original (one state change per frame; the get-demo order is not
 *     made a frame early);
 *   - mode_wait's leaf sparkle sound (mDoAud_seStart, started on every frame: a continuous sound) on the tick30 step
 *     only (the one-emitter-per-event rule / sound-gate): once per 30 Hz frame. The emitter follow, the cylinder centre and the CO registration
 *     stay per step (non-attack contact, step-resolution threshold (f)).
 * dt 1: the original. Negative control: T60C_NEGCTRL (t60conv.h). */
#pragma once
#include "d/t60conv.h"
