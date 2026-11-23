/**
 * d_a_ki_exec.cpp (WWHD)
 * Enemy - Keese: ki_atack_move and daKi_Execute (with its inlined move functions)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ki.h"
#define KI_T60_UNIT 2
#include "d/actor/d_a_ki_t60.h" /* true60 */

/* functions of d_a_ki.cpp (guest calls by address) */
static inline void ki_check(ki_class* i_this) { kiAttack_check(i_this); }
static inline void anm_init(ki_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float playSpeed, int soundResIdx) {
    kiAttack_animation(i_this, anmResIdx, morf, loopMode, playSpeed, soundResIdx);
}
static inline void tex_anm_set(ki_class* i_this, unsigned short idx) { kiAttack_texture(i_this, idx); }
static inline BOOL ki_player_bg_check(ki_class* i_this) { return kiAttack_playerBackground(i_this); }
static inline void ki_pos_move(ki_class* i_this, s8 arg1) { kiAttack_position(i_this, arg1); }

/* sp = a + b */
static inline void cXyz_add_to(cXyz* a, const cXyz* b) { PSVECAdd(a, b, a); }

/* true60: the motion blocks of ki_atack_move (d_a_ki_t60.h journal: a held step replays them) */
static void kiT60_posMove(ki_class* i_this, s32 arg, s32, s32) { ki_pos_move(i_this, (s8)arg); }
static void kiT60_turnShape(ki_class* i_this, s32 scale, s32 step, s32) {
    cLib_addCalcAngleS2(&i_this->actor.shape_angle.y, i_this->actor.current.angle.y, (s16)scale, (s16)step);
}
static void kiT60_facePlayer(ki_class* i_this, s32, s32, s32) {
    cLib_addCalcAngleS2(&i_this->actor.shape_angle.y, fopAcM_searchPlayerAngleY(&i_this->actor), 2, 0x1000);
}
/* the circling chase of behaviour 2 (reserve 0) and 10 (reserve 1) around mPosMove */
static void kiT60_circle(ki_class* i_this, s32 reserve, s32, s32) {
    fopAc_ac_c* a_this = &i_this->actor;
    kiHIO_c& hio = l_kiHIO;
    if (reserve == 0) {
        f32 x = cM_ssin(i_this->mFrameCounter * 0x76C) * 50.0f;
        f32 y = cM_ssin(i_this->mFrameCounter * 0x9C4) * 60.0f;
        f32 z = cM_scos(i_this->mFrameCounter * 0x5DC) * 50.0f;

        cLib_addCalc2(&a_this->current.pos.x, i_this->mPosMove.x + x, 0.1f, hio.mCircleMoveRate * i_this->mPosMoveDist);
        cLib_addCalc2(&a_this->current.pos.y, i_this->mPosMove.y + y, 0.1f, hio.mCircleMoveRate * i_this->mPosMoveDist);
        cLib_addCalc2(&a_this->current.pos.z, i_this->mPosMove.z + z, 0.1f, hio.mCircleMoveRate * i_this->mPosMoveDist);
        cLib_addCalc2(&i_this->mPosMoveDist, 1.0f, 1.0f, 0.05f);
    } else {
        f32 x = cM_ssin(i_this->mFrameCounter * 0x76C) * 100.0f;
        f32 y = cM_ssin(i_this->mFrameCounter * 0x9C4) * 60.0f;
        f32 z = cM_scos(i_this->mFrameCounter * 0x5DC) * 100.0f;

        cLib_addCalc2(&a_this->current.pos.x, i_this->mPosMove.x + x, 0.1f, hio.mReserveMoveSpeed * i_this->mPosMoveDist);
        cLib_addCalc2(&a_this->current.pos.y, i_this->mPosMove.y + y, 0.1f, hio.mReserveMoveSpeed * i_this->mPosMoveDist);
        cLib_addCalc2(&a_this->current.pos.z, i_this->mPosMove.z + z, 0.1f, hio.mReserveMoveSpeed * i_this->mPosMoveDist);
        cLib_addCalc2(&i_this->mPosMoveDist, 1.0f, 1.0f, 0.025f);
    }
}

