/**
 * d_a_komore.cpp (WWHD)
 * Komore: light shafts through foliage (scrolling-texture model "frLt").
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_komore.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10013658)     /* "frLt" */
#define SAFESTRING_VTBL 0x100135E8    /* this TU's sead::SafeString vtable */
#define KOMORE_VTBL 0x10013600        /* daKomore::Act_c vtable (HD virtual destructor) */
#define FILE_NAME STR(0x10013630)     /* "d_a_komore.cpp" */

enum {
    dRes_INDEX_FRLT_BDL_YFRLT00_e = 4,
    dRes_INDEX_FRLT_BTK_YFRLT00_e = 7,
};
enum { TEV_TYPE_BG2 = 3 };

namespace daKomore {
struct Act_c : fopAc_ac_c {
    BOOL create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;  /* HD 0x74 */
    /* 0x42C */ Mtx34 mMtx;
};
}
using daKomore::Act_c;
WWHD_OFFSET(Act_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(Act_c, mMtx, 0x42C);
WWHD_SIZE(Act_c, 0x45C);

/* 021A6A6C */
BOOL Act_c::create_heap() {
    WWHD_FUNC(0x021A6A6C, bool, this);
    J3DModelData* mdl_data;
    J3DAnmTextureSRTKey* btk_data;

    mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_FRLT_BDL_YFRLT00_e, SAFESTRING_VTBL);
    if (mdl_data == nullptr) /* JUT_ASSERT(0x66, mdl_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x66, STR(0x10013610));
    else
        mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);

    btk_data = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_FRLT_BTK_YFRLT00_e, SAFESTRING_VTBL);
    if (btk_data == nullptr) /* JUT_ASSERT(0x6d, btk_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x6D, STR(0x10013620));

    s32 btkRet = mBtkAnm.init(mdl_data, btk_data, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
    bool ret = false;
    if (mdl_data != nullptr && mpModel != nullptr && btkRet != 0) {
        ret = true;
    }
    return ret;
}
VERIFY(0x021A6A6C, &Act_c::create_heap);

/* 021A6B90: daKomore::Act_c::solidHeapCB (tail call) */
static u32 solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021A6B90, u32, i_this);
    /* tail call: r3 is passed through unchanged */
    return gabi::call<u32>(0x021A6A6C /* create_heap */, i_this);
}
VERIFY(0x021A6B90, solidHeapCB);

/* 021A6B94 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x021A6B94, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    J3DModel_calc(mpModel);
}
VERIFY(0x021A6B94, &Act_c::set_mtx);

/* 021A6C88 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x021A6C88, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) { /* GHS: placement new checks this */
            fopAc_ac_c_ct(this);
            __vtbl = KOMORE_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x021A6B90 /* solidHeapCB */, 0x0)) {
            set_mtx();
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
            fopAcM_setCullSizeBox(this, -100.0f, -1000.0f, -100.0f, 100.0f, 100.0f, 100.0f);
        } else {
            phase_state = cPhs_ERROR_e;
        }
    }
    return phase_state;
}
VERIFY(0x021A6C88, &Act_c::_create);

/* 021A6D84 */
bool Act_c::_delete() {
    WWHD_FUNC(0x021A6D84, bool, this);
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}
VERIFY(0x021A6D84, &Act_c::_delete);

/* 021A6DB4 */
bool Act_c::_execute() {
    WWHD_FUNC(0x021A6DB4, bool, this);
    mBtkAnm.play();
    return true;
}
VERIFY(0x021A6DB4, &Act_c::_execute);

/* 021A6DDC */
bool Act_c::_draw() {
    WWHD_FUNC(0x021A6DDC, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG2, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBtkAnm.entry(J3DModel_getModelData(mpModel), mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
VERIFY(0x021A6DDC, &Act_c::_draw);

/* 021A6E4C..021A6E58, 021A6F5C: method table (tail calls) */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x021A6E4C, cPhs_State, i_this);
    return static_cast<Act_c*>(i_this)->_create();
}
VERIFY(0x021A6E4C, Mthd_Create);

static u32 Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x021A6E50, u32, i_this);
    return gabi::call<u32>(0x021A6D84 /* _delete (tail call) */, i_this);
}
VERIFY(0x021A6E50, Mthd_Delete);

static u32 Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x021A6E54, u32, i_this);
    return gabi::call<u32>(0x021A6DB4 /* _execute (tail call) */, i_this);
}
VERIFY(0x021A6E54, Mthd_Execute);

static u32 Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x021A6E58, u32, i_this);
    return gabi::call<u32>(0x021A6DDC /* _draw (tail call) */, i_this);
}
VERIFY(0x021A6E58, Mthd_Draw);

/* 021A6F5C (placed after the destructor) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x021A6F5C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021A6F5C, Mthd_IsDelete);

/* 021A6E5C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_komore_cpp() {
    WWHD_FUNC(0x021A6E5C, void, (u32)0);
    sinit_header_statics(0x10464C58, 0x101B87C8);
}
VERIFY(0x021A6E5C, __sinit_d_a_komore_cpp);

/* 021A6EF0: sead::SafeString deleting destructor (this TU's vtable 0x100135E8) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021A6EF0, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021A6EF0, SafeString_dt);

/* 021A6F04: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021A6F04, void, (u32)0);
}
VERIFY(0x021A6F04, SafeString_assureTerminationImpl);

/* 021A6F08: daKomore::Act_c deleting destructor (virtual ~Act_c() {}) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x021A6F08, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021A6F08, Act_c_dt);
