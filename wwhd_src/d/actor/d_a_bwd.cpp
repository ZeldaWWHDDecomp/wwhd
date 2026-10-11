/**
 * d_a_bwd.cpp (WWHD)
 * Boss - Molgera: everything but end, move, demo_camera and daBwd_Execute (d_a_bwd_*.cpp).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bwd.h"
#include "gc_api/d/d_com_inf_game.h"

void bwdEnableBlur();
BOOL bwdStartShock(dVibration_c*, s32, s32, cXyz*);
void bwdSetMaterialFlags(J3DModel*, u32);
void* bwdConstructSmoke(void*, u8);
void* bwdConstructFollow(void*, u8, u8);
void bwdStartBgm(u32);
void bwdDestroyActor(fopAc_ac_c*, s32);
void bwdConstructSphere(void*);
void bwdDestroySphere(void*, s32);
static void smoke_dt(void*, s32);
static void follow_dt(void*, s32);
void bwdTypicalCorrection(dBgW*, void*, cBgS_PolyInfo*, bool, cXyz*, csXyz*, csXyz*);

const gc_tu_data gc_tu = {
    .safeString = SAFESTRING_VTBL,
    .headerStatics = {0x10462A5C, 0x1019309C, 0x10462AA8},
};

static bwd_detail::BlurState blurState;
static bwd_detail::LightBlock defaultLight;

static u32 taki_bdl[2] = {35, 36};
static u32 s_bdl[2] = {33, 34};
static u32 s_btk[2] = {48, 49};
static u32 s_brk[2] = {43, 44};
static u32 s_bck[2] = {24, 25};
static bwd_detail::SphereSource body_sph_src;
static bwd_detail::SphereSource bero_sph_src;
static bwd_detail::SphereSource bero_co_sph_src;

enum {
    dRes_INDEX_BWD_BCK_BWD_WAIT_e = 0x17,
    dRes_INDEX_BWD_BDL_BERO_e = 0x1C,
    dRes_INDEX_BWD_BDL_BERO_SAKI_e = 0x1D,
    dRes_INDEX_BWD_BDL_BWD_e = 0x1E,
    dRes_INDEX_BWD_BDL_BWD_SHIPPOA_e = 0x1F,
    dRes_INDEX_BWD_BDL_BWD_SHIPPOB_e = 0x20,
    dRes_INDEX_BWD_BDL_HTRYF1_e = 0x25,
    dRes_INDEX_BWD_BRK_BWD_e = 0x28,
    dRes_INDEX_BWD_BRK_BWD_SHIPPOA_e = 0x29,
    dRes_INDEX_BWD_BRK_BWD_SHIPPOB_e = 0x2A,
    dRes_INDEX_BWD_BTK_TAKI_START_e = 0x34,
    dRes_INDEX_BWD_DZB_HTAKI1_e = 0x38,
    dRes_INDEX_BWD_DZB_HTRYF1_e = 0x39,
};

/* 020F99F8 */
static void g_eff_on(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (i_this->m3960 != 0) {
        return;
    }
    i_this->m3960 = 1;
}

/* 020F9A10 */
static void g_eff_off(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (i_this->m3960 == 0) {
        return;
    }
    i_this->m3960 = -1;
}

/* 020F9A28 */
static void* ko_s_sub(void* param_1, void*) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (fopAc_IsActor(param_1) && param_1 != nullptr &&
        fpcM_GetName(param_1) == bwd_detail::fpcNm_BWDS_e) {
        if (ko_count < l_HIO.m26) {
            ko_ac()[ko_count] = gabi::ea(param_1);
            ko_count = ko_count + 1;
        }
        return nullptr;
    }
    return nullptr;
}

/* 020F9AA0 */
static void* ko_delete_sub(void* param_1, void*) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (fopAc_IsActor(param_1) && param_1 != nullptr &&
        fpcM_GetName(param_1) == bwd_detail::fpcNm_BWDS_e) {
        static_cast<bwd_detail::Larva*>(param_1)->m04F4 = 1;
    }
    return nullptr;
}

