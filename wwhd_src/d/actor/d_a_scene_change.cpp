/**
 * d_a_scene_change.cpp (WWHD)
 * Scene change marker (transform matrix only).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_scene_change.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_scene_change.h"

static daSceneChgHIO_c l_HIO;

/* 0246B020 */
static cPhs_State daSceneChgCreate(void* i_this) {
    d_a_scene_change_c* scnChg = static_cast<d_a_scene_change_c*>(i_this);

    fopAcM_ct(scnChg, d_a_scene_change_c);

    mDoMtx_stack_c::transS(scnChg->current.pos);
    mDoMtx_stack_c::ZXYrotM(scnChg->shape_angle);

    MTXCopy(mDoMtx_stack_c::get(), &scnChg->mTransformMtx);
    return cPhs_COMPLEATE_e;
}

/* 0246B0C0 */
static BOOL daSceneChgDelete(void* i_this) {
    return TRUE;
}

/* 0246B0C8 */
static BOOL daSceneChgExecute(void* i_this) {
    return TRUE;
}

/* 0246B0D0 */
static BOOL daSceneChgDraw(void* i_this) {
    return TRUE;
}

/* 0246B1E0 */
static BOOL daSceneChgIsDelete(void* i_this) {
    return TRUE;
}

/* 0246B0D8 */
// HD: GHS constructor entry allocates when called with NULL.
static daSceneChgHIO_c* daSceneChgHIO_c_ct(daSceneChgHIO_c* h) {
    if (h == nullptr) {
        h = (daSceneChgHIO_c*)operator_new(sizeof(daSceneChgHIO_c));
        if (h == nullptr)
            return h;
    }
    h->__vtbl = daSceneChgHIO_c::__vt;
    h->mNo = -1;
    h->m0005 = 0;
    h->m0008 = 100.0f;
    return h;
}

/* inlined into daSceneChgCreate (fopAcM_ct) */
d_a_scene_change_c::d_a_scene_change_c() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0246B134 */
// HD: header statics followed by HIO construction and destructor registration.
static void __sinit_d_a_scene_change_cpp() {
    sinit_header_statics(0x1046D888, 0x101D0080);
    daSceneChgHIO_c_ct(&l_HIO);
    __register_global_object(0x101D00A4);
}

/* 0246B1E8 */
// HD: actor deleting destructor.
static void d_a_scene_change_c_dt(d_a_scene_change_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0246B23C */
// HD: deleting entry for the HIO destructor.
static void daSceneChgHIO_c_dt(daSceneChgHIO_c* h, s32 flags) {
    if (h != nullptr) {
        h->__vtbl = daSceneChgHIO_c::__vt;
        h->mNo = -1;
        if (flags & 1)
            operator_delete(h);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_HIO 1046D8A4
*/
