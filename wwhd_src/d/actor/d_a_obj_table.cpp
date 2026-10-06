/**
 * d_a_obj_table.cpp (WWHD)
 * Object - Table (two models, background collision through dBgS_MoveBgActor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_table.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x100305E0) /* "Table" */
#define SAFESTRING_VTBL 0x10030530
#define FILE_NAME STR(0x100305B0)
#define ACT_VTBL 0x100305E8 /* HD: daObjTable::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046C0BC)

enum {
    dRes_INDEX_TABLE_BDL_YTBLE_e = 4,
    dRes_INDEX_TABLE_BDL_QCFIS_e = 5,
    dRes_INDEX_TABLE_DZB_YTBLE_e = 9, /* HD archive order */
    dRes_INDEX_TABLE_DZB_QCFIS_e = 8,
};

namespace daObjTable {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_MDL_W = 8, PRM_MDL_S = 0 };
    u8 prm_get_mdl();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** matrix);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
}  // namespace daObjTable
using daObjTable::Act_c;

/* 023996D8: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x023996D8, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x023996D8, PrmAbstract);
u8 Act_c::prm_get_mdl() { return (u8)PrmAbstract(this, PRM_MDL_W, PRM_MDL_S); }

/* 023991FC */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x023991FC, BOOL, this);
    J3DModelData* model_data;
    J3DModel* model;
    if (prm_get_mdl() == 0) {
        model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TABLE_BDL_YTBLE_e, SAFESTRING_VTBL);
        if (model_data == nullptr) /* JUT_ASSERT(0x51, model_data != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x51, STR(0x100305A0));
        model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
        mpModel = model;
    } else {
        model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TABLE_BDL_QCFIS_e, SAFESTRING_VTBL);
        if (model_data == nullptr) /* JUT_ASSERT(0x57, model_data != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x57, STR(0x100305A0));
        model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
        mpModel = model;
    }
    return model != nullptr; /* HD: tests the returned pointer, not a reload of mpModel */
}
VERIFY(0x023991FC, &Act_c::CreateHeap);

/* 0239940C */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0239940C, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -80.0f, -1.0f, -80.0f, 80.0f, 205.0f, 80.0f);
    if (prm_get_mdl() == 2 && mpBgW != nullptr) {
        if (dBgW_ChkUsed(mpBgW)) {
            cBgS_Release(dComIfG_Bgsp(), mpBgW);
        }
    }
    return TRUE;
}
VERIFY(0x0239940C, &Act_c::Create);

/* 023990AC */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x023990AC, cPhs_State, this);
    cPhs_State phase_state;
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (prm_get_mdl() == 0) {
            phase_state = MoveBGCreate(M_arcname, dRes_INDEX_TABLE_DZB_YTBLE_e, 0, (u32)-1);
        } else {
            phase_state = MoveBGCreate(M_arcname, dRes_INDEX_TABLE_DZB_QCFIS_e, 0, (u32)-1);
        }
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x8b, ...) */
            JUT_ASSERT_fail(STR(0x10030548), 0x8b, STR(0x1003055C));
    }
    return phase_state;
}
VERIFY(0x023990AC, &Act_c::Mthd_Create);

/* 023991B0 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x023991B0, BOOL, this);
    BOOL ret = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return ret;
}
VERIFY(0x023991B0, &Act_c::Mthd_Delete);

/* 02399318 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02399318, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x02399318, &Act_c::set_mtx);

/* 023993EC */
void Act_c::init_mtx() {
    WWHD_FUNC(0x023993EC, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x023993EC, &Act_c::init_mtx);

/* 023994C8 */
BOOL Act_c::Execute(Mtx34** matrix) {
    WWHD_FUNC(0x023994C8, BOOL, this, matrix);
    set_mtx();
    gabi::store<u32>(gabi::ea(matrix), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x023994C8, &Act_c::Execute);

/* 02399504: HD: no simple shadow (GameCube dComIfGd_setSimpleShadow) */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x02399504, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x02399504, &Act_c::Draw);

/* 0239967C */
static BOOL Act_c_Delete(Act_c* i_this) {
    WWHD_FUNC(0x0239967C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0239967C, Act_c_Delete);

/* method table entries (HD: tail branches) */
/* 0239959C */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0239959C, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0239959C, Mthd_Create);
/* 023995A0 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023995A0, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x023995A0, Mthd_Delete);
/* 023995A4 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023995A4, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x023995A4, Mthd_Execute);
/* 023995A8: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023995A8, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x023995A8, Mthd_Draw);
/* 023995B8: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x023995B8, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x023995B8, Mthd_IsDelete);

/* 023995C8 */
static void __sinit_d_a_obj_table_cpp() {
    WWHD_FUNC(0x023995C8, void, (u32)0);
    sinit_header_statics(0x1046C0A0, 0x101CD084);
}
VERIFY(0x023995C8, __sinit_d_a_obj_table_cpp);

/* 0239965C: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0239965C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0239965C, trivial_dt);

/* ---- leftover functions of the translation unit ---- */

/* 02399670 dBgS_MoveBgActor::IsDelete (out-of-line copy; daObjTable::Act_c vtable slot 10030624) */
static BOOL table_MoveBgActor_IsDelete(void* p) {
    WWHD_FUNC(0x02399670, BOOL, p);
    return TRUE;
}
VERIFY(0x02399670, table_MoveBgActor_IsDelete);

/* 02399678 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10030544, after the destructor 0239965C */
static void table_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02399678, void, p);
}
VERIFY(0x02399678, table_SafeString_assureTermination);

/* 02399684 daObjTable::Act_c::~Act_c (deleting; vtable slot 100305F4): only the fopAc_ac_c base */
static void table_Act_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02399684, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x02399684, table_Act_c_dt);
