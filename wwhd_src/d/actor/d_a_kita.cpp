/**
 * d_a_kita.cpp (WWHD)
 * Object - Forbidden Woods - Hanging flower platform
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kita.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * kita_move is inlined into daKita_Execute, himo_create into daKita_Create.
 */
#include <cmath>

#include "d/actor/d_a_kita.h"

// HD: GHS emits these inline-class vtables once per translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x10012DAC,
    .objAcch = {0x10012E94, 0x10012EB4, 0x10012EA4},
    .objAcchDt = {0x10012EA4, 0x10012EB4},
    .headerStatics = {0x10464BA8, 0x101B83D0},
};
static const dBgS_GndChk_vt GNDCHK_VT = {0x10012DD4, 0x10012DE4, 0x10012E04, 0x10012DF4};
static const dBgS_GndChk_vt OBJGNDCHK_VT = {0x10012E54, 0x10012E64, 0x10012E84, 0x10012E74};

enum { fpcNm_PLAYER_e = 0xA8, fpcNm_SHAND_e = 0x63 };
enum {
    JA_SE_LK_FLIFT_GO_WATER = 0x2884,
    JA_SE_OBJ_KOKIRI_H_LANDING = 0x6951,
    JA_SE_OBJ_P_FLOWER_LAND_W = 0x6953,
};
enum {
    dPa_name_ID_AK_SN_FFSPLASH00 = 0x828C,
    dPa_name_ID_AK_SN_FFHAMON00 = 0x828D,
    dPa_name_ID_IT_JN_WP_HAMON03 = 0x3F,
};

/* file statics (.data) */
static u8 himo_off_check[4] = {1, 2, 4, 8};
static s16 himo_off_ya[16] = {1,    -24576, 24576, -32768, -8192, -16384, -8192, -24576,
                              8192, 8192,   16384, 24576,  0,     -8192,  8192,  1};
static s16 himo_off_xa[16] = {0,     -4096, -4096,  -14336, -4096,  -14336, 0,      -16384,
                              -4096, 0,     -14336, -16384, -14336, -16384, -16384, 0};
static s16 himo_off_yp[16] = {0,   150, 150, 400, 150, 400, 400, 500,
                              150, 400, 400, 500, 400, 500, 500, 0};
static s16 yad[4] = {8192, -8192, 24576, -24576};
static f32 xd[4] = {130.0f, -130.0f, 130.0f, -130.0f};
static f32 zd[4] = {130.0f, 130.0f, -130.0f, -130.0f};
// HD: packed collision-source bytes retain the tww sphere configuration.
static u32 utiwa_sph_src[16] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000005, 0x00200000, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x43C80000};
#define REG0_S_(i) REG_S(0, i)

