/**
 * d_a_fm.cpp (WWHD)
 * Enemy - Floormaster: creation, heaps, node callbacks, small helpers, HIO, static init, destructors.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Other parts: d_a_fm_a.cpp, d_a_fm_b.cpp, d_a_fm_c.cpp.
 */
#include "d/actor/d_a_fm_local.h"

/* 02149D78 sead::SafeString deleting destructor (this TU's vtable) */
static void fm_SafeString_dtor(void* p, s32 flags) {
    WWHD_FUNC(0x02149D78, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02149D78, fm_SafeString_dtor);

#define SAFESTRING_VTBL 0x1000F258 /* this TU's sead::SafeString vtable */
#define m_arc_name STR(0x1000FA3C)  /* "Fm" */

enum { FM_JNT_TE_e = 5, FM_JNT_CYUBIA_e = 6 };
enum {
    dRes_INDEX_FM_BCK_MODORU_e = 0xC, dRes_INDEX_FM_BDL_FM_e = 0x15, dRes_INDEX_FM_BDL_YPIT00_e = 0x18,
    dRes_INDEX_FM_BTK_YPIT00_e = 0x1B,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025F25CC mDoMtx_stack_c::quatM(const Quaternion*) */
static inline void mDoMtx_stack_quatM(Quaternion_fm* q) { gabi::call(0x025F25CC, q); }
/* 028E9108 PSMTXConcat(a, b, ab) */
static inline void PSMTXConcat_g(Mtx34* a, Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* REG12_S(2) */
static inline s16 REG12_S2() { return gabi::load<s16>(0x1047BD4C); }

/* ---- functions ---- */

/* 021404AC */
void daFm_c::_nodeControl(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x021404AC, void, this, node, model);
    J3DJoint* joint = J3DNode_toJoint(node);
    s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);

    gabi::Local<Mtx34> mtx;
    PSMTXCopy(fm_getAnmMtx(model, jntNo), mtx);

    f32 z = mtx->m[2][3];
    mtx->m[2][3] = 0.0f;
    f32 y = mtx->m[1][3];
    mtx->m[1][3] = 0.0f;
    f32 x = mtx->m[0][3];
    mtx->m[0][3] = 0.0f;
    mDoMtx_stack_c::transS(x, y, z);
    if (jntNo < FM_JNT_CYUBIA_e && jntNo >= field_0x390 && hio_u8(0x0F + jntNo) == 1) {
        mDoMtx_stack_quatM(&field_0x330[jntNo]);
    }
    PSMTXConcat_g(mtx_now(), mtx, mtx_now());

    if (jntNo == FM_JNT_TE_e) {
        if (cLib_calcTimer(&field_0x64C) != 0) {
            field_0x68A = (s16)(field_0x68A + (REG12_S2() + 0x1830));
            cLib_addCalcAngleS2(&field_0x68C, 0, 10, 0x1C8);
            s16 a = field_0x68A;
            f32 amp = (f32)(s16)field_0x68C;
            s16 temp3 = (s16)gabi::ftoi(amp * cM_ssin(a));
            mDoMtx_YrotM(mtx_now(), temp3);
        }
        gabi::Local<cXyz> temp2;
        temp2->copy(*hio_xyz(0x38));
        PSMTXMultVec(mtx_now(), temp2, &field_0x61C);
        temp2->copy(*hio_xyz(0x104));
        PSMTXMultVec(mtx_now(), temp2, &field_0x63C);
        field_0x2E8[jntNo].x = mtx_now()->m[0][3];
        field_0x2E8[jntNo].y = mtx_now()->m[1][3];
        field_0x2E8[jntNo].z = mtx_now()->m[2][3];
    } else if (jntNo < FM_JNT_CYUBIA_e) {
        field_0x2E8[jntNo].x = mtx_now()->m[0][3];
        field_0x2E8[jntNo].y = mtx_now()->m[1][3];
        field_0x2E8[jntNo].z = mtx_now()->m[2][3];
    }

    mtx_copy(fm_getAnmMtx(model, jntNo), mtx_now()); /* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */
    PSMTXCopy(mtx_now(), J3DSys_mCurrentMtx());
}
VERIFY(0x021404AC, &daFm_c::_nodeControl);

/* 0214075C */
static BOOL nodeControl_CB(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0214075C, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = j3dSys_model();
        daFm_c* i_this = gabi::at<daFm_c>(gabi::load<u32>(model + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeControl(node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x0214075C, nodeControl_CB);

/* 021407A4 */
bool daFm_c::holeCreateHeap() {
    WWHD_FUNC(0x021407A4, bool, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BDL_YPIT00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1000F37C), 0x2CB, STR(0x1000F388)); /* modelData != NULL */
    }

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x19001222);
    if (mpModel == nullptr) {
        return false;
    }

    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BTK_YPIT00_e, SAFESTRING_VTBL);
    if (btk == nullptr) {
        JUT_ASSERT_fail(STR(0x1000F37C), 0x2D4, STR(0x1000F39C)); /* btk != NULL */
    }
    if (!mBtkAnm.init(modelData, btk, true, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, false, 0)) {
        return false;
    }
    return true;
}
VERIFY(0x021407A4, &daFm_c::holeCreateHeap);

