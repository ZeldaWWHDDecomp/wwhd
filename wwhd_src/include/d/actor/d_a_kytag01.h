#pragma once
#include "gc_api/gc_api.h"
struct WAVE_INFO {
    cXyz mPos;
    be<f32> mOuterRadius, mInnerRadius;
};
WWHD_SIZE(WAVE_INFO, 0x14);
struct kytag01_class : fopAc_ac_c {
    kytag01_class();
    enum : u32 { __vt = 0x10013A3C };
    WAVE_INFO mWaveInfo;
};
WWHD_OFFSET(kytag01_class, mWaveInfo, 0x3AC);
WWHD_SIZE(kytag01_class, 0x3C0);
// HD: private view of the wave channel in the larger environment object.
struct kytag01_wave_chan_c {
    be<f32> mWaveSpeed, mWaveSpawnDist, mWaveSpawnRadius, mWaveScale;
    be<f32> mWaveScaleRand, mWaveCounterSpeedScale, mWaveScaleBottom, mWaveFlatInter;
    be<s16> mWaveCount;
    be<u8> mWaveReset, field_0x2f;
};
WWHD_SIZE(kytag01_wave_chan_c, 0x24);
struct kytag01_env_c {
    u8 padding[0x9D8];
    kytag01_wave_chan_c mWaveChan;
    u8 paddingWave[0xAEC - 0x9FC];
    gptr<WAVE_INFO> mpWaveInfl[10];
};
WWHD_OFFSET(kytag01_env_c, mWaveChan, 0x9D8);
WWHD_OFFSET(kytag01_env_c, mpWaveInfl, 0xAEC);
struct kytag01_play_c {
    u8 padding[0x5134];
    char mStartStageName[8];
};
enum : u32 { kytag01_SafeStringVtable = 0x10013A24 };
// HD: SafeString invokes assureTermination twice on the literal and once on the stage.
static inline void SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static inline bool dComIfGp_checkStageName(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(name);
    a->__vtbl = kytag01_SafeStringVtable;
    auto* play = gabi::at<kytag01_play_c>(dComIfGp_ea());
    u32 stage = gabi::ea(play->mStartStageName);
    gabi::Local<SafeString> b;
    b->__vtbl = kytag01_SafeStringVtable;
    b->mStringTop = stage;
    SafeString_vcall(a);
    SafeString_vcall(a);
    u32 pa = a->mStringTop;
    SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::at<be<u8>>(pa)[i];
        u8 cb = gabi::at<be<u8>>(pb)[i];
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}


inline kytag01_env_c* kytag01_env() { return (kytag01_env_c*)dKy_getEnvlight(); }
void kytag01_actor_dt(void*, s32);
