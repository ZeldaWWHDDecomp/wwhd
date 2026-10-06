/**
 * d_a_ki_exec.cpp (WWHD)
 * Enemy - Keese: ki_atack_move and daKi_Execute (with its inlined move functions)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ki.h"

/* functions of d_a_ki.cpp (guest calls by address) */
static inline void ki_check(ki_class* i_this) { gabi::call(0x02199364, i_this); }
static inline void anm_init(ki_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float playSpeed, int soundResIdx) {
    gabi::call(0x02199388, i_this, anmResIdx, morf, loopMode, playSpeed, soundResIdx);
}
static inline void tex_anm_set(ki_class* i_this, unsigned short idx) { gabi::call(0x021994B0, i_this, idx); }
static inline BOOL ki_player_bg_check(ki_class* i_this) { return gabi::call<BOOL>(0x021995BC, i_this); }
static inline void ki_pos_move(ki_class* i_this, s8 arg1) { gabi::call(0x02199A64, i_this, arg1); }

/* sp = a + b */
static inline void cXyz_add_to(cXyz* a, const cXyz* b) { PSVECAdd(a, b, a); }

/* 02199C48 */
static void ki_atack_move(ki_class* i_this) {
    WWHD_FUNC(0x02199C48, void, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> sp24;
    kiHIO_c& hio = l_kiHIO();

    i_this->m540 = 1;
    s8 cVar6 = 0;

    if (i_this->mTimers[3] == 0 && i_this->mBehaviorType <= 3) {
        i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(14.0f) + 15.0f);
        fopAcM_monsSeStart(a_this, JA_SE_CV_KI_ATTACK, 0);
    }

    switch (i_this->mBehaviorType) {
    case 0: {
        anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 10.0f, 2, hio.m08, dRes_INDEX_KI_BAS_FLY1_e);
        i_this->mBehaviorType = 1;
        tex_anm_set(i_this, 0);
        f32 r = cM_rndF(std::fabs((f32)(s32)(hio.m4E - hio.m50)));
        i_this->mTimers[0] = (s16)gabi::ftoi(r + (f32)hio.m50);
        i_this->mTimers[2] = 20;
    }
        [[fallthrough]];
    case 1: {
        i_this->mPosMoveTarget = hio.m28;
        i_this->mPosMoveMaxSpeed = 2.0f;
        i_this->mPosMove.x = player->current.pos.x;
        i_this->mPosMove.y = player->current.pos.y;
        i_this->mPosMove.z = player->current.pos.z;
        i_this->mPosMove.y += 80.0f;

        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->mPosMove, tmp, &a_this->current.pos);
        sp24->copy(*tmp);
        cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 2, 0x1000);
        if (std_sqrtf(PSVECSquareMag(sp24)) < 300.0f) {
            i_this->mBehaviorType = 10;
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY2_e, 5.0f, 2, hio.m14, dRes_INDEX_KI_BAS_FLY2_e);
            i_this->mPosMoveDist = 0.0f;
        }
        break;
    }

    case 2:
        if (i_this->mTimers[2] == 0) {
            ki_check(i_this);
            i_this->mTimers[2] = hio.m4C;
            f32 tmp = hio.m48;
            if (ki_fight_count() >= hio.m54) {
                tmp *= 0.5f;
                i_this->mTimers[2] <<= 1;
            }

            if (cM_rndF(1.0f) < tmp) {
                i_this->mBehaviorType = 3;
                anm_init(i_this, dRes_INDEX_KI_BCK_ATTACK1_e, 5.0f, 2, hio.m18, dRes_INDEX_KI_BAS_ATTACK1_e);
                i_this->mPosMove.x = player->current.pos.x;
                i_this->mPosMove.y = player->current.pos.y;
                i_this->mPosMove.z = player->current.pos.z;
                i_this->mPosMove.y += 80.0f;
                a_this->current.angle.y = fopAcM_searchPlayerAngleY(a_this);
                a_this->speedF = 0.0f;
                i_this->mPosMoveTarget = hio.m44;
                i_this->mPosMoveMaxSpeed = 3.0f;
                i_this->mTimers[1] = 30;
                break;
            }
        }

        cLib_addCalcAngleS2(&a_this->shape_angle.y, fopAcM_searchPlayerAngleY(a_this), 2, 0x1000);
        if (i_this->mTimers[1] == 0) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 20.0f);
            s32 rndFX = gabi::ftoi(cM_rndFX(hio.m24 * 182.0444f));
            cMtx_YrotS(calc_mtx(), (s16)(player->current.angle.y + rndFX));
            sp24->x = 0.0f;
            f32 y = cM_rndF(50.0f) + hio.m1C;
            sp24->y = y;
            sp24->z = hio.m20;
            MtxPosition(sp24, &i_this->mPosMove);
            cXyz_add_to(&i_this->mPosMove, &player->current.pos);
            i_this->mPosMoveDist = 0.0f;
        }

        {
            f32 x = cM_ssin(i_this->m320 * 0x76C) * 50.0f;
            f32 y = cM_ssin(i_this->m320 * 0x9C4) * 60.0f;
            f32 z = cM_scos(i_this->m320 * 0x5DC) * 50.0f;

            cLib_addCalc2(&a_this->current.pos.x, i_this->mPosMove.x + x, 0.1f, hio.m2C * i_this->mPosMoveDist);
            cLib_addCalc2(&a_this->current.pos.y, i_this->mPosMove.y + y, 0.1f, hio.m2C * i_this->mPosMoveDist);
            cLib_addCalc2(&a_this->current.pos.z, i_this->mPosMove.z + z, 0.1f, hio.m2C * i_this->mPosMoveDist);
            cLib_addCalc2(&i_this->mPosMoveDist, 1.0f, 1.0f, 0.05f);
        }
        cVar6 = -1;
        break;

    case 3:
        i_this->m580.SetR(30.0f);
        i_this->mDamageSphere.SetR(20.0f);
        i_this->m6AC.SetR(0.0f);
        if (i_this->mDamageType == 0) {
            i_this->m580.SetAtAtp(1);
        } else {
            i_this->m580.SetAtAtp(2);
        }

        cVar6 = 1;

        if (i_this->mTimers[1] == 10) {
            i_this->mPosMoveTarget = 0.0f;
            i_this->mPosMoveMaxSpeed = 3.0f;
        }

        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 5.0f, 2, hio.m08, dRes_INDEX_KI_BAS_FLY1_e);
            i_this->mBehaviorType = 1;
        }

        cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 2, 0x2000);

        if (i_this->m580.ChkAtHit() || i_this->mDamageSphere.ChkCoHit()) {
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY2_e, 2.0f, 2, hio.m14, dRes_INDEX_KI_BAS_FLY2_e);
            i_this->mBehaviorType = 4;
            i_this->mTimers[1] = 30;
            a_this->speedF = -15.0f;
            i_this->mPosMoveTarget = 0.0f;
            i_this->mPosMoveMaxSpeed = 1.0f;
        }
        break;

    case 4:
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 3.0f, 2, hio.m08, dRes_INDEX_KI_BAS_FLY1_e);
            i_this->mBehaviorType = 1;
        }
        break;

    case 10:
        i_this->mPosMoveTarget = hio.m30;
        ki_check(i_this);

        if (ki_fight_count() < hio.m52) {
            i_this->mBehaviorType = 2;
            i_this->mTimers[2] = 50;
        }

        cLib_addCalcAngleS2(&a_this->shape_angle.y, fopAcM_searchPlayerAngleY(a_this), 2, 0x1000);
        if (i_this->mTimers[1] == 0) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            s32 rndFX = gabi::ftoi(cM_rndFX(32768.0f));
            cMtx_YrotS(calc_mtx(), (s16)(player->current.angle.y + rndFX));
            sp24->x = 0.0f;
            f32 y = (cM_rndFX(50.0f) + 200.0f) + REG0_F(12);
            sp24->y = y;
            sp24->z = hio.m20 + REG0_F(13);
            MtxPosition(sp24, &i_this->mPosMove);
            cXyz_add_to(&i_this->mPosMove, &player->current.pos);
            i_this->mPosMoveDist = 0.0f;
        }

        {
            f32 x = cM_ssin(i_this->m320 * 0x76C) * 100.0f;
            f32 y = cM_ssin(i_this->m320 * 0x9C4) * 60.0f;
            f32 z = cM_scos(i_this->m320 * 0x5DC) * 100.0f;

            cLib_addCalc2(&a_this->current.pos.x, i_this->mPosMove.x + x, 0.1f, hio.m30 * i_this->mPosMoveDist);
            cLib_addCalc2(&a_this->current.pos.y, i_this->mPosMove.y + y, 0.1f, hio.m30 * i_this->mPosMoveDist);
            cLib_addCalc2(&a_this->current.pos.z, i_this->mPosMove.z + z, 0.1f, hio.m30 * i_this->mPosMoveDist);
            cLib_addCalc2(&i_this->mPosMoveDist, 1.0f, 1.0f, 0.025f);
        }
        cVar6 = -1;
        break;
    }

    i_this->m304 = 10000.0f;
    i_this->mPosMoveDist = 1.0f;

    if (cVar6 >= 0) {
        ki_pos_move(i_this, cVar6);
    }

    if (i_this->mParameters != 3) {
        if (ki_player_bg_check(i_this)) {
            i_this->mTimers[0] = 0;
        }

        if (i_this->mTimers[0] == 0) {
            i_this->mAction = i_this->mParameters;
            i_this->mTimers[2] = 0;

            if (i_this->mKiPathIndex != 0xFF) {
                i_this->mCurrKiPathIndex = i_this->mKiPathIndex + 1;
                i_this->mBehaviorType = -1;
            } else if (i_this->mAction == ki_class::ACT_FIRE_SET_MOVE_e) {
                i_this->mBehaviorType = 10;
            } else if (i_this->mAction == ki_class::ACT_WAIT_MOVE_e) {
                gabi::Local<dBgS_LinChk_l> linChk;
                ki_LinChk_ct(linChk);
                gabi::Local<cXyz> sp18;
                sp18->x = a_this->current.pos.x;
                sp18->y = a_this->current.pos.y;
                sp18->z = a_this->current.pos.z;
                sp18->y += 5000.0f;
                dBgS_LinChk_Set(linChk, &a_this->current.pos, sp18, a_this);
                if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                    u32 cross = gabi::ea(linChk.get()) + 0x30; /* linChk.GetCross() */
                    a_this->home.pos.x = gabi::load<f32>(cross + 0);
                    a_this->home.pos.y = gabi::load<f32>(cross + 4);
                    a_this->home.pos.z = gabi::load<f32>(cross + 8);
                    a_this->home.pos.y -= 30.0f;
                    i_this->mBehaviorType = 10;
                    anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 5.0f, 2, hio.m08, dRes_INDEX_KI_BAS_FLY1_e);
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
VERIFY(0x02199C48, ki_atack_move);
