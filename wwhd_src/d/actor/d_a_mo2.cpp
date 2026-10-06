/**
 * d_a_mo2.cpp (WWHD)
 * Enemy - Moblin
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mo2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_mo2.h"
#include <cmath>

/* not yet decompiled: called by address */
static BOOL daMo2_Execute(mo2_class* i_this) { return gabi::call<BOOL>(0x021C9B1C, i_this); }

/* 021C6368 */
static void tex_anm_set(mo2_class* i_this, u16 idx) {
    WWHD_FUNC(0x021C6368, void, i_this, idx);
    i_this->m02DD = 1;
    i_this->m02DE = (u8)mo2_tex_max_frame(idx);
    i_this->m02DC = 0;
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x10014F04), mo2_tex_anm_idx(idx), SAFESTRING_VTBL);
    if (btp == nullptr) JUT_ASSERT_fail(STR(0x10014F0C), 0x237, STR(0x10014F08));
    mDoExt_btpAnm_init(&i_this->m02C8, J3DModel_getModelData(i_this->mpMorf->getModel()), btp, 0, 2, 1.0f, 0, -1, 1, 0);
}
VERIFY(0x021C6368, tex_anm_set);

/* 021C6440 */
static void anm_init(mo2_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x021C6440, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (i_this->m05B0 == 0) {
        if (soundFileIdx >= 0) {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10014F20), bckFileIdx, SAFESTRING_VTBL);
            void* bas = dComIfG_getObjectRes(STR(0x10014F20), soundFileIdx, SAFESTRING_VTBL);
            i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, bas);
        } else {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10014F20), bckFileIdx, SAFESTRING_VTBL);
            i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
        }
    }
}
VERIFY(0x021C6440, anm_init);

/* 021C826C */
static void* s_w_sub(void* param_1, void*) {
    WWHD_FUNC(0x021C826C, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BOKO_e) {
        fopAc_ac_c* boko = (fopAc_ac_c*)param_1;
        if (fopAcM_GetParam(boko) == 4 /* daBoko_c::Type_MOBLIN_SPEAR_e */ && !fopAcM_checkCarryNow(boko)) {
            if (target_info_count() < 10) {
                s32 n = target_info_count();
                target_info_count() = n + 1;
                target_info()[n] = boko;
            }
        }
    }
    return nullptr;
}
VERIFY(0x021C826C, s_w_sub);

/* 021C82F4 */
static void* s_b_sub(void* param_1, void*) {
    WWHD_FUNC(0x021C82F4, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BOMB_e) {
        if (!daBomb_chk_state(param_1, 0 /* daBomb_c::STATE_0 */)) {
            if (target_info_count() < 10) {
                s32 n = target_info_count();
                target_info_count() = n + 1;
                target_info()[n] = (fopAc_ac_c*)param_1;
            }
        }
    }
    return nullptr;
}
VERIFY(0x021C82F4, s_b_sub);

/* 021C6578 */
static void smoke_set_s(mo2_class* i_this, f32 rate) {
    WWHD_FUNC(0x021C6578, void, i_this, rate);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VTBLS);
    s32 attribCode = 0;
    gabi::Local<cXyz> startPos;
    gabi::Local<cXyz> endPos;
    f32 x = i_this->m05DC.x;
    f32 y = i_this->m05DC.y;
    f32 z = i_this->m05DC.z;
    startPos->x = x;
    startPos->y = y + 100.0f;
    startPos->z = z;
    endPos->x = x;
    endPos->y = y - 100.0f;
    endPos->z = z;
    dBgS_LinChk_Set(linChk, startPos, endPos, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        cXyz* cross = gabi::at<cXyz>(gabi::ea(linChk.get()) + 0x30); /* GetCross() */
        Vec3f c = cross->get();
        *endPos = c;
        i_this->m05DC.y = c.y;
        attribCode = dBgS_GetAttributeCode(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(linChk.get()) + 0x14));
    } else {
        i_this->m05DC.y -= 20000.0f;
    }
    if (i_this->m05F3 != 0 && attribCode != 4 /* dBgS_Attr_GRASS_e */) {
        dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
        return;
    }
    i_this->m05F3++;
    switch ((u32)attribCode) {
    case 0: case 1: case 2: case 3: case 0xB: {
        dPa_smokeEcallBack_end_l(&i_this->m05F4);
        s8 room = fopAcM_GetRoomNo(i_this);
        u8 alpha = (u8)l_mo2HIO().m022;
        JPABaseEmitter* emitter1 = dComIfGp_particle_setToon(0x2022 /* ID_AK_JT_ELEMENTSMOKE00 */, &i_this->m05DC, &i_this->m05E8,
                                                             nullptr, alpha, &i_this->m05F4, room);
        if (emitter1 != nullptr) {
            JPA_setRate(emitter1, rate);
            JPA_setSpread(emitter1, 1.0f);
            JPA_setGlobalDynamicsScale(emitter1, 1.5f);
            JPA_setGlobalParticleScale(emitter1, REG0_F(11) + 3.6f);
        }
        break;
    }
    case 4: {
        JPABaseEmitter* emitter2 = dComIfGp_particle_set(0x24 /* ID_AK_JN_ELEMENTKUSA00 */, &i_this->m05DC, &i_this->m05E8);
        if (emitter2 != nullptr) {
            JPA_setRate(emitter2, rate * 0.5f);
            JPA_setMaxFrame(emitter2, 3);
        }
        break;
    }
    }
    dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
}
VERIFY(0x021C6578, smoke_set_s);

/* 021C6854 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021C6854, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (jntNo >= 0x34) {
            JUT_ASSERT_fail(STR(0x10014F4C), 0x3D7, STR(0x10014F58));
            if (jntNo >= 0x34) return TRUE;
        }
        J3DModel_l* model = j3dSys_getModel();
        mo2_class* i_this = gabi::at<mo2_class>(model->mUserArea);
        s32 r28 = joint_check(jntNo);
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == MO_JNT_JAWA_J_e) {
                cMtx_ZrotM(calc_mtx(), i_this->m2952);
                mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            } else {
                cMtx_YrotM(calc_mtx(), i_this->mDamageReaction.m088[r28].y);
                cMtx_XrotM(calc_mtx(), i_this->mDamageReaction.m088[r28].x);
                cMtx_ZrotM(calc_mtx(), i_this->mDamageReaction.m088[r28].z);
                mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());

                gabi::Local<cXyz> offset;
                offset->z = 0.0f;
                if ((u32)r28 <= 7) {
                    offset->y = 0.0f;
                    offset->x = 0.0f;
                } else if (r28 == 0x12) {
                    offset->x = 200.0f;
                    offset->y = -100.0f;
                    gabi::Local<cXyz> sp08;
                    MtxPosition(offset, sp08);
                    offset->x = 0.0f;
                    offset->y = 0.0f;
                    MtxPosition(offset, &i_this->eyePos);
                    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(i_this) + 0x390); /* attention_info.position */
                    attnPos->x = i_this->eyePos.x;
                    attnPos->y = i_this->eyePos.y;
                    attnPos->z = i_this->eyePos.z;
                    attnPos->y = attnPos->y + l_mo2HIO().m028;
                    if (l_mo2HIO().m008 == 0) {
                        i_this->m05D4 = cM_atan2s(sp08->x - i_this->eyePos.x, sp08->z - i_this->eyePos.z);
                    } else {
                        i_this->m05D4 = i_this->current.angle.y;
                    }
                    offset->x = 20.75f;
                    offset->y = 18.5f;
                    offset->z = 0.0f;
                    MtxPosition(offset, &i_this->m28C8);
                    offset->y = -45.0f;
                } else {
                    offset->y = 0.0f;
                    offset->x = 0.0f;
                }
                MtxPosition(offset, &i_this->mDamageReaction.m100[r28]);
            }
        }
    }
    return TRUE;
}
VERIFY(0x021C6854, nodeCallBack);

/* 021C6BE0 nodeCallBack_P (HD: no mMode != 100 checks) */
static BOOL nodeCallBack_P(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x021C6BE0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (jntNo >= 0x34) {
            JUT_ASSERT_fail(STR(0x10014FCC), 0x434, STR(0x10014FD8));
            if (jntNo >= 0x34) return TRUE;
        }
        J3DModel_l* model = j3dSys_getModel();
        mo2_class* i_this = gabi::at<mo2_class>(model->mUserArea);
        s32 r30 = joint_check(jntNo);
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());
            gabi::Local<cXyz> offset;
            if (r30 == 0x11) {
                offset->z = 0.0f;
                offset->y = -8.75f;
                offset->x = 17.5f;
                MtxPosition(offset, &i_this->m28EC);
            } else {
                offset->z = 0.0f;
                offset->x = 0.0f;
                if (r30 == 0x10) {
                    offset->y = 0.0f;
                    MtxPosition(offset, &i_this->m28F8);
                } else if (r30 == 0x14) {
                    offset->y = 0.0f;
                    MtxPosition(offset, &i_this->m2034);
                    offset->x = -50.0f;
                    MtxPosition(offset, &i_this->m28E0);
                    offset->x = 165.0f;
                    MtxPosition(offset, &i_this->m2040[0]);
                    offset->x = -130.0f;
                    MtxPosition(offset, &i_this->m2040[1]);
                    offset->z = 15.0f;
                    gabi::Local<cXyz> cStack_c8;
                    for (s32 i = 0; i < 16; i++) {
                        MtxPush();
                        f32 fi = (f32)i;
                        f32 t = fi * 3.5f;
                        offset->x = gabi::fmsubs((t + t) + 432.0f, 0.25f, 10.0f);
                        MtxRotX(fi * 0.73919827f, 0);
                        MtxPosition(offset, cStack_c8);
                        MtxPull();
                        MtxPosition(cStack_c8, &i_this->m0DD8[i].m000[0]);
                    }
                    i_this->m28D4.copy(i_this->m0DD8[0].m000[0]);
                } else if (r30 == 0x0E || r30 == 0x0F) {
                    offset->y = 25.0f;
                } else {
                    offset->y = 0.0f;
                }
            }
            MtxPosition(offset, &i_this->mDamageReaction.m100[r30]);
        }
    }
    return TRUE;
}
VERIFY(0x021C6BE0, nodeCallBack_P);

/* 021C6F14 ke_control: HD-changed (the global wind is replaced by a per-strand sine sway with debug
 * register tuning; the y velocity is no longer used) */
static void ke_control(mo2_class* i_this, ke_s* param_2, int param_3) {
    WWHD_FUNC(0x021C6F14, void, i_this, param_2, param_3);
    cXyz* pcVar5 = &param_2->m000[1];
    cXyz* pcVar4 = &param_2->m078[1];
    gabi::Local<cXyz> local_124;
    gabi::Local<cXyz> local_118;
    local_118->x = 0.0f;
    local_118->y = 0.0f;
    local_118->z = 21.875f;
    f32 f27 = REG_F(10, 8) + -8.25f;
    gabi::Local<dBgS_GndChk_l> gndChk;
    dBgS_GndChk_ct(gndChk);
    f32 dVar10;
    if (l_mo2HIO().m020 == 0) {
        f32 px = pcVar5->x;
        f32 pz = pcVar5->z;
        gndChk->m_pos.x = px;
        gndChk->m_pos.z = pz;
        gndChk->m_pos.y = pcVar5->y + 75.0f;
        dVar10 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk) + 2.5f;
        if (dVar10 == -1000000000.0f) {
            dVar10 = 1000000000.0f;
        }
        if (dVar10 - pcVar5->y > 50.0f) {
            dVar10 = pcVar5->y;
        }
    } else {
        dVar10 = i_this->mDamageReaction.mSpawnY + 2.5f;
    }
    f32 temp_f29 = gabi::fmadds((f32)param_3, REG_F(10, 9) + 0.007f, 0.7f);
    s32 a1 = param_3 * 0x1388 - 0x2328;
    s32 a2 = (param_3 - 1) * 0x1B58;
    for (int i = 1; i < 10; i++, pcVar5++, pcVar4++) {
        f32 x = gabi::fmadds(cM_ssin(a1), 0.5f, (pcVar5->x - pcVar5[-1].x) + pcVar4->x);
        f32 z = gabi::fmadds(cM_ssin(a2), 0.5f, (pcVar5->z - pcVar5[-1].z) + pcVar4->z);
        f32 dVar8 = pcVar5->y + f27;
        if (dVar8 < dVar10) {
            dVar8 = dVar10;
        }
        f32 y = dVar8 - pcVar5[-1].y;
        s16 iVar2 = -cM_atan2s(y, z);
        s16 iVar3 = cM_atan2s(x, std_sqrtf(gabi::fmadds(y, y, z * z)));
        mDoMtx_XrotS(calc_mtx(), iVar2);
        cMtx_YrotM(calc_mtx(), iVar3);
        MtxPosition(local_118, local_124);
        pcVar4->x = pcVar5->x;
        pcVar4->z = pcVar5->z;
        f32 nx = pcVar5[-1].x + local_124->x;
        pcVar5->x = nx;
        pcVar5->y = pcVar5[-1].y + local_124->y;
        pcVar5->z = pcVar5[-1].z + local_124->z;
        pcVar4->x = (nx - pcVar4->x) * temp_f29;
        pcVar4->z = (pcVar5->z - pcVar4->z) * temp_f29;
        a1 -= 0x2328;
        a2 -= 0x1B58;
    }
    dBgS_GndChk_dt(gndChk);
}
VERIFY(0x021C6F14, ke_control);

