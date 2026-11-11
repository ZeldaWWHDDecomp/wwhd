/**
 * d_a_bmd_move.cpp (WWHD)
 * Boss - Kalle Demos: move() (020B1DE4) with its inlined states (wait, attack_1, attack_2,
 * damage, eat, start, end, damage_check).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bmd.h"

#define ARC_BMD "Bmd" /* "Bmd" */

/* daPy_py_c virtuals (HD vtable at +0xB4) */
static inline void player_setThrowDamage(fopAc_ac_c* p, cXyz* pos, s16 angle, f32 speedF, f32 speedY,
                                         s32 mode) {
    daPy_py_c_setThrowDamage((daPy_py_c*)p, pos, angle, speedF, speedY, mode);
}
static inline void player_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 angle) {
    ((daPy_py_c*)p)->setPlayerPosAndAngle(pos, angle);
}
static inline f32 REG8_F(int i) { return REG_F(8, i); }
static inline f32 REG10_F(int i) { return REG_F(10, i); }
static inline f32 REG17_F(int i) { return REG_F(17, i); }

/* inline helpers of this unit */
static inline s16 frame_s16(mDoExt_McaMorf* m) { return (s16)gabi::ftoi(m->getFrame()); }

static inline void wait(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> local_30;
    gabi::Local<cXyz> local_3c;

    if (i_this->mTimers[3] == 1) {
        fopAcM_seStart(actor, JA_SE_CM_BKM_BODY_UP, 0);
    }
    switch (i_this->mSubMode) {
    case -1:
        i_this->mTurnTargetAngleY = actor->shape_angle.y;
        i_this->mTimers[1] = 100;
        i_this->mTimers[2] = 0x28;
        if (i_this->mTimers[0] == 0) {
            i_this->mSubMode = 0;
            i_this->mFlowerState = 0;
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_WAIT_e, 20.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            /* (the inline's actor check is folded: the actor was dereferenced above) */
            if (gabi::ea(&actor->eyePos) != 0) {
                mDoAud_seStart(JA_SE_CM_BKM_VINE_OUT, &actor->eyePos, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            }
            i_this->mTimers[3] = 0x5A;
        }
        break;
    case 0: {
        f32 hx = actor->home.pos.x;
        cXyz_fcopy(&i_this->mTargetPos, &actor->home.pos);
        i_this->mPetalAngle = -0x4000;
        i_this->mSubMode = 1;
        f32 r = cM_rndFX(500.0f);
        i_this->mTargetPos.x = hx + r;
        r = cM_rndFX(500.0f);
        i_this->mTargetPos.z = i_this->mTargetPos.z + r;
        {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&i_this->mTargetPos, tmp, &player->current.pos);
            local_3c->x = tmp->x;
            local_3c->y = tmp->y;
            local_3c->z = tmp->z;
        }
        f32 fVar7 = std_sqrtf(gabi::fmadds(local_3c->x, local_3c->x, local_3c->z * local_3c->z));
        if (fVar7 > REG0_F(11) + 1000.0f) {
            local_30->y = 0.0f;
            local_30->x = 0.0f;
            local_30->z = REG0_F(12) + 1300.0f;
            Mtx34* m = calc_mtx();
            s16 a = cM_atan2s(local_3c->x, local_3c->z);
            cMtx_YrotS(m, a);
            MtxPosition(local_30, local_3c);
            gabi::Local<cXyz> tmp;
            cXyz_pl(&player->current.pos, tmp, local_3c);
            i_this->mTargetPos.copy(*tmp);
        }
    }
        /* fallthrough */
    case 1: {
        f32 tmp = (f32)i_this->mVineCount * 0.15f;
        cLib_addCalc2(&actor->speedF, tmp, 1.0f, 0.1f);
        {
            gabi::Local<cXyz> t;
            cXyz_mi(&i_this->mTargetPos, t, &actor->current.pos);
            local_30->x = t->x;
            local_30->y = t->y;
            local_30->z = t->z;
        }
        cLib_addCalcAngleS2(&actor->current.angle.y, cM_atan2s(local_30->x, local_30->z), 0x10, 0x800);
        f32 fVar7 = std_sqrtf(gabi::fmadds(local_30->x, local_30->x, local_30->z * local_30->z));
        if (fVar7 < 50.0f) {
            i_this->mSubMode = 2;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(150.0f) + 150.0f);
        }
        break;
    }
    case 2:
        i_this->mIsResting = 1;
        cLib_addCalc0(&actor->speedF, 1.0f, 0.1f);
        if (i_this->mTimers[0] == 0) {
            i_this->mSubMode = 0;
        }
        break;
    }
    if (i_this->mSubMode >= 0) {
        move1(i_this);
        if (i_this->mTimers[1] == 0) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(200.0f) + 100.0f);
            i_this->mTurnTargetAngleY = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->mTurnSpeed = 0.0f;
        }
        cLib_addCalcAngleS2(&actor->shape_angle.y, i_this->mTurnTargetAngleY, 0x20, (s16)gabi::ftoi(i_this->mTurnSpeed));
        cLib_addCalc2(&i_this->mTurnSpeed, 64.0f, 1.0f, 1.0f);
        if (i_this->mTimers[2] == 0) {
            f32 dVar6 = fopAcM_searchPlayerDistance(actor);
            if (dVar6 < 1300.0f) {
                i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(50.0f) + 30.0f);
                if (cM_rndF(1.0f) < l_HIO().mMoveChance) {
                    i_this->mMode = 1;
                    i_this->mSubMode = 0;
                }
            } else {
                i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(50.0f) + 30.0f);
                if (cM_rndF(1.0f) < l_HIO().mMoveChance) {
                    i_this->mMode = 2;
                    i_this->mSubMode = 0;
                }
            }
        }
    }
}

