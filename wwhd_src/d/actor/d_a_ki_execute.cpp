/**
 * d_a_ki_execute.cpp (WWHD)
 * Enemy - Keese: daKi_Execute with its inlined move functions (ki_wait_move, ki_fly_move,
 * ki_fire_set_move, ki_damage_move + wall_angle_get, ki_fail_move, ki_path_move, ki_eye_tex_anm)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ki.h"
#define KI_T60_UNIT 3
#include "d/actor/d_a_ki_t60.h" /* true60 */

/* functions of d_a_ki.cpp / d_a_ki_exec.cpp (guest calls by address) */
static inline void ki_check(ki_class* i_this) { kiExecute_check(i_this); }
static inline void anm_init(ki_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float playSpeed, int soundResIdx) {
    kiExecute_animation(i_this, anmResIdx, morf, loopMode, playSpeed, soundResIdx);
}
static inline void tex_anm_set(ki_class* i_this, unsigned short idx) { kiExecute_texture(i_this, idx); }
static inline BOOL ki_player_bg_check(ki_class* i_this) { return kiExecute_playerBackground(i_this); }
static inline void ki_pos_move(ki_class* i_this, s8 arg1) { kiExecute_position(i_this, arg1); }
static inline void ki_atack_move(ki_class* i_this) { kiExecute_attack(i_this); }

// HD: named call bindings preserve this split translation unit's signatures.
static inline BOOL enemy_ice(enemyice_l* e) { return kiExecute_ice(e); }
static inline void enemy_fire(enemyfire_l* f) { kiExecute_fire(f); }
static inline void enemy_fire_remove_x(enemyfire_l* f) { kiExecute_removeFire(f); }
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) { kiExecute_report(x, y, fmt, v); }
static inline fopAc_ac_c* cc_at_check(fopAc_ac_c* a, void* info) { return kiExecute_checkAttack(a, info); }
/* kiForceDeleteEmitter: HD: the play object is fetched (unused); the emitter manager
 * is a global pointer (0x1047B2D4); 0282215C JPAEmitterManager::forceDeleteEmitter(emtr, 0) */
static inline void kiForceDeleteEmitter(u32 emtr) {
    dComIfGp_get();
    kiExecute_deleteEmitter((u32)ki_emitterManager, emtr, 0);
}
/* save info (HD, through the pointer at 0x101F84DC): life u16 +0x22, max magic u8 +0x33, magic
 * u8 +0x34, arrows u8 +0x89 */
static inline void fopAcM_onActor(fopAc_ac_c* a) { dComIfGs_onActor(a->setID, a->home.roomNo); }
#define REG8_F(i) REG_F(8, i)

// HD: strcmp(dComIfGp_getStartStageName(), "GanonK") == 0 as sead::SafeString operator==
/* inlined into daKi_Execute */
static bool ki_stage_is(const char* name) {
    SafeString a;
    a.mTextPtr = gabi::guest_addr(name);
    a.__vtbl = KI_SAFESTRING_VTABLE;
    SafeString b;
    {
        u32 stage = dComIfGp_ea() + 0x5134;
        b.__vtbl = KI_SAFESTRING_VTABLE;
        b.mTextPtr = stage;
    }
    kiAssureString(a.__vtbl, &a);
    kiAssureString(a.__vtbl, &a);
    u32 bv = b.__vtbl;
    u32 s1 = a.mTextPtr;
    kiAssureString(bv, &b);
    u32 s2 = b.mTextPtr;
    if (s1 == s2) return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = kiStringByte(s1);
        u8 c2 = kiStringByte(s2);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
        s1++;
        s2++;
    }
    return false;
}

enum {
    JA_SE_CV_KI_WAKEUP = 0x481C,
    JA_SE_CV_KI_DAMAGE = 0x481E,
    JA_SE_OBJ_TORCH_BURNING = 0x6103,
};
enum {
    dPa_ID_IT_SN_O_FIREK_KASU = 0x8061,
    dPa_ID_IT_SN_FIREK_FIRE_A = 0x8099,
    dPa_ID_IT_SN_FIREK_FIRE_B = 0x809A,
    dPa_ID_IT_SN_FIREK_HAHEN = 0x809B,
};
enum {
    daDisItem_IBALL_e = 0,
    daDisItem_HEART_e = 0xA,
    daDisItem_MAGIC_e = 0xB,
    daDisItem_ARROW_e = 0xC,
    daDisItem_NONE13_e = 0xD,
};

/* point of an HD dPath: points at +8, 0x10 bytes each (arg3 at +3, position at +4) */
static inline KiPathPoint* ki_path_point(ki_class* i_this) {
    return kiPath(i_this->ppd)->m_points.get() + (s32)i_this->mPathPoint;
}

static inline void ki_eye_tex_anm(ki_class* i_this) {
    if (i_this->mEyeAnmOn != 0) {
        if (i_this->mEyeFrame < i_this->mEyeMaxFrame) {
            i_this->mEyeFrame = i_this->mEyeFrame + 1;
        } else if (i_this->mEyeLoop != 0) {
            i_this->mEyeFrame = 0;
        } else {
            i_this->mEyeAnmOn = 0;
        }
    }
}

/* true60: the motion blocks of the dispatch functions of this unit (d_a_ki_t60.h journal: a held step
 * replays them) */
static void kiT60_posMove(ki_class* i_this, s32 arg, s32, s32) { ki_pos_move(i_this, (s8)arg); }
static void kiT60_turnShape(ki_class* i_this, s32 scale, s32 step, s32) {
    cLib_addCalcAngleS2(&i_this->actor.shape_angle.y, i_this->actor.current.angle.y, (s16)scale, (s16)step);
}
/* the wait's settle at home (behaviour 11) */
static void kiT60_settle(ki_class* i_this, s32, s32, s32) {
    fopAc_ac_c* a_this = &i_this->actor;
    cLib_addCalc2(&a_this->current.pos.x, a_this->home.pos.x, 0.5f, std::fabs((f32)a_this->speed.x) + 1.0f);
    cLib_addCalc2(&a_this->current.pos.y, a_this->home.pos.y, 0.5f, std::fabs((f32)a_this->speed.y) + 1.0f);
    cLib_addCalc2(&a_this->current.pos.z, a_this->home.pos.z, 0.5f, std::fabs((f32)a_this->speed.z) + 1.0f);
}
/* the flame's spins (behaviour 1): x and y separately (a fall start sets x back to 0) */
static void kiT60_spinX(ki_class* i_this, s32, s32, s32) { i_this->actor.shape_angle.x += t60c::share(0x1400); }
static void kiT60_spinY(ki_class* i_this, s32, s32, s32) { i_this->actor.shape_angle.y += t60c::share(0x1000); }
/* the flame's arc (the per-step movement rule: pos += v x dt, gravity x dt) */
/* (reset: the rise of behaviour 11 sets speed.y = 20 on every call, so no speed carries from step to step: no arc term;
 * the arc sets it again itself, as the case body did: a held step (the vanish the rise sets on the frame's first step
 * holds the rest of the frame) replays only this journal, not the body) */
