/**
 * d_a_obj_smplbg.cpp (WWHD)
 * Object - Top/"head" of Tingle Tower (simple background actor).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_smplbg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_smplbg.h"
using daObjSmplbg::Act_c;
const gc_tu_data gc_tu = { .safeString=0x1002F5A8, .headerStatics={0x1046BEAC, 0x101CCC38, 0x1046BEB8} };
static const char M_arcname[] = "Qtkhd";
static Mtx34 M_tmp_mtx;
static f32 farCullFactor; // HD: global distance adjustment

// HD: parameter helper is emitted in the Tingle NPC translation unit.
s32 Act_c::prm_get_type() { return Smplbg_PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }

/* 02390168 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_QTKHD_BDL_QTKHD_e);
    JUT_ASSERT(0x6b, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}

/* 023902F8 */
// HD: attr() folded (M_attr[0]: flags 1|4|8, sphere cull 0/2250/0 r 750, eye +1687)
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    eyePos.y += 1687.0f;
    actor_status &= ~fopAcStts_NOCULLEXEC_e; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
    cullType = fopAc_CULLSPHERE_CUSTOM_e;        /* fopAcM_SetCullSize(this, fopAc_CULLSPHERE_CUSTOM_e) */
    fopAcM_setCullSizeSphere(this, 0.0f, 2250.0f, 0.0f, 750.0f);
    // HD: global far-cull adjustment uses a fused multiply-add.
    cullSizeFar = gabi::fmadds(farCullFactor, 10000.0f, 40000.0f);
    return TRUE;
}

/* 02390024 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    s32 type = prm_get_type();
    if (type >= 1)
        type = 0;
    mType = type;

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_QTKHD_DZB_QTKHD_e, dBgS_MoveBGProc_TypicalRotY, 0x15E0);
        JUT_ASSERT(0xbc, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 0239011C */
// HD: (not named by the matcher)
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return result;
}

/* 02390204 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 023902D8 */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 02390458 */
void Act_c::exec_qtkhd() {
    if (!mIsStop) {
        shape_angle.y = (s16)(shape_angle.y + 0x5b);
        fopAcM_seStart(this, JA_SE_OBJ_TC_TOWER_ROUND, 0);
    }
}

/* 023903AC */
// HD: the one-entry exec_proc table is folded into a direct call
BOOL Act_c::Execute(Mtx34** matrix) {
    exec_qtkhd();
    set_mtx();
    // HD: the output matrix is a guest pointer.
    *reinterpret_cast<gptr<Mtx34>*>(matrix) = &M_tmp_mtx;
    return TRUE;
}

/* 023903FC */
// HD: attr().mFlags & 1 folded: TEV_TYPE_BG0
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 02390598 */
static BOOL Act_c_Delete(Act_c* i_this) {
    return TRUE;
}

/* 0239058C */
// HD: dBgS_MoveBgActor::IsDelete (this TU's copy)
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    return TRUE;
}

/* 02390594 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* method table entries (HD: tail branches) */
/* 023904B8 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 023904BC */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 023904C0 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 023904C4 */
// HD: virtual Draw
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 023904D4 */
// HD: virtual IsDelete
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 023904E4 */
static void __sinit_d_a_obj_smplbg_cpp() {
    __sinit_header_statics();
}

/* 02390578 */
// HD: sead::SafeString deleting destructor (this TU's vtable slot, 0x1002F5B4)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023905A0 */
// HD: Act_c deleting destructor (vtable +0xC)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data M_tmp_mtx 1046BEC8
data farCullFactor 1047C6F0
str 1002F5A0 # "Qtkhd"
str 1002F628 in 02390168 # "d_a_obj_smplbg.cpp"
str 1002F5C0 in 02390024 # "d_a_obj_smplbg.cpp"
str 1002F618 "model_data != NULL" renamed # HD: null printed as 0
str 1002F5D4
ext 025D6768 void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32)
ext 022ED834 s32 Smplbg_PrmAbstract(fopAc_ac_c*, s32, s32)
*/