/* 020F9AF4 */
void anm_init(bwd_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              int soundFileIdx) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes("Bwd", bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes("Bwd", soundFileIdx, SAFESTRING_VTBL);
        i_this->mpHeadMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes("Bwd", bckFileIdx, SAFESTRING_VTBL);
        i_this->mpHeadMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

// HD: blur is stored as separate enable and rate bytes.
static inline void mDoGph_setBlureRate(u8 rate) { blurState.mRate = rate; }
static inline void mDoGph_onBlure() { bwdEnableBlur(); }
static inline void mDoGph_offBlure() { blurState.mEnabled = 0; }
/* mDoExt_btkAnm::remove (inline, HD): clears the model data's texture-matrix animation word */
static inline void btk_remove(J3DModelData* d) {
    reinterpret_cast<bwd_detail::ModelDataAnimation*>(d)->mTextureMatrixAnimation = 0;
}

/* 020F9C1C */
static BOOL daBwd_Draw(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    fopAc_ac_c* actor = i_this;

    if (i_this->m3C1C >= 1) {
        if (i_this->m3C1C > 1) {
            mDoGph_setBlureRate((u8)i_this->m3C1C);
            mDoGph_onBlure();
        } else {
            i_this->m3C1C = 0;
            mDoGph_offBlure();
        }
    }

    J3DModel* model = i_this->mpHeadMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
    mDoExt_brkAnm* brk = i_this->mpHeadBrkAnm;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(model), brk->getFrame());
    i_this->mpHeadMorf->entryDL();
    if (i_this->m1BB6 < 2) {
        sita_s* psVar3 = i_this->mTongueSegments;
        for (int i = 0; i < 0x1E; i++, psVar3++) {
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, psVar3->m00, &actor->tevStr);
            mDoExt_modelUpdateDL(psVar3->m00);
        }
    }
    if (i_this->m170C != 0) {
        for (int i = 0; i < 20; i++) {
            model = i_this->mpBodyModel[i];
            if (model != nullptr) {
                setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
                mDoExt_brkAnm* b = i_this->mpBodyMorf[i];
                mDoExt_brkAnm_entry(b, J3DModel_getModelData(model), b->getFrame());
                mDoExt_modelUpdateDL(model);
            }
        }
    }
    dSnap_RegistFig(bwd_detail::DSNAP_TYPE_BWD, actor, 1.0f, 1.0f, 1.0f);

    /* gr_draw */
    {
        gabi::Local<cXyz> local_18;
        local_18->x = 0.0f;
        local_18->z = 0.0f;
        local_18->y = 0.0f;
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, local_18.get(), &i_this->m1714);
        if (i_this->m1865 == 0) {
            dComIfGd_setListBG();
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, i_this->mpTriforcePlatformModel, &i_this->m1714);
            mDoExt_modelUpdateDL(i_this->mpTriforcePlatformModel);
            dComIfGd_setList();
        }
    }
    /* suna_draw */
    for (int i = 0; i < 2; i++) {
        if (i_this->m17E4[i] != 0) {
            dScnKy_env_light_c* env = dKy_getEnvlight();
            setLightTevColorType(env, i_this->m17EC[i], &i_this->m1714);
            mDoExt_btkAnm* btk = i_this->m17F4[i];
            btk->entry(J3DModel_getModelData(i_this->m17EC[i]), btk->getFrame());
            mDoExt_modelUpdateDL(i_this->m17EC[i]);
            btk_remove(J3DModel_getModelData(i_this->m17EC[i]));
        }
    }
    for (int i = 0; i < 2; i++) {
        if (i_this->m3B08[i] != 0) {
            mDoExt_brkAnm* b = i_this->mpGspBrkAnm[i];
            mDoExt_brkAnm_entry(b, J3DModel_getModelData(i_this->mpGspMorf[i]->getModel()),
                                b->getFrame());
            mDoExt_btkAnm* btk = i_this->mpGspBtkAnm[i];
            btk->entry(J3DModel_getModelData(i_this->mpGspMorf[i]->getModel()), btk->getFrame());
            i_this->mpGspMorf[i]->updateDL();
        }
    }
    return TRUE;
}

