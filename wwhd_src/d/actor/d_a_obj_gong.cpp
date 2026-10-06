/**
 * d_a_obj_gong.cpp (WWHD)
 * Object - Gong (Tetra's Ship)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_gong.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10029910) /* "Vdora" */
#define SAFESTRING_VTBL 0x10029880
#define ACT_VTBL 0x10029898 /* HD: daObjGong::Act_c vtable */

enum {
    dRes_INDEX_VDORA_BDL_VDORA_e = 4,
    dRes_INDEX_VDORA_BCK_05_VDORA_CUT02_HIT_e = 7,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0252CA4C dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c*, J3DModel*, f32): HD passes the model */
static inline void dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c* tev, J3DModel* model, f32 scale) {
    gabi::call(0x0252CA4C, eye, tev, model, scale);
}
/* 02527028 dDemo_setDemoData(actor, u8 flags, morf, arcName, int, u16*, u32, s8) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n = 0,
                                     void* ids = nullptr, u32 p6 = 0, s8 p7 = 0) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}

namespace daObjGong {
struct Act_c : fopAc_ac_c {
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();
    void init_mtx();
    void set_mtx();
    bool demo_move();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
};
WWHD_OFFSET(Act_c, mpMorf, 0x3B4);
}  // namespace daObjGong
using daObjGong::Act_c;

/* 0234BEB8. HD: no J3DSkinDeform (GameCube: new J3DSkinDeform + setSkinDeform) */
bool Act_c::create_heap() {
    WWHD_FUNC(0x0234BEB8, bool, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VDORA_BDL_VDORA_e, SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VDORA_BCK_05_VDORA_CUT02_HIT_e, SAFESTRING_VTBL);
    if (!((modelData != nullptr) && (bck != nullptr))) /* JUT_ASSERT(0xbd, ...) */
        JUT_ASSERT_fail(STR(0x100298A8), 0xbd, STR(0x100298BC));

    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                    nullptr, 0x00000000, 0x11020203);
    mDoExt_McaMorf* morf = mpMorf;
    J3DModel* model = morf != nullptr ? morf->getModel() : nullptr;
    return model != nullptr;
}
VERIFY(0x0234BEB8, &Act_c::create_heap);

/* 0234BFC0 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0234BFC0, BOOL, i_this);
    /* tail branch: create_heap's r3 is passed through unchanged */
    return gabi::call<BOOL>(0x0234BEB8, i_this); /* ((Act_c*)i_this)->create_heap() */
}
VERIFY(0x0234BFC0, solidHeapCB);

/* 0234C0B0 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x0234C0B0, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);

    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x0234BFC0 /* solidHeapCB */, 0x0)) {
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
            init_mtx();
            fopAcM_setCullSizeBox(this, -100.0f, -1.0f, -50.0f, 100.0f, 230.0f, 50.0f);
            eyePos.y += 125.0f; /* attr().offsetY */
        } else {
            ret = cPhs_ERROR_e;
        }
    }

    return ret;
}
VERIFY(0x0234C0B0, &Act_c::_create);

/* 0234C1C4 */
bool Act_c::_delete() {
    WWHD_FUNC(0x0234C1C4, bool, this);
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return true;
}
VERIFY(0x0234C1C4, &Act_c::_delete);

/* 0234BFC4 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0234BFC4, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x0234BFC4, &Act_c::set_mtx);

/* 0234C08C */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0234C08C, void, this);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    set_mtx();
}
VERIFY(0x0234C08C, &Act_c::init_mtx);

/* 0234C1F4 */
bool Act_c::demo_move() {
    WWHD_FUNC(0x0234C1F4, bool, this);
    /* ENABLE_TRANS | ENABLE_ROTATE | ENABLE_ANM | ENABLE_ANM_FRAME */
    return dDemo_setDemoData(this, 0x6A, mpMorf, STR(0x100298F8) /* "Vdora" */) != 0;
}
VERIFY(0x0234C1F4, &Act_c::demo_move);

/* 0234C23C */
bool Act_c::_execute() {
    WWHD_FUNC(0x0234C23C, bool, this);
    demo_move();
    mpMorf->play(nullptr, 0, 0);
    set_mtx();
    return true;
}
VERIFY(0x0234C23C, &Act_c::_execute);

/* 0234C288 */
bool Act_c::_draw() {
    WWHD_FUNC(0x0234C288, bool, this);
    J3DModel* model = mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dDlst_texSpecmapST(&eyePos, &tevStr, model, 0.75f /* attr().spec */); /* HD: the model, not its data */
    mpMorf->updateDL();
    return true;
}
VERIFY(0x0234C288, &Act_c::_draw);

/* method table (HD: tail branches) */
/* 0234C308 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0234C308, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x0234C308, Mthd_Create);
/* 0234C30C */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0234C30C, BOOL, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x0234C30C, Mthd_Delete);
/* 0234C310 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0234C310, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x0234C310, Mthd_Execute);
/* 0234C314 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0234C314, BOOL, i_this);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x0234C314, Mthd_Draw);
/* 0234C418 */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0234C418, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0234C418, Mthd_IsDelete);

/* 0234C318 */
static void __sinit_d_a_obj_gong_cpp() {
    WWHD_FUNC(0x0234C318, void, (u32)0);
    sinit_header_statics(0x10469CAC, 0x101C99B4);
}
VERIFY(0x0234C318, __sinit_d_a_obj_gong_cpp);

/* 0234C3AC: sead::SafeString deleting destructor (this TU's SafeString vtable) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0234C3AC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0234C3AC, SafeString_dt);

/* 0234C3C0: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void* p) { WWHD_FUNC(0x0234C3C0, void, p); }
VERIFY(0x0234C3C0, SafeString_assureTerminationImpl);

/* 0234C3C4: daObjGong::Act_c deleting destructor (vtable +0xC) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0234C3C4, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0234C3C4, Act_c_dt);
