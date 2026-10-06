/**
 * d_a_scene_change.cpp (WWHD)
 * Scene change marker (transform matrix only).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_scene_change.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x10039DFC /* HD: d_a_scene_change_c vtable */
#define HIO_VTBL 0x10039DEC /* daSceneChgHIO_c vtable */

struct d_a_scene_change_c : fopAc_ac_c {
    /* 0x3AC */ Mtx34 mTransformMtx; /* GameCube 0x290 */
};
WWHD_OFFSET(d_a_scene_change_c, mTransformMtx, 0x3AC);

/* HD: GHS places the vtable pointer after the members */
struct daSceneChgHIO_c {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ be<s8> m0005;
    /* 0x2 */ u8 _2[2];
    /* 0x4 */ be<f32> m0008;
    /* 0x8 */ be<u32> __vtbl;
};
WWHD_SIZE(daSceneChgHIO_c, 0xC);

static daSceneChgHIO_c& l_HIO() { return *gabi::at<daSceneChgHIO_c>(0x1046D8A4); }

/* 0246B020 */
static cPhs_State daSceneChgCreate(void* i_this) {
    WWHD_FUNC(0x0246B020, cPhs_State, i_this);
    d_a_scene_change_c* scnChg = static_cast<d_a_scene_change_c*>(i_this);

    /* fopAcM_ct(scnChg, d_a_scene_change_c) */
    if (!fopAcM_CheckCondition(scnChg, fopAcCnd_INIT_e)) {
        if (scnChg != nullptr) {
            fopAc_ac_c_ct(scnChg);
            scnChg->__vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(scnChg, fopAcCnd_INIT_e);
    }

    mDoMtx_stack_c::transS(scnChg->current.pos.x, scnChg->current.pos.y, scnChg->current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), scnChg->shape_angle.x, scnChg->shape_angle.y, scnChg->shape_angle.z);

    PSMTXCopy(mDoMtx_stack_c::get(), &scnChg->mTransformMtx); /* cMtx_copy */
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0246B020, daSceneChgCreate);

/* 0246B0C0 */
static BOOL daSceneChgDelete(void* i_this) {
    WWHD_FUNC(0x0246B0C0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246B0C0, daSceneChgDelete);

/* 0246B0C8 */
static BOOL daSceneChgExecute(void* i_this) {
    WWHD_FUNC(0x0246B0C8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246B0C8, daSceneChgExecute);

/* 0246B0D0 */
static BOOL daSceneChgDraw(void* i_this) {
    WWHD_FUNC(0x0246B0D0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246B0D0, daSceneChgDraw);

/* 0246B1E0 */
static BOOL daSceneChgIsDelete(void* i_this) {
    WWHD_FUNC(0x0246B1E0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246B1E0, daSceneChgIsDelete);

/* 0246B0D8: daSceneChgHIO_c::daSceneChgHIO_c (allocates when this == NULL) */
static daSceneChgHIO_c* daSceneChgHIO_c_ct(daSceneChgHIO_c* h) {
    WWHD_FUNC(0x0246B0D8, daSceneChgHIO_c*, h);
    if (h == nullptr) {
        h = (daSceneChgHIO_c*)operator_new(0xC);
        if (h == nullptr)
            return h;
    }
    h->__vtbl = HIO_VTBL;
    h->mNo = -1;
    h->m0005 = 0;
    h->m0008 = 100.0f;
    return h;
}
VERIFY(0x0246B0D8, daSceneChgHIO_c_ct);

/* 0246B134: __sinit_d_a_scene_change_cpp: header statics, then l_HIO (destructor registered) */
static void __sinit_d_a_scene_change_cpp() {
    WWHD_FUNC(0x0246B134, void, (u32)0);
    sinit_header_statics(0x1046D888, 0x101D0080);
    daSceneChgHIO_c_ct(&l_HIO());
    __register_global_object(0x101D00A4);
}
VERIFY(0x0246B134, __sinit_d_a_scene_change_cpp);

/* 0246B1E8: d_a_scene_change_c deleting destructor */
static void d_a_scene_change_c_dt(d_a_scene_change_c* i_this, s32 flags) {
    WWHD_FUNC(0x0246B1E8, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0246B1E8, d_a_scene_change_c_dt);

/* 0246B23C: daSceneChgHIO_c deleting destructor: virtual ~daSceneChgHIO_c() { mNo = -1; } */
static void daSceneChgHIO_c_dt(daSceneChgHIO_c* h, s32 flags) {
    WWHD_FUNC(0x0246B23C, void, h, flags);
    if (h != nullptr) {
        h->__vtbl = HIO_VTBL;
        h->mNo = -1;
        if (flags & 1)
            operator_delete(h);
    }
}
VERIFY(0x0246B23C, daSceneChgHIO_c_dt);
