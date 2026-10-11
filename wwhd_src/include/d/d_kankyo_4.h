// Per-frame environment lighting. HD packed colors and light preset additions retain their verified layouts.
#pragma once
#include "d/d_kankyo.h"
namespace d_kankyo_cpp {
enum DICE_MODE {
    DICE_MODE_SUNNY_e,
    DICE_MODE_CLOUDY_e,
    DICE_MODE_RAIN_LIGHT_e,
    DICE_MODE_RAIN_HEAVY_e,
    DICE_MODE_THUNDER_LIGHT_e,
    DICE_MODE_THUNDER_HEAVY_e,
    DICE_MODE_DONE_e = 0xFF
};
enum DICE_STATE { DICE_STATE_RESET_e, DICE_STATE_INIT_e, DICE_STATE_EXEC_e, DICE_STATE_NEXT_e };

struct FrameColors {
    GXColor mActorC0, mActorK0, mBG0_C0, mBG0_K0, mBG1_C0, mBG1_K0, mBG2_C0, mBG2_K0, mBG3_C0, mBG3_K0;
};
WWHD_SIZE(FrameColors, 0x28);
struct dScnKy_env_light_c : KankyoEnvLight {
    be<f32> &monotoneBlend() { return field_0x10e8; }
    be<f32> &skyBrightness() { return field_0x103c; }
    FrameColors &colors() { return *reinterpret_cast<FrameColors *>(&mHdField_0xb64); }
    void drawKankyo();
    void setDaytime();
    void CalcTevColor();
    void Sndpos();
    void Eflight_flush_proc();
    void SetSchbit();
    void exeKankyo();
};
WWHD_SIZE(dScnKy_env_light_c, 0x120C);
struct FramePlay {
    u8 prefix[0x5134];
    char mStageName[8];
    u8 reserved[3];
    be<s8> mStageLayer;
    u8 padding[0x5292 - 0x5140];
    be<u8> mEventRunning;
    u8 rest[0x5AF8 - 0x5293];
    gptr<struct FrameCamera> mpCamera;
    u8 more[0x5B08 - 0x5AFC];
    be<f32> mLightOverride;
    u8 tail[0x5B2C - 0x5B0C];
    gptr<fopAc_ac_c> mpPlayer;
};
WWHD_SIZE(FramePlay, 0x5B30);
struct FrameCamera {
    u8 prefix[0xDC];
    cXyz mEye, mCenter;
};
WWHD_SIZE(FrameCamera, 0xF4);
struct FrameSave {
    u8 prefix[0x44];
    be<f32> mTime;
    be<u16> mDate;
    u8 rest[0x71 - 0x4A];
    u8 mItems[1];
    u8 tail[0x1178 - 0x72];
    dSv_event_c mTmp;
};
WWHD_SIZE(FrameSave, 0x1278);
WWHD_OFFSET(FrameSave, mTmp, 0x1178);
struct FrameSavePointer {
    gptr<FrameSave> value;
};
WWHD_SIZE(FrameSavePointer, 4);
struct FrameString {
    be<u32> str, vtbl;
};
WWHD_SIZE(FrameString, 8);
} // namespace d_kankyo_cpp
d_kankyo_cpp::dScnKy_env_light_c *frameEnv();
d_kankyo_cpp::FramePlay *framePlay();
f32 frameAddCalc(be<f32> *, f32, f32, f32, f32);
void frameReportWarning(const char *);
void frameGetVector(cXyz *, cXyz *, cXyz *);
f32 frameSqrt(f32);
s16 frameAtan2s(f32, f32);
BOOL frameIsGetItem(void *, u32, u32);
void mDoAud_setHour(s32);
void mDoAud_setMinute(s32);
void mDoAud_setWeekday(s32);
void dKyw_wether_proc();
void frameApplyLightPresets(void *, void *, void *, f32);
void frameCalcMonotone();
BOOL frameChaseF(be<f32> *, f32, f32);
void frameAssureTermination(d_kankyo_cpp::FrameString *);
f32 frameRandom(f32);
void dKyw_wind_set();
void dKyw_rain_set(s32);
void frameAssert(const char *, s32, const char *);

