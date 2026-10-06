/**
 * d_a_obj_aygr.cpp (WWHD)
 * Object - Aygr (lookout tower "yagura", optionally with a ladder "hashigo" that has its own
 * background collision)
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_aygr.cpp) has only "Nonmatching" stubs for this TU: every
 * function here is written from the WWHD code, with the GameCube names. Verified against
 * cking.rpx.
 */
#include "bindings.h"

#define M_arcname STR(0x100253D0) /* "Aygr" */
#define SAFESTRING_VTBL 0x10025300
#define ACT_VTBL 0x100253D8 /* HD: daObjAygr::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046915C)

enum {
    dRes_INDEX_AYGR_BDL_YAGURA_e = 4,
    dRes_INDEX_AYGR_BDL_HASHIGO_e = 5,
    dRes_INDEX_AYGR_DZB_YAGURA_e = 8,
    dRes_INDEX_AYGR_DZB_HASHIGO_e = 9,
};

namespace daObjAygr {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_MDL_W = 1, PRM_MDL_S = 0 };
    u8 prm_get_mdl();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;      /* tower */
    /* 0x3EC */ gptr<J3DModel> mpModel2;     /* ladder (prm_get_mdl() != 0) */
    /* 0x3F0 */ gptr<dBgW> mpBgW2;           /* ladder collision */
    /* 0x3F4 */ Mtx34 mBgMtx2;
    /* 0x424 */ be<u8> mBgW2Regist;          /* mpBgW2 is set up (released in Mthd_Delete) */
};
WWHD_OFFSET(Act_c, mpBgW2, 0x3F0);
WWHD_OFFSET(Act_c, mBgW2Regist, 0x424);
}  // namespace daObjAygr
using daObjAygr::Act_c;

/* dBgW::SetLock (inline): flag byte +0x6C |= 0x80 (HD) */
static inline void dBgW_SetLock_l(dBgW* w) {
    u32 p = gabi::ea(w) + 0x6C;
    gabi::store<u8>(p, (u8)(gabi::load<u8>(p) | 0x80));
}

/* 0231AE78: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0231AE78, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0231AE78, PrmAbstract);
u8 Act_c::prm_get_mdl() { return (u8)PrmAbstract(this, PRM_MDL_W, PRM_MDL_S); }

/* 0231A818 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0231A818, BOOL, this);
    J3DModelData* model_data_yagura = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_AYGR_BDL_YAGURA_e, SAFESTRING_VTBL);
    if (model_data_yagura == nullptr) /* JUT_ASSERT(0x50, model_data_yagura != NULL) */
        JUT_ASSERT_fail(STR(0x10025388), 0x50, STR(0x1002539C));
    J3DModel* model = mDoExt_J3DModel__create(model_data_yagura, 0, 0x11020203);
    mpModel = model;
    if (model == nullptr)
        return FALSE;
    if (prm_get_mdl()) {
        J3DModelData* model_data_hashigo = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_AYGR_BDL_HASHIGO_e, SAFESTRING_VTBL);
        if (model_data_hashigo == nullptr) /* JUT_ASSERT(0x59, model_data_hashigo != NULL) */
            JUT_ASSERT_fail(STR(0x10025388), 0x59, STR(0x10025370));
        model = mDoExt_J3DModel__create(model_data_hashigo, 0, 0x11020203);
        mpModel2 = model;
        if (model == nullptr)
            return FALSE;
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
        PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx2);
        dBgW* bgw = new_dBgW();
        mpBgW2 = bgw;
        if (bgw == nullptr)
            return FALSE;
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_AYGR_DZB_HASHIGO_e, SAFESTRING_VTBL);
        if (cBgW_Set(mpBgW2, dzb, cBgW_MOVE_BG_e, &mBgMtx2))
            return FALSE;
        mBgW2Regist = TRUE;
    } else {
        mpModel2 = nullptr;
        mBgW2Regist = FALSE;
        mpBgW2 = nullptr;
    }
    return TRUE;
}
VERIFY(0x0231A818, &Act_c::CreateHeap);

