/* Weather tag 5. HD actor base is 0x3AC; tag fields move by 0x11C. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/SSystem/c_phase.h"

struct kytag05_class : fopAc_ac_c {
    /* 0x3AC */ be<u8> mIndex;
    u8 mPadding[3];
    /* 0x3B0 */ be<s32> mTimer;
    /* 0x3B4 */ be<s32> mUnknownParam;
    enum : u32 { VTABLE = 0x10013B94 };
};
WWHD_OFFSET(kytag05_class, mIndex, 0x3AC);
WWHD_OFFSET(kytag05_class, mTimer, 0x3B0);
WWHD_OFFSET(kytag05_class, mUnknownParam, 0x3B4);
WWHD_SIZE(kytag05_class, 0x3B8);

// Private views of fields used by this tag, pending the shared environment/camera layouts.
struct Kytag05EnvLight {
    u8 mEnvironment[0xA2D];
    struct { be<u8> mEvtWindSet; } mWind;
    u8 mWeather[0xA4C - 0xA2E];
    be<s32> mSnowCount;
    u8 mWeatherEffects[0xA7D - 0xA50];
    be<u8> mMoyaMode;
    u8 mMoyaPadding[2];
    be<s32> mMoyaCount;
};
WWHD_OFFSET(Kytag05EnvLight, mWind, 0xA2D);
WWHD_OFFSET(Kytag05EnvLight, mSnowCount, 0xA4C);
WWHD_OFFSET(Kytag05EnvLight, mMoyaMode, 0xA7D);
WWHD_OFFSET(Kytag05EnvLight, mMoyaCount, 0xA80);
WWHD_SIZE(Kytag05EnvLight, 0xA84);
struct Kytag05Camera {
    struct {
        u8 mViewState[0xDC];
        struct { cXyz mEye; } mLookat;
    } view;
};
WWHD_OFFSET(Kytag05Camera, view.mLookat.mEye, 0xDC);
WWHD_SIZE(Kytag05Camera, 0xE8);
struct Kytag05PlayView {
    u8 mPlayState[0x5AF8];
    gptr<Kytag05Camera> mpCamera;
};
WWHD_OFFSET(Kytag05PlayView, mpCamera, 0x5AF8);
WWHD_SIZE(Kytag05PlayView, 0x5AFC);
inline Kytag05Camera* kytag05_getCamera(int) {
    return reinterpret_cast<Kytag05PlayView*>(dComIfGp_get())->mpCamera;
}
inline Kytag05EnvLight& kytag05_getEnvLight() {
    return *reinterpret_cast<Kytag05EnvLight*>(dKy_getEnvlight());
}
#undef g_env_light
#define g_env_light kytag05_getEnvLight()

void dKyw_evt_wind_set(s16, s16);
void dKyw_evt_wind_set_go();
void dKy_custom_colset(u8, u8, f32);
// HD: the three-register sound entry omits the GameCube reverb argument.
void mDoAud_seStartHD(u32, cXyz*, u32);
BOOL dEvent_manager_startCheckOld(dEvent_manager_c*, const char*);
inline BOOL kytag05_startCheckOld(const char* name) {
    return dEvent_manager_startCheckOld(dComIfGp_getPEvtManager(), name);
}
enum { JA_SE_ATM_WIND_VAR = 0x106A, dSymbol_DIN_e = 1 };
