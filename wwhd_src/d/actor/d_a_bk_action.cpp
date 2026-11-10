/**
 * d_a_bk_action.cpp (WWHD)
 * Enemy - Bokoblin: action functions (hukki .. rope_on).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_action.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10009474,
    .gndChk = {0x100094BC, 0x100094CC, 0x100094EC, 0x100094DC},
    .gndChkDt = {0x100094CC, 0x100094EC, 0x100094AC},
};

/* weak stubs for functions defined in d_a_bk.cpp (this part is also verified as its own unit) */
__attribute__((weak)) void anm_init(bk_class* a, int bck, f32 morf, u8 loop, f32 speed, int snd) {
    bkAction_anm_init(a, bck, morf, loop, speed, snd);
}
__attribute__((weak)) void wait_set(bk_class* a) { bkAction_wait_set(a); }

/* 020A3048 */
void hukki(bk_class* i_this) {
    dComIfGp_get(); /* player: HD reads nothing from it (the attack branch is gone) */
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->dr.mJntPos[10], tmp, &i_this->dr.mJntPos[13]);
    gabi::Local<cXyz> sp24;
    *sp24 = tmp->get();
    gabi::Local<cXyz> sp18;
    {
        Mtx34* m = calc_mtx();
        s16 a = cM_atan2s(sp24->x, sp24->z);
        cMtx_YrotS(m, a);
    }
    {
        Mtx34* m = calc_mtx();
        f32 x = sp24->x, z = sp24->z;
        f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
        s16 a = cM_atan2s(sp24->y, d);
        cMtx_XrotM(m, -a);
    }

    sp24->y = 0.0f;
    sp24->x = 0.0f;
    /* HD: no `mNoHitTimer = 2` here (set to REG10_S(1) + 35 after the push-out below) */

    switch (i_this->dr.mMode) {
    case 10:
    case 12: {
        if (i_this->dr.mMode == 10) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_OKIRUA_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_OKIRUA_e);
            i_this->speedF = 0.0f;
            i_this->mTimers[2] = 15;
        } else {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_OKIRUU_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_OKIRUU_e);
            i_this->speedF = 0.0f;
            i_this->mTimers[2] = 15 + REG0_S(5);
        }
        i_this->dr.mMode = 13;
        i_this->dr.mFallPitch = 0;
        f32 dy = i_this->dr.mPivot.y;
        i_this->current.pos.y -= dy;
        i_this->old.pos.y -= dy;
        i_this->dr.mPivot.y = 0.0f;
        sp24->z = (375.0f + REG14_F(8)) * 0.1f; /* HD: 375 (GameCube 125) */
        MtxPosition(sp24, sp18);
        for (int i = 0; i < 3; i++) {
            i_this->old.pos.copy(i_this->current.pos);
            /* HD: only x and z are pushed */
            i_this->current.pos.x += sp18->x;
            i_this->current.pos.z += sp18->z;
            bkAction_correctPosition(&i_this->dr.mAcch);
        }
        i_this->mNoHitTimer = REG10_S(1) + 35; /* HD */
    }
        // Fall-through
    case 13:
        if (i_this->mTimers[2] == 1) {
            i_this->mSmokeTimer = l_bkHIO().m00C + 6;
            i_this->mSmokeType = 3;
        }
        if (i_this->mTimers[2] > 3) {
            i_this->dr.mSkipMoveOnce = 1;
        }
        if (i_this->mpMorf->isStop()) {
            if (i_this->mPlayerDistance < l_bkHIO().m02C) {
                i_this->dr.mMode = 14;
                i_this->mTimers[1] = 10;
            } else {
                i_this->dr.mAction = 0;
                path_check(i_this, 0);
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
            i_this->dr.mKnockedDown = 0;
        }
        break;
    case 14: {
        i_this->dr.mLookMode = 1;
        s16 ang = i_this->mTargetAngleY;
        i_this->dr.mTurnTargetY = ang;
        cLib_addCalcAngleS2(&i_this->current.angle.y, ang, 3, 0x1000);
        if (i_this->mTimers[1] == 0) {
            /* HD: no counter-attack (GameCube: attack when the player is not grabbing and the
             * bokoblin holds a weapon) */
            i_this->dr.mAction = 4;
            i_this->dr.mMode = 0;
        }
        break;
    }
    }

    gabi::Local<dBgS_GndChk_a> gndChk;
    a_GndChk_ct(gndChk);
    gndChk->m_pos.x = i_this->current.pos.x;
    gndChk->m_pos.y = i_this->current.pos.y + 200.0f;
    gndChk->m_pos.z = i_this->current.pos.z;
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    groundY -= 50.0f;
    groundY = groundY + REG17_F(2);
    if (i_this->current.pos.y < groundY) {
        i_this->current.pos.y = groundY;
    }
    a_GndChk_dt(gndChk);
}

