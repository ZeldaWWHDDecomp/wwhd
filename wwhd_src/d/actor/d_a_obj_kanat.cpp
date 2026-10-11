/**
 * d_a_obj_kanat.cpp (WWHD)
 * Object - Forbidden Woods - Solid vine floor (blocks entry to B1)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_kanat.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_kanat.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002BB38,
    .headerStatics = {0x1046A368, 0x101CA618},
};
const char Act_c::M_arcname[] = "Kanat";
// HD: mapped matrix storage is a translation-unit object in the host verification build.
static Mtx34 M_tmp_mtx;

/* 02367420 */
// HD: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU)
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 02366E88 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_KANAT_BDL_KANAT_e);
    JUT_ASSERT(79, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr;
}

/* 02367018 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -100.0f, -500.0f, 500.0f, 200.0f, 500.0f);
    mIsBroken = false;
    mIsVisible = true;
    mSmokeCb.setRateOff(0);
    return TRUE;
}

/* 02366D00 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);

    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        return cPhs_STOP_e;
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_KANAT_DZB_KANAT_e, 0, 0x6440);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(126, ...) */
            JUTAssertion::showAssert("d_a_obj_kanat.cpp", 0x7D, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase_state;
}

/* 023672BC */
BOOL Act_c::Delete() {
    mSmokeCb.remove();
    return TRUE;
}

/* 02366E30 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    if (fpcM_CreateResult(this) != cPhs_STOP_e) {
        dComIfG_resDeleteDemo(&mPhs, M_arcname);
    }
    return result;
}

/* 02366F24 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 02366FF8 */
void Act_c::init_mtx() {
    // HD: base-scale copies the float words without changing NaN payloads.
    mpModel->mBaseScale.copy(scale);
    set_mtx();
}

/* 023670A0 */
BOOL Act_c::Execute(Mtx34** pMtx) {
    if (!mIsBroken) {
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            GXColor color;
            // HD: signed color components truncate to bytes; broken flag precedes alpha.
            color.r = (u8)tevStr.mColorC0.r;
            color.g = (u8)tevStr.mColorC0.g;
            color.b = (u8)tevStr.mColorC0.b;
            mIsBroken = true;
            color.a = (u8)tevStr.mColorC0.a;
            const GXColor* k0 = &tevStr.mColorK0;
            s8 room = current.roomNo;
            dPa_control_c* pa = dComIfGp_getParticle();
            pa->set(0, dPa_name::ID_AK_SN_KOKIRIHOUSEHAHEN00, &current.pos, &current.angle, nullptr, 0xFF,
                            nullptr, room, k0, &color, nullptr);
            /* dComIfGp_particle_setToon: HD group 2 */
            room = current.roomNo;
            pa = dComIfGp_getParticle();
            pa->set(2, dPa_name::ID_AK_ST_KOKIRIHOUSESMOKE00, &current.pos, &current.angle, nullptr, 0xB4,
                            (dPa_levelEcallBack*)(void*)&mSmokeCb, room, k0, &color, nullptr);
        }
    } else {
        mIsVisible = false;
        if (mSmokeCb.isEnd()) {
            fopAcM_delete(this);
        }
    }
    set_mtx();
    *reinterpret_cast<gptr<Mtx34>*>(pMtx) = &M_tmp_mtx; // HD: guest pointer representation.
    return TRUE;
}

/* 02367218 */
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    if (!mIsVisible) {
        return TRUE;
    }
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 023672EC */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 023672F0 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 023672F4 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 023672F8 */
// HD: MoveBGDraw inlined: virtual Draw
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 02367308 */
// HD: MoveBGIsDelete inlined: virtual IsDelete
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 02367318 */
static void __sinit_d_a_obj_kanat_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 023673AC */
// HD: sead::SafeString deleting destructor (this TU's vtable slot)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 023673C0 */
// HD: dBgS_MoveBgActor::IsDelete (this TU's copy, Act_c vtable +0x3C)
static BOOL Act_c_IsDelete(Act_c* i_this) {
    return TRUE;
}

/* 023673C8 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {
}

/* 023673CC */
// HD: Act_c deleting destructor (compiler-generated, vtable +0xC)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002BBE8
str 1002BBB8 in 02366E88
str 1002BBA8 "model_data != NULL" renamed
str 1002BB50 in 02366D00
str 1002BB64
data M_tmp_mtx 1046A384
*/
