/**
 * d_a_obj_msdan_sub2.cpp (WWHD)
 * Object - Msdan sub-part 2 (a block that slides out horizontally when a switch is set)
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_msdan_sub2.cpp) has only "Nonmatching" stubs for this TU:
 * every function here is written from the WWHD code, with the names and structure of the sibling
 * d_a_obj_msdan_sub. Verified against cking.rpx.
 *
 * GHS knows the per-TU daObj::PrmAbstract copy is pure and keeps fields in registers across it;
 * the candidate mirrors where the original loads each field relative to those calls.
 */
#include "bindings.h"

#define M_arcname STR(0x1002DA50) /* "Msdan" */
#define SAFESTRING_VTBL 0x1002D988
#define ACT_VTBL 0x1002DA58 /* HD: daObjMsdanSub2::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046B9AC)

enum {
    dRes_INDEX_MSDAN_BDL_MSDAN_e = 4,
    dRes_INDEX_MSDAN_DZB_MSDAN_e = 7,
};
enum { JA_SE_OBJ_MSDAN_SUB2 = 0x69BD };
#define dBgS_MoveBGProc_Trans 0x024EE76C

namespace daObjMsdanSub2 {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e {
        PRM_SWSAVE_W = 0x8,
        PRM_SWSAVE_S = 0x0,
        PRM_OBJNO_W = 0x8,
        PRM_OBJNO_S = 0x8,
    };
    u32 prm_get_swSave();
    u32 prm_get_objNo();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mModel;
    /* 0x3EC */ be<s32> mCurObjNo; /* 0 (off) .. 16 */
    /* 0x3F0 */ be<f32> mDist;     /* distance moved out of home */
    /* 0x3F4 */ be<f32> mSpeed;
};
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, mSpeed, 0x3F4);
}  // namespace daObjMsdanSub2
using daObjMsdanSub2::Act_c;

/* 0237CE6C: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0237CE6C, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0237CE6C, PrmAbstract);
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_objNo() { return PrmAbstract(this, PRM_OBJNO_W, PRM_OBJNO_S); }

/* 0237C64C */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0237C64C, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MSDAN_BDL_MSDAN_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x57, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x1002DA0C), 0x57, STR(0x1002D9FC));
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mModel = model;
    return model != nullptr;
}
VERIFY(0x0237C64C, &Act_c::CreateHeap);


/* 0237C82C */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0237C82C, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -1500.0f, -1000.0f, -1500.0f, 1500.0f, 1000.0f, 1500.0f);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        u32 obj = prm_get_objNo();
        if ((obj & 1) == 0) {
            current.pos.x = gabi::fmadds(600.0f, cM_scos(current.angle.y), home.pos.x);
            current.pos.z = gabi::fmadds(600.0f, cM_ssin(current.angle.y), home.pos.z);
        } else {
            current.pos.x = gabi::fnmsubs(600.0f, cM_scos(current.angle.y), home.pos.x);
            current.pos.z = gabi::fnmsubs(600.0f, cM_ssin(current.angle.y), home.pos.z);
        }
        mCurObjNo = 16;
    } else {
        mCurObjNo = 0;
        mDist = 0.0f;
        mSpeed = 0.0f;
    }
    init_mtx();
    dBgW_Move(mpBgW);
    return TRUE;
}
VERIFY(0x0237C82C, &Act_c::Create);

/* 0237C4D0 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0237C4D0, cPhs_State, this);
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
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MSDAN_DZB_MSDAN_e, dBgS_MoveBGProc_Trans, 0x9A0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x90, ...) */
            JUT_ASSERT_fail(STR(0x1002D9A0), 0x90, STR(0x1002D9B8));
        /* already switched on: no collision */
        if (fopAcM_isSwitch(this, prm_get_swSave()) && mpBgW != nullptr && dBgW_ChkUsed(mpBgW)) {
            dBgS* bgs = dComIfG_Bgsp();
            cBgS_Release(bgs, mpBgW);
        }
    }
    return phase_state;
}
VERIFY(0x0237C4D0, &Act_c::Mthd_Create);

/* 0237C600 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0237C600, BOOL, this);
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x0237C600, &Act_c::Mthd_Delete);

/* 0237C6E8 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0237C6E8, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x0237C6E8, &Act_c::set_mtx);

/* 0237C7BC */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0237C7BC, void, this);
    PSVECScale(&scale, &scale, 1.01f); /* scale *= 1.01f */
    J3DModel_setBaseScale(mModel, &scale);
    PSMTXCopy(M_tmp_mtx, &mBgMtx);
    set_mtx();
}
VERIFY(0x0237C7BC, &Act_c::init_mtx);

