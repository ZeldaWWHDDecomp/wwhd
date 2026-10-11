/**
 * d_a_obj_xfuta.cpp (WWHD) - X_futa object.
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_xfuta.h"
using daObjXfuta::Act_c;
const gc_tu_data gc_tu = { .safeString = 0x100338B8,
    .headerStatics = {0x1046CA34, 0x101CDEE8, 0x1046CA40} };
const char Act_c::M_arcname[] = "X_futa";
// HD: retain the guest demo mode and lighting table reads.
static s32 l_demoMode;
static int dComIfGp_demo_mode() { return l_demoMode; }
static s32 tev_mode[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 91, 92, 93, 94};

/* 023BBF28 */
bool Act_c::create_heap() {
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_X_FUTA_BDL_X_FUTA_e);
    JUT_ASSERT(0x105, mdl_data != NULL);
    mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11000002);
    return mdl_data != NULL && mpModel != NULL;
}

/* 023BBFD8 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    // HD: tail branch preserves the full result register of create_heap.
    return Xfuta_create_heap_result((Act_c*)i_this);
}

/* 023BBFDC */
void Act_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    J3DModel_calc(mpModel);
}

/* 023BC0D0 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0)) {
            set_mtx();
            fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(mpModel));
        } else {
            phase_state = cPhs_ERROR_e;
        }
    }
    return phase_state;
}

/* 023BC194 */
bool Act_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 023BC1C4 */
bool Act_c::_execute() {
    f32 x = home.pos.x, y = home.pos.y, z = home.pos.z;
    // HD: assignment order of the inlined cXyz copy.
    current.pos.y = y;
    current.pos.z = z;
    current.pos.x = x;
    set_mtx();
    return true;
}

/* 023BC200 */
bool Act_c::_draw() {
    if (dComIfGp_demo_mode() != 1) {
        gabi::Local<cXyz> pos;
        pos->set(0.0f, 0.0f, 0.0f);
        g_env_light.settingTevStruct(tev_mode[1], pos.get(), &tevStr);
        dComIfGd_setListBG();
        g_env_light.setLightTevColorType(mpModel, &tevStr);
        mDoExt_modelUpdateDL(mpModel);
        dComIfGd_setList();
    }
    return true;
}

/* 023BC2C0 */
static cPhs_State Mthd_Create(void* i_this) { return static_cast<Act_c*>(i_this)->_create(); }
/* 023BC2C4 */
static bool Mthd_Delete(void* i_this) { return static_cast<Act_c*>(i_this)->_delete(); }
/* 023BC2C8 */
static bool Mthd_Execute(void* i_this) { return static_cast<Act_c*>(i_this)->_execute(); }
/* 023BC2CC */
static bool Mthd_Draw(void* i_this) { return static_cast<Act_c*>(i_this)->_draw(); }

/* 023BC2D0 */
static void __sinit_d_a_obj_xfuta_cpp() { __sinit_header_statics(); }
/* 023BC364 */
// HD: compiler-emitted deleting destructor of a header static.
static void header_static_dt(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 023BC37C */
// HD: actor deleting destructor.
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 023BC378 */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_() {}
/* 023BC3D0 */
static BOOL Mthd_IsDelete(void*) { return TRUE; }

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 10033910 # "X_futa"
str 100338E0 # "d_a_obj_xfuta.cpp"
str 100338F4 "mdl_data != NULL" renamed # HD: assertion prints null as 0
data l_demoMode 101D6010
data tev_mode 101CDEB0
ext 023BBF28 u32 Xfuta_create_heap_result(Act_c*)
*/
