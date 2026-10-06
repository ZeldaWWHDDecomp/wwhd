/**
 * d_a_obj_Vteng.cpp (WWHD)
 * Object - Puppet Ganon curtains (intro cutscene)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_Vteng.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define l_arcname STR(0x10033104)   /* "Vteng" */
#define SAFESTRING_VTBL 0x1003310C  /* this TU's sead::SafeString vtable */
#define VTENG_VTBL 0x10033124       /* daObjVteng_c vtable (HD virtual destructor) */

enum {
    dRes_INDEX_VTENG_BCK_VTENG_e = 5,
    dRes_INDEX_VTENG_BDL_VTENG_e = 8,
    dRes_INDEX_VTENG_DZB_VTENG_e = 0xB,
};
enum { TEV_TYPE_BG3 = 4 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025F19F8 mDoMtx_XYZrotM(Mtx, s16, s16, s16) (as in d_a_fallrock.cpp) */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 024F2478 dBgW_NewSet(cBgD_t*, u32 flags, Mtx*) */
static inline dBgW* dBgW_NewSet(void* dzb, u32 flags, Mtx34* mtx) { return gabi::call<dBgW*>(0x024F2478, dzb, flags, mtx); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) (as in d_a_npc_zl1.h) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, u16* ids,
                                     u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}

struct daObjVteng_c : fopAc_ac_c {
    void init_mtx();
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();
    bool jokai_demo();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ gptr<dBgW> mpBgW;
    /* 0x3C0 */ Mtx34 mtx;
};
WWHD_OFFSET(daObjVteng_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjVteng_c, mpBgW, 0x3BC);
WWHD_OFFSET(daObjVteng_c, mtx, 0x3C0);
WWHD_SIZE(daObjVteng_c, 0x3F0);

/* 023B59D0 */
void daObjVteng_c::init_mtx() {
    WWHD_FUNC(0x023B59D0, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
}
VERIFY(0x023B59D0, &daObjVteng_c::init_mtx);

/* 023B59CC */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023B59CC, BOOL, i_this);
    return ((daObjVteng_c*)i_this)->create_heap();
}
VERIFY(0x023B59CC, solidHeapCB);

/* 023B5898 */
bool daObjVteng_c::create_heap() {
    WWHD_FUNC(0x023B5898, bool, this);
    bool ret = true;

    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_VTENG_BDL_VTENG_e, SAFESTRING_VTBL);
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_VTENG_BCK_VTENG_e, SAFESTRING_VTBL);

    if (!pModelData || !pAnm) {
        JUT_ASSERT_fail(STR(0x1003313C), 0xb7, STR(0x10033138)); /* JUT_ASSERT(0xb7, FALSE) */
        ret = false;
    } else {
        mpMorf = mDoExt_McaMorf::create(nullptr, pModelData, nullptr, nullptr, pAnm, 0 /* EMode_NONE */, 1.0f, 0x3B, -1,
                                        0, nullptr, 0x00000000, 0x11020203);
        if (!mpMorf) {
            ret = false;
        } else {
            mpModel = mpMorf->getModel();
            mpBgW = dBgW_NewSet(dComIfG_getObjectRes(l_arcname, dRes_INDEX_VTENG_DZB_VTENG_e, SAFESTRING_VTBL),
                                1 /* cBgW::MOVE_BG_e */, &mtx);
            if (!mpBgW)
                ret = false;
        }
    }

    return ret;
}
VERIFY(0x023B5898, &daObjVteng_c::create_heap);

/* 023B5C28 */
bool daObjVteng_c::jokai_demo() {
    WWHD_FUNC(0x023B5C28, bool, this);
    bool ret = false;
    if (dDemo_setDemoData(this, 0x60 /* ENABLE_ANM_e | ENABLE_ANM_FRAME_e */, mpMorf, STR(0x10033150), 0, nullptr, 0, 0) == TRUE)
        ret = true;
    return ret;
}
VERIFY(0x023B5C28, &daObjVteng_c::jokai_demo);