/* br_draw (inlined in WWHD) */
static inline void br_draw(mo2_class* i_this) {
    if (i_this->m0594 == 0) {
        return;
    }
    J3DModel* model = i_this->m058C;
    for (u16 i = 0; i < gabi::load<u16>(modelData_getJointTree(gabi::load<u32>(gabi::ea(model) + 0xAC)) + 8); i++) {
        u32 md = gabi::load<u32>(gabi::ea(model) + 0xAC);
        u32 jnts = gabi::load<u32>(md + 8);
        u32 jnt = i < gabi::load<u32>(md + 4) ? jnts + i * 0x1C : jnts;
        u32 mat = gabi::load<u32>(jnt + 0x10);
        while (mat != 0) {
            u32 shape = gabi::load<u32>(mat + 8);
            if (i_this->m0598 == 0) {
                gabi::store<u8>(shape + 4, i == 1);
            } else {
                gabi::store<u8>(shape + 4, i == 2);
            }
            mat = gabi::load<u32>(mat + 4);
        }
    }
    PSMTXCopy(model_getAnmMtx((J3DModel_l*)i_this->mpMorf->getModel(), MO_JNT_MO_YARI_e), calc_mtx());
    MtxTrans(l_mo2HIO().m138 + 150.0f, 0.0f, 0.0f, 1);
    cMtx_ZrotM(calc_mtx(), 0x4000);
    MtxScale(l_mo2HIO().m014 * i_this->m0590, l_mo2HIO().m014, l_mo2HIO().m014, 1);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, model, &i_this->tevStr);
    mDoExt_modelUpdateDL(model, 0);
}

/* search_check_draw (debug geometry, inlined; nothing is drawn) */
static inline void search_check_draw(mo2_class* i_this) {
    if (l_mo2HIO().m005 == 0) {
        return;
    }
    gabi::Local<cXyz> sp08;
    gabi::Local<cXyz> sp14[8];
    sp08->x = 0.0f;
    sp08->y = 0.0f;
    sp08->z = l_mo2HIO().m02C;
    s16 r26 = 0;
    for (int i = 0; i < 16; i++, r26 += 0x1000) {
        MtxTrans(i_this->current.pos.x, i_this->mDamageReaction.mSpawnY + 2.5f, i_this->current.pos.z, 0);
        cMtx_YrotM(calc_mtx(), r26);
        MtxPosition(sp08, sp14[0]);
        cMtx_YrotM(calc_mtx(), 0x1000);
        MtxPosition(sp08, sp14[1]);
    }
    sp08->z = l_mo2HIO().m030;
    for (int i = 0; i < 16; i++, r26 += 0x1000) {
        MtxTrans(i_this->current.pos.x, i_this->mDamageReaction.mSpawnY + 2.5f, i_this->current.pos.z, 0);
        cMtx_YrotM(calc_mtx(), r26);
        MtxPosition(sp08, sp14[0]);
        cMtx_YrotM(calc_mtx(), 0x1000);
        MtxPosition(sp08, sp14[1]);
    }
    sp08->x = 0.0f;
    sp08->z = l_mo2HIO().m02C;
    MtxTrans(i_this->current.pos.x, i_this->eyePos.y, i_this->current.pos.z, 0);
    MtxPush();
    cMtx_YrotM(calc_mtx(), i_this->m05D4 - l_mo2HIO().m038);
    sp08->y = l_mo2HIO().m03C;
    MtxPosition(sp08, sp14[1]);
    MtxPull();
    MtxPush();
    sp08->y = l_mo2HIO().m03C;
    cMtx_YrotM(calc_mtx(), i_this->m05D4 + l_mo2HIO().m038);
    MtxPosition(sp08, sp14[2]);
    MtxPull();
    MtxPush();
    cMtx_YrotM(calc_mtx(), i_this->m05D4 - l_mo2HIO().m038);
    sp08->y = -l_mo2HIO().m03C;
    MtxPosition(sp08, sp14[4]);
    MtxPull();
    sp08->y = -l_mo2HIO().m03C;
    cMtx_YrotM(calc_mtx(), i_this->m05D4 + l_mo2HIO().m038);
    MtxPosition(sp08, sp14[5]);

    f32 m03C = l_mo2HIO().m03C;
    sp14[0]->copy(i_this->current.pos);
    sp14[0]->y = i_this->eyePos.y + m03C;
    f32 m030 = l_mo2HIO().m030;
    sp14[3]->copy(i_this->current.pos);
    sp14[3]->y = i_this->eyePos.y + m03C;
    sp08->x = 0.0f;
    sp08->z = m030;
    MtxTrans(i_this->current.pos.x, i_this->eyePos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    sp08->x = l_mo2HIO().m040.x;
    sp08->y = l_mo2HIO().m040.y;
    sp08->z = l_mo2HIO().m040.z;
    MtxPosition(sp08, sp14[7]);
    sp08->y = -l_mo2HIO().m040.y;
    MtxPosition(sp08, sp14[5]);
    sp08->x = -l_mo2HIO().m040.x;
    sp08->y = l_mo2HIO().m040.y;
    MtxPosition(sp08, sp14[6]);
    sp08->y = -l_mo2HIO().m040.y;
    MtxPosition(sp08, sp14[4]);
    sp08->x = l_mo2HIO().m040.x;
    sp08->y = l_mo2HIO().m040.y;
    sp08->z = l_mo2HIO().m04C;
    MtxPosition(sp08, sp14[1]);
    sp08->y = -l_mo2HIO().m040.y;
    MtxPosition(sp08, sp14[3]);
    sp08->x = -l_mo2HIO().m040.x;
    sp08->y = l_mo2HIO().m040.y;
    MtxPosition(sp08, sp14[0]);
    sp08->y = -l_mo2HIO().m040.y;
    MtxPosition(sp08, sp14[2]);
}

/* ke_disp (inlined; HD: no wind / cM_initRnd2, extra warm-up while frozen in a statue pose) */
static inline void ke_disp(mo2_class* i_this) {
    if (i_this->mDamageReaction.mAction == ACTION_D_DOZOU && i_this->m05A4[0] != 0) {
        for (s32 j = 0; j < REG_S(10, 6) + 0x46; j++) {
            for (s32 i = 0; i < 16; i++) {
                ke_control(i_this, &i_this->m0DD8[i], i);
            }
        }
    }
    for (s32 i = 0; i < 16; i++) {
        ke_s* ke = &i_this->m0DD8[i];
        ke_control(i_this, ke, i);
        /* ke_draw */
        u32 lines = gabi::load<u32>(gabi::ea(&i_this->m3Dline) + 0x144);
        cXyz* pos = gabi::at<cXyz>(gabi::load<u32>(lines + i * 0x10));
        for (s32 k = 0; k < 10; k++) {
            pos[k].copy(ke->m000[k]);
        }
    }
    mDoExt_3DlineMat0_update(&i_this->m3Dline, 10, 1.25f, gabi::at<GXColor>(0x101BA68C), 2, &i_this->tevStr);
    dComIfGd_set3DlineMat(&i_this->m3Dline);
}

/* 021C7344 */
static BOOL daMo2_Draw(mo2_class* i_this) {
    WWHD_FUNC(0x021C7344, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    /* HD: no m2970 test */
    if (i_this->m02C1 != 0 || i_this->m2A1C != 0 || (i_this->m2A08 != 0 && i_this->m2A08 != 100)) {
        return TRUE;
    }
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, model, &i_this->tevStr);
    /* HD: the materials are found by name, before the ice test */
    u32 md = gabi::load<u32>(gabi::ea(model) + 0xAC);
    u32 mat = modelData_getMaterialByName(md, STR(0x10015040) /* "mo_e1_2_" */);
    gabi::store<u8>(gabi::load<u32>(mat + 8) + 4, i_this->mbHasInnateWeapon == 1); /* shape show/hide */
    md = gabi::load<u32>(gabi::ea(model) + 0xAC);
    mat = modelData_getMaterialByName(md, STR(0x10015038) /* "mo_e1" */);
    u32 shape = gabi::load<u32>(mat + 8);
    if (REG_S(17, 1) != 0 || i_this->m2951 != 0) {
        gabi::store<u8>(shape + 4, 0);
    } else {
        gabi::store<u8>(shape + 4, 1);
    }
    if (i_this->mEnemyIce.mFreezeTimer > 0x14) {
        dMat_ice_entryDL(i_this->mpMorf, -1, nullptr);
        return TRUE; /* HD: no blob shadow */
    }
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)&i_this->m02C8, gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(model) + 0xAC)), i_this->m02DC);
    if (!dScnPly_isPause()) {
        br_draw(i_this);
    }
    search_check_draw(i_this);
    i_this->mpMorf->entryDL();
    gabi::store<u32>(gabi::load<u32>(gabi::ea(model) + 0xAC) + 0x38, 0); /* m02C8.remove(modelData) */
    if (i_this->mbHasInnateWeapon == 1 && l_mo2HIO().m020 <= 1) {
        ke_disp(i_this);
    }
    dSnap_RegistFig(0xBD /* DSNAP_TYPE_MO2 */, i_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x021C7344, daMo2_Draw);

/* 021C7C30 */
static void way_pos_check(mo2_class* i_this, cXyz* r31) {
    WWHD_FUNC(0x021C7C30, void, i_this, r31);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VTBLS);
    gabi::Local<dBgS_GndChk_l> gndChk;
    dBgS_GndChk_ct(gndChk);
    gabi::Local<cXyz> sp2C;
    gabi::Local<cXyz> sp20;
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    sp2C->x = 0.0f;
    sp2C->y = 50.0f;
    *sp14 = i_this->current.pos.get();
    sp14->y += 50.0f;
    for (int i = 0; i < 100; i++) {
        sp2C->z = cM_rndF(200.0f) + 300.0f;
        MtxRotY(cM_rndF(6.2831855f), 0);
        MtxPosition(sp2C, sp20);
        f32 x = i_this->current.pos.x + sp20->x;
        sp08->x = x;
        f32 y = i_this->current.pos.y + sp20->y;
        sp08->y = y;
        f32 z = i_this->current.pos.z + sp20->z;
        r31->y = y;
        r31->x = x;
        sp08->z = z;
        r31->z = z;
        dBgS_LinChk_Set(linChk, sp14, sp08, i_this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            gndChk->SetPos(sp08);
            f32 g = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            if (i_this->mDamageReaction.mAcch.GetGroundH() - g < 200.0f) {
                break;
            }
        }
    }
    dBgS_GndChk_dt(gndChk);
    dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
}
VERIFY(0x021C7C30, way_pos_check);

/* 021C7EDC */
static u32 ground_4_check(mo2_class* i_this, int r18, s16 r20, f32 f29) { /* u8 in GameCube; the r3 result is used unextended */
    WWHD_FUNC(0x021C7EDC, u32, i_this, r18, r20, f29);
    gabi::Local<dBgS_GndChk_l> gndChk;
    dBgS_GndChk_ct(gndChk);
    u8 r19 = 0;
    cMtx_YrotS(calc_mtx(), r20);
    gabi::Local<cXyz> sp14;
    sp14->y = 100.0f;
    for (int i = 0; i < r18; i++) {
        sp14->x = gabi::load<f32>(0x101BA9F4 + 4 * i) * f29; /* xad */
        sp14->z = gabi::load<f32>(0x101BAA04 + 4 * i) * f29; /* zad */
        gabi::Local<cXyz> sp8;
        MtxPosition(sp14, sp8);
        PSVECAdd(sp8, &i_this->current.pos, sp8); /* sp8 += current.pos */
        gndChk->SetPos(sp8);
        f32 y = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        f32 gh = i_this->mDamageReaction.mAcch.GetGroundH();
        if (y == -1000000000.0f /* -G_CM3D_F_INF */) {
            y = 1000000000.0f;
        }
        sp8->y = y;
        if (gh - y > 200.0f) {
            r19 |= gabi::load<u8>(0x101BA9F0 + i); /* check_bit */
        }
    }
    dBgS_GndChk_dt(gndChk);
    return r19;
}
VERIFY(0x021C7EDC, ground_4_check);

/* 021C80FC */
static BOOL daMo2_other_bg_check(mo2_class* i_this, fopAc_ac_c* r23) {
    WWHD_FUNC(0x021C80FC, BOOL, i_this, r23);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VTBLS);
    if (r23 != nullptr) {
        gabi::Local<cXyz> sp14;
        gabi::Local<cXyz> sp08;
        Vec3f p = r23->current.pos.get();
        p.y += 50.0f;
        *sp08 = p;
        sp14->copy(i_this->current.pos);
        sp14->y = i_this->eyePos.y;
        dBgS_LinChk_Set(linChk, sp14, sp08, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
            return TRUE;
        } else {
            dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
            return FALSE;
        }
    }
    dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
    return TRUE;
}
VERIFY(0x021C80FC, daMo2_other_bg_check);

