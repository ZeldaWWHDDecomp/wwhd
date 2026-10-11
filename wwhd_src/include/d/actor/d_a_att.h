// Puppet Ganon attention target. HD: GC actor fields 0x290 move to 0x3AC.
#pragma once
#include "gc_api/gc_api.h"

class att_class : public fopAc_ac_c {
public:
    att_class();
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ u8 m298[0x3D1 - 0x3B4];
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 m2B6[2];
    /* 0x3D4 */ dCcD_Stts mStts;
    /* 0x410 */ dCcD_Cyl mCyl;
    /* 0x540 */ dCcD_Sph mSph;
    /* 0x66C */ be<s8> m550;
    /* 0x66D */ u8 _66D[3];
};
WWHD_OFFSET(att_class, mStts, 0x3D4);
WWHD_OFFSET(att_class, mCyl, 0x410);
WWHD_OFFSET(att_class, mSph, 0x540);
WWHD_OFFSET(att_class, m550, 0x66C);
WWHD_SIZE(att_class, 0x670);


// HD: unit-local views of only the boss fields used by its attention targets.
struct bgn_att_move_s {
    u8 mPadding[0x2D0];
    /* 0x2D0 */ be<u8> m2D0;
    u8 mPadding2[0x2EC - 0x2D1];
    /* 0x2EC */ be<f32> m2EC;
    u8 mPadding3[0x300 - 0x2F0];
    /* 0x300 */ be<s16> m300;
    u8 mPadding4[2];
    /* 0x304 */ be<f32> m304;
    /* 0x308 */ be<s8> m308;
    u8 mPadding5[3];
};
WWHD_SIZE(bgn_att_move_s, 0x30C);
struct bgn_att_view_c : fopAc_ac_c {
    u8 mPadding[0x3D5 - sizeof(fopAc_ac_c)];
    /* 0x003D5 */ be<s8> m02B5;
    u8 mPadding2[0x12C20 - 0x3D6];
    /* 0x12C20 */ bgn_att_move_s mAAA8[8];
    u8 mPadding3[0x144B4 - 0x14480];
    /* 0x144B4 */ cXyz mC33C[8];
    u8 mPadding4[0x14EB4 - 0x14514];
    /* 0x14EB4 */ be<s16> mC748;
    u8 mPadding5[2];
    /* 0x14EB8 */ be<s16> mC74C;
    u8 mPadding6[0x14F18 - 0x14EBA];
    /* 0x14F18 */ be<s16> mC7AC[5];
};
WWHD_OFFSET(bgn_att_view_c, mAAA8, 0x12C20);
WWHD_OFFSET(bgn_att_view_c, mC33C, 0x144B4);
WWHD_OFFSET(bgn_att_view_c, mC7AC, 0x14F18);
void dCcD_Sph_construct(dCcD_Sph* sphere);
void dCcD_Sph_destruct(dCcD_Sph* sphere, s32 flags);

// HD: bridge the raw process-search callback API to a named candidate function.
inline void* fpcM_Search(void* (*callback)(void*, void*), void* data) {
    return fpcM_Search(gabi::guest_fn((const void*)callback), data);
}
enum { fpcNm_BGN_e = 0xF3 };
