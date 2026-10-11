/**
 * d_a_obj_pbka.cpp (WWHD)
 * Object - Windfall Island - Bomb Shop - Ceiling fan.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_pbka.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_pbka.h"

// HD: GHS emits a SafeString vtable and header statics for each translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x1002E5DC,
    .headerStatics = {0x1046BBE0, 0x101CC354, 0},
};

/* 02382550 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daObjPbka_c*>(i_this)->CreateHeap();
}

/* 023824B4 */
BOOL daObjPbka_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Pbka", dRes_INDEX_PBKA_BDL_PBKA_e);
    JUT_ASSERT(0x51, modelData != NULL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    mpModel = model;
    return model != nullptr; // HD: tests the returned pointer
}

/* 0238262C */
void daObjPbka_c::CreateInit() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -300.0f, -300.0f, -300.0f, 300.0f, 300.0f, 300.0f);
    fopAcM_setCullSizeFar(this, 1.0f);
    set_mtx();
}

/* 02382554 */
void daObjPbka_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 023826A4 */
cPhs_State daObjPbka_c::_create() {
    fopAcM_ct(this, daObjPbka_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, "Pbka");
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x680) == 0) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return phase_state;
}

/* 02382768 */
bool daObjPbka_c::_delete() {
    dComIfG_resDeleteDemo(&mPhase, "Pbka");
    return TRUE;
}

/* 02382798 */
bool daObjPbka_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* 02382830 */
bool daObjPbka_c::_execute() {
    current.angle.y += 0x500;
    shape_angle.y = current.angle.y;
    mDoAud_seStart(JA_SE_OBJ_BOMB_SHOP_FAN, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    set_mtx();
    return TRUE;
}

/* 0238293C */
static BOOL daObjPbka_IsDelete(void* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02382894 */
static void __sinit_d_a_obj_pbka_cpp() {
    sinit_header_statics(0x1046BBE0, 0x101CC354);
}

/* 02382928 */
// HD: SafeString deleting destructor, this translation unit's copy.
static void SafeString_dtor(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02382944 */
static void daObjPbka_c_dt(daObjPbka_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02382998 */
// HD: SafeString assureTerminationImpl_, this translation unit's empty copy.
static void SafeString_assureTerminationImpl(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 1002E604 in 023824B4 # "Pbka"
str 1002E5D4 in 023826A4 # "Pbka"
str 1002E640 in 02382768 # "Pbka"
str 1002E60C # "d_a_obj_pbka.cpp"
str 1002E620 "modelData != NULL"
*/