static void kiT60_arc(ki_class* i_this, s32 reset, s32, s32) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (reset)
        a_this->speed.y = 20.0f;
    gabi::Local<cXyz> sp24; /* (guest stack: MtxPosition reads / writes them) */
    gabi::Local<cXyz> sp18;
    sp24->x = 0.0f;
    sp24->y = 0.0f;
    sp24->z = a_this->speedF;
    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    MtxPosition(sp24, sp18);

    a_this->current.pos.x += t60c::step(sp18->x);
    a_this->current.pos.y += t60c::step(a_this->speed.y);
    a_this->current.pos.z += t60c::step(sp18->z);
    if (!reset)
        ki_t60_arc_y(i_this, REG8_F(10) + 8.0f);
    /* (the rise: speed.y starts every step at 20, so it ends each step on the frame's value 20 - g, as in the original;
     * a share of g would leave a step's end value the original never had) */
    if (reset)
        a_this->speed.y -= REG8_F(10) + 8.0f;
    else
        a_this->speed.y -= t60c::step(REG8_F(10) + 8.0f);
}
/* the knockback (damage behaviour 1): `decide` = the dispatch call on the frame's first step (the wall stun) */
static bool ki_t60_knock(ki_class* i_this, bool decide);
static void kiT60_knock(ki_class* i_this, s32 decide, s32, s32) { ki_t60_knock(i_this, decide != 0); }
/* the wall stun's fall (damage behaviour 2) */
static void kiT60_fall(ki_class* i_this, s32, s32, s32) {
    fopAc_ac_c* a_this = &i_this->actor;
    cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x4000);
    if (i_this->mTimers[0] < 30) {
        a_this->current.pos.y += t60c::step(a_this->speed.y);
        ki_t60_arc_y(i_this, 0.1f);
        a_this->speed.y -= t60c::step(0.1f);
        if (i_this->mFallSpinDir == 0) {
            a_this->shape_angle.z += t60c::share(-0x250);
        } else {
            a_this->shape_angle.z += t60c::share(0x250);
        }
    }
}

/* inlined into daKi_Execute */
static void ki_wait_move(ki_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* a_this = &i_this->actor;
    cXyz sp58;

    switch (i_this->mBehaviorType) {
    case 0:
        anm_init(i_this, dRes_INDEX_KI_BCK_WAIT1_e, 10.0f, 2, 1.0f, dRes_INDEX_KI_BAS_WAIT1_e);
        i_this->mBehaviorType = 1;
        tex_anm_set(i_this, 2);
        ki_t60_ran(i_this); /* true60: the entered behaviour's body runs in this call (fall-through): not held */
        [[fallthrough]];

    case 1: {
        cXyz tmp;
        cXyz_mi(&player->current.pos, &tmp, &a_this->current.pos);
        f32 x = tmp.x, z = tmp.z;
        /* true60 (the first-step input rule addendum): Link's distance and motion decide on the tick30 step */
        if (t60c::end() && std_sqrtf(gabi::fmadds(x, x, z * z)) < i_this->mMaxAttackMoveDist300) {
            cXyz tmp2;
            cXyz_mi(&player->current.pos, &tmp2, &player->old.pos);
            sp58.copy(*&tmp2);
            if (std_sqrtf(PSVECSquareMag(&sp58)) > 1.0f) {
                i_this->mBehaviorType = 2;
                i_this->mTimers[0] = 10;
            }
        }
        break;
    }

    case 2:
        if (t60c::first() && i_this->mTimers[0] == 0) { /* true60 (first-step AI-timer) */
            i_this->mBehaviorType = 3;
            tex_anm_set(i_this, 0);
            i_this->mTimers[0] = 0x28;
            fopAcM_monsSeStart(a_this, JA_SE_CV_KI_WAKEUP, 0);
        }
        break;

    case 3: {
        i_this->mAnmPause = 1;
        /* true60 (the first-step input rule addendum): the wake-up's end reads Link's motion: tick30 step */
        if (t60c::end() && i_this->mTimers[0] == 0) {
            cXyz tmp;
            cXyz_mi(&player->current.pos, &tmp, &player->old.pos);
            sp58.copy(*&tmp);
            f32 d = std_sqrtf(PSVECSquareMag(&sp58));
            i_this->mBehaviorType = 0;
            if (d > 1.0f) {
                i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                a_this->current.angle.x = 0x4000;
            }
        }

        cXyz tmp;
        cXyz_mi(&player->current.pos, &tmp, &a_this->current.pos);
        f32 x = tmp.x, y = tmp.y, z = tmp.z;
        i_this->mHeadTargetAngleY = cM_atan2s(x, z) - a_this->current.angle.y;
        i_this->mHeadTargetAngleX = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        if (i_this->mHeadTargetAngleY > 10000) {
            i_this->mHeadTargetAngleY = 10000;
        } else if (i_this->mHeadTargetAngleY < -10000) {
            i_this->mHeadTargetAngleY = -10000;
        }

        if (i_this->mHeadTargetAngleX > 0x2000) {
            i_this->mHeadTargetAngleX = 0x1FF6;
        } else if (i_this->mHeadTargetAngleX < -0x38E) {
            i_this->mHeadTargetAngleX = -0x38E;
        }
        break;
    }

    case 10:
        i_this->mPosMove.copy(a_this->home.pos);
        i_this->mPosMoveTarget = 20.0f;
        i_this->mPosMoveMaxSpeed = 20.0f;
        i_this->mTurnScale = 10000.0f;
        i_this->mPosMoveDist = 1.0f;
        ki_t60_do(i_this, kiT60_posMove, 0);

        {
            cXyz tmp;
            cXyz_mi(&i_this->mPosMove, &tmp, &a_this->current.pos);
            sp58.copy(*&tmp);
        }
        /* true60: the arrival after the frame's motion (A's order): tick30 step */
        if (t60c::end() && std_sqrtf(PSVECSquareMag(&sp58)) < 100.0f) {
            i_this->mBehaviorType = 0xB;
            i_this->mTimers[0] = 0x32;
            anm_init(i_this, dRes_INDEX_KI_BCK_WAIT1_e, 10.0f, 2, 1.0f, dRes_INDEX_KI_BAS_WAIT1_e);
        }
        break;

    case 0xB:
        ki_t60_do(i_this, kiT60_settle);
        if (t60c::first() && i_this->mTimers[0] == 0) { /* true60 (first-step AI-timer) */
            i_this->mBehaviorType = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 70.0f);
        }
        break;
    }

    ki_t60_do(i_this, kiT60_turnShape, 1, 0x1000);
}

