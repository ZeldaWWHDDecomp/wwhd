/** ChuChu execution and inlined actions, following tww with verified HD behaviour. */
#include "d/actor/d_a_cc_exec.h"
#include "d/actor/d_a_cc_t60.h"
#include "gc_api/tu.h"
#include <cstdlib>
#include <unordered_map>
const gc_tu_data gc_tu = {.safeString = 0x1000C548};
static const char splitArchive[3] = {0x43, 0x63, 0};
static const char deadArchive[3] = {0x43, 0x63, 0};

/* true60 (the 30 Hz timer rule, the first-step AI-timer rule): the AI timers mTimers[7] count on the frame's FIRST step, before its
 * dispatch, as the original counts at the start of its frame: the decisions they time fall on the first step and the new
 * state moves on every step of the frame (on the tick30 step the new motion started (N-1)/N frame
 * late: cck9s1_2 jump launch 319). A timer the dispatch sets on any step of the frame comes after that count and is first
 * counted next frame, as in the original. Only this unit counts them. */
namespace {
/* the first-step push rule: the collision push m_cc_move of the tick30 collision Move (mixed pairs with the unconverted player are
 * checked there only; the next step's Move clears it), held from the value the frame's first step sees (or the first
 * step that sees one after a frame without it), dt of it per step (fopAcM_posMove scales the push by dt), dropped on the
 * frame's last step: the frame's total stays (c_damagereaction.cpp t60dr_push_step, the same form). With a converted
 * player this holds A's push only because the body Co is registered for the tick30 Move alone
 * (cc_t60_at_mask). */
struct CcT60Push {
    u32 frame;
    f32 held[3];
};
std::unordered_map<u32, CcT60Push> s_ccT60Pushes;
/* the shape-angle chase the frame's dispatch call ran after its action (action_nomal_move 0x400, action_attack_move
 * 0x1000; 0: none), for the frame's held steps (cc_t60_held_chase) */
struct CcT60Chase {
    u32 frame;
    s16 step;
};
std::unordered_map<u32, CcT60Chase> s_ccT60Chases;
/* the body cylinder's At set bit, cleared for the collision Move of an in-between step (cc_t60_at_mask) and set again at
 * the next execute's start (cc_t60_at_restore); key: the actor */
std::unordered_map<u32, u32> s_ccT60AtMasked;
}  // namespace
/* true60 (the first-step AI-timer rule: motion on every step): the action functions end with a per-step shape-angle
 * chase toward the heading (cLib_addCalcAngleS2, its step x dt). When the dispatch call changed the state on an
 * in-between step, the frame's remaining steps hold the dispatch (one AI state step per frame), and with it the chase:
 * the frame turned only its first steps' share (cckR1_2 frame 308: the attack's launch, B's shape_angle.y 0x800 short
 * at N 2). The dispatch call notes the chase it ran; the held steps run it again with their dt share, so the frame
 * turns as at 30 Hz (the original's one call ran the chase once, after the change). dt = 1: never held, nothing noted. */
static inline void cc_t60_note_chase(cc_class* i_this, s16 step) {
    if (t60::dt() == 1.0f)
        return;
    s_ccT60Chases[gabi::ea(i_this)] = CcT60Chase{bk_t60_frame_tag(), step};
}
static inline void cc_t60_held_chase(cc_class* i_this) {
    auto it = s_ccT60Chases.find(gabi::ea(i_this));
    if (it == s_ccT60Chases.end() || it->second.frame != bk_t60_frame_tag() || it->second.step == 0)
        return;
    cLib_addCalcAngleS2(&i_this->actor.shape_angle.y, i_this->actor.current.angle.y, 1, it->second.step);
}
static void cc_t60_push_step(cc_class* i_this, cXyz* out) {
    const cXyz* cc = &i_this->mStts.m_cc_move;
    u32 a = gabi::ea(i_this), frame = bk_t60_frame_tag();
    auto it = s_ccT60Pushes.find(a);
    bool known = it != s_ccT60Pushes.end() && it->second.frame == frame;
    CcT60Push* e = it != s_ccT60Pushes.end() ? &it->second : &s_ccT60Pushes[a];
    f32 x = cc->x, y = cc->y, z = cc->z;
    bool fresh = x != 0.0f || y != 0.0f || z != 0.0f;
    bool held = known && (e->held[0] != 0.0f || e->held[1] != 0.0f || e->held[2] != 0.0f);
    if (!known || t60::first_step() || (fresh && !held)) {
        e->frame = frame;
        e->held[0] = x, e->held[1] = y, e->held[2] = z;
    }
    out->x = e->held[0];
    out->y = e->held[1];
    out->z = e->held[2];
    if (t60::tick30())
        s_ccT60Pushes.erase(a);
}

/* HD fopAcM_monsSeStart inline (the process id is read before the reverb call) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 se, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        CcExec_monsSeStart(se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}
/* mCyl (dCcD_Cyl) bits */
static inline void cyl_OnAtSet(cc_class* i) {
    i->mCyl.OnAtSPrmBit(1);
}
static inline void cyl_OffAtSet(cc_class* i) {
    i->mCyl.OffAtSPrmBit(1);
}
/* true60 (attack hit checks like the original, once per 30 Hz frame): the body
 * cylinder's At (the jump attack) is checked once per frame, by the tick30 collision Move, as the original's one Move per
 * frame (the cylinder centre is the frame's, set on the first step; with a converted player the converted pair was also
 * checked by every in-between step's Move, against the player's in-between positions). On an in-between step the At set
 * bit is cleared for that step's Move only (its Tg / Co stay registered every step) and set again at the next execute's
 * start, before any state code reads or changes it. dt = 1 and the tick30 step: nothing. */
/* (candidate, pair frame 308: while the At is masked, the same cylinder's Co set bit
 * is masked too: the jump attack's body cylinder is the attack volume, and a Co push on an in-between step's Move
 * separated it from Link before the tick30 Move's At check: A's trade (both hit at 308's Move, Link damaged at 310)
 * became a one-sided hit in B (no damage). With both masked, the cylinder meets Link once per frame, At and Co
 * together, as in the original Move. The push of the tick30 Move is held over the next frame's steps (the first-step push rule);
 * the posMove branch counts a masked Co set bit as set.) */
