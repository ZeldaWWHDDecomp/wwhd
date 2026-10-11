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
#include "d/actor/d_a_obj_aygr.h"

const gc_tu_data gc_tu = {.safeString = 0x10025300};
static const char* M_arcname = "Aygr";
static Mtx34 M_tmp_mtx;

enum {
    dRes_INDEX_AYGR_BDL_YAGURA_e = 4,
    dRes_INDEX_AYGR_BDL_HASHIGO_e = 5,
    dRes_INDEX_AYGR_DZB_YAGURA_e = 8,
    dRes_INDEX_AYGR_DZB_HASHIGO_e = 9,
};

using daObjAygr::Act_c;

// HD: the GHS actor constructor stores this translation unit's vtable.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x100253D8;
}

/* 0231AE78 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
u8 Act_c::prm_get_mdl() { return (u8)PrmAbstract(this, PRM_MDL_W, PRM_MDL_S); }

/* 0231A818 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data_yagura = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_AYGR_BDL_YAGURA_e);
    JUT_ASSERT(0x50, model_data_yagura != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data_yagura, 0, 0x11020203);
    mpModel = model;
    if (model == nullptr)
        return FALSE;
    if (prm_get_mdl()) {
        J3DModelData* model_data_hashigo = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_AYGR_BDL_HASHIGO_e);
        JUT_ASSERT(0x59, model_data_hashigo != NULL);
        model = mDoExt_J3DModel__create(model_data_hashigo, 0, 0x11020203);
        mpModel2 = model;
        if (model == nullptr)
            return FALSE;
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
        MTXCopy(mDoMtx_stack_c::get(), &mBgMtx2);
        dBgW* bgw = new_dBgW();
        mpBgW2 = bgw;
        if (bgw == nullptr)
            return FALSE;
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_AYGR_DZB_HASHIGO_e);
        if (mpBgW2->Set(dzb, dBgW::MOVE_BG_e, &mBgMtx2))
            return FALSE;
        mBgW2Regist = TRUE;
    } else {
        mpModel2 = nullptr;
        mBgW2Regist = FALSE;
        mpBgW2 = nullptr;
    }
    return TRUE;
}

/* 0231ABA4 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -100.0f, -500.0f, 500.0f, 4000.0f, 500.0f);
    return TRUE;
}

/* 0231A648 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    mBgW2Regist = FALSE;
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_AYGR_DZB_YAGURA_e, 0, 0x85F0);
        if (mpBgW != nullptr && mpBgW->ChkUsed()) {
            mpBgW->Move();
            static_cast<daObjAygr_bgW_c*>(static_cast<dBgW*>(mpBgW))->SetLock();
        }
        if (prm_get_mdl()) {
            dBgS* bgs = dComIfG_Bgsp();
            bgs->Regist(mpBgW2, this);
            if (mpBgW2 != nullptr && mpBgW2->ChkUsed()) {
                mpBgW2->Move();
                static_cast<daObjAygr_bgW_c*>(static_cast<dBgW*>(mpBgW2))->SetLock();
            }
        }
        JUT_ASSERT(0xB6, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 0231A7AC */
BOOL Act_c::Mthd_Delete() {
    if (mBgW2Regist) {
        dBgS* bgs = dComIfG_Bgsp();
        bgs->Release(mpBgW2);
    }
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}

/* 0231A9D8 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    if (prm_get_mdl()) {
        mpModel2->setBaseTRMtx(mDoMtx_stack_c::get());
    }
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0231AB28 */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    if (prm_get_mdl()) {
        mpModel2->setBaseScale(scale);
    }
    set_mtx();
}

/* 0231AC1C */
BOOL Act_c::Execute(gptr<Mtx34>* pMtx) {
    set_mtx();
    *pMtx = &M_tmp_mtx;
    return TRUE;
}

/* 0231AC58 */
BOOL Act_c::Draw() {
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

/* method table entries (HD: tail branches) */
/* 0231AD3C */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0231AD40 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0231AD44 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 0231AD48 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 0231AD58 */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 0231AD68 */
static void __sinit_d_a_obj_aygr_cpp() {
    sinit_header_statics(0x10469140, 0x101C7D60);
}

/* 0231ADFC */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}


/* 0231AE10 */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    return TRUE;
}

/* 0231AE18 */
static void SafeString_empty(void*) {
}

/* 0231AE1C */
static BOOL Act_c_Delete(Act_c*) {
    return TRUE;
}

/* 0231AE24 */
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}


/* WWHD glue (tools/verify/autoglue.py)
str 100253D0
str 10025388
str 10025318 in 0231A648
str 1002539C "model_data_yagura != NULL" renamed # HD: original spells NULL as 0
str 10025370 "model_data_hashigo != NULL" renamed # HD: original spells NULL as 0
str 1002532C "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046915C
*/
