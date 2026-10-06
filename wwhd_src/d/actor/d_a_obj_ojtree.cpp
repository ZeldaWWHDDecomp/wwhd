/**
 * d_a_obj_ojtree.cpp (WWHD)
 * Object - Tall thin poles leading to the Deku Leaf (Forest Haven interior)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_ojtree.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002E088) /* "Ojtree" */
#define SAFESTRING_VTBL 0x1002DFD0
#define ACT_VTBL 0x1002E090 /* HD: daObjOjtree::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046BAF8)

enum {
    dRes_INDEX_OJTREE_BDL_OJTREE_e = 4,
    dRes_INDEX_OJTREE_DZB_OJTREE_e = 7,
};
enum { fpcNm_JBO_e = 0xD5, daJbo_Type_NORMAL_e = 0 };

namespace daObjOjtree {
struct Act_c : dBgS_MoveBgActor {
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<u8> mLockTimer;
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mLockTimer, 0x3EC);
}  // namespace daObjOjtree
using daObjOjtree::Act_c;

/* dBgW::SetLock (inline): flag byte +0x6C |= 0x80 (HD) */
static inline void dBgW_SetLock_l(dBgW* w) {
    u32 p = gabi::ea(w) + 0x6C;
    gabi::store<u8>(p, (u8)(gabi::load<u8>(p) | 0x80));
}

/* 0237FDF4 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0237FDF4, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_OJTREE_BDL_OJTREE_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(67, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x1002E050), 0x43, STR(0x1002E040));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}
VERIFY(0x0237FDF4, &Act_c::CreateHeap);

/* 0237FF84 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0237FF84, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -1.0f, -300.0f, 251.0f, 5001.0f, 251.0f);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y + 5000.0f;
    pos->z = current.pos.z;
    fopAcM_create(fpcNm_JBO_e, daJbo_Type_NORMAL_e, pos, home.roomNo, &shape_angle, nullptr, -1, 0);
    mLockTimer = 2;
    return TRUE;
}
VERIFY(0x0237FF84, &Act_c::Create);

/* 0237FCD8 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0237FCD8, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_OJTREE_DZB_OJTREE_e, 0, 0x26A0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(123, ...) */
            JUT_ASSERT_fail(STR(0x1002DFE8), 0x7A, STR(0x1002DFFC));
    }
    return phase_state;
}
VERIFY(0x0237FCD8, &Act_c::Mthd_Create);

/* 0237FDA8 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0237FDA8, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x0237FDA8, &Act_c::Mthd_Delete);

/* 0237FE90 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0237FE90, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x0237FE90, &Act_c::set_mtx);

/* 0237FF64 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0237FF64, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x0237FF64, &Act_c::init_mtx);

/* 02380054 */
BOOL Act_c::Execute(Mtx34** pMtx) {
    WWHD_FUNC(0x02380054, BOOL, this, pMtx);
    if (mLockTimer != 0) {
        u8 t = (u8)(mLockTimer - 1);
        mLockTimer = t;
        if (t == 0) {
            dBgW_SetLock_l(mpBgW);
        }
    }
    set_mtx();
    gabi::store<u32>(gabi::ea(pMtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x02380054, &Act_c::Execute);

/* 023800BC */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x023800BC, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x023800BC, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 02380154 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02380154, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x02380154, Mthd_Create);
/* 02380158 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x02380158, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x02380158, Mthd_Delete);
/* 0238015C */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0238015C, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0238015C, Mthd_Execute);
/* 02380160: MoveBGDraw (virtual Draw) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02380160, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x02380160, Mthd_Draw);
/* 02380170: MoveBGIsDelete (virtual IsDelete) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x02380170, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x02380170, Mthd_IsDelete);

/* 02380180 */
static void __sinit_d_a_obj_ojtree_cpp() {
    WWHD_FUNC(0x02380180, void, (u32)0);
    sinit_header_statics(0x1046BADC, 0x101CC0F0);
}
VERIFY(0x02380180, __sinit_d_a_obj_ojtree_cpp);

/* 02380214: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02380214, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02380214, trivial_dt);

/* 02380228: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE) */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    WWHD_FUNC(0x02380228, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02380228, MoveBgActor_IsDelete);

/* 02380230: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_) */
static void SafeString_empty(void*) {
    WWHD_FUNC(0x02380230, void, (u32)0);
}
VERIFY(0x02380230, SafeString_empty);

/* 02380234 */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x02380234, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02380234, Act_c_Delete);

/* 0238023C: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x0238023C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0238023C, Act_c_dt);
