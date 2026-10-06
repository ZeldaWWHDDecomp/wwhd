/**
 * d_a_bmd_exec.cpp (WWHD)
 * Boss - Kalle Demos: daBmd_Execute (020AEBCC) with its inlined helpers (core_move,
 * core_damage_check, bmd_kankyo, eff_cont, mk_move).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bmd.h"
#include <cmath>

/* CcAtInfo (0x1C) */
struct CcAtInfo_bl {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_bl, 0x1C);
static inline fopAc_ac_c* cc_at_check(fopAc_ac_c* a, CcAtInfo_bl* info) { return gabi::call<fopAc_ac_c*>(0x025192A8, a, info); }

/* dComIfGp_particle_setToon (HD: dPa_control_c::set group 2); the room is read before the play object */
static inline JPABaseEmitter* particle_setToon(u16 id, cXyz* pos, u8 alpha, dPa_smokeEcallBack_l* cb, s8 room) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, nullptr, nullptr, alpha, (dPa_levelEcallBack*)cb, room, nullptr, nullptr, nullptr);
}
/* emitter->setGlobalRTMatrix(mtx): JPASetRMtxTVecfromMtx(mtx, mGlobalRot (+0x1F0), mGlobalTrs (+0x22C)) */
static inline void emitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}
/* fopAcM_seStartCurrent (HD inline: null checks on the actor and its position) */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_monsSeStart, with the inline's actor check folded */
static inline void monsSeStart_e(fopAc_ac_c* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, 0, reverb);
    }
}
/* the morf pointer is read after the resource lookup */
static inline void morf_setAnm(gptr<mDoExt_McaMorf>& mp, const char* arc, s32 idx, s32 mode, f32 morf) {
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, idx, SAFESTRING_VTBL);
    mDoExt_McaMorf* m = mp;
    m->setAnm(anm, mode, morf, 1.0f, 0.0f, -1.0f, nullptr);
}
/* model->setBaseTRMtx(mDoMtx_stack_c::get()) */
static inline void setBaseTRMtx_now(J3DModel* model) {
    mtx_copy(gabi::at<Mtx34>(gabi::ea(model) + 0xC8), mDoMtx_stack_c::get());
}
static inline s32 frame_int(mDoExt_McaMorf* m) { return gabi::ftoi(m->getFrame()); }
static inline void dCamera_SetTypeForce(u32 cam, const char* type) { gabi::call(0x02514EE4, cam, type, 0); }

#define ARC_CORE STR(0x10009A90) /* "Bmd" */
#define ARC_MK STR(0x10009A94)   /* "Bmd" */

