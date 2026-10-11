/**
 * d_a_magma.cpp
 * Object - Magma floor registration
 *
 * WWHD (cking.rpx), written in the style of zeldaret/tww and verified function by function
 * against the recompiled original (tools/verify, unit d_a_magma).  Address markers and the glue block identify the verified functions/data.
 */
#include "d/actor/d_a_magma.h"

/* inlined into daMagma_Create (fopAcM_ct_Retail) */
daMagma_c::daMagma_c() {
    // HD: GHS explicitly constructs the actor base and stores the virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* inlined into daMagma_Create */
cPhs_State daMagma_c::create() {
    fopAcM_ct(this, daMagma_c);

    cPhs_State result = dComIfG_resLoad(&mPhs, "Magma");
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }

    if (dComIfGp_createMagma()) {
        dMagma_packet_c* magma = dComIfGp_getMagma();
        magma->newFloor(current.pos, scale, fopAcM_GetRoomNo(this), getPathNo());
    }

    return cPhs_ERROR_e;
}

/* 021B834C */
static BOOL daMagma_IsDelete(daMagma_c* i_this) {
    return TRUE;
}

/* 021B8354 */
static BOOL daMagma_Delete(daMagma_c* i_this) {
    // HD: destruction occurs through the virtual destructor rather than this callback.
    return TRUE;
}

/* 021B835C */
static cPhs_State daMagma_Create(fopAc_ac_c* i_this) {
    return ((daMagma_c*)i_this)->create();
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 021B841C */
static void __sinit_d_a_magma_cpp() {
    sinit_header_statics(0x10464EA4, 0x101B92BC);
}

/* 021B84B0 */
static void daMagma_c_dt(daMagma_c* i_this, s32 flags) {
    if (i_this != NULL) {
        i_this->__vtbl = daMagma_c::__vt;
        // HD: resDelete rather than GameCube's resDeleteDemo, followed by the base destructor.
        dComIfG_resDelete(&i_this->mPhs, "Magma");
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 1001436C "Magma" in 021B835C
str 10014374 "Magma" in 021B84B0
*/
