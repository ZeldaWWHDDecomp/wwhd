/**
 * d_a_pt_action.cpp (WWHD)
 * Enemy - Miniblin: action() with its inlined helpers.
 *
 * Written from the WWHD code (02447300, 10 KB): the
 * GameCube decompilation has only "Nonmatching" stubs for pt_wait, next_pos_set, pt_move,
 * view_check, pt_attack, pt_koke, pt_ples, pt_bat, damage_check, water_check and action. GHS
 * inlined them all into action(); they are written here as inline helpers with the GameCube
 * names (the split of the code between them is a reconstruction).
 */
#include "d/actor/d_a_pt.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_pt_t60.h" /* true60 */

enum { fpcNm_PLAYER_e = 0xA8 };
enum {
    JA_SE_CV_PT_LAND = 0x48F7, JA_SE_CV_PT_JUMP = 0x48F8, JA_SE_CV_PT_DAMAGE = 0x48F9, JA_SE_CV_PT_DIE = 0x48FA,
    JA_SE_CV_PT_KOKE = 0x48FB, JA_SE_CV_PT_KOKE2 = 0x48FC,
    JA_SE_OBJ_PT_JUMP = 0x592D, JA_SE_OBJ_PT_LAND = 0x592E, JA_SE_OBJ_PT_FALL = 0x592F, JA_SE_OBJ_FALL_WATER_S = 0x6918,
};

/* fopAcM_seStart / fopAcM_monsSeStart: HD inlines test &eyePos only */
/* inlined into action */
static inline void se_start(pt_class* i_this, u32 id) {
    cXyz* eye = &i_this->eyePos;
    if (gabi::ea(eye) != 0) mDoAud_seStart(id, eye, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
}
/* inlined into action */
static inline void mons_se_start(pt_class* i_this, u32 id) {
    cXyz* eye = &i_this->eyePos;
    if (gabi::ea(eye) != 0) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        u32 pid = i_this != nullptr ? fopAcM_GetID(i_this) : 0xFFFFFFFFu; /* fopAcM_GetID */
        pt_monsterSound(id, eye, pid, 0, dComIfGp_getReverb(roomNo));
    }
}

/* inlined into action */
static inline void dBgS_GndChk_ct(PtGroundCheck* c) {
    pt_groundCtor(c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 1; i < 7; i++) c->mPass[i] = 0;
    c->mpGrpPassChk = gabi::ea(&c->__vtbl_4C);
    c->__vtbl_4C = gc_tu.gndChk.v4C;
    c->mGrp = 0xE;
    c->mpPolyPassChk = gabi::ea(&c->__vtbl_40);
    c->__vtbl_40 = gc_tu.gndChk.v40;
    c->__vtbl_10 = gc_tu.gndChk.v10;
    c->__vtbl_20 = gc_tu.gndChk.v20;
    c->mPass[0] = 1;
}
/* inlined into action */
static inline void dBgS_GndChk_dt(PtGroundCheck* c) {
    c->__vtbl_20 = gc_tu.gndChkDt[0];
    c->__vtbl_40 = gc_tu.gndChkDt[1];
    c->__vtbl_4C = gc_tu.gndChkDt[2];
    pt_groundDtor(c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}

/* inlined into action */
static inline bool spawn_switch_ok(pt_class* i_this) {
    if (i_this->mEnableSpawnSwitch != 0xFF && !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, dComIfGp_roomControl_getStayNo()))
        return false;
    if (i_this->mDisableRespawnSwitch != 0xFF && dComIfGs_isSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo()))
        return false;
    return true;
}

/* true60: the dispatch's motion blocks (d_a_pt_t60.h journal: a held step replays the blocks its frame's
 * dispatch call ran). At dt = 1 each block is the original code. */
/* pos += v x dt (+ the exact-arc term g on y when `g` is given and the fall speed is not clamped).
 * true60 (a11p91n4): the frame's steps add up in f32 with a rounding of their own: at N = 4 four adds of v / 4 ended
 * one ulp past A's single add on every frame of a straight jump (pos.z += 18.46 from 162.59: +3.05e-5 a frame, 0.000244
 * after the jump's 8 frames, kept from then on and carried into mJumpTarget / mTargetDist / the collider centres). The
 * frame form of the add: the steps move by their share as before (the picture between), and the tick30 step ends the frame
 * on A's sum, frame-start position + the frame-start speed (one f32 add, A's PSVECAdd), when only these adds moved the
 * Miniblin in the frame (host table: the first step's position and speed and the position the last add left; a port latch
 * for its frame, the verification harness reuses the frame tag across inputs). dt = 1: the original. T60C_NEGCTRL: the
 * per-step adds. */
struct PtT60P0 { u32 ea = 0; f32 p[3] = {0, 0, 0}, v[3] = {0, 0, 0}, q[3] = {0, 0, 0}; };
static inline PtT60P0* pt_t60_p0_ent(pt_class* i) {
    static PtT60P0 s_ent[64];
    const u32 ea = gabi::ea(i);
    for (u32 k = 0; k < 64u; k++) {
        PtT60P0& e = s_ent[(ea / 16 + k) & 63u];
        if (e.ea == ea || e.ea == 0) return &e;
    }
    return &s_ent[(ea / 16) & 63u];
}
static void pt_t60_pos_add(pt_class* i_this) {
    if (!t60c::on()) {
        PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
        return;
    }
#ifndef T60C_NEGCTRL
    const u32 key = gabi::ea(&i_this->speed.z) | 0x80000000u;
    PtT60P0* e = pt_t60_p0_ent(i_this);
    bool exact = false;
    if (t60::first_step()) {
        e->ea = gabi::ea(i_this);
        e->p[0] = i_this->current.pos.x, e->p[1] = i_this->current.pos.y, e->p[2] = i_this->current.pos.z;
        e->v[0] = i_this->speed.x, e->v[1] = i_this->speed.y, e->v[2] = i_this->speed.z;
        wwhd_t60_latch_set(key, bk_t60_frame_tag());
    } else if (wwhd_t60_latch_pending(key, bk_t60_frame_tag()) && e->ea == gabi::ea(i_this)) {
        if (i_this->current.pos.x != e->q[0] || i_this->current.pos.y != e->q[1] || i_this->current.pos.z != e->q[2])
            wwhd_t60_latch_set(key, ~bk_t60_frame_tag()); /* moved by something else: the per-step sum stays */
        else
            exact = t60::tick30();
    }
    if (exact) {
        i_this->current.pos.x = e->p[0] + e->v[0];
        i_this->current.pos.y = e->p[1] + e->v[1];
        i_this->current.pos.z = e->p[2] + e->v[2];
        return;
    }
#endif
    i_this->current.pos.x = i_this->current.pos.x + t60c::step(i_this->speed.x);
    i_this->current.pos.y = i_this->current.pos.y + t60c::step(i_this->speed.y);
    i_this->current.pos.z = i_this->current.pos.z + t60c::step(i_this->speed.z);
#ifndef T60C_NEGCTRL
    e->q[0] = i_this->current.pos.x, e->q[1] = i_this->current.pos.y, e->q[2] = i_this->current.pos.z;
#endif
}
/* the pt_attack / pt_bat tails at dt < 1: `pos += speed; speed.y -= g (floor -120)` per frame. A's frame moves y by
 * the speed the frame started with; with the floor inside the frame the arc term is not exact, so the step moves y by
 * its share of the frame-start speed (taken by the frame's first step's tail: host table + a port latch for its frame,
 * the harness reuses the frame tag across inputs); a tail first run on a later step takes the semi-implicit form with
 * the arc term. x / z: pos += speed x dt. Then speed.y -= g x dt, floor -120 (monotone: the frame ends on A's speed). */