/* true60: the end of ki_atack_move (after the frame's motion: tick30 step), also run by a held tick30 step
 * whose frame's dispatch was this function (d_a_ki_t60.h) */
static void ki_t60_atk_end(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    kiHIO_c& hio = l_kiHIO;
    if (i_this->mParameters != 3) {
        /* true60 (the first-step input rule addendum): the wall between Link and the Keese is a Link read: tick30 step */
        if (t60c::end() && ki_player_bg_check(i_this)) {
            i_this->mTimers[0] = 0;
        }

        /* true60: the return timer comes after the frame's motion in A's call: tick30 step (A's order) */
        if (t60c::end() && i_this->mTimers[0] == 0) {
            i_this->mAction = i_this->mParameters;
            i_this->mTimers[2] = 0;

            if (i_this->mKiPathIndex != 0xFF) {
                i_this->mCurrKiPathIndex = i_this->mKiPathIndex + 1;
                i_this->mBehaviorType = -1;
            } else if (i_this->mAction == ki_class::ACT_FIRE_SET_MOVE_e) {
                i_this->mBehaviorType = 10;
            } else if (i_this->mAction == ki_class::ACT_WAIT_MOVE_e) {
                gabi::Local<KiLineCheck> linChk; /* (guest stack: the line check reads / writes it) */
                ki_LinChk_ct(linChk);
                gabi::Local<cXyz> sp18;
                sp18->x = a_this->current.pos.x;
                sp18->y = a_this->current.pos.y;
                sp18->z = a_this->current.pos.z;
                sp18->y += 5000.0f;
                dBgS_LinChk_Set(linChk, &a_this->current.pos, sp18, a_this);
                if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                    cXyz* cross = kiLineCross(linChk); /* linChk.GetCross() */
                    a_this->home.pos.x = cross->x;
                    a_this->home.pos.y = cross->y;
                    a_this->home.pos.z = cross->z;
                    a_this->home.pos.y -= 30.0f;
                    i_this->mBehaviorType = 10;
                    anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 5.0f, 2, hio.mFly1AnmSpeed, dRes_INDEX_KI_BAS_FLY1_e);
                } else {
                    i_this->mTimers[0] = 50;
                    i_this->mBehaviorType = 1;
                    i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                }
                ki_LinChk_dt(linChk);
            } else {
                i_this->mBehaviorType = 0;
                i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 100.0f);
            }
        }
    }
}

