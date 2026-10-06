/**
 * d_a_bmd.cpp (WWHD)
 * Boss - Kalle Demos (Core) / 森ボス (Forest Boss)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * The large functions are in d_a_bmd_move.cpp (move with its inlined states), d_a_bmd_camera.cpp
 * (demo_camera) and d_a_bmd_exec.cpp (daBmd_Execute with core_move, mk_move, eff_cont, ...).
 */
#include "d/actor/d_a_bmd.h"

/* 020AE138 */
static BOOL core_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x020AE138, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_bl* model = j3dSys_getModel_bl();
        bmd_class* i_this = gabi::at<bmd_class>(model->mUserArea);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr && jntNo != BKM_COA_JNT_KUBI1_e && jntNo != BKM_COA_JNT_SITAAGO_e) {
            PSMTXCopy(bl_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo < BKM_COA_JNT_TOSAKA1_e) {
                cMtx_XrotM(calc_mtx(), i_this->m90C[0].x);
                cMtx_ZrotM(calc_mtx(), i_this->m90C[0].z);
            } else {
                cMtx_XrotM(calc_mtx(), i_this->m90C[1].x);
                cMtx_ZrotM(calc_mtx(), i_this->m90C[1].z);
            }
            mtx_copy(bl_getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx */
            PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx_bl());
        }
    }
    return TRUE;
}
VERIFY(0x020AE138, core_nodeCallBack);

