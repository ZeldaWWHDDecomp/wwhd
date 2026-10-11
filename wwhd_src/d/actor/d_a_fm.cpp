/**
 * d_a_fm.cpp (WWHD)
 * Enemy - Floormaster: creation, heaps, node callbacks, small helpers, HIO, static init, destructors.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Other parts: d_a_fm_a.cpp, d_a_fm_b.cpp, d_a_fm_c.cpp.
 */
#include "d/actor/d_a_fm_main.h"

const gc_tu_data gc_tu = {.safeString = fm_main::SafeStringVtable};
static daFm_c* daFm_c_ct(daFm_c* p);
static const dBgS_ObjAcch_vt FM_OBJACCH_VT = {0x1000F2A0, 0x1000F2C0, 0x1000F2B0};
static const dBgS_LinChk_vt FM_OBJLINCHK_VT = {0x1000F310, 0x1000F320, 0x1000F340, 0x1000F330};

static const s32 a_anm_bcks_tbl[] = {17, 8, 12, 14, 15, 9, 7, 16, 6, 13, 18, 11, 10};
static const fm_main::AnimationParameters a_anm_prm_tbl[] = {
    {-1, -1, 0, 8.0f, 1.0f, 0},
    {0, -1, 0, 8.0f, 1.0f, 2},
    {1, 1, 0, 8.0f, 1.0f, 0},
    {2, 8, 0, 8.0f, 1.0f, 0},
    {3, -1, 0, 8.0f, 1.0f, 0},
    {4, 1, 0, 8.0f, 1.0f, 0},
    {5, -1, 0, 8.0f, 1.0f, 0},
    {6, 1, 0, 8.0f, 1.0f, 0},
    {1, -1, 0, 0.0f, 0.0f, 0},
    {7, -1, 0, 8.0f, 1.0f, 2},
    {8, -1, 0, 8.0f, 1.0f, 0},
    {9, 1, 0, 8.0f, 1.0f, 0},
    {10, 9, 0, 8.0f, 1.0f, 0},
    {11, -1, 0, 8.0f, 1.0f, 2},
    {12, -1, 0, 8.0f, 1.0f, 2},
};
// The other Floormaster parts own these callbacks; the glue checks their guest PTMF records.
static fm_main::ModeRecord mode_tbl[] = {
    {0, -1, 0x02144190, 0, -1, 0x021442A8, 0x1000F584},
    {0, -1, 0x02144568, 0, -1, 0x021445B8, 0x1000F58C},
    {0, -1, 0x0214468C, 0, -1, 0x02144838, 0x1000F594},
    {0, -1, 0x021449C4, 0, -1, 0x02144A18, 0x1000F5A0},
    {0, -1, 0x02144B54, 0, -1, 0x02144B94, 0x1000F5AC},
    {0, -1, 0x02144D88, 0, -1, 0x021461D0, 0x1000F5B8},
    {0, -1, 0x021462A0, 0, -1, 0x02146310, 0x1000F5C0},
    {0, -1, 0x02146434, 0, -1, 0x021464B4, 0x1000F5CC},
    {0, -1, 0x02146764, 0, -1, 0x02146A34, 0x1000F5D4},
    {0, -1, 0x021473FC, 0, -1, 0x02147408, 0x1000F5DC},
    {0, -1, 0x02147724, 0, -1, 0x02147764, 0x1000F5E4},
    {0, -1, 0x02148D68, 0, -1, 0x02148DB0, 0x1000F5EC},
    {0, -1, 0x02148CB4, 0, -1, 0x02148CC0, 0x1000F5FC},
    {0, -1, 0x02147810, 0, -1, 0x02147BE0, 0x1000F604},
    {0, -1, 0x021482AC, 0, -1, 0x021482F0, 0x1000F60C},
    {0, -1, 0x0214879C, 0, -1, 0x021487F8, 0x1000F618},
    {0, -1, 0x02148F9C, 0, -1, 0x02148FA8, 0x1000F628},
    {0, -1, 0x021491B4, 0, -1, 0x02149208, 0x1000F638},
    {0, -1, 0x02149D94, 0, -1, 0x021495F8, 0x1000F64C},
    {0, -1, 0x02149688, 0, -1, 0x021496C8, 0x1000F654},
    {0, -1, 0x021497C0, 0, -1, 0x02149818, 0x1000F660},
};
static fm_main::JointHitRecord search_data[] = {
    {2, 5, 22.0f, 0x101B51E0},
    {0, 1, 8.0f, 0x101B51F8},
    {0, 2, 8.0f, 0x101B51F8},
    {0, 3, 8.0f, 0x101B51F8},
    {0, 4, 8.0f, 0x101B51F8},
    {0, 6, 4.0f, 0x101B5210},
    {0, 7, 4.0f, 0x101B5210},
    {0, 8, 4.0f, 0x101B5210},
    {0, 9, 4.0f, 0x101B5210},
    {0, 10, 4.0f, 0x101B5210},
    {0, 11, 4.0f, 0x101B5210},
    {0, 12, 4.0f, 0x101B5210},
    {0, 13, 4.0f, 0x101B5210},
    {0, 14, 4.0f, 0x101B5210},
    {0, 15, 4.0f, 0x101B5210},
};
static const fm_main::SphereSource m_sph_src = {0, 0, 0, {0, 0, 0}, 0, 0xFFBDFEF7, 9, 281, 0, 0, 0, 0, 0, 35, 0, 0, 0, 6, 0, 0.0f, 0.0f, 0.0f, 60.0f};
static const fm_main::CylinderSource m_cyl_src = {0, 2048, 0, {0, 0, 0}, 0, 0xFF1DFEFF, 0, 281, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 60.0f, 0.0f};

static const char m_arc_name[] = "fm"; // HD: GameCube archive name is "Fm".

/* ---- functions ---- */

