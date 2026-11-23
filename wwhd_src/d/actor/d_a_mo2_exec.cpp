/**
 * d_a_mo2_exec.cpp (WWHD)
 * Enemy - Moblin: daMo2_Execute with its inlined helpers (damage_check, mo2_eye_tex_anm,
 * mo2_demo_camera, ground_smoke_set, yari_off_check).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mo2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_mo2_exec.h"
#include "d/actor/d_a_mo2_t60.h"
#include <cstdlib>
#include <unordered_map>

/* true60 (the first-step AI-timer rule): the AI timers (slots 0-10) and the phase counter count on the frame's FIRST
 * step, before its decisions, as the original counts at the start of its frame: a timer an
 * action sets on any step of the frame is first counted next frame, as in the original. The catch timers (slots 11, 12:
 * event work on the 30 Hz grid) still count on the tick30 step; one an action set on an earlier step of the same 30 Hz
 * frame is not counted in that frame: the frame's first step holds their values before its actions (host memory keyed by
 * the actor, frame tag g_Counter.mCounter0) and the tick30 step counts one only when it still has that value (a set to
 * the very value it had is not seen: counted one early). Slots: 0-4 mTimers, 5 mSideStepTimer, 6 mSideStepWait,
 * 7 mWeaponRunTimer, 8 mWeaponViewWait, 9 mHitStunTimer, 10 mDashStopTimer, 11 mCatchTimer, 12 mCatchDemoDelay. */
namespace {
constexpr int kMo2T60Slots = 13;
struct Mo2T60Count {
    u32 frame;
    s16 v[kMo2T60Slots];
};
std::unordered_map<u32, Mo2T60Count> s_mo2T60Counts;
}  // namespace
static inline s16 mo2_t60_count_value(mo2_class* i_this, int i) {
    switch (i) {
    case 5: return i_this->mSideStepTimer;
    case 6: return i_this->mSideStepWait;
    case 7: return i_this->mWeaponRunTimer;
    case 8: return i_this->mWeaponViewWait;
    case 9: return i_this->mDamageReaction.mHitStunTimer;
    case 10: return i_this->mDashStopTimer;
    case 11: return i_this->mCatchTimer;
    case 12: return i_this->mCatchDemoDelay;
    default: return i_this->mTimers[i];
    }
}
static void mo2_t60_count_begin(mo2_class* i_this) {
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    Mo2T60Count& e = s_mo2T60Counts[gabi::ea(i_this)];
    e.frame = bk_t60_frame_tag();
    for (int i = 0; i < kMo2T60Slots; i++)
        e.v[i] = mo2_t60_count_value(i_this, i);
}
/* on the tick30 step: count slot i (dt = 1: always) */
static bool mo2_t60_count(mo2_class* i_this, int i) {
    if (t60::dt() == 1.0f)
        return true;
    auto it = s_mo2T60Counts.find(gabi::ea(i_this));
    if (it == s_mo2T60Counts.end() || it->second.frame != bk_t60_frame_tag())
        return true;
    return it->second.v[i] == mo2_t60_count_value(i_this, i);
}
static inline void mo2_t60_count_end(mo2_class* i_this) {
    if (t60::dt() != 1.0f)
        s_mo2T60Counts.erase(gabi::ea(i_this));
}

/* true60 (the process-order read rule, pair): daMo2_Execute plays and calcs its morf
 * BEFORE Mo2_move, and sets the model's base matrix (position, angle.y, scale) only after it: the original's calc of frame
 * f runs on the base matrix of frame f - 1's end, so every joint-callback word (mLeftHandPos -> mAtPos, mRightHandPos,
 * the spear points, eyePos / attention, mJntPos) is placed by the previous frame's transform. At N steps the tick30 calc
 * ran on the base matrix the first step had set (the frame's half-way angle): f196 the 4096 turn, B's hand rotated
 * by B's step-0 angle 3392 instead of A's 1344 (10.98 / 35.66 off, decaying to f207; the rigid rotation by 2048 of A's
 * hand gives B's to 0.6). Now the frame's first step keeps the base matrix it finds (frame f - 1's end) and the later
 * steps' calcs run on it (host memory keyed by the actor, frame tag). Picture cost: the body's placement (position /
 * turn) in the in-between picture is the frame-start one, as A's picture of the frame (the pose still plays per step).
 * dt = 1: nothing. */
namespace {
struct Mo2T60Base {
    u32 frame;
    f32 m[3][4]; /* host floats (a host map's value holds no guest be<> word; beprobe) */
};
std::unordered_map<u32, Mo2T60Base> s_mo2T60Base;
}  // namespace
static void mo2_t60_base_frame(mo2_class* i_this) {
    if (t60::dt() == 1.0f)
        return;
    const u32 model = gabi::ea(i_this->mpMorf->getModel());
    if (model == 0)
        return;
    Mtx34* base = gabi::at<Mtx34>(model + 0xC8); /* J3DModel mBaseTransformMtx */
    if (t60::first_step()) {
        Mo2T60Base& e = s_mo2T60Base[gabi::ea(i_this)];
        e.frame = bk_t60_frame_tag();
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 4; j++) e.m[i][j] = base->m[i][j];
        return;
    }
    auto it = s_mo2T60Base.find(gabi::ea(i_this));
    if (it == s_mo2T60Base.end() || it->second.frame != bk_t60_frame_tag())
        return;
    const Mo2T60Base& e = it->second;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) base->m[i][j] = e.m[i][j];
}

/* true60 (True60 pair: Moblin 47C5CECC f45 mLeftHandPos.y A 81.46 / B 69.29,
 * f46 58.18 / 53.55, with the morf (WALK frame 27, no blend) and the base matrix equal): the calc's joint callback
 * (nodeCallBack, d_a_mo2.cpp 021C6854) rotates the joints by the damage reaction's mJntAngle[] (the look / body bend)
 * and the jaw by mJawAngleZ, which damage_reaction / Mo2_move write AFTER the calc. A's calc of frame f reads frame
 * f - 1's angles; at N steps the tick30 calc read the angles the frame's first step had already moved (f45: fight_run
 * started on step 0, its look turned the upper body half a frame early: every joint-callback word ahead, the hand
 * 12 off, decaying to f47; the same per-step lead wherever the angles move). As the base matrix above: the frame's
 * first step keeps the angles it finds (frame f - 1's end) and the later steps' calcs run on them; the angles the
 * steps moved are put back after the calc, so the per-step easing continues from them. Host memory keyed by the actor,
 * frame tag, this unit only (split-actor state (1)). Negative control WWHD_T60_MOPOSE_NEGCTRL=1 (game build: the calc on the
 * step's own angles, the form before). dt = 1: nothing. */
