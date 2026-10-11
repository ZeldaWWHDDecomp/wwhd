/**
 * d_a_kui.cpp (WWHD)
 * Object - Hookable posts for the Grappling Hook
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kui.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * search_dragontail / search_btd and demo_camera are inlined into daKui_Execute.
 */
#include <cmath>

#include "d/actor/d_a_kui.h"

// HD: compiler data belongs to this translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x100138E4,
    .headerStatics = {0x10464CAC, 0x101B8A54, 0x10464CB8},
};
static const Mtx34 mtx_adj = {
    {{0.5f, 0.0f, 0.0f, 0.5f}, {0.0f, -0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 0.0f}}};
static const s16 bure_xa_d[] = {0x1000, -0x1000};

/* 021AC660 */
static void* s_a_i_sub(void* search, void* user) {
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_DR2_e)
        return search;
    return nullptr;
}

/* 021AC6B0 */
static void* b_a_i_sub(void* search, void* user) {
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BTD_e)
        return search;
    return nullptr;
}

/* 021AC700 */
// HD: HD: takes the J3DModel; the effect matrix is written to the model (+0xF8) and every *
// material instance's texture-matrix slots of type 10/11 point to it (marking them dirty)
static void setEffectMtx(fopAc_ac_c* a_this, J3DModel* model, f32 scale) {
    cXyz* eyePos = &a_this->eyePos;
    KuiCameraView* camera = dCam_getCamera();
    gabi::Local<cXyz> look_dir, light_dir, refl;
    cXyz_mi(eyePos, look_dir, &camera->mEye);
    KuiModelView* hdModel = reinterpret_cast<KuiModelView*>(model);
    dKyr_get_vectle_calc(&a_this->tevStr.mLightPosWorld, eyePos, light_dir.get());
    C_VECHalfAngle(look_dir.get(), light_dir.get(), refl.get());
    gabi::Local<Mtx34> reflMtx;
    C_MTXLookAt(reflMtx.get(), &cXyz::Zero, &kui_baseY, refl.get());

    Mtx34* now = mDoMtx_stack_c::get();
    PSMTXScale(now, scale, scale, 1.0f);
    PSMTXConcat(now, &mtx_adj, now);
    PSMTXConcat(now, reflMtx, now);
    now->m[0][3] = 0.0f;
    now->m[2][3] = 0.0f;
    now->m[1][3] = 0.0f;

    Mtx34* effMtx = &hdModel->mEffectMtx;
    PSMTXCopy(now, effMtx);
    u16 n = hdModel->mMaterialNum;
    for (u16 i = 0; i < n; i++) {
        KuiMaterialInstance* mat = &hdModel->mMaterials.get()[i];
        for (u32 j = 0; j < 8; j++) {
            gabi::Local<be<s32>> slot;
            *slot = -1;
            KuiTextureInfo* texMtx = mat->getTexMtx(j, slot.get());
            if (texMtx != nullptr) {
                u32 info = texMtx->mInfo;
                if (info >= 10 && info <= 11) {
                    KuiTextureEntry* tbl = mat->mpData->textureTable();
                    s32 idx = *slot;
                    KuiTextureEntry* entry = &tbl[idx];
                    if (entry->mIndex >= 0) {
                        mat->markDirty(idx);
                    }
                    // HD: fetch the relative table again after the dirty-bit writes.
                    KuiTextureEntry* tbl2 = mat->mpData->textureTable();
                    u32 idx2 = entry->mParentIndex;
                    if (tbl2[idx2].mIndex >= 0) {
                        mat->markDirty(idx2);
                    }
                    gptr<Mtx34>* effect = mat->getEffectSlot(j);
                    if (effect != nullptr) {
                        *effect = effMtx;
                    }
                }
            }
        }
    }
}