static inline void attack_1(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player fetch */
    switch (i_this->mSubMode) {
    case 0:
        i_this->mVineCommand = 1;
        i_this->mTimers[0] = 0x32;
        i_this->mSubMode = 1;
        break;
    case 1:
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 0;
            i_this->mSubMode = 0;
        }
        break;
    }
    cLib_addCalc0(&actor->speedF, 1.0f, 0.1f);
    move1(i_this);
}

static inline void attack_2(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player fetch */
    switch (i_this->mSubMode) {
    case 0:
        i_this->mVineCommand = 2;
        i_this->mSubMode = 1;
        i_this->mAttackTimer = 10;
        anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_ATTACK_e, 20.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        break;
    case 1:
        if (i_this->mAttackTimer == 0) {
            i_this->mMode = 0;
            i_this->mSubMode = 0;
            i_this->mVineCommand = 0;
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_WAIT_e, 20.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        }
        break;
    }
    cLib_addCalc0(&actor->speedF, 1.0f, 0.1f);
    move1(i_this);
}

/* dComIfGp_particle_setToon (HD: dPa_control_c::set group 2); the room is read before the
 * play object */
static inline JPABaseEmitter* particle_setToon(u16 id, cXyz* pos, u8 alpha, dPa_smokeEcallBack* cb, s8 room) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, nullptr, nullptr, alpha, (dPa_levelEcallBack*)cb, room, nullptr,
                           nullptr, nullptr);
}
/* emitter->setGlobalRTMatrix(mtx): JPASetRMtxTVecfromMtx(mtx, mGlobalRot (+0x1F0), mGlobalTrs (+0x22C)) */
static inline void emitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    ((BmdEmitterView*)e)->setGlobalRTMatrix(m);
}

static const s32 openSmokeJoints[] = {26, 35, 44, 53, 62};