/* (stone-slime route pair: the Co set bit is masked on
 * EVERY in-between step, with or without the At: the body cylinder pushes on the tick30 Move only, the one-sweep-per-frame attack rule's one
 * registration per 30 Hz frame, as the Bokoblin / Darknut / Octorok / ReDead / Miniblin Co volumes. Why: the centre is
 * set on the frame's first step only, and a Move's Co correction (dCcS::SetPosCorrect) moves that centre out of the
 * overlap. With Link converted and the Co registered every step, the Move after the first step met the frame's whole
 * interpenetration and corrected the centre; the tick30 Move then met only the leftover (about Link's share, which his
 * held push had not moved him yet), and the first-step push rule hold handed out that leftover over the next frame: ~1/10 of
 * A's push (B = A - 2 steps: A's first push |1.39| at A 3807, Link 0.15; B's Move after the first step |1.95| at B 3804
 * (B ~0.6 deeper from the 1-frame-early landing), its tick30 Move |0.06|; B 3805 |3.55| / |0.21|). B's ChuChu walked
 * into Link (Link pushed ~1/10 too), attacked ~6 units closer, Link's damage 2 frames early (301 / 299, 556 / 554).
 * Registered for the tick30 Move only, the pair meets the frame-start centre and Link's frame-end position, A's one
 * Move, and the first-step push hold hands out A's push (the ChuChu's and Link's share). The in-between Moves leave m_cc_move as the
 * tick30 Move wrote it (the cylinder is not in their Co list). Negative control WWHD_T60_CCPUSH_NEGCTRL=1: the Co
 * masked with the At only, as before.) */
static inline bool cc_t60_co_negctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_CCPUSH_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
static inline void cc_t60_at_mask(cc_class* i) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    const bool at = (i->mCyl.mObjAt.mSPrm & 1) != 0;
    if (!at && cc_t60_co_negctrl())
        return;
    u32 m = 0;
    if (at) {
        i->mCyl.OffAtSPrmBit(1);
        m |= 1;
    }
    if (i->mCyl.mObjCo.mSPrm & 1) {
        i->mCyl.OffCoSPrmBit(1);
        m |= 2;
    }
    if (m != 0)
        s_ccT60AtMasked[gabi::ea(i)] = m;
}
static inline bool cc_t60_co_masked(cc_class* i) {
    auto it = s_ccT60AtMasked.find(gabi::ea(i));
    return it != s_ccT60AtMasked.end() && (it->second & 2) != 0;
}
static inline void cc_t60_at_restore(cc_class* i) {
    auto it = s_ccT60AtMasked.find(gabi::ea(i));
    if (it == s_ccT60AtMasked.end())
        return;
    const u32 m = it->second;
    s_ccT60AtMasked.erase(it);
    if (m & 1)
        i->mCyl.OnAtSPrmBit(1);
    if (m & 2)
        i->mCyl.OnCoSPrmBit(1);
}
static inline void cyl_OnTgSet(cc_class* i) {
    i->mCyl.OnTgSPrmBit(1);
}
static inline void cyl_OffTgSet(cc_class* i) {
    i->mCyl.OffTgSPrmBit(1);
}
static inline void cyl_OnCoSet(cc_class* i) {
    i->mCyl.OnCoSPrmBit(1);
}
static inline void cyl_OffCoSet(cc_class* i) {
    i->mCyl.OffCoSPrmBit(1);
}

// HD: reading a joint matrix marks its matrix block dirty.
static Mtx34* cc_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 block = reinterpret_cast<CcModel*>(model)->mMatrixBlock;
    auto* matrices = Cc_view<CcMatrixBlock>(block);
    matrices->mFlags = matrices->mFlags | 0x10;
    return Cc_view<Mtx34>(matrices->mMatrices + jntNo * sizeof(Mtx34));
}

/* true60 (stone-slime pair): the dispatch's
 * landing decisions (the fall's mode 2, the stand-up 0xb, the swim's ground exit, the jump attack's landing 0x21) read
 * the ground answer of the frame's FIRST step, held over the frame: in A the dispatch reads the BG check of the previous
 * frame's end. Read live, a landing inside the first half of a frame (the BG check of an in-between step met the ground)
 * was decided on that frame's tick30 step, one record frame before A (the et ChuChu's fall from 1000 at 55 a frame: A
 * lands with the BG check of frame 136 and changes to mode 3 at 137; B's step 0 of 136 met the ground and its tick30 step
 * changed at 136), and every later state of the ChuChu, its attacks and Link's damage came one frame early. The answer is
 * latched at the execute's start on the frame's first step (the tick30 BG check of the previous frame, A's); an execute
 * that missed the frame's first step reads live. The other ground reads (the exact-arc term) stay per step. dt = 1: live.
 * Negative control WWHD_T60_CCGND_NEGCTRL=1. Host memory, this unit. */
struct CcT60Gnd {
    u32 frame;
    bool gnd;
};
static std::unordered_map<u32, CcT60Gnd> s_ccT60GndHeld;
static inline bool cc_t60_gnd_negctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_CCGND_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
/* the execute's start, every step */
static inline void cc_t60_gnd_begin(cc_class* i) {
    if (t60::dt() == 1.0f || !t60::first_step() || cc_t60_gnd_negctrl())
        return;
    s_ccT60GndHeld[gabi::ea(i)] = CcT60Gnd{bk_t60_frame_tag(), i->mAcch.ChkGroundHit() != 0};
}
/* the dispatch's ground answer */
static inline bool cc_t60_gnd(cc_class* i) {
    if (t60::dt() != 1.0f && !cc_t60_gnd_negctrl()) {
        auto it = s_ccT60GndHeld.find(gabi::ea(i));
        if (it != s_ccT60GndHeld.end() && it->second.frame == bk_t60_frame_tag())
            return it->second.gnd;
    }
    return i->mAcch.ChkGroundHit();
}
static inline void zero_m35C(cc_class* i_this) {
    for (s32 i = 0; i < 5; i++)
        i_this->mWavePhase[i] = 0;
}