/* 020A3624 */
void aite_miru(bk_class* i_this) {
    switch (i_this->dr.mMode) {
    case 0:
        i_this->mTimers[1] = 20 + REG0_S(8);
        i_this->speedF = 0.0f;
        i_this->dr.mMode = 1;
        // Fall-through
    case 1:
        cLib_addCalcAngleS2(&i_this->mHeadAngleZ, 0x2EE0, 2, 0x1800);
        i_this->dr.mLookMode = 1;
        if (i_this->mHitActorId != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* temp = fopAcM_SearchByID(i_this->mHitActorId);
            if (temp != nullptr) {
                i_this->dr.mpLookTarget = temp;
            }
        }
        if (i_this->mTimers[1] == 0) {
            i_this->mHitActorId = fpcM_ERROR_PROCESS_ID_e;
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
            path_check(i_this, 0);
        }
        break;
    }
}

/* 020A3704 */
void fail(bk_class* i_this) {
    i_this->mNoHitTimer = 5;
    i_this->attention_info.flags = 0;
    i_this->speedF = 0.0f;
    i_this->speed.y = 0.0f;

    switch (i_this->dr.mMode) {
    case -1:
        if (i_this->mDemoCamMode == 0) {
            fopAcM_delete(i_this);
        }
        break;
    case 0:
        i_this->dr.mMode = 1;
        // Fall-through
    case 1: {
        gabi::Local<cXyz> sp08;
        auto p = i_this->current.pos.get();
        p.y += 100.0f + l_bkHIO().m020;
        *sp08 = p;
        u8 drop_type = daDisItem_IBALL_e;
        if (i_this->mTimers[2] >= 1000) {
            drop_type = daDisItem_NONE1_e;
        }
        fopAcM_createDisappear(i_this, sp08, 10, drop_type, i_this->stealItemBitNo);
        if (i_this->mType == 10) {
            i_this->mDefeated = 1;
            i_this->dr.mMode = -1;
            mDoAud_subBgmStop();
        } else {
            fopAcM_delete(i_this);
        }

        if (i_this->mDeathSwitch != 0) {
            dComIfGs_onSwitch(i_this->mDeathSwitch, fopAcM_GetRoomNo(i_this));
        }

        fopAcM_onActor(i_this);

        if (i_this->mType != 4) {
            if (dComIfGs_isEventBit(UNK_0301)) {
                dComIfGs_onEventBit(UNK_0480);
            } else {
                dComIfGs_onEventBit(UNK_0301);
            }
        }
        break;
    }
    }
}

/* 020A38D4 */
void yogan_fail(bk_class* i_this) {
    i_this->mNoHitTimer = 5;
    i_this->attention_info.flags = 0;
    i_this->speedF = 0.0f;

    switch (i_this->dr.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_NIGERU_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                 dRes_INDEX_BK_BAS_BK_NIGERU_e);
        i_this->dr.mMode = 1;
        i_this->speed.y = REG0_F(19) + 190.0f;
        fopAcM_monsSeStart(i_this, JA_SE_CV_BK_SURPRISE, 0);
        /* HD: one store (GameCube clears mBodyOffset.y and mPivot.y) */
        i_this->dr.mPivot.y = 0.0f;
        i_this->dr.mFallPitch = 0;
        if (i_this->mHoldWeapon != 0) {
            i_this->mDropWeapon = 1;
        }
        // Fall-through
    case 1:
        dComIfGp_particle_setSimple(ID_IT_SN_O_FIREK_KASU, &i_this->current.pos);
        dComIfGp_particle_setSimple(ID_IT_SN_O_MAGT_FCHIP, &i_this->current.pos);

        if ((i_this->mFrameCounter & 7) == 0) {
            i_this->mEffectAngle.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
            i_this->mEffectAngle.x = -0x2000;
            dComIfGp_particle_set(ID_AK_JN_TUBA00, &i_this->mSpitPos, &i_this->mEffectAngle);
        }

        cLib_addCalcAngleS2(&i_this->current.angle.x, -0x4000, 10, 0x200);
        if (i_this->speed.y < 0.0f) {
            i_this->dr.mAction = 20;
            i_this->dr.mMode = 0;
            i_this->mTimers[2] = 2000;
        }
        break;
    }
}