static inline void damage(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_40;
    gabi::Local<cXyz> local_4c;
    s8 bVar1 = false;
    local_40->x = 0.0f;
    local_40->y = 0.0f;
    local_40->z = 0.0f;
    switch (i_this->mSubMode) {
    case 0:
        actor->speed.y = 0.0f;
        actor->gravity = 0.0f;
        i_this->mSubMode = 1;
        i_this->mTimers[0] = 0x28;
        /* fallthrough */
    case 1:
        if (i_this->mTimers[0] == 0x1E) {
            i_this->mVineCommand = 3;
            actor->gravity = -3.0f;
        }
        if (!(actor->current.pos.y > i_this->mGroundY + 5.0f)) {
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_OTIRU_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mFlowerState = 1;
            i_this->mSubMode = 2;
            i_this->mCoreAngleY = fopAcM_searchPlayerAngleY(actor);
            i_this->mWindRadiusDelta = 2000.0f;
            dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMOPENHOUSHI00, &actor->current.pos);
            if (bmd_startStageName0() == 'X') {
                mDoAudioApi::bgmStart(JA_BGM_UNK_151);
            } else {
                mDoAudioApi::bgmStart(JA_BGM_UNK_105);
            }
        }
        break;
    case 2:
        if ((s16)(REG0_S(1) + 10) == frame_s16(i_this->mpBodyMorf) &&
            fopAcM_searchPlayerDistance(actor) < REG0_F(8) + 400.0f) {
            i_this->mBgOffTimer = 0x1E;
            i_this->mPushTimer = REG0_S(2) + 0x14;
        }
        if (frame_s16(i_this->mpBodyMorf) == 30) {
            for (s32 i = 0; i < 5; i++) {
                s8 room = fopAcM_GetRoomNo(actor);
                JPABaseEmitter* emitter = particle_setToon(
                    dPa_name::ID_AK_ST_BKMOPENSMOKE00, &actor->current.pos, 0xB9, &i_this->mSmokeCb[i], room);
                if (emitter != nullptr) {
                    J3DModel* model = i_this->mpBodyMorf->getModel();
                    emitter_setGlobalRTMatrix(emitter, model->getAnmMtx(openSmokeJoints[i]));
                }
            }
        }
        if (frame_s16(i_this->mpBodyMorf) == 10) {
            i_this->mWindRadiusDelta = 2000.0f;
        }
        if (i_this->mpBodyMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BMD_BCK_HIRAKU_WAIT_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->mSubMode = 3;
            i_this->mTimers[0] = l_HIO().mOpenTime;
            i_this->mPetalAngle = 0;
        }
        if (frame_s16(i_this->mpBodyMorf) > 25) {
            cLib_addCalc2(&i_this->mColBlend, 1.0f, 1.0f, 0.1f);
        }
        break;
    case 3:
        i_this->mCamTypeMode = 2;
        if (i_this->mTimers[0] == 0) {
            i_this->mSubMode = 4;
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_TOJIRU_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mFlowerState = 2;
            fopAcM_seStart(actor, JA_SE_CM_BKM_FLW_WAVING, 0);
        }
        cLib_addCalc2(&i_this->mColBlend, 1.0f, 1.0f, 0.1f);
        break;
    case 4: {
        if (frame_s16(i_this->mpBodyMorf) == 23) {
            fopAcM_seStart(actor, JA_SE_CM_BKM_BODY_CLOSE, 0);
            i_this->mDamageTimer = 200; /* HD */
        }
        if (frame_s16(i_this->mpBodyMorf) == 27) {
            dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMCLOSEHOUSHI00, &actor->current.pos);
            if (bmd_startStageName0() == 'X') {
                mDoAudioApi::bgmStart(JA_BGM_PAST_BKM);
            } else {
                mDoAudioApi::bgmStart(JA_BGM_KINDAN_BOSS);
            }
        }
        if (frame_s16(i_this->mpBodyMorf) <= 15) {
            cLib_addCalc2(&i_this->mColBlend, 1.0f, 1.0f, 0.1f);
        }
        if (frame_s16(i_this->mpBodyMorf) == 15) {
            i_this->mWindRadiusDelta = 2000.0f;
        }
        s16 sVar3;
        if (frame_s16(i_this->mpBodyMorf) < (s16)(REG0_S(5) + 0x14)) {
            sVar3 = REG0_S(3) + 200;
            i_this->mRideCaught = 0;
        } else {
            i_this->mFlowerState = 3;
            sVar3 = REG0_S(4) + 0x290;
            if ((i_this->mRideCaught != 0) && (REG0_S(9) == 0)) {
                i_this->mRideCaught = i_this->mRideCaught - 1;
                i_this->mMode = 5;
                i_this->mSubMode = 0;
                return;
            }
        }
        cLib_addCalcAngleS2(&i_this->mPetalAngle, -0x4000, 1, sVar3);
        if (i_this->mpBodyMorf->isStop()) {
            i_this->mSubMode = 5;
            i_this->mTimers[0] = 0x1E;
            bVar1 = true;
        }
        break;
    }
    case 5:
        cLib_addCalcAngleS2(&i_this->mPetalAngle, -0x4000, 1, 0x290);
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 0;
            i_this->mSubMode = -1;
            i_this->mTimers[0] = 0x3C;
            i_this->mDamageTimer = 0x46; /* HD */
            i_this->mVineCommand = 4;
            bVar1 = true;
        }
        break;
    }
    if ((i_this->mSubMode >= 2) && (i_this->mSubMode <= 4)) {
        i_this->mCoreEffTimers[0] = 3;
    }
    cLib_addCalcAngleS2(&actor->shape_angle.x, 0, 0x10, 0x200);
    cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 0x10, 0x200);
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    local_40->y = 0.0f;
    local_40->x = 0.0f;
    local_40->z = actor->speedF;
    MtxPosition(local_40, local_4c);
    actor->current.pos.x = actor->current.pos.x + local_4c->x;
    actor->current.pos.z = actor->current.pos.z + local_4c->z;
    actor->current.pos.y = actor->current.pos.y + actor->speed.y;
    actor->speed.y = actor->speed.y + actor->gravity;
    if (actor->current.pos.y < i_this->mGroundY) {
        actor->current.pos.y = i_this->mGroundY;
        if (actor->speed.y < -20.0f) {
            gabi::Local<cXyz> shock;
            shock->x = 0.0f;
            shock->y = 1.0f;
            shock->z = 0.0f;
            dVibration_c* vib = dComIfGp_getVibration();
            vib->StartShock(REG0_S(2) + 5, -0x21, shock.get());
            if (gabi::ea(&actor->eyePos) != 0) {
                mDoAud_seStart(JA_SE_CM_BKM_BODY_FALL, &actor->eyePos, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            }
            actor->speed.y = 10.0f;
            dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMDROPDOWNSOIL00, &actor->current.pos);
            s8 room = fopAcM_GetRoomNo(actor);
            particle_setToon(dPa_name::ID_AK_ST_BKMDROPDOWNSMOKE00, &actor->current.pos, 0xB9,
                             &i_this->mSmokeCb[5], room);
        } else {
            actor->speed.y = 0.0f;
        }
        cLib_addCalc0(&actor->speedF, 1.0f, 0.2f);
    }
    if (bVar1 && fopAcM_searchPlayerDistance(actor) < REG8_F(8) + 200.0f) {
        i_this->mMode = 5;
        i_this->mSubMode = 0;
        anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_TOJIRU_e, 1.0f, J3DFrameCtrl::EMode_NONE, 5.0f, -1);
    }
}