/* inlined into daKi_Execute */
static void ki_fly_move(ki_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* a_this = &i_this->actor;
    kiHIO_c& hio = l_kiHIO;

    i_this->mBgCheckOn = 1;

    cXyz sp28;
    cXyz_mi(&i_this->mPosMove, &sp28, &a_this->current.pos);
    f32 sx = sp28.x, sz = sp28.z;
    f32 sqrt = std_sqrtf(gabi::fmadds(sx, sx, sz * sz));

    switch (i_this->mBehaviorType) {
    case 0:
        anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 10.0f, 2, hio.mFly1AnmSpeed, dRes_INDEX_KI_BAS_FLY1_e);
        i_this->mBehaviorType = 1;
        tex_anm_set(i_this, 0);
        i_this->mPosMoveTarget = hio.mFlySpeed;
        i_this->mPosMoveMaxSpeed = 2.0f;
        i_this->mTurnScale = 3000.0f;
        ki_t60_ran(i_this); /* true60: the entered behaviour's body runs in this call (fall-through): not held */
        [[fallthrough]];

    case 1:
        /* true60 (first-step AI-timer): the wander timer, the target arrival (before the frame's motion) and the draws: first step */
        if (t60c::first() && (i_this->mTimers[1] == 0 || sqrt < 50.0f)) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            // HD: home.pos.x is kept in a register across the random call
            f32 hx = a_this->home.pos.x;
            i_this->mPosMove.x = hx;
            i_this->mPosMove.y = a_this->home.pos.y;
            i_this->mPosMove.z = a_this->home.pos.z;
            f32 r = cM_rndFX(hio.mFlyWanderRange);
            i_this->mPosMove.x = hx + r;
            i_this->mPosMove.y -= cM_rndF(200.0f);
            i_this->mPosMove.z += cM_rndFX(hio.mFlyWanderRange);
            i_this->mPosMoveDist = 0.0f;
        }

        /* true60 (the first-step input rule addendum): Link's distance decides on the tick30 step */
        if (t60c::end() && i_this->mTimers[0] == 0) {
            cXyz tmp;
            cXyz_mi(&player->current.pos, &tmp, &a_this->current.pos);
            f32 x = tmp.x, z = tmp.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < i_this->mMaxAttackMoveDist300) {
                i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                i_this->mBehaviorType = 0;
            }
        }
        break;
    }

    /* true60 (first-step AI-timer): the turn timer and the previous ground check's wall: first step */
    if (t60c::first() && i_this->mTimers[2] == 0 && i_this->mAcch.ChkWallHit()) {
        a_this->current.angle.y -= -0x8000;
        i_this->mTimers[2] = 20;
    }

    ki_t60_do(i_this, kiT60_posMove, 0);
    ki_t60_do(i_this, kiT60_turnShape, 1, 0x2000);
}

/* true60: the flame's end (after the frame's arc: Link's distance and line of sight, tick30 step), also run
 * by a held tick30 step whose frame's dispatch was ki_fire_set_move */
static void ki_t60_fire_end(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (t60c::end() && fopAcM_searchPlayerDistance(a_this) > 2000.0f && ki_player_bg_check(i_this)) {
        i_this->mVanish = 1;
    }
}

/* inlined into daKi_Execute */
static void ki_fire_set_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */

    i_this->mBgCheckOn = 0;

    switch (i_this->mBehaviorType) {
    case 0: {
        f32 base = REG8_F(8) + 35.0f;
        a_this->speedF = base + cM_rndF(10.0f);
        f32 base2 = REG8_F(9) + 97.0f;
        a_this->speed.y = base2 + cM_rndF(30.0f);
        anm_init(i_this, dRes_INDEX_KI_BCK_WAIT1_e, 1.0f, 2, 1.0f, dRes_INDEX_KI_BAS_WAIT1_e);
        i_this->mBehaviorType = 1;
        i_this->mFireEmitter = dComIfGp_particle_set(dPa_ID_IT_SN_FIREK_FIRE_A, &a_this->current.pos, nullptr, nullptr, 0xFF,
                                             (dPa_levelEcallBack*)&i_this->mFireCallBack);
    ki_t60_ran(i_this); /* true60: the entered behaviour's body runs in this call (fall-through): not held */
    }
        [[fallthrough]];

    case 1:
        ki_t60_do(i_this, kiT60_spinX);
        ki_t60_do(i_this, kiT60_spinY);
        /* true60 (first-step AI-timer): the fall start, measured before the frame's arc: first step */
        if (t60c::first() && a_this->speed.y < REG8_F(11) + -70.0f) {
            i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
            i_this->mBehaviorType = 0;
            a_this->shape_angle.x = 0;
            ki_t60_undo(i_this, kiT60_spinX); /* true60: the frame ends with x = 0 */
            if (i_this->mFireEmitter != nullptr) {
                kiRemoveFollow(&i_this->mFireCallBack); /* mFireCallBack.remove() */
                kiForceDeleteEmitter(gabi::ea(i_this->mFireEmitter.get()));
                i_this->mFireEmitter = nullptr;
            }
            dComIfGp_particle_set(dPa_ID_IT_SN_FIREK_FIRE_B, &a_this->current.pos);
            dComIfGp_particle_set(dPa_ID_IT_SN_FIREK_HAHEN, &a_this->current.pos);
        }
        break;

    case 10:
        anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 10.0f, 2, l_kiHIO.mFly1AnmSpeed, dRes_INDEX_KI_BAS_FLY1_e);
        i_this->mBehaviorType = 0xB;
        i_this->mTimers[1] = 100;
        ki_t60_ran(i_this); /* true60: the entered behaviour's body runs in this call (fall-through): not held */
        [[fallthrough]];

    case 11:
        i_this->mBgCheckOn = 1;
        a_this->speed.y = 20.0f;
        if (t60c::first() && i_this->mTimers[1] == 0) { /* true60 (first-step AI-timer) */
            i_this->mVanish = 1;
        }
        break;
    }

    ki_t60_do(i_this, kiT60_arc, i_this->mBehaviorType == 0xB ? 1 : 0);
    ki_t60_fire_end(i_this);
}