/* 021408C0 */
bool daFm_c::bodyCreateHeap() {
    WWHD_FUNC(0x021408C0, bool, this);
    J3DModelData* fmModelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BDL_FM_e, SAFESTRING_VTBL);
    if (fmModelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1000F3A8), 0x2E1, STR(0x1000F3B4)); /* fmModelData != NULL */
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_FM_BCK_MODORU_e, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, fmModelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x37441422);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return false;
    }
    if (!invisibleModel_create(mInvisibleModel, mpMorf->getModel())) {
        return false;
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    return true;
}
VERIFY(0x021408C0, &daFm_c::bodyCreateHeap);

/* 021409E8 */
bool daFm_c::jntHitCreateHeap() {
    WWHD_FUNC(0x021409E8, bool, this);
    /* search_data (15 joint-hit shapes) in .data */
    mpJntHit = JntHit_create(mpMorf->getModel(), 0x101B512C, 0xF);
    if (mpJntHit) {
        jntHit = gabi::ea(mpJntHit.get()); /* fopAcM_SetJntHit */
    } else {
        return false;
    }
    return true;
}
VERIFY(0x021409E8, &daFm_c::jntHitCreateHeap);

/* 02140A54 */
BOOL daFm_c::_createHeap() {
    WWHD_FUNC(0x02140A54, BOOL, this);
    if (holeCreateHeap() == false) {
        return FALSE;
    }
    if (bodyCreateHeap() == false) {
        return FALSE;
    }
    return jntHitCreateHeap() ? TRUE : FALSE;
}
VERIFY(0x02140A54, &daFm_c::_createHeap);

/* 02140AC4 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02140AC4, BOOL, i_this);
    return ((daFm_c*)i_this)->_createHeap();
}
VERIFY(0x02140AC4, createHeap_CB);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0201B12C cXyz::normZP() (this, hidden result) */
static inline void cXyz_normZP(const cXyz* a, cXyz* res) { gabi::call(0x0201B12C, a, res); }
/* 028E8DE8 PSVECSquareDistance(a, b) */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* 020CB92C daBomb_c::chk_state(state) */
static inline bool daBomb_chk_state(fopAc_ac_c* b, s32 st) { return gabi::call<bool>(0x020CB92C, b, st); }
/* 02048038 daObj::PrmAbstract(actor, width, shift) */
static inline u32 daObj_PrmAbstract(fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<u32>(0x02048038, a, w, s); }
/* daPy_py_c::getGrabActorID (virtual, HD vtable +0xBC) */
static inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return gabi::call_ptr<u32>(gabi::load<u32>(p->__vtbl + 0xBC), p); }
/* fopAcM_GetName / fopAcM_GetID with the HD null checks */
static inline s32 fm_GetName(fopAc_ac_c* a) { return a != nullptr ? (s32)fpcM_GetName(a) : -1; }
static inline u32 fm_GetID(fopAc_ac_c* a) { return a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu; }
enum { fpcNm_FM_e = 0x77, fpcNm_BOMB_e = 0x126, fpcNm_TSUBO_e = 0x1C5 };