/* 021C8378 */
static fopAc_ac_c* search_bomb(mo2_class* i_this, int r26) {
    WWHD_FUNC(0x021C8378, fopAc_ac_c*, i_this, r26);
    if (!(i_this->m2960 & 0x0200)) {
        return nullptr;
    }
    target_info_count() = 0;
    for (int i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }
    fpcM_Search(0x021C82F4 /* s_b_sub */, i_this);

    f32 f29 = 50.0f;
    if (target_info_count() != 0) {
        int i = 0;
        while (i < target_info_count()) {
            fopAc_ac_c* r24 = target_info()[i];
            gabi::Local<cXyz> sp28;
            f32 dx = r24->current.pos.x - i_this->current.pos.x;
            sp28->x = dx;
            sp28->y = (r24->current.pos.y + 50.0f) - i_this->eyePos.y;
            f32 dz = r24->current.pos.z - i_this->current.pos.z;
            sp28->z = dz;
            f32 f0 = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            /* `<=` written as !(>) : GHS branches on the negated comparison (NaN) */
            if (f0 < f29 && !(f0 > i_this->m05C0 + 30.0f) && (!daMo2_other_bg_check(i_this, r24) || !r26)) {
                if (r26) {
                    if (!(std::fabs((r24->current.pos.y + 50.0f) - i_this->eyePos.y) > l_mo2HIO().m03C)) {
                        s16 ang = cM_atan2s(sp28->x, sp28->z);
                        s16 angleDiff = i_this->m05D4 - ang;
                        if (angleDiff < 0) {
                            angleDiff = -angleDiff;
                        }
                        if ((u16)angleDiff < l_mo2HIO().m038) {
                            return r24;
                        }
                        cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
                        gabi::Local<cXyz> sp10;
                        MtxPosition(sp28, sp10);
                        if (std::fabs(sp10->x) < l_mo2HIO().m040.x && std::fabs(sp10->y) < l_mo2HIO().m040.y &&
                            sp10->z > l_mo2HIO().m04C && sp10->z < l_mo2HIO().m040.z) {
                            return r24;
                        }
                    }
                } else {
                    return r24;
                }
            }
            i++;
            if (i == target_info_count()) {
                i = 0;
                f29 += 50.0f;
                if (f29 > 1500.0f) {
                    return nullptr;
                }
            }
        }
    } else {
        return nullptr;
    }
    return nullptr;
}
VERIFY(0x021C8378, search_bomb);

/* 021C872C daMo2_wepon_view_check, search_wepon inlined (the matcher names it search_wepon) */
static s32 daMo2_wepon_view_check(mo2_class* i_this) {
    WWHD_FUNC(0x021C872C, s32, i_this);
    if (i_this->m02E2 != 0) {
        return FALSE;
    }
    /* search_wepon */
    fopAc_ac_c* found = nullptr;
    target_info_count() = 0;
    for (int i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }
    fpcM_Search(0x021C826C /* s_w_sub */, i_this);
    f32 f29 = 50.0f;
    if (target_info_count() != 0) {
        int i = 0;
        while (i < target_info_count()) {
            fopAc_ac_c* r25 = target_info()[i];
            gabi::Local<cXyz> sp18;
            f32 dx = r25->current.pos.x - i_this->current.pos.x;
            sp18->x = dx;
            sp18->y = (r25->current.pos.y + 50.0f) - i_this->eyePos.y;
            f32 dz = r25->current.pos.z - i_this->current.pos.z;
            sp18->z = dz;
            f32 f4 = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            if (f4 < f29 && !daMo2_other_bg_check(i_this, r25)) {
                if (!(std::fabs((r25->current.pos.y + 50.0f) - i_this->eyePos.y) > l_mo2HIO().m03C)) {
                    s16 ang = cM_atan2s(sp18->x, sp18->z);
                        s16 angleDiff = i_this->m05D4 - ang;
                    if (angleDiff < 0) {
                        angleDiff = -angleDiff;
                    }
                    if ((u16)angleDiff < 0x1800) {
                        found = r25;
                        break;
                    }
                    cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
                    gabi::Local<cXyz> sp0C;
                    MtxPosition(sp18, sp0C);
                    if (std::fabs(sp0C->x) < l_mo2HIO().m040.x && std::fabs(sp0C->y) < l_mo2HIO().m040.y &&
                        sp0C->z > l_mo2HIO().m04C && sp0C->z < l_mo2HIO().m040.z) {
                        found = r25;
                        break;
                    }
                }
            }
            i++;
            if (i == target_info_count()) {
                i = 0;
                f29 += 50.0f;
                if (f29 > 1500.0f) {
                    break;
                }
            }
        }
    }
    i_this->mWeaponPcId = fopAcM_GetID(found);
    if (i_this->mWeaponPcId != fpcM_ERROR_PROCESS_ID_e && fopAcM_SearchByID(i_this->mWeaponPcId) != nullptr) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x021C872C, daMo2_wepon_view_check);

/* 021C89FC daMo2_bomb_view_check (not named by the matcher) */
static s32 daMo2_bomb_view_check(mo2_class* i_this) {
    WWHD_FUNC(0x021C89FC, s32, i_this);
    fopAc_ac_c* bomb = search_bomb(i_this, 1);
    i_this->mpBomb = bomb;
    return bomb != nullptr;
}
VERIFY(0x021C89FC, daMo2_bomb_view_check);

/* 021C8A38 */
static s32 daMo2_player_bg_check(mo2_class* i_this, cXyz* r22) {
    WWHD_FUNC(0x021C8A38, s32, i_this, r22);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (search_sp() != 0) {
        return FALSE;
    }
    if (i_this->mDamageReaction.m713 == 0 && std::fabs(player->speedF) < 0.1f && daPy_checkGrabWear(player)) {
        return 2;
    }
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VTBLS);
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    Vec3f p = r22->get();
    p.y += 75.0f;
    *sp08 = p;
    sp14->copy(i_this->current.pos);
    sp14->y = i_this->eyePos.y;
    dBgS_LinChk_Set(linChk, sp14, sp08, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        i_this->mDamageReaction.m713 = 0;
        dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
        return TRUE;
    }
    dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
    return FALSE;
}
VERIFY(0x021C8A38, daMo2_player_bg_check);

/* 021C8C14 */
static BOOL daMo2_player_view_check(mo2_class* i_this, cXyz* r30, s16 r27, s16 r31) {
    WWHD_FUNC(0x021C8C14, BOOL, i_this, r30, r27, r31);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (search_sp() != 0) {
        return TRUE;
    }
    if (daMo2_player_bg_check(i_this, r30)) {
        return FALSE;
    }
    if (std::fabs((player->current.pos.y + 50.0f) - i_this->eyePos.y) > l_mo2HIO().m03C) {
        return FALSE;
    }
    s16 angleDiff = i_this->m05D4 - r27;
    if (angleDiff < 0) {
        angleDiff = -angleDiff;
    }
    if ((u16)angleDiff < r31) {
        i_this->mDamageReaction.m713 = 1;
        return TRUE;
    }
    cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
    gabi::Local<cXyz> sp14;
    sp14->x = r30->x - i_this->current.pos.x;
    sp14->y = r30->y - i_this->current.pos.y;
    sp14->z = r30->z - i_this->current.pos.z;
    gabi::Local<cXyz> sp08;
    MtxPosition(sp14, sp08);
    if (std::fabs(sp08->x) < l_mo2HIO().m040.x && std::fabs(sp08->y) < l_mo2HIO().m040.y && sp08->z > l_mo2HIO().m04C &&
        sp08->z < l_mo2HIO().m040.z) {
        i_this->mDamageReaction.m713 = 2;
        return TRUE;
    } else {
        i_this->mDamageReaction.m713 = 0;
        return FALSE;
    }
}
VERIFY(0x021C8C14, daMo2_player_view_check);

/* 021C8DD4 */
static s32 daMo2_player_way_check(mo2_class* i_this) {
    WWHD_FUNC(0x021C8DD4, s32, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 sVar2 = i_this->current.angle.y - player->shape_angle.y;
    if (sVar2 < 0) {
        sVar2 = -sVar2;
    }
    if ((u16)sVar2 < 0x4000) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x021C8DD4, daMo2_player_way_check);

/* 021C8E30 */
static void wait_set(mo2_class* i_this) {
    WWHD_FUNC(0x021C8E30, void, i_this);
    if (i_this->mbHasInnateWeapon != 0 || i_this->m2943 != 0) {
        if (i_this->mMode == 1) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_KWAIT_e, 15.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_KWAIT_e);
            i_this->m05A4[1] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            return;
        }
        anm_init(i_this, dRes_INDEX_MO2_BCK_WAIT_e, 15.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_WAIT_e);
        i_this->m05A4[1] = (s16)gabi::ftoi(cM_rndF(60.0f) + 40.0f);
        return;
    }
    if (i_this->mDamageReaction.mAction >= ACTION_FIGHT_RUN) {
        anm_init(i_this, dRes_INDEX_MO2_BCK_NBWAIT_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NBWAIT_e);
    } else {
        anm_init(i_this, dRes_INDEX_MO2_BCK_SKYORO_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_SKYORO_e);
    }
    i_this->m05A4[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 50.0f);
}
VERIFY(0x021C8E30, wait_set);

/* 021C8FB8 */
static void fight_run_set(mo2_class* i_this) {
    WWHD_FUNC(0x021C8FB8, void, i_this);
    if (i_this->mbHasInnateWeapon != 0) {
        anm_init(i_this, dRes_INDEX_MO2_BCK_WALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_MO2_BAS_WALK_e);
    } else {
        anm_init(i_this, dRes_INDEX_MO2_BCK_SWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_SWALK_e);
    }
}
VERIFY(0x021C8FB8, fight_run_set);

/* 021C9004 */
static void path_check(mo2_class* i_this) {
    WWHD_FUNC(0x021C9004, void, i_this);
    if (ppd(i_this) != nullptr && (i_this->mbHasInnateWeapon != 0 || i_this->m2943 != 0)) {
        gabi::Local<dBgS_LinChk_l> chk;
        dBgS_LinChk_ct(chk, LINCHK_VTBLS);
        gabi::Local<cXyz> local_c8;
        gabi::Local<cXyz> local_d4;
        *local_c8 = i_this->current.pos.get();
        local_c8->y += REG_F(13, 0) + 10.0f;
        dPnt_l* point = ppd(i_this)->m_points;
        for (s32 i = 0; i < ppd(i_this)->m_num; i++, point++) {
            local_d4->x = point->m_position.x;
            local_d4->y = (point->m_position.y + 10.0f) + REG_F(13, 1);
            local_d4->z = point->m_position.z;
            dBgS_LinChk_Set(chk, local_c8, local_d4, i_this);
            check_index()[i] = !cBgS_LineCross(dComIfG_Bgsp(), chk);
        }

        f32 fDist = 100.0f;
        bool r7 = false;
        for (s32 j = 0; j < 100; j++) {
            point = ppd(i_this)->m_points;
            for (s32 i = 0; i < ppd(i_this)->m_num; i++, point++) {
                if (check_index()[i] != 0) {
                    f32 x = i_this->current.pos.x - point->m_position.x;
                    f32 y = i_this->current.pos.y - point->m_position.y;
                    f32 z = i_this->current.pos.z - point->m_position.z;
                    if (std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < fDist) {
                        s8 n = (s8)(i - (u8)i_this->mHasPath);
                        i_this->m2969 = n;
                        u16 num = ppd(i_this)->m_num;
                        if (n >= (s8)num) {
                            i_this->m2969 = (s8)num;
                        } else if (n < 0) {
                            i_this->m2969 = 0;
                        }
                        r7 = true;
                        break;
                    }
                }
            }
            if (r7) {
                break;
            }
            fDist += 50.0f;
        }
        if (!r7) {
            i_this->m2968 = 0;
        } else {
            i_this->m2968 = i_this->mPathIndex + 1;
        }
        dBgS_LinChk_dt(chk, LINCHK_VTBLS);
    }
}
VERIFY(0x021C9004, path_check);

