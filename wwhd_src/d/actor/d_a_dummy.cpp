/**
 * d_a_dummy.cpp (WWHD)
 * Dummy actor (a matrix, no model)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dummy.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x1000E228 /* HD: daDummy::Act_c vtable */

namespace daDummy {
struct Act_c : fopAc_ac_c {
    bool create_heap() { return true; }
    cPhs_State _create();
    bool _delete() { return true; }
    void set_mtx();
    bool _execute() { return true; }
    bool _draw() { return true; }

    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ Mtx34 mMtx;
};
WWHD_OFFSET(Act_c, mMtx, 0x3B4);
WWHD_SIZE(Act_c, 0x3E4);
}  // namespace daDummy
using daDummy::Act_c;

/* 0212D6A8: solidHeapCB (create_heap inlined) */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0212D6A8, BOOL, (u32)0);
    return ((Act_c*)i_this)->create_heap();
}
VERIFY(0x0212D6A8, solidHeapCB);

/* 0212D718 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x0212D718, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    if (fopAcM_entrySolidHeap(this, 0x0212D6A8 /* solidHeapCB */, 0)) {
        set_mtx();
        cullMtx = gabi::ea(&mMtx); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(this, -100.0f, -1000.0f, -100.0f, 100.0f, 100.0f, 100.0f);
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0212D718, &Act_c::_create);

/* 0212D6B0 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0212D6B0, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
VERIFY(0x0212D6B0, &Act_c::set_mtx);

/* method table (HD: _create is a tail branch, the others are inlined) */
/* 0212D7D0 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0212D7D0, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x0212D7D0, Mthd_Create);
/* 0212D7D4 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0212D7D4, BOOL, (u32)0);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x0212D7D4, Mthd_Delete);
/* 0212D7DC */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0212D7DC, BOOL, (u32)0);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x0212D7DC, Mthd_Execute);
/* 0212D7E4 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0212D7E4, BOOL, (u32)0);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x0212D7E4, Mthd_Draw);
/* 0212D8D4 */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x0212D8D4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0212D8D4, Mthd_IsDelete);

/* 0212D7EC */
static void __sinit_d_a_dummy_cpp() {
    WWHD_FUNC(0x0212D7EC, void, (u32)0);
    sinit_header_statics(0x10463C5C, 0x101B4834);
}
VERIFY(0x0212D7EC, __sinit_d_a_dummy_cpp);

/* 0212D880: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x0212D880, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0212D880, Act_c_dt);
