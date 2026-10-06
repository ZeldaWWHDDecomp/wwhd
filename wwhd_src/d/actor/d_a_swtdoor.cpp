/**
 * d_a_swtdoor.cpp (WWHD)
 * Object - Forsaken Fortress tower (Helmaroc King fight) - gray "doors" blocking windows.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swtdoor.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003EDE4 /* this TU's sead::SafeString vtable */
#define SWTDOOR_VTBL 0x1003EDFC    /* swtdoor_class vtable (HD virtual destructor) */

enum { dRes_INDEX_SWTDOOR_BMD_SWTDOOR_e = 3 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D672C / 025D673C fopAcM_SetMin / fopAcM_SetMax (HD: out of line) */
static inline void fopAcM_SetMin_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }

struct swtdoor_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> model;
    /* 0x3B8 */ be<u8> field_0x29c;
    /* 0x3B9 */ be<u8> mSwitchNo;
};
WWHD_OFFSET(swtdoor_class, model, 0x3B4);
WWHD_OFFSET(swtdoor_class, mSwitchNo, 0x3B9);

/* 024A3D5C: HD: no settingTevStruct here (it moved to Execute in GameCube too) */
static BOOL daSwtdoor_Draw(swtdoor_class* i_this) {
    WWHD_FUNC(0x024A3D5C, BOOL, i_this);
    J3DModel* model = i_this->model;
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    mDoExt_modelUpdateDL(model);
    return TRUE;
}
VERIFY(0x024A3D5C, daSwtdoor_Draw);

/* 024A3DB0 */
static BOOL daSwtdoor_Execute(swtdoor_class* i_this) {
    WWHD_FUNC(0x024A3DB0, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    if (dComIfGs_isSwitch(i_this->mSwitchNo, fopAcM_GetRoomNo(i_this)) && i_this->home.pos.y > -300.0f)
        i_this->home.pos.y -= 10.0f;

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + i_this->home.pos.y, i_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    Mtx34* m = calc_mtx();
    J3DModel* model = i_this->model;
    J3DModel_setBaseTRMtx(model, m);
    return TRUE;
}
VERIFY(0x024A3DB0, daSwtdoor_Execute);

/* 024A3F8C */
static BOOL daSwtdoor_IsDelete(swtdoor_class* i_this) {
    WWHD_FUNC(0x024A3F8C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A3F8C, daSwtdoor_IsDelete);

/* 024A3F94: HD: resDelete (GameCube resDeleteDemo) */
static BOOL daSwtdoor_Delete(swtdoor_class* i_this) {
    WWHD_FUNC(0x024A3F94, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1003EE14) /* "Swtdoor" */);
    return TRUE;
}
VERIFY(0x024A3F94, daSwtdoor_Delete);

/* 024A3FC4 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x024A3FC4, BOOL, i_ac);
    swtdoor_class* i_this = (swtdoor_class*)i_ac;
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes(STR(0x1003EE1C) /* "Swtdoor" */, dRes_INDEX_SWTDOOR_BMD_SWTDOOR_e, SAFESTRING_VTBL);
    i_this->model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->model == NULL)
        return FALSE;
    return TRUE;
}
VERIFY(0x024A3FC4, useHeapInit);

/* 024A4034 */
static cPhs_State daSwtdoor_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x024A4034, cPhs_State, i_ac);
    /* fopAcM_ct(i_ac, swtdoor_class): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = SWTDOOR_VTBL;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    swtdoor_class* i_this = (swtdoor_class*)i_ac;

    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, STR(0x1003EE38) /* "Swtdoor" */);
    if (rt == cPhs_ERROR_e)
        return cPhs_ERROR_e;
    if (rt != cPhs_COMPLEATE_e)
        return rt;

    i_this->field_0x29c = (fopAcM_GetParam(i_this) >> 0) & 0xFF;
    if (i_this->field_0x29c == 0xFF)
        i_this->field_0x29c = 0;

    i_this->mSwitchNo = (fopAcM_GetParam(i_this) >> 24) & 0xFF;
    if (fopAcM_entrySolidHeap(i_ac, 0x024A3FC4 /* useHeapInit */, 0x3000) == 0)
        return cPhs_ERROR_e;

    fopAcM_SetMin_l(i_this, -2000.0f, -1000.0f, -2000.0f);
    fopAcM_SetMax_l(i_this, 2000.0f, 1000.0f, 2000.0f);
    /* fopAcM_SetMtx(i_this, model->getBaseTRMtx()): null-preserving address */
    u32 mdl = gabi::ea(i_this->model.get());
    i_this->cullMtx = mdl != 0 ? mdl + 0xC8 : 0;
    i_this->home.pos.y = 0.0f;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A4034, daSwtdoor_Create);

/* 024A4198: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_swtdoor_cpp() {
    WWHD_FUNC(0x024A4198, void, (u32)0);
    sinit_header_statics(0x1046E1D4, 0x101D1604);
}
VERIFY(0x024A4198, __sinit_d_a_swtdoor_cpp);

/* 024A422C: sead::SafeString deleting destructor (this TU's vtable 0x1003EDE4) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024A422C, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x024A422C, SafeString_dt);

/* 024A4240: swtdoor_class deleting destructor (compiler-generated, HD virtual destructor) */
static void swtdoor_class_dt(swtdoor_class* i_this, s32 flags) {
    WWHD_FUNC(0x024A4240, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A4240, swtdoor_class_dt);

/* 024A4294: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024A4294, void, (u32)0);
}
VERIFY(0x024A4294, SafeString_assureTerminationImpl);
