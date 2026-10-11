/**
 * d_a_spotbox.cpp (WWHD)
 * Spot box (a box-shaped volume for the GameCube's spot-light model).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_spotbox.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_spotbox.h"

/* 0248AA64 */
// HD: the spot-light model is not drawn.
static BOOL daSpotbox_Draw(daSpotbox_c* i_this) {
    return TRUE;
}

/* 0248A7F0 */
static BOOL daSpotbox_Execute(daSpotbox_c* i_this) {
    return i_this->execute();
}

/* inlined into daSpotbox_Execute */
BOOL daSpotbox_c::execute() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    return TRUE;
}

/* 0248A864 */
static BOOL daSpotbox_IsDelete(daSpotbox_c* i_this) {
    return TRUE;
}

/* 0248A86C */
// HD: no destructor call.
static BOOL daSpotbox_Delete(daSpotbox_c* i_this) {
    return true;
}

/* 0248A874 */
static cPhs_State daSpotbox_Create(fopAc_ac_c* i_this) {
    return ((daSpotbox_c*)i_this)->create();
}

/* inlined into daSpotbox_Create */
cPhs_State daSpotbox_c::create() {
    fopAcM_ct(this, daSpotbox_c);

    f32 baseScale = getType() != 0 ? 1000.0f : 100.0f;
    scale.x *= baseScale;
    scale.y *= baseScale;
    current.pos.y = fmadds(0.5f, scale.y, current.pos.y); // HD: position is updated before scaling z; GHS fuses the multiply/add.
    scale.z *= (baseScale * 1.2f);
    fopAcM_SetMtx(this, &mMtx);
    fopAcM_setCullSizeBox(this, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
    return cPhs_COMPLEATE_e;
}

/* inlined into daSpotbox_Create (fopAcM_ct) */
daSpotbox_c::daSpotbox_c() {
    // HD: explicit base construction and the virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0248A9D0 */
static void __sinit_d_a_spotbox_cpp() {
    sinit_header_statics(0x1046DF38, 0x101D0B38);
}

/* 0248AA6C */
// HD: compiler-generated deleting destructor.
static void daSpotbox_c_dt(daSpotbox_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