static inline void eat(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    switch (i_this->mSubMode) {
    case 0:
        i_this->mDemoMode = 1;
        i_this->mSubMode = 1;
        i_this->mBlurRate = 0x96;
        i_this->mFlowerState = 2;
        /* fallthrough */
    case 1:
        cLib_addCalcAngleS2(&i_this->mPetalAngle, -0x4000, 1, 0x1000);
        if (i_this->mpBodyMorf->isStop()) {
            i_this->mSubMode = 2;
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_EAT_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->mTimers[0] = 0x32;
            if (bmd_startStageName0() == 'X') {
                mDoAudioApi::bgmStart(JA_BGM_PAST_BKM);
            } else {
                mDoAudioApi::bgmStart(JA_BGM_KINDAN_BOSS);
            }
            fopAcM_seStart(actor, JA_SE_CM_BKM_BODY_EATING, 0);
        }
        break;
    case 2:
        if (i_this->mTimers[0] != 0) {
            return;
        }
        i_this->mSubMode = 3;
        anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_HAKIDASU_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        fopAcM_seStart(player, JA_SE_CM_BKM_LINK_OUT, 0);
        return;
    case 3:
        if (i_this->mDemoSubCount == REG0_S(2) + 0x50) {
            dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMHAKIDASHISMOKE00, &actor->current.pos);
            dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMHAKIDASHIHOUSHI01, &actor->current.pos);
            i_this->mBgOffTimer = 100;
        }
        if (i_this->mDemoSubCount == 0x55) {
            gabi::Local<cXyz> sp18;
            sp18->x = actor->current.pos.x;
            f32 y = actor->current.pos.y;
            sp18->y = y;
            sp18->z = actor->current.pos.z;
            sp18->y = y + (REG17_F(0) + 1000.0f);
            /* HD: the throw angle is +0x8000 (GameCube +0x5000) */
            player_setThrowDamage(player, sp18, (s16)((i_this->mDemoCamAngleY + REG0_S(6)) + 0x8000),
                                  REG17_F(1) + 50.0f, REG17_F(2) + 120.0f, 4);
            i_this->mPushTimer = REG0_S(2) + 0x14;
        }
        if (i_this->mpBodyMorf->isStop()) {
            i_this->mSubMode = 4;
        }
        break;
    case 4:
        if (i_this->mDemoMainCount > (s16)(REG0_S(7) + 0x82)) {
            i_this->mSubMode = 5;
            i_this->mTimers[0] = 0x1A;
        }
        break;
    case 5:
        if (fopAcM_searchPlayerDistanceXZ(actor) < 300.0f) {
            gabi::Local<cXyz> sp30;
            gabi::Local<cXyz> sp24;
            cMtx_YrotS(calc_mtx(), i_this->mDemoCamAngleY);
            sp30->x = 0.0f;
            sp30->y = 0.0f;
            sp30->z = 350.0f;
            MtxPosition(sp30, sp24);
            PSVECAdd(sp24, &actor->current.pos, sp24); /* sp24 += current.pos */
            player_setPlayerPosAndAngle(player, sp24, player->shape_angle.y);
        }
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 0;
            i_this->mSubMode = -1;
            i_this->mTimers[0] = 0x3C;
            i_this->mDemoMode = 0x96;
            dComIfGp_event_reset();
            i_this->mPetalAngle = -0x4000;
            i_this->mVineCommand = 4;
            i_this->mBgOffTimer = 0;
            i_this->mBlurRate = 1;
        }
        break;
    }
}

