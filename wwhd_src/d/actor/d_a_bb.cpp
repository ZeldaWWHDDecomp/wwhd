/**
 * d_a_bb.cpp (WWHD)
 * Enemy - Kargaroc
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bb.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bb.h"

/* 0205D078 */
BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0205D078, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        /* HD: bounds check on the joint number */
        if (jntNo >= BB_JNT_NUM_e) {
            JUT_ASSERT_fail(STR(0x10007CD4), 0x194, STR(0x10007CCC));
            return TRUE;
        }
        J3DModel_l* model = j3dSys_getModel();
        bb_class* i_this = gabi::at<bb_class>(model->mUserArea);
        s32 idx = callback_check_index(jntNo);
        if (i_this != nullptr) {
            PSMTXCopy(model_getAnmMtx(model, jntNo), calc_mtx());

            if (idx >= 0 && idx <= 10) {
                cMtx_YrotM(calc_mtx(), i_this->unk_C00[idx].y);
                cMtx_XrotM(calc_mtx(), i_this->unk_C00[idx].x);
                cMtx_ZrotM(calc_mtx(), i_this->unk_C00[idx].z);
                mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
            }

            gabi::Local<cXyz> v;
            if (jntNo == BB_JNT_ATAMA_e) {
                v->x = 0.0f;
                v->z = 0.0f;
                v->y = 0.0f;
                MtxPosition(v, &i_this->eyePos);
                cXyz* attn = gabi::at<cXyz>(gabi::ea(i_this) + 0x390); /* attention_info.position */
                f32 ez = i_this->eyePos.z;
                f32 ey = i_this->eyePos.y;
                attn->z = ez;
                f32 ex = i_this->eyePos.x;
                attn->y = ey;
                attn->x = ex;
                attn->y = ey + gabi::fmadds(REG0_F(8), 10.0f, 50.0f);
            } else if (jntNo == BB_JNT_OB_e) {
                v->x = REG_F(10, 1) + 10.0f; /* HD (GameCube REG0_F(10) * 10.0f + 10.0f) */
                v->y = REG0_F(11) * 10.0f;
                v->z = REG0_F(12) * 10.0f;
                MtxPosition(v, &i_this->unk_BD4[jntNo - BB_JNT_OA_e]);
            } else if (jntNo == BB_JNT_OA_e) {
                v->x = 0.0f;
                v->z = 0.0f;
                v->y = 0.0f;
                MtxPosition(v, &i_this->unk_BD4[jntNo - BB_JNT_OA_e]);
            } else if (jntNo == BB_JNT_FOOTL_e || jntNo == BB_JNT_FOOTR_e) {
                v->x = gabi::fmadds(REG0_F(10), 10.0f, 5.0f);
                v->y = REG0_F(11) * 10.0f;
                v->z = REG0_F(12) * 10.0f;
                if (jntNo == BB_JNT_FOOTL_e) {
                    MtxPosition(v, &i_this->unk_A6C[0]);
                } else {
                    MtxPosition(v, &i_this->unk_A6C[1]);
                }
            } else if (jntNo == BB_JNT_KUCHIA_e) {
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->unk_C4E);
                mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
            } else if (jntNo == BB_JNT_KUCHIB_e) {
                cMtx_YrotM(calc_mtx(), i_this->unk_C4E);
                mtx_copy(model_getAnmMtx(model, jntNo), calc_mtx());
            }
            PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x0205D078, nodeCallBack);

/* 0205D4BC */
void tail_control(bb_class* i_this) {
    WWHD_FUNC(0x0205D4BC, void, i_this);
    gabi::Local<cXyz> sp28;
    gabi::Local<cXyz> sp1C;
    f32 x = i_this->unk_BD4[1].x - i_this->unk_BD4[0].x;
    f32 y = i_this->unk_BD4[1].y - i_this->unk_BD4[0].y;
    f32 z = i_this->unk_BD4[1].z - i_this->unk_BD4[0].z;

    i_this->unk_BEE = cM_atan2s(x, z);
    i_this->unk_BEC = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));

    sp28->x = 0.0f;
    sp28->y = 0.0f;
    sp28->z = REG0_F(5) + 5.0f;

    cMtx_YrotS(calc_mtx(), i_this->unk_BEE);
    cMtx_XrotM(calc_mtx(), i_this->unk_BEC);
    MtxPosition(sp28, &i_this->unk_BF4);

    cXyz* AA8 = &i_this->unk_AA8[1];
    csXyz* B20 = &i_this->unk_B20[1];
    cXyz* B5C = &i_this->unk_B5C[1];

    f32 reg2 = REG0_F(2) + 0.8f;
    f32 reg3 = REG0_F(3);
    f32 groundY = dBgS_Acch_GetGroundH(&i_this->mAcch) + 5.0f;
    bool wave = reg3 > 1.0f; /* GHS: tested once before the loop */

    for (s32 i = 1; i < 10; i++, AA8++, B20++, B5C++) {
        f32 tmp = gabi::fnmsubs((f32)(i - 1), REG0_F(4) + 0.1f, 1.0f);
        f32 sx = gabi::fmadds(i_this->unk_BF4.x, tmp, B5C->x);
        f32 sy = gabi::fmadds(i_this->unk_BF4.y, tmp, B5C->y);
        f32 sz = gabi::fmadds(i_this->unk_BF4.z, tmp, B5C->z);

        if (wave) {
            s16 s5 = REG0_S(5);
            s16 s6 = REG0_S(6);
            s16 t = i_this->unk_352;
            sp28->z = 0.0f;
            sp28->x = cM_ssin(t * (s5 + 1200) + i * (s6 + 6000)) * reg3;
            sp28->y = cM_ssin(t * (s5 + 1000) + i * (s6 + 5000)) * reg3;

            cMtx_YrotS(calc_mtx(), i_this->unk_BEE);
            cMtx_XrotM(calc_mtx(), i_this->unk_BEC);
            MtxPosition(sp28, sp1C);

            sx += sp1C->x;
            sy += sp1C->y;
            sz += sp1C->z;
        }

        f32 gY = AA8->y + sy;
        if (gY < groundY) {
            gY = groundY;
        }
        x = (AA8->x - (AA8 - 1)->x) + sx;
        z = (AA8->z - (AA8 - 1)->z) + sz;
        y = gY - (AA8 - 1)->y;

        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));

        (B20 - 1)->y = atan;
        (B20 - 1)->x = atan2;

        sp28->y = 0.0f;
        sp28->x = 0.0f;
        f32 l = 20.0f * gabi::fmadds((f32)i, 0.03f, 0.25f);
        sp28->z = l + l;

        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp28, sp1C);

        B5C->copy(*AA8);

        f32 nx = (AA8 - 1)->x + sp1C->x;
        AA8->x = nx;
        f32 ny = (AA8 - 1)->y + sp1C->y;
        AA8->y = ny;
        f32 nz = (AA8 - 1)->z + sp1C->z;
        AA8->z = nz;

        B5C->x = (nx - B5C->x) * reg2;
        B5C->y = (AA8->y - B5C->y) * reg2;
        B5C->z = (AA8->z - B5C->z) * reg2;
    }
}
VERIFY(0x0205D4BC, tail_control);

