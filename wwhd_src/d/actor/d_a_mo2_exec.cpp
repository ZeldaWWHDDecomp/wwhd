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
        i_this->m05AE = l_mo2HIO().m08A;
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
static inline void ground_smoke_set(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m05F0 == 0) {
        return;
    }
    i_this->m05F0--;
    if (i_this->m05F0 >= l_mo2HIO().m024) {
        gabi::Local<cXyz> sp8;
        sp8->x = 0.0f;
        sp8->y = 0.0f;
        i_this->m05E8.x = 0;
        i_this->m05E8.z = 0;
        MtxTrans(actor->current.pos.x, actor->current.pos.y + 7.5f, actor->current.pos.z, 0);
        if (i_this->m05F2 == 0) {
            sp8->z = -350.0f;
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 6.0f);
            i_this->m05EE = i_this->m05EE + REG0_S(7) + 2000;
        } else if (i_this->m05F2 == 1) {
            cMtx_YrotM(calc_mtx(), actor->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            sp8->z = -55.0f;
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 6.0f);
            i_this->m05EE += 0x1FA0;
        } else if (i_this->m05F2 == 2) {
            MtxTrans(i_this->m2904.x, i_this->m2904.y + 7.5f, i_this->m2904.z, 0);
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            sp8->z = -12.5f;
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 6.0f);
            i_this->m05EE += 0x2000;
        } else if (i_this->m05F2 == 3) {
            cMtx_YrotM(calc_mtx(), actor->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            sp8->z = -37.5f;
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 2.0f);
            i_this->m05EE += 0x1FA0;
        } else if (i_this->m05F2 == 4) {
            cXyz* src = (i_this->m059C & 1) ? &i_this->mDamageReaction.m100[14]
                                            : &i_this->mDamageReaction.m100[15];
            f32 sy = src->y;
            i_this->m05DC.y = sy;
            f32 sz = src->z;
            f32 sx = src->x;
            i_this->m05DC.x = sx;
            i_this->m05DC.z = sz;
            if (i_this->mDamageReaction.m712 != 0) {
                i_this->m05DC.y = 512.5f;
            } else {
                i_this->m05DC.y = sy - 12.5f;
            }
            i_this->m05E8.y = cM_atan2s(actor->speed.x, actor->speed.z);
            smoke_set_s(i_this, 1.0f);
        }
    } else {
        i_this->m05DC.y = i_this->mDamageReaction.mSpawnY + 25000.0f;
    }
    if (i_this->m05F0 == 0) {
        Mo2Exec_endSmoke(&i_this->m05F4);
        i_this->m05F3 = 0;
    }
}

/* inlined into daMo2_Execute */
static inline void mo2_eye_tex_anm(mo2_class* i_this) {
    if (i_this->m02E0 != 0) {
        i_this->m02E0--;
    } else {
        i_this->m02E0 = (s16)gabi::ftoi(cM_rndF(50.0f) + 10.0f);
        if (i_this->m02DD == 0) {
            tex_anm_set(i_this, 3);
        }
    }
    if (i_this->m02DD != 0) {
        if (i_this->m02DC < i_this->m02DE) {
            i_this->m02DC++;
        } else {
            i_this->m02DD = 0;
        }
    }
}