/* inlined into daCC_Execute */
static inline void action_nomal_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    switch (i_this->mMode) {
    case 0:
        cyl_OffTgSet(i_this);
        cyl_OffCoSet(i_this);
        i_this->mCyl.ClrTgHit();
        anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 0.0f, -1);
        if (i_this->mBehaviorType == 0) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TSTART01_e, 0.0f, 0, 0.0f, -1);
        }
        i_this->mMode += 1;
        cc_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
        [[fallthrough]];
    case 1:
        if (fopAcM_searchPlayerDistance(a_this) < i_this->mNoticeRange) {
            a_this->actor_status |= fopAcStts_SHOWMAP_e;
            if (i_this->mBehaviorType == 1) {
                anm_init(i_this, dRes_INDEX_CC_BCK_TSTART01_e, 0.0f, 0, 1.0f, -1);
                i_this->mMode = 2;
            } else {
                fopAcM_seStart(a_this, JA_SE_CM_CC_ENTER_GND, 0);
                anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 1.0f, -1);
                i_this->mNoBgCheck = 0;
                a_this->gravity = -3.0f;
                i_this->mMode = 3;
                cyl_OnTgSet(i_this);
                a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            }
        }
        break;

    case 2:
        if (i_this->mpMorf->checkFrame(22.0f)) {
            i_this->mNoBgCheck = 0;
            a_this->gravity = -11.0f;
        }
        if (i_this->mpMorf->getFrame() > 22.0f) {
            cLib_addCalcAngleS2(&a_this->shape_angle.x, 0, 1, 0x1000);
        }
        if (cc_t60_gnd(i_this)) { /* true60: the frame-start answer */
            u32 snd =
                dComIfG_Bgsp()->GetMtrlSndId(static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd));
            fopAcM_seStart(a_this, JA_SE_CM_CC_LANDING, snd);
            cc_eff_set(i_this, 1);
            a_this->shape_angle.x = 0;
            a_this->speed.y = 0.0f;
            anm_init(i_this, dRes_INDEX_CC_BCK_TSTART02_e, 0.0f, 0, 1.0f, -1);
            i_this->mMode = 3;
            cyl_OnTgSet(i_this);
            a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            denki_start(i_this);
        }
        break;

    case 3:
        if (!i_this->mpMorf->isStop()) {
            break;
        }
        i_this->mMode += 1;
        [[fallthrough]];
    case 4: {
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        zero_m35C(i_this);
        i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);
        cyl_OnCoSet(i_this);
        cyl_OnTgSet(i_this);
        i_this->mTimers[0] = 10;
        s16 r = (s16)gabi::ftoi(cM_rndF(15.0f));
        /* HD: the stored 10 is not reloaded after the call */
        i_this->mTimers[0] = (s16)(r + 10 + i_this->mTimers[1]);
        denki_start(i_this);
        if (i_this->mAnmResIdx != dRes_INDEX_CC_BCK_TACHI_WALK_e) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI_WALK_e, 2.0f, 2, 1.0f, -1);
        }
        fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_STAND, 0);
        i_this->mMode += 1;
        break;
    }

    case 5:
        search_angle_set(i_this);
        if (i_this->mpMorf->checkFrame(0.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_STAND, 0);
        }
        cLib_addCalc2(&a_this->speedF, 2.0f, 0.2f, 0.5f);
        if (i_this->mTimers[0] != 0) {
            break;
        }
        if (i_this->mBehaviorType == 2 && i_this->mTimers[5] == 0) {
            i_this->mMode = 6;
            break;
        }
        if (i_this->mColorType == 1 || i_this->mColorType == 4) {
            if (fopAcM_searchPlayerDistance(a_this) < 380.0f) {
                i_this->mCurrAction = 2;
                i_this->mMode = 0x1e;
                return;
            }
            break;
        }
        i_this->mMode += 1;
        [[fallthrough]];
    case 6:
        denki_start(i_this);
        fopAcM_seStart(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);
        i_this->mCyl.SetTgType(CC_TG_TYPE_CROUCH);
        cyl_OffCoSet(i_this);
        anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 2.0f, 0, 1.0f, -1);
        i_this->mMode += 1;
        break;

    case 7:
        if (!i_this->mpMorf->isStop()) {
            break;
        }
        if (i_this->mBehaviorType == 2 && i_this->mTimers[5] == 0) {
            i_this->mCurrAction = 5;
            i_this->mMode = 0x44;
            break;
        }
        anm_init(i_this, dRes_INDEX_CC_BCK_HUSE_WALK_e, 0.0f, 2, 1.0f, -1);
        i_this->mMode += 1;
        [[fallthrough]];
    case 8: {
        i_this->mTimers[0] = 0x46;
        s16 r = (s16)gabi::ftoi(cM_rndF(15.0f));
        i_this->mTimers[0] = (s16)(r + 0x46);
        fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_LIE, 0);
        a_this->speedF = 1.0f;
        i_this->mMode += 1;
        break;
    }

    case 9:
        if (i_this->mpMorf->checkFrame(0.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_LIE, 0);
        }
        search_angle_set(i_this);
        if (i_this->mTimers[0] != 0) {
            break;
        }
        i_this->mMode += 1;
        [[fallthrough]];
    case 10:
        i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);
        anm_init(i_this, dRes_INDEX_CC_BCK_HUSE2TACHI_e, 0.0f, 0, 1.0f, -1);
        fopAcM_seStart(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
        i_this->mMode += 1;
        break;

    case 0xb:
        if (i_this->mpMorf->isStop()) {
            fopAcM_monsSeStart(a_this, JA_SE_CV_CC_STAND_UP, 0);
            if (i_this->mBehaviorType != 3 && fopAcM_searchPlayerDistance(a_this) < 380.0f &&
                cc_t60_gnd(i_this) /* true60: the frame-start answer */) {
                i_this->mCurrAction = 2;
                i_this->mMode = 0x1e;
                return;
            }
            i_this->mMode = 4;
        }
        break;
    }

    cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x400);
    cc_t60_note_chase(i_this, 0x400);

    if ((i_this->mMode < 4 || i_this->mMode > 0xb || !shock_damage_check(i_this)) &&
        i_this->mMode >= 3) {
        body_atari_check(i_this);
        black_light_check(i_this);
    }
}

/* inlined into daCC_Execute */
static inline void action_oyogu(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;

    switch (i_this->mMode) {
    case 0x14:
        zero_m35C(i_this);
        denki_start(i_this);
        i_this->mIsSplit = 0;
        fopAcM_seStart(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);
        i_this->mSwingAngle.x = 0;
        i_this->mSwingAngle.y = 0;
        i_this->mSwingAngle.z = 0;
        i_this->mCyl.SetTgType(CC_TG_TYPE_SWIM);
        cyl_OffCoSet(i_this);
        if (i_this->mColorType != 2 && i_this->mColorType != 4) {
            cyl_OffAtSet(i_this);
        }
        fopAcM_seStart(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);
        if (i_this->mAnmResIdx != dRes_INDEX_CC_BCK_TACHI2HUSE_e) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 2.0f, 0, 1.0f, -1);
        }
        a_this->speedF = 0.0f;
        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;
        a_this->gravity = -3.0f;
        i_this->mWaterRiseSpeed = 0.0f;
        i_this->mOnWaterSurface = 0;
        i_this->mMode += 1;
        cc_t60_ai_ran(i_this); /* true60: mode 0x15's body runs in this call (fall-through): not held */
        [[fallthrough]];
    case 0x15: {
        if (i_this->mAnmResIdx == dRes_INDEX_CC_BCK_TACHI2HUSE_e) {
            if (i_this->mpMorf->isStop()) {
                anm_init(i_this, dRes_INDEX_CC_BCK_HUSE_WALK_e, 0.0f, 2, 1.0f, -1);
            }
        }
        cLib_addCalc0(&a_this->gravity, 0.2f, 1.0f);
        cLib_addCalc0(&a_this->speed.y, 0.2f, 1.0f);
        /* true60: the bob's phase (visual) by the step's share of its per-frame step */
        i_this->mWavePhase[0] += cc_t60_step_s16(0x800);
        f32 s = gabi::fmadds(cM_ssin(i_this->mWavePhase[0]), 3.0f, -15.0f);
        cLib_addCalc2(&i_this->mSwimBobY, s, 1.0f, 10.0f);

        if (i_this->mAcch.m_flags & dBgS_Acch::WATER_IN) {
            /* m_wtr.GetHeight(): mAcch + 0x1BC */
            cLib_addCalc2(&a_this->current.pos.y, i_this->mAcch.m_wtr.m_height - 20.0f, 1.0f,
                          i_this->mWaterRiseSpeed);
            cLib_addCalc2(&i_this->mWaterRiseSpeed, 6.0f, 1.0f, 1.0f);
            f32 h = i_this->mAcch.m_wtr.m_height - 20.0f;
            if (std::fabs((f32)(a_this->current.pos.y - h)) < 2.0f) {
                i_this->mOnWaterSurface = 1;
                cLib_addCalc2(&a_this->speedF, REG_F(8, 17) + 1.0f, 0.3f, 1.0f);
                if (REG_F(8, 17) != 0.0f || a_this->speedF < 2.0f) {
                    search_angle_set(i_this);
                }
            }
        }

        if (i_this->mpMorf->checkFrame(0.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_LIE, 0);
        }
        break;
    }
    }

    body_atari_check(i_this);

    if (!(i_this->mAcch.m_flags & dBgS_Acch::WATER_IN)) {
        a_this->gravity = -3.0f;
        if (cc_t60_gnd(i_this) /* true60 */ || (i_this->mBehaviorType == 2 && i_this->mTimers[5] == 0)) {
            i_this->mOnWaterSurface = 0;
            i_this->mSwimBobY = 0.0f;
            if (i_this->mBehaviorType != 2) {
                i_this->mCurrAction = 0;
                i_this->mMode = 10;
            } else {
                i_this->mCurrAction = 5;
                i_this->mMode = 0x44;
            }
        }
    }
}