/* 020A3B18 */
void water_fail(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->dr.mBodyBgCheckDelay = 5;
    i_this->mNoHitTimer = 5;
    actor->attention_info.flags = 0;
    actor->speedF = 0.0f;

    switch (i_this->dr.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_NIGERU_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                 dRes_INDEX_BK_BAS_BK_NIGERU_e);
        i_this->dr.mMode = 1;
        fopAcM_monsSeStart(actor, JA_SE_CV_BK_SURPRISE, 0);
        /* HD: one store (GameCube clears mBodyOffset.y and mPivot.y) */
        i_this->dr.mPivot.y = 0.0f;
        i_this->dr.mFallPitch = 0;
        if (i_this->mHoldWeapon != 0) {
            i_this->mDropWeapon = 1;
        }
        i_this->mTimers[3] = 120;
        // Fall-through
    case 1:
        actor->speed.y = 0.0f;
        actor->current.pos.y -= 1.0f;
        cLib_addCalcAngleS2(&actor->current.angle.x, 0, 8, 0x800);
        if (i_this->mTimers[3] == 0) {
            fopAcM_delete(actor);
            if (i_this->mDeathSwitch != 0) {
                dComIfGs_onSwitch(i_this->mDeathSwitch, fopAcM_GetRoomNo(actor));
            }
        }
        break;
    }
}