template <int U> struct PtT60V0 {
    enum { kSize = 64 };
    struct Ent {
        u32 ea;
        f32 v0;
    };
    static Ent s_ent[kSize];
    static Ent* find(u32 ea) {
        Ent* free_slot = 0;
        for (u32 k = 0; k < (u32)kSize; k++) {
            Ent& e = s_ent[(ea / 16 + k) & (kSize - 1)];
            if (e.ea == ea) return &e;
            if (!free_slot && e.ea == 0) free_slot = &e;
        }
        if (free_slot) free_slot->ea = ea;
        return free_slot ? free_slot : &s_ent[(ea / 16) & (kSize - 1)];
    }
};
template <int U> typename PtT60V0<U>::Ent PtT60V0<U>::s_ent[PtT60V0<U>::kSize];
static void pt_t60_tail(pt_class* i_this, f32 g) {
    /* true60 (d_a_pt_t60.h): the move + gravity whole on the tick30 step at dt 1; nothing before */
    f32 t60Prev;
    if (!pt_t60_tail_frame(&t60Prev))
        return;
    if (t60Prev >= 0.0f) {
        pt_t60_tail(i_this, g);
        t60_set_dt(t60Prev);
        return;
    }
    if (!t60c::on()) { /* the original */
        PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
        f32 vy = i_this->speed.y - g;
        if (vy < -120.0f) vy = -120.0f;
        i_this->speed.y = vy;
        return;
    }
    i_this->current.pos.x = i_this->current.pos.x + t60c::step(i_this->speed.x);
    i_this->current.pos.z = i_this->current.pos.z + t60c::step(i_this->speed.z);
    f32 dy = t60c::step(i_this->speed.y) + pt_t60_arc(g);
#ifndef T60C_NEGCTRL
    {
        const u32 key = gabi::ea(&i_this->speed.y) | 0x80000000u;
        typename PtT60V0<0>::Ent* e = PtT60V0<0>::find(gabi::ea(i_this));
        if (t60::first_step()) {
            e->v0 = i_this->speed.y;
            wwhd_t60_latch_set(key, bk_t60_frame_tag());
            dy = t60c::step(e->v0);
        } else if (wwhd_t60_latch_pending(key, bk_t60_frame_tag())) {
            dy = t60c::step(e->v0);
        }
    }
#endif
    i_this->current.pos.y = i_this->current.pos.y + dy;
    f32 vy = i_this->speed.y - t60c::step(g);
    if (vy < -120.0f) vy = -120.0f;
    i_this->speed.y = vy;
}
/* pt_move mode 1: the jump towards mJumpTarget (speed = 20 along the aim) */
static void pt_t60_b_move1(pt_class* i_this) {
    gabi::Local<cXyz> step;
    step->y = 0.0f;
    step->x = 0.0f;
    step->z = 20.0f;
    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    MtxPosition(step, &i_this->speed);
    pt_t60_pos_add(i_this);
}
static inline s16 pt_wallAngle_t60(pt_class* i_this) { return pt_wallAngle(i_this); }
/* pt_move mode 2: the landing chase onto mJumpTarget with the wall angle */
static void pt_t60_b_move2(pt_class* i_this) {
    cLib_addCalc2(&i_this->current.pos.x, i_this->mJumpTarget.x, 1.0f, fabsf(i_this->speed.x));
    cLib_addCalc2(&i_this->current.pos.y, i_this->mJumpTarget.y, 1.0f, fabsf(i_this->speed.y));
    cLib_addCalc2(&i_this->current.pos.z, i_this->mJumpTarget.z, 1.0f, fabsf(i_this->speed.z));
    s16 z = pt_wallAngle_t60(i_this);
    if (z != 0xDCF) i_this->current.angle.z = z;
}
/* pt_move's end: near the target (reads Link / the bait: tick30 step, after the frame's motion as in A) */
static void pt_t60_b_move_end(pt_class* i_this) {
    if (t60c::end() && i_this->current.pos.y - i_this->mpTargetPos->y > -50.0f && i_this->mTargetDistXZ < 500.0f) {
        i_this->mAction = PT_ATTACK;
        i_this->mMode = 0;
    }
}
/* pt_attack: the turn to the target and the upright chases. true60: A's call turns toward mTargetAngleY, which A's action
 * computes from Link's position after Link's whole frame (Link runs earlier in the process line), then sets the run's
 * speed from the turned heading (mode 2). A step before the tick30 step only sees Link's mid-frame position (a5p91n2: the
 * second Miniblin's step-1 target 0x073E vs A's 0x0766 at f202, from Link x 257.69 vs 260.34), so the turn runs once, on
 * the tick30 step (after Link's last step; the target is recomputed there from the frame-start position), whole (dt 1)
 * from the frame-start heading (the first-step input rule / shared-world-state) */
static void pt_t60_b_att_ang(pt_class* i_this) {
    if (!t60c::end())
        return;
    const f32 prev = t60_set_dt(1.0f);
    s16 target = i_this->mTargetAngleY;
    if (i_this->mTimers[1] != 0) target -= 0x8000;
    cLib_addCalcAngleS2(&i_this->current.angle.y, target, 2, 0x1000);
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 2, 0x1000);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x1000);
    t60_set_dt(prev);
}
/* pt_attack mode 2: the run's speed. true60: A's call sets the frame's speed from the turned heading, then moves the frame
 * with it. The heading turns on the tick30 step (above), so the speed is set there, once, as the whole frame's (dt 1);
 * the tail then moves the whole frame with it (frame form). T60C_NEGCTRL (per-step tails): the earlier steps moved x / z
 * with the previous frame's speed (dt share 1 - dt), and the tick30 step makes up the difference (new - old) x (1 - dt) */
static void pt_t60_b_att_run(pt_class* i_this) {
    if (!t60c::end())
        return;
    const f32 oldX = i_this->speed.x, oldZ = i_this->speed.z;
    const f32 prev = t60_set_dt(1.0f);
    cLib_addCalc2(&i_this->speedF, 20.0f, 1.0f, 4.0f);
    t60_set_dt(prev);
    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    gabi::Local<cXyz> fwd;
    gabi::Local<cXyz> spd;
    fwd->x = 0.0f;
    fwd->y = 0.0f;
    fwd->z = i_this->speedF;
    MtxPosition(fwd, spd);
    i_this->speed.x = (f32)spd->x;
    i_this->speed.z = (f32)spd->z;
#ifdef T60C_NEGCTRL
    if (t60::dt() != 1.0f) {
        const f32 rest = 1.0f - t60::dt();
        i_this->current.pos.x = i_this->current.pos.x + (i_this->speed.x - oldX) * rest;
        i_this->current.pos.z = i_this->current.pos.z + (i_this->speed.z - oldZ) * rest;
    }
#else
    /* true60 (frame form): the earlier steps no longer move (pt_t60_tail runs whole on the tick30
     * step at dt 1 with this speed), so there is no difference to make up */
    (void)oldX, (void)oldZ;
#endif
}
/* pt_attack mode 3: the lunge's slow-down. true60: A's call damps the frame's speed, then moves the frame with it: the
 * frame's damp once, on the first step, and the frame's steps move with that speed */
