/**
 * d_bg_w_deform.cpp - Deformable background collision.
 * WWHD, written in GameCube decompilation style and verified against the original
 * (tools/verify, d_bg_w_deform).
 */
#include "d/d_bg_w_deform.h"

/* 024F49E8 */
BOOL dBgWDeform::Set(cBgD_t* bgd, u32 flag) {
    // HD: no J3DModel parameter, skin-deform setup, or model flag changes.
    if (cBgW_Set((dBgW*)this, bgd, MOVE_BG_e | NO_CALC_VTX_e | NO_VTX_TBL_e | GLOBAL_e, NULL)) {
        return TRUE;
    }
    SetFlag(flag);
    if (ChkNoCrrPos()) {
        return FALSE;
    }
    SetBackVtx((cXyz*)operator_new_array(pm_bgd->m_v_num * sizeof(cXyz)));
    return GetBackVtx() == NULL;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 024F4A78 */
static void __sinit_d_bg_w_deform_cpp() {
    sinit_header_statics_rodata(0x1046EDEC, 0x101D5378, 0x1046EDF8, 0x10044070);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