/* 020A3CC8 */
void wepon_search(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* boko = fopAcM_SearchByID(i_this->mWeaponId);

    if (i_this->dr.mMode < 2 &&
        (boko == nullptr || (boko != nullptr && fopAcM_checkCarryNow(boko)))) {
        i_this->dr.mAction = 0;
        path_check(i_this, 0);
        wait_set(i_this);
        i_this->dr.mMode = 2;
        return;
    }

    f32 f31 = 10000.0f;
    if (boko != nullptr) {
        f32 dx = boko->current.pos.x - actor->current.pos.x;
        f32 dz = boko->current.pos.z - actor->current.pos.z;
        i_this->dr.mTurnTargetY = cM_atan2s(dx, dz);
        f31 = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    }

    switch (i_this->dr.mMode) {
    case -1:
        i_this->dr.mMode = 0;
        i_this->mBareHanded = 0;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_HAKKEN_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                 dRes_INDEX_BK_BAS_BK_HAKKEN_e);
        if (f31 < 900.0f) {
            i_this->mTimers[1] = 20;
        } else {
            i_this->mTimers[1] = 200;
        }
        i_this->mAnmStopTimer = 5;
        fopAcM_monsSeStart(actor, JA_SE_CV_BK_FOUND_BOKO, 0);
        break;
    case 0:
        actor->speedF = 0.0f;
        if (i_this->mAnmStopTimer == 0) {
            cLib_addCalcAngleS2(&actor->current.angle.y, i_this->dr.mTurnTargetY, 2, 0x3000);
            if (i_this->mpMorf->isStop() || i_this->mTimers[1] == 0) {
                i_this->dr.mMode = 1;
                i_this->mWeaponRunTimer = l_bkHIO().m08E;
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 3.0f, J3DFrameCtrl::EMode_LOOP,
                         l_bkHIO().m074, dRes_INDEX_BK_BAS_BK_RUN_e);
                i_this->mWeaponJumpCount = 0;
            }
        }
        break;
    case 1:
        actor->speedF = l_bkHIO().m05C;
        i_this->mSmokeTimer = l_bkHIO().m00C + 3;
        i_this->mSmokeType = 4;
        cLib_addCalcAngleS2(&actor->current.angle.y, i_this->dr.mTurnTargetY, 4, 0x1000);
        if (f31 < REG8_F(2) + 150.0f) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_CATCH_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_CATCH_e);
            i_this->mTimers[1] = 29;
        } else {
            if (i_this->mWeaponRunTimer == 0) {
                i_this->dr.mAction = 4;
                i_this->mTimers[1] = 0;
                i_this->mBareHanded = 1;
            } else if (i_this->dr.mAcch.ChkGroundHit() && i_this->dr.mAcch.ChkWallHit()) {
                if (i_this->mWeaponJumpCount < 2) {
                    actor->speed.y = 100.0f + REG0_F(16);
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, J3DFrameCtrl::EMode_NONE,
                             1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
                    fopAcM_monsSeStart(actor, JA_SE_CV_BK_JUMP, 0);
                    i_this->dr.mMode = 5;
                    i_this->mWeaponJumpCount++;
                } else {
                    i_this->dr.mAction = 0;
                    path_check(i_this, 0);
                    wait_set(i_this);
                    i_this->dr.mMode = 2;
                    i_this->mWeaponViewWait = 70;
                }
            }
        }
        break;
    case 2:
        actor->speedF = 0.0f;
        if (i_this->mTimers[1] == 24) {
            if (boko != nullptr && !fopAcM_checkCarryNow(boko)) {
                i_this->mHoldWeapon = 2;
                if (fopAcM_GetParam(boko) == 0 /* daBoko_c::Type_BOKO_STICK_e */) {
                    i_this->mHasMachete = 0;
                    i_this->mWeaponSph.SetAtType(AT_TYPE_UNK2000);
                    i_this->mWeaponSph.SetAtSe(dCcG_SE_WOOD);
                } else {
                    i_this->mHasMachete = 1;
                    i_this->mWeaponSph.SetAtType(AT_TYPE_UNK800);
                    i_this->mWeaponSph.SetAtSe(dCcG_SE_UNK2);
                }
                fopAcM_setCarryNow(boko, FALSE);
            } else {
                i_this->dr.mAction = 0;
                path_check(i_this, 0);
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
        }
        if (i_this->mTimers[1] < 14) {
            i_this->dr.mLookMode = 1;
            s16 ang = i_this->mTargetAngleY;
            i_this->dr.mTurnTargetY = ang;
            cLib_addCalcAngleS2(&actor->current.angle.y, ang, 3, 0x800);
        }
        if (i_this->mpMorf->isStop()) {
            if (l_bkHIO().m00A == 0 && !(daPy_getGrabWearTimer(player) < 0.0f) &&
                i_this->mPlayerDistance < l_bkHIO().m02C) {
                i_this->dr.mAction = 5;
                i_this->mAttackKind = 0;
                i_this->mAttackFrame = 18.0f;
                i_this->mAtStartFrame = REG6_F(6) + 23.0f;
                i_this->mAtEndFrame = REG6_F(7) + 26.0f;
                i_this->mLookStartFrame = 63.0f;
                i_this->mTurnEndFrame = l_bkHIO().m09C;
                i_this->mAttackDir = 1;
                i_this->mAttackStep = 1;
                attack_info_s* info = attack_info(i_this->mAttackKind);
                info++;
                anm_init(i_this, info->bckFileIdx, 5.0f, J3DFrameCtrl::EMode_NONE, info->speed,
                         info->soundFileIdx);
                /* HD: more of the attack state is reset here */
                i_this->mTimers[4] = 0;
                i_this->mTurnAlways = 1;
                i_this->mHitActorId = fpcM_ERROR_PROCESS_ID_e;
                i_this->dr.mMode = 1;
                i_this->mAtStarted = 0;
                i_this->mWeaponSph.SetR(REG8_F(3) + 60.0f);
            } else {
                i_this->dr.mAction = 0;
                path_check(i_this, 0);
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
        }
        break;
    case 5:
        actor->speedF = l_bkHIO().m05C * 0.5f;
        if (i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode++;
        }
        break;
    case 6:
        actor->speedF = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 1;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 3.0f, J3DFrameCtrl::EMode_LOOP,
                     l_bkHIO().m074, dRes_INDEX_BK_BAS_BK_RUN_e);
        }
        break;
    }
}