/* 021404AC */
void daFm_c::_nodeControl(J3DNode* node, J3DModel* model) {
    J3DJoint* joint = J3DNode_toJoint(node);
    s32 jntNo = reinterpret_cast<fm_main::Joint*>(joint)->mJointNo;

    gabi::Local<Mtx34> mtx;
    PSMTXCopy(fm_main::anmMtx(model, jntNo), mtx);

    f32 z = mtx->m[2][3];
    mtx->m[2][3] = 0.0f;
    f32 y = mtx->m[1][3];
    mtx->m[1][3] = 0.0f;
    f32 x = mtx->m[0][3];
    mtx->m[0][3] = 0.0f;
    mDoMtx_stack_c::transS(x, y, z);
    if (jntNo < FM_JNT_CYUBIA_e && jntNo >= field_0x390 && fm_main::hio().field_0x00F[jntNo] == 1) {
        mDoMtx_stack_quatM(&field_0x330[jntNo]);
    }
    PSMTXConcat_g(fm_main::stackMtx(), mtx, fm_main::stackMtx());

    if (jntNo == FM_JNT_TE_e) {
        if (fm_calcTimer(&field_0x64C) != 0) {
            field_0x68A = (s16)(field_0x68A + (REG_S(12, 2) + 0x1830));
            cLib_addCalcAngleS2(&field_0x68C, 0, 10, 0x1C8);
            s16 a = field_0x68A;
            f32 amp = (f32)(s16)field_0x68C;
            s16 temp3 = (s16)gabi::ftoi(amp * cM_ssin(a));
            mDoMtx_YrotM(fm_main::stackMtx(), temp3);
        }
        gabi::Local<cXyz> temp2;
        temp2->copy(*(&fm_main::hio().field_0x038));
        PSMTXMultVec(fm_main::stackMtx(), temp2, &field_0x61C);
        temp2->copy(*(&fm_main::hio().field_0x104));
        PSMTXMultVec(fm_main::stackMtx(), temp2, &field_0x63C);
        field_0x2E8[jntNo].x = fm_main::stackMtx()->m[0][3];
        field_0x2E8[jntNo].y = fm_main::stackMtx()->m[1][3];
        field_0x2E8[jntNo].z = fm_main::stackMtx()->m[2][3];
    } else if (jntNo < FM_JNT_CYUBIA_e) {
        field_0x2E8[jntNo].x = fm_main::stackMtx()->m[0][3];
        field_0x2E8[jntNo].y = fm_main::stackMtx()->m[1][3];
        field_0x2E8[jntNo].z = fm_main::stackMtx()->m[2][3];
    }

    mtx_copy(fm_main::anmMtx(model, jntNo), fm_main::stackMtx()); /* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */
    PSMTXCopy(fm_main::stackMtx(), fm_main::currentMtx());
}

/* 0214075C */
static BOOL nodeControl_CB(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = fm_main::currentModel();
        daFm_c* i_this = fm_main::model(model)->mUserArea;
        if (i_this != nullptr) {
            i_this->_nodeControl(node, model);
        }
    }
    return TRUE;
}

/* 021407A4 */
bool daFm_c::holeCreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BDL_YPIT00_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_fm.cpp", 0x2CB, "modelData != (0)"); /* modelData != NULL */
    }

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x19001222);
    if (mpModel == nullptr) {
        return false;
    }

    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BTK_YPIT00_e);
    if (btk == nullptr) {
        JUT_ASSERT_fail("d_a_fm.cpp", 0x2D4, "btk != 0"); /* btk != NULL */
    }
    if (!mBtkAnm.init(modelData, btk, true, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, false, 0)) {
        return false;
    }
    return true;
}

/* 021408C0 */
bool daFm_c::bodyCreateHeap() {
    J3DModelData* fmModelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BDL_FM_e);
    if (fmModelData == nullptr) {
        JUT_ASSERT_fail("d_a_fm.cpp", 0x2E1, "fmModelData != (0)"); /* fmModelData != NULL */
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BCK_MODORU_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, fmModelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x37441422);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return false;
    }
    if (!invisibleModel_create(mInvisibleModel, mpMorf->getModel())) {
        return false;
    }
    fm_main::model(mpMorf->getModel())->mUserArea = this; /* setUserArea */
    return true;
}

/* 021409E8 */
bool daFm_c::jntHitCreateHeap() {
    /* search_data (15 joint-hit shapes) in .data */
    mpJntHit = JntHit_create(mpMorf->getModel(), gabi::ea(search_data), 0xF);
    if (mpJntHit) {
        jntHit = gabi::ea(mpJntHit.get()); /* fopAcM_SetJntHit */
    } else {
        return false;
    }
    return true;
}

/* 02140A54 */
BOOL daFm_c::_createHeap() {
    if (holeCreateHeap() == false) {
        return FALSE;
    }
    if (bodyCreateHeap() == false) {
        return FALSE;
    }
    return jntHitCreateHeap() ? TRUE : FALSE;
}

/* 02140AC4 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    return ((daFm_c*)i_this)->_createHeap();
}

/* daPy_py_c::getGrabActorID (virtual, HD vtable +0xBC) */
/* fopAcM_GetName / fopAcM_GetID with the HD null checks */

/* 02140AC8 */
BOOL daFm_c::_pathMove(cXyz* param_1, cXyz* param_2, cXyz* param_3) {
    field_0x398.copy(*param_3);
    field_0x3A4.copy(*param_2);
    gabi::Local<cXyz> d, temp, rel;
    cXyz_mi(param_3, d, param_2);
    cXyz_normZP(d, temp);

    cXyz_mi(&current.pos, rel, param_1);
    f32 dist = std_sqrtf(PSVECSquareMag(rel));
    if (dist < fm_main::hio().field_0x0D4 + 1.0f) {
        f32 temp2 = cM_rndF(9.0f) + 1.0f;
        field_0x394 = cM_rndF(9.0f) + 1.0f;
        gabi::Local<cXyz> a, b, c;
        cXyz_ml(temp, a, fm_main::hio().field_0x0D8);
        cXyz_ml(a, b, temp2);
        cXyz_pl(param_1, c, b);
        param_1->copy(*c);
    }

    f32 d1 = std_sqrtf(PSVECSquareDistance(param_2, param_1));
    f32 d2 = std_sqrtf(PSVECSquareDistance(param_2, param_3));
    if (d1 > d2) {
        param_1->copy(*param_3);
        return TRUE;
    }
    return FALSE;
}

