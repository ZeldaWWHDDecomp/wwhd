/* d_meter_t60.h (WWHD, true 60 build; UI): the in-game UI systems the decompilation converted to the
 * general step length (dt = 1/N; UI rows).
 *
 * Two UI updates run from the draw path, not from the process executor (so the per-process rate gate does not reach
 * them): the HUD meter's per-frame update dMeter_Execute 02593B10 (from the play scene's draw) and the HD UI manager's
 * screen updates after fpcM_Management 02715310. The port holds both on the in-between passes of frame interpolation
 * (issues #262, #64 / #74); with true 60 the earlier steps of an N-step frame are such passes. A system named here runs
 * on every step of an N-step frame instead, with g_dt = 1/N set around it (port runtime/src); an unnamed
 * one keeps the tick30 step with dt 1. A unit that converts one sets its bit (one line, with the unit ID). */
#pragma once
#include "true60.h"

enum {
    kT60Ui_Hud = 1u << 0,  /* dMeter_Execute 02593B10 (meter_01#1..3 / B01) */
    kT60Ui_HdUi = 1u << 1, /* hdIngameUi_c::calcSecondaryScreens 02715310 */
    /* the HD layouts' picture between frames (nw/lyt/LYT_t60.h): the logic stays whole on the first step, the
     * port shows the recorded layout animators interpolated on the steps and re-applies them at the frame's end */
    kT60Ui_HdLayout = 1u << 2,
};
/* kT60Ui_Hud stays unset (meter_01#1..3 / B01's d_meter_02 / d_meter_head): the
 * HUD meter is once-a-frame game logic, not a picture: it applies the pending life / max-life changes to the save, counts
 * the item timers, sets the action / button statuses Link reads, starts the heart refill SE, draws the magic sparkle's
 * random numbers and writes the HD HUD's shared block (1047B03C: visibility, health, alpha ratios) the hd_screen_*
 * layouts draw from; its values are functions of 30 Hz frame counters. It runs from the play scene's draw, after the
 * executes, so on the tick30 step it reads the frame-end world A reads: the whole form on the tick30 step at dt 1 is the
 * port's unconverted path (held on the earlier steps). The HUD's 60 Hz picture is the HD layouts' (03). */
static const u32 kT60UiConverted = kT60Ui_HdLayout; /* */

/* (game build only, defined in d_meter_01.cpp: the port asks which UI systems step on every step, t60_ui.h:
 * wwhd_t60_ui_converted) */
