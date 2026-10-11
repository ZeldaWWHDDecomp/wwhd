/* WWHD flame pillar. */
#pragma once
#include "gc_api/gc_api.h"
struct FlameEmitter;
namespace daObjFlame {
struct Act_c : fopAc_ac_c {
    // HD: offsets grow by 0x11C; tww names, including unnamed fields, are retained.
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    gptr<mDoExt_btkAnm> mpBtkAnm;
    gptr<mDoExt_brkAnm> mpBrkAnm;
    dCcD_Stts mStts;
    dCcD_Cps mCps;
    cXyz mCpsP0, mCpsP1;
    be<f32> mCpsRad;
    be<u8> m434;
    u8 m435[3];
    be<s32> mType, mModeProc;
    be<f32> mTimer, mHeight;
    gptr<FlameEmitter> mpEmitter0;
    gptr<FlameEmitter> mpEmitter1;
    gptr<FlameEmitter> mpEmitter2;
    be<u8> mEm0State, mEm1State, mEm2State, m457;
    be<s8> m458;
    be<u8> m459, m45A;
    u8 m45B;
    be<s32> m45C, m460, m464;
    be<s16> mRotY;
    u8 m46A[2];
    be<f32> m46C, mExtraScaleY;
    cXyz mOrigScale;
    u8 m480[0x80];
    s32 prm(s32 width, s32 shift) const;
    bool create_heap();
    cPhs_State _create();
    void create_mode_init();
    void set_mtx();
    void init_mtx();
    void em_position();
    void em_simple_set();
    void em_simple_inv();
    void em_manual_set();
    void em_manual_inv();
    void ki_make();
    void eff_hase();
    void mode_proc_call();
    // HD: the omen and before-mode retain the audio return value in r3.
    void set_switch();
    void ki_init();
    void mode_wait();
    u32 se_fireblast_omen();
    void mode_wait2();
    u32 mode_l_before();
    void mode_l_u();
    void mode_u();
    void mode_u_l();
    void mode_l_after();
};
WWHD_OFFSET(Act_c, mPhs, 0x3AC);
WWHD_OFFSET(Act_c, mCps, 0x3FC);
WWHD_OFFSET(Act_c, mType, 0x554);
WWHD_OFFSET(Act_c, mOrigScale, 0x590);
WWHD_SIZE(Act_c, 0x61C);
} // namespace daObjFlame
