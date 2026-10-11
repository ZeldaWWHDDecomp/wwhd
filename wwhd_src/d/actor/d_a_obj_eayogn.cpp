/**
 * d_a_obj_eayogn.cpp (WWHD).
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_eayogn.h"
const gc_tu_data gc_tu = { .safeString = 0x100278DC,
    .headerStatics = {0x104696C0, 0x101C8E50, 0x104696CC} };
const char daObjEayogn_c::M_arcname[7] = "Eayogn";

/* 02339330 */
BOOL daObjEayogn_c::create_heap() {
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EAYOGN_BDL_EAYOGN_e);
    JUT_ASSERT(0x5C, mdl_data != NULL);
    if (mdl_data == NULL)
        return FALSE;
    mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    if (mpModel == NULL)
        return FALSE;
    cBgD_t* bgw_data = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_EAYOGN_DZB_EAYOGN_e);
    mpBgW = dBgW_NewSet(bgw_data, dBgW::MOVE_BG_e, J3DModel_getBaseTRMtx(mpModel));
    return mpBgW != NULL;
}

/* 02339410 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((daObjEayogn_c*)i_this)->create_heap();
}

/* 02339414 */
void daObjEayogn_c::init_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 023394F4 */
cPhs_State daObjEayogn_c::_create() {
    fopAcM_ct(this, daObjEayogn_c);
    if (!check_ev_bit())
        return cPhs_ERROR_e;
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret != cPhs_COMPLEATE_e)
        return ret;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
        return cPhs_ERROR_e;
    fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(mpModel));
    init_mtx();
    dComIfG_Bgsp()->Regist(mpBgW, this);
    mpBgW->Move();
    fopAcM_setCullSizeBox(this, -550.0f, -50.0f, -600.0f, 550.0f, 50.0f, 600.0f);
    return cPhs_COMPLEATE_e;
}

/* 0233962C */
bool daObjEayogn_c::_delete() {
    if (heap != NULL && mpBgW != NULL && mpBgW->ChkUsed()) {
        dComIfG_Bgsp()->Release(mpBgW);
        mpBgW = NULL;
    }
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return true;
}

/* 023396A4 */
bool daObjEayogn_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}

/* 0233973C */
static cPhs_State Mthd_Create(void* i_this) { return ((daObjEayogn_c*)i_this)->_create(); }
/* 02339740 */
static bool Mthd_Delete(void* i_this) { return ((daObjEayogn_c*)i_this)->_delete(); }
/* 02339744 */
// HD: _execute is folded into the method-table entry.
static BOOL Mthd_Execute(void*) { return TRUE; }
/* 0233974C */
static bool Mthd_Draw(void* i_this) { return ((daObjEayogn_c*)i_this)->_draw(); }
/* 02339750 */
static void __sinit_d_a_obj_eayogn_cpp() { __sinit_header_statics(); }
/* 023397E4 */
// HD: compiler-emitted deleting destructor of a header static.
static void header_static_dt(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 023397F8 */
// HD: actor deleting destructor.
static void daObjEayogn_c_dt(daObjEayogn_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 0233984C */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_() {}
/* 02339850 */
static BOOL Mthd_IsDelete(void*) { return TRUE; }

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 10027950 # "Eayogn"
str 10027914 # "d_a_obj_eayogn.cpp"
str 10027904 "mdl_data != NULL" renamed # HD: assertion prints null as 0
*/