/* 0219DBFC */
static void ride_call_back(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt) {
    kita_class* pActor = static_cast<kita_class*>(i_ac);

    if (!pActor->field_29A || (pActor->field_360 != 0)) {
        gabi::Local<cXyz> tmp, delta_pos, local_44, local_50;
        cMtx_YrotS(calc_mtx(), (s16)-pActor->current.angle.y);
        cXyz_mi(&i_pt->current.pos, tmp, &pActor->current.pos);
        delta_pos->copy(*tmp);
        MtxPosition(delta_pos, local_44);
        cXyz_mi(&i_pt->old.pos, tmp, &pActor->current.pos);
        delta_pos->copy(*tmp);
        MtxPosition(delta_pos, local_50);
        if (i_pt != nullptr && fpcM_GetName(i_pt) == fpcNm_PLAYER_e) {
            pActor->mExecuteCount = 10;
        }
        f32 k = REG0_F(0) + 10.0f;
        f32 kx = k / pActor->scale.x;
        f32 kz = k / pActor->scale.z;
        s16 xAngle_target = (s16)gabi::ftoi(local_44->z * kz);
        s16 zAngle_target = (s16)gabi::ftoi(-(local_44->x * kx)); /* HD: -(x * k) */
        cLib_addCalcAngleS2(&pActor->current.angle.x, xAngle_target, 10, 0x800);
        cLib_addCalcAngleS2(&pActor->current.angle.z, zAngle_target, 10, 0x800);

        f32 min_val_x = std::fabs(local_44->z - local_50->z) * (REG0_F(4) + 50.0f);
        if (pActor->field_2BC.x < min_val_x)
            pActor->field_2BC.x = min_val_x;

        f32 min_val_z = std::fabs(local_44->x - local_50->x) * (REG0_F(4) + 50.0f);
        if (pActor->field_2BC.z < min_val_z)
            pActor->field_2BC.z = min_val_z;

        f32 fVar1 = std::fabs(local_44->x - local_50->x) * (REG0_F(8) + 5.0f);
        if (fVar1 > 10.0f && pActor->field_2B0.x < fVar1) {
            cLib_addCalc2(&pActor->field_2B0.x, fVar1, 1.0f, REG0_F(7) + 1.2f);
        }
        fVar1 = std::fabs(local_44->z - local_50->z) * (REG0_F(8) + 5.0f);
        if (fVar1 > 10.0f && pActor->field_2B0.z < fVar1) {
            cLib_addCalc2(&pActor->field_2B0.z, fVar1, 1.0f, REG0_F(7) + 1.2f);
        }
        cLib_addCalc2(&pActor->mPosRel.y, REG0_F(2) + -100.0f, 0.1f, REG0_F(3) + 10.0f);
    }
}

/* 0219DEF8 */
static BOOL daKita_Draw(kita_class* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(i_this->mModel);
    dComIfGd_setList();
    return TRUE;
}

