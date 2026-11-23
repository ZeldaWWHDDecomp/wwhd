/**
 * d_a_ks_exec.cpp (WWHD)
 * Enemy - Morth: daKS_Execute with the inlined action functions and naraku_check.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ks.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ks.h"

#include <cmath>
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_ks_exec_t60.h" /* true60 (t60conv): step helpers */
/* true60: a landing (tyaku_check) that starts a hop / bounce or ends a state is a decision on the frame's ground answer:
 * the first step only (the original sees a landing at the next frame's start; a landing found by a mid-frame BG check
 * waits for the next frame's first step, as A's end-of-frame check does). tyaku_check itself still runs on every step. */
static inline bool ks_t60_land(ks_class* i_this, int site, bool live) {
    (void)i_this, (void)site;
    return live && t60c::first();
}

/* true60 (case 2): the run / damp branch's draws (and the sway phase's add) are the frame's,
 * taken on the tick30 step in the branch the tick30 decision picks. A step before it that ran case 2 marks its branch;
 * when the state was left after that step (kuttuki, a hit: the tick30 step holds the dispatch), the tick30 step takes
 * the marked branch's draws there, so every frame that ran case 2 draws once, as A does. dt 1: never marked. */
enum { KS_T60_C2_DAMP = 6, KS_T60_C2_RUN = 7 };
/* true60: the frame-start ChkGroundHit for the tick30 heading aim (held site) */
enum { KS_T60_AIM_G0 = 11, KS_T60_AIM_OFS = 12 };
static inline void ks_t60_c2_run_draws(ks_class* i_this) {
    if (i_this->mMoveAngleOffset == 0) {
        f32 id = (f32)(fopAcM_GetID(i_this) & 0xf);
        i_this->mMoveAngleOffset = (s16)gabi::ftoi(id * cM_rndFX(512.0f));
    }
    i_this->shape_angle.z = (s16)gabi::ftoi(cM_rndFX(2000.0f));
}
static inline void ks_t60_c2_deferred(ks_class* i_this) {
    if (t60c::marked(KS_T60_C2_RUN, i_this)) {
        ks_t60_c2_run_draws(i_this);
        i_this->m2F0[0] += 1000;
    } else if (t60c::marked(KS_T60_C2_DAMP, i_this)) {
        (void)cM_rndF(1.0f); /* the damping draw (the step used the mean) */
    }
}

enum { fpcNm_GM_e = 0xCC, fpcNm_KS_e = 0xCD };
enum {
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_CV_KS_ATTACK = 0x486D,
    JA_SE_CV_KS_DAMAGE = 0x486E,
    JA_SE_CV_KS_BLOW = 0x486F,
    JA_SE_CM_KS_MOVE = 0x587A,
    JA_SE_CM_KS_ATTACK = 0x587B,
};
enum : u32 { daPyStts0_SWIM_e = 0x100000 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02041C30-style: enemy_ice(enemyice*) 020402C8 (as in d_a_cc_exec) */
static inline BOOL enemy_ice(enemyice_l* e) { return ksExecute_ice(e); }
static inline void ripple_remove(void* r) { ksExecute_rippleEnd(r); } /* 025A9270 dPa_rippleEcallBack::end */
/* 025D5418 fopAcM_setRoomLayer(actor, s8 roomNo) */
static inline void fopAcM_setRoomLayer(fopAc_ac_c* a, s32 roomNo) { ksExecute_roomLayer(a, roomNo); }
/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
static inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb): HD fopAcM_monsSeStart inline */
static inline void monsSeStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        ksExecute_monsterSound(id, &a->eyePos, pid, param, reverb);
    }
}
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr) monsSeStart_nn(a, id, param);
}
/* dComIfGp_checkPlayerStatus0(0, flag): play+0x5CD8 */
/* daPy_lk_c virtuals (HD vtable at +0xB4): getModelJointMtx +0xFC, checkFrontRoll +0x5C */
static inline u32 daPy_getModelJointMtx(fopAc_ac_c* link,u16 joint) { return ksPlayerJoint(link,joint); }
static inline BOOL daPy_checkFrontRoll(fopAc_ac_c* link) { return ksPlayerRolling(link); }
static inline f32 REG8_F(int i) { return REG_F(8, i); }
static inline f32 REG12_F(int i) { return REG_F(12, i); }

/* inlined into daKS_Execute */
static inline void clear_m2F0(ks_class* i_this) {
    for (int i = 0; i < 5; i++) {
        i_this->m2F0[i] = 0;
    }
}

