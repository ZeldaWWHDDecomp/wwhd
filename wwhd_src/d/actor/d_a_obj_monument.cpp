/**
 * d_a_obj_monument.cpp (WWHD)
 * Object - Monument ("Esekh" stone tablet with its own background collision)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_monument.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002CF48) /* "Esekh" */
#define SAFESTRING_VTBL 0x1002CEF0
#define ACT_VTBL 0x1002CF08 /* HD: daObjMonument::Act_c vtable */
/* L_attr[] = { {ESEKH bdl, ESEKH dzb}, {ESEKH2 bdl, ESEKH2 dzb} }: s16 pairs at 0x101CB168 */
#define L_attr_ea 0x101CB168u

namespace daObjMonument {
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_TYPE_W = 0x01,
        PRM_TYPE_S = 0x00,
        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x08,
    };
    s32 prm_get_type();
    u32 prm_get_swSave();
    bool create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void init_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ be<s32> mType;
    /* 0x3BC */ gptr<dBgW> mpBgW;
    /* 0x3C0 */ Mtx34 mtx;
};
WWHD_OFFSET(Act_c, mType, 0x3B8);
WWHD_OFFSET(Act_c, mtx, 0x3C0);
WWHD_SIZE(Act_c, 0x3F0);
}  // namespace daObjMonument
using daObjMonument::Act_c;

static inline s16 attr_mModelId(s32 type) { return gabi::load<s16>(L_attr_ea + 4 * type); }
static inline s16 attr_mBgWId(s32 type) { return gabi::load<s16>(L_attr_ea + 4 * type + 2); }

/* 0237625C: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0237625C, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0237625C, PrmAbstract);
s32 Act_c::prm_get_type() { return (s32)PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 02375E18 (HD: a tail branch) */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02375E18, BOOL, i_this);
    return ((Act_c*)i_this)->create_heap();
}
VERIFY(0x02375E18, solidHeapCB);

/* 02375C64 */
bool Act_c::create_heap() {
    WWHD_FUNC(0x02375C64, bool, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, attr_mModelId(mType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x81, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1002CF18), 0x81, STR(0x1002CF30));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x00, 0x11020203);
    mpModel = model;
    if (model == nullptr)
        return false;

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());

    dBgW* bgw = new_dBgW();
    mpBgW = bgw;
    if (bgw != nullptr) {
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, attr_mBgWId(mType), SAFESTRING_VTBL);
        if (cBgW_Set(mpBgW, dzb, cBgW_MOVE_BG_e, &mtx))
            return false;
    }
    return true;
}
VERIFY(0x02375C64, &Act_c::create_heap);

/* 02375F00 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x02375F00, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);

    mpBgW = nullptr;
    if (ret == cPhs_COMPLEATE_e) {
        mType = prm_get_type();

        if (fopAcM_entrySolidHeap(this, 0x02375E18 /* solidHeapCB */, 0xD20)) {
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
            dBgS* bgs = dComIfG_Bgsp();
            dBgS_Regist(bgs, mpBgW, this);
            init_mtx();
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}
VERIFY(0x02375F00, &Act_c::_create);

/* 02375FF4 */
bool Act_c::_delete() {
    WWHD_FUNC(0x02375FF4, bool, this);
    if (mpBgW != nullptr && dBgW_ChkUsed(mpBgW)) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, mpBgW);
    }
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return true;
}
VERIFY(0x02375FF4, &Act_c::_delete);

/* 02375E1C */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02375E1C, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02375E1C, &Act_c::set_mtx);

/* 02375EE0 (matcher: daObj::PrmAbstract) */
void Act_c::init_mtx() {
    WWHD_FUNC(0x02375EE0, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x02375EE0, &Act_c::init_mtx);

/* 02376058 */
bool Act_c::_execute() {
    WWHD_FUNC(0x02376058, bool, this);
    set_mtx();
    return true;
}
VERIFY(0x02376058, &Act_c::_execute);

/* 0237607C: HD: tev type 1 (TEV_TYPE_BG0 in the bindings' numbering; GameCube source: TEV_TYPE_BG1) */
bool Act_c::_draw() {
    WWHD_FUNC(0x0237607C, bool, this);
    u32 swSave = prm_get_swSave();
    if (dComIfGs_isSwitch(swSave, home.roomNo))
        return true;

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}
VERIFY(0x0237607C, &Act_c::_draw);

/* method table entries (HD: tail branches) */
/* 02376144 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02376144, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x02376144, Mthd_Create);
/* 02376148 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x02376148, BOOL, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x02376148, Mthd_Delete);
/* 0237614C */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0237614C, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x0237614C, Mthd_Execute);
/* 02376150 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02376150, BOOL, i_this);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x02376150, Mthd_Draw);
/* 02376254 */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x02376254, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02376254, Mthd_IsDelete);

/* 02376154 */
static void __sinit_d_a_obj_monument_cpp() {
    WWHD_FUNC(0x02376154, void, (u32)0);
    sinit_header_statics(0x1046A664, 0x101CB144);
}
VERIFY(0x02376154, __sinit_d_a_obj_monument_cpp);

/* 023761E8: this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023761E8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x023761E8, trivial_dt);

/* 023761FC: this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_) */
static void SafeString_empty(void*) {
    WWHD_FUNC(0x023761FC, void, (u32)0);
}
VERIFY(0x023761FC, SafeString_empty);

/* 02376200: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x02376200, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02376200, Act_c_dt);