/* 02140AC8 */
BOOL daFm_c::_pathMove(cXyz* param_1, cXyz* param_2, cXyz* param_3) {
    WWHD_FUNC(0x02140AC8, BOOL, this, param_1, param_2, param_3);
    field_0x398.copy(*param_3);
    field_0x3A4.copy(*param_2);
    gabi::Local<cXyz> d, temp, rel;
    cXyz_mi(param_3, d, param_2);
    cXyz_normZP(d, temp);

    cXyz_mi(&current.pos, rel, param_1);
    f32 dist = std_sqrtf(PSVECSquareMag(rel));
    if (dist < hio_f(0xD4) + 1.0f) {
        f32 temp2 = cM_rndF(9.0f) + 1.0f;
        field_0x394 = cM_rndF(9.0f) + 1.0f;
        gabi::Local<cXyz> a, b, c;
        cXyz_ml(temp, a, hio_f(0xD8));
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
VERIFY(0x02140AC8, &daFm_c::_pathMove);

/* 02140CA8 */
static BOOL pathMove_CB(cXyz* param_1, cXyz* param_2, cXyz* param_3, void* i_this) {
    WWHD_FUNC(0x02140CA8, BOOL, param_1, param_2, param_3, i_this);
    return ((daFm_c*)i_this)->_pathMove(param_1, param_2, param_3);
}
VERIFY(0x02140CA8, pathMove_CB);

/* 02140CC0 */
bool daFm_c::checkHeight(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02140CC0, bool, this, i_actor);
    if (i_actor == nullptr) {
        return false;
    }
    f32 dy = i_actor->current.pos.y - current.pos.y;
    return !(std::fabs(dy) > hio_f(0xA8) * 0.5f); /* GHS: ble */
}
VERIFY(0x02140CC0, &daFm_c::checkHeight);

/* 02140D04 */
fopAc_ac_c* daFm_c::searchNearOtherActor(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02140D04, fopAc_ac_c*, this, i_actor);
    if (dComIfGp_event_runCheck()) {
        return nullptr;
    }
    if (fopAc_IsActor(i_actor)) {
        if (!checkHeight(i_actor)) {
            return nullptr;
        }
        f32 dist = fopAcM_searchActorDistanceXZ(this, i_actor);
        if (dist < hio_f(0xE4)) {
            if (fm_GetName(i_actor) == fpcNm_BOMB_e) {
                mpActorTarget = i_actor;
                if (!daBomb_chk_state(i_actor, 0)) {
                    return i_actor;
                }
            } else if (fm_GetName(i_actor) == fpcNm_TSUBO_e) {
                fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
                f32 dist2 = fopAcM_searchPlayerDistanceXZ(this);
                f32 dist3 = fopAcM_searchPlayerDistanceXZ(i_actor);
                if (dist2 < hio_f(0xE8)) {
                    if (dist3 > REG_F(12, 0) + 80.0f || fm_GetID(i_actor) == daPy_getGrabActorID(pLink)) {
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
VERIFY(0x02140D04, &daFm_c::searchNearOtherActor);

/* 02140F04 */
static void* searchNearOtherActor_CB(void* param_1, void* param_2) {
    WWHD_FUNC(0x02140F04, void*, param_1, param_2);
    return ((daFm_c*)param_2)->searchNearOtherActor((fopAc_ac_c*)param_1);
}
VERIFY(0x02140F04, searchNearOtherActor_CB);

/* 02140F14 */
void* daFm_c::searchNearFm(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02140F14, void*, this, i_actor);
    if (fopAc_IsActor(i_actor) && fopAc_IsActor(i_actor) && fm_GetName(i_actor) == fpcNm_FM_e) {
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
VERIFY(0x02140F14, &daFm_c::searchNearFm);

/* 02141064 */
static void* searchNearFm_CB(void* param_1, void* param_2) {
    WWHD_FUNC(0x02141064, void*, param_1, param_2);
    return ((daFm_c*)param_2)->searchNearFm((fopAc_ac_c*)param_1);
}
VERIFY(0x02141064, searchNearFm_CB);

static const dBgS_ObjAcch_vt FM_OBJACCH_VT = {0x1000F2A0, 0x1000F2C0, 0x1000F2B0};
static const dBgS_LinChk_vt FM_OBJLINCHK_VT = {0x1000F310, 0x1000F320, 0x1000F340, 0x1000F330};
#define FM_AAB_VTBL 0x1000F270 /* this TU's cM3dGAab vtable */
#define FM_MODE_TBL 0x1000F3E0 /* modeProc::mode_tbl (init PTMF +0, run PTMF +8, name +0x10) */

/* 02141074: daFm_c::daFm_c (compiler-generated) */
static daFm_c* daFm_c_ct(daFm_c* p) {
    WWHD_FUNC(0x02141074, daFm_c*, p);
    if (p == nullptr) {
        p = (daFm_c*)operator_new(0x101C);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = FM_VTBL;
    dPa_followEcallBack_ct(&p->mpFollowEcallBack, 0, 0);
    mDoExt_btkAnm::ct(&p->mBtkAnm);
    dBgS_ObjAcch_ct(&p->mObjAcch, FM_OBJACCH_VT);
    dBgS_AcchCir_ct(&p->mAcchCir);
    gabi::call(0x025E895C, p->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    dCcD_Stts_ct(&p->mStts);
    dCcD_Stts_ct(&p->mStts2);
    gabi::call(0x025166F0, &p->mSph); /* dCcD_Sph::dCcD_Sph */
    dCcD_Cyl_ct(&p->mCyl, FM_AAB_VTBL);
    dBgS_LinChk_ct(&p->mLinChk, FM_OBJLINCHK_VT, true); /* dBgS_ObjLinChk */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, FM_AAB_VTBL);
    dBgS_AcchCir_ct(&p->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&p->mEnemyIce.mBgAcch, FM_OBJACCH_VT);
    return p;
}
VERIFY(0x02141074, daFm_c_ct);

/* 02141280 */
bool daFm_c::isLink(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02141280, bool, this, i_actor);
    return i_actor == dComIfGp_getLinkPlayer();
}
VERIFY(0x02141280, &daFm_c::isLink);

/* 021412BC */
void daFm_c::modeProc(daFm_c::Proc_e proc, int newMode) {
    WWHD_FUNC(0x021412BC, void, this, proc, newMode);
    if (proc == PROC_INIT_e) {
        switch ((u32)newMode) {
        case 0: case 1: case 2: case 3: case 4: case 6: case 0xB: case 0xC:
        case 0xE: case 0xF: case 0x10: case 0x11: case 0x12:
            *attention_flags(this) &= ~4u; /* LOCKON_BATTLE */
            break;
        default:
            *attention_flags(this) |= 4u;
            break;
        }
        mMode = newMode;
        ptmf_call(FM_MODE_TBL + 0x14 * newMode, this); /* (this->*mode_tbl[mMode].init)() */
    } else if (proc == PROC_EXEC_e) {
        ptmf_call(FM_MODE_TBL + 0x14 * mMode + 8, this); /* (this->*mode_tbl[mMode].run)() */
    }
}
VERIFY(0x021412BC, &daFm_c::modeProc);

/* 021413E0 */
void daFm_c::getArg() {
    WWHD_FUNC(0x021413E0, void, this);
    u32 params = gabi::load<u32>(gabi::ea(this) + 0xB0); /* fopAcM_GetParam */
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
VERIFY(0x021413E0, &daFm_c::getArg);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* fopAcM_SearchByName (HD inline): fopAcIt_Judge(fpcSch_JudgeForPName, &name) */
static inline fopAc_ac_c* fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C, key.get());
}
/* 027F3F94: J3DModelData joint-tree header (HD; +8 joint count) */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
enum { fpcNm_NPC_CB1_e = 0x14E, fpcNm_NPC_MD_e = 0x16F };

/* 0214153C */
void daFm_c::bodySetMtx() {
    WWHD_FUNC(0x0214153C, void, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModel_setBaseScale(pModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_transM(field_0x610.x, field_0x610.y, field_0x610.z);
    J3DModel_setBaseTRMtx(pModel, mtx_now());
}
VERIFY(0x0214153C, &daFm_c::bodySetMtx);

/* 0214162C */
void daFm_c::holeSetMtx() {
    WWHD_FUNC(0x0214162C, void, this);
    f32 s = field_0x3E0;
    J3DModel* m = mpModel;
    gabi::store<f32>(gabi::ea(m) + 0xC0, s); /* setBaseScale({s, s, s}) */
    gabi::store<f32>(gabi::ea(m) + 0xC4, s);
    gabi::store<f32>(gabi::ea(m) + 0xBC, s);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    cXyz* o = hio_xyz(0x20);
    mDoMtx_stack_transM(o->x, o->y, o->z);
    J3DModel_setBaseTRMtx(mpModel, mtx_now());
}
VERIFY(0x0214162C, &daFm_c::holeSetMtx);

/* 02141714 */
void daFm_c::setBaseTarget() {
    WWHD_FUNC(0x02141714, void, this);
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
            if (dist2 < hio_f(0xE8)) {
                mBaseTarget = pMdActor;
            } else if (dist < hio_f(0xE8)) {
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
VERIFY(0x02141714, &daFm_c::setBaseTarget);

/* 021418D0 (matcher: checkPlayerGrabBomb) */
void daFm_c::setAnm(s8 anmPrmIdx, bool force) {
    WWHD_FUNC(0x021418D0, void, this, anmPrmIdx, force);
    if (anmPrmIdx != 0xF) {
        mAnmPrmIdx = anmPrmIdx;
    }
    dLib_bcks_setAnm(m_arc_name, mpMorf, &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, 0x1000F680 /* a_anm_bcks_tbl */,
                     0x1000F6B4 /* a_anm_prm_tbl */, force);
}
VERIFY(0x021418D0, &daFm_c::setAnm);

/* 02141930 (matcher: isGrabPos) */
void daFm_c::createInit() {
    WWHD_FUNC(0x02141930, void, this);
    mStts2.Init(200, 0, this);
    mSph.Set(gabi::at<dCcD_SrcSph>(0x1000FA40) /* m_sph_src */);
    mSph.SetStts(&mStts2);
    mStts.Init(200, 0, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x1000FA80) /* m_cyl_src */);
    mCyl.SetStts(&mStts);
    mAcchCir.SetWall(30.0f, 200.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mObjAcch.SetRoofNone();
    if (field_0x2E4 != 0) {
        mObjAcch.SetWallNone();
    }

    J3DModelData* modelData = fm_modelData(mpMorf->getModel());
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        switch (i) {
        case 0: case 1: case 2: case 3: case FM_JNT_TE_e: /* CENTER, UDEA, UDEB, UDEC, TE */
            gabi::store<u32>(fm_jointNode(modelData, i) + 8, 0x0214075C /* nodeControl_CB */);
            break;
        }
    }

    for (int i = 0; i < 6; i++) {
        gabi::store<u32>(gabi::ea(&field_0x330[i]) + 0, gabi::load<u32>(0x101E9C38)); /* ZeroQuat */
        gabi::store<u32>(gabi::ea(&field_0x330[i]) + 4, gabi::load<u32>(0x101E9C3C));
        gabi::store<u32>(gabi::ea(&field_0x330[i]) + 8, gabi::load<u32>(0x101E9C40));
        gabi::store<u32>(gabi::ea(&field_0x330[i]) + 12, gabi::load<u32>(0x101E9C44));
    }

    bodySetMtx();
    holeSetMtx();
    mpMorf->calc();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

    if (field_0x2D0 == 0) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x1000F7C0) /* "Fmaster" */, 0);
    } else if (field_0x2D0 == 1) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x1000F7C8) /* "Fmastr1" */, 0);
    } else if (field_0x2D0 == 2) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x1000F7D0) /* "Fmastr2" */, 0);
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
    *attention_flags(this) = 0;
    stealItemLeft = 3;
    max_health = (s8)hio_s(0x64); /* mMaxHealth */
    health = max_health;
    mGrabPos.copy(current.pos);

    field_0x684 = 0;
    mpActorTarget = nullptr;
    field_0x69C.copy(current.pos);
    setBaseTarget();
    field_0x690.copy(current.pos);
    setAnm(8, false);
    field_0x394 = cM_rndF(9.0f) + 1.0f;
    field_0x610.copy(*hio_xyz(0x2C));
    if (mMode != 0x11) {
        if (field_0x2D4 != 0xFF && !dComIfGs_isSwitch(field_0x2D4, current.roomNo)) {
            modeProc(PROC_INIT_e, 0);
        } else {
            switch ((u32)field_0x2D0) {
            case 0:
                modeProc(PROC_INIT_e, 2);
                break;
            case 1:
                modeProc(PROC_INIT_e, 3);
                if (m_path_no == 0xFF) {
                    JUT_ASSERT_fail(STR(0x1000F7EC), 0x146B, STR(0x1000F7D8)); /* m_path_no != 0xff */
                }
                break;
            case 2:
                modeProc(PROC_INIT_e, 4);
                break;
            }
        }
    }
}
VERIFY(0x02141930, &daFm_c::createInit);

/* 02141E40 */
cPhs_State daFm_c::_create() {
    WWHD_FUNC(0x02141E40, cPhs_State, this);
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
                modeProc(PROC_INIT_e, 0x11);
            }
        }

        if (mMode != 0x11) {
            getArg();
            if (field_0x2D8 != 0xFF && dComIfGs_isSwitch(field_0x2D8, current.roomNo)) {
                return cPhs_ERROR_e;
            }
            if (field_0x2D4 != 0xFF && dComIfGs_isSwitch(field_0x2D4, current.roomNo) && field_0x2E4 != 0) {
                return cPhs_ERROR_e;
            }
        }

        if (!fopAcM_entrySolidHeap(this, 0x02140AC4 /* createHeap_CB */, 0x2100)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return phase_state;
}
VERIFY(0x02141E40, &daFm_c::_create);

