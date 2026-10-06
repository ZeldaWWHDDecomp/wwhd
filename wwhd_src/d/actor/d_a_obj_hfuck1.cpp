/**
 * d_a_obj_hfuck1.cpp (WWHD)
 * Object - Hookshot target.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_hfuck1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define l_arcname STR(0x1002A26C) /* "Hfuck1" */
#define SAFESTRING_VTBL 0x1002A274
#define HFUCK1_VTBL 0x1002A28C
#define l_sph_src 0x1002A29C
#define l_hook_offset 0x1002A2DC

enum { dRes_INDEX_HFUCK1_BDL_HFUCK1_e = 4, dRes_INDEX_HFUCK1_DZB_HFUCK1_e = 7 };
enum { fpcNm_HOOKSHOT_e = 0xA9 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 024F2478 dBgW_NewSet(cBgD_t*, u32 flag, Mtx34*) -> dBgW* */
static inline dBgW* dBgW_NewSet(void* dzb, u32 flag, Mtx34* mtx) { return gabi::call<dBgW*>(0x024F2478, dzb, flag, mtx); }
/* 025F19F8 mDoMtx_XYZrotM(Mtx, s16, s16, s16) */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 025166F0 dCcD_Sph::dCcD_Sph (out of line) */
static inline void dCcD_Sph_ct(dCcD_Sph* p) { gabi::call(0x025166F0, p); }
/* daPy_py_c::setHookshotCarryOffset: virtual, vtable slot +0x10C */
static inline void daPy_setHookshotCarryOffset(fopAc_ac_c* player, u32 id, u32 offset) {
    gabi::call_ptr(gabi::load<u32>(player->__vtbl + 0x10C), player, id, offset);
}
static inline u32 fopAcM_GetID(fopAc_ac_c* a) { return gabi::load<u32>(gabi::ea(a) + 4); }

struct daObjHfuck1_c : fopAc_ac_c {
    void init_mtx();
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();
    bool checkCollision();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ dCcD_Stts mStts;
    /* 0x3F8 */ dCcD_Sph mSph;
    /* 0x524 */ gptr<fopAc_ac_c> mpHookshotActor;
};
WWHD_OFFSET(daObjHfuck1_c, mStts, 0x3BC);
WWHD_OFFSET(daObjHfuck1_c, mSph, 0x3F8);
WWHD_OFFSET(daObjHfuck1_c, mpHookshotActor, 0x524);
WWHD_SIZE(daObjHfuck1_c, 0x528);

/* 02353344 */
void daObjHfuck1_c::init_mtx() {
    WWHD_FUNC(0x02353344, void, this);
    /* mpModel->setBaseScale(scale) */
    u32 m = gabi::ea(mpModel.get());
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    gabi::store<f32>(m + 0xBC, sx);
    gabi::store<f32>(m + 0xC0, sy);
    gabi::store<f32>(m + 0xC4, sz);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02353344, &daObjHfuck1_c::init_mtx);

/* 02353340 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02353340, BOOL, i_this);
    return ((daObjHfuck1_c*)i_this)->create_heap();
}
VERIFY(0x02353340, solidHeapCB);

/* 0235325C */
bool daObjHfuck1_c::create_heap() {
    WWHD_FUNC(0x0235325C, bool, this);
    bool ret = true;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_HFUCK1_BDL_HFUCK1_e, SAFESTRING_VTBL);
    if (!pModelData) {
        JUT_ASSERT_fail(STR(0x1002A2EC), 0xF5, STR(0x1002A2E8)); /* JUT_ASSERT(245, FALSE) */
        ret = false;
    } else {
        mpModel = mDoExt_J3DModel__create(pModelData, 0x80000, 0x11000022);
        void* dzb = dComIfG_getObjectRes(l_arcname, dRes_INDEX_HFUCK1_DZB_HFUCK1_e, SAFESTRING_VTBL);
        mpBgW = dBgW_NewSet(dzb, cBgW_MOVE_BG_e, J3DModel_getBaseTRMtx(mpModel));
        if (!mpModel || !mpBgW)
            ret = false;
    }
    return ret;
}
VERIFY(0x0235325C, &daObjHfuck1_c::create_heap);

/* 02353664 */
bool daObjHfuck1_c::checkCollision() {
    WWHD_FUNC(0x02353664, bool, this);
    bool ret = false;
    if (mSph.ChkTgHit()) {
        void* at = mSph.GetTgHitObj();
        if (at != nullptr && cCcD_Obj_ChkAtType(at, 0x8000 /* AT_TYPE_HOOKSHOT */)) {
            mpHookshotActor = mSph.GetTgHitAc();
            ret = true;
        }
        mSph.ClrTgHit();
    }
    return ret;
}
VERIFY(0x02353664, &daObjHfuck1_c::checkCollision);