namespace {
constexpr u32 kMo2T60JntWords = 21 * 3; /* mJntAngle[21] (csXyz) */
struct Mo2T60Jnt {
    u32 frame;
    bool held; /* the frame-start angles are in place for this calc; cur / curJaw: the step's own */
    u16 a[kMo2T60JntWords], cur[kMo2T60JntWords];
    u16 jaw, curJaw;
};
std::unordered_map<u32, Mo2T60Jnt> s_mo2T60Jnt;
}  // namespace
static inline bool mo2_t60_pose_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_MOPOSE_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
static void mo2_t60_jnt_frame(mo2_class* i_this) {
    if (t60::dt() == 1.0f || mo2_t60_pose_nc())
        return;
    const u32 jnt = gabi::ea(&i_this->mDamageReaction.mJntAngle[0]), jaw = gabi::ea(&i_this->mJawAngleZ);
    if (t60::first_step()) {
        Mo2T60Jnt& e = s_mo2T60Jnt[gabi::ea(i_this)];
        e.frame = bk_t60_frame_tag();
        e.held = false;
        for (u32 k = 0; k < kMo2T60JntWords; k++) e.a[k] = gabi::load<u16>(jnt + k * 2u);
        e.jaw = gabi::load<u16>(jaw);
        return;
    }
    auto it = s_mo2T60Jnt.find(gabi::ea(i_this));
    if (it == s_mo2T60Jnt.end() || it->second.frame != bk_t60_frame_tag())
        return;
    Mo2T60Jnt& e = it->second;
    e.held = true;
    for (u32 k = 0; k < kMo2T60JntWords; k++) {
        e.cur[k] = gabi::load<u16>(jnt + k * 2u);
        gabi::store<u16>(jnt + k * 2u, e.a[k]);
    }
    e.curJaw = gabi::load<u16>(jaw);
    gabi::store<u16>(jaw, e.jaw);
}
static void mo2_t60_jnt_restore(mo2_class* i_this) {
    if (t60::dt() == 1.0f)
        return;
    auto it = s_mo2T60Jnt.find(gabi::ea(i_this));
    if (it == s_mo2T60Jnt.end() || !it->second.held)
        return;
    Mo2T60Jnt& e = it->second;
    e.held = false;
    const u32 jnt = gabi::ea(&i_this->mDamageReaction.mJntAngle[0]), jaw = gabi::ea(&i_this->mJawAngleZ);
    for (u32 k = 0; k < kMo2T60JntWords; k++) gabi::store<u16>(jnt + k * 2u, e.cur[k]);
    gabi::store<u16>(jaw, e.curJaw);
}

enum {
    AT_TYPE_HD_ICE_ARROW = 0x80000,
    AT_TYPE_HD_ICE_OR_LIGHT = 0x180000,
    AT_TYPE_HD_FIRE = 0x40200,
    JA_SE_CV_MO_DAMAGE = 0x4809,
    JA_SE_CV_MO_DAMAGE_HIP = 0x480A,
    JA_SE_CV_MO_FAINTED = 0x480E,
    JA_SE_MAJU_MO_CHECK = 0x905,
};

/* inlined into daMo2_Execute */
static inline void fopAcM_monsSeStart(fopAc_ac_c* actor, u32 id, u32 param) {
    if (gabi::ea(&actor->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(actor);
        u32 pid = fopAcM_GetID(actor);
        s32 reverb = dComIfGp_getReverb(room);
        Mo2Exec_monsSeStart(id, &actor->eyePos, pid, param, reverb);
    }
}
static inline void fopAcM_onActor(fopAc_ac_c* actor) {
    dComIfGs_onActor(actor->setID, actor->home.roomNo);
}
static inline bool fopAcM_CheckStatus(fopAc_ac_c* actor, u32 mask) {
    return (actor->actor_status & mask) != 0;
}

/* inlined into daMo2_Execute */
static inline void yari_off_check(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->mSpawnWeaponActor != 0) {
        i_this->mWeaponRunTimer = l_mo2HIO().mWeaponRunTime;
        PSMTXCopy(model_getAnmMtx((Mo2ExecModel*)i_this->mpMorf->getModel(), MO_JNT_MO_YARI_e),
                  calc_mtx());
        gabi::Local<cXyz> local_48;
        local_48->x = 0.0f;
        local_48->y = 0.0f;
        local_48->z = 0.0f;
        gabi::Local<cXyz> cStack_54;
        MtxPosition(local_48, cStack_54);
        i_this->mWeaponPcId =
            fopAcM_create(fpcNm_BOKO_e, Mo2ExecBoko::Type_MOBLIN_SPEAR_e, cStack_54,
                          fopAcM_GetRoomNo(actor), nullptr, nullptr, -1, 0);
        i_this->mbThrowWeapon = 1;
        i_this->mSpawnWeaponActorMode = i_this->mSpawnWeaponActor;
        i_this->mSpawnWeaponActor = 0;
        i_this->mbHasInnateWeapon = 0;
    }
    if (i_this->mbThrowWeapon != 0) {
        fopAc_ac_c* boko = fopAcM_SearchByID(i_this->mWeaponPcId);
        if (boko != nullptr && i_this->mSpawnWeaponActorMode != 2) {
            reinterpret_cast<Mo2ExecBoko*>(boko)->mRotAngleSpeed =
                (s16)gabi::ftoi(cM_rndFX(2000.0f));
            s16 angleY = actor->shape_angle.y + 0x8000;
            angleY = angleY + (s16)gabi::ftoi(cM_rndFX(8000.0f));
            f32 s1 = cM_rndF(10.0f) + 20.0f;
            f32 s2 = cM_rndF(10.0f) + 20.0f;
            /* daBoko_c::moveStateInit */
            boko->current.angle.y = angleY;
            boko->speed.y = s2;
            boko->speedF = s1;
            i_this->mbThrowWeapon = 0;
        }
    }
}

/* inlined into daMo2_Execute */
/* true60: a per-frame spawner with its counter: daMo2_Execute calls it on the tick30 step only (a start
 * set after the call on an in-between step waits for the next frame) */
static inline void ground_smoke_set(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->mSmokeTimer == 0) {
        return;
    }
    i_this->mSmokeTimer--;
    if (i_this->mSmokeTimer >= l_mo2HIO().mSmokeTimerBase) {
        gabi::Local<cXyz> sp8;
        sp8->x = 0.0f;
        sp8->y = 0.0f;
        i_this->mEffectAngle.x = 0;
        i_this->mEffectAngle.z = 0;
        MtxTrans(actor->current.pos.x, actor->current.pos.y + 7.5f, actor->current.pos.z, 0);
        if (i_this->mSmokeType == 0) {
            sp8->z = -350.0f;
            cMtx_YrotM(calc_mtx(), i_this->mSmokeAngleY);
            MtxPosition(sp8, &i_this->mSmokePos);
            i_this->mEffectAngle.y = i_this->mSmokeAngleY;
            smoke_set_s(i_this, 6.0f);
            i_this->mSmokeAngleY = i_this->mSmokeAngleY + REG0_S(7) + 2000;
        } else if (i_this->mSmokeType == 1) {
            cMtx_YrotM(calc_mtx(), actor->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->mSmokeAngleY);
            sp8->z = -55.0f;
            MtxPosition(sp8, &i_this->mSmokePos);
            i_this->mEffectAngle.y = i_this->mSmokeAngleY;
            smoke_set_s(i_this, 6.0f);
            i_this->mSmokeAngleY += 0x1FA0;
        } else if (i_this->mSmokeType == 2) {
            MtxTrans(i_this->mAtPos.x, i_this->mAtPos.y + 7.5f, i_this->mAtPos.z, 0);
            cMtx_YrotM(calc_mtx(), i_this->mSmokeAngleY);
            sp8->z = -12.5f;
            MtxPosition(sp8, &i_this->mSmokePos);
            i_this->mEffectAngle.y = i_this->mSmokeAngleY;
            smoke_set_s(i_this, 6.0f);
            i_this->mSmokeAngleY += 0x2000;
        } else if (i_this->mSmokeType == 3) {
            cMtx_YrotM(calc_mtx(), actor->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->mSmokeAngleY);
            sp8->z = -37.5f;
            MtxPosition(sp8, &i_this->mSmokePos);
            i_this->mEffectAngle.y = i_this->mSmokeAngleY;
            smoke_set_s(i_this, 2.0f);
            i_this->mSmokeAngleY += 0x1FA0;
        } else if (i_this->mSmokeType == 4) {
            cXyz* src = (i_this->mFrameCounter & 1) ? &i_this->mDamageReaction.mJntPos[14]
                                            : &i_this->mDamageReaction.mJntPos[15];
            f32 sy = src->y;
            i_this->mSmokePos.y = sy;
            f32 sz = src->z;
            f32 sx = src->x;
            i_this->mSmokePos.x = sx;
            i_this->mSmokePos.z = sz;
            if (i_this->mDamageReaction.m712 != 0) {
                i_this->mSmokePos.y = 512.5f;
            } else {
                i_this->mSmokePos.y = sy - 12.5f;
            }
            i_this->mEffectAngle.y = cM_atan2s(actor->speed.x, actor->speed.z);
            smoke_set_s(i_this, 1.0f);
        }
    } else {
        i_this->mSmokePos.y = i_this->mDamageReaction.mSpawnY + 25000.0f;
    }
    if (i_this->mSmokeTimer == 0) {
        Mo2Exec_endSmoke(&i_this->mSmokeCallBack);
        i_this->mSmokeCount = 0;
    }
}