/* 021AC94C */
static BOOL daKui_Draw(kui_class* i_this) {
    if (i_this->type == 3) {
        u32 light_type = 0;
        if (REG0_S(0) != 0) {
            light_type = 1;
        }

        f32 x = i_this->current.pos.x, y = i_this->current.pos.y;
        i_this->eyePos.x = x;
        i_this->eyePos.y = y;
        i_this->eyePos.z = i_this->current.pos.z;
        i_this->eyePos.y = y + REG0_F(10);

        settingTevStruct(dKy_getEnvlight(), light_type, &i_this->eyePos, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel2, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);

        /* HD: setEffectMtx takes the model */
        setEffectMtx(i_this, i_this->mpModel2, REG0_F(11) + 1.0f);
        setEffectMtx(i_this, i_this->mpModel, REG0_F(12) + 1.0f);

        mDoExt_modelUpdateDL(i_this->mpModel2);
        mDoExt_modelUpdateDL(i_this->mpModel);
    } else if (i_this->type != 1) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), i_this->mpModel2, &i_this->tevStr);

        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(i_this->mpModel2);
        dComIfGd_setList();

        if (i_this->mpModel) {
            setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
            mDoExt_modelUpdateDL(i_this->mpModel);
        }
    }
    return TRUE;
}

/* inlined into daKui_Execute */
static inline void demo_camera(kui_class* i_this) {
    s32 camId = dComIfGp_getPlayerCameraID(0);
    KuiCameraProcessView* camera =
        reinterpret_cast<KuiCameraProcessView*>(dComIfGp_getCamera(camId));
    KuiCameraControl* dcam = &camera->mCamera;
    s8 bVar2 = true;

    s8 st = i_this->field_0x2E8;
    switch (st) {
    case 0:
        break;
    case 1:
        if (!i_this->eventInfo.checkCommandDemoAccrpt()) {
            fopAcM_orderPotentialEvent(i_this, dEvtFlag_STAFF_ALL_e, 0xFFFF, 0);
            i_this->eventInfo.onCondition(dEvtCnd_UNK2_e);
            bVar2 = false;
            break;
        }
        i_this->field_0x2E8 = (s8)(st + 1);
        dcam->Stop();
        dcam->SetTrimSize(2);
        i_this->field_0x2EA = 0;
        /* fallthrough */
    case 2: {
        i_this->current.pos.y = i_this->home.pos.y + i_this->field_0x30C;

        s32 uVar3 = i_this->field_0x2EA;
        if (uVar3 < 20) {
            f32 sin_result = cM_ssin((uVar3 & 0x1F) * 0x800);
            i_this->field_0x30C = sin_result * 5.0f;
        } else if (uVar3 <= 27) {
            if (uVar3 == 27) {
                fopAcM_seStartCurrent_nn(i_this, JA_SE_OBJ_ROPE_SW_ON, 0);
                StartShock_up(false);
            }
            cLib_addCalc2(&i_this->field_0x30C, -70.0f, 1.0f, 10.0f);
        } else if (uVar3 < 42) {
            f32 sin_result = cM_ssin(uVar3 * 0x3A00);
            i_this->field_0x30C = gabi::fmadds(sin_result, 5.0f, -70.0f);
        }

        cXyz_fcopy(&i_this->field_0x2F8, &i_this->home.pos);
        i_this->field_0x2F8.y = i_this->field_0x2F8.y + (REG_F(8, 0) + 200.0f);
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);

        gabi::Local<cXyz> vec, posVec, sum;
        vec->x = REG_F(8, 1) + 800.f;
        vec->y = REG_F(8, 2);
        vec->z = REG_F(8, 3) + 100.0f;
        MtxPosition(vec, posVec);

        cXyz_pl(&i_this->home.pos, sum, posVec);
        i_this->field_0x2EC.copy(*sum);

        if (i_this->field_0x2EA == 70) {
            gabi::Local<cXyz> c, e;
            cXyz_fcopy(c, &i_this->field_0x2F8);
            cXyz_fcopy(e, &i_this->field_0x2EC);
            i_this->field_0x2E8 = 0;
            dcam->Reset(c, e);
            dcam->Start();
            dcam->SetTrimSize(0);

            dComIfGp_event_reset();

            dComIfGs_onSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(i_this));
            mDoAud_seStart_id(JA_SE_READ_RIDDLE_1);
        }
        break;
    }
    }

    if ((i_this->field_0x2E8 != 0) && bVar2) {
        gabi::Local<cXyz> c, e;
        cXyz_fcopy(e, &i_this->field_0x2EC);
        cXyz_fcopy(c, &i_this->field_0x2F8);
        dcam->Set(c, e);
        JUTReport(0x19a, 0x1ae, "K SUB  COUNT  %d", i_this->field_0x2EA);
        i_this->field_0x2EA = i_this->field_0x2EA + 1;
    }
}

