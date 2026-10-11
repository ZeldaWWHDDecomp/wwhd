/**
 * HUD draw-list construction and destruction helpers from d_meter.cpp.
 * Verified against WWHD (unit d_meter_04).
 */
#include "d/d_meter_04.h"

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0259CF60 */
// HD: GHS allocates when the constructor receives NULL.
static dDlst_2Dm_c* dDlst_2Dm_c__ct(dDlst_2Dm_c* i_this) {
    if (i_this == NULL) {
        i_this = (dDlst_2Dm_c*)operator_new(sizeof(dDlst_2Dm_c));
    }
    if (i_this != NULL) {
        dDlst_base_c__ct(i_this);
        i_this->__vtbl = dDlst_2Dm_c::__vt;
    }
    return i_this;
}

/* 0259CFB4 */
static void dDlst_2Dm_c__dt(dDlst_2Dm_c* i_this, u32 flags) {
    if (i_this != NULL) {
        dDlst_base_c__dt(i_this, 0);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 0259D10C */
static void dDlst_2DMETER2_c__dt(dDlst_2DMETER2_c* i_this, u32 flags) {
    if (i_this != NULL) {
        dDlst_base_c__dt(i_this, 0);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 0259D160 */
static void dDlst_2DMETER1_c__dt(dDlst_2DMETER1_c* i_this, u32 flags) {
    if (i_this != NULL) {
        dDlst_base_c__dt(i_this, 0);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 0259D1B4 */
// HD: this TU's empty SafeString virtual method.
static void SafeString_assureTerminationImpl(SafeString* i_this) {
}

/* WWHD glue (tools/verify/autoglue.py): external draw-list base helpers
ext 0252CCBC dDlst_base_c* dDlst_base_c__ct(dDlst_base_c* i_this)
ext 0252CCFC void dDlst_base_c__dt(dDlst_base_c* i_this, u32 flags)
*/
