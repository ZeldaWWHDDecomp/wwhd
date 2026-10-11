/**
 * d_a_obj_msdan_sub.cpp (WWHD)
 * Object - Msdan sub-step (one step of a staircase that sinks/rises with a switch)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_msdan_sub.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * GHS knows the per-TU daObj::PrmAbstract copy is pure: in Execute it keeps field values in
 * registers across those calls (and evaluates a second prm_get_objNo() only on one path). The
 * candidate mirrors where the original loads each field relative to those calls.
 */
#include "d/actor/d_a_obj_msdan_sub.h"

const gc_tu_data gc_tu = {.safeString = 0x1002D858};
static const char* M_arcname = "Msdan";
static Mtx34 M_tmp_mtx;

enum {
    dRes_INDEX_MSDAN_BDL_MSDAN_e = 4,
    dRes_INDEX_MSDAN_DZB_MSDAN_e = 7,
};
enum {
    JA_SE_OBJ_SW_STAIR_ON_1 = 0x69BB,
    JA_SE_OBJ_SW_STAIR_OFF_1 = 0x69BC,
};
// HD: the translation callback entry is passed directly to MoveBGCreate.
static const u32 dBgS_MoveBGProc_Trans = 0x024EE76C;

using daObjMsdanSub::Act_c;

// HD: placement construction stores this translation unit's actor vtable.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x1002D930;
}

// HD: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU)
/* 0237C4B4 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u32 Act_c::prm_get_size() { return PrmAbstract(this, PRM_SIZE_W, PRM_SIZE_H); }
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_objNo() { return PrmAbstract(this, PRM_OBJNO_W, PRM_OBJNO_S); }

/* home.pos.y - 800 + (n + 1) * 25 (fmadds) */
static inline f32 step_y(f32 home_y, s32 n) { return gabi::fmadds((f32)(n + 1), 25.0f, home_y - 800.0f); }
/* home.pos.y + ((n + 1) * 25 - 800) / 2 (fmsubs, fmadds) */
static inline f32 half_y(f32 home_y, s32 n) { return gabi::fmadds(gabi::fmsubs((f32)(n + 1), 25.0f, 800.0f), 0.5f, home_y); }

/* 0237B880 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MSDAN_BDL_MSDAN_e);
    JUT_ASSERT(93, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}

/* 0237BA50 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -1000.0f, -1000.0f, -1000.0f, 1000.0f, 1000.0f, 1000.0f);
    if (prm_get_size()) {
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            mCurObjNo = 31;
            s32 obj = prm_get_objNo();
            current.pos.y = step_y(home.pos.y, obj);
        } else {
            mCurObjNo = 0;
        }
        m2D4 = 0;
    } else {
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            mCurObjNo = 31;
            s32 obj = prm_get_objNo();
            current.pos.y = step_y(home.pos.y, obj);
        } else {
            mCurObjNo = 16;
        }
        m2D4 = 16;
    }
    m2DC = home.pos.y;
    m2E0 = 0.0f;
    init_mtx();
    mpBgW->Move();
    m2E4 = false;
    return TRUE;
}

/* 0237B91C */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0237B9F0 */
void Act_c::init_mtx() {
    PSVECScale(&scale, &scale, 1.01f); /* scale *= 1.01f */
    mModel->setBaseScale(scale);
    set_mtx();
}