static inline void start_shock(s32 strength, s32 flags) {
    gabi::Local<cXyz> dir;
    u32 vib = gabi::ea(dComIfGp_getVibration());
    dir->x = 0.0f;
    dir->y = 1.0f;
    dir->z = 0.0f;
    gabi::call(0x025CB374, vib, strength, flags, dir.get()); /* dVibration_c::StartShock */
}

/* 0237C9C0 */
BOOL Act_c::Execute(Mtx34** pMtx) {
    WWHD_FUNC(0x0237C9C0, BOOL, this, pMtx);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        f32 v = mSpeed;
        s32 cur = mCurObjNo;
        if (v < 0.0f) {
            v = 0.0f;
            mSpeed = v;
        }
        if (cur < 16) {
            if (v == 0.0f) {
                mDoAud_seStart(JA_SE_OBJ_MSDAN_SUB2, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
                v = mSpeed;
            }
            v = v + 10.0f;
            f32 dist = mDist + v;
            mSpeed = v;
            mDist = dist;
            u32 obj = prm_get_objNo();
            cur = mCurObjNo;
            if ((u32)cur == obj) {
                f32 hx = home.pos.x;
                f32 c = cM_scos(current.angle.y);
                if ((cur & 1) == 0) {
                    current.pos.x = gabi::fmadds(dist, c, hx);
                    f32 hz = home.pos.z;
                    dist = mDist;
                    current.pos.z = gabi::fmadds(dist, cM_ssin(current.angle.y), hz);
                } else {
                    current.pos.x = gabi::fnmsubs(dist, c, hx);
                    f32 hz = home.pos.z;
                    dist = mDist;
                    current.pos.z = gabi::fnmsubs(dist, cM_ssin(current.angle.y), hz);
                }
            }
            if (!(dist < 600.0f)) {
                obj = prm_get_objNo();
                cur = mCurObjNo;
                if ((u32)cur == obj) {
                    f32 hx = home.pos.x;
                    f32 c = cM_scos(current.angle.y);
                    if ((cur & 1) == 0) {
                        current.pos.x = gabi::fmadds(600.0f, c, hx);
                        current.pos.z = gabi::fmadds(600.0f, cM_ssin(current.angle.y), home.pos.z);
                    } else {
                        current.pos.x = gabi::fnmsubs(600.0f, c, hx);
                        current.pos.z = gabi::fnmsubs(600.0f, cM_ssin(current.angle.y), home.pos.z);
                    }
                    start_shock(1, 1);
                    cur = mCurObjNo;
                }
                mDist = 0.0f;
                mSpeed = 0.0f;
                mCurObjNo = cur + 1;
            }
        }
    }
    set_mtx();
    gabi::store<u32>(gabi::ea(pMtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x0237C9C0, &Act_c::Execute);

/* 0237CC94: HD: one more (unused) dComIfGp_get() call after the draw lists */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0237CC94, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    dComIfGp_get();
    return TRUE;
}
VERIFY(0x0237CC94, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 0237CD30 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0237CD30, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0237CD30, Mthd_Create);
/* 0237CD34 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0237CD34, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0237CD34, Mthd_Delete);
/* 0237CD38 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0237CD38, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0237CD38, Mthd_Execute);
/* 0237CD3C: MoveBGDraw (virtual Draw) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0237CD3C, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x0237CD3C, Mthd_Draw);
/* 0237CD4C: MoveBGIsDelete (virtual IsDelete) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0237CD4C, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x0237CD4C, Mthd_IsDelete);

/* 0237CD5C */
static void __sinit_d_a_obj_msdan_sub2_cpp() {
    WWHD_FUNC(0x0237CD5C, void, (u32)0);
    sinit_header_statics(0x1046B990, 0x101CBD6C);
}
VERIFY(0x0237CD5C, __sinit_d_a_obj_msdan_sub2_cpp);

/* 0237CDF0: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0237CDF0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0237CDF0, trivial_dt);


/* 0237CE04: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE) */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    WWHD_FUNC(0x0237CE04, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0237CE04, MoveBgActor_IsDelete);

/* 0237CE0C: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_) */
static void SafeString_empty(void*) {
    WWHD_FUNC(0x0237CE0C, void, (u32)0);
}
VERIFY(0x0237CE0C, SafeString_empty);

/* 0237CE10 */
static BOOL Act_c_Delete(Act_c*) {
    WWHD_FUNC(0x0237CE10, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0237CE10, Act_c_Delete);

/* 0237CE18: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x0237CE18, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0237CE18, Act_c_dt);