/* 0205D964 */
void tex_anm_set(bb_class* i_this, u16 idx) {
    WWHD_FUNC(0x0205D964, void, i_this, idx);
    i_this->unk_2CD = 1;
    i_this->unk_2CE = (u8)bb_tex_max_frame(idx);
    i_this->unk_2CC = 0;

    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x10007D0C), bb_tex_anm_idx(idx), SAFESTRING_VTBL);
    if (btp == nullptr) JUT_ASSERT_fail(STR(0x10007D10), 0x2A6, STR(0x10007D08));

    mDoExt_btpAnm_init(&i_this->mBtpAnm, J3DModel_getModelData(i_this->mpMorf->getModel()), btp, 0, 2, 1.0f, 0, -1, 1, 0);
}
VERIFY(0x0205D964, tex_anm_set);

/* 0205DA3C */
void anm_init(bb_class* i_this, int animFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x0205DA3C, void, i_this, animFileIdx, morf, loopMode, speed, soundFileIdx);
    if (i_this->unk_2DF < 3) {
        if (soundFileIdx >= 0) {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(BB_ARC, animFileIdx, SAFESTRING_VTBL);
            void* bas = dComIfG_getObjectRes(BB_ARC, soundFileIdx, SAFESTRING_VTBL);
            i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, bas);
        } else {
            J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(BB_ARC, animFileIdx, SAFESTRING_VTBL);
            i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
        }
    }
}
VERIFY(0x0205DA3C, anm_init);

/* 0205DB74 */
void* s_a_d_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x0205DB74, void*, ac1, ac2);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);

    if (esa_check_count() < 100 && fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == fpcNm_ESA_e) {
        esa_class* esa1 = (esa_class*)ac1;
        fopAc_ac_c* esa2 = (fopAc_ac_c*)ac2;
        if (esa1->field_0x298 == 0) {
            gabi::Local<cXyz> sp14;
            gabi::Local<cXyz> sp8;
            Vec3f p = esa1->current.pos.get();
            p.y += 10.0f;
            *sp8 = p;
            sp14->copy(esa2->current.pos);

            dBgS_LinChk_Set(linChk, sp14, sp8, esa2);
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                s32 n = esa_check_count();
                esa_check_count() = n + 1;
                esa_info()[n] = esa1;
            }
        }
    }
    dBgS_LinChk_dt(linChk);
    return nullptr;
}
VERIFY(0x0205DB74, s_a_d_sub);