static inline void core_damage_check(bmd_class* i_this, fopAc_ac_c* player) {
    fopAc_ac_c* actor = i_this;
    s8 bVar2 = false;
    /* HD: no damage while the flower is open */
    if (i_this->m304 == 0) {
        i_this->m310 = 4;
        return;
    }
    if (i_this->m310 != 0) {
        return;
    }
    gabi::Local<cXyz> local_38;
    if (i_this->mCoreSph.ChkTgHit() != 0) {
        i_this->m310 = 8;
        fopAcM_monsSeStart(actor, 0x4855 /* JA_SE_CV_BKM_DAMAGE */, 0);
        i_this->m306 = 1;
        gabi::Local<CcAtInfo_bl> atInfo;
        atInfo->pParticlePos = 0;
        atInfo->mpObj = gabi::ea(i_this->mCoreSph.GetTgHitObj());
        atInfo->pParticlePos = gabi::ea(i_this->mCoreSph.GetTgHitPosP());
        fopAc_ac_c* pfVar4 = cc_at_check(actor, atInfo);
        if (pfVar4 != nullptr) {
            f32 x = actor->eyePos.x - pfVar4->current.pos.x;
            local_38->x = x;
            f32 z = actor->eyePos.z - pfVar4->current.pos.z;
            local_38->z = z;
            i_this->m90C[0].y = cM_atan2s(-x, -z);
        }
        i_this->mA88[1] = 10;
        if (actor->health <= 0) {
            bVar2 = true;
        }
    }
    {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&player->current.pos, tmp, &actor->current.pos);
        gabi::store<u32>(gabi::ea(local_38.get()), gabi::load<u32>(gabi::ea(tmp.get())));
        gabi::store<u32>(gabi::ea(local_38.get()) + 8, gabi::load<u32>(gabi::ea(tmp.get()) + 8));
        local_38->y = 0.0f;
    }
    /* daPy_py_c::checkForestWaterUse(): a player flag */
    if (!((gabi::load<u32>(gabi::ea(player) + 0x3BC) & 0x20000) && i_this->m304 != 0 &&
          std_sqrtf(PSVECSquareMag(local_38)) < 300.0f) &&
        !bVar2) {
        return;
    }
    if (!bVar2) {
        fopAcM_seStartCurrent(actor, 0x2828 /* JA_SE_LK_LAST_HIT */, 0);
    }
    gabi::store<f32>(dComIfGp_ea() + 0x5B44, 0.0f); /* HD */
    i_this->mB72 = 1;                              /* HD */
    i_this->m306 = 100;
    i_this->mMode = 0xB;
    i_this->m302 = 0;
    i_this->mB74 = 100;
    gabi::store<u8>(0x101EACB7, 8); /* dScnPly_ply_c::setPauseTimer(8) */
    i_this->m310 = 20000;
    i_this->mBE0 = 2;
    i_this->mBDC = 1.0f;
    mDoAud_bgmStop(30);
}

