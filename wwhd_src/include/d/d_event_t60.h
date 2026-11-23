/* d_event_t60.h (WWHD, true 60 build): events and messages at N steps (2026-11-16; the conversion rules
 * 1.5 the event-step rule / 10).
 *
 * The event system (dEvent_manager_c::runProc: exceptionProc, dEvt_control_c::check, mainProc) runs inside the play
 * scene's execute, once per 30 Hz frame; its "Timer" / WAIT data are 30 Hz counts and need no dt. Until is done the
 * whole world falls back to N = 1 while an event or a message runs (d_s_play.cpp). This header holds the
 * switch that lifts that fallback for events and messages (menus, pauses and stage changes keep it):
 *   t60_events60  the port's wwhd_t60_events60 (WWHD_T60_EVENTS60=1; default 0 = the fallback unchanged).
 * With nothing converted, a frame at N during an event runs its unconverted world on the tick30 step exactly as at 30 Hz,
 * so a pair with the switch on and no conversion must stay strict-equal through the event (the runtime-side proof). */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) extern int wwhd_t60_events60;
}
static inline bool t60_events60() { return wwhd_t60_events60 != 0; }
#else
static inline bool t60_events60() { return false; }
#endif