static void pt_t60_b_att_damp(pt_class* i_this) {
    if (!t60c::first())
        return;
    i_this->speed.x *= 0.8f;
    i_this->speed.z *= 0.8f;
}
/* pt_attack's tail: the move, the gravity (7, floor -120), the ground check on */
static void pt_t60_b_att_tail(pt_class* i_this) {
    pt_t60_tail(i_this, 7.0f);
    i_this->mbBgCheck = 1;
}
/* pt_attack's end: back to jumping when far (reads the target distance: tick30 step, after the frame's motion) */
static void pt_t60_b_att_end(pt_class* i_this) {
    if (t60c::end() && (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT) && i_this->mTargetDist > 700.0f) {
        i_this->mAction = PT_MOVE;
        i_this->mMode = 0;
    }
}
/* pt_koke: the upright chases */
static void pt_t60_b_koke_ang(pt_class* i_this) {
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 2, 0x2000);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x2000);
}
/* pt_koke mode 11: the spin (an integer per-frame add: the step's share, the frame adds exactly mSpinSpeed) */
static void pt_t60_b_koke_spin(pt_class* i_this) {
    i_this->current.angle.y = i_this->current.angle.y + (s16)t60c::frameAdd(&i_this->current.angle.y, 1, i_this->mSpinSpeed);
}
/* fall_move: pos.y += v x dt (+ arc), v -= 7 x dt */
static void pt_t60_b_fall(pt_class* i_this) {
    i_this->mbBgCheck = 1;
    /* true60 (d_a_pt_t60.h): the fall whole on the tick30 step at dt 1; nothing before */
    {
        f32 t60Prev;
        if (!pt_t60_tail_frame(&t60Prev))
            return;
        if (t60Prev >= 0.0f) {
            pt_t60_b_fall(i_this);
            t60_set_dt(t60Prev);
            return;
        }
    }
    f32 vy = i_this->speed.y;
    i_this->current.pos.y = i_this->current.pos.y + t60c::step(vy);
    /* true60: no arc term on a step that starts on the ground after an earlier step's ground
     * reset (v = 0 under a GROUND_HIT, not the frame's first step). The arc makes the frame's steps add up to A's
     * y + v0 for a fall that runs through the frame; after a mid-frame reset A's frame ends on the ground too, and the
     * arc lifted B 0.875 above it every 1.5 frames (v5p91n2 47C5D3AC f203-, the Miniblin standing) */
    const bool ptReset = !t60::first_step() && vy == 0.0f && (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT);
    if (t60c::on() && !ptReset) i_this->current.pos.y = i_this->current.pos.y + pt_t60_arc(7.0f);
    i_this->speed.y = vy - t60c::step(7.0f);
}
/* pt_ples: the squash. true60 (c_lib-real sweep): the Miniblin's own state, eased once a frame, whole (dt 1) on
 * the tick30 step (per-step eases with the cap 0.5 x dt did not add up to A's frame: N-step spec scale.x 0x3E / 0x3F) */
static void pt_t60_b_ples(pt_class* i_this) {
    if (t60c::on() && !t60c::end())
        return;
    const bool whole = t60c::on();
    const f32 prev = whole ? t60_set_dt(1.0f) : 1.0f;
    cLib_addCalc2(&i_this->scale.y, 0.1f, 1.0f, 0.5f);
    cLib_addCalc2(&i_this->scale.x, 1.3f, 0.8f, 0.5f);
    if (whole) t60_set_dt(prev);
}
/* pt_bat: the tumble (integer per-frame adds) */
static void pt_t60_b_bat_spin(pt_class* i_this) {
    i_this->current.angle.y = i_this->current.angle.y + (s16)t60c::share(0x400);
    i_this->current.angle.x = i_this->current.angle.x + (s16)t60c::share(0x300);
}
/* pt_bat's tail: the flight (gravity 5, floor -120) */
static void pt_t60_b_bat_tail(pt_class* i_this) {
    i_this->mbBgCheck = 1;
    pt_t60_tail(i_this, 5.0f);
}

/* the spawn delay was counted down on this frame's first step (port latch; dt = 1: false) */
static inline bool pt_t60_delay_counted(pt_class* i_this) {
#ifdef T60C_NEGCTRL
    (void)i_this;
    return false;
#else
    return t60c::on() &&
           wwhd_t60_latch_pending(gabi::ea(&i_this->mInitialSpawnDelay) | 0x80000000u, bk_t60_frame_tag()) != 0;
#endif
}

/* the Miniblin's frame-start position: stored by the first step (host table + a port latch for its frame: the harness
 * reuses the frame tag across inputs); true and the stored position on the frame's later steps. dt = 1 / first step:
 * false. */
template <int U> struct PtT60Start {
    enum { kSize = 64 };
    struct Ent {
        u32 ea;
        f32 p[3];
        f32 d[2]; /* the dispatch motion (x, z) the knockback's reset discarded on the frame's earlier steps */
        u32 nd;   /* the number of such steps */
    };
    static Ent s_ent[kSize];
};
template <int U> typename PtT60Start<U>::Ent PtT60Start<U>::s_ent[PtT60Start<U>::kSize];
#ifndef T60C_NEGCTRL
static inline typename PtT60Start<0>::Ent* pt_t60_start_ent(pt_class* i_this) {
    const u32 ea = gabi::ea(i_this);
    typename PtT60Start<0>::Ent* e = &PtT60Start<0>::s_ent[(ea / 16) & (PtT60Start<0>::kSize - 1)];
    for (u32 k = 0; k < (u32)PtT60Start<0>::kSize; k++) {
        typename PtT60Start<0>::Ent& c = PtT60Start<0>::s_ent[(ea / 16 + k) & (PtT60Start<0>::kSize - 1)];
        if (c.ea == ea || c.ea == 0) {
            e = &c;
            break;
        }
    }
    return e;
}
#endif
static inline bool pt_t60_frame_start(pt_class* i_this, f32* x, f32* y, f32* z) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return false;
    const u32 ea = gabi::ea(i_this), key = gabi::ea(&i_this->mTargetDist) | 0x80000000u;
    typename PtT60Start<0>::Ent* e = pt_t60_start_ent(i_this);
    if (t60::first_step()) {
        e->ea = ea;
        e->p[0] = i_this->current.pos.x, e->p[1] = i_this->current.pos.y, e->p[2] = i_this->current.pos.z;
        e->d[0] = 0.0f, e->d[1] = 0.0f, e->nd = 0;
        wwhd_t60_latch_set(key, bk_t60_frame_tag());
        return false;
    }
    if (e->ea != ea || !wwhd_t60_latch_pending(key, bk_t60_frame_tag()))
        return false;
    *x = e->p[0], *y = e->p[1], *z = e->p[2];
    return true;
#else
    (void)i_this, (void)x, (void)y, (void)z;
    return false;
#endif
}

/* the knockback's reset on a step before the tick30 step discards that step's dispatch motion; A's call reads the
 * position after its whole dispatch motion in damage_check (the spin distance, the hit angles) BEFORE its knockback
 * goes back to old.pos. The discarded motion is kept here (step start -> position after the dispatch), and the tick30
 * damage_check reads the frame-start position + the frame's whole dispatch motion. dt = 1 / tick30: nothing. */
static inline void pt_t60_kb_discard(pt_class* i_this, f32 startX, f32 startZ) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    typename PtT60Start<0>::Ent* e = pt_t60_start_ent(i_this);
    if (e->ea != gabi::ea(i_this) ||
        !wwhd_t60_latch_pending(gabi::ea(&i_this->mTargetDist) | 0x80000000u, bk_t60_frame_tag()))
        return;
    e->d[0] = e->d[0] + (i_this->current.pos.x - startX);
    e->d[1] = e->d[1] + (i_this->current.pos.z - startZ);
    e->nd++;
