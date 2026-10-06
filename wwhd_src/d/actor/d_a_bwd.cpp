/**
 * d_a_bwd.cpp (WWHD)
 * Boss - Molgera: everything but end, move, demo_camera and daBwd_Execute (d_a_bwd_*.cpp).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bwd.h"

/* guest string literals (each use has its own copy) */
#define STR_BWD_ANM STR(0x1000BECC)     /* "Bwd" (anm_init) */
#define STR_BWD_DELETE STR(0x1000BFA4)  /* "Bwd" (daBwd_Delete) */
#define STR_BWDS_DELETE STR(0x1000BFA8) /* "Bwds" (daBwd_Delete) */
#define STR_BWD_HEAP STR(0x1000BFB0)    /* "Bwd" (useHeapInit) */
#define STR_FILE STR(0x1000BFB4)        /* "d_a_bwd.cpp" */
#define STR_MODELDATA STR(0x1000BFC0)   /* "modelData != 0" */
#define STR_BWD_CREATE STR(0x1000BFDC)  /* "Bwd" (daBwd_Create) */
#define STR_BWDS_CREATE STR(0x1000BFE0) /* "Bwds" (daBwd_Create) */
#define STR_HIO_NAME STR(0x1000BFE8)    /* "風ボス" */
/* static tables (.data) */
static inline u32 taki_bdl(int i) { return gabi::load<u32>(0x10192FB4 + 4 * i); }
static inline u32 s_bdl(int i) { return gabi::load<u32>(0x10192FBC + 4 * i); }
static inline u32 s_btk(int i) { return gabi::load<u32>(0x10192FC4 + 4 * i); }
static inline u32 s_brk(int i) { return gabi::load<u32>(0x10192FCC + 4 * i); }
static inline u32 s_bck(int i) { return gabi::load<u32>(0x10192FD4 + 4 * i); }
#define body_sph_src gabi::at<dCcD_SrcSph>(0x10192FDC)
#define bero_sph_src gabi::at<dCcD_SrcSph>(0x1019301C)
#define bero_co_sph_src gabi::at<dCcD_SrcSph>(0x1019305C)

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
    WWHD_FUNC(0x020F99F8, void, i_this);
    if (i_this->m3960 != 0) {
        return;
    }
    i_this->m3960 = 1;
}
VERIFY(0x020F99F8, g_eff_on);

/* 020F9A10 */
static void g_eff_off(bwd_class* i_this) {
    WWHD_FUNC(0x020F9A10, void, i_this);
    if (i_this->m3960 == 0) {
        return;
    }
    i_this->m3960 = -1;
}
VERIFY(0x020F9A10, g_eff_off);

/* 020F9A28 (not named by the matcher) */
static void* ko_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x020F9A28, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xDA /* fpcNm_BWDS_e */) {
        if (ko_count < l_HIO.m26) {
            ko_ac()[ko_count] = gabi::ea(param_1);
            ko_count = ko_count + 1;
        }
        return nullptr;
    }
    return nullptr;
}
VERIFY(0x020F9A28, ko_s_sub);

/* 020F9AA0 */
static void* ko_delete_sub(void* param_1, void*) {
    WWHD_FUNC(0x020F9AA0, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xDA /* fpcNm_BWDS_e */) {
        gabi::store<s8>(gabi::ea(param_1) + 0x60C, 1); /* bwds_class::m04F4 */
    }
    return nullptr;
}
VERIFY(0x020F9AA0, ko_delete_sub);

/* 020F9AF4 */
void anm_init(bwd_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x020F9AF4, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWD_ANM, bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR_BWD_ANM, soundFileIdx, SAFESTRING_VTBL);
        i_this->mpHeadMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWD_ANM, bckFileIdx, SAFESTRING_VTBL);
        i_this->mpHeadMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x020F9AF4, anm_init);