/* 021C9310 */
static void attack_set(mo2_class* i_this, u8 param_2) {
    WWHD_FUNC(0x021C9310, void, i_this, param_2);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->m2060 = 0;
    i_this->m2941 = 0;
    i_this->m2954 = fpcM_ERROR_PROCESS_ID_e;
    i_this->m2942 = 0;
    i_this->m2068 = 0.0f;
    i_this->m05A4[4] = 0;
    f32 dVar7 = cM_rndF(100.0f);
    i_this->mWeaponSph.SetR(100.0f);
    if (i_this->mbHasInnateWeapon != 0) {
        if (daPy_checkPlayerGuard(player) && cM_rndF(1.0f) < 0.5f && param_2 == 0) {
            i_this->m2060 = 3;
            i_this->m206C = 9.0f;
            i_this->m2070 = 16.0f;
            i_this->m2074 = 0.0f;
            i_this->m2078 = (f32)l_mo2HIO().m0D2;
        } else if (param_2 == 2) {
            goto set2;
        } else if (param_2 == 1) {
            i_this->m2060 = 1;
            i_this->m206C = 26.0f;
            i_this->m2070 = 34.0f;
            i_this->m2074 = 59.0f;
            i_this->m2078 = (f32)l_mo2HIO().m0CE;
        } else if (daMo2_player_way_check(i_this)) {
            if (dVar7 < l_mo2HIO().m07C) {
                i_this->m2060 = 0;
                i_this->m206C = 17.0f;
                i_this->m2070 = 27.0f;
                i_this->m2074 = 10.0f;
                i_this->m2078 = (f32)l_mo2HIO().m0CC;
            } else if ((dVar7 - l_mo2HIO().m07C) < l_mo2HIO().m080) {
                i_this->m2060 = 1;
                i_this->m206C = 26.0f;
                i_this->m2070 = 36.0f;
                i_this->m2074 = 59.0f;
                i_this->m2078 = (f32)l_mo2HIO().m0CE;
            } else {
                goto set2;
            }
        } else {
        set2:
            i_this->m2060 = 2;
            i_this->m206C = 36.0f;
            i_this->m2070 = 52.0f;
            i_this->m2074 = 75.0f;
            i_this->m2078 = (f32)l_mo2HIO().m0D0;
        }
    } else if (dVar7 < l_mo2HIO().m084) {
        i_this->mWeaponSph.SetR(50.0f);
        i_this->m2060 = 4;
        i_this->m206C = 3.0f;
        i_this->m2070 = 8.0f;
        i_this->m2074 = 0.0f;
        i_this->m2078 = (f32)l_mo2HIO().m0D4;
    } else {
        i_this->m2060 = 5;
        i_this->m206C = 32.0f;
        i_this->m2070 = 39.0f;
        i_this->m2074 = 0.0f;
        i_this->m2078 = (f32)l_mo2HIO().m0D6;
    }
    i_this->m207E = 1;
    i_this->m2064 = 0;
    attack_info_s* info = attack_info(i_this->m2060);
    anm_init(i_this, info->bckFileIdx, 5.0f, J3DFrameCtrl::EMode_NONE, info->speed, info->soundFileIdx);
    s32 se = mo2_attack_ready_SE(i_this->m2060);
    if (se != -0xDCF) {
        if (i_this != nullptr) {
            fopAcM_monsSeStart(i_this, se, 0);
        }
    }
    if (i_this->m2060 == 1) {
        i_this->m2946 = l_mo2HIO().m144;
        i_this->m2948 = l_mo2HIO().m146;
        i_this->m294A = l_mo2HIO().m148;
        i_this->m294C = l_mo2HIO().m14A;
        i_this->m2944 = 0;
        i_this->mParryOpeningType = OPENING_ROLL_PARRY;
    } else if (i_this->m2060 == 2) {
        i_this->m2946 = l_mo2HIO().m14C;
        i_this->m2948 = l_mo2HIO().m14E;
        i_this->m294A = l_mo2HIO().m150;
        i_this->m294C = l_mo2HIO().m152;
        i_this->m2944 = 0;
        i_this->mParryOpeningType = OPENING_JUMP_PARRY;
    } else if (i_this->m2060 == 5) {
        i_this->m2946 = l_mo2HIO().m154;
        i_this->m2948 = l_mo2HIO().m156;
        i_this->m294A = l_mo2HIO().m158;
        i_this->m294C = l_mo2HIO().m15A;
        i_this->m2944 = 0;
        i_this->mParryOpeningType = OPENING_JUMP_PARRY;
    }
}
VERIFY(0x021C9310, attack_set);

/* 021C9AA0 */
static void AtHitCallback(fopAc_ac_c* a_this, dCcD_GObjInf*, fopAc_ac_c* actor, dCcD_GObjInf*) {
    WWHD_FUNC(0x021C9AA0, void, a_this, (u32)0, actor, (u32)0);
    mo2_class* i_this = (mo2_class*)a_this;
    if (rouya_mode() == 0) {
        return;
    }
    /* HD: fopAcM_GetName tests the actor for NULL */
    if (actor == nullptr || fpcM_GetName(actor) != fpcNm_PLAYER_e) {
        return;
    }
    if (!daPy_checkGrabWear(dComIfGp_getPlayer(0))) {
        return;
    }
    i_this->m2A4A = 1;
    i_this->m2A4C = 200;
    i_this->m2A48 = 1;
}
VERIFY(0x021C9AA0, AtHitCallback);

/* 021CC444 */
static BOOL daMo2_IsDelete(mo2_class*) {
    WWHD_FUNC(0x021CC444, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021CC444, daMo2_IsDelete);

/* 021CC44C */
static BOOL daMo2_Delete(mo2_class* i_this) {
    WWHD_FUNC(0x021CC44C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhsMo2, STR(0x10015120));
    dComIfG_resDelete(&i_this->mPhsSpear, STR(0x10015124));
    if (i_this->m2A0A != 0) {
        hio_set() = 0;
        mDoHIO_deleteChild(l_mo2HIO().mNo);
        camera_mode() = 0;
    }
    if (i_this->heap != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    dPa_smokeEcallBack_remove(&i_this->m05F4);
    dPa_smokeEcallBack_remove(&i_this->mDamageReaction.mParticleCallBack);
    enemy_fire_remove(&i_this->mEnemyFire);
    mDoAud_seDeleteObject(&i_this->m28D4);
    mDoAud_seDeleteObject(&i_this->m2934);
    return TRUE;
}
VERIFY(0x021CC44C, daMo2_Delete);

/* 021CC890 enemyfire::enemyfire (inline constructor, out of line in this TU) */
static enemyfire_l* enemyfire_ct(enemyfire_l* i_this) {
    WWHD_FUNC(0x021CC890, enemyfire_l*, i_this);
    if (i_this == nullptr) {
        i_this = (enemyfire_l*)operator_new(0x22C);
        if (i_this == nullptr) return nullptr;
    }
    if (gabi::ea(&i_this->mDirection) == 0) {
        operator_new(0xC); /* GHS: JGeometry::TVec3 member constructor with a null `this` */
    }
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Sph_ct(&i_this->mSph);
    gabi::store<f32>(gabi::ea(i_this) + 0x228, 1.0f); /* LIGHT_INFLUENCE (HD) */
    return i_this;
}
VERIFY(0x021CC890, enemyfire_ct);

/* 021CC91C mo2_class::mo2_class (compiler-generated) */
static mo2_class* mo2_class_ct(mo2_class* i_this) {
    WWHD_FUNC(0x021CC91C, mo2_class*, i_this);
    if (i_this == nullptr) {
        i_this = (mo2_class*)operator_new(0x343C);
        if (i_this == nullptr) return nullptr;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = MO2_VTBL;
    gabi::call(0x025E7820, &i_this->m02C8);        /* mDoExt_btpAnm */
    gabi::call(0x025E9960, &i_this->m0570);        /* mDoExt_3DlineMat0_c */
    dPa_smokeEcallBack_ct(&i_this->m05F4, 1);
    dBgS_AcchCir_ct(&i_this->mDamageReaction.mAcchCir);
    dBgS_ObjAcch_ct(&i_this->mDamageReaction.mAcch);
    dCcD_Stts_ct(&i_this->mDamageReaction.mStts);
    dPa_smokeEcallBack_ct(&i_this->mDamageReaction.mParticleCallBack, 1);
    gabi::call(0x025E9960, &i_this->m3Dline);
    dCcD_Cyl_ct(&i_this->mCoCyl, MO2_AAB_VTBL);
    dCcD_Cyl_ct(&i_this->mTgCyl, MO2_AAB_VTBL);
    dCcD_Sph_ct(&i_this->mHeadSph);
    dCcD_Sph_ct(&i_this->mDefenseSph);
    dCcD_Sph_ct(&i_this->mWeaponSph);
    dCcD_Sph_ct(&i_this->mWeapon2Sph);
    dCcD_Sph_ct(&i_this->m279C);
    dCcD_Stts_ct(&i_this->mEnemyIce.mStts);
    dCcD_Cyl_ct(&i_this->mEnemyIce.mCyl, MO2_AAB_VTBL);
    dBgS_AcchCir_ct(&i_this->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&i_this->mEnemyIce.mBgAcch);
    enemyfire_ct(&i_this->mEnemyFire);
    return i_this;
}
VERIFY(0x021CC91C, mo2_class_ct);

/* 021CC524 createHeap (useArrowHeapInit inlined). HD: a separate model (res 0x7F) instead of the
 * green material table when mMode != 1; no blur material table for the spear trail model */
static BOOL createHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021CC524, BOOL, a_this);
    mo2_class* i_this = (mo2_class*)a_this;
    J3DModelData* bdl;
    if (i_this->mMode != 1) {
        bdl = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001512C), 0x7F, SAFESTRING_VTBL);
    } else {
        bdl = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001512C), dRes_INDEX_MO2_BDL_MO_e, SAFESTRING_VTBL);
    }
    if (bdl == nullptr) JUT_ASSERT_fail(STR(0x10015134), 0x1AEB, STR(0x10015140));
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1001512C), dRes_INDEX_MO2_BCK_WAIT_e, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(STR(0x1001512C), dRes_INDEX_MO2_BAS_WAIT_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, bas,
                                            0x00080000, 0x37441422);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    J3DModel* model = i_this->mpMorf->getModel();
    for (u16 i = 0; i < gabi::load<u16>(modelData_getJointTree(gabi::load<u32>(gabi::ea(model) + 0xAC)) + 8); i++) {
        if (i >= 0x34) continue;
        s32 r3 = joint_check(i);
        if (r3 < 0) continue;
        u32 md = gabi::load<u32>(gabi::ea(model) + 0xAC);
        u32 jnts = gabi::load<u32>(md + 8);
        u32 node = i < gabi::load<u32>(md + 4) ? jnts + i * 0x1C : jnts;
        if ((u32)r3 >= 0xE && ((u32)r3 <= 0x11 || r3 == 0x14)) {
            gabi::store<u32>(node + 8, 0x021C6BE0 /* nodeCallBack_P */);
        } else {
            gabi::store<u32>(node + 8, 0x021C6854 /* nodeCallBack */);
        }
    }
    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1001512C), mo2_tex_anm_idx(3), SAFESTRING_VTBL);
    if (btp == nullptr) JUT_ASSERT_fail(STR(0x10015134), 0x1B39, STR(0x10015130));
    if (!mDoExt_btpAnm_init(&i_this->m02C8, J3DModel_getModelData(i_this->mpMorf->getModel()), btp, 0, 2, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    mDoExt_3DlineMat0_init(&i_this->m3Dline, 16, 10, 0);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001512C), 0x7E /* HD index of the spear-trail BMD (GameCube BMD_KB 0x7B) */, SAFESTRING_VTBL);
    if (modelData == nullptr) JUT_ASSERT_fail(STR(0x10015134), 0x1B5C, STR(0x10015154));
    i_this->m058C = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->m058C == nullptr) {
        return FALSE;
    }
    J3DModel_setBaseScale(i_this->m058C, &a_this->scale);
    /* useArrowHeapInit */
    i_this->mpJntHit = JntHit_create(i_this->mpMorf->getModel(), 0x101BA93C /* search_data */, 0xF);
    if (i_this->mpJntHit == nullptr) {
        return FALSE;
    }
    a_this->jntHit = gabi::ea(i_this->mpJntHit.get()); /* fopAcM_SetJntHit */
    return TRUE;
}
VERIFY(0x021CC524, createHeap);

/* kantera_get_init (inlined in daMo2_Create) */
static inline s32 kantera_get_init(mo2_class* i_this) {
    if (i_this->m2A08 == 0) {
        u8* params = fopAcM_CreateAppend();
        gabi::store<f32>(gabi::ea(params) + 4, i_this->current.pos.x);
        gabi::store<f32>(gabi::ea(params) + 8, i_this->current.pos.y);
        gabi::store<f32>(gabi::ea(params) + 0xC, i_this->current.pos.z);
        gabi::store<s16>(gabi::ea(params) + 0x12, i_this->current.angle.y);
        gabi::store<u32>(gabi::ea(params), (fopAcM_GetParam(i_this) & 0xFF000000U) | 0xFFFF23);
        gabi::store<s8>(gabi::ea(params) + 0x21, fopAcM_GetRoomNo(i_this));
        u32 layer = fpcLy_CurrentLayer();
        i_this->m02E8 = fpcSCtRq_Request(layer, fpcNm_KANTERA_e, 0, 0, params); /* fopAcM_Create */
        i_this->m2A08++;
        return TRUE;
    } else if (i_this->m2A08 != 100) {
        fopAc_ac_c* kantera = fopAcM_SearchByID(i_this->m02E8);
        if (kantera != nullptr) {
            gabi::store<u32>(gabi::ea(kantera) + 0x3C0, fopAcM_GetID(i_this)); /* mTargetActorID */
            i_this->m2A08 = 100;
            return FALSE;
        }
        i_this->m2A08++;
        if (i_this->m2A08 > 0x14) {
            fopAcM_delete(i_this);
        }
        return TRUE;
    }
    return FALSE;
}