/* inlined into daKita_Execute */
static inline void kita_move_wait(kita_class* i_this) {
    fopAc_ac_c* actor = i_this;
    s32 mask = 0;
    for (int i = 0; i < 4; i++) {
        if (i_this->field_318[i] == 2) {
            i_this->field_318[i] = 0;
            i_this->field_320 = 0.0f;
        }
        u8 v = i_this->field_318[i];
        if (v != 0) {
            if (v == 1) {
                i_this->field_320 = 0.0f;
                i_this->field_318[i] = 5;
            }
            mask |= himo_off_check[i];
        }
    }

    f32 step = 250.0f * i_this->field_320;
    f32 yp = (f32)himo_off_yp[mask];
    s16 ya_offset = himo_off_ya[mask];
    s16 xa_offset = himo_off_xa[mask];
    cLib_addCalc2(&i_this->mHeight, gabi::fnmsubs(yp, 0.3f, REG0_F(4)), 0.05f, step);
    if (mask == 0xF) {
        i_this->field_29A = 1;

        gabi::Local<dBgS_GndChk> solid_ground_check;
        dBgS_GndChk_ct(solid_ground_check, GNDCHK_VT, false);
        {
            f32 y = actor->current.pos.y, x = actor->current.pos.x, z = actor->current.pos.z;
            y -= 200.0f;
            solid_ground_check->m_pos.x = x;
            solid_ground_check->m_pos.y = y;
            solid_ground_check->m_pos.z = z;
        }
        f32 solid_ground_cross = cBgS_GroundCross(dComIfG_Bgsp(), solid_ground_check) + REG0_F(13);
        if (i_this->field_35C < solid_ground_cross)
            i_this->field_35C = solid_ground_cross;

        gabi::Local<dBgS_GndChk> liquid_ground_check; /* dBgS_ObjGndChk_Spl */
        dBgS_GndChk_ct(liquid_ground_check, OBJGNDCHK_VT, true);
        liquid_ground_check->mGrp = 0xE; /* mGrp: Spl */
        {
            f32 y = i_this->current.pos.y, x = i_this->current.pos.x, z = i_this->current.pos.z;
            y -= 200.0f;
            liquid_ground_check->m_pos.x = x;
            liquid_ground_check->m_pos.y = y;
            liquid_ground_check->m_pos.z = z;
        }
        f32 liquid_gnd_cross = cBgS_GroundCross(dComIfG_Bgsp(), liquid_ground_check);
        if (liquid_gnd_cross != -1000000000.0f && liquid_gnd_cross > i_this->field_35C) {
            i_this->field_360 = 1;
            i_this->field_35C = liquid_gnd_cross + 40.0f + REG0_F(17);
        }
        actor->health = 0;
        dBgS_GndChk_dt(liquid_ground_check, GNDCHK_VT, 0x10012DC4);
        dBgS_GndChk_dt(solid_ground_check, GNDCHK_VT, 0x10012DC4);
    }
    s16 maxSpeed = (s16)gabi::ftoi(10000.0f * i_this->field_320);
    cLib_addCalcAngleS2(&i_this->mRotX, xa_offset, 16, maxSpeed);
    if (xa_offset != 0)
        cLib_addCalcAngleS2(&i_this->mRotY, ya_offset, 4, (s16)(maxSpeed * 2));
    cLib_addCalc2(&i_this->field_320, 1.0f, 1.0f, REG0_F(14) + 0.001f);
    cLib_addCalcAngleS2(&actor->current.angle.x, 0, 10, 0x200);
    cLib_addCalcAngleS2(&actor->current.angle.z, 0, 10, 0x200);
    {
        s16 cnt = i_this->mMoveCounter;
        f32 bx = i_this->field_2BC.x;
        i_this->field_2C8.x = (s16)gabi::ftoi(cM_ssin(cnt * 1500) * bx);
        f32 bz = i_this->field_2BC.z;
        i_this->field_2C8.z = (s16)gabi::ftoi(cM_ssin(cnt * 1300) * bz);
    }
    cLib_addCalc2(&i_this->field_2BC.x, REG0_F(9) + 300.0f, 1.0f, REG0_F(3) + 20.0f);
    cLib_addCalc2(&i_this->field_2BC.z, REG0_F(9) + 300.0f, 1.0f, REG0_F(3) + 20.0f);
    {
        s16 cnt = i_this->mMoveCounter;
        i_this->mPosRel.x = cM_ssin(cnt * 750) * i_this->field_2B0.x;
        i_this->mPosRel.z = cM_ssin(cnt * 900) * i_this->field_2B0.z;
    }
    cLib_addCalc0(&i_this->field_2B0.x, 1.0f, REG0_F(6) + 0.25f);
    cLib_addCalc0(&i_this->field_2B0.z, 1.0f, REG0_F(6) + 0.25f);
    csXyz_pl_to(&actor->shape_angle, &actor->current.angle, &i_this->field_2C8);
    {
        gabi::Local<cXyz> sum;
        cXyz_pl(&actor->home.pos, sum, &i_this->mPosRel);
        f32 x = sum->x, y = sum->y, z = sum->z;
        actor->current.pos.x = x;
        actor->current.pos.y = y;
        actor->current.pos.z = z;
        actor->current.pos.y = y + i_this->mHeight;
    }
    cLib_addCalc0(&i_this->mPosRel.y, 0.05f, REG0_F(7) + 2.0f);
}