/* inlined into daKui_Execute */
// Type 3 is the bell, swinging from the grappling hook.
static inline void kui_bell(kui_class* i_this, fopAc_ac_c* player) {
    fopAc_ac_c* actor = i_this;
    s16 target_x_angle;
    if (actor->health == 3) {
        gabi::Local<cXyz> hand, diff, temp2, temp;
        cXyz_fcopy(hand, &static_cast<daPy_py_c*>(player)
                              ->mLeftHandPos); /* daPy_py_c::getLeftHandPos() (HD +0x3F0) */
        cXyz_mi(hand, diff, &actor->home.pos);
        temp2->copy(*diff);
        cMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);

        MtxPosition(temp2, temp);
        f32 tz = temp->z * (REG0_F(1) + 1.0f);
        temp->z = tz;

        if (REG0_S(0) == 0) {
            target_x_angle = (s16)-cM_atan2s(tz, -temp->y);
        } else {
            target_x_angle = cM_atan2s(tz, -temp->y);
        }
        s16 unk = (s16)gabi::ftoi(std::fabs(cM_ssin(actor->shape_angle.y) * (REG0_F(2) + 3000.0f)));

        s32 unk_flag = 0;
        if (target_x_angle > unk) {
            target_x_angle = unk;
            unk_flag = 1;
        } else if (target_x_angle < (s16)-unk) {
            target_x_angle = (s16)-unk;
            unk_flag = 2;
        }

        /* HD: no `unk > 2000` condition */
        if (unk_flag != 0) {
            if (i_this->field_0x2DC[unk_flag] == 0) {
                i_this->field_0x2DC[unk_flag] = 0x50;
                i_this->field_0x2DC[0] =
                    (s16)(REG_S_(10, 3) + 40); /* HD: REG10_S(3) (GameCube REG0_S(3)) */

                StartShock_up(true);
                fopAcM_seStartCurrent_nn(actor, JA_SE_OBJ_ST_CHIME, 0);

                s16 v = bure_xa_d[unk_flag - 1];
                i_this->field_0x2E4 = v;
                i_this->field_0x2E4 = (s16)(v * (REG_S_(17, 4) + 1));
                i_this->field_0x2E6 = (s16)(REG_S_(17, 5) + 0x400);
            }
        }

        s16 d = (s16)(actor->current.angle.y - player->shape_angle.y);
        if (REG0_S(1) == 0) {
            actor->shape_angle.y = (s16)-d;
        } else {
            actor->shape_angle.y = d;
        }
    } else {
        target_x_angle = 0;
    }

    cLib_addCalcAngleS2(&actor->current.angle.x, target_x_angle, 4, (s16)(REG0_S(1) + 0x200));
    s16 c1 = i_this->field_0x2DC[1];
    s16 c2 = i_this->field_0x2DC[2];
    if (c1 != 0) {
        i_this->field_0x2DC[1] = (s16)(c1 - 1);
    }
    s16 c0 = i_this->field_0x2DC[0];
    if (c2 != 0) {
        i_this->field_0x2DC[2] = (s16)(c2 - 1);
    }

    s16 x = 0;
    s16 z = 0;
    f32 unk_f = 1.0f;

    if (c0 != 0) {
        s16 iVar10 = (s16)(c0 - 1);
        i_this->field_0x2DC[0] = iVar10;

        f32 fv = (f32)iVar10;
        f32 fVar1 = fv * (REG0_F(16) + 40.0f);

        x = (s16)gabi::ftoi(cM_ssin(iVar10 * (REG0_S(4) + 0x1900)) * fVar1);
        z = (s16)gabi::ftoi((cM_scos(iVar10 * (REG0_S(5) + 0x2100)) * fVar1) * 0.25f);

        if ((iVar10 & 1) != 0) {
            unk_f = gabi::fmadds(fv, REG0_F(17) + 0.001f, 1.0f);
        }

        if (iVar10 == 0 && actor->health == 3 && REG0_S(3) == 0) {
            dComIfGs_onSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(actor));
        }
    }

    cLib_addCalcAngleS2(&i_this->field_0x2E2, i_this->field_0x2E4, 4, i_this->field_0x2E6);
    cLib_addCalcAngleS2(&i_this->field_0x2E4, 0, 1, (s16)(REG_S_(17, 6) + 0x80));
    cLib_addCalcAngleS2(&i_this->field_0x2E6, (s16)(REG_S_(17, 7) + 0x100), 1,
                        (s16)(REG_S_(17, 8) + 0x40));

    MtxTrans(actor->home.pos.x, actor->home.pos.y, actor->home.pos.z, FALSE);
    cMtx_YrotM(calc_mtx(), actor->current.angle.y);

    MtxPush();
    cMtx_YrotM(calc_mtx(), actor->shape_angle.y);
    cMtx_XrotM(calc_mtx(), (s16)(x + i_this->field_0x2E2));
    cMtx_ZrotM(calc_mtx(), z);
    cMtx_YrotM(calc_mtx(), (s16)-actor->shape_angle.y);
    MtxScale(unk_f, unk_f, unk_f, TRUE);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());

    MtxPull();
    cMtx_YrotM(calc_mtx(), (s16)(actor->shape_angle.y + REG0_S(5)));
    cMtx_XrotM(calc_mtx(), (s16)(actor->current.angle.x + REG0_S(6)));
    {
        s16 r = REG0_S(5);
        cMtx_YrotM(calc_mtx(), (s16) - (actor->shape_angle.y + r + 0x4000));
    }

    MtxScale(unk_f, 1.0f, unk_f, TRUE);
    J3DModel_setBaseTRMtx(i_this->mpModel2, calc_mtx());
    MtxTrans(0.0f, REG0_F(6) + -850.0f, 0.0f, TRUE);

    gabi::Local<cXyz> zero;
    zero->set(0.0f, 0.0f, 0.0f);
    MtxPosition(zero, &actor->current.pos);

    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, FALSE);
    cMtx_YrotM(calc_mtx(), actor->current.angle.y);
    MtxScale(actor->scale.x, actor->scale.y, actor->scale.z, TRUE);
    PSMTXCopy(calc_mtx(), &i_this->field_0x2A8);
    dBgW_Move(i_this->field_0x2D8);
}

