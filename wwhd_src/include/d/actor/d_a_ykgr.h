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
    /* 0x0C */ be<s32> m0C;
    /* 0x10 */ be<f32> m10;
    /* 0x14 */ be<f32> m14;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<f32> m1C;
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
};

struct dPa_YkgrPcallBack {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> m04;
    /* 0x08 */ be<f32> m08;
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ be<f32> m10;
    /* 0x14 */ be<f32> m14;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<s8> m1C;
    void setParam(f32 arg1);
};

struct daYkgr_c : fopAc_ac_c {
    daYkgr_c() { fopAc_ac_c_ct(this); __vtbl = 0x10043058; }
    f32 getPosRate();
    cPhs_State _create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ Mtx34 mMtx;
    /* 0x3E4 */ u8 m2C8[4];
    /* 0x3E8 */ be<f32> m2CC;
    /* 0x3EC */ be<f32> m2D0;
};
WWHD_OFFSET(daYkgr_c, m2D0, 0x3EC);
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
    sa->mStringTop = gabi::guest_addr(a);
    sa->__vtbl = vtbl;
    gabi::Local<SafeString> sb;
    sb->mStringTop = gabi::guest_addr(b);
    sb->__vtbl = vtbl;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pa = sa->mStringTop;
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}