/* 023B5ACC */
cPhs_State daObjVteng_c::_create() {
    WWHD_FUNC(0x023B5ACC, cPhs_State, this);
    /* fopAcM_ct(this, daObjVteng_c); HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = VTENG_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname);

    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x023B59CC /* solidHeapCB */, 0x72a0)) {
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
            init_mtx();
            if (dBgS_Regist(dComIfG_Bgsp(), mpBgW, this)) {
                ret = cPhs_ERROR_e;
            }
        } else {
            ret = cPhs_ERROR_e;
        }
    }

    return ret;
}
VERIFY(0x023B5ACC, &daObjVteng_c::_create);

/* 023B5BAC */
bool daObjVteng_c::_delete() {
    WWHD_FUNC(0x023B5BAC, bool, this);
    dComIfG_resDelete(&mPhs, l_arcname);

    if (heap && mpBgW) {
        if (dBgW_ChkUsed(mpBgW)) {
            cBgS_Release(dComIfG_Bgsp(), mpBgW);
        }
        mpBgW = nullptr;
    }

    return true;
}
VERIFY(0x023B5BAC, &daObjVteng_c::_delete);

/* 023B5C84 */
bool daObjVteng_c::_execute() {
    WWHD_FUNC(0x023B5C84, bool, this);
    if (mpBgW && dBgW_ChkUsed(mpBgW))
        dBgW_Move(mpBgW);
    if (!jokai_demo())
        mpMorf->play(nullptr, 0, 0);
    return true;
}
VERIFY(0x023B5C84, &daObjVteng_c::_execute);

/* 023B5CF4 */
bool daObjVteng_c::_draw() {
    WWHD_FUNC(0x023B5CF4, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG3, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mpMorf->updateDL();
    model = gabi::ea(mpModel.get()); /* fopAcM_SetModel */
    return true;
}
VERIFY(0x023B5CF4, &daObjVteng_c::_draw);

/* 023B5BA8 */
static cPhs_State daObjVteng_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023B5BA8, cPhs_State, i_this);
    return ((daObjVteng_c*)i_this)->_create();
}
VERIFY(0x023B5BA8, daObjVteng_Create);

/* 023B5C24 */
static BOOL daObjVteng_Delete(daObjVteng_c* i_this) {
    WWHD_FUNC(0x023B5C24, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x023B5C24, daObjVteng_Delete);

/* 023B5CF0 */
static BOOL daObjVteng_Execute(daObjVteng_c* i_this) {
    WWHD_FUNC(0x023B5CF0, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x023B5CF0, daObjVteng_Execute);

/* 023B5D54 */
static BOOL daObjVteng_Draw(daObjVteng_c* i_this) {
    WWHD_FUNC(0x023B5D54, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x023B5D54, daObjVteng_Draw);

/* 023B5D58 */
static BOOL daObjVteng_IsDelete(daObjVteng_c* i_this) {
    WWHD_FUNC(0x023B5D58, BOOL, i_this);
    return TRUE;
}
VERIFY(0x023B5D58, daObjVteng_IsDelete);

/* 023B5D60 */
static void __sinit_d_a_obj_Vteng_cpp() {
    WWHD_FUNC(0x023B5D60, void, (u32)0);
    sinit_header_statics(0x1046C934, 0x101CDC7C);
}
VERIFY(0x023B5D60, __sinit_d_a_obj_Vteng_cpp);

/* 023B5DF4: deleting destructor of a class with a trivial destructor (sead::SafeString, per TU) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023B5DF4, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023B5DF4, trivial_dt);

/* 023B5E08: daObjVteng_c deleting destructor */
static void daObjVteng_c_dt(daObjVteng_c* i_this, s32 flags) {
    WWHD_FUNC(0x023B5E08, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023B5E08, daObjVteng_c_dt);

/* 023B5E5C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x023B5E5C, void, p);
}
VERIFY(0x023B5E5C, empty_virtual);