/* 021ACB0C */
static BOOL daKui_Execute(kui_class* i_this) {
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (i_this->field_0x2A2 != 0) {
        KuiDragonTailView* dragon_tail =
            (KuiDragonTailView*)fpcM_Search(gabi::guest_fn((const void*)s_a_i_sub), i_this);
        KuiBtdView* btd = (KuiBtdView*)fpcM_Search(gabi::guest_fn((const void*)b_a_i_sub), i_this);

        if (dragon_tail != nullptr && btd != nullptr) {
            s16 u4BA = dragon_tail->unk_4BA;                    /* dr2_class::unk_4BA */
            if ((u4BA == 0 || u4BA >= 10) && btd->m02E4 < 10) { /* btd_class::m02E4 */
                actor->current.pos.copy(dragon_tail->unk_3B0);  /* dr2_class::unk_3B0 */
                actor->current.angle.x = dragon_tail->current.angle.x;
                actor->current.angle.y = dragon_tail->current.angle.y;
                actor->current.angle.z = dragon_tail->current.angle.z;
            } else {
                actor->current.pos.set(0.0f, -10000.0f, 0.0f);
            }
        } else {
            actor->current.pos.set(0.0f, -10000.0f, 0.0f);
        }
    }

    if (i_this->type == 3) {
        kui_bell(i_this, player);
    } else {
        MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, FALSE);
        cMtx_YrotM(calc_mtx(), actor->current.angle.y);
        if (i_this->type == 2 || i_this->type == 4) {
            J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
        }

        J3DModel_setBaseScale(i_this->mpModel2, &actor->scale);
        J3DModel_setBaseTRMtx(i_this->mpModel2, calc_mtx());
        gabi::Local<Mtx34> local_mtx;
        if (i_this->field_0x2A2) {
            PSMTXScale(local_mtx, 4.0f, 4.0f, 4.0f);
        } else {
            PSMTXScale(local_mtx, actor->scale.x, actor->scale.y, actor->scale.z);
        }

        PSMTXConcat(calc_mtx(), local_mtx, &i_this->field_0x2A8);
        dBgW_Move(i_this->field_0x2D8);
    }

    if (i_this->type == 2 || i_this->type == 4) {
        BOOL is_switch = dComIfGs_isSwitch(i_this->mSwitchNo, dComIfGp_roomControl_getStayNo());

        if (!is_switch) {
            if (actor->health == 3 && i_this->field_0x308 == 0) {
                i_this->field_0x308 = 1000; /* HD: folded with the decrement below (stores 999) */
            }
        } else {
            actor->current.pos.y = actor->home.pos.y - 70.0f;
        }

        if (i_this->field_0x308 != 0) {
            i_this->field_0x308 = (s16)(i_this->field_0x308 - 1);
            s16 finished = (s16)(REG_S_(8, 3) + 970);
            if (i_this->field_0x308 == finished) {
                if (i_this->type == 2) {
                    i_this->field_0x2E8 = 1;
                } else {
                    fopAcM_seStartCurrent_nn(actor, JA_SE_OBJ_ROPE_SW_ON, 0);
                    dComIfGs_onSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(actor));
                    mDoAud_seStart_id(JA_SE_READ_RIDDLE_1);
                }
            }
        }
        demo_camera(i_this);
    }

    actor->eyePos.copy(actor->current.pos);
    return TRUE;
}

