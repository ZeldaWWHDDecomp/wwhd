/* d_a_obj_barrel_t60.h (WWHD, true 60 build; 2026-11-18): the carryable barrel
 * (d_a_obj_barrel.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_obj_barrel.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables (as stone2 / tsubo): the barrel's frame decides from Link's carry /
 * hookshot-carry status (actor_status 0x102000), the tick30 collision Move's hits and pushes (damage_cc_proc,
 * m_cc_move), the ground hits (damage_bg_proc), moves through the shared movers (daObj::posMoveF_stream /
 * posMoveF_grade; fopAcM_posMoveF), and changes mode within the frame (vib / jump / walk / carry). So the
 * whole _execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h): every
 * decision with the other actors' current state, the movers' original move, the cylinder registered once for the
 * tick30 Move (one-sweep-per-frame attack), the mode timers, effects and sounds once. The earlier steps change no logic state and show the
 * barrel moving on by the previous frame's change of its position and its roll / tilt angles times the frame's dt so
 * far (set_mtx), none while Link carries it. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every
 * step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* the held picture */
#include "d/t60_carry_step.h" /* the step form */
#include "d/t60conv.h"

namespace barrel_t60 {
static inline void get(daObjBarrel::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->mRollAngle, v[4] = (f32)(s16)a->mTiltDir, v[5] = (f32)(s16)a->mTiltAngle;
}
static inline void set(daObjBarrel::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mRollAngle = t60w::angle(v[3]), a->mTiltDir = t60w::angle(v[4]), a->mTiltAngle = t60w::angle(v[5]);
}
static inline void mtx(daObjBarrel::Act_c* a) { a->set_mtx(); }
/* held: Link carries the barrel (carry / hookshot carry status, or the frame ended in mode_carry) */
static inline bool held(daObjBarrel::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) != 0 ||
           static_cast<s32>(a->mMode) == daObjBarrel::Act_c::MODE_CARRY;
}
static inline bool carried(daObjBarrel::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) != 0;
}
static const t60w::Ops<daObjBarrel::Act_c> kOps = {6, 3, get, set, mtx, held};
/* the execute's earlier-step tail; held, the model from this step's placement (set_mtx writes the
 * model's base matrix and the matrix stack scratch only). The worn barrel's tilt: Link's setGrabItemPos sets shape_angle.z
 * (his mMoveSlantAngle) on every step and the frame's mode_carry chases it toward mSlantAngle (0x1000) before set_mtx, so
 * A draws the chased tilt; the picture draws the same, from a copy (the dt 1 cLib_chaseAngleS on a local value: no t60
 * chase latch), and shape_angle.z is put back (carry route: without it the drawn root alternated by ~4.3 between
 * the earlier and the tick30 passes). d/t60_held_pic.h */
static inline void heldMtx(daObjBarrel::Act_c* a) {
    const s16 z0 = a->shape_angle.z;
    s32 x = z0, step = 0x1000;
    const s32 target = (s16)a->mSlantAngle;
    if ((s16)((u32)x - (u32)target) > 0)
        step = (s16)(0u - (u32)step);
    x = (s16)((u32)x + (u32)step);
    if ((s32)((u32)(s32)(s16)((u32)x - (u32)target) * (u32)step) >= 0)
        x = target;
    if (static_cast<s32>(a->mMode) == daObjBarrel::Act_c::MODE_CARRY)
        a->shape_angle.z = (s16)x;
    mtx(a);
    a->shape_angle.z = z0;
}
static inline bool held_picture(daObjBarrel::Act_c* a) {
    t60hp::picture(a, kOps, heldMtx);
    return true;
}
/* The step form (d/t60_carry_step.h; "convert the whole-form carriables to per-step logic",
 * rule: exact-frame-move family, decisions on the frame grid). The barrel's earlier steps run its mode's motion
 * as a picture of the current frame: the predicted moves (Barrel_posMoveF, daObj posMoveF_stream / _grade from the frame
 * start), the shape / tilt chases (central cLib_chaseAngleS), set_walk_rot's turn and roll (from the frame's predicted
 * move: its frame-grid thresholds magnitude > 5 / 2.5 on the frame's move, the integer roll / tilt adds as the step's
 * share through t60c::frameAdd), vib_pos_ang's spin, the picture ground check and set_mtx (held: Link's place and
 * bounce picture). No decision there: the mode inits, the carry entry / release, the timers mTimer / mInitTimer, the
 * hit / water / landing-break checks, the attention and weight, the room, the sounds and effects, the collider
 * registration (one-sweep-per-frame attack), mMove's decay. The tick30 step puts the frame-start words back (the base words and the barrel's
 * tilt / dir / timer / roll words) and runs the original _execute at dt 1: the frame end is A's bit for bit. NC
 * WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form above. */
static const t60cs::Spec<daObjBarrel::Act_c> kSpec = {{{0x72C, 8}, {0x74C, 4}}, held, 0x750, 0x3B4, carried};
}  // namespace barrel_t60