namespace d_kankyo_cpp {
struct FrameFloat {
    be<f32> value;
};
WWHD_SIZE(FrameFloat, 4);
struct FrameByte {
    be<u8> value;
};
WWHD_SIZE(FrameByte, 1);
struct FrameStringVtable {
    u8 prefix[0x14];
    gabi::gfn resolve;
};
WWHD_SIZE(FrameStringVtable, 0x18);
struct FrameStageInfo {
    u8 prefix[12];
    union {
        be<u32> mParameters;
        struct {
            u8 bytes[3];
            be<u8> mScheduleSeconds;
        };
    };
    be<u32> mSchedule;
};
WWHD_SIZE(FrameStageInfo, 0x14);
struct FrameFlushColor {
    be<u8> frame, r, g, b;
};
WWHD_SIZE(FrameFlushColor, 4);
struct FrameFlushTable {
    FrameFlushColor values[4];
};
WWHD_SIZE(FrameFlushTable, 16);
struct FrameSinCos {
    be<f32> sine, cosine;
};
WWHD_SIZE(FrameSinCos, 8);
struct FrameSinCosTable {
    FrameSinCos values[8192];
};
WWHD_SIZE(FrameSinCosTable, 0x10000);
} // namespace d_kankyo_cpp
BOOL frameIsEventBit(void *, u32);
namespace d_kankyo_cpp {
struct FramePresets {
    u8 prefix[0x30];
    gptr<void> mpDay, mpNight, mpOverride, mpMonotoneA, mpMonotone, mpMonotoneB, mpEvent;
};
WWHD_SIZE(FramePresets, 0x4C);
struct FramePresetPointer {
    gptr<FramePresets> value;
};
WWHD_SIZE(FramePresetPointer, 4);
struct FrameSceneLights {
    u8 prefix[0x1020];
    be<u32> mSetCount;
    gptr<gptr<void>> mpSets;
};
WWHD_SIZE(FrameSceneLights, 0x1028);
struct FrameScenePointer {
    gptr<FrameSceneLights> value;
};
WWHD_SIZE(FrameScenePointer, 4);
struct FrameHdOptions {
    be<s16> mLevel, mDirection;
    u8 padding[4];
    be<u8> mOverride, mMonotone;
};
WWHD_SIZE(FrameHdOptions, 10);
struct FrameWord {
    be<u32> value;
};
WWHD_SIZE(FrameWord, 4);
struct FrameEventName {
    char value[16];
};
WWHD_SIZE(FrameEventName, 16);
struct FrameWeatherChoices {
    be<u8> values[8];
};
WWHD_SIZE(FrameWeatherChoices, 8);
struct FrameWeatherModes {
    be<u8> values[4];
};
WWHD_SIZE(FrameWeatherModes, 4);
struct FrameWeatherTimes {
    be<f32> values[3];
};
WWHD_SIZE(FrameWeatherTimes, 12);
struct FrameWeatherDelays {
    be<f32> values[8];
};
WWHD_SIZE(FrameWeatherDelays, 32);
struct FrameLightStatus {
    be<u32> words[0xE8 / 4];
};
WWHD_SIZE(FrameLightStatus, 0xE8);
struct FrameLightStatuses {
    FrameLightStatus values[8];
};
WWHD_SIZE(FrameLightStatuses, 0x740);
} // namespace d_kankyo_cpp

namespace d_kankyo_cpp {
WWHD_OFFSET(FrameColors, mBG3_K0, 0x24);
WWHD_OFFSET(FramePlay, mStageName, 0x5134);
WWHD_OFFSET(FramePlay, mStageLayer, 0x513F);
WWHD_OFFSET(FramePlay, mEventRunning, 0x5292);
WWHD_OFFSET(FramePlay, mpCamera, 0x5AF8);
WWHD_OFFSET(FramePlay, mLightOverride, 0x5B08);
WWHD_OFFSET(FramePlay, mpPlayer, 0x5B2C);
WWHD_OFFSET(FrameCamera, mEye, 0xDC);
WWHD_OFFSET(FrameCamera, mCenter, 0xE8);
WWHD_OFFSET(FrameSave, mTime, 0x44);
WWHD_OFFSET(FrameSave, mDate, 0x48);
WWHD_OFFSET(FrameSave, mItems, 0x71);
WWHD_OFFSET(FrameString, vtbl, 4);
WWHD_OFFSET(FrameStringVtable, resolve, 0x14);
WWHD_OFFSET(FrameStageInfo, mParameters, 0xC);
WWHD_OFFSET(FrameStageInfo, mScheduleSeconds, 0xF);
WWHD_OFFSET(FrameStageInfo, mSchedule, 0x10);
WWHD_OFFSET(FramePresets, mpDay, 0x30);
WWHD_OFFSET(FramePresets, mpNight, 0x34);
WWHD_OFFSET(FramePresets, mpEvent, 0x48);
WWHD_OFFSET(FrameSceneLights, mSetCount, 0x1020);
WWHD_OFFSET(FrameSceneLights, mpSets, 0x1024);
WWHD_OFFSET(FrameHdOptions, mOverride, 8);
WWHD_OFFSET(FrameHdOptions, mMonotone, 9);
} // namespace d_kankyo_cpp