/* 021CCB00 */
static cPhs_State daMo2_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021CCB00, cPhs_State, a_this);
    /* fopAcM_ct(a_this, mo2_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            mo2_class_ct((mo2_class*)a_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    mo2_class* i_this = (mo2_class*)a_this;
    cPhs_State res = dComIfG_resLoad(&i_this->mPhsSpear, STR(0x1001518C) /* "Spear" */);
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    cPhs_State res2 = dComIfG_resLoad(&i_this->mPhsMo2, STR(0x10015184) /* "Mo2" */);
    if (res2 != cPhs_COMPLEATE_e) {
        return res2;
    }
    i_this->mMode = (u8)fopAcM_GetParam(a_this);
    if (i_this->mMode == 1 && kantera_get_init(i_this) != 0) {
        return cPhs_INIT_e;
    }
    a_this->gbaName = 2;
    /* HD: also kazeMB */
    if (SafeString_eq(STR(0x1001517C) /* "ITest63" */, dComIfGp_getStartStageName(), SAFESTRING_VTBL) ||
        SafeString_eq(STR(0x10015194) /* "GanonJ" */, dComIfGp_getStartStageName(), SAFESTRING_VTBL) ||
        SafeString_eq(STR(0x1001519C) /* "kazeMB" */, dComIfGp_getStartStageName(), SAFESTRING_VTBL)) {
        search_sp() = 1;
    } else {
        search_sp() = 0;
    }
    u32 prm = fopAcM_GetParam(a_this);
    s16 az = a_this->current.angle.z;
    a_this->current.angle.z = 0;
    i_this->mDeathSwitch = (u8)az;
    i_this->mFrozenInTimePose = prm >> 8;
    a_this->current.angle.x = 0;
    i_this->mPathIndex = prm >> 0x10;
    i_this->mEnableSpawnSwitch = prm >> 0x18;
    if ((u8)az == 0xFF) {
        i_this->mDeathSwitch = 0;
    } else if ((u8)az != 0 && (u8)az <= 0x7F) {
        a_this->actor_status |= fopAcStts_BOSS_e;
        search_sp() = 1;
    }
    if (dComIfGs_isEventBit(0x1101) && i_this->mDeathSwitch != 0 &&
        dComIfGs_isSwitch(i_this->mDeathSwitch, fopAcM_GetRoomNo(a_this))) {
        return cPhs_ERROR_e;
    }
    if (SafeString_eq(STR(0x100151A4) /* "Hyroom" */, dComIfGp_getStartStageName(), SAFESTRING_VTBL)) {
        a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex2(STR(0x10015188) /* "mo2" */, 1);
    } else {
        a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x10015188), 0);
    }
    if (!fopAcM_entrySolidHeap(a_this, 0x021CC524 /* createHeap */, 0)) {
        return cPhs_ERROR_e;
    }
    if (hio_set() == 0) {
        l_mo2HIO().mNo = mDoHIO_createChild(STR(0x100151AC) /* "モ２" */, &l_mo2HIO());
        l_mo2HIO().m020 = 0;
        camera_mode() = 0;
        i_this->m2A0A = 1;
        hio_set() = 1;
        alerm_set() = 0;
    }
    fopAcM_SetMin(a_this, -200.0f, -50.0f, -100.0f);
    fopAcM_SetMax(a_this, 125.0f, 250.0f, 250.0f);
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
    gabi::store<u32>(gabi::ea(i_this->mpMorf->getModel()) + 0xB8, gabi::ea(a_this)); /* setUserArea */
    i_this->mBtBodyR = 125.0f; /* initBt(162.5f, 125.0f) */
    i_this->mDamageReaction.mSpawnY = a_this->current.pos.y;
    i_this->mBtHeight = 162.5f;
    i_this->mbHasInnateWeapon = 1;
    i_this->mDamageReaction.m70C = 1;
    i_this->mDamageReaction.mMaxFallDistance = 1000.0f;
    i_this->mDamageReaction.mInvincibleTimer = 5;
    if (dComIfGs_isCollect(0, 0) || dComIfGs_isCollect(0, 1) || dComIfGs_isCollect(0, 2) || dComIfGs_isCollect(0, 3)) {
        rouya_mode() = 0;
    } else {
        rouya_mode() = 1;
    }
    if (i_this->mMode != 1 || rouya_mode() == 0) {
        attention_flags(a_this) = fopAc_Attn_LOCKON_BATTLE_e;
        if (i_this->mMode == 5) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_BB_FLY_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_BB_FLY_e);
            i_this->mDamageReaction.mAction = ACTION_CARRY;
            i_this->mDamageReaction.mMaxFallDistance = 100000.0f;
        }
    }
    if (i_this->mPathIndex != 0xFF) {
        i_this->ppd = dPath_GetRoomPath(i_this->mPathIndex, fopAcM_GetRoomNo(a_this));
        if (i_this->ppd == nullptr) {
            return cPhs_ERROR_e;
        }
        i_this->mHasPath = 1;
        i_this->m2968 = i_this->mPathIndex + 1;
    }
    if (i_this->mEnableSpawnSwitch != 0xFF) {
        i_this->m02C1 = i_this->mEnableSpawnSwitch + 1;
    }
    if (i_this->mMode == 0xf) {
        i_this->m02C1 = 0;
        i_this->mDamageReaction.mAction = ACTION_D_DOZOU;
        /* HD: unless a save byte (+0x2E) is 0x39, 0x3E or 0x3A, wait 5 frames */
        u8 b = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E);
        if (b != 0x39 && b != 0x3E && b != 0x3A) {
            i_this->m05A4[0] = 5;
        }
    }
    i_this->mDamageReaction.mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mDamageReaction.mAcchCir,
                                      &a_this->speed);
    i_this->mDamageReaction.mAcchCir.SetWall(50.0f, 50.0f);
    i_this->mDamageReaction.mAcch.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE; /* ClrRoofNone */
    i_this->mDamageReaction.mAcch.SetRoofCrrHeight(REG0_F(7) + 100.0f);
    a_this->health = 10;
    a_this->max_health = 10;
    i_this->mDamageReaction.mStts.Init(200, 0xFF, a_this);
    dCcD_Stts* stts = &i_this->mDamageReaction.mStts;
    i_this->mCoCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BAB48));
    i_this->mCoCyl.SetStts(stts);
    i_this->mTgCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BAB8C));
    i_this->mTgCyl.SetStts(stts);
    i_this->mHeadSph.Set(gabi::at<dCcD_SrcSph>(0x101BAA14));
    i_this->mHeadSph.SetStts(stts);
    i_this->mWeaponSph.Set(gabi::at<dCcD_SrcSph>(0x101BAA54));
    i_this->mWeaponSph.SetStts(stts);
    i_this->mWeapon2Sph.Set(gabi::at<dCcD_SrcSph>(0x101BAA94));
    i_this->mWeapon2Sph.SetStts(stts);
    i_this->mWeaponSph.mGObjAt.mHitCallback = 0x021C9AA0;  /* AtHitCallback */
    i_this->mWeapon2Sph.mGObjAt.mHitCallback = 0x021C9AA0;
    i_this->mDefenseSph.Set(gabi::at<dCcD_SrcSph>(0x101BAAD4));
    i_this->mDefenseSph.SetStts(stts);
    mDoExt_McaMorf* morf = i_this->mpMorf;
    a_this->model = gabi::ea(morf->getModel());
    i_this->mEnemyIce.mpActor = a_this;
    i_this->mEnemyIce.mWallRadius = REG0_F(6) + 70.0f;
    i_this->mEnemyIce.mCylHeight = REG0_F(7) + 200.0f;
    i_this->mEnemyIce.mParticleScale = 1.5f;
    i_this->mEnemyIce.mDeathSwitch = i_this->mDeathSwitch;
    i_this->mEnemyFire.mpMcaMorf = morf;
    i_this->mEnemyFire.mpActor = a_this;
    for (s32 i = 0; i < 10; i++) {
        i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101BAB3C + i);       /* fire_j */
        i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101BAB14 + 4 * i); /* fire_sc */
    }
    a_this->stealItemLeft = 5;
    daMo2_Execute(i_this);
    daMo2_Execute(i_this);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021CCB00, daMo2_Create);

/* 021CD5B4 __sinit_d_a_mo2_cpp (the matcher names it mo2HIO_c::mo2HIO_c): header statics, then the
 * mo2HIO_c constructor of l_mo2HIO (HD values: m0CE, m0D0 and m0D6 are 10; GameCube 0x18, 0x16, 0x1C) */
static void __sinit_d_a_mo2_cpp() {
    WWHD_FUNC(0x021CD5B4, void, (u32)0);
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x104654D0 + 4 * i, 0);
    __register_global_object(0x101BABD0);
    gabi::store<f32>(0x1046549C, -3.1415927f);
    gabi::store<f32>(0x104654A0, 3.1415927f);
    gabi::call(0x028ED6F8, 0x104654A5u);
    __register_global_object(0x101BABDC);
    gabi::call(0x028EAB2C, 0x104654A6u);
    __register_global_object(0x101BABE8);
    u32 hio = 0x104654E0;
    gabi::store<u32>(hio + 0x184, 0x10014E8C); /* mo2HIO_c vtable */
    gabi::call(0x02552BE8, hio + 0x158);       /* JntHit_HIO_c::JntHit_HIO_c */
    gabi::store<u8>(hio + 0x1, 0);
    gabi::store<u8>(hio + 0x2, 0);
    gabi::store<u8>(hio + 0x3, 0);
    gabi::store<u8>(hio + 0x4, 0);
    gabi::store<u8>(hio + 0x5, 0);
    gabi::store<s16>(hio + 0x6, 20);
    gabi::store<s16>(hio + 0x8, 40);
    gabi::store<f32>(hio + 0xC, 1.0f);
    gabi::store<f32>(hio + 0x10, 1.0f);
    gabi::store<f32>(hio + 0x14, 1.0f);
    gabi::store<f32>(hio + 0x18, 25.0f);
    gabi::store<s16>(hio + 0x1C, 0);
    gabi::store<s16>(hio + 0x1E, 185);
    gabi::store<s16>(hio + 0x20, 12);
    gabi::store<f32>(hio + 0x24, 25.0f);
    gabi::store<f32>(hio + 0x28, 1250.0f);
    gabi::store<f32>(hio + 0x2C, 575.0f);
    gabi::store<f32>(hio + 0x30, 375.0f);
    gabi::store<s16>(hio + 0x34, 12740);
    gabi::store<f32>(hio + 0x38, 400.0f);
    gabi::store<f32>(hio + 0x3C, 500.0f);
    gabi::store<f32>(hio + 0x40, 130.0f);
    gabi::store<f32>(hio + 0x44, 500.0f);
    gabi::store<f32>(hio + 0x48, -125.0f);
    gabi::store<f32>(hio + 0x4C, 12.0f);
    gabi::store<f32>(hio + 0x50, 60.0f);
    gabi::store<f32>(hio + 0x54, 45.0f);
    gabi::store<f32>(hio + 0x58, 70.0f);
    gabi::store<f32>(hio + 0x5C, 90.0f);
    gabi::store<f32>(hio + 0x60, 20.0f);
    gabi::store<f32>(hio + 0x64, 20.0f);
    gabi::store<f32>(hio + 0x68, 70.0f);
    gabi::store<f32>(hio + 0x6C, 1.0f);
    gabi::store<s16>(hio + 0x70, 15);
    gabi::store<f32>(hio + 0x74, 80.0f);
    gabi::store<f32>(hio + 0x78, 40.0f);
    gabi::store<f32>(hio + 0x7C, 30.0f);
    gabi::store<f32>(hio + 0x80, 50.0f);
    gabi::store<s16>(hio + 0x84, 30);
    gabi::store<s16>(hio + 0x86, 300);
    gabi::store<f32>(hio + 0x88, 0.899999976f);
    gabi::store<f32>(hio + 0x8C, 1.0f);
    gabi::store<f32>(hio + 0x90, 1.0f);
    gabi::store<f32>(hio + 0x94, 1.0f);
    gabi::store<f32>(hio + 0x98, 1.0f);
    gabi::store<f32>(hio + 0x9C, 1.0f);
    gabi::store<f32>(hio + 0xA0, 1.10000002f);
    gabi::store<f32>(hio + 0xA4, 1.0f);
    gabi::store<f32>(hio + 0xA8, 1.0f);
    gabi::store<f32>(hio + 0xAC, 0.5f);
    gabi::store<f32>(hio + 0xB0, 1.0f);
    gabi::store<f32>(hio + 0xB4, 1.20000005f);
    gabi::store<f32>(hio + 0xB8, 1.0f);
    gabi::store<f32>(hio + 0xBC, 1.0f);
    gabi::store<f32>(hio + 0xC0, 1.0f);
    gabi::store<f32>(hio + 0xC4, 1.0f);
    gabi::store<s16>(hio + 0xC8, 14);
    gabi::store<s16>(hio + 0xCA, 10);
    gabi::store<s16>(hio + 0xCC, 10);
    gabi::store<s16>(hio + 0xCE, 5);
    gabi::store<s16>(hio + 0xD0, 100);
    gabi::store<s16>(hio + 0xD2, 10);
    gabi::store<s16>(hio + 0xD4, 29);
    gabi::store<s16>(hio + 0xD6, 1);
    gabi::store<s16>(hio + 0xD8, 2);
    gabi::store<s16>(hio + 0xDA, 1);
    gabi::store<f32>(hio + 0xDC, 1.0f);
    gabi::store<f32>(hio + 0xE0, 1.25f);
    gabi::store<f32>(hio + 0xE4, 1.25f);
    gabi::store<f32>(hio + 0xE8, 1.0f);
    gabi::store<f32>(hio + 0xEC, 0.5f);
    gabi::store<f32>(hio + 0xF0, 0.5f);
    gabi::store<f32>(hio + 0xF4, 0.5f);
    gabi::store<f32>(hio + 0xF8, 1.0f);
    gabi::store<f32>(hio + 0xFC, 1.0f);
    gabi::store<f32>(hio + 0x100, 1.0f);
    gabi::store<s16>(hio + 0x104, 40);
    gabi::store<s16>(hio + 0x106, 1);
    gabi::store<s16>(hio + 0x108, 3);
    gabi::store<s16>(hio + 0x10A, 1);
    gabi::store<f32>(hio + 0x10C, 1.0f);
    gabi::store<f32>(hio + 0x110, 1.10000002f);
    gabi::store<f32>(hio + 0x114, 1.14999998f);
    gabi::store<f32>(hio + 0x118, 1.25f);
    gabi::store<f32>(hio + 0x11C, 1.10000002f);
    gabi::store<f32>(hio + 0x120, 1.0f);
    gabi::store<f32>(hio + 0x124, 1.0f);
    gabi::store<f32>(hio + 0x128, 1.0f);
    gabi::store<f32>(hio + 0x12C, 1.0f);
    gabi::store<f32>(hio + 0x130, 1.0f);
    gabi::store<f32>(hio + 0x134, -22.25f);
    gabi::store<f32>(hio + 0x138, 30.0f);
    gabi::store<f32>(hio + 0x13C, 650.0f);
    gabi::store<s16>(hio + 0x140, 8);
    gabi::store<s16>(hio + 0x142, 12);
    gabi::store<s16>(hio + 0x144, 34);
    gabi::store<s16>(hio + 0x146, 25);
    gabi::store<s16>(hio + 0x148, 16);
    gabi::store<s16>(hio + 0x14A, 12);
    gabi::store<s16>(hio + 0x14C, 45);
    gabi::store<s16>(hio + 0x14E, 30);
    gabi::store<s16>(hio + 0x150, 8);
    gabi::store<s16>(hio + 0x152, 12);
    gabi::store<s16>(hio + 0x154, 40);
    gabi::store<s16>(hio + 0x156, 25);
}
VERIFY(0x021CD5B4, __sinit_d_a_mo2_cpp);

