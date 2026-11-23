/* d_mesg_core_t60.h (WWHD, true 60 build): the message manager's per-frame update at N steps
 * (2026-11-18; dt = 1/N, the first-step frame-level rule). Decomp build only.
 *
 * dMesg_core_c::update (025F77B0) is the Wii U frame's end-of-frame message work: it advances the frame counter (+0x10,
 * mod 10000) and the three cyclic glyph timers (+0x994..+0x99C, periods 160 / 40 / 80 frames: the animated button
 * glyphs), clears play+0x5BD2 and clears play+0x5C20 (the message BGM flag dComIfGp_setMesgBgmOn) unless the current
 * message keeps it. It is not a process: the per-frame function calls it after the loop body (027199C0, the port's
 * per-frame child 11), and the port runs that child on EVERY pass, the hold passes included (interp.cpp hold_skip_child,
 * run mask 0x7F4C; it holds drawing state other children need). At N steps per 30 Hz frame it therefore ran N times a
 * frame: glyph animations N times too fast, and the message BGM flag a converted actor set on an earlier step cleared
 * before the 30 Hz world's tick30 step could read it.
 *
 * The rule: the update is the 30 Hz frame's end; it runs on the tick30 pass (the frame's last pass, after every
 * execute: Wii U order) and is skipped on the other passes. At N = 1 without interpolation every pass is a tick30 pass:
 * nothing changes. GABI_T60=0 / the verification harness's dt-1 check: tick30 is 1, the original path. */
#pragma once
#include "true60.h"

/* true: this pass is not the 30 Hz frame's end; the update waits for the tick30 pass */
static inline bool t60mc_not_frame_end() { return !t60::tick30(); }
