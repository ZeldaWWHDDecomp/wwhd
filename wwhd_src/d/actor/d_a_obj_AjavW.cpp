/**
 * d_a_obj_AjavW.cpp (WWHD)
 * Object - Water inside Jabun's cave (texture-scrolled model with moving background collision).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_AjavW.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define l_arcname STR(0x100249E4) /* "AjavW" (one static array, used by every call) */
#define SAFESTRING_VTBL 0x100249EC
#define AJAVW_VTBL 0x10024A04 /* HD: daObjAjavW_c vtable */

enum {
    dRes_INDEX_AJAVW_BDL_AJAVW_e = 5,
    dRes_INDEX_AJAVW_BTK_AJAVW_e = 8,
    dRes_INDEX_AJAVW_DZB_AJAVW_e = 0xB,
};
enum { TEV_TYPE_BG1_l = 2 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 024F2478 dBgW_NewSet(cBgD_t*, u32 flag, Mtx34*) -> dBgW* (as in d_a_lwood) */
static inline dBgW* dBgW_NewSet(void* bgd, u32 flag, Mtx34* mtx) { return gabi::call<dBgW*>(0x024F2478, bgd, flag, mtx); }

struct daObjAjavW_c : fopAc_ac_c {
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ gptr<dBgW> mpBgW;
};
WWHD_OFFSET(daObjAjavW_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(daObjAjavW_c, mpBgW, 0x42C);

static inline J3DModelData* model_getModelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }

/* 02315F14 */
bool daObjAjavW_c::create_heap() {
    WWHD_FUNC(0x02315F14, bool, this);
    bool ret = true;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_AJAVW_BDL_AJAVW_e, SAFESTRING_VTBL);
    J3DAnmTextureSRTKey* pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_AJAVW_BTK_AJAVW_e, SAFESTRING_VTBL);
    if (pModelData == nullptr || pBtk == nullptr) {
        JUT_ASSERT_fail(STR(0x10024A1C), 0xA7, STR(0x10024A18)); /* JUT_ASSERT(0xa7, FALSE) */
        ret = false;
    } else {
        mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000222);
        s32 btkRet = mBtkAnm.init(pModelData, pBtk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
        /* HD: no dBgW when the model is missing */
        if (mpModel == nullptr) {
            ret = false;
        } else {
            void* bgd = dComIfG_getObjectRes(l_arcname, dRes_INDEX_AJAVW_DZB_AJAVW_e, SAFESTRING_VTBL);
            dBgW* bgw = dBgW_NewSet(bgd, 1 /* cBgW::MOVE_BG_e */, J3DModel_getBaseTRMtx(mpModel));
            mpBgW = bgw;
            if (mpModel == nullptr || !btkRet || bgw == nullptr)
                ret = false;
        }
    }
    return ret;
}
VERIFY(0x02315F14, &daObjAjavW_c::create_heap);

/* 0231606C */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0231606C, BOOL, i_this);
    return ((daObjAjavW_c*)i_this)->create_heap();
}
VERIFY(0x0231606C, solidHeapCB);

/* 02316070 */
cPhs_State daObjAjavW_c::_create() {
    WWHD_FUNC(0x02316070, cPhs_State, this);
    /* fopAcM_ct(this, daObjAjavW_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = AJAVW_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x0231606C /* solidHeapCB */, 0x8C0) != 0) { /* HD: != 0 (GameCube == 1) */
            if (dBgS_Regist(dComIfG_Bgsp(), mpBgW, this)) {
                ret = cPhs_ERROR_e;
            } else {
                cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
                J3DModel_setBaseScale(mpModel, &scale);
                mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
                cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}
VERIFY(0x02316070, &daObjAjavW_c::_create);

/* 02316210 */
bool daObjAjavW_c::_delete() {
    WWHD_FUNC(0x02316210, bool, this);
    dComIfG_resDelete(&mPhs, l_arcname);
    if (heap != nullptr && mpBgW != nullptr) {
        if (dBgW_ChkUsed(mpBgW)) {
            cBgS_Release(dComIfG_Bgsp(), mpBgW);
        }
        mpBgW = nullptr;
    }
    return true;
}
VERIFY(0x02316210, &daObjAjavW_c::_delete);

/* 0231628C */
bool daObjAjavW_c::_execute() {
    WWHD_FUNC(0x0231628C, bool, this);
    mBtkAnm.play();
    if (mpBgW != nullptr && dBgW_ChkUsed(mpBgW))
        dBgW_Move(mpBgW);
    return true;
}
VERIFY(0x0231628C, &daObjAjavW_c::_execute);

/* 023162E0 */
bool daObjAjavW_c::_draw() {
    WWHD_FUNC(0x023162E0, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG1_l, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBtkAnm.entry(model_getModelData(mpModel), mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
VERIFY(0x023162E0, &daObjAjavW_c::_draw);

/* method table entries (HD: tail branches) */
/* 0231620C */
static cPhs_State daObjAjavW_Create(daObjAjavW_c* i_this) {
    WWHD_FUNC(0x0231620C, cPhs_State, i_this);
    return i_this->_create();
}
VERIFY(0x0231620C, daObjAjavW_Create);
/* 02316288 */
static BOOL daObjAjavW_Delete(daObjAjavW_c* i_this) {
    WWHD_FUNC(0x02316288, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02316288, daObjAjavW_Delete);
/* 023162DC */
static BOOL daObjAjavW_Execute(daObjAjavW_c* i_this) {
    WWHD_FUNC(0x023162DC, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x023162DC, daObjAjavW_Execute);
/* 02316350 */
static BOOL daObjAjavW_Draw(daObjAjavW_c* i_this) {
    WWHD_FUNC(0x02316350, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02316350, daObjAjavW_Draw);
/* 02316354 */
static BOOL daObjAjavW_IsDelete(daObjAjavW_c* i_this) {
    WWHD_FUNC(0x02316354, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02316354, daObjAjavW_IsDelete);

/* 0231635C */
static void __sinit_d_a_obj_AjavW_cpp() {
    WWHD_FUNC(0x0231635C, void, (u32)0);
    sinit_header_statics(0x10469084, 0x101C7AF8);
}
VERIFY(0x0231635C, __sinit_d_a_obj_AjavW_cpp);

/* 023163F0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023163F0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023163F0, trivial_dt);

/* 02316404: daObjAjavW_c deleting destructor (mDoExt_btkAnm has a trivial destructor) */
static void daObjAjavW_c_dt(daObjAjavW_c* i_this, s32 flags) {
    WWHD_FUNC(0x02316404, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02316404, daObjAjavW_c_dt);

/* 02316458: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02316458, void, p);
}
VERIFY(0x02316458, empty_virtual);
