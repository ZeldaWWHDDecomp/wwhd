/* d_a_rd_t60.h (WWHD, true 60 build; 2026-11-19): the ReDead's step helpers (d_a_rd.cpp;
 * dt = 1/N). Decomp build only.
 *
 * A converted ReDead (daRd_c, PROC_RD) executes on every step with dt = 1/N; unconverted (and at N = 1) it executes on
 * the tick30 step with dt = 1, where every helper here returns the original's answer, so the dt = 1 path is the original
 * code. The ReDead's frame (A): the pre-dispatch decisions (corpse search, the attack event, the fire death), the head
 * eases, lookBack, the collider set (frame-START position), posMoveF + CrrPos, the plays, THEN the mode dispatch: the
 * dispatch reads the frame-END state (position, animation, Link, the BG check). The forms:
 *   first step   the AI timers (the first-step AI-timer rule: mTimer1 / mTimer2 decisions, mFireDeathTimer, mHitReactTimer,
 *                mTgHitWait, mLookPlayerTimer), the hit read checkTgHit (mixed-pair hit-flag; its answer held over the frame), the
 *                break-free stick / button mash (first-step input: the frame's input), the event-state decisions (first-step frame-level: the event
 *                system ran on the frame's first step; isLinkControl, the attack event order once per frame), the
 *                pre-dispatch decisions, the facing copy current.angle = shape_angle (A moves the whole frame with
 *                the frame-start facing), the mode / animation, facing and eye lookBack starts from (rd_t60_look_note), and the
 *                collider's shape and centre (A sets it from the frame-start position before the move).
 *   tick30 step  the decisions that read Link (notice, fan / circle tests with Link, distance, life; first-step input addendum),
 *                lookBack whole at dt 1 (it reads Link's head top: process-order read; head joints and the body turn of
 *                dNpc_JntCtrl_c, d_npc.cpp is not stepped: NPC form, once per frame, before the move),
 *                the animation-end decisions and the animation choice of a mode (the dispatch runs after the play: the
 *                frame-end animation state, the animation-driven decision rule Link form), the arrival tests (turn reached), the writes into
 *                Link (the grab's damage, the wall push: process-order read addendum (b), Link is earlier in the line), the collider
 *                registration Ccsp_Set (one-sweep-per-frame attack), the end-of-execute animation chain setAnm(NULL), the lighting.
 *   every step   the motion (posMoveF with the held push first-step push, CrrPos, the speed / facing / position eases), the plays,
 *                the matrices, the attention / eye positions.
 * One AI state step per 30 Hz frame: a mode changed inside the dispatch on an earlier step holds the dispatch on the
 * frame's remaining steps (A's call returned after the change). An animation (bck / btk / brk) set on an earlier step
 * starts with the next frame's play (A plays before its dispatch: a set in the dispatch first plays next frame). A
 * self-delete decided on an earlier step takes effect after the tick30 step (the first-step AI-timer rule addendum 2; the ReDead's only
 * delete is on the tick30 step already). */
#pragma once
#include "true60.h"
#include "d/actor/d_a_bk_t60.h"
#include "d/actor/d_a_rd.h"

/* the frame's first / tick30 step (every step at dt = 1). Negative control (RD_T60_NEGCTRL, with T60C_NEGCTRL): every
 * site in the per-step form (decide, count, read and register on every step, nothing held). */
#ifdef RD_T60_NEGCTRL
static inline bool rd_t60_first() { return true; }
static inline bool rd_t60_end() { return true; }
#else
static inline bool rd_t60_first() { return bk_t60_first(); }
static inline bool rd_t60_end() { return bk_t60_end(); }
#endif
/* a decision that reads Link, an animation end, an arrival: on the tick30 step (every step at dt = 1) */
static inline bool rd_t60_link() { return rd_t60_end(); }

/* setAttention (the process-order read rule, pair): eyePos and attention_info.position come from the
 * head joint of the last calc. A's frame-end values are the previous frame's pose; per step, B's frame end held the
 * pose of the frame's first step (half a frame on). Link runs before the ReDead and reads them on his next step
 * (checkAttentionPosAngle: the 0x6000 look cone, frame 798: B 24561 / A 24619), so Link looked at it and set
 * daPyFlg1_UNK400 one frame early; the head top the ReDead's lookBack reads followed (mHeadAngle 817-820). Written on the
 * frame's first step (from the frame-start pose, = A's frame end: B's first-step record = A's), held on the later steps;
 * the freeze-status bit stays per step (mFreezeTimer). dt 1: every step writes (the original). */
