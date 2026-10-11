/**
 * d_a_obj_msdan2.cpp (WWHD)
 * Object - spawns a row of 16 Obj_MsdanSub2 steps and plays the "Msdan2" event on its switch.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_msdan2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_msdan2.h"

using daObjMsdan2::Act_c;

enum { fpcNm_Obj_MsdanSub2_e = 0x51 };

/* 0237B744 */
// HD: this TU keeps daObj::PrmAbstract out of line.
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
int Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* inlined into Mthd_Create (fopAcM_ct) */
Act_c::Act_c() {
    // HD: GHS constructs the base and installs the derived vtable.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* 0237B37C */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);

    cXyz pos = current.pos;
    csXyz angle;
    angle.x = current.angle.x;
    angle.y = current.angle.y;
    angle.z = current.angle.z;
    angle.y = (s16)(angle.y + 0x8000);
    pos.y = pos.y + 400.0f;
    for (int i = 0; i < 16; i++) {
        // HD: both results are calculated before either coordinate is stored.
        s16 a = current.angle.y;
        f32 x = fmadds(cM_ssin(a), 50.0f, pos.x);
        f32 z = fmadds(cM_scos(a), 50.0f, pos.z);
        pos.x = x;
        pos.z = z;
        fopAcM_create(fpcNm_Obj_MsdanSub2_e, (i << 8) + prm_get_swSave(), &pos, current.roomNo, &angle, nullptr, -1, 0);
    }

    mEventIdx = dComIfGp_evmng_getEventIdx("Msdan2", 0xFF);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mMode = MODE_END_e;
    } else {
        mMode = MODE_WAIT_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 0237B53C */
BOOL Act_c::Mthd_Execute() {
    switch (mMode) {
    case MODE_WAIT_e:
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
            mMode = MODE_ORDER_e;
        }
        break;
    case MODE_ORDER_e:
        if (eventInfo.checkCommandDemoAccrpt()) {
            mMode = MODE_EVENT_e;
        }
        break;
    case MODE_EVENT_e:
        if (dComIfGp_evmng_endCheck(mEventIdx)) {
            dComIfGp_event_reset();
            mMode = MODE_END_e;
        }
        break;
    case MODE_END_e:
        break;
    }
    return TRUE;
}

/* method table entries (HD: tail branches / inlined bodies) */
/* 0237B63C */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0237B640 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0237B648 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Execute();
}
/* 0237B734 */
static BOOL Mthd_Draw(void*) {
    return TRUE;
}
/* 0237B73C */
static BOOL Mthd_IsDelete(void*) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0237B64C */
static void __sinit_d_a_obj_msdan2_cpp() {
    sinit_header_statics(0x1046B928, 0x101CBC84);
}

/* 0237B6E0 */
// HD: deleting destructor, with the actor base destroyed before the allocation.
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 1002D840 # "Msdan2"
*/