/* 020A4594 */
void d_dozou(bk_class* i_this) {
    i_this->mNoHitTimer = 5;
    i_this->attention_info.flags = 0;

    switch (i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        i_this->dr.mStts.Init(0xFF, 0xFF, i_this);
        if (i_this->mSightRangePrm == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_ATTACK2_e, 1.0f, J3DFrameCtrl::EMode_NONE, 0.0f,
                     dRes_INDEX_BK_BAS_BK_ATTACK2_e);
            i_this->mpMorf->setFrame(0.0f);
        } else {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOBI_e, 1.0f, J3DFrameCtrl::EMode_NONE, 0.0f,
                     dRes_INDEX_BK_BAS_BK_NOBI_e);
            i_this->mpMorf->setFrame(27.0f);
        }
        // Fall-through
    case 1:
        if ((i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(i_this))) ||
            (i_this->mSwitchNo == 0xFF && dComIfGs_isEventBit(COLORS_IN_HYRULE))) {
            i_this->mpMorf->setPlaySpeed(1.0f);
            i_this->dr.mMode = 2;
        }
        break;
    case 2:
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mStts.Init(200, 0xFF, i_this);
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
            i_this->attention_info.flags = 4 /* fopAc_Attn_LOCKON_BATTLE_e */;
        }
        break;
    }
}

/* 020A47A4 */
void carry(bk_class* i_this) { i_this->speed.y = 0.0f; }

/* 020A47B4 */
void carry_drop(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;

    cLib_addCalc0(&i_this->dr.mCarryOffsetY, 1.0f, 5.5f);
    cLib_addCalc0(&i_this->dr.mCarryOffsetZ, 1.0f, 0.5f);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x100);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 1, 0x100);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->mTargetAngleY, 4, 0x800);

    switch (i_this->dr.mMode) {
    case 0:
        dComIfGs_onEventBit(UNK_0004);
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KOUKA_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        // Fall-through
    case 1:
        if (i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_TYAKU_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     -1);
            i_this->dr.mMode = 2;
            i_this->mSmokeTimer = l_bkHIO().m00C + 15;
            i_this->mSmokeType = 3;
            i_this->dr.mMaxFallDistance = 1000.0f;
            fopAcM_seStart(i_this, JA_SE_CM_BK_BB_LANDING, 0);
        }
        break;
    case 2:
        cLib_addCalc0(&i_this->dr.mCarryOffsetY, 1.0f, 50.0f);
        cLib_addCalc0(&i_this->dr.mCarryOffsetZ, 1.0f, 50.0f);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x1000);
        cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 1, 0x1000);
        cLib_addCalc0(&i_this->speedF, 1.0f, 1.0f);
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_KIME_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->dr.mMode = 3;
            fopAcM_monsSeStart(actor, JA_SE_CV_BK_SURPRISE, 0);
        }
        break;
    case 3:
        cLib_addCalc0(&i_this->speedF, 1.0f, 1.0f);
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
            i_this->current.angle.x = i_this->shape_angle.x;
            i_this->current.angle.y = i_this->shape_angle.y;
            i_this->current.angle.z = i_this->shape_angle.z;
        }
        break;
    }
}