/* wall_angle_get (inlined in ki_damage_move) */
/* inlined into daKi_Execute */
static inline s16 wall_angle_get(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    KiLineCheck linChk;
    ki_LinChk_ct(&linChk);
    cXyz sp2C0;
    cXyz sp2C1;
    cXyz* sp2C[2] = {&sp2C0, &sp2C1};
    cXyz sp20;
    cXyz sp14;

    cMtx_YrotS(calc_mtx(), a_this->shape_angle.y);
    sp20.x = 0.0f;
    sp20.y = 0.0f;
    sp20.z = 100.0f;
    MtxPosition(&sp20, &sp14);
    PSVECAdd(&sp14, &a_this->current.pos, &sp14);
    sp20.x = 10.0f;
    sp20.y = 0.0f;
    sp20.z = -300.0f;

    for (s32 i = 0; i < 2; i++) {
        MtxPosition(&sp20, sp2C[i]);
        sp20.x = -sp20.x;
        PSVECAdd(sp2C[i], &sp14, sp2C[i]);

        dBgS_LinChk_Set(&linChk, &sp14, sp2C[i], a_this);

        if (cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
            sp2C[i]->copy(*kiLineCross(&linChk)); /* linChk.GetCross() */
        } else {
            ki_LinChk_dt(&linChk);
            return TRUE;
        }
    }

    cXyz tmp;
    cXyz_mi(sp2C[1], &tmp, sp2C[0]);
    f32 x = tmp.x, z = tmp.z;
    s16 ret = cM_atan2s(x, z) + 0x4000;
    ki_LinChk_dt(&linChk);
    return ret;
}

/* true60: the knockback block of ki_damage_move behaviour 1 (the original statements; returns the wall
 * stun, taken only by the dispatch call on the frame's first step) */
static bool ki_t60_knock(ki_class* i_this, bool decide) {
    fopAc_ac_c* a_this = &i_this->actor;
    bool stunned = false;
    cLib_addCalcAngleS2(&a_this->shape_angle.y, i_this->mKnockbackAngleY, 1, 0x3000);
    gabi::Local<cXyz> sp1C; /* (guest stack: MtxPosition reads / writes them) */
    sp1C->x = 0.0f;
    sp1C->y = 0.0f;
    sp1C->z = -i_this->mKnockbackSpeed;
    cMtx_YrotS(calc_mtx(), i_this->mKnockbackAngleY);
    cMtx_XrotM(calc_mtx(), i_this->mRand2000);

    if (i_this->mAcch.ChkWallHit()) {
        sp1C->z = 0.0f;
        /* true60 (first-step AI-timer): the wall stun on the frame's first step (the previous ground check's wall) */
        if (decide && t60c::first() && !(i_this->mKnockbackSpeed < l_kiHIO.mWallStunSpeed)) { /* mKnockbackSpeed >= mWallStunSpeed (blt: NaN enters) */
            i_this->mBehaviorType = 2;
            i_this->mTimers[0] = 0x32;
            anm_init(i_this, dRes_INDEX_KI_BCK_BITA1_e, 1.0f, 0, 1.0f, dRes_INDEX_KI_BAS_BITA1_e);
            tex_anm_set(i_this, 3);
            a_this->speed.y = 0.0f;
            /* csXyz shapeAngle = shape_angle; shapeAngle.y -= -0x8000;  (unused) */
            s16 sVar2 = wall_angle_get(i_this);
            if (sVar2 != 1) {
                a_this->current.angle.y = sVar2;
            }
            i_this->mFallSpinDir = (u8)gabi::ftoi(cM_rndF(1.9f));
            stunned = true;
        }
    }

    gabi::Local<cXyz> sp10;
    MtxPosition(sp1C, sp10);
    a_this->current.pos.x += t60c::step(sp10->x); /* true60 (the per-step movement rule) */
    a_this->current.pos.y += t60c::step(sp10->y);
    a_this->current.pos.z += t60c::step(sp10->z);
    cLib_addCalc0(&i_this->mKnockbackSpeed, 1.0f, l_kiHIO.mKnockbackDecay);
    a_this->speedF = 0.0f;
    return stunned;
}

/* inlined into daKi_Execute */
static void ki_damage_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */
    bool bVar1 = false;

    i_this->mBgCheckOn = 1;
    i_this->mNoHitTimer = 3;

    switch (i_this->mBehaviorType) {
    case 0:
        anm_init(i_this, dRes_INDEX_KI_BCK_DAMAGE1_e, 2.0f, 0, 1.0f, dRes_INDEX_KI_BAS_DAMAGE1_e);
        tex_anm_set(i_this, 3);
        i_this->mBehaviorType = 1;
        ki_t60_ran(i_this); /* true60: the entered behaviour's body runs in this call (fall-through): not held */
        [[fallthrough]];

    case 1:
        /* true60 (first-step AI-timer): the knockback's end is measured before the frame's motion: first step; the later steps of the
         * frame keep moving */
        if (i_this->mKnockbackSpeed > 0.1f || !t60c::first()) {
            const s8 t60Beh = i_this->mBehaviorType;
            ki_t60_do(i_this, kiT60_knock, 1); /* (decides on the first step only: a replay never does) */
            if (i_this->mBehaviorType != t60Beh)
                bVar1 = true;
        } else {
            i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
            i_this->mBehaviorType = 0;
            bVar1 = true;
        }
        break;

    case 2:
        ki_t60_do(i_this, kiT60_fall);

        if (t60c::first() && i_this->mTimers[0] == 0) { /* true60 (first-step AI-timer) */
            i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
            i_this->mBehaviorType = 0;
        }
        break;
    }

    if (bVar1 && a_this->health <= 0) {
        i_this->mAction = ki_class::ACT_FAIL_MOVE_e;
        i_this->mBehaviorType = 0;
    }
}