/* 021CD944: mo2HIO_c deleting destructor (compiler-generated; JORReflexible has no members) */
static void mo2HIO_c_dt(mo2HIO_c* i_this, s32 flags) {
    WWHD_FUNC(0x021CD944, void, i_this, flags);
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}
VERIFY(0x021CD944, mo2HIO_c_dt);

/* 021CD958 nage: HD-changed (the -8 state chases the player before throwing; no m713 == 2 shortcut) */
static void nage(mo2_class* i_this) {
    WWHD_FUNC(0x021CD958, void, i_this);
    dComIfGp_getPlayer(0);
    u8 iVar5 = 0;
    s16 maxSpeed = 0x400;
    i_this->m02E0 = 2;
    i_this->mDamageReaction.m710 = 1;
    i_this->mDamageReaction.m4D0 = i_this->m05D6;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case (u16)-10:
        if (i_this->m05B4 != 0 || i_this->m2A0C != 0 ||
            daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
            i_this->m05A4[1] = l_mo2HIO().m00A + l_mo2HIO().m00C;
            i_this->m05A4[2] = l_mo2HIO().m00C;
            anm_init(i_this, dRes_INDEX_MO2_BCK_KKEIKAI_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_KKEIKAI_e);
            i_this->mDamageReaction.m713 = 0;
            i_this->mDamageReaction.mMode = -9;
            i_this->m05A4[0] = REG0_S(7) + 0xf;
            if (camera_mode() == 0) {
                i_this->m2A1D = 2;
                i_this->m2A40 = REG_F(8, 3) + 50.0f;
            }
        } else {
            iVar5 = 1;
            break;
        }
        /* fallthrough */
    case (u16)-9:
        maxSpeed = i_this->m05A4[0] != 0 ? (s16)0 : (s16)0x1000;
        if (i_this->m05A4[1] == 0) {
            i_this->speedF = 0.0f;
            iVar5 = 2;
            break;
        }
        if (i_this->m05A4[2] == 0 &&
            daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
            /* HD: run at the player first (state -8) */
            i_this->mDamageReaction.mMode = -8;
            fight_run_set(i_this);
            i_this->speedF = 0.0f;
            i_this->m05A4[0] = 0x50;
            i_this->mDamageReaction.m713 = 0;
            break;
        }
        i_this->speedF = 0.0f;
        break;
    case (u16)-8:
        /* HD */
        cLib_addCalc2(&i_this->speedF, l_mo2HIO().m058, 1.0f, 20.0f);
        if (i_this->m05A4[0] == 0 || i_this->m05C0 < l_mo2HIO().m030 * 0.5f) {
            if (daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
                i_this->mDamageReaction.mMode = -5;
                if (i_this != nullptr) {
                    fopAcM_monsSeStart(i_this, JA_SE_CV_MO_ALERT, 0);
                }
                i_this->m2A4A = 1;
                i_this->m2A48 = 1;
            } else {
                i_this->mDamageReaction.mMode = 2;
                s16 base = l_mo2HIO().m00A + l_mo2HIO().m00C;
                i_this->m05A4[1] = base + (s16)gabi::ftoi(cM_rndF(40.0f) + 60.0f);
                i_this->m05A4[2] = l_mo2HIO().m00C;
                anm_init(i_this, dRes_INDEX_MO2_BCK_KKEIKAI_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_KKEIKAI_e);
                i_this->mDamageReaction.m713 = 0;
                i_this->mDamageReaction.mMode = -9;
                i_this->m05A4[0] = REG0_S(7) + 0xf;
            }
        }
        break;
    case 0:
        maxSpeed = 0;
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mMode = 1;
            anm_init(i_this, dRes_INDEX_MO2_BCK_KWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_MO2_BAS_KWALK_e);
        }
        i_this->speedF = 0.0f;
        break;
    case 1:
        cLib_addCalc2(&i_this->speedF, l_mo2HIO().m058, 1.0f, 20.0f);
        if (i_this->m05C0 < l_mo2HIO().m030) {
            i_this->mDamageReaction.mMode = 2;
            anm_init(i_this, dRes_INDEX_MO2_BCK_KNAGE_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_KNAGE_e);
        }
        break;
    case 2: {
        maxSpeed = 0xc00;
        cLib_addCalc0(&i_this->speedF, 1.0f, 20.0f);
        s32 frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (frame == REG0_S(6) + 0xf) {
            i_this->mMode = 0;
            i_this->m2A09 = 2;
        }
        if (i_this->mpMorf->isStop()) {
            iVar5 = 1;
        }
        break;
    }
    }
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 3, maxSpeed);
    if (iVar5 != 0 || daMo2_player_bg_check(i_this, &i_this->mDamageReaction.m714->current.pos) == 1) {
        i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
        wait_set(i_this);
        if (iVar5 == 2) {
            i_this->m05A4[1] = 0;
        }
        i_this->mDamageReaction.mMode = 2;
        i_this->m2969 = i_this->m2969 - i_this->mHasPath;
        if (i_this->m2A1D != 0) {
            i_this->m2A1D = 10;
        }
    }
}
VERIFY(0x021CD958, nage);

/* 021CDE6C */
static void p_lost(mo2_class* i_this) {
    WWHD_FUNC(0x021CDE6C, void, i_this);
    dComIfGp_getPlayer(0);
    i_this->m02E0 = 2;
    i_this->mDamageReaction.m710 = 0;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case (u16)-10:
        if (i_this->mDamageReaction.m470 < 200.0f) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_UKYADEMO_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mDamageReaction.mMode = -9;
            tex_anm_set(i_this, 4);
            i_this->m05A4[1] = 0x23;
        }
        break;
    case (u16)-9:
        i_this->speedF = 0.0f;
        if (i_this->m05A4[1] == (s16)(REG_S(17, 5) + 0x19)) {
            fopAcM_monsSeStart(i_this, JA_SE_CV_MO_STOLEN, 0);
        }
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = 2;
        }
        break;
    case 0:
        i_this->mDamageReaction.mMode = 1;
        tex_anm_set(i_this, 4);
        anm_init(i_this, dRes_INDEX_MO2_BCK_BKYORO_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_BKYORO_e);
        break;
    case 1:
        i_this->speedF = 0.0f;
        if ((i_this->m059C & 0x1F) == 0 && cM_rndF(1.0f) < 0.5f) {
            fopAcM_monsSeStart(i_this, JA_SE_CV_MO_SEARCH, 0);
        }
        if (i_this->mpMorf->isStop()) {
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            wait_set(i_this);
            i_this->m05A4[1] = 0;
            i_this->mDamageReaction.mMode = 2;
        }
    }
    if (gabi::ftoi(i_this->mpMorf->getFrame()) > 25 && i_this->m05A4[1] == 0 &&
        daMo2_player_view_check(i_this, &i_this->mDamageReaction.m714->current.pos, i_this->m05D6, l_mo2HIO().m038)) {
        i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
        i_this->mDamageReaction.mMode = 2;
    }
}
VERIFY(0x021CDE6C, p_lost);

/* 021CE138 */
static void b_nige(mo2_class* i_this) {
    WWHD_FUNC(0x021CE138, void, i_this);
    /* daMo2_bomb_check */
    i_this->mpBomb = search_bomb(i_this, 0);
    if (i_this->mpBomb == nullptr) {
        i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
        path_check(i_this);
        wait_set(i_this);
        i_this->mDamageReaction.mMode = 2;
    } else {
        fopAc_ac_c* r3 = i_this->mpBomb;
        f32 x = r3->current.pos.x - i_this->current.pos.x;
        f32 z = r3->current.pos.z - i_this->current.pos.z;
        i_this->mDamageReaction.m4D0 = cM_atan2s(-x, -z);
        switch ((u16)i_this->mDamageReaction.mMode) {
        case 0:
            i_this->mDamageReaction.mMode = 1;
            anm_init(i_this, dRes_INDEX_MO2_BCK_SHAKKEN_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_SHAKKEN_e);
            i_this->m05A4[1] = 0x14;
            /* fallthrough */
        case 1:
            i_this->speedF = 0.0f;
            cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(i_this->mDamageReaction.m4D0 + 0x8000), 2, 0x3000);
            if (i_this->m05A4[1] == 0) {
                i_this->mDamageReaction.mMode = 2;
                anm_init(i_this, dRes_INDEX_MO2_BCK_NWALK_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NWALK_e);
            }
            break;
        case 2:
            i_this->speedF = l_mo2HIO().m060;
            i_this->m05F0 = l_mo2HIO().m024 + 3;
            i_this->m05F2 = 4;
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x1000);
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) > 800.0f) {
                i_this->mDamageReaction.mMode = 3;
                anm_init(i_this, dRes_INDEX_MO2_BCK_NBWAIT_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_NBWAIT_e);
            }
            break;
        case 3:
            i_this->speedF = 0.0f;
            i_this->mDamageReaction.m4D0 = i_this->m05D6;
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 3, 0x1000);
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 700.0f) {
                i_this->mDamageReaction.mMode = 0;
            }
        }
    }
}
VERIFY(0x021CE138, b_nige);

/* 021CE46C */
static void defence(mo2_class* i_this) {
    WWHD_FUNC(0x021CE46C, void, i_this);
    dComIfGp_getPlayer(0); /* GameCube: unused `player` local; HD still calls the accessor */
    i_this->m02E0 = 2;
    i_this->mDamageReaction.m710 = 1;
    i_this->mDamageReaction.m4D0 = i_this->m05D6;
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x400);
    i_this->mDamageReaction.m711 = 1;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_BWALKFB_e, 5.0f, J3DFrameCtrl::EMode_NONE, -1.0f, dRes_INDEX_MO2_BAS_BWALKFB_e);
        i_this->m05A4[1] = REG_S(6, 3) + 0x1e;
        tex_anm_set(i_this, 4);
        i_this->speedF = -20.0f;
        /* fallthrough */
    case 1:
        i_this->mDefenseSph.SetR(62.5f);
        i_this->m2928.copy(i_this->m28E0);
        if (i_this->mpMorf->isStop()) {
            i_this->speedF = 0.0f;
        }
        if (i_this->m05A4[1] == 0) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = 0;
            i_this->m05A4[1] = 0;
        }
        break;
    }
}
VERIFY(0x021CE46C, defence);

/* 021CE5B8 */
static void oshi(mo2_class* i_this) {
    WWHD_FUNC(0x021CE5B8, void, i_this);
    dComIfGp_getPlayer(0);
    i_this->m02E0 = 2;
    i_this->mDamageReaction.m4D0 = i_this->m05D6;
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 3, 0x800);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_ADOTSUKI_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_ADOTSUKI_e);
        tex_anm_set(i_this, 4);
        i_this->speedF = 0.0f;
        /* fallthrough */
    case 1:
        i_this->m2928.copy(i_this->m28E0);
        i_this->mDefenseSph.SetR(87.5f);
        if (i_this->mpMorf->isStop()) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = 0;
            i_this->m05A4[1] = 0;
        }
        break;
    }
}
VERIFY(0x021CE5B8, oshi);

