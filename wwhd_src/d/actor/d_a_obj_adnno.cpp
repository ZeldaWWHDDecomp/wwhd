/**
 * d_a_obj_adnno.cpp (WWHD) - unused Triforce prayer mural.
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_adnno.h"
const gc_tu_data gc_tu = { .safeString = 0x10024714,
    .headerStatics = {0x10468F04, 0x101C790C, 0x10468F10} };

/* 02312FB8 */
BOOL daObjAdnno_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Adnno", dRes_INDEX_ADNNO_BDL_ADNNO_e);
    JUT_ASSERT(0x5C, modelData != NULL);
    for (s32 i = 0; i < 16; i++) {
        mpModel[i] = mDoExt_J3DModel__create(modelData, 0x80000, 0x37441422);
        if (!mpModel[i])
            return FALSE;
    }
    return TRUE;
}

/* 023130A0 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daObjAdnno_c*)i_this)->CreateHeap();
}

/* 023130A4 */
void daObjAdnno_c::set_mtx() {
    for (s32 i = 0; i < 16; i++) {
        J3DModel_setBaseScale(mpModel[i], &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        mDoMtx_stack_c::transM((i % 4) * 40.0f - 60.0f, 60.0f - (i / 4) * 40.0f, 0.0f);
        J3DModel_setBaseTRMtx(mpModel[i], mDoMtx_stack_c::get());
    }
}

/* 0231323C */
void daObjAdnno_c::CreateInit() {
    fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(mpModel[0]));
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    fopAcM_setCullSizeFar(this, 1.0f);
    set_mtx();
}

/* 023132BC */
// HD: _create is emitted directly as the method-table entry.
cPhs_State daObjAdnno_c::_create() {
    fopAcM_ct(this, daObjAdnno_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, "Adnno");
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x9C00))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return ret;
}

/* 02313384 */
// HD: _delete is emitted directly as the method-table entry.
bool daObjAdnno_c::_delete() {
    dComIfG_resDeleteDemo(&mPhs, "Adnno");
    return true;
}

/* 023133B4 */
// HD: the event-bit/material swap and model draw loop were removed.
bool daObjAdnno_c::_draw() {
    dComIfGd_setListBG();
    dComIfGd_setList();
    return true;
}

/* 02313410 */
// HD: _execute is emitted directly as the method-table entry.
bool daObjAdnno_c::_execute() {
    set_mtx();
    return true;
}

/* 02313434 */
static void __sinit_d_a_obj_adnno_cpp() { __sinit_header_statics(); }
/* 023134C8 */
// HD: compiler-emitted deleting destructor of a header static.
static void header_static_dt(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 023134E4 */
// HD: actor deleting destructor.
static void daObjAdnno_c_dt(daObjAdnno_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 02313538 */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_() {}
/* 023134DC */
static BOOL daObjAdnno_IsDelete(void*) { return TRUE; }

/* 0231353C */
// HD: color conversion companion, absent from the GameCube actor TU.
static void colorToFloat(AdnnoFloatColor* out, GXColor* color) {
    f32 r = f32(u8(color->r)) / 255.0f;
    f32 g = f32(u8(color->g)) / 255.0f;
    f32 b = f32(u8(color->b)) / 255.0f;
    f32 a = f32(u8(color->a)) / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 1002473C in 02312FB8 # "Adnno"
str 1002470C in 023132BC # "Adnno"
str 10024794 in 02313384 # "Adnno"
str 10024744 # "d_a_obj_adnno.cpp"
str 10024758 "modelData != NULL" renamed # HD: assertion prints null as 0
*/