#else
    (void)i_this, (void)startX, (void)startZ;
#endif
}
/* the tick30 step: A's position at its damage_check when the frame's earlier steps' motion was discarded (true and x / z;
 * the frame-start position + the earlier steps' dispatch motion + this step's). Otherwise false. */
static inline bool pt_t60_kb_dispatch_pos(pt_class* i_this, f32 sx, f32 sz, f32 startX, f32 startZ, f32* x, f32* z) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || !t60::tick30())
        return false;
    typename PtT60Start<0>::Ent* e = pt_t60_start_ent(i_this);
    if (e->ea != gabi::ea(i_this) || e->nd == 0 ||
        !wwhd_t60_latch_pending(gabi::ea(&i_this->mTargetDist) | 0x80000000u, bk_t60_frame_tag()))
        return false;
    *x = sx + e->d[0] + (i_this->current.pos.x - startX);
    *z = sz + e->d[1] + (i_this->current.pos.z - startZ);
    return true;
#else
    (void)i_this, (void)sx, (void)sz, (void)startX, (void)startZ, (void)x, (void)z;
    return false;
#endif
}

/* action 0: hidden until the player comes near and the spot is in view */
/* inlined into action */
static inline void pt_wait(pt_class* i_this) {
    PtCamera* camera = reinterpret_cast<PtCamera*>(dComIfGp_getCamera(0));
    gabi::Local<PtLineCheck> linChk;
    dBgS_LinChk_ct(linChk);
    i_this->attention_info.flags = 0; /* attention_info.flags */
    i_this->mNoHitTimer = 6;
    s16 delay = i_this->mInitialSpawnDelay;
    /* true60: the delay counts on the first step (first-step AI-timer); the wake-up reads Link and the camera: tick30 step,
     * and not in the frame whose first step counted the delay to 0 (A's call counts it and decides next frame) */
    if (delay != 0) {
        if (t60c::first()) {
            i_this->mInitialSpawnDelay = delay - 1;
#ifndef T60C_NEGCTRL
            if (t60c::on() && !t60::tick30()) /* port latch: the verify harness clears it per input */
                wwhd_t60_latch_set(gabi::ea(&i_this->mInitialSpawnDelay) | 0x80000000u, bk_t60_frame_tag());
#endif
        }
    } else if (t60c::end() && !pt_t60_delay_counted(i_this) && spawn_switch_ok(i_this)) {
        if (fopAcM_searchPlayerDistance(i_this) < i_this->mNoticeRange * 100.0f) {
            /* view_check */
            gabi::Local<cXyz> pos;
            pos->x = (f32)i_this->current.pos.x;
            pos->y = (f32)i_this->current.pos.y;
            pos->z = (f32)i_this->current.pos.z;
            pos->y = i_this->current.pos.y + 100.0f;
            dBgS_LinChk_Set(linChk, &camera->mEye, pos, i_this);
            bool appear = true;
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                gabi::Local<cXyz> tmp;
                gabi::Local<cXyz> d;
                cXyz_mi(&camera->mCenter, tmp, &camera->mEye);
                d->z = (f32)tmp->z;
                d->y = (f32)tmp->y;
                d->x = (f32)tmp->x;
                s16 camAngle = cM_atan2s(d->x, d->z);
                cXyz_mi(pos, tmp, &camera->mEye);
                d->copy(*tmp);
                cMtx_YrotS(calc_mtx(), -camAngle);
                MtxPosition(d, pos);
                /* in front of the camera: appears only when not hidden */
                if (!(pos->z < 0.0f) && i_this->mInitialSpawnWaitsUntilOffScreen != 0) appear = false;
            }
            if (appear) {
                i_this->mInitialSpawnWaitsUntilOffScreen = 0;
                i_this->actor_status |= 0x36;
                i_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
                i_this->mAction = PT_MOVE;
                i_this->mMode = 0;
            }
        }
    }
    dBgS_LinChk_dt(linChk);
}

/* jump target found at the line check's cross point: move it by `ofs` and start jumping */
/* inlined into action */
static inline void jump_target_set(pt_class* i_this, PtLineCheck* linChk, cXyz* ofs) {
    i_this->mJumpTarget.copy(*LinChk_GetCross(linChk));
    gabi::Local<cXyz> d;
    MtxPosition(ofs, d);
    PSVECAdd(&i_this->mJumpTarget, d, &i_this->mJumpTarget);
    dBgS_LinChk_dt(linChk);
    i_this->mMode = 1;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> diff;
    cXyz_mi(&i_this->mJumpTarget, tmp, &i_this->current.pos);
    diff->copy(*tmp);
    f32 h = std_sqrtf(PSVECSquareMag(diff)) * 0.1f;
    if (h > 50.0f) h = 50.0f;
    i_this->mHopSpeed = h;
}

/* pt_move mode 0: pick the next spot to jump to; false when there is none (back to pt_attack) */
/* inlined into action */
static inline bool next_pos_set(pt_class* i_this) {
    gabi::Local<PtLineCheck> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> ofs;
    gabi::Local<cXyz> p1;
    gabi::Local<cXyz> p2;
    if ((u32)(i_this->current.angle.x + 0x1FFF) < 0x3FFF) {
        /* on the floor: jump towards the target (away from it when the switches say so) */
        s16 ang = i_this->mTargetAngleY;
        if (i_this->mTimers[1] != 0 || !spawn_switch_ok(i_this)) {
            ang -= 0x8000;
        }
        ang += (s16)gabi::ftoi(cM_rndFX(4000.0f));
        cMtx_YrotS(calc_mtx(), ang);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        ofs->x = 0.0f;
        ofs->y = 200.0f;
        ofs->z = 200.0f;
    } else {
        Mtx34* m = calc_mtx();
        s16 rnd = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        s16 ang = i_this->current.angle.y + rnd;
        cMtx_YrotS(m, ang);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        ofs->x = 0.0f;
        ofs->z = 200.0f;
        ofs->y = 200.0f;
    }
    MtxPosition(ofs, p1);
    PSVECAdd(p1, &i_this->current.pos, p1);
    dBgS_LinChk_Set(linChk, &i_this->current.pos, p1, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ofs->z = -2.0f;
        ofs->y = -2.0f;
        jump_target_set(i_this, linChk, ofs);
        return true;
    }
    ofs->y = -400.0f;
    ofs->z = 200.0f;
    MtxPosition(ofs, p2);
    PSVECAdd(p2, &i_this->current.pos, p2);
    dBgS_LinChk_Set(linChk, p1, p2, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ofs->z = 0.0f;
        ofs->y = 2.0f;
        jump_target_set(i_this, linChk, ofs);
        return true;
    }
    ofs->z = -50.0f;
    ofs->y = -50.0f;
    MtxPosition(ofs, p1);
    PSVECAdd(p1, &i_this->current.pos, p1);
    dBgS_LinChk_Set(linChk, p2, p1, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ofs->z = 2.0f;
        ofs->y = 0.0f;
        jump_target_set(i_this, linChk, ofs);
        return true;
    }
    dBgS_LinChk_dt(linChk);
    i_this->mAction = PT_ATTACK;
    i_this->mMode = 0;
    return false;
}

