/** Small red flag: GameCube actor tail 0x290 begins at WWHD 0x3AC. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/gc_api.h"
// HD: only the cloth virtual slots used by the flag are represented.
struct ToriFlagClothVtableHD {
    u8 _0[0xC];
    be<u32> mDelete;
    u8 _10[0x3C - 0x10];
    be<u32> mMove;
    u8 _40[4];
    be<u32> mDraw;
};
struct ToriFlagClothHD {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ gptr<ToriFlagClothVtableHD> __vtbl;
    /* 0x010 */ u8 _010[0xE4 - 0x10];
    /* 0x0E4 */ be<f32> mWindSpeed;     /* GameCube 0x5C */
    /* 0x0E8 */ be<f32> mWindSpeedWave; /* GameCube 0x60 */
    /* 0x0EC */ u8 _0EC[0x1A0 - 0xEC];
    /* 0x1A0 */ be<f32> mSpring; /* GameCube 0xD8 */
    /* 0x1A4 */ be<f32> mGravity;
    /* 0x1A8 */ be<f32> mDrag;
    /* 0x1AC */ be<f32> mFlyFlex;
    /* 0x1B0 */ be<f32> mHoistFlex;
    /* 0x1B4 */ be<s16> mWave;
    /* 0x1B6 */ be<s16> mWaveSpeed;
    /* 0x1B8 */ be<s16> field_0xF0;
    /* 0x1BA */ be<s16> field_0xF2;
    /* 0x1BC */ be<s16> mRipple;
    /* 0x1BE */ be<s16> mRotateY;

    void setWindPower(f32 wind, f32 windWave) {
        mWindSpeed = wind;
        mWindSpeedWave = windWave;
    }
    void setParam(f32 spring, f32 grav, f32 drag, f32 flyFlex, f32 hoistFlex, s16 wave, s16 param_1,
                  s16 ripple, s16 rotate, f32 wind, f32 windWave) {
        mSpring = spring;
        mGravity = grav;
        mDrag = drag;
        mFlyFlex = flyFlex;
        mHoistFlex = hoistFlex;
        mWaveSpeed = wave;
        field_0xF2 = param_1;
        mRipple = ripple;
        mRotateY = rotate;
        setWindPower(wind, windWave);
    }
    /* 0251B638 dCloth_packet_c::setMtx(Mtx) */
    void setMtx(Mtx34 *);
    /* 0251E7A8 dCloth_packet_c::setGlobalWind(cXyz*) */
    void setGlobalWind(cXyz *);
    /* virtuals (HD vtable at +0xC): deleting destructor +0xC, cloth_move +0x3C, cloth_draw +0x44 */
    void destroy(s32 flags) {
        gabi::call_ptr(__vtbl->mDelete, this, flags);
    }
    void cloth_move() {
        gabi::call_ptr(__vtbl->mMove, this);
    }
    void cloth_draw() {
        gabi::call_ptr(__vtbl->mDraw, this);
    }
};

// HD: GHS copies three 0x44-byte light blocks using mixed scalar widths.
struct ToriFlagLightHD {
    be<f32> mVectors[6];
    be<u8> mColor[4];
    be<s16> mAttenuation[4];
    be<f32> mParameters[8];
};
WWHD_SIZE(ToriFlagLightHD, 0x44);
struct ToriFlagTevCtorHD {
    ToriFlagLightHD mLight0;
    u8 _44[0xC0 - 0x44];
    ToriFlagLightHD mLight1;
    u8 _104[0x144 - 0x104];
    ToriFlagLightHD mLight2;
};
inline void ToriFlag_copyLight(ToriFlagLightHD *dst, const ToriFlagLightHD *src) {
    for (int i = 0; i < 6; ++i)
        dst->mVectors[i] = src->mVectors[i];
    for (int i = 0; i < 4; ++i)
        dst->mColor[i] = src->mColor[i];
    for (int i = 0; i < 4; ++i)
        dst->mAttenuation[i] = src->mAttenuation[i];
    for (int i = 0; i < 8; ++i)
        dst->mParameters[i] = src->mParameters[i];
}
struct daTori_Flag_HIO_c {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1[3];
    /* 0x4 */ be<f32> m08;
    /* 0x8 */ be<s16> m0C;
    /* 0xA */ u8 _A[2];
    /* 0xC */ be<u32> __vtbl;
};
WWHD_SIZE(daTori_Flag_HIO_c, 0x10);

struct daTori_Flag_c : fopAc_ac_c {
    void set_mtx();
    BOOL CreateHeap();
    cPhs_State CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhsTrflag; /* GameCube 0x290 */
    /* 0x3B4 */ request_of_phase_process_class mPhsCloth;
    /* 0x3BC */ gptr<J3DModel> mpModel;
    /* 0x3C0 */ gptr<ToriFlagClothHD> mpCloth;
    /* 0x3C4 */ cXyz mWindvec;
    /* 0x3D0 */ dKy_tevstr_c mClothTevStr;
    /* 0x598 */ dCcD_Stts mStts;
    /* 0x5D4 */ dCcD_Cyl mCyl;
    /* 0x704 */ dCcD_Cyl mCyl2;
};
WWHD_OFFSET(daTori_Flag_c, mpCloth, 0x3C0);
WWHD_OFFSET(daTori_Flag_c, mClothTevStr, 0x3D0);
WWHD_OFFSET(daTori_Flag_c, mStts, 0x598);
WWHD_OFFSET(daTori_Flag_c, mCyl2, 0x704);
WWHD_SIZE(daTori_Flag_c, 0x834);

WWHD_SIZE(ToriFlagClothVtableHD, 0x48);
WWHD_SIZE(ToriFlagClothHD, 0x1C0);
WWHD_OFFSET(ToriFlagClothHD, mSpring, 0x1A0);
WWHD_OFFSET(ToriFlagClothHD, mRotateY, 0x1BE);
