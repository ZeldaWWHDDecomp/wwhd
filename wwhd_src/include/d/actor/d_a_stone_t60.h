/* d_a_stone_t60.h (WWHD, true 60 build; 2026-11-18): the carryable stone (d_a_stone.cpp,
 * profile 0x1C6) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_stone.cpp keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the barrel2 form through d/t60_whole.h: the stone's
 * frame decides from other actors' state (Link's carry status bit 0x2000 and getGrabActorID / grab status in mode_carry,
 * Link's speedF for the throw speed in mode_drop_init, the AT / TG hits of the tick30 collision Move in damage_cc_proc)
 * and moves through the shared mover (daObj::posMoveF_stream; fopAcM_posMoveF on moving ground) and the
 * ground correction (dBgS_Acch::CrrPos), and a mode change runs the new mode in the same frame (carry -> drop). So the
 * whole execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state: the mover's original
 * move, every decision with the other actors' current state, the thrown stone's attack cylinder registered once for the
 * tick30 Move (the one-sweep-per-frame attack rule), the effects and sounds once. The earlier steps change no logic state and show the stone moving
 * on by the previous frame's change (position, tilt mTilt, shape_angle) times the frame's dt so far (set_mtx), not while
 * Link carries it (Link places it and its model on every step). The tool's whole-function first-step gate (first-step AI-timer / first-step frame-level)
 * was wrong here: it ran the mover once a frame at dt 1/N (part of the move; the mover's tick30 frame end never reached).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* the held picture */
#include "d/t60_carry_step.h" /* the step form */

namespace stone_t60 {
static inline void get(daStone::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->mTilt;
    v[4] = (f32)(s16)a->shape_angle.x, v[5] = (f32)(s16)a->shape_angle.y, v[6] = (f32)(s16)a->shape_angle.z;
}
static inline void set(daStone::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mTilt = t60w::angle(v[3]);
    a->shape_angle.x = t60w::angle(v[4]), a->shape_angle.y = t60w::angle(v[5]), a->shape_angle.z = t60w::angle(v[6]);
}
static inline void mtx(daStone::Act_c* a) { a->set_mtx(); }
/* held: Link carries the stone (fopAcM carry status, or the frame ended in mode_carry) */
static inline bool held(daStone::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & 0x2000) != 0 || static_cast<s32>(a->mMode) == daStone::MODE_CARRY;
}
static inline bool carried(daStone::Act_c* a) { return (static_cast<u32>(a->actor_status) & 0x2000) != 0; }
static const t60w::Ops<daStone::Act_c> kOps = {7, 3, get, set, mtx, held};
/* the small stones' held picture (seen in play: the same issue as the pot's): the execute's earlier-step tail; held, the
 * model from this step's placement (set_mtx writes the model's base matrix and the matrix / quaternion stack scratch
 * only). Returns the execute's result. d/t60_held_pic.h */
static inline s32 held_picture(daStone::Act_c* a) {
    t60hp::picture(a, kOps, mtx);
    return 1;
}
/* The step form (d/t60_carry_step.h; "convert the whole-form carriables to per-step logic",
 * rule: exact-frame-move family, frame ends bit-exact, decisions on the frame grid). The stone's earlier steps
 * run the frame's motion as a picture: the mode's predicted move (fopAcM_posMoveF / daObj posMoveF_stream from the frame
 * start; in water the tension first, as in A), the picture ground check (CrrPos on a copy), the thrown / sinking tilt
 * mTilt on the line to its frame end (set_drop_rot / set_sink_rot once at dt 1 on a copy, the shared rotation axis
 * stone_axis put back: visual, the 30 Hz visual-motion rule / whole-frame form), set_mtx (carried: Link's place and bounce picture). No decision
 * there: carry / throw / put (mode_carry on the frame grid only), the hit / water / lava / landing-break checks, the
 * moving ground, the room, attention, the mass delay, the collider and its mass (one-sweep-per-frame attack), eye, model. The tick30 step puts
 * the frame-start words back (base words, the rotation words 0x798..0x7A4) and runs the original _execute at dt 1.
 * NC WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form above. */
WWHD_OFFSET(daStone::Act_c, mRoll, 0x798);
WWHD_OFFSET(daStone::Act_c, mTilt, 0x79E);
static const t60cs::Spec<daStone::Act_c> kSpec = {{{0x798, 12}}, held, 0x7AC, 0x3B4, carried};
static inline void rotCall(daStone::Act_c* a, void (*fn)(daStone::Act_c*)) { fn(a); }
static inline void rotCall(daStone::Act_c* a, void (daStone::Act_c::*fn)()) { (a->*fn)(); }
/* the drop / sink rotation: PIC, mTilt on the line to its frame end (the prediction puts the shared axis back) */
template <class F> static inline void rot(daStone::Act_c* a, F fn) {
    if (!t60cs::pic()) {
        rotCall(a, fn);
        return;
    }
    t60cs::glideT<s16>(a, 0x79E, 1, [a, fn] {
        u32 ax[3];
        t60cs::gget(ax, 0x1046E044, 12); /* stone_axis() */
        rotCall(a, fn);
        t60cs::gput(0x1046E044, ax, 12);
    });
}
/* a PIC step's model matrix (carried: at Link's picture place, d/t60_held_pic.h) */
static inline void picMtx(daStone::Act_c* a) {
    t60cs::mtx(a, held(a), &daStone::Act_c::set_mtx);
}
}  // namespace stone_t60
