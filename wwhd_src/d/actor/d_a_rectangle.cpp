/**
 * d_a_rectangle.cpp (WWHD)
 * Unused dummy actor.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_rectangle.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

struct RECTANGLE_class : fopAc_ac_c {};

/* 0245F364 */
static BOOL daRct_Draw(RECTANGLE_class* i_this) {
    WWHD_FUNC(0x0245F364, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0245F364, daRct_Draw);

/* 0245F36C */
static BOOL daRct_Execute(RECTANGLE_class* i_this) {
    WWHD_FUNC(0x0245F36C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0245F36C, daRct_Execute);

/* 0245F374 */
static BOOL daRct_IsDelete(RECTANGLE_class* i_this) {
    WWHD_FUNC(0x0245F374, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0245F374, daRct_IsDelete);

/* 0245F37C */
static BOOL daRct_Delete(RECTANGLE_class* i_this) {
    WWHD_FUNC(0x0245F37C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0245F37C, daRct_Delete);

/* 0245F384 */
static cPhs_State daRct_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0245F384, cPhs_State, i_this);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0245F384, daRct_Create);

/* 0245F38C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_rectangle_cpp() {
    WWHD_FUNC(0x0245F38C, void, (u32)0);
    sinit_header_statics(0x1046D750, 0x101CF6A8);
}
VERIFY(0x0245F38C, __sinit_d_a_rectangle_cpp);