/* 0205DCFC */
esa_class* search_esa(bb_class* i_this) {
    WWHD_FUNC(0x0205DCFC, esa_class*, i_this);
    esa_check_count() = 0;
    fpcM_Search(0x0205DB74 /* s_a_d_sub */, i_this);

    if (esa_check_count() != 0) {
        f32 fDist = 50.0f;
        s32 i = 0;
        while (i < esa_check_count()) {
            esa_class* esa = esa_info()[i];
            f32 x = esa->current.pos.x - i_this->current.pos.x;
            f32 z = esa->current.pos.z - i_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < fDist) {
                esa->field_0x298 = 1;
                return esa;
            }
            i++;
            if (i == esa_check_count()) {
                i = 0;
                fDist += 50.0f;
                if (fDist > 10000.0f) {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}
VERIFY(0x0205DCFC, search_esa);

/* 0205DE54 */
void kuti_open(bb_class* i_this, s16 arg1, u32 sfxId) {
    WWHD_FUNC(0x0205DE54, void, i_this, arg1, sfxId);
    if (i_this->unk_C50 == 0) {
        i_this->unk_C50 = arg1;
        i_this->unk_C52 = arg1 - 3;
        i_this->unk_C54 = sfxId;
    }
}
VERIFY(0x0205DE54, kuti_open);

/* 0205DE74 (unnamed by the matcher) */
BOOL bb_player_bg_check(bb_class* i_this) {
    WWHD_FUNC(0x0205DE74, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp8;

    Vec3f p = player->current.pos.get();
    p.y += 100.0f;
    *sp8 = p;
    sp14->copy(i_this->eyePos);

    dBgS_LinChk_Set(linChk, sp14, sp8, i_this);
    BOOL ret = cBgS_LineCross(dComIfG_Bgsp(), linChk);
    dBgS_LinChk_dt(linChk);
    return ret;
}
VERIFY(0x0205DE74, bb_player_bg_check);

/* 0205DFC0 */
s32 bb_player_view_check(bb_class* i_this) {
    WWHD_FUNC(0x0205DFC0, s32, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 var_r5;
    f32 var_f2;

    if (l_bbHIO().unk_06 != 0) {
        return 1;
    }

    if (i_this->unk_2D9 != 0xFF) {
        if (i_this->unk_33C > (f32)(u8)i_this->unk_2D9 * 100.0f || bb_player_bg_check(i_this)) {
            return 0;
        }
    } else if (i_this->unk_2DD == 4 || i_this->unk_2DD == 7) {
        if (i_this->unk_33C > l_bbHIO().unk_6C || bb_player_bg_check(i_this)) {
            return 0;
        }
    } else if (i_this->unk_33C > l_bbHIO().unk_68 || bb_player_bg_check(i_this)) {
        return 0;
    }

    if (i_this->unk_2DD == 4 || i_this->unk_2DD == 7) {
        var_r5 = l_bbHIO().unk_72;
        var_f2 = l_bbHIO().unk_78;
    } else {
        var_r5 = l_bbHIO().unk_70;
        var_f2 = l_bbHIO().unk_74;
    }

    if (std::fabs(player->current.pos.y - i_this->eyePos.y) < var_f2) {
        s16 tmp = i_this->current.angle.y - i_this->unk_C5C - i_this->unk_336;
        if (tmp < 0) {
            tmp = -tmp;
        }
        if ((u16)tmp < var_r5) {
            return 1;
        }
    }
    return 0;
}
VERIFY(0x0205DFC0, bb_player_view_check);

/* 0205E14C */
void path_check(bb_class* i_this) {
    WWHD_FUNC(0x0205E14C, void, i_this);
    if (i_this->ppd == nullptr) {
        return;
    }

    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> spC;

    Vec3f p = i_this->current.pos.get();
    p.y += 100.0f;
    *sp24 = p;

    dPnt_l* point = i_this->ppd->m_points;
    for (s32 i = 0; i < i_this->ppd->m_num; i++, point++) {
        spC->x = point->m_position.x;
        spC->y = point->m_position.y + 100.0f;
        spC->z = point->m_position.z;

        dBgS_LinChk_Set(linChk, sp24, spC, i_this);
        check_index()[i] = (u8)(cBgS_LineCross(dComIfG_Bgsp(), linChk) ^ 1);
    }

    f32 fDist = 0.0f;
    bool r7 = false;

    for (s32 j = 0; j < 100; j++) {
        point = i_this->ppd->m_points;
        for (s32 i = 0; i < i_this->ppd->m_num; i++, point++) {
            if (check_index()[i] != 0) {
                f32 y = i_this->current.pos.y - point->m_position.y;
                f32 x = i_this->current.pos.x - point->m_position.x;
                f32 z = i_this->current.pos.z - point->m_position.z;
                if (std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < fDist) {
                    i_this->unk_35E = (s8)(i - (u8)i_this->unk_35F);
                    u16 num = i_this->ppd->m_num;
                    if (i_this->unk_35E >= (s8)num) {
                        i_this->unk_35E = (s8)num;
                    } else if (i_this->unk_35E < 0) {
                        i_this->unk_35E = 0;
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
        i_this->unk_35D = 0;
    } else {
        i_this->unk_35D = (s8)(i_this->unk_2DA + 1);
    }
    dBgS_LinChk_dt(linChk);
}
VERIFY(0x0205E14C, path_check);

/* 0205E420. HD: no blob shadow (daBb_shadowDraw is gone); tail_draw inlined, with an extra
 * translation along the segment and mDoExt_modelUpdate */
BOOL daBb_Draw(bb_class* i_this) {
    WWHD_FUNC(0x0205E420, BOOL, i_this);
    if (i_this->unk_2F2 != 0) {
        return TRUE;
    }

    J3DModel* model = i_this->mpMorf->getModel();
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);

    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        dMat_ice_entryDL(i_this->mpMorf, -1, nullptr);
        return TRUE;
    }

    mDoExt_btpAnm_entry_l(&i_this->mBtpAnm, J3DModel_getModelData(model), i_this->unk_2CC);
    i_this->mpMorf->entryDL();
    mDoExt_btpAnm_remove_l(J3DModel_getModelData(model));

    /* tail_draw */
    cXyz* AA8 = &i_this->unk_AA8[0];
    csXyz* B20 = &i_this->unk_B20[0];
    for (s32 i = 0; i < 9; AA8++, B20++, i++) {
        MtxTrans(AA8->x, AA8->y, AA8->z, 0);
        f32 scale = tial_scale(i);
        MtxScale(scale, scale, scale, 1);
        cMtx_YrotM(calc_mtx(), B20->y);
        cMtx_XrotM(calc_mtx(), B20->x);
        MtxTrans(0.0f, 0.0f, REG_F(10, 0) + 25.0f, 1); /* HD */

        J3DModel* tail = i_this->unk_A84[i];
        mtx_copy(gabi::at<Mtx34>(gabi::ea(tail) + 0xC8), calc_mtx()); /* setBaseTRMtx */
        setLightTevColorType(dKy_getEnvlight(), tail, &i_this->tevStr);
        mDoExt_modelUpdate(tail);
    }
    dSnap_RegistFig_l(0xB0 /* DSNAP_TYPE_BB */, i_this, &i_this->eyePos, i_this->shape_angle.y, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0205E420, daBb_Draw);

/* 0205E638 */
void bb_pos_move(bb_class* i_this) {
    WWHD_FUNC(0x0205E638, void, i_this);
    f32 x = i_this->unk_2F4.x - i_this->current.pos.x;
    f32 z = i_this->unk_2F4.z - i_this->current.pos.z;
    f32 y = i_this->unk_2F4.y - i_this->current.pos.y;

    s16 atan = cM_atan2s(x, z);
    f32 sqrt = std_sqrtf(gabi::fmadds(x, x, z * z));
    s16 atan2 = (s16)-cM_atan2s(y, sqrt);

    s16 old_y = i_this->current.angle.y;
    cLib_addCalcAngleS2_l(&i_this->current.angle.y, atan, (s16)(REG0_S(3) + 10),
                          (s16)gabi::ftoi(i_this->unk_310 * i_this->unk_308));
    old_y = (s16)((old_y - i_this->current.angle.y) * 32);

    s16 target = REG0_S(1) + 5500;
    if (old_y > target) {
        old_y = target;
    } else if (old_y < -target) {
        old_y = -target;
    }

    cLib_addCalcAngleS2_l(&i_this->current.angle.z, old_y, (s16)(REG0_S(3) + 10),
                          (s16)gabi::ftoi(i_this->unk_310 * i_this->unk_308 * 0.5f));
    cLib_addCalcAngleS2_l(&i_this->current.angle.x, atan2, (s16)(REG0_S(3) + 10),
                          (s16)gabi::ftoi(i_this->unk_310 * i_this->unk_308));
    cLib_addCalc2_l(&i_this->unk_308, 1.0f, 1.0f, 0.04f);
    cLib_addCalc2_l(&i_this->speedF, i_this->unk_300, 1.0f, i_this->unk_304);

    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = i_this->speedF;

    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    MtxPosition(v, &i_this->speed);

    i_this->current.pos.x = i_this->current.pos.x + i_this->speed.x;
    i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
    i_this->current.pos.z = i_this->current.pos.z + i_this->speed.z;

    if (i_this->unk_324 != 0) {
        i_this->unk_324 = i_this->unk_324 - 1;
        i_this->current.pos.y = i_this->current.pos.y + 5.0f;
    }
}
VERIFY(0x0205E638, bb_pos_move);

/* 0205E8AC */
void bb_ground_pos_move(bb_class* i_this) {
    WWHD_FUNC(0x0205E8AC, void, i_this);
    gabi::Local<cXyz> v;
    gabi::Local<cXyz> v2;

    s16 atan = cM_atan2s(i_this->unk_2F4.x - i_this->current.pos.x, i_this->unk_2F4.z - i_this->current.pos.z);
    cLib_addCalcAngleS2_l(&i_this->current.angle.y, atan, (s16)(REG0_S(3) + 2),
                          (s16)gabi::ftoi(i_this->unk_310 * i_this->unk_308));
    cLib_addCalc2_l(&i_this->unk_308, 1.0f, 1.0f, 0.1f);
    cLib_addCalc2_l(&i_this->speedF, i_this->unk_300, 1.0f, i_this->unk_304);

    v->x = 0.0f;
    v->y = 0.0f;
    v->z = i_this->speedF;

    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    MtxPosition(v, v2);

    f32 sx = v2->x;
    i_this->speed.x = sx;
    f32 sz = v2->z;
    i_this->speed.z = sz;
    f32 sy = i_this->speed.y;

    i_this->current.pos.x = i_this->current.pos.x + sx;
    i_this->current.pos.y = i_this->current.pos.y + sy;
    i_this->current.pos.z = i_this->current.pos.z + sz;

    i_this->speed.y = sy - 3.0f;

    if (dBgS_Acch_ChkGroundHit(&i_this->mAcch)) {
        i_this->speed.y = -0.5f;
    }
}
VERIFY(0x0205E8AC, bb_ground_pos_move);

/* 0205EA08 (fpcNm_NPC_KAM_e 0xC3) */
void* pl_name_check(void* ac, void*) {
    WWHD_FUNC(0x0205EA08, void*, ac, (u32)0);
    if (fopAc_IsActor(ac) && ac != nullptr && fpcM_GetName(ac) == 0xC3) {
        return ac;
    }
    return nullptr;
}
VERIFY(0x0205EA08, pl_name_check);

/* 0206195C */
static BOOL daBb_IsDelete(bb_class*) {
    WWHD_FUNC(0x0206195C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0206195C, daBb_IsDelete);

/* 02061964 */
static BOOL daBb_Delete(bb_class* i_this) {
    WWHD_FUNC(0x02061964, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10007DC8));
    dPa_EcallBack_remove(&i_this->mParticleCallBack);
    enemy_fire_remove(&i_this->mEnemyFire);
    if (i_this->heap != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x02061964, daBb_Delete);

/* 020619D0 */
BOOL useHeapInit(fopAc_ac_c* ac) {
    WWHD_FUNC(0x020619D0, BOOL, ac);
    bb_class* i_this = (bb_class*)ac;
    const char* arc = STR(0x10007DD0); /* "Bb" */

    J3DModelData* bdl = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_BB_BDL_BB_e, SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(arc, dRes_INDEX_BB_BCK_FLY02_e, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(arc, dRes_INDEX_BB_BAS_FLY02_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, bas,
                                            0x80000, 0x37221203);

    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_BB_BDL_BB_TAIL_e, SAFESTRING_VTBL);
    if (modelData == nullptr) JUT_ASSERT_fail(STR(0x10007DD4), 0x11C4, STR(0x10007DE0));

    for (s32 i = 0; i < 9; i++) {
        i_this->unk_A84[i] = mDoExt_J3DModel__create(modelData, 0x80000, 0x33221202);
        if (i_this->unk_A84[i] == nullptr) {
            return FALSE;
        }
    }

    J3DAnmTexPattern* btp = (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, bb_tex_anm_idx(4), SAFESTRING_VTBL);
    if (btp == nullptr) JUT_ASSERT_fail(STR(0x10007DD4), 0x11D9, STR(0x10007DCC));

    if (!mDoExt_btpAnm_init(&i_this->mBtpAnm, J3DModel_getModelData(i_this->mpMorf->getModel()), btp, 0, 2, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x020619D0, useHeapInit);

/* 02061BE4 enemyfire::enemyfire (inline constructor, out of line in this TU) */
static enemyfire_l* enemyfire_ct(enemyfire_l* i_this) {
    WWHD_FUNC(0x02061BE4, enemyfire_l*, i_this);
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
VERIFY(0x02061BE4, enemyfire_ct);

/* 02061C70 bb_class::bb_class (compiler-generated) */
static bb_class* bb_class_ct(bb_class* i_this) {
    WWHD_FUNC(0x02061C70, bb_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bb_class*)operator_new(0x13D8);
        if (i_this == nullptr) return nullptr;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BB_VTBL;
    gabi::call(0x025E7820, &i_this->mBtpAnm); /* mDoExt_btpAnm */
    gabi::call(0x024EFE94, &i_this->mAcchCir); /* dBgS_AcchCir */
    dBgS_ObjAcch_ct_l(&i_this->mAcch);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Sph_ct(&i_this->mHeadAtSph);
    dCcD_Sph_ct(&i_this->mHeadTgSph);
    dCcD_Sph_ct(&i_this->mBodyTgSph);
    dCcD_Sph_ct(&i_this->mBodyCoSph);
    dPa_followEcallBack_ct_l(&i_this->mParticleCallBack, 0, 0);
    dCcD_Stts_ct(&i_this->mEnemyIce.mStts);
    dCcD_Cyl_ct(&i_this->mEnemyIce.mCyl, BB_AAB_VTBL);
    gabi::call(0x024EFE94, &i_this->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct_l(&i_this->mEnemyIce.mBgAcch);
    enemyfire_ct(&i_this->mEnemyFire);
    return i_this;
}
VERIFY(0x02061C70, bb_class_ct);

/* 02061DEC */
static cPhs_State daBb_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02061DEC, cPhs_State, a_this);
    bb_class* i_this = (bb_class*)a_this;

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x10007E04) /* "Bb" */);

    /* fopAcM_ct(a_this, bb_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            bb_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    if (ret == cPhs_COMPLEATE_e) {
        i_this->unk_2D8 = (u8)fopAcM_GetParam(i_this);
        i_this->unk_2DD = i_this->unk_2D8;
        i_this->unk_2D9 = (u8)(fopAcM_GetParam(i_this) >> 8);
        i_this->unk_2DA = (u8)(fopAcM_GetParam(i_this) >> 16);
        i_this->unk_2DB = (u8)(fopAcM_GetParam(i_this) >> 24);
        i_this->unk_2DC = (u8)a_this->home.angle.z;
        a_this->current.angle.z = 0;
        a_this->current.angle.x = 0;

        if (i_this->unk_2DC == 0xFF) {
            i_this->unk_2DC = 0;
        }

        if (dComIfGs_isEventBit(0x1101) && i_this->unk_2DC != 0 &&
            dComIfGs_isSwitch(i_this->unk_2DC, fopAcM_GetRoomNo(a_this))) {
            return cPhs_ERROR_e;
        }

        if (!fopAcM_entrySolidHeap(a_this, 0x020619D0 /* useHeapInit */, 0x9FA0)) {
            return cPhs_ERROR_e;
        }

        tex_anm_set(i_this, 4);
        f32 y = a_this->current.pos.y + (REG0_F(5) + 70.0f);
        a_this->gbaName = 0x1A;
        f32 x = a_this->current.pos.x;
        f32 z = a_this->current.pos.z;
        a_this->home.pos.x = x;
        a_this->home.pos.y = y;
        a_this->current.pos.y = y;
        a_this->home.pos.z = z;

        a_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x10007E04), 0);
        J3DModel* model = i_this->mpMorf->getModel();
        for (u16 i = 0; i < gabi::load<u16>(modelData_getJointTree(gabi::load<u32>(gabi::ea(model) + 0xAC)) + 8); i++) {
            if (i >= BB_JNT_NUM_e) continue; /* HD: bounds check on the table */
            if (callback_check_index(i) >= 0) {
                u32 md2 = gabi::load<u32>(gabi::ea(model) + 0xAC);
                u32 jnts = gabi::load<u32>(md2 + 8);
                u32 nodeP = i < gabi::load<u32>(md2 + 4) ? jnts + i * 0x1C : jnts;
                gabi::store<u32>(nodeP + 8, 0x0205D078 /* nodeCallBack */);
            }
        }

        gabi::store<u32>(gabi::ea(model) + 0xB8, gabi::ea(i_this)); /* setUserArea */

        if (i_this->unk_2DA != 0xFF) {
            i_this->ppd = (dPath_l*)dPath_GetRoomPath(i_this->unk_2DA, fopAcM_GetRoomNo(a_this));
            if (i_this->ppd == nullptr) {
                return cPhs_ERROR_e;
            }
            i_this->unk_35D = (s8)(i_this->unk_2DA + 1);
            i_this->unk_35F = 1;
        }

        if (i_this->unk_2DB != 0xFF) {
            i_this->unk_2F2 = i_this->unk_2DB + 1;
        }

        if (i_this->unk_2DD == 5 || i_this->unk_2DD == 6) {
            i_this->unk_2DF = 1;

            u8* ac = fopAcM_CreateAppend();
            gabi::store<u32>(gabi::ea(ac) + 4, gabi::load<u32>(gabi::ea(&a_this->current.pos.x)));
            gabi::store<u32>(gabi::ea(ac) + 8, gabi::load<u32>(gabi::ea(&a_this->current.pos.y)));
            gabi::store<u32>(gabi::ea(ac) + 0xC, gabi::load<u32>(gabi::ea(&a_this->current.pos.z)));
            gabi::store<u16>(gabi::ea(ac) + 0x10, (u16)a_this->home.angle.x);
            gabi::store<u16>(gabi::ea(ac) + 0x12, (u16)a_this->home.angle.y);
            gabi::store<u16>(gabi::ea(ac) + 0x14, (u16)a_this->home.angle.z);
            gabi::store<s8>(gabi::ea(ac) + 0x21, fopAcM_GetRoomNo(a_this));
            gabi::store<u32>(gabi::ea(ac), (fopAcM_GetParam(i_this) & 0xFF000000) | 0xFFFF05);

            if (i_this->unk_2DD == 5) {
                i_this->unk_2E8 = fpcSCtRq_Request(fpcLy_CurrentLayer(), fpcNm_MO2_e, 0, 0, ac);
                i_this->unk_2EC = fpcNm_MO2_e;
            } else {
                i_this->unk_2E8 = fpcSCtRq_Request(fpcLy_CurrentLayer(), fpcNm_BK_e, 0, 0, ac);
                i_this->unk_2EC = fpcNm_BK_e;
            }
        } else if (i_this->unk_2D8 == 3) {
            i_this->unk_2DD = 3;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
        fopAcM_SetMin(a_this, -200.0f, -200.0f, -200.0f);
        fopAcM_SetMax(a_this, 200.0f, 200.0f, 200.0f);

        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
        i_this->mAcchCir.SetWall(50.0f, 120.0f);

        a_this->health = 2;
        a_this->max_health = 2;

        i_this->mStts.Init(100, 0xFF, a_this);
        i_this->mHeadAtSph.Set(gabi::at<dCcD_SrcSph>(0x101904CC));
        i_this->mHeadTgSph.Set(gabi::at<dCcD_SrcSph>(0x1019050C));
        i_this->mBodyTgSph.Set(gabi::at<dCcD_SrcSph>(0x1019054C));
        i_this->mBodyCoSph.Set(gabi::at<dCcD_SrcSph>(0x1019058C));

        i_this->mHeadAtSph.SetStts(&i_this->mStts);
        i_this->mHeadTgSph.SetStts(&i_this->mStts);
        i_this->mBodyTgSph.SetStts(&i_this->mStts);
        i_this->mBodyCoSph.SetStts(&i_this->mStts);

        i_this->unk_318[3] = (s16)gabi::ftoi(cM_rndF(200.0f) + 300.0f);
        i_this->unk_352 = (s16)gabi::ftoi(cM_rndF(10000.0f));
        i_this->mEnemyIce.mpActor = a_this;
        i_this->mEnemyIce.mWallRadius = REG0_F(4) + 50.0f;
        i_this->mEnemyIce.mYOffset = -2.0f;
        i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
        i_this->mEnemyFire.mpActor = a_this;
        i_this->mEnemyIce.mCylHeight = REG0_F(5) + 80.0f;
        i_this->mEnemyIce.mParticleScale = 1.3f;

        for (int i = 0; i < 10; i++) {
            i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101905F4 + i);       /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101905CC + 4 * i); /* fire_sc */
        }

        a_this->stealItemLeft = 3;
        daBb_Execute(i_this);
    }
    return ret;
}
VERIFY(0x02061DEC, daBb_Create);

/* 02062394 __sinit_d_a_bb_cpp: header statics, then the bbHIO_c constructor of l_bbHIO */
static void __sinit_d_a_bb_cpp() {
    WWHD_FUNC(0x02062394, void, (u32)0);
    sinit_header_statics(0x10461590, 0x10190600);
    bbHIO_c& h = l_bbHIO();
    h.__vtbl = 0x10007C7C;
    h.unk_04 = 0;
    h.unk_05 = 0;
    h.unk_08 = 20.0f;
    h.unk_0C = 2500.0f;
    h.unk_12 = 0x258;
    h.unk_14 = 0x4B0;
    h.unk_16 = 0x1C2;
    h.unk_18 = 0x2EE;
    h.unk_1C = 0.35f;
    h.unk_20 = 0.5f;
    h.unk_24 = 1.5f;
    h.unk_28 = 1.0f;
    h.unk_2C = 0.85f;
    h.unk_38 = 0.85f;
    h.unk_3C = 1.0f;
    h.unk_40 = 30;
    h.unk_44 = 1.0f;
    h.unk_48 = 1.0f;
    h.unk_4C = 1.5f;
    h.unk_50 = 0x44C;
    h.unk_54 = 3.5f;
    h.unk_32 = 20;
    h.unk_30 = 0x2D;
    h.unk_34 = 2.5f;
    h.unk_58 = 45.0f;
    h.unk_5C = 30.0f;
    h.unk_60 = 2;
    h.unk_62 = 2;
    h.unk_64 = 1;
    h.unk_68 = 8000.0f;
    h.unk_74 = 3000.0f;
    h.unk_70 = 0x6D60;
    h.unk_6C = 1500.0f;
    h.unk_78 = 2000.0f;
    h.unk_72 = 0x59D8;
    h.unk_7C = 10000.0f;
}
VERIFY(0x02062394, __sinit_d_a_bb_cpp);

/* 02062584: this TU's out-of-line copy of fopAcM_SearchByID (called by daBb_Execute) */
static fopAc_ac_c* fopAcM_SearchByID_copy(u32 id) {
    WWHD_FUNC(0x02062584, fopAc_ac_c*, id);
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu) {
        return nullptr;
    }
    return fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
}
VERIFY(0x02062584, fopAcM_SearchByID_copy);

/* 020625C0 / 02063474: this TU's copies of the sin/cos table lookups (JMath TSinCosTable,
 * 8 bytes per entry: sin, cos) */
static f32 sinShort_copy(void* table, s16 a) {
    WWHD_FUNC(0x020625C0, f32, table, a);
    return gabi::load<f32>(gabi::ea(table) + ((((s32)(u16)a) >> 3) << 3));
}
VERIFY(0x020625C0, sinShort_copy);
static f32 cosShort_copy(void* table, s16 a) {
    WWHD_FUNC(0x02063474, f32, table, a);
    return gabi::load<f32>(gabi::ea(table) + ((((s32)(u16)a) >> 3) << 3) + 4);
}
VERIFY(0x02063474, cosShort_copy);

/* 020625D4: this TU's sead::SafeString deleting destructor (SafeString vtable 0x10007BC4 +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x020625D4, void, s, flags);
    if (s != nullptr && (flags & 1)) {
        operator_delete(s);
    }
}
VERIFY(0x020625D4, SafeString_dt);

/* 02064CF0: this TU's sead::SafeString::assureTerminationImpl_ (empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x02064CF0, void, (u32)0);
}
VERIFY(0x02064CF0, SafeString_assureTerminationImpl);

/* 02063404 */
void bb_water_check(bb_class* i_this) {
    WWHD_FUNC(0x02063404, void, i_this);
    if (daSea_ChkArea(i_this->unk_2F4.x, i_this->unk_2F4.z)) {
        f32 waveHeight = daSea_calcWave(i_this->unk_2F4.x, i_this->unk_2F4.z);
        waveHeight = waveHeight + REG0_F(0);
        waveHeight = waveHeight + 100.0f;
        if (!(i_this->current.pos.y > waveHeight)) { /* GHS: bgt over the store */
            i_this->current.pos.y = waveHeight;
        }
    }
}
VERIFY(0x02063404, bb_water_check);

/* dBgS_ObjAcch destructor as GHS expands it: this TU's vtables, then dBgS_Acch::~dBgS_Acch */
static inline void dBgS_ObjAcch_dt_l(dBgS_ObjAcch* p) {
    gabi::store<u32>(gabi::ea(p) + 0x20, 0x10007C1C);
    gabi::store<u32>(gabi::ea(p) + 0x14, 0x10007C2C);
    gabi::call(0x024EFD9C, p, 0);
}

/* 02064BD0 bb_class deleting destructor (compiler-generated, HD virtual destructor) */
static void bb_class_dt(bb_class* i_this, s32 flags) {
    WWHD_FUNC(0x02064BD0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mEnemyFire.mStts, 2);
        dBgS_ObjAcch_dt_l(&i_this->mEnemyIce.mBgAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mEnemyIce.mBgAcchCir) + 0x14), 2); /* cM3dGCir::~cM3dGCir */
        dCcD_Cyl_dt(&i_this->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&i_this->mEnemyIce.mStts, 2);
        gabi::call(0x02515AE8, &i_this->mBodyCoSph, 2);
        gabi::call(0x02515AE8, &i_this->mBodyTgSph, 2);
        gabi::call(0x02515AE8, &i_this->mHeadTgSph, 2);
        gabi::call(0x02515AE8, &i_this->mHeadAtSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        dBgS_ObjAcch_dt_l(&i_this->mAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mAcchCir) + 0x14), 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x02064BD0, bb_class_dt);