/* 02199C48 */
static void ki_atack_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz sp24;
    kiHIO_c& hio = l_kiHIO;

    /* true60: a held step (the state changed on an earlier step of this frame) replays the motion of the
     * frame's dispatch call; a call that runs begins the frame's journal (d_a_ki_t60.h) */
    if (ki_t60_held(i_this)) {
        const bool t60Mine = ki_t60_jr_id(i_this) == 0;
        ki_t60_jr_replay(i_this);
        if (t60Mine && t60::tick30())
            ki_t60_atk_end(i_this);
        return;
    }
    ki_t60_jr_begin(i_this, 0);

    i_this->mBgCheckOn = 1;
    s8 cVar6 = 0;

    /* true60 (the first-step AI-timer rule): the cry timer and its draw on the first step */
    if (t60c::first() && i_this->mTimers[3] == 0 && i_this->mBehaviorType <= 3) {
        i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(14.0f) + 15.0f);
        fopAcM_monsSeStart(a_this, JA_SE_CV_KI_ATTACK, 0);
    }

    switch (i_this->mBehaviorType) {
    case 0: {
        anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 10.0f, 2, hio.mFly1AnmSpeed, dRes_INDEX_KI_BAS_FLY1_e);
        i_this->mBehaviorType = 1;
        tex_anm_set(i_this, 0);
        f32 r = cM_rndF(std::fabs((f32)(s32)(hio.mAttackModeTimeMax - hio.mAttackModeTimeMin)));
        i_this->mTimers[0] = (s16)gabi::ftoi(r + (f32)hio.mAttackModeTimeMin);
        i_this->mTimers[2] = 20;
        ki_t60_ran(i_this); /* true60: behaviour 1's body runs in this call (fall-through): not held */
    }
        [[fallthrough]];
    case 1: {
        i_this->mPosMoveTarget = hio.mFlySpeed;
        i_this->mPosMoveMaxSpeed = 2.0f;
        i_this->mPosMove.x = player->current.pos.x;
        i_this->mPosMove.y = player->current.pos.y;
        i_this->mPosMove.z = player->current.pos.z;
        i_this->mPosMove.y += 80.0f;

        cXyz tmp;
        cXyz_mi(&i_this->mPosMove, &tmp, &a_this->current.pos);
        sp24.copy(*&tmp);
        ki_t60_do(i_this, kiT60_turnShape, 2, 0x1000);
        /* true60 (the first-step input rule addendum): the distance to Link decides on the tick30 step */
        if (t60c::end() && std_sqrtf(PSVECSquareMag(&sp24)) < 300.0f) {
            i_this->mBehaviorType = 10;
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY2_e, 5.0f, 2, hio.mFly2AnmSpeed, dRes_INDEX_KI_BAS_FLY2_e);
            i_this->mPosMoveDist = 0.0f;
        }
        break;
    }

    case 2:
        /* true60 (the first-step AI-timer rule): the attack timer and its chance on the first step */
        if (t60c::first() && i_this->mTimers[2] == 0) {
            ki_check(i_this);
            i_this->mTimers[2] = hio.mAttackCheckTime;
            f32 attackChance = hio.mAttackChance;
            if (ki_fight_count >= hio.mCrowdFightCount) {
                attackChance *= 0.5f;
                i_this->mTimers[2] <<= 1;
            }

            if (cM_rndF(1.0f) < attackChance) {
                i_this->mBehaviorType = 3;
                anm_init(i_this, dRes_INDEX_KI_BCK_ATTACK1_e, 5.0f, 2, hio.mAttack1AnmSpeed, dRes_INDEX_KI_BAS_ATTACK1_e);
                i_this->mPosMove.x = player->current.pos.x;
                i_this->mPosMove.y = player->current.pos.y;
                i_this->mPosMove.z = player->current.pos.z;
                i_this->mPosMove.y += 80.0f;
                a_this->current.angle.y = fopAcM_searchPlayerAngleY(a_this);
                a_this->speedF = 0.0f;
                i_this->mPosMoveTarget = hio.mAttackSpeed;
                i_this->mPosMoveMaxSpeed = 3.0f;
                i_this->mTimers[1] = 30;
                break;
            }
        }

        ki_t60_do(i_this, kiT60_facePlayer);
        if (t60c::first() && i_this->mTimers[1] == 0) { /* true60 (first-step AI-timer): the circle target's timer and draws */
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 20.0f);
            s32 rndFX = gabi::ftoi(cM_rndFX(hio.mCircleAngleRange * 182.0444f));
            cMtx_YrotS(calc_mtx(), (s16)(player->current.angle.y + rndFX));
            sp24.x = 0.0f;
            f32 y = cM_rndF(50.0f) + hio.mCircleHeight;
            sp24.y = y;
            sp24.z = hio.mCircleDist;
            MtxPosition(&sp24, &i_this->mPosMove);
            cXyz_add_to(&i_this->mPosMove, &player->current.pos);
            i_this->mPosMoveDist = 0.0f;
        }

        ki_t60_do(i_this, kiT60_circle, 0);
        cVar6 = -1;
        break;

    case 3:
        i_this->mAtSph.SetR(30.0f);
        i_this->mDamageSphere.SetR(20.0f);
        i_this->mCoSph.SetR(0.0f);
        if (i_this->mDamageType == 0) {
            i_this->mAtSph.SetAtAtp(1);
        } else {
            i_this->mAtSph.SetAtAtp(2);
        }

        cVar6 = 1;

        if (t60c::first() && i_this->mTimers[1] == 10) { /* true60 (first-step AI-timer): timer one-shots on the first step */
            i_this->mPosMoveTarget = 0.0f;
            i_this->mPosMoveMaxSpeed = 3.0f;
        }

        if (t60c::first() && i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 5.0f, 2, hio.mFly1AnmSpeed, dRes_INDEX_KI_BAS_FLY1_e);
            i_this->mBehaviorType = 1;
        }

        ki_t60_do(i_this, kiT60_turnShape, 2, 0x2000);

        /* true60 (the mixed-pair hit-flag rule / one-sweep-per-frame attack): the tick30 Move's At / Co result, read on the first step */
        if (t60c::first() && (i_this->mAtSph.ChkAtHit() || i_this->mDamageSphere.ChkCoHit())) {
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY2_e, 2.0f, 2, hio.mFly2AnmSpeed, dRes_INDEX_KI_BAS_FLY2_e);
            i_this->mBehaviorType = 4;
            i_this->mTimers[1] = 30;
            a_this->speedF = -15.0f;
            i_this->mPosMoveTarget = 0.0f;
            i_this->mPosMoveMaxSpeed = 1.0f;
        }
        break;

    case 4:
        if (t60c::first() && i_this->mTimers[1] == 0) { /* true60 (first-step AI-timer) */
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 3.0f, 2, hio.mFly1AnmSpeed, dRes_INDEX_KI_BAS_FLY1_e);
            i_this->mBehaviorType = 1;
        }
        break;

    case 10:
        i_this->mPosMoveTarget = hio.mReserveMoveSpeed;
        /* true60 (the process-order read rule): the join decision counts the other Keese: on the tick30 step, a Keese later in the
         * line with its frame-start state (ki_a_d_sub) */
        if (t60c::end()) {
            ki_check(i_this);

            if (ki_fight_count < hio.mJoinFightCount) {
                i_this->mBehaviorType = 2;
                i_this->mTimers[2] = 50;
            }
        }

        ki_t60_do(i_this, kiT60_facePlayer);
        if (t60c::first() && i_this->mTimers[1] == 0) { /* true60 (first-step AI-timer) */
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            s32 rndFX = gabi::ftoi(cM_rndFX(32768.0f));
            cMtx_YrotS(calc_mtx(), (s16)(player->current.angle.y + rndFX));
            sp24.x = 0.0f;
            f32 y = (cM_rndFX(50.0f) + 200.0f) + REG0_F(12);
            sp24.y = y;
            sp24.z = hio.mCircleDist + REG0_F(13);
            MtxPosition(&sp24, &i_this->mPosMove);
            cXyz_add_to(&i_this->mPosMove, &player->current.pos);
            i_this->mPosMoveDist = 0.0f;
        }

        ki_t60_do(i_this, kiT60_circle, 1);
        cVar6 = -1;
        break;
    }

    i_this->mTurnScale = 10000.0f;
    i_this->mPosMoveDist = 1.0f;

    if (cVar6 >= 0) {
        ki_t60_do(i_this, kiT60_posMove, cVar6);
    }

    ki_t60_atk_end(i_this);
}

/* WWHD glue
ext 02199364 void kiAttack_check(ki_class* actor)
ext 02199388 void kiAttack_animation(ki_class* actor, int resource, f32 blend, u8 loop, f32 speed, int sound)
ext 021994B0 void kiAttack_texture(ki_class* actor, u16 index)
ext 021995BC BOOL kiAttack_playerBackground(ki_class* actor)
ext 02199A64 void kiAttack_position(ki_class* actor, s8 mode)
*/