static inline void start_posMk(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->mCoreAngleY = fopAcM_searchPlayerAngleY(actor);
    cMtx_YrotS(calc_mtx(), i_this->mCoreAngleY);
    gabi::Local<cXyz> local_28;
    local_28->x = 0.0f;
    local_28->y = REG0_F(1) + 20.0f;
    local_28->z = REG0_F(2) + 90.0f;
    MtxPosition(local_28, &i_this->mMakarPos);
    PSVECAdd(&i_this->mMakarPos, &actor->current.pos, &i_this->mMakarPos); /* mMakarPos += current.pos */
}

static inline void start(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    i_this->mDamageTimer = 8;
    switch (i_this->mSubMode) {
    case 0: {
        anm_init(i_this, dRes_INDEX_BMD_BCK_HIRAKU_WAIT_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes(ARC_BMD, dRes_INDEX_BMD_BCK_COA_START1_e, SAFESTRING_VTBL);
        i_this->mpHeadMorf->setAnm(anm, 2, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
        i_this->mMakarMode = 1;
        i_this->mSubMode = 1;
        i_this->mFlowerState = 2;
    }
        /* fallthrough */
    case 1:
        i_this->mColBlend = 1.0f;
        start_posMk(i_this);
        i_this->mMakarAngleY = i_this->mCoreAngleY;
        /* HD: REG10_F(0) + 700 (GameCube 500) */
        if (fopAcM_searchPlayerDistance(actor) < REG10_F(0) + 700.0f) {
            i_this->mSubMode = 2;
            i_this->mDemoMode = 5;
            i_this->mPetalAngle = -0x4000;
            mDoAudioApi::bgmStreamPlay();
            i_this->mFlowerState = 0; /* HD */
        }
        break;
    case 2:
        i_this->mBgOffTimer = 0x1E;
        start_posMk(i_this);
        i_this->mColBlend = 1.0f;
        i_this->mMakarAngleY = i_this->mCoreAngleY;
        break;
    case 3:
        i_this->mBgOffTimer = 0x1E;
        if (i_this->mDemoMainCount < 0xFA) {
            i_this->mColBlend = 1.0f;
        }
        if (i_this->mDemoMainCount == 0x78) {
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_START1_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            fopAcM_seStart(actor, JA_SE_CM_BKM_FLW_WAVING, 0);
        }
        if (i_this->mDemoMainCount == 0x96) {
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_START2_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mWindRadiusDelta = 2000.0f;
            fopAcM_seStart(actor, JA_SE_CM_BKM_FLW_TO_BUD, 0);
        }
        if (i_this->mDemoMainCount == 0xB1) {
            dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMCLOSEHOUSHI00, &actor->current.pos);
        }
        if (i_this->mDemoMainCount >= 0x96) {
            if (i_this->mVineCount > 0) {
                move1(i_this);
            }
            if (i_this->mDemoMainCount == 0xBE) {
                anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_WAIT_e, 20.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            }
        }
        if (i_this->mDemoMainCount == 0xD2) {
            i_this->mVineCommand = 5;
            fopAcM_seStart(actor, JA_SE_CM_BKM_VINE_OUT, 0);
        }
        if (i_this->mDemoMainCount == 0x1C2) {
            i_this->mVineCommand = 6;
            fopAcM_seStart(actor, JA_SE_CM_BKM_ATKVINE_ENTER, 0);
        }
        if (i_this->mDemoMainCount == 0x168) {
            fopAcM_seStart(actor, JA_SE_CM_BKM_BODY_UP, 0);
        }
        break;
    }
}

/* fopAcM_seStartCurrent (HD inline: null checks on the actor and its position) */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

static const s32 witherJoints[] = {25, 34, 43, 52, 61};

static inline void end(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mFlowerState = 1;
    i_this->mPetalAngle = 0;
    switch (i_this->mSubMode) {
    case 0:
        if (bmd_startStageName0() == 'X') {
            dLib_setNextStageBySclsNum(0, (s32)fopAcM_GetRoomNo(actor));
            mDoAud_seStart(JA_SE_LK_B_BOSS_WARP, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            dComIfGs_onEventBit(0x3220); /* dComIfGs_onEventBit(KALLE_DEMOS_TRIALS_CLEAR) */
            dComIfGs_onTmpBit(0x480);    /* dComIfGs_onTmpBit(UNK_0480) */
            return;
        }
        mDoAudioApi::bgmStreamPrepare(JA_STRM_BOSS_CLEAR);
        anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_DEAD1_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->mVineCommand = 8;
        i_this->mSubMode = 1;
        i_this->mBgOffTimer = 30000;
        /* fallthrough */
    case 1:
        if (i_this->mDemoMainCount < 0x1A4) {
            i_this->mColBlend = 1.0f;
        }
        if (i_this->mDemoMainCount == REG0_S(2) + 300) {
            ((daPy_py_c*)player)->mDemo.mDemoMode =
                daPy_demo_c::DEMO_UNK_029_e; /* player->changeDemoMode(DEMO_UNK_029_e) */
        }
        if (i_this->mDemoMainCount == REG0_S(4) + 0x17C) {
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_DEAD2_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            fopAcM_seStartCurrent(actor, JA_SE_CM_BKM_END_FLW_WAVING, 0);
        }
        if (i_this->mDemoMainCount == REG0_S(5) + 0x1C2) {
            anm_init(i_this, dRes_INDEX_BMD_BCK_HANA_DEAD3_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            fopAcM_seStartCurrent(actor, JA_SE_CM_BKM_END_FLW_LIFTUP, 0);
            i_this->mVineCommand = 9;
            i_this->mWindRadiusDelta = 2000.0f;
            for (s32 i = 0; i < 5; i++) {
                JPABaseEmitter* emitter =
                    dComIfGp_particle_set(dPa_name::ID_AK_SN_BKMHANAKARERU00, &actor->current.pos);
                if (emitter != nullptr) {
                    J3DModel* model = i_this->mpBodyMorf->getModel();
                    emitter_setGlobalRTMatrix(emitter, model->getAnmMtx(witherJoints[i]));
                }
            }
        }
        if (i_this->mDemoMainCount == 0x230) {
            fopAcM_seStartCurrent(actor, JA_SE_CM_BKM_END_FLW_DIE, 0);
        }
        if (i_this->mDemoMainCount == REG0_S(6) + 0x276) {
            bmd_onDungeonItem(dSv_memBit_c::STAGE_BOSS_ENEMY);
            fopAcMApi::createWarpFlower(&actor->current.pos, 0, (s32)fopAcM_GetRoomNo(actor), 0);
            i_this->mCamTypeMode = 0;
            i_this->mSubMode = 2;
        }
    case 2:
        break;
    }
}

/* 020B1DE4 */
void move(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player fetch */
    actor->attention_info.flags = 0;
    i_this->mVineCommand = 0;
    switch (i_this->mMode) {
    case 0:
        wait(i_this);
        break;
    case 1:
        attack_1(i_this);
        break;
    case 2:
        attack_2(i_this);
        break;
    case 3:
        damage(i_this);
        break;
    case 5:
        eat(i_this);
        break;
    case 10:
        start(i_this);
        break;
    case 11:
        end(i_this);
        break;
    }
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    if (i_this->mMode != 3) {
        f32 fVar1 = 0.0f;
        if (0 < i_this->mVineCount) {
            fVar1 = gabi::fmadds((f32)i_this->mVineCount, REG0_F(17) + 2.5f, 250.0f) + REG0_F(18);
            if (i_this->mMode == 10) {
                fVar1 += 100.0f;
            }
        }
        cLib_addCalc2(&actor->current.pos.y, (actor->home.pos.y + i_this->mBobHeight) + fVar1, 0.1f, 5.0f);
        dComIfGp_get(); /* HD: an unused player fetch */
        /* damage_check */
        if ((i_this->mVineCount == 0) && (i_this->mVineCutTimer == 1)) {
            i_this->mMode = 3;
            i_this->mSubMode = 0;
            fopAcM_monsSeStart(actor, JA_SE_CV_BKM_CUT_VINE_ALL, 0);
        }
    } else {
        i_this->mBobHeight = 0.0f;
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data witherJoints 101919F0
data openSmokeJoints 101919DC
str 10009A8C in 020B1DE4
*/
// clang-format on