/* inlined into daKita_Execute */
static inline void kita_move_fall(kita_class* i_this, fopAc_ac_c* player_actor) {
    fopAc_ac_c* actor = i_this;
    if (i_this->field_360 == 2) {
        i_this->mSph.SetC(&actor->current.pos);
        dComIfG_Ccsp_Set(&i_this->mSph);
        if (i_this->mSph.ChkTgHit() != 0) {
            s16 execCount = i_this->mExecuteCount;
            /* HD: the push direction is the player's facing, not the player-to-platform angle */
            i_this->mPlayerAngle = player_actor->shape_angle.y;
            i_this->field_374 = 20;
            s16 search = fopAcM_searchPlayerAngleY(actor);
            if (execCount != 0) {
                s16 angleY_kita_player = (s16)(search - player_actor->shape_angle.y);
                i_this->field_36C =
                    std::fabs(cM_scos(angleY_kita_player)) * (REG0_F(2) + -6.0f); /* HD: fabs */
                f32 fVar3 = fopAcM_searchPlayerDistance(actor) * 0.003f;
                if (fVar3 > 1.0f)
                    fVar3 = 1.0f;
                i_this->field_370 = (cM_ssin(angleY_kita_player) * (REG0_F(3) + 200.0f)) * fVar3;
            } else {
                s16 angle = (s16)(search - player_actor->shape_angle.y);
                i_this->field_36C = std::fabs(cM_scos(angle)) * (REG0_F(12) + 6.0f); /* HD: fabs */
                i_this->field_370 = cM_rndFX(100.0f);
            }
            fopAcM_seStartCurrent(actor, JA_SE_LK_FLIFT_GO_WATER, 0);
        }
        if (i_this->field_374 != 0) {
            i_this->field_374 = i_this->field_374 - 1;
            cLib_addCalc2(&i_this->field_364, i_this->field_36C, 1.0f, REG0_F(5) + 0.3f);
            cLib_addCalc2(&i_this->mAngleYSpeed, i_this->field_370, 1.0f, REG0_F(6) + 5.0f);
        } else {
            cLib_addCalc0(&i_this->field_364, 1.0f, REG0_F(0) + 0.1f);
            cLib_addCalc0(&i_this->mAngleYSpeed, 1.0f, REG0_F(1) + 1.0f);
        }
        /* HD: the drift goes through speed (chased), not straight into current.pos */
        cMtx_YrotS(calc_mtx(), i_this->mPlayerAngle);
        gabi::Local<cXyz> pos_offset2, pos_offset;
        pos_offset2->x = 0.0f;
        pos_offset2->y = 0.0f;
        f32 f = i_this->field_364;
        pos_offset2->z = f + f;
        MtxPosition(pos_offset2, pos_offset);
        cLib_addCalc2(&actor->speed.x, pos_offset->x, 0.5f, 0.5f);
        cLib_addCalc2(&actor->speed.z, pos_offset->z, 0.5f, 0.5f);
        s16 spd = (s16)gabi::ftoi(i_this->mAngleYSpeed);
        i_this->current.angle.y = (s16)(i_this->current.angle.y + spd);
    } else {
        cLib_addCalc2(&actor->current.pos.x, actor->home.pos.x, 0.5f, 10.0f);
        cLib_addCalc2(&actor->current.pos.z, actor->home.pos.z, 0.5f, 10.0f);
    }
    cLib_addCalcAngleS2(&actor->current.angle.x, 0, 10, 0x300);
    cLib_addCalcAngleS2(&actor->current.angle.z, 0, 10, 0x300);
    cLib_addCalcAngleS2(&i_this->mRotX, 0, 4, 0x200);

    f32 fVar4;
    s32 local29A;
    if (i_this->field_360 == 2) {
        fVar4 = 1.0f;
        local29A = 800;
    } else {
        fVar4 = 0.0f;
        local29A = 2500;
    }
    {
        s16 cnt = i_this->mMoveCounter;
        f32 bx = i_this->field_2BC.x;
        i_this->field_2C8.x = (s16)gabi::ftoi(cM_ssin(cnt * local29A) * bx);
        f32 bz = i_this->field_2BC.z;
        i_this->field_2C8.z = (s16)gabi::ftoi(cM_ssin(cnt * (local29A - 200)) * bz);
    }
    cLib_addCalc2(&i_this->field_2BC.x, (REG0_F(9) + 400.0f) * fVar4, 1.0f, REG0_F(3) + 20.0f);
    cLib_addCalc2(&i_this->field_2BC.z, (REG0_F(9) + 400.0f) * fVar4, 1.0f, REG0_F(3) + 20.0f);

    csXyz_pl_to(&actor->shape_angle, &actor->current.angle, &i_this->field_2C8);
    PSVECAdd(&actor->current.pos, &actor->speed,
             &actor->current.pos); /* HD: current.pos += speed */
    f32 sy = actor->speed.y - 5.0f;
    if (sy < -150.0f)
        sy = -150.0f;
    actor->speed.y = sy;

    f32 floor = i_this->field_35C;
    if (!(actor->current.pos.y > floor)) {
        actor->current.pos.y = floor;
        if (i_this->speed.y < -50.0f) {
            i_this->field_2BC.z = 2000.0f;
            i_this->field_2BC.x = 2000.0f;
            if (i_this->field_360 != 0) {
                i_this->field_360 = 2;
                fopAcM_seStart_nn(actor, JA_SE_OBJ_P_FLOWER_LAND_W, 0);
                gabi::Local<cXyz> particle_scale;
                particle_scale->set(3.0f, 3.0f, 3.0f);
                dComIfGp_particle_set(dPa_name_ID_AK_SN_FFSPLASH00, &actor->current.pos);
                dComIfGp_particle_set(dPa_name_ID_IT_JN_WP_HAMON03, &actor->current.pos, nullptr,
                                      particle_scale);

                dVibration_c* vibration = dComIfGp_getVibration();
                s16 str = REG0_S_(2);
                gabi::Local<cXyz> up;
                up->set(0.0f, 1.0f, 0.0f);
                kitaStartShock(vibration, (s32)(str + 4), -0x21, up.get()); /* StartShock */
            } else {
                fopAcM_seStart_nn(actor, JA_SE_OBJ_KOKIRI_H_LANDING, 0);
            }
        }
        actor->speed.y = 0.0f;
        cLib_addCalcAngleS2(&i_this->mRotX, 0, 2, 0x2000);
    }
    if (i_this->field_360 != 0) {
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
    }
}

