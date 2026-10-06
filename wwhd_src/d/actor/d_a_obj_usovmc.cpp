/**
 * d_a_obj_usovmc.cpp (WWHD)
 * Object - Usovmc (box model with background collision, dBgS_MoveBgActor)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_usovmc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x100328C8) /* "Usovmc" */
#define SAFESTRING_VTBL 0x10032818
#define ACT_VTBL 0x100328D0 /* HD: daObjUsovmc::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046C7DC)

enum {
    dRes_INDEX_USOVMC_BDL_VMCBX_e = 4,
    dRes_INDEX_USOVMC_DZB_VMCBS_e = 7,
};

namespace daObjUsovmc {
struct Act_c : dBgS_MoveBgActor {
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mModel;
};
WWHD_OFFSET(Act_c, mModel, 0x3E8);
}  // namespace daObjUsovmc
using daObjUsovmc::Act_c;

/* 023AEA04 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x023AEA04, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_USOVMC_BDL_VMCBX_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x4a, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x10032898), 0x4A, STR(0x10032888));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}
VERIFY(0x023AEA04, &Act_c::CreateHeap);

/* 023AEB94 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x023AEB94, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -80.0f, -1.0f, -80.0f, 80.0f, 205.0f, 80.0f);
    return TRUE;
}
VERIFY(0x023AEB94, &Act_c::Create);

/* 023AEAA0 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x023AEAA0, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x023AEAA0, &Act_c::set_mtx);

/* 023AEB74 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x023AEB74, void, this);
    J3DModel_setBaseScale(mModel, &scale);
    set_mtx();
}
VERIFY(0x023AEB74, &Act_c::init_mtx);

/* 023AEC0C */
BOOL Act_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x023AEC0C, BOOL, this, mtx);
    set_mtx();
    gabi::store<u32>(gabi::ea(mtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x023AEC0C, &Act_c::Execute);

/* 023AEC48 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x023AEC48, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x023AEC48, &Act_c::Draw);

/* 023AE8E8 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x023AE8E8, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_USOVMC_DZB_VMCBS_e, 0, 0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT */
            JUT_ASSERT_fail(STR(0x10032830), 0x73, STR(0x10032844));
    }
    return phase_state;
}
VERIFY(0x023AE8E8, &Act_c::Mthd_Create);

/* 023AE9B8 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x023AE9B8, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x023AE9B8, &Act_c::Mthd_Delete);

/* method table entries (HD: tail branches) */
/* 023AECE0 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x023AECE0, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x023AECE0, Mthd_Create);
/* 023AECE4 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023AECE4, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x023AECE4, Mthd_Delete);
/* 023AECE8 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023AECE8, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x023AECE8, Mthd_Execute);
/* 023AECEC: MoveBGDraw (virtual Draw) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023AECEC, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x023AECEC, Mthd_Draw);
/* 023AECFC: MoveBGIsDelete (virtual IsDelete) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x023AECFC, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x023AECFC, Mthd_IsDelete);

/* 023AED0C */
static void __sinit_d_a_obj_usovmc_cpp() {
    WWHD_FUNC(0x023AED0C, void, (u32)0);
    sinit_header_statics(0x1046C7C0, 0x101CD7B4);
}
VERIFY(0x023AED0C, __sinit_d_a_obj_usovmc_cpp);

/* 023AEDA0: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023AEDA0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023AEDA0, trivial_dt);


/* 023AEDB4: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE) */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    WWHD_FUNC(0x023AEDB4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023AEDB4, MoveBgActor_IsDelete);

/* 023AEDBC: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_) */
static void SafeString_empty(void*) {
    WWHD_FUNC(0x023AEDBC, void, (u32)0);
}
VERIFY(0x023AEDBC, SafeString_empty);

/* 023AEDC0 */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x023AEDC0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023AEDC0, Act_c_Delete);

/* 023AEDC8: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x023AEDC8, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x023AEDC8, Act_c_dt);