static inline void se_start_current(Act_c* a, u32 id) { /* fopAcM_seStartCurrent */
    mDoAud_seStart(id, &a->current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void start_shock() {
    gabi::Local<cXyz> dir;
    dVibration_c* vib = dComIfGp_getVibration();
    dir->x = 0.0f;
    dir->y = 1.0f;
    dir->z = 0.0f;
    vib->StartShock(1, -0x21, dir.get());
}

/* 0237BCB4 */
BOOL Act_c::Execute(gptr<Mtx34>* pMtx) {
    // HD: retain GHS cached field loads around pure PrmAbstract calls and unordered comparisons.
    u32 size = prm_get_size();
    u32 sw = prm_get_swSave();
    if (size) {
        if (fopAcM_isSwitch(this, sw)) {
            f32 v = m2E0;
            s32 cur = mCurObjNo;
            if (v > 0.0f) {
                v = 0.0f;
                m2E0 = v;
            }
            if (cur < 31) {
                v = v - 10.0f;
                f32 dc = m2DC + v;
                m2E0 = v;
                m2DC = dc;
                u32 obj = prm_get_objNo();
                if ((u32)(s32)mCurObjNo == obj) {
                    s32 n = mCurObjNo;
                    f32 y = step_y(home.pos.y, n);
                    current.pos.y = dc;
                    if (!(m2DC > y)) {
                        n = mCurObjNo + 1;
                        current.pos.y = y;
                        mCurObjNo = n;
                        if (m2E4 == 0) {
                            if (!(m2DC > half_y(home.pos.y, n)) && (n <= 20 || (n & 1) != 0)) {
                                start_shock();
                                m2E4 = true;
                            }
                        }
                        se_start_current(this, JA_SE_OBJ_SW_STAIR_ON_1);
                        m2E0 = 0.0f;
                    }
                } else {
                    s32 n = mCurObjNo; /* loaded after the call above */
                    f32 h = home.pos.y;
                    f32 lim = gabi::fmadds((f32)(n + 1), 25.0f, (h + h) - 800.0f) * 0.5f;
                    if (!(dc > lim)) {
                        m2E0 = 0.0f;
                        mCurObjNo = n + 1;
                        m2DC = home.pos.y;
                    }
                }
            }
        }
    } else if (fopAcM_isSwitch(this, sw)) {
        f32 v = m2E0;
        s32 cur = mCurObjNo;
        if (v > 0.0f) {
            v = 0.0f;
            m2E0 = v;
        }
        if (cur < 31) {
            v = v - 10.0f;
            f32 dc = m2DC + v;
            m2E0 = v;
            m2DC = dc;
            u32 obj = prm_get_objNo();
            s32 n = mCurObjNo;
            if ((u32)n == obj) {
                current.pos.y = dc;
                dc = m2DC; /* loaded before the second (pure) prm_get_objNo() */
                obj = prm_get_objNo();
            }
            f32 y = step_y(home.pos.y, n);
            if (!(dc > y)) {
                s32 n1 = n + 1;
                if ((u32)n == obj) {
                    current.pos.y = y;
                    se_start_current(this, JA_SE_OBJ_SW_STAIR_ON_1);
                    n1 = mCurObjNo + 1;
                }
                dc = home.pos.y;
                mCurObjNo = n1;
                m2E0 = 0.0f;
                n = n1;
                m2DC = dc;
                obj = prm_get_objNo();
            }
            if ((u32)n == obj && m2E4 == 0) {
                if (!(dc > half_y(home.pos.y, n)) && (n <= 20 || (n & 1) != 0)) {
                    start_shock();
                    m2E4 = true;
                }
            }
        }
    } else {
        f32 v = m2E0;
        s32 cur = mCurObjNo;
        s32 d4 = m2D4;
        if (v < 0.0f) {
            v = 0.0f;
            m2E0 = v;
        }
        if (cur >= d4) {
            v = v + 15.0f;
            f32 dc = m2DC + v;
            m2E0 = v;
            m2DC = dc;
            u32 obj = prm_get_objNo();
            f32 h = home.pos.y;
            if ((u32)(s32)mCurObjNo == obj) {
                current.pos.y = dc;
                dc = m2DC;
            }
            if (!(dc < h)) {
                obj = prm_get_objNo();
                s32 n = mCurObjNo;
                if ((u32)n == obj) {
                    current.pos.y = h;
                    n = mCurObjNo;
                }
                if (n > m2D4) {
                    if ((u32)n == prm_get_objNo()) {
                        se_start_current(this, JA_SE_OBJ_SW_STAIR_OFF_1);
                        n = mCurObjNo;
                        m2E4 = false;
                    }
                    n = n - 1;
                    f32 y = step_y(home.pos.y, n);
                    mCurObjNo = n;
                    m2DC = y;
                    m2E0 = 0.0f;
                } else {
                    m2DC = home.pos.y;
                    m2E0 = 0.0f;
                }
            }
        }
    }
    set_mtx();
    *pMtx = &M_tmp_mtx;
    return TRUE;
}

/* 0237C2E0 */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return TRUE;
}

/* 0237B760 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MSDAN_DZB_MSDAN_e, dBgS_MoveBGProc_Trans, 0x9A0);
        JUT_ASSERT(0x9A, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 0237B834 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}

/* method table entries (HD: tail branches) */
/* 0237C378 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0237C37C */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0237C380 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
// HD: MoveBGDraw (virtual Draw)
/* 0237C384 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
// HD: MoveBGIsDelete (virtual IsDelete)
/* 0237C394 */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 0237C3A4 */
static void __sinit_d_a_obj_msdan_sub_cpp() {
    sinit_header_statics(0x1046B944, 0x101CBCF8);
}

// HD: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8)
/* 0237C438 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}


// HD: this TU's copy of dBgS_MoveBgActor::IsDelete (virtual, returns TRUE)
/* 0237C44C */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    return TRUE;
}

// HD: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_)
/* 0237C454 */
static void SafeString_empty(void*) {
}

/* 0237C458 */
BOOL Act_c::Delete() {
    return TRUE;
}

// HD: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c)
/* 0237C460 */
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}


/* WWHD glue (tools/verify/autoglue.py)
str 1002D928
str 1002D8DC
str 1002D870 in 0237B760
str 1002D8CC "model_data != NULL" renamed # HD: assertion spells zero instead of NULL.
str 1002D888 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046B960
*/