/* 020AE30C: mk_draw inlined. HD: no simple shadow; the snap picture includes Makar */
static BOOL daBmd_Draw(bmd_class* i_this) {
    WWHD_FUNC(0x020AE30C, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    s16 blur = i_this->mB72;
    if (blur >= 1) {
        if (blur > 1) {
            gabi::store<u8>(0x101F4826, (u8)blur); /* mDoGph_gInf_c::setBlureRate */
            gabi::call(0x025F064C);                /* mDoGph_gInf_c::onBlure */
        } else {
            i_this->mB72 = 0;
            gabi::store<u8>(0x101F4825, 0);        /* mDoGph_gInf_c::offBlure */
        }
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    J3DModel* model = i_this->mpBodyMorf->getModel();
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, model, &actor->tevStr);
    if (i_this->mB70 != 0) {
        mDoExt_baseAnm* brk = i_this->mpBrkAnm;
        gabi::call(0x025E83FC, brk, J3DModel_getModelData(model), brk->getFrame()); /* mDoExt_brkAnm::entry */
        mDoExt_baseAnm* btk = i_this->mpBtkAnm;
        gabi::call(0x025E7FC4, btk, J3DModel_getModelData(model), btk->getFrame()); /* mDoExt_btkAnm::entry */
    }
    i_this->mpBodyMorf->entryDL();
    if (i_this->m306 < 100) {
        J3DModel* m = i_this->mpHeadMorf->getModel();
        env = dKy_getEnvlight();
        setLightTevColorType(env, m, &actor->tevStr);
        i_this->mpHeadMorf->entryDL();
    } else if (i_this->m306 < 0x6E) {
        J3DModel* m = i_this->mpHeadDeadMorf->getModel();
        env = dKy_getEnvlight();
        setLightTevColorType(env, m, &actor->tevStr);
        i_this->mpHeadDeadMorf->entryDL();
    }
    /* mk_draw */
    if (i_this->m2DC != 0) {
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        env = dKy_getEnvlight();
        setLightTevColorType(env, body->getModel(), &actor->tevStr);
        i_this->mpMakarMorf->entryDL();
        J3DModel_bl* mk = (J3DModel_bl*)i_this->mpMakarMorf->getModel();
        Mtx34* src = bl_getAnmMtx(mk, CB_JNT_BACKBONE_e);
        mtx_copy(gabi::at<Mtx34>(gabi::ea(i_this->mpMakarFaceModel.get()) + 0xC8), src); /* setBaseTRMtx */
        env = dKy_getEnvlight();
        setLightTevColorType(env, i_this->mpMakarFaceModel, &actor->tevStr);
        mDoExt_modelUpdateDL(i_this->mpMakarFaceModel, 0);
    }
    /* dKy_tevstr_c tevstr = actor->tevStr: member-wise, the padding 0xBD..0xBF is not copied */
    gabi::Local<dKy_tevstr_c> tevstr;
    {
        u32 src = gabi::ea(&actor->tevStr), dst = gabi::ea(tevstr.get());
        for (u32 i = 0; i < 0xBC; i += 4) {
            if ((i >= 0x84 && i <= 0x8C) || (i >= 0xA8 && i <= 0xB0)) /* f32 members: lfs/stfs */
                gabi::store<f32>(dst + i, gabi::load<f32>(src + i));
            else
                gabi::store<u32>(dst + i, gabi::load<u32>(src + i));
        }
        gabi::store<u8>(dst + 0xBC, gabi::load<u8>(src + 0xBC));
        for (u32 i = 0xC0; i < 0x1C8; i += 4) gabi::store<u32>(dst + i, gabi::load<u32>(src + i));
    }
    /* static cXyz g_pos(0.0f, 0.0f, 0.0f) */
    if (gabi::load<u32>(0x104624D4) == 0) {
        gabi::store<u32>(0x104624D4, 1);
        g_pos()->x = 0.0f;
        g_pos()->z = 0.0f;
        g_pos()->y = 0.0f;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG1, g_pos(), tevstr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, i_this->mpR00_EFModel, tevstr);
    {
        J3DModel* ef = i_this->mpR00_EFModel;
        mDoExt_baseAnm* brk = i_this->mpR00_EFBrk;
        gabi::call(0x025E83FC, brk, J3DModel_getModelData(ef), brk->getFrame());
    }
    mDoExt_modelUpdateDL(i_this->mpR00_EFModel, 0);
    if (i_this->m2DC != 0) {
        /* HD: the picture is taken at Makar */
        J3DModel_bl* mk = (J3DModel_bl*)i_this->mpMakarMorf->getModel();
        s32 jnt = i_this->mMakarSnapJnt;
        gabi::Local<Mtx34> mtx;
        PSMTXCopy(bl_getAnmMtx(mk, jnt), mtx);
        gabi::Local<cXyz> pos;
        pos->x = i_this->m2E0.x;
        pos->z = i_this->m2E0.z;
        pos->y = mtx->m[1][3] - 20.0f;
        dSnap_RegistFig_pos(0x9C, actor, pos, i_this->m2FA, 1.0f, 1.0f, 1.0f);
    } else {
        dSnap_RegistFig(0xC8 /* DSNAP_TYPE_BMD */, actor, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}
VERIFY(0x020AE30C, daBmd_Draw);

/* 020AE7B8 */
void anm_init(bmd_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x020AE7B8, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009BB0) /* "Bmd" */, bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes(STR(0x10009BB4) /* "Ki" */, soundFileIdx, SAFESTRING_VTBL);
        i_this->mpBodyMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, snd);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009BB0) /* "Bmd" */, bckFileIdx, SAFESTRING_VTBL);
        i_this->mpBodyMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x020AE7B8, anm_init);

/* 020AE8E8 */
void move1(bmd_class* i_this) {
    WWHD_FUNC(0x020AE8E8, void, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused dComIfGp_getPlayer(0) */
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    gabi::Local<cXyz> local_28;
    local_28->y = 0.0f;
    local_28->x = 0.0f;
    local_28->z = actor->speedF;
    MtxPosition(local_28, &actor->speed);
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos); /* current.pos += speed */
    cLib_addCalc2(&i_this->m324, cM_ssin(i_this->m2FE * 400) * 20.0f, 0.1f, 10.0f);
    cLib_addCalcAngleS2(&actor->shape_angle.x, (s16)gabi::ftoi(cM_ssin(i_this->m2FE * 500) * 700.0f), 0x10, 0x80);
    cLib_addCalcAngleS2(&actor->shape_angle.z, (s16)gabi::ftoi(cM_scos(i_this->m2FE * 300) * 700.0f), 0x10, 0x80);
}
VERIFY(0x020AE8E8, move1);