/* mDoGph_gInf_c blur (HD): rate byte 0x101F4826, flag byte 0x101F4825 */
static inline void mDoGph_setBlureRate(u8 rate) { gabi::store<u8>(0x101F4826, rate); }
static inline void mDoGph_onBlure() { gabi::call(0x025F064C); }
static inline void mDoGph_offBlure() { gabi::store<u8>(0x101F4825, 0); }
/* mDoExt_btkAnm::remove (inline, HD): clears the model data's texture-matrix animation word */
static inline void btk_remove(J3DModelData* d) { gabi::store<u32>(gabi::ea(d) + 0x44, 0); }

/* 020F9C1C: gr_draw and suna_draw inlined */
static BOOL daBwd_Draw(bwd_class* i_this) {
    WWHD_FUNC(0x020F9C1C, BOOL, i_this);
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
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(model), gabi::load<f32>(gabi::ea(brk) + 4));
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
                mDoExt_brkAnm_entry(b, J3DModel_getModelData(model), gabi::load<f32>(gabi::ea(b) + 4));
                mDoExt_modelUpdateDL(model);
            }
        }
    }
    dSnap_RegistFig(0xCC /* DSNAP_TYPE_BWD */, actor, 1.0f, 1.0f, 1.0f);

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
            mDoExt_brkAnm_entry(b, J3DModel_getModelData(i_this->mpGspMorf[i]->getModel()), gabi::load<f32>(gabi::ea(b) + 4));
            mDoExt_btkAnm* btk = i_this->mpGspBtkAnm[i];
            btk->entry(J3DModel_getModelData(i_this->mpGspMorf[i]->getModel()), btk->getFrame());
            i_this->mpGspMorf[i]->updateDL();
        }
    }
    return TRUE;
}
VERIFY(0x020F9C1C, daBwd_Draw);

/* 020F9F80 */
static void fly_pos_move(bwd_class* i_this, s16 param_2, s16 param_3) {
    WWHD_FUNC(0x020F9F80, void, i_this, param_2, param_3);
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
    cLib_addCalcAngleS2(&actor->current.angle.y, temp_r22, REG0_S(3) + 10, (s16)gabi::ftoi(i_this->m18C4 * i_this->m18C8));
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
    cLib_addCalcAngleS2(&actor->current.angle.z, var_r4, REG0_S(3) + 10, (s16)gabi::ftoi(i_this->m18C4 * i_this->m18C8 * 0.5f));
    cLib_addCalcAngleS2(&actor->current.angle.x, temp_r21, REG0_S(3) + 10, (s16)gabi::ftoi(i_this->m18C4 * i_this->m18C8));
    cLib_addCalc2(&i_this->m18C8, 1.0f, 1.0f, REG10_F(17) + 0.04f); /* HD: debug register added to the step */
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
                gabi::call<BOOL>(0x025CB374, vib, strength, -0x21, up.get()); /* StartShock(int, int, cXyz) */
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
                dComIfGp_particle_setToon(0xA25A /* ID_IT_ST_BWO_DIVE_SMOKE00 */, &i_this->m3954, nullptr, nullptr,
                                          gabi::load<u8>(eff_col + 3), &i_this->m3978[0], roomNo);
                smoke_setColor(&i_this->m3978[0], eff_col);
                dComIfGp_particle_set(0x8259 /* ID_IT_SN_BWO_DIVE_SUNA00 */, &i_this->m3954, nullptr, nullptr,
                                      gabi::load<u8>(eff_col + 3), (dPa_levelEcallBack*)&i_this->m3AB8[0], actor->current.roomNo);
                mDoAud_seStart(0x5912 /* JA_SE_CM_BWD_IN_SAND_1 */, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                i_this->m3AE0[0] = 0x6e;
            } else if (i_this->m3AE0[1] == 0) {
                s8 roomNo = actor->current.roomNo;
                i_this->m3954.x = actor->current.pos.x;
                i_this->m3954.z = actor->current.pos.z;
                i_this->m3B08[0] = 1;
                i_this->m3B0C[0].copy(i_this->m3954);
                i_this->m3C15 = 2;
                i_this->m3C14 = 0x5a;
                dComIfGp_particle_setToon(0xA258 /* ID_IT_ST_BWO_OUT_SMOKE00 */, &i_this->m3954, nullptr, nullptr,
                                          gabi::load<u8>(eff_col + 3), &i_this->m3978[1], roomNo);
                smoke_setColor(&i_this->m3978[1], eff_col);
                dComIfGp_particle_set(0x8257 /* ID_IT_SN_BWO_OUT_SUNA00 */, &i_this->m3954, nullptr, nullptr,
                                      gabi::load<u8>(eff_col + 3), (dPa_levelEcallBack*)&i_this->m3AB8[1], actor->current.roomNo);
                mDoAud_seStart(0x590F /* JA_SE_CM_BWD_OUT_SAND_1 */, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                i_this->m3AE0[1] = 0x6e;
            }
        }
        bd_LinChk_dt(linChk.get());
    }
}
VERIFY(0x020F9F80, fly_pos_move);

