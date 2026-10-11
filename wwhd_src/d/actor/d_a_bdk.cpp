/**
 * d_a_bdk.cpp (WWHD)
 * Boss - Helmaroc King (battle): the search callbacks, small helpers, anm_init, nodeCallBack,
 * the compiler-generated functions and the HIO. The larger functions are in d_a_bdk_*.cpp.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk_base.h"

static cXyz center_pos;
static cXyz center_pos2;
static BdkBaseFootPositions foot_eff_pos;
static daBdk_HIO_c l_HIO;
static BdkBaseLight l_defaultLight;
static BdkBaseMaskModels kamen_break_bdl;
static BdkBaseJointHits search_data;
static dCcD_SrcSph head_at_sph_src;
static dCcD_SrcSph head_tg_sph_src;
static dCcD_SrcSph tosaka_tg_sph_src;
static dCcD_SrcSph body_cc_sph_src;
static dCcD_SrcSph foot_cc_sph_src;
static dCcD_SrcSph wind_at_sph_src;
static dCcD_SrcSph kamen_sph_src;
static dCcD_SrcSph eff_sph_src;

static bdk_class *bdk_class_ct(bdk_class *self);

/* 02064CF4 */
static void *kamome_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_KAMOME_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 02064D48 */
static void *kui_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_KUI_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 02064D9C */
static void *ep_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_EP_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 02064DF0 */
static void *dk_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_DK_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 02064E44 */
static void *obj_delete_sub(void *param_1, void *) {
    fopAc_ac_c *actor = (fopAc_ac_c *)param_1;
    if (fopAcM_IsActor(actor) && actor != nullptr && actor->mProcName == fpcNm_BDKOBJ_e &&
        (fopAcM_GetParam(actor) & 0xF) >= 2) {
        fopAcM_delete(actor);
    }
    return nullptr;
}

/* 02064EA8 (HD only; t_down): the bdkobj's falling fragments (state 2) get a random spin and a
 * horizontal push of 10 away from the boss */
/* 02064EA8 */
static void *obj_hahen_sub(void *param_1, void *param_2) {
    fopAc_ac_c *actor = (fopAc_ac_c *)param_1;
    fopAc_ac_c *bdk = (fopAc_ac_c *)param_2;
    if (fopAcM_IsActor(actor) && actor != nullptr && actor->mProcName == fpcNm_BDKOBJ_e &&
        BdkBase_parameterByte(actor) == 0xF) {
        gabi::Local<cXyz> local_1;
        gabi::Local<cXyz> local_2;
        BdkBaseFragment *eff = reinterpret_cast<BdkBaseObject *>(actor)->mEffs;
        for (int i = 0; i < 3; i++, eff++) {
            if (eff->m000 == 2) {
                eff->m036.y = (s16)gabi::ftoi(cM_rndF(500.0f) + 500.0f);
                f32 r = cM_rndF(1.0f);
                f32 x = eff->m008.x;
                if (r < 0.5f) {
                    eff->m036.y = -eff->m036.y;
                }
                cMtx_YrotS(calc_mtx(), cM_atan2s(x - bdk->current.pos.x, eff->m008.z - bdk->current.pos.z));
                local_1->x = 0.0f;
                local_1->y = 0.0f;
                local_1->z = 10.0f;
                MtxPosition(local_1, local_2);
                BdkBase_copyFloat(eff->m024x, local_2->x);
                BdkBase_copyFloat(eff->m024z, local_2->z);
            }
        }
    }
    return nullptr;
}

/* 0206505C */
static void *sea_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_Obj_Tide_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 020650B0 */
static void *bk_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_BK_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 02065104 */
static void *boko_delete_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == fpcNm_BOKO_e) {
        fopAcM_delete((fopAc_ac_c *)param_1);
    }
    return nullptr;
}

/* 02065158 */
static void *obj2_delete_sub(void *param_1, void *) {
    fopAc_ac_c *actor = (fopAc_ac_c *)param_1;
    if (fopAcM_IsActor(actor) && actor != nullptr && actor->mProcName == fpcNm_BDKOBJ_e &&
        actor->model == 0) {
        fopAcM_delete(actor);
    }
    return nullptr;
}