/* inlined into daMo2_Execute */
/* true60: daMo2_Execute calls it on the frame's first step only (the blink counts and draws cM_rnd once
 * per frame, before Mo2_move as in the original) */
static inline void mo2_eye_tex_anm(mo2_class* i_this) {
    if (i_this->mEyeAnmWait != 0) {
        i_this->mEyeAnmWait--;
    } else {
        i_this->mEyeAnmWait = (s16)gabi::ftoi(cM_rndF(50.0f) + 10.0f);
        if (i_this->mEyeBtpPlaying == 0) {
            tex_anm_set(i_this, 3);
        }
    }
    if (i_this->mEyeBtpPlaying != 0) {
        if (i_this->mEyeBtpFrame < i_this->mEyeBtpMaxFrame) {
            i_this->mEyeBtpFrame++;
        } else {
            i_this->mEyeBtpPlaying = 0;
        }
    }
}

/* inlined into daMo2_Execute */
/* true60 (the one-sweep-per-frame attack rule with the): the guard sphere is also a Co
 * sphere (defence_sph_src: Co set, vs all): in the push (oshi, R 87.5 on the spear shaft) it shoves Link. Collision
 * registration is once per 30 Hz frame: its Co set bit is cleared for an in-between step's Move and set again at the
 * next execute's start (pair frame 44: Link pushed (42, 44) in A, (42, 33) in B, the shove met on two Moves).
 * Its Tg stays registered every step (the block reads above). dt = 1 and the tick30 step: nothing. Host memory, this
 * unit only (split-actor state (1)). */
static std::unordered_map<u32, u8> s_mo2T60CoMasked;
static inline void mo2_t60_guard_co_restore(mo2_class* i) {
    auto it = s_mo2T60CoMasked.find(gabi::ea(i));
    if (it == s_mo2T60CoMasked.end())
        return;
    s_mo2T60CoMasked.erase(it);
    i->mDefenseSph.OnCoSPrmBit(1);
}
static inline void mo2_t60_guard_co_mask(mo2_class* i) {
    if (t60::dt() == 1.0f || t60::tick30() || (i->mDefenseSph.mObjCo.mSPrm & 1) == 0)
        return;
    i->mDefenseSph.OffCoSPrmBit(1);
    s_mo2T60CoMasked[gabi::ea(i)] = 1;
}

