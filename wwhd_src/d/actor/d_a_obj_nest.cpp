/**
 * d_a_obj_nest.cpp (WWHD)
 * Object - Kargaroc nest (vibrates when something lands on it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_nest.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002DEB8) /* "MtoriSU" */
#define SAFESTRING_VTBL 0x1002DDF8
#define ACT_VTBL 0x1002DEC0 /* HD: daObjNest::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046BA90)
#define csXyz_Zero gabi::at<csXyz>(0x101FFB14)

enum {
    dRes_INDEX_MTORISU_BDL_MTORISU_e = 4,
    dRes_INDEX_MTORISU_DZB_MTORISU_e = 7,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void fopAcM_setCullSizeSphere(fopAc_ac_c* a, f32 x, f32 y, f32 z, f32 r) { gabi::call(0x025D6768, a, x, y, z, r); }

/* cLib_maxLimit(f32): GHS emits fsel(v - max, max, v) */
static inline f32 maxLimitF(f32 v, f32 max) { return (v - max) >= 0.0f ? max : v; }

namespace daObjNest {
struct Act_c : dBgS_MoveBgActor {
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    static void rideCB(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt);
    void vib_set(f32 param);
    void vib_proc();
    BOOL Execute(Mtx34** matrix);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ cXyz mVib;       /* GameCube mcXyz_0x2D4 */
    /* 0x3F8 */ csXyz mVibPhase;  /* GameCube mcsXyz_0x2E0 */
    /* 0x3FE */ csXyz mVibAngle;  /* GameCube mcsXyz_0x2E6 */
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mVibAngle, 0x3FE);
}  // namespace daObjNest
using daObjNest::Act_c;

/* 0237E9D0 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0237E9D0, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MTORISU_BDL_MTORISU_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(281, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x1002DE78), 0x119, STR(0x1002DE68));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}
VERIFY(0x0237E9D0, &Act_c::CreateHeap);

/* 0237EC48 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0237EC48, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeSphere(this, 0.0f, 20.0f, 0.0f, 210.0f);
    gabi::store<u32>(gabi::ea(mpBgW) + 0xB0, 0x0237EBD0); /* mpBgW->SetRideCallback(rideCB) */
    mVib.x = cXyz_Zero->x;
    mVib.y = cXyz_Zero->y;
    mVib.z = cXyz_Zero->z;
    mVibPhase.x = csXyz_Zero->x;
    mVibPhase.y = csXyz_Zero->y;
    mVibPhase.z = csXyz_Zero->z;
    mVibAngle.x = csXyz_Zero->x;
    mVibAngle.y = csXyz_Zero->y;
    mVibAngle.z = csXyz_Zero->z;
    return TRUE;
}
VERIFY(0x0237EC48, &Act_c::Create);

/* 0237E8B4 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0237E8B4, cPhs_State, this);
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
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MTORISU_DZB_MTORISU_e, 0, 0xAA0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(329, ...) */
            JUT_ASSERT_fail(STR(0x1002DE10), 0x148, STR(0x1002DE24));
    }
    return phase_state;
}
VERIFY(0x0237E8B4, &Act_c::Mthd_Create);

/* 0237E984 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0237E984, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x0237E984, &Act_c::Mthd_Delete);

/* 0237EA6C */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0237EA6C, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), mVibAngle.x, mVibAngle.y, mVibAngle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0237EA6C, &Act_c::set_mtx);

/* 0237EB54 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0237EB54, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x0237EB54, &Act_c::init_mtx);

/* 0237EBD0 */
void Act_c::rideCB(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt) {
    WWHD_FUNC(0x0237EBD0, void, bgw, i_ac, i_pt);
    Act_c* i_this = (Act_c*)i_ac;
    f32 actorDistXZ2 = fopAcM_searchActorDistanceXZ2(i_this, i_pt);
    if (actorDistXZ2 > 100.0f * 100.0f) { /* SQUARE(attr().field_0x04) */
        f32 fVar = i_pt->speedF * (1.0f / 30.0f);
        i_this->vib_set(maxLimitF(fVar, 1.0f));
    }
}
VERIFY(0x0237EBD0, &Act_c::rideCB);