/* 020AEA40 */
void mk_voice_set(bmd_class* i_this, u32 param_2) {
    WWHD_FUNC(0x020AEA40, void, i_this, param_2);
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
    mDoAud_monsSeStart4(param_2, &i_this->m2E0, 0, reverb);
}
VERIFY(0x020AEA40, mk_voice_set);

/* 020AEA94. HD: Link standing on the flower while it is closed (m304 == 0) hides the core
 * (m314 = 20) */
static void ride_call_back(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt) {
    WWHD_FUNC(0x020AEA94, void, bgw, i_ac, i_pt);
    bmd_class* i_this = (bmd_class*)i_ac;
    if (i_this->m304 == 3 && !dScnPly_isPause_bl()) {
        cLib_addCalc2(&i_pt->current.pos.x, i_ac->current.pos.x, 1.0f, REG0_F(2) + 400.0f);
        cLib_addCalc2(&i_pt->current.pos.y, i_ac->current.pos.y, 1.0f, REG0_F(2) + 400.0f);
        cLib_addCalc2(&i_pt->current.pos.z, i_ac->current.pos.z, 1.0f, REG0_F(2) + 400.0f);
        i_pt->old.pos.copy(i_pt->current.pos);
        if (i_pt != nullptr && fpcM_GetName(i_pt) == 0xA8 /* fpcNm_PLAYER_e */) {
            i_this->m942 = 5;
        }
    }
    if (i_this->m304 == 0 && i_pt != nullptr && fpcM_GetName(i_pt) == 0xA8 /* fpcNm_PLAYER_e */) {
        i_this->m314 = 20;
    }
}
VERIFY(0x020AEA94, ride_call_back);

/* 020B1104 */
static BOOL daBmd_IsDelete(bmd_class*) {
    WWHD_FUNC(0x020B1104, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020B1104, daBmd_IsDelete);

/* 020B110C */
static BOOL daBmd_Delete(bmd_class* i_this) {
    WWHD_FUNC(0x020B110C, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfG_resDelete(&i_this->mPhs, STR(0x10009C5C) /* "Bmd" */); /* dComIfG_resDeleteDemo */
    mDoHIO_deleteChild(l_HIO().mNo);
    if (actor->heap != nullptr) {
        for (s32 i = 0; i < 6; i++) {
            if (i_this->pm_bgw[i] != nullptr) {
                dBgS* bgs = dComIfG_Bgsp();
                cBgS_Release(bgs, i_this->pm_bgw[i]);
            }
        }
    }
    for (s32 i = 0; i < 7; i++) {
        /* mSmokeCb[i].remove(): virtual (vtable +0x44) */
        gabi::call_ptr(gabi::load<u32>(i_this->mSmokeCb[i].__vtbl + 0x44), &i_this->mSmokeCb[i]);
    }
    dKyw_pntwind_cut(&i_this->mWindInfluence);
    mDoAud_seDeleteObject(&i_this->m2E0);
    return TRUE;
}
VERIFY(0x020B110C, daBmd_Delete);

enum {
    dRes_INDEX_BMD_BCK_CALL_01_e = 0x8,
    dRes_INDEX_BMD_BCK_COA_DEAD1_e = 0xC,
    dRes_INDEX_BMD_BCK_COA_WAIT_e = 0x14,
    dRes_INDEX_BMD_BCK_HANA_WAIT_e = 0x20,
    dRes_INDEX_BMD_BDL_R00_EF_e = 0x28,
    dRes_INDEX_BMD_BMD_BKM_e = 0x2B,
    dRes_INDEX_BMD_BMD_BKM_COA_e = 0x2C,
    dRes_INDEX_BMD_BMD_BKM_COA_DEADMODEL_e = 0x2D,
    dRes_INDEX_BMD_BMD_CB_e = 0x2E,
    dRes_INDEX_BMD_BMD_CB_FACE_e = 0x2F,
    dRes_INDEX_BMD_BRK_BKM_e = 0x33,
    dRes_INDEX_BMD_BRK_R00_EF_e = 0x34,
    dRes_INDEX_BMD_BTK_BKM_e = 0x37,
    dRes_INDEX_BMD_DZB_COLL1_e = 0x3A,
    dRes_INDEX_BMD_DZB_COLL2_e = 0x3B,
};

static mDoExt_McaMorf* new_McaMorf(s32 bmdIdx, s32 bckIdx) {
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x10009A9C) /* "Bmd" */, bmdIdx, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10009A9C), bckIdx, SAFESTRING_VTBL);
    return mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0,
                                  0x11020203);
}

