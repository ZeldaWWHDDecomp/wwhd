/* d_a_tsubo_mode_t60.h (WWHD, true 60 build; 2026-11-18): the small carryable objects (pots,
 * skulls, barrels, stools, ...: d_a_tsubo, profile 0x1C9 family; execute in d_a_tsubo_mode.cpp) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * d_a_tsubo_mode.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables: the pot's frame decides from Link's carry / hookshot-carry status
 * (actor_status 0x102000) and grab flags (se_pickup_carry), the tick30 collision Move's hits and pushes (damage_cc_proc,
 * m_cc_move), moves through the shared movers (daObj::posMoveF_stream / posMoveF_grade; fopAcM_posMoveF),
 * swings on springs (moment_proc_call, set_tensor_*: the pose tensor), decays the wind push and changes mode within the
 * frame (carry -> drop / put). So the whole _execute runs once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h): every decision with the other actors' current state, the movers' original move,
 * the springs once, the cylinder registered once for the tick30 Move (one-sweep-per-frame attack), effects and sounds once. The earlier steps
 * change no logic state and show the pot moving on by the previous frame's change of its position and model height
 * offset (mOffsetY) times the frame's dt so far (set_mtx; the pose matrix stays on the 30 Hz grid, visual only: 30 Hz visual-motion),
 * none while Link carries it. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* t60hp::with_grab_dy */
#include "d/t60_carry_step.h" /* the step form */
#include <cstdlib>
#include <cstring>

namespace tsubo_t60 {
static inline void get(daTsuboMode::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z, v[3] = a->mOffsetY;
}
static inline void set(daTsuboMode::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2], a->mOffsetY = v[3];
}
static inline void mtx(daTsuboMode::Act_c* a) { tsuboMode_set_mtx(a); }
/* held: Link carries the pot (carry / hookshot carry status, or the frame ended in mode_carry) */
static inline bool held(daTsuboMode::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & 0x102000) != 0 || static_cast<s32>(a->mMode) == 4;
}
static inline bool carried(daTsuboMode::Act_c* a) { return (static_cast<u32>(a->actor_status) & 0x102000) != 0; }
static const t60w::Ops<daTsuboMode::Act_c> kOps = {4, 4, get, set, mtx, held};
/* the held pot's picture (2026-11-23; seen in play: a little stutter on the vase itself while Link was
 * carrying it around): while Link carries the pot, his setGrabItemPos places it (current.pos, shape_angle) on every
 * step, but the pot's model matrix (set_mtx, the pose from set_tensor_carry) ran only in the tick30 step's whole run: on
 * a frame's earlier steps the pot was drawn at the previous frame end's place while Link's hands moved on (pot carry
 * route, N 2: pot - left hand drawn offset (19.996, 0, 0) on every tick30 pass, up to 12.7 off on the passes
 * between, the pot standing still there). The picture: on an earlier step of a held pot, its pose and model matrix from
 * the place its holder gave it on this step, i.e. what set_mtx would show; the logic words those calls write (mPoseMtx,
 * mWaterDropPos) are put back, so the tick30 run reads what it read before (only the model's base matrix, a picture,
 * changes; the tick30 run sets it again). dt 1: never (no earlier steps). Negative control WWHD_T60_POTHELD_NEGCTRL=1:
 * the form before it (held: no picture). */
static inline bool held_pic_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_POTHELD_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
/* the execute's earlier-step tail (t60w::begin returned true); returns true (the execute's result) */
static inline bool picture(daTsuboMode::Act_c* a) {
    if (t60::dt() == 1.0f || t60::tick30() || !held(a) || held_pic_nc())
        return true;
    /* (word copies through be<u32> views: guest pages are mapped on access in the verify harness, no raw memcpy) */
    const u32 poseEa = gabi::ea(&a->mPoseMtx), dropEa = gabi::ea(&a->mWaterDropPos);
    constexpr u32 kPose = sizeof(a->mPoseMtx) / 4, kDrop = sizeof(a->mWaterDropPos) / 4;
    u32 pose[kPose], drop[kDrop];
    for (u32 i = 0; i < kPose; i++)
        pose[i] = *gabi::at<be<u32>>(poseEa + 4u * i);
    for (u32 i = 0; i < kDrop; i++)
        drop[i] = *gabi::at<be<u32>>(dropEa + 4u * i);
    t60hp::with_grab_dy(gabi::ea(a), [a] { /* Link's bounce picture (d/t60_held_pic.h) */
        if (static_cast<s32>(a->mMode) == 4)
            a->set_tensor_carry(); /* (the carry pose: the axis rotation and Link's shape_angle of this step) */
        tsuboMode_set_mtx(a);
    });
    for (u32 i = 0; i < kPose; i++)
        *gabi::at<be<u32>>(poseEa + 4u * i) = pose[i];
    for (u32 i = 0; i < kDrop; i++)
        *gabi::at<be<u32>>(dropEa + 4u * i) = drop[i];
    return true;
}
/* The step form (d/t60_carry_step.h; "convert the whole-form carriables to per-step logic",
 * rule: exact-frame-move family, frame ends bit-exact, decisions on the frame grid). The pot's earlier steps
 * run the frame's motion as a picture: the mode's predicted move (fopAcM_posMoveF / daObj posMoveF_stream / _grade from
 * the frame start, water tension first as in A), the picture ground check (crr_pos with its water / lava levels on a
 * copy), the offset ease (moment_proc_call, central cLib_addCalc0), the pose tensor on the straight line to its frame
 * end (set_tensor_* once at dt 1 on a copy: the 30 Hz visual-motion rule / whole-frame form, visual), set_mtx (carried: the axis from this step's
 * angles and Link's place / bounce picture). No decision there: carry / throw / put (mode_carry runs on the frame grid
 * only), the hit / water / lava / landing-break checks, the bound, the appear timer and sounds, the afloat bob's random
 * draws, the room, attention, the wind decays, the Bokoblin sticks, the collider and its mass (one-sweep-per-frame attack), eye, model. The
 * tick30 step puts the frame-start words back (base words, the pose / tensor words 0x7A4..0x81C, mWaterDropPos) and runs
 * the original _execute at dt 1. NC WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form above. */
static const t60cs::Spec<daTsuboMode::Act_c> kSpec = {{{0x7A4, 0x78}, {0x81C, 12}}, held, 0x93C, 0x3B4, carried};
/* a PIC step's pose: carried, set_tensor_carry from this step's angles; else the glide to the frame end */
template <class P> static inline void tensorPic(daTsuboMode::Act_c* a, const P& proc) {
    if (static_cast<s32>(a->mMode) == 4)
        (a->*proc)();
    else
        t60cs::glide(a, 0x7D8, 12, [a, &proc] { (a->*proc)(); });
}
/* a PIC step's model matrix (carried: at Link's picture place, d/t60_held_pic.h) */
static inline void picMtx(daTsuboMode::Act_c* a) {
    t60cs::mtxF(a, held(a), [a] { tsuboMode_set_mtx(a); });
}
}  // namespace tsubo_t60