/* inlined into daKS_Execute: action_dousa_move */
static inline void action_dousa_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* player (unused) */
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();

    switch (i_this->mMode) {
    case 0: if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        i_this->mSwayAmp = 0.0f;
        actor->gravity = -3.0f;
        clear_m2F0(i_this);
        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        // Fall-through
    case 1: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 1, 0x1000);

        if (fopAcM_searchPlayerDistance(actor) > 10000.0f) {
            break;
        }

        if (actor->shape_angle.z > 0x100) {
            return;
        }

        actor->shape_angle.z = 0;

        fopAcM_seStart_nn(actor, JA_SE_CM_KS_MOVE, 0);

        if (tyaku_check(i_this) != 0) {
            actor->gravity = -3.0f;
            // HD: speed.y is not re-read after cM_rndF
            actor->speed.y = 1.0f;
            actor->speed.y = 1.0f + cM_rndF(5.0f);
        }

        s16 ang = fopAcM_searchPlayerAngleY(actor);
        actor->current.angle.y = ang + i_this->mMoveAngleOffset;

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        // Fall-through
    }
    case 2: {
        /* true60 (ks f206): the hop heading is aimed once per frame on the frame-start ground
         * answer, on the tick30 step: it reads Link (an earlier actor) at his frame-end position, as A does (first-step AI-timer: order
         * beats first-step placement); the offset drawn below is first used next frame, as in the original. A heading
         * that turns there moves the whole frame from the frame-start position (first-step AI-timer catch-up addendum) */
        bool ksAimLate = false;
        if (t60c::end() && t60c::held(i_this->mAcch.ChkGroundHit(), KS_T60_AIM_G0, i_this)) {
            s16 ang = fopAcM_searchPlayerAngleY(actor);
            /* the frame-start offset (A aims before the frame's damp branch clears it / the run branch redraws it) */
            const s16 ksAim = (s16)(ang + (s16)t60c::heldInt((s16)i_this->mMoveAngleOffset, KS_T60_AIM_OFS, i_this));
            ksAimLate = !t60::first_step() && ksAim != (s16)actor->current.angle.y;
            actor->current.angle.y = ksAim;
            if (ksAimLate) ks_t60_catch_begin(i_this, &actor->current.pos, false);
        }

        /* true60: Link's speed answer (both reads, in the original's order) is the tick30 step's (A
         * reads Link's frame-end value), held over the next frame's earlier steps (ks_t60_link_decision) */
        if ((u32)(s32)KUTTUKU_ALL_COUNT < 0x14 && (ks_t60_link_decision(i_this, link->speedF > 12.0f) || HEAVY_IN) &&
            fopAcM_searchPlayerDistance(actor) < 500.0f && !dComIfGp_checkPlayerStatus0(0, daPyStts0_SWIM_e) &&
            (ks_t60_land(i_this, 0, tyaku_check(i_this)) ||
             (ks_t60_c2_late(i_this, KS_T60_C2_DAMP) && t60c::held(false, KS_T60_G0, i_this) && tyaku_check(i_this)))) {
            /* true60: the attack starts from a landing: first step; on the tick30 step when Link's answer turned there
             * (first-step AI-timer catch-up addendum, 2026-11-19: A attacks on this frame from the frame-start landing) */
            if (ks_t60_c2_late(i_this, KS_T60_C2_DAMP)) ks_t60_catch_begin(i_this, &actor->current.pos, false);
            /* true60 (ks f206 rule): the state is left on a step before the tick30 step: the frame's heading aim is
             * taken here (A aims at case 2's top before this branch; the tick30 step does not run case 2) */
            if (!t60c::end() && t60c::held(i_this->mAcch.ChkGroundHit(), KS_T60_AIM_G0, i_this))
                actor->current.angle.y = (s16)(fopAcM_searchPlayerAngleY(actor) +
                                               (s16)t60c::heldInt((s16)i_this->mMoveAngleOffset, KS_T60_AIM_OFS, i_this));
            i_this->mAction = KS_ATTACK;
            i_this->mMode = 10;
            return;
        }

        /* GHS: `ble` on speedF > 12 (taken on NaN) */
        if (!ks_t60_link_decision(i_this, link->speedF > 12.0f) || dComIfGp_checkPlayerStatus0(0, daPyStts0_SWIM_e)) {
            /* true60 (first-step AI-timer catch-up addendum, 2026-11-19): the frame's earlier steps ran: the stop decided here
             * moves the whole frame from the frame-start position */
            if (ks_t60_c2_late(i_this, KS_T60_C2_RUN)) ks_t60_catch_begin(i_this, &actor->current.pos, false);
            cLib_addCalc0(&actor->speedF, 0.3f, (t60c::end() ? cM_rndF(1.0f) : 0.5f) + 0.3f);
            if (!t60c::end()) t60c::mark(KS_T60_C2_DAMP, i_this); /* true60: the frame's draw is owed (taken on the tick30 step) */ /* true60 (the per-step chance rule visual draw): the branch is the tick30 step's decision on Link's frame-end speed, so the frame's draw is taken on the tick30 step (one draw per frame from the branch A takes), the mean between */
            cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 1, 0x1000);
            i_this->mMoveAngleOffset = 0;
            break;
        }

        /* true60 (the per-step chance rule event draw / visual jitter): the offset (first used by the next frame's
         * heading aim, as in the original; an offset of 0 draws again next frame) and the roll jitter are drawn once per
         * frame on the tick30 step, where the run decision is taken (A's order); between: the held draw */
        if (t60c::end()) ks_t60_c2_run_draws(i_this);
        else t60c::mark(KS_T60_C2_RUN, i_this);

        /* true60 (first-step AI-timer catch-up addendum, 2026-11-19): a run started on the tick30 step (the earlier steps damped on
         * the held Link answer) moves the whole frame from the frame-start position: the sway shift x 1 here, the speed
         * / dt in the posMove below */
        const bool ksCatch = ks_t60_c2_late(i_this, KS_T60_C2_DAMP);
        if (ksCatch) ks_t60_catch_begin(i_this, &actor->current.pos, false);
        const f32 ksSx = actor->current.pos.x, ksSz = actor->current.pos.z;
        speed_keisan(i_this, actor->current.angle.y);
        if ((ksCatch || ksAimLate) && t60c::marked(KS_T60_CATCH, i_this)) {
            const f32 r = gabi::fsubs_ppc(1.0f / t60::dt(), 1.0f); /* the earlier steps' share of the shift */
            actor->current.pos.x = gabi::fadds_ppc(actor->current.pos.x, gabi::fmuls_ppc(gabi::fsubs_ppc(actor->current.pos.x, ksSx), r));
            actor->current.pos.z = gabi::fadds_ppc(actor->current.pos.z, gabi::fmuls_ppc(gabi::fsubs_ppc(actor->current.pos.z, ksSz), r));
        }

        i_this->mSwayAmp = 10.0f;
        actor->speedF = 12.0f;
        if (t60c::end()) { i_this->m2F0[0] += 1000; } /* true60 (the 30 Hz timer rule form): the sway phase's per-frame add once per 30 Hz frame on the tick30 step, where the run decision is taken (A adds after the frame's speed_keisan) */

        /* true60 (first-step AI-timer catch-up addendum): the run's hop launched on the tick30 step that started the run, on the
         * frame-start landing (A hops on this frame); its y moves the whole frame too */
        if (ks_t60_land(i_this, 1, tyaku_check(i_this)) ||
            (ksCatch && t60c::held(false, KS_T60_G0, i_this) && tyaku_check(i_this))) {
            if (ksCatch) ks_t60_catch_begin(i_this, &actor->current.pos, true);
            fopAcM_seStart_nn(actor, JA_SE_CM_KS_MOVE, 0);
            actor->gravity = -3.0f;
            actor->speed.y = 1.0f;
            actor->speed.y = 1.0f + cM_rndF(5.0f);
        }
        break;
    }
    case 3: {
        if (fopAcM_searchPlayerDistance(actor) < 500.0f) {
            actor->current.angle.y = fopAcM_searchPlayerAngleY(actor);
        }
        break;
    }
    }

    if (i_this->mMode == 2 || i_this->mMode == 3) {
        cLib_addCalcAngleS2(&actor->shape_angle.y, actor->current.angle.y, 1, 0x1000);
    }

    if (body_atari_check(i_this) != 0) {
        if (t60c::first()) fopAcM_seStart(actor, JA_SE_LK_LAST_HIT, 0); /* true60: a per-frame start, once per 30 Hz frame on the first step (a held dispatch skips the later steps) */
    }
}

