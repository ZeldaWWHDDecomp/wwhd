/**
 * d_a_obj_AjavW.cpp (WWHD)
 * Object - Water inside Jabun's cave (texture-scrolled model with moving background collision).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_AjavW.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_AjavW.h"

// HD: this translation unit's SafeString vtable.
const gc_tu_data gc_tu = { .safeString = 0x100249EC };
static const char l_arcname[] = "AjavW";

/* inlined into _create (fopAcM_ct) */
daObjAjavW_c::daObjAjavW_c() {
    // HD: GHS constructs the base and animation controller explicitly.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    mDoExt_btkAnm::ct(&mBtkAnm);
}

/* 0231606C */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((daObjAjavW_c*)i_this)->create_heap();
}

/* 02315F14 */
bool daObjAjavW_c::create_heap() {
    bool ret = true;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_AJAVW_BDL_AJAVW_e);
    J3DAnmTextureSRTKey* pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_AJAVW_BTK_AJAVW_e);
    if (pModelData == nullptr || pBtk == nullptr) {
        JUT_ASSERT(0xA7, FALSE);
        ret = false;
    } else {
        mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000222);
        s32 btkRet = mBtkAnm.init(pModelData, pBtk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
        /* HD: no dBgW when the model is missing */
        if (mpModel == nullptr) {
            ret = false;
        } else {
            cBgD_t* bgd = (cBgD_t*) dComIfG_getObjectRes(l_arcname, dRes_INDEX_AJAVW_DZB_AJAVW_e);
            dBgW* bgw = dBgW_NewSet(bgd, dBgW::MOVE_BG_e, mpModel->getBaseTRMtx());
            mpBgW = bgw;
            if (mpModel == nullptr || !btkRet || bgw == nullptr)
                ret = false;
        }
    }
    return ret;
}

/* 02316070 */
cPhs_State daObjAjavW_c::_create() {
    fopAcM_ct(this, daObjAjavW_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0x8C0) != 0) { /* HD: != 0 (GameCube == 1) */
            if (dComIfG_Bgsp()->Regist(mpBgW, this)) {
                ret = cPhs_ERROR_e;
            } else {
                fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
                mpModel->setBaseScale(scale);
                mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
                fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}

/* 02316210 */
bool daObjAjavW_c::_delete() {
    dComIfG_resDelete(&mPhs, l_arcname);
    if (heap != nullptr && mpBgW != nullptr) {
        if (mpBgW->ChkUsed()) {
            dComIfG_Bgsp()->Release(mpBgW);
        }
        mpBgW = nullptr;
    }
    return true;
}

/* 0231628C */
bool daObjAjavW_c::_execute() {
    mBtkAnm.play();
    if (mpBgW != nullptr && mpBgW->ChkUsed())
        mpBgW->Move();
    return true;
}

/* 023162E0 */
bool daObjAjavW_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG1, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mBtkAnm.entry(mpModel->getModelData(), mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return true;
}

/* method table entries (HD: tail branches) */
/* 0231620C */
static cPhs_State daObjAjavW_Create(daObjAjavW_c* i_this) {
    return i_this->_create();
}
/* 02316288 */
static BOOL daObjAjavW_Delete(daObjAjavW_c* i_this) {
    return i_this->_delete();
}
/* 023162DC */
static BOOL daObjAjavW_Execute(daObjAjavW_c* i_this) {
    return i_this->_execute();
}
/* 02316350 */
static BOOL daObjAjavW_Draw(daObjAjavW_c* i_this) {
    return i_this->_draw();
}
/* 02316354 */
static BOOL daObjAjavW_IsDelete(daObjAjavW_c* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0231635C */
static void __sinit_d_a_obj_AjavW_cpp() {
    sinit_header_statics(0x10469084, 0x101C7AF8);
}

/* 023163F0 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02316404 */
static void daObjAjavW_c_dt(daObjAjavW_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02316458 */
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 100249E4 # "AjavW"
str 10024A1C # "d_a_obj_AjavW.cpp"
str 10024A18 "FALSE" renamed # HD: "0" (FALSE expanded by the original assert macro)
*/