static inline bool rd_t60_attn_held() { return t60::dt() != 1.0f && !rd_t60_first(); }
static inline void rd_t60_attn_status(daRd_c* i) {
    if (i->mEnemyIce.mFreezeTimer > 20) {
        i->actor_status &= ~0x00200000u /* fopAcStts_UNK200000_e (gc_api f_op_actor_mng.h is included after this header) */;
    } else {
        i->actor_status |= 0x00200000u; /* fopAcStts_UNK200000_e */
    }
}

/* the hold: a mode changed inside the dispatch on an earlier step (latch keyed by mMode) */
static inline void rd_t60_note(const daRd_c* i, s32 before) {
    if (t60::dt() == 1.0f || t60::tick30() || (s32)i->mMode == before)
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mMode), bk_t60_frame_tag());
}
static inline bool rd_t60_held(const daRd_c* i) {
#ifdef RD_T60_NEGCTRL
    return false;
#endif
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(&i->mMode), bk_t60_frame_tag()) != 0;
}

/* a first-step answer held over the frame's later steps (two keys: "answered this frame" and "true"; a latch query with
 * another frame value drops the entry, so no tag / ~tag pair on one key). A frame whose first answer falls on a later
 * step answers there. dt = 1: the live answer. */
static inline bool rd_t60_has(u32 key) { return wwhd_t60_latch_pending(key + 1, bk_t60_frame_tag()) != 0; }
static inline bool rd_t60_get(u32 key) { return wwhd_t60_latch_pending(key, bk_t60_frame_tag()) != 0; }
static inline bool rd_t60_put(u32 key, bool v) {
    if (t60::dt() != 1.0f) {
        const u32 tag = bk_t60_frame_tag();
        wwhd_t60_latch_set(key + 1, tag);
        wwhd_t60_latch_set(key, v ? tag : ~tag);
    }
    return v;
}
/* keys inside the ReDead (distinct guest words, high bit set: not a field address) */
static inline u32 rd_t60_key(const void* field, u32 site) { return (gabi::ea(field) ^ 0x40000000u) + site * 2u; }

/* checkTgHit's answer (the mixed-pair hit-flag rule): the hits of the tick30 collision Move read on the frame's first step (with its
 * mTgHitWait count, mStts.Move and the light detect), the answer held on the later steps (A's one call: a mode body
 * that returned on a hit did nothing else that frame). */
static inline bool rd_t60_hit_has(const daRd_c* i) {
#ifdef RD_T60_NEGCTRL
    return false;
#endif
    return t60::dt() != 1.0f && !rd_t60_first() && rd_t60_has(rd_t60_key(&i->mTgHitWait, 0));
}

/* an animation set on an earlier step first plays next frame (the ReDead plays before its dispatch): a latch keyed by
 * the animation object (morf: &mpMorf, as d_a_bk_t60.h bk_t60_pending; btk: &mBtkAnm; brk: &mBrkAnm). A set on the
 * tick30 step plays from the next frame anyway. The morf set also ends the frame's held stop answer. */
static inline void rd_t60_mark(const void* anm) {
#ifndef RD_T60_NEGCTRL
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(anm), bk_t60_frame_tag());
#else
    (void)anm;
#endif
}
static inline void rd_t60_mark_morf(daRd_c* i) {
    if (t60::dt() == 1.0f)
        return;
    en_t60_stop_reset(gabi::load<u32>(gabi::ea(&i->mpMorf)));
    rd_t60_mark(&i->mpMorf);
}
static inline bool rd_t60_play(const void* anm) { return !bk_t60_pending(anm); }

/* a whole-frame call at dt = 1 (lookBack: dNpc_JntCtrl_c is not stepped; a write into Link: the frame's amount) */
template <class F> static inline void rd_t60_whole(F f) {
    if (t60::dt() == 1.0f) {
        f();
        return;
    }
    const f32 previous = t60_set_dt(1.0f);
    f();
    t60_set_dt(previous);
}

/* lookBack (the process-order read rule: Link executes before the ReDead, so its read of Link's head top is on the tick30 step, Link's
 * frame-end state as in A; rdP4_2 frame 328: on the first step it read Link half a frame early and the head / backbone
 * joint angles went the other way, mHeadAngle 7363 vs A's 8671). It runs whole (dt 1) once per frame on the tick30 step,
 * with the ReDead's own branch inputs of A's place (whether it is due, the mode and the animation after the first step's
 * pre-dispatch decisions, before the dispatch): noted on the first step (rd_t60_look_note), read on the tick30 step. A
 * frame without a first-step note (created on a later step) uses the live values. Plain host values, per instance.
 * dt = 1: nothing noted, the live values (the original). */