/* 020B11E4: also the solid heap callback (solidHeapCB folded into it) */
static BOOL useHeapInit(bmd_class* i_this) {
    WWHD_FUNC(0x020B11E4, BOOL, i_this);
    const char* arc = STR(0x10009A9C); /* "Bmd" */
    i_this->mpBodyMorf = new_McaMorf(dRes_INDEX_BMD_BMD_BKM_e, dRes_INDEX_BMD_BCK_HANA_WAIT_e);
    /* HD: the morf is checked before its model */
    if (i_this->mpBodyMorf == nullptr)
        return FALSE;
    J3DModel* model = i_this->mpBodyMorf->getModel();
    if (model == nullptr)
        return FALSE;
    {
        void* p = operator_new(0x78);
        if (p != nullptr)
            p = mDoExt_brkAnm_ct_bl(p);
        i_this->mpBrkAnm = (mDoExt_baseAnm*)p;
        if (p == nullptr)
            return FALSE;
    }
    void* pBrk = dComIfG_getObjectRes(arc, dRes_INDEX_BMD_BRK_BKM_e, SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init_bl(i_this->mpBrkAnm, J3DModel_getModelData(model), pBrk, 1, 0, 1.0f, 0, -1, false, 0))
        return FALSE;
    {
        void* p = operator_new(0x74);
        if (p != nullptr)
            p = mDoExt_btkAnm_ct_bl(p);
        i_this->mpBtkAnm = (mDoExt_baseAnm*)p;
        if (p == nullptr)
            return FALSE;
    }
    void* pBtk = dComIfG_getObjectRes(arc, dRes_INDEX_BMD_BTK_BKM_e, SAFESTRING_VTBL);
    if (!mDoExt_btkAnm_init_bl(i_this->mpBtkAnm, J3DModel_getModelData(model), pBtk, 1, 0, 1.0f, 0, -1, false, 0))
        return FALSE;

    i_this->mpHeadMorf = new_McaMorf(dRes_INDEX_BMD_BMD_BKM_COA_e, dRes_INDEX_BMD_BCK_COA_WAIT_e);
    model = i_this->mpHeadMorf->getModel();
    if (model == nullptr)
        return FALSE;
    gabi::store<u32>(gabi::ea(model) + 0xB8, gabi::ea(i_this)); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum_bl(J3DModel_getModelData(model)); i++) {
        setJointCallBack_bl(J3DModel_getModelData(model), i, 0x020AE138 /* core_nodeCallBack */);
    }

    i_this->mpHeadDeadMorf = new_McaMorf(dRes_INDEX_BMD_BMD_BKM_COA_DEADMODEL_e, dRes_INDEX_BMD_BCK_COA_DEAD1_e);
    if (i_this->mpHeadDeadMorf->getModel() == nullptr)
        return FALSE;

    i_this->pm_bgw[5] = new_dBgW();
    if (i_this->pm_bgw[5] == nullptr) /* JUT_ASSERT(0x1010, i_this->pm_bgw[5] != 0) */
        JUT_ASSERT_fail(STR(0x10009B20), 0x1010, STR(0x10009B68));
    if (i_this->pm_bgw[5] == nullptr)
        return FALSE;
    {
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(arc, dRes_INDEX_BMD_DZB_COLL1_e, SAFESTRING_VTBL);
        cBgW_Set(i_this->pm_bgw[5], dzb, cBgW_MOVE_BG_e, &i_this->mA34);
    }
    dBgW_SetCrrFunc(i_this->pm_bgw[5], 0x024EE658 /* dBgS_MoveBGProc_Typical */);
    dBgW_SetRideCallback(i_this->pm_bgw[5], 0x020AEA94 /* ride_call_back */);
    for (s32 i = 0; i < 5; i++) {
        i_this->pm_bgw[i] = new_dBgW();
        if (i_this->pm_bgw[i] == nullptr) /* JUT_ASSERT(0x1022, i_this->pm_bgw[i] != 0) */
            JUT_ASSERT_fail(STR(0x10009B20), 0x1022, STR(0x10009B84));
        if (i_this->pm_bgw[i] == nullptr)
            return FALSE;
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(arc, dRes_INDEX_BMD_DZB_COLL2_e, SAFESTRING_VTBL);
        cBgW_Set(i_this->pm_bgw[i], dzb, cBgW_MOVE_BG_e, &i_this->m944[i]);
        dBgW_SetCrrFunc(i_this->pm_bgw[i], 0x024EE658 /* dBgS_MoveBGProc_Typical */);
        dBgW_SetRideCallback(i_this->pm_bgw[i], 0x020AEA94 /* ride_call_back */);
    }

    i_this->mpMakarMorf = new_McaMorf(dRes_INDEX_BMD_BMD_CB_e, dRes_INDEX_BMD_BCK_CALL_01_e);
    model = i_this->mpMakarMorf->getModel();
    if (model == nullptr)
        return FALSE;
    /* HD: the joint of Makar's model that the snap picture uses, by name */
    i_this->mMakarSnapJnt = JUTNameTab_getIndex_bl(J3DModelData_getJointName_bl(J3DModel_getModelData(model)), STR(0x10009AA0));

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_BMD_BMD_CB_FACE_e, SAFESTRING_VTBL);
    i_this->mpMakarFaceModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpMakarFaceModel == nullptr)
        return FALSE;
    modelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_BMD_BDL_R00_EF_e, SAFESTRING_VTBL);
    i_this->mpR00_EFModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->mpR00_EFModel == nullptr)
        return FALSE;
    {
        void* p = operator_new(0x78);
        if (p != nullptr)
            p = mDoExt_brkAnm_ct_bl(p);
        i_this->mpR00_EFBrk = (mDoExt_baseAnm*)p;
        if (p == nullptr)
            return FALSE;
    }
    pBrk = dComIfG_getObjectRes(arc, dRes_INDEX_BMD_BRK_R00_EF_e, SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init_bl(i_this->mpR00_EFBrk, modelData, pBrk, 1, 0, 0.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x020B11E4, useHeapInit);

/* 020B1918 */
static cPhs_State daBmd_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x020B1918, cPhs_State, a_this);
    bmd_class* i_this = (bmd_class*)a_this;
    /* fopAcM_ct(a_this, bmd_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            gabi::call(0x020B17EC, a_this); /* bmd_class::bmd_class */
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhs, STR(0x10009C68) /* "Bmd" */);
    if (res != cPhs_COMPLEATE_e)
        return res;
    for (s32 i = 0; i < 7; i++) {
        gabi::store<u8>(gabi::ea(&i_this->mSmokeCb[i]) + 0x12, 1); /* setFollowOff */
    }
    if (dComIfGs_isDungeonItem_bl(3) /* isStageBossEnemy */ && dComIfGp_getStartStageName0() != 'X') {
        /* HD: no REG0_S(6) test */
        if (!dComIfGs_checkGetItem_bl(0x6B /* dItemNo_PEARL_FARORE_e */)) {
            gabi::Local<cXyz> local_30;
            local_30->x = 100.0f;
            local_30->y = 0.0f;
            local_30->z = 800.0f;
            fopAcM_create(0x14E /* fpcNm_NPC_CB1_e */, 0, local_30, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
        }
        return cPhs_ERROR_e;
    }
    if (!fopAcM_entrySolidHeap(a_this, 0x020B11E4 /* useHeapInit */, 0x96000))
        return cPhs_ERROR_e;
    for (s32 i = 0; i < 6; i++) {
        dBgS* bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, i_this->pm_bgw[i], a_this))
            return cPhs_ERROR_e;
    }
    l_HIO().mNo = mDoHIO_createChild(STR(0x10009C6C) /* "森ボス" */, &l_HIO());
    a_this->health = 0xF;
    a_this->max_health = 0xF;
    for (s32 i = 0; i < 20; i++) {
        u8* params = fopAcM_CreateAppend_bl();
        gabi::store<u32>(gabi::ea(params), i); /* base.parameters */
        fpcSCtRq_Request_bl(fpcLy_CurrentLayer_bl(), 0xEC /* fpcNm_BMDHAND_e */, 0, 0, params);
    }
    for (s32 i = 0; i < 8; i++) {
        u8* params = fopAcM_CreateAppend_bl();
        gabi::store<u32>(gabi::ea(params), i);
        fpcSCtRq_Request_bl(fpcLy_CurrentLayer_bl(), 0xED /* fpcNm_BMDFOOT_e */, 0, 0, params);
    }
    f32 y = a_this->home.pos.y + (REG0_F(2) + 20.0f);
    a_this->current.pos.y = y;
    a_this->home.pos.y = y;
    i_this->m328 = y;
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
    i_this->mAcchCir.SetWall(500.0f, 1000.0f);
    i_this->mStts.Init(0xFF, 0, a_this);
    i_this->mBodySph.SetStts(&i_this->mStts);
    i_this->mBodySph.Set(gabi::at<dCcD_SrcSph>(0x10191A04) /* body_sph_src */);
    i_this->mCoreSph.SetStts(&i_this->mStts);
    i_this->mCoreSph.Set(gabi::at<dCcD_SrcSph>(0x10191A44) /* core_sph_src */);
    i_this->mCoCyl.SetStts(&i_this->mStts);
    i_this->mCoCyl.Set(gabi::at<dCcD_SrcCyl>(0x10191A84) /* co_cyl_src */);
    i_this->m308[2] = 200;
    if (!dComIfGs_isDungeonItem_bl(5) /* isStageBossDemo */ && dComIfGp_getStartStageName0() != 'X') {
        dComIfGs_offTmpBit_bl(0x480 /* dSv_event_tmp_flag_c::UNK_0480 */);
        i_this->mMode = 10;
        i_this->mBE0 = 1;
        i_this->mBDC = 1.0f;
    } else {
        if (dComIfGp_getStartStageName0() == 'X') {
            mDoAud_bgmStart(0x8000004A /* JA_BGM_PAST_BKM */);
        } else {
            mDoAud_bgmStart(0x80000005 /* JA_BGM_KINDAN_BOSS */);
        }
        i_this->mB71 = 1;
    }
    dKyw_pntwind_set(&i_this->mWindInfluence);
    gabi::store<u8>(gabi::ea(a_this) + 0x38A, 4); /* attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] */
    gabi::call(0x020AEBCC, i_this);              /* daBmd_Execute */
    return cPhs_COMPLEATE_e;
}
VERIFY(0x020B1918, daBmd_Create);