/* 0231ABA4 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0231ABA4, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -100.0f, -500.0f, 500.0f, 4000.0f, 500.0f);
    return TRUE;
}
VERIFY(0x0231ABA4, &Act_c::Create);

/* 0231A648 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0231A648, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mBgW2Regist = FALSE;
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_AYGR_DZB_YAGURA_e, 0, 0x85F0);
        if (mpBgW != nullptr && dBgW_ChkUsed(mpBgW)) {
            dBgW_Move(mpBgW);
            dBgW_SetLock_l(mpBgW);
        }
        if (prm_get_mdl()) {
            dBgS* bgs = dComIfG_Bgsp();
            dBgS_Regist(bgs, mpBgW2, this);
            if (mpBgW2 != nullptr && dBgW_ChkUsed(mpBgW2)) {
                dBgW_Move(mpBgW2);
                dBgW_SetLock_l(mpBgW2);
            }
        }
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0xB6, ...) */
            JUT_ASSERT_fail(STR(0x10025318), 0xB6, STR(0x1002532C));
    }
    return phase_state;
}
VERIFY(0x0231A648, &Act_c::Mthd_Create);

/* 0231A7AC */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0231A7AC, BOOL, this);
    if (mBgW2Regist) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, mpBgW2);
    }
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x0231A7AC, &Act_c::Mthd_Delete);

/* 0231A9D8 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0231A9D8, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    if (prm_get_mdl()) {
        J3DModel_setBaseTRMtx(mpModel2, mDoMtx_stack_c::get());
    }
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x0231A9D8, &Act_c::set_mtx);

/* 0231AB28 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0231AB28, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    if (prm_get_mdl()) {
        J3DModel_setBaseScale(mpModel2, &scale);
    }
    set_mtx();
}
VERIFY(0x0231AB28, &Act_c::init_mtx);

/* 0231AC1C */
BOOL Act_c::Execute(Mtx34** pMtx) {
    WWHD_FUNC(0x0231AC1C, BOOL, this, pMtx);
    set_mtx();
    gabi::store<u32>(gabi::ea(pMtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x0231AC1C, &Act_c::Execute);

/* 0231AC58 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0231AC58, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    if (prm_get_mdl()) {
        setLightTevColorType(dKy_getEnvlight(), mpModel2, &tevStr);
    }
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    if (prm_get_mdl()) {
        mDoExt_modelUpdateDL(mpModel2);
    }
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0231AC58, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 0231AD3C */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0231AD3C, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0231AD3C, Mthd_Create);
/* 0231AD40 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0231AD40, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0231AD40, Mthd_Delete);
/* 0231AD44 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0231AD44, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0231AD44, Mthd_Execute);
/* 0231AD48: MoveBGDraw (virtual Draw) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0231AD48, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x0231AD48, Mthd_Draw);
/* 0231AD58: MoveBGIsDelete (virtual IsDelete) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0231AD58, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x0231AD58, Mthd_IsDelete);

/* 0231AD68 */
static void __sinit_d_a_obj_aygr_cpp() {
    WWHD_FUNC(0x0231AD68, void, (u32)0);
    sinit_header_statics(0x10469140, 0x101C7D60);
}
VERIFY(0x0231AD68, __sinit_d_a_obj_aygr_cpp);

/* 0231ADFC: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0231ADFC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0231ADFC, trivial_dt);


/* 0231AE10: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE) */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    WWHD_FUNC(0x0231AE10, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0231AE10, MoveBgActor_IsDelete);

/* 0231AE18: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_) */
static void SafeString_empty(void*) {
    WWHD_FUNC(0x0231AE18, void, (u32)0);
}
VERIFY(0x0231AE18, SafeString_empty);

/* 0231AE1C */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x0231AE1C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0231AE1C, Act_c_Delete);

/* 0231AE24: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x0231AE24, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0231AE24, Act_c_dt);