/* inlined into daCC_Execute */
static inline void action_attack_move(cc_class* i_this, u32 t60Entry) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopEn_enemy_c* e_this = &i_this->actor;

    switch (i_this->mMode) {
    case 0x1E:
        zero_m35C(i_this);
        a_this->speedF = 0.0f;
        denki_start(i_this);
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        cyl_OnCoSet(i_this);
        fopAcM_monsSeStart(a_this, JA_SE_CV_CC_ATTACK, 0);
        i_this->mAttackFrame = 0;
        anm_init(i_this, dRes_INDEX_CC_BCK_ATACK01_e, 0.0f, 0, 1.0f, -1);
        i_this->mMode += 1;
        cc_t60_ai_ran(i_this); /* true60: mode 0x1F's body runs in this call (fall-through): not held */
        [[fallthrough]];
    case 0x1F:
        if (!i_this->mpMorf->isStop())
            break;
        i_this->mTimers[0] = 10;
        i_this->mMode += 1;
        cc_t60_ai_ran(i_this); /* true60: mode 0x20's body runs in this call (fall-through): not held */
        [[fallthrough]];
    case 0x20:
        if (i_this->mTimers[0] == 0) {
            a_this->current.angle.y = fopAcM_searchPlayerAngleY(a_this);
            cyl_OnAtSet(i_this);
            i_this->mCyl.mObjAt.mRPrm = 1; /* OnAtHitBit (HD: a plain store) */
            anm_init(i_this, dRes_INDEX_CC_BCK_ATACK02_e, 0.0f, 0, 1.0f, -1);
            fopAcM_seStart(a_this, JA_SE_CM_CC_ATTACK, 0);
            a_this->speedF = 18.0f;
            a_this->gravity = -3.0f;
            a_this->speed.y = 25.0f;
            i_this->mMode += 1;
        }
        break;

    case 0x21:
        i_this->mAcch.OnLineCheck();
        /* ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
        if (a_this->speedF > 0.0f && (i_this->mCyl.mGObjAt.mRPrm & 1)) {
            a_this->speedF *= -0.5f;
            a_this->speed.y = 0.0f;
        }
        if (cc_t60_gnd(i_this)) { /* true60: the frame-start answer */
            if (i_this->mColorType != 2 && i_this->mColorType != 4) {
                cyl_OffAtSet(i_this);
            }
            u32 snd =
                dComIfG_Bgsp()->GetMtrlSndId(static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd));
            fopAcM_seStart(a_this, JA_SE_CM_CC_LANDING, snd);
            cc_eff_set(i_this, 1);
            a_this->speedF = 0.0f;
            anm_init(i_this, dRes_INDEX_CC_BCK_ATACK03_e, 0.0f, 0, 1.0f, -1);
            i_this->mMode += 1;
        }
        break;

    case 0x22:
        if (i_this->mpMorf->isStop()) {
            i_this->mTimers[1] = 5;
            /* HD: mTimers[1] += cM_rndF(mTimers[1]) folded to 5 + cM_rndF(5) in float */
            f32 r = cM_rndF(5.0f);
            i_this->mTimers[1] = (s16)gabi::ftoi(5.0f + r);
            i_this->mCurrAction = 0;
            i_this->mMode = 4;
        }
        break;
    }

    /* true60 (the 30 Hz timer rule): the attack frame counts once per 30 Hz frame, on the frame's last dispatch call
     * (the tick30 step, or the in-between step whose change holds the dispatch for the rest of the frame; d_a_bk_t60.h
     * bk_t60_frame_work); the parry window it opens is read by the player (it executes before the ChuChu), so the
     * player sees the previous frame's window on every step of the frame, as at 30 Hz. the first-step AI-timer rule audit:
     *the attack frame drives no decision or motion of the ChuChu (only the window read
     * by the player; the original counts it after the dispatch), so it keeps this form, like the Moblin's parry counter
     * (d_a_mo2_exec). The window value holds on the frame's other steps while the attack runs (before: reset to 1000 on
     * every in-between step, closing the window there). dt = 1: counted on every call, 1000 written as the original. */
    const bool t60InAttack = i_this->mMode >= 0x1F && i_this->mMode <= 0x22;
    /* (a change made by a fall-through, cc_t60_ai_ran, is not held: the frame's count stays on the tick30 step; counted
     * here too it counted twice in the frame: cckB1_4 / cckB4_4 / cckZ4_4 CF50 attack start 2/30 -> 2/31, B one ahead) */
    const bool t60Counted =
        t60InAttack && (bk_t60_end() || (cc_t60_ai_state(i_this) != t60Entry && !cc_t60_ai_ran_now(i_this)));
    if (t60Counted || !t60InAttack)
        e_this->mBtNowFrame = 1000.0f;
    if (t60Counted) {
        i_this->mAttackFrame += 1;
        if (i_this->mAttackFrame > (s16)gabi::ftoi(REG_F(8, 7)) &&
            i_this->mAttackFrame < (s16)gabi::ftoi(REG_F(8, 8) + 25.0f)) {
            e_this->mBtNowFrame = 5.0f;
        }
    }

    if (i_this->mMode > 0x20 || !shock_damage_check(i_this)) {
        cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x1000);
        cc_t60_note_chase(i_this, 0x1000);
        black_light_check(i_this);
        body_atari_check(i_this);
    }
    /* true60: a hit (or shock / light) read after the count changed the state on an in-between step:
     * this call is the frame's last dispatch call, so it takes the frame's count (the original counted it before the
     * read in the same call; pair frame 286: one count short). dt = 1: counted above. */
    if (t60InAttack && !t60Counted && cc_t60_ai_state(i_this) != t60Entry && !cc_t60_ai_ran_now(i_this))
        i_this->mAttackFrame += 1;
}