/* 020F9F80 */
static void fly_pos_move(bwd_class* i_this, s16 param_2, s16 param_3) {
    gabi::AutoFrame frame{gabi::current_function()};
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_ec;
    gabi::Local<cXyz> cStack_f8;

    f32 temp_f30 = i_this->m18B4.x - actor->current.pos.x;
    f32 temp_f29 = i_this->m18B4.z - actor->current.pos.z;
    f32 temp_f31 = i_this->m18B4.y - actor->current.pos.y;
    s16 temp_r22 = param_3 + cM_atan2s(temp_f30, temp_f29);
    f32 var_f2 = std_sqrtf(gabi::fmadds(temp_f30, temp_f30, temp_f29 * temp_f29));
    s16 temp_r21 = param_2 - cM_atan2s(temp_f31, var_f2);
    s16 temp_r23 = actor->current.angle.y;
    cLib_addCalcAngleS2(&actor->current.angle.y, temp_r22, REG0_S(3) + 10,
                        (s16)gabi::ftoi(i_this->m18C4 * i_this->m18C8));
    s16 var_r4 = (s16)((temp_r23 - actor->current.angle.y) << 5);
    s16 temp_r0 = REG0_S(1) + 0x157c;
    if (var_r4 > temp_r0) {
        var_r4 = temp_r0;
    } else {
        int temp_r0_2 = -temp_r0;
        if (var_r4 < temp_r0_2) {
            var_r4 = temp_r0_2;
        }
    }
    cLib_addCalcAngleS2(&actor->current.angle.z, var_r4, REG0_S(3) + 10,
                        (s16)gabi::ftoi(i_this->m18C4 * i_this->m18C8 * 0.5f));
    cLib_addCalcAngleS2(&actor->current.angle.x, temp_r21, REG0_S(3) + 10,
                        (s16)gabi::ftoi(i_this->m18C4 * i_this->m18C8));
    cLib_addCalc2(&i_this->m18C8, 1.0f, 1.0f,
                  REG10_F(17) + 0.04f); /* HD: debug register added to the step */
    local_ec->x = 0.0f;
    local_ec->y = 0.0f;
    local_ec->z = actor->speedF;
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    cMtx_XrotM(calc_mtx(), actor->current.angle.x);
    MtxPosition(local_ec.get(), &actor->speed);
    actor->current.pos.x = actor->current.pos.x + actor->speed.x;
    actor->current.pos.y = actor->current.pos.y + actor->speed.y;
    actor->current.pos.z = actor->current.pos.z + actor->speed.z;
    if (i_this->m18FC == 0) {
        gabi::Local<dBgS_LinChk_bd> linChk;
        bd_LinChk_ct(linChk.get());
        s8 r21 = 0;
        local_ec->x = 0.0f;
        local_ec->y = 0.0f;
        local_ec->z = REG0_F(7) + 700.0f;
        MtxPosition(local_ec.get(), cStack_f8.get());
        PSVECAdd(cStack_f8.get(), &actor->current.pos, cStack_f8.get());
        dBgS_LinChk_Set(linChk.get(), &actor->current.pos, cStack_f8.get(), nullptr);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
            r21 = 1;
        }
        dBgS_LinChk_Set(linChk.get(), cStack_f8.get(), &actor->current.pos, nullptr);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
            r21 = 2;
        }
        if (r21 != 0) {
            i_this->m18FC = 0x1e;
            {
                gabi::Local<cXyz> up;
                dVibration_c* vib = dComIfGp_getVibration();
                s32 strength = REG0_S(2) + 7;
                up->z = 0.0f;
                up->y = 1.0f;
                up->x = 0.0f;
                bwdStartShock(vib, strength, -0x21, up.get()); /* StartShock(int, int, cXyz) */
            }
            if (i_this->m3C1E != 0) {
                i_this->m3C1C = 0xb4;
            }
            i_this->m3C4C = REG0_F(19) + 50.0f;
            if ((r21 == 1) && (i_this->m3AE0[0] == 0)) {
                s8 roomNo = actor->current.roomNo;
                i_this->m17C4 = 199;
                i_this->m3954.x = actor->current.pos.x;
                i_this->m3954.z = actor->current.pos.z;
                i_this->m17C8.copy(i_this->m3954);
                i_this->m3B0C[1].copy(i_this->m3954);
                i_this->m3C15 = 2;
                i_this->m3C14 = 0x5a;
                i_this->m3B08[1] = 1;
                dComIfGp_particle_setToon(bwd_detail::ID_IT_ST_BWO_DIVE_SMOKE00, &i_this->m3954,
                                          nullptr, nullptr, reinterpret_cast<GXColor*>(eff_col)->a,
                                          &i_this->m3978[0], roomNo);
                smoke_setColor(&i_this->m3978[0], eff_col);
                dComIfGp_particle_set(bwd_detail::ID_IT_SN_BWO_DIVE_SUNA00, &i_this->m3954, nullptr,
                                      nullptr, reinterpret_cast<GXColor*>(eff_col)->a,
                                      (dPa_levelEcallBack*)&i_this->m3AB8[0],
                                      actor->current.roomNo);
                mDoAud_seStart(bwd_detail::JA_SE_CM_BWD_IN_SAND_1, &i_this->m3954, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                i_this->m3AE0[0] = 0x6e;
            } else if (i_this->m3AE0[1] == 0) {
                s8 roomNo = actor->current.roomNo;
                i_this->m3954.x = actor->current.pos.x;
                i_this->m3954.z = actor->current.pos.z;
                i_this->m3B08[0] = 1;
                i_this->m3B0C[0].copy(i_this->m3954);
                i_this->m3C15 = 2;
                i_this->m3C14 = 0x5a;
                dComIfGp_particle_setToon(bwd_detail::ID_IT_ST_BWO_OUT_SMOKE00, &i_this->m3954,
                                          nullptr, nullptr, reinterpret_cast<GXColor*>(eff_col)->a,
                                          &i_this->m3978[1], roomNo);
                smoke_setColor(&i_this->m3978[1], eff_col);
                dComIfGp_particle_set(bwd_detail::ID_IT_SN_BWO_OUT_SUNA00, &i_this->m3954, nullptr,
                                      nullptr, reinterpret_cast<GXColor*>(eff_col)->a,
                                      (dPa_levelEcallBack*)&i_this->m3AB8[1],
                                      actor->current.roomNo);
                mDoAud_seStart(bwd_detail::JA_SE_CM_BWD_OUT_SAND_1, &i_this->m3954, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                i_this->m3AE0[1] = 0x6e;
            }
        }
        bd_LinChk_dt(linChk.get());
    }
}