/* inlined into daMo2_Execute */
static inline u8 damage_check(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<Mo2AtInfo> atInfo;
    atInfo->mpObj = 0;
    atInfo->mDamage = 0;
    atInfo->mPlCutBit = 0;
    u8 iVar12 = 0;
    u8 iVar11 = 0;
    i_this->mDamageReaction.mStts.Move();
    dComIfG_Ccsp_Set(&i_this->mDefenseSph);
    i_this->mDefenseSph.SetC(&i_this->m2928);
    if (i_this->mDefenseSph.ChkTgHit()) {
        void* obj = i_this->mDefenseSph.GetTgHitObj();
        def_se_set(actor, obj, 0x41);
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        gabi::Local<cXyz> local_48;
        local_48->x = 0.0f;
        local_48->y = 0.0f;
        local_48->z = -10.0f;
        MtxPosition(local_48, &i_this->mDamageReaction.m42C);
        i_this->mDamageReaction.m4D4 = -15.0f;
        gabi::Local<csXyz> local_68;
        local_68->x = player->shape_angle.x;
        s16 py = player->shape_angle.y;
        local_68->y = py;
        s16 pz = player->shape_angle.z;
        local_68->y = (s16)(py - 0x8000);
        local_68->z = pz;
        GXColor* k0 = &actor->tevStr.mColorK0;
        Mo2ExecEmitter* emitter = reinterpret_cast<Mo2ExecEmitter*>(
            dComIfGp_particle_set(ID_AK_JN_ELEMENTKIKUZU00, &i_this->m2928, local_68, nullptr, 0xFF,
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
    i_this->m2928.y = -10000.0f;
    i_this->mDefenseSph.SetR(-200.0f);
    if (i_this->m05B4 == 0 && (i_this->mHeadSph.ChkTgHit() || i_this->mTgCyl.ChkTgHit())) {
        i_this->m05A0 = 0;
        i_this->m2944 = -1;
        i_this->m294E = 0;
        i_this->m05B4 = 4; // HD: four frames instead of the GameCube register plus five.
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
                    path_check(i_this);
                    wait_set(i_this);
                    i_this->mDamageReaction.mMode = -10;
                } else {
                    i_this->mEnemyIce.mLightShrinkTimer = 1;
                }
                Mo2Exec_enemyFireRemove(&i_this->mEnemyFire);
                i_this->m05F0 = 0;
                Mo2Exec_endSmoke(&i_this->m05F4);
                if (i_this->mbHasInnateWeapon != 0) {
                    i_this->mSpawnWeaponActor = 2;
                }
                i_this->m2A09 = 1;
                return 0;
            }
            if (obj->ChkAtType(AT_TYPE_HD_FIRE)) {
                i_this->m05B4 = 0x32;
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
            if (atInfo->mResultingAttackType == 0xe && i_this->m2951 == 0) {
                i_this->m2951 = 1;
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
        i_this->m05B6 = 0x19;
        pos->z = pz;
        dKy_Sound_set(pos, 100, fopAcM_GetID(actor), 5);
        // HD: health debug reset removed; type twelve sets ten invincible frames.
        if (atInfo->mResultingAttackType == 0xC) {
            i_this->m05B4 = 0xA;
        }
        i_this->m2960 |= atInfo->mPlCutBit;
        if (atInfo->mResultingAttackType == 10) {
            i_this->m2A0B = REG_S(13, 3) + 8;
            atInfo->mDamage = 1;
            iVar11 = atInfo->mbDead != 0 ? 7 : 4;
            cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
        } else {
            i_this->m2A0B = 0;
            if (atInfo->mResultingAttackType == 1) {
                s16 sVar3 = i_this->m05D6 - actor->current.angle.y;
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
                cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
            } else {
                iVar11 = atInfo->mbDead != 0 ? 7 : 4;
                cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
            }
        }
    }
    if (iVar11 != 0 && i_this->mMode == 1) {
        if (rouya_mode() != 0) {
            i_this->mDamageReaction.mAction = ACTION_NAGE;
            i_this->mDamageReaction.mMode = -10;
            iVar11 = 5;
            i_this->m05B4 = 0x32;
        } else {
            i_this->mMode = 0;
            i_this->m2A09 = 1;
        }
    }
    switch (iVar11) {
    case 1:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x10;
        i_this->mDamageReaction.m428 = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        break;
    case 2:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x40;
        cMtx_YrotS(calc_mtx(), (s16)(actor->current.angle.y + 0x8000));
        i_this->mDamageReaction.m428 = 26.0f;
        break;
    case 3:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x40;
        i_this->mDamageReaction.m428 = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        break;
    case 4:
        tex_anm_set(i_this, 1);
        i_this->mDamageReaction.m428 = 23.0f;
        i_this->mDamageReaction.m424 |= 0x10;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        break;
    case 5:
        tex_anm_set(i_this, 1);
        i_this->mDamageReaction.m424 |= 0x20;
        i_this->mDamageReaction.m428 = 23.0f;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f,
                 dRes_INDEX_MO2_BAS_PAOMUKE_e);
        i_this->mDamageReaction.m70E = 7;
        i_this->mDamageReaction.m48A = 10;
        i_this->mDamageReaction.m474 = 5000.0f;
        if (i_this->m05DA == 0) {
            i_this->m05DA = 3;
            i_this->m05E8.x = 0;
        }
        break;
    case 6:
        tex_anm_set(i_this, 1);
        i_this->m05B4 = 200;
        i_this->mDamageReaction.mAction = ACTION_HIP_DAMAGE;
        i_this->mDamageReaction.mMode = 0;
        if (i_this->m05DA == 0) {
            i_this->m05DA = 10;
            i_this->m05E8.x = -0x4000;
        }
        break;
    case 7:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x10;
        i_this->mDamageReaction.m428 = 26.0f;
        break;
    }
    if (i_this->mDamageReaction.m424 != 0) {
        gabi::Local<cXyz> local_54;
        local_54->x = 0.0f;
        local_54->y = 0.0f;
        local_54->z = -10.0f;
        MtxPosition(local_54, &i_this->mDamageReaction.m42C);
        if (i_this->mDamageReaction.m428 < 25.0f) {
            i_this->mDamageReaction.m4D4 = -l_mo2HIO().m13C;
        } else {
            i_this->mDamageReaction.m428 = cM_rndF(10.0f) + 70.0f;
        }
    }
    if (iVar11 != 0) {
        if (actor->health <= 0 && atInfo->mbDead != 0) {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_FAINTED, 0);
            if (fopAcM_CheckStatus(actor, fopAcStts_BOSS_e)) {
                i_this->m2A1D = 0x32;
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
static inline void mo2_demo_camera(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    dCam_getCamera();
    u8* camera = dCam_getBody();
    bool r28 = true;
    switch ((u8)i_this->m2A1D) {
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
        i_this->m2A44 = 55.0f;
        i_this->m2A2C.copy(actor->current.pos);
        i_this->m2A1D = 3;
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
        i_this->m2A1D = 0;
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
        i_this->m2A1D = 0x33;
        {
            Mo2ExecCamera* camera2 = reinterpret_cast<Mo2ExecCamera*>(dComIfGp_getCamera(0));
            i_this->m2A20.copy(camera2->mLookat.mEye);    /* view.mLookat.mEye */
            i_this->m2A2C.copy(camera2->mLookat.mCenter); /* view.mLookat.mCenter */
        }
        i_this->m2A44 = 55.0f;
        i_this->m2A1E = 0;
        /* fallthrough */
    case 0x33:
        cLib_addCalc2(&i_this->m2A44, REG0_F(0xd) + 30.0f, 0.2f, REG0_F(0xe) + 0.4f);
        if (i_this->m2A1C == 0) {
            cLib_addCalc2(&i_this->m2A2C.x, actor->current.pos.x, 0.1f, 100.0f);
            cLib_addCalc2(&i_this->m2A2C.y, actor->current.pos.y + 230.0f + REG0_F(0xc), 0.1f,
                          100.0f);
            cLib_addCalc2(&i_this->m2A2C.z, actor->current.pos.z, 0.1f, 100.0f);
        }
        i_this->m2A1E++;
        if (i_this->m2A1E > 0x96) {
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
        *center = i_this->m2A2C.get();
        *eye = i_this->m2A20.get();
        dCamera_Set(camera, center, eye, i_this->m2A44, 0);
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
    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&actor->eyePos, 0, 0);
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    enemy_fire(&i_this->mEnemyFire);
    if (i_this->m2A4B != 0 || i_this->m2A4C == 1) {
        dComIfGp_setNextStage("majroom", 0, 0, -1, 0.0f, 0, 1, 0);
        return TRUE;
    }
    if (i_this->m2A4C != 0) {
        i_this->m2A4C--;
        if (i_this->m2A4C == 0x1E) {
            mDoAud_seStop(JA_SE_MAJUTOU_ALERM, 30);
        }
    }
    /* HD: no m2970 / fell-4000-below-home deletion here */
    if (i_this->m2A4A != 0) {
        if (actor->eventInfo.checkCommandDemoAccrpt()) {
            i_this->m2A4A = 0;
        } else {
            fopAcM_orderPotentialEvent(actor, dEvtType_OTHER_e, 0xFFFF, 0);
            actor->eventInfo.onCondition(dEvtCnd_UNK2_e);
        }
    }
    if (i_this->m2A48 != 0) {
        i_this->m2A48--;
        if (i_this->m2A48 == 0) {
            if (camera_mode() == 0) {
                i_this->m2A1D = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_E3_DEMO;
            i_this->mDamageReaction.mMode = 0;
        }
    }
    if (i_this->mDamageReaction.mAction == ACTION_JYUNKAI &&
        (reinterpret_cast<Mo2ExecPlayer*>(player)->mGrabWear < 0.0f)) {
        if (i_this->mDamageReaction.mMode < 2) {
            l_mo2HIO().m02C = REG0_F(10) + 800.0f;
        } else {
            l_mo2HIO().m02C = REG0_F(0xb) + 1200.0f;
        }
        l_mo2HIO().m038 = 0x1932;
        l_mo2HIO().m040.x = 340.0f;
        l_mo2HIO().m040.z = 340.0f;
        l_mo2HIO().m04C = -200.0f;
    }
    u8 r23 = 0;
    if (l_mo2HIO().m006 == 0 || CPad_CHECK_TRIG_B(0) || CPad_CHECK_HOLD_Y(0)) {
        i_this->m059C++;
        for (s32 i = 0; i < 5; i++) {
            if (i_this->m05A4[i] != 0) {
                i_this->m05A4[i]--;
            }
        }
        if (i_this->m05BA != 0) {
            i_this->m05BA--;
        }
        if (i_this->m05BC != 0) {
            i_this->m05BC--;
        }
        if (i_this->m05AE != 0) {
            i_this->m05AE--;
        }
        if (i_this->m02E2 != 0) {
            i_this->m02E2--;
        }
        if (i_this->m05B0 != 0) {
            i_this->m05B0--;
            i_this->m05F0 = l_mo2HIO().m024 + 4;
            i_this->m05F2 = 4;
        }
        if (i_this->mDamageReaction.m48A != 0) {
            i_this->mDamageReaction.m48A--;
        }
        if (i_this->m05B4 != 0) {
            i_this->m05B4--;
        }
        if (i_this->m05B6 != 0) {
            i_this->m05B6--;
        }
        if (dComIfGs_isCollect(0, 0) || dComIfGs_isCollect(0, 1) || dComIfGs_isCollect(0, 2) ||
            dComIfGs_isCollect(0, 3)) {
            rouya_mode() = 0;
        } else {
            rouya_mode() = 1;
        }
        if (i_this->m02C1 != 0) {
            if (dComIfGs_isSwitch(i_this->m02C1 - 1, fopAcM_GetRoomNo(actor))) {
                i_this->m02C1 = 0;
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
        i_this->m0594 = 0;
        i_this->m0598 = 0;
        mo2_eye_tex_anm(i_this);
        i_this->mDamageReaction.m438 = 0;
        if (i_this->m05B2 == 0) {
            i_this->mpMorf->play(&actor->eyePos, 0, 0);
        }
        if (i_this->mDamageReaction.mAction < ACTION_CARRY) {
            i_this->mpMorf->calc();
        }
        r23 = damage_check(i_this);
        Mo2_move(i_this);
        mo2_demo_camera(i_this);
        ground_smoke_set(i_this);
        if (i_this->m05B2 != 0) {
            i_this->m05B2--;
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
        i_this->m05F0 = 1;
        i_this->m2954 = fpcM_ERROR_PROCESS_ID_e;
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
                 i_this->mDamageReaction.mAction != ACTION_P_LOST && i_this->m05A4[1] == 0)) {
                wait_set(i_this);
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
                i_this->m05A4[1] = 0x1e;
            }
            i_this->m05DA = 5;
            i_this->m05E8.x = -0x4000;
            break;
        case 10:
            wait_set(i_this);
            i_this->mDamageReaction.mMode = 2;
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            break;
        case 0x14:
            i_this->m05DA = 1;
            i_this->m05E8.x = -0x4000;
            i_this->m05F2 = 1;
            i_this->m05F0 = l_mo2HIO().m024 + 0x10;
            i_this->mDamageReaction.m4C8[2] = l_mo2HIO().m088;
            break;
        case 0x15:
            i_this->m05DA = 1;
            i_this->m05E8.x = 0;
            i_this->mDamageReaction.m4C8[2] = l_mo2HIO().m088;
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
    f32 sc = l_mo2HIO().m010;
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
    f32 d0x = i_this->m28C8.x, d0y = i_this->m28C8.y, d0z = i_this->m28C8.z;
    f32 dcx = actor->current.pos.x, dcy = actor->current.pos.y, dcz = actor->current.pos.z;
    if (i_this->m05B4 != 0) {
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
    if (i_this->m05DA != 0) {
        i_this->m05DA--;
        if (i_this->m05DA == 0) {
            i_this->m05E8.y = actor->current.angle.y;
            dComIfGp_particle_set(ID_AK_JN_TUBA00, &i_this->m28C8, &i_this->m05E8);
        }
    }
    if (i_this->mDamageReaction.mAcch.ChkGroundHit()) {
        gabi::Local<Mo2ExecGndChk> gndChk;
        dBgS_GndChk_ct(gndChk);
        s16 r22 = 0x7FFF;
        s16 r21 = 0x7FFF;
        f32 f31 = i_this->mDamageReaction.m480 != 0 ? 100.0f : 10.0f;
        f32 px = actor->current.pos.x;
        f32 py = actor->current.pos.y + (50.0f - i_this->mDamageReaction.m44C.y);
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
        if (i_this->mDamageReaction.m480 != 0) {
            maxSpeed = 0x400;
            speedRatio = 1;
        } else {
            maxSpeed = 0x100;
            speedRatio = 8;
        }
        if (r22 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->mDamageReaction.m48C.x, r22, speedRatio, maxSpeed);
        }
        if (r21 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->mDamageReaction.m48C.z, r21, speedRatio, maxSpeed);
        }
        dBgS_GndChk_dt(gndChk);
    }
    if (i_this->m2944 >= 0) {
        i_this->m2944++;
        if (i_this->m2944 == i_this->m2946) {
            i_this->m294E = i_this->m2948;
        }
        if (i_this->m2944 == i_this->m294A) {
            i_this->m294E = i_this->m294C;
        }
        if (i_this->m2944 > 200) {
            i_this->m2944 = -1;
        }
    }
    /* setBtAttackData(0, 10, 10000, type); setBtMaxDis(m140) */
    i_this->mBtStartFrame = 0.0f;
    i_this->mBtEndFrame = 10.0f;
    i_this->mBtMaxDis = 10000.0f;
    i_this->mBtAttackType = i_this->mParryOpeningType;
    i_this->mBtMaxDis = l_mo2HIO().m140;
    if (i_this->m294E != 0) {
        i_this->m294E--;
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