/* 021CE6E8 */
static void hukki(mo2_class* i_this) {
    WWHD_FUNC(0x021CE6E8, void, i_this);
    gabi::Local<cXyz> local_2c;
    gabi::Local<cXyz> local_38;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dx = i_this->mDamageReaction.m100[10].x - i_this->mDamageReaction.m100[0xd].x;
    local_2c->x = dx;
    f32 dz = i_this->mDamageReaction.m100[10].z - i_this->mDamageReaction.m100[0xd].z;
    local_2c->z = dz;
    Mtx34* m = calc_mtx();
    cMtx_YrotS(m, cM_atan2s(dx, dz));
    local_2c->x = 0.0f;
    local_2c->y = 0.0f;
    /* HD: no `m05B4 = 2` here; it is set (REG10_S(0) + 0x23) when getting up starts */
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 10:
        anm_init(i_this, dRes_INDEX_MO2_BCK_OKIRUA_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_OKIRUA_e);
        i_this->m05A4[2] = 0xF;
        goto LAB_806ae194;
    case 12:
        anm_init(i_this, dRes_INDEX_MO2_BCK_OKIRUU_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_OKIRUU_e);
        i_this->m05A4[2] = REG0_S(5) + 0xF;
    LAB_806ae194:
        tex_anm_set(i_this, 5);
        i_this->speedF = 0.0f;
        i_this->mDamageReaction.mMode = 0xD;
        i_this->mDamageReaction.m480 = 0;
        local_2c->z = 125.0f;
        MtxPosition(local_2c, &i_this->mDamageReaction.m458);
        i_this->current.pos.y -= i_this->mDamageReaction.m44C.y;
        i_this->old.pos.y -= i_this->mDamageReaction.m44C.y;
        i_this->mDamageReaction.m44C.y = 0.0f;
        i_this->m05B4 = REG_S(10, 0) + 0x23;
        /* fallthrough */
    case 13:
        i_this->m02E0 = 2;
        if (i_this->m05A4[2] <= REG_S(6, 5) + 6) {
            cLib_addCalc0(&i_this->mDamageReaction.m458.x, 1.0f, (REG_F(6, 5) + 100.0f) * 0.25f);
            cLib_addCalc0(&i_this->mDamageReaction.m458.z, 1.0f, (REG_F(6, 5) + 100.0f) * 0.25f);
            if (i_this->m05A4[2] >= REG_S(6, 6) + 1) {
                local_2c->z = 25.0f;
                MtxPosition(local_2c, local_38);
                i_this->current.pos.x += local_38->x;
                i_this->current.pos.z += local_38->z;
            }
        }
        if (i_this->m05A4[2] == 1) {
            i_this->m05F0 = l_mo2HIO().m024 + 6;
            i_this->m05F2 = 3;
        }
        if (i_this->mpMorf->isStop()) {
            if (!daPy_checkGrabWear(player) && i_this->m05C0 < l_mo2HIO().m030) {
                i_this->mDamageReaction.m488 = 0;
                i_this->mDamageReaction.mMode = 0xe;
                i_this->m05A4[1] = 10;
            } else {
                i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
                path_check(i_this);
                wait_set(i_this);
                i_this->mDamageReaction.m488 = 0;
                i_this->mDamageReaction.mMode = 2;
            }
        }
        break;
    case 14:
        i_this->mDamageReaction.m710 = 1;
        i_this->mDamageReaction.m4D0 = i_this->m05D6;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 3, 0x1000);
        if (i_this->m05A4[1] == 0) {
            if (l_mo2HIO().m009 == 0) {
                i_this->m2060 = 5;
                i_this->mDamageReaction.mAction = ACTION_FIGHT;
                i_this->m2068 = REG0_F(8) + 29.0f;
                i_this->m206C = 32.0f;
                i_this->m2070 = 39.0f;
                i_this->m2074 = 0.0f;
                i_this->m2078 = (f32)l_mo2HIO().m0D6;
                i_this->mWeaponSph.SetR(REG_F(10, 13) + 100.0f); /* HD */
                i_this->m2064 = 1;
                i_this->m207E = 1;
                attack_info_s* info = attack_info(i_this->m2060);
                info++;
                anm_init(i_this, info->bckFileIdx, 5.0f, 0, info->speed, info->soundFileIdx);
                tex_anm_set(i_this, 6);
                i_this->m2942 = 1;
                i_this->mDamageReaction.mMode = 1;
            } else {
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
            }
        }
        break;
    }
}
VERIFY(0x021CE6E8, hukki);

/* 021CEC44 */
static void aite_miru(mo2_class* i_this) {
    WWHD_FUNC(0x021CEC44, void, i_this);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->m05A4[1] = REG0_S(8) + 0x14;
        i_this->speedF = 0.0f;
        i_this->mDamageReaction.mMode = 1;
        tex_anm_set(i_this, 4);
        fopAcM_monsSeStart(i_this, JA_SE_CV_MO_SEARCH, 0);
        /* fallthrough */
    case 1: {
        cLib_addCalcAngleS2(&i_this->m2952, 12000, 2, 0x1800);
        i_this->mDamageReaction.m710 = 1;
        if (i_this->m2954 != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* pfVar2 = fopAcM_SearchByID(i_this->m2954);
            if (pfVar2 != nullptr) {
                i_this->mDamageReaction.m714 = pfVar2;
            }
        }
        if (i_this->m05A4[1] == 0) {
            i_this->m2954 = fpcM_ERROR_PROCESS_ID_e;
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            i_this->mDamageReaction.mMode = 0;
            path_check(i_this);
        }
    }
    }
}
VERIFY(0x021CEC44, aite_miru);

/* 021CED78 */
static void fail(mo2_class* i_this) {
    WWHD_FUNC(0x021CED78, void, i_this);
    i_this->m05B4 = 5;
    attention_flags(i_this) = 0;
    i_this->speedF = 0.0f;
    i_this->speed.y = 0.0f;
    if (i_this->mDamageReaction.mMode == 0) {
        gabi::Local<cXyz> local_18;
        *local_18 = i_this->current.pos.get();
        local_18->y += l_mo2HIO().m01C + 100.0f;
        u8 var_r6 = 0;
        if (i_this->m05A4[2] >= 0x3E8) {
            var_r6 = 1;
        }
        fopAcM_createDisappear(i_this, local_18, 10, var_r6, i_this->stealItemBitNo);
        if (fopAcM_CheckStatus(i_this, fopAcStts_BOSS_e)) {
            i_this->m2A1C = 1;
            i_this->mDamageReaction.mMode++;
        } else {
            fopAcM_delete(i_this);
        }
        if (i_this->mDeathSwitch != 0) {
            dComIfGs_onSwitch(i_this->mDeathSwitch, fopAcM_GetRoomNo(i_this));
        }
        fopAcM_onActor(i_this);
    }
}
VERIFY(0x021CED78, fail);

/* 021CEEB0 */
static void yogan_fail(mo2_class* i_this) {
    WWHD_FUNC(0x021CEEB0, void, i_this);
    i_this->m05B4 = 5;
    attention_flags(i_this) = 0;
    i_this->speedF = 0.0f;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_MO2_BCK_AWATEDEMO_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->mDamageReaction.mMode = 1;
        i_this->speed.y = REG0_F(0x13) + 190.0f;
        fopAcM_monsSeStart(i_this, JA_SE_CV_MO_ALERT, 0);
        i_this->mDamageReaction.m458.y = 0.0f;
        i_this->mDamageReaction.m44C.y = 0.0f;
        if (i_this->mbHasInnateWeapon != 0) {
            i_this->mSpawnWeaponActor = 1;
        }
        /* fallthrough */
    case 1:
        dComIfGp_particle_setSimple(0x8061 /* ID_IT_SN_O_FIREK_KASU */, &i_this->current.pos);
        dComIfGp_particle_setSimple(0x8058 /* ID_IT_SN_O_MAGT_FCHIP */, &i_this->current.pos);
        if ((i_this->m059C & 3U) == 0) {
            i_this->m05E8.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
            i_this->m05E8.x = -0x2000;
            dComIfGp_particle_set(0xE /* ID_AK_JN_TUBA00 */, &i_this->m28C8, &i_this->m05E8);
        }
        cLib_addCalcAngleS2(&i_this->current.angle.x, -0x4000, 10, 0x200);
        if (i_this->speed.y < 0.0f) {
            i_this->mDamageReaction.mAction = ACTION_FAIL;
            i_this->mDamageReaction.mMode = 0;
            i_this->m05A4[2] = 2000;
        }
    }
}
VERIFY(0x021CEEB0, yogan_fail);

static inline void back_to_jyunkai(mo2_class* i_this) {
    i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
    path_check(i_this);
    wait_set(i_this);
    i_this->mDamageReaction.mMode = 2;
}

/* 021CF0F4 */
static void wepon_search(mo2_class* i_this) {
    WWHD_FUNC(0x021CF0F4, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* boko = fopAcM_SearchByID(i_this->mWeaponPcId);

    if (i_this->mDamageReaction.mMode < 2 && (boko == nullptr || fopAcM_checkCarryNow(boko))) {
        back_to_jyunkai(i_this);
        return;
    }
    f32 dVar9 = 10000.0f;
    if (boko != nullptr) {
        f32 dx = boko->current.pos.x - i_this->current.pos.x;
        f32 dz = boko->current.pos.z - i_this->current.pos.z;
        i_this->mDamageReaction.m4D0 = cM_atan2s(dx, dz);
        dVar9 = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    }
    switch ((u16)i_this->mDamageReaction.mMode) {
    case (u16)-1:
        i_this->mDamageReaction.mMode = 0;
        i_this->m2943 = 0;
        anm_init(i_this, dRes_INDEX_MO2_BCK_SHAKKEN_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_SHAKKEN_e);
        if (dVar9 < 900.0f) {
            i_this->m05A4[1] = 0x14;
        } else {
            i_this->m05A4[1] = 200;
        }
        i_this->m05B2 = 5;
        tex_anm_set(i_this, 2);
        if (i_this != nullptr) {
            fopAcM_monsSeStart(i_this, JA_SE_CV_MO_FIND_LANCE, 0);
        }
        break;
    case 0:
        i_this->speedF = 0.0f;
        if (i_this->m05B2 == 0) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 2, 0x3000);
            if (i_this->mpMorf->isStop() || i_this->m05A4[1] == 0) {
                i_this->mDamageReaction.mMode = 1;
                i_this->m05AE = l_mo2HIO().m08A;
                anm_init(i_this, dRes_INDEX_MO2_BCK_SWALK_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_SWALK_e);
            }
        }
        break;
    case 1:
        i_this->speedF = l_mo2HIO().m060;
        i_this->m05F0 = l_mo2HIO().m024 + 3;
        i_this->m05F2 = 4;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x1000);
        if (ground_4_check(i_this, 1, i_this->current.angle.y, 300.0f)) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_GAKEDEMO_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mDamageReaction.mMode = 3;
            i_this->m05A4[1] = 0x1e;
            i_this->m2943 = 1;
        } else if (dVar9 < 300.0f) {
            i_this->mDamageReaction.mMode = 2;
            anm_init(i_this, dRes_INDEX_MO2_BCK_SCATCH_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_SCATCH_e);
            i_this->m05A4[1] = 0x1d;
        } else if (i_this->m05AE == 0 || i_this->mDamageReaction.mAcch.ChkWallHit()) {
            i_this->m02E2 = 0x46;
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->m05A4[1] = 0;
            i_this->m2943 = 1;
        }
        break;
    case 2:
        i_this->speedF = 0.0f;
        if (i_this->m05A4[1] == 0x18) {
            if (boko != nullptr && !fopAcM_checkCarryNow(boko)) {
                i_this->mbHasInnateWeapon = 1;
                fopAcM_delete(boko);
                fopAcM_seStart(i_this, JA_SE_CM_LANCE_PICKUP, 0);
            } else {
                back_to_jyunkai(i_this);
            }
        }
        if (i_this->m05A4[1] < 0xe) {
            i_this->mDamageReaction.m710 = 1;
            i_this->mDamageReaction.m4D0 = i_this->m05D6;
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 3, 0x800);
        }
        if (i_this->mpMorf->isStop()) {
            if (l_mo2HIO().m009 == 0 && !daPy_checkGrabWear(player) && i_this->m05C0 < l_mo2HIO().m030) {
                i_this->mDamageReaction.mAction = ACTION_FIGHT;
                i_this->m2060 = 2;
                i_this->m2068 = REG0_F(8) + 33.0f;
                i_this->m206C = 36.0f;
                i_this->m2070 = 52.0f;
                i_this->m2074 = 75.0f;
                i_this->m2078 = (f32)l_mo2HIO().m0D0;
                i_this->m207E = 1;
                i_this->m2064 = 1;
                attack_info_s* info = attack_info(2);
                info++;
                anm_init(i_this, info->bckFileIdx, 5.0f, J3DFrameCtrl::EMode_NONE, info->speed, info->soundFileIdx);
                tex_anm_set(i_this, 6);
                i_this->mDamageReaction.mMode = 1;
                i_this->m2942 = 1;
            } else {
                back_to_jyunkai(i_this);
            }
        }
        break;
    case 3:
        if (dVar9 < 300.0f) {
            i_this->mDamageReaction.mMode = 2;
            anm_init(i_this, 0x6c, 2.0f, 0, 1.0f, 0x33);
            if (i_this != nullptr) {
                fopAcM_seStart(i_this, JA_SE_CM_LANCE_PICKUP, 0);
            }
            i_this->m05A4[1] = 0x1d;
            i_this->speedF = 0.0f;
        } else {
            cLib_addCalc0(&i_this->speedF, 1.0f, REG0_F(8) + 7.0f);
            if (i_this->speedF > 1.0f) {
                i_this->m05F0 = l_mo2HIO().m024 + 4;
                i_this->m05F2 = 4;
            }
            if (i_this->m05A4[1] == 0) {
                back_to_jyunkai(i_this);
            }
        }
    }
}
VERIFY(0x021CF0F4, wepon_search);