/* 02141FD4 */
static cPhs_State daFmCreate(void* i_this) {
    WWHD_FUNC(0x02141FD4, cPhs_State, i_this);
    return ((daFm_c*)i_this)->_create();
}
VERIFY(0x02141FD4, daFmCreate);

/* 02141FD8 */
u8 daFm_c::checkPlayerGrabTarget() {
    WWHD_FUNC(0x02141FD8, u8, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    return daPy_getGrabActorID(pLink) == mProcId;
}
VERIFY(0x02141FD8, &daFm_c::checkPlayerGrabTarget);

/* 02142028 */
void daFm_c::cancelGrab() {
    WWHD_FUNC(0x02142028, void, this);
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
VERIFY(0x02142028, &daFm_c::cancelGrab);

/* 021420EC */
bool daFm_c::_delete() {
    WWHD_FUNC(0x021420EC, bool, this);
    cancelGrab();
    dComIfG_resDelete(&mPhs, m_arc_name);
    u32 cb = gabi::ea(&mpFollowEcallBack);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(cb) + 0x44), cb); /* mpFollowEcallBack.remove() (virtual) */
    if (heap) {
        mpMorf->stopZelAnime();
    }
    return true;
}
VERIFY(0x021420EC, &daFm_c::_delete);

/* 02142154 */
static BOOL daFmDelete(void* i_this) {
    WWHD_FUNC(0x02142154, BOOL, i_this);
    return ((daFm_c*)i_this)->_delete();
}
VERIFY(0x02142154, daFmDelete);

