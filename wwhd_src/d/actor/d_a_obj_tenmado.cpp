/**
 * d_a_obj_tenmado.cpp (WWHD)
 * Object - Tenmado (two-leaf shutter window that opens when a switch is set)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_tenmado.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_tenmado.h"

const gc_tu_data gc_tu = {.safeString = 0x10030A18};
static const char* M_arcname = "Tenmado";
static Mtx34 M_tmp_mtx;

enum {
    dRes_INDEX_TENMADO_BDL_MMADOL_e = 4,
    dRes_INDEX_TENMADO_BDL_MMADOR_e = 5,
    dRes_INDEX_TENMADO_DZB_MMADO_e = 8,
};
enum { JA_SE_OBJ_TENMADO_SHTR_OP = 0x69BF };

using daObjTenmado::Act_c;

// HD: GHS stores this translation unit's actor vtable during placement construction.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x10030AF0;
}

/* 023A03FC */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_swSave2() { return PrmAbstract(this, PRM_SWSAVE2_W, PRM_SWSAVE2_S); }

/* 0239FC78 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data_l = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TENMADO_BDL_MMADOL_e);
    JUT_ASSERT(85, model_data_l != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data_l, 0, 0x11020203);
    mModel1 = model;
    if (model == nullptr) {
        return FALSE;
    }
    J3DModelData* model_data_r = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_TENMADO_BDL_MMADOR_e);
    JUT_ASSERT(94, model_data_r != NULL);
    model = mDoExt_J3DModel__create(model_data_r, 0, 0x11020203);
    mModel2 = model;
    if (model == nullptr) {
        return FALSE;
    }
    return TRUE;
}

/* 0239FF5C */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mModel1->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -500.0f, -500.0f, 500.0f, 500.0f, 500.0f);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        dComIfGs_onSwitch(prm_get_swSave2(), home.roomNo); /* fopAcM_onSwitch */
        m2C8 = 200.0f;
        m2CC = MODE_OPEN_e;
    } else {
        m2C8 = 0.0f;
        m2CC = MODE_WAIT_e;
    }
    return TRUE;
}

/* 0239FB5C */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_TENMADO_DZB_MMADO_e, 0, 0xF00);
        // HD: the assertion line is 145 (GameCube uses 146).
        JUT_ASSERT(145, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 0239FC2C */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhase, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}

/* 0239FD7C */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
    mDoMtx_stack_c::transM(-m2C8, 0.0f, 0.0f);
    mModel1->setBaseTRMtx(mDoMtx_stack_c::get());
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(m2C8, 0.0f, 0.0f);
    mModel2->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0239FF20 */
void Act_c::init_mtx() {
    mModel1->setBaseScale(scale);
    mModel2->setBaseScale(scale);
    set_mtx();
}

/* 023A0060 */
BOOL Act_c::Execute(gptr<Mtx34>* mtx) {
    switch ((u32)(s32)m2CC) {
    case MODE_WAIT_e:
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            m2CC = MODE_START_e;
        }
        break;
    case MODE_START_e:
        mDoAud_seStart(JA_SE_OBJ_TENMADO_SHTR_OP, &current.pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this))); /* fopAcM_seStartCurrent */
        m2CC = MODE_OPENING_e;
        break;
    case MODE_OPENING_e: {
        // HD: unordered floating comparisons take the same branch as reaching the limit.
        f32 v = m2C8 + 1.0f;
        if (!(v < 100.0f)) {
            m2C8 = 100.0f;
            m2CC = MODE_OPEN_e;
        } else {
            m2C8 = v;
            if (!(v < 80.0f)) {
                dComIfGs_onSwitch(prm_get_swSave2(), home.roomNo); /* fopAcM_onSwitch */
            }
        }
        break;
    }
    default:
        break;
    }
    set_mtx();
    *mtx = &M_tmp_mtx;
    return TRUE;
}

/* 023A020C */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel1, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel2, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel1);
    mDoExt_modelUpdateDL(mModel2);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 023A02C0 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 023A02C4 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 023A02C8 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 023A02CC */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 023A02DC */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 023A02EC */
static void __sinit_d_a_obj_tenmado_cpp() {
    sinit_header_statics(0x1046C454, 0x101CD1C8);
}

/* 023A0380 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}


/* 023A0394 */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    return TRUE;
}

/* 023A039C */
static void SafeString_empty(void*) {
}

/* 023A03A0 */
static BOOL Act_c_Delete(Act_c*) {
    return TRUE;
}

/* 023A03A8 */
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}


/* WWHD glue (tools/verify/autoglue.py)
str 10030AE8
str 10030A88
str 10030A30 in 0239FB5C
str 10030A9C "model_data_l != NULL" renamed # HD: 0 rather than NULL.
str 10030AB0 "model_data_r != NULL" renamed # HD: 0 rather than NULL.
str 10030A44 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046C470
*/
