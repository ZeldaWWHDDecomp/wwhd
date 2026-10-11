/**
 * d_a_obj_hfuck1.cpp (WWHD)
 * Object - Hookshot target.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_hfuck1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_hfuck1.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002A274,
    .headerStatics = {0x10469E3C, 0x101C9E90, 0x10469E48},
};

static const char l_arcname[] = "Hfuck1";
static dCcD_SrcSph l_sph_src;
static const cXyz l_hook_offset = {0.0f, 0.0f, 0.0f};

/* 02353344 */
void daObjHfuck1_c::init_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02353340 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((daObjHfuck1_c*)i_this)->create_heap();
}

/* 0235325C */
bool daObjHfuck1_c::create_heap() {
    bool ret = true;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_HFUCK1_BDL_HFUCK1_e);
    if (!pModelData) {
        JUTAssertion::showAssert("d_a_obj_hfuck1.cpp", 0xF5, "0"); /* JUT_ASSERT(245, FALSE) */
        ret = false;
    } else {
        mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000022);
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_HFUCK1_DZB_HFUCK1_e);
        mpBgW = dBgW_NewSet(dzb, dBgW::MOVE_BG_e, mpModel->getBaseTRMtx());
        if (!mpModel || !mpBgW)
            ret = false;
    }
    return ret;
}

/* 02353664 */
bool daObjHfuck1_c::checkCollision() {
    bool ret = false;
    if (mSph.ChkTgHit()) {
        dCcD_GObjInf* at = (dCcD_GObjInf*)mSph.GetTgHitObj();
        if (at != nullptr && at->ChkAtType(AT_TYPE_HOOKSHOT)) {
            mpHookshotActor = mSph.GetTgHitAc();
            ret = true;
        }
        mSph.ClrTgHit();
    }
    return ret;
}

/* 02353424 */
cPhs_State daObjHfuck1_c::_create() {
    fopAcM_ct(this, daObjHfuck1_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0xC20)) {
            dBgS* bgs = dComIfG_Bgsp();
            if (bgs->Regist(mpBgW, this)) {
                ret = cPhs_ERROR_e;
            } else {
                fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
                init_mtx();
                mStts.Init(0xFF, 0xFF, this);
                mSph.Set(&l_sph_src);
                mSph.SetStts(&mStts);
                mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
                mDoMtx_stack_c::transM(0.0f, 0.0f, -50.0f);
                cXyz center; // HD: mDoMtx_stack_c::multVecZero is inlined.
                Mtx34* now = mDoMtx_stack_c::get();
                center.x = now->m[0][3];
                center.y = now->m[1][3];
                center.z = now->m[2][3];
                mSph.SetC(&center);
                mSph.SetR(90.0f);
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}

/* 023535E8 */
bool daObjHfuck1_c::_delete() {
    dComIfG_resDelete(&mPhs, l_arcname);
    if (heap != nullptr && mpBgW != nullptr) {
        if (mpBgW->ChkUsed()) {
            dBgS* bgs = dComIfG_Bgsp();
            bgs->Release(mpBgW);
        }
        mpBgW = nullptr;
    }
    return true;
}

/* 023536E0 */
bool daObjHfuck1_c::_execute() {
    mpBgW->Move();
    mStts.Move();
    if (mpHookshotActor != nullptr) {
        /* HD: fopAcM_GetName checks the actor for NULL */
        if (fopAc_IsActor(mpHookshotActor) == TRUE && mpHookshotActor != nullptr &&
            fopAcM_GetName(mpHookshotActor) == fpcNm_HOOKSHOT_e) {
            daPy_hookshot_hfuck1* player = (daPy_hookshot_hfuck1*)dComIfGp_getPlayer(0);
            if (player != nullptr) {
                player->setHookshotCarryOffset(fopAcM_GetID(this), &l_hook_offset);
            }
        }
        mpHookshotActor = nullptr;
    }
    if (!checkCollision())
        dComIfG_Ccsp_Set(&mSph);
    return true;
}

/* 023537A4 */
bool daObjHfuck1_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}

/* 023535E4 */
static cPhs_State daObjHfuck1_Create(fopAc_ac_c* i_this) {
    return ((daObjHfuck1_c*)i_this)->_create();
}
/* 02353660 */
static BOOL daObjHfuck1_Delete(daObjHfuck1_c* i_this) {
    return i_this->_delete();
}
/* 023537A0 */
static BOOL daObjHfuck1_Execute(daObjHfuck1_c* i_this) {
    return i_this->_execute();
}
/* 02353800 */
static BOOL daObjHfuck1_Draw(daObjHfuck1_c* i_this) {
    return i_this->_draw();
}
/* 02353804 */
static BOOL daObjHfuck1_IsDelete(daObjHfuck1_c* i_this) {
    return TRUE;
}

/* 0235380C */
static void __sinit_d_a_obj_hfuck1_cpp() {
    __sinit_header_statics();
}

/* 023538A0 */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023538B4 */
// HD: daObjHfuck1_c deleting destructor
static void daObjHfuck1_c_dt(daObjHfuck1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Sph__dt(&i_this->mSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02353920 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and existing local bindings
str 1002A26C
str 1002A2EC
str 1002A2E8
data l_sph_src 1002A29C
data l_hook_offset 1002A2DC
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 025166F0 void dCcD_Sph__ct(dCcD_Sph*)
ext 02515AE8 void dCcD_Sph__dt(dCcD_Sph*, s32)
*/
