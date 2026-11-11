/**
 * d_a_bgn2_exec.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 2): daBgn2_Execute
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn2.cpp) to the WWHD layout and verified against cking.rpx.
 * WWHD inlines move() with its action functions (start, plesattack, jumpattack, mahi, damage,
 * hensin), asi_eff_set, attack_eff_set, ki_set, damage_check and ki_check into daBgn2_Execute;
 * they are inline helpers here, in the GameCube structure.
 */
#include "d/actor/d_a_bgn2.h"

/* asi_eff_set (inlined)
 * HD: the six feet are mLegPos[i * 5 + 4] (GameCube i * 5 + 8, past the end of the array) */
/* inlined into daBgn2_Execute */
static inline void asi_eff_set(bgn2_class* i_this) {
    gabi::Local<cXyz> local_1c;
    gabi::Local<cXyz> local_28;

    local_28->x = 0.0f;
    local_28->y = 0.0f;
    local_28->z = 0.0f;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG2, local_28, &bg_tevstr);
    for (int i = 0; i < 6; i++) {
        int j = i * 5 + 4;
        local_1c->copy(i_this->mLegPos[j]);
        if (!gr_check(i_this, local_1c)) {
            dComIfGp_particle_setSimple(ID_AK_SN_O_KGTT2JUMPHANDSPLASH00, local_1c, 0xFF);
        } else {
            dComIfGp_particle_setSimple(ID_AK_ST_O_KGTT2JUMPHANDSMOKE00, local_1c, 0xFF);
            if (!(i_this->mFrameCounter & 1)) {
                dComIfGp_particle_setSimple(ID_AK_SN_O_KGTCOMMONHAMON00, local_1c, 0xFF);
            }
        }
    }
}

/* attack_eff_set (inlined; the position is passed by value) */
/* inlined into daBgn2_Execute */
static inline void attack_eff_set(bgn2_class* i_this, cXyz* param_2) {
    JPABaseEmitter* emitter;
    gabi::Local<cXyz> local_28;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (!gr_check(i_this, param_2)) {
        J3DModel* bodyModel = i_this->mpBodyMorf->getModel();
        emitter = dComIfGp_particle_set(ID_AK_SN_KGTT2CHESTSPLASH00, param_2);
        if (emitter != nullptr) {
            JPABaseEmitter_setGlobalRTMatrix(
                emitter, model_getAnmMtx(bodyModel, BGN_KUMO1_JNT_J_BGN2_KARADA1_e));
        }
        emitter = dComIfGp_particle_set(ID_AK_SN_KGTT2STOMACHSPLASH00, param_2);
        if (emitter != nullptr) {
            JPABaseEmitter_setGlobalRTMatrix(
                emitter, model_getAnmMtx(bodyModel, BGN_KUMO1_JNT_J_BGN2_KARADA2_e));
        }
        fopAcM_seStart(player, JA_SE_CM_BGN_T_FALL_WATER, 0);
    } else {
        fopAcM_seStart(player, JA_SE_CM_BGN_T_FALL, 0);
        for (int i = 0; i < 16; i++) {
            f32 r = cM_rndFX(500.0f);
            local_28->x = param_2->x + r;
            r = cM_rndF(100.0f);
            local_28->y = param_2->y + r;
            r = cM_rndFX(500.0f);
            local_28->z = param_2->z + r;
            dComIfGp_particle_setSimple(ID_AK_ST_O_KGTT2JUMPHANDSMOKE00, local_28, 0xFF);
        }
    }
}

static inline BOOL morf_isStop(mDoExt_McaMorf* m) { return m->isStop(); }

/* start (inlined) */
/* inlined into daBgn2_Execute */
static inline void start(bgn2_class* i_this) {
    dComIfGp_get(); /* HD: an unused play-object lookup */
    switch (i_this->mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BGN_BCK_DERU1_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mMode = 1;
        // fallthrough
    case 1:
        if (morf_isStop(i_this->mpBodyMorf)) {
            i_this->mAction = 1;
            anm_init(i_this, dRes_INDEX_BGN_BCK_RAKKA1_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     -1);
            i_this->mMode = 1;
        }
        break;
    }
}

/* ki_set (inlined) */
/* inlined into daBgn2_Execute */
static inline void ki_set(bgn2_class* i_this) {
    if (i_this->mKiSetTimer != 0) {
        return;
    }
    i_this->mKiSetCount = (s8)l_HIO.mKiPerLanding;
    i_this->mKiSetTimer = l_HIO.mKiSetInterval;
}