/* inlined into daKS_Execute: action_kougeki_move */
static inline void action_kougeki_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 head_top_y = (f32)ksPlayer(dComIfGp_getPlayer(0))->mHeadTopPos.y; /* getHeadTopPos().y */

    i_this->mSphRadius = 20.0f;

    if (head_top_y > actor->current.pos.y) {
        i_this->mSphRadius = 60.0f;
    }

    switch (i_this->mMode) {
    case 10: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        i_this->mSph.mObjCo.mSPrm &= ~1u; /* OffCoSetBit */

        actor->speedF = 26.0f;
        actor->gravity = -4.0f;
        actor->speed.y = 28.0f;

        clear_m2F0(i_this);

        i_this->mSwayAmp = 0.0f;

        actor->current.angle.y = fopAcM_searchPlayerAngleY(actor);

        fopAcM_seStart_nn(actor, JA_SE_CM_KS_ATTACK, 0);
        monsSeStart_nn(actor, JA_SE_CV_KS_ATTACK, 0);

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        break;
    }
    case 11: {
        if (i_this->m2F0[1] == 0) {
            if (actor->speedF > 0.0f && (i_this->mSph.mGObjAt.mRPrm & 1) /* ChkAtShieldHit */) {
                actor->gravity = -4.0f;
                actor->speed.y = 25.0f;
                i_this->m2F0[1] = 1;
                actor->speedF = actor->speedF * -0.5f;
            } else if (ks_kuttuki_check(i_this)) {
                return;
            }
        }

        if (!ks_t60_land(i_this, 0, i_this->mAcch.ChkGroundHit() || i_this->mAcch.ChkWaterIn())) { /* true60: the landing: first step */
            break;
        }

        i_this->mSph.mObjCo.mSPrm |= 1u; /* OnCoSetBit */

        i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);

        i_this->mMode = i_this->mMode + 1; /* true60: the landing (seen on the first step) is an event, per event */
        // Fall-through
    }
    case 12:
        ks_kuttuki_check(i_this);

        if (ks_t60_land(i_this, 2, tyaku_check(i_this))) {
            actor->gravity = -4.0f;
            actor->speed.y = 1.0f;
            actor->speed.y += cM_rndF(5.0f);
            actor->speedF = 16.0f;
        }

        if (i_this->mTimers[2] == 0) {
            i_this->mSphRadius = 20.0f;
            i_this->mAction = KS_MOVE;
            i_this->mMode = 0;
            actor->speedF = 0.0f;
        }
        break;
    }

    cLib_addCalcAngleS2(&actor->shape_angle.y, actor->current.angle.y, 1, 0x1000);

    if (body_atari_check(i_this)) {
        if (t60c::first()) fopAcM_seStart_nn(actor, JA_SE_LK_LAST_HIT, 0); /* true60: a per-frame start, once per 30 Hz frame on the first step (a held dispatch skips the later steps) */
    }
}

/* inlined into daKS_Execute: action_kaze_move */
static inline void action_kaze_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();

    switch (i_this->mMode) {
    case 20: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        clear_m2F0(i_this);

        i_this->mTimers[1] = 0x32;

        fopAcM_monsSeStart(actor, JA_SE_CV_KS_BLOW, 0);

        i_this->mWindStartY = actor->current.pos.y;
        i_this->mSwayAmp = 0.0f;

        actor->speedF = cM_rndF(15.0f) + 10.0f;
        i_this->mWindRiseSpeed = cM_rndF(15.0f) + 10.0f;

        i_this->mWindRiseStep = 0.0f;

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        // Fall-through
    }
    case 21: {
        cLib_addCalc2(&actor->speed.y, i_this->mWindRiseSpeed, 0.8f, i_this->mWindRiseStep);
        cLib_addCalc2(&i_this->mWindRiseStep, 5.0f, 1.0f, 0.5f);

        if (i_this->mWindStartY + 200.0f < actor->current.pos.y || i_this->mTimers[1] == 0) {
            i_this->mMode = i_this->mMode + 1; /* true60: a height / timer crossing, once (the next mode), unchanged */
        }
        break;
    }
    case 22: {
        cLib_addCalc2(&actor->gravity, -1.0f, 0.3f, 0.5f);

        if (actor->speed.y < -2.0f) {
            actor->speed.y = -2.0f;
        }

        cLib_addCalc0(&actor->speedF, 0.5f, 2.0f);

        speed_keisan(i_this, actor->shape_angle.y);

        if (t60c::first()) { i_this->m2F0[0] += 3000; } /* true60 (the 30 Hz timer rule form): the per-frame add once per 30 Hz frame on the first step (a step share stays incomplete when the state ends on an earlier step) */
        i_this->mSwayAmp = 2.0f;

        if (ks_t60_land(i_this, 0, tyaku_check(i_this))) { /* true60: the landing: first step */
            i_this->mAction = KS_MOVE;
            i_this->mMode = 0;
            return;
        }
        break;
    }
    }

    if (t60c::first()) { actor->shape_angle.z += 0x500; ks_t60_pic_add(i_this, KS_T60_PIC_Z, 0x500); } /* true60 (the 30 Hz timer rule form): the per-frame add once per 30 Hz frame on the first step (a step share stays incomplete when the state ends on an earlier step) */

    if (body_atari_check(i_this)) {
        if (t60c::first()) fopAcM_seStart_nn(actor, JA_SE_LK_LAST_HIT, 0); /* true60: a per-frame start, once per 30 Hz frame on the first step (a held dispatch skips the later steps) */
    }
}

