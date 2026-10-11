/**
 * d_a_dummy.cpp (WWHD)
 * Dummy actor (a matrix, no model)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dummy.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_dummy.h"

using daDummy::Act_c;

/* inlined into _create (fopAcM_ct) */
Act_c::Act_c() {
    // HD: GHS explicitly constructs the base and stores the virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 0212D6A8 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((Act_c*)i_this)->create_heap();
}

/* 0212D718 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    if (fopAcM_entrySolidHeap(this, solidHeapCB, 0)) {
        set_mtx();
        fopAcM_SetMtx(this, &mMtx);
        fopAcM_setCullSizeBox(this, -100.0f, -1000.0f, -100.0f, 100.0f, 100.0f, 100.0f);
    }
    return cPhs_COMPLEATE_e;
}

/* 0212D6B0 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* method table (HD: _create is a tail branch, the others are inlined) */
/* 0212D7D0 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 0212D7D4 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
/* 0212D7DC */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 0212D7E4 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}
/* 0212D8D4 */
static BOOL Mthd_IsDelete(void*) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0212D7EC */
static void __sinit_d_a_dummy_cpp() {
    sinit_header_statics(0x10463C5C, 0x101B4834);
}

/* 0212D880 */
// HD: compiler-generated deleting destructor, including the actor base.
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
