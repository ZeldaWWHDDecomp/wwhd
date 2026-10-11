/* Wind Temple floor spikes. HD expands the actor prefix by 0x11C.
 * Field names, including unk470/unk474/unk485/unk486, follow the retail decompilation. */
#pragma once
#include "gc_api/gc_api.h"
struct daToge_c : fopAc_ac_c {
    enum EventState { WAIT, RETRACT_WAIT, RETRACT, EXTEND_START, EXTEND };
    BOOL _delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    BOOL _execute();
    void set_collision();
    void search_wind();
    void toge_move();
    void toge_seStart(u32 i_seNum);
    BOOL _draw();
    /* inlined into _execute, Create */
    void set_mtx() {
        J3DModel_setBaseScale(mpModel, &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx1);
    }

    /* 0x3AC */ request_of_phase_process_class m_Phs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ gptr<dBgW> mpBgW1;
    /* 0x528 */ Mtx34 mtx1;
    /* 0x558 */ gptr<dBgW> mpBgW2;
    /* 0x55C */ Mtx34 mtx2;
    /* 0x58C */ be<f32> unk470;
    /* 0x590 */ be<f32> unk474;
    /* 0x594 */ be<s32> mSwitchNo;
    /* 0x598 */ u8 pad47C[4];
    /* 0x59C */ be<u32> mWindTagId;
    /* 0x5A0 */ be<u8> mEventState;
    /* 0x5A1 */ be<u8> unk485;
    /* 0x5A2 */ be<u8> unk486;
    /* 0x5A3 */ u8 _5A3;
};
WWHD_OFFSET(daToge_c, m_Phs, 0x3AC);
WWHD_OFFSET(daToge_c, mpModel, 0x3B4);
WWHD_OFFSET(daToge_c, mStts, 0x3B8);
WWHD_OFFSET(daToge_c, mCyl, 0x3F4);
WWHD_OFFSET(daToge_c, mpBgW1, 0x524);
WWHD_OFFSET(daToge_c, mtx1, 0x528);
WWHD_OFFSET(daToge_c, mpBgW2, 0x558);
WWHD_OFFSET(daToge_c, mtx2, 0x55C);
WWHD_OFFSET(daToge_c, unk470, 0x58C);
WWHD_OFFSET(daToge_c, unk474, 0x590);
WWHD_OFFSET(daToge_c, mSwitchNo, 0x594);
WWHD_OFFSET(daToge_c, pad47C, 0x598);
WWHD_OFFSET(daToge_c, mWindTagId, 0x59C);
WWHD_OFFSET(daToge_c, mEventState, 0x5A0);
WWHD_OFFSET(daToge_c, unk485, 0x5A1);
WWHD_OFFSET(daToge_c, unk486, 0x5A2);
WWHD_SIZE(daToge_c, 0x5A4);
struct TogeJointData {
    u8 _00[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(TogeJointData, mJointNo, 4);
WWHD_SIZE(TogeJointData, 6);
struct TogeWindTag : fopAc_ac_c {
    u8 _3AC[0x6F0 - sizeof(fopAc_ac_c)];
    be<f32> mOffsY;
};
WWHD_OFFSET(TogeWindTag, mOffsY, 0x6F0);
WWHD_SIZE(TogeWindTag, 0x6F4);
struct TogeText {
    char bytes[1];
};
WWHD_SIZE(TogeText, 1);
enum { fpcNm_WindTag_e = 0x187, TOGE_AAB_VTBL = 0x100407B4, TOGE_VTBL = 0x100407C4 };
namespace daToge_prm {
inline u8 getSwitchNo(daToge_c *actor) { return fopAcM_GetParam(actor) & 0xFF; }
inline u8 getSeEnabled(daToge_c *actor) { return (fopAcM_GetParam(actor) >> 8) & 0xF; }
} // namespace daToge_prm
enum {
    dRes_INDEX_HTOGE1_BDL_HTOGE1_e = 4,
    dRes_INDEX_HTOGE1_DZB_HTOGE1A_e = 7,
    dRes_INDEX_HTOGE1_DZB_HTOGE1B_e = 8,
};
enum {
    JA_SE_OBJ_TOGETOGE_OUT = 0x7840,
    JA_SE_OBJ_TOGETOGE_MOVE = 0x7841,
    JA_SE_OBJ_TOGETOGE_IN = 0x7842,
};

/* 0200ECD4 toge_cLib_addCalc(f32* value, target, scale, maxStep, minStep) -> remaining distance */
static inline f32 toge_cLib_addCalc(be<f32> *v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    constexpr u32 callTarget = 0x0200ECD4;
    return gabi::call<f32>(callTarget, v, target, scale, maxStep, minStep);
}
/* 0207A9A0 cLib_calcTimer<u8> (the copy the WWHD linker kept) */
static inline u8 toge_cLib_calcTimer_u8(be<u8> *t) {
    constexpr u32 callTarget = 0x0207A9A0;
    return gabi::call<u8>(callTarget, t);
}
/* 025D50BC fopAc_ac_c::~fopAc_ac_c */
static inline void toge_fopAc_ac_c_dt(fopAc_ac_c *a, s32 flags) {
    constexpr u32 callTarget = 0x025D50BC;
    gabi::call(callTarget, a, flags);
}
/* toge_fopAcM_SearchByName (HD inline): the process name is passed by address to
 * fpcSch_JudgeForPName */
static inline fopAc_ac_c *toge_fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C /* fpcSch_JudgeForPName */, key.get());
}

namespace toge_bg {
enum { MOVE_BG_e = 1 };
}