struct RdT60Look {
    u32 ea, tag;
    s32 mode, anm;
    bool due;
    s16 shapeY; /* shape_angle.y at A's place (lookAtTarget's base and turned angle) */
    Vec3f eye;  /* mRdEyePos at A's place (the previous frame's calc; the tick30 step sees the first step's) */
    s16 calcY;  /* shape_angle.y of A's setMtx / calc (the frame-start facing + lookBack's turn): rd_t60_head_angle */
};
static RdT60Look s_rdT60Look[32];
static inline RdT60Look* rd_t60_look_entry(const daRd_c* i, bool make) {
    const u32 ea = gabi::ea(i), tag = bk_t60_frame_tag();
    for (u32 k = 0; k < 32; k++) {
        RdT60Look& s = s_rdT60Look[(ea / 16 + k) & 31];
        if (s.ea == ea && s.tag == tag)
            return &s;
    }
    if (!make)
        return nullptr;
    RdT60Look* e = &s_rdT60Look[(ea / 16) & 31];
    for (u32 k = 0; k < 32; k++) {
        RdT60Look& s = s_rdT60Look[(ea / 16 + k) & 31];
        if (s.ea == 0 || s.tag != tag) {
            e = &s;
            break;
        }
    }
    e->ea = ea;
    e->tag = tag;
    return e;
}
static inline void rd_t60_look_note(const daRd_c* i, bool due) {
    if (t60::dt() == 1.0f)
        return;
    RdT60Look* e = rd_t60_look_entry(i, true);
    e->mode = (s32)i->mMode;
    e->anm = (s32)(s8)i->mAnmPrmIdx;
    e->due = due;
    e->shapeY = (s16)i->shape_angle.y;
    e->calcY = e->shapeY;
    e->eye = i->mRdEyePos.get();
}
/* on the tick30 step: lookBack due this frame (A's condition at its place; no note: the live condition) */
static inline bool rd_t60_look_due(const daRd_c* i, bool live) {
    const RdT60Look* e = t60::dt() == 1.0f ? nullptr : rd_t60_look_entry(i, false);
    return e != nullptr ? e->due : live;
}
/* the tick30 step's lookBack call: whole (dt 1), from the noted facing and eye (A's inputs: the first step's dispatch has
 * already eased the facing, its calc moved the eye; rdP5_2 frames 780-784: backbone -306 vs A's -329, then the facing
 * 51 apart for the rest of the run and mHeadAngle 882 apart at 819). The turn lookAtTarget gives the noted facing is
 * added to the live one; the eye goes back to the live value (the next calc writes it). No note: the live values. */
template <class F> static inline void rd_t60_look_run(daRd_c* i, F f) {
    RdT60Look* e = t60::dt() == 1.0f ? nullptr : rd_t60_look_entry(i, false);
    if (e == nullptr) {
        rd_t60_whole(f);
        return;
    }
    const s16 live = (s16)i->shape_angle.y;
    const Vec3f liveEye = i->mRdEyePos.get();
    i->shape_angle.y = e->shapeY;
    i->mRdEyePos = e->eye;
    rd_t60_whole(f);
    const s16 turn = (s16)((s16)i->shape_angle.y - e->shapeY);
    e->calcY = (s16)(e->shapeY + turn);
    i->shape_angle.y = (s16)(live + turn);
    i->mRdEyePos = liveEye;
}
/* mHeadAngle on the tick30 step (the process-order read rule, pair frame 855: A 10376 / B 11117, the
 * Link-only control exact): the head joint's world yaw from the last calc, read by the tick30 dispatch
 * (checkPlayerInCry's fan, modeMove's walk test). A's calc runs before its dispatch, so its base matrix has the
 * frame-start facing plus lookBack's turn (noted: calcY); B's tick30 calc has the facing the earlier steps' dispatch
 * already turned (frame 855: the grab's turn to Link, shape 43 after step 0). When the two differ, the tick30 step
 * calcs once more at A's facing (setMtx + calc: f) and keeps that calc's mHeadAngle, then calcs again at the live
 * facing, so the picture (the joint matrices, mHeadMtx, mRdEyePos, mTargetPos) keeps the live facing. A whole calc, not
 * an added angle: cM_atan2s is a table lookup, so the yaw of a turned matrix is not the old yaw plus the turn (N-step spec
 * _t60/d_a_rd: 16 of about 900 inputs a few units off). The calc is a function of the model state (the frame, the
 * joint controls), so the repeat changes nothing else. No note (created on a later step) or dt 1: unchanged (the
 * original). */