/* inlined into daCC_Execute */
static inline void damage_swing(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    cMtx_YrotS(calc_mtx(), i_this->mSwingDirY);
    gabi::Local<cXyz> sp28;
    gabi::Local<cXyz> sp1C;
    sp28->x = 0.0f;
    sp28->y = 0.0f;
    // HD: retain the sine-first operand order and unextended angle passed by GHS.
    sp28->z = cM_ssin((u16)i_this->mSwingPhase) * i_this->mSwingAmplitude;
    MtxPosition(sp28, sp1C);
    i_this->mSwingAngle.z = (s16)gabi::ftoi(-sp1C->x);
    i_this->mSwingAngle.x = (s16)gabi::ftoi(sp1C->z);
    cLib_addCalc0(&a_this->speedF, 0.8f, 4.0f);
    if (i_this->mDamageType != 3) {
        /* true60: the swing's phase (visual) by the step's share of its per-frame step */
        i_this->mSwingPhase = i_this->mSwingPhase + cc_t60_step_s16(0x3000);
    }
    cLib_addCalc0(&i_this->mSwingAmplitude, 1.0f, i_this->mSwingDecay);
}

/* inlined into daCC_Execute */
static inline void action_damage_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;

    switch (i_this->mMode) {
    case 0x28: {
        zero_m35C(i_this);
        cyl_OffAtSet(i_this);
        cyl_OnTgSet(i_this);
        cyl_OnCoSet(i_this);
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;

        if (i_this->mColorType == 3 && i_this->mDamageType == 0) {
            cyl_OffTgSet(i_this);
            i_this->mCyl.ClrTgHit();
            i_this->mSwingAmplitude = 0.0f;
            i_this->mSwingDecay = 0.0f;
            i_this->mSwingPhase = 0;
            i_this->mSwingAngle.x = 0;
            i_this->mSwingAngle.y = 0;
            i_this->mSwingAngle.z = 0;
            a_this->speedF = 0.0f;
            i_this->mMode = 0x2d;
            break;
        }

        denki_end(i_this);
        anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 0.0f, 0, 0.0f, -1);
        i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);

        s16 ang = fopAcM_searchPlayerAngleY(a_this);
        i_this->mSwingDirY = (s16)((ang + 0x8000) - a_this->shape_angle.y);
        i_this->mSwingPhase = 0;
        i_this->mSwingAmplitude = 7000.0f;

        if (i_this->mDamageType == 1 || a_this->health <= 0) {
            i_this->mSwingAmplitude = 16000.0f;
            i_this->mDamageType = 1;
        }

        if (a_this->health > 0) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_DMG_SWING, 0);
            fopAcM_monsSeStart(a_this, JA_SE_CV_CC_DAMAGE, 0);
        } else {
            fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
            fopAcM_monsSeStart(a_this, JA_SE_CV_CC_DIE, 0);
        }

        cc_eff_set(i_this, 0);

        a_this->speed.y = 0.0f;
        i_this->mSwingDecay = 250.0f;

        switch (i_this->mDamageType) {
        case 1:
            i_this->mSwingDecay = 500.0f;
            a_this->speedF = 60.0f;
            break;
        case 4:
        case 9:
            anm_init(i_this, dRes_INDEX_CC_BCK_MAHI_e, 0.0f, 0, 1.0f, -1);
            dComIfGp_particle_set(ID_IT_JN_PIYOHIT00, &a_this->attention_info.position);
            i_this->mSwingAmplitude = 0.0f;
            i_this->mSwingDecay = 0.0f;
            i_this->mSwingPhase = 0;
            break;
        case 3:
            if (((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
                fopAcM_cancelCarryNow(a_this);
                a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
                cyl_OnCoSet(i_this);
                a_this->gravity = -3.0f;
                a_this->speedF = 0.0f;
            }
            i_this->mSwingAmplitude = 16000.0f;
            i_this->mSwingPhase = 10000;
            break;
        default:
            a_this->speedF = 20.0f;
            break;
        }

        if (i_this->mBehaviorType == 3) {
            a_this->speedF = 0.0f;
        }

        if (i_this->mDamageType != 3) {
            s16 a = fopAcM_searchPlayerAngleY(a_this);
            a_this->current.angle.y = (s16)(a + 0x8000);
        }

        i_this->mMode += 1;
    }
        cc_t60_ai_ran(i_this); /* true60: mode 0x29's body runs in this call (fall-through): not held */
        [[fallthrough]];
    case 0x29:
        damage_swing(i_this);
        if (std::fabs((f32)i_this->mSwingAmplitude) < 1.0f) {
            a_this->speedF = 0.0f;
            i_this->mSwingAngle.x = 0;
            i_this->mSwingAngle.y = 0;
            i_this->mSwingAngle.z = 0;

            if (i_this->mDamageType == 4 || i_this->mDamageType == 2) {
                i_this->mTimers[4] = 0x5A; /* 0x2d + 0x2d */
                i_this->mTimers[4] = (s16)(REG_S(8, 5) + 0x5A);
                enemy_piyo_set(a_this);
                fopAcM_seStart(a_this, JA_SE_CM_MD_PIYO, 0);
                i_this->mMode = 0x2b;
            } else if (i_this->mDamageType == 9) {
                i_this->mTimers[4] = (s16)(REG_S(8, 4) + 0x14);
                i_this->mMode = 0x2c;
            } else {
                i_this->mMode += 1;
            }
        }
        break;

    case 0x2a:
        if (a_this->health > 0) {
            if (i_this->mTimers[4] == 0) {
                i_this->mCurrAction = 0;
                i_this->mMode = 6;
                if (i_this->mColorType == 1 || i_this->mColorType == 4) {
                    i_this->mMode = 4;
                }
            }
        } else {
            i_this->mCurrAction = 4;
            i_this->mMode = 0x32;
        }
        break;

    case 0x2b:
        if (i_this->mTimers[4] <= 1) {
            anm_init(i_this, dRes_INDEX_CC_BCK_HUKKATSU_e, 0.0f, 2, 1.0f, -1);
            i_this->mTimers[4] = 0x3c;
            i_this->mTimers[4] = (s16)(REG_S(8, 6) + 0x3c);
            i_this->mMode += 1;
        }
        break;

    case 0x2c:
        if (i_this->mTimers[4] == 0) {
            i_this->mCurrAction = 0;
            i_this->mMode = 6;
            if (i_this->mColorType == 1 || i_this->mColorType == 4) {
                i_this->mMode = 4;
            }
        }
        break;

    case 0x2d: {
        i_this->mIsSplit = 1;
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes(reinterpret_cast<const char*>(splitArchive),
                                                   dRes_INDEX_CC_BCK_CC_PTCL_e, gc_tu.safeString);
        i_this->mpSplitMorf->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
        bk_t60_mark_anm(&i_this->mpSplitMorf); /* true60: set on a middle step, first played next frame */

        JPABaseEmitter* emitter = dComIfGp_particle_set(
            ID_AK_SN_CCSPLIT00, &a_this->attention_info.position, &a_this->current.angle);
        if (emitter != nullptr) {
            /* HD: emitter->setGlobalPrmColor / setGlobalEnvColor inlines; the prm colour scales a
             * colour of the particle manager (dPa_control_c + 0xF4) by the tev colour K0 */
            auto* e = reinterpret_cast<CcExecEmitter*>(emitter);
            auto* pa = reinterpret_cast<CcParticle*>(dComIfGp_getParticle());
            u8 r = 0, g = 0, b = 0;
            if (pa != nullptr) {
                f32 k = (f32)a_this->tevStr.mColorK0.r / 255.0f;
                r = (u8)gabi::ftoi((f32)pa->mToonColor.r * k);
                k = (f32)a_this->tevStr.mColorK0.g / 255.0f;
                g = (u8)gabi::ftoi((f32)pa->mToonColor.g * k);
                k = (f32)a_this->tevStr.mColorK0.b / 255.0f;
                b = (u8)gabi::ftoi((f32)pa->mToonColor.b * k);
            }
            e->mPrmColor.r = r;
            e->mPrmColor.g = g;
            e->mPrmColor.b = b;
            e->mEnvColor.r = a_this->tevStr.mColorK0.r;
            e->mEnvColor.g = a_this->tevStr.mColorK0.g;
            e->mEnvColor.b = a_this->tevStr.mColorK0.b;
        }
        i_this->mStts.SetWeight(0xFE);
        i_this->mMode += 1;
        break;
    }

    case 0x2e:
        if (i_this->mpSplitMorf->checkFrame(12.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_B_CC_RECOVER, 0);
        }
        if (i_this->mpSplitMorf->isStop()) {
            cyl_OnTgSet(i_this);
            i_this->mIsSplit = 0;
            i_this->mStts.SetWeight(0x32);
            i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);
            anm_init(i_this, dRes_INDEX_CC_BCK_HUSE2TACHI_e, 0.0f, 0, 1.0f, -1);
            fopAcM_seStart(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
            s16 a = fopAcM_searchPlayerAngleY(a_this);
            a_this->shape_angle.y = a;
            a_this->current.angle.y = a;
            i_this->mCurrAction = 0;
            i_this->mMode = 0xb;
        }
        break;
    }

    if (a_this->health > 0 && !body_atari_check(i_this)) {
        if (i_this->mMode >= 0x2A) {
            shock_damage_check(i_this);
        }
        black_light_check(i_this);
    }
}

