/* d_a_bomb2_t60.h (WWHD, true 60 build; 2026-11-18): the bomb flower's bomb (d_a_bomb2.cpp)
 * on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_bomb2.cpp keeps thin calls.
 *
 * the whole-frame form rule, carryables (as stone2 / tsubo / obj_barrel): the bomb's frame decides from
 * Link's carry status, the tick30 collision Move's hits and pushes, the ground / water hits and the wind; it moves
 * through the shared movers, swings on its pose tensor (tensor_proc_call, vib_proc) and counts its fuse to the
 * explosion within the frame (mode_proc_call: wait / carry / explode / sink). So the whole _execute runs once per 30 Hz
 * frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h): the fuse and explosion intervals, the
 * decisions, the movers, the springs, the cylinder registration (one-sweep-per-frame attack), effects and sounds once per frame, as the
 * original. The earlier steps change no logic state and show the bomb moving on by the previous frame's change of its
 * position times the frame's dt so far (set_mtx, which also moves the fuse emitters: visual), none while Link carries
 * it. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include "d/t60_held_pic.h" /* the held picture */
#include "d/t60_carry_step.h" /* the carriables' step form (t60cs) */
#include "d/t60_carry_floor.h" /* the picture floor on the ground plane */
#include <cstdlib>
#include <cstring>
#include <cstddef>

namespace bomb2_t60 {
static inline void get(daBomb2::Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
}
static inline void set(daBomb2::Act_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
}
/* true60 (2026-11-22, bf.sh pair / control): set_mtx ends in
 * eff_fuse_move, which shifts the fuse emitters' old positions and, while the fuse burns, starts the ignition sound
 * (se_ignition, 0x6100). The whole form's picture (an earlier step) and its put-back on the tick30 step ran set_mtx too:
 * the sound up to three times a frame (241 starts, A = control 111) and the old positions shifted per call
 * (mFuseOldOldPos +0x8D0 off). Form: a picture set_mtx (s_picMtx) moves the model and the fuse emitter (mFusePos) only;
 * the shift and the sound stay with the tick30 run's own set_mtx, once a frame as A. One unit only (static). NC
 * WWHD_T60_BOMB2SND_NEGCTRL=1. dt 1: no picture step (the original). */
static bool s_picMtx = false;
static inline void mtx(daBomb2::Act_c* a) {
    s_picMtx = true;
    a->set_mtx();
    s_picMtx = false;
}
static inline bool picMtx() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BOMB2SND_NEGCTRL"); return e && e[0] == '1'; }();
    if (nc) /* negative control: the picture set_mtx shifts and sounds as before */
        return false;
#endif
    return s_picMtx;
}
/* held: Link carries the bomb (carry / hookshot carry status, or the frame ended in mode_carry) */
static inline bool held(daBomb2::Act_c* a) {
    return (static_cast<u32>(a->actor_status) & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) != 0 ||
           static_cast<s32>(a->mState) == daBomb2::Mode_CARRY_e;
}
static const t60w::Ops<daBomb2::Act_c> kOps = {3, 3, get, set, mtx, held};
/* the execute's earlier-step tail; held, the model from this step's placement (the picture set_mtx
 * above: no fuse shift, no ignition sound). The fuse emitter's place it writes (mFusePos) is put back: held, no frame-
 * start put-back runs on the tick30 step, and its set_mtx shifts mFusePos into mFuseOldPos (A's frame-end value).
 * d/t60_held_pic.h */
/* (user request 2026-11-23): the fuse sparks stayed at 30 Hz while the bomb is carried: the put-back
 * above leaves mFusePos at the frame end on the earlier steps, and the sparks emitter (FuseSparksCB_c::execute, in the
 * particle calc that runs on every step) is placed at mFusePos. The picture's fuse place is kept here (host memory, one
 * unit) and the sparks callback of the same step uses it instead of the logic word (thin call in d_a_bomb2.cpp); the
 * callback is run for the earlier step from the held picture (heldMtx). The word, the smoke trail (its old positions)
 * and the frame end stay as they were. NC WWHD_T60_FUSEPIC_NEGCTRL=1. */
struct FusePic {
    u32 posEa = 0, step = ~0u;
    f32 p[3] = {0, 0, 0};
};
static FusePic s_fusePic;
static inline bool fusePicNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_FUSEPIC_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
/* the sparks emitter's place: the picture's fuse place noted on this step for this fuse (else the word read) */
static inline void sparksPos(u32 posEa, f32& x, f32& y, f32& z) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (fusePicNc() || posEa == 0 || s_fusePic.posEa != posEa || s_fusePic.step != t60_step_counter() || t60::tick30())
        return;
    x = s_fusePic.p[0], y = s_fusePic.p[1], z = s_fusePic.p[2];
