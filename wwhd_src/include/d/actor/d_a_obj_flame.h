/* WWHD flame pillar. */
#pragma once
#include "gc_api/gc_api.h"
struct FlameEmitter;
namespace daObjFlame {
struct Act_c : fopAc_ac_c {
    // HD: offsets grow by 0x11C; tww names are kept, its offset names only in the was-comments.
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_btkAnm> mpBtkAnm;
    /* 0x3BC */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x3C0 */ dCcD_Stts mStts;
    /* 0x3FC */ dCcD_Cps mCps;
    /* 0x534 */ cXyz mCpsP0;
    /* 0x540 */ cXyz mCpsP1;
    /* 0x54C */ be<f32> mCpsRad;
    /* 0x550 */ be<u8> mbCpsActive;  /* was m434: em_position 02343D7C: set when the flame is tall enough (1500 * mHeight - 300 > 0) and only then places mCpsP1; mode_proc_call 02344DF4 registers mCps only while set; _create 0234521C 0 */
    /* 0x551 */ u8 m435[3];
    /* 0x554 */ be<s32> mType;
    /* 0x558 */ be<s32> mModeProc;
    /* 0x55C */ be<f32> mTimer;
    /* 0x560 */ be<f32> mHeight;
    /* 0x564 */ gptr<FlameEmitter> mpEmitter0;
    /* 0x568 */ gptr<FlameEmitter> mpEmitter1;
    /* 0x56C */ gptr<FlameEmitter> mpEmitter2;
    /* 0x570 */ be<u8> mEm0State;
    /* 0x571 */ be<u8> mEm1State;
    /* 0x572 */ be<u8> mEm2State;
    /* 0x573 */ be<u8> mbEmPosUpdate;  /* was m457: em_position 02343D7C returns at once unless set; create_mode_init 0234365C / mode_wait 02344A8C / mode_l_u 02344CA4 / mode_u_l 02344D28 set it, mode_l_before 02344C50 clears it */
    /* 0x574 */ be<s8> mReverb;  /* was m458: _create 0234521C: dComIfGp_getReverb(home.roomNo); mode_l_before 02344C50 / mode_proc_call 02344DF4 pass it to mDoAud_seStart (as daObjMovebox::Act_c::mReverb) */
    /* 0x575 */ be<u8> mbLiftingObj;  /* was m459: liftup_magmarock 023443A0 / liftup_mflft 023446B0 set it when they lift an object this frame (cleared before the search, 023455E0); em_simple_set 02344014 / em_manual_set 023432B4 / em_manual_inv 02344230 hold the emitter back while set; _create 0234521C 0 */
    /* 0x576 */ be<u8> mbKiMake;  /* was m45A: ki_init 02344A30 sets it when the prm Keese count is not 32; ki_make 02345068 runs only while set and clears it when the count is reached or a delay starts */
    /* 0x577 */ u8 m45B;
    /* 0x578 */ be<s32> mKiDelay;  /* was m45C: _create 0234521C: random delay from prm(2, 13) (at least 1 outside MODE_WAIT); ki_make 02345068 counts it down before spawning and reloads it from prm(2, 13) after the last Keese */
    /* 0x57C */ be<s32> mKiTimer;  /* was m460: ki_init 02344A30 0; ki_make 02345068 counts it down and spawns a Keese (fpcNm_KI_e) every 5 frames */
    /* 0x580 */ be<s32> mKiCount;  /* was m464: ki_init 02344A30 0; ki_make 02345068: Keese spawned so far, compared with the prm count (prm(5, 8) + 1) */
    /* 0x584 */ be<s16> mRotY;
    /* 0x586 */ u8 m46A[2];
    /* 0x588 */ be<f32> mHeightRatio;  /* was m46C: em_position 02343D7C / liftup_magmarock 023443A0 / liftup_mflft 023446B0: factor on the flame height (1500 * mHeight * attr scale * it); liftup_mflft eases it toward 1; _create 0234521C 1 */
    /* 0x58C */ be<f32> mExtraScaleY;
    /* 0x590 */ cXyz mOrigScale;
    /* 0x59C */ u8 m480[0x80];
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