/* plesattack (inlined) */
/* inlined into daBgn2_Execute */
static inline void plesattack(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 bVar1 = false;
    switch (i_this->mMode) {
    case 0:
        if (actor->current.pos.y > l_HIO.mPressHeight + 2.0f) {
            cLib_addCalc2(&actor->current.pos.y, l_HIO.mPressHeight, 0.1f, l_HIO.mPressRiseStep);
            actor->speed.y = 0.0f;
            actor->speedF = REG0_F(9) + 30.0f;
            break;
        }
        i_this->mSpinSpeedTarget = 0;
        anm_init(i_this, dRes_INDEX_BGN_BCK_RAKKA1_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mMode = 1;
        if (actor->health == 3) {
            i_this->mTimers[0] = l_HIO.mPressHangTimeHealth3;
        } else if (actor->health == 2) {
            i_this->mTimers[0] = l_HIO.mPressHangTimeHealth2;
        } else {
            i_this->mTimers[0] = l_HIO.mPressHangTimeHealth1;
        }
        // fallthrough
    case 1:
        if (morf_isStop(i_this->mpBodyMorf)) {
            anm_init(i_this, dRes_INDEX_BGN_BCK_RAKKA2_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mMode = 2;
        }
        // fallthrough
    case 2:
        if (i_this->mTimers[0] != 0) {
            actor->speed.y = actor->speed.y - l_HIO.mGravity;
            if (i_this->mTimers[0] == 1) {
                fopAcM_seStart(actor, JA_SE_CM_BGN_T_FALL_WIND, 0);
            }
        }
        i_this->mBodyAtOn = 1;
        i_this->mLegAtOn = 1;
        if (checkGround(i_this)) {
            ki_set(i_this);
            anm_init(i_this, dRes_INDEX_BGN_BCK_SETTI1_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mMode = 3;
            bgn2_StartShock(5);
            if (actor->health == 3) {
                i_this->mTimers[0] = l_HIO.mPressLandTimeHealth3;
            } else if (actor->health == 2) {
                i_this->mTimers[0] = l_HIO.mPressLandTimeHealth2;
            } else {
                i_this->mTimers[0] = l_HIO.mPressLandTimeHealth1;
            }
            gabi::Local<cXyz> pos;
            pos->x = actor->current.pos.x;
            pos->y = actor->current.pos.y;
            pos->z = actor->current.pos.z;
            attack_eff_set(i_this, pos);
        }
        break;
    case 3:
        asi_hamon_set(i_this);
        if (i_this->mTimers[0] > 10) {
            bVar1 = true;
        }
        if (i_this->mTimers[0] == 0) {
            i_this->mAction = 2;
            i_this->mMode = 0;
            actor->speed.y = 0.0f;
            return;
        }
    }
    i_this->mTargetPos.x = player->current.pos.x;
    i_this->mTargetPos.y = player->current.pos.y;
    i_this->mTargetPos.z = player->current.pos.z;
    actor->current.angle.y =
        cM_atan2s(i_this->mTargetPos.x - actor->current.pos.x, i_this->mTargetPos.z - actor->current.pos.z);
    pos_move(i_this);
    cLib_addCalc0(&actor->speedF, 1.0f, 2.0f);
    actor->current.pos.y = actor->current.pos.y + actor->speed.y;
    actor->speed.y = actor->speed.y + l_HIO.mGravity;
    checkGround(i_this);
    if (bVar1) {
        cLib_addCalc2(&i_this->mRopeSway, REG0_F(9) + 250.0f, 1.0f, REG0_F(10) + 50.0f);
    }
}

/* jumpattack (inlined) */
/* inlined into daBgn2_Execute */
static inline void jumpattack(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_34;

    dComIfGp_get(); /* HD: an unused play-object lookup */
    int frame = gabi::ftoi(i_this->mpBodyMorf->getFrame());
    i_this->mSpinSpeedTarget = 0;
    J3DModel* bodyModel = i_this->mpBodyMorf->getModel();
    switch (i_this->mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BGN_BCK_JUMP1_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mMode = 1;
        // fallthrough
    case 1:
        actor->speedF = 0.0f;
        if (frame == 6) {
            i_this->mpPotaEmitter[0] =
                dComIfGp_particle_set(ID_AK_SN_KGTT2CHESTPOTA00, &actor->current.pos);
            i_this->mpPotaEmitter[1] =
                dComIfGp_particle_set(ID_AK_SN_KGTT2STOMACHPOTA00, &actor->current.pos);
        }
        if (frame >= 5 && frame <= 20) {
            asi_eff_set(i_this);
        }
        if (frame == REG0_S(0) + 0x14) {
            actor->speed.y = REG0_F(8) + 250.0f;
            i_this->mMode = 2;
            fopAcM_seStart(actor, JA_SE_CM_BGN_T_JUMP_UP, 0);
        } else {
            break;
        }
        // fallthrough
    case 2:
        actor->speedF = REG0_F(9) + 50.0f;
        break;
    }
    if (i_this->mpPotaEmitter[0] != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(
            i_this->mpPotaEmitter[0], model_getAnmMtx(bodyModel, BGN_KUMO1_JNT_J_BGN2_KARADA1_e));
    }
    if (i_this->mpPotaEmitter[1] != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(
            i_this->mpPotaEmitter[1], model_getAnmMtx(bodyModel, BGN_KUMO1_JNT_J_BGN2_KARADA2_e));
    }
    actor->current.angle.y = cM_atan2s(-actor->current.pos.x, -actor->current.pos.z);
    local_34->copy(actor->current.pos);
    local_34->y = 0.0f;
    actor->current.pos.y = actor->current.pos.y + actor->speed.y;
    actor->speed.y = actor->speed.y - (REG0_F(10) + 8.0f);
    if (!(actor->speed.y > 0.0f)) {
        actor->speed.y = 0.0f;
        if (i_this->mMode >= 2) {
            for (int i = 0; i < 2; i++) {
                if (i_this->mpPotaEmitter[i] != nullptr) {
                    JPABaseEmitter_becomeInvalidEmitter(i_this->mpPotaEmitter[i]);
                    i_this->mpPotaEmitter[i] = nullptr;
                }
            }
            if (pos_move(i_this) || std_sqrtf(PSVECSquareMag(local_34)) < 500.0f) {
                i_this->mAction = 1;
                i_this->mMode = 0;
                if (actor->health == 3) {
                    i_this->mSpinSpeedTarget = l_HIO.mSpinSpeedHealth3;
                } else if (actor->health == 2) {
                    i_this->mSpinSpeedTarget = l_HIO.mSpinSpeedHealth2;
                } else {
                    i_this->mSpinSpeedTarget = l_HIO.mSpinSpeedHealth1;
                }
                if (cM_rndF(1.0f) < 0.5f) {
                    i_this->mSpinSpeedTarget = (s16)-i_this->mSpinSpeedTarget;
                }
            }
        }
    }
    checkGround(i_this);
}

/* mahi (inlined) */
/* inlined into daBgn2_Execute */
static inline void mahi(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused play-object lookup */
    switch (i_this->mMode) {
    case 0:
        fopAcM_monsSeStart(actor, JA_SE_CV_BGN_HIT_2, 0);
        i_this->mTimers[0] = l_HIO.mMahiTime;
        i_this->mMode = 1;
        // fallthrough
    case 1:
        i_this->mpHeadMorf->play(&actor->current.pos, 0, 0);
        if (i_this->mTimers[0] == 0) {
            i_this->mAction = 2;
            i_this->mMode = 0;
            actor->speed.y = 0.0f;
            actor->current.angle.y = actor->shape_angle.y;
        }
        break;
    }
    asi_hamon_set(i_this);
    checkGround(i_this);
}

/* damage (inlined) */
/* inlined into daBgn2_Execute */
static inline void damage(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused play-object lookup */
    i_this->mpHeadMorf->play(&actor->current.pos, 0, 0);
    switch (i_this->mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BGN_BCK_DAMAGE1_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mMode = 1;
        // fallthrough
    case 1:
        if (morf_isStop(i_this->mpBodyMorf)) {
            i_this->mMode = 2;
        }
        // fallthrough
    case 2:
        if (actor->current.pos.y > 1490.0f && i_this->mMode == 2) {
            i_this->mAction = 2;
            actor->speed.y = REG0_F(8) + 250.0f;
            i_this->mMode = 2;
            actor->current.angle.y = actor->shape_angle.y;
        } else {
            cLib_addCalc2(&i_this->mRopeSway, REG0_F(11) + 250.0f, 1.0f, REG0_F(12) + 100.0f);
        }
        break;
    }
    cLib_addCalc2(&actor->current.pos.y, 1500.0f, 0.2f, 200.0f);
    i_this->mSpinSpeedTarget = 0x800;
    i_this->mSpinSpeed = 0x800;
}

/* debug registers of child 8 (REG8_F(i), REG8_S(i)) */
static inline f32 REG8_F(int i) { return REG_F(8, i); }
static inline s16 REG8_S(int i) { return REG_S(8, i); }

/* csXyz assignment (GHS: halfword copies) */
static inline void csXyz_copy(csXyz* d, const csXyz* s) {
    d->x = (s16)s->x;
    d->y = (s16)s->y;
    d->z = (s16)s->z;
}

/* hensin (inlined) */
/* inlined into daBgn2_Execute */
static inline void hensin(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_2c;

    cLib_addCalc2(&i_this->mMovePos.x, 0.0f, 0.05f, 50.0f);
    cLib_addCalc2(&i_this->mMovePos.z, 0.0f, 0.05f, 50.0f);
    i_this->mSpinSpeedTarget = 0;
    i_this->mSpinSpeed = 0;
    switch (i_this->mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BGN_BCK_SETTI1_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mMode = 1;
        bgn.get()->mCSMode = 10;
        actor->speed.y = REG8_F(0xc) + 50.0f;
        i_this->mTimers[0] = 0x30;
        break;
    case 1:
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 2;
            anm_init(i_this, dRes_INDEX_BGN_BCK_WAIT2_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->mTimers[0] = REG8_S(5) + 0x5a;
            actor->speed.y = 0.0f;
        }
        break;
    case 2:
        if (gabi::ftoi(i_this->mpBodyMorf->getFrame()) == 3) {
            fopAcM_seStart(actor, JA_SE_CM_BGN_T_SLIDE, 0);
        }
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 5;
            i_this->mTimers[0] = 0x46;
            anm_init(i_this, dRes_INDEX_BGN_BCK_MODORU1_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                     -1);
            fopAcM_seStart(actor, JA_SE_CM_BGN_T_TO_M_1, 0);
        }
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        if (i_this->mTimers[0] == 0x32) {
            bgn.get()->mBlurRate = 0x96;
        }
        if (i_this->mTimers[0] <= 0x32) {
            cLib_addCalc2(&i_this->mFogFade, 1.0f, 1.0f, 0.04f);
            if (i_this->mFogFade > 0.99f) {
                bgn.get()->mFormRequest = 1;
                {
                    be<s8>* mode = &bgn.get()->mCSMode;
                    *mode = (s8)(*mode + 1);
                }
                csXyz_copy(&bgn3.get()->current.angle, &actor->shape_angle);
                csXyz_copy(&bgn3.get()->shape_angle, &actor->shape_angle);
                bgn3.get()->current.pos.copy(actor->current.pos);
                bgn3.get()->mFogFade = 1.0f;
                f32 s = REG0_F(4) + 10.0f;
                local_2c->x = s;
                local_2c->z = s;
                local_2c->y = s;
                dComIfGp_particle_set(ID_AK_JN_SIBOUBAKUEN, &bgn3.get()->current.pos, nullptr,
                                      local_2c);
                dComIfGp_particle_set(ID_AK_JN_SIBOUFLASH, &bgn3.get()->current.pos, nullptr,
                                      local_2c);
                mDoAud_seStart(JA_SE_CM_BGN_METAM_EXPLODE, nullptr, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                bgn2_StartShock(8);
                mDoAud_bgmStart_l(0x8000005B /* JA_BGM_BGN_HAYAMUSHI */);
            }
        }
        break;
    }
    actor->current.pos.y = actor->current.pos.y + actor->speed.y;
    actor->speed.y = actor->speed.y + l_HIO.mGravity;
    asi_hamon_set(i_this);
    checkGround(i_this);
}

/* move (inlined)
 * HD: the rope segments are 80 apart (GameCube 50) */
/* inlined into daBgn2_Execute */
static inline void move(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 fVar1;
    gabi::Local<cXyz> local_a0;
    gabi::Local<cXyz> cStack_ac;

    dComIfGp_get(); /* HD: an unused play-object lookup */
    switch (i_this->mAction) {
    case 0:
        start(i_this);
        break;
    case 1:
        plesattack(i_this);
        move_se_set(i_this);
        break;
    case 2:
        jumpattack(i_this);
        move_se_set(i_this);
        break;
    case 4:
        mahi(i_this);
        break;
    case 5:
        damage(i_this);
        break;
    case 6:
        hensin(i_this);
        move_se_set(i_this);
    case 10:
        break;
    }
    cLib_addCalc2(&actor->current.pos.x, i_this->mMovePos.x, 0.05f, 1000.0f);
    cLib_addCalc2(&actor->current.pos.z, i_this->mMovePos.z, 0.05f, 1000.0f);
    actor->shape_angle.y = (s16)(actor->shape_angle.y + i_this->mSpinSpeed);
    fVar1 = (f32)(s16)i_this->mSpinSpeed;
    if (fVar1 < 0.0f) {
        fVar1 = -fVar1;
    }
    fVar1 *= 0.02631579f;
    if (fVar1 > 100.0f) {
        fVar1 = 100.0f;
    }
    fopAcM_seStart(actor, JA_SE_CM_BGN_T_ROUND, f2u(fVar1));
    cLib_addCalcAngleS2(&i_this->mSpinSpeed, i_this->mSpinSpeedTarget, 1, 100);
    {
        Mtx34* m = model_getAnmMtx(i_this->mpBodyMorf->getModel(), BGN_KUMO1_JNT_J_BGN2_KARADA1_e);
        PSMTXCopy(m, calc_mtx());
    }
    local_a0->x = 0.0f;
    local_a0->y = 0.0f;
    local_a0->z = 0.0f;
    MtxPosition(local_a0, cStack_ac);
    cLib_addCalc0(&i_this->mRopeSway, 1.0f, 25.0f);
    u32 lines = i_this->mRedRopeMat.mpLines;
    u32 pcVar8 = bgn2_linePositions(lines); /* getPos(0) */
    u32 pcVar7 = bgn2_lineSizes(lines);     /* getSize(0) */
    gabi::Local<cXyz> sum;
    for (int i = 0; i < 60; i++) {
        f32 s = cM_fsin(0.053247336f * (f32)i);
        f32 dVar9 = s * i_this->mRopeSway;
        dVar9 = dVar9 * (0.01666667f * (f32)(59 - i));
        s32 mFrameCounter = i_this->mFrameCounter;
        local_a0->x = cM_ssin(mFrameCounter * (REG0_S(3) + 300) + i * (REG0_S(4) + 2000)) * dVar9;
        local_a0->y = 0.0f;
        local_a0->z = cM_ssin(mFrameCounter * (REG0_S(5) + 0xfa) + i * (REG0_S(6) + 2000)) * dVar9;
        cXyz_pl(cStack_ac, sum, local_a0);
        bgn2_setLinePoint(pcVar8, i, sum);
        bgn2_setLineSize(pcVar7, i, (u8)(REG0_S(3) + 10));
        cStack_ac->y = cStack_ac->y + 80.0f;
    }
}

/* damage_check (inlined) */
/* inlined into daBgn2_Execute */
static inline void damage_check(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    char cVar5;
    JPABaseEmitter* emitter;
    /* local_78 and atInfo are adjacent in the frame (the angle is compared as a 12-byte object) */
    gabi::Local<frame_l> fr;
    csXyz* local_78 = &fr->local_78;
    CcAtInfo_l* atInfo = &fr->atInfo;
    gabi::Local<cXyz> local_58;
    gabi::Local<cXyz> flush;

    dComIfGp_get(); /* HD: an unused play-object lookup */
    if (i_this->mDamageTimer == 0) {
        atInfo->pParticlePos = nullptr;
        if (i_this->mHeadSph.ChkTgHit()) {
            atInfo->mpObj = gabi::ea(i_this->mHeadSph.GetTgHitObj());
            at_power_check(atInfo);
            if (atInfo->mResultingAttackType == 9 || atInfo->mResultingAttackType == 2) {
                i_this->mAction = 4;
                i_this->mMode = 0;
                i_this->mDamageTimer = 0x14;
                i_this->mShakeTimer = REG0_S(5) + 0x14;
                bgn2_setPauseTimer(4); /* dScnPly_ply_c::setPauseTimer(4) */
                dComIfGp_particle_set(ID_AK_JN_CRITICALHITFLASH, i_this->mHeadSph.GetTgHitPosP());
                local_58->x = 2.0f;
                local_58->y = 2.0f;
                local_58->z = 2.0f;
                local_78->z = 0;
                local_78->x = 0;
                local_78->y = fopAcM_searchPlayerAngleY(actor);
                dComIfGp_particle_set(ID_AK_JN_OK, i_this->mHeadSph.GetTgHitPosP(), local_78,
                                      local_58);
                cXyz* hp = i_this->mHeadSph.GetTgHitPosP();
                flush->x = hp->x;
                flush->y = hp->y;
                flush->z = hp->z;
                dKy_SordFlush_set(flush, 1);
                bgn2_StartShock(3);
                def_se_set(actor, atInfo->mpObj, 0x40);
                return;
            }
        }
        if (i_this->mCoreSph.ChkTgHit()) {
            i_this->mDamageTimer = 0x14;
            i_this->mCoreHitFrame = 1;
            i_this->mArrowHitFlashTimer = 30;
            i_this->mAction = 5;
            i_this->mMode = 0;
            if (actor->health != 0) {
                actor->health = actor->health - 1;
                mDoAud_seStart(JA_SE_LK_ARROW_HIT, nullptr, 0x35,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                f32 dVar8 = REG0_F(5) + 2.0f;
                if (actor->health == 0) {
                    /* mDoAud_bgmStop(30); HD: also clears a play-object float (+0x5B44) */
                    bgn2_clearPhaseChange();
                    mDoAud_bgmStop_l(30);
                    mDoAud_monsSeStart(JA_SE_CV_BGN_HIT_2, nullptr, 0,
                                       dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    mDoAud_seStart(JA_SE_LK_LAST_HIT, nullptr, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    mDoAud_seStart(JA_SE_CM_BGN_M_BRK_ORB, nullptr, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    i_this->mAction = 6;
                    i_this->mMode = 0;
                    emitter = dComIfGp_particle_set(ID_AK_SN_KGTBREAKWEAKPOINT00, &i_this->mCorePos);
                    if (emitter != nullptr) {
                        JPABaseEmitter_setGlobalScale(emitter, dVar8, dVar8, dVar8);
                    }
                    emitter = dComIfGp_particle_set(ID_AK_SN_KGTBREAKWEAKPOINT01, &i_this->mCorePos);
                    if (emitter != nullptr) {
                        JPABaseEmitter_setGlobalScale(emitter, dVar8, dVar8, dVar8);
                    }
                    for (int i = 0; i < 2; i++) {
                        if (i_this->mpPotaEmitter[i] != nullptr) {
                            JPABaseEmitter_becomeInvalidEmitter(i_this->mpPotaEmitter[i]);
                            i_this->mpPotaEmitter[i] = nullptr;
                        }
                    }
                } else {
                    mDoAud_monsSeStart(JA_SE_CV_BGN_HIT_1, nullptr, 0,
                                       dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    emitter = dComIfGp_particle_set(ID_AK_SN_KGTHITWEAKPOINT00, &i_this->mCorePos);
                    if (emitter != nullptr) {
                        JPABaseEmitter_setGlobalScale(emitter, dVar8, dVar8, dVar8);
                    }
                }
            }
        }
        cVar5 = 0;
        if (i_this->mHeadSph.ChkTgHit()) {
            atInfo->mpObj = gabi::ea(i_this->mHeadSph.GetTgHitObj());
            cVar5 = 1;
        }
        for (int i = 0; i < 2; i++) {
            if (i_this->mBodySph[i].ChkTgHit()) {
                atInfo->mpObj = gabi::ea(i_this->mBodySph[i].GetTgHitObj());
                cVar5 = 2;
            }
        }
        for (int i = 0; i < 30; i++) {
            if (i_this->mLegSph[i].ChkTgHit()) {
                atInfo->mpObj = gabi::ea(i_this->mLegSph[i].GetTgHitObj());
                cVar5 = 3;
            }
        }
        if (cVar5 != 0 && i_this->mHitSeTimer == 0) {
            i_this->mHitSeTimer = 10;
            def_se_set(actor, atInfo->mpObj, 0x44);
        }
    }
}

/* ki_check (inlined) */
/* inlined into daBgn2_Execute */
static inline int ki_check(bgn2_class* i_this) {
    ki_all_count = 0;
    fpcM_Search(gabi::guest_fn((const void*)&ki_c_sub), i_this);
    return ki_all_count;
}

/* static tables (.data) */

static inline void sph_OnAtSetBit(dCcD_Sph* s) { s->mObjAt.mSPrm |= 1u; }
static inline void sph_OffAtSetBit(dCcD_Sph* s) { s->mObjAt.mSPrm &= ~1u; }

/* 0208B6A4 */
BOOL daBgn2_Execute(bgn2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 fVar2 = 0.0f;
    gabi::Local<cXyz> local_a0;
    gabi::Local<cXyz> local_ac;
    gabi::Local<cXyz> local_b8;

    dComIfGp_get(); /* HD: an unused play-object lookup */
    bgn = (bgn2BgnView*)fpcM_Search(gabi::guest_fn((const void*)&bgn_s_sub), i_this);
    if (bgn == nullptr) {
        return TRUE;
    }
    bgn3 = (bgn2Bgn3View*)fpcM_Search(gabi::guest_fn((const void*)&bgn3_s_sub), i_this);
    if (bgn3 == nullptr) {
        return TRUE;
    }
    if (l_HIO.mDebugHealth != 0) {
        actor->health = (s8)l_HIO.mDebugHealth;
    }
    if (bgn.get()->mForm != 1) {
        i_this->mAction = 10;
        actor->current.pos.set(0.0f, 2000.0f, 0.0f);
        /* fopAcM_OffStatus(actor, 0) */
        actor->attention_info.flags = 0;
        i_this->mKiSetTimer = l_HIO.mKiSetInterval;
        return TRUE;
    }
    if (i_this->mAction == 10) {
        i_this->mAction = 0;
        i_this->mMode = 0;
        actor->health = 3;
    }
    actor->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
    cLib_addCalc2(&i_this->mFogFade, 0.0f, 1.0f, 0.01f);
    i_this->mFrameCounter = (s16)(i_this->mFrameCounter + 1);
    actor->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 4;
    for (int i = 0; i < 5; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] = (s16)(i_this->mTimers[i] - 1);
        }
    }
    if (i_this->mDamageTimer != 0) {
        i_this->mDamageTimer = (s16)(i_this->mDamageTimer - 1);
    }
    if (i_this->mHitSeTimer != 0) {
        i_this->mHitSeTimer = (s16)(i_this->mHitSeTimer - 1);
    }
    if (i_this->mKiSetTimer != 0) {
        i_this->mKiSetTimer = (s16)(i_this->mKiSetTimer - 1);
    }
    if (l_HIO.mNoMove == 0) {
        move(i_this);
    }
    i_this->mpHeadMorf->play(&actor->current.pos, 0, 0);
    i_this->mpBodyMorf->play(&actor->current.pos, 0, 0);
    mDoExt_baseAnm_play(i_this->mJyakutenCBrkAnm);
    mDoExt_baseAnm_play(i_this->mJyakutenBBrkAnm);
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), actor->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    J3DModel* bodyModel = i_this->mpBodyMorf->getModel();
    J3DModel_setBaseTRMtx(bodyModel, mDoMtx_stack_c::get());
    local_a0->x = 0.0f;
    local_a0->y = 0.0f;
    local_a0->z = 0.0f;
    i_this->mpBodyMorf->calc();
    {
        Mtx34* headMtx = model_getAnmMtx(bodyModel, BGN_KUMO1_JNT_ATAMA_e);
        PSMTXCopy(headMtx, calc_mtx());
    }
    MtxTrans(REG0_F(0), REG0_F(1), REG0_F(2) + 100.0f, true);
    if (i_this->mShakeTimer != 0) {
        i_this->mShakeTimer = (s16)(i_this->mShakeTimer - 1);
    }
    f32 dVar18 = (f32)(s16)i_this->mShakeTimer * (REG0_F(14) + 500.0f);
    s16 y = (s16)gabi::ftoi(dVar18 * cM_ssin(i_this->mFrameCounter * 0x2100));
    s16 x = (s16)gabi::ftoi(dVar18 * cM_scos(i_this->mFrameCounter * 0x2300));
    cMtx_YrotM(calc_mtx(), y);
    cMtx_XrotM(calc_mtx(), x);
    MtxScale(REG0_F(7) + 2.0f, REG0_F(7) + 2.0f, REG0_F(7) + 2.0f, true);
    J3DModel* headModel = i_this->mpHeadMorf->getModel();
    J3DModel_setBaseTRMtx(headModel, calc_mtx());
    MtxPosition(local_a0, &i_this->mHeadPos);
    i_this->mpHeadMorf->calc();
    {
        Mtx34* m = model_getAnmMtx(bodyModel, BGN_KUMO1_JNT_JYAKUTEN_e);
        PSMTXCopy(m, calc_mtx());
    }
    MtxTrans(REG0_F(3) + 150.0f, REG0_F(4) + 150.0f, REG0_F(5), true);
    cMtx_YrotM(calc_mtx(), 0x4000);
    MtxScale(REG0_F(6) + 2.0f, REG0_F(6) + 2.0f, REG0_F(6) + 2.0f, true);
    for (int i = 0; i < 3; i++) {
        J3DModel* model = i_this->mpJyakutenModel[i];
        J3DModel_setBaseTRMtx(model, calc_mtx());
    }
    MtxPosition(local_a0, &i_this->mCorePos);
    i_this->mHeadSph.SetC(&i_this->mHeadPos);
    i_this->mHeadSph.SetR(REG0_F(0) + 150.0f);
    dComIfG_Ccsp_Set(&i_this->mHeadSph);
    actor->eyePos.copy(i_this->mHeadPos);
    actor->attention_info.position.x = actor->eyePos.x;
    actor->attention_info.position.y = actor->eyePos.y + 100.0f;
    actor->attention_info.position.z = actor->eyePos.z;
    for (int i = 0; i < 2; i++) {
        {
            Mtx34* m = model_getAnmMtx(bodyModel, body_d[i]);
            PSMTXCopy(m, calc_mtx());
        }
        if (i == 1) {
            local_a0->x = 400.0f;
        }
        MtxPosition(local_a0, &i_this->mBodyPos[i]);
        i_this->mBodySph[i].SetC(&i_this->mBodyPos[i]);
        i_this->mBodySph[i].SetR(REG0_F(i + 1) + body_scale[i]);
        if (i_this->mBodyAtOn != 0) {
            sph_OnAtSetBit(&i_this->mBodySph[i]);
        } else {
            sph_OffAtSetBit(&i_this->mBodySph[i]);
        }
        dComIfG_Ccsp_Set(&i_this->mBodySph[i]);
    }
    i_this->mBodyAtOn = 0;
    local_a0->x = 0.0f;

    if (i_this->mCoreHitFrame != 0) {
        i_this->mCoreHitFrame = (s16)(i_this->mCoreHitFrame + 1);
        if (i_this->mCoreHitFrame > 100) {
            i_this->mCoreHitFrame = 0;
        }
    }
    if (i_this->mArrowHitFlashTimer != 0) {
        i_this->mArrowHitFlashTimer = (s16)(i_this->mArrowHitFlashTimer - 1);
    }

    for (int i = 0; i < 32; i++) {
        if (i < 30) {
            {
                Mtx34* m = model_getAnmMtx(bodyModel, i + BGN_KUMO1_JNT_J_BGN2_ASHI_LB1_e);
                PSMTXCopy(m, calc_mtx());
            }
            MtxPosition(local_a0, &i_this->mLegPos[i]);
            i_this->mLegSph[i].SetC(&i_this->mLegPos[i]);
            fVar2 = (REG0_F(2) + 1.6f) * asi_scale[i % 5];
            i_this->mLegSph[i].SetR(fVar2);
            if (i_this->mLegAtOn != 0) {
                sph_OnAtSetBit(&i_this->mLegSph[i]);
            } else {
                sph_OffAtSetBit(&i_this->mLegSph[i]);
            }
            dComIfG_Ccsp_Set(&i_this->mLegSph[i]);
        }
        if (i_this->mCoreHitFrame == fl_check_d[i]) {
            i_this->mArrowHitEffectTimer[i] = 100;
        }
        if (i_this->mArrowHitEffectTimer[i] != 0) {
            i_this->mArrowHitEffectTimer[i] = (s16)(i_this->mArrowHitEffectTimer[i] - 1);
            if (i < 30) {
                local_ac->copy(i_this->mLegPos[i]);
            } else if (i == 30) {
                local_ac->copy(i_this->mBodyPos[0]);
                fVar2 = (REG8_F(3) + 400.0f);
            } else if (i == 31) {
                local_ac->copy(i_this->mBodyPos[1]);
                fVar2 = (REG8_F(4) + 400.0f);
            }
            f32 f29 = (REG8_F(0) + 0.0003f) * (fVar2 * (f32)(s16)i_this->mArrowHitEffectTimer[i]);
            if (i_this->mpArrowHitEmitter1[i] == nullptr) {
                i_this->mpArrowHitEmitter1[i] =
                    dComIfGp_particle_set(ID_AK_JN_CCTHUNDER00, local_ac);
            } else {
                JPABaseEmitter_setGlobalTranslation(i_this->mpArrowHitEmitter1[i], local_ac->x,
                                                    local_ac->y, local_ac->z);
                JPABaseEmitter_setGlobalScale(i_this->mpArrowHitEmitter1[i], f29, f29, f29);
            }
            if (i_this->mpArrowHitEmitter2[i] == nullptr) {
                i_this->mpArrowHitEmitter2[i] =
                    dComIfGp_particle_set(ID_AK_JN_CCTHUNDER01, local_ac);
            } else {
                JPABaseEmitter_setGlobalTranslation(i_this->mpArrowHitEmitter2[i], local_ac->x,
                                                    local_ac->y, local_ac->z);
                JPABaseEmitter_setGlobalScale(i_this->mpArrowHitEmitter2[i], f29, f29, f29);
            }
        } else {
            if (i_this->mpArrowHitEmitter1[i] != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(i_this->mpArrowHitEmitter1[i]);
                i_this->mpArrowHitEmitter1[i] = nullptr;
            }
            if (i_this->mpArrowHitEmitter2[i] != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(i_this->mpArrowHitEmitter2[i]);
                i_this->mpArrowHitEmitter2[i] = nullptr;
            }
        }
    }

    i_this->mLegAtOn = 0;
    i_this->mCoreSph.SetC(&i_this->mCorePos);
    i_this->mCoreSph.SetR(REG0_F(9) + 210.0f);
    dComIfG_Ccsp_Set(&i_this->mCoreSph);
    damage_check(i_this);
    if (i_this->mKiSetCount != 0) {
        i_this->mKiSetCount = (s8)(i_this->mKiSetCount - 1);
        if (ki_check(i_this) < l_HIO.mKiMax) {
            local_b8->x = cM_rndFX(2500.0f);
            local_b8->y = cM_rndF(500.0f) + 3500.0f;
            local_b8->z = cM_rndFX(2500.0f);
            fopAcM_create(fpcNm_KI_e, 0xFFFF0003, local_b8, fopAcM_GetRoomNo(actor), nullptr,
                          nullptr, -1, 0);
        }
    }
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    return TRUE;
}

/* WWHD glue
 */