/* inlined into daKS_Execute: action_dead_move */
static inline void action_dead_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;

    switch (i_this->mMode) {
    case 30: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        clear_m2F0(i_this);

        i_this->mSph.mObjTg.mSPrm &= ~1u; /* OffTgSetBit */
        i_this->mSph.mObjCo.mSPrm &= ~1u; /* ClrCoSet */
        i_this->mSph.ClrTgHit();

        fopAcM_monsSeStart(actor, JA_SE_CV_KS_DAMAGE, 0);

        actor->speedF = cM_rndF(5.0f) + 15.0f;
        actor->gravity = -3.0f;
        actor->speed.y = cM_rndF(5.0f) + 20.0f;

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */

        i_this->m2F0[1] = (s16)gabi::ftoi(cM_rndFX(0x1000));

        i_this->mSph.mObjAt.mSPrm &= ~1u; /* OffAtSPrmBit(cCcD_AtSPrm_Set_e) (twice in GameCube) */
        // Fall-through
    }
    case 31: {
        if (t60c::first()) { actor->shape_angle.z += i_this->m2F0[1]; ks_t60_pic_add(i_this, KS_T60_PIC_Z, (s16)i_this->m2F0[1]); } /* true60 (the 30 Hz timer rule form): the per-frame add once per 30 Hz frame on the first step (a step share stays incomplete when the state ends on an earlier step) */

        if (ks_t60_land(i_this, 3, tyaku_check(i_this))) {
            actor->gravity = -3.0f;
            actor->speedF = actor->speedF * 0.5f;

            switch (i_this->m2F0[0]) {
            case 0:
                actor->speed.y = 13.0f;
                i_this->m2F0[0] += 1; /* true60: a bounce count, per landing event (unchanged) */
                break;
            case 1:
                actor->speed.y = 7.0f;
                i_this->m2F0[0] += 1; /* true60: a bounce count, per landing event (unchanged) */
                break;
            case 2: {
                cXyz local_28;
                local_28.x = actor->current.pos.x;
                local_28.y = actor->current.pos.y;
                local_28.z = actor->current.pos.z;
                local_28.y = local_28.y + 20.0f;
                dead_eff_set(i_this, &local_28);
                break;
            }
            }
        }
        break;
    }
    case 32: {
        cXyz local_28;
        local_28.x = actor->current.pos.x;
        local_28.y = actor->current.pos.y;
        local_28.z = actor->current.pos.z;
        local_28.y = local_28.y + 45.0f;
        dead_eff_set(i_this, &local_28);
        break;
    }
    }
}