/* 0219DF90 */
static BOOL daKita_Execute(kita_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: result unused */

    if (i_this->mExecuteCount != 0)
        i_this->mExecuteCount = i_this->mExecuteCount - 1;

    /* kita_move(i_this) */
    fopAc_ac_c* player_actor = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
    i_this->mMoveCounter = i_this->mMoveCounter + 1;
    switch ((u16)i_this->field_29A) {
    case 0:
        kita_move_wait(i_this);
        break;
    case 1:
        kita_move_fall(i_this, player_actor);
        break;
    }

    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), actor->shape_angle.y);
    cMtx_YrotM(calc_mtx(), i_this->mRotY);
    {
        f32 s = cM_ssin(i_this->mRotX);
        MtxTrans(0.0f, 0.0f, s * (REG0_F(11) + -150.0f), true);
    }
    cMtx_XrotM(calc_mtx(), i_this->mRotX);
    cMtx_YrotM(calc_mtx(), (s16)-i_this->mRotY);
    cMtx_XrotM(calc_mtx(), actor->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), actor->shape_angle.z);
    J3DModel_setBaseTRMtx(i_this->mModel, calc_mtx());

    gabi::Local<cXyz> local48;
    for (int i = 0; i < 4; i++) {
        MtxPush();
        local48->x = xd[i] * actor->scale.x;
        local48->y = REG0_F(5) + -15.0f;
        local48->z = zd[i] * actor->scale.z;
        MtxPosition(local48, &i_this->field_2E8[i]);
        MtxPull();
    }
    PSMTXCopy(calc_mtx(), &i_this->mBgwMtx);
    dBgW_Move(i_this->pm_bgw);

    if (i_this->field_360 == 2) {
        if (i_this->mBaseEmitter == nullptr) {
            i_this->mBaseEmitter =
                dComIfGp_particle_set(dPa_name_ID_AK_SN_FFHAMON00, &actor->current.pos);
        } else {
            f32 d = REG0_F(17) + 40.0f;
            JPABaseEmitter* e = i_this->mBaseEmitter;
            JPABaseEmitter_setGlobalTranslation(e, actor->current.pos.x, actor->current.pos.y - d,
                                                actor->current.pos.z);
        }
    } else {
        if (i_this->mBaseEmitter != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mBaseEmitter);
            i_this->mBaseEmitter = nullptr;
        }
    }

    return TRUE;
}