/* 021ADAF4 */
static BOOL daKui_IsDelete(kui_class*) { return TRUE; }

/* 021ADAFC */
static BOOL daKui_Delete(kui_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Kui"); /* dComIfG_resDeleteDemo */
    cBgS_Release(dComIfG_Bgsp(), i_this->field_0x2D8);
    return TRUE;
}

/* 021ADB48 */
// HD: assertion text spells NULL as (0).
static BOOL daKui_CreateHeap(fopAc_ac_c* a_this) {
    kui_class* i_this = (kui_class*)a_this;
    J3DModelData* modelData;

    if (i_this->type == 3) {
        /* Bell body */
        modelData = (J3DModelData*)dComIfG_getObjectRes("Kui", dRes_INDEX_KUI_BDL_HKANE1_e);
        JUT_ASSERT(0x391, modelData != NULL);

        i_this->mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (!i_this->mpModel) {
            return FALSE;
        }

        /* Bell handle */
        modelData = (J3DModelData*)dComIfG_getObjectRes("Kui", dRes_INDEX_KUI_BDL_HKANE2_e);
        JUT_ASSERT(0x39D, modelData != NULL);

        i_this->mpModel2 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (!i_this->mpModel2) {
            return FALSE;
        }
    } else {
        /* Wooden post */
        modelData = (J3DModelData*)dComIfG_getObjectRes("Kui", dRes_INDEX_KUI_BDL_OBI_ROPETAG_e);
        JUT_ASSERT(0x3A9, modelData != NULL);

        i_this->mpModel2 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
        if (!i_this->mpModel2) {
            return FALSE;
        }

        if (i_this->type == 2 || i_this->type == 4) {
            /* Device that holds the post */
            modelData = (J3DModelData*)dComIfG_getObjectRes("Kui", dRes_INDEX_KUI_BDL_MROPESW_e);
            JUT_ASSERT(0x3B5, modelData != NULL);

            i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
            if (!i_this->mpModel) {
                return FALSE;
            }
        }
    }

    dBgW* bgw = new_dBgW();
    i_this->field_0x2D8 = bgw;
    if (!i_this->field_0x2D8) {
        return FALSE;
    }

    cBgD_t* pData = (cBgD_t*)dComIfG_getObjectRes("Kui", dRes_INDEX_KUI_DZB_OBI_ROPETAG_e);
    if (cBgW_Set(i_this->field_0x2D8, pData, cBgW_MOVE_BG_e, &i_this->field_0x2A8) == true) {
        return FALSE;
    }

    dBgW_SetCrrFunc(i_this->field_0x2D8, 0x024EE658 /* dBgS_MoveBGProc_Typical */);

    return TRUE;
}