#else
    (void)posEa, (void)x, (void)y, (void)z;
#endif
}
static inline void heldMtx(daBomb2::Act_c* a) {
    /* (word copies through be<u32> views: guest pages are mapped on access in the verify harness, no raw memcpy) */
    const u32 fuseEa = gabi::ea(&a->mFusePos);
    u32 fuse[3];
    for (int i = 0; i < 3; i++)
        fuse[i] = *gabi::at<be<u32>>(fuseEa + 4u * i);
    mtx(a);
#if defined(GABI_GAME_BUILD) && GABI_T60
    s_fusePic.posEa = gabi::ea(&a->mFusePos);
    s_fusePic.step = t60_step_counter();
    s_fusePic.p[0] = a->mFusePos.x, s_fusePic.p[1] = a->mFusePos.y, s_fusePic.p[2] = a->mFusePos.z;
    /* the particle library runs the game's emitter callbacks on the tick30 step only: this step's calc
     * would move the sparks from the frame end's place; the sparks callback's placement (emitter translation and its
     * alive particles, at the picture's fuse place through sparksPos) runs here, before this step's calc. The tick30
     * calc's own callback places them at mFusePos again (the frame end as A). */
    if (!fusePicNc() && static_cast<u8>(a->mFuseLit) && gabi::ea(a->mSparks.mpEmitter.get()) != 0)
        a->mSparks.execute(a->mSparks.mpEmitter.get());
#endif
    for (int i = 0; i < 3; i++)
        *gabi::at<be<u32>>(fuseEa + 4u * i) = fuse[i];
}
static inline bool held_picture(daBomb2::Act_c* a) {
    t60hp::picture(a, kOps, heldMtx);
    return true;
}

/* (rules: the exact frame move family, the bombs on thin head hooks),
 * as d_a_bomb_t60.h, through d/t60_carry_step.h (t60cs): the frame stays decided on the tick30 step (t60cs TICK: the
 * bomb's own picture words back, dt 1, the original _execute: bit-exact frame ends); the earlier steps (PIC) run no
 * bomb code and show this frame's own motion: in mode_wait (not fixed by PRM_1, not eaten) set_wind_vec + posMoveF once
 * at dt 1 on the copied words (t60cs::move, put back), the C1 arc floored at the frame-start ground (t60cs::check),
 * through the picture set_mtx (mFusePos, the fuse emitters' place, moves with it; no trail shift, no ignition sound:
 * s_picMtx); none when the fuse may run out this frame (mBombTimer 1); released by Link on an earlier step (mode_carry
 * with the carry status cleared), the same from mode_wait_init; carried: the held picture (heldMtx through t60cs::mtxF:
 * Link's / the hookshot's placement, the sparks' fuse place); in mode_explode the explosion light (mEnv: point light
 * power, point wind strength, ambient / BG colour adds) from the line between the previous and this frame's mIntensity.
 * NC: WWHD_T60_BOMBSTEP_NEGCTRL=1 or WWHD_T60_CARRYSTEP_NEGCTRL=1: the whole form before. dt 1: the original. */
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
/* the holder's carry status bits alone (t60cs Spec.carried: held without them = released this frame) */
static inline bool carried(daBomb2::Act_c* a) { return (static_cast<u32>(a->actor_status) & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) != 0; }
static const t60cs::Spec<daBomb2::Act_c> kSpec = {
    {{(u16)offsetof(daBomb2::Act_c, mFusePos), 12},
     {(u16)(offsetof(daBomb2::Act_c, mEnv) + offsetof(daBomb2::Env_c, mPntLight) + offsetof(LIGHT_INFLUENCE, mPower)), 4},
     {(u16)(offsetof(daBomb2::Act_c, mEnv) + offsetof(daBomb2::Env_c, mPntWind) + offsetof(daBomb2::Bomb2WindInfluence, mStrength)), 4}},
    held, (u32)sizeof(daBomb2::Act_c), (u32)offsetof(daBomb2::Act_c, mpModel), carried};
