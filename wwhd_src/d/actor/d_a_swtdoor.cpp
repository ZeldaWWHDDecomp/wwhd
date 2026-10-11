/**
 * d_a_swtdoor.cpp (WWHD)
 * Object - Forsaken Fortress tower (Helmaroc King fight) - gray "doors" blocking windows.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swtdoor.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_swtdoor.h"

// HD: this translation unit's inline SafeString vtable.
const gc_tu_data gc_tu = { .safeString = 0x1003EDE4 };

/* 024A3D5C */
static BOOL daSwtdoor_Draw(swtdoor_class* i_this) {
    J3DModel* model = i_this->model;
    g_env_light.setLightTevColorType(model, &i_this->tevStr);
    mDoExt_modelUpdateDL(model);
    return TRUE;
}

/* 024A3DB0 */
static BOOL daSwtdoor_Execute(swtdoor_class* i_this) {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    if (dComIfGs_isSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(i_this)) && i_this->home.pos.y > -300.0f)
        i_this->home.pos.y -= 10.0f;

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + i_this->home.pos.y, i_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    Mtx34* m = calc_mtx();
    J3DModel* model = i_this->model;
    model->setBaseTRMtx(m);
    return TRUE;
}

/* 024A3F8C */
static BOOL daSwtdoor_IsDelete(swtdoor_class* i_this) {
    return TRUE;
}

/* 024A3F94 */
// HD: resDelete replaces resDeleteDemo.
static BOOL daSwtdoor_Delete(swtdoor_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Swtdoor");
    return TRUE;
}

/* 024A3FC4 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    swtdoor_class* i_this = (swtdoor_class*)i_ac;
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes("Swtdoor", dRes_INDEX_SWTDOOR_BMD_SWTDOOR_e);
    i_this->model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->model == NULL)
        return FALSE;
    return TRUE;
}

/* 024A4034 */
static cPhs_State daSwtdoor_Create(fopAc_ac_c* i_ac) {
    fopAcM_ct((swtdoor_class*)i_ac, swtdoor_class);
    swtdoor_class* i_this = (swtdoor_class*)i_ac;

    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, "Swtdoor");
    if (rt == cPhs_ERROR_e)
        return cPhs_ERROR_e;
    if (rt != cPhs_COMPLEATE_e)
        return rt;

    i_this->field_0x29c = (fopAcM_GetParam(i_this) >> 0) & 0xFF;
    if (i_this->field_0x29c == 0xFF)
        i_this->field_0x29c = 0;

    i_this->mSwitchNo = (fopAcM_GetParam(i_this) >> 24) & 0xFF;
    if (fopAcM_entrySolidHeap(i_ac, useHeapInit, 0x3000) == 0)
        return cPhs_ERROR_e;

    fopAcM_SetMin(i_this, -2000.0f, -1000.0f, -2000.0f);
    fopAcM_SetMax(i_this, 2000.0f, 1000.0f, 2000.0f);
    fopAcM_SetMtx(i_this, i_this->model->getBaseTRMtx());
    i_this->home.pos.y = 0.0f;
    return cPhs_COMPLEATE_e;
}

/* inlined into daSwtdoor_Create (fopAcM_ct) */
swtdoor_class::swtdoor_class() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 024A4198 */
static void __sinit_d_a_swtdoor_cpp() {
    sinit_header_statics(0x1046E1D4, 0x101D1604);
}

/* 024A422C */
// HD: this translation unit's SafeString deleting destructor.
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 024A4240 */
// HD: actor deleting destructor.
static void swtdoor_class_dt(swtdoor_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A4294 */
// HD: this translation unit's empty SafeString termination method.
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 1003EE14 "Swtdoor" in 024A3F94
str 1003EE1C "Swtdoor" in 024A3FC4
str 1003EE38 "Swtdoor" in 024A4034
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
*/