static void ki_fail_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    u8 dropType;

    /* true60 (the first-step frame-level rule form): the death drop and the delete are frame logic, once per frame (the delete takes
     * effect at the next frame's handlers: a later step of this frame would drop again) */
    if (!t60c::first())
        return;

    if (ki_stage_is("GanonK")) {
        if (a_this->health != -10) {
            if (dComIfGs_getLife() <= 8) {
                dropType = daDisItem_HEART_e;
            } else if (cM_rndF(1.0f) < 0.5f) {
                if (dComIfGs_getArrowNum() == 0) {
                    dropType = daDisItem_ARROW_e;
                } else if (dComIfGs_getMagic() < dComIfGs_getMaxMagic() / 2) {
                    dropType = daDisItem_MAGIC_e;
                } else if (dComIfGs_getArrowNum() < 10) {
                    dropType = daDisItem_ARROW_e;
                } else {
                    /* static u8 item_tbl[] = {HEART, MAGIC, ARROW, HEART} (0x101B8164) */
                    dropType = ki_item_tbl[gabi::ftoi(cM_rndF(2.99f))];
                }
            } else {
                dropType = daDisItem_NONE13_e;
            }
        } else {
            dropType = daDisItem_NONE13_e;
        }
        fopAcM_createDisappear(a_this, &a_this->current.pos, 5, dropType, 0xFF);
    } else {
        fopAcM_createDisappear(a_this, &a_this->current.pos, 5, daDisItem_IBALL_e, 0xFF);
    }
    fopAcM_delete(a_this);
    fopAcM_onActor(a_this);
}

/* true60: the path's end (after the frame's motion: Link's distance, the first-step input rule addendum, tick30 step),
 * also run by a held tick30 step whose frame's dispatch was ki_path_move */
static void ki_t60_path_end(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (t60c::end() && fopAcM_searchPlayerDistance(a_this) < i_this->mMaxAttackMoveDist300) {
        i_this->mCurrKiPathIndex = 0;
        i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
        i_this->mBehaviorType = 0;
    }
}

static void ki_path_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */
    kiHIO_c& hio = l_kiHIO;

    switch (i_this->mBehaviorType) {
    case 0:
        i_this->mPathPoint += i_this->mPathDir;
        /* HD dPath: m_num u16 at +0 (compared as s8), m_nextID u16 at +2, closed flag at +5 bit 0 */
        if (i_this->mPathPoint >= kiPath(i_this->ppd)->mCountBytes[1]) {
            KiPath* ppd = kiPath(i_this->ppd);
            if ((u8)ppd->m_closed & 1) {
                i_this->mPathPoint = 0;
            } else {
                i_this->mPathDir = -1;
                i_this->mPathPoint = (u16)kiPath(i_this->ppd)->m_num - 2;
            }

            if ((s32)(u16)ppd->m_nextID != 0xFFFF) {
                i_this->ppd = dPath_GetRoomPath((u16)ppd->m_nextID, fopAcM_GetRoomNo(a_this));
                if (i_this->ppd == nullptr) /* JUT_ASSERT(1779, i_this->ppd != NULL) */
                    JUT_ASSERT_fail("d_a_ki.cpp", 0x6F3, "i_this->ppd != (0)");
            }
        } else if (i_this->mPathPoint < 0) {
            i_this->mPathDir = 1;
            i_this->mPathPoint = 1;
        }
        [[fallthrough]];

    case -1: {
        i_this->mBehaviorType = 1;
        KiPathPoint* point = ki_path_point(i_this);
        i_this->mPosMoveTarget = cM_rndF(5.0f) + 30.0f;
        i_this->mPosMoveMaxSpeed = REG0_F(13) + 1.0f;
        i_this->mPosMoveDist = REG0_F(7);
        i_this->mPosMove.x = point->m_position.x;
        i_this->mPosMove.y = point->m_position.y;
        i_this->mPosMove.z = point->m_position.z;
        f32 r = cM_rndFX(150.0f);
        i_this->mPosMove.x = point->m_position.x + r;
        r = cM_rndFX(150.0f);
        i_this->mPosMove.y = point->m_position.y + r;
        r = cM_rndFX(150.0f);
        i_this->mPosMove.z = point->m_position.z + r;
        break;
    }
    case 1: {
        f32 x = i_this->mPosMove.x - a_this->current.pos.x;
        f32 y = i_this->mPosMove.y - a_this->current.pos.y;
        f32 z = i_this->mPosMove.z - a_this->current.pos.z;

        /* true60 (first-step AI-timer): the point arrival, measured before the frame's motion: first step */
        if (t60c::first() && std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < gabi::fmadds(REG0_F(10), 10.0f, 200.0f)) {
            i_this->mBehaviorType = 0;
            if (ki_path_point(i_this)->mArg3 == 6) {
                fopAcM_delete(a_this);
            }
        }
        break;
    }
    }

    i_this->mPosMoveTarget = hio.mFlySpeed;
    i_this->mPosMoveMaxSpeed = 2.0f;
    i_this->mTurnScale = 4000.0f;
    i_this->mPosMoveDist = 1.0f;

    ki_t60_do(i_this, kiT60_posMove, 0);

    ki_t60_do(i_this, kiT60_turnShape, 1, 0x2000);
    ki_t60_path_end(i_this);
}

/* true60 (the one-sweep-per-frame attack rule, the Co push mask): the attack sphere's At and the Co set bits of the Co and damage
 * spheres are met by the tick30 collision Move only (one meeting per 30 Hz frame, At and Co together); on an in-between
 * step they are cleared for that step's Move and set again at the next execute's start, before any code reads them.
 * The damage sphere's Tg stays registered every step. dt = 1 and the tick30 step: nothing. (Host table of this unit.) */