/* action 1: jumping from spot to spot */
/* inlined into action */
static inline void pt_move(pt_class* i_this) {
    gabi::Local<cXyz> step;
    step->y = 0.0f;
    step->x = 0.0f;
    step->z = 20.0f;
    switch ((u16)i_this->mMode) {
    case 0:
        if (!next_pos_set(i_this)) return;
        break;
    case 1: {
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> d;
        cXyz_mi(&i_this->mJumpTarget, tmp, &i_this->current.pos);
        d->z = (f32)tmp->z;
        d->y = (f32)tmp->y;
        d->x = (f32)tmp->x;
        i_this->current.angle.y = cM_atan2s(d->x, d->z);
        f32 xz = std_sqrtf(gabi::fmadds(d->x, d->x, d->z * d->z));
        i_this->current.angle.x = -cM_atan2s(d->y, xz);
        s16 z = pt_wallAngle(i_this);
        if (z != 0xDCF) i_this->current.angle.z = z;
        /* true60: the arrival is measured before the frame's motion: first step (first-step AI-timer) */
        if (t60c::first() && std_sqrtf(PSVECSquareMag(d)) < step->z * 1.5f) {
            /* arrived */
            if (cM_rndF(1.0f) < 0.02f && (u32)(i_this->current.angle.x + 0xFFF) < 0x1FFF) {
                i_this->mAction = PT_KOKE;
                i_this->mMode = 0;
                pt_smokeSet(i_this, 3);
            } else {
                i_this->mMode = 2;
                i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(5.0f) + 2.0f);
                pt_anmInit(i_this, 0xC, 3.0f, 2, 1.0f, -1);
                se_start(i_this, JA_SE_OBJ_PT_JUMP);
            }
        } else {
            pt_t60_do(i_this, pt_t60_b_move1); /* true60: the jump, pos += speed x dt */
        }
        break;
    }
    case 2: {
        pt_t60_do(i_this, pt_t60_b_move2); /* true60: the landing chase (cLib steps) and the wall angle */
        s16 t = i_this->mTimers[0];
        if (t == 0 && t60c::first()) { /* true60: the timer's end on the first step (first-step AI-timer) */
            i_this->mMode = 0;
            pt_anmInit(i_this, 9, 3.0f, 0, 1.0f, -1);
            mons_se_start(i_this, JA_SE_CV_PT_JUMP);
        }
        break;
    }
    }
    pt_t60_do(i_this, pt_t60_b_move_end); /* true60: near the target, tick30 step */
}

/* action 2: on the ground, running at the target and attacking */
/* inlined into action */
static inline void pt_attack(pt_class* i_this) {
    f32 targetDist = i_this->mTargetDist;
    pt_t60_do(i_this, pt_t60_b_att_ang); /* true60: the turn and upright chases (whole, tick30 step) */
    u32 groundHit = i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT;
    /* true60: the ground check of the previous step, read on the first step as A reads the previous frame's (first-step AI-timer) */
    const bool t60First = t60c::first();
    if (t60First && groundHit && i_this->mFallSpeedY < -100.0f) {
        /* landed hard */
        i_this->mAction = PT_KOKE;
        i_this->mMode = cM_rndF(1.0f) < 0.5f;
        pt_smokeSet(i_this, 5);
        return;
    }
    switch ((u16)i_this->mMode) {
    case 0:
        if (!t60First) {
            /* true60: the wait's timer end and its speed reset once per frame, on the first step (the gravity of the
             * tail then runs per step from the reset as A's frame does) */
        } else if (i_this->mTimers[0] == 0) {
            i_this->mMode = 1;
        } else {
            i_this->speed.x = 0.0f;
            i_this->speed.y = 0.0f;
            i_this->speed.z = 0.0f;
        }
        break;
    case 1:
        if (t60First && groundHit) {
            if (cM_rndF(1.0f) < 0.02f) {
                i_this->mAction = PT_KOKE;
                i_this->mMode = 0;
                pt_smokeSet(i_this, 3);
            } else {
                i_this->mMode = 2;
                pt_anmInit(i_this, 7, 5.0f, 2, 1.0f, -1);
                i_this->speed.x = 0.0f;
                i_this->speed.y = 0.0f;
                i_this->speed.z = 0.0f;
                i_this->speedF = 0.0f;
                se_start(i_this, JA_SE_OBJ_PT_JUMP);
            }
        }
        break;
    case 2: {
        pt_t60_do(i_this, pt_t60_b_att_run); /* true60: the run's speed (whole, tick30 step) */
        if (t60c::end() && targetDist < 300.0f) { /* true60: reads the target: tick30 step */
            gabi::Local<cXyz> pos;
            cXyz* tp = i_this->mpTargetPos;
            pos->x = (f32)tp->x;
            f32 y = tp->y;
            pos->y = y;
            pos->z = (f32)tp->z;
            pos->y = y + 100.0f;
            gabi::Local<PtLineCheck> linChk;
            dBgS_LinChk_ct(linChk);
            dBgS_LinChk_Set(linChk, &i_this->eyePos, pos, i_this);
            BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), linChk);
            dBgS_LinChk_dt(linChk);
            if (!hit) {
                i_this->mTimers[2] = i_this->mColorNo * 3 + 10;
                i_this->mMode = 3;
                pt_anmInit(i_this, 0xC, 3.0f, 2, 1.0f, -1);
            }
        }
        break;
    }
    case 3: {
        if (t60First && i_this->mTimers[2] == 1) { /* true60: the timer one-shot on the first step */
            pt_anmInit(i_this, 6, 5.0f, 0, 1.0f, -1);
            if (i_this != nullptr) {
                mons_se_start(i_this, JA_SE_CV_PT_LAND);
                se_start(i_this, JA_SE_OBJ_PT_LAND);
            }
        }
        pt_t60_do(i_this, pt_t60_b_att_damp); /* true60: 0.8^dt */
        /* true60 (frame form first-step AI-timer): the attack frame test on the first step (A reads the frame-start morf frame);
         * daPt_Execute holds mbAttack to the tick30 step (a10p91n2: B's sphere off one frame early, f240 / f266) */
        if (t60First && i_this->mTimers[2] == 0) {
            f32 frame = i_this->mpMorf->getFrame();
            if (!(frame < 11.0f) && !(i_this->mpMorf->getFrame() > 14.0f)) {
                i_this->mbAttack = 1;
            }
        }
        if (en_t60_isStop(i_this->mpMorf)) { /* true60: the stop test of the first step, held (d_a_enemy_anm_t60.h) */
            i_this->mMode = 1;
        }
        break;
    }
    }
    pt_t60_do(i_this, pt_t60_b_att_tail); /* true60: pos += speed x dt, gravity x dt with the arc term */
    pt_t60_do(i_this, pt_t60_b_att_end);  /* true60: far from the target, tick30 step */
}

/* inlined into action */
static inline void fall_move(pt_class* i_this) {
    pt_t60_do(i_this, pt_t60_b_fall); /* true60: pos.y += v x dt (+ arc), v -= 7 x dt */
}

