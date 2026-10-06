/**
 * d_a_obj_msdan2.cpp (WWHD)
 * Object - spawns a row of 16 Obj_MsdanSub2 steps and plays the "Msdan2" event on its switch.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_msdan2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x1002D828 /* HD: daObjMsdan2::Act_c vtable */
#define EVENT_NAME STR(0x1002D840) /* "Msdan2" */

enum { fpcNm_Obj_MsdanSub2_e = 0x51 };

namespace daObjMsdan2 {
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x00,
    };
    int prm_get_swSave();

    cPhs_State Mthd_Create();
    BOOL Mthd_Execute();
    BOOL Mthd_Delete() { return TRUE; }

    /* 0x3AC */ u8 field_0x290[0x8]; /* GameCube 0x290 */
    /* 0x3B4 */ be<s16> mEventIdx;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ be<s32> mMode;
};
WWHD_OFFSET(Act_c, mEventIdx, 0x3B4);
WWHD_OFFSET(Act_c, mMode, 0x3B8);
WWHD_SIZE(Act_c, 0x3BC);
}  // namespace daObjMsdan2
using daObjMsdan2::Act_c;

/* 0237B744: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0237B744, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0237B744, PrmAbstract);
int Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 0237B37C */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0237B37C, cPhs_State, this);
    /* fopAcM_ct(this, Act_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> angle;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    angle->x = current.angle.x;
    angle->y = current.angle.y;
    angle->z = current.angle.z;
    angle->y = (s16)(angle->y + 0x8000);
    pos->y = pos->y + 400.0f;
    for (int i = 0; i < 16; i++) {
        /* pos.x += 50.0f * cM_ssin(angle); pos.z += 50.0f * cM_scos(angle) (contracted to fmadds) */
        s16 a = current.angle.y;
        f32 x = gabi::fmadds(cM_ssin(a), 50.0f, pos->x);
        f32 z = gabi::fmadds(cM_scos(a), 50.0f, pos->z);
        pos->x = x;
        pos->z = z;
        fopAcM_create(fpcNm_Obj_MsdanSub2_e, (i << 8) + prm_get_swSave(), pos, current.roomNo, angle, nullptr, -1, 0);
    }

    mEventIdx = dComIfGp_evmng_getEventIdx(EVENT_NAME, 0xFF);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mMode = 3;
    } else {
        mMode = 0;
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0237B37C, &Act_c::Mthd_Create);

/* 0237B53C */
BOOL Act_c::Mthd_Execute() {
    WWHD_FUNC(0x0237B53C, BOOL, this);
    switch ((u32)(s32)mMode) {
    case 0:
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
            mMode = 1;
        }
        break;
    case 1:
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            mMode = 2;
        }
        break;
    case 2:
        if (dComIfGp_evmng_endCheck(mEventIdx)) {
            dComIfGp_event_reset();
            mMode = 3;
        }
        break;
    case 3:
        break;
    }
    return TRUE;
}
VERIFY(0x0237B53C, &Act_c::Mthd_Execute);

/* method table entries (HD: tail branches / inlined bodies) */
/* 0237B63C */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0237B63C, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0237B63C, Mthd_Create);
/* 0237B640 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0237B640, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0237B640, Mthd_Delete);
/* 0237B648 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0237B648, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Execute();
}
VERIFY(0x0237B648, Mthd_Execute);
/* 0237B734 */
static BOOL Mthd_Draw(void*) {
    WWHD_FUNC(0x0237B734, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0237B734, Mthd_Draw);
/* 0237B73C */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x0237B73C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0237B73C, Mthd_IsDelete);

/* 0237B64C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_obj_msdan2_cpp() {
    WWHD_FUNC(0x0237B64C, void, (u32)0);
    sinit_header_statics(0x1046B928, 0x101CBC84);
}
VERIFY(0x0237B64C, __sinit_d_a_obj_msdan2_cpp);

/* 0237B6E0: Act_c deleting destructor (compiler-generated, HD virtual destructor) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0237B6E0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0237B6E0, Act_c_dt);