static inline u8 damage_check(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<Mo2AtInfo> atInfo;
    atInfo->mpObj = 0;
    atInfo->mDamage = 0;
    atInfo->mPlCutBit = 0;
    u8 iVar12 = 0;
    u8 iVar11 = 0;
    /* true60: the attacker ids of the last hits shift once per 30 Hz frame, on its first step (the
     * tick30 collision Move pairs the Moblin with unconverted attackers and compares them with the previous frame's) */
    if (bk_t60_first())
        i_this->mDamageReaction.mStts.Move();
    mo2_t60_guard_co_restore(i_this);
    mo2_t60_guard_co_mask(i_this); /* true60: the guard sphere's Co meets the tick30 Move only (one-sweep-per-frame attack) */
    dComIfG_Ccsp_Set(&i_this->mDefenseSph);
    /* true60: the guard sphere's centre is the guard position the previous frame's dispatch left
     * (defence / oshi copy the spear shaft after this point): taken on the frame's first step, where that is the value
     * of the frame before (per step it would be the previous step's, a fraction of a frame ahead in the tick30 Move) */
    if (bk_t60_first())
        i_this->mDefenseSph.SetC(&i_this->mDefenseSphPos);
    /* true60 (the mixed-pair hit-flag rule): the guard's block is a mixed-pair result of the tick30 collision Move,
     * seen only on the next frame's first step (the next step's Move clears it). Read there (its sound, particle and
     * push-back once per frame, as the original) and held over the frame's other steps (frame-tag latch keyed by the
     * guard sphere): they return here too, as the original frame did not reach the guard sphere's reset below or the
     * body checks; without the hold the reset ran on those steps and the guard sphere could miss the tick30 Move */
    /* One collision Move's hits are read per 30 Hz frame (the original reads one Move per frame). A converted attacker
     * (Link converted) meets the Moblin on every step's Move, and each Move's result is cleared by the next: a later
     * step reads its predecessor's Move (block, then body, as the first step would) when no hit was read this frame;
     * a body hit read this frame (latch keyed by the head sphere) stops the frame's later reads, the guard sphere's
     * reset and the rest of the execute still run (as cckP2_4's ChuChu) */
    const bool t60BlockHeld = !bk_t60_first() &&
                              wwhd_t60_latch_pending(gabi::ea(&i_this->mDefenseSph), bk_t60_frame_tag()) != 0;
    const bool t60Skip = !bk_t60_first() && bk_t60_pending(&i_this->mHeadSph);
    if (t60BlockHeld || (!t60Skip && i_this->mDefenseSph.ChkTgHit())) {
        if (t60BlockHeld)
            return 0;
        if (t60::dt() != 1.0f && !t60::tick30())
            wwhd_t60_latch_set(gabi::ea(&i_this->mDefenseSph), bk_t60_frame_tag());
        void* obj = i_this->mDefenseSph.GetTgHitObj();
        def_se_set(actor, obj, 0x41);
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        gabi::Local<cXyz> local_48;
        local_48->x = 0.0f;
        local_48->y = 0.0f;
        local_48->z = -10.0f;
        MtxPosition(local_48, &i_this->mDamageReaction.mHitVec);
        i_this->mDamageReaction.mPushback = -15.0f;
        gabi::Local<csXyz> local_68;
        local_68->x = player->shape_angle.x;
        s16 py = player->shape_angle.y;
        local_68->y = py;
        s16 pz = player->shape_angle.z;
        local_68->y = (s16)(py - 0x8000);
        local_68->z = pz;
        GXColor* k0 = &actor->tevStr.mColorK0;
        Mo2ExecEmitter* emitter = reinterpret_cast<Mo2ExecEmitter*>(
            dComIfGp_particle_set(ID_AK_JN_ELEMENTKIKUZU00, &i_this->mDefenseSphPos, local_68, nullptr, 0xFF,
                                  nullptr, -1, k0, k0, nullptr));
        if (emitter != nullptr) {
            emitter->mMaxFrame = 1;
            emitter->mRate = 10.0f;
            emitter->mSpread = 0.2f;
            emitter->mVolumeSweep = 0.15f;
            f32 scale = REG_F(14, 16) + 0.85f;
            emitter->mGlobalParticleScale.x = scale;
            emitter->mGlobalParticleScale.y = scale;
            emitter->mGlobalParticleScale.z = scale;
        }
        return 0;
    }
    /* true60: the guard sphere's reset before the dispatch sets it again; on a held step (the state
     * entered on an earlier step of this frame, d_a_mo2_t60.h) the dispatch does not run: the frame keeps the guard its
     * dispatch set, as the original frame's Move sees it */
    if (!mo2_t60_ai_held(i_this)) {
        i_this->mDefenseSphPos.y = -10000.0f;
        i_this->mDefenseSph.SetR(-200.0f);
    }
    if (i_this->mNoHitTimer == 0 && !t60Skip && (i_this->mHeadSph.ChkTgHit() || i_this->mTgCyl.ChkTgHit())) {
        bk_t60_mark_set(&i_this->mHeadSph); /* true60: this frame's hit read (above) */
        i_this->mOshiCount = 0;
        i_this->mBtCounter = -1;
        i_this->mBtWindowTimer = 0;
        i_this->mNoHitTimer = 4; // HD: four frames instead of the GameCube register plus five.
        dCcD_GObjInf* obj;
        if (i_this->mHeadSph.ChkTgHit()) {
            iVar12 = 1;
            obj = static_cast<dCcD_GObjInf*>(i_this->mHeadSph.GetTgHitObj());
            atInfo->mpObj = obj;
            atInfo->pParticlePos = i_this->mHeadSph.GetTgHitPosP();
        } else if (i_this->mTgCyl.ChkTgHit()) {
            iVar12 = 2;
            obj = static_cast<dCcD_GObjInf*>(i_this->mTgCyl.GetTgHitObj());
            atInfo->mpObj = obj;
            atInfo->pParticlePos = i_this->mTgCyl.GetTgHitPosP();
        } else {
            obj = atInfo->mpObj;
        }
        /* HD: ChkAtType tests the object for NULL */
        if (obj != 0) {
            if (obj->ChkAtType(AT_TYPE_HD_ICE_OR_LIGHT)) {
                if (obj->ChkAtType(AT_TYPE_HD_ICE_ARROW)) {
                    i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
                    i_this->mEnemyIce.mFreezeDuration = REG0_S(3) + 300;
                    /* true60: set after this execute's enemy_ice call: the freeze starts next frame */
                    bk_t60_mark_set(&i_this->mEnemyIce.mFreezeDuration);
                    path_check(i_this);
                    wait_set(i_this);
                    i_this->mDamageReaction.mMode = -10;
                } else {
                    i_this->mEnemyIce.mLightShrinkTimer = 1;
                    /* true60: set after this execute's enemy_ice call: its phase starts next frame */
                    bk_t60_mark_set(&i_this->mEnemyIce.mLightShrinkTimer);
                }
                Mo2Exec_enemyFireRemove(&i_this->mEnemyFire);
                i_this->mSmokeTimer = 0;
                Mo2Exec_endSmoke(&i_this->mSmokeCallBack);
                if (i_this->mbHasInnateWeapon != 0) {
                    i_this->mSpawnWeaponActor = 2;
                }
                i_this->m2A09 = 1;
                return 0;
            }
            if (obj->ChkAtType(AT_TYPE_HD_FIRE)) {
                i_this->mNoHitTimer = 0x32;
                i_this->mEnemyFire.mFireDuration = REG0_S(2) + 100;
            }
        }
        s8 hp = actor->health;
        at_power_check(atInfo);
        if (atInfo->mResultingAttackType == 10 || atInfo->mResultingAttackType == 0xe) {
            actor->health = 0x14;
        }
        atInfo->mpActor = cc_at_check(actor, atInfo);
        if (atInfo->mResultingAttackType == 10 || atInfo->mResultingAttackType == 0xe) {
            actor->health = hp;
            if (atInfo->mResultingAttackType == 0xe && i_this->mItemStolen == 0) {
                i_this->mItemStolen = 1;
                i_this->mDamageReaction.mAction = ACTION_P_LOST;
                i_this->mDamageReaction.mMode = -10;
            }
        }
        gabi::Local<cXyz> pos;
        f32 px = actor->current.pos.x;
        pos->x = px;
        f32 py = actor->current.pos.y;
        pos->y = py;
        f32 pz = actor->current.pos.z;
        i_this->mNoGuardTimer = 0x19;
        pos->z = pz;
        dKy_Sound_set(pos, 100, fopAcM_GetID(actor), 5);
        // HD: health debug reset removed; type twelve sets ten invincible frames.
        if (atInfo->mResultingAttackType == 0xC) {
            i_this->mNoHitTimer = 0xA;
        }
        i_this->mCutBits |= atInfo->mPlCutBit;
        if (atInfo->mResultingAttackType == 10) {
            /* true60: Mo2_move counts it later in this execute, on the frame's first step; set on
             * another step, it starts with this frame's count already taken (as the original's same-frame count) */
            s8 delay = (s8)(REG_S(13, 3) + 8);
            if (!bk_t60_first() && delay > 1)
                delay = (s8)(delay - 1);
            i_this->mParalyzeDelay = delay;
            atInfo->mDamage = 1;
            iVar11 = atInfo->mbDead != 0 ? 7 : 4;
            cMtx_YrotS(calc_mtx(), atInfo->mHitAngle.y);
        } else {
            i_this->mParalyzeDelay = 0;
            if (atInfo->mResultingAttackType == 1) {
                s16 sVar3 = i_this->mTargetAngleY - actor->current.angle.y;
                if (sVar3 < 0) {
                    sVar3 = -sVar3;
                }
                if (reinterpret_cast<Mo2ExecPlayer*>(player)->mCutType == 5) {
                    iVar11 = 2;
                } else if ((u16)sVar3 > 0x4000) {
                    if (atInfo->mbDead != 0) {
                        iVar11 = 3;
                    } else if (iVar12 == 1) {
                        iVar11 = 4;
                    } else {
                        iVar11 = 6;
                    }
                } else if (atInfo->mbDead != 0) {
                    iVar11 = 1;
                } else if (iVar12 == 1) {
                    iVar11 = 4;
                } else {
                    iVar11 = 5;
                }
            } else if (atInfo->mResultingAttackType == 2) {
                iVar11 = 7;
                cMtx_YrotS(calc_mtx(), atInfo->mHitAngle.y);
            } else {
                iVar11 = atInfo->mbDead != 0 ? 7 : 4;
                cMtx_YrotS(calc_mtx(), atInfo->mHitAngle.y);
            }
        }
    }
    if (iVar11 != 0 && i_this->mMode == 1) {
        if (rouya_mode() != 0) {
            i_this->mDamageReaction.mAction = ACTION_NAGE;
            i_this->mDamageReaction.mMode = -10;
            iVar11 = 5;
            i_this->mNoHitTimer = 0x32;
        } else {
            i_this->mMode = 0;
            i_this->m2A09 = 1;
        }
    }
    switch (iVar11) {
    case 1:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.mHitFlags |= 0x10;
        i_this->mDamageReaction.mKnockbackPower = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->mTargetAngleY);
        break;
    case 2:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.mHitFlags |= 0x40;
        cMtx_YrotS(calc_mtx(), (s16)(actor->current.angle.y + 0x8000));
        i_this->mDamageReaction.mKnockbackPower = 26.0f;
        break;
    case 3:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.mHitFlags |= 0x40;
        i_this->mDamageReaction.mKnockbackPower = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->mTargetAngleY);
        break;
    case 4:
        tex_anm_set(i_this, 1);
        i_this->mDamageReaction.mKnockbackPower = 23.0f;
        i_this->mDamageReaction.mHitFlags |= 0x10;
        cMtx_YrotS(calc_mtx(), i_this->mTargetAngleY);
        break;
    case 5:
        tex_anm_set(i_this, 1);
        i_this->mDamageReaction.mHitFlags |= 0x20;
        i_this->mDamageReaction.mKnockbackPower = 23.0f;
        cMtx_YrotS(calc_mtx(), i_this->mTargetAngleY);
        anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                 dRes_INDEX_MO2_BAS_PAOMUKE_e);
        i_this->mDamageReaction.mShakeJntMask = 7;
        i_this->mDamageReaction.mHitStunTimer = 10;
        i_this->mDamageReaction.mShakeAmplitude = 5000.0f;
        if (i_this->mSpitTimer == 0) {
            i_this->mSpitTimer = 3;
            i_this->mEffectAngle.x = 0;
        }
        break;
    case 6:
        tex_anm_set(i_this, 1);
        i_this->mNoHitTimer = 200;
        i_this->mDamageReaction.mAction = ACTION_HIP_DAMAGE;
        i_this->mDamageReaction.mMode = 0;
        if (i_this->mSpitTimer == 0) {
            i_this->mSpitTimer = 10;
            i_this->mEffectAngle.x = -0x4000;
        }
        break;
    case 7:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.mHitFlags |= 0x10;
        i_this->mDamageReaction.mKnockbackPower = 26.0f;
        break;
    }
    if (i_this->mDamageReaction.mHitFlags != 0) {
        gabi::Local<cXyz> local_54;
        local_54->x = 0.0f;
        local_54->y = 0.0f;
        local_54->z = -10.0f;
        MtxPosition(local_54, &i_this->mDamageReaction.mHitVec);
        if (i_this->mDamageReaction.mKnockbackPower < 25.0f) {
            i_this->mDamageReaction.mPushback = -l_mo2HIO().mHitPushback;
        } else {
            i_this->mDamageReaction.mKnockbackPower = cM_rndF(10.0f) + 70.0f;
        }
    }
    if (iVar11 != 0) {
        if (actor->health <= 0 && atInfo->mbDead != 0) {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_FAINTED, 0);
            if (fopAcM_CheckStatus(actor, fopAcStts_BOSS_e)) {
                i_this->mDemoCamMode = 0x32;
            }
        } else if (iVar11 == 6) {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_DAMAGE_HIP, 0);
        } else {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_DAMAGE, 0);
        }
    }
    return atInfo->mDamage;
}