static inline void predWait(daBomb2::Act_c* b) {
    b->set_wind_vec();
    b->posMoveF();
}
static inline void predRelease(daBomb2::Act_c* b) {
    b->set_wind_vec();
    b->mode_wait_init();
    b->posMoveF();
}
/* (2026-11-24) the sparks of an unheld moving bomb on the earlier steps. The picture
 * set_mtx moves mFusePos (the sparks emitter's place), but the particle library runs the game's emitter callbacks on the
 * tick30 step only: the sparks stayed at the frame end's place on the earlier steps. As the held picture
 * (heldMtx): the picture's fuse place is noted for sparksPos and the sparks callback runs here, before this
 * step's particle calc; the tick30 calc's own callback places them at mFusePos again (the frame end as A; the word is
 * put back on the tick30 step as a t60cs extra). Game build only. NC WWHD_T60_BOMBSPARKS_NEGCTRL=1 (and the held
 * picture's WWHD_T60_FUSEPIC_NEGCTRL=1). */
static inline bool sparksNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BOMBSPARKS_NEGCTRL"); return e && e[0] == '1'; }();
    return nc || fusePicNc();
#else
    return true;
#endif
}
static inline void picSparks(daBomb2::Act_c* a) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (sparksNc() || !static_cast<u8>(a->mFuseLit) || gabi::ea(a->mSparks.mpEmitter.get()) == 0)
        return;
    s_fusePic.posEa = gabi::ea(&a->mFusePos);
    s_fusePic.step = t60_step_counter();
    s_fusePic.p[0] = a->mFusePos.x, s_fusePic.p[1] = a->mFusePos.y, s_fusePic.p[2] = a->mFusePos.z;
    a->mSparks.execute(a->mSparks.mpEmitter.get());
#else
    (void)a;
#endif
}
/* the explosion light line: this frame's and the previous frame's mIntensity at their first steps (host memory) */
struct Light {
    u32 actor, pcid, tag;
    f32 l0, lPrev;
    bool prev;
};
static Light s_light[8];
static inline bool light(daBomb2::Act_c* a, f32& shown) {
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
        e->l0 = a->mEnv.mIntensity;
        e->tag = tag;
    }
    if (!e->prev)
        return false;
    shown = e->lPrev + (e->l0 - e->lPrev) * t60cs::span();
    return true;
}
/* the explosion light of an earlier step (Env_c::proc's head with the shown value) */
static inline void explodeLight(daBomb2::Act_c* a, f32 l) {
    a->mEnv.mPntWind.mStrength = l;
    a->mEnv.mPntLight.mPower = 1500.0f * l;
    u32 camera = gabi::at<dComIfG_inf_c>(gabi::ea(dComIfGp_get()))->play.mCameraInfo[0].mpCamera;
    f32 distance = bomb2_sqrtf_f32(bomb2_PSVECSquareDistance_f32(&a->current.pos, camera + 0xDC));
    f32 factor = distance < 1500.0f ? 1.0f - distance * 0.0006666666595265269f : 0.0f;
    factor = l * factor;
    bomb2_dKy_actor_addcol_amb_set(200, 180, 100, factor);
    bomb2_dKy_bg_addcol_amb_set(180, 160, 60, factor);
    bomb2_dKy_bg_addcol_dif_set(255, 225, 120, factor);
}
/* an earlier step's picture (t60cs PIC) */
static inline void picture(daBomb2::Act_c* a) {
    const bool released = static_cast<s32>(a->mState) == daBomb2::Mode_CARRY_e &&
                          (static_cast<u32>(a->actor_status) & (fopAcStts_CARRY_e | fopAcStts_HOOK_CARRY_e)) == 0;
    if (held(a) && !released) { /* Link's (or the hookshot's) placement of this step: the held picture (sparks included) */
        if (!t60hp::nc())
            t60cs::mtxF(a, true, [a] { heldMtx(a); });
        return;
    }
    const s32 mode = released ? (s32)daBomb2::Mode_WAIT_e : (s32)a->mState;
    if (mode == daBomb2::Mode_WAIT_e && !a->mEaten && !bomb2_PrmAbstract_u32(a, daBomb2::PRM_1_W, daBomb2::PRM_1_S) &&
        (s32)a->mBombTimer != 1) {
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
        t60cs::mtxF(a, false, [a] { mtx(a); }); /* the picture set_mtx: the model and the fuse emitter's place */
        picSparks(a); /* the sparks at the picture's fuse place on this step */
    }
    f32 l;
    if (mode == daBomb2::Mode_EXPLODE_e && light(a, l))
        explodeLight(a, l);
}
/* the _execute's head: true on an earlier step (handled: the picture), false on the tick30 step (t60cs TICK) and at
 * dt 1; NC: the whole form's head */
static inline bool head(daBomb2::Act_c* a) {
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
/* the _execute's end on every return */
static inline void tail(daBomb2::Act_c* a, bool model) {
    if (stepNc())
        t60w::end(a, kOps, model);
    else
        t60cs::end();
}
}  // namespace bomb2_t60