/* inlined into daCC_Execute */
static inline void dead_drop(cc_class* i_this, cXyz* pos) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (i_this->mBehaviorType == 3) {
        if (a_this->stealItemBitNo != 0xff) {
            fopAcM_createDisappear(a_this, pos, 5, daDisItem_IBALL_e, a_this->stealItemBitNo);
        }
    } else if (i_this->mOtherHitState == 0) {
        fopAcM_createDisappear(a_this, pos, 5, daDisItem_IBALL_e, a_this->stealItemBitNo);
    } else if (i_this->mOtherHitState != 2) {
        u8 item;
        if (dComIfGs_getLife() <= 4) {
            item = daDisItem_HEART_e;
        } else if (cM_rndF(1.0f) < 0.5f) {
            if (dComIfGs_getMagic() <= 0x10) {
                item = daDisItem_MAGIC_e;
            } else if (dComIfGs_getArrowNum() <= 10) {
                item = daDisItem_ARROW_e;
            } else {
                item = daDisItem_HEART_e;
            }
        } else {
            item = daDisItem_NONE13_e;
        }
        fopAcM_createDisappear(a_this, pos, 5, item, 0xFF);
    } else {
        fopAcM_createDisappear(a_this, pos, 5, daDisItem_NONE13_e, 0xFF);
    }
    fopAcM_delete(a_this);
    dComIfGs_onActor(a_this->setID, a_this->home.roomNo); /* fopAcM_onActor */
}

/* inlined into daCC_Execute */
static inline void action_dead_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    gabi::Local<cXyz> sp08;
    /* HD: recompiled lfs/stfs pairs keep a signalling NaN's bits (the host elides the double round trip);
     * the stack copy is compared exactly, so copy the untouched words bit for bit */
    reinterpret_cast<be<u32>&>(sp08->x) = reinterpret_cast<be<u32>&>(a_this->current.pos.x);
    sp08->y = a_this->current.pos.y;
    reinterpret_cast<be<u32>&>(sp08->z) = reinterpret_cast<be<u32>&>(a_this->current.pos.z);
    sp08->y += 60.0f;

    switch (i_this->mMode) {
    case 50:
        denki_end(i_this);
        cyl_OffAtSet(i_this);
        cyl_OffTgSet(i_this);
        cyl_OffCoSet(i_this);
        i_this->mCyl.ClrTgHit();
        i_this->mMode += 1;
        [[fallthrough]];
    case 51:
        dead_drop(i_this, sp08);
        break;

    case 52: {
        denki_end(i_this);
        cc_eff_set(i_this, 1);
        fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
        fopAcM_monsSeStart(a_this, JA_SE_CV_CC_DIE, 0);
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes(reinterpret_cast<const char*>(deadArchive),
                                                   dRes_INDEX_CC_BCK_CC_BETA_e, gc_tu.safeString);
        i_this->mpFlatMorf->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
        bk_t60_mark_anm(&i_this->mpFlatMorf); /* true60: set on a middle step, first played next frame */
        a_this->speedF = 0.0f;
        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;
        cyl_OffAtSet(i_this);
        cyl_OffTgSet(i_this);
        cyl_OffCoSet(i_this);
        i_this->mCyl.ClrTgHit();
        i_this->mTimers[0] = 0;
        i_this->mMode += 1;
    }
        cc_t60_ai_ran(i_this); /* true60: mode 53's body runs in this call (fall-through): not held */
        [[fallthrough]];
    case 53:
        if (i_this->mpFlatMorf->isStop()) {
            dead_drop(i_this, sp08);
        }
        break;
    }
}

