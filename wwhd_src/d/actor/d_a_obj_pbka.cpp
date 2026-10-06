/**
 * d_a_obj_pbka.cpp (WWHD)
 * Object - Windfall Island - Bomb Shop - Ceiling fan.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_pbka.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1002E5DC
#define PBKA_VTBL 0x1002E5F4 /* HD: daObjPbka_c vtable */

enum { dRes_INDEX_PBKA_BDL_PBKA_e = 3 };

struct daObjPbka_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
};
WWHD_OFFSET(daObjPbka_c, mpModel, 0x3B4);

/* 023824B4 */
BOOL daObjPbka_c::CreateHeap() {
    WWHD_FUNC(0x023824B4, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1002E604) /* "Pbka" */, dRes_INDEX_PBKA_BDL_PBKA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x51, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1002E60C), 0x51, STR(0x1002E620));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}
VERIFY(0x023824B4, &daObjPbka_c::CreateHeap);

/* 02382550 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02382550, BOOL, i_this);
    return static_cast<daObjPbka_c*>(i_this)->CreateHeap();
}
VERIFY(0x02382550, CheckCreateHeap);

/* 02382554 */
void daObjPbka_c::set_mtx() {
    WWHD_FUNC(0x02382554, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02382554, &daObjPbka_c::set_mtx);

/* 0238262C */
void daObjPbka_c::CreateInit() {
    WWHD_FUNC(0x0238262C, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -300.0f, -300.0f, -300.0f, 300.0f, 300.0f, 300.0f);
    cullSizeFar = 1.0f;
    set_mtx();
}
VERIFY(0x0238262C, &daObjPbka_c::CreateInit);

/* 023826A4: daObjPbka_Create (_create inlined) */
cPhs_State daObjPbka_c::_create() {
    WWHD_FUNC(0x023826A4, cPhs_State, this);
    /* fopAcM_ct(this, daObjPbka_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = PBKA_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, STR(0x1002E5D4) /* "Pbka" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x02382550 /* CheckCreateHeap */, 0x680) == 0) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return phase_state;
}
VERIFY(0x023826A4, &daObjPbka_c::_create);

/* 02382768: daObjPbka_Delete */
bool daObjPbka_c::_delete() {
    WWHD_FUNC(0x02382768, BOOL, this);
    dComIfG_resDelete(&mPhase, STR(0x1002E640) /* "Pbka" */); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x02382768, &daObjPbka_c::_delete);

/* 02382798: daObjPbka_Draw */
bool daObjPbka_c::_draw() {
    WWHD_FUNC(0x02382798, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x02382798, &daObjPbka_c::_draw);

/* 02382830: daObjPbka_Execute */
bool daObjPbka_c::_execute() {
    WWHD_FUNC(0x02382830, BOOL, this);
    current.angle.y += 0x500;
    shape_angle.y = current.angle.y;
    /* fopAcM_seStartCurrent(this, JA_SE_OBJ_BOMB_SHOP_FAN, 0) */
    mDoAud_seStart(0x619F, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    set_mtx();
    return TRUE;
}
VERIFY(0x02382830, &daObjPbka_c::_execute);

/* 02382894 */
static void __sinit_d_a_obj_pbka_cpp() {
    WWHD_FUNC(0x02382894, void, (u32)0);
    sinit_header_statics(0x1046BBE0, 0x101CC354);
}
VERIFY(0x02382894, __sinit_d_a_obj_pbka_cpp);

/* 02382928: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02382928, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02382928, trivial_dt);

/* 0238293C */
static BOOL daObjPbka_IsDelete(void* i_this) {
    WWHD_FUNC(0x0238293C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0238293C, daObjPbka_IsDelete);

/* 02382944: daObjPbka_c deleting destructor */
static void daObjPbka_c_dt(daObjPbka_c* i_this, s32 flags) {
    WWHD_FUNC(0x02382944, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02382944, daObjPbka_c_dt);

/* 02382998: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02382998, void, p);
}
VERIFY(0x02382998, empty_virtual);
