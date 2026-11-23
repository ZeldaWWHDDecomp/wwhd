/* d_a_pw_t60.h (WWHD, true 60 build, 2026-11-19): the Poe (pw, d_a_pw.cpp, pw_class) on a
 * general step length (dt = 1/N). Decomp build only. Separate
 * file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: the idle / attack
 * states and the demos (the Jalhalla split) decide from Link, the tick30 collision hits, enemy_ice, the lantern
 * (kantera) and the event system, and the timers, the RNG draws, the fade / visibility, the posMove / BG check and the
 * morf advance once per 30 Hz frame. So daPW_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from
 * the frame-start state (d/t60_whole.h, t60objw form); the earlier steps change no logic state (one AI state step per
 * 30 Hz frame, the check item, holds by construction). The picture: frame interpolation. The execute calls draw_SUB
 * (matrix, the light state), so the light state is made on the tick30 step only; daPW_Draw writes no logic state.
 * daPW_Create's draw_SUB pose is the original at dt 1 (createBegin / createEnd). Never held. dt 1: the original. Negative control:
 * T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"
#include "d/t60_cameye.h"

struct pw_class;
namespace pw_t60 {
static inline const t60w::Ops<pw_class>& ops() { return t60objw::Logic<pw_class>::kOps; }
/* daPW_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the process-order read rule (fix, pair from frame 59): action_dousa mode 11 places the lantern 150 units
 * toward the camera eye (dCamera_c + 0xDC). The Poe is before the camera in the line: at 30 Hz it reads the eye of the
 * frame before; its whole run on the tick30 step would see the eye after the camera's first step of the frame. The skip
 * step (the frame's first step, before the camera) notes the eye (d/t60_cameye.h); mode 11 reads the note. N 1: live. */
/* (no guest call in the skip step: the whole gate counts calls; the camera is the one action_dousa's last run found, a
 * host note) */
static u32 s_camera;
static inline void seeCamera(u32 camera) { s_camera = camera; }
static inline void noteEye() { t60cam::noteFrameStartEye(s_camera); }
}  // namespace pw_t60