/* 020B17E4: __construct_array helper: dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
static void* smokeEcallBack_ct1(void* p) {
    WWHD_FUNC(0x020B17E4, void*, p);
    return gabi::call<void*>(0x025A5B18, p, 1);
}
VERIFY(0x020B17E4, smokeEcallBack_ct1);

/* 020B17EC: bmd_class::bmd_class (fopAcM_ct; allocates when this == NULL) */
static bmd_class* bmd_class_ct(bmd_class* i_this) {
    WWHD_FUNC(0x020B17EC, bmd_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bmd_class*)operator_new(0xD04);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = BMD_VTBL;
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    dBgS_ObjAcch_ct(&i_this->mAcch, OBJACCH_VT);
    dCcD_Stts_ct(&i_this->mStts);
    gabi::call(0x025166F0, &i_this->mBodySph); /* dCcD_Sph::dCcD_Sph */
    gabi::call(0x025166F0, &i_this->mCoreSph);
    dCcD_Cyl_ct(&i_this->mCoCyl, 0x10009AC0);
    gabi::call(0x028EFFD0, &i_this->mSmokeCb[0], 7, 0x20, 0x020B17E4); /* __construct_array */
    return i_this;
}
VERIFY(0x020B17EC, bmd_class_ct);

/* 020B1CB4: daBmd_HIO_c::daBmd_HIO_c (allocates when this == NULL) */
static daBmd_HIO_c* daBmd_HIO_c_ct(daBmd_HIO_c* h) {
    WWHD_FUNC(0x020B1CB4, daBmd_HIO_c*, h);
    if (h == nullptr) {
        h = (daBmd_HIO_c*)operator_new(0x18);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->m05 = 0;
    h->m08 = 0.8f;
    h->m14 = 100;
    h->m0C = 1.0f;
    h->m10 = 0.5f;
    h->__vtbl = 0x10009B00;
    return h;
}
VERIFY(0x020B1CB4, daBmd_HIO_c_ct);

/* 020B1D30 */
static void __sinit_d_a_bmd_cpp() {
    WWHD_FUNC(0x020B1D30, void, (u32)0);
    sinit_header_statics_z(0x10462494, 0x10191AC8, 0x104624B8);
    daBmd_HIO_c_ct(&l_HIO());
}
VERIFY(0x020B1D30, __sinit_d_a_bmd_cpp);

/* 020B1DD0: deleting destructor of a class with a trivial destructor (daBmd_HIO_c) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020B1DD0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020B1DD0, trivial_dt);

/* 020B5494: deleting destructor of dPa_smokeEcallBack (trivial) */
static void smoke_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020B5494, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020B5494, smoke_dt);

/* 020B54A8: bmd_class deleting destructor (inline member destructors) */
static void bmd_class_dt(bmd_class* i_this, s32 flags) {
    WWHD_FUNC(0x020B54A8, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x028F0164, &i_this->mSmokeCb[0], 7, 0x20, 0x020B5494, 0, 0); /* __destroy_arr */
        gabi::call(0x02515A70, &i_this->mCoCyl, 2);   /* dCcD_Cyl::~dCcD_Cyl */
        gabi::call(0x02515AE8, &i_this->mCoreSph, 2); /* dCcD_Sph::~dCcD_Sph */
        gabi::call(0x02515AE8, &i_this->mBodySph, 2);
        gabi::call(0x02515860, &i_this->mStts, 2);    /* dCcD_Stts::~dCcD_Stts */
        /* ~dBgS_ObjAcch */
        u32 b = gabi::ea(&i_this->mAcch);
        gabi::store<u32>(b + 0x20, OBJACCH_VT.v20);
        gabi::store<u32>(b + 0x14, OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);    /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mAcchCir) + 0x14), 2); /* ~dBgS_AcchCir: its cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0);            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020B54A8, bmd_class_dt);

/* 020B557C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x020B557C, void, p);
}
VERIFY(0x020B557C, empty_virtual);