/* action 3: tripped / knocked over */
/* inlined into action */
static inline void pt_koke(pt_class* i_this) {
    pt_t60_do(i_this, pt_t60_b_koke_ang); /* true60: the upright chases (cLib steps) */
    const bool t60First = t60c::first();  /* true60: timers, one-shots, frame tests and draws on the first step (first-step AI-timer) */
    bool end = false;
    switch ((u16)i_this->mMode) {
    case 0:
        i_this->mMode = 4;
        pt_anmInit(i_this, 0xA, 2.0f, 0, 1.0f, -1);
        i_this->mTimers[0] = 0;
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_KOKE);
        break;
    case 1:
        i_this->mMode = 5;
        pt_anmInit(i_this, 0xB, 2.0f, 0, 1.0f, -1);
        i_this->mTimers[0] = 0;
        if (i_this != nullptr) {
            mons_se_start(i_this, JA_SE_CV_PT_KOKE);
            se_start(i_this, JA_SE_OBJ_PT_FALL);
        }
        break;
    case 2:
        i_this->mMode = 5;
        pt_anmInit(i_this, 8, 2.0f, 2, 1.0f, -1);
        i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_KOKE2);
        break;
    case 4:
        if (t60First && gabi::ftoi(i_this->mpMorf->getFrame()) == 6 && i_this != nullptr) se_start(i_this, JA_SE_OBJ_PT_FALL);
        /* fallthrough */
    case 5:
        if (t60First && i_this->mTimers[1] == 0) {
            if (en_t60_isStop(i_this->mpMorf) || i_this->mTimers[0] == 1) end = true;
        }
        break;
    case 10:
        i_this->mMode = 11;
        pt_anmInit(i_this, 0xA, 2.0f, 0, 1.0f, -1);
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_KOKE);
        break;
    case 11:
        pt_t60_do(i_this, pt_t60_b_koke_spin); /* true60: the spin's step share */
        if (t60First && gabi::ftoi(i_this->mpMorf->getFrame()) == 6) se_start(i_this, JA_SE_OBJ_PT_FALL);
        if (t60First && i_this->mKnockbackSpeed < 0.05f) {
            i_this->mMode = 12;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(30.0f));
        }
        break;
    case 12:
        if (t60First && i_this->mTimers[0] == 0) end = true;
        break;
    }
    if (end) {
        i_this->mAction = PT_MOVE;
        i_this->mMode = 0;
        pt_anmInit(i_this, 9, 3.0f, 0, 1.0f, -1);
    }
    fall_move(i_this);
}

/* action 4: pressed flat (returns: dies) */
/* inlined into action */
static inline int pt_ples(pt_class* i_this) {
    i_this->current.angle.z = 0;
    i_this->current.angle.x = 0;
    i_this->mNoHitTimer = 5;
    switch ((u16)i_this->mMode) {
    case 0:
        i_this->mTimers[0] = 0x23;
        i_this->mMode = 1;
        pt_anmInit(i_this, 0xC, 1.0f, 0, 5.0f, -1);
        pt_t60_ran(i_this); /* true60: the fall-through runs mode 1 in this call: not held */
        /* fallthrough */
    case 1:
        if (t60c::on() && t60::first_step() && i_this->mTimers[0] == 0) {
            /* true60: the timer's end on the first step ends the Miniblin in this call: A's call squashed once more
             * (the whole frame, dt 1) before it */
            const f32 prev = t60_set_dt(1.0f);
            pt_t60_b_ples(i_this);
            t60_set_dt(prev);
            return 1;
        }
        pt_t60_do(i_this, pt_t60_b_ples); /* true60: the squash (whole, tick30 step) */
        if (t60c::first() && i_this->mTimers[0] == 0) return 1; /* true60: the timer's end on the first step */
        break;
    }
    fall_move(i_this);
    return 0;
}

/* action 5: batted away (returns: dies) */
/* inlined into action */
static inline int pt_bat(pt_class* i_this) {
    i_this->mNoHitTimer = 5;
    switch ((u16)i_this->mMode) {
    case 0: {
        i_this->mTimers[0] = 0x96;
        i_this->mMode = 1;
        pt_anmInit(i_this, 0xC, 1.0f, 0, 5.0f, -1);
        s16 a = fopAcM_searchPlayerAngleY(i_this);
        s16 ang = a + (s16)gabi::ftoi(cM_rndFX(6000.0f)) + 0x8000;
        i_this->mKnockbackAngleY = ang;
        cMtx_YrotS(calc_mtx(), ang);
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = cM_rndF(20.0f) + 50.0f;
        v->z = 100.0f;
        MtxPosition(v, &i_this->speed);
        i_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        pt_t60_ran(i_this); /* true60: the fall-through runs mode 1 in this call: not held */
    }
        /* fallthrough */
    case 1:
        if (t60c::on() && t60::first_step() && i_this->mTimers[0] == 0) {
            /* true60: the timer's end on the first step ends the Miniblin in this call: A's call turned once more (the
             * whole frame, dt 1) before it */
            const f32 prev = t60_set_dt(1.0f);
            pt_t60_b_bat_spin(i_this);
            t60_set_dt(prev);
            return 1;
        }
        pt_t60_do(i_this, pt_t60_b_bat_spin); /* true60: the tumble's step share */
        if (t60c::first() && i_this->mTimers[0] == 0) return 1; /* true60: the timer's end on the first step */
        break;
    }
    pt_t60_do(i_this, pt_t60_b_bat_tail); /* true60: pos += speed x dt, gravity x dt with the arc term */
    /* true60: the ground check A's call reads is the previous frame's: taken on the first step, the death after the
     * frame's motion on the tick30 step (port latches: the wall / ground answer of the first step) */
    u32 flags = i_this->mAcch.m_flags;
    f32 vy = i_this->speed.y;
#ifndef T60C_NEGCTRL
    if (t60c::on()) {
        const u32 kw = gabi::ea(&i_this->mbBgCheck) | 0x80000000u, kg = gabi::ea(&i_this->mbAttack) | 0x80000000u;
        if (t60::first_step()) {
            /* "no hit" = ~tag: never this frame's tag (a fixed 0xFFFFFFFF matched a frame tag of 0xFFFFFFFF) */
            const u32 tag = bk_t60_frame_tag();
            wwhd_t60_latch_set(kw, (flags & dBgS_Acch::WALL_HIT) ? tag : ~tag);
            wwhd_t60_latch_set(kg, (flags & dBgS_Acch::GROUND_HIT) ? tag : ~tag);
        }
        if (!t60::tick30()) return 0;
        flags = (wwhd_t60_latch_pending(kw, bk_t60_frame_tag()) ? (u32)dBgS_Acch::WALL_HIT : 0u) |
                (wwhd_t60_latch_pending(kg, bk_t60_frame_tag()) ? (u32)dBgS_Acch::GROUND_HIT : 0u);
    }
#endif
    if (flags & dBgS_Acch::WALL_HIT) return 1;
    if (vy < 0.0f && (flags & dBgS_Acch::GROUND_HIT)) return 1;
    return 0;
}

