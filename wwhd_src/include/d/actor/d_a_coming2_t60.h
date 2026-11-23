/* d_a_coming2_t60.h (WWHD, true 60 build; coming2, 2026-11-18): the sea barrel course
 * spawner (d_a_coming2.cpp: follows the ship, counts the sailing time, makes the barrels / flags and gives their rupees)
 * on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_coming2.cpp keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads): the frame decides from the ship's position, speed, tornado / whirlpool
 * links and the event state, from Link's ship-ride status and the barrels' item flags (other actors), and its process
 * chain (coming_process_main -> the guest-table modes, barrel_main -> barrel_execute x 15) changes state within the
 * frame (the per-step tool forms failed N-step spec there). So the whole _execute runs once per 30 Hz frame on the tick30
 * step at dt 1 (d/t60_whole.h): every decision with the other actors' current state, the wait timer, the barrel exit
 * timers and the spawns once per frame, as the original. The actor draws nothing (Mthd_Draw returns 1): no picture on
 * the earlier steps. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step. */
#pragma once
#include "d/t60_whole.h"

namespace coming2_t60 {
static inline void get(daComing2::Act_c*, f32*) {}
static inline void set(daComing2::Act_c*, const f32*) {}
static inline void mtx(daComing2::Act_c*) {}
static inline bool held(daComing2::Act_c*) { return false; }
static const t60w::Ops<daComing2::Act_c> kOps = {0, 0, get, set, mtx, held};
}  // namespace coming2_t60