static inline void core_move(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_40;
    gabi::Local<cXyz> local_4c;
    bool bVar5 = false;
    dComIfGp_get(); /* HD: an unused player fetch */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    core_damage_check(i_this, player);
    switch (i_this->m306) {
    case 0:
        break;
    case 1:
        morf_setAnm(i_this->mpHeadMorf, ARC_CORE, 0xB /* COA_DAMAGE */, 0, 1.0f);
        i_this->m918 = 0;
        i_this->m306 = 2;
        i_this->m91C = REG0_F(9) + 6000.0f;
        i_this->m920 = 9000.0f;
        break;
    case 2:
        if (std::fabs((f32)i_this->m91C) < 200.0f) {
            i_this->m306 = 0;
            morf_setAnm(i_this->mpHeadMorf, ARC_CORE, 0x14 /* COA_WAIT */, 2, 5.0f);
        }
        break;
    case 100:
        morf_setAnm(i_this->mpHeadDeadMorf, ARC_CORE, 0xC /* COA_DEAD1 */, 0, 5.0f);
        i_this->m306 = 0x65;
        i_this->m924.copy(actor->current.pos);
        cMtx_YrotS(calc_mtx(), (actor->shape_angle.y + REG0_S(4)) + 3000);
        local_40->y = 0.0f;
        local_40->x = 0.0f;
        local_40->z = REG0_F(15) + 5.5f; /* HD: 5.5 (GameCube 7.0) */
        MtxPosition(local_40, &i_this->m930);
        i_this->m93E = 0;
        i_this->m93C = 0;
        i_this->m930.y = REG0_F(11) + 50.0f;
        i_this->m308[2] = 0;
        monsSeStart_e(actor, 0x4856 /* JA_SE_CV_BKM_LAST_DAMAGE */);
        break;
    case 101:
        i_this->mA8C = 3;
        i_this->m93C = i_this->m93C + 0x800;
        i_this->m93E = i_this->m93E + 0x500;
        PSVECAdd(&i_this->m924, &i_this->m930, &i_this->m924); /* m924 += m930 */
        i_this->m930.y = i_this->m930.y - (REG0_F(12) + 1.0f);
        if (!(i_this->m924.y > (i_this->m328 - 10.0f) + REG0_F(13))) {
            i_this->m924.y = (i_this->m328 - 10.0f) + REG0_F(13);
            i_this->m306 = 0x66;
            mDoAud_bgmStreamPlay();
            morf_setAnm(i_this->mpHeadDeadMorf, ARC_CORE, 0xD /* COA_DEAD2 */, 0, 1.0f);
            i_this->m930.y = 0.0f;
            i_this->m930.z = 0.0f;
            i_this->m930.x = 0.0f;
            bVar5 = true;
            fopAcM_monsSeStart(actor, 0x4857 /* JA_SE_CV_BKM_FALLDOWN */, 0);
            fopAcM_seStart(actor, 0x5855 /* JA_SE_CM_BKM_END_CORE_DROP */, 0);
        }
        break;
    case 102:
        if (frame_int(i_this->mpHeadDeadMorf) == 14) {
            bVar5 = true;
            fopAcM_monsSeStart(actor, 0x4858 /* JA_SE_CV_BKM_JITABATA */, 0);
        }
        i_this->mA8C = 3;
        if (i_this->mpHeadDeadMorf->isStop()) {
            morf_setAnm(i_this->mpHeadDeadMorf, ARC_CORE, 0xF /* COA_NDEAD1 */, 0, 1.0f);
            i_this->m306 = 0x67;
            i_this->m930.z = 10.0f;
            i_this->m930.x = 10.0f;
            i_this->m308[1] = REG0_S(6) + 0xC3;
        }
        goto temp_884;
    case 103:
        if (frame_int(i_this->mpHeadDeadMorf) == 18) {
            bVar5 = true;
            fopAcM_monsSeStart(actor, 0x4858 /* JA_SE_CV_BKM_JITABATA */, 0);
        }
        i_this->mA8C = 3;
        if (!i_this->mpHeadDeadMorf->isStop()) {
            goto temp_884;
        }
        morf_setAnm(i_this->mpHeadDeadMorf, ARC_CORE, 0x10 /* COA_NDEAD2 */, 0, 1.0f);
        i_this->m306 = 0x68;
        goto temp_884;
    case 104:
        if (frame_int(i_this->mpHeadDeadMorf) == 17 || frame_int(i_this->mpHeadDeadMorf) == 37) {
            bVar5 = true;
            fopAcM_monsSeStart(actor, 0x4858 /* JA_SE_CV_BKM_JITABATA */, 0);
        }
        i_this->mA8C = 10;
        if (!i_this->mpHeadDeadMorf->isStop()) {
            goto temp_884;
        }
        morf_setAnm(i_this->mpHeadDeadMorf, ARC_CORE, 0xE /* COA_LDEAD */, 0, 1.0f);
        i_this->m930.z = 0.0f;
        i_this->m930.x = 0.0f;
        i_this->m306 = 0x69;
        goto temp_884;
    case 105:
        if (frame_int(i_this->mpHeadDeadMorf) == 37) {
            JPABaseEmitter* pJVar7 = dComIfGp_particle_set(0x80FE /* ID_AK_SN_BKMCORESIGH00 */, &actor->current.pos);
            if (pJVar7 != nullptr) {
                emitter_setGlobalRTMatrix(pJVar7, bl_getAnmMtx(i_this->mpHeadDeadMorf->getModel(), 5 /* UWAAGO */));
            }
            fopAcM_monsSeStart(actor, 0x4859 /* JA_SE_CV_BKM_DIE */, 0);
        }
        if (i_this->mpHeadDeadMorf->isStop()) {
            gabi::Local<cXyz> local_58;
            local_58->x = i_this->m924.x;
            local_58->z = i_this->m924.z;
            local_58->y = i_this->m924.y + (REG0_F(2) + 80.0f);
            /* HD: createDisappear(actor, pos, 8, 2, 0xFF) */
            fopAcM_createDisappear(actor, local_58, 8, 2, 0xFF);
            i_this->m306 = 0x6E;
            i_this->m308[2] = 0xAA;
        }
    temp_884:
        PSVECAdd(&i_this->m924, &i_this->m930, &i_this->m924); /* m924 += m930 */
        if ((i_this->m308[1] & 0xF) == 0) {
            i_this->m930.x = -i_this->m930.x;
            i_this->m930.z = -i_this->m930.z;
        }
        cLib_addCalcAngleS2(&i_this->m93C, 0, 2, 0x800);
        cLib_addCalcAngleS2(&i_this->m93E, 0, 2, 0x800);
        break;
    case 110:
        if (i_this->m308[2] == 1) {
            i_this->m306 = 0x6F;
            i_this->mB74 = 0x66; /* HD (GameCube: mB74++) */
            i_this->mB78 = 0;
        }
        break;
    case 111:
        break;
    }

    if (i_this->m306 < 100) {
        cMtx_YrotS(calc_mtx(), (s16)((i_this->m93C + 0x8000) - i_this->m90C[0].y));
        local_40->x = 0.0f;
        local_40->y = 0.0f;
        local_40->z = i_this->m91C * cM_ssin(i_this->m918);
        MtxPosition(local_40, local_4c);
        i_this->m90C[0].x = (s16)gabi::ftoi(local_4c->z);
        i_this->m90C[0].z = (s16)gabi::ftoi(local_4c->x);
        local_40->z = i_this->m920 * cM_ssin((i_this->m918 - 20000) + REG0_S(6));
        MtxPosition(local_40, local_4c);
        i_this->m90C[1].x = (s16)gabi::ftoi(local_4c->z);
        i_this->m90C[1].z = (s16)gabi::ftoi(local_4c->x);
        i_this->m918 = i_this->m918 + (REG0_S(5) + 6000);
        cLib_addCalc0(&i_this->m91C, 1.0f, REG0_F(10) + 250.0f);
        cLib_addCalc0(&i_this->m920, 1.0f, REG0_F(10) + 200.0f);
        J3DModel* pJVar11 = i_this->mpHeadMorf->getModel();
        mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
        mDoMtx_stack_c::YrotM(i_this->m93C);
        setBaseTRMtx_now(pJVar11);
        i_this->mpHeadMorf->play(&actor->eyePos, 0, 0);
        i_this->mpHeadMorf->calc();
        PSMTXCopy(bl_getAnmMtx(pJVar11, 6 /* BKM_COA_JNT_UWAAGO_e */), calc_mtx());
        local_40->x = 0.0f;
        local_40->y = -20.0f;
        local_40->z = 30.0f;
        MtxPosition(local_40, &actor->eyePos);
        cXyz* attn = gabi::at<cXyz>(gabi::ea(actor) + 0x390); /* attention_info.position */
        cXyz_fcopy(attn, &actor->eyePos);
        attn->y = actor->eyePos.y + 50.0f;
        if (i_this->m304 != 0 && i_this->m940 == 0 && i_this->mMode != 10) {
            gabi::store<u32>(gabi::ea(actor) + 0x39C, 4); /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        }
    } else {
        mDoMtx_stack_c::transS(i_this->m924.x, i_this->m924.y, i_this->m924.z);
        mDoMtx_stack_c::YrotM(i_this->m93C);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->m93E);
        J3DModel* pJVar11 = i_this->mpHeadDeadMorf->getModel();
        setBaseTRMtx_now(pJVar11);
        i_this->mpHeadDeadMorf->play(&actor->eyePos, 0, 0);
        i_this->mpHeadDeadMorf->calc();
        PSMTXCopy(bl_getAnmMtx(pJVar11, REG0_S(4) + 1 /* BKM_COA_DEADMODEL_JNT_KOUTOUBU_e */), calc_mtx());
        local_40->z = 0.0f;
        local_40->y = 0.0f;
        local_40->x = 0.0f;
        MtxPosition(local_40, &actor->eyePos);
        if (i_this->m306 >= 0x6F) {
            actor->eyePos.y = actor->eyePos.y + 20000.0f;
        }
    }
    i_this->mCoreSph.SetC(&actor->eyePos);
    i_this->mCoreSph.SetR(50.0f);
    dComIfG_Ccsp_Set(&i_this->mCoreSph);
    if (bVar5) {
        s8 room = fopAcM_GetRoomNo(actor);
        particle_setToon(0xA0FD /* ID_AK_ST_BKMCOREDEADSMOKE00 */, &i_this->m924, 0xB9, &i_this->mSmokeCb[6], room);
        fopAcM_seStart(actor, 0x5856 /* JA_SE_CM_BKM_END_CORE_LEAP */, 0);
    }
}