template <class F> static inline void rd_t60_head_angle(daRd_c* i, F f) {
    if (t60::dt() == 1.0f || !rd_t60_end())
        return;
    const RdT60Look* e = rd_t60_look_entry(i, false);
    if (e == nullptr || e->calcY == (s16)i->shape_angle.y)
        return;
    const s16 live = (s16)i->shape_angle.y;
    i->shape_angle.y = e->calcY;
    f();
    const s16 headAngle = (s16)i->mHeadAngle;
    i->shape_angle.y = live;
    f();
    i->mHeadAngle = headAngle;
}
static inline s32 rd_t60_look_mode(const daRd_c* i) {
    const RdT60Look* e = t60::dt() == 1.0f ? nullptr : rd_t60_look_entry(i, false);
    return e != nullptr ? e->mode : (s32)i->mMode;
}
static inline s32 rd_t60_look_anm(const daRd_c* i) {
    const RdT60Look* e = t60::dt() == 1.0f ? nullptr : rd_t60_look_entry(i, false);
    return e != nullptr ? e->anm : (s32)(s8)i->mAnmPrmIdx;
}

/* the first-step push rule: the push m_cc_move of the tick30 collision Move, held from the value the frame's first step sees (or
 * the first step that sees one after a frame without it), dt of it per step (fopAcM_posMove scales by dt); the tick30
 * step applies the rest of the frame's amount, so every 30 Hz frame moves by A's push (d_a_oq_t60.h oq_t60_push_step
 * with the Co condition always on: the ReDead passes m_cc_move to every posMoveF). Per instance (keyed by the actor). */
struct RdT60Push {
    u32 ea, tag;
    f32 held[3];
    f32 applied[3];
};
static RdT60Push s_rdT60Push[32];
static inline void rd_t60_push_step(daRd_c* i, cXyz* out) {
    const cXyz* cc = &i->mStts.m_cc_move;
    const u32 ea = gabi::ea(i), tag = bk_t60_frame_tag();
    RdT60Push* e = nullptr;
    for (u32 k = 0; k < 32 && e == nullptr; k++) {
        RdT60Push& s = s_rdT60Push[(ea / 16 + k) & 31];
        if (s.ea == ea)
            e = &s;
    }
    const bool known = e != nullptr && e->tag == tag;
    if (e == nullptr) {
        for (u32 k = 0; k < 32 && e == nullptr; k++) {
            RdT60Push& s = s_rdT60Push[(ea / 16 + k) & 31];
            if (s.ea == 0 || s.tag != tag)
                e = &s;
        }
        if (e == nullptr)
            e = &s_rdT60Push[(ea / 16) & 31];
        e->ea = ea;
        e->held[0] = e->held[1] = e->held[2] = 0.0f;
    }
    const f32 x = cc->x, y = cc->y, z = cc->z;
    const bool fresh = x != 0.0f || y != 0.0f || z != 0.0f;
    const bool held = known && (e->held[0] != 0.0f || e->held[1] != 0.0f || e->held[2] != 0.0f);
    if (!known || t60::first_step()) {
        e->applied[0] = e->applied[1] = e->applied[2] = 0.0f;
    }
    if (!known || t60::first_step() || (fresh && !held)) {
        e->tag = tag;
        e->held[0] = x, e->held[1] = y, e->held[2] = z;
    }
    const f32 dt = t60::dt();
    if (!t60::tick30()) {
        out->x = e->held[0];
        out->y = e->held[1];
        out->z = e->held[2];
        for (int k = 0; k < 3; k++)
            e->applied[k] = gabi::fadds_ppc(e->applied[k], gabi::fmuls_ppc(e->held[k], dt));
    } else {
        for (int k = 0; k < 3; k++) {
            const f32 rest = gabi::fsubs_ppc((f32)e->held[k], e->applied[k]);
            (&out->x)[k] = rest / dt;
        }
        e->ea = 0;
    }
}

/* posMoveF with the frame's held push (dt = 1: the original call) */
static inline void rd_t60_posMoveF(daRd_c* i) {
    if (t60::dt() == 1.0f) {
        fopAcM_posMoveF(i, &i->mStts.m_cc_move);
        return;
    }
    gabi::Local<cXyz> push;
    rd_t60_push_step(i, push);
    fopAcM_posMoveF(i, push);
}

/* a frame that began on the coffin-wait path (MODE_SW_WAIT: move, play, dispatch only) keeps that path on its later
 * steps when the first step's switch decision left it (A's frame ran that path only) */
static inline void rd_t60_sw_note(const daRd_c* i, s32 before) {
    if (t60::dt() == 1.0f || t60::tick30() || (s32)i->mMode == before)
        return;
    wwhd_t60_latch_set(rd_t60_key(&i->mMode, 1), bk_t60_frame_tag());
}
static inline bool rd_t60_sw_path(const daRd_c* i) {
#ifdef RD_T60_NEGCTRL
    return false;
#endif
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(rd_t60_key(&i->mMode, 1), bk_t60_frame_tag()) != 0;
}