/* 0219EEB8 */
static BOOL daKita_IsDelete(kita_class*) { return TRUE; }

/* 0219EEC0 */
static BOOL daKita_Delete(kita_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "kita"); /* dComIfG_resDeleteDemo */
    if (i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
    }
    if (i_this->mBaseEmitter != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->mBaseEmitter);
    }
    return TRUE;
}

/* 0219EF38 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) {
    BOOL ret;
    kita_class* actor = static_cast<kita_class*>(i_this);

    J3DModelData* modelData = static_cast<J3DModelData*>(
        dComIfG_getObjectRes("kita", dRes_INDEX_KITA_BDL_VHLIF_00_e, gc_tu.safeString));
    actor->mModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);

    if (actor->mModel == nullptr) {
        ret = FALSE;
    } else {
        if (modelData == nullptr) /* JUT_ASSERT(946, modelData != NULL) */
            JUT_ASSERT_fail("d_a_kita.cpp", 0x539, "modelData != (0)");
        actor->pm_bgw = new_dBgW();
        if (actor->pm_bgw == nullptr) /* JUT_ASSERT(951, actor->pm_bgw != NULL) */
            JUT_ASSERT_fail("d_a_kita.cpp", 0x53E, "actor->pm_bgw != (0)");
        cBgD_t* dzb = static_cast<cBgD_t*>(
            dComIfG_getObjectRes("kita", dRes_INDEX_KITA_DZB_HLIF_00_e, gc_tu.safeString));
        cBgW_Set(actor->pm_bgw, dzb, cBgW_MOVE_BG_e, &actor->mBgwMtx);
        dBgW_SetCrrFunc(actor->pm_bgw, kitaMoveBGProcTypical);
        dBgW_SetRideCallback(actor->pm_bgw, gabi::guest_fn((const void*)ride_call_back));
        ret = TRUE;
    }

    return ret;
}

/* inlined into daKita_Create */
static inline BOOL himo_create(kita_class* i_this) {
    int shand_count = 0;
    fopAcM_prm_class* param;

    for (int i = 0; i < 4; i++) {
        switch (i_this->field_2E4[i]) {
        case 0: {
            param = fopAcM_CreateAppend();
            f32 x = i_this->current.pos.x;
            param->position.x = x;
            f32 y = i_this->current.pos.y;
            param->position.y = y;
            f32 z = i_this->current.pos.z;
            param->position.z = z;
            param->angle.y = yad[i];
            param->parameters = 0xFFFFFF35;
            param->room_no = i_this->current.roomNo;
            i_this->field_2D4[i] = fopAcM_Create(fpcNm_SHAND_e, param);
            i_this->field_2E4[i] = (u8)(i_this->field_2E4[i] + 1);
        }
            /* fallthrough */
        case 1: {
            fopAc_ac_c* shand_i = fopAcM_SearchByID(i_this->field_2D4[i]);
            if (shand_i != nullptr) {
                auto* shand = static_cast<KitaShandView*>(shand_i);
                shand->field_308 = fopAcM_GetID(i_this);
                shand->field_310 = &i_this->field_2E8[i];
                shand->field_314 = &i_this->field_318[i];
                i_this->field_2E4[i] = (u8)(i_this->field_2E4[i] + 1);
                shand_count++;
            }
            break;
        }
        case 2:
            break;
        }
    }

    if (shand_count < 4) {
        return TRUE;
    }
    return FALSE;
}

