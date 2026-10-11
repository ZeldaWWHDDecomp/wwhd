/**
 * d_a_obj_rflw.cpp (WWHD)
 * Object - Rflw (Rito flower: swings when something bumps into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_rflw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_rflw.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002ECCC,
    .headerStatics = {0x1046BD40, 0x101CC864},
};
static dCcD_SrcCyl l_cyl_src;
static BOOL nodeCallBack(J3DNode*, s32);

/* 02387CCC */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daObjRflw_c*)i_this)->CreateHeap();
}

/* 02387BB8 */
BOOL daObjRflw_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Rflw", dRes_INDEX_RFLW_BDL_PHANA_e);
    JUT_ASSERT(0xAA, modelData != NULL);

    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpModel != nullptr) {
        /* HD: the joint is looked up by name (GameCube: strcmp over all joints) */
        JUTNameTab* nameTab = mpModel->getModelData()->getJointName();
        s32 idx = nameTab->getIndex("joint2");
        if (idx >= 0)
            mpModel->getModelData()->getJointNodePointer((u16)idx)->setCallBack(nodeCallBack);
        mpModel->setUserArea(reinterpret_cast<uintptr_t>(this));
    } else {
        return FALSE;
    }
    return TRUE;
}

/* 02387DA8 */
void daObjRflw_c::CreateInit() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    cullSizeFar = 1.0f;
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    m408 = 0;
    set_mtx();
}

/* inlined into daObjRflw_Create */
cPhs_State daObjRflw_c::_create() {
    fopAcM_ct(this, daObjRflw_c);

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, "Rflw");
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x0C60))
            return cPhs_ERROR_e;
        CreateInit();
    }
    m410 = 0;
    return phase_state;
}

/* inlined into daObjRflw_Draw */
BOOL daObjRflw_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    /* HD: no simple shadow */
    return TRUE;
}

/* inlined into daObjRflw_Delete */
BOOL daObjRflw_c::_delete() {
    dComIfG_resDeleteDemo(&mPhs, "Rflw");
    return TRUE;
}

/* inlined into daObjRflw_Execute */
BOOL daObjRflw_c::_execute() {
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);

    if ((m40C > 0x38 || m410 == 0) && mCyl.ChkCoHit()) {
        fopAc_ac_c* hitAc = mCyl.GetCoHitAc();
        cXyz dir_to_obj;
        dir_to_obj = current.pos - hitAc->current.pos;
        s16 dir_angle = cM_atan2s(dir_to_obj.x, dir_to_obj.z);
        m40E = (s16)(dir_angle - 0x4000);
        m410 = 1;
        m40C = 0;
        m40A = 0;
        fopAcM_seStart(this, JA_SE_OBJ_TREE_SWING_S, 0);
    }

    if (m410 != 0) {
        m40C = m40C + 1;
        if (m40C < 8) {
            m408 = (s16)ftoi(cM_ssin(m40A) * (f32)(0x100 - m40C) * 4.0f);
            m40A = m40A + 0x1000;
        } else {
            m408 = (s16)ftoi(cM_ssin(m40A) * (f32)(0x100 - m40C) * 2.0f);
            m40A = m40A + 0x0800;
        }
        if (m40C > 0x100) {
            m410 = 0;
            m40C = 0;
            m408 = 0;
        }
    }

    set_mtx();
    return TRUE;
}

/* 02387A74 */
static BOOL nodeCallBack(J3DNode* node, s32 calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        u32 jntNo = ((J3DJoint*)node)->getJntNo();
        J3DModel* model = j3dSys.getModel();
        daObjRflw_c* plant = (daObjRflw_c*)model->getUserArea();
        if (plant != nullptr) {
            PSMTXCopy(((J3DModel_rflw_view*)model)->getAnmMtx(jntNo), calc_mtx());
            cMtx_XrotM(calc_mtx(), plant->m40E);
            cMtx_YrotM(calc_mtx(), plant->m408);
            cMtx_XrotM(calc_mtx(), (s16)-plant->m40E);
            model->setAnmMtx(jntNo, calc_mtx());
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* 02387CD0 */
void daObjRflw_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02387E5C */
static cPhs_State daObjRflw_Create(void* i_this) {
    return ((daObjRflw_c*)i_this)->_create();
}

/* 02387F9C */
static BOOL daObjRflw_Delete(void* i_this) {
    return ((daObjRflw_c*)i_this)->_delete();
}

/* 02387FCC */
static BOOL daObjRflw_Draw(void* i_this) {
    return ((daObjRflw_c*)i_this)->_draw();
}

/* 02388064 */
static BOOL daObjRflw_Execute(void* i_this) {
    return ((daObjRflw_c*)i_this)->_execute();
}

/* 02388310 */
static BOOL daObjRflw_IsDelete(void* i_this) {
    return TRUE;
}

/* 02388268 */
static void __sinit_d_a_obj_rflw_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 023882FC */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02388318 */
// HD: daObjRflw_c deleting destructor
static void daObjRflw_c_dt(daObjRflw_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02388384 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002ECC4 "Rflw" in 02387E5C
str 1002ED04 "Rflw" in 02387BB8
str 1002ED50 "Rflw" in 02387F9C
str 1002ED14
str 1002ED28 "modelData != NULL" renamed
str 1002ED0C "joint2"
data l_cyl_src 101CC820
*/
