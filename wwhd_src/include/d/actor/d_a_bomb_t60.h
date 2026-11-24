/* d_a_bomb_t60.h (WWHD, true 60 build; 2026-11-18): Link's bomb, the bomb flower nut and
 * the cannon bomb (d_a_bomb.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_bomb.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables (as stone2 / tsubo / obj_barrel / bomb2): the bomb's frame decides
 * from Link's carry status, the tick30 collision Move's hits (checkExplodeCc), the ground / wall / water / lava hits
 * (bgCrrPos, checkExplodeBg, bound) and the wind; it moves through the shared movers (fopAcM_posMoveF,
 * daObj::posMoveF_grade), counts its fuse (checkExplodeTimer: mRestTime, the fuse length bomb tricks depend on), its
 * no-gravity time, draw delay, explosion light / collider timers within the frame, and registers its sphere once per
 * frame (mMassCounter against the frame counter: one-sweep-per-frame attack). So the whole execute runs once per 30 Hz frame on the tick30
 * step at dt 1 from the frame-start state (d/t60_whole.h): fuse and explosion intervals, decisions, movers, the
 * explosion light fade (cLib_addCalc) and its env colour / point light / point wind sets, effects and sounds (fuse
 * sound 0x6100 every frame) once per frame, as the original. The earlier steps change no logic state and show the
 * bomb moving on by the previous frame's change of its position times the frame's dt so far (set_mtx), with the fuse
 * point (mFusePos: the sparks / smoke emitters follow it) moved by the same rule; the tick30 step puts both back first,
 * so the fuse trail history (mFusePos2 / 3) is the original's. Nothing is shown while Link carries the bomb (he places
 * it on every step). dt 1: the original. Negative control: T60C_NEGCTRL runs execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* the held picture */
#include "d/t60_carry_step.h" /* the carriables' step form (t60cs) */
#include "d/t60_carry_floor.h" /* the picture floor on the ground plane */
#include <cstdlib>
#include <cstddef>

