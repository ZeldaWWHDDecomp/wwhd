/**
 * d_a_seatag.cpp (WWHD)
 * Sea tag (marker actor without behaviour).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_seatag.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SEATAG_VTBL 0x10039F6C /* daSeatag_c vtable (HD virtual destructor) */

struct daSeatag_c : fopAc_ac_c {};

/* 0246FA88: draw() inlined */
static BOOL daSeatag_Draw(daSeatag_c* i_this) {
    WWHD_FUNC(0x0246FA88, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246FA88, daSeatag_Draw);

/* 0246FA90: execute() inlined */
static BOOL daSeatag_Execute(daSeatag_c* i_this) {
    WWHD_FUNC(0x0246FA90, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246FA90, daSeatag_Execute);

/* 0246F984 */
static BOOL daSeatag_IsDelete(daSeatag_c*) {
    WWHD_FUNC(0x0246F984, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0246F984, daSeatag_IsDelete);

/* 0246F98C: HD: no fopAcM_RegisterDeleteID (debug only) */
static BOOL daSeatag_Delete(daSeatag_c* i_this) {
    WWHD_FUNC(0x0246F98C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0246F98C, daSeatag_Delete);

/* 0246F994: HD: no fopAcM_RegisterCreateID */
static cPhs_State daSeatag_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0246F994, cPhs_State, i_this);
    /* create(): fopAcM_ct(this, daSeatag_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = SEATAG_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0246F994, daSeatag_Create);

/* 0246F9F4: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_seatag_cpp() {
    WWHD_FUNC(0x0246F9F4, void, (u32)0);
    sinit_header_statics(0x1046DC80, 0x101D01F4);
}
VERIFY(0x0246F9F4, __sinit_d_a_seatag_cpp);

/* 0246FA98: daSeatag_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daSeatag_c_dt(daSeatag_c* i_this, s32 flags) {
    WWHD_FUNC(0x0246FA98, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0246FA98, daSeatag_c_dt);