/* 02353424 */
cPhs_State daObjHfuck1_c::_create() {
    WWHD_FUNC(0x02353424, cPhs_State, this);
    /* fopAcM_ct(this, daObjHfuck1_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = HFUCK1_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Sph_ct(&mSph);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x02353340 /* solidHeapCB */, 0xC20)) {
            dBgS* bgs = dComIfG_Bgsp();
            if (dBgS_Regist(bgs, mpBgW, this)) {
                ret = cPhs_ERROR_e;
            } else {
                cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
                init_mtx();
                mStts.Init(0xFF, 0xFF, this);
                mSph.Set((const dCcD_SrcSph*)gabi::at<u8>(l_sph_src));
                mSph.SetStts(&mStts);
                mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
                mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
                mDoMtx_stack_c::transM(0.0f, 0.0f, -50.0f);
                gabi::Local<cXyz> center; /* mDoMtx_stack_c::multVecZero(&center) */
                Mtx34* now = mDoMtx_stack_c::get();
                center->x = now->m[0][3];
                center->y = now->m[1][3];
                center->z = now->m[2][3];
                mSph.SetC(center);
                mSph.SetR(90.0f);
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}
VERIFY(0x02353424, &daObjHfuck1_c::_create);

/* 023535E8 */
bool daObjHfuck1_c::_delete() {
    WWHD_FUNC(0x023535E8, bool, this);
    dComIfG_resDelete(&mPhs, l_arcname);
    if (heap != nullptr && mpBgW != nullptr) {
        if (dBgW_ChkUsed(mpBgW)) {
            dBgS* bgs = dComIfG_Bgsp();
            cBgS_Release(bgs, mpBgW);
        }
        mpBgW = nullptr;
    }
    return true;
}
VERIFY(0x023535E8, &daObjHfuck1_c::_delete);

/* 023536E0 */
bool daObjHfuck1_c::_execute() {
    WWHD_FUNC(0x023536E0, bool, this);
    dBgW_Move(mpBgW);
    mStts.Move();
    if (mpHookshotActor != nullptr) {
        /* HD: fopAcM_GetName checks the actor for NULL */
        if (fopAc_IsActor(mpHookshotActor) == TRUE && mpHookshotActor != nullptr &&
            fpcM_GetName(mpHookshotActor) == fpcNm_HOOKSHOT_e) {
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            if (player != nullptr) {
                daPy_setHookshotCarryOffset(player, fopAcM_GetID(this), l_hook_offset);
            }
        }
        mpHookshotActor = nullptr;
    }
    if (!checkCollision())
        dComIfG_Ccsp_Set(&mSph);
    return true;
}
VERIFY(0x023536E0, &daObjHfuck1_c::_execute);

/* 023537A4 */
bool daObjHfuck1_c::_draw() {
    WWHD_FUNC(0x023537A4, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
VERIFY(0x023537A4, &daObjHfuck1_c::_draw);

/* 023535E4 */
static cPhs_State daObjHfuck1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023535E4, cPhs_State, i_this);
    return ((daObjHfuck1_c*)i_this)->_create();
}
VERIFY(0x023535E4, daObjHfuck1_Create);
/* 02353660 */
static BOOL daObjHfuck1_Delete(daObjHfuck1_c* i_this) {
    WWHD_FUNC(0x02353660, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02353660, daObjHfuck1_Delete);
/* 023537A0 */
static BOOL daObjHfuck1_Execute(daObjHfuck1_c* i_this) {
    WWHD_FUNC(0x023537A0, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x023537A0, daObjHfuck1_Execute);
/* 02353800 */
static BOOL daObjHfuck1_Draw(daObjHfuck1_c* i_this) {
    WWHD_FUNC(0x02353800, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02353800, daObjHfuck1_Draw);
/* 02353804 */
static BOOL daObjHfuck1_IsDelete(daObjHfuck1_c* i_this) {
    WWHD_FUNC(0x02353804, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02353804, daObjHfuck1_IsDelete);

/* 0235380C */
static void __sinit_d_a_obj_hfuck1_cpp() {
    WWHD_FUNC(0x0235380C, void, (u32)0);
    sinit_header_statics(0x10469E3C, 0x101C9E90);
}
VERIFY(0x0235380C, __sinit_d_a_obj_hfuck1_cpp);

/* 023538A0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023538A0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023538A0, trivial_dt);

/* 023538B4: daObjHfuck1_c deleting destructor */
static void daObjHfuck1_c_dt(daObjHfuck1_c* i_this, s32 flags) {
    WWHD_FUNC(0x023538B4, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0);        /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023538B4, daObjHfuck1_c_dt);

/* 02353920: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02353920, void, p);
}
VERIFY(0x02353920, empty_virtual);