/* (HD only; demo_camera): finds the actor with process name 0x1BE */
/* 020651B8 */
static void *s_1BE_sub(void *param_1, void *) {
    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c *>(param_1)->mProcName == BdkBaseProcess_DemoActor) {
        return param_1;
    }
    return nullptr;
}

/* 02065208 */
static BOOL land_area_check(cXyz *param_1, f32 param_2) {
    f32 val2 = param_1->z - -3800.0f;
    f32 val1 = param_1->x - 3600.0f;
    if (std_sqrtf(gabi::fmadds(val1, val1, val2 * val2)) < param_2) {
        return TRUE;
    }
    return FALSE;
}

/* 020669B8 */
static void *obj_s_sub(void *param_1, void *) {
    fopAc_ac_c *actor = (fopAc_ac_c *)param_1;
    if (fopAcM_IsActor(actor) && actor != nullptr && actor->mProcName == fpcNm_BDKOBJ_e &&
        (fopAcM_GetParam(actor) & 0xF) != 0xF && actor->health != 0) {
        return param_1;
    }
    return nullptr;
}

/* 0206D638 */
static void after_fight(bdk_class *i_this) { i_this->m2F8 = 10; }

/* 020692D8 */
static BOOL daBdk_IsDelete(bdk_class *) { return TRUE; }

/* 020692E0 */
static BOOL daBdk_Delete(bdk_class *i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Bdk");
    mDoHIO_deleteChild(l_HIO.mNo);
    fopAc_ac_c *actor = i_this;
    if (actor->heap.get() != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->pm_bgw);
        cBgS_Release(dComIfG_Bgsp(), i_this->mp63BC[0]);
        cBgS_Release(dComIfG_Bgsp(), i_this->mp63BC[1]);
        cBgS_Release(dComIfG_Bgsp(), i_this->mp63BC[2]);
        i_this->mpMorf->stopZelAnime();
    }
    for (s32 i = 0; i < 4; i++) {
        mDoAud_seDeleteObject(&i_this->m910[i]);
    }
    mDoAud_seDeleteObject(&center_pos);
    mDoAud_seDeleteObject(&wind_se_pos);
    mDoAud_seDeleteObject(&i_this->m2CC);
    for (s32 i = 0; i < 4; i++) {
        BdkBase_remove(&i_this->m6080[i]);
        BdkBase_remove(&i_this->m6130[i]);
    }
    BdkBase_remove(&i_this->m6110);
    BdkBase_remove(&i_this->m61B0);
    return TRUE;
}