/* 020A4B44 */
void d_mahi(bk_class* i_this) {
    switch (i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_SUWARI_e, 20.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                 dRes_INDEX_BK_BAS_BK_SUWARI_e);
        i_this->mTimers[0] = 100;
        // Fall-through
    case 1:
        if (i_this->mTimers[0] <= 80 && i_this->mTimers[0] >= 40) {
            if (i_this->mTimers[0] == 80) {
                /* enemy_piyo_set(i_this): HD inline, stars on the HD follow callback */
                dComIfGp_particle_set(0x27A, &i_this->attention_info.position, nullptr, nullptr,
                                      0xFF, (dPa_levelEcallBack*)&i_this->mHdFollowCb);
            }
            fopAcM_seStart(i_this, JA_SE_CM_MD_PIYO, 0);
        }
        i_this->speedF = 0.0f;
        /* HD: the stars end at 35 */
        if (i_this->mTimers[0] == 35) {
            dPa_followEcallBack_end((dPa_followEcallBack*)&i_this->mHdFollowCb);
        }
        if (i_this->mTimers[0] == 30) {
            i_this->dr.mParalysisTimer = 0xF;
        }
        if (i_this->mTimers[0] == 0) {
            i_this->dr.mAction = 0;
            path_check(i_this, 0);
            wait_set(i_this);
            i_this->dr.mMode = 2;
        }
        break;
    }
}

/* 020A4CD4 */
void tubo_wait(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;

    bool r29 = false;
    bool r28 = false;
    cLib_addCalc2(&actor->scale.x, 1.0f, 1.0f, 0.1f);
    f32 s = actor->scale.x;
    actor->scale.y = s;
    actor->scale.z = s;

    switch (i_this->dr.mMode) {
    case 0:
        if (i_this->mTimers[0] != 0) {
            i_this->mTimers[0] = REG0_S(3) + 30;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_SUWARI_e, 0.0f, J3DFrameCtrl::EMode_NONE, 0.01f,
                     dRes_INDEX_BK_BAS_BK_SUWARI_e);
            i_this->dr.mMode = 20;
        } else {
            if (i_this->mType == 3) {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_JATTACK1_e, 0.0f, J3DFrameCtrl::EMode_NONE,
                         1.0f, -1);
                i_this->dr.mMode = 1;
                actor->speed.y = REG0_F(8) + 120.0f;
                actor->speedF = REG0_F(9) + 40.0f;
            } else {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                         dRes_INDEX_BK_BAS_BK_JUMP1_e);
                i_this->dr.mMode = 10;
                actor->speed.y = REG8_F(8) + 120.0f;
                actor->speedF = REG8_F(9) + 40.0f;
            }

            /* HD: in Dragon Roost Cavern room 1 the bokoblin keeps its angle */
            bool keep = false;
            if (dComIfGp_checkStageName("M_NewD2") && fopAcM_GetRoomNo(actor) == 1) {
                keep = true;
            }
            if (!keep) {
                actor->current.angle.y = fopAcM_searchPlayerAngleY(actor);
            }

            fopAcM_monsSeStart(actor, JA_SE_CV_BK_SURPRISE, 0);
            i_this->mTimers[0] = 10;
            r29 = true;
        }
        break;
    case 1:
        if (i_this->dr.mAcch.ChkGroundHit()) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JATTACK2_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JATTACK2_e);
            fopAcM_monsSeStart(actor, JA_SE_CV_BK_ATTACK_L, 0);
        }
        break;
    case 2:
        r28 = true;
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 11;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JATTACK3_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JATTACK3_e);
        }
        break;
    case 10:
        if (i_this->mTimers[0] == 0 && i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode = 11;
            i_this->mSmokeTimer = l_bkHIO().m00C + 6;
            i_this->mSmokeType = 3;
        }
        break;
    case 11:
        cLib_addCalc0(&i_this->speedF, 1.0f, 20.0f);
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
        }
        break;
    case 20:
        if (i_this->mTimers[0] != 0) {
            if (i_this->mTimers[0] == 1) {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_SUWARI_e, 0.0f, J3DFrameCtrl::EMode_NONE,
                         1.0f, dRes_INDEX_BK_BAS_BK_SUWARI_e);
            }
        } else if (i_this->mpMorf->isStop()) {
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
        }
        break;
    }

    if (r29) {
        i_this->mWeaponSph.SetC(&actor->current.pos);
        i_this->mWeaponSph.SetR(20.0f);
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
    } else if (r28) {
        i_this->mWeaponSph.SetC(&i_this->mWeaponTipPos);
        i_this->mWeaponSph.SetR(60.0f);
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
    }
}

