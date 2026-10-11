/**
 * d_a_obj_rforce.cpp (WWHD) - rotating-force platform.
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_rforce.h"
using daObjRforce::Act_c;
const gc_tu_data gc_tu = { .safeString = 0x1002ED70,
    .headerStatics = {0x1046BD5C, 0x101CC8B8, 0x1046BD68} };
const char Act_c::M_arcname[] = "StpTetu";

/* 02388388 */
void Act_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
    J3DModel_calc(mpModel);
}

/* 0238847C */
bool Act_c::create_heap() {
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_STPTETU_BDL_STPTETU_e);
    JUT_ASSERT(0x57, mdl_data != NULL);
    mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11000002);
    set_mtx();
    cBgD_t* bgw_data = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_STPTETU_DZB_STPTETU_e);
    JUT_ASSERT(0x64, bgw_data != NULL);
    if (bgw_data != NULL) {
        mpBgw = (RforceBgW*)new_dBgW();
        if (mpBgw != NULL && mpBgw->Set(bgw_data, dBgW::MOVE_BG_e, &mtx)) {
            return false;
        }
    }
    return mdl_data != NULL && mpModel != NULL && bgw_data != NULL && mpBgw != NULL;
}

/* 023885CC */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    // HD: tail branch preserves the full result register of create_heap.
    return Rforce_create_heap_result((Act_c*)i_this);
}

/* 023885D0 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
            return cPhs_ERROR_e;
        fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(mpModel));
        dComIfG_Bgsp()->Regist(mpBgw, this);
        mpBgw->SetCrrFunc(dBgS_MoveBGProc_Typical);
    }
    return ret;
}

/* 023886B0 */
bool Act_c::_delete() {
    if (heap != NULL && mpBgw != NULL && mpBgw->ChkUsed()) {
        dComIfG_Bgsp()->Release(mpBgw);
    }
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 02388720 */
bool Act_c::_execute() {
    set_mtx();
    mpBgw->Move();
    return true;
}

/* 02388758 */
bool Act_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    dComIfGd_setListBG();
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}

/* 023887F0 */
static cPhs_State Mthd_Create(void* i_this) {
    return static_cast<Act_c*>(i_this)->_create();
}
/* 023887F4 */
static bool Mthd_Delete(void* i_this) {
    return static_cast<Act_c*>(i_this)->_delete();
}
/* 023887F8 */
static bool Mthd_Execute(void* i_this) {
    return static_cast<Act_c*>(i_this)->_execute();
}
/* 023887FC */
static bool Mthd_Draw(void* i_this) {
    return static_cast<Act_c*>(i_this)->_draw();
}

/* 02388800 */
static void __sinit_d_a_obj_rforce_cpp() {
    __sinit_header_statics();
}
/* 02388894 */
// HD: compiler-emitted deleting destructor for a header static.
static void header_static_dt(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 023888A8 */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_() {}
/* 023888AC */
// HD: actor deleting destructor.
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 02388900 */
static BOOL Mthd_IsDelete(void*) {
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 1002EDD8 # "StpTetu"
str 1002EDB8 # "d_a_obj_rforce.cpp"
str 1002ED98 "mdl_data != NULL" renamed # HD: assertion prints null as 0
str 1002EDA8 "bgw_data != NULL" renamed # HD: assertion prints null as 0
ext 0238847C u32 Rforce_create_heap_result(Act_c*)
*/