static inline void bmd_kankyo(bmd_class* i_this) {
    gabi::call(0x0255FE50, (f32)l_HIO().m0C); /* dKy_custom_timeset */
    switch (i_this->mBE0) {
    case 0:
        gabi::call(0x0255FDA4, 0, 6, (f32)i_this->mBDC); /* dKy_custom_colset */
        cLib_addCalc0(&i_this->mBDC, 1.0f, 0.025f);
        i_this->m2D8 = 11.0f * (1.0f - i_this->mBDC);
        break;
    case 1:
        gabi::call(0x0255FDA4, 0, 4, (f32)i_this->mBDC);
        cLib_addCalc0(&i_this->mBDC, 1.0f, 0.025f);
        i_this->m2D8 = 11.0f * (1.0f - i_this->mBDC);
        break;
    case 2:
        gabi::call(0x0255FDA4, 5, 6, (f32)i_this->mBDC);
        cLib_addCalc0(&i_this->mBDC, 1.0f, 0.02f);
        i_this->m2D8 = 11.0f * i_this->mBDC;
        break;
    }
}

static inline void eff_cont(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    for (s32 i = 0; i < 3; i++) {
        if (i_this->mA7C[i] != nullptr) {
            JPABaseEmitter* e = i_this->mA7C[i];
            u16 jnt = gabi::load<u16>(0x101919B4 + 2 * i); /* eff_joint */
            if (i >= 2) {
                emitter_setGlobalRTMatrix(e, bl_getAnmMtx(i_this->mpHeadDeadMorf->getModel(), jnt));
            } else {
                emitter_setGlobalRTMatrix(e, bl_getAnmMtx(i_this->mpHeadMorf->getModel(), jnt));
            }
            if (i_this->mA88[i] != 0) {
                i_this->mA88[i] = i_this->mA88[i] - 1;
                if (i_this->mA88[i] == 0) {
                    /* becomeInvalidEmitter(): stopCreateParticle(); mMaxFrame = -1 */
                    u32 em = gabi::ea(i_this->mA7C[i].get());
                    u32 flags = gabi::load<u32>(em + 0x254);
                    gabi::store<s32>(em + 0x5C, -1);
                    gabi::store<u32>(em + 0x254, flags | 1);
                    i_this->mA7C[i] = nullptr;
                }
            }
        } else if (i_this->mA88[i] != 0) {
            u16 name = gabi::load<u16>(0x101919AC + 2 * i); /* eff_name */
            i_this->mA7C[i] = dComIfGp_particle_set(name, &actor->current.pos);
        }
    }
}