/* 020A5354 */
void z_demo_1(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    i_this->attention_info.flags = 0;
    i_this->mNoHitTimer = 10;

    switch (i_this->dr.mMode) {
    case 0:
        if (i_this->mTimers[0] == 0) {
            f32 f1 = cM_rndF(1.0f);
            if (f1 < 0.5f) {
                i_this->mTimers[0] = 72;
            } else {
                i_this->mTimers[0] = 90;
            }
            i_this->dr.mMode = 1;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_TUTUKU1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     dRes_INDEX_BK_BAS_BK_TUTUKU1_e);
        }
        break;
    case 1:
        if (i_this->mTimers[0] == 87 || i_this->mTimers[0] == 69 || i_this->mTimers[0] == 51 ||
            i_this->mTimers[0] == 33 || i_this->mTimers[0] == 15) {
            fopAcM_monsSeStart(actor, JA_SE_CV_BK_JUMP, 0);
        }
        if (i_this->mTimers[0] == 0) {
            f32 f1 = cM_rndF(1.0f);
            if (f1 < 0.5f) {
                i_this->mTimers[0] = 120;
            } else {
                i_this->mTimers[0] = 180;
            }
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_TUTUKU2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     -1);
        }
        break;
    case 2:
        if (i_this->mTimers[0] == 0) {
            f32 f1 = cM_rndF(1.0f);
            if (f1 < 0.5f) {
                i_this->mTimers[0] = 100;
            } else {
                i_this->mTimers[0] = 150;
            }
            i_this->dr.mMode = 0;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_TUTUKU3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     -1);
        }
        break;
    }

    f32 f1;
    if (i_this->mSightRangePrm != 0xFF) {
        f1 = (f32)i_this->mSightRangePrm * 10.0f;
    } else {
        f1 = 500.0f;
    }
    if (i_this->mPlayerDistance < f1 &&
        std::fabs(player->current.pos.y - actor->current.pos.y) < 250.0f) {
        i_this->mType = 0;
        i_this->dr.mAction = 1;
        i_this->dr.mMode = 20;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mTimers[1] = 30;
        fopAcM_monsSeStart(actor, JA_SE_CV_BK_FOUND_LINK, 0);
    }
}

