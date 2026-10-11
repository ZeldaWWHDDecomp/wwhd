/**
 * d_a_msw.cpp (WWHD)
 * Object - swinging platform hung on four chains.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_msw.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_msw.h"
#define REG6_F(i) REG_F(6, i)
#define REG6_S(i) REG_S(6, i)
static dCcD_Cyl *dCcD_Cyl_ct_tu(dCcD_Cyl *);
static f32 xd[4] = {1, 1, -1, -1};
static f32 zd[4] = {1, -1, 1, -1};
static dCcD_SrcCyl himo_cyl_src;

// HD: arithmetic operand and stack-copy order retain the verified GHS form.
/* 021D6270 */
static void ride_call_back(dBgW *bgw, fopAc_ac_c *i_ac, fopAc_ac_c *i_pt) {
    msw_class *pActor = static_cast<msw_class *>(i_ac);

    cMtx_YrotS(calc_mtx(), -i_ac->current.angle.y);

    cXyz res;
    cXyz tmp;
    cXyz pos1;
    cXyz pos2;
    cXyz_mi(&i_pt->current.pos, &res,
            &i_ac->current.pos); /* tmp = i_pt->current.pos - i_ac->current.pos */
    tmp.copy(res);
    MtxPosition(&tmp, &pos1);
    cXyz_mi(&i_pt->old.pos, &res, &i_ac->current.pos);
    tmp.copy(res);
    MtxPosition(&tmp, &pos2);

    f32 k = REG0_F(0) + 20.0f;
    f32 kx = k / i_ac->scale.x;
    f32 kz = k / i_ac->scale.z;
    s16 z = (s16)gabi::ftoi(pos1.z * kz);
    s16 x = (s16)gabi::ftoi(-(pos1.x * kx));

    cLib_addCalcAngleS2(&i_ac->current.angle.x, z, 10, 0x800);
    cLib_addCalcAngleS2(&i_ac->current.angle.z, x, 10, 0x800);

    f32 dist = (REG0_F(4) + 50.0f) * __builtin_fabsf(pos1.z - pos2.z);
    if (pActor->m2BC.x < dist) {
        pActor->m2BC.x = dist;
    }

    dist = (REG0_F(4) + 50.0f) * __builtin_fabsf(pos1.x - pos2.x);
    if (pActor->m2BC.z < dist) {
        pActor->m2BC.z = dist;
    }

    dist = (REG0_F(8) + 5.0f) * __builtin_fabsf(pos1.x - pos2.x);
    if (dist > 10.0f && pActor->m2B0.x < dist) {
        cLib_addCalc2(&pActor->m2B0.x, dist, 1.0f, REG0_F(7) + 1.2f);
    }

    dist = (REG0_F(8) + 5.0f) * __builtin_fabsf(pos1.z - pos2.z);
    if (dist > 10.0f && pActor->m2B0.z < dist) {
        cLib_addCalc2(&pActor->m2B0.z, dist, 1.0f, REG0_F(7) + 1.2f);
    }
}

/* inlined into daMsw_Draw */
static inline void chain_Draw(msw_class *i_this) {
    for (int i = 0; i < 4; i++) {
        cXyz tmp;
        cXyz_mi(&i_this->m310[i], &tmp, &i_this->m2E0[i]);
        f32 ty = tmp.y, tz = tmp.z, tx = tmp.x;
        s16 angle1 = -cM_atan2s(ty, tz);
        s16 angle2 = cM_atan2s(tx, std_sqrtf(gabi::fmadds(ty, ty, tz * tz)));

        MtxTrans(i_this->m2E0[i].x, i_this->m2E0[i].y, i_this->m2E0[i].z, false);

        s16 var_r4;
        s16 var_r20;
        if (i_this->m83C[i] != 0) {
            s16 t = i_this->m298;
            var_r20 =
                (s16)gabi::ftoi(cM_ssin(t * 18000) * (f32)i_this->m83C[i] * (REG6_F(6) + 100.0f));
            var_r4 = (s16)(t * 3000);
        } else {
            var_r4 = 0;
            var_r20 = 0;
        }

        cMtx_YrotM(calc_mtx(), var_r4);
        cMtx_XrotM(calc_mtx(), (s16)(angle1 + var_r20));
        cMtx_YrotM(calc_mtx(), angle2);

        J3DModel_setBaseTRMtx(i_this->mpChainModels[i], calc_mtx());
        dScnKy_env_light_c *env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->mpChainModels[i], &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->mpChainModels[i]);
    }
}

/* 021D6518 */
static BOOL daMsw_Draw(msw_class *i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    dScnKy_env_light_c *env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpModel, &i_this->tevStr);

    mDoExt_modelUpdateDL(i_this->mpModel);

    dComIfGd_setListBG();
    chain_Draw(i_this);
    dComIfGd_setList();

    return TRUE;
}