/* inlined into daMo2_Execute */
/* true60: event camera work on the 30 Hz grid (the event-step rule): daMo2_Execute calls it on the tick30 step */
static inline void mo2_demo_camera(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    dCam_getCamera();
    u8* camera = dCam_getBody();
    bool r28 = true;
    switch ((u8)i_this->mDemoCamMode) {
    case 0:
        r28 = false;
        break;
    case 1:
    case 2: {
        mDoAud_seStart(JA_SE_MAJU_MO_CHECK, nullptr, 0,
                       dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        gabi::Local<cXyz> dir;
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        dVibration_c* vib = dComIfGp_getVibration();
        dVibration_StartShock(reinterpret_cast<u8*>(vib), REG0_S(2) + 4, -0x21, dir);
        camera_mode() = 1;
        dCamera_SetTrimSize(camera, 1);
        i_this->mDemoCamFovy = 55.0f;
        i_this->mDemoCamCenter.copy(actor->current.pos);
        i_this->mDemoCamMode = 3;
        i_this->m2A38 = REG0_F(8) + 200.0f;
        i_this->m2A3C = 0.0f;
    }
        /* fallthrough */
    case 3:
        r28 = false;
        dCamera_SetTypeForce(camera, "Restrict", actor);
        dCamera_SetTrimTypeForce(camera, 1);
        break;
    case 10:
        dCamera_SetTrimSize(camera, 0);
        r28 = false;
        dMeter_mtrShow();
        i_this->mDemoCamMode = 0;
        camera_mode() = 0;
        break;
    case 0x32:
        if (!actor->eventInfo.checkCommandDemoAccrpt()) {
            fopAcM_orderPotentialEvent(actor, dEvtFlag_STAFF_ALL_e, 0xFFFF, 0);
            actor->eventInfo.onCondition(dEvtCnd_UNK2_e);
            r28 = false;
            break;
        }
        dCamera_Stop(camera);
        dCamera_SetTrimSize(camera, 1);
        i_this->mDemoCamMode = 0x33;
        {
            Mo2ExecCamera* camera2 = reinterpret_cast<Mo2ExecCamera*>(dComIfGp_getCamera(0));
            i_this->mDemoCamEye.copy(camera2->mLookat.mEye);    /* view.mLookat.mEye */
            i_this->mDemoCamCenter.copy(camera2->mLookat.mCenter); /* view.mLookat.mCenter */
        }
        i_this->mDemoCamFovy = 55.0f;
        i_this->mDemoCamTimer = 0;
        /* fallthrough */
    case 0x33:
        cLib_addCalc2(&i_this->mDemoCamFovy, REG0_F(0xd) + 30.0f, 0.2f, REG0_F(0xe) + 0.4f);
        if (i_this->mDefeated == 0) {
            cLib_addCalc2(&i_this->mDemoCamCenter.x, actor->current.pos.x, 0.1f, 100.0f);
            cLib_addCalc2(&i_this->mDemoCamCenter.y, actor->current.pos.y + 230.0f + REG0_F(0xc), 0.1f,
                          100.0f);
            cLib_addCalc2(&i_this->mDemoCamCenter.z, actor->current.pos.z, 0.1f, 100.0f);
        }
        i_this->mDemoCamTimer++;
        if (i_this->mDemoCamTimer > 0x96) {
            dCamera_SetTrimSize(camera, 0);
            dCamera_Start(camera);
            dMeter_mtrShow();
            dComIfGp_event_reset();
            fopAcM_delete(actor);
            return;
        }
        break;
    }
    if (r28) {
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        *center = i_this->mDemoCamCenter.get();
        *eye = i_this->mDemoCamEye.get();
        dCamera_Set(camera, center, eye, i_this->mDemoCamFovy, 0);
    }
}

/* 021C9B1C */
static BOOL daMo2_Execute(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    /* HD: a Moblin with a death switch that fell far below its home sets the switch and goes away */
    if (i_this->mDeathSwitch != 0 && actor->current.pos.y < actor->home.pos.y - 5000.0f) {
        dComIfGs_onSwitch(i_this->mDeathSwitch, fopAcM_GetRoomNo(actor));
        fopAcM_onActor(actor);
        fopAcM_delete(actor);
        if (i_this->mbHasInnateWeapon != 0) {
            fopAc_ac_c* boko = fopAcM_SearchByID(i_this->mWeaponPcId);
            if (boko != nullptr) {
                fopAcM_delete(boko);
            }
        }
        return TRUE;
    }
    /* true60: the AI state before this execute's changes (the ice thaw, the catch demo, damage_check,
     * Mo2_move, damage_reaction and its results): one state step per 30 Hz frame (mo2_t60_ai_note below) */
    const uint64_t t60AiState = mo2_t60_ai_state(i_this);
    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&actor->eyePos, 0, 0);
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }
    /* true60 (rule 14, the first-step AI-timer rule): the lighting of the draw (it draws the light flicker from cM_rnd)
     * once per frame, on the frame's first step: the original sets it before Mo2_move from the previous frame's position,
     * and its draw comes before the AI's draws of the frame */
    if (bk_t60_first())
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    enemy_fire(&i_this->mEnemyFire);
    /* true60: the timers' values on the frame's first step, before its actions (mo2_t60_count) */
    mo2_t60_count_begin(i_this);
    /* true60: the catch timer's stage change at 1 once, on the tick30 step (the timer changes only there) */
    if (i_this->mSendToPrison != 0 || (i_this->mCatchTimer == 1 && bk_t60_end())) {
        dComIfGp_setNextStage("majroom", 0, 0, -1, 0.0f, 0, 1, 0);
        return TRUE;
    }
    /* true60 (the 30 Hz timer rule): the catch timers and the catch event count / order on the tick30 step (event work
     * on the 30 Hz grid) */
    if (i_this->mCatchTimer != 0 && bk_t60_end() && mo2_t60_count(i_this, 11)) {
        i_this->mCatchTimer--;
        if (i_this->mCatchTimer == 0x1E) {
            mDoAud_seStop(JA_SE_MAJUTOU_ALERM, 30);
        }
    }
    /* HD: no m2970 / fell-4000-below-home deletion here */
    if (i_this->mCatchEventPending != 0 && bk_t60_end()) {
        if (actor->eventInfo.checkCommandDemoAccrpt()) {
            i_this->mCatchEventPending = 0;
        } else {
            fopAcM_orderPotentialEvent(actor, dEvtType_OTHER_e, 0xFFFF, 0);
            actor->eventInfo.onCondition(dEvtCnd_UNK2_e);
        }
    }
    if (i_this->mCatchDemoDelay != 0 && bk_t60_end() && mo2_t60_count(i_this, 12)) {
        i_this->mCatchDemoDelay--;
        if (i_this->mCatchDemoDelay == 0) {
            if (camera_mode() == 0) {
                i_this->mDemoCamMode = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_E3_DEMO;
            i_this->mDamageReaction.mMode = 0;
        }
    }
    if (i_this->mDamageReaction.mAction == ACTION_JYUNKAI &&
        (reinterpret_cast<Mo2ExecPlayer*>(player)->mGrabWear < 0.0f)) {
        if (i_this->mDamageReaction.mMode < 2) {
            l_mo2HIO().mSearchDist = REG0_F(10) + 800.0f;
        } else {
            l_mo2HIO().mSearchDist = REG0_F(0xb) + 1200.0f;
        }
        l_mo2HIO().mViewAngle = 0x1932;
        l_mo2HIO().mViewBox.x = 340.0f;
        l_mo2HIO().mViewBox.z = 340.0f;
        l_mo2HIO().mViewBoxBackZ = -200.0f;
    }
    u8 r23 = 0;
    if (l_mo2HIO().mFrameStep == 0 || CPad_CHECK_TRIG_B(0) || CPad_CHECK_HOLD_Y(0)) {
        /* true60 (the 30 Hz timer rule, the first-step AI-timer rule): the per-frame counters count on the frame's first step: the hit
         * gates (mNoHitTimer, mNoGuardTimer), before the collision results of the tick30 Move are read there
         * (damage_check), and the phase counter and the AI timers, before the decisions they time, so a timer-driven
         * state change falls on the first step and the new state moves on every step of the frame, as the original
         * counts, decides, then moves its whole frame */
        if (bk_t60_first()) {
            if (i_this->mNoHitTimer != 0) {
                i_this->mNoHitTimer--;
            }
            if (i_this->mNoGuardTimer != 0) {
                i_this->mNoGuardTimer--;
            }
            i_this->mFrameCounter++;
            for (s32 i = 0; i < 5; i++) {
                if (i_this->mTimers[i] != 0) {
                    i_this->mTimers[i]--;
                }
            }
            if (i_this->mSideStepTimer != 0) {
                i_this->mSideStepTimer--;
            }
            if (i_this->mSideStepWait != 0) {
                i_this->mSideStepWait--;
            }
            if (i_this->mWeaponRunTimer != 0) {
                i_this->mWeaponRunTimer--;
            }
            if (i_this->mWeaponViewWait != 0) {
                i_this->mWeaponViewWait--;
            }
            if (i_this->mDashStopTimer != 0) {
                i_this->mDashStopTimer--;
                i_this->mSmokeTimer = l_mo2HIO().mSmokeTimerBase + 4;
                i_this->mSmokeType = 4;
            }
            if (i_this->mDamageReaction.mHitStunTimer != 0) {
                i_this->mDamageReaction.mHitStunTimer--;
            }
        }
        /* the catch timers' snapshot (mo2_t60_count, slots 11 and 12) ends with the frame's tick30 step */
        if (bk_t60_end())
            mo2_t60_count_end(i_this);
        if (dComIfGs_isCollect(0, 0) || dComIfGs_isCollect(0, 1) || dComIfGs_isCollect(0, 2) ||
            dComIfGs_isCollect(0, 3)) {
            rouya_mode() = 0;
        } else {
            rouya_mode() = 1;
        }
        if (i_this->mAppearWait != 0) {
            if (dComIfGs_isSwitch(i_this->mAppearWait - 1, fopAcM_GetRoomNo(actor))) {
                i_this->mAppearWait = 0;
                actor->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            } else {
                actor->attention_info.flags = 0;
                return TRUE;
            }
        } else {
            // HD: clear the carry attention flag when no spawn switch gates the actor.
            actor->attention_info.flags &= ~fopAc_Attn_ACTION_CARRY_e;
        }
        actor->actor_status |= fopAcStts_SHOWMAP_e;
        i_this->mBlurOn = 0;
        i_this->mBlurUseB = 0;
        /* true60 (the first-step AI-timer rule): the blink counts and draws once per frame, on the first step, before
         * the frame's decisions as in the original */
        if (bk_t60_first())
            mo2_eye_tex_anm(i_this);
        i_this->mDamageReaction.m438 = 0;
        /* true60: a stop set on an in-between step (wepon_search, after this play) starts next frame:
         * the frame's remaining steps finish the frame's play, as the original played the whole frame before the set */
        /* ... an animation set before the tick30 step (anm_init, d_a_mo2.cpp) is a request: the old animation plays to
         * the frame end and the request is set after the tick30 step's play / calc, at the original's place (before
         * Mo2_move): its morf blends from the old animation's frame-end pose (d_a_mo2_t60.h) */
        if (i_this->mAnmStopTimer == 0 || bk_t60_pending(&i_this->mAnmStopTimer)) {
            i_this->mpMorf->play(&actor->eyePos, 0, 0);
        }
        if (i_this->mDamageReaction.mAction < ACTION_CARRY) {
#ifndef T60C_NEGCTRL
            mo2_t60_base_frame(i_this); /* true60 (process-order read): the calc on frame f - 1's base matrix, as A's (above) */
#endif
            mo2_t60_jnt_frame(i_this); /* true60 (process-order read): ... and on frame f - 1's joint angles (above) */
            i_this->mpMorf->calc();
            mo2_t60_jnt_restore(i_this);
        }
        mo2_t60_anm_apply(i_this);
        r23 = damage_check(i_this);
        /* true60: a state change by damage_check on an in-between step makes this step's Mo2_move the
         * frame's last dispatch call (d_a_mo2_t60.h mo2_t60_note_before_move) */
        mo2_t60_note_before_move(i_this, t60AiState);
        Mo2_move(i_this);
        /* true60: the demo camera is event work on the 30 Hz grid (the event-step rule) */
        if (bk_t60_end())
            mo2_demo_camera(i_this);
        /* true60 (the 30 Hz timer rule and 11): the smoke counts and spawns on the tick30 step; a smoke start set after
         * this point on an in-between step (the damage reaction below) starts next frame, as in the original */
        if (bk_t60_end() && !bk_t60_pending(&i_this->mSmokeTimer))
            ground_smoke_set(i_this);
        /* true60 (the 30 Hz timer rule): set before this point (wepon_search), counted on the tick30 step */
        if (i_this->mAnmStopTimer != 0 && bk_t60_end()) {
            i_this->mAnmStopTimer--;
        }
        i_this->mDamageReaction.mpEnemy = (fopEn_enemy_c*)actor;
        i_this->mDamageReaction.mEnemyType = damagereaction_l::TYPE_MOBLIN;
    }
    if (i_this->mDamageReaction.mAction != ACTION_CARRY_DROP) {
        actor->shape_angle.x = actor->current.angle.x;
        actor->shape_angle.y = actor->current.angle.y;
        actor->shape_angle.z = actor->current.angle.z;
    }
    s32 r3 = damage_reaction(&i_this->mDamageReaction);
    if (r3 != 0) {
        i_this->mSmokeTimer = 1;
        bk_t60_mark_set(&i_this->mSmokeTimer); /* true60: after this frame's smoke count */
        i_this->mHitActorId = fpcM_ERROR_PROCESS_ID_e;
        switch ((u32)r3) {
        case 1:
            anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_MO2_BAS_PAOMUKE_e);
            if (i_this->mbHasInnateWeapon != 0 &&
                (actor->health <= 0 || r23 >= 4 || cM_rndF(1.0f) < 0.5f)) {
                i_this->mSpawnWeaponActor = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            break;
        case 2:
            anm_init(i_this, dRes_INDEX_MO2_BCK_PUTSUBUSE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_MO2_BAS_PUTSUBUSE_e);
            break;
        case 5:
            if ((i_this->mMode != 1 || rouya_mode() == 0) &&
                (i_this->mDamageReaction.mAction != ACTION_FIGHT_RUN &&
                 i_this->mDamageReaction.mAction != ACTION_HUKKI &&
                 i_this->mDamageReaction.mAction != ACTION_P_LOST && i_this->mTimers[1] == 0)) {
                wait_set(i_this);
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
                i_this->mTimers[1] = 0x1e;
            }
            i_this->mSpitTimer = 5;
            i_this->mEffectAngle.x = -0x4000;
            break;
        case 10:
            wait_set(i_this);
            i_this->mDamageReaction.mMode = 2;
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            break;
        case 0x14:
            i_this->mSpitTimer = 1;
            i_this->mEffectAngle.x = -0x4000;
            i_this->mSmokeType = 1;
            i_this->mSmokeTimer = l_mo2HIO().mSmokeTimerBase + 0x10;
            bk_t60_mark_set(&i_this->mSmokeTimer); /* true60: after this frame's smoke count */
            i_this->mDamageReaction.mDownTimers[2] = l_mo2HIO().mDownRecoverDelay;
            break;
        case 0x15:
            i_this->mSpitTimer = 1;
            i_this->mEffectAngle.x = 0;
            i_this->mDamageReaction.mDownTimers[2] = l_mo2HIO().mDownRecoverDelay;
            break;
        case 0x1E:
            anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_MO2_BAS_PAOMUKE_e);
            if (i_this->mbHasInnateWeapon != 0) {
                i_this->mSpawnWeaponActor = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            if (actor != nullptr) {
                fopAcM_monsSeStart(actor, JA_SE_CV_MO_FAINTED, 0);
            }
            break;
        }
    }
    /* true60: a state entered on an in-between step waits for the next frame (Mo2_move holds its
     * dispatch) */
    mo2_t60_ai_note(i_this, t60AiState);

    f32 sc = l_mo2HIO().mModelScale;
    J3DModel* model = i_this->mpMorf->getModel();
    MtxScale(sc, sc, sc, 1);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    if (i_this->mDamageReaction.mAction >= ACTION_CARRY) {
        i_this->mpMorf->calc();
    }
    enemy_fire(&i_this->mEnemyFire);
    yari_off_check(i_this);

    gabi::Local<cXyz> local_b8;
    local_b8->y = 0.0f;
    local_b8->x = 0.0f;
    /* HD: the rotation comes before the (resetting) translation */
    cMtx_YrotM(calc_mtx(), actor->current.angle.y);
    if (i_this->mDamageReaction.mAction == ACTION_D_DOZOU) {
        MtxTrans(actor->eyePos.x, actor->current.pos.y, actor->eyePos.z, 0);
        local_b8->z = REG_F(18, 11);
    } else {
        MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, 0);
        if (i_this->mDamageReaction.mAction == ACTION_D_MAHI) {
            local_b8->z = REG_F(10, 4) + -50.0f;
        } else {
            local_b8->z = 35.0f;
        }
    }
    gabi::Local<cXyz> cStack_c4;
    MtxPosition(local_b8, cStack_c4);
    i_this->mCoCyl.SetC(cStack_c4);
    dComIfG_Ccsp_Set(&i_this->mCoCyl);
    Mo2Exec_massSet(&reinterpret_cast<Mo2ExecMassManager*>(dComIfG_Ccsp())->mMass, &i_this->mCoCyl,
                    3);
    gabi::Local<cXyz> local_d0;
    gabi::Local<cXyz> local_dc;
    f32 d0x = i_this->mSpitPos.x, d0y = i_this->mSpitPos.y, d0z = i_this->mSpitPos.z;
    f32 dcx = actor->current.pos.x, dcy = actor->current.pos.y, dcz = actor->current.pos.z;
    if (i_this->mNoHitTimer != 0) {
        d0y -= 20000.0f;
        dcy -= 20000.0f;
    }
    if (i_this->mDamageReaction.mAction == ACTION_DEFENCE) {
        dcy -= 100.0f;
        d0y -= 20000.0f;
    }
    local_d0->x = d0x;
    local_d0->y = d0y;
    local_d0->z = d0z;
    local_dc->x = dcx;
    local_dc->y = dcy;
    local_dc->z = dcz;
    i_this->mTgCyl.SetC(local_dc);
    dComIfG_Ccsp_Set(&i_this->mTgCyl);
    i_this->mHeadSph.SetC(local_d0);
    dComIfG_Ccsp_Set(&i_this->mHeadSph);
    /* true60 (the 30 Hz timer rule and 11): set before this point, counted with its particle on the tick30 step */
    if (i_this->mSpitTimer != 0 && bk_t60_end()) {
        i_this->mSpitTimer--;
        if (i_this->mSpitTimer == 0) {
            i_this->mEffectAngle.y = actor->current.angle.y;
            dComIfGp_particle_set(ID_AK_JN_TUBA00, &i_this->mSpitPos, &i_this->mEffectAngle);
        }
    }
    if (i_this->mDamageReaction.mAcch.ChkGroundHit()) {
        gabi::Local<Mo2ExecGndChk> gndChk;
        dBgS_GndChk_ct(gndChk);
        s16 r22 = 0x7FFF;
        s16 r21 = 0x7FFF;
        f32 f31 = i_this->mDamageReaction.mFallPitch != 0 ? 100.0f : 10.0f;
        f32 px = actor->current.pos.x;
        f32 py = actor->current.pos.y + (50.0f - i_this->mDamageReaction.mPivot.y);
        f32 pz = actor->current.pos.z;
        gndChk->m_pos.x = px;
        gndChk->m_pos.y = py;
        gndChk->m_pos.z = pz;
        py = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        if (py != -1000000000.0f) {
            f32 tz = pz + f31;
            f32 ty = py + 50.0f;
            gndChk->m_pos.x = px;
            gndChk->m_pos.z = tz;
            gndChk->m_pos.y = ty;
            f32 f1 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            if (f1 != -1000000000.0f) {
                r22 = -cM_atan2s(f1 - py, tz - pz);
                if (r22 > 0x2000 || r22 < -0x2000) {
                    r22 = 0;
                }
            }
            f32 tx = px + f31;
            gndChk->m_pos.y = ty;
            gndChk->m_pos.z = pz;
            gndChk->m_pos.x = tx;
            f1 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            if (f1 != -1000000000.0f) {
                r21 = cM_atan2s(f1 - py, tx - px);
                if (r21 > 0x2000 || r21 < -0x2000) {
                    r21 = 0;
                }
            }
        }
        s16 maxSpeed, speedRatio;
        if (i_this->mDamageReaction.mFallPitch != 0) {
            maxSpeed = 0x400;
            speedRatio = 1;
        } else {
            maxSpeed = 0x100;
            speedRatio = 8;
        }
        if (r22 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->mDamageReaction.mSlopeTilt.x, r22, speedRatio, maxSpeed);
        }
        if (r21 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->mDamageReaction.mSlopeTilt.z, r21, speedRatio, maxSpeed);
        }
        dBgS_GndChk_dt(gndChk);
    }
    /* true60 (the 30 Hz timer rule): the parry (Bt) counter and its window count on the tick30 step (the unconverted
     * player reads mBtNowFrame there); a counter started by attack_set on an in-between step counts in its frame, as the
     * original's set before this count */
    if (i_this->mBtCounter >= 0 && bk_t60_end()) {
        i_this->mBtCounter++;
        if (i_this->mBtCounter == i_this->mBtWindow1Start) {
            i_this->mBtWindowTimer = i_this->mBtWindow1Len;
        }
        if (i_this->mBtCounter == i_this->mBtWindow2Start) {
            i_this->mBtWindowTimer = i_this->mBtWindow2Len;
        }
        if (i_this->mBtCounter > 200) {
            i_this->mBtCounter = -1;
        }
    }
    /* setBtAttackData(0, 10, 10000, type); setBtMaxDis(mBtMaxDistance) */
    i_this->mBtStartFrame = 0.0f;
    i_this->mBtEndFrame = 10.0f;
    i_this->mBtMaxDis = 10000.0f;
    i_this->mBtAttackType = i_this->mParryOpeningType;
    i_this->mBtMaxDis = l_mo2HIO().mBtMaxDistance;
    if (i_this->mBtWindowTimer != 0) {
        if (bk_t60_end())
            i_this->mBtWindowTimer--;
        i_this->mBtNowFrame = 5.0f;
    } else {
        i_this->mBtNowFrame = 100.0f;
    }
    return TRUE;
}