#define STR_BDK_HEAP "Bdk"
#define STR_FILE "d_a_bdk.cpp"
#define STR_MODELDATA "modelData != (0)"
#define STR_PM_BGW "i_this->pm_bgw != (0)"
static inline void *bdk_res(s32 idx) { return dComIfG_getObjectRes(STR_BDK_HEAP, idx); }
// HD: no shadow model, no mask visibility animation.
/* 02069428 */
static BOOL useHeapInit(fopAc_ac_c *i_actor) {
    bdk_class *i_this = (bdk_class *)i_actor;
    J3DModelData *bdl = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_DK_e);
    J3DAnmTransform *bck = (J3DAnmTransform *)bdk_res(dRes_INDEX_BDK_BCK_FLY1_e);
    void *bas = bdk_res(dRes_INDEX_BDK_BAS_FLY1_e);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, bdl, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP,
                                            1.0f, 0, -1, 1, bas, 0x80000, 0x11000022);
    if (i_this->mpMorf.get() == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    for (u16 i = 0; i < i_this->mpMorf->getModel()->getModelData()->getJointNum(); i++) {
        if (i == DK_JNT_J_DK_KUBI1_e || (u32)(i - DK_JNT_J_DK_TOSAKA_A1_e) < 6 || i == DK_JNT_J_DK_O_LA2_e ||
            i == DK_JNT_J_DK_O_LB2_e || i == DK_JNT_J_DK_O_RA2_e || i == DK_JNT_J_DK_O_RB2_e) {
            i_this->mpMorf->getModel()->getModelData()->getJointNodePointer(i)->mCallBack =
                BdkBaseEntries::nodeCallBack;
        }
    }
    i_this->mpMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(i_this));

    J3DModelData *modelData = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_DK_KAMEN4_e);
    i_this->mp8F0 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (i_this->mp8F0.get() == nullptr) {
        return FALSE;
    }

    for (s32 i = 0; i < 4; i++) {
        u16 idx = kamen_break_bdl.mModels[i];
        i_this->m8FC[i] = mDoExt_J3DModel__create((J3DModelData *)bdk_res(idx), 0x80000, 0x11000022);
        if (i_this->m8FC[i].get() == nullptr) {
            return FALSE;
        }
    }

    modelData = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_DK_TAIL_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B0A, STR_MODELDATA);
    }
    for (int i = 0; i < 4; i++) {
        for (s32 j = 0; j < 9; j++) {
            i_this->m300[i].m000[j] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (i_this->m300[i].m000[j].get() == nullptr) {
                return FALSE;
            }
        }
    }

    for (s32 i = 0; i < 0x28; i++) {
        if (i == 0x27) {
            modelData = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_GROCK00_e);
        } else if (i < 0x27) {
            modelData = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_GHANE00_e);
        }
        if (modelData == nullptr) {
            JUT_ASSERT_fail(STR_FILE, 0x1B2E, STR_MODELDATA);
        }
        i_this->m261C[i].m044 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
        if (i_this->m261C[i].m044.get() == nullptr) {
            return FALSE;
        }
    }

    modelData = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_S_TSHUTTER_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B3B, STR_MODELDATA);
    }
    for (s32 i = 0; i < 3; i++) {
        i_this->mp6310[i] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
        if (i_this->mp6310[i].get() == nullptr) {
            return FALSE;
        }
        i_this->mp63BC[i] = new_dBgW();
        if (i_this->mp63BC[i].get() == nullptr) {
            return FALSE;
        }
        cBgD_t *dzb = (cBgD_t *)bdk_res(dRes_INDEX_BDK_DZB_S_TSHUTTER_e);
        if (cBgW_Set(i_this->mp63BC[i], dzb, cBgW_MOVE_BG_e, &i_this->m632C[i])) {
            return FALSE;
        }
        BdkBase_setCrrFunc(i_this->mp63BC[i], BdkBaseEntries::moveBgTypical);
    }

    modelData = (J3DModelData *)bdk_res(dRes_INDEX_BDK_BDL_S_TTOGE_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B56, STR_MODELDATA);
    }
    i_this->mp62D8 = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (i_this->mp62D8.get() == nullptr) {
        return FALSE;
    }

    i_this->pm_bgw = new_dBgW();
    if (i_this->pm_bgw.get() == nullptr) {
        JUT_ASSERT_fail(STR_FILE, 0x1B60, STR_PM_BGW);
    }
    cBgD_t *dzb = (cBgD_t *)bdk_res(dRes_INDEX_BDK_DZB_S_TTOGE_e);
    cBgW_Set(i_this->pm_bgw, dzb, cBgW_MOVE_BG_e, &i_this->m62DC);
    BdkBase_setCrrFunc(i_this->pm_bgw, BdkBaseEntries::moveBgTypical);

    i_this->mp63C8 =
        BdkBase_createJntHit(i_this->mpMorf->getModel(), search_data.mEntries, 0xC); /* JntHit_create */
    if (i_this->mp63C8 != 0) {
        i_actor->jntHit = i_this->mp63C8;
    }
    return TRUE;
}