static inline void mk_move(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m2DC != 0) {
        switch ((u32)(s32)i_this->m2DC) {
        case 0:
            i_this->m308[3] = 0x96;
            break;
        case 1:
            if (i_this->m308[3] == 0) {
                morf_setAnm(i_this->mpMakarMorf, ARC_MK, 8 /* CALL_01 */, 2, 5.0f);
                i_this->m308[3] = (s16)gabi::ftoi(cM_rndF(90.0f) + 60.0f);
                i_this->m2DC = 2;
                mk_voice_set(i_this, 0x48C5 /* JA_SE_CV_CB_HELP */);
            }
            break;
        case 2:
            if (i_this->m308[3] == 0) {
                morf_setAnm(i_this->mpMakarMorf, ARC_MK, 9 /* CALL_02 */, 2, 5.0f);
                i_this->m308[3] = (s16)gabi::ftoi(cM_rndF(90.0f) + 60.0f);
                i_this->m2DC = 1;
                mk_voice_set(i_this, 0x48C5 /* JA_SE_CV_CB_HELP */);
            }
            break;
        case 3:
            morf_setAnm(i_this->mpMakarMorf, ARC_MK, 8 /* CALL_01 */, 2, 5.0f);
            i_this->m2DC = 10;
            mk_voice_set(i_this, 0x48C5 /* JA_SE_CV_CB_HELP */);
            break;
        case 4:
            morf_setAnm(i_this->mpMakarMorf, ARC_MK, 0xA /* CALL_03 */, 2, 5.0f);
            i_this->m2DC = 10;
            mk_voice_set(i_this, 0x48C5 /* JA_SE_CV_CB_HELP */);
            break;
        case 5:
            morf_setAnm(i_this->mpMakarMorf, ARC_MK, 0x15 /* DROP */, 0, 1.0f);
            i_this->m2DC = 6;
            break;
        case 6:
            if (frame_int(i_this->mpMakarMorf) == 6) {
                gabi::Local<cXyz> local_28;
                local_28->x = i_this->m2E0.x;
                f32 y = i_this->m2E0.y;
                local_28->z = i_this->m2E0.z;
                local_28->y = y + 10.0f;
                s8 room = fopAcM_GetRoomNo(actor);
                JPABaseEmitter* emitter =
                    particle_setToon(0xA0FD /* ID_AK_ST_BKMCOREDEADSMOKE00 */, local_28, 0xB9, &i_this->mSmokeCb[6], room);
                if (emitter != nullptr) {
                    /* setGlobalScale(0.5): HD stores the 3D and 2D scales */
                    u32 e = gabi::ea(emitter);
                    for (u32 o : {0x220u, 0x224u, 0x228u, 0x238u, 0x23Cu, 0x240u}) gabi::store<f32>(e + o, 0.5f);
                }
                mk_voice_set(i_this, 0x48C7 /* JA_SE_CV_CB_SAVED */);
            }
            break;
        }
        mDoMtx_stack_c::transS(i_this->m2E0.x, i_this->m2E0.y, i_this->m2E0.z);
        mDoMtx_stack_c::YrotM(i_this->m2FA);
        setBaseTRMtx_now(i_this->mpMakarMorf->getModel());
        i_this->mpMakarMorf->play(nullptr, 0, 0);
        i_this->mpMakarMorf->calc();
    }
}