/* 020FD570 */
static BOOL daBwd_IsDelete(bwd_class*) {
    gabi::AutoFrame frame{gabi::current_function()};
    return TRUE;
}

/* 020FD578 */
static BOOL daBwd_Delete(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    dComIfG_resDelete(&i_this->mPhaseBwd, "Bwd");
    dComIfG_resDelete(&i_this->mPhaseBwds, "Bwds");
    if (i_this->m3C51 != 0) {
        hio_set = 0;
        mDoHIO_deleteChild(l_HIO.mNo);
    }
    cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW2);
    cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW1[0]);
    cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW1[1]);
    for (int i = 0; i < 20; i++) {
        mDoAud_seDeleteObject(&i_this->m0418[i]);
    }
    for (int i = 0; i < 6; i++) {
        mDoAud_seDeleteObject(&suna_gr_pos()[i]);
    }
    mDoAud_seDeleteObject(center_pos);
    mDoAud_seDeleteObject(&i_this->m3954);
    for (int i = 0; i < 10; i++) {
        bd_vremove(&i_this->m3978[i]);
        if (i < 2) {
            bd_vremove(&i_this->m3AB8[i]);
        }
    }
    bd_vremove(&i_this->m3AF4);
    for (int i = 0; i < 6; i++) {
        bd_vremove(&i_this->m3B54[i]);
    }
    return TRUE;
}

/* J3DModel (HD): clear flag bit 0 at +0x74 and pass the flags to every material packet (027F596C)
 */
static inline void J3DModel_offFlag1(J3DModel* m) {
    auto* flags = reinterpret_cast<bwd_detail::ModelFlags*>(m);
    u32 f = flags->mFlags & ~1u;
    flags->mFlags = f;
    bwdSetMaterialFlags(m, f);
}
/* dBgW::SetCrrFunc(dBgS_MoveBGProc_Typical): the function pointer at +0xA8 */
static inline void dBgW_SetCrrFunc_Typical(dBgW* w) {
    reinterpret_cast<bwd_detail::Background*>(w)->mCorrectionCallback =
        bwd_detail::bwdTypicalCorrectionCallback;
}

