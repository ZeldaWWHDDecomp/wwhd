/** Master Sword pedestal. */
#include "d/actor/d_a_obj_vmsdz.h"

// HD: archive resources use this translation unit's SafeString vtable.
const gc_tu_data gc_tu = { .safeString = 0x10032E6C };
const char daObjVmsdz_c::M_arcname[] = "VmsDZ";
enum { dRes_INDEX_VMSDZ_BDL_VMSDZ_e = 3 };

/* 023B346C */
BOOL daObjVmsdz_c::create_heap() {
    J3DModelData* mdl_data =
        (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VMSDZ_BDL_VMSDZ_e);
    JUT_ASSERT(0x59, mdl_data != NULL);
    if (mdl_data == NULL)
        return FALSE;
    mModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    return mModel != NULL;
}

/* 023B3514 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((daObjVmsdz_c*)i_this)->create_heap();
}

/* 023B3518 */
void daObjVmsdz_c::init_mtx() {
    mModel->setBaseScale(scale);
}

/* 023B3538 */
cPhs_State daObjVmsdz_c::_create() {
    fopAcM_ct(this, daObjVmsdz_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
            return cPhs_ERROR_e;
        fopAcM_SetMtx(this, mModel->getBaseTRMtx());
        init_mtx();
    }
    return ret;
}

/* 023B35FC */
// HD: resDelete replaces resDeleteDemo.
bool daObjVmsdz_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 023B362C */
bool daObjVmsdz_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return true;
}

/* 023B36C4 */
static cPhs_State Mthd_Create(daObjVmsdz_c* i_this) { return i_this->_create(); }
/* 023B36C8 */
static bool Mthd_Delete(daObjVmsdz_c* i_this) { return i_this->_delete(); }
/* 023B36CC */
static bool Mthd_Execute(daObjVmsdz_c*) { return true; }
/* 023B36D4 */
static bool Mthd_Draw(daObjVmsdz_c* i_this) { return i_this->_draw(); }

/* 023B36D8 */
// HD: translation-unit static initialisation.
static void __sinit_d_a_obj_vmsdz_cpp() {
    sinit_header_statics(0x1046C8B0, 0x101CDABC);
}

/* 023B376C */
// HD: empty SafeString deleting destructor.
static void SafeString_dt(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023B3780 */
// HD: actor deleting destructor.
static void daObjVmsdz_c_dt(daObjVmsdz_c* i_this, u32 flags) {
    if (i_this) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 023B37D4 */
// HD: empty SafeString termination method.
static void SafeString_assureTerminationImpl() {}

/* 023B37D8 */
static BOOL Mthd_IsDelete(daObjVmsdz_c*) { return TRUE; }

daObjVmsdz_c::daObjVmsdz_c() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* WWHD glue (tools/verify/autoglue.py): private bindings
str 10032EC8 "VmsDZ"
str 10032E94 "d_a_obj_vmsdz.cpp"
str 10032EA8 "mdl_data != NULL" renamed # HD spells NULL as 0
*/