/* 0219F05C */
static cPhs_State daKita_Create(fopAc_ac_c* a_this) {
    kita_class* i_this = static_cast<kita_class*>(a_this);
    dComIfGp_get(); /* HD: result unused */

    /* fopAcM_ct(a_this, kita_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            i_this->__vtbl = kita_class::__vt;
            dCcD_Stts_ct(&i_this->mStts);
            kitaSphereConstruct(&i_this->mSph); /* dCcD_Sph::dCcD_Sph */
            dBgS_AcchCir_ct(&i_this->mAcchCir);
            __ct(&i_this->mAcch);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret;

    ret = dComIfG_resLoad(&i_this->mPhs, "kita");
    if (ret == cPhs_COMPLEATE_e) {
        i_this->field_2A0 = (u8)fopAcM_GetParam(a_this);
        i_this->field_2A1 = (u8)(fopAcM_GetParam(a_this) >> 8);

        if (i_this->field_2A1 == 1) {
            i_this->field_29A = 1;
            i_this->field_360 = 1;
            i_this->field_35C = a_this->current.pos.y + 70.0f + REG0_F(17);
        } else if (himo_create(i_this))
            return cPhs_INIT_e;

        if (i_this->field_2A0 == 0xff)
            i_this->field_2A0 = 0;

        if (fopAcM_entrySolidHeap(i_this, CallbackCreateHeap, 0x10000) == false)
            return cPhs_ERROR_e;

        if (i_this->pm_bgw != nullptr) {
            dBgS* bgs = dComIfG_Bgsp();
            if (dBgS_Regist(bgs, i_this->pm_bgw, i_this) != 0)
                return cPhs_ERROR_e;
        }

        switch (i_this->field_2A0) {
        case 1:
            i_this->scale.x = 1.25f;
            i_this->scale.z = 1.25f;
            break;
        case 2:
            i_this->scale.x = 1.5f;
            i_this->scale.z = 1.5f;
            break;
        default:
            i_this->scale.z = 1.0f;
            i_this->scale.x = 1.0f;
            break;
        }
        i_this->scale.y = 1.0f;
        fopAcM_SetMtx(i_this, J3DModel_getBaseTRMtx(i_this->mModel));
        fopAcM_SetMin(i_this, -200.0f * i_this->scale.x, -200.0f, -200.0f * i_this->scale.z);
        fopAcM_SetMax(i_this, 200.0f * i_this->scale.x, 200.0f, 200.0f * i_this->scale.z);
        J3DModel_setBaseScale(i_this->mModel, &i_this->scale);
        i_this->health = 1;
        i_this->mAcch.Set(&i_this->current.pos, &i_this->old.pos, i_this, 1, &i_this->mAcchCir,
                          &i_this->speed);
        i_this->mAcchCir.SetWall(50.0f, 300.0f);
        i_this->mStts.Init(0xff, 0xff, i_this);
        i_this->mSph.Set(reinterpret_cast<dCcD_SrcSph*>(utiwa_sph_src));
        i_this->mSph.SetStts(&i_this->mStts);

        for (int i = 0; i < 2; i++)
            daKita_Execute(i_this);
    }

    return ret;
}

// HD: compiler-generated and template code of this translation unit.
/* 0219F4C0 */
static void __sinit_d_a_kita_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

// HD: deleting destructor of a class with a trivial destructor
/* 0219F554 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: kita_class deleting destructor (inline member destructors)
/* 0219F568 */
static void kita_class_dt(kita_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mAcch);
        __dt(&i_this->mAcchCir);
        kitaSphereDestroy(&i_this->mSph);
        __dt(&i_this->mStts);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: an empty virtual (per-TU copy)
/* 0219F604 */
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue
data himo_off_check 101B82EC
data himo_off_ya 101B8310
data himo_off_xa 101B8330
data himo_off_yp 101B8350
data yad 101B82E4
data xd 101B8370
data zd 101B8380
data utiwa_sph_src 101B8390
str 10012F60 "kita" in 0219EF38
str 10012F68 "d_a_kita.cpp" in 0219EF38
str 10012F78 "modelData != (0)" in 0219EF38
str 10012F8C "actor->pm_bgw != (0)" in 0219EF38
str 10012F58 "kita" in 0219EEC0
str 10012FB4 "kita" in 0219F05C
*/
// clang-format on