/* 0210DD3C */
static BOOL daCC_Execute(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    cc_t60_at_restore(i_this); /* true60: the At set bit an in-between step's Move did not see (cc_t60_at_mask) */
    cc_t60_gnd_begin(i_this);  /* true60: the frame-start ground answer for the dispatch's landing decisions */

    a_this->model = 0;
    switch (i_this->mColorType) {
    case 2:
        fopAcM_setGbaName(a_this, dItemNo_BOW_e, 30, 0x2b);
        break;
    case 3:
        fopAcM_setGbaName(a_this, dItemNo_MIRROR_SHIELD_e, 16, 0x2e);
        break;
    case 4:
        fopAcM_setGbaName(a_this, dItemNo_BOW_e, 10, 0x28);
        break;
    }

    if ((i_this->mMode >= 6 && i_this->mMode < 11) || i_this->mCurrAction == 1) {
        a_this->eyePos.x = a_this->current.pos.x;
        a_this->eyePos.y = a_this->current.pos.y;
        a_this->eyePos.z = a_this->current.pos.z;
        a_this->eyePos.y += 10.0f;
    } else {
        a_this->eyePos.x = a_this->current.pos.x;
        a_this->eyePos.y = a_this->current.pos.y;
        a_this->eyePos.z = a_this->current.pos.z;
        a_this->eyePos.y += 70.0f;
    }

    cXyz* attn = &a_this->attention_info.position; /* attention_info.position */
    if (i_this->mCurrAction != 5) {
        attn->x = a_this->eyePos.x;
        attn->y = a_this->eyePos.y;
        attn->z = a_this->eyePos.z;
        attn->y += 30.0f;
    } else {
        attn->x = a_this->current.pos.x;
        attn->y = a_this->current.pos.y;
        attn->z = a_this->current.pos.z;
        attn->y += 30.0f;
    }

    /* true60: the state this execute starts in (one AI state step per 30 Hz frame, d_a_cc_t60.h) */
    const u32 t60Entry = cc_t60_ai_state(i_this);

    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel* model = i_this->mpMorf->getModel();
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        naraku_check(i_this);
        cc_t60_ai_note(i_this, t60Entry);
        return TRUE;
    }

    /* true60 (the first-step AI-timer rule): the AI timers count on the frame's first step, before its dispatch (the
     * original counts, decides, then moves the whole frame); a timer set later in the frame is counted next frame */
    if (bk_t60_first()) {
        for (s32 i = 0; i < 7; i++) {
            if (i_this->mTimers[i] != 0) {
                i_this->mTimers[i] -= 1;
            }
        }
    }

    if (i_this->mCurrAction != 5 && i_this->mCurrAction != 4 && a_this->health > 0 &&
        i_this->mBehaviorType == 2 && DEMO_RET_START_FLAG() != 0) {
        i_this->mTimers[5] = 0;
        i_this->mCurrAction = 5;
        i_this->mMode = 0x44;
    }

    /* true60: a state entered on an earlier step of this frame waits for the next frame; the held step
     * keeps the dispatch call's shape-angle chase (cc_t60_held_chase); a dispatch call notes its own (0: none) */
    const bool t60Held = cc_t60_ai_held(i_this);
    if (t60Held)
        cc_t60_held_chase(i_this);
    else
        cc_t60_note_chase(i_this, 0);
    switch (t60Held ? 0xFF : (s32)i_this->mCurrAction) {
    case 0:
        action_nomal_move(i_this);
        break;
    case 1:
        action_oyogu(i_this);
        break;
    case 2:
        action_attack_move(i_this, t60Entry);
        break;
    case 3:
        action_damage_move(i_this);
        break;
    case 4:
        action_dead_move(i_this);
        break;
    case 5:
        action_noboru(i_this);
        break;
    case 6:
        action_up_check(i_this);
        break;
    case 7:
        action_tomaru(i_this);
        break;
    case 8:
        action_tubo_search(i_this);
        break;
    }

    if (i_this->mBehaviorType == 4) {
        cc_t60_ai_note(i_this, t60Entry);
        return TRUE;
    }

    if (i_this->mColorType == 2 || i_this->mColorType == 4) {
        Mtx34* mtx = cc_getAnmMtx(i_this->mpMorf->getModel(), CC_JNT_BODY03_e);
        i_this->mThunderPos.x = mtx->m[0][3];
        i_this->mThunderPos.y = mtx->m[1][3];
        i_this->mThunderPos.z = mtx->m[2][3];

        switch (i_this->mElectricState) {
        case 0:
            /* HD: dPa_followEcallBack::end (GameCube remove()) */
            dPa_followEcallBack_end(&i_this->mThunderEcallBack0);
            dPa_followEcallBack_end(&i_this->mThunderEcallBack1);
            break;

        case 1:
            if (i_this->mThunderEcallBack0.getEmitter() == nullptr) {
                dComIfGp_particle_set(ID_AK_JN_CCTHUNDER00, &i_this->mThunderPos, nullptr, nullptr, 0xff,
                                      (dPa_levelEcallBack*)&i_this->mThunderEcallBack0);
            }
            if (i_this->mThunderEcallBack1.getEmitter() == nullptr) {
                dComIfGp_particle_set(ID_AK_JN_CCTHUNDER01, &i_this->mThunderPos, nullptr, nullptr, 0xff,
                                      (dPa_levelEcallBack*)&i_this->mThunderEcallBack1);
            }
            if (i_this->mThunderEcallBack0.getEmitter() != nullptr && i_this->mThunderEcallBack1.getEmitter() != nullptr) {
                i_this->mElectricState += 1;
            }
            break;

        case 2:
            /* emitter->setGlobalRTMatrix(getAnmMtx(BODY03)) */
            if (JPABaseEmitter* e = i_this->mThunderEcallBack0.getEmitter()) {
                Mtx34* mtx2 = cc_getAnmMtx(i_this->mpMorf->getModel(), CC_JNT_BODY03_e);
                // HD: the callee leaves r3 (the rotation) unchanged; the original keeps using it.
                CcExecRotation* r = &reinterpret_cast<CcExecEmitter*>(e)->mGlobalRotation;
                CcExec_globalRotation(r);
                JPASetRMtxTVecfromMtx(mtx2, r,
                                      &reinterpret_cast<CcExecEmitter*>(e)->mGlobalTranslation);
            }
            if (JPABaseEmitter* e = i_this->mThunderEcallBack1.getEmitter()) {
                Mtx34* mtx2 = cc_getAnmMtx(i_this->mpMorf->getModel(), CC_JNT_BODY03_e);
                // HD: the callee leaves r3 (the rotation) unchanged; the original keeps using it.
                CcExecRotation* r = &reinterpret_cast<CcExecEmitter*>(e)->mGlobalRotation;
                CcExec_globalRotation(r);
                JPASetRMtxTVecfromMtx(mtx2, r,
                                      &reinterpret_cast<CcExecEmitter*>(e)->mGlobalTranslation);
            }
            if (i_this->mTimers[3] == 0) {
                fopAcM_seStart_ool(a_this, JA_SE_CM_CC_BLUE_SPARK, 0);
                i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(42.0f) + 30.0f);
            }
            break;
        }
    }

    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), a_this->current.angle.x);
    gabi::Local<cXyz> sp30;
    sp30->x = 0.0f;
    sp30->y = 0.0f;
    reinterpret_cast<be<u32>&>(sp30->z) =
        reinterpret_cast<be<u32>&>(a_this->speedF); /* HD: word copy, as in dead_move */
    gabi::Local<cXyz> sp24;
    MtxPosition(sp30, sp24);

    a_this->speed.x = sp24->x;
    a_this->speed.z = sp24->z;
    /* true60 (the per-step movement rule): v += g dt; the move below adds the exact-arc term of the change this step made */
    const f32 t60Vy0 = a_this->speed.y;
    if (t60::dt() == 1.0f)
        a_this->speed.y += a_this->gravity;
    else
        a_this->speed.y = gabi::fadds_ppc(t60Vy0, gabi::fmuls_ppc((f32)a_this->gravity, t60::dt()));

    if (i_this->mCurrAction != 5) {
        i_this->mGroundAngle.x = 0;
        i_this->mGroundAngle.y = 0;
        i_this->mGroundAngle.z = 0;
        if (a_this->speed.y < -55.0f) {
            a_this->speed.y = -55.0f;
        }
    } else if (a_this->speed.y < -55.0f) {
        a_this->speed.y = -55.0f;
    } else if (a_this->speed.y > 1.0f) {
        a_this->speed.y = 1.0f;
    }

    if (i_this->mMode != 0x35) {
        if (i_this->mBehaviorType == 2 && i_this->mDekuState != 3) {
            J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), &i_this->mDekuMtx);
            a_this->current.pos.copy(i_this->mHomePos);
        }

        /* true60: an animation set on a middle step starts playing next frame (d_a_bk_t60.h
         * bk_t60_mark_anm; the ChuChu plays after its dispatch: the Bokoblin form, the split-actor state rule) */
        if (i_this->mIsSplit == 0) {
            if (!bk_t60_pending(&i_this->mpMorf))
                i_this->mpMorf->play(nullptr, 0, 0);
        } else {
            if (!bk_t60_pending(&i_this->mpSplitMorf))
                i_this->mpSplitMorf->play(nullptr, 0, 0);
        }

        if (i_this->mStoneState != 0) {
            i_this->mpStoneMorf->play(nullptr, 0, 0);
            switch (i_this->mStoneState) {
            case 1:
            case 2:
                /* true60 (the animation-rate rule): a hand-stepped frame (1 per frame, ends the recovery at 25): on the
                 * tick30 step, after the frame's dispatch as in the original */
                if (!bk_t60_end())
                    break;
                i_this->mStoneAnmFrame += 1;
                if (i_this->mStoneAnmFrame > 0x19) {
                    i_this->mStoneAnmFrame = 0x19;
                }
                break;
            }
        }

        if (i_this->mBehaviorType == 2 && i_this->mDekuState != 3) {
            gabi::Local<cXyz> sp18;
            sp18->x = 0.0f;
            sp18->y = 0.0f;
            sp18->z = 0.0f;
            PSMTXCopy(&i_this->mDekuMtx, mDoMtx_stack_c::get());
            PSMTXMultVec(mDoMtx_stack_c::get(), sp18, &i_this->mHomePos);
            a_this->current.pos.copy(i_this->mHomePos);
        }
    } else if (!bk_t60_pending(&i_this->mpFlatMorf)) {
        i_this->mpFlatMorf->play(nullptr, 0, 0);
    }

    /* true60 (rule 14): the cylinder's centre where the original frame has it for the tick30 collision
     * Move (after the dispatch, before the frame's move): set on the frame's first step */
    if (bk_t60_first())
        i_this->mCyl.SetC(&a_this->current.pos);
    f32 h = 100.0f;
    if (i_this->mAnmResIdx == dRes_INDEX_CC_BCK_HUSE_WALK_e) {
        h = 30.0f;
    }
    i_this->mCyl.SetH(h);
    i_this->mCyl.SetR(35.0f);
    cc_t60_at_mask(i_this); /* true60: the At and the Co are checked by the tick30 Move only */
    dComIfG_Ccsp_Set(&i_this->mCyl);
    if (!((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
        if ((i_this->mCyl.mObjCo.mSPrm & 1) || cc_t60_co_masked(i_this)) { /* ChkCoSet (or masked for this step's Move) */
            if (t60::dt() == 1.0f) {
                fopAcM_posMove(a_this, &i_this->mStts.m_cc_move);
            } else {
                gabi::Local<cXyz> t60Push; /* true60 (the first-step push rule): the frame's held push */
                cc_t60_push_step(i_this, t60Push);
                fopAcM_posMove(a_this, t60Push);
            }
        } else {
            fopAcM_posMove(a_this, nullptr);
        }
        /* true60 (the per-step movement rule): the exact-arc term of this step's gravity (v += g dt, then p += v dt end
         * where the original frame's v += g, p += v ends). Not while resting on the ground (falling or still, and the
         * last step met the ground): the ground stops that fall on every step, there is no arc to carry, and the term
         * would lift the body off the ground at N >= 4 ('s ice block, pair). */
        if (t60::dt() != 1.0f) {
            f32 dv = gabi::fsubs_ppc((f32)a_this->speed.y, t60Vy0);
            if (dv != 0.0f && !((f32)a_this->speed.y <= 0.0f && i_this->mAcch.ChkGroundHit()))
                a_this->current.pos.y = gabi::fadds_ppc(
                    (f32)a_this->current.pos.y,
                    gabi::fmuls_ppc(gabi::fmuls_ppc(dv, gabi::fsubs_ppc(1.0f, t60::dt())), 0.5f));
        }
    }
    BG_check(i_this);
    draw_SUB(i_this);
    /* true60: a state entered on an in-between step (the dispatch, BG_check's water / abyss checks)
     * holds the dispatch for the rest of the frame */
    cc_t60_ai_note(i_this, t60Entry);
    return TRUE;
}

/* WWHD glue
data splitArchive 1000C530
data deadArchive 1000C533
ext 0210C828 void naraku_check(cc_class*)
ext 0210BB58 void draw_SUB(cc_class*)
ext 0210C364 void cc_eff_set(cc_class*, u8)
ext 0210C5C8 void anm_init(cc_class*, int, f32, u8, f32, int)
ext 0210CBC0 void denki_start(cc_class*)
ext 0210CC00 void denki_end(cc_class*)
ext 0210CC58 BOOL shock_damage_check(cc_class*)
ext 0210CE2C void black_light_check(cc_class*)
ext 0210CE9C BOOL body_atari_check(cc_class*)
ext 0210DA18 BOOL search_angle_set(cc_class*)
ext 0210DC8C void BG_check(cc_class*)
ext 021116B8 void action_noboru(cc_class*)
ext 02112414 void action_up_check(cc_class*)
ext 02112DA0 void action_tomaru(cc_class*)
ext 02112E74 void action_tubo_search(cc_class*)
ext 02111638 void fopAcM_seStart_ool(fopAc_ac_c*, u32, u32)
ext 0211310C void CcExec_globalRotation(CcExecRotation*)
ext 020402C8 BOOL enemy_ice(enemyice_l*)
ext 02041CAC void enemy_piyo_set(fopAc_ac_c*)
ext 025DA088 void fopAcM_setGbaName(fopAc_ac_c*, u8, u8, u8)
ext 028249B0 void JPASetRMtxTVecfromMtx(Mtx34*, CcExecRotation*, cXyz*)
ext 025E1AA4 void CcExec_monsSeStart(u32, cXyz*, u32, u32, s32)
*/