template <int U> struct KiT60Mask {
    enum { kSize = 64 };
    struct Ent {
        u32 ea, bits;
    };
    static Ent s_ent[kSize];
    static Ent* find(u32 ea, bool make) {
        Ent* free_slot = 0;
        for (u32 k = 0; k < kSize; k++) {
            Ent& e = s_ent[(ea / 16 + k) & (kSize - 1)];
            if (e.ea == ea)
                return &e;
            if (!free_slot && e.ea == 0)
                free_slot = &e;
        }
        if (make && free_slot)
            free_slot->ea = ea, free_slot->bits = 0;
        return make ? free_slot : 0;
    }
};
template <int U> typename KiT60Mask<U>::Ent KiT60Mask<U>::s_ent[KiT60Mask<U>::kSize];
static inline void ki_t60_cc_mask(ki_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    u32 m = 0;
    if (i->mAtSph.mObjAt.mSPrm & 1)
        i->mAtSph.OffAtSPrmBit(1), m |= 1;
    if (i->mCoSph.mObjCo.mSPrm & 1)
        i->mCoSph.OffCoSPrmBit(1), m |= 2;
    if (i->mDamageSphere.mObjCo.mSPrm & 1)
        i->mDamageSphere.OffCoSPrmBit(1), m |= 4;
    if (i->mDamageSphere.mObjTg.mSPrm & 1)
        i->mDamageSphere.OffTgSPrmBit(1), m |= 8;
    if (m == 0)
        return;
    typename KiT60Mask<0>::Ent* e = KiT60Mask<0>::find(gabi::ea(i), true);
    if (e)
        e->bits = m;
#else
    (void)i;
#endif
}
static inline void ki_t60_cc_restore(ki_class* i) {
    typename KiT60Mask<0>::Ent* e = KiT60Mask<0>::find(gabi::ea(i), false);
    if (!e)
        return;
    const u32 m = e->bits;
    e->ea = 0, e->bits = 0;
    if (m & 1)
        i->mAtSph.OnAtSPrmBit(1);
    if (m & 2)
        i->mCoSph.OnCoSPrmBit(1);
    if (m & 4)
        i->mDamageSphere.OnCoSPrmBit(1);
    if (m & 8)
        i->mDamageSphere.OnTgSPrmBit(1);
}
/* the first-step push rule: the push of the tick30 collision Move (the collider status's cc move, read on the frame's first step)
 * is held over the frame's steps, dt of it per step: the frame's total is the original's one push. dt = 1: the live
 * push. (Host table of this unit, entries of an older frame are dead.) */
template <int U> struct KiT60Push {
    enum { kSize = 64 };
    struct Ent {
        u32 ea, tag;
        f32 v[3];
    };
    static Ent s_ent[kSize];
};
template <int U> typename KiT60Push<U>::Ent KiT60Push<U>::s_ent[KiT60Push<U>::kSize];
static inline void ki_t60_push(ki_class* i, f32* x, f32* y, f32* z) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    const u32 ea = gabi::ea(i), tag = bk_t60_frame_tag();
    typename KiT60Push<0>::Ent* slot = 0;
    for (u32 k = 0; k < (u32)KiT60Push<0>::kSize; k++) {
        typename KiT60Push<0>::Ent& e = KiT60Push<0>::s_ent[(ea / 16 + k) & (KiT60Push<0>::kSize - 1)];
        if (e.ea == ea) {
            slot = &e;
            break;
        }
        if (!slot && (e.ea == 0 || e.tag != tag))
            slot = &e;
    }
    if (slot && (t60::first_step() || slot->ea != ea || slot->tag != tag)) {
        slot->ea = ea, slot->tag = tag;
        slot->v[0] = *x, slot->v[1] = *y, slot->v[2] = *z;
    }
    if (slot) {
        *x = slot->v[0], *y = slot->v[1], *z = slot->v[2];
    }
#endif
    *x = t60c::step(*x), *y = t60c::step(*y), *z = t60c::step(*z);
}