/* 021CF900 */
static void hip_damage(mo2_class* i_this) {
    WWHD_FUNC(0x021CF900, void, i_this);
    dComIfGp_getPlayer(0);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_HIPDMG01_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_HIPDMG01_e);
        i_this->speedF = REG_F(6, 0) + 100.0f;
        i_this->m05A4[0] = 0xF;
        i_this->m05F0 = l_mo2HIO().m024 + 3;
        i_this->m05F2 = 4;
        /* fallthrough */
    case 1:
        cLib_addCalc2(&i_this->speedF, 0.0f, 1.0f, REG_F(6, 1) + 10.0f);
        if (i_this->m05A4[0] == 0) {
            i_this->mDamageReaction.mMode = 2;
            anm_init(i_this, dRes_INDEX_MO2_BCK_HIPDMG02_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_HIPDMG02_e);
            i_this->m05A4[0] = 0x3c;
            i_this->mDamageReaction.m4D0 = i_this->current.angle.y + 0x8000;
        }
        break;
    case 2:
        if ((i_this->m059C & 7) == 0) {
            i_this->m05E8.y = (s16)gabi::ftoi(cM_rndF(65536.0f));
            i_this->m05E8.x = -0x2000;
            dComIfGp_particle_set(0xE /* ID_AK_JN_TUBA00 */, &i_this->m28C8, &i_this->m05E8);
        }
        i_this->speedF = REG_F(6, 2) + 50.0f;
        if (i_this->m05A4[0] < REG_S(6, 1) + 0x28) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x500);
            if (i_this->m05A4[0] == 0) {
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
                i_this->m05B4 = 0;
                i_this->m05A4[1] = 0;
                break;
            }
        }
    }
}
VERIFY(0x021CF900, hip_damage);

/* 021CFBBC */
static void d_mahi(mo2_class* i_this) {
    WWHD_FUNC(0x021CFBBC, void, i_this);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_KOKERUF_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_KOKERUF_e);
        i_this->m05A4[0] = 100;
        /* fallthrough */
    case 1:
        if (i_this->m05A4[0] <= 0x50 && i_this->m05A4[0] >= 0x28) {
            if (i_this->m05A4[0] == 0x50) {
                enemy_piyo_set(i_this);
            }
            fopAcM_seStart(i_this, JA_SE_CM_MD_PIYO, 0);
        }
        i_this->speedF = 0.0f;
        if (i_this->m05A4[0] == 0x1E) {
            i_this->mDamageReaction.m49E = 0xF;
        }
        if (i_this->m05A4[0] == 0) {
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            wait_set(i_this);
            i_this->mDamageReaction.mMode = 2;
        }
    }
    /* HD: in Hyrule, keep a paralysed Moblin out of the wall behind it */
    if (i_this->m05A4[0] > 0x50 && SafeString_eq(STR(0x10014D8C) /* "Hyrule" */, dComIfGp_getStartStageName(), SAFESTRING_VTBL)) {
        gabi::Local<dBgS_LinChk_l> linChk;
        dBgS_LinChk_ct(linChk, LINCHK_VTBLS);
        gabi::Local<cXyz> sp18;
        *sp18 = i_this->current.pos.get();
        sp18->y += 20.0f;
        cMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
        gabi::Local<cXyz> sp24;
        sp24->x = 0.0f;
        sp24->y = 20.0f;
        sp24->z = REG_F(18, 8) + -50.0f;
        gabi::Local<cXyz> sp30;
        MtxPosition(sp24, sp30);
        gabi::Local<cXyz> sp3C;
        cXyz_pl(&i_this->current.pos, sp3C, sp30);
        gabi::Local<cXyz> sp48;
        sp48->copy(*sp3C);
        dBgS_LinChk_Set(linChk, sp18, sp48, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            i_this->current.pos.x = gabi::fnmsubs(sp30->x, REG_F(18, 10) + 1.0f, i_this->current.pos.x);
            i_this->current.pos.z = gabi::fnmsubs(sp30->z, REG_F(18, 10) + 1.0f, i_this->current.pos.z);
            hd_debug_print(100, 0x19A, STR(0x10014EB8) /* "MO2 BACK KABE HIT2!" */);
        }
        dBgS_LinChk_dt(linChk, LINCHK_VTBLS);
    }
}
VERIFY(0x021CFBBC, d_mahi);

/* 021CFFA4 */
static void d_sit(mo2_class* i_this) {
    WWHD_FUNC(0x021CFFA4, void, i_this);
    i_this->m05B4 = 5;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_KOKERUF_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_KOKERUF_e);
        i_this->m05A4[0] = 0x32;
        /* fallthrough */
    case 1:
        i_this->speedF = 0.0f;
        if (i_this->m05A4[0] == 0) {
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            wait_set(i_this);
            i_this->mDamageReaction.mMode = 2;
        }
    }
}
VERIFY(0x021CFFA4, d_sit);

/* 021D0064 */
static void d_dozou(mo2_class* i_this) {
    WWHD_FUNC(0x021D0064, void, i_this);
    i_this->m05B4 = 5;
    i_this->m02E0 = 2;
    attention_flags(i_this) = 0;
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        i_this->mDamageReaction.mStts.Init(0xFF, 0xFF, i_this);
        if (i_this->mFrozenInTimePose == 0) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_TACKLEDEMO_e, 1.0f, J3DFrameCtrl::EMode_NONE, 0.0f, -1);
            i_this->mpMorf->setFrame(3.0f);
        } else {
            anm_init(i_this, dRes_INDEX_MO2_BCK_WALK_e, 1.0f, J3DFrameCtrl::EMode_NONE, 0.0f, dRes_INDEX_MO2_BAS_WALK_e);
            i_this->mpMorf->setFrame(37.0f);
        }
        /* fallthrough */
    case 1:
        if ((i_this->mEnableSpawnSwitch != 0xFF && dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, fopAcM_GetRoomNo(i_this))) ||
            (i_this->mEnableSpawnSwitch == 0xFF && dComIfGs_isEventBit(COLORS_IN_HYRULE))) {
            i_this->mpMorf->setPlaySpeed(1.0f);
            i_this->mDamageReaction.mMode = 2;
        }
        break;
    case 2:
        if (i_this->mpMorf->isStop()) {
            i_this->mDamageReaction.mStts.Init(200, 0xFF, i_this);
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            i_this->mDamageReaction.mMode = 0;
            attention_flags(i_this) = fopAc_Attn_LOCKON_BATTLE_e;
            break;
        }
    }
}
VERIFY(0x021D0064, d_dozou);

/* 021D0284 carry (inlined in GameCube; out of line in WWHD, not named by the matcher) */
static void carry(mo2_class* i_this) {
    WWHD_FUNC(0x021D0284, void, i_this);
    i_this->speed.y = 0.0f;
}
VERIFY(0x021D0284, carry);

/* 021D0294 */
static void carry_drop(mo2_class* i_this) {
    WWHD_FUNC(0x021D0294, void, i_this);
    cLib_addCalc0(&i_this->mDamageReaction.m468, 1.0f, 5.5f);
    cLib_addCalc0(&i_this->mDamageReaction.m46C, 1.0f, 0.5f);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x100);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 1, 0x100);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->m05D6, 4, 0x800);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_MO2_BCK_JSTART_e, 10.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_JSTART_e);
        /* fallthrough */
    case 1:
        if (i_this->mDamageReaction.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_JEND_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_JEND_e);
            i_this->mDamageReaction.mMode = 2;
            i_this->m05F0 = l_mo2HIO().m024 + 6;
            i_this->m05F2 = 3;
            i_this->mDamageReaction.mMaxFallDistance = 300.0f;
            fopAcM_monsSeStart(i_this, JA_SE_CV_MO_JAB, 0);
            fopAcM_seStart(i_this, JA_SE_CM_MO_BB_LANDING, 0);
        }
        break;
    case 2:
        cLib_addCalc0(&i_this->mDamageReaction.m468, 1.0f, 50.0f);
        cLib_addCalc0(&i_this->mDamageReaction.m46C, 1.0f, 50.0f);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x1000);
        cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 1, 0x1000);
        cLib_addCalc0(&i_this->speedF, 1.0f, 1.0f);
        if (i_this->mpMorf->isStop()) {
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            i_this->mDamageReaction.mMode = 0;
            i_this->current.angle.x = i_this->shape_angle.x;
            i_this->current.angle.y = i_this->shape_angle.y;
            i_this->current.angle.z = i_this->shape_angle.z;
            break;
        }
    }
}
VERIFY(0x021D0294, carry_drop);

/* 021D0570 */
static void e3_demo(mo2_class* i_this) {
    WWHD_FUNC(0x021D0570, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u8* camera = dComIfGp_getPlayerCamera0();
    i_this->m05B4 = 5;
    i_this->mDamageReaction.m4D0 = fopAcM_searchPlayerAngleY(i_this);
    dCamera_SetTrimSize(camera + 0x248, 2);
    switch ((u16)i_this->mDamageReaction.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_MO2_BCK_UKYADEMO_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m05A4[1] = 0x19;
        i_this->mDamageReaction.mMode = 1;
        dMeter_mtrHide();
        i_this->m2A4C = 200;
        /* fallthrough */
    case 1:
        if (i_this->m05A4[1] == 0) {
            anm_init(i_this, dRes_INDEX_MO2_BCK_SWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_SWALK_e);
            i_this->mDamageReaction.mMode = 2;
            if (alerm_set() == 0) {
                mDoAud_seStart_1(JA_SE_MAJUTOU_ALERM);
                alerm_set()++;
            }
        }
        i_this->speedF = 0.0f;
        break;
    case 2:
        cLib_addCalc0(&i_this->m2A40, 0.1f, 3.0f);
        cLib_addCalc2(&i_this->speedF, l_mo2HIO().m058, 1.0f, 20.0f);
        if (i_this->m05C0 < l_mo2HIO().m030 || ground_4_check(i_this, 1, i_this->current.angle.y, 100.0f)) {
            i_this->mDamageReaction.mMode = 3;
            anm_init(i_this, dRes_INDEX_MO2_BCK_KNAGE_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_KNAGE_e);
        }
        break;
    case 3: {
        cLib_addCalc0(&i_this->speedF, 1.0f, 20.0f);
        s32 frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (frame == REG0_S(6) + 0xF) {
            i_this->m2A09 = 2;
        }
        if (i_this->mpMorf->isStop()) {
            i_this->mDamageReaction.mMode = 4;
            anm_init(i_this, dRes_INDEX_MO2_BCK_SWALK_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_MO2_BAS_SWALK_e);
        }
        break;
    }
    case 4:
        cLib_addCalc2(&i_this->speedF, l_mo2HIO().m058, 1.0f, 20.0f);
        if (fopAcM_searchPlayerDistance(i_this) < 250.0f || ground_4_check(i_this, 1, i_this->current.angle.y, 100.0f)) {
            i_this->mDamageReaction.mMode = 5;
            anm_init(i_this, dRes_INDEX_MO2_BCK_WAITDEMO_e, 15.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->m05A4[1] = 0x1E;
            mDoAud_seStop(JA_SE_MAJUTOU_ALERM, 30);
            daPy_changeOriginalDemo(player);
            daPy_changeDemoMode(player, DEMO_HOLDUP_e);
        }
        break;
    case 5:
        i_this->speedF = 0.0f;
        if (i_this->m05A4[1] == 0) {
            i_this->m2A4B = 1;
        }
    }
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mDamageReaction.m4D0, 4, 0x800);
    if (!daPy_checkGrabWear(player)) {
        cLib_addCalcAngleS2(&player->current.angle.y, (s16)(i_this->mDamageReaction.m4D0 + 0x8000), 4, 0x400);
        cLib_addCalcAngleS2(&player->shape_angle.y, (s16)(i_this->mDamageReaction.m4D0 + 0x8000), 4, 0x400);
    }
}
VERIFY(0x021D0570, e3_demo);

/* 021D3DEC: this TU's copy of sead::SafeString::assureTerminationImpl_ (empty for literal strings) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x021D3DEC, void, (u32)0);
}
VERIFY(0x021D3DEC, SafeString_assureTerminationImpl);

/* dBgS_ObjAcch destructor as GHS expands it: this TU's vtables, then dBgS_Acch::~dBgS_Acch */
static inline void dBgS_ObjAcch_dt(dBgS_ObjAcch* p) {
    gabi::store<u32>(gabi::ea(p) + 0x20, 0x10014E2C);
    gabi::store<u32>(gabi::ea(p) + 0x14, 0x10014E3C);
    gabi::call(0x024EFD9C, p, 0);
}

/* 021D3C90 mo2_class deleting destructor (compiler-generated, HD virtual destructor) */
static void mo2_class_dt(mo2_class* i_this, s32 flags) {
    WWHD_FUNC(0x021D3C90, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mEnemyFire.mStts, 2);
        dBgS_ObjAcch_dt(&i_this->mEnemyIce.mBgAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mEnemyIce.mBgAcchCir) + 0x14), 2); /* cM3dGCir::~cM3dGCir */
        dCcD_Cyl_dt(&i_this->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&i_this->mEnemyIce.mStts, 2);
        gabi::call(0x02515AE8, &i_this->m279C, 2);
        gabi::call(0x02515AE8, &i_this->mWeapon2Sph, 2);
        gabi::call(0x02515AE8, &i_this->mWeaponSph, 2);
        gabi::call(0x02515AE8, &i_this->mDefenseSph, 2);
        gabi::call(0x02515AE8, &i_this->mHeadSph, 2);
        dCcD_Cyl_dt(&i_this->mTgCyl, 2);
        dCcD_Cyl_dt(&i_this->mCoCyl, 2);
        gabi::call(0x025E99E0, &i_this->m3Dline, 2); /* mDoExt_3DlineMat0_c::~ */
        dCcD_Stts_dt(&i_this->mDamageReaction.mStts, 2);
        dBgS_ObjAcch_dt(&i_this->mDamageReaction.mAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mDamageReaction.mAcchCir) + 0x14), 2);
        gabi::call(0x025E99E0, &i_this->m0570, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x021D3C90, mo2_class_dt);