namespace bomb_t60 {
static inline void get(daBomb_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = a->mFusePos.x, v[4] = a->mFusePos.y, v[5] = a->mFusePos.z;
}
static inline void set(daBomb_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mFusePos.x = v[3], a->mFusePos.y = v[4], a->mFusePos.z = v[5];
}
static inline void mtx(daBomb_c* a) { a->set_mtx(); }
/* held: Link carries the bomb (carry / hookshot carry status, or the frame ended in procCarry 020C76E0) */
static inline bool held(daBomb_c* a) {
    return (static_cast<u32>(a->actor_status) & 0x102000u) != 0 || /* fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e */
           static_cast<u32>(a->mFunc.f) == 0x020C76E0u;
}
static const t60w::Ops<daBomb_c> kOps = {6, 6, get, set, mtx, held};
/* the execute's earlier-step tail; held, the model from this step's placement (set_mtx writes the
 * model's base scale / matrix and the matrix stack scratch only). d/t60_held_pic.h */
static inline BOOL held_picture(daBomb_c* a) {
    t60hp::picture(a, kOps, mtx);
    return TRUE;
}

/* ("convert the whole-form carriables to per-step logic"; rules: the
 * exact frame move family, frame ends bit-exact, decisions on the frame grid, the bombs on thin head hooks with no actor
 * code on earlier steps). Through the carriables' step form d/t60_carry_step.h (t60cs: the frame latch, the tick30
 * put-back of the actor's own earlier-step changes only, the predicted move with its C1 arc, the picture floor, the
 * picture's model / matrix stack words back on tick30):
 *   - every step runs the head; the frame's earlier steps (t60cs PIC) run no decision and no bomb code: they show this
 *     frame's motion. procWait (020C7358): set_wind_vec + posMoveF run once at dt 1 on the bomb's copied words
 *     (t60cs::move: put back), the step shows p0 + D s + dvy/2 s(s-1) (C1 across frame ends) floored at the frame-start
 *     ground (t60cs::check); none on a frame whose fuse may run out (mRestTime 1: the explosion starts at the frame
 *     start, as A). Released by Link on an earlier step (procCarry with the carry status cleared: thrown / put down, the
 *     tick30 run ends the carry), the same from procWait_init. Carried: the model and the fuse point from Link's (or
 *     the hookshot's) placement of the step (t60cs::mtxF -> t60hp::with_grab_dy). The fuse point (mFusePos: the sparks /
 *     smoke emitters' place) moves with the shown place. procExplode (020C74A0): the explosion light (point light power,
 *     point wind strength, ambient / BG colour adds) from the line between the previous and this frame's mExplodeLight;
 *   - the tick30 step (t60cs TICK): the frame start of the bomb's own picture words back (place, old place, fuse point,
 *     light power / strength; a third party's write, e.g. Link's throw speed, stays), the model / stack words back, dt 1,
 *     then the original execute: every decision, the move, the fuse trail shift, the collider, sounds, effects once.
 *   - the step's matrices are drawn as the bomb's own step (no frame interpolation picture note).
 * NC: WWHD_T60_BOMBSTEP_NEGCTRL=1 or t60cs's WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form before (t60w::begin / end,
 * the previous frame's change extrapolated, the held picture). dt 1: the original. */
static inline bool stepNc() {
#if defined(T60C_NEGCTRL) /* (tools/negative control: the whole form's NC, the execute on every step at dt) */
    return true;
#elif defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BOMBSTEP_NEGCTRL"); return e && e[0] == '1'; }();
    return nc || t60cs::nc();
#else
    return t60cs::nc();
#endif
}
enum : u32 { kProcWait = 0x020C7358u, kProcCarry = 0x020C76E0u, kProcExplode = 0x020C74A0u };
/* the holder's carry status bits alone (t60cs Spec.carried: held without them = released this frame) */
static inline bool carried(daBomb_c* a) { return (static_cast<u32>(a->actor_status) & 0x102000u) != 0; }
static const t60cs::Spec<daBomb_c> kSpec = {
    {{(u16)offsetof(daBomb_c, mFusePos), 12},
     {(u16)(offsetof(daBomb_c, mPntLight) + offsetof(LIGHT_INFLUENCE, mPower)), 4},
     {(u16)(offsetof(daBomb_c, mPntWind) + offsetof(BombWindInfluence, mStrength)), 4}},
    held, (u32)sizeof(daBomb_c), (u32)offsetof(daBomb_c, mpModel), carried};
/* the fuse point of the model's base matrix (execute's PSMTXMultVec of l_fuseOffset (0, 60, 5)): a picture value */
static inline void fuseFromModel(daBomb_c* a) {
    const u32 m = gabi::ea(a->mpModel.get());
    if (!m)
        return;
    const J3DModel* j = gabi::at<J3DModel>(m);
    f32 f[3];
    for (int r = 0; r < 3; r++)
        f[r] = (f32)j->mBaseTransformMtx.m[r][1] * 60.0f + (f32)j->mBaseTransformMtx.m[r][2] * 5.0f +
               (f32)j->mBaseTransformMtx.m[r][3];
    a->mFusePos.x = f[0], a->mFusePos.y = f[1], a->mFusePos.z = f[2];
}
static inline void predWait(daBomb_c* b) {
    b->set_wind_vec();
    b->posMoveF();
}
static inline void predRelease(daBomb_c* b) {
    b->procWait_init();
    b->set_wind_vec();
    b->posMoveF();
}
/* the explosion light line: this frame's and the previous frame's mExplodeLight at their first steps (host memory) */
struct Light {
    u32 actor, pcid, tag;
    f32 l0, lPrev;
    bool prev;
};
static Light s_light[8];
static inline bool light(daBomb_c* a, f32& shown) {
    const u32 actor = gabi::ea(a), pcid = *gabi::at<be<u32>>(actor + 0x04), tag = t60w::frameTag();
    Light* e = 0;
    for (Light& x : s_light)
        if (x.actor == actor && x.pcid == pcid)
            e = &x;
    if (!e) {
        e = &s_light[(actor >> 4) & 7];
        e->actor = actor, e->pcid = pcid, e->tag = ~0u;
    }
    if (e->tag != tag) {
        e->prev = e->tag + 1 == tag;
        e->lPrev = e->l0;
        e->l0 = a->mExplodeLight;
        e->tag = tag;
    }
    if (!e->prev)
        return false;
    shown = e->lPrev + (e->l0 - e->lPrev) * t60cs::span();
    return true;
}
/* the explosion light of an earlier step (procExplode's head with the shown value) */
static inline void explodeLight(daBomb_c* a, f32 l) {
    a->mPntLight.mPower = 1500.0f * l;
    a->mPntWind.mStrength = l;
    u32 camera = gabi::at<dComIfG_inf_c>(gabi::ea(dComIfGp_get()))->play.mCameraInfo[0].mpCamera;
    f32 distance = bomb_sqrtf(bomb_PSVECSquareDistance(&a->current.pos, camera + 0xDC));
    f32 factor = distance < 1500.0f ? 1.0f - distance / 1500.0f : 0.0f;
    bomb_dKy_actor_addcol_amb_set(200, 180, 100, l * factor);
    bomb_dKy_bg_addcol_amb_set(180, 160, 60, l * factor);
    bomb_dKy_bg_addcol_dif_set(255, 225, 120, l * factor);
}
/* an earlier step's picture (t60cs PIC) */
static inline void picture(daBomb_c* a) {
    const bool released = static_cast<u32>(a->mFunc.f) == kProcCarry && (s16)a->mFunc.i != 0 &&
                          (static_cast<u32>(a->actor_status) & 0x102000u) == 0;
    if (held(a) && !released) { /* Link's (or the hookshot's) placement of this step: the model and the fuse point */
        if (!t60hp::nc())
            t60cs::mtxF(a, true, [a] {
                mtx(a);
                fuseFromModel(a);
            });
        return;
    }
    const u32 proc = released ? kProcWait : (s16)a->mFunc.i != 0 ? (u32)a->mFunc.f : 0u;
    if (proc == kProcWait && (s16)a->mRestTime != 1) {
        const f32 x0 = a->current.pos.x, y0 = a->current.pos.y, z0 = a->current.pos.z;
        if (released)
            t60cs::move(a, predRelease);
        else
            t60cs::move(a, predWait);
        f32 fy; /* the picture floor on the frame-start ground plane (d/t60_carry_floor.h; t60cs::check's flat form) */
        if (t60cs::s_cur && t60cf::floorAt(&a->mAcch, t60cs::s_cur->p0[0], t60cs::s_cur->p0[2], a->current.pos.x,
                                           a->current.pos.z, &fy) &&
            t60cs::s_cur->p0[1] >= (f32)a->mAcch.m_ground_h + (f32)a->mAcch.m_ground_up_h - 1.0f &&
            (f32)a->current.pos.y < fy) /* (started on or above the ground it found) */
            a->current.pos.y = fy;
        a->mFusePos.x = (f32)a->mFusePos.x + ((f32)a->current.pos.x - x0);
        a->mFusePos.y = (f32)a->mFusePos.y + ((f32)a->current.pos.y - y0);
        a->mFusePos.z = (f32)a->mFusePos.z + ((f32)a->current.pos.z - z0);
        t60cs::mtxF(a, false, [a] { mtx(a); });
    }
    f32 l;
    if (proc == kProcExplode && light(a, l))
        explodeLight(a, l);
}
/* the execute's head: true on an earlier step (handled: the picture), false on the tick30 step (t60cs TICK: the frame
 * start back, dt 1) and at dt 1; NC: the whole form's head */
static inline bool head(daBomb_c* a) {
    if (stepNc()) {
        if (t60w::begin(a, kOps))
            return held_picture(a);
        return false;
    }
    const t60cs::Phase ph = t60cs::begin(a, kSpec); /* (TICK: a carry released this frame takes the previous frame's end place as its old place: Spec.carried) */
    if (ph != t60cs::PIC)
        return false;
    picture(a);
    t60cs::end();
    return true;
}
/* the execute's end on every return (model: the frame reached its model step, for the NC's whole form) */
static inline void tail(daBomb_c* a, bool model) {
    if (stepNc())
        t60w::end(a, kOps, model);
    else
        t60cs::end();
}
}  // namespace bomb_t60
