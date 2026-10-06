/**
 * d_a_obj_msdan.cpp (WWHD)
 * Object - Msdan: spawns a staircase of step actors (Obj_MsdanSub) and plays an event when its
 * switch is set.
 *
 * The GameCube decompilation of this unit is only
 * "Nonmatching" placeholders, so the three methods are written from the WWHD code (cking.rpx)
 * and verified against it; names follow the GameCube header.
 */
#include "bindings.h"

#define M_arcname STR(0x1002D820)
#define ACT_VTBL 0x1002D7F0 /* HD: daObjMsdan::Act_c vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1988 HD: mDoAud_seStart(id) without position (as in d_a_mo2.h) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }

enum { PROC_Obj_MsdanSub = 0x4F };

namespace daObjMsdan {
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0,
        PRM_SIZE_W = 1, PRM_SIZE_S = 0x10,
        PRM_SOUND_W = 1, PRM_SOUND_S = 0x12,
        PRM_EVID_W = 8, PRM_EVID_S = 0x18,
    };
    u32 prm_get_swSave();
    u32 prm_get_size();
    u32 prm_get_sound();
    u32 prm_get_evId();
    cPhs_State Mthd_Create();
    BOOL Mthd_Execute();
    BOOL Mthd_Delete();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> mEventIdx;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ be<u32> mState;
};
WWHD_OFFSET(Act_c, mState, 0x3B8);
WWHD_SIZE(Act_c, 0x3BC);
}  // namespace daObjMsdan
using daObjMsdan::Act_c;

/* 0237B360: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0237B360, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0237B360, PrmAbstract);
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u32 Act_c::prm_get_size() { return PrmAbstract(this, PRM_SIZE_W, PRM_SIZE_S); }
u32 Act_c::prm_get_sound() { return PrmAbstract(this, PRM_SOUND_W, PRM_SOUND_S); }
u32 Act_c::prm_get_evId() { return PrmAbstract(this, PRM_EVID_W, PRM_EVID_S); }

/* 0237AD28: new (GameCube "Nonmatching") */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0237AD28, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        /* the steps go up in a spiral of radius 50 around the actor */
        s16 angY = current.angle.y;
        gabi::Local<cXyz> pos;
        gabi::Local<csXyz> angle;
        angle->x = current.angle.x;
        pos->x = current.pos.x;
        angle->y = (s16)(angY - 0x8000);
        f32 y = current.pos.y;
        pos->y = y;
        angle->z = current.angle.z;
        pos->z = current.pos.z;
        s32 count;
        u32 sub;
        if (prm_get_size() != 0) {
            pos->y = y + 800.0f;
            count = 31;
            sub = 0;
        } else {
            pos->y = y + 400.0f;
            count = 15;
            sub = 0x1000;
        }
        for (;;) {
            pos->x = gabi::fmadds(cM_ssin(angY), 50.0f, pos->x);
            pos->z = gabi::fmadds(cM_scos(angY), 50.0f, pos->z);
            u32 sw = prm_get_swSave();
            u32 size = prm_get_size();
            fopAcM_create(PROC_Obj_MsdanSub, sw + sub + (size << 16), pos, current.roomNo, angle, nullptr, -1, 0);
            sub += 0x100;
            if (--count == 0)
                break;
            angY = current.angle.y;
        }
        u8 evId = (u8)prm_get_evId();
        if (evId == 0xFF) {
            mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1002D80C), 0xFF);
        } else {
            mEventIdx = dComIfGp_evmng_getEventIdx(nullptr, evId);
        }
        mState = fopAcM_isSwitch(this, prm_get_swSave()) ? 3 : 0;
    }
    return phase_state;
}
VERIFY(0x0237AD28, &Act_c::Mthd_Create);

/* 0237B0C8: new (GameCube "Nonmatching") */
BOOL Act_c::Mthd_Execute() {
    WWHD_FUNC(0x0237B0C8, BOOL, this);
    switch ((u32)mState) {
    case 0:
        if (fopAcM_isSwitch(this, prm_get_swSave())) {
            if (prm_get_size() != 0 || (u8)prm_get_sound() != 0) {
                mState = 3;
            } else if (mEventIdx == -1) {
                mDoAud_seStart_1(0x806); /* JA_SE_READ_RIDDLE_1 */
                mState = 3;
            } else {
                fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
                mState = 1;
            }
        }
        break;
    case 1:
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            mState = 2;
            mDoAud_seStart_1(0x806);
        } else {
            fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
        }
        break;
    case 2:
        if (dComIfGp_evmng_endCheck(mEventIdx)) {
            dComIfGp_event_reset();
            mState = 3;
        }
        break;
    }
    return TRUE;
}
VERIFY(0x0237B0C8, &Act_c::Mthd_Execute);

/* 0237B098: new (GameCube "Nonmatching") */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0237B098, BOOL, this);
    dComIfG_resDelete(&mPhs, M_arcname);
    return TRUE;
}
VERIFY(0x0237B098, &Act_c::Mthd_Delete);

/* method table entries (HD: tail branches) */
/* 0237B25C */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0237B25C, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0237B25C, Mthd_Create);
/* 0237B260 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0237B260, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0237B260, Mthd_Delete);
/* 0237B264 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0237B264, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Execute();
}
VERIFY(0x0237B264, Mthd_Execute);

/* 0237B268 */
static void __sinit_d_a_obj_msdan_cpp() {
    WWHD_FUNC(0x0237B268, void, (u32)0);
    sinit_header_statics(0x1046B90C, 0x101CBC10);
}
VERIFY(0x0237B268, __sinit_d_a_obj_msdan_cpp);

/* 0237B2FC: daObjMsdan::Act_c deleting destructor */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0237B2FC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0237B2FC, Act_c_dt);

/* 0237B350 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0237B350, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0237B350, Mthd_Draw);
/* 0237B358 */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0237B358, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0237B358, Mthd_IsDelete);
