/* Weather tag 6, WWHD actor members +0x11C from GameCube. */
#pragma once
#include "gc_api/gc_api.h"

struct kytag06_class : fopAc_ac_c {
    kytag06_class();
    enum : u32 { __vt = 0x10013C04 };
    /* 0x3AC */ be<s32> field_0x290;
    /* 0x3B0 */ be<f32> field_0x294;
};
WWHD_SIZE(kytag06_class, 0x3B4);

// HD: private partial views; padding preserves verified member offsets.
struct kytag06_save_c {
    u8 _0[0x44];
    be<f32> mTime;
    be<u16> mDate;
    u8 _4A[0x8A];
    u8 mCollect;
};
WWHD_OFFSET(kytag06_save_c, mTime, 0x44);
WWHD_OFFSET(kytag06_save_c, mDate, 0x48);
WWHD_OFFSET(kytag06_save_c, mCollect, 0xD4);
struct kytag06_env_c {
    u8 _0[0xA40];
    be<s32> mRainCount;
    u8 _A44[0x70];
    struct { be<s32> mMode; } mThunderEff;
};
WWHD_OFFSET(kytag06_env_c, mRainCount, 0xA40);
WWHD_OFFSET(kytag06_env_c, mThunderEff, 0xAB4);
enum { dSymbol_NAYRU_e = 0 };
void dKy_change_colpat(u8);
void dKyw_rain_set(s32);
f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);
BOOL kytag06_isSymbol(void*, u8);
BOOL kytag06_startCheckOld(void*, const char*);