/* ---- damage_check ---- */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);
static inline void at_power_check(CcAtInfo_l* i) { gabi::call(0x02518DB0, i); }
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* i) { return gabi::call<u32>(0x025192A8, a, i); }
static inline void dKy_Sound_set(cXyz* pos, s32 p, u32 pid, s32 t) { gabi::call(0x0255F458, pos, p, pid, t); }
/* 025A5AC8 dPa_followEcallBack::end (HD: called directly here instead of the virtual remove) */
static inline void dPa_followEcallBack_end_l(dPa_followEcallBack_l* cb) { gabi::call(0x025A5AC8, cb); }
static inline JPABaseEmitter* particle_set_l(u8 grp, u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                             void* cb, s8 setup, const void* prm, const void* env) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, grp, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, (const GXColor*)prm,
                           (const GXColor*)env, nullptr);
}
enum { AT_TYPE_LIGHT_ICE_ARROW = 0x180000, AT_TYPE_ICE_ARROW = 0x80000, AT_TYPE_FIRE_ARROW_FIRE = 0x40200 };
enum { JA_SE_CV_BB_DAMAGE = 0x4816, JA_SE_CV_BB_FAINTED = 0x4817 };

/* 0206432C */
void damage_check(bb_class* i_this) {
    WWHD_FUNC(0x0206432C, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> scale;
    gabi::Local<CcAtInfo_l> sp30;

    i_this->mStts.Move();

    bool bodyHit = i_this->mBodyTgSph.ChkTgHit() != 0;
    if ((bodyHit || i_this->mHeadTgSph.ChkTgHit()) && i_this->unk_340 == 0) {
        i_this->unk_340 = 5;
        tex_anm_set(i_this, 2);

        if (bodyHit) {
            sp30->mpObj = gabi::ea(i_this->mBodyTgSph.GetTgHitObj());
            sp30->pParticlePos = i_this->mBodyTgSph.GetTgHitPosP();
        } else {
            sp30->mpObj = gabi::ea(i_this->mHeadTgSph.GetTgHitObj());
            sp30->pParticlePos = i_this->mHeadTgSph.GetTgHitPosP();
        }

        u32 atType = gabi::load<u32>(sp30->mpObj + 0x10);
        if (atType & AT_TYPE_LIGHT_ICE_ARROW) {
            if (gabi::load<u32>(sp30->mpObj + 0x10) & AT_TYPE_ICE_ARROW) {
                i_this->mEnemyIce.mFreezeDuration = REG0_S(3) + 300;
                i_this->unk_2DD = 3;
                i_this->unk_2F1 = 0;
                anm_init(i_this, dRes_INDEX_BB_BCK_DAMAGEP_e, 0.0f, 0, 1.0f, -1);
            } else {
                i_this->mEnemyIce.mLightShrinkTimer = 1;
            }
            enemy_fire_remove(&i_this->mEnemyFire);
            return;
        }

        if (atType & AT_TYPE_FIRE_ARROW_FIRE) {
            i_this->mEnemyFire.mFireDuration = REG0_S(2) + 100;
            i_this->unk_340 = 50;
        }

        s8 old_health = i_this->health;
        at_power_check(sp30);
        if (sp30->mResultingAttackType == 14) {
            i_this->health = 20;
        }
        sp30->mpActor = cc_at_check(i_this, sp30);

        if (sp30->mResultingAttackType == 14) {
            i_this->health = old_health;
            particle_set_l(0, 0x27B /* ID_IT_JN_PIYOHIT00 */, sp30->pParticlePos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr);
        } else if (i_this->health <= 0) {
            particle_set_l(0, 0x10 /* ID_AK_JN_CRITICALHITFLASH */, sp30->pParticlePos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr);
            scale->x = 2.0f;
            scale->y = 2.0f;
            scale->z = 2.0f;
            particle_set_l(0, 0xF /* ID_AK_JN_CRITICALHIT */, sp30->pParticlePos, &player->shape_angle, scale, 0xFF, nullptr, -1, nullptr, nullptr);
        } else {
            particle_set_l(0, 0xD /* ID_AK_JN_OK */, sp30->pParticlePos, &player->shape_angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr);
        }

        gabi::Local<cXyz> pos;
        Vec3f p = i_this->current.pos.get();
        *pos = p;
        dKy_Sound_set(pos, 100, fopAcM_GetID(i_this), 5);

        if (l_bbHIO().unk_04 != 0) {
            i_this->health = 10;
        }

        if (sp30->mbDead) {
            if (i_this->health <= 0) {
                i_this->unk_C7E = 1;
                i_this->unk_2F1 = 0;
                fopAcM_monsSeStart(i_this, JA_SE_CV_BB_FAINTED, 0);
            } else {
                fopAcM_monsSeStart(i_this, JA_SE_CV_BB_DAMAGE, 0);
            }
            i_this->unk_34C = gabi::fmadds(REG0_F(13), 100.0f, 5000.0f);
        } else {
            i_this->unk_C58 = gabi::fmadds(REG0_F(9), 200.0f, 10000.0f);
            i_this->unk_34C = gabi::fmadds(REG0_F(13), 100.0f, 3000.0f);
            fopAcM_monsSeStart(i_this, JA_SE_CV_BB_DAMAGE, 0);
        }

        u8* k0 = gabi::at<u8>(gabi::ea(i_this) + 0x1A8); /* tevStr.mColorK0 */
        particle_set_l(2, 0x438 /* ID_IT_JN_KG_HANE_A */, &i_this->current.pos, &i_this->current.angle, nullptr, 0xFF, nullptr,
                       fopAcM_GetRoomNo(i_this), k0, k0);
        dPa_followEcallBack_end_l(&i_this->mParticleCallBack);
        JPABaseEmitter* emitter = particle_set_l(2, 0x439 /* ID_IT_JN_KG_HANE_B */, &i_this->current.pos, &i_this->current.angle,
                                                 nullptr, 0xFF, &i_this->mParticleCallBack, fopAcM_GetRoomNo(i_this), nullptr, nullptr);
        if (emitter != nullptr) {
            u32 e = gabi::ea(emitter);
            if (sp30->mbDead) {
                gabi::store<s32>(e + 0x5C, REG0_S(7) + 20); /* setMaxFrame */
            } else {
                gabi::store<s32>(e + 0x5C, REG0_S(8) + 6);
            }
            /* setGlobalPrmColor / setGlobalEnvColor */
            gabi::store<u8>(e + 0x244, gabi::load<u8>(gabi::ea(k0) + 0));
            gabi::store<u8>(e + 0x245, gabi::load<u8>(gabi::ea(k0) + 1));
            gabi::store<u8>(e + 0x246, gabi::load<u8>(gabi::ea(k0) + 2));
            gabi::store<u8>(e + 0x248, gabi::load<u8>(gabi::ea(k0) + 0));
            gabi::store<u8>(e + 0x249, gabi::load<u8>(gabi::ea(k0) + 1));
            gabi::store<u8>(e + 0x24A, gabi::load<u8>(gabi::ea(k0) + 2));
        }

        if (sp30->mResultingAttackType == 1) {
            i_this->unk_342 = i_this->unk_336;
            i_this->unk_344 = i_this->unk_338;
        } else {
            i_this->unk_342 = sp30->m0C.y;
            i_this->unk_344 = 0;
        }

        if (i_this->unk_2DD != 3) {
            i_this->unk_2DD = 3;
            f32 r = cM_rndF((f32)(l_bbHIO().unk_18 - l_bbHIO().unk_16));
            i_this->unk_35D = 0;
            i_this->unk_318[1] = (s16)gabi::ftoi(r + (f32)l_bbHIO().unk_16);
        }

        if (sp30->mbDead) {
            i_this->unk_348 = l_bbHIO().unk_58;
            if (cM_rndF(1.0f) < 0.5f) {
                i_this->unk_350 = (s16)gabi::ftoi(cM_rndF(3000.0f) + 5000.0f);
            } else {
                i_this->unk_350 = (s16)gabi::ftoi(-(cM_rndF(3000.0f) + 5000.0f));
            }
        } else {
            i_this->unk_348 = l_bbHIO().unk_5C;
            i_this->unk_350 = 0;
        }

        i_this->unk_C7C = 1;
        anm_init(i_this, dRes_INDEX_BB_BCK_DAMAGEP_e, 0.0f, 0, 1.0f, -1);
    }
}
VERIFY(0x0206432C, damage_check);
