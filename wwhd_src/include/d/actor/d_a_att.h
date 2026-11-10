// Puppet Ganon attention target. HD: GC actor fields 0x290 move to 0x3AC.
#pragma once
#include "gc_api/gc_api.h"

class att_class : public fopAc_ac_c {
public:
    att_class();
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ u8 m298[0x3D1 - 0x3B4];
    /* 0x3D1 */ be<u8> mStringNo;  /* was m2B5: daAtt_Create 02058B38: parameter byte 0; below 10 it is the index of Puppet Ganon's string (daAtt_Execute 020587B4 follows bgn mHimoTopPos[it] and hits mMove[it]); 100 = a plain attention point, 101 = the tongue sphere (sita_sph_src) */
    /* 0x3D2 */ u8 m2B6[2];
    /* 0x3D4 */ dCcD_Stts mStts;
    /* 0x410 */ dCcD_Cyl mCyl;
    /* 0x540 */ dCcD_Sph mSph;
    /* 0x66C */ be<s8> mHitWaitTimer;  /* was m550: daAtt_Execute 020587B4 sets 10 after a hit on the string and counts it down; no new hit is taken until it reaches 0 */
    /* 0x66D */ u8 _66D[3];
};
WWHD_OFFSET(att_class, mStts, 0x3D4);
WWHD_OFFSET(att_class, mCyl, 0x410);
WWHD_OFFSET(att_class, mSph, 0x540);
WWHD_OFFSET(att_class, mHitWaitTimer, 0x66C);
WWHD_SIZE(att_class, 0x670);


// HD: unit-local views of only the boss fields used by its attention targets.
struct bgn_att_move_s {
    u8 mPadding[0x2D0];
    /* 0x2D0 */ be<u8> mCut; /* was m2D0: move_s mCut (bgn, d_a_bgn.h) */
    u8 mPadding2[0x2EC - 0x2D1];
    /* 0x2EC */ be<f32> mSwayAmplitude; /* was m2EC: move_s mSwayAmplitude (bgn, d_a_bgn.h) */
    u8 mPadding3[0x300 - 0x2F0];
    /* 0x300 */ be<s16> mPluckTimer; /* was m300: move_s mPluckTimer (bgn, d_a_bgn.h) */
    u8 mPadding4[2];
    /* 0x304 */ be<f32> mCutSwayAmplitude; /* was m304: move_s mCutSwayAmplitude (bgn, d_a_bgn.h) */
    /* 0x308 */ be<s8> mHitPoints; /* was m308: move_s mHitPoints (bgn, d_a_bgn.h) */
    u8 mPadding5[3];
};
WWHD_SIZE(bgn_att_move_s, 0x30C);
struct bgn_att_view_c : fopAc_ac_c {
    u8 mPadding[0x3D5 - sizeof(fopAc_ac_c)];
    /* 0x003D5 */ be<s8> mForm;
    u8 mPadding2[0x12C20 - 0x3D6];
    /* 0x12C20 */ bgn_att_move_s mMove[8];
    u8 mPadding3[0x144B4 - 0x14480];
    /* 0x144B4 */ cXyz mHimoTopPos[8];
    u8 mPadding4[0x14EB4 - 0x14514];
    /* 0x14EB4 */ be<s16> mAction;
    u8 mPadding5[2];
    /* 0x14EB8 */ be<s16> mDanceType;
    u8 mPadding6[0x14F18 - 0x14EBA];
    /* 0x14F18 */ be<s16> mTimers[5];
};
WWHD_OFFSET(bgn_att_view_c, mMove, 0x12C20);
WWHD_OFFSET(bgn_att_view_c, mHimoTopPos, 0x144B4);
WWHD_OFFSET(bgn_att_view_c, mTimers, 0x14F18);
void dCcD_Sph_construct(dCcD_Sph* sphere);
void dCcD_Sph_destruct(dCcD_Sph* sphere, s32 flags);

// HD: bridge the raw process-search callback API to a named candidate function.
inline void* fpcM_Search(void* (*callback)(void*, void*), void* data) {
    return fpcM_Search(gabi::guest_fn((const void*)callback), data);
}
enum { fpcNm_BGN_e = 0xF3 };
