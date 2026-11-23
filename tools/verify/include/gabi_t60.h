/* gabi_t60.h: the true60 step state for converted sources (dt = 1/N rule 1, §1.6).
 *
 * A converted function reads the length of the logic step it runs in, never a hard-coded half step:
 *
 *   t60::dt      g_dt: step length in Wii U frames, 1/N with N = steps (1 = the original 30 Hz step)
 *   t60::tick30  g_tick30: 1 on the step that ends a 30 Hz frame (always 1 at N = 1)
 *   t60::steps   N: steps per 30 Hz frame (1, 2, 4 or 8)
 *   t60::first_step  the first step of a 30 Hz frame (always 1 at N = 1): the frame's one input read happens on it,
 *                  so every step of the frame sees the frame's input
 *   t60::step    the step's index within its 30 Hz frame, 1..N (N on the tick30 step; always 1 at N = 1): k / N of
 *                  the frame has elapsed at the end of step k (an emitter created on step k)
 *
 * The dt = 1 path must stay the original (rule 1.5.2): with dt = 1, tick30 = 1, N = 1 every converted function is
 * verified bit-exact against the recompiled original by tools/verify (dt-1 check), and the N-step behaviour against one
 * full step by `verify.py -t60steps N` (N-step spec, tools/verify/README.md "true60").
 *
 * Build config GABI_T60 (default 1):
 *   GABI_T60=1  the state is read from three globals that the program defines once:
 *               the verify harness (src/harness.cpp: pinned to dt 1 / tick30 1 / N 1 for every normal run, set per
 *               step by -t60steps) and the port's true60 decomp build (the runtime's step driver, sets them
 *               before each logic step / process execute). The values are loaded at run time, so the compiler
 *               cannot fold the dt = 1 case: the verified code is the code that runs at 60.
 *   GABI_T60=0  a build without the true60 runtime (the plain decomp build): constants dt 1, tick30 1, N 1, no
 *               symbols needed. `verify.py -t60fixed` builds a unit this way (own object directory).
 * The globals are host data, not guest memory: they are not part of a function's compared memory effect, and the
 * harness reports a candidate that changes them (`t60 step state written`).
 */
#pragma once
#include <stdint.h>

#ifndef GABI_T60
#define GABI_T60 1
#endif

#if GABI_T60
extern "C" {
/* default visibility: every game unit (compiled hidden, merged with ld -r) binds to the one definition */
__attribute__((visibility("default"))) extern float wwhd_t60_dt;        /* g_dt */
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_tick30; /* g_tick30 (0 or 1) */
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_steps;  /* N */
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_first;  /* the first step of a 30 Hz frame (0 or 1) */
__attribute__((visibility("default"))) extern uint32_t wwhd_t60_stepk;  /* the step within the 30 Hz frame, 1..N */
}
namespace t60 {
inline float dt() { return wwhd_t60_dt; }
inline bool tick30() { return wwhd_t60_tick30 != 0; }
inline uint32_t steps() { return wwhd_t60_steps; }
inline bool first_step() { return wwhd_t60_first != 0; }
inline uint32_t step() { return wwhd_t60_stepk; }
}  // namespace t60
#else
namespace t60 {
constexpr float dt() { return 1.0f; }
constexpr bool tick30() { return true; }
constexpr uint32_t steps() { return 1; }
constexpr bool first_step() { return true; }
constexpr uint32_t step() { return 1; }
}  // namespace t60
#endif