/* hits taken and dealt */
/* inlined into action */
static inline void damage_check(pt_class* i_this, fopAc_ac_c* player) {
    i_this->mStts.Move();
    if (i_this->mAtSph.ChkAtHit()) {
        void* hitObj = pt_atHitObj(&i_this->mAtSph);
        if (hitObj != nullptr) {
            dCcD_Stts* stts = reinterpret_cast<dCcD_GObjInf*>(hitObj)->mStts;
            fopAc_ac_c* hitAc = stts != nullptr ? stts->mp_actor.get() : nullptr;
            if (hitAc != nullptr && fpcM_GetName(hitAc) == fpcNm_PLAYER_e) {
                /* hit the player: bounce off a guard */
                if (pt_checkGuard(player)) {
                    i_this->mKnockbackAngleY = fopAcM_searchPlayerAngleY(i_this);
                    i_this->mKnockbackSpeed = 40.0f;
                } else {
                    i_this->mAction = PT_KOKE;
                    i_this->mMode = 2;
                    i_this->mNoHitTimer = 6;
                    return;
                }
            }
        }
    }
    if (reinterpret_cast<PtPlayer*>(player)->mStateFlags & 0x20000) {
        /* the player's spin / shockwave nearby */
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> d;
        cXyz_mi(&player->current.pos, tmp, &i_this->current.pos);
        d->copy(*tmp);
        if (std_sqrtf(PSVECSquareMag(d)) < 350.0f) {
            i_this->mAction = PT_KOKE;
            i_this->mMode = 0;
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
            pt_smokeSet(i_this, 3);
            i_this->mNoHitTimer = 10;
            i_this->speed.y = 350.0f;
            return;
        }
    }
    if (!i_this->mSph.ChkTgHit()) return;

    i_this->mNoHitTimer = 6;
    gabi::Local<PtAttackInfo> atInfo;
    void* hitObj = i_this->mSph.GetTgHitObj();
    atInfo->mpObj = gabi::ea(hitObj);
    atInfo->pParticlePos = gabi::ea(&i_this->mSph.mGObjTg.mHitPos);
    if (reinterpret_cast<dCcD_GObjInf*>(hitObj)->mAtType & 0x100000) {
        /* light arrow */
        i_this->mEnemyIce.mLightShrinkTimer = 1;
        pt_enemyFireRemove(&i_this->mEnemyFire);
        i_this->mSmokeType = 0;
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
        return;
    }
    if (reinterpret_cast<dCcD_GObjInf*>(hitObj)->mAtType & 0x40200) {
        /* fire */
        i_this->mEnemyFire.mFireDuration = 100;
        i_this->mNoHitTimer = 50;
    }
    if (reinterpret_cast<dCcD_GObjInf*>(hitObj)->mAtType & 0x200000) {
        i_this->mKnockbackAngleY = fopAcM_searchPlayerAngleY(i_this);
        pt_smokeSet(i_this, 5);
        i_this->mKnockbackSpeed = cM_rndF(30.0f) + 90.0f;
        i_this->mSpinSpeed = (s16)gabi::ftoi(cM_rndFX(2000.0f));
        i_this->mAction = PT_KOKE;
        i_this->mMode = 10;
        pt_smokeSet(i_this, 10);
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_DAMAGE);
        return;
    }
    pt_checkAttack(i_this, atInfo);
    if (atInfo->mResultingAttackType == 9) {
        if (reinterpret_cast<PtPlayer*>(player)->mCutType == 0x11) {
            i_this->mAction = PT_BAT;
            pt_smokeSet(i_this, 0x1E);
            i_this->mMode = 0;
        } else {
            i_this->mAction = PT_PLES;
            i_this->mMode = 0;
        }
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_DIE);
        i_this->mNoHitTimer = 50;
        return;
    }
    i_this->mKnockbackAngleY = fopAcM_searchPlayerAngleY(i_this);
    pt_smokeSet(i_this, 10);
    if (i_this->health > 0) {
        mons_se_start(i_this, JA_SE_CV_PT_DAMAGE);
        i_this->mAction = PT_ATTACK;
        i_this->mMode = 1;
        i_this->mKnockbackSpeed = 80.0f;
    } else {
        i_this->mNoHitTimer = 100;
        mons_se_start(i_this, JA_SE_CV_PT_DIE);
        i_this->mAction = PT_ATTACK;
        i_this->mKnockbackSpeed = 110.0f;
        i_this->mMode = 1;
    }
}

/* below the ground or in the sea (returns: dies) */
/* inlined into action */
static inline int water_check(pt_class* i_this) {
    int dead = 0;
    gabi::Local<PtGroundCheck> gndChk;
    dBgS_GndChk_ct(gndChk);
    gndChk->m_pos.x = (f32)i_this->current.pos.x;
    gndChk->m_pos.z = (f32)i_this->current.pos.z;
    gndChk->m_pos.y = i_this->current.pos.y + 500.0f;
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    if (groundY != -1e9f && !(i_this->current.pos.y > groundY)) {
        dead = 1;
    } else if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z) - 30.0f;
        if (!(i_this->current.pos.y > h)) {
            i_this->current.pos.y = h;
            gabi::Local<cXyz> pos;
            pos->x = (f32)i_this->current.pos.x;
            pos->y = (f32)i_this->current.pos.y;
            pos->y = h;
            pos->z = (f32)i_this->current.pos.z;
            fopKyM_createWpillar(pos, 1.0f, 1.0f, 0);
            se_start(i_this, JA_SE_OBJ_FALL_WATER_S);
            dead = 1;
        }
    }
    dBgS_GndChk_dt(gndChk);
    return dead;
}