/* inlined into daMsw_Execute */
static inline void msw_move(msw_class *i_this) {
    i_this->m298 = (s16)(i_this->m298 + 1);

    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 10, 0x200);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 10, 0x200);

    s16 t = i_this->m298;
    i_this->m2C8.x = (s16)gabi::ftoi(cM_ssin(t * 1500) * i_this->m2BC.x);
    i_this->m2C8.z = (s16)gabi::ftoi(cM_ssin(t * 1300) * i_this->m2BC.z);

    cLib_addCalc2(&i_this->m2BC.x, REG0_F(9), 1.0f, REG0_F(3) + 20.0f);
    cLib_addCalc2(&i_this->m2BC.z, REG0_F(9), 1.0f, REG0_F(3) + 20.0f);

    t = i_this->m298;
    i_this->m2A4.x = cM_ssin(t * 750) * i_this->m2B0.x;
    i_this->m2A4.z = cM_ssin(t * 900) * i_this->m2B0.z;

    cLib_addCalc0(&i_this->m2B0.x, 1.0f, REG0_F(6) + 0.25f);
    cLib_addCalc0(&i_this->m2B0.z, 1.0f, REG0_F(6) + 0.25f);

    mswAddAngles(&i_this->shape_angle, &i_this->current.angle,
                 &i_this->m2C8); /* shape_angle = current.angle + m2C8 */
    cXyz res;
    cXyz_pl(&i_this->home.pos, &res, &i_this->m2A4); /* current.pos = home.pos + m2A4 */
    i_this->current.pos.copy(res);
}

/* 021D6880 */
static BOOL daMsw_Execute(msw_class *i_this) {
    fopAc_ac_c *actor = i_this;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    dComIfGp_get(); /* HD: a second play-object access whose result is unused */

    msw_move(i_this);

    MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), actor->shape_angle.y);
    cMtx_XrotM(calc_mtx(), actor->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), actor->shape_angle.z);
    MtxScale(actor->scale.x, 1.0f, actor->scale.z, true);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
    MtxPush();

    for (int chainIdx = 0; chainIdx < 4; chainIdx++) {
        cXyz src;
        f32 k = REG0_F(10) + 200.0f;
        src.y = 0.0f;
        src.z = zd[chainIdx] * k;
        src.x = xd[chainIdx] * k;
        MtxPosition(&src, &i_this->m2E0[chainIdx]);

        if (i_this->m844 != 0) {
            i_this->m844 = (s16)(i_this->m844 - 1);
            i_this->m310[chainIdx].copy(i_this->m2E0[chainIdx]);
            i_this->m310[chainIdx].y = i_this->m2E0[chainIdx].y + 1000.0f;
        }

        if (i_this->m83C[chainIdx] != 0) {
            i_this->m83C[chainIdx] = (s16)(i_this->m83C[chainIdx] - 1);
        }

        if (i_this->mChainCyls[chainIdx].ChkTgHit() && i_this->m83C[chainIdx] < 10) {
            i_this->m83C[chainIdx] = (s16)(REG6_S(3) + 15);

            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(actor));
            JAIZelBasic_seStart(JA_SE_LK_HIT_SBRIDGE_CHAIN, &i_this->m2E0[chainIdx], 0, reverb);

            cXyz scale;
            scale.z = 2.0f;
            scale.y = 2.0f;
            scale.x = 2.0f;
            dComIfGp_particle_set(ID_AK_JN_NG, i_this->mChainCyls[chainIdx].GetTgHitPosP(),
                                  &player->shape_angle, &scale);
        }
        i_this->mChainCyls[chainIdx].SetC(&i_this->m2E0[chainIdx]);

        dComIfG_Ccsp_Set(&i_this->mChainCyls[chainIdx]);
    }

    MtxPull();
    PSMTXCopy(calc_mtx(), &i_this->mMtx); /* MTXCopy */
    dBgW_Move(i_this->mpBgW);

    return TRUE;
}

/* 021D6D50 */
static BOOL daMsw_IsDelete(msw_class *i_this) {
    for (int i = 0; i < 4; i++) {
        mDoAud_seDeleteObject(&i_this->m2E0[i]);
    }

    return TRUE;
}

/* 021D6D9C */
static BOOL daMsw_Delete(msw_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Msw"); /* dComIfG_resDeleteDemo */
    cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);

    return TRUE;
}

/* 021D6DE8 */
static BOOL daMsw_CreateInit(fopAc_ac_c *i_this) {
    msw_class *pActor = static_cast<msw_class *>(i_this);

    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
        "Msw", dRes_INDEX_MSW_BDL_MSWNG_e, mswSafeStringVtable);
    J3DModel *model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    pActor->mpModel = model;

    if (model == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData *)dComIfG_getObjectRes("Msw", dRes_INDEX_MSW_BDL_OBM_CHAIN1_e,
                                                     mswSafeStringVtable);
    if (modelData == nullptr) /* JUT_ASSERT(523, modelData != NULL) */
        JUT_ASSERT_fail("d_a_msw.cpp", 0x20B, "modelData != NULL");

    for (int chainIdx = 0; chainIdx < 4; chainIdx++) {
        model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        pActor->mpChainModels[chainIdx] = model;
        if (model == nullptr) {
            return FALSE;
        }
    }

    dBgW *bgw = new_dBgW();
    pActor->mpBgW = bgw;

    if (bgw == nullptr) {
        return FALSE;
    }

    cBgD_t *pBgd =
        (cBgD_t *)dComIfG_getObjectRes("Msw", dRes_INDEX_MSW_DZB_MSWING_e, mswSafeStringVtable);

    BOOL error = mswSetBg(pActor->mpBgW, pBgd, cBgW_MOVE_BG_e, &pActor->mMtx);
    if (error == TRUE) {
        return FALSE;
    }

    mswCallbacks(pActor->mpBgW)->mpCrrFunc = dBgS_MoveBGProc_Typical;
    mswCallbacks(pActor->mpBgW)->mpRideCallback = gabi::guest_fn((const void *)ride_call_back);

    return TRUE;
}

