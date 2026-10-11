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
#include "d/actor/d_a_obj_msdan_sub2.h"

const gc_tu_data gc_tu = {.safeString = 0x1002D988};
static const char* M_arcname = "Msdan";
static Mtx34 M_tmp_mtx;

enum {
    dRes_INDEX_MSDAN_BDL_MSDAN_e = 4,
    dRes_INDEX_MSDAN_DZB_MSDAN_e = 7,
};
enum { JA_SE_OBJ_MSDAN_SUB2 = 0x69BD };
// HD: the translation callback entry is passed directly to MoveBGCreate.
static const u32 dBgS_MoveBGProc_Trans = 0x024EE76C;

using daObjMsdanSub2::Act_c;

// HD: GHS stores this translation unit's actor vtable during placement construction.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x1002DA58;
}

/* 0237CE6C */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_objNo() { return PrmAbstract(this, PRM_OBJNO_W, PRM_OBJNO_S); }

/* 0237C64C */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MSDAN_BDL_MSDAN_e);
    JUT_ASSERT(0x57, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mModel = model;
    return model != nullptr;
}


/* 0237C82C */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mModel->getBaseTRMtx());
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
        mCurObjNo = OBJ_COUNT_e;
    } else {
        mCurObjNo = 0;
        mDist = 0.0f;
        mSpeed = 0.0f;
    }
    init_mtx();
    mpBgW->Move();
    return TRUE;
}

/* 0237C4D0 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MSDAN_DZB_MSDAN_e, dBgS_MoveBGProc_Trans, 0x9A0);
        JUT_ASSERT(0x90, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
        /* already switched on: no collision */
        if (fopAcM_isSwitch(this, prm_get_swSave()) && mpBgW != nullptr && mpBgW->ChkUsed()) {
            dBgS* bgs = dComIfG_Bgsp();
            bgs->Release(mpBgW);
        }
    }
    return phase_state;
}

/* 0237C600 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}

/* 0237C6E8 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0237C7BC */
void Act_c::init_mtx() {
    PSVECScale(&scale, &scale, 1.01f); /* scale *= 1.01f */
    mModel->setBaseScale(scale);
    MTXCopy(&M_tmp_mtx, &mBgMtx);
    set_mtx();
}

static inline void start_shock(s32 strength, s32 flags) {
    gabi::Local<cXyz> dir;
    dVibration_c* vib = dComIfGp_getVibration();
    dir->x = 0.0f;
    dir->y = 1.0f;
    dir->z = 0.0f;
    vib->StartShock(strength, flags, dir.get());
}

/* 0237C9C0 */
BOOL Act_c::Execute(gptr<Mtx34>* pMtx) {
    // HD: preserve GHS field-load ordering around the pure PrmAbstract calls.
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        f32 v = mSpeed;
        s32 cur = mCurObjNo;
        if (v < 0.0f) {
            v = 0.0f;
            mSpeed = v;
        }
        if (cur < OBJ_COUNT_e) {
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
    *pMtx = &M_tmp_mtx;
    return TRUE;
}

/* 0237CC94 */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    // HD: an extra unused play-object call remains after the draw lists.
    dComIfGp_get();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 0237CD30 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0237CD34 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0237CD38 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 0237CD3C */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 0237CD4C */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 0237CD5C */
static void __sinit_d_a_obj_msdan_sub2_cpp() {
    sinit_header_statics(0x1046B990, 0x101CBD6C);
}

/* 0237CDF0 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}


/* 0237CE04 */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    return TRUE;
}

/* 0237CE0C */
static void SafeString_empty(void*) {
}

/* 0237CE10 */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 0237CE18 */
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}


/* WWHD glue (tools/verify/autoglue.py)
str 1002DA50
str 1002DA0C
str 1002D9A0 in 0237C4D0
str 1002D9FC "model_data != NULL" renamed # HD: 0 rather than NULL.
str 1002D9B8 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046B9AC
*/