enum { JA_SE_CM_FM_SPOT = 0x7032 };
static inline bool fm_checkPlayerStatus0(u32 mask) { return (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & mask) != 0; }

/* 02142158 */
void daFm_c::setHoleEffect() {
    WWHD_FUNC(0x02142158, void, this);
    if (mpFollowEcallBack.getEmitter() == nullptr) {
        /* dComIfGp_particle_setShipTail(ID_AK_SN_PITFALL00, &current.pos, NULL, NULL, 0xFF, &mpFollowEcallBack, roomNo) */
        s8 roomNo = current.roomNo;
        dPa_control_set(dComIfGp_getParticle(), 5, 0x809E, &current.pos, nullptr, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mpFollowEcallBack, roomNo, nullptr, nullptr, nullptr);
    }
    JPABaseEmitter* emitter = mpFollowEcallBack.getEmitter();
    if (emitter != nullptr) {
        f32 s = field_0x3E0; /* emitter->setGlobalScale({s, s, s}) */
        u32 e = gabi::ea(emitter);
        gabi::store<f32>(e + 0x23C, s);
        gabi::store<f32>(e + 0x220, s);
        gabi::store<f32>(e + 0x228, s);
        gabi::store<f32>(e + 0x224, s);
        gabi::store<f32>(e + 0x240, s);
        gabi::store<f32>(e + 0x238, s);
    }
}
VERIFY(0x02142158, &daFm_c::setHoleEffect);