/* 0219AAE8 */
static BOOL daKi_Execute(ki_class* i_this) {
    /* true60 (the whole-frame form rule addendum): the whole frame on the tick30 step
     * (d_a_ki_t60.h; end at every return) */
    t60w::Guard<ki_class> t60g(i_this, ki_t60w::ops());
    if (t60g.skip) /* a step before the tick30 step: the picture only (frame interpolation) */
        return TRUE;
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */
    kiHIO_c& hio = l_kiHIO;

    a_this->model = 0;

    /* true60 (d_a_ki_t60.h): the masked At / Co bits back, the step stamp (the process-order read rule), the state this step
     * starts in, the hold (the state changed on an earlier step of this frame) and the frame's starting vanish */
    ki_t60_cc_restore(i_this);
    ki_t60_fs_stamp(i_this);
    const u32 t60Entry = ki_t60_state(i_this);
    const bool t60Held = ki_t60_held(i_this);
    const bool t60FrameVanish = t60c::heldInt(i_this->mVanish != 0 ? 1 : 0, 1, i_this) != 0;

    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&a_this->eyePos, 0, 0);
        J3DModel* model = i_this->mpMorf->getModel();
        t60g.model = true; /* true60 (whole form): the matrix step */
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }

    /* true60 (the per-step chance rule, the Bokoblin): the lighting draws the light flicker from cM_rnd: once per
     * frame, on the first step before the dispatch as in the original (on the tick30 step the first-step lunge chance took
     * the flicker's keyed value: pair) */
    if (t60c::first())
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    /* true60: a vanish set on an earlier step of this frame starts shrinking next frame (the hold) */
    if (i_this->mVanish != 0 && (!t60Held || t60FrameVanish)) {
        cLib_addCalc0(&a_this->scale.x, 1.0f, 0.05f);
        f32 s = a_this->scale.x;
        a_this->scale.z = s;
        a_this->scale.y = s;
        if (!(s > 0.04f)) {
            fopAcM_delete(a_this);
            return TRUE;
        }
    }

    if (i_this->mAppearSwitch != 0) {
        if (dComIfGs_isSwitch(i_this->mAppearSwitch - 1, fopAcM_GetRoomNo(a_this))) {
            i_this->mAppearSwitch = 0;
            a_this->speedF = hio.mFlySpeed;
            a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e; /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        } else {
            // HD: no fopAcM_OnStatus(0) / OffStatus(0)
            a_this->attention_info.flags = 0;
            return TRUE;
        }
    }

    a_this->actor_status |= fopAcStts_SHOWMAP_e; /* fopAcM_OnStatus(fopAcStts_SHOWMAP_e) */

    if (hio.mDebugReport != 0) {
        ki_check(i_this);
        JUTReport(0x104, 0x186, "ALL      %d", ki_all_count);
        JUTReport(0x104, 0x19A, "READY    %d", ki_all_count - ki_fight_count);
        JUTReport(0x104, 0x1AE, "FIGHT    %d", ki_fight_count);
    }

    /* true60: a held step keeps the radii (and below the ground check flag) the frame's dispatch call set */
    if (!t60Held) {
        i_this->mAtSph.SetR(0.0f);
        i_this->mDamageSphere.SetR(40.0f);
        i_this->mCoSph.SetR(hio.mCoSphRadius);
    }

    if (hio.mDebugPause == 0) {
        if (!t60Held)
            i_this->mBgCheckOn = 0;

        /* true60 (the first-step AI-timer rule): the AI timers, the hit gate and the circling phase count on the frame's first step */
        if (t60c::first()) {
            for (s32 i = 0; i < 4; i++) {
                if (i_this->mTimers[i] != 0) {
                    i_this->mTimers[i] -= 1;
                }
            }

            if (i_this->mNoHitTimer != 0) {
                i_this->mNoHitTimer -= 1;
            }

            i_this->mFrameCounter += 1;
        }

        /* true60: the wake-up pause holds the frame's play (taken on the first step); an animation set before the tick30
         * step is a request: the old animation plays to the frame end, the request is set on the tick30 step after this
         * play, before the dispatch (d_a_ki_t60.h ki_t60_anm_apply) */
        const bool t60Pause = t60c::held(i_this->mAnmPause != 0, 2, i_this);
        if (!t60Pause) {
            i_this->mpMorf->play(&a_this->current.pos, 0, 0);
        } else if (t60c::first()) {
            i_this->mAnmPause = 0;
        }
        ki_t60_anm_apply(i_this);

        if (t60Held) {
            /* true60: the state changed on an earlier step of this frame: the frame's dispatch motion again (each
             * unit replays its own journal; the attack is in d_a_ki_exec.cpp), and on the tick30 step the decisions
             * that follow that call's motion */
            ki_atack_move(i_this);
            const s32 t60Fn = ki_t60_jr_id(i_this);
            ki_t60_jr_replay(i_this);
            if (t60::tick30() && t60Fn == 1)
                ki_t60_path_end(i_this);
            else if (t60::tick30() && t60Fn == 3)
                ki_t60_fire_end(i_this);
        } else {
        /* (the journal id: 1 path, 2 wait, 3 flame, 4 fly, 5 damage, 6 fail; 0 the attack in d_a_ki_exec.cpp) */
        ki_t60_jr_begin(i_this, i_this->mAction == ki_class::ACT_ATTACK_MOVE_e ? 0
                                : i_this->mCurrKiPathIndex != 0                   ? 1
                                : i_this->mAction == ki_class::ACT_WAIT_MOVE_e     ? 2
                                : i_this->mAction == ki_class::ACT_FIRE_SET_MOVE_e ? 3
                                : i_this->mAction == ki_class::ACT_FLY_MOVE_e      ? 4
                                : i_this->mAction == ki_class::ACT_DAMAGE_MOVE_e   ? 5
                                : i_this->mAction == ki_class::ACT_FAIL_MOVE_e     ? 6
                                                                                   : 0);
        if (i_this->mAction == ki_class::ACT_ATTACK_MOVE_e) {
            ki_atack_move(i_this);
        } else if (i_this->mCurrKiPathIndex != 0) {
            ki_path_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_WAIT_MOVE_e) {
            ki_wait_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_FIRE_SET_MOVE_e) {
            ki_fire_set_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_FLY_MOVE_e) {
            ki_fly_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_ATTACK_MOVE_INDEX_e) {
            ki_atack_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_DAMAGE_MOVE_e) {
            ki_damage_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_FAIL_MOVE_e) {
            ki_fail_move(i_this);
        }
        }
        if (t60c::end()) /* true60: the eye pattern frame (visual) once per frame, on the tick30 step */
            ki_eye_tex_anm(i_this);
    }

    if (i_this->mDamageType != 0) {
        mDoExt_baseAnm_play(i_this->mBtkAnm);
        /* true60 (the sound-gate rule, 11): the burning smoke and sound once per frame, on the tick30 step */
        if (t60c::end() && i_this->mAction != ki_class::ACT_FAIL_MOVE_e) {
            dComIfGp_particle_setSimple(dPa_ID_IT_SN_O_FIREK_KASU, &a_this->current.pos);
            fopAcM_seStart(a_this, JA_SE_OBJ_TORCH_BURNING, 0);
        }
    }

    cXyz* ccMoveP = &i_this->mStts.m_cc_move; /* mStts.GetCCMoveP() (GHS null-checks the address) */
    if (gabi::ea(&i_this->mStts) != 0) {
        /* operand order as the recompiled original computes it (matters only for NaN payloads,
         * which an integer copy to eyePos makes observable) */
        f32 t60Px = ccMoveP->x, t60Py = ccMoveP->y, t60Pz = ccMoveP->z;
        ki_t60_push(i_this, &t60Px, &t60Py, &t60Pz); /* true60 (the first-step push rule): the frame's held push, dt per step */
        a_this->current.pos.x = t60Px + a_this->current.pos.x;
        a_this->current.pos.y = a_this->current.pos.y + t60Py;
        a_this->current.pos.z = t60Pz + a_this->current.pos.z;
    }

    MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), a_this->shape_angle.y);
    cMtx_XrotM(calc_mtx(), a_this->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), a_this->shape_angle.z);

    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &a_this->scale);
    t60g.model = true; /* true60 (whole form): the matrix step */
    J3DModel_setBaseTRMtx(model, calc_mtx());
    i_this->mpMorf->calc();
    enemy_fire(&i_this->mEnemyFire);

    a_this->eyePos.copy(a_this->current.pos);
    cXyz* attnPos = &a_this->attention_info.position; /* attention_info.position */
    attnPos->x = a_this->eyePos.x;
    attnPos->y = a_this->eyePos.y;
    attnPos->z = a_this->eyePos.z;
    attnPos->y += 50.0f;

    i_this->mAtSph.mSph.SetC(&a_this->current.pos);
    i_this->mCoSph.mSph.SetC(&a_this->current.pos);
    i_this->mDamageSphere.mSph.SetC(&a_this->current.pos);

    ki_t60_cc_mask(i_this); /* true60 (the one-sweep-per-frame attack rule): At and Co for the tick30 Move only */
    dComIfG_Ccsp_Set(&i_this->mAtSph);
    dComIfG_Ccsp_Set(&i_this->mCoSph);
    dComIfG_Ccsp_Set(&i_this->mDamageSphere);

    if (i_this->mBgCheckOn != 0) {
        a_this->current.pos.y -= REG0_F(5) + 30.0f;
        a_this->old.pos.y -= REG0_F(5) + 30.0f;
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        a_this->current.pos.y += REG0_F(5) + 30.0f;
        a_this->old.pos.y += REG0_F(5) + 30.0f;
    }

    /* true60 (the mixed-pair hit-flag rule, first-step AI-timer order addendum): the collider status shift and the hit reaction come after the frame's
     * dispatch and motion in A's call: on the tick30 step, the tick30 Move's hit flags kept (the damage sphere's Tg is
     * masked on the in-between Moves); the reaction's state starts next frame, as in A */
    if (t60c::end())
        i_this->mStts.Move();
    if (t60c::end() && i_this->mDamageSphere.ChkTgHit() && i_this->mNoHitTimer == 0) {
        i_this->mNoHitTimer = 10;

        KiAttackInfo atInfo;
        void* obj = i_this->mDamageSphere.GetTgHitObj();
        atInfo.mpObj = gabi::ea(obj);
        atInfo.pParticlePos = i_this->mDamageSphere.GetTgHitPosP();

        if (cCcD_Obj_ChkAtType(obj, KiAttackType::LIGHT_ARROW | KiAttackType::ICE_ARROW)) {
            if (cCcD_Obj_ChkAtType(obj, KiAttackType::ICE_ARROW)) {
                i_this->mEnemyIce.mFreezeDuration = REG0_S(3) + 300;
                i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                i_this->mBehaviorType = 0;
            } else {
                i_this->mEnemyIce.mLightShrinkTimer = 1;
            }
            enemy_fire_remove_x(&i_this->mEnemyFire);
            ki_t60_note(i_this, t60Entry); /* true60: a state entered on a non-tick30 step holds the frame */
            return TRUE;
        }

        if (cCcD_Obj_ChkAtType(obj, KiAttackType::FIRE_ARROW | KiAttackType::FIRE)) {
            i_this->mEnemyFire.mFireDuration = REG0_S(2) + 100;
            i_this->mNoHitTimer = 0x32;
            i_this->mAtSph.OnAtSPrmBit(2 /* cCcD_AtSPrm_VsEnemy_e */);
        }

        if (cCcD_Obj_ChkAtType(obj, KiAttackType::WIND)) {
            i_this->mKnockbackSpeed = 100.0f;
        } else {
            atInfo.mpActor = cc_at_check(a_this, &atInfo);

            fopAc_ac_c* ac = atInfo.mpActor;
            if (ac != nullptr) {
                if (fpcM_GetName(ac) == fpcNm_BGN_e || fpcM_GetName(ac) == fpcNm_BGN2_e ||
                    fpcM_GetName(ac) == fpcNm_BGN3_e) {
                    a_this->health = -10;
                }
            }

            if (hio.mDebugNoDeath != 0) {
                a_this->health = 10;
            }

            if (atInfo.mResultingAttackType == 9) {
                i_this->mKnockbackSpeed = 150.0f;
            } else if (i_this->mStts.mAtSpl == 1 /* dCcG_At_Spl_UNK1 */) {
                i_this->mKnockbackSpeed = hio.mHitKnockbackSpl;
            } else {
                i_this->mKnockbackSpeed = hio.mHitKnockback;
            }
        }

        i_this->mKnockbackAngleY = fopAcM_searchPlayerAngleY(a_this);
        i_this->mRand2000 = (s16)gabi::ftoi(cM_rndF(2000.0f));
        i_this->mAction = ki_class::ACT_DAMAGE_MOVE_e;
        i_this->mBehaviorType = 0;

        fopAcM_monsSeStart(a_this, JA_SE_CV_KI_DAMAGE, 0);
    }

    s16 tmp;
    if (i_this->mHeadTargetAngleY != 0) {
        tmp = 0x1000;
    } else {
        tmp = 0x200;
    }
    cLib_addCalcAngleS2(&i_this->mHeadAngleY, i_this->mHeadTargetAngleY, 5, tmp);

    if (i_this->mHeadTargetAngleX != 0) {
        tmp = 0x1000;
    } else {
        tmp = 0x200;
    }
    cLib_addCalcAngleS2(&i_this->mHeadAngleX, i_this->mHeadTargetAngleX, 5, tmp);

    i_this->mHeadTargetAngleX = 0;
    i_this->mHeadTargetAngleY = 0;
    cLib_addCalcAngleS2(&a_this->shape_angle.z, 0, 1, 0x200);
    ki_t60_note(i_this, t60Entry); /* true60: a state entered on a non-tick30 step holds the frame */
    return TRUE;
}

/* WWHD glue
str 10012C34
str 10012C40
str 10012D2C in 0219AAE8
str 10012D38 in 0219AAE8
str 10012D44 in 0219AAE8
str 10012B54
ext 02199364 void kiExecute_check(ki_class* actor)
ext 02199388 void kiExecute_animation(ki_class* actor, int resource, f32 blend, u8 loop, f32 speed, int sound)
ext 021994B0 void kiExecute_texture(ki_class* actor, u16 index)
ext 021995BC BOOL kiExecute_playerBackground(ki_class* actor)
ext 02199A64 void kiExecute_position(ki_class* actor, s8 mode)
ext 02199C48 void kiExecute_attack(ki_class* actor)
ext 02041C30 void kiExecute_removeFire(enemyfire_l* fire)
ext 020402C8 BOOL kiExecute_ice(enemyice_l* ice)
ext 02041570 void kiExecute_fire(enemyfire_l* fire)
ext 027EC9E8 void kiExecute_report(s32 x, s32 y, const char* text, s32 count)
ext 025192A8 fopAc_ac_c* kiExecute_checkAttack(fopAc_ac_c* actor, void* info)
ext 0282215C void kiExecute_deleteEmitter(u32 manager, u32 emitter, s32 mode)
*/