/* 020A5674 */
void b_hang(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;

    i_this->actor_status &= ~(u32)fopAcStts_HD_80000_e; /* HD */
    bool r30 = false;
    fopAc_ac_c* r29 = nullptr;
    if (i_this->dr.mHangBridgeId != fpcM_ERROR_PROCESS_ID_e) {
        r29 = fopAcM_SearchByID(i_this->dr.mHangBridgeId);
        if (r29 == nullptr) {
            i_this->dr.mAction = 4;
            i_this->dr.mMode = 0;
            return;
        }
    }

    i_this->mCoCyl.OffCoSPrmBit(1); /* OffCoSetBit */
    i_this->dr.mBodyBgCheckDelay = 5;
    switch (i_this->dr.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_OTISOU1_e, REG12_F(9) + 10.0f, 2, 1.0f,
                 dRes_INDEX_BK_BAS_BK_OTISOU1_e);
        i_this->dr.mMode = 1;
        i_this->dr.mHangMoveSpeed = 0.0f;
        i_this->mTimers[0] = 100;
        if (i_this->mHoldWeapon != 0) {
            i_this->mDropWeapon = 1;
        }
        fopAcM_monsSeStart(actor, JA_SE_CV_BK_LOST_BOKO, 0);
        break;
    case 1:
        if ((i_this->mTimers[0] & 0x1F) == 0) {
            i_this->mSpitTimer = 2;
        }
        if (i_this->mTimers[0] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_OTISOU2_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_OTISOU2_e);
            i_this->dr.mMode = 2;
        }
        break;
    case 2:
        if (i_this->mpMorf->isStop()) {
            r30 = true;
        }
        break;
    }

    actor->speed.z = 0.0f;
    actor->speed.x = 0.0f;
    actor->speedF = 0.0f;
    actor->speed.y = 0.0f;
    cMtx_YrotS(calc_mtx(), i_this->dr.mHangAngle.y);
    gabi::Local<cXyz> sp18;
    sp18->x = 0.0f;
    sp18->y = REG12_F(10) + 25.0f;
    sp18->z = REG12_F(11) + 50.0f;
    gabi::Local<cXyz> sp0C;
    MtxPosition(sp18, sp0C);
    cLib_addCalcAngleS2(&actor->current.angle.y, i_this->dr.mHangAngle.y, 1, 0x1000);
    if (i_this->dr.mHangBridgeId != fpcM_ERROR_PROCESS_ID_e) {
        cLib_addCalc2(&actor->current.pos.x, i_this->dr.mpHangPos->x + sp0C->x, 1.0f, i_this->dr.mHangMoveSpeed);
        cLib_addCalc2(&actor->current.pos.y, i_this->dr.mpHangPos->y + sp0C->y, 1.0f, i_this->dr.mHangMoveSpeed);
        cLib_addCalc2(&actor->current.pos.z, i_this->dr.mpHangPos->z + sp0C->z, 1.0f, i_this->dr.mHangMoveSpeed);
        cLib_addCalc2(&i_this->dr.mHangMoveSpeed, 100.0f, 1.0f, 20.0f);

        bkActionBridgeSegment* r3 = (bkActionBridgeSegment*)(br_s*)i_this->mpBridge;
        r3->mPlankRideTimer = 1;
        r3->mRideRollTarget = i_this->dr.mBridgeRideRoll;
        r3->mRideSag = REG14_F(12) + -25.0f;

        if (((bkActionBridgeView*)r29)->mMoveProcMode >= 4) {
            r30 = true;
        }
    } else {
        cLib_addCalc2(&actor->current.pos.x, i_this->dr.mHangPos.x + sp0C->x, 1.0f, i_this->dr.mHangMoveSpeed);
        cLib_addCalc2(&actor->current.pos.y, i_this->dr.mHangPos.y, 1.0f, i_this->dr.mHangMoveSpeed);
        cLib_addCalc2(&actor->current.pos.z, i_this->dr.mHangPos.z + sp0C->z, 1.0f, i_this->dr.mHangMoveSpeed);
        cLib_addCalc2(&i_this->dr.mHangMoveSpeed, 100.0f, 1.0f, 20.0f);
    }

    if (r30) {
        i_this->dr.mAction = 4;
        i_this->dr.mMode = 0;
        i_this->dr.mBodyBgCheckDelay = 0;
        i_this->dr.mSpawnY = actor->current.pos.y;
        i_this->actor_status |= fopAcStts_HD_80000_e; /* HD */
    }
}

/* 020A5B74 */
void rope_on(bk_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* unused */
    (void)player;

    i_this->dr.mLookMode = 1;
    i_this->mNoHitTimer = 2;

    switch (i_this->dr.mMode) {
    case 0:
        i_this->speedF = 0.0f;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 2.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO2_e);
        i_this->dr.mMode = 1;
        i_this->mTimers[0] = 40;
        // Fall-through
    case 1:
        if (i_this->mTimers[0] == 0) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 2.0f,
                     dRes_INDEX_BK_BAS_BK_WALK_e);
        }
        break;
    case 2: {
        i_this->speedF = 70.0f;
        s16 targetAngle = fopAcM_searchPlayerAngleY(i_this);
        cLib_addCalcAngleS2(&i_this->current.angle.y, targetAngle, 4, 0x1000);
        if (i_this->dr.mAcch.ChkWallHit()) {
            i_this->speed.y = 100.0f + REG0_F(16);
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP1_e);
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
            i_this->dr.mMode = 3;
        }
        if (fopAcM_searchPlayerDistance(i_this) < 200.0f) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_SUWARI_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_SUWARI_e);
            i_this->dr.mMode = 5;
        }
        break;
    }
    case 3:
        i_this->speedF = 35.0f;
        if (i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode = 4;
        }
        break;
    case 4:
        i_this->speedF = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 2.0f,
                     dRes_INDEX_BK_BAS_BK_WALK_e);
        }
        break;
    case 5:
        i_this->speedF = 0.0f;
        if (fopAcM_searchPlayerDistance(i_this) > 250.0f) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 2.0f,
                     dRes_INDEX_BK_BAS_BK_WALK_e);
        }
        break;
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 10009468 "M_NewD2"
*/
// clang-format on