/* 02142200 */
void daFm_c::holeExecute() {
    WWHD_FUNC(0x02142200, void, this);
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
VERIFY(0x02142200, &daFm_c::holeExecute);

/* 021422A4 */
void daFm_c::setAttention() {
    WWHD_FUNC(0x021422A4, void, this);
    s32 m = mMode;
    if (field_0x2E4 != 0 || (u32)m == 0 || ((u32)m >= 0xE && (u32)m <= 0x11)) {
        attention_pos(this)->copy(current.pos);
        eyePos.copy(current.pos);
        return;
    }
    cXyz* att = attention_pos(this);
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
    if (!fm_checkPlayerStatus0(0x402) && mMode == 8 && !(abs > hio_f(0xEC))) {
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
VERIFY(0x021422A4, &daFm_c::setAttention);

/* 02142450 */
bool daFm_c::setHoleScale(f32 param_1, f32 param_2, f32 param_3) {
    WWHD_FUNC(0x02142450, bool, this, param_1, param_2, param_3);
    cLib_addCalc2(&field_0x3E0, param_1, param_2, param_3);
    return !(std::fabs(field_0x3E0 - param_1) > param_3);
}
VERIFY(0x02142450, &daFm_c::setHoleScale);

/* 021424D0 */
void daFm_c::iceProc() {
    WWHD_FUNC(0x021424D0, void, this);
    setAttention();
    J3DModel* pModel = mpMorf->getModel();
    J3DModel_setBaseTRMtx(pModel, mtx_now());
    mpMorf->calc();
    if (mEnemyIce.mLightShrinkTimer != 0 || !(health > 0)) {
        setHoleScale(hio_f(0x120), 0.1f, hio_f(0x128));
    }
    holeSetMtx();
    cancelGrab();
}
VERIFY(0x021424D0, &daFm_c::iceProc);

/* 021425BC */
bool daFm_c::lineCheck(cXyz* param_1, cXyz* param_2) {
    WWHD_FUNC(0x021425BC, bool, this, param_1, param_2);
    dBgS_LinChk_Set(&mLinChk, param_1, param_2, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        param_2->copy(*gabi::at<cXyz>(gabi::ea(&mLinChk) + 0x30)); /* GetCross */
        return true;
    }
    return false;
}
VERIFY(0x021425BC, &daFm_c::lineCheck);

/* 0214264C */
bool daFm_c::areaCheck() {
    WWHD_FUNC(0x0214264C, bool, this);
    bool ret = true;
    for (int i = 0; i < 12; i++) {
        f32 r = hio_f(0xE0);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM((s16)(i * 0x1555));
        mDoMtx_stack_transM(r, -30.0f, 0.0f);
        cXyz* p = &field_0xA48[i];
        f32 x = mtx_now()->m[0][3];
        p->x = x;
        p->y = mtx_now()->m[1][3];
        p->z = mtx_now()->m[2][3];

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
VERIFY(0x0214264C, &daFm_c::areaCheck);

/* 02142870 (unnamed) */
void daFm_c::spAttackVJump() {
    WWHD_FUNC(0x02142870, void, this);
    mBtHeight = hio_f(0x6C); /* initBt(mSpAttackVJumpHeight, mSpAttackVJumpRadius) */
    mBtBodyR = hio_f(0x70);
    f32 frame = gabi::load<f32>(gabi::ea(mpMorf.get()) + 0x9C); /* mpMorf->getFrame() */
    mBtAttackType = 3; /* setBtAttackData(start, end, 700.0f, 3) */
    mBtStartFrame = (f32)hio_s(0x74);
    mBtEndFrame = (f32)hio_s(0x76);
    mBtMaxDis = 700.0f;
    mBtNowFrame = frame;
}
VERIFY(0x02142870, &daFm_c::spAttackVJump);

/* 021428FC (unnamed) */
void daFm_c::spAttackJump() {
    WWHD_FUNC(0x021428FC, void, this);
    mBtHeight = hio_f(0x78);
    mBtBodyR = hio_f(0x7C);
    f32 frame = gabi::load<f32>(gabi::ea(mpMorf.get()) + 0x9C);
    mBtAttackType = 1;
    mBtStartFrame = (f32)hio_s(0x80);
    mBtEndFrame = (f32)hio_s(0x82);
    mBtMaxDis = 700.0f;
    mBtNowFrame = frame;
}
VERIFY(0x021428FC, &daFm_c::spAttackJump);

/* 0214996C */
static void* daFm_HIO_c_ct(void* p) {
    WWHD_FUNC(0x0214996C, void*, p);
    if (p == nullptr) {
        p = operator_new(0x15C);
        if (p == nullptr) return p;
    }
    u32 h = gabi::ea(p);
    gabi::store<u32>(h, 0x1000F360);   /* vtable */
    gabi::call(0x02552BE8, h + 0x130); /* JntHit_HIO_c::JntHit_HIO_c */
    gabi::store<u8>(h + 0x006, 0);
    gabi::store<u8>(h + 0x007, 0);
    gabi::store<u8>(h + 0x008, 0);
    gabi::store<u8>(h + 0x004, 0);
    gabi::store<u8>(h + 0x01F, 0);
    gabi::store<u8>(h + 0x00B, 0);
    gabi::store<u8>(h + 0x009, 0);
    gabi::store<u8>(h + 0x00A, 0);
    gabi::store<u8>(h + 0x00C, 0);
    gabi::store<u8>(h + 0x005, 0);

    gabi::store<s16>(h + 0x068, 0x1E);
    gabi::store<f32>(h + 0x06C, 240.0f); /* mSpAttackVJumpHeight */
    gabi::store<f32>(h + 0x070, 100.0f);
    gabi::store<s16>(h + 0x074, 0);
    gabi::store<s16>(h + 0x076, 0xF);
    gabi::store<f32>(h + 0x078, 120.0f); /* mSpAttackJumpHeight */
    gabi::store<f32>(h + 0x07C, 100.0f);
    gabi::store<s16>(h + 0x080, 5);
    gabi::store<s16>(h + 0x082, 0xF);
    gabi::store<s16>(h + 0x11C, 100);
    gabi::store<u8>(h + 0x00E, 0);
    gabi::store<u8>(h + 0x00D, 0);
    gabi::store<s16>(h + 0x062, 0x78); /* mFreezeDuration */

    for (int i = 0; i < 6; i++) {
        switch (i) {
        case 0: case 1: case 2: case 3: case 5:
            gabi::store<u8>(h + 0x0F + i, 1);
            break;
        default:
            gabi::store<u8>(h + 0x0F + i, 0);
            break;
        }
    }
    gabi::store<f32>(h + 0x050, 0.0f);
    gabi::store<f32>(h + 0x054, 0.0f);
    gabi::store<f32>(h + 0x058, 0.0f);
    gabi::store<s16>(h + 0x05C, 0);
    gabi::store<s16>(h + 0x05E, 0);
    gabi::store<s16>(h + 0x060, 0);
    gabi::store<f32>(h + 0x020, -10.0f);
    gabi::store<f32>(h + 0x024, 5.0f);
    gabi::store<f32>(h + 0x028, -10.0f);
    gabi::store<f32>(h + 0x02C, 0.0f);
    gabi::store<f32>(h + 0x030, 0.0f);
    gabi::store<f32>(h + 0x034, -30.0f);
    gabi::store<f32>(h + 0x038, 40.0f);
    gabi::store<f32>(h + 0x03C, -20.0f);
    gabi::store<f32>(h + 0x040, 0.0f);
    gabi::store<f32>(h + 0x044, 0.0f);
    gabi::store<f32>(h + 0x048, 0.0f);
    gabi::store<f32>(h + 0x04C, 0.0f);
    gabi::store<f32>(h + 0x104, 0.0f);
    gabi::store<f32>(h + 0x108, -20.0f);
    gabi::store<f32>(h + 0x10C, -0.0f);

    gabi::store<f32>(h + 0x0A8, 600.0f);
    gabi::store<s16>(h + 0x064, 0xC); /* mMaxHealth */
    gabi::store<s16>(h + 0x066, 0x28);
    gabi::store<s16>(h + 0x084, 0x5A);
    gabi::store<s16>(h + 0x086, 0xF0);
    gabi::store<s16>(h + 0x088, 0x3C);
    gabi::store<s16>(h + 0x08A, 1);
    gabi::store<s16>(h + 0x08C, 0x3C);
    gabi::store<s16>(h + 0x08E, 0x78);
    gabi::store<s16>(h + 0x090, 10);
    gabi::store<s16>(h + 0x092, 0x11);
    gabi::store<s16>(h + 0x094, 0);
    gabi::store<s16>(h + 0x096, 500);
    gabi::store<s16>(h + 0x098, 18000);
    gabi::store<s16>(h + 0x09A, 7000);
    gabi::store<f32>(h + 0x0C0, 15.0f);
    gabi::store<f32>(h + 0x120, 0.01f);
    gabi::store<f32>(h + 0x124, 1.0f);
    gabi::store<f32>(h + 0x128, 0.1f);
    gabi::store<f32>(h + 0x12C, 0.02f);
    gabi::store<f32>(h + 0x0C4, 290.0f);
    gabi::store<f32>(h + 0x0C8, 50.0f); /* HD: GameCube 80.0f (body sphere radius in setCollision) */
    gabi::store<f32>(h + 0x0CC, 7.0f);
    gabi::store<f32>(h + 0x0D0, 2.0f);
    gabi::store<f32>(h + 0x0D4, 5.0f);
    gabi::store<f32>(h + 0x0D8, 80.0f);
    gabi::store<f32>(h + 0x0DC, 100.0f);
    gabi::store<f32>(h + 0x0E0, 150.0f);
    gabi::store<f32>(h + 0x0E4, 500.0f);
    gabi::store<f32>(h + 0x0E8, 800.0f);
    gabi::store<f32>(h + 0x0EC, 30.0f);
    gabi::store<f32>(h + 0x0F0, 25.0f);
    gabi::store<f32>(h + 0x0F4, 9.0f);
    gabi::store<f32>(h + 0x0F8, -2.5f);
    gabi::store<f32>(h + 0x0FC, 16.0f);
    gabi::store<s16>(h + 0x09C, 5);
    gabi::store<s16>(h + 0x100, 0x1E);
    gabi::store<f32>(h + 0x110, 20.0f);
    gabi::store<f32>(h + 0x114, 50.0f);
    gabi::store<f32>(h + 0x118, -20.0f);
    gabi::store<f32>(h + 0x0B0, 30.0f);
    gabi::store<s16>(h + 0x0A0, 0x14);
    gabi::store<s16>(h + 0x0A2, 0x96);
    gabi::store<f32>(h + 0x0B4, 1000.0f);
    gabi::store<f32>(h + 0x0B8, 1000.0f);
    gabi::store<s16>(h + 0x09E, 0x96);
    gabi::store<f32>(h + 0x0AC, 300.0f);
    gabi::store<f32>(h + 0x0BC, 750.0f);
    gabi::store<s16>(h + 0x0A6, 10);
    return p;
}
VERIFY(0x0214996C, daFm_HIO_c_ct);

/* 02149CD8: static initialisation (header statics, then l_HIO) */
static void __sinit_d_a_fm_cpp() {
    WWHD_FUNC(0x02149CD8, void, (u32)0);
    sinit_header_statics(0x10463DF0, 0x101B5228);
    daFm_HIO_c_ct(gabi::at<void>(L_HIO));
}
VERIFY(0x02149CD8, __sinit_d_a_fm_cpp);

/* 02149D8C */
static BOOL daFmIsDelete(void* i_this) {
    WWHD_FUNC(0x02149D8C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02149D8C, daFmIsDelete);

/* 02149D94 (unnamed) */
void daFm_c::modeDeleteInit() {
    WWHD_FUNC(0x02149D94, void, this);
}
VERIFY(0x02149D94, &daFm_c::modeDeleteInit);

/* dBgS_ObjAcch destructor (inline): this TU's vtables back, then dBgS_Acch::~dBgS_Acch */
static inline void fm_objacch_dt(dBgS_ObjAcch* a) {
    gabi::store<u32>(gabi::ea(a) + 0x20, 0x1000F2B0);
    gabi::store<u32>(gabi::ea(a) + 0x14, 0x1000F2C0);
    gabi::call(0x024EFD9C, a, 0);
}

/* 02149D98: daFm_c deleting destructor (compiler-generated; HD virtual destructor) */
static void daFm_c_dt(daFm_c* self, s32 flags) {
    WWHD_FUNC(0x02149D98, void, self, flags);
    if (self == nullptr) return;
    fm_objacch_dt(&self->mEnemyIce.mBgAcch);
    gabi::call(0x02018034, gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    /* dBgS_ObjLinChk::~dBgS_ObjLinChk (inline): this TU's vtables back, then cBgS_LinChk::~cBgS_LinChk */
    u32 lc = gabi::ea(&self->mLinChk);
    gabi::store<u32>(lc + 0x58, 0x1000F300);
    gabi::store<u32>(lc + 0x64, 0x1000F290);
    gabi::store<u32>(lc + 0x20, 0x1000F280);
    gabi::call(0x02008B4C, lc, 0);
    dCcD_Cyl_dt(&self->mCyl, 2);
    gabi::call(0x02515AE8, &self->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mStts2, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    gabi::call(0x025E89F8, self->mInvisibleModel, 2); /* mDoExt_invisibleModel::~mDoExt_invisibleModel */
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);
    fm_objacch_dt(&self->mObjAcch);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x02149D98, daFm_c_dt);

/* 02149ED0: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable + 0x14) */
static void fm_SafeString_assureTermination(void*) {
    WWHD_FUNC(0x02149ED0, void, (u32)0);
}
VERIFY(0x02149ED0, fm_SafeString_assureTermination);