/* inlined into daKS_Execute: action_omoi */
static inline void action_omoi(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();

    // HD: instead of `if (mFirstStuck == 1 && HEAVY_IN) link->onHeavyState()`, a counter in the link
// player is bumped every frame
    be<s16>& heavy = ksPlayer(link)->mHeavyStateCounter;
    heavy = heavy + 1;

    switch (i_this->mMode) {
    case 40: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        ripple_remove(i_this->mRipple);

        actor->speedF = 0.0f;
        actor->gravity = 0.0f;
        actor->speed.x = 0.0f;
        actor->speed.y = 0.0f;
        actor->speed.z = 0.0f;

        if (GORON_COUNT != 0 || KUTTUKU_ALL_COUNT >= 0x14) {
            i_this->mpStickJointMtx = 0;
            actor->actor_status &= ~0x4000u; /* fopAcM_OffStatus(actor, fopAcStts_UNK4000_e) */
            i_this->mMode = 42;
            break;
        }

        clear_m2F0(i_this);

        if (KUTTUKU_ALL_COUNT >= 0x14) {
            i_this->mStickJointNo = 0x13;
        }
        s16 idx;
        if (KUTTUKU_ALL_COUNT < 0) {
            idx = 0;
        } else {
            idx = (s16)KUTTUKU_ALL_COUNT;
        }
        i_this->mStickJointNo = idx;

        i_this->mpStickJointMtx = daPy_getModelJointMtx(link, (u16)pl_harituki_joint_dt[idx]);

        if (KUTTUKU_ALL_COUNT == 0) {
            GORON_COUNT = 0;
            i_this->mFirstStuck = 1;
        }

        KUTTUKU_ALL_COUNT = KUTTUKU_ALL_COUNT + 1;

        // HD: no `if (!HEAVY_IN && KUTTUKU_ALL_COUNT >= 5) { onHeavyState(); HEAVY_IN = TRUE; }`

        i_this->mStickJointDir = 0;

        i_this->mTimers[1] = 0;
        i_this->mTimers[2] = 0;

        actor->actor_status |= 0x4000u; /* fopAcM_OnStatus(actor, fopAcStts_UNK4000_e) */

        fopAcM_setStageLayer(actor);

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        // Fall-through
    }
    case 0x29: {
        actor->current.roomNo = player->current.roomNo; /* fopAcM_SetRoomNo */

        if (t60c::first()) { /* true60 (the 30 Hz timer rule): the stuck Morth's turn, once per 30 Hz frame on the first step (a share
                              * would stay incomplete when the state ends on an earlier step of the frame) */
            const s16 a0 = (s16)actor->shape_angle.y;
            s16 a1;
            if (!(fopAcM_GetID(actor) & 1)) {
                a1 = (s16)gabi::ftoi(gabi::fmadds((f32)(s16)i_this->mSpinSpeed, 0.25f, (f32)a0));
            } else {
                a1 = (s16)gabi::ftoi(gabi::fnmsubs((f32)(s16)i_this->mSpinSpeed, 0.25f, (f32)a0));
            }
            actor->shape_angle.y = a1;
            ks_t60_pic_add(i_this, KS_T60_PIC_Y, (s16)(a1 - a0)); /* true60: the picture share */
        }

        f32 x, y, z;
        Mtx34* m = ksAttachedMatrix(i_this);
        if (m != nullptr) {
            x = (f32)m->m[0][3];
            z = (f32)m->m[2][3];
            y = (f32)m->m[1][3];
        } else {
            x = player->current.pos.x;
            z = player->current.pos.z;
            y = player->current.pos.y;
        }

        if (x == 0.0f && y == 0.0f && z == 0.0f) {
            y = player->current.pos.y;
            x = player->current.pos.x;
            z = player->current.pos.z;
        }

        f32 psx = player->speed.x;
        f32 bound = REG12_F(16) + 10.0f;
        f32 lx = std::fabs(psx * 10.0f);
        if (lx < bound) {
            lx = bound;
        }
        f32 psy = player->speed.y;
        f32 ly = std::fabs(psy * 10.0f);
        if (ly < bound) {
            ly = bound;
        }
        f32 psz = player->speed.z;
        f32 lz = std::fabs(psz * 10.0f);
        if (lz < bound) {
            lz = bound;
        }

        if (std_sqrtf(gabi::fmadds(psz, psz, gabi::fmadds(psx, psx, psy * psy))) < REG12_F(17) + 8.0f) {
            f32 l = REG12_F(18) + 8.0f;
            lx = l;
            ly = l;
            lz = l;

            f32 dy = actor->current.pos.y - y;
            f32 dx = actor->current.pos.x - x;
            f32 dz = actor->current.pos.z - z;

            if (std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy))) < 2.0f && i_this->mTimers[1] == 0) {
                i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(10.0f) + 10.0f);

                switch (i_this->mStickJointDir) {
                case 0:
                    if (i_this->mStickJointNo < 0x13) {
                        if (t60c::first()) { i_this->mStickJointNo += 1; } /* true60 (t60conv): counted on the frame's first step */
                    } else {
                        i_this->mStickJointDir = 1;
                        if (t60c::first()) { i_this->mStickJointNo -= 1; } /* true60 (t60conv): counted on the frame's first step */
                    }
                    break;
                case 1:
                    if (i_this->mStickJointNo > 0) {
                        if (t60c::first()) { i_this->mStickJointNo -= 1; } /* true60 (t60conv): counted on the frame's first step */
                    } else {
                        i_this->mStickJointDir = 0;
                        if (t60c::first()) { i_this->mStickJointNo += 1; } /* true60 (t60conv): counted on the frame's first step */
                    }
                    break;
                }

                if (i_this->mStickJointNo >= 0x14) {
                    i_this->mStickJointNo = 0x13;
                } else if (i_this->mStickJointNo < 0) {
                    i_this->mStickJointNo = 1;
                }

                i_this->mpStickJointMtx = daPy_getModelJointMtx(link, (u16)pl_harituki_joint_dt[i_this->mStickJointNo]);
            }
        }

        // HD: the morth follows the player's movement; no status0 0x800000 / status1 0x10 test
        cXyz move;
        cXyz_mi(&player->current.pos, &move, &player->old.pos);
        PSVECAdd(&actor->current.pos, &move, &actor->current.pos);

        cLib_addCalc2(&actor->current.pos.x, x, 1.0f, lx);
        cLib_addCalc2(&actor->current.pos.y, y, 1.0f, ly);
        cLib_addCalc2(&actor->current.pos.z, z, 1.0f, lz);

        u8 cut = ksPlayer(link)->mCutType; /* getCutType() */
        if ((ksPlayer(link)->mStateFlags & 0x2000) /* checkFrontRollCrash() */ ||
            cut == 8 /* CUT_TURN */ || (ksPlayer(link)->mCutType == 9) /* CUT_ROLL */ ||
            dComIfGp_checkPlayerStatus0(0, daPyStts0_SWIM_e)) {
            i_this->mSph.mObjCo.mSPrm |= 1u; /* OnCoSetBit */
            i_this->mSph.mObjTg.mSPrm |= 1u; /* OnTgSetBit */

            if (i_this->mFirstStuck) {
                // HD: no link->offHeavyState()
                HEAVY_IN = FALSE;
                KUTTUKU_ALL_COUNT = 0;
                GORON_COUNT = 0;
                i_this->m2F0[0] = 0;
                i_this->m2F0[1] = 0;
            }

            i_this->mMode = 42;
            return;
        }

        if (!daPy_checkFrontRoll(link)) {
            if (i_this->mFirstStuck == 0) {
                return;
            }

            if (i_this->m2F0[1]) {
                i_this->m2F0[1] = 0;
                i_this->mTimers[2] = 0x32;
                return;
            }

            if (i_this->mTimers[2] != 1) {
                return;
            }

            if (i_this->m2F0[0] <= 0) {
                return;
            }

            i_this->m2F0[0] = 0;
            return;
        }

        if (i_this->mFirstStuck) {
            if (i_this->m2F0[1]) {
                return;
            }

            switch (i_this->m2F0[0]) {
            case 0:
                GORON_COUNT = gabi::ftoi((f32)(s32)KUTTUKU_ALL_COUNT * 0.25f);
                break;
            case 1:
                GORON_COUNT = gabi::ftoi((f32)(s32)KUTTUKU_ALL_COUNT * 0.5f);
                break;
            case 2:
                GORON_COUNT = KUTTUKU_ALL_COUNT;
                break;
            }

            if (GORON_COUNT == 0) {
                GORON_COUNT = 1;
            }
            if (t60c::first()) { i_this->m2F0[0] += 1; } /* true60 (t60conv): counted on the frame's first step */
            i_this->m2F0[1] = 1;

            if (i_this->m2F0[0] < 3 && KUTTUKU_ALL_COUNT >= 3) {
                return;
            }

            i_this->mFirstStuck = 0;

            // HD: no link->offHeavyState()
            HEAVY_IN = FALSE;

            i_this->m2F0[0] = 0;
            i_this->m2F0[1] = 0;

            if (KUTTUKU_ALL_COUNT > 0) {
                KUTTUKU_ALL_COUNT = KUTTUKU_ALL_COUNT - 1;
            }

            GORON_COUNT = KUTTUKU_ALL_COUNT;

            i_this->mMode = 42;
            return;
        }

        if (KUTTUKU_ALL_COUNT != 0 && GORON_COUNT > 0) {
            GORON_COUNT = GORON_COUNT - 1;
            KUTTUKU_ALL_COUNT = KUTTUKU_ALL_COUNT - 1;

            if (HEAVY_IN) {
                if (KUTTUKU_ALL_COUNT < 5) {
                    HEAVY_IN = FALSE;
                    // HD: no link->offHeavyState()
                }
            }

            i_this->mMode = 42;
        }
        break;
    }
    case 42: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        if (i_this->mpStickJointMtx) {
            actor->actor_status &= ~0x4000u;
            fopAcM_setRoomLayer(actor, actor->current.roomNo);
            i_this->mpStickJointMtx = 0;
        }

        actor->current.angle.y = fopAcM_searchPlayerAngleY(actor) + 0x8000;
        actor->current.angle.y += (s16)gabi::ftoi(cM_rndFX(0x4000));

        actor->speedF = 15.0f;
        actor->gravity = -3.0f;
        actor->speed.y = 26.0f;

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        // Fall-through
    }
    case 43: {
        /* true60: the drop's landing takes the frame's landing of the attack hop (site 2): the attack's hop on the frame's
         * later steps would take the same ground answer again; the original hops on the next frame */
        if (ks_t60_land(i_this, 2, i_this->mAcch.ChkGroundHit() || i_this->mAcch.ChkWaterIn())) {
            i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(40.0f) + 40.0f);

            i_this->mAction = KS_ATTACK;
            i_this->mSph.mObjCo.mSPrm |= 1u; /* OnCoSetBit */
            i_this->mSph.mObjTg.mSPrm |= 1u; /* OnTgSetBit */
            i_this->mMode = 12;
        }
        break;
    }
    }

    if (t60c::first() && (i_this->mMode == 43 && body_atari_check(i_this) && i_this->mFirstStuck)) { /* true60 (t60conv): a decision on the first-step timer, on the first step */
        fopAcM_seStart(actor, JA_SE_LK_LAST_HIT, 0);
        i_this->mFirstStuck = 0;
    }
}

