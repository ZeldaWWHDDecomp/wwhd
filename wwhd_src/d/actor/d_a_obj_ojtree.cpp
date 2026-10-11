/**
 * d_a_obj_ojtree.cpp (WWHD)
 * Object - Tall thin poles leading to the Deku Leaf (Forest Haven interior)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_ojtree.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_ojtree.h"

const gc_tu_data gc_tu = {.safeString = 0x1002DFD0};
static const char* M_arcname = "Ojtree";
static Mtx34 M_tmp_mtx;

enum {
    dRes_INDEX_OJTREE_BDL_OJTREE_e = 4,
    dRes_INDEX_OJTREE_DZB_OJTREE_e = 7,
};
enum { fpcNm_JBO_e = 0xD5, daJbo_Type_NORMAL_e = 0 };

using daObjOjtree::Act_c;

// HD: GHS stores this translation unit's vtable during placement construction.
Act_c::Act_c() {
    dBgS_MoveBgActor::ct(this);
    __vtbl = 0x1002E090;
}

/* 0237FDF4 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_OJTREE_BDL_OJTREE_e);
    JUT_ASSERT(67, model_data != NULL);
    J3DModel* model = mDoExt_J3DModel__create(model_data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr; /* HD: tests the returned pointer */
}

/* 0237FF84 */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
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

/* 0237FCD8 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_OJTREE_DZB_OJTREE_e, 0, 0x26A0);
        // HD: assertion line 122, rather than GameCube line 123.
        JUT_ASSERT(122, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 0237FDA8 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return result;
}

/* 0237FE90 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0237FF64 */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 02380054 */
BOOL Act_c::Execute(gptr<Mtx34>* pMtx) {
    if (mLockTimer != 0) {
        u8 t = (u8)(mLockTimer - 1);
        mLockTimer = t;
        if (t == 0) {
            static_cast<daObjOjtree_bgW_c*>(static_cast<dBgW*>(mpBgW))->SetLock();
        }
    }
    set_mtx();
    *pMtx = &M_tmp_mtx;
    return TRUE;
}

/* 023800BC */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 02380154 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 02380158 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0238015C */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 02380160 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 02380170 */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 02380180 */
static void __sinit_d_a_obj_ojtree_cpp() {
    sinit_header_statics(0x1046BADC, 0x101CC0F0);
}

/* 02380214 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02380228 */
static BOOL MoveBgActor_IsDelete(Act_c*) {
    return TRUE;
}

/* 02380230 */
static void SafeString_empty(void*) {
}

/* 02380234 */
BOOL Act_c::Delete() {
    return TRUE;
}

/* 0238023C */
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002E088
str 1002E050
str 1002DFE8 in 0237FCD8
str 1002E040 "model_data != NULL" renamed # HD: 0 rather than NULL.
str 1002DFFC "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046BAF8
*/