/* 0237EB74 */
void Act_c::vib_set(f32 param) {
    WWHD_FUNC(0x0237EB74, void, this, param);
    f32 d = param * 400.0f; /* attr().field_0x08/0C/10 (all 400) */
    f32 y = mVib.y + d;
    f32 x = mVib.x + d;
    f32 z = mVib.z + d;
    mVib.x = maxLimitF(x, 50.0f);
    mVib.y = maxLimitF(y, 100.0f);
    mVib.z = maxLimitF(z, 50.0f);
}
VERIFY(0x0237EB74, &Act_c::vib_set);

/* 0237ED14 */
void Act_c::vib_proc() {
    WWHD_FUNC(0x0237ED14, void, this);
    mVibPhase.x = (s16)(mVibPhase.x + 0x4650);
    mVibPhase.y = (s16)(mVibPhase.y + 0x4650);
    mVibPhase.z = (s16)(mVibPhase.z + 0x4650);
    f32 mag = std_sqrtf(PSVECSquareMag(&mVib)); /* cXyz::abs */
    if (mag < 100.0f) {
        mVibAngle.x = csXyz_Zero->x;
        mVibAngle.y = csXyz_Zero->y;
        mVibAngle.z = csXyz_Zero->z;
    } else {
        mVibAngle.x = (s16)gabi::ftoi(cM_scos(mVibPhase.x) * mVib.x);
        mVibAngle.y = (s16)gabi::ftoi(cM_scos(mVibPhase.y) * mVib.y);
        mVibAngle.z = (s16)gabi::ftoi(cM_scos(mVibPhase.z) * mVib.z);
    }
    PSVECScale(&mVib, &mVib, 0.92f);
}
VERIFY(0x0237ED14, &Act_c::vib_proc);

/* 0237EE58 */
BOOL Act_c::Execute(Mtx34** matrix) {
    WWHD_FUNC(0x0237EE58, BOOL, this, matrix);
    vib_proc();
    set_mtx();
    gabi::store<u32>(gabi::ea(matrix), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x0237EE58, &Act_c::Execute);

/* 0237EEA8: HD: no dComIfG_Bgsp() call left */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0237EEA8, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0237EEA8, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 0237EF40 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0237EF40, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0237EF40, Mthd_Create);
/* 0237EF44 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0237EF44, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0237EF44, Mthd_Delete);
/* 0237EF48 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0237EF48, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0237EF48, Mthd_Execute);
/* 0237EF4C: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0237EF4C, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x0237EF4C, Mthd_Draw);
/* 0237EF5C: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0237EF5C, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x0237EF5C, Mthd_IsDelete);

/* 0237EF6C */
static void __sinit_d_a_obj_nest_cpp() {
    WWHD_FUNC(0x0237EF6C, void, (u32)0);
    sinit_header_statics(0x1046BA74, 0x101CC008);
}
VERIFY(0x0237EF6C, __sinit_d_a_obj_nest_cpp);

/* 0237F000: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0237F000, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0237F000, trivial_dt);

/* 0237F014: dBgS_MoveBgActor::IsDelete (per-TU copy) */
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    WWHD_FUNC(0x0237F014, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0237F014, MoveBgActor_IsDelete);

/* 0237F020 */
static BOOL Act_c_Delete(Act_c* i_this) {
    WWHD_FUNC(0x0237F020, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0237F020, Act_c_Delete);

/* 0237F028: daObjNest::Act_c deleting destructor */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0237F028, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0237F028, Act_c_dt);

/* ---- leftover functions of the translation unit ---- */

/* 0237F01C sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002DE0C, after the destructor 0237F000 */
static void nest_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x0237F01C, void, p);
}
VERIFY(0x0237F01C, nest_SafeString_assureTermination);
