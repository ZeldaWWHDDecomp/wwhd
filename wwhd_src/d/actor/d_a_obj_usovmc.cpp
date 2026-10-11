/**
 * d_a_obj_usovmc.cpp (WWHD)
 * Object - Usovmc (box model with background collision, dBgS_MoveBgActor)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_usovmc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_usovmc.h"
using daObjUsovmc::Act_c;

const gc_tu_data gc_tu = { .safeString = 0x10032818, .headerStatics = {0x1046C7C0, 0x101CD7B4, 0x1046C7CC} };
const char Act_c::M_arcname[] = "Usovmc";
static Mtx34 M_tmp_mtx;

/* 023AEA04 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_USOVMC_BDL_VMCBX_e);
    JUT_ASSERT(0x4A, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}

/* 023AEB94 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -80.0f, -1.0f, -80.0f, 80.0f, 205.0f, 80.0f);
    return TRUE;
}

/* 023AE8E8 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_USOVMC_DZB_VMCBS_e, 0, 0);
        // HD: assertion line is one lower than GameCube.
        JUT_ASSERT(0x73, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 023AEDC0 */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 023AE9B8 */
BOOL Act_c::Mthd_Delete() {
    BOOL ret = MoveBGDelete();
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return ret;
}

/* 023AEAA0 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 023AEB74 */
void Act_c::init_mtx() {
    mModel->setBaseScale(scale);
    set_mtx();
}

/* 023AEC0C */
BOOL Act_c::Execute(Mtx34** mtx) {
    set_mtx();
    // HD: store the matrix address using the guest pointer representation.
    *reinterpret_cast<gptr<Mtx34>*>(mtx) = &M_tmp_mtx;
    return TRUE;
}

/* 023AEC48 */
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return TRUE;
}

/* 023AECE0 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}

/* 023AECE4 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}

/* 023AECE8 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}

/* 023AECEC */
// HD: MoveBGDraw (virtual Draw)
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}

/* 023AECFC */
// HD: MoveBGIsDelete (virtual IsDelete)
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

// HD: compiler-generated and template code of this translation unit.
/* 023AED0C */
static void __sinit_d_a_obj_usovmc_cpp() {
    __sinit_header_statics();
}

/* 023AEDA0 */
// HD: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8)
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023AEDB4 */
// HD: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE)
static BOOL MoveBgActor_IsDelete(Act_c*) {
    return TRUE;
}

/* 023AEDBC */
// HD: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_)
static void SafeString_empty(void*) {
}

/* 023AEDC8 */
// HD: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c)
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data M_tmp_mtx 1046C7DC
str 100328C8 # "Usovmc"
str 10032898 in 023AEA04 # "d_a_obj_usovmc.cpp"
str 10032830 in 023AE8E8 # "d_a_obj_usovmc.cpp"
str 10032888 "model_data != NULL" renamed # HD: model_data != 0
str 10032844 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)" renamed # HD: numeric phase names
*/
