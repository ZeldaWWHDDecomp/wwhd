/**
 * d_a_rectangle.cpp (WWHD)
 * Unused dummy actor.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_rectangle.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_rectangle.h"

/* 0245F364 */
static BOOL daRct_Draw(RECTANGLE_class* i_this) {
    return TRUE;
}

/* 0245F36C */
static BOOL daRct_Execute(RECTANGLE_class* i_this) {
    return TRUE;
}

/* 0245F374 */
static BOOL daRct_IsDelete(RECTANGLE_class* i_this) {
    return TRUE;
}

/* 0245F37C */
static BOOL daRct_Delete(RECTANGLE_class* i_this) {
    return TRUE;
}

/* 0245F384 */
static cPhs_State daRct_Create(fopAc_ac_c* i_this) {
    return cPhs_COMPLEATE_e;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0245F38C */
// HD: static initialisation of the common header statics
static void __sinit_d_a_rectangle_cpp() {
    sinit_header_statics(0x1046D750, 0x101CF6A8);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
