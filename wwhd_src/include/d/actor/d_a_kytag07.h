#pragma once
#include "gc_api/gc_api.h"
struct kytag07_class : fopAc_ac_c {
    kytag07_class();
    enum : u32 { __vt = 0x10013C6C };
};
WWHD_SIZE(kytag07_class, 0x3AC);
// HD: private views expose only this tag's weather and saved-time fields.
struct kytag07_env_c {
    u8 padding[0xA40];
    be<s32> mRainCount;
    u8 paddingRain[0xA7D - 0xA44];
    be<u8> mMoyaMode;
    u8 paddingMoya[2];
    be<s32> mMoyaCount;
    u8 paddingThunder[0xAB4 - 0xA84];
    struct { be<s32> mMode; } mThunderEff;
    u8 paddingTact[0x10A1 - 0xAB8];
    be<u8> mbDayNightTactStop;
};
WWHD_OFFSET(kytag07_env_c, mMoyaMode, 0xA7D);
WWHD_OFFSET(kytag07_env_c, mMoyaCount, 0xA80);
WWHD_OFFSET(kytag07_env_c, mThunderEff, 0xAB4);
WWHD_OFFSET(kytag07_env_c, mbDayNightTactStop, 0x10A1);
struct kytag07_save_c {
    u8 padding[0x44];
    be<f32> mTime;
    be<u16> mDate;
};
WWHD_OFFSET(kytag07_save_c, mTime, 0x44);
WWHD_OFFSET(kytag07_save_c, mDate, 0x48);
inline kytag07_env_c* kytag07_env() { return (kytag07_env_c*)dKy_getEnvlight(); }
inline kytag07_save_c* kytag07_save(u32 address) { return gabi::at<kytag07_save_c>(address); }
void dKy_change_colpat(u8);
void dKy_custom_colset(u8, u8, f32);
BOOL kytag07_startCheckOld(void*, const char*);
void kytag07_actor_dt(void*, s32);
struct kytag07_play_c {
    u8 padding[0x5134];
    char mStartStageName[8];
};
enum : u32 { kytag07_SafeStringVtable = 0x10013C54 };
// HD: SafeString invokes assureTermination twice on the literal and once on the stage.
static inline void SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static inline bool dComIfGp_checkStageName(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(name);
    a->__vtbl = kytag07_SafeStringVtable;
    auto* play = gabi::at<kytag07_play_c>(dComIfGp_ea());
    u32 stage = gabi::ea(play->mStartStageName);
    gabi::Local<SafeString> b;
    b->__vtbl = kytag07_SafeStringVtable;
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


