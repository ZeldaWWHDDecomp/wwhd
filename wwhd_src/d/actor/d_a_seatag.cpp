/**
 * d_a_seatag.cpp
 * Sea tag - marker actor without behaviour.
 *
 * WWHD (cking.rpx), written in the style of zeldaret/tww and verified function by
 * function (tools/verify, unit d_a_seatag).
 */
#include "d/actor/d_a_seatag.h"

/* inlined into daSeatag_Create (fopAcM_ct) */
daSeatag_c::daSeatag_c() {
    // HD: GHS constructs the actor base and stores the virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* inlined into daSeatag_Draw */
inline BOOL daSeatag_c::draw() {
    return TRUE;
}

/* inlined into daSeatag_Execute */
inline BOOL daSeatag_c::execute() {
    return TRUE;
}

/* inlined into daSeatag_Create */
inline cPhs_State daSeatag_c::create() {
    fopAcM_ct(this, daSeatag_c);
    return cPhs_COMPLEATE_e;
}

/* 0246FA88 */
static BOOL daSeatag_Draw(daSeatag_c* i_this) {
    return i_this->draw();
}

/* 0246FA90 */
static BOOL daSeatag_Execute(daSeatag_c* i_this) {
    return i_this->execute();
}

/* 0246F984 */
static BOOL daSeatag_IsDelete(daSeatag_c*) {
    return TRUE;
}

/* 0246F98C */
static BOOL daSeatag_Delete(daSeatag_c* i_this) {
    // HD: no debug delete registration or destructor call here.
    return TRUE;
}

/* 0246F994 */
static cPhs_State daSeatag_Create(fopAc_ac_c* i_this) {
    // HD: no debug create registration.
    return ((daSeatag_c*)i_this)->create();
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0246F9F4 */
static void __sinit_d_a_seatag_cpp() {
    sinit_header_statics(0x1046DC80, 0x101D01F4);
}

/* 0246FA98 */
// HD: compiler-generated deleting destructor, separate from daSeatag_Delete.
static void daSeatag_c_dt(daSeatag_c* i_this, s32 flags) {
    if (i_this != NULL) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
