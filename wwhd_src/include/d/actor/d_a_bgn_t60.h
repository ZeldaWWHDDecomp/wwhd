/* d_a_bgn_t60.h (WWHD, true 60 build, 2026-11-19): Puppet Ganon, phase 1 and the fight's
 * controller (d_a_bgn*.cpp, bgn_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's / Helmaroc's form): the frame reads Link (position,
 * the cut scene placement of the player), the tick30 collision hits (damage_check on the parts, the strings and the
 * core), the phase-2 / phase-3 bodies bgn2 / bgn3 (fpcM_Search, their angles and forms), the keese count (ki_check) and
 * the shared five-timer set mTimers[] that gates every action (bgn_action / bgn_move); the puppet's parts and strings
 * are a chain stepped per frame (bgn_exec_move). So daBgn_Execute runs whole once per 30 Hz frame on the tick30 step at
 * dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation. Every model of the fight (the chest, the parts, the water / room reflection models, and the
 * bgn2 / bgn3 bodies: their own draws are empty, daBgn_Draw draws them) is drawn by this process, so the port's draw
 * history blends it between the frame ends; a picture step's draw is kept and put back (draw keep). The draw's fog
 * flash fades once per frame already (t60_draw_frame_pass); its light states (settingTevStruct: water, room, the
 * actor's, bgn2's, bgn3's and its parts') are made on the tick30 step's draw only (rule 14).
 * daBgn_Create calls daBgn_Execute once (the first frame's pose): that call is the original at dt 1 (create's run, not
 * a frame step). dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bgn_class;
namespace bgn_t60 {
static inline const t60w::Ops<bgn_class>& ops() { return t60objw::Logic<bgn_class>::kOps; }
/* daBgn_Create's execute call: the original at dt 1 (t60w::on() is false at dt 1, so the Guard does nothing) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the draw's light states: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bgn_t60