/* 021D6F48 */
static cPhs_State daMsw_Create(fopAc_ac_c *i_this) {
    msw_class *a_this = static_cast<msw_class *>(i_this);
    dComIfGp_get(); /* HD: unused play-object access */

    /* fopAcM_ct(i_this, msw_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = mswActorVtable;
            dCcD_Stts_ct(&a_this->mStts);
            mswConstructArray(a_this->mChainCyls, 4, 0x130,
                              gabi::guest_fn((const void *)dCcD_Cyl_ct_tu));
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhs, "Msw");

    if (phase_state == cPhs_COMPLEATE_e) {
        u8 prm = (u8)(fopAcM_GetParam(a_this) >> 0);
        a_this->m2A0 = prm == 0xFF ? 0 : prm;

        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void *)daMsw_CreateInit),
                                   0x10040)) {
            return cPhs_ERROR_e;
        }

        if (a_this->mpModel == nullptr) {
            return cPhs_ERROR_e;
        }

        if (dBgS_Regist(dComIfG_Bgsp(), a_this->mpBgW, a_this)) {
            return cPhs_ERROR_e;
        }

        switch (a_this->m2A0) {
        case 1:
            a_this->scale.x = 1.5f;
            a_this->scale.z = 1.5f;
            break;
        case 2:
            a_this->scale.x = 2.0f;
            a_this->scale.z = 2.0f;
            break;
        case 3:
            a_this->scale.x = 3.0f;
            a_this->scale.z = 3.0f;
            break;
        default:
            a_this->scale.z = 1.0f;
            a_this->scale.x = 1.0f;
            break;
        }

        a_this->scale.y = 1.0f;
        J3DModel *model = a_this->mpModel;
        a_this->cullMtx = model != nullptr ? gabi::ea(model->getBaseTRMtx())
                                           : 0; /* fopAcM_SetMtx(getBaseTRMtx()) */
        fopAcM_SetMin(a_this, -200.0f * a_this->scale.x, -5000.0f, -200.0f * a_this->scale.z);
        fopAcM_SetMax(a_this, 200.0f * a_this->scale.x, 5000.0f, 200.0f * a_this->scale.z);

        a_this->mStts.Init(0xFF, 0xFF, a_this);

        for (int chainIdx = 0; chainIdx < 4; chainIdx++) {
            a_this->mChainCyls[chainIdx].Set(&himo_cyl_src);
            a_this->mChainCyls[chainIdx].SetStts(&a_this->mStts);
        }

        a_this->m844 = 10;
        daMsw_Execute(a_this);
    }

    return phase_state;
}

// HD: compiler-generated and template code of this translation unit
/* 021D7208 */
static void __sinit_d_a_msw_cpp() {
    sinit_header_statics(0x1046582C, 0x101BAD6C);
}

/* 021D729C */
static void trivial_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021D72B0 */
static dCcD_Cyl *dCcD_Cyl_ct_tu(dCcD_Cyl *c) {
    if (c == nullptr) {
        c = (dCcD_Cyl *)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, mswAabVtable);
    return c;
}

/* 021D733C */
static void msw_class_dt(msw_class *i_this, s32 flags) {
    if (i_this != nullptr) {
        mswDestroyArray(i_this->mChainCyls, 4, 0x130, mswCylDestructor, 0,
                        0); // HD: GHS array destruction ABI.
        dCcD_Stts_dt(&i_this->mStts, 2);
        mswActorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021D73BC */
static void empty_virtual(void *p) {
}

// clang-format off
/* WWHD glue
str 10015480 "Msw" in 021D6F48
str 10015444 "Msw" in 021D6D9C
str 10015448 "Msw" in 021D6DE8
str 1001544C "d_a_msw.cpp"
str 10015458 "modelData != NULL"
data xd 101BAD08
data zd 101BAD18
data himo_cyl_src 101BAD28
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*,f32,f32,f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*,f32,f32,f32)
ext 0200A030 BOOL mswSetBg(dBgW*,cBgD_t*,u32,Mtx34*)
ext 0201A4DC u32 mswAddAngleWords(csXyz*,csXyz*)
ext 028EFFD0 void mswConstructArray(void*,u32,u32,u32)
ext 028F0164 void mswDestroyArray(void*,u32,u32,u32,u32,u32)
ext 025E1A40 void JAIZelBasic_seStart(u32,cXyz*,u32,s32)
ext 025D50BC void mswActorDestroy(void*,s32)
*/
// clang-format on
