/**
 * d_a_obj_nest.cpp (WWHD)
 * Object - Kargaroc nest (vibrates when something lands on it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_nest.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_nest.h"
using daObjNest::Act_c;

const gc_tu_data gc_tu = {
    .safeString = 0x1002DDF8,
    .headerStatics = {0x1046BA74, 0x101CC008, 0x1046BA80},
};

const char Act_c::M_arcname[] = "MtoriSU";
static Mtx34 M_tmp_mtx;
static csXyz csXyz_Zero;
static void rideCB(dBgW*, fopAc_ac_c*, fopAc_ac_c*);

// HD: GHS emits cLib_maxLimit with subtraction and fsel.
static inline f32 maxLimitF(f32 v, f32 max) { return (v - max) >= 0.0f ? max : v; }

/* 0237E9D0 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MTORISU_BDL_MTORISU_e);
    JUT_ASSERT(281, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}

/* 0237EC48 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeSphere(this, 0.0f, 20.0f, 0.0f, 210.0f);
    ((dBgW_nest_view*)mpBgW.get())->SetRideCallback(rideCB);
    mcXyz_0x2D4.x = cXyz::Zero.x;
    mcXyz_0x2D4.y = cXyz::Zero.y;
    mcXyz_0x2D4.z = cXyz::Zero.z;
    mcsXyz_0x2E0.x = csXyz_Zero.x;
    mcsXyz_0x2E0.y = csXyz_Zero.y;
    mcsXyz_0x2E0.z = csXyz_Zero.z;
    mcsXyz_0x2E6.x = csXyz_Zero.x;
    mcsXyz_0x2E6.y = csXyz_Zero.y;
    mcsXyz_0x2E6.z = csXyz_Zero.z;
    return TRUE;
}

/* 0237E8B4 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MTORISU_DZB_MTORISU_e, 0, 0xAA0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(329, ...) */
            JUTAssertion::showAssert("d_a_obj_nest.cpp", 0x148, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase_state;
}

/* 0237F020 */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 0237E984 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return result;
}

/* 0237EA6C */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
    mDoMtx_stack_c::ZXYrotM(mcsXyz_0x2E6);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0237EB54 */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

// HD: free callback preserves the three actor-callback arguments.
/* 0237EBD0 */
static void rideCB(dBgW* bgw, fopAc_ac_c* i_ac, fopAc_ac_c* i_pt) {
    Act_c* i_this = (Act_c*)i_ac;
    f32 actorDistXZ2 = fopAcM_searchActorDistanceXZ2(i_this, i_pt);
    if (actorDistXZ2 > 100.0f * 100.0f) { /* SQUARE(attr().field_0x04) */
        f32 fVar = i_pt->speedF * (1.0f / 30.0f);
        i_this->vib_set(maxLimitF(fVar, 1.0f));
    }
}

/* 0237EB74 */
void Act_c::vib_set(f32 param) {
    f32 d = param * 400.0f; /* attr().field_0x08/0C/10 (all 400) */
    f32 y = mcXyz_0x2D4.y + d;
    f32 x = mcXyz_0x2D4.x + d;
    f32 z = mcXyz_0x2D4.z + d;
    mcXyz_0x2D4.x = maxLimitF(x, 50.0f);
    mcXyz_0x2D4.y = maxLimitF(y, 100.0f);
    mcXyz_0x2D4.z = maxLimitF(z, 50.0f);
}

/* 0237ED14 */
void Act_c::vib_proc() {
    mcsXyz_0x2E0.x = (s16)(mcsXyz_0x2E0.x + 0x4650);
    mcsXyz_0x2E0.y = (s16)(mcsXyz_0x2E0.y + 0x4650);
    mcsXyz_0x2E0.z = (s16)(mcsXyz_0x2E0.z + 0x4650);
    f32 mag = std_sqrtf(PSVECSquareMag(&mcXyz_0x2D4)); /* cXyz::abs */
    if (mag < 100.0f) {
        mcsXyz_0x2E6.x = csXyz_Zero.x;
        mcsXyz_0x2E6.y = csXyz_Zero.y;
        mcsXyz_0x2E6.z = csXyz_Zero.z;
    } else {
        mcsXyz_0x2E6.x = (s16)ftoi(cM_scos(mcsXyz_0x2E0.x) * mcXyz_0x2D4.x);
        mcsXyz_0x2E6.y = (s16)ftoi(cM_scos(mcsXyz_0x2E0.y) * mcXyz_0x2D4.y);
        mcsXyz_0x2E6.z = (s16)ftoi(cM_scos(mcsXyz_0x2E0.z) * mcXyz_0x2D4.z);
    }
    PSVECScale(&mcXyz_0x2D4, &mcXyz_0x2D4, 0.92f);
}

/* 0237EE58 */
BOOL Act_c::Execute(Mtx34** pMtx) {
    vib_proc();
    set_mtx();
    *reinterpret_cast<gptr<Mtx34>*>(pMtx) = &M_tmp_mtx; // HD: guest pointer representation.
    return TRUE;
}

/* 0237EEA8 */
// HD: no dComIfG_Bgsp() call left
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 0237EF40 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0237EF44 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0237EF48 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 0237EF4C */
// HD: virtual Draw
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 0237EF5C */
// HD: virtual IsDelete
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 0237EF6C */
static void __sinit_d_a_obj_nest_cpp() {
    __sinit_header_statics();
}

/* 0237F000 */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0237F014 */
// HD: dBgS_MoveBgActor::IsDelete (per-TU copy)
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    return TRUE;
}

/* 0237F028 */
// HD: daObjNest::Act_c deleting destructor
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* ---- leftover functions of the translation unit ---- */

/* 0237F01C */
// HD: this translation unit's empty SafeString virtual.
static void nest_SafeString_assureTermination(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and existing local binding
str 1002DEB8
str 1002DE78 in 0237E9D0
str 1002DE68 "model_data != NULL" renamed
str 1002DE10 in 0237E8B4
str 1002DE24
data M_tmp_mtx 1046BA90
data csXyz_Zero 101FFB14
ext 025D6768 void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32)
*/