/* WWHD glue
ext 021C6368 void tex_anm_set(mo2_class*, u16)
ext 021C6440 void anm_init(mo2_class*, s32, f32, u8, f32, s32)
ext 021C6578 void smoke_set_s(mo2_class*, f32)
ext 021C8E30 void wait_set(mo2_class*)
ext 021C9004 void path_check(mo2_class*)
ext 021D0A14 void Mo2_move(mo2_class*)
ext 020402C8 BOOL enemy_ice(enemyice_l*)
ext 02041570 void enemy_fire(enemyfire_l*)
ext 02041F94 s32 damage_reaction(damagereaction_l*)
ext 0252012C void dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8)
ext 025D7B24 void fopAcM_orderPotentialEvent(fopAc_ac_c*, u16, u16, u16)
ext 020078BC s32 CPad_CHECK_TRIG_B(s32)
ext 02007738 s32 CPad_CHECK_HOLD_Y(s32)
ext 02518CC8 void def_se_set(fopAc_ac_c*, void*, u32)
ext 02518DB0 void at_power_check(Mo2AtInfo*)
ext 025192A8 fopAc_ac_c* cc_at_check(fopAc_ac_c*, Mo2AtInfo*)
ext 0255F458 void dKy_Sound_set(cXyz*, s32, u32, s32)
ext 024F8020 void dCam_getCamera()
ext 024F8044 u8* dCam_getBody()
ext 02514F88 void dCamera_Set(u8*, cXyz*, cXyz*, f32, s16)
ext 02514EE4 void dCamera_SetTypeForce(u8*, const char*, fopAc_ac_c*)
ext 0251528C void dCamera_SetTrimTypeForce(u8*, s32)
ext 02515280 void dCamera_SetTrimSize(u8*, s32)
ext 02514F2C void dCamera_Stop(u8*)
ext 02514F38 void dCamera_Start(u8*)
ext 0259169C void dMeter_mtrShow()
ext 025CB374 void dVibration_StartShock(u8*, s32, s32, cXyz*)
ext 025A5F88 void Mo2Exec_endSmoke(dPa_smokeEcallBack_l*)
ext 02041C30 void Mo2Exec_enemyFireRemove(enemyfire_l*)
ext 025E1AA4 void Mo2Exec_monsSeStart(u32, cXyz*, u32, u32, s32)
ext 025E1AE0 void mDoAud_seStop(u32, s32)
ext 02516C14 void Mo2Exec_massSet(void*, void*, u32)
ext 02008E0C void Mo2Exec_gndConstruct(void*)
ext 02008DAC void Mo2Exec_gndDestroy(void*, s32)
str 10014EF4 "Restrict"
str 10015118 "majroom"
*/