/* 021ADD6C */
static cPhs_State daKui_Create(fopAc_ac_c* a_this) {
    kui_class* i_this = (kui_class*)a_this;
    cPhs_State result;

    /* fopAcM_ct(a_this, kui_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            i_this->__vtbl = KUI_VTBL;
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    result = dComIfG_resLoad(&i_this->mPhs, "Kui");
    if (result == cPhs_COMPLEATE_e) {
        dComIfGp_get(); /* HD: result unused */

        if (fopAcM_GetParam(a_this) == 0xFFFFFFFF) {
            return cPhs_ERROR_e;
        }

        i_this->type = fopAcM_GetParam(a_this) & 0xF;
        i_this->field_0x2A2 = fopAcM_GetParam(a_this) & 0xF0;
        i_this->field_0x2A1 = (u8)(fopAcM_GetParam(a_this) >> 8);
        u8 sw = (u8)(fopAcM_GetParam(a_this) >> 0x18);

        if (sw == 0xFF) {
            i_this->mSwitchNo = 0;
        } else {
            i_this->mSwitchNo = sw;
        }
        if (i_this->type == 3) {
            i_this->field_0x2A1 = 4;
        }

        switch (i_this->field_0x2A1) {
        case 0:
            i_this->scale.x = 0.5f;
            i_this->scale.y = 0.5f;
            break;
        case 1:
            i_this->scale.x = 2.0f;
            i_this->scale.y = 2.0f;
            break;
        case 2:
            i_this->scale.z = 2.0f;
            break;
        case 3:
            i_this->scale.x = 0.5f;
            i_this->scale.y = 0.5f;
            i_this->scale.z = 2.0f;
            break;
        case 4:
            i_this->scale.set(2.0f, 2.0f, 2.0f);
            break;
        case 5:
            i_this->scale.x = 4.0f;
            i_this->scale.y = REG0_F(2) + 2.0f;
            i_this->scale.z = 4.0f;
            break;
        case 0xff:
        default:
            break;
        }

        if (!fopAcM_entrySolidHeap(i_this, daKui_CreateHeap, 0x29f4)) {
            return cPhs_ERROR_e;
        }

        if (dBgS_Regist(dComIfG_Bgsp(), i_this->field_0x2D8, i_this)) {
            return cPhs_ERROR_e;
        }

        fopAcM_SetMtx(i_this, i_this->mpModel2->getBaseTRMtx());
        if (i_this->type >= 2) {
            fopAcM_SetMin(i_this, -200.0f, -1000.0f, -200.0f);
            fopAcM_SetMax(i_this, 200.0f, 2000.0f, 200.0f);
        } else {
            fopAcM_SetMin(i_this, -200.0f, -200.0f, -200.0f);
            fopAcM_SetMax(i_this, 200.0f, 200.0f, 200.0f);
        }
    }
    return result;
}

// HD: compiler-generated and inline copies of this translation unit.

/* 021AE0EC */
static void __sinit_d_a_kui_cpp() { __sinit_header_statics(); }

/* 021AE180 */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021AE194 */
// HD: kui_class deleting destructor
static void kui_class_dt(kui_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021AE1E8 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 028E9E88 void C_VECHalfAngle(const cXyz*, const cXyz*, cXyz*)
ext 028E9684 void C_MTXLookAt(Mtx34*, const cXyz*, const cXyz*, const cXyz*)
str 1001390C "K SUB  COUNT  %d"
str 10013968 "Kui" in 021ADAFC
str 1001396C "Kui" in 021ADB48
str 10013970 "d_a_kui.cpp"
str 1001397C "modelData != NULL" renamed
str 100139A4 "Kui" in 021ADD6C
data mtx_adj 101B8A20
data bure_xa_d 101B8A50
*/
// clang-format on