/* 020FD6F8 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    bwd_class* i_this = (bwd_class*)a_this;
    J3DModelData* modelData;
    u16 fileIndex;

    {
        J3DModelData* bdl =
            (J3DModelData*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BDL_BWD_e, SAFESTRING_VTBL);
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(
            "Bwd", dRes_INDEX_BWD_BCK_BWD_WAIT_e, SAFESTRING_VTBL);
        i_this->mpHeadMorf =
            mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP,
                                   1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    }
    mDoExt_McaMorf* morf = i_this->mpHeadMorf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }
    i_this->m02CC = mDoExt_J3DModel__create(J3DModel_getModelData(morf->getModel()), 0, 0x11020203);
    if (i_this->m02CC == nullptr) {
        return FALSE;
    }
    {
        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mpHeadBrkAnm = (mDoExt_brkAnm*)brk;
        J3DModel* model = i_this->mpHeadMorf->getModel();
        void* res = dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BRK_BWD_e, SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpHeadBrkAnm, J3DModel_getModelData(model), res, 1,
                                J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    modelData =
        (J3DModelData*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BDL_BERO_e, SAFESTRING_VTBL);
    for (int i = 0; i < 30; i++) {
        if (i == 29) {
            modelData = (J3DModelData*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BDL_BERO_SAKI_e,
                                                            SAFESTRING_VTBL);
        }
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mTongueSegments[i].m00 = m;
        if (m == nullptr) {
            return FALSE;
        }
    }
    modelData = (J3DModelData*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BDL_BWD_SHIPPOA_e,
                                                    SAFESTRING_VTBL);
    fileIndex = dRes_INDEX_BWD_BRK_BWD_SHIPPOA_e;
    for (int i = 0; i < 20; i++) {
        if (i == 0x13) {
            modelData = (J3DModelData*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BDL_BWD_SHIPPOB_e,
                                                            SAFESTRING_VTBL);
            fileIndex = dRes_INDEX_BWD_BRK_BWD_SHIPPOB_e;
        }
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mpBodyModel[i] = m;
        if (m == nullptr) {
            return FALSE;
        }
        m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->m0324[i] = m;
        if (m == nullptr) {
            return FALSE;
        }
        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mpBodyMorf[i] = (mDoExt_brkAnm*)brk;
        void* res = dComIfG_getObjectRes("Bwd", fileIndex, SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpBodyMorf[i], modelData, res, 1, J3DFrameCtrl::EMode_NONE,
                                1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    modelData =
        (J3DModelData*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_BDL_HTRYF1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(4764, modelData != NULL) */
        JUT_ASSERT_fail("d_a_bwd.cpp", 0x129C, "modelData != NULL");
    {
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mpTriforcePlatformModel = m;
        if (m == nullptr) {
            return FALSE;
        }
    }
    {
        dBgW* w = new_dBgW();
        i_this->mpBgW2 = w;
        if (w == nullptr) {
            return FALSE;
        }
    }
    {
        cBgD_t* res =
            (cBgD_t*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_DZB_HTRYF1_e, SAFESTRING_VTBL);
        if (cBgW_Set(i_this->mpBgW2, res, cBgW_MOVE_BG_e, &i_this->mBgwMtx2)) {
            return FALSE;
        }
    }
    dBgW_SetCrrFunc_Typical(i_this->mpBgW2);
    for (int i = 0; i < 2; i++) {
        dBgW* w = new_dBgW();
        i_this->mpBgW1[i] = w;
        if (w == nullptr) {
            return FALSE;
        }
        cBgD_t* res =
            (cBgD_t*)dComIfG_getObjectRes("Bwd", dRes_INDEX_BWD_DZB_HTAKI1_e, SAFESTRING_VTBL);
        if (cBgW_Set(i_this->mpBgW1[i], res, cBgW_MOVE_BG_e, &i_this->mBgwMtx1[i])) {
            return FALSE;
        }
        dBgW_SetCrrFunc_Typical(i_this->mpBgW1[i]);
        modelData = (J3DModelData*)dComIfG_getObjectRes("Bwd", taki_bdl[i], SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(4805, modelData != NULL) */
            JUT_ASSERT_fail("d_a_bwd.cpp", 0x12C5, "modelData != NULL");
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->m17EC[i] = m;
        if (m == nullptr) {
            return FALSE;
        }
        J3DModel_offFlag1(m); /* HD */
        void* btk = operator_new(0x74);
        if (btk != nullptr)
            btk = mDoExt_btkAnm_ct(btk);
        i_this->m17F4[i] = (mDoExt_btkAnm*)btk;
        if (btk == nullptr) {
            return FALSE;
        }
        J3DAnmTextureSRTKey* key = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(
            "Bwd", dRes_INDEX_BWD_BTK_TAKI_START_e, SAFESTRING_VTBL);
        if (!i_this->m17F4[i]->init(modelData, key, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1,
                                    false, 0)) {
            return FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        J3DModelData* bdl = (J3DModelData*)dComIfG_getObjectRes("Bwd", s_bdl[i], SAFESTRING_VTBL);
        J3DAnmTransform* bck =
            (J3DAnmTransform*)dComIfG_getObjectRes("Bwd", s_bck[i], SAFESTRING_VTBL);
        mDoExt_McaMorf* m =
            mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_NONE,
                                   1.0f, 0, -1, 0, nullptr, 0, 0x11020203);
        i_this->mpGspMorf[i] = m;
        if (m == nullptr || m->getModel() == nullptr) {
            return FALSE;
        }
        modelData = J3DModel_getModelData(m->getModel());
        void* btk = operator_new(0x74);
        if (btk != nullptr)
            btk = mDoExt_btkAnm_ct(btk);
        i_this->mpGspBtkAnm[i] = (mDoExt_btkAnm*)btk;
        if (btk == nullptr) {
            return FALSE;
        }
        J3DAnmTextureSRTKey* key =
            (J3DAnmTextureSRTKey*)dComIfG_getObjectRes("Bwd", s_btk[i], SAFESTRING_VTBL);
        if (!i_this->mpGspBtkAnm[i]->init(modelData, key, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0,
                                          -1, false, 0)) {
            return FALSE;
        }
        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mpGspBrkAnm[i] = (mDoExt_brkAnm*)brk;
        void* res = dComIfG_getObjectRes("Bwd", s_brk[i], SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpGspBrkAnm[i], modelData, res, 1, J3DFrameCtrl::EMode_NONE,
                                1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    return TRUE;
}

/* 020FDE24 */
static void* smoke_ct_1(void* p) {
    gabi::AutoFrame frame{gabi::current_function()};
    return bwdConstructSmoke(p, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
}

/* 020FDE2C */
static void* follow_ct_0(void* p) {
    gabi::AutoFrame frame{gabi::current_function()};
    return bwdConstructFollow(p, 0, 0);
}

/* 020FDE38 */
static void* smoke_ct_1b(void* p) {
    gabi::AutoFrame frame{gabi::current_function()};
    return bwdConstructSmoke(p, 1);
}

/* dKy_tevstr_c (HD): three light blocks of 0x44 bytes at +0x00, +0xC0, +0x144 (floats 0..0x14,
 * bytes 0x18..0x1B, shorts 0x1C..0x22, floats 0x24..0x40) */
static inline void tev_light_copy(bwd_detail::LightBlock* dst, const bwd_detail::LightBlock* src) {
    for (int i = 0; i < 3; i++)
        dst->mPosition[i] = src->mPosition[i];
    for (int i = 0; i < 3; i++)
        dst->mDirection[i] = src->mDirection[i];
    for (int i = 0; i < 4; i++)
        dst->mColor[i] = src->mColor[i];
    for (int i = 0; i < 4; i++)
        dst->mParameters[i] = src->mParameters[i];
    for (int i = 0; i < 8; i++)
        dst->mFactors[i] = src->mFactors[i];
}
// HD: copy only the fields touched by GHS, in their original word/float/byte order.
static inline void tevstr_copy(dKy_tevstr_c* dst, const dKy_tevstr_c* src) {
    auto* d = reinterpret_cast<bwd_detail::TevCopyView*>(dst);
    auto* s = reinterpret_cast<const bwd_detail::TevCopyView*>(src);
    tev_light_copy(&d->mLight, &s->mLight);
    for (int i = 0; i < 3; i++)
        d->mLightPositionWords[i] = s->mLightPositionWords[i];
    for (int i = 0; i < 4; i++)
        d->mColorC0[i] = s->mColorC0[i];
    for (int i = 0; i < 8; i++)
        d->mColorsK[i] = s->mColorsK[i];
    for (int i = 0; i < 4; i++)
        d->mFogColor[i] = s->mFogColor[i];
    for (int i = 0; i < 3; i++)
        d->mFogAndBlend[i] = s->mFogAndBlend[i];
    for (int i = 0; i < 9; i++)
        d->mEnvironment[i] = s->mEnvironment[i];
    tev_light_copy(&d->mHdLight, &s->mHdLight);
    tev_light_copy(&d->mHdLight2, &s->mHdLight2);
}

/* 020FDE40 */
static bwd_class* bwd_class_ct(bwd_class* i_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (i_this == nullptr) {
        i_this = (bwd_class*)operator_new(0x3ECC);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BWD_VTBL;
    /* dKy_tevstr_c m1714: each light block starts as the default light (1016E414) */
    auto* lighting = reinterpret_cast<bwd_detail::TevCopyView*>(&i_this->m1714);
    tev_light_copy(&lighting->mLight, &defaultLight);
    tev_light_copy(&lighting->mHdLight, &defaultLight);
    tev_light_copy(&lighting->mHdLight2, &defaultLight);
    dCcD_Stts_ct(&i_this->mStts);
    __construct_array(i_this->mBodySph, 0x13, 0x12C, bwd_detail::bwdConstructSphereCallback);
    dCcD_Sph_ct(&i_this->mTongueSph);
    __construct_array(i_this->mTongueCoSph, 5, 0x12C, bwd_detail::bwdConstructSphereCallback);
    __construct_array(i_this->m3978, 10, 0x20, gabi::guest_fn((const void*)smoke_ct_1));
    __construct_array(i_this->m3AB8, 2, 0x14, gabi::guest_fn((const void*)follow_ct_0));
    dPa_followEcallBack_ct(&i_this->m3AF4, 0, 0);
    __construct_array(i_this->m3B54, 6, 0x20, gabi::guest_fn((const void*)smoke_ct_1b));
    return i_this;
}

static inline void mDoAud_bgmStart(u32 id) { bwdStartBgm(id); }

/* 020FE0B4 */
static cPhs_State daBwd_Create(fopAc_ac_c* a_this) {
    gabi::AutoFrame frame{gabi::current_function()};
    bwd_class* i_this = (bwd_class*)a_this;
    /* fopAcM_ct(a_this, bwd_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bwd_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhaseBwd, "Bwd");
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    cPhs_State res2 = dComIfG_resLoad(&i_this->mPhaseBwds, "Bwds");
    if (res2 != cPhs_COMPLEATE_e) {
        return res2;
    }
    i_this->m02BC = (u8)fopAcM_GetParam(a_this);
    if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void*)useHeapInit), 0x96000)) {
        return cPhs_ERROR_e;
    }
    if (dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW2, a_this)) {
        return cPhs_ERROR_e;
    }
    for (int i = 0; i < 2; i++) {
        if (dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW1[i], a_this)) {
            return cPhs_ERROR_e;
        }
    }
    a_this->attention_info.flags = 4;           /* fopAc_Attn_LOCKON_BATTLE_e */
    a_this->attention_info.distances[2] = 0x22; /* fopAc_Attn_TYPE_BATTLE_e */
    if (hio_set == 0) {
        i_this->m3C51 = 1;
        hio_set = 1;
        l_HIO.mNo = mDoHIO_createChild("Wind boss", &l_HIO); // HD: guest label is 風ボス.
    }
    a_this->max_health = 0xC;
    a_this->health = 0xC;
    i_this->mStts.Init(0xFA, 0, a_this);
    for (int i = 0; i < 17; i++) {
        i_this->mBodySph[i].SetStts(&i_this->mStts);
        i_this->mBodySph[i].Set(reinterpret_cast<dCcD_SrcSph*>(&body_sph_src));
    }
    i_this->mTongueSph.SetStts(&i_this->mStts);
    i_this->mTongueSph.Set(reinterpret_cast<dCcD_SrcSph*>(&bero_sph_src));
    for (int i = 0; i < 5; i++) {
        i_this->mTongueCoSph[i].SetStts(&i_this->mStts);
        i_this->mTongueCoSph[i].Set(reinterpret_cast<dCcD_SrcSph*>(&bero_co_sph_src));
    }
    if (!dComIfGs_isStageBossDemo() && dComIfGp_getStartStageName0() != 'X') {
        /* HD: starts at y = -5000 (GameCube -20000) */
        a_this->current.pos.x = 0.0f;
        a_this->current.pos.y = -5000.0f;
        a_this->current.pos.z = 0.0f;
        i_this->m18AE = 0;
        i_this->m17DC = 1.0f;
        i_this->m17E0 = 200.0f;
        a_this->actor_status |= 0x20; /* fopAcStts_SHOWMAP_e */
        i_this->m3C50 = 1;
        i_this->m1B88.y = -5000.0f;
    } else if (dComIfGs_isStageBossEnemy() && dComIfGp_getStartStageName0() != 'X') {
        i_this->m18AE = 0xD;
        a_this->current.pos.x = 0.0f;
        a_this->current.pos.y = -20000.0f;
        a_this->current.pos.z = 0.0f;
        i_this->m17E0 = REG0_F(11) + 400.0f;
        i_this->m3C15 = 5;
        i_this->m1B88.y = -20000.0f;
    } else {
        /* dComIfGs_offTmpBit(dSv_event_tmp_flag_c::UNK_0480) */
        dComIfGs_offTmpBit(0x480);
        a_this->actor_status |= 0x20; /* fopAcStts_SHOWMAP_e */
        i_this->m18AE = 1;
        i_this->m18B0 = -10;
        f32 fVar1 = REG0_F(17) + -1500.0f;
        a_this->current.pos.x = 0.0f;
        a_this->current.pos.y = fVar1;
        a_this->current.pos.z = 0.0f;
        i_this->m17E0 = 900.0f;
        i_this->m1865 = 1;
        i_this->m17E4[0] = 1;
        i_this->m17E4[1] = 1;
        i_this->m3C15 = 1;
        if (dComIfGp_getStartStageName0() == 'X') {
            mDoAud_bgmStart(bwd_detail::JA_BGM_PAST_RANE); /* JA_BGM_PAST_RANE */
        } else {
            mDoAud_bgmStart(bwd_detail::JA_BGM_RANE_BATTLE); /* JA_BGM_RANE_BATTLE */
        }
        i_this->m1B88.y = a_this->current.pos.y;
    }
    for (int i = 0; i < 30; i++) {
        f32 y = a_this->current.pos.y;
        i_this->mTongueSegments[i].m04.x = 0.0f;
        i_this->mTongueSegments[i].m04.y = y;
        i_this->mTongueSegments[i].m04.z = 0.0f;
    }
    reinterpret_cast<GXColor*>(eff_col)->r = 0xA0;
    reinterpret_cast<GXColor*>(eff_col)->g = 0xA0;
    reinterpret_cast<GXColor*>(eff_col)->b = 0x80;
    reinterpret_cast<GXColor*>(eff_col)->a = 0x96;
    tevstr_copy(&i_this->m1714, &a_this->tevStr);
    i_this->m1BC0 = fopAcM_create(bwd_detail::fpcNm_ATT_e, 0x65, &a_this->current.pos,
                                  fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
    return cPhs_COMPLEATE_e;
}

/* 020FE7BC */
static daBwd_HIO_c* daBwd_HIO_c_ct(daBwd_HIO_c* hio) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (hio == nullptr) {
        hio = (daBwd_HIO_c*)operator_new(0x40);
        if (hio == nullptr)
            return hio;
    }
    hio->mNo = -1;
    hio->m05 = 0;
    hio->m06 = 0;
    hio->m07 = 0;
    hio->m08 = 0;
    hio->m0C = -400.0f;
    hio->m10 = 6.0f;
    hio->m14 = 0x78;
    hio->m18 = 700.0f;
    hio->m1C = 3000.0f;
    hio->m20 = 1.3f;
    hio->m24 = 3;
    hio->m26 = 5;
    hio->m28 = 800.0f;
    hio->m2C = -500.0f;
    hio->m30 = 3.5f;
    hio->m34 = 2.0f;
    hio->m38 = 0x8C;
    hio->m3A = 0x78;
    hio->m3C = 0x50;
    hio->m3E = 0x96;
    hio->__vtbl = BWD_HIO_VTBL;
    return hio;
}

/* 020FE8B8 */
static void __sinit_d_a_bwd_cpp() {
    gabi::AutoFrame frame{gabi::current_function()};
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
    daBwd_HIO_c_ct(&l_HIO);
    constexpr f32 pos[6][2] = {{-3791.0f, -467.0f}, {2138.0f, -3176.0f}, {1655.0f, 3434.0f},
                               {-2405.0f, 2935.0f}, {3747.0f, 753.0f},   {-1393.0f, -3546.0f}};
    for (int i = 0; i < 6; i++) {
        suna_gr_pos()[i].x = pos[i][0];
        suna_gr_pos()[i].y = 0.0f;
        suna_gr_pos()[i].z = pos[i][1];
    }
    center_pos->x = 0.0f;
    center_pos->y = 0.0f;
    center_pos->z = 0.0f;
}

/* 020FEA1C */
static void daBwd_HIO_c_dt(void* p, s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02103894 */
static void follow_dt(void* p, s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021038A8 */
static void smoke_dt(void* p, s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021038BC */
static void bwd_class_dt(bwd_class* i_this, s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (i_this != nullptr) {
        __destroy_arr(i_this->m3B54, 6, 0x20, gabi::guest_fn((const void*)smoke_dt), 0, 0);
        __destroy_arr(i_this->m3AB8, 2, 0x14, gabi::guest_fn((const void*)follow_dt), 0, 0);
        __destroy_arr(i_this->m3978, 10, 0x20, gabi::guest_fn((const void*)smoke_dt), 0, 0);
        __destroy_arr(i_this->mTongueCoSph, 5, 0x12C, bwd_detail::bwdDestroySphereCallback, 0, 0);
        dCcD_Sph_dt(&i_this->mTongueSph, 2);
        __destroy_arr(i_this->mBodySph, 0x13, 0x12C, bwd_detail::bwdDestroySphereCallback, 0, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        bwdDestroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021039C8 */
static void daBwd_HIO_genMessage(void* self, void* ctx) {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* WWHD glue (tools/verify/autoglue.py): TU data and named bindings
data blurState 101F4825
data defaultLight 1016E414
data taki_bdl 10192FB4
data s_bdl 10192FBC
data s_btk 10192FC4
data s_brk 10192FCC
data s_bck 10192FD4
data body_sph_src 10192FDC
data bero_sph_src 1019301C
data bero_co_sph_src 1019305C
str 1000BECC in 020F9AF4
str 1000BFA4 in 020FD578
str 1000BFA8 in 020FD578
str 1000BFB0 in 020FD6F8
str 1000BFB4
str 1000BFC0 "modelData != NULL"
str 1000BFDC in 020FE0B4
str 1000BFE0 in 020FE0B4
str 1000BFE8 "Wind boss" renamed
ext 025F064C void bwdEnableBlur()
ext 025CB374 BOOL bwdStartShock(dVibration_c*, s32, s32, cXyz*)
ext 027F596C void bwdSetMaterialFlags(J3DModel*, u32)
ext 025A5B18 void* bwdConstructSmoke(void*, u8)
ext 025A5894 void* bwdConstructFollow(void*, u8, u8)
ext 025E18EC void bwdStartBgm(u32)
ext 025166F0 void bwdConstructSphere(void*)
ext 02515AE8 void bwdDestroySphere(void*, s32)
ext 025D50BC void bwdDestroyActor(fopAc_ac_c*, s32)
ext 024EE658 void bwdTypicalCorrection(dBgW*, void*, cBgS_PolyInfo*, bool, cXyz*, csXyz*, csXyz*)
*/
