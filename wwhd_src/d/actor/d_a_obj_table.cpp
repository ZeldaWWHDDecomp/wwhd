/**
 * d_a_obj_table.cpp (WWHD)
 * Object - Table (two models, background collision through dBgS_MoveBgActor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_table.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_table.h"
using daObjTable::Act_c;

// HD: this translation unit's SafeString vtable.
const gc_tu_data gc_tu = { .safeString = 0x10030530 };
const char Act_c::M_arcname[] = "Table";
static Mtx34 M_tmp_mtx;

/* 023996D8 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u8 Act_c::prm_get_mdl() { return (u8)PrmAbstract(this, PRM_MDL_W, PRM_MDL_S); }

/* 023991FC */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data;
    J3DModel* model;
    if (prm_get_mdl() == 0) {
        model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TABLE_BDL_YTBLE_e);
        JUT_ASSERT(0x51, model_data != NULL);
        model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
        mpModel = model;
    } else {
        model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TABLE_BDL_QCFIS_e);
        JUT_ASSERT(0x57, model_data != NULL);
        model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
        mpModel = model;
    }
    return model != nullptr; /* HD: tests the returned pointer, not a reload of mpModel */
}

/* 0239940C */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -80.0f, -1.0f, -80.0f, 80.0f, 205.0f, 80.0f);
    if (prm_get_mdl() == 2 && mpBgW != nullptr) {
        if (mpBgW->ChkUsed()) {
            dComIfG_Bgsp()->Release(mpBgW);
        }
    }
    return TRUE;
}

/* 023990AC */
cPhs_State Act_c::Mthd_Create() {
    cPhs_State phase_state;
    fopAcM_ct(this, Act_c);
    phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (prm_get_mdl() == 0) {
            phase_state = MoveBGCreate(M_arcname, dRes_INDEX_TABLE_DZB_YTBLE_e, 0, (u32)-1);
        } else {
            phase_state = MoveBGCreate(M_arcname, dRes_INDEX_TABLE_DZB_QCFIS_e, 0, (u32)-1);
        }
        JUT_ASSERT(0x8B, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)); // HD: assertion line differs
    }
    return phase_state;
}

/* 0239967C */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 023991B0 */
BOOL Act_c::Mthd_Delete() {
    BOOL ret = MoveBGDelete();
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return ret;
}

/* 02399318 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 023993EC */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 023994C8 */
BOOL Act_c::Execute(Mtx34** matrix) {
    set_mtx();
    *reinterpret_cast<gptr<Mtx34>*>(matrix) = &M_tmp_mtx; // HD: guest pointer representation
    return TRUE;
}

/* 02399504 */
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 0239959C */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 023995A0 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 023995A4 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 023995A8 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 023995B8 */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 023995C8 */
static void __sinit_d_a_obj_table_cpp() {
    sinit_header_statics(0x1046C0A0, 0x101CD084);
}

/* 0239965C */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* ---- leftover functions of the translation unit ---- */

/* 02399670 */
static BOOL table_MoveBgActor_IsDelete(void* p) {
    return TRUE;
}

/* 02399678 */
static void table_SafeString_assureTermination(void* p) {
}

/* 02399684 */
static void table_Act_c_dt(void* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1) operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 100305E0 # "Table"
str 100305B0 in 023991FC # "d_a_obj_table.cpp"
str 10030548 in 023990AC # "d_a_obj_table.cpp"
str 100305A0 "model_data != NULL" renamed # HD: "model_data != 0"
str 1003055C "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)" renamed # HD assertion uses numeric phase values
data M_tmp_mtx 1046C0BC
*/