/* 020FD570 */
static BOOL daBwd_IsDelete(bwd_class*) {
    WWHD_FUNC(0x020FD570, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020FD570, daBwd_IsDelete);

/* 020FD578 */
static BOOL daBwd_Delete(bwd_class* i_this) {
    WWHD_FUNC(0x020FD578, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhaseBwd, STR_BWD_DELETE);
    dComIfG_resDelete(&i_this->mPhaseBwds, STR_BWDS_DELETE);
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
VERIFY(0x020FD578, daBwd_Delete);

/* J3DModel (HD): clear flag bit 0 at +0x74 and pass the flags to every material packet (027F596C) */
static inline void J3DModel_offFlag1(J3DModel* m) {
    u32 f = gabi::load<u32>(gabi::ea(m) + 0x74) & ~1u;
    gabi::store<u32>(gabi::ea(m) + 0x74, f);
    gabi::call(0x027F596C, m, f);
}
/* dBgW::SetCrrFunc(dBgS_MoveBGProc_Typical): the function pointer at +0xA8 */
static inline void dBgW_SetCrrFunc_Typical(dBgW* w) { gabi::store<u32>(gabi::ea(w) + 0xA8, 0x024EE658); }

/* 020FD6F8 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020FD6F8, BOOL, a_this);
    bwd_class* i_this = (bwd_class*)a_this;
    J3DModelData* modelData;
    u16 fileIndex;

    {
        J3DModelData* bdl = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BDL_BWD_e, SAFESTRING_VTBL);
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BCK_BWD_WAIT_e, SAFESTRING_VTBL);
        i_this->mpHeadMorf = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                    nullptr, 0, 0x11020203);
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
        void* res = dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BRK_BWD_e, SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpHeadBrkAnm, J3DModel_getModelData(model), res, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1,
                                false, 0)) {
            return FALSE;
        }
    }
    modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BDL_BERO_e, SAFESTRING_VTBL);
    for (int i = 0; i < 30; i++) {
        if (i == 29) {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BDL_BERO_SAKI_e, SAFESTRING_VTBL);
        }
        J3DModel* m = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        i_this->mTongueSegments[i].m00 = m;
        if (m == nullptr) {
            return FALSE;
        }
    }
    modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BDL_BWD_SHIPPOA_e, SAFESTRING_VTBL);
    fileIndex = dRes_INDEX_BWD_BRK_BWD_SHIPPOA_e;
    for (int i = 0; i < 20; i++) {
        if (i == 0x13) {
            modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BDL_BWD_SHIPPOB_e, SAFESTRING_VTBL);
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
        void* res = dComIfG_getObjectRes(STR_BWD_HEAP, fileIndex, SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpBodyMorf[i], modelData, res, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BDL_HTRYF1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(4764, modelData != NULL) */
        JUT_ASSERT_fail(STR_FILE, 0x129C, STR_MODELDATA);
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
        cBgD_t* res = (cBgD_t*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_DZB_HTRYF1_e, SAFESTRING_VTBL);
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
        cBgD_t* res = (cBgD_t*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_DZB_HTAKI1_e, SAFESTRING_VTBL);
        if (cBgW_Set(i_this->mpBgW1[i], res, cBgW_MOVE_BG_e, &i_this->mBgwMtx1[i])) {
            return FALSE;
        }
        dBgW_SetCrrFunc_Typical(i_this->mpBgW1[i]);
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, taki_bdl(i), SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(4805, modelData != NULL) */
            JUT_ASSERT_fail(STR_FILE, 0x12C5, STR_MODELDATA);
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
        J3DAnmTextureSRTKey* key = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR_BWD_HEAP, dRes_INDEX_BWD_BTK_TAKI_START_e, SAFESTRING_VTBL);
        if (!i_this->m17F4[i]->init(modelData, key, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        J3DModelData* bdl = (J3DModelData*)dComIfG_getObjectRes(STR_BWD_HEAP, s_bdl(i), SAFESTRING_VTBL);
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR_BWD_HEAP, s_bck(i), SAFESTRING_VTBL);
        mDoExt_McaMorf* m = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, 0,
                                                   nullptr, 0, 0x11020203);
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
        J3DAnmTextureSRTKey* key = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(STR_BWD_HEAP, s_btk(i), SAFESTRING_VTBL);
        if (!i_this->mpGspBtkAnm[i]->init(modelData, key, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        i_this->mpGspBrkAnm[i] = (mDoExt_brkAnm*)brk;
        void* res = dComIfG_getObjectRes(STR_BWD_HEAP, s_brk(i), SAFESTRING_VTBL);
        if (!mDoExt_brkAnm_init(i_this->mpGspBrkAnm[i], modelData, res, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
            return FALSE;
        }
    }
    return TRUE;
}
VERIFY(0x020FD6F8, useHeapInit);

/* 020FDE24: element constructor for m3978[] (dPa_smokeEcallBack(1)) */
static void* smoke_ct_1(void* p) {
    WWHD_FUNC(0x020FDE24, void*, p);
    return gabi::call<void*>(0x025A5B18, p, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
}
VERIFY(0x020FDE24, smoke_ct_1);

/* 020FDE2C: element constructor for m3AB8[] (dPa_followEcallBack(0, 0)) */
static void* follow_ct_0(void* p) {
    WWHD_FUNC(0x020FDE2C, void*, p);
    return gabi::call<void*>(0x025A5894, p, 0, 0);
}
VERIFY(0x020FDE2C, follow_ct_0);

/* 020FDE38: element constructor for m3B54[] (dPa_smokeEcallBack(1)) */
static void* smoke_ct_1b(void* p) {
    WWHD_FUNC(0x020FDE38, void*, p);
    return gabi::call<void*>(0x025A5B18, p, 1);
}
VERIFY(0x020FDE38, smoke_ct_1b);

/* dKy_tevstr_c (HD): three light blocks of 0x44 bytes at +0x00, +0xC0, +0x144 (floats 0..0x14,
 * bytes 0x18..0x1B, shorts 0x1C..0x22, floats 0x24..0x40) */
static inline void tev_light_copy(u32 d, u32 s) {
    for (int k = 0; k < 0x18; k += 4) gabi::store<f32>(d + k, gabi::load<f32>(s + k));
    for (int k = 0x18; k < 0x1C; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    for (int k = 0x1C; k < 0x24; k += 2) gabi::store<s16>(d + k, gabi::load<s16>(s + k));
    for (int k = 0x24; k < 0x44; k += 4) gabi::store<f32>(d + k, gabi::load<f32>(s + k));
}
/* dKy_tevstr_c::operator= (HD, inline): the three light blocks and the members at 0x84..0xBC */
static inline void tevstr_copy(dKy_tevstr_c* dst, const dKy_tevstr_c* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    tev_light_copy(d, s);
    for (int k = 0x84; k < 0x90; k += 4) gabi::store<u32>(d + k, gabi::load<u32>(s + k));
    for (int k = 0x90; k < 0x98; k += 2) gabi::store<u16>(d + k, gabi::load<u16>(s + k));
    for (int k = 0x98; k < 0xA0; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    for (int k = 0xA0; k < 0xA8; k += 2) gabi::store<u16>(d + k, gabi::load<u16>(s + k));
    for (int k = 0xA8; k < 0xB4; k += 4) gabi::store<f32>(d + k, gabi::load<f32>(s + k));
    for (int k = 0xB4; k < 0xBD; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    tev_light_copy(d + 0xC0, s + 0xC0);
    tev_light_copy(d + 0x144, s + 0x144);
}

/* 020FDE40: bwd_class::bwd_class (HD: out of line; allocates when this == NULL) */
static bwd_class* bwd_class_ct(bwd_class* i_this) {
    WWHD_FUNC(0x020FDE40, bwd_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bwd_class*)operator_new(0x3ECC);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BWD_VTBL;
    /* dKy_tevstr_c m1714: each light block starts as the default light (1016E414) */
    u32 t = gabi::ea(&i_this->m1714);
    tev_light_copy(t, 0x1016E414);
    tev_light_copy(t + 0xC0, 0x1016E414);
    tev_light_copy(t + 0x144, 0x1016E414);
    dCcD_Stts_ct(&i_this->mStts);
    __construct_array(i_this->mBodySph, 0x13, 0x12C, 0x025166F0);
    dCcD_Sph_ct(&i_this->mTongueSph);
    __construct_array(i_this->mTongueCoSph, 5, 0x12C, 0x025166F0);
    __construct_array(i_this->m3978, 10, 0x20, 0x020FDE24);
    __construct_array(i_this->m3AB8, 2, 0x14, 0x020FDE2C);
    dPa_followEcallBack_ct(&i_this->m3AF4, 0, 0);
    __construct_array(i_this->m3B54, 6, 0x20, 0x020FDE38);
    return i_this;
}
VERIFY(0x020FDE40, bwd_class_ct);

/* dSv_memBit_c::isDungeonItem (HD: the save's memory bits at info+0x798) */
static inline BOOL dComIfGs_isDungeonItem(s32 i) { return gabi::call<BOOL>(0x025B9100, gabi::load<u32>(0x101F84DC) + 0x798, i); }
static inline BOOL dComIfGs_isStageBossDemo() { return dComIfGs_isDungeonItem(5); }
static inline BOOL dComIfGs_isStageBossEnemy() { return dComIfGs_isDungeonItem(3); }
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }

/* 020FE0B4 */
static cPhs_State daBwd_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020FE0B4, cPhs_State, a_this);
    bwd_class* i_this = (bwd_class*)a_this;
    /* fopAcM_ct(a_this, bwd_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bwd_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhaseBwd, STR_BWD_CREATE);
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    cPhs_State res2 = dComIfG_resLoad(&i_this->mPhaseBwds, STR_BWDS_CREATE);
    if (res2 != cPhs_COMPLEATE_e) {
        return res2;
    }
    i_this->m02BC = (u8)fopAcM_GetParam(a_this);
    if (!fopAcM_entrySolidHeap(a_this, 0x020FD6F8 /* useHeapInit */, 0x96000)) {
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
    attn_flags(a_this) = 4; /* fopAc_Attn_LOCKON_BATTLE_e */
    gabi::store<u8>(gabi::ea(a_this) + 0x38A, 0x22); /* attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] */
    if (hio_set == 0) {
        i_this->m3C51 = 1;
        hio_set = 1;
        l_HIO.mNo = mDoHIO_createChild(STR_HIO_NAME, &l_HIO);
    }
    a_this->max_health = 0xC;
    a_this->health = 0xC;
    i_this->mStts.Init(0xFA, 0, a_this);
    for (int i = 0; i < 17; i++) {
        i_this->mBodySph[i].SetStts(&i_this->mStts);
        i_this->mBodySph[i].Set(body_sph_src);
    }
    i_this->mTongueSph.SetStts(&i_this->mStts);
    i_this->mTongueSph.Set(bero_sph_src);
    for (int i = 0; i < 5; i++) {
        i_this->mTongueCoSph[i].SetStts(&i_this->mStts);
        i_this->mTongueCoSph[i].Set(bero_co_sph_src);
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
        gabi::call(0x025B8B7C, gabi::load<u32>(0x101F84DC) + 0x1178, 0x480);
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
            mDoAud_bgmStart(0x8000004C); /* JA_BGM_PAST_RANE */
        } else {
            mDoAud_bgmStart(0x80000029); /* JA_BGM_RANE_BATTLE */
        }
        i_this->m1B88.y = a_this->current.pos.y;
    }
    for (int i = 0; i < 30; i++) {
        f32 y = a_this->current.pos.y;
        i_this->mTongueSegments[i].m04.x = 0.0f;
        i_this->mTongueSegments[i].m04.y = y;
        i_this->mTongueSegments[i].m04.z = 0.0f;
    }
    gabi::store<u8>(eff_col + 0, 0xA0);
    gabi::store<u8>(eff_col + 1, 0xA0);
    gabi::store<u8>(eff_col + 2, 0x80);
    gabi::store<u8>(eff_col + 3, 0x96);
    tevstr_copy(&i_this->m1714, &a_this->tevStr);
    i_this->m1BC0 = fopAcM_create(0x1C0 /* fpcNm_ATT_e */, 0x65, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x020FE0B4, daBwd_Create);

/* 020FE7BC */
static daBwd_HIO_c* daBwd_HIO_c_ct(daBwd_HIO_c* hio) {
    WWHD_FUNC(0x020FE7BC, daBwd_HIO_c*, hio);
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
VERIFY(0x020FE7BC, daBwd_HIO_c_ct);

/* 020FE8B8: header statics, l_HIO's constructor, suna_gr_pos[6] and center_pos */
static void __sinit_d_a_bwd_cpp() {
    WWHD_FUNC(0x020FE8B8, void, (u32)0);
    sinit_header_statics_z(0x10462A5C, 0x1019309C, 0x10462AA8);
    daBwd_HIO_c_ct(&l_HIO);
    static const f32 pos[6][2] = {{-3791.0f, -467.0f}, {2138.0f, -3176.0f}, {1655.0f, 3434.0f},
                                  {-2405.0f, 2935.0f}, {3747.0f, 753.0f}, {-1393.0f, -3546.0f}};
    for (int i = 0; i < 6; i++) {
        suna_gr_pos()[i].x = pos[i][0];
        suna_gr_pos()[i].y = 0.0f;
        suna_gr_pos()[i].z = pos[i][1];
    }
    center_pos->x = 0.0f;
    center_pos->y = 0.0f;
    center_pos->z = 0.0f;
}
VERIFY(0x020FE8B8, __sinit_d_a_bwd_cpp);

/* 020FEA1C: daBwd_HIO_c deleting destructor (trivial) */
static void daBwd_HIO_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020FEA1C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020FEA1C, daBwd_HIO_c_dt);

/* 02103894: element destructor for m3AB8[] (dPa_followEcallBack: trivial) */
static void follow_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02103894, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02103894, follow_dt);

/* 021038A8: element destructor for m3978[] / m3B54[] (dPa_smokeEcallBack: trivial) */
static void smoke_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021038A8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021038A8, smoke_dt);

/* 021038BC: bwd_class deleting destructor (compiler-generated) */
static void bwd_class_dt(bwd_class* i_this, s32 flags) {
    WWHD_FUNC(0x021038BC, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(i_this->m3B54, 6, 0x20, 0x021038A8, 0, 0);
        __destroy_arr(i_this->m3AB8, 2, 0x14, 0x02103894, 0, 0);
        __destroy_arr(i_this->m3978, 10, 0x20, 0x021038A8, 0, 0);
        __destroy_arr(i_this->mTongueCoSph, 5, 0x12C, 0x02515AE8, 0, 0);
        dCcD_Sph_dt(&i_this->mTongueSph, 2);
        __destroy_arr(i_this->mBodySph, 0x13, 0x12C, 0x02515AE8, 0, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021038BC, bwd_class_dt);

/* 021039C8: daBwd_HIO_c::genMessage(JORMContext*) (empty virtual, HD) */
static void daBwd_HIO_genMessage(void* self, void* ctx) {
    WWHD_FUNC(0x021039C8, void, self, ctx);
}
VERIFY(0x021039C8, daBwd_HIO_genMessage);
