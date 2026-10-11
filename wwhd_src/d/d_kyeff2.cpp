/**
 * d_kyeff2.cpp
 * Environment - Volumetric-cloud drawing and movement
 *
 * WWHD (cking.rpx), written in the style of zeldaret/tww and verified function by function
 * against the recompiled original (tools/verify, unit d_kyeff2).  Address markers identify the verified functions.
 */
#include "d/d_kyeff2.h"
#include "bindings.h"
#include "gc_api/f_pc/f_pc_base.h"
#include "gc_api/d/d_kankyo_wether.h"

/* 02586BC0 */
static u32 dKyeff2_Draw(dKyeff2_c* i_this) {
    dKyw_wether_draw2();
    return TRUE;
}

/* 02586BE4 */
u32 dKyeff2_c::execute() {
    dKyw_wether_move_draw2();
    return TRUE;
}

/* 02586C08 */
static u32 dKyeff2_Execute(dKyeff2_c* i_this) {
    return i_this->execute();
}

/* 02586C0C */
static u32 dKyeff2_IsDelete(dKyeff2_c* i_this) {
    return TRUE;
}

/* 02586C14 */
static u32 dKyeff2_Delete(dKyeff2_c* i_this) {
    // HD: GHS emits the non-deleting base process destructor before weather cleanup.
    if (i_this != NULL) {
        __dt(&i_this->base);
    }
    dKyw_wether_delete2();
    return TRUE;
}

/* 02586C48 */
static u32 dKyeff2_Create(kankyo_class* i_this) {
    // HD: GHS explicitly constructs the base and installs the derived virtual destructor table.
    if (i_this != NULL) {
        __ct(&i_this->base);
        i_this->base.__vtbl = dKyeff2_c::__vt;
    }
    dKyw_wether_init2();
    return cPhs_COMPLEATE_e;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02586C90 */
static void __sinit_d_kyeff2_cpp() {
    sinit_header_statics(0x104775F4, 0x101E9B44);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
