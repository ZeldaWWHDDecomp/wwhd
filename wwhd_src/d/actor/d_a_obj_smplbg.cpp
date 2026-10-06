/**
 * d_a_obj_smplbg.cpp (WWHD)
 * Object - Top/"head" of Tingle Tower (simple background actor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_smplbg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002F5A0) /* "Qtkhd" (M_attr[0].mResName) */
#define SAFESTRING_VTBL 0x1002F5A8
#define ACT_VTBL 0x1002F664 /* HD: daObjSmplbg::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046BEC8)

enum {
    dRes_INDEX_QTKHD_BDL_QTKHD_e = 4,
    dRes_INDEX_QTKHD_DZB_QTKHD_e = 7,
};
enum { JA_SE_OBJ_TC_TOWER_ROUND = 0x619A };

namespace daObjSmplbg {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_TYPE_W = 8, PRM_TYPE_S = 0 };
    s32 prm_get_type();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void exec_qtkhd();
    BOOL Execute(Mtx34** matrix);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<s32> mType;
    /* 0x3F0 */ be<u8> mIsStop;
    /* 0x3F1 */ u8 _3F1[3];
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mIsStop, 0x3F0);
}  // namespace daObjSmplbg
using daObjSmplbg::Act_c;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D6768 fopAcM_setCullSizeSphere(actor, x, y, z, r) (also local in tsubo/buoyflag) */
static inline void fopAcM_setCullSizeSphere(fopAc_ac_c* a, f32 x, f32 y, f32 z, f32 r) { gabi::call(0x025D6768, a, x, y, z, r); }

/* 022ED834: daObj::PrmAbstract<daObjSmplbg::Act_c::Prm_e> (the copy lives in d_a_npc_tc's TU) */
s32 Act_c::prm_get_type() { return gabi::call<s32>(0x022ED834, this, PRM_TYPE_W, PRM_TYPE_S); }

/* 02390168 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x02390168, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_QTKHD_BDL_QTKHD_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x6b, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x1002F628), 0x6b, STR(0x1002F618));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}
VERIFY(0x02390168, &Act_c::CreateHeap);

/* 023902F8: attr() folded (M_attr[0]: flags 1|4|8, sphere cull 0/2250/0 r 750, eye +1687) */
BOOL Act_c::Create() {
    WWHD_FUNC(0x023902F8, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    eyePos.y += 1687.0f;
    actor_status &= ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    cullType = 0x17;        /* fopAcM_SetCullSize(this, fopAc_CULLSPHERE_CUSTOM_e) */
    fopAcM_setCullSizeSphere(this, 0.0f, 2250.0f, 0.0f, 750.0f);
    /* HD: far cull distance from a global (debug?) float at 0x1047C6F0 */
    cullSizeFar = gabi::fmadds(gabi::load<f32>(0x1047C6F0), 10000.0f, 40000.0f);
    return TRUE;
}
VERIFY(0x023902F8, &Act_c::Create);

/* 02390024 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x02390024, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    s32 type = prm_get_type();
    if (type >= 1)
        type = 0;
    mType = type;

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_QTKHD_DZB_QTKHD_e, 0x024EE708 /* dBgS_MoveBGProc_TypicalRotY */, 0x15E0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0xbc, ...) */
            JUT_ASSERT_fail(STR(0x1002F5C0), 0xbc, STR(0x1002F5D4));
    }
    return phase_state;
}
VERIFY(0x02390024, &Act_c::Mthd_Create);

/* 0239011C (not named by the matcher) */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0239011C, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x0239011C, &Act_c::Mthd_Delete);

/* 02390204 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02390204, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x02390204, &Act_c::set_mtx);

/* 023902D8 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x023902D8, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x023902D8, &Act_c::init_mtx);

/* 02390458 */
void Act_c::exec_qtkhd() {
    WWHD_FUNC(0x02390458, void, this);
    if (!mIsStop) {
        shape_angle.y = (s16)(shape_angle.y + 0x5b);
        fopAcM_seStart(this, JA_SE_OBJ_TC_TOWER_ROUND, 0);
    }
}
VERIFY(0x02390458, &Act_c::exec_qtkhd);

/* 023903AC: HD: the one-entry exec_proc table is folded into a direct call */
BOOL Act_c::Execute(Mtx34** matrix) {
    WWHD_FUNC(0x023903AC, BOOL, this, matrix);
    exec_qtkhd();
    set_mtx();
    gabi::store<u32>(gabi::ea(matrix), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x023903AC, &Act_c::Execute);

/* 023903FC: attr().mFlags & 1 folded: TEV_TYPE_BG0 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x023903FC, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}
VERIFY(0x023903FC, &Act_c::Draw);

/* 02390598 */
static BOOL Act_c_Delete(Act_c* i_this) {
    WWHD_FUNC(0x02390598, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02390598, Act_c_Delete);

/* 0239058C: dBgS_MoveBgActor::IsDelete (this TU's copy) */
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    WWHD_FUNC(0x0239058C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0239058C, MoveBgActor_IsDelete);

/* 02390594: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02390594, void, (u32)0);
}
VERIFY(0x02390594, SafeString_assureTerminationImpl);

/* method table entries (HD: tail branches) */
/* 023904B8 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x023904B8, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x023904B8, Mthd_Create);
/* 023904BC */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x023904BC, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x023904BC, Mthd_Delete);
/* 023904C0 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x023904C0, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x023904C0, Mthd_Execute);
/* 023904C4: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x023904C4, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x023904C4, Mthd_Draw);
/* 023904D4: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x023904D4, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x023904D4, Mthd_IsDelete);

/* 023904E4 */
static void __sinit_d_a_obj_smplbg_cpp() {
    WWHD_FUNC(0x023904E4, void, (u32)0);
    sinit_header_statics(0x1046BEAC, 0x101CCC38);
}
VERIFY(0x023904E4, __sinit_d_a_obj_smplbg_cpp);

/* 02390578: sead::SafeString deleting destructor (this TU's vtable slot, 0x1002F5B4) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02390578, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02390578, SafeString_dt);

/* 023905A0: Act_c deleting destructor (vtable +0xC) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x023905A0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023905A0, Act_c_dt);
