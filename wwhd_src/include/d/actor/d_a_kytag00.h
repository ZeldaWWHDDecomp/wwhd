/* Weather tag 00. GameCube actor members are +0x11C in HD (base size 0x3AC). */
#pragma once
#include "gc_api/gc_api.h"
struct kytag00_class : fopAc_ac_c {
    enum : u32 { __vt = 0x100139D8 };
    /* 0x3AC */ be<u32> field_0x290;
    /* 0x3B0 */ be<u8> mbEfSet;
    /* 0x3B1 */ be<u8> mbPselSet;
    /* 0x3B2 */ be<u8> field_0x296;
    /* 0x3B3 */ be<u8> mPselIdx;
    /* 0x3B4 */ be<u8> mEfMode;
    /* 0x3B5 */ u8 _3B5[3];
    /* 0x3B8 */ be<s32> mThickness;
    /* 0x3BC */ be<s32> mInnerFadeY;
    /* 0x3C0 */ be<f32> mInnerRadius;
    /* 0x3C4 */ be<f32> mOuterRadius;
    /* 0x3C8 */ be<f32> mTarget;
    /* 0x3CC */ be<u8> mSwitchNo;
    /* 0x3CD */ be<u8> mbInvert;
    /* 0x3CE */ be<u8> mMode;
    /* 0x3CF */ u8 _3CF;
};
WWHD_SIZE(kytag00_class, 0x3D0);

WWHD_OFFSET(kytag00_class, field_0x290, 0x3AC);
WWHD_OFFSET(kytag00_class, mbEfSet, 0x3B0);
WWHD_OFFSET(kytag00_class, mbPselSet, 0x3B1);
WWHD_OFFSET(kytag00_class, field_0x296, 0x3B2);
WWHD_OFFSET(kytag00_class, mPselIdx, 0x3B3);
WWHD_OFFSET(kytag00_class, mEfMode, 0x3B4);
WWHD_OFFSET(kytag00_class, _3B5, 0x3B5);
WWHD_OFFSET(kytag00_class, mThickness, 0x3B8);
WWHD_OFFSET(kytag00_class, mInnerFadeY, 0x3BC);
WWHD_OFFSET(kytag00_class, mInnerRadius, 0x3C0);
WWHD_OFFSET(kytag00_class, mOuterRadius, 0x3C4);
WWHD_OFFSET(kytag00_class, mTarget, 0x3C8);
WWHD_OFFSET(kytag00_class, mSwitchNo, 0x3CC);
WWHD_OFFSET(kytag00_class, mbInvert, 0x3CD);
WWHD_OFFSET(kytag00_class, mMode, 0x3CE);
WWHD_OFFSET(kytag00_class, _3CF, 0x3CF);

// HD: private partial environment and camera views; shared environment is opaque.
struct kytag00_env_c {
    u8 padding0[0xA40];
    /* 0xA40 */ be<s32> mRainCount;
    u8 paddingA44[0x8];
    /* 0xA4C */ be<s32> mSnowCount;
    u8 paddingA50[0x24];
    /* 0xA74 */ be<s32> mHousiCount;
    u8 paddingA78[0x5];
    /* 0xA7D */ be<u8> mMoyaMode;
    u8 paddingA7E[0x2];
    /* 0xA80 */ be<s32> mMoyaCount;
    u8 paddingA84[0x30];
    /* 0xAB4 */ struct {
        be<s32> mMode;
    } mThunderEff;
    u8 paddingAB8[0x514];
    /* 0xFCC */ be<f32> mColPatBlendGather;
    u8 paddingFD0[0x9C];
    /* 0x106C */ be<s32> mRainCountOrig;
    u8 padding1070[0x1E];
    /* 0x108E */ be<u8> mColpatPrevGather;
    /* 0x108F */ be<u8> mColpatCurrGather;
    /* 0x1090 */ be<u8> mEnvrIdxPrev;
    /* 0x1091 */ be<u8> mEnvrIdxCurr;
    /* 0x1092 */ be<u8> mColpatWeather;
    u8 padding1093[0x5];
    /* 0x1098 */ be<u8> mColPatModeGather;
};
WWHD_OFFSET(kytag00_env_c, mRainCount, 0xA40);
WWHD_OFFSET(kytag00_env_c, mSnowCount, 0xA4C);
WWHD_OFFSET(kytag00_env_c, mHousiCount, 0xA74);
WWHD_OFFSET(kytag00_env_c, mMoyaMode, 0xA7D);
WWHD_OFFSET(kytag00_env_c, mMoyaCount, 0xA80);
WWHD_OFFSET(kytag00_env_c, mThunderEff, 0xAB4);
WWHD_OFFSET(kytag00_env_c, mColPatBlendGather, 0xFCC);
WWHD_OFFSET(kytag00_env_c, mRainCountOrig, 0x106C);
WWHD_OFFSET(kytag00_env_c, mColpatPrevGather, 0x108E);
WWHD_OFFSET(kytag00_env_c, mColpatCurrGather, 0x108F);
WWHD_OFFSET(kytag00_env_c, mEnvrIdxPrev, 0x1090);
WWHD_OFFSET(kytag00_env_c, mEnvrIdxCurr, 0x1091);
WWHD_OFFSET(kytag00_env_c, mColpatWeather, 0x1092);
WWHD_OFFSET(kytag00_env_c, mColPatModeGather, 0x1098);
WWHD_SIZE(kytag00_env_c, 0x1099);
struct kytag00_camera_c {
    u8 padding[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(kytag00_camera_c, mEye, 0xDC);
WWHD_SIZE(kytag00_camera_c, 0xE8);
static inline kytag00_camera_c *kytag00_getCamera() {
    return reinterpret_cast<kytag00_camera_c *>(dComIfGp_getCamera(0));
}
static inline kytag00_env_c *kytag00_env() {
    return reinterpret_cast<kytag00_env_c *>(dKy_getEnvlight());
}
f32 PSVECSquareDistance(const cXyz *, const cXyz *);
BOOL dKy_checkEventNightStop();
f32 cLib_addCalc(be<f32> *, f32, f32, f32, f32);
void kytag00_actorDestroy(void *, s32);

// Encoded mEfMode values; GameCube names only the field, not these weather presets.
enum Kytag00Effect {
    KYTAG00_RAIN = 1,
    KYTAG00_SNOW = 2,
    KYTAG00_FOG = 3,
    KYTAG00_FOG_1 = 4,
    KYTAG00_FOG_2 = 5,
    KYTAG00_HOUSI = 6,
    KYTAG00_THUNDER = 7,
    KYTAG00_THUNDER_RAIN = 8,
    KYTAG00_THUNDER_RAIN_FOG = 9,
    KYTAG00_FOG_3 = 10,
    KYTAG00_FOG_4 = 11
};