/* inlined into daKS_Execute: action_tubo_search */
static inline void action_tubo_search(ks_class* i_this) {
    /* true60 (t60conv, the one-sweep-per-frame attack rule): frame-level logic with an AT registration on the tick30 step; (hand) the earlier
     * steps keep the pot Morth on its pot (mode 51's position copy, before the execute's water / ground code as in A) */
    if (!t60c::end()) {
        if (i_this->mMode == 51) {
            fopAc_ac_c* ksActor = fopAcM_SearchByID(i_this->mKsID);
            if (ksActor)
                i_this->current.pos.copy(ksActor->current.pos);
        }
        return;
    }
    fopAc_ac_c* actor = i_this;
    switch (i_this->mMode) {
    case 50: {
        i_this->mKsID = fpcM_ERROR_PROCESS_ID_e;

        fpcM_Search(KS_POT_SEARCH_CALLBACK, actor);

        if (i_this->mKsID == fpcM_ERROR_PROCESS_ID_e) {
            fopAcM_delete(actor);
        } else {
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    }
    case 51: {
        fopAc_ac_c* ksActor = fopAcM_SearchByID(i_this->mKsID);

        if (ksActor) {
            actor->current.pos.copy(ksActor->current.pos);

            if (i_this->mSearchRange == 0.0f) {
                return;
            }

            if (fopAcM_searchPlayerDistance(actor) > i_this->mSearchRange) {
                return;
            }
        }

        i_this->mSphRadius = 100.0f;

        u32 sprm = i_this->mSph.mObjAt.mSPrm;
        sprm &= ~0x10u; /* OffAtNoTgHitInfSet */
        sprm |= 8;      /* OnAtVsBitSet(cCcD_AtSPrm_VsOther_e) */
        sprm &= ~0x14u; /* OffAtVsBitSet(VsPlayer), OffAtSPrmBit(NoTgHitInfSet) */
        sprm |= 1;      /* OnAtSPrmBit(cCcD_AtSPrm_Set_e) */
        i_this->mSph.mObjAt.mSPrm = sprm;
        i_this->mSph.mObjAt.mRPrm = 1; /* OnAtHitBit (HD: the word is set) */

        i_this->mSph.SetC(&actor->current.pos);
        i_this->mSph.SetR(i_this->mSphRadius);
        dComIfG_Ccsp_Set(&i_this->mSph);

        i_this->mMode = i_this->mMode + 1;
        break;
    }
    case 52: {
        i_this->mSph.SetC(&actor->current.pos);
        i_this->mSph.SetR(i_this->mSphRadius);
        dComIfG_Ccsp_Set(&i_this->mSph);

        if (i_this->mBirthNum > 0x15 || i_this->mBirthNum == 0) {
            i_this->mBirthNum = 1;
        }

        for (int i = 0; i < i_this->mBirthNum; i++) {
            cXyz local_24;
            local_24.copy(actor->current.pos);
            local_24.y += cM_rndF(40.0f);

            if (i != 0) {
                local_24.x += cM_rndFX(40.0f);
                local_24.z += cM_rndFX(40.0f);
            }

            /* actor->shape_angle = actor->current.angle; actor->shape_angle.x = 0; */
            actor->shape_angle.x = 0;
            actor->shape_angle.y = actor->current.angle.y;
            actor->shape_angle.z = actor->current.angle.z;

            fopAcM_create(fpcNm_KS_e, 3, &local_24, fopAcM_GetRoomNo(actor), &actor->shape_angle, &actor->scale, 0, 0);
        }

        fopAcM_delete(actor);
        break;
    }
    }
}

/* inlined into daKS_Execute: action_kb_birth_check */
static inline void action_kb_birth_check(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch (i_this->mMode) {
    case 60: { if (!t60c::first()) break; /* true60 (first-step AI-timer): a state's entry block runs on the frame's first step; a state entered on a later step starts next frame (the one-state-step hold) */
        actor->current.pos.y += REG8_F(13) + 30.0f;
        actor->current.angle.y = (s16)gabi::ftoi(cM_rndFX(0x7FFF));

        f32 a = REG8_F(8) + 4.0f;
        actor->speedF = a + cM_rndF(REG8_F(9) + 4.0f);
        a = REG8_F(10) + 20.0f;
        actor->speed.y = a + cM_rndF(REG8_F(11) + 5.0f);
        actor->gravity = -(REG8_F(12) + 2.0f);

        if (t60c::first()) { i_this->mMode = i_this->mMode + 1; } /* true60 (t60conv): counted on the frame's first step */
        // Fall-through
    }
    case 61: {
        if (t60c::first() && !(actor->speed.y > 0.0f) && /* true60 (first-step AI-timer): the birth's landing is a state change on the frame's speed: first step */
            (i_this->mAcch.ChkGroundHit() || i_this->mAcch.GetGroundH() + (REG8_F(19) + 10.0f) > actor->current.pos.y)) {
            actor->actor_status &= ~0x4000u;

            i_this->mAction = KS_MOVE;
            actor->speedF = 0.0f;
            actor->gravity = 0.0f;
            actor->speed.x = 0.0f;
            actor->speed.y = 0.0f;
            actor->speed.z = 0.0f;
            i_this->mMode = 0;
        }
        break;
    }
    }
}

/* 021A7F94 */
static BOOL daKS_Execute(ks_class* i_this) {
    /* true60 (the whole-frame form rule addendum): the whole frame on the tick30 step
     * (d_a_ks_exec_t60.h; end at every return) */
    t60w::Guard<ks_class> t60g(i_this, ks_t60w::ops());
    if (t60g.skip) /* a step before the tick30 step: the picture only (frame interpolation) */
        return TRUE;
    t60g.model = true; /* the matrix steps (draw_SUB / the ice pose) run in every tick30 run */

    fopAc_ac_c* actor = i_this;
    ks_t60_pic_begin(i_this); /* true60: the picture share's step count (host table) */

    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel_setBaseTRMtx(i_this->mpBodyMorf->getModel(), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(i_this->mpEyeMorf->getModel(), mDoMtx_stack_c::get());
        if (i_this->mFrozen) {
            if (t60c::first()) { i_this->mBrkFrame = i_this->mBrkFrame + 1.0f; } /* true60: the freeze frame counted once per 30 Hz frame */
            if (i_this->mBrkFrame > 7.0f) {
                i_this->mBrkFrame = 7.0f;
            }
        }
        return TRUE;
    }

    for (int i = 0; i < 4; i++) {
        if (i_this->mTimers[i]) {
            if (t60c::first()) { i_this->mTimers[i] -= 1; } /* true60 (t60conv): counted on the frame's first step */
        }
    }

    if (t60c::first() && i_this->mGmID != 0 && i_this->mAction != KS_DEAD) { /* true60 (first-step AI-timer): the Mothula check is a decision on another actor's state, once per frame on the first step */
        fopAc_ac_c* gmActor = fopAcM_SearchByID(i_this->mGmID);

        if (gmActor && ((fopAcM_GetParam(gmActor) & 0xff0000) == 0xff0000 || (fopAcM_GetParam(gmActor) & 0xff0000) == 0)) {
            bool bVar5;
            if (gmActor && fpcM_GetName(gmActor) == fpcNm_GM_e) {
                bVar5 = !(gmActor->health > 0);
            } else {
                bVar5 = true;
            }

            if (bVar5) {
                if (i_this->mAction != KS_ATTACHED) {
                    i_this->mAction = KS_DEAD;
                    i_this->mMode = 30;
                } else if (i_this->mMode != 43) {
                    dComIfGp_get(); /* daPy_getPlayerLinkActorClass()->offHeavyState(): HD, no effect */

                    GORON_COUNT = 0;
                    HEAVY_IN = FALSE;

                    i_this->m2F0[0] = 0;
                    i_this->m2F0[1] = 0;

                    i_this->mMode = 42;
                }
            }
        }
    }

    /* true60 (the one-state-step hold, bk_t60_ai_held / pt): a state (action, mode) entered on an earlier step of this
     * frame first runs on the next frame's first step (the original runs it next frame; a fall-through inside one step
     * is unchanged); the motion below still runs on every step */
    if (t60c::first() && t60c::on()) {
        t60c::unmark(KS_T60_C2_DAMP, i_this); t60c::unmark(KS_T60_C2_RUN, i_this);
        t60c::unmark(KS_T60_CATCH, i_this); t60c::unmark(KS_T60_CATCH_HOP, i_this);
    }
    /* true60 (first-step AI-timer catch-up addendum): the frame-start position and ground answer, for a decision case 2 takes late */
    ks_t60_pos_begin(i_this, actor->current.pos);
    ks_t60_cc_begin(i_this, i_this->mStts.m_cc_move); /* true60 (first-step push / process-order read addendum): the frame's Co push */
    (void)t60c::held(i_this->mAcch.ChkGroundHit() || i_this->mAcch.ChkWaterIn(), KS_T60_G0, i_this);
    (void)t60c::held(i_this->mAcch.ChkGroundHit(), KS_T60_AIM_G0, i_this); /* true60: the frame-start answer for the tick30 heading aim */
    (void)t60c::heldInt((s16)i_this->mMoveAngleOffset, KS_T60_AIM_OFS, i_this); /* true60: and its frame-start offset */
    const s32 ksState = (s32)((u32)(u8)i_this->mAction | ((u32)(u8)i_this->mMode << 8));
    const bool ksHeld = t60c::heldInt(ksState, 3, i_this) != ksState;
    if (!ksHeld)
    switch (i_this->mAction) {
    case KS_MOVE:
        action_dousa_move(i_this);
        ks_kuttuki_check(i_this);
        break;
    case KS_ATTACK:
        action_kougeki_move(i_this);
        break;
    case KS_WIND:
        action_kaze_move(i_this);
        break;
    case KS_DEAD:
        action_dead_move(i_this);
        break;
    case KS_ATTACHED:
        action_omoi(i_this);
        break;
    case KS_POT_SEARCH:
        action_tubo_search(i_this);
        break;
    case KS_BIRTH:
        action_kb_birth_check(i_this);
        break;
    }

    if (ksHeld && t60c::end()) ks_t60_c2_deferred(i_this); /* true60: a case-2 frame left on an earlier step: its draws */

    if (i_this->mType == 6) {
        return TRUE;
    }

    if (t60c::first() && i_this->mEyeBtkFrame) { /* true60: the blink frame steps once per 30 Hz frame, on the first step (A's order: before the blink timer) */
        s16 f = (s16)(i_this->mEyeBtkFrame + 1);
        if (f > 7) {
            i_this->mEyeBtkFrame = 0;
        } else {
            i_this->mEyeBtkFrame = f;
        }
    }

    if (t60c::first() && (i_this->mTimers[0] == 0)) { /* true60 (t60conv): a decision on the first-step timer, on the first step */
        i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndFX(25.0f) + 50.0f);
        i_this->mEyeBtkFrame = 1;
    }

    cMtx_YrotS(calc_mtx(), actor->current.angle.y);

    cXyz local_18;
    local_18.x = 0.0f;
    local_18.y = 0.0f;
    local_18.z = actor->speedF;

    cXyz local_c;
    MtxPosition(&local_18, &local_c);

    actor->speed.x = local_c.x;
    actor->speed.z = local_c.z;

    /* true60 (the per-step movement rule): the airborne answer is the frame's (first step, held): the original skips gravity for the whole
     * frame a hop starts on (the ground answer of the previous frame's BG check), so the hop frame's steps move by the
     * launch speed only and a mid-frame landing keeps the frame's gravity */
    const bool ksAir = t60c::held(!i_this->mAcch.ChkGroundHit() && !i_this->mAcch.ChkWaterIn(), 4, i_this);
    f32 ksArc = 0.0f;
    if (i_this->mMode != 41 && ksAir) {
        /* true60 (the per-step movement rule): gravity x dt plus the exact-arc term of fopAcM_posMove for this inline integration:
         * N steps end where the original frame ends */
        const f32 vy0 = actor->speed.y;
        actor->speed.y = actor->speed.y + t60c::step(actor->gravity);

        if (actor->speed.y < -20.0f) {
            actor->speed.y = -20.0f;
        }
        ksArc = ks_t60_arc(actor->speed.y - vy0);
    }

    actor->eyePos.copy(actor->current.pos);
    actor->attention_info.position.copy(actor->current.pos); /* attention_info.position = eyePos */

    /* true60 (the Co push as a process-order read addendum case): the tick30 collision Move is A's frame-end
     * Move, whose sphere A registered at the frame-start position (this SetC runs before the frame's posMove): on the
     * tick30 step the sphere goes in where A's is (frame start + the frame's direct shifts, without the earlier steps'
     * posMoves: ks_t60_cc_at), not at the step's mid-frame position (pair f204: the push from the
     * half-frame-deeper overlap was twice A's). Earlier steps: the step's position */
    gabi::Local<cXyz> ksCc;
    if (ks_t60_cc_at(i_this, ksCc.get(), actor->current.pos)) i_this->mSph.SetC(ksCc.get());
    else i_this->mSph.SetC(&actor->current.pos);
    i_this->mSph.SetR(i_this->mSphRadius);
    dComIfG_Ccsp_Set(&i_this->mSph);

    /* true60: the exact-arc term rides on posMove's y step (speed.y + arc / dt for the move only, the stored speed.y put
     * back): none at dt 1 */
    const f32 ksVy = actor->speed.y;
    if (ksArc != 0.0f) actor->speed.y = gabi::fadds_ppc(ksVy, ks_t60_arc_speed(ksArc));
    /* true60 (first-step AI-timer catch-up addendum): the tick30 step that decided a motion late moves the whole frame's amount from the
     * frame-start position (speed / dt for this move only, put back after) */
    const bool ksCatchMove = t60c::end() && t60c::marked(KS_T60_CATCH, i_this);
    const bool ksCatchY = ksCatchMove && t60c::marked(KS_T60_CATCH_HOP, i_this);
    const f32 ksCx = actor->speed.x, ksCz = actor->speed.z, ksCy = actor->speed.y;
    if (ksCatchMove) {
        const f32 inv = 1.0f / t60::dt();
        actor->speed.x = gabi::fmuls_ppc(ksCx, inv);
        actor->speed.z = gabi::fmuls_ppc(ksCz, inv);
        if (ksCatchY) actor->speed.y = gabi::fmuls_ppc(ksCy, inv);
    }
    ks_t60_mv_begin(i_this); /* true60: the frame's posMove sum (tick30 sphere) */
    const f32 ksMx = actor->current.pos.x, ksMy = actor->current.pos.y, ksMz = actor->current.pos.z;
    if ((i_this->mSph.mObjCo.mSPrm & 1) /* ChkCoSet */ && (i_this->mAction == KS_MOVE || i_this->mAction == KS_WIND)) {
        ks_t60_cc_step(i_this, &i_this->mStts.m_cc_move, ksCatchMove, ksCatchY); /* true60: the frame's push */
        fopAcM_posMove(actor, &i_this->mStts.m_cc_move);
        /* true60 (first-step AI-timer catch-up addendum): the catch step's push / dt was for this move only: the stored push is A's */
        if (ksCatchMove) ks_t60_cc_step(i_this, &i_this->mStts.m_cc_move, false, false);
    } else {
        fopAcM_posMove(actor, nullptr);
    }
    ks_t60_mv_end(i_this, ksMx, ksMy, ksMz, actor->current.pos);
    if (ksCatchMove) {
        actor->speed.x = ksCx;
        actor->speed.z = ksCz;
        if (ksCatchY) actor->speed.y = ksCy;
    }
    if (ksArc != 0.0f) actor->speed.y = ksVy;

    if (i_this->mMode != 41) {
        BG_check(i_this);

        /* naraku_check (inlined). HD: falling out of the map is detected by a missing ground
         * (GetGroundH() == -G_CM3D_F_INF) for 50 frames; no ground-code / y < -500 test */
        if (i_this->mAcch.GetGroundH() == -1000000000.0f) {
            if (t60c::first()) { i_this->mNoGroundCount = i_this->mNoGroundCount + 1; } /* true60 (t60conv): counted on the frame's first step */
            if (i_this->mNoGroundCount > 0x32) {
                fopAcM_delete(actor);
                draw_SUB(i_this);
                i_this->mDrawSubDone = 1;
                return TRUE;
            }
        } else {
            i_this->mNoGroundCount = 0;
        }

        f32 wtrH = i_this->mAcch.m_wtr.GetHeight(); /* m_wtr.GetHeight() */
        if (i_this->mAcch.ChkWaterIn() && actor->current.pos.y < wtrH + 20.0f) {
            if (!i_this->mRippleOn) {
                i_this->mRippleOn = 1;

                cXyz local_18b;
                local_18b.x = 0.5f;
                local_18b.z = 0.5f;
                local_18b.y = 0.5f;

                ripple_remove(i_this->mRipple);
                /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &current.pos, NULL, &scale, 0xFF, &mRipple) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &actor->current.pos, nullptr, &local_18b, 0xFF,
                                reinterpret_cast<dPa_levelEcallBack*>(i_this->mRipple), -1, nullptr, nullptr, nullptr);
                ksRipple(i_this->mRipple)->mRate = 0.0f; /* mRipple.setRate(0.0f) */
            }

            cLib_addCalc2(&actor->current.pos.y, i_this->mAcch.m_wtr.GetHeight() + 20.0f, 1.0f, 10.0f);

            actor->gravity = 0.0f;
            actor->speed.y = 0.0f;
        } else if (i_this->mRippleOn && (i_this->mAcch.ChkGroundHit() || actor->current.pos.y > wtrH + 100.0f)) {
            i_this->mRippleOn = 0;
            ripple_remove(i_this->mRipple);
        }
    }

    {
        /* true60 (picture only, d_a_ks_exec_t60.h): draw the spins / tumble / stuck turn with their per-step share from
         * a local copy; the stored values are put back (none of this at dt 1 or on the tick30 step) */
        const s16 zLag = ks_t60_pic_lag(i_this, ks_t60_pic_sum(i_this, KS_T60_PIC_Z));
        const s16 yLag = ks_t60_pic_lag(i_this, ks_t60_pic_sum(i_this, KS_T60_PIC_Y));
        const s16 spinLag = ks_t60_pic_lag(i_this, (s16)i_this->mSpinSpeed);
        if (zLag != 0 || yLag != 0 || spinLag != 0) {
            const s16 z0 = actor->shape_angle.z, y0 = actor->shape_angle.y, spin0 = i_this->mSpinAngle;
            actor->shape_angle.z = (s16)(z0 - zLag);
            actor->shape_angle.y = (s16)(y0 - yLag);
            i_this->mSpinAngle = (s16)(spin0 - spinLag);
            draw_SUB(i_this);
            actor->shape_angle.z = z0;
            actor->shape_angle.y = y0;
            i_this->mSpinAngle = spin0;
        } else {
            draw_SUB(i_this);
        }
    }

    i_this->mDrawSubDone = 1;

    return TRUE;
}

/* WWHD glue
ext 020402C8 BOOL ksExecute_ice(enemyice_l* ice)
ext 025A9270 void ksExecute_rippleEnd(void* ripple)
ext 025D5418 void ksExecute_roomLayer(fopAc_ac_c* actor, s32 room)
ext 025E1AA4 void ksExecute_monsterSound(u32 sound, cXyz* position, u32 id, u32 param, s32 reverb)
*/