/* 02447300 */
void action(pt_class* i_this) {
    if (pt_t60_deleted(i_this)) /* true60: deleted on an earlier step of this frame (the execute returns before) */
        return;
    /* true60: the target (bait or Link) is measured from the Miniblin's frame-start position, as A's call
     * measures it before the frame's motion: on the first step, and on the tick30 step (Link's tick30 position, rule
     * the first-step input rule addendum) with the frame-start position put in for the measurement; the middle steps keep the values */
    f32 t60Sx = 0.0f, t60Sy = 0.0f, t60Sz = 0.0f;
    const bool t60Swap = pt_t60_frame_start(i_this, &t60Sx, &t60Sy, &t60Sz);
    if (t60c::first() || t60c::end()) {
    f32 t60Cx = i_this->current.pos.x, t60Cy = i_this->current.pos.y, t60Cz = i_this->current.pos.z;
    if (t60Swap) i_this->current.pos.x = t60Sx, i_this->current.pos.y = t60Sy, i_this->current.pos.z = t60Sz;
    fopAc_ac_c* esa = (fopAc_ac_c*)fpcM_Search(PT_ESA_SEARCH_CALLBACK, i_this);
    if (esa != nullptr) {
        i_this->mpTargetPos = &esa->current.pos;
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> d;
        cXyz_mi(&esa->current.pos, tmp, &i_this->current.pos);
        d->copy(*tmp);
        i_this->mTargetDist = std_sqrtf(PSVECSquareMag(d));
        d->y = 0.0f;
        i_this->mTargetDistXZ = std_sqrtf(PSVECSquareMag(d));
        i_this->mTargetAngleY = cM_atan2s(d->x, d->z);
    } else {
        i_this->mpTargetPos = &dComIfGp_getPlayer(0)->current.pos;
        i_this->mTargetDist = fopAcM_searchPlayerDistance(i_this);
        i_this->mTargetDistXZ = fopAcM_searchPlayerDistanceXZ(i_this);
        i_this->mTargetAngleY = fopAcM_searchPlayerAngleY(i_this);
    }
    if (t60Swap) i_this->current.pos.x = t60Cx, i_this->current.pos.y = t60Cy, i_this->current.pos.z = t60Cz;
    }

    int dead = 0;
    /* true60: the step's start position (= old.pos in the game, where fopAc_Execute copies it before every step's
     * execute; the knockback below goes back to it) */
    const f32 t60StartX = i_this->current.pos.x, t60StartZ = i_this->current.pos.z;
    /* true60: one AI state step per 30 Hz frame: a state changed on an earlier step of this frame holds the
     * dispatch; the held step replays the motion blocks (and the tick30 decisions after them) the frame's call ran */
    const u32 t60Before = pt_t60_state(i_this);
    if (pt_t60_held(i_this)) {
        pt_t60_jr_replay(i_this);
    } else {
        pt_t60_jr_begin(i_this);
        switch ((u16)i_this->mAction) {
        case PT_WAIT: pt_wait(i_this); break;
        case PT_MOVE: pt_move(i_this); break;
        case PT_ATTACK: pt_attack(i_this); break;
        case PT_KOKE: pt_koke(i_this); break;
        case PT_PLES: dead = pt_ples(i_this); break;
        case PT_BAT: dead = pt_bat(i_this); break;
        }
        pt_t60_note(i_this, t60Before);
    }
    /* true60: a death decided by the first step's dispatch (pt_ples / pt_bat timers, before their motion) ends the
     * actor in this call: the rest of A's call (hit check, knockback, chases, hop) runs once, as the whole frame (dt 1),
     * and the frame's later steps do nothing */
    f32 t60PrevDt = -1.0f;
    if (dead != 0 && t60c::on() && !t60::tick30()) {
        pt_t60_mark_deleted(i_this);
        t60PrevDt = t60_set_dt(1.0f);
    }

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    /* true60 (the mixed-pair hit-flag rule, first-step AI-timer order addendum; the Keese's rule): the collider status shift and the hit reaction
     * come after the frame's dispatch and motion in A's call: tick30 step (the body's Tg is masked on the in-between
     * Moves, so the tick30 Move's hits are read); the water / ground check after the frame's motion: tick30 step */
    /* true60: A's damage_check and water / ground check read the position after its whole dispatch motion (its
     * knockback comes after): with a knockback on since the frame's start, the tick30 step puts that position in for
     * both (the earlier steps' reset discarded their dispatch motion) and goes back afterwards */
    f32 t60Dx = 0.0f, t60Dz = 0.0f;
    const f32 t60Kx = i_this->current.pos.x, t60Kz = i_this->current.pos.z;
    const bool t60D = t60Swap && pt_t60_kb_dispatch_pos(i_this, t60Sx, t60Sz, t60StartX, t60StartZ, &t60Dx, &t60Dz);
    if (t60D) i_this->current.pos.x = t60Dx, i_this->current.pos.z = t60Dz;
    if (t60c::end() && i_this->mNoHitTimer == 0) {
        damage_check(i_this, player);
    }
    if (t60c::end()) dead += water_check(i_this);
    if (t60D) i_this->current.pos.x = t60Kx, i_this->current.pos.z = t60Kz;

    /* true60 (first-step AI-timer): the knockback runs on the frame's steps when it was on at the frame's start (the first step's answer,
     * held); one the frame's tick30 hit reaction starts runs on the tick30 step as A's whole frame (dt 1: A's call runs
     * it right after the reaction) */
    const bool t60KStart = t60c::on();
    bool t60K = i_this->mKnockbackSpeed > 0.01f;
    f32 t60KPrev = -1.0f;
    if (t60KStart && !t60c::held(t60K, 2, &i_this->mKnockbackSpeed)) {
        t60K = t60K && t60::tick30();
        if (t60K) t60KPrev = t60_set_dt(1.0f);
    }
    if (t60K) {
        /* knock-back */
        if (!t60KStart) {
            i_this->current.pos.x = (f32)i_this->old.pos.x;
            i_this->current.pos.z = (f32)i_this->old.pos.z;
        } else if (t60KPrev >= 0.0f && t60Swap) {
            /* true60: a knockback the tick30 reaction started discards the frame's whole dispatch motion, as A's call
             * goes back to the frame's start (old.pos) */
            i_this->current.pos.x = t60Sx;
            i_this->current.pos.z = t60Sz;
        } else {
            pt_t60_kb_discard(i_this, t60StartX, t60StartZ); /* true60: kept for the tick30 damage_check */
            i_this->current.pos.x = t60StartX;
            i_this->current.pos.z = t60StartZ;
        }
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> mv;
        f32 s = i_this->mKnockbackSpeed;
        s16 a = i_this->mKnockbackAngleY;
        v->y = 0.0f;
        v->x = 0.0f;
        v->z = -s;
        cMtx_YrotS(calc_mtx(), a);
        MtxPosition(v, mv);
        if (!t60c::on()) {
            PSVECAdd(&i_this->current.pos, mv, &i_this->current.pos);
        } else { /* true60 (the per-step movement rule): the step's share of the frame's knockback (old.pos is the step's start) */
            i_this->current.pos.x = i_this->current.pos.x + t60c::step(mv->x);
            i_this->current.pos.y = i_this->current.pos.y + t60c::step(mv->y);
            i_this->current.pos.z = i_this->current.pos.z + t60c::step(mv->z);
        }
        /* true60 (c_lib-real sweep): the knockback's slow-down once a frame, whole on the tick30 step after the frame's
         * motion (A's order: move with the frame's speed, then ease; the earlier steps move with the frame-start speed) */
        if (!t60c::on() || t60c::end()) {
            const bool t60KbWhole = t60c::on();
            const f32 t60KbDt = t60KbWhole ? t60_set_dt(1.0f) : 1.0f;
            cLib_addCalc0(&i_this->mKnockbackSpeed, 1.0f, 7.0f);
            if (t60KbWhole) t60_set_dt(t60KbDt);
        }
        /* true60: the death at the knockback's end, after the frame's motion: tick30 step */
        if (t60c::end() && i_this->health <= 0) {
            if ((i_this->mAcch.m_flags & dBgS_Acch::WALL_HIT) || i_this->mKnockbackSpeed < 0.1f) dead++;
        }
        i_this->mbBgCheck = 1;
        if (t60KPrev >= 0.0f) t60_set_dt(t60KPrev); /* true60 */
    }

    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 4, 0x2000);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 4, 0x1000);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 4, 0x1000);

    /* the hop drawn on top of the movement */
    /* true60 (the per-step movement rule): y += v x dt (the exact-arc term), v -= 5 x dt */
    f32 dy = i_this->mHopSpeed;
    f32 y = i_this->mHopY + t60c::step(dy);
    if (t60c::on()) y = y + pt_t60_arc(5.0f);
    i_this->mHopY = y;
    i_this->mHopSpeed = dy - t60c::step(5.0f);
    if (y < 0.0f) i_this->mHopY = 0.0f;

    /* true60: the smoke's countdown (a visual counter) once per frame on the tick30 step, after the frame's hit reaction
     * as in A's call (a smoke_set of the reaction sees the count the frame started with) */
    if (t60c::end() && i_this->mSmokeType != 0) {
        s8 t = i_this->mSmokeType - 1;
        i_this->mSmokeType = t;
        if (t == 0) dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
    }

    if (dead != 0) {
        gabi::Local<cXyz> pos;
        pos->z = (f32)i_this->current.pos.z;
        pos->x = (f32)i_this->current.pos.x;
        pos->y = i_this->current.pos.y + 30.0f;
        fopAcM_createDisappear(i_this, pos, 5, 4, 0xFF);
        fopAcM_delete(i_this);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        if (i_this->mBehaviorType == 0) {
            i_this->mRespawn = 1;
            if (t60PrevDt >= 0.0f) t60_set_dt(t60PrevDt); /* true60 */
            return;
        }
        if (i_this->mDisableRespawnSwitch != 0xFF) {
            dComIfGs_onSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo());
        }
    }
    if (t60PrevDt >= 0.0f) t60_set_dt(t60PrevDt); /* true60 */
}

/* WWHD glue (tools/verify/autoglue.py)
ext 02516178 void* pt_atHitObj(dCcD_GObjInf* collision)
ext 025192A8 void pt_checkAttack(fopAc_ac_c* actor, PtAttackInfo* info)
ext 025E1AA4 void pt_monsterSound(u32 sound, cXyz* position, u32 processId, u32 parameter, s32 reverb)
ext 02008E0C void pt_groundCtor(PtGroundCheck* check)
ext 02008DAC void pt_groundDtor(PtGroundCheck* check, s32 flags)
ext 02445DBC void pt_anmInit(pt_class* actor, int animation, f32 morph, u8 loop, f32 rate, int sound)
ext 02445F98 void pt_smokeSet(pt_class* actor, s8 type)
ext 02446074 s16 pt_wallAngle(pt_class* actor)
*/
