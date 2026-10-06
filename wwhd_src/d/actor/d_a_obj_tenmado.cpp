/**
 * d_a_obj_tenmado.cpp (WWHD)
 * Object - Tenmado (two-leaf shutter window that opens when a switch is set)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_tenmado.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10030AE8) /* "Tenmado" */
#define SAFESTRING_VTBL 0x10030A18
#define ACT_VTBL 0x10030AF0 /* HD: daObjTenmado::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046C470)

enum {
    dRes_INDEX_TENMADO_BDL_MMADOL_e = 4,
    dRes_INDEX_TENMADO_BDL_MMADOR_e = 5,
    dRes_INDEX_TENMADO_DZB_MMADO_e = 8,
};
enum { JA_SE_OBJ_TENMADO_SHTR_OP = 0x69BF };

namespace daObjTenmado {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_SWSAVE2_W = 8,
        PRM_SWSAVE2_S = 8,
    };
    u32 prm_get_swSave();
    u32 prm_get_swSave2();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** mtx);
    BOOL Draw();

    /* 0x3E0 */ be<f32> m2C8;
    /* 0x3E4 */ be<s32> m2CC;
    /* 0x3E8 */ request_of_phase_process_class mPhase;
    /* 0x3F0 */ gptr<J3DModel> mModel1;
    /* 0x3F4 */ gptr<J3DModel> mModel2;
};
WWHD_OFFSET(Act_c, m2CC, 0x3E4);
WWHD_OFFSET(Act_c, mModel2, 0x3F4);
}  // namespace daObjTenmado
using daObjTenmado::Act_c;

/* 023A03FC: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x023A03FC, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x023A03FC, PrmAbstract);
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_swSave2() { return PrmAbstract(this, PRM_SWSAVE2_W, PRM_SWSAVE2_S); }

/* 0239FC78 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0239FC78, BOOL, this);
    J3DModelData* model_data_l = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TENMADO_BDL_MMADOL_e, SAFESTRING_VTBL);
    if (model_data_l == nullptr) /* JUT_ASSERT(85, model_data_l != NULL) */
        JUT_ASSERT_fail(STR(0x10030A88), 0x55, STR(0x10030A9C));
    J3DModel* model = mDoExt_J3DModel__create(model_data_l, 0, 0x11020203);
    mModel1 = model;
    if (model == nullptr) {
        return FALSE;
    }
    J3DModelData* model_data_r = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TENMADO_BDL_MMADOR_e, SAFESTRING_VTBL);
    if (model_data_r == nullptr) /* JUT_ASSERT(94, model_data_r != NULL) */
        JUT_ASSERT_fail(STR(0x10030A88), 0x5E, STR(0x10030AB0));
    model = mDoExt_J3DModel__create(model_data_r, 0, 0x11020203);
    mModel2 = model;
    if (model == nullptr) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0239FC78, &Act_c::CreateHeap);

/* 0239FF5C */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0239FF5C, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel1)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -500.0f, -500.0f, 500.0f, 500.0f, 500.0f);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        dComIfGs_onSwitch(prm_get_swSave2(), home.roomNo); /* fopAcM_onSwitch */
        m2C8 = 200.0f;
        m2CC = 3;
    } else {
        m2C8 = 0.0f;
        m2CC = 0;
    }
    return TRUE;
}
VERIFY(0x0239FF5C, &Act_c::Create);

/* 0239FB5C */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0239FB5C, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_TENMADO_DZB_MMADO_e, 0, 0xF00);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT */
            JUT_ASSERT_fail(STR(0x10030A30), 0x91, STR(0x10030A44));
    }
    return phase_state;
}
VERIFY(0x0239FB5C, &Act_c::Mthd_Create);

/* 0239FC2C */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0239FC2C, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhase, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x0239FC2C, &Act_c::Mthd_Delete);

/* 0239FD7C */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0239FD7C, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
    mDoMtx_stack_c::transM(-m2C8, 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(mModel1, mDoMtx_stack_c::get());
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(m2C8, 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(mModel2, mDoMtx_stack_c::get());
}
VERIFY(0x0239FD7C, &Act_c::set_mtx);

/* 0239FF20 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0239FF20, void, this);
    J3DModel_setBaseScale(mModel1, &scale);
    J3DModel_setBaseScale(mModel2, &scale);
    set_mtx();
}
VERIFY(0x0239FF20, &Act_c::init_mtx);

/* 023A0060 */
BOOL Act_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x023A0060, BOOL, this, mtx);
    switch ((u32)(s32)m2CC) {
    case 0:
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            m2CC = 1;
        }
        break;
    case 1:
        mDoAud_seStart(JA_SE_OBJ_TENMADO_SHTR_OP, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this))); /* fopAcM_seStartCurrent */
        m2CC = 2;
        break;
    case 2: {
        f32 v = m2C8 + 1.0f;
        if (!(v < 100.0f)) {
            m2C8 = 100.0f;
            m2CC = 3;
        } else {
            m2C8 = v;
            if (!(v < 80.0f)) {
                dComIfGs_onSwitch(prm_get_swSave2(), home.roomNo); /* fopAcM_onSwitch */
            }
        }
        break;
    }
    default:
        break;
    }
    set_mtx();
    gabi::store<u32>(gabi::ea(mtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x023A0060, &Act_c::Execute);

/* 023A020C */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x023A020C, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel1, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel2, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel1);
    mDoExt_modelUpdateDL(mModel2);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x023A020C, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 023A02C0 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x023A02C0, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x023A02C0, Mthd_Create);
/* 023A02C4 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023A02C4, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x023A02C4, Mthd_Delete);
/* 023A02C8 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023A02C8, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x023A02C8, Mthd_Execute);
/* 023A02CC: MoveBGDraw (virtual Draw) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023A02CC, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x023A02CC, Mthd_Draw);
/* 023A02DC: MoveBGIsDelete (virtual IsDelete) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x023A02DC, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x023A02DC, Mthd_IsDelete);

/* 023A02EC */
static void __sinit_d_a_obj_tenmado_cpp() {
    WWHD_FUNC(0x023A02EC, void, (u32)0);
    sinit_header_statics(0x1046C454, 0x101CD1C8);
}
VERIFY(0x023A02EC, __sinit_d_a_obj_tenmado_cpp);

/* 023A0380: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023A0380, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023A0380, trivial_dt);


/* 023A0394: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE) */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    WWHD_FUNC(0x023A0394, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023A0394, MoveBgActor_IsDelete);

/* 023A039C: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_) */
static void SafeString_empty(void*) {
    WWHD_FUNC(0x023A039C, void, (u32)0);
}
VERIFY(0x023A039C, SafeString_empty);

/* 023A03A0 */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x023A03A0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023A03A0, Act_c_Delete);

/* 023A03A8: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x023A03A8, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x023A03A8, Act_c_dt);

