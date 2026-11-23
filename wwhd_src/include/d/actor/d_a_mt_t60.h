/* d_a_mt_t60.h (WWHD, true 60 build, 2026-11-19): Magtail (d_a_mt*.cpp, mt_class) on a general
 * step length (dt = 1/N). Decomp build only. Separate file:
 *the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form: the frame reads Link (procNo / grab / position: the lunge, the
 * carry chase), the attention lock, the tick30 collision hits and the lava ground, and its segment chain is a
 * follow-the-leader per frame (body_control2..5: each segment chases the one before along the stored trail, the wave
 * phase mJointWavePhase / mFrameCounter advances once per frame). So daMt_Execute runs whole once per 30 Hz frame on the
 * tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form); the earlier steps change no logic state.
 * The picture: frame interpolation. The eight segment models (mpMorf[0..7]) get their base matrices in the execute
 * (body_control*) and are calculated and drawn in daMt_Draw; the rubble models (mpBrModelA/B) get theirs in the draw
 * (br_draw). Every one of them is a model drawn by this process, so the port's draw history keeps it per model (keyed
 * by its joint array) and blends it between the frame ends; a picture step's draw is kept and put back (draw keep), so
 * the draw's own writes (tevStr, the ice scale, the btk / brk / btp frames, the node callbacks' anim matrices) leave
 * nothing behind. settingTevStruct (the light state) is made on the tick30 step's draw only (rule 14).
 * daMt_Create calls daMt_Execute once (the first frame's pose): that call is the original at dt 1 (create's run, not a
 * frame step). Never held: carried, Link's grab code (setGrabItemPos, converted) writes its position on every step
 * and the Magtail chases Link in its own execute, but its segment base matrices are set only in the execute; a held
 * actor's earlier-step draws would be stamped as steps of their own with the frame-start pose (a stall, then the jump),
 * so the carried Magtail too is drawn blended between its frame ends (the picture-step draw keep restores only what
 * the draw itself changed, not Link's write).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct mt_class;
namespace mt_t60 {
static inline const t60w::Ops<mt_class>& ops() { return t60objw::Logic<mt_class>::kOps; }
/* daMt_Create's execute call: the original at dt 1 (t60w::on() is false at dt 1, so the Guard does nothing) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace mt_t60
