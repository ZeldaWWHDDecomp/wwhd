/* HUD draw-list compiler-generated helpers, WWHD layout.
 * dDlst_2Dm_c grows from GameCube 0x84 to HD 0xC4; only the base vtable
 * is accessed by this unit. The remaining members belong to d_drawlist. */
#pragma once
#include "bindings.h"

struct dDlst_base_c {
    /* 0x00 */ be<u32> __vtbl;
};
WWHD_SIZE(dDlst_base_c, 4);
WWHD_OFFSET(dDlst_base_c, __vtbl, 0);

struct dDlst_2Dm_c : dDlst_base_c {
    enum : u32 { __vt = 0x1004CBD8 };
    /* 0x04 */ u8 mDrawData[0xC4 - 4];
};
WWHD_SIZE(dDlst_2Dm_c, 0xC4);

struct dDlst_2DMETER1_c : dDlst_base_c {};
struct dDlst_2DMETER2_c : dDlst_base_c {};
WWHD_SIZE(dDlst_2DMETER1_c, 4);
WWHD_SIZE(dDlst_2DMETER2_c, 4);

dDlst_base_c* dDlst_base_c__ct(dDlst_base_c*);
void dDlst_base_c__dt(dDlst_base_c*, u32);
