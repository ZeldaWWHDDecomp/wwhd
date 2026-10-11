/** Master Sword pedestal sword. */
#include "d/actor/d_a_obj_vmsms.h"

// HD: archive resources use this translation unit's SafeString vtable.
const gc_tu_data gc_tu = { .safeString = 0x10032ED4 };
const char daObjVmsms_c::M_arcname[] = "VmsMS";
enum { dRes_INDEX_VMSMS_BDL_VMSMS_e = 3 };
using vmsms_save_ptr = gptr<vmsms_save_c>;
static vmsms_save_ptr l_save;

/* 023B37E0 */
BOOL daObjVmsms_c::create_heap() {
    J3DModelData* mdl_data =
        (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VMSMS_BDL_VMSMS_e);
    JUT_ASSERT(0x5B, mdl_data != NULL);
    if (mdl_data == NULL)
        return FALSE;
    mModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);
    return mModel != NULL;
}

/* 023B3888 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((daObjVmsms_c*)i_this)->create_heap();
}

/* 023B388C */
// HD: the matcher labels this check_demo; the function implements init_mtx.
void daObjVmsms_c::init_mtx() {
    mModel->setBaseScale(scale);
}

bool daObjVmsms_c::check_demo() const {
    // HD: save event flags begin at 0x644.
    return vmsms_isEventBit(&l_save->mEvent,
                           dSv_event_flag_c::MASTER_SWORD_CUTSCENE);
}

/* 023B38AC */
cPhs_State daObjVmsms_c::_create() {
    fopAcM_ct(this, daObjVmsms_c);
    if (check_demo())
        return cPhs_ERROR_e;
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0))
            return cPhs_ERROR_e;
        fopAcM_SetMtx(this, mModel->getBaseTRMtx());
        init_mtx();
    }
    return ret;
}

/* 023B3990 */
bool daObjVmsms_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 023B39C0 */
bool daObjVmsms_c::_execute() {
    if (dComIfGp_event_runCheck()) {
        // HD: the old event-manager entry point takes only the event name.
        if (vmsms_startCheckOld(dComIfGp_getPEvtManager(), "master_sword"))
            fopAcM_delete(this);
    }
    return true;
}

/* 023B3A20 */
bool daObjVmsms_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mModel, &tevStr);
    mDoExt_modelUpdateDL(mModel);
    return true;
}

/* 023B3A7C */
static cPhs_State Mthd_Create(daObjVmsms_c* i_this) { return i_this->_create(); }
/* 023B3A80 */
static bool Mthd_Delete(daObjVmsms_c* i_this) { return i_this->_delete(); }
/* 023B3A84 */
static bool Mthd_Execute(daObjVmsms_c* i_this) { return i_this->_execute(); }
/* 023B3A88 */
static bool Mthd_Draw(daObjVmsms_c* i_this) { return i_this->_draw(); }

/* 023B3A8C */
// HD: translation-unit static initialisation.
static void __sinit_d_a_obj_vmsms_cpp() {
    sinit_header_statics(0x1046C8CC, 0x101CDB30);
}

/* 023B3B20 */
// HD: empty SafeString deleting destructor.
static void SafeString_dt(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023B3B34 */
// HD: actor deleting destructor.
static void daObjVmsms_c_dt(daObjVmsms_c* i_this, u32 flags) {
    if (i_this) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 023B3B88 */
// HD: empty SafeString termination method.
static void SafeString_assureTerminationImpl(void*) {}

/* 023B3B8C */
static BOOL Mthd_IsDelete(daObjVmsms_c*) { return TRUE; }

daObjVmsms_c::daObjVmsms_c() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* WWHD glue (tools/verify/autoglue.py): private bindings
data l_save 101F84DC
str 10032F40 "VmsMS"
str 10032EFC "d_a_obj_vmsms.cpp"
str 10032F10 "mdl_data != NULL" renamed # HD spells NULL as 0
str 10032F20 "master_sword"
ext 025B8B94 BOOL vmsms_isEventBit(void*, u16)
ext 025445B8 BOOL vmsms_startCheckOld(void*, const char*)
*/
