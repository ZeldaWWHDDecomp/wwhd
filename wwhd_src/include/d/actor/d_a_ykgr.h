/* Dragon Roost heat haze, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_path.h"
f32 PSVECSquareDistance(const cXyz*, const cXyz*);
void JPASetIndTexMtx(u32, s32, be<f32>*, s32);
enum { dSymbol_DIN_e = 1 };
namespace dPa_name { enum { ID_AK_SP_MTDRAGONKAGEROU = 0xC06D }; }
struct daYkgr_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ be<s32> mAlphaStep;  /* was m0C: daYkgrExecute 024EAAE8 (_execute inlined): m_alpha moves by it per frame toward 0 (stopped) or 0xFF (started); __sinit_d_a_ykgr_cpp 024EAD68 3 (tww the same) */
    /* 0x10 */ be<f32> m10;
    /* 0x14 */ be<f32> mStrengthLow;  /* was m14: daYkgrExecute 024EAAE8: setParam(rate * mStrengthHigh + (1 - rate) * it), the haze strength (the ind-matrix scale exponent and mantissa) at rate 0; __sinit_d_a_ykgr_cpp 024EAD68 -5 (tww the same) */
    /* 0x18 */ be<f32> mStrengthHigh;  /* was m18: daYkgrExecute 024EAAE8: the haze strength passed to setParam at rate 1 (rate = half mAimRate + half mPosRate); __sinit_d_a_ykgr_cpp 024EAD68 -3 (tww the same) */
    /* 0x1C */ be<f32> mAimRateTarget;  /* was m1C: daYkgrExecute 024EAAE8: m_aim_rate eases toward it, and mAimRate toward m_aim_rate; __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) */
    /* 0x20 */ be<f32> mPathDistMax;  /* was m20: getPosRate 024EA984: the player's xz distance to the nearest path point is clamped to [mPathDistMin, it]; the rate is 1 - (dist - min) / (it - min); __sinit_d_a_ykgr_cpp 024EAD68 1500 (tww the same) */
    /* 0x24 */ be<f32> mPathDistMin;  /* was m24: getPosRate 024EA984: the lower clamp of the path distance, where the position rate is 1; __sinit_d_a_ykgr_cpp 024EAD68 500 (tww the same) */
};

struct dPa_YkgrPcallBack {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> mIndMtx00;  /* was m04: YkgrPcallBack_draw 024EAD4C passes &it to JPASetIndTexMtx as the 2x3 indirect texture matrix (tww GXSetIndTexMtx(GX_ITM_0, &m04, m1C)); row 0 column 0, the mantissa setParam 024EA58C computes */
    /* 0x08 */ be<f32> mIndMtx01;  /* was m08: YkgrPcallBack_draw 024EAD4C: row 0 column 1 of the indirect texture matrix starting at mIndMtx00; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) */
    /* 0x0C */ be<f32> mIndMtx02;  /* was m0C: YkgrPcallBack_draw 024EAD4C: row 0 column 2 of the indirect texture matrix; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) */
    /* 0x10 */ be<f32> mIndMtx10;  /* was m10: YkgrPcallBack_draw 024EAD4C: row 1 column 0 of the indirect texture matrix; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) */
    /* 0x14 */ be<f32> mIndMtx11;  /* was m14: YkgrPcallBack_draw 024EAD4C: row 1 column 1 of the indirect texture matrix; setParam 024EA58C stores the same mantissa as mIndMtx00 (tww m04 = m14 = ..) */
    /* 0x18 */ be<f32> mIndMtx12;  /* was m18: YkgrPcallBack_draw 024EAD4C: row 1 column 2 of the indirect texture matrix; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) */
    /* 0x1C */ be<s8> mIndScaleExp;  /* was m1C: YkgrPcallBack_draw 024EAD4C passes it to JPASetIndTexMtx as the matrix's scale exponent (tww GXSetIndTexMtx scale_exp); setParam 024EA58C stores its parameter rounded down; __sinit_d_a_ykgr_cpp 024EAD68 1 (tww the same) */
    void setParam(f32 arg1);
};

struct daYkgr_c : fopAc_ac_c {
    daYkgr_c() { fopAc_ac_c_ct(this); __vtbl = 0x10043058; }
    f32 getPosRate();
    cPhs_State _create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ Mtx34 mMtx;
    /* 0x3E4 */ u8 m2C8[4];
    /* 0x3E8 */ be<f32> mAimRate;  /* was m2CC: daYkgrExecute 024EAAE8 eases it toward the global m_aim_rate and blends it half and half with mPosRate into the particle callback parameter (between l_HIO mStrengthLow and mStrengthHigh); _create 024EA6A0 sets 0.5 (tww the same) */
    /* 0x3EC */ be<f32> mPosRate;  /* was m2D0: daYkgrExecute 024EAAE8 eases it toward getPosRate() and blends it with mAimRate into the particle callback parameter; _create 024EA6A0 sets 0 (tww the same) */
};
WWHD_OFFSET(daYkgr_c, mPosRate, 0x3EC);
WWHD_SIZE(daYkgr_c, 0x3F0);

// HD: emitter fields at GameCube offsets minus four.
struct YkgrEmitter_view {
    u8 rest[0x1E8];
    gptr<dPa_YkgrPcallBack> mpParticleCallBack;
    u8 pad1EC[4];
    be<f32> mGlobalRot[3][3];
    u8 pad214[0x18];
    cXyz mGlobalTranslation;
    u8 pad238[0xF];
    be<u8> mGlobalAlpha;
    void setParticleCallBackPtr(dPa_YkgrPcallBack* cb) { mpParticleCallBack = cb; }
    void setGlobalAlpha(u8 alpha) { mGlobalAlpha = alpha; }
    void setGlobalRTMatrix(Mtx34* m) { gabi::call(0x028249B0, m, &mGlobalRot, &mGlobalTranslation); }
};
WWHD_OFFSET(YkgrEmitter_view, mGlobalRot, 0x1F0);
WWHD_OFFSET(YkgrEmitter_view, mGlobalTranslation, 0x22C);
WWHD_OFFSET(YkgrEmitter_view, mGlobalAlpha, 0x247);
struct YkgrCamera_view {
    u8 rest[0xDC];
    cXyz mEye;
    u8 padE8[0x234-0xE8];
    csXyz mAngle;
};
static bool SafeString_eq(const char* a, const char* b, u32 vtbl) {
    gabi::Local<SafeString> sa;
    sa->mTextPtr = gabi::guest_addr(a);
    sa->__vtbl = vtbl;
    gabi::Local<SafeString> sb;
    sb->mTextPtr = gabi::guest_addr(b);
    sb->__vtbl = vtbl;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pa = sa->mTextPtr;
    u32 pb = sb->mTextPtr;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}