/* 020AEBCC */
static BOOL daBmd_Execute(bmd_class* i_this) {
    WWHD_FUNC(0x020AEBCC, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused fetch */
    if (dComIfGp_getStartStageName0() == 'X') {
        i_this->mB72 = 0x32;
    }
    i_this->m2FE = i_this->m2FE + 1;
    i_this->m330 = 0;
    for (s32 i = 0; i < 4; i++) {
        if (i_this->m308[i] != 0) {
            i_this->m308[i] = i_this->m308[i] - 1;
        }
    }
    if (i_this->m310 != 0) {
        i_this->m310 = i_this->m310 - 1;
    }
    if (i_this->m314 != 0) {
        i_this->m314 = i_this->m314 - 1;
    }
    if (i_this->m312 != 0) {
        i_this->m312 = i_this->m312 - 1;
    }
    if (i_this->m334 != 0) {
        i_this->m334 = i_this->m334 - 1;
    }
    move(i_this);
    core_move(i_this);
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), actor->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    setBaseTRMtx_now(i_this->mpBodyMorf->getModel());
    PSMTXCopy(mDoMtx_stack_c::get(), calc_mtx());
    if ((0 < i_this->m940) || (i_this->m314 != 0)) {
        MtxScale(0.0f, 0.0f, 0.0f, true);
    }
    PSMTXCopy(calc_mtx(), &i_this->mA34);
    dBgW_Move(i_this->pm_bgw[5]);
    for (s32 i = 0; i < 5; i++) {
        PSMTXCopy(mDoMtx_stack_c::get(), calc_mtx());
        cMtx_YrotM(calc_mtx(), (s16)gabi::ftoi(gabi::fmadds(13107.2f, (f32)i, REG0_F(11))));
        MtxTrans(REG0_F(4) - 200.0f, 0.0f, 0.0f, true);
        cMtx_ZrotM(calc_mtx(), i_this->m940);
        MtxTrans(-(REG0_F(4) - 200.0f), REG0_F(5) + 15.0f, REG0_F(6), true);
        if ((0 < i_this->m940) || (i_this->m314 != 0)) {
            MtxScale(0.0f, 0.0f, 0.0f, true);
        }
        PSMTXCopy(calc_mtx(), &i_this->m944[i]);
        dBgW_Move(i_this->pm_bgw[i]);
    }
    i_this->mpBodyMorf->play(&actor->eyePos, 0, 0);
    i_this->mpBodyMorf->calc();
    if (i_this->mB70 != 0) {
        i_this->mpBrkAnm->play();
        i_this->mpBtkAnm->play();
    }
    gabi::Local<cXyz> local_88;
    local_88->x = actor->current.pos.x;
    f32 y = actor->current.pos.y;
    local_88->y = y;
    local_88->z = actor->current.pos.z;
    y = y + (REG0_F(7) + 200.0f);
    if (i_this->m304 != 0) {
        y = y + 10000.0f;
    }
    local_88->y = y;
    i_this->mBodySph.SetC(local_88);
    i_this->mBodySph.SetR(REG0_F(8) + 300.0f);
    dComIfG_Ccsp_Set(&i_this->mBodySph);
    if (i_this->m904 != 0) {
        i_this->m904 = i_this->m904 - 1;
        local_88->copy(actor->current.pos);
        local_88->y = 0.0f;
        cLib_addCalc2(&i_this->m908, REG0_F(4) + 800.0f, 1.0f, REG0_F(5) + 50.0f);
    } else {
        i_this->m908 = 0.0f;
        local_88->x = -20000.0f;
        local_88->y = 10000.0f;
        local_88->z = -10000.0f;
    }
    i_this->mCoCyl.SetC(local_88);
    i_this->mCoCyl.SetR(i_this->m908);
    i_this->mCoCyl.SetH(REG0_F(3) + 3000.0f);
    dComIfG_Ccsp_Set(&i_this->mCoCyl);
    demo_camera(i_this);
    bmd_kankyo(i_this);
    eff_cont(i_this);
    i_this->mWindInfluence.mPos.copy(actor->current.pos);
    i_this->mWindInfluence.mDir.x = 0.0f;
    i_this->mWindInfluence.mDir.y = 1.0f;
    i_this->mWindInfluence.mDir.z = 0.0f;
    cLib_addCalc0(&i_this->mBD8, 1.0f, 50.0f);
    i_this->mWindInfluence.mRadius = 2000.0f - i_this->mBD8;
    if (i_this->mBD8 > 1.0f) {
        i_this->mWindInfluence.mStrength = 1.0f;
        f32 fVar2 = i_this->mWindInfluence.mRadius;
        if (fVar2 > 1500.0f) {
            i_this->mWindInfluence.mStrength = (2000.0f - fVar2) * 0.002f;
        }
    } else {
        i_this->mWindInfluence.mStrength = 0.0f;
    }
    dComIfGp_get(); /* HD: an unused fetch */
    mk_move(i_this);
    s8 b71 = i_this->mB71;
    if (b71 != 0) {
        if (actor->current.pos.y > 100.0f) {
            u32 body = gabi::ea(gabi::call<void*>(0x024F8044)); /* dCam_getBody() */
            dCamera_SetTypeForce(body, STR(0x10009C54) /* "Boss02" */);
        } else {
            u32 cam = dComIfGp_getCamera0_bl() + 0x248;
            if (b71 == 2) {
                dCamera_SetTypeForce(cam, STR(0x10009C4C) /* "Field" */);
                i_this->mB71 = 1;
            } else {
                dCamera_SetTypeForce(cam, STR(0x10009C44) /* "Dungeon" */);
            }
        }
    }
    mDoMtx_stack_c::transS(0.0f, 0.1f, 0.0f);
    setBaseTRMtx_now(i_this->mpR00_EFModel);
    i_this->mpR00_EFBrk->mFrameCtrl.setFrame(i_this->m2D8); /* setFrame(m2D8) */
    return TRUE;
}
VERIFY(0x020AEBCC, daBmd_Execute);