/* 02069C8C */
static cPhs_State daBdk_Create(fopAc_ac_c *a_this) {
    bdk_class *i_this = (bdk_class *)a_this;
    fopAc_ac_c *actor = a_this;
    /* fopAcM_ct(a_this, bdk_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            bdk_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhase, "Bdk");
    if (phase_state == cPhs_COMPLEATE_e) {
        for (s32 i = 0; i < 4; i++) {
            BdkBase_setFollowOff(&i_this->m6080[i]);
        }
        for (s32 i = 0; i < 3; i++) {
            BdkBase_setFollowOff(&i_this->m6130[i]);
        }
        i_this->m2B4 = 1;
        BdkBase_setFollowOff(&i_this->m6110);

        if (!fopAcM_entrySolidHeap(actor, gabi::guest_fn(reinterpret_cast<const void *>(useHeapInit)),
                                   0x96000)) {
            return cPhs_ERROR_e;
        }
        if (i_this->pm_bgw.get() != nullptr) {
            dBgS *bgs = dComIfG_Bgsp();
            if (dBgS_Regist(bgs, i_this->pm_bgw, actor)) {
                return cPhs_ERROR_e;
            }
        }
        for (s32 i = 0; i < 3; i++) {
            if (i_this->mp63BC[i].get() != nullptr) {
                dBgS *bgs = dComIfG_Bgsp();
                if (dBgS_Regist(bgs, i_this->mp63BC[i], actor)) {
                    return cPhs_ERROR_e;
                }
            }
        }

        actor->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        actor->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0x2A;
        l_HIO.mNo = mDoHIO_createChild("戦闘用大怪鳥", &l_HIO);
        i_this->mAcch.Set(&actor->current.pos, &actor->old.pos, actor, 1, &i_this->mAcchCir, &actor->speed);
        i_this->mAcch.mSameActorChk = 0;
        i_this->mAcchCir.SetWall(400.0f, 500.0f);
        i_this->mStts.Init(0xFF, 0xFF, actor);

        i_this->mHeadAtSph.Set(&head_at_sph_src);
        i_this->mHeadTgSph.Set(&head_tg_sph_src);
        i_this->mBodyCCSph.Set(&body_cc_sph_src);
        i_this->mFootCCSph[0].Set(&foot_cc_sph_src);
        i_this->mFootCCSph[1].Set(&foot_cc_sph_src);
        i_this->mHeadAtSph.SetStts(&i_this->mStts);
        i_this->mHeadTgSph.SetStts(&i_this->mStts);
        i_this->mBodyCCSph.SetStts(&i_this->mStts);
        i_this->mBodyCCSph.mGObjTg.mSPrm |= 4; // OnTgNoHitMark
        i_this->mFootCCSph[0].SetStts(&i_this->mStts);
        i_this->mFootCCSph[1].SetStts(&i_this->mStts);

        i_this->mTosakaTgSph.Set(&tosaka_tg_sph_src);
        i_this->mTosakaTgSph.SetStts(&i_this->mStts);

        for (s32 i = 0; i < 10; i++) {
            i_this->mWindAtSph[i].Set(&wind_at_sph_src);
            i_this->mWindAtSph[i].SetStts(&i_this->mStts);
        }
        i_this->mA14.Init(0xFF, 0xFF, nullptr);
        for (s32 i = 0; i < 4; i++) {
            i_this->mA50[i].Set(&kamen_sph_src);
            i_this->mA50[i].SetStts(&i_this->mA14);
        }
        i_this->m603C.Init(0xFF, 0xFF, nullptr);
        for (s32 i = 0; i < 0x28; i++) {
            i_this->m261C[i].m048.Set(&eff_sph_src);
            i_this->m261C[i].m048.SetStts(&i_this->m603C);
        }
        center_pos.x = 3600.0f;
        center_pos.y = 9800.0f;
        center_pos.z = -3800.0f;
        center_pos2.x = 3600.0f;
        center_pos2.y = 9720.0f;
        center_pos2.z = -3800.0f;

        if (dComIfGs_isStageBossEnemy() || REG0_S(0) != 0) {
            BdkBase_onDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::STAGE_BOSS_ENEMY);
            i_this->mAction = ACTION_AFTER_FIGHT;
            actor->current.pos.x = 300000.0f;
            actor->current.pos.y = 300000.0f;
            actor->current.pos.z = 300000.0f;
            i_this->m6320 = -630.0f;
            i_this->m6324 = -70.0f;
            i_this->m62D4 = -500.0f;
        } else if (i_this->m2B4 == 1) {
            i_this->mAction = ACTION_T_FLY;
            actor->current.pos.copy(center_pos);
            actor->current.pos.y = 5000.0f;
            i_this->m6320 = -630.0f;
            i_this->m6324 = -70.0f;
            mDoAud_bgmStart(JA_BGM_MJ_TOWER_BATTLE);
            for (s32 i = 0; i < 10; i++) {
                i_this->mWindAtSph[i].SetAtType(AT_TYPE_UNK400000);
            }
            i_this->m2EC[0] = 0x168;
        } else {
            actor->gravity = REG0_F(4) + -5.0f;
            actor->speedF = (f32)l_HIO.m020;
            i_this->m2CA = -1;
            i_this->m1120 = -2000;
            actor->health = 20;
            actor->max_health = 20;
            actor->current.pos.y = 11000.0f;
        }
    }
    BdkBase_copyTev(&i_this->m6224, &actor->tevStr);
    return phase_state;
}

// HD: compiler-generated and template code of this translation unit.

// HD: allocates when this is NULL; the vtable is the last member.
/* 0206A43C */
static daBdk_HIO_c *daBdk_HIO_c_ct(daBdk_HIO_c *self) {
    if (self == nullptr) {
        self = (daBdk_HIO_c *)operator_new(sizeof(daBdk_HIO_c));
        if (self == nullptr)
            return self;
    }
    self->m014 = 0;
    self->m005 = 0;
    self->m020 = 60.0f;
    self->m006 = 0;
    self->m008 = 1.0f;
    self->m018 = 20.0f;
    self->__vtbl = DABDK_HIO_VTBL;
    self->m028 = 0x2D0;
    self->m010 = 1.5f;
    self->m00C = 4.0f;
    self->m01C = 2.0f;
    self->m024 = 100.0f;
    self->mNo = -1;
    return self;
}

/* static initialisation (header statics, then l_HIO) */
/* 0206A4F0 */
static void __sinit_d_a_bdk_cpp() {
    /* the header statics (this unit places the two initialised objects at +0x10/+0x11) */
    for (int i = 0; i < 4; i++)
        BdkBase_headerStatics().mZero[i] = 0;
    BdkBase_registerObject(&BdkBase_descriptors()[0]);
    BdkBase_headerStatics().mNegativePi = -3.1415927f;
    BdkBase_headerStatics().mPositivePi = 3.1415927f;
    BdkBase_firstHeaderObject(&BdkBase_headerStatics().mFirstObject);
    BdkBase_registerObject(&BdkBase_descriptors()[1]);
    BdkBase_secondHeaderObject(&BdkBase_headerStatics().mSecondObject);
    BdkBase_registerObject(&BdkBase_descriptors()[2]);
    daBdk_HIO_c_ct(&l_HIO);
}

/* (not named by the matcher): fopAcM_seStart on current.pos, out of line (HD: null checks) */
/* 0206A590 */
static void fopAcM_seStartCurrent(fopAc_ac_c *actor, u32 sfx, u32 param) {
    if (actor != nullptr && gabi::ea(&actor->current.pos) != 0) {
        mDoAud_seStart(sfx, &actor->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
}

// HD: sead::SafeString deleting destructor.
/* 0206A5FC */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: empty sead::SafeString::assureTerminationImpl_.
/* 02072884 */
static void SafeString_assureTermination(void *) {}

/* (out of line): JPABaseEmitter status flags (HD +0x254) |= flag */
/* 0206BA98 */
static void JPABaseEmitter_setStatus(JPABaseEmitter *e, u32 flag) { BdkBase_emitterFlags(e) |= flag; }

/* (out of line): JGeometry::TVec3<f32>::set(x, y, z) */
/* 020725FC */
static void TVec3_set(cXyz *v, f32 x, f32 y, f32 z) {
    v->y = y;
    v->z = z;
    v->x = x;
}

// HD: array element constructors for the two smoke callback arrays.
/* 02069948 */
static u32 smokeEcallBack_ct_a(void *p) { return BdkBase_smokeCtor(p, (u8)1); }

/* 02069950 */
static u32 smokeEcallBack_ct_b(void *p) { return BdkBase_smokeCtor(p, (u8)1); }

/* dPa_followEcallBack array element constructor (__construct_array) */
/* 02069958 */
static u32 followEcallBack_ct(void *p) { return BdkBase_followCtor(p, (u8)0, (u8)0); }

/* bdk_eff_s::bdk_eff_s (HD: allocates when this == NULL) */
/* 0207260C */
static bdk_eff_s *bdk_eff_s_ct(bdk_eff_s *self) {
    if (self == nullptr) {
        self = (bdk_eff_s *)operator_new(sizeof(bdk_eff_s));
        if (self == nullptr)
            return self;
    }
    BdkBase_sphereCtor(&self->m048); /* dCcD_Sph::dCcD_Sph */
    return self;
}

/* dPa_followEcallBack array element deleting destructor (__destroy_arr) */
/* 02072654 */
static void followEcallBack_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* dPa_smokeEcallBack array element deleting destructor (__destroy_arr) */
/* 02072668 */
static void smokeEcallBack_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* bdk_eff_s deleting destructor */
/* 0207267C */
static void bdk_eff_s_dt(bdk_eff_s *self, s32 flags) {
    if (self == nullptr)
        return;
    BdkBase_sphereDtor(&self->m048, 2); /* dCcD_Sph::~dCcD_Sph */
    if (flags & 1)
        operator_delete(self);
}

/* bdk_class deleting destructor (HD virtual destructor) */
/* 020726D0 */
static void bdk_class_dt(bdk_class *self, s32 flags) {
    if (self == nullptr)
        return;
    BdkBase_destroyArray(self->m61C4, 4, 0x14, BdkBaseEntries::followDtor, 0, 0);
    BdkBase_destroyArray(self->m6130, 4, 0x20, BdkBaseEntries::smokeDtor, 0, 0);
    BdkBase_destroyArray(self->m6080, 4, 0x20, BdkBaseEntries::smokeDtor, 0, 0);
    BdkBase_statusDtor(&self->m603C, 2); /* dCcD_Stts::~dCcD_Stts */
    BdkBase_destroyArray(self->m261C, 40, 0x174, BdkBaseEntries::effectDtor, 0, 0);
    BdkBase_destroyArray(self->mWindAtSph, 10, 0x12C, BdkBaseEntries::sphereDtor, 0, 0);
    BdkBase_destroyArray(self->mFootCCSph, 2, 0x12C, BdkBaseEntries::sphereDtor, 0, 0);
    BdkBase_sphereDtor(&self->mBodyCCSph, 2);
    BdkBase_sphereDtor(&self->mTosakaTgSph, 2);
    BdkBase_sphereDtor(&self->mHeadTgSph, 2);
    BdkBase_sphereDtor(&self->mHeadAtSph, 2);
    BdkBase_statusDtor(&self->mStts, 2);
    /* ~dBgS_ObjAcch (inline) -> ~dBgS_Acch */
    self->mAcch.dBgS_GrpPassChk::__vtbl = BdkBaseVtables::acchGroup;
    self->mAcch.dBgS_PolyPassChk::__vtbl = BdkBaseVtables::acch;
    BdkBase_acchDtor(&self->mAcch, 0);            /* dBgS_Acch::~dBgS_Acch */
    BdkBase_circleDtor(&self->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
    BdkBase_destroyArray(self->mA50, 4, 0x12C, BdkBaseEntries::sphereDtor, 0, 0);
    BdkBase_statusDtor(&self->mA14, 2);
    BdkBase_actorDtor(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(self);
}

// HD: initialize the three light parameter blocks from the common default.
static inline void bdk_tevstr_ct(dKy_tevstr_c *t) {
    BdkBaseTev &dst = *reinterpret_cast<BdkBaseTev *>(t);
    BdkBase_initLight(dst.mLight, l_defaultLight);
    BdkBase_initLight(dst.mHdLight1, l_defaultLight);
    BdkBase_initLight(dst.mHdLight2, l_defaultLight);
}
static const dBgS_ObjAcch_vt BDK_OBJACCH_VT = {0x10007ED8, 0x10007EF8, 0x10007EE8};

/* bdk_class::bdk_class (HD: out of line; allocates when this == NULL) */
/* 02069964 */
static bdk_class *bdk_class_ct(bdk_class *self) {
    if (self == nullptr) {
        self = (bdk_class *)operator_new(sizeof(bdk_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = BDK_VTBL;
    dCcD_Stts_ct(&self->mA14);
    BdkBase_constructArray(self->mA50, 4, 0x12C, BdkBaseEntries::sphereCtor);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, BDK_OBJACCH_VT);
    dCcD_Stts_ct(&self->mStts);
    BdkBase_sphereCtor(&self->mHeadAtSph); /* dCcD_Sph::dCcD_Sph */
    BdkBase_sphereCtor(&self->mHeadTgSph);
    BdkBase_sphereCtor(&self->mTosakaTgSph);
    BdkBase_sphereCtor(&self->mBodyCCSph);
    BdkBase_constructArray(self->mFootCCSph, 2, 0x12C, BdkBaseEntries::sphereCtor);
    BdkBase_constructArray(self->mWindAtSph, 10, 0x12C, BdkBaseEntries::sphereCtor);
    BdkBase_constructArray(self->m261C, 40, 0x174, BdkBaseEntries::effectCtor);
    dCcD_Stts_ct(&self->m603C);
    BdkBase_constructArray(self->m6080, 4, 0x20, BdkBaseEntries::smokeCtorA);
    BdkBase_smokeCtor(&self->m6110, (u8)1); /* dPa_smokeEcallBack::dPa_smokeEcallBack */
    BdkBase_constructArray(self->m6130, 4, 0x20, BdkBaseEntries::smokeCtorB);
    dPa_followEcallBack_ct(&self->m61B0, 0, 0);
    BdkBase_constructArray(self->m61C4, 4, 0x14, BdkBaseEntries::followCtor);
    bdk_tevstr_ct(&self->m6224);
    return self;
}

/* WWHD glue
data center_pos 104618F8
data center_pos2 10461904
data foot_eff_pos 104618D0
data l_HIO 1046191C
data l_defaultLight 1016E414
data kamen_break_bdl 1019070C
data search_data 10190714
data head_at_sph_src 10190870
data head_tg_sph_src 101908B0
data tosaka_tg_sph_src 101908F0
data body_cc_sph_src 10190930
data foot_cc_sph_src 10190970
data wind_at_sph_src 101909B0
data kamen_sph_src 101909F0
data eff_sph_src 10190A30
str 100080B4 "Bdk" in 020692E0
str 100080B8 "Bdk" in 02069428
str 1000810C "Bdk" in 02069C8C
str 100080BC "d_a_bdk.cpp"
str 100080E0 "modelData != (0)"
str 100080C8 "i_this->pm_bgw != (0)"
str 10008110 "\u6226\u95d8\u7528\u5927\u602a\u9ce5" renamed # HD: original HIO label uses Shift-JIS.
ext 025A5B18 u32 BdkBase_smokeCtor(void*, u8)
ext 025A5894 u32 BdkBase_followCtor(void*, u8, u8)
ext 025166F0 void BdkBase_sphereCtor(dCcD_Sph*)
ext 02515AE8 void BdkBase_sphereDtor(dCcD_Sph*, s32)
ext 02515860 void BdkBase_statusDtor(dCcD_Stts*, s32)
ext 024EFD9C void BdkBase_acchDtor(dBgS_Acch*, s32)
ext 02018034 void BdkBase_circleDtor(cM3dGCir_l*, s32)
ext 025D50BC void BdkBase_actorDtor(fopAc_ac_c*, s32)
ext 028EFFD0 void BdkBase_constructArray(void*, u32, u32, u32)
ext 028F0164 void BdkBase_destroyArray(void*, u32, u32, u32, u32, u32)
ext 028F026C void BdkBase_registerObject(BdkBaseDestructorRecord*)
ext 028ED6F8 void BdkBase_firstHeaderObject(be<u8>*)
ext 028EAB2C void BdkBase_secondHeaderObject(be<u8>*)
ext 02552B60 u32 BdkBase_createJntHit(J3DModel*, __jnt_hit_data_c*, s32)
ext 025B9098 void BdkBase_onDungeonItem(dSv_memBit_c*, s32)
*/