/* 02140CA8 */
static BOOL pathMove_CB(cXyz* param_1, cXyz* param_2, cXyz* param_3, void* i_this) {
    return ((daFm_c*)i_this)->_pathMove(param_1, param_2, param_3);
}

/* 02140CC0 */
bool daFm_c::checkHeight(fopAc_ac_c* i_actor) {
    if (i_actor == nullptr) {
        return false;
    }
    f32 dy = i_actor->current.pos.y - current.pos.y;
    return !(std::fabs(dy) > fm_main::hio().field_0x0A8 * 0.5f); /* GHS: ble */
}

/* 02140D04 */
fopAc_ac_c* daFm_c::searchNearOtherActor(fopAc_ac_c* i_actor) {
    if (dComIfGp_event_runCheck()) {
        return nullptr;
    }
    if (fopAc_IsActor(i_actor)) {
        if (!checkHeight(i_actor)) {
            return nullptr;
        }
        f32 dist = fopAcM_searchActorDistanceXZ(this, i_actor);
        if (dist < fm_main::hio().field_0x0E4) {
            if (fopAcM_GetName(i_actor) == fpcNm_BOMB_e) {
                mpActorTarget = i_actor;
                if (!daBomb_chk_state(i_actor, 0)) {
                    return i_actor;
                }
            } else if (fopAcM_GetName(i_actor) == fpcNm_TSUBO_e) {
                fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
                f32 dist2 = fopAcM_searchPlayerDistanceXZ(this);
                f32 dist3 = fopAcM_searchPlayerDistanceXZ(i_actor);
                if (dist2 < fm_main::hio().field_0x0E8) {
                    if (dist3 > REG_F(12, 0) + 80.0f || fopAcM_GetID(i_actor) == daPy_getGrabActorID(pLink)) {
                        u32 type = daObj_PrmAbstract(i_actor, 4, 0x18); /* daTsubo::Act_c::prm_get_type */
                        switch (type) {
                        case 0: case 1: case 2: case 4: case 5: case 6:
                            mpActorTarget = i_actor;
                            return i_actor;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

/* 02140F04 */
static void* searchNearOtherActor_CB(void* param_1, void* param_2) {
    return ((daFm_c*)param_2)->searchNearOtherActor((fopAc_ac_c*)param_1);
}

/* 02140F14 */
void* daFm_c::searchNearFm(fopAc_ac_c* i_actor) {
    if (fopAc_IsActor(i_actor) && fopAc_IsActor(i_actor) && fopAcM_GetName(i_actor) == fpcNm_FM_e) {
        gabi::Local<cXyz> rel, xz;
        cXyz_mi(&field_0x3E4, rel, &current.pos);
        xz->y = 0.0f;
        xz->x = rel->x;
        xz->z = rel->z;
        f32 abs = std_sqrtf(PSVECSquareMag(xz)); /* (field_0x3E4 - current.pos).absXZ() */
        f32 dist = fopAcM_searchActorDistanceXZ(this, i_actor);
        if (dist != 0.0f) {
            gabi::Local<cXyz> xz2;
            xz2->x = field_0x3E4.x;
            xz2->y = 0.0f;
            xz2->z = field_0x3E4.z;
            if ((f64)std_sqrtf(PSVECSquareMag(xz2)) == 0.0 || dist < abs) {
                field_0x3E4.copy(i_actor->current.pos);
            }
        }
    }
    return nullptr;
}

/* 02141064 */
static void* searchNearFm_CB(void* param_1, void* param_2) {
    return ((daFm_c*)param_2)->searchNearFm((fopAc_ac_c*)param_1);
}

/* 02141280 */
bool daFm_c::isLink(fopAc_ac_c* i_actor) {
    return i_actor == dComIfGp_getLinkPlayer();
}

/* 021412BC */
void daFm_c::modeProc(daFm_c::Proc_e proc, int newMode) {
    if (proc == PROC_INIT_e) {
        switch ((u32)newMode) {
        case MODE_SW_WAIT: case MODE_HIDE: case MODE_UNDERFOOT: case MODE_PATH_MOVE: case MODE_GOAL_KEEPER: case MODE_DISAPPEAR: case MODE_PREPARE_ITEM: case MODE_DEATH:
        case MODE_GRAB_DEMO: case MODE_GRAB_FOOT_DEMO: case MODE_GRAB_NPC_DEMO: case MODE_PLAYER_START_DEMO: case MODE_DELETE:
            attention_info.flags &= ~u32(fopAc_Attn_LOCKON_BATTLE_e);
            break;
        default:
            attention_info.flags |= fopAc_Attn_LOCKON_BATTLE_e;
            break;
        }
        mMode = newMode;
        (this->*fm_main::initFunction(mode_tbl[newMode]))(); /* (this->*mode_tbl[mMode].init)() */
    } else if (proc == PROC_EXEC_e) {
        (this->*fm_main::runFunction(mode_tbl[mMode]))(); /* (this->*mode_tbl[mMode].run)() */
    }
}

/* 021413E0 */
void daFm_c::getArg() {
    u32 params = fopAcM_GetParam(this); /* fopAcM_GetParam */
    s16 homeAngleX = home.angle.x;
    s16 homeAngleZ = home.angle.z;
    field_0x2C7 = (u8)params;
    field_0x2D0 = (params >> 8) & 3;
    field_0x2DC = (params >> 10) & 3;
    m_path_no = (params >> 16) & 0xFF;
    field_0x2D4 = params >> 0x18;
    field_0x2D8 = homeAngleX & 0xFF;
    field_0x2C8 = (homeAngleX >> 8) & 0xFF;
    u32 p = homeAngleZ & 0xFF;

    if (p == 0xFF) {
        field_0x2E0 = 0.0f;
    } else {
        field_0x2E0 = (f32)p * 100.0f;
    }

    if ((f32)(s16)home.angle.y != 0.0f && field_0x2D4 != 0xFF) {
        field_0x2E4 = 1;
    }
    if (field_0x2D0 == 3) {
        field_0x2D0 = 0;
    }
    if (field_0x2DC == 3) {
        field_0x2DC = 0;
    }
    if (field_0x2E0 == 0.0f) {
        field_0x2E0 = 3000.0f;
    }

    home.angle.z = 0;
    current.angle.z = 0;
    shape_angle.z = 0;
    home.angle.x = 0;
    current.angle.x = 0;
    shape_angle.x = 0;
}

/* fopAcM_SearchByName (HD inline): fopAcIt_Judge(fpcSch_JudgeForPName, &name) */
static inline fopAc_ac_c* fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C, key.get());
}

/* 0214153C */
void daFm_c::bodySetMtx() {
    J3DModel* pModel = mpMorf->getModel();
    J3DModel_setBaseScale(pModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_transM(field_0x610.x, field_0x610.y, field_0x610.z);
    J3DModel_setBaseTRMtx(pModel, fm_main::stackMtx());
}

/* 0214162C */
void daFm_c::holeSetMtx() {
    f32 s = field_0x3E0;
    J3DModel* m = mpModel;
    fm_main::model(m)->mBaseScale.y = s; /* setBaseScale({s, s, s}) */
    fm_main::model(m)->mBaseScale.z = s;
    fm_main::model(m)->mBaseScale.x = s;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    cXyz* o = (&fm_main::hio().field_0x020);
    mDoMtx_stack_transM(o->x, o->y, o->z);
    J3DModel_setBaseTRMtx(mpModel, fm_main::stackMtx());
}

/* 02141714 */
void daFm_c::setBaseTarget() {
    switch ((u32)field_0x2DC) {
    case 0: {
        fopAcM_SearchByName(fpcNm_NPC_CB1_e);
        fopAc_ac_c* pMdActor = fopAcM_SearchByName(fpcNm_NPC_MD_e);
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        if (pMdActor == nullptr) {
            mBaseTarget = pLink;
        } else if (field_0x2D0 == 1 || field_0x2D0 == 2) {
            f32 dist = fopAcM_searchActorDistanceXZ(this, pLink);
            f32 dist2 = fopAcM_searchActorDistanceXZ(this, pMdActor);
            if (dist2 < fm_main::hio().field_0x0E8) {
                mBaseTarget = pMdActor;
            } else if (dist < fm_main::hio().field_0x0E8) {
                mBaseTarget = pLink;
            }
        } else {
            mBaseTarget = pMdActor;
        }
        break;
    }
    case 1:
        mBaseTarget = dComIfGp_getLinkPlayer();
        break;
    case 2: {
        fopAc_ac_c* actor = fopAcM_SearchByName(fpcNm_NPC_CB1_e);
        if (actor != nullptr) {
            mBaseTarget = actor;
        } else {
            actor = fopAcM_SearchByName(fpcNm_NPC_MD_e);
            if (actor != nullptr) {
                mBaseTarget = actor;
            }
        }
        break;
    }
    }

    if (mBaseTarget == nullptr) {
        field_0x9D0 = 0;
        field_0x9D4 = 0.0f;
    } else {
        field_0x9D0 = fopAcM_searchActorAngleY(this, mBaseTarget);
        field_0x9D4 = fopAcM_searchActorDistanceXZ(this, mBaseTarget);
    }
}

/* 021418D0 */
void daFm_c::setAnm(s8 anmPrmIdx, bool force) {
    if (anmPrmIdx != 0xF) {
        mAnmPrmIdx = anmPrmIdx;
    }
    fm_setAnmTables(m_arc_name, mpMorf, &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, gabi::ea(a_anm_bcks_tbl),
                     gabi::ea(a_anm_prm_tbl), force, 0);
}

/* 02141930 */
void daFm_c::createInit() {
    mStts2.Init(200, 0, this);
    mSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&m_sph_src) /* m_sph_src */);
    mSph.SetStts(&mStts2);
    mStts.Init(200, 0, this);
    mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&m_cyl_src) /* m_cyl_src */);
    mCyl.SetStts(&mStts);
    mAcchCir.SetWall(30.0f, 200.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mObjAcch.SetRoofNone();
    if (field_0x2E4 != 0) {
        mObjAcch.SetWallNone();
    }

    J3DModelData* modelData = fm_main::model(mpMorf->getModel())->mModelData.get();
    for (u16 i = 0; i < fm_jointTree(modelData)->mJointCount; i++) {
        switch (i) {
        case 0: case 1: case 2: case 3: case FM_JNT_TE_e: /* CENTER, UDEA, UDEB, UDEC, TE */
            fm_main::jointNode(modelData, i)->mCallBack = gabi::guest_fn((const void*)&nodeControl_CB);
            break;
        }
    }

    for (int i = 0; i < 6; i++) {
        auto* q = reinterpret_cast<fm_main::QuaternionWords*>(&field_0x330[i]);
        q->x = fm_main::zeroQuat().x; /* ZeroQuat */
        q->y = fm_main::zeroQuat().y;
        q->z = fm_main::zeroQuat().z;
        q->w = fm_main::zeroQuat().w;
    }

    bodySetMtx();
    holeSetMtx();
    mpMorf->calc();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

    if (field_0x2D0 == 0) {
        itemTableIdx = fm_charNameIndex("Fmaster" /* "Fmaster" */, 0);
    } else if (field_0x2D0 == 1) {
        itemTableIdx = fm_charNameIndex("Fmastr1" /* "Fmastr1" */, 0);
    } else if (field_0x2D0 == 2) {
        itemTableIdx = fm_charNameIndex("Fmastr2" /* "Fmastr2" */, 0);
    }

    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -300.0f, -50.0f, -300.0f, 300.0f, 400.0f, 300.0f);

    if (m_path_no != 0xFF) {
        mpPath = dPath_GetRoomPath(m_path_no, current.roomNo);
    }

    mEnemyIce.mpActor = this;
    mEnemyIce.m00C = 1;
    mEnemyIce.mWallRadius = 100.0f;
    mEnemyIce.mCylHeight = 200.0f;

    mBtHeight = 240.0f;
    mBtBodyR = 100.0f;
    attention_info.flags = 0;
    stealItemLeft = 3;
    max_health = (s8)fm_main::hio().mMaxHealth; /* mMaxHealth */
    health = max_health;
    mGrabPos.copy(current.pos);

    field_0x684 = 0;
    mpActorTarget = nullptr;
    field_0x69C.copy(current.pos);
    setBaseTarget();
    field_0x690.copy(current.pos);
    setAnm(8, false);
    field_0x394 = cM_rndF(9.0f) + 1.0f;
    field_0x610.copy(*(&fm_main::hio().field_0x02C));
    if (mMode != MODE_PLAYER_START_DEMO) {
        if (field_0x2D4 != 0xFF && !dComIfGs_isSwitch(field_0x2D4, current.roomNo)) {
            modeProc(PROC_INIT_e, MODE_SW_WAIT);
        } else {
            switch ((u32)field_0x2D0) {
            case 0:
                modeProc(PROC_INIT_e, MODE_UNDERFOOT);
                break;
            case 1:
                modeProc(PROC_INIT_e, MODE_PATH_MOVE);
                if (m_path_no == 0xFF) {
                    JUT_ASSERT_fail("d_a_fm.cpp", 0x146B, "m_path_no != 0xff"); /* m_path_no != 0xff */
                }
                break;
            case 2:
                modeProc(PROC_INIT_e, MODE_GOAL_KEEPER);
                break;
            }
        }
    }
}

/* 02141E40 */
cPhs_State daFm_c::_create() {
    /* fopAcM_ct(this, daFm_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daFm_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, m_arc_name);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (parentActorID != 0xFFFFFFFFu) {
            fopAc_ac_c* ac = fopAcM_SearchByID(parentActorID);
            if (ac != nullptr && fopAc_IsActor(ac) && isLink(ac)) {
                modeProc(PROC_INIT_e, MODE_PLAYER_START_DEMO);
            }
        }

        if (mMode != MODE_PLAYER_START_DEMO) {
            getArg();
            if (field_0x2D8 != 0xFF && dComIfGs_isSwitch(field_0x2D8, current.roomNo)) {
                return cPhs_ERROR_e;
            }
            if (field_0x2D4 != 0xFF && dComIfGs_isSwitch(field_0x2D4, current.roomNo) && field_0x2E4 != 0) {
                return cPhs_ERROR_e;
            }
        }

        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)&createHeap_CB), 0x2100)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return phase_state;
}

/* 02141FD4 */
static cPhs_State daFmCreate(void* i_this) {
    return ((daFm_c*)i_this)->_create();
}

/* 02141FD8 */
u8 daFm_c::checkPlayerGrabTarget() {
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    return daPy_getGrabActorID(pLink) == mProcId;
}

/* 02142028 */
void daFm_c::cancelGrab() {
    if (mpActorTarget != nullptr) {
        if (field_0x684 != 0 && (mpActorTarget->actor_status & 0x2000) /* fopAcM_checkCarryNow */ && !checkPlayerGrabTarget()) {
            mpActorTarget->gravity = 0.0f;
            mpActorTarget->speedF = 0.0f;
            mpActorTarget->speed.x = 0.0f;
            mpActorTarget->speed.y = 0.0f;
            mpActorTarget->speed.z = 0.0f;
            mpActorTarget->current.angle.x = 0;
            mpActorTarget->current.angle.z = 0;
            mpActorTarget->shape_angle.x = 0;
            mpActorTarget->shape_angle.z = 0;
            fopAcM_cancelCarryNow(mpActorTarget);
        }
        field_0x684 = 0;
    }
}

/* 021420EC */
bool daFm_c::_delete() {
    cancelGrab();
    dComIfG_resDelete(&mPhs, m_arc_name);
    auto* cb = reinterpret_cast<fm_main::Follow*>(&mpFollowEcallBack);
    gabi::call_ptr(cb->mVtable->remove, cb); /* mpFollowEcallBack.remove() (virtual) */
    if (heap) {
        mpMorf->stopZelAnime();
    }
    return true;
}

/* 02142154 */
static BOOL daFmDelete(void* i_this) {
    return ((daFm_c*)i_this)->_delete();
}

enum { JA_SE_CM_FM_SPOT = 0x7032 };

/* 02142158 */
void daFm_c::setHoleEffect() {
    if (mpFollowEcallBack.getEmitter() == nullptr) {
        /* dComIfGp_particle_setShipTail(ID_AK_SN_PITFALL00, &current.pos, NULL, NULL, 0xFF, &mpFollowEcallBack, roomNo) */
        s8 roomNo = current.roomNo;
        dPa_control_set(dComIfGp_getParticle(), 5, 0x809E, &current.pos, nullptr, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mpFollowEcallBack, roomNo, nullptr, nullptr, nullptr);
    }
    JPABaseEmitter* emitter = mpFollowEcallBack.getEmitter();
    if (emitter != nullptr) {
        f32 s = field_0x3E0; /* emitter->setGlobalScale({s, s, s}) */
        auto* e = reinterpret_cast<fm_main::Emitter*>(emitter);
        e->mGlobalParticleScale.y = s;
        e->mGlobalScale.x = s;
        e->mGlobalScale.z = s;
        e->mGlobalScale.y = s;
        e->mGlobalParticleScale.z = s;
        e->mGlobalParticleScale.x = s;
    }
}

/* 02142200 */
void daFm_c::holeExecute() {
    if (field_0x3E0 > 0.015f) { /* isHoleAppear */
        mBtkAnm.play();
        fm_seStart(this, JA_SE_CM_FM_SPOT, 0);
        actor_status = (actor_status & ~0x3Fu) | 0x20; /* fopAcM_SetStatusMap(this, 0x20) */
        setHoleEffect();
    } else {
        actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap */
        dPa_followEcallBack_end(&mpFollowEcallBack);
    }
}

/* 021422A4 */
void daFm_c::setAttention() {
    s32 m = mMode;
    if (field_0x2E4 != 0 || (u32)m == 0 || ((u32)m >= 0xE && (u32)m <= 0x11)) {
        (&attention_info.position)->copy(current.pos);
        eyePos.copy(current.pos);
        return;
    }
    cXyz* att = (&attention_info.position);
    f32 ax = field_0x61C.x;
    f32 ay = field_0x61C.y;
    f32 az = field_0x61C.z;
    att->x = ax;
    att->z = az;
    att->y = ay + 20.0f;
    fopAc_ac_c* pLink = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> head, d;
    cXyz* top = daPy_getHeadTopPos(pLink);
    head->x = top->x;
    head->y = top->y;
    head->z = top->z;
    cXyz_mi(head, d, &field_0x61C);
    f32 abs = std_sqrtf(PSVECSquareMag(d));
    if (!dComIfGp_checkPlayerStatus0(0, 0x402) && mMode == MODE_ATTACK && !(abs > fm_main::hio().field_0x0EC)) {
        f32 x = current.pos.x;
        f32 y = current.pos.y;
        f32 z = current.pos.z;
        eyePos.x = x;
        eyePos.z = z;
        eyePos.y = y + 200.0f;
    } else {
        eyePos.copy(field_0x61C);
    }
}

/* 02142450 */
bool daFm_c::setHoleScale(f32 param_1, f32 param_2, f32 param_3) {
    cLib_addCalc2(&field_0x3E0, param_1, param_2, param_3);
    return !(std::fabs(field_0x3E0 - param_1) > param_3);
}

/* 021424D0 */
void daFm_c::iceProc() {
    setAttention();
    J3DModel* pModel = mpMorf->getModel();
    J3DModel_setBaseTRMtx(pModel, fm_main::stackMtx());
    mpMorf->calc();
    if (mEnemyIce.mLightShrinkTimer != 0 || !(health > 0)) {
        setHoleScale(fm_main::hio().field_0x120, 0.1f, fm_main::hio().field_0x128);
    }
    holeSetMtx();
    cancelGrab();
}

/* 021425BC */
bool daFm_c::lineCheck(cXyz* param_1, cXyz* param_2) {
    dBgS_LinChk_Set(&mLinChk, param_1, param_2, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        param_2->copy(*(&reinterpret_cast<fm_main::LineCheck*>(&mLinChk)->mCross)); /* GetCross */
        return true;
    }
    return false;
}

/* 0214264C */
bool daFm_c::areaCheck() {
    bool ret = true;
    for (int i = 0; i < 12; i++) {
        f32 r = fm_main::hio().field_0x0E0;
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM((s16)(i * 0x1555));
        mDoMtx_stack_transM(r, -30.0f, 0.0f);
        cXyz* p = &field_0xA48[i];
        f32 x = fm_main::stackMtx()->m[0][3];
        p->x = x;
        p->y = fm_main::stackMtx()->m[1][3];
        p->z = fm_main::stackMtx()->m[2][3];

        gabi::Local<cXyz> temp2;
        temp2->x = x;
        temp2->y = p->y + 60.0f;
        temp2->z = p->z;

        if (field_0x2D0 == 0 || field_0x2D0 == 1) {
            gabi::Local<cXyz> d, xz;
            cXyz_mi(p, d, &field_0x69C);
            xz->x = d->x;
            xz->y = 0.0f;
            xz->z = d->z;
            if (std_sqrtf(PSVECSquareMag(xz)) > field_0x2E0) {
                field_0xAD8[i] = 0;
                ret = false;
            } else {
                field_0xAD8[i] = 1;
            }
        }

        if (field_0xAD8[i] != 0 || field_0x2D0 == 2) {
            if (!lineCheck(temp2, p)) {
                field_0xAD8[i] = 0;
                ret = false;
            } else {
                field_0xAD8[i] = 1;
            }
        }
    }
    return ret;
}

/* 02142870 */
void daFm_c::spAttackVJump() {
    mBtHeight = fm_main::hio().mSpAttackVJumpHeight; /* initBt(mSpAttackVJumpHeight, mSpAttackVJumpRadius) */
    mBtBodyR = fm_main::hio().mSpAttackVJumpRadius;
    f32 frame = mpMorf->getFrame(); /* mpMorf->getFrame() */
    mBtAttackType = 3; /* setBtAttackData(start, end, 700.0f, 3) */
    mBtStartFrame = (f32)fm_main::hio().mSpAttackVJumpStartFrame;
    mBtEndFrame = (f32)fm_main::hio().mSpAttackVJumpEndFrame;
    mBtMaxDis = 700.0f;
    mBtNowFrame = frame;
}

/* 021428FC */
void daFm_c::spAttackJump() {
    mBtHeight = fm_main::hio().mSpAttackJumpHeight;
    mBtBodyR = fm_main::hio().mSpAttackJumpRadius;
    f32 frame = mpMorf->getFrame();
    mBtAttackType = 1;
    mBtStartFrame = (f32)fm_main::hio().mSpAttackJumpStartFrame;
    mBtEndFrame = (f32)fm_main::hio().mSpAttackJumpEndFrame;
    mBtMaxDis = 700.0f;
    mBtNowFrame = frame;
}

// HD: compiler-generated and template code of this translation unit.

/* 02141074 */
static daFm_c* daFm_c_ct(daFm_c* p) {
    if (p == nullptr) {
        p = (daFm_c*)operator_new(0x101C);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = fm_main::ActorVtable;
    dPa_followEcallBack_ct(&p->mpFollowEcallBack, 0, 0);
    mDoExt_btkAnm::ct(&p->mBtkAnm);
    dBgS_ObjAcch_ct(&p->mObjAcch, FM_OBJACCH_VT);
    dBgS_AcchCir_ct(&p->mAcchCir);
    fm_invisible_ct(p->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    dCcD_Stts_ct(&p->mStts);
    dCcD_Stts_ct(&p->mStts2);
    fm_sphere_ct(&p->mSph); /* dCcD_Sph::dCcD_Sph */
    dCcD_Cyl_ct(&p->mCyl, fm_main::AabVtable);
    dBgS_LinChk_ct(&p->mLinChk, FM_OBJLINCHK_VT, true); /* dBgS_ObjLinChk */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, fm_main::AabVtable);
    dBgS_AcchCir_ct(&p->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&p->mEnemyIce.mBgAcch, FM_OBJACCH_VT);
    return p;
}

/* 02149D78 */
static void fm_SafeString_dtor(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0214996C */
static void* daFm_HIO_c_ct(void* p) {
    if (p == nullptr) {
        p = operator_new(0x15C);
        if (p == nullptr) return p;
    }
    auto* h = reinterpret_cast<fm_main::HIO*>(p);
    h->__vtbl = fm_main::HIOVtable;   /* vtable */
    fm_jointHitHIO_ct(h->field_0x130); /* JntHit_HIO_c::JntHit_HIO_c */
    h->field_0x006 = 0;
    h->field_0x007 = 0;
    h->field_0x008 = 0;
    h->field_0x004 = 0;
    h->field_0x01F = 0;
    h->field_0x00B = 0;
    h->field_0x009 = 0;
    h->field_0x00A = 0;
    h->field_0x00C = 0;
    h->field_0x005 = 0;

    h->field_0x068 = 0x1E;
    h->mSpAttackVJumpHeight = 240.0f; /* mSpAttackVJumpHeight */
    h->mSpAttackVJumpRadius = 100.0f;
    h->mSpAttackVJumpStartFrame = 0;
    h->mSpAttackVJumpEndFrame = 0xF;
    h->mSpAttackJumpHeight = 120.0f; /* mSpAttackJumpHeight */
    h->mSpAttackJumpRadius = 100.0f;
    h->mSpAttackJumpStartFrame = 5;
    h->mSpAttackJumpEndFrame = 0xF;
    h->field_0x11C = 100;
    h->field_0x00E = 0;
    h->field_0x00D = 0;
    h->mFreezeDuration = 0x78; /* mFreezeDuration */

    for (int i = 0; i < 6; i++) {
        switch (i) {
        case 0: case 1: case 2: case 3: case 5:
            h->field_0x00F[i] = 1;
            break;
        default:
            h->field_0x00F[i] = 0;
            break;
        }
    }
    h->field_0x050.x = 0.0f;
    h->field_0x050.y = 0.0f;
    h->field_0x050.z = 0.0f;
    h->field_0x05C.x = 0;
    h->field_0x05C.y = 0;
    h->field_0x05C.z = 0;
    h->field_0x020.x = -10.0f;
    h->field_0x020.y = 5.0f;
    h->field_0x020.z = -10.0f;
    h->field_0x02C.x = 0.0f;
    h->field_0x02C.y = 0.0f;
    h->field_0x02C.z = -30.0f;
    h->field_0x038.x = 40.0f;
    h->field_0x038.y = -20.0f;
    h->field_0x038.z = 0.0f;
    h->field_0x044.x = 0.0f;
    h->field_0x044.y = 0.0f;
    h->field_0x044.z = 0.0f;
    h->field_0x104.x = 0.0f;
    h->field_0x104.y = -20.0f;
    h->field_0x104.z = -0.0f;

    h->field_0x0A8 = 600.0f;
    h->mMaxHealth = 0xC; /* mMaxHealth */
    h->field_0x066 = 0x28;
    h->field_0x084 = 0x5A;
    h->field_0x086 = 0xF0;
    h->field_0x088 = 0x3C;
    h->field_0x08A = 1;
    h->field_0x08C = 0x3C;
    h->field_0x08E = 0x78;
    h->field_0x090 = 10;
    h->field_0x092 = 0x11;
    h->field_0x094 = 0;
    h->field_0x096 = 500;
    h->field_0x098 = 18000;
    h->field_0x09A = 7000;
    h->field_0x0C0 = 15.0f;
    h->field_0x120 = 0.01f;
    h->field_0x124 = 1.0f;
    h->field_0x128 = 0.1f;
    h->field_0x12C = 0.02f;
    h->field_0x0C4 = 290.0f;
    // HD: GameCube uses 80.0f for the body sphere radius.
    h->field_0x0C8 = 50.0f;
    h->field_0x0CC = 7.0f;
    h->field_0x0D0 = 2.0f;
    h->field_0x0D4 = 5.0f;
    h->field_0x0D8 = 80.0f;
    h->field_0x0DC = 100.0f;
    h->field_0x0E0 = 150.0f;
    h->field_0x0E4 = 500.0f;
    h->field_0x0E8 = 800.0f;
    h->field_0x0EC = 30.0f;
    h->field_0x0F0 = 25.0f;
    h->field_0x0F4 = 9.0f;
    h->field_0x0F8 = -2.5f;
    h->field_0x0FC = 16.0f;
    h->field_0x09C = 5;
    h->field_0x100 = 0x1E;
    h->field_0x110 = 20.0f;
    h->field_0x114 = 50.0f;
    h->field_0x118 = -20.0f;
    h->field_0x0B0 = 30.0f;
    h->field_0x0A0 = 0x14;
    h->field_0x0A2 = 0x96;
    h->field_0x0B4 = 1000.0f;
    h->field_0x0B8 = 1000.0f;
    h->field_0x09E = 0x96;
    h->field_0x0AC = 300.0f;
    h->field_0x0BC = 750.0f;
    h->field_0x0A6 = 10;
    return p;
}

/* 02149CD8 */
static void __sinit_d_a_fm_cpp() {
    sinit_header_statics(fm_main::headerGuards(), fm_main::headerDestructors());
    daFm_HIO_c_ct(&fm_main::hio());
}

/* 02149D8C */
static BOOL daFmIsDelete(void* i_this) {
    return TRUE;
}

/* 02149D94 */
void daFm_c::modeDeleteInit() {
}

/* dBgS_ObjAcch destructor (inline): this TU's vtables back, then dBgS_Acch::~dBgS_Acch */
static inline void fm_objacch_dt(dBgS_ObjAcch* a) {
    reinterpret_cast<fm_main::Acch*>(a)->mVtable20 = fm_main::AcchVtable20;
    reinterpret_cast<fm_main::Acch*>(a)->mVtable14 = fm_main::AcchVtable14;
    fm_acch_dt(a, 0);
}

/* 02149D98 */
static void daFm_c_dt(daFm_c* self, s32 flags) {
    if (self == nullptr) return;
    fm_objacch_dt(&self->mEnemyIce.mBgAcch);
    fm_circle_dt(reinterpret_cast<fm_main::AcchCir*>(&self->mEnemyIce.mBgAcchCir)->mCircle, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    /* dBgS_ObjLinChk::~dBgS_ObjLinChk (inline): this TU's vtables back, then cBgS_LinChk::~cBgS_LinChk */
    auto* lc = reinterpret_cast<fm_main::LineCheck*>(&self->mLinChk);
    lc->mVtable58 = fm_main::LineVtable58;
    lc->mVtable64 = fm_main::LineVtable64;
    lc->mVtable20 = fm_main::LineVtable20;
    fm_line_dt(lc, 0);
    dCcD_Cyl_dt(&self->mCyl, 2);
    fm_sphere_dt(&self->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mStts2, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    fm_invisible_dt(self->mInvisibleModel, 2); /* mDoExt_invisibleModel::~mDoExt_invisibleModel */
    fm_circle_dt(reinterpret_cast<fm_main::AcchCir*>(&self->mAcchCir)->mCircle, 2);
    fm_objacch_dt(&self->mObjAcch);
    fm_actor_dt(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}

/* 02149ED0 */
static void fm_SafeString_assureTermination(void*) {
}

/* WWHD glue
data a_anm_bcks_tbl 1000F680
data a_anm_prm_tbl 1000F6B4
data mode_tbl 1000F3E0
data search_data 101B512C
data m_sph_src 1000FA40
data m_cyl_src 1000FA80
ext 025E8A48 bool invisibleModel_create(void* inv, J3DModel* model)
ext 02552B60 JntHit_c* JntHit_create(J3DModel* model, u32 data, s16 count)
str 1000F37C "d_a_fm.cpp" in 021407A4
str 1000F388 "modelData != (0)"
str 1000F39C "btk != 0"
str 1000F3A8 "d_a_fm.cpp" in 021408C0
str 1000F3B4 "fmModelData != (0)"
str 1000F7C0 "Fmaster"
str 1000F7C8 "Fmastr1"
str 1000F7D0 "Fmastr2"
str 1000F7D8 "m_path_no != 0xff"
str 1000F7EC "d_a_fm.cpp" in 02141930
ext 024EFD9C void fm_acch_dt(dBgS_ObjAcch* p, s32 flags)
ext 02018034 void fm_circle_dt(void* p, s32 flags)
ext 02008B4C void fm_line_dt(void* p, s32 flags)
ext 02515AE8 void fm_sphere_dt(dCcD_Sph* p, s32 flags)
ext 025E89F8 void fm_invisible_dt(void* p, s32 flags)
ext 025D50BC void fm_actor_dt(fopAc_ac_c* p, s32 flags)
ext 0211D2F8 s32 fm_calcTimer(be<s32>* timer)
ext 0200E814 s32 fm_nameIndex(void* table, const char* name, s32 type)
ext 025F25CC void mDoMtx_stack_quatM(Quaternion_fm* q)
ext 028E9108 void PSMTXConcat_g(Mtx34* a, Mtx34* b, Mtx34* ab)
ext 0201B12C void cXyz_normZP(const cXyz* a, cXyz* res)
ext 028E8DE8 f32 PSVECSquareDistance(const cXyz* a, const cXyz* b)
ext 020CB92C bool daBomb_chk_state(fopAc_ac_c* b, s32 st)
ext 02048038 u32 daObj_PrmAbstract(fopAc_ac_c* a, s32 w, s32 s)
ext 027F3F94 fm_main::JointTree* fm_jointTree(J3DModelData* d)
ext 02552BE8 void fm_jointHitHIO_ct(void* h)
ext 025E895C void fm_invisible_ct(void* p)
ext 025166F0 void fm_sphere_ct(dCcD_Sph* p)
ext 02587A0C void fm_setAnmTables(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks, u32 prms, bool force, s32 hd)
str 1000FA3C "fm"
*/
